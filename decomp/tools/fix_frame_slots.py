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

# ★ A FOURTH SPELLING: THE AREA BELOW AN `alloca`, REACHED THROUGH A CURSOR —
# and the last thing standing between arm64 and a clean Onslaught match.
#
#     kd_alloca_iVar3 = (char *)alloca(n * (int)sizeof(*(McdUserTriangle *)0) + 0);
#     puVar12 = (undefined1 *)kd_alloca_iVar3;
#     ...
#     *(int *)          (puVar12 + -0x20) = (kd_iptr)&(triList->list)->mRefCtAndID + i;
#     *(MeReal **)      (puVar12 + -0x1c) = relPos;
#     *(undefined4 *)   (puVar12 + -0x10) = ((McdSphyl *)geom)->mRadius;
#     GenerateTriangleContact(*(McdUserTriangle **)(puVar12 + -0x20),
#                             *(MeReal **)(puVar12 + -0x1c), ...);
#
# gcc puts the alloca'd block immediately ABOVE the outgoing argument area, so
# Ghidra spells the area as NEGATIVE offsets from the allocation — nine words at
# -4 … -0x24. `ghidra_clean.py`'s `materialise_alloca_relative_slots` gives that
# shape real storage where the offsets hang off `kd_alloca_*` directly; here the
# pointer is COPIED into a local first, so it never matched and the area stayed
# where it was.
#
# ⚠⚠ IT IS TWO DEFECTS AT LP64, AND HALF OF IT IS WORSE THAN NONE.
#   * the SLOTS OVERLAP. `-0x1c` and `-0x20` are four bytes apart and the
#     pointers written into them are eight, so `relPos` lands on top of the
#     `tri` argument's high half;
#   * and `tri` is STORED through `*(int *)` and READ through
#     `*(McdUserTriangle **)` — four bytes of eight, then an eight-byte read of
#     the truncation. `GenerateTriangleContact` dereferences `tri->vertices[0]`
#     and that is `McdSphylTriangleListIntersect`'s `SEGV_MAPERR` on the device
#     and 6 of 8 gametypes at x86-64.
#
# ★ THE STORAGE IS THE HARD HALF, AND THE `alloca` IS WHERE IT COMES FROM.
# Scaling the offsets alone doubles the area to 72 bytes below a block whose
# base is where the allocation STARTS — below it is not ours at either width. So
# the allocation is grown by the scaled area and the base is walked up past it,
# on ONE statement:
#
#     alloca(SIZE)   ->   alloca(SIZE + PAD) + PAD
#
# `PAD` is `(int)(sizeof(void *) / 8) * (nslots * (int)sizeof(void *))`, which is
# **0 at i386 and on wasm32** — `4 / 8` is zero in integer arithmetic — so the
# statement folds back to `alloca(SIZE) + 0` on every shipping target and the
# byte-identity gate is the thing that says so. At LP64 it is 72 bytes, the area
# moves inside the allocation, and what was an out-of-bounds write on a doubled
# area becomes an in-bounds one.
#
# ⚠ THE CURSOR MUST BE A CURSOR AND NOTHING ELSE. The block itself is real
# memory handed to callees — here it is the triangle array the generator fills —
# so a positive offset or a bare use means the name is the ARRAY, not the
# argument area, and the site is declined rather than guessed at.
ALLOCA_DECL = re.compile(r'(?P<name>kd_alloca_\w+)[ \t]*=[ \t]*\(char \*\)[ \t]*'
                         r'alloca\((?P<size>(?:[^()]|\([^()]*(?:\([^()]*\))?'
                         r'[^()]*\))*)\)')
# `V = (undefined1 *)kd_alloca_iVar3;` and the plain copy `V = W;` that follows it.
CURSOR_ASSIGN = re.compile(r'(?m)^[ \t]*(?P<var>[A-Za-z_]\w*)[ \t]*=[ \t]*'
                           r'(?:\((?:undefined1|char|uchar|byte|undefined) \*\)[ \t]*)?'
                           r'(?P<src>[A-Za-z_]\w*)[ \t]*;')
# `*(McdUserTriangle **)(puVar12 + -0x20)` — the access type and the slot together.
# ⚠ THE TYPE GROUP HAS TO ADMIT ITS OWN `*`s. Written `[\w ]*?` it matches
# `*(int *)` and NOT `*(McdUserTriangle **)`, so the pass sees the four-byte
# stores, misses every pointer load, concludes no slot carries a pointer and
# widens nothing — the one access this rule exists for. It found zero sites in
# the file it was written for and reported no decline, because a group that
# never forms has nothing to decline.
CURSOR_SITE = (r'\*\((?P<ty>[A-Za-z_][\w *]*?)[ \t]*\*\)[ \t]*\([ \t]*%s[ \t]*\+[ \t]*'
               r'(?P<off>-(?:0x[0-9a-fA-F]+|\d+))[ \t]*\)')
