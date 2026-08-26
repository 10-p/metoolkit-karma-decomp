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


HEAD = '#include <MePrecision.h>\n#include <McdCTypes.h>\n#include <MdtTypes.h>\n' \
       '#include <McdFrame.h>\n#include <MstTypes.h>\n#include <MeMemory.h>\n' \
       '#include <MeMath.h>\n#include <MeHeap.h>\n'


def i386_sizes(names, inc, work):
    """Compile and RUN a probe on the host, one struct per line."""
    body = HEAD + '#include <stdio.h>\nint main(void){\n'
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
        open(t, 'w').write(HEAD + 'char probe[sizeof(struct %s)];\n' % n)
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
        open(t, 'w').write(HEAD +
                           '_Static_assert(sizeof(struct %s) == %d, "differs");\n' % (n, sz))
        r = subprocess.run([cc, '-DLINUX', '-w', '-c', '-o', '/dev/null', t]
                           + includes(inc), capture_output=True, text=True)
        if r.returncode:
            m = re.search(r"static assertion failed.*", r.stderr)
            bad.append(n)
    return bad


# Arithmetic that BAKES a layout in, as opposed to naming a field.
BAKED = [
    (re.compile(r'\*\s*\(\s*[A-Za-z_][\w ]*\**\s*\*\s*\)\s*\(\s*[^();]*?\+\s*'
                r'(?:0x[0-9a-f]+|\d+)\s*\)'), 'hardcoded byte offset'),
    (re.compile(r'\b[A-Za-z_]\w*\s*\[\s*[1-9]\d*\s*\]\s*\.\s*[A-Za-z_]'),
     'index through a struct type at a constant > 0'),
]


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

    tot, objs, worst = 0, 0, []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        txt = open(os.path.join(srcdir, fn), errors='ignore').read()
        n = sum(len(p.findall(txt)) for p, _w in BAKED)
        if n:
            objs += 1
            tot += n
            worst.append((n, fn[:-2]))
    worst.sort(reverse=True)
    print('  sites in the build that BAKE a layout in     : %d across %d object(s)'
          % (tot, objs))
    print('     worst: %s' % ' '.join('%s:%d' % (b, n) for n, b in worst[:6]))
    print('  -> a size difference alone is not a defect; a BAKED site over a')
    print('     struct whose size differs is. Neither number is the other.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
