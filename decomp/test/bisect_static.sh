#!/usr/bin/env bash
# bisect_static.sh — which FILE-STATIC function inside a recovered object is the
# wrong one, measured through difftest_pair.sh.
#
#   bisect_static.sh <object> <difftest-pair-name> <fn[,fn...]>|--none|--all [iters]
#
# e.g.  ./test/bisect_static.sh IxBoxTriList McdBoxTriangleListIntersect --none
#
# WHY THIS EXISTS AND bisect_object.sh DOES NOT ANSWER IT. bisect_object.sh
# arbitrates GLOBAL symbols — it weakens the shipped member's globals and
# localises all but one of the recovered object's. IxBoxTriList has two globals
# and five file-statics, and the whole collision algorithm is in the statics, so
# there is no symbol for the linker to pick between. That object sat REFUSED for
# four sessions on "difftest measures it diverging on 139,961 of 200,000 pairs",
# which named the object and nothing inside it.
#
# HOW. The substitution is done in the SOURCE, not the link: the chosen function
# keeps its name and prototype and its body becomes a forward call into a
# `sh_`-prefixed, globalised copy of the SHIPPED object. Inlining therefore
# cannot defeat it, which matters — gcc inlines three of IxBoxTriList's five
# statics, so the compiled object does not even have symbols for them.
#
# READ IT AS A CALL TREE, NOT A LIST. Shipping a function also ships everything
# it calls, because those are direct calls inside the shipped object. So
# `--none` FAIL + `<root>` PASS localises the defect to the root's own body ONLY
# if some function BELOW the root also reads FAIL; if every callee reads PASS
# too, the defect is in the root. IxBoxTriList had defects at two levels and
# neither masked the other, because each alone diverged on 70% of pairs.
#
# THE TWO CONTROLS ARE THE POINT. `--none` must reproduce the known divergence
# and `--all` must read clean; a harness that cannot do both is measuring
# itself. Both are one run each.
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
MT="${KD_METOOLKIT:-$HERE/../Thirdparty/metoolkit}"
INC=$MT/include
LIB=$MT/lib.rel/linux_single_gcc3.2
OUTDIR="${KD_OUT:-/tmp/kd_out}/allobj"
BUILD="${KD_BUILD:-/tmp/kd_build}"
SHIPDIR="${KD_SHIPPED:-/home/ion/tools/karma-lab/allobj}"

OBJ="${1:?object base name, e.g. IxBoxTriList}"
PAIR="${2:?difftest_pair.sh pair name}"
WHICH="${3:?fn[,fn...] | --none | --all}"
ITERS="${4:-20000}"
SRC="${KD_SRC:-$OUTDIR/$OBJ.c}"
SHIPPED="$SHIPDIR/$OBJ.o"

W=$(mktemp -d); trap 'rm -rf "$W"' EXIT

# ---- the shipped object, every symbol globalised and prefixed sh_ ----------
# Two objcopy passes, not one: --globalize-symbol matches the name BEFORE
# --redefine-syms in a combined invocation, so the combined form silently
# globalises nothing and the link fails on an undefined sh_*.
cp "$SHIPPED" "$W/sh.o"
nm --defined-only --format=posix "$W/sh.o" \
    | awk '$2 ~ /^[tTdDbBrRvVwW]$/ {print $1}' > "$W/defined"
: > "$W/glob.args"; : > "$W/map"
while read -r s; do
    echo "--globalize-symbol=$s" >> "$W/glob.args"
    echo "$s sh_$s" >> "$W/map"
done < "$W/defined"
objcopy @"$W/glob.args" "$W/sh.o" "$W/sh_glob.o"
objcopy --redefine-syms="$W/map" "$W/sh_glob.o" "$W/sh_pref.o"

# The mangled ELF name of each file-static, read from the shipped symbol table
# rather than spelled here: a C++ static's asm label is its mangled name and
# nothing in the recovered source records it.
nm --defined-only --format=posix "$W/sh.o" | awk '$2 == "t" {print $1}' > "$W/statics"

python3 - "$SRC" "$WHICH" "$W/statics" > "$W/$OBJ.c" <<'PY'
import re, subprocess, sys
src   = open(sys.argv[1]).read()
which = sys.argv[2]

MANGLED = {}
for sym in open(sys.argv[3]).read().split():
    plain = sym
    if sym.startswith('_Z'):
        plain = subprocess.run(['c++filt', sym], capture_output=True,
                               text=True).stdout.split('(')[0].strip()
        plain = plain.split()[-1].split('::')[-1]
    MANGLED[plain] = sym

targets = [] if which == '--none' else (
    list(MANGLED) if which == '--all' else which.split(','))

