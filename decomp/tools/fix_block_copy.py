#!/usr/bin/env python3
"""fix_block_copy.py — a WHOLE-STRUCT COPY that Ghidra rendered as a word loop.

    fix_block_copy.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT. `rep movsd` is how a 32-bit compiler copies a struct, and Ghidra
renders it as a loop that walks ONE FIELD AT A TIME and counts in WORDS:

    pMVar18 = &dummyModel;
    pMVar16 = p->model1;
    for (iVar12 = 0x1e; iVar12 != 0; iVar12 = iVar12 + -1) {
      pMVar18->frame = pMVar16->frame;
      pMVar16 = (McdModelID)&pMVar16->prev;      <- steps offsetof(McdModel, prev)
      pMVar18 = (McdModel *)&pMVar18->prev;
    }

That is `dummyModel = *p->model1;`. It is correct on i386 for one reason only:
`0x1e` is 30 and `sizeof(McdModel) / offsetof(McdModel, prev)` is 120 / 4 is
also 30. At LP64 the step becomes 8 while the literal stays 30, so the loop
copies 240 bytes into a 208-byte struct — 32 bytes past the end of a caller's
stack object, every time an aggregate is tested.

★ THIS IS THE DEFECT THAT CRASHED UT2004 AT 64-BIT, and it is the first one
found by RUNNING the game rather than by reading. The x86-64 Linux vehicle
(`-DUT_ALLOW_64BIT=ON`) died with SIGSEGV in `__strcmp_avx2` under
`MeXMLElementProcess`, called from `Handle_GeometryPrimitive_1_0` — a `strcmp`
against a NULL `name`, because the handler table it was searching had been built
by one of these loops and never contained the `MeXMLActionEnd` sentinel it stops
on. The engine's own SA_RESETHAND handler on Android had been swallowing exactly
this. Measured, per site, before and after:

    site                        n   step32 step64  sizeof32 sizeof64  n*step64
    McdAggregate McdModel      30      4      8       120      208      240  overrun
    McdAggregate McdGeomInst   13      4      8        52       80      104  overrun
    MdtWorld     KeaDebugReq   11      4      8        44       80       88  overrun
    keaIntegrate KeaParameters 19      4      4        76      128       76  SHORT
    MeXML*       MeXMLHandler   8      4      8        32       48       64  overrun
    MdtContact   BclContactPar 18      4      4        72       72       72  CORRECT

⚠ THE LAST ROW IS WHY THIS PASS MEASURES INSTEAD OF MATCHING. `MdtBclContactParams`
holds no pointers, so its size and its step are the same at both widths and the
loop is already right. It matches the pattern perfectly and must be left alone.

TWO SHAPES, AND THE SECOND IS NOT A COPY AT ALL.

  (1) STRUCT COPY — both ends are real memory. The whole loop is one struct
      assignment, which is correct at every width by construction.

  (2) RODATA TEMPLATE — the source is `&PTR_s_RADIUS_...`, i.e.
      `kd_relsec_rodata[]`, which is `void *const []`: ONE SLOT PER ORIGINAL
      4-BYTE WORD. A struct assignment is impossible there, because at LP64 the
      source has EIGHT slots per handler while the destination has six words.
      The only correct reading is field by field: word j of element i is
      `slot[i * W + j]`, where W is the i386 size in words. Requires every field
      of the destination struct to be exactly 4 bytes at i386 — which is the
      same fact that makes the word loop legal there in the first place — and
      the pass verifies that from the type database rather than assuming it.

★ WHY THE REPAIR IS GUARDED, AND WHY THAT IS THE STRONGER ANSWER HERE.
Every other pass in this chain re-spells the arithmetic so one expression folds
correctly at both widths. That was tried first and MEASURED: the per-field
expansion compiles to a DIFFERENT i386 object (22800 -> 22824 bytes), so it
cannot pass the acceptance gate, and no single spelling can — the two widths
genuinely need different code, because at LP64 the source and destination have
different numbers of words per element. So the i386 text is kept VERBATIM under
`#if __SIZEOF_POINTER__ == 4` and the corrected body goes in the `#else`.

That is not a concession, it is a proof: the preprocessor deletes the new branch
on every 4-byte-pointer target, so

  * the shipped 32-bit Linux object is byte-identical BY CONSTRUCTION, and
  * so is wasm32 — `__SIZEOF_POINTER__` is 4 there too, so THE WEB ARTIFACT
    CANNOT HAVE CHANGED. No re-stamp argument is needed; there is nothing for
    the browser to notice.

Byte-identity is still checked per site rather than trusted, because a guard put
in the wrong place is exactly the kind of edit that reads fine and moves a brace.

⚠ RUN IT ON A COPY: it edits in place.
"""
import json
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

