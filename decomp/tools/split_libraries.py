#!/usr/bin/env python3
"""split_libraries.py — lay the recovered sources out as MathEngine's 16 libraries.

    split_libraries.py <src-dir> [build-dir]        # e.g. /tmp/kd_lp64/allobj
    split_libraries.py <src-dir> --check            # verify the checked-in tree matches
    split_libraries.py --map                        # print the object -> archive map

WHAT THIS IS FOR.

`recover.py` emits one flat directory of 145 `.c`, because the recovery works
object by object and an archive boundary means nothing to it. MathEngine shipped
those objects as SIXTEEN static libraries, and a consumer that wants to drop the
recovered Karma in where the original was expects that shape — one target per
archive, named the same, so `McdPrimitives` is `McdPrimitives` and a link line
written against the 2003 SDK still reads correctly.

THE MAP IS NOT A JUDGEMENT, IT IS READ OFF THE SHIPPED ARCHIVES. `ar t` on
`metoolkit/lib.rel/linux_single_gcc3.2/*.a` says exactly which member came from
which library. Verified 2026-08-30: all 145 recovered objects map, none appears
in two archives, so there is no tie to break and no rule to invent.

    McdPrimitives 27   Mdt           26   MeGlobals       23   MdtKea 18
    McdConvex     13   McdFrame      12   MeAssetDB        7   McdCommon 5
    Mst            4   MeAssetDBXMLIO 3   MeAssetFactory   3   MeXML  3
    MdtBcl         1
    McdConvexCreateHull  <- the hand-written hull, not a recovered object

THREE ARCHIVES GET NO TARGET, AND THAT IS A FINDING RATHER THAN A GAP.
`MeApp` and `MeViewer2` are the sample-application and viewer libraries; nothing
in UT2004 links them and no object of theirs was ever recovered.
`McdConvexCreateHull` is qhull, which was REPLACED rather than recovered — see
`decomp/src/McdConvexCreateHull/kd_convexhull.c` and HANDOVER.md §8a — so its
target is built from that source instead.

⚠ THE SPLIT IS A LAYOUT, NOT A LINK BOUNDARY. The shipped archives have circular
dependencies between them (McdCommon <-> McdPrimitives, Mst -> McdSpace/Mdt),
which is why the engine's reference link needs `--start-group`. The recovered
targets are INTERFACE-linked into one aggregate for exactly that reason; see
metoolkit_decomp/CMakeLists.txt.
"""
import glob
import os
import shutil
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

MD = os.path.join(kd_paths.REPO, 'metoolkit_decomp')
MAP_FILE = os.path.join(MD, 'library-map.txt')

# Recovered from decomp/src/, not from an archive member.
HANDWRITTEN = {
    'McdConvexCreateHull': [
        os.path.join(kd_paths.ROOT, 'src', 'McdConvexCreateHull', 'kd_convexhull.c'),
    ],
}


def archive_map():
    """{object-stem: [libraries]} straight out of the shipped archives.

    ⚠ A STEM CAN LEGITIMATELY BE IN EVERY ARCHIVE. MathEngine compiled a
    per-library `version.o` — same name, sixteen different objects, each holding
    that library's version stamp — so "this member is in more than one archive"
    is a normal fact about the SDK, not a defect. It only becomes a problem if it
    happens to an object we are actually laying out, which is checked in plan()
    where the answer matters. `version` is not among the 145: recover.py skips it
    ("dump has no functions").
    """
    kd_paths.require_metoolkit()
    out = {}
    for a in sorted(glob.glob(os.path.join(kd_paths.MT_LIB, '*.a'))):
        lib = os.path.basename(a)[3:-2]              # libMdtKea.a -> MdtKea
        r = subprocess.run(['ar', 't', a], capture_output=True, text=True)
        if r.returncode:
            sys.exit('split_libraries: ar t failed on %s' % a)
        for member in r.stdout.split():
            stem = member[:-2] if member.endswith('.o') else member
            out.setdefault(stem, [])
            if lib not in out[stem]:
                out[stem].append(lib)
    return out


