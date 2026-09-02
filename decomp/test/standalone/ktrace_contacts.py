#!/usr/bin/env python3
"""ktrace_contacts.py — compare the CONTACTS in two Karma traces recorded with -KTRACECONTACTS.

    ktrace_contacts.py A.csv B.csv [--body NAME] [--frame N] [--tol 1e-4]
    (--body=NAME also works; a valueless --body is refused rather than ignored)

WHY THIS IS A SEPARATE INSTRUMENT FROM ktrace_diff.py.

  ktrace_diff.py compares STATE, and state is continuous. Two builds whose arithmetic differs
  in the last bit produce B rows that differ in the last digit and then diverge without bound,
  so that instrument reports a difference either way and cannot say WHICH KIND it is. The two
  kinds need opposite responses:

    continuous   the two builds are computing the same thing to different precision.
                 Hold the FP model fixed (-mfpmath=sse on the 32-bit side) and it goes away.

    discrete     one build generated a contact the other did not, or generated it at a
                 different depth. Nothing about precision makes that go away; it is a defect
                 in a decision, and the decision is what has to be found.

  Measured 2026-09-01: with the FP model held fixed, 14 of 15 bodies on ONS-Torlan are
  BIT-identical over all 89 frames and ONSHoverBike3 is bit-identical for eight frames and then
  steps by 1.06 at frame 9. Exact agreement followed by a step change of order one is the
  signature of the second kind, and this tool is what can see it.

  ⚠⚠ AND "FRAME 9" WAS ONE TOSS OF A COIN — corrected 2026-09-02. The LP64 build of the day
  read a pointer's high half as a box half-extent, so its answer moved with ASLR and the same
  binary flipped between two outcomes: one diverging at frame 9, one at frame 1. The KIND of
  finding above was right and is what led to the defect; the frame NUMBER never meant anything.
  ★ Take every native A/B under `setarch --addr-no-randomize`, which makes the run repeatable,
  and repeat it — one sample that agrees is silence. See decomp/proven.txt LP64-BOX-I386-OFF.
  With the repair in, the two traces are the SAME FILE (md5 c31ed77b7323).

WHAT IT REPORTS

  ngroups/ncontacts   straight from the C rows. A count that differs is the finding, full stop:
                      a body held up by three contacts and a body held up by two are not the
                      same simulation and no tolerance argument applies.

  matched             contacts paired between the two sides by NEAREST POSITION, not by row
                      order. ⚠ Row order is NOT meaningful — a body's constraints hang off an
                      MeDict whose order is an allocation artefact and need not agree between
                      two builds. Comparing row 0 to row 0 would manufacture differences out of
                      allocation order, which is the same mistake that made MdtWorld's bodyDict
                      unusable as a trace key.

  maxdev              over matched pairs, the largest absolute difference in any of position,
                      normal or penetration. This is the continuous residual AFTER the discrete
                      question has been answered, and it should be zero on a run where the B
                      rows are bit-identical.

  unmatched           a contact on one side with no counterpart within --tol on the other.
                      Printed in full, because one of these IS the answer.
"""
import sys
from collections import OrderedDict


def bodykey(name, bone):
    return name if bone == '-1' else '%s#%s' % (name, bone)


def load(path):
    """-> (counts: (frame, body) -> (ngroups, ncontacts),
           contacts: (frame, body) -> [ (other, otherbone, p[3], n[3], pen) ],
           frames: sorted list)"""
    counts, contacts, frames = OrderedDict(), OrderedDict(), set()
    with open(path) as fh:
        for line in fh:
            if line.startswith('#') or not line.strip():
                continue
            f = line.rstrip('\n').split(',')
            if f[0] == 'C':
                frame = int(f[1])
                counts[(frame, bodykey(f[3], f[4]))] = (int(f[5]), int(f[6]))
                frames.add(frame)
            elif f[0] == 'K':
                frame = int(f[1])
                v = [float(x) for x in f[7:14]]
                contacts.setdefault((frame, bodykey(f[3], f[4])), []).append(
                    (f[5], f[6], v[0:3], v[3:6], v[6]))
                frames.add(frame)
    return counts, contacts, sorted(frames)


def dist(a, b):
    """Position distance only — the identity of a contact is WHERE it is."""
    return max(abs(x - y) for x, y in zip(a[2], b[2]))


def dev(a, b):
    """Full deviation over every recorded column of a matched pair."""
    return max([abs(x - y) for x, y in zip(a[2] + a[3], b[2] + b[3])] + [abs(a[4] - b[4])])


def pair(ca, cb, tol):
    """Greedy nearest-position matching. -> (pairs, only_a, only_b)

    Greedy is enough and its failure mode is safe: contacts on one body are metres apart in
    practice, so the nearest is unambiguous, and if it ever were ambiguous the result is an
    unmatched pair reported in full rather than a silent mismatch."""
    left, pairs = list(range(len(cb))), []
    only_a = []
    for a in ca:
        best, bd = None, None
        for j in left:
            d = dist(a, cb[j])
            if bd is None or d < bd:
                best, bd = j, d
        if best is not None and bd <= tol:
            left.remove(best)
            pairs.append((a, cb[best]))
        else:
            only_a.append(a)
    return pairs, only_a, [cb[j] for j in left]


def fmt(c):
    return '%-14s bone=%-3s p=(%.6g,%.6g,%.6g) n=(%.6g,%.6g,%.6g) pen=%.6g' % (
        c[0], c[1], c[2][0], c[2][1], c[2][2], c[3][0], c[3][1], c[3][2], c[4])


