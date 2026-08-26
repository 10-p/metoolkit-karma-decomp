#!/usr/bin/env python3
"""Which recovered Karma objects actually reached the linked .wasm?

THE QUESTION THIS ANSWERS, and why the obvious answer is wrong.

`cmake --build --preset wasm-karmadecomp-perf` succeeding says the 146 sources
COMPILED and the archive was OFFERED to the linker. It does not say a single one
of them is in the binary: `libKarmaDecomp.a` is an ordinary static archive, so
the linker pulls only the members something references. A build that reported
"145 recovered object(s)" at configure time and shipped a .wasm containing none
of them would look exactly the same on the console.

    python3 tools/wasm_members.py <build-dir>

    python3 tools/wasm_members.py ../build-wasm-karmadecomp-perf
    python3 tools/wasm_members.py ../build-wasm-karmadecomp-perf -v   # per-member counts

Prints, per archive member, how many of its defined symbols survived into the
.wasm, and lists the members that contributed none. Exit status is 0 unless the
archive or the wasm is missing — a member contributing nothing is INFORMATION,
not a failure: HANDOVER.md 3b retires the profiling, debug, XML-output and
unused-constraint objects on purpose, and they are exactly the ones that should
be absent.

TWO TRAPS, both hit while writing this, both of which make it under-report:

  1. wasm-ld DEMANGLES the name section by default. The archive holds
     `_ZN12keaLCPSolver8solveLCPEv` and the .wasm holds
     `keaLCPSolver::solveLCP()`. Comparing raw against demangled loses every C++
     object -- the whole kea solver, CxSmallSort, keaMatrix* -- and reads like a
     catastrophic under-link. Both sides are read with `llvm-nm -C`.

  2. DATA symbols do not survive linking as named symbols; they are merged into
     the data segment. A member whose only definitions are data (MeMemory's
     `MeMemoryAPI`, MePool's tables, MeDebugDraw's) can be fully linked in and
     still score zero. Those are reported separately as "data-only", not as
     missing.

AND THE THIRD, WHICH IS WHY THERE IS A SELF-CHECK. A `-g0` build -- which is
every `*-perf` preset -- has NO NAME SECTION, so `llvm-nm` returns nothing and
every single member scores zero. The first run of this tool was on
`build-wasm-karmadecomp-perf` and it confidently reported "143 contribute
nothing", which is a measurement artefact wearing a finding's clothes. It now
refuses that case instead of printing it. Run it on a `wasm-karmadecomp-debug`
tree, or on the `-O2` dev default; the answer is a property of the LINK, and the
two presets differ only in optimisation.
"""
import argparse
import os
import subprocess
import sys

# emsdk's llvm-nm. The system one cannot read wasm objects.
NM = os.path.expanduser('~/emsdk/upstream/bin/llvm-nm')


def nm_archive(path):
    """{member: [(kind, name), ...]} for a wasm static archive, demangled."""
    out, cur = {}, None
    for line in subprocess.run([NM, '-C', path], capture_output=True,
                               text=True).stdout.splitlines():
        if line.endswith('.o:'):
            cur = line[:-1]
            out.setdefault(cur, [])
            continue
        parts = line.split(None, 2)
        if len(parts) == 3 and cur is not None:
            out[cur].append((parts[1], parts[2]))
    return out


def nm_wasm(path):
    """Every symbol name in a linked .wasm, demangled."""
    names = set()
    for line in subprocess.run([NM, '-C', path], capture_output=True,
                               text=True).stdout.splitlines():
        parts = line.split(None, 2)
        if len(parts) == 3:
            names.add(parts[2])
    return names


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('build_dir', help='a build-wasm-karmadecomp-* directory')
    ap.add_argument('-v', '--verbose', action='store_true',
                    help='one line per member')
    args = ap.parse_args()

    archive = os.path.join(args.build_dir, 'Source/Engine/libKarmaDecomp.a')
    wasm = os.path.join(args.build_dir, 'Source/SDLLaunch/SDLLaunch.wasm')
    for p in (archive, wasm):
        if not os.path.exists(p):
            sys.exit(f'not found: {p}\n'
                     '(this needs a BUILT wasm-karmadecomp-* tree; see BUILD.md)')
    if not os.path.exists(NM):
        sys.exit(f'not found: {NM} — source ~/emsdk/emsdk_env.sh, or install emsdk')

    members = nm_archive(archive)
    linked = nm_wasm(wasm)

    # SELF-CHECK. A -g0 link has no name section, so `linked` comes back empty
    # or near-empty and every member below scores zero — a number that looks
    # exactly like a catastrophic finding and is nothing of the kind. The engine
    # alone contributes tens of thousands of symbols, so anything under a
    # thousand means the binary, not the link, is what is missing.
    if len(linked) < 1000:
        sys.exit(
            f'{wasm}\n'
            f'  yields only {len(linked)} symbol(s) — this .wasm has no usable name\n'
            f'  section, which is what `-g0` (every *-perf preset) produces. This tool\n'
            f'  cannot measure it, and would otherwise report every member as missing.\n'
            f'  Build and point at wasm-karmadecomp-debug (or the -O2 dev default): the\n'
            f'  answer is a property of the LINK and does not vary with -O.')

    hit, dataonly, missing = [], [], []
    for name, syms in sorted(members.items()):
        defined = [n for k, n in syms if k in 'TDB']
        funcs = [n for k, n in syms if k == 'T']
        present = [n for n in defined if n in linked]
        if present:
            hit.append((name, len(defined), len(present)))
        elif not funcs:
            dataonly.append((name, len(defined)))
        else:
            missing.append((name, len(funcs)))

    if args.verbose:
        for name, total, present in hit:
            print(f'  {present:>4}/{total:<4} {name}')

    print(f'{len(members)} archive member(s)')
    print(f'  {len(hit):>3} contribute at least one symbol to the .wasm')
    print(f'  {len(dataonly):>3} define only DATA — linked or not, nm cannot tell '
          f'(see the docstring)')
    for name, total in dataonly:
        print(f'        {name} ({total} data symbol(s))')
    print(f'  {len(missing):>3} contribute nothing — not referenced, so not pulled in')
    for name, funcs in missing:
        print(f'        {name} ({funcs} function(s))')


if __name__ == '__main__':
    main()
