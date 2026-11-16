#!/usr/bin/env bash
# lineseg_lp64.sh — Karma's line-segment intersections at LP64, against MathEngine's SHIPPED library.
#
#   ./lineseg_lp64.sh                                     # the product: metoolkit_decomp/src
#   KD_LINESEG_SRC=/tmp/kd_lp64/allobj ./lineseg_lp64.sh  # a post-passed generator tree instead
#   KD_LINESEG_N=10000 ./lineseg_lp64.sh                  # cases per category per geometry (2500)
#   KD_LINESEG_OPT=-O0 ./lineseg_lp64.sh                  # the candidate's -O (default -O2)
#   KD_KEEP=1 ./lineseg_lp64.sh                           # keep the binaries and the three outputs
#
# Another 64-bit ABI, built and run elsewhere, compared the same way (the reference and the i386
# control are always built here):
#   KD_CC64=x86_64-w64-mingw32-gcc KD_RUN64=wine KD_LDFLAGS64=-static KD_LIBS64="-lstdc++ -lm -lws2_32" \
#     ./lineseg_lp64.sh
#   KD_CC64=aarch64-linux-gnu-gcc KD_RUN64="qemu-aarch64 -L /usr/aarch64-linux-gnu" ./lineseg_lp64.sh
#   KD_CC64=$NDK/aarch64-linux-android24-clang KD_LIBS64="-lc++abi -lm" KD_LDFLAGS64=-static \
#     KD_RUN64="qemu-aarch64" ./lineseg_lp64.sh            # Android: a static bionic executable
#   KD_CFLAGS64=-ffp-contract=off ...                    # extra candidate flags (aarch64: no FMA, so
#                                                         # the i386 control comparison stays exact)
#
# WHAT IT ANSWERS. Does every line-segment function Karma registers — sphere, box, plane, cylinder,
# sphyl, convex mesh, aggregate — give the shipped i386 library's answer when the recovered source runs
# at 64-bit pointer width? `USkeletalMesh::LineCheck` asks exactly these for every zero-extent trace
# against a ragdoll, one call per bone, so this is every shot at a corpse on native x86-64, Win64 and
# Android.
#
# ★ NO OTHER GATE HERE COULD SEE IT. The scenes collide bodies and never trace them; difftest_pair.sh
# drives PAIR interactions at i386; ktrace follows bodies, not traces; and the i386 acceptance test is
# blind to every layout defect by construction. `IxSphylLineSegment` read `McdSphyl::mRadius` and
# `mHalfHeight` at their i386 offsets 0x10/0x14 — the low halves of `_McdGeometry`'s `prev`/`next`
# pointers at LP64 — and passed all of them. See ../../proven.txt LP64-LINESEG-REGISTRATION.
#
# THREE BUILDS of lineseg_lp64.c, one seeded case set, compared by lineseg_compare.py:
#
#   ref   -m32, linked against $KD_MT_LIB's shipped archives            the reference
#   c64   the recovered product at LP64, the product's flags + -O2      the candidate
#   c32   the recovered product at -m32 -msse2 -mfpmath=sse, same flags the pointer-width control
#
# c64 against ref has a TOLERANCE (x87 against SSE; lineseg_compare.py says how much and why); c64
# against c32 has NONE — the same source with the same SSE arithmetic, so only the pointer width is
# left, and every defined field must be bit-identical. The harness's own arithmetic is SSE at both
# widths so the INPUTS are bit-identical in all three, and the comparator checks that per case.
set -uo pipefail
. "$(cd "$(dirname "$0")" && pwd)/../../lib/kd-paths.sh"
kd_require_metoolkit || exit 2

N="${KD_LINESEG_N:-2500}"
OPT="${KD_LINESEG_OPT:--O2}"
CC64="${KD_CC64:-gcc}"
RUN64="${KD_RUN64:-}"
J="${KD_JOBS:-4}"
W=$(mktemp -d)
[ "${KD_KEEP:-0}" = 1 ] || trap 'rm -rf "$W"' EXIT

# ---- the candidate's sources: library-map.txt names every object the product builds, and the
# convex hull replacement always comes from the product tree (it is hand-written, not generated)
srcs=()
while read -r lib file; do
    case "$lib" in ''|\#*) continue ;; esac
    if [ -n "${KD_LINESEG_SRC:-}" ] && [ "$lib" != McdConvexCreateHull ]; then
        srcs+=("$KD_LINESEG_SRC/$file")
    else
        srcs+=("$KD_MD_SRC/$lib/$file")
    fi
