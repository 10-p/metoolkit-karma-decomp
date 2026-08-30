#!/usr/bin/env python3
"""fix_index_layout.py — the residue `fix_derived_fields.py` cannot type.

    fix_index_layout.py <kd_out/allobj> <kd_build> [metoolkit-root]

★ READ `fix_derived_fields.py` FIRST. It is the same defect class and it runs
BEFORE this: a derived struct's field addressed as an index past its base, 186
of 281 sites repaired, typed PER FILE and rewritten by NAMING the concrete field
so its own declared type carries the store. That is the better repair and it is
not duplicated here.

This one exists for the sentence in that pass's README: **"44 where the concrete
type is genuinely ambiguous — an `Ix*` function handles TWO geometries, so
per-file inference cannot work and those need per-site typing"**. Those 44 are
where the defect `proven.txt` calls LP64-INDEX-LAYOUT actually lives.

THE DEFECT, and it is the one `layout_check.py`'s own header names.

    IxBoxTriList.c:140   pcVar4 = (code *)trilistgeom[3].mRefCtAndID;
                         count  = (*(int (*)(...))pcVar4)(...);

`trilistgeom` is an `McdGeometry *`. The object behind it is an
`McdTriangleList`, and byte 48 of that object is the triangle-generator
CALLBACK. The shipped binary read `[base + 48]`; Ghidra rendered that as an
index through the type IT had for the pointer:

    sizeof(struct _McdGeometry)   i386 16    LP64 32
    trilistgeom[3] starts at      i386 48    LP64 96

So at 64-bit pointer width the call target is read from byte 96 — past the end
of an 88-byte object — and what comes back is whatever the allocator left there.
That is the address dependence `proven.txt` LP64-ADDRESS-DEPENDENT measured as
eight runs of one binary giving five answers, and it is absent at -m32 because
at -m32 the arithmetic is right. `IxBoxTriList` is a box against a triangle
list, `IxSphereTriList` a sphere and `IxSphylPrimitives` a capsule: two
geometries per file, which is exactly why the per-file inference declines them
and exactly the path `scene_ragdoll` — capsules on a triangle-list floor — runs
down every step.

NOTHING TRUNCATES AND NOTHING WARNS. The cast is width-correct, the index is a
legal C array subscript, `-Wint-to-pointer-cast` has nothing to say and
`ptrwidth_check.sh` counts zero. This is the second arm64 defect, the one that
is invisible to the first one's gate by construction.

WHAT THIS ADDS to the per-file pass, and it is three things:

  * the pointer is typed PER VARIABLE, from where it came from rather than from
    what else is in the file — see WHICH TYPE C IS below;
  * a FRAME test, because knowing the object is a `C` is not knowing the pointer
    points at its start — see THE FRAME, which is where a first version of this
    was wrong;
  * and it MEASURES whether the index is wrong before touching it. Half the
    remaining sites are already right at LP64 and rewriting them would be churn
    with a byte-identity risk and no defect behind it.

★ WHAT MAKES IT SAFE IS THE FILTER, NOT THE REWRITE. 1,211 sites in the corpus
have the shape `NAME[k].field` and MOST OF THEM ARE CORRECT C:

    handlers[1].name        `MeXMLHandler handlers [6];` — a real local array
    limit->limit[1].stop    a real member array
    pMVar26[1].separation   a real walk over an McdContact array
    row[1].v                an lsVec3, three floats, the same twelve bytes

Rewriting any of those to a baked offset BREAKS a target that works today. So a
site is only touched when all of the following are true, each of them measured:

  1. the base is a plain pointer VARIABLE — not `x->arr[k]`, not an array local
     (an array declaration does not match `declared_type`, which is why the 92
     `lsVec3 vector [4]` sites drop out here rather than being special-cased);
  2. its declared type resolves to a tag T whose size CHANGES at LP64;
  3. some other tag C has T as its FIRST MEMBER — the pointer is to a base class
     and the object is a derived one. An honest array of `McdContact` is the
     first member of nothing, and all 43 genuine array walks fall out here;
  4. the pointer provably points at the START of such an object — see THE FRAME
     below, which is the rule this tool got wrong first and which is the whole
     difference between a repair and a new defect;
  5. and the naive index is ACTUALLY WRONG at LP64. See below: half are not.

★★ HALF THE REMAINDER NEED NO REPAIR, WHICH IS WHY THIS COMPARES RATHER THAN
REWRITES. `CxSmallSort`'s `Link *` sites index a base class exactly like the
geometry ones, and they are RIGHT at 64-bit:

    struct Link { Link *mNext, *mPrev; }          i386  8   LP64 16
    struct CxSmallSortMarker { Link super_Link; MeReal mOrdinate;
                               CxSmallSortRep *mRep; MeU8 mType; }
                             i386 0,4,8,12,16     LP64 0,8,16,24,32   — 2x

Every field offset and the size both double, so `pLVar2[1].mPrev` lands on
`mRep` at both widths. The geometry family does not scale that way, because
`MeReal center[3]` does not grow: `McdTriangleList` is 0,16,28,40,44,48,52 here
and 0,32,44,56,64,72,80 there. So this MEASURES BOTH — what the index computes
at LP64, and where the field really is at LP64 — and only rewrites where they
disagree.

★★★ THE FRAME, AND THIS IS WHERE THE FIRST VERSION OF THIS TOOL WAS WRONG.
Knowing the object is a `C` is not enough; the pointer has to point at its
START. `CxSmallSort.c` is full of pointers that do not:

    pLVar2  = &pCVar1->mAABBMarkers[2].super_Link;   /* INTO an array */
    pLVar12 = (inMarker->super_Link).mNext;          /* a list traversal */
    pLVar12 = pLVar18[1].mPrev;                      /* ...and then a REP */

Every offset used against `pLVar2` lands on a real field of `CxSmallSortRep`, so
an all-offsets-land test says "it is a Rep" and it is not — it is the third
marker of one. The rewrite that follows is byte-identical at i386, so the
acceptance test cannot see it either, and `pLVar2[3].mPrev` would have moved
from a right answer to a wrong one. Ghidra also REUSES one variable for two
frames in the same function, which no per-variable answer can be right about.

So the frame is required to come from a source that means "a whole object":

    a PARAMETER of the base type, never reassigned    the API's own contract
    v = SomeCall(...)                                 with a declared return type
    v = (MeMemoryAPI.createAligned)(sizeof(*(C *)0))  the allocation names C
    v = otherVariable                                 an alias, one hop
    v = x->field                                      only if `field` is NOT a
                                                      member of T itself — a
                                                      `Link *` read out of a
                                                      `Link` is a traversal

and everything else — an address-of, pointer arithmetic, a masking trick —
declines the variable outright. That is what excludes CxSmallSort's 52 sites,
and it excludes them for a stated reason rather than by name.

⚠ SO CxSmallSort's REPS ARE NOT REPAIRED HERE AND SOME OF THEM ARE WRONG.
`CxSmallSortRep::mCullingIndex` is at 28 here and 52 there while the index says
56 — the scaling breaks at the second of two adjacent 4-byte fields. Those sites
need per-ASSIGNMENT typing, not per-variable, and this tool does not have it.
Reported, not quietly skipped.

THE REPAIR is the address re-spelling `fix_literal_offsets.py` uses, which is
what makes it a no-op at i386 BY CONSTRUCTION:

    NAME[k].field   ->   *(A *)((char *)NAME + ((int)((char *)&((C *)0)->P - (char *)0)))

with P the field of C at byte `k*sizeof_i386(T) + offsetof_i386(T,field)`. The
number becomes an expression the compiler recomputes per target; at i386 it
folds back to the constant it replaced and the object must come out
byte-identical, which is verified per site rather than asserted.

★ THE ACCESS TYPE IS CHOSEN BY THE CONCRETE FIELD'S WIDTH, and getting it wrong
writes eight bytes into a float:

    trilistgeom[3].mRefCtAndID   MeU32          the generator, a POINTER  -> kd_uptr
    pMVar1[1].prev               McdGeometryID  center[1], a MeReal       -> MeU32
    trilistgeom[3].prev          McdGeometryID  list, a POINTER           -> unchanged

Keeping `McdGeometryID` on the second stores eight bytes into a four-byte float
at LP64 and takes `center[2]` with it. Keeping `MeU32` on the first reads half a
function pointer — `fix_narrow_loads.py`'s defect, repaired here at the same
time because the width is known here and is not knowable there. The declared
type is KEPT whenever its width already matches, because a pointer type is also
what lets `&(trilistgeom[3].prev)->mRefCtAndID` go on compiling.

WHICH TYPE C IS, in order, and never guessed:

  1. the allocation in the same function — `sizeof(*(McdBox *)0)` names it;
  2. the declared return type of the call the pointer came from, when that
     resolves to a derived type rather than to the base;
  3. what `fix_literal_offsets.py` already decided for this same variable — its
     output spells the tag in the source (`&((struct McdBox *)0)->mR`);
  4. the geometry the FILE REGISTERS — `McdBox.c` calls
     McdFrameworkRegisterGeometryType(frame, 2, "McdBox", ...) with its own
     functions, so every geometry pointer in it is an McdBox;
  5. otherwise the one derived tag on which EVERY offset used against that
     variable lands on a real field, asked narrowest tier first.
     `IxBoxTriList`'s `trilistgeom` uses 40, 48 and 52 and only McdTriangleList
     has members starting at all three.

Every one of them is then gated by that same all-offsets-land test, and a
variable no rule answers is reported and left alone.

⚠ AND EVERY OFFSET IS CONFIRMED AGAINST THE SHIPPED 64-BIT BUILD, not against
this file's arithmetic. `tools/amd64_oracle.py` reads MathEngine's own amd64
archives; `McdTriangleListSetGenerator` writes displacement 0x48, which is 72,
which is where this pass puts the generator. That oracle is LLP64 (`long` is
four bytes there), so a tag whose Windows and Linux layouts differ is reported
unconfirmable rather than confirmed against the wrong number — measured with
mingw, not assumed.

Run it on a COPY: it edits in place. After `fix_literal_offsets.py`, whose
answers it reads, and before `fix_ptrwidth.py`, which rewrites the casts these
sites sit in.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402
import fix_literal_offsets as flo                           # noqa: E402
import interaction_types                                    # noqa: E402

HERE = kd_paths.MD
WORK = '/tmp/kd_idxlayout'

# The four data models this has to be right on. i386 is what the recovery
# encodes and what byte-identity is measured at; lp64 is Linux, both 64-bit
# Android ABIs and this machine; llp64 is the shipped amd64 archive's model and
# is only ever used to ask whether the ORACLE can speak for a type.
CCS = {
    'i386': ['gcc', '-m32'],
    'lp64': ['gcc', '-m64'],
    'llp64': ['x86_64-w64-mingw32-gcc'],
}
NDK = kd_paths.NDK_BIN
ARM64 = 'aarch64-linux-android21-clang'

# ⚠ THE PROBE IS `char (*)[1] = &array`, NOT `int = &array`, AND THAT IS NOT A
# STYLE CHOICE. The `int` form is what the sibling tools use and it works on
# gcc/Linux — but mingw rejects it before it ever types the expression
# ("initializer element is not computable at load time"), so every win64 answer
# came back None and every offset read as "unconfirmable". A pointer initialiser
# is diagnosed by all four compilers.
#
# ⚠ AND THE SIZE IS THE **LAST** `char (*)[N]` ON THE LINE. Both spellings name
# the DESTINATION type first — clang says "initializing 'char (*)[1]' with an
# expression of type 'char (*)[25]'" — so a non-greedy match reads 1 for
# everything, which is a number, not an error, and it would have made every
# struct the same size.
# ⚠ AND THE PROBE ADDS **2**, NOT 1, WHICH LOOKS LIKE A TYPO AND IS NOT. The
# destination is `char (*)[1]`, so an expression whose answer is 0 — every
# `offsetof` of a FIRST member, `mRefCtAndID` among them — produces `char (*)[1]`
# on both sides, the types are COMPATIBLE, and no diagnostic is emitted at all.
# The offset then reads as unmeasurable and the site is silently dropped: 14
# McdTriangleList repairs, including the triangle generator itself, disappeared
# from a run that reported no error.
_BLINE = re.compile(r'probe\.c:(\d+):\d+: (?:error|warning):')
_BSZ = re.compile(r'char \(\*\)\[(\d+)\]')

# `NAME[k].field`, k non-zero. `pre` is what precedes the name: a `.` or a `>`
# means the base is a MEMBER (`this->mMinBound[1].mRep`), which is a real array
# in a real struct and is not ours to touch.
INDEXED = re.compile(r'(?P<pre>[.>]?)\b(?P<v>[A-Za-z_]\w*)\s*\[\s*(?P<k>[1-9]\d*)\s*\]'
                     r'\s*\.\s*(?P<f>[A-Za-z_]\w*)\b')

# `v = <rhs>;` at the start of a statement. Ghidra writes assignments that way;
# an initialiser inside a `for` is not matched and the variable then has no
# recognised frame, which declines it rather than admitting it.
ASSIGN = re.compile(r'(?m)^[ \t]*(?P<v>[A-Za-z_]\w*)\s*=\s*(?P<rhs>[^;]{0,400});')

# The shapes a frame may come from — see THE FRAME in the header.
CALL_RHS = re.compile(r'^(?:\([^()]*\)\s*)*\(?(?P<fn>[A-Za-z_][\w.]*)\)?\s*\(')
ALIAS_RHS = re.compile(r'^(?:\([^()]*\)\s*)?(?P<v>[A-Za-z_]\w*)$')
NULL_RHS = re.compile(r'^(?:\([^()]*\)\s*)?0(?:x0)?$')
FIELD_RHS = re.compile(r'^\(?[A-Za-z_][\w]*(?:\s*->\s*[A-Za-z_]\w*)*\)?'
                       r'\s*(?:->|\.)\s*(?P<f>[A-Za-z_]\w*)$')
SIZEOF_OF = re.compile(r'sizeof\(\s*\*\(\s*(?P<tag>\w+)\s*\*\s*\)\s*0\s*\)')

# The tag `fix_literal_offsets` wrote into the source for a pointer it typed.
NAMED_TAG = re.compile(r'\(\s*(?:char|int|kd_iptr)\s*\*?\)\s*(?P<v>[A-Za-z_]\w*)'
                       r'\s*\+\s*[^;]{0,200}?\(\(char \*\)&\(\((?:struct )?'
                       r'(?P<tag>\w+) \*\)0\)->')


# ---------------------------------------------------------------------------
# Layout measurement: ask the compiler, one process per data model.
# ---------------------------------------------------------------------------
_MEAS = {}


def measure(model, exprs, inc):
    """Compile-time constants for many expressions under one data model.

    ⚠ THE FAILURES ARE THE SIGNAL, not an accident tolerated. An expression with
    no answer is recorded as None and every caller declines on it; nothing here
    invents a number, which is what the self-check in `main` proves."""
    cache = _MEAS.setdefault(model, {})
    todo = sorted({e for e in exprs if e not in cache})
    if not todo:
        return cache
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'probe.c')
    head = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
            '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))
    line_of, n = {}, head.count('\n')
    with open(src, 'w') as f:
        f.write(head)
        for i, e in enumerate(todo):
            f.write('char kd_p%d[%s];\nchar (*kd_f%d)[1] = &kd_p%d;\n' % (i, e, i, i))
            n += 2
            line_of[n] = e
    cc = CCS[model] if model in CCS else [os.path.join(NDK, model)]
    limit = ['-ferror-limit=0'] if 'clang' in cc[0] else ['-fmax-errors=0']
    r = subprocess.run(cc + ['-DLINUX'] + limit
                       + ['-I' + os.path.join(HERE, 'include')]
                       + flo.includes(inc) + ['-c', '-o', os.devnull, src],
                       capture_output=True, text=True)
    for e in todo:
        cache[e] = None
    for line in r.stderr.split('\n'):
        m = _BLINE.search(line)
        if not m:
            continue
        sizes = _BSZ.findall(line)
        e = line_of.get(int(m.group(1)))
        if e is not None and sizes:
            cache[e] = int(sizes[-1]) - 2
    return cache


def sizeof_expr(tag):
    return 'sizeof(%s%s) + 2' % (flo.kw_of(tag), tag)


def offset_expr(tag, path):
    """offsetof, spelled so the probe can read it AND the source can use it.

    ⚠ This also has to answer for `McdGeometryID prev,next;` — ONE declaration
    with TWO declarators, which the member regex every tool here shares does not
    match, so `prev` and `next` are absent from `offsets_of`'s map. Asking the
    compiler directly is the only spelling that covers both."""
    return '((char *)&((%s%s *)0)->%s - (char *)0) + 2' % (flo.kw_of(tag), tag, path)


def fieldsize_expr(tag, path):
    return 'sizeof(((%s%s *)0)->%s) + 2' % (flo.kw_of(tag), tag, path)


# ---------------------------------------------------------------------------
_DECLS = {}


def member_type(tag, field, inc):
    """The declared type of a member, comma-separated declarators included.

    `struct Link *mNext, *mPrev;` gives `struct Link *` for either name, and
    `McdGeometryID prev,next;` gives `McdGeometryID`. This is needed because the
    ACCESS TYPE keeps the original spelling whenever its width already matches,
    and a pointer spelling is what lets `&(v[3].prev)->mRefCtAndID` keep
    compiling."""
    if tag not in _DECLS:
        d = {}
        body = flo.struct_bodies(inc).get(tag, '')
        body = re.sub(r'/\*.*?\*/', ' ', body, flags=re.S)
        for stmt in body.split(';'):
            stmt = ' '.join(stmt.split())
            m = re.match(r'^((?:const |struct |unsigned )*[A-Za-z_]\w*)\s+(.*)$', stmt)
            if not m:
                continue
            base, rest = m.group(1), m.group(2)
            for part in rest.split(','):
                part = part.strip()
                stars = '*' * part.count('*')
                nm = re.sub(r'[^A-Za-z_0-9]', ' ', part).split()
                nm = [x for x in nm if not x.isdigit()]
                if nm:
                    d[nm[0]] = (base + ' ' + stars).strip()
        _DECLS[tag] = d
    return _DECLS[tag].get(field)


def derived_map(inc):
    """base tag -> tags whose FIRST member has that tag's type.

    This is the C spelling of single inheritance and the only reason a pointer
    can legally be indexed into somebody else's fields. A tag nothing derives
    from is an ordinary struct and its arrays are ordinary arrays."""
    if getattr(derived_map, '_c', None):
        return derived_map._c
    bodies = flo.struct_bodies(inc)
    out = {}
    for c, body in bodies.items():
        m = flo.MEMBER.search(body)
        if not m:
            continue
        ty = re.sub(r'\s+', ' ', m.group('ty')).strip()
        if '*' in ty:
            continue                      # a pointer first member is not a base
        base = flo.tag_of(ty, inc)
        if base and base != c:
            out.setdefault(base, []).append(c)
    derived_map._c = out
    return out


# ---------------------------------------------------------------------------
def frames(region, group, tag, inc):
    """What this pointer GROUP points at: (ok, named tag, note).

    See THE FRAME in the header — this is the guard that separates a repair from
    a new defect, so it admits shapes rather than rejecting them: anything not
    recognised declines the variable.

    ⚠ IT ASKS ABOUT A GROUP, NOT A VARIABLE, because Ghidra spells one pointer
    four ways. `IxSphylPrimitives` has `pMVar9 = McdModelGetGeometry(...)` and
    then `pMVar9 = trilistGeom` and `trilistGeom = pMVar9` — a mutual alias, and
    a per-variable walk either recurses forever or bottoms out and reports "not
    an object handle" on a pointer whose origin is three lines up. That decline
    cost the sphyl-against-trianglelist path, which is the one the RAGDOLL
    SCENE runs on, so it was the difference between repairing the located defect
    and reporting it."""
    asg = [m for m in ASSIGN.finditer(region) if m.group('v') in group]
    if not asg:
        return True, None, 'a parameter, never reassigned'
    named, notes = set(), []
    for m in asg:
        rhs = ' '.join(m.group('rhs').split())
        a = ALIAS_RHS.match(rhs)
        if a and a.group('v') in group:
            continue                      # inside the group: names the same object
        s = SIZEOF_OF.search(rhs)
        if s and CALL_RHS.match(rhs):
            named.add(s.group('tag'))
            notes.append('allocated as sizeof(%s)' % s.group('tag'))
            continue
        if NULL_RHS.match(rhs):
            continue
        c = CALL_RHS.match(rhs)
        if c:
            rt = flo.header_returns(inc).get(re.sub(r'^kd_', '', c.group('fn')))
            t = flo.tag_of(rt, inc) if rt else None
            if t == tag:
                notes.append('from %s(), which returns %s' % (c.group('fn'), tag))
                continue
            if t in derived_map(inc).get(tag, []):
                named.add(t)
                notes.append('from %s(), which returns %s' % (c.group('fn'), t))
                continue
            return False, None, 'assigned from %s(), whose return type is %r' % (
                c.group('fn'), rt)
        f = FIELD_RHS.match(rhs)
        if f:
            # ★ A `Link *` READ OUT OF A `Link` IS A TRAVERSAL, not a handle.
            # `ins->mGeometry` points at a whole geometry; `x->mNext` points at
            # the node inside the next object. The field being a member of T
            # itself is what tells them apart, and it is why CxSmallSort's
            # walks decline here.
            if member_type(tag, f.group('f'), inc):
                return False, None, ('assigned from ->%s, a member of %s itself '
                                     '— a traversal, not an object'
                                     % (f.group('f'), tag))
            notes.append('read from ->%s' % f.group('f'))
            continue
        return False, None, 'assigned %r, which is not an object handle' % rhs[:48]
    if len(named) > 1:
        return False, None, 'assigned as two types (%s)' % ', '.join(sorted(named))
    return True, (named.pop() if named else None), '; '.join(notes[:2]) or 'a handle'


def alias_groups(region, names, tag, inc):
    """Variables that name the same object, by `v = w;` in either direction.

    ⚠ THE SEED IS EVERY POINTER OF THE BASE TYPE IN THE FUNCTION, not only the
    ones with an index site in them. `IxSphylPrimitives` writes
    `pMVar9 = trilistGeom;` where `trilistGeom` is never indexed itself — so a
    seed of indexed names alone does not contain it, the alias is not an alias,
    and `pMVar9` declines for want of a frame that is three lines above it. That
    decline cost the sphyl-against-trianglelist path, which is the one the
    ragdoll scene runs on.

    A plain copy is the only shape admitted; anything with an operator, a cast
    or a field in it is not an alias and is left to `frames` to judge."""
    seed = set(names)
    # ⚠ AND THE STAR IS NOT ALWAYS WRITTEN: this SDK returns its handles by
    # typedef, so the pointer that names the object is declared
    # `McdTriangleListID trilistGeom;`. A pattern that requires a literal `*`
    # finds neither it nor the alias, which is exactly how the sphyl path
    # declined once already. `tag_of` decides instead of the spelling.
    for m in re.finditer(r'(?m)^[ \t]*(?:const |struct )*[A-Za-z_]\w*[ \t]*\**[ \t]*'
                         r'(?P<v>[A-Za-z_]\w*)[ \t]*;', region):
        v = m.group('v')
        if flo.tag_of(flo.declared_type(region, v), inc) == tag:
            seed.add(v)
    parent = {n: n for n in seed}

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x

    for m in ASSIGN.finditer(region):
        v, rhs = m.group('v'), ' '.join(m.group('rhs').split())
        a = ALIAS_RHS.match(rhs)
        if v in parent and a and a.group('v') in parent:
            parent[find(v)] = find(a.group('v'))
    out = {}
    for n in names:                       # report only the groups we were asked about
        out.setdefault(find(n), set()).add(n)
    for n in seed:                        # ...but carry the whole group's members
        if find(n) in out:
            out[find(n)].add(n)
    return out


def collect(srcdir, build, inc):
    """Every candidate site, with its i386 byte offset K already measured."""
    texts = {}
    for fn in sorted(os.listdir(srcdir)):
        if fn.endswith('.c') and os.path.exists(os.path.join(build, fn[:-2] + '.o')):
            texts[fn] = open(os.path.join(srcdir, fn), errors='ignore').read()
    derived = derived_map(inc)

    raw = []
    for fn, text in texts.items():
        for m in INDEXED.finditer(text):
            if m.group('pre'):
                continue                  # a member array, not a bare pointer
            region = flo.region_of(text, m.start())
            dt = flo.declared_type(region, m.group('v'))
            tag = flo.tag_of(dt, inc) if dt else None
            if not tag or tag not in derived:
                continue
            raw.append((fn, flo.enclosing(text, m.start()), m, tag))
    need = set()
    for fn, func, m, tag in raw:
        need.add(sizeof_expr(tag))
        need.add(offset_expr(tag, m.group('f')))
        need.add(fieldsize_expr(tag, m.group('f')))
        for c in derived[tag]:
            need.add(sizeof_expr(c))
    for model in ('i386', 'lp64'):
        measure(model, need, inc)
    i386, lp64 = _MEAS['i386'], _MEAS['lp64']

    sites = []
    for fn, func, m, tag in raw:
        sz, fo = i386.get(sizeof_expr(tag)), i386.get(offset_expr(tag, m.group('f')))
        szl, fol = lp64.get(sizeof_expr(tag)), lp64.get(offset_expr(tag, m.group('f')))
        if None in (sz, fo, szl, fol) or sz == szl:
            continue                      # unmeasurable, or T does not move
        k = int(m.group('k'))
        sites.append(dict(fn=fn, func=func, m=m, tag=tag, k=k, v=m.group('v'),
                          f=m.group('f'), K=k * sz + fo, naive=k * szl + fol,
                          fw=lp64.get(fieldsize_expr(tag, m.group('f')))))
    return texts, sites


def concrete_types(texts, sites, inc, cache):
    """(file, function, variable) -> (tag, why), plus the declines and why.

    Five sources of evidence, strongest first, and every one of them is gated
    twice: by THE FRAME above, and by the requirement that EVERY offset used
    against that variable lands on a real field of the answer AND inside it."""
    offs, decl, region_of_key = {}, {}, {}
    for s in sites:
        key = (s['fn'], s['func'], s['v'])
        offs.setdefault(key, set()).add(s['K'])
        decl[key] = s['tag']
        region_of_key[key] = flo.region_of(texts[s['fn']], s['m'].start())
    # whatever baked literals fix_literal_offsets could not resolve count too:
    # they are offsets against the same pointer and the fit has to explain them
    for fn, text in texts.items():
        for m in flo.all_sites(text):
            v = m.group('base')
            if '->' in v:
                continue
            key = (fn, flo.enclosing(text, m.start()), v)
            if key in offs:
                offs[key].add(int(m.group('off'), 0))

    named = {}                                  # what fix_literal_offsets wrote
    for fn, text in texts.items():
        for m in NAMED_TAG.finditer(text):
            key = (fn, flo.enclosing(text, m.start()), m.group('v'))
            prev = named.get(key, m.group('tag'))
            named[key] = m.group('tag') if prev == m.group('tag') else None
    reg = {}                                    # the geometry a file registers
    for fn, text in texts.items():
        names = {m.group('name') for m in flo.GEOM_REG.finditer(text)}
        if len(names) == 1:
            reg[fn] = names.pop()
    # the interaction each function is REGISTERED for, which is the only source
    # that can type an Ix* object — see interaction_types.py. `McdSphylPlane` is
    # registered as the pair (5, 3), so `p->model1` is an McdSphyl and
    # `p->model2` an McdPlane, with nothing inferred.
    pairs, _dropped = interaction_types.pair_types(texts)
    GEOMOF = re.compile(r'(?m)^[ \t]*(?P<v>[A-Za-z_]\w*)\s*=\s*(?:\([^)]*\)\s*)?'
                        r'(?:kd_)?McdModelGetGeometry\s*\(\s*[A-Za-z_]\w*\s*->\s*'
                        r'model(?P<n>[12])\s*\)')

    measure('i386', [sizeof_expr(c) for cs in derived_map(inc).values()
                     for c in cs], inc)
    i386 = _MEAS['i386']

    def fits(c, want):
        sz = i386.get(sizeof_expr(c))
        if not sz or any(o >= sz for o in want):
            return False
        return any(all(o in lookup(c, inc, cache) for o in want)
                   for lookup in (flo.head_offsets_of, flo.flat_offsets_of,
                                  flo.offsets_of))

    out, declined = {}, {}
    # ---- group the variables that name the same object, per function. The
    # offsets and the frame are then asked ONCE of the group; see `frames`.
    byfunc = {}
    for key in offs:
        byfunc.setdefault((key[0], key[1], decl[key]), []).append(key[2])
    for (fn, func, tag0), names in sorted(byfunc.items()):
        region = region_of_key[(fn, func, names[0])]
        for _root, group in sorted(alias_groups(region, set(names), tag0, inc).items()):
            group = sorted(group)
            keys = [(fn, func, v) for v in group if (fn, func, v) in offs]
            if not keys:
                continue
            tags = {decl[k] for k in keys}
            if len(tags) != 1:
                for k in keys:
                    declined[k] = 'aliases of two declared types (%s)' % ', '.join(
                        sorted(tags))
                continue
            tag = tags.pop()
            want = set().union(*[offs[k] for k in keys])
            cands = derived_map(inc).get(tag, [])
            ok, frame_tag, note = frames(region, set(group), tag, inc)
            if len(group) > 1:
                note += ' (with %s)' % ', '.join(group)
            if not ok:
                for k in keys:
                    declined[k] = 'no frame: %s' % note
                continue
            # ⚠ ONE MODEL PER GROUP OR NOTHING. Ghidra reuses a name, and a
            # pointer assigned from model1 in one branch and model2 in another
            # is evidence about neither.
            registered = registered_why = None
            pt = pairs.get(re.sub(r'^kd_', '', func))
            if pt:
                seen = set()
                for m in GEOMOF.finditer(region):
                    if m.group('v') in group:
                        seen.add(int(m.group('n')))
                if len(seen) == 1:
                    n = seen.pop()
                    registered = pt[n - 1]
                    registered_why = ('%s is registered for %s/%s, so model%d is '
                                      'an %s' % (func, pt[0], pt[1], n, registered))
            order = [(frame_tag, 'the frame says so: %s' % note),
                     (registered, registered_why),
                     (next((named[k] for k in keys if named.get(k)), None),
                      'named by fix_literal_offsets'),
                     (reg.get(fn), 'the geometry %s registers' % fn)]
            got = None
            for c, why in order:
                if c and c in cands and fits(c, want):
                    got = (c, why)
                    break
            if not got:
                for lookup in (flo.head_offsets_of, flo.flat_offsets_of,
                               flo.offsets_of):
                    hits = [c for c in cands if i386.get(sizeof_expr(c))
                            and all(o < i386[sizeof_expr(c)]
                                    and o in lookup(c, inc, cache) for o in want)]
                    if hits:
                        break
                if len(hits) == 1:
                    got = (hits[0], 'the only %s whose fields cover %s (%s)'
                           % (tag, ','.join(str(o) for o in sorted(want)), note))
                else:
                    for k in keys:
                        declined[k] = ('%d of the %d %s types cover offsets %s%s'
                                       % (len(hits), len(cands), tag,
                                          ','.join(str(o) for o in sorted(want)),
                                          ' (%s)' % ', '.join(hits[:4]) if hits
                                          else ''))
                    continue
            for k in keys:
                out[k] = got
    return out, declined


def control_arm64(tags, inc):
    """The LP64 numbers this pass used, asserted at arm64.

    x86-64 and arm64 are the same data model, which is why `lp64_run.sh` can run
    this corpus at all — but "the same data model" is an argument and this is the
    measurement. A tag whose two answers differ would make every offset emitted
    for it x86-64-specific."""
    if not NDK or not os.path.exists(os.path.join(NDK, ARM64)):
        return None
    exprs = [sizeof_expr(t) for t in tags]
    a = measure(ARM64, exprs, inc)
    return [t for t in tags if a.get(sizeof_expr(t)) != _MEAS['lp64'].get(sizeof_expr(t))]


def oracle_confirm(pairs, inc):
    """Each (tag, LP64 offset) looked for in MathEngine's own amd64 build.

    ⚠ IT IS LLP64 AND THIS IS WHERE THAT BITES. `long` is four bytes in the
    shipped Windows build, so a tag holding one has a different layout there and
    its offsets cannot be confirmed against it — measured with mingw rather than
    assumed, and reported unconfirmable rather than quietly compared.

    ⚠ AND IT READS CODE, so a field no instruction in the archive touches has no
    entry. "not touched" is not "wrong"; it is the oracle declining to speak."""
    try:
        import amd64_oracle as ora
    except Exception as e:                                   # pragma: no cover
        return None, 'amd64_oracle unavailable: %s' % e
    measure('llp64', [sizeof_expr(t) for t, _o in pairs], inc)
    win = _MEAS['llp64']
    disp = {}
    for stem, path in sorted(ora.extract().items()):
        if '/' in stem:
            continue
        for _fn, lines in ora.disasm(path).items():
            disp.setdefault(stem, set()).update(ora.displacements(lines))
    out = []
    for tag, off in pairs:
        if win.get(sizeof_expr(tag)) != _MEAS['lp64'].get(sizeof_expr(tag)):
            out.append((tag, off, 'unconfirmable: win64 %s, linux %s'
                        % (win.get(sizeof_expr(tag)),
                           _MEAS['lp64'].get(sizeof_expr(tag)))))
            continue
        stem = re.sub(r'^_', '', tag)
        seen = sorted(s for s, d in disp.items()
                      if off in d and s.lower().startswith(stem.lower()[:6]))
        out.append((tag, off, 'confirmed in %s' % ', '.join(seen[:2]) if seen
                    else 'not touched by any %s* function in the archive' % stem[:8]))
    return out, None


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')
    cache = {}

    # ---- THE SELF-CHECK, and it is the one every tool here needs: a probe that
    # cannot compile answers None for everything, every site declines, and
    # "0 repaired" reads exactly like "there was nothing to repair". Four facts,
    # measured, taken from the defect this pass exists for.
    want = [('i386', sizeof_expr('_McdGeometry'), 16),
            ('lp64', sizeof_expr('_McdGeometry'), 32),
            ('i386', offset_expr('McdTriangleList', 'triangleListGenerator'), 48),
            ('lp64', offset_expr('McdTriangleList', 'triangleListGenerator'), 72)]
    for model in ('i386', 'lp64'):
        measure(model, [e for mo, e, _w in want if mo == model], inc)
    bad = [(mo, e, w, _MEAS[mo].get(e)) for mo, e, w in want if _MEAS[mo].get(e) != w]
    if bad:
        sys.exit('fix_index_layout: SELF-CHECK FAILED — '
                 + '; '.join('%s %s read %r (want %d)'
                             % (mo, e[:52], g, w) for mo, e, w, g in bad)
                 + '\n  The layout probe is not measuring anything; every number '
                   'below would be an artefact.')

    texts, sites = collect(srcdir, build, inc)
    concrete, declined = concrete_types(texts, sites, inc, cache)

    fixed = same = nofield = 0
    notes, pairs, tags_used, edits_by_file = [], [], set(), {}
    for s in sites:
        key = (s['fn'], s['func'], s['v'])
        if key not in concrete:
            continue
        c, why = concrete[key]
        path = flo.offsets_of(c, inc, cache).get(s['K'])
        if not path:
            nofield += 1
            notes.append('%-26s %s[%d].%s: %d is not a field start of %s — declined'
                         % (s['fn'], s['v'], s['k'], s['f'], s['K'], c))
            continue
        measure('lp64', [offset_expr(c, path), fieldsize_expr(c, path)], inc)
        measure('i386', [offset_expr(c, path)], inc)
        real = _MEAS['lp64'].get(offset_expr(c, path))
        if real is None or _MEAS['i386'].get(offset_expr(c, path)) != s['K']:
            nofield += 1
            notes.append('%-26s %s[%d].%s: %s::%s does not measure at %d — declined'
                         % (s['fn'], s['v'], s['k'], s['f'], c, path, s['K']))
            continue
        if real == s['naive']:
            # ★ THE INDEX IS ALREADY RIGHT — see the header. Rewriting these
            # would be churn with a byte-identity risk and no defect behind it.
            same += 1
            continue
        # the ACCESS WIDTH comes from the CONCRETE field; the SPELLING stays
        # Ghidra's whenever that already has the right width, because a pointer
        # spelling is what keeps `&(v[3].prev)->mRefCtAndID` compiling.
        w = _MEAS['lp64'].get(fieldsize_expr(c, path))
        ft = member_type(s['tag'], s['f'], inc)
        acc = ft if (ft and s['fw'] == w) else ('kd_uptr' if w == 8 else 'MeU32')
        # ⚠ THE WHOLE THING IS PARENTHESISED, and leaving that out declined
        # three sites in the ONE object the ragdoll's own path goes through.
        # `&pMVar9[3].prev[1].next` has a further subscript on it: without the
        # outer parentheses `[1]` binds to the cast's operand rather than to the
        # dereference, and gcc says "request for member 'next' in something not
        # a structure". A failure to compile reads as "not byte-identical", so
        # it declined quietly instead of erroring.
        rep = ('(*(%s *)((char *)%s + ((int)((char *)&((%s%s *)0)->%s - (char *)0))))'
               % (acc, s['v'], flo.kw_of(c), c, path))
        edits_by_file.setdefault(s['fn'], []).append(
            (s['m'].start(), s['m'].end(), rep,
             '%-24s %-15s %s[%d].%s -> %s  i386 %d, LP64 %d (the index said %d), '
             'as %s  [%s]'
             % (s['fn'], c, s['v'], s['k'], s['f'], path, s['K'], real, s['naive'],
                acc, why)))
        pairs.append((c, real))
        tags_used.add(c)

    for fn, edits in sorted(edits_by_file.items()):
        edits.sort(key=lambda e: -e[0])
        text0 = texts[fn]
        text, ok = flo.accept_edits(fn, text0, edits, build, inc)
        fixed += len(ok)
        keep = {id(e) for e in ok}
        for e in edits:
            notes.append(e[3] if id(e) in keep
                         else e[3] + '   DECLINED: not byte-identical at i386')
        if text != text0:
            open(os.path.join(srcdir, fn), 'w').write(text)

    for k, m in sorted(declined.items()):
        notes.append('%-24s %s in %s: %s — declined' % (k[0], k[2], k[1], m))
    print('  indexed base-class reads re-spelled as the real field : %d' % fixed)
    print('  already correct at LP64 (the whole layout scales)     : %d' % same)
    print('  declined (reported, not guessed)                      : %d'
          % (nofield + len(declined) + sum(len(e) for e in edits_by_file.values())
             - fixed))
    for n in notes:
        print('     %s' % n)

    if tags_used:
        drift = control_arm64(sorted(tags_used), inc)
        if drift is None:
            print('     ⚠ CONTROL arm64 not asked (no NDK) — the LP64 numbers '
                  'above are x86-64\'s alone')
        elif drift:
            print('     ⚠ CONTROL arm64 DISAGREES with x86-64 on: %s' % ', '.join(drift))
        else:
            print('     control: all %d tag(s) measure the same at arm64 as at x86-64'
                  % len(tags_used))
        conf, err = oracle_confirm(sorted(set(pairs)), inc)
        if err:
            print('     ⚠ %s' % err)
        else:
            for tag, off, verdict in conf:
                print('     oracle  %-18s +%-4d %s' % (tag, off, verdict))
    return 0


if __name__ == '__main__':
    sys.exit(main())
