"""kd_paths — where things are, for the Python half of the toolchain.

The shell half is ``decomp/lib/kd-paths.sh`` and the two agree on every
variable name. Import it and call the resolvers; do not hardcode a path.

    from kd_paths import METOOLKIT_DIR, LAB_DIR, DUMP_DIR, OBJ_DIR, PROTOS
    from kd_paths import require_ut2004

⚠ ROOT is found by walking up for ``tools/recover.py``, not by counting ``..``.
Several tools used to resolve the SDK relative to their own depth and half of
them disagreed with the other half; that is only invisible until a file moves.
"""

import os

__all__ = [
    'ROOT', 'REPO', 'MD', 'MD_INC', 'MD_SRC',
    'METOOLKIT_DIR', 'MT_INC', 'MT_LIB', 'MT_LIB_SUBDIR',
    'AMD64_LIB', 'LAB_DIR', 'DUMP_DIR', 'OBJ_DIR', 'PROTOS',
    'OUT_SRC', 'OUT', 'BUILD', 'NDK_BIN', 'includes',
    'ut2004', 'require_ut2004', 'require_metoolkit', 'require_lab',
]


def _find_root():
    d = os.path.dirname(os.path.abspath(__file__))
    while d != '/' and not os.path.isfile(os.path.join(d, 'tools', 'recover.py')):
        d = os.path.dirname(d)
    if not os.path.isfile(os.path.join(d, 'tools', 'recover.py')):
        raise RuntimeError('kd_paths: cannot locate decomp/ above %s' % __file__)
    return d


ROOT = os.environ.get('KD_ROOT') or _find_root()
REPO = os.environ.get('KD_REPO') or os.path.dirname(ROOT)

# The PRODUCT: the recovered sources laid out as MathEngine's libraries, and the
# three headers every one of them includes (kd_compat.h / kd_karma.h /
# kd_types.h). They live with the product rather than with the toolchain because
# they ship with it — a consumer needs them, and nothing that consumes the
# library needs decomp/ at all. kd_types_fields.json sits beside kd_types.h
# because gen_typedb.py emits the two together and always has.
MD = os.environ.get('KD_MD') or os.path.join(REPO, 'metoolkit_decomp')
MD_INC = os.path.join(MD, 'include')
MD_SRC = os.path.join(MD, 'src')

# The Karma SDK. KD_METOOLKIT / METOOLKIT are the pre-2.44 spellings, still
# honoured because a lot of recorded commands use them.
METOOLKIT_DIR = (os.environ.get('METOOLKIT_DIR')
                 or os.environ.get('KD_METOOLKIT')
                 or os.environ.get('METOOLKIT')
                 or os.path.join(REPO, 'metoolkit'))

# gcc3.2 is the only configuration in the drop built with -g3, so it is the one
# carrying the DWARF the recovery reads, and lab/allobj/*.o are byte-identical
# members of it. Changing this changes what "byte-identical" means.
MT_LIB_SUBDIR = os.environ.get('KD_MT_LIB_SUBDIR', 'lib.rel/linux_single_gcc3.2')
MT_INC = os.path.join(METOOLKIT_DIR, 'include')
MT_LIB = os.path.join(METOOLKIT_DIR, MT_LIB_SUBDIR)

# The 64-bit build of the same sources — the LP64 oracle. It is LLP64, not LP64;
# see proven.txt LP64-BAKED-SIZES.
AMD64_LIB = os.environ.get(
    'KD_AMD64_LIB', os.path.join(METOOLKIT_DIR, 'lib.rel', 'win_amd64_single'))

# The Ghidra working set. DUMP_DIR and PROTOS are a PAIR — mixing one with the
# other generation's partner gets McdSpace wrong in opposite directions.
LAB_DIR = os.environ.get('KD_LAB_DIR', os.path.join(REPO, 'lab'))
DUMP_DIR = os.environ.get('KD_DUMP_DIR', os.path.join(LAB_DIR, 'out14'))
OBJ_DIR = os.environ.get('KD_OBJ_DIR', os.path.join(LAB_DIR, 'allobj'))
PROTOS = os.environ.get('KD_PROTOS', os.path.join(LAB_DIR, 'kd_protos11.h'))

# Scratch. /tmp IS VOLATILE.
#
# ⚠ There is no OUT default, and that is not an omission. KD_OUT means "the tree
# under test" and its default legitimately differs by harness — /tmp/kd_lp64 for
# the ones that exist to exercise the post-passes, /tmp/kd_out for the ones that
# read the raw recovery. Defaulting it in one place would silently re-point four
# harnesses at a tree they were never measuring, and every one would still pass,
# because the post-passes are no-ops at i386 by construction.
OUT_SRC = os.environ.get('KD_OUT_SRC', '/tmp/kd_out')
OUT = os.environ.get('KD_OUT') or None
BUILD = os.environ.get('KD_BUILD', '/tmp/kd_build')


