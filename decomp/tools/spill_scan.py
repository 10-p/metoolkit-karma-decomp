#!/usr/bin/env python3
"""spill_scan.py — float locals Ghidra DECLARED and then never used, where the
shipped code both STORES to their slot and READS IT BACK.

    spill_scan.py <kd_out/allobj> <kd_build> <shipped-obj-dir>

WHAT THIS IS AND WHY IT IS NOT COSMETIC.

`restore_float_text`'s 'spill' shape: gcc 3.2 keeps a float intermediate in an
x87 register at 80 bits, spills it to a 4-byte stack slot — WHICH ROUNDS IT —
and reloads it. Ghidra folds that store/reload pair into one expression, so the
recovered C carries 80 bits where the shipped code carried 32, and the last bit
drifts. The variable Ghidra declared for the slot is left behind, DECLARED AND
NEVER USED, and that is the textual fingerprint.

IT IS NOT A ROUNDING CURIOSITY. On 2026-08-27 three of them in
`McdVanillaSegmentCylinderIntersect` — `tEnterNum`, `tExitNum`, `tExitNumZ` —
were the whole of `IxCylinderTriList`'s live-match defect: 37 `count_diff` in
153,391 real calls, reproducing offline under `KD_CORNER=1` as count 30/32.
Restoring the three roundings took `difftest_pair` from FAIL to PASS and the
per-function A/B from 1 differing return in 885,855 calls to ZERO. A dropped
rounding changes a DISCRETE DECISION, on i386, in a shipped product.

WHY A TEXTUAL SCAN ALONE WOULD BE USELESS. 316 float locals in the corpus are
declared and unused, and most are not spills: Ghidra also emits locals from the
DWARF that the optimised code never materialised at all, and locals it kept
purely in registers. So every candidate is CHECKED against the shipped
disassembly — the slot must show both an f32 store and a read-back — using the
same `_f32_spill_slots` evidence `restore_float_text` demands before it will
apply a repair. What survives that is a measured list, not a grep.

WHAT THE OUTPUT IS FOR. Each surviving row is a site where the recovered code
is arithmetically wrong by one rounding. Repair them through
`ghidra_clean.FLOAT_TEXT_REPAIRS` with `kind='spill'`, which re-checks this same
evidence at generation time and RAISES if a re-dump moves it.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ghidra_clean as gc                                   # noqa: E402

BANNER = re.compile(r'(?m)^/\* ---- (\S+)')
DECL = re.compile(r'(?m)^[ \t]*(?:const\s+)?(?:float|MeReal|double)[ \t]+'
                  r'([A-Za-z_]\w*)[ \t]*(\[[^\]]*\])?[ \t]*;')


def main():
    srcdir, build, shipdir = sys.argv[1], sys.argv[2], sys.argv[3]
    tot = cand = 0
    rows = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c'):
            continue
        base = fn[:-2]
        inbuild = os.path.exists(os.path.join(build, base + '.o'))
        obj = os.path.join(shipdir, base + '.o')
        if not os.path.exists(obj):
            continue
        text = open(os.path.join(srcdir, fn), errors='ignore').read()
        dump = os.path.join(os.path.dirname(srcdir.rstrip('/')), '..')
        locals_table = {}
        lp = os.path.join('/home/ion/tools/karma-lab/out14', base + '.o.locals')
        if os.path.exists(lp):
            locals_table = gc.read_ghidra_locals(
                os.path.join('/home/ion/tools/karma-lab/out14', base + '.o.c'))
        parts = BANNER.split(text)
        for i in range(1, len(parts), 2):
            func, region = parts[i], parts[i + 1]
            unused = [m.group(1) for m in DECL.finditer(region)
                      if len(re.findall(r'(?<![\w])' + re.escape(m.group(1)) + r'\b',
                                        region)) == 1]
            if not unused:
                continue
            cand += len(unused)
            locs = locals_table.get(func, {})
            stored, loaded = gc._f32_spill_slots(obj, func)
            if not stored or not loaded:
                continue
            for name in unused:
                loc = locs.get(name)
                if loc is None:
                    continue
                off, size, _ty = loc
                # PARAMETERS ARE NOT SPILLS. `.locals` offsets are relative to
                # the first parameter slot, so a parameter has off >= 0. Its
                # "store and reload" is gcc copying an incoming float to the
                # frame — the value arrived at 32 bits and rounding it again is
                # a no-op. Counting those inflated the first run of this scan
                # from 155 to 231 and put `eps` and `fatness` at the top of the
                # list, which is what made the filter obvious.
                if off >= 0:
                    continue
                lo, hi = off + 4, off + 4 + size      # .locals is param-relative
                if any(lo <= o < hi for o in stored) and \
                   any(lo <= o < hi for o in loaded):
                    tot += 1
                    rows.append((base, func, name, lo, inbuild))

    inb = [r for r in rows if r[4]]
    print('  float locals declared and never used            : %d' % cand)
    print('  of those, CONFIRMED spills (stored AND reloaded): %d' % tot)
    print('  ... in objects that are IN THE BUILD            : %d' % len(inb))
    by = {}
    for b, f, n, o, ib in inb:
        by.setdefault(b, []).append('%s:%s' % (f, n))
    for b in sorted(by, key=lambda k: -len(by[k]))[:12]:
        print('    %-26s %2d   %s' % (b, len(by[b]), ', '.join(by[b][:3])
                                      + (' ...' if len(by[b]) > 3 else '')))
    print('  -> each row is a slot the shipped code ROUND-TRIPPED THROUGH 32-BIT')
    print('     MEMORY and the recovery keeps in an 80-bit register. It changed a')
    print('     CONTACT COUNT in IxCylinderTriList; it is not cosmetic.')
    print('  AND IT IS STILL AN UPPER BOUND. A slot can round-trip without the')
    print('  recovered expression having dropped anything — the decisive test is')
    print('  per-site: apply KD_F32 and see whether the .o CHANGES. Byte-identical')
    print('  means the rounding was already there.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
