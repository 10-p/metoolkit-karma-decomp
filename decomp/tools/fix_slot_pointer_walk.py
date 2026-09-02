#!/usr/bin/env python3
"""fix_slot_pointer_walk.py — a POINTER parked in a fabricated struct slot, then
walked through THAT struct's offsets.

    fix_slot_pointer_walk.py <kd_lp64/allobj> <kd_build> [metoolkit-root]

THE DEFECT. Ghidra models a group of unrelated stack slots as one local of some
real struct type, and then addresses whatever each slot holds through THAT
struct's members. `IxBoxTriList` declares `McdGeometry MStack_24c;` and uses it
as a bag:

    MStack_24c.frame = (McdFrameworkID)(pfVar26 + 0xc);       /* a MeReal *   */
    MStack_24c.next  = (McdGeometryID)result->normal;         /* a MeReal *   */
    ...
    (MStack_24c.next)->prev = (McdGeometryID)KD_FBITS(diff.v[1] + *(float *)&((MStack_24c.next)->prev));
    (MStack_24c.next)->next = (McdGeometryID)KD_FBITS(diff.v[2] + *(float *)&((MStack_24c.next)->next));

`MStack_24c.next` holds `result->normal`, a `MeReal[3]`. `->prev` is byte 4 and
`->next` is byte 8 of `McdGeometry`, so at i386 those two statements are
`normal[1] += diff.v[1]` and `normal[2] += diff.v[2]` — exactly right.

    McdGeometry   i386  mRefCtAndID 0  prev 4  next  8  frame 12  sizeof 16
                  LP64  mRefCtAndID 0  prev 8  next 16  frame 24  sizeof 32

★ At LP64 `->prev` is byte 8, which is `normal[2]`, and `->next` is byte **16**,
which is off the end of a three-float vector — and both are written EIGHT bytes
wide because the lvalue is pointer-typed. On the Box-vs-TriangleList path, i.e.
every vehicle on level geometry.

⚠ NOTHING IS TRUNCATED, so `ptrwidth_check` cannot see it; the census lands it in
UNEXPLAINED, which is where it sat. This is the same family as
`fix_callback_context`'s `void *` — a value of one type read at another type's
offsets — with the wrong type supplied by a stack aggregate instead of a
callback signature.

THE RULE, and every clause is measured rather than assumed:

  1. `VAR` is a struct VALUE local (`McdGeometry MStack_24c;`, no `*`). A
     pointer-typed local walking its own type is an ordinary, correct walk.
  2. the region assigns `VAR.M = (CAST)EXPR;` and `EXPR` resolves, member by
     member, to an ARRAY-OF-SCALAR type. `result->normal` is `MeVector3`, i.e.
     `MeReal[3]`, so the slot holds `MeReal *`.
  3. the site is `(VAR.M)->F` with `F` a member of `VAR`'s struct.
  4. `offsetof(struct, F)` is an exact multiple of `sizeof(MeReal)` — otherwise
     the original was not addressing an element and this pass has no business
     naming one.
  5. ★ THE OFFSET MUST ACTUALLY MOVE between the two widths. `fix_member_base_walk`
     learned this the expensive way: rewriting a site that is already right at
     LP64 turns a correct statement into an expression that merely agrees at
     i386. `MStack_24c.frame`'s two sites are exactly that case —
     `McdFramework::geometryRegisteredCount` is byte 4 and `firstModel` byte 8
     at BOTH widths, so they are left alone and are NOT defects.
  6. the rewrite must reproduce the i386 object BYTE FOR BYTE.

  ->  ((MeReal *)MStack_24c.next)[1]

⚠⚠ CLAUSE 6 IS A BACKSTOP, NOT THE EVIDENCE. Byte-identity at i386 cannot see a
type error at 64-bit — this project has recorded six occasions where it did not.
The evidence is clauses 1-5 plus the ktrace measurement: with this repair and the
geometry-offset one, the LP64 trace of `test-karma-1` under Onslaught is
BYTE-IDENTICAL to the SSE-32 control.

Run it on a COPY: it edits in place. AFTER `fix_literal_offsets`, BEFORE
`fix_ptrwidth`.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402
from fix_derived_fields import (BANNER, HEAD, compiles_identically,  # noqa: E402
                                includes)
from fix_narrow_pointers import declared_type               # noqa: E402

WORK = '/tmp/kd_slotwalk'
_SZ = re.compile(r'char \(\*\)\[(\d+)\]')
_CACHE = {}

# `(VAR.M)->F` — the walk. The parentheses are Ghidra's and they are what makes
# the shape searchable at all.
SITE = re.compile(r'\(\s*(?P<var>[A-Za-z_]\w*)\s*\.\s*(?P<m>[A-Za-z_]\w*)\s*\)\s*->\s*(?P<f>[A-Za-z_]\w*)')
# `VAR.M = (CAST)EXPR;` — what the slot was actually given.
ASSIGN = r'(?m)^\s*%s\s*\.\s*%s\s*=\s*(?:\(\s*[\w ]*\*?\s*\)\s*)?(?P<rhs>[^;]+);'


def probe(body, bits, inc):
    """Compile a one-liner and read the answer out of gcc's own diagnostic."""
    key = (body, bits)
    if key in _CACHE:
        return _CACHE[key]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p%d.c' % bits)
    open(src, 'w').write(HEAD + body)
    r = subprocess.run(['gcc', '-m%d' % bits, '-DLINUX'] + includes(inc)
                       + ['-Werror=int-conversion', '-c', '-o', os.devnull, src],
                       capture_output=True, text=True)
    m = _SZ.search(r.stderr)
    _CACHE[key] = (int(m.group(1)) - 1) if m else None
    return _CACHE[key]