def main():
    # ⚠ BOTH SPELLINGS, BECAUSE THE DOCSTRING DOCUMENTS THE ONE THE PARSER DID
    # NOT ACCEPT. `--body=NAME` worked; `--body NAME` — which is what the usage
    # line above says — put NAME in the POSITIONALS, where it was silently
    # ignored, and left `body` holding True. `True not in b` then raised a
    # TypeError deep in the loop. A tool that compares every body when it was
    # asked for one is worse than a tool that crashes, so both forms parse now
    # and a valueless option is refused by name.
    VALUED = ('body', 'frame', 'tol')
    args, opts, argv = [], {}, sys.argv[1:]
    i = 0
    while i < len(argv):
        a = argv[i]
        if a.startswith('--'):
            k, eq, v = a[2:].partition('=')
            if not eq and k in VALUED:
                if i + 1 >= len(argv) or argv[i + 1].startswith('--'):
                    print('ktrace_contacts: --%s needs a value' % k)
                    return 2
                i += 1
                v = argv[i]
            opts[k] = v or True
        else:
            args.append(a)
        i += 1
    if len(args) < 2:
        print(__doc__)
        return 2
    tol = float(opts.get('tol', 1e-4))
    only = opts.get('body')
    at = opts.get('frame')
    at = int(at) if at not in (None, True) else None

    cA, kA, fA = load(args[0])
    cB, kB, fB = load(args[1])
    la, lb = args[0].split('/')[-1], args[1].split('/')[-1]

    if not cA or not cB:
        side = la if not cA else lb
        print('*** NO C ROWS IN %s — the run was not given -KTRACECONTACTS. ***' % side)
        print('    The B rows alone cannot answer this question; that is the whole reason')
        print('    this tool exists. Re-record with KD_CONTACTS=1.')
        return 2

    print('%s   %d frames, %d body-frames with contacts' % (la, len(fA), len(cA)))
    print('%s   %d frames, %d body-frames with contacts' % (lb, len(fB), len(cB)))
    print()

    bodies = []
    for (fr, b) in list(cA.keys()) + list(cB.keys()):
        if b not in bodies:
            bodies.append(b)
    shared = [f for f in fA if f in set(fB)]

    # ⚠ SAME GUARD AS ktrace_diff.py, FOR THE SAME REASON: a trace that stopped early agrees
    # with everything it never reached, and a verdict computed over the shared prefix would
    # call two builds identical on the strength of the frames one of them never ran.
    if fA and fB and len(shared) < 0.9 * max(len(fA), len(fB)):
        print('*** NOT COMPARABLE: %s has %d frames against %s\'s %d. ***'
              % (la, len(fA), lb, len(fB)))
        return 2

    hdr = '%-18s %10s %10s %12s %10s' % ('body', 'firstcount', 'firstdev', 'maxdev', 'unmatched')
    print(hdr)
    print('-' * len(hdr))

    verdict_count, verdict_dev = [], []
    for b in bodies:
        if only and only not in b:
            continue
        first_count, first_dev, maxdev, unmatched = None, None, 0.0, 0
        for fr in shared:
            a_n = cA.get((fr, b))
            b_n = cB.get((fr, b))
            if a_n is None or b_n is None:
                continue
            if a_n != b_n and first_count is None:
                first_count = fr
            ka, kb = kA.get((fr, b), []), kB.get((fr, b), [])
            pairs, oa, ob = pair(ka, kb, tol)
            unmatched += len(oa) + len(ob)
            for x, y in pairs:
                d = dev(x, y)
                if d > maxdev:
                    maxdev = d
                if d > tol and first_dev is None:
                    first_dev = fr
            if (oa or ob) and first_dev is None:
                first_dev = fr
        print('%-18s %10s %10s %12.6g %10d'
              % (b, first_count if first_count is not None else 'same',
                 first_dev if first_dev is not None else 'same', maxdev, unmatched))
        verdict_count.append((b, first_count))
        verdict_dev.append((b, first_dev))

    print()
    same_c = [b for b, f in verdict_count if f is None]
    same_d = [b for b, f in verdict_dev if f is None]
    print('%d of %d bodies have the SAME CONTACT COUNT in every shared frame.'
          % (len(same_c), len(verdict_count)))
    print('%d of %d bodies have contacts agreeing to %g in every shared frame.'
          % (len(same_d), len(verdict_dev), tol))

    # The detail dump. This is what a lead gets narrowed with, so it prints every contact on
    # both sides rather than a summary of them.
    if at is not None:
        print()
        print('=== frame %d, contact by contact ===' % at)
        for b in bodies:
            if only and only not in b:
                continue
            a_n, b_n = cA.get((at, b)), cB.get((at, b))
            if a_n is None and b_n is None:
                continue
            flag = '' if a_n == b_n else '   <<< COUNT DIFFERS'
            print('  %-18s %s: groups=%s contacts=%s   %s: groups=%s contacts=%s%s'
                  % (b, la[:10], a_n[0] if a_n else '-', a_n[1] if a_n else '-',
                     lb[:10], b_n[0] if b_n else '-', b_n[1] if b_n else '-', flag))
            ka, kb = kA.get((at, b), []), kB.get((at, b), [])
            pairs, oa, ob = pair(ka, kb, tol)
            for x, y in pairs:
                d = dev(x, y)
                mark = '  dev=%.6g' % d if d > tol else ''
                print('      = %s%s' % (fmt(x), mark))
            for x in oa:
                print('      %s ONLY %s' % (la[:10], fmt(x)))
            for y in ob:
                print('      %s ONLY %s' % (lb[:10], fmt(y)))

    if len(same_c) == len(verdict_count) and len(same_d) == len(verdict_dev):
        print()
        print('The two builds generate the SAME CONTACTS on this map at this tolerance —')
        print('so a state divergence here is NOT a contact appearing on one side only.')
        return 0
    return 1


if __name__ == '__main__':
    sys.exit(main())
