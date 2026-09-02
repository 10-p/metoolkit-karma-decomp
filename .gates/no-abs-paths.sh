#!/usr/bin/env bash
# GATE: no-abs-paths  [hard · pre-commit]
# Purpose:        this repository has to work from a bare clone on somebody else's machine.
#                 An absolute /home/... path in a tool, a harness, or a RUN COMMAND is the
#                 failure mode the 2.44 relocation existed to remove — 230 of them across 166
#                 files — and one new one is enough to make "standalone" false again.
#
# ⚠ CODE AND COMMANDS ARE CHECKED; PROSE IS NOT, AND THAT IS DELIBERATE.
#                 This project's value is largely in its written record, and that record
#                 legitimately QUOTES paths that no longer exist: STATE.md says
#                 "/home/ion/tools/karma-lab → lab/" because that is what happened, and
#                 KARMA-ON-WASM.md quotes `DW_AT_comp_dir: /home/icculus/projects/...` read
#                 out of MathEngine's own DWARF. Rewriting either would be falsifying history
#                 to please a linter. So in .md and .txt only FENCED CODE BLOCKS are checked —
#                 which is where a command someone will paste actually lives.
#
# False positives: an absolute path inside a fenced block that is genuinely an example. Write
#                 it as $HOME/... or $UT2004_*, or move it out of the fence.
# Bypass:         git commit --no-verify  (say why in the body).
# Removal:        never, while the standalone claim is made.
#
# ⚠⚠ `--all` EXISTS BECAUSE CI USED TO REIMPLEMENT THIS GATE AND GOT IT WRONG.
#                 The workflow could not use a staged-file gate, so it inlined "its whole-tree
#                 equivalent": a flat `grep -rInE '/home/[a-z]'` over the tree. That is not this
#                 gate. It checked PROSE, which this one deliberately does not, and it matched
#                 `/home/icculus` — MathEngine's own build directory, quoted out of their DWARF —
#                 which this one deliberately exempts. So CI failed on four documentation lines,
#                 every push, for weeks; and because it was the FIRST step, `Recover`, the LP64
#                 pipeline, the i386 acceptance test and the whole standalone tier were SKIPPED.
#                 A red CI that nobody could act on is a CI that runs nothing.
#                 ★ There is now ONE definition of this gate and CI calls it.
set -uo pipefail
root="$(git rev-parse --show-toplevel)"
if [ "${1:-}" = "--all" ]; then
    # Every TRACKED file — the form CI wants. `git ls-files` and not a find(1)
    # walk, so an untracked scratch file in the working tree cannot fail the run.
    staged="$(git -C "$root" ls-files)"
else
    staged="$(git diff --cached --name-only --diff-filter=ACMR)"
fi
[ -z "$staged" ] && exit 0

# ⚠ THE TARGET IS THIS MACHINE'S HOME, NOT EVERY /home PATH. A path under
# somebody ELSE'S home is not a portability hazard, and at least one is real
# evidence: KARMA-ON-WASM.md quotes `DW_AT_comp_dir: /home/icculus/projects/
# UT2-Code/karma/Mdt/src` straight out of MathEngine's DWARF, which is where the
# original build tree is recorded. Flagging that would be flagging the artefact.
homes="$(printf '%s\n%s\n' "${HOME:-/home/ion}" /home/ion | sort -u | tr '\n' '|')"
homes="${homes%|}"
pat="(^|[^A-Za-z0-9_\$])(${homes})/"

fail=0
while IFS= read -r f; do
  case "$f" in
    lab/*|metoolkit/*) continue ;;        # preserved artefacts, not ours to rewrite
    .gates/no-abs-paths.sh) continue ;;   # this file names the pattern
  esac
  [ -f "$root/$f" ] || continue
  file --mime-type -b "$root/$f" 2>/dev/null | grep -q '^text/' || continue

  case "$f" in
    *.md|*.txt)
      # Fenced blocks only. awk toggles on ``` and prints "line:text" inside.
      hits="$(awk '/^[ \t]*```/ {inb = !inb; next} inb {print FNR": "$0}' "$root/$f" \
              | grep -E "$pat" || true)"
      where="in a fenced code block" ;;
    *)
      hits="$(grep -nE "$pat" "$root/$f" || true)"
      where="" ;;
  esac

  if [ -n "$hits" ]; then
    echo "no-abs-paths[BLOCK]: $f $where"
    printf '%s\n' "$hits" | head -5 | sed 's/^/    /'
    echo "  → resolve it through decomp/lib/kd-paths.sh (shell) or decomp/tools/kd_paths.py"
    echo "    (Python), or name it \$UT2004_ENGINE_DIR / \$UT2004_ASSETS_DIR / \$HOME."
    fail=1
  fi
done <<< "$staged"
exit $fail
