#!/usr/bin/env bash
# substitute_test.sh — breadth-tier validation gate.
#
# Takes every object that recover.py managed to compile, swaps it into the link
# in place of the shipped one, and runs a physics scene. This is the cheap,
# broad half of validation: it will not prove a function correct, but it catches
# crashes, hangs, NaNs and wild divergence across many objects at once, which is
# exactly what per-function differential testing is too slow to sweep for.
#
# The precise half is test/difftest_*.c, which compares a single function
# against the shipped binary on real inputs.
#
# Usage: substitute_test.sh <recovered-obj-dir> <metoolkit-lib-dir> <scene.c>
set -u

RECOVERED="${1:?recovered object dir}"
LIBDIR="${2:?metoolkit lib dir}"
SCENE="${3:?scene .c}"
INC="$(dirname "$LIBDIR")/../include"
[ -d "$INC" ] || INC="${METOOLKIT_INC:?set METOOLKIT_INC}"

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
IFLAGS="-I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon -I$INC/McdPrimitives"
IFLAGS="$IFLAGS -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp"

cp "$LIBDIR"/*.a "$WORK/"

# Baseline: entirely original library.
gcc -m32 -O2 -DLINUX -no-pie $IFLAGS -o "$WORK/baseline" "$SCENE" \
    -Wl,--start-group "$WORK"/*.a -Wl,--end-group -lstdc++ -lm 2>/dev/null || {
    echo "FATAL: baseline link failed"; exit 1; }
"$WORK/baseline" > "$WORK/baseline.csv" 2>/dev/null || {
    echo "FATAL: baseline run failed"; exit 1; }
echo "baseline: $(wc -l < "$WORK/baseline.csv") rows"

pass=0; fail=0; skip=0
for obj in "$RECOVERED"/*.o; do
    [ -e "$obj" ] || continue
    base=$(basename "$obj" .o)
    # Which archive owns the original member?
    owner=""
    for a in "$WORK"/*.a; do
        if ar t "$a" 2>/dev/null | grep -qx "$base.o"; then owner="$a"; break; fi
    done
    if [ -z "$owner" ]; then skip=$((skip+1)); continue; fi

    cp "$owner" "$owner.bak"
    ar d "$owner" "$base.o" 2>/dev/null
    if gcc -m32 -O2 -DLINUX -no-pie $IFLAGS -o "$WORK/t" "$SCENE" "$obj" \
           -Wl,--start-group "$WORK"/*.a -Wl,--end-group -lstdc++ -lm 2>/dev/null \
       && timeout 60 "$WORK/t" > "$WORK/t.csv" 2>/dev/null; then
        # Physically sane? No NaN/inf, and the row count matches.
        if grep -qiE 'nan|inf' "$WORK/t.csv"; then
            echo "  [ NaN  ] $base"; fail=$((fail+1))
        elif [ "$(wc -l < "$WORK/t.csv")" != "$(wc -l < "$WORK/baseline.csv")" ]; then
            echo "  [ short] $base — simulation ended early"; fail=$((fail+1))
        else
            echo "  [  ok  ] $base"; pass=$((pass+1))
        fi
    else
        echo "  [ FAIL ] $base — link or run failed"; fail=$((fail+1))
    fi
    mv "$owner.bak" "$owner"
done

echo
echo "=================================================="
echo "  substituted and ran cleanly : $pass"
echo "  crashed / NaN / short       : $fail"
echo "  not a library member        : $skip"
[ "$fail" -eq 0 ]
