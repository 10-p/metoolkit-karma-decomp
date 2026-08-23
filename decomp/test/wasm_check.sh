#!/usr/bin/env bash
# wasm_check.sh — compile the recovered set for wasm32 and diff its exported
# symbols against the native build.
#
# §12 item 6 is the actual goal of this project: the recovered C has to build
# for wasm32 and arm64, not just i386. This makes that a one-command check
# instead of an open question.
#
#   wasm_check.sh <recovered-src-dir> <native-obj-dir> <metoolkit-root>
set -uo pipefail
SRC="${1:?recovered .c dir}"; NATIVE="${2:?native .o dir}"; MT="${3:?metoolkit root}"
source ~/emsdk/emsdk_env.sh >/dev/null 2>&1 || { echo "no emsdk"; exit 1; }
NM=~/emsdk/upstream/bin/llvm-nm
INC="$MT/include"
OUT=$(mktemp -d); trap 'rm -rf "$OUT"' EXIT
IFLAGS="-I$(dirname "$0")/../include -I$INC"
for d in McdCommon McdPrimitives McdFrame MeGlobals MdtBcl MdtKea Mst MeApp; do
    IFLAGS="$IFLAGS -I$INC/$d"
done
ok=0; bad=0; same=0; drift=0
for c in "$SRC"/*.c; do
    b=$(basename "$c" .c)
    [ -f "$NATIVE/$b.o" ] || continue          # only what the native build accepted
    # No -m32: wasm32 is already 32-bit and emcc rejects the flag.
    if emcc -O2 -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion \
            -Wno-incompatible-pointer-types -DLINUX $IFLAGS \
            -c -o "$OUT/$b.o" "$c" 2>"$OUT/$b.err"; then
        ok=$((ok+1))
        a=$($NM --defined-only "$NATIVE/$b.o" | awk '$2 ~ /^[TDBRWV]$/{print $3}' | sort -u)
        w=$($NM --defined-only "$OUT/$b.o"    | awk '$2 ~ /^[TDBRWV]$/{print $3}' | sort -u)
        if [ "$a" = "$w" ]; then same=$((same+1)); else
            drift=$((drift+1)); echo "  [symbols] $b"; diff <(echo "$a") <(echo "$w") | head -4
        fi
    else
        bad=$((bad+1)); echo "  [compile] $b: $(grep -m1 error "$OUT/$b.err")"
    fi
done
echo
echo "  wasm32 compiled            : $ok"
echo "  failed to compile          : $bad"
echo "  exported symbols identical : $same"
echo "  exported symbols differ    : $drift"
[ "$bad" -eq 0 ] && [ "$drift" -eq 0 ]
