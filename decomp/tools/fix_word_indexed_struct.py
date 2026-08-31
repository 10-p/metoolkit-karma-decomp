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

# ★ THE SECOND ANCHOR: a word pointer derived from a field declared `E *`.
# `IxBoxTriList` walks the triangle list with
#     puVar31 = (undefined4 *)((kd_iptr)&(<McdTriangleList::list>)->mRefCtAndID + cursor);
#     pfVar26 = (float *)*puVar31;      /* vertices[0] */
#     pfVar26 = (float *)puVar31[1];    /* vertices[1] -- byte 4 here, EIGHT there */
# `McdUserTriangle` is `{MeVector3 *vertices[3]; MeVector3 *normal; ...}`, so the
# four words are four POINTERS. The cursor happens to be right at both widths
# (`&p[1].next` is 24 here and 48 there, and so is sizeof) — only the field
# indices are wrong, which is why nothing about the stride reveals it.
#
# ⚠ THIS IS THE RAGDOLL PATH AND THE OFFLINE SCENES CANNOT REACH IT.
# `scene_ragdoll` is capsules on a PLANE; there is no triangle list in it. Only a
# body hitting real world geometry runs this.
FIELD_PTR = re.compile(
    r'(?P<v>\w+) = \((?:undefined4|int|uint) \*\)\(\(kd_iptr\)&'
    r'[^;\n]*?\(\((?:struct )?(?P<T>\w+) \*\)0\)->(?P<F>\w+)\b')


# ★ THE THIRD ANCHOR: a DIRECT member chain, already typed by an earlier pass.
# `IxCylinderTriList` reaches the same triangle array as `IxBoxTriList` but says
# so in C rather than through an offsetof:
#     puVar31 = (undefined4 *)((kd_iptr)triList->list->vertices + cursor);
# Matching only the offsetof spelling missed it, and that file is where the
# ANDROID arm64 tombstone lands — `McdCylinderTriangleListIntersect+1796`, fault
# addr 0x7a, under KHandleCollisions/KUpdateContacts/KTickLevelKarma.
EXPR_PTR = re.compile(
    r'(?P<v>\w+) = \((?:undefined4|int|uint) \*\)\(\(kd_iptr\)'
    r'(?P<expr>[A-Za-z_][\w\[\]>.-]*?)\s*\+')


def resolve_expr_owner(expr, text, root):
    """Follow `triList->list->vertices` to the struct that DECLARES the last hop.

    The pointer addresses that struct, so the struct is the element type — but
    only if the member it names starts at byte 0, because anything else means the
    cursor is not element-aligned and the word indices would be measured from the
    wrong place. Every hop is read from the ORACLE; an unresolvable hop declines."""
    bodies = flo.struct_bodies(os.path.join(root, 'include'))
    parts = [x for x in re.split(r'->|\.', expr) if x]
    if len(parts) < 2:
        return None
    base = re.sub(r'\[[^\]]*\]', '', parts[0])
    m = re.search(r'(?<![\w])([A-Za-z_]\w*)\s*\*\s*%s\b' % re.escape(base), text)
    if not m:
        return None
    tag = m.group(1)
    for hop in parts[1:]:
        hop = re.sub(r'\[[^\]]*\]', '', hop)
        body = bodies.get(tag) or bodies.get('_' + tag)
        if not body:
            return None
        mm = re.search(r'([A-Za-z_]\w*)\s*\*?\s*%s\s*[;,\[]' % re.escape(hop),
                       re.sub(r'/\*.*?\*/', ' ', body, flags=re.S))
        if not mm:
            return None
        owner, tag = tag, mm.group(1)
    return owner        # the struct declaring the final hop


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


EMCC = os.path.join(os.path.expanduser('~'), 'emsdk', 'upstream', 'emscripten', 'emcc')


