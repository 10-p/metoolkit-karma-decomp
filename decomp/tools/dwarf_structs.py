#!/usr/bin/env python3
"""
dwarf_structs.py — emit C struct definitions for types defined in an object's DWARF.

Ghidra applies DWARF types well for plain C, but for some C++ classes it leaves
`this` untyped and renders members as `field_0x14`. Those accesses are *correct*
— an offset is an offset — but unreadable, and unreadable code is unreviewable.

This tool reads the real member names, offsets and types straight out of
.debug_info so the recovered source can use them.

  ./dwarf_structs.py keaMatrix_PcSparse_vanilla.o --type keaMatrix_pcSparse
  ./dwarf_structs.py foo.o --list

Emitting a struct whose members line up with the offsets Ghidra used lets you
sed `field_0x14` -> the real name with confidence, because the offsets are
printed alongside.
"""
import argparse
import re
import subprocess
import sys

DIE_RE = re.compile(r'^\s*<(\d+)><([0-9a-f]+)>:\s+Abbrev Number:\s+\d+\s+\((\w+)\)')
ATTR_RE = re.compile(r'^\s*<[0-9a-f]+>\s+(DW_AT_\w+)\s*:\s*(.*)$')
STR_RE = re.compile(r'\(indirect string, offset: 0x[0-9a-f]+\):\s*(.*)$')
REF_RE = re.compile(r'<0x([0-9a-f]+)>')
OFF_RE = re.compile(r'DW_OP_plus_uconst:\s*(\d+)')


def parse(obj):
    """Return {offset: {'tag':…, 'depth':…, 'attrs':{…}, 'children':[…]}}"""
    out = subprocess.run(['readelf', '--debug-dump=info', obj],
                         capture_output=True, text=True).stdout
    dies, stack, cur = {}, [], None
    for line in out.splitlines():
        m = DIE_RE.match(line)
        if m:
            depth, off, tag = int(m.group(1)), int(m.group(2), 16), m.group(3)
            cur = {'tag': tag, 'depth': depth, 'attrs': {}, 'children': [], 'off': off}
            dies[off] = cur
            while stack and stack[-1]['depth'] >= depth:
                stack.pop()
            if stack:
                stack[-1]['children'].append(cur)
            stack.append(cur)
            continue
        m = ATTR_RE.match(line)
        if m and cur is not None:
            key, val = m.group(1), m.group(2).strip()
            s = STR_RE.search(val)
            if s:
                val = s.group(1).strip()
            cur['attrs'][key] = val
    return dies


def type_name(dies, ref, depth=0):
    """Best-effort C spelling of the type at DIE offset `ref`."""
    if ref is None or depth > 12:
        return 'void'
    d = dies.get(ref)
    if d is None:
        return 'void'
    a = d['attrs']
    nm = a.get('DW_AT_name')
    tag = d['tag']
    sub = a.get('DW_AT_type')
    subref = int(REF_RE.search(sub).group(1), 16) if sub and REF_RE.search(sub) else None
    if tag in ('DW_TAG_base_type', 'DW_TAG_typedef'):
        return nm or 'int'
    if tag == 'DW_TAG_pointer_type':
        return type_name(dies, subref, depth + 1) + ' *'
    if tag == 'DW_TAG_const_type':
        return 'const ' + type_name(dies, subref, depth + 1)
    if tag == 'DW_TAG_volatile_type':
        return 'volatile ' + type_name(dies, subref, depth + 1)
    if tag == 'DW_TAG_array_type':
        return type_name(dies, subref, depth + 1) + ' []'
    if tag in ('DW_TAG_structure_type', 'DW_TAG_class_type'):
        return f'struct {nm}' if nm else 'struct /*anon*/'
    if tag == 'DW_TAG_union_type':
        return f'union {nm}' if nm else 'union /*anon*/'
    if tag == 'DW_TAG_enumeration_type':
        return f'enum {nm}' if nm else 'int'
    if tag == 'DW_TAG_subroutine_type':
        return 'void (*)()'
    return nm or 'void'


