#!/usr/bin/env bash
# run-standalone.sh — every gate that needs NOTHING but this repository.
#
#   ./decomp/test/run-standalone.sh              # all of it
#   ./decomp/test/run-standalone.sh -l           # list what it would run
#   KD_QUICK=1 ./decomp/test/run-standalone.sh   # skip the two slow ones
#
# This is the tier CI runs, and the contract is exact: a bare `git clone`, no
# environment variables set, no game installed, no engine checkout. If anything
# here needs a path from outside the repo, it is in the wrong tier — the ut2004
# tier is `run-ut2004.sh` and it fails loudly rather than guessing.
#
# ⚠ WHAT A GREEN RUN HERE DOES NOT MEAN. Every gate in this tier is either a
# per-object comparison or an offline physics scene. None of them runs the game.
# "145 objects pass" is a statement about 145 separate links, not about the
# program: `difftest_pair.sh` substitutes the INTERACTION object and links the
# GEOMETRY objects from the shipped library, so a broken McdSphereGetRadius is
# never in the loop and reads byte-identical before and after a repair. The gate
# that compares the trajectory the calls ADD UP TO is `ktrace`, and it is in the
# ut2004 tier because it needs a running match. See proven.txt MCD-GEOM-FLOAT-FIELDS.
set -uo pipefail
. "$(cd "$(dirname "$0")" && pwd)/../lib/kd-paths.sh"
. "$(cd "$(dirname "$0")" && pwd)/baselines.sh"

kd_require_metoolkit || exit 2

SRC="${KD_OUT_SRC}/allobj"
LP64="${KD_LP64:-/tmp/kd_lp64}/allobj"
pass=0; fail=0; skip=0
FAILED=""

run() {   # run <name> <why-it-matters> -- <command...>
    local name="$1" why="$2"; shift 3
    printf '\n\033[1m== %s\033[0m — %s\n' "$name" "$why"
    # rc must be captured BEFORE anything else runs, or $? is the exit status of
    # the arithmetic that increments the counter. That reported "(exit 0)" next
    # to the word FAIL, which is exactly the kind of self-contradicting output
    # that gets a gate ignored.
    local rc=0
    "$@" || rc=$?
    if [ "$rc" = 0 ]; then
        pass=$((pass + 1)); printf '   \033[32mPASS\033[0m %s\n' "$name"
    else
        fail=$((fail + 1)); FAILED="$FAILED $name"
        printf '   \033[31mFAIL\033[0m %s (exit %d)\n' "$name" "$rc"
    fi
}

skipped() { skip=$((skip + 1)); printf '\n== %s — SKIPPED: %s\n' "$1" "$2"; }

if [ "${1:-}" = "-l" ]; then
    sed -n 's/^    run \(.*\)$/  \1/p' "$0" | sed 's/ --.*//'
    exit 0
fi

# --- preconditions ----------------------------------------------------------
# The recovery is an INPUT to most of this tier, and regenerating it is 95
# seconds we should not spend implicitly — say what to run instead of failing
# thirteen gates with the same cause.
if [ ! -d "$SRC" ] || [ ! -d "$KD_BUILD" ]; then
    cat <<EOT
run-standalone: no recovery to gate.

  python3 decomp/tools/recover.py --out-dir $KD_OUT_SRC --build-dir $KD_BUILD

⚠ --build-dir is not optional: it defaults to $KD_BUILD, which is the i386
  baseline every acceptance test compares against, so a run that forgets it
  overwrites the thing it was about to be measured against.
EOT
    exit 2
fi

n_c=$(find "$SRC" -name '*.c' | wc -l)
n_o=$(find "$KD_BUILD" -name '*.o' | wc -l)
echo "run-standalone: $n_c recovered source(s), $n_o compiled object(s)"
echo "  metoolkit  $METOOLKIT_DIR"
echo "  lab        $KD_LAB_DIR"

# --- the product is what the checked-in tree says it is ---------------------
if [ -d "$LP64" ]; then
    run split-libraries "the shipped tree is what the generator would produce" -- \
        python3 "$KD_ROOT/tools/split_libraries.py" "$LP64" --check
else
    skipped split-libraries "no post-passed tree at $LP64 — run lp64_pipeline.sh"
fi

# --- interface + structure --------------------------------------------------
run frame-bounds "a defect no behavioural test can find" -- \
    python3 "$KD_ROOT/tools/check_frame_bounds.py" "$SRC" "$KD_BUILD"

# ⚠ PASS THE BUILD DIR. Without it every HELD object's violations count as
# failures, and a held object's violations are the detector working.
run symbol-bindings "does each object export what the shipped one exported — name, BINDING and size" -- \
    python3 "$KD_ROOT/tools/check_symbol_bindings.py" "$KD_BUILD" "$KD_OBJ_DIR"

run code-call-check "no call to something that is not code" -- \
    python3 "$KD_ROOT/tools/code_call_check.py" "$SRC"

# --- portability ------------------------------------------------------------
if [ -f "$HOME/emsdk/emsdk_env.sh" ]; then
    run wasm-check "all of it compiles for wasm32 — the actual goal" -- \
        "$KD_T_STD/wasm_check.sh" "$SRC" "$KD_BUILD" "$METOOLKIT_DIR"
else
    # A missing emsdk is a missing toolchain, not a failure of this repository.
    skipped wasm-check "no emsdk at ~/emsdk"
fi

if [ -n "$KD_NDK" ] && [ -x "$KD_NDK/aarch64-linux-android21-clang" ]; then
    run ptrwidth "pointer truncation, as a compile-time diagnostic — no arm64 hardware needed" -- \
        "$KD_T_STD/ptrwidth_check.sh" "$SRC" "$KD_BUILD"
