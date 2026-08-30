#!/usr/bin/env bash
# scene_census.sh — which recovered functions does a substitute scene actually RUN?
#
#   scene_census.sh <recovered-c-dir> <metoolkit-lib-dir> <scene.c> [obj...]
#
# substitute_test.sh answers "did the trajectory change". It cannot tell
# "the recovery is exact" from "the code never executed" — both print
# `trajectory bit-identical`. That distinction is the whole question for the
# solver: all nine recovered libMdtKea objects pass scene_chain.c bit-identical,
# and scene_chain.c is a twelve-body joint chain with no collisions, so if those
# objects really were driving the solver a Ghidra decompile reproducing 900
# steps to the last bit would be extraordinary.
#
# This links the recovered sources with -finstrument-functions and counts every
# entry, file-local functions included. No sampling, no mutation of the code
# under test. It is KD_CENSUS (kd_shadow.c) for the offline scenes.
#
# With no object names every recovered object that owns an archive member is
# swapped in at once, which also answers a question substitute_test.sh does not
# ask: does the scene still run with ALL of them in the link together.
#
# KD_CENSUS_VALIDATED=<dir> restricts the sweep to objects present in that
# directory — point it at recover.py's build dir (/tmp/kd_build) to census the
# VALIDATED set only.
#
# Without it this script instruments every object whose .c compiles, which
# includes the ones recover.py's detectors are holding back, and one of those
# is MdtPartition: HANDOVER.md 4a records that adding it turns scene_chain from
# bit-identical into an immediate SIGSEGV. So the unrestricted sweep now dies
# with `0 rows of trajectory` before writing any census at all — a gate that
# cannot pass is not a gate, and the failure looks like a regression in
# whatever was last changed rather than like the quarantine working. Restricting
# to the validated set is what §4 means when it lists this as a gate.
set -uo pipefail
. "$(cd "$(dirname "$0")" && pwd)/../../lib/kd-paths.sh"

CDIR="${1:?dir of recovered .c files}"
LIBDIR="${2:?metoolkit lib dir}"
SCENE="${3:?scene .c}"
shift 3
WANT=("$@")

