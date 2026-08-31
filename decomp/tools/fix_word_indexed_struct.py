#!/usr/bin/env python3
"""fix_word_indexed_struct.py — a struct reached through a WORD-INDEXED pointer.

    fix_word_indexed_struct.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT. `McdCache` comes out of a pool as a `void *`, Ghidra types the local
`undefined4 *`, and every field of it is then addressed by WORD INDEX:

    puVar3 = (MePoolFixedAPI.getStruct)(pMVar1);
    puVar3[0xb] = p->model2->mPadding + p->model1->mPadding;   /* padding, 44 */
    puVar3[0xd] = &p->model1->mInstance;                       /* ins1,    52 */
    puVar3[0xe] = &p->model2->mInstance;                       /* ins2,    56 */
    ...
    McdGjkFatness((void *)puVar3[0xe]);

    struct _McdCache { MeVector3 normal, location, offset;
                       MeReal fat1, fat2, padding, separation;
                       McdGeometryInstanceID ins1, ins2; }
        i386   0 12 24 36 40 44 48 52 56        sizeof 60
        LP64   0 12 24 36 40 44 48 56 64        sizeof 72

Three floats do not grow, so everything up to `separation` keeps its offset and
every one of those indices is right. The two POINTERS at the end move — 52 -> 56
and 56 -> 64 — and they are eight bytes wide, so `puVar3[0xd] = ptr` writes the
low half of a pointer into the middle of `separation`'s padding and
`puVar3[0xe]` reads it back short.

★ THIS IS THE ONSLAUGHT CRASH. `McdGjkFatness (ins = 0xfffcf878)` — the low half
of the stack address `0x7ffffffcf878` its caller had. The read-back is the
truncation, and the write is where it was lost.

THE TYPE COMES FROM THE POOL, WHICH IS EVIDENCE RATHER THAN INFERENCE:

    (MePoolFixedAPI.init)(pMVar1, 100, (int)sizeof(*(McdCache *)0), 0x10);
    puVar3 = (MePoolFixedAPI.getStruct)(pMVar1);

The allocation site says what the pool holds, and the pointer is what that pool
handed back. `m_cachedData` itself is declared `void *` in the oracle — the
header only says, in a comment, that it "should point to this" — so the pool is
the only place the type is written down at all. Same chain `fix_baked_sizeof`
uses for its pool form.

THE INDEX IS RESOLVED, NOT ASSUMED. `V[k]` becomes the member of T that STARTS
at i386 byte `4*k`, taken from the same nested path map `fix_literal_offsets`
builds — so `puVar3[1]` is `normal[1]`, an array element, and an index landing
mid-member is declined rather than guessed at. At i386 the rewritten access is
the same address at the same width, so the object is unchanged; at LP64 the two
pointers land where they really are and are written whole.

SCOPE: three pools in the corpus name a type this way and one of them is walked
by word index — twelve sites in `McdGjk.c`.

⚠ RUN IT ON A COPY: it edits in place.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402
import fix_literal_offsets as flo                           # noqa: E402

HERE = kd_paths.MD
WORK = '/tmp/kd_wordstruct'

CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types',
          '-DLINUX']

POOL_INIT = re.compile(
    r'\(MePool(?:Fixed)?API\.init\)\(\s*([^,]+?)\s*,[^,]+,\s*'
    r'\(int\)sizeof\(\*\(([A-Za-z_]\w*) \*\)0\)')
GET = re.compile(r'(\w+) = \(MePool(?:Fixed)?API\.getStruct\)\(\s*([^)]+?)\s*\)')


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def compiles_identically(fn, text, build, inc):
    """Does this source still produce the baseline .o, byte for byte?

    ⚠ The file has to keep its NAME: gcc records the source basename in an
    STT_FILE symbol, so identical text from a scratch name is a different
    object and reads exactly like a codegen change."""
    ref = os.path.join(build, fn[:-2] + '.o')
    if not os.path.exists(ref):
        return False
    d = os.path.join(WORK, 'ident')
    os.makedirs(d, exist_ok=True)
    src = os.path.join(d, fn)
    open(src, 'w').write(text)
    obj = os.path.join(d, fn[:-2] + '.probe.o')
    if subprocess.run(['gcc'] + CFLAGS + ['-I' + os.path.join(HERE, 'include')]
                      + includes(inc) + ['-c', '-o', obj, src],
                      capture_output=True).returncode:
        return False
    return open(ref, 'rb').read() == open(obj, 'rb').read()


def field_paths(T, inc, cache):
    """byte offset -> member path, from BOTH sources and for a reason.

    ⚠ `flo.offsets_of` expands arrays — `normal[1]` at byte 4 — but its member
    regex cannot parse a COMMA-SEPARATED declarator, and `McdCache` ends with
    `McdGeometryInstanceID ins1, ins2;`. So the two fields that actually move at
    LP64 are exactly the two it cannot see. The type database has them but only
    at top level, with no array expansion. Neither alone can resolve this struct;
    the union can, and the self-check below is what proves it."""
    out = dict(flo.offsets_of(T, inc, cache))
    tf = os.path.join(HERE, 'include', 'kd_types_fields.json')
    if os.path.exists(tf):
        import json
        db = json.load(open(tf))
        d = db.get(T) or db.get('_' + T) or {}
        for off, name in d.items():
            out.setdefault(int(off), name)
    return out


_SZ = re.compile(r'char \(\*\)\[(\d+)\]')


def probe(expr, inc, cache, bits):
    key = (expr, bits)
    if key in cache:
        return cache[key]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p.c')
    head = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
            '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))
    open(src, 'w').write(head + 'char kd_probe[%s];\nint kd_force = &kd_probe;\n' % expr)
    r = subprocess.run(['gcc', bits, '-DLINUX'] + includes(inc)
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    m = _SZ.search(r.stderr)
    cache[key] = int(m.group(1)) if m else None
    return cache[key]


def actually_moves(T, path, k, inc, cache):
    """Does this member's ADDRESS or WIDTH change at LP64?

    ★ ONLY REWRITE WHAT IS BROKEN. `McdCache`'s leading floats keep their offsets
    and their width at both widths, so re-spelling them is churn — and MEASURABLY
    worse than churn: `puVar3[9] = fVar5` through `undefined4` is an integer
    store and `->fat1 = fVar5` is a float one, so five of the twelve sites were
    not byte-identical at i386. The two that move, `ins1` and `ins2`, are the two
    the crash is about. This is the same rule `fix_index_layout` applies: measure
    both, rewrite only where they disagree."""
    off = '((char *)&((%s *)0)->%s - (char *)0)' % (T, path)
    sz = 'sizeof(((%s *)0)->%s)' % (T, path)
    a32, a64 = probe(off, inc, cache, '-m32'), probe(off, inc, cache, '-m64')
    w32, w64 = probe(sz, inc, cache, '-m32'), probe(sz, inc, cache, '-m64')
    if None in (a32, a64, w32, w64):
        return False
    if a32 != k * 4:
        return False                      # the path does not start here at i386
    return a64 != a32 or w64 != w32


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')
    cache = {}

    # ---- THE SELF-CHECK. An empty path map declines every site and prints a
    # clean zero that reads exactly like "there was nothing to do".
    probe = field_paths('McdCache', inc, cache)
    if probe.get(52) != 'ins1':
        sys.exit('fix_word_indexed_struct: SELF-CHECK FAILED — McdCache byte 52 '
                 'read as %r, want ins1.' % probe.get(52))

    done = declined = 0
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        pools = {m.group(1).strip(): m.group(2) for m in POOL_INIT.finditer(text)}
        if not pools:
            continue
        out = text
        for m in GET.finditer(text):
            v, pe = m.group(1), m.group(2).strip()
            T = pools.get(pe)
            if not T:
                continue
            paths = field_paths(T, inc, cache)
            if not paths:
                continue
            sites = list(re.finditer(
                r'(?<![\w.])%s\[(0x[0-9a-fA-F]+|\d+)\]' % re.escape(v), out))
            if not sites:
                continue
            cand, n, bad = out, 0, []
            for s in reversed(sites):
                k = int(s.group(1), 0)
                p = paths.get(k * 4)
                if not p:
                    bad.append(k)
                    continue
                if not actually_moves(T, p, k, inc, cache):
                    continue              # right at both widths — leave it alone
                cand = (cand[:s.start()] + '((%s *)%s)->%s' % (T, v, p)
                        + cand[s.end():])
                n += 1
            if not n:
                continue
            if compiles_identically(fn, cand, build, inc):
                out = cand
                done += n
                notes.append('%-22s %-10s -> %s  %d site(s)%s'
                             % (fn, v, T, n,
                                ', %d index(es) not a member start: %s'
                                % (len(bad), sorted(bad)) if bad else ''))
            else:
                declined += 1
                notes.append('%-22s %-10s -> %s: not byte-identical at i386 — '
                             'declined' % (fn, v, T))
        if out != text:
            open(path, 'w').write(out)

    for x in notes:
        print('  ' + x)
    print('fix_word_indexed_struct: %d site(s) rewritten, %d declined'
          % (done, declined))


if __name__ == '__main__':
    main()
