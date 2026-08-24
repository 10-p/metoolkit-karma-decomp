#!/usr/bin/env python3
"""
gen_vtable_callsites.py — say which function each C++ virtual call actually reaches,
as a table of Ghidra addresses for DumpDecomp.java to apply signatures at.

THE PROBLEM THIS EXISTS FOR. Ghidra has no signature for a call through a
function pointer, so it emits the call with EVERY ARGUMENT DROPPED. In the
libMdtKea solver driver that is not a cosmetic loss — it is the whole reason the
object cannot be recovered:

    (**(code **)(_vanillaFunctions + 0x10))();
    (**(code **)(_vanillaAMatrix   + 0xc ))();

Those are `keaFunctions_Vanilla::platformInit(this)` and
`keaMatrix_pcSparse_vanilla::factorize(this)`, and four of their siblings are the
calls into the three compute kernels that are already proven bit-identical
against the shipped library. Until the calls carry their arguments, the kernels
cannot be reached and nothing runs on recovered kea.

DumpDecomp.java already applies a signature at a call site — that is what
KD_CALLSITE_SIG=trilist does for McdTriangleListFnPtr. What it cannot do is work
out WHICH signature, because the vtable lives in a different object file and
therefore a different Ghidra program. That part is done here, in Python, where
vtable_slots.py has already established the ABI facts and checks them.

HOW A SITE IS RESOLVED. gcc 3.2 constructs these objects as LOCALS and the
dispatch is a two-instruction idiom:

    mov  -0xb8(%ebp),%ecx        ; ecx = the vptr stored in the local
    call *0x10(%ecx)             ; slot +0x10 of that class's vtable

so a site is resolved by taking the slot offset from the call, walking back to
the `mov` that last defined the call's register, and looking the local's frame
offset up in the set of vptr stores vtable_slots.vptr_stores() read out of the
constructor. `-0xb8` is -184, which is where MdtKeaAddConstraintForces stores
keaFunctions_Vanilla's vtable address point.

WHY THIS IS ALLOWED TO BE BELIEVED. Nothing here is inferred from decompiled
text; every step is read from the object and every step is checked:

  * the class comes from a relocation naming `vtable for <class>`, and
    vptr_stores() refuses any store whose addend is not the +8 ABI address
    point, so a multiple-inheritance secondary vtable cannot be mistaken for a
    primary one.
  * the slot must EXIST in that class's vtable, which is read from relocation
    records in the object that defines it. An offset that is not a slot is
    refused rather than rounded to one.
  * the backward walk refuses if anything else writes the register in between,
    so a reused register cannot silently carry a stale vptr.
  * the resolved mapping was already confirmed against an independent
    measurement: it puts platformInit at +0x10, calcJinvMandRHS at +0,
    calcIworldandNonInertialForceandVhmf at +4 and
    calculateConstraintAndResultantForces at +8, and test/scene_census.sh counts
    exactly those four running 900 times each in 900 solver steps.

Getting a slot wrong does not fail to compile — it calls the wrong function with
the wrong arguments. So every rule here declines rather than guesses, and the
`skipped` count in the summary is meant to be read, not ignored.

Output is one row per resolved site:

    <object>  <ghidra-address>  <method>  <class>+<slot>

The address is in Ghidra's invented memory map for a relocatable .o, which
ghidra_clean.ghidra_memory_map() reproduces and which HANDOVER.md 5 documents.
"""
import argparse
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ghidra_clean
import vtable_slots

# `call *0x10(%ecx)` / `call *(%ecx)` — a dispatch through a register.
CALL_IND = re.compile(r'^\s*([0-9a-f]+):\s+(?:[0-9a-f]{2} )+\s*call\s+\*'
                      r'(?:(-?0x[0-9a-f]+))?\(%(e[a-z]{2})\)')
# `mov -0xb8(%ebp),%ecx` — loading a vptr out of a local.
MOV_FROM_EBP = re.compile(r'^\s*([0-9a-f]+):\s+(?:[0-9a-f]{2} )+\s*mov\s+'
                          r'(-?0x[0-9a-f]+)\(%ebp\),%(e[a-z]{2})')
# Anything else that writes the register we are tracking.
WRITES_REG = re.compile(r'^\s*[0-9a-f]+:\s+(?:[0-9a-f]{2} )+\s*'
                        r'(?:mov|lea|add|sub|xor|or|and|pop|inc|dec|imul|movl)\S*\s+'
                        r'.*,%(e[a-z]{2})\s*$')
FUNC_HDR = re.compile(r'^([0-9a-f]+) <(.+)>:')
SECTION_HDR = re.compile(r'^Disassembly of section (\S+):')


def run(*cmd):
    return subprocess.run(cmd, capture_output=True, text=True).stdout


def disassemble(obj):
    """{section: [(func_name, func_off, [lines])]} from objdump, demangled."""
    out = run('objdump', '-d', '--demangle', obj)
    sections, cur_sec, cur_fn = {}, None, None
    for line in out.splitlines():
        m = SECTION_HDR.match(line)
        if m:
            cur_sec = m.group(1)
            sections.setdefault(cur_sec, [])
            cur_fn = None
            continue
        m = FUNC_HDR.match(line)
        if m and cur_sec is not None:
            cur_fn = (m.group(2), int(m.group(1), 16), [])
            sections[cur_sec].append(cur_fn)
            continue
        if cur_fn is not None:
            cur_fn[2].append(line)
    return sections


def section_base(obj, section):
    """Where Ghidra puts `section` in its invented map, or None."""
    secs, _ext = ghidra_clean.ghidra_memory_map(obj)
    for name, _typ, start, _end in secs:
        if name == section:
            return start
    return None