def wasm_unchanged(fn, before, after, inc):
    """Does this rewrite leave the WASM32 object identical too?

    ⚠⚠ THE i386 ACCEPTANCE TEST CANNOT SPEAK FOR wasm32, AND THIS IS THE PASS
    THAT PROVED IT. i386 is compiled by GCC and wasm32 by CLANG, so "byte
    identical" is a statement about one code generator. The triangle-list repair
    below is byte-identical under gcc at -m32 and NOT under emcc — same address,
    same width, different instruction selection. The web artifact ships, so a
    repair that only gcc certifies is not certified.

    There is no wasm baseline to compare against, so this compares the candidate
    against the source it replaces — a no-op check that needs nothing but emcc.
    If emcc is missing the site is DECLINED, not assumed."""
    if not os.path.exists(EMCC):
        return False
    d = os.path.join(WORK, 'wasm')
    os.makedirs(d, exist_ok=True)
    objs = []
    for tag, txt in (('a', before), ('b', after)):
        src = os.path.join(d, tag + '_' + fn)
        open(src, 'w').write(txt)
        obj = os.path.join(d, tag + '.o')
        r = subprocess.run(
            [EMCC, '-O2', '-g0', '-c', '-std=gnu99', '-fno-strict-aliasing', '-w',
             '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX',
             '-I' + os.path.join(HERE, 'include')] + includes(inc)
            + ['-o', obj, src], capture_output=True)
        if r.returncode:
            return False
        objs.append(open(obj, 'rb').read())
    return objs[0] == objs[1]


def field_paths(T, inc, cache):
    """byte offset -> member path, from BOTH sources and for a reason.

    ⚠ `flo.offsets_of` expands arrays — `normal[1]` at byte 4 — but its member
    regex cannot parse a COMMA-SEPARATED declarator, and `McdCache` ends with
    `McdGeometryInstanceID ins1, ins2;`. So the two fields that actually move at
    LP64 are exactly the two it cannot see. The type database has them but only
    at top level, with no array expansion. Neither alone can resolve this struct;
    the union can, and the self-check below is what proves it."""
    # ⚠ ASK FOR BOTH SPELLINGS. `flo` keys by struct TAG (`_McdUserTriangle`) and
    # the code names the TYPEDEF (`McdUserTriangle`); asking for the wrong one
    # returns an empty map, the type-database overlay then supplies only
    # TOP-LEVEL members, and `vertices` comes back as the array NAME instead of
    # `vertices[0]`. That decays to `MeVector3 **` and is not what the index
    # meant. The array expansion is the whole reason flo is consulted first.
    out = dict(flo.offsets_of(T, inc, cache) or flo.offsets_of('_' + T, inc, cache))
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


def pointee_of(T, F, root):
    """`McdTriangleList::list` is declared `McdUserTriangle *` — the element type
    of anything reached through it, read from the ORACLE. Returns None unless the
    member is a pointer to a struct whose size CHANGES at LP64, because a size
    that does not change is not a defect."""
    import fix_literal_offsets as flo
    inc = os.path.join(root, 'include')
    bodies = flo.struct_bodies(inc)
    body = bodies.get(T) or bodies.get('_' + T)
    if not body:
        return None
    m = re.search(r'([A-Za-z_]\w*)\s*\*\s*%s\s*[;,]' % re.escape(F),
                  re.sub(r'/\*.*?\*/', ' ', body, flags=re.S))
    # ⚠ BOTH SPELLINGS. `typedef struct _McdUserTriangle {...} McdUserTriangle;`
    # is keyed by its TAG, and the member is declared with the TYPEDEF name, so a
    # membership test on one of them silently returns nothing.
    if not m or not (m.group(1) in bodies or '_' + m.group(1) in bodies):
        return None
    return m.group(1)


