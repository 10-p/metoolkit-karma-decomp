#!/usr/bin/env bash
# stack_shift.sh — does this build's PHYSICS depend on the contents of the stack?
#
#   stack_shift.sh <binary> <label> [seconds] [map]
#
# Runs the SAME binary twice, changing nothing but the size of the environment, and diffs the two
# Karma traces. The environment sits above the initial stack pointer, so 20 KB more of it shifts
# every stack frame in the process by 20 KB — and a program that only ever reads what it has
# written cannot notice.
#
# ★ WHY THIS IS A GATE AND NOT A CURIOSITY. A pointer-width defect that reads UNINITIALISED
# memory is invisible to every instrument this project already has:
#
#   the i386 acceptance test   compiles the same source at 32-bit, where the read is in range
#   ASan                       reports out-of-bounds and use-after-free, NOT uninitialised reads
#   ktrace against a control   reports A DIFFERENCE, and cannot say whether it is arithmetic
#   a repeated run             agrees with itself, because the stack is identical each time
#
# So the defect reads as "the two widths disagree by a bit and it grows", which is the reading
# this project spent a day on before. Measured 2026-09-01 on test-karma-1 + Onslaught:
#
#     32-bit (SSE)   15 of 15 bodies BIT-IDENTICAL across the shift
#     LP64           ONSHoverBike3 diverges at frame 4; the other 14 bit-identical
#
# ⚠ A PASS IS NECESSARY, NOT SUFFICIENT. It only sees a read whose VALUE changes with the shift.
# A read of memory that happens to hold the same bytes either way passes and is still a defect.
#
# ⚠ AND A FAIL IS NOT AUTOMATICALLY A KARMA FAULT. The engine has its own uninitialised reads
# (`UNameProperty::ImportText` is one, and ASan finds it at startup on every build). What makes
# this attributable is the CONTROL: run the same map through the 32-bit build, where every
# pointer fits, and see whether it moves too.
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
. "$HERE/../../lib/kd-paths.sh"

BIN="${1:?usage: stack_shift.sh <binary> <label> [seconds] [map]}"
LABEL="${2:?label}"
SECS="${3:-60}"
MAP="${4:-test-karma-1}"

: "${KD_FRAMES:=12}"
: "${KD_GAME:=Onslaught.ONSOnslaughtGame}"
export KD_FRAMES KD_GAME

# ⚠⚠ SEVERAL SHIFTS, NOT ONE, AND THAT IS THE WHOLE DIFFERENCE BETWEEN A RESULT AND A COIN TOSS.
# The first version of this harness compared ONE shifted run against one unshifted one. A build
# under investigation PASSED that, the pass was taken as "the defect is in the C locals", and a
# whole attribution was built on it — then the same binary FAILED the same test half an hour
# later. A shift only exposes the read if it happens to change the BYTES that get read; one
# sample that agrees is silence, not evidence.
SHIFTS="0 4096 20000 65536"
echo "=== $LABEL: $(basename "$BIN") at $(echo $SHIFTS | wc -w) environment sizes ==="

i=0
FILES=""
for n in $SHIFTS; do
    i=$((i+1))
    if [ "$n" = 0 ]; then
        KD_CONTACTS=1 "$HERE/ktrace_run.sh" "$BIN" "${LABEL}-$i" "$SECS" "$MAP" >/dev/null 2>&1
    else
        KD_STACK_SHIFT="$(python3 -c "print('X'*$n)")" \
        KD_CONTACTS=1 "$HERE/ktrace_run.sh" "$BIN" "${LABEL}-$i" "$SECS" "$MAP" >/dev/null 2>&1
    fi
    f="/tmp/ktrace-${LABEL}-$i.csv"
    [ -s "$f" ] || { echo "  *** NO TRACE at $f — see /tmp/ktrace-${LABEL}-$i.log ***"; exit 2; }
    FILES="$FILES $f"
done

set -- $FILES
A="$1"
shift
bad=0
for B in "$@"; do
    out="$(python3 "$HERE/../standalone/ktrace_diff.py" "$A" "$B" --tol=0 2>&1)"
    if echo "$out" | grep -q 'behaviourally IDENTICAL'; then
        printf '  %-28s same\n' "$(basename "$B")"
    else
        bad=$((bad+1))
        printf '  %-28s MOVED\n' "$(basename "$B")"
        echo "$out" | grep -vE '^\s*$' | tail -4 | sed 's/^/      /'
    fi
done

if [ "$bad" -eq 0 ]; then
    printf '  \033[32m-> PASS\033[0m  the trajectory is the same at every environment size.\n'
    printf '    ⚠ NECESSARY, NOT SUFFICIENT: a read of memory that happens to hold the same\n'
    printf '    bytes at all of these sizes passes and is still a defect.\n'
    exit 0
fi
printf '  \033[31m-> FAIL\033[0m  %d of %d shifts moved the trajectory.\n' "$bad" "$#"
printf '    Something on the physics path is reading memory it did not write. Run the 32-bit\n'
printf '    build through this same harness before attributing it: if that moves too, it is\n'
printf '    not a pointer-width defect.\n'
exit 1