# What a four-byte access widens to when its slot also carries a pointer. A
# FLOAT cannot: widening it would change the value, not just the width, so a
# slot that mixes a float with a pointer is declined and reported.
WIDEN = {'int': 'kd_iptr', 'MeI32': 'kd_iptr', 'long': 'kd_iptr',
         'undefined4': 'kd_uptr', 'uint': 'kd_uptr', 'MeU32': 'kd_uptr',
         'ulong': 'kd_uptr', 'undefined': 'kd_uptr'}


def argpad(nslots):
    """The headroom an `alloca`-relative argument area needs at LP64, and
    exactly zero on every target that ships. `sizeof(void *) / 8` is 0 at four
    bytes and 1 at eight — the mirror of the `/ 4` this file already uses to
    widen a declaration."""
    return ('((int)(sizeof(void *) / 8) * (%d * (int)sizeof(void *)))' % nslots)


# ★ A FIFTH SPELLING: A FRAME SLOT'S MEMBER HOLDING A POINTER. Where Ghidra's
# invented local is a STRUCT, one of its members can be the spill slot for a
# pointer — and the struct is a real declared type, so the member cannot be
# widened the way an `int aiStack_9cb0[6]` can:
#
#     McdUserTriangle MStack_26c;
#     MStack_26c.flags = (McdTriangleFlags)result->normal;
#     *(float *)(MStack_26c.flags + 4) = diff.v[1] + *(MeReal *)(MStack_26c.flags + 4);
#
# `McdUserTriangle::flags` is four bytes at BOTH widths, so at LP64 the address
# of `result->normal` is cut in half and the two writes land wherever the low
# word points. It is `ptrwidth_classify`'s worst single object —
# `IxCylinderTriList` 6 of 6 UNEXPLAINED — and it is on the ragdoll-vs-WORLD
# path, which is where the first Android Karma tombstone landed.
#
# ★ THE REPAIR IS STORAGE, NOT A RE-SPELLING, AND THAT WAS MEASURED. Rewriting
# the reads as `result->normal[1]` is exact — line 290 right above them already
# writes `result->normal[0]` that way — and it is NOT byte-identical, with or
# without the now-dead store. Giving the member its own pointer-width local and
# leaving every expression otherwise verbatim IS.
#
# ⚠ ONLY IF THE STRUCT NEVER ESCAPES. If `&NAME` is taken, a callee can read the
# member and splitting it out silently stops updating what that callee sees.
SLOTMEMBER = re.compile(r'(?m)^(?P<ind>[ \t]+)(?P<ty>[A-Za-z_]\w*)[ \t]+'
                        r'(?P<name>[A-Za-z]{1,4}Stack[Y]?_[0-9a-f]+)[ \t]*;')


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


