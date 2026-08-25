#!/usr/bin/env python3
"""
ghidra_clean.py — turn a Ghidra headless decompilation dump into compilable C.

Input:  a .c file produced by gscripts/DumpDecomp.java (functions separated by
        `/* ==== name ==== */` banners).
Output: a single .c translation unit that compiles with -std=gnu99.

What it does, and what it deliberately does NOT do:

  * strips Ghidra's advisory comments (WARNING blocks, "Unresolved local var"
    blocks). These are diagnostics about *naming*, not correctness — the values
    are still computed. See docs/KARMA-ON-WASM.md §II.5.
  * marks internal (originally `static`) functions static, from a symbol list
    read out of the ORIGINAL object file, so the exported ABI matches exactly.
  * emits forward declarations for every function in the unit, so definition
    order doesn't matter.
  * does NOT invent externs. Anything the object imports must be declared in a
    hand-written prelude (see --prelude); that is the per-object manual step and
    it is small (~10 lines for IxBoxBox).
"""
import argparse
import os
import re
import struct
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import vtable_slots

BANNER = re.compile(r'/\* ==== (\S+) ==== \*/')


MEINLINE_RE = re.compile(r'\bMeINLINE\b[\s\w\*]*?\b([A-Za-z_]\w*)\s*\(', re.S)


def header_inline_names(include_dir):
    """Functions the public headers define as MeINLINE.

    gcc 3.2 also emits an out-of-line LOCAL copy of each one it could not inline
    away, so the same function appears in the object AND in the header. Emitting
    our recovered copy then collides with the header's definition:
    "redefinition of MeVector3Normalize". The header's version is the same code,
    so drop ours and let the header win — which also means the compiler gets to
    inline it, as originally intended."""
    names = set()
    if not include_dir or not os.path.isdir(include_dir):
        return names
    for root, _, files in os.walk(include_dir):
        for f in files:
            if not f.endswith('.h'):
                continue
            try:
                txt = open(os.path.join(root, f), errors='ignore').read()
            except OSError:
                continue
            names.update(MEINLINE_RE.findall(txt))
    return names


def object_symbols(obj):
    """(exported, internal, real_symbol_of, weak) for the original object.

    `real_symbol_of` maps the name Ghidra displays to the actual ELF symbol.
    They differ for C++: Ghidra shows `MovingBoxBoxIntersect` while the object
    exports `_Z21MovingBoxBoxIntersectPKfPK11lsTransform...`. Emitting the
    displayed name in the asm label produces a symbol nothing links against —
    which is precisely why MovingBoxBoxIntersect and PolynomialRoots came out
    as undefined references.

    `weak` is the subset of `exported` that the shipped object exports as W
    rather than T, and it is kept separate because the binding is part of the
    interface. Collapsing the two is how the recovered keaMatrix_tester came to
    export a GLOBAL `putchar`: the shipped object's is weak so that libc wins,
    and the recovered one silently took over stdio for the entire engine. See
    KD_WEAK in kd_compat.h."""
    out = subprocess.run(['nm', '--defined-only', obj],
                         capture_output=True, text=True).stdout
    exported, internal, real, weak = set(), set(), {}, set()
    rows = []
    for line in out.splitlines():
        p = line.split()
        if len(p) >= 3 and p[-2] in 'TtWw':
            rows.append((p[-2], p[-1]))
    if rows:
        dem = subprocess.run(['c++filt'] + [n for _, n in rows],
                             capture_output=True, text=True).stdout.split('\n')
        for (kind, mangled), d in zip(rows, dem):
            short = re.sub(r'.*::', '', d.split('(')[0]).strip() or mangled
            (exported if kind in 'TW' else internal).add(short)
            if kind in 'Ww':
                weak.add(short)
            real.setdefault(short, mangled)
    return exported, internal, real, weak


def imported_names(obj):
    """Every name the object IMPORTS, mangled and demangled-short.

    Distinct from undefined_symbols() below, which returns UND symbols in ELF
    symbol-table ORDER because the EXTERNAL-block inversion depends on that
    order. This one is an unordered membership test: a bare identifier that
    appears here is a call into another translation unit, not to a sibling
    method, and must not be renamed."""
    out = subprocess.run(['nm', '-u', obj], capture_output=True, text=True).stdout
    raw = [l.split()[-1] for l in out.splitlines() if l.strip()]
    names = set(raw)
    if raw:
        dem = subprocess.run(['c++filt'] + raw, capture_output=True,
                             text=True).stdout.split('\n')
        for d in dem:
            names.add(re.sub(r'.*::', '', d.split('(')[0]).strip())
    return names


def split_functions(text):
    """Yield (name, body) for each decompiled function, in file order."""
    parts = BANNER.split(text)
    # parts = [preamble, name1, body1, name2, body2, ...]
    for i in range(1, len(parts) - 1, 2):
        yield parts[i], parts[i + 1]


def strip_comments(body):
    # Ghidra advisory blocks only; keep nothing, they are all diagnostics.
    body = re.sub(r'/\*.*?\*/', '', body, flags=re.S)
    # collapse the blank lines the stripping leaves behind
    body = re.sub(r'\n[ \t]*\n[ \t]*\n+', '\n\n', body)
    return body


def ghidra_type_quirks(body):
    """Undo two Ghidra spellings that are not valid C.

    `Foo_conflict` is what Ghidra calls a type it saw defined more than once
    with differing definitions; the underlying type is just `Foo`. Karma hits
    this for McdModelID / MdtContactID / MdtContactGroupID, whose typedefs
    appear in several CUs.

    `NAN(x)` is Ghidra's NaN test, and C99 already has NAN as a float CONSTANT,
    so the call form expands to `(0.0f/0.0f)(x)`. Both spellings appear in this
    corpus — McdGjk assigns the bare constant and tests with the call in the same
    object — so the call form is rewritten here and the constant is left to
    <math.h>. `NAN` followed by `(` is never the constant, so this is exact."""
    body = re.sub(r'\b([A-Za-z_]\w*)_conflict\b', r'\1', body)
    return re.sub(r'\bNAN\s*\(', 'isnan(', body)


ANON_CAST = re.compile(
    r'([A-Za-z_][\w.\->\[\]]*)\s*=\s*\*\(\s*anon_\w+\s*\*\)', re.S)
ANON_STORE = re.compile(
    r'\*\(\s*anon_\w+\s*\*\)(\([^;]*?\))\s*=\s*([A-Za-z_][\w.\->\[\]]*)')


ANON_DECL = re.compile(r'\banon_(?:union|struct)_\w*?_for_(\w+)\b')


def resolve_anon_types(body):
    """Replace Ghidra's invented anonymous-aggregate type names.

    Karma has several `union { void *ptr; int tag; }` members — McdContact's
    element1/element2, McdUserTriangle's triangleData — and Ghidra names their
    type `anon_union_4_2_<hash>_for_<field>`, which exists nowhere.

    Every use is a copy to or from a member of exactly that type, so
    __typeof__ of the other side names it precisely, with no table of hashes to
    maintain and nothing to keep in sync."""
    body = ANON_CAST.sub(lambda m: f'{m.group(1)} = *(__typeof__({m.group(1)}) *)', body)
    body = ANON_STORE.sub(
        lambda m: f'*(__typeof__({m.group(2)}) *){m.group(1)} = {m.group(2)}', body)
    # Anything left — a local DECLARATION of the anon type, which __typeof__ of
    # the other side cannot reach — becomes the exact typedef kd_types.h emits.
    body = ANON_DECL.sub(lambda m: f'kd_anon_{m.group(1)}', body)
    return body


FIELD_REF = re.compile(r'\b(\w+)\s*->\s*field_0x([0-9a-f]+)')
# `Type *var;` as a local, and `(Type *var, ...)` as a parameter. The parameter
# form matters most: the object a method operates on is `this`, which is always a
# parameter and never a local declaration.
# The trailing delimiter is a LOOKAHEAD: consuming it would eat the comma that
# separates this parameter from the next, so only every other parameter matched
# (McdContactSimplify's `inContacts` was skipped, `outContacts` was not).
DECL_PTR = re.compile(r'(?:^\s*|[(,]\s*)(?:struct\s+)?(\w+)\s*\*\s*(\w+)\s*(?=[;,)])', re.M)


VARARG_STACK = re.compile(r'&stack0x0000([0-9a-f]{4})\b')


def resolve_varargs(body, sig):
    """Turn Ghidra's `&stack0x0000000c` into a real va_list.

    A variadic function's extra arguments sit just past the named ones, and
    Ghidra names that stack location rather than recognising it:

        void MeInfo(int level, char *format, ...)
        { (*MeInfoHandler)(level, format, &stack0x0000000c); }

    On i386 a va_list IS a pointer to the first vararg, so the address Ghidra
    names and `ap` after va_start are the same thing. Only applied when the
    signature actually ends in `...`, so a same-shaped name in a non-variadic
    function is left alone."""
    if '...' not in sig or not VARARG_STACK.search(body):
        return body, 0
    # Last named parameter: what va_start anchors on.
    params = sig[sig.rfind('(') + 1:sig.rfind(')')].split(',')
    named = [p.strip() for p in params if p.strip() and p.strip() != '...']
    if not named:
        return body, 0
    last = re.findall(r'(\w+)\s*$', named[-1])
    if not last:
        return body, 0
    body = VARARG_STACK.sub('kd_ap', body)
    brace = body.find('\n{')
    if brace < 0:
        return body, 0
    body = (body[:brace + 2] + '\n  va_list kd_ap;\n'
            f'  va_start(kd_ap, {last[0]});\n' + body[brace + 2:])
    return body, 1


def resolve_field_names(body, fieldmap):
    """Turn `this->field_0x14` into `this->m_blocks`.

    Ghidra falls back to offset-named members for classes whose DWARF layout it
    did not apply — the kea matrix hierarchy, mostly. Now that kd_types.h carries
    the real layout, the offsets can be mapped back to names.

    The variable's type comes from its own declaration in the same function, so
    nothing is guessed: an offset is only renamed when the pointer's type is
    known AND that type has a member at exactly that offset."""
    if not fieldmap:
        return body
    var_type = {v: t for t, v in DECL_PTR.findall(body)}

    def sub(m):
        var, off = m.group(1), int(m.group(2), 16)
        t = var_type.get(var)
        if not t:
            return m.group(0)
        # DWARF keys the layout on the struct TAG (_McdContact) while the
        # declaration uses the typedef (McdContact), so try both spellings.
        name = (fieldmap.get(t, {}).get(str(off))
                or fieldmap.get('_' + t, {}).get(str(off)))
        if name:
            return f'{var}->{name}'
        known = t in fieldmap or ('_' + t) in fieldmap
        if known:
            # No member at this offset: Ghidra is touching PADDING. McdContact
            # has `short dims` at 28 and a 4-aligned union at 32, so field_0x1e
            # is the two bytes between them — the original copies them because
            # the compiler moved the struct in wider chunks. Byte arithmetic
            # says exactly that, and stays correct where a member name cannot
            # exist. Written as a char lvalue so a surrounding `&` still works.
            return f'(*(char *)((char *)({var}) + 0x{off:x}))'
        return m.group(0)

    return FIELD_REF.sub(sub, body)


def cxx_names_to_c(body):
    """`Foo::bar` is how Ghidra renders a C++ function-local static or member.
    C has no `::`; flatten it to a legal identifier. The declaration itself is
    supplied by the per-object prelude.

    A destructor is `Foo::~Foo`, and `~` is not a word character, so an earlier
    pattern left the `::` in place and then rename_exported inserted `kd_` after
    the tilde — `CxSmallSort::~kd_CxSmallSort`, which is not anything. Handle it
    first and give it a name that cannot collide with a method called
    `dtor_Foo`."""
    body = re.sub(r'\b([A-Za-z_]\w*)::~([A-Za-z_]\w*)', r'\1__dtor_\2', body)
    return re.sub(r'\b([A-Za-z_]\w*)::([A-Za-z_]\w*)', r'\1__\2', body)


RET_NAME_RE = re.compile(r'^(.*?\b)([A-Za-z_]\w*)\s*\(')


def declarator_name(sig):
    """The identifier immediately before the parameter list."""
    m = RET_NAME_RE.match(sig)
    return m.group(2) if m else None


def rename_exported(sig, name):
    """Give an exported function an internal C name plus an asm label.

    Ghidra recovers the parameter types the CODE actually uses, which is often
    not how the public header spells them: McdBoxGetXYAABB really takes an
    lsTransform*, but McdBox.h declares it as MeMatrix4. Same 64 bytes, same
    ABI, incompatible C types — so defining it under its own name collides with
    the header's prototype and the translation unit fails to compile.

    Defining it as kd_<name> with __asm__("<name>") emits exactly the same
    symbol while sidestepping the C type system. Only the ABI has to match, and
    it does."""
    m = RET_NAME_RE.match(sig)
    if not m:
        return None, sig
    # For a C++ method the banner says `writebackMatrixChol` but the declarator
    # (after Foo::bar was flattened) reads `keaMatrix__writebackMatrixChol`.
    # Rename whatever the declarator actually says, not the banner name.
    decl = m.group(2)
    return f'kd_{decl}', sig[:m.start(2)] + f'kd_{decl}' + sig[m.end(2):]


STACK_SYM = re.compile(r'\bstack0x([0-9a-f]{8})\b')


