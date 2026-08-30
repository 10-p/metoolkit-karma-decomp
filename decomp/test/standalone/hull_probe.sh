#!/usr/bin/env bash
# hull_probe.sh — check a convex-hull implementation against the contract.
#
#   hull_probe.sh [metoolkit-lib-dir]        # the SHIPPED library (ground truth)
#   KD_HULL_IMPL=<file.c> hull_probe.sh      # a REPLACEMENT, same checks
#
# Establishes what McdComputeHull produces, and checks it against every invariant
# McdQHullTypes.h claims in prose. See HANDOVER.md §8a: replacing
# libMcdConvexCreateHull means reproducing that structure well enough for GJK's
# support function to hill-climb it, and "diff against the shipped hull" is NOT
# an available test — the hull reindexes, so a differently indexed but
# geometrically identical answer is legitimate. So the acceptance test is
# invariants plus geometry, and this is it.
#
# 32-bit, because the shipped archives are i386 and the replacement has to link
# beside them.
set -uo pipefail
. "$(cd "$(dirname "$0")" && pwd)/../../lib/kd-paths.sh"

HERE="$(cd "$(dirname "$0")" && pwd)"
LIBDIR="${1:-$KD_MT_LIB}"
INC="$(cd "$(dirname "$LIBDIR")/../include" && pwd)"
IMPL="${KD_HULL_IMPL:-}"

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

INCS=(-I"$INC" -I"$INC/MeGlobals" -I"$INC/McdCommon" -I"$INC/McdPrimitives")

if [ -n "$IMPL" ]; then
    # The replacement supplies every McdComputeHull/McdCnv* symbol itself, so the
    # hull archive is left OUT of the link entirely — if it were present, the
    # linker would quietly satisfy a missing symbol from qhull and the run would
    # be measuring a hybrid.
    echo >&2 "== implementation under test: $IMPL =="
    gcc -m32 -O2 -DLINUX -w -no-pie "${INCS[@]}" \
        -o "$WORK/probe" "$HERE/hull_probe.c" "$IMPL" \
        "$LIBDIR/libMeGlobals.a" -lstdc++ -lm 2>"$WORK/err" || {
        echo "FATAL: hull_probe did not build"; sed 's/^/  /' "$WORK/err" | head -30; exit 1; }
else
    # libMeGlobals supplies MeMemoryAPI, the hull archive's only non-libc,
    # non-qhull import. --start-group because the metoolkit archives are mutually
    # recursive. -lstdc++ for __gxx_personality_v0 (McdConvexHull.o is
    # C++-compiled) and -no-pie because these are non-PIC i386 archives.
    echo >&2 "== implementation under test: SHIPPED libMcdConvexCreateHull.a =="
    gcc -m32 -O2 -DLINUX -w -no-pie "${INCS[@]}" \
        -o "$WORK/probe" "$HERE/hull_probe.c" \
        -Wl,--start-group \
           "$LIBDIR/libMcdConvexCreateHull.a" "$LIBDIR/libMeGlobals.a" \
        -Wl,--end-group -lstdc++ -lm 2>"$WORK/err" || {
        echo "FATAL: hull_probe did not build"; sed 's/^/  /' "$WORK/err" | head -30; exit 1; }
fi

"$WORK/probe"
