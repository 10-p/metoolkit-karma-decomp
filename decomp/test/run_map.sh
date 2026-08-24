#!/usr/bin/env bash
# run_map.sh — play a map headless with the shadow-instrumented engine.
#
#   run_map.sh <map> [seconds] [extra url args]
#
# Runs under Xvfb, so no GPU or display is needed. The shadow harness rewrites
# its CSV as the match proceeds, not only at exit, so a timeout or a crash still
# yields data.
#
# GAMETYPE MATTERS MORE THAN IT LOOKS. ONS vehicle factories only spawn under the
# Onslaught gametype, and a map whose only physics comes from bots and vehicles
# produces NO collision calls at all if neither appears — which reads exactly
# like a broken harness. Verify with the "Game class is" line in the run log.
set -uo pipefail

MAP="${1:?map name}"; SECS="${2:-180}"; EXTRA="${3:-}"
RUN="${KD_RUNTIME:-/home/ion/karma-run}"
BIN="${KD_BIN:?set KD_BIN to the instrumented ut2004 binary}"

# The prefix decides the gametype, and getting it wrong is not a small error: an
# AS- map run as xDeathMatch loads, starts, ticks, and produces FOURTEEN collision
# calls in four minutes, because none of the objectives or vehicles exist. It reads
# as "this map has no physics" rather than as a misconfiguration. Verified against
# the "Game class is" line in the run log, which is what the check below prints.
case "$MAP" in
    ONS-*|VCTF-*) GAME="${KD_GAME:-Onslaught.ONSOnslaughtGame}" ;;
    AS-*)         GAME="${KD_GAME:-UT2k4Assault.ASGameInfo}" ;;
    CTF-*)        GAME="${KD_GAME:-XGame.xCTFGame}" ;;
    BR-*)         GAME="${KD_GAME:-XGame.xBombingRun}" ;;
    DOM-*)        GAME="${KD_GAME:-XGame.xDoubleDom}" ;;
    *)            GAME="${KD_GAME:-XGame.xDeathMatch}" ;;
esac

export KD_SHADOW_OUT="${KD_SHADOW_OUT:-/tmp/kd_shadow_$MAP.csv}"
export KD_SHADOW_DIVERGENCES="${KD_SHADOW_DIVERGENCES:-/tmp/kd_shadow_${MAP}_divergences.txt}"
LOG="/tmp/kd_run_$MAP.log"
rm -f "$KD_SHADOW_OUT" "$KD_SHADOW_DIVERGENCES" "$LOG"

cp -f "$BIN" "$RUN/System/ut2004-shadow.bin"
cd "$RUN/System"
echo "running $MAP ($GAME) for ${SECS}s -> $KD_SHADOW_OUT"
timeout --signal=TERM "$SECS" xvfb-run -a -s "-screen 0 640x480x24" \
    ./ut2004-shadow.bin "$MAP?game=$GAME?TimeLimit=0$EXTRA" \
    -GL4ESRENDERER -nohomedir > "$LOG" 2>&1
echo "exit=$? (124 = hit the time limit, which is normal)"
# The LAST one, not the first. The first is always the Entry level's GameInfo,
# which reads as a silent fallback to the wrong gametype and sent one
# investigation down a blind alley.
echo "gametype actually used: $(grep -oE "Game class is '[^']+'" "$LOG" | tail -1 || echo '?')"
if grep -q 'SIGSEGV\|SIGBUS\|Critical Error' "$LOG"; then
    echo "*** THE ENGINE CRASHED — the numbers below stop where it died ***"
    grep -A16 -m1 'Signal:' "$LOG" | sed 's/^/  /'
fi

if [ -s "$KD_SHADOW_OUT" ]; then
    echo "--- interaction pairs the game actually called ---"
    awk -F, 'NR==1 || $5+0>0' "$KD_SHADOW_OUT" | column -s, -t
    n=$(awk -F, 'NR>1 && $5+0>0' "$KD_SHADOW_OUT" | wc -l)
    t=$(awk -F, 'NR>1' "$KD_SHADOW_OUT" | wc -l)
    echo "($n of $t registered pairs exercised)"
else
    echo "no shadow data written — check the gametype line above"
fi
