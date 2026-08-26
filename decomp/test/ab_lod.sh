#!/usr/bin/env bash
# ab_lod.sh — run MdtLOD's code and compare the trajectory, at two settings.
#
#   ab_lod.sh <recovered-obj-dir> [metoolkit-root]
#
# WHAT THIS IS FOR. `MdtLODLastPartition` is the only symbol the engine imports
# from `MdtLOD`, and the engine's one call site is guarded by
# `rowCount > params->maxMatrixSize` with maxMatrixSize = 0x7ffffffc and nothing
# anywhere lowering it. So in the game the function cannot execute — argued in
# proven.txt and measured with a breakpoint that fires zero times in a 235 s
# match. That is a reachability argument, and this project has been burned by
# reachability arguments (MeAssetDBXMLIO passed every offline gate and killed
# the engine at init). `MdtWorldSetMaxMatrixSize` is a public API, so the guard
# can simply be MADE true and the code measured instead of argued about.
#
# TWO SETTINGS, because neither alone is the whole answer:
#
#   KD_MAXMATRIX=40   2 calls of MdtLODLastPartition, 48 of ResizeConstraint,
#                     on partitions built by the real solver from the ragdoll's
#                     joints and contacts. The ragdoll still lands and settles,
#                     so this is the physically meaningful regime — and it is a
#                     thin sample.
#   KD_MAXMATRIX=8    900 and 7,200 calls. LOD discards the contact constraints
#                     and the ragdoll free-falls; the scene's plausibility check
#                     fails and that is CORRECT, not a defect. The run is still
#                     deterministic, so the trajectory is still an oracle, and
#                     it exercises the function 450x harder.
#
# THE NEGATIVE CONTROL IS BUILT IN. KD_MAXMATRIX=128 never reaches the guard, so
# the two builds must agree there whatever MdtLOD contains — if the "8" and "40"
# rows are identical AND the 128 row is too, this harness is not measuring
# MdtLOD at all and every row is meaningless.
#
# AND IT HAS BEEN MADE TO FAIL ON PURPOSE, twice, which is the only reason the
# clean rows mean anything:
#   * the object BEFORE this session's repairs: 128 identical, 40 and 8 both
#     SEGFAULT — the reachability release everyone had been about to sign off on
#     would have shipped a function that dies on its first real call;
#   * qsort's `nmemb` and `size` argument slots swapped: 128 identical, 40
#     SEGFAULTS. Note that 8 stays identical for that one — at 8 the contact
#     groups are too small for the sort to matter — so 40 is the DISCRIMINATING
#     setting and 8 is the VOLUME setting. Neither replaces the other.
set -u
HERE="$(cd "$(dirname "$0")/.." && pwd)"
REC="${1:-/tmp/kd_build}"
MT="${2:-$HERE/../Thirdparty/metoolkit}"
INC="$MT/include"
SCENE="$HERE/test/scene_ragdoll_lod.c"
IF="-I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon -I$INC/McdPrimitives"
IF="$IF -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp"

W=$(mktemp -d); trap 'rm -rf "$W"' EXIT
[ -e "$REC/MdtLOD.o" ] || { echo "FATAL: no $REC/MdtLOD.o — compile it first"; exit 1; }

"$HERE/test/make_substituted_metoolkit.sh" "$REC" "$MT" "$W/sub" MdtLOD >/dev/null

build() {  # $1 = lib dir, $2 = output binary
    gcc -m32 -O2 -DLINUX -no-pie $IF -o "$2" "$SCENE" \
        -Wl,--start-group "$1"/*.a -Wl,--end-group -lstdc++ -lm 2>&1 | tail -3
}
build "$MT/lib.rel/linux_single_gcc3.2"      "$W/stock" || exit 1
build "$W/sub/lib.rel/linux_single_gcc3.2"   "$W/rec"   || exit 1

fail=0
for N in 128 40 8; do
    KD_MAXMATRIX=$N "$W/stock" > "$W/a.csv" 2>/dev/null; ra=$?
    KD_MAXMATRIX=$N "$W/rec"   > "$W/b.csv" 2>/dev/null; rb=$?
    if cmp -s "$W/a.csv" "$W/b.csv"; then
        v="trajectory bit-identical"
    else
        # the FIRST differing step, not the largest difference: after contact a
        # last-bit difference is amplified without bound and the maximum stops
        # discriminating. See substitute_test.sh and HANDOVER.md 4a.
        step=$(diff "$W/a.csv" "$W/b.csv" | grep -m1 '^< ' | sed 's/^< //' | cut -d, -f1)
        v="DIVERGES, first at step ${step:-?}"
        [ "$N" = 128 ] || fail=1
        [ "$N" = 128 ] && v="$v  <-- THE CONTROL FAILED; this harness is broken"
    fi
    printf '  KD_MAXMATRIX=%-4s exit %d/%d  %s\n' "$N" "$ra" "$rb" "$v"
done
[ "$fail" = 0 ] && echo "  -> PASS" || echo "  -> FAIL"
exit $fail
