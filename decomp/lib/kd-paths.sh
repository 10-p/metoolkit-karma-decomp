#!/usr/bin/env bash
# kd-paths.sh — the ONE place any harness learns where things are.
#
#   . "$(dirname "$0")/../lib/kd-paths.sh"      # depth does not matter; see below
#
# Source it, never execute it. Everything it sets is overridable from the
# environment, and every default resolves inside this repository — that is what
# makes the standalone tier standalone.
#
# ⚠ KD_ROOT IS FOUND BY WALKING UP FOR A MARKER, NOT BY COUNTING `..`.
# The old layout had harnesses in one directory and half of them computed the
# metoolkit path as `$HERE/../Thirdparty` and half as `$HERE/../../Thirdparty`,
# because `HERE` meant the decomp root in some scripts and the test directory in
# others. Both were right then and both break the moment a file moves. Walking
# up to `tools/recover.py` cannot be wrong at any depth.

# ---------------------------------------------------------------------------
# KD_ROOT (this repo's decomp/) and KD_REPO (the repository root)
# ---------------------------------------------------------------------------
if [ -z "${KD_ROOT:-}" ]; then
    _kd_d="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    while [ "$_kd_d" != "/" ] && [ ! -f "$_kd_d/tools/recover.py" ]; do
        _kd_d="$(dirname "$_kd_d")"
    done
    if [ ! -f "$_kd_d/tools/recover.py" ]; then
        echo "kd-paths: cannot locate decomp/ (no tools/recover.py above ${BASH_SOURCE[0]})" >&2
        return 1 2>/dev/null || exit 2
    fi
    KD_ROOT="$_kd_d"
    unset _kd_d
fi
KD_REPO="${KD_REPO:-$(cd "$KD_ROOT/.." && pwd)}"
export KD_ROOT KD_REPO

# The two harness tiers. A harness that calls a sibling names it through one of
# these rather than through its own directory, so a cross-tier call (try_subst
# in the ut2004 tier reaching make_substituted_metoolkit in the standalone one)
# stays correct.
KD_T_STD="$KD_ROOT/test/standalone"
KD_T_UT="$KD_ROOT/test/ut2004"
export KD_T_STD KD_T_UT

# The PRODUCT: the recovered sources laid out as MathEngine's libraries, and the
# three headers every one of them includes (kd_compat.h / kd_karma.h /
# kd_types.h). These live with the product rather than with the toolchain
# because they ship with it — a consumer needs them, and nothing that consumes
# the library needs decomp/ at all.
KD_MD="${KD_MD:-$KD_REPO/metoolkit_decomp}"
KD_MD_INC="$KD_MD/include"
KD_MD_SRC="$KD_MD/src"
export KD_MD KD_MD_INC KD_MD_SRC

# ---------------------------------------------------------------------------
# The Karma SDK. KD_METOOLKIT / METOOLKIT are the pre-2.44 spellings and are
# still honoured, because they are in a lot of recorded commands.
# ---------------------------------------------------------------------------
METOOLKIT_DIR="${METOOLKIT_DIR:-${KD_METOOLKIT:-${METOOLKIT:-$KD_REPO/metoolkit}}}"

# The i386 archives everything is measured against. gcc3.2 is not an arbitrary
# pick: it is the only configuration in the drop built with -g3, so it is the
# one carrying the DWARF the recovery reads, and lab/allobj/*.o are byte-
# identical members of it. Changing this changes what "byte-identical" means.
KD_MT_LIB_SUBDIR="${KD_MT_LIB_SUBDIR:-lib.rel/linux_single_gcc3.2}"

KD_MT_INC="$METOOLKIT_DIR/include"
KD_MT_LIB="$METOOLKIT_DIR/$KD_MT_LIB_SUBDIR"
export METOOLKIT_DIR KD_MT_LIB_SUBDIR KD_MT_INC KD_MT_LIB

# The include list, spelled once. Karma's public headers are split per library
# and several of them include a sibling by bare name, so all nine directories
# have to be on the path or the failure is a missing type three headers deep.
KD_INCS="-I$KD_MT_INC -I$KD_MT_INC/MdtBcl -I$KD_MT_INC/MdtKea -I$KD_MT_INC/McdCommon"
KD_INCS="$KD_INCS -I$KD_MT_INC/McdPrimitives -I$KD_MT_INC/McdFrame -I$KD_MT_INC/Mst"
KD_INCS="$KD_INCS -I$KD_MT_INC/MeGlobals -I$KD_MT_INC/MeApp"
export KD_INCS

# ---------------------------------------------------------------------------
# The Ghidra working set
# ---------------------------------------------------------------------------
KD_LAB_DIR="${KD_LAB_DIR:-$KD_REPO/lab}"
KD_DUMP_DIR="${KD_DUMP_DIR:-$KD_LAB_DIR/out14}"     # ⚠ pairs with KD_PROTOS
KD_OBJ_DIR="${KD_OBJ_DIR:-$KD_LAB_DIR/allobj}"
KD_PROTOS="${KD_PROTOS:-$KD_LAB_DIR/kd_protos11.h}" # ⚠ pairs with KD_DUMP_DIR
export KD_LAB_DIR KD_DUMP_DIR KD_OBJ_DIR KD_PROTOS

