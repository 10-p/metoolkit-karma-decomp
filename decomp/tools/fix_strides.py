#!/usr/bin/env python3
"""fix_strides.py — a POINTER STRIDE spelled as an offset inside the struct.

Two shapes, one defect: a step that is correct on i386 only because of a
coincidence of field widths. Both are repaired and each repair is COMPILED and
compared against the baseline object before it is kept.

    fix_strides.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT. Ghidra renders `memset(table, 0, n)` over a struct table as a loop
that walks ONE FIELD AT A TIME and counts in words:

    for (uVar7 = (uint)(count * count * 0x1c) >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
        pMVar4->helloFn = (McdHelloFn)0x0;
        pMVar4 = (McdInteractions *)&pMVar4->goodbyeFn;      <- steps 4 bytes
    }

That is correct on i386 for one reason only: EVERY FIELD OF McdInteractions IS
FOUR BYTES THERE, so the byte count over 4 is the number of steps. At LP64 the
step `&p->goodbyeFn` becomes EIGHT bytes while `>> 2` still divides by four, so
the loop walks twice as far as the table is long. ASan reports it as a write
past the interaction table with a backtrace pointing at the zeroing loop, which
is at least honest — the same defect one allocation earlier reports two hundred
lines away (see `fix_baked_sizeof.py`).

★ THIS CLASS WAS RECORDED AS POSSIBLY UNFIXABLE UNDER THE BYTE-IDENTITY GATE,
AND THAT WAS WRONG. The reasoning was that the i386 code is only correct because
of a coincidence of field widths, so any repair must change the arithmetic
rather than re-spell it. The reasoning is right and the conclusion does not
follow: the count is a CONSTANT-FOLDED expression, and

    (uint)(A * A * 0x1c) >> 2
    (uint)(A * A * (int)sizeof(*(McdInteractions *)0)) / (uint)KD_OFFSET(...)

compile BYTE-IDENTICALLY, because both fold to `A * A * 7`. Two spellings were
tried and both reproduced the object exactly. The lesson is the one this project
keeps re-learning: ASK THE COMPILER, do not reason about what it will emit.

THE REPAIR IS DERIVED, NOT PATTERN-MATCHED. The step the loop actually takes is
`offsetof(T, F2)` — the second field named, not a guess about pointer size — and
the total it must cover is `n * sizeof(T)`. Both are compile-time constants on
every target, so:

    trip = n * sizeof(T) / offsetof(T, F2)

is the same number on i386 and the right number everywhere else. `n` is
recovered from the i386 arithmetic and must come out whole, or the site is
declined and reported.

SCOPE: three word-counted loops and one self-advancing pointer. It is a
post-pass rather than a `ghidra_clean` rule because it needs a compiler to
measure `offsetof`. Run it on a COPY: it edits in place.

⚠ A TOOL THAT REPORTS A REPAIR IT DID NOT MAKE. `path` is the output file, and
the second rule shadowed it with the FIELD path it was building. The repaired
text went to a file called `mAABBMarkers[0].mOrdinate` in the working directory,
this printed "4 repaired", and `CxSmallSort.c` was untouched on disk — so the
LP64 harness kept reporting the same SEGV against a repair that had, as far as
the report was concerned, already landed. Verify a pass by diffing the SOURCE,
not by reading its summary.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fix_narrow_pointers as FNP

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

# The product root. HERE/include holds kd_compat.h, kd_karma.h and
# kd_types.h — the three headers every recovered source includes, and the
# ones the size/offset probes below have to see to measure anything.
HERE = kd_paths.MD
WORK = '/tmp/kd_wordloop'

# for (V = COUNT; V != 0; V = V - 1) { P->F1... = ...; P = (T *)&P->F2...; }
#
# ⚠ THE STEP IS NOT ALWAYS A DIRECT MEMBER. CxSmallSort::New walks a base-class
# subobject —
#
#     pCVar4 = (CxSmallSortRep *)&(pCVar4->super_Link).mPrev;
#
# — so the parenthesised, dotted path has to be part of the pattern. Matching
# only `&p->field` found McdFrame's loop and silently missed this one, which is
# the loop that actually overruns the CxSmallSortRep table by 800 bytes.
PATH = r'\(?(?P=p)->\w+\)?(?:\s*\.\s*\w+)*'
LOOP_SRC = (r'for \((?P<v>\w+) = (?P<count>[^;]+?); (?P=v) != 0; '
            r'(?P=v) = (?P=v) (?:- 1|\+ -1)\) \{\s*'
            r'\(?(?P<p>\w+)->\w+\)?(?:\s*\.\s*\w+)*\s*=\s*[^;]+;\s*'
            r'(?P=p) = \((?P<ty>[\w ]+) \*\)&\(?(?P=p)->(?P<f2>\w+(?:\)?\s*\.\s*\w+)*)\)?;\s*\}')
LOOP = re.compile(LOOP_SRC, re.S)

# `(uint)(EXPR * K) >> k` — Ghidra's rendering of `bytes / 4`.
SHIFTED = re.compile(r'^\(uint\)\((?P<expr>.*?)\s*\*\s*(?P<sz>0x[0-9a-f]+|\d+)\s*\)'
                     r'\s*>>\s*(?P<k>\d+)$', re.S)

# ---------------------------------------------------------------------------
# THE SECOND SHAPE: A STRIDE SPELLED AS AN ADDRESS INSIDE THE STRUCT.
#
#     pCVar5 = (CxSmallSortRep *)&pCVar5->mAABBMarkers[0].mOrdinate;
#
# `_Update` walks the three axes, reading `mAABBMarkers[0]` and `[1]` off an
# advancing pointer, so each iteration must step TWO markers. Ghidra spells that
# step as whatever address happens to be two markers along, and on i386 those
# coincide: offsetof(mAABBMarkers)=32 plus offsetof(mOrdinate)=8 is 40, and
# 2*sizeof(CxSmallSortMarker) is also 40. At LP64 they diverge — 56+16=72 against
# 2*40=80 — so from the second axis on, every "marker" is 8 bytes adrift and
# `inMarker->mRep` is garbage. The SEGV lands in MoveStartMarkerDown, ~300 lines
# and two calls away from this line.
#
# THE STRIDE IS DERIVED, NOT GUESSED: the path goes through an ARRAY member, the
# i386 step must be a whole multiple of that array's element size, and the
# multiple is the answer. Both quantities are asked of the compiler. The shipped
# amd64 build then has to pass the 64-bit value — `_Update` there does
# `add $0x50,%rbx`, which is 80.
# ★ A THIRD SHAPE: A TABLE WALKED BY A BYTE CURSOR WITH A BAKED STRIDE.
#
#     iVar10 = 0xc;
#     do {
#       *(undefined4 *)((kd_iptr)&context->pools->contactCount + iVar10) = 0;
#       *(undefined4 *)((kd_iptr)&context->pools[1].contactCount + iVar10) = 0;
#       ...
#       iVar10 = iVar10 + 0x30;
#     } while (-1 < iVar11);
#
# gcc unrolled `for (i = 0; i < 16; i++) pools[i].contactCount = 0;` four ways
# and Ghidra rendered the induction variable as a BYTE cursor. `0xc` is one
# `McdBatchContactPool` and `0x30` is four of them — 12 bytes each at i386 and
# SIXTEEN at LP64, because the struct carries a `contacts` pointer. At 64-bit the
# cursor lands four bytes short of every element from the first step on, so the
# loop zeroes the middle of `contacts` instead of `contactCount` and leaves the
# pool counts holding whatever was there.
#
# ⚠ THE ELEMENT TYPE IS THE STRUCT THAT OWNS THE NAMED FIELD, not the field's
# own type. `&context->pools->contactCount` names `contactCount`, an `int`; what
# the cursor strides over is the `McdBatchContactPool` that CONTAINS it.
#
# ⚠ AND IT IS LATENT, WHICH IS WHY IT IS WRITTEN DOWN. Measured: repairing it
# alone changed not one number in any of the three scenes. It is a real defect
# by measurement — 12 against 16, and the cursor provably misses — and it was
# NOT the cause of the divergence it was found while chasing. Bisect before
# believing; that is four for four in this project.
# ⚠ THE CAST IS NOT PART OF THE PATTERN. This pass runs BEFORE `fix_ptrwidth`,
# so the sites still say `(int)&...` here and `(kd_iptr)&...` afterwards —
# writing the later spelling into the matcher made it find nothing at all, which
# is the same silent miss `fix_frame_slots`' trailing addend had.
CURSOR_SITE = re.compile(
    r'\((?:int|kd_iptr|kd_uptr)\)&(?P<expr>[A-Za-z_]\w*(?:(?:->|\.)[A-Za-z_]\w*|\[[^\]]*\])*)'
    r'\s*\+\s*(?P<cur>[A-Za-z_]\w*)\s*\)')
CURSOR_SET = r'(?m)^(?P<ind>[ \t]*)%s\s*=\s*(?:%s\s*\+\s*)?(?P<k>0x[0-9a-fA-F]+|\d+)\s*;'

ADVANCE = re.compile(
    r'(?m)^(?P<ind>[ \t]*)(?P<p>\w+) = \((?P<ty>[\w ]+) \*\)'
    r'&(?P=p)->(?P<arr>\w+)\[(?P<idx>\d+)\](?P<tail>(?:\s*\.\s*\w+)*)\s*;')

# ---------------------------------------------------------------------------
# THE THIRD SHAPE: THE ELEMENT TYPE GHIDRA LOST FROM A CURSOR.
#
# `McdContactSimplify` walks the incoming contact array with a local Ghidra put
# in the WRONG stack slot — it merged an `McdContact *` into an unrelated
# `McdContactLink` local and used its `next` field as the cursor. Every use of
# it then carries the wrong element type, in two places at once:
#
#     MStack_9c.next = (_McdContactLink *)(inContacts + cNum);   /* the real type */
#     ... *(float *)&((&(MStack_9c.next)->contact)[2]) ...       /* index in POINTER units */
#     MStack_9c.next = (_McdContactLink *)((kd_iptr)MStack_9c.next + 0x28);
#
# 0x28 is 40 is `sizeof(McdContact)` — ON i386. At LP64 it is 48, because the
# contact's two `element` unions hold pointers, so the cursor walks 40 bytes at
# a time over 48-byte elements and reads a different contact every step. And the
# index `[2]` steps in units of `sizeof(McdContact *)`, four bytes here and
# EIGHT there, while what it dereferences is a four-byte float.
#
# ★ MEASURED, AND IT IS THE LAST THING BETWEEN THE THREE SCENES AND BIT
# IDENTITY. `scene_boxes_on_plane` was byte-identical at i386 and LP64 for 94
# steps and then diverged; the cause is here, and it presents as two contacts
# with EQUAL separation arriving in the opposite order — and, four calls later,
# as a simplify that returns 3 contacts where i386 returns 4.
#
# BOTH REPAIRS ARE NO-OPS AT i386 BY CONSTRUCTION and are verified as such:
#     + 0x28                ->  + (int)sizeof(*(McdContact *)0)
#     &((&(P)->F)[k])       ->  ((char *)&(P)->F + k * (int)sizeof(ACC))
# The first needs the element type, which only the cursor's PROVENANCE can give
# — what it was assigned from before it started stepping. The second needs
# nothing: it re-spells an index whose unit disagrees with its dereference, and
# a wrong guess about the member's width is a different address at i386, which
# the byte-identity check refuses.
SELF_ADVANCE = re.compile(
    r'(?m)^[ \t]*(?P<p>[A-Za-z_]\w*(?:(?:->|\.)[A-Za-z_]\w*)*)\s*=\s*'
    r'\((?P<ty>[\w ]+)\*\)\s*\(\s*\((?:kd_iptr|kd_uptr|int)\)\s*(?P=p)\s*\+\s*'
    r'(?P<k>0x[0-9a-fA-F]+|\d+)\s*\)\s*;')
# what the cursor was set from — the only thing that names its element type
PROVENANCE = (r'(?m)^[ \t]*%s\s*=\s*(?:\([\w ]+\*\)\s*)?\(?\s*'
              r'(?P<v>[A-Za-z_]\w*)\s*(?:\+[^;]*)?\)?\s*;')
# `TYPE *name` as a local or as a parameter
DECL_OF = r'(?<![\w>.])((?:const |struct |unsigned )*[A-Za-z_]\w*)[ \t]*\*[ \t]*%s\b'
# an index whose UNIT is a pointer and whose dereference is four bytes
UNIT_MISMATCH = re.compile(
    r'\*\(\s*(?P<acc>float|MeReal|undefined4|int|uint|MeU32|MeI32)\s*\*\s*\)\s*&\s*'
    r'\(\s*\(\s*&\s*\(?(?P<base>[A-Za-z_][\w.]*)\)?\s*->\s*(?P<f>[A-Za-z_]\w*)\s*\)'
    r'\s*\[\s*(?P<k>[1-9]\d*)\s*\]\s*\)')


CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types',
          '-DLINUX']


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


HEAD = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
        '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))
_SZ = re.compile(r'char \(\*\)\[(\d+)\]')


def measure(expr, inc, cache):
    """A compile-time constant, read out of the compiler's own type printer.

    `char probe[EXPR]; int force = &probe;` makes gcc say `char (*)[N]` in the
    diagnostic. One compile, no execution, and the number is the compiler's
    rather than a reconstruction of what it would have said."""
    if expr in cache:
        return cache[expr]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p.c')
    open(src, 'w').write(HEAD + 'char kd_probe[%s];\nint kd_force = &kd_probe;\n' % expr)
    r = subprocess.run(['gcc', '-m32', '-DLINUX'] + includes(inc)
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    m = _SZ.search(r.stderr)
    cache[expr] = int(m.group(1)) if m else None
    return cache[expr]


def compiles_identically(fn, text, build, inc):
    """Does this source still produce the baseline .o, byte for byte?

    ⚠ The file has to keep its NAME: gcc records the source basename in an
    STT_FILE symbol, so identical text from a scratch name is a different
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


