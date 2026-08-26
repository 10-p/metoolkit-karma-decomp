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
import collections
import os
import re
import struct
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import vtable_slots
import dwarf_structs

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
    # THE ADDEND IS NOT ALWAYS 0xf. `(x + 15) & ~15` is round-up-to-16, so a
    # request of `n*K + EXTRA` bytes is spelled `n*K + (EXTRA + 15) & ~15`.
    # MdtLODLastPartition asks for `(n+1)*4` and gcc writes
    #     4e: lea 0x13(,%eax,4),%ecx ; 56: and $0xfffffff0,%ecx ; 5c: sub %ecx,%esp
    # Requiring 0xf made that not an alloca to any rule here, and took MdtLOD's
    # whole frame with it — 243 guessed-frame sites, 237 of them in the one
    # function the engine imports. The extra is DERIVED as `C - 15`, not
    # guessed, and `C < 15` is not this idiom at all. One site corpus-wide.
    for m in re.finditer(r'(\w+)\s*=\s*-\(\s*(?:\(int\)\s*)?(.+?)\s*\*\s*(0x[0-9a-f]+|\d+)'
                         r'\s*\+\s*(0x[0-9a-f]+|\d+)U?\s*&\s*0xfffffff0\)\s*;', body):
        if int(m.group(4), 0) < 15:
            continue
        neg[m.group(1)] = (m.group(2).strip(), m.group(3),
                           str(int(m.group(4), 0) - 15))

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
                         r'\s*\+\s*(0x[0-9a-f]+|\d+)U?\s*&\s*0xfffffff0\s*;', body):
        if int(m.group(4), 0) < 15:
            continue
        sized[m.group(1)] = (m.group(2).strip(), m.group(3),
                             str(int(m.group(4), 0) - 15))
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
        # ---- A CHAIN OF ALLOCAS, and it must be resolved before anything else
        #
        # gcc lowers two allocas in one function to two `sub %reg,%esp`, so the
        # SECOND block's address is BOTH shifts applied to the same anchor.
        # MdtUpdatePartitions' prologue says it outright:
        #
        #    20: sub %esi,%esp    addedBodies      = &local_3c + iVar16
        #    39: sub %edx,%esp    addedConstraints = &local_3c + iVar8 + iVar16
        #    3e: sub $0xc,%esp    the outgoing argument area
        #
        # Only the ONE-shift form was recognised, so the second block was never
        # allocated at all: the generic collapse at the bottom of this function
        # dropped `+ iVar8` and left `addedConstraints` pointing at `local_3c`
        # ITSELF — a four-byte local, written `maxConstraints` times. That
        # compiles, and it is the same class of defect as the 64 KB buffer this
        # function's docstring already records.
        #
        # WHICH VAR SIZES THE BLOCK IS DERIVED, NOT GUESSED. The shift sets are
        # nested by construction — each is the running total of the `sub`s so
        # far — so sorting a base's sets by length makes each one introduce
        # exactly one new var, and that var is the `sub` that created this
        # block. A set that introduces none, or more than one, is not a chain
        # and is left alone.
        by_base = {}
        for m in re.finditer(r'=\s*(?:\([^()]*\)\s*)?\(\s*(?:\(int\)\s*)?&?(\w+)'
                             r'((?:\s*\+\s*[A-Za-z_]\w*)+)\s*\)\s*;', body):
            vs = re.findall(r'\w+', m.group(2))
            if all(v in neg for v in vs):
                by_base.setdefault(m.group(1), []).append((m.group(0), vs))
        for base, sites in by_base.items():
            seen = set()
            for whole, vs in sorted(sites, key=lambda s: len(s[1])):
                new = [v for v in vs if v not in seen]
                seen.update(vs)
                if len(vs) < 2 or len(new) != 1 or new[0] in allocated:
                    continue
                var = new[0]
                expr, mult, add = neg[var]
                allocated.add(var)
                alloca_base.setdefault(var, base)
                n += 1
                body = body.replace(
                    whole,
                    re.sub(r'\(\s*(?:\(int\)\s*)?&?\w+(?:\s*\+\s*[A-Za-z_]\w*)+\s*\)',
                           f'(kd_alloca_{var} = (char *)alloca((size_t)({expr}) * {mult} + {add}))',
                           whole, count=1))

        def sub_alloca(m):
            nonlocal n
            var = m.group(3)
            if var not in neg:
                return m.group(0)
            expr, mult, add = neg[var]
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
                    f'(char *)alloca((size_t)({expr}) * {mult} + {add}))')
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
                expr, mult, add = neg[var]
                n += 1
                allocated.add(var)
                alloca_base.setdefault(var, m.group(1))
                return (f'= (kd_alloca_{var} = '
                        f'(char *)alloca((size_t)({expr}) * {mult} + {add}));')
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

        # THE SAME AREA AT A NON-NEGATIVE OFFSET — the other half of the
        # outgoing-argument region, and the half that had no answer at all.
        #
        # Where the anchor Ghidra reached for sits BELOW the block instead of
        # above it, the very same slots come out as PLUS a small constant.
        # MdtUpdatePartitions writes its five argument words as
        # `(int)aiStack_50 + iVar8 + iVar16 + {0,4,8,0xc,0x10}` because
        # `aiStack_50` is 0x14 under `local_3c`, and the machine agrees: after
        # the two allocas comes `sub $0xc,%esp`, and the call site pushes one
        # more word and cleans all four with `add $0x10,%esp`.
        #
        # Left alone the generic collapses below take `+ iVar8` off and KEEP
        # `+ iVar16`, which is a large negative dynamic index into
        # `int aiStack_50[5]`.
        #
        # AN AREA IS TOLD FROM A POINTER DERIVATION BY HAVING MORE THAN ONE
        # SLOT IN IT — the group must carry at least one explicit constant.
        # That is the whole discriminator and it is what keeps this off
        # MeAssetDBXMLOutput_1_0, which is validated and in the build: every
        # one of its twenty-two multi-shift sites is a bare `base + shifts`
        # with no constant, i.e. a derivation of the block, and it stays with
        # the existing collapses. Measured, not assumed — see the scan in
        # proven.txt.
        #
        # And never where the base is one the ALLOCA was expressed against:
        # there `+ K` indexes the block itself, which the collapses below
        # already handle.
        # AND ONLY WHERE THERE IS MORE THAN ONE SHIFT, which is the boundary
        # between the case that already has an answer and the case that does
        # not. With ONE shift the generic collapse at the bottom removes it
        # outright and the slot lands on its own base local, which is what
        # IxConvexTriList, IxCylinderTriList, IxSphereTriList,
        # IxSphylPrimitives, MeAssetFactory, MeFAsset and MstUtils all rely on
        # — seven objects, validated and in the build, and check_frame_bounds
        # reports 0 for them. With TWO the collapse removes one shift and KEEPS
        # the other, so the slot becomes a large negative DYNAMIC index into a
        # fixed local. That is the defect, and it is the only case this takes.
        #
        # Written after measuring it the other way round: without this bound the
        # rule changed five of those seven objects and knocked MstUtils out of
        # the build entirely.
        AREA = re.compile(r'(\(int\)\s*)?&?(?<![.>])\b(\w+)'
                          r'((?:\s*\+\s*[A-Za-z_]\w*){2,})'
                          r'(?:\s*\+\s*(0x[0-9a-f]+|\d+))?\b')
        anchors = set(alloca_base.values())
        groups = {}
        for m in AREA.finditer(body):
            shifts = tuple(re.findall(r'\w+', m.group(3)))
            if any(v not in neg for v in shifts) or m.group(2) in anchors:
                continue
            groups.setdefault((m.group(2), shifts), set()).add(m.group(4))
        areas = {k for k, ks in groups.items() if any(x is not None for x in ks)}

        def sub_area(m):
            shifts = tuple(re.findall(r'\w+', m.group(3)))
            if (m.group(2), shifts) not in areas:
                return m.group(0)
            off = int(m.group(4), 0) if m.group(4) else 0
            slot = ('kd_frameslot_' + m.group(2) + '_' + '_'.join(shifts)
                    + '_p%x' % off)
            below.add(slot)
            return (f'(int)({slot})' if m.group(1) else f'({slot})')
        if areas:
            body = AREA.sub(sub_area, body)

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
                # INDEXED FIRST, and the order matters. `(int)aiStack_4c +
                # i * 4 + iVar5` is element i OF THE ALLOCATED BLOCK, because
                # `iVar5` is the shift that puts the block there. The two
                # collapses below only see the base and the shift ADJACENT, so
                # without this the `+ iVar5` is dropped by the generic rule at
                # the bottom and the expression becomes `(int)aiStack_4c +
                # i * 4` — an index into `int aiStack_4c[3]`, a twelve-byte
                # local, with i running to `count`. MeAssetFactory has eight,
                # and they are what its guessed-frame detector was reporting.
                #
                # check_frame_bounds cannot see this one: the index is a
                # VARIABLE, and it only reads constant offsets. The detector is
                # the only thing between it and the build.
                body = re.sub(r'\(int\)\s*&?' + re.escape(ab) + r'\s*\+\s*'
                              r'([A-Za-z_]\w*\s*\*\s*\d+)\s*\+\s*'
                              + re.escape(var) + r'\b',
                              lambda m: f'(int)(kd_alloca_{var}) + {m.group(1)}',
                              body)
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


def api_member_param_types(include_dir):
    """{(struct, member): first parameter's base type} for function-pointer
    members of the API structs the public headers declare.

    `MePoolAPI.destroy` takes `MePool *`; `MeMemoryAPI.destroy` takes `void *`.
    That is the only thing that tells the two apart when a relocation's addend
    lands on BOTH — `MePoolAPI+4` and `MeMemoryAPI+12` are the same address in
    Ghidra's invented memory map, and `_resolve_external` has to decline. It is
    read from the header, not inferred."""
    out = {}
    if not include_dir or not os.path.isdir(include_dir):
        return out
    member = re.compile(
        r'\(\s*(?:MEAPI\s*)?\*\s*(?P<name>\w+)\s*\)\s*\(\s*'
        r'(?:const\s+)?(?P<type>\w+)\s*(?P<star>\*?)')
    # `MeMemoryAPIStruct`'s members are TYPEDEF'D function pointers rather than
    # inline ones — `MeMemoryFuncPtrDestroy destroy;` — so the inline pattern
    # finds nothing for it and the whole rule declines on the one struct the
    # ambiguity is actually about. Resolve the typedefs first.
    fnptr = {}
    for root, _dirs, files in os.walk(include_dir):
        for f in sorted(files):
            if not f.endswith('.h'):
                continue
            try:
                txt = open(os.path.join(root, f), errors='ignore').read()
            except OSError:
                continue
            for mm in re.finditer(r'typedef\s+[\w \*]+?\(\s*(?:MEAPI\s*)?\*\s*'
                                  r'(?P<td>\w+)\s*\)\s*\(\s*(?:const\s+)?'
                                  r'(?P<type>\w+)', txt):
                fnptr.setdefault(mm.group('td'), mm.group('type'))
    for root, _dirs, files in os.walk(include_dir):
        for f in sorted(files):
            if not f.endswith('.h'):
                continue
            try:
                txt = open(os.path.join(root, f), errors='ignore').read()
            except OSError:
                continue
            for m in re.finditer(r'(?:typedef\s+)?struct\s+(\w+)\s*\{', txt):
                start = m.end()
                depth, i = 1, start
                while i < len(txt) and depth:
                    depth += (txt[i] == '{') - (txt[i] == '}')
                    i += 1
                body = txt[start:i]
                for mm in member.finditer(body):
                    out.setdefault((m.group(1), mm.group('name')),
                                   mm.group('type'))
                for mm in re.finditer(r'(?m)^\s*(\w+)\s+(\w+)\s*;', body):
                    if mm.group(1) in fnptr:
                        out.setdefault((m.group(1), mm.group(2)),
                                       fnptr[mm.group(1)])
    return out


# Both spellings. Ghidra prefixes the name with an underscore where the
# reference came through a relocation it could not attribute — `_DAT_000136c0`
# in MeProfile is 0x136c0, which is inside `.rodata.cst16`, a REAL section of
# that object, not the EXTERNAL block (which starts at 0x14000). Matching only
# the bare spelling left it to fall through to the mislabelled-external rule,
# which had nothing to say about it, and MeProfile failed on nothing else.
DATA_REF = re.compile(r'(?<![\w])(_?)DAT_([0-9a-f]{8})\b')

# The same thing with Ghidra's leading underscore, at an address BELOW the
# invented image base — so it names no section of this object.
ABS_DATA_REF = re.compile(r'(?<![\w])(_?DAT_([0-9a-f]{8}))\b')


def resolve_absolute_data_refs(obj, text):
    r"""`_DAT_00000050` — an absolute address, not a symbol and not a section.

    `ghidra_memory_map()` lays this object's sections out from 0x10000, so an
    address below that is in none of them. It is what it says: a load from a
    literal address, and the machine code has exactly that instruction.

    `MeFAssetPartEnableCollision` is the worked case, and it is worth reading
    because the recovery is faithful to something that looks like a decompiler
    artefact and is not. The shipped function:

        70f: mov 0x4(%eax),%ebx      ebx = p1->asset
        712: test %ebx,%ebx
        714: je  775                 asset == NULL: skip the range check
        716: mov 0x50(%ebx),%ecx     ecx = asset->maxParts
        719: cmp %ecx,0x4c(%ebx)
        71c: jg  761                 partCount > maxParts: return
        71e: ...                     BODY, and it needs ecx
        775: mov 0x50,%ecx           <-- ecx = *(int *)0x50
        77b: jmp 71e

    MathEngine wrote `asset == NULL || asset->partCount <= asset->maxParts`,
    which lets NULL through into a body that then uses `asset->maxParts`; gcc
    rematerialised that load on the branch where it knows the base is zero, and
    `mov 0x50,%ecx` is `((MeFAsset *)0)->maxParts` with the zero folded in. The
    original dereferences NULL on that path — at 0x775 and again at 0x74b — so
    reproducing the load reproduces the program, including the fault, which is
    the standard everything else here is held to.

    Two sites in the corpus, in two objects, both held out of the build on it.
    Anything at or above the image base is left to materialise_data_refs, which
    reads real section bytes; this only ever fires where there is no section to
    read."""
    secs, _ext = ghidra_memory_map(obj)
    base = min((lo for _n, _t, lo, _hi in secs), default=0x10000)
    seen = {}
    for full, addr in ABS_DATA_REF.findall(text):
        if int(addr, 16) < base:
            seen[full] = int(addr, 16)
    if not seen:
        return '', 0
    out = ['/* ---- absolute addresses, below any section of this object ---- */']
    for full, addr in sorted(seen.items(), key=lambda kv: kv[1]):
        out.append(f'#define {full} (*(int *)0x{addr:x})')
    return '\n'.join(out) + '\n\n', len(seen)


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
    addrs = sorted({int(a, 16) for _u, a in DATA_REF.findall(text)})
    spellings = {int(a, 16): set() for _u, a in DATA_REF.findall(text)}
    for u, a in DATA_REF.findall(text):
        spellings[int(a, 16)].add(u)
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
        for u in sorted(spellings.get(addr, {''})):
            out.append(f'#define {u}DAT_{addr:08x} ({c}[0x{off:x}])')
    return '\n'.join(out) + '\n\n', len(defines)


PTR_BLOCK_REF = re.compile(r'(?<![\w])(PTR_[A-Za-z0-9_]*?_([0-9a-f]{8}))\b')