def alloca_cursor_edits(text, notes, fn, quiet=False):
    """The outgoing area BELOW an `alloca`, reached through a cursor copy.

    All-or-nothing per allocation: the offsets, the access widths and the
    allocation's own headroom move together or none of them do. See
    ALLOCA_DECL for what the shape is and why the padding is the hard half."""
    groups = []
    for func, s, e in regions(text):
        body = text[s:e]
        for d in ALLOCA_DECL.finditer(body):
            base = d.group('name')
            if 'sizeof(void *) / 8' in d.group(0):
                continue                                  # already padded
            # ---- who is a cursor: the copies of the base, transitively.
            cursors, grew = set(), True
            while grew:
                grew = False
                for m in CURSOR_ASSIGN.finditer(body):
                    v, src = m.group('var'), m.group('src')
                    if v == base or v in cursors:
                        continue
                    if src == base or src in cursors:
                        cursors.add(v)
                        grew = True
            if not cursors:
                continue
            edits, offs, bad, byslot, ok = [], [], [], {}, True
            for cur in sorted(cursors):
                rx = re.compile(CURSOR_SITE % re.escape(cur))
                sites = list(rx.finditer(body))
                if not sites:
                    continue
                # ⚠ USED AS ANYTHING BUT A CURSOR? Then it is the ARRAY. A
                # positive offset or a bare pass hands the block to a callee,
                # and moving the base under it would corrupt a real allocation.
                spans = [(m.start(), m.end()) for m in sites]
                spans += [(m.start(), m.end()) for m in CURSOR_ASSIGN.finditer(body)
                          if cur in (m.group('var'), m.group('src'))]
                spans += [(m.start(), m.end()) for m in re.finditer(
                    r'(?m)^[ \t]*[A-Za-z_][\w ]*\**[ \t]*%s[ \t]*;' % re.escape(cur),
                    body)]
                stray = [m for m in re.finditer(r'\b%s\b' % re.escape(cur), body)
                         if not any(a <= m.start() < b for a, b in spans)]
                if stray:
                    if not quiet:
                        notes.append('%-26s %s::%s is used outside a negative '
                                     'cursor slot — declined' % (fn, func, cur))
                    ok = False
                    break
                for m in sites:
                    k = -int(m.group('off'), 0)
                    offs.append(k)
                    if k <= 0 or k % I386_PTR:
                        bad.append(k)
                        continue
                    byslot.setdefault(k // I386_PTR, []).append(
                        (s + m.start('ty'), s + m.end('ty'), m.group('ty').strip()))
                    edits.append((s + m.start('off'), s + m.end('off'),
                                  '-(%d * (int)sizeof(void *))' % (k // I386_PTR)))
            if not ok or not edits:
                continue
            if bad:
                if not quiet:
                    notes.append('%-26s %s::%s has offsets that are not argument '
                                 'words (%s) — declined'
                                 % (fn, func, base,
                                    ' '.join(hex(x) for x in sorted(set(bad)))))
                continue
            # ---- F: THE ACCESS WIDTH, PER SLOT. A slot that carries a pointer
            # anywhere is a pointer-width slot everywhere, or the store and the
            # load disagree by four bytes — which is the `tri` argument, and the
            # crash. `int` becomes `kd_iptr` and `undefined4` becomes `kd_uptr`,
            # both of which ARE the type they replace at 32-bit pointer width.
            for k, uses in sorted(byslot.items()):
                if not any(t.endswith('*') for _, _, t in uses):
                    continue
                for st, en, t in uses:
                    if t.endswith('*'):
                        continue
                    if t not in WIDEN:
                        bad.append(k)
                        if not quiet:
                            notes.append('%-26s %s::%s slot -%#x mixes %r with a '
                                         'pointer — declined'
                                         % (fn, func, base, k * I386_PTR, t))
                        break
                    edits.append((st, en, WIDEN[t]))
                if bad:
                    break
            if bad:
                continue
            # ---- the allocation grows by the scaled area and the base walks up
            # past it, so what the offsets now reach is inside the block.
            nslots = max(offs) // I386_PTR
            pad = argpad(nslots)
            edits.append((s + d.end('size'), s + d.end('size'), ' + ' + pad))
            edits.append((s + d.end(), s + d.end(), ' + ' + pad))
            edits = list({(a, b): (a, b, r) for a, b, r in edits}.values())
            groups.append(((func, base),
                           '%s::%s (alloca cursor, %d slot%s, %d word area)'
                           % (func, base, len(set(offs)),
                              '' if len(set(offs)) == 1 else 's', nslots),
                           edits))
    return groups


def slotmember_edits(text, notes, fn, quiet=False):
    """A frame slot's MEMBER used as an address. See SLOTMEMBER."""
    groups = []
    for func, s, e in regions(text):
        body = text[s:e]
        for d in SLOTMEMBER.finditer(body):
            name, ty = d.group('name'), d.group('ty')
            if re.search(r'&[ \t]*\(?[ \t]*%s\b' % re.escape(name), body):
                continue                     # the struct escapes — not ours
            used = {m.group(1) for m in re.finditer(
                r'\*\([A-Za-z_][\w *]*\*\)[ \t]*\([ \t]*%s\.(\w+)[ \t]*[+)]'
                % re.escape(name), body)}
            for M in sorted(used):
                a = size_of('((%s *)0)->%s' % (ty, M), '-m32')
                b = size_of('((%s *)0)->%s' % (ty, M), '-m64')
                if not (a == 4 and b == 4):
                    continue                 # already wide enough there
                new = 'kd_slot_%s_%s' % (name, M)
                edits = [(s + m.start(), s + m.end(), new) for m in re.finditer(
                    r'(?<![\w.])%s\.%s\b' % (re.escape(name), re.escape(M)), body)]
                if not edits:
                    continue
                edits.append((s + d.end(), s + d.end(),
                              '\n%skd_iptr %s;' % (d.group('ind'), new)))
                groups.append(((func, name + '.' + M),
                               '%s::%s.%s (slot member -> kd_iptr, %d use%s)'
                               % (func, name, M, len(edits) - 1,
                                  '' if len(edits) == 2 else 's'),
                               edits))
    return groups


def slotpair_edits(text, notes, fn, quiet=False):
    """A slot STORED narrower than the very next read of it.

    ★ A SIXTH SPELLING, and it is the one the fifth exposes. An argument slot is
    REUSED across calls, so the same address carries an `int` for one call and a
    pointer for the next:

        *(int *)             (&(*kd_argslot_fffffd84)) = triList->triangleMaxCount;
        ... McdCylinderIntersect(..., *(int *)(&(*kd_argslot_fffffd84)));   /* fine  */
        *(McdTriangleFlags *)(&(*kd_argslot_fffffd84)) = kd_slot_MStack_26c_flags;
        MeVector3Normalize(  *(MeReal **)(&(*kd_argslot_fffffd84)));        /* FOUR  */

    The last pair stores four bytes and reads eight, so `MeVector3Normalize`
    normalises through half an address. ⚠ GROUPING BY ADDRESS WOULD BE WRONG —
    widening every access to that slot would take the `triangleMaxCount` store
    with it, and that one is a genuine `int` read back as a genuine `int`. The
    pairing is a STORE and the NEXT READ of the same address with no store in
    between, which is the only thing that makes the two widths comparable."""
    groups = []
    ADDR = (r'\*\((?P<ty>[A-Za-z_][\w *]*?)[ \t]*\*\)[ \t]*'
            r'(?P<addr>\(&\(\*[A-Za-z_]\w*\)\)|\((?:kd_iptr|kd_uptr)\)'
            r'[ \t]*&?[A-Za-z_]\w*\))')
    for func, s, e in regions(text):
        body = text[s:e]
        seq = list(re.finditer(ADDR, body))
        edits = []
        for i, m in enumerate(seq):
            after = body[m.end():m.end() + 4]
            if not re.match(r'\s*=[^=]', after):
                continue                                  # not a store
            for n in seq[i + 1:]:
                if n.group('addr') != m.group('addr'):
                    continue
                if re.match(r'\s*=[^=]', body[n.end():n.end() + 4]):
                    break                                 # stored again first
                a = size_of(m.group('ty').strip(), '-m32')
                b = size_of(m.group('ty').strip(), '-m64')
                c = size_of(n.group('ty').strip(), '-m64')
                if a == 4 and b == 4 and c == 8:
                    edits.append((s + m.start('ty'), s + m.end('ty'), 'kd_iptr'))
                break
        if edits:
            edits = list({(a, b): (a, b, r) for a, b, r in edits}.values())
            groups.append(((func, 'slotpair'),
                           '%s (slot stored narrower than read, %d site%s)'
                           % (func, len(edits), '' if len(edits) == 1 else 's'),
                           edits))
    return groups


_SIZES = {}


def size_of(expr, bits):
    """`sizeof(expr)` off the compiler — the member widths this rule turns on."""
    key = (expr, bits)
    if key in _SIZES:
        return _SIZES[key]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'sz.c')
    open(src, 'w').write(
        '#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
        '#include "%s/include/kd_types.h"\nchar kd_probe[sizeof(%s)];\n'
        'int kd_force = &kd_probe;\n' % (HERE, HERE, HERE, expr))
    r = subprocess.run(['gcc', bits, '-DLINUX'] + includes(INC_G or '')
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    m = re.search(r'char \(\*\)\[(\d+)\]', r.stderr)
    _SIZES[key] = int(m.group(1)) if m else None
    return _SIZES[key]


INC_G = None


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
    global INC_G
    INC_G = inc
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
                and not kdframe_edits(text0, [], fn, quiet=True) \
                and not alloca_cursor_edits(text0, [], fn, quiet=True) \
                and not slotmember_edits(text0, [], fn, quiet=True) \
                and not slotpair_edits(text0, [], fn, quiet=True):
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
                      + kdframe_edits(text, notes if first else [], fn, quiet=not first)
                      + alloca_cursor_edits(text, notes if first else [], fn,
                                            quiet=not first)
                      + slotmember_edits(text, notes if first else [], fn,
                                         quiet=not first)
                      + slotpair_edits(text, notes if first else [], fn,
                                       quiet=not first))
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
