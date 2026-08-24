#!/usr/bin/env python3
"""
vtable_slots.py — resolve a virtual call to the function it actually reaches,
and check the answer against the machine code before believing it.

THE PROBLEM. When a function constructs a polymorphic C++ object as a LOCAL and
calls its methods, Ghidra loses the vptr. It names the slot after the variable
with a leading underscore and emits the dispatch with the offset intact:

    keaFunctions_Vanilla vanillaFunctions;        /* a local */
    _vanillaFunctions = keaFunctions__initPool;   /* the vptr store, mislabelled */
    (**(code **)(_vanillaFunctions + 0x10))();    /* a virtual call */

`_vanillaFunctions` is undeclared, so the object does not compile, and the
recover.py detector quarantines it as an indirect call through a mislabelled
symbol. Two of the four objects blocking libMdtKea fail this way and no other:
keaRbdCore_unified (the solver driver) and keaLCPSolver (the LCP).

WHAT IS ACTUALLY KNOWN HERE. Everything. The local's dynamic type is its static
type — it is constructed in place and never assigned — and gcc's Itanium ABI
fixes the rest:

    _ZTV<class> = { 0, &_ZTI<class>, &slot0, &slot1, ... }

with the ADDRESS POINT at +8, which is what a constructor stores in the vptr.
So `_var + N` is the slot at `_ZTV<class> + 8 + N`, and every slot is a
relocation record naming a real function.

Read the machine code for keaLCPSolver::PrincipalSubmatrix and it says so
literally:

    movl $0x8,-0x58(%ebp)    R_386_32 vtable for keaMatrix_pcSparse_vanilla
    mov  -0x58(%ebp),%edx
    call *0xc(%edx)

— store vtable+8 into the local's first word, then call through +0xc of it.

WHY THIS FILE CHECKS ITSELF. Getting a slot wrong does not fail to compile, it
calls the wrong function. That is the one outcome this project treats as worse
than not compiling, so `vptr_stores()` reads back from the SHIPPED function's
disassembly which class's vtable address point it actually stores into a local,
and the caller is expected to refuse the rewrite unless that agrees with the
local's declared type. Read its docstring before changing the check: verifying
against the CALL SITES instead looks obvious and is wrong.

The mapping has also been confirmed against an independent source. Applying it
to keaRbdCore_unified gives platformInit at +0x10, calcJinvMandRHS at +0,
calcIworldandNonInertialForceandVhmf at +4 and
calculateConstraintAndResultantForces at +8 — and test/scene_census.sh counts
exactly those four running 900 times each in 900 solver steps, which is a
measurement taken for an unrelated reason months apart from this.
"""
import os
import re
import subprocess

_slot_cache = {}
_owner_cache = {}


def _run(*cmd):
    return subprocess.run(cmd, capture_output=True, text=True).stdout


def mangle_class(name):
    """`keaMatrix_pcSparse_vanilla` -> `26keaMatrix_pcSparse_vanilla`.

    Only unqualified names; Karma has no namespaced polymorphic classes and a
    nested one would need the N...E form. Callers get None rather than a guess."""
    if not name or '::' in name or not re.fullmatch(r'[A-Za-z_]\w*', name):
        return None
    return '%d%s' % (len(name), name)


def vtable_owner(corpus, class_name):
    """The object file defining this class's vtable, or None."""
    key = (corpus, class_name)
    if key in _owner_cache:
        return _owner_cache[key]
    mangled = mangle_class(class_name)
    sym = '_ZTV' + mangled if mangled else None
    found = None
    if sym:
        for fn in sorted(os.listdir(corpus)):
            if not fn.endswith('.o'):
                continue
            path = os.path.join(corpus, fn)
            out = _run('nm', '--defined-only', path)
            if re.search(r'\s%s$' % re.escape(sym), out, re.M):
                found = path
                break
    _owner_cache[key] = found
    return found


def slot_table(corpus, class_name):
    """{byte offset from the ADDRESS POINT: mangled symbol}.

    Offsets are from the address point (vtable + 8), because that is the value a
    constructor stores in the vptr and therefore the base Ghidra's arithmetic is
    relative to. The two ABI header words are deliberately not in the table: a
    call through them would be a call to the typeinfo pointer, which is a defect,
    not a slot."""
    key = (corpus, class_name)
    if key in _slot_cache:
        return _slot_cache[key]
    obj = vtable_owner(corpus, class_name)
    table = {}
    if obj:
        mangled = mangle_class(class_name)
        section = None
        for line in _run('readelf', '-SW', obj).splitlines():
            m = re.search(r'(\.gnu\.linkonce\.[dr]\._ZTV%s)\b' % re.escape(mangled), line)
            if m:
                section = m.group(1)
                break
        if section:
            for line in _run('objdump', '-r', '--section=' + section, obj).splitlines():
                m = re.match(r'([0-9a-f]{8})\s+R_386_32\s+(\S+)', line)
                if not m:
                    continue
                off = int(m.group(1), 16)
                if off < 8:                    # offset-to-top / typeinfo
                    continue
                table[off - 8] = m.group(2)
    _slot_cache[key] = table
    return table