HERE="$(cd "$(dirname "$0")" && pwd)"
INC="$(cd "$(dirname "$LIBDIR")/../include" && pwd)"
IFLAGS="-I$KD_MD_INC -I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon"
IFLAGS="$IFLAGS -I$INC/McdPrimitives -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp"
# Same flags recover.py compiles with. -fno-strict-aliasing is REQUIRED: under
# -O2 without it GCC deletes argument stores (HANDOVER.md 10).
CFLAGS="-m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion"
CFLAGS="$CFLAGS -Wno-incompatible-pointer-types -DLINUX"

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
cp "$LIBDIR"/*.a "$WORK/"
BASELIB="$WORK/orig"; mkdir -p "$BASELIB"; cp "$LIBDIR"/*.a "$BASELIB/"

gcc -m32 -O2 -fno-pic -std=gnu99 -w -c -o "$WORK/kd_instr.o" "$HERE/kd_instr.c" || {
    echo "FATAL: kd_instr.c did not compile"; exit 1; }

objs=""; n=0; skipped=0; notrecovered=0; quarantined=0
VALIDATED="${KD_CENSUS_VALIDATED:-}"
for c in "$CDIR"/*.c; do
    [ -e "$c" ] || continue
    base=$(basename "$c" .c)
    if [ ${#WANT[@]} -gt 0 ]; then
        keep=0
        for w in "${WANT[@]}"; do [ "$w" = "$base" ] && keep=1; done
        [ "$keep" = 1 ] || continue
    fi
    # Held back by a recover.py detector: it compiles, and compiling is not the
    # bar. Counted separately so the number is visible rather than silent.
    if [ -n "$VALIDATED" ] && [ ! -e "$VALIDATED/$base.o" ]; then
        quarantined=$((quarantined+1)); continue
    fi
    # Only objects that actually replace a library member are meaningful here:
    # anything else is dead code in the link and would report zero calls for a
    # reason that has nothing to do with the scene.
    owner=""
    for a in "$WORK"/*.a; do
        if ar t "$a" 2>/dev/null | grep -qx "$base.o"; then owner="$a"; break; fi
    done
    [ -n "$owner" ] || { skipped=$((skipped+1)); continue; }

    # Distinguish "this object does not compile at all" — it is in recover.py's
    # FAIL pile and is not part of the recovered set — from "it compiles but not
    # under instrumentation", which would be a defect in this tool.
    if ! gcc $CFLAGS $IFLAGS -c -o /dev/null "$c" 2>/dev/null; then
        notrecovered=$((notrecovered+1)); continue
    fi
    if ! gcc $CFLAGS -finstrument-functions $IFLAGS -c -o "$WORK/$base.o" "$c" 2>/dev/null; then
        echo "  [ FAIL ] $base — compiles normally but NOT under -finstrument-functions"
        continue
    fi
    ar d "$owner" "$base.o" 2>/dev/null
    objs="$objs $WORK/$base.o"; n=$((n+1))
done
echo "instrumented $n object(s), $skipped not library members, $notrecovered do not compile${VALIDATED:+, $quarantined quarantined}"
[ "$n" -gt 0 ] || { echo "nothing to census"; exit 1; }

if ! gcc -m32 -O2 -DLINUX -no-pie $IFLAGS -o "$WORK/t" "$SCENE" $objs "$WORK/kd_instr.o" \
        -Wl,--start-group "$WORK"/*.a -Wl,--end-group -lstdc++ -lm 2>"$WORK/link.err"; then
    echo "FATAL: link failed"; sed 's/^/  /' "$WORK/link.err" | head -20; exit 1
fi

# The baseline has to be built from the ORIGINAL archives, before any member
# was deleted, so build it first and keep it.
gcc -m32 -O2 -DLINUX -no-pie $IFLAGS -o "$WORK/baseline" "$SCENE" \
    -Wl,--start-group "$BASELIB"/*.a -Wl,--end-group -lstdc++ -lm 2>/dev/null \
    && "$WORK/baseline" > "$WORK/baseline.csv" 2>/dev/null

OUT="${KD_INSTR_OUT:-/tmp/kd_instr.txt}"
rm -f "$OUT"
KD_INSTR_OUT="$OUT" timeout 300 "$WORK/t" > "$WORK/t.csv" 2>"$WORK/t.err"
rc=$?
rows=$(wc -l < "$WORK/t.csv")
echo "scene exit=$rc, $rows rows of trajectory"
if grep -qiE 'nan|inf' "$WORK/t.csv"; then echo "*** the scene went non-finite ***"; fi

# ALL of them at once, against stock. substitute_test.sh swaps one object at a
# time by design, so it never asks whether the recovered set is consistent with
# ITSELF — which is the question definition-of-done item 7 actually poses.
if [ -s "$WORK/baseline.csv" ]; then
    python3 - "$WORK/baseline.csv" "$WORK/t.csv" <<'PY'
import csv, sys
def load(p):
    rows = list(csv.reader(open(p)))
    return [[float(x) for x in r] for r in rows[1:]]
a, b = load(sys.argv[1]), load(sys.argv[2])
if len(a) != len(b):
    print('all substituted TOGETHER: trajectory is SHORT (%d rows vs %d)'
          % (len(b), len(a)))
else:
    w = max((abs(x - y) for r1, r2 in zip(a, b) for x, y in zip(r1[1:], r2[1:])),
            default=0.0)
    verdict = 'bit-identical' if w == 0.0 else 'max delta %.3e m' % w
    print('all substituted TOGETHER vs stock: %s' % verdict)
PY
else
    echo "(no baseline — could not link the stock scene)"
fi
[ -s "$OUT" ] || { echo "FATAL: no census written — did the scene reach exit()?"; exit 1; }

if grep -q 'overflow=1' "$OUT"; then
    echo "*** census table overflowed: counts are INCOMPLETE, raise KD_INSTR_SLOTS ***"
fi

# Symbolise against the linked executable. -no-pie, so the addresses the
# instrumentation recorded are the ones in the symbol table. Local symbols
# count: a file-static helper is exactly the kind of thing that turns out to be
# where all the arithmetic lives.
nm -n "$WORK/t" | awk '$2 ~ /^[tTwW]$/ {print $1, $3}' > "$WORK/syms.txt"
python3 - "$OUT" "$WORK/syms.txt" "$CDIR" $objs <<'PYEOF'
import sys, os, re, bisect
census, symfile, cdir = sys.argv[1], sys.argv[2], sys.argv[3]
objs = sys.argv[4:]

addrs, names = [], []
for line in open(symfile):
    a, n = line.split()
    addrs.append(int(a, 16)); names.append(n)

def sym(a):
    i = bisect.bisect_right(addrs, a) - 1
    return names[i] if i >= 0 else '?'

counts = {}
for line in open(census):
    if line.startswith('#'): continue
    a, c = line.split()
    counts[sym(int(a, 16))] = counts.get(sym(int(a, 16)), 0) + int(c)

# Which functions does each instrumented object define? A name that never
# appears in the census is a function the scene did not reach, and that is the
# result this tool exists to report — so the zero rows have to be enumerated,
# not left out.
import subprocess
per_obj = {}
for o in objs:
    base = os.path.basename(o)[:-2]
    out = subprocess.run(['nm', '--defined-only', o], capture_output=True, text=True).stdout
    fns = [l.split()[-1] for l in out.splitlines() if len(l.split()) == 3 and l.split()[1] in 'tT']
    per_obj[base] = sorted(set(fns))

live = dead = 0
print()
print('%-34s %-46s %12s' % ('object', 'function', 'calls'))
print('-' * 94)
for base in sorted(per_obj):
    hit = [(f, counts.get(f, 0)) for f in per_obj[base]]
    n_live = sum(1 for _, c in hit if c)
    tot = sum(c for _, c in hit)
    live += n_live; dead += len(hit) - n_live
    mark = ' ' if n_live else '*'
    print('%s%-33s %-46s %12s' % (mark, base,
          '(%d of %d functions ran)' % (n_live, len(hit)),
          '{:,}'.format(tot) if tot else 'NEVER RAN'))
    for f, c in sorted(hit, key=lambda x: -x[1]):
        print('%-34s   %-44s %12s' % ('', f, '{:,}'.format(c) if c else '-'))
print('-' * 94)
print('%d function(s) executed, %d never reached' % (live, dead))
print()
print('An object marked * contributed NOTHING to this scene: any verdict the')
print('substitute gate printed for it on this scene is about the link, not the code.')
PYEOF
