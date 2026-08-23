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

# gcc-3.2 C++ static-constructor scaffolding. It only ever runs file-scope
# initialisers, which the prelude expresses as plain C initialisers instead.
DROP_ALWAYS = re.compile(r'^(__static_initialization_and_destruction_\d+|_GLOBAL__[ID]_)')


def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, **kw)


def dump_functions(path):
    return re.findall(r'/\* ==== (\S+) ==== \*/', open(path, errors='ignore').read())


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
    GHIDRA_STACK_GUESS = re.compile(
        r'\(int\)a[a-z]Stack_[0-9a-f]+\s*\+\s*[A-Za-z_]\w*')

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

    # Objects a real match has proven, with the evidence recorded alongside.
    proven = set()
    pf = os.path.join(root, 'proven.txt')
    if os.path.exists(pf):
        for line in open(pf):
            line = line.strip()
            if line and not line.startswith('#'):
                proven.add(line.split()[0])

    rows, counts = [], {'OK': 0, 'TODO': 0, 'REVIEW': 0, 'FAIL': 0, 'SKIP': 0}
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
            r = run(cmd)
            if r.returncode != 0:
                rows.append((archive, base, 'FAIL', 'gen_prelude: ' + r.stderr.strip()[:90]))
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
                                             'kd_types_fields.json')] + drops)
        if r.returncode != 0:
            rows.append((archive, base, 'FAIL', 'clean: ' + r.stderr.strip()[:90]))
            counts['FAIL'] += 1
            continue

        o = os.path.join(args.build_dir, base + '.o')
        if os.path.exists(o):
            os.unlink(o)                          # a stale object must never look like a pass
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
        r = run(['gcc', '-m32', '-O2', '-fno-pic', '-fno-strict-aliasing',
                 '-std=gnu99', '-w', '-Wno-int-conversion',
                 '-Wno-incompatible-pointer-types',
                 '-DLINUX', '-c', '-o', o, csrc] + iflags)
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

        src = open(csrc, errors='ignore').read()
        nguess = len(GHIDRA_STACK_GUESS.findall(src))
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
                     f'{len(fns)} fns' + (f', {todos} prelude TODO(s)' if todos else '')))
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
    return 0


if __name__ == '__main__':
    sys.exit(main())
