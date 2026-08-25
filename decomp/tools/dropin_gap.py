#!/usr/bin/env python3
"""
dropin_gap.py — what still has to be recovered before metoolkit can be DROPPED?

The project's end state is a drop-in library: the engine links against recovered
source and NO shipped `.a` member at all. Object counts do not measure that.
"115 of 153 recovered" says nothing about whether the remaining 38 are ones the
engine reaches, and HANDOVER.md 3b is emphatic that most are not.

This answers the question directly, at symbol level:

    seed   every symbol the ENGINE's own object files import
    walk   resolve each against the RECOVERED build first, and only then
           against the shipped archives, transitively
    gap    every SHIPPED member the walk still had to reach

The gap is the work. It shrinks by one member each time an object is recovered
AND released, and by nothing at all when an object is recovered and quarantined
— which is the distinction an object count hides.

WHY THIS IS SOUND FOR CODE REACHED ONLY THROUGH FUNCTION POINTERS, which Karma
is full of: `McdBoxBoxIntersect` is never called by name, it is installed into an
interaction table by its registrar — but TAKING ITS ADDRESS is a relocation
against the symbol, so the reference exists and the walk sees it. Same for
vtable slots. See tools/reachable.py, which uses the same argument for the
narrower question of what is in scope at all.

WHAT IT CANNOT SEE: a symbol reached only through data the engine supplies at
run time. That is why the answer is CHECKED against a real link — pass
--verify-against <undefined-symbols-file> with the output of

    grep -oE "undefined reference to \\`[^']+'" build.log | sed ... | sort -u

taken from a build with every shipped member deleted. An instrument nobody has
run against ground truth is the failure mode this project keeps finding.

  ./dropin_gap.py <engine-build-dir> <recovered-build-dir> <shipped-members-dir>
"""
import argparse
import os
import subprocess
import sys
from collections import defaultdict


def nm(path, *flags):
    return subprocess.run(['nm', '--no-demangle', *flags, path],
                          capture_output=True, text=True).stdout


def symbols(obj):
    """(defined, undefined) for one object file."""
    d, u = set(), set()
    for line in nm(obj).splitlines():
        parts = line.split()
        if not parts:
            continue
        if parts[0] == 'U' and len(parts) >= 2:
            u.add(parts[1])
        elif len(parts) >= 3 and parts[1] in 'TDBRWVGSi':
            d.add(parts[2])
        elif len(parts) >= 2 and parts[0] in 'TDBRWVGSi':
            d.add(parts[1])
    return d, u


def index(members):
    """{symbol: member path} over a directory of .o files, plus the reverse."""
    provides, imports = {}, {}
    for root, _dirs, files in os.walk(members):
        for f in sorted(files):
            if not f.endswith('.o'):
                continue
            p = os.path.join(root, f)
            d, u = symbols(p)
            imports[p] = u
            for s in d:
                provides.setdefault(s, p)
    return provides, imports


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('engine_build')
    ap.add_argument('recovered_build')
    ap.add_argument('shipped_members')
    ap.add_argument('--verify-against',
                    help='file of undefined symbols from a real link with every '
                         'shipped member deleted; the gap must cover all of them')
    ap.add_argument('--status', help='recover.py output, to annotate each gap row')
    args = ap.parse_args()

    rec_provides, rec_imports = index(args.recovered_build)
    shp_provides, shp_imports = index(args.shipped_members)

    # Seed: everything the engine's own objects import that metoolkit defines at
    # all. Engine objects are the ones NOT under the metoolkit trees.
    seed = set()
    for root, _dirs, files in os.walk(args.engine_build):
        for f in files:
            if not f.endswith('.o'):
                continue
            _d, u = symbols(os.path.join(root, f))
            seed |= u
    seed &= (set(rec_provides) | set(shp_provides))

    # Walk. A symbol resolves to the RECOVERED member when one defines it, so a
    # shipped member only appears when nothing recovered can satisfy the link.
    seen_syms, queue = set(), list(seed)
    gap = defaultdict(set)
    used_rec = set()
    while queue:
        s = queue.pop()
        if s in seen_syms:
            continue
        seen_syms.add(s)
        if s in rec_provides:
            m = rec_provides[s]
            used_rec.add(m)
            nxt = rec_imports[m]
        elif s in shp_provides:
            m = shp_provides[s]
            gap[os.path.basename(m)[:-2]].add(s)
            nxt = shp_imports[m]
        else:
            continue
        for t in nxt:
            if t not in seen_syms and (t in rec_provides or t in shp_provides):
                queue.append(t)

    status = {}
    if args.status and os.path.exists(args.status):
        for line in open(args.status, errors='ignore'):
            line = line.strip()
            if line.startswith('[') and ']' in line:
                tag, rest = line[1:].split(']', 1)
                bits = rest.split(None, 1)
                if bits:
                    status[bits[0]] = (tag.strip(),
                                       (bits[1].strip() if len(bits) > 1 else ''))

    print('DROP-IN GAP — shipped members the engine still needs\n')
    print('  %-28s %5s  %-8s %s' % ('member', 'syms', 'state', 'why it is not released'))
    print('  ' + '-' * 96)
    for m, syms in sorted(gap.items(), key=lambda kv: (-len(kv[1]), kv[0])):
        tag, why = status.get(m, ('?', ''))
        print('  %-28s %5d  %-8s %s' % (m, len(syms), tag, why[:60]))
    print('\n  %d shipped member(s), %d symbol(s); %d recovered member(s) in the closure'
          % (len(gap), sum(len(v) for v in gap.values()), len(used_rec)))

    if args.verify_against:
        real = {l.strip() for l in open(args.verify_against) if l.strip()}
        covered = set().union(*gap.values()) if gap else set()
        # gcc prints DEMANGLED names in "undefined reference to", nm prints
        # mangled ones, so compare on both spellings or three C++ symbols look
        # like a hole in the walk when they are a hole in the comparison.
        if covered:
            dem = subprocess.run(['c++filt'], input='\n'.join(sorted(covered)),
                                 capture_output=True, text=True).stdout.split('\n')
            covered = covered | {d.strip() for d in dem if d.strip()}
        missed = real - covered
        print('\n  verify: %d symbol(s) from the real link, %d covered by the gap'
              % (len(real), len(real - missed)))
        if missed:
            print('  NOT PREDICTED (%d) — the walk cannot see these:' % len(missed))
            for s in sorted(missed)[:20]:
                print('    ', s)
        else:
            print('  every symbol the real link reported is in the gap.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
