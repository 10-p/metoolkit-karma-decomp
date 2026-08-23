#!/usr/bin/env bash
# gate_sensitivity.sh — is the substitute gate actually sensitive to this object?
#
#   gate_sensitivity.sh <recovered-c-dir> <metoolkit-lib-dir> <scene.c> [obj...]
#
# substitute_test.sh prints `trajectory bit-identical` for all 93 recovered
# objects on scene_chain.c. That line has two completely different meanings and
# the gate cannot tell them apart:
#
#   (a) the recovery reproduces the shipped code exactly, or
#   (b) nothing this object computes reaches the numbers being compared.
#
# scene_census.sh separates "ran" from "never ran". This separates "ran and
# mattered" from "ran and was irrelevant", which is the other half.
#
# HOW. Rebuild the object with -ffloat-store -mfpmath=sse -msse2 and nothing
# else changed, so intermediates are rounded to storage precision instead of
# being kept in an x87 80-bit register. That moves most float computation in the
# translation unit by about an ULP, and the scenes amplify an ULP into metres.
#
# READ THE RESULT AS A ONE-SIDED TEST. That is the whole discipline here:
#
#   delta > 0   PROVEN sensitive. A bit-identical verdict for this object on
#               this scene is real evidence, and the number is roughly the
#               smallest error the scene would have caught.
#   delta = 0   PROVES NOTHING. Not "the gate is blind" — the probe simply did
#               not perturb anything, and there are three reasons:
#                 * the object does no float arithmetic (fp=0);
#                 * it never ran (scene_census.sh says which);
#                 * its arithmetic is a single rounding step, so rounding it
#                   differently is a no-op.
#
# The third one is not hypothetical and is why this file does not say "blind".
# keaCalcAcceleration_vanilla reads zero under every rounding probe tried, and
# it is emphatically NOT irrelevant: it computes accel[i] = invmass * force[i]
# and a torque dot product, and forcing those outputs to 7777 moves scene_chain
# by 8.8e5 m. One multiply rounds identically at any precision, and in these
# scenes most of the torque terms are zero. So the arithmetic is exact, the
# output matters, and no rounding probe can see it.
#
# To settle a zero, mutate the object's OUTPUTS to a wild value by hand and
# re-run. That is not generic enough to automate — it needs to know what the
# outputs are — which is exactly why this tool reports "not demonstrated"
# rather than a verdict.
#
# The probe deliberately does not touch the source: a hand-placed perturbation
# tests the line it was placed on, a compiler flag tests the whole object. It is
# also checked rather than assumed — an object whose code is byte-identical with
# and without the flags is reported as `not applied`, because a probe that did
# nothing produces exactly the same zero as an object that does not matter.
set -uo pipefail

CDIR="${1:?dir of recovered .c files}"
LIBDIR="${2:?metoolkit lib dir}"
SCENE="${3:?scene .c}"
shift 3
WANT=("$@")

INC="$(cd "$(dirname "$LIBDIR")/../include" && pwd)"
HERE="$(cd "$(dirname "$0")" && pwd)"
IFLAGS="-I$HERE/../include -I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon"
IFLAGS="$IFLAGS -I$INC/McdPrimitives -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp"
CFLAGS="-m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion"
CFLAGS="$CFLAGS -Wno-incompatible-pointer-types -DLINUX"