def materialise_relocated_data(obj, text, renames=None, declared=''):
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

    A word whose relocation names a symbol the object does not define USED TO BE
    left as its literal addend with the symbol in a comment, on the grounds that
    emitting an undeclared name would trade a wrong value for a link error and
    neither is better than leaving the object visibly failing.

    THAT REASONING WAS WRONG ABOUT THE OUTCOME, and MeAssetDBXMLIO is what
    showed it. The object is not visibly failing: it compiles, passes every gate
    in this project, and then the engine calls through the null word during
    `KCreateAssetDB` and dies before a match starts —

        #0  0x00000000 in ?? ()
        #1  MeXMLElementProcess (…) at MeXMLParser.c:656
        #2  MeXMLInputProcess (…)
        #3  MeAssetDBXMLInputRead ()
        #4  KCreateAssetDB (…) KUtils.cpp:1374

    which is HANDOVER.md §7c's defect exactly, in a second object. A link error
    is loud and immediate; a null handler in a table is silent and fatal. And
    there is a third option better than either: the symbol is very often defined
    by a SIBLING object in the same corpus — `Handle_KaFile_0_1` is a global `T`
    in `MeAssetDBXMLInput_1_0.o` — so it can simply be declared and its address
    taken. The declaration's shape comes from the defining object's symbol TYPE,
    not from a guess. Only a symbol nothing in the corpus defines is still left
    as a commented literal."""
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
    # `.text` is not one: a relocation into it is resolved to the function that
    # lives at the offset, so copying its bytes would emit a large dead array
    # and invite exactly the mistake the loop below now refuses to make.
    targets = {}
    for name, (lo, hi) in wanted.items():
        for sym in relocs[name].values():
            if sym.startswith('.') and not sym.startswith('.text'):
                targets[sym] = True

    externs = set()
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
            elif sym.startswith('.text'):
                # A RELOCATION AGAINST .text IS A FUNCTION POINTER, not an
                # offset into a pool of bytes. Emitting it as the latter is what
                # `MeAssetDBXMLInput_1_0` did: handler[0].fn came out as
                # `&kd_relstr_text[0x1310]`, the engine loaded the table, called
                # through it, and died in a copy of .text that is neither
                # executable nor at that address. §7c's defect for the THIRD
                # time, and again in an object a detector had been holding back.
                fn = _function_at(obj, sym, raw)
                if fn:
                    words.append('(void *)&%s' % renames.get(fn, fn))
                else:
                    # Loud beats silent: an undeclared name fails the compile and
                    # the object goes back to review. A byte-pool address would
                    # link, pass every gate here, and segfault on a real `.ka`.
                    words.append('(void *)&kd_unresolved_text_0x%x' % raw)
            elif sym.startswith('.'):
                t = 'kd_relstr' + re.sub(r'\W', '_', sym)
                words.append('(void *)&%s[0x%x]' % (t, raw))
            elif sym in defined:
                words.append('(void *)&%s' % renames.get(sym, sym))
            elif _corpus_defines(obj, sym):
                # Defined by a SIBLING object, so it can be declared and its
                # address taken. See the note below on why leaving it null is
                # the worst of the three options.
                externs.add((sym, _corpus_defines(obj, sym)))
                words.append('(void *)&%s' % sym)
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
    # NO DECLARATION IS EMITTED, and that is deliberate. The symbol is already
    # declared in every case observed — by the prelude for `Handle_KaFile_0_1`,
    # by MeXMLParser.h for `MeXMLParseInt` — and a second, weaker `extern void
    # X();` beside either is a `conflicting types` error. Two objects left the
    # build that way on the first attempt, and the header case cannot be
    # detected by looking at the prelude and the body, which is all this sees.
    #
    # So the reference is emitted bare. If nothing declares the symbol the
    # compile fails with `undeclared`, which is loud, immediate, and strictly
    # better than the silent null this replaced — see the docstring.
    return '\n'.join(out) + '\n\n', len(defines)


def _corpus_defines(obj, sym):
    """'func' / 'data' if a sibling object in the same corpus defines `sym`.

    The declaration this drives only ever has its ADDRESS taken, so the kind
    decides the spelling and nothing else — but a function declared as an array
    reads as a lie in the generated source, and this file is meant to be read."""
    cache = _corpus_defines.cache
    d = os.path.dirname(os.path.abspath(obj))
    if d not in cache:
        table = {}
        for f in sorted(os.listdir(d)):
            if not f.endswith('.o') or os.path.join(d, f) == os.path.abspath(obj):
                continue
            out = subprocess.run(['nm', '--defined-only', '--format=posix',
                                  os.path.join(d, f)],
                                 capture_output=True, text=True).stdout
            for line in out.splitlines():
                p = line.split()
                if len(p) >= 2 and p[1] in 'TtWwVvDdBbRr':
                    table.setdefault(p[0], 'func' if p[1] in 'TtWw' else 'data')
        cache[d] = table
    return cache[d].get(sym)


_corpus_defines.cache = {}


def _function_at(obj, section, off):
    """The FUNC symbol defined at `section + off`, or None.

    A relocation against `.text` in a data section names a FUNCTION, and for a
    file-static one there is no symbol in the relocation to name it with — the
    relocation says `.text` and puts the function's offset in the addend. So the
    offset has to be looked up in the symbol table, which is where the static's
    name actually is."""
    cache = _function_at.cache
    if obj not in cache:
        table = {}
        sects = {}
        for line in subprocess.run(['readelf', '-SW', obj], capture_output=True,
                                   text=True).stdout.splitlines():
            m = re.match(r'\s*\[\s*(\d+)\]\s+(\S+)', line)
            if m and m.group(2) != 'Name':
                sects[m.group(1)] = m.group(2)
        for line in subprocess.run(['readelf', '-sW', obj], capture_output=True,
                                   text=True).stdout.splitlines():
            p = line.split()
            if len(p) < 8 or p[3] != 'FUNC':
                continue
            sec = sects.get(p[6])
            if sec:
                table[(sec, int(p[1], 16))] = p[7]
        cache[obj] = table
    return cache[obj].get((section, off))


_function_at.cache = {}


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


def _counted_trip_count(region, pos):
    """Iterations of the `do { } while (v < N)` loop containing `pos`, or None.

    Only the shape gcc leaves behind for a fully unrolled counted loop is read:
    an integer initialised to a constant before the `do`, incremented by a
    constant inside it, and compared against a constant at the `while`. Anything
    else returns None and the caller declines."""
    end = region.find('} while (', pos)
    if end < 0:
        return None
    m = re.match(r'\}\s*while\s*\((\w+)\s*<\s*(\d+)\)\s*;', region[end:])
    if not m:
        return None
    var, bound = m.group(1), int(m.group(2))
    steps = [int(s, 0) for s in
             re.findall(re.escape(var) + r'\s*=\s*' + re.escape(var)
                        + r'\s*\+\s*(\d+)\s*;', region[:end])]
    inits = [int(s, 0) for s in
             re.findall(r'(?m)^\s*' + re.escape(var) + r'\s*=\s*(\d+)\s*;', region[:end])]
    if len(steps) != 1 or not inits:
        return None
    step, init = steps[0], inits[-1]
    if step <= 0 or init >= bound:
        return None
    return -(-(bound - init) // step)


_PTR_ACCESS = r'\*\(([A-Za-z_][\w ]*?)\s*\*\)\(\s*%s\s*\+\s*(-?(?:0x[0-9a-f]+|\d+))\s*\)'


def _rebase_onto_covering_local(region, fn, off, ctx):
    r"""`local_124 = &stack0xfffffff4;` where NOTHING is declared at that offset.

    `frame_offsets.py --cover` separated three cases, and this is the third:
    the name is right, but Ghidra declared no local there at all. `MdtBcl` and
    `MeMath` are the whole corpus, and they need opposite answers — MeMath's
    accesses land in an array that is declared and NEVER WRITTEN, so repairing
    it would buy a compile at the price of reading uninitialised memory, while
    MdtBcl's land inside `MeReal ref2world[4][4]`, which the line above fills
    via MeMatrix4MultiplyMatrix.

    THE BASE POINTS 144 BYTES INTO A 64-BYTE ARRAY, which is why the obvious
    repair — point the pointer at the array and keep the displacements — is what
    `check_frame_bounds.py` exists to reject. So the pointer is REBASED instead:
    it is pointed at the covering local, and every displacement through it is
    shifted by the same constant. Every access keeps its address exactly, and
    the base becomes a pointer into the object it addresses:

        local_124 = &stack0xfffffff4;              frame -12
          *(MeReal *)(local_124 + -0x80)           -> frame -140
        becomes
        local_124 = (undefined1 *)ref2world;       frame -156
          *(MeReal *)(local_124 + 0x10)            -> frame -140

    In `MdtBclAddSpring6` the six accesses come out at ref2world[1][0..2] and
    [2][0..2], and the three statements above the loop write [0][0..2] — one
    3x3 rotation block copied into `clist->Jstore`, which is what the function
    is for. That agreement is the evidence the offsets are right; nothing about
    the shape would have shown it.

    THE TRIP COUNT IS READ, NOT ASSUMED. The pointer is incremented inside the
    loop, so the bounds check has to cover every iteration, and `_counted_trip_count`
    derives it from the `do { } while (v < N)` around it — 1 here, because
    `uVar20` starts at 1 and steps by 2. A loop this cannot read declines, and so
    does any use of the pointer other than an access or the increment."""
    want = int(off, 16) - (1 << 32) if int(off, 16) >= (1 << 31) else int(off, 16)
    tbl = (getattr(ctx, 'locals', None) or {}).get(fn) or {}
    if not tbl:
        return None
    m = re.search(r'(?m)^\s*(\w+)\s*=\s*&stack0x' + off + r'\s*;\s*$', region)
    if not m:
        return None
    ptr = m.group(1)
    acc = list(re.finditer(_PTR_ACCESS % re.escape(ptr), region))
    if not acc:
        return None
    steps = re.findall(re.escape(ptr) + r'\s*=\s*' + re.escape(ptr)
                       + r'\s*\+\s*(0x[0-9a-f]+|\d+)\s*;', region)
    if len(steps) > 1:
        return None
    step = int(steps[0], 0) if steps else 0
    trips = 1
    if step:
        trips = _counted_trip_count(region, m.end())
        if not trips:
            return None
    # every other mention of the pointer would move with the rebase
    mentions = len(re.findall(r'(?<![\w])' + re.escape(ptr) + r'\b', region))
    if mentions != 1 + len(acc) + 2 * len(steps) + 1:      # decl, accesses, increment, assign
        return None
    targets = []
    for a in acc:
        d = int(a.group(2), 0)
        width = _dwarf_type_size(getattr(ctx, 'obj', None), a.group(1).strip()) or 4
        for k in range(trips):
            targets.append((want + d + k * step, width))
    lo = min(t for t, _w in targets)
    cover = [(n, o, s) for n, (o, s, _t) in tbl.items()
             if o <= lo and all(o <= t and t + w <= o + s for t, w in targets)]
    if len(cover) != 1:
        return None
    name, base, _size = cover[0]
    # AND IT HAS TO BE WRITTEN. This is the guard that refuses MeMath, and it is
    # the documented reason to refuse it (§5c): `MeReal eR[3][3]` is DECLARED AND
    # NEVER WRITTEN, because Ghidra emitted `fcos`/`fsin` and discarded their
    # results, so rebasing onto it would buy a compile at the price of reading
    # uninitialised memory. MeMath already declined for an unrelated reason — its
    # loop is not the counted shape `_counted_trip_count` reads — and a guard
    # that holds by accident is not a guard. Measured: `eR` is mentioned ONCE in
    # its function, which is its declaration; `ref2world` is mentioned 69 times
    # in MdtBclAddSpring6 and assigned element by element.
    if not (re.search(r'(?m)^\s*' + re.escape(name) + r'\s*(?:\[[^\]]*\])*\s*=[^=]', region)
            or re.search(r'[(,]\s*' + re.escape(name) + r'\s*[,)]', region)):
        return None
    shift = want - base
    new = re.sub(r'(?m)^(\s*)' + re.escape(ptr) + r'\s*=\s*&stack0x' + off + r'\s*;\s*$',
                 lambda mm: f'{mm.group(1)}{ptr} = (undefined1 *){name};', region)
    def _shift(mm):
        return '*(%s *)(%s + %#x)' % (mm.group(1).strip(), ptr,
                                      int(mm.group(2), 0) + shift)
    new = re.sub(_PTR_ACCESS % re.escape(ptr), _shift, new)
    return new


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
            # Nothing declared at this offset — the third case of
            # frame_offsets.py --cover. Try rebasing onto the local the
            # ACCESSES land in; that declines too where they land nowhere.
            rebased = _rebase_onto_covering_local(region, _fn, off, ctx)
            if rebased is None:
                out.append(region)      # nothing declared here — decline
            else:
                out.append(rebased)
                changed = True
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


def fix_int_store_to_aggregate(line, diag, ctx):
    r"""`frameTime = (int)uVar1;` where `frameTime` is a 24-byte struct.

    The other half of the mislabelled-slot family, and the reason it looks like
    a different defect is that this one is spelled CORRECTLY. Ghidra stores four
    bytes to the first word of an exported aggregate, and the relocation says so
    — `R_386_32 frameTime` with addend 0 — but at addend ZERO the name it prints
    is the real one, so nothing flags it until the compiler does:

        57: mov %edx,0x0    R_386_32 frameTime  addend 0  ->  frameTime = ...
        5d: mov %ecx,0x4    R_386_32 frameTime  addend 4  ->  _select   = ...

    `_resolve_external` turns the second into `*(int *)((char *)&frameTime + 4)`
    and every one after it, which leaves the first as the only reference still
    naming the whole struct. This makes it the same access the others already
    are — six four-byte stores filling a six-word object.

    THE WIDTH IS NOT A GUESS: GCC names the source type in the diagnostic and
    the rule only fires for `int`. The left side has to be a BARE identifier; a
    member access or a subscript is a different shape and is left alone."""
    if not re.search(r'incompatible types when assigning to type '
                     r'[‘\'"][^’\'"]+[’\'"] from type [‘\'"]int[’\'"]', diag):
        return None
    a = re.match(r'^(\s*)([A-Za-z_]\w*)\s*=\s*([^;]+);\s*$', line)
    if not a:
        return None
    return f'{a.group(1)}*(int *)&{a.group(2)} = {a.group(3)};'


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
# `    inlined from ‘kd_MeOpenRaw’ at /tmp/…/MeSimpleFile_linux.c:76:11:`
INLINED_FROM = re.compile(
    r'^\s+inlined from .+? at (?P<file>.+?):(?P<line>\d+):(?P<col>\d+):?\s*$')

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
    inlined = None
    for line in r.stderr.splitlines():
        # `inlined from ‘kd_MeOpenRaw’ at <our file>:76:11:` — GCC reporting a
        # diagnostic inside a header that is ABOUT one of our lines. glibc's
        # fortified `open` is the case: the error is raised at fcntl2.h:46 and
        # the only thing wrong is at MeSimpleFile_linux.c:76. Without this the
        # diagnostic is dropped by the header rule below and no repair can ever
        # see it — the object stayed in the FAIL bucket for the project's whole
        # life on one such line.
        m = INLINED_FROM.match(line)
        if m:
            if os.path.abspath(m.group('file')) == here:
                inlined = (int(m.group('line')), int(m.group('col')))
            continue
        m = GCC_DIAG.match(line)
        if not m:
            continue
        if os.path.abspath(m.group('file')) == here:
            diags.append((int(m.group('line')), int(m.group('col')), m.group('msg')))
        elif inlined:
            diags.append((inlined[0], inlined[1], m.group('msg')))
        inlined = None
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
# `(` or `[` before the asm label — the declarator, in most shapes this pipeline
# emits: `extern void f(...) KD_MANGLED(…)`, `void __thiscall kd_f (…) …`,
# `extern void *v[] …`, `float kd_x[3] …`. A SCALAR has neither bracket
# (`struct MdtKeaDebugDataRequest *kd_gDebug KD_MANGLED("gDebug")`), and for
# those the declarator is simply the last identifier before the label — which is
# wrong for a function, whose last identifier is a parameter name, so the
# bracket rule has to be tried first and the fallback only used when it finds
# nothing at all.
_DECLARATOR = re.compile(r'\b([A-Za-z_]\w*)[ \t\n]*[(\[]')
_IDENT = re.compile(r'\b([A-Za-z_]\w*)\b')
_ASM_LABEL = re.compile(r'(?:KD_MANGLED|__asm__)\([ \t]*"([^"]+)"[ \t]*\)')


def declared_under(declared):
    """{ELF symbol: the C identifier this unit declares it as}.

    COMMENTS AND PREPROCESSOR LINES ARE STRIPPED FIRST, and that is not tidying.
    The statement boundary here is `;`, and a `#define` has none — so the chunk
    ending at `KD_MANGLED("frameTime")` ran back through the whole tail of the
    prelude, and `_DECLARATOR` found `MeProfileOutputResults__size64` from

        #define MeProfileOutputResults__size64 (*(const size_t *)...)

    which is followed by `(` where `kd_frameTime` is followed by a space. The
    LAST declarator in that chunk was therefore a macro from a different
    declaration, and `frameTime` mapped to it. `fix_exported_data_name` then
    renamed nothing, because the identifier it was offered does not appear on
    the line — which from the outside is indistinguishable from a rule that
    declined. MeProfile was the object holding it.
    """
    declared = re.sub(r'/\*.*?\*/', ' ', declared, flags=re.S)
    declared = re.sub(r'(?m)^[ \t]*#.*$', ' ', declared)
    out = {}
    for stmt in declared.split(';'):
        m = _ASM_LABEL.search(stmt)
        if not m:
            continue
        head = stmt[:m.start()]
        idents = [d for d in _DECLARATOR.findall(head)
                  if d not in ('KD_MANGLED', '__asm__')]
        if not idents:
            idents = _IDENT.findall(head)
        if idents and idents[-1] != m.group(1):
            out[m.group(1)] = idents[-1]
    return out


UNMODELLED_DECL = re.compile(
    r'(?m)^(?P<ind>[ \t]+)(?P<decl>[A-Za-z_][\w ]*\**[ \t]'
    r'(?P<name>(?:unaff_|in_)[A-Z]\w*))[ \t]*;[ \t]*$')


def initialise_unmodelled_locals(text):
    r"""Give `unaff_ESI` and `in_EDX` a defined initial value.

    These are Ghidra's names for a callee-saved register or an incoming slot it
    could not account for, and on some paths NOTHING assigns them before they
    are read. `MeOpenRaw` is the case:

        if (mode == kMeOpenModeWRONLY) { unaff_EBX = 0x241; unaff_ESI = 0x1a0; }
        else { … unaff_EBX = 0; }                 /* unaff_ESI untouched */
        iVar1 = open(filename, unaff_EBX, unaff_ESI);

    and the READ-ONLY path is the one the asset loader takes constantly.

    THE RECOVERY IS FAITHFUL AND THE C IS NOT. The shipped code does the same
    thing — `%esi` is genuinely live-in there, and `open` ignores the mode
    argument unless O_CREAT is set, so the value never mattered. But an
    uninitialised read is UNDEFINED BEHAVIOUR in C, and a compiler is entitled
    to conclude the path is unreachable and delete it. There was no information
    in the register to preserve, so zero costs nothing and removes the licence.

    Deliberately NOT applied to `extraout_*`: recover.py's `live_unmodelled`
    detector decides on reads-before-assignment in the SOURCE, so initialising
    those would switch the detector off for exactly the objects it is holding.
    `unaff_*`/`in_*` are named for a register the callee never wrote, which is a
    different claim from "a value a call returned"."""
    out, n = [], 0
    for m in UNMODELLED_DECL.finditer(text):
        pass
    text, n = UNMODELLED_DECL.subn(
        lambda m: '%s%s = 0;' % (m.group('ind'), m.group('decl')), text)
    return text, n


# Variadic functions whose format string this pipeline can read, and where it
# sits in the argument list. MeInfo/MeWarning/MeLog/MeFatalError are Karma's own
# and take (level, format, ...) — MeMessage.h.
PRINTF_FAMILY = {'printf': 0, 'fprintf': 1, 'sprintf': 1, 'snprintf': 2,
                 'vsprintf': 1, 'MeInfo': 1, 'MeWarning': 1, 'MeLog': 1,
                 'MeFatalError': 1}

# The same defect on the reading side. `readIntArrayFromFile` calls
# `sscanf(buf, "%d\n", array, uVar4)` and the machine code says outright that
# the fourth word is not an argument: gcc pushes four words before the PRECEDING
# MeStreamReadLine, cleans only three (`add $0xc,%esp`), and leaves the leftover
# to serve as sscanf's alignment slot — which is why sscanf's own cleanup is
# `add $0x10,%esp` for a three-argument call. Ghidra recovers the leftover as an
# argument and sources it from whatever register was live, which is where this
# object's `extraout_ECX`/`extraout_EDX` come from. Counted with _scanf_arity,
# never _printf_arity: `%*d` means the opposite thing here.
SCANF_FAMILY = {'sscanf': 1, 'fscanf': 1, 'scanf': 0}

# One conversion. `%%` consumes nothing; a `*` width or precision consumes an
# extra int each, which is why they are captured rather than skipped.
PRINTF_SPEC = re.compile(
    r'%(?P<flags>[-+ #0\']*)(?P<w>\*|\d+)?(?:\.(?P<p>\*|\d+))?'
    r'(?:hh|h|ll|l|L|q|z|j|t)?(?P<conv>[diouxXeEfFgGaAcspn%])')


def _printf_arity(fmt):
    """How many arguments the literal format consumes, or None if unsure."""
    body = fmt.strip()
    if not (body.startswith('"') and body.endswith('"')):
        return None
    n, pos = 0, 0
    body = body[1:-1]
    while True:
        i = body.find('%', pos)
        if i < 0:
            return n
        m = PRINTF_SPEC.match(body, i)
        if not m:
            return None                     # a conversion this cannot classify
        if m.group('conv') == 'n':
            return None                     # %n writes through its argument
        if m.group('conv') != '%':
            n += 1
            n += (m.group('w') == '*') + (m.group('p') == '*')
        pos = m.end()


# `pcVar7 = "line %d, char %d: expected %d strings, found %d\n";`
_FORMAT_ASSIGN = r'(?m)^[ \t]*%s[ \t]*=[ \t]*("(?:[^"\\]|\\.)*")[ \t]*;[ \t]*$'
# any assignment at all to the same name, literal or not
_ANY_ASSIGN = r'(?m)^[ \t]*%s[ \t]*=[^=]'


def _nearest_assignment(region, upto, var):
    """The last `var = ...;` statement starting before `upto`, or None."""
    last = None
    for m in re.finditer(r'(?m)^[ \t]*' + re.escape(var) + r'[ \t]*=([^=][^;]*);', region):
        if m.start() < upto:
            last = m
        else:
            break
    return last


def _reaching_formats(region, call_at, var):
    """Every definition of `var` that can reach the call, or None if unsure.

    Ghidra merges two call sites that share a tail into ONE call, and where the
    two sites push DIFFERENT format strings the format becomes a variable. That
    is what MeXMLParseStringArray does, and the shipped code says so outright —
    each path pushes its format as an IMMEDIATE and jumps to the shared tail:

        8df: push %ecx ; push %ecx          <- 2 padding words
        8e7: push %esi ; push %eax ; push %edi ; push %ebx     <- 4 arguments
        8f7: push $0x280   R_386_32 .rodata.str1.1  <- "...expected %d strings..."
        8fc: push <buffer> ; call sprintf            <- the shared tail
        915: sub $0xc,%esp                  <- 3 padding words, the other path
        91e: push %edx ; push %edx ; push %eax                 <- 3 arguments
        927: push $0x2c0   R_386_32 .rodata.str1.1  <- "...greater than max..."
        92c: jmp 8fc

    Eight pushed words either way, and at most FOUR of them are ever read. So
    the maximum over the reaching formats is the right arity — but only over the
    ones that REACH, and `pcVar7 = x` earlier in the same function does not: it
    is `readToNextTag`'s output buffer.

    The predecessors are readable because Ghidra writes its control flow with
    labels and gotos. The call's reaching definitions are the nearest assignment
    before it, plus the nearest assignment before every `goto` to any label
    between that assignment and the call. One level, no fixed point: a
    predecessor whose own nearest assignment is not a literal returns None and
    the site is declined."""
    first = _nearest_assignment(region, call_at, var)
    if first is None:
        return None
    span = region[first.start():call_at]
    labels = re.findall(r'(?m)^(\w+):\s*$', span)
    out = [first.group(1)]
    for lab in labels:
        for g in re.finditer(r'goto\s+' + re.escape(lab) + r'\s*;', region):
            a = _nearest_assignment(region, g.start(), var)
            if a is None:
                return None
            out.append(a.group(1))
    return out


def _scanf_arity(fmt):
    """How many arguments a literal *scanf* format consumes, or None if unsure.

    NOT `_printf_arity`. The two families share a syntax and disagree about the
    one piece of it that matters here: in `printf` a `*` width is read FROM an
    argument, and in `scanf` it SUPPRESSES the assignment and consumes none.
    Reusing the printf counter would over-count `%*d` by two, and over-counting
    only ever makes this rule keep an argument it could have dropped — so the
    error would have been silent, which is the reason to spell it out rather
    than share the function.

    A conversion this cannot classify — a `%[...]` scanset, most of all —
    returns None and the site is left alone. `%n` writes through its argument
    and is refused, exactly as in printf.

    Every scanf format in the corpus is `%d`, `%08x`, `%f` or a run of them; no
    suppression and no scanset appear at all. This is written for what the
    family MEANS, not for what the corpus happens to contain."""
    body = fmt.strip()
    if not (body.startswith('"') and body.endswith('"')):
        return None
    n, pos = 0, 0
    body = body[1:-1]
    while True:
        i = body.find('%', pos)
        if i < 0:
            return n
        m = PRINTF_SPEC.match(body, i)
        if not m:
            return None                     # a scanset or worse
        if m.group('conv') == 'n':
            return None                     # writes through its argument
        if m.group('conv') != '%' and m.group('w') != '*':
            n += 1                          # `*` here suppresses; it consumes nothing
        pos = m.end()


def _resolve_arity(region, arg, call_at=None, arity=None):
    """How many arguments the format consumes, following a variable if it is one.

    Ghidra hoists a format out of the call when one call site serves several
    strings:

        if (iVar1 == 1)      __format = " type=\"dynamics_only\"";
        else if (iVar1 == 2) __format = " type=\"geometry_only\"";
        else                 __format = " type=\"dynamics_and_geometry\"";
        iVar1 = sprintf(buffer + iVar3,__format,iVar3,iVar3);

    so `fix_printf_extra_args` saw a name where it wanted a literal and left the
    two padding words alone.

    THE ARITY TAKEN IS THE LARGEST OVER THE DEFINITIONS THAT REACH THE CALL, and
    both halves are load-bearing. MeXMLParser has one call reached by two
    definitions with DIFFERENT arities:

        pcVar7 = "line %d, char %d: expected %d strings, found %d\n";   4
        ...
        pcVar7 = "line %d, char %d: string data greater than max of %d\n";  3
        goto LAB_000108fc;
        ...
      LAB_000108fc:
        sprintf(file->error,pcVar7,iVar1,iVar2,uVar8,puVar9,pcVar4,pcVar6);

    Taking the nearer one textually gives 3, and dropping three arguments would
    drop `puVar9`, which the other path reads. The largest is the only safe
    answer: nothing beyond it is read on any path.

    And the same function assigns `pcVar7 = x` — `readToNextTag`'s output
    buffer, whose arity cannot be bounded at all. `_reaching_formats` is what
    says it does not reach this call; where every definition in the function
    must be considered, one non-literal declines the site outright."""
    a = arg.strip()
    arity = arity or _printf_arity
    if a.startswith('"'):
        return arity(a)
    if not re.match(r'^[A-Za-z_]\w*$', a):
        return None
    if call_at is not None:
        reaching = _reaching_formats(region, call_at, a)
        if reaching is not None:
            arities = [arity(r.strip()) for r in reaching]
            if not any(x is None for x in arities):
                return max(arities)
    lits = [m.group(1) for m in re.finditer(_FORMAT_ASSIGN % re.escape(a), region)]
    if not lits:
        return None
    if len(re.findall(_ANY_ASSIGN % re.escape(a), region)) != len(lits):
        return None                     # a definition this cannot bound
    arities = [arity(l) for l in lits]
    return None if any(x is None for x in arities) else max(arities)


def fix_printf_extra_args(text):
    r"""`sprintf(buf, "%s", name, extraout_EDX)` — gcc's padding, not an argument.

    The same defect as fix_variadic_extra_args and the same evidence, without a
    diagnostic to hang it on: gcc pushes alignment padding before a call, Ghidra
    recovers every pushed word as an argument, and it sources the extra ones
    from whatever register happened to be live — which is where a large part of
    the `extraout_ECX`/`extraout_EDX` family comes from.

    C permits any number of arguments to a variadic function, so nothing
    diagnoses it. But the FORMAT STRING says how many are read, and it is right
    there as a literal, so the count is derived rather than guessed. Dropping
    the rest cannot change behaviour: a variadic argument the callee never
    fetches is not observable.

    FOUR GUARDS, because this rewrites calls in objects that are already
    validated:

      * the format must resolve to a literal and every conversion in it must be
        one this can classify. `%n` writes through its argument and is refused
        outright; `%*d` consumes an extra int for the width and is counted.
        Where the format is a VARIABLE, `_resolve_arity` requires every
        assignment to it in the function to be a literal and takes the LARGEST
        arity among them — nothing beyond that is read on any path.
      * only arguments with NO side effect are dropped — a bare identifier, a
        constant or a simple member access. Anything containing a call is left,
        and with it the whole site.
      * arguments are dropped from the END only, never reordered.
      * at most THREE are dropped. gcc aligns the pushed block to 16 bytes, so
        the filler is 0-3 words and nothing here can legitimately ask for more.
        This is what bounds the damage if a resolved-through-a-variable format
        is ever the wrong one.

    156 sites across 7 objects, 6 of them held out of the build. The one that is
    in it, McdMessage, has a single site.

    THE SCANF SIDE was added afterwards and is the same defect read backwards —
    see SCANF_FAMILY for the machine code that proves the extra word is gcc's
    leftover alignment slot and not an argument. It counts with `_scanf_arity`,
    which differs from the printf counter on `%*d`; sharing one counter would
    have been wrong in the safe direction, and therefore silent."""
    out, n = [], 0
    for _fn, region in _split_definitions(text):
        for family, arity in ((PRINTF_FAMILY, _printf_arity),
                              (SCANF_FAMILY, _scanf_arity)):
            for name, fi in family.items():
                pos = 0
                while True:
                    m = re.compile(r'(?<![\w.>])' + re.escape(name)
                                   + r'[ \t\n]*\(').search(region, pos)
                    if not m:
                        break
                    op = m.end() - 1
                    end = _match_bracket(region, op)
                    if end is None:
                        pos = m.end()
                        continue
                    args = _split_args(region[op + 1:end - 1])
                    pos = m.end()
                    if len(args) <= fi:
                        continue
                    want = _resolve_arity(region, args[fi], m.start(), arity)
                    if want is None or len(args) <= fi + 1 + want:
                        continue
                    keep = args[:fi + 1 + want]
                    drop = args[fi + 1 + want:]
                    if len(drop) > 3:
                        continue            # more than gcc's alignment can explain
                    if any('(' in d or '=' in d or '++' in d or '--' in d
                           for d in drop):
                        continue            # a side effect; leave the site alone
                    new_call = region[:op + 1] + ','.join(a.strip() for a in keep) \
                        + region[end - 1:]
                    n += len(drop)
                    pos = op + 1
                    region = new_call
        out.append(region)
    return ''.join(out), n


# Ghidra's frame-slot names all encode their own offset, so two slots spelled
# differently can still be compared numerically. `kd_argslot_*` is this
# pipeline's own name, emitted by materialise_shifted_frame.
_SLOT_NAME = [
    (re.compile(r'^(?:kd_argslot|in_stack)_([0-9a-f]{8})$'),
     lambda h: int(h, 16) - (1 << 32) if int(h, 16) >= (1 << 31) else int(h, 16)),
    (re.compile(r'^[A-Za-z_]*Stack[A-Z]?_([0-9a-f]+)$'), lambda h: -int(h, 16)),
    (re.compile(r'^local_([0-9a-f]+)$'), lambda h: -int(h, 16)),
]

_DEREF = re.compile(r'\*\([^()]*\*\)\(')

# A bare unmodelled register name, and nothing else, on the right of a store.
_BARE_UNMODELLED = re.compile(
    r'^(?:extraout_[A-Z]+[0-9_]*|unaff_[A-Z]+[0-9_]*)$')


def _slot_base_offset(name):
    for pat, fn in _SLOT_NAME:
        m = pat.match(name)
        if m:
            return fn(m.group(1))
    return None


def _split_plus(s):
    """Split on `+` at bracket depth zero."""
    out, depth, cur = [], 0, ''
    for ch in s:
        if ch in _OPEN:
            depth += 1
        elif ch in _CLOSE:
            depth -= 1
        if ch == '+' and depth == 0:
            out.append(cur)
            cur = ''
        else:
            cur += ch
    out.append(cur)
    return [p.strip() for p in out if p.strip()]


def _decode_slot(text, start):
    """`*(T *)( base + terms + const )` -> (terms, frame offset, end) or None."""
    m = _DEREF.match(text, start)
    if not m:
        return None
    end = _match_bracket(text, m.end() - 1)
    if end is None:
        return None
    base, off, terms = None, 0, []
    for piece in _split_plus(text[m.end():end - 1]):
        if re.match(r'^-?(?:0x[0-9a-f]+|\d+)$', piece):
            off += int(piece, 16) if 'x' in piece.lower() else int(piece)
            continue
        nm = re.match(r'^(?:\(int\))?\s*&?\s*\(?\s*\*?\s*([A-Za-z_]\w*)\s*\)?$', piece)
        if nm and base is None and _slot_base_offset(nm.group(1)) is not None:
            base = _slot_base_offset(nm.group(1))
            continue
        terms.append(re.sub(r'\s+', '', piece))
    if base is None:
        return None
    return tuple(sorted(terms)), base + off, end


def _slots_in(text):
    """Every decodable frame slot in `text`, left to right."""
    i, out = 0, []
    while True:
        j = text.find('*(', i)
        if j < 0:
            return out
        k = _decode_slot(text, j)
        if k:
            out.append((j, k[0], k[1], k[2]))
            i = k[2]
        else:
            i = j + 2


def _header_index(include_dir):
    """{name: ('fn', return type)} and {name: ('fnptr', arity)} from the headers.

    Two things are wanted and both are one regex each: what a public function
    RETURNS, so the base of a struct access can be typed, and how many
    parameters a function-pointer TYPEDEF takes, so an indirect call through it
    can be given its arguments back."""
    cache = _header_index.cache
    if include_dir in cache:
        return cache[include_dir]
    idx = {}
    if include_dir and os.path.isdir(include_dir):
        for root, _d, files in os.walk(include_dir):
            for f in files:
                if not f.endswith('.h'):
                    continue
                try:
                    txt = open(os.path.join(root, f), errors='ignore').read()
                except OSError:
                    continue
                txt = re.sub(r'/\*.*?\*/', ' ', txt, flags=re.S)
                for m in re.finditer(
                        r'typedef\s+[\w\s\*]{0,80}?\(\s*(?:MEAPI\s*)?\*\s*(\w+)\s*\)'
                        r'\s*\(([^;]*?)\)\s*;', txt):
                    args = [a for a in _split_arguments(m.group(2)) if a.strip()]
                    if len(args) == 1 and args[0].strip() in ('void', ''):
                        args = []
                    idx[m.group(1)] = ('fnptr', len(args))
                for m in re.finditer(
                        r'(?m)^\s*([A-Za-z_][\w]*\s*\**)\s+MEAPI\s+(\w+)\s*\(', txt):
                    idx.setdefault(m.group(2), ('fn', m.group(1).strip()))
    cache[include_dir] = idx
    return idx


_header_index.cache = {}


def _struct_member_type(include_dir, tag, member):
    """The declared TYPE of `tag.member`, read from the public headers."""
    idx = _struct_member_type.cache.setdefault(include_dir, {})
    if tag not in idx:
        body = None
        for root, _d, files in os.walk(include_dir or '.'):
            for f in files:
                if not f.endswith('.h'):
                    continue
                try:
                    txt = open(os.path.join(root, f), errors='ignore').read()
                except OSError:
                    continue
                m = re.search(r'struct\s+' + re.escape(tag) + r'\s*\{(.*?)\n\}', txt, re.S)
                if m:
                    body = m.group(1)
                    break
            if body:
                break
        fields = {}
        if body:
            body = re.sub(r'/\*.*?\*/', ' ', body, flags=re.S)
            for line in body.split(';'):
                mm = re.match(r'\s*([A-Za-z_][\w]*(?:\s*\*)*)\s+(\w+)\s*$', line)
                if mm:
                    fields[mm.group(2)] = mm.group(1).strip()
        idx[tag] = fields
    return idx[tag].get(member)


_struct_member_type.cache = {}


def restore_indirect_call_args(text, fieldmap, include_dir):
    r"""`iVar7 = (*pcVar1)();` — Ghidra had no signature, so it dropped every argument.

    This is the defect that crashed `IxSphereTriList` (§8): the callee reads
    whatever is on the stack, and the recovery compiles, links and passes the
    scripted scenes because they never reach the call. Two sites are left in the
    corpus, `McdInteractions` and `MstUtils`, and between them they are eight
    symbols of the drop-in gap.

    EVERYTHING NEEDED IS ALREADY WRITTEN DOWN — the point is only that it is in
    four different places:

        pvVar4 = McdFrameworkGetInteractions(frame,type1,iVar7);
        ...
        *(McdIntersectResult **)((int)pMVar6 + -0xc)  = result;
        *(McdModelPair **)((int)pMVar6 + -0x10)       = p;
        *(undefined4 *)((int)pMVar6 + -0x14)          = 0x10232;   <- return address
        pcVar1 = *(code **)((int)pvVar4 + 8);
        iVar7 = (*pcVar1)();

      1. `McdFrame.h` says McdFrameworkGetInteractions returns `McdInteractions*`
      2. `kd_types_fields.json` says `_McdInteractions` +8 is `intersectFn`
      3. `McdCTypes.h` says that member is a `McdIntersectFn`
      4. `McdCTypes.h` says `McdIntersectFn` is
         `int (MEAPI *)(McdModelPair *, McdIntersectResult *)` — TWO arguments

    and the arguments themselves are in the slots the call sits on: the return
    address marks the base, so argument i is at base + 4*(i+1), and the stores
    that filled them are right there with their casts. The two words above them
    are gcc's alignment padding, which `drop_padding_arg_stores` then removes for
    free — it could not before, because with no arguments in the call there was
    no top argument slot to be above.

    IT DECLINES ON ANY BREAK IN THE CHAIN, and the chain is four links long, so
    that is most of the code. The arity has to come out of a typedef; the base
    has to be typed by a real prototype; every argument slot has to have been
    stored with a cast this can copy. A guess here is the thing the detector
    exists to prevent."""
    if not fieldmap:
        return text, 0
    hdr = _header_index(include_dir)
    out, n = [], 0
    for _fn, region in _split_definitions(text):
        for call in list(re.finditer(r'\(\*(\w+)\)\(\s*\)', region)):
            ptr = call.group(1)
            mtype = None
            # SHAPE 1 — the pointer comes straight out of a function whose
            # RETURN type is a function-pointer typedef. MstUtils:
            #   pcVar12 = MstBridgeGetPerPairCB(...);   ->  MstPerPairCBPtr
            direct = re.search(re.escape(ptr) + r'\s*=\s*(\w+)\s*\(', region)
            if direct and hdr.get(direct.group(1), ('', ''))[0] == 'fn':
                cand = hdr[direct.group(1)][1].strip()
                if hdr.get(cand, ('', ''))[0] == 'fnptr':
                    mtype = cand
            # SHAPE 2 — the pointer is a STRUCT MEMBER, so the member's type has
            # to be found before the typedef can be.
            if mtype is None:
                src = re.search(re.escape(ptr) + r'\s*=\s*\*\(code \*\*\)\('
                                r'(?:\(int\))?\s*(\w+)\s*\+\s*(0x[0-9a-f]+|\d+)\s*\)\s*;',
                                region)
                if not src:
                    continue
                base, off = src.group(1), int(src.group(2), 0)
                made = re.search(re.escape(base) + r'\s*=\s*(\w+)\s*\(', region)
                if not made or hdr.get(made.group(1), ('', ''))[0] != 'fn':
                    continue
                tag = re.sub(r'\s*\*+$', '', hdr[made.group(1)][1]).strip()
                member = (fieldmap.get(tag, {}).get(str(off))
                          or fieldmap.get('_' + tag, {}).get(str(off)))
                if not member:
                    continue
                mtype = (_struct_member_type(include_dir, '_' + tag, member)
                         or _struct_member_type(include_dir, tag, member))
            if not mtype or hdr.get(mtype, ('', ''))[0] != 'fnptr':
                continue
            arity = hdr[mtype][1]
            # THE OUTGOING AREA IS SPELLED TWO WAYS and both occur. McdInteractions
            # addresses it through an ORDINARY variable — `(int)pMVar6 + -0x10` —
            # where only offsets relative to that one name are meaningful.
            # MstUtils spreads the same area across THREE frame-named locals,
            # `aiStack_9cb0 + 0x14`, `&fStack_9c98` and `aMStack_9c94`, which are
            # consecutive slots only once each name is decoded to its frame
            # offset. So both decoders are tried, absolute first.
            head = region[:call.start()]
            tail = head.split('\n')[-24:]
            best = None
            for decoder in ('abs', 'rel'):
                slots, retaddr = {}, None
                for line in reversed(tail):
                    s = line.strip()
                    if decoder == 'abs':
                        sl = _slots_in(s)
                        if not sl or sl[0][0] != 0:
                            continue
                        _, terms, o, end = sl[0]
                        key = (terms, o)
                    else:
                        m = re.match(r'\*\([^()]*\*\)\(\s*(?:\(int\))?\s*(\w+)\s*'
                                     r'\+\s*(-?(?:0x[0-9a-f]+|\d+))\s*\)', s)
                        if not m:
                            continue
                        key, end = (m.group(1), int(m.group(2), 0)), m.end()
                    if '=' not in s[end:end + 3]:
                        continue
                    if re.match(r'\*\(undefined4 \*\)\(.*\)\s*=\s*0x1[0-9a-f]{4}\s*;$', s):
                        if retaddr is None:
                            retaddr = key   # the LAST store before the call, so
                        continue            # keep going: the arguments are above
                    slots.setdefault(key, s[:end])
                if retaddr is None:
                    continue
                args = []
                for i in range(arity):
                    lhs = slots.get((retaddr[0], retaddr[1] + 4 * (i + 1)))
                    if lhs is None:
                        break
                    args.append(lhs)
                if len(args) == arity:
                    best = args
                    break
            if best is None:
                continue
            args = best
            new = '(*(%s)%s)(%s)' % (mtype, ptr, ',\n                 '.join(args))
            region = region[:call.start()] + new + region[call.end():]
            n += 1
        out.append(region)
    return ''.join(out), n


def materialise_alloca_relative_slots(body, fname):
    r"""`(int)(kd_alloca_iVar5) + -4` — an argument slot BELOW an alloca'd block.

    `materialise_shifted_frame` gives the outgoing-argument area real storage
    wherever Ghidra named its slots `in_stack_*`/`stack0x*`. Where a function
    also calls `alloca`, gcc puts the allocated block immediately ABOVE that
    area and addresses the area from it, so Ghidra spells one or more of the
    slots as a NEGATIVE offset from the allocation instead, and
    `materialise_shifted_frame` never sees them.

    `MeAssetFactory` has one such slot, and it is a real argument — not padding,
    which is what this looked like until the read was found:

        *(MeMatrix4Ptr *)((int)(kd_alloca_iVar5) + -4) = tm;
        *(MdtWorldID *)(&(*kd_argslot_ffffffac)) = world;
        *(McdGeometryID *)(&(*kd_argslot_ffffffa8)) = pMVar11;
        *(MeAssetFactory **)(&(*kd_argslot_ffffffa4)) = pMVar21;
        *(undefined4 *)((int)auStackY_70 + 0x10) = 0x104e4;
        pMVar16 = (*p_Var3)(*(MeFAssetPart **)(&(*kd_argslot_ffffffa4)),
                            *(McdGeometryID *)(&(*kd_argslot_ffffffa8)),
                            *(MdtWorldID *)(&(*kd_argslot_ffffffac)),
                            *(MeMatrix4Ptr *)((int)(kd_alloca_iVar5) + -4));

    The return address is at -0x60, so the four arguments are -0x5c, -0x58,
    -0x54 and -0x50 — and `kd_alloca_iVar5 - 4` IS -0x50, read back by the call
    as its fourth argument. Three of the four already have storage as
    `kd_argslot_*`; this gives the fourth the same, which is the only reason the
    other three are not out of bounds either.

    It is a WRITE-THEN-READ of one address, so rewriting base and offset
    together preserves the dataflow exactly, and the result is in bounds where
    the original was four bytes under an `alloca(n)` block. `check_frame_bounds`
    is what says so, before and after.

    ONLY NEGATIVE OFFSETS, and never the pointer's own value: the block itself
    is real memory that gets passed to callees — in `MeAssetFactory` it is
    `MeFAssetGetPartsSortedByName`'s output array — so touching a positive
    offset or a bare use would corrupt an actual allocation."""
    ptrs = set(re.findall(r'\(\s*(kd_alloca_\w+)\s*=\s*\(char \*\)alloca\s*\(', body))
    ptrs |= set(re.findall(r'^\s*(kd_alloca_\w+)\s*=\s*\(char \*\)alloca\s*\(', body, re.M))
    if not ptrs:
        return body, 0
    n = 0
    for ptr in sorted(ptrs):
        ref = re.compile(r'\(int\)\s*\(\s*' + re.escape(ptr) + r'\s*\)\s*\+\s*'
                         r'(-(?:0x[0-9a-f]+|\d+))\b')
        offs = sorted({int(m.group(1), 0) for m in ref.finditer(body)})
        if not offs:
            continue
        size = -offs[0]
        if size % 4 or size > 64:
            continue              # not an argument area this can account for
        buf = 'kd_argslot_' + ptr
        body = ref.sub(lambda m: f'(int)({buf}) + {int(m.group(1), 0) + size:#x}', body)
        brace = body.find('\n{')
        body = (body[:brace + 2] + f'\n  undefined1 {buf} [{size}];\n'
                + body[brace + 2:])
        n += len(offs)
    return body, n


def materialise_pointer_arg_area(text):
    r"""`pMVar6 = (McdContact *)&type1;` then writes NINE WORDS BELOW `type1`.

    The outgoing-argument area again, in the one spelling
    `materialise_shifted_frame` does not answer: Ghidra could not name the base,
    so it anchored the area on whatever local happened to sit just above it and
    addressed everything at a NEGATIVE offset from that. `McdIntersect` picks
    two different anchors on two paths —

        pMVar6 = (McdContact *)&type1;              /* an int, 4 bytes */
        if (...) { pMVar6 = aMStackY_501c; }        /* a 20,440-byte array */
        *(void **)((int)pMVar6 + -4)  = pvVar5;
        ...
        *(undefined4 *)((int)pMVar6 + -0x24) = 0x1027d;

    — which is the giveaway: the anchor is arbitrary, because the VALUE of the
    pointer is never used. Every one of its nineteen uses is `(int)pMVar6 + K`
    with K negative, so what the pointer names is a 0x24-byte region below
    itself and nothing else. That region gets real storage, and the pointer is
    set to its top so every offset lands inside it.

    THE GUARD IS THAT THE POINTER IS ONLY EVER A BASE, and it has two halves.
    Every use must be `(int)ptr + K` with K negative, or an assignment TO it —
    if any use reads it positively, or passes it anywhere, the anchor is
    load-bearing after all and this declines. AND EVERY ASSIGNMENT TO IT MUST BE
    THE ADDRESS OF A LOCAL. Without that second half the rule fires on `MeHeap`,
    where `pvVar2 = (MeMemoryAPI.create)(capacity * 4);` is a real allocation
    and the "repair" replaces it with a four-byte stack buffer — in an object
    that is validated and in the build. The shape being matched is Ghidra
    anchoring on a local it did not choose; a pointer that came from a call is
    somebody's real object, and inventing storage for it is dead end 3.

    `check_frame_bounds.pointer_aliases` reads the `+ 0x24` in the repaired
    assignment, so the result is CHECKED rather than laundered: before, the
    object reports nine references outside a four-byte int; after, zero, and it
    is the same checker saying so."""
    out, n = [], 0
    for _fn, region in _split_definitions(text):
        for decl in re.finditer(r'(?m)^([ \t]+)([A-Za-z_]\w*)\s*\*\s*(\w+)\s*;\s*$', region):
            ptr = decl.group(3)
            uses = list(re.finditer(r'(?<![\w])' + re.escape(ptr) + r'\b', region))
            if len(uses) < 3:
                continue
            offs, ok = [], True
            for u in uses:
                if u.start() == decl.start(3):
                    continue                                  # its declaration
                line = region[region.rfind('\n', 0, u.start()) + 1:
                              region.find('\n', u.end())]
                before, after = region[:u.start()], region[u.end():]
                if before.endswith('(int)') and \
                        re.match(r'\s*\+\s*-(?:0x[0-9a-f]+|\d+)\s*\)', after):
                    offs.append(int(re.match(r'\s*\+\s*(-(?:0x[0-9a-f]+|\d+))',
                                             after).group(1), 0))
                    continue
                if re.match(r'^\s*' + re.escape(ptr) + r'\s*=[^=]', line):
                    # assigned TO — and it has to be the ADDRESS OF A LOCAL,
                    # not an allocation. See the docstring on MeHeap.
                    if not re.match(r'^\s*' + re.escape(ptr) + r'\s*=\s*'
                                    r'(?:\([\w ]*\*+\)\s*)?&?\w+\s*;\s*$', line):
                        ok = False
                        break
                    continue
                ok = False
                break
            if not ok or not offs:
                continue
            size = ((-min(offs)) + 3) // 4 * 4
            buf = 'kd_argarea_' + ptr
            region = re.sub(r'(?m)^(' + re.escape(decl.group(1)) + re.escape(decl.group(2))
                            + r'\s*\*\s*' + re.escape(ptr) + r'\s*;)$',
                            r'\1\n' + decl.group(1) + f'undefined1 {buf} [{size}];',
                            region, count=1)
            region = re.sub(r'(?m)^(\s*)' + re.escape(ptr) + r'\s*=\s*[^;]*;\s*$',
                            lambda mm: (f'{mm.group(1)}{ptr} = ({decl.group(2)} *)'
                                        f'({buf} + {size:#x});'), region)
            n += 1
        out.append(region)
    return ''.join(out), n


def drop_padding_arg_stores(text):
    r"""`*(slot above the last argument) = extraout_EDX;` — gcc's padding push.

    The third rendering of the defect fix_variadic_extra_args and
    fix_printf_extra_args already fix in the other two. gcc 3.2 keeps `%esp`
    16-byte aligned across a call, so it pushes 0-3 filler words before the
    arguments; the filler is whatever register was live, and Ghidra recovers
    the push honestly as a store of a value it could not account for:

        *(undefined4 *)((int)auStack_78 + iVar2 + iVar6 + 8) = extraout_EDX;
        *(undefined4 *)((int)auStack_78 + iVar2 + iVar6 + 4) = extraout_EDX;
        *(MdtBaseConstraint **)((int)auStack_78 + iVar2 + iVar6) = ...;
        *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
        *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x1029f;
        MeHeapPush(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14),
                   *(void **)((int)auStack_78 + iVar2 + iVar6));

    `MdtLOD.o` at 0x291: `push %edx; lea; push %edx; push %ebx; push %eax;
    call MeHeapPush; add $0x10,%esp`. Four words pushed, two arguments read.

    THE ARITY IS DERIVED, NOT GUESSED, AND IT COMES OUT OF THE TEXT. Ghidra
    knows the callee's prototype, so it emits the right number of arguments and
    READS THEM BACK out of the same slots; the pushes it had left over become
    stray stores. So the top argument slot is the highest slot the call itself
    reads, and everything above it was already known to be beyond the arity —
    by Ghidra, at the moment it wrote the call.

    THREE GUARDS:

      * the block size must come out a multiple of four words. gcc aligns to 16
        bytes, so `arguments + padding` is 4, 8, 12 ... — checked against the
        real thing at every site examined (IxConvexTriList's 6 arguments plus 2
        filler is `add $0x20,%esp`; MdtLOD's 2 plus 2 is `add $0x10,%esp`). A
        site that does not fit that shape is left alone, and the object stays in
        review, which is the right answer for something this cannot explain.
      * only a BARE unmodelled name is dropped. Where Ghidra did model the value
        the code is valid C that reproduces the machine faithfully and there is
        no defect to fix — IxConvexTriList stores a live `pfVar21` into its two
        filler words and is validated, in the build, and left untouched.
      * the store must be pending for THIS call: anything written to the area
        before the previous call on it has already been consumed.

    12 sites across 5 objects directly, and 9 more through a variable.

    THE VARIABLE CASE IS DECIDED ON THE WHOLE VARIABLE, not on a definition.
    Ghidra merges unrelated values into one C local, so `uVar14 = extraout_ECX_00`
    says nothing on its own about what `uVar14` carries elsewhere. What can be
    read straight off the text is whether the local carries ANYTHING else:

        uVar14 = extraout_ECX_00;                       <- assigned, unmodelled
        uVar14 = extraout_ECX_01;                       <- assigned, unmodelled
        *(undefined4 *)((int)aMStack_9c94 + 4) = uVar14; <- read, padding slot
        *(undefined4 *)((int)aMStack_9c94)     = uVar14; <- read, padding slot

    Every assignment unmodelled and every read a padding slot means no
    definition of it can reach anything observable, whatever the control flow
    does — so the local is deleted whole. `MstUtils`'s `p_Var8` fails that test
    (it is also assigned from four real calls and read three more times) and is
    left, which is the right answer: proving THAT one inert needs to know which
    definition reaches which read, and this does not."""
    out, n = [], 0
    for _fn, region in _split_definitions(text):
        lines = region.split('\n')
        pending, drop = collections.defaultdict(list), set()
        via = collections.defaultdict(set)      # local -> padding-store line(s)
        for i, line in enumerate(lines):
            s = line.strip()
            sl = _slots_in(s)
            if not sl or sl[0][0] != 0:
                continue
            _, terms, off, end = sl[0]
            if re.match(r'\*\(undefined4 \*\)\(.*\)\s*=\s*0x1[0-9a-f]{4}\s*;$', s):
                call = '\n'.join(lines[i + 1:i + 14])
                stop = call.find(';')
                if stop >= 0:
                    args = [a[2] for a in _slots_in(call[:stop + 1]) if a[1] == terms]
                    if args:
                        top = max(args)
                        nargs = (top - off) // 4
                        # gcc aligns the pushed block to four words
                        block = ((nargs + 3) // 4) * 4
                        for (ln, rhs) in pending.get(terms, []):
                            if not (top < ln[1] <= off + 4 * block):
                                continue
                            if _BARE_UNMODELLED.match(rhs):
                                drop.add(ln[0])
                            elif re.match(r'^[A-Za-z_]\w*$', rhs):
                                via[rhs].add(ln[0])
                pending[terms] = []
                continue
            eq = s.find('=', end)
            if eq > 0 and s[eq + 1:eq + 2] != '=':
                pending[terms].append(((i, off), s[eq + 1:].strip().rstrip(';').strip()))
        # A local whose every assignment is unmodelled and whose every read is a
        # padding store cannot reach anything observable, on any path — so the
        # local goes too, assignments and all.
        #
        # Where the local carries real values as well, only the STORES go.
        # MeFAsset's `iVar18` is assigned from `count` and from
        # `MeFAssetPartIsCollisionEnabled` as well as from `extraout_EAX_03`,
        # and is tested with `if (iVar18 == 0)`, so the whole-variable test
        # rightly declines — but deleting a store into a word beyond the
        # callee's arity is sound whatever the word holds, and it is what takes
        # the unmodelled value out of the emitted code.
        #
        # The taint check is not part of the soundness argument; it is what
        # keeps the blast radius on objects that have the defect. IxConvexTriList
        # puts a live `pfVar21` in its two filler words, is validated, in the
        # build, and is left byte-identical.
        for var, stores in via.items():
            word = re.compile(r'(?<![\w])' + re.escape(var) + r'\b')
            assigns, reads, whole = [], [], True
            for i, line in enumerate(lines):
                if not word.search(line):
                    continue
                s = line.strip()
                if re.match(r'^(?:const\s+|struct\s+|unsigned\s+|signed\s+)*'
                            r'[A-Za-z_]\w*\s*[\*\s]*' + re.escape(var)
                            + r'\s*(?:\[[^\]]*\])?;$', s):
                    continue                                    # its declaration
                m = re.match(r'^' + re.escape(var) + r'\s*=([^=].*);$', s)
                if m and not word.search(m.group(1)):
                    if _BARE_UNMODELLED.match(m.group(1).strip()):
                        assigns.append(i)
                    else:
                        whole = False
                    continue
                if i in stores and len(word.findall(line)) == 1:
                    reads.append(i)
                    continue
                whole = False
            if not assigns:
                continue                    # nothing unmodelled ever reaches it
            if whole and set(reads) == stores:
                drop |= set(assigns) | stores
            else:
                drop |= stores
        if drop:
            n += len(drop)
            region = '\n'.join(l for j, l in enumerate(lines) if j not in drop)
        out.append(region)
    return ''.join(out), n


def _compat_variadic_arity():
    """{macro name: named-parameter count} from kd_compat.h's variadic macros.

    That header maps gcc 3.2's glibc spellings back to portable ones:

        #define __strtod_internal(s, e, ...)     strtod((s), (e))

    The `...` is there because Ghidra over-counts these call sites — the same
    alignment padding as everywhere else — and the macro discards the surplus at
    compile time. Reading the NAMED parameter count back out of the header lets
    the surplus be dropped in the SOURCE instead, which is what the detector
    reads. Deriving it here rather than tabulating it keeps the two in step."""
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    path = os.path.join(root, 'include', 'kd_compat.h')
    out = {}
    try:
        text = open(path, errors='ignore').read()
    except OSError:
        return out
    for m in re.finditer(r'^#define\s+(\w+)\(([^)]*\.\.\.)\)(.*)$', text, re.M):
        # A macro whose BODY uses __VA_ARGS__ keeps its extra arguments, so they
        # are not padding and must not be dropped. `__strtod_internal(s, e, ...)`
        # expands to `strtod((s), (e))` and discards them; `__divdi3(al, ah, ...)`
        # expands through KD_LL_2(__VA_ARGS__, 0, 0) and the "extras" ARE the
        # divisor. Truncating those to the named count left `__divdi3(a_lo,
        # a_hi)` and took MeProfile from one error to a macro expansion failure.
        if '__VA_ARGS__' in m.group(3):
            continue
        named = [p.strip() for p in m.group(2).split(',') if p.strip() != '...']
        out[m.group(1)] = len(named)
    return out


def fix_compat_macro_extra_args(text):
    r"""`__strtod_internal(x,&c,0,extraout_EDX)` — one more padding push.

    `kd_compat.h` already declares these variadic and throws the surplus away,
    so this changes nothing a compiler can see. What it changes is what the
    RECOVERED SOURCE says: with the argument still written down, `extraout_EDX`
    is read, `recover.py`'s live_unmodelled fires, and the object is held out of
    the build for a value that provably reaches nothing. MeXMLParser is twelve
    symbols of the drop-in gap held on exactly that.

    The arity comes from the macro's own named-parameter list, and the same two
    guards as the printf rule apply: side-effect-free arguments only, dropped
    from the end, and never more than gcc's three words of alignment filler."""
    arity = _compat_variadic_arity()
    if not arity:
        return text, 0
    n = 0
    for name, keep_n in arity.items():
        pos = 0
        while True:
            m = re.compile(r'(?<![\w.>])' + re.escape(name) + r'[ \t\n]*\(').search(text, pos)
            if not m:
                break
            op = m.end() - 1
            end = _match_bracket(text, op)
            if end is None:
                pos = m.end()
                break
            args = _split_args(text[op + 1:end - 1])
            pos = m.end()
            if len(args) <= keep_n or len(args) - keep_n > 3:
                continue
            drop = args[keep_n:]
            if any('(' in d or '=' in d or '++' in d or '--' in d for d in drop):
                continue
            text = (text[:op + 1] + ','.join(a.strip() for a in args[:keep_n])
                    + text[end - 1:])
            n += len(drop)
            pos = op + 1
    return text, n


def fix_undeclared_underscore_local(text):
    r"""`_iVar3` used where only `iVar3` is declared — Ghidra's own name collision.

    `MeDictLookup` is the worked case. Ghidra declares `int iVar3;`, never uses
    it, and then writes the function against `_iVar3`, which it declares
    nowhere:

        int iVar3;                                     /* declared, unused */
        ...
        _iVar3 = (*dict->compare)(key,(pMVar2->nilnode).key);
        if (-1 < (int)_iVar3) break;
        ...
        if ((int)_iVar3 < 1) break;

    Three sibling functions IN THE SAME FILE emit the identical shape with a
    single `iVar3`, so this is a per-function decompiler quirk and not a second
    variable. `recover.py` catches it with the MISLABELLED-CALL detector, whose
    pattern is `error: '_<name>' undeclared` — the same diagnostic a genuinely
    mislabelled external produces, which is why it reads as one family and is
    four.

    THE GUARD IS WHAT MAKES IT SAFE, and it is not decoration. The rename only
    fires when `X` is declared in this function and has NO other use in it, so
    the declaration is exactly the storage `_X` wants and nothing is merged. If
    `X` is used too, Ghidra is distinguishing two values and merging them would
    be a silent semantic change — so it declines and the object stays in REVIEW,
    which is the right answer.
    """
    out, n = [], 0
    for _fn, region in _split_definitions(text):
        names = {m.group(1) for m in re.finditer(r'\b_([A-Za-z]\w*)\b', region)}
        for name in sorted(names):
            if not re.search(r'(?m)^[ \t]+[\w \*]*\b' + re.escape(name)
                             + r'\s*(\[[^\]]*\])?;\s*$', region):
                continue                      # `X` is not declared here
            if re.search(r'(?<![\w_])_' + re.escape(name) + r'\s*(\[[^\]]*\])?;\s*$',
                         region, re.M):
                continue                      # `_X` has a declaration of its own
            bare = [m for m in re.finditer(r'(?<![\w_])' + re.escape(name) + r'\b',
                                           region)]
            # One bare use is the declaration itself; more means Ghidra is
            # distinguishing two values and this must not merge them.
            if len(bare) != 1:
                continue
            region, k = re.subn(r'\b_' + re.escape(name) + r'\b', name, region)
            n += k
        out.append(region)
    return ''.join(out), n


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


_PROTO_TEXT = {}


def _slot_fnptr_type(sym, protos):
    """`void (**)(void *, …, int, float)` for a vtable slot, or None.

    ONLY where the slot takes a FLOAT, and that restriction is the whole point.
    Ghidra dispatches through `code *`, which kd_compat.h declares
    `typedef int code();` — an UNPROTOTYPED function type. On i386 an argument
    passed to one of those undergoes the default argument promotions, so a
    `float` is pushed as a DOUBLE: eight bytes where the callee reads four, and
    every argument after it shifted by four.

    That is not theoretical. keaRbdCore_unified dispatches
    `calcIworldandNonInertialForceandVhmf(…, int, float)` and
    `calcJinvMandRHS(…, int, int, int, float, float)` this way, and the moment
    the object compiled it produced 3.3e+14 on step 0 of the collision-free
    scene and NaN on step 1. Typing those two sites takes the same object to
    4.7e-03 m at step 0. Integer promotions need no such care — they are already
    int-sized on the stack — so a slot with no float keeps `code *` and nothing
    else in the corpus moves.

    Declines on a prototype that mentions `kd_aggNN`: those stand-ins are only
    emitted into a translation unit that passes one, and after
    collapse_outgoing_aggregate_copy has done its work it may not be."""
    if not protos:
        return None
    if protos not in _PROTO_TEXT:
        try:
            _PROTO_TEXT[protos] = open(protos, errors='ignore').read()
        except OSError:
            _PROTO_TEXT[protos] = ''
    m = re.search(r'^(\S+) ' + re.escape(sym) + r'\(([^)]*)\);', _PROTO_TEXT[protos], re.M)
    if not m:
        return None
    params = m.group(2)
    if 'kd_agg' in params or not re.search(r'\b(float|double)\b', params):
        return None
    return '%s (**)(%s)' % (m.group(1), params)


def fix_vptr_store(obj, text, declared=None, locals_table=None, protos=None):
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
                                     corpus, locals_table, protos)
            if new is not None:
                region, n = new, n + 1
        out.append(region)
    return ''.join(out), n


