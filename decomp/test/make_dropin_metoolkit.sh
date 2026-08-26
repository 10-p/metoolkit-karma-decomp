#!/usr/bin/env bash
# make_dropin_metoolkit.sh — a metoolkit tree with NO SHIPPED MEMBER IN IT.
#
#   make_dropin_metoolkit.sh <recovered-obj-dir> <metoolkit-root> <out-root>
#
# This is HANDOVER.md §12 CHECK 2, and it is the deliverable. Everything else in
# this project measures pieces; this is the whole thing:
#
#   1. `make_hull_lib.sh` replaces libMcdConvexCreateHull.a — qhull, 1.4 MB —
#      with src/McdConvexCreateHull/kd_convexhull.c. That one is a REPLACEMENT,
#      not a recovery, so it is a whole-archive swap.
#   2. `make_substituted_metoolkit.sh` puts every recovered object into the
#      archive IN PLACE OF the shipped member.
#   3. Then every member that is still MathEngine's is `ar d`'d out.
#
# After step 3 the archives contain our code and nothing else, so a link that
# succeeds is the proof — there is no shipped object left for it to fall back
# on. `tools/dropin_gap.py` PREDICTS this result by walking the symbol closure;
# this is the same claim measured instead of computed, and the two have to
# agree. They did on 2026-08-27: gap 0, and the engine linked.
#
# THE DELETIONS ARE LISTED, not silent. Most are objects §3b retires as
# unreachable and a few are the tail this project never recovered; if the link
# then fails, the undefined symbols name exactly which of them was needed after
# all, which is more useful than refusing to delete them.
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
REC="${1:?recovered object dir}"
SRC="${2:?metoolkit root}"
OUT="${3:?output metoolkit root}"
SUBDIR="lib.rel/linux_single_gcc3.2"

TMPHULL=$(mktemp -d)/hull
"$HERE/test/make_hull_lib.sh" "$SRC" "$TMPHULL" | sed 's/^/  /'
"$HERE/test/make_substituted_metoolkit.sh" "$REC" "$TMPHULL" "$OUT" | sed 's/^/  /'

# `make_substituted_metoolkit.sh` requeues each recovered object under a
# `.subst_` member name, so the surviving SHIPPED members are the ones whose
# name has neither that prefix nor a recovered counterpart.
deleted=0; kept=0
: > /tmp/kd_dropin_deleted.txt
for a in "$OUT/$SUBDIR"/*.a; do
    [ "$(basename "$a")" = "libMcdConvexCreateHull.a" ] && continue   # already ours
    for m in $(ar t "$a"); do
        case "$m" in
            .subst_*) kept=$((kept+1)); continue ;;
        esac
        ar d "$a" "$m"
        echo "$(basename "$a") $m" >> /tmp/kd_dropin_deleted.txt
        deleted=$((deleted+1))
    done
done
echo "  kept $kept recovered member(s); DELETED $deleted shipped member(s)"
echo "  deletions listed in /tmp/kd_dropin_deleted.txt"
echo "  configure the engine with -DMETOOLKIT_DIR=$OUT"