done < "$KD_MD/library-map.txt"
for c in "${srcs[@]}"; do [ -f "$c" ] || { echo "  no such source: $c"; exit 2; }; done
echo "  candidate: ${#srcs[@]} source(s) from ${KD_LINESEG_SRC:-$KD_MD_SRC}, $OPT, $N case(s) per category"

# The harness: SSE scalar arithmetic at every width, no contraction, so its inputs are bit-identical.
HF="-std=gnu99 -O1 -ffp-contract=off -fno-strict-aliasing -w -DLINUX -DME_FORCE_CDECL=1 $KD_INCS"
# The product: metoolkit_decomp/CMakeLists.txt's MD_C_FLAGS and definitions.
PF="-std=gnu99 -fno-strict-aliasing -w -Wno-int-conversion -Wno-incompatible-pointer-types"
PF="$PF -DLINUX=1 -DME_FORCE_CDECL=1 -I$KD_MD_INC $KD_INCS $OPT"

build_product() {   # <dir> <compiler> [flags...] — every source, $J at a time
    local dir="$1"; shift
    mkdir -p "$dir"
    # shellcheck disable=SC2016,SC2086
    printf '%s\n' "${srcs[@]}" | xargs -P "$J" -I{} sh -c '
        d=$1 c=$2; shift 2
        o="$d/$(basename "$c" .c).o"
        "$@" -c -o "$o" "$c" 2>"${o%.o}.err" || { echo "  did not compile: $c"; exit 255; }
    ' _ "$dir" {} "$@" $PF
}

echo "== building =="
mkdir -p "$W/ref"
gcc -m32 -msse2 -mfpmath=sse $HF -no-pie -o "$W/ref/run" "$KD_T_STD/lineseg_lp64.c" \
    -Wl,--start-group "$KD_MT_LIB"/*.a -Wl,--end-group -lstdc++ -lm 2>"$W/ref.err" \
    || { echo "  the reference did not link:"; head -5 "$W/ref.err"; exit 2; }

build_product "$W/c32" gcc -m32 -msse2 -mfpmath=sse -fPIC || exit 2
gcc -m32 -msse2 -mfpmath=sse $HF -o "$W/c32/run" "$KD_T_STD/lineseg_lp64.c" "$W/c32"/*.o \
    -lstdc++ -lm 2>"$W/c32.err" || { echo "  c32 did not link:"; head -5 "$W/c32.err"; exit 2; }

PIC64=-fPIC; case "$CC64" in *mingw*) PIC64= ;; esac
# shellcheck disable=SC2086
build_product "$W/c64" "$CC64" $PIC64 ${KD_CFLAGS64:-} || exit 2
"$CC64" $HF ${KD_CFLAGS64:-} -o "$W/c64/run" "$KD_T_STD/lineseg_lp64.c" "$W/c64"/*.o ${KD_LIBS64:--lstdc++ -lm} \
    ${KD_LDFLAGS64:-} 2>"$W/c64.err" || { echo "  c64 did not link:"; head -5 "$W/c64.err"; exit 2; }
EXE64="$W/c64/run"; [ -f "$EXE64" ] || EXE64="$W/c64/run.exe"     # MinGW appends .exe

echo "== running =="
for b in ref c32; do
    timeout 900 "$W/$b/run" "$N" > "$W/$b.txt" || { echo "  $b exited $?"; exit 2; }
done
# shellcheck disable=SC2086
timeout 1800 $RUN64 "$EXE64" "$N" | tr -d '\r' > "$W/c64.txt" || { echo "  c64 exited $?"; exit 2; }
echo "  $(grep -vc '^#' "$W/ref.txt") case(s); pointer widths: ref $(sed -n 's/.*pointer=\([0-9]*\).*/\1/p' "$W/ref.txt"), c32 $(sed -n 's/.*pointer=\([0-9]*\).*/\1/p' "$W/c32.txt"), c64 $(sed -n 's/.*pointer=\([0-9]*\).*/\1/p' "$W/c64.txt")"

echo "== c64 against the shipped library, and against the i386 control =="
# the differences the recovery has at EVERY width are pinned there, not here
. "$KD_ROOT/test/baselines.sh"
KD_BASE_LINESEG="$KD_BASE_LINESEG" python3 "$KD_T_STD/lineseg_compare.py" "$W/ref.txt" "$W/c64.txt" "$W/c32.txt"
rc=$?
[ "${KD_KEEP:-0}" = 1 ] && echo "  (KD_KEEP: kept in $W)"
exit $rc
