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
