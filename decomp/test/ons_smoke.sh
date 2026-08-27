#!/usr/bin/env bash
# ons_smoke.sh — play a 5-minute Onslaught match on a NATIVE build and report whether it survived.
#
#   ons_smoke.sh <binary> <label> [seconds]
#
# The acceptance check the owner asked for when the recovered Karma became the default on every
# 32-bit target: does each build start, and does it play ONS for five minutes without crashing.
#
# ONS-Torlan is the map on purpose. ONS is the gametype that spawns VEHICLES, and vehicles plus
# ragdolls are the entire reason Karma exists — a DM map exercises almost none of it, and
# karma-decomp/test/run_map.sh records an AS- map run as xDeathMatch producing FOURTEEN collision
# calls in four minutes, which reads as "no physics" rather than as a misconfiguration.
#
# Exit status: 0 = survived the whole run, 1 = crashed or died early.
set -uo pipefail

BIN="${1:?usage: ons_smoke.sh <binary> <label> [seconds]}"
LABEL="${2:?label}"
SECS="${3:-300}"
RUN="${KD_RUNTIME:-/home/ion/karma-run}"
MAP="${ONS_MAP:-ONS-Torlan}"
GAME="Onslaught.ONSOnslaughtGame"
# ⚠ THE RENDERER IS NOT THE SAME ON EVERY TARGET. Windows has no GLES, so it uses the LEGACY
# OpenGL device; passing -GL4ESRENDERER there produces a crash that looks like a physics bug and
# is not one — it cost a wrong conclusion in both directions on 2026-08-27.
RENDERER="${ONS_RENDERER:--GL4ESRENDERER}"
LOG="/tmp/ons-${LABEL}.log"

[ -x "$BIN" ] || { echo "$LABEL: no such binary: $BIN"; exit 1; }
cp -f "$BIN" "$RUN/System/ons-smoke-${LABEL}.bin"
cd "$RUN/System" || exit 1
rm -f "$LOG"

echo "=== $LABEL: $MAP ($GAME) for ${SECS}s, $RENDERER ==="
start=$(date +%s)
# -nohomedir keeps logs/User.ini/saves in System/ instead of ~/.ut2004/ (ufront rule).
# bAutoNumBots + QuickStart + bPlayerMustBeReady=False are what actually make the match TICK:
# a URL with only ?game= loads the level, burns CPU and never leaves the pre-match state.
timeout --signal=TERM "$SECS" xvfb-run -a -s "-screen 0 640x480x24" \
    "./ons-smoke-${LABEL}.bin" \
    "$MAP?game=$GAME?TimeLimit=0?bAutoNumBots=True?QuickStart=True?bPlayerMustBeReady=False" \
    "$RENDERER" -nohomedir > "$LOG" 2>&1
rc=$?
elapsed=$(( $(date +%s) - start ))
rm -f "./ons-smoke-${LABEL}.bin"

echo "  exit=$rc after ${elapsed}s (124 = hit the time limit, which is the PASS case)"
echo "  gametype: $(grep -oE "Game class is '[^']+'" "$LOG" | tail -1 || echo '?')"

# Karma actually did something, rather than the level merely loading. KarmaData/*.ka are the
# ragdoll/vehicle asset files; a NO_KARMA build never opens one.
ka=$(grep -ciE "\.ka\b" "$LOG" || true)
veh=$(grep -ciE "ONSVehicle|ONSHoverTank|ONSRV|Vehicle" "$LOG" || true)
echo "  karma asset lines: $ka   vehicle mentions: $veh"

if grep -qE 'SIGSEGV|SIGBUS|SIGILL|SIGFPE|Critical Error|Assertion failed' "$LOG"; then
    echo "  *** CRASHED ***"
    grep -A14 -m1 -E 'Signal:|Critical Error' "$LOG" | sed 's/^/    /'
    exit 1
fi
if [ "$rc" -ne 124 ] && [ "$elapsed" -lt "$((SECS - 10))" ]; then
    echo "  *** EXITED EARLY at ${elapsed}s of ${SECS}s (rc=$rc) ***"
    tail -20 "$LOG" | sed 's/^/    /'
    exit 1
fi
echo "  PASS — ran the full ${SECS}s with no fault"
