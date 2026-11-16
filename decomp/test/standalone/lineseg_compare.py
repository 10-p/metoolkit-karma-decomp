#!/usr/bin/env python3
"""lineseg_compare.py — the verdict for lineseg_lp64.sh.

    lineseg_compare.py <ref.txt> <cand.txt> [control.txt]
    KD_BASE_LINESEG="<baseline lines>" lineseg_compare.py ...      # baselines.sh sets it

`ref` is lineseg_lp64.c linked against MathEngine's shipped i386 archives, `cand` the recovered product
at the pointer width under test, `control` (optional) the recovered product at i386 with SSE math.
Exit 0 = pass, 1 = a difference, 2 = the comparison itself is invalid (inputs differ, a file is short).

THREE RULES.

1. cand against ref, with a TOLERANCE. The reference is x87 code from gcc 3.2: intermediates carry 64
   mantissa bits and are rounded to float only where that compiler spilled them. The candidate rounds
   every operation to float (SSE), so last-bit differences are expected and are not defects.
     position, distance  |delta| <= 1e-3 Karma units (0.05 Unreal units)
     normal              |delta| <= 1e-2 per component (about half a degree)
   Measured x87-against-SSE noise is an order of magnitude under both; a layout defect is not close — a
   wrong radius moves a hit by the radius or turns it into a miss. Hit/miss must agree on every case
   except a KNIFE EDGE: a segment whose deepest penetration into the TRUE shape (computed by the harness
   in double precision, from the geometry it created, not from Karma) is within KNIFE of zero. There
   the answer is decided by the last bit of a world-to-local transform and either is right.

2. A difference from ref that the i386 CONTROL REPRODUCES is not pointer width. It is a difference of
   the recovery itself, present at every width, and this gate does not own it: such cases are SHARED,
   counted per geometry and category, and asserted against `KD_BASE_LINESEG` (test/baselines.sh) at the
   case count that baseline was measured at — in both directions, so a new one and one that disappeared
   are both findings. A difference the control does NOT reproduce is LP64 and always fails — for a
   candidate built by the control's compiler (gcc) without FMA. With another compiler it can also be
   that compiler: clang gives MathEngine's uninitialised start-inside reads a stable value no poison
   reveals, so build the same compiler at 32 bits as the candidate before reading it as width.

3. cand against control, EXACTLY. Both are the same recovered source compiled with SSE scalar math, so
   the arithmetic is the same IEEE operations in the same order; the only difference left is the
   pointer width. Every defined field must be bit-identical, and so must the definedness itself.

A field the function did not write (see the mask in lineseg_lp64.c) is not compared. A candidate that
leaves undefined a field the reference defines fails: that is what reading a field at the wrong offset
looks like. Also printed, and never part of the verdict: how often each build disagrees with the TRUE
geometry outside the knife band — the reference is the yardstick for fidelity, not for correctness.
"""
import math
import os
import re
import struct
import sys
from collections import defaultdict

TOL_POS = 1e-3      # Karma units; UT2004 is 50 Unreal units per Karma unit
TOL_NRM = 1e-2
KNIFE = 1e-4        # Karma units of true penetration depth; see the docstring
CATS = ['random', 'start-inside', 'end-inside', 'through', 'grazing', 'axis-parallel', 'degenerate']
FIELDS = ['dist', 'px', 'py', 'pz', 'nx', 'ny', 'nz']


def fl(h):
    return struct.unpack('>f', bytes.fromhex(h))[0]


def load(path):
    head, rows = [], []
    with open(path) as f:
        for line in f:
            if line.startswith('#'):
                head.append(line.strip())
                continue
            p = line.split()
            if len(p) != 15:
                sys.exit('lineseg_compare: %s: malformed line %r' % (path, line[:80]))
            # (kind, category, index, digest, length, depth) is the KEY: equal, or not comparable
            rows.append((tuple([p[0], int(p[1]), int(p[2])] + p[3:6]), int(p[6]), int(p[7], 16),
                         p[8:]))
    return head, rows


def same(a, b, tol):
    x, y = fl(a), fl(b)
    if math.isnan(x) or math.isnan(y):
        return math.isnan(x) and math.isnan(y), 0.0
    if math.isinf(x) or math.isinf(y):
        return x == y, 0.0
    d = abs(x - y)
    return d <= tol, d


