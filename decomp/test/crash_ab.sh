#!/usr/bin/env bash
# crash_ab.sh — alternate two engine builds on the same map and measure which
# one dies, and how fast.
#
#   crash_ab.sh <map> <seconds> <runs-per-arm> <label-A>=<binA> <label-B>=<binB>
#
# WHY IT ALTERNATES. The KHandleCollisions SIGSEGV is situational: the same
# binary and URL have produced a full clean match and an immediate crash on
# consecutive attempts. Comparing "four runs of A this morning" against "four
# runs of B this afternoon" cannot separate the build from the weather, so the
# arms are interleaved A,B,A,B and each run gets its own log.
#
# WHAT IT RECORDS. Not just crashed-or-not: seconds survived and the crash site.
# A build that always dies in the same place after ninety seconds and one that
# dies anywhere after eight minutes are different findings, and a pass/fail
# column loses that.
#
# Results go to $KD_AB_OUT (default /tmp/kd_ab.csv), rewritten after every run
# so a killed sweep still yields data.
set -uo pipefail

MAP="${1:?map}"; SECS="${2:?seconds per run}"; RUNS="${3:?runs per arm}"; shift 3
HERE="$(cd "$(dirname "$0")" && pwd)"
OUT="${KD_AB_OUT:-/tmp/kd_ab.csv}"
URL="${KD_AB_URL:-?Name=Player1?Class=Engine.Pawn?Character=Jakob?team=0?NumBots=8?MinPlayers=9?bAutoNumBots=False?QuickStart=True?bPlayerMustBeReady=False}"

ARMS=("$@")
[ ${#ARMS[@]} -ge 2 ] || { echo "need at least two arms"; exit 1; }

echo "arm,run,seconds,exit,outcome,crash_site" > "$OUT"
for i in $(seq 1 "$RUNS"); do
    for spec in "${ARMS[@]}"; do
        label="${spec%%=*}"; bin="${spec#*=}"
        [ -x "$bin" ] || { echo "missing binary for $label: $bin"; exit 1; }
        printf '%-10s run %s/%s ... ' "$label" "$i" "$RUNS"
        t0=$(date +%s)
        # KD_CENSUS keeps any shadow harness that happens to be in the build
        # from running anything twice; on a build with no harness it is inert.
        KD_BIN="$bin" KD_CENSUS=1 \
          KD_SHADOW_OUT="/tmp/kd_ab_${label}_$i.csv" \
          KD_SHADOW_DIVERGENCES="/tmp/kd_ab_${label}_${i}_div.txt" \
          timeout $((SECS + 120)) "$HERE/run_map.sh" "$MAP" "$SECS" "$URL" \
          > "/tmp/kd_ab_${label}_$i.out" 2>&1
        rc=$?
        t1=$(date +%s); dt=$((t1 - t0))
        cp -f "/tmp/kd_run_$MAP.log" "/tmp/kd_ab_${label}_$i.log" 2>/dev/null

        site=""; outcome="ran"
        # SIGTERM is `timeout` ending the run normally and the engine logs it as
        # `Signal: SIGTERM [terminate]`, so matching on `Signal:` alone marks
        # EVERY clean run as a crash. It did, for a whole sweep, and the numbers
        # had to be re-derived from the logs afterwards.
        if grep -qE 'Signal: SIG(SEGV|BUS|ILL|FPE|ABRT)|Critical Error' \
                "/tmp/kd_ab_${label}_$i.log" 2>/dev/null; then
            outcome="crash"
            # First backtrace frame that names something, skipping the signal
            # handler and the kernel trampoline.
            site=$(grep -oE '\bLog: \[ *[0-9]+\] +[^ ]+\(([^)+]+)' "/tmp/kd_ab_${label}_$i.log" \
                   | sed 's/.*(//' | grep -v '__kernel_sigreturn' | head -1)
            [ -n "$site" ] || site="unknown"
        elif [ "$dt" -lt $((SECS - 20)) ]; then
            outcome="exited-early"
        fi
        # Every log is kept as /tmp/kd_ab_<arm>_<run>.log precisely so a verdict
        # can be re-derived without re-running a 70-minute sweep.
        # Did the match reach kickoff at all? A level that loads and parks looks
        # identical to a clean run in every other column (HANDOVER.md 6).
        grep -q 'START MATCH' "/tmp/kd_ab_${label}_$i.log" 2>/dev/null || outcome="$outcome/no-kickoff"

        echo "$label,$i,$dt,$rc,$outcome,$site" >> "$OUT"
        printf '%ss  %s  %s\n' "$dt" "$outcome" "$site"
    done
done

echo
echo "=== $OUT ==="
column -s, -t < "$OUT"
echo
awk -F, 'NR>1 {n[$1]++; s[$1]+=$3; if($5 ~ /crash/) c[$1]++}
         END {printf "%-10s %6s %8s %10s\n","arm","runs","crashes","mean secs";
              for (a in n) printf "%-10s %6d %8d %10.0f\n", a, n[a], c[a]+0, s[a]/n[a]}' "$OUT"