def materialise_alloca_frame(body, fname):
    """Restore a variable-length stack allocation as a real alloca().

    The shipped code does exactly what alloca does, and the assembly says so
    plainly (McdSphereTriangleListIntersect):

        mov 0x28(%ecx),%edx        ; count = geom->maxTriangles
        lea (%edx,%edx,2),%eax     ; x3
        lea 0xf(,%eax,8),%edx      ; x8  -> count*24 + 15
        and $0xfffffff0,%edx       ; round up to 16
        sub %edx,%esp              ; alloca(count * 24)

    Ghidra renders it as a negative size plus a named stack address:

        iVar15 = -((int)EXPR * 0x18 + 0xfU & 0xfffffff0);
        dest   = (T)(&stack0xfffffeb4 + iVar15);

    An earlier attempt here substituted a fixed 64 KB buffer. That was wrong in
    the worst way: it COMPILED, passed the substitute gate, and then segfaulted
    on its first call in a real match, handing a wild pointer to the engine's own
    KTriListGenerator callback. A real alloca reproduces the original exactly and
    needs no size assumption.

    Any `stack0xNNNN` NOT part of this idiom is left alone. Those are references
    into the shifted frame — usually outgoing call arguments Ghidra failed to
    model — and inventing storage for them would repeat the same mistake.
    recover.py keeps such objects out of the validated set."""
    # `dest = (T)(&stack0xHHHH + negVar);` where negVar = -(EXPR * K + 15 & ~15)
    neg = {}
    # The `(int)` cast is not always there — Ghidra omits it when the count is
    # already an int, as in `-(triList->triangleMaxCount * 0x18 + 0xfU & ...)`.
    # Requiring it cost IxCylinderTriList and IxConvexTriList their allocas.
    for m in re.finditer(r'(\w+)\s*=\s*-\(\s*(?:\(int\)\s*)?(.+?)\s*\*\s*(0x[0-9a-f]+|\d+)'
                         r'\s*\+\s*0xfU?\s*&\s*0xfffffff0\)\s*;', body):
        neg[m.group(1)] = (m.group(2).strip(), m.group(3))

    # The SPLIT form of the same idiom. Ghidra does not always fold the rounding
    # and the negation into one expression:
    #
    #     uVar19 = count * 4 + 0xfU & 0xfffffff0;
    #     iVar5  = -uVar19;
    #
    # is the identical `sub %edx,%esp` alloca, and reading it as anything else
    # leaves `&stack0xffffffac + iVar5` undeclared — which is what kept
    # MeAssetFactory and MeFAsset out of the build, both of them on the .ka path
    # that instances every ragdoll. The size expression is matched exactly as
    # above, so this widens WHICH SPELLING is recognised, not what counts as an
    # alloca.
    sized = {}
    for m in re.finditer(r'(\w+)\s*=\s*(?:\(int\)\s*)?(.+?)\s*\*\s*(0x[0-9a-f]+|\d+)'
                         r'\s*\+\s*0xfU?\s*&\s*0xfffffff0\s*;', body):
        sized[m.group(1)] = (m.group(2).strip(), m.group(3))
    for m in re.finditer(r'(\w+)\s*=\s*-\s*(\w+)\s*;', body):
        if m.group(2) in sized and m.group(1) not in neg:
            neg[m.group(1)] = sized[m.group(2)]

    n = 0
    # negVar -> the base it was expressed against. Anything ELSE written as
    # `base + negVar` denotes the same block and must not be collapsed to the
    # bare base; see the assignment below.
    alloca_base = {}
    # negVars whose alloca() call has already been emitted. The block is
    # allocated ONCE — `sub %edx,%esp` runs once — so every later
    # `base + negVar` is the same address and must reuse the same pointer.
    allocated = set()
    # Invented storage for slots BELOW the block; see sub_below().
    below = set()
    if neg:
        def sub_alloca(m):
            nonlocal n
            var = m.group(3)
            if var not in neg:
                return m.group(0)
            expr, mult = neg[var]
            base = m.group(2)
            if var in allocated:
                # A SECOND derivation of the block, not a second block. esp was
                # decremented once, so `base + negVar` names one address. A
                # fresh alloca() here hands out different storage each time —
                # the code then writes into one block and reads from another,
                # and where the derivation sits inside a loop (IxSphylPrimitives
                # re-materialises it twice per triangle) the stack grows without
                # bound.
                #
                # Only where the BASE matches. A different base is a different
                # address — MstUtils derives both a 40 KB fixed array and the
                # block itself off the same negVar — and this rule has nothing
                # to say about it, so it keeps the older behaviour there.
                if base == alloca_base.get(var):
                    n += 1
                    return f'{m.group(1)}kd_alloca_{var}'
            n += 1
            allocated.add(var)
            alloca_base.setdefault(var, base)
            return (f'{m.group(1)}(kd_alloca_{var} = '
                    f'(char *)alloca((size_t)({expr}) * {mult}))')
        # ONLY the defining use: the alloca'd pointer appearing as the RHS of an
        # assignment, i.e. `dest = (T)(&stack0xH + negVar);`. Other
        # `&stack0xH + negVar` sites are STORES into the shifted frame — Ghidra's
        # rendering of outgoing call arguments — and substituting an alloca there
        # would hand out a fresh block per store, which is nonsense. Leaving them
        # unresolved is deliberate: the object then fails to compile and
        # recover.py holds it back, which is the honest outcome.
        # Ghidra writes the address either as `&stack0xH` or, when it wants an
        # integer, `(int)&stack0xH`. Both are the same defining use.
        #
        # The base is not always a stack0x name. When Ghidra did manage to name
        # SOME local at the right place it uses that instead —
        # `(McdGeometryID)((int)&MStack_24c + iVar5)` in IxBoxTriList — and an
        # earlier version of this pattern only recognised the stack0x spelling,
        # so three TriangleList objects never got their alloca restored and
        # failed to compile on the leftover slot names. What identifies the
        # idiom is `negVar`, which is only in `neg` if it was assigned the
        # rounded negative size; the base is incidental.
        body = re.sub(r'(=\s*\([^()]*\)\s*)\(\s*(?:\(int\)\s*)?&(\w+)'
                      r'\s*\+\s*(\w+)\s*\)',
                      sub_alloca, body)
        # The cast and the parentheses are not always there either. Ghidra
        # writes the defining store as whatever is shortest:
        #
        #   trilistgeom[3].prev = (McdGeometryID)((int)&MStack_24c + iVar5);
        #   *(undefined1 **)((int)pvVar22 + 0x34) = auStack_12c + iVar17;
        #
        # Both put the allocation into the triangle-list geometry, and only the
        # second cost IxConvexTriList its alloca. What separates the DEFINING
        # use from the frame stores is the shape of the statement: the defining
        # one has `base + negVar` as the whole right-hand side, and a frame
        # store has it inside a dereference on the LEFT. Anchoring on `= ... ;`
        # says exactly that.
        for var in neg:
            def sub_bare(m, var=var):
                nonlocal n
                if var in allocated and m.group(1) == alloca_base.get(var):
                    n += 1
                    return f'= kd_alloca_{var};'
                expr, mult = neg[var]
                n += 1
                allocated.add(var)
                alloca_base.setdefault(var, m.group(1))
                return (f'= (kd_alloca_{var} = '
                        f'(char *)alloca((size_t)({expr}) * {mult}));')
            body = re.sub(r'=\s*(?:\([^()]*\)\s*)?\(?\s*(?:\(int\)\s*)?'
                          r'&?(\w+)\s*\+\s*' + re.escape(var) + r'\s*\)?\s*;',
                          sub_bare, body, count=1)

    if n:
        # `base + negVar - K` is BELOW the block, so it is neither the
        # allocation nor any local Ghidra named: the original decremented esp,
        # so the bottom of the frame is the outgoing-argument area for a call
        # and Ghidra had no variable there to attribute it to. It renders it
        # against whichever local it could reach, at a negative offset.
        #
        # Dropping negVar the way the collapse below does leaves
        # `(int)afStack_11c + -0x1c` — a write 28 bytes under an 8-byte array,
        # i.e. a stack smash. That compiles, passes all three substitute
        # scenes, and is 18% wrong in a live match, which is exactly what
        # IxConvexTriList did. IxSphylPrimitives has the same shape against
        # `MeReal tmp[3]` and was released carrying it.
        #
        # These slots are pure argument scratch — every one is written and then
        # read back a few lines later — so, like the stack0xNNNN path below,
        # all they need is storage of their own that nothing else can reach.
        # Keyed on the whole spelling, base and shift vars included, because
        # that is as far as the text can be trusted to identify an address: a
        # function with two allocas shifts its frame twice and writes the slot
        # as `buffer + iVar10 + iVar3 + -0x18` (MeAssetDBXMLOutput_1_0). Taking
        # only the nearest term as the base there names two different addresses
        # the same, and leaves `buffer +` dangling in front of a void*[2].
        def sub_below(m):
            shifts = re.findall(r'\w+', m.group(3))
            if any(v not in neg for v in shifts):
                return m.group(0)      # not a frame shift; leave it alone
            slot = ('kd_frameslot_' + m.group(2) + '_' + '_'.join(shifts)
                    + '_m' + m.group(4))
            below.add(slot)
            return (f'(int)({slot})' if m.group(1) else f'({slot})')
        body = re.sub(r'(\(int\)\s*)?&?(?<![.>])\b(\w+)((?:\s*\+\s*\w+)+)'
                      r'\s*\+\s*-0x([0-9a-f]+)\b', sub_below, body)
        if below:
            decls = [f'    void *{s}[2];' for s in sorted(below)]
            brace = body.find('\n{')
            if brace >= 0:
                body = (body[:brace + 2] + '\n' + '\n'.join(decls) + '\n'
                        + body[brace + 2:])

        # After the real alloca is restored, every REMAINING reference of the
        # form `<base> + negVar` is outgoing-argument scratch: Ghidra renders a
        # push as a store to the shifted frame and the matching read as a load
        # from the same place. Store and load pair up, so the semantics are
        # already right — the address just has to be valid and consistent.
        #
        # `negVar` is the (negative) alloca size, so keeping it would index far
        # outside the local it is added to. Dropping it collapses each slot onto
        # its own base local, which preserves the pairing exactly. This is only
        # safe BECAUSE the alloca is now a separate real allocation and can no
        # longer alias these slots.
        for var in neg:
            # `base + negVar` where base is the one the ALLOCA was expressed
            # against is not a scratch slot — it is the allocated block itself,
            # written somewhere as a value. IxConvexTriList does exactly this:
            #
            #   pfVar15 = (float *)((int)afStack_11c + iVar16);      <- the block
            #   *(int *)(&stack0x... + iVar16) = (int)afStack_11c + iVar16;
            #
            # Collapsing the second to `(int)afStack_11c` hands out a pointer to
            # an unrelated local, and whatever consumes that slot reads the
            # wrong memory. Point it at the block instead.
            ab = alloca_base.get(var)
            if ab:
                body = re.sub(r'\(int\)\s*&?' + re.escape(ab) + r'\s*\+\s*'
                              + re.escape(var) + r'\b',
                              f'(int)(kd_alloca_{var})', body)
                body = re.sub(r'&?' + re.escape(ab) + r'\s*\+\s*'
                              + re.escape(var) + r'\b(?=\s*[\)\,;])',
                              f'kd_alloca_{var}', body)
            body = re.sub(r'\s*\+\s*' + re.escape(var) + r'\b(?=\s*[\)\,])', '', body)
            body = re.sub(r'\(int\)(&?\w+)\s*\+\s*' + re.escape(var) + r'\b',
                          r'(int)\1', body)
        # The allocated block needs a name, because it is referred to in more
        # than one place; see the collapse above.
        if alloca_base:
            decls = [f'    char *kd_alloca_{v};' for v in sorted(alloca_base)]
            brace = body.find('\n{')
            if brace >= 0:
                body = (body[:brace + 2] + '\n' + '\n'.join(decls) + '\n'
                        + body[brace + 2:])

        # Any stack0xNNNN still standing is a scratch slot with no named local
        # behind it; give it one.
        leftover = sorted(set(STACK_SYM.findall(body)))
        if leftover:
            decls = [f'    void *kd_argslot_{h}[2];' for h in leftover]
            body = STACK_SYM.sub(lambda m: f'(*kd_argslot_{m.group(1)})', body)
            brace = body.find('\n{')
            if brace >= 0:
                body = (body[:brace + 2] + '\n' + '\n'.join(decls) + '\n'
                        + body[brace + 2:])

    return body, n


# ---- Ghidra's EXTERNAL block, inverted ------------------------------------
#
# A relocatable .o has no addresses for its imports, so Ghidra invents them: it
# gives every undefined symbol a four-byte slot in a synthetic EXTERNAL block,
# in ELF symbol-table order. That is fine until a relocation carries an ADDEND,
# because the addend then lands in a neighbouring symbol's slot and Ghidra
# reports the neighbour. McdNull.o is the clearest case:
#
#     5b:  ff 15 08 00 00 00     call *0x8
#          5d: R_386_32 MeMemoryAPI          <- MeMemoryAPI.createAligned
#
# comes back as
#
#     (*_McdGeometryDeinit)(0x20,0x10)
#
# because McdGeometryDeinit happens to sit two slots after MeMemoryAPI. The
# name is wrong, but nothing is lost: the relocation records the true base and
# the addend is sitting in the instruction. Reading both back inverts the
# mangling exactly, and the result is checkable — if the slot arithmetic did not
# reproduce the name Ghidra printed, the assumption is wrong and the rule stays
# out of it.

def undefined_symbols(obj):
    """Undefined symbols in ELF symbol-table order — Ghidra's slot order."""
    out = subprocess.run(['readelf', '-sW', obj], capture_output=True, text=True).stdout
    syms = []
    for line in out.splitlines():
        m = re.match(r'\s*\d+:\s+\S+\s+\S+\s+\S+\s+\S+\s+\S+\s+(\S+)\s+(\S+)', line)
        if m and m.group(1) == 'UND' and m.group(2):
            syms.append(m.group(2))
    return syms


GHIDRA_IMAGE_BASE = 0x10000


def ghidra_memory_map(obj):
    """([(section, start, end)], external_block_base) as Ghidra lays a .o out.

    Ghidra gives a relocatable object addresses it invents: the allocatable
    sections consecutively from 0x10000 in section-header order, each aligned to
    its own sh_addralign, then the synthetic EXTERNAL block at the next 0x1000
    boundary.

    That is a claim about someone else's tool, so it is checked rather than
    trusted. MeXMLOutput settles it: .text is 0x405 bytes, and the dump's
    `MeStreamWrite(&DAT_00010405,1,1,...)` writes one byte from 0x10405 —
    offset 0 of .rodata.str1.1, whose first byte is '<'. The next two writes are
    '>' at +2 and "</" at +0x12, which is what an XML writer emits.

    ZERO-SIZED SECTIONS ARE SKIPPED, and getting that wrong is not cosmetic.
    That same object has an EMPTY `.data` and `.bss` between `.text` and the
    strings; honouring their 4-byte alignment pushes `.rodata.str1.1` from
    0x10405 to 0x10408, which puts two of the object's four data references at a
    NEGATIVE offset — impossible — and silently resolves the other two to the
    wrong bytes. Ghidra creates no block for an empty section, so it neither
    advances nor aligns the address.

    Checked two ways rather than assumed. MeXMLOutput's four references are
    0x10405, 0x10407, 0x10417 and 0x1041a; against a base of 0x10405 those are
    offsets 0, 2, 0x12 and 0x15 — '<', '>', "</" and a newline, every one a
    valid string start and exactly the verification above. Corpus-wide, across
    every DAT_/PTR_ reference in every dump, skipping empty sections leaves ZERO
    addresses outside any section while honouring them leaves two.

    The EXTERNAL block base is unchanged in all 192 members, so the symbol
    inversion that rests on it does not move."""
    out = subprocess.run(['readelf', '-SW', obj], capture_output=True, text=True).stdout
    addr, secs = GHIDRA_IMAGE_BASE, []
    for line in out.splitlines():
        m = re.match(r'\s*\[\s*\d+\]\s+(\S+)\s+(\S+)\s+[0-9a-f]+\s+[0-9a-f]+\s+'
                     r'([0-9a-f]+)\s+[0-9a-f]+\s+(\S*)\s+\d+\s+\d+\s+(\d+)', line)
        if not m:
            continue
        name, typ, size, flags, align = (m.group(1), m.group(2), int(m.group(3), 16),
                                         m.group(4), int(m.group(5)))
        if 'A' not in flags or size == 0:
            continue
        align = max(align, 1)
        addr = (addr + align - 1) // align * align
        secs.append((name, typ, addr, addr + size))
        addr += size
    return secs, (addr + 0xfff) // 0x1000 * 0x1000


def relocation_targets(obj, per_function=False):
    """{ghidra_name: {(real_symbol, addend), ...}} for relocated data references.

    A name can have more than one candidate, and they are NOT interchangeable.
    Ghidra's block is a fiction, so two relocations that land on the same slot
    in it — `MeMemoryAPI + 12` and `MePoolAPI + 0` in MdtWorld.o — describe two
    completely unrelated addresses at link time, and Ghidra prints the SAME name
    at both. Which one a given site meant has to be decided from where the site
    is, not from a preference between them.

    With per_function, the result is {function: {ghidra_name: {...}}}, keyed on
    which function's byte range the relocation falls in. That is usually enough
    to leave one candidate standing: MdtWorldCreate relocates against MePoolAPI
    and MdtWorldDestroy against MeMemoryAPI, so inside either one the name is
    unambiguous even though it is not across the file.

    The addend is the implicit one i386 REL relocations keep in the instruction
    stream, so it is read out of the section bytes rather than the relocation
    record."""
    und = undefined_symbols(obj)
    slot = {s: i for i, s in enumerate(und)}
    if not und:
        return {}
    extents = function_extents(obj) if per_function else []
    _secs, extbase = ghidra_memory_map(obj)

    out = subprocess.run(['readelf', '-rW', obj], capture_output=True, text=True).stdout
    cache, found = {}, {}
    sect = None
    for line in out.splitlines():
        m = re.match(r"Relocation section '(\S+)'", line)
        if m:
            sect = m.group(1)[4:] if m.group(1).startswith('.rel') else m.group(1)
            continue
        m = re.match(r'([0-9a-f]{8})\s+\S+\s+(\S+)\s+[0-9a-f]{8}\s+(\S+)', line)
        if not (m and sect and m.group(2) == 'R_386_32'):
            continue
        sym, off = m.group(3), int(m.group(1), 16)
        if sym not in slot:
            continue
        if sect not in cache:
            cache[sect] = _section_bytes(obj, sect)
        data = cache[sect]
        if off + 4 > len(data):
            continue
        addend = struct.unpack('<i', data[off:off + 4])[0]
        if addend % 4:
            continue
        target = slot[sym] + addend // 4
        if target < 0:
            continue
        # A slot past the last undefined symbol has no name to borrow, so Ghidra
        # falls back to the address: `_DAT_0001100c`. The address is the block
        # base plus four bytes a slot, which the memory map gives exactly.
        name = ('_' + und[target] if target < len(und)
                else '_DAT_%08x' % (extbase + 4 * target))
        if not per_function:
            found.setdefault(name, set()).add((sym, addend))
            continue
        if not (sect.startswith('.text') or sect.startswith('.gnu.linkonce.t.')):
            continue
        # gcc 3.2 emits an inlined local copy of a libc function into a section
        # of its own — `.gnu.linkonce.t.putchar`, its pre-comdat mechanism.
        # Offsets there are section-relative, so the byte-range lookup does not
        # apply; key it on the section instead. It will not match a banner,
        # which is correct — the per-function lookup misses and the file-wide
        # identity fallback in fix_mislabelled_external picks it up. That is how
        # keaDebug's inlined putchar finds `stdout`.
        fn = (next((n for n, lo, hi in extents if lo <= off < hi), None)
              if sect == '.text' else sect)
        if fn:
            found.setdefault(fn, {}).setdefault(name, set()).add((sym, addend))
    return found


def function_extents(obj):
    """[(short_name, start, end)] for every function the object defines."""
    out = subprocess.run(['nm', '--print-size', '--defined-only', obj],
                         capture_output=True, text=True).stdout
    rows = []
    for line in out.splitlines():
        p = line.split()
        if len(p) == 4 and p[2] in 'tTwW':
            rows.append((int(p[0], 16), int(p[1], 16), p[3]))
    if not rows:
        return []
    dem = subprocess.run(['c++filt'] + [n for _, _, n in rows],
                         capture_output=True, text=True).stdout.split('\n')
    ext = []
    for (value, size, _mangled), d in zip(rows, dem):
        short = re.sub(r'.*::', '', d.split('(')[0]).strip()
        if short:
            ext.append((short, value, value + size))
    return ext


def _section_bytes(obj, section):
    tmp = f'/tmp/.kd_sec_{os.getpid()}.bin'
    r = subprocess.run(['objcopy', '-O', 'binary', f'--only-section={section}',
                        obj, tmp], capture_output=True)
    if r.returncode != 0 or not os.path.exists(tmp):
        return b''
    data = open(tmp, 'rb').read()
    os.unlink(tmp)
    return data


EXTERN_VAR = re.compile(
    r'^\s*(?:MEPUBLIC\s+)?extern\s+(?:const\s+)?((?:struct\s+)?[A-Za-z_]\w*)\s+'
    r'([A-Za-z_]\w*)\s*;', re.M)


def extern_var_types(include_dir):
    """{variable: type} for every `extern T name;` the public headers declare.

    Only the ones spelled out in a header are usable, which is the point: the
    type is read, never inferred."""
    types = {}
    if not include_dir or not os.path.isdir(include_dir):
        return types
    for root, _, files in os.walk(include_dir):
        for f in files:
            if not f.endswith('.h'):
                continue
            try:
                txt = open(os.path.join(root, f), errors='ignore').read()
            except OSError:
                continue
            for typ, name in EXTERN_VAR.findall(txt):
                types.setdefault(name, typ.replace('struct ', '').strip())
    return types


DATA_REF = re.compile(r'(?<![\w])DAT_([0-9a-f]{8})\b')


def materialise_data_refs(obj, text):
    """Give `DAT_00010405` the bytes it names.

    Ghidra falls back to an address when a data reference has no symbol, which
    is most of the time for string literals: gcc puts them in .rodata.str1.1
    with no symbol at all. The bytes are right there in the object, and
    ghidra_memory_map() says which offset the address is, so this is a
    transcription rather than a reconstruction:

        MeStreamWrite(&DAT_00010405,1,1,op->stream);   ->   writes '<'

    Only .rodata and .data are eligible, and only when the section carries no
    relocations. A relocated word is a POINTER whose value exists solely in the
    relocation record, and emitting the zero that sits in the section bytes
    would look right and be null. Anything not covered is left undefined, so
    the object keeps failing to compile, which is the honest outcome."""
    addrs = sorted(set(int(a, 16) for a in DATA_REF.findall(text)))
    if not addrs:
        return '', 0
    secs, _ext = ghidra_memory_map(obj)
    relocated = set(re.findall(r"Relocation section '\.rel(\S+)'",
                               subprocess.run(['readelf', '-rW', obj],
                                              capture_output=True, text=True).stdout))
    used, defines = {}, []
    for addr in addrs:
        for name, typ, lo, hi in secs:
            if not lo <= addr < hi or typ != 'PROGBITS':
                continue
            if not (name.startswith('.rodata') or name.startswith('.data')):
                continue
            if name in relocated:
                continue
            used.setdefault(name, (lo, hi))
            defines.append((addr, name, addr - lo))
            break
    if not defines:
        return '', 0

    out = ['/* ---- unnamed object data, read from the original sections ---- */']
    for name, (lo, hi) in sorted(used.items()):
        data = _section_bytes(obj, name)[:hi - lo]
        c = 'kd_sec' + re.sub(r'\W', '_', name)
        rows = ', '.join('0x%02x' % b for b in data)
        out.append(f'/* {name}: {len(data)} bytes at 0x{lo:x} */')
        out.append(f'static const unsigned char {c}[{max(len(data), 1)}] = {{ {rows} }};')
    for addr, name, off in defines:
        c = 'kd_sec' + re.sub(r'\W', '_', name)
        out.append(f'#define DAT_{addr:08x} ({c}[0x{off:x}])')
    return '\n'.join(out) + '\n\n', len(defines)


PTR_BLOCK_REF = re.compile(r'(?<![\w])(PTR_[A-Za-z0-9_]*?_([0-9a-f]{8}))\b')