def declared(fn):
    """Does the RECOVERED source define this static at all?

    Not every file-static of the shipped object has one. IxBoxTriList's
    `MeVector3Normalize` is a static there and a header MeINLINE here, so the
    recovered unit inlines MeMath.h's copy and never defines the name — which
    is correct, and is also the same source the shipped one was compiled from.
    `--all` skips those rather than dying; naming one explicitly still errors."""
    return re.search(r'^static\s+[\w \*]+?\s*' + fn + r'\s*\([^;{}]*?\)\s*;',
                     src, re.M | re.S) is not None

if which == '--all':
    skipped = [f for f in targets if not declared(f)]
    targets = [f for f in targets if declared(f)]
    if skipped:
        sys.stderr.write('bisect_static: not defined in the recovered source, '
                         'skipped: %s\n' % ', '.join(sorted(skipped)))

def split_params(s):
    out, depth, cur = [], 0, ''
    for ch in s:
        if ch in '([': depth += 1
        elif ch in ')]': depth -= 1
        if ch == ',' and depth == 0: out.append(cur); cur = ''
        else: cur += ch
    if cur.strip(): out.append(cur)
    return [p.strip() for p in out]

def param_name(p):
    # the declarator name is the LAST identifier: array bounds in these
    # prototypes are numeric (`MeReal (*edge) [3]`), so nothing else can be.
    return re.findall(r'[A-Za-z_]\w*', p)[-1]

for fn in targets:
    if fn not in MANGLED:
        sys.exit('bisect_static: %s is not a file-static of this object' % fn)
    # The forward declaration carries the full prototype. `[^;{}]` keeps the
    # match inside ONE declaration — with `.*?` the definition pattern below
    # matches from the forward declaration all the way to the body's brace and
    # deletes every declaration in between.
    m = re.search(r'^static\s+([\w \*]+?)\s*' + fn + r'\s*\(([^;{}]*?)\)\s*;',
                  src, re.M | re.S)
    if not m:
        sys.exit('bisect_static: no forward declaration for ' + fn)
    ret, params = m.group(1).strip(), m.group(2)
    names = [param_name(p) for p in split_params(params)]
    d = re.search(r'^static\s+[\w \*]+?\s*\n?' + fn + r'\s*\n?\s*\(([^;{}]*?)\)\s*\n\{',
                  src, re.M | re.S)
    if not d:
        sys.exit('bisect_static: no definition for ' + fn)
    i, depth, j = d.end() - 1, 0, d.end() - 1
    while True:
        if src[j] == '{': depth += 1
        elif src[j] == '}':
            depth -= 1
            if depth == 0: break
        j += 1
    src = src[:d.start()] + (
        'extern %s sh_%s(%s) __asm__("sh_%s");\n'
        'static %s %s(%s)\n{\n  %ssh_%s(%s);\n}'
        % (ret, fn, params, MANGLED[fn], ret, fn, params,
           '' if ret == 'void' else 'return ', fn, ', '.join(names))
    ) + src[j + 1:]

sys.stdout.write(src)
PY

rm -rf "$W/build"; cp -a "$BUILD" "$W/build"
gcc -m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w \
    -Wno-int-conversion -Wno-incompatible-pointer-types -DLINUX \
    -I"$HERE/include" -I$INC -I$INC/McdCommon -I$INC/McdPrimitives \
    -I$INC/McdFrame -I$INC/MeGlobals -I$INC/MdtBcl -I$INC/MdtKea -I$INC/Mst -I$INC/MeApp \
    -c -o "$W/build/$OBJ.o" "$W/$OBJ.c"

# stage exactly as difftest_pair.sh does, then add the sh_ copy to the link
for o in "$W/build"/*.o; do
    dst="$W/rec_$(basename "$o")"; cp "$o" "$dst"
    nm --defined-only "$dst" | awk '$2 ~ /^[TDBRWV]$/ {print $3, "rec_" $3}' > "$W/m2"
    [ -s "$W/m2" ] && objcopy --redefine-syms="$W/m2" "$dst"
done
IF="-I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon -I$INC/McdPrimitives"
IF="$IF -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp"
# THE sh_ COPY GOES IN AN ARCHIVE, not on the command line, and this is not
# tidiness. Linking it as a plain object forces ALL of its undefined references
# to be resolved, which pulls archive members in a different order — and on
# IxCylinderCylinder that made the SHIPPED OverlapCylCyl call through a null
# pointer with NOTHING substituted, i.e. the `--none` control segfaulted. In an
# archive it is pulled in only when a substitution actually references it, and
# `--none` then reproduces the object's known baseline exactly.
ar rcs "$W/sh_pref.a" "$W/sh_pref.o"
gcc -m32 -O2 -DLINUX -no-pie $IF -o "$W/difftest" "$HERE/test/difftest_pair.c" \
    "$W"/rec_*.o -Wl,--start-group "$LIB"/*.a "$W/sh_pref.a" -Wl,--end-group -lstdc++ -lm
"$W/difftest" "$PAIR" "$ITERS" 2>&1 | tail -9