def _repair_vptr_store(fn, region, groups, declared, obj, corpus, locals_table,
                       protos=None):
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

    # ONE pass over the ORIGINAL region, not one per name. `skip` holds offsets
    # into that region, and substituting the first name shifts every offset
    # after it — so a two-pass rewrite silently un-skips exactly the sites the
    # check above refused. keaRbdCore_unified is what found it: the object
    # compiled, and `scene_chain` then called `allocate()` with no arguments and
    # a garbage size on step 4. The skip was right; the bookkeeping was not.
    subs = {'_' + name: alias, '_' + label: slot0}
    combined = re.compile('|'.join(
        r'(?<![\w])' + re.escape(t) + r'\b'
        for t in sorted(subs, key=len, reverse=True)))
    new = combined.sub(
        lambda m: m.group(0) if m.start() in skip else subs[m.group(0)], region)
    # A slot that takes a FLOAT cannot go through `code *` — see
    # _slot_fnptr_type. Retype those dispatches now that the base is named.
    new = _type_float_dispatches(new, alias, etype, name, table, protos)
    store = re.compile(r'^([ \t]*)' + re.escape(alias) + r'[ \t]*=[^;\n]*;[ \t]*$',
                       re.M)
    new, k = store.subn(
        lambda mm: f'{mm.group(1)}{alias} = ({etype} *)((char *)&{vname}[0] + 8);',
        new)
    return new if k == 1 else None


