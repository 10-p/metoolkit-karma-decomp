#!/usr/bin/env python3
"""fix_setter_typed_slot.py — a `void *` slot whose type only its SETTER knows.

    fix_setter_typed_slot.py <kd_lp64/allobj> <kd_build> [metoolkit-root]

THE DEFECT. `MdtContactGroup::generator` is declared `void *`, so the oracle
cannot type it and `accessor_scopes` — which reads the accessor's DECLARED
return type — gets `void *` and stops. Ghidra therefore addresses whatever is in
there by baked offsets, in two different spellings:

    int *piVar4;
    piVar4     = MdtContactGroupGetGenerator(pvVar3);
    otherModel = (McdModelID)*piVar4;                    /* model1 */
    if ((otherModel != pMVar1) && ((McdModelID)piVar4[1] != pMVar1)) {   /* model2 */

    *(undefined4 *)((kd_iptr)c->generator + 0x18) = 0;   /* responseData */

```
McdModelPair   i386  model1 0  model2 4  phase  8  request 12 … responseData 24
               LP64  model1 0  model2 8  phase 16  request 24 … responseData 48
```

★ At LP64 `piVar4[1]` is the HIGH HALF OF `model1`, not `model2`, and `+0x18` is
`request` rather than `responseData`. `McdModelSetBody` is called from
`KarmaSupport.cpp` and `KSkeletal.cpp`, so this is on the vehicle/ragdoll path.

THE TYPE IS DECLARED — IN THE SETTER, and that is the same evidence chain
`interaction_types` and `fix_callback_context` use:

    MdtContactGroupSetGenerator(MdtContactGroupID c, void *generator)
        c->generator = generator;
    …and the ONLY call in the corpus:
    MstUtils.c   createContactGroup(MdtWorldID w, McdModelPairID pair)
                 MdtContactGroupSetGenerator(pMVar5, pair);

`pair` is declared `McdModelPairID`, so everything read back out of that slot is
an `McdModelPair *`, inferred from nothing.

THE RULE, and every clause is measured:

  1. a setter `void XSetY(H h, void *v)` whose body is `h->FIELD = v;`, and a
     getter `void * XGetY(H h)` returning the same FIELD. Both come from the
     recovered sources, not from a name convention.
  2. EVERY call to the setter in the corpus resolves its value argument to the
     SAME struct tag. ⚠ One call that resolves to something else, or does not
     resolve at all, drops the slot — a type that is true of some callers is not
     a type.
  3. a site reads that slot, either through a local assigned from the getter
     (`VAR[k]`, `*VAR`) or straight off the member (`(kd_iptr)X->FIELD + K`).
  4. the byte the site addresses at i386 — `k * sizeof(*VAR)` or `K` — is the
     START of a top-level member of the tag. An offset landing mid-member means
     the shape was not what this rule assumes, and it declines.
  5. ★ THE MEMBER MUST ACTUALLY MOVE. `model1` is byte 0 at both widths, so
     `*piVar4` is already right and is left exactly as it is —
     `fix_member_base_walk`'s rule, for its reason: rewriting a correct site
     produces an expression that merely agrees at i386.
  6. the rewrite reproduces the i386 object BYTE FOR BYTE.

⚠⚠ CLAUSE 6 IS A BACKSTOP. i386 byte-identity cannot see a wrong TYPE — every
member at the same offset compiles identically — so the evidence is 1-5 plus the
ktrace measurement.

Run it on a COPY: it edits in place. AFTER `fix_callback_context`, BEFORE
`fix_ptrwidth`.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402
from fix_derived_fields import (BANNER, HEAD, compiles_identically,  # noqa: E402
                                includes)
from fix_narrow_pointers import declared_type, tag_of       # noqa: E402

WORK = '/tmp/kd_settyped'
_SZ = re.compile(r'char \(\*\)\[(\d+)\]')
_CACHE = {}

# `void kd_XSetY(H h, void *v) { ... h->FIELD = v; ... }`
SETTER = re.compile(
    r'(?m)^\w[\w \t\*]*\bkd_(?P<name>\w*Set\w+)\s*\((?P<params>[^)]*)\)\s*\n?\s*\{'
    r'(?P<body>(?:[^{}]|\{[^{}]*\})*)\}')
# `void * kd_XGetY(H h) { return h->FIELD; }`
GETTER = re.compile(
    r'(?m)^void\s*\*\s*kd_(?P<name>\w*Get\w+)\s*\((?P<params>[^)]*)\)\s*\n?\s*\{'
    r'(?P<body>(?:[^{}]|\{[^{}]*\})*)\}')


def probe(body, bits, inc):
    key = (body, bits)
    if key in _CACHE:
        return _CACHE[key]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p%d.c' % bits)
    open(src, 'w').write(HEAD + body)
    r = subprocess.run(['gcc', '-m%d' % bits, '-DLINUX'] + includes(inc)
                       + ['-Werror=int-conversion', '-c', '-o', os.devnull, src],
                       capture_output=True, text=True)
    m = _SZ.search(r.stderr)
    _CACHE[key] = (int(m.group(1)) - 1) if m else None
    return _CACHE[key]


def offset_of(tag, field, bits, inc):
    return probe('char kd_probe[((char *)&((%s *)0)->%s - (char *)0) + 1];\n'
                 'int kd_force = &kd_probe;\n' % (tag, field), bits, inc)


def members(tag, inc):
    """Top-level member names of `tag`, in declaration order."""
    import fix_literal_offsets as _flo
    bodies = _flo.struct_bodies(inc)
    body = bodies.get(tag) or bodies.get('_' + tag)
    if not body:
        return []
    out = []
    for line in body.split(';'):
        m = re.search(r'(?:\*\s*|\s)(\w+)\s*(?:\[[^\]]*\])?\s*$', line.strip())
        if m and not re.match(r'^\s*(?:struct|union|enum)\s*\{', line):
            out.append(m.group(1))
    return out


def field_width(tag, field, bits, inc):
    """sizeof of `((TAG *)0)->FIELD` at `bits` — the other half of clause 5."""
    return probe('char kd_probe[sizeof(((%s *)0)->%s) + 1];\n'
                 'int kd_force = &kd_probe;\n' % (tag, field), bits, inc)


def member_at(tag, off, bits, inc):
    for name in members(tag, inc):
        if offset_of(tag, name, bits, inc) == off:
            return name
    return None


def usable_tag(tag, inc):
    """The spelling of `tag` that can be written in a CAST.

    ⚠ `tag_of` answers with the struct TAG (`_McdModelPair`) and the corpus
    casts through the TYPEDEF (`McdModelPair`). Both name the same type and only
    one of them compiles in `(X *)p`, so the choice is made by asking the
    compiler rather than by stripping an underscore and hoping."""
    for cand in (tag.lstrip('_'), tag, 'struct %s' % tag):
        if probe('char kd_probe[sizeof(%s) + 1];\nint kd_force = &kd_probe;\n'
                 % cand, 32, inc):
            return cand
    return None


def blank_comments(text):
    """Comments replaced by spaces, LENGTH PRESERVED so offsets still line up.

    ⚠⚠ NOT COSMETIC. Every recovered function carries the banner

        /* ---- MdtContactGroupSetGenerator (exported as kd_…, asm label "…") ---- */

    and `NAME\\s*\\(...\\)` matches that as a CALL whose "argument" is
    `exported as kd_MdtContactGroupSetGenerator`. It does not resolve to a type,
    clause 2 then reads "one caller could not be resolved", and the slot is
    dropped — silently, and in the safe direction, which is why it looks like
    there was simply nothing to repair. The self-check is what caught it."""
    out, i, n = list(text), 0, len(text)
    while i < n - 1:
        if text[i] == '/' and text[i + 1] == '*':
            j = text.find('*/', i + 2)
            j = n if j < 0 else j + 2
            for k in range(i, j):
                if out[k] != '\n':
                    out[k] = ' '
            i = j
        else:
            i += 1
    return ''.join(out)


def region_bounds(text, pos):
    start, end = 0, len(text)
    for m in BANNER.finditer(text):
        if m.start() <= pos:
            start = m.start()
        elif m.start() > pos:
            end = m.start()
            break
    return start, end


def slot_types(srcdir, inc):
    """{(getter, field, tag)} — a `void *` member typed by its setter's callers."""
    setters, getters, texts = {}, {}, {}
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c'):
            continue
        t = open(os.path.join(srcdir, fn), errors='ignore').read()
        texts[fn] = t
        for m in SETTER.finditer(t):
            ps = [p.strip() for p in m.group('params').split(',')]
            v = re.fullmatch(r'void\s*\*\s*(\w+)', ps[-1]) if ps else None
            if not v:
                continue
            a = re.search(r'(\w+)\s*->\s*(\w+)\s*=\s*%s\s*;' % re.escape(v.group(1)),
                          m.group('body'))
            if a:
                setters[m.group('name')] = a.group(2)
        for m in GETTER.finditer(t):
            a = re.search(r'return\s+(\w+)\s*->\s*(\w+)\s*;', m.group('body'))
            if a:
                getters[m.group('name')] = a.group(2)

    out = {}
    for sname, field in setters.items():
        gname = sname.replace('Set', 'Get', 1)
        if getters.get(gname) != field:
            continue
        tags, ok = set(), True
        for fn, t in texts.items():
            code = blank_comments(t)
            for c in re.finditer(r'(?<![\w])%s\s*\(([^();]*)\)' % re.escape(sname), code):
                args = [a.strip() for a in c.group(1).split(',')]
                if len(args) < 2:
                    continue
                rs, re_ = region_bounds(t, c.start())
                ty = declared_type(t[rs:re_], args[-1])
                tag = tag_of(ty, inc) if ty else None
                if not tag:
                    ok = False              # (2) one unresolvable caller drops it
                else:
                    tags.add(tag)
        if ok and len(tags) == 1:
            t = usable_tag(next(iter(tags)), inc)
            if t:
                out[gname] = (field, t)
    return out


