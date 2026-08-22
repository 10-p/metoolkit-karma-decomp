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
    ap.add_argument('--drop', action='append', default=[],
                    help='function to omit entirely (repeatable)')
    args = ap.parse_args()

    exported, internal, real_symbol_of = object_symbols(args.object)
    text = open(args.input, errors='ignore').read()

    # A Ghidra dump can contain the same name twice — typically a small import
    # thunk plus the real definition. Emitting both yields two incompatible
    # forward declarations and the translation unit stops compiling. Keep the
    # largest body, which is always the real one.
    by_name = {}
    for name, body in split_functions(text):
        if name not in by_name or len(body) > len(by_name[name]):
            by_name[name] = body

    decls, defs, dropped = [], [], []
    for name, body in by_name.items():
        if name in args.drop:
            dropped.append(name)
            continue
        body = strip_comments(body).strip('\n')
        body = cxx_names_to_c(ghidra_type_quirks(body))
        sig = signature_of(body)
        if sig is None:
            print(f'  ! skipping {name}: no body found', file=sys.stderr)
            continue
        # A name Ghidra shows unmangled that is NOT an exported symbol was
        # `static` in the original source. Keep it that way.
        is_static = name not in exported
        if is_static:
            decls.append(f'static {sig};')
            defs.append(f'/* ---- {name} (static) ---- */\nstatic {body}\n')
            continue
        newname, newsig = rename_exported(sig, name)
        if newname is None:                    # could not parse; emit as-is
            decls.append(f'{sig};')
            defs.append(f'/* ---- {name} (exported) ---- */\n{body}\n')
            continue
        # Use the REAL ELF symbol, which for C++ is the mangled form.
        symbol = real_symbol_of.get(name, name)
        decls.append(f'{newsig} KD_MANGLED("{symbol}");')
        # `sig` is whitespace-normalised, so it will not match the raw body.
        # Rewrite the declarator in place instead: the name is the identifier
        # immediately before the first '(' in the text preceding the body.
        brace = body.find('\n{')
        head, rest = body[:brace], body[brace:]
        decl = declarator_name(sig) or name
        head = re.sub(r'\b' + re.escape(decl) + r'\b(?=\s*\()',
                      f'kd_{decl}', head, count=1)
        body = head + rest
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

    print(f'{args.output}: {len(defs)} functions '
          f'({sum(1 for d in decls if d.startswith("static"))} static)'
          + (f', dropped {dropped}' if dropped else ''))


if __name__ == '__main__':
    main()
