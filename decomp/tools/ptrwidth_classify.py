#!/usr/bin/env python3
"""ptrwidth_classify.py — what the aarch64 truncation warnings actually ARE.

    ptrwidth_classify.py <kd_lp64/allobj> <kd_build> [metoolkit-root]

`ptrwidth_check.sh` counts. It cannot say which of its warnings is a defect, and
the count has been quoted as if every one of them were: "98 truncations across 28
objects" reads as ninety-eight bugs. It is not — roughly a third of them are
diagnostics on conversions that lose nothing, and a tool that cannot tell them
apart makes the number useless in both directions. Nobody can tell whether a drop
from 98 to 94 removed four defects or four false alarms.

So this reads the same three diagnostics and sorts each site into a NAMED class
with a rule behind it. The classes are deliberately narrow and everything that
does not match one lands in `UNEXPLAINED`, which is the number that matters.

THE BENIGN CLASSES, each with the reason it is benign:

  fbits-to-4byte    `*(MeU32 *)(...) = (McdGeometryID)KD_FBITS(x);`
                    A float's four bytes stored into a four-byte slot. The
                    pointer typedef in the middle is a Ghidra artefact — see
                    `KD_FBITS` in kd_compat.h — and the DESTINATION is `MeU32`,
                    four bytes at every width, so nothing widens and nothing
                    truncates. Rule: the assignment target is `*(MeU32 *)`.

  medict-key        `MeDictInsert(d, node, (void *)b->sortKey);`
                    MathEngine's own `MeDict` API takes its key as `void *`
                    (`MeDict.h`), and the engine stores an `MeI32` in it. At LP64
                    that WIDENS — it cannot truncate — and insert and compare go
                    through the identical conversion, so the ordering is
                    unchanged. Rule: the call is an `MeDict*` function.

  int-return        `pvVar6 = (void *)sscanf(buf, "%d\\n", p);`
                    Ghidra kept the return register in a pointer-typed local. The
                    callee returns an int, the value is a count or a status, and
                    nothing dereferences it. Rule: the right-hand side is a call
                    to a function whose recovered prototype returns a non-pointer.

  count-to-id       `((McdTriangleList *)g)->triangleMaxCount = (McdGeometryID)max;`
                    A count converted through an ID typedef into an INT field.
                    Same artefact as `fbits-to-4byte` with a different source.
                    Rule: the assignment target is a named field whose size is 4
                    at both widths.

  fbits-in-local    `pMVar3 = (McdGeometryID)KD_FBITS(dy * 0.5);`
                    ... later ... `*(float *)&(pMVar3)`
                    A float's bits parked in a POINTER-TYPED LOCAL and read back
                    only through `*(float *)&VAR`. ★ An int-to-pointer cast
                    WIDENS and cannot truncate; `KD_FBITS` returns `unsigned
                    int`, so at LP64 the bits sit in the low four bytes — which
                    is exactly where `*(float *)&` looks on a little-endian
                    target, and every target here is little-endian. Rule: the
                    destination is a bare local (not a member, not a deref), and
                    every other use of that local in the function is
                    `*(...)&VAR`. A local that is DEREFERENCED or passed on as a
                    pointer does not qualify and stays UNEXPLAINED.

  int-in-local      `pMVar9 = (MeDictNode *)(pMVar5->jointCount + 1);`
                    ... `pMVar5->jointCount = (kd_iptr)pMVar9;`
                    Integer arithmetic carried in a pointer-typed local and
                    converted straight back. Widen-then-narrow is
                    value-preserving. Rule: the RHS contains no pointer operand,
                    and every use converts back to an integer, indexes an array,
                    or compares against another such value.

⚠ `KD_FBITS` ALONE IS NOT A CLASS, and assuming it was is what this tool exists
to stop. `(MStack_24c.next)->prev = (McdGeometryID)KD_FBITS(...)` in
`IxBoxTriList` uses the same macro and is a REAL defect: `MStack_24c` is a
fabricated `McdGeometry` whose `next` holds a `MeVector3 *`, so `->prev` is
`normal[1]` at i386 and byte 16 — off the end of a three-float vector — at LP64,
written EIGHT bytes wide. The macro says only "a float's bits are going
somewhere"; where they go is the whole question. Those land in UNEXPLAINED, which
is where they belong.

⚠ AND A ZERO IN A CLASS IS ONLY A MEASUREMENT OVER WHAT COMPILED. A file clang
rejects emits no warnings, so this prints the compile failures separately — the
same failure `ptrwidth_check.sh` hid for months.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402
from fix_derived_fields import declared_type                # noqa: E402

HERE = kd_paths.MD

WARN = re.compile(r'^(?P<file>[^:]+):(?P<line>\d+):\d+: warning: (?P<what>.*)$')

# ---- the class rules, in order. Each is (name, statement predicate).
FBITS = re.compile(r'KD_FBITS\s*\(')
FBITS_TO_U32 = re.compile(r'^\s*\(?\*\(MeU32 \*\)')
MEDICT = re.compile(r'\bMeDict\w*\s*\(')
# …and the same key put in a temporary first. `pvVar1 = (void *)b->sortKey;` is
# two lines above the `MeDictInsert` that consumes it, so a rule that only looks
# at the call misses three of the fifteen. The member NAME is the evidence:
# `MdtBody::sortKey` and `MdtConstraint::sortKey` are declared `MeI32` and exist
# for exactly this.
SORTKEY = re.compile(r'\(void \*\)[\w\->.() ]*\bsortKey\b')
# a call whose result is put straight into a pointer-typed local
INT_RETURN_CALLS = (
    'printf', 'sscanf', 'fscanf', 'kd_putchar', 'putchar', 'puts', 'fputs',
    'MeOpen', 'McdCnvVertexGetNeighbor', 'MeFAssetPartIsCollisionEnabled',
    'McdSpaceDisablePair', 'McdFrameworkGetInteractionsWarned',
)
# ⚠ THE CAST SPELLING MUST NOT GATE THIS ANY MORE. While the rule was a name
# list, restricting the cast to `(void|char|int|...) *` was harmless. Now that
# the verdict comes from the CALLEE'S PROTOTYPE, a cast the list does not
# recognise silently suppresses the lookup — which is exactly what kept
# `ppMVar7 = (MdtBody **)MdtKeaMemoryRequired(...)` in UNEXPLAINED while
# `MdtKea.h` declared it `int MEAPI` two directories away.
CALL_RHS = re.compile(r'=\s*\(\s*(?:[A-Za-z_][\w \t]*\*+|(?:Mcd|Mdt|Me|kea)\w*ID)\s*\)\s*'
                      r'(?P<fn>[A-Za-z_]\w*)\s*\(')

# ---- `int-return`, AS THE DOCSTRING ALWAYS DESCRIBED IT --------------------
# ⚠⚠ THIS RULE USED TO BE THE TUPLE ABOVE, AND THE DOCSTRING PROMISED "a
# function whose recovered prototype returns a non-pointer". A hardcoded list of
# eleven names is not that: it cannot answer for the twelfth caller, and it
# quietly leaves real sites in UNEXPLAINED where they look like open defects.
# The prototype IS available — every recovered file declares what it calls in
# its own prelude, and the SDK headers declare the rest — so the rule now READS
# it, which is what makes the class a measurement instead of a memory.
RET = re.compile(r'(?m)^(?P<ret>[A-Za-z_][\w \t]*?[\w \t\*]*?)\b(?:kd_)?%s\s*\(')
_PROTO_CACHE = {}


def _returns_pointer(text, inc, fn):
    """Does `fn` return a pointer? -> True / False / None (not resolvable).

    Looked up in the recovered file's own prelude first — that is the
    *recovered* prototype, and it is the one the compiler saw — then in the SDK
    headers. ⚠ `MEAPI` sits between the return type and the name in the oracle
    (`MeReal * MEAPI McdBoxGetRadii(...)`), so it is stripped rather than being
    allowed to read as the type."""
    if fn in _PROTO_CACHE:
        return _PROTO_CACHE[fn]
    verdict = None
    pat = re.compile(RET.pattern % re.escape(fn))
    for src in (text,) + tuple(_header_texts(inc)):
        m = pat.search(src)
        if not m:
            continue
        ret = re.sub(r'\b(?:MEAPI|MEAPI_INLINE|static|extern|inline|__inline)\b',
                     ' ', m.group('ret')).strip()
        if not ret or ret in ('return', 'else', 'if', 'while', 'for', 'switch'):
            continue
        verdict = '*' in ret
        break
    _PROTO_CACHE[fn] = verdict
    return verdict


_HEADERS = []


def _offsetof(cast, member, inc, bits):
    """offsetof of `((CAST)0)->MEMBER` at `bits`, or None."""
    key = ('off', cast, member, bits)
    if key not in _SIZE_CACHE:
        os.makedirs('/tmp/kd_ptrsz', exist_ok=True)
        src = '/tmp/kd_ptrsz/o%d.c' % bits
        open(src, 'w').write(
            '#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
            '#include "%s/include/kd_types.h"\n'
            'char kd_probe[((char *)&((%s)0)->%s - (char *)0) + 1];\n'
            'int kd_force = &kd_probe;\n' % (HERE, HERE, HERE, cast, member))
        r = subprocess.run(['gcc', '-m%d' % bits, '-DLINUX'] + includes(inc)
                           + ['-w', '-c', '-o', os.devnull, src],
                           capture_output=True, text=True)
        m = _FIELD_SZ.search(r.stderr)
        _SIZE_CACHE[key] = (int(m.group(1)) - 1) if m else None
    return _SIZE_CACHE[key]


def _int_return_safe(region, var, inc):
    """The OTHER HALF of this class's own justification — "and nothing
    dereferences it" — which the rule never checked.

    ⚠ WITHOUT THIS, THE CLASS MOVES A POTENTIAL DEFECT INTO A BENIGN BUCKET,
    which is the one direction this tool must never fail in. A count parked in a
    pointer local is harmless only while it stays a number.

    ★ `&VAR->M` WHERE M IS AT OFFSET 0 AT BOTH WIDTHS IS THE IDENTITY, NOT A
    DEREFERENCE, and that is measured rather than assumed. `MdtPartition` writes
    `pMVar5->rowCount = (kd_iptr)&pMVar9->left + pMVar5->rowCount;` where `left`
    is `MeDictNode`'s first member — so the expression is the count itself. Had
    it been `&pMVar9->right`, 4 at i386 and 8 at LP64, this returns False and
    the site stays open."""
    ty = declared_type(region, var)
    for m in re.finditer(r'\b%s\b' % re.escape(var), region):
        tail = region[m.end():m.end() + 2]
        head = region[max(0, m.start() - 8):m.start()]
        if tail == '->':
            mem = re.match(r'->\s*(\w+)', region[m.end():])
            if not (mem and re.search(r'&\s*$', head) and ty
                    and _offsetof(ty.strip(), mem.group(1), inc, 32) == 0
                    and _offsetof(ty.strip(), mem.group(1), inc, 64) == 0):
                return False
        elif tail.startswith('['):
            return False
    return True


def _header_texts(inc):
    if not _HEADERS:
        for dirpath, _d, files in os.walk(inc):
            for f in sorted(files):
                if f.endswith('.h'):
                    _HEADERS.append(
                        open(os.path.join(dirpath, f), errors='ignore').read())
    return _HEADERS

# `X->field = (SomeID)expr;` / `((T *)p)->field = (SomeID)expr;`
COUNT_TO_ID = re.compile(r'->(?P<f>\w+)\s*=\s*\((?:Mcd|Mdt|Me)\w*ID\)\s*'
                         r'(?!KD_FBITS)')

# ---- `count-to-id`, AS THE DOCSTRING ALWAYS DESCRIBED IT -------------------
# ⚠⚠ THE RULE ABOVE IS A PATTERN AND THE DOCSTRING PROMISES A MEASUREMENT —
# "the assignment target is a named field whose size is 4 at both widths". It
# does not check any size, and `->(\w+)\s*=` cannot even parse a DOTTED path, so
# `((McdConvexMesh *)pMVar1)->mHull.numFace = (McdGeometryID)poly->numFace;`
# never matched and four sites sat in UNEXPLAINED reading like open defects.
# This is the promised rule: resolve the destination to a struct and a member
# path, then ask the compiler for `sizeof` at BOTH widths.
DEST = re.compile(
    r'(?:\(\s*\(\s*(?P<tag>\w+)\s*\*\s*\)\s*(?P<cv>[A-Za-z_]\w*)\s*\)|(?P<v>[A-Za-z_]\w*))'
    r'\s*->\s*(?P<path>\w+(?:\s*\.\s*\w+)*)\s*=\s*\((?:Mcd|Mdt|Me)\w*ID\)\s*(?!KD_FBITS)')
_FIELD_SZ = re.compile(r'char \(\*\)\[(\d+)\]')
_SIZE_CACHE = {}


def _field_size(cast, path, inc, bits):
    """sizeof of `((CAST)0)->PATH` at `bits`, or None if it will not compile."""
    key = (cast, path, bits)
    if key not in _SIZE_CACHE:
        os.makedirs('/tmp/kd_ptrsz', exist_ok=True)
        src = '/tmp/kd_ptrsz/p%d.c' % bits
        open(src, 'w').write(
            '#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
            '#include "%s/include/kd_types.h"\n'
            'char kd_probe[sizeof(((%s)0)->%s) + 1];\nint kd_force = &kd_probe;\n'
            % (HERE, HERE, HERE, cast, path))
        r = subprocess.run(['gcc', '-m%d' % bits, '-DLINUX'] + includes(inc)
                           + ['-w', '-c', '-o', os.devnull, src],
                           capture_output=True, text=True)
        m = _FIELD_SZ.search(r.stderr)
        _SIZE_CACHE[key] = (int(m.group(1)) - 1) if m else None
    return _SIZE_CACHE[key]


def _count_to_id(stmt, region, inc):
    """The measured form of the rule. -> True when the destination member is
    four bytes at BOTH widths, i.e. nothing can widen or truncate into it.

    ⚠ The cast is used VERBATIM rather than reduced to a struct tag, because the
    corpus addresses these objects through POINTER TYPEDEFS (`McdGeometryID`)
    as often as through a struct name, and `(McdGeometryID *)` would be a
    pointer-to-pointer and measure the wrong thing."""
    m = DEST.search(stmt)
    if not m:
        return False
    if m.group('tag'):
        cast = '%s *' % m.group('tag')
    else:
        ty = declared_type(region, m.group('v')) if region else None
        if not ty:
            return False
        cast = ty.strip()
    path = re.sub(r'\s+', '', m.group('path'))
    return (_field_size(cast, path, inc, 32) == 4
            and _field_size(cast, path, inc, 64) == 4)


# A bare local as the destination: `NAME = ...;` with no `->`, `.`, `[` or `*` on the LEFT.
# ⚠ THE CAST IS NOT PART OF THE PATTERN. Written to require `(TYPE *)` this matched nothing at
# all, because the corpus casts through POINTER TYPEDEFS — `(McdGeometryID)`, `(MdtBodyID)` —
# which carry their own `*`. The destination being pointer-typed is not in doubt here anyway:
# clang has already raised an int-to-pointer diagnostic on this very line, which is why the
# classifier is looking at it.
BARE_LOCAL = re.compile(r'^\s*(?P<v>[A-Za-z_]\w*)\s*=\s*(?![=])')
# Every use of a local that keeps it an integer or reinterprets its storage.
def _uses_are_value_only(region, var):
    """Is `var` only ever converted back, indexed with, or reinterpreted — never
    dereferenced and never passed on as a pointer?

    ⚠ THE CONSERVATIVE DIRECTION IS UNEXPLAINED. This is the difference between
    "the diagnostic is noise" and "the diagnostic is the defect", and
    `IxBoxTriList`'s `(MStack_24c.next)->prev` — which IS a defect and uses the
    very same `KD_FBITS` macro — must not be swept up by it."""
    for m in re.finditer(r'\b%s\b' % re.escape(var), region):
        tail = region[m.end():m.end() + 3]
        head = region[max(0, m.start() - 24):m.start()]
        if tail.startswith('->') or tail.startswith('['):
            return False                       # dereferenced or indexed THROUGH
        if re.search(r'\*\s*$', head) and not re.search(r'\)\s*&\s*$', head):
            return False                       # `*var`
        if re.search(r'\w\s*\(\s*$|,\s*$', head) and not re.search(r'\(\s*(?:kd_[iu]ptr|int|uint)\s*\)\s*$', head):
            return False                       # handed to a call as-is
    return True


CLASSES = ('fbits-to-4byte', 'fbits-in-local', 'medict-key', 'int-return',
           'count-to-id', 'int-in-local', 'UNEXPLAINED')


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


BANNER = re.compile(r'(?m)^/\* ---- (\S+)')


def region_of(text, line_index):
    """The `/* ---- name ---- */` block containing this LINE, as text.

    The "every use of this local" rules are statements about one FUNCTION; a
    whole-file scan would let an unrelated function's use of the same Ghidra
    name (`pMVar3` appears in most of them) decide the verdict."""
    pos = sum(len(l) + 1 for l in text.split('\n')[:line_index])
    start, end = 0, len(text)
    for m in BANNER.finditer(text):
        if m.start() <= pos:
            start = m.start()
        elif m.start() > pos:
            end = m.start()
            break
    return text[start:end]


def statement(lines, i):
    """The whole statement the warning sits in — Ghidra wraps long ones, and a
    rule applied to half a statement is a rule applied to the wrong thing."""
    j = i
    while j > 0 and ';' not in lines[j - 1] and '{' not in lines[j - 1] \
            and '}' not in lines[j - 1] and j > i - 4:
        j -= 1
    k = i
    while k < len(lines) - 1 and ';' not in lines[k] and k < i + 4:
        k += 1
    return ' '.join(x.strip() for x in lines[j:k + 1])


NEAR = []          # near-miss reasons for the site being classified, cleared per site


def classify(stmt, line, region='', what='', text='', inc=''):
    if FBITS.search(stmt) and FBITS_TO_U32.match(line):
        return 'fbits-to-4byte'
    if MEDICT.search(stmt) or SORTKEY.search(stmt):
        return 'medict-key'
    m = CALL_RHS.search(stmt)
    if m:
        # ★ THE PROTOTYPE, NOT A NAME LIST. `_returns_pointer` returns None when
        # it cannot find a declaration, and None is NOT "benign" — an
        # unresolvable callee stays UNEXPLAINED, which is the conservative
        # direction and the whole point of the class.
        int_ret = (_returns_pointer(text, inc, m.group('fn')) is False
                   or m.group('fn') in INT_RETURN_CALLS)
        if int_ret:
            dest = BARE_LOCAL.match(line)
            if not dest or not region or not inc:
                return 'int-return'
            if _int_return_safe(region, dest.group('v'), inc):
                return 'int-return'
            # ⚠ NEAR-MISS, AND SAYING SO IS THE POINT. The callee does return an
            # int, but the local is dereferenced elsewhere in the function —
            # Ghidra reuses one name for two live ranges (`keaDebug`'s
            # `piVar15` is `printf`'s return here and a real `int *` sixty lines
            # down). A whole-function rule cannot separate those, so the site
            # stays open with the reason attached rather than being counted
            # benign on half the evidence.
            NEAR.append('%s is dereferenced elsewhere in the function; '
                        '%s() does return an int' % (dest.group('v'), m.group('fn')))
    b = BARE_LOCAL.match(line)
    if b and region and _uses_are_value_only(region, b.group('v')):
        if FBITS.search(stmt):
            return 'fbits-in-local'
        # ★ ASK THE DIAGNOSTIC WHICH DIRECTION IT IS, rather than inspecting the
        # right-hand side for things that look like addresses. `-Wint-to-pointer-cast`
        # means an INTEGER became a pointer — that is a WIDENING and cannot lose a
        # bit. The first version tested the RHS for `&`, `->` and `[` and rejected
        # `(MeDictNode *)(pMVar5->jointCount + 1)`, which is the commonest shape of
        # the class: `->` there is a member READ, not an address.
        if 'int-to-pointer' in what:
            return 'int-in-local'
    if COUNT_TO_ID.search(stmt) and not FBITS.search(stmt):
        return 'count-to-id'
    if inc and _count_to_id(stmt, region, inc):
        return 'count-to-id'
    return 'UNEXPLAINED'


def main():
    srcdir = sys.argv[1]
    build = sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')
    # ⚠ THE SHIM, NOT `..` ARITHMETIC OR A PINNED PATH. `kd_paths.NDK_BIN`
    # discovers the toolchain from the standard NDK variables; a pinned r30-beta
    # path stopped existing on an upgrade once and the failure read as a
    # compiler bug.
    cc = os.path.join(kd_paths.NDK_BIN, 'aarch64-linux-android21-clang')
    if not kd_paths.NDK_BIN or not os.path.exists(cc):
        sys.exit('ptrwidth_classify: no aarch64 clang at %r — set KD_NDK.' % cc)

    cf = ['-O2', '-fno-strict-aliasing', '-std=gnu99', '-DLINUX',
          '-I' + os.path.join(HERE, 'include')] + includes(inc) + [
        '-Wno-everything', '-Wint-to-pointer-cast', '-Wpointer-to-int-cast',
        '-Wvoid-pointer-to-int-cast', '-ferror-limit=0']

    counts = {c: 0 for c in CLASSES}
    per_obj = {}
    unexplained = []
    broke = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        r = subprocess.run([cc] + cf + ['-c', '-o', os.devnull, path],
                           capture_output=True, text=True)
        if r.returncode:
            broke.append(fn)
            continue
        lines = open(path, errors='ignore').read().splitlines()
        whole = '\n'.join(lines)
        for ln in r.stderr.splitlines():
            m = WARN.match(ln)
            if not m:
                continue
            i = int(m.group('line')) - 1
            if i >= len(lines):
                continue
            del NEAR[:]
            cls = classify(statement(lines, i), lines[i],
                           region_of(whole, i), m.group('what'), whole, inc)
            near = ('   [near-miss: %s]' % NEAR[0]) if NEAR else ''
            counts[cls] += 1
            per_obj.setdefault(fn[:-2], {}).setdefault(cls, 0)
            per_obj[fn[:-2]][cls] += 1
            if cls == 'UNEXPLAINED':
                unexplained.append('%-26s %-5d %s%s'
                                   % (fn[:-2], i + 1, lines[i].strip()[:104], near))

    total = sum(counts.values())
    print('  aarch64 truncation diagnostics: %d across %d object(s)'
          % (total, len(per_obj)))
    for c in CLASSES:
        print('    %-16s %4d%s' % (c, counts[c],
                                   '   <- the ones that are open' if c == 'UNEXPLAINED' else ''))
    if broke:
        # ⚠ A FILE THAT DOES NOT COMPILE EMITS NO WARNINGS. Without this line a
        # target failing on every object reads exactly like a clean one.
        print('    %d object(s) DID NOT COMPILE — NOT MEASURED: %s'
              % (len(broke), ' '.join(broke)))
    print('  worst objects by UNEXPLAINED:')
    for o, d in sorted(per_obj.items(), key=lambda kv: -kv[1].get('UNEXPLAINED', 0))[:8]:
        if d.get('UNEXPLAINED'):
            print('    %-26s %d of %d' % (o, d['UNEXPLAINED'], sum(d.values())))
    print('  every UNEXPLAINED site:')
    for u in unexplained:
        print('    ' + u)
    return 1 if broke else 0


if __name__ == '__main__':
    sys.exit(main())