def compiles_at_m64(fn, text, inc):
    """Does this candidate COMPILE at LP64 at all?

    ⚠⚠ i386 BYTE-IDENTITY CANNOT ANSWER THIS, and a `#if __SIZEOF_POINTER__`
    guard makes it structurally blind: the i386 branch is the ORIGINAL text, so
    the gate passes by construction while the `#else` branch is whatever was
    proposed. `fix_typeid_dispatch` shipped an uncompilable LP64 branch past a
    `145 object(s), 0 byte difference(s)` report exactly that way."""
    d = os.path.join(WORK, 'm64')
    os.makedirs(d, exist_ok=True)
    src = os.path.join(d, fn)
    open(src, 'w').write(text)
    return subprocess.run(
        ['gcc', '-m64', '-O2', '-fno-strict-aliasing', '-std=gnu99', '-w',
         '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX',
         '-I' + os.path.join(kd_paths.MD, 'include')] + includes(inc)
        + ['-c', '-o', os.devnull, src], capture_output=True).returncode == 0


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')

    slots = slot_types(srcdir, inc)
    # ---- THE SELF-CHECK: "0 repaired" must not read the same as "the evidence
    # scan found nothing", which is how a pass like this fails silently.
    got = slots.get('MdtContactGroupGetGenerator')
    # ⚠ ASSERT ON THE MEASUREMENT, NOT THE SPELLING. `_McdModelPair` and
    # `McdModelPair` name the same type and both compile; pinning the self-check
    # to one of them makes it fail on a correct tree.
    if not got or got[0] != 'generator' or \
            offset_of(got[1], 'model2', 32, inc) != 4 or \
            offset_of(got[1], 'model2', 64, inc) != 8:
        sys.exit('fix_setter_typed_slot: SELF-CHECK FAILED — '
                 'MdtContactGroupGetGenerator typed as %r, and its model2 sits '
                 'at %r/%r; want the `generator` field of a struct whose model2 '
                 'is 4 at i386 and 8 at LP64. The setter evidence is not being '
                 'read.' % (got, got and offset_of(got[1], 'model2', 32, inc),
                            got and offset_of(got[1], 'model2', 64, inc)))

    by_field = {f: tag for (f, tag) in slots.values()}
    fixed = declined = 0
    notes, declines = [], []

    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        edits = []

        # (a) a local assigned from the getter, indexed as an array
        for g, (field, tag) in slots.items():
            for a in re.finditer(r'(?m)^\s*(?P<v>\w+)\s*=\s*(?:\([^()]*\)\s*)?'
                                 r'(?:kd_)?%s\s*\(' % re.escape(g), text):
                var = a.group('v')
                rs, re_ = region_bounds(text, a.start())
                ty = declared_type(text[rs:re_], var)
                esz = probe('char kd_probe[sizeof(*(%s)0) + 1];\nint kd_force = &kd_probe;\n'
                            % ty, 32, inc) if ty else None
                if not esz:
                    continue
                stop = text.find('\n%s = ' % var, a.end())
                stop = re_ if stop < 0 or stop > re_ else stop
                for s in re.finditer(r'(?<![\w>])(?:\*\s*%s(?![\w\[])|%s\s*\[\s*(\d+)\s*\])'
                                     % (re.escape(var), re.escape(var)),
                                     text[a.end():stop]):
                    k = int(s.group(1)) if s.group(1) else 0
                    off = k * esz
                    name = member_at(tag, off, 32, inc)
                    if not name:
                        declined += 1
                        declines.append('%-24s %s[%d]: i386 byte %d is not a member '
                                        'start of %s' % (fn, var, k, off, tag))
                        continue
                    # ⚠⚠ (5) IS ABOUT THE OFFSET **AND** THE WIDTH, AND IT USED
                    # TO BE ABOUT ONLY THE OFFSET. `McdModelPair::model1` is
                    # byte 0 at BOTH widths, so "nothing moves" was true and
                    # `*piVar4` was skipped and written up as correct — while
                    # `piVar4` is an `int *`, so the read is FOUR bytes and the
                    # member is an eight-byte pointer. MathEngine's own amd64
                    # build settles it: `mov (%rax),%rax`, eight bytes.
                    # ★ A field at a fixed offset can still need a wider load.
                    moved = offset_of(tag, name, 32, inc) != offset_of(tag, name, 64, inc)
                    w32 = field_width(tag, name, 32, inc)
                    w64 = field_width(tag, name, 64, inc)
                    widened = (w32 is not None and w64 is not None and w32 != w64)
                    if not moved and not widened:
                        continue                    # (5) neither the byte nor the width
                    st = a.end() + s.start()
                    edits.append((st, a.end() + s.end(),
                                  ['((%s *)%s)->%s' % (tag, var, name)],
                                  '%-24s %s[%d] -> ((%s *)%s)->%s   i386 %d, LP64 %d'
                                  % (fn, var, k, tag, var, name, off,
                                     offset_of(tag, name, 64, inc))))

        # (b) straight off the member: `(kd_iptr)X->FIELD + K`
        for field, tag in by_field.items():
            for s in re.finditer(r'\(\s*kd_iptr\s*\)\s*(?P<b>[\w>.\-]+?)->%s\s*\+\s*'
                                 r'(?P<off>0x[0-9a-f]+|\d+)' % re.escape(field), text):
                off = int(s.group('off'), 0)
                name = member_at(tag, off, 32, inc)
                if not name:
                    declined += 1
                    declines.append('%-24s %s->%s + %d is not a member start of %s'
                                    % (fn, s.group('b'), field, off, tag))
                    continue
                if offset_of(tag, name, 32, inc) == offset_of(tag, name, 64, inc):
                    continue
                rep = ('(kd_iptr)%s->%s + ((int)((char *)&((%s *)0)->%s - (char *)0))'
                       % (s.group('b'), field, tag, name))
                edits.append((s.start(), s.end(), [rep],
                              '%-24s %s->%s + %d -> offsetof(%s, %s)   LP64 %d'
                              % (fn, s.group('b'), field, off, tag, name,
                                 offset_of(tag, name, 64, inc))))

        n0 = fixed
        for start, end, reps, note in sorted(edits, key=lambda e: -e[0]):
            for rep in reps:
                cand = text[:start] + rep + text[end:]
                if compiles_identically(fn, cand, build, inc) \
                        and compiles_at_m64(fn, cand, inc):
                    text = cand
                    fixed += 1
                    notes.append(note)
                    break
            else:
                # ★ THE WIDTH GUARD IS A LEGITIMATE REPAIR WHEN A MEASUREMENT
                # SAYS NO SINGLE SPELLING EXISTS — `fix_block_copy`'s precedent,
                # for the same reason. Here the i386 original genuinely performs
                # a FOUR-byte load (`piVar4[1]`) where LP64 needs an EIGHT-byte
                # one, so no one expression can be both. Every in-place spelling
                # was tried and measured first; this is the fallback, not the
                # first idea.
                #
                # ⚠ It is stronger than an argument about the web build:
                # `__SIZEOF_POINTER__` is 4 on wasm32 too, so the shipped
                # artefact keeps the ORIGINAL text by construction and the guard
                # cannot change it.
                ls = text.rfind('\n', 0, start) + 1
                le = text.find('\n', end)
                if le < 0:
                    le = len(text)
                line = text[ls:le]
                for rep in reps:
                    fixed_line = text[ls:start] + rep + text[end:le]
                    guarded = ('#if __SIZEOF_POINTER__ == 4\n%s\n#else\n%s\n#endif'
                               % (line, fixed_line))
                    cand = text[:ls] + guarded + text[le:]
                    if compiles_identically(fn, cand, build, inc) \
                        and compiles_at_m64(fn, cand, inc):
                        text = cand
                        fixed += 1
                        notes.append(note + '   [#if __SIZEOF_POINTER__ guard: '
                                     'a 4-byte load at i386, 8 at LP64]')
                        break
                else:
                    declined += 1
                    declines.append('%s  — no spelling, guarded or not, '
                                    'reproduced the i386 object' % note)
        if fixed > n0:
            open(path, 'w').write(text)

    print('fix_setter_typed_slot:')
    print('  slots typed by their setter                   : %d' % len(slots))
    for g, (f, t) in sorted(slots.items()):
        print('     %-38s ->%-14s is %s *' % (g, f, t))
    print('  repaired (i386 byte-identical)                : %d' % fixed)
    print('  declined (reported, not guessed)              : %d' % declined)
    for n in notes:
        print('     ' + n)
    for d in declines:
        print('     - ' + d)
    return 0


if __name__ == '__main__':
    sys.exit(main())