HERE = kd_paths.MD
WORK = '/tmp/kd_blockcopy'

CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types',
          '-DLINUX']

HEAD = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
        '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))
_SZ = re.compile(r'char \(\*\)\[(\d+)\]')

# for (V = LITERAL; V != 0; V = V - 1) {
LOOP_HEAD = re.compile(
    r'(?m)^(?P<ind>[ \t]*)for \((?P<v>\w+) = (?P<count>0x[0-9a-fA-F]+|\d+); '
    r'(?P=v) != 0; (?P=v) = (?P=v) (?:- 1|\+ -1)\) \{')

# P = (T *)&P->F;   or   P = (Tptr)&P->F;   — the cast is not always a `T *`,
# MdtContact writes `p = (MdtContactParamsID)&p->model1;` through a typedef.
STRUCT_ADV = re.compile(
    r'^\s*(?P<p>\w+) = \((?P<cast>[\w ]+?)\s*(?P<star>\*?)\)&(?P=p)->(?P<f>\w+);\s*$')
UNIT_ADV = re.compile(r'^\s*(?P<p>\w+) = (?P=p) \+ 1;\s*$')
# the payload: `P->F = ...;` or `*P = ...;`
PAYLOAD = re.compile(r'^\s*(?:\(?\*(?P<sp>\w+)\)?|(?P<dp>\w+)->(?P<df>\w+))'
                     r'(?:\s*\.\s*\w+)*\s*=\s*(?P<rhs>[^;]+);\s*$')
# a rodata template base: `&PTR_something`
TEMPLATE_SRC = re.compile(r'^&\s*(?:PTR_\w+|kd_relsec_\w+\[[^\]]*\])$')