def _type_float_dispatches(region, alias, etype, name, table, protos):
    """Replace `(**(code **)(BASE + N))` with the slot's real function type.

    Only for a slot _slot_fnptr_type accepts, i.e. one that takes a float, and
    the shape is the same one Ghidra wrote — `(**(T (**)(…))((char *)BASE + N))`
    — so the call site around it is untouched.
    """
    esize = _FIXED_WIDTH.get(etype, 1)
    base = re.escape(alias)
    forms = [
        (re.compile(r'\(\*\*\(code \*\*\)\(' + base + r' \+ (0x[0-9a-f]+|\d+)\)\)'),
         lambda g: int(g, 0) * esize),
        (re.compile(r'\(\*\*\(code \*\*\)' + base + r'\)'), lambda g: 0),
        (re.compile(r'\(\*\(code \*\)' + base + r'\[(0x[0-9a-f]+|\d+)\]\)'),
         lambda g: int(g, 0) * esize),
        (re.compile(r'\(\*\(code \*\)\*' + base + r'\)'), lambda g: 0),
    ]
    for pat, off_of in forms:
        def repl(m, off_of=off_of):
            off = off_of(m.group(1)) if m.groups() else 0
            sym = table.get(off)
            fnptr = _slot_fnptr_type(sym, protos) if sym else None
            if not fnptr:
                return m.group(0)
            return '(**(%s)((char *)(*(%s **)&%s) + %d))' % (fnptr, etype, name, off)
        region = pat.sub(repl, region)
    return region


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


# x87 covers every float operation gcc 3.2 emits for these objects; the SSE
# scalar and packed forms are included so that over-detecting is the failure
# mode, since over-detecting only makes fix_void_pointer_member decline.
_FP_MNEMONIC = re.compile(r'f[a-z0-9]+$|[a-z0-9]{2,}(?:ss|sd|ps|pd)$')


def _function_has_float(obj, fn):
    """Does the SHIPPED function contain any floating-point instruction?

    None when the function cannot be located, which callers must treat as
    "cannot confirm" rather than as "no".

    The mnemonic is taken from objdump's THIRD tab-separated field, not matched
    out of the line. A first version matched the line and read the last hex
    BYTE as the mnemonic — `c7 45 f0` came back as `f0`, an x87 instruction
    that is not there — so every function looked as if it did float work and
    the rule declined everywhere. A check that cannot pass is not a check."""
    for name, lo, hi in function_extents(obj):
        if name != fn:
            continue
        out = subprocess.run(['objdump', '-d', '--start-address', hex(lo),
                              '--stop-address', hex(hi), obj],
                             capture_output=True, text=True).stdout
        for line in out.splitlines():
            parts = line.split('\t')
            if len(parts) < 3 or not parts[2].split():
                continue
            if _FP_MNEMONIC.match(parts[2].split()[0]):
                return True
        return False
    return None


