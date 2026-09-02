#!/usr/bin/env python3
"""fix_typeid_dispatch.py — a derived geometry field reached through a TYPE-ID BRANCH.

    fix_typeid_dispatch.py <allobj> <kd_build> [metoolkit-root]

THE DEFECT. It is `fix_derived_fields.py`'s defect exactly — a concrete geometry
type's field addressed as an index past `McdGeometry`, which lands right at i386
and moves at LP64 — but in a function where THAT PASS CANNOT ANSWER ITS OWN
QUESTION. `fix_derived_fields` asks "which one concrete type is this base
pointer, in this file?", and infers it from every offset used in the file
landing on a real field. In a POLYMORPHIC function there is no such type:

    MeReal kd_McdGjkFatness(McdGeometryInstanceID ins)          <- McdGjkMaximumPoint.c
    {
      pMVar3 = ins->mGeometry;
      bVar2 = (byte)pMVar3->mRefCtAndID;          <- the type id, in the low 8 bits
      if (bVar2 == 5) {                           <- kMcdGeometryTypeSphyl
    LAB_00010020:
        MVar1 = *(float *)&(pMVar3[1].mRefCtAndID);         is  McdSphyl::mRadius
      }
      else {
        if (bVar2 < 6) { if (bVar2 == 1) goto LAB_00010020; }   <- Sphere, same field
        else if (bVar2 == 7) {                    <- kMcdGeometryTypeConvexMesh
          return *(float *)&(pMVar3[2].frame);              is  McdConvexMesh::mFatness
        }
        MVar1 = 0.0;
      }
      return MVar1;
    }

    McdGjkMaximumPoint.c   4 site(s): 38 concrete type(s) fit [16, 44]  -> DECLINED

38 structs have a field at both 16 and 44, so the file-wide inference is
ambiguous and all four sites decline. ★ THE SOURCE STATES THE TYPE ANYWAY: the
branch the site sits in has already compared the type id against a named
constant. This pass reads that instead of guessing.

★★ AND THIS WAS A REAL DEFECT, NOT A TIDY-UP. Measured 2026-09-01 on
test-karma-1 under the Onslaught gametype, an SSE-32 build against an LP64 one
so the FP model is identical on both sides:

    McdConvexMesh   i386   mHull 16   mFatness 44   mBSRadius 48   mBSCenter 52
                    LP64   mHull 32   mFatness 80   mBSRadius 84   mBSCenter 88
    pMVar3[2].frame ->     i386 byte 44 = mFatness       LP64 byte 88 = mBSCenter[0]

So at LP64 `McdGjkFatness` returned a convex mesh's BOUNDING SPHERE CENTRE X
where GJK wanted its margin, and `McdGjkMaximumPoint` subtracted that from every
support point. `ONSHoverBike3` vs `StaticMeshActor2` — Box against ConvexMesh,
the one such pair in the map — went `touch=1 n=3` at 32-bit and `touch=0 n=0` at
64-bit, the bike lost the 1.06 y-impulse those contacts carry, and the two
builds' trajectories parted at frame 9 after eight bit-identical frames.

THE RULE, and every part of it is measured rather than assumed:

  1. the site is `BASE[k].FIELD` with FIELD a field of `McdGeometry` — the same
     shape `fix_derived_fields` repairs, and this pass runs after it so it only
     ever sees what that one declined.
  2. some statement in the function derives a type id from BASE:
     `V = (byte)BASE->mRefCtAndID;` and friends. `McdGeometryGetTypeId` is
     `mRefCtAndID & 255`, so the low byte IS the type.
  3. the INNERMOST `{...}` block enclosing the site whose `if` compares V
     against an integer literal gives the type id.
  4. ⚠ EVERY id THAT REACHES THE SITE HAS TO AGREE, not just the nearest one.
     The `[1].mRefCtAndID` site above is reached by `bVar2 == 5` directly AND by
     `bVar2 == 1` through a `goto`, so the pass finds the label, finds every
     `goto` to it, resolves each one's guard, and requires all of them to give
     the same field NAME, the same field TYPE and the same LP64 offset. Sphere
     and Sphyl both put `mRadius` at 16/32, so they agree and the site is
     repaired; had they disagreed it would decline.
  5. the literal names a struct through the `kMcdGeometryType*` enum, and that
     struct must EXIST and must have a field at the i386 offset.
  6. the rewrite must reproduce the baseline i386 object BYTE FOR BYTE.

⚠⚠ AND (6) IS A BACKSTOP, NOT THE EVIDENCE. This project has recorded five
occasions where i386 byte-identity could not see a semantic change at 64-bit —
most recently sixteen rewrites of a transform matrix, all byte-identical because
`2*sizeof(E)` is `0x30` at i386. The evidence for THIS pass is (2)-(5), which
are statements about the source and the headers, plus the ktrace measurement
above. The gate only catches a rewrite that is wrong at BOTH widths.

Run it on a COPY: it edits in place. AFTER fix_derived_fields, BEFORE fix_ptrwidth.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402
from fix_derived_fields import (BANNER, BASES, HEAD, compiles_identically,  # noqa: E402
                                field_paths, includes, measure, struct_bodies)

WORK = '/tmp/kd_typeid'

# The enum in McdGeometryTypes.h names the struct: kMcdGeometryTypeConvexMesh is
# McdConvexMesh. Read from the header rather than tabulated here, so a type this
# corpus does not use cannot be silently wrong — and every name is then checked
# against the structs that actually exist.
ENUM = re.compile(r'\bkMcdGeometryType(\w+)\s*=\s*(\d+)')

# `V = (byte)BASE->mRefCtAndID;`, `V = BASE->mRefCtAndID & 0xff;`,
# `V = McdGeometryGetTypeId(BASE);` — the three spellings the corpus uses for
# "the low byte of mRefCtAndID", which is what McdGeometryGetTypeId expands to.
TYPEVAR = [
    re.compile(r'(?P<v>[A-Za-z_]\w*)\s*=\s*\(\s*(?:byte|unsigned char|uchar)\s*\)\s*'
               r'(?P<b>[A-Za-z_]\w*)\s*->\s*mRefCtAndID\s*;'),
    re.compile(r'(?P<v>[A-Za-z_]\w*)\s*=\s*(?P<b>[A-Za-z_]\w*)\s*->\s*mRefCtAndID\s*'
               r'&\s*(?:0xff|255)\s*;'),
    re.compile(r'(?P<v>[A-Za-z_]\w*)\s*=\s*McdGeometryGetTypeId\s*\(\s*(?P<b>[A-Za-z_]\w*)\s*\)\s*;'),
]

LABEL = re.compile(r'(?m)^\s*(LAB_[0-9a-fA-F]+)\s*:')

_SZ = re.compile(r'char \(\*\)\[(\d+)\]')


def probe(body, inc, bits, pat):
    """Compile a one-liner and read the answer out of gcc's own diagnostic.

    The same trick `fix_derived_fields.measure` uses, with the WIDTH as a
    parameter — because "the two types agree at LP64 too" is the thing this pass
    has to know and cannot know from a 32-bit probe."""
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p%d.c' % bits)
    open(src, 'w').write(HEAD + body)
    r = subprocess.run(['gcc', '-m%d' % bits, '-DLINUX'] + includes(inc)
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    m = pat.search(r.stderr)
    return m.group(1) if m else None


def offset_at(tag, fp, inc, bits, cache):
    key = (tag, fp, bits)
    if key not in cache:
        v = probe('char kd_probe[((char *)&((%s *)0)->%s - (char *)0) + 1];\n'
                  'int kd_force = &kd_probe;\n' % (tag, fp), inc, bits, _SZ)
        cache[key] = (int(v) - 1) if v else None
    return cache[key]


def field_size(tag, fp, inc, bits, cache):
    """sizeof of `((TAG *)0)->FP` at `bits`, or None."""
    key = ('sz', tag, fp, bits)
    if key not in cache:
        v = probe('char kd_probe[sizeof(((%s *)0)->%s) + 1];\n'
                  'int kd_force = &kd_probe;\n' % (tag, fp), inc, bits, _SZ)
        cache[key] = (int(v) - 1) if v else None
    return cache[key]


def type_compatible(t1, f1, t2, f2, inc, cache):
    """Do two candidate fields have the SAME C type?

    ⚠ ASKED OF THE COMPILER, NOT SCRAPED OUT OF A DIAGNOSTIC. The first version
    of this provoked an error and matched clang's `has type 'float *'` wording;
    gcc says `assignment to 'float *' from 'int'` instead, so the match failed,
    every type read back as None, and EVERY SITE DECLINED — the failure looked
    exactly like "there was nothing here to repair". `__builtin_types_compatible_p`
    is a compile-time constant and answers the actual question."""
    key = (t1, f1, t2, f2)
    if key not in cache:
        v = probe('char kd_probe[__builtin_types_compatible_p('
                  '__typeof__(((%s *)0)->%s), __typeof__(((%s *)0)->%s)) + 1];\n'
                  'int kd_force = &kd_probe;\n' % (t1, f1, t2, f2), inc, 32, _SZ)
        cache[key] = (v == '2')          # 1 (compatible) + 1
    return cache[key]


def region_bounds(text, pos):
    """The `/* ---- name ---- */` block containing pos, as (start, end).

    ⚠ BOUNDS, NOT THE SUBSTRING. Taking the text and then looking for it again
    with `.index()` finds the FIRST identical block, and two small recovered
    functions in one file are identical often enough for that to matter."""
    start, end = 0, len(text)
    for m in BANNER.finditer(text):
        if m.start() <= pos:
            start = m.start()
        elif m.start() > pos:
            end = m.start()
            break
    return start, end


def enum_types(inc):
    """type id -> struct name, for the ids whose struct actually exists."""
    txt = open(os.path.join(inc, 'McdGeometryTypes.h'), errors='ignore').read()
    bodies = struct_bodies(inc)
    out = {}
    for name, num in ENUM.findall(txt):
        tag = 'Mcd' + name
        if tag in bodies:
            out[int(num)] = tag
    return out


def blocks(region):
    """[(open_index, close_index, header_text)] for every `{...}` in the region."""
    stack, out = [], []
    i, n = 0, len(region)
    while i < n:
        c = region[i]
        if c == '{':
            stack.append(i)
        elif c == '}' and stack:
            o = stack.pop()
            out.append((o, i, region[max(0, o - 300):o]))
        i += 1
    return out


COND = re.compile(r'(?:else\s+)?if\s*\((?P<c>[^()]*(?:\([^()]*\)[^()]*)*)\)\s*$')
EQ = [re.compile(r'^\s*(?P<v>[A-Za-z_]\w*)\s*==\s*(?P<n>\d+)\s*$'),
      re.compile(r'^\s*(?P<n>\d+)\s*==\s*(?P<v>[A-Za-z_]\w*)\s*$')]

# ⚠ A BRACE-LESS `if` IS A GUARD TOO, and leaving it out is not a gap in
# coverage — it is a WRONG ANSWER. Ghidra writes the Sphere arm of the geometry
# dispatch as `if (bVar2 == 1) goto LAB_00010020;`, with no braces at all. A
# brace-only walk finds no equality there, so the `goto` into the Sphyl arm
# reads as "reached from somewhere unknown" and the site declines — or, worse,
# a version that ignored unresolvable gotos would have concluded the site is
# Sphyl-only and rewritten it on half the evidence.
BARE = re.compile(r'(?:else\s+)?if\s*\(\s*(?P<v>[A-Za-z_]\w*)\s*==\s*(?P<n>\d+)\s*\)\s*'
                  r'(?P<body>(?!\{)[^;{}]*;)')


def guard_of(region, pos, tyvar, blks):
    """The type id of the innermost `if (tyvar == LIT)` guard containing pos.

    -> (id, open_index, close_index) or None."""
    best = None
    for o, c, head in blks:
        if not (o < pos < c):
            continue
        m = COND.search(head.rstrip())
        if not m:
            continue
        for eq in EQ:
            q = eq.match(m.group('c'))
            if q and q.group('v') == tyvar:
                if best is None or o > best[1]:
                    best = (int(q.group('n')), o, c)
    for m in BARE.finditer(region):
        if m.group('v') != tyvar:
            continue
        o, c = m.start('body'), m.end('body')
        if o <= pos < c and (best is None or o > best[1]):
            best = (int(m.group('n')), o, c)
    return best


def reaching_ids(region, pos, tyvar, blks):
    """Every type id that can reach the site, or None if any cannot be resolved.

    ⚠ THE `goto` IS NOT A DETAIL. Ghidra turns the Sphere arm of the dispatch
    into `if (bVar2 == 1) goto LAB_00010020;` where the label sits inside the
    Sphyl arm, so the nearest enclosing `if` sees only 5 and a rule that stopped
    there would be reasoning about one of the two types that get here. A label
    whose guard cannot be read is an UNKNOWN reaching id and declines the site,
    rather than being ignored."""
    g = guard_of(region, pos, tyvar, blks)
    if g is None:
        return None
    ids, (first, o, c) = {g[0]}, g
    for m in LABEL.finditer(region, o, pos):        # labels above the site, in its block
        for j in re.finditer(r'\bgoto\s+' + m.group(1) + r'\s*;', region):
            if o <= j.start() <= c:
                continue                            # a back-edge inside the same arm
            gj = guard_of(region, j.start(), tyvar, blks)
            if gj is None:
                return None
            ids.add(gj[0])
    return sorted(ids), first


# ---------------------------------------------------------------------------
# A SECOND SITE SHAPE, AND TWO MORE WAYS TO KNOW THE TYPE.
#
# ★★ THE SITES BELOW ARE THE ONES THAT PUT A POINTER'S BYTES WHERE A BOX'S HALF
# EXTENTS BELONG, and they are the frame-9 defect this project chased for three
# sessions. `McdGjkMaximumPoint`'s Box arm reads
#
#     fVar10 = *(float *)((kd_iptr)pvVar9 + 0x10);      /* McdBox::mR[0] at i386 */
#
# and `McdBox.mR` is at 16/20/24 at i386 and **32/36/40** at LP64 — measured,
# not argued — so at 64-bit those three loads come out of `McdGeometry`'s `next`
# and `frame` POINTERS. `mR[1]` is a pointer's HIGH half, which is a tiny
# denormal that MOVES WITH ASLR, so the build's physics was not merely wrong, it
# was not even the same twice. Box-vs-ConvexMesh is the only pair that reaches
# this arm, which is why exactly one body of fifteen misbehaved.
#
# ⚠ NEITHER EXISTING PASS COULD SEE IT, AND NOT FOR WANT OF A PATTERN.
# `fix_literal_offsets` matches this site exactly and declines it, because the
# base is a `void *` from an accessor and its "which struct do all the offsets
# in this file land on" inference is ambiguous ({0x10,0x14,0x18} fits dozens).
# `fix_derived_fields` and this pass's own SITE want `BASE[k].FIELD`. The
# missing ingredient was never the shape — it was the TYPE, and the type is
# stated twice over in the source.
# ---------------------------------------------------------------------------

# `V = (byte)INS->mGeometry->mRefCtAndID;` — the type id taken through a
# geometry INSTANCE rather than off a geometry directly. ⚠ TYPEVAR cannot see
# this: its `(?P<b>\w+)->mRefCtAndID` captures `mGeometry`, a FIELD name, so the
# site's base never matches it and every such arm declined silently.
TYPEVAR_INS = re.compile(
    r'(?P<v>[A-Za-z_]\w*)\s*=\s*\(\s*(?:byte|char|unsigned char|uchar)\s*\)\s*'
    r'(?P<ins>[A-Za-z_]\w*)\s*->\s*mGeometry\s*->\s*mRefCtAndID\s*;')

# The same question as a `switch`, which is how the slice dispatcher spells it.
SWITCH_INS = re.compile(
    r'switch\s*\(\s*\(\s*(?:byte|char|unsigned char|uchar)\s*\)\s*'
    r'(?P<ins>[A-Za-z_]\w*)\s*->\s*mGeometry\s*->\s*mRefCtAndID\s*\)')

# `B = McdGeometryInstanceGetGeometry(INS);` / `B = INS->mGeometry;` — the two
# spellings of "the geometry of this instance". The accessor returns `void *`,
# which is exactly what leaves the base untyped.
GEOM_OF = [
    re.compile(r'(?P<b>[A-Za-z_]\w*)\s*=\s*McdGeometryInstanceGetGeometry\s*'
               r'\(\s*(?P<ins>[A-Za-z_]\w*)\s*\)\s*;'),
    re.compile(r'(?P<b>[A-Za-z_]\w*)\s*=\s*(?P<ins>[A-Za-z_]\w*)\s*->\s*mGeometry\s*;'),
]

# fix_literal_offsets' SITE, restated. The deref and the cast are deliberately
# NOT part of the match: only the ADDRESS is re-spelled, so the expression's
# type is untouched and i386 stays byte-identical by construction.
OFFSITE = re.compile(r'\(\((?:char|int|kd_iptr)\s*\*?\)'
                     r'(?P<base>[A-Za-z_]\w*)\s*\+\s*(?P<off>0x[0-9a-f]+|\d+)\)')

CASE = re.compile(r"case\s+'(?P<lit>\\x[0-9a-fA-F]{1,2}|\\[0-7]{1,3}|\\.|.)'\s*:")

# A FUNCTION's banner, as opposed to `/* ---- forward declarations ---- */` and
# the prelude, which `BANNER` also matches.
DEFN_BANNER = re.compile(r'(?m)^/\* ---- (\S+) \(exported as')


def case_value(lit):
    """The integer a C character literal denotes. `'\\x02'`, `'\\0'`, `'\\a'`,
    `'\\b'` all appear in the switch this reads, and Python's own escape rules
    agree with C's on every one of them."""
    try:
        return ord(('"%s"' % lit).encode().decode('unicode_escape')[1:-1])
    except (UnicodeDecodeError, IndexError, TypeError):
        return None


