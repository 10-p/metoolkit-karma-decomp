#!/usr/bin/env python3
"""fix_literal_offsets.py — a struct field addressed by a BAKED BYTE OFFSET.

    fix_literal_offsets.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT.

    *(code **)((char *)pMVar1 + 200) = McdModelUpdatePathCompatible;
    McdModelPairManagerSetPoolFullHandler(*(void **)((char *)pMVar1 + 0x2c), ...);

200 and 0x2c are `mAABBUpdateFn` and `mManager` of `CxSmallSort` — ON i386. At
LP64 they are 392 and 80, so both statements address someone else's memory with
nothing truncated and clang silent. This is the class `layout_check.py` counts in
its OFFSET column and the one `ptrwidth_check.sh` is blind to by construction.

THE REPAIR IS A PURE ADDRESS RE-SPELLING, which is what makes it safe:

    *(T *)((char *)p + K)   ->   *(T *)&p->FIELD

The cast and therefore the expression's TYPE are untouched — only the way the
address is computed changes, from a number to a field the compiler recomputes
per target. i386 stays byte-identical by construction, and that is verified per
site rather than asserted.

⚠ THE DECLARED TYPE IS OFTEN THE WRONG ONE, and this is the fourth face of the
same problem (after the geometry allocations, the constraint pool and
`fix_baked_sizeof`'s targets). `pMVar1` is declared `McdSpaceID`, i.e.
`McdSpace *`, and `McdSpace` is an OPAQUE handle — the object really is a
`CxSmallSort`. So the concrete type is inferred from the offsets themselves: the
struct on which EVERY offset used against that declared type in this file lands
on a real field. For McdSpace.c that is {4, 0xc, 0x18, 0x1c, 0x24, 0x2c, 0xc8,
0xcc} and CxSmallSort is the only complete type that covers them.

AND IT IS CONFIRMED AGAINST A BUILD NOBODY HERE MADE. The shipped amd64
metoolkit's `McdSpaceAxisSortCreate` touches displacements 0x50 and 0x188 —
which are exactly `mManager` and `mAABBUpdateFn` at 64-bit, measured from
`kd_types.h`'s own declaration. Two independent 64-bit sources agreeing on a
layout the i386 build cannot express.

SCOPE: 27 sites of this shape in the corpus, 21 in McdSpace.c and 6 in
ReadWriteKeaInputToFile.c. ⚠ `layout_check.py` reports 128 in its OFFSET column
because that count ALSO includes Ghidra's invented stack frames
(`(int)&local + K`, MdtBcl 70 and MdtMainLoop 25). Those are a decompilation
defect, not a layout one, and this tool deliberately does not touch them.

Run it on a COPY: it edits in place. Before `fix_ptrwidth.py`, which would
otherwise rewrite these casts into a shape this no longer recognises.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = '/tmp/kd_offsets'

# THE SITE, in every spelling: an address computed as `BASE + K` with K frozen
# at its i386 value. The deref is not part of it and neither is the cast —
# `*(int *)((int)p + 0xb4)`, `(float *)((int)p + 0xbc)` and
# `(MeReal (*) [4])((int)p + 0xfc)` are the same defect and get the same repair.
SITE = re.compile(r'\(\((?:char|int|kd_iptr)\s*\*?\)'
                  r'(?P<base>[A-Za-z_]\w*(?:->[A-Za-z_]\w*)?)'
                  r'\s*\+\s*(?P<off>0x[0-9a-f]+|\d+)\)')

# ★ THE BASE IS NOT ALWAYS A VARIABLE, and the one that is not was the defect
# that kept scene_chain failing after everything else was repaired:
#
#     if ((*(byte *)((int)pMVar9->data + 0x1ec) & 2) == 0) goto ...
#
# 0x1ec is 492 is `offsetof(MdtBody, flags)` at i386 and 556 at LP64, so the
# partitioner's VISITED test reads a byte 64 short of the flag. No body is ever
# seen as visited, the root loop re-seeds one already placed, and the failure
# shows up as `addedBodies[12]` overflowing a 12-element buffer — a count
# overrun that looks nothing like a layout defect and had me diagnose it as a
# bookkeeping bug twice.
#
# `pMVar9->data` is a `void *` member, so it carries no type of its own. What
# names it is the SAME FIELD read into a typed local elsewhere in the function:
# `pMVar3 = rootNode->data;` with `pMVar3` declared `MdtBody *`. The field is
# the key, not the variable — `rootNode`, `node` and `pMVar9` are three names
# for the same kind of node.
FIELD_READ = re.compile(r'(?m)^[ \t]*(?P<var>[A-Za-z_]\w*)\s*=\s*'
                        r'[A-Za-z_]\w*->(?P<f>[A-Za-z_]\w*)\s*;')

BANNER = re.compile(r'(?m)^/\* ---- (\S+)')


def all_sites(text):
    """Every baked-offset site, in source order."""
    return list(SITE.finditer(text))

CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']
_SZ = re.compile(r'char \(\*\)\[(\d+)\]')


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


HEAD = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
        '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))


# gcc's report for `int x = &probe;` names the array type, and it carries the
# LINE, which is what lets one file answer thousands of probes at once. Matched
# loosely on error-or-warning because which one it is has moved between gcc
# versions and the number is in the text either way.
_BSZ = re.compile(r'batch\.c:(\d+):\d+: (?:error|warning): initialization of .int. '
                  r'from .char \(\*\)\[(\d+)\]')


def measure_many(exprs, inc, cache):
    """Compile-time constants for MANY expressions in ONE gcc run.

    ⚠ ONE PROCESS PER PROBE STOPS SCALING THE MOMENT THE MAP IS NESTED. The flat
    map asked 1,229 questions and paid 1,229 gcc startups; expanding arrays and
    recursing into nested structs asks about 9,000, which is twenty minutes of
    `fork`. gcc reports every line of a file it is given, so one file answers
    all of them.

    AND THE FAILURES ARE THE POINT, not an accident tolerated: `flags[0]` on a
    scalar member simply has no answer on its line, and that silence is exactly
    the signal array-ness is read from. An expression whose type is incomplete
    behaves the same way, so a probe this cannot answer is a member the map
    does not claim to cover."""
    todo = sorted({e for e in exprs if e not in cache})
    if not todo:
        return
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'batch.c')
    line_of, n = {}, HEAD.count('\n')
    with open(src, 'w') as f:
        f.write(HEAD)
        for i, e in enumerate(todo):
            f.write('char kd_p%d[%s];\nint kd_f%d = &kd_p%d;\n' % (i, e, i, i))
            n += 2
            line_of[n] = e
    r = subprocess.run(['gcc', '-m32', '-DLINUX', '-fmax-errors=0',
                        '-I' + os.path.join(HERE, 'include')] + includes(inc)
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    for e in todo:
        cache[e] = None
    for m in _BSZ.finditer(r.stderr):
        e = line_of.get(int(m.group(1)))
        if e is not None:
            cache[e] = int(m.group(2))


def measure(expr, inc, cache):
    """A compile-time constant, read out of gcc's own type printer."""
    if expr not in cache:
        measure_many([expr], inc, cache)
    return cache[expr]