# ---------------------------------------------------------------------------
# Scratch. /tmp IS VOLATILE — a crash wipes it. Copy out anything you want.
#
# ⚠ KD_OUT IS DELIBERATELY NOT DEFAULTED HERE, and that is not an omission.
# It means "the tree under test", and its default legitimately DIFFERS by
# harness: lp64_collide.sh and lp64_pipeline.sh default it to /tmp/kd_lp64 (the
# post-passed tree, which is what they exist to exercise), while ab_contact.sh,
# bisect_static.sh, lp64_run.sh and trace_cylcyl.sh default it to /tmp/kd_out
# (the raw recovery). lp64_pipeline.sh then passes KD_OUT="$DST" into
# lp64_run.sh to point it at the post-passed tree on purpose.
#
# Exporting one default here would silently re-point four harnesses at a tree
# they were never measuring, and every one of them would still pass — the
# post-passes are no-ops at i386 by construction, so the number would not move
# and the change would be invisible. Each script keeps its own `${KD_OUT:-...}`.
# ---------------------------------------------------------------------------
KD_OUT_SRC="${KD_OUT_SRC:-/tmp/kd_out}"    # raw recover.py output
KD_BUILD="${KD_BUILD:-/tmp/kd_build}"      # the i386 .o baseline
export KD_OUT_SRC KD_BUILD

# ---------------------------------------------------------------------------
# The Android NDK's llvm bin/, for the aarch64 pointer-width checks. A real
# external toolchain, so there is no in-repo default — discovered from the
# standard NDK variables rather than pinned to one version. The old pinned r30
# path stopped existing on an NDK upgrade and the failure read like a compiler
# bug rather than a missing directory. Empty is a legitimate answer; the one
# harness that wants it SKIPS rather than fails.
# ---------------------------------------------------------------------------
if [ -z "${KD_NDK:-}" ]; then
    for _kd_r in "${ANDROID_NDK_HOME:-}" "${ANDROID_NDK_ROOT:-}" "${NDK_HOME:-}"; do
        if [ -n "$_kd_r" ] && [ -d "$_kd_r/toolchains/llvm/prebuilt/linux-x86_64/bin" ]; then
            KD_NDK="$_kd_r/toolchains/llvm/prebuilt/linux-x86_64/bin"; break
        fi
    done
fi
if [ -z "${KD_NDK:-}" ]; then
    _kd_sdk="${ANDROID_HOME:-$HOME/Android/Sdk}"
    if [ -d "$_kd_sdk/ndk" ]; then
        for _kd_v in $(ls -1r "$_kd_sdk/ndk" 2>/dev/null); do
            if [ -d "$_kd_sdk/ndk/$_kd_v/toolchains/llvm/prebuilt/linux-x86_64/bin" ]; then
                KD_NDK="$_kd_sdk/ndk/$_kd_v/toolchains/llvm/prebuilt/linux-x86_64/bin"; break
            fi
        done
    fi
fi
KD_NDK="${KD_NDK:-}"
unset _kd_r _kd_sdk _kd_v
export KD_NDK

# ---------------------------------------------------------------------------
# UT2004. NO DEFAULTS ON PURPOSE — this repository does not own a game install,
# a build tree or an engine checkout, and quietly guessing one is how a harness
# ends up measuring the wrong binary. kd_require_ut2004 fails loudly instead.
# KD_RUNTIME is the pre-2.44 spelling of UT2004_RUN_DIR.
# ---------------------------------------------------------------------------
UT2004_RUN_DIR="${UT2004_RUN_DIR:-${KD_RUNTIME:-}}"
export UT2004_ENGINE_DIR UT2004_BUILD_DIR UT2004_ASSETS_DIR UT2004_RUN_DIR

# kd_require_ut2004 VAR [VAR...] — assert each is set and points somewhere real.
kd_require_ut2004() {
    local v p missing=0
    for v in "$@"; do
        eval "p=\${$v:-}"
        if [ -z "$p" ]; then
            echo "  $v is not set." >&2; missing=1
        elif [ ! -e "$p" ]; then
            echo "  $v=$p does not exist." >&2; missing=1
        fi
    done
    if [ "$missing" -ne 0 ]; then
        cat >&2 <<'EOT'

  This harness needs UT2004, which does not live in this repository. Point it at
  your own copy:

    UT2004_ENGINE_DIR   an engine-ut2004 checkout
    UT2004_BUILD_DIR    a configured build tree (e.g. .../build-native-karma)
    UT2004_ASSETS_DIR   the game data (Maps/, Textures/, ...)
    UT2004_RUN_DIR      a run tree with System/ and a drop-in metoolkit

  decomp/test/ut2004/README.md explains what each harness wants and why.
EOT
        return 2
    fi
    return 0
}

# kd_require_metoolkit — the standalone tier's one precondition.
kd_require_metoolkit() {
    [ -d "$KD_MT_INC" ] && [ -d "$KD_MT_LIB" ] && return 0
    echo "  no Karma SDK at $METOOLKIT_DIR ($KD_MT_LIB_SUBDIR missing)." >&2
    echo "  It ships in this repository; set METOOLKIT_DIR to override." >&2
    return 2
}

# kd_require_lab — the pipeline's one precondition.
kd_require_lab() {
    [ -d "$KD_DUMP_DIR" ] && [ -d "$KD_OBJ_DIR" ] && [ -f "$KD_PROTOS" ] && return 0
    echo "  incomplete Ghidra lab at $KD_LAB_DIR." >&2
    echo "  want: $KD_DUMP_DIR, $KD_OBJ_DIR, $KD_PROTOS" >&2
    echo "  ⚠ the dump and the protos header are a PAIR — see lab/README.md." >&2
    return 2
}
