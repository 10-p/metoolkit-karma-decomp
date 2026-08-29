#!/usr/bin/env python3
"""fix_literal_offsets.py — a struct field addressed by a BAKED BYTE OFFSET.

    fix_literal_offsets.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT.

    *(code **)((char *)pMVar1 + 200) = McdModelUpdatePathCompatible;
    McdModelPairManagerSetPoolFullHandler(*(void **)((char *)pMVar1 + 0x2c), ...);

200 and 0x2c are `mAABBUpdateFn` and `mManager` of `CxSmallSort` — ON i386. At
LP64 they are 392 and 80, so both statements address someone else's memory with
nothing truncated and clang silent. This is the class `layout_check.py` counts in
its OFFSET column and the one `ptrwidth_check.sh` is blind to by construction.

THE REPAIR IS A PURE ADDRESS RE-SPELLING, which is what makes it safe:

    *(T *)((char *)p + K)   ->   *(T *)&p->FIELD

The cast and therefore the expression's TYPE are untouched — only the way the
address is computed changes, from a number to a field the compiler recomputes
per target. i386 stays byte-identical by construction, and that is verified per
site rather than asserted.

⚠ THE DECLARED TYPE IS OFTEN THE WRONG ONE, and this is the fourth face of the
same problem (after the geometry allocations, the constraint pool and
`fix_baked_sizeof`'s targets). `pMVar1` is declared `McdSpaceID`, i.e.
`McdSpace *`, and `McdSpace` is an OPAQUE handle — the object really is a
`CxSmallSort`. So the concrete type is inferred from the offsets themselves: the
struct on which EVERY offset used against that declared type in this file lands
on a real field. For McdSpace.c that is {4, 0xc, 0x18, 0x1c, 0x24, 0x2c, 0xc8,
0xcc} and CxSmallSort is the only complete type that covers them.

AND IT IS CONFIRMED AGAINST A BUILD NOBODY HERE MADE. The shipped amd64
metoolkit's `McdSpaceAxisSortCreate` touches displacements 0x50 and 0x188 —
which are exactly `mManager` and `mAABBUpdateFn` at 64-bit, measured from
`kd_types.h`'s own declaration. Two independent 64-bit sources agreeing on a
layout the i386 build cannot express.

SCOPE: 27 sites of this shape in the corpus, 21 in McdSpace.c and 6 in
ReadWriteKeaInputToFile.c. ⚠ `layout_check.py` reports 128 in its OFFSET column
because that count ALSO includes Ghidra's invented stack frames
(`(int)&local + K`, MdtBcl 70 and MdtMainLoop 25). Those are a decompilation
defect, not a layout one, and this tool deliberately does not touch them.

Run it on a COPY: it edits in place. Before `fix_ptrwidth.py`, which would
otherwise rewrite these casts into a shape this no longer recognises.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = '/tmp/kd_offsets'

# `*(T *)((char *)BASE + K)` — and the `(int)`/`(kd_iptr)` spellings of the same.
SITE = re.compile(
    r'\*\((?P<ty>[A-Za-z_]\w*(?:\s+\w+)*\s*\**)\s*\*\)'
    r'\(\((?:char|int|kd_iptr)\s*\*?\)(?P<base>[A-Za-z_]\w*(?:->[A-Za-z_]\w*)?)'
    r'\s*\+\s*(?P<off>0x[0-9a-f]+|\d+)\)')

# ★ THE BASE IS NOT ALWAYS A VARIABLE, and the one that is not was the defect
# that kept scene_chain failing after everything else was repaired:
#
#     if ((*(byte *)((int)pMVar9->data + 0x1ec) & 2) == 0) goto ...
#
# 0x1ec is 492 is `offsetof(MdtBody, flags)` at i386 and 556 at LP64, so the
# partitioner's VISITED test reads a byte 64 short of the flag. No body is ever
# seen as visited, the root loop re-seeds one already placed, and the failure
# shows up as `addedBodies[12]` overflowing a 12-element buffer — a count
# overrun that looks nothing like a layout defect and had me diagnose it as a
# bookkeeping bug twice.
#
# `pMVar9->data` is a `void *` member, so it carries no type of its own. What
# names it is the SAME FIELD read into a typed local elsewhere in the function:
# `pMVar3 = rootNode->data;` with `pMVar3` declared `MdtBody *`. The field is
# the key, not the variable — `rootNode`, `node` and `pMVar9` are three names
# for the same kind of node.
FIELD_READ = re.compile(r'(?m)^[ \t]*(?P<var>[A-Za-z_]\w*)\s*=\s*'
                        r'[A-Za-z_]\w*->(?P<f>[A-Za-z_]\w*)\s*;')

BANNER = re.compile(r'(?m)^/\* ---- (\S+)')
CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']
_SZ = re.compile(r'char \(\*\)\[(\d+)\]')


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


HEAD = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
        '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))


def measure(expr, inc, cache):
    """A compile-time constant, read out of gcc's own type printer."""
    if expr in cache:
        return cache[expr]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p.c')
    open(src, 'w').write(HEAD + 'char kd_probe[%s];\nint kd_force = &kd_probe;\n' % expr)
    r = subprocess.run(['gcc', '-m32', '-DLINUX'] + includes(inc)
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    m = _SZ.search(r.stderr)
    cache[expr] = int(m.group(1)) if m else None
    return cache[expr]


# `TYPE NAME;` / `TYPE NAME[3];` inside a struct body. Bit-fields, function
# pointers and anonymous members are deliberately NOT matched — a member this
# cannot name is a member the offset map must not claim to cover.
MEMBER = re.compile(r'(?m)^[ \t]+(?:const\s+|struct\s+|unsigned\s+)*[A-Za-z_]\w*'
                    r'[ \t]*\**[ \t]*(?P<name>[A-Za-z_]\w*)[ \t]*(?:\[\d*\])?[ \t]*;')

_BODIES = {}


def struct_bodies(inc):
    """tag -> the text between its braces, for every struct with a body."""
    if _BODIES:
        return _BODIES
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
    return _BODIES


_OFFSETS = {}


def offsets_of(tag, inc, cache):
    """i386 byte offset -> member name, for one struct tag."""
    if tag in _OFFSETS:
        return _OFFSETS[tag]
    body = struct_bodies(inc).get(tag)
    out = {}
    if body:
        for m in MEMBER.finditer(body):
            name = m.group('name')
            off = measure('((char *)&((struct %s *)0)->%s - (char *)0) + 1'
                          % (tag, name), inc, cache)
            if off is not None:
                out[off - 1] = name
    _OFFSETS[tag] = out
    return out


def region_of(text, pos):
    start, end = 0, len(text)
    for m in BANNER.finditer(text):
        if m.start() <= pos:
            start = m.start()
        elif m.start() > pos:
            end = m.start()
            break
    return text[start:end]


def enclosing(text, pos):
    last = None
    for m in BANNER.finditer(text, 0, pos):
        last = m.group(1)
    return last


def declared_type(region, var):
    """The type of a local OR a parameter.

    ⚠ PARAMETERS ARE NOT DECLARATIONS. Ghidra writes locals as `T x;` at the top
    of the body and parameters in the signature, so a locals-only lookup found
    `pMVar1` in McdSpaceAxisSortCreate and missed `s` and `space` in every other
    function of the same file — two of twenty-one sites repaired, and the rest
    silently 'declined' for want of a type that was written down three lines
    above them."""
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


# A base whose declared type carries no structure tells us nothing, and a
# thousand of them reported as "declined" buries the sites that could be
# repaired. They are out of scope, not refused.
OPAQUE = re.compile(r'^(void|undefined\d*|int|uint|char|float|double|code'
                    r'|MeReal|MeU\d+|MeI\d+)\b')


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


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else os.path.join(
        HERE, '..', 'Thirdparty', 'metoolkit')
    inc = os.path.join(root, 'include')
    cache = {}

    # ---- THE SELF-CHECK. If the offset probe cannot compile it returns None
    # for everything, every site declines, and "0 repaired" reads exactly like
    # "there was nothing to repair".
    probe = offsets_of('CxSmallSort', inc, cache)
    if probe.get(0x2c) != 'mManager' or probe.get(200) != 'mAABBUpdateFn':
        sys.exit('fix_literal_offsets: SELF-CHECK FAILED — CxSmallSort+0x2c read %r '
                 '(want mManager) and +200 read %r (want mAABBUpdateFn). The offset '
                 'map is not measuring anything.'
                 % (probe.get(0x2c), probe.get(200)))

    tags = [t for t in struct_bodies(inc) if t not in ('_IO_FILE',)]
    fixed = declined = skipped = 0
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        sites = list(SITE.finditer(text))
        if not sites:
            continue

        # ---- Which concrete struct is each declared type, in THIS file?
        # The declared type is frequently an opaque handle, so the offsets it is
        # used with are the evidence: the tag on which every one of them lands
        # on a real field. Requiring ALL of them is what makes it a measurement
        # rather than a guess — a single offset would fit dozens of structs.
        # For a `X->F` base the type of the POINTER tells us nothing (`data` is
        # a `void *`); what names it is the same field read into a typed local.
        def base_type(m):
            b = m.group('base')
            region = region_of(text, m.start())
            if '->' not in b:
                return declared_type(region, b)
            fld = b.split('->')[1]
            for r in FIELD_READ.finditer(region):
                if r.group('f') == fld:
                    t = declared_type(region, r.group('var'))
                    if t and not OPAQUE.match(t):
                        return t
            return None

        used = {}
        for m in sites:
            ty = base_type(m)
            if ty and not OPAQUE.match(ty):
                used.setdefault(ty, set()).add(int(m.group('off'), 0))
        concrete = {}
        for ty, offs in used.items():
            fits = [t for t in tags
                    if all(o in offsets_of(t, inc, cache) for o in offs)]
            # A declared type that IS the concrete type wins outright.
            direct = ty.replace('struct ', '').replace('*', '').strip()
            if direct in fits:
                concrete[ty] = direct
            elif len(fits) == 1:
                concrete[ty] = fits[0]
            elif fits:
                notes.append('%-26s %s: %d offsets fit %d types (%s) — declined'
                             % (fn, ty, len(offs), len(fits), ', '.join(fits[:4])))

        edits = []
        for m in sites:
            ty = base_type(m)
            tag = concrete.get(ty)
            off = int(m.group('off'), 0)
            if not ty or OPAQUE.match(ty):
                skipped += 1
                continue
            if not tag:
                declined += 1
                continue
            field = offsets_of(tag, inc, cache).get(off)
            if not field:
                declined += 1
                notes.append('%-26s %s+0x%x is not a field start of %s'
                             % (fn, ty, off, tag))
                continue
            # ⚠ THE CAST MAY ONLY BE ELIDED FOR A BARE VARIABLE. For a `X->F`
            # base the type was INFERRED — the member itself is a `void *` — so
            # dropping the cast emits `(pMVar9->data)->flags`, which does not
            # compile, and the site declines for a reason that looks like the
            # repair being wrong rather than the spelling.
            cast = '(struct %s *)' % tag
            if '->' not in m.group('base') and \
                    ty.replace('struct ', '').replace('*', '').strip() == tag:
                cast = ''
            rep = '*(%s*)&(%s%s)->%s' % (m.group('ty'), cast, m.group('base'), field)
            edits.append((m.start(), m.end(), rep,
                          '%-26s %-14s +0x%-4x -> %s->%s'
                          % (fn, tag, off, m.group('base'), field)))

        for start, end, rep, note in sorted(edits, key=lambda e: -e[0]):
            cand = text[:start] + rep + text[end:]
            if compiles_identically(fn, cand, build, inc):
                text = cand
                fixed += 1
                notes.append(note)
            else:
                declined += 1
                notes.append(note + '   DECLINED: not byte-identical at i386')
        if edits:
            open(path, 'w').write(text)

    print('  baked field offsets rewritten as named fields : %d' % fixed)
    print('  declined (reported, not guessed)              : %d' % declined)
    print('  out of scope (base type carries no structure) : %d' % skipped)
    for n in notes:
        print('     %s' % n)
    return 0


if __name__ == '__main__':
    sys.exit(main())
