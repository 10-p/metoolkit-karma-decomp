#!/usr/bin/env bash
# float_sentinel.sh — the float-sentinel comparisons, driven at the sentinel, against MathEngine's SHIPPED library.
#
#   ./float_sentinel.sh                                      # the product: metoolkit_decomp/src
#   KD_SENTINEL_SRC=/tmp/kd_lp64/allobj ./float_sentinel.sh  # a post-passed generator tree instead
#   KD_KEEP=1 ./float_sentinel.sh                            # keep the binaries and the outputs
#
# Another 64-bit ABI for the candidate, built and run as lineseg_lp64.sh does it:
#   KD_CC64=x86_64-w64-mingw32-gcc KD_RUN64=wine KD_LDFLAGS64=-static KD_LIBS64="-lstdc++ -lm -lws2_32" ...
#   KD_CC64=aarch64-linux-gnu-gcc KD_RUN64="qemu-aarch64 -L /usr/aarch64-linux-gnu" ...
#
# WHAT IT ANSWERS. Ghidra prints a float constant as an unsuffixed decimal, so a comparison against
# MEINFINITY reads `x != 3.4028235e+38` — a DOUBLE, which promoted FLT_MAX never equals. Does every
# entry point whose result rests on such a test give the shipped answer when the operand IS the sentinel?
# IxAggregateLineSegment and McdSpace's line-segment queries report a hit on a miss when it does not.
#
# Three builds of float_sentinel.c (ref: shipped -m32; c32: product -m32 SSE; c64: product at LP64),
# compared by float_sentinel_compare.py: ret exactly, every value the reference defines within 1e-3,
# and c64 against c32 bit-identical.
set -uo pipefail
. "$(cd "$(dirname "$0")" && pwd)/../../lib/kd-paths.sh"
kd_require_metoolkit || exit 2

CC64="${KD_CC64:-gcc}"
RUN64="${KD_RUN64:-}"
W=$(mktemp -d)
[ "${KD_KEEP:-0}" = 1 ] || trap 'rm -rf "$W"' EXIT

srcs=()
while read -r lib file; do
    case "$lib" in ''|\#*) continue ;; esac
    if [ -n "${KD_SENTINEL_SRC:-}" ] && [ "$lib" != McdConvexCreateHull ]; then
        srcs+=("$KD_SENTINEL_SRC/$file")
    else
        srcs+=("$KD_MD_SRC/$lib/$file")
    fi
done < "$KD_MD/library-map.txt"
for c in "${srcs[@]}"; do [ -f "$c" ] || { echo "  no such source: $c"; exit 2; }; done
echo "  candidate: ${#srcs[@]} source(s) from ${KD_SENTINEL_SRC:-$KD_MD_SRC}"

HF="-std=gnu99 -O1 -ffp-contract=off -fno-strict-aliasing -w -DLINUX -DME_FORCE_CDECL=1 $KD_INCS"
PF="-std=gnu99 -fno-strict-aliasing -w -Wno-int-conversion -Wno-incompatible-pointer-types"
PF="$PF -DLINUX=1 -DME_FORCE_CDECL=1 -I$KD_MD_INC $KD_INCS -O2"

build_product() {   # <dir> <compiler> [flags...]
    local dir="$1"; shift
    mkdir -p "$dir"
    # shellcheck disable=SC2016,SC2086
    printf '%s\n' "${srcs[@]}" | xargs -P "${KD_JOBS:-4}" -I{} sh -c '
        d=$1 c=$2; shift 2
        o="$d/$(basename "$c" .c).o"
        "$@" -c -o "$o" "$c" 2>"${o%.o}.err" || { echo "  did not compile: $c"; exit 255; }
    ' _ "$dir" {} "$@" $PF
}

echo "== building =="
mkdir -p "$W/ref"
gcc -m32 -msse2 -mfpmath=sse $HF -no-pie -o "$W/ref/run" "$KD_T_STD/float_sentinel.c" \
    -Wl,--start-group "$KD_MT_LIB"/*.a -Wl,--end-group -lstdc++ -lm 2>"$W/ref.err" \
    || { echo "  the reference did not link:"; head -5 "$W/ref.err"; exit 2; }
build_product "$W/c32" gcc -m32 -msse2 -mfpmath=sse -fPIC || exit 2
gcc -m32 -msse2 -mfpmath=sse $HF -o "$W/c32/run" "$KD_T_STD/float_sentinel.c" "$W/c32"/*.o \
    -lstdc++ -lm 2>"$W/c32.err" || { echo "  c32 did not link:"; head -5 "$W/c32.err"; exit 2; }
PIC64=-fPIC; case "$CC64" in *mingw*) PIC64= ;; esac
# shellcheck disable=SC2086
build_product "$W/c64" "$CC64" $PIC64 ${KD_CFLAGS64:-} || exit 2
"$CC64" $HF ${KD_CFLAGS64:-} -o "$W/c64/run" "$KD_T_STD/float_sentinel.c" "$W/c64"/*.o ${KD_LIBS64:--lstdc++ -lm} \
    ${KD_LDFLAGS64:-} 2>"$W/c64.err" || { echo "  c64 did not link:"; head -5 "$W/c64.err"; exit 2; }
EXE64="$W/c64/run"; [ -f "$EXE64" ] || EXE64="$W/c64/run.exe"

echo "== running =="
for b in ref c32; do
    timeout 120 "$W/$b/run" > "$W/$b.txt" || { echo "  $b exited $?"; exit 2; }
done
# shellcheck disable=SC2086
timeout 600 $RUN64 "$EXE64" | tr -d '\r' > "$W/c64.txt" || { echo "  c64 exited $?"; exit 2; }

echo "== c32 and c64 against the shipped library =="
python3 "$KD_T_STD/float_sentinel_compare.py" "$W/ref.txt" "$W/c32.txt" "$W/c64.txt"
rc=$?
[ "${KD_KEEP:-0}" = 1 ] && echo "  (KD_KEEP: kept in $W)"
exit $rc
