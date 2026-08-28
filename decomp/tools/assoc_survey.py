#!/usr/bin/env python3
"""assoc_survey.py — our float-chain order beside the x86-64 original's, site by site.

    assoc_survey.py <sites.txt> <recovered-allobj> <amd64-objdir> [-o OUT]

WHAT IT IS FOR. tools/assoc_scan.py finds 575 places a float `+` chain could have been
re-associated and its own docstring says "it decides nothing": Ghidra decompiles the i386 build,
where x87's 80-bit registers make `a+(b+c)` and `(a+b)+c` EXACTLY equal, so the binary carries no
evidence of which the source wrote. The x86-64 build has no x87 — every add is an explicit `addss`
in a fixed order — so it is the only place the association is written down.

⚠ IT IS A RECORD, NOT A REPAIR LIST, AND THAT DISTINCTION COST TWO NEAR-MISSES. Having read
`2,1,0` out of MeVector3Dot and MeMatrix4TMInverseTransform, a blanket "reorder every chain to
2,1,0" looked obviously right. It is wrong three times over:

  MeMatrix4TMInverseRotate   row 0 is 2,1,0 and rows 1 and 2 are 2,0,1 — the order differs per ROW
                             of one function, because it is instruction scheduling, not source
  McdGjkMaximumPoint         our two live sites are already 2,0,1 and already correct
  MeQuaternionFromTM         i386 sums the trace 1,0,2 and amd64 sums it 0,1,2 — the two SHIPPED
                             builds disagree, so there is no "the original's order" to match

So the criterion is narrow: act only where ours matches NEITHER shipped build. Where they disagree
with each other, record it and leave the code alone — our recovery is derived from i386 and
changing it to MSVC's schedule is a coin toss dressed as a fix.

⚠ AND THERE IS CURRENTLY NO ACCEPTANCE TEST FOR THIS CLASS. wasm reproduces i386 to 0.007 degrees
on test-karma-1, so an association edit moves us away from the only reference that can be measured,
toward one that cannot be run here. Do not batch-apply this file. proven.txt records the same
lesson from the other direction: reversing 41 flat chains in one object changed the object and
changed nothing measurable.

OUTPUT per site: the recovered expression, the operand order read out of it, and the float
instruction sequence of the enclosing function in the amd64 build, so the reader can audit the
claim rather than take it.
"""
import os
import re
import subprocess
import sys

FLOAT_OP = re.compile(r'^\s*[0-9a-f]+:\s+((?:mul|add|sub|div)ss|movss|sqrtss)\s+(.*)$')
DISP = re.compile(r'(-?0x[0-9a-f]+|\b0\b)\(')


def our_order(expr):
    """The subscript order of a 3-term chain -> ('2,0,1', frozenset({'2','0'})).

    ★ THE PAIR IS THE ANSWER, NOT THE SEQUENCE. IEEE addition is COMMUTATIVE — only associativity
    fails — so `(a2 + a1) + a0` and `(a1 + a2) + a0` are bit-identical and only the GROUPING
    matters. The first version of this tool compared the literal left-to-right order and made its
    very first site look like a mismatch: ours reads 2,1,0 and the amd64 reads 1,2,0, which are the
    same computation. For a three-term chain there are exactly three distinct groupings — {0,1},
    {0,2}, {1,2} — and that is the whole question.
    """
    terms = [t.strip() for t in re.split(r'\+(?![^(]*\))', expr)]
    if len(terms) != 3:
        return None, None
    idx = []
    for t in terms:
        nums = re.findall(r'\[(\d+)\]|\[(0x[0-9a-f]+)\]', t)
        flat = [a or b for a, b in nums]
        if not flat:
            return None, None
        idx.append(str(int(flat[-1], 0)))
    return ','.join(idx), frozenset(idx[:2])


