#!/usr/bin/env bash
# run-ut2004.sh — the gates that need the GAME, not just this repository.
#
#   UT2004_ENGINE_DIR=... UT2004_BUILD_DIR=... UT2004_RUN_DIR=... \
#     ./decomp/test/run-ut2004.sh
#
# ---------------------------------------------------------------------------
# WHY THIS TIER EXISTS AT ALL, when run-standalone.sh already passes.
#
# Every gate in the standalone tier is a per-object comparison or an offline
# scene, and there is a whole class of defect none of them can see. The clearest
# case, and it is recorded in proven.txt as MCD-GEOM-FLOAT-FIELDS:
# `difftest_pair.sh` substitutes the INTERACTION object and links the GEOMETRY
# objects from the shipped library, so a broken `McdSphereGetRadius` is never in
# the loop — it reads byte-identical before and after the repair, and the object
# passes. What catches it is `ktrace`, which compares the trajectory the calls
# ADD UP TO over a real Onslaught match against a build linking MathEngine's
# originals.
#
# So: standalone green means the objects are right one at a time. This tier is
# where "the program is right" gets decided.
# ---------------------------------------------------------------------------
set -uo pipefail
. "$(cd "$(dirname "$0")" && pwd)/../lib/kd-paths.sh"

cat <<'EOT'
run-ut2004: the harnesses in this tier drive a real game, so this script does
not run them for you — each one wants a binary, a map, a gametype and a run
length that depend on what you are asking. It checks your environment, then
prints the block to run.

EOT

kd_require_metoolkit || exit 2
kd_require_ut2004 UT2004_ENGINE_DIR UT2004_BUILD_DIR UT2004_RUN_DIR || exit 2

BIN="${KD_BIN:-$UT2004_BUILD_DIR/Source/SDLLaunch/ut2004-pixo.bin}"
[ -x "$BIN" ] || { echo "  no engine binary at $BIN — set KD_BIN."; exit 2; }

echo "  engine   $UT2004_ENGINE_DIR"
echo "  build    $UT2004_BUILD_DIR"
echo "  run tree $UT2004_RUN_DIR"
echo "  binary   $BIN"
echo

cat <<EOT
# ---------------------------------------------------------------------------
# 1. ons_smoke — does this build survive five minutes of Onslaught?
#
# ONS is the gametype that spawns VEHICLES, and vehicles plus ragdolls are most
# of what Karma exists for; a DM map exercises almost none of it. This is the
# gate that caught McdGjkFaceQueueInit writing float bit patterns into
# _McdGjkFace.fi[] — a fault the browser suite could not see, because that suite
# is almost entirely DM.
#
# ⚠ The renderer is not the same on every target. Windows has no GLES; passing
# the default -GL4ESRENDERER there produces a crash that looks like a physics
# bug and is not one.
$KD_T_UT/ons_smoke.sh "$BIN" candidate 300

# ---------------------------------------------------------------------------
# 2. ktrace — does it BEHAVE? The gate the other twelve could not be.
#
# ★ -FIXEDFPS IS NOT OPTIONAL AND THE ENGINE ENFORCES IT. KTickLevelKarma
# derives its timestep AND its substep count from DeltaSeconds, so two runs at
# different frame rates integrate different equations. The hook appErrorf's
# rather than write a trace that would be diffed in good faith.
#
# Read 'first', not 'restZ', when the traces differ in length: restZ is the LAST
# frame of each file, so a 600-frame run against an 85-frame one compares
# different instants and reads like catastrophe.
KD_TOL_LIBM=1 $KD_T_UT/ktrace_gate.sh "$BIN" \\
    "$UT2004_ENGINE_DIR/build-native/Source/SDLLaunch/ut2004-pixo.bin"

# ★ ktrace_gate.sh is the WHOLE of step 2 now, and it RUNS rather than being remembered. It takes
# two runs of the candidate (non-determinism is the signature this project keeps meeting, and a
# single run cannot see it), one run of the 32-bit control, and asserts the trace is non-degenerate
# before it asserts anything about agreement. A `.exe` candidate goes through wine, so the Windows
# x64 target uses this same gate.
#
# ⚠ THE CONTROL IS A POINTER-WIDTH CONTROL — the same recovered sources built 32-bit, so the only
# variable is the width. The implementation A/B (recovered vs MathEngine) lives in the standalone
# tier, links the SDK archives this repository vendors, and needs no engine at all; the engine-side
# `BUILD_KARMA_REF` path that used to duplicate it was removed on 2026-09-02.

# to LOCALISE — both controls first, then the complement
$KD_T_UT/ktrace_subst.sh ctl-all  ALL        # must MATCH   — the mechanic works
$KD_T_UT/ktrace_subst.sh ctl-none NONE       # must MISMATCH — the defect is present

# ---------------------------------------------------------------------------
# 3. the drop-in gap — WHICH SHIPPED MEMBERS DOES THE ENGINE STILL NEED?
#
# This is the metric that measures the goal; object counts do not. It must read
# ZERO. ⚠ dropin_gap.py refuses any build tree but a Karma one — the metric is
# vacuous without it, and it reported ZERO once for a NO_KARMA build.
python3 $KD_ROOT/tools/dropin_gap.py \\
    "$UT2004_BUILD_DIR" $KD_BUILD $KD_OBJ_DIR

# ---------------------------------------------------------------------------
# 4. which recovered functions the REAL GAME actually enters
#
# "This object was substituted and the trajectory did not change" and "this
# object never executed" produce the identical line of output from every gate in
# the standalone tier. This is how you tell them apart.
#
# ⚠ -no-pie MUST GO ON THE EXECUTABLE. kd_instr.c records RUNTIME addresses;
# nm prints link-time offsets, and in a PIE those differ by the load bias, so
# every address resolves to nothing and the census reads "0 of 2025 functions
# entered" — indistinguishable from "the physics never ran".
cmake --preset native -B "$UT2004_ENGINE_DIR/build-native-census" \\
      -DMETOOLKIT_DECOMP_CENSUS=ON -DCMAKE_EXE_LINKER_FLAGS=-no-pie
python3 $KD_ROOT/tools/census_report.py /tmp/census.txt <binary>
EOT
