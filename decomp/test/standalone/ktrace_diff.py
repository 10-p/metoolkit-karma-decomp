#!/usr/bin/env python3
"""ktrace_diff.py — compare two Karma state traces recorded by test/ktrace_run.sh.

    ktrace_diff.py A.csv B.csv [--tol 1e-4] [--body NAME]

WHAT IT REPORTS, and why each column is there rather than a single "max delta":

  first    the frame at which the two runs first differ by more than --tol. In a contact
           simulation a last-bit difference amplifies without bound, so the MAXIMUM stops
           discriminating almost immediately — every body on the path reaches metres.
           karma-decomp/test/README.md records that believing the maximum cost this project
           a session; the first-differing step is the column that still means something.

  restZ    the body's Z in the LAST frame of the trace, per side. This is the column the
           owner's report is actually about: "clips through the floor" is restZ far below
           the floor, and "rests" is restZ steady. It is a behavioural reading, not a
           numerical one, and it survives the amplification that ruins `first`.

  ended    whether the body was still ENABLED in the last frame. Karma auto-disables a body
           that has come to rest, so "sits still" and "sinks slowly" differ here before they
           differ visibly in position — and a body that never disables is being pushed by
           something every frame.

  fell     the largest drop in Z from the body's own starting height. Separates "settled
           slightly lower" from "left the level".

A body present in one trace and not the other is reported rather than skipped: an actor that
was destroyed mid-run (bDestroyOnWorldPenetrate) disappears from the trace, and that is a
result, not a gap in the data.
"""
import sys, csv, math
from collections import OrderedDict


def load(path):
    """-> (bodies: name -> [(frame, enabled, pos, quat, vel, avel)], inventory rows)"""
    bodies, inv = OrderedDict(), []
    with open(path) as fh:
        for line in fh:
            if line.startswith('#') or not line.strip():
                continue
            f = line.rstrip('\n').split(',')
            if f[0] == 'A':
                inv.append(f[1:])
            elif f[0] == 'B':
                frame = int(f[1])
                key = f[3] if f[4] == '-1' else '%s#%s' % (f[3], f[4])
                v = [float(x) for x in f[6:19]]
                bodies.setdefault(key, []).append((frame, int(f[5]), v[0:3], v[3:7], v[7:10], v[10:13]))
    return bodies, inv


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    opts = dict()
    for a in sys.argv[1:]:
        if a.startswith('--'):
            k, _, v = a[2:].partition('=')
            opts[k] = v or True
    if len(args) < 2:
        print(__doc__)
        return 2
    tol = float(opts.get('tol', 1e-4))
    only = opts.get('body')

    A, invA = load(args[0])
    B, invB = load(args[1])
    la, lb = args[0].split('/')[-1], args[1].split('/')[-1]

    print('%s   %d bodies, %d frames' % (la, len(A), max((v[-1][0] for v in A.values()), default=0)))
    print('%s   %d bodies, %d frames' % (lb, len(B), max((v[-1][0] for v in B.values()), default=0)))
    print()

    names = list(A.keys()) + [n for n in B if n not in A]
    hdr = ('%-16s %8s  %10s %10s  %5s %5s  %9s %9s' %
           ('body', 'first', 'restZ:' + la[:8], lb[:8], 'en', 'en', 'fell', 'fell'))
    print(hdr)
    print('-' * len(hdr))

    verdicts = []
    for n in names:
        if only and only not in n:
            continue
        a, b = A.get(n), B.get(n)
        if a is None or b is None:
            print('%-16s %8s  %s' % (n, '-', 'PRESENT ONLY IN ' + (lb if a is None else la)))
            verdicts.append((n, 'missing'))
            continue

        # Frames are the trace's own index and are directly comparable: the runs are pinned to
        # the same fixed step, which the F rows record and the hook refuses to run without.
        bf = {f[0]: f for f in b}
        first = None
        for fa in a:
            fb = bf.get(fa[0])
            if fb is None:
                continue
            d = max(abs(x - y) for x, y in zip(fa[2] + fa[3], fb[2] + fb[3]))
            if d > tol:
                first = fa[0]
                break

        za0, zb0 = a[0][2][2], b[0][2][2]
        za, zb = a[-1][2][2], b[-1][2][2]
        fella = min(x[2][2] for x in a) - za0
        fellb = min(x[2][2] for x in b) - zb0
        print('%-16s %8s  %10.3f %10.3f  %5d %5d  %9.2f %9.2f' %
              (n, first if first is not None else 'same', za, zb, a[-1][1], b[-1][1], fella, fellb))
        verdicts.append((n, first))

    print()
    same = [n for n, f in verdicts if f is None]
    print('%d of %d bodies never differ by more than %g in position or orientation.' %
          (len(same), len(verdicts), tol))
    if len(same) == len(verdicts):
        print('The two builds are behaviourally IDENTICAL on this map at this tolerance.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
