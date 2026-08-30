#!/usr/bin/env python3
"""fix_narrow_loads.py — widen the LOAD, not just the address arithmetic.

    fix_narrow_loads.py <kd_out/allobj> <kd_build> [metoolkit-root]

WHAT THIS IS AND WHY fix_ptrwidth.py IS NOT ENOUGH.

`fix_ptrwidth.py` widens the CASTS clang identifies as narrowing a pointer —
`(int)&x` becomes `(kd_iptr)&x` — and it closed 3,864 of those. What it leaves
behind is the other half of the same round trip, and its own header says so:
`-Wint-to-pointer-cast`, `(T *)(narrow)`. It predicted those would "disappear on
their own once the inner cast is width-correct", and that "what remains after
the loop settles is a genuinely integer-valued address (Ghidra's
`(code *)0x10074` and the like)".

⚠ THAT PREDICTION IS WRONG, AND IT WAS MEASURED WRONG ON 2026-08-30. Of the 151
`-Wint-to-pointer-cast` diagnostics remaining at aarch64, **not one is a literal
address**. They are dominated by:

    (float **)(*(int *)((kd_iptr)pvVar22 + 0x34))     <- 4-byte LOAD of a pointer
    (char *)**(undefined4 **)elem1                    <- same, through a chain
    (Link *)pCVar2->mCullingID                        <- a 32-bit FIELD

The ADDRESS was widened; the LOAD WIDTH was not. `*(int *)p` reads four bytes
wherever `p` points, so at LP64 it returns the low half of a stored pointer and
the high half is dropped. Nothing truncates visibly, nothing warns except this
diagnostic, and the value that comes back is a real address on a machine whose
heap happens to live below 4 GB — which is why it can look fine.

THIS IS NOT A THEORETICAL DEFECT. `scene_ragdoll`, plain -m64, no sanitizer:
eight runs of the same binary gave four different answers, ASLR off made it
deterministic AND wrong, and the same sources at -m32 were identical 11 times of
11. MALLOC_PERTURB_ across five values changed nothing, so it is not
uninitialised memory — it is the ADDRESS VALUE, which is exactly this.

WHAT IT REWRITES, and it declines everything else:

    A  *(NARROW *)expr          ->  *(kd_iptr *)expr     when the result is cast
    B  **(NARROW **)expr        ->  **(kd_uptr **)expr   to a POINTER type

WHY IT IS A NO-OP ON THE TARGETS THAT WORK. `kd_iptr` is not merely the same
SIZE as `int` at 32-bit pointer width — it is the same TYPE. Every rewritten
load means precisely what it meant before on i386, wasm32 and armv7, and the
acceptance test (every already-built .o byte-identical at -m32) passes by
construction. Verify it anyway; that is the rule.

WHAT IT DOES NOT TOUCH. A narrow STRUCT FIELD (`.mRefCtAndID`, `.mCullingID`)
is a layout question, not a load-width one: the field is four bytes in the type
and widening the access would read past it. Those are reported, not patched —
they need the field itself widened, which is `kd_types.h`'s business and the
amd64 oracle's evidence. `KD_FBITS(...)` sites are float bit-punning and are not
pointers at all; they are counted separately so the remainder is honest.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

NDK = kd_paths.NDK_BIN
CC = os.path.join(NDK, 'aarch64-linux-android21-clang') if NDK else ''

# The narrow integer types the corpus loads pointers through, and what each
# becomes. Signedness is preserved: a comparison downstream can depend on it.
WIDEN = {
    'int': 'kd_iptr', 'MeI32': 'kd_iptr',
    'uint': 'kd_uptr', 'unsigned int': 'kd_uptr', 'MeU32': 'kd_uptr',
    'undefined4': 'kd_uptr',
}

DIAG = re.compile(r'^(?P<file>[^:]+):(?P<line>\d+):(?P<col>\d+): warning: '
                  r"cast to '(?P<to>[^']+)'.*from smaller integer type "
                  r"'(?P<from>[^']+)'")

# `(Type *)` or `(Type **)` at a known column — the OUTER cast clang points at.
OUTER = re.compile(r'\(\s*[A-Za-z_][\w ]*\*+\s*\)\s*')
# `*(int *)` / `**(undefined4 **)` after it — possibly wrapped in parentheses,
# which is the common spelling: `(float **)(*(int *)((kd_iptr)p + 0x34))`.
LOAD = re.compile(r'^(?P<lead>\(*)\s*(?P<stars>\*{1,2})\s*\(\s*'
                  r'(?P<ty>[A-Za-z_][\w ]*?)\s*(?P<p>\*{1,2})\s*\)')


def cflags(root):
    inc = os.path.join(root, 'include')
    cf = ['-O2', '-fno-strict-aliasing', '-std=gnu99', '-DLINUX'] \
        + kd_paths.includes(inc) + ['-I' + kd_paths.MD_INC]
    cf += ['-Wno-everything', '-Wint-to-pointer-cast', '-ferror-limit=0']
    return cf


def diagnose(path, cf):
    r = subprocess.run([CC] + cf + ['-c', '-o', os.devnull, path],
                       capture_output=True, text=True)
    out = []
    for line in r.stderr.splitlines():
        m = DIAG.match(line.strip())
        if m and os.path.abspath(m.group('file')) == os.path.abspath(path):
            out.append((int(m.group('line')), int(m.group('col'))))
    return out, r.returncode


def fix_file(path, cf, rounds=6):
    """Returns (edits, declined_field, declined_fbits, declined_other)."""
    total = fld = fbits = other = 0
    for _ in range(rounds):
        diags, rc = diagnose(path, cf)
        if rc != 0:
            return total, fld, fbits, other
        if not diags:
            break
        lines = open(path).read().split('\n')
        by_line = {}
        for ln, col in diags:
            by_line.setdefault(ln, set()).add(col)
        edits = 0
        for ln, cols in by_line.items():
            if ln - 1 >= len(lines):
                continue
            s = lines[ln - 1]
            for col in sorted(cols, reverse=True):
                m = OUTER.match(s, col - 1)
                if not m:
                    other += 1
                    continue
                rest = s[m.end():]
                if rest.startswith('KD_FBITS'):
                    fbits += 1                 # float bit-punning, not a pointer
                    continue
                lm = LOAD.match(rest)
                if not lm:
                    fld += 1                   # a field or a plain variable
                    continue
                new = WIDEN.get(re.sub(r'\s+', ' ', lm.group('ty')).strip())
                if not new:
                    other += 1
                    continue
                start = m.end() + lm.start('ty')
                end = m.end() + lm.end('ty')
                s = s[:start] + new + s[end:]
                edits += 1
            lines[ln - 1] = s
        if not edits:
            break                              # no progress; stop rather than spin
        total += edits
        open(path, 'w').write('\n'.join(lines))
    return total, fld, fbits, other


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    if not CC or not os.path.exists(CC):
        print(f'fix_narrow_loads: no NDK at {NDK} — set KD_NDK.', file=sys.stderr)
        return 2
    cf = cflags(root)
    tot = touched = fld = fbits = other = 0
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c'):
            continue
        base = fn[:-2]
        if not os.path.exists(os.path.join(build, base + '.o')):
            continue                          # only what is in the build
        n, a, b, c = fix_file(os.path.join(srcdir, fn), cf)
        tot += n; fld += a; fbits += b; other += c
        touched += 1 if n else 0
    print(f'  {tot} narrow pointer LOAD(s) widened in {touched} object(s)')
    print(f'  declined: {fld} narrow struct field(s) — a LAYOUT question, see kd_types.h')
    print(f'            {fbits} KD_FBITS float bit-pun(s) — not pointers')
    print(f'            {other} unrecognised shape(s) — reported, not guessed')
    return 0


if __name__ == '__main__':
    sys.exit(main())
