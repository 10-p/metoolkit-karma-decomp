#!/usr/bin/env bash
# ab_integrate.sh — drive test/ab_integrate.c against a recovered keaIntegrate_pc.
#
# Usage: ab_integrate.sh <recovered keaIntegrate_pc.c> [iterations] [fastspin]
#
# Links the SHIPPED MdtKeaIntegrateSystem and the recovered one into one binary
# by renaming the recovered symbols, then reports divergent bodies per field.
# `fastspin` set to 1 sets MdtKeaBodyFlagUseFastSpin on every body, which is the
# only way to reach the MeQuaternionFiniteRotation branch — the scenes never do.
set -eu
HERE=$(cd "$(dirname "$0")" && pwd)
KD=$(dirname "$HERE")
MT=${METOOLKIT:-$KD/../Thirdparty/metoolkit}
INC=$MT/include
SRC=${1:?recovered keaIntegrate_pc.c}
IT=${2:-100000}
FS=${3:-0}
W=$(mktemp -d); trap 'rm -rf "$W"' EXIT

IF="-I$KD/include -I$INC -I$INC/McdCommon -I$INC/McdPrimitives -I$INC/McdFrame"
IF="$IF -I$INC/MeGlobals -I$INC/MdtBcl -I$INC/MdtKea -I$INC/Mst -I$INC/MeApp"

gcc -m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion \
    -Wno-incompatible-pointer-types -DLINUX $IF -c -o "$W/rec.o" "$SRC"

VAN=_Z26KeaIntegrateSystem_vanillaPKP10MdtKeaBodyP20MdtKeaTransformationi16MdtKeaParameters
REC=_Z30recKeaIntegrateSystem_vanillaPKP10MdtKeaBodyP20MdtKeaTransformationi16MdtKeaParameters
objcopy --redefine-sym MdtKeaIntegrateSystem=rec_MdtKeaIntegrateSystem \
        --redefine-sym "$VAN=$REC" "$W/rec.o" "$W/rec_r.o"

gcc -m32 -O2 -DLINUX -no-pie $IF -o "$W/ab" "$HERE/ab_integrate.c" "$W/rec_r.o" \
    -Wl,--start-group "$MT"/lib.rel/linux_single_gcc3.2/*.a -Wl,--end-group -lstdc++ -lm
"$W/ab" "$IT" "$FS" quiet
