#!/usr/bin/env python3
"""frame_offsets.py — what does Ghidra say lives at the offset a `stack0x` names?

    frame_offsets.py <dump-dir> [object ...]

Ghidra spells a stack slot's ADDRESS `&stack0xffffff6c` and does not declare it.
`ghidra_clean.fix_stack_address_name` resolves that to the local `in_stack_ffffff6c`
on the grounds that both names encode the same frame offset — which was an
assumption about how Ghidra builds names, not a checked fact.

`DumpDecomp.java` now emits `<object>.locals` beside each dump: one row per
decompiled local that has stack storage, with the offset Ghidra ASSIGNED it,
read out of `HighFunction.getLocalSymbolMap()`. This tool joins the two, so
every `stack0x` site in a dump can be answered from Ghidra's own symbol map
instead of from its naming convention.

WHAT IT ESTABLISHED, 2026-08-25, over every `stack0x` site in the corpus:

  * The assumption is EXACT. Every site that has a covering local resolves to
    `in_stack_<same offset>`, at that offset, with no exceptions. The rule was
    right; now it is checked.
  * The sites that remain are NOT naming problems, and the table says which
    kind each one is:
      - SIZE: a covering local exists and is too small for what the copy loop
        writes. `MdtWorld`'s ffffff6c is `MeReal` (4 bytes) against a 76-byte
        copy, because Ghidra split ONE outgoing argument block into eight
        locals — in_stack_ffffff6c, in_stack_ffffff70[48] and six scalars that
        it also uses as ordinary variables. Same shape in
        `keaRbdCore_unified` (72 declared, 92 written).
      - NO LOCAL AT ALL: `MdtBcl` and `MeMath` at offset -12, `keaLCP_new` at
        -76 and -72. For these the base is an anchor, and what it points into
        is found by adding the displacements the text shows.

  And it settled two long-standing judgement calls with evidence:

  * `MeMath` CANNOT be repaired. `&stack0xfffffff4 + -0x30, -0x2c, -0x28`
    striding 0xc is exactly the nine elements of `MeReal eR[3][3]`, which the
    table puts at -60. So the mapping is exact — and `eR` is DECLARED AND NEVER
    WRITTEN in the dump, because Ghidra emitted `fcos`/`fsin` and discarded
    their results. Repairing the name would make it compile and read
    uninitialised memory. The refusal is now evidenced rather than argued.
  * `MdtBcl` COULD be. Its accesses land inside `MeReal ref2world[4][4]` at
    -156, which the line above the loop fills via `MeMatrix4MultiplyMatrix`.
    Not attempted here: the base points 144 bytes into a 64-byte array, so a
    base-level rewrite is what `check_frame_bounds.py` exists to reject, and an
    access-level rewrite needs the loop trip count. One object, in the tail.

AND WHAT `--cover` ESTABLISHED, which is the bigger one: THE SOLVER FRAMES ARE
NOT UNDESCRIBED. They are described and FRAGMENTED. Every `stack0x` site in the
solver objects sits at the start of a CONTIGUOUS run of declared locals that
covers at least what the copy loop writes:

    object / site              copy writes   contiguous described
    MdtWorld     ffffff6c          76 B      80 B in  9 locals
    MdtWorld     ffffff04          92 B      92 B in  1
    keaRbdCore   fffffdd8          92 B     176 B in 20 locals
    keaRbdCore   fffffe2c           —        92 B in 16
    keaIntegrate ffffffa0          76 B      76 B in  1
    keaMemory    ffffff88          92 B      92 B in  1

`fix_stack_address_name` declines on the fragmented ones because it size-checks
ONE local — `MdtWorld`'s ffffff6c is a 4-byte `MeReal` — and it is right to,
because writing 76 bytes into it would be a 72-byte overflow. But the missing 72
bytes are not missing: they are `in_stack_ffffff70[48]` and six scalars Ghidra
also uses as ordinary variables, in that order, with no gap.

So the repair is an OVERLAY, not a reconstruction: alias the contiguous run into
one block so that the address `&stack0xNNNN` and every `in_stack_*` read see the
same storage. Offsets, sizes and types all come from this table, so nothing is
inferred. What has to be got right, and is the reason it is not done here: the
run includes locals the decompiler also treats as SSA VALUES (`uVar18`,
`pMVar19`), and giving them aliased storage is only faithful if Ghidra really
means they live at that offset for the whole function. Validate on `MdtWorld` —
non-kea, and `scene_chain` drives `MdtWorldStep` where a wrong answer shows.
"""
import argparse
import collections
import glob
import os
import re
import sys

