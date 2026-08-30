#!/usr/bin/env bash
# difftest_pair.sh — build and run the precision-tier gate for one interaction.
#
#   difftest_pair.sh <recovered-obj-dir> <metoolkit-root> [pair name] [iterations]
#
# Stages every recovered object with its defined symbols prefixed rec_, exactly
# as make_shadow_metoolkit.sh does, so both implementations coexist in one
# process and share one supporting library. Prefixing EVERY symbol and not just
# the one under test is deliberate: otherwise the object's siblings collide with
# the shipped archive and the linker silently picks one of the two, which would
# mean measuring a build that is half recovered.
set -euo pipefail
. "$(cd "$(dirname "$0")" && pwd)/../../lib/kd-paths.sh"

RECOVERED="${1:?recovered object dir}"
SRC="${2:?metoolkit root}"
PAIR="${3:-}"
ITERS="${4:-200000}"
HERE="$(cd "$(dirname "$0")" && pwd)"
INC="$SRC/include"
LIB="$SRC/$KD_MT_LIB_SUBDIR"

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

for o in "$RECOVERED"/*.o; do
    [ -e "$o" ] || continue
    dst="$WORK/rec_$(basename "$o")"
    cp "$o" "$dst"
    map="$WORK/map"
    nm --defined-only "$dst" | awk '$2 ~ /^[TDBRWV]$/ {print $3, "rec_" $3}' > "$map"
    [ -s "$map" ] && objcopy --redefine-syms="$map" "$dst"
done

IFLAGS="-I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon -I$INC/McdPrimitives"
IFLAGS="$IFLAGS -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp"

gcc -m32 -O2 -DLINUX -no-pie $IFLAGS -o "$WORK/difftest" "$HERE/difftest_pair.c" \
    "$WORK"/rec_*.o -Wl,--start-group "$LIB"/*.a -Wl,--end-group -lstdc++ -lm

if [ -n "$PAIR" ]; then "$WORK/difftest" "$PAIR" "$ITERS"; else "$WORK/difftest" all "$ITERS"; fi
