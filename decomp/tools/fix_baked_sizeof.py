#!/usr/bin/env python3
"""fix_baked_sizeof.py — an allocation SIZE frozen at the i386 value.

    fix_baked_sizeof.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT, which `test/lp64_run.sh` found on the first statement of the first
scene and which neither `ptrwidth_check.sh` nor `layout_check.py` counts:

    w = (MeMemoryAPI.create)(0x234);          MdtWorld.c:95
    ...
    (w->params).lengthScale = lengthScale;    MdtWorld.c:98  <- heap overflow

0x234 is 564, which is `sizeof(MdtWorld)` ON i386. At LP64 the struct is 880
bytes, so the very first allocation is 316 bytes short and the next statement
writes past it. Nothing is truncated, no cast is narrowed, and no compiler says
a word.

THE REWRITE IS DERIVED FROM TWO INDEPENDENT FACTS, and it refuses unless they
agree — which is what makes it a repair rather than a guess:

  * the ASSIGNMENT TARGET names the type. `w` is declared `MdtWorldID`, which
    is `MdtWorld *`, so the thing being allocated is an `MdtWorld`;
  * the LITERAL confirms it. `sizeof(MdtWorld)` measured at i386 must equal the
    constant — or divide it exactly, for the array allocations
    (`McdBatch` asks for 96000 bytes of `McdModelPair`).

Neither alone is enough. The size alone is ambiguous — many structs are 0x50
bytes — and the type alone would let a byte buffer be rewritten as an array of
something. Together they pin it, and a site where they disagree is left ALONE
and reported.

WHY IT CANNOT REGRESS i386, wasm32 OR armv7: on those targets `sizeof(T)` IS
the constant it replaces, by the check above. It is written `(int)sizeof(...)`
and the cast is load-bearing — `sizeof` is `size_t`, so `count * sizeof(T)` is
UNSIGNED where `count * 0x98` was `int`, with different overflow rules, and gcc
emits a different loop for it. The i386 acceptance test caught exactly that:
CxSmallSort's object grew by 64 bytes. No metoolkit struct is near 2 GB, so the
cast costs nothing. So the acceptance test is the one
this project always uses — every object recompiled from the rewritten sources is
BYTE-IDENTICAL — and it passes by construction. Verify it anyway.

It is a POST-PASS, like `fix_ptrwidth.py`, because it needs a compiler to
measure `sizeof`. §4's 95-second output is not the LP64-correct source; this and
`fix_ptrwidth.py` together are what make it so. Run it on a COPY: it edits in
place.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = '/tmp/kd_sizeof'

# `X = [(cast)] (MeMemoryAPI.create)(LITERAL[, ...])`
ALLOC = re.compile(
    r'(?m)^(?P<ind>[ \t]*)(?P<var>[A-Za-z_]\w*)[ \t]*=[ \t]*'
    r'(?P<pre>\(?[ \t]*(?:\([A-Za-z_][\w ]*\**\)[ \t]*)?\(?)'
    r'(?P<fn>MeMemoryAPI\.\w+|MePoolAPI\.\w+)\)?[ \t]*\([ \t]*'
    r'(?P<size>(?:[A-Za-z_]\w*|\([^()]*\))[ \t]*(?:<<|\*)[ \t]*(?:0x[0-9a-f]+|\d+)'
    r'|0x[0-9a-f]+|\d+)(?P<rest>[,)])')

# `poolSize << 2` — a COUNT times the i386 size of the element, which for a
# `void **` is the pointer size. Same defect as a baked `sizeof`, different
# spelling, and it is the one that survives into MePoolFixedInit:
#     ppvVar2 = (MeMemoryAPI.create)(poolSize << 2);   /* void ** */
# 4 is sizeof(void *) HERE and 8 at LP64, so the free-list is half the size it
# needs to be and the loop below it writes off the end.
SCALED = re.compile(r'^(?P<expr>[A-Za-z_]\w*|\([^()]*\))[ \t]*(?P<op><<|\*)[ \t]*'
                    r'(?P<k>0x[0-9a-f]+|\d+)$')


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


HEAD = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
        '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))


def elem_size(ty, inc, cache):
    """sizeof(*x) for a variable declared `ty x`, measured at i386. None if the
    type is not a pointer to a complete type — `void *`, an opaque handle, or a
    tag this translation unit invents."""
    if ty in cache:
        return cache[ty]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p.c')
    open(src, 'w').write(HEAD + '#include <stdio.h>\n'
                         'int main(void){printf("%d\\n",(int)sizeof(*(' + ty + ')0));'
                         'return 0;}\n')
    exe = os.path.join(WORK, 'p')
    if subprocess.run(['gcc', '-m32', '-DLINUX', '-w', '-o', exe, src]
                      + includes(inc), capture_output=True).returncode:
        cache[ty] = None
        return None
    out = subprocess.run([exe], capture_output=True, text=True).stdout.strip()
    cache[ty] = int(out) if out.isdigit() and int(out) > 0 else None
    return cache[ty]


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else os.path.join(HERE, '..', 'Thirdparty',
                                                              'metoolkit')
    inc = os.path.join(root, 'include')
    cache = {}

    # ---- THE SELF-CHECK. A probe that cannot compile returns None for every
    # type, which reads as "nothing to fix" — an absence of findings that looks
    # exactly like success. `MdtWorldID` must measure 564; if it does not, the
    # measurement is broken and nothing below should be believed.
    probe = elem_size('MdtWorldID', inc, cache)
    if probe != 564:
        sys.exit('fix_baked_sizeof: SELF-CHECK FAILED — sizeof(*(MdtWorldID)0) '
                 'measured %r, want 564. The size probe is not measuring '
                 'anything.' % probe)

    fixed = declined = 0
    reasons = {}
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        out, last, n = [], 0, 0
        for m in ALLOC.finditer(text):
            raw = m.group('size').strip()
            scaled = SCALED.match(raw)
            lit = 0 if scaled else int(raw, 0)
            d = re.search(r'(?m)^[ \t]*((?:const |struct )*[A-Za-z_]\w*[ \t]*\**)'
                          r'[ \t]*' + re.escape(m.group('var')) + r'[ \t]*;', text)
            why = None
            if not scaled and lit < 8:
                why = 'size below 8 bytes'
            elif not d:
                why = 'target has no local declaration'
            else:
                ty = re.sub(r'\s+', ' ', d.group(1)).strip()
                sz = elem_size(ty, inc, cache)
                if sz is None:
                    why = 'target type %s is not a pointer to a complete type' % ty
                elif scaled is not None:
                    k = int(scaled.group('k'), 0)
                    k = (1 << k) if scaled.group('op') == '<<' else k
                    if k != sz:
                        why = ('%s elements are %d bytes, the scale is %d'
                               % (ty, sz, k))
                    else:
                        rep = '(%s) * (int)sizeof(*(%s)0)' % (scaled.group('expr'), ty)
                elif lit == sz:
                    rep = '(int)sizeof(*(%s)0)' % ty
                elif lit % sz == 0:
                    rep = '%d * (int)sizeof(*(%s)0)' % (lit // sz, ty)
                else:
                    why = ('%s is %d bytes and does not divide %d'
                           % (ty, sz, lit))
            if why:
                declined += 1
                reasons[why.split(' is ')[0][:40]] = reasons.get(
                    why.split(' is ')[0][:40], 0) + 1
                continue
            out.append(text[last:m.start('size')])
            out.append(rep)
            last = m.end('size')
            n += 1
        if n:
            out.append(text[last:])
            open(path, 'w').write(''.join(out))
            fixed += n
    print('  allocation sizes rewritten from a literal to sizeof : %d' % fixed)
    print('  declined (reported, not guessed)                    : %d' % declined)
    for r, c in sorted(reasons.items(), key=lambda kv: -kv[1])[:6]:
        print('     %4d  %s' % (c, r))
    print('  -> no-op on i386/wasm32/armv7 by construction: the constant IS')
    print('     sizeof there. Acceptance test is every .o byte-identical.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
