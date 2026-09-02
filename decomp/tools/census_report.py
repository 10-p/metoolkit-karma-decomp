#!/usr/bin/env python3
"""census_report.py — which recovered Karma functions has the REAL GAME ever entered?

    census_report.py <kd_instr output> <engine binary> [<kd_instr output> ...] [--objdir DIR]

Every gate in karma-decomp/test/ answers "does this object reproduce the original". None of them
answers "was this code reached", and the two are indistinguishable in their output: a substituted
object whose trajectory does not move and an object that never executed print the identical line.
test/kd_instr.c settled that for the offline scenes; this settles it for a live match.

WHY IT MATTERS MORE AFTER A PASS THAN BEFORE ONE. A whole-library A/B that reads bit-identical
against MathEngine's own library is strong evidence — for the paths the map exercised, and for
nothing else. "The recovered Karma matches the original" and "the recovered Karma matches the
original on the 38% of its functions this map calls" are very different claims, and only this tool
can tell them apart.

Pass several census files to union them: one map exercises one slice of the library, and the
useful number is what NO run has ever reached.

    KD_INSTR_OUT=/tmp/c1.txt <bin> test-karma-1?game=Onslaught.ONSOnslaughtGame?SpectatorOnly=1 ...
    KD_INSTR_OUT=/tmp/c2.txt <bin> ONS-Torlan?game=Onslaught.ONSOnslaughtGame?NumBots=4 ...
    census_report.py /tmp/c1.txt /tmp/c2.txt build-native-census/Source/SDLLaunch/ut2004-pixo.bin

⚠ IT REPORTS ZERO IF THE BINARY IS NOT THE INSTRUMENTED ONE, which reads exactly like "nothing
ran". Build with -DKD_CENSUS=ON. The check below refuses that case rather than printing a zero.
"""
import bisect
import os
import subprocess
import sys


def load_counts(paths):
    """address -> total entries, unioned across runs."""
    counts = {}
    for p in paths:
        with open(p) as fh:
            for line in fh:
                f = line.split()
                if len(f) != 2:
                    continue
                try:
                    a, n = int(f[0], 16), int(f[1])
                except ValueError:
                    continue
                counts[a] = counts.get(a, 0) + n
    return counts


def symbols(binary):
    out = subprocess.run(['nm', '-n', binary], capture_output=True, text=True).stdout
    addrs, names = [], []
    for line in out.splitlines():
        f = line.split()
        if len(f) == 3 and f[1] in 'tTwW':
            addrs.append(int(f[0], 16))
            names.append(f[2])
    return addrs, names


def resolve(addrs, names, a):
    i = bisect.bisect_right(addrs, a) - 1
    return names[i] if i >= 0 else None


def object_functions(objdir):
    """object base name -> the function symbols it defines."""
    per = {}
    for fn in sorted(os.listdir(objdir)):
        if not fn.endswith('.c.o'):
            continue
        base = fn[:-4]
        out = subprocess.run(['nm', '--defined-only', os.path.join(objdir, fn)],
                             capture_output=True, text=True).stdout
        fns = [l.split()[2] for l in out.splitlines()
               if len(l.split()) == 3 and l.split()[1] in 'tTwW']
        per[base] = sorted(set(fns))
    return per


def main():
    # ⚠ BOTH SPELLINGS OF --objdir, and the space-separated one used to be
    # SILENTLY MISPARSED: only `--objdir=DIR` was recognised, so `--objdir DIR`
    # left DIR in the positional list, `binary = args[-1]` picked up the
    # DIRECTORY, and the real binary was read as a census file. The failure is a
    # UnicodeDecodeError deep in load_counts, which says nothing about the actual
    # mistake. The docstring shows `[--objdir DIR]`, so the form the docs teach
    # was the form that did not work.
    argv, args, objdir = sys.argv[1:], [], None
    i = 0
    while i < len(argv):
        a = argv[i]
        if a == '--objdir' and i + 1 < len(argv):
            objdir = argv[i + 1]; i += 2; continue
        if a.startswith('--objdir='):
            objdir = a.split('=', 1)[1]; i += 1; continue
        if not a.startswith('--'):
            args.append(a)
        i += 1
    if len(args) < 2:
        print(__doc__)
        return 2
    binary = args[-1]
    census = args[:-1]
    if objdir is None:
        objdir = os.path.join(os.path.dirname(binary), '..', '..',
                              'Source/Engine/CMakeFiles/KarmaDecomp.dir/__/__/'
                              'karma-decomp/generated/allobj')
    objdir = os.path.normpath(objdir)

    counts = load_counts(census)
    addrs, names = symbols(binary)
    if not addrs:
        print('no symbols in %s — is it stripped?' % binary)
        return 2
    if not counts:
        print('NO COUNTS in %s.\n'
              '  That is what an UNINSTRUMENTED binary produces, and it is indistinguishable\n'
              '  from "nothing ran". Rebuild with -DKD_CENSUS=ON before believing this.'
              % ', '.join(census))
        return 2

    hit = {}
    for a, n in counts.items():
        s = resolve(addrs, names, a)
        if s:
            hit[s] = hit.get(s, 0) + n

    per_obj = object_functions(objdir)
    if not per_obj:
        print('no .c.o files under %s' % objdir)
        return 2

    tot_f = tot_h = 0
    cold_objs, partial = [], []
    for base in sorted(per_obj):
        fns = per_obj[base]
        h = [f for f in fns if f in hit]
        tot_f += len(fns)
        tot_h += len(h)
        if not h:
            cold_objs.append((base, len(fns)))
        elif len(h) < len(fns):
            partial.append((base, len(h), len(fns), [f for f in fns if f not in hit]))

    print('%d census file(s), %d distinct addresses, %d resolved symbols' %
          (len(census), len(counts), len(hit)))
    print('FUNCTIONS ENTERED: %d of %d (%.1f%%) across %d objects\n' %
          (tot_h, tot_f, 100.0 * tot_h / max(tot_f, 1), len(per_obj)))

    print('OBJECTS NEVER ENTERED AT ALL (%d):' % len(cold_objs))
    for base, n in cold_objs:
        print('   %-34s %3d function(s)' % (base, n))

    print('\nOBJECTS PARTIALLY ENTERED (%d) — the untested functions are named:' % len(partial))
    for base, h, n, missing in sorted(partial, key=lambda x: x[1] - x[2]):
        print('   %-34s %3d/%-3d entered' % (base, h, n))
        for m in missing:
            print('        cold: %s' % m)
    return 0


if __name__ == '__main__':
    sys.exit(main())
