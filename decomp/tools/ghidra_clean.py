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
    """(exported, internal, real_symbol_of) for the original object.

    `real_symbol_of` maps the name Ghidra displays to the actual ELF symbol.
    They differ for C++: Ghidra shows `MovingBoxBoxIntersect` while the object
    exports `_Z21MovingBoxBoxIntersectPKfPK11lsTransform...`. Emitting the
    displayed name in the asm label produces a symbol nothing links against —
    which is precisely why MovingBoxBoxIntersect and PolynomialRoots came out
    as undefined references."""
    out = subprocess.run(['nm', '--defined-only', obj],
                         capture_output=True, text=True).stdout
    exported, internal, real = set(), set(), {}
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
            real.setdefault(short, mangled)
    return exported, internal, real


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
    '>' at +2 and "</" at +0x12, which is what an XML writer emits."""
    out = subprocess.run(['readelf', '-SW', obj], capture_output=True, text=True).stdout
    addr, secs = GHIDRA_IMAGE_BASE, []
    for line in out.splitlines():
        m = re.match(r'\s*\[\s*\d+\]\s+(\S+)\s+(\S+)\s+[0-9a-f]+\s+[0-9a-f]+\s+'
                     r'([0-9a-f]+)\s+[0-9a-f]+\s+(\S*)\s+\d+\s+\d+\s+(\d+)', line)
        if not m:
            continue
        name, typ, size, flags, align = (m.group(1), m.group(2), int(m.group(3), 16),
                                         m.group(4), int(m.group(5)))
        if 'A' not in flags:
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
    silently changes the arithmetic."""
    n = len(s)
    while i < n and s[i] in ' \t':
        i += 1
    if i >= n:
        return None
    if s[i] in '-+!~':                 # not addressable; the caller must refuse
        return None
    while i < n and s[i] in '*& \t':   # dereference / address-of prefixes
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
        if k > 0 and s[k - 1] == '.' and not s[k - 2:k - 1].isdigit():
            j = k - 1
        elif k > 1 and s[k - 2:k] == '->':
            j = k - 2
        else:
            return j


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
    chosen = casts if len(casts) == getattr(ctx, 'n_same', 0) else casts[:1]
    out, last = [], 0
    for start, mid, end, typ, operand in chosen:
        out.append(line[last:start])
        out.append(f'(*({typ} *)&({operand}))')
        last = end
    out.append(line[last:])
    return ''.join(out)


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
    new = re.sub(r'([A-Za-z_]\w*)\s*->\s*' + field + r'\b', byte_ref, line)
    new = re.sub(r'([A-Za-z_]\w*)\s*\.\s*' + field + r'\b',
                 lambda mm: f'(*(char *)((char *)&({mm.group(1)}) + 0x{off:x}))', new)
    return new if new != line else None


SCALAR_KEYWORDS = {
    'int', 'char', 'short', 'long', 'signed', 'unsigned', 'float', 'double',
    'void', 'uint', 'ushort', 'uchar', 'byte', 'sbyte', 'bool', 'size_t',
    'longlong', 'ulonglong', 'longdouble', 'MeReal', 'MeI16', 'MeU32', 'MeI32',
    'undefined', 'undefined1', 'undefined2', 'undefined4', 'undefined8',
}
ANY_CAST = re.compile(r'\(\s*((?:struct\s+)?[A-Za-z_]\w*)\s*\)')


def fix_float_as_pointer(line, diag, ctx):
    """`p->member = (McdGeometryID)(dy * 0.5)` — the mirror of the case above.

    Ghidra typed a four-byte slot as a pointer and the original stored a float
    in it, so the cast has nothing to convert: the bytes are already what the
    original wrote. KD_FBITS takes them across without arithmetic, and the
    pointer cast that follows is then an ordinary integer-to-pointer conversion.

    Casts to a scalar keyword are skipped, because `(int)f` is a legal
    conversion and rewriting it would replace a rounded value with a bit
    pattern — the same mistake as dead end 6, pointing the other way."""
    for m in ANY_CAST.finditer(line):
        if m.group(1).replace('struct ', '') in SCALAR_KEYWORDS:
            continue
        end = scan_unary_forward(line, m.end())
        if end is None or end <= m.end():
            continue
        operand = line[m.end():end].strip()
        if not operand or operand.startswith('KD_FBITS'):
            continue
        return (line[:m.end()] + f'KD_FBITS({operand})' + line[end:])
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


# Each entry: (diagnostic pattern the rule claims, rewrite function).
# A rule is offered a line only when GCC reported that exact kind of error on
# it, so the pattern here is half of the rule's safety argument.
REPAIR_RULES = [
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
    (re.compile(r'too many arguments to function'),
     fix_too_many_arguments),
]


