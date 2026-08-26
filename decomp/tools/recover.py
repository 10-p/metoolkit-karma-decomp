#!/usr/bin/env python3
"""
recover.py — run the whole recovery recipe over many objects and report status.

For each object: generate the prelude, clean the Ghidra dump into C, compile it,
and classify the outcome. The point is to measure how much of Karma comes back
with no human intervention, so the remaining manual work can be sized honestly.

  ./recover.py --dump-dir <ghidra-out> --obj-dir <extracted .o> --out-dir src \\
               --metoolkit ../Thirdparty/metoolkit [--only libMdt]

Outcomes:
  OK        compiled, and the generated prelude had no TODOs
  TODO      compiled, but the prelude has TODOs a human must fill in
            (usually a .bss static whose value comes from a C++ static ctor)
  FAIL      did not compile — first error is reported

Nothing here edits a prelude that already exists on disk; hand-completed
preludes are preserved and reused.
"""
import argparse
import glob
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import check_frame_bounds

# gcc-3.2 C++ static-constructor scaffolding. It only ever runs file-scope
# initialisers, which the prelude expresses as plain C initialisers instead.
DROP_ALWAYS = re.compile(r'^(__static_initialization_and_destruction_\d+|_GLOBAL__[ID]_)')


def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, **kw)


def tool_error(stderr, limit=110):
    """The useful line of a failing tool's stderr.

    The first 90 characters of a Python traceback are "Traceback (most recent
    call last):" and the start of a file path — every crash in the pipeline
    therefore reported the same string and none of them said what went wrong.
    keaMemory, one of the five libMdtKea objects blocking the solver, sat in the
    FAIL column behind exactly that message. For a traceback the last line is
    the exception; for a compiler it is the first line mentioning an error."""
    lines = [l for l in stderr.strip().splitlines() if l.strip()]
    if not lines:
        return '(no stderr)'
    if lines[0].startswith('Traceback'):
        # Last line is the exception; the line before it is where it was raised.
        where = ''
        for l in reversed(lines[:-1]):
            m = re.search(r'File "([^"]+)", line (\d+)', l)
            if m:
                where = ' at %s:%s' % (os.path.basename(m.group(1)), m.group(2))
                break
        return (lines[-1] + where)[:limit]
    err = next((l for l in lines if ' error' in l or 'Error' in l), lines[0])
    return err[:limit]


def dump_functions(path):
    return re.findall(r'/\* ==== (\S+) ==== \*/', open(path, errors='ignore').read())


# Objects held out of the build by a MEASUREMENT rather than by a pattern.
#
# Every other detector below recognises a SHAPE in the recovered source — a
# guessed stack frame, an out-of-range frame reference, a value Ghidra could not
# account for. That works because those shapes are what usually precedes a
# defect. It cannot work when an object is clean by every shape and simply
# computes the wrong answer, and HANDOVER.md §11 item 0 has been recording that
# hole for several sessions: "the quarantine can only hold what a detector
# recognises; an object that is simply never measured, and is wrong, walks
# straight in."
#
# So this is the hole, closed. An entry here is a claim about evidence and must
# cite the line in proven.txt that carries it. Removing one is a release and
# needs the same standard as any other.
#
# NOT for a small, understood divergence: IxCylinderCylinder is measurably
# imperfect and §11 item 0 says explicitly to leave it in the build, because
# what it gets wrong is a field the engine does not read for that pair. This is
# for an object whose output is wrong in a way that would make any build
# containing it useless.
#
# EMPTY, and the entry it used to hold is worth keeping in view. keaLCPSolver
# was put here on the strength of `first@1 = 3.772e+00` on the ragdoll — a body
# teleporting 3.7 m on step 1 — and came out the same day, because the
# measurement that held it also located the defect: three float stores Ghidra
# rendered as int CONVERSIONS, in PrincipalSubmatrix, which the compiler had no
# reason to complain about. See fix_int_store_of_float and proven.txt. It is now
# bit-identical on all three scenes and on every one of its fifteen functions
# individually (test/bisect_object.sh).
MEASURED_WRONG = {}