def materialise_relocated_data(obj, text, renames=None):
    """Rebuild a RELOCATED .rodata block — the thing materialise_data_refs won't.

    That function deliberately refuses a section carrying relocations, because a
    relocated word's value exists only in the relocation record and the zero
    sitting in the section bytes would look right and be null. This does the
    other half: it emits the words WITH their relocations applied, which is the
    only way to recover a statically initialised table of pointers.

    Karma's XML readers are built entirely out of those. `MeAssetDBXMLInput_1_0`
    copies a handler template out of .rodata into a local:

        ppuVar3 = &PTR_s_ASSET_000124fc;
        for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
            pMVar4->name = *ppuVar3;  ppuVar3 = ppuVar3 + 1; ...
        }

    and at that address sit two `MeXMLHandler`s — `{"ASSET",
    MeXMLActionCallback, Handle_Asset_1_0, ...}` followed by an
    `MeXMLActionEnd` sentinel. The tag name and the handler function are BOTH
    relocations; the rest is literal zeros and a 2.

    WHAT EACH WORD BECOMES, and none of it is inferred from decompiled text:

      * a relocation naming a SECTION — gcc's spelling for a string literal,
        `.rodata.str1.1 + 1137` — becomes a pointer into that section's bytes,
        emitted alongside;
      * a relocation naming a SYMBOL becomes that symbol's address, under
        whatever name the recovery gave it. That is why this block is emitted
        AFTER the forward declarations rather than with the other data;
      * no relocation: the literal word out of the section bytes.

    THE WHOLE SECTION IS EMITTED, not a guessed extent. Nothing here knows where
    a table starts or ends — the copy loop's trip count does, and that lives in
    the code, not the data — so the `#define` just indexes the section image and
    the pointer arithmetic in the body keeps working unchanged.

    A word whose relocation names a symbol the object does not define is left as
    its literal addend with the symbol in a comment: emitting a reference to a
    name that is not declared would trade a wrong value for a link error, and
    neither is better than leaving the object visibly failing."""
    refs = {}
    for full, addr in PTR_BLOCK_REF.findall(text):
        refs[full] = int(addr, 16)
    if not refs:
        return '', 0
    secs, _ext = ghidra_memory_map(obj)
    relocs = _section_relocations(obj)
    _exported, _internal, _real, _weak = object_symbols(obj)
    defined = set(_exported) | set(_internal)
    renames = renames or {}

    wanted, defines = {}, []
    for full, addr in sorted(refs.items()):
        for name, typ, lo, hi in secs:
            if not (lo <= addr < hi and typ == 'PROGBITS'):
                continue
            if not (name.startswith('.rodata') or name.startswith('.data')):
                continue
            if name not in relocs or (addr - lo) % 4:
                continue
            wanted.setdefault(name, (lo, hi))
            defines.append((full, name, (addr - lo) // 4))
            break
    if not defines:
        return '', 0

    # Sections a relocation points INTO (the string pools) have to come first.
    targets = {}
    for name, (lo, hi) in wanted.items():
        for sym in relocs[name].values():
            if sym.startswith('.'):
                targets[sym] = True

    out = ['/* ---- relocated section data (needs the declarations above) ---- */']
    for sym in sorted(targets):
        data = _section_bytes(obj, sym)
        c = 'kd_relstr' + re.sub(r'\W', '_', sym)
        rows = ', '.join('0x%02x' % b for b in data) or '0'
        out.append(f'static const unsigned char {c}[{max(len(data), 1)}] = {{ {rows} }};')

    for name, (lo, hi) in sorted(wanted.items()):
        data = _section_bytes(obj, name)[:hi - lo]
        rel = relocs[name]
        words = []
        for off in range(0, len(data) - 3, 4):
            raw = struct.unpack('<I', data[off:off + 4])[0]
            sym = rel.get(off)
            if sym is None:
                words.append('(void *)0x%xu' % raw)
            elif sym.startswith('.'):
                t = 'kd_relstr' + re.sub(r'\W', '_', sym)
                words.append('(void *)&%s[0x%x]' % (t, raw))
            elif sym in defined:
                words.append('(void *)&%s' % renames.get(sym, sym))
            else:
                words.append('(void *)0x%xu /* %s */' % (raw, sym))
        c = 'kd_relsec' + re.sub(r'\W', '_', name)
        out.append(f'/* {name}: {len(words)} words at 0x{lo:x} */')
        out.append(f'static void *const {c}[{max(len(words), 1)}] = {{')
        out.append('    ' + ',\n    '.join(words) if words else '    0')
        out.append('};')
    for full, name, idx in defines:
        c = 'kd_relsec' + re.sub(r'\W', '_', name)
        out.append(f'#define {full} ({c}[0x{idx:x}])')
    return '\n'.join(out) + '\n\n', len(defines)


def _section_relocations(obj):
    """{section: {offset: symbol}} for R_386_32 relocations in data sections."""
    out, sect = {}, None
    raw = subprocess.run(['readelf', '-rW', obj], capture_output=True, text=True).stdout
    for line in raw.splitlines():
        m = re.match(r"Relocation section '(\S+)'", line)
        if m:
            sect = m.group(1)[4:] if m.group(1).startswith('.rel') else m.group(1)
            continue
        m = re.match(r'([0-9a-f]{8})\s+\S+\s+(\S+)\s+[0-9a-f]{8}\s+(\S+)', line)
        if m and sect and m.group(2) == 'R_386_32':
            out.setdefault(sect, {})[int(m.group(1), 16)] = m.group(3)
    return out


FUNC_TYPE = re.compile(r'\b_func_(\w+)\b')


def ghidra_functype_typedefs(text):
    """Define the function types Ghidra names but never declares.

    A DWARF function type with no typedef of its own comes back as a NAME that
    spells out the signature — `_func_MeReal_float`, `_func_void_MeReal_ptr_
    MeReal_ptr` — with `ptr` as a suffix meaning one level of indirection. The
    name is the definition, so the typedef can be written straight from it.

    Checked against the mangled symbols in the same object, which carry the same
    information independently:

        _Z6mnbrakRfS_S_S_S_S_PFffE        mnbrak(..., float (*)(float))
        _Z11LineFnMinNdiPfS_fPFfS_EPFvS_S_E
                                          LineFnMinNd(..., float (*)(float*),
                                                           void (*)(float*, float*))

    A name whose tokens are not types will not compile, and recover.py then
    holds the object back — so a bad split fails loudly rather than quietly."""
    names = sorted(set(FUNC_TYPE.findall(text)))
    if not names:
        return '', 0
    out = ['/* ---- function types Ghidra spelled out in the name ---- */']
    n = 0
    for name in names:
        toks, parts = name.split('_'), []
        for t in toks:
            if t == 'ptr' and parts:
                parts[-1] += ' *'
            else:
                parts.append(t)
        if not parts:
            continue
        ret, args = parts[0], parts[1:]
        if args == ['varargs'] or not args:
            arglist = ''                    # unprototyped, as Ghidra means it
        elif args == ['void']:
            arglist = 'void'
        else:
            arglist = ', '.join('...' if a == 'varargs' else a for a in args)
        out.append(f'typedef {ret} _func_{name}({arglist});')
        n += 1
    return '\n'.join(out) + '\n\n', n


def _statement_bounds(lines, idx):
    """[lo, hi) — the lines of the statement containing lines[idx].

    Walks back over continuation lines, which are the ones whose predecessor
    does not end a statement or open a block."""
    lo = idx
    while lo > 0 and not lines[lo - 1].rstrip().endswith((';', '{', '}', ':')):
        lo -= 1
        if idx - lo > 8:                   # a runaway is not a statement
            lo = idx
            break
    hi = idx + 1
    while hi < len(lines) and not lines[hi - 1].rstrip().endswith((';', '{', '}', ':')):
        hi += 1
        if hi - idx > 8:
            hi = idx + 1
            break
    return lo, hi


SELF_ASSIGN = re.compile(
    r'^([ \t]*)([A-Za-z_]\w*)\[([A-Za-z_]\w*)\]\s*=\s*\2\[\3\]\s*;[ \t]*$', re.M)
CONST_STORE = re.compile(r'^[ \t]*([A-Za-z_]\w*)\[(\d+)\]\s*=')


def restore_saved_element(body):
    """`boxP[axis] = boxP[axis];` is a SAVE AND RESTORE, not a no-op.

    McdSphylBoxIntersect keeps one component of a point while overwriting the
    other three, and gcc 3.2 does it by stashing the word in a register first:

        1c24:  mov   -0x48(%ebp,%edi,4),%ecx   ; save boxP[axis]
        1c28:  fstps -0x48(%ebp)               ; boxP[0] = n[0]
        1c2d:  fstps -0x40(%ebp)               ; boxP[2] = n[2]
        1c30:  fstps -0x44(%ebp)               ; boxP[1] = n[1]
        1c33:  mov   %ecx,-0x48(%ebp,%edi,4)   ; restore boxP[axis]

    Ghidra folds the save into the restore, which puts the READ after the three
    stores instead of before them — because it has no way to know that a
    variable index can alias a constant one. The line then reads the value it
    just wrote and the component is lost.

    That is not a cosmetic difference. It is why McdSphylBoxIntersect returns
    the right penetration depth at the wrong point: the normal is computed as
    `n - boxP`, and with boxP an exact copy of n it comes out as zero. Found by
    test/difftest_pair.sh, 1 in 300,000 pairs structurally and ~0.2 world units
    of position error on many more.

    Only fires when the intervening stores use CONSTANT indices into the same
    array, which is precisely the case Ghidra cannot see through. Where nothing
    aliases, the self-assignment really is a no-op and is left alone."""
    out, n = [], 0
    lines = body.split('\n')
    i = 0
    while i < len(lines):
        m = SELF_ASSIGN.match(lines[i])
        if not m:
            out.append(lines[i]); i += 1
            continue
        indent, base, idx = m.group(1), m.group(2), m.group(3)
        # walk back over the run of constant-index stores to the same array
        j = len(out)
        while j > 0:
            cm = CONST_STORE.match(out[j - 1])
            if not cm or cm.group(1) != base:
                break
            j -= 1
        if j == len(out):
            out.append(lines[i]); i += 1      # nothing aliased; leave it alone
            continue
        tmp = f'kd_saved_{base}_{n}'
        out.insert(j, f'{indent}__typeof__({base}[{idx}]) {tmp} = {base}[{idx}];')
        out.append(f'{indent}{base}[{idx}] = {tmp};')
        n += 1
        i += 1
    return '\n'.join(out), n


EXT_BASE = re.compile(r'&(\w+)(?=\s*[-+])')


def fix_external_base_arithmetic(obj, text):
    """`&pool_ptr + poolstack_ptr * 4` is `(char *)poolstack + -4 + ...`.

    Two separate defects meet on one line, and fixing either alone leaves the
    object wrong in a way that still compiles.

    ONE — THE FOLDED ADDEND. When a relocation carries an addend, Ghidra
    resolves the address into a NEIGHBOUR's EXTERNAL slot and prints the
    neighbour (see relocation_targets). keaPopPoolFrame reads
    `poolstack[poolstack_ptr - 1]`:

        mov -0x4(,%ecx,4),%eax      R_386_32 poolstack

    `poolstack` is slot 2 and the -4 addend lands on slot 1, `pool_ptr`, so
    Ghidra prints `&pool_ptr + poolstack_ptr * 4` — the pop reading from the
    push's SCALAR instead of from the stack array. keaPushPoolFrame, whose
    addend is 0, is printed correctly, so the two halves of one 12-byte array
    come out under two different names.

    TWO — THE SCALE. Ghidra's `&X + i * 4` is BYTE arithmetic. That is
    harmless while `X` is undeclared or an `undefined1[]`, and stops being
    harmless the moment gen_prelude declares the symbol at its real DWARF type:
    `&poolstack` is then `void *(*)[3]` and `+ i * 4` strides twelve bytes at a
    time. So declaring the four symbols — which is what makes the object
    compile — is exactly what would break the line Ghidra got RIGHT. Both sites
    are rewritten to explicit `(char *)` byte arithmetic, which is what the
    machine code does and what Ghidra meant.

    Scoped to a bare `&NAME` that is an UNDEFINED symbol of this object and is
    immediately followed by `+` or `-`, i.e. genuinely a base for arithmetic.
    Across the whole corpus that is one object, keaMemory, and two names.

    THE SELF-CHECK IS NOT OPTIONAL. Choosing a candidate wrongly does not fail
    to compile, it reads the wrong memory. Two checks, and the first alone is
    not enough:

      * the slot arithmetic must reproduce the name Ghidra printed
        (`slot[sym] + addend/4 == slot[printed]`). This verifies the MECHANISM,
        but it cannot choose — the identity reading satisfies it trivially, and
        that both readings satisfy it is precisely what makes the slot
        ambiguous.
      * so the choice is made on where the addend went. Ghidra FOLDS a constant
        addend into the printed base, leaving only the variable part as `+ expr`.
        A site that reads `&X + expr` therefore resolves to the unique candidate
        with a NON-ZERO addend when one exists — the identity reading of an
        indexed base would mean indexing a four-byte scalar, which is not what
        any of these instructions do. With no non-zero candidate the site is
        already correct and only needs the cast, so a single identity candidate
        is accepted and anything else declines.

    Worked example, and both halves are in one function pair. `keaPopPoolFrame`
    offers {(pool_ptr, 0), (poolstack, -4)}: the non-zero rule picks `poolstack`
    for the indexed base, and `fix_mislabelled_external` independently resolves
    the plain value use to `pool_ptr`. `keaPushPoolFrame` offers only
    {(poolstack, 0)}, so it keeps its name and gains the cast."""
    und = undefined_symbols(obj)
    if not und:
        return text, 0
    slot = {s: i for i, s in enumerate(und)}
    names = {m.group(1) for m in EXT_BASE.finditer(text)} & set(slot)
    if not names:
        return text, 0
    per_fn = relocation_targets(obj, per_function=True)

    out, n = [], 0
    for fn, region in _split_definitions(text):
        cands = per_fn.get(fn, {}) if fn else {}

        def rewrite(m):
            nonlocal n
            printed = m.group(1)
            if printed not in slot:
                return m.group(0)
            # relocation_targets keys on the underscore spelling Ghidra gives
            # the four-byte VALUE at the slot; `&X` is the same slot's address.
            got = cands.get('_' + printed)
            if not got:
                return m.group(0)
            ok = [(s, a) for s, a in sorted(got)
                  if s in slot and slot[s] + a // 4 == slot[printed]]
            folded = [c for c in ok if c[1] != 0]
            if len(folded) == 1:
                sym, addend = folded[0]
            elif not folded and len(ok) == 1:
                sym, addend = ok[0]
            else:
                return m.group(0)          # ambiguous — decline, do not pick
            n += 1
            return f'(char *){sym} + ({addend})'

        new = EXT_BASE.sub(rewrite, region)
        out.append(new)
    return ''.join(out), n


_dwarf_size_cache = {}

# `undefined1 in_stack_fffffdd8 [72];` / `MdtKeaParameters in_stack_ffffffa0;`
IN_STACK_DECL = re.compile(
    r'^[ \t]*(.+?)\bin_stack_([0-9a-f]{8})[ \t]*(?:\[[ \t]*(\d+)[ \t]*\])?[ \t]*;[ \t]*$', re.M)
# `pMVar3 = (MeReal *)&stack0xffffffa0;`
STACK_ADDR_ASSIGN = re.compile(
    r'(\w+)\s*=\s*\(\s*([A-Za-z_]\w*)\s*\*\s*\)\s*&stack0x([0-9a-f]{8})\s*;')
# `for (iVar1 = 0x13; iVar1 != 0; iVar1 = iVar1 + -1) {`
COPY_LOOP = re.compile(r'for\s*\(\s*(\w+)\s*=\s*(0x[0-9a-f]+|\d+)\s*;')

# Ghidra's own fixed-width placeholder types, and the C types whose width is not
# in this corpus's DWARF under a name worth looking up.
_FIXED_WIDTH = {'undefined1': 1, 'undefined2': 2, 'undefined4': 4, 'undefined8': 8,
                'undefined': 1, 'byte': 1, 'char': 1, 'short': 2, 'ushort': 2,
                'int': 4, 'uint': 4, 'long': 4, 'ulong': 4, 'float': 4,
                'double': 8, 'MeReal': 4}


def read_ghidra_locals(dump):
    """{function: {name: (stack offset, size, type)}} from `<dump>.locals`.

    DumpDecomp.java emits one row per decompiled local that has stack storage,
    with the offset out of `HighFunction.getLocalSymbolMap()`. That is Ghidra's
    own answer to "what lives at this frame offset", as opposed to the offset
    encoded in the NAME it invented, and the two are not the same kind of fact.

    Absent for a dump directory made before this existed, in which case every
    consumer falls back to the name — see fix_stack_address_name.
    """
    if not dump:
        return {}
    path = dump[:-2] + '.locals' if dump.endswith('.c') else dump + '.locals'
    if not os.path.exists(path):
        return {}
    out = {}
    with open(path) as f:
        for line in f:
            if line.startswith('#'):
                continue
            parts = line.rstrip('\n').split(',', 5)
            if len(parts) != 6:
                continue
            fn, name, off, _hexoff, size, ty = parts
            try:
                out.setdefault(fn, {})[name] = (int(off), int(size), ty)
            except ValueError:
                continue
    return out


def _dwarf_type_size(obj, name):
    """Bytes of the named type, from the object's own DWARF, or None."""
    if name in _FIXED_WIDTH:
        return _FIXED_WIDTH[name]
    # `kd_agg92` is OURS — gen_protos.py invents it as an opaque stand-in for an
    # aggregate passed by value, so it is not in anybody's DWARF and the lookup
    # below returns None. That made fix_stack_address_name decline on the one
    # case in MdtWorld where the frame IS exactly described: 23 words copied
    # into a slot Ghidra declared `kd_agg92 in_stack_ffffff04`, 92 bytes against
    # 92 needed. The size is in the name, by construction, so read it there.
    m = re.fullmatch(r'kd_agg(\d+)', name)
    if m:
        return int(m.group(1))
    if obj is None:
        return None
    key = (obj, name)
    if key in _dwarf_size_cache:
        return _dwarf_size_cache[key]
    _dwarf_size_cache[key] = None
    try:
        import dwarf_structs
        dies = dwarf_structs.parse(obj)
    except Exception:
        return None
    for d in dies.values():
        if d['tag'] not in ('DW_TAG_structure_type', 'DW_TAG_class_type',
                            'DW_TAG_union_type', 'DW_TAG_typedef',
                            'DW_TAG_base_type'):
            continue
        if d['attrs'].get('DW_AT_name') != name:
            continue
        s = dwarf_structs.type_size(dies, d['off'])
        if s:
            _dwarf_size_cache[key] = s
            return s
    return None


def fix_stack_address_name(text, diag, ctx):
    """`&stack0xffffffa0` is the address of the local `in_stack_ffffffa0`.

    Ghidra spells a stack slot's VALUE and its ADDRESS differently — the value
    is `in_stack_<offset>` and gets a declaration, the address is
    `stack0x<offset>` and does not — so a function that copies a by-value
    aggregate into an outgoing argument area comes out referring to storage it
    also declares, under a name that does not exist. GCC says so itself:

        error: 'stack0xffffffa0' undeclared; did you mean 'in_stack_ffffffa0'?

    Both names encode the same frame offset, so the mapping is exact rather than
    inferred. `keaIntegrate_pc` — `MdtKeaIntegrateSystem`, 900 calls per 900
    solver steps — has this as its ONLY error.

    THAT USED TO BE AN ASSUMPTION ABOUT HOW GHIDRA BUILDS NAMES. It is now
    checked against Ghidra's own symbol map, which `DumpDecomp.java` emits
    beside each dump as `<object>.locals`. Over every `stack0x` site in the
    corpus, every one that has a covering local sits at exactly the offset its
    name encodes — no exceptions — so the convention holds and the rule is
    right for the reason it claimed. Where the dump carries no table the rule
    falls back to the name, i.e. to what it did before.

    `tools/frame_offsets.py` prints that join, and it is what to run first on
    any new `stack0x`: it separates "the name is wrong" from "the local is too
    small" from "there is no local here at all", and those need different
    answers. It also settled MeMath — the mapping there is exact and the target
    `MeReal eR[3][3]` is DECLARED AND NEVER WRITTEN, because Ghidra emitted
    `fcos`/`fsin` and discarded the results, so repairing the name would buy a
    compile at the price of reading uninitialised memory.

    THIS IS NOT `materialise_alloca_frame`'S CASE, and the distinction is the
    whole reason it is allowed. That function's docstring declines to touch a
    `stack0xNNNN` because inventing storage for one repeats the fixed-buffer
    mistake of dead end 3. Nothing is invented here: Ghidra already declared the
    local, with a real type, and this only connects the address to it. Where
    there is no such declaration the rule declines.

    THE SIZE CHECK IS WHY THIS IS SAFE, and it is not decoration — one of the
    three sites in the corpus fails it. `keaRbdCore_unified` copies 92 bytes
    (0x17 words) through a pointer into a slot Ghidra declared as
    `undefined1[72]`, so rewriting it would put a 20-byte overflow into an
    object that currently does not compile at all — the exact trade the
    detectors exist to prevent. The written length comes from the copy loop's
    own trip count and the pointer's element type; the declared length from the
    object's DWARF. If either is unavailable, or the block does not fit, the
    rule declines.

    It cannot be left to the compiler either: the repair loop accepts an edit
    when the error count does not GROW, so trading `undeclared` for a failed
    size assertion would be kept rather than reverted."""
    m = re.search(r'[‘\'"]stack0x([0-9a-f]{8})[’\'"] undeclared', diag)
    if not m:
        return None
    off = m.group(1)
    obj = getattr(ctx, 'obj', None)

    out, changed = [], False
    for _fn, region in _split_definitions(text):
        if ('&stack0x' + off) not in region:
            out.append(region)
            continue
        # The local Ghidra declared for this exact offset.
        decl = None
        for d in IN_STACK_DECL.finditer(region):
            if d.group(2) == off:
                decl = d
                break
        if decl is None:
            out.append(region)          # nothing declared here — decline
            continue
        base_type = decl.group(1).strip().split()[-1] if decl.group(1).strip() else ''
        if '*' in decl.group(1):
            declared = 4                              # a pointer-typed slot
        else:
            unit = _dwarf_type_size(obj, base_type)
            declared = None if unit is None else unit * int(decl.group(3) or 1)

        # Ghidra's own frame assignment, where the dump carried one. This does
        # two things the name cannot: it CHECKS that `in_stack_<off>` really is
        # at `<off>` — the assumption this rule rests on — and it supplies the
        # size directly instead of deriving it from a type name.
        entry = (ctx.locals.get(_fn) or {}).get('in_stack_' + off) \
            if getattr(ctx, 'locals', None) else None
        if entry is not None:
            want = int(off, 16)
            if want >= 1 << 31:
                want -= 1 << 32
            if entry[0] != want:
                out.append(region)      # the name and the storage disagree
                continue
            declared = entry[1]

        # How much the copy loop writes through the pointer taken off this slot.
        need = None
        for a in STACK_ADDR_ASSIGN.finditer(region):
            if a.group(3) != off:
                continue
            elem = _FIXED_WIDTH.get(a.group(2))
            loop = COPY_LOOP.search(region, a.end())
            if elem and loop:
                need = int(loop.group(2), 0) * elem
            break

        if declared is None or need is None or declared < need:
            out.append(region)          # unknown or does not fit — decline
            continue
        out.append(region.replace('&stack0x' + off, '&in_stack_' + off))
        changed = True
    return ''.join(out) if changed else None


fix_stack_address_name.file_wide = True


VOID_CALL = re.compile(r'\(\s*\*\s*([A-Za-z_]\w*(?:\s*(?:->|\.)\s*[A-Za-z_]\w*)+)\s*\)\s*\(')


def fix_call_through_void_ptr(line, diag, ctx):
    """`(*handler->fn)(a,b,c)` where the header declares `fn` as `void *`.

    Some metoolkit structs deliberately type a function pointer as `void *`
    because the signature varies with a discriminator field — `MeXMLHandler.fn`
    is either an `MeXMLCallback` or an `MeXMLParseFn` depending on `->type`. C
    will not call through a `void *`, so GCC says `called object is not a
    function or function pointer`, and no generic rule can know which of the two
    a given site meant: only the surrounding branch does.

    So this does NOT try to name the target type. It casts to a function pointer
    built from `__typeof__` of the argument expressions actually present, which
    needs no knowledge of the callee and cannot get the ABI wrong:

        (*(__typeof__(lhs) (*)(__typeof__(a), __typeof__(b)))handler->fn)(a, b)

    THAT IS THE POINT OF `__typeof__` HERE rather than a hand-written signature.
    A cast with `void *` parameters would be a guess, and the guess is exactly
    the one HANDOVER.md §8 records twice — an unprototyped or wrongly-prototyped
    indirect call promoting a `float` to `double`, which is what made
    IxConvexTriList wrong for three sessions. Taking each parameter's type from
    the argument expression means the call site's ABI is whatever the arguments
    already are, so a float stays a float.

    Verified on the only object in the corpus that has this shape,
    `MeXMLParser`. Both of its sites were checked against the machine code
    before this rule was written: the `MeXMLActionCallback` branch pushes
    `(&start, userdata)` and the parse branch pushes `(fi, handler, data)`,
    matching `MeXMLCallback` and `MeXMLParseFn` exactly. Ghidra supplies one
    extra argument at each — harmless, and what the original does too, since it
    pushes four slots and the callee reads its own declared parameters."""
    if 'called object is not a function or function pointer' not in diag:
        return None
    m = VOID_CALL.search(line)
    if not m:
        return None
    target = m.group(1).strip()
    open_paren = line.index('(', m.end() - 1)
    close = _match_bracket(line, open_paren)
    # _match_bracket returns the index JUST PAST the closing bracket, so the
    # argument text stops one short of it.
    if not close:
        return None
    args = [a.strip() for a in _split_arguments(line[open_paren + 1:close - 1])]
    if not args or args == ['']:
        return None                     # no arguments: nothing to build a type from

    lhs = re.match(r'^\s*([A-Za-z_]\w*)\s*=\s*', line)
    ret = '__typeof__(%s)' % lhs.group(1) if lhs else 'int'
    params = ', '.join('__typeof__(%s)' % a for a in args)
    cast = '(*(%s (*)(%s))%s)' % (ret, params, target)
    return line[:m.start()] + cast + line[open_paren:]


def fix_void_assignment(line, diag, ctx):
    """`uVar2 = MeMemoryAPI.destroyAligned(p);` — that function returns void.

    Ghidra models a call's return value from the ABI (eax is live afterwards, so
    it invents a variable) rather than from the callee's type, so a call to a
    void function comes out assigned to something. GCC then says `invalid use of
    void expression` and points at the call.

    Dropping the assignment is exactly right and is not a guess about types: the
    compiler has already resolved the callee and told us its return type is void.
    Contrast dead end 9, where the compiler was reasoning about GHIDRA's types
    and was therefore wrong — here it is reasoning about the real prototype out
    of metoolkit's own headers.

    Deliberately narrow. Only a whole statement of the form `lhs = rhs;` on the
    line GCC named, so a nested or compound expression is left alone rather than
    rewritten by pattern. The assigned variable usually becomes unused, which is
    harmless — the recovered sources compile with -w."""
    if 'invalid use of void expression' not in diag:
        return None
    m = re.match(r'^(\s*)[A-Za-z_]\w*\s*=\s*(.+;)\s*$', line)
    if not m:
        return None
    rhs = m.group(2)
    if '(' not in rhs:                    # not a call; leave it
        return None
    return m.group(1) + rhs


PTR_REF = re.compile(r'(?<![\w])(PTR_[A-Za-z0-9_]*?_([0-9a-f]{8}))\b')


def resolve_ptr_labels(obj, text):
    """`PTR__CxSmallSort_00011f20` is a pointer-sized slot at a known address.

    Ghidra names a data reference `PTR_<what it points at>_<address>` when the
    slot holds a pointer. The address is in its invented memory map, which
    ghidra_memory_map() reproduces, so it can be resolved to a real symbol plus
    an offset — no different from the DAT_ case, except that the answer is a
    location rather than bytes.

    The one that matters is a class's own vtable pointer:

        this->_vptr_CxSmallSort = (...)&PTR__CxSmallSort_00011f20;

    which is the Itanium ABI address point, `&vtable[2]`. gen_vtables.py has
    already re-emitted that vtable, so the define lands on it and the
    constructor sets the right thing.

    Only symbols the object actually defines are used, so nothing is invented."""
    refs = {m.group(1): int(m.group(2), 16) for m in PTR_REF.finditer(text)}
    if not refs:
        return '', 0
    secs, _ext = ghidra_memory_map(obj)
    by_index = {}
    out = subprocess.run(['readelf', '-SW', obj], capture_output=True, text=True).stdout
    for line in out.splitlines():
        m = re.match(r'\s*\[\s*(\d+)\]\s+(\S+)', line)
        if m:
            by_index[m.group(1)] = m.group(2)

    # Symbols with their SECTION. nm gives an offset with no section, and a
    # .text offset can equal a .rodata one — matching without the section put
    # a vtable pointer on a constructor.
    syms = []
    out = subprocess.run(['readelf', '-sW', obj], capture_output=True, text=True).stdout
    for line in out.splitlines():
        m = re.match(r'\s*\d+:\s+([0-9a-f]+)\s+(\d+)\s+(\S+)\s+\S+\s+\S+\s+(\S+)\s+(\S+)', line)
        if not m or m.group(4) in ('UND', 'ABS'):
            continue
        sec = by_index.get(m.group(4))
        if sec and int(m.group(2)) > 0:
            syms.append((sec, int(m.group(1), 16), int(m.group(2)), m.group(5)))
    if not syms:
        return '', 0
    dem = subprocess.run(['c++filt'] + [n for _, _, _, n in syms],
                         capture_output=True, text=True).stdout.split('\n')
    short = {}
    for i, (_sec, _v, _sz, n) in enumerate(syms):
        d = dem[i].strip() if i < len(dem) else n
        short[n] = re.sub(r'\(.*', '', d).strip() or n

    lines, n = [], 0
    for label, addr in sorted(refs.items()):
        hit = None
        for sname, _typ, lo, hi in secs:
            if not lo <= addr < hi:
                continue
            off = addr - lo
            for sec, value, size, sym in syms:
                if sec == sname and value <= off < value + size:
                    hit = (sym, off - value)
                    break
            break
        if hit is None:
            continue
        sym, delta = hit
        # Match whichever generator owns the symbol. gen_vtables emits the C++
        # ABI objects as `kd` + the mangled name (`_ZTV11CxSmallSort` becomes
        # `kd_ZTV11CxSmallSort`); everything else is `kd_` + the demangled name
        # with `::` flattened. Demangling a vtable gives "vtable for X", which
        # is not what anything is called.
        if sym.startswith('_ZT'):
            c = 'kd' + sym
        else:
            flat = re.sub(r'\b([A-Za-z_]\w*)::~([A-Za-z_]\w*)', r'\1__dtor_\2', short[sym])
            c = 'kd_' + re.sub(r'[^A-Za-z0-9_]', '_', flat.replace('::', '__'))
        lines.append(f'/* {label} -> {sym} + 0x{delta:x} */')
        lines.append(f'#define {label} (*(void **)((char *)&{c} + 0x{delta:x}))')
        n += 1
    if not n:
        return '', 0
    return ('/* ---- pointer slots Ghidra named by address ---- */\n'
            + '\n'.join(lines) + '\n\n'), n


def signature_of(body):

    """Extract the declarator text preceding the function's opening brace."""
    brace = body.find('\n{')
    if brace < 0:
        return None
    return ' '.join(body[:brace].split())


# ---------------------------------------------------------------------------
#  Compile-feedback repair
#
#  Some of what Ghidra emits is not C, and the tempting fix is a regex over the
#  whole file. That is how this pipeline nearly shipped a silent miscompile:
#
#      (float)x->member
#
#  is a legal *conversion* when `member` is an int, and a bit *reinterpretation*
#  when it is a pointer. The two are indistinguishable by looking at the text,
#  and rewriting every occurrence to `*(float *)&x->member` turns the first kind
#  into garbage that still compiles. See HANDOVER.md §9 dead end 6.
#
#  The compiler already knows which is which — it says so, with a line number.
#  So nothing below fires on a pattern alone. A rule fires only on a line GCC
#  has ALREADY rejected, only when the diagnostic on that line is one the rule
#  claims, and it rewrites that line and nothing else. Every batch of edits is
#  then kept only if recompiling produces fewer errors than before; a rule that
#  guesses wrong drives the count up and is reverted without anyone noticing it
#  was tried.
#
#  Adding a rule is therefore cheap and safe. Removing the verification is not.
# ---------------------------------------------------------------------------

GCC_DIAG = re.compile(r'^(?P<file>.+?):(?P<line>\d+):(?P<col>\d+):\s+error:\s+(?P<msg>.*)$')

# GCC quotes identifiers with typographic quotes under a UTF-8 locale and ASCII
# ones otherwise; match either so the rules do not depend on the environment.
Q = '[‘\'"]([^’\'"]+)[’\'"]'


def compile_diags(path, cc, cflags):
    """Errors GCC reports *in this file*, as (line, col, message).

    Diagnostics from headers are deliberately dropped: they have no line in the
    generated file to rewrite, and papering over them here would hide a real
    problem in kd_types.h, which fails globally (HANDOVER.md §4)."""
    r = subprocess.run([cc] + list(cflags) + ['-c', '-o', os.devnull, path],
                       capture_output=True, text=True)
    here = os.path.abspath(path)
    diags = []
    for line in r.stderr.splitlines():
        m = GCC_DIAG.match(line)
        if m and os.path.abspath(m.group('file')) == here:
            diags.append((int(m.group('line')), int(m.group('col')), m.group('msg')))
    return diags


# ---- expression scanning -------------------------------------------------
#
# The rules need to know where an operand starts and ends. GCC's column number
# points at the enclosing statement, not at the offending sub-expression, so it
# cannot be used for this; these two scanners find the extent by matching
# brackets, which is exact.

_OPEN, _CLOSE = {'(': ')', '[': ']'}, {')': '(', ']': '['}


def scan_unary_forward(s, i):
    """End of the unary-expression starting at `s[i]`, or None.

    A cast binds tighter than any binary operator, so `(float)a->b - c` casts
    `a->b` and not `a->b - c`. Getting this boundary wrong is how a rewrite
    silently changes the arithmetic.

    NEWLINES COUNT AS WHITESPACE. Ghidra wraps long expressions, so a cast can
    sit at the end of one line with its operand on the next — which is why the
    `multiline` rules are handed a whole statement in the first place. Stopping
    at the newline made those rules silently decline: McdCylinder's

        g[1].next = (McdGeometryID)
                    SQRT(...);

    was left alone while three identical single-line casts in the same file were
    rewritten."""
    n = len(s)
    while i < n and s[i] in ' \t\r\n':
        i += 1
    if i >= n:
        return None
    if s[i] in '-+!~':                 # not addressable; the caller must refuse
        return None
    while i < n and s[i] in '*& \t\r\n':   # dereference / address-of prefixes
        i += 1
    if i >= n:
        return None
    if s[i] == '(':
        i = _match_bracket(s, i)
        if i is None:
            return None
    elif s[i].isalnum() or s[i] == '_':
        while i < n and (s[i].isalnum() or s[i] == '_'):
            i += 1
    else:
        return None
    while i < n:                       # postfix: [..] (..) .name ->name
        if s[i] in '([':
            j = _match_bracket(s, i)
            if j is None:
                return i
            i = j
        elif s[i] == '.' and i + 1 < n and (s[i + 1].isalpha() or s[i + 1] == '_'):
            i += 1
            while i < n and (s[i].isalnum() or s[i] == '_'):
                i += 1
        elif s.startswith('->', i) and i + 2 < n and (s[i + 2].isalpha() or s[i + 2] == '_'):
            i += 2
            while i < n and (s[i].isalnum() or s[i] == '_'):
                i += 1
        else:
            break
    return i


def scan_postfix_backward(s, i):
    """Start of the postfix-expression ending just before `s[i]`, or None."""
    j = i
    while True:
        while j > 0 and s[j - 1] in ' \t':
            j -= 1
        if j > 0 and s[j - 1] in ')]':
            k = _match_bracket_back(s, j - 1)
            if k is None:
                return None
            j = k
        elif j > 0 and (s[j - 1].isalnum() or s[j - 1] == '_'):
            while j > 0 and (s[j - 1].isalnum() or s[j - 1] == '_'):
                j -= 1
        else:
            return None
        # keep walking left through member selectors
        k = j
        while k > 0 and s[k - 1] in ' \t':
            k -= 1
        if k > 0 and s[k - 1] == '.' and not _is_numeric_literal_end(s, k - 1):
            j = k - 1
        elif k > 1 and s[k - 2:k] == '->':
            j = k - 2
        else:
            return j


def _is_numeric_literal_end(s, dot):
    """Is the `.` at s[dot] a float literal's point rather than a selector?

    The guard this replaces looked at ONE character — `s[dot-1].isdigit()` —
    which is true for `1.5` and equally true for `kVar2._kd`, where the base is
    an ordinary identifier that merely ends in a digit. Ghidra names locals
    `kVar2`, `pMVar19`, `local_70`, so that is not a corner case: every subfield
    access on such a base scanned back only as far as the member, and
    fix_subfield_access rewrote `kVar2._kd._56_4_` into
    `kVar2.(*(unsigned int *)((char *)&(_kd) + 56))` — eighteen of MdtWorld's
    thirty errors.

    A numeric literal starts with a digit; an identifier cannot. So read the
    whole token to the left of the dot, not its last character."""
    t = dot
    while t > 0 and (s[t - 1].isalnum() or s[t - 1] == '_'):
        t -= 1
    return t < dot and s[t].isdigit()


def _match_bracket(s, i):
    """Index just past the bracket group opening at `s[i]`."""
    close, depth = _OPEN[s[i]], 0
    for j in range(i, len(s)):
        if s[j] in _OPEN:
            depth += 1
        elif s[j] in _CLOSE:
            depth -= 1
            if depth == 0:
                return j + 1 if s[j] == close else None
    return None


def _match_bracket_back(s, i):
    """Index of the bracket opening the group that closes at `s[i]`."""
    depth = 0
    for j in range(i, -1, -1):
        if s[j] in _CLOSE:
            depth += 1
        elif s[j] in _OPEN:
            depth -= 1
            if depth == 0:
                return j
    return None


# ---- the rules -----------------------------------------------------------

FLOAT_TYPES = ('float', 'double', 'MeReal', 'longdouble')
CAST_TO_FLOAT = re.compile(r'\((%s)\)' % '|'.join(FLOAT_TYPES))


def fix_pointer_as_float(line, diag, ctx):
    """`(float)x->member` where `member` is a POINTER.

    The original loaded four bytes into an FP register. Ghidra typed the memory
    as a pointer, so it had to emit a cast, and C rejects casting a pointer to a
    float. The intent is a reinterpretation of those four bytes, which is what
    `*(float *)&expr` says.

    This must NEVER run on a line GCC accepts: where `member` really is an int,
    the same text is an ordinary int-to-float conversion and this rewrite would
    replace the converted value with the bit pattern.

    All the casts in a statement are rewritten at once WHEN GCC reported exactly
    as many errors as there are casts — then every one of them is a pointer and
    there is nothing to choose between. `SQRT((float)a * (float)a + b * b +
    (float)c * (float)c)` is four casts and four diagnostics. If the counts
    disagree, some cast on that line is a legal conversion, so the rule falls
    back to one per pass and lets the loop's verification decide."""
    casts = []
    pos = 0
    while True:
        m = CAST_TO_FLOAT.search(line, pos)
        if not m:
            break
        end = scan_unary_forward(line, m.end())
        pos = m.end()
        if end is None or end <= m.end():
            continue
        operand = line[m.end():end].strip()
        if operand:
            casts.append((m.start(), m.end(), end, m.group(1), operand))
            pos = end
    if not casts:
        return None
    def build(chosen):
        out, last = [], 0
        for start, mid, end, typ, operand in chosen:
            out.append(line[last:start])
            out.append(f'(*({typ} *)&({operand}))')
            last = end
        out.append(line[last:])
        return ''.join(out)

    if len(casts) == getattr(ctx, 'n_same', 0):
        return build(casts)
    # The counts disagree, so at least one cast on this line is a conversion GCC
    # ACCEPTS. Rewrite only the first and let the loop's verification decide.
    #
    # DO NOT "improve" this by cycling through the casts until one is accepted.
    # It was tried on 2026-08-24 and it works, in the sense that McdSphyl then
    # compiles — and what it compiles is wrong. The line is
    #
    #     *radius = (float)s[1].mRefCtAndID + (float)s[1].prev;
    #
    # GCC rejects only the second cast, so cycling rewrites that one and leaves
    # the first as an integer-to-float CONVERSION. The original does no such
    # thing:
    #
    #     flds  0x10(%ecx)      ; load a float
    #     fadds 0x14(%ecx)      ; add a float
    #
    # Both operands are floats; Ghidra mis-typed the first member as an integer,
    # so the "legal conversion" is legal C and wrong semantics. This is dead end
    # 6 turned around: there, the compiler was the only thing that knew, and
    # here the compiler is wrong because it is reasoning about Ghidra's types.
    # Leaving the object in the FAIL pile keeps the defect visible.
    return build(casts[:1])


fix_pointer_as_float.multiline = True

SUBFIELD = re.compile(r'\.\s*_(\d+)_(\d+)_')

NO_MEMBER = re.compile(
    r'[‘\'"]([^’\'"]+)[’\'"](?:\s*\{aka\s*[‘\'"]([^’\'"]+)[’\'"]\})?'
    r'\s*has no member named\s*[‘\'"]field_0x([0-9a-f]+)[’\'"]')


def fix_field_offset(line, diag, ctx):
    """`this->field_0x8` when GCC knows what `this` points at.

    resolve_field_names() already maps offsets back to member names, but it has
    to work out the variable's type by reading its declaration, and that fails
    for a parameter it cannot parse or a typedef it cannot follow. GCC has none
    of those problems and puts the answer in the diagnostic:

        ‘keaMatrix_pcSparse_vanilla’ {aka ‘keaMatrix_pcSparse’} has no member
        named ‘field_0x8’

    so the offset can be looked up against the type GCC actually resolved.
    With no member at that offset the reference is to PADDING — McdContact has
    `short dims` at 28 and a 4-aligned union at 32, and gcc copies the two bytes
    between them because it moved the struct in wider chunks. Byte arithmetic
    says exactly that and stays correct where no member name can exist."""
    m = NO_MEMBER.search(diag)
    if not m:
        return None
    spelled, aka, off = m.group(1), m.group(2), int(m.group(3), 16)
    field = 'field_0x%x' % off
    member = None
    for t in (aka, spelled):
        if not t:
            continue
        t = t.replace('struct ', '').strip()
        member = (ctx.fieldmap.get(t, {}).get(str(off))
                  or ctx.fieldmap.get('_' + t, {}).get(str(off)))
        if member:
            break
    if member:
        new = re.sub(r'\b' + field + r'\b', member, line)
        return new if new != line else None

    def byte_ref(mm):
        return f'(*(char *)((char *)({mm.group(1)}) + 0x{off:x}))'
    # The base may be SUBSCRIPTED. `McdContactSimplify` copies a contact with
    # `pMVar23[1].field_0x1e`, and a bare-identifier base declined on it
    # silently while rewriting the `->` read of the same padding two lines up —
    # so one half of a two-line copy was repaired and the other was not.
    # A nested subscript (`p[a[0]].f`) still declines, which is the safe answer.
    base = r'([A-Za-z_]\w*(?:\s*\[[^\]\[]*\])*)'
    new = re.sub(base + r'\s*->\s*' + field + r'\b', byte_ref, line)
    new = re.sub(base + r'\s*\.\s*' + field + r'\b',
                 lambda mm: f'(*(char *)((char *)&({mm.group(1)}) + 0x{off:x}))', new)
    return new if new != line else None


SCALAR_KEYWORDS = {
    'int', 'char', 'short', 'long', 'signed', 'unsigned', 'float', 'double',
    'void', 'uint', 'ushort', 'uchar', 'byte', 'sbyte', 'bool', 'size_t',
    'longlong', 'ulonglong', 'longdouble', 'MeReal', 'MeI16', 'MeU32', 'MeI32',
    'undefined', 'undefined1', 'undefined2', 'undefined4', 'undefined8',
}
ANY_CAST = re.compile(r'\(\s*((?:struct\s+)?[A-Za-z_]\w*)\s*\)')
# A cast with an EXPLICIT star. Kept separate from ANY_CAST rather than folded
# into it, because fix_aggregate_as_integer also uses that pattern and filters on
# SCALAR_KEYWORDS — widening it there would make `(int *)x` match as `int` and be
# rewritten to `(*(int *)&(x))`, which is a different and wrong transformation.
PTR_CAST = re.compile(r'\(\s*((?:struct\s+)?[A-Za-z_]\w*)\s*\*+\s*\)')


def fix_float_as_pointer(line, diag, ctx):
    """`p->member = (McdGeometryID)(dy * 0.5)` — the mirror of the case above.

    Ghidra typed a four-byte slot as a pointer and the original stored a float
    in it, so the cast has nothing to convert: the bytes are already what the
    original wrote. KD_FBITS takes them across without arithmetic, and the
    pointer cast that follows is then an ordinary integer-to-pointer conversion.

    Casts to a scalar keyword are skipped, because `(int)f` is a legal
    conversion and rewriting it would replace a rounded value with a bit
    pattern — the same mistake as dead end 6, pointing the other way.

    BOTH SPELLINGS OF A POINTER CAST. The rule was written for a typedef that
    already IS a pointer (`McdGeometryID`), and silently did nothing for an
    explicit `(MeFAsset *)` because ANY_CAST has no star in it. That left
    MeFAsset one error from compiling —

        pMVar25 = (MeFAsset *)asset->massScale;   /* massScale is a MeReal */
        ...
        asset_1->massScale = (*(MeReal *)&(pMVar25));

    which is Ghidra carrying a float through a slot it typed as a pointer, then
    reading the bytes back.

    THE SCALAR FILTER APPLIES TO THE STARRED FORM TOO, and a first version that
    skipped it broke McdCylinder. The reasoning for skipping looked sound — no
    float converts to `int *` either — but it ignores that this rule rewrites
    the FIRST cast it can find on the line, not necessarily the one GCC is
    complaining about. `*(float *)&x` is an ordinary bit-reinterpretation read
    and perfectly legal, so wrapping its operand produced

        (*(float *)KD_FBITS(&(pMVar1)))

    and three new errors in an object that had none. A cast to a scalar type is
    left alone whether or not it has a star."""
    for pat in (ANY_CAST, PTR_CAST):
        for m in pat.finditer(line):
            if m.group(1).replace('struct ', '') in SCALAR_KEYWORDS:
                continue
            end = scan_unary_forward(line, m.end())
            if end is None or end <= m.end():
                continue
            raw = line[m.end():end]
            if not raw.strip() or raw.lstrip().startswith('KD_FBITS'):
                continue
            # Wrap the operand WITHOUT collapsing its whitespace. Ghidra wraps
            # long expressions, so the operand often begins on the next line,
            # and the repair loop rejects any multiline rewrite that changes the
            # line count — stripping here made this rule silently decline on
            # exactly the wrapped statements it was extended to handle.
            return line[:m.end()] + 'KD_FBITS(' + raw + ')' + line[end:]
    return None

fix_float_as_pointer.multiline = True

SUBFIELD_WIDTH = {1: 'unsigned char', 2: 'unsigned short',
                  4: 'unsigned int', 8: 'unsigned long long'}


def fix_subfield_access(line, diag, ctx):
    """`x._0_1_` is Ghidra for "the byte at offset 0 of x".

    It appears wherever the original wrote a register subfield — `mov %al, ...`
    against a variable Ghidra typed as a whole word. The offset and the width
    are both spelled out in the name, so the rewrite is a transcription rather
    than an inference. Little-endian is assumed, which holds for every target
    this project cares about (i386, x86-64, wasm32, arm64 and armv7 LE)."""
    for m in SUBFIELD.finditer(line):
        width = SUBFIELD_WIDTH.get(int(m.group(2)))
        if width is None:
            continue
        start = scan_postfix_backward(line, m.start())
        if start is None:
            continue
        expr = line[start:m.start()].strip()
        if not expr:
            continue
        off = int(m.group(1))
        return (line[:start]
                + f'(*({width} *)((char *)&({expr}) + {off}))'
                + line[m.end():])
    return None


MISLABELLED = re.compile(r'\b_(\w+)\b')


DEF_BANNER = re.compile(r'^/\* ---- (\S+) \(', re.M)


# `_vanillaQMatrix = (undefined4 *)&__gxx_personality_v0;`
# `_vanillaFunctions = _ZN12keaFunctions8initPoolEPvi;`
VPTR_STORE = re.compile(
    r'^([ \t]*)_(\w+)[ \t]*=[ \t]*'
    r'(?:\([ \t]*([A-Za-z_]\w*)[ \t]*\*[ \t]*\)[ \t]*)?'
    r'&?(\w+)[ \t]*;[ \t]*$', re.M)

# The identifier some other part of the pipeline declared for a mangled symbol.
# gen_prelude writes `extern void *vtable_for_X[] KD_MANGLED("_ZTVN…")`,
# gen_vtables `extern const void *kd_ext__ZTVN…[] __asm__("_ZTVN…")`. Either is
# a usable base; a declaration that is NOT an array is not, which is why the
# `[]` is part of the pattern rather than optional.
def _declared_array_for(text, sym):
    m = re.search(r'\b(\w+)[ \t]*\[[ \t]*\][ \t\n]*'
                  r'(?:KD_MANGLED|__asm__)\([ \t]*"' + re.escape(sym) + r'"[ \t]*\)',
                  text)
    return m.group(1) if m else None


# The identifier a declaration introduces is the last one directly followed by
# `(` or `[` before the asm label — the declarator, in every shape this pipeline
# emits: `extern void f(...) KD_MANGLED(…)`, `void __thiscall kd_f (…) …`,
# `extern void *v[] …`, `float kd_x[3] …`.
_DECLARATOR = re.compile(r'\b([A-Za-z_]\w*)[ \t\n]*[(\[]')
_ASM_LABEL = re.compile(r'(?:KD_MANGLED|__asm__)\([ \t]*"([^"]+)"[ \t]*\)')


def declared_under(declared):
    """{ELF symbol: the C identifier this unit declares it as}."""
    out = {}
    for stmt in declared.split(';'):
        m = _ASM_LABEL.search(stmt)
        if not m:
            continue
        head = stmt[:m.start()]
        idents = [d for d in _DECLARATOR.findall(head)
                  if d not in ('KD_MANGLED', '__asm__')]
        if idents and idents[-1] != m.group(1):
            out[m.group(1)] = idents[-1]
    return out


def resolve_mangled_call_names(text, declared):
    """`_ZN12keaLCPSolver8setUpperEPf(x)` is a call to something already declared.

    Ghidra spells an intra-class or imported C++ call site either with the bare
    method name or with the MANGLED symbol, and which one it picks is not under
    our control. `cxx_names_to_c` flattens `Class::method` to `Class__method`
    and gen_prelude declares imports under the same flattened spelling, so a
    call site Ghidra spelled mangled matches neither and comes out as an
    implicit declaration of a function that does not exist.

    This is a DEBT `out10` created rather than a defect it exposed. Before it,
    ParseKarmaHeaders had no prototype for any mangled symbol and Ghidra
    arity-guessed those calls from the call site; giving every C++ function a
    prototype under its linkage name (§5b) fixed the arguments and moved the
    spelling. `keaLCP_new` went from 10 errors to 11 in that trade, and it was
    still worth it — a call with the right arguments and the wrong spelling
    fails to COMPILE, while the other way round it silently linked.

    The map is READ from the declarations this unit is about to emit — its own
    forward declarations and its prelude — keyed on the asm label, which is the
    ELF symbol and therefore the only thing the two spellings share. Nothing is
    demangled and nothing is matched by name. Scoped to `_Z…` symbols: an
    ordinary C name is declared under its own spelling, and rewriting those
    would collide with the renaming passes that already own them."""
    names = {s: c for s, c in declared_under(declared).items()
             if s.startswith('_Z')}
    if not names:
        return text, 0
    pat = re.compile(r'(?<![\w.>])(' + '|'.join(re.escape(s) for s in
                                                sorted(names, key=len, reverse=True))
                     + r')\b')
    return pat.subn(lambda m: names[m.group(1)], text)


_vptr_store_cache = {}


def _vptr_stores_cached(obj, func_symbol):
    key = (obj, func_symbol)
    if key not in _vptr_store_cache:
        try:
            _vptr_store_cache[key] = vtable_slots.vptr_stores(obj, func_symbol)
        except Exception:
            _vptr_store_cache[key] = {}
    return _vptr_store_cache[key]


def _demangled_param_count(mangled):
    """How many parameters the demangling of `mangled` declares, or None.

    `this` is NOT counted — the caller adds it, because every slot in this
    corpus is a non-static member function and the call site passes the object
    as its first argument."""
    dem = subprocess.run(['c++filt', mangled], capture_output=True,
                         text=True).stdout.strip()
    if not dem or dem == mangled:
        return None
    dem = re.sub(r'\s*const\s*$', '', dem)
    if not dem.endswith(')'):
        return None
    open_at = _match_bracket_back(dem, len(dem) - 1)
    if open_at is None:
        return None
    inner = dem[open_at + 1:-1].strip()
    if not inner or inner == 'void':
        return 0
    return len(_split_args(inner))


def fix_vptr_store(obj, text, declared=None, locals_table=None):
    """`_vanillaQMatrix = &__gxx_personality_v0;` is `vanillaQMatrix.vptr = &vtable[2]`.

    A function that constructs a polymorphic class as a LOCAL stores the vtable
    ADDRESS POINT into its first word, and Ghidra loses that: the store becomes
    an assignment to an undeclared `_<local>`, and the right-hand side is a
    neighbour's EXTERNAL slot (the addend collision of relocation_targets), so
    `keaLCPSolver` reads `&__gxx_personality_v0` where the object file says
    `_ZTV26keaMatrix_pcSparse_vanilla + 8`. Every virtual call in the function
    then goes through the undeclared name and the object does not compile.

    THE REWRITE. `_<local>` is the local's first word, so every occurrence
    becomes `(*(E **)&<local>)` and the store's right-hand side becomes the
    vtable's address point. Nothing about the frame is reconstructed — the
    storage is the local Ghidra already declared, and the recovered function
    only has to be self-consistent with itself.

    WHAT MAKES IT BELIEVABLE, because a wrong slot calls the wrong function and
    still compiles. Six checks, each READ from the object file:

      1. `vtable_slots.vptr_stores` disassembles the SHIPPED function and reads
         which class's address point it stores, and at which stack offset. That
         is the fact the rewrite rests on; see its docstring for why the call
         sites are the wrong thing to verify against.
      2. the relocation at the store must name `_ZTV<class>` with addend
         exactly 8 — the Itanium address point. A secondary vtable of a
         multiply-inherited class would have a different addend and is refused.
      3. the local's DECLARED type must be that same class.
      4. the class must have exactly one such local in the function and the
         function exactly one store of it, so there is nothing to choose
         between.
      5. `<object>.locals` — Ghidra's own frame assignment, not the offset
         encoded in a name — must put that local at the offset the machine code
         stores to. Ghidra's offsets are relative to the entry ESP and the
         disassembly's to EBP, which differ by the saved EBP: +4.
      6. every offset the call sites use must be a real slot in that class's
         vtable, read from the defining object's relocations, and the site's
         argument count must equal that slot's parameter count plus `this`.

    Check 6 is what settles the one thing NOT read from the object: the scale.
    Ghidra types `_vanillaQMatrix` as `undefined4 *` in `makeXandW`, so `[2]` is
    the third WORD, and leaves it byte-like in `PrincipalSubmatrix`, so `+ 0xc`
    is the fourth. Read the second at word scale and the offsets are 48 and 64
    in a table whose last slot is 32; read the first at byte scale and `[2]` is
    not a multiple of four. Either error is refused rather than compiled.

    Corpus-wide this fires on two objects — the only two that import a vtable —
    and on four functions between them.

    It is a PRE-PASS and not a REPAIR_RULES entry on purpose. Nothing about it
    is driven by a diagnostic: the evidence is the object's relocations and the
    shipped disassembly, so there is no reason to wait for GCC to complain, and
    the `undeclared` pattern is already spoken for by fix_mislabelled_external —
    a second entry with that pattern would be dead code (see the note above
    REPAIR_RULES)."""
    if not obj:
        return text, 0
    corpus = os.path.dirname(os.path.abspath(obj))
    declared = declared if declared is not None else text
    locals_table = locals_table or {}

    out, n = [], 0
    for fn, region in _split_definitions(text):
        if not fn:
            out.append(region)
            continue
        # A function may construct more than one polymorphic local —
        # MdtKeaAddConstraintForces builds both a keaFunctions_Vanilla and a
        # keaMatrix_pcSparse_vanilla. Each store is judged on its own evidence,
        # and one that cannot be resolved leaves its own error standing rather
        # than costing the others their repair.
        for store in VPTR_STORE.finditer(region):
            new = _repair_vptr_store(fn, region, store.groups(), declared, obj,
                                     corpus, locals_table)
            if new is not None:
                region, n = new, n + 1
        out.append(region)
    return ''.join(out), n


def _repair_vptr_store(fn, region, groups, declared, obj, corpus, locals_table):
    _indent, name, cast, label = groups

    # (3) the local, and (4) it must be the only one of its type here.
    decls = re.findall(r'^[ \t]*([A-Za-z_]\w*)[ \t]+(\w+)[ \t]*;[ \t]*$',
                       region, re.M)
    ltype = next((t for t, v in decls if v == name), None)
    if not ltype or sum(1 for t, _v in decls if t == ltype) != 1:
        return None

    # (2) the relocation under the store, resolved in THIS function.
    per_fn = relocation_targets(obj, per_function=True)
    cands = per_fn.get(fn, {}).get('_' + label)
    if not cands or len(cands) != 1:
        return None
    sym, addend = next(iter(cands))
    if addend != 8 or not sym.startswith('_ZTV'):
        return None
    if vtable_slots.mangle_class(ltype) != sym[len('_ZTV'):]:
        return None

    # (1) what the shipped machine code stores, and where.
    m = re.search(r'asm label "(\S+)"', region)
    if not m:
        return None
    dem = subprocess.run(['c++filt', m.group(1)], capture_output=True,
                         text=True).stdout.strip()
    st = _vptr_stores_cached(obj, dem or m.group(1))
    offsets = st.get(ltype)
    if not offsets or len(offsets) != 1:
        return None

    # (5) Ghidra's frame assignment for that local must be the same slot.
    ghidra_local = locals_table.get(fn, {}).get(name)
    if ghidra_local is not None and ghidra_local[0] + 4 != offsets[0]:
        return None

    table = vtable_slots.slot_table(corpus, ltype)
    if not table:
        return None

    etype = cast or 'char'
    esize = _FIXED_WIDTH.get(etype)
    if not esize:
        return None

    alias = f'(*({etype} **)&{name})'
    slot0 = f'(*(code **)({alias}))'

    # (6) every use is a dispatch through a real slot, with the right arity.
    uses = [(re.compile(r'(?<![\w])_' + re.escape(name) + r'\b'), esize, alias),
            (re.compile(r'(?<![\w])_' + re.escape(label) + r'\b'), None, slot0)]
    sm = re.search(r'^([ \t]*)_' + re.escape(name) + r'[ \t]*=[^;\n]*;[ \t]*$',
                   region, re.M)
    if not sm:
        return None
    store_at = sm.start() + len(sm.group(1))
    skip = set()
    for pat, scale, _sub in uses:
        for u in pat.finditer(region):
            if u.start() == store_at:
                continue                # the store's own left-hand side
            site = _vptr_call_site(region, u.end(), scale)
            if site is None:
                return None
            off, nargs = site
            if off not in table:
                return None
            want = _demangled_param_count(table[off])
            if want is None:
                return None
            if nargs == want + 1:
                continue
            # Ghidra dropped this call's arguments — a separate defect, and not
            # one this rule may paper over: rewriting the site would compile
            # into a call to the right function with the wrong stack, which is
            # the one outcome worse than not compiling. Leave the site, and its
            # diagnostic, exactly as they are. An argument count that is wrong
            # in any OTHER way is not "dropped", it is "misread", and then the
            # slot derivation itself is suspect and the whole store declines.
            if nargs:
                return None
            skip.add(u.start())

    vname = _declared_array_for(declared, sym)
    if not vname:
        return None

    new = region
    for pat, _scale, sub in uses:
        new = pat.sub(lambda m, s=sub: m.group(0) if m.start() in skip else s, new)
    store = re.compile(r'^([ \t]*)' + re.escape(alias) + r'[ \t]*=[^;\n]*;[ \t]*$',
                       re.M)
    new, k = store.subn(
        lambda mm: f'{mm.group(1)}{alias} = ({etype} *)((char *)&{vname}[0] + 8);',
        new)
    return new if k == 1 else None


def _vptr_call_site(region, pos, scale):
    """(slot byte offset, argument count) for the dispatch starting at `pos`.

    `pos` is just past the mislabelled name. Four shapes reach here and they
    differ only in how the offset is spelled:

        (*(code *)*_V)(...)             +0, Ghidra's word-typed base
        (*(code *)_V[2])(...)           word 2
        (**(code **)(_V + 0xc))(...)    byte 0xc
        (**(code **)_V)(...)            +0
        (*_L)(...)                      +0, the slot's VALUE rather than the base

    Anything else — a plain read, a store, an address taken — returns None, and
    the caller then declines the whole function. That is deliberate: a use this
    does not recognise is a use whose offset it cannot derive either."""
    off = 0
    if scale is not None:
        m = re.match(r'[ \t]*\[[ \t]*(0x[0-9a-f]+|\d+)[ \t]*\]', region[pos:])
        if not m:
            m = re.match(r'[ \t]*\+[ \t]*(0x[0-9a-f]+|\d+)\b', region[pos:])
        if m:
            off = int(m.group(1), 0) * scale
            pos += m.end()
    j = pos
    while j < len(region) and region[j] in ') \t\n':
        j += 1
    if j >= len(region) or region[j] != '(':
        return None
    end = _match_bracket(region, j)
    if end is None:
        return None
    args = [a for a in _split_args(region[j + 1:end - 1]) if a.strip()]
    return off, len(args)


def fix_mislabelled_external(text, diag, ctx):
    """`(*_McdGeometryDeinit)(0x20, 0x10)` is `MeMemoryAPI.createAligned(...)`.

    Ghidra printed the wrong name for the right address; see relocation_targets()
    for the mechanism and the evidence. Two shapes come out of it:

      * a CALL through the slot. Here the rewrite must name the struct member,
        because that is what supplies the prototype: calling through an
        unprototyped pointer would default-promote a float argument to double
        and silently change the ABI. If the member cannot be named, the rule
        declines and the object stays in review rather than being guessed at.
      * a plain READ of the slot, which has no calling convention to get wrong,
        so the exact four-byte access Ghidra meant is always available.

    Resolution is per FUNCTION, because one name can stand for two addresses in
    one file. It is not per line: GCC reports an undeclared name once per
    function, so a line-at-a-time rule would need one compile per occurrence to
    reach the same answer, and every occurrence within a function is the same
    defect with the same fix."""
    m = re.search(r'[‘\'"]_(\w+)[’\'"] undeclared', diag)
    if not m:
        return None
    ghidra_name = '_' + m.group(1)

    out, changed = [], False
    for fn, region in _split_definitions(text):
        cands = ctx.externals.get(fn, {}).get(ghidra_name) if fn else None
        if not cands:
            # gcc 3.2 inlines a LOCAL copy of putchar into objects that print,
            # and its reference to `stdout` is not always attributable to a
            # named function's byte range. Fall back to the whole file, but only
            # when the file offers exactly one reading and it is the identity
            # one — `_X` at X's own slot, addend 0. That cannot be the
            # mislabelled case, which is by definition a non-zero addend from
            # somewhere else.
            cands = {c for d in ctx.externals.values()
                     for c in d.get(ghidra_name, ())}
            if len(cands) != 1 or next(iter(cands))[1] != 0:
                cands = None
        new = _resolve_external(region, ghidra_name, cands, ctx) if cands else None
        out.append(new if new is not None else region)
        changed = changed or new is not None
    return ''.join(out) if changed else None


fix_mislabelled_external.file_wide = True


def not_code(name, obj, body=''):
    """Did Ghidra make a "function" out of bytes that are not code?

    Two ways to know, and both are facts about the object file rather than
    judgements about the decompilation.

    ONE — THE ADDRESS IS OUTSIDE `.text`. Ghidra names a function it found no
    symbol for after its address, and its address space is the invented one of
    §5: allocatable sections laid out consecutively from 0x10000. So the address
    says which SECTION the "function" came from. MdtBcl's `FUN_00021130` is at
    0x21130; that object's `.text` ends at 0x21013, and 0x21130 lands in
    `.eh_frame`. Ghidra disassembled exception-handling metadata as i386 code,
    and the body reads exactly like that once you look: `swi` (an ARM
    instruction), `in` (x86 port I/O), `uleb128`, `bRam00000018`. Twenty-two of
    MdtBcl's thirty-one errors were in that one function.

    TWO — THE BODY IS NOTHING BUT `halt_baddata()`. That is Ghidra's marker for
    bytes it could not decode at all, so such a function contains ZERO recovered
    instructions. MdtBcl has two, `FUN_000200f0` and `FUN_00020120`, and nothing
    calls either.

    Those diagnostics are the useful signal and must NOT be answered by defining
    `undefined6`, `SCARRY1`, `POPCOUNT` and `halt_baddata` in kd_compat.h —
    that would make the noise compile and publish bogus symbols. Dropping loses
    nothing, because there is nothing there.

    Only `FUN_`-named functions are eligible for the address test, since a
    symbol-named function is at an address the symbol table vouches for.
    Corpus-wide the two tests fire on one object between them: MdtBcl."""
    if re.fullmatch(r'\s*\{?\s*halt_baddata\(\);\s*\}?\s*',
                    re.sub(r'/\*.*?\*/', '', body, flags=re.S).split('\n{', 1)[-1]):
        return True
    m = re.fullmatch(r'FUN_([0-9a-f]{8})', name or '')
    if not m or not obj:
        return False
    try:
        secs, _ext = ghidra_memory_map(obj)
    except Exception:
        return False
    text = [s for s in secs if s[0] == '.text']
    if not text:
        return False
    return not (text[0][2] <= int(m.group(1), 16) < text[0][3])


def _split_definitions(text):
    """[(function or None, text)] — the file cut at ghidra_clean's own banners."""
    parts, last, name = [], 0, None
    for m in DEF_BANNER.finditer(text):
        parts.append((name, text[last:m.start()]))
        name, last = m.group(1), m.start()
    parts.append((name, text[last:]))
    return parts


def _resolve_external(region, ghidra_name, candidates, ctx):
    # Choose between candidates on evidence, never on preference. A base symbol
    # the public headers declare as a struct WITH a member at this exact offset
    # is a description of a function-pointer table slot, which is what an
    # indirect call through a data symbol is. Nothing else in the candidate set
    # explains the site at all.
    named = []
    for sym, addend in sorted(candidates):
        typ = ctx.extern_types.get(sym)
        if not typ:
            continue
        member = (ctx.fieldmap.get(typ, {}).get(str(addend))
                  or ctx.fieldmap.get('_' + typ, {}).get(str(addend)))
        if member:
            named.append((sym, addend, member))

    call = re.compile(r'\(\s*\*\s*' + re.escape(ghidra_name) + r'\s*\)\s*\(')
    if len(named) == 1:
        sym, _addend, member = named[0]
        region = call.sub(f'({sym}.{member})(', region)
        return re.sub(r'(?<![\w])' + re.escape(ghidra_name) + r'\b',
                      f'{sym}.{member}', region)
    if named:
        return None                        # still ambiguous; a human should look
    if call.search(region):
        return None                        # a call with no prototype to give it
    zero = [s for s, a in sorted(candidates) if a == 0]
    if len(zero) != 1:
        return None
    return re.sub(r'(?<![\w])' + re.escape(ghidra_name) + r'\b', zero[0], region)


def fix_too_many_arguments(line, diag, ctx):
    """Drop the arguments past the end of a known prototype.

    Ghidra counts a call's arguments from the pushes it can see before it, and
    over-counts when the surrounding code adjusts the stack for its own reasons.
    MdtBodyCreate's `MePoolAPI.init(pool, size, structSize, align)` came back
    with eight.

    On cdecl the CALLER cleans the stack and the callee reads only as far as its
    prototype, so the extra pushes are invisible to it: cutting the list back to
    the declared arity reproduces exactly what the original callee saw. This is
    only safe because the arity comes from a real prototype — which is precisely
    what GCC is complaining about, so the rule can never run without one."""
    m = re.search(r'too many arguments to function', diag)
    if not m:
        return None
    best = None
    for call in re.finditer(r'\)\s*\(', line):        # `(x.y)(a, b, ...)`
        open_paren = call.end() - 1
        close = _match_bracket(line, open_paren)
        if close is None:
            continue
        args = _split_arguments(line[open_paren + 1:close - 1])
        if len(args) > 1 and (best is None or len(args) > best[2]):
            best = (open_paren, close, len(args), args)
    if best is None:
        return None
    open_paren, close, n, args = best
    return (line[:open_paren + 1] + ', '.join(a.strip() for a in args[:n - 1])
            + line[close - 1:])


def _split_arguments(s):
    """Top-level comma split, ignoring commas inside brackets."""
    out, depth, start = [], 0, 0
    for i, ch in enumerate(s):
        if ch in _OPEN:
            depth += 1
        elif ch in _CLOSE:
            depth -= 1
        elif ch == ',' and depth == 0:
            out.append(s[start:i])
            start = i + 1
    out.append(s[start:])
    return out


CODE_PTR_DECL = re.compile(r'^\s*code\s*\*\s*(\w+)\s*;\s*$', re.M)
# `*(float *)(expr)`, `*(void **)(expr)` — the cast Ghidra puts on an argument
# it loaded out of the frame, and the only statement of that argument's type
# there is. One star is the dereference; the rest belong to the type.
ARG_CAST = re.compile(r'^\s*\*\s*\(\s*([A-Za-z_]\w*(?:\s+[A-Za-z_]\w*)*)'
                      r'\s*(\*+)\s*\)')
# Types narrower than int/double, i.e. exactly the ones default argument
# promotion changes.
NARROW = {'float', 'MeReal', 'short', 'ushort', 'undefined2', 'MeI16', 'MeU16',
          'char', 'byte', 'undefined1', 'MeI8', 'MeU8', 'bool'}


def prototype_indirect_calls(body):
    """Give `(*pcVar)(...)` a prototype when an argument is narrower than int.

    `code` is `typedef int code();` — a function type with NO parameter list —
    so a call through `code *` is unprototyped and C applies the default
    argument promotions. A `float` argument is pushed as an 8-byte double, and
    the callee, which does have a prototype, reads the low half as its float
    and the high half as the argument after it.

    Not theoretical, and not small. All four TriangleList interactions call the
    engine's McdTriangleListFnPtr this way, handing it the bounding-sphere
    radius of the other body:

        pair                        radius the generator received
        Sphere     x TriangleList   2048         (should be 0.91)
        Sphyl      x TriangleList   8.796e+12    (should be 0.91)
        Box        x TriangleList   6.019e-36    (should be 0.939)
        ConvexMesh x TriangleList   -0           (should be 1.019)

    UT2004's KTriListGenerator turns that into a sphere query against the
    level, so a radius landing too LARGE is harmless — the query returns a
    superset and the same contacts come out — and one landing too SMALL returns
    nothing. That is the whole of IxConvexTriList's in-game divergence, and it
    is why IxSphereTriList and IxSphylPrimitives passed 1.76 M and 74,921 real
    calls with the same defect: their garbage radius happened to be enormous.

    Nothing in the pipeline could see it. The code reads correctly, every store
    pairs with its load, it compiles clean, all three substitute scenes pass,
    and difftest_pair's generator ignored pos and radius, so the argument was
    never looked at. It took making that generator cull to the query sphere and
    then printing what each side passed.

    The prototype is synthesised from the casts Ghidra already wrote on the
    arguments, so it needs no outside knowledge of the callee. Only calls with
    a narrow argument are rewritten: promotion is a no-op for int and pointer,
    so touching those would be churn with a chance of being wrong."""
    ptrs = set(CODE_PTR_DECL.findall(body))
    if not ptrs:
        return body, 0
    out, i, n = [], 0, 0
    for m in re.finditer(r'\(\s*\*\s*(\w+)\s*\)\s*\(', body):
        if m.group(1) not in ptrs or m.start() < i:
            continue
        close = _match_bracket(body, m.end() - 1)
        if close < 0:
            continue
        types = []
        for a in _split_arguments(body[m.end():close]):
            c = ARG_CAST.match(a)
            if not c:
                types = None
                break
            types.append(c.group(1).strip() + ' ' + c.group(2)[:-1])
        if not types or not any(t.strip() in NARROW for t in types):
            continue
        proto = 'int (*)(' + ', '.join(t.strip() for t in types) + ')'
        out.append(body[i:m.start()])
        out.append(f'(*({proto}){m.group(1)})(')
        i = m.end()
        n += 1
    out.append(body[i:])
    return ''.join(out), n


# Each entry: (diagnostic pattern the rule claims, rewrite function).
# A rule is offered a line only when GCC reported that exact kind of error on
# it, so the pattern here is half of the rule's safety argument.
def fix_aggregate_cast(line, diag, ctx):
    """`f((kd_agg16)*HWTMode)` — a cast to a by-value aggregate, which C forbids.

    Once gen_protos.py gives Ghidra the real signature for a function taking a
    struct BY VALUE (see its simple_type()), Ghidra casts the argument to that
    type at the call site. `(T)x` where T is a struct is "conversion to
    non-scalar type requested" — legal in Ghidra's output language, not in C.

    The operand is always an lvalue of the right size in practice, because that
    is what the original passed, so reinterpreting through a pointer is exactly
    what the ABI does: `*(kd_agg16 *)&x`. Where the operand is already a
    dereference, `*(T *)p` drops the redundant round trip.

    Only kd_agg types, deliberately. A cast to some OTHER struct type would mean
    Ghidra had recovered a real aggregate and reinterpreting it might not be
    what the original did — that case is left to fail loudly."""
    m = re.search(r'\((kd_agg\d+)\)\s*', line)
    if not m:
        return None
    typ = m.group(1)
    end = scan_unary_forward(line, m.end())
    if end is None or end <= m.end():
        return None
    operand = line[m.end():end].strip()
    if not operand:
        return None
    if operand.startswith('*'):
        inner = operand[1:].strip()
        repl = f'*({typ} *)({inner})'
    elif operand.startswith('&'):
        repl = f'*({typ} *)({operand})'
    else:
        repl = f'*({typ} *)&({operand})'
    return line[:m.start()] + repl + line[end:]


def fix_aggregate_as_integer(line, diag, ctx):
    """`(uint)faceId & 0xffff` where faceId is a STRUCT.

    The mirror of fix_aggregate_cast: there a scalar was cast TO an aggregate,
    here an aggregate is cast to a scalar. C forbids both; the machine code does
    neither, it just loads the bytes. IxBoxLineSegment — the raycast path — does

        pfVar12 = pfVar12 + ((uint)faceId & 0xffff) * 4;

    where faceId is a two-field struct and the original loads its four bytes and
    masks off the low half. `*(uint *)&faceId` says exactly that.

    Safe because it only ever runs on a line GCC has already REJECTED: a cast of
    an aggregate to an integer has no legal reading to be confused with, unlike
    (float)x->member, which is a valid conversion when the member is an int
    (dead end 6). One cast per pass, so the loop's verification decides.

    ENDIANNESS. This reinterprets bytes rather than naming a field, which is what
    the original does — `struct FaceId { MeU16 axis; MeU16 minside; }` and the
    mask picks out `axis` because it is first in memory. That is only true on a
    little-endian target. Every target this project builds for is little-endian
    (i386, wasm32, arm64/armv7 as configured), and a great deal of the recovered
    corpus already assumes it, but a big-endian port would need the field name
    here rather than the mask."""
    for m in ANY_CAST.finditer(line):
        typ = m.group(1)
        if typ.replace('struct ', '') not in SCALAR_KEYWORDS:
            continue
        end = scan_unary_forward(line, m.end())
        if end is None or end <= m.end():
            continue
        operand = line[m.end():end].strip()
        # An operand that is already a dereference or an address-of is not the
        # struct-valued identifier this rule is for.
        if not re.fullmatch(r'[A-Za-z_]\w*(?:\.\w+|->\w+|\[[^\]]*\])*', operand):
            continue
        return (line[:m.start()] + f'(*({typ} *)&({operand}))' + line[end:])
    return None



# `<dst> = (T *)&stack0xNNNN;` followed by a word-at-a-time copy out of a
# struct pointer. gcc 3.2 marshals a BY-VALUE AGGREGATE ARGUMENT this way, and
# Ghidra renders the ABI mechanics as C.
MARSHAL_LOOP = re.compile(
    r'^(?P<ind>[ \t]*)(?P<dst>\w+) = \((?P<ety>[A-Za-z_]\w*) \*\)'
    r'&(?P<spell>stack0x|in_stack_)(?P<off>[0-9a-f]{8});\n'
    # gcc sets up unrelated outgoing arguments between the pointer and the
    # loop — `ppMVar7 = keabodyArray_00;` — so tolerate a few PLAIN assignments
    # here. Only plain ones: anything with a call or a dereference could be
    # what fills the source, and stepping over that would be a guess.
    r'(?P<between>(?:(?P=ind)\w+ = [\w.>\[\]-]+;\n){0,4})'
    r'(?P=ind)for \((?P<iv>\w+) = (?P<cnt>0x[0-9a-f]+|\d+); (?P=iv) != 0; (?P=iv) = (?P=iv) \+ -1\) \{\n'
    r'(?P=ind)  \*(?P=dst) = (?P<src>\w+)->\w+;\n'
    r'(?P=ind)  (?P=src) = \([^)]*\)&(?P=src)->\w+;\n'
    r'(?P=ind)  (?P=dst) = (?P=dst) \+ 1;\n'
    r'(?P=ind)\}\n', re.M)

# `kVar3._kd[0x30] = (char)uVar18;` — Ghidra reassembling the aggregate it is
# about to pass, byte by byte, out of the slot the loop above just filled.
AGG_REASSEMBLE = re.compile(
    r'^[ \t]*(?P<agg>kVar\d+)\._kd(?:\[[^\]]+\]|\._\d+_\d+_) = [^;\n]*;\n'
    r'|^[ \t]*\(\*\(unsigned int \*\)\(\(char \*\)&\((?P<agg2>kVar\d+)\._kd\) \+ \d+\)\) = [^;\n]*;\n',
    re.M)


def _trace_pointer_base(region, upto, var, hops=3):
    """Name the aggregate the copy loop is reading FROM, or decline.

    Follows `var = X;` backwards. Two endings are accepted and nothing else:

      `var = &EXPR;`   the aggregate is EXPR — `pMVar14 = &w->keaParams`.
      `var = <call>;`  the aggregate is `*var`, but ONLY if that variable is
                       assigned exactly once in the whole function. A pointer
                       that is reassigned somewhere else does not name one
                       thing, and `MdtKeaConstraintsCreateFromChunk` returning
                       into `keaCon_00` does.

    Anything else — arithmetic, a second assignment, a chain longer than
    `hops` — declines. The rule this serves rewrites a call argument, so a
    wrong answer here is a wrong argument that still compiles, which is the
    failure mode HANDOVER.md §8 records twice.
    """
    for _ in range(hops):
        # Ghidra wraps a long right-hand side, so `keaCon_00 =
        # MdtKeaConstraintsCreateFromChunk\n    (...);` is ONE assignment on two
        # lines. Matching only single-line ones made this decline silently.
        assigns = list(re.finditer(r'^[ \t]*' + re.escape(var) + r' = ([^;]*?);',
                                   region, re.M | re.S))
        before = [m for m in assigns if m.start() < upto]
        if not before:
            return None
        rhs = before[-1].group(1).strip()
        if rhs.startswith('&'):
            return rhs[1:]
        if re.fullmatch(r'\w+', rhs):
            var, upto = rhs, before[-1].start()
            continue
        # A pointer that already holds the aggregate. Only if it holds it for
        # the whole function — one assignment, no reassignment anywhere.
        if len(assigns) == 1:
            return '*' + var
        return None
    return None


def collapse_outgoing_aggregate_copy(body, protos=None):
    """Pass the by-value aggregate SOURCE, instead of the frame it was copied to.

    gcc 3.2 passes an aggregate by value by copying it word-at-a-time into the
    outgoing argument area and then calling. Ghidra renders those ABI mechanics
    literally: a pointer to a frame slot it does not declare, a copy loop, and
    then the callee's argument rebuilt BYTE BY BYTE out of that slot into a
    `kd_aggNN` temporary. In C none of that is needed — `f(x)` marshals `x`
    itself — so the whole sequence collapses to passing the source.

        pMVar12 = pMVar14;                                  <- source
        pMVar17 = (MeReal *)&stack0xffffff6c;               <- outgoing slot
        for (i = 0x13; i != 0; i = i + -1) { ... }           <- 19 * 4 = 76 bytes
        kVar2._kd[4] = in_stack_ffffff70[0];  ... x76        <- rebuild
        MdtKeaAddConstraintForces(..., kVar2);

    becomes `MdtKeaAddConstraintForces(..., w->keaParams);`, because
    `pMVar14 = &w->keaParams`.

    WHY THIS IS ALLOWED AND THE OVERLAY WAS NOT. Dead end 20 tried to make the
    frame self-consistent — alias the slot so the writes and the reads agree —
    and it compiles and goes NaN, because the argument block is not the
    contiguous run of locals Ghidra split the frame into. This does not model
    the frame at all. It removes it, which is what the C compiler will rebuild
    from the source anyway.

    THE CHECK THAT MAKES IT SAFE is the size. The loop's own trip count times
    its element width must equal the size of the aggregate parameter at the
    call, taken from the prototype — 19 * 4 = 76 = `MdtKeaParameters`,
    23 * 4 = 92 = `MdtKeaConstraints`. A source that does not match the
    parameter it is being passed as is refused, not rounded.

    Validated on `MdtWorld`, which is why that object was the subject: 10
    errors to 0, `MdtWorldStep` ran 900 times in `scene_chain` (scene_census),
    trajectory BIT-IDENTICAL on all three scenes, and the gate is sensitive to
    one part per million in the aggregate this passes — perturbing
    `stepsize` by 1e-6 moves the chain 4.9e-04 m, `gamma` by 1% moves it
    8.6e-04 m. It stays quarantined afterwards, correctly: another function in
    the same object genuinely lost `num_bodies`.
    """
    region, n = body, 0
    loops = list(MARSHAL_LOOP.finditer(region))
    if not loops:
        return body, 0

    # PLAN first, against the untouched body, then apply once. Blanking a loop
    # shortens the text, so a second pass over match offsets taken before it
    # would read the wrong places.
    plan = []
    for loop in loops:
        elem = _FIXED_WIDTH.get(loop.group('ety'))
        if elem is None:
            continue
        nbytes = int(loop.group('cnt'), 0) * elem
        base = _trace_pointer_base(region, loop.start(), loop.group('src'))
        if not base:
            continue            # cannot NAME the source — decline, do not guess
        # Which aggregate is passed for this slot? Two spellings: Ghidra either
        # rebuilds a `kd_aggNN kVarM` byte by byte out of the slot, or — when it
        # declared the slot itself as a `kd_aggNN` — passes `in_stack_NNNN`
        # directly. Same repair either way.
        aggs = set(re.findall(r'\b(kVar\d+)\._kd', region))
        # `in_stack_<off>` names the SAME SLOT as `&stack0x<off>` — the value
        # and the address of one frame offset (§5c). So it is a candidate
        # whichever spelling the loop used to write there; gating on the
        # spelling missed the case where Ghidra declared the slot as a
        # `kd_agg92` and the loop still addressed it as `&stack0x...`.
        aggs.add('in_stack_' + loop.group('off'))
        for agg in sorted(aggs):
            if any(p[1] == agg for p in plan):
                continue
            # It must be ONLY the aggregate. In MdtWorldStepSafeTime Ghidra
            # uses `in_stack_ffffff5c` as the copy destination AND as an
            # ordinary MeReal — `in_stack_ffffff5c = *(MeReal *)(...)` — so the
            # slot is not one value and rewriting the name breaks the other use.
            if re.search(r'^[ \t]*' + re.escape(agg) + r'[ \t]*=[^=]', region, re.M):
                continue
            call = re.search(r'\b(\w+)\(([^;\n]*\b' + agg + r'\b[^;\n]*)\);', region)
            if not call:
                continue
            # THE CHECK THAT MAKES THIS SAFE: the bytes the loop copies must be
            # the bytes the callee's parameter is.
            if _proto_param_size(call.group(1), _arg_index(call.group(2), agg),
                                 protos) == nbytes:
                plan.append((loop.group('off'), agg, base))
                break
    if not plan:
        return body, 0

    offs = {p[0] for p in plan}
    aggnames = {p[1] for p in plan}
    # Keep the intervening assignments. They are UNRELATED outgoing arguments —
    # `ppMVar7 = keabodyArray_00; pvVar8 = pvVar5;` — that happen to sit between
    # the destination pointer and the loop, and deleting them with the loop left
    # the call passing uninitialised locals. That compiled, and segfaulted on
    # scene_chain.
    region = MARSHAL_LOOP.sub(
        lambda m: m.group('between') if m.group('off') in offs else m.group(0),
        region)
    region = AGG_REASSEMBLE.sub(
        lambda m: '' if (m.group('agg') or m.group('agg2')) in aggnames else m.group(0),
        region)
    for _off, agg, base in plan:
        # The declaration goes first: renaming every occurrence would turn
        # `kd_agg76 kVar2;` into `kd_agg76 (w->keaParams);`.
        #
        # ANY type, not a list of them. This used to name four —
        # kd_aggNN/undefined1/char/MeReal — and Ghidra declares the slot under
        # whatever type it worked out, which for keaMemory's allocateMemory is
        # the real `MdtKeaConstraints`. The declaration then survived the
        # removal, the rename rewrote it, and the object failed on
        # `MdtKeaConstraints (constraints);` — `constraints` redeclared as a
        # different kind of symbol, because the source it collapses to is the
        # function's own by-value PARAMETER of that name. Anchoring on the
        # NAME is what makes this safe; the type never was doing any work.
        region = re.sub(r'^[ \t]*(?:[A-Za-z_]\w*[ \t]+)+' + re.escape(agg)
                        + r'[ \t]*(?:\[\d+\])?[ \t]*;[ \t]*\n', '', region,
                        count=1, flags=re.M)
        region = re.sub(r'\b' + re.escape(agg) + r'\b', '(' + base + ')', region)
        n += 1
    return region, n


def _arg_index(arglist, name):
    args = [a.strip() for a in _split_args(arglist)]
    return args.index(name) if name in args else -1





def _split_args(s):
    args, depth, cur = [], 0, ''
    for ch in s:
        if ch in '([':
            depth += 1
        elif ch in ')]':
            depth -= 1
        if ch == ',' and depth == 0:
            args.append(cur); cur = ''
        else:
            cur += ch
    args.append(cur)
    return args


_PROTO_AGG_SIZES = {}


def _proto_param_size(fname, idx, protos=None):
    """Parameter `idx` of `fname`, in bytes, if it is a by-value aggregate.

    Sourced from kd_protos*.h, where gen_protos.py renders exactly these as
    `kd_aggNN` — the size is in the name, by construction.
    """
    if not _PROTO_AGG_SIZES:
        for path in (protos, os.environ.get('KARMA_PROTOS')):
            if not path or not os.path.exists(path):
                continue
            with open(path) as f:
                for line in f:
                    m = re.match(r'\S+ (\w+)\(([^)]*)\);', line.strip())
                    if m:
                        _PROTO_AGG_SIZES[m.group(1)] = [
                            int(x.group(1)) if x else 0
                            for x in (re.search(r'kd_agg(\d+)', a)
                                      for a in m.group(2).split(','))]
            break
    sizes = _PROTO_AGG_SIZES.get(fname)
    if not sizes or idx >= len(sizes):
        return None
    return sizes[idx] or None


# NOTE, found 2026-08-25: repair_loop picks ONE rule per diagnostic —
# `next((fn for pat, fn in REPAIR_RULES if pat.search(msg)), None)` — so a
# SECOND entry with the same pattern is dead code and looks exactly like a rule
# that declines. It also requires a file-wide rule to leave the LINE COUNT
# unchanged, and folds the result back into per-line edits that are then
# verified individually. A structural rewrite whose lines only help as a GROUP
# cannot live here at all; put it in the pre-pass chain in main() instead, which
# is where collapse_outgoing_aggregate_copy ended up after being written as a
# rule first and applied piecemeal.
REPAIR_RULES = [
    (re.compile(r'conversion to non-scalar type requested'),
     fix_aggregate_cast),
    (re.compile(r'aggregate value used where an integer was expected'),
     fix_aggregate_as_integer),
    (re.compile(r'pointer value used where a floating-point was expected'),
     fix_pointer_as_float),
    (re.compile(r'cannot convert to a pointer type'),
     fix_float_as_pointer),
    (re.compile(r'has no member named ' + Q.replace('([^', '(field_0x[^')),
     fix_field_offset),
    (re.compile(r'request for member ' + Q + r' in something not a structure'),
     fix_subfield_access),
    (re.compile(r'expected expression before ‘\.’ token|'
                r"expected expression before '\.' token"),
     fix_subfield_access),
    (re.compile(r'[‘\'"]_\w+[’\'"] undeclared'),
     fix_mislabelled_external),
    (re.compile(r'[‘\'"]stack0x[0-9a-f]+[’\'"] undeclared'),
     fix_stack_address_name),
    (re.compile(r'too many arguments to function'),
     fix_too_many_arguments),
    (re.compile(r'invalid use of void expression'),
     fix_void_assignment),
    (re.compile(r'called object is not a function or function pointer'),
     fix_call_through_void_ptr),
]


class RepairContext:
    """What the rules are allowed to consult. Everything here is READ from the
    object, the public headers or the DWARF-derived type database — nothing in
    it is inferred from the decompiled text."""

    def __init__(self, obj=None, fieldmap=None, include_dir=None, dump=None):
        self.obj = obj
        self.fieldmap = fieldmap or {}
        self.externals = relocation_targets(obj, per_function=True) if obj else {}
        self.extern_types = extern_var_types(include_dir)
        # Ghidra's stack-frame assignments, if the dump carried them. Read from
        # the decompiler's symbol map, not from the decompiled text.
        self.locals = read_ghidra_locals(dump)
        self.n_same = 0


def repair_loop(path, cc, cflags, ctx=None, max_rounds=200, verbose=True,
                max_restarts=3):
    """Compile, rewrite the lines GCC rejected, recompile, until it settles.

    Returns (n_edits, remaining_errors, log). The file is left with whichever
    version compiled best; if no rule helped it is left exactly as generated,
    so a failure to repair can never make an object worse than not trying.

    max_rounds is generous on purpose. Most rules fix one construct per pass,
    and GCC reports an undeclared name once per function, so an object with
    thirty of something needs thirty rounds. At 30 the cap was BINDING and
    silently so — McdBox stopped at 29 errors, and simply running the loop a
    second time took it to 8, which is the actual fixed point. A cap that stops
    early looks exactly like a rule that does not work. One round is one
    compile of one file; being wrong in this direction costs seconds.

    "Running it a second time gets further" is now BEHAVIOUR rather than an
    anecdote, and max_restarts is what makes it so. The reason it works is the
    rejection memory: `applied` holds every edit that was TRIED, accepted or
    not, so a line that got a wrong candidate can be offered a different one —
    and a rejection then outlives the reason for it. keaMemory is the worked
    case. `_pool_ptr` -> `pool_ptr` is exactly right and exchanges three
    `undeclared` diagnostics for three of a different kind, so on the round it
    was first offered the count did not fall and it was refused; by the time
    the surrounding repairs made it a strict improvement it was already in
    `applied` and could never be offered again. Clearing the memory and going
    round again takes the object from 5 errors to 5 with every one of them the
    same honest class, and 31 lines correctly renamed.

    Clearing is safe for the ACCEPTED half: an accepted edit is already in the
    file, so a rule re-offering it produces no change and it never reaches the
    filter. Only rejections are actually forgotten. A restart is taken only
    after real progress, so a rule that simply cannot help this object does not
    buy itself extra rounds."""
    ctx = ctx or RepairContext()
    original = open(path, errors='ignore').read()
    best_text, log = original, []
    diags = compile_diags(path, cc, cflags)
    best_n = len(diags)
    if not best_n:
        return 0, 0, log

    n_edits = 0
    since_restart, restarts = 0, 0
    applied = set()                 # (line, text) pairs already tried; no loops
    for _ in range(max_rounds):
        lines = best_text.split('\n')
        by_line = {}
        for lineno, _col, msg in diags:
            by_line.setdefault(lineno, []).append(msg)

        # `groups` is the same information as `edits`, kept per RULE
        # APPLICATION rather than per line. A multiline or file-wide rule
        # rewrites several lines from one diagnostic, so line -> entry is
        # not a function, and the retry path below needs the entry.
        edits, tried, groups = {}, [], []
        for lineno, msgs in sorted(by_line.items()):
            if not 1 <= lineno <= len(lines):
                continue
            text = lines[lineno - 1]
            for msg in msgs:
                rule = next((fn for pat, fn in REPAIR_RULES if pat.search(msg)), None)
                if rule is None:
                    continue
                # How many times GCC said this about this line. A rule can use
                # it to tell "every candidate here is wrong" from "one of them
                # is"; see fix_pointer_as_float.
                ctx.n_same = sum(1 for m2 in msgs if m2 == msg)
                # So a rule can offer a DIFFERENT candidate for a line whose
                # first candidate was tried and rejected. Without this the loop
                # can only ever make progress with its first guess per line.
                ctx.lineno = lineno
                ctx.applied = applied
                if getattr(rule, 'file_wide', False):
                    # A rule may resolve a name rather than a line, in which case
                    # every occurrence in the unit is the same defect. Fold the
                    # result back into per-line edits so the verification below
                    # is unchanged; a file-wide rule must not add or remove
                    # lines, and this asserts it.
                    whole = rule('\n'.join(lines), msg, ctx)
                    if whole is None:
                        continue
                    fixed = whole.split('\n')
                    if len(fixed) != len(lines):
                        continue
                    changed = {i + 1: b for i, (a, b) in enumerate(zip(lines, fixed))
                               if a != b and (i + 1, b) not in applied}
                    if not changed:
                        continue
                    edits.update(changed)
                    tried.append((lineno, rule.__name__, msg))
                    groups.append((tried[-1], changed))
                    break
                if getattr(rule, 'multiline', False):
                    # Ghidra wraps long expressions, so the construct GCC is
                    # complaining about is often not on the line GCC names —
                    # McdBoxCreate's `(McdFrameworkID)` sits a line above the
                    # SQRT it casts. Offer the rule the whole statement. It
                    # still may not add or remove lines.
                    lo, hi = _statement_bounds(lines, lineno - 1)
                    stmt = '\n'.join(lines[lo:hi])
                    new = rule(stmt, msg, ctx)
                    if new is None:
                        continue
                    fixed = new.split('\n')
                    if len(fixed) != hi - lo:
                        continue
                    changed = {lo + i + 1: b for i, (a, b)
                               in enumerate(zip(lines[lo:hi], fixed))
                               if a != b and (lo + i + 1, b) not in applied}
                    if not changed:
                        continue
                    edits.update(changed)
                    tried.append((lineno, rule.__name__, msg))
                    groups.append((tried[-1], changed))
                    break
                new = rule(text, msg, ctx)
                if new is not None and new != text and (lineno, new) not in applied:
                    edits[lineno] = new
                    tried.append((lineno, rule.__name__, msg))
                    groups.append((tried[-1], {lineno: new}))
                    break
        if not edits:
            # Every rule has now declined every line it was offered. That is
            # only the fixed point if no rule was ever REFUSED on evidence that
            # has since changed — see the docstring. Forget the rejections and
            # go round once more, but only if this pass actually got somewhere.
            if best_n and since_restart and restarts < max_restarts:
                applied, since_restart = set(), 0
                restarts += 1
                continue
            break

        # The verification, and the reason a wrong rule is harmless: an edit is
        # kept only if the diagnostic it CLAIMED got rarer and the total did not
        # grow. Counting total errors alone is not enough — GCC reports an
        # undeclared name once per function ("first use in this function"), so
        # repairing the first of several occurrences leaves the count unchanged
        # while making real progress.
        #
        # Occurrences, not presence: three `(float)ptr` casts in one expression
        # are three diagnostics with the same line and the same text, and a rule
        # that rewrites one of them has to be recognised as progress or the loop
        # rejects its own work and stalls on the first line it meets.
        #
        # Rewrites never add or remove lines, so line numbers stay comparable
        # across a round and (line, message) identifies a diagnostic exactly.
        def census(diaglist):
            c = {}
            for l, _col, m in diaglist:
                c[(l, m)] = c.get((l, m), 0) + 1
            return c

        before = census(diags)

        def verdict(new_diags, targets):
            live = census(new_diags)
            return (len(new_diags) <= best_n,
                    sum(1 for t in targets if live.get(t, 0) < before.get(t, 0)))

        targets = [(l, m) for l, _r, m in tried]
        candidate = list(lines)
        for lineno, new in edits.items():
            candidate[lineno - 1] = new
        candidate_text = '\n'.join(candidate)
        open(path, 'w').write(candidate_text)
        new_diags = compile_diags(path, cc, cflags)
        no_worse, resolved = verdict(new_diags, targets)

        if no_worse and resolved == len(targets):
            best_text, best_n, diags = candidate_text, len(new_diags), new_diags
            n_edits += len(edits)
            since_restart += len(edits)
            log += tried
            applied |= {(l, t) for l, t in edits.items()}
            if verbose:
                for lineno, rule, msg in tried:
                    print(f'  fixed {os.path.basename(path)}:{lineno} [{rule}] {msg[:60]}')
            if best_n == 0:
                break
        else:
            # Batch rejected. Retry one at a time: usually all but one were fine
            # and a single bad rewrite masked them.
            progress = False
            # One RULE APPLICATION at a time. Retrying a single line of a
            # multiline rewrite would apply half a rewritten statement, and
            # there is no `tried` entry for the lines a multiline rule touched
            # beyond the one GCC named — looking one up used to raise
            # StopIteration and take the whole object out of the build with a
            # traceback. keaMemory, in libMdtKea, failed exactly this way.
            for entry, changed in groups:
                one = best_text.split('\n')
                for lineno, new in changed.items():
                    one[lineno - 1] = new
                one_text = '\n'.join(one)
                open(path, 'w').write(one_text)
                d = compile_diags(path, cc, cflags)
                ok, res = verdict(d, [(entry[0], entry[2])])
                if ok and res:
                    best_text, best_n, diags = one_text, len(d), d
                    n_edits += 1
                    since_restart += 1
                    progress = True
                    applied |= {(l, t) for l, t in changed.items()}
                    log.append(entry)
                    if verbose:
                        print(f'  fixed {os.path.basename(path)}:{entry[0]} '
                              f'[{entry[1]}] {entry[2][:60]}')
                else:
                    # Remember the REJECTION too. `applied` used to hold only
                    # accepted edits, so a rule that guessed wrong regenerated
                    # the same guess every round and the line never got a second
                    # candidate — McdSphylGetBSphere stalled for exactly this
                    # reason, on a line with two casts where only the second was
                    # the pointer one.
                    applied |= {(l, t) for l, t in changed.items()}
            open(path, 'w').write(best_text)
            if not progress:
                break

    open(path, 'w').write(best_text)
    return n_edits, best_n, log


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('input', help='Ghidra .c dump')
    ap.add_argument('-o', '--output', required=True)
    ap.add_argument('--object', required=True,
                    help='original .o, used to decide exported vs static')
    ap.add_argument('--prelude', help='hand-written extern declarations to inline')
    ap.add_argument('--exports', help='exported DATA symbols, emitted after the forward '
                                      'declarations because they take function addresses')
    ap.add_argument('--vtables', help='C++ ABI data (vtable/typeinfo/type string) from '
                                      'tools/gen_vtables.py; emitted after the forward '
                                      'declarations because the slots take method addresses')
    ap.add_argument('--field-map', help='kd_types_fields.json — offsets to member '
                                        'names, for classes where Ghidra emitted '
                                        'field_0xNN instead')
    ap.add_argument('--protos', help='kd_protos*.h — used ONLY to size the '
                    'by-value aggregate parameters collapse_outgoing_aggregate_copy '
                    'checks against; unset disables that repair rather than '
                    'guessing a size')
    ap.add_argument('--metoolkit-include',
                    help='metoolkit include/ root; functions the headers define as '
                         'MeINLINE are dropped, since the header already supplies them')
    ap.add_argument('--drop', action='append', default=[],
                    help='function to omit entirely (repeatable)')
    ap.add_argument('--cc', default='gcc',
                    help='compiler to drive the repair loop with')
    ap.add_argument('--cflag', action='append', default=[],
                    help='flag for the repair loop, exactly as the caller will '
                         'later compile with (repeatable). Without any, the loop '
                         'does not run and the output is the raw generated C.')
    args = ap.parse_args()

    exported, internal, real_symbol_of, weak_symbols = object_symbols(args.object)
    hdr_inlines = header_inline_names(args.metoolkit_include)
    fieldmap = {}
    if args.field_map and os.path.exists(args.field_map):
        import json
        fieldmap = json.load(open(args.field_map))
    text = open(args.input, errors='ignore').read()

    # A Ghidra dump can contain the same name twice — typically a small import
    # thunk plus the real definition. Emitting both yields two incompatible
    # forward declarations and the translation unit stops compiling. Keep the
    # largest body, which is always the real one.
    by_name = {}
    for name, body in split_functions(text):
        if name not in by_name or len(body) > len(by_name[name]):
            by_name[name] = body

    # PASS 1: decide the renames. An exported function becomes kd_<name> with an
    # asm label carrying the real ELF symbol.
    renames = {}
    banner_of = {}
    for name, body in by_name.items():
        if name in args.drop or name in internal or name not in exported:
            continue
        b = resolve_anon_types(cxx_names_to_c(ghidra_type_quirks(strip_comments(body).strip('\n'))))
        sig = signature_of(b)
        if sig is None:
            continue
        d = declarator_name(sig)
        if d:
            renames[d] = 'kd_' + d
            # Remember the BANNER name too. For a C++ method Ghidra's banner is
            # the bare `PrincipalSubmatrix` while the declarator, after
            # cxx_names_to_c flattened `Class::method`, is
            # `keaLCPSolver__PrincipalSubmatrix`. Deriving the short name back
            # out of the declarator by splitting on '__' is wrong the moment a
            # method's own name starts with an underscore: CxSmallSort::_Update
            # flattens to `CxSmallSort___Update` and an rsplit yields `Update`.
            # The banner already has the right answer.
            if name != d:
                banner_of.setdefault(name, []).append(d)

    # Ghidra writes an INTRA-CLASS call site with the bare method name, while the
    # definition's banner is `Class::method` and cxx_names_to_c flattens that to
    # `Class__method`. The map above is keyed on the flattened declarator, so the
    # call site was left calling an undeclared `method` and the object failed to
    # compile. keaLCPSolver — the LCP solver — fails exactly this way on
    # PrincipalSubmatrix, PrincipalPivotTransformMakeW and
    # PrincipalPivotTransformMakeX.
    #
    # Add the short name as an alias, but only where it cannot mean anything else
    # in this translation unit. A bare name that is ambiguous between two classes,
    # or that the object also imports or defines in its own right, is left alone:
    # a wrong rename here would silently redirect a call, which is worse than not
    # compiling.
    imported = imported_names(args.object)
    n_short_alias = 0
    for short, owners in banner_of.items():
        if len(owners) != 1:
            continue                       # ambiguous between classes
        # NOT `short in exported`: object_symbols() already keys that set on the
        # SHORT name, so every C++ method is in it and the guard would reject
        # every alias. The real ambiguities are a sibling whose own declarator is
        # that bare name, a file-static of the same name, and an import.
        if short in renames or short in internal or short in imported:
            continue
        renames[short] = renames[owners[0]]
        n_short_alias += 1

    def apply_renames(text):
        """Rewrite every REFERENCE to a renamed function, not just its definition.

        Missing this is subtle: the definition gets its asm label and the symbol
        is right, but a sibling in the same file still calls the old name, which
        C then treats as an implicit declaration of an unrelated function.
        IxSphylPrimitives — 36,828 calls in real gameplay — failed exactly this
        way on AccumulateSphylContacts."""
        for old, new in renames.items():
            text = re.sub(r'(?<![\w.>])' + re.escape(old) + r'\b(?=\s*[(,);&\]])',
                          new, text)
        return text

    decls, defs, dropped = [], [], []
    n_alloca_fns = 0
    n_proto_calls = 0
    n_saved_elems = 0
    n_agg_copies = 0
    n_inline_dropped = 0
    n_vararg_fns = 0
    n_not_code = 0
    for name, body in by_name.items():
        if name in args.drop:
            dropped.append(name)
            continue
        if not_code(name, args.object, body):
            n_not_code += 1
            continue
        if name in hdr_inlines and name not in exported:
            n_inline_dropped += 1
            continue
        body = strip_comments(body).strip('\n')
        body = resolve_anon_types(cxx_names_to_c(ghidra_type_quirks(body)))
        body = resolve_field_names(body, fieldmap)
        body, nsaved = restore_saved_element(body)
        n_saved_elems += nsaved
        body, nva = resolve_varargs(body, signature_of(body) or '')
        n_vararg_fns += nva
        body, nalloca = materialise_alloca_frame(body, name)
        if nalloca:
            n_alloca_fns += 1
        body, nproto = prototype_indirect_calls(body)
        n_proto_calls += nproto
        body, nagg = collapse_outgoing_aggregate_copy(body, args.protos)
        n_agg_copies += nagg
        sig = signature_of(body)
        if sig is None:
            print(f'  ! skipping {name}: no body found', file=sys.stderr)
            continue
        # A name Ghidra shows unmangled that is NOT an exported symbol was
        # `static` in the original source. Keep it that way.
        is_static = name not in exported
        if is_static:
            decls.append(f'static {apply_renames(sig)};')
            defs.append(f'/* ---- {name} (static) ---- */\nstatic {apply_renames(body)}\n')
            continue
        newname, newsig = rename_exported(sig, name)
        if newname is None:                    # could not parse; emit as-is
            decls.append(f'{sig};')
            defs.append(f'/* ---- {name} (exported) ---- */\n{apply_renames(body)}\n')
            continue
        # Use the REAL ELF symbol, which for C++ is the mangled form.
        symbol = real_symbol_of.get(name, name)
        # Weak stays weak. The binding is part of the interface, not a detail
        # of how gcc happened to emit the original — see KD_WEAK in kd_compat.h.
        wk = ' KD_WEAK' if name in weak_symbols else ''
        decls.append(f'{apply_renames(newsig)} KD_MANGLED("{symbol}"){wk};')
        # `sig` is whitespace-normalised, so it will not match the raw body.
        # Rewrite the declarator in place instead: the name is the identifier
        # immediately before the first '(' in the text preceding the body.
        brace = body.find('\n{')
        head, rest = body[:brace], body[brace:]
        decl = declarator_name(sig) or name
        head = re.sub(r'\b' + re.escape(decl) + r'\b(?=\s*\()',
                      f'kd_{decl}', head, count=1)
        body = apply_renames(head + rest)
        defs.append(f'/* ---- {name} (exported as {newname}, asm label "{symbol}") ---- */\n'
                    f'{body}\n')

    body_text = '\n'.join(defs)
    body_text, n_extbase = fix_external_base_arithmetic(args.object, body_text)
    # The vtable a virtual call goes through is declared by the PRELUDE, so the
    # text this reads for the declaration is not the text it rewrites. Same for
    # the mangled call sites below, which resolve against this unit's own
    # forward declarations as well.
    _declared = ''
    for _extra in (args.prelude, args.vtables):
        if _extra and os.path.exists(_extra):
            _declared += open(_extra, errors='ignore').read()
    body_text, n_vptr = fix_vptr_store(args.object, body_text, _declared,
                                       read_ghidra_locals(args.input))
    body_text, n_mangled = resolve_mangled_call_names(
        body_text, _declared + '\n' + '\n'.join(decls))
    data_block, n_data = materialise_data_refs(args.object, body_text)
    # `kd_aggN` is the opaque stand-in gen_protos.py uses for an aggregate passed
    # BY VALUE. Once Ghidra has the signature it puts that type in the BODY too,
    # not only in the imported declaration — MdtWorld declares `kd_agg76 kVar2;`
    # as a local. gen_prelude can only see the prelude, so the typedef has to be
    # emitted from here, where the whole file is assembled.
    # Scan everything that ends up in the file, not just what is generated here.
    # The type reaches the output by two independent routes: Ghidra puts it in a
    # BODY once it has the signature (MdtWorld declares `kd_agg76 kVar2;` as a
    # local), and gen_prelude puts it in an extern DECLARATION for an imported
    # callee (keaCalcJinvMandRHS_vanilla). Scanning only the first missed the
    # second and took two objects out of the build.
    _agg_scan = body_text + '\n'.join(decls)
    for _extra in (args.prelude, args.exports, args.vtables):
        if _extra and os.path.exists(_extra):
            _agg_scan += open(_extra, errors='ignore').read()
    agg_sizes = sorted({int(n) for n in re.findall(r'\bkd_agg(\d+)\b', _agg_scan)})
    # Forward declarations mention these types too, so scan both.
    ftype_block, n_ftype = ghidra_functype_typedefs(body_text + '\n'.join(decls))
    ptr_block, n_ptr = resolve_ptr_labels(args.object, body_text)
    # After resolve_ptr_labels, so anything it could name from the object's own
    # symbols is already gone and only the genuinely unnamed blocks remain.
    rel_block, n_rel = materialise_relocated_data(args.object, body_text, renames)

    with open(args.output, 'w') as f:
        f.write('/* Generated by karma-decomp/tools/ghidra_clean.py — do not edit by hand.\n'
                f' * source object: {args.object}\n'
                ' */\n')
        f.write('#include "kd_compat.h"\n')
        f.write('#include "kd_karma.h"\n')
        f.write('#include "kd_types.h"\n')
        f.write('#include <stdbool.h>\n')
        f.write('#include <stdarg.h>\n\n')
        if agg_sizes:
            f.write('/* Stand-ins for aggregates passed BY VALUE; only the size\n'
                    ' * matters, and it is what fixes the ABI. See gen_protos.py. */\n')
            for n in agg_sizes:
                f.write('typedef struct { char _kd[%d]; } kd_agg%d;\n' % (n, n))
            f.write('\n')
        if ftype_block:
            f.write(ftype_block)
        if data_block:
            f.write(data_block)
        if args.prelude:
            f.write('/* ---- hand-written prelude ---- */\n')
            f.write(open(args.prelude).read())
            f.write('\n')
        f.write('/* ---- forward declarations ---- */\n')
        f.write('\n'.join(decls))
        f.write('\n\n')
        if args.vtables and os.path.exists(args.vtables):
            f.write('/* ---- C++ ABI data (needs the declarations above) ---- */\n')
            f.write(open(args.vtables).read())
            f.write('\n')
        if ptr_block:
            f.write(ptr_block)
        if rel_block:
            f.write(rel_block)
        if args.exports and os.path.exists(args.exports):
            f.write('/* ---- exported data symbols (need the declarations above) ---- */\n')
            f.write(open(args.exports).read())
            f.write('\n')
        f.write(body_text)

    if n_inline_dropped:
        print(f'  {n_inline_dropped} header-inline function(s) dropped')
    if n_not_code:
        print(f'  {n_not_code} function(s) dropped as NOT CODE (outside .text)')
    if n_vararg_fns:
        print(f'  {n_vararg_fns} variadic function(s) given a real va_list')
    if n_alloca_fns:
        print(f'  {n_alloca_fns} function(s) needed an alloca frame')
    if n_proto_calls:
        print(f'  {n_proto_calls} indirect call(s) given a prototype '
              f'(a float argument was being promoted to double)')
    if n_agg_copies:
        print(f'  {n_agg_copies} outgoing by-value aggregate copy(s) collapsed '
              f'to the source')
    if n_saved_elems:
        print(f'  {n_saved_elems} save-and-restore(s) of an array element reordered')
    if n_data:
        print(f'  {n_data} unnamed data reference(s) read from the object')
    if n_ftype:
        print(f'  {n_ftype} function type(s) declared from the name Ghidra gave them')
    if n_ptr:
        print(f'  {n_ptr} pointer slot(s) resolved from Ghidra addresses')
    if n_extbase:
        print(f'  {n_extbase} external base(s) re-resolved to byte arithmetic')
    if n_vptr:
        print(f'  {n_vptr} vptr store(s) resolved to a vtable address point')
    if n_mangled:
        print(f'  {n_mangled} mangled call site(s) resolved to a declared name')
    if n_rel:
        print(f'  {n_rel} relocated data block(s) rebuilt with their pointers')
    if args.cflag:
        ctx = RepairContext(args.object, fieldmap, args.metoolkit_include,
                            dump=args.input)
        n_edits, left, _log = repair_loop(args.output, args.cc, args.cflag, ctx)
        if n_edits:
            print(f'  {n_edits} line(s) repaired from compiler feedback'
                  + (f', {left} error(s) left' if left else ', clean'))
    print(f'{args.output}: {len(defs)} functions '
          f'({sum(1 for d in decls if d.startswith("static"))} static)'
          + (f', dropped {dropped}' if dropped else ''))


if __name__ == '__main__':
    main()