def dispatch_ids(text):
    """function name -> the ONE geometry type id it is reached for, file-local.

    ★ THE DISPATCHER IS THE PROOF, and it is a statement in the source rather
    than an inference about it:

        switch((char)ins->mGeometry->mRefCtAndID) {
        case '\\x02': kd_McdBoxGetSlice(ins,normal,dist,maxVert,numVert,outVert); return;

    `kd_McdBoxGetSlice`'s `ins` IS a box, because that is the only way control
    reaches it. ⚠ AND THE FUNCTION MUST BE REACHED NO OTHER WAY: a name called
    from anywhere outside a case arm is dropped, because one unguarded caller is
    enough to make the whole claim false.

    ⚠ FILE-LOCAL ON PURPOSE. Both dispatchers this repairs sit in the same
    translation unit as the function they dispatch to. A cross-file version
    would need the whole corpus's call graph to be sure of the "no other
    caller" half, and a rule that is sure of half of itself is not a rule."""
    ids, seen_elsewhere = {}, set()
    for sw in SWITCH_INS.finditer(text):
        ins = sw.group('ins')
        o = text.find('{', sw.end())
        if o < 0:
            continue
        depth, i, n = 0, o, len(text)
        while i < n:                                    # the switch's own block
            if text[i] == '{':
                depth += 1
            elif text[i] == '}':
                depth -= 1
                if depth == 0:
                    break
            i += 1
        body, base = text[o:i], o
        marks = [(m.start(), case_value(m.group('lit')), m.end())
                 for m in CASE.finditer(body)]
        for k, (s, val, e) in enumerate(marks):
            if val is None:
                continue
            end = marks[k + 1][0] if k + 1 < len(marks) else len(body)
            for c in re.finditer(r'\b(kd_[A-Za-z_]\w*)\s*\(\s*' + ins + r'\s*[,)]',
                                 body[e:end]):
                ids.setdefault(c.group(1), set()).add(val)
                seen_elsewhere.add((c.group(1), base + e + c.start()))
    # ⚠⚠ AND THE "NO OTHER CALLER" HALF IS STRUCTURAL, NOT A TEXT HEURISTIC.
    # i386 byte-identity CANNOT validate a type — any field at the same offset
    # compiles the same — so this claim is load-bearing and a brittle way of
    # making it would be a wrong repair that every gate waves through. Two
    # things in the file mention the name and are not calls: the forward
    # DECLARATION, which lives above the first function, and the function's own
    # DEFINITION, which follows its banner. Both are located by structure.
    # Anything else outside a case arm disqualifies the name.
    #
    # ⚠ `BANNER` IS NOT THE RIGHT MARKER HERE and using it silently emptied this
    # map: `/* ---- hand-written prelude ---- */` and
    # `/* ---- forward declarations ---- */` match it too, so the declarations
    # landed *after* the "first banner" and every name read as strayed. A
    # function's banner is the one that says `(exported as`.
    fnb = list(DEFN_BANNER.finditer(text))
    decls_end = fnb[0].start() if fnb else len(text)
    defn = {m.group(1): m.end() for m in fnb}
    out = {}
    for name, vals in ids.items():
        if len(vals) != 1:
            continue
        own = defn.get(name[3:] if name.startswith('kd_') else name, defn.get(name))
        guarded = {p for n, p in seen_elsewhere if n == name}
        stray = False
        for m in re.finditer(r'\b' + re.escape(name) + r'\s*\(', text):
            p = m.start()
            if p < decls_end:                       # the forward declaration
                continue
            if own is not None and own <= p <= own + 400:
                continue                            # its own definition header
            if any(abs(p - g) < 2 for g in guarded):
                continue                            # a dispatcher case arm
            stray = True
            break
        if not stray:
            out[name] = next(iter(vals))
    return out