# The other side of the same coin: a detector that fires on a shape which, for
# THIS object, has been measured to be inert.
#
# `live_unmodelled` holds an object that READS a value Ghidra could not account
# for. That is the right default — reading one is reading whatever happened to
# be there. But two objects now reach the end of the evidence chain (exact on
# all three substitute scenes, and per-function under test/bisect_object.sh)
# while still tripping it, and in both cases the complaint has been traced to a
# store that goes nowhere. Holding a PROVEN object back on a shape is the same
# error as letting an UNPROVEN one through on one.
#
# An entry names the exact identifiers released, not the object, so a NEW
# unmodelled read in the same object still quarantines it. Each must cite the
# paragraph in proven.txt that carries the measurement.
#
# This is a release and needs the standard of any other: a measurement, plus an
# account of why the detector's complaint is true and inert. "It compiles and
# the scenes look fine" is not that.
RELEASED_UNMODELLED = {
    # proven.txt, "keaLCP_new — THE LAST MODULE". Both stores land in the two
    # alignment-padding words the shipped `commonPivot` call pushes before its
    # six arguments (`push %edx; push %edx` at 4ec/4ed): beyond the arity the
    # callee reads, and slot 32 is never read anywhere in the function.
    'keaLCP_new': ['extraout_EDX'],
    # proven.txt, "keaIntegrate_pc — the dropped rounding". The name is the
    # OUTGOING by-value argument area of the tail call to
    # KeaIntegrateSystem_vanilla: `MdtKeaParameters in_stack_ffffffa0` is
    # declared, 76 bytes, and the copy loop fills all 19 MeReal of it before it
    # is read, so the dataflow is complete and only the SPELLING is unmodelled.
    # fix_stack_address_name's size check already refuses the shape when the
    # declaration is too small (HANDOVER.md §11 item 2); here it fits exactly.
    'keaIntegrate_pc': ['in_stack_ffffffa0'],
}


# A declaration of a value Ghidra could not account for, with its type in front:
#   `ushort *extraout_EDX;`  `undefined4 extraout_ECX;`  `McdContact *extraout_EAX;`
# The name must END the declarator, so `MeReal (*unaff_ESI) [3];` does not match
# and the object stays in review — which is the right answer for a shape whose
# type this cannot rebuild.
UNMODELLED_DECL = r'(?m)^([ \t]*)((?:const |struct |unsigned |signed )*[A-Za-z_]\w*[ \t*]+)%s[ \t]*;[ \t]*$'

# Three constants, not one. Two arbitrary values can coincide — `if (v)` folds
# the same way for 1 and 3 — so the set includes 0, which folds the OTHER way
# for every predicate, and -1, which is all-bits-set.
INERT_PROBES = ('0', '-1', '0x5a5a5a5a')


def initialise_unmodelled(src, names):
    """Give each named value a defined one, once it is proven not to matter."""
    for name in names:
        src = re.sub(UNMODELLED_DECL % re.escape(name),
                     lambda m: f'{m.group(1)}{m.group(2)}{name} = '
                               f'({m.group(2).strip()})0;', src)
    return src


