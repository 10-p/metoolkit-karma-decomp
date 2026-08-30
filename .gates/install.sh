#!/usr/bin/env bash
# Install this repository's git hooks. Idempotent; re-run after cloning.
#
#   ./.gates/install.sh
#
# The hooks live in .gates/hooks/ rather than .git/hooks/ so they are versioned
# with the checks they run. git does not do that for you, hence this script.
set -uo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
for h in pre-commit commit-msg; do
    src="$root/.gates/hooks/$h"
    dst="$root/.git/hooks/$h"
    [ -f "$src" ] || continue
    chmod +x "$src"
    ln -sf "../../.gates/hooks/$h" "$dst"
    echo "  installed $h"
done
chmod +x "$root"/.gates/*.sh
echo "done — .gates/README.md says what each gate is for and how to bypass it."
