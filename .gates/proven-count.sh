#!/usr/bin/env bash
# GATE: proven-count  [hard · pre-commit]
# Purpose:        proven.txt is the release ledger, and its FORMAT is load-bearing — the first
#                 word of any non-comment line RELEASES an object from quarantine. Prose that
#                 forgets its `#` silently releases whatever word it starts with, and nothing
#                 else in the project would notice.
# False positives: a real release changes the count on purpose. Update EXPECT below in the same
#                 commit, which is the point: the number moves deliberately or not at all.
# Bypass:         git commit --no-verify  (say why in the body).
# Removal:        when proven.txt stops driving quarantine.
set -uo pipefail
root="$(git rev-parse --show-toplevel)"
file="$root/decomp/proven.txt"
[ -f "$file" ] || exit 0

EXPECT=30
have="$(grep -vcE '^\s*#|^\s*$' "$file")"

if [ "$have" != "$EXPECT" ]; then
  echo "proven-count[BLOCK]: proven.txt has $have release line(s), expected $EXPECT."
  echo "  Non-comment lines RELEASE an object. If this is a real release, update EXPECT in"
  echo "  .gates/proven-count.sh in the same commit. If it is prose, comment it:"
  grep -vnE '^\s*#|^\s*$' "$file" | head -20 | sed 's/^/    /'
  exit 1
fi
exit 0
