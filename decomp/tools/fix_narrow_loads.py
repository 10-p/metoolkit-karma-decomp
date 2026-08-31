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

★ RULE C, AND THE DIAGNOSTIC IS STRUCTURALLY BLIND TO IT. Rules A and B both
fire on `-Wint-to-pointer-cast`, so they need the loaded value to be cast to a
POINTER. When `fix_narrow_pointers` has already widened the destination local to
`kd_iptr`, the assignment is integer-to-integer and **clang says nothing at
all** — the earlier repair hides the remaining one:

    kd_iptr iVar8;                                     /* already widened */
    iVar8 = *(int *)((kd_iptr)pvVar6 + KD_OFFSET(McdAggregate, elementTable));

The address is right and the local is wide; the LOAD still takes four bytes of
an eight-byte pointer. This is frame (5) of `LP64-RUNS-THE-GAME`:
`McdAggregateUpdateAABB` walking the element table off a half-pointer.

So rule C is MEASURED rather than diagnosed. It fires only when the address is
spelled as an offsetof naming a real field, the destination is declared
pointer-width, and `sizeof(T::F)` is **4 at i386 and 8 at LP64** — the field is
provably a pointer that grew. Two sites in the corpus qualify; the other 32
narrow loads into widened locals do not name a field, so they are not measurable
this way and are left alone rather than guessed at. Widening a load of a genuine
`int` field would read four bytes past it, which is why the measurement is the
gate and not the shape.
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


# RULE C. `V = *(NARROW *)((kd_iptr)X + KD_OFFSET(T, F));` where V is declared
# pointer-width. The diagnostic cannot see this one — see RULE C in the header.
NAMED_LOAD = re.compile(
    r'(?P<v>\b\w+)\s*=\s*\*\(\s*(?P<ty>int|uint|undefined4|MeU32|MeI32)\s*\*\)'
    r'\(\((?:kd_iptr|kd_uptr)\)\w+ \+ '
    r'\(\(int\)\(\(char \*\)&\(\((?:struct )?(?P<T>\w+) \*\)0\)->(?P<F>[\w.\[\]]+)'
    r' - \(char \*\)0\)\)\)')
WIDE_DECL = r'(?m)^\s*kd_[iu]ptr\s+%s\s*;'
_FSZ = re.compile(r'char \(\*\)\[(\d+)\]')
_FCACHE = {}


def field_size(T, F, bits, root):
    """sizeof(T::F) under one data model, read out of the compiler's own type
    printer. A field that cannot be measured returns None and the site declines;
    nothing here invents a number."""
    key = (T, F, bits)
    if key in _FCACHE:
        return _FCACHE[key]
    inc = os.path.join(root, 'include')
    os.makedirs('/tmp/kd_narrowload', exist_ok=True)
    src = '/tmp/kd_narrowload/p.c'
    head = ('#include "%s/kd_compat.h"\n#include "%s/kd_karma.h"\n'
            '#include "%s/kd_types.h"\n' % ((kd_paths.MD_INC,) * 3))
    open(src, 'w').write(head + 'char kd_probe[sizeof(((%s *)0)->%s)];\n'
                                'int kd_force = &kd_probe;\n' % (T, F))
    r = subprocess.run(['gcc', bits, '-DLINUX'] + kd_paths.includes(inc)
                       + ['-I' + kd_paths.MD_INC, '-c', '-o', os.devnull, src],
                       capture_output=True, text=True)
    m = _FSZ.search(r.stderr)
    _FCACHE[key] = int(m.group(1)) if m else None
    return _FCACHE[key]