def judge(r, c, worst=None, unwritten=None):
    """(status, why) for one case: 'ok', 'knife', 'skip' (the reference's own return is unwritten) or
    'fail' with the first reason. `worst` collects the largest deltas over agreeing hits."""
    (_kind, _cat, _idx, _dg, _slen, sdepth), rret, rmask, rv = r
    _key, cret, cmask, cv = c
    dep = fl(sdepth)
    if not rmask & 1:
        return 'skip', None
    if not cmask & 1:
        return 'fail', 'the return value depends on unwritten memory'
    if (rret != 0) != (cret != 0):
        if abs(dep) <= KNIFE:
            return 'knife', None
        return 'fail', 'hit/miss at true depth %.4g: reference %s, candidate %s' % (
            dep, 'HIT' if rret else 'miss', 'HIT' if cret else 'miss')
    if rret == 0:
        return 'ok', None
    for k in range(7):
        bit = 2 << k
        if not rmask & bit:
            if unwritten is not None:
                unwritten[FIELDS[k]] += 1
            continue
        if not cmask & bit:
            return 'fail', ('%s: written by the reference, read from unwritten memory by the candidate'
                            % FIELDS[k])
        ok, d = same(rv[k], cv[k], TOL_NRM if k >= 4 else TOL_POS)
        if worst is not None:
            worst[k] = max(worst[k], d)
        if not ok:
            return 'fail', '%s: %.9g vs %.9g (true depth %.3g)' % (FIELDS[k], fl(rv[k]), fl(cv[k]), dep)
    return 'ok', None


def where(row):
    return '%-9s %-13s #%-5d' % (row[0][0], CATS[row[0][1]], row[0][2])


