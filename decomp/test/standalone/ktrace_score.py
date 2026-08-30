#!/usr/bin/env python3
"""ktrace_score.py — reduce a Karma state trace to a behavioural verdict against a reference.

    ktrace_score.py REFERENCE.csv CANDIDATE.csv

Prints one line and exits 0 when the candidate BEHAVES like the reference, 1 otherwise.

WHY A BEHAVIOURAL SCORE RATHER THAN A NUMERIC ONE. A contact simulation amplifies a last-bit
difference without bound, so "max delta" saturates within a second of the first contact and stops
telling you anything — karma-decomp/test/README.md records that lesson against substitute_test.sh.
What does NOT saturate is what the body ended up doing: a box that comes to rest on the floor and a
box that is 800 units below it are different outcomes no matter how the arithmetic got there.

The three columns:

  rest    bodies whose final Z is within --ztol of the reference's. Coarse on purpose.
  sleep   bodies whose final ENABLED flag matches. Karma disables a body that has settled, so this
          separates "resting" from "still being pushed every frame" — which is the difference
          between the reference's hoverbikes and the recovered build's, and it shows up long before
          the position does.
  gone    bodies present in one trace and not the other, i.e. destroyed mid-run. Reported, never
          silently dropped: bDestroyOnWorldPenetrate removes an actor that passed through the
          world, so a missing row IS the defect rather than missing data.

Exit status is `bodies that do not match`, so it composes into a bisect.
"""
import sys


def load(path):
    last, first = {}, {}
    for line in open(path):
        if line[0] in '#\n':
            continue
        f = line.rstrip('\n').split(',')
        if f[0] != 'B':
            continue
        key = f[3] if f[4] == '-1' else '%s#%s' % (f[3], f[4])
        rec = (int(f[1]), int(f[5]), float(f[8]))   # frame, enabled, z
        last[key] = rec
        first.setdefault(key, rec)
    return first, last


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    ztol = 1.0
    for a in sys.argv[3:]:
        if a.startswith('--ztol='):
            ztol = float(a.split('=', 1)[1])

    rf, rl = load(sys.argv[1])
    cf, cl = load(sys.argv[2])

    names = sorted(set(rl) | set(cl))
    rest = sleep = gone = 0
    bad = []
    for n in names:
        if n not in rl or n not in cl:
            gone += 1
            bad.append('%s:GONE' % n)
            continue
        okz = abs(rl[n][2] - cl[n][2]) <= ztol
        oke = rl[n][1] == cl[n][1]
        rest += okz
        sleep += oke
        if not (okz and oke):
            bad.append('%s:%s%s' % (n, '' if okz else 'z%+.1f' % (cl[n][2] - rl[n][2]),
                                    '' if oke else '/en%d!=%d' % (cl[n][1], rl[n][1])))

    n = len(names)
    mism = n - sum(1 for x in names if x in rl and x in cl and
                   abs(rl[x][2] - cl[x][2]) <= ztol and rl[x][1] == cl[x][1])
    print('rest %d/%d  sleep %d/%d  gone %d  ->  %s' %
          (rest, n, sleep, n, gone, 'MATCH' if mism == 0 else '%d MISMATCH' % mism))
    if bad:
        print('   ' + '  '.join(bad[:12]))
    return mism


if __name__ == '__main__':
    sys.exit(main())
