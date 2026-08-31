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

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

# The product root. HERE/include holds kd_compat.h, kd_karma.h and
# kd_types.h — the three headers every recovered source includes, and the
# ones the size/offset probes below have to see to measure anything.
HERE = kd_paths.MD
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

# ★ THE SECOND SPELLING, and it is `ghidra_clean.py`'s own. Where Ghidra could
# not name the base of an outgoing area, the cleaner MATERIALISES one:
#
#     undefined1 kd_argarea_aiStack_b0 [0x14];
#     *(MeReal **)((kd_iptr)kd_argarea_aiStack_b0 + 0xc) = normal;
#     kd_UpdateHull(*(McdContactList **)((kd_iptr)kd_argarea_aiStack_b0 + 0x4), ...);
#
# A BYTE array addressed at 4-byte words. At LP64 the eight-byte writes at +0,
# +4, +8, +0xc overlap and `UpdateHull` is called with `contact=0x7fff` and
# `normal=0x7fff00000000` — the two halves of one stack address, in two
# different parameters. That is what `scene_boxes_on_plane` crashes on at step
# 34, and there are exactly TWO of these areas in the corpus.
#
# ⚠ ONE OF THE TWO IS ADDRESSED DOWNWARDS FROM ITS TOP.
#
#     pMVar6 = (McdContact *)(kd_argarea_pMVar6 + 0x24);
#     *(void **)((kd_iptr)pMVar6 + -4) = pvVar5;
#
# so the base offset and every negative offset off the DERIVED variable have to
# scale together with the array. The area is named after the variable it feeds,
# which is what ties the two together without any dataflow.
ARGAREA = re.compile(r'(?m)^(?P<ind>[ \t]+)(?P<ty>[A-Za-z_]\w*)[ \t]+'
                     r'(?P<name>kd_argarea_(?P<var>\w+))[ \t]*'
                     r'\[\s*(?P<n>0x[0-9a-fA-F]+|\d+)\s*\][ \t]*;')
DERIVED = re.compile(r'(?P<var>[A-Za-z_]\w*)\s*=\s*\([^()]*\)\s*\(\s*'
                     r'(?P<area>kd_argarea_\w+)\s*\+\s*(?P<off>0x[0-9a-fA-F]+|\d+)\s*\)')

# ★ A THIRD SPELLING: THE WHOLE FRAME MATERIALISED. Where Ghidra's locals are an
# alloca-shifted frame, `ghidra_clean.py` allocates one and lays everything out
# in it by hand:
#
#     char *kd_frame = (char *)alloca(uVar6 * 6 + 48);
#     uint *auStack_3c = (uint *)(kd_frame + uVar6 * 6 + 36);  /* KD_MATERIALISED_BASE */
#     *(int **)((kd_frame + 28)) = C;
#     keaLCPSolver__getClampIndices(*(void **)((kd_frame + 20)), ...);
#
# The `uVar6 * 6` part is four `int` arrays and is width-independent; the
# TRAILING constants — 48 in the size, 36 in the base — are the outgoing
# argument area, nine four-byte words, and every `kd_frame + K` indexes it. At
# LP64 the words overlap and `getClampIndices` is called with
# `I=0x7fff00007fff, C=0x800000007fff`: two stack addresses interleaved half and
# half. One site in the corpus, and it is what both contact scenes segfault on
# once everything before it is repaired.
KDFRAME = re.compile(r'(?m)^[ \t]*char \*(?P<name>kd_frame\w*)\s*=\s*\(char \*\)'
                     r'alloca\((?P<expr>[^;]*?)\+\s*(?P<n>\d+)\s*\)\s*;')
MATBASE = re.compile(r'(?m)^[ \t]*[A-Za-z_][\w ]*\**\s*\w+\s*=\s*\([^()]*\)\s*\('
                     r'(?P<name>kd_frame\w*)\s*\+[^;]*?\+\s*(?P<n>\d+)\s*\)\s*;'
                     r'[ \t]*/\* KD_MATERIALISED_BASE')
FRAME_OFF = re.compile(r'\bkd_frame\w*\s*\+\s*(?P<n>\d+)\b')


