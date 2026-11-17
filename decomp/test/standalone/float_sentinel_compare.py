#!/usr/bin/env python3
"""float_sentinel_compare.py <ref.txt> <c32.txt> <c64.txt> — the verdict for float_sentinel.sh.

Each line is `<case> <ret> <defined mask> <v0> ... <v7>` (floats as bit patterns; mask bit 0 = ret,
bit 1+k = value k, set when two poisoned runs agreed). Per case and candidate:

  - ret must equal the reference's;
  - every value the reference defines must be defined in the candidate and within 1e-3 of it (x87
    against SSE; the cases are exact on axis-aligned spheres, so this only absorbs rounding);
  - c64 must equal c32 bit for bit in every value either defines (only the pointer width differs).
"""
import struct
import sys


def load(path):
    rows = {}
    for line in open(path):
        if line.startswith('#') or not line.strip():
            continue
        p = line.split()
        if p[1] == 'skip-lp64':
            rows[p[0]] = None
            continue
        vals = [struct.unpack('<f', struct.pack('<I', int(x, 16)))[0] for x in p[3:]]
        rows[p[0]] = (int(p[1]), int(p[2], 16), vals, p[3:])
    return rows


def close(a, b):
    if a == b or (a != a and b != b):
        return True
    return abs(a - b) <= 1e-3


def main():
    ref, c32, c64 = (load(p) for p in sys.argv[1:4])
    bad = skipped = 0
    for case in ref:
        rret, rmask, rv, _ = ref[case]
        verdicts = []
        for name, cand in (('c32', c32), ('c64', c64)):
            if case not in cand:
                verdicts.append(f'{name} MISSING')
                bad += 1
                continue
            if cand[case] is None:
                # McdSpace's line-segment walks fault at LP64 (a pointer-width defect of their own,
                # docs/STATE.md); the case says so instead of passing.
                verdicts.append(f'{name} NOT RUN (McdSpace faults at LP64)')
                skipped += 1
                continue
            cret, cmask, cv, _ = cand[case]
            why = []
            if (rmask & 1) and cret != rret:
                why.append(f'ret {cret} (ref {rret})')
            for k, (a, b) in enumerate(zip(rv, cv)):
                if not rmask & (2 << k):
                    continue
                if not cmask & (2 << k):
                    why.append(f'v{k} undefined (ref {a:g})')
                elif not close(a, b):
                    why.append(f'v{k} {b:g} (ref {a:g})')
            if why:
                bad += 1
                verdicts.append(f'{name} DIFFERS: ' + ', '.join(why))
            else:
                verdicts.append(f'{name} ok')
        if c32.get(case) and c64.get(case):
            m32, m64 = c32[case][1], c64[case][1]
            if c32[case][0] != c64[case][0]:
                bad += 1
                verdicts.append('c64 != c32 (ret)')
            for k in range(len(c32[case][3])):
                if (m32 | m64) & (2 << k) and c32[case][3][k] != c64[case][3][k]:
                    bad += 1
                    verdicts.append(f'c64 != c32 (v{k})')
                    break
        print(f'  {case:24s} ref ret {rret:2d}   ' + '; '.join(verdicts))
    print(f'  {len(ref)} case(s), {bad} difference(s), {skipped} candidate run(s) not made')
    print('  -> PASS' if bad == 0 else '  -> FAIL')
    return 0 if bad == 0 else 1


if __name__ == '__main__':
    sys.exit(main())