def _ndk_bin():
    """The Android NDK's llvm bin/, for the aarch64 pointer-width checks.

    A real external toolchain, so there is no in-repo default. Discovered from
    the standard NDK variables rather than pinned to one version, because the
    old pinned path (r30 beta) silently stopped existing on an NDK upgrade and
    the failure looked like a compiler bug.
    """
    explicit = os.environ.get('KD_NDK')
    if explicit:
        return explicit
    for var in ('ANDROID_NDK_HOME', 'ANDROID_NDK_ROOT', 'NDK_HOME'):
        root = os.environ.get(var)
        if root:
            cand = os.path.join(root, 'toolchains', 'llvm', 'prebuilt',
                                'linux-x86_64', 'bin')
            if os.path.isdir(cand):
                return cand
    sdk = os.environ.get('ANDROID_HOME') or os.path.expanduser('~/Android/Sdk')
    ndks = os.path.join(sdk, 'ndk')
    if os.path.isdir(ndks):
        for ver in sorted(os.listdir(ndks), reverse=True):
            cand = os.path.join(ndks, ver, 'toolchains', 'llvm', 'prebuilt',
                                'linux-x86_64', 'bin')
            if os.path.isdir(cand):
                return cand
    return ''


NDK_BIN = _ndk_bin()


def includes(inc=None):
    """The -I list. Karma's public headers are split per library and several
    include a sibling by bare name, so all nine directories have to be on the
    path or the failure is a missing type three headers deep."""
    inc = inc or MT_INC
    return ['-I' + inc] + [
        '-I' + os.path.join(inc, d) for d in
        ('MdtBcl', 'MdtKea', 'McdCommon', 'McdPrimitives', 'McdFrame',
         'Mst', 'MeGlobals', 'MeApp')
    ]


# --- UT2004 -----------------------------------------------------------------
# NO DEFAULTS ON PURPOSE. This repository does not own a game install, a build
# tree or an engine checkout, and quietly guessing one is how a tool ends up
# measuring the wrong binary.

_UT_VARS = {
    'engine': ('UT2004_ENGINE_DIR', 'an engine-ut2004 checkout'),
    'build': ('UT2004_BUILD_DIR', 'a configured build tree (e.g. .../build-native-karma)'),
    'assets': ('UT2004_ASSETS_DIR', 'the game data (Maps/, Textures/, ...)'),
    'run': ('UT2004_RUN_DIR', 'a run tree with System/ and a drop-in metoolkit'),
}


def ut2004(which):
    """Return the UT2004 path named by `which`, or '' if unset."""
    var = _UT_VARS[which][0]
    val = os.environ.get(var, '')
    if not val and which == 'run':
        val = os.environ.get('KD_RUNTIME', '')      # pre-2.44 spelling
    return val


def require_ut2004(*which):
    """Exit with a usable message unless every named UT2004 path is set and real."""
    missing = []
    for w in which:
        var, what = _UT_VARS[w]
        val = ut2004(w)
        if not val:
            missing.append('  %s is not set (%s).' % (var, what))
        elif not os.path.exists(val):
            missing.append('  %s=%s does not exist.' % (var, val))
    if missing:
        raise SystemExit(
            '\n'.join(missing) + '\n\n'
            '  This tool needs UT2004, which does not live in this repository.\n'
            '  decomp/test/ut2004/README.md explains what each one wants and why.\n')
    return [ut2004(w) for w in which]


def require_metoolkit():
    if os.path.isdir(MT_INC) and os.path.isdir(MT_LIB):
        return
    raise SystemExit(
        '  no Karma SDK at %s (%s missing).\n'
        '  It ships in this repository; set METOOLKIT_DIR to override.\n'
        % (METOOLKIT_DIR, MT_LIB_SUBDIR))


def require_lab():
    if (os.path.isdir(DUMP_DIR) and os.path.isdir(OBJ_DIR)
            and os.path.isfile(PROTOS)):
        return
    raise SystemExit(
        '  incomplete Ghidra lab at %s.\n'
        '  want: %s, %s, %s\n'
        '  ⚠ the dump and the protos header are a PAIR — see lab/README.md.\n'
        % (LAB_DIR, DUMP_DIR, OBJ_DIR, PROTOS))