def frame_trailing_addends(body, name):
    """Every `(<frame group>) + K` — the argument word written OUTSIDE the
    base's own parentheses:

        *(int **)((int)(kd_frame + 0) + 8) = clamped;
        *(keaLCPSolver **)((int)(kd_frame + 0) + 4) = this;

    ⚠ THIS IS STRUCTURAL BECAUSE THE CAST IN FRONT OF THE GROUP IS NOT STABLE,
    AND KEYING ON ITS SPELLING MADE THE RULE FIND NOTHING. The predecessor
    matched a literal `(int)`, and `fix_ptrwidth` — which runs FOUR passes
    earlier — has already rewritten that to `(kd_iptr)` by the time this pass
    sees the file. Twenty sites, no match, and NO DECLINE REPORTED, because a
    pattern that does not fire has nothing to report.

    What that cost, measured: `this` is written at `+ 4` and `clamped` at `+ 8`,
    which are argument words ONE and TWO. At LP64 word one occupies +8..+15, so
    the write of `this` lands on top of `clamped`'s low half and the read comes
    back `0x00007fff00007fff` — the two pointers' high halves interleaved. That
    is `setClampedValues` writing through `clamped` at keaLCPSolver.c:825, and
    the value the arithmetic predicts is the value in `rsi`."""
    out = []
    for m in re.finditer(r'\b%s\s*\+' % re.escape(name), body):
        i, depth = m.start(), 0             # walk back to the group's `(`
        while i > 0:
            i -= 1
            if body[i] == ')':
                depth += 1
            elif body[i] == '(':
                if depth == 0:
                    break
                depth -= 1
        else:
            continue
        j, depth = i, 0                     # and forward to its `)`
        while j < len(body):
            if body[j] == '(':
                depth += 1
            elif body[j] == ')':
                depth -= 1
                if depth == 0:
                    break
            j += 1
        t = re.match(r'\s*\+\s*(?P<n>\d+)\b', body[j + 1:])
        if t:
            out.append((j + 1 + t.start('n'), j + 1 + t.end('n'), t.group('n')))
    return out


