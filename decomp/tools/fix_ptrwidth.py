#!/usr/bin/env python3
"""fix_ptrwidth.py — make the recovery's pointer punning survive 64-bit pointers.

    fix_ptrwidth.py <kd_out/allobj> <kd_build> [metoolkit-root]

HANDOVER.md §6b: the recovery puns pointers through integer slots constantly,
because Ghidra recovers a stack slot as an integer and the code stores an
address in it —

    *(T *)((int)base + K)

On i386, wasm32 and armv7 a pointer is 32 bits and that round-trip is lossless.
On arm64 it truncates, and `test/ptrwidth_check.sh` counts 7,771 sites across 89
of 139 objects. §6b says the fix "is a generator-wide change, not a flag", and
this is that change.

WHY THIS IS DRIVEN BY THE COMPILER AND NOT BY A PATTERN
-------------------------------------------------------
The rewrite is only correct where the integer holds an ADDRESS. Widening an
ordinary `int` changes truncation behaviour on arm64 and nowhere else, and there
is no gate in this project that executes arm64 code — so a heuristic that is
wrong 1% of the time would be undetectable. Textually `(int)x` says nothing
about whether `x` is a pointer.

Clang already knows. `-Wpointer-to-int-cast` fires exactly when a pointer is
cast to something narrower and names the column of the offending cast:

    137:27: warning: cast to smaller integer type 'int' from 'MeReal *'
      137 |       pfVar12 = (float *)((int)&convex->edge->invLength + local_a8);
          |                           ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~

so the set of sites is a real compiler's judgement about types, not a guess, and
a site this cannot locate is left alone rather than approximated.

WHY IT CANNOT REGRESS THE TARGETS THAT WORK
-------------------------------------------
`intptr_t` is not merely the same SIZE as `int` on i386 — it is the same TYPE.
So every rewritten cast is spelled differently and means precisely what it meant
before on i386, wasm32 and armv7, and the acceptance test (every already-built
`.o` byte-identical) passes by construction. Verify it anyway; that is the rule.

`-Wint-to-pointer-cast` is NOT rewritten directly. It is the other half of the
same round trip — `(T *)(narrow)` — and it disappears on its own once the inner
cast is width-correct. What remains after the loop settles is a genuinely
integer-valued address (Ghidra's `(code *)0x10074` and the like) and is
reported, not patched: those are a different defect with a different answer.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

# The Android NDK's llvm bin/ — a real external toolchain, discovered from the
# standard NDK variables rather than pinned to one version. The old pinned r30
# path stopped existing on an NDK upgrade and the failure read like a compiler
# bug rather than a missing directory.
NDK = kd_paths.NDK_BIN
CC = os.path.join(NDK, 'aarch64-linux-android21-clang') if NDK else ''

# The cast spellings the corpus actually puns through, and what each becomes.
# Signedness is preserved: `(uint)ptr` must stay unsigned or a comparison it
# feeds changes meaning.
WIDEN = {
    'int': 'kd_iptr',
    'uint': 'kd_uptr',
    'unsigned int': 'kd_uptr',
    'MeI32': 'kd_iptr',
    'MeU32': 'kd_uptr',
    'undefined4': 'kd_uptr',
    'MeBool': 'kd_iptr',
    'MeXMLElementID': 'kd_iptr',
    'McdTriangleFlags': 'kd_uptr',
}

DIAG = re.compile(r'^(?P<file>[^:]+):(?P<line>\d+):(?P<col>\d+): warning: '
                  r'(?P<msg>.*) \[-W(?P<kind>[a-z-]+)\]$')

# `(int)` / `( unsigned int )` at a known column.
CAST_AT = re.compile(r'\(\s*(?P<ty>[A-Za-z_][\w ]*?)\s*\)')


def cflags(root):
    inc = os.path.join(root, 'include')
    here = kd_paths.MD          # here/include holds the three kd_*.h
    cf = ['-O2', '-fno-strict-aliasing', '-std=gnu99', '-DLINUX',
          '-I' + os.path.join(here, 'include'), '-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        cf.append('-I' + os.path.join(inc, d))
    cf += ['-Wno-everything', '-Wint-to-pointer-cast', '-Wpointer-to-int-cast',
           '-Wvoid-pointer-to-int-cast', '-Wint-to-void-pointer-cast',
           '-ferror-limit=0']
    return cf


def diagnose(path, cf):
    r = subprocess.run([CC] + cf + ['-c', '-o', '/dev/null', path],
                       capture_output=True, text=True)
    out = []
    for line in r.stderr.splitlines():
        m = DIAG.match(line.strip())
        if m and os.path.abspath(m.group('file')) == os.path.abspath(path):
            out.append((int(m.group('line')), int(m.group('col')),
                        m.group('kind'), m.group('msg')))
    return out, r.returncode


def widen_file(path, cf, rounds=6):
    """Rewrite the narrow casts clang names. Returns (edits, leftover diags)."""
    total = 0
    for _ in range(rounds):
        diags, rc = diagnose(path, cf)
        if rc != 0:
            return total, [('DID NOT COMPILE', 0, 'error', '')]
        want = [d for d in diags
                if d[2] in ('pointer-to-int-cast', 'void-pointer-to-int-cast')]
        if not want:
            return total, diags
        lines = open(path).read().split('\n')
        # right-to-left within a line, so earlier columns stay valid
        by_line = {}
        for ln, col, _k, _m in want:
            by_line.setdefault(ln, set()).add(col)
        edits = 0
        for ln, cols in by_line.items():
            if ln - 1 >= len(lines):
                continue
            s = lines[ln - 1]
            for col in sorted(cols, reverse=True):
                m = CAST_AT.match(s, col - 1)
                if not m:
                    continue                 # not a cast here; leave it alone
                ty = re.sub(r'\s+', ' ', m.group('ty')).strip()
                new = WIDEN.get(ty)
                if not new:
                    continue                 # a spelling this does not know
                s = s[:m.start()] + '(' + new + ')' + s[m.end():]
                edits += 1
            lines[ln - 1] = s
        if not edits:
            return total, diags              # no progress; stop rather than spin
        total += edits
        open(path, 'w').write('\n'.join(lines))
    return total, diagnose(path, cf)[0]


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    if not os.path.exists(CC):
        print(f'fix_ptrwidth: no NDK at {NDK} — set KD_NDK.', file=sys.stderr)
        return 2
    cf = cflags(root)
    tot, left, touched, unknown = 0, 0, 0, {}
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c'):
            continue
        base = fn[:-2]
        if not os.path.exists(os.path.join(build, base + '.o')):
            continue                          # only what is in the build
        n, rest = widen_file(os.path.join(srcdir, fn), cf)
        tot += n
        touched += 1 if n else 0
        for _ln, _c, kind, msg in rest:
            left += 1
            m = re.search(r"smaller integer type '([^']*)'", msg)
            if m:
                unknown.setdefault(m.group(1), 0)
                unknown[m.group(1)] += 1
            else:
                unknown.setdefault(kind, 0)
                unknown[kind] += 1
    print(f'  {tot} narrow pointer cast(s) widened in {touched} object(s)')
    if left:
        print(f'  {left} diagnostic(s) remain, by cause:')
        for k, v in sorted(unknown.items(), key=lambda x: -x[1]):
            print(f'      {v:6}  {k}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
