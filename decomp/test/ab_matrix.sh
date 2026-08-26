#!/usr/bin/env bash
# ab_matrix.sh — A/B the recovered MeMath against the shipped one, per function.
#
#   ab_matrix.sh <recovered MeMath.c> <metoolkit-lib-dir> [iterations]
#
# HANDOVER.md §5c refuses MeMath, and the reason recorded for it was that no
# gate here could verify a reconstruction. That was wrong: the gate did not
# exist. These functions are PURE, so an A/B is easier for them than for the
# collision pairs difftest_pair.sh already does this to.
#
# THE RECOVERED FILE DOES NOT COMPILE AS A WHOLE — one `stack0x` in
# MeMatrix4TMUpdateFromVelocities — so the driver stubs that one function out
# and measures its SIBLING, `...AndAcceler`, which compiles and which no
# detector holds. That is the point: the sibling has the SAME defect (Ghidra
# discards the x87 fcos/fsin results) and produces NO diagnostic, so it is the
# half that would ship silently.
#
# KD_SELFTEST=1 runs the shipped function against ITSELF and must read zero.
set -uo pipefail

SRC="${1:?recovered MeMath.c}"
LIBDIR="${2:?metoolkit lib dir}"
N="${3:-200000}"
HERE="$(cd "$(dirname "$0")/.." && pwd)"
INC="$(cd "$LIBDIR/../.." && pwd)/include"
W=$(mktemp -d); trap 'rm -rf "$W"' EXIT

CF="-m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion"
CF="$CF -Wno-incompatible-pointer-types -DLINUX -I$HERE/include -I$INC"
for d in McdCommon McdPrimitives McdFrame MeGlobals MdtBcl MdtKea Mst MeApp; do
    CF="$CF -I$INC/$d"
done

# The one function that does not compile is replaced by a stub. It is NOT under
# test here; leaving it in would stop the file building and measure nothing.
python3 - "$SRC" "$W/memath.c" <<'PY'
import re, sys
src = open(sys.argv[1], errors='ignore').read()
parts = re.split(r'(?m)^(/\* ---- \S+ .*?---- \*/)$', src)
out = []
for p in parts:
    if 'MeMatrix4TMUpdateFromVelocities (' in p or \
       re.search(r'\bkd_MeMatrix4TMUpdateFromVelocities\s*\(\s*MeVector4', p):
        # keep the banner, drop the body, emit a stub with the same linkage
        p = re.sub(r'(?s)\{.*\n\}', '{ (void)0; }', p, count=1)
    out.append(p)
open(sys.argv[2], 'w').write(''.join(out))
PY

gcc $CF -c -o "$W/rec.o" "$W/memath.c" 2>"$W/cc.log" || {
    echo "  ab_matrix: recovered MeMath did not compile even with the stub:"
    grep -m3 "error:" "$W/cc.log" | sed 's/^/    /'
    exit 2
}
nm --defined-only "$W/rec.o" | awk '$2 ~ /^[TDBRWV]$/ {print $3, "rec_" $3}' > "$W/map"
[ -s "$W/map" ] && objcopy --redefine-syms="$W/map" "$W/rec.o"

gcc $CF -c -o "$W/drv.o" "$HERE/test/ab_matrix.c" || exit 2
gcc -m32 -o "$W/ab" "$W/drv.o" "$W/rec.o" \
    -Wl,--start-group "$LIBDIR"/*.a -Wl,--end-group -lstdc++ -lm 2>"$W/ld.log" || {
    echo "  ab_matrix: link failed"; grep -m5 . "$W/ld.log" | sed 's/^/    /'; exit 2; }

echo -n "  control  "; KD_SELFTEST=1 "$W/ab" "$N"
ctl=$?
echo -n "  measured "; "$W/ab" "$N"
res=$?
if [ "$ctl" -ne 0 ]; then
    echo "  !! SELFTEST IS NON-ZERO — the harness is measuring itself. Every"
    echo "     number above is meaningless until that reads 0."
    exit 1
fi
exit $res