def if_dispatch_ids(text):
    """function -> {parameter index: type id}, from `if (V == N) { … FN(…,ins,…) }`.

    ★ THE SWITCH IS NOT THE ONLY DISPATCHER, AND THE INSTANCE IS NOT ALWAYS THE
    FIRST ARGUMENT. `McdBatch` guards with a plain `if` and passes the geometry
    instance THIRD:

        bVar3 = (byte)ins1_00->mGeometry->mRefCtAndID;
        if (bVar3 == 8) {                       kMcdGeometryTypeAggregate
          … kd_McdBatchFlattenAggregate(context, 0, ins1_00, ins2_00, …);

    and inside that callee `McdGeometryInstanceGetGeometry(ins1)` is read at
    `+0x10` and `+0x18` — `McdAggregate::elementTable` and `elementCountMax` at
    i386, and `McdGeometry`'s `next` and `frame` POINTERS at LP64. An element
    table and a loop bound taken out of two pointers.

    ⚠⚠ SELF-RECURSION IS EXCLUDED FROM THE "no other caller" TEST, AND THAT IS AN
    ASSUMPTION, NOT A MEASUREMENT. `McdBatchFlattenAggregate` calls itself, and
    both recursive paths are reached through INVERTED guards (`if (bVar5 != 8)`
    with the interesting work in the complement) that this pass deliberately does
    not try to read — a complement is where control-flow subtleties bite, and a
    rule that is sure of half of itself is not a rule. What is relied on instead:
    the callee dereferences the aggregate's element table UNCONDITIONALLY, so a
    non-aggregate reaching it would already crash the i386 build, which runs.
    That is empirical, it is stated here rather than buried, and the ktrace and
    ONS smoke runs are what stand behind it."""
    out = {}
    params_of = {}
    for m in DEFN_BANNER.finditer(text):
        sig = text[m.end():m.end() + 600]
        p = re.search(r'\(([^)]*)\)', sig)
        if p:
            params_of[m.group(1)] = [
                re.sub(r'.*[\s\*](\w+)\s*$', r'\1', a.strip())
                for a in p.group(1).split(',')]

    for tv in TYPEVAR_INS.finditer(text):
        v, ins = tv.group('v'), tv.group('ins')
        rs, re_ = region_bounds(text, tv.start())
        region, blks = text[rs:re_], blocks(text[rs:re_])
        for c in re.finditer(r'\b(?:kd_)?(?P<fn>[A-Za-z_]\w*)\s*\((?P<args>[^();]*)\)', region):
            args = [a.strip() for a in c.group('args').split(',')]
            if ins not in args:
                continue
            g = guard_of(region, c.start(), v, blks)
            if not g:
                continue
            fn = re.sub(r'^kd_', '', c.group('fn'))
            if fn not in params_of:
                continue
            out.setdefault(fn, {})[args.index(ins)] = g[0]

    # every non-recursive, unguarded call of the same name disqualifies it.
    # ⚠ THE FORWARD DECLARATION IS NOT A CALL, and counting it as one drops
    # every function here — its "arguments" are the parameter list, which is
    # guarded by nothing. Same structural marker as `dispatch_ids`.
    fnb = list(DEFN_BANNER.finditer(text))
    decls_end = fnb[0].start() if fnb else len(text)
    final = {}
    for fn, idx in out.items():
        if len(idx) != 1:
            continue
        i, tid = next(iter(idx.items()))
        bad = False
        for c in re.finditer(r'\b(?:kd_)?%s\s*\((?P<args>[^();]*)\)' % re.escape(fn), text):
            if c.start() < decls_end:
                continue                        # the forward declaration
            crs, _ = region_bounds(text, c.start())
            if region_fn(text, crs) == fn:
                continue                        # self-recursion: see the docstring
            args = [a.strip() for a in c.group('args').split(',')]
            if len(args) <= i:
                continue
            rr, re2 = region_bounds(text, c.start())
            reg2 = text[rr:re2]
            ok = False
            for tv in TYPEVAR_INS.finditer(reg2):
                if tv.group('ins') != args[i]:
                    continue
                g = guard_of(reg2, c.start() - rr, tv.group('v'), blocks(reg2))
                if g and g[0] == tid:
                    ok = True
            if not ok:
                bad = True
                break
        if not bad:
            final[fn] = (i, tid, params_of[fn][i] if i < len(params_of[fn]) else None)
    return final


