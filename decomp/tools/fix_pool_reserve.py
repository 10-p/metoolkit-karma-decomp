#!/usr/bin/env python3
"""fix_pool_reserve.py — a POOL RESERVATION frozen at the i386 element size.

`fix_narrow_pointers.py` rule G repairs the ALLOCATION: `keaPoolAlloc(n << 2,
"NAZ")` becomes `n * sizeof(void *)`, because `NAZ` is a `MeReal **` and its
elements double at LP64.  Nothing repaired the other half.  The kea pool is a
single arena, sized up front by `MdtKeaMemoryRequired`, and that function
computes the SAME sizes a second time —

    uVar27 = iVar12 >> 4;                          /* blocks * blocks       */
    if ((uVar27 & 0xf) == 0) { ANAZ = uVar27 << 2; }
    else { ... ANAZ = (uVar27 & 0xfffffff0) * 4 + 0x40; }

so at LP64 every allocation asks for twice what the arena reserved for it and
the pool runs dry mid-step.  Measured on `scene_boxes_on_plane`: a 5,664-byte
pool, 32 bytes left, and a 128-byte `NCZ` request at step 60 —

    MeFatalError{3}: Kea error:Memory pool size exceeded when allocating NCZ

★ THE ROUNDING ADDEND IS ELEMENTS TOO, AND SCALING ONLY THE STRIDE FIXES
NOTHING.  `(uVar27 & 0xfffffff0) * 4 + 0x40` is Ghidra's rendering of
`((n + 15) & ~15) * 4`: the `0x40` is `16 * 4`, the sixteen elements the
round-up adds.  With the matrices in these scenes `n` is under 16, so
`uVar27 & 0xfffffff0` is ZERO and the addend IS the whole reservation.  The
first patch scaled the `* 4` alone, changed the pool size by not one byte, and
read exactly like a wrong diagnosis.  `McdContactSimplify`'s alloca taught the
same lesson from the other end — see proven.txt LP64-COLLISION-CHAIN item 4.

WHICH TERMS, and it is not a guess in either half:

 1. THE ELEMENT SIZE PER POOL NAME comes from the allocation itself.  Every
    `keaPoolAlloc(expr, "NAME")` in the corpus says whether NAME's elements are
    pointers (`* sizeof(void *)`, written by rule G) or four bytes (`<< 2`).
 2. WHICH RESERVATION IS WHICH is anchored on the names Ghidra recovered from
    DWARF: the locals are `ANAZ`, `ANCZ`, `ANR`, `ANC`, `QNC`, and each ends
    with a pool name.  Those five are read directly.
 3. THE UNNAMED ONES — Ghidra kept no name for the Q matrix's NAZ and NCZ, so
    they arrive as `iVar22` and `iVar23` — are typed by their DERIVATION.  A
    reservation whose count comes from a SELF-MULTIPLICATION is a blocks*blocks
    array; one that does not is a blocks array.  The named five confirm that
    split rather than being assumed to fit it: `ANAZ`/`ANCZ` are squared and are
    pointer arrays, `ANR`/`ANC`/`QNC` are not squared and are not.  A group with
    no named anchor, or with anchors that disagree, is REFUSED.

⚠ ORDER: AFTER `fix_narrow_pointers.py`.  Fact 1 is read out of the repaired
allocation, and before that pass runs `NAZ` and `NR` are spelled identically —
`(count + 0xf & 0xfffffff0) << 2` both — so this would learn that nothing is a
pointer array and print a clean, wrong zero.

⚠ AND THE REPAIR HAS TO BE OFFERED TO THE COMPILER RATHER THAN DERIVED.
`uVar27 << 2` -> `uVar27 * (int)sizeof(void *)` is the obvious rewrite and it is
NOT byte-identical; `(uVar27 << 2) + uVar27 * ((int)sizeof(void *) - 4)` — the
anchor-and-correct form, zero at i386 — is.  For the addend sites the direct
rewrite IS identical.  Which one gcc schedules the same way is a property of the
surrounding function, so every candidate is compiled and the first that
reproduces the baseline object byte for byte is kept.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = '/tmp/kd_poolreserve'
BANNER = re.compile(r'(?m)^/\* ---- (\S+)')
I386_PTR = 4
PT = '(int)sizeof(void *)'

CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']

# `keaPoolAlloc(<expr>, "NAME")` — the call spans two lines once rule G has
# widened the stride, so this is matched across newlines deliberately.
ALLOC = re.compile(r'keaPoolAlloc\s*\(\s*(?P<expr>[^;]*?),\s*"(?P<name>\w+)"\s*\)')

# The three shapes Ghidra writes a `roundup16(n) * 4` reservation in. The first
# two are the already-16-aligned branch, the third the general one — and the
# `0x40` in it is sixteen ELEMENTS, not a constant.
RESERVE = [
    (re.compile(r'(?m)^[ \t]*(?P<lv>[A-Za-z_]\w*)\s*=\s*'
                r'(?P<rhs>(?P<v>[A-Za-z_]\w*)\s*<<\s*2)\s*;'),
     ['({rhs}) + {v} * ({pt} - 4)', '{v} * {pt}']),
    (re.compile(r'(?m)^[ \t]*(?P<lv>[A-Za-z_]\w*)\s*=\s*'
                r'(?P<rhs>(?P<v>[A-Za-z_]\w*)\s*\*\s*4)\s*;'),
     ['{rhs} + {v} * ({pt} - 4)', '{v} * {pt}']),
    (re.compile(r'(?m)^[ \t]*(?P<lv>[A-Za-z_]\w*)\s*=\s*'
                r'(?P<rhs>\(\s*(?P<v>[A-Za-z_]\w*)\s*&\s*0xfffffff0\s*\)\s*\*\s*4'
                r'\s*\+\s*0x40)\s*;'),
     ['({v} & 0xfffffff0) * {pt} + 0x10 * {pt}',
      '{rhs} + (({v} & 0xfffffff0) + 0x10) * ({pt} - 4)']),
]
SELF_MUL = re.compile(r'\b(?P<a>[A-Za-z_]\w*)\s*\*\s*(?P=a)\b')


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
    # ⚠ THE FILE HAS TO KEEP ITS NAME: gcc records the source basename in an
    # STT_FILE symbol, so identical text out of a scratch name is a different
    # object and reads exactly like a codegen change.
    src = os.path.join(d, fn)
    open(src, 'w').write(text)
    obj = os.path.join(d, fn[:-2] + '.probe.o')
    if subprocess.run(['gcc'] + CFLAGS + ['-I' + os.path.join(HERE, 'include')]
                      + includes(inc) + ['-c', '-o', obj, src],
                      capture_output=True).returncode:
        return False
    return open(ref, 'rb').read() == open(obj, 'rb').read()


def regions(text):
    out, marks = [], [m.start() for m in BANNER.finditer(text)] + [len(text)]
    for i in range(len(marks) - 1):
        out.append((marks[i], marks[i + 1]))
    return out


def origin(body, pos, var, depth=4):
    """The text `var` was last given before `pos`, skipping the conditional
    self-adjustments Ghidra emits (`if (iVar12 < 0) iVar12 = iVar12 + 0xf;`)
    and following temporaries one hop further.

    ⚠ Without the self-reference skip the derivation of `ANAZ` reads
    `iVar12 + 0xf` and the squaring — two statements above it — is never seen,
    which puts every pointer reservation in the four-byte group."""
    if depth <= 0:
        return ''
    best = None
    for m in re.finditer(r'(?m)^[ \t]*%s\s*=\s*(?P<rhs>[^;]*);' % re.escape(var), body):
        if m.start() >= pos:
            break
        if re.search(r'\b%s\b' % re.escape(var), m.group('rhs')):
            continue                    # a self-adjustment, not the origin
        best = m
    if not best:
        return ''
    rhs = best.group('rhs')
    more = ''
    for name in set(re.findall(r'\b[A-Za-z_]\w*\b', rhs)):
        if re.match(r'^[a-z]{1,2}Var\d+$', name) or name.startswith('local_'):
            more += ' ' + origin(body, best.start(), name, depth - 1)
    return rhs + more


def elem_sizes(srcdir, build):
    """Pool name -> 'ptr' or '4', read off every keaPoolAlloc in the corpus."""
    sizes, conflict = {}, set()
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        text = open(os.path.join(srcdir, fn), errors='ignore').read()
        for m in ALLOC.finditer(text):
            expr, name = m.group('expr'), m.group('name')
            if 'sizeof(void *)' in expr:
                kind = 'ptr'
            elif '<< 2' in expr or re.search(r'\*\s*4\b', expr):
                kind = '4'
            else:
                continue                # a byte size, nothing to scale
            if sizes.setdefault(name, kind) != kind:
                conflict.add(name)
    for n in conflict:
        sizes.pop(n, None)
    return sizes, conflict


def sites(body, sizes):
    """Every reservation in one function, with its group and its anchor.

    ⚠ A RESERVATION IS A PAIR OF BRANCHES, AND WITHOUT THAT TEST THE SQUARING
    OVER-MATCHES.  Ghidra renders `roundup16(n) * 4` as an if/else that assigns
    the SAME local twice — `ANAZ = uVar27 << 2` in the already-aligned arm and
    `ANAZ = (uVar27 & 0xfffffff0) * 4 + 0x40` in the other.  A lone `X = n * 4`
    is a plain array size, not a rounded one.  `iVar2 = iVar12 * 4` in this very
    function is the A matrix — `c4size * c4size` MeReals, a squared count that
    holds FLOATS — and the first version of this pass filed it with `ANAZ` on
    the strength of the squaring and offered to double it.  The round-up partner
    is what says the site is one of the pool's rounded element arrays."""
    out = []
    for rx, cands in RESERVE:
        for m in rx.finditer(body):
            lv, v = m.group('lv'), m.group('v')
            squared = bool(SELF_MUL.search(origin(body, m.start(), v)))
            named = next((n for n in sorted(sizes, key=len, reverse=True)
                          if lv.endswith(n)), None)
            out.append({'m': m, 'lv': lv, 'v': v, 'rhs': m.group('rhs'),
                        'group': squared, 'anchor': named, 'cands': cands,
                        'roundup': rx is RESERVE[2][0]})
    rounded = {x['lv'] for x in out if x['roundup']}
    return [x for x in out if x['lv'] in rounded]


