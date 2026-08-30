#!/usr/bin/env python3
"""ragdoll_metrics.py — is a ragdoll HELD TOGETHER, or is it just moving?

    ragdoll_metrics.py TRACE.csv [TRACE.csv ...]

WHY THIS EXISTS. The owner's report is "limbs open 180 degrees, limbs do multiple rotations, bodies
overlap" — and a ragdoll is chaotic by nature, so two builds can differ enormously frame to frame
and both be correct. Asking a human to eyeball whether one small change helped is asking for a
measurement eyes cannot make, and asking for it repeatedly is worse. These are the invariants a
CONSTRAINED ragdoll has and an unconstrained one does not, and they hold regardless of trajectory.

  bones        how many bodies the ragdoll has, and how many frames it was traced for.

  link drift   THE JOINT TEST. Two bones joined by a ball-socket are held at a FIXED distance, so
               the standard deviation of that distance over the ragdoll's life is ~0 no matter how
               wildly the pair tumbles. The skeleton's topology is not in the trace, so it is
               INFERRED: the N pairs with the lowest relative distance-variance are the joined
               ones. Reported as the worst such pair's drift, in world units. A held skeleton reads
               hundredths; a skeleton whose joints are not being solved reads whole units and grows.

  spread       max pairwise bone distance. A human ragdoll is a bounded blob. This catches the
               gross failure — a skeleton coming apart — that link drift would also catch but less
               legibly.

  min gap      the closest approach between any two NON-adjacent bones, i.e. self-intersection.
               "Bodies overlap" is this going to ~0. Adjacent pairs are excluded because bones that
               share a joint are SUPPOSED to touch.

  twist        the largest relative rotation between an inferred-joined pair, in degrees. Joint
               LIMITS cap this. Approaching 180 is the owner's "legs opening 180"; a value that
               keeps growing across frames is the "multiple rotations" case.

⚠ READ IT AS A COMPARISON, NEVER AS AN ABSOLUTE. There is no threshold here that means "correct".
The only meaningful use is the same trace format from two builds, one of which is known good — a
`legacy-karma` run is the reference. A number on its own says nothing.
"""
import math
import sys
from collections import defaultdict


def load(path):
    """-> {ragdoll_key: {frame: {bone: (pos, quat)}}}"""
    rag = defaultdict(lambda: defaultdict(dict))
    for line in open(path):
        if not line.startswith('B,'):
            continue
        f = line.rstrip('\n').split(',')
        if f[4] == '-1':
            continue                                  # not a bone body
        frame, actor, bone = int(f[1]), f[2], int(f[4])
        v = [float(x) for x in f[6:13]]
        rag['%s@%s' % (f[3], actor)][frame][bone] = (v[0:3], v[3:7])
    return rag


def dist(a, b):
    return math.sqrt(sum((x - y) ** 2 for x, y in zip(a, b)))


def quat_angle(qa, qb):
    """Relative rotation between two orientations, in degrees."""
    d = abs(sum(x * y for x, y in zip(qa, qb)))
    return math.degrees(2.0 * math.acos(max(-1.0, min(1.0, d))))


def analyse(frames):
    fr = sorted(frames)
    bones = sorted({b for f in fr for b in frames[f]})
    if len(bones) < 2 or len(fr) < 3:
        return None

    # distance series per pair
    series = defaultdict(list)
    for f in fr:
        st = frames[f]
        for i, a in enumerate(bones):
            for b in bones[i + 1:]:
                if a in st and b in st:
                    series[(a, b)].append(dist(st[a][0], st[b][0]))
    if not series:
        return None

    def sd(xs):
        if len(xs) < 2:
            return 0.0
        m = sum(xs) / len(xs)
        return math.sqrt(sum((x - m) ** 2 for x in xs) / len(xs))

    # infer the joined pairs: a joint holds its two bones at a fixed distance, so the pairs with
    # the smallest distance-variance are the skeleton's links. len(bones)-1 of them for a tree.
    ranked = sorted(series, key=lambda k: sd(series[k]))
    links = ranked[:max(1, len(bones) - 1)]
    linkset = set(links)

    drift = max(sd(series[k]) for k in links)
    spread = max(max(v) for v in series.values())
    nonadj = [k for k in series if k not in linkset]
    mingap = min((min(series[k]) for k in nonadj), default=float('nan'))

    # ⚠ RANGE, NOT VALUE. The first version took the largest ABSOLUTE angle between two joined
    # bones and read 179-180 degrees on every trace including `legacy-karma` — because two bones can
    # sit 180 degrees apart in the REST POSE and a limit does not forbid that. A joint limit caps
    # the RANGE OF MOTION, so what it constrains is max-minus-min over the ragdoll's life.
    twist = 0.0
    for (a, b) in links:
        angs = [quat_angle(frames[f][a][1], frames[f][b][1])
                for f in fr if a in frames[f] and b in frames[f]]
        if len(angs) > 1:
            twist = max(twist, max(angs) - min(angs))
    return dict(bones=len(bones), frames=len(fr), drift=drift,
                spread=spread, mingap=mingap, twist=twist)


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    for path in sys.argv[1:]:
        rag = load(path)
        rows = [(k, analyse(v)) for k, v in rag.items()]
        rows = [(k, r) for k, r in rows if r]
        name = path.split('/')[-1]
        if not rows:
            print('%-28s no ragdoll bone rows (need -KTRACEBONES and a match with deaths)' % name)
            continue
        n = len(rows)
        agg = lambda key: sum(r[key] for _, r in rows) / n
        worst = lambda key: max(r[key] for _, r in rows)
        print('%-28s %3d ragdoll(s), %2d bones, %4d frames' %
              (name, n, rows[0][1]['bones'], rows[0][1]['frames']))
        print('%-28s   link drift  mean %8.4f   worst %8.4f   <- joints; ~0 means held' %
              ('', agg('drift'), worst('drift')))
        print('%-28s   spread      mean %8.3f   worst %8.3f' % ('', agg('spread'), worst('spread')))
        print('%-28s   min gap     mean %8.3f   worst %8.3f   <- self-intersection' %
              ('', agg('mingap'), min(r['mingap'] for _, r in rows)))
        print('%-28s   twist range mean %8.1f   worst %8.1f   <- degrees of motion a limit caps' %
              ('', agg('twist'), worst('twist')))
    return 0


if __name__ == '__main__':
    sys.exit(main())