def region_fn(text, start):
    """The `/* ---- name ---- */` banner name for the region beginning at start."""
    m = BANNER.search(text, start)
    return m.group(1) if m and m.start() == start else None


try:
    import amd64_oracle                                     # noqa: E402
except Exception:                                           # pragma: no cover
    amd64_oracle = None

_ORACLE = {}
_LLP64 = {}


def llp64_safe(tag, inc):
    """Is `tag`'s layout the SAME in the shipped Windows build as on Linux?

    ⚠⚠ THE SHIPPED 64-BIT BUILD IS LLP64, NOT LP64 — `amd64_oracle`'s own header
    says so and it is the one thing that invalidates this whole evidence source
    when ignored. MSVC's `long` is FOUR bytes; Linux's is eight. A struct holding
    one is a different size and shape in the two, so its Windows displacements
    are NOT Linux's LP64 offsets:

        sizeof(MdtBody)   i386 576   win64 696   linux64 704

    Asked of two compilers rather than assumed: if MinGW and gcc -m64 agree on
    `sizeof(tag)`, the layouts coincide and the oracle may speak for Linux. If
    they disagree the tag is UNCONFIRMABLE and the site declines — which is what
    `fix_index_layout.oracle_confirm` already does, and this pass had been
    silently skipping.

    ⚠⚠ THE SIZE IS READ OUT OF `-S` OUTPUT, NOT OUT OF A DIAGNOSTIC. The
    `int x = &array;` trick every other probe here uses makes gcc print
    `char (*)[48]`; MinGW answers `initializer element is not computable at load
    time` and prints no type at all, so every tag measured as "unconfirmable"
    and this whole evidence source switched itself off — silently, and in the
    safe direction, which is exactly how it would never have been noticed.
    `.comm`/`.space`/`.zero` carries the number on every target."""
    if tag in _LLP64:
        return _LLP64[tag]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'llp.c')
    open(src, 'w').write(HEAD + 'char kd_size_probe[sizeof(%s)];\n' % tag)
    sizes = []
    for cc in (['gcc', '-m64'], ['x86_64-w64-mingw32-gcc']):
        r = subprocess.run(cc + ['-DLINUX'] + includes(inc)
                           + ['-w', '-S', '-o', '-', src],
                           capture_output=True, text=True)
        m = re.search(r'\.(?:comm|space|zero)\s+(?:[._a-zA-Z]*kd_size_probe\s*,\s*)?(\d+)',
                      r.stdout)
        sizes.append(int(m.group(1)) if m else None)
    _LLP64[tag] = (sizes[0] is not None and sizes[0] == sizes[1])
    return _LLP64[tag]