def offset_of(tag, field, bits, inc):
    return probe('char kd_probe[((char *)&((%s *)0)->%s - (char *)0) + 1];\n'
                 'int kd_force = &kd_probe;\n' % (tag, field), bits, inc)


def elem_size(expr_type, bits, inc):
    """sizeof of one element of an array-typed member spelling."""
    return probe('char kd_probe[sizeof(((%s *)0)[0]) + 1];\n'
                 'int kd_force = &kd_probe;\n' % expr_type, bits, inc)


def region_bounds(text, pos):
    start, end = 0, len(text)
    for m in BANNER.finditer(text):
        if m.start() <= pos:
            start = m.start()
        elif m.start() > pos:
            end = m.start()
            break
    return start, end


def rhs_elem_type(region, rhs, inc):
    """`result->normal` -> the ELEMENT type spelling (`MeReal`), or None.

    Follows the member chain through the oracle rather than guessing from the
    name: four structs in this corpus declare a `normal`."""
    rhs = rhs.strip()
    m = re.fullmatch(r'([A-Za-z_]\w*)\s*->\s*([A-Za-z_]\w*)', rhs)
    if not m:
        return None
    base_ty = declared_type(region, m.group(1))
    if not base_ty:
        return None
    tag = base_ty.replace('*', '').replace('struct', '').strip()
    # the member must be an ARRAY of a scalar; ask the compiler for both sizes
    whole = probe('char kd_probe[sizeof(((%s *)0)->%s) + 1];\n'
                  'int kd_force = &kd_probe;\n' % (tag, m.group(2)), 32, inc)
    one = probe('char kd_probe[sizeof(((%s *)0)->%s[0]) + 1];\n'
                'int kd_force = &kd_probe;\n' % (tag, m.group(2)), 32, inc)
    if not whole or not one or whole == one or whole % one:
        return None
    one64 = probe('char kd_probe[sizeof(((%s *)0)->%s[0]) + 1];\n'
                  'int kd_force = &kd_probe;\n' % (tag, m.group(2)), 64, inc)
    if one64 != one:
        return None                        # the element itself moves; not ours
    return '__typeof__(((%s *)0)->%s[0])' % (tag, m.group(2)), one