def plan(srcdir, builddir):
    """[(library, source-path)] for everything that will be laid out.

    ⚠ THE BUILD DIRECTORY IS THE AUTHORITY ON WHAT SHIPS, NOT THE SOURCE
    DIRECTORY. recover.py emits a .c for every object it attempts, including the
    three it cannot compile (MeASELoad, MeFGeometryFromMesh, McduDebugDraw —
    HANDOVER.md §13 shows nothing in the engine can reach them). Laying those out
    would hand every consumer three sources that do not build, and the mistake is
    invisible here: the split succeeds, the map looks right, and the failure
    lands in somebody else's compile. The same rule is what generated/README.md
    has always documented for refreshing the checked-in tree — iterate over the
    .o, not over the .c.
    """
    amap = archive_map()
    if not os.path.isdir(builddir):
        sys.exit('split_libraries: no build directory at %s. It says which objects\n'
                 '  compiled; run recover.py --build-dir first.' % builddir)
    built = {os.path.basename(o)[:-2]
             for o in glob.glob(os.path.join(builddir, '*.o'))}
    if not built:
        sys.exit('split_libraries: %s holds no .o — nothing compiled.' % builddir)

    rows, unmapped, ambiguous, skipped = [], [], [], []
    for c in sorted(glob.glob(os.path.join(srcdir, '*.c'))):
        stem = os.path.basename(c)[:-2]
        if stem not in built:
            skipped.append(stem)
            continue
        libs = amap.get(stem)
        if not libs:
            unmapped.append(stem)
        elif len(libs) > 1:
            ambiguous.append((stem, libs))
        else:
            rows.append((libs[0], c))
    if unmapped:
        sys.exit('split_libraries: %d object(s) are in no shipped archive, so the\n'
                 '  library they belong to cannot be read off the SDK:\n    %s'
                 % (len(unmapped), '\n    '.join(unmapped)))
    if ambiguous:
        # Never happens for the recovered set; if it starts to, the split needs a
        # documented rule rather than a silent last-wins.
        sys.exit('split_libraries: object in more than one archive, and it IS one we\n'
                 '  lay out, so there is no rule to pick:\n    %s'
                 % '\n    '.join('%s: %s' % (s, ', '.join(l)) for s, l in ambiguous))
    if skipped:
        # Say it out loud. A silently smaller library is the failure mode here.
        print('  not laid out (no .o in %s, i.e. did not compile): %s'
              % (os.path.basename(builddir), ', '.join(sorted(skipped))))
    for lib, srcs in sorted(HANDWRITTEN.items()):
        for s in srcs:
            if not os.path.isfile(s):
                sys.exit('split_libraries: missing hand-written source %s' % s)
            rows.append((lib, s))
    return rows


def write_map(rows):
    libs = {}
    for lib, path in rows:
        libs.setdefault(lib, []).append(os.path.basename(path))
    with open(MAP_FILE, 'w') as f:
        f.write('# object -> library, read off metoolkit/lib.rel/linux_single_gcc3.2/*.a\n'
                '# by decomp/tools/split_libraries.py. Do not hand-edit; regenerate.\n')
        for lib in sorted(libs):
            for src in sorted(libs[lib]):
                f.write('%-22s %s\n' % (lib, src))


def layout(rows, check=False):
    want = {}
    for lib, path in rows:
        want[os.path.join(MD, 'src', lib, os.path.basename(path))] = path

    have = set(glob.glob(os.path.join(MD, 'src', '*', '*.c')))
    bad = 0

    if check:
        for extra in sorted(have - set(want)):
            print('  EXTRA   %s' % os.path.relpath(extra, kd_paths.REPO)); bad += 1
        for dst, src in sorted(want.items()):
            rel = os.path.relpath(dst, kd_paths.REPO)
            if dst not in have:
                print('  MISSING %s' % rel); bad += 1
            elif open(dst, 'rb').read() != open(src, 'rb').read():
                print('  DIFFERS %s' % rel); bad += 1
        return bad

    for extra in have - set(want):
        os.remove(extra)
    for dst, src in want.items():
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copyfile(src, dst)
    return 0


def main():
    if '--map' in sys.argv:
        amap = archive_map()
        by = {}
        for stem, libs in amap.items():
            for lib in libs:
                by.setdefault(lib, []).append(stem)
        for lib in sorted(by):
            print('%-22s %3d' % (lib, len(by[lib])))
        return 0

    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    if not args:
        sys.exit('usage: split_libraries.py <src-dir> [build-dir] [--check|--map]\n'
                 '  src-dir   the post-passed recovery, e.g. /tmp/kd_lp64/allobj\n'
                 '  build-dir which of those compiled (default: %s)' % kd_paths.BUILD)
    builddir = args[1] if len(args) > 1 else kd_paths.BUILD
    rows = plan(args[0], builddir)

    if '--check' in sys.argv:
        bad = layout(rows, check=True)
        libs = len({r[0] for r in rows})
        if bad:
            print('split_libraries: %d discrepancy(ies) — the checked-in tree is not\n'
                  '  what %s would produce. Regenerate rather than hand-edit.'
                  % (bad, args[0]))
            return 1
        print('split_libraries: %d sources across %d libraries, all match' %
              (len(rows), libs))
        return 0

    layout(rows)
    write_map(rows)
    libs = {}
    for lib, _p in rows:
        libs[lib] = libs.get(lib, 0) + 1
    print('split_libraries: laid out %d sources into %d libraries' % (len(rows), len(libs)))
    for lib in sorted(libs):
        print('  %-22s %3d' % (lib, libs[lib]))
    return 0


if __name__ == '__main__':
    sys.exit(main())
