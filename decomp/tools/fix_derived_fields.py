#!/usr/bin/env python3
"""fix_derived_fields.py — a DERIVED struct's field addressed as an index past its BASE.

    fix_derived_fields.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT, and it is the one MCD-GEOM-FLOAT-FIELDS repaired the TYPE half of:

    *(float *)&(g[1].mRefCtAndID)          is  ((McdSphere *)g)->mRadius
    pMVar4[1].prev = (McdGeometryID)...    is  ((McdBox *)pMVar4)->mR[1] = ...

UT2004's geometry types derive from `McdGeometry` by PREFIXING it, and Ghidra
has only `McdGeometry *`. So it addresses a derived field as an index past the
base struct: byte `k * sizeof(McdGeometry) + offsetof(McdGeometry, FIELD)`,
which on i386 is `k * 16 + {0,4,8,12}` and lands exactly where the derived field
is. At LP64 `sizeof(McdGeometry)` is 32 and its fields are at {0,8,16,24}, so
BOTH halves move and they move differently from the derived struct's own layout:

    McdBox at LP64   m_g 0..32   mR[3] 32..44   mRadius 44..48
      g[1].mRefCtAndID  -> 32    mR[0]      correct, by luck
      g[1].prev         -> 40    mR[1] is at 36        WRONG
      g[1].next         -> 48    past the end of the struct
      g[1].frame        -> 56    past the end of the struct

★ AND THE ACCESS TYPE HAS TO CHANGE TOO, WHICH IS WHY THIS IS NOT A PURE
ADDRESS RE-SPELLING. The recovery stores through the BASE field's type —
`pMVar4[1].prev` is an `McdGeometryID`, a pointer — into what is really a
4-byte `MeReal`. On i386 both are four bytes and the bytes are what matters; at
LP64 the pointer is EIGHT and would overwrite the next field. So the repair
names the concrete field and lets its own type carry the store.

WHAT MAKES THIS SAFE: the i386 byte-identity check is STRONG for this class, in
a way it is not for `fix_baked_sizeof`. There, any type of the right size passed
the gate, which is how seven wrong ones shipped (`../proven.txt`
LP64-WRONG-TYPE). Here a wrong field means a different ADDRESS and a wrong type
means a different STORE WIDTH, and either changes the i386 object. So every
rewrite is compiled and compared, and a site that does not reproduce its
baseline object byte for byte is declined and reported.

THE CONCRETE TYPE is inferred the same way `fix_literal_offsets.py` does it: the
struct on which EVERY offset used against that base pointer in this file lands
on a real field, with the object's own name as the tie-break. It is confirmed
against the shipped amd64 build wherever that build has the function —
`McdSphereGetRadius` reads 0x20 there against 0x10 here, `McdBoxGetRadii` 0x20,
`McdTriangleListSetGenerator` 0x48.

Run it on a COPY: it edits in place. Before `fix_ptrwidth.py`.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = '/tmp/kd_derived'

# The base struct whose fields Ghidra indexes past. Only `_McdGeometry` occurs
# in this corpus; the shape is general and the table is the place to add more.
BASES = {'_McdGeometry': ('McdGeometry', ('mRefCtAndID', 'prev', 'next', 'frame'))}

CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']
BANNER = re.compile(r'(?m)^/\* ---- (\S+)')
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
    open(src, 'w').write(HEAD + 'char kd_probe[(%s) + 1];\nint kd_force = &kd_probe;\n' % expr)
    r = subprocess.run(['gcc', '-m32', '-DLINUX'] + includes(inc)
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    m = _SZ.search(r.stderr)
    cache[expr] = (int(m.group(1)) - 1) if m else None
    return cache[expr]


# `TYPE NAME;` / `TYPE NAME[3];` inside a struct body, with the type kept.
MEMBER = re.compile(
    r'(?m)^[ \t]+(?P<ty>(?:const\s+|struct\s+|unsigned\s+)*[A-Za-z_]\w*[ \t]*\**)'
    r'[ \t]*(?P<name>[A-Za-z_]\w*)[ \t]*(?P<arr>\[\d*\])?[ \t]*;')

_BODIES = {}


def struct_bodies(inc):
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
                # `typedef struct { ... } Name;` — the concrete geometry types
                # are all spelled this way and have no tag at all.
                for m in re.finditer(r'\btypedef\s+struct\s*(?:\w+\s*)?\{', txt):
                    depth, i = 0, m.end() - 1
                    while i < len(txt):
                        if txt[i] == '{':
                            depth += 1
                        elif txt[i] == '}':
                            depth -= 1
                            if depth == 0:
                                break
                        i += 1
                    tail = re.match(r'\s*(\w+)\s*;', txt[i + 1:])
                    if tail:
                        _BODIES.setdefault(tail.group(1), txt[m.end():i])
    return _BODIES


_PATHS = {}


def field_paths(tag, inc, cache, depth=0):
    """i386 byte offset -> a C path naming the field at it.

    ⚠ ARRAY-NESS IS MEASURED, NOT READ OFF THE DECLARATION. `MeVector3 center;`
    is a `MeReal[3]` behind a typedef, so a scan for `TYPE NAME[N];` records
    only its first element and the map silently has holes at +20 and +24. That
    made McdTriangleList fail to resolve at all — its nine offsets could not all
    land, so the concrete type came back ambiguous and all 70 sites declined.
    Asking the compiler for `sizeof(f)` and `sizeof(f[0])` gets it right whether
    the brackets are in the declaration or in the typedef.

    Recurses into a nested struct member (`mHull.vertex`) by composing offsets."""
    key = (tag, depth)
    if key in _PATHS:
        return _PATHS[key]
    out = {}
    _PATHS[key] = out                      # guard against a cyclic type
    body = struct_bodies(inc).get(tag)
    if not body:
        return out
    for m in MEMBER.finditer(body):
        name = m.group('name')
        ty = re.sub(r'\s+', ' ', m.group('ty')).strip()
        off = measure('((char *)&((%s *)0)->%s - (char *)0)' % (tag, name), inc, cache)
        msz = measure('sizeof(((%s *)0)->%s)' % (tag, name), inc, cache)
        if off is None or not msz:
            continue
        if '*' in ty:                      # a pointer is not an array of anything
            out.setdefault(off, name)
            continue
        # An ARRAY? `sizeof(f[0])` compiles for one and not for a scalar. The
        # `esz >= 2` floor is load-bearing: GNU C gives `sizeof(void)` and
        # `sizeof(function)` as 1, so a `void *`-ish or function-pointer member
        # would otherwise read as an array of its own byte count.
        #
        # ⚠ TESTED BEFORE THE BARE NAME IS RECORDED. Registering `mR` at +16 and
        # then `mR[0]` at the same offset lets the bare one win, and `p->mR` is
        # not an assignable lvalue. The self-check below exists because that is
        # exactly what the first version did.
        esz = measure('sizeof(((%s *)0)->%s[0])' % (tag, name), inc, cache)
        if esz and esz >= 2 and msz % esz == 0 and msz // esz >= 2:
            for i in range(msz // esz):
                out.setdefault(off + i * esz, '%s[%d]' % (name, i))
            continue
        out.setdefault(off, name)
        base = re.sub(r'^(?:const|struct|unsigned)\s+', '', ty).strip()
        if depth < 1 and base in struct_bodies(inc) and base != tag:
            for o, p in field_paths(base, inc, cache, depth + 1).items():
                out.setdefault(off + o, '%s.%s' % (name, p))
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

    # ---- THE SELF-CHECK. If the offset map cannot be measured every site
    # declines and "0 repaired" reads exactly like "nothing to repair".
    box = field_paths('McdBox', inc, cache)
    if box.get(16) != 'mR[0]' or box.get(20) != 'mR[1]' or box.get(28) != 'mRadius':
        sys.exit('fix_derived_fields: SELF-CHECK FAILED — McdBox+16/20/28 read '
                 '%r/%r/%r, want mR[0]/mR[1]/mRadius. The field map is not '
                 'measuring anything.'
                 % (box.get(16), box.get(20), box.get(28)))

    tags = sorted(struct_bodies(inc))
    paths_size = {}
    base_tag, (base_name, base_fields) = 'McdGeometry', BASES['_McdGeometry']
    bsz = measure('sizeof(*(McdGeometryID)0)', inc, cache)
    boff = {f: measure('((char *)&((McdGeometryID)0)->%s - (char *)0)' % f, inc, cache)
            for f in base_fields}
    if bsz != 16 or boff.get('frame') != 12:
        sys.exit('fix_derived_fields: SELF-CHECK FAILED — sizeof(McdGeometry) %r '
                 'and offsetof(frame) %r, want 16 and 12.' % (bsz, boff.get('frame')))

    ASSIGN = re.compile(r'(?P<site>[A-Za-z_]\w*\[[1-9]\d*\]\s*\.\s*(?:'
                        + '|'.join(base_fields) + r'))\s*=\s*(?P<val>[^;]+);')
    READ = re.compile(r'(?P<var>[A-Za-z_]\w*)\s*=\s*(?P<site>[A-Za-z_]\w*\[[1-9]\d*\]'
                      r'\s*\.\s*(?:' + '|'.join(base_fields) + r'))\s*;')
    SITE = re.compile(r'(?P<base>[A-Za-z_]\w*)\[(?P<k>[1-9]\d*)\]\s*\.\s*'
                      r'(?P<f>' + '|'.join(base_fields) + r')\b')

    fixed = declined = 0
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

        # ---- which concrete type is this base pointer, in THIS file?
        used = set()
        for m in sites:
            used.add(int(m.group('k')) * bsz + boff[m.group('f')])
        fits = [t for t in tags
                if all(o in field_paths(t, inc, cache) for o in used)
                and t != base_tag and t != '_' + base_tag]
        stem = fn[:-2]
        named = [t for t in fits if t == stem or stem.endswith(t)]
        concrete = named[0] if len(named) == 1 else (fits[0] if len(fits) == 1 else None)
        if not concrete:
            declined += len(sites)
            notes.append('%-26s %d site(s): %d concrete type(s) fit %s%s'
                         % (fn, len(sites), len(fits), sorted(used),
                            ' — ' + ', '.join(fits[:5]) if fits else ''))
            continue

        paths = field_paths(concrete, inc, cache)
        if concrete not in paths_size:
            paths_size[concrete] = measure('sizeof(%s)' % concrete, inc, cache)
        edits = []
        # ---- AN ASSIGNMENT HAS TO BE REWRITTEN WHOLE, because the value
        # carries the BASE field's type. `pMVar1[1].prev = (McdGeometryID)
        # KD_FBITS(x)` stores the BITS of a float through a pointer-typed
        # lvalue — fine at i386, where both are four bytes. Naming the concrete
        # float field and leaving the value alone would CONVERT the integer to
        # a float instead of reinterpreting it, which is a different program.
        # So the value is adapted too, and the three candidate spellings are
        # offered to the compiler rather than chosen by reasoning:
        #   a  the value already has the right type (an int field)
        #   b  `(T)KD_FBITS(EXPR)` — the laundering is no longer needed
        #   c  a variable holding the bits — reinterpret it through __typeof__
        # Whichever reproduces the baseline object is kept; if none does the
        # site is declined, so a wrong guess cannot ship.
        done = set()
        for m in ASSIGN.finditer(text):
            sm = SITE.match(text, m.start('site'))
            if not sm:
                continue
            off = int(sm.group('k')) * bsz + boff[sm.group('f')]
            fp = paths.get(off)
            if not fp:
                continue
            lhs = '((%s *)%s)->%s' % (concrete, sm.group('base'), fp)
            val = m.group('val').strip()
            reps = ['%s = %s;' % (lhs, val)]
            # re.S: `= (McdFrameworkID)KD_FBITS(\n SQRT(...))` wraps across
            # lines in McdBoxCreate, and without DOTALL the laundering is not
            # recognised and the site declines for a formatting reason.
            fb = re.match(r'^\([\w ]+\*?\)\s*KD_FBITS\s*\((?P<e>.*)\)$', val, re.S)
            if fb:
                reps.append('%s = %s;' % (lhs, fb.group('e').strip()))
            if re.fullmatch(r'[A-Za-z_]\w*', val):
                reps.append('%s = *(__typeof__(%s) *)&%s;' % (lhs, lhs, val))
            edits.append((m.start(), m.end(), reps,
                          '%-26s %-16s [%s].%-13s +%-3d -> %s ='
                          % (fn, concrete, sm.group('k'), sm.group('f'), off, fp)))
            done.add(sm.start())
        # ---- THE MIRROR CASE: a READ of the field into a bit-carrier local.
        # `pMVar1 = g[1].prev;` types pMVar1 as `McdGeometryID` because that is
        # what the BASE field is, and every later use spells it
        # `*(float *)&pMVar1`. The variable is a float in a pointer's clothing.
        # Naming the concrete field and assigning it plainly would CONVERT the
        # float to a pointer; re-laundering it through KD_FBITS keeps the bits,
        # which is what every use of the variable expects — and at 64-bit they
        # sit in the low four bytes, where `*(float *)&` finds them.
        for m in READ.finditer(text):
            sm = SITE.match(text, m.start('site'))
            if not sm or sm.start() in done:
                continue
            off = int(sm.group('k')) * bsz + boff[sm.group('f')]
            fp = paths.get(off)
            if not fp:
                continue
            rhs = '((%s *)%s)->%s' % (concrete, sm.group('base'), fp)
            var = m.group('var')
            # A third spelling, and it needs a BOUNDS GUARD to be honest.
            # `VAR = *(__typeof__(VAR) *)&field;` is byte-identical at i386 and
            # gives the right VALUE at LP64 — the variable is 8 bytes there, the
            # field is 4, and every use reads the low half through
            # `*(float *)&VAR`, which on little-endian is exactly the field. But
            # it READS 8 bytes from a 4-byte member, so it is only admissible
            # when the struct actually has 8 bytes left at that offset. For
            # McdBox's mR[1] and mR[2] it does; for a trailing field it would
            # not, and the guard is what keeps this from becoming a real
            # over-read somewhere else.
            room = paths_size.get(concrete)
            off_ok = room is not None and off + 8 <= room
            reps = ['%s = %s;' % (var, rhs),
                    '%s = (__typeof__(%s))KD_FBITS(%s);' % (var, var, rhs)]
            if off_ok:
                reps.append('%s = *(__typeof__(%s) *)&%s;' % (var, var, rhs))
            edits.append((m.start(), m.end(), reps,
                          '%-26s %-16s [%s].%-13s +%-3d -> = %s'
                          % (fn, concrete, sm.group('k'), sm.group('f'), off, fp)))
            done.add(sm.start())

        for m in sites:
            if m.start() in done:
                continue
            off = int(m.group('k')) * bsz + boff[m.group('f')]
            fp = paths.get(off)
            if not fp:
                declined += 1
                continue
            edits.append((m.start(), m.end(),
                          ['((%s *)%s)->%s' % (concrete, m.group('base'), fp)],
                          '%-26s %-16s [%s].%-13s +%-3d -> %s'
                          % (fn, concrete, m.group('k'), m.group('f'), off, fp)))
        n0 = fixed
        for start, end, reps, note in sorted(edits, key=lambda e: -e[0]):
            for rep in reps:
                cand = text[:start] + rep + text[end:]
                if compiles_identically(fn, cand, build, inc):
                    text = cand
                    fixed += 1
                    break
            else:
                declined += 1
        if fixed > n0:
            open(path, 'w').write(text)
        notes.append('%-26s %-16s %3d of %3d site(s) repaired'
                     % (fn, concrete, fixed - n0, len(edits)))

    print('  derived fields rewritten as the concrete field : %d' % fixed)
    print('  declined (reported, not guessed)               : %d' % declined)
    for n in notes:
        print('     %s' % n)
    return 0


if __name__ == '__main__':
    sys.exit(main())