def slot_form(E, pth, k, v, inc, cache, addr):
    """`((kd_uptr *)v)[k]` — the SAME byte at the SAME width as `((E *)v)->pth`,
    and the only spelling of it clang schedules identically.

    ⚠⚠ THE MEMBER SPELLING IS NOT ALWAYS AVAILABLE, AND `IxBoxTriList` IS WHY.
    `((McdUserTriangle *)p)->vertices[1]` is byte-identical under gcc at -m32 and
    NOT under emcc, so the whole variable declined and the four truncated
    triangle reads stayed in the shipped source. Bisected site by site, the three
    that break it are exactly the three where the rewrite turns an INTEGER load
    into a POINTER load: `p[1]` yields `undefined4` and is converted to `float *`,
    while `->vertices[1]` yields `MeVector3 *` directly. Same address, same
    width, same result — and clang tees one extra local, three bytes of wasm, on
    a statement four lines away. The address is identical at wasm32 either way;
    what differs is scheduling.

    ★ SO KEEP THE VALUE AN INTEGER OF POINTER WIDTH. `((kd_uptr *)p)[1]` is
    `unsigned int` at i386 and wasm32 — exactly what `undefined4` was — so the
    conversion that follows is the same int-to-pointer conversion it always was,
    and nothing downstream moves. Measured: all four sites byte-identical under
    BOTH compilers, where every member spelling tried (`->vertices[1]`,
    `((MeVector3 **)p)[1]`, `*(MeVector3 **)&...`, `(kd_uptr)...->vertices[1]`)
    failed under emcc.

    THE GUARD IS THREE MEASUREMENTS, NOT A SHAPE. This spelling is only the same
    access if the member sits at word `k` at BOTH widths and is pointer-width at
    both — otherwise `[k]` in pointer units addresses something else. So:

        offset at i386 == 4k    offset at LP64 == 8k    sizeof 4 here, 8 there

    all read off the compiler. `McdUserTriangle::flags` is an `MeU32` and fails
    the width test, so it keeps the member spelling — which is correct, and which
    emcc accepts."""
    off = '((char *)&((%s *)0)->%s - (char *)0)' % (E, pth)
    sz = 'sizeof(((%s *)0)->%s)' % (E, pth)
    a32, a64 = probe(off, inc, cache, '-m32'), probe(off, inc, cache, '-m64')
    w32, w64 = probe(sz, inc, cache, '-m32'), probe(sz, inc, cache, '-m64')
    if None in (a32, a64, w32, w64):
        return None
    if a32 != k * 4 or a64 != k * 8 or (w32, w64) != (4, 8):
        return None
    return ('((kd_uptr *)%s + %d)' % (v, k)) if addr else \
           ('((kd_uptr *)%s)[%d]' % (v, k))


def rewrite_sites(out, v, E, paths, inc, cache, slot=False):
    """Rewrite every word-indexed access to `v`, returning (text, count).

    Three spellings, and missing the third left the ANDROID crash in place:
        V[k]        a field read/write        -> ((E *)V)->PATH
        *V          the same at index 0       -> ((E *)V)->PATH
        (V + k)     an ADDRESS handed onward  -> (&((E *)V)->PATH)
    `IxCylinderTriList` takes `*(__typeof__(ct.triangleData) *)(puVar31 + 4)` —
    byte 16 here and THIRTY-TWO there — and it is the only one of the six that a
    `V[k]`-shaped pattern cannot see.

    `slot=True` asks for `slot_form` wherever it is available — the same access
    written as a pointer-width WORD INDEX rather than a member name. The caller
    tries the member spelling first and falls back to this one, because the
    member name is the more legible repair and only `IxBoxTriList` needs the
    other."""
    decl_re = re.compile(r'(?m)^\s*[A-Za-z_]\w*\s*\*\s*%s\s*;' % re.escape(v))
    pat = (r'\((?<![\w.])%s \+ (?P<a>\d+)\)'
           r'|(?<![\w.])%s\[(?P<b>0x[0-9a-fA-F]+|\d+)\]'
           r'|\*(?<![\w.])%s(?![\w\[])') % ((re.escape(v),) * 3)
    sites = [st for st in re.finditer(pat, out)
             if not decl_re.match(out, out.rfind('\n', 0, st.start()) + 1)]
    n = 0
    for st in reversed(sites):
        if st.group('a') is not None:
            k, addr = int(st.group('a'), 0), True
        elif st.group('b') is not None:
            k, addr = int(st.group('b'), 0), False
        else:
            k, addr = 0, False
        pth = paths.get(k * 4)
        if not pth or not actually_moves(E, pth, k, inc, cache):
            continue
        rep = None
        if slot:
            rep = slot_form(E, pth, k, v, inc, cache, addr)
        if rep is None:
            rep = ('(&((%s *)%s)->%s)' if addr else '((%s *)%s)->%s') % (E, v, pth)
        out = out[:st.start()] + rep + out[st.end():]
        n += 1
    return out, n