def oracle_disps(fnname):
    """(displacements, symbol) MathEngine's own x86-64 build uses in `fnname`.

    ★★ THE EVIDENCE THE i386 GATE CANNOT SUPPLY. `CLAUDE.md` warns that
    byte-identity cannot validate a TYPE, and six defects have proved it. The
    SDK drop ships a 64-bit build of these sources, and `amd64_oracle` already
    reads it — this pass had simply never asked.

    Measured on `McdBoxMaximumPointNew`, which has NO caller in this corpus and
    NO enclosing type-id branch, so every other rule here declines it:

        mulss 0x20 / 0x24 / 0x28      =  McdBox::mR[0..2] at LP64 (32/36/40)

    while the recovery reads 0x10/0x14/0x18. Nothing is inferred.

    ⚠ Matched as a SUBSTRING: C symbols are plain but C++ ones are MSVC-mangled
    (`?McdBoxMaximumPointNew@@YAXPEAU_McdGeometryInstance@@QEBMQEAM@Z`), so an
    exact-key lookup finds half of them."""
    if amd64_oracle is None or not fnname:
        return None
    if fnname in _ORACLE:
        return _ORACLE[fnname]
    try:
        sym, lines = amd64_oracle.find_function(fnname)
    except SystemExit:
        _ORACLE[fnname] = None
        return None
    hits = amd64_oracle.field_displacements(lines) if lines else set()
    _ORACLE[fnname] = (hits, sym) if hits else None
    return _ORACLE[fnname]


def oracle_type(fnname, offs, types, inc, cache32, offcache):
    """The one geometry type whose LP64 layout matches the shipped amd64 build.

    `fix_derived_fields` declines `McdGjkMaximumPoint` because "38 concrete
    type(s) fit [16, 44]" at i386. Only some of them also put those fields where
    the amd64 binary reaches for them.

    ⚠ EVERY offset must map and EVERY resulting LP64 offset must appear. A
    partial match is not a match, and a function the oracle does not contain
    returns None — which declines rather than guesses.

    ⚠ SEVERAL CANDIDATES AGREEING ON THE BYTE IS AN ANSWER, NOT A TIE. `McdNull`,
    `McdBox` and `McdCylinder` all put three floats at 16/20/24 and all move them
    to 32/36/40, so the ADDRESS is not in doubt and only the spelling is; the
    tie-break is the oracle's own mangled symbol, which is MathEngine's naming
    and not this project's."""
    got = oracle_disps(fnname)
    if not got or not offs:
        return None
    disps, sym = got
    winners = []
    for tid, tag in sorted(types.items()):
        if not llp64_safe(tag, inc):
            continue                    # LLP64: this tag's win64 layout is not Linux's
        fps = field_paths(tag, inc, cache32)
        shape, ok = [], True
        for o in sorted(offs):
            fp = fps.get(o)
            o64 = offset_at(tag, fp, inc, 64, offcache) if fp else None
            if o64 is None:
                ok = False
                break
            shape.append((fp, o64))
        if ok and shape and {o64 for _f, o64 in shape} <= disps:
            winners.append((tid, tag, tuple(shape)))
    if not winners:
        return None
    if len({tuple(o for _f, o in w[2]) for w in winners}) != 1:
        return None                     # they disagree on which BYTE — no answer
    if len({w[2] for w in winners}) == 1:
        return winners[0][0], winners[0][1]
    named = [w for w in winners if w[1] in (sym or '')]
    return (named[0][0], named[0][1]) if len(named) == 1 else None


def compiles_at_m64(fn, text, inc):
    """Does this candidate COMPILE at LP64 at all?

    ⚠⚠ THE i386 BACKSTOP CANNOT ANSWER THIS, AND A GUARDED REPAIR MAKES IT
    STRUCTURALLY BLIND. `#if __SIZEOF_POINTER__ == 4` keeps the ORIGINAL text on
    the i386 side, so byte-identity passes by construction while the `#else`
    branch is whatever was proposed — including, measured 2026-09-02,
    `((McdTriangleList *)t)->list)->mRefCtAndID`, a member `McdUserTriangle` does
    not have. The pipeline reported `145 object(s), 0 byte difference(s)` and then
    failed to build at all.

    So every accepted edit is compiled at -m64 as well. It is not a semantic
    check — a wrong FIELD of the right width still compiles — but it is the one
    thing byte-identity provably cannot do, and it is two seconds."""
    d = os.path.join(WORK, 'm64')
    os.makedirs(d, exist_ok=True)
    src = os.path.join(d, fn)
    open(src, 'w').write(text)
    return subprocess.run(
        ['gcc', '-m64', '-O2', '-fno-strict-aliasing', '-std=gnu99', '-w',
         '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX',
         '-I' + os.path.join(kd_paths.MD, 'include')] + includes(inc)
        + ['-c', '-o', os.devnull, src], capture_output=True).returncode == 0


# ⚠⚠ A STATEMENT ANOTHER PASS HAS ALREADY REPAIRED IS OFF LIMITS, AND THIS COST
# A HEAP OVERFLOW. This pass runs early; `fix_baked_sizeof`, `fix_element_stride`
# and `fix_literal_offsets` run around it and key on the SHAPE of the expression
# they are going to fix. Rewriting `((char *)g + offsetof(T,f))` into `g->f`
# removes their anchor, so their repair never happens and the raw i386 literal
# survives:
#
#   before   alloca(trilistgeom->triangleMaxCount * (int)sizeof(*(McdUserTriangle *)0))
#   after    alloca(((McdTriangleList *)trilistgeom)->triangleMaxCount * 0x18)
#
# `sizeof(McdUserTriangle)` is 24 at i386 and 48 at LP64, so that allocates HALF
# the triangle list and the physics goes non-deterministic — measured, twice, as
# two different ktrace hashes from one binary. Neither the acceptance gate nor
# the -m64 compile check can see it; only the trace against the 32-bit control
# can. So: if the statement already contains an offsetof idiom or a `sizeof`,
# somebody else owns it.
REPAIRED = re.compile(r'-\s*\(char \*\)0\)|\bsizeof\s*\(|\bKD_OFFSET\b')


def already_repaired(text, pos):
    """Is the statement containing `pos` already in a repaired (portable) form?"""
    a = text.rfind(';', 0, pos) + 1
    b = text.find(';', pos)
    return bool(REPAIRED.search(text[a:b if b > 0 else len(text)]))


