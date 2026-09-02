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
. "$(cd "$(dirname "$0")" && pwd)/../../lib/kd-paths.sh"

BIN="${1:?usage: ons_smoke.sh <binary> <label> [seconds]}"
LABEL="${2:?label}"
SECS="${3:-300}"
kd_require_ut2004 UT2004_RUN_DIR || exit 2
RUN="$UT2004_RUN_DIR"
MAP="${ONS_MAP:-ONS-Torlan}"
GAME="Onslaught.ONSOnslaughtGame"
# ⚠ THE RENDERER IS NOT THE SAME ON EVERY TARGET. Windows has no GLES, so it uses the LEGACY
# OpenGL device; passing -GL4ESRENDERER there produces a crash that looks like a physics bug and
# is not one — it cost a wrong conclusion in both directions on 2026-08-27.
RENDERER="${ONS_RENDERER:--GL4ESRENDERER}"
LOG="/tmp/ons-${LABEL}.log"

# ---- WINDOWS, THROUGH THE SAME HARNESS AND DELIBERATELY NOT A SECOND SCRIPT.
#
# The Windows x64 build with Karma is the one target that had never been RUN, and the reason it
# stayed that way for so long is that "run it" meant "write another harness". It does not: the
# match URL, the pass criterion, the crash grep and the Karma-did-something check are identical on
# every target, and only the LAUNCHER differs. So a `.exe` argument selects `wine` and everything
# below is shared.
#
# ⚠ THE RENDERER DEFAULT FLIPS HERE, and the comment above is why: Windows has no GLES, so
# `-GL4ESRENDERER` produces a crash that reads exactly like a physics fault. The default for a PE
# binary is the legacy OpenGL device.
# ⚠ AND IT NEEDS ITS OWN RUN TREE. The engine writes the viewport back into `System/UT2004.ini` on
# exit, so a Windows run and a Linux run sharing one tree poison each other — the same defect that
# produced two "engine UI bugs" that were neither (see ../../docs/STATE.md, the poisoned run tree).
case "$BIN" in
    *.exe)
        command -v wine >/dev/null || { echo "$LABEL: no wine on PATH"; exit 2; }
        IS_PE=1; EXT=exe
        RENDERER="${ONS_RENDERER:--OPENGLRENDERER}"
        ;;
    *)  IS_PE=0; EXT=bin ;;
esac

[ -x "$BIN" ] || { echo "$LABEL: no such binary: $BIN"; exit 1; }
cp -f "$BIN" "$RUN/System/ons-smoke-${LABEL}.${EXT}"
cd "$RUN/System" || exit 1
rm -f "$LOG"

echo "=== $LABEL: $MAP ($GAME) for ${SECS}s, $RENDERER ==="
start=$(date +%s)
# -nohomedir keeps logs/User.ini/saves in System/ instead of ~/.ut2004/ (ufront rule).
# bAutoNumBots + QuickStart + bPlayerMustBeReady=False are what actually make the match TICK:
# a URL with only ?game= loads the level, burns CPU and never leaves the pre-match state.
URL="$MAP?game=$GAME?TimeLimit=0?bAutoNumBots=True?QuickStart=True?bPlayerMustBeReady=False"
if [ "$IS_PE" = 1 ]; then
    # WINEDEBUG=-all keeps wine's own chatter out of a log this script greps for `Critical Error`.
    WINEDEBUG="${WINEDEBUG:--all}" timeout --signal=TERM "$SECS" \
        xvfb-run -a -s "-screen 0 640x480x24" \
        wine "./ons-smoke-${LABEL}.exe" "$URL" "$RENDERER" -nohomedir > "$LOG" 2>&1
else
    timeout --signal=TERM "$SECS" xvfb-run -a -s "-screen 0 640x480x24" \
        "./ons-smoke-${LABEL}.bin" "$URL" "$RENDERER" -nohomedir > "$LOG" 2>&1
fi
rc=$?
elapsed=$(( $(date +%s) - start ))
rm -f "./ons-smoke-${LABEL}.${EXT}"

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
