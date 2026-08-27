#!/usr/bin/env bash
# gjk_bisect.sh — which FUNCTION of McdGjkPenetrationDepth crashes a live ONS match?
#
# The whole-object substitution in proven.txt ONS-CRASH-GJKPD localised the ONS-Torlan crash to
# this one object out of 145. This narrows it to one FUNCTION, by building a hybrid per function:
# that function comes from MathEngine's shipped object, everything else from ours.
#
#   ./gjk_bisect.sh [seconds]        # 90 s is plenty — the crash lands at ~25 s
#
# HOW THE HYBRID IS BUILT, because the obvious way does not work:
#   * `ar`-swapping a member of libKarmaDecomp.a is DISCARDED — cmake rebuilds the archive from its
#     objects, so the test measures nothing and still looks like a reproduction. Replace the
#     compiled .o instead and touch it, which works WITH the build system.
#   * The two objects export the same symbols, so they cannot simply be linked together. Rename the
#     one under test out of OUR object and rename every OTHER function out of the SHIPPED one, then
#     merge the pair with `ld -r` into a single .o that defines each symbol exactly once.
#
# ⚠ READ THE LEAVES FIRST. objcopy renames a symbol everywhere it appears, references included, so
# a shipped function's calls to its own siblings are renamed too. For a LEAF (McdGjkFaceLoad calls
# only into McdGjk.o) the result is exactly "this one function from theirs". For the driver
# (McdGjkPenetrationDepth calls its own helpers) it is coarser — "the driver AND its helpers".
set -uo pipefail
SECS="${1:-90}"
ENG=/home/ion/engines/engine-ut2004
REC_O="$ENG/build-native/Source/Engine/CMakeFiles/KarmaDecomp.dir/__/__/karma-decomp/generated/allobj/McdGjkPenetrationDepth.c.o"
SHIP_A="$ENG/Thirdparty/metoolkit/lib.rel/linux_single_gcc3.2/libMcdConvex.a"
WORK=/tmp/gjk-bisect
BIN="$ENG/build-native/Source/SDLLaunch/ut2004-pixo.bin"

FNS=(
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
# ⚠ REFUSE A CONTAMINATED SNAPSHOT. This script overwrites $REC_O with a hybrid, so running it (or
# doing a round by hand) and then running it again snapshots the HYBRID as "pristine" — every
# subsequent round then has two definitions of every orig_ symbol and `ld -r` fails on all of them.
# Rebuild first: `touch karma-decomp/generated/allobj/McdGjkPenetrationDepth.c && cmake --build ...`
if nm "$REC_O" 2>/dev/null | grep -qE 'rec__Z|orig__Z|orig_McdGjk'; then
    echo "REFUSING: $REC_O already contains rec_/orig_ symbols — it is a leftover hybrid, not the"
    echo "  recovered object. Rebuild it from source before bisecting."
    exit 2
fi
cp "$REC_O" "$WORK/pristine-rec.o"
( cd "$WORK" && ar x "$SHIP_A" McdGjkPenetrationDepth.o && mv McdGjkPenetrationDepth.o pristine-ship.o )

restore() { cp "$WORK/pristine-rec.o" "$REC_O"; touch "$REC_O"; }
trap restore EXIT

# NEGATIVE CONTROL FIRST. Rename every shipped function away, so the hybrid takes nothing from
# theirs and is our object plus dead weight. If this "survives", the harness is measuring the mere
# PRESENCE of the shipped object rather than any one function, and every result below is void.
FNS_PLUS=("__NONE__" "${FNS[@]}")
for F in "${FNS_PLUS[@]}"; do
    short=$(echo "$F" | sed 's/^_Z[0-9]*//; s/P16.*//; s/PKv.*//; s/Pff.*//; s/P9_.*//')
    if [ "$F" = "__NONE__" ]; then
        short="(negative control: nothing from shipped)"
        cp "$WORK/pristine-rec.o" "$WORK/rec.o"
    else
        objcopy --redefine-sym "$F=rec_$F" "$WORK/pristine-rec.o" "$WORK/rec.o" || { echo "$short: objcopy failed"; continue; }
    fi
    args=(); for G in "${FNS[@]}"; do [ "$G" = "$F" ] || args+=(--redefine-sym "$G=orig_$G"); done
    objcopy "${args[@]}" "$WORK/pristine-ship.o" "$WORK/ship.o" || { echo "$short: objcopy failed"; continue; }
    ld -r -m elf_i386 -o "$WORK/combined.o" "$WORK/rec.o" "$WORK/ship.o" 2>"$WORK/ld.err" || {
        echo "$short: ld -r failed: $(head -1 "$WORK/ld.err")"; continue; }
    cp "$WORK/combined.o" "$REC_O"; touch "$REC_O"
    ( cd "$ENG" && cmake --build build-native -j"$(nproc)" ) > "$WORK/build.log" 2>&1 || {
        echo "$short: BUILD FAILED — $(grep -m1 -iE 'undefined reference|error:' "$WORK/build.log")"; continue; }
    # ⚠ USE THE EXIT CODE, NOT THE TEXT. The first version of this script grepped the output for
    # "PASS" — and ons_smoke.sh prints "(124 = hit the time limit, which is the PASS case)" on
    # EVERY run, pass or crash. All nine functions reported PASS, which is impossible, and the
    # only reason it was caught is that an impossible answer is obviously impossible. A check
    # that cannot fail is worse than no check.
    out=$("$ENG/karma-decomp/test/ons_smoke.sh" "$BIN" "bisect" "$SECS" 2>&1); rc=$?
    if [ "$rc" -eq 0 ]; then
        echo "  *** $short  -> SURVIVED (this function is the culprit) ***"
    else
        echo "      $short  -> still crashes ($(echo "$out" | grep -oE 'after [0-9]+s' | head -1))"
    fi
done
echo "done — restoring the pristine recovered object"
