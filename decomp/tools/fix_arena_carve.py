#!/usr/bin/env python3
"""fix_arena_carve.py — one allocation carved into arrays with baked strides.

    fix_arena_carve.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT. `MdtPartOutCreateFromChunk` asks for one block and cuts it into
seven arrays with a cursor:

    pMVar1 = MeChunkGetMem(chunk, maxBodies * 0x20 + 0x40 + maxConstraints * 4);
    pppMVar2 = &pMVar1[1].constraints + maxBodies;      /* MdtBaseConstraint *** */
    pMVar1->bodiesSize = (int *)pppMVar2;
    pppMVar2 = pppMVar2 + maxBodies;                    /* steps sizeof(ptr) */

Three things are frozen at i386 and every one of them is wrong at LP64:

    0x40  is sizeof(MdtPartitionOutput)            64 here,  96 there
    0x20  is the per-body total                    32 here,  36 there
          (bodiesStart, bodiesSize, constraintsStart, constraintsSize are
           `int`; `bodies` is a POINTER array, which is what grows)
    the cursor's element is POINTER-sized, so `+ maxBodies` steps 4 here and
    8 there — correct for the one array of pointers it walks over, and wrong
    for the four arrays of `int`.

Every downstream `po->bodies[i]` and `po->info[i].rowCount` is CORRECT CODE
indexing arrays that overlap each other, which is why the ASan reports land in
`MdtUpdatePartitions` and `MdtPackAllPartitions` with nothing pointing here.

★ ANCHOR-AND-CORRECT IS WHAT MAKES THIS BYTE-IDENTICAL, and this file exists
partly to record that it IS. `STATE.md` and `proven.txt` both said this class
would "almost certainly not" survive the byte-identity gate because the repair
changes arithmetic rather than re-spelling it. That was never measured and it is
wrong — the third such wrong prediction in this session. Rewriting each
expression outright does fail: `info` and `constraints` share the subexpression
`(pppMVar2 + maxBodies)`, and rewriting both changes what gcc common-
subexpression-eliminates, so five of the six edits compose and the sixth breaks
it. Keeping the ORIGINAL expression verbatim and adding a delta that is ZERO at
i386 —

    (char *)(pppMVar2 + maxBodies) + maxBodies * (int)(sizeof(int) - sizeof(T *))

— leaves the shared subexpression intact, folds to nothing here, and corrects
the stride everywhere else. All six edits then compose byte-identically.

⚠ SCOPE, AND IT IS DELIBERATELY NARROW. Six `MeChunkGetMem` carve-ups exist:
this one, `MdtKeaConstraintsCreateFromChunk` (MdtMainLoop.c:942) and four in
MdtWorld.c. Only this one is repaired. The others are NOT the same shape —
MdtMainLoop's accumulates a dozen sizes across thirty lines with alignment
rounding and branches before it allocates — and a rule general enough to cover
them is a static analysis, not a pattern match. The derivable part of the rule
is stated below for whoever writes it.

THE RULE, for when someone generalises this: `X->field = (T *)cursor;` NAMES the
element type of the array at the cursor, so the next advance of that cursor must
be `count * sizeof(T)`. Every element type in a carve-up is written down in the
cast that consumes it; nothing has to be guessed. The TOTAL is then the sum of
those, plus the header struct.

Each edit below is keyed to the EXACT source text and applied only if it still
compiles byte-identically, so a regenerated source that no longer matches
declines loudly instead of half-applying. Run it on a COPY: it edits in place.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = '/tmp/kd_arena'

CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']

# The cursor walks `MdtBaseConstraint **` elements; every array it steps over
# that is really `int` needs this correction, and it is zero at i386.
D = 'maxBodies * (int)(sizeof(int) - sizeof(MdtBaseConstraint **))'
ANCHOR = '(MdtBaseConstraint ***)((char *)(%s) + ' + D + ')'

EDITS = {
    'MdtPartition.c': [
        # the block total: header + per-body arrays + the constraint pointers
        ("  pMVar1 = MeChunkGetMem(chunk,maxBodies * 0x20 + 0x40 + maxConstraints * 4);",
         "  pMVar1 = MeChunkGetMem(chunk,maxBodies * (int)(4 * sizeof(int) + sizeof(MdtBody *)"
         " + sizeof(MdtPartitionInfo)) + (int)sizeof(MdtPartitionOutput)"
         " + maxConstraints * (int)sizeof(MdtBaseConstraint *));"),
        # cursor start, then the advances over the four `int` arrays. The advance
        # over `bodies` is NOT corrected: that array really is pointer-sized, so
        # the cursor's own stride is already right for it.
        ("  pppMVar2 = &pMVar1[1].constraints + maxBodies;",
         "  pppMVar2 = " + ANCHOR % "&pMVar1[1].constraints + maxBodies" + ";"),
        ("  pMVar1->bodiesSize = (int *)pppMVar2;\n"
         "  pppMVar2 = pppMVar2 + maxBodies;",
         "  pMVar1->bodiesSize = (int *)pppMVar2;\n"
         "  pppMVar2 = " + ANCHOR % "pppMVar2 + maxBodies" + ";"),
        ("  pMVar1->constraintsStart = (int *)pppMVar2;\n"
         "  pppMVar2 = pppMVar2 + maxBodies;",
         "  pMVar1->constraintsStart = (int *)pppMVar2;\n"
         "  pppMVar2 = " + ANCHOR % "pppMVar2 + maxBodies" + ";"),
        ("  pMVar1->info = (MdtPartitionInfo *)(pppMVar2 + maxBodies);",
         "  pMVar1->info = (MdtPartitionInfo *)((char *)(pppMVar2 + maxBodies) + " + D + ");"),
        ("       (MdtBaseConstraint **)((MdtPartitionInfo *)(pppMVar2 + maxBodies) + maxBodies);",
         "       (MdtBaseConstraint **)((MdtPartitionInfo *)((char *)(pppMVar2 + maxBodies) + "
         + D + ") + maxBodies);"),
    ],
}


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def compiles_identically(fn, text, build, inc):
    """⚠ The file must keep its NAME: gcc records the source basename in an
    STT_FILE symbol, so identical text under a scratch name is a different
    object and reads exactly like a codegen change."""
    ref = os.path.join(build, fn[:-2] + '.o')
    if not os.path.exists(ref):
        return False
    d = os.path.join(WORK, 'ident')
    os.makedirs(d, exist_ok=True)
    src = os.path.join(d, fn)
    open(src, 'w').write(text)
    obj = os.path.join(d, fn[:-2] + '.probe.o')
    if subprocess.run(['gcc'] + CFLAGS + ['-I' + os.path.join(HERE, 'include')]
                      + includes(inc) + ['-c', '-o', obj, src],
                      capture_output=True).returncode:
        return False
    return open(ref, 'rb').read() == open(obj, 'rb').read()


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else os.path.join(
        HERE, '..', 'Thirdparty', 'metoolkit')
    inc = os.path.join(root, 'include')

    fixed = declined = 0
    notes = []
    for fn, edits in sorted(EDITS.items()):
        path = os.path.join(srcdir, fn)
        if not os.path.exists(path) or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        text = open(path, errors='ignore').read()
        # ---- ALL OR NOTHING. A half-carved arena is worse than an uncarved
        # one: the arrays would overlap differently rather than not at all, and
        # the byte-identity check would still pass on each piece.
        missing = [a for a, _ in edits if a not in text]
        if missing:
            declined += len(edits)
            notes.append('%-22s SOURCE HAS MOVED — %d of %d anchors not found; '
                         'not applying any' % (fn, len(missing), len(edits)))
            continue
        cand = text
        for a, b in edits:
            cand = cand.replace(a, b, 1)
        if compiles_identically(fn, cand, build, inc):
            open(path, 'w').write(cand)
            fixed += len(edits)
            notes.append('%-22s %d edit(s), arena strides derived from the '
                         'element types' % (fn, len(edits)))
        else:
            declined += len(edits)
            notes.append('%-22s DECLINED: the carve-up is not byte-identical '
                         'at i386' % fn)

    print('  arena carve-up edits applied       : %d' % fixed)
    print('  declined (reported, not guessed)   : %d' % declined)
    for n in notes:
        print('     %s' % n)
    print('  -> NOT DONE: MdtKeaConstraintsCreateFromChunk (MdtMainLoop.c) and')
    print('     four carve-ups in MdtWorld.c. Different shape; see this file.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
