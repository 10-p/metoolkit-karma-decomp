#!/usr/bin/env bash
# GATE: product-in-sync  [hard · pre-commit, when metoolkit_decomp/src is staged]
# Purpose:        metoolkit_decomp/src is GENERATED. A hand edit there builds, passes every
#                 i386 gate, and is silently lost on the next regeneration — there is no other
#                 check in the project that would notice. This makes the checked-in tree prove
#                 it is what the generator would produce.
# False positives: none when a post-passed tree is on disk. Skips (loudly) when it is not,
#                 because regenerating one costs ten minutes and a pre-commit hook must not.
# Bypass:         git commit --no-verify  (say why in the body).
# Removal:        if metoolkit_decomp/src ever stops being generated.
set -uo pipefail
root="$(git rev-parse --show-toplevel)"

git diff --cached --name-only --diff-filter=ACMR \
  | grep -qE '^metoolkit_decomp/(src/|library-map\.txt)' || exit 0

LP64="${KD_LP64:-/tmp/kd_lp64}/allobj"
if [ ! -d "$LP64" ]; then
  echo "product-in-sync[SKIP]: no post-passed tree at $LP64, so the checked-in"
  echo "  metoolkit_decomp/src cannot be verified against its generator here."
  echo "  ⚠ You are committing generated sources unverified. Before pushing:"
  echo "      python3 decomp/tools/recover.py --out-dir /tmp/kd_out --build-dir /tmp/kd_build"
  echo "      ./decomp/test/standalone/lp64_pipeline.sh"
  echo "      python3 decomp/tools/split_libraries.py /tmp/kd_lp64/allobj --check"
  exit 0
fi

if ! python3 "$root/decomp/tools/split_libraries.py" "$LP64" --check; then
  echo "product-in-sync[BLOCK]: metoolkit_decomp/src is not what the generator produces."
  echo "  ⚠ SPLIT FROM /tmp/kd_lp64, NOT /tmp/kd_out. The raw recovery encodes i386 struct"
  echo "    LAYOUTS; using it builds, is byte-identical on every 32-bit target, passes every"
  echo "    i386 gate, and is wrong only on the 64-bit ABIs."
  exit 1
fi
exit 0
