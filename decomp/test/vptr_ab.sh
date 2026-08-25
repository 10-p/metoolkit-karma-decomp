#!/usr/bin/env bash
# vptr_ab.sh — does the repaired vtable dispatch reach the function it claims?
#
#   vptr_ab.sh <recovered-c-dir> <recovered-obj-dir> <metoolkit-lib-dir> <object> <scene.c>...
#
# ghidra_clean's fix_vptr_store turns Ghidra's undeclared `_vanillaQMatrix` into
# an alias for the local's first word and stores the vtable ADDRESS POINT into
# it. Six static checks stand behind that (see its docstring), but every one of
# them is about the object file. None of them observes the dispatch RUNNING.
#
# That gap is not academic. substitute_test.sh reports keaLCPSolver
# `trajectory bit-identical` on scene_chain, and scene_census.sh says only 4 of
# its 15 functions ran there — none of the three the repair touched. A green
# result whose measurement cannot see the failure is the recurring hazard in
# this project, and this is one.
#
# WHAT THIS DOES. Rebuild the object with every repaired dispatch replaced by a
# DIRECT call to the symbol that slot holds, resolved by the linker from the
# name. The two builds differ in exactly one way: one loads a function pointer
# out of memory at run time, the other does not. If their trajectories are
# bit-identical then the address point, the scale and the slot offsets all
# reach those functions — dynamically, not by argument.
#
# The slot symbols come from tools/vtable_slots.py, i.e. from the relocations in
# the object that DEFINES the vtable. They are not read back out of the rewrite
# being tested.
#
# AND THE CONTROLS ARE THE POINT. A dispatch block that never executes makes
# both builds trivially identical, so the script also builds two deliberately
# wrong versions — one slot rotated, and the ABI address point moved from +8 to
# +12 — and requires the scene to NOTICE. A scene where the controls come back
# clean is a scene that never reached the code, and it says so.
set -u

