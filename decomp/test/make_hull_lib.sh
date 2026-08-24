#!/usr/bin/env bash
# make_hull_lib.sh — put the recovered convex hull into a metoolkit tree.
#
#   make_hull_lib.sh <metoolkit-root> [out-root]
#
# Replaces libMcdConvexCreateHull.a — qhull 2.6, 1.4 MB — with an archive built
# from src/McdConvexCreateHull/kd_convexhull.c, about 10 KB. With no out-root the
# tree is built in /tmp/mt_newhull; the source tree is never modified.
#
# This is HANDOVER.md §12 item 4, the qhull half, and it is a whole-archive swap
# rather than the member-level substitution make_substituted_metoolkit.sh does:
# nothing here is decompiled, so there is no shipped member to replace one for
# one.
#
# Use the resulting tree anywhere a metoolkit root is taken, e.g.
#
#   ./test/hull_ab.sh                                   # geometry, vs shipped
#   ./test/difftest_pair.sh /tmp/kd_build_h "$OUT"      # collisions, new hull
#                                                       # on BOTH sides
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
SRC="${1:?metoolkit root}"
OUT="${2:-/tmp/mt_newhull}"
SUBDIR="lib.rel/linux_single_gcc3.2"
IMPL="$HERE/../src/McdConvexCreateHull/kd_convexhull.c"
INC="$(cd "$SRC/include" && pwd)"

rm -rf "$OUT"; mkdir -p "$OUT/$SUBDIR"
ln -s "$INC" "$OUT/include"
cp "$SRC/$SUBDIR"/*.a "$OUT/$SUBDIR/"

# Same flags the recovered objects are built with, minus the Ghidra-specific
# warning suppressions this file does not need.
gcc -m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -Wall -DLINUX \
    -I"$INC" -I"$INC/MeGlobals" -I"$INC/McdCommon" -I"$INC/McdPrimitives" \
    -c -o "$OUT/kd_convexhull.o" "$IMPL"

rm -f "$OUT/$SUBDIR/libMcdConvexCreateHull.a"
ar rcs "$OUT/$SUBDIR/libMcdConvexCreateHull.a" "$OUT/kd_convexhull.o"

# The shipped archive also exports `dfacet` and `dvertex` — qhull's own
# debugger helpers. Nothing in metoolkit references them (checked across all
# sixteen archives), so they are deliberately absent rather than stubbed.
want=$(nm --defined-only "$SRC/$SUBDIR/libMcdConvexCreateHull.a" \
       | awk '$2=="T"{print $3}' | grep -v '^qh' | grep -vE '^(dfacet|dvertex)$' | sort)
got=$(nm --defined-only "$OUT/$SUBDIR/libMcdConvexCreateHull.a" \
      | awk '$2=="T"{print $3}' | sort)
if [ "$want" != "$got" ]; then
    echo "FATAL: exported symbol set does not match the shipped archive"
    diff <(echo "$want") <(echo "$got") | sed 's/^/  /'
    exit 1
fi

o=$(stat -c%s "$SRC/$SUBDIR/libMcdConvexCreateHull.a")
n=$(stat -c%s "$OUT/$SUBDIR/libMcdConvexCreateHull.a")
echo "$OUT"
echo "  libMcdConvexCreateHull.a: $o -> $n bytes"
echo "  exported symbols match the shipped archive ($(echo "$got" | wc -l) functions)"