WORK=$(mktemp -d); trap 'rm -rf "$WORK"' EXIT
cp "$LIBDIR"/*.a "$WORK/"

gcc -m32 -O2 -DLINUX -no-pie $IFLAGS -o "$WORK/baseline" "$SCENE" \
    -Wl,--start-group "$WORK"/*.a -Wl,--end-group -lstdc++ -lm 2>/dev/null || {
    echo "FATAL: baseline link failed"; exit 1; }
"$WORK/baseline" > "$WORK/baseline.csv" || { echo "FATAL: baseline run failed"; exit 1; }

compare() {   # $1 = csv to compare against baseline
    python3 - "$WORK/baseline.csv" "$1" <<'PY'
import csv, sys
def load(p):
    rows = list(csv.reader(open(p)))
    return [[float(x) for x in r] for r in rows[1:]]
a, b = load(sys.argv[1]), load(sys.argv[2])
if len(a) != len(b):
    print('SHORT'); raise SystemExit
w = 0.0
for r1, r2 in zip(a, b):
    for x, y in zip(r1[1:], r2[1:]):
        w = max(w, abs(x - y))
print('%.3e' % w)
PY
}

printf '%-38s %6s %13s %13s   %s\n' 'object' 'fp' 'as recovered' 'rounded' 'verdict'
printf -- '%.0s-' {1..104}; echo
blind=0; seen=0; inapplicable=0
for c in "$CDIR"/*.c; do
    [ -e "$c" ] || continue
    base=$(basename "$c" .c)
    if [ ${#WANT[@]} -gt 0 ]; then
        keep=0; for w in "${WANT[@]}"; do [ "$w" = "$base" ] && keep=1; done
        [ "$keep" = 1 ] || continue
    fi
    owner=""
    for a in "$WORK"/*.a; do
        if ar t "$a" 2>/dev/null | grep -qx "$base.o"; then owner="$a"; break; fi
    done
    [ -n "$owner" ] || continue

    gcc $CFLAGS $IFLAGS -c -o "$WORK/plain.o" "$c" 2>/dev/null || continue
    gcc $CFLAGS -ffloat-store -mfpmath=sse -msse2 $IFLAGS -c -o "$WORK/fs.o" "$c" \
        2>/dev/null || continue
    # Does this object do float arithmetic at all, and did the flag actually
    # change the code? Both have to be true for a zero delta to mean anything.
    fp=$(objdump -d "$WORK/plain.o" 2>/dev/null | grep -cE \
         '^[[:space:]]+[0-9a-f]+:.*[[:space:]](f(add|sub|mul|div|ld|st|stp|abs|sqrt|chs|com|ucom|xch)|(add|sub|mul|div)ss)')
    applied=yes
    [ "$(objdump -d "$WORK/plain.o" | md5sum)" = "$(objdump -d "$WORK/fs.o" | md5sum)" ] && applied=no

    cp "$owner" "$owner.bak"; ar d "$owner" "$base.o" 2>/dev/null
    d_plain='link'; d_fs='link'
    if gcc -m32 -O2 -DLINUX -no-pie $IFLAGS -o "$WORK/p" "$SCENE" "$WORK/plain.o" \
           -Wl,--start-group "$WORK"/*.a -Wl,--end-group -lstdc++ -lm 2>/dev/null \
       && timeout 120 "$WORK/p" > "$WORK/p.csv" 2>/dev/null; then
        d_plain=$(compare "$WORK/p.csv")
    fi
    if gcc -m32 -O2 -DLINUX -no-pie $IFLAGS -o "$WORK/f" "$SCENE" "$WORK/fs.o" \
           -Wl,--start-group "$WORK"/*.a -Wl,--end-group -lstdc++ -lm 2>/dev/null \
       && timeout 120 "$WORK/f" > "$WORK/f.csv" 2>/dev/null; then
        d_fs=$(compare "$WORK/f.csv")
    fi
    mv "$owner.bak" "$owner"

    seen=$((seen+1))
    verdict="PROVEN sensitive"
    if [ "$d_fs" = "link" ] || [ "$d_fs" = "SHORT" ]; then
        verdict="probe did not run"; blind=$((blind+1))
    elif [ "$applied" = no ]; then
        verdict="not demonstrated — flags changed no code"; inapplicable=$((inapplicable+1))
    elif [ "$d_fs" = "0.000e+00" ] && [ "$fp" = 0 ]; then
        verdict="not demonstrated — no float arithmetic"; inapplicable=$((inapplicable+1))
    elif [ "$d_fs" = "0.000e+00" ]; then
        verdict="not demonstrated — mutate its outputs to settle"; blind=$((blind+1))
    fi
    printf '%-38s %6s %13s %13s   %s\n' "$base" "$fp" "$d_plain" "$d_fs" "$verdict"
done
printf -- '%.0s-' {1..104}; echo
echo "  objects probed          : $seen"
echo "  sensitivity PROVEN      : $((seen - blind - inapplicable))"
echo "  not demonstrated        : $((blind + inapplicable))"
echo
echo "'as recovered' is what substitute_test.sh reports. 'rounded' is the same"
echo "recovered code with intermediates forced to storage precision — roughly"
echo "the smallest error this scene could have caught. A bit-identical recovery"
echo "is evidence ONLY on the rows where that column is non-zero; a zero there"
echo "is an absence of evidence, not evidence of absence (see the header)."