def _is_indirect_call(data, off):
    """Is there really a `call *m32` at this section offset?

    THE ADDRESS ARITHMETIC GETS ONE CHANCE TO BE RIGHT. A signature applied at
    the wrong address is not a no-op — it can land on a DIFFERENT call site and
    give that one somebody else's parameter list, which compiles and calls the
    right function with the wrong arguments.

    An earlier version of this file computed `base + function_offset +
    call_offset`, on the assumption that objdump prints call offsets relative to
    the function. It does not: they are relative to the SECTION, so the function
    offset was counted twice. It went unnoticed because the object this tool
    exists for, keaRbdCore_unified, has its only relevant function at offset 0 —
    where the two agree — while keaLCPSolver's later functions were thrown past
    the end of .text into .eh_frame.

    So the offset is now checked against the bytes rather than trusted. Every
    `call *m32` form is opcode 0xFF with reg field /2, optionally after prefixes,
    so the check is cheap and it fails loudly on exactly the arithmetic error
    that produced the bug."""
    i = off
    # skip legacy prefixes (segment, operand/address size, lock/rep)
    while i < len(data) and data[i] in (0x26, 0x2e, 0x36, 0x3e, 0x64, 0x65,
                                        0x66, 0x67, 0xf0, 0xf2, 0xf3):
        i += 1
    if i + 1 >= len(data) or data[i] != 0xFF:
        return False
    return ((data[i + 1] >> 3) & 7) == 2          # /2 == CALL r/m32


def resolve_object(corpus, obj, report):
    """[(ghidra_addr, method, class, slot)] for every virtual call site resolved."""
    rows = []
    for section, funcs in disassemble(obj).items():
        base = section_base(obj, section)
        if base is None:
            continue
        data = ghidra_clean._section_bytes(obj, section)
        for fname, _foff, lines in funcs:
            stores = vtable_slots.vptr_stores(obj, fname)
            if not stores:
                continue
            # {frame offset: class}. A frame slot holding two different classes
            # would make the lookup ambiguous, so drop any such slot entirely
            # rather than pick — it cannot happen for a local whose dynamic type
            # is its static type, and if it ever does the assumption is wrong.
            by_slot = {}
            for cls, offs in stores.items():
                for off in offs:
                    by_slot.setdefault(off, set()).add(cls)
            by_slot = {o: next(iter(c)) for o, c in by_slot.items() if len(c) == 1}
            if not by_slot:
                continue

            for i, line in enumerate(lines):
                m = CALL_IND.match(line)
                if not m:
                    continue
                call_off, slot_txt, reg = m.group(1), m.group(2), m.group(3)
                slot = int(slot_txt, 16) if slot_txt else 0
                # Walk back for the mov that last defined `reg`.
                frame_off = None
                for j in range(i - 1, -1, -1):
                    prev = lines[j]
                    mm = MOV_FROM_EBP.match(prev)
                    if mm and mm.group(3) == reg:
                        frame_off = int(mm.group(2), 16)
                        break
                    w = WRITES_REG.match(prev)
                    if w and w.group(1) == reg:
                        break            # redefined by something else; refuse
                if frame_off is None:
                    report['no_vptr_source'] += 1
                    continue
                cls = by_slot.get(frame_off)
                if cls is None:
                    report['not_a_vptr_local'] += 1
                    continue
                table = vtable_slots.slot_table(corpus, cls)
                target = table.get(slot)
                if target is None:
                    # Not a slot in that class's vtable. Refuse loudly rather
                    # than round to the nearest one.
                    report['slot_not_in_vtable'] += 1
                    continue
                dem = run('c++filt', target).strip() or target
                method = re.sub(r'\(.*', '', dem).split('::')[-1].strip()
                if not re.fullmatch(r'[A-Za-z_]\w*', method):
                    report['unnameable'] += 1
                    continue
                # objdump's offsets are SECTION-relative, so the function offset
                # must NOT be added again. Confirm against the bytes.
                off = int(call_off, 16)
                if not _is_indirect_call(data, off):
                    report['not_an_indirect_call'] += 1
                    continue
                addr = base + off
                rows.append((addr, method, cls, slot))
                report['resolved'] += 1
    return rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('corpus', help='directory of every archive member')
    ap.add_argument('-o', '--output', required=True)
    ap.add_argument('--only', action='append',
                    help='restrict to these object basenames (repeatable). '
                         'Applying a wrong signature is worse than applying '
                         'none, so widening this is a decision to take on '
                         'evidence.')
    args = ap.parse_args()

    report = {k: 0 for k in ('resolved', 'no_vptr_source', 'not_a_vptr_local',
                             'slot_not_in_vtable', 'unnameable',
                             'not_an_indirect_call')}
    out_rows = []
    for fn in sorted(os.listdir(args.corpus)):
        if not fn.endswith('.o'):
            continue
        if args.only and fn[:-2] not in args.only and fn not in args.only:
            continue
        obj = os.path.join(args.corpus, fn)
        for addr, method, cls, slot in resolve_object(args.corpus, obj, report):
            out_rows.append((fn, addr, method, cls, slot))

    with open(args.output, 'w') as f:
        f.write('# object  ghidra-address  method  class+slot\n')
        f.write('# generated by tools/gen_vtable_callsites.py — see its docstring\n')
        for fn, addr, method, cls, slot in out_rows:
            f.write(f'{fn} 0x{addr:08x} {method} {cls}+0x{slot:02x}\n')

    print(f'{args.output}: {len(out_rows)} call site(s) resolved')
    for k, v in report.items():
        if v:
            print(f'  {k}: {v}')
    by_obj = {}
    for fn, _a, _m, _c, _s in out_rows:
        by_obj[fn] = by_obj.get(fn, 0) + 1
    for fn, n in sorted(by_obj.items()):
        print(f'  {fn}: {n}')


if __name__ == '__main__':
    main()
