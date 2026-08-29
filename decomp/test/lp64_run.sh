#!/usr/bin/env bash
# lp64_run.sh — RUN the recovered library at 64-bit pointer width, and let
# AddressSanitizer name the defects instead of a static scan bounding them.
#
#   lp64_run.sh [scene.c ...]        default: all three scenes
#
# WHY THIS EXISTS, AND WHY IT DID NOT UNTIL 2026-08-27.
#
# The whole 64-bit story here was compile-time only. `ptrwidth_check.sh` counts
# truncation diagnostics and `layout_check.py` counts structs whose size moves;
# both are static, both report BOUNDS, and neither can say whether the code
# would run — HANDOVER.md said so in those words, on the grounds that "nothing
# on this machine can execute arm64".
#
# THAT IS TRUE AND IT WAS THE WRONG CONCLUSION. **x86-64 is the same data model
# as arm64** — LP64, 64-bit pointers, 8-byte alignment — so every struct that
# changes size on arm64 changes size here, and this machine executes it natively.
# The question was never "can we run arm64", it was "can we run LP64", and the
# answer was always yes.
#
# It could not have been done before today for one reason: it needs a COMPLETE
# recovered library. The shipped metoolkit is i386-only, so any 64-bit link had
# to resolve every symbol from our own code. The drop-in gap reaching zero is
# what makes this possible.
#
# WHAT IT FOUND ON ITS FIRST RUN, in about a second:
#
#     MdtWorld.c:98  heap-buffer-overflow, WRITE of size 4
#     48 bytes after a 564-byte region allocated at MdtWorld.c:95
#         w = (MeMemoryAPI.create)(0x234);
#     0x234 == 564 == sizeof(MdtWorld) ON i386.  At 64-bit it is 880.
#
# That is the FIRST STATEMENT of the first scene. It is also a defect class
# neither static gate counts: not a truncated pointer, not a byte offset, but a
# `sizeof` FROZEN INTO AN ALLOCATION. 105 literal-sized allocations exist across
# 41 objects in the build.
#
# -mfpmath=387 IS LOAD-BEARING. x86-64 defaults to SSE, which is storage
# precision — the same thing wasm32 and arm64 give, and the thing the
# association defect (§11 item 2a) shows up under. Forcing x87 holds the
# floating point identical to the i386 build so that ANY divergence here is a
# POINTER-WIDTH defect and not an arithmetic one. Drop the flag deliberately,
# afterwards, to measure the other defect on its own.
#
# WHAT THIS IS NOT. x86-64 is a proxy for arm64, not a substitute: arm64 faults
# on unaligned access where x86-64 tolerates it, so a clean run here is
# necessary and not sufficient. And UT2004 itself is a 32-bit binary, so this
# gives no in-engine evidence — only the offline scenes.
set -u
HERE="$(cd "$(dirname "$0")/.." && pwd)"
MT="${KD_METOOLKIT:-$HERE/../Thirdparty/metoolkit}"
INC="$MT/include"
SRCDIR="${KD_OUT:-/tmp/kd_out}/allobj"
BUILD="${KD_BUILD:-/tmp/kd_build}"
FPMATH="${KD_FPMATH:--mfpmath=387}"
IF="-I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon -I$INC/McdPrimitives"
IF="$IF -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp"
SCENES=("$@")
[ ${#SCENES[@]} -eq 0 ] && SCENES=("$HERE/test/scene_chain.c"
                                   "$HERE/test/scene_boxes_on_plane.c"
                                   "$HERE/test/scene_ragdoll.c")

W=$(mktemp -d)
# KD_KEEP=1 leaves the objects, the scene binaries and — the reason it exists —
# each scene's stderr, which is the only place the ASan report with its
# addresses, its allocation and its stack actually lives. The summary printed
# below is a list of SITES; diagnosing one needs the report.
[ "${KD_KEEP:-0}" = 1 ] || trap 'rm -rf "$W"' EXIT
# -fsanitize-recover so ONE run reports many defects rather than the first.
CF="-O1 -g -fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer"
CF="$CF -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion"
CF="$CF -Wno-incompatible-pointer-types -DLINUX -I$HERE/include $IF"

n=0
for c in "$SRCDIR"/*.c; do
    b=$(basename "$c" .c)
    [ -f "$BUILD/$b.o" ] || continue          # only what is IN the build
    gcc -m64 $CF $FPMATH -c -o "$W/$b.o" "$c" || { echo "  did not compile at m64: $b"; exit 1; }
    n=$((n+1))
done
gcc -m64 $CF -c -o "$W/kd_convexhull.o" "$HERE/src/McdConvexCreateHull/kd_convexhull.c" || exit 1
echo "  $n recovered object(s) built for x86-64 (LP64), $FPMATH"

# ---- THE CONTROL, and without it none of the rows below mean anything -------
# These sources are decompiled from a 32-bit binary and could simply contain
# bugs. Building the SAME sources with the SAME sanitizer at -m32 and getting a
# clean run is what makes a 64-bit error attributable to POINTER WIDTH rather
# than to the recovery. Measured 2026-08-27: scene_chain at i386 runs 900 steps,
# exit 0, ZERO AddressSanitizer errors; the identical sources at LP64 abort with
# five. Run on the first scene only — it is a control, not a survey.
if [ "${KD_SKIP_CONTROL:-}" != 1 ]; then
    for c in "$SRCDIR"/*.c; do
        b=$(basename "$c" .c); [ -f "$BUILD/$b.o" ] || continue
        gcc -m32 $CF $FPMATH -c -o "$W/c32_$b.o" "$c" 2>/dev/null \
            || { echo "  control: -m32 ASan build failed on $b"; exit 2; }
    done
    gcc -m32 $CF $FPMATH -c -o "$W/c32_hull.o" "$HERE/src/McdConvexCreateHull/kd_convexhull.c" 2>/dev/null
    # ⚠ EVERY SCENE, NOT JUST THE FIRST, AND KEEP THE TRAJECTORY. The error count
    # is only half of what the control can say: the same sources at i386 also
    # produce the trajectory the LP64 build is supposed to reproduce, and a
    # pointer that is wrong but IN BOUNDS shows up there and nowhere else.
    # ⚠ THE FLAGS MUST MATCH DOWN TO -mfpmath AND -O. Comparing an -O2 i386
    # build against the -O1 LP64 one measures x87 excess-precision rounding, not
    # pointer width — it read "boxes diverges at step 45" either way and only one
    # of those readings meant anything.
    for s in "${SCENES[@]}"; do
        cn=$(basename "$s" .c)
        if gcc -m32 $CF $FPMATH -no-pie -o "$W/ctl_$cn" \
               "$s" "$W"/c32_*.o -lstdc++ -lm 2>/dev/null; then
            ASAN_OPTIONS=detect_leaks=0:halt_on_error=0 timeout 600 "$W/ctl_$cn" \
                > "$W/ctl_$cn.csv" 2> "$W/ctl_$cn.err"
            ce=$(grep -c "ERROR: AddressSanitizer" "$W/ctl_$cn.err")
            [ "$ce" -eq 0 ] || { echo "  CONTROL $cn: $ce error(s) at i386 <- THE CONTROL IS DIRTY; nothing below is attributable to pointer width"; exit 2; }
        else
            echo "  CONTROL  $cn could not be built — treat its row as unattributed"
        fi
    done
    echo "  CONTROL  same sources at i386 + ASan: 0 error(s) on ${#SCENES[@]} scene(s) (good)"
    rm -f "$W"/c32_*.o
fi

status=0
for s in "${SCENES[@]}"; do
    name=$(basename "$s" .c)
    if ! gcc -m64 -O1 -g -fsanitize=address -fsanitize-recover=address \
             -fno-omit-frame-pointer $FPMATH -DLINUX -no-pie $IF \
             -o "$W/$name" "$s" "$W"/*.o -lstdc++ -lm 2>"$W/link.err"; then
        echo "  $name: DID NOT LINK — and that is a result, not a skip:"
        grep -m5 "undefined reference" "$W/link.err" | sed 's/^/      /'
        status=1
        continue
    fi
    ASAN_OPTIONS=detect_leaks=0:halt_on_error=0:print_stacktrace=1 \
        timeout 600 "$W/$name" > "$W/$name.csv" 2> "$W/$name.err"
    rc=$?
    errs=$(grep -c "ERROR: AddressSanitizer" "$W/$name.err")
    printf '  %-24s exit %-3d  %s\n' "$name" "$rc" \
        "$( [ "$errs" -eq 0 ] && echo 'no sanitizer error' || echo "$errs AddressSanitizer error(s)")"
    if [ "$errs" -gt 0 ]; then
        status=1
        # the DISTINCT sites, not every hit: one bad allocation reports thousands
        grep -A2 "ERROR: AddressSanitizer" "$W/$name.err" \
            | grep -oE "in [A-Za-z_][A-Za-z0-9_]* [^ ]+\.c:[0-9]+" | sort -u \
            | head -12 | sed 's/^/      /'
    elif [ "$rc" != 0 ]; then
        # ⚠ A CLEAN SANITIZER IS NOT A PASS. Every scene returns non-zero when
        # its OWN verdict fails — `scene_ragdoll` exits 1 on "BLOWN UP", bodies
        # past 1e3 — and that is a pointer-width defect the sanitizer cannot
        # see, because the wrong pointer was still in bounds. Counting only
        # ASan errors reported exactly that case as PASS.
        status=1
        sed -n 's/^/      /p' "$W/$name.err" | tail -4
    fi
    # ---- THE TRAJECTORY, against the i386 control's own. This is the half the
    # error count cannot reach: an in-bounds wrong pointer changes the numbers
    # and nothing else. `scene_chain` is COLLISION-FREE and matches over all 900
    # steps, which is what makes a divergence in the other two attributable to
    # the collision side rather than to arithmetic.
    if [ -s "$W/ctl_$name.csv" ]; then
        python3 - "$W/ctl_$name.csv" "$W/$name.csv" <<'PY'
import sys
a=[l.split(',') for l in open(sys.argv[1]) if l.strip()]
b=[l.split(',') for l in open(sys.argv[2]) if l.strip()]
# ⚠ A SHORTER RUN IS NOT A MATCHING RUN. Zipping two traces of different
# lengths compares only the shorter one, so a scene that died or timed out at
# step 20 reports "matches" on the strength of 20 rows out of 900. HANDOVER
# records the same trap for ktrace's `restZ`.
if len(a)!=len(b):
    print('      TRAJECTORY INCOMPLETE: %d row(s) against the control\'s %d'%(len(b),len(a)))
    sys.exit(0)
worst=0.0
for x,y in zip(a,b):
    for p,q in zip(x[1:],y[1:]):
        try: fp,fq=float(p),float(q)
        except ValueError: continue
        if abs(fp-fq) > 1e-2*max(1.0,abs(fp)):
            print('      TRAJECTORY diverges >1%% from the i386 control at step %s'%x[0])
            sys.exit(0)
        worst=max(worst,abs(fp-fq))
print('      trajectory matches the i386 control over all %d rows (worst |delta| %.2e)'
      %(len(a),worst))
PY
    fi
done
# ---- AND THE SAME SCENES WITHOUT THE SANITIZER ------------------------------
# ★ ASan HIDES THIS CLASS OF DEFECT, WHICH IS THE OPPOSITE OF WHAT IT IS FOR.
# A pointer that lost its top 32 bits addresses somewhere in the low 4 GB — and
# under ASan that region is mapped (its shadow and its allocator live there), so
# the load succeeds and the run reads CLEAN. Without the sanitizer the same
# address is unmapped and it is an immediate SIGSEGV.
#
# Measured: with the ASan build reporting "no sanitizer error" on all three
# scenes, the plain build segfaulted on two of them — in `UpdateHull`, in
# `MdtConstraintGetRowCount` and in `getClampIndices`, each a live pointer
# arriving as two interleaved halves. Three real defects the sanitized run could
# not see. This build is also 20x faster, so it costs a minute and pays for
# itself the first time it fires.
if [ "${KD_SKIP_PLAIN:-}" != 1 ]; then
    echo "== LP64, no sanitizer (ASan MAPS a truncated pointer; bare metal does not) =="
    PF="-O1 -g -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion"
    PF="$PF -Wno-incompatible-pointer-types -DLINUX -I$HERE/include $IF"
    mkdir -p "$W/plain"
    for c in "$SRCDIR"/*.c; do
        b=$(basename "$c" .c); [ -f "$BUILD/$b.o" ] || continue
        gcc -m64 $PF $FPMATH -c -o "$W/plain/$b.o" "$c" 2>/dev/null \
            || { echo "  plain: did not compile at m64: $b"; exit 2; }
    done
    gcc -m64 $PF $FPMATH -c -o "$W/plain/hull.o" \
        "$HERE/src/McdConvexCreateHull/kd_convexhull.c" 2>/dev/null || exit 2
    for s in "${SCENES[@]}"; do
        name=$(basename "$s" .c)
        gcc -m64 $PF $FPMATH -no-pie -o "$W/plain_$name" "$s" "$W/plain"/*.o \
            -lstdc++ -lm 2>/dev/null || { echo "  plain: $name did not link"; status=1; continue; }
        ( timeout 600 "$W/plain_$name" > "$W/plain_$name.csv" 2> "$W/plain_$name.err" ) 2>/dev/null
        rc=$?
        if [ "$rc" = 0 ]; then
            printf '  %-24s exit 0    clean, %s row(s)\n' "$name" \
                   "$(wc -l < "$W/plain_$name.csv")"
        else
            status=1
            printf '  %-24s exit %-3d  %s\n' "$name" "$rc" \
                   "$( [ "$rc" -ge 128 ] && echo "SIGNAL $((rc-128)) after $(wc -l < "$W/plain_$name.csv") row(s)" || echo "verdict failed" )"
            sed -n 's/^/      /p' "$W/plain_$name.err" | tail -3
        fi
    done
fi

[ "$status" = 0 ] && echo "  -> PASS (necessary, NOT sufficient: arm64 also faults on unaligned access)" \
                  || echo "  -> FAIL — 64-bit pointer width breaks this. See the header of this file."
[ "${KD_KEEP:-0}" = 1 ] && echo "  (KD_KEEP: reports and binaries kept in $W)"
exit $status