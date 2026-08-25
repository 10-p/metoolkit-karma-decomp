#!/usr/bin/env bash
# bisect_object.sh — which FUNCTION in a recovered object is the wrong one?
#
#   bisect_object.sh <recovered.o> <metoolkit-lib-dir> <scene.c> [symbol ...]
#
# substitute_test.sh works one OBJECT at a time, which is the right granularity
# for a gate and useless once an object is known to be wrong: keaLCPSolver has
# fifteen functions and twelve of them run on the ragdoll. This drives the same
# scene with exactly ONE recovered function in the link and the shipped code
# everywhere else, so a divergence names the function.
#
# HOW, because "link one function out of an object" is not a thing ld does. An
# archive member is all-or-nothing: ask for one symbol from it and the whole
# member comes in, and then the recovered definition and the shipped one are a
# duplicate-symbol error. Two objcopy passes get around it:
#
#   * every global the SHIPPED member defines is WEAKENED, so it no longer wins
#     against a strong definition of the same name;
#   * every global the RECOVERED object defines EXCEPT the one under test is
#     LOCALISED, so it satisfies nothing and the weak shipped one is taken.
#
# The result is one link with exactly one recovered function in it. Weak-vs-
# strong is the ordinary ELF rule, so nothing here depends on link order.
#
# WHAT TO READ. The first step a run differs on, not the largest difference:
# after contact a last-bit difference grows without bound, so every function on
# the path eventually reads metres. See substitute_test.sh and HANDOVER.md §4a.
#
# The control is built in. `--none` links no recovered function at all and must
# come back identical; if it does not, the harness is measuring itself and every
# other row is meaningless.
set -u

REC="${1:?recovered .o}"
LIBDIR="${2:?metoolkit lib dir}"
SCENE="${3:?scene .c}"
shift 3
MEMBER="$(basename "$REC")"
INC="$(dirname "$LIBDIR")/../include"
W=$(mktemp -d); trap 'rm -rf "$W"' EXIT
IF="-I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon -I$INC/McdPrimitives"
IF="$IF -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp"

cp "$LIBDIR"/*.a "$W/"
OWNER=""
for a in "$W"/*.a; do
    ar t "$a" 2>/dev/null | grep -qx "$MEMBER" && { OWNER="$a"; break; }
done
[ -n "$OWNER" ] || { echo "FATAL: no archive defines $MEMBER"; exit 1; }

# The shipped member, weakened.
( cd "$W" && ar x "$OWNER" "$MEMBER" && mv "$MEMBER" shipped.o )
ar d "$OWNER" "$MEMBER"
nm --defined-only -g --format=posix "$W/shipped.o" \
    | awk '$2 ~ /^[TDBRW]$/ {print "--weaken-symbol=" $1}' > "$W/weaken.args"
objcopy $(cat "$W/weaken.args") "$W/shipped.o" "$W/shipped_weak.o"

SYMS=$(nm --defined-only -g --format=posix "$REC" | awk '$2 ~ /^[TW]$/ {print $1}')
[ $# -gt 0 ] && SYMS="$*"

gcc -m32 -O2 -DLINUX -no-pie $IF -o "$W/base" "$SCENE" "$W/shipped_weak.o" \
    -Wl,--start-group "$W"/*.a -Wl,--end-group -lstdc++ -lm 2>/dev/null \
    || { echo "FATAL: baseline link failed"; exit 1; }
"$W/base" > "$W/base.csv" 2>/dev/null || { echo "FATAL: baseline run failed"; exit 1; }

report() {   # label, csv
    python3 - "$W/base.csv" "$2" "$1" <<'PY'
import csv, sys
def load(p):
    r = list(csv.reader(open(p)))
    return [[float(x) for x in row] for row in r[1:]]
try:
    a, b = load(sys.argv[1]), load(sys.argv[2])
except Exception:
    print('  %-44s ran short or unreadable' % sys.argv[3]); raise SystemExit
if len(a) != len(b):
    print('  %-44s SHORT (%d vs %d rows)' % (sys.argv[3], len(a), len(b))); raise SystemExit
first, worst = None, 0.0
for i, (ra, rb) in enumerate(zip(a, b)):
    d = max((abs(x - y) for x, y in zip(ra[1:], rb[1:])), default=0.0)
    if d and first is None:
        first = (i, d)
    worst = max(worst, d)
if first is None:
    print('  %-44s identical' % sys.argv[3])
else:
    print('  %-44s first@%d=%.3e  max=%.3e' % (sys.argv[3], first[0], first[1], worst))
PY
}

# The control: no recovered function at all.
gcc -m32 -O2 -DLINUX -no-pie $IF -o "$W/n" "$SCENE" "$W/shipped_weak.o" \
    -Wl,--start-group "$W"/*.a -Wl,--end-group -lstdc++ -lm 2>/dev/null
"$W/n" > "$W/n.csv" 2>/dev/null
report "--none-- (control, must be identical)" "$W/n.csv"

for sym in $SYMS; do
    keep="$sym"
    nm --defined-only -g --format=posix "$REC" \
        | awk -v k="$keep" '$2 ~ /^[TDBRW]$/ && $1 != k {print "--localize-symbol=" $1}' \
        > "$W/loc.args"
    objcopy $(cat "$W/loc.args") "$REC" "$W/one.o" 2>/dev/null || { echo "  $sym objcopy failed"; continue; }
    if ! gcc -m32 -O2 -DLINUX -no-pie $IF -o "$W/t" "$SCENE" "$W/one.o" "$W/shipped_weak.o" \
            -Wl,--start-group "$W"/*.a -Wl,--end-group -lstdc++ -lm 2>/dev/null; then
        echo "  $(c++filt "$sym" | sed 's/(.*//') link failed"; continue
    fi
    if ! timeout 120 "$W/t" > "$W/t.csv" 2>/dev/null; then
        echo "  $(c++filt "$sym" | sed 's/(.*//' | cut -c1-44) CRASHED"; continue
    fi
    report "$(c++filt "$sym" | sed 's/(.*//' | cut -c1-44)" "$W/t.csv"
done
