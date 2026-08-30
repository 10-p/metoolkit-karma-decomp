#!/usr/bin/env bash
# ktrace_run.sh — record a Karma state trace from a NATIVE build, for A/B against another build.
#
#   ktrace_run.sh <binary> <label> [seconds] [map]
#
# Writes /tmp/ktrace-<label>.csv (one row per simulated body per frame) and /tmp/ktrace-<label>.log.
# Pair it with ktrace_diff.py.
#
# ★ -FIXEDFPS IS NOT OPTIONAL AND THE ENGINE ENFORCES IT. KTickLevelKarma derives FullTimestep,
# SubTimestep and NumSubSteps from DeltaSeconds, so two runs at different frame rates integrate
# different equations. Without it the trace hook appErrorf's rather than emit a file that would be
# diffed in good faith. That is also why SECONDS below is wall-clock but the trace is indexed by
# FRAME: at a fixed step, frame N is the same simulated instant in both runs no matter how fast
# each machine got there.
#
# -SOFTWARERENDERER on purpose: this needs no GPU, and a wedged GPU has produced failures that read
# exactly like physics regressions. The physics is identical either way once the step is pinned.
set -uo pipefail
. "$(cd "$(dirname "$0")" && pwd)/../../lib/kd-paths.sh"

BIN="${1:?usage: ktrace_run.sh <binary> <label> [seconds] [map]}"
LABEL="${2:?label}"
SECS="${3:-90}"
MAP="${4:-test-karma-1}"
kd_require_ut2004 UT2004_RUN_DIR || exit 2
RUN="$UT2004_RUN_DIR"
GAME="${KD_GAME:-XGame.xDeathMatch}"
FPS="${KD_FPS:-30}"
EVERY="${KD_EVERY:-1}"
FRAMES="${KD_FRAMES:-0}"
EXTRA="${KD_EXTRA:-}"
# The default is a bot-free scripted run — the vehicle/box scenario, where a bot spawning
# ragdolls would add bodies whose indices churn between runs. KD_URLOPTS REPLACES it rather
# than appending, because appending `?NumBots=1` after `?NumBots=0` leaves two of the same
# option in one URL and which one wins is not a thing to find out by accident.
URLOPTS="${KD_URLOPTS:-?NumBots=0?QuickStart=True?bPlayerMustBeReady=False}"

CSV="/tmp/ktrace-${LABEL}.csv"
LOG="/tmp/ktrace-${LABEL}.log"

[ -x "$BIN" ] || { echo "$LABEL: no such binary: $BIN"; exit 1; }
[ -f "$RUN/Maps/${MAP}.ut2" ] || { echo "$LABEL: no such map: $RUN/Maps/${MAP}.ut2"; exit 1; }

cp -f "$BIN" "$RUN/System/ktrace-${LABEL}.bin"
cd "$RUN/System" || exit 1
rm -f "$CSV" "$LOG"

echo "=== $LABEL: $MAP for ${SECS}s at ${FPS} fixed fps -> $CSV ==="
# NumBots=0 deliberately: the owner's vehicle/box defects are scripted-actor behaviour, and a bot
# spawning ragdolls on top of them adds bodies whose indices churn between runs.
timeout --signal=TERM "$SECS" xvfb-run -a -s "-screen 0 640x480x24" \
    "./ktrace-${LABEL}.bin" \
    "${MAP}?game=${GAME}?TimeLimit=0${URLOPTS}${EXTRA}" \
    -SOFTWARERENDERER -nohomedir \
    "-FIXEDFPS=${FPS}" "-KTRACE=${CSV}" "-KTRACEEVERY=${EVERY}" "-KTRACEFRAMES=${FRAMES}" \
    ${KD_BONES:+-KTRACEBONES} \
    > "$LOG" 2>&1
rc=$?
rm -f "./ktrace-${LABEL}.bin"

echo "  exit=$rc (124 = hit the time limit, which is the normal case)"
echo "  gametype: $(grep -oE "Game class is '[^']+'" "$LOG" | tail -1 || echo '?')"

if grep -qE 'SIGSEGV|SIGBUS|SIGILL|SIGFPE|Critical Error|Assertion failed' "$LOG"; then
    echo "  *** CRASHED ***"
    grep -A14 -m1 -E 'Signal:|Critical Error' "$LOG" | sed 's/^/    /'
fi

if [ ! -s "$CSV" ]; then
    echo "  *** NO TRACE WRITTEN — check the log for the -KTRACE line ***"
    grep -iE "KTRACE|FIXEDFPS" "$LOG" | head -5 | sed 's/^/    /'
    exit 1
fi

frames=$(grep -c '^F,' "$CSV")
rows=$(grep -c '^B,' "$CSV")
bodies=$(awk -F, '/^B,/{print $4"/"$5}' "$CSV" | sort -u | wc -l)
echo "  frames: $frames   body-rows: $rows   distinct bodies: $bodies"

# ★ SAY IT WHEN THE RUN FELL SHORT, because everything downstream compares the LAST frame of two
# files and a short run therefore compares two different INSTANTS.
#
# Measured 2026-08-30: a legacy arm captured 94 frames and its candidate 35, same map, same
# gametype, same requested KD_FRAMES. ktrace_score.py read that as "8 MISMATCH — bodies diverging
# by up to 53 units". Truncated to matched frames it read MATCH 15/15. The whole difference was the
# reference having fallen a further 59 frames.
#
# The scorer truncates for itself now, so this is not load-bearing — but a short run is still a
# WEAKER measurement than the one you asked for, and it should not be silent. The usual cause is
# not a crash: ULevel::Tick guards Karma with IsPaused(), so a match that pauses itself stops the
# trace dead while the HUD — which ticks on the render path, outside that guard — keeps logging.
# HUD output is not evidence the level is running.
if [ "${FRAMES:-0}" -gt 0 ] && [ "$frames" -lt "$FRAMES" ]; then
    pct=$(( frames * 100 / FRAMES ))
    printf '  \033[33m⚠ SHORT RUN\033[0m — asked for %s frames, captured %s (%s%%).\n' \
           "$FRAMES" "$frames" "$pct"
    printf '    Compare only against another trace of the same length, or let\n'
    printf '    ktrace_score.py truncate. A paused match, not a crash, is the usual cause.\n'
fi
# Contacts the ENGINE threw away before Karma ever saw them. A non-zero count here is a finding in
# itself: KPerContactCB rejects a contact whose normal is not unit length, and a rejected contact is
# a body with nothing holding it up.
bad=$(grep -c "Bad Normal Length" "$LOG" || true)
nok=$(grep -c "no KParams" "$LOG" || true)
echo "  rejected contacts: bad-normal=$bad no-kparams=$nok"