def widen_named_field_loads(path, root):
    """Rule C. Returns (widened, declined) — declined sites are reported.

    ★ THE DESTINATION IS WIDENED TOO WHEN IT IS STILL AN `int`. Ghidra reuses one
    local for several unrelated things — in `MdtBcl3`'s solver `iVar15` is a row
    index in one statement and an `MdtBody *` in the next — so
    `fix_narrow_pointers` will not widen it, and rule C's "destination is already
    pointer-width" test skips it. But `kd_iptr` IS `int` at 32-bit pointer width,
    so promoting the DECLARATION is a no-op on every shipping target by
    construction, and at LP64 it is right for both uses: the index values fit and
    the pointer stops being truncated. It is still compiled and compared rather
    than argued."""
    text = open(path, errors='ignore').read()
    out, done, dec = text, 0, []
    promote = set()
    for m in NAMED_LOAD.finditer(text):
        v, ty, T, F = m.group('v'), m.group('ty'), m.group('T'), m.group('F')
        if not re.search(WIDE_DECL % re.escape(v), text):
            if re.search(r'(?m)^\s*(?:int|uint)\s+%s\s*;' % re.escape(v), text):
                promote.add(v)             # widen the declaration as well
            else:
                continue                   # destination is not pointer-width
        a = field_size(T, F, '-m32', root)
        b = field_size(T, F, '-m64', root)
        if a is None or b is None:
            dec.append('%s::%s cannot be measured' % (T, F))
            continue
        if not (a == 4 and b == 8):
            # ⚠ A GENUINE int FIELD. Widening this load would read four bytes
            # PAST it. The measurement is the gate, not the shape.
            dec.append('%s::%s is %d/%d, not a pointer that grew' % (T, F, a, b))
            continue
        new = WIDEN[ty]
        old_txt = m.group(0)
        out = out.replace(old_txt, old_txt.replace('*(%s *)' % ty,
                                                   '*(%s *)' % new, 1))
        done += 1
    # ⚠ SIGNEDNESS SURVIVES THE PROMOTION. `uint -> kd_iptr` is not a widening,
    # it is a sign change, and it cost MdtBcl its byte-identity on three
    # declarations that had nothing else wrong with them.
    for v in sorted(promote):
        out = re.sub(r'(?m)^(\s*)(int|uint)(\s+%s\s*;)' % re.escape(v),
                     lambda mm: '%s%s%s' % (mm.group(1),
                                            'kd_iptr' if mm.group(2) == 'int'
                                            else 'kd_uptr', mm.group(3)), out)
    if out != text:
        open(path, 'w').write(out)
    return done, dec


# RULE D. `V = (NARROW *)((kd_iptr)&EXPR.FIELD + ...)` where FIELD is a POINTER
# that grows. The local is a word pointer, so `*V` reads four bytes of it.
ADDR_OF_FIELD = re.compile(
    r'(?P<v>\w+) = \((?P<ty>undefined4|int|uint) \*\)\(\(kd_iptr\)&'
    r'(?P<expr>[A-Za-z_][\w\[\]\.>-]*?)\s*\+')


def resolve_member(expr, text, root):
    """Follow `context->pools[3].contacts` to the member it names.

    ⚠ A MEMBER NAME IS NOT A TYPE. Four structs in this corpus declare a pointer
    called `contacts` — `McdBatchEntry`, `McdBatchContactPool`,
    `_McdIntersectResult` and `MstBridge` — so matching on the name alone is a
    coin toss. The expression says which: `context` is declared
    `McdBatchContext *`, its `pools` is an `McdBatchContactPool *`, and `[3]` of
    that is where `.contacts` lives. Each hop is read from the ORACLE, and a hop
    that cannot be resolved declines the site.

    Returns (owning tag, member name) or None."""
    import fix_literal_offsets as flo
    inc = os.path.join(root, 'include')
    bodies = flo.struct_bodies(inc)
    parts = [x for x in re.split(r'->|\.', expr) if x]
    base = re.sub(r'\[[^\]]*\]', '', parts[0])
    m = re.search(r'(?<![\w])([A-Za-z_]\w*)\s*\*\s*%s\b' % re.escape(base), text)
    if not m:
        return None
    tag = m.group(1)
    for hop in parts[1:-1]:
        hop = re.sub(r'\[[^\]]*\]', '', hop)
        body = bodies.get(tag)
        if not body:
            return None
        mm = re.search(r'([A-Za-z_]\w*)\s*\*?\s*%s\s*[;,\[]' % re.escape(hop),
                       re.sub(r'/\*.*?\*/', ' ', body, flags=re.S))
        if not mm:
            return None
        tag = mm.group(1)
    field = re.sub(r'\[[^\]]*\]', '', parts[-1])
    if tag not in bodies:
        return None
    return tag, field


