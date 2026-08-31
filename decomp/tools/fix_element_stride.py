#!/usr/bin/env python3
"""fix_element_stride.py — a table's ELEMENT SIZE frozen at the i386 value.

    fix_element_stride.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT. A struct holds a pointer to an array of another struct, and every
walk of that array carries the element size as a LITERAL:

    McdAggregate      { McdGeometry m_g; McdAggregateElement *elementTable; ... }
    McdAggregateElement { MeMatrix4 mRelTM; McdGeometryID mGeometry; }

                       i386                LP64
    sizeof(element)    68 (0x44)           72
    offsetof(mGeometry) 64 (0x40)          64

★ THE OFFSET IS RIGHT AT BOTH WIDTHS AND THE STRIDE IS NOT, which is why this
survives every pass that checks offsets. `mRelTM` is sixteen floats and does not
grow, so `mGeometry` stays at byte 64; only the trailing pointer grows, and only
the SIZE moves. Every `i * 0x44` therefore lands 4·i bytes short from the second
element on, and reads the tail of the previous element's matrix as a pointer.

THIS IS FRAME (6) OF `LP64-RUNS-THE-GAME`, and the crash says so in one value:

    McdGeometryIncrementReferenceCount (g = 0x3f800000596203b0)

`0x3f800000` is 1.0f. A matrix diagonal is sitting in the top half of a pointer.

WHY IT IS NOT ALREADY COVERED.
  * `fix_baked_sizeof` wants an ASSIGNMENT TARGET that names the type. Here the
    allocation goes into `void *pvVar2` and only later into the typed field, so
    there is no type at the allocation.
  * `fix_strides`' byte-cursor rule needs `(kd_iptr)&expr + cursor` — an
    address-of. These cursors are added to a LOADED pointer, so it cannot match.

THE TYPE IS DECLARED, NOT INFERRED. `McdAggregateElement *elementTable;` is in
the oracle header, so the element type of anything reached through that field is
written down. The literal must ALSO equal `sizeof(E)` at i386, and the size must
actually CHANGE at LP64, or there is no defect and the site is skipped. Two
independent facts, exactly as `fix_baked_sizeof` requires, and a site where they
disagree is left alone.

SCOPE, MEASURED RATHER THAN ASSUMED: 117 fields in the corpus are declared as a
pointer to a struct whose size changes at LP64. Exactly ONE of them has its i386
element size present as a literal in the file that uses it —
`McdAggregate::elementTable`. So this rewrites one object and reports the rest
as having nothing to do.

WHAT IT REWRITES, and it needs the file to reach the table at all before it
touches anything:

    A  the allocation      create(n * 0x44)          -> n * (int)sizeof(E)
    B  an index stride     ... + i * 0x44            -> i * (int)sizeof(E)
    C  a byte cursor       cur = cur + 0x44          -> cur + (int)sizeof(E)

C is the reason this needs a little dataflow: the cursor is an ordinary `int`
that only means anything because it is added to a pointer loaded out of the
field. A cursor is recognised only where it is used that way, so a literal 68
that means something else is never in scope.

⚠ THE SPELLING IS CHOSEN BY THE COMPILER, NOT BY THIS FILE. `sizeof` is
`size_t`, so `n * sizeof(T)` is UNSIGNED where `n * 0x44` was `int`, with
different overflow rules and a different loop out of gcc — `fix_baked_sizeof`
lost 64 bytes of CxSmallSort to exactly that. Each candidate is compiled and
compared against the baseline object; whichever reproduces it byte for byte is
kept, and a file where none does is declined and reported.

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
WORK = '/tmp/kd_elemstride'

CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types',
          '-DLINUX']

HEAD = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
        '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))
_SZ = re.compile(r'char \(\*\)\[(\d+)\]')

# set once in main() — the width probes inside rewrite() need them
INC_G = None
CACHE_G = {}

# a member declared `E *name;`
MEMBER_PTR = re.compile(r'(?P<ty>\w+)\s*\*\s*(?P<n>\w+)\s*;')


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def measure(expr, inc, cache, bits='-m32'):
    key = (expr, bits)
    if key in cache:
        return cache[key]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p.c')
    open(src, 'w').write(HEAD + 'char kd_probe[%s];\nint kd_force = &kd_probe;\n' % expr)
    r = subprocess.run(['gcc', bits, '-DLINUX'] + includes(inc)
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    m = _SZ.search(r.stderr)
    cache[key] = int(m.group(1)) if m else None
    return cache[key]


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


def element_fields(inc, cache):
    """(T, F) -> (E, i386 size) for every member declared as a pointer to a
    struct whose size CHANGES at LP64. A size that does not change is not a
    defect and is never returned."""
    bodies = flo.struct_bodies(inc)
    out = {}
    for T, body in bodies.items():
        body = re.sub(r'/\*.*?\*/', ' ', body, flags=re.S)
        for m in MEMBER_PTR.finditer(body):
            E, F = m.group('ty'), m.group('n')
            if E not in bodies or E == T:
                continue
            a = measure('sizeof(%s)' % E, inc, cache, '-m32')
            b = measure('sizeof(%s)' % E, inc, cache, '-m64')
            if a and b and a != b:
                out[(T, F)] = (E, a)
    return out


def lit(n):
    """Both spellings Ghidra emits for the same constant."""
    return r'(?<![\w.])(?:0x%x|%d)(?![\w])' % (n, n)


def fields_of(E):
    """{i386 offset -> member name} for the element type, top level only.

    Top level ONLY, and that is the filter that makes the decomposition below a
    measurement rather than a coincidence: a literal qualifies as an offset into
    this table if its remainder lands on a member START, and `McdAggregateElement`
    has exactly two — 0 and 64. A remainder of 4 or 1 is arithmetic that has
    nothing to do with the table and is left alone."""
    import json
    tf = os.path.join(HERE, 'include', 'kd_types_fields.json')
    db = json.load(open(tf)) if os.path.exists(tf) else {}
    d = db.get(E) or db.get('_' + E) or {}
    return {int(k): v for k, v in d.items()}


def off_expr(E, F):
    return '((int)((char *)&((%s *)0)->%s - (char *)0))' % (E, F)


def term(E, flds, q, r, szname):
    """`q * sizeof(E) + offsetof(E, field_at_r)`, with the zero terms omitted so
    the i386 text folds to exactly the constant it replaced."""
    parts = []
    if q:
        parts.append('%d * %s' % (q, szname) if q != 1 else szname)
    if r:
        parts.append(off_expr(E, flds[r]))
    return ' + '.join(parts) if parts else '0'


def rewrite(text, T, F, E, sz, repl):
    """Rewrite every baked element offset, stride and access width.

    ⚠ IT IS NOT ONLY THE STRIDE. `McdAggregateCreate` zeroes the table with gcc's
    four-at-a-time unrolling, and every offset in it is baked:

        *(undefined4 *)(table + 0x40  + cur) = 0;      /* 64 + 0*68 */
        *(undefined4 *)(table + 0x84  + cur) = 0;      /* 64 + 1*68 */
        *(undefined4 *)(table + 200   + cur) = 0;      /* 64 + 2*68 */
        *(undefined4 *)(table + 0x10c + cur) = 0;      /* 64 + 3*68 */
        cur = cur + 0x110;                             /*      4*68 */

    At LP64 those slots are at 64, 136, 208 and 280, so the loop zeroes the
    middle of the matrices and leaves EVERY mGeometry holding malloc garbage.
    ★ AND THE WIDTH IS WRONG TOO: `undefined4` clears four bytes of an
    eight-byte pointer, so even a correctly-addressed slot keeps its high half.
    Fixing one without the other still ships a dangling pointer.

    THE DECOMPOSITION IS THE MEASUREMENT. A literal `L` on the table's additive
    chain is an offset into it only if `L % sizeof_i386(E)` lands on a member
    START of E. `q` elements plus that member is then the same address at every
    width. A remainder that is not a member start is ordinary arithmetic and is
    left alone, which is what keeps loop counters and `+ 4`s out of scope."""
    off = r'\(\(%s \*\)0\)->%s\b' % (re.escape(T), re.escape(F))
    if not re.search(off, text):
        return text, 0, 0
    L = lit(sz)
    szname = repl
    flds = fields_of(E)
    n = 0

    # ---- WHO IS A TABLE BASE AND WHO IS A CURSOR, TO A FIXPOINT.
    #
    # ⚠ ONE PASS IS NOT ENOUGH, and both misses were real cursors. A cursor can
    # be added to a variable that was loaded through the field in Ghidra's OTHER
    # spelling (`MVar2 = g[1].mRefCtAndID`), which this anchor cannot see; and a
    # cursor can sit on the LEFT of the `+` (`local_64 + (*(kd_uptr *)...)`),
    # which a right-hand-side pattern misses. So the two facts feed each other:
    #   R1  a variable loaded through the field's offsetof IS a table base
    #   R2  a variable added to a table base IS a cursor
    #   R3  a variable a CURSOR is added to IS a table base
    # R3 is what reaches `MVar2` — a confirmed cursor is added to it, which is
    # the same evidence the offsetof would have given, arrived at from the other
    # end. Iterating to a fixpoint is what makes the pair complete.
    ident = r'(?<![\w.])%s(?![\w])'
    tab = set(re.findall(r'(\w+)\s*=\s*[^;\n]*' + off + r'[^;\n]*;', text))
    cur = set()
    lines = text.split('\n')
    for _ in range(8):
        grew = False
        for line in lines:
            anchored = bool(re.search(off, line)) or \
                any(re.search(ident % v, line) for v in tab)
            names = set(re.findall(r'\+\s*([A-Za-z_]\w*)(?![\w(])', line)) | \
                set(re.findall(r'([A-Za-z_]\w*)\s*\+', line))
            if anchored:
                new = {v for v in names if v not in tab and v not in cur}
                if new:
                    cur |= new
                    grew = True
            elif any(v in cur for v in names):
                # R3: whatever a cursor is being added to is the table
                new = {v for v in names if v not in cur and v not in tab}
                if new:
                    tab |= new
                    grew = True
        if not grew:
            break

    for i, line in enumerate(lines):
        touches = re.search(off, line) or any(
            re.search(ident % v, line) for v in tab)
        # C: a cursor's own step — any WHOLE number of elements
        step = re.match(r'^(\s*)(\w+) = \2 \+ (0x[0-9a-fA-F]+|\d+)\s*;\s*$', line)
        seed = re.match(r'^(\s*)(\w+) = (0x[0-9a-fA-F]+|\d+)\s*;\s*$', line)
        for m in (step, seed):
            if not (m and m.group(2) in cur):
                continue
            v = int(m.group(3), 0)
            if v == 0 or v % sz:
                continue
            lines[i] = line.replace(m.group(3), term(E, flds, v // sz, 0, szname), 1)
            n += 1
            break
        else:
            if not (touches or re.search(r'MeMemoryAPI\.create', line)):
                continue
            # B: an index stride  `i * 0x44`
            new, k = re.subn(r'\*\s*' + L, '* ' + szname, line)
            n += k
            # D: a baked offset on the table's additive chain
            def sub(mm):
                nonlocal n
                v = int(mm.group(1), 0)
                if v == 0:
                    return mm.group(0)
                q, r = divmod(v, sz)
                if r not in flds:
                    return mm.group(0)          # not an offset into this table
                n += 1
                return '+ ' + term(E, flds, q, r, szname)
            new = re.sub(r'\+ (0x[0-9a-fA-F]+|\d+)(?![\w*])', sub, new)
            # F: the ACCESS WIDTH of the member the address lands on.
            #
            # ⚠ SIGNEDNESS IS PART OF THE REPAIR, and dropping it is not a
            # cosmetic difference: `*(int *)(...) != 0` compiled through
            # `kd_uptr` is a DIFFERENT i386 object, and three lines declined the
            # whole file on exactly that. `int` widens to `kd_iptr`, which IS
            # `int` at 32-bit pointer width, so the test is untouched there.
            #
            # ⚠ AND ONLY THE OUTERMOST ACCESS. These lines carry a second,
            # nested `*(int *)` that loads the TABLE POINTER itself — a
            # different defect, and fix_narrow_loads' to fix. Rewriting every
            # match on the line took that one too and changed what it meant.
            for r, fname in sorted(flds.items()):
                a = measure('sizeof(((%s *)0)->%s)' % (E, fname), INC_G, CACHE_G, '-m32')
                b = measure('sizeof(((%s *)0)->%s)' % (E, fname), INC_G, CACHE_G, '-m64')
                if not (a == 4 and b == 8 and off_expr(E, fname) in new):
                    continue
                new2, k2 = re.subn(
                    r'\*\((int|MeI32|undefined4|uint|MeU32) \*\)',
                    lambda mm: '*(%s *)' % ('kd_iptr' if mm.group(1) in ('int', 'MeI32')
                                            else 'kd_uptr'), new, count=1)
                if k2:
                    new, n = new2, n + k2
            lines[i] = new
    out = '\n'.join(lines)
    residual = len(re.findall(L, out))
    return out, n, residual


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')
    cache = {}
    global INC_G, CACHE_G
    INC_G, CACHE_G = inc, cache

    # ---- THE SELF-CHECK. A probe that cannot compile measures nothing, every
    # field drops out, and "0 rewritten" reads exactly like "there were none".
    probe = measure('sizeof(McdAggregateElement)', inc, cache, '-m32')
    if probe != 68:
        sys.exit('fix_element_stride: SELF-CHECK FAILED — '
                 'sizeof(McdAggregateElement) measured %r, want 68.' % probe)

    fields = element_fields(inc, cache)
    done = declined = 0
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        cur = text
        for (T, F), (E, sz) in sorted(fields.items()):
            if not re.search(r'\(\(%s \*\)0\)->%s\b' % (re.escape(T), re.escape(F)), cur):
                continue
            # ⚠ NO `is the bare literal present` PRECONDITION. The unrolled
            # zeroing loop carries 0x84, 200 and 0x10c — `64 + k*68` — and a
            # step of 0x110, none of which is sizeof(E) itself. Gating on the
            # bare stride found the strides and left the loop that clears the
            # table untouched, which is the half that actually crashed.
            kept = None
            for repl in ('(int)sizeof(%s)' % E, 'sizeof(%s)' % E):
                cand, n, residual = rewrite(cur, T, F, E, sz, repl)
                if not n:
                    break
                if compiles_identically(fn, cand, build, inc):
                    kept = (cand, n, residual, repl)
                    break
            if kept is None:
                declined += 1
                notes.append('%-24s %s::%s -> %s: no spelling is byte-identical '
                             'at i386 — declined' % (fn, T, F, E))
                continue
            cur, n, residual, repl = kept
            done += n
            notes.append('%-24s %s::%s -> %s  %d site(s) as %s, %d literal(s) left'
                         % (fn, T, F, E, n, repl, residual))
        if cur != text:
            open(path, 'w').write(cur)

    for x in notes:
        print('  ' + x)
    print('fix_element_stride: %d site(s) rewritten, %d declined' % (done, declined))


if __name__ == '__main__':
    main()
