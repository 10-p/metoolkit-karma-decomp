#!/usr/bin/env bash
# ab_contact.sh — build and run the per-function A/B for McdContactSimplify.
#
#   ab_contact.sh [recovered-obj-dir] [iters]
#   KD_SELFTEST=1 ab_contact.sh ...      <- the control; must read 0
#
# Stages every recovered object with its defined symbols prefixed rec_, exactly
# as difftest_pair.sh and make_shadow_metoolkit.sh do, so both implementations
# coexist in one process over one supporting library. McdContact is normally
# HELD, so it is compiled into a copy of the build dir here rather than into it:
# putting it in /tmp/kd_build would make the other gates report on an object the
# detectors are holding back.
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
MT="${KD_METOOLKIT:-$HERE/../Thirdparty/metoolkit}"
INC=$MT/include
LIB=$MT/lib.rel/linux_single_gcc3.2
RECDIR="${1:-/tmp/kd_build}"
ITERS="${2:-200000}"
SRC="${KD_SRC:-${KD_OUT:-/tmp/kd_out}/allobj/McdContact.c}"

W=$(mktemp -d); trap 'rm -rf "$W"' EXIT
mkdir -p "$W/build"; cp "$RECDIR"/*.o "$W/build/" 2>/dev/null || true

IF="-I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon -I$INC/McdPrimitives"
IF="$IF -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp"

# McdContact is held, so it is not in the build dir; compile it here.
gcc -m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w \
    -Wno-int-conversion -Wno-incompatible-pointer-types -DLINUX \
    -I"$HERE/include" $IF -c -o "$W/build/McdContact.o" "$SRC"

for o in "$W/build"/*.o; do
    dst="$W/rec_$(basename "$o")"; cp "$o" "$dst"
    nm --defined-only "$dst" | awk '$2 ~ /^[TDBRWV]$/ {print $3, "rec_" $3}' > "$W/map"
    [ -s "$W/map" ] && objcopy --redefine-syms="$W/map" "$dst"
done

gcc -m32 -O2 -DLINUX -no-pie $IF -o "$W/ab" "$HERE/test/ab_contact.c" \
    "$W"/rec_*.o -Wl,--start-group "$LIB"/*.a -Wl,--end-group -lstdc++ -lm
"$W/ab" "$ITERS"
