#!/usr/bin/env bash
# hull_probe.sh — run test/hull_probe.c against the SHIPPED hull library.
#
#   hull_probe.sh [metoolkit-lib-dir]
#
# Establishes what McdComputeHull actually produces, and checks it against every
# invariant McdQHullTypes.h claims in prose. See HANDOVER.md §8a: replacing
# libMcdConvexCreateHull means reproducing that structure well enough for GJK's
# support function to hill-climb it, and "diff against the shipped hull" is NOT
# an available test — a differently indexed but geometrically identical hull is
# a legitimate answer. So the acceptance test is invariants plus geometry, and
# this is it.
#
# 32-bit, because the shipped archives are i386.
set -uo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
LIBDIR="${1:-$HERE/../../Thirdparty/metoolkit/lib.rel/linux_single_gcc3.2}"
INC="$(cd "$(dirname "$LIBDIR")/../include" && pwd)"

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

# libMeGlobals supplies MeMemoryAPI, the hull archive's only non-libc, non-qhull
# import. --start-group because the metoolkit archives are mutually recursive.
# -lstdc++ for __gxx_personality_v0 (McdConvexHull.o is C++-compiled) and
# -no-pie because these are non-PIC i386 archives, same as the other harnesses.
gcc -m32 -O2 -DLINUX -w -no-pie \
    -I"$INC" -I"$INC/MeGlobals" -I"$INC/McdCommon" -I"$INC/McdPrimitives" \
    -o "$WORK/probe" "$HERE/hull_probe.c" \
    -Wl,--start-group \
       "$LIBDIR/libMcdConvexCreateHull.a" "$LIBDIR/libMeGlobals.a" \
    -Wl,--end-group -lstdc++ -lm 2>"$WORK/err" || {
    echo "FATAL: hull_probe did not build"; sed 's/^/  /' "$WORK/err" | head -30; exit 1; }

"$WORK/probe"