def measure(expr, inc, cache, bits='-m32'):
    """A compile-time constant read out of the compiler's own type printer."""
    key = (expr, bits)
    if key in cache:
        return cache[key]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p.c')
    open(src, 'w').write(HEAD + 'char kd_probe[%s];\nint kd_force = &kd_probe;\n' % expr)
    r = subprocess.run(['gcc', bits, '-DLINUX'] + includes(inc)
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    m = _SZ.search(r.stderr)
    cache[key] = int(m.group(1)) if m else None
    return cache[key]


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


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


def brace_span(text, open_pos):
    """(start, end) of the `{ ... }` whose `{` is at open_pos."""
    depth, i = 0, open_pos
    while i < len(text):
        if text[i] == '{':
            depth += 1
        elif text[i] == '}':
            depth -= 1
            if depth == 0:
                return open_pos, i + 1
        i += 1
    return None


def initialiser(text, before, var):
    """The last `var = <expr>;` before the loop. That is what the cursor points
    at, and it is the only thing that names the object being copied."""
    pat = re.compile(r'(?m)^[ \t]*%s = (?P<e>[^;]+);\s*$' % re.escape(var))
    last = None
    for m in pat.finditer(text, 0, before):
        last = m
    return last.group('e').strip() if last else None


def field_words(fields, size32):
    """The struct's fields in i386 word order, or None if they are not all
    exactly 4 bytes. That uniformity is what makes a word loop legal at i386,
    so its absence is a decline, not an assumption to paper over."""
    want = list(range(0, size32, 4))
    have = sorted(int(k) for k in fields)
    if have != want:
        return None
    return [fields[str(o)] for o in want]


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')
    cache = {}

    # ---- THE SELF-CHECK. A probe that cannot compile returns None for every
    # measurement, every site declines, and "0 repaired" reads exactly like
    # "there were none to repair".
    probe = measure('sizeof(McdModel)', inc, cache)
    if probe != 120:
        sys.exit('fix_block_copy: SELF-CHECK FAILED — sizeof(McdModel) measured '
                 '%r, want 120. Nothing below is measuring anything.' % probe)

    tf = os.path.join(HERE, 'include', 'kd_types_fields.json')
    typedb = json.load(open(tf)) if os.path.exists(tf) else {}

    fixed = declined = already = 0
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        edits = []
        for hm in LOOP_HEAD.finditer(text):
            span = brace_span(text, hm.end() - 1)
            if not span:
                continue
            whole = text[hm.start():span[1]]
            body = text[span[0] + 1:span[1] - 1]
            if '{' in body:
                continue
            stmts = [s for s in body.split('\n') if s.strip()]
            advs, pay = [], None
            bad = False
            for s in stmts:
                m = STRUCT_ADV.match(s)
                if m:
                    advs.append(('struct', m))
                    continue
                m = UNIT_ADV.match(s)
                if m:
                    advs.append(('unit', m))
                    continue
                m = PAYLOAD.match(s)
                if m and pay is None:
                    pay = m
                    continue
                bad = True
                break
            if bad or pay is None or not advs:
                continue
            sadv = [a for a in advs if a[0] == 'struct']
            if not sadv:
                continue
            # ---- the struct being copied, named by the cursor's own cast
            m = sadv[0][1]
            cast, star, f2 = m.group('cast').strip(), m.group('star'), m.group('f')
            ty = cast if star else None            # `(T *)` gives T directly
            base = '%s *' % cast if star else cast  # a pointer spelling either way
            szx = 'sizeof(*(%s)0)' % base
            offx = '((char *)&((%s)0)->%s - (char *)0)' % (base, f2)
            n = int(hm.group('count'), 0)
            s32, s64 = (measure(offx, inc, cache, b) for b in ('-m32', '-m64'))
            z32, z64 = (measure(szx, inc, cache, b) for b in ('-m32', '-m64'))
            if not all((s32, s64, z32, z64)):
                continue
            # ---- THE FRAME TEST: the literal must be a whole number of structs
            if s32 * n % z32:
                declined += 1
                notes.append('%-26s DECLINED: %d words * %d is not a whole '
                             'sizeof(%s)=%d' % (fn, n, s32, cast, z32))
                continue
            k = s32 * n // z32
            # ---- already correct? Only when the layout does not move at all.
            if s32 == s64 and z32 == z64:
                already += 1
                notes.append('%-26s %-24s already correct (%d==%d, %d==%d)'
                             % (fn, cast, s32, s64, z32, z64))
                continue
            # ---- which cursor is written to
            dst = pay.group('sp') or pay.group('dp')
            others = [a[1].group('p') for a in advs if a[1].group('p') != dst]
            src = others[0] if others else None
            if src is None:
                declined += 1
                notes.append('%-26s DECLINED: no source cursor' % fn)
                continue
            dinit = initialiser(text, hm.start(), dst)
            sinit = initialiser(text, hm.start(), src)
            if not dinit or not sinit:
                declined += 1
                notes.append('%-26s DECLINED: cannot find initialiser for %s/%s'
                             % (fn, dst, src))
                continue
            ind = hm.group('ind')
            if TEMPLATE_SRC.match(sinit):
                # ---- SHAPE 2: a word-indexed rodata template
                if not ty:
                    ty = cast
                flds = typedb.get(ty) or typedb.get('_' + ty) or \
                    typedb.get(ty.lstrip('_'))
                words = field_words(flds, z32) if flds else None
                if not words:
                    declined += 1
                    notes.append('%-26s DECLINED: %s fields are not a uniform '
                                 '4-byte word layout' % (fn, cast))
                    continue
                w = z32 // 4
                lines = ['%s  { int kd_e; for (kd_e = 0; kd_e < %d; kd_e++) {'
                         % (ind, k)]
                for j, fname in enumerate(words):
                    lines.append(
                        '%s    ((%s)(%s))[kd_e].%s = (__typeof__(((%s)(%s))[kd_e].%s))'
                        '(kd_uptr)((void *const *)(%s))[kd_e * %d + %d];'
                        % (ind, base, dinit, fname, base, dinit, fname, sinit, w, j))
                lines.append('%s  } }' % ind)
                new_body = '\n'.join(lines)
                what = '%s[%d] from a word-indexed rodata template' % (cast, k)
            elif k == 1:
                # ---- SHAPE 1: one whole struct, so it is a struct assignment
                new_body = ('%s  *(%s)(%s) = *(%s)(%s);'
                            % (ind, base, dinit, base, sinit))
                what = '%s struct assignment' % cast
            else:
                declined += 1
                notes.append('%-26s DECLINED: %d structs, and the source is not '
                             'a template' % (fn, k))
                continue
            rep = ('#if __SIZEOF_POINTER__ == 4\n%s\n#else\n%s\n#endif'
                   % (whole, new_body))
            edits.append((hm.start(), span[1], rep,
                          '%-26s %-24s n=%d step %d->%d sizeof %d->%d : %s'
                          % (fn, cast, n, s32, s64, z32, z64, what)))

        if not edits:
            continue
        # ---- apply back-to-front so earlier offsets stay valid
        out = text
        kept = []
        for start, end, rep, note in sorted(edits, reverse=True):
            cand = out[:start] + rep + out[end:]
            if compiles_identically(fn, cand, build, inc):
                out = cand
                kept.append(note)
            else:
                declined += 1
                notes.append('%-26s DECLINED: repair is NOT byte-identical at '
                             'i386' % fn)
        if out != text:
            open(path, 'w').write(out)
            fixed += len(kept)
            notes.extend(kept)

    for n in notes:
        print('  ' + n)
    print('fix_block_copy: %d repaired, %d already correct, %d declined'
          % (fixed, already, declined))


if __name__ == '__main__':
    main()
