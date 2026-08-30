#!/usr/bin/env bash
# gjk_bisect_complement.sh — WHICH helpers are defective, when more than one is.
#
# gjk_bisect.sh asks "does taking function F from the shipped object fix the crash?" and every
# helper answered NO while the whole object answered YES. That is the signature of MORE THAN ONE
# defective function: swapping one still leaves the other, so no single swap can pass.
#
# This asks the complement, which does not have that blind spot:
#
#     keep exactly ONE function of OURS; take everything else in the object from theirs.
#
#     crashes  -> that one function is DEFECTIVE (it broke an otherwise-working object)
#     survives -> that one function is INNOCENT
#
# Built with weak symbols rather than renaming, so the shipped driver's calls still reach the
# function under test: --weaken-symbol H on theirs lets our strong H win the link, while every
# other function of ours is renamed out of the way.
set -uo pipefail
. "$(cd "$(dirname "$0")" && pwd)/../../lib/kd-paths.sh"
SECS="${1:-120}"
kd_require_ut2004 UT2004_BUILD_DIR || exit 2
REC_O="${KD_REC_O:-$(find "$UT2004_BUILD_DIR" -name McdGjkPenetrationDepth.c.o -print -quit 2>/dev/null)}"
SHIP_A="$KD_MT_LIB/libMcdConvex.a"
WORK=/tmp/gjk-comp
BIN="${KD_BIN:-$UT2004_BUILD_DIR/Source/SDLLaunch/ut2004-pixo.bin}"

ALL=(
  _Z14McdGjkFaceLoadP16_McdGjkFaceQueueP11_McdGjkFace
  _Z13McdGjkFaceAddP16_McdGjkFaceQueuePKfiii
  _Z18McdGjkFaceAddPointP16_McdGjkFaceQueuei
  _Z13McdGjkFacePopP16_McdGjkFaceQueue
  _Z17McdGjkFaceComparePKvS0_
  _Z19McdGjkFaceQueueInitP16_McdGjkFaceQueueP14_McdGjkSimplex
  _Z26McdGjkFindNextInflatePointPffP9_McdCacheP14_McdGjkSimplex
  _Z24McdGjkResultIsReasonableP9_McdCacheP14_McdGjkSimplex
  McdGjkPenetrationDepth
)

rm -rf "$WORK"; mkdir -p "$WORK"
if nm "$REC_O" 2>/dev/null | grep -qE 'rec__Z|orig__Z|rec_McdGjk'; then
    echo "REFUSING: $REC_O is a leftover hybrid. Rebuild it from source first."; exit 2
fi
cp "$REC_O" "$WORK/pristine-rec.o"
( cd "$WORK" && ar x "$SHIP_A" McdGjkPenetrationDepth.o && mv McdGjkPenetrationDepth.o pristine-ship.o )
restore() { cp "$WORK/pristine-rec.o" "$REC_O"; touch "$REC_O"; }
trap restore EXIT

# Positive control: keep NOTHING of ours -> the whole object is theirs -> must SURVIVE.
for KEEP in "__NONE__" "${ALL[@]}"; do
    short=$(echo "$KEEP" | sed 's/^_Z[0-9]*//; s/P16.*//; s/PKv.*//; s/Pff.*//; s/P9_.*//')
    rargs=(); wargs=()
    for G in "${ALL[@]}"; do
        if [ "$G" = "$KEEP" ]; then wargs+=(--weaken-symbol "$G")   # theirs yields to ours
        else rargs+=(--redefine-sym "$G=rec_$G"); fi                # ours out of the way
    done
    [ "$KEEP" = "__NONE__" ] && short="(positive control: nothing of ours)"
    objcopy "${rargs[@]}" "$WORK/pristine-rec.o" "$WORK/r.o" || { echo "$short: objcopy failed"; continue; }
    if [ ${#wargs[@]} -gt 0 ]; then
        objcopy "${wargs[@]}" "$WORK/pristine-ship.o" "$WORK/s.o" || { echo "$short: objcopy failed"; continue; }
    else cp "$WORK/pristine-ship.o" "$WORK/s.o"; fi
    ld -r -m elf_i386 -o "$WORK/c.o" "$WORK/r.o" "$WORK/s.o" 2>"$WORK/ld.err" || {
        echo "$short: ld -r failed: $(head -1 "$WORK/ld.err")"; continue; }
    cp "$WORK/c.o" "$REC_O"; touch "$REC_O"
    ( cd "$ENG" && cmake --build build-native -j"$(nproc)" ) > "$WORK/build.log" 2>&1 || {
        echo "$short: BUILD FAILED"; continue; }
    "$KD_T_UT/ons_smoke.sh" "$BIN" "comp" "$SECS" > "$WORK/run.log" 2>&1; rc=$?
    if [ "$rc" -eq 0 ]; then
        echo "      $short  -> survived (innocent)"
    else
        echo "  *** $short  -> CRASHES ($(grep -oE 'after [0-9]+s' "$WORK/run.log" | head -1)) — DEFECTIVE ***"
    fi
done
echo "done — restoring the pristine recovered object"