def vptr_stores(obj, func_symbol):
    """Classes whose vtable ADDRESS POINT this function stores into a local.

    This, and not the call sites, is what the rewrite actually rests on. A first
    version of this file tried to verify by collecting every `call *0xNN(%reg)`
    in the function and checking the offsets against the slot table. That is
    wrong, and keaLCPSolver::PrincipalSubmatrix shows why:

        mov  0x24(%esi),%ecx     ; ecx = this->A, a keaMatrix*
        mov  (%ecx),%ebx         ; ebx = A's vptr
        call *0x14(%ebx)         ; a virtual call on this->A

    That is a different object with a different vtable, and counting its offset
    against keaMatrix_pcSparse_vanilla's slot table would have "verified" a
    mapping onto the wrong class. Ghidra gets these right anyway — `this->A` is a
    typed member, so it emits `(**(code **)(*(int *)this->A + 0x18))(...)` — and
    only the LOCAL's vptr is mislabelled.

    What the constructor does is unambiguous and needs no dataflow:

        movl $0x8,-0x58(%ebp)    R_386_32 vtable for keaMatrix_pcSparse_vanilla

    an absolute store of `vtable + 8` into a stack slot. It fixes both the class
    and the fact that the address point is +8. Returns {class name: [stack
    offsets]}; an empty result means "cannot confirm", which callers must treat
    as a refusal rather than as consent.
    """
    dis = _run('objdump', '-dr', '--demangle', obj)
    lines, seen = [], False
    for line in dis.splitlines():
        if re.match(r'^[0-9a-f]+ <', line):
            seen = func_symbol in line
            continue
        if seen:
            if not line.strip():
                seen = False
                continue
            lines.append(line)

    out = {}
    for i, line in enumerate(lines):
        # movl $ADDEND, DISP(%ebp)  — the addend is the ABI address point.
        m = re.search(r'movl\s+\$0x([0-9a-f]+),\s*(-?0x[0-9a-f]+)\(%e[bs]p\)', line)
        if not m:
            continue
        # objdump WRAPS a long instruction encoding onto a continuation line, so
        # the relocation is not always the next line. keaRbdCore_unified's store
        # is ten bytes and lands two lines up from its relocation; looking only
        # at i+1 found the store in keaLCPSolver and missed the solver driver
        # entirely, which is the object this whole file exists for.
        r = None
        for j in range(i + 1, min(i + 4, len(lines))):
            r = re.search(r'R_386_32\s+vtable for (\S.*?)\s*$', lines[j])
            if r:
                break
            if not re.fullmatch(r'\s*[0-9a-f]+:\s*(?:[0-9a-f]{2} )+\s*', lines[j]):
                break                          # a real instruction; stop looking
        if not r:
            continue
        if int(m.group(1), 16) != 8:
            # Not the address point. Refuse loudly rather than shifting the
            # table by a guess — a multiple-inheritance secondary vtable would
            # land here and its slots are a different set.
            continue
        out.setdefault(r.group(1).strip(), []).append(int(m.group(2), 16))
    return out


def describe(corpus, class_name):
    """Human-readable slot table, for reports and for eyeballing a mapping."""
    table = slot_table(corpus, class_name)
    rows = []
    for off in sorted(table):
        sym = table[off]
        dem = _run('c++filt', sym).strip() or sym
        rows.append((off, sym, dem))
    return rows


if __name__ == '__main__':
    import sys
    if len(sys.argv) < 3:
        print(__doc__)
        print('usage: vtable_slots.py <corpus-dir> <class> [obj func_symbol]')
        raise SystemExit(2)
    corpus, cls = sys.argv[1], sys.argv[2]
    owner = vtable_owner(corpus, cls)
    print('vtable for %s defined in %s' % (cls, owner or '(not found)'))
    for off, sym, dem in describe(corpus, cls):
        print('  +0x%02x  %s' % (off, dem))
    if len(sys.argv) >= 5:
        st = vptr_stores(sys.argv[3], sys.argv[4])
        print('\nvptr stores in %s: %s' % (sys.argv[4], st or '(none found)'))
