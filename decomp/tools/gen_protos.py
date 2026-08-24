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
        # A by-value aggregate. This used to return None and the whole function
        # was then DROPPED from the header, with the comment "leave to Ghidra" —
        # which is precisely the thing this file exists to stop Ghidra doing.
        #
        # It cost the solver. MdtKeaConstraints and MdtKeaParameters are passed
        # BY VALUE all through the kea API, so the dropped list was
        # MdtKeaAddConstraintForces (the driver), MdtKeaIntegrateSystem and
        # KeaIntegrateSystem_vanilla (the integrator), allocateMemory and
        # vanillaAllocateMemory (the allocator) — every one of the four objects
        # blocking libMdtKea, and no coincidence. With no prototype Ghidra had to
        # guess a ~130-byte argument area and lost the frame entirely, which is
        # why keaRbdCore_unified decompiles to a wall of in_stack_fffffeNN.
        #
        # Ghidra needs the argument COUNT and SIZE, not the field layout, so an
        # opaque aggregate of the right size is enough and parses with no
        # dependencies. Returning the size lets main() emit the typedef.
        try:
            size = int(a.get('DW_AT_byte_size', 0))
        except ValueError:
            size = 0
        if size <= 0:
            return None                      # genuinely unknown; still refuse
        return ('AGG', size)
    if tag == 'DW_TAG_enumeration_type':
        return 'int'
    return 'int'          # unknown: assume one 4-byte slot rather than nothing


def collect_aliases(dies, out):
    """Record {mangled ELF name: DW_AT_name} for the C++ functions in this object.

    The source is DW_AT_MIPS_linkage_name, which gcc 3.2 emits for class
    MEMBERS. That is a fact out of the object's own debug info — nothing here
    re-implements Itanium mangling, and nothing matches on a name.

    C++ FREE functions (`MovingBoxBoxIntersect`, `AccumulateSphylContacts`,
    `ConvexHullNSegment`, `PolynomialRoots`) get no linkage attribute, and
    covering them is TRIED AND REJECTED — see HANDOVER.md dead end 19. The
    route works: a subprogram's DW_AT_low_pc and the ELF symbol's value are
    offsets into the same .text, so the symbol at the concrete instance's
    low_pc is the linkage name, and it took coverage from 40 of 75 undefined
    mangled symbols to 66. It also took the build from 109 objects to 105,
    because those functions ARE declared in the importing object's own DWARF,
    so Ghidra already had their real signatures and the simplified prototype
    (every pointer `void *`) overwrote them. `lsVec3 *` became `void *` in
    IxBoxBox, IxConvexTriList, IxCylinderCylinder and IxSphylPrimitives — four
    objects with live in-game evidence in proven.txt — and they stopped
    compiling.

    Members do not have that problem, measured rather than argued: with this
    route alone all 109 compiled objects stay byte-identical. The distinction
    that matters is not C-vs-C++, it is whether Ghidra already had a signature.
    """
    for die in dies.values():
        if die['tag'] != 'DW_TAG_subprogram':
            continue
        a = die['attrs']
        name = a.get('DW_AT_name')
        linkage = a.get('DW_AT_MIPS_linkage_name') or a.get('DW_AT_linkage_name')
        if not (linkage and name) or linkage == name:
            continue
        if not linkage.replace('_', 'a').isalnum():
            continue
        # An offset that two differently-named functions claim is not evidence.
        if out.get(linkage, name) != name:
            out[linkage] = None
        else:
            out.setdefault(linkage, name)


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

    protos, skipped, aggs = {}, [], set()
    sigs, aliases = {}, {}
    for obj in objs:
        try:
            dies = parse(obj)
        except Exception:                                       # noqa: BLE001
            continue
        collect_aliases(dies, aliases)
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
            if isinstance(ret, tuple):
                aggs.add(ret[1])
                ret = 'kd_agg%d' % ret[1]
            params, bad = [], False
            for c in die['children']:
                if c['tag'] != 'DW_TAG_formal_parameter':
                    continue
                pt = c['attrs'].get('DW_AT_type')
                pref = int(REF_RE.search(pt).group(1), 16) if pt and REF_RE.search(pt) else None
                t = simple_type(dies, pref)
                if isinstance(t, tuple):
                    aggs.add(t[1])
                    t = 'kd_agg%d' % t[1]
                if t is None:
                    bad = True
                    break
                params.append(t)
            if bad or ret is None:
                skipped.append(name)          # size genuinely unknown
                continue
            plist = ', '.join(params) if params else 'void'
            protos[name] = f'{ret} {name}({plist});'
            sigs[name] = (ret, plist)

    # Re-emit every prototype under its MANGLED name as well.
    #
    # ParseKarmaHeaders matches a prototype to an ELF symbol by NAME, and it
    # runs as a preScript — before Ghidra's demangler analyzer. So the symbol is
    # still `_ZN12keaFunctions20checkPrintDebugInput...` while DW_AT_name, and
    # therefore this header, says `checkPrintDebugInput`. Every C++ function in
    # the corpus went unmatched and Ghidra fell back to guessing the arity from
    # the call site — the exact failure this whole file exists to prevent. 75
    # undefined mangled symbols across 21 objects were in that state.
    #
    # keaRbdCore_unified is the worked example, and the damage is not cosmetic.
    # Ghidra guessed the outgoing argument area for
    #   keaFunctions::checkPrintDebugInput(this, constraints, parameters, blist, n)
    # with `this` DROPPED, so it read the 92-byte `constraints` from esp+0 where
    # the machine code writes it at esp+4 (`lea 0x4(%esp),%edi`) and
    # `parameters` from esp+0x5c where the code writes esp+0x60 — every by-value
    # aggregate argument one word early. Same shape as the __regparm shift of
    # HANDOVER.md 10.
    #
    # A mangled name is a valid C identifier and is unique by construction, so
    # nothing on the Ghidra side has to change: its existing exact-name lookup
    # simply starts hitting.
    for mangled, bare in sorted(aliases.items()):
        if bare and bare in sigs and mangled not in protos:
            ret, plist = sigs[bare]
            protos[mangled] = f'{ret} {mangled}({plist});'


    with open(args.output, 'w') as f:
        f.write('/* kd_protos.h — Karma function prototypes recovered from DWARF.\n'
                ' *\n'
                ' * Generated by tools/gen_protos.py, for GHIDRA to consume — not for the\n'
                ' * build. Types are simplified on purpose: Ghidra only needs argument\n'
                ' * count and size to get its stack analysis right, and simplified types\n'
                ' * parse with no external dependencies.\n'
                ' */\n')
        if aggs:
            f.write('\n/* Opaque stand-ins for aggregates passed or returned BY VALUE.\n'
                    ' * Only the size matters: it is what tells Ghidra how much of the\n'
                    ' * argument area belongs to that parameter. Dropping these functions\n'
                    ' * instead cost the whole libMdtKea frame analysis — see simple_type().\n'
                    ' */\n')
            for n in sorted(aggs):
                f.write('typedef struct { char _kd[%d]; } kd_agg%d;\n' % (n, n))
            f.write('\n')
        for n in sorted(protos):
            f.write(protos[n] + '\n')

    print(f'{args.output}: {len(protos)} prototypes from {len(objs)} objects, '
          f'{len(aggs)} by-value aggregate size(s)'
          + (f', {len(set(skipped))} skipped (size unknown)' if skipped else ''))


if __name__ == '__main__':
    main()
