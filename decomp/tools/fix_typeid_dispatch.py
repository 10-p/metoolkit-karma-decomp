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
    if bsz != 16 or boff.get('frame') != 12 or cm.get(44) != 'mFatness' or sp.get(16) != 'mRadius':
        sys.exit('fix_typeid_dispatch: SELF-CHECK FAILED — sizeof(McdGeometry) %r, '
                 'offsetof(frame) %r, McdConvexMesh+44 %r, McdSphere+16 %r; want '
                 '16 / 12 / mFatness / mRadius.'
                 % (bsz, boff.get('frame'), cm.get(44), sp.get(16)))
    if types.get(1) != 'McdSphere' or types.get(7) != 'McdConvexMesh':
        sys.exit('fix_typeid_dispatch: SELF-CHECK FAILED — type id 1 is %r and 7 is %r, '
                 'want McdSphere and McdConvexMesh. The enum is not being read.'
                 % (types.get(1), types.get(7)))

    SITE = re.compile(r'(?P<base>[A-Za-z_]\w*)\[(?P<k>[1-9]\d*)\]\s*\.\s*'
                      r'(?P<f>' + '|'.join(base_fields) + r')\b')

    fixed = declined = 0
    notes, declines = [], []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        if not SITE.search(text):
            continue

        edits = []
        for m in SITE.finditer(text):
            base = m.group('base')
            rs, re_ = region_bounds(text, m.start())
            region = text[rs:re_]
            rel = m.start() - rs
            blks = blocks(region)

            tyvar = None
            for pat in TYPEVAR:
                for t in pat.finditer(region):
                    if t.group('b') == base:
                        tyvar = t.group('v')
                        break
                if tyvar:
                    break
            if tyvar is None:
                declined += 1
                declines.append('%-26s +%-4d no type-id variable derived from %s'
                                % (fn, m.start(), base))
                continue

            got = reaching_ids(region, rel, tyvar, blks)
            if got is None:
                declined += 1
                declines.append('%-26s +%-4d %s is compared, but the site is not inside a '
                                'resolvable branch' % (fn, m.start(), tyvar))
                continue
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
            reps = ['((%s *)%s)->%s' % (tag, base, fp),
                    'KD_FBITS(((%s *)%s)->%s)' % (tag, base, fp),
                    '(*(unsigned int *)&((%s *)%s)->%s)' % (tag, base, fp)]
            edits.append((m.start(), m.end(), reps,
                          '%-26s %-16s [%s].%-13s +%-3d ids %-9s -> %s'
                          % (fn, tag, m.group('k'), m.group('f'), off,
                             ','.join(str(i) for i in ids), fp)))

        n0 = fixed
        for start, end, reps, note in sorted(edits, key=lambda e: -e[0]):
            for rep in reps:
                cand = text[:start] + rep + text[end:]
                if compiles_identically(fn, cand, build, inc):
                    text = cand
                    fixed += 1
                    notes.append(note)
                    break
            else:
                declined += 1
                declines.append('%s  — no spelling reproduced the i386 object' % note)
        if fixed > n0:
            open(path, 'w').write(text)

    print('fix_typeid_dispatch:')
    print('  repaired (i386 byte-identical)                : %d' % fixed)
    print('  declined (reported, not guessed)              : %d' % declined)
    for n in notes:
        print('     ' + n)
    for d in declines:
        print('     - ' + d)
    return 0


if __name__ == '__main__':
    sys.exit(main())
