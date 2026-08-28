#!/usr/bin/env python3
"""cold_triage.py — separate Karma code that is DEAD from Karma code that is merely UNTESTED.

    cold_triage.py <census.txt> [<census.txt> ...] <instrumented binary> \\
        --build <engine-build-dir> --members <per-archive-member-dir> [--objdir DIR]

WHY THIS EXISTS. `census_report.py` answers "which recovered functions has the real game ever
entered" and gets 32%. Read alone that number is alarming and it is also wrong, because a large
part of what it calls cold is code UT2004 cannot reach on any input: MathEngine shipped
profiling, debug-draw, XML output and five constraint types this game never instantiates.
`reachable.py` answers the other half — which objects are linkable-to at all — and retires 31
members on that basis. Neither number is the one worth quoting. Their INTERSECTION is:

    reachable  AND  never executed  =  the real unvalidated set

and that is what this prints. "145 objects pass" and "656 of 2,025 functions have ever run" are
both true and neither is the denominator anyone should be reasoning about.

THE FOUR VERDICTS, and the two weak ones are marked as weak rather than folded into the strong
ones:

  DEAD-OBJECT   its member is in `reachable.py`'s unreachable set. Nothing the engine imports
                leads here, transitively, and that includes taking a function's ADDRESS — Karma
                installs `McdBoxBoxIntersect` into a table rather than calling it, and the
                address-taking is a relocation the walk follows. Out of scope, not a to-do.

  DEAD-SYMBOL   the object is reachable but this particular GLOBAL symbol is referenced by
                nothing in the closure: not by the engine, not by any member the engine reaches.
                An exported entry point of the library that this game never calls. Strong, but
                strictly weaker than DEAD-OBJECT — see the limits below.

  FILE-LOCAL    a static function. It has no external symbol, so symbol-level reachability has
                NOTHING TO SAY about it, and reporting it as dead would be an artefact of the
                instrument rather than a finding. Counted separately and never silently merged.

  UNVALIDATED   reachable, referenced, and never executed on any recorded run. THIS IS THE
                NUMBER. Every one of these is code that ships to a player and that nothing in
                this project has measured.

⚠ WHAT DEAD-SYMBOL CANNOT SEE, and it is the same blind spot `reachable.py` documents about
itself: a table the ENGINE fills in, and anything reached only through data the engine supplies.
A symbol referenced by no relocation may still be entered through a function pointer the engine
computed. So DEAD-SYMBOL means "no static reference exists", which is evidence and not proof.
Treat it as a strong prior, not a licence to delete.

⚠ AND THE CENSUS SIDE IS A LOWER BOUND ON WHAT RUNS. It is a union over the runs it was given.
A function absent from every one of them was not reached BY THOSE RUNS; another map, another
gametype or another five minutes may reach it. That asymmetry is why the verdict is
"UNVALIDATED" and not "unreachable" — the claim is about our measurements, not about the code.
"""
import importlib.util
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

# ★ 148 OF THE PROJECT'S "2,025 RECOVERED FUNCTIONS" ARE NOT FUNCTIONS.
# `__x86.get_pc_thunk.bx` and its siblings are three instructions gcc emits per
# object to load the PC into a register; every `nm`-based count in this project
# has been including them, so the denominator `census_report.py` prints is 7.3%
# larger than the recovery is. They cannot be "entered" in any interesting sense
# and reporting them as untested code would be padding. Excluded here, and the
# number is printed BOTH ways so this file's figures can be reconciled against
# the ones already recorded in `../proven.txt`.
NOT_RECOVERED = ('__x86.get_pc_thunk',)


def is_thunk(sym):
    return any(sym.startswith(p) for p in NOT_RECOVERED)


def _load(name):
    spec = importlib.util.spec_from_file_location('kd_' + name,
                                                  os.path.join(HERE, name + '.py'))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


census_report = _load('census_report')
reachable = _load('reachable')


def metoolkit_index(members):
    """symbol -> defining member, member -> its imports, member -> archive."""
    defines, imports, owner = {}, {}, {}
    for archive in sorted(os.listdir(members)):
        adir = os.path.join(members, archive)
        if not os.path.isdir(adir):
            continue
        for fn in sorted(os.listdir(adir)):
            if not fn.endswith('.o'):
                continue
            key, path = fn[:-2], os.path.join(adir, fn)
            d, u = reachable.defined_and_undefined(path)
            imports[key] = u
            owner[key] = archive
            for s in d:
                defines.setdefault(s, key)
    return defines, imports, owner


def closure(build, defines, imports):
    """(reached members, every symbol anything in the closure REFERENCES)."""
    engine_objs = []
    for root, _dirs, files in os.walk(build):
        for f in files:
            if f.endswith('.o') and 'metoolkit' not in root:
                engine_objs.append(os.path.join(root, f))
    referenced = set()
    for o in engine_objs:
        _d, u = reachable.defined_and_undefined(o)
        referenced |= u
    reached, queue = set(), []
    for s in referenced & set(defines):
        m = defines[s]
        if m not in reached:
            reached.add(m)
            queue.append(m)
    while queue:
        m = queue.pop()
        for s in imports.get(m, ()):
            referenced.add(s)
            n = defines.get(s)
            if n and n not in reached:
                reached.add(n)
                queue.append(n)
    return engine_objs, reached, referenced


def bindings(objdir):
    """object -> {symbol: True if GLOBAL}. Ghidra's statics stay static."""
    import subprocess
    out = {}
    for fn in sorted(os.listdir(objdir)):
        if not fn.endswith('.c.o'):
            continue
        r = subprocess.run(['nm', '--defined-only', os.path.join(objdir, fn)],
                           capture_output=True, text=True).stdout
        d = {}
        for line in r.splitlines():
            p = line.split()
            if len(p) == 3 and p[1] in 'tTwW':
                d[p[2]] = p[1] in 'TW'
        out[fn[:-4]] = d
    return out