def elem_spelling(text, pos, inc):
    """For a SITE match at `pos`, the element spelling that replaces it, plus
    the note — or None when any clause of the rule declines."""
    m = SITE.match(text, pos)
    if not m:
        return None
    var, mem, fld = m.group('var'), m.group('m'), m.group('f')
    rs, re_ = region_bounds(text, pos)
    region = text[rs:re_]

    ty = declared_type(region, var)
    if not ty or '*' in ty:
        return None                         # (1) a real pointer walk, not a slot
    tag = ty.replace('struct', '').strip()

    a = re.search(ASSIGN % (re.escape(var), re.escape(mem)), region)
    if not a:
        return None
    got = rhs_elem_type(region, a.group('rhs'), inc)
    if not got:
        return None                         # (2) not an array-of-scalar slot
    etype, esize = got

    o32, o64 = offset_of(tag, fld, 32, inc), offset_of(tag, fld, 64, inc)
    if o32 is None or o64 is None or o32 % esize or o32 == o64:
        return None                         # (4) / (5)
    return ('((%s *)%s.%s)[%d]' % (etype, var, mem, o32 // esize),
            '%s.%s->%-6s  i386 %d -> LP64 %d   -> [%d]'
            % (var, mem, fld, o32, o64, o32 // esize), m.end())


# `LHS = (SomeID)KD_FBITS( ... );` — the float's bits put back through a POINTER
# typedef. ★ Once the lvalue is a `MeReal` again the round trip is not merely
# unnecessary, it will not compile: there is no float-to-pointer conversion in C.
# So the whole STATEMENT is rewritten, not just the address inside it.
FBITS_RHS = re.compile(r'\s*=\s*\(\s*(?:Mcd|Mdt|Me)\w*ID\s*\)\s*KD_FBITS\s*\(')


def balanced(text, i):
    """Index just past the `)` matching the `(` at text[i-1]."""
    depth = 1
    while i < len(text):
        if text[i] == '(':
            depth += 1
        elif text[i] == ')':
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return None


def compiles_at_m64(fn, text, inc):
    """Does this candidate COMPILE at LP64 at all?

    ⚠⚠ i386 BYTE-IDENTITY CANNOT ANSWER THIS, and a `#if __SIZEOF_POINTER__`
    guard makes it structurally blind: the i386 branch is the ORIGINAL text, so
    the gate passes by construction while the `#else` branch is whatever was
    proposed. `fix_typeid_dispatch` shipped an uncompilable LP64 branch past a
    `145 object(s), 0 byte difference(s)` report exactly that way."""
    d = os.path.join(WORK, 'm64')
    os.makedirs(d, exist_ok=True)
    src = os.path.join(d, fn)
    open(src, 'w').write(text)
    return subprocess.run(
        ['gcc', '-m64', '-O2', '-fno-strict-aliasing', '-std=gnu99', '-w',
         '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX',
         '-I' + os.path.join(kd_paths.MD, 'include')] + includes(inc)
        + ['-c', '-o', os.devnull, src], capture_output=True).returncode == 0


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')

    # ---- THE SELF-CHECK. "0 repaired" must not read the same as "the offset
    # map measured nothing", which is how this class of pass fails silently.
    if (offset_of('McdGeometry', 'prev', 32, inc) != 4
            or offset_of('McdGeometry', 'prev', 64, inc) != 8
            or offset_of('McdGeometry', 'next', 64, inc) != 16):
        sys.exit('fix_slot_pointer_walk: SELF-CHECK FAILED — McdGeometry prev '
                 '%r/%r next(64) %r; want 4/8 and 16.'
                 % (offset_of('McdGeometry', 'prev', 32, inc),
                    offset_of('McdGeometry', 'prev', 64, inc),
                    offset_of('McdGeometry', 'next', 64, inc)))

    fixed = declined = 0
    notes, declines = [], []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        if not SITE.search(text):
            continue

        # ---- statements first, so an assignment is rewritten whole and its
        # own reads are not edited twice.
        edits, taken = [], []
        for m in SITE.finditer(text):
            got = elem_spelling(text, m.start(), inc)
            if not got:
                continue
            rep, note, _e = got
            fb = FBITS_RHS.match(text, m.end())
            if not fb:
                continue
            close = balanced(text, fb.end())
            semi = text.find(';', close)
            if close is None or semi < 0:
                continue
            inner = text[fb.end():close]
            # ⚠ RESOLVE THE INNER SITES AT THEIR ABSOLUTE POSITIONS. Rewriting
            # the extracted fragment on its own looks equivalent and is not:
            # `region_bounds` finds no banner in a detached string, so
            # `declared_type` answers None for every variable in it and each
            # inner read silently declines — which is exactly what left
            # `*(float *)&((MStack_24c.next)->prev)` sitting inside a repaired
            # assignment, still reading byte 8 at LP64.
            pieces, i = [], fb.end()
            for s in SITE.finditer(text, fb.end(), close):
                g = elem_spelling(text, s.start(), inc)
                if not g:
                    continue
                pieces.append(text[i:s.start()])
                pieces.append(g[0])
                i = s.end()
            pieces.append(text[i:close])
            stmt = '%s = %s' % (rep, ''.join(pieces))
            edits.append((m.start(), semi, [stmt], '%-24s %s  (assignment)' % (fn, note)))
            taken.append((m.start(), semi))

        for m in SITE.finditer(text):
            if any(s <= m.start() < e for s, e in taken):
                continue
            got = elem_spelling(text, m.start(), inc)
            if got:
                edits.append((m.start(), m.end(), [got[0]],
                              '%-24s %s' % (fn, got[1])))

        n0 = fixed
        for start, end, reps, note in sorted(edits, key=lambda e: -e[0]):
            for rep in reps:
                cand = text[:start] + rep + text[end:]
                if compiles_identically(fn, cand, build, inc) \
                        and compiles_at_m64(fn, cand, inc):
                    text = cand
                    fixed += 1
                    notes.append(note)
                    break
            else:
                declined += 1
                declines.append('%s  — no spelling reproduced the i386 object' % note)
        if fixed > n0:
            open(path, 'w').write(text)

    print('fix_slot_pointer_walk:')
    print('  repaired (i386 byte-identical)                : %d' % fixed)
    print('  declined (reported, not guessed)              : %d' % declined)
    for n in notes:
        print('     ' + n)
    for d in declines:
        print('     - ' + d)
    return 0


if __name__ == '__main__':
    sys.exit(main())
