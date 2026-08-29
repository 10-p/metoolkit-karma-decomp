#!/usr/bin/env python3
"""fix_frame_slots.py — GHIDRA'S INVENTED STACK FRAME, addressed in 4-byte words.

    fix_frame_slots.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT. The i386 code pushes its arguments, so Ghidra fabricates a local to
hold the outgoing argument area and writes each word into it at a constant byte
offset — then reads the very same offsets back as the call's arguments:

    int aiStack_9cb0 [6];
    ...
    *(McdBatchContext **)((kd_iptr)aiStack_9cb0 + 0x14) = b->context;
    *(int **)          ((kd_iptr)aiStack_9cb0 + 0x10) = &resultCount;
    ...
    arrayFinished = McdBatchIntersectEach(*(void **)((kd_iptr)aiStack_9cb0 + 4),
                                          *(void **)((kd_iptr)aiStack_9cb0 + 8), ...);

Every offset is an argument WORD — 0, 4, 8, 0xc, 0x10, 0x14 — and every word is
four bytes because that is what a pointer weighed on the shipped target. At LP64
the words are eight: slot 4 and slot 5 overlap, and the write to slot 5 runs four
bytes off the end of a 24-byte array. That is `MstUtils.c:91`, and `McdBatch.c:829`
is its consequence one call deeper — `*resultCount = 0` through a pointer that
arrived in a clobbered slot.

★ AND IT IS BYTE-IDENTICAL AT i386, WHICH THIS PROJECT PREDICTED IT COULD NOT BE.
`proven.txt` LP64-TWO-REMAIN records the class as "a PIPELINE change, so
generated/allobj moves and byte-identity CANNOT be the gate — the repair gives
the scratch its own storage, which is a different stack layout by construction."
That is true of the repair it had in mind (converting the shape upstream, in
`recover.py`) and it is not true of this one. Scaling the offsets and the object
TOGETHER, in a post-pass, leaves `generated/allobj` untouched and folds to the
original text on the shipped target:

    + 0x14                ->  + (5 * (int)sizeof(void *))
    int aiStack_9cb0[6];  ->  int aiStack_9cb0[6 * (int)(sizeof(void *) / 4)];

Measured on MstUtils, the object whose upstream attempt "knocked it out of the
build entirely": 123 offsets scaled, all three slot objects widened, object
byte-identical. FOURTH time this session a class called unfixable under the gate
turned out to be fixable. ASK THE COMPILER.

WHICH LOCALS. Ghidra names a stack local `<type><Stack|StackY>_<hex>`, and this
takes only those — a real local with a real name is not a fabricated frame. Two
further conditions, both refusals rather than guesses:

  * EVERY offset used against it must be a multiple of four. A slot area is
    argument words; anything else is not this class.
  * The name must appear NOWHERE except inside one of these casts. If the
    recovery ever uses `fStack_9c98` as the float it is declared to be, widening
    it changes what that read means, and the site is declined and reported.

⚠ ALL OR NOTHING PER SLOT OBJECT. A half-scaled area overlaps DIFFERENTLY rather
than not at all, and every piece of it passes byte-identity on its own — the same
trap `fix_arena_carve.py` records. The declaration and every offset into it move
together or none of them do.

⚠ AND IT BLINDS A GATE IF YOU LET IT. `check_frame_bounds.py` reads CONSTANT
offsets, and after this pass they are constant EXPRESSIONS. It has been taught
this spelling; if you change the spelling here, teach it again, or a detector
that exists for exactly this defect starts reporting zero.

Run it on a COPY: it edits in place, AFTER `fix_ptrwidth.py` (which writes the
`kd_iptr` the sites are spelled with).
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = '/tmp/kd_frameslots'

BANNER = re.compile(r'(?m)^/\* ---- (\S+)')
# Ghidra's own naming for a stack local: `aiStack_9cb0`, `fStackY_140`.
#
# ⚠ THE `\b` IS LOAD-BEARING. Without it the greedy type group eats the first
# letter of the name and `aiStack_9cb0` is captured as `iStack_9cb0` — a name
# that appears nowhere, so the slot has no sites, so it is skipped SILENTLY.
# Two of MstUtils' three areas went missing that way and the tool reported nine
# clean repairs while the object it was written for was untouched.
STACKNAME = re.compile(r'^[A-Za-z]{1,4}Stack[Y]?_[0-9a-f]+$')
DECLARED = re.compile(r'(?m)^(?P<ind>[ \t]+)(?P<ty>[A-Za-z_][\w ]*\**)[ \t]*'
                      r'\b(?P<name>[A-Za-z]{1,4}Stack[Y]?_[0-9a-f]+)[ \t]*'
                      r'(?:\[\s*(?P<n>\d+)\s*\])?[ \t]*;')
I386_PTR = 4
CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
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


def regions(text):
    """(name, start, end) for every function body in the file."""
    marks = [(m.group(1), m.start()) for m in BANNER.finditer(text)]
    for i, (name, s) in enumerate(marks):
        yield name, s, (marks[i + 1][1] if i + 1 < len(marks) else len(text))


def site_re(var):
    return re.compile(r'\(kd_iptr\)\s*(?P<amp>&?)\s*' + re.escape(var)
                      + r'\s*(?:\+\s*(?P<off>0x[0-9a-fA-F]+|\d+))?')


def slot_edits(text, notes, fn, quiet=False):
    """Every fabricated argument area in the file, as an all-or-nothing group."""
    groups = []
    for func, s, e in regions(text):
        body = text[s:e]
        for d in DECLARED.finditer(body):
            var = d.group('name')
            if 'sizeof(void *)' in d.group(0):
                continue                                  # already scaled
            sites = list(site_re(var).finditer(body))
            if not sites:
                continue
            # ⚠ USED AS ITSELF ANYWHERE? Then widening it changes a read.
            covered = len(sites) + 1                      # + its own declaration
            if len(re.findall(r'\b%s\b' % re.escape(var), body)) != covered:
                if not quiet:
                    notes.append('%-26s %s::%s is also used outside a frame-slot '
                                 'cast — declined' % (fn, func, var))
                continue
            offs = [int(m.group('off'), 0) if m.group('off') else 0 for m in sites]
            bad = [o for o in offs if o % I386_PTR]
            if bad:
                if not quiet:
                    notes.append('%-26s %s::%s has offsets that are not argument '
                                 'words (%s) — declined'
                                 % (fn, func, var, ' '.join(hex(o) for o in bad)))
                continue
            edits = []
            for m, o in zip(sites, offs):
                if m.group('off') is None:
                    continue                              # `+ 0` is already right
                edits.append((s + m.start('off'), s + m.end('off'),
                              '(%d * (int)sizeof(void *))' % (o // I386_PTR)))
            n = d.group('n')
            edits.append((s + d.end('name'), s + d.end(),
                          ' [%s(int)(sizeof(void *) / 4)];'
                          % ('%s * ' % n if n else '')))
            groups.append(((func, var),
                           '%s::%s (%s[%s], %d slot%s)'
                           % (func, var, d.group('ty').strip(), n or '',
                              len(set(offs)), '' if len(set(offs)) == 1 else 's'),
                           edits))
    return groups


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else os.path.join(
        HERE, '..', 'Thirdparty', 'metoolkit')
    inc = os.path.join(root, 'include')

    fixed = declined = 0
    notes = []
    seen = 0
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text0 = open(path, errors='ignore').read()
        if not slot_edits(text0, [], fn, quiet=True):
            continue
        text, tried, first = text0, set(), True
        # ⚠ RECOMPUTE AFTER EVERY ACCEPTED GROUP. The areas INTERLEAVE — MstUtils
        # writes `aiStack_9cb0` and `aMStack_9c94` in alternate statements — so
        # offsets taken against the original text are stale the moment one group
        # lands, and the next group's declaration edit then falls in the middle
        # of an expression. It reads as "not byte-identical", which is a decline
        # on a repair that was never actually tried: `aiStack_9cb0` declined that
        # way while the same three areas, edited together by hand, were identical.
        while True:
            groups = slot_edits(text, notes if first else [], fn, quiet=not first)
            first = False
            todo = [g for g in groups if g[0] not in tried]
            if not todo:
                break
            key, label, edits = todo[0]
            tried.add(key)
            seen += 1
            cand = text
            for st, en, rep in sorted(edits, key=lambda x: -x[0]):
                cand = cand[:st] + rep + cand[en:]
            # ⚠ ALL OR NOTHING PER AREA: a half-scaled one overlaps differently.
            if compiles_identically(fn, cand, build, inc):
                text = cand
                fixed += 1
                notes.append('%-26s %-52s %d edit(s)' % (fn, label, len(edits)))
            else:
                declined += 1
                notes.append('%-26s %-52s DECLINED: not byte-identical at i386'
                             % (fn, label))
        if text != text0:
            open(path, 'w').write(text)

    # A search that comes back empty has to say whether it can find anything.
    if not seen:
        sys.exit('fix_frame_slots: NOT ONE fabricated argument area in the corpus. '
                 'Ghidra emits them in every object that passes arguments on the '
                 'stack, so zero means the pattern stopped matching.')

    print('  invented argument areas scaled to pointer words : %d' % fixed)
    print('  declined (reported, not guessed)                : %d' % declined)
    for n in notes:
        print('     %s' % n)
    return 0


if __name__ == '__main__':
    sys.exit(main())
