#!/usr/bin/env python3
"""fix_callback_context.py — a callback's `void *` context, read at i386 offsets.

    fix_callback_context.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT, and ⚠⚠ **NO STATIC GATE IN THIS PROJECT CAN SEE IT.** A callback
takes its state as a `void *`, so Ghidra has no type for it and addresses every
field by a baked byte offset:

    static MeDictNode *MePoolxDictNodeAllocate(void *pool)
    {
      if (*(int *)((kd_iptr)pool + 0xc) != 0) {                 /* numfree */
        pMVar2 = (MeDictNode *)(*(kd_iptr *)pool +
                                *(int *)((kd_iptr)pool + 0x10) * 4);   /* ifree */
        ...
        *(MeDictNode **)((kd_iptr)pool + 0x10) = pMVar2->left;
      }
    }

```
MePoolx   i386  mem 0  isize 4  numrec 8   numfree 12  ifree 16   sizeof 20
          LP64  mem 0  isize 8  numrec 12  numfree 16  ifree 20   sizeof 24
```

Every member after `mem` moves, so at LP64 `0xc` is `numrec`, `0x10` is
`numfree` and `4` is the **high half of `mem`**. Nothing is truncated and no
cast is narrowed, so `ptrwidth_check` and `ptrwidth_classify` report this file
as clean — it appears on neither the 91 diagnostics nor the 52 open ones. Only
running the game finds it.

★ THE TOMBSTONE IS THE DEFECT, FIELD BY FIELD. gdb on `ONS-Torlan`, at the
`SIGSEGV` in `MeDictInsert` ← `MeSetAdd` ← `McdConvexMeshPlaneCut` — a convex
mesh being sliced, i.e. a **vehicle**:

    nodemem      = 0x7ffffffc66d0                 (MeDictNode[200], 8-aligned)
    set.nodepool = { mem = 0x7ffffffc66d0, isize = 12,
                     numrec = 196, numfree = 32767, ifree = -235024 }

`numrec` should be 200 and is 196 — decremented four times by the code that
means `numfree`. `numfree` should be 200 and is **32767**, the high half of a
stack address, because `*(MeDictNode **)(pool + 0x10)` stores EIGHT bytes into
a four-byte index. The node the pool then hands back is `0x…cc`, not 8-aligned,
which a `MeDictNode` can never be.

THE TYPE IS DECLARED — IN THE REGISTRATION, and that is the same evidence chain
`interaction_types` uses:

    void kd_MePoolxUseWithDict(MePoolx *p, MeDict *d)
    { MeDictSetAllocator(d, MePoolxDictNodeAllocate, MePoolxDictNodeDeallocate, p); }

The function's address and its context are passed in one call, so the candidate
types are the declared types of that call's pointer arguments. ⚠ THERE ARE TWO
OF THEM AND POSITION DOES NOT SAY WHICH — `d` is an `MeDict *`. The tie-break is
a MEASUREMENT: every offset used against the parameter must land on a top-level
member START of the candidate at i386. `MeDict`'s members are 0, 24, 28, 32, 36,
40, 44, 48, so `4` lands inside `nilnode` and it is refused. Exactly one
candidate must fit or the site is declined.

THE REPAIR IS AN ADDRESS RE-SPELLING, PLUS A WIDTH WHERE THE MEMBER DEMANDS ONE:

    *(int *)((kd_iptr)pool + 0xc)          ->  ... + KD_OFF(MePoolx, numfree)
    *(MeDictNode **)((kd_iptr)pool + 0x10) ->  *(int *)(... + KD_OFF(MePoolx, ifree))

⚠⚠ AND THE MEMBER SPELLING — `((MePoolx *)pool)->numfree` — IS **NOT**
BYTE-IDENTICAL, WHICH IS WHY THIS RE-SPELLS THE ADDRESS INSTEAD. Measured:
`numfree` alone is identical, `ifree` alone is identical, and the two TOGETHER
are not — gcc schedules the member form differently once both loads are visible
as fields of one object. A typed local (`MePoolx *p = pool;`) does not rescue it
either. The address form keeps every expression's TYPE exactly as it was and
changes only how the address is computed, which is `fix_literal_offsets`' own
rule and the reason it survives the gate.

RULE D — THE POOL'S OWN MEMORY IS DECLARED, SO THE LINK IN IT IS TOO.
`MePoolx::mem` is `int *`, so a record handed out of it holds a four-byte
free-list index in its first word. Ghidra typed the record `MeDictNode *` and
read `->left`, which is eight bytes at LP64:

    if (pMVar2->left == (MeDictNode *)0xffffffff)   ->  *(int *)pMVar2 == (int)0xffffffff
    *(...)(pool + 0x10) = pMVar2->left;             ->  ... = *(int *)pMVar2;

Only offset ZERO, only where the member read is 4 bytes here and 8 there, and
only for a local whose initialiser comes off the context's own scalar-array
member. A constant compared against a re-spelled site is cast to the new type so
`0xffffffff` stays `-1` at both widths rather than becoming 4294967295.

⚠ RUN IT ON A COPY: it edits in place, after `fix_ptrwidth` (which writes the
`kd_iptr` these sites are spelled with).
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402
import fix_literal_offsets as flo                           # noqa: E402

HERE = kd_paths.MD
WORK = '/tmp/kd_cbctx'
CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']
HEAD = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
        '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))
_SZ = re.compile(r'char \(\*\)\[(\d+)\]')
BANNER = re.compile(r'(?m)^/\* ---- (\S+)')

# `static <ret> NAME(<params>)` — the callbacks this is about are file-local.
#
# ⚠ THE LOOKBEHIND IS LOAD-BEARING, AND THIS PROJECT HAS BEEN BITTEN BY EXACTLY
# THIS BEFORE (`fix_frame_slots`, `aiStack_9cb0` captured as `iStack_9cb0`).
# `[\w ]*` is greedy, so `static void MePoolxDictNodeDeallocate(` yields a name
# of **`e`** — the return type eats everything up to the last character. The
# sibling `static MeDictNode * MePoolxDictNodeAllocate(` survives only because
# the `*` forces the split, so one of the two callbacks was typed and the other
# silently was not, and the tool reported no decline for the one it dropped.
STATIC_FN = re.compile(r'(?m)^static\s+[A-Za-z_][\w ]*\**\s*(?<![\w])'
                       r'(?P<name>\w+)\s*\((?P<params>[^)]*)\)')
# a site against the context: `*(TY *)((kd_iptr)pool + 0xc)`, and the K == 0 form.
def site_re(var):
    return re.compile(r'\*\((?P<ty>[A-Za-z_][\w *]*?)[ \t]*\*\)[ \t]*\(?[ \t]*'
                      r'(?:\((?:kd_iptr|kd_uptr|int|uint|char)[ \t]*\*?\)[ \t]*)?'
                      r'(?<![\w.])' + re.escape(var) + r'(?:[ \t]*\+[ \t]*'
                      r'(?P<off>0x[0-9a-fA-F]+|\d+))?[ \t]*\)?')
# what a mis-sized access becomes: the member's own width, same signedness.
NARROW = {'int': 'int', 'MeI32': 'int', 'uint': 'uint', 'MeU32': 'uint',
          'undefined4': 'undefined4'}
INC_G = None
CACHE = {}


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def measure(expr, bits='-m32'):
    key = (expr, bits)
    if key in CACHE:
        return CACHE[key]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p.c')
    open(src, 'w').write(HEAD + 'char kd_probe[%s];\nint kd_force = &kd_probe;\n' % expr)
    r = subprocess.run(['gcc', bits, '-DLINUX'] + includes(INC_G)
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    m = _SZ.search(r.stderr)
    CACHE[key] = int(m.group(1)) if m else None
    return CACHE[key]


def off_expr(T, F):
    return '((int)((char *)&((%s *)0)->%s - (char *)0))' % (T, F)


def fields_of(T):
    """{i386 offset -> member name}, top level only, from the type database."""
    import json
    tf = os.path.join(HERE, 'include', 'kd_types_fields.json')
    db = json.load(open(tf)) if os.path.exists(tf) else {}
    d = db.get(T) or db.get('_' + T) or {}
    return {int(k): v for k, v in d.items()}


def regions(text):
    marks = [(m.group(1), m.start()) for m in BANNER.finditer(text)]
    for i, (name, s) in enumerate(marks):
        yield name, s, (marks[i + 1][1] if i + 1 < len(marks) else len(text))


def compiles_identically(fn, text, build, inc):
    """⚠ THE FILE MUST KEEP ITS NAME — gcc records the basename in STT_FILE."""
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


def registrations(text):
    """{static callback name -> [candidate struct tags]}, read off the calls that
    install it. The candidates are the declared types of the call's other
    pointer arguments; which one is the context is decided by measurement."""
    statics = {}
    for m in STATIC_FN.finditer(text):
        params = [p.strip() for p in m.group('params').split(',') if p.strip()]
        voids = [p for p in params if re.fullmatch(r'void\s*\*\s*\w+', p)]
        if len(voids) == 1:
            statics[m.group('name')] = voids[0].split('*')[-1].strip()
    if not statics:
        return {}
    out = {}
    for call in re.finditer(r'(?<![\w>])(\w+)\s*\(([^();]*)\)\s*;', text):
        args = [a.strip() for a in call.group(2).split(',')]
        named = [a for a in args if a in statics]
        if not named:
            continue
        cands = []
        for a in args:
            # ★ THE CONTEXT IS USUALLY PASSED BY ADDRESS, and requiring a bare
            # identifier missed every one of those. `MdtBodyForAllConstraints(
            # pMVar1, transferContactGroups, &bp)` hands a STACK STRUCT as the
            # `void *`, which is the commonest shape there is — and because the
            # rule never saw the call, `transferContactGroups` reading `BodyData`
            # at i386 offsets (+4 `newBody`, +8 `model`, +0xc `space`, which are
            # +8/+16/+24 at LP64) was not merely unrepaired, it was never
            # reported. The declared type of a `&VAR` argument is `VAR`'s own,
            # taken from its VALUE declaration rather than a pointer one.
            amp = re.fullmatch(r'&\s*([A-Za-z_]\w*)', a)
            if amp:
                d = re.search(r'(?<![\w])([A-Za-z_]\w*)\s+%s\s*;'
                              % re.escape(amp.group(1)), text)
                if d and d.group(1) not in ('void', 'char', 'unsigned', 'return',
                                            'struct', 'const', 'static'):
                    cands.append(d.group(1))
                continue
            if not re.fullmatch(r'[A-Za-z_]\w*', a) or a in statics:
                continue
            d = re.search(r'(?<![\w])([A-Za-z_]\w*)\s*\*\s*(?:const\s+)?%s\b'
                          % re.escape(a), text)
            if d and d.group(1) not in ('void', 'char', 'unsigned'):
                cands.append(d.group(1))
        for n in named:
            out.setdefault(n, []).extend(cands)
    return {k: (v, statics[k]) for k, v in out.items() if v}


def accessor_scopes(text, root):
    """★ A SECOND EVIDENCE SOURCE: a `void *` LOCAL TYPED BY THE ACCESSOR IT WAS
    ASSIGNED FROM. The oracle declares the return type, so nothing is inferred:

        void *pvVar13;
        pvVar13 = McdModelGetGeometry(model);                 /* McdGeometry *  */
        ...
        pvVar13 = McdConvexMeshGetPolyhedron(pvVar13);        /* McdConvexHull *  <- */
        if (0 < *(int *)((kd_iptr)pvVar13 + 0x14)) {          /* numFace          */
          pfVar16 = (float *)(iVar17 * 0x10 + *(int *)((kd_iptr)pvVar13 + 4));  /* face */

    ```
    McdConvexHull  i386  vertex 0  face 4   edge 8   edgeIndex 12  numVertex 16  numFace 20
                   LP64  vertex 0  face 8   edge 16  edgeIndex 24  numVertex 32  numFace 36
    ```

    So at LP64 `+4` is the HIGH HALF OF `vertex` — read four bytes wide and used as
    the base of the face array — and `+0x14` is the high half of `edge`.
    ⚠ `sizeof(McdCnvFace)` is 16 at BOTH widths (three floats and an int), so the
    `* 0x10` stride is correct and nothing about it looks wrong. `IxConvexLineSegment`
    is the convex-mesh LINE-SEGMENT intersection — the path a hover vehicle's
    repulsor line checks take — and it is the only body that diverges between the
    two widths on `ONS-Torlan` once the FP model is held fixed.

    ⚠⚠ THE SCOPE IS THE ASSIGNMENT, NOT THE FUNCTION. `pvVar13` above holds an
    `McdGeometry *` for twenty-five lines and an `McdConvexHull *` afterwards.
    Typing the whole function from one assignment would re-spell the earlier uses
    against the wrong struct, and at i386 several of those offsets exist in both —
    so the byte-identity gate would not necessarily catch it. Each assignment types
    only the sites between it and the next assignment to the same name.

    Returns [(var, T, start, end)] in text coordinates."""
    import fix_literal_offsets as _flo
    bodies = _flo.struct_bodies(INC_G)
    protos = {}
    for dp, _, files in os.walk(INC_G):
        for f in files:
            if not f.endswith('.h'):
                continue
            src = re.sub(r'/\*.*?\*/', ' ',
                         open(os.path.join(dp, f), errors='ignore').read(), flags=re.S)
            # `const McdConvexHull * MEAPI McdConvexMeshGetPolyhedron(McdConvexMeshID);`
            for m in re.finditer(r'(?<![\w])(?:const\s+)?([A-Za-z_]\w*)\s*\*\s*'
                                 r'(?:MEAPI\s+)?([A-Za-z_]\w*)\s*\([^;]*\)\s*;', src):
                ty, name = m.group(1), m.group(2)
                if ty in ('void', 'char', 'struct', 'unsigned', 'const'):
                    continue
                if ty in bodies or '_' + ty in bodies:
                    protos.setdefault(name, ty)
    out = []
    for func, s, e in regions(text):
        body = text[s:e]
        voids = set(re.findall(r'(?m)^\s*void\s*\*\s*(\w+)\s*;', body))
        if not voids:
            continue
        for v in sorted(voids):
            asg = [(m.start(), m.group('fn')) for m in re.finditer(
                r'(?m)^\s*%s\s*=\s*(?:\([^()]*\)\s*)?(?P<fn>[A-Za-z_]\w*)\s*\(' % re.escape(v),
                body)]
            for i, (pos, callee) in enumerate(asg):
                T = protos.get(re.sub(r'^kd_', '', callee)) or protos.get(callee)
                if not T:
                    continue
                stop = asg[i + 1][0] if i + 1 < len(asg) else len(body)
                out.append((v, T, s + pos, s + stop))
    return out


def plan(text, notes, fn, quiet=False):
    edits, label = [], []
    regs = registrations(text)
    scopes = accessor_scopes(text, None)
    if not regs and not scopes:
        return None
    # ---- (a) the registration form: a callback's `void *` context.
    for func, s, e in regions(text):
        base = func.split('(')[0]
        if base not in regs:
            continue
        cands, param = regs[base]
        body = text[s:e]
        sites = list(site_re(param).finditer(body))
        if not sites:
            continue
        offs = sorted({int(m.group('off'), 0) if m.group('off') else 0
                       for m in sites})
        # ---- WHICH CANDIDATE. Every offset must land on a top-level member
        # START at i386. `MeDict` has no member at 4, so it is refused; exactly
        # one survivor or the site is declined rather than guessed at.
        fit = []
        for T in dict.fromkeys(cands):
            flds = fields_of(T)
            if flds and all(o in flds for o in offs):
                fit.append(T)
        if len(fit) != 1:
            if not quiet:
                notes.append('%-22s %s: %d candidate type(s) fit %s — declined'
                             % (fn, base, len(fit),
                                ' '.join(hex(o) for o in offs)))
            continue
        T = fit[0]
        flds = fields_of(T)
        edits += respell(text, s, sites, T, flds)
        label.append('%s(%s: %s)' % (base, param, T))
        edits += rule_d(text, s, e, param, T, flds)
    # ---- (b) the accessor form, scoped to the assignment that types it.
    for v, T, a, b in scopes:
        flds = fields_of(T)
        if not flds:
            continue
        sites = [m for m in site_re(v).finditer(text[a:b])]
        if not sites:
            continue
        offs = sorted({int(m.group('off'), 0) if m.group('off') else 0 for m in sites})
        if not all(o in flds for o in offs):
            if not quiet:
                notes.append('%-22s %s as %s: offsets %s are not member starts — declined'
                             % (fn, v, T, ' '.join(hex(o) for o in offs)))
            continue
        # ★ ONLY REWRITE WHAT MOVES. `McdCnvFace` is 16 bytes at both widths, so a
        # stride over it is already right; only offsets whose member actually
        # relocates are touched, and a scope where none does is left alone.
        if not any(measure(off_expr(T, flds[o])) != measure(off_expr(T, flds[o]), '-m64')
                   for o in offs):
            continue
        new = respell(text, a, sites, T, flds)
        if new:
            edits += new
            label.append('%s(%s: %s)' % (v, 'accessor', T))
    if not edits:
        return None
    edits = list({(a, b): (a, b, r) for a, b, r in edits}.values())
    return edits, ' '.join(label)


def respell(text, base, sites, T, flds):
    """The address, always; the access width only where the member demands it."""
    edits = []
    for m in sites:
        k = int(m.group('off'), 0) if m.group('off') else 0
        F, ty = flds[k], m.group('ty').strip()
        if m.group('off') is not None:
            edits.append((base + m.start('off'), base + m.end('off'), off_expr(T, F)))
        a = measure('sizeof(((%s *)0)->%s)' % (T, F))
        b = measure('sizeof(((%s *)0)->%s)' % (T, F), '-m64')
        w = measure('sizeof(%s)' % ty, '-m64')
        if None in (a, b, w) or w == b:
            continue
        if a != measure('sizeof(%s)' % ty):
            continue                        # already disagreed at i386 — not ours
        edits.append((base + m.start('ty'), base + m.end('ty'),
                      'int' if b == 4 else 'kd_iptr'))
    return edits


def rule_d(text, s, e, param, T, flds):
    """The record handed out of a `SCALAR *` member is that scalar, not the
    struct Ghidra typed the cursor as. See the module docstring."""
    body = text[s:e]
    scalar = [F for o, F in sorted(flds.items())
              if re.fullmatch(r'(?:int|MeI32|uint|MeU32)',
                              (measure_type(T, F) or '').replace('*', '').strip())
              and (measure_type(T, F) or '').endswith('*')]
    if not scalar:
        return []
    E = (measure_type(T, scalar[0]) or '').replace('*', '').strip()
    out = []
    for d in re.finditer(r'(?m)^\s*(?P<v>\w+)\s*=\s*\((?P<cast>[A-Za-z_][\w ]*)\*\)\s*'
                         r'\((?P<init>[^;]*%s[^;]*)\)\s*;' % re.escape(param), body):
        v, cast = d.group('v'), d.group('cast').strip()
        for m in re.finditer(r'(?<![\w.])%s->(\w+)\b' % re.escape(v), body):
            F = m.group(1)
            if measure('((int)((char *)&((%s *)0)->%s - (char *)0))' % (cast, F)) != 0:
                continue
            a = measure('sizeof(((%s *)0)->%s)' % (cast, F))
            b = measure('sizeof(((%s *)0)->%s)' % (cast, F), '-m64')
            if not (a == 4 and b == 8):
                continue
            out.append((s + m.start(), s + m.end(), '*(%s *)%s' % (E, v)))
            # a constant on the other side of a comparison follows the new type
            tail = body[m.end():m.end() + 60]
            c = re.match(r'\s*==\s*\((?P<t>[A-Za-z_][\w ]*)\*\)(?P<k>0x[0-9a-fA-F]+)',
                         tail)
            if c:
                out.append((s + m.end() + c.start('t') - 1,
                            s + m.end() + c.end('k'),
                            '(%s)%s' % (E, c.group('k'))))
    return out


_TYPECACHE = {}


def measure_type(T, F):
    """The DECLARED type of `T::F`, out of the oracle."""
    key = (T, F)
    if key in _TYPECACHE:
        return _TYPECACHE[key]
    bodies = flo.struct_bodies(INC_G)
    b = bodies.get(T) or bodies.get('_' + T)
    r = None
    if b:
        b = re.sub(r'/\*.*?\*/', ' ', b, flags=re.S)
        m = re.search(r'(?<![\w])([A-Za-z_]\w*)\s*(\**)\s*%s\s*[;,\[]' % re.escape(F), b)
        if m:
            r = (m.group(1) + ' ' + m.group(2)).strip()
    _TYPECACHE[key] = r
    return r


def selftest():
    """A matcher that cannot see the site reports no decline either — so the two
    spellings that actually occur are checked before anything is read."""
    for src, want in (
            ('static void MePoolxDictNodeDeallocate(MeDictNode *node,void *pool)\n',
             'MePoolxDictNodeDeallocate'),
            ('static MeDictNode * MePoolxDictNodeAllocate(void *pool)\n',
             'MePoolxDictNodeAllocate')):
        m = STATIC_FN.search(src)
        if not m or m.group('name') != want:
            sys.exit('fix_callback_context: SELF-CHECK FAILED — read the name as '
                     '%r, want %r. A greedy return type eats the name and the '
                     'callback is dropped silently.'
                     % (m and m.group('name'), want))


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    global INC_G
    INC_G = os.path.join(root, 'include')
    selftest()

    # ---- THE SELF-CHECK. A probe that cannot compile measures nothing, every
    # site drops out, and "0 rewritten" reads exactly like "there were none".
    if measure('sizeof(MePoolx)') != 20 or measure('sizeof(MePoolx)', '-m64') != 24:
        sys.exit('fix_callback_context: SELF-CHECK FAILED — sizeof(MePoolx) '
                 'measured %r/%r, want 20/24.'
                 % (measure('sizeof(MePoolx)'), measure('sizeof(MePoolx)', '-m64')))

    done = declined = 0
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        p = plan(text, notes, fn)
        if not p:
            continue
        edits, label = p
        cand = text
        for st, en, rep in sorted(edits, key=lambda x: -x[0]):
            cand = cand[:st] + rep + cand[en:]
        # ⚠ ALL OR NOTHING PER FILE. A half-re-spelled context reads some fields
        # at one layout and some at the other, which is worse than neither.
        if compiles_identically(fn, cand, build, INC_G):
            open(path, 'w').write(cand)
            done += len(edits)
            notes.append('%-22s %s  %d edit(s)' % (fn, label, len(edits)))
        else:
            declined += 1
            notes.append('%-22s %s  DECLINED: not byte-identical at i386'
                         % (fn, label))
    for x in notes:
        print('  ' + x)
    print('fix_callback_context: %d edit(s), %d declined' % (done, declined))


if __name__ == '__main__':
    main()