def amd64_first_pair(ops):
    """Which two products the x86-64 code adds FIRST, as index positions.

    Tracks which xmm register each `mulss <disp>(%reg)` result lives in, then reads the first
    `addss %src,%dst` as the pair. Displacements are mapped to 0/1/2 by dividing by 4 and taking
    the value mod 3 of the row — good enough for the matrix-row and dot-product shapes these sites
    are, and it RETURNS None rather than guessing on anything else.
    """
    holds = {}
    for op in ops:
        m = re.match(r'mulss\s+(-?0x[0-9a-f]+|\b0\b)?\(?[^,]*,\s*(%xmm\d+)', op)
        if m and m.group(1) is not None:
            holds[m.group(2)] = int(m.group(1), 0)
            continue
        m = re.match(r'addss\s+(%xmm\d+),\s*(%xmm\d+)', op)
        if m:
            a, b = holds.get(m.group(1)), holds.get(m.group(2))
            if a is None or b is None:
                return None
            return frozenset(str((v // 4) % 3) for v in (a, b))
    return None

def amd64_float_ops(objdir, obj, fn, limit=26):
    path = os.path.join(objdir, obj + '.obj')
    if not os.path.exists(path):
        return ['(no amd64 object for %s)' % obj]
    dis = subprocess.run(['objdump', '-d', '--no-show-raw-insn', path],
                         capture_output=True, text=True).stdout
    # COMDAT: each function is its own section, all based at 0.
    blocks = re.split(r'\n(?=[0-9a-f]{16} <)', dis)
    want = [b for b in blocks if re.match(r'^[0-9a-f]{16} <[^>]*%s[^>]*>:' % re.escape(fn), b)]
    if not want:
        return ['(function %s not found in the amd64 object)' % fn]
    ops = []
    for line in want[0].splitlines():
        m = FLOAT_OP.match(line)
        if m:
            ops.append('%-8s %s' % (m.group(1), m.group(2).strip()))
        if len(ops) >= limit:
            ops.append('... (truncated)')
            break
    return ops


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    out = sys.stdout
    if '-o' in sys.argv:
        out = open(sys.argv[sys.argv.index('-o') + 1], 'w')
    if len(args) < 3:
        print(__doc__)
        return 2
    sites, rec, a64 = args[0], args[1], args[2]

    print('# association survey — recovered order vs the x86-64 original', file=out)
    print('# generated by karma-decomp/tools/assoc_survey.py; READ ITS DOCSTRING BEFORE ACTING\n',
          file=out)
    n = 0
    diff, unread = [], []
    for line in open(sites):
        line = line.rstrip('\n')
        if not line.strip() or line.startswith('#'):
            continue
        obj, fn, expr = line.split('\t', 2)
        n += 1
        print('=' * 96, file=out)
        print('%s : %s' % (obj, fn), file=out)
        print('  ours   %s' % expr.strip(), file=out)
        o, pair = our_order(expr)
        ops = amd64_float_ops(a64, obj, fn)
        theirs = amd64_first_pair(ops)
        print('  order  %s' % (o if o else '(not a plain 3-term chain — read it by hand)'), file=out)
        if pair and theirs:
            verdict = 'SAME GROUPING' if pair == theirs else '*** DIFFERENT GROUPING ***'
            print('  pair   ours {%s}   amd64 {%s}   -> %s'
                  % (','.join(sorted(pair)), ','.join(sorted(theirs)), verdict), file=out)
            if pair != theirs:
                diff.append((obj, fn, expr.strip(), sorted(pair), sorted(theirs)))
        else:
            print('  pair   (could not be read mechanically — audit the ops below by hand)', file=out)
            unread.append((obj, fn))
        print('  amd64  float ops of the enclosing function:', file=out)
        for op in ops:
            print('           %s' % op, file=out)
        print(file=out)
    print('=' * 96, file=out)
    print('%d site(s) surveyed.' % n, file=out)
    print('%d with a DIFFERENT grouping from the x86-64 build:' % len(diff), file=out)
    for o, f, e, a, b in diff:
        print('   %-22s %-30s ours {%s} amd64 {%s}' % (o, f, ','.join(a), ','.join(b)), file=out)
    print('%d could not be read mechanically and need a hand audit:' % len(unread), file=out)
    for o, f in unread:
        print('   %-22s %s' % (o, f), file=out)
    if out is not sys.stdout:
        out.close()
    return 0


if __name__ == '__main__':
    sys.exit(main())