def kdframe_edits(text, notes, fn, quiet=False):
    """The materialised whole frame, as an all-or-nothing group. See KDFRAME."""
    groups = []
    for func, s, e in regions(text):
        body = text[s:e]
        d = KDFRAME.search(body)
        if not d or 'sizeof(void *)' in d.group(0):
            continue
        edits, bad = [], []
        for rx, g in ((KDFRAME, 'n'), (MATBASE, 'n')):
            for m in rx.finditer(body):
                edits.append((s + m.start(g), s + m.end(g),
                              '%s * (int)(sizeof(void *) / 4)' % m.group(g)))
        for rx in (FRAME_OFF,):
            for m in rx.finditer(body):
                k = int(m.group('n'))
                if k % I386_PTR:
                    bad.append(k)
                    continue
                edits.append((s + m.start('n'), s + m.end('n'),
                              '(%d * (int)sizeof(void *))' % (k // I386_PTR)))
        for st, en, n in frame_trailing_addends(body, d.group('name')):
            k = int(n)
            if k % I386_PTR:
                bad.append(k)
                continue
            edits.append((s + st, s + en,
                          '(%d * (int)sizeof(void *))' % (k // I386_PTR)))
        if bad:
            if not quiet:
                notes.append('%-26s %s::%s has offsets that are not argument words '
                             '(%s) — declined'
                             % (fn, func, d.group('name'),
                                ' '.join(str(x) for x in sorted(set(bad)))))
            continue
        # ⚠ The alloca's own trailing constant is matched by FRAME_OFF too when
        # the frame name appears in the expression; dedupe by span or the same
        # text is rewritten twice and the object stops matching.
        edits = list({(a, b): (a, b, r) for a, b, r in edits}.values())
        groups.append(((func, d.group('name')),
                       '%s::%s (materialised frame, %d edit%s)'
                       % (func, d.group('name'), len(edits),
                          '' if len(edits) == 1 else 's'),
                       edits))
    return groups


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
                      + r'\s*(?:\+\s*(?P<off>-?(?:0x[0-9a-fA-F]+|\d+)))?')


def argarea_edits(text, notes, fn, quiet=False):
    """The materialised argument areas, as all-or-nothing groups. See ARGAREA."""
    groups = []
    for func, s, e in regions(text):
        body = text[s:e]
        for d in ARGAREA.finditer(body):
            area, n = d.group('name'), int(d.group('n'), 0)
            if 'sizeof(void *)' in d.group(0):
                continue                                  # already scaled
            edits, offs, bad = [], [], []
            # ---- the area addressed directly
            for m in site_re(area).finditer(body):
                if m.group('off') is None:
                    continue
                k = int(m.group('off'), 0)
                offs.append(k)
                if k % I386_PTR:
                    bad.append(k)
                    continue
                edits.append((s + m.start('off'), s + m.end('off'),
                              '(%d * (int)sizeof(void *))' % (k // I386_PTR)))
            # ---- and through the pointer derived from its top
            for dm in DERIVED.finditer(body):
                if dm.group('area') != area:
                    continue
                k = int(dm.group('off'), 0)
                offs.append(k)
                if k % I386_PTR:
                    bad.append(k)
                    continue
                edits.append((s + dm.start('off'), s + dm.end('off'),
                              '(%d * (int)sizeof(void *))' % (k // I386_PTR)))
                for m in site_re(dm.group('var')).finditer(body):
                    if m.group('off') is None:
                        continue
                    k2 = int(m.group('off'), 0)
                    offs.append(k2)
                    if k2 % I386_PTR:
                        bad.append(k2)
                        continue
                    edits.append((s + m.start('off'), s + m.end('off'),
                                  '(%d * (int)sizeof(void *))' % (k2 // I386_PTR)))
            if bad:
                if not quiet:
                    notes.append('%-26s %s::%s has offsets that are not argument '
                                 'words (%s) — declined'
                                 % (fn, func, area,
                                    ' '.join(hex(x) for x in sorted(set(bad)))))
                continue
            if not edits:
                continue
            edits.append((s + d.start('n'), s + d.end('n'),
                          '%d * (int)(sizeof(void *) / 4)' % n))
            # ⚠ DEDUPE BY SPAN. The area is derived into its pointer TWICE —
            # `pMVar6 = (McdContact *)(kd_argarea_pMVar6 + 0x24);` appears on
            # both sides of a branch — so the walk over that pointer's sites
            # runs once per assignment and adds every edit twice. Applying a
            # span twice mangles the text, the object stops matching, and it
            # reads as "this area cannot be repaired byte-identically".
            edits = list({(a, b): (a, b, r) for a, b, r in edits}.values())
            groups.append(((func, area),
                           '%s::%s (%s[%d] argarea, %d slot%s)'
                           % (func, area, d.group('ty'), n, len(set(offs)),
                              '' if len(set(offs)) == 1 else 's'),
                           edits))
    return groups


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
            # ⚠ USED AS ITSELF ANYWHERE? Then widening it changes a read —
            # UNLESS every such use is the bare scalar, in which case `var[0]`
            # means exactly what `var` meant and the widening is meaning-
            # preserving. That distinction is the difference between declining
            # `fStackY_160` and fixing it.
            #
            # ★ AND IT IS A REAL DEFECT, NOT TIDYING. `IxSphereTriList` declares
            # `float fStackY_160;` and stores an EIGHT-BYTE pointer through its
            # address — `*(MeReal (**) [3])((kd_iptr)&fStackY_160) = edge;` — so
            # at LP64 the store runs four bytes past a four-byte local and into
            # whatever the compiler put next. The arm64 tombstone lands two calls
            # later, in `McdVanillaOverlapSphereTri`, dereferencing an
            # `inTri->vertices[0]` of 0x7200000072. `check_frame_bounds` cannot
            # see it: it reads ARRAY bounds, and this slot is a scalar.
            spans = [(m.start(), m.end()) for m in sites] + [(d.start(), d.end())]
            extra = [m for m in re.finditer(r'\b%s\b' % re.escape(var), body)
                     if not any(a <= m.start() < b for a, b in spans)]
            scalar_uses = []
            for m in extra:
                before = body[max(0, m.start() - 1):m.start()]
                after = body[m.end():m.end() + 1]
                if before == '&' or after == '[':
                    scalar_uses = None
                    break
                scalar_uses.append(m)
            if scalar_uses is None:
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
            # the bare scalar uses become element 0 of the widened slot
            for m in scalar_uses:
                edits.append((s + m.start(), s + m.end(), var + '[0]'))
            groups.append(((func, var),
                           '%s::%s (%s[%s], %d slot%s)'
                           % (func, var, d.group('ty').strip(), n or '',
                              len(set(offs)), '' if len(set(offs)) == 1 else 's'),
                           edits))
    return groups


def selftest_trailing():
    """The matcher has to find the site under EVERY cast spelling, and not find
    its own output. This exists because the predecessor did neither: it matched
    a literal `(int)`, `fix_ptrwidth` rewrites that to `(kd_iptr)` four passes
    earlier, and twenty sites went unrepaired with nothing printed."""
    for cast in ('(int)', '(kd_iptr)', '(kd_uptr)', ''):
        body = '  *(int **)(%s(kd_frame + 0) + 8) = c;\n' % cast
        got = frame_trailing_addends(body, 'kd_frame')
        if [g[2] for g in got] != ['8']:
            sys.exit('fix_frame_slots: SELF-CHECK FAILED — the trailing addend is '
                     'invisible under the cast %r (read %r). A rule that cannot '
                     'see the site reports no decline either.' % (cast or 'none', got))
    done = ('  *(int **)((kd_iptr)(kd_frame + (0 * (int)sizeof(void *)))'
            ' + (2 * (int)sizeof(void *))) = c;\n')
    if frame_trailing_addends(done, 'kd_frame'):
        sys.exit('fix_frame_slots: SELF-CHECK FAILED — the matcher does not '
                 'recognise its own output and would rescale a repaired site.')


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')
    selftest_trailing()

    fixed = declined = 0
    notes = []
    seen = 0
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text0 = open(path, errors='ignore').read()
        if not slot_edits(text0, [], fn, quiet=True) \
                and not argarea_edits(text0, [], fn, quiet=True) \
                and not kdframe_edits(text0, [], fn, quiet=True):
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
            groups = (slot_edits(text, notes if first else [], fn, quiet=not first)
                      + argarea_edits(text, notes if first else [], fn, quiet=not first)
                      + kdframe_edits(text, notes if first else [], fn, quiet=not first))
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
