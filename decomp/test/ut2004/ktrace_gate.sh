#!/usr/bin/env bash
# ktrace_gate.sh — the trajectory gate, as something that RUNS rather than something you remember.
#
#   ktrace_gate.sh <candidate-binary> [control-binary]
#
#   UT2004_RUN_DIR=/tmp/kd_runkt \
#     ./decomp/test/ut2004/ktrace_gate.sh /tmp/kd_b64/Source/SDLLaunch/ut2004.bin \
#                                         /tmp/kd_b32_sse/Source/SDLLaunch/ut2004-pixo.bin
#
# WHY THIS EXISTS, AND WHY IT IS NOT ANOTHER ONE-OFF.
#
# Every gate in `run-standalone.sh` is a per-object comparison or an offline scene, and this
# project's own record says plainly what that is worth: `145/145 · 0 byte differences` has been
# green for a crash, for an uncompilable branch, for a doubled pointer stride and for a heap
# under-allocation. **The trace against the 32-bit control is the only instrument that has ever
# caught any of them.**
#
# And it was not wired into anything. `run-ut2004.sh` PRINTS a runbook — it says so in its own
# header — so the one gate that protects the port was a paragraph you had to read, remember, and
# type correctly, three times, before every commit. That is not a gate; it is a good intention. A
# regression only had to arrive on a day someone was in a hurry.
#
# WHAT IT ASSERTS, and each of the four has been the actual failure at least once:
#
#   1. THE TRACE EXISTS AND IS NOT DEGENERATE. `-KTRACE` writing nothing, or writing a header and
#      no bodies, is indistinguishable from "the physics agreed" if you only diff the files.
#   2. K ROWS ARE PRESENT. On a vehicle map, zero contact rows means nothing was holding anything
#      up — a hover bike in free fall agrees with itself perfectly.
#   3. THE CANDIDATE AGREES WITH THE CONTROL, BYTE FOR BYTE. Not "within a tolerance": the two
#      differ only in pointer width, so at a pinned timestep they are the same arithmetic.
#   4. ★★ THE CANDIDATE AGREES WITH ITSELF. NON-DETERMINISM IS THE SIGNATURE THIS PROJECT KEEPS
#      MEETING — one binary giving md5 a0d45750bf40 (K=1933) and 766866acdb8a (K=1351) against a
#      control's c31ed77b7323 (K=1396) — and a SINGLE run cannot see it. One sample that agrees is
#      silence, not evidence; the harness that skipped this check is how an address-dependent read
#      shipped twice.
#
# ⚠ THE CONTROL IS A POINTER-WIDTH CONTROL, NOT AN IMPLEMENTATION ONE. It is the same recovered
# sources built 32-bit with `-mfpmath=sse -msse2`, so the ONLY variable between the two arms is the
# pointer width — which is the whole question. (The implementation A/B, recovered-vs-MathEngine, is
# `../standalone/difftest_pair.sh` and `substitute_test.sh`, which link the SDK archives this
# repository vendors. It does not need the engine and never did; the engine-side `BUILD_KARMA_REF`
# path that used to duplicate it was removed on 2026-09-02.)
#
# ⚠ A `.exe` CANDIDATE IS RUN UNDER WINE by `ktrace_run.sh`, so the Windows x64 target goes through
# this same gate. Expect two values at the 1e-17 floor to differ there and nowhere else: mingw's
# libm and glibc's disagree in the last digits of `cos(pi/2)`, the body is at rest while it shows,
# and it is gone by the frame the body starts moving. `KD_TOL_LIBM=1` accepts exactly that shape.
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
. "$HERE/../../lib/kd-paths.sh"

CAND="${1:?usage: ktrace_gate.sh <candidate-binary> [control-binary]}"
CTRL="${2:-}"
SECS="${KD_SECS:-75}"
MAP="${KD_MAP:-test-karma-1}"
kd_require_ut2004 UT2004_RUN_DIR || exit 2

export KD_FRAMES="${KD_FRAMES:-40}"
export KD_CONTACTS="${KD_CONTACTS:-1}"
export KD_GAME="${KD_GAME:-Onslaught.ONSOnslaughtGame}"

fail=0
note() { printf '  %-34s %s\n' "$1" "$2"; }

run() {  # $1 = binary, $2 = label -> /tmp/ktrace-$2.csv
    "$HERE/ktrace_run.sh" "$1" "$2" "$SECS" "$MAP" >"/tmp/ktrace-gate-$2.out" 2>&1
    local f="/tmp/ktrace-$2.csv"
    if [ ! -s "$f" ]; then
        note "$2" "*** NO TRACE — see /tmp/ktrace-gate-$2.out"
        return 1
    fi
    # (1) and (2): a file that exists is not a run that simulated anything.
    local b k
    b=$(grep -c '^B,' "$f"); k=$(grep -c '^K,' "$f")
    if [ "$b" -eq 0 ]; then
        note "$2" "*** DEGENERATE — 0 body rows"
        return 1
    fi
    if [ "$k" -eq 0 ]; then
        note "$2" "*** NO CONTACT ROWS — nothing was holding anything up"
        return 1
    fi
    note "$2" "$(grep -c '^F,' "$f") frame(s), $b body-row(s), K=$k, md5 $(md5sum <"$f" | cut -c1-12)"
    return 0
}

echo "=== ktrace gate: $MAP, ${KD_FRAMES} frames, $KD_GAME ==="

# (4) THE SELF-CHECK COMES FIRST, because if the candidate is non-deterministic then every
# comparison below it is a coin toss and saying "it differs from the control" would be the wrong
# finding about the right symptom.
run "$CAND" gate-a || exit 1
run "$CAND" gate-b || exit 1
if cmp -s /tmp/ktrace-gate-a.csv /tmp/ktrace-gate-b.csv; then
    note "self-consistency" "two runs of one binary are IDENTICAL"
else
    note "self-consistency" "*** NON-DETERMINISTIC — the signature of an address-dependent read"
    echo "      that is the finding. Do not read the control comparison below as the cause."
    fail=1
fi

if [ -n "$CTRL" ]; then
    run "$CTRL" gate-ctl || exit 1
    if cmp -s /tmp/ktrace-gate-a.csv /tmp/ktrace-gate-ctl.csv; then
        note "vs the 32-bit control" "BYTE-IDENTICAL"
    elif [ "${KD_TOL_LIBM:-0}" = 1 ] && \
         [ "$(diff /tmp/ktrace-gate-a.csv /tmp/ktrace-gate-ctl.csv | grep -c '^<')" -le 8 ] && \
         ! diff /tmp/ktrace-gate-a.csv /tmp/ktrace-gate-ctl.csv | grep '^[<>]' |
             grep -qvE '[-+]?[0-9.]+e-1[5-9]'; then
        note "vs the 32-bit control" "differs ONLY at the 1e-15..1e-19 floor (KD_TOL_LIBM)"
    else
        note "vs the 32-bit control" "*** DIVERGES"
        diff /tmp/ktrace-gate-a.csv /tmp/ktrace-gate-ctl.csv | head -8 | sed 's/^/      /'
        fail=1
    fi
else
    note "vs the 32-bit control" "SKIPPED — no control binary given"
    echo "      ⚠ a self-consistent build can still be consistently wrong; pass one."
fi

[ "$fail" = 0 ] && { echo "  -> PASS"; exit 0; }
echo "  -> FAIL"; exit 1
