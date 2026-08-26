#!/usr/bin/env python3
"""assoc_scan.py — enumerate the sites the association defect can hide in.

    assoc_scan.py <kd_out/allobj> [kd_build]

HANDOVER.md §11 item 2a: Ghidra prints a right-leaning float `+` chain FLAT.
`a + b + c` is printed for both `(a+b)+c` and `a+(b+c)`, C parses it
left-leaning, and float addition is not associative. On i386 the x87 keeps 64
mantissa bits so these sums are EXACTLY equal either way — measured 0 in
2,000,000 — and under storage precision, which is what wasm32, armv7 and arm64
all give, they differ in 31%. Every behavioural gate in this project runs on
i386, so all of them are structurally blind to it.

THE ASSOCIATION HALF HAS NO TEXTUAL FINGERPRINT. That is the whole difficulty:
a correct chain and a wrong one look identical. So this does not find defects —
it BOUNDS them. It reports every place one could be hiding, which turns "nobody
has looked at the other 152 objects" into a finite, ranked list.

WHAT COUNTS AS A CANDIDATE. A `+` chain at bracket depth zero with THREE OR MORE
terms, at least one of which mentions a float-typed local declared in the same
function. Two-term chains are excluded because `a + b` is unambiguous — there is
only one tree. Integer chains are excluded because integer addition IS
associative in the only sense that matters here.

WHY IT IS DELIBERATELY CONSERVATIVE ABOUT `float`-ness. It reads the function's
own declarations rather than guessing from names, so a chain over struct fields
whose types live in the DWARF is MISSED. The number this prints is therefore a
LOWER BOUND on the exposure, and it says so. A tool that guessed would produce a
bigger number that nobody could act on.
"""
import os
import re
import sys

DECL = re.compile(r'(?m)^\s*(?:const\s+)?(float|MeReal|double)\s+([A-Za-z_]\w*)'
                  r'\s*(?:\[[^\]]*\])?\s*;')
BANNER = re.compile(r'(?m)^/\* ---- (\S+)')


def split_top(expr):
    """Terms of `expr` split on `+` at bracket depth zero."""
    out, depth, cur = [], 0, ''
    i = 0
    while i < len(expr):
        c = expr[i]
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
        if c == '+' and depth == 0:
            # not a `++`, and not the sign of an exponent
            if expr[i - 1:i] != '+' and expr[i + 1:i + 2] != '+' \
                    and not re.match(r'[0-9]e$', expr[max(0, i - 2):i], re.I):
                out.append(cur)
                cur = ''
                i += 1
                continue
        cur += c
        i += 1
    out.append(cur)
    return [p.strip() for p in out if p.strip()]


def rhs_of(s):
    """The assigned expression, or the whole thing when there is no assignment.

    NOT `s.split('=', 1)[1]`. A chunk between semicolons carries whatever
    control flow preceded it — `if (uVar5 != 0) { fVar1 = myw[0] * ...` — and
    splitting on the FIRST `=` cuts inside the `!=`, which silently turned the
    one site in this project with known ground truth into a single unparsable
    term. Ghidra spells assignment ` = ` with spaces, so the last one at bracket
    depth zero is the operator, and `!=`/`==`/`<=`/`>=` never match it."""
    depth, at = 0, -1
    for i, c in enumerate(s):
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
        elif depth == 0 and c == '=' and s[i - 1:i] not in ('!', '=', '<', '>',
                                                            '+', '-', '*', '/',
                                                            '&', '|', '^', '%') \
                and s[i + 1:i + 2] != '=':
            at = i
    return s[at + 1:] if at >= 0 else s


def scan_function(text):
    """[(term count, permuted?, statement)] for flat float chains of 3+ terms."""
    floats = {m.group(2) for m in DECL.finditer(text)}
    if not floats:
        return []
    word = re.compile(r'[A-Za-z_]\w*')
    hits = []
    for stmt in re.split(r';', text):
        s = ' '.join(stmt.split())
        if s.count('+') < 2:
            continue
        terms = split_top(rhs_of(s))
        if len(terms) < 3:
            continue
        if not any(w in floats for t in terms for w in word.findall(t)):
            continue
        hits.append((len(terms), subscript_order(terms), s[:140]))
    return hits