class RepairContext:
    """What the rules are allowed to consult. Everything here is READ from the
    object, the public headers or the DWARF-derived type database — nothing in
    it is inferred from the decompiled text."""

    def __init__(self, obj=None, fieldmap=None, include_dir=None):
        self.fieldmap = fieldmap or {}
        self.externals = relocation_targets(obj, per_function=True) if obj else {}
        self.extern_types = extern_var_types(include_dir)
        self.n_same = 0


def repair_loop(path, cc, cflags, ctx=None, max_rounds=90, verbose=True):
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
    compile of one file; being wrong in this direction costs seconds."""
    ctx = ctx or RepairContext()
    original = open(path, errors='ignore').read()
    best_text, log = original, []
    diags = compile_diags(path, cc, cflags)
    best_n = len(diags)
    if not best_n:
        return 0, 0, log

    n_edits = 0
    applied = set()                 # (line, text) pairs already tried; no loops
    for _ in range(max_rounds):
        lines = best_text.split('\n')
        by_line = {}
        for lineno, _col, msg in diags:
            by_line.setdefault(lineno, []).append(msg)

        edits, tried = {}, []
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
                    break
                new = rule(text, msg, ctx)
                if new is not None and new != text and (lineno, new) not in applied:
                    edits[lineno] = new
                    tried.append((lineno, rule.__name__, msg))
                    break
        if not edits:
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
            for lineno, new in sorted(edits.items()):
                one = best_text.split('\n')
                one[lineno - 1] = new
                one_text = '\n'.join(one)
                open(path, 'w').write(one_text)
                d = compile_diags(path, cc, cflags)
                entry = next(t for t in tried if t[0] == lineno)
                ok, res = verdict(d, [(entry[0], entry[2])])
                if ok and res:
                    best_text, best_n, diags = one_text, len(d), d
                    n_edits += 1
                    progress = True
                    applied.add((lineno, new))
                    log.append(entry)
                    if verbose:
                        print(f'  fixed {os.path.basename(path)}:{lineno} '
                              f'[{entry[1]}] {entry[2][:60]}')
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

    exported, internal, real_symbol_of = object_symbols(args.object)
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
    n_saved_elems = 0
    n_inline_dropped = 0
    n_vararg_fns = 0
    for name, body in by_name.items():
        if name in args.drop:
            dropped.append(name)
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
        decls.append(f'{apply_renames(newsig)} KD_MANGLED("{symbol}");')
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
    data_block, n_data = materialise_data_refs(args.object, body_text)
    # Forward declarations mention these types too, so scan both.
    ftype_block, n_ftype = ghidra_functype_typedefs(body_text + '\n'.join(decls))
    ptr_block, n_ptr = resolve_ptr_labels(args.object, body_text)

    with open(args.output, 'w') as f:
        f.write('/* Generated by karma-decomp/tools/ghidra_clean.py — do not edit by hand.\n'
                f' * source object: {args.object}\n'
                ' */\n')
        f.write('#include "kd_compat.h"\n')
        f.write('#include "kd_karma.h"\n')
        f.write('#include "kd_types.h"\n')
        f.write('#include <stdbool.h>\n')
        f.write('#include <stdarg.h>\n\n')
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
        if args.exports and os.path.exists(args.exports):
            f.write('/* ---- exported data symbols (need the declarations above) ---- */\n')
            f.write(open(args.exports).read())
            f.write('\n')
        f.write(body_text)

    if n_inline_dropped:
        print(f'  {n_inline_dropped} header-inline function(s) dropped')
    if n_vararg_fns:
        print(f'  {n_vararg_fns} variadic function(s) given a real va_list')
    if n_alloca_fns:
        print(f'  {n_alloca_fns} function(s) needed an alloca frame')
    if n_saved_elems:
        print(f'  {n_saved_elems} save-and-restore(s) of an array element reordered')
    if n_data:
        print(f'  {n_data} unnamed data reference(s) read from the object')
    if n_ftype:
        print(f'  {n_ftype} function type(s) declared from the name Ghidra gave them')
    if n_ptr:
        print(f'  {n_ptr} pointer slot(s) resolved from Ghidra addresses')
    if args.cflag:
        ctx = RepairContext(args.object, fieldmap, args.metoolkit_include)
        n_edits, left, _log = repair_loop(args.output, args.cc, args.cflag, ctx)
        if n_edits:
            print(f'  {n_edits} line(s) repaired from compiler feedback'
                  + (f', {left} error(s) left' if left else ', clean'))
    print(f'{args.output}: {len(defs)} functions '
          f'({sum(1 for d in decls if d.startswith("static"))} static)'
          + (f', dropped {dropped}' if dropped else ''))


if __name__ == '__main__':
    main()