def main():
    argv = sys.argv[1:]
    if not argv or argv[0] in ('-h', '--help'):
        print(__doc__)
        return 2
    build = members = objdir = None
    rest = []
    i = 0
    while i < len(argv):
        a = argv[i]
        if a == '--build':
            i += 1; build = argv[i]
        elif a == '--members':
            i += 1; members = argv[i]
        elif a.startswith('--objdir='):
            objdir = a.split('=', 1)[1]
        elif a == '--objdir':
            i += 1; objdir = argv[i]
        else:
            rest.append(a)
        i += 1
    if len(rest) < 2 or not build or not members:
        print(__doc__)
        return 2
    binary, census = rest[-1], rest[:-1]

    if objdir is None:
        objdir = os.path.normpath(os.path.join(
            os.path.dirname(binary), '..', '..',
            'Source/Engine/CMakeFiles/KarmaDecomp.dir/__/__/karma-decomp/generated/allobj'))

    counts = census_report.load_counts(census)
    addrs, names = census_report.symbols(binary)
    # ---- THE TWO REFUSALS, both of which produce a finding-shaped zero.
    # A stripped or uninstrumented binary resolves nothing and reads as "the
    # physics never ran"; census_report.py records that this exact reading cost
    # a session, because -no-pie has to be on the EXECUTABLE.
    if not addrs:
        sys.exit('cold_triage: no symbols in %s — is it stripped?' % binary)
    if not counts:
        sys.exit('cold_triage: NO COUNTS in %s. That is what an uninstrumented binary\n'
                 '  produces and it is indistinguishable from "nothing ran". Build with\n'
                 '  -DKD_CENSUS=ON and -no-pie ON THE EXECUTABLE.' % ', '.join(census))
    hit = set()
    for a in counts:
        s = census_report.resolve(addrs, names, a)
        if s:
            hit.add(s)
    if not hit:
        sys.exit('cold_triage: %d addresses resolved to 0 symbols — the census and the\n'
                 '  binary do not match. In a PIE they never will.' % len(counts))

    per_obj = census_report.object_functions(objdir)
    if not per_obj:
        sys.exit('cold_triage: no .c.o files under %s' % objdir)
    glob = bindings(objdir)

    defines, imports, owner = metoolkit_index(members)
    if not imports:
        sys.exit('cold_triage: %s holds no archive member. It wants ONE SUBDIRECTORY PER\n'
                 '  ARCHIVE (see tools/README.md), not a flat directory of .o files —\n'
                 '  which yields nothing and would be reported here as "all dead".'
                 % members)
    engine_objs, reached, referenced = closure(build, defines, imports)
    if not engine_objs:
        sys.exit('cold_triage: no engine objects under %s. An engine built WITHOUT Karma\n'
                 '  imports none of these symbols, the closure is empty, and every\n'
                 '  function would be reported dead. Point at build-native-karma.' % build)

    verdicts = {'DEAD-OBJECT': [], 'DEAD-SYMBOL': [], 'FILE-LOCAL': [], 'UNVALIDATED': []}
    total = entered = thunks = 0
    for base in sorted(per_obj):
        for f in per_obj[base]:
            if is_thunk(f):
                thunks += 1
                continue
            total += 1
            if f in hit:
                entered += 1
                continue
            if base in imports and base not in reached:
                v = 'DEAD-OBJECT'
            elif not glob.get(base, {}).get(f, True):
                v = 'FILE-LOCAL'
            elif f not in referenced:
                v = 'DEAD-SYMBOL'
            else:
                v = 'UNVALIDATED'
            verdicts[v].append((base, f))

    cold = total - entered
    print('  census file(s)                       : %d' % len(census))
    print('  symbols nm reports                   : %d' % (total + thunks))
    print('  …of which compiler PC thunks         : %d  (NOT recovered code — every'
          % thunks)
    print('                                             other count in this project,')
    print('                                             including proven.txt\'s 2,025,')
    print('                                             has been including them)')
    print('  functions the recovery actually has  : %d' % total)
    print('  ENTERED on some recorded run         : %d (%.1f%%)'
          % (entered, 100.0 * entered / max(total, 1)))
    print('  cold                                 : %d (%.1f%%)'
          % (cold, 100.0 * cold / max(total, 1)))
    print()
    print('  of the cold:')
    for v in ('DEAD-OBJECT', 'DEAD-SYMBOL', 'FILE-LOCAL', 'UNVALIDATED'):
        print('    %-12s %5d' % (v, len(verdicts[v])))
    print()
    print('  ★ THE NUMBER: %d function(s) are reachable, referenced and NEVER MEASURED.'
          % len(verdicts['UNVALIDATED']))
    print('    That is %.1f%% of the recovery, not the %.1f%% a bare census reads as cold.'
          % (100.0 * len(verdicts['UNVALIDATED']) / max(total, 1),
             100.0 * cold / max(total, 1)))
    print()
    for v in ('UNVALIDATED', 'DEAD-SYMBOL', 'FILE-LOCAL', 'DEAD-OBJECT'):
        by = {}
        for base, f in verdicts[v]:
            by.setdefault(base, []).append(f)
        print('  ---- %s (%d in %d object(s)) ----' % (v, len(verdicts[v]), len(by)))
        for base in sorted(by, key=lambda b: -len(by[b])):
            print('    %-32s %3d  %s' % (base, len(by[base]),
                                         ' '.join(sorted(by[base])[:4])
                                         + (' …' if len(by[base]) > 4 else '')))
        print()
    return 0


if __name__ == '__main__':
    sys.exit(main())