def prove_inert(csrc, src, names, cflags, build_dir, base):
    """Does the emitted code depend on the value? Ask the compiler.

    `live_unmodelled` reports that the recovery READS a value Ghidra could not
    account for. That is the right thing to report and the wrong question to
    stop on, because Ghidra also invents such a read where the machine simply
    pushed a live register as stack padding, or where it merged a dead
    definition into a local that is reassigned before anything looks at it.
    `MeXMLParser` is twelve symbols of the drop-in gap held on

        puVar6 = extraout_EDX;                  <- the value
        ...  __strtod_internal(c,&d);           <- the only consumer, now gone
        puVar6 = (ushort *)(int)*pcVar7;        <- overwritten before any read

    and a regex cannot tell that from a value that matters: it needs to know
    which definition reaches which read, on every path.

    THE COMPILER ALREADY KNOWS. Give the value a defined constant, compile, and
    do it again with a different constant. If the two objects are BYTE-IDENTICAL
    the emitted code does not depend on the value, so nothing the value could
    have been would change what the object does — which is exactly the property
    `live_unmodelled` is trying to decide, decided by GCC's own dataflow instead
    of by a pattern.

    THREE probes rather than two, because two arbitrary constants can coincide.
    `if (v)` compiles the same for 1 and for 3; it does not for 0. The set is
    {0, -1, 0x5a5a5a5a}: a value that is false, a value that is all-bits-set and
    a value that is neither.

    THIS IS A RELEASE, so it is worth being clear about what it does NOT prove.
    It proves the value is inert *in this object*, not that the object is
    correct: `MeAssetDBXMLIO` passed every gate here and still killed engine
    init, because a detector had been shielding an unrelated defect
    (HANDOVER.md §3c). Anything on the `.ka` path still has to be run in the
    engine before it counts.

    Returns True when every name is inert, or the list of names that are not.
    """
    probe_src, missing = src, []
    for name in names:
        if not re.search(UNMODELLED_DECL % re.escape(name), src):
            missing.append(name)                  # no declaration this can type
    if missing:
        return missing
    objs, tmp_c = [], os.path.join(build_dir, base + '.inert.c')
    for k, probe in enumerate(INERT_PROBES):
        text = src
        for name in names:
            text = re.sub(UNMODELLED_DECL % re.escape(name),
                          lambda m: f'{m.group(1)}{m.group(2)}{name} = '
                                    f'({m.group(2).strip()})({probe});', text)
        open(tmp_c, 'w').write(text)
        tmp_o = os.path.join(build_dir, f'{base}.inert{k}.o')
        run(['gcc'] + cflags + ['-c', '-o', tmp_o, tmp_c])
        if not os.path.exists(tmp_o):
            for p in objs + [tmp_c]:
                if os.path.exists(p):
                    os.unlink(p)
            return names                          # a probe that will not build
        objs.append(tmp_o)
    same = all(open(objs[0], 'rb').read() == open(o, 'rb').read() for o in objs[1:])
    for p in objs + [tmp_c]:
        os.unlink(p)
    return True if same else names


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--dump-dir', required=True, help='Ghidra .c dumps')
    ap.add_argument('--obj-dir', required=True, help='root of extracted .o files')
    ap.add_argument('--out-dir', required=True, help='where recovered .c/.prelude.h go')
    ap.add_argument('--metoolkit', required=True, help='metoolkit root (include/ + lib.*)')
    ap.add_argument('--only', help='substring filter on the archive directory name')
    ap.add_argument('--build-dir', default='/tmp/kd_build')
    ap.add_argument('--protos', help='kd_protos.h — gives C++-mangled imports real signatures')
    args = ap.parse_args()

    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.dirname(here)
    inc = os.path.join(args.metoolkit, 'include')
    iflags = ['-I' + os.path.join(root, 'include'), '-I' + inc] + [
        '-I' + os.path.join(inc, d) for d in
        ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
         'MdtBcl', 'MdtKea', 'Mst', 'MeApp')]
    # Decompiled code has an EXACT ABI but only approximate types: Ghidra
    # recovers the type a value is used as, which is often not the type the
    # public header names for it. Same layout, same calling convention,
    # different spelling. These two diagnostics are about spelling, so they
    # are downgraded here rather than fought one cast at a time. Both are
    # safe only because every target is 32-bit-pointer (i386 and wasm32).
    # -fno-strict-aliasing is REQUIRED, not a nicety. Decompiled code
    # type-puns constantly: Ghidra recovers a stack slot as `MeReal x[2]`
    # and the code stores a pointer through it. Under -O2 strict aliasing
    # GCC is entitled to assume those accesses cannot alias, and it deletes
    # the stores — which is exactly what happened to IxSphereTriList.
    # KTriListGenerator received (pair, 0, 0, 0, 0): the first argument
    # survived because it went through a register, and the four passed via
    # punned stack slots were optimised away. The engine's own build already
    # uses this flag for the same reason (root CMakeLists.txt).
    cflags = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing',
              '-std=gnu99', '-w', '-Wno-int-conversion',
              '-Wno-incompatible-pointer-types', '-DLINUX'] + iflags
    os.makedirs(args.build_dir, exist_ok=True)

    objs = sorted(glob.glob(os.path.join(args.obj_dir, '**', '*.o'), recursive=True))
    if args.only:
        objs = [o for o in objs if args.only in o]

    # Ghidra sometimes resolves a relocation-with-addend against a DATA symbol to
    # the wrong name, and emits an indirect call through it:
    #     (*_McdGeometryDeinit)(0x1c, 0x10)      <- actually an allocator call
    # The call target is simply wrong. That is a semantic defect, not a missing
    # declaration, so it must not be papered over — flag it for a human.
    MISLABELLED_CALL = re.compile(r"error: ['\u2018]_(\w+)['\u2019] undeclared")

    # Ghidra sometimes fails to model a function's stack frame and invents a
    # local array, then routes CALL ARGUMENTS through it at computed offsets:
    #
    #   *(MeDict **)((int)aiStack_50 + iVar8 + iVar16 + 4) = dict;
    #   pMVar9 = MeDictFirst(*(void **)((int)aiStack_50 + iVar8 + iVar16 + 4));
    #
    # The offsets are wrong, so the callee receives garbage. This COMPILES and
    # LINKS with a symbol set identical to the original — MdtPartition did, then
    # segfaulted in MeDictNext(dict, NULL). Silently-wrong code is the worst
    # outcome available, so detect the shape and refuse to call it recovered.
    #
    # The premise is "Ghidra INVENTED the array", i.e. the storage is not there.
    # ghidra_clean.materialise_shifted_frame falsifies that premise for the
    # bases it repairs: it redeclares the base as a pointer into an alloca'd
    # block sized from <object>.locals and big enough by construction, so the
    # same text is then ordinary address arithmetic. It marks each base it
    # rewrote, and those are excluded below — otherwise the detector quarantines
    # exactly the objects the repair fixed (keaLCP_new, 20 sites, measured
    # bit-identical on all three scenes). The exclusion is scoped to the
    # FUNCTION that carries the marker, so an unrepaired base of the same name
    # in a sibling function still fires.
    GHIDRA_STACK_GUESS = re.compile(
        r'\(int\)(a[a-z]Stack_[0-9a-f]+)\s*\+\s*[A-Za-z_]\w*')
    KD_MATERIALISED_BASE = re.compile(r'KD_MATERIALISED_BASE\((\w+)\)')

    def stack_guesses(src):
        """Matches of the invented-frame shape, minus the materialised bases."""
        out = []
        for region in re.split(r'^/\* ---- ', src, flags=re.M):
            ok = set(KD_MATERIALISED_BASE.findall(region))
            out += [m for m in GHIDRA_STACK_GUESS.findall(region) if m not in ok]
        return out

    # `stack0xNNNN` marks a variable-length stack allocation Ghidra could not
    # model. ghidra_clean.py materialises a buffer so it COMPILES, but that is
    # not the same as being correct: the allocation SIZE is read from the same
    # mis-modelled frame, so it can be garbage.
    #
    # IxSphereTriList proved the point. It compiled, passed the substitute gate
    # (the scenes never exercise Sphere x TriangleList), and then segfaulted on
    # its FIRST call in a real match, passing a wild triangle-buffer pointer
    # into the engine's own KTriListGenerator callback.
    #
    # So materialising is for readability and compilation, never a certificate.
    # Any function needing it is REVIEW.
    GHIDRA_ALLOCA_FRAME = re.compile(r'\bkd_frame_top_\b')

    # An indirect call emitted with NO arguments: `count = (*pcVar11)();`
    #
    # Ghidra has no signature for a call through a function pointer, so it drops
    # every argument. The callee then reads whatever happens to be on the stack.
    # Prototypes cannot fix this — kd_protos.h resolves calls by NAME, and this
    # call has none.
    #
    # This is what actually crashed IxSphereTriList: Karma calls the engine's own
    # KTriListGenerator through McdTriangleListFnPtr, got garbage for `pos`, and
    # died in KME2UPosition. It compiled, and it passed the substitute gate,
    # because the scripted scenes never reach Sphere x TriangleList.
    GHIDRA_ARGLESS_INDIRECT = re.compile(r'\(\*\w+\)\(\)')

    # `kd_argslot_` marks a frame this pipeline RECONSTRUCTED by inference:
    # Ghidra lost the stack pointer after a variable-length allocation, and the
    # outgoing-argument slots were collapsed back onto their base locals.
    #
    # That reconstruction can be textually perfect and still wrong. IxSphereTriList
    # is the proof: with a Ghidra call-site signature override its
    # McdTriangleListFnPtr call regained all five arguments, every store pairs
    # with its load, and it STILL segfaults in a live match. Something about the
    # frame is off in a way reading the C does not reveal.
    #
    # So a reconstructed frame is a hypothesis, not a result. It stays out of the
    # validated set until a real match proves it.
    GHIDRA_RECONSTRUCTED_FRAME = re.compile(r'\bkd_argslot_\w+\b')

    # `__regparmN` means Ghidra decided the first N arguments arrive in
    # registers, and then LAID OUT THE PARAMETER LIST TO MATCH. The annotation
    # itself is a no-op — GCC's i386 C++ ABI is plain cdecl here — but the body
    # Ghidra generated under the assumption is shifted by N and one incoming
    # argument falls off the end:
    #
    #   MeI16 __regparm1 McdGeometryGetMassProperties(g, relTM, m, volume)
    #   { return (**vtable)(relTM, m, volume); }        <- should be (g, relTM, m, volume)
    #
    # The original pushes four arguments; the recovery passes three, with every
    # one of them shifted. This compiled, it linked, it passed the collision-free
    # scene, and it segfaulted in McdGeometryGetMassProperties on the FIRST
    # collision scene — which is how it was found.
    #
    # Every instance examined is shifted the same way (McdConvexMeshCreate reads
    # `fatness` as a pointer; McdGeometryInstanceDestroy and MePoolMallocGetStruct
    # never touch their declared parameters at all and read `in_stack_*` instead),
    # so this is unconditional rather than a heuristic.
    GHIDRA_REGPARM = re.compile(r'__regparm[1-9]')

    # Ghidra's names for a value it could not account for: an incoming stack
    # argument outside the frame it modelled, a register left live by a call it
    # did not model, a callee-saved register it lost track of. Reading one is
    # reading whatever happened to be there.
    #
    # Not all of them are live. gcc 3.2 sets up argument slots that a later
    # optimisation pass abandoned, and Ghidra faithfully recovers the dead store:
    # McdAggregate's `uVar7 = extraout_ECX;` assigns to a variable nothing ever
    # reads. Those are noise. MdtConstraint's `unaff_ESI` is assigned before its
    # use, so the name is misleading but the dataflow is complete. Only a read
    # that reaches something is a defect, so that is what this looks for.
    GHIDRA_UNMODELLED = re.compile(
        r'\b(in_stack_[0-9a-f]+|extraout_[A-Z]+[0-9]*|unaff_[A-Z]+[0-9]*)\b')

    # `x.f = x.f;` — a store Ghidra reordered past the thing it was saving from.
    #
    # An optimising compiler does not emit a self-assignment and a programmer
    # does not write one, so when the decompiler produces one it is describing
    # something else. In every case examined it is a SAVE AND RESTORE around an
    # aggregate overwrite, with the read folded into the write and thereby moved
    # to after the stores it was supposed to precede.
    #
    # ghidra_clean.restore_saved_element() repairs the form this pipeline can
    # prove — a variable index into an array whose constant indices are stored
    # just above, which is exactly the aliasing Ghidra cannot see through, and
    # which is what McdSphylBoxIntersect does. This detector is the backstop for
    # any other shape: the value is still being dropped, it still compiles and
    # runs, and it is still silently wrong.
    GHIDRA_LOST_STORE = re.compile(
        r'^\s*([A-Za-z_]\w*(?:\[[A-Za-z0-9_]+\]|\.\w+|->\w+)*)\s*=\s*\1\s*;\s*$', re.M)

    def _is_read(region, var):
        """Is `var` READ anywhere in this function, as opposed to only written?

        The old test counted OCCURRENCES and called two or fewer a dead store —
        a declaration plus one assignment. That is the right idea and the wrong
        measure: gcc often initialises the slot as well, and

            uVar2 = 0;
            uVar2 = extraout_EAX;               /* and nothing ever reads it */

        is three occurrences and still dead. MeAssetDBXMLIO was held on exactly
        that, after the store that used to read it had been removed as a
        variadic padding word. Count reads, which is what the docstring above
        has always claimed this does."""
        word = r'(?<![\w])' + re.escape(var) + r'\b'
        for line in region.split('\n'):
            for m in re.finditer(word, line):
                rest = line[m.end():]
                if re.match(r'\s*=[^=]', rest):
                    continue                      # written, not read
                before = line[:m.start()].rstrip()
                if re.search(r'[A-Za-z_]\w*[\s\*]*$', before) and not before.endswith(
                        (',', '(', '=', '+', '-', '*', '/', '&', '|', '!', '<', '>', '[')):
                    continue                      # part of its own declaration
                return True
        return False

    def live_unmodelled(src):
        """Unmodelled-value names that are READ BEFORE anything assigns them.

        Order matters and scope matters, so this walks each function's lines in
        turn. MdtConstraintDisable declares `MeDictNode *unaff_ESI`, assigns it
        `(c->head).bodyNode`, and only then passes it to MeDictDelete: the name
        records where gcc kept the value, not a gap in the recovery. Reading one
        that nothing has assigned is the actual defect."""
        live = []
        for region in re.split(r'^/\* ---- ', src, flags=re.M):
            names = set(m.group(1) for m in GHIDRA_UNMODELLED.finditer(region))
            for name in sorted(names):
                word = r'\b' + re.escape(name) + r'\b'
                for line in region.split('\n'):
                    if not re.search(word, line):
                        continue
                    # Its own declaration. The declarator can be a pointer to an
                    # array — `MeReal (*unaff_ESI) [3];` — which a plainer
                    # pattern misses, and then the DECLARATION reads as a stray
                    # read and the object is held back for nothing. McdGjk was.
                    if re.match(r'\s*(?:const\s+|struct\s+|unsigned\s+|signed\s+)*'
                                r'[A-Za-z_]\w*\s*[\*\(\s]*' + word
                                + r'\s*[\)\[\]\w\s\*]*(?:=[^=][^;]*)?;\s*$', line):
                        continue      # declaration, with or without an initialiser
                    if re.match(r'\s*' + word + r'\s*=[^=]', line):
                        break                     # assigned first; dataflow intact
                    m = re.match(r'\s*(\w+)\s*=\s*' + word + r'\s*;\s*$', line)
                    if m and not _is_read(region, m.group(1)):
                        continue                  # dead store: nothing reads it
                    live.append(name)
                    break
        return sorted(set(live))

    # Objects a real match has proven, with the evidence recorded alongside.
    proven = set()
    pf = os.path.join(root, 'proven.txt')
    if os.path.exists(pf):
        for line in open(pf):
            line = line.strip()
            if line and not line.startswith('#'):
                proven.add(line.split()[0])

    rows, counts = [], {'OK': 0, 'TODO': 0, 'REVIEW': 0, 'FAIL': 0, 'SKIP': 0}
    inert_notes = {}
    for obj in objs:
        base = os.path.basename(obj)[:-2]
        archive = os.path.basename(os.path.dirname(obj))
        dump = os.path.join(args.dump_dir, base + '.o.c')
        if not os.path.exists(dump):
            rows.append((archive, base, 'SKIP', 'no Ghidra dump')); counts['SKIP'] += 1
            continue
        fns = dump_functions(dump)
        if not fns:
            rows.append((archive, base, 'SKIP', 'dump has no functions')); counts['SKIP'] += 1
            continue

        outdir = os.path.join(args.out_dir, archive.replace('lib', ''))
        os.makedirs(outdir, exist_ok=True)
        prelude = os.path.join(outdir, base + '.prelude.h')
        exports = os.path.join(outdir, base + '.exports.h')
        vtables = os.path.join(outdir, base + '.vtables.h')
        if not os.path.exists(prelude):          # never clobber hand-edited work
            cmd = [sys.executable, os.path.join(here, 'gen_prelude.py'), obj,
                   '--include-dir', inc, '--dump', dump, '-o', prelude,
                   '--exports-out', exports,
                   '--umbrella', os.path.join(root, 'include', 'kd_karma.h')]
            if args.protos:
                cmd += ['--protos', args.protos]
            cmd += ['--corpus', args.obj_dir]
            r = run(cmd)
            if r.returncode != 0:
                rows.append((archive, base, 'FAIL', 'gen_prelude: ' + tool_error(r.stderr)))
                counts['FAIL'] += 1
                continue
        # Count only real markers. The generated file's own header comment
        # contains the word TODO, which otherwise makes every object look as if
        # it needs a human and pins the OK count at zero.
        # C++ ABI data is pure derivation from the object, so always regenerate.
        run([sys.executable, os.path.join(here, 'gen_vtables.py'), obj, '-o', vtables])

        todos = open(prelude, errors='ignore').read().count('/* TODO')

        csrc = os.path.join(outdir, base + '.c')
        drops = []
        for f in fns:
            if DROP_ALWAYS.match(f):
                drops += ['--drop', f]
        r = run([sys.executable, os.path.join(here, 'ghidra_clean.py'), dump,
                 '-o', csrc, '--object', obj, '--prelude', prelude,
                 '--exports', exports, '--vtables', vtables,
                 '--metoolkit-include', inc,
                 '--field-map', os.path.join(root, 'include',
                                             'kd_types_fields.json')]
                + (['--protos', args.protos] if args.protos else [])
                # `--cflag=-m32`, not `--cflag -m32`: argparse reads a value
                # that starts with `-` as the next option and bails out.
                + [f'--cflag={f}' for f in cflags] + drops)
        if r.returncode != 0:
            rows.append((archive, base, 'FAIL', 'clean: ' + tool_error(r.stderr)))
            counts['FAIL'] += 1
            continue
        # ghidra_clean.py drives a compile-feedback repair loop; report what it
        # had to do, so a jump in repairs is visible rather than silent.
        m = re.search(r'(\d+) line\(s\) repaired', r.stdout)
        repaired = f', {m.group(1)} line(s) repaired' if m else ''

        o = os.path.join(args.build_dir, base + '.o')
        if os.path.exists(o):
            os.unlink(o)                          # a stale object must never look like a pass
        r = run(['gcc'] + cflags + ['-c', '-o', o, csrc])
        if not os.path.exists(o):
            first = next((l for l in r.stderr.splitlines() if ' error' in l), r.stderr[:90])
            if MISLABELLED_CALL.search(first):
                rows.append((archive, base, 'REVIEW',
                             'indirect call through a mislabelled symbol: ' + first.strip()[:70]))
                counts['REVIEW'] += 1
            else:
                rows.append((archive, base, 'FAIL', first.strip()[:110]))
                counts['FAIL'] += 1
            continue

        if base in MEASURED_WRONG:
            os.unlink(o)
            rows.append((archive, base, 'REVIEW',
                         'measured wrong: ' + MEASURED_WRONG[base]))
            counts['REVIEW'] += 1
            continue

        src = open(csrc, errors='ignore').read()
        outside = check_frame_bounds.violations(src)
        if outside:
            os.unlink(o)
            rows.append((archive, base, 'REVIEW',
                         f'{len(outside)} reference(s) outside the local they '
                         f'name: ' + ', '.join(f'{v}{o_:+#x} of {s}' for
                                               _, v, o_, s in outside[:2])))
            counts['REVIEW'] += 1
            continue
        lost = GHIDRA_LOST_STORE.findall(src)
        if lost:
            os.unlink(o)
            rows.append((archive, base, 'REVIEW',
                         f'{len(lost)} self-assignment(s) — a save-and-restore '
                         f'Ghidra reordered: ' + ', '.join(f'{x} = {x};' for x in lost[:2])))
            counts['REVIEW'] += 1
            continue
        nregparm = len(GHIDRA_REGPARM.findall(src))
        if nregparm:
            os.unlink(o)
            rows.append((archive, base, 'REVIEW',
                         f'{nregparm} function(s) tagged __regparmN — Ghidra shifted '
                         f'the parameter list and dropped an argument'))
            counts['REVIEW'] += 1
            continue
        unmodelled = live_unmodelled(src)
        released = RELEASED_UNMODELLED.get(base, ())
        # A release that no longer applies is worse than no release: it sits
        # there looking like evidence and covers whatever appears next under the
        # same name. Say so rather than letting it rot silently — the stale
        # skip-list of HANDOVER.md §12 is exactly this failure.
        for name in released:
            if name not in unmodelled:
                print(f'  ! {base}: RELEASED_UNMODELLED lists {name!r}, which the '
                      f'detector no longer reports — the entry is stale',
                      file=sys.stderr)
        unmodelled = [u for u in unmodelled if u not in released]
        if unmodelled:
            # ASK THE COMPILER WHETHER THE VALUE REACHES ANYTHING. §8.
            proof = prove_inert(csrc, src, unmodelled, cflags, args.build_dir, base)
            if proof is True:
                src = initialise_unmodelled(src, unmodelled)
                open(csrc, 'w').write(src)
                r = run(['gcc'] + cflags + ['-c', '-o', o, csrc])
                if not os.path.exists(o):
                    rows.append((archive, base, 'REVIEW',
                                 'reads a value Ghidra could not account for: '
                                 + ', '.join(unmodelled[:3])))
                    counts['REVIEW'] += 1
                    continue
                inert_notes[base] = unmodelled
                unmodelled = []
            elif proof:
                unmodelled = proof
        if unmodelled:
            os.unlink(o)
            rows.append((archive, base, 'REVIEW',
                         'reads a value Ghidra could not account for: '
                         + ', '.join(unmodelled[:3])))
            counts['REVIEW'] += 1
            continue
        nguess = len(stack_guesses(src))
        nalloca = len(GHIDRA_ALLOCA_FRAME.findall(src))
        nrecon = len(GHIDRA_RECONSTRUCTED_FRAME.findall(src))
        if nrecon and base in proven:
            nrecon = 0            # released on evidence; see proven.txt
        if nrecon:
            os.unlink(o)
            rows.append((archive, base, 'REVIEW',
                         'frame reconstructed by inference — unproven in a real match'))
            counts['REVIEW'] += 1
            continue
        nind = len(GHIDRA_ARGLESS_INDIRECT.findall(src))
        if nind:
            os.unlink(o)
            rows.append((archive, base, 'REVIEW',
                         f'{nind} indirect call(s) emitted with no arguments — '
                         f'Ghidra had no signature for the function pointer'))
            counts['REVIEW'] += 1
            continue
        if nalloca and not nguess:
            os.unlink(o)
            rows.append((archive, base, 'REVIEW',
                         f'{nalloca} materialised alloca frame(s) — size comes from '
                         f'the same frame Ghidra could not model'))
            counts['REVIEW'] += 1
            continue
        if nguess:
            # Drop the object: it compiled, but it must not reach the validated
            # set or downstream tooling will treat broken code as recovered.
            os.unlink(o)
            rows.append((archive, base, 'REVIEW',
                         f'{nguess} call arg(s) routed through a guessed stack frame'))
            counts['REVIEW'] += 1
            continue

        status = 'TODO' if todos else 'OK'
        rows.append((archive, base, status,
                     f'{len(fns)} fns' + (f', {todos} prelude TODO(s)' if todos else '')
                     + repaired))
        counts[status] += 1

    width = max((len(r[1]) for r in rows), default=10)
    cur = None
    for archive, base, status, note in rows:
        if archive != cur:
            print(f'\n{archive}'); cur = archive
        mark = {'OK': '  ok  ', 'TODO': ' todo ', 'REVIEW': 'review',
                'FAIL': ' FAIL ', 'SKIP': ' skip '}[status]
        print(f'  [{mark}] {base:<{width}}  {note}')

    total = sum(counts.values())
    print(f'\n{"="*66}')
    print(f'  compiled, no human input needed : {counts["OK"]:4d} / {total}')
    print(f'  compiled, prelude has TODOs     : {counts["TODO"]:4d} / {total}')
    print(f'  needs human review (bad symbol) : {counts["REVIEW"]:4d} / {total}')
    print(f'  did not compile                 : {counts["FAIL"]:4d} / {total}')
    print(f'  skipped (no dump)               : {counts["SKIP"]:4d} / {total}')
    ok = counts['OK'] + counts['TODO']
    if total - counts['SKIP']:
        print(f'  -> {100.0*ok/(total-counts["SKIP"]):.1f}% of attempted objects compile')
    if inert_notes:
        n = sum(len(v) for v in inert_notes.values())
        print(f'\n  {n} unmodelled value(s) in {len(inert_notes)} object(s) PROVEN INERT '
              f'and initialised — the emitted code is byte-identical for all of '
              f'{", ".join(INERT_PROBES)}:')
        for base in sorted(inert_notes):
            print(f'    {base:<34} {", ".join(inert_notes[base])}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