def selftest():
    """`origin` must see through a conditional self-adjustment, because that is
    the one thing between `ANAZ` and its squaring."""
    body = ('  iVar12 = c4size * c4size;\n'
            '  if (iVar12 < 0) {\n    iVar12 = iVar12 + 0xf;\n  }\n'
            '  uVar27 = iVar12 >> 4;\n'
            '  ANAZ = uVar27 << 2;\n')
    got = origin(body, body.index('ANAZ'), 'uVar27')
    if not SELF_MUL.search(got):
        sys.exit('fix_pool_reserve: SELF-CHECK FAILED — the derivation of a '
                 'squared count reads %r, with no self-multiplication in it. '
                 'Every pointer reservation would be filed as four-byte and the '
                 'pass would print repairs that change nothing.' % got)
    plain = ('  iVar12 = c4size;\n  uVar27 = iVar12 >> 2;\n  ANR = uVar27 << 2;\n')
    if SELF_MUL.search(origin(plain, plain.index('ANR'), 'uVar27')):
        sys.exit('fix_pool_reserve: SELF-CHECK FAILED — an unsquared count reads '
                 'as squared, so every four-byte reservation would be widened.')


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else os.path.join(
        HERE, '..', 'Thirdparty', 'metoolkit')
    inc = os.path.join(root, 'include')
    selftest()

    sizes, conflict = elem_sizes(srcdir, build)
    ptr_names = sorted(n for n, k in sizes.items() if k == 'ptr')
    # ---- A SEARCH THAT COMES BACK EMPTY HAS TO SAY WHETHER IT COULD FIND
    # ANYTHING. If no pool name reads as a pointer array then rule G has not run
    # yet and every reservation below would be filed as four-byte — a clean zero
    # that means the ordering is wrong, not that there is nothing to do.
    if not ptr_names:
        sys.exit('fix_pool_reserve: NOT ONE keaPoolAlloc in the corpus allocates '
                 'a pointer array. fix_narrow_pointers.py rule G writes that '
                 '`* sizeof(void *)`, so zero here means this pass ran BEFORE it '
                 'and has nothing to learn the element size from.')

    fixed = declined = 0
    notes = ['pool element sizes read from the allocations: %s'
             % ', '.join('%s=%s' % (n, sizes[n]) for n in sorted(sizes))]
    if conflict:
        notes.append('names allocated inconsistently, ignored: %s'
                     % ', '.join(sorted(conflict)))

    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text0 = open(path, errors='ignore').read()
        if 'keaPoolAlloc' not in text0 and 'MemoryRequired' not in text0:
            continue
        text = text0
        for s, e in regions(text0):
            body = text0[s:e]
            found = sites(body, sizes)
            if len(found) < 2:
                continue
            # ---- WHICH GROUP IS THE POINTER GROUP, decided by the anchors and
            # refused if they cannot decide it.
            verdict = {}
            for g in (True, False):
                anchors = {sizes[x['anchor']] for x in found
                           if x['group'] is g and x['anchor']}
                if len(anchors) == 1:
                    verdict[g] = anchors.pop()
                elif anchors:
                    sys.exit('fix_pool_reserve: %s has reservations in one group '
                             'whose named anchors disagree (%s). The derivation '
                             'is not typing them and a guess here mis-sizes the '
                             'arena.' % (fn, ', '.join(sorted(anchors))))
            todo = [x for x in found
                    if verdict.get(x['group']) == 'ptr']
            unanchored = [x for x in found if x['group'] not in verdict]
            for x in unanchored:
                notes.append('%-22s %s has no named anchor in its group — declined'
                             % (fn, x['lv']))
                declined += 1
            if not todo:
                continue
            # ---- ALL OR NOTHING, and every candidate offered to the compiler.
            # A half-scaled arena overlaps DIFFERENTLY rather than not at all.
            chosen = {}
            for x in todo:
                for c in x['cands']:
                    rep = c.format(rhs=x['rhs'], v=x['v'], pt=PT)
                    cand = (text[:s + x['m'].start('rhs')] + rep
                            + text[s + x['m'].end('rhs'):])
                    if compiles_identically(fn, cand, build, inc):
                        chosen[x['m'].start('rhs')] = (x['m'].end('rhs'), rep, x)
                        break
                else:
                    notes.append('%-22s %s: no candidate spelling is '
                                 'byte-identical at i386 — declined'
                                 % (fn, x['lv']))
                    declined += 1
            if len(chosen) != len(todo):
                continue
            cand = text
            for st in sorted(chosen, reverse=True):
                en, rep, _x = chosen[st]
                cand = cand[:s + st] + rep + cand[s + en:]
            if not compiles_identically(fn, cand, build, inc):
                notes.append('%-22s the %d reservations are byte-identical alone '
                             'and not together — declined as a group'
                             % (fn, len(chosen)))
                declined += len(chosen)
                continue
            text = cand
            fixed += len(chosen)
            for st in sorted(chosen):
                _en, _rep, x = chosen[st]
                notes.append('%-22s %-8s %-22s -> %s elements'
                             % (fn, x['lv'], x['rhs'][:22], 'sizeof(void *)'))
        if text != text0:
            open(path, 'w').write(text)

    print('  pool reservations scaled to the element size    : %d' % fixed)
    print('  declined (reported, not guessed)                : %d' % declined)
    for n in notes:
        print('     %s' % n)
    return 0


if __name__ == '__main__':
    sys.exit(main())
