#!/usr/bin/env python3
"""interaction_types.py — which two geometries does this Ix* function handle?

    interaction_types.py [kd_out/allobj]        # print the table and self-check

WHY THIS EXISTS. `fix_literal_offsets.py` types an opaque pointer from the
geometry a FILE registers, and its README records what that cannot reach:

    "44 where the concrete type is genuinely ambiguous — an Ix* function
     handles TWO geometries, so per-file inference cannot work and those need
     per-site typing"

Those 44 are not a tail. One of them is `McdSphylPlaneIntersect`, which every
step of `scene_ragdoll` runs 7,298 times:

    pvVar9 = McdModelGetGeometry(p->model1);
    fVar2  = -*(float *)((kd_iptr)pvVar9 + 0x14);      /* mHalfHeight */
             *(float *)((kd_iptr)pvVar9 + 0x10)        /* mRadius     */

`McdSphyl` is `{ McdGeometry m_g; MeReal mRadius; MeReal mHalfHeight; }`, so
those are 16 and 20 here and **32 and 36** at LP64. At 64-bit the capsule's
radius is read out of the middle of its own base class — bytes 16..19 of
`_McdGeometry`, which are the low half of the `prev` POINTER. The value is an
ADDRESS reinterpreted as a float, it changes with ASLR, and it is the
address-dependent read `proven.txt` LP64-ADDRESS-DEPENDENT measured and could
not name.

★ AND THE TYPE IS WRITTEN DOWN, IN THE REGISTRATION. Every interaction says
which pair it is for, by type id, and every geometry says what its own id is:

    McdSphyl.c    McdSphylGetTypeId()            { return 5; }
    McdPlane.c    McdPlaneGetTypeId()            { return 3; }
    IxSphyl...c   McdSphylPlaneRegisterInteraction(McdFramework *frame)
                    interactions.intersectFn = kd_McdSphylPlaneIntersect;
                    McdFrameworkSetInteractions(frame, 5, 3, &interactions);

so `McdSphylPlaneIntersect`'s `p->model1` is an `McdSphyl` and its `p->model2`
is an `McdPlane`. No inference, no offsets, no guessing — the same installer
evidence `proven.txt` LP64-OPAQUE-API used for the sixteen `MdtBclAdd*`
functions, one indirection along.

⚠ THE ARGUMENT ORDER IS THE MODEL ORDER, AND THAT IS A CLAIM WORTH CHECKING
RATHER THAN ASSUMING. It is confirmed twice over by the offsets themselves:
`McdSphylPlaneIntersect` reads `model1`'s geometry at 0x10/0x14, which are
fields of `McdSphyl` (type 5, the FIRST argument) and land on nothing in
`McdPlane`; `McdBoxTriangleListIntersect` reads `model2`'s at 40/48/52, which
are `McdTriangleList`'s (type 6, the SECOND). Both agree with
`McdFrameworkSetInteractions(frame, type1, type2, ...)`.

WHAT IT DOES NOT COVER. A function registered for two different pairs is
dropped rather than arbitrated, and so is one whose type id names no geometry in
this corpus. Both are reported.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

BANNER = re.compile(r'(?m)^/\* ---- (\S+)')
# `MeI16 kd_McdSphylGetTypeId(void) { return 5; }`, whatever the return type is
TYPEID = re.compile(r'(?ms)^/\* ---- (?P<name>\w+GetTypeId) \(exported.*?\{\s*'
                    r'return (?P<id>\d+);')
SETINT = re.compile(r'McdFrameworkSetInteractions\s*\(\s*[^,]+,\s*(?P<a>\d+)\s*,'
                    r'\s*(?P<b>\d+)\s*,')
# every function installed into the McdInteractions struct in the same function
INSTALL = re.compile(r'interactions\s*\.\s*\w+\s*=\s*(?:\([^)]*\)\s*)?'
                     r'(?P<fn>[A-Za-z_]\w*)\s*;')


def region_of(text, pos):
    start, end = 0, len(text)
    for m in BANNER.finditer(text):
        if m.start() <= pos:
            start = m.start()
        elif m.start() > pos:
            end = m.start()
            break
    return text[start:end]


def type_ids(texts):
    """geometry type id -> the concrete tag, read from each GetTypeId().

    ⚠⚠ A SECOND ACCESSOR ON THE SAME TYPE IS NOT A CONFLICT, AND TREATING IT AS
    ONE COST THE WHOLE CONVEX-MESH FAMILY. `McdConvexMesh.c` exports both
    `McdConvexMeshGetTypeId` and `McdConvexMeshMeshGetTypeId`, and both return 7.
    Stripping `GetTypeId` gives the tags `McdConvexMesh` and `McdConvexMeshMesh`,
    the "claimed by two tags" rule read that as evidence about neither, and id 7
    dropped out of the table entirely — so `McdSphylConvexMeshIntersect` and its
    seven siblings were never typed and `fix_literal_offsets` left every geometry
    offset in `IxConvexPrimitives.c` at its i386 value. `McdConvexMesh::mHull` is
    at 16 here and 32 there, so `ConvexHullNSegment` was handed a hull pointer
    sixteen bytes short and `ConvexHullVoronoiRegion` faulted on the face table.
    Measured on the LP64 vehicle, 2026-08-31, frame 27 of a ktrace run.

    ★ THE TIE-BREAK IS THE TYPE DATABASE, NOT A NAME RULE. A tag that names a
    struct the oracle declares is a type; one that does not is an accessor whose
    name happens to end that way. `McdConvexMesh` has a body, `McdConvexMeshMesh`
    does not. If that leaves one candidate the id is typed; if it leaves none or
    more than one the conflict stands and the id is dropped, as before."""
    import fix_literal_offsets as flo
    bodies = flo.struct_bodies(os.path.join(kd_paths.METOOLKIT_DIR, 'include'))
    claims = {}
    for text in texts.values():
        for m in TYPEID.finditer(text):
            tag = m.group('name')[:-len('GetTypeId')]
            claims.setdefault(int(m.group('id')), set()).add(tag)
    out = {}
    for i, tags in claims.items():
        if len(tags) == 1:
            out[i] = next(iter(tags))
            continue
        real = [t for t in tags if t in bodies or '_' + t in bodies]
        if len(real) == 1:
            out[i] = real[0]
    return out


def pair_types(texts):
    """installed function -> (tag of model1, tag of model2).

    A function installed for two different pairs is DROPPED. `McdNullIntersect`
    is registered against every type there is, and picking the first would be a
    guess dressed as a measurement."""
    ids = type_ids(texts)
    out, dropped = {}, {}
    for text in texts.values():
        for s in SETINT.finditer(text):
            region = region_of(text, s.start())
            a, b = ids.get(int(s.group('a'))), ids.get(int(s.group('b')))
            if not a or not b:
                continue
            for i in INSTALL.finditer(region):
                fn = re.sub(r'^kd_', '', i.group('fn'))
                if fn in ('0x0',) or not fn:
                    continue
                prev = out.get(fn, (a, b))
                # ⚠ ONCE DROPPED, STAY DROPPED. `out[fn] = None` is the marker,
                # and a THIRD registration of the same function then indexed
                # `None[0]` and threw. Latent until the ConvexMesh family started
                # being typed at all — `McdNullIntersect` is registered against
                # every type there is.
                if prev is None:
                    continue
                if prev != (a, b):
                    dropped[fn] = 'registered for %s/%s and %s/%s' % (
                        prev[0], prev[1], a, b)
                    out[fn] = None
                elif fn not in dropped:
                    out[fn] = (a, b)
    return {k: v for k, v in out.items() if v}, dropped


def load(srcdir, build=None):
    texts = {}
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c'):
            continue
        if build and not os.path.exists(os.path.join(build, fn[:-2] + '.o')):
            continue
        texts[fn] = open(os.path.join(srcdir, fn), errors='ignore').read()
    return texts


# ---- THE SELF-CHECK. A regex that stops matching returns an EMPTY table, and
# an empty table is not an error message — every caller simply declines and
# reports "nothing to type", which reads exactly like "there was nothing here".
# Two facts, one per argument position, from the two objects this was written
# for.
SELFCHECK = [('McdSphylPlaneIntersect', ('McdSphyl', 'McdPlane')),
             ('McdBoxTriangleListIntersect', ('McdBox', 'McdTriangleList'))]


def checked(srcdir, build=None):
    """(pairs, dropped), with the table proved to be measuring something."""
    texts = load(srcdir, build)
    pairs, dropped = pair_types(texts)
    bad = [(f, w, pairs.get(f)) for f, w in SELFCHECK if pairs.get(f) != w]
    if bad:
        sys.exit('interaction_types: SELF-CHECK FAILED — '
                 + '; '.join('%s read %r (want %r)' % (f, g, w) for f, w, g in bad)
                 + '\n  The registration table is not measuring anything.')
    return pairs, dropped


def main():
    srcdir = sys.argv[1] if len(sys.argv) > 1 else '/tmp/kd_out/allobj'
    texts = load(srcdir)
    ids = type_ids(texts)
    pairs, dropped = pair_types(texts)
    print('  geometry type ids : %s'
          % ', '.join('%d=%s' % (k, v) for k, v in sorted(ids.items())))
    print('  interaction functions typed by their registration : %d' % len(pairs))
    for f, (a, b) in sorted(pairs.items()):
        print('     %-34s model1 %-16s model2 %s' % (f, a, b))
    if dropped:
        print('  dropped (registered for more than one pair) : %d' % len(dropped))
        for f, why in sorted(dropped.items()):
            print('     %-34s %s' % (f, why))
    checked(srcdir)
    print('  self-check ok')
    return 0


if __name__ == '__main__':
    sys.exit(main())