def emit(dies, die):
    a = die['attrs']
    name = a.get('DW_AT_name', '<anon>')
    size = a.get('DW_AT_byte_size', '?')
    lines = [f'/* {name} — {size} bytes, recovered from DWARF */',
             f'typedef struct {name} {{']
    has_vptr = False
    first_off = None
    for c in die['children']:
        if c['tag'] != 'DW_TAG_member':
            continue
        mn = c['attrs'].get('DW_AT_name', '<anon>')
        mt = c['attrs'].get('DW_AT_type')
        mref = int(REF_RE.search(mt).group(1), 16) if mt and REF_RE.search(mt) else None
        loc = c['attrs'].get('DW_AT_data_member_location', '')
        mo = OFF_RE.search(loc)
        off = int(mo.group(1)) if mo else None
        if first_off is None:
            first_off = off
        # gcc names the vtable pointer `_vptr.Klass`, which is not a legal C
        # identifier. Rename it and give it a usable type.
        if mn.startswith('_vptr'):
            has_vptr = True
            lines.append(f'    const struct {name}_vtbl *vptr;'.ljust(52) +
                         (f'/* +0x{off:x} */' if off is not None else ''))
            continue
        mn = re.sub(r'[^A-Za-z0-9_]', '_', mn)
        t = type_name(dies, mref)
        star = '' if t.endswith('*') else ' '
        lines.append(f'    {t}{star}{mn};'.ljust(52) +
                     (f'/* +0x{off:x} */' if off is not None else '/* +? */'))
    lines.append(f'}} {name};')
    if first_off:
        lines.append(f'/* NOTE: first member is at +0x{first_off:x}, not 0 — {name} DERIVES from a')
        lines.append(f' * base class occupying [0, 0x{first_off:x}). Embed the base as the first')
        lines.append(' * member in C, or repeat its fields, to keep the layout identical. */')
    if has_vptr:
        lines.append(f'/* NOTE: {name} is polymorphic. Ghidra renders virtual calls as')
        lines.append(' *   (**(code **)(*(int *)obj + N))(obj, ...)   i.e. vtable slot N/4.')
        lines.append(f' * Declare `struct {name}_vtbl` with one function pointer per slot. */')
    return '\n'.join(lines)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('object')
    ap.add_argument('--type', action='append', default=[],
                    help='type name to emit (repeatable); default: all named')
    ap.add_argument('--list', action='store_true', help='just list named types + sizes')
    ap.add_argument('-o', '--output')
    args = ap.parse_args()

    dies = parse(args.object)
    structs = [d for d in dies.values()
               if d['tag'] in ('DW_TAG_structure_type', 'DW_TAG_class_type')
               and d['attrs'].get('DW_AT_name')
               and any(c['tag'] == 'DW_TAG_member' for c in d['children'])]

    if args.list:
        for d in sorted(structs, key=lambda x: x['attrs']['DW_AT_name']):
            nm = d['attrs']['DW_AT_name']
            sz = d['attrs'].get('DW_AT_byte_size', '?')
            nmem = sum(1 for c in d['children'] if c['tag'] == 'DW_TAG_member')
            print(f'  {nm:44s} {sz:>5} bytes  {nmem:3d} members')
        return

    wanted = set(args.type)
    chosen = [d for d in structs
              if not wanted or d['attrs']['DW_AT_name'] in wanted]
    if wanted and not chosen:
        print(f'no such type in {args.object}', file=sys.stderr)
        sys.exit(1)
    seen, text = set(), []
    for d in chosen:
        nm = d['attrs']['DW_AT_name']
        if nm in seen:
            continue
        seen.add(nm)
        text.append(emit(dies, d))
    body = '\n\n'.join(text)
    if args.output:
        open(args.output, 'w').write(body + '\n')
        print(f'{args.output}: {len(seen)} type(s)')
    else:
        print(body)


if __name__ == '__main__':
    main()