FUNC_SPLIT = re.compile(r'/\* ==== (\w+) ==== \*/')
STACK_REF = re.compile(r'stack0x([0-9a-f]{8})')


def signed32(v):
    return v - (1 << 32) if v >= 0x80000000 else v


def read_locals(path):
    """{function: [(offset, name, size, type), ...]}"""
    out = collections.defaultdict(list)
    if not os.path.exists(path):
        return out
    with open(path) as f:
        for line in f:
            if line.startswith('#'):
                continue
            parts = line.rstrip('\n').split(',', 5)
            if len(parts) != 6:
                continue
            fn, name, off, _hexoff, size, ty = parts
            try:
                out[fn].append((int(off), name, int(size), ty))
            except ValueError:
                continue
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('dumpdir')
    ap.add_argument('objects', nargs='*')
    ap.add_argument('--quiet', action='store_true',
                    help='only print sites with no covering local, or a size problem')
    ap.add_argument('--cover', action='store_true',
                    help='for each site, print the CONTIGUOUS RUN of locals starting '
                         'there and the cumulative size — which answers whether the '
                         'frame is undescribed or merely fragmented')
    args = ap.parse_args()

    dumps = sorted(glob.glob(os.path.join(args.dumpdir, '*.c')))
    if args.objects:
        want = set(args.objects)
        dumps = [d for d in dumps
                 if os.path.basename(d)[:-4] in want
                 or os.path.basename(d).split('.')[0] in want]

    if not any(os.path.exists(d[:-2] + '.locals') for d in dumps):
        print(f'{args.dumpdir}: no .locals files. That table is emitted by\n'
              '  DumpDecomp.java, so a dump directory made before it was added\n'
              '  has none — re-dump, or point at one that does. Nothing here can\n'
              '  be answered from the .c alone, which is the whole point.',
              file=sys.stderr)
        return 2

    n_exact = n_nolocal = n_small = 0
    for c in dumps:
        locs = read_locals(c[:-2] + '.locals')
        parts = FUNC_SPLIT.split(open(c).read())
        rows = []
        for i in range(1, len(parts), 2):
            fn, body = parts[i], parts[i + 1]
            seen = set()
            for m in STACK_REF.finditer(body):
                if m.group(1) in seen:
                    continue
                seen.add(m.group(1))
                off = signed32(int(m.group(1), 16))
                cover = [l for l in locs.get(fn, []) if l[0] <= off < l[0] + l[2]]
                exact = [l for l in cover if l[0] == off]
                rows.append((fn, m.group(1), off, exact[0] if exact else
                             (cover[0] if cover else None), bool(exact)))
        if not rows:
            continue
        printed = False
        for fn, hx, off, cov, is_exact in rows:
            if cov is None:
                n_nolocal += 1
                kind, desc = 'NO LOCAL ', 'nothing declared covers this offset'
            else:
                if is_exact:
                    n_exact += 1
                kind = 'exact    ' if is_exact else 'inside   '
                desc = f'{cov[1]} ({cov[3]}, {cov[2]}B at {cov[0]})'
            if args.quiet and cov is not None and is_exact:
                continue
            if not printed:
                print(f'== {os.path.basename(c)[:-4]}')
                printed = True
            print(f'   {fn:34s} stack0x{hx} ({off:6d})  {kind}  {desc}')
            if args.cover and cov is not None:
                run, total, at = [], 0, off
                for l in sorted(locs.get(fn, [])):
                    if l[0] < at:
                        continue
                    if l[0] != at:
                        break          # a gap: the frame really is undescribed here
                    run.append(l)
                    total += l[2]
                    at += l[2]
                for l in run:
                    print(f'        +{l[0]-off:<4d} {l[1]:<24s} {l[2]:>4d}B  {l[3]}')
                print(f'        = {total} contiguous byte(s) described from this '
                      f'offset, in {len(run)} local(s)')

    print(f'\n{n_exact} site(s) resolve to a local at exactly that offset, '
          f'{n_nolocal} have no covering local.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
