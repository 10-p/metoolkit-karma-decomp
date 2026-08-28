#!/usr/bin/env python3
"""fix_word_loops.py — a table-zeroing loop whose trip count is in 4-BYTE WORDS.

    fix_word_loops.py <kd_out/allobj> <kd_build> [metoolkit-root]

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

SCOPE: two loops in the corpus have this exact shape. It is a post-pass rather
than a `ghidra_clean` rule because it needs a compiler to measure `offsetof`.
Run it on a COPY: it edits in place.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = '/tmp/kd_wordloop'

# for (V = COUNT; V != 0; V = V - 1) { P->F1 = ...; P = (T *)&P->F2; }
LOOP = re.compile(
    r'for \((?P<v>\w+) = (?P<count>[^;]+?); (?P=v) != 0; (?P=v) = (?P=v) (?:- 1|\+ -1)\) \{\s*'
    r'(?P<p>\w+)->(?P<f1>\w+) = [^;]+;\s*'
    r'(?P=p) = \((?P<ty>[\w ]+) \*\)&(?P=p)->(?P<f2>\w+);\s*\}', re.S)

# `(uint)(EXPR * K) >> k` — Ghidra's rendering of `bytes / 4`.
SHIFTED = re.compile(r'^\(uint\)\((?P<expr>.*?)\s*\*\s*(?P<sz>0x[0-9a-f]+|\d+)\s*\)'
                     r'\s*>>\s*(?P<k>\d+)$', re.S)

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


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else os.path.join(
        HERE, '..', 'Thirdparty', 'metoolkit')
    inc = os.path.join(root, 'include')
    cache = {}

    # ---- THE SELF-CHECK. A probe that cannot compile returns None for every
    # measurement, every site declines, and "0 loops repaired" reads exactly
    # like "there were none to repair".
    probe = measure('sizeof(*(McdInteractions *)0)', inc, cache)
    if probe != 28:
        sys.exit('fix_word_loops: SELF-CHECK FAILED — sizeof(McdInteractions) '
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
        edits = []
        for m in LOOP.finditer(text):
            ty = m.group('ty').strip()
            raw = re.sub(r'\s+', ' ', m.group('count')).strip()
            step = measure('((char *)&((%s *)0)->%s - (char *)0)'
                           % (ty, m.group('f2')), inc, cache)
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
                   % (count_expr, ty, ty, m.group('f2')))
            edits.append((m.start('count'), m.end('count'), rep,
                          '%-26s %s trip count: %d-byte words -> / offsetof(%s, %s)'
                          % (fn, ty, step, ty, m.group('f2'))))
        for start, end, rep, note in sorted(edits, key=lambda e: -e[0]):
            cand = text[:start] + rep + text[end:]
            if compiles_identically(fn, cand, build, inc):
                text = cand
                fixed += 1
                notes.append(note)
            else:
                declined += 1
                notes.append('%-26s DECLINED: the repair is not byte-identical '
                             'at i386' % fn)
        if fixed and edits:
            open(path, 'w').write(text)
    print('  word-counted table loops repaired  : %d' % fixed)
    print('  declined (reported, not guessed)   : %d' % declined)
    for n in notes:
        print('     %s' % n)
    return 0


if __name__ == '__main__':
    sys.exit(main())