# `TYPE NAME;` / `TYPE NAME[3];` inside a struct body. Bit-fields, function
# pointers and anonymous members are deliberately NOT matched — a member this
# cannot name is a member the offset map must not claim to cover.
#
# ⚠ THE ARRAY BOUND IS NOT ALWAYS A NUMBER, and requiring one dropped members
# silently. `int bodyindex[MdtKeaMAXBODYCONSTRAINT];` did not match `\[\d*\]` at
# all, so `MdtConstraintHeader+0xb4` — the field every one of the sixteen
# MdtBclAdd* functions reads first — was simply not in the map, and the type
# inference then read as "nothing fits" rather than "a member is missing".
MEMBER = re.compile(r'(?m)^[ \t]+(?P<ty>(?:const\s+|struct\s+|unsigned\s+)*[A-Za-z_]\w*'
                    r'[ \t]*\**)[ \t]*(?P<name>[A-Za-z_]\w*)[ \t]*(?:\[[^\]]*\])?[ \t]*;')

_BODIES = {}


def struct_bodies(inc):
    """tag -> the text between its braces, for every struct with a body."""
    if _BODIES:
        return _BODIES
    for root in (os.path.join(HERE, 'include'), inc):
        for dirpath, _d, files in os.walk(root):
            for fn in sorted(files):
                if not fn.endswith('.h'):
                    continue
                txt = open(os.path.join(dirpath, fn), errors='ignore').read()
                for m in re.finditer(r'\bstruct\s+(\w+)\s*\{', txt):
                    depth, i = 0, m.end() - 1
                    while i < len(txt):
                        if txt[i] == '{':
                            depth += 1
                        elif txt[i] == '}':
                            depth -= 1
                            if depth == 0:
                                break
                        i += 1
                    _BODIES.setdefault(m.group(1), txt[m.end():i])
    return _BODIES


_PATHS = {}
_FLAT = {}
_HEAD = {}
ELEM_CAP = 512          # an array wider than this is registered at its base only
DEPTH = 3


