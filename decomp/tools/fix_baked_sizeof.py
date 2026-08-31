#!/usr/bin/env python3
"""fix_baked_sizeof.py — an allocation SIZE frozen at the i386 value.

    fix_baked_sizeof.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT, which `test/lp64_run.sh` found on the first statement of the first
scene and which neither `ptrwidth_check.sh` nor `layout_check.py` counts:

    w = (MeMemoryAPI.create)(0x234);          MdtWorld.c:95
    ...
    (w->params).lengthScale = lengthScale;    MdtWorld.c:98  <- heap overflow

0x234 is 564, which is `sizeof(MdtWorld)` ON i386. At LP64 the struct is 880
bytes, so the very first allocation is 316 bytes short and the next statement
writes past it. Nothing is truncated, no cast is narrowed, and no compiler says
a word.

THE REWRITE IS DERIVED FROM TWO INDEPENDENT FACTS, and it refuses unless they
agree — which is what makes it a repair rather than a guess:

  * the ASSIGNMENT TARGET names the type. `w` is declared `MdtWorldID`, which
    is `MdtWorld *`, so the thing being allocated is an `MdtWorld`;
  * the LITERAL confirms it. `sizeof(MdtWorld)` measured at i386 must equal the
    constant — or divide it exactly, for the array allocations
    (`McdBatch` asks for 96000 bytes of `McdModelPair`).

Neither alone is enough. The size alone is ambiguous — many structs are 0x50
bytes — and the type alone would let a byte buffer be rewritten as an array of
something. Together they pin it, and a site where they disagree is left ALONE
and reported.

WHY IT CANNOT REGRESS i386, wasm32 OR armv7: on those targets `sizeof(T)` IS
the constant it replaces, by the check above. It is written `(int)sizeof(...)`
and the cast is load-bearing — `sizeof` is `size_t`, so `count * sizeof(T)` is
UNSIGNED where `count * 0x98` was `int`, with different overflow rules, and gcc
emits a different loop for it. The i386 acceptance test caught exactly that:
CxSmallSort's object grew by 64 bytes.

★ AND THAT PRESCRIPTION IS NOT GENERAL, WHICH IS WHY THIS TOOL NOW COMPILES
EVERY SITE ITSELF. The next site to need it wanted the opposite spelling:
`pMVar1->bucketCount << 2` is byte-identical as `(int)(count * sizeof(T))` and
DIFFERS as `count * (int)sizeof(T)`. Both compute the same value on every
target; which one gcc schedules the same way is a property of the surrounding
function, not a rule anyone can write down in advance. So each candidate is
compiled and compared against the baseline object, whichever reproduces it
byte for byte is kept, and a site where neither does is declined and reported.
"No-op by construction" is a claim, and the compiler is the only thing that can
check it — the corpus-wide acceptance test now confirms rather than discovers.

THREE SPELLINGS OF THE ONE DEFECT are covered: `create(LITERAL)`,
`create(COUNT * K)`, and `MePoolAPI.init(pool, n, K, align)` — where the size is
argument THREE and there is no assignment target to take a type from. The pool
form is the worst of the three: `K` is an ELEMENT STRIDE, so at LP64 every
element overlaps the one before it and the crash surfaces in a red-black tree
walk two files away. Its element type is pinned by three facts (getStruct's
declared type, the i386 literal, and the constant the shipped amd64 build passes
in the same function — `amd64_oracle.py`), because the FIRST of those is wrong
for one of the seven: `constraintPool`'s getStruct is typed as the base
`MdtConstraint` and the pool is sized for `MdtContact`.

It is a POST-PASS, like `fix_ptrwidth.py`, because it needs a compiler to
measure `sizeof`. §4's 95-second output is not the LP64-correct source; this and
`fix_ptrwidth.py` together are what make it so. Run it on a COPY: it edits in
place. `test/lp64_pipeline.sh` does the copy, both passes and the gate in order.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

# The product root. HERE/include holds kd_compat.h, kd_karma.h and
# kd_types.h — the three headers every recovered source includes, and the
# ones the size/offset probes below have to see to measure anything.
HERE = kd_paths.MD
WORK = '/tmp/kd_sizeof'

# `X = [(cast)] (MeMemoryAPI.create)(SIZE[, ...])`. SIZE runs to the matching
# `,` or `)` and may span lines and one level of parentheses — Ghidra wraps the
# interaction-table allocation across two — so it is matched permissively here
# and VALIDATED below. Anything that is neither a literal nor a `count * size`
# product is declined and reported, never guessed at.
ALLOC = re.compile(
    r'(?m)^(?P<ind>[ \t]*)(?P<var>[A-Za-z_]\w*)[ \t]*=[ \t]*'
    r'(?P<pre>\(?[ \t]*(?:\([A-Za-z_][\w ]*\**\)[ \t]*)?\(?)'
    r'(?P<fn>MeMemoryAPI\.\w+|MePoolAPI\.\w+)\)?[ \t]*\([ \t\r\n]*'
    r'(?P<size>(?:[^,;()]|\([^()]*\))+?)(?P<rest>[,)])')

# `poolSize << 2` — a COUNT times the i386 size of the element, which for a
# `void **` is the pointer size. Same defect as a baked `sizeof`, different
# spelling, and it is the one that survives into MePoolFixedInit:
#     ppvVar2 = (MeMemoryAPI.create)(poolSize << 2);   /* void ** */
# 4 is sizeof(void *) HERE and 8 at LP64, so the free-list is half the size it
# needs to be and the loop below it writes off the end.
#
# ★ THE COUNT IS NOT ALWAYS ONE IDENTIFIER, and restricting it to one missed the
# defect that stopped `lp64_run.sh` after the pools were repaired:
#
#     pMVar4 = (MeMemoryAPI.create)(frame->geometryRegisteredCountMax *
#                                   frame->geometryRegisteredCountMax * 0x1c);
#
# 0x1c is 28 is `sizeof(McdInteractions)` at i386 and 48 at LP64, so the
# interaction table comes back at 58% of the size it needs — and the ASan report
# lands in `McdFrameworkSetInteractions`, which is CORRECT CODE indexing a short
# array, two hundred lines from the allocation that shortened it.
#
# ⚠ PRECEDENCE IS LOAD-BEARING IN THIS PATTERN. `A * B * K` may be rewritten as
# `(A * B) * sizeof(T)`; `A + B * K` may NOT, because the `* K` binds to B alone
# and parenthesising the sum changes the arithmetic. So the left operand is
# allowed products, members, indexes and calls — and no top-level `+ - / % ? :`.
SCALED = re.compile(r'^(?P<expr>[A-Za-z0-9_ \t()\[\]>.*+/%,-]*?[A-Za-z0-9_\])])'
                    r'[ \t]*(?P<op><<|\*)[ \t]*(?P<k>0x[0-9a-f]+|\d+)$')
_BADOP = re.compile(r'[+/%?:,]|(?<![-\w])-(?!>)')
_GROUP = re.compile(r'\([^()]*\)')


def scale_unsafe(expr):
    """Is there an operator at TOP LEVEL that `* K` would not have applied to?

    ⚠ ASKING WITHOUT STRIPPING THE PARENTHESES FIRST IS WRONG IN BOTH
    DIRECTIONS, and the blunt version silently dropped a real repair:
    `(geoTypeMaxCount + 9) * 0x28` is the geometry vtable table, the `+` is
    inside its own group, and `* K` does apply to the whole thing. It read as
    unsafe and the site went unrewritten — a decline that looks like a decision
    and was a bug."""
    prev = None
    while prev != expr:
        prev, expr = expr, _GROUP.sub(' ', expr)
    return bool(_BADOP.search(expr))

# ---------------------------------------------------------------------------
# THE SAME DEFECT WITH THE SIZE IN ARGUMENT THREE, WHICH `ALLOC` CANNOT SEE.
#
#     (MePoolAPI.init)(&w->bodyPool, w->maxBodies, 0x240, 0x10);   MdtWorld.c:108
#
# 0x240 is 576 is `sizeof(MdtBody)` ON i386. `ALLOC` misses it twice over: the
# call is not an assignment, so there is no target to take a type from, and the
# size is the THIRD argument rather than the first. The consequence is worse
# than a short allocation — it is an ELEMENT STRIDE, so at LP64 every body after
# the first overlaps the one before it, `constraintDict` comes back as garbage,
# and the crash surfaces two files away in `MeDictInsert` with nothing in the
# backtrace pointing at the pool. That is where `lp64_run.sh` stopped once the
# first-argument allocations were repaired.
#
# `MePool.h`: init(MePool *pool, int poolSize, int structSize, int alignment).
POOL_INIT = re.compile(
    r'\(\s*(?P<api>MePool\w*API)\s*\.\s*init\s*\)\s*\(\s*'
    r'(?P<pool>[^,]+?)\s*,\s*(?P<count>[^,]+?)\s*,\s*'
    r'(?P<size>0x[0-9a-f]+|\d+)\s*,')

# `pMVar5 = (MePoolAPI.getStruct)(manager->linkPool);` — the pool's ELEMENT type,
# named by the thing the caller assigns it to. This is the primary fact for a
# pool site, exactly as the assignment target is for an allocation: the literal
# and the shipped 64-bit build then CONFIRM it. It has to be corpus-wide,
# because the pool is initialised in one object and drawn from in another —
# `bodyPool` is set up in MdtWorld.c and used in MdtBody.c.
GETSTRUCT = re.compile(
    r'(?P<var>[A-Za-z_]\w*)\s*=\s*\(\s*MePool\w*API\s*\.\s*getStruct\s*\)\s*'
    r'\(\s*(?P<pool>[^;]+?)\s*\)\s*;')

# ---------------------------------------------------------------------------
# THE SAME DEFECT IN `qsort`, AND IT IS THE ARM64 RAGDOLL CRASH.
#
#     qsort(partArray, asset->partCount, 4, _MeFAssetPartSortFunc);   MeFAsset.c:920
#
# `partArray` is `MeFAssetPart **` — an array of POINTERS — so `4` is
# `sizeof(*partArray)` ON i386 and eight at LP64. qsort walks the array in
# four-byte steps over eight-byte elements, so every "element" the comparator is
# handed after the first is the top half of one pointer welded to the bottom half
# of the next. Nothing is truncated and no diagnostic fires; the comparator just
# dereferences a spliced address.
#
# ★ MEASURED ON THE DEVICE, 2026-08-31, on a OnePlus 6 loading a ragdoll:
#
#     signal 11 (SIGSEGV), SEGV_MAPERR, fault addr 0x6390f87800000073
#     #00 <the comparator>  #01 local_qsort+1012  #02 MeFAssetGetPartsSortedByName
#     #04 KInitSkeletonKarma  #05 KInitActorKarma  #06 AActor::setPhysics
#
# `libUT2004.so` sits around `0x73_00000000` there, so an element reads
# `0x00000073_6390f878`; the fault address is those two words exchanged. Four
# sites in `MeFAsset.c` — parts, geometries, models and joints — and every one of
# them is on the ragdoll-creation path.
#
# ⚠ THE TYPE NEEDS NO LOOKUP AND MUST NOT HAVE ONE. `sizeof(*partArray)` names
# the element through the array itself, so there is no struct to resolve, no
# typedef to guess and nothing to get wrong — and it is the same expression at
# every pointer width by construction. The first argument therefore has to be a
# plain identifier; `MdtLOD.c`'s `qsort(*(void **)(&(*kd_argslot_ffffffc4)), ...)`
# is not one, and is declined and reported rather than rewritten through a cast
# whose pointee is `void`.
QSORT = re.compile(
    r'\b(?P<call>qsort|bsearch)\s*\((?P<args>\s*(?P<base>[A-Za-z_]\w*)\s*,'
    r'\s*[^,;()]+?\s*,\s*(?P<size>0x[0-9a-fA-F]+|\d+)\s*,)')
# ---- THE SAME FACT IN `MePoolxInit`, WHICH IS NOT THE `MePoolAPI.init` FORM.
#
#     MePoolxInit(&s->nodepool, nodemem, 0x18, maxnode);        MeSet.c
#
# `MePoolxInit(MePoolx *p, void *memory, int recsize, int numrec)` — a direct
# call with the size in argument THREE and the array in argument TWO, so neither
# `POOL_INIT` (a function-POINTER table call, and a different argument order) nor
# `ALLOC` matches it. `0x18` is 24 is `sizeof(MeDictNode)` at i386 and FORTY-EIGHT
# at LP64, so the pool hands out node addresses 24 bytes apart over 48-byte nodes
# and every node it allocates overlaps the one before it.
#
# ★ REACHED FROM `McdConvexMeshPlaneCut`, which is `McdGeometryInstanceGetSlice`
# — a convex mesh being sliced, i.e. a VEHICLE. Measured on a OnePlus 6:
# `SEGV_ACCERR` inside `MeSetAdd+68`, two minutes into an Onslaught match.
#
# ⚠ AND THE SIBLING CALL IS CORRECT AND MUST BE LEFT ALONE.
# `McdGjkPenetrationDepth` does `MePoolxInit(&qmem.fpool, poolmem, 0x2c, 0x32)`,
# and `McdGjkFace` is 44 bytes at BOTH widths — all ints and floats. Two calls in
# the corpus, one moves and one does not, which is exactly what `moves_at_lp64`
# is for.
POOLX_INIT = re.compile(
    r'\bMePoolxInit\s*\(\s*[^,;()]+?\s*,\s*(?P<base>[A-Za-z_]\w*)\s*,'
    r'\s*(?P<size>0x[0-9a-fA-F]+|\d+)\s*,')
# …and the same call whose first argument is anything else, so it can be counted
# rather than silently skipped. A pass that matches nothing reports a clean zero.
QSORT_ANY = re.compile(r'\b(?:qsort|bsearch)\s*\(')

# ---------------------------------------------------------------------------
# THE SAME `alloca`, WITH THE TYPE ON THE FIELD IT IS STORED INTO.
#
#     (*(McdGeometryID *)((char *)pMVar9 + KD_OFFSET(McdTriangleList, list)))
#         = (McdGeometryID)(kd_alloca_iVar3 = (char *)alloca((size_t)(n) * 0x18 + 0));
#
# `ALLOCA` cannot see this: it wants the statement to open with a variable or a
# cast, and this one opens with a dereferenced offsetof. `0x18` is 24 is
# `sizeof(McdUserTriangle)` at i386 and FORTY-EIGHT at LP64, so the triangle
# array is half the size the generator is about to fill and every element past
# the midpoint is written off the end of the frame.
#
# ★ THE TYPE IS DECLARED, NOT INFERRED, AND IT IS THE STRONGEST EVIDENCE THIS
# PASS HAS. The block is stored into `McdTriangleList::list`, which the oracle
# declares `McdUserTriangle *list;` — the assignment target names the type,
# exactly as it does for `MeMemoryAPI.create`, only spelled as an offsetof. The
# literal then has to equal that type's i386 size or the site declines, which is
# the same two-facts-must-agree rule as everywhere else here.
#
# ⚠ WHY NOT LEAVE IT TO `fix_element_stride`. That pass repairs three of the
# four triangle-list allocas and CANNOT repair the fourth, for a good reason:
# its anchor is gated on `fix_word_indexed_struct` having already typed an
# access in the same file as `((E *)v)->`, and broadening that gate is
# measured-unsafe (proven.txt LP64-ANDROID-ARM64: 63 -> 147 rewrites,
# `scene_ragdoll` nondeterministic). `IxSphylPrimitives` walks its triangles
# through BYTE cursors (`undefined1 *`), so no such access exists and the gate
# correctly refuses. This rule needs no gate of that kind because it does not
# infer the type at all.
#
# ★ MEASURED: `McdSphylTriangleListIntersect` is where a ragdoll capsule meets
# world geometry, and it is the crash the x86-64 vehicle reaches on five of
# eight gametypes once ragdolls start being created — `GenerateTriangleContact`
# dereferencing `tri->vertices[0]` of a triangle read past the array.
ALLOCA_INTO_FIELD = re.compile(
    r'\(\*\([A-Za-z_][\w ]*\**\)\(\(char \*\)\w+ \+ '
    r'\(\(int\)\(\(char \*\)&\(\((?:struct )?(?P<T>\w+) \*\)0\)->(?P<F>[\w.\[\]]+)'
    r' - \(char \*\)0\)\)\)\)\s*=\s*\([A-Za-z_][\w ]*\**\)\s*'
    r'\((?P<blk>kd_\w+) = \(char \*\)alloca\(\(size_t\)\(.*?\)'
    r'\s*\*\s*(?P<size>0x[0-9a-f]+|\d+)\s*\+[ \t]*(?P<add>0x[0-9a-f]+|\d+)?')
_MEMBER_PTR = re.compile(r'([A-Za-z_]\w*)\s*\*\s*%s\s*[;,]')


def field_pointee(T, F, inc):
    """`McdTriangleList::list` is declared `McdUserTriangle *` — read from the
    ORACLE, which is the yardstick and is never edited."""
    import fix_literal_offsets as flo
    bodies = flo.struct_bodies(inc)
    body = bodies.get(T) or bodies.get('_' + T)
    if not body:
        return None
    m = re.search(_MEMBER_PTR.pattern % re.escape(F),
                  re.sub(r'/\*.*?\*/', ' ', body, flags=re.S))
    return (m.group(1) + ' *') if m else None

BANNER = re.compile(r'(?m)^/\* ---- (\S+)')

# ---------------------------------------------------------------------------
# THE SAME DEFECT ON THE STACK. `alloca` is not `MeMemoryAPI.create`, so none of
# the patterns above see it:
#
#     addedBodies = (MdtBody **)(kd_alloca_x = (char *)alloca((size_t)(n) * 4 + 0));
#
# `4` is `sizeof(MdtBody *)` at i386. These are arrays of POINTERS, so at LP64
# they come back HALF SIZE and every write past the midpoint runs off the end —
# which is what `MdtUpdatePartitions` was still failing on after its arena was
# repaired, at four different lines, none of them near the allocation.
#
# The element type is written down in the CAST that consumes the block, or in
# the declaration of the variable it lands in; 24 sites carry a baked multiplier.
ALLOCA = re.compile(
    r'(?m)^[ \t]*(?:(?P<var>[A-Za-z_]\w*)\s*=\s*)?'
    r'(?:\((?P<cast>[A-Za-z_][\w ]*\**)\)\s*)?'
    r'\(?\s*(?P<blk>kd_\w+)\s*=\s*\(char \*\)alloca\(\(size_t\)\(.*?\)'
    r'\s*\*\s*(?P<size>0x[0-9a-f]+|\d+)\s*\+[ \t]*(?P<add>0x[0-9a-f]+|\d+)?')

# ★ AND THE TYPE IS NOT ALWAYS ON THE SAME STATEMENT. The block lands in a
# `char *` and is cast into place on a LATER line:
#
#     kd_alloca_iVar30 = (char *)alloca((size_t)(inMaxContactPointCount) * 0x10 + 0x10);
#     iVar24 = 0;
#     list.link = (McdContactLink *)(kd_alloca_iVar30);
#
# `0x10` is `sizeof(McdContactLink)` at i386 and 32 at LP64, so the link array
# comes back HALF SIZE — and this site was one of the five the tool reported as
# "alloca: no type for the block" while `McdContactSimplify` overran it in
# seventeen places. The cast that CONSUMES the block names the element type just
# as well as one on the allocation itself; it is only further away.
CONSUMER = r'=\s*\(\s*(?P<cast>[A-Za-z_][\w ]*\*)\s*\)\s*\(?\s*%s\b'


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


HEAD = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
        '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))


def elem_size(ty, inc, cache):
    """sizeof(*x) for a variable declared `ty x`, measured at i386. None if the
    type is not a pointer to a complete type — `void *`, an opaque handle, or a
    tag this translation unit invents."""
    if ty in cache:
        return cache[ty]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p.c')
    open(src, 'w').write(HEAD + '#include <stdio.h>\n'
                         'int main(void){printf("%d\\n",(int)sizeof(*(' + ty + ')0));'
                         'return 0;}\n')
    exe = os.path.join(WORK, 'p')
    if subprocess.run(['gcc', '-m32', '-DLINUX', '-w', '-o', exe, src]
                      + includes(inc), capture_output=True).returncode:
        cache[ty] = None
        return None
    out = subprocess.run([exe], capture_output=True, text=True).stdout.strip()
    cache[ty] = int(out) if out.isdigit() and int(out) > 0 else None
    return cache[ty]


# ---------------------------------------------------------------------------
# THE SECOND OPINION, AND IT COMES FROM A BUILD WE DID NOT MAKE.
#
# A pool's i386 element size does NOT pin the type: fourteen metoolkit types are
# 20 bytes at i386 and four of them are 40 at 64-bit, so `0x14` alone would let
# `linkPool` be rewritten as an array of `McdSphere`. The owner's UT2004 v3369
# tree carries a 64-bit build of this same MathEngine source, so the constant it
# passes in the same argument slot is a measurement of the answer:
#
#     MdtWorldCreate   i386  0x240 / 0x1ec        amd64  0x2b8 / 0x270
#     sizeof at win64  MdtBody 696 = 0x2b8        MdtContact 624 = 0x270
#
# ★ AND IT IS LLP64, NOT LP64. That build is MSVC for Windows, where `long` is
# four bytes; Android is eight. `sizeof(MdtBody)` is 696 there and 704 on
# Android. So this is a check on WHICH TYPE, never a number to paste — the
# rewrite stays `sizeof(T)` and lets each target's compiler answer for itself.
# `x86_64-w64-mingw32-gcc` reproduces the win64 column exactly, which is what
# makes the check mechanical; the self-check below asserts two of them against
# the immediates actually present in the shipped `MdtWorldCreate`.
#
# The size is read out of the compiler's own type printer — `char (*)[696]` in
# the diagnostic for `int x = &probe;`. That is a scrape, and this file's project
# has been burned by scraping a build system for an answer the compiler should
# have been asked directly (`proven.txt` UNICODE-GUARD-REGRESSION). The
# difference that matters: this probe is a real translation unit compiled with
# the real include path, so the number is the compiler's, not a reconstruction
# of what the compiler would have said.
WINCC = os.environ.get('KD_WINCC', 'x86_64-w64-mingw32-gcc')
_WINSZ = re.compile(r'char \(\*\)\[(\d+)\]')


def win_elem_size(ty, inc, cache):
    """sizeof(*(ty)0) at Windows x86-64 (LLP64). None if unavailable."""
    key = 'win:' + ty
    if key in cache:
        return cache[key]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'w.c')
    open(src, 'w').write(HEAD + 'char kd_probe[sizeof(*(' + ty + ')0)];\n'
                         'int kd_force = &kd_probe;\n')
    r = subprocess.run([WINCC, '-DWIN32', '-D_WIN32', '-c', '-o', os.devnull, src]
                       + includes(inc), capture_output=True, text=True)
    m = _WINSZ.search(r.stderr)
    cache[key] = int(m.group(1)) if m else None
    return cache[key]


def enclosing(text, pos):
    """The name in the nearest `/* ---- NAME ... */` banner above `pos`."""
    last = None
    for m in BANNER.finditer(text, 0, pos):
        last = m.group(1)
    return last


def region_of(text, pos):
    """The banner-delimited function body containing `pos` — a local's
    declaration must be looked for in ITS function and nowhere else."""
    start, end = 0, len(text)
    for m in BANNER.finditer(text):
        if m.start() <= pos:
            start = m.start()
        elif m.start() > pos:
            end = m.start()
            break
    return text[start:end]


def declared_type(region, var):
    """`T *x;` -> 'T *'. Ghidra declares every local at the top of its body.

    ⚠ `return pMVar1;` matches the same shape and yields the "type" `return`.
    Harmless downstream — `sizeof(*(return)0)` does not compile, so the site is
    declined — but it turns a real answer into a decline whenever a `return`
    precedes the declaration, so it is excluded here rather than later."""
    for m in re.finditer(r'(?m)^[ \t]*((?:const |struct )*[A-Za-z_]\w*[ \t]*\**)'
                         r'[ \t]*' + re.escape(var) + r'[ \t]*;', region):
        ty = re.sub(r'\s+', ' ', m.group(1)).strip()
        if ty.split()[0] not in ('return', 'goto', 'break', 'continue'):
            return ty
    return None


def pool_key(expr):
    """`&w->bodyPool` -> 'bodyPool'; a bare local -> itself. The trailing name
    is what identifies the pool across objects; the leading `&frame->` is not."""
    ids = re.findall(r'[A-Za-z_]\w*', expr)
    return ids[-1] if ids else None


# ---------------------------------------------------------------------------
# ★ PROVE THE NO-OP PER SITE INSTEAD OF ASSERTING IT.
#
# This file's own docstring said the rewrite is "no-op on i386 by construction"
# and prescribed ONE spelling — `count * (int)sizeof(T)` — because `sizeof` is
# `size_t` and an unsigned `count * sizeof(T)` made `CxSmallSort`'s object grow
# by 64 bytes. That is true, and it is not general. The very next site to need
# it wanted the OPPOSITE:
#
#     ppMVar3 = (MeMemoryAPI.create)(pMVar1->bucketCount << 2);
#
#     pMVar1->bucketCount * (int)sizeof(*(T **)0)     DIFFERS  (the prescribed one)
#     (int)(pMVar1->bucketCount * sizeof(*(T **)0))   identical
#
# Both spellings compute the same value on every target; gcc simply schedules
# them differently, and WHICH ONE survives is a property of the surrounding
# function, not of the rule. So the tool now compiles each candidate and keeps
# whichever reproduces the baseline object BYTE FOR BYTE — and if neither does,
# the site is declined and reported rather than rewritten on a promise. The
# whole-corpus acceptance test stops being the thing that catches this tool's
# mistakes and becomes a confirmation of something already established.
CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types',
          '-DLINUX']


def compiles_identically(fn, text, build, inc):
    """Does this source still produce the baseline .o, byte for byte?

    ⚠ THE FILE HAS TO KEEP ITS NAME. gcc records the source basename in an
    `STT_FILE` symbol, so compiling the same text from `/tmp/t.c` produces a
    different object and reads exactly like a codegen change. That cost a
    bisection round: the disassembly was identical and `cmp` still said no."""
    base = fn[:-2]
    ref = os.path.join(build, base + '.o')
    if not os.path.exists(ref):
        return False
    d = os.path.join(WORK, 'ident')
    os.makedirs(d, exist_ok=True)
    src = os.path.join(d, fn)
    open(src, 'w').write(text)
    obj = os.path.join(d, base + '.probe.o')
    r = subprocess.run(['gcc'] + CFLAGS + ['-I' + os.path.join(HERE, 'include')]
                       + includes(inc) + ['-c', '-o', obj, src],
                       capture_output=True)
    if r.returncode:
        return False
    try:
        return open(ref, 'rb').read() == open(obj, 'rb').read()
    except OSError:
        return False


def moves_at_lp64(fn, before, after, inc):
    """Does this rewrite change anything AT 64-BIT POINTER WIDTH?

    ★ ONLY REWRITE WHAT IS BROKEN, and for a `sizeof(*p)` there is no type to
    measure — that is the point of the spelling. So measure the OBJECT instead:
    compile the file at `-m64` before and after. If the two are identical the
    literal was already the LP64 element size, the site is not a defect, and
    rewriting it is churn. If they differ, the literal was an i386 size and this
    is exactly the class.

    `McdPolygonIntersection.c`'s `qsort(poly, numpoly, 0xc, ...)` is why this
    exists: twelve bytes is three floats at every pointer width, so that call is
    already right and must be left alone. The four in `MeFAsset.c` are arrays of
    pointers and all four move.

    Paired with `compiles_identically`, the two together say the whole thing: no
    change on the shipped target, a change on the broken one.

    ⚠ BOTH SIDES MUST KEEP THE FILE'S OWN NAME — the same `STT_FILE` trap
    `compiles_identically` documents, and it bites HARDER here because there is
    no baseline to notice it against. Written as `a_<fn>` and `b_<fn>` the two
    objects differ on the name alone, `moves_at_lp64` returns True for
    everything, and the "only rewrite what moves" test silently stops testing
    anything: it passed `McdPolygonSort`, which sorts `MeVector3` — twelve bytes
    at every pointer width and already correct. Two directories, one name."""
    cf = ['-m64', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99', '-w',
          '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']
    objs = []
    for tag, txt in (('a', before), ('b', after)):
        d = os.path.join(WORK, 'lp64', tag)
        os.makedirs(d, exist_ok=True)
        src = os.path.join(d, fn)
        open(src, 'w').write(txt)
        obj = os.path.join(d, fn[:-2] + '.o')
        if subprocess.run(['gcc'] + cf + ['-I' + os.path.join(HERE, 'include')]
                          + includes(inc) + ['-c', '-o', obj, src],
                          capture_output=True).returncode:
            return False
        objs.append(open(obj, 'rb').read())
    return objs[0] != objs[1]


def spellings(count, ty):
    """The candidate ways to say `count elements of T`, most-constrained first.
    Both are correct C on every target; only their i386 codegen differs."""
    return ['(%s) * (int)sizeof(*(%s)0)' % (count, ty),
            '(int)((%s) * sizeof(*(%s)0))' % (count, ty)]



# `pMVar1->linkPool = pMVar2;` two lines above `(MePoolAPI.init)(pMVar2, ...)`.
# The init spells the pool as a LOCAL and every getStruct spells it as a MEMBER,
# so without this the two never meet and both of McdModelPairManager's pools
# decline. NEAREST PRECEDING wins: that function reuses `pMVar2` for both pools,
# so any-match would resolve the pair pool to the link pool's type.
ALIAS = (r'(?:\w+\s*->\s*(?P<a>\w+)\s*=\s*%s\s*;'
         r'|%s\s*=\s*&\s*\w+\s*->\s*(?P<b>\w+)\s*;)')


def alias_member(region, pos, local):
    """The struct member this local was last made to refer to, before `pos`."""
    pat = re.compile(ALIAS % (re.escape(local), re.escape(local)))
    last = None
    for m in pat.finditer(region, 0, pos):
        last = m.group('a') or m.group('b')
    return last


def same_type(a, b, inc, cache):
    """Are these two spellings the SAME type? `McdGeometryInstance` and
    `struct _McdGeometryInstance` are, and a candidate list that cannot say so
    reports an ambiguity that does not exist. Asked of the compiler rather than
    inferred from the names."""
    key = ('compat', a, b)
    if key in cache:
        return cache[key]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'c.c')
    open(src, 'w').write(HEAD + 'int kd_c[__builtin_types_compatible_p(%s, %s) '
                         '? 1 : -1];\n' % (a, b))
    cache[key] = subprocess.run(
        ['gcc', '-m32', '-DLINUX', '-w', '-c', '-o', os.devnull, src]
        + includes(inc), capture_output=True).returncode == 0
    return cache[key]


_TYPES = []


def all_types(inc):
    """Every tag and typedef the headers give a body to. The universe a
    two-build pin is drawn from."""
    if _TYPES:
        return _TYPES
    names = set()
    for root in (inc, os.path.join(HERE, 'include')):
        for dirpath, _d, files in os.walk(root):
            for fn in files:
                if not fn.endswith('.h'):
                    continue
                txt = open(os.path.join(dirpath, fn), errors='ignore').read()
                for m in re.finditer(r'\bstruct\s+(_?\w+)\s*\{', txt):
                    names.add('struct ' + m.group(1))
                for m in re.finditer(r'\}\s*(\w+)\s*;', txt):
                    names.add(m.group(1))
    _TYPES.extend(sorted(names))
    return _TYPES


def pin_by_size(lit, seen, inc, cache):
    """Types whose i386 size is the baked stride AND whose 64-bit size the
    shipped amd64 build actually passes in this function. Two builds we did not
    make, agreeing. Returns the distinct types that survive."""
    out = []
    for ty in all_types(inc):
        p = ty + ' *'
        if elem_size(p, inc, cache) != lit:
            continue
        w = win_elem_size(p, inc, cache)
        if w is None or w not in seen:
            continue
        if not any(same_type(ty, o, inc, cache) for o in out):
            out.append(ty)
    return out


# The bridge to `amd64_oracle.py`. Imported lazily and by path, because that
# tool needs a shipped SDK this repo does not carry: without it the pool half
# declines and says so, and the allocation half — which never needed it — is
# unaffected.
_AMD64 = {}


def amd64_constants(function):
    """Every integer constant the SHIPPED 64-bit build materialises in this
    function, or None if it cannot be read. `None` and `set()` are different
    answers and the caller must not conflate them: "not in that build" is not
    "that build passes no such constant"."""
    if function in _AMD64:
        return _AMD64[function]
    if 'mod' not in _AMD64:
        try:
            import importlib.util
            p = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                             'amd64_oracle.py')
            spec = importlib.util.spec_from_file_location('kd_amd64_oracle', p)
            mod = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(mod)
            mod.extract()
            _AMD64['mod'] = mod
        except SystemExit:
            _AMD64['mod'] = None
        except Exception:
            _AMD64['mod'] = None
    mod = _AMD64['mod']
    if mod is None:
        _AMD64[function] = None
        return None
    _path, lines = mod.find(function)
    _AMD64[function] = mod.constants(lines) if lines else None
    return _AMD64[function]


def pool_elem_types(corpus):
    """{pool-name: {declared type of what getStruct hands back}}.

    A pool reached through a MEMBER (`manager->linkPool`) is matched by that
    member's name across every object, because the init and the getStruct are
    usually in different ones. A pool held in a LOCAL is matched only inside its
    own function — `pMVar1` and `pMVar2` are Ghidra's names for a hundred
    unrelated variables and a corpus-wide match on one would be meaningless."""
    member, local = {}, {}
    for path, text in corpus.items():
        for m in GETSTRUCT.finditer(text):
            key = pool_key(m.group('pool'))
            ty = declared_type(region_of(text, m.start()), m.group('var'))
            if not key or not ty:
                continue
            if re.search(r'->|\.', m.group('pool')):
                member.setdefault(key, set()).add(ty)
            else:
                local.setdefault((path, enclosing(text, m.start()), key),
                                 set()).add(ty)
    return member, local


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    # ★ THE SECOND RUN, AND IT IS NOT BELT-AND-BRACES — the same reason
    # `fix_literal_offsets` runs twice. `ALLOCA_INTO_FIELD` needs the target
    # spelled as an offsetof naming a real member, and in the raw recovery that
    # statement is `pMVar9[3].prev = ...`: the field does not acquire a NAME
    # until `fix_literal_offsets` and `fix_index_layout` have run, which is long
    # after this pass. So the pipeline invokes it again, late, with this flag —
    # and ONLY that rule fires, so nothing else is re-litigated over text five
    # passes have since rewritten.
    only_field = '--field-allocas-only' in sys.argv[3:]
    rest = [a for a in sys.argv[3:] if not a.startswith('--')]
    root = rest[0] if rest else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')
    cache = {}

    # ---- THE SELF-CHECK. A probe that cannot compile returns None for every
    # type, which reads as "nothing to fix" — an absence of findings that looks
    # exactly like success. `MdtWorldID` must measure 564; if it does not, the
    # measurement is broken and nothing below should be believed.
    probe = elem_size('MdtWorldID', inc, cache)
    if probe != 564:
        sys.exit('fix_baked_sizeof: SELF-CHECK FAILED — sizeof(*(MdtWorldID)0) '
                 'measured %r, want 564. The size probe is not measuring '
                 'anything.' % probe)

    # ---- THE SECOND SELF-CHECK, for the pool half. These two numbers are the
    # immediates the SHIPPED 64-bit build passes in `MdtWorldCreate` (0x2b8 and
    # 0x358). If MinGW does not reproduce them then either the probe is broken
    # or these headers do not describe the original's 64-bit layout, and in
    # either case no pool site below should be rewritten on its evidence.
    win = {t: win_elem_size(t, inc, cache) for t in ('MdtBodyID', 'MdtWorldID')}
    pool_ok = win == {'MdtBodyID': 696, 'MdtWorldID': 856}
    if not pool_ok:
        print('  WIN64 CONFIRMATION UNAVAILABLE — measured %r, want '
              "{'MdtBodyID': 696, 'MdtWorldID': 856} (the immediates in the "
              'shipped amd64 MdtWorldCreate).' % win)
        print('     Pool element sizes will be left ALONE rather than rewritten '
              'on one fact.')

    corpus = {}
    for fn in sorted(os.listdir(srcdir)):
        if fn.endswith('.c') and os.path.exists(os.path.join(build, fn[:-2] + '.o')):
            corpus[fn] = open(os.path.join(srcdir, fn), errors='ignore').read()
    member_pools, local_pools = pool_elem_types(corpus)

    fixed = declined = 0
    confirmed = unconfirmed = 0
    pooled = pool_declined = 0
    pool_notes = []
    qsort_fixed = qsort_declined = qsort_same = 0
    field_allocas = 0
    qsort_notes = []
    reasons = {}
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        edits = []
        for m in (() if only_field else ALLOC.finditer(text)):
            raw = re.sub(r'\s+', ' ', m.group('size')).strip()
            scaled = SCALED.match(raw)
            if scaled and scale_unsafe(scaled.group('expr')):
                scaled = None          # `A + B * K`: the K binds to B alone
            lit = 0 if scaled else (int(raw, 0) if re.fullmatch(
                r'0x[0-9a-f]+|\d+', raw) else None)
            # ★ THE DECLARATION MUST COME FROM THIS FUNCTION, NOT THIS FILE.
            # Ghidra names locals `pMVar1` in every function it decompiles, so a
            # file-wide search returns whichever came first. In
            # McdModelPairManager.c `pMVar1` is declared FOUR different ways —
            # McdModelPairManagerHash *, McdModelPairManagerID,
            # McdModelPairManagerLink * — and the file-wide lookup took the one
            # from line 43 for a site at line 87. That is worse than a missed
            # repair: where the wrong type's size happens to equal or divide the
            # literal, the site is rewritten with it, the LP64 size is wrong,
            # and i386 byte-identity STILL PASSES, because the match required
            # the two to coincide there. The gate cannot see this class at all.
            ty = declared_type(region_of(text, m.start()), m.group('var'))
            why = None
            reps = []
            want64 = None       # the constant the shipped amd64 build should pass
            ask_oracle = False  # declared type did not fit; let the oracle try
            if lit is None:
                why = 'size is neither a literal nor count * size'
            elif not scaled and lit < 8:
                why = 'size below 8 bytes'
            elif not ty:
                why = 'target has no declaration in its own function'
            else:
                sz = elem_size(ty, inc, cache)
                if sz is None:
                    why = 'target type %s is not a pointer to a complete type' % ty
                elif scaled is not None:
                    k = int(scaled.group('k'), 0)
                    k = (1 << k) if scaled.group('op') == '<<' else k
                    if k != sz:
                        why = ('%s elements are %d bytes, the scale is %d'
                               % (ty, sz, k))
                    else:
                        reps = spellings(scaled.group('expr'), ty)
                        want64 = 1      # the ELEMENT size; the count is runtime
                elif sz < 2:
                    # `void *` and `MeU8 *` have an element size of 1, so EVERY
                    # literal divides them and the type has told us nothing. A
                    # byte buffer must stay a byte buffer.
                    why = ('%s elements are %d byte, which any literal divides'
                           % (ty, sz))
                elif lit == sz:
                    reps = ['(int)sizeof(*(%s)0)' % ty]
                    want64 = 1
                elif lit % sz == 0:
                    reps = spellings(str(lit // sz), ty)
                    want64 = lit // sz
                else:
                    # The DECLARED type does not fit the literal at all, which
                    # is the same base-vs-derived mistake as the vetoes below in
                    # a louder form: `McdCylinderCreate` asks for 28 bytes and
                    # its target is typed `McdCylinderID`, i.e. `McdGeometry *`,
                    # which is 16. Ask the oracle rather than give up.
                    why = ('%s is %d bytes and does not divide %d'
                           % (ty, sz, lit))
                    ask_oracle = (scaled is None and lit >= 8)
            # ---- THE SECOND FACT, and without it the i386 gate is not enough.
            #
            # Compiling each rewrite and comparing the object proves the CODE
            # SHAPE is unchanged on i386. It proves NOTHING about the type,
            # because the rewrite is only offered when `lit == sizeof_i386(T)`
            # — so ANY type of the right i386 size passes, while giving a
            # different size at LP64, which is the entire point of the change.
            #
            # That is not hypothetical. Before the declaration lookup was made
            # per-function, SEVEN sites were rewritten with the wrong type and
            # all seven passed the byte-identity gate; MeXMLTree.c:285 and :286
            # had `Attribute` and `AttributeNode` swapped with each other.
            #
            # So the type is confirmed against a build nobody here made: the
            # shipped amd64 metoolkit must pass this type's win64 size in this
            # same function. Where that build has no such function the check
            # cannot speak, and the site is counted separately rather than
            # quietly treated as confirmed.
            #
            # ⚠ IT MAY ONLY VETO WHEN THE COUNT IS A COMPILE-TIME CONSTANT.
            # `create(sizeof(T))` and `create(4 * sizeof(T))` reach the
            # allocator as an immediate in any sane codegen, so ABSENCE is
            # evidence. `create(n * sizeof(T))` does not: MSVC strength-reduces
            # a runtime multiply into shifts and scaled LEAs — `n * 48` comes
            # out as `lea (%rax,%rax,2)` then `shl $4` and the number 48 appears
            # nowhere. Vetoing on that rejected five sites that were correct,
            # including McdInit's interaction table. For those the check can
            # only confirm.
            if (not why or ask_oracle) and (want64 is not None or ask_oracle):
                fname = enclosing(text, m.start())
                seen = amd64_constants(fname)
                w = win_elem_size(ty, inc, cache) if want64 is not None else None
                may_veto = scaled is None
                if not pool_ok or seen is None or (want64 is not None and w is None):
                    if not why:
                        unconfirmed += 1
                elif want64 is not None and want64 * w in seen:
                    confirmed += 1
                elif may_veto:
                    # ---- BEFORE VETOING, ASK THE ORACLE WHICH TYPE IT IS.
                    # Four of these are one defect: Ghidra declares the target
                    # with the BASE handle and the code allocates the DERIVED
                    # struct. `McdBoxCreate` reads as `2 * sizeof(McdGeometry)`
                    # — 32 at i386, which is right — while the shipped amd64
                    # build passes 0x30, i.e. `sizeof(McdBox)` at 64-bit. Same
                    # two-build pin the pool sites use: the type whose i386 size
                    # IS the literal and whose 64-bit size this function is seen
                    # to pass.
                    pin = pin_by_size(lit, seen, inc, cache)
                    # ⚠ THE PIN IS OFTEN NOT UNIQUE AND DOES NOT NEED TO BE.
                    # McdBox and McdNull are declared identically, so no size on
                    # any target can separate them; McdNullCreate's candidate
                    # list also picks up MeXMLHandler and _MeStream, which are
                    # the same size by coincidence. The function's own NAME is
                    # the discriminator that costs nothing to check, and a
                    # single name match settles it.
                    if len(pin) > 1:
                        named = [t for t in pin if t.split()[-1] in (fname or '')]
                        if len(named) == 1:
                            pin = named
                    if len(pin) == 1:
                        ty = pin[0] + ' *'
                        reps = ['(int)sizeof(*(%s)0)' % ty]
                        why = None
                        confirmed += 1
                    elif not why:
                        why = ('%s is %d at win64 and the shipped amd64 %s does '
                               'not pass %d%s'
                               % (ty, w, fname, want64 * w,
                                  '; candidates ' + ', '.join(pin) if pin else ''))
                else:
                    unconfirmed += 1
            if why:
                declined += 1
                reasons[why.split(' is ')[0][:40]] = reasons.get(
                    why.split(' is ')[0][:40], 0) + 1
                continue
            edits.append((m.start('size'), m.end('size'), reps, 'alloc', None))

        # ---- the stack allocations whose target is an offsetof-named FIELD.
        # Run before `ALLOCA` so the two cannot both claim a site; they match
        # disjoint statement shapes in any case.
        for m in ALLOCA_INTO_FIELD.finditer(text):
            lit = int(m.group('size'), 0)
            T, F = m.group('T'), m.group('F')
            ty = field_pointee(T, F, inc)
            if not ty:
                declined += 1
                k = 'alloca: %s::%s is not a declared pointer member' % (T, F)
                reasons[k] = reasons.get(k, 0) + 1
                continue
            sz = elem_size(ty, inc, cache)
            if sz != lit:
                declined += 1
                k = ('alloca into %s::%s: %s is %s, the stride is %d'
                     % (T, F, ty, sz, lit))
                reasons[k] = reasons.get(k, 0) + 1
                continue
            edits.append((m.start('size'), m.end('size'),
                          ['(int)sizeof(*(%s)0)' % ty], 'alloc', None))
            field_allocas += 1

        # ---- the stack allocations
        for m in (() if only_field else ALLOCA.finditer(text)):
            lit = int(m.group('size'), 0)
            ty = m.group('cast')
            if ty:
                ty = re.sub(r'\s+', ' ', ty).strip()
            elif m.group('var'):
                ty = declared_type(region_of(text, m.start()), m.group('var'))
            if not ty and m.group('blk'):
                c = re.search(CONSUMER % re.escape(m.group('blk')),
                              region_of(text, m.start()))
                if c:
                    ty = re.sub(r'\s+', ' ', c.group('cast')).strip()
            if not ty:
                declined += 1
                reasons['alloca: no type for the block'] = reasons.get(
                    'alloca: no type for the block', 0) + 1
                continue
            sz = elem_size(ty, inc, cache)
            if sz is None or sz < 2:
                declined += 1
                reasons['alloca: %s elements are not a usable size' % ty] = \
                    reasons.get('alloca: %s elements are not a usable size' % ty, 0) + 1
                continue
            if sz != lit:
                declined += 1
                reasons['alloca: %s is %d, the stride is %d' % (ty, sz, lit)] = \
                    reasons.get('alloca: %s is %d, the stride is %d' % (ty, sz, lit), 0) + 1
                continue
            # The count is a RUNTIME value, so MSVC may strength-reduce the
            # multiply and the shipped amd64 build proves nothing by absence —
            # confirm only, never veto. See the note on `may_veto` above.
            w = win_elem_size(ty, inc, cache)
            seen = amd64_constants(enclosing(text, m.start()))
            if pool_ok and w and seen and w in seen:
                confirmed += 1
            else:
                unconfirmed += 1
            edits.append((m.start('size'), m.end('size'),
                          ['(int)sizeof(*(%s)0)' % ty], 'alloc', None))
            # ⚠ THE ADDEND IS AN ELEMENT TOO, when it equals the stride:
            # `n * 0x10 + 0x10` is "n links, plus one". Scaling the multiplier
            # and leaving the addend gives a block one element short at LP64,
            # which is the same defect with a smaller overrun. Anything else —
            # `+ 0` or a real slack constant — is left alone.
            if m.group('add') is not None and int(m.group('add'), 0) == lit:
                edits.append((m.start('add'), m.end('add'),
                              ['(int)sizeof(*(%s)0)' % ty], 'alloc', None))

        # ---- the pool element strides. Both patterns are matched against the
        # ORIGINAL text and applied together below, back to front, so neither
        # invalidates the other's offsets. They cannot overlap in any case:
        # `ALLOC` requires an assignment target and matches the first argument.
        for m in (() if only_field else POOL_INIT.finditer(text)):
            lit = int(m.group('size'), 0)
            raw_pool = m.group('pool')
            fname = enclosing(text, m.start())
            region = region_of(text, m.start())
            key = pool_key(raw_pool)
            if re.search(r'->|\.', raw_pool):
                cands = member_pools.get(key, set())
                how = 'member %s' % key
            else:
                cands = set(local_pools.get((fn, fname, key), set()))
                how = 'local %s' % key
                # …and the same pool spelled as the member it was just stored
                # into, which is how every getStruct in another object sees it.
                alias = alias_member(region, m.start() - text.index(region), key)
                if alias:
                    cands |= member_pools.get(alias, set())
                    how = 'local %s (= ->%s)' % (key, alias)
            why = note = None
            reps = []
            seen = amd64_constants(fname)
            # ---- FACT 1, the primary: what does getStruct hand back? Kept only
            # if its i386 size IS the stride. `constraintPool` fails that on
            # purpose — getStruct is typed as the base `MdtConstraint` (352) and
            # the pool is sized for the largest variant (492) — and a tool that
            # trusted the primary blindly would have sized the pool for the base
            # class and corrupted every contact.
            typed = [t for t in sorted(cands) if elem_size(t, inc, cache) == lit]
            ty = typed[0] if len(typed) == 1 else None
            src_of = 'getStruct'
            if ty is None and pool_ok and seen:
                # ---- FACT 2, the fallback: the only type whose i386 size is the
                # stride AND whose 64-bit size this function is SEEN to pass in
                # the shipped amd64 build. Two builds nobody here made, agreeing.
                pin = pin_by_size(lit, seen, inc, cache)
                if len(pin) == 1:
                    ty, src_of = pin[0] + ' *', 'i386+win64 pin'
                elif pin:
                    why = ('pool %s: %d bytes is ambiguous — %s all fit both '
                           'builds' % (how, lit, ', '.join(pin)))
                else:
                    why = ('pool %s: no type is %d bytes at i386 with a 64-bit '
                           'size %s passes' % (how, lit, fname))
            if ty is None and why is None:
                why = ('pool %s: getStruct names %s and none is %d bytes'
                       % (how, sorted(cands) or '(nothing)', lit))
            if why:
                pass
            elif not pool_ok:
                why = 'no win64 confirmation available'
            else:
                wsz = win_elem_size(ty, inc, cache)
                if wsz is None:
                    why = 'pool %s: %s has no win64 size' % (how, ty)
                elif seen is None:
                    why = ('pool %s: %s is not in the shipped amd64 build'
                           % (how, fname))
                elif wsz not in seen:
                    why = ('pool %s: %s is %d at win64, which %s does not '
                           'pass' % (how, ty, wsz, fname))
                else:
                    reps = ['(int)sizeof(*(%s)0)' % ty]
                    note = ('%-26s %-26s %4d -> sizeof(%s) via %s '
                            '[win64 %d, and %s passes it]'
                            % (fn, fname, lit, ty, src_of, wsz, fname))
            if why:
                pool_declined += 1
                pool_notes.append('%s %s  DECLINED: %s' % (fn, fname, why))
                continue
            edits.append((m.start('size'), m.end('size'), reps, 'pool', note))

        # ---- the qsort/bsearch element size. Same argument position as the
        # pool form and a stronger fact behind it: the array itself names its
        # element type, so the replacement needs no type resolution at all.
        # `MePoolxInit` is the same fact one argument along; both go through
        # this code because both are "argument three is an element size and
        # another argument names the array".
        matched = set()
        rules = () if only_field else (
            [m for m in QSORT.finditer(text)]
            + [m for m in POOLX_INIT.finditer(text)])
        for m in rules:
            matched.add(m.start())
            lit = int(m.group('size'), 0)
            base = m.group('base')
            rep = 'sizeof(*(%s))' % base
            cand = text[:m.start('size')] + rep + text[m.end('size'):]
            fname = enclosing(text, m.start())
            if not moves_at_lp64(fn, text, cand, inc):
                qsort_same += 1
                qsort_notes.append('%-24s %-28s %s(..., %d, ...) already correct '
                                   'at LP64 — left alone'
                                   % (fn, fname,
                                      m.groupdict().get('call') or 'MePoolxInit',
                                      lit))
                continue
            edits.append((m.start('size'), m.end('size'), [rep], 'qsort',
                          '%-24s %-28s %s(..., %d, ...) -> sizeof(*%s)'
                          % (fn, fname,
                             m.groupdict().get('call') or 'MePoolxInit',
                             lit, base)))
        for m in (() if only_field else QSORT_ANY.finditer(text)):
            if m.start() not in matched:
                qsort_declined += 1
                qsort_notes.append(
                    '%-24s %-28s DECLINED: the array is not a plain identifier, '
                    'so sizeof(*it) cannot name the element'
                    % (fn, enclosing(text, m.start())))

        # ---- APPLY, back to front, VERIFYING EACH. Every accepted edit must
        # leave the object byte-identical at i386; the candidates differ only in
        # how gcc schedules them and which one survives is a property of the
        # surrounding function (see `spellings`). Back to front so that an
        # earlier splice never moves a later match's offsets.
        dirty = 0
        for start, end, reps, kind, note in sorted(edits, key=lambda e: -e[0]):
            for rep in reps:
                cand = text[:start] + rep + text[end:]
                if compiles_identically(fn, cand, build, inc):
                    text = cand
                    dirty = 1
                    if kind == 'alloc':
                        fixed += 1
                    elif kind == 'qsort':
                        qsort_fixed += 1
                        qsort_notes.append(note)
                    else:
                        pooled += 1
                        pool_notes.append(note)
                    break
            else:
                if kind == 'alloc':
                    declined += 1
                    reasons['no spelling reproduces the i386 object'] = reasons.get(
                        'no spelling reproduces the i386 object', 0) + 1
                elif kind == 'qsort':
                    qsort_declined += 1
                    qsort_notes.append('%s  DECLINED: sizeof(*array) does not '
                                       'reproduce the i386 object' % note)
                else:
                    pool_declined += 1
                    pool_notes.append('%s  DECLINED: no spelling of it reproduces '
                                      'the i386 object' % fn)

        if dirty:
            open(path, 'w').write(text)
    print('  allocation sizes rewritten from a literal to sizeof : %d' % fixed)
    print('    …type CONFIRMED against the shipped amd64 build    : %d' % confirmed)
    print('    …not confirmable (that build has no such function) : %d' % unconfirmed)
    print('  declined (reported, not guessed)                    : %d' % declined)
    for r, c in sorted(reasons.items(), key=lambda kv: -kv[1])[:6]:
        print('     %4d  %s' % (c, r))
    print('    …of those, alloca sized by the FIELD it is stored into  : %d'
          % field_allocas)
    print('  qsort/bsearch element sizes rewritten to sizeof(*array): %d'
          % qsort_fixed)
    print('    …already correct at LP64, left alone                : %d' % qsort_same)
    print('    …declined (reported, not guessed)                   : %d' % qsort_declined)
    for note in qsort_notes:
        print('     %s' % note)
    print('  POOL element strides rewritten (arg 3 of MePool init): %d' % pooled)
    print('  pool sites declined                                 : %d' % pool_declined)
    for note in pool_notes:
        print('     %s' % note)
    print('  -> every rewrite above was COMPILED and compared: each one')
    print('     reproduces its baseline i386 object byte for byte. The')
    print('     corpus-wide acceptance test confirms; it no longer discovers.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