# ⚠⚠ QUARANTINE, WITH THE MEASUREMENT THAT PUT IT HERE.
#
# `IxSphereTriList.c` is excluded from this pass's NEW evidence sources. Landing
# its repairs makes the LP64 build NON-DETERMINISTIC — two runs of one binary
# gave ktrace md5 a0d45750bf40 (K=1933) and 766866acdb8a (K=1351) where the
# 32-bit control is c31ed77b7323 (K=1396) — while the other eight files this
# pass touches are byte-identical to that control, twice, with it excluded.
#
# TWO THINGS ARE WRONG THERE AND ONLY ONE IS THIS PASS'S:
#
#   1. `(float *)((kd_iptr)pvVar21 + 0x30)` is matched as
#      `McdTriangleList::triangleListGenerator`, which is byte 48 at i386 — the
#      right BYTE and the wrong TYPE: the site dereferences it as a FLOAT and
#      the field is a function pointer. A displacement match is not a type match.
#   2. `alloca(n * (int)sizeof(*(McdUserTriangle *)0))` comes out as `n * 0x18`.
#      That is 24 — `sizeof(McdUserTriangle)` at i386 and HALF of it at LP64, so
#      the triangle list is under-allocated and the physics runs on a heap
#      overrun. ★ THIS PASS DOES NOT WRITE THAT LINE. Editing the file at all
#      makes a LATER pass, which accepts its edits ALL-OR-NOTHING PER FILE, fail
#      byte-identity on its bundle and drop the whole thing — taking its good
#      `sizeof` repair with it.
#
# So the hazard is a pipeline INTERACTION, not a bad rule, and fixing it properly
# means making that acceptance per-edit rather than per-file. Until then this one
# file is left to the passes that already handle it correctly.
#
# ⚠ Neither the i386 acceptance test (145/145, 0 byte differences) nor the -m64
# compile check can see any of this. The ktrace against the 32-bit control is
# what caught it, and it is why that run is not optional before landing.
QUARANTINE = {'IxSphereTriList.c'}