SUBS = re.compile(r'\[\s*(\d+)\s*\]')


def subscript_order(terms):
    """'permuted' when every term carries ONE constant subscript and the
    sequence is not ascending; 'ordered' when it is; None when undecidable.

    THE ONE SITE IN THIS PROJECT WHERE GROUND TRUTH EXISTS HAS THIS SHAPE, and
    that is the entire justification for singling it out. Epic copied
    MathEngine's integrator verbatim into `KDynStep.cpp`, so for
    `KeaIntegrateSystem_vanilla` the original source is readable:

        Ghidra       myw[2]*axis[2] + myw[0]*axis[0] + myw[1]*axis[1]
        KDynStep:647 myw[0]*axis[0] + myw[1]*axis[1] + myw[2]*axis[2]

    a dot product accumulated in INDEX ORDER, which Ghidra printed permuted.
    Two of the three fixed sites in that function are this shape.

    IT IS A RANKING, NOT A VERDICT, and the difference matters. On i386 a
    reassociation of these sums is EXACTLY inert, so no gate here can falsify a
    repair — reversing all 41 flat chains in `keaMatrix_PcSparse_vanilla`
    changed the object file and changed nothing measurable. So this column says
    "look here first", and the machine code still has to settle each one."""
    idx = []
    for t in terms:
        m = SUBS.findall(t)
        if len(m) < 1 or len(set(m)) != 1:
            return None
        idx.append(int(m[0]))
    if len(set(idx)) != len(idx):
        return None
    return 'permuted' if idx != sorted(idx) else 'ordered'


def main():
    srcdir = sys.argv[1]
    build = sys.argv[2] if len(sys.argv) > 2 else None
    rows, total, inbuild, perm, perm_build = [], 0, 0, 0, 0
    detail = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c'):
            continue
        base = fn[:-2]
        text = open(os.path.join(srcdir, fn), errors='ignore').read()
        parts = BANNER.split(text)
        n, worst, p = 0, 0, 0
        for i in range(1, len(parts), 2):
            for cnt, order, stmt in scan_function(parts[i + 1]):
                n += 1
                worst = max(worst, cnt)
                if order == 'permuted':
                    p += 1
                    detail.append((base, parts[i], stmt))
        if n:
            here = bool(build and os.path.exists(os.path.join(build, base + '.o')))
            rows.append((n, base, worst, p, here))
            total += n
            perm += p
            inbuild += n if here else 0
            perm_build += p if here else 0
    rows.sort(reverse=True)
    print('  sites  object                          longest  permuted  in build')
    print('  ' + '-' * 72)
    for n, base, worst, p, here in rows:
        print(f'  {n:5}  {base:32} {worst:5}   {p:6}    {"yes" if here else "no"}')
    print(f'\n  {total} candidate site(s) across {len(rows)} object(s); '
          f'{inbuild} in objects that are IN THE BUILD.')
    print(f'  {perm} of them are INDEX-PERMUTED dot products ({perm_build} in the build)'
          ' — the\n  shape of the one site where ground truth exists (KDynStep.cpp). '
          'Look there first.')
    print('  A LOWER BOUND — a chain over struct fields is not counted, because')
    print('  their types are in the DWARF and this reads only local declarations.')
    print('  These are places the defect CAN hide, not defects: the association')
    print('  half has no textual fingerprint, and on i386 it is exactly inert, so')
    print('  no gate in this project can falsify a repair. §11 item 2a.')
    if '-v' in sys.argv:
        print('\n  index-permuted sites:')
        for base, fnname, stmt in detail:
            print(f'    {base}:{fnname}\n      {stmt}')


if __name__ == '__main__':
    main()
