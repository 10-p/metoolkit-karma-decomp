#!/usr/bin/env python3
"""fix_member_base_walk.py — an array walked from the address of a MEMBER of
its first element.

    fix_member_base_walk.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT. gcc keeps the contact array in a register pointing at the first
element's `normal`, not at the element, and Ghidra writes every later field as
an offset from THAT:

    iVar31 = iVar32 * 0x28;                                   /* 40 = sizeof at i386 */
    do {
      *(__typeof__(tri->triangleData) *)
          ((kd_iptr)result->contacts->normal + iVar31 + 0x18) = tri->triangleData;
      iVar31 = iVar31 + 0x28;
    } while (iVar32 < result->contactCount);

★ SO THE OFFSET IS RELATIVE TO A MEMBER, AND THAT CHANGES THE ARITHMETIC EVERY
OTHER PASS DOES. `McdContact::normal` is at 12; `+ 0x18` is therefore byte 36 of
the element, which is `element2` at i386 and — because `element1` is a union
holding a `void *` — byte 40 at LP64. The write is EIGHT bytes wide there, so it
lands across `element1`'s high half and `element2`'s low half and corrupts both.

    McdContact   i386  sizeof 40   normal 12  element1 32  element2 36
                 LP64  sizeof 48   normal 12  element1 32  element2 40

⚠⚠ AND THE OBVIOUS DECOMPOSITION IS WRONG IN A WAY i386 CANNOT SHOW YOU.
`fix_element_stride`'s rule D reads a literal as `q*sizeof(E) + offsetof(member
at L % sizeof(E))`. Here L is 0x18 = 24 and byte 24 of `McdContact` is
`separation` — a real member, at 24 at BOTH widths. That decomposition compiles
to the same 24 it replaced at i386, passes the byte-identity gate, and is still
four bytes short at LP64. The remainder has to be taken from `b + L`, where `b`
is the offset of the member the chain is ROOTED at.

★ ONLY REWRITE WHAT MOVES, AND HERE THAT IS 4 OF THE 8 SITES. The same shape
carries `+ 0xc` (byte 24, `separation`) and `+ 0x14` (byte 32, `element1`) and
BOTH are already right at LP64, because neither member moves. `McdBatch.c`
writes `+ 0x14` and `+ 0x18` four lines apart. A rule that rewrote the shape
would have changed two correct statements into two expressions that happen to
agree at i386; the LP64 value is computed for every site and one that does not
move is left exactly as it was.

THE TYPE IS DECLARED AND THE EXPRESSION SAYS WHICH. `resolve_member` follows
`result->contacts` hop by hop through the oracle — ⚠ FOUR structs in this corpus
declare a pointer called `contacts`, so the name alone is a coin toss — and the
member chain's last hop gives the element type. Nothing is inferred: `contacts`
is `McdContact *` in `_McdIntersectResult`, and `normal` is a member of
`McdContact`.

WHAT IT REWRITES, per file, all-or-nothing, byte-identity at i386 the gate:

    A  the field offset   base + cur + 0x18   ->  base + cur + (OFF(element2) - OFF(normal))
    B  the cursor's seed  cur = k * 0x28      ->  k * (int)sizeof(McdContact)
    C  the cursor's step  cur = cur + 0x28    ->  cur + (int)sizeof(McdContact)

⚠ RUN IT ON A COPY: it edits in place. It needs `fix_ptrwidth` to have written
the `kd_iptr` the sites are spelled with, and `fix_derived_fields` to have named
the members, so it belongs late in the chain.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402
import fix_literal_offsets as flo                           # noqa: E402
import fix_narrow_loads as fnl                              # noqa: E402

HERE = kd_paths.MD
WORK = '/tmp/kd_memberbase'

CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types',
          '-DLINUX']
HEAD = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
        '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))
_SZ = re.compile(r'char \(\*\)\[(\d+)\]')

# `(kd_iptr)result->contacts->normal + iVar31 + 0x18`, and every shorter form of
# it. The cast is whatever `fix_ptrwidth` left; keying on a literal `(int)` is how
# a sibling rule in this project once matched nothing and reported no decline.
#
# ⚠ THE LITERAL IS OPTIONAL, AND THAT IS NOT TIDINESS. `McdGjk` and
# `McdPlaneIntersect` write the contact's `normal` at `+ cur`, `+ cur + 4`,
# `+ cur + 8` and `+ cur + 0xc` — all four of which are correct at LP64, because
# nothing before `separation` moves. The DEFECT in those two is the pitch alone:
# `cur = cur + 0x28` walks 40 bytes over 48-byte elements, so every contact after
# the first is written eight bytes short. A rule that needed an offset to move
# before it looked at the stride skipped both files silently.
BASE = re.compile(r'\((?:kd_iptr|kd_uptr|int|uint)\)\s*'
                  r'(?P<expr>[A-Za-z_]\w*(?:(?:->|\.)\w+)+)->(?P<mem>\w+)'
                  r'(?:\s*\+\s*(?P<cur>[A-Za-z_]\w*)(?![\w(]))?'
                  r'(?:\s*\+\s*(?P<k>0x[0-9a-fA-F]+|\d+)\b)?')
INC_G = None
CACHE_G = {}


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def measure(expr, bits='-m32'):
    """`sizeof`/`offsetof` read off the compiler, never computed here."""
    key = (expr, bits)
    if key in CACHE_G:
        return CACHE_G[key]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p.c')
    open(src, 'w').write(HEAD + 'char kd_probe[%s];\nint kd_force = &kd_probe;\n' % expr)
    r = subprocess.run(['gcc', bits, '-DLINUX'] + includes(INC_G)
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    m = _SZ.search(r.stderr)
    CACHE_G[key] = int(m.group(1)) if m else None
    return CACHE_G[key]


def off_expr(E, F):
    return '((int)((char *)&((%s *)0)->%s - (char *)0))' % (E, F)


def fields_of(E):
    """{i386 offset -> member name}, top level only, from the type database."""
    import json
    tf = os.path.join(HERE, 'include', 'kd_types_fields.json')
    db = json.load(open(tf)) if os.path.exists(tf) else {}
    d = db.get(E) or db.get('_' + E) or {}
    return {int(k): v for k, v in d.items()}


def element_type(expr, text, root):
    """`result->contacts` -> `McdContact`, every hop out of the oracle.

    ⚠ A MEMBER NAME IS NOT A TYPE. Four structs in this corpus declare a pointer
    called `contacts` — `McdBatchEntry`, `McdBatchContactPool`,
    `_McdIntersectResult` and `MstBridge` — so matching on the name alone is a
    coin toss. Each hop is read from the ORACLE and an unresolvable hop returns
    None, which declines the site.

    ⚠ AND THE ORACLE KEYS BY TAG WHILE THE CODE NAMES THE TYPEDEF.
    `typedef struct _McdIntersectResult {...} McdIntersectResult;` is stored under
    `_McdIntersectResult`, and `McdIntersectResult *result` is what the recovered
    source says — so asking once, with the spelling in front of you, returns
    nothing. `fix_narrow_loads.resolve_member` asks once and answers None here;
    that is why this does not simply call it. Both spellings, every hop."""
    bodies = flo.struct_bodies(os.path.join(root, 'include'))

    def body_of(tag):
        b = bodies.get(tag) or bodies.get('_' + tag) or bodies.get(tag.lstrip('_'))
        return re.sub(r'/\*.*?\*/', ' ', b, flags=re.S) if b else None

    parts = [re.sub(r'\[[^\]]*\]', '', x) for x in re.split(r'->|\.', expr) if x]
    m = re.search(r'(?<![\w])([A-Za-z_]\w*)\s*\*\s*%s\b' % re.escape(parts[0]), text)
    if not m:
        return None
    tag = m.group(1)
    for hop in parts[1:]:
        body = body_of(tag)
        if not body:
            return None
        mm = re.search(r'(?<![\w])([A-Za-z_]\w*)\s*\*\s*%s\s*[;,\[]' % re.escape(hop),
                       body)
        if not mm:
            return None
        tag = mm.group(1)
    return tag


def compiles_identically(fn, text, build, inc):
    """⚠ THE FILE MUST KEEP ITS NAME — gcc records the basename in STT_FILE."""
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


def plan(text, root, notes, fn):
    """(edits, label) for one file, or None.

    ⚠ GROUPED BY ELEMENT TYPE, BECAUSE ONE FILE CAN WALK TWO ARRAYS THIS WAY.
    `McdBatch.c` steps `pMVar1->start->normal` over `McdBatchEntry` and
    `pMVar3->contacts->normal` over `McdContact` in the same function, with
    different cursors and different strides. Taking "the" element type for the
    file gave the second array the first one's `sizeof` — a stride that is wrong
    at BOTH widths, which is the one way to fail this that i386 would have
    caught. It is grouped so that cannot happen rather than noticed."""
    edits, moved, by_e = [], 0, {}
    for m in BASE.finditer(text):
        expr, mem = m.group('expr'), m.group('mem')
        E = element_type(expr, text, root)
        if not E:
            continue
        sz32, sz64 = measure('sizeof(%s)' % E), measure('sizeof(%s)' % E, '-m64')
        b32 = measure(off_expr(E, mem))
        b64 = measure(off_expr(E, mem), '-m64')
        if None in (sz32, sz64, b32, b64) or sz32 == sz64:
            continue
        by_e.setdefault(E, (set(), set()))[0].add(mem)
        if m.group('cur'):
            by_e[E][1].add(m.group('cur'))
        if m.group('k') is None:
            continue
        K = int(m.group('k'), 0)
        # ---- A: the field offset, measured from the member the chain is ROOTED
        # at. `q` whole elements plus the member that begins at the remainder.
        q, r = divmod(b32 + K, sz32)
        flds = fields_of(E)
        if r not in flds:
            continue                       # not a field of this element — leave it
        want = measure(off_expr(E, flds[r]), '-m64')
        if want is None:
            continue
        if q * sz64 + want - b64 == K:
            continue                       # ★ MEASURED NOT TO MOVE — leave it alone
        parts = []
        if q:
            parts.append('%d * (int)sizeof(%s)' % (q, E) if q != 1
                         else '(int)sizeof(%s)' % E)
        parts.append('(%s - %s)' % (off_expr(E, flds[r]), off_expr(E, mem)))
        edits.append((m.start('k'), m.end('k'), ' + '.join(parts)))
        moved += 1
    # ---- B and C: each cursor's own seed and step, at ITS OWN element size.
    # ⚠ ONLY WHERE THE LITERAL IS THE i386 ELEMENT SIZE. A cursor into this array
    # can also be stepped by 1 as an index (`iVar31 = iVar31 + 1` walks the
    # triangle's three vertices in the same function), and that is not a stride.
    #
    # ⚠ AND NOT ANCHORED TO A STATEMENT. `McdPlaneIntersect` advances its cursor
    # inside the loop's own condition — `while ((i < numv) && (iVar11 = iVar11 +
    # 0x28, i < result->contactMaxCount))` — so a `^...;$` pattern leaves the one
    # stride in the file untouched and reports a clean nothing.
    for E, (mems, cursors) in sorted(by_e.items()):
        sz32 = measure('sizeof(%s)' % E)
        for cur in sorted(cursors):
            c = re.escape(cur)
            for rx in (r'(?<![\w])%s = %s \+ (0x[0-9a-fA-F]+|\d+)\b' % (c, c),
                       r'(?<![\w])%s = [A-Za-z_]\w* \* (0x[0-9a-fA-F]+|\d+)\b' % c):
                for mm in re.finditer(rx, text):
                    if int(mm.group(1), 0) != sz32:
                        continue
                    edits.append((mm.start(1), mm.end(1), '(int)sizeof(%s)' % E))
    edits = list({(a, b): (a, b, r) for a, b, r in edits}.values())
    if not edits:
        return None
    label = '; '.join('%s via ::%s' % (E, '/'.join(sorted(m)))
                      for E, (m, _) in sorted(by_e.items()))
    return edits, '%s  (%d offset(s) moved, %d edit(s))' % (label, moved, len(edits))


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    global INC_G
    INC_G = os.path.join(root, 'include')

    # ---- THE SELF-CHECK. A probe that cannot compile measures nothing, every
    # site drops out, and "0 rewritten" reads exactly like "there were none".
    if measure('sizeof(McdContact)') != 40 or measure('sizeof(McdContact)', '-m64') != 48:
        sys.exit('fix_member_base_walk: SELF-CHECK FAILED — sizeof(McdContact) '
                 'measured %r/%r, want 40/48.'
                 % (measure('sizeof(McdContact)'), measure('sizeof(McdContact)', '-m64')))

    done = declined = 0
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        p = plan(text, root, notes, fn)
        if not p:
            continue
        edits, label = p
        cand = text
        for st, en, rep in sorted(edits, key=lambda x: -x[0]):
            cand = cand[:st] + rep + cand[en:]
        # ⚠ ALL OR NOTHING PER FILE: the offsets and the stride are one walk, and
        # half of it lands somewhere neither width means.
        if compiles_identically(fn, cand, build, INC_G):
            open(path, 'w').write(cand)
            done += len(edits)
            notes.append('%-24s %s' % (fn, label))
        else:
            declined += 1
            notes.append('%-24s %s — DECLINED: not byte-identical at i386'
                         % (fn, label))
    for x in notes:
        print('  ' + x)
    print('fix_member_base_walk: %d edit(s) in %d file(s), %d declined'
          % (done, len(notes) - declined, declined))


if __name__ == '__main__':
    main()
