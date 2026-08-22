#!/usr/bin/env bash
# run_map.sh — play a map headless with the shadow-instrumented engine.
#
#   run_map.sh <map> [seconds] [extra url args]
#
# Runs under Xvfb, so no GPU or display is needed. The shadow harness writes its
# CSV to $KD_SHADOW_OUT as the match proceeds, not only at exit, so a timeout or
# a crash still yields data.
set -uo pipefail
MAP="${1:?map name}"; SECS="${2:-180}"; EXTRA="${3:-}"
RUN="${KD_RUNTIME:-/home/ion/karma-run}"
BIN="${KD_BIN:?set KD_BIN to the instrumented ut2004 binary}"

export KD_SHADOW_OUT="${KD_SHADOW_OUT:-/tmp/kd_shadow_$MAP.csv}"
export KD_SHADOW_DIVERGENCES="${KD_SHADOW_DIVERGENCES:-/tmp/kd_shadow_${MAP}_divergences.txt}"
rm -f "$KD_SHADOW_OUT" "$KD_SHADOW_DIVERGENCES"

cp -f "$BIN" "$RUN/System/ut2004-shadow.bin"
cd "$RUN/System"
echo "running $MAP for ${SECS}s -> $KD_SHADOW_OUT"
timeout --signal=TERM "$SECS" xvfb-run -a -s "-screen 0 640x480x24" \
    ./ut2004-shadow.bin "$MAP?game=XGame.xDeathMatch?NumBots=6?TimeLimit=0$EXTRA" \
    -GL4ESRENDERER -nohomedir > "/tmp/kd_run_$MAP.log" 2>&1
echo "exit=$? (124 = hit the time limit, which is normal)"
[ -s "$KD_SHADOW_OUT" ] && column -s, -t < "$KD_SHADOW_OUT" || echo "no shadow data written"