def widen_field_address_pointers(path, root):
    """Rule D. Returns (widened, declined).

    `McdBatchContextDestroy` walks the contact pools and frees the last of every
    four through `(void *)*puVar1`, where `puVar1` is an `undefined4 *` pointing
    at a `contacts` POINTER. The other three in the same loop use
    `*(void **)(...)` and are fine; this one reads four bytes of an eight-byte
    pointer and hands the half to `free()`. It is the BR-Anubis shutdown SIGSEGV,
    and it is on every level teardown — the map sweep only surfaced it where the
    timing let the teardown finish.

    Rule A cannot see it: `(void *)*puVar1` dereferences a VARIABLE, not a
    `*(T *)` cast, so the `LOAD` pattern does not match and the site is counted
    as "a field or a plain variable". The repair is to type the POINTER instead
    of the load — `void **` — which is the same four-byte access at i386."""
    text = open(path, errors='ignore').read()
    out, done, dec = text, 0, []
    for m in ADDR_OF_FIELD.finditer(text):
        v, ty = m.group('v'), m.group('ty')
        if not re.search(r'(?m)^\s*%s\s*\*\s*%s\s*;' % (ty, re.escape(v)), out):
            continue
        r = resolve_member(m.group('expr'), text, root)
        if not r:
            dec.append('cannot resolve %s to a member' % m.group('expr'))
            continue
        T, f = r
        a = field_size(T, f, '-m32', root)
        b = field_size(T, f, '-m64', root)
        if not (a == 4 and b == 8):
            dec.append('%s::%s is %s/%s, not a pointer that grew' % (T, f, a, b))
            continue
        out = re.sub(r'(?m)^(\s*)%s(\s*)\*(\s*%s\s*;)' % (ty, re.escape(v)),
                     r'\1void\2**\3', out)
        out = out.replace('%s = (%s *)((kd_iptr)&' % (v, ty),
                          '%s = (void **)((kd_iptr)&' % v)
        done += 1
    if out != text:
        open(path, 'w').write(out)
    return done, dec


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    if not CC or not os.path.exists(CC):
        print(f'fix_narrow_loads: no NDK at {NDK} — set KD_NDK.', file=sys.stderr)
        return 2
    cf = cflags(root)
    tot = touched = fld = fbits = other = 0
    named, named_dec = 0, []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c'):
            continue
        base = fn[:-2]
        if not os.path.exists(os.path.join(build, base + '.o')):
            continue                          # only what is in the build
        n, a, b, c = fix_file(os.path.join(srcdir, fn), cf)
        tot += n; fld += a; fbits += b; other += c
        touched += 1 if n else 0
        # ---- RULE C, after the diagnostic loop: the sites it cannot see
        # because fix_narrow_pointers already made the destination an integer.
        k, d = widen_named_field_loads(os.path.join(srcdir, fn), root)
        named += k
        named_dec += ['%-24s %s' % (fn, x) for x in d]
        # ---- RULE D: a word pointer AT a growing pointer field
        k2, d2 = widen_field_address_pointers(os.path.join(srcdir, fn), root)
        named += k2
        named_dec += ['%-24s %s' % (fn, x) for x in d2]
    print(f'  {tot} narrow pointer LOAD(s) widened in {touched} object(s)')
    print(f'  {named} named-field LOAD(s) widened by measurement (rule C)')
    for x in named_dec:
        print(f'    declined: {x}')
    print(f'  declined: {fld} narrow struct field(s) — a LAYOUT question, see kd_types.h')
    print(f'            {fbits} KD_FBITS float bit-pun(s) — not pointers')
    print(f'            {other} unrecognised shape(s) — reported, not guessed')
    return 0


if __name__ == '__main__':
    sys.exit(main())
