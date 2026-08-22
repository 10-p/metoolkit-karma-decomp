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
    appear in several CUs."""
    return re.sub(r'\b([A-Za-z_]\w*)_conflict\b', r'\1', body)


ANON_CAST = re.compile(
    r'([A-Za-z_][\w.\->\[\]]*)\s*=\s*\*\(\s*anon_\w+\s*\*\)', re.S)
ANON_STORE = re.compile(
    r'\*\(\s*anon_\w+\s*\*\)(\([^;]*?\))\s*=\s*([A-Za-z_][\w.\->\[\]]*)')


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
    return body


def cxx_names_to_c(body):
    """`Foo::bar` is how Ghidra renders a C++ function-local static or member.
    C has no `::`; flatten it to a legal identifier. The declaration itself is
    supplied by the per-object prelude."""
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
    for m in re.finditer(r'(\w+)\s*=\s*-\(\(int\)(.+?)\s*\*\s*(0x[0-9a-f]+|\d+)'
                         r'\s*\+\s*0xfU?\s*&\s*0xfffffff0\)\s*;', body):
        neg[m.group(1)] = (m.group(2).strip(), m.group(3))

    n = 0
    if neg:
        def sub_alloca(m):
            nonlocal n
            var = m.group(3)
            if var not in neg:
                return m.group(0)
            expr, mult = neg[var]
            n += 1
            return f'{m.group(1)}((char *)alloca((size_t)({expr}) * {mult}))'
        # ONLY the defining use: the alloca'd pointer appearing as the RHS of an
        # assignment, i.e. `dest = (T)(&stack0xH + negVar);`. Other
        # `&stack0xH + negVar` sites are STORES into the shifted frame — Ghidra's
        # rendering of outgoing call arguments — and substituting an alloca there
        # would hand out a fresh block per store, which is nonsense. Leaving them
        # unresolved is deliberate: the object then fails to compile and
        # recover.py holds it back, which is the honest outcome.
        body = re.sub(r'(=\s*\([^()]*\)\s*)\(\s*&stack0x([0-9a-f]{8})\s*\+\s*(\w+)\s*\)',
                      sub_alloca, body)

    return body, n


def signature_of(body):
    """Extract the declarator text preceding the function's opening brace."""
    brace = body.find('\n{')
    if brace < 0:
        return None
    return ' '.join(body[:brace].split())


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
    ap.add_argument('--metoolkit-include',
                    help='metoolkit include/ root; functions the headers define as '
                         'MeINLINE are dropped, since the header already supplies them')
    ap.add_argument('--drop', action='append', default=[],
                    help='function to omit entirely (repeatable)')
    args = ap.parse_args()

    exported, internal, real_symbol_of = object_symbols(args.object)
    hdr_inlines = header_inline_names(args.metoolkit_include)
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
    n_inline_dropped = 0
    for name, body in by_name.items():
        if name in args.drop:
            dropped.append(name)
            continue
        if name in hdr_inlines and name not in exported:
            n_inline_dropped += 1
            continue
        body = strip_comments(body).strip('\n')
        body = resolve_anon_types(cxx_names_to_c(ghidra_type_quirks(body)))
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

    with open(args.output, 'w') as f:
        f.write('/* Generated by karma-decomp/tools/ghidra_clean.py — do not edit by hand.\n'
                f' * source object: {args.object}\n'
                ' */\n')
        f.write('#include "kd_compat.h"\n')
        f.write('#include "kd_karma.h"\n')
        f.write('#include "kd_types.h"\n')
        f.write('#include <stdbool.h>\n\n')
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
        if args.exports and os.path.exists(args.exports):
            f.write('/* ---- exported data symbols (need the declarations above) ---- */\n')
            f.write(open(args.exports).read())
            f.write('\n')
        f.write('\n'.join(defs))

    if n_inline_dropped:
        print(f'  {n_inline_dropped} header-inline function(s) dropped')
    if n_alloca_fns:
        print(f'  {n_alloca_fns} function(s) needed an alloca frame')
    print(f'{args.output}: {len(defs)} functions '
          f'({sum(1 for d in decls if d.startswith("static"))} static)'
          + (f', dropped {dropped}' if dropped else ''))


if __name__ == '__main__':
    main()