def baseline():
    """{(geometry, category): shared failures} and the case count it was measured at."""
    out, n = {}, None
    for line in os.environ.get('KD_BASE_LINESEG', '').split('\n'):
        p = line.split()
        if not p or p[0].startswith('#'):
            continue
        if p[0] == 'cases-per-category':
            n = int(p[1])
            continue
        out[(p[0], CATS.index(p[1]))] = int(p[2])
    return out, n


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    hr, ref = load(sys.argv[1])
    hc, cand = load(sys.argv[2])
    ctl = load(sys.argv[3])[1] if len(sys.argv) > 3 else None
    types = lambda h: [l for l in h if 'linesegfn=' in l]
    ncases = int((re.search(r'cases-per-category=(\d+)', ' '.join(hr)) or [0, 0])[1])
    print('  geometries with a line-segment function: %s'
          % ' '.join(l.split()[1] for l in types(hr) if l.endswith('yes')))
    if types(hr) != types(hc):
        print('  the two builds register DIFFERENT line-segment functions:')
        print('     ref  %s' % types(hr))
        print('     cand %s' % types(hc))
        return 1
    if len(ref) != len(cand) or (ctl is not None and len(ctl) != len(cand)):
        print('  NOT COMPARABLE: %d reference lines, %d candidate%s' % (
            len(ref), len(cand), '' if ctl is None else ', %d control' % len(ctl)))
        return 2

    stat = defaultdict(lambda: defaultdict(int))
    worst = defaultdict(lambda: [0.0] * 7)
    unwritten = defaultdict(int)
    lp64, shared, knife, invalid = [], [], [], 0
    for i, (r, c) in enumerate(zip(ref, cand)):
        if r[0] != c[0] or (ctl is not None and ctl[i][0] != r[0]):
            invalid += 1
            if invalid <= 3:
                print('  INPUTS DIFFER: ref %s / cand %s' % (r[0], c[0]))
            continue
        kind, cat = r[0][0], r[0][1]
        dep = fl(r[0][5])
        s = stat[(kind, cat)]
        s['cases'] += 1
        st, why = judge(r, c, worst[(kind, cat)], unwritten)
        if st == 'skip':
            unwritten['ret'] += 1
            continue
        s['rhit'] += r[1] != 0
        s['chit'] += c[1] != 0
        for who, ret in (('ref', r[1]), ('cand', c[1])):
            if abs(dep) > KNIFE and (dep > 0) != (ret != 0):
                s['truth-' + who] += 1
        if st == 'knife':
            s['knife'] += 1
            knife.append((r, 'true depth %.3g' % dep))
        if st != 'fail':
            continue
        ctl_st = judge(r, ctl[i])[0] if ctl is not None else 'ok'
        if ctl_st == 'fail':
            s['shared'] += 1
            shared.append((r, why))
        else:
            s['lp64'] += 1
            lp64.append((r, why))

    if invalid:
        print('  NOT COMPARABLE: %d case(s) had different inputs in the builds' % invalid)
        return 2

    print('\n  %-9s %-13s %5s %6s %6s %5s %6s %5s  %-8s %-8s %-8s  %s' % (
        'geometry', 'category', 'cases', 'refhit', 'cndhit', 'LP64', 'shared', 'knife',
        'max|dp|', 'max|dd|', 'max|dn|', 'vs truth ref/cand'))
    for key in sorted(stat):
        s, w = stat[key], worst[key]
        print('  %-9s %-13s %5d %6d %6d %5d %6d %5d  %8.1e %8.1e %8.1e  %d/%d' % (
            key[0], CATS[key[1]], s['cases'], s['rhit'], s['chit'], s['lp64'], s['shared'],
            s['knife'], max(w[1:4]), w[0], max(w[4:7]), s['truth-ref'], s['truth-cand']))
    print('\n  hit results whose field the REFERENCE never writes (not compared): %s' % (
        ', '.join('%s %d' % (k, v) for k, v in sorted(unwritten.items())) or 'none'))
    print('  knife edges (|true depth| <= %g, either answer right): %d' % (KNIFE, len(knife)))
    for r, why in knife[:3]:
        print('     %s %s' % (where(r), why))

    rc = 0
    if lp64:
        rc = 1
        # ⚠ "POINTER WIDTH" holds when candidate and control come from the same compiler without FMA.
        # A clang candidate gives MathEngine's uninitialised start-inside reads a stable value the
        # poison cannot expose, and FMA moves the long aggregate segments across the tolerance: build
        # that compiler at 32 bits as the candidate to tell (test/README.md).
        print('\n  FAIL — %d case(s) differ from the shipped library where the i386 control does not:'
              ' POINTER WIDTH (or, for another compiler, that compiler). First ten:' % len(lp64))
        for r, why in lp64[:10]:
            print('     %s %s' % (where(r), why))
    else:
        print('\n  no case differs from the shipped library that the i386 control does not reproduce')

    if ctl is None:
        if shared:
            print('  (no control given: %d candidate difference(s) cannot be attributed)' % len(shared))
    else:
        base, base_n = baseline()
        got = defaultdict(int)
        for r, _w in shared:
            got[(r[0][0], r[0][1])] += 1
        print('  shared with the i386 control (the recovery at every width, not this gate\'s): %d'
              % len(shared))
        for key in sorted(set(got) | set(base)):
            print('     %-9s %-13s %5d   baseline %s' % (key[0], CATS[key[1]], got.get(key, 0),
                                                        base.get(key, '-')))
        if base_n is None:
            print('  (no KD_BASE_LINESEG: the shared differences are reported, not asserted)')
        elif base_n != ncases:
            print('  (the baseline was measured at %d cases per category, this run is %d: reported,'
                  ' not asserted)' % (base_n, ncases))
        elif dict(got) != {k: v for k, v in base.items() if v}:
            rc = 1
            print('  FAIL — the shared differences MOVED from test/baselines.sh. Not automatically a'
                  ' bug, and never a reason to edit the baseline unread: find out why.')
            for r, why in shared[:5]:
                print('     %s %s' % (where(r), why))

        diffs = []
        for a, b in zip(ctl, cand):
            if a[2] != b[2]:
                diffs.append((a, 'definedness mask %02x vs %02x' % (a[2], b[2])))
                continue
            if a[2] & 1 and a[1] != b[1]:
                diffs.append((a, 'ret %d vs %d' % (a[1], b[1])))
                continue
            for k in range(7):
                if a[2] & (2 << k) and a[3][k] != b[3][k]:
                    diffs.append((a, '%s %s vs %s' % (FIELDS[k], a[3][k], b[3][k])))
                    break
        if diffs:
            rc = 1
            per = defaultdict(int)
            for a, _w in diffs:
                per[a[0][0]] += 1
            print('  FAIL against the i386 control (same source, same SSE arithmetic): %d case(s) '
                  'differ (%s). First ten:' % (len(diffs), ', '.join(
                      '%s %d' % kv for kv in sorted(per.items()))))
            for a, why in diffs[:10]:
                print('     %s %s' % (where(a), why))
        else:
            print('  and the candidate is BIT-IDENTICAL to the i386 control in every defined field '
                  '(%d cases)' % len(cand))
    print('  -> %s' % ('PASS' if rc == 0 else 'FAIL'))
    return rc


if __name__ == '__main__':
    sys.exit(main())
