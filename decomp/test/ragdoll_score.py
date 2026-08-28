#!/usr/bin/env python3
"""ragdoll_score.py — one PASS/FAIL line comparing a ragdoll trace to a reference.

    ragdoll_score.py REFERENCE.csv CANDIDATE.csv

The bisect-friendly form of ragdoll_metrics.py. Exit 0 when the candidate's ragdolls behave like
the reference's, non-zero otherwise, so it drops straight into test/ktrace_subst.sh.

WHICH TWO NUMBERS, AND WHY ONLY TWO. The owner's report has exactly two measurable claims —
"bodies overlap" and "limbs open 180 degrees / rotate freely" — and they map to `min gap` and
`twist range`. `link drift` and `spread` are deliberately NOT scored: they read the same on both
arms (0.014 vs 0.015, 1.51 vs 1.44), so the joints ARE being held and scoring them would only add
noise to the verdict.

★ THE TOLERANCE IS 5% AND THAT IS NOT ARBITRARY. Two runs of the SAME binary produce byte-identical
metrics — the scenario is fully deterministic under -FIXEDFPS, self-tested at 38 ragdolls x 61
frames. So any difference at all is signal, and 5% is slack against a future non-determinism rather
than against measurement noise. Re-run the self-test before widening it.

  reference (legacy-karma)   min gap 0.092 / worst 0.029    twist range 118.2 / worst 171.2
  recovered                  min gap 0.057 / worst 0.012    twist range 138.3 / worst 179.6
"""
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ragdoll_metrics import load, analyse            # noqa: E402

TOL = 0.05


def summarise(path):
    rag = load(path)
    rows = [r for r in (analyse(v) for v in rag.values()) if r]
    if not rows:
        return None
    n = len(rows)
    return dict(n=n,
                gap=sum(r['mingap'] for r in rows) / n,
                gapw=min(r['mingap'] for r in rows),
                twist=sum(r['twist'] for r in rows) / n,
                twistw=max(r['twist'] for r in rows))


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    ref, cand = summarise(sys.argv[1]), summarise(sys.argv[2])
    if ref is None or cand is None:
        # A ragdoll-free trace is not a pass. It is a scenario that did not run, and reporting it
        # as a pass is the "check that cannot fail" this project keeps being bitten by.
        print('NO RAGDOLLS in %s — the scenario did not run; this is not a pass' %
              (sys.argv[1] if ref is None else sys.argv[2]))
        return 3

    bad = []
    for key, label in (('gap', 'min gap'), ('twist', 'twist range')):
        r, c = ref[key], cand[key]
        if r and abs(c - r) / abs(r) > TOL:
            bad.append('%s %.4g vs %.4g (%+.0f%%)' % (label, c, r, 100.0 * (c - r) / r))
    print('ragdolls %d/%d  gap %.4f/%.4f  twist %.1f/%.1f  ->  %s' %
          (cand['n'], ref['n'], cand['gap'], ref['gap'], cand['twist'], ref['twist'],
           'MATCH' if not bad else 'DIFFERS: ' + '; '.join(bad)))
    return len(bad)


if __name__ == '__main__':
    sys.exit(main())
