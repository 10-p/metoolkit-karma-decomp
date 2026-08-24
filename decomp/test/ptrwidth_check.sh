#!/usr/bin/env bash
# ptrwidth_check.sh — the gate that can see the arm64 defect.
#
#   ptrwidth_check.sh <kd_out/allobj> <kd_build> [metoolkit-root]
#
# HANDOVER.md §6b says arm64 "compiles 109/109 with identical symbol sets and is
# not trustworthy", and that "no gate can see it" because not one of the seven
# executes arm64 code. This is that gate, and it does not need to execute
# anything.
#
# The recovery puns pointers through integer slots constantly — Ghidra recovers
# a stack slot as `undefined4` and the code stores a pointer in it. On a
# 32-bit-pointer target (i386, wasm32, armv7) that is lossless. On arm64 it
# truncates a 64-bit pointer to 32 bits. Three clang diagnostics name exactly
# that and nothing else:
#
#     -Wint-to-pointer-cast        cast to 'T *' from smaller integer type
#     -Wpointer-to-int-cast        cast to smaller integer type from 'T *'
#     -Wvoid-pointer-to-int-cast   ... from 'void *'
#
# They are enabled on top of -Wno-everything, so the count is those three and
# nothing else. That matters: §6b's original figure — "920 pointer/int
# conversion diagnostics against armv7's 23" — compared arm64's TOTAL warning
# count against armv7's pointer/int subset, on five objects. The conclusion was
# right and the measurement was apples-to-oranges. Measured this way, over all
# 109 objects:
#
#     armv7   0 truncation warnings across  0 objects
#     arm64   2162                    across 65 objects
#
# A zero on armv7 is what makes the arm64 number mean something: the same source
# through the same compiler at 32-bit pointer width is clean, so this is pointer
# WIDTH and not a coding style the diagnostic happens to dislike.
#
# Do NOT "fix" this by adding -Wno-int-to-pointer-cast. The fix is at the
# generator: Ghidra's undefined4 slots have to become pointer-width. §6b.
set -euo pipefail

SRCDIR="${1:?recovered .c dir, e.g. /tmp/kd_out/allobj}"
BUILD="${2:?build dir, to know which objects are IN the build}"
SRC="${3:-$(cd "$(dirname "$0")/../.." && pwd)/Thirdparty/metoolkit}"
NDK="${KD_NDK:-/home/ion/Android/Sdk/ndk/30.0.14904198/toolchains/llvm/prebuilt/linux-x86_64/bin}"

HERE="$(cd "$(dirname "$0")/.." && pwd)"
INC="$SRC/include"
CF="-O2 -fno-strict-aliasing -std=gnu99 -DLINUX -I$HERE/include -I$INC"
for d in McdCommon McdPrimitives McdFrame MeGlobals MdtBcl MdtKea Mst MeApp; do
    CF="$CF -I$INC/$d"
done
CF="$CF -Wno-everything -Wint-to-pointer-cast -Wpointer-to-int-cast"
CF="$CF -Wvoid-pointer-to-int-cast -ferror-limit=0"

if [ ! -x "$NDK/aarch64-linux-android21-clang" ]; then
    echo "ptrwidth_check: no NDK at $NDK — set KD_NDK. (An earlier session"
    echo "  reported 'no cross-compiler' after looking only in /usr/bin.)"
    exit 2
fi

status=0
for tc in armv7a-linux-androideabi21-clang aarch64-linux-android21-clang; do
    tot=0; objs=0; worst=""
    for c in "$SRCDIR"/*.c; do
        b=$(basename "$c" .c)
        [ -f "$BUILD/$b.o" ] || continue      # only what is actually in the build
        n=$("$NDK/$tc" $CF -c -o /dev/null "$c" 2>&1 | grep -c "warning:" || true)
        tot=$((tot + n))
        if [ "$n" -gt 0 ]; then objs=$((objs + 1)); worst="$worst $b:$n"; fi
    done
    printf "  %-34s %6d truncation warning(s) across %d object(s)\n" \
           "${tc%%-*}" "$tot" "$objs"
    case "$tc" in
      armv7a*) [ "$tot" -eq 0 ] || { echo "  !! armv7 is a 32-bit-pointer target and MUST be 0."
                                     echo "     A non-zero here means the diagnostic is firing on"
                                     echo "     something other than pointer width — re-read it"
                                     echo "     before believing the arm64 number."; status=1; } ;;
      aarch64*) if [ "$tot" -gt 0 ]; then
                    echo "     worst:$(echo $worst | tr ' ' '\n' | sort -t: -k2 -rn | head -5 | tr '\n' ' ')"
                    echo "  -> arm64 TRUNCATES POINTERS. Compiling is not porting; see §6b."
                fi ;;
    esac
done
exit $status