def evidence(region, rel, blks, base, disp, fnname, ifdisp={}, types=None):
    """Every geometry type id that can reach this site, and which one names the
    field. -> ((ids, first), None)  or  (None, reason).

    The sources, in the order they are believed — and each is a STATEMENT in
    the source or in a shipped binary, not an inference about it:

      0  an explicit cast of the SAME base to a concrete geometry type,
         already present in this function
      1  a type-id branch on the site's own base                    TYPEVAR
      2  a type-id branch on the INSTANCE whose geometry the base is
         (`b = McdGeometryInstanceGetGeometry(ins)` + `v = (byte)ins->
         mGeometry->mRefCtAndID`)                                   TYPEVAR_INS
      3  the file-local dispatcher that can only reach this function
         for one type                                    dispatch_ids/if_dispatch_ids
      4  (in main, last) the shipped amd64 build's own displacements  oracle_type
    """
    # (0) ★ THE FILE ALREADY SAYS SO. An earlier pass, or Ghidra itself, has
    # written `((McdTriangleList *)pMVar1)` against this very base somewhere in
    # this function — often in the SAME STATEMENT as the unrepaired half:
    #
    #   *(float *)&pMVar1[1].mRefCtAndID = ((McdTriangleList *)pMVar1)->center[0] * 0.5;
    #   ^ i386 byte 16, unrepaired          ^ the very same field, already named
    #
    # Nothing is inferred, so it is tried first. ⚠ The cast must name a GEOMETRY
    # type: a cast to anything else says nothing about which concrete geometry
    # this base is.
    if types:
        tags = {t for t in re.findall(
            r'\(\s*(\w+)\s*\*\s*\)\s*%s\b' % re.escape(base), region)
            if t in set(types.values())}
        if len(tags) == 1:
            tag = next(iter(tags))
            for tid, name in sorted(types.items()):
                if name == tag:
                    return ([tid], tid), None
    for pat in TYPEVAR:
        for t in pat.finditer(region):
            if t.group('b') == base:
                got = reaching_ids(region, rel, t.group('v'), blks)
                if got is None:
                    return None, ('%s is compared, but the site is not inside a '
                                  'resolvable branch' % t.group('v'))
                return got, None
    for pat in GEOM_OF:
        for g in pat.finditer(region):
            if g.group('b') != base:
                continue
            for t in TYPEVAR_INS.finditer(region):
                if t.group('ins') != g.group('ins'):
                    continue
                got = reaching_ids(region, rel, t.group('v'), blks)
                if got is None:
                    return None, ('%s is compared, but the site is not inside a '
                                  'resolvable branch' % t.group('v'))
                return got, None
    # ⚠ THE BANNER AND THE CALL SPELL THE SAME FUNCTION DIFFERENTLY. The region
    # header is `/* ---- McdBoxGetSlice ... */` and the dispatcher calls
    # `kd_McdBoxGetSlice` — the `kd_` prefix every recovered definition carries.
    # Matching on one spelling finds nothing and looks exactly like "there was
    # no dispatcher", which is the failure mode this whole pass is about.
    for cand in ((fnname, 'kd_' + fnname) if fnname else ()):
        if cand in disp:
            return ([disp[cand]], disp[cand]), None
    # (4) the file-local `if` dispatcher, where the instance is an ARGUMENT at a
    # known position and the site's base is the geometry OF that parameter.
    got = ifdisp.get(fnname) if fnname else None
    if got:
        _i, tid, pname = got
        if pname:
            for pat in GEOM_OF:
                for g in pat.finditer(region):
                    if g.group('b') == base and g.group('ins') == pname:
                        return ([tid], tid), None
    return None, 'no type-id evidence reaches %s' % base


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')
    cache32, cache64 = {}, {}
    tycache, offcache = {}, {}

    base_name, base_fields = BASES['_McdGeometry']
    bsz = measure('sizeof(*(McdGeometryID)0)', inc, cache32)
    boff = {f: measure('((char *)&((McdGeometryID)0)->%s - (char *)0)' % f, inc, cache32)
            for f in base_fields}
    types = enum_types(inc)

    # ---- THE SELF-CHECK. Without it "0 repaired" reads the same whether there
    # was nothing to repair or the offset map measured nothing at all — which is
    # how this class of pass fails silently.
    cm = field_paths('McdConvexMesh', inc, cache32)
    sp = field_paths('McdSphere', inc, cache32)
    bx = field_paths('McdBox', inc, cache32)
    if bsz != 16 or boff.get('frame') != 12 or cm.get(44) != 'mFatness' or sp.get(16) != 'mRadius':
        sys.exit('fix_typeid_dispatch: SELF-CHECK FAILED — sizeof(McdGeometry) %r, '
                 'offsetof(frame) %r, McdConvexMesh+44 %r, McdSphere+16 %r; want '
                 '16 / 12 / mFatness / mRadius.'
                 % (bsz, boff.get('frame'), cm.get(44), sp.get(16)))
    # ★ THE BOX IS CHECKED BY NAME AND BY BOTH WIDTHS, because it is the type
    # the baked-offset half of this pass exists for and the one whose i386
    # offsets (16/20/24) land on `McdGeometry`'s POINTERS at LP64 (32/36/40).
    # A run that could not measure that must not report "0 repaired".
    if bx.get(16) != 'mR[0]' or bx.get(24) != 'mR[2]' or \
            offset_at('McdBox', 'mR[0]', inc, 64, offcache) != 32:
        sys.exit('fix_typeid_dispatch: SELF-CHECK FAILED — McdBox+16 %r, +24 %r, '
                 'LP64 offsetof(mR[0]) %r; want mR[0] / mR[2] / 32.'
                 % (bx.get(16), bx.get(24),
                    offset_at('McdBox', 'mR[0]', inc, 64, offcache)))
    if types.get(1) != 'McdSphere' or types.get(7) != 'McdConvexMesh':
        sys.exit('fix_typeid_dispatch: SELF-CHECK FAILED — type id 1 is %r and 7 is %r, '
                 'want McdSphere and McdConvexMesh. The enum is not being read.'
                 % (types.get(1), types.get(7)))

    SITE = re.compile(r'(?P<base>[A-Za-z_]\w*)\[(?P<k>[1-9]\d*)\]\s*\.\s*'
                      r'(?P<f>' + '|'.join(base_fields) + r')\b')

    fixed = declined = 0
    notes, declines, oracle_used = [], [], []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        if not SITE.search(text) and not OFFSITE.search(text):
            continue
        if fn in QUARANTINE:
            continue
        disp = dispatch_ids(text)
        ifdisp = if_dispatch_ids(text)

        edits = []
        for m in SITE.finditer(text):
            base = m.group('base')
            if already_repaired(text, m.start()):
                continue
            # ⚠ `X->list[1].next` IS NOT A GEOMETRY WALK. The site pattern sees a
            # bare identifier followed by `[k].FIELD`, which `list` satisfies —
            # but it is a MEMBER of an already-typed object, and
            # `((McdTriangleList *)p)->list[1]` gets its stride from the compiler
            # at both widths. Three of those were the whole decline list.
            if re.search(r'(?:->|\.)\s*$', text[max(0, m.start() - 3):m.start()]):
                continue
            rs, re_ = region_bounds(text, m.start())
            region = text[rs:re_]
            rel = m.start() - rs
            blks = blocks(region)
            fnname = region_fn(text, rs)

            got, why = evidence(region, rel, blks, base, disp, fnname, ifdisp, types)
            if got is None:
                # (5) the shipped 64-bit build, for the BASE[k].FIELD shape too:
                # gather the i386 bytes this base is read at and ask which
                # geometry layout the amd64 binary reaches for.
                offs = {int(s2.group('k')) * bsz + boff[s2.group('f')]
                        for s2 in SITE.finditer(region) if s2.group('base') == base}
                won = oracle_type(fnname, offs, types, inc, cache32, offcache)
                if not won:
                    declined += 1
                    declines.append('%-26s +%-4d %s' % (fn, m.start(), why))
                    continue
                ids, first = [won[0]], won[0]
                oracle_used.append('%-26s %-16s %s  <- shipped amd64 build'
                                   % (fn, won[1], sorted(offs)))
            else:
                ids, first = got

            off = int(m.group('k')) * bsz + boff[m.group('f')]
            # ⚠ EVERY REACHING TYPE MUST AGREE ON THE FIELD — its NAME, its C
            # TYPE and its LP64 OFFSET. Agreement is what lets one cast stand
            # for all of them; disagreement means the site genuinely needs a
            # branch and this pass has no business rewriting it. Sphere and
            # Sphyl both put `mRadius` at 16 and at 32, so the shared site in
            # McdGjkFatness resolves; had they not, it would decline.
            cands, ok = [], True
            for tid in ids:
                tag = types.get(tid)
                fp = field_paths(tag, inc, cache32).get(off) if tag else None
                off64 = offset_at(tag, fp, inc, 64, offcache) if fp else None
                if not tag or not fp or off64 is None:
                    ok = False
                    break
                cands.append((tag, fp, off64))
            if ok and len(cands) > 1:
                t0, f0, o0 = cands[0]
                for tag, fp, off64 in cands[1:]:
                    if fp != f0 or off64 != o0 or not type_compatible(
                            t0, f0, tag, fp, inc, tycache):
                        ok = False
                        break
            if not ok:
                declined += 1
                declines.append('%-26s +%-4d ids %s do not agree on the field at +%d %s'
                                % (fn, m.start(), ids, off,
                                   sorted(set(c[1:] for c in cands)) if cands else '(unresolved)'))
                continue

            fp = cands[0][1]
            tag = types[first]
            # The three spellings are OFFERED TO THE COMPILER rather than chosen
            # by reasoning, and the i386 object decides. They exist because the
            # site's context varies:
            #   a  `*(float *)&(SITE)` — naming the float field is the whole fix
            #   b  the value is wanted as BITS in an integer context
            #   c  ★ the value is wanted as BITS in a POINTER-typed local. This
            #      is the shape inside `McdGjkMaximumPoint` itself, where the
            #      inlined fatness lands in an `McdFrameworkID` and every later
            #      use reads it back through `*(float *)&`. (a) will not compile
            #      there — C has no float-to-pointer conversion — and (b) goes
            #      through `KD_FBITS`, whose parameter is a `double`, so the
            #      round trip is not the plain four-byte load the original was
            #      and the i386 object differs. Reading the field's storage as a
            #      32-bit word IS that load, and at LP64 it zero-extends into
            #      the low half of the pointer, which is exactly where
            #      `*(float *)&` looks on little-endian.
            # ⚠⚠ (b) AND (c) ARE FOUR-BYTE SPELLINGS AND MUST NOT TOUCH AN
            # EIGHT-BYTE FIELD. `*(unsigned int *)&x->elementTable` is
            # byte-identical at i386 — where a pointer IS four bytes — and
            # TRUNCATES it at LP64. That is invisible to the acceptance gate and
            # to the -m64 compile check, and it crashed `McdAggregateCreate` the
            # first time a new evidence source made this shape reachable on a
            # pointer field. So they are offered only for a field that measures
            # four bytes at BOTH widths, which is the same check `count-to-id`
            # makes and for the same reason.
            fsz32 = field_size(tag, fp, inc, 32, offcache)
            fsz64 = field_size(tag, fp, inc, 64, offcache)
            reps = ['((%s *)%s)->%s' % (tag, base, fp)]
            if fsz32 == 4 and fsz64 == 4:
                reps += ['KD_FBITS(((%s *)%s)->%s)' % (tag, base, fp),
                         '(*(unsigned int *)&((%s *)%s)->%s)' % (tag, base, fp)]
            edits.append((m.start(), m.end(), reps,
                          '%-26s %-16s [%s].%-13s +%-3d ids %-9s -> %s'
                          % (fn, tag, m.group('k'), m.group('f'), off,
                             ','.join(str(i) for i in ids), fp)))

        # ---- THE BAKED-OFFSET SITES, with the same evidence and the same
        # backstop. ★ THE REPAIR IS AN ADDRESS RE-SPELLING, NOT A MEMBER READ:
        # `fix_callback_context` measured that naming two members of one object
        # lets gcc schedule the loads differently and the i386 object stops
        # matching, while `&p->FIELD` keeps every expression's type exactly as
        # it was and changes only how the address is computed. The cast outside
        # the match is untouched for the same reason.
        for m in OFFSITE.finditer(text):
            base = m.group('base')
            if already_repaired(text, m.start()):
                continue
            rs, re_ = region_bounds(text, m.start())
            region = text[rs:re_]
            rel = m.start() - rs
            blks = blocks(region)
            fnname = region_fn(text, rs)

            got, why = evidence(region, rel, blks, base, disp, fnname, ifdisp, types)
            if got is None:
                # (5) LAST RESORT: the shipped 64-bit build. Gather every i386
                # offset used against this base in this function and ask which
                # geometry type's LP64 layout the amd64 binary actually reaches
                # for. This is what closes a site with no caller and no branch.
                offs = {int(o.group('off'), 0) for o in OFFSITE.finditer(region)
                        if o.group('base') == base and int(o.group('off'), 0) >= bsz}
                won = oracle_type(fnname, offs, types, inc, cache32, offcache)
                if won:
                    ids, first = [won[0]], won[0]
                    oracle_used.append('%-26s %-16s %s  <- shipped amd64 build'
                                       % (fn, won[1], sorted(offs)))
                else:
                    # ⚠ SILENCE HERE WOULD BE THE BUG. OFFSITE matches baked
                    # offsets against every kind of base in the corpus, so
                    # reporting all of them is noise — but a base that IS a
                    # geometry and still has no type evidence is a site this
                    # pass looked at and could not repair, and it has to say so.
                    if any(g.group('b') == base
                           for pat in GEOM_OF for g in pat.finditer(region)):
                        declined += 1
                        declines.append('%-26s +%-4d geometry base %s: %s'
                                        % (fn, m.start(), base, why))
                    continue
            else:
                ids, first = got

            off = int(m.group('off'), 0)
            # ⚠ ONLY THE CONCRETE TYPE'S OWN FIELDS. An offset inside
            # `McdGeometry` itself is the base class and does not move relative
            # to the base, so rewriting it would be noise at best.
            if off < bsz:
                continue
            cands, ok = [], True
            for tid in ids:
                tag = types.get(tid)
                fp = field_paths(tag, inc, cache32).get(off) if tag else None
                off64 = offset_at(tag, fp, inc, 64, offcache) if fp else None
                if not tag or not fp or off64 is None:
                    ok = False
                    break
                cands.append((tag, fp, off64))
            if ok and len(cands) > 1:
                t0, f0, o0 = cands[0]
                for tag, fp, off64 in cands[1:]:
                    if fp != f0 or off64 != o0 or not type_compatible(
                            t0, f0, tag, fp, inc, tycache):
                        ok = False
                        break
            if not ok:
                declined += 1
                declines.append('%-26s +%-4d ids %s do not agree on the field at +%d'
                                % (fn, m.start(), ids, off))
                continue

            fp, tag = cands[0][1], types[first]
            # ★ THE THIRD SPELLING IS THE ONE THE REST OF THE PIPELINE CAN READ,
            # AND IT IS WHY McdBatch's ELEMENT WALK WAS STUCK.
            #
            # `&((T *)b)->F` is the tightest repair and it is tried first, but on
            # a POINTER field it fails i386 byte-identity: naming the member
            # changes the expression's type, gcc schedules the load differently
            # and the object stops matching. Both existing spellings then fail
            # and the site declines as "a width change, not an offset change" —
            # which is true, and leaves it unrepaired because nothing downstream
            # can see a raw `0x10` either.
            #
            # `((int)b + ((int)((char *)&((T *)0)->F - (char *)0)))` is
            # `fix_literal_offsets`' own output spelling. It is byte-identical
            # BY CONSTRUCTION — the offsetof folds to the literal it replaces and
            # nothing's type changes — and it hands the site to the passes that
            # key on that idiom. Measured on `McdBatchFlattenAggregate`, where
            # this one edit cascades into the whole walk:
            #
            #   ours before   *(int *)(*(int *)((int)pvVar8 + 0x10) + 0x40 + local_70)
            #                 local_70 = local_70 + 0x44
            #   ours after    *(kd_iptr *)(*(kd_iptr *)((kd_iptr)pvVar8 + OFF(McdAggregate,
            #                   elementTable)) + OFF(McdAggregateElement, mGeometry) + local_70)
            #                 local_70 = local_70 + (int)sizeof(McdAggregateElement)
            #   amd64 build   mov 0x20(%rcx),%rcx          elementTable, EIGHT
            #                 cmpq $0x0,0x40(%rcx,%rax,1)  mGeometry,    EIGHT
            #                 imul $0x48,%rax,%rax         stride 72, not 68
            #
            # ⚠ It is LAST deliberately. It re-spells the ADDRESS and leaves the
            # narrow load in place, so on a field that does NOT grow it would
            # take a site away from the two tighter spellings above for no gain.
            reps = ['(&((%s *)%s)->%s)' % (tag, base, fp),
                    '((kd_iptr)&((%s *)%s)->%s)' % (tag, base, fp),
                    '((int)%s + ((int)((char *)&((%s *)0)->%s - (char *)0)))'
                    % (base, tag, fp)]
            edits.append((m.start(), m.end(), reps,
                          '%-26s %-16s +%-3d %-18s ids %-9s -> %s'
                          % (fn, tag, off, '(baked offset)',
                             ','.join(str(i) for i in ids), fp)))

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
                # ⚠⚠ NO `#if __SIZEOF_POINTER__` FALLBACK HERE, DELIBERATELY.
                # `fix_setter_typed_slot` uses one on a single hand-checked site
                # and it is validated by ktrace; applied WHOLESALE by this pass
                # it failed twice in one session. It is LINE-based and C is not,
                # so a statement spanning lines came out with an EMPTY i386
                # branch — and because the guard puts the original text on the
                # i386 side, byte-identity passes BY CONSTRUCTION and neither
                # that gate nor the -m64 compile check can see any of it. A site
                # that needs its LOAD WIDTH changed is `fix_narrow_loads`' work;
                # here it declines and says so.
                declined += 1
                declines.append('%s  — no spelling reproduced the i386 object '
                                '(a width change, not an offset change)' % note)
        if fixed > n0:
            open(path, 'w').write(text)

    print('fix_typeid_dispatch:')
    print('  repaired (i386 byte-identical)                : %d' % fixed)
    for n in notes:
        print('     ' + n)
    print('  of those, typed by the shipped amd64 build    : %d' % len(oracle_used))
    for o in oracle_used:
        print('     ' + o)
    print('  declined (reported, not guessed)              : %d' % declined)
    for d in declines:
        print('     - ' + d)
    return 0


if __name__ == '__main__':
    sys.exit(main())