def enclosing_block(text, pos):
    """The innermost `{ ... }` containing `pos`, as a (start, end) span.

    ⚠ A GHIDRA TEMPORARY IS REUSED ACROSS THE WHOLE FUNCTION. `iVar10` is the
    pool cursor inside one loop and an entry index three statements later, so a
    function-wide scan for its literal assignments finds `iVar10 = iVar10 + 1`
    and declines the site as "not all multiples". The cursor's arithmetic is a
    property of its LOOP, so that is what gets read."""
    depth, i = 0, pos
    while i > 0:
        i -= 1
        if text[i] == '}':
            depth += 1
        elif text[i] == '{':
            if depth == 0:
                break
            depth -= 1
    else:
        return 0, len(text)
    start, depth, j = i, 0, i
    while j < len(text):
        if text[j] == '{':
            depth += 1
        elif text[j] == '}':
            depth -= 1
            if depth == 0:
                break
        j += 1
    return start, j + 1


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')
    cache = {}

    # ---- THE SELF-CHECK. A probe that cannot compile returns None for every
    # measurement, every site declines, and "0 loops repaired" reads exactly
    # like "there were none to repair".
    probe = measure('sizeof(*(McdInteractions *)0)', inc, cache)
    if probe != 28:
        sys.exit('fix_strides: SELF-CHECK FAILED — sizeof(McdInteractions) '
                 'measured %r, want 28. Nothing below is measuring anything.'
                 % probe)

    fixed = declined = 0
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        text0 = text
        edits = []
        for m in LOOP.finditer(text):
            ty = m.group('ty').strip()
            # `(p->super_Link).mPrev` captures as `super_Link).mPrev`
            f2 = re.sub(r'\s*\)\s*\.\s*', '.', m.group('f2')).strip()
            raw = re.sub(r'\s+', ' ', m.group('count')).strip()
            step = measure('((char *)&((%s *)0)->%s - (char *)0)'
                           % (ty, f2), inc, cache)
            size = measure('sizeof(*(%s *)0)' % ty, inc, cache)
            why = None
            if not step or not size:
                why = 'cannot measure %s / %s' % (ty, m.group('f2'))
            elif step == size:
                # The step IS the whole struct, so the loop is ordinary array
                # iteration and scales correctly on its own. Nothing to do.
                continue
            else:
                sh = SHIFTED.match(raw)
                if sh and int(sh.group('sz'), 0) == size and \
                        (1 << int(sh.group('k'))) == step:
                    # `(uint)(EXPR * sizeof) >> log2(step)` — EXPR is the count
                    count_expr = '(%s)' % sh.group('expr').strip()
                elif re.fullmatch(r'0x[0-9a-f]+|\d+', raw):
                    total = int(raw, 0) * step
                    if total % size:
                        why = ('%d words * %d does not divide sizeof(%s)=%d'
                               % (int(raw, 0), step, ty, size))
                    else:
                        count_expr = str(total // size)
                else:
                    why = 'count %r is not words-of-a-table' % raw[:40]
            if why:
                declined += 1
                notes.append('%-26s DECLINED: %s' % (fn, why))
                continue
            rep = ('(uint)(%s * (int)sizeof(*(%s *)0)) / '
                   '(uint)((char *)&((%s *)0)->%s - (char *)0)'
                   % (count_expr, ty, ty, f2))
            edits.append((m.start('count'), m.end('count'), [rep],
                          '%-26s %s trip count: %d-byte words -> / offsetof(%s, %s)'
                          % (fn, ty, step, ty, f2)))
        # ---- the self-advancing pointer whose stride is an in-struct address
        for m in ADVANCE.finditer(text):
            ty, arr = m.group('ty').strip(), m.group('arr')
            # NOT `path` — that is the output file, and shadowing it here wrote
            # the repaired source to a file called `mAABBMarkers[0].mOrdinate`
            # in the CWD while reporting the repair as done. The tool said
            # "4 repaired" and CxSmallSort.c was untouched on disk.
            fpath = '%s[%s]%s' % (arr, m.group('idx'),
                                  re.sub(r'\s*', '', m.group('tail')))
            step = measure('((char *)&((%s *)0)->%s - (char *)0)' % (ty, fpath),
                           inc, cache)
            elem = measure('sizeof(((%s *)0)->%s[0])' % (ty, arr), inc, cache)
            if not step or not elem:
                continue
            if step % elem:
                declined += 1
                notes.append('%-26s %s stride %d is not a whole number of %s[] '
                             'elements (%d)' % (fn, ty, step, arr, elem))
                continue
            n = step // elem
            # ★ KEEP THE ORIGINAL ADDRESS AND ADD A CORRECTION THAT IS ZERO HERE.
            # `(char *)p + N * sizeof(elem)` is the obvious spelling and it is
            # NOT byte-identical — gcc re-allocates registers across the whole
            # of `_Update`, 508 differing instructions, for what is the same
            # address. Anchoring on the expression Ghidra already wrote and
            # adding `(want64 - offsetof(path))` keeps the i386 object EXACTLY,
            # because that delta folds to 0 there, while correcting the stride
            # everywhere the two disagree. The obvious spelling is kept as a
            # fallback so the compiler, not this comment, decides.
            off = '((char *)&((%s *)0)->%s - (char *)0)' % (ty, fpath)
            esz = '(int)sizeof(((%s *)0)->%s[0])' % (ty, arr)
            reps = ['(%s *)((char *)&%s->%s + (%d * %s - (int)%s))'
                    % (ty, m.group('p'), fpath, n, esz, off),
                    '(%s *)((char *)%s + %d * %s)' % (ty, m.group('p'), n, esz)]
            edits.append((m.start(), m.end(),
                          ['%s%s = %s;' % (m.group('ind'), m.group('p'), r)
                           for r in reps],
                          '%-26s %-16s stride %d bytes -> %d * sizeof(%s[0])'
                          % (fn, ty, step, n, arr)))

        # ---- THE BYTE CURSOR. See CURSOR_SITE. Per cursor, all or nothing:
        # a half-scaled walk lands somewhere else entirely rather than nowhere.
        cursors = {}
        for m in CURSOR_SITE.finditer(text):
            expr, cur = m.group('expr'), m.group('cur')
            if not re.search(r'(->|\.)[A-Za-z_]\w*$', expr):
                continue                    # no named field, so no owner to size
            owner_expr = re.sub(r'(->|\.)[A-Za-z_]\w*$', '', expr)
            # ⚠ `pools[1].contactCount` and `pools->contactCount` are the same
            # table. Dropping the subscript is what makes one cursor read as one
            # type instead of "this type at some sites and nothing at others".
            owner_expr = re.sub(r'\[[^\]]*\]', '', owner_expr)
            oty = FNP.lvalue_type(text, m.start(), owner_expr, inc)
            otag = FNP.tag_of(oty or '', inc)
            if not otag:
                cursors.setdefault(cur, []).append(None)
                continue
            cursors.setdefault(cur, []).append(otag)
        for cur, tags in sorted(cursors.items()):
            named = {x for x in tags if x}
            if len(named) != 1 or None in tags:
                why = ('nothing this can type' if not named
                       else 'more than one type (%s)' % ', '.join(sorted(named))
                       if len(named) > 1
                       else '%s at some sites and nothing at others'
                            % ', '.join(sorted(named)))
                notes.append('%-26s cursor %s walks %s — declined' % (fn, cur, why))
                declined += 1
                continue
            otag = named.pop()
            sz = measure('sizeof(*(%s *)0)' % otag, inc, cache)
            if not sz:
                notes.append('%-26s cursor %s: cannot measure sizeof(%s) — declined'
                             % (fn, cur, otag))
                declined += 1
                continue
            # The loop the cursor lives in, plus the initialiser just above it.
            first = min(m.start() for m in CURSOR_SITE.finditer(text)
                        if m.group('cur') == cur)
            bs, be = enclosing_block(text, first)
            rx = CURSOR_SET % (re.escape(cur), re.escape(cur))
            sets = list(re.finditer(rx, text, ))
            sets = [s for s in sets if bs <= s.start() < be]
            init = [s for s in re.finditer(rx, text) if s.end() <= bs]
            if init:
                sets.append(init[-1])
            if not sets or any(int(s.group('k'), 0) % sz for s in sets):
                notes.append('%-26s cursor %s: %d literal(s), not all multiples of '
                             'sizeof(%s)=%d — declined'
                             % (fn, cur, len(sets), otag, sz))
                declined += 1
                continue
            cand = text
            for s in sorted(sets, key=lambda x: -x.start('k')):
                n = int(s.group('k'), 0) // sz
                cand = (cand[:s.start('k')] + '%d * (int)sizeof(%s)' % (n, otag)
                        + cand[s.end('k'):])
            if compiles_identically(fn, cand, build, inc):
                text = cand
                fixed += 1
                notes.append('%-26s cursor %-8s %d literal(s) -> n * sizeof(%s) (%d bytes)'
                             % (fn, cur, len(sets), otag, sz))
            else:
                declined += 1
                notes.append('%-26s cursor %s: not byte-identical at i386 — declined'
                             % (fn, cur))

        for start, end, reps, note in sorted(edits, key=lambda e: -e[0]):
            for rep in reps:
                cand = text[:start] + rep + text[end:]
                if compiles_identically(fn, cand, build, inc):
                    text = cand
                    fixed += 1
                    notes.append(note)
                    break
            else:
                declined += 1
                notes.append('%-26s DECLINED: no spelling of it is '
                             'byte-identical at i386' % fn)

        # ---- THE THIRD SHAPE, both halves. See the block above SELF_ADVANCE.
        for m in SELF_ADVANCE.finditer(text):
            cur, k = m.group('p'), int(m.group('k'), 0)
            # ⚠ THE FUNCTION, NOT THE LOOP. `enclosing_block` deliberately reads
            # the innermost `{ }` because a Ghidra temporary is reused across a
            # function — but the cursor's PROVENANCE is by definition outside
            # the loop that steps it, and so is its declaration.
            bs = text.rfind('\n/* ---- ', 0, m.start())
            be = text.find('\n/* ---- ', m.start())
            region = text[bs if bs >= 0 else 0: be if be >= 0 else len(text)]
            # the element type is whatever the cursor was set from BEFORE it
            # started stepping; a cursor with two different sources names
            # nothing and is declined.
            srcs = {p.group('v') for p in
                    re.finditer(PROVENANCE % re.escape(cur), region)
                    if p.group('v') != cur.split('.')[0]}
            tys = set()
            for v in srcs:
                d = re.search(DECL_OF % re.escape(v), region)
                if d:
                    tys.add(d.group(1).strip())
            if len(tys) != 1:
                notes.append('%-26s cursor %s steps %d: %d element type(s) from '
                             '%s — declined'
                             % (fn, cur, k, len(tys), ', '.join(sorted(srcs)) or 'nothing'))
                declined += 1
                continue
            ety = tys.pop()
            sz = measure('sizeof(*(%s *)0)' % ety, inc, cache)
            if sz != k:
                # K is not the element size, so it is a step INSIDE the element
                # (`poly1 + 4` over an `MeVector3` is one float) and scales
                # correctly on its own. Not this defect.
                continue
            cand = (text[:m.start('k')] + '(int)sizeof(*(%s *)0)' % ety
                    + text[m.end('k'):])
            if compiles_identically(fn, cand, build, inc):
                text = cand
                fixed += 1
                notes.append('%-26s cursor %-14s steps sizeof(%s) = %d, was a literal'
                             % (fn, cur, ety, sz))
            else:
                declined += 1
                notes.append('%-26s cursor %s: sizeof(%s) is not byte-identical '
                             'at i386 — declined' % (fn, cur, ety))

        for m in reversed(list(UNIT_MISMATCH.finditer(text))):
            rep = ('*(%s *)((char *)&(%s)->%s + %s * (int)sizeof(%s))'
                   % (m.group('acc'), m.group('base'), m.group('f'),
                      m.group('k'), m.group('acc')))
            cand = text[:m.start()] + rep + text[m.end():]
            if compiles_identically(fn, cand, build, inc):
                text = cand
                fixed += 1
                notes.append('%-26s %s->%s[%s] indexed in POINTER units, read as '
                             '%s — re-spelled in %s units'
                             % (fn, m.group('base'), m.group('f'), m.group('k'),
                                m.group('acc'), m.group('acc')))
            else:
                declined += 1
                notes.append('%-26s %s->%s[%s]: not byte-identical at i386 — declined'
                             % (fn, m.group('base'), m.group('f'), m.group('k')))

        if text != text0:
            open(path, 'w').write(text)
    print('  word-counted table loops repaired  : %d' % fixed)
    print('  declined (reported, not guessed)   : %d' % declined)
    for n in notes:
        print('     %s' % n)
    return 0


if __name__ == '__main__':
    sys.exit(main())
