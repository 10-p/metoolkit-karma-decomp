#!/usr/bin/env bash
# lp64_pipeline.sh — the whole LP64 post-pass chain, with its gate.
#
#   ./lp64_pipeline.sh [scene.c ...]
#
# §4's 95-second `recover.py` output is NOT arm64-correct source; the post-passes
# in `tools/` are what make it so, and they EDIT IN PLACE. This runs them on a
# copy in the right order, applies the i386 acceptance test that says they were
# no-ops on the shipped target, and only then runs the LP64 harness. Doing those
# by hand is four commands with two ways to get the order wrong, and the copy is
# the one that is easy to forget.
set -u
HERE="$(cd "$(dirname "$0")/.." && pwd)"
MT="$(cd "$HERE/../Thirdparty/metoolkit" && pwd)"
INC="$MT/include"
SRC="${KD_OUT_SRC:-/tmp/kd_out}"
DST="${KD_OUT:-/tmp/kd_lp64}"
BUILD="${KD_BUILD:-/tmp/kd_build}"

[ -d "$SRC/allobj" ] || { echo "  no recovery at $SRC — run tools/recover.py first"; exit 2; }
rm -rf "$DST" && cp -a "$SRC" "$DST" || exit 2
echo "== post-passes on a COPY ($DST) =="
python3 "$HERE/tools/fix_baked_sizeof.py" "$DST/allobj" "$BUILD" "$MT" || exit 2
python3 "$HERE/tools/fix_strides.py"   "$DST/allobj" "$BUILD" "$MT" || exit 2
# ⚠ TWICE, AND THE SECOND RUN IS NOT BELT-AND-BRACES. Each run resolves one
# more link of a type chain: a call names `pvVar2`, `pvVar2` names the field
# `first`, and only then can the local that READS `first` be typed. The pass is
# idempotent — a repaired site has no literal left to match — so the second run
# sees only what the first could not resolve.
python3 "$HERE/tools/fix_literal_offsets.py" "$DST/allobj" "$BUILD" "$MT" | tail -3 || exit 2
python3 "$HERE/tools/fix_literal_offsets.py" "$DST/allobj" "$BUILD" "$MT" | tail -3 || exit 2
python3 "$HERE/tools/fix_derived_fields.py" "$DST/allobj" "$BUILD" "$MT" | head -2 || exit 2
python3 "$HERE/tools/fix_arena_carve.py" "$DST/allobj" "$BUILD" "$MT" | head -3 || exit 2
python3 "$HERE/tools/fix_vtable_offsets.py" "$DST/allobj" "$BUILD" "$MT" | head -2 || exit 2
python3 "$HERE/tools/fix_ptrwidth.py"    "$DST/allobj" "$BUILD" "$MT" || exit 2
# AFTER fix_ptrwidth: it widens the CASTS, and these two key on what it wrote.
# fix_narrow_pointers first — it widens the locals whose masks the next pass has
# to recognise, and a mask on a widened local has no cast in it to match.
python3 "$HERE/tools/fix_narrow_pointers.py" "$DST/allobj" "$BUILD" "$MT" | head -4 || exit 2
python3 "$HERE/tools/fix_align_masks.py" "$DST/allobj" "$BUILD" "$MT" | head -2 || exit 2
python3 "$HERE/tools/fix_frame_slots.py" "$DST/allobj" "$BUILD" "$MT" | head -2 || exit 2

# ---- THE DETECTOR THIS PASS COULD BLIND. check_frame_bounds reads CONSTANT
# offsets and constant array bounds, and fix_frame_slots replaces both with
# constant EXPRESSIONS. It has been taught the new spelling; running it here
# says so out loud rather than leaving a zero to be trusted.
echo "== frame bounds after the post-passes =="
python3 "$HERE/tools/check_frame_bounds.py" "$DST/allobj" "$BUILD" | tail -1 || exit 1

# ---- THE ACCEPTANCE TEST. `intptr_t` IS `int` at 32-bit pointer width and the
# rewritten sizes ARE the constants they replaced, so both passes are no-ops
# here BY CONSTRUCTION — which is exactly the kind of claim that stops being
# true quietly. It has already caught `fix_baked_sizeof` changing CxSmallSort by
# 64 bytes and `McdModelPairManager` by a scheduling difference.
#
# ⚠ THE FILE MUST KEEP ITS NAME. gcc records the source basename in an STT_FILE
# symbol, so compiling identical text from `/tmp/t.c` yields a different object
# and reads exactly like a codegen change.
echo "== i386 acceptance: every object byte-identical =="
IF="-I$INC -I$INC/McdCommon -I$INC/McdPrimitives -I$INC/McdFrame -I$INC/MeGlobals"
IF="$IF -I$INC/MdtBcl -I$INC/MdtKea -I$INC/Mst -I$INC/MeApp -I$HERE/include"
CF="-m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion"
CF="$CF -Wno-incompatible-pointer-types -DLINUX $IF"
W=$(mktemp -d); trap 'rm -rf "$W"' EXIT
bad=0; fail=0; n=0
for o in "$BUILD"/*.o; do
    b=$(basename "$o" .o); n=$((n+1))
    cp "$DST/allobj/$b.c" "$W/$b.c"
    if gcc $CF -c -o "$W/$b.o" "$W/$b.c" 2>/dev/null; then
        cmp -s "$o" "$W/$b.o" || { echo "  DIFFERS: $b"; bad=$((bad+1)); }
    else
        echo "  DID NOT COMPILE: $b"; fail=$((fail+1))
    fi
done
echo "  $n object(s), $fail compile failure(s), $bad byte difference(s)"
[ "$fail" = 0 ] && [ "$bad" = 0 ] || { echo "  -> STOP: the post-passes are not no-ops at i386."; exit 1; }

echo "== LP64 =="
KD_OUT="$DST" KD_SKIP_CONTROL="${KD_SKIP_CONTROL:-1}" "$HERE/test/lp64_run.sh" "$@"
