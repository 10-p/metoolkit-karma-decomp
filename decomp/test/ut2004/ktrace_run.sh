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

# ---- WINDOWS, THROUGH THE SAME HARNESS. A `.exe` selects `wine`; everything else — the fixed
# step, the trace hook, the CSV, the checks below — is identical, which is the point. The Windows
# x64 build is LLP64 (`long` is 4 where Linux's is 8) and pointers are eight bytes on both, so a
# trace that MATCHES the 32-bit control is the strongest statement available about that target:
# not "it started", but "its physics is the same trajectory".
#
# ⚠ "IT RAN 300 s WITH NO FAULT" IS NOT EVIDENCE THAT IT SIMULATED ANYTHING. This project has
# already recorded that mistake once, on an Android device where the screenshot at 13 minutes was
# pixel-identical to the one before it while the log kept growing. The engine does not log per
# frame, so a smoke run that hangs and a smoke run that plays look the same. The trace is what
# distinguishes them, and it is why the Windows target is validated here and not there.
case "$BIN" in
    *.exe)
        command -v wine >/dev/null || { echo "$LABEL: no wine on PATH"; exit 2; }
        IS_PE=1; EXT=exe ;;
    *)  IS_PE=0; EXT=bin ;;
esac

[ -x "$BIN" ] || { echo "$LABEL: no such binary: $BIN"; exit 1; }
[ -f "$RUN/Maps/${MAP}.ut2" ] || { echo "$LABEL: no such map: $RUN/Maps/${MAP}.ut2"; exit 1; }

# ⚠⚠ RE-PIN THE VIEWPORT BEFORE EVERY RUN. THE ENGINE WRITES IT BACK ON EXIT, so one run that came
# up with a degenerate window poisons every run after it and the value FEEDS ON ITSELF. Measured
# 2026-09-01: /tmp/kd_run64 had drifted to WindowedViewportX=2 WindowedViewportY=28 and
# /tmp/kd_runkt to 2x1, and every native sweep of that day was reading it. A 2-pixel row makes
# `UGUITabControl::PreDraw` divide by a zero button count (INTEGER divide -> SIGFPE) and makes the
# canvas ask for 2.55 GB of FCanvasVertex. ★ AND THE 32-BIT CONTROL DOES NOT CATCH IT, because
# both binaries read the SAME ini — a control that shares the defect is not a control.
kd_pin_viewport() {
    local ini="$1/System/UT2004.ini"
    [ -f "$ini" ] || return 0
    python3 - "$ini" <<'KDPIN'
import re, sys
p = sys.argv[1]
want = {'WindowedViewportX': '1280', 'WindowedViewportY': '720',
        'FullscreenViewportX': '1280', 'FullscreenViewportY': '720',
        'MenuViewportX': '1280', 'MenuViewportY': '720'}
out, sec = [], None
for line in open(p, encoding='latin-1').read().split('\n'):
    m = re.match(r'^\[(.+)\]\s*$', line)
    if m:
        sec = m.group(1)
    elif sec in ('SDL2Drv.SDL2Client', 'WinDrv.WindowsClient'):
        k = line.split('=')[0]
        if k in want:
            line = '%s=%s' % (k, want[k])
    out.append(line)
open(p, 'w', encoding='latin-1').write('\n'.join(out))
KDPIN
}
kd_pin_viewport "$RUN"

cp -f "$BIN" "$RUN/System/ktrace-${LABEL}.${EXT}"
cd "$RUN/System" || exit 1
rm -f "$CSV" "$LOG"

echo "=== $LABEL: $MAP for ${SECS}s at ${FPS} fixed fps -> $CSV ==="
# NumBots=0 deliberately: the owner's vehicle/box defects are scripted-actor behaviour, and a bot
# spawning ragdolls on top of them adds bodies whose indices churn between runs.
#
# ⚠ THE TRACE PATH GOES TO THE ENGINE AS A COMMAND-LINE ARGUMENT, and under wine the engine sees
# a Windows path. `Z:` is wine's mapping of the Linux root, so `/tmp/x` is `Z:\tmp\x` — passing the
# bare Linux path writes the trace somewhere inside the wine prefix instead, and the harness would
# report "NO TRACE WRITTEN" for a run that worked perfectly.
KCSV="$CSV"
[ "$IS_PE" = 1 ] && KCSV="Z:$(echo "$CSV" | tr '/' '\\')"
LAUNCH="./ktrace-${LABEL}.${EXT}"
[ "$IS_PE" = 1 ] && LAUNCH="wine $LAUNCH"
# KD_LAUNCH_PREFIX lets a caller put something in front of the binary — `setarch
# --addr-no-randomize` is the one that matters, and `stack_shift.sh` sets it so its measurement
# varies the ONE thing it claims to vary. Empty by default; a normal ktrace run is unaffected.
WINEDEBUG="${WINEDEBUG:--all}" timeout --signal=TERM "$SECS" \
    xvfb-run -a -s "-screen 0 640x480x24" \
    ${KD_LAUNCH_PREFIX:-} $LAUNCH \
    "${MAP}?game=${GAME}?TimeLimit=0${URLOPTS}${EXTRA}" \
    -SOFTWARERENDERER -nohomedir \
    "-FIXEDFPS=${FPS}" "-KTRACE=${KCSV}" "-KTRACEEVERY=${EVERY}" "-KTRACEFRAMES=${FRAMES}" \
    ${KD_BONES:+-KTRACEBONES} ${KD_CONTACTS:+-KTRACECONTACTS} \
    > "$LOG" 2>&1
rc=$?
rm -f "./ktrace-${LABEL}.${EXT}"

# ⚠ THE WINDOWS CRT WRITES CRLF, AND THAT ALONE MAKES EVERY LINE DIFFER. Every comparison
# downstream — `md5sum`, `cmp`, `ktrace_diff.py` — would report a total divergence for a trace that
# agrees to the last digit, which is the most misleading possible failure: it looks like the port
# is broken and it is a line ending. Normalise here, once, where the file is produced.
if [ "$IS_PE" = 1 ] && [ -s "$CSV" ]; then
    tr -d '\r' < "$CSV" > "$CSV.lf" && mv -f "$CSV.lf" "$CSV"
fi

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

# KD_CONTACTS=1 adds the C and K rows — the contacts holding each body up. They answer a
# question the B rows structurally cannot: whether a state divergence is arithmetic amplifying
# (continuous) or a contact appearing on one side and not the other (discrete). Pair with
# ktrace_contacts.py, NOT with ktrace_diff.py, which ignores these rows.
if [ -n "${KD_CONTACTS:-}" ]; then
    crows=$(grep -c '^C,' "$CSV")
    krows=$(grep -c '^K,' "$CSV")
    echo "  contact rows: C=$crows K=$krows"
    if [ "$krows" -eq 0 ]; then
        printf '  \033[33m⚠ NO K ROWS\033[0m — every traced body was contact-free for the whole run.\n'
        printf '    On a vehicle map that is a finding, not a quiet result: a hover bike with no\n'
        printf '    contacts is not being held up by anything.\n'
    fi
fi

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
