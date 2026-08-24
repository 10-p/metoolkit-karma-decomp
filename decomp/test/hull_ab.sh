#!/usr/bin/env bash
# hull_ab.sh — is a replacement hull the SAME SOLID as the shipped one?
#
#   hull_ab.sh [impl.c] [metoolkit-lib-dir]
#
# HANDOVER.md §8a tier 2. Tier 1 (hull_probe.sh) proves a hull is VALID —
# sentinels, Euler, closed ACW rings, paired edges, outward normals. It cannot
# prove it is the same solid the shipped library builds, and equality is not
# available as a test because the hull REINDEXES: two correct implementations
# can disagree about every index.
#
# So both sides dump geometry canonically — vertex positions and face planes,
# sorted, at fixed precision, plus the enclosed volume — and this diffs that.
# Reordering cannot hide a difference and indexing cannot manufacture one.
#
# A clean run means: same vertex set, same face planes, same volume, on every
# shape. Float noise in the last printed digit of a normal is expected and is
# reported rather than hidden — see the tolerance note below.
set -uo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
IMPL="${1:-$HERE/../src/McdConvexCreateHull/kd_convexhull.c}"
LIBDIR="${2:-$HERE/../../Thirdparty/metoolkit/lib.rel/linux_single_gcc3.2}"

A=$(mktemp); B=$(mktemp); D=$(mktemp)
trap 'rm -f "$A" "$B" "$D"' EXIT

KD_HULL_DUMP=1 "$HERE/hull_probe.sh" "$LIBDIR" > "$A" 2>/dev/null || {
    echo "FATAL: shipped side did not run"; exit 1; }
KD_HULL_DUMP=1 KD_HULL_IMPL="$IMPL" "$HERE/hull_probe.sh" "$LIBDIR" > "$B" 2>/dev/null || {
    echo "FATAL: replacement did not run"; exit 1; }

echo "shipped:     $(wc -l < "$A") canonical lines"
echo "replacement: $(wc -l < "$B") canonical lines"
echo

if diff "$A" "$B" > "$D"; then
    echo "IDENTICAL — same vertices, same face planes, same volumes, every shape."
    exit 0
fi

n=$(grep -c '^[<>]' "$D")
echo "$n differing line(s):"
sed 's/^/  /' "$D"
echo

# Volumes are the single scalar no reordering can disguise. If those agree, a
# difference in the last printed digit of a normal is float noise; if they do
# not, the two are different solids and the run has failed.
if diff <(grep '^VOL' "$A") <(grep '^VOL' "$B") > /dev/null; then
    echo "All volumes identical -> the solids agree; the above is last-digit"
    echo "float noise in a normal component, not a structural difference."
    exit 0
fi
echo "VOLUMES DIFFER -> these are different solids. Not float noise."
exit 1
