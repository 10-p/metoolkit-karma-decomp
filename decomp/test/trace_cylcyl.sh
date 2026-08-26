#!/usr/bin/env bash
# trace_cylcyl.sh — print OverlapCylCyl's outputs from BOTH implementations, in
# one run, on difftest's own seeded inputs.
#
#   trace_cylcyl.sh [iters]
#
# WHY NOT bisect_static.sh's MECHANISM. That stages a `sh_`-prefixed copy of the
# whole shipped object and forwards one function into it. On IxCylinderCylinder
# it produced a `call 0` inside the shipped code: the object carries file-static
# DATA (`cylR1`, `dispDotX1`, ...) shared with `EndCapMin`, plus a weak
# `CylPerpAndPara`, and globalising and renaming all of it changes which
# definition the mixed link picks. That harness gave a verdict for
# CylCylIntersect and a SEGFAULT for OverlapCylCyl, which is not a verdict.
#
# THIS DOES NOT MIX IMPLEMENTATIONS. `_Z13OverlapCylCyl...` is a GLOBAL in the
# shipped object, so the archive member's own definition is RENAMED to
# `orig_OverlapCylCyl` and a wrapper takes the original name. Every internal
# call in the shipped member then lands on the wrapper, which calls the renamed
# original — one implementation, no arbitration, nothing weakened. The recovered
# side is instrumented in its own source by tools/mk_trace_obj.py.
#
# difftest_pair is SEEDED (rs = 0xC0FFEE), and the driver calls orig then rec
# for the same pair, so the two traces interleave call for call and
# `paste`-ing the O and R streams lines them up exactly.
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
MT="${KD_METOOLKIT:-$HERE/../Thirdparty/metoolkit}"
INC=$MT/include
LIB=$MT/lib.rel/linux_single_gcc3.2
SRC="${KD_SRC:-${KD_OUT:-/tmp/kd_out}/allobj/IxCylinderCylinder.c}"
BUILD="${KD_BUILD:-/tmp/kd_build}"
ITERS="${1:-200000}"
OUT="${KD_TRACE:-/tmp/cylcyl_trace.txt}"
MANG=_Z13OverlapCylCylRfR6lsVec3S_RPS0_RsfffffRK11lsTransformf
P='MeReal *outSep,lsVec3 *outN,MeReal *outPN,lsVec3 **outPos,MeI16 *outDims,MeReal inEps,MeReal inR1,MeReal inHH1,MeReal inR2,MeReal inHH2,lsTransform *inT12,MeReal scale'
A='outSep,outN,outPN,outPos,outDims,inEps,inR1,inHH1,inR2,inHH2,inT12,scale'

W=$(mktemp -d); trap 'rm -rf "$W"' EXIT

# NO renaming and NO extra copy of the shipped object. `_Z13OverlapCylCyl...` is
# a GLOBAL there, and difftest_pair stages the recovered object with its own
# symbols prefixed `rec_`, so the shipped name is free for the shadow to call
# directly. An earlier version renamed the shipped definition with
# `objcopy --redefine-sym` — which renames the CALL SITES in the same object
# too, so the shipped caller went straight past the wrapper and it printed
# nothing while looking like it had run.
python3 "$HERE/tools/mk_trace_obj.py" "$SRC" "$MANG" > "$W/obj.c"

rm -rf "$W/b"; cp -a "$BUILD" "$W/b"
CF="-m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion"
CF="$CF -Wno-incompatible-pointer-types -DLINUX -I$HERE/include -I$INC -I$INC/McdCommon"
CF="$CF -I$INC/McdPrimitives -I$INC/McdFrame -I$INC/MeGlobals -I$INC/MdtBcl -I$INC/MdtKea"
CF="$CF -I$INC/Mst -I$INC/MeApp"
gcc $CF -c -o "$W/b/IxCylinderCylinder.o" "$W/obj.c"
for o in "$W/b"/*.o; do
    d="$W/rec_$(basename "$o")"; cp "$o" "$d"
    nm --defined-only "$d" | awk '$2 ~ /^[TDBRWV]$/ {print $3, "rec_" $3}' > "$W/m"
    [ -s "$W/m" ] && objcopy --redefine-syms="$W/m" "$d"
done
IF="-I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon -I$INC/McdPrimitives"
IF="$IF -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp"
gcc -m32 -O2 -DLINUX -no-pie $IF -o "$W/dt" "$HERE/test/difftest_pair.c" \
    "$W"/rec_*.o -Wl,--start-group "$LIB"/*.a -Wl,--end-group -lstdc++ -lm
"$W/dt" McdCylinderCylinderIntersect "$ITERS" 2>"$W/t.txt" | tail -7
cp "$W/t.txt" "$OUT"
grep -E "^CC#|^OVERLAPCYLCYL" "$OUT" | head -60