def fix_void_pointer_member(obj, text, declared=None):
    """`pool_ptr->invI0` is `pool_ptr` — a `void *` has no members.

    Ghidra gives an untyped external its own working type and then spells the
    POINTER through it. keaMemory's pool cursor is `void *pool_ptr` in the
    DWARF of the object that defines it, and the recovered allocator reads

        if (pool_max < (undefined1 *)((int)pool_ptr->invI0    + iVar10))
        piVar4    =            (int *)((int)pool_ptr->invI0[0] + iVar10);
        mem->rhs  = pool_ptr->invI0;

    — three spellings of one thing. The machine code for that block is
    `mov pool_ptr,%edx; lea (%ebx,%edx,1),%eax; cmp; add %ebx,%edx;
    mov %edx,pool_ptr; mov %esi,0xc(%ecx)`: pointer arithmetic and a store,
    with NO load through the pointer anywhere in the function.

    THE OBVIOUS REPAIR IS WRONG AND WOULD COMPILE, which is why this one is
    written the way it is. Supplying the cast Ghidra elided —
    `((MdtKeaInverseMassMatrix *)pool_ptr)->invI0` — is right for the two
    array-decay spellings and WRONG for `->invI0[0]`, which then reads a
    `MeReal` out of the pool. Four sites correct, one silently reading memory
    the original never touches, and the object builds either way.

    So the rewrite drops the member access entirely, and four things are read
    before it does:

      1. the extern's DWARF type is `void *` — it has no members, so what
         Ghidra printed is its typing and not the program's;
      2. every cast Ghidra assigns to that name in this unit agrees on one
         struct type, which is therefore the type it was working with;
      3. that struct's own DWARF, in THIS object, puts the member at offset 0
         and declares it an array of a floating type — so the array-decay
         spellings denote the address of the object itself;
      4. the SHIPPED function contains no floating-point instruction, so the
         only other reading of `->M[0]` — a `MeReal` loaded from the pool — is
         refuted by the object rather than argued away.

    Check 4 is the one doing the work. Without it this is a guess about what
    Ghidra meant; with it, the alternative reading requires an instruction that
    is not in the function."""
    if not obj:
        return text, 0
    declared = declared if declared is not None else text
    voids = set(re.findall(r'\bextern[ \t]+void[ \t]*\*[ \t]*(\w+)[ \t]*;', declared))
    if not voids:
        return text, 0
    used = {v for v in voids if re.search(r'(?<![\w.>])_?' + re.escape(v) + r'\s*->', text)}
    if not used:
        return text, 0

    # Ghidra spells the four-byte VALUE at an EXTERNAL slot `_name`, and this
    # runs BEFORE fix_mislabelled_external has renamed it, so both spellings
    # have to be recognised. `_name` is accepted only where the relocations in
    # THIS function say it is that slot and nothing else — the same evidence
    # fix_mislabelled_external resolves on — and it is rewritten under the name
    # Ghidra gave it, so the naming stays that rule's job and not this one's.
    per_fn = relocation_targets(obj, per_function=True)
    dies = None
    out, n = [], 0
    for fn, region in _split_definitions(text):
        if not fn:
            out.append(region)
            continue
        spellings = []
        for v in sorted(used):
            spellings.append(v)
            if per_fn.get(fn, {}).get('_' + v) == {(v, 0)}:
                spellings.append('_' + v)
        for name in spellings:
            acc = re.compile(r'(?<![\w.>])' + re.escape(name)
                             + r'[ \t]*->[ \t]*(\w+)(?:[ \t]*\[[ \t]*0[ \t]*\])?')
            members = {m.group(1) for m in acc.finditer(region)}
            if not members:
                continue
            # (2) Ghidra's own type for the name, from the casts it prints.
            # `(void *)` is not one of them — it is the type the name already
            # has, so it says nothing about what Ghidra was working with, and
            # counting it made keaMemory's two casts look ambiguous.
            casts = {c for c in re.findall(
                r'(?<![\w.>])' + re.escape(name)
                + r'[ \t]*=[ \t]*\([ \t]*(\w+)[ \t]*\*[ \t]*\)', text)
                if c != 'void'}
            if len(casts) != 1:
                continue
            struct = next(iter(casts))
            # (4) the load the other reading needs is not in this function.
            if _function_has_float(obj, fn) is not False:
                continue
            if dies is None:
                dies = dwarf_structs.parse(obj)
            ok = True
            for member in sorted(members):
                # (3) offset 0 and an array of a floating type, from the DWARF
                # of this very object.
                got = _dwarf_member(dies, struct, member)
                if not got or got[0] != 0 or not re.search(
                        r'\b(MeReal|float|double)\b.*\[', got[1]):
                    ok = False
                    break
            if not ok:
                continue
            region, k = acc.subn(name, region)
            n += k
        out.append(region)
    return ''.join(out), n


def _dwarf_member(dies, struct, member):
    """(offset, declarator) for `struct.member`, straight out of .debug_info."""
    for die in dies.values():
        if die['tag'] not in ('DW_TAG_structure_type', 'DW_TAG_class_type'):
            continue
        if die['attrs'].get('DW_AT_name') != struct:
            continue
        for c in die['children']:
            if c['tag'] != 'DW_TAG_member' or c['attrs'].get('DW_AT_name') != member:
                continue
            loc = c['attrs'].get('DW_AT_data_member_location', '')
            m = dwarf_structs.OFF_RE.search(loc)
            ref = dwarf_structs.REF_RE.search(c['attrs'].get('DW_AT_type', ''))
            if not (m and ref):
                return None
            return (int(m.group(1)),
                    dwarf_structs.declarator(dies, int(ref.group(1), 16), member))
    return None


_FLOAT_TYPES = {'MeReal', 'float', 'double'}
# fistp/fist store an integer FROM the x87 stack; cvttss2si and friends are the
# SSE equivalents. If none of these is in a function, that function performs no
# float-to-int conversion, whatever the decompiled text says it does.
_F2I_MNEMONIC = re.compile(r'^(fi?st?t?p?|fist|fistp|fisttp|cvtt?s[sd]2si)$')


def _function_has_float_to_int(obj, fn):
    """Does the SHIPPED function convert a float to an integer? None if unsure."""
    for name, lo, hi in function_extents(obj):
        if name != fn:
            continue
        out = subprocess.run(['objdump', '-d', '--start-address', hex(lo),
                              '--stop-address', hex(hi), obj],
                             capture_output=True, text=True).stdout
        for line in out.splitlines():
            parts = line.split('\t')
            if len(parts) < 3 or not parts[2].split():
                continue
            m = parts[2].split()[0]
            if m in ('fistp', 'fist', 'fisttp') or m.startswith('cvtts') \
               or m.startswith('cvtss2si') or m.startswith('cvtsd2si'):
                return True
        return False
    return None


def _declared_pointee(region, sig, var):
    """`MeReal` for a `MeReal *var` declared as a local or a parameter here."""
    m = re.search(r'^[ \t]*([A-Za-z_]\w*)[ \t]*\*[ \t]*' + re.escape(var)
                  + r'[ \t]*;[ \t]*$', region, re.M)
    if m:
        return m.group(1)
    m = re.search(r'\b([A-Za-z_]\w*)[ \t]*\*[ \t]*' + re.escape(var) + r'\b', sig or '')
    return m.group(1) if m else None


def _expr_float_type(expr, region, sig, obj, dies):
    r"""The floating type of `expr`, or None. Only shapes it can RESOLVE.

    The subscript is matched with a BRACKET MATCHER, not with `.*`. A greedy
    `\[.*\]` reads `(axis->v[1] < axis->v[0])` as one subscripted member and
    types a COMPARISON as a float — which is how the first version of this rule
    silently changed IxCylinderCylinder, an object that had nothing wrong with
    it. Requiring the closing bracket to be the end of the expression is what
    keeps a top-level operator from being swallowed."""
    e = expr.strip()
    while e.startswith('(') and _match_bracket(e, 0) == len(e):
        e = e[1:-1].strip()
    base = e
    i = e.find('[')
    if i >= 0:
        if _match_bracket(e, i) != len(e):
            return None                 # the subscript is not the whole thing
        base = e[:i].strip()
    m = re.fullmatch(r'\*?[ \t]*(\w+)', base)
    if m:
        # `*p` and `p[i]` are the POINTEE's type; a bare `r` is its own. Only
        # the first was resolved, so `(MeU32)r` for a `MeReal r` parameter typed
        # as nothing and the rule declined — see INT_STORE above for what that
        # cost. `[` present means the subscript form, `*` the dereference.
        bare = not base.lstrip().startswith('*') and i < 0
        t = (_declared_type_name(region, sig, m.group(1)) if bare
             else _declared_pointee(region, sig, m.group(1)))
        return t if t in _FLOAT_TYPES else None
    m = re.fullmatch(r'\*?[ \t]*(\w+)->(\w+)', base)
    if m:
        owner = _declared_pointee(region, sig, m.group(1))
        got = _dwarf_member(dies, owner, m.group(2)) if owner else None
        if got and re.match(r'^(' + '|'.join(_FLOAT_TYPES) + r')\b', got[1]):
            return got[1].split()[0]
    return None


def _function_has_int_to_float(obj, fn):
    """Does the SHIPPED function convert an integer to a float? None if unsure.

    The mirror of _function_has_float_to_int, and the mnemonic is taken from
    objdump's THIRD tab-separated field for the same reason: matching it out of
    the line reads a hex byte as an instruction and the check declines
    everywhere. See _function_has_float."""
    for name, lo, hi in function_extents(obj):
        if name != fn:
            continue
        out = subprocess.run(['objdump', '-d', '--start-address', hex(lo),
                              '--stop-address', hex(hi), obj],
                             capture_output=True, text=True).stdout
        for line in out.splitlines():
            parts = line.split('\t')
            if len(parts) < 3 or not parts[2].split():
                continue
            m = parts[2].split()[0]
            if m.startswith('fild') or m.startswith('cvtsi2'):
                return True
        return False
    return None


def _dwarf_struct_of_typedef(dies, name, hops=8):
    """The STRUCT TAG behind a typedef, following the chain, or `name`.

    Declarations use Karma's typedef and .debug_info names the struct by its
    tag, so a member lookup keyed on the declared spelling finds nothing and the
    rule declines everywhere. That is the shape of failure this file has hit
    four times, so it is resolved rather than assumed.

    THE CHAIN IS NOT ONE HOP, which is the version that declined on two of the
    three objects it was written for: `McdSphylID` is a typedef of
    `McdGeometryID`, which is a typedef of `struct _McdGeometry *`. Pointer,
    const and volatile links are followed too, because the declared type of a
    Karma geometry handle is a POINTER typedef and the member being read belongs
    to what it points at."""
    seen = set()
    die = None
    for d in dies.values():
        if d['tag'] in ('DW_TAG_typedef', 'DW_TAG_base_type',
                        'DW_TAG_structure_type', 'DW_TAG_class_type') \
           and d['attrs'].get('DW_AT_name') == name:
            die = d
            break
    while die is not None and hops > 0:
        hops -= 1
        if die['tag'] in ('DW_TAG_structure_type', 'DW_TAG_class_type'):
            return die['attrs'].get('DW_AT_name') or name
        if die['off'] in seen:
            break
        seen.add(die['off'])
        ref = dwarf_structs.REF_RE.search(die['attrs'].get('DW_AT_type', ''))
        if not ref:
            break
        die = dies.get(int(ref.group(1), 16))
    return name


def _declared_type_name(region, sig, var):
    """The bare declared TYPE NAME of `var` here — `McdSphylID` for
    `McdSphylID s`. Complements _declared_pointee, which only sees `T *var`."""
    m = re.search(r'^[ \t]*(?:const[ \t]+)?([A-Za-z_]\w*)[ \t]+' + re.escape(var)
                  + r'[ \t]*(?:\[[^\]]*\])?[ \t]*;[ \t]*$', region, re.M)
    if m:
        return m.group(1)
    m = re.search(r'(?:\(|,)[ \t]*(?:const[ \t]+)?([A-Za-z_]\w*)[ \t]+'
                  + re.escape(var) + r'[ \t]*(?:,|\))', sig or '')
    return m.group(1) if m else None


def _expr_int_type(expr, region, sig, obj, dies):
    """The INTEGER type of `expr`, or None. Deliberately the same shapes, and
    the same bracket matcher, as _expr_float_type — see its docstring for why a
    greedy subscript is not acceptable here."""
    e = expr.strip()
    while e.startswith('(') and _match_bracket(e, 0) == len(e):
        e = e[1:-1].strip()
    # `g[1].mRefCtAndID` / `s[1].prev` — a member of the struct a pointer names.
    # This is tried FIRST because the whole-expression subscript guard below
    # rejects it: in `g[1].mRefCtAndID` the bracket closes at 4 and the
    # expression is 16 long, so the guard returns None before the member
    # pattern is ever reached. That is how the rule declined on every one of the
    # sites it was written for.
    m = re.fullmatch(r'\*?[ \t]*(\w+)(?:\[[^\]]*\])?[ \t]*(?:->|\.)[ \t]*(\w+)', e)
    if m:
        owner = (_declared_pointee(region, sig, m.group(1))
                 or _declared_type_name(region, sig, m.group(1)))
        if owner:
            owner = _dwarf_struct_of_typedef(dies, owner)
        got = _dwarf_member(dies, owner, m.group(2)) if owner else None
        if got and not re.match(r'^(' + '|'.join(_FLOAT_TYPES) + r')\b', got[1]):
            return got[1]
        return None
    base = e
    i = e.find('[')
    if i >= 0:
        if _match_bracket(e, i) != len(e):
            return None                 # the subscript is not the whole thing
        base = e[:i].strip()
    m = re.fullmatch(r'\*?[ \t]*(\w+)', base)
    if m:
        t = _declared_pointee(region, sig, m.group(1))
        return t if t and t not in _FLOAT_TYPES else None
    return None


# `(float)g[1].mRefCtAndID` — a cast applied to something that is not a call and
# not already a pointer dereference of the right type. The operand is captured
# with the postfix scanner rather than `.*` so a following binary operator is
# not swallowed; see _expr_float_type on the greedy-subscript bug.
FLOAT_CAST = re.compile(r'\(float\)[ \t]*(?=[A-Za-z_*])')


def fix_float_load_of_int(obj, text):
    r"""`(float)g[1].prev` is a four-byte LOAD, not a conversion.

    The mirror of fix_int_store_of_float, and the same defect seen from the
    other side. UT2004's geometry types derive from `McdGeometry` by prefixing
    it, so a `McdBox`'s dimensions live PAST the base struct. Ghidra has only
    `McdGeometry *`, so it addresses them as `g[1].<field-of-the-base>` — the
    byte offset is right and the TYPE is whatever field of a second
    `McdGeometry` happens to sit there:

        fVar1 = ABS(tm->row[2].v.v[0]) * (*(float *)&(g[1].next)) +
                ABS(tm->row[1].v.v[0]) * (*(float *)&(g[1].prev)) +
                ABS(tm->row[0].v.v[0]) * (float)g[1].mRefCtAndID;

    Two of the three are already reinterprets and the third is a CONVERSION,
    which is a different program: `mRefCtAndID` is an `int`, so C converts the
    bit pattern instead of reading the float that is there.

    THIS IS DEAD END 9, RE-DIAGNOSED. That entry blames `fix_pointer_as_float`
    for "cycling through the casts on a line until one is accepted" and
    concludes the compiler is reasoning about Ghidra's types and is wrong. The
    first half is right and the conclusion was too weak: gcc rejects only the
    cast whose operand is a POINTER, so cycling fixes that one and leaves the
    `int` one — which compiles, and is wrong. The line needs both, and which
    both is decided by the operand's type, not by which one the compiler
    complained about.

    THE EVIDENCE IS THE MACHINE CODE, exactly as in the store direction. An
    integer-to-float conversion on i386 is `fild` or `cvtsi2s*`; `McdBox.o`,
    `McdSphyl.o` and `McdTriangleList.o` contain NOT ONE, in any function — every
    float they touch arrives by `flds`. So a `(float)` applied to an integer
    lvalue in those functions cannot be a conversion the program performs.

    Both halves are required, as in the store direction: the operand must
    RESOLVE to a non-floating type from a declaration or from the object's own
    DWARF, and the function must contain no conversion instruction. An
    expression this cannot type is left alone; so is a function that really does
    convert."""
    if not obj:
        return text, 0
    dies, out, n = None, [], 0
    for fn, region in _split_definitions(text):
        if not fn or '(float)' not in region:
            out.append(region)
            continue
        if _function_has_int_to_float(obj, fn) is not False:
            out.append(region)
            continue
        sig = signature_of(region)
        if dies is None:
            dies = dwarf_structs.parse(obj)
        pieces, last = [], 0
        for m in FLOAT_CAST.finditer(region):
            end = scan_unary_forward(region, m.end())
            if end is None or end <= m.end():
                continue
            operand = region[m.end():end]
            if _expr_int_type(operand, region, sig, obj, dies) is None:
                continue
            pieces.append(region[last:m.start()])
            pieces.append('*(float *)&(%s)' % operand.strip())
            last = end
            n += 1
        pieces.append(region[last:])
        out.append(''.join(pieces))
    return ''.join(out), n


# Ghidra spells the destination with whatever integer type the SLOT has, not
# with `int`. `McdSphylCreate` writes the capsule radius as
# `pMVar1[1].mRefCtAndID = (MeU32)r;` — a truncation to zero for any radius
# under 1 — where the shipped code does `fstps 0x10(%ebx)`. Matching only
# `(int)` left that in place, and because McdSphyl never compiled it had never
# been run: the first scene it reached produced 8,100 non-finite samples.
INT_STORE = re.compile(r'^(?P<ind>[ \t]*)(?P<lhs>[^;=\n]+?) = '
                       r'\((?:int|uint|long|ulong|short|ushort|char|byte|sbyte'
                       r'|MeI32|MeU32|MeI16|MeU16|MeI8|MeU8'
                       r'|undefined4|undefined2|undefined1)\)'
                       r'(?P<rhs>[^;\n]+);[ \t]*$', re.M)


def fix_int_store_of_float(obj, text):
    """`p[i] = (int)q[i];` is a four-byte MOVE, not a conversion.

    Ghidra type-puns a `MeReal *` into a struct pointer — `this_00 =
    (keaLCPSolver *)this->x;` — and then writes through the first member, which
    is an `int`. In C that is a float-to-int CONVERSION and it destroys the
    value; in the shipped code it is `mov`, four bytes moved unchanged.

    NOTHING DIAGNOSES THIS. It compiles without a warning, which is why
    keaLCPSolver sat in the build for a session with three of these in
    PrincipalSubmatrix, putting a ragdoll body 3.7 m out of place on step 1
    while twelve of its fifteen functions were bit-identical. That is also why
    this is a PRE-PASS: there is no diagnostic for a repair rule to hang on.

    THE EVIDENCE IS THE MACHINE CODE. A float-to-int conversion on i386 is
    `fistp`, `fisttp` or a `cvtt*2si`; keaLCPSolver contains NOT ONE, in any
    function. So an `(int)` applied to a floating expression there cannot be a
    conversion the program performs, and the store is of the float. The rewrite
    keeps the address exactly as Ghidra computed it and only fixes the width
    question: `*(T *)&(LHS) = RHS`.

    Both halves are required. The RHS must RESOLVE to a floating type — from a
    local's or parameter's declaration, or from the object's own DWARF for a
    struct member — and the function must contain no conversion instruction. An
    expression this cannot type is left alone; so is a function that really does
    convert."""
    if not obj:
        return text, 0
    dies, out, n = None, [], 0
    for fn, region in _split_definitions(text):
        # Test the PATTERN, not the literal `(int)`. The early-out used to look
        # for that string, so widening INT_STORE to the other integer spellings
        # changed nothing at all: `McdSphylCreate` contains `(MeU32)` and no
        # `(int)`, so it was skipped before the regex ever ran. A cheap guard
        # that disagrees with the rule it guards is a rule that silently
        # declines — the fifth time in this file.
        if not fn or not INT_STORE.search(region):
            out.append(region)
            continue
        if _function_has_float_to_int(obj, fn) is not False:
            out.append(region)
            continue
        sig = signature_of(region)
        if dies is None:
            dies = dwarf_structs.parse(obj)

        def rewrite(m):
            nonlocal n
            t = _expr_float_type(m.group('rhs'), region, sig, obj, dies)
            if not t:
                return m.group(0)
            n += 1
            return '%s*(%s *)&%s = %s;' % (m.group('ind'), t, m.group('lhs'),
                                           m.group('rhs'))

        out.append(INT_STORE.sub(rewrite, region))
    return ''.join(out), n


# `uVar6 = this->n * 4 + 0xfU & 0xfffffff0;` — the alloca SIZE, positive. This
# is the same `+ 0xf & 0xfffffff0` rounding materialise_alloca_frame reads, in
# the form gcc leaves when the size is used more than once.
ALLOCA_SIZE = re.compile(
    r'^[ \t]*(?P<var>\w+) = (?P<expr>[^;\n]+?) \* (?P<k>0x[0-9a-f]+|\d+)'
    r' \+ 0xfU? & 0xfffffff0;[ \t]*$', re.M)


def _shifted_frame_refs(region, var):
    """[(whole match, base KEY, frame offset or None, multiplier)].

    The key is the base's spelling, `stack0xHHHH` included — keying on the
    identifier alone gives every `stack0x` base the key None, and then two of
    them at different offsets look like one base that disagrees with itself."""
    pat = re.compile(
        r'(?P<all>(?:&stack0x(?P<hex>[0-9a-f]{8})|&?(?P<name>[A-Za-z_]\w*))'
        r'[ \t]*\+[ \t]*' + re.escape(var) + r'[ \t]*\*[ \t]*-(?P<k>\d+))')
    out = []
    for m in pat.finditer(region):
        if m.group('hex'):
            key = 'stack0x' + m.group('hex')
            off = struct.unpack('<i', struct.pack('<I', int(m.group('hex'), 16)))[0]
        else:
            key, off = m.group('name'), None
        out.append((m.group('all'), key, off, int(m.group('k'))))
    return out


