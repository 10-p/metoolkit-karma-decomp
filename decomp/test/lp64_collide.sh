#!/usr/bin/env bash
# lp64_collide.sh — diff the COLLISION half between i386 and LP64.
#
#   ./lp64_collide.sh                 # uses $KD_OUT, default /tmp/kd_lp64
#
# `lp64_run.sh` says two scenes diverge and `scene_chain` — collision-free —
# does not, which localises the remaining pointer-width defect to the collision
# half and no further: a trajectory is the sum of everything, so the first
# differing POSITION names nothing.
#
# This builds `test/lp64_collide.c` at BOTH widths from the same sources and
# diffs what the pipeline's own intersect callback reports. The first differing
# line names the PAIR and the FIELD, which names the object.
#
# ⚠ THE FLAGS MUST MATCH DOWN TO -O AND -mfpmath. An -O2 reference against an
# -O1 candidate measures x87 excess-precision rounding, not pointer width — and
# it reads plausibly either way. Both sides here use one CF.
set -u
HERE="$(cd "$(dirname "$0")/.." && pwd)"
MT="${KD_METOOLKIT:-$HERE/../Thirdparty/metoolkit}"
INC="$MT/include"
SRCDIR="${KD_OUT:-/tmp/kd_lp64}/allobj"
BUILD="${KD_BUILD:-/tmp/kd_build}"
FPMATH="${KD_FPMATH:--mfpmath=387}"
IF="-I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon -I$INC/McdPrimitives"
IF="$IF -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp -I$HERE/include"
CF="-O1 -g -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion"
CF="$CF -Wno-incompatible-pointer-types -DLINUX $IF"

W=$(mktemp -d)
[ "${KD_KEEP:-0}" = 1 ] || trap 'rm -rf "$W"' EXIT

for bits in 32 64; do
    mkdir -p "$W/o$bits"
    for c in "$SRCDIR"/*.c; do
        b=$(basename "$c" .c)
        [ -f "$BUILD/$b.o" ] || continue      # only what is IN the build
        gcc -m$bits $CF $FPMATH -c -o "$W/o$bits/$b.o" "$c" 2>/dev/null \
            || { echo "  did not compile at m$bits: $b"; exit 1; }
    done
    gcc -m$bits $CF $FPMATH -c -o "$W/o$bits/hull.o" \
        "$HERE/src/McdConvexCreateHull/kd_convexhull.c" 2>/dev/null || exit 1
    gcc -m$bits $CF $FPMATH -no-pie -o "$W/probe$bits" \
        "$HERE/test/lp64_collide.c" "$W/o$bits"/*.o -lstdc++ -lm 2>"$W/link$bits.err" \
        || { echo "  did not link at m$bits:"; grep -m5 'undefined reference' "$W/link$bits.err"; exit 1; }
    timeout 600 "$W/probe$bits" > "$W/out$bits.csv" 2>/dev/null
done

a=$(wc -l < "$W/out32.csv"); b=$(wc -l < "$W/out64.csv")
echo "  i386 reported $a intersect result(s), LP64 $b"

python3 - "$W/out32.csv" "$W/out64.csv" <<'PY'
import sys
A=[l.rstrip('\n').split(',') for l in open(sys.argv[1])]
B=[l.rstrip('\n').split(',') for l in open(sys.argv[2])]
for i,(x,y) in enumerate(zip(A,B)):
    if len(x)!=len(y):
        print('  LINE %d: %d field(s) at i386, %d at LP64'%(i,len(x),len(y)))
        print('     i386 %s'%','.join(x[:6])); print('     lp64 %s'%','.join(y[:6])); sys.exit(1)
    for j,(p,q) in enumerate(zip(x,y)):
        if p==q: continue
        try: fp,fq=float(p),float(q)
        except ValueError:
            print('  LINE %d field %d: %r vs %r'%(i,j,p,q)); sys.exit(1)
        # An identical computation must print identically; anything else is a
        # difference, and how BIG it is says whether it is noise or a defect.
        print('  FIRST DIFFERENCE line %d field %d: i386=%s lp64=%s  (|delta| %.3e)'
              % (i,j,p,q,abs(fp-fq)))
        print('     i386 %s'%','.join(x[:5]))
        print('     lp64 %s'%','.join(y[:5]))
        sys.exit(1)
if len(A)!=len(B):
    print('  the shorter output is a prefix of the longer: %d vs %d lines'%(len(A),len(B)))
    print('     first extra line: %s'%','.join((A if len(A)>len(B) else B)[min(len(A),len(B))][:6]))
    sys.exit(1)
print('  the collision half is IDENTICAL at both widths (%d results)'%len(A))
PY
rc=$?
[ "${KD_KEEP:-0}" = 1 ] && echo "  (KD_KEEP: kept in $W)"
exit $rc
