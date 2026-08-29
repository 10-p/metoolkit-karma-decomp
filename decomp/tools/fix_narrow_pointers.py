#!/usr/bin/env python3
"""fix_narrow_pointers.py — a POINTER held in something four bytes wide.

    fix_narrow_pointers.py <kd_out/allobj> <kd_build> [metoolkit-root]

`fix_ptrwidth.py` widens the punned CASTS — `(int)p` becomes `(kd_iptr)p` — at
the columns clang names. Two shapes of the same defect have no cast for it to
rewrite, so they survive that pass untouched and unreported:

  A. THE LOCAL IS DECLARED NARROW.

        int iVar3;
        iVar3 = keaPoolAlloc(size, "Achol");     /* returns void * */

     There is no cast anywhere; the pointer is narrowed by the ASSIGNMENT. On
     i386 that is lossless and clang is silent unless asked. Asked, it is exact:

        warning: incompatible pointer to integer conversion assigning to 'int'
                 from 'void *' [-Wint-conversion]

  B. THE ACCESS TYPE IS NARROW.

        *(undefined4 *)&this->mLP = *(undefined4 *)&this->matrix;

     `mLP` is a `MeReal *`. At LP64 that store writes FOUR of its EIGHT bytes,
     so the field keeps whatever was in its top half — and the value reads back
     as a pointer with the high word missing.

★ WHAT B LOOKS LIKE WHEN IT FAILS, because the report is three files away.
`keaMatrix_pcSparse_vanilla::allocate` half-writes `mLP` and `mcLP`;
`makeFromJMJT` reads them back and hands the result to
`Multiply46BlockBy46BlockTranspose`, which stores through it:

    SEGV on unknown address 0x00009fcf60b8 ... rbx = 0x00000000fe7f05c0
    #0 Multiply46BlockBy46BlockTranspose keaMatrix_PcSparse_vanilla.c:1181

`rbx` is a live pointer with its top 32 bits gone. That register, not the line
number, is what identifies this class — the faulting function is correct code.

THE REPAIR IS A TYPE SUBSTITUTION AND NOTHING ELSE, which is what makes it
byte-identical on the shipped target: `kd_iptr` IS `int` at 32-bit pointer
width, not merely the same size, so every rewritten declaration and every
rewritten access means exactly what it meant before. Verified per file all the
same.

  A:  int iVar3;                  ->  kd_iptr iVar3;
  B:  *(undefined4 *)&this->mLP   ->  *(kd_uptr *)&this->mLP

SIGNEDNESS IS PRESERVED — `uint`/`undefined4` become `kd_uptr` and `int`
becomes `kd_iptr` — because a comparison downstream changes meaning otherwise.
The map is `fix_ptrwidth.WIDEN`, imported rather than copied.

⚠ B IS DECIDED BY MEASURING THE TARGET, NOT BY ITS NAME. `*(int *)&this->m_blocks`
has the identical shape and is CORRECT: `m_blocks` is an `int` on every target.
The tool resolves the lvalue to a declared member type and rewrites only where
that type is a POINTER — 153 sites have the shape and the ones that matter are
the ones whose target grows.

⚠ AND A WIDENED LOCAL IS WIDER FOR EVERY USE IT HAS. That is the intent — the
variable holds an address throughout — but Ghidra does reuse a name, so the
gate is the i386 object and the honest reading of a decline is "this local is
not only a pointer".

Run it on a COPY: it edits in place, AFTER `fix_ptrwidth.py`.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fix_ptrwidth                                          # noqa: E402  (WIDEN, CC, cflags)

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = '/tmp/kd_narrowptr'

BANNER = re.compile(r'(?m)^/\* ---- (\S+)')
CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']

NARROW = '|'.join(re.escape(k) for k in sorted(fix_ptrwidth.WIDEN, key=len, reverse=True))
# B: `*(NARROW *)&lvalue`
ACCESS = re.compile(r'\*\(\s*(?P<ty>' + NARROW + r')\s*\*\s*\)\s*&\s*'
                    r'(?P<lv>[A-Za-z_]\w*(?:(?:->|\.)[A-Za-z_]\w*)*)')
DIAG = re.compile(r'^(?P<file>[^:]+):(?P<line>\d+):(?P<col>\d+): warning: '
                  r'incompatible pointer to integer conversion assigning to '
                  r"'(?P<ty>[^']*)'")
# The OTHER direction, and it is the bigger half: `(T *)iVar10` where `iVar10`
# is an `int` that has been holding an address for three statements. This is
# what `fix_ptrwidth.py` reports as "N diagnostic(s) remain" — it rewrites the
# CAST at the column clang names, and here the cast is already as wide as it
# can be; what is narrow is the VARIABLE.
DIAG_TOPTR = re.compile(r'^(?P<file>[^:]+):(?P<line>\d+):(?P<col>\d+): warning: '
                        r"cast to '[^']*' .*?from smaller integer type "
                        r"'(?P<ty>[^']*)'")
CAST_THEN_VAR = re.compile(r'\(\s*[A-Za-z_][\w ]*\**\s*\)\s*(?P<var>[A-Za-z_]\w*)'
                           r'\s*(?=[;,)\]]|$)')
# A member declaration inside a struct body, with its type.
MEMBER = re.compile(r'(?m)^[ \t]+(?P<ty>(?:const\s+|struct\s+|unsigned\s+)*[A-Za-z_]\w*'
                    r'[ \t]*\**)[ \t]*(?P<name>[A-Za-z_]\w*)[ \t]*(?:\[[^\]]*\])?[ \t]*;')

_BODIES = {}
_TD = {}


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def headers(inc):
    """Every struct body and every typedef, over kd_types.h and the SDK."""
    if _BODIES:
        return _BODIES, _TD
    for root in (os.path.join(HERE, 'include'), inc):
        for dirpath, _d, files in os.walk(root):
            for fn in sorted(files):
                if not fn.endswith('.h'):
                    continue
                txt = open(os.path.join(dirpath, fn), errors='ignore').read()
                for m in re.finditer(r'\bstruct\s+(\w+)\s*\{', txt):
                    depth, i = 0, m.end() - 1
                    while i < len(txt):
                        if txt[i] == '{':
                            depth += 1
                        elif txt[i] == '}':
                            depth -= 1
                            if depth == 0:
                                break
                        i += 1
                    _BODIES.setdefault(m.group(1), txt[m.end():i])
                for m in re.finditer(r'(?m)^\s*typedef\s+([A-Za-z_][\w \t*]*?)'
                                     r'\s*(?P<stars>\**)\s*(\w+)\s*;', txt):
                    _TD.setdefault(m.group(3), (re.sub(r'\s+', ' ', m.group(1)).strip(),
                                                len(m.group('stars'))))
    return _BODIES, _TD


def tag_of(ty, inc):
    """A declared type spelling -> the struct tag it names, or None."""
    bodies, td = headers(inc)
    t = re.sub(r'\s+', ' ', ty or '').replace('*', '').replace('const ', '')
    t = t.replace('struct ', '').strip()
    for _ in range(8):
        if t in bodies:
            return t
        if t not in td:
            return None
        nxt = td[t][0].replace('struct ', '').strip()
        if nxt == t:
            return None
        t = nxt
    return None


def is_pointer(ty, inc):
    """Does this type spelling denote a POINTER, typedefs followed?"""
    bodies, td = headers(inc)
    t = re.sub(r'\s+', ' ', ty or '').strip()
    if '*' in t:
        return True
    t = t.replace('const ', '').replace('struct ', '').strip()
    for _ in range(8):
        if t in bodies or t not in td:
            return False
        under, stars = td[t]
        if stars:
            return True
        nxt = under.replace('struct ', '').strip()
        if '*' in under:
            return True
        if nxt == t:
            return False
        t = nxt
    return False


def member_type(tag, name, inc):
    bodies, _td = headers(inc)
    for m in MEMBER.finditer(bodies.get(tag, '')):
        if m.group('name') == name:
            return re.sub(r'\s+', ' ', m.group('ty')).strip()
    return None


def region_of(text, pos):
    start, end = 0, len(text)
    for m in BANNER.finditer(text):
        if m.start() <= pos:
            start = m.start()
        elif m.start() > pos:
            end = m.start()
            break
    return text[start:end]


DECL = r'(?m)^[ \t]*(?P<ty>%s)[ \t]+(?P<name>%s)[ \t]*;'


def declared_type(region, var):
    for m in re.finditer(r'(?m)^[ \t]*((?:const |struct )*[A-Za-z_]\w*[ \t]*\**)'
                         r'[ \t]*' + re.escape(var) + r'[ \t]*;', region):
        ty = re.sub(r'\s+', ' ', m.group(1)).strip()
        if ty.split()[0] not in ('return', 'goto', 'break', 'continue'):
            return ty
    sig = re.search(r'(?s)^/\* ---- .*?\*/\s*\n(.*?)\{', region)
    if sig:
        for p in re.split(r',', sig.group(1)[sig.group(1).find('(') + 1:]):
            m = re.match(r'\s*((?:const |struct )*[A-Za-z_]\w*[ \t]*\**)\s*'
                         + re.escape(var) + r'\s*\)?\s*$', p)
            if m:
                return re.sub(r'\s+', ' ', m.group(1)).strip()
    return None


def lvalue_type(text, pos, lv, inc):
    """The declared type of `a->b.c`, following each member in turn."""
    parts = re.split(r'->|\.', lv)
    region = region_of(text, pos)
    ty = declared_type(region, parts[0])
    for name in parts[1:]:
        tag = tag_of(ty, inc)
        if not tag:
            return None
        ty = member_type(tag, name, inc)
        if not ty:
            return None
    return ty


def compiles_identically(fn, text, build, inc):
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


def accept_edits(fn, text, edits, build, inc):
    if not edits:
        return text, []
    cand = text
    for start, end, rep, _n in edits:            # descending by position
        cand = cand[:start] + rep + cand[end:]
    if compiles_identically(fn, cand, build, inc):
        return cand, edits
    if len(edits) == 1:
        return text, []
    mid = len(edits) // 2
    text, a = accept_edits(fn, text, edits[:mid], build, inc)
    text, b = accept_edits(fn, text, edits[mid:], build, inc)
    return text, a + b


def narrow_locals(path, cf):
    """(line, destination type, variable) for every pointer narrowed by an
    assignment, and every narrow local cast BACK to a pointer — clang's
    judgement in both directions, not a pattern."""
    r = subprocess.run([fix_ptrwidth.CC] + cf + ['-Wint-conversion',
                                                 '-c', '-o', os.devnull, path],
                       capture_output=True, text=True)
    text = open(path, errors='ignore').read()
    lines = text.split('\n')
    out, unnamed = [], 0
    for line in r.stderr.splitlines():
        line = line.strip()
        m = DIAG.match(line)
        if m and os.path.abspath(m.group('file')) == os.path.abspath(path):
            ty = m.group('ty')
            if ty not in fix_ptrwidth.WIDEN:
                continue                        # a pointer target: the other half
            ln, col = int(m.group('line')), int(m.group('col'))
            if ln - 1 >= len(lines):
                continue
            lhs = lines[ln - 1][:col - 1].rstrip()
            v = re.search(r'(?<![\w.\]>])([A-Za-z_]\w*)$', lhs)
            if not v or (len(lhs) > len(v.group(1))
                         and lhs[-len(v.group(1)) - 1] in '.>])'):
                unnamed += 1
                continue                        # not a bare local; reported
            out.append((ln, ty, v.group(1)))
            continue
        m = DIAG_TOPTR.match(line)
        if not m or os.path.abspath(m.group('file')) != os.path.abspath(path):
            continue
        ty = m.group('ty')
        if ty not in fix_ptrwidth.WIDEN:
            continue
        ln, col = int(m.group('line')), int(m.group('col'))
        if ln - 1 >= len(lines):
            continue
        # The cast starts at the column clang names; what follows it has to be
        # a BARE variable, because that is the only operand whose declaration
        # this can widen. `(T *)(a + b)` is a different defect.
        v = CAST_THEN_VAR.match(lines[ln - 1], col - 1)
        if not v:
            unnamed += 1
            continue
        out.append((ln, ty, v.group('var')))
    return out, unnamed


WIDE = re.compile(r'\*\(\s*kd_[iu]ptr\s*\*\s*\)\s*&\s*'
                  r'(?P<lv>[A-Za-z_]\w*(?:(?:->|\.)[A-Za-z_]\w*)*)\s*=\s*'
                  r'(?P<rhs>[A-Za-z_]\w*)\s*;')
# E: the mirror of C. A local that READS a pointer-width value has to be
# pointer-width itself — `iVar2 = *(kd_iptr *)&this->NAZ;` truncates and the
# next `(int *)iVar2` sign-extends, which is how a live pointer comes back as
# 0xffffffffe5ff1a10 with its low half intact. `kd_iptr` -> `int` is an
# integer narrowing, so no compiler warns; what makes it evidence is that the
# RHS is *exactly* a pointer-width read, put there by rule B.
FROM_WIDE = re.compile(r'(?m)^[ \t]*(?P<var>[A-Za-z_]\w*)\s*=\s*'
                       r'(?:\*\(\s*kd_[iu]ptr\s*\*\s*\)|\(\s*kd_[iu]ptr\s*\))[^;]*;')
# F: an array OF POINTERS indexed with the i386 element size baked in.
#
#     *(MeReal **)(*(kd_iptr *)&this->NCZ + z * 4) = AcholMatrix;
#     *(undefined4 *)(*(kd_iptr *)&this->NAZ + z * 4) = 0;
#
# `NCZ` is a `MeReal **`, so its elements are EIGHT bytes at LP64 and the walk
# lands mid-element from the second one on. The second line is the same array
# with the access width wrong as well — `NAZ[z] = 0` writing four bytes of an
# eight-byte slot — so both the stride and the type have to move.
#
# The element type is not guessed: either the cast itself is `T **`, which says
# the element is a `T *`, or the base is a pointer-width read of an lvalue this
# can resolve to a pointer-to-pointer.
ELEM = re.compile(r'\*\(\s*(?P<acc>[A-Za-z_][\w ]*\**)\s*\*\s*\)\s*\(')
BASE_READ = re.compile(r'^\s*\*\(\s*kd_[iu]ptr\s*\*\s*\)\s*&\s*'
                       r'(?P<lv>[A-Za-z_]\w*(?:(?:->|\.)[A-Za-z_]\w*)*)')
I386_PTR = 4
# G: the ARRAY OF POINTERS is ALLOCATED at four bytes an element.
#
#     uVar2 = keaPoolAlloc((m_blocks * m_blocks + 0xf & 0xfffffff0) << 2, "NAZ");
#     *(kd_uptr *)&this->NAZ = uVar2;
#
# `<< 2` is `* sizeof(void *)` at i386, so at LP64 the block is HALF the size
# the eight-byte walk needs. Repairing the stride (F) without this makes it
# worse, not better: the walk is then correct and runs off the end, and what it
# reads back is a NULL where a matrix block should be —
#
#     SEGV on address 0x0 ... in makeFromJMJT ...:304   *AcholMatrix = ...
#
# which is three hundred lines from the allocation and looks like a logic bug.
# ⚠ THE SHIFT'S LEFT OPERAND MUST ALREADY BE PARENTHESISED: `<<` binds looser
# than `*`, so rewriting `A + B << 2` as `A + B * sizeof(void *)` would change
# the arithmetic. A site that is not parenthesised is declined and reported.
ALLOC_OF = re.compile(r'(?m)^[ \t]*(?P<var>[A-Za-z_]\w*)\s*=\s*[A-Za-z_]\w*\s*\(')
SHIFT2 = re.compile(r'\)\s*(?P<sh><<\s*2)(?![0-9])')


PTR_ARRAY_DECL = re.compile(r'(?m)^\s*(?:extern\s+|static\s+)?[A-Za-z_][\w ]*\*\s*'
                            r'(?P<n>[A-Za-z_]\w*)\s*\[\s*\d*\s*\]\s*;')
BYTE_BASE = re.compile(r'\(\s*char\s*\*\s*\)\s*(?P<arr>[A-Za-z_]\w*)\b')
CONST_ADDEND = re.compile(r'\+\s*\(\s*(?P<k>-?\d+)\s*\)')


def pointer_array_sites(text, inc):
    """Every array of pointers walked at the i386 element size, as edits.

    Three ways the element type is established, none of them a guess: the cast
    is `T **`; the base is a pointer-width read of an lvalue that resolves to a
    pointer-to-pointer; or the base is a byte cast of a symbol this file
    DECLARES as an array of pointers —

        pool_ptr = *(undefined4 *)((char *)poolstack + (-4) + poolstack_ptr * 4);

    `void *poolstack[3]`, so that is `poolstack[poolstack_ptr - 1]` with the
    element size, the element offset AND the access width all frozen at four.
    It is the pool-frame restore, so at LP64 every allocation after the first
    `keaPopPoolFrame` comes back with its top half gone — on the second
    simulation step, which is why the first one looked clean."""
    out = []
    arrays = {m.group('n') for m in PTR_ARRAY_DECL.finditer(text)}
    for m in ELEM.finditer(text):
        i, d = m.end() - 1, 0
        while i < len(text):
            if text[i] == '(':
                d += 1
            elif text[i] == ')':
                d -= 1
                if d == 0:
                    break
            i += 1
        if i >= len(text):
            continue
        inner = text[m.end():i]
        k = re.search(r'\*\s*(?P<k>\d+)\s*$', inner)
        if not k or int(k.group('k')) != I386_PTR:
            continue
        if inner[:k.start()].count('(') != inner[:k.start()].count(')'):
            continue                            # the `* 4` is not at top level
        acc = re.sub(r'\s+', ' ', m.group('acc')).strip()
        by_symbol = False
        if acc.endswith('*'):                   # the cast IS `T **`
            elem_ptr = True
        else:
            b = BASE_READ.match(inner)
            ty = lvalue_type(text, m.start(), b.group('lv'), inc) if b else None
            elem_ptr = bool(ty and ty.replace(' ', '').count('*') >= 2)
            if not elem_ptr:
                # ... or the base is a LOCAL that read one. `local_48` is a
                # `kd_iptr` assigned from `*(kd_iptr *)&this->NCZ` four lines
                # up, so `*(int *)(local_48 + k * 4)` is the same array walk
                # with the base held in a variable.
                v = re.match(r'\s*(?P<v>[A-Za-z_]\w*)\s*\+', inner)
                if v:
                    region = region_of(text, m.start())
                    for a in re.finditer(r'(?m)^[ \t]*' + re.escape(v.group('v'))
                                         + r'\s*=\s*\*\(\s*kd_[iu]ptr\s*\*\s*\)\s*&\s*'
                                         r'(?P<lv>[A-Za-z_]\w*(?:(?:->|\.)[A-Za-z_]\w*)*)',
                                         region):
                        t2 = lvalue_type(text, m.start(), a.group('lv'), inc)
                        if t2 and t2.replace(' ', '').count('*') >= 2:
                            elem_ptr = True
                            break
            if not elem_ptr:
                a = BYTE_BASE.search(inner)
                elem_ptr = by_symbol = bool(a and a.group('arr') in arrays)
        if not elem_ptr:
            continue
        out.append((m.end() + k.start('k'), m.end() + k.end('k'),
                    '(int)sizeof(void *)', 'stride %s x%d' % (acc, I386_PTR)))
        if acc in fix_ptrwidth.WIDEN:
            out.append((m.start('acc'), m.end('acc'), fix_ptrwidth.WIDEN[acc],
                        'elem %s -> %s' % (acc, fix_ptrwidth.WIDEN[acc])))
        if by_symbol:
            # The element OFFSET is in bytes too: `+ (-4)` is `- 1` element.
            for cm in CONST_ADDEND.finditer(inner):
                v = int(cm.group('k'))
                if v % I386_PTR:
                    continue
                out.append((m.end() + cm.start('k'), m.end() + cm.end('k'),
                            '%d * (int)sizeof(void *)' % (v // I386_PTR),
                            'offset %+d -> %+d elements' % (v, v // I386_PTR)))
    return out


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else os.path.join(
        HERE, '..', 'Thirdparty', 'metoolkit')
    inc = os.path.join(root, 'include')
    if not os.path.exists(fix_ptrwidth.CC):
        print('fix_narrow_pointers: no NDK at %s — set KD_NDK.' % fix_ptrwidth.NDK,
              file=sys.stderr)
        return 2
    cf = fix_ptrwidth.cflags(root)

    # ---- SELF-CHECK. A `void *` member is a pointer and an `int` member is not;
    # if the type resolver cannot tell them apart it declines everything and
    # prints a zero that reads exactly like "there is nothing here".
    probes = [('keaMatrix_pcSparse', 'mLP', True),
              ('keaMatrix_pcSparse', 'm_blocks', False),
              ('MeDictNode', 'data', True)]
    for tag, name, want in probes:
        got = is_pointer(member_type(tag, name, inc) or '', inc)
        if got != want:
            sys.exit('fix_narrow_pointers: SELF-CHECK FAILED — %s::%s reads as %s '
                     '(type %r), want %s. The type resolver is not resolving.'
                     % (tag, name, got, member_type(tag, name, inc), want))

    fixed_a = fixed_b = fixed_c = fixed_f = declined = unnamed = 0
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        # ⚠ ROUNDS, because the rules FEED each other: widening an access
        # (B) is what reveals that the value being stored through it is a
        # narrow local (C), and widening that local can reveal another. A
        # single pass leaves `*(kd_uptr *)&this->matrixChol = uVar1;` with
        # `uVar1` still four bytes wide, which is the same defect one step
        # back and reads as a clean run.
        for _round in range(6):
            text0 = open(path, errors='ignore').read()
            lines = text0.split('\n')
            starts, n = [], 0
            for ln in lines:                    # byte offset of each line
                starts.append(n)
                n += len(ln) + 1

            edits, kinds, seen = [], {}, set()

            def want(s, e, rep, note, kind):
                if s in seen:
                    return
                seen.add(s)
                edits.append((s, e, rep, note))
                kinds[s] = kind

            # ---- A: the narrow local, named by clang
            locs, un = narrow_locals(path, cf)
            if _round == 0:
                unnamed += un
            for ln, ty, var in locs:
                region = region_of(text0, starts[ln - 1])
                base = text0.index(region)
                d = re.search(DECL % (re.escape(ty), re.escape(var)), region)
                if not d:
                    notes.append('%-26s %s (line %d) is %s but has no local '
                                 'declaration here — declined' % (fn, var, ln, ty))
                    continue
                want(base + d.start('ty'), base + d.end('ty'), fix_ptrwidth.WIDEN[ty],
                     '%-26s local  %-14s %-12s -> %s'
                     % (fn, var, ty, fix_ptrwidth.WIDEN[ty]), 'A')
            # ---- B: the narrow access type
            for m in ACCESS.finditer(text0):
                ty = lvalue_type(text0, m.start(), m.group('lv'), inc)
                if ty is None or not is_pointer(ty, inc):
                    continue                    # an int member IS four bytes
                want(m.start('ty'), m.end('ty'), fix_ptrwidth.WIDEN[m.group('ty')],
                     '%-26s access %-14s %-12s -> %s'
                     % (fn, m.group('lv'), m.group('ty'),
                        fix_ptrwidth.WIDEN[m.group('ty')]), 'B')
            # ---- C: the VALUE stored into a pointer must be pointer-wide too
            for m in WIDE.finditer(text0):
                var = m.group('rhs')
                region = region_of(text0, m.start())
                base = text0.index(region)
                ty = declared_type(region, var)
                if ty not in fix_ptrwidth.WIDEN:
                    continue
                d = re.search(DECL % (re.escape(ty), re.escape(var)), region)
                if not d:
                    continue
                want(base + d.start('ty'), base + d.end('ty'), fix_ptrwidth.WIDEN[ty],
                     '%-26s value  %-14s %-12s -> %s (stored into %s)'
                     % (fn, var, ty, fix_ptrwidth.WIDEN[ty], m.group('lv')), 'C')
            # ---- E: a local that READS a pointer-width value. See FROM_WIDE.
            for m in FROM_WIDE.finditer(text0):
                var = m.group('var')
                region = region_of(text0, m.start())
                base = text0.index(region)
                ty = declared_type(region, var)
                if ty not in fix_ptrwidth.WIDEN:
                    continue
                d = re.search(DECL % (re.escape(ty), re.escape(var)), region)
                if not d:
                    continue
                want(base + d.start('ty'), base + d.end('ty'), fix_ptrwidth.WIDEN[ty],
                     '%-26s read   %-14s %-12s -> %s'
                     % (fn, var, ty, fix_ptrwidth.WIDEN[ty]), 'C')
            # ---- F: an array of pointers walked four bytes at a time
            for s, e, rep, label in pointer_array_sites(text0, inc):
                want(s, e, rep, '%-26s %s' % (fn, label), 'F')
            # ---- G: the allocation behind such an array. See ALLOC_OF.
            for m in WIDE.finditer(text0):
                lv, var = m.group('lv'), m.group('rhs')
                ty = lvalue_type(text0, m.start(), lv, inc)
                if not ty or ty.replace(' ', '').count('*') < 2:
                    continue
                region = region_of(text0, m.start())
                base = text0.index(region)
                a = None
                for x in ALLOC_OF.finditer(region, 0, m.start() - base):
                    if x.group('var') == var:
                        a = x
                if not a:
                    continue
                i, d = a.end() - 1, 0
                while i < len(region):
                    if region[i] == '(':
                        d += 1
                    elif region[i] == ')':
                        d -= 1
                        if d == 0:
                            break
                    i += 1
                sh = SHIFT2.search(region, a.end(), i)
                if not sh:
                    notes.append('%-26s %s is a pointer array but its allocation '
                                 'has no `) << 2` to widen — declined' % (fn, lv))
                    continue
                want(base + sh.start('sh'), base + sh.end('sh'),
                     '* (int)sizeof(void *)',
                     '%-26s alloc  %-14s << 2 -> * sizeof(void *)' % (fn, lv), 'F')

            if not edits:
                break
            edits.sort(key=lambda e: -e[0])
            text, ok = accept_edits(fn, text0, edits, build, inc)
            for e in ok:
                if kinds[e[0]] == 'A':
                    fixed_a += 1
                elif kinds[e[0]] == 'B':
                    fixed_b += 1
                elif kinds[e[0]] == 'F':
                    fixed_f += 1
                else:
                    fixed_c += 1
            declined += len(edits) - len(ok)
            keep = {id(e) for e in ok}
            for e in edits:
                notes.append(e[3] + ('' if id(e) in keep
                                     else '   DECLINED: not byte-identical at i386'))
            if text == text0:
                break
            open(path, 'w').write(text)

    print('  narrow locals holding a pointer widened  : %d' % fixed_a)
    print('  narrow accesses to a pointer widened     : %d' % fixed_b)
    print('  narrow values stored into a pointer      : %d' % fixed_c)
    print('  pointer arrays walked at 4 bytes         : %d' % fixed_f)
    print('  declined (reported, not guessed)         : %d' % declined)
    if unnamed:
        print('  narrowed by an assignment to something that is not a local : %d'
              % unnamed)
    for n in notes:
        print('     %s' % n)
    return 0


if __name__ == '__main__':
    sys.exit(main())
