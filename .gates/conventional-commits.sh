#!/usr/bin/env bash
# GATE: conventional-commits  [warn · commit-msg]   ($1 = commit message file)
# Purpose:        nudge `type(scope): subject` so the log stays parseable.
# False positives: merge/revert/fixup commits (explicitly allowed below). Warn-only.
# Bypass:         n/a (advisory). Removal: if a different convention is adopted.
set -uo pipefail
msgfile="${1:-}"
[ -z "$msgfile" ] || [ ! -f "$msgfile" ] && exit 0
first="$(head -n1 "$msgfile")"
case "$first" in
  Merge\ *|Revert\ *|fixup!*|squash!*) exit 0 ;;
esac
if ! printf '%s' "$first" | grep -qE '^(feat|fix|docs|style|refactor|perf|test|build|ci|chore|revert)(\([^)]+\))?!?: .+'; then
  echo "conventional-commits[warn]: \"$first\""
  echo "  → prefer: type(scope): subject   (feat|fix|docs|chore|refactor|test|build|ci|perf|style|revert)"
fi
exit 0