def build_paths(inc, cache):
    """Every struct tag's i386 byte offset -> the C path naming the field at it.

    NESTED, AND THE NESTING IS THE WHOLE POINT FOR THE `void *` APIs.
    `MdtBSJoint` is *nothing but* `MdtConstraintHeader head;`, so a flat map of
    its top-level members has exactly one entry and every offset MdtBcl uses
    reads as "does not land". One level of nesting is not enough either —
    `head.ref1[0][0]` is a member, of an array, of an array, of a nested struct.

    ⚠ AND THE OBVIOUS RECURSIVE SPELLING HAS A GUARD BUG THAT RETURNS ZERO. A
    memoising `field_paths(tag, depth)` has to publish its (empty) result before
    it walks, or a cyclic type recurses forever; a deeper walk then re-enters
    that key and gets the empty dict, and the whole map comes back empty — which
    reads as "nothing fits" rather than as a bug. This is written as an
    ITERATIVE expansion with a hard budget instead, so there is no in-progress
    entry to re-enter and no guard to get wrong.

    Every offset is measured from the ROOT tag, so nothing is composed by
    addition and a nested path cannot drift.

    ⚠ ARRAY-NESS IS MEASURED, NOT READ OFF THE DECLARATION. `MeVector3 center;`
    is a `MeReal[3]` behind a typedef and `MeMatrix4 ref1;` is a `MeReal[4][4]`,
    so a scan for brackets records only the first element and the map has holes
    where a repair would have to land. `sizeof(f)` over `sizeof(f[0])` gets it
    right wherever the brackets are, and the `esz >= 2` floor is load-bearing:
    GNU C gives `sizeof(void)` and `sizeof(function)` as 1, so a `void *`-ish
    member would otherwise read as an array of its own byte count.

    ⚠ A POINTER MEMBER CAN STILL BE AN ARRAY. `MdtBody *mdtbody[2]` has a `*` in
    its type and two elements; skipping the expansion on that ground registered
    `mdtbody` at +0x54 and left +0x58 missing, which is one of the two offsets
    every MdtBclAdd* reads. The `*` only decides whether to recurse INTO the
    type, never whether to expand it."""
    if _PATHS:
        return _PATHS
    bodies = struct_bodies(inc)
    for t in bodies:
        _PATHS[t] = {}
        _FLAT[t] = {}
        _HEAD[t] = {}
    work = [(t, m.group('name'), re.sub(r'\s+', ' ', m.group('ty')).strip(), DEPTH)
            for t, body in bodies.items() for m in MEMBER.finditer(body)]
    head = True
    while work:
        probes = []
        for t, e, _ty, _b in work:
            probes += ['((char *)&((struct %s *)0)->%s - (char *)0) + 1' % (t, e),
                       'sizeof(((struct %s *)0)->%s) + 1' % (t, e),
                       'sizeof(((struct %s *)0)->%s[0]) + 1' % (t, e)]
        measure_many(probes, inc, cache)
        nxt = []
        for t, e, ty, b in work:
            off = cache.get('((char *)&((struct %s *)0)->%s - (char *)0) + 1' % (t, e))
            if off is None:
                continue
            msz = cache.get('sizeof(((struct %s *)0)->%s) + 1' % (t, e)) or 1
            esz = cache.get('sizeof(((struct %s *)0)->%s[0]) + 1' % (t, e)) or 1
            off, msz, esz = off - 1, msz - 1, esz - 1
            if head:
                _HEAD[t].setdefault(off, e)
            if esz >= 2 and msz % esz == 0 and 2 <= msz // esz <= ELEM_CAP:
                nxt += [(t, '%s[%d]' % (e, i), ty, b) for i in range(msz // esz)]
                continue
            _PATHS[t].setdefault(off, e)
            if '.' not in e:
                _FLAT[t].setdefault(off, e)
            base = re.sub(r'^(?:const|struct|unsigned)\s+', '', ty).strip()
            if b > 0 and '*' not in ty and base in bodies and base != t:
                nxt += [(t, '%s.%s' % (e, m.group('name')),
                         re.sub(r'\s+', ' ', m.group('ty')).strip(), b - 1)
                        for m in MEMBER.finditer(bodies[base])]
        work, head = nxt, False
    return _PATHS


def offsets_of(tag, inc, cache):
    """i386 byte offset -> the field path at it, for one struct tag."""
    return build_paths(inc, cache).get(tag, {})


def flat_offsets_of(tag, inc, cache):
    """The same map restricted to TOP-LEVEL members — the stronger measurement.

    ⚠ A NESTED MAP IS A WEAKER DISCRIMINATOR AND IT COST TWENTY REPAIRS. The
    concrete-type inference asks which struct every offset lands on, and
    requiring all of them is what made it a measurement rather than a guess.
    Recursing three levels means almost anything covers almost any offset:
    `McdSpace.c`'s six offsets went from fitting ONE type (`CxSmallSort`) to
    fitting TWENTY-TWO, the site declined as ambiguous, and the tool's output
    fell from 22 rewrites to 2 — a silent regression the byte-identity gate
    cannot see, because a decline compiles perfectly.

    So the fit is asked in THREE tiers, narrowest first:

      _HEAD   every offset is the START of a direct member
      _FLAT   ... or an element of one of its arrays
      _PATHS  ... or anywhere at all, three levels deep

    Expanding arrays alone was enough to reintroduce the ambiguity — the same
    six offsets fit three types at the `_FLAT` tier — which is why the narrowest
    tier is the members' own offsets and nothing else. `CxSmallSort` is unique
    there; `MdtBSJoint`, which is *nothing but* a nested header, can only ever
    be answered at the third."""
    build_paths(inc, cache)
    return _FLAT.get(tag, {})


def head_offsets_of(tag, inc, cache):
    """Narrower still: the offset each direct member STARTS at. See above."""
    build_paths(inc, cache)
    return _HEAD.get(tag, {})


def region_of(text, pos):
    start, end = 0, len(text)
    for m in BANNER.finditer(text):
        if m.start() <= pos:
            start = m.start()
        elif m.start() > pos:
            end = m.start()
            break
    return text[start:end]


def enclosing(text, pos):
    last = None
    for m in BANNER.finditer(text, 0, pos):
        last = m.group(1)
    return last


def declared_type(region, var):
    """The type of a local OR a parameter.

    ⚠ PARAMETERS ARE NOT DECLARATIONS. Ghidra writes locals as `T x;` at the top
    of the body and parameters in the signature, so a locals-only lookup found
    `pMVar1` in McdSpaceAxisSortCreate and missed `s` and `space` in every other
    function of the same file — two of twenty-one sites repaired, and the rest
    silently 'declined' for want of a type that was written down three lines
    above them."""
    for m in re.finditer(r'(?m)^[ \t]*((?:const |struct )*[A-Za-z_]\w*[ \t]*\**)'
                         r'[ \t]*' + re.escape(var) + r'[ \t]*;', region):
        ty = re.sub(r'\s+', ' ', m.group(1)).strip()
        if ty.split()[0] not in ('return', 'goto', 'break', 'continue'):
            return ty
    sig = re.search(r'(?s)^/\* ---- .*?\*/\s*\n(.*?)\{', region)
    if sig:
        for p in re.split(r',', sig.group(1)[sig.group(1).find('(') + 1:]):
            m = re.match(r'\s*((?:const |struct )*[A-Za-z_]\w*[ \t]*\**)\s*'
                         + re.escape(var) + r'\s*\)?\s*$', p)
            if m:
                return re.sub(r'\s+', ' ', m.group(1)).strip()
    return None


# A base whose declared type carries no structure tells us nothing, and a
# thousand of them reported as "declined" buries the sites that could be
# repaired. They are out of scope, not refused.
OPAQUE = re.compile(r'^(void|undefined\d*|int|uint|char|float|double|code'
                    r'|MeReal|MeU\d+|MeI\d+)\b')


# ---------------------------------------------------------------- the `void *` API
#
# ★ SOME OF THIS IS NOT GHIDRA LOSING A TYPE — IT IS THE ORIGINAL API, and no
# header can be made to type it. `MdtBcl.h` declares
#
#     void (*MdtBclAddConstraintFn)(MdtKeaConstraints *const, void *const constraint, ...)
#
# so MathEngine hands the Bcl layer an opaque pointer and the Bcl layer knows
# the layout by offset. Every one of the sixteen `MdtBclAdd*` functions takes a
# DIFFERENT constraint type through that same `void *`, which is also why the
# per-file inference above cannot work here: pooling MdtBcl.c's offsets asks one
# question of sixteen answers.
#
# ★ BUT THE TYPE IS WRITTEN DOWN, IN THE INSTALLER. Each of the sixteen is
# registered as a callback on the very type it takes:
#
#     MdtBSJoint.c:54    (pMVar1->head).bclFunction = MdtBclAddBSJoint;
#
# and `pMVar1` is an `MdtBSJointID`. That resolves all sixteen mechanically,
# with no guessing and no aligning of two compilers' instruction streams.
INSTALL = re.compile(r'\(?\s*(?P<var>[A-Za-z_]\w*)\s*->\s*[A-Za-z_]\w*\s*\)?\s*\.\s*'
                     r'[A-Za-z_]\w*\s*=\s*(?P<fn>[A-Za-z_]\w*)\s*;')
INSTALL_FLAT = re.compile(r'(?P<var>[A-Za-z_]\w*)\s*->\s*[A-Za-z_]\w*\s*=\s*'
                          r'(?P<fn>[A-Za-z_]\w*)\s*;')

_TD = {}


def typedefs(inc):
    """name -> the spelling it is a typedef for, over every header."""
    if _TD:
        return _TD
    for root in (os.path.join(HERE, 'include'), inc):
        for dirpath, _d, files in os.walk(root):
            for fn in sorted(files):
                if not fn.endswith('.h'):
                    continue
                txt = open(os.path.join(dirpath, fn), errors='ignore').read()
                for m in re.finditer(r'(?m)^\s*typedef\s+([A-Za-z_][\w \t*]*?)'
                                     r'\s*\**\s*(\w+)\s*;', txt):
                    _TD.setdefault(m.group(2), re.sub(r'\s+', ' ', m.group(1)).strip())
    return _TD


def tag_of(ty, inc):
    """A declared type spelling -> the struct TAG it ultimately names.

    The offsets live on the tag, and what Ghidra writes is the handle:
    `MdtBSJointID` is `MdtBSJoint *` is `struct MdtBSJoint`."""
    t = re.sub(r'\s+', ' ', ty or '').replace('*', '').replace('const ', '')
    t = t.replace('struct ', '').strip()
    for _ in range(8):
        if t in struct_bodies(inc):
            return t
        nxt = typedefs(inc).get(t)
        if not nxt:
            return None
        nxt = nxt.replace('struct ', '').strip()
        if nxt == t:
            return None
        t = nxt
    return None


def corpus_callbacks(texts, inc):
    """FUNCTION -> the struct tag of the object it is installed as a callback on.

    Corpus-wide on purpose: the installer and the function it names are in
    different objects (`MdtBSJoint.c` and `MdtBcl.c`), which is the whole reason
    a per-file rule cannot see this.

    A function installed on two different types is dropped rather than
    arbitrated — there is no evidence here that could choose between them."""
    names = set()
    for t in texts.values():
        names |= {m.group(1) for m in BANNER.finditer(t)}
    out = {}
    for t in texts.values():
        for rx in (INSTALL, INSTALL_FLAT):
            for m in rx.finditer(t):
                f = m.group('fn')
                if f not in names:
                    continue
                tag = tag_of(declared_type(region_of(t, m.start()), m.group('var')), inc)
                if not tag:
                    continue
                out[f] = tag if out.get(f, tag) == tag else None
    return {k: v for k, v in out.items() if v}


def signature_params(region):
    """The parameter NAMES of the function this region holds, in order."""
    sig = re.search(r'(?s)^/\* ---- .*?\*/\s*\n(.*?)\{', region)
    if not sig:
        return []
    sig = sig.group(1)
    i = sig.find('(')
    if i < 0:
        return []
    out, depth, cur = [], 0, ''
    for ch in sig[i:]:
        if ch in '([':
            depth += 1
            if depth == 1:
                continue
        elif ch in ')]':
            depth -= 1
            if depth == 0:
                break
        if ch == ',' and depth == 1:
            out.append(cur)
            cur = ''
        else:
            cur += ch
    out.append(cur)
    return [(re.findall(r'[A-Za-z_]\w*', p) or [''])[-1] for p in out]


def call_args(text, pos):
    """The argument expressions of the call whose `(` is at `pos`."""
    out, depth, cur = [], 0, ''
    for ch in text[pos:]:
        if ch in '([':
            depth += 1
            if depth == 1:
                continue
        elif ch in ')]':
            depth -= 1
            if depth == 0:
                break
        if ch == ',' and depth == 1:
            out.append(cur.strip())
            cur = ''
        else:
            cur += ch
    out.append(cur.strip())
    return out


CALL = re.compile(r'\b(?P<fn>[A-Za-z_]\w*)\s*\(')

# ★ A THIRD SOURCE OF TYPE, AND THE ONLY ONE THAT IS NOT AN INFERENCE AT ALL:
# the vendor declared what the function returns.
#
#     for (pvVar5 = MeDictFirst(pMVar4); ...)
#         MdtBodyResetForces(*(void **)((kd_iptr)pvVar5 + 0x14));
#
# `MeDict.h` says `extern MeDictNode *MeDictFirst(MeDict *);`, and 0x14 is
# `MeDictNode::data` at i386 and **40** at LP64 — so `MdtWorldStep` hands
# `MdtBodyResetForces` a pointer read out of the middle of a tree node.
#
# ⚠ IT HAS TO BE PER FUNCTION AND NOT PER VARIABLE. Ghidra reuses `pvVar5` for
# everything: in `MdtWorldStep` it is assigned from `MeChunkGetMem` at line 218
# and from `MeDictFirst` at 284. `MeChunkGetMem` returns `void *`, which names
# nothing, so the KNOWN assignments still agree — and a function where two of
# them name different structs is declined and reported rather than resolved by
# picking the nearer one.
RETURNS = re.compile(r'(?m)^\s*(?:extern\s+)?(?P<ty>[A-Za-z_]\w*(?:\s+\w+)*\s*\*+)\s*'
                     r'(?:MEAPI\s+)?(?P<fn>\w+)\s*\(')
CALL_ASSIGN = re.compile(r'(?m)(?P<var>[A-Za-z_]\w*)\s*=\s*(?P<fn>[A-Za-z_]\w*)\s*\(')

_RET = {}


def header_returns(inc):
    """FUNCTION -> its declared return type, from the vendor's own headers."""
    if _RET:
        return _RET
    for root in (os.path.join(HERE, 'include'), inc):
        for dirpath, _d, files in os.walk(root):
            for fn in sorted(files):
                if not fn.endswith('.h'):
                    continue
                txt = open(os.path.join(dirpath, fn), errors='ignore').read()
                for m in RETURNS.finditer(txt):
                    _RET.setdefault(m.group('fn'),
                                    re.sub(r'\s+', ' ', m.group('ty')).strip())
    return _RET


def compiles_identically(fn, text, build, inc):
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


def accept_edits(fn, text, edits, build, inc):
    """Apply as many edits as keep the i386 object byte-identical, cheaply.

    ⚠ ONE COMPILE PER EDIT DOES NOT SURVIVE MdtBcl. That file has 600 sites and
    takes a second to build at -O2, so the site-at-a-time loop this replaced is
    twenty minutes for one object. The whole set is tried first and the answer
    is almost always yes; only a set that changes the object gets bisected, and
    then only far enough to find which edits did it.

    ⚠ AND BISECTING IS NOT AN OPTIMISATION HERE, IT IS THE POINT. Edits are not
    independent: `fix_arena_carve` measured five of six composing and the sixth
    breaking them, because two statements shared a subexpression gcc had
    common-subexpression-eliminated. Accepting a subset one half at a time keeps
    that property observable instead of assuming it away."""
    if not edits:
        return text, []
    cand = text
    for start, end, rep, _n in edits:            # descending by position
        cand = cand[:start] + rep + cand[end:]
    if compiles_identically(fn, cand, build, inc):
        return cand, edits
    if len(edits) == 1:
        return text, []
    mid = len(edits) // 2
    text, a = accept_edits(fn, text, edits[:mid], build, inc)
    text, b = accept_edits(fn, text, edits[mid:], build, inc)
    return text, a + b


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else os.path.join(
        HERE, '..', 'Thirdparty', 'metoolkit')
    inc = os.path.join(root, 'include')
    cache = {}

    # ---- THE SELF-CHECK. If the offset probe cannot compile it returns None
    # for everything, every site declines, and "0 repaired" reads exactly like
    # "there was nothing to repair". Three facts, one per tier of the map: a
    # direct member, an element of an array whose bound is a MACRO, and a
    # member of a member of a nested struct.
    probe = offsets_of('CxSmallSort', inc, cache)
    want = [('CxSmallSort', 0x2c, 'mManager'), ('CxSmallSort', 200, 'mAABBUpdateFn'),
            ('MdtConstraintHeader', 0xb8, 'bodyindex[1]'),
            ('MdtBSJoint', 0xbc, 'head.ref1[0][0]')]
    bad = [(t, o, w, offsets_of(t, inc, cache).get(o))
           for t, o, w in want if offsets_of(t, inc, cache).get(o) != w]
    if bad:
        sys.exit('fix_literal_offsets: SELF-CHECK FAILED — '
                 + '; '.join('%s+0x%x read %r (want %r)' % (t, o, g, w)
                             for t, o, w, g in bad)
                 + '. The offset map is not measuring anything.')

    texts = {}
    for fn in sorted(os.listdir(srcdir)):
        if fn.endswith('.c') and os.path.exists(os.path.join(build, fn[:-2] + '.o')):
            texts[fn] = open(os.path.join(srcdir, fn), errors='ignore').read()
    installed = corpus_callbacks(texts, inc)

    # ---- WHICH `void *` IS THE OBJECT, AND WHICH PARAMETER IS IT?
    #
    # The installer says an `MdtBclAdd*` takes an `MdtBSJoint`; it does not say
    # through which of the function's TWO `void *` parameters. That is not
    # guessed either: the answer is the parameter every one of whose baked
    # offsets lands on a real field of that type, and requiring ALL of them is
    # the same measurement the per-file inference uses. `constraint` reads 21 of
    # 21 in MdtBclAddBSJoint; `params` has no baked offsets at all. A function
    # where two parameters both fit is declined and reported.
    opaque_var = {}                    # (file, function, var) -> tag
    param_of = {}                      # FUNCTION -> (arg index, tag)
    notes_early = []
    for fn, text in texts.items():
        for b in BANNER.finditer(text):
            func = b.group(1)
            tag = installed.get(func)
            if not tag:
                continue
            region = region_of(text, b.start())
            paths = offsets_of(tag, inc, cache)
            by_var = {}
            for m in all_sites(region):
                v = m.group('base')
                if '->' in v or not OPAQUE.match(declared_type(region, v) or 'void *'):
                    continue
                by_var.setdefault(v, set()).add(int(m.group('off'), 0))
            fits = [v for v, offs in sorted(by_var.items())
                    if all(o in paths for o in offs)]
            if len(fits) != 1:
                if by_var:
                    notes_early.append(
                        '%-26s %s: installed on %s but %d of its opaque bases fit '
                        '(%s) — declined' % (fn, func, tag, len(fits),
                                             ', '.join(fits) or 'none'))
                continue
            v = fits[0]
            opaque_var[(fn, func, v)] = tag
            params = signature_params(region)
            if v in params:
                param_of[func] = (params.index(v), tag)

    # ---- ONE HOP. `MdtBclAddContactGroup` walks the group's contact list and
    # hands each one to `MdtBclAddContact`, so the local it walks with is an
    # `MdtContact` — named by the call, not by any declaration. The same
    # all-offsets-land test confirms it.
    for fn, text in texts.items():
        for b in BANNER.finditer(text):
            func, region = b.group(1), region_of(text, b.start())
            by_var = {}
            for m in all_sites(region):
                v = m.group('base')
                if '->' in v or (fn, func, v) in opaque_var:
                    continue
                if not OPAQUE.match(declared_type(region, v) or 'void *'):
                    continue
                by_var.setdefault(v, set()).add(int(m.group('off'), 0))
            if not by_var:
                continue
            for c in CALL.finditer(region):
                callee = re.sub(r'^kd_', '', c.group('fn'))
                if callee not in param_of:
                    continue
                idx, tag = param_of[callee]
                args = call_args(region, c.end() - 1)
                if idx >= len(args):
                    continue
                a = args[idx].strip()
                if not re.fullmatch(r'[A-Za-z_]\w*', a) or a not in by_var:
                    continue
                paths = offsets_of(tag, inc, cache)
                if all(o in paths for o in by_var[a]):
                    opaque_var[(fn, func, a)] = tag
                else:
                    notes_early.append(
                        '%-26s %s: %s is handed to %s as an %s but %d of its %d '
                        'offsets do not land — declined'
                        % (fn, func, a, callee, tag,
                           sum(1 for o in by_var[a] if o not in paths), len(by_var[a])))

    tags = [t for t in struct_bodies(inc) if t not in ('_IO_FILE',)]

    # ---- WHAT THE FUNCTION SAID IT RETURNS. See RETURNS above: no inference,
    # just the vendor's declaration, gated by the same all-offsets-land test.
    for fn, text in texts.items():
        for b in BANNER.finditer(text):
            func, region = b.group(1), region_of(text, b.start())
            by_var = {}
            for m in all_sites(region):
                v = m.group('base')
                if '->' in v or (fn, func, v) in opaque_var:
                    continue
                if not OPAQUE.match(declared_type(region, v) or 'void *'):
                    continue
                by_var.setdefault(v, set()).add(int(m.group('off'), 0))
            for v, offs in sorted(by_var.items()):
                named = {tag_of(header_returns(inc).get(
                    re.sub(r'^kd_', '', a.group('fn')), ''), inc)
                    for a in CALL_ASSIGN.finditer(region) if a.group('var') == v}
                named.discard(None)
                if len(named) != 1:
                    if named:
                        notes_early.append(
                            '%-26s %s: %s is assigned from calls returning %s — declined'
                            % (fn, func, v, ', '.join(sorted(named))))
                    continue
                tag = named.pop()
                paths = offsets_of(tag, inc, cache)
                if all(o in paths for o in offs):
                    opaque_var[(fn, func, v)] = tag
                else:
                    notes_early.append(
                        '%-26s %s: %s reads as %s but %d of its %d offsets do not '
                        'land — declined' % (fn, func, v, tag,
                                             sum(1 for o in offs if o not in paths),
                                             len(offs)))

    fixed = declined = skipped = 0
    notes = list(notes_early)
    for fn, text0 in texts.items():
        text = text0
        sites = all_sites(text)
        if not sites:
            continue
        path = os.path.join(srcdir, fn)

        # ---- Which concrete struct is each declared type, in THIS file?
        # The declared type is frequently an opaque handle, so the offsets it is
        # used with are the evidence: the tag on which every one of them lands
        # on a real field. Requiring ALL of them is what makes it a measurement
        # rather than a guess — a single offset would fit dozens of structs.
        # For a `X->F` base the type of the POINTER tells us nothing (`data` is
        # a `void *`); what names it is the same field read into a typed local.
        def base_type(m):
            b = m.group('base')
            region = region_of(text, m.start())
            if '->' not in b:
                return declared_type(region, b)
            fld = b.split('->')[1]
            # The function first — a local reading of the same field is the
            # strongest evidence — then the whole FILE. What a container's
            # member points at is a property of the CONTAINER, not of the
            # function that happens to read it: MdtPartition.c:334 sits in a
            # function with no typed read of `->data`, while the one three
            # hundred lines above has `pMVar3 = rootNode->data;` and both walk
            # the same body dict.
            for scope in (region, text):
                for r in FIELD_READ.finditer(scope):
                    if r.group('f') == fld:
                        t = declared_type(region_of(scope, r.start())
                                          if scope is text else region,
                                          r.group('var'))
                        if t and not OPAQUE.match(t):
                            return t
            return None

        def site_tag(m):
            """(tag, declared type). The installer wins over the declaration —
            for a `void *const` there IS no declaration to lose to."""
            v = opaque_var.get((fn, enclosing(text, m.start()), m.group('base')))
            if v:
                return v, None
            ty = base_type(m)
            return (concrete.get(ty) if ty and not OPAQUE.match(ty) else None), ty

        used = {}
        for m in sites:
            if opaque_var.get((fn, enclosing(text, m.start()), m.group('base'))):
                continue
            ty = base_type(m)
            if ty and not OPAQUE.match(ty):
                used.setdefault(ty, set()).add(int(m.group('off'), 0))
        concrete = {}
        for ty, offs in used.items():
            # THREE TIERS, narrowest first. A tag whose own members explain
            # every offset is a stronger answer than one that needs array
            # elements, which is stronger than one that needs three levels of
            # nesting; asking the widest map first makes almost every site
            # ambiguous — see flat_offsets_of.
            for lookup in (head_offsets_of, flat_offsets_of, offsets_of):
                fits = [t for t in tags
                        if all(o in lookup(t, inc, cache) for o in offs)]
                if fits:
                    break
            # A declared type that IS the concrete type wins outright.
            direct = ty.replace('struct ', '').replace('*', '').strip()
            if direct in fits:
                concrete[ty] = direct
            elif len(fits) == 1:
                concrete[ty] = fits[0]
            elif fits:
                notes.append('%-26s %s: %d offsets fit %d types (%s) — declined'
                             % (fn, ty, len(offs), len(fits), ', '.join(fits[:4])))

        edits = []
        for m in sites:
            tag, ty = site_tag(m)
            off = int(m.group('off'), 0)
            if not tag:
                if ty is None or OPAQUE.match(ty):
                    skipped += 1
                else:
                    declined += 1
                continue
            field = offsets_of(tag, inc, cache).get(off)
            if not field:
                declined += 1
                notes.append('%-26s %s+0x%x is not a field start of %s'
                             % (fn, ty or 'void *', off, tag))
                continue
            # ★ SUBSTITUTE THE LITERAL AND NOTHING ELSE. See REPAIR at the top:
            # re-spelling the whole address as `&p->FIELD` changes what gcc
            # knows and is NOT byte-identical here.
            s, e = m.span('off')
            edits.append((s, e, '((int)((char *)&((struct %s *)0)->%s - (char *)0))'
                          % (tag, field),
                          '%-26s %-14s +0x%-4x -> %s->%s'
                          % (fn, tag, off, m.group('base'), field)))

        edits.sort(key=lambda e: -e[0])
        text, ok = accept_edits(fn, text, edits, build, inc)
        fixed += len(ok)
        keep = {id(e) for e in ok}
        for e in edits:
            notes.append(e[3] if id(e) in keep
                         else e[3] + '   DECLINED: not byte-identical at i386')
        declined += len(edits) - len(ok)
        if text != text0:
            open(path, 'w').write(text)

    print('  baked field offsets rewritten as named fields : %d' % fixed)
    print('  declined (reported, not guessed)              : %d' % declined)
    print('  out of scope (base type carries no structure) : %d' % skipped)
    for n in notes:
        print('     %s' % n)
    return 0


if __name__ == '__main__':
    sys.exit(main())
