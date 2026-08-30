#!/usr/bin/env bash
# make_substituted_metoolkit.sh — a metoolkit tree with recovered objects
# REPLACING the shipped ones, so the engine actually runs on the recovered code.
#
# This is a different question from the shadow harness, and a harder one. The
# shadow runs both implementations and lets the engine consume the ORIGINAL's
# output, so it proves the two agree on the inputs the game generates. It cannot
# prove the recovered code is usable, because an error that would compound frame
# to frame never gets the chance to: every frame starts from the original's
# answer again.
#
# Substituting removes that safety net. A contact that flaps at the tolerance
# now feeds the next frame's integration. That is the only way to find out
# whether 0.0013% threshold flapping matters, and it is definition-of-done
# item 7 in miniature.
#
#   make_substituted_metoolkit.sh <recovered-obj-dir> <metoolkit-root> <out> [obj...]
#
# With no object names, every recovered object is swapped in. With names, only
# those — which is how you attribute a problem to one object.
set -euo pipefail
. "$(cd "$(dirname "$0")" && pwd)/../../lib/kd-paths.sh"

RECOVERED="${1:?recovered object dir}"
SRC="${2:?metoolkit root}"
OUT="${3:?output metoolkit root}"
shift 3
WANT=("$@")
SUBDIR="$KD_MT_LIB_SUBDIR"

rm -rf "$OUT"; mkdir -p "$OUT/$SUBDIR"
ln -s "$(cd "$SRC" && pwd)/include" "$OUT/include"
cp "$SRC/$SUBDIR"/*.a "$OUT/$SUBDIR/"

n=0; skipped=0
for o in "$RECOVERED"/*.o; do
    [ -e "$o" ] || continue
    base=$(basename "$o" .o)
    if [ ${#WANT[@]} -gt 0 ]; then
        keep=0
        for w in "${WANT[@]}"; do [ "$w" = "$base" ] && keep=1; done
        [ "$keep" = 1 ] || continue
    fi
    owner=""
    for a in "$OUT/$SUBDIR"/*.a; do
        if ar t "$a" 2>/dev/null | grep -qx "$base.o"; then owner="$a"; break; fi
    done
    if [ -z "$owner" ]; then skipped=$((skipped+1)); continue; fi
    # Replace the member outright. `ar r` matches on member name, so the
    # recovered object has to keep its own filename — which it does.
    ar d "$owner" "$base.o"
    cp "$o" "/tmp/.subst_$base.o"
    (cd /tmp && ar q "$owner" ".subst_$base.o" && ar m "$owner" ".subst_$base.o") 2>/dev/null \
        || ar q "$owner" "/tmp/.subst_$base.o"
    rm -f "/tmp/.subst_$base.o"
    n=$((n+1))
done
echo "substituted $n object(s), $skipped not library members"
echo "  configure the engine with -DMETOOLKIT_DIR=$OUT"