CDIR="${1:?recovered .c dir}"
ODIR="${2:?recovered .o dir}"
LIBDIR="${3:?metoolkit lib dir}"
OBJ="${4:?object base name}"
shift 4
[ $# -gt 0 ] || { echo "usage: vptr_ab.sh <c-dir> <o-dir> <lib-dir> <object> <scene.c>..."; exit 2; }

HERE="$(cd "$(dirname "$0")/.." && pwd)"
INC="$(dirname "$LIBDIR")/../include"
CORPUS="${KD_CORPUS:-/home/ion/tools/karma-lab/allobj}"
W=$(mktemp -d); trap 'rm -rf "$W"' EXIT
IF="-I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon -I$INC/McdPrimitives"
IF="$IF -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp"
CF="-m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion"
CF="$CF -Wno-incompatible-pointer-types -DLINUX -I$HERE/include $IF"

# ---- generate the devirtualised source and the two controls -----------------
python3 - "$CDIR/$OBJ.c" "$CORPUS" "$HERE" "$W" <<'PYEOF' || exit 1
import os, re, subprocess, sys
src_path, corpus, here, work = sys.argv[1:5]
sys.path.insert(0, os.path.join(here, 'tools'))
import vtable_slots

src = open(src_path).read()
# What fix_vptr_store emits: `(*(T **)&NAME) = (T *)((char *)&VT[0] + 8);`
stores = re.findall(r'\(\*\((\w+) \*\*\)&(\w+)\) = \(\1 \*\)\(\(char \*\)&(\w+)\[0\] \+ 8\);', src)
if not stores:
    print('  no repaired vptr store in this object — nothing to A/B'); sys.exit(1)

WIDTH = {'char': 1, 'undefined1': 1, 'undefined2': 2, 'undefined4': 4, 'undefined8': 8}
out, syms, n = src, set(), 0
for etype, name, _vt in sorted(set(stores)):
    m = re.search(r'^[ \t]*([A-Za-z_]\w*)[ \t]+%s[ \t]*;' % re.escape(name), src, re.M)
    if not m:
        print('  cannot find the local declaration for %s' % name); sys.exit(1)
    cls = m.group(1)
    table = vtable_slots.slot_table(corpus, cls)
    if not table:
        print('  no vtable for %s in %s' % (cls, corpus)); sys.exit(1)
    w = WIDTH[etype]
    A = re.escape('(*(%s **)&%s)' % (etype, name))
    forms = [(r'\(\*\(code \*\)\*' + A + r'\)', lambda g: 0),
             (r'\(\*\(code \*\)' + A + r'\[(0x[0-9a-f]+|\d+)\]\)', lambda g: int(g, 0) * w),
             (r'\(\*\(\*\(code \*\*\)\(' + A + r'\)\)\)', lambda g: 0),
             (r'\(\*\*\(code \*\*\)\(' + A + r' \+ (0x[0-9a-f]+|\d+)\)\)', lambda g: int(g, 0) * w),
             (r'\(\*\*\(code \*\*\)' + A + r'\)', lambda g: 0)]
    for pat, off_of in forms:
        def repl(mm, off_of=off_of):
            global n
            off = off_of(mm.group(1)) if mm.groups() else 0
            if off not in table:
                print('  slot +0x%02x is not in %s\'s vtable' % (off, cls)); sys.exit(1)
            n += 1
            syms.add(table[off])
            return table[off]
        out = re.sub(pat, repl, out)

if not n:
    print('  the store was repaired but no dispatch uses it — nothing to A/B'); sys.exit(1)
decl = '\n'.join('extern int %s();' % s for s in sorted(syms))
tag = '/* ---- forward declarations ---- */'
out = out.replace(tag, '/* ---- devirtualised control ---- */\n' + decl + '\n\n' + tag, 1)
open(os.path.join(work, 'devirt.c'), 'w').write(out)

# CONTROL A: one slot rotated onto another real function of the same class.
ordered = sorted(syms)
if len(ordered) < 2:
    print('  only one distinct slot; the rotation control cannot be built'); sys.exit(1)
rot = out.replace(ordered[0], '@@KD@@').replace(ordered[1], ordered[0]).replace('@@KD@@', ordered[1])
open(os.path.join(work, 'ctl_slot.c'), 'w').write(rot)
# CONTROL B: the ABI address point moved off +8, in the object AS REPAIRED.
open(os.path.join(work, 'ctl_ap.c'), 'w').write(
    re.sub(r'(\(\(char \*\)&\w+\[0\] \+ )8\)', r'\g<1>12)', src))
print('  %d dispatch(es) over %d slot(s): %s'
      % (n, len(syms), ', '.join(s[:38] for s in ordered)))
PYEOF

for v in devirt ctl_slot ctl_ap; do
    gcc $CF -c -o "$W/$v.o" "$W/$v.c" || { echo "FATAL: $v did not compile"; exit 1; }
done

# ---- link each variant into each scene beside the shipped library -----------
cp "$LIBDIR"/*.a "$W/"
for a in "$W"/*.a; do
    ar t "$a" 2>/dev/null | grep -qx "$OBJ.o" && { ar d "$a" "$OBJ.o"; break; }
done

run_pair() {   # scene, variant.o  ->  prints the trajectory delta or 'crashed'
    local scene="$1" obj="$2" tag="$3"
    gcc -m32 -O2 -DLINUX -no-pie $IF -o "$W/bin_$tag" "$scene" "$obj" \
        -Wl,--start-group "$W"/*.a -Wl,--end-group -lstdc++ -lm 2>/dev/null \
        || { echo "link-failed"; return; }
    timeout 120 "$W/bin_$tag" > "$W/$tag.csv" 2>/dev/null || { echo "crashed"; return; }
    echo ok
}

fail=0
for scene in "$@"; do
    sname=$(basename "$scene" .c)
    echo "### $sname"
    [ "$(run_pair "$scene" "$ODIR/$OBJ.o" a)" = ok ] || { echo "  FATAL: as-repaired build failed"; exit 1; }
    for v in devirt ctl_slot ctl_ap; do
        st=$(run_pair "$scene" "$W/$v.o" "$v")
        if [ "$st" != ok ]; then
            d="$st"
        else
            d=$(python3 - "$W/a.csv" "$W/$v.csv" <<'PY'
import csv, sys
def load(p):
    r = list(csv.reader(open(p)))
    return [[float(x) for x in row] for row in r[1:]]
a, b = load(sys.argv[1]), load(sys.argv[2])
if len(a) != len(b):
    print('short'); raise SystemExit
w = 0.0
for ra, rb in zip(a, b):
    for x, y in zip(ra[1:], rb[1:]):
        w = max(w, abs(x - y))
print('%.3e' % w)
PY
)
        fi
        case "$v" in
        devirt)
            if [ "$d" = "0.000e+00" ]; then
                echo "  [  ok  ] devirtualised control — trajectory bit-identical"
            else
                echo "  [ FAIL ] devirtualised control — $d"; fail=$((fail+1))
            fi ;;
        *)
            label="rotated slot"; [ "$v" = ctl_ap ] && label="address point +12"
            if [ "$d" = "0.000e+00" ]; then
                echo "  [ blind] $label changed NOTHING — this scene never reaches the dispatch"
            else
                echo "  [ seen ] $label — $d"
            fi ;;
        esac
    done
done

echo
echo "A bit-identical devirtualised control is evidence ONLY on a scene where"
echo "the two wrong versions were SEEN. Where they read [blind] the dispatch"
echo "did not execute and the scene proves nothing about it."
exit $((fail > 0))