else
    # A missing NDK is a missing toolchain, not a failure of this repository.
    skipped ptrwidth "no Android NDK (set ANDROID_NDK_HOME or KD_NDK)"
fi

# --- behaviour --------------------------------------------------------------
# ⚠ THESE TWO ARE CHECKED AGAINST test/baselines.sh, NOT AGAINST THEIR EXIT
# CODES, and that is not laxity — it is the only reading that can fail usefully.
# Both exit non-zero on a CORRECT tree and always have (proven.txt: "those three
# FAILs ARE the baseline, not a regression"). A runner that believed the exit
# code would report three failures on a perfect tree, every time, until nobody
# read the red any more. Asserting on the measured numbers catches a NEW
# divergence and also one that disappeared, which an exit code cannot.

check_difftest() {   # stdin: the difftest log
    local log="$1" bad=0 pair cnt dims want_c want_d
    while read -r pair cnt dims; do
        want_c=0; want_d=0
        while read -r bp bc bd; do
            [ "$bp" = "$pair" ] && { want_c=$bc; want_d=$bd; }
        done <<< "$(printf '%s\n' "$KD_BASE_DIFFTEST" | grep -E '^[A-Za-z]')"
        if [ "$cnt" != "$want_c" ] || [ "$dims" != "$want_d" ]; then
            printf '   %-38s count %s (want %s)  dims %s (want %s)\n' \
                   "$pair" "$cnt" "$want_c" "$dims" "$want_d"
            bad=$((bad + 1))
        fi
    done < <(awk '
        /^Mcd.*Intersect:/ { p = $1; sub(/:$/, "", p) }
        /DIFFERENT contact count/ { c = $NF }
        /DIFFERENT contact dims/  { print p, c, $NF }' "$log")
    [ "$bad" = 0 ]
}

check_substitute() { # <scene> <log>
    local scene="$1" log="$2"
    local clean crashed bitid diverg failed
    clean=$(grep -oP 'substituted and ran cleanly\s*:\s*\K\d+' "$log")
    crashed=$(grep -oP 'crashed / NaN / short\s*:\s*\K\d+' "$log")
    bitid=$(grep -c 'trajectory bit-identical' "$log")
    diverg=$(grep -c '^  \[ diverg\]' "$log")
    failed=$(grep -oP '^  \[ FAIL \] \K[A-Za-z_0-9]+' "$log" | sort | tr '\n' ' ')
    failed="${failed% }"; [ -z "$failed" ] && failed="-"
    local want
    want=$(printf '%s\n' "$KD_BASE_SUBSTITUTE" | awk -v s="$scene" '$1==s {print $2, $3, $4, $5, $6}')
    if [ -z "$want" ]; then
        echo "   no baseline for $scene in test/baselines.sh"; return 1
    fi
    local got="$clean $crashed $bitid $diverg $failed"
    if [ "$got" != "$want" ]; then
        echo "   got  : $got"
        echo "   want : $want   (clean crashed bit-identical diverged FAIL)"
        return 1
    fi
    echo "   $bitid bit-identical, $diverg diverged, $crashed crashed — as recorded"
    return 0
}

if [ "${KD_QUICK:-}" = 1 ]; then
    skipped substitute "KD_QUICK"
    skipped difftest "KD_QUICK"
else
    # ⚠ READ THE PER-OBJECT LINES, NOT THE SUMMARY. "substituted and ran cleanly"
    # counts crashes, not bit-identity — `pass` increments in all three branches
    # including [ diverg]. This project quoted it as bit-identity once and a
    # bisect had to catch it. check_substitute reads both, separately.
    for scene in scene_chain scene_boxes_on_plane scene_ragdoll; do
        log="/tmp/kd_sub_$scene.log"
        "$KD_T_STD/substitute_test.sh" "$KD_BUILD" "$KD_MT_LIB" \
            "$KD_T_STD/$scene.c" > "$log" 2>&1
        run "substitute:$scene" "swap each object into a scene and diff the trajectory" -- \
            check_substitute "$scene" "$log"
    done

    dtlog=/tmp/kd_difftest.log
    "$KD_T_STD/difftest_pair.sh" "$KD_BUILD" "$METOOLKIT_DIR" > "$dtlog" 2>&1
    run difftest "drive each interaction over 200k randomised transforms" -- \
        check_difftest "$dtlog"

    # The default 4x2 patch reads 0 here; KD_CORNER=1 is what found the live
    # defect, at iteration 23624.
    run difftest-corner "the cylinder/TriangleList repro the default mesh cannot reach" -- \
        env KD_CORNER=1 "$KD_T_STD/difftest_pair.sh" "$KD_BUILD" "$METOOLKIT_DIR" \
            McdCylinderTriangleListIntersect 30000
fi

run hull "the convex-hull replacement against the contract the shipped one meets" -- \
    "$KD_T_STD/hull_ab.sh"

# --- summary ----------------------------------------------------------------
printf '\n\033[1m== run-standalone ==\033[0m\n'
printf '  %d passed, %d failed, %d skipped\n' "$pass" "$fail" "$skip"
if [ "$fail" != 0 ]; then
    printf '  failed:%s\n' "$FAILED"
    printf '\n  Before treating a failure as a regression, check it is not one of the\n'
    printf '  known-unstable verdicts in CLAUDE.md — scene_ragdoll under ASan is a\n'
    printf '  coin flip by measurement, not a defect.\n'
    exit 1
fi
echo "  -> PASS"
