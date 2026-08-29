#!/usr/bin/env python3
"""fix_align_masks.py — an ALIGNMENT MASK frozen at 32 bits.

    fix_align_masks.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT. A carve-up rounds each array up to a cache line by masking the low
bits off the cursor, and the mask is a literal:

    paiVar7 = (MdtKeaBodyIndexPair *)
              (maxPartitions * 4 + 0x3f + (kd_iptr)(piVar8 + maxPartitions)
               & 0xffffffc0);

`0xffffffc0` is `~63` — IN 32 BITS. It is an `unsigned int`, so at LP64 the
usual arithmetic conversions widen it with ZEROS to `0x00000000ffffffc0`, and
the AND does not round the pointer down, it CUTS THE TOP HALF OFF IT.

★ AND NOTHING ELSE IN THIS PROJECT CAN SEE IT. There is no truncating cast, so
`ptrwidth_check.sh` and `fix_ptrwidth.py` — which take their site list from
clang's own `pointer-to-int-cast` diagnostics — have nothing to report: the
expression's type is already `intptr_t` and stays `intptr_t`. No struct changes
size, so `layout_check.py` says nothing either. It is not a baked offset, not a
baked size and not a stride. The first evidence was a register dump:

    SEGV on unknown address 0x00004c1e01c0 ... WRITE
    rbx = 0x000000004c1e01c0   rdi = 0x000072b84c1e0134

the faulting address IS the low half of a live pointer.

⚠ AND IT SURFACES SOMEWHERE ELSE ENTIRELY. `MdtKeaConstraintsCreateFromChunk`
hands back an `MdtKeaConstraints` whose `Jbody` is that half-pointer; the report
lands two files away on `clist->Jbody[clist->num_constraints][0] = ...`, the
first statement of `MdtBclAddBSJoint`, which is correct code writing through a
pointer somebody else computed. Every baked offset in that statement was
repaired first, on the strength of the file and line in the report, and the
scene failed at exactly the same place afterwards. READ THE REGISTERS.

THE REPAIR IS ANCHOR-AND-CORRECT, the technique `LP64-STRIDE-AS-ADDR` and
`LP64-ARENA-CARVE` found, in its simplest form yet:

    0xffffffc0   ->   (0xffffffc0 | ~(kd_uptr)0xffffffffU)

`~(kd_uptr)0xffffffffU` is ZERO at 32-bit pointer width, so gcc folds the whole
thing back to the original literal and the i386 object cannot change; at LP64 it
is `0xffffffff00000000` and the mask becomes the 64-bit `~63` it was always
meant to be. The expression Ghidra wrote is untouched — only the constant in it
moves, which is what keeps the surrounding code identical.

WHICH MASKS. Two conditions, both measured rather than assumed:

  1. the literal's top 16 bits are all ones and it fits in 32 — the shape of
     `~(align - 1)`, and not the shape of `flags & 0xfffffffd`, which is a bit
     being CLEARED in an integer and is correct at every width;
  2. the value being masked is POINTER-DERIVED — it contains a `kd_iptr` or
     `kd_uptr` cast. That marker is not a guess either: `fix_ptrwidth.py` writes
     it, and only at the sites clang itself named as pointer-to-integer casts.
     So this pass runs AFTER that one.

⚠ CONDITION 2 IS THE WHOLE DISCRIMINATOR AND IT IS NARROWER THAN IT LOOKS.
`(void *)((kd_iptr)pMVar5 + (uVar8 & 0xfffffffc))` has a pointer in the
statement, a mask of the right shape, and a pointer cast around the result — and
is CORRECT: the mask is applied to `uVar8`, a count. The operand of the `&` is
what matters, not the statement it sits in, so the scan walks back from the `&`
to its own enclosing parenthesis and asks about THAT span alone.

Run it on a COPY: it edits in place. Every rewrite is compiled and compared
against the shipped i386 object, all-or-nothing per file with a bisect, exactly
as `fix_literal_offsets.py` does.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = '/tmp/kd_masks'

MASK = re.compile(r'&\s*(?P<lit>0x[0-9a-fA-F]{6,8})(?![0-9a-fA-FuUlL])')
PTR = re.compile(r'\bkd_[iu]ptr\b')
BANNER = re.compile(r'(?m)^/\* ---- (\S+)')
# A local this function declares at pointer width. `fix_narrow_pointers.py`
# widens these, and after it has run a bare `iVar3 + 0xf & 0xfffffff0` is a
# pointer expression with no cast in it to say so.
WIDE_DECL = re.compile(r'(?m)^[ \t]*kd_[iu]ptr[ \t]+(?P<name>[A-Za-z_]\w*)[ \t]*;')
CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']

REPAIR = '(%s | ~(kd_uptr)0xffffffffU)'


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def is_align_mask(lit):
    """`~(align - 1)` in 32 bits, and not a bit being cleared in an integer."""
    v = int(lit, 16)
    if v > 0xffffffff or (v & 0xffff0000) != 0xffff0000:
        return False
    align = (~v & 0xffffffff) + 1
    return align >= 2 and (align & (align - 1)) == 0


def operand_span(text, amp):
    """Where the `&`'s left operand begins.

    `&` binds looser than `+` and `*`, so the operand runs back to whatever
    parenthesis, statement or assignment encloses it — and NOT to the start of
    the statement, which is what makes `(kd_iptr)p + (n & MASK)` read correctly
    as a mask on `n`."""
    depth, i = 0, amp - 1
    while i >= 0:
        ch = text[i]
        if ch == ')':
            depth += 1
        elif ch == '(':
            if depth == 0:
                return i + 1
            depth -= 1
        elif depth == 0 and ch in ';{}=,?:':
            return i + 1
        i -= 1
    return 0


def region_of(text, pos):
    start, end = 0, len(text)
    for m in BANNER.finditer(text):
        if m.start() <= pos:
            start = m.start()
        elif m.start() > pos:
            end = m.start()
            break
    return text[start:end]


def sites(text):
    """Every 32-bit alignment mask applied to a pointer-derived value."""
    out = []
    for m in MASK.finditer(text):
        if not is_align_mask(m.group('lit')):
            continue
        operand = text[operand_span(text, m.start()):m.start()]
        if PTR.search(operand):
            out.append(m)
            continue
        # No cast in the operand — but a local DECLARED at pointer width says
        # the same thing, and after fix_narrow_pointers.py that is how the
        # allocator's cursor is spelled.
        wide = {d.group('name')
                for d in WIDE_DECL.finditer(region_of(text, m.start()))}
        if wide and any(re.search(r'\b%s\b' % re.escape(w), operand) for w in wide):
            out.append(m)
    return out


def compiles_identically(fn, text, build, inc):
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


def accept_edits(fn, text, edits, build, inc):
    """As many edits as keep the i386 object byte-identical; see fix_literal_offsets."""
    if not edits:
        return text, []
    cand = text
    for start, end, rep, _n in edits:            # descending by position
        cand = cand[:start] + rep + cand[end:]
    if compiles_identically(fn, cand, build, inc):
        return cand, edits
    if len(edits) == 1:
        return text, []
    mid = len(edits) // 2
    text, a = accept_edits(fn, text, edits[:mid], build, inc)
    text, b = accept_edits(fn, text, edits[mid:], build, inc)
    return text, a + b


def selftest(inc):
    """ASK THE COMPILER whether the rewrite is the no-op it claims to be.

    "Zero at i386 by construction" is a claim, and this project has recorded
    three times in one session that the compiler is the only thing that can
    check one. Both widths are built and the two mask values read back."""
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'selftest.c')
    open(src, 'w').write(
        '#include "%s/include/kd_compat.h"\n#include <stdio.h>\n'
        'int main(void){ printf("%%d %%llx %%llx\\n", (int)sizeof(void *),\n'
        '  (unsigned long long)(0xffffffc0),\n'
        '  (unsigned long long)(0xffffffc0 | ~(kd_uptr)0xffffffffU)); return 0; }\n'
        % HERE)
    got = {}
    for bits in ('-m32', '-m64'):
        exe = os.path.join(WORK, 'selftest' + bits)
        if subprocess.run(['gcc', bits, '-O2', '-DLINUX', '-w',
                           '-I' + os.path.join(HERE, 'include')] + includes(inc)
                          + ['-o', exe, src], capture_output=True).returncode:
            sys.exit('fix_align_masks: SELF-CHECK could not build at %s' % bits)
        r = subprocess.run([exe], capture_output=True, text=True)
        got[bits] = r.stdout.split()
    if got['-m32'] != ['4', 'ffffffc0', 'ffffffc0']:
        sys.exit('fix_align_masks: SELF-CHECK FAILED — at i386 the rewrite reads %r, '
                 'and it has to be a no-op there or the acceptance test is measuring '
                 'the wrong thing.' % (got['-m32'],))
    if got['-m64'] != ['8', 'ffffffc0', 'ffffffffffffffc0']:
        sys.exit('fix_align_masks: SELF-CHECK FAILED — at LP64 the rewrite reads %r '
                 '(want ffffffffffffffc0). It is not widening anything.'
                 % (got['-m64'],))


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else os.path.join(
        HERE, '..', 'Thirdparty', 'metoolkit')
    inc = os.path.join(root, 'include')
    selftest(inc)

    fixed = declined = 0
    notes = []
    seen_masks = 0
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text0 = open(path, errors='ignore').read()
        seen_masks += sum(1 for m in MASK.finditer(text0)
                          if is_align_mask(m.group('lit')))
        found = sites(text0)
        if not found:
            continue
        edits = [(m.start('lit'), m.end('lit'), REPAIR % m.group('lit'),
                  '%-26s %s widened to pointer width' % (fn, m.group('lit')))
                 for m in found]
        edits.sort(key=lambda e: -e[0])
        text, ok = accept_edits(fn, text0, edits, build, inc)
        fixed += len(ok)
        declined += len(edits) - len(ok)
        keep = {id(e) for e in ok}
        by_file = {}
        for e in edits:
            k = e[3] + ('' if id(e) in keep
                        else '   DECLINED: not byte-identical at i386')
            by_file[k] = by_file.get(k, 0) + 1
        for k, n in sorted(by_file.items()):
            notes.append('%s  x%d' % (k, n))
        if text != text0:
            open(path, 'w').write(text)

    # ---- A SEARCH THAT COMES BACK EMPTY HAS TO SAY WHETHER IT CAN FIND
    # ANYTHING AT ALL. There are alignment-shaped masks all over this corpus and
    # almost none of them are on a pointer; if the pointer-derived test ever
    # stops matching, "0 rewritten" is indistinguishable from "nothing to do".
    if seen_masks and not fixed and not declined:
        sys.exit('fix_align_masks: %d alignment-shaped mask(s) in the corpus and NOT '
                 'ONE of them read as pointer-derived. Either fix_ptrwidth.py has not '
                 'run yet (it writes the kd_iptr this keys on) or the test is broken.'
                 % seen_masks)

    print('  32-bit alignment masks widened  : %d' % fixed)
    print('  declined (reported, not guessed): %d' % declined)
    for n in notes:
        print('     %s' % n)
    return 0


if __name__ == '__main__':
    sys.exit(main())