# ---------------------------------------------------------------------------
# FLOAT_TEXT_REPAIRS — sites where the decompiled text is arithmetically wrong
# in a way NO compile diagnostic and NO i386 gate can see.
#
# Two shapes live here, both found on keaIntegrate_pc on 2026-08-25.
#
#   'spill'  Ghidra folded a store to a float local and its reload into one
#            expression. On i386 the x87 keeps intermediates at 80 bits, so
#            dropping that store drops a rounding the shipped code performed
#            and the last bit drifts. The local is left DECLARED AND UNUSED,
#            which is the textual fingerprint; the disassembly is the proof.
#
#   'assoc'  Ghidra printed a right-leaning float `+` chain FLAT — `a + b + c`
#            for a tree that is `a + (b + c)` — so C re-parses it left-leaning
#            over the wrong operand order. Float addition is not associative.
#
# THE SECOND ONE IS INVISIBLE TO EVERY GATE THIS PROJECT HAS, and that is the
# reason it is repaired rather than left alone. A float product needs 48 bits
# of mantissa and the x87 register has 64, so on i386 these sums are EXACT
# whatever the association: measured 0 differences in 2,000,000 samples under
# `-mfpmath=387`. The same probe under `-mfpmath=sse` — storage precision,
# which is what wasm32, armv7 and arm64 all give — differs in 31% of them.
# So the substitute scenes cannot fail on this and the actual port target can.
#
# Each entry carries an evidence precondition that is CHECKED, not asserted:
# `old` must appear exactly once, and a 'spill' site must show `fst(p)s` and a
# matching `flds` against the named local's slots in the shipped disassembly.
# A site that stops matching raises. A repair that silently declines is the
# failure mode this file has hit four times (HANDOVER.md §12); it must not be
# available here.
#
# These are per-site because the CORRECTION is not yet derivable by rule — see
# HANDOVER.md §11 item 2a for the 220-site scan and what a general rule needs.
FLOAT_TEXT_REPAIRS = {
    ('keaIntegrate_pc', 'KeaIntegrateSystem_vanilla'): [
        # -- assoc: MeVector3Dot(myw, fastSpinAxis). KDynStep.cpp:647 has the
        #    original source verbatim; the shipped fadd order agrees.
        dict(kind='assoc', old="""        fVar1 = myw[2] * pMVar11->fastSpinAxis[2] +
                myw[0] * pMVar11->fastSpinAxis[0] + myw[1] * pMVar11->fastSpinAxis[1];""",
             new="""        fVar1 = myw[0] * pMVar11->fastSpinAxis[0] +
                myw[1] * pMVar11->fastSpinAxis[1] + myw[2] * pMVar11->fastSpinAxis[2];"""),
        # -- spill: `rot` is spilled as a float across the MeQuaternionFiniteRotation
        #    call (fstps -0x48(%ebp) / flds -0x48(%ebp)), so the value the three
        #    myw updates see is float-rounded; the multiply that forms the CALL
        #    ARGUMENT happens before the spill and uses the 80-bit value.
        #    myw itself is a declared MeReal[3] the shipped code writes with fstps.
        dict(kind='spill', local='myw',
             old="""        pMVar11 = *blist;
        myw[0] = myw[0] - pMVar11->fastSpinAxis[0] * fVar1;
        myw[1] = myw[1] - pMVar11->fastSpinAxis[1] * fVar1;
        myw[2] = myw[2] - fVar1 * pMVar11->fastSpinAxis[2];""",
             new="""        pMVar11 = *blist;
        fVar1 = KD_F32(fVar1);
        myw[0] = KD_F32(myw[0] - pMVar11->fastSpinAxis[0] * fVar1);
        myw[1] = KD_F32(myw[1] - pMVar11->fastSpinAxis[1] * fVar1);
        myw[2] = KD_F32(myw[2] - fVar1 * pMVar11->fastSpinAxis[2]);"""),
        # -- spill + assoc: `MeReal dq[4]` at ebp-0x38. The shipped code stores
        #    all four and RELOADS dq[1..3] before the `* stepsize` (dq[0] is kept
        #    in st, so its store is dead and it must NOT be rounded). dq[2]'s
        #    chain is also mis-parenthesised — KDynStep.cpp:657.
        dict(kind='spill', local='dq',
             old="""      (*blist)->qrot[1] =
           ((fVar1 * myw[0] + fVar2 * myw[1]) - fVar3 * myw[2]) * 0.5 * parameters.stepsize +
           (*blist)->qrot[1];
      (*blist)->qrot[2] =
           (fVar6 * myw[2] + -fVar4 * myw[0] + fVar5 * myw[1]) * 0.5 * parameters.stepsize +
           (*blist)->qrot[2];
      (*blist)->qrot[3] =
           (myw[2] * fVar9 + (myw[0] * fVar7 - myw[1] * fVar8)) * 0.5 * parameters.stepsize +
           (*blist)->qrot[3];""",
             new="""      dq[1] = KD_F32(((fVar1 * myw[0] + fVar2 * myw[1]) - fVar3 * myw[2]) * 0.5);
      (*blist)->qrot[1] = dq[1] * parameters.stepsize + (*blist)->qrot[1];
      dq[2] = KD_F32((-fVar4 * myw[0] + fVar5 * myw[1] + fVar6 * myw[2]) * 0.5);
      (*blist)->qrot[2] = dq[2] * parameters.stepsize + (*blist)->qrot[2];
      dq[3] = KD_F32((myw[2] * fVar9 + (myw[0] * fVar7 - myw[1] * fVar8)) * 0.5);
      (*blist)->qrot[3] = dq[3] * parameters.stepsize + (*blist)->qrot[3];"""),
        # -- assoc: `for (j=0;j<4;j++) s += MeSqr(qrot[j])` accumulates from q0.
        #    KDynStep.cpp:667; the shipped faddp order agrees.
        dict(kind='assoc',
             old="      fVar1 = 1.0 / SQRT(fVar4 * fVar4 + fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1);",
             new="      fVar1 = 1.0 / SQRT(fVar1 * fVar1 + fVar2 * fVar2 + fVar3 * fVar3 + fVar4 * fVar4);"),
    ],
    ('keaMatrix_PcSparse_vanilla', 'solve'): [
        # -- round_local: `MeReal tmp[4]` at ebp-0x28. gcc 3.2 accumulates the
        #    running total THROUGH MEMORY — sixteen `fsts` and nineteen `fadds`
        #    against those four slots, and not one `flds` — so every partial sum
        #    is rounded to 32 bits. Ghidra keeps it in an expression and gcc then
        #    keeps it in an 80-bit register.
        #
        #    test/bisect_object.sh (with the vtable-isolation fix) takes this
        #    function from `first@0=4.700e-10 max=4.300e-04` to IDENTICAL, with
        #    both its controls reading identical. The other five functions of the
        #    object were already exact; `factorize` still is not, and blanket
        #    rounding of the two Cholesky statics makes it WORSE
        #    (`first@10` -> `first@7`), which is dead end 17 in miniature: the
        #    rounding has to go where the original spilled, not everywhere.
        dict(kind='round_local', local='tmp'),
    ],
}

# A 4-byte x87 access to a frame slot. The WIDTH matters: `fstpl`/`fstpt` is a
# double or extended spill, which loses nothing and is not what this looks for.
#
# `fadds -0x28(%ebp)` counts as a RELOAD. gcc 3.2 accumulates through memory —
# `fsts` the running total, then `fadds` it back next iteration — so requiring a
# literal `flds` misses the whole shape: `keaMatrix_pcSparse_vanilla::solve`
# writes tmp[0..3] with `fsts` sixteen times and reads them back with `fadds`
# nineteen times and not one `flds`. A check that cannot see the case it is
# there for is the failure mode this file has hit four times.
_F32_SLOT = re.compile(
    r'\bf(?P<op>stps?|sts|lds|adds|subs|subrs|muls|divs|divrs|coms|comps)'
    r'\s+-?0x(?P<off>[0-9a-f]+)\(%ebp\)')
_F32_STORE_OPS = ('stps', 'sts')


def _f32_spill_slots(obj, fn):
    """({stored offsets}, {read-back offsets}) as ebp-relative ints, from the
    SHIPPED disassembly. ({}, {}) when the function cannot be located."""
    for name, lo, hi in function_extents(obj):
        if name != fn:
            continue
        out = subprocess.run(['objdump', '-d', '--start-address', hex(lo),
                              '--stop-address', hex(hi), obj],
                             capture_output=True, text=True).stdout
        stored, loaded = set(), set()
        for m in _F32_SLOT.finditer(out):
            off = int(m.group('off'), 16)
            if '-0x' in m.group(0):
                off = -off
            (stored if m.group('op') in _F32_STORE_OPS else loaded).add(off)
        return stored, loaded
    return set(), set()


def restore_float_text(text, obj, locals_table):
    """Apply FLOAT_TEXT_REPAIRS for this object, checking the evidence first.

    Raises rather than declining. Every site here is a defect no gate in this
    project can see on i386, so a silent no-op would look exactly like success
    and would ship the bug to wasm — see the table's comment."""
    base = os.path.basename(obj)
    base = base[:-2] if base.endswith('.o') else base
    sites = [(fn, s) for (o, fn), lst in FLOAT_TEXT_REPAIRS.items()
             if o == base for s in lst]
    if not sites:
        return text, 0
    n = 0
    for fn, site in sites:
        if site['kind'] in ('spill', 'round_local'):
            # `<obj>.locals` gives the local's frame offset; the shipped code
            # must both STORE into it and READ IT BACK, which is what makes the
            # dropped rounding observable rather than a dead spill.
            loc = locals_table.get(fn, {}).get(site['local'])
            if loc is None:
                raise SystemExit(
                    f'restore_float_text: {base}:{fn}: no `{site["local"]}` in '
                    f'<object>.locals — the frame evidence this repair rests on '
                    f'is gone; re-derive it before trusting the site')
            # `.locals` offsets are relative to the first parameter slot, which
            # is ebp+8: `blist,4` is `0x8(%ebp)`. So ebp = stackoff + 4.
            off, size, _ty = loc
            lo, hi = off + 4, off + 4 + size
            stored, loaded = _f32_spill_slots(obj, fn)
            hit_s = {o for o in stored if lo <= o < hi}
            hit_l = {o for o in loaded if lo <= o < hi}
            if not (hit_s and hit_l):
                raise SystemExit(
                    f'restore_float_text: {base}:{fn}: `{site["local"]}` at '
                    f'[{lo:#x},{hi:#x}) shows {len(hit_s)} f32 store(s) and '
                    f'{len(hit_l)} read-back(s) in the shipped code — the spill '
                    f'this repair restores is not there')
        if site['kind'] == 'round_local':
            # Scoped mechanical rewrite rather than a literal text match: every
            # assignment to the named local inside this function goes through
            # KD_F32. Robust to a re-dump renaming the temporaries around it,
            # which a literal `old`/`new` pair is not.
            region = None
            for name, body in _split_definitions(text):
                if name == fn:
                    region = body
                    break
            if region is None:
                raise SystemExit(
                    f'restore_float_text: {base}: no `{fn}` in the emitted text')
            pat = re.compile(r'(?m)^([ \t]*)(' + re.escape(site['local'])
                             + r'\[[^\]]*\]) = ((?![ \t]*KD_F32\b)[^;]+);[ \t]*$')
            new, k = pat.subn(lambda m: '%s%s = KD_F32(%s);'
                              % (m.group(1), m.group(2), m.group(3)), region)
            if k == 0:
                raise SystemExit(
                    f'restore_float_text: {base}:{fn}: no assignment to '
                    f'`{site["local"]}[]` to round. The dump changed; re-read '
                    f'the site against the disassembly rather than deleting it.')
            text = text.replace(region, new, 1)
            n += 1
            continue
        if text.count(site['old']) != 1:
            raise SystemExit(
                f'restore_float_text: {base}:{fn}: site text matched '
                f'{text.count(site["old"])} times, expected 1. The dump changed; '
                f're-read the site against the disassembly rather than deleting it.')
        text = text.replace(site['old'], site['new'])
        n += 1
    return text, n


def materialise_shifted_frame(text, obj, locals_table):
    r"""Give an alloca'd frame real storage, instead of a twelve-byte local.

    gcc 3.2 lowers several `alloca` calls in one function to a run of
    `sub %edi,%esp`, and Ghidra models the result by re-basing the whole shifted
    area on whichever declared local sits at its top and writing every address
    as A FIXED FRAME OFFSET PLUS A RUNTIME SHIFT:

        uint auStack_3c [3];                             /* twelve bytes */
        uVar6 = this->n * 4 + 0xfU & 0xfffffff0;         /* a runtime size */
        I         = (int *)((int)auStack_3c - uVar6);
        unclamped = (int *)((int)auStack_3c + uVar6 * -6);
        *(keaLCPSolver **)(&stack0xffffffb4 + uVar6 * -6) = this;

    `keaLCPSolver::solveLCP` does that six times and then once more for a
    twelve-byte outgoing argument area, and the shipped prologue says so
    outright: six `sub %edi,%esp` followed by `sub $0xc,%esp`.

    LEFT ALONE THIS COMPILES AND CORRUPTS. `auStack_3c` is twelve bytes, so
    every one of those addresses is below a twelve-byte local and the function
    reads and writes `6*(4n+15 & ~15)` bytes of somebody else's stack. That is
    why HANDOVER.md said twice not to name the two `stack0x` slots: naming them
    takes the object to zero errors and hands the build a landmine.

    WHAT MAKES THE REPAIR SAFE IS THAT THE LAYOUT IS READ, NOT INVENTED.

      * every base's frame offset comes from `<object>.locals` — Ghidra's own
        symbol map — or, for a `stack0xHHHH`, from the name, which §5c checked
        against that map over the whole corpus: 15 of 15 exact, 0 disagreements;
      * the RELATIVE offsets are therefore preserved exactly, which is all the
        recovered function needs. It does not have to match the original's frame
        LAYOUT, only to be self-consistent — the same standard
        collapse_outgoing_aggregate_copy is held to;
      * the block is sized from the lowest offset used and the largest
        multiplier, so it is big enough BY CONSTRUCTION;
      * and the two areas cannot overlap, also by construction: the alloca
        regions occupy `[base - K*size, base)` and every other base sits below
        `base`, so shifted by the same `K*size` it stays below them.

    The alloca base is identified as the base used with multiplier ONE — the
    first region — and it is redeclared as a pointer into the block, so its own
    uses need no rewriting at all: `auStack_3c[2]`, `(int)auStack_3c - uVar6`
    and `auStack_3c[i - uVar6]` all keep working, the last because Ghidra's
    element arithmetic on a `uint *` is the same bytes as `- 4*uVar6`.

    Declines unless there is exactly one alloca-size variable, exactly one
    alloca base, and a known offset for every shifted base.
    """
    if not locals_table:
        return text, 0
    out, n = [], 0
    for fn, region in _split_definitions(text):
        new = _materialise_one_frame(fn, region, locals_table) if fn else None
        out.append(new if new is not None else region)
        n += 1 if new is not None else 0
    return ''.join(out), n


def _materialise_one_frame(fn, region, locals_table):
    sizes = list(ALLOCA_SIZE.finditer(region))
    if len(sizes) != 1:
        return None
    var = sizes[0].group('var')
    if len(re.findall(r'^[ \t]*' + re.escape(var) + r' = ', region, re.M)) != 1:
        return None                      # the size must not be reassigned
    refs = _shifted_frame_refs(region, var)
    if not refs:
        return None

    # The alloca BASE: used with multiplier one, or as `(int)NAME - var`.
    base = None
    for m in re.finditer(r'\(int\)([A-Za-z_]\w*) - ' + re.escape(var) + r'\b', region):
        if base and base != m.group(1):
            return None
        base = m.group(1)
    if not base:
        return None
    bdecl = re.search(r'^([ \t]*)([A-Za-z_]\w*)[ \t]+' + re.escape(base)
                      + r'[ \t]*\[[ \t]*(\d+)[ \t]*\][ \t]*;[ \t]*\n', region, re.M)
    if not bdecl:
        return None
    elem = _FIXED_WIDTH.get(bdecl.group(2))
    if not elem:
        return None
    bsize = int(bdecl.group(3)) * elem

    locs = locals_table.get(fn, {})
    if base not in locs:
        return None
    base_off = locs[base][0]

    maxk = 0
    for _all, _name, _off, k in refs:
        maxk = max(maxk, k)
    for m in re.finditer(re.escape(base) + r'[ \t]*\+[ \t]*' + re.escape(var)
                         + r'[ \t]*\*[ \t]*-(\d+)', region):
        maxk = max(maxk, int(m.group(1)))
    maxk = max(maxk, 1)

    offs = {}
    for _all, key, off, _k in refs:
        if key == base:
            continue
        if off is None:
            if key not in locs:
                return None
            off = locs[key][0]
        if offs.get(key, off) != off:
            return None
        offs[key] = off
    minoff = min([base_off] + list(offs.values()))
    top = base_off + bsize
    if any(o >= base_off for o in offs.values()):
        return None                      # a base at or above the alloca base

    # ---- emit --------------------------------------------------------------
    ind = bdecl.group(1)
    region = region.replace(bdecl.group(0), '', 1)
    # The marker is load-bearing, not decoration. After this repair the base is
    # a POINTER into storage sized by construction, so `(int)base + var * -K`
    # is ordinary address arithmetic — but it is textually identical to the
    # invented-array shape recover.py's GHIDRA_STACK_GUESS exists to catch, and
    # that detector would otherwise quarantine the object this repair just
    # fixed. Naming the bases it repaired is how the detector tells the two
    # apart; a re-dump that stops triggering the repair drops the marker and
    # re-arms it in the same edit.
    setup = ('%schar *kd_frame = (char *)alloca(%s * %d + %d);\n'
             '%s%s *%s = (%s *)(kd_frame + %s * %d + %d);'
             '  /* KD_MATERIALISED_BASE(%s) */\n'
             % (ind, var, maxk, top - minoff,
                ind, bdecl.group(2), base, bdecl.group(2), var, maxk,
                base_off - minoff, base))
    at = sizes[0].end()
    at = region.index('\n', region.index(sizes[0].group(0))) + 1
    region = region[:at] + setup + region[at:]

    for whole, key, _off, k in refs:
        if key == base:
            continue
        o = offs[key] - minoff
        if maxk == k:
            rep = '(kd_frame + %d)' % o
        else:
            rep = '(kd_frame + %s * %d + %d)' % (var, maxk - k, o)
        region = region.replace(whole, rep)
    return region


_LOADED_REG = r'e(?:ax|bx|cx|dx|si|di|bp)'

# A write to a register kills the fact that it still holds the symbol. Listed
# rather than inferred, because the cost of missing one is a displacement
# attributed to the wrong base — and the caller's check against the text is
# what catches that, not this list.
_REG_WRITE = (r'\b(?:mov\w*|lea|pop|add|sub|xor|and|or|inc|dec|imul|sar|shl|shr'
              r'|neg|not|set\w+)\b[^,]*,\s*%%%s$')


def extern_load_displacements(obj, sym):
    """{function: {byte displacements used off the register loaded FROM sym}}.

    The instrument that decides how Ghidra TYPED an external data symbol, by
    reading the machine code instead of the decompiled text — because the text
    does not say. Ghidra folds the load of a pointer variable away, so

        mov gDebug,%edx ; mov (%edx),%eax ; mov 0x24(%edx),%ecx

    comes out as `*_gDebug` and `_gDebug[9]`, and the element SIZE that turns 9
    into 0x24 is nowhere in the file. It is here: the displacements the function
    actually uses off the register the symbol was loaded into.

    ONLY A LOAD OF THE SYMBOL'S VALUE COUNTS — `mov <sym>,%reg`, opcode 8b. An
    address taken (`push $<sym>`, `lea`) records nothing, which is what keeps
    this away from `fix_external_base_arithmetic`'s `&pool_ptr + i*4`: that is a
    different shape with a different answer. Control, on
    ReadWriteKeaInputToFile.o: `.rodata.str1.1` is referenced 456 times, every
    one of them an address, and this returns the empty set for it.

    Tracking stops at the first write to the register, so a displacement is only
    reported while the register still holds the symbol. Where that analysis is
    wrong the result disagrees with the text and the caller declines; it is
    checked, not trusted."""
    out = subprocess.run(['objdump', '-dr', obj],
                         capture_output=True, text=True).stdout
    res, fn, live, prev = {}, None, {}, None
    for line in out.splitlines():
        m = re.match(r'^[0-9a-f]+ <(.+)>:$', line)
        if m:
            fn, live, prev = m.group(1), {}, None
            continue
        m = re.match(r'^\s+[0-9a-f]+:\s+(?:[0-9a-f]{2} )+\s*\t(.*)$', line)
        if m:
            insn = m.group(1).strip()
            for reg in list(live):
                for d in re.finditer(r'(-?0x[0-9a-f]+)?\(%' + reg + r'\)', insn):
                    res.setdefault(fn, set()).add(
                        int(d.group(1), 16) if d.group(1) else 0)
            for reg in list(live):
                if re.search(_REG_WRITE % reg, insn) or \
                        re.match(r'^(?:pop|inc|dec|neg|not)\s+%' + reg + r'$', insn):
                    del live[reg]
            prev = insn
            continue
        m = re.search(r'\bR_386_32\s+(\S+)\s*$', line)
        if m and prev is not None and m.group(1) == sym:
            lm = re.match(r'^mov\s+0x0,%(' + _LOADED_REG + r')$', prev)
            if lm:
                live[lm.group(1)] = True
    return res


_ELEM_WIDTH_TYPE = {1: 'char', 2: 'short', 4: 'int'}


