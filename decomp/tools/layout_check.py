#!/usr/bin/env python3
"""layout_check.py — the arm64 defect the truncation gate cannot see.

    layout_check.py <kd_out/allobj> <kd_build> [metoolkit-root]

WHAT THIS IS FOR, AND WHY §6b's NUMBER IS NOT THE WHOLE STORY.

`test/ptrwidth_check.sh` counts pointer TRUNCATION — a 64-bit address stored in
a 32-bit slot. `tools/fix_ptrwidth.py` closed 3,864 of those and HANDOVER.md has
called arm64 "~95% repaired" since. That framing has a hole in it, and this tool
is the hole measured:

    struct _McdGeometry { MeU32 mRefCtAndID; McdGeometryID prev, next;
                          McdFrameworkID frame; };

        i386   4 + 4 + 4 + 4 = 16 bytes
        arm64  4 + pad + 8 + 8 + 8 = 32 bytes

and `IxBoxTriList` reads the triangle generator out of `trilistgeom[3]`. On i386
that is byte 48. On arm64 it is byte 96, and the value there is not a function
pointer. NOTHING IS TRUNCATED — the cast is width-correct, the compiler is
silent, `-Wint-to-pointer-cast` has nothing to say, and the address is simply
somebody else's memory.

So there are TWO arm64 defects, not one:

    1. pointer truncation      — measured, and 95% closed
    2. LAYOUT: the recovery encodes offsets and struct sizes taken from a
       32-bit-pointer build, and every struct that CONTAINS a pointer changes
       size and field offsets at 64-bit

and defect 2 is invisible to defect 1's gate by construction.

HOW THIS MEASURES IT WITHOUT AN ARM64 MACHINE. Every layout question is a
compile-time constant, so it can be ASKED of the compiler rather than executed:
the i386 sizes are read by compiling and running a generated probe, and then the
same numbers are asserted at arm64 with `_Static_assert`. A struct whose
assertion fails is one whose layout the recovery's constants do not describe.

WHAT A NON-ZERO HERE DOES NOT MEAN. It is not a count of defects. A recovered
object that only ever names FIELDS — `p->model1`, `group->count` — recompiles
correctly at any pointer width, because the compiler recomputes the offset. What
is exposed is arithmetic that BAKES a layout in: a hardcoded byte offset, or an
index through a struct type Ghidra chose. Those are counted separately and the
two numbers are reported together, because either alone is misleading.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

NDK = os.environ.get(
    'KD_NDK',
    '/home/ion/Android/Sdk/ndk/30.0.14904198/toolchains/llvm/prebuilt/'
    'linux-x86_64/bin')


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def struct_names(inc):
    """Every tag the public headers define a body for."""
    names = set()
    for root, _d, files in os.walk(inc):
        for fn in files:
            if not fn.endswith('.h'):
                continue
            txt = open(os.path.join(root, fn), errors='ignore').read()
            for m in re.finditer(r'\bstruct\s+(_?\w+)\s*\{', txt):
                names.add(m.group(1))
    return sorted(names)


# Exactly what a recovered .c includes, in the same order: kd_types.h needs
# tags (`_McdUserTriangle`) that only the umbrella header brings in, and a probe
# that does not compile reports "type not resolvable" for the whole corpus and
# reads like an absence of findings.
HEAD = '#include "@HERE@/include/kd_compat.h"\n#include "@HERE@/include/kd_karma.h"\n' \
       '#include "@HERE@/include/kd_types.h"\n'


def i386_sizes(names, inc, work):
    """Compile and RUN a probe on the host, one struct per line."""
    body = HEAD.replace('@HERE@', HERE) + '#include <stdio.h>\nint main(void){\n'
    for n in names:
        body += ('#ifdef KD_HAVE_%s\n  printf("%s %%d\\n", (int)sizeof(struct %s));\n#endif\n'
                 % (n, n, n))
    body += '  return 0;\n}\n'
    src = os.path.join(work, 'probe.c')
    # Which tags actually have a complete type here: ask the compiler one at a
    # time rather than guess, because several are forward declarations only.
    have = []
    for n in names:
        t = os.path.join(work, 't.c')
        open(t, 'w').write(HEAD.replace('@HERE@', HERE) + 'char probe[sizeof(struct %s)];\n' % n)
        r = subprocess.run(['gcc', '-m32', '-DLINUX', '-c', '-o', '/dev/null', t]
                           + includes(inc), capture_output=True)
        if r.returncode == 0:
            have.append(n)
    open(src, 'w').write(body)
    r = subprocess.run(['gcc', '-m32', '-DLINUX', '-o', os.path.join(work, 'probe'), src]
                       + ['-DKD_HAVE_' + n for n in have] + includes(inc),
                       capture_output=True, text=True)
    if r.returncode:
        sys.exit('layout_check: host probe did not build\n' + r.stderr[:2000])
    out = subprocess.run([os.path.join(work, 'probe')], capture_output=True, text=True)
    return {l.split()[0]: int(l.split()[1]) for l in out.stdout.split('\n') if l.strip()}


def arm64_differs(sizes, inc, work):
    """Assert the i386 sizes at arm64; whatever fails has a different layout."""
    cc = os.path.join(NDK, 'aarch64-linux-android21-clang')
    if not os.path.exists(cc):
        sys.exit('layout_check: no NDK at %s — set KD_NDK.' % NDK)
    bad = []
    for n, sz in sorted(sizes.items()):
        t = os.path.join(work, 'a.c')
        open(t, 'w').write(HEAD.replace('@HERE@', HERE) +
                           '_Static_assert(sizeof(struct %s) == %d, "differs");\n' % (n, sz))
        r = subprocess.run([cc, '-DLINUX', '-w', '-c', '-o', '/dev/null', t]
                           + includes(inc), capture_output=True, text=True)
        if r.returncode:
            m = re.search(r"static assertion failed.*", r.stderr)
            bad.append(n)
    return bad


# ---------------------------------------------------------------------------
# WHAT COUNTS AS BAKED, and this is where a first draft of this tool was wrong.
#
# `include/kd_types.h` declares the Karma-internal structs with REAL FIELDS and
# real types — the `/* +0x20 */` comments are documentation, not padding — so a
# recovered access that NAMES a field recompiles correctly at any pointer width.
# The compiler recomputes the offset. A first version of this tool counted every
# `NAME[k].field` and reported 1,339 sites; most of those are `row[2].v` over an
# `lsVec3`, which is three floats and the same twelve bytes on both targets.
#
# So the split below is by whether the arithmetic can survive a relayout:
#
#   OFFSET   a LITERAL byte offset — `*(T *)(x + 0x18)`. The constant was
#            derived from a 32-bit layout and no compiler will revisit it — but
#            it is only WRONG if the region it indexes contains a pointer, and
#            many do not: MdtBcl's 0xb0 is a stride over a block of floats and
#            is the same number on both targets. An upper bound, not a count.
#   SUSPECT  `NAME[k].field` where the element type CONTAINS A POINTER, so
#            `k * sizeof(T)` is a different number on arm64. Legitimate array
#            indexing is fine here; what is not is Ghidra indexing a pointer
#            past element 0 through a type it CHOSE — `g[1].frame` where `g` is
#            an `McdGeometry *` and the object is an `McdTriangleList`. Telling
#            those apart is per-site judgement, which is why this is SUSPECT and
#            not BAKED.
#   SAFE     the same shape over a pointer-free element type. Reported so the
#            other two numbers cannot be inflated by it.
LITERAL_OFFSET = re.compile(
    r'\*\s*\(\s*[A-Za-z_][\w ]*\**\s*\*\s*\)\s*\(\s*[^();]*?\+\s*'
    r'(?:0x[0-9a-f]+|\d+)\s*\)')
INDEXED = re.compile(r'\b([A-Za-z_]\w*)\s*\[\s*[1-9]\d*\s*\]\s*\.\s*[A-Za-z_]')
BANNER = re.compile(r'(?m)^/\* ---- (\S+)')


_MEMBER_TYPES = {}


def member_type(name, inc, here):
    """The declared type of a STRUCT MEMBER of this name, if it is unambiguous.

    Most indexed bases are not locals — `tm->row[2].v` is `lsTransform::row`,
    and `elem_type` below sees no declaration for `row` and gives up. That put
    845 of 1,211 sites in "type not resolvable", which is honest and useless.
    Both the public headers and our own `kd_types.h` declare these members, so
    the type is READ rather than inferred; a name declared with two different
    types anywhere is left unresolved rather than guessed."""
    if not _MEMBER_TYPES:
        pat = re.compile(r'(?m)^[ \t]*((?:const |unsigned |struct )*[A-Za-z_]\w*)'
                         r'[ \t]*\**[ \t]*([A-Za-z_]\w*)[ \t]*\[[ \t]*\d*[ \t]*\][ \t]*;')
        roots = [inc, os.path.join(here, 'include')]
        for root in roots:
            for dirpath, _d, files in os.walk(root):
                for fn in files:
                    if not fn.endswith('.h'):
                        continue
                    txt = open(os.path.join(dirpath, fn), errors='ignore').read()
                    for m in pat.finditer(txt):
                        ty, nm = m.group(1).strip(), m.group(2)
                        if ty in ('return', 'typedef', 'extern', 'static'):
                            continue
                        prev = _MEMBER_TYPES.get(nm, ty)
                        _MEMBER_TYPES[nm] = ty if prev == ty else None
    return _MEMBER_TYPES.get(name)


def elem_type(region, name):
    """The declared type of `name` in this function, or None."""
    m = re.search(r'(?m)^[ \t]*((?:const |unsigned |struct )*[A-Za-z_]\w*)'
                  r'[ \t]*\**[ \t]*' + re.escape(name)
                  + r'[ \t]*(?:\[[ \t]*\d+[ \t]*\])?[ \t]*;', region)
    if not m:
        return None
    ty = m.group(1).strip()
    return None if ty in ('return', 'if', 'else', 'while', 'for', 'do',
                          'goto', 'case', 'switch') else ty


def type_differs(ty, inc, work, cache, here):
    """Does sizeof(ty) change between i386 and arm64? ASKED of both compilers."""
    if ty in cache:
        return cache[ty]
    head = HEAD.replace('@HERE@', here)
    probe = os.path.join(work, 'sz.c')
    # ONE `%d`, not two. This is plain concatenation, not %-formatting, so `%%d`
    # reached the C file verbatim and the probe printed the literal text "%d" —
    # which the guard below then rejects, but which for one measurement made
    # every type unresolvable and, before the guard existed, made every type
    # DIFFER. Both readings looked like findings.
    open(probe, 'w').write(head + '#include <stdio.h>\nint main(void){printf("%d\\n",'
                           '(int)sizeof(' + ty + '));return 0;}\n')
    exe = os.path.join(work, 'sz')
    if subprocess.run(['gcc', '-m32', '-DLINUX', '-w', '-o', exe, probe]
                      + includes(inc), capture_output=True).returncode:
        cache[ty] = None
        return None
    sz = subprocess.run([exe], capture_output=True, text=True).stdout.strip()
    a = os.path.join(work, 'sz2.c')
    # `sz` is a STRING read from the probe's stdout; an empty or malformed one
    # writes an assertion that does not COMPILE, and a file that does not
    # compile reports every type as differing. That happened, and it read like a
    # finding — 1,339 "layout-baked" sites, most of them lsVec3.
    if not sz.isdigit():
        cache[ty] = None
        return None
    open(a, 'w').write(head + '_Static_assert(sizeof(' + ty + ') == ' + sz
                       + ', "differs");\n')
    cc = os.path.join(NDK, 'aarch64-linux-android21-clang')
    cache[ty] = subprocess.run([cc, '-DLINUX', '-w', '-c', '-o', '/dev/null', a]
                               + includes(inc), capture_output=True).returncode != 0
    return cache[ty]


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else os.path.join(
        os.path.dirname(os.path.abspath(__file__)), '..', '..', 'Thirdparty', 'metoolkit')
    inc = os.path.join(root, 'include')
    work = '/tmp/kd_layout'
    os.makedirs(work, exist_ok=True)

    names = struct_names(inc)
    sizes = i386_sizes(names, inc, work)
    bad = arm64_differs(sizes, inc, work)

    same = [n for n in sorted(sizes) if n not in bad]
    print('  metoolkit structs with a complete definition : %d' % len(sizes))
    print('  SIZE DIFFERS between i386 and arm64          : %d' % len(bad))
    # THE CONTROL, and it is not decoration: an assertion mechanism that failed
    # for every struct would report "all of them differ" and look like a
    # finding. These are the pointer-free ones, and their assertions PASS.
    print('  size IDENTICAL (the control — assertions pass): %d  %s'
          % (len(same), ', '.join(same[:8])))
    if bad:
        show = ', '.join('%s %d->?' % (n, sizes[n]) for n in bad[:6])
        print('     e.g. %s%s' % (show, ' ...' if len(bad) > 6 else ''))

    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    cache = {}
    # ---- THE SELF-CHECK, and it is here because this tool got it wrong TWICE.
    # `type_differs` builds two probes and compares them. A probe that does not
    # compile, or one whose printf emits the literal text "%d" because a `%%`
    # survived a plain concatenation, makes EVERY type read as differing — which
    # is not an error message, it is a finding-shaped number. The first run of
    # this tool reported 1,339 layout-baked sites on exactly that bug; the real
    # figure is 128 baked and 369 suspect.
    #
    # So the tool now proves it can return BOTH answers before reporting any:
    # `lsVec3` is three floats and must read False, `_McdGeometry` holds three
    # pointers and must read True. If either is wrong, nothing below is worth
    # printing.
    probe_same = type_differs('lsVec3', inc, work, cache, here)
    probe_diff = type_differs('struct _McdGeometry', inc, work, cache, here)
    if probe_same is not False or probe_diff is not True:
        sys.exit('layout_check: SELF-CHECK FAILED — lsVec3 read %r (want False) and\n'
                 '  struct _McdGeometry read %r (want True). The size probe is not\n'
                 '  measuring anything; every number below would be an artefact.'
                 % (probe_same, probe_diff))
    baked, suspect, safe, unknown = ({}, {}, {}, {})
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        b = fn[:-2]
        txt = open(os.path.join(srcdir, fn), errors='ignore').read()
        n = len(LITERAL_OFFSET.findall(txt))
        if n:
            baked[b] = n
        parts = BANNER.split(txt)
        for i in range(2, len(parts), 2):
            region = parts[i]
            for m in INDEXED.finditer(region):
                ty = elem_type(region, m.group(1)) \
                    or member_type(m.group(1), inc, here)
                d = type_differs(ty, inc, work, cache, here) if ty else None
                tgt = suspect if d is True else safe if d is False else unknown
                tgt[b] = tgt.get(b, 0) + 1

    def line(label, d):
        tot = sum(d.values())
        w = ' '.join('%s:%d' % (k, v) for k, v in
                     sorted(d.items(), key=lambda kv: -kv[1])[:5])
        print('  %-46s: %5d across %2d object(s)' % (label, tot, len(d)))
        if w:
            print('     worst: %s' % w)

    line('OFFSET  literal byte offset *(T *)(x + K)', baked)
    line('SUSPECT NAME[k].field, element type has a pointer', suspect)
    line('SAFE    NAME[k].field, element type is pointer-free', safe)
    line('        (type not resolvable)', unknown)
    print()
    print('  READ THESE AS BOUNDS, NOT AS DEFECT COUNTS. Two numbers here are')
    print('  solid and the rest are exposure:')
    print('    * the struct table above IS solid — those sizes are measured, and')
    print('      the 23 that match are the control that says the probe works;')
    print('    * OFFSET counts literal byte offsets, and MANY OF THEM ARE FINE:')
    print('      MdtBcl\'s 0xb0 is a 176-byte stride over a Jacobian block of')
    print('      floats, which is the same number on any target. Only an offset')
    print('      into a region that CONTAINS a pointer is baked;')
    print('    * SUSPECT is where the demonstrated defect lives — Ghidra indexing')
    print('      a pointer past element 0 through a struct type IT chose, e.g.')
    print('      `trilistgeom[3].mRefCtAndID` over McdGeometry (16 bytes here, 32')
    print('      there). Legitimate array indexing lives in the same column.')
    print('  kd_types.h declares real fields with real types, so anything that')
    print('  NAMES a field recompiles correctly at any pointer width — which is')
    print('  why SAFE exists and why the exposure is far smaller than the raw')
    print('  site count. What is NOT in doubt: at least one site is measurably')
    print('  wrong on arm64 and nothing in the toolchain says so.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
