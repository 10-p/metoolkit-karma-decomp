#!/usr/bin/env python3
"""
gen_protos.py — emit a clean C prototype header for Ghidra, built from DWARF.

Why this exists: Ghidra has no signature for a function an object merely
*imports*, so it guesses the arity from the call site — and gets it wrong. In
IxBoxTriList it decided McdModelGetGeometry takes no arguments and emitted the
pushed arguments as writes to unrelated stack variables:

    pMStack_25c = p->model1;
    fStack_260  = 9.18817e-41;        // a pointer misread as a denormal float
    boxgeom = (McdBoxID)McdModelGetGeometry();

That is a correctness problem, not a cosmetic one. Enabling "Decompiler
Parameter ID" does not fix it; only real prototypes do.

Feeding metoolkit's own headers to Ghidra's C parser fails — they are layered
with MEAPI/MEPUBLIC calling-convention macros and platform blocks it trips over.
But every function is DEFINED in some object, with a full DWARF signature. So we
generate the prototypes from that instead: same ground truth, no macros.

Types are deliberately SIMPLIFIED (every pointer becomes void *, integers become
the right-sized builtin). Ghidra only needs argument count and size to fix its
stack analysis, and simplified types parse without a single external dependency.
"""
import argparse
import glob
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dwarf_structs import parse, REF_RE  # noqa: E402

INT_BY_SIZE = {1: 'char', 2: 'short', 4: 'int', 8: 'long long'}


def simple_type(dies, ref, depth=0):
    """Collapse a DWARF type to something with the right size and class."""
    if ref is None or depth > 16:
        return 'void'
    d = dies.get(ref)
    if d is None:
        return 'void'
    tag, a = d['tag'], d['attrs']
    sub = a.get('DW_AT_type')
    subref = int(REF_RE.search(sub).group(1), 16) if sub and REF_RE.search(sub) else None

    if tag in ('DW_TAG_pointer_type', 'DW_TAG_array_type', 'DW_TAG_reference_type'):
        # Arrays decay in a parameter list, and a C++ reference is a pointer at
        # the ABI level — which is the only level Ghidra cares about here.
        return 'void *'
    if tag in ('DW_TAG_const_type', 'DW_TAG_volatile_type', 'DW_TAG_typedef'):
        return simple_type(dies, subref, depth + 1)
    if tag == 'DW_TAG_base_type':
        try:
            size = int(a.get('DW_AT_byte_size', 4))
        except ValueError:
            size = 4
        enc = a.get('DW_AT_encoding', '')
        if 'float' in enc:
            return 'float' if size == 4 else 'double'
        if 'boolean' in enc:
            return 'char'
        unsigned = 'unsigned' in enc
        base = INT_BY_SIZE.get(size, 'int')
        return ('unsigned ' + base) if unsigned and base != 'char' else base
    if tag in ('DW_TAG_structure_type', 'DW_TAG_class_type', 'DW_TAG_union_type'):
        return None                          # by-value aggregate: cannot simplify
    if tag == 'DW_TAG_enumeration_type':
        return 'int'
    return 'int'          # unknown: assume one 4-byte slot rather than nothing


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('objdirs', nargs='+')
    ap.add_argument('-o', '--output', required=True)
    ap.add_argument('--exclude', action='append', default=[])
    args = ap.parse_args()

    objs = []
    for d in args.objdirs:
        objs.extend(sorted(glob.glob(os.path.join(d, '**', '*.o'), recursive=True)))
    objs = [o for o in objs if not any(x in o for x in args.exclude)]

    protos, skipped = {}, []
    for obj in objs:
        try:
            dies = parse(obj)
        except Exception:                                       # noqa: BLE001
            continue
        for die in dies.values():
            if die['tag'] != 'DW_TAG_subprogram':
                continue
            a = die['attrs']
            name = a.get('DW_AT_name')
            # Externally visible, and not a bare class-member declaration.
            #
            # NOT filtered on DW_AT_low_pc: gcc 3.2 marks most of this API
            # "declared as inline but ignored", so the DIE carrying the
            # prototype is the abstract instance and has no low_pc at all. It
            # still has the return type and the formal parameters, which is all
            # we need.
            if not name or 'DW_AT_external' not in a:
                continue
            if 'DW_AT_declaration' in a and not any(
                    c['tag'] == 'DW_TAG_formal_parameter' for c in die['children']):
                continue
            if not name.replace('_', 'a').isalnum():
                continue
            if name in protos:
                continue
            rt = a.get('DW_AT_type')
            rref = int(REF_RE.search(rt).group(1), 16) if rt and REF_RE.search(rt) else None
            ret = simple_type(dies, rref)
            params, bad = [], False
            for c in die['children']:
                if c['tag'] != 'DW_TAG_formal_parameter':
                    continue
                pt = c['attrs'].get('DW_AT_type')
                pref = int(REF_RE.search(pt).group(1), 16) if pt and REF_RE.search(pt) else None
                t = simple_type(dies, pref)
                if t is None:
                    bad = True
                    break
                params.append(t)
            if bad or ret is None:
                skipped.append(name)          # by-value aggregate; leave to Ghidra
                continue
            plist = ', '.join(params) if params else 'void'
            protos[name] = f'{ret} {name}({plist});'

    with open(args.output, 'w') as f:
        f.write('/* kd_protos.h — Karma function prototypes recovered from DWARF.\n'
                ' *\n'
                ' * Generated by tools/gen_protos.py, for GHIDRA to consume — not for the\n'
                ' * build. Types are simplified on purpose: Ghidra only needs argument\n'
                ' * count and size to get its stack analysis right, and simplified types\n'
                ' * parse with no external dependencies.\n'
                ' */\n')
        for n in sorted(protos):
            f.write(protos[n] + '\n')

    print(f'{args.output}: {len(protos)} prototypes from {len(objs)} objects'
          + (f' ({len(set(skipped))} skipped: by-value aggregate params)' if skipped else ''))


if __name__ == '__main__':
    main()