def normalise_external_indexing(obj, text):
    r"""One external symbol, two incompatible types, in one file.

    `ReadWriteKeaInputToFile` reads `gDebug` four times and Ghidra typed it
    differently in three functions than in the fourth:

        *(void **)(_gDebug + 4)                 /* byte arithmetic  */
        (*_gDebug != 0) && (_gDebug[8] == _gDebug[9])   /* element arithmetic */

    NO SINGLE C DECLARATION MAKES BOTH RIGHT, and that is the point: it is a
    contradiction visible in the text, so it cannot be left alone and it cannot
    be resolved by preferring one spelling. Declaring the import `char *` makes
    the first three correct and silently turns `_gDebug[8]` into byte 8; giving
    it the real `MdtKeaDebugDataRequest *` makes `+ 4` mean byte 176. Either way
    the object COMPILES and reads the wrong memory, which is the failure mode
    this project has been bitten by more than once.

    THE SCALE IS READ OUT OF THE MACHINE CODE AND THEN CHECKED.
    `extern_load_displacements` gives the byte displacements each function uses
    off the register the symbol was loaded into; the text gives that function's
    element indices and byte offsets. The rule accepts a scale only when the two
    agree EXACTLY as sets:

        checkPrintDebugInput   text {*0, [8], [9]}   code {0, 0x20, 0x24}  -> 4
        writeKeaInputToFile    text {+4}             code {0x4}            -> 1
        readKeaInputFromFile   text {+0xc}           code {0xc}            -> 1
        writeLambdaToFile      text {+0x1c}          code {0x1c}           -> 1

    Two scales for one name is the contradiction; every site is then rewritten
    to explicit byte arithmetic against a `char *`, which is correct under
    ANY declaration the prelude gives the symbol and leaves the name itself for
    `fix_mislabelled_external` to resolve.

    It declines — leaving the object in review, which is the honest outcome —
    when the name is used in any other shape, when a function's text and code do
    not agree under a single scale, when two scales are both consistent, or when
    the symbol's address is taken rather than its value loaded.

    Corpus-wide the contradiction appears in TWO objects and this fires on one.
    The other is `keaLCPSolver`'s `_vanillaQMatrix`, which is not an external at
    all — it is a local `fix_vptr_store` already resolves, and running after it
    means that spelling is gone before this looks. `keaLCPSolver` is validated
    and bit-identical on three scenes; it must not change, and it does not."""
    if not obj or not os.path.exists(obj):
        return text, 0
    und = set(undefined_symbols(obj))
    names = set()
    for m in re.finditer(r'(?<![\w&])_([A-Za-z]\w*)\s*(?:\+\s*(?:0x[0-9a-f]+|\d+)|\[)',
                         text):
        if m.group(1) in und:
            names.add(m.group(1))
    if not names:
        return text, 0

    total = 0
    for sym in sorted(names):
        gname = '_' + sym
        byte_re = re.compile(r'(?<![\w&])' + gname + r'\s*\+\s*(0x[0-9a-f]+|\d+)\b')
        elem_re = re.compile(r'(?<![\w&])' + gname + r'\s*\[\s*(0x[0-9a-f]+|\d+)\s*\]')
        star_re = re.compile(r'\*\s*' + gname + r'\b(?!\s*[\[\(])')
        any_re = re.compile(r'(?<![\w])' + gname + r'\b')
        if not (byte_re.search(text) and (elem_re.search(text) or star_re.search(text))):
            continue                       # no contradiction; nothing to resolve
        disp = extern_load_displacements(obj, sym)
        if not disp:
            continue

        plan, ok = [], True
        for fn, region in _split_definitions(text):
            if not any_re.search(region):
                continue
            # every use must be one of the three shapes, or the anchor is
            # load-bearing in a way this cannot see
            accounted = (len(byte_re.findall(region)) + len(elem_re.findall(region))
                         + len(star_re.findall(region)))
            if accounted != len(any_re.findall(region)):
                ok = False
                break
            code = disp.get(fn) or next(
                (v for k, v in disp.items() if _demangled_short(k) == fn), None)
            if not code:
                ok = False
                break
            bytes_ = {int(x, 0) for x in byte_re.findall(region)}
            elems = {int(x, 0) for x in elem_re.findall(region)}
            if star_re.search(region):
                elems.add(0)
            if not elems:
                # No element use, so there is no scale to infer and every one
                # would satisfy the equation — asking for a UNIQUE scale here
                # rejects the function and, with it, the whole name. That is
                # what made the first version of this rule decline on the one
                # object it exists for, silently, while passing every negative
                # control: a rule that never fires passes them all.
                if bytes_ != code:
                    ok = False
                    break
                plan.append((fn, 1))
                continue
            scales = [s for s in sorted(_ELEM_WIDTH_TYPE)
                      if bytes_ | {e * s for e in elems} == code]
            if len(scales) != 1:
                ok = False
                break
            plan.append((fn, scales[0]))
        if not ok or len({s for _f, s in plan}) < 2:
            continue                       # not a contradiction after all

        out = []
        for fn, region in _split_definitions(text):
            s = dict(plan).get(fn)
            if s is None:
                out.append(region)
                continue
            typ = _ELEM_WIDTH_TYPE[s]
            # BYTE FORM FIRST. The element rewrites emit `(char *)_X + K`,
            # which is itself a byte-form site; running byte_re afterwards
            # matches its own siblings' output and emits `(char *)(char *)_X`.
            region = byte_re.sub(lambda m: '(char *)%s + %s' % (gname, m.group(1)),
                                 region)
            region = elem_re.sub(
                lambda m: '*(%s *)((char *)%s + %#x)' % (typ, gname,
                                                         int(m.group(1), 0) * s),
                region)
            region = star_re.sub('*(%s *)((char *)%s + 0)' % (typ, gname), region)
            out.append(region)
        text = ''.join(out)
        total += 1
    return text, total


def _demangled_short(name):
    d = subprocess.run(['c++filt', name], capture_output=True, text=True).stdout
    return re.sub(r'.*::', '', d.split('(')[0]).strip()


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


def _sub_in_code(pattern, repl, text):
    r"""Substitute outside string literals and comments.

    A file-wide rename cannot use a plain `re.sub`, and the reason is not
    hypothetical. `gDebugDataFile` is declared

        int kd_gDebugDataFile KD_MANGLED("gDebugDataFile");

    and the payload of KD_MANGLED is the ELF symbol this unit EXPORTS. Renaming
    it there renamed the export: `keaRbdCore_unified` — validated, bit-identical
    on all three scenes — started emitting `kd_gDebug`, `kd_gDebugDataFile` and
    `kd_gPartition` in place of the three names the shipped object exports, and
    `check_symbol_bindings` reported zero interface errors while it did.
    """
    out, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        if c == '"' or c == "'":
            j = i + 1
            while j < n:
                if text[j] == '\\':
                    j += 2
                    continue
                if text[j] == c:
                    j += 1
                    break
                j += 1
            out.append(text[i:j])
            i = j
        elif text.startswith('/*', i):
            j = text.find('*/', i + 2)
            j = n if j < 0 else j + 2
            out.append(text[i:j])
            i = j
        else:
            j = i
            while j < n and text[j] not in '"\'' and not text.startswith('/*', j):
                j += 1
            out.append(re.sub(pattern, repl, text[i:j]))
            i = j
    return ''.join(out)


def fix_exported_data_name(text, diag, ctx):
    """`gDebug` is defined in this unit as `kd_gDebug` with an asm label.

    A DATA symbol the object exports is emitted by gen_prelude under a `kd_`
    name so it cannot collide with a public header's declaration of the same
    thing, and the body still writes the bare name. For functions
    `apply_renames` closes exactly this gap; for data it was never closed, and
    `keaRbdCore_unified` fails on three of them — `gDebug`, `gDebugDataFile`,
    `gPartition`.

    **THIS IS DEAD END 10 AND THE CONDITION BELOW IS WHAT MAKES IT NOT ONE.**
    The first attempt renamed every exported data symbol in every body and cost
    `McdGjk` — released, on the busiest pair in the census — and `MeMessage`,
    because a body that CALLS through an exported pointer resolves the bare name
    through the public header, and `float kd_MeInfoShow[1]` is not callable. The
    guessed `float[n]` was the whole problem: a .bss symbol is all zeros, so its
    bytes say nothing about its type.

    So the rename is allowed only where the definition is NOT a guess — where
    the object's own DWARF gave the symbol a type whose size matches its
    symbol-table entry, which is the same test gen_prelude applies before
    emitting it. Under that condition the `kd_` name has the real type and every
    use, call and dereference included, means what it did before.

    And it is driven by the DIAGNOSTIC rather than applied wholesale, which
    settles the other half of dead end 10 by construction: a name the public
    headers already declare never comes back undeclared, so this never sees it.
    Note the ordering in REPAIR_RULES — `fix_mislabelled_external` owns
    `‘_name’ undeclared` and matches first; only a name with no leading
    underscore reaches here.

    IT IS FILE-WIDE FOR THE ONE NAME, and per-line it could not converge. GCC
    reports an undeclared identifier ONCE PER FUNCTION, and `frameTime` appears
    twenty-odd times across MeProfile's, so a line-at-a-time rename needs one
    compile per occurrence and the repair loop runs out first — which reads from
    the outside as a rule that fixed the first line and then declined. The
    mapping is a property of the TRANSLATION UNIT, not of a line: `declared_as`
    says this unit declares that ELF symbol under that identifier, and
    `_dwarf_typed_export` says the definition has the right type. Both are
    checked before a single character is rewritten, and neither can be true of
    one occurrence and false of another."""
    m = re.search(r'[‘\'"](\w+)[’\'"] undeclared', diag)
    if not m:
        return None
    name = m.group(1)
    new = ctx.declared_as.get(name)
    if not new or new == name or not ctx.obj:
        return None
    if not _dwarf_typed_export(ctx.obj, name, new, ctx.declared):
        return None
    out = _sub_in_code(r'(?<![\w.>])' + re.escape(name) + r'\b', new, text)
    out = _rewrite_subfields_of(out, new)
    return out if out != text else None


def _rewrite_subfields_of(text, name):
    r"""`kd_frameTime.cpuCycles._4_4_` -> the four bytes at offset 4.

    THE RENAME UNMASKS THESE, and without rewriting them here it is rejected.
    While `frameTime` was undeclared GCC reported that and said nothing about
    the member accesses on it; renaming it to the identifier this unit really
    declares makes six `request for member '_4_4_'` errors appear at once, so
    the edit costs +5 in a single step and the repair loop reverts it — which is
    the loop working correctly and is indistinguishable, from outside, from a
    rule that declined. The follow-on repair that would take the count to -1
    never gets a turn.

    So the rule performs it. `fix_subfield_access` would do exactly this and the
    transformation is a transcription — the offset and the width are both spelled
    out in the name — but it is applied ONLY to expressions containing the
    renamed identifier, so nothing else in the file moves."""
    out = []
    for line in text.split('\n'):
        while True:
            hit = None
            for m in SUBFIELD.finditer(line):
                width = SUBFIELD_WIDTH.get(int(m.group(2)))
                if width is None:
                    continue
                start = scan_postfix_backward(line, m.start())
                if start is None:
                    continue
                expr = line[start:m.start()].strip()
                if not expr or not re.search(r'(?<![\w])' + re.escape(name) + r'\b', expr):
                    continue
                hit = (start, m.end(), expr, width, int(m.group(1)))
                break
            if hit is None:
                break
            start, end, expr, width, off = hit
            line = (line[:start] + f'(*({width} *)((char *)&({expr}) + {off}))'
                    + line[end:])
        out.append(line)
    return '\n'.join(out)


fix_exported_data_name.file_wide = True


_export_typed_cache = {}


def _dwarf_typed_export(obj, name, ident, declared):
    """Is `ident` DEFINED at the type this object's DWARF gives `name`?

    Not "does the DWARF have a type for it" — that question is the one that
    made this rule take McduDebugDraw backwards. `boxDraw` is a `.data` export,
    so gen_prelude renders it byte-exactly as `void *kd_boxDraw[72]` while the
    DWARF says `MeReal boxDraw[12][2][3]`; the DWARF answer was yes, the
    definition in the file was something else, and the body's `boxDraw[0][0]`
    became an index into a `void *`.

    So the two are compared. The declarator is rendered under the emitted name
    and looked for in the text this unit is about to emit; `struct`/`union`
    keywords are dropped from both sides first, because a `typedef struct {…} X`
    is spelled `X` in the headers and `struct X` in gcc 3.2's DWARF and they are
    the same type (gen_prelude.anonymous_struct_typedefs has the evidence).

    The size check behind the declarator is not decoration: a declaration that
    disagrees with the shipped object about how big a symbol is does not fail to
    compile, it reads or writes the wrong number of bytes at link time."""
    key = (obj, name, ident)
    if key in _export_typed_cache:
        return _export_typed_cache[key]
    _export_typed_cache[key] = False
    size = None
    for row in subprocess.run(['readelf', '-sW', obj], capture_output=True,
                              text=True).stdout.splitlines():
        p = row.split()
        if len(p) >= 8 and p[3] == 'OBJECT' and p[6] != 'UND' and p[7] == name:
            size = int(p[2])
            break
    if size is None:
        return False
    dies = dwarf_structs.parse(obj)
    for die in dies.values():
        if die['tag'] != 'DW_TAG_variable':
            continue
        if die['attrs'].get('DW_AT_name') != name:
            continue
        ref = dwarf_structs.REF_RE.search(die['attrs'].get('DW_AT_type', ''))
        if not ref:
            break
        r = int(ref.group(1), 16)
        if dwarf_structs.type_size(dies, r) != size:
            break
        want = _tagless(dwarf_structs.declarator(dies, r, ident))
        _export_typed_cache[key] = want in _tagless(declared)
        break
    return _export_typed_cache[key]


def _tagless(text):
    return re.sub(r'\s+', ' ', re.sub(r'\b(?:struct|union)\s+', '', text))


def not_code(name, obj, body='', known=None):
    """Did Ghidra make a "function" out of bytes that are not code?

    Three ways to know, and all three are facts about the object file rather
    than judgements about the decompilation.

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

    THREE — THE ADDRESS IS *INSIDE* A FUNCTION THAT IS ALREADY DECOMPILED.
    Ghidra sometimes cannot follow the control flow of a large function and
    slices interior blocks off as separate "functions". They are real code, but
    they are the SAME code the parent already carries, entered mid-frame — which
    is why they read `unaff_EBP + -0x25c`: `%ebp` was set by the parent's
    prologue, so from the fragment's point of view the frame base arrives in a
    register nobody assigned. `initialise_unmodelled_locals` then honestly sets
    it to 0 and the fragment dereferences address -604.

    Nothing calls them — they are `static` and unreferenced, so gcc discards
    them and the emitted object never depended on them, which is exactly why
    `prove_inert` reported MdtBcl's four `unaff_` values inert. Dropping them is
    therefore byte-identical by construction AND removes 585 of the 1,215 arm64
    pointer-truncation diagnostics left after `tools/fix_ptrwidth.py`.

    Read from the symbol table, not judged: five `FUN_` addresses in MdtBcl fall
    strictly inside `MdtBclAddRPROJoint`, `LimitSingleAxis` and
    `MdtContactWriteRow`, all three of which are decompiled under their own
    names. A fragment whose parent is NOT decompiled is KEPT — dropping it would
    lose the only copy — and one that starts exactly at a function's entry is
    not a fragment at all, it is that function under a different name.

    Those diagnostics are the useful signal and must NOT be answered by defining
    `undefined6`, `SCARRY1`, `POPCOUNT` and `halt_baddata` in kd_compat.h —
    that would make the noise compile and publish bogus symbols. Dropping loses
    nothing, because there is nothing there.

    Only `FUN_`-named functions are eligible for the address tests, since a
    symbol-named function is at an address the symbol table vouches for.
    Corpus-wide all three tests fire on one object between them: MdtBcl."""
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
    addr = int(m.group(1), 16)
    if not (text[0][2] <= addr < text[0][3]):
        return True
    if known:
        # Ghidra lays .text out at text[0][2]; function_extents is file-relative.
        at = addr - text[0][2]
        for fn, lo, hi in function_extents(obj):
            if lo < at < hi and fn in known:
                return True
    return False


def _split_definitions(text):
    """[(function or None, text)] — the file cut at ghidra_clean's own banners."""
    parts, last, name = [], 0, None
    for m in DEF_BANNER.finditer(text):
        parts.append((name, text[last:m.start()]))
        name, last = m.group(1), m.start()
    parts.append((name, text[last:]))
    return parts


def _resolve_external_per_site(region, ghidra_name, named, ctx, call):
    """One Ghidra name, two API slots — decide each CALL SITE by its argument.

    `McdTerm` calls `MePoolAPI.destroy(MePool *)` twice and
    `MeMemoryAPI.destroy(void *)` four times, and Ghidra spells both
    `_McdGeometryGetReferenceCount` because `MePoolAPI+4` and `MeMemoryAPI+12`
    are the same address in its invented memory map. `relocation_targets`
    correctly offers BOTH candidates and `_resolve_external` correctly declines,
    which left thirteen symbols of the drop-in gap (§3c) on one ambiguity.

    The argument settles it, and both halves of the comparison are READ:

      * the slot's first parameter type comes from the public header —
        `MePoolAPI.destroy` takes `MePool`, `MeMemoryAPI.destroy` takes `void`;
      * the argument's type comes from the object's own DWARF: `&frame->cachePool`
        is a `MePool *` because `cachePool` is an `MePool` member of the struct
        `frame` points at.

    A site whose argument this cannot type, or that matches neither candidate
    and has no `void`-taking fallback, is left alone — and leaving ONE site
    alone leaves the whole object in review, which is the right outcome: half a
    resolution is not a resolution."""
    params = ctx.api_params
    if not params:
        return None
    sig = signature_of(region)
    dies = dwarf_structs.parse(ctx.obj) if ctx.obj else {}
    want = {}
    for sym, _addend, member in named:
        typ = ctx.extern_types.get(sym)
        p = params.get((typ, member)) or params.get(('_' + str(typ), member))
        if p is None:
            return None                    # cannot type one of the candidates
        want[p] = (sym, member)

    out, last, n = [], 0, 0
    for m in call.finditer(region):
        end = _match_bracket(region, m.end() - 1)
        if end is None:
            return None
        arg = _split_args(region[m.end():end - 1])
        arg = arg[0].strip() if arg else ''
        t = _arg_base_type(arg, region, sig, dies)
        pick = want.get(t) or (want.get('void') if t else None)
        if pick is None:
            return None                    # unresolved site; leave the object
        out.append(region[last:m.start()])
        out.append('(%s.%s)(' % pick)
        # Resume just after the call's OPENING paren. Resuming at the closing
        # one drops the argument, and gcc then says `too few arguments`, the
        # error count goes up, and the repair loop reverts the edit — so the
        # rule reads as if it declined.
        last = m.end()
        n += 1
    if not n:
        return None
    out.append(region[last:])
    new = ''.join(out)
    # Any REMAINING bare reference is a read of the slot, not a call, and this
    # cannot say which slot it is. Refuse rather than guess.
    if re.search(r'(?<![\w])' + re.escape(ghidra_name) + r'\b', new):
        return None
    return new


def _arg_base_type(arg, region, sig, dies):
    """`MePool` for `&frame->cachePool`, `void` for a plain `void *`. None if
    this cannot say."""
    a = arg.strip()
    if a.startswith('&'):
        m = re.fullmatch(r'&\s*(\w+)\s*->\s*(\w+)', a)
        if not m:
            return None
        # `McdFrameworkID frame` is a POINTER typedef with no `*`, so
        # _declared_pointee alone returns None and every `&frame->pool` site
        # went unresolved — which left the whole object in review even though
        # five of its eight sites had typed cleanly.
        owner = (_declared_pointee(region, sig, m.group(1))
                 or _declared_type_name(region, sig, m.group(1)))
        if owner:
            owner = _dwarf_struct_of_typedef(dies, owner)
        got = _dwarf_member(dies, owner, m.group(2)) if owner else None
        if not got:
            return None
        # `MePool cachePool` -> MePool; a pointer member is not this shape.
        decl = got[1].replace('struct ', '').strip()
        m2 = re.match(r'^(\w+)\s+\w+$', decl)
        return m2.group(1) if m2 else None
    return 'void'


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
        return _resolve_external_per_site(region, ghidra_name, named, ctx, call)
    if call.search(region):
        return None                        # a call with no prototype to give it
    zero = [s for s, a in sorted(candidates) if a == 0]
    if len(zero) == 1:
        return re.sub(r'(?<![\w])' + re.escape(ghidra_name) + r'\b', zero[0], region)
    if len(candidates) != 1:
        return None
    # A PLAIN READ OR WRITE AT A NON-ZERO ADDEND, which is what the docstring
    # above has always promised and what the addend-zero branch alone could not
    # deliver. `MeProfileStartHardwareTimer` fills a 24-byte exported object one
    # word at a time, and every store is the SAME relocation:
    #
    #   57: mov %edx,0x0     R_386_32 frameTime      addend 0
    #   5d: mov %ecx,0x4     R_386_32 frameTime      addend 4
    #   63: movl $0x0,0x8    R_386_32 frameTime      addend 8   ... to 0x14
    #
    # Ghidra names each by whichever EXTERNAL slot the addend landed in, so the
    # six come out as `frameTime`, `_select`, `_clockSpeed`, `_DAT_0001100c`,
    # `_DAT_00011010`, `_DAT_00011014` — five of them borrowed from unrelated
    # undefined symbols. relocation_targets already inverts that exactly; all
    # that was missing was somewhere to put the answer.
    #
    # The slot is four bytes wide by construction — that is how Ghidra's block
    # is laid out — so the access width is not a guess. Only reached when the
    # name is never CALLED, because a call needs a prototype and this gives it
    # none; that guard is above.
    sym, addend = next(iter(candidates))
    return re.sub(r'(?<![\w])' + re.escape(ghidra_name) + r'\b',
                  '(*(int *)((char *)&%s + %#x))' % (sym, addend), region)


def fix_variadic_extra_args(line, diag, ctx):
    """`open(filename, flags, mode, junk)` — the padding word gcc pushed.

    `MeOpenRaw`'s call site pushes FOUR words:

        2d: push %edx    2e: push %esi    32: push %ebx    33: push %edx
        34: call open

    and the stack reads `open(filename, ebx, esi, edx)`. `open` is variadic, so
    the callee reads two or three and the fourth is never looked at — it is
    stack padding gcc left, and on the read-only paths `%esi` is not even
    assigned. Ghidra recovers all four honestly, which is right about the
    machine and wrong about the program.

    NOTHING COULD SEE IT UNTIL NOW. C permits any number of arguments to a
    variadic function, so the only complaint comes from glibc's FORTIFIED
    `open`, which raises its error inside `fcntl2.h` — a header, and
    compile_diags dropped header diagnostics by design. It is reachable because
    GCC also prints `inlined from … at <our file>:76`, which compile_diags now
    honours.

    THE ARITY IS TAKEN FROM THE DIAGNOSTIC, not assumed: glibc says "open can be
    called either with 2 or 3 arguments, not more", so the cap is the largest
    number in that sentence. A message this cannot read is declined."""
    m = re.search(r'[‘\'"](\w+)[’\'"] declared with attribute error:\s*'
                  r'(?P<fn>\w+) can be called either with '
                  r'(?P<lo>\d+) or (?P<hi>\d+) arguments, not more', diag)
    if not m:
        return None
    callee, cap = m.group('fn'), int(m.group('hi'))
    i = line.find(callee + '(')
    if i < 0:
        return None
    open_paren = i + len(callee)
    end = _match_bracket(line, open_paren)
    if end is None:
        return None
    args = _split_args(line[open_paren + 1:end - 1])
    if len(args) <= cap:
        return None
    return line[:open_paren + 1] + ','.join(a.strip() for a in args[:cap]) \
        + line[end - 1:]


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


