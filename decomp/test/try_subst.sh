#!/usr/bin/env bash
# try_subst.sh — link the engine against a chosen subset of recovered objects
# and report whether it survives engine init.
#
# The 108-object substitution segfaults in KCreateAssetDB before a match starts
# (HANDOVER 7c). That is early enough that a 60 s run settles it, which is what
# makes bisecting 108 objects affordable at ~2 min a cycle.
#
#   try_subst.sh <objdir> [name...]      names = substitute ONLY these
#
# Prints RUNS or CRASHES plus the top frame, and leaves the engine log in place.
set -uo pipefail
ROOT=/home/ion/engines/engine-ut2004
KD=$ROOT/karma-decomp
BUILD=$ROOT/build-subst108
OBJDIR="${1:?object dir}"; shift

"$KD/test/make_substituted_metoolkit.sh" "$OBJDIR" "$ROOT/Thirdparty/metoolkit" \
    /tmp/mt_subst "$@" 2>&1 | tail -1
cmake --build "$BUILD" -j"$(nproc)" >/tmp/try_link.log 2>&1 || { echo "LINK FAILED"; exit 2; }

URL='?Name=Player1?Class=Engine.Pawn?Character=Jakob?team=0?NumBots=4?MinPlayers=5?bAutoNumBots=False?QuickStart=True?bPlayerMustBeReady=False'
KD_BIN="$BUILD/Source/SDLLaunch/ut2004-karma-pixo.bin" \
    "$KD/test/run_map.sh" DM-Insidious 60 "$URL" >/tmp/try_run.log 2>&1
LOG=/tmp/kd_run_DM-Insidious.log

# "did it SIGSEGV" is the WRONG question and cost a cycle here. The stock,
# unsubstituted engine also faults under Xvfb in UCanvas::DrawTile — a HUD
# rendering path, nothing to do with Karma — and a grep for SIGSEGV therefore
# reports CRASHES for a build that started a match perfectly well. Ask instead
# whether the engine got where it was going.
if grep -q 'ScriptLog: START MATCH' "$LOG" 2>/dev/null; then
    echo "RUNS     reached START MATCH"
    exit 0
fi
echo "FAILS    $(grep -A6 'Developer Backtrace:' "$LOG" | sed -n '4,7p' |
                sed 's/.*bin(//;s/).*//;s/+0x[0-9a-f]*//' | tr '\n' ' ')"
exit 1
