#!/usr/bin/env bash
# GATE: doc-sync  [hard · pre-commit]
# Purpose:        the recovery's value is in its written record — proven.txt, the handovers,
#                 the per-tool run commands. A tool or harness that changes without its doc is
#                 how a documented command stops working, which is the failure this repository
#                 was reorganised to end.
# False positives: a pure rename or a formatting pass. Use the bypass and say so.
# Bypass:         git commit --no-verify  (please note why in the commit body).
# Removal:        when doc drift stops being a risk (i.e. ~never during active development).
set -uo pipefail
staged="$(git diff --cached --name-only --diff-filter=ACMR)"
[ -z "$staged" ] && exit 0

code="$(printf '%s\n' "$staged" | grep -E '^decomp/(tools|test|lib|src)/' || true)"
[ -z "$code" ] && exit 0

doc="$(printf '%s\n' "$staged" | grep -E '^(decomp/(README\.md|HANDOVER\.md|HANDOVER-WEB\.md|STATUS-EXEC\.md|proven\.txt|docs/|tools/README\.md|test/README\.md)|CLAUDE\.md)' || true)"
if [ -z "$doc" ]; then
  echo "doc-sync[BLOCK]: decomp/ code changed but no doc is staged with it."
  printf '%s\n' "$code" | sed 's/^/    /'
  echo "  → update the tool's block in decomp/tools/README.md or decomp/test/README.md,"
  echo "    the reasoning in decomp/HANDOVER.md, or the record in decomp/docs/STATE.md."
  exit 1
fi
exit 0