def _skip_literal(s, i):
    """Index just past the string or character literal starting at `s[i]`.

    A comma inside a literal is not an argument separator, and until this
    existed both splitters believed it was:

        sprintf(file->error,"line %d, char %d: expected %d MeReals, found %d\n",
                iVar1,iVar2,action->max,uVar8,puVar6,puVar6)

    came back as ten arguments with the format in three pieces, so
    fix_printf_extra_args saw something that was not a literal and declined —
    silently, and for every format in the corpus that contains a comma. That is
    most of them. The rule looked right and did nothing."""
    q, j = s[i], i + 1
    while j < len(s):
        if s[j] == '\\':
            j += 2
            continue
        if s[j] == q:
            return j + 1
        j += 1
    return len(s)


def _split_arguments(s):
    """Top-level comma split, ignoring commas inside brackets and literals."""
    out, depth, start, i = [], 0, 0, 0
    while i < len(s):
        ch = s[i]
        if ch in '"\'':
            i = _skip_literal(s, i)
            continue
        if ch in _OPEN:
            depth += 1
        elif ch in _CLOSE:
            depth -= 1
        elif ch == ',' and depth == 0:
            out.append(s[start:i])
            start = i + 1
        i += 1
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
# The SECOND shape of the same thing, and the one the solver DRIVER uses. Here
# the source is not a typed pointer walked field by field but the raw incoming
# argument area, advanced in the for-header's COMMA operator:
#
#     puVar15 = (undefined4 *)&stack0xfffffdd8;
#     puVar12 = (undefined4 *)register0x00000010;
#     for (i = 0x17; puVar12 = puVar12 + 1, i != 0; i = i + -1) {
#       *puVar15 = *puVar12;
#       puVar15 = puVar15 + 1;
#     }
#
# `register0x00000010` is x86 register-space offset 0x10 — the ENTRY value of
# ESP — so the `+ 1` word the loop advances before its first read is the return
# address plus four, i.e. `ebp+8`, i.e. the first stack argument. That is the
# cdecl ABI and not a guess; the shipped prologue says it outright with
# `lea 0x8(%ebp),%esi`.
REG_MARSHAL_LOOP = re.compile(
    r'^(?P<ind>[ 	]*)(?P<dst>\w+) = \((?P<ety>[A-Za-z_]\w*) \*\)'
    r'&(?P<spell>stack0x|in_stack_)(?P<off>[0-9a-f]{8});\n'
    r'(?P=ind)(?P<sv>\w+) = \([A-Za-z_]\w* \*\)register0x00000010;\n'
    r'(?P<between>(?:(?P=ind)\w+ = [\w.>\[\]-]+;\n){0,4})'
    r'(?P=ind)for \((?P<iv>\w+) = (?P<cnt>0x[0-9a-f]+|\d+); '
    r'(?P=sv) = (?P=sv) \+ 1, (?P=iv) != 0; (?P=iv) = (?P=iv) \+ -1\) \{\n'
    r'(?P=ind)  \*(?P=dst) = \*(?P=sv);\n'
    r'(?P=ind)  (?P=dst) = (?P=dst) \+ 1;\n'
    r'(?P=ind)\}\n', re.M)


def _enclosing_call(region, pos):
    r"""(callee, argument list) for the call whose arguments contain `pos`.

    Ghidra WRAPS a long call — the name on one line and `(args…)` on the next —
    so a regex anchored on `\bname\(` with a newline-free argument list finds
    nothing and the size check that guards the whole collapse silently never
    runs. That is what kept keaRbdCore_unified, the solver driver, out of the
    build after everything else about it was solved. Walking out from the
    argument instead is immune to the wrapping and to an inner call, which a
    left-to-right regex is not."""
    depth, i = 0, pos
    while i > 0:
        c = region[i]
        if c == ';':
            return None
        if c == ')':
            depth += 1
        elif c == '(':
            if depth == 0:
                end = _match_bracket(region, i)
                m = re.search(r'(\w+)[ \t\n]*$', region[:i])
                if end is None:
                    return None
                if m:
                    return m.group(1), region[i + 1:end - 1]
                # No identifier before the `(` — the callee is an EXPRESSION,
                # which in this corpus means a vtable dispatch. Hand the
                # expression back and let the caller resolve it; returning None
                # here declines every by-value aggregate passed through a
                # virtual call, and keaRbdCore_unified passes one.
                if i and region[i - 1] == ')':
                    o = _match_bracket_back(region, i - 1)
                    if o is not None:
                        return region[o:i], region[i + 1:end - 1]
                return None
            depth -= 1
        i -= 1
    return None


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


# `(**(code **)(_vanillaFunctions + 0x14))` and, once fix_vptr_store has run,
# `(**(code **)((*(char **)&vanillaFunctions) + 0x14))`. Both name a LOCAL whose
# class fixes the vtable and an offset that fixes the slot.
_VT_DISPATCH = re.compile(
    r'^\(\*\*\(code \*\*\)\(?'
    r'(?:\(\*\(\w+ \*\*\)&(?P<loc1>\w+)\)|_(?P<loc2>\w+))'
    r'(?:[ \t]*\+[ \t]*(?P<off>0x[0-9a-f]+|\d+))?\)?\)$')


def _vtable_dispatch_callee(expr, region, corpus):
    """The mangled symbol a virtual call through a local's vptr reaches.

    The slot table is read from the relocations in the object that DEFINES the
    vtable — vtable_slots.slot_table — so this is the same evidence
    fix_vptr_store rests on and not a second guess at it. Returns None for
    anything that is not this exact shape, which is the only kind of indirect
    call in this corpus whose target is knowable."""
    m = _VT_DISPATCH.match(expr.strip())
    if not m:
        return None
    local = m.group('loc1') or m.group('loc2')
    d = re.search(r'^[ \t]*([A-Za-z_]\w*)[ \t]+' + re.escape(local) + r'[ \t]*;[ \t]*$',
                  region, re.M)
    if not d:
        return None
    table = vtable_slots.slot_table(corpus, d.group(1))
    return table.get(int(m.group('off'), 0) if m.group('off') else 0)


# The INCOMING mirror of REG_MARSHAL_LOOP: gcc 3.2 copies a by-value aggregate
# PARAMETER into a local before using it, and Ghidra renders that as a field
# walk out of the raw argument area.
#
#     pMVar16 = &constraints;
#     piVar13 = (int *)register0x00000010;
#     for (i = 0x17; piVar13 = piVar13 + 1, i != 0; i = i + -1) {
#       pMVar16->num_partitions = *piVar13;
#       pMVar16 = (MdtKeaConstraints *)&pMVar16->max_partitions;
#     }
#
# is `constraints = pconstraints;`, and the shipped code says so in one
# instruction: `lea -0xa8(%ebp),%edi; mov $0x17,%ecx; lea 0x8(%ebp),%esi;
# rep movsl`.
INCOMING_ARG_COPY = re.compile(
    r'^(?P<ind>[ \t]*)(?P<dp>\w+) = &(?P<dst>\w+);\n'
    r'(?P=ind)(?P<sv>\w+) = \((?P<ety>[A-Za-z_]\w*) \*\)register0x00000010;\n'
    r'(?P<between>(?:(?P=ind)\w+ = [\w.>\[\]-]+;\n){0,4})'
    r'(?P=ind)for \((?P<iv>\w+) = (?P<cnt>0x[0-9a-f]+|\d+); '
    r'(?P=sv) = (?P=sv) \+ 1, (?P=iv) != 0; (?P=iv) = (?P=iv) \+ -1\) \{\n'
    r'(?P=ind)  (?P=dp)->\w+ = \*(?P=sv);\n'
    r'(?P=ind)  (?P=dp) = \([^)]*\)&(?P=dp)->\w+;\n'
    r'(?P=ind)\}\n', re.M)


def _collapse_incoming_arg_copy(region, first_param, obj):
    """`constraints = pconstraints;` — the assignment the copy loop spells out.

    Three things must agree before the loop is replaced, and together they make
    the assignment type-correct by construction rather than by inspection:

      * the destination is a LOCAL declared in this function, at some type T;
      * the function's first stack parameter is declared at the SAME type T —
        which is what the cdecl ABI says `register0x00000010 + 1` points at;
      * T's size in this object's DWARF is exactly what the loop copies.

    A loop that copies part of an aggregate, or into something of another type,
    fails all three and is left alone."""
    if not (first_param and obj):
        return region, 0
    ptype, pname = first_param
    n = 0

    def rewrite(m):
        nonlocal n
        elem = _FIXED_WIDTH.get(m.group('ety'))
        if elem is None:
            return m.group(0)
        nbytes = int(m.group('cnt'), 0) * elem
        d = re.search(r'^[ \t]*([A-Za-z_]\w*)[ \t]+' + re.escape(m.group('dst'))
                      + r'[ \t]*;[ \t]*$', region, re.M)
        if not d or d.group(1) != ptype or _dwarf_type_size(obj, ptype) != nbytes:
            return m.group(0)
        n += 1
        return '%s%s = %s;\n%s' % (m.group('ind'), m.group('dst'), pname,
                                   m.group('between'))

    return INCOMING_ARG_COPY.sub(rewrite, region), n


def _first_stack_parameter(body):
    """(type, name) of the function's first parameter, or None.

    Only a BY-VALUE aggregate is returned — no `*`, no array, no scalar the
    fixed-width table knows — because that is the only thing the caller may pass
    in place of a copy loop, and because on cdecl it is the only shape whose
    address is the incoming argument area itself.
    """
    sig = signature_of(body)
    if not sig:
        return None
    m = re.search(r'\(([^()]*)\)\s*$', sig.replace('\n', ' ').strip())
    if not m:
        return None
    first = _split_args(m.group(1))[0].strip()
    d = re.fullmatch(r'([A-Za-z_]\w*)[ \t]+([A-Za-z_]\w*)', first)
    if not d or d.group(1) in _FIXED_WIDTH:
        return None
    return d.group(1), d.group(2)


def collapse_outgoing_aggregate_copy(body, protos=None, obj=None, declared=''):
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
    region, n = _collapse_incoming_arg_copy(body, _first_stack_parameter(body), obj)
    body = region
    loops = [(m, False) for m in MARSHAL_LOOP.finditer(region)]
    loops += [(m, True) for m in REG_MARSHAL_LOOP.finditer(region)]
    if not loops:
        return body, n
    first_param = _first_stack_parameter(body)
    # kd_protos*.h keys a C++ function on its DWARF name and on its linkage
    # name (§5b); the BODY calls it by the flattened `Class__method` spelling
    # gen_prelude declares. The asm label is the only thing the two share, so
    # the prototype lookup goes through it rather than through a name rule —
    # `CxSmallSort::_Update` flattens to `CxSmallSort___Update` and no rsplit
    # gets back to it.
    sym_of = {c: sym for sym, c in declared_under(declared).items()}

    # PLAN first, against the untouched body, then apply once. Blanking a loop
    # shortens the text, so a second pass over match offsets taken before it
    # would read the wrong places.
    plan = []
    for loop, from_args in loops:
        elem = _FIXED_WIDTH.get(loop.group('ety'))
        if elem is None:
            continue
        nbytes = int(loop.group('cnt'), 0) * elem
        if from_args:
            # The source is `register0x00000010 + 1` word, which the cdecl ABI
            # pins to the first stack parameter — see REG_MARSHAL_LOOP. Its
            # DECLARED TYPE must be exactly the size the loop copies, which is a
            # second and independent confirmation to the call-site size check
            # below: one says the destination is that big, this says the source
            # is. A signature this cannot read declines rather than guesses.
            base = None
            if first_param and obj:
                ptype, pname = first_param
                if _dwarf_type_size(obj, ptype) == nbytes:
                    base = pname
        else:
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
            # Every occurrence, not the first: the first is the DECLARATION
            # (`kd_agg92 kVar1;`), which is in no call at all.
            call = next((c for c in (
                _enclosing_call(region, u.start()) for u in
                re.finditer(r'(?<![\w.>])' + re.escape(agg) + r'\b(?![\w.])', region))
                if c), None)
            if not call:
                continue
            # THE CHECK THAT MAKES THIS SAFE: the bytes the loop copies must be
            # the bytes the callee's parameter is.
            callee = call[0]
            if not re.fullmatch(r'\w+', callee):
                callee = _vtable_dispatch_callee(
                    callee, region,
                    os.path.dirname(os.path.abspath(obj))) if obj else None
                if not callee:
                    continue
            elif _proto_param_size(callee, _arg_index(call[1], agg),
                                   protos) is None:
                callee = sym_of.get(callee, callee)
            if _proto_param_size(callee, _arg_index(call[1], agg),
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
    for _pat in (MARSHAL_LOOP, REG_MARSHAL_LOOP):
        region = _pat.sub(
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
    args, depth, cur, i = [], 0, '', 0
    while i < len(s):
        ch = s[i]
        if ch in '"\'':                       # a comma in a literal is not a comma
            j = _skip_literal(s, i)
            cur += s[i:j]
            i = j
            continue
        if ch in '([':
            depth += 1
        elif ch in ')]':
            depth -= 1
        if ch == ',' and depth == 0:
            args.append(cur); cur = ''
        else:
            cur += ch
        i += 1
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
    (re.compile(r'can be called either with \d+ or \d+ arguments, not more'),
     fix_variadic_extra_args),
    (re.compile(r'too many arguments to function'),
     fix_too_many_arguments),
    (re.compile(r'incompatible types when assigning to type .* from type '
                r'[‘\'"]int[’\'"]'),
     fix_int_store_to_aggregate),
    (re.compile(r'invalid use of void expression'),
     fix_void_assignment),
    (re.compile(r'called object is not a function or function pointer'),
     fix_call_through_void_ptr),
    # LAST, and the position is the whole mechanism. REPAIR_RULES takes the
    # FIRST pattern that matches, and this one is a catch-all for `‘X’
    # undeclared` — put anywhere earlier it shadows both the rules above that
    # answer a NARROWER form of the same diagnostic, `‘_name’` and
    # `‘stack0xNNNN’`. Placed before fix_stack_address_name it silently took
    # keaIntegrate_pc back out of the build, which is what a shadowed rule looks
    # like from the outside: a rule that declines.
    (re.compile(r'[‘\'"]\w+[’\'"] undeclared'),
     fix_exported_data_name),
]


class RepairContext:
    """What the rules are allowed to consult. Everything here is READ from the
    object, the public headers or the DWARF-derived type database — nothing in
    it is inferred from the decompiled text."""

    def __init__(self, obj=None, fieldmap=None, include_dir=None, dump=None,
                 declared=''):
        self.obj = obj
        self.fieldmap = fieldmap or {}
        self.externals = relocation_targets(obj, per_function=True) if obj else {}
        self.extern_types = extern_var_types(include_dir)
        self.api_params = api_member_param_types(include_dir)
        # {ELF symbol: the C identifier this unit defines it as} — read from the
        # prelude and exports this unit is about to emit, keyed on the asm label.
        self.declared = declared
        self.declared_as = declared_under(declared)
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

    # The prelude and the exports are read here rather than beside their use
    # below, because collapse_outgoing_aggregate_copy runs INSIDE the loop that
    # follows and needs the asm labels to look a callee's prototype up.
    _declared = ''
    for _extra in (args.prelude, args.vtables, args.exports):
        if _extra and os.path.exists(_extra):
            _declared += open(_extra, errors='ignore').read()

    decls, defs, dropped = [], [], []
    n_alloca_fns = 0
    n_alloca_slots = 0
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
        if not_code(name, args.object, body, set(by_name)):
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
        # Immediately after, because it works on the `kd_alloca_*` names that
        # pass creates, and before anything reads the slot expressions.
        body, nslot = materialise_alloca_relative_slots(body, name)
        n_alloca_slots += nslot
        body, nproto = prototype_indirect_calls(body)
        n_proto_calls += nproto
        body, nagg = collapse_outgoing_aggregate_copy(body, args.protos,
                                                       args.object, _declared)
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
    body_text, n_vptr = fix_vptr_store(args.object, body_text, _declared,
                                       read_ghidra_locals(args.input), args.protos)
    # AFTER fix_vptr_store, and the order is the whole of why this is safe.
    # `_vanillaQMatrix` is the corpus's other two-typings site and it is not an
    # external at all — fix_vptr_store resolves it to a local, so that spelling
    # is gone before this looks. keaLCPSolver is validated and bit-identical on
    # three scenes and must not change.
    body_text, n_extidx = normalise_external_indexing(args.object, body_text)
    body_text, n_pfmt = fix_printf_extra_args(body_text)
    body_text, n_cmac = fix_compat_macro_extra_args(body_text)
    body_text, n_uscore = fix_undeclared_underscore_local(body_text)
    body_text, n_uninit = initialise_unmodelled_locals(body_text)
    body_text, n_mangled = resolve_mangled_call_names(
        body_text, _declared + '\n' + '\n'.join(decls))
    body_text, n_voidmem = fix_void_pointer_member(args.object, body_text, _declared)
    body_text, n_intstore = fix_int_store_of_float(args.object, body_text)
    body_text, n_fltload = fix_float_load_of_int(args.object, body_text)
    body_text, n_frame = materialise_shifted_frame(
        body_text, args.object, read_ghidra_locals(args.input))
    # After materialise_shifted_frame, because the outgoing-argument area it
    # gives real storage to is spelled `kd_argslot_*` in its output and the
    # padding slots sit in exactly that area.
    # BEFORE drop_padding_arg_stores, and the order is the mechanism: with no
    # arguments in the call there is no top argument slot, so the padding rule
    # has nothing to be above. Giving the call its arguments back makes the two
    # words over them derivable, and MstUtils' pair falls out for free.
    body_text, n_parea = materialise_pointer_arg_area(body_text)
    body_text, n_ind = restore_indirect_call_args(
        body_text, fieldmap, args.metoolkit_include)
    body_text, n_pad = drop_padding_arg_stores(body_text)
    body_text, n_ftext = restore_float_text(
        body_text, args.object, read_ghidra_locals(args.input))
    data_block, n_data = materialise_data_refs(args.object, body_text)
    abs_block, n_abs = resolve_absolute_data_refs(args.object, body_text)
    data_block = abs_block + data_block
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
    rel_block, n_rel = materialise_relocated_data(args.object, body_text,
                                                 renames, _declared)

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
    if n_abs:
        print(f'  {n_abs} absolute address(es) below any section resolved')
    if n_ftype:
        print(f'  {n_ftype} function type(s) declared from the name Ghidra gave them')
    if n_ptr:
        print(f'  {n_ptr} pointer slot(s) resolved from Ghidra addresses')
    if n_extbase:
        print(f'  {n_extbase} external base(s) re-resolved to byte arithmetic')
    if n_vptr:
        print(f'  {n_vptr} vptr store(s) resolved to a vtable address point')
    if n_extidx:
        print(f'  {n_extidx} external symbol(s) typed two ways normalised to '
              'byte arithmetic, scale read from the machine code')
    if n_mangled:
        print(f'  {n_mangled} mangled call site(s) resolved to a declared name')
    if n_voidmem:
        print(f'  {n_voidmem} member access(es) on a void * resolved to the pointer')
    if n_intstore:
        print(f'  {n_intstore} float store(s) rendered as an int conversion')
    if n_frame:
        print(f'  {n_frame} alloca-shifted frame(s) given real storage')
    if n_ftext:
        print(f'  {n_ftext} float expression site(s) repaired (spill rounding / '
              f'add-chain association)')
    if n_uscore:
        print(f'  {n_uscore} undeclared `_local` reference(s) bound to their declaration')
    if n_fltload:
        print(f'  {n_fltload} int-to-float conversion(s) rewritten as a four-byte load')
    if n_uninit:
        print(f'  {n_uninit} unmodelled-register local(s) given a defined initial value')
    if n_pfmt:
        print(f'  {n_pfmt} padding word(s) dropped from variadic call(s)')
    if n_pad:
        print(f'  {n_pad} padding word(s) dropped from an outgoing argument area')
    if n_ind:
        print(f'  {n_ind} argument-less indirect call(s) given their arguments back')
    if n_parea:
        print(f'  {n_parea} outgoing argument area(s) anchored on a local given '
              f'real storage')
    if n_alloca_slots:
        print(f'  {n_alloca_slots} argument slot(s) below an alloca given real storage')
    if n_cmac:
        print(f'  {n_cmac} padding word(s) dropped from a kd_compat.h call')
    if n_rel:
        print(f'  {n_rel} relocated data block(s) rebuilt with their pointers')
    if args.cflag:
        ctx = RepairContext(args.object, fieldmap, args.metoolkit_include,
                            dump=args.input, declared=_declared)
        n_edits, left, _log = repair_loop(args.output, args.cc, args.cflag, ctx)
        if n_edits:
            print(f'  {n_edits} line(s) repaired from compiler feedback'
                  + (f', {left} error(s) left' if left else ', clean'))
    print(f'{args.output}: {len(defs)} functions '
          f'({sum(1 for d in decls if d.startswith("static"))} static)'
          + (f', dropped {dropped}' if dropped else ''))


if __name__ == '__main__':
    main()