def accepted(fn, out, v, E, paths, inc, cache, build):
    """The member spelling if both compilers accept it, else the word-index one.

    Returns (text, count, how) or (None, 0, None). ⚠ The order matters and is not
    a preference: `((E *)p)->field` names what the code is doing and is what
    every other file gets. The fallback exists for the one file where clang will
    not reproduce it, and taking it first would spread an opaque spelling over
    seventeen sites that do not need it."""
    for how in ('member', 'word-index'):
        cand, n = rewrite_sites(out, v, E, paths, inc, cache,
                                slot=(how == 'word-index'))
        if not n:
            return None, 0, None          # nothing to do — NOT a decline
        if compiles_identically(fn, cand, build, inc) \
                and wasm_unchanged(fn, out, cand, inc):
            return cand, n, how
    return None, n, None                  # sites found, no spelling survived


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
        out = text
        # ⚠ THE POOL GUARD USED TO SKIP THE WHOLE FILE. The second anchor does not
        # need a pool at all — `IxBoxTriList` has none — so an early `continue`
        # here silently excluded every triangle-list site in the corpus.
        for m in (GET.finditer(text) if pools else ()):
            v, pe = m.group(1), m.group(2).strip()
            T = pools.get(pe)
            if not T:
                continue
            paths = field_paths(T, inc, cache)
            if not paths:
                continue
            # ⚠ `T` HERE, NOT `E`. The pool block names the type `T` and the two
            # anchors below name it `E`; passing `E` from here does not raise —
            # Python leaks the loop variable, so it silently reused the PREVIOUS
            # file's type and quietly stopped repairing McdCache. The symptom was
            # a regression two commits later with no decline reported.
            cand, n, how = accepted(fn, out, v, T, paths, inc, cache, build)
            if not n:
                continue
            if cand is not None:
                out = cand
                done += n
                notes.append('%-22s %-10s -> %s  %d site(s) [%s]'
                             % (fn, v, T, n, how))
            else:
                declined += 1
                notes.append('%-22s %-10s -> %s: not byte-identical at i386 OR '
                             'wasm32, in either spelling — declined' % (fn, v, T))
        # ---- the second anchor: a field declared `E *`
        for m in FIELD_PTR.finditer(text):
            v, T, F = m.group('v'), m.group('T'), m.group('F')
            E = pointee_of(T, F, root)
            if not E:
                continue
            paths = field_paths(E, inc, cache)
            if not paths:
                continue
            # ⚠ THE DECLARATION LOOKS EXACTLY LIKE A DEREFERENCE.
            # `undefined4 *puVar31;` matches `\*puVar31` and rewriting it
            # produces a syntax error, which reads back as "not byte-identical"
            # and quietly declined the whole variable — including the five sites
            # that were correct.
            cand, n, how = accepted(fn, out, v, E, paths, inc, cache, build)
            if not n:
                continue
            if cand is not None:
                out = cand
                done += n
                notes.append('%-22s %-10s -> %s (via %s::%s)  %d site(s) [%s]'
                             % (fn, v, E, T, F, n, how))
            else:
                declined += 1
                notes.append('%-22s %-10s -> %s: not byte-identical at i386 OR '
                             'wasm32, in either spelling — declined' % (fn, v, E))
        # ---- the third anchor: a DIRECT member chain (already-typed C)
        for m in EXPR_PTR.finditer(text):
            v = m.group('v')
            E = resolve_expr_owner(m.group('expr'), text, root)
            if not E:
                continue
            paths = field_paths(E, inc, cache)
            if not paths:
                continue
            cand, n, how = accepted(fn, out, v, E, paths, inc, cache, build)
            if not n:
                continue
            if cand is not None:
                out = cand
                done += n
                notes.append('%-22s %-10s -> %s (via %s)  %d site(s) [%s]'
                             % (fn, v, E, m.group('expr')[:28], n, how))
            else:
                declined += 1
                notes.append('%-22s %-10s -> %s: not byte-identical at i386 OR '
                             'wasm32, in either spelling — declined' % (fn, v, E))
        if out != text:
            open(path, 'w').write(out)

    for x in notes:
        print('  ' + x)
    print('fix_word_indexed_struct: %d site(s) rewritten, %d declined'
          % (done, declined))


if __name__ == '__main__':
    main()
