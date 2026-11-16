#!/usr/bin/env bash
# baselines.sh — the numbers these gates are SUPPOSED to produce.
#
# ---------------------------------------------------------------------------
# WHY THIS FILE EXISTS, AND IT IS THE MOST IMPORTANT THING IN THE TIER.
#
# Three of the standalone gates EXIT NON-ZERO ON A CORRECT TREE, and always
# have. `proven.txt` says so in as many words:
#
#     "THE FULL DIFFTEST REPRODUCES THE DOCUMENTED BASELINE EXACTLY. 14 pairs,
#      200,000 each: IxBoxBox 1 count, IxSphereTriList 137 dims,
#      IxCylinderCylinder 1 count + 20 dims, the other eleven 0/0/0. The
#      aggregate exit is 1 — those three FAILs ARE the baseline, not a
#      regression, and a future session should not read them as one."
#
# and HANDOVER.md §4 says the same about substitute_test.sh: read the per-object
# lines, never the summary, and never the exit code.
#
# A runner that reads those exit codes reports three failures on a perfect tree.
# That is worse than no gate: it trains the reader to skip the red. So the tier
# asserts on the MEASURED BASELINE instead, and a change in either direction —
# a new divergence OR one that disappeared — is a finding.
#
# ⚠ A NUMBER HERE MOVING IS NOT AUTOMATICALLY A BUG, AND MUST NOT BE EDITED TO
# MAKE THE GATE GREEN. It means the recovery's behaviour changed. Find out why,
# record it in proven.txt, and then update this file in the same commit.
#
# Measured 2026-08-30 on the relocated repository, and A/B'd against
# engine-ut2004's karma/decompile tree object-for-object: identical.
# ---------------------------------------------------------------------------

# --- difftest_pair.sh, all 14 pairs, 200k transforms each ------------------
# "<pair> <different-contact-count> <different-contact-dims>"
# Anything not listed must be 0 0.
KD_BASE_DIFFTEST="
McdBoxBoxIntersect                 1  0
McdSphereTriangleListIntersect     0  137
McdCylinderCylinderIntersect       1  20
"

# The three above are the only pairs allowed to differ at all. Their causes are
# recorded: IxCylinderCylinder's residue is smaller than the SHIPPED library's
# own run-to-run self-consistency (KD_JITTER: 9,744 count_diff / 975 dims_diff
# against itself at 1e-7 m input noise, versus our 1 / 20), which is why
# CYLCYL-GRIND concluded that closing it "would demand more consistency than
# MathEngine has".

# --- substitute_test.sh, per scene -----------------------------------------
# "<scene> <ran-cleanly> <crashed> <bit-identical> <diverged> <FAIL objects>"
#
# MdtPartition fails to LINK on every scene and is a known, released object:
# every function the engine reaches is bit-identical and the one that diverges
# is imported by 0 engine objects (proven.txt). The four ragdoll divergences are
# its collision path — the harness itself annotates them "expected if on the
# collision path".
KD_BASE_SUBSTITUTE="
scene_chain             145  0  143  1  -
scene_boxes_on_plane    145  0  141  3  -
scene_ragdoll           144  1  140  4  MdtPartition
"
# ⚠ scene_chain and scene_boxes_on_plane link MdtPartition FINE and only the
# ragdoll does not, which is worth knowing before chasing it: the failure is
# scene-dependent, not a property of the object alone.

# --- lineseg_lp64.sh, the default case set ----------------------------------
# "<geometry> <category> <cases>": differences from the SHIPPED library that the
# i386 SSE control reproduces case for case — the recovered source at EVERY
# pointer width, wasm32 included, so not this gate's question. Anything not
# listed must be 0. Asserted only at the case count on the first line.
#
#   aggregate random / axis-parallel / degenerate: 2419 / 1219 / 953 of them are
#     hit/miss, and every one is IxAggregateLineSegment's return value:
#         return (uint)(overlap->distance != 3.4028235e+38);
#     a DOUBLE literal. MathEngine compares the float against the FLOAT constant
#     0x7f7fffff (`fcomps .rodata.cst4`); promoted to double, FLT_MAX is
#     3.4028234663852886e38 and never equals it, so the recovery reports a hit on
#     every miss, with the distance still FLT_MAX and no position or normal.
#   aggregate degenerate 73 + end-inside 1: distances 1e-3 apart at 14-100 Karma
#     units of range (x87 against SSE in the cap's line-sphere cancellation; one
#     grazed child resolved differently, 0.31), and one normal where two children
#     are hit at the same point.
#   convex random 2: the hull is kd_convexhull.c, not qhull; with MathEngine's
#     qhull archive linked into the same recovered build both cases agree.
KD_BASE_LINESEG="
cases-per-category 2500
aggregate random         2419
aggregate end-inside        1
aggregate axis-parallel  1219
aggregate degenerate     1026
convex    random            2
"
