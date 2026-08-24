#!/usr/bin/env python3
"""
reachable.py — which Karma objects can UT2004 actually reach?

The point of this project is Karma AS UT2004 USES IT, not MathEngine's library.
An object nothing reaches does not need to compile, and time spent making one
compile is time not spent on the solver — twice now an object was made to
compile that way and turned out to be wrong (HANDOVER.md dead ends 9 and 10).

So this answers the scoping question directly, at symbol level:

    seed  = every symbol the ENGINE's own object files import and metoolkit
            defines
    walk  = for each reachable member, everything IT imports that metoolkit
            defines, transitively
    rest  = unreachable. Out of scope, not a to-do.

WHY SYMBOL-LEVEL IS SOUND HERE, including for the function pointers Karma is
full of. `McdBoxBoxIntersect` is never called by name — it is installed into an
interaction table by `McdBoxBoxRegisterInteraction`. But TAKING ITS ADDRESS is a
relocation against the symbol, so the reference exists and the walk sees it. The
same holds for vtable slots, which are relocations in the vtable's own section.

WHAT IT CANNOT SEE, and where the answer stays "unknown" rather than "dead":
a table the ENGINE fills in with engine functions, and anything reached only
through data the engine supplies. That is why the census (HANDOVER.md §3) and
this are complementary — the census says which pairs the game CALLS, this says
which objects are even linkable-to. Neither subsumes the other.

  ./reachable.py <engine-build-dir> <extracted-members-dir>
"""
import os
import subprocess
import sys
from collections import defaultdict


def run(*cmd):
    return subprocess.run(cmd, capture_output=True, text=True).stdout


def defined_and_undefined(obj):
    """(defined symbols, undefined symbols) for one object."""
    d, u = set(), set()
    for line in run('nm', '--no-demangle', obj).splitlines():
        p = line.split()
        if len(p) == 2 and p[0] == 'U':
            u.add(p[1])
        elif len(p) == 3 and p[1] in 'TDBRWVGSti':
            d.add(p[2])
        elif len(p) == 2 and p[0] in 'TDBRWVGSti':
            d.add(p[1])
    return d, u


def main():
    build, members = sys.argv[1], sys.argv[2]

    # --- index metoolkit: symbol -> defining member, and member -> imports ---
    defines, imports, owner = {}, {}, {}
    for archive in sorted(os.listdir(members)):
        adir = os.path.join(members, archive)
        if not os.path.isdir(adir):
            continue
        for fn in sorted(os.listdir(adir)):
            if not fn.endswith('.o'):
                continue
            key = fn[:-2]
            path = os.path.join(adir, fn)
            d, u = defined_and_undefined(path)
            imports[key] = u
            owner[key] = archive
            for s in d:
                defines.setdefault(s, key)

    # --- seed from the engine's own objects ---
    engine_objs = []
    for root, _dirs, files in os.walk(build):
        for f in files:
            if f.endswith('.o') and 'metoolkit' not in root:
                engine_objs.append(os.path.join(root, f))
    seed_syms = set()
    for o in engine_objs:
        _d, u = defined_and_undefined(o)
        seed_syms |= (u & set(defines))

    # --- transitive closure ---
    reached, queue = set(), []
    for s in seed_syms:
        m = defines.get(s)
        if m and m not in reached:
            reached.add(m); queue.append(m)
    entry = set(reached)
    while queue:
        m = queue.pop()
        for s in imports.get(m, ()):
            n = defines.get(s)
            if n and n not in reached:
                reached.add(n); queue.append(n)

    allm = set(imports)
    dead = allm - reached
    print(f'engine objects scanned      : {len(engine_objs)}')
    print(f'metoolkit members           : {len(allm)}')
    print(f'  reached from the engine   : {len(reached)}   '
          f'({len(entry)} of them directly)')
    print(f'  NOT reachable at all      : {len(dead)}')
    print()
    by_archive = defaultdict(lambda: [0, 0])
    for m in allm:
        by_archive[owner[m]][0 if m in reached else 1] += 1
    print(f'{"archive":32} {"reached":>8} {"dead":>6}')
    for a in sorted(by_archive):
        r, d = by_archive[a]
        print(f'{a:32} {r:8} {d:6}')
    print()
    print('UNREACHABLE members (out of scope unless a call chain appears):')
    for m in sorted(dead):
        print(f'  {owner[m]:28} {m}')


if __name__ == '__main__':
    main()
