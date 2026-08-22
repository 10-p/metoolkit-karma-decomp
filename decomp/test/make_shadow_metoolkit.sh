#!/usr/bin/env bash
# make_shadow_metoolkit.sh — build a metoolkit tree that shadow-tests itself.
#
# Produces a drop-in replacement for Thirdparty/metoolkit in which the shadowed
# functions are renamed:
#
#   original archive member   McdBoxBoxIntersect  ->  orig_McdBoxBoxIntersect
#   recovered object          McdBoxBoxIntersect  ->  rec_McdBoxBoxIntersect
#   kd_shadow.c              defines McdBoxBoxIntersect, calls both, compares
#
# Every existing caller — all of UT2004 — therefore calls the shadow, which
# returns the ORIGINAL's result. Behaviour is unchanged; the recovered code is
# exercised on real gameplay inputs for free.
#
# Point the engine at the result with:
#   cmake -DMETOOLKIT_DIR=<out> ...
set -euo pipefail

SRC="${1:?metoolkit source root (Thirdparty/metoolkit)}"
RECOVERED="${2:?dir of recovered .o files}"
OUT="${3:?output metoolkit root}"
SUBDIR="lib.rel/linux_single_gcc3.2"
HERE="$(cd "$(dirname "$0")" && pwd)"

# ONE interposed function. Karma installs its intersection routines as function
# POINTERS in an interaction table, so renaming those routines just rewrites the
# table's own reference and the replacement is never reached. Hooking the
# registration point instead covers the entire collision matrix with one rename.
FNS=(McdFrameworkSetInteractions)

# Recovered functions to compare against, parsed out of kd_shadow.c so the two
# can never drift. These are NOT renamed in the shipped archives; the harness
# identifies them by address at registration time.
mapfile -t RECFNS < <(grep -oP '^\s+X\(\K\w+' "$HERE/kd_shadow.c")
echo "interposing: ${FNS[*]}"
echo "comparing ${#RECFNS[@]} recovered function(s): ${RECFNS[*]}"

rm -rf "$OUT"; mkdir -p "$OUT/$SUBDIR"
ln -s "$(cd "$SRC" && pwd)/include" "$OUT/include"
cp "$SRC/$SUBDIR"/*.a "$OUT/$SUBDIR/"

# 1. Rename the shipped definitions to orig_*.
REDEF_ORIG=$(mktemp); REDEF_REC=$(mktemp)
for f in "${FNS[@]}"; do echo "$f orig_$f" >> "$REDEF_ORIG"; done
for a in "$OUT/$SUBDIR"/*.a; do
    tmp=$(mktemp -d); (cd "$tmp" && ar x "$a")
    changed=0
    for o in "$tmp"/*.o; do
        for f in "${FNS[@]}"; do
            if nm --defined-only "$o" 2>/dev/null | grep -qE "^[0-9a-f]+ T $f\$"; then
                objcopy --redefine-syms="$REDEF_ORIG" "$o"; changed=1; break
            fi
        done
    done
    [ "$changed" = 1 ] && (cd "$tmp" && ar rcs "$a" *.o)
    rm -rf "$tmp"
done

# 2. Recovered objects, with the same names renamed to rec_*.
STAGE=$(mktemp -d)
n=0
for o in "$RECOVERED"/*.o; do
    [ -e "$o" ] || continue
    for f in "${RECFNS[@]}"; do
        if nm --defined-only "$o" 2>/dev/null | grep -qE "^[0-9a-f]+ T $f\$"; then
            dst="$STAGE/rec_$(basename "$o")"
            cp "$o" "$dst"
            # Prefix EVERY symbol the object defines, not just the one under
            # test. Otherwise its siblings (McdBoxBoxSafeTime,
            # McdBoxBoxRegisterInteraction, ...) collide with the shipped
            # archive's definitions and the linker silently picks one of the
            # two — which would mean testing a build that is half recovered.
            # Undefined symbols are deliberately left alone, so the object's
            # imports still resolve to the originals.
            nm --defined-only "$dst" | awk '$2 ~ /^[TDBRWV]$/ {print $3, "rec_" $3}' > "$REDEF_REC"
            [ -s "$REDEF_REC" ] && objcopy --redefine-syms="$REDEF_REC" "$dst"
            n=$((n+1)); break
        fi
    done
done
echo "staged $n recovered object(s)"

# 3. The shadow itself.
INC="$SRC/include"
gcc -m32 -O2 -fno-pic -std=gnu99 -w -DLINUX -c -o "$STAGE/kd_shadow.o" "$HERE/kd_shadow.c" \
    -I"$INC" -I"$INC/McdCommon" -I"$INC/McdPrimitives" -I"$INC/McdFrame" \
    -I"$INC/MeGlobals" -I"$INC/MdtBcl" -I"$INC/MdtKea" -I"$INC/Mst" -I"$INC/MeApp"

# One archive, linked inside the engine's existing --start-group.
ar rcs "$OUT/$SUBDIR/libKarmaShadow.a" "$STAGE"/*.o
rm -rf "$STAGE" "$REDEF_ORIG" "$REDEF_REC"

echo "shadow metoolkit ready: $OUT"
echo "  configure the engine with -DMETOOLKIT_DIR=$OUT"
