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
#   aggregate degenerate 73 + end-inside 1: distances 1e-3 apart at 14-100 Karma
#     units of range (x87 against SSE in the cap's line-sphere cancellation; one
#     grazed child resolved differently, 0.31), and one normal where two children
#     are hit at the same point.
#   aggregate degenerate #2291: a 68-unit shot that truly enters the sphyl child
#     0.209 deep. The recovery reports the hit at 33.972; MathEngine's aggregate
#     returns a miss, while its own sphyl child, asked alone with fresh input
#     arrays, hits at 33.971. The candidate's answer is the same before and after
#     the float-sentinel repair; it was counted with the return-value cases below
#     until those were repaired.
#   convex random 2: the hull is kd_convexhull.c, not qhull; with MathEngine's
#     qhull archive linked into the same recovered build both cases agree.
#
# ★ ZERO aggregate hit/miss cases otherwise. IxAggregateLineSegment ended
#   `return distance != 3.4028235e+38;` — a DOUBLE literal, which promoted FLT_MAX
#   never equals — and reported a hit on every miss: 2419 random, 1219
#   axis-parallel and 952 degenerate cases here. ghidra_clean's
#   type_float_sentinel_comparisons spells it as the float it is
#   (proven.txt FLOAT-SENTINEL-COMPARISONS); a hit/miss case in any of those
#   categories is that defect back.
KD_BASE_LINESEG="
cases-per-category 2500
aggregate end-inside        1
aggregate degenerate       74
convex    random            2
"
