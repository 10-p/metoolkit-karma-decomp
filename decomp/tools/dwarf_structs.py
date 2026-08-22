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
STR_RE = re.compile(r'\(indirect string, offset:\s*(?:0x)?[0-9a-f]+\):\s*(.*)$')
REF_RE = re.compile(r'<0x([0-9a-f]+)>')
OFF_RE = re.compile(r'DW_OP_plus_uconst:\s*(\d+)')

# Types that come from the system headers the recovered sources already include.
# Emitting our own copy would collide.
SYSTEM_TYPES = {
    '_IO_FILE', '_IO_marker', '_IO_wide_data', '_IO_codecvt',
    'timeval', 'timespec', 'tm', '__jmp_buf_tag', '__sigset_t',
    'sigaction', 'sigcontext', 'div_t', 'ldiv_t', 'lconv', 'FILE',
}


def array_extent(dies, ref):
    """For a DW_TAG_array_type DIE, return its element count (or None)."""
    d = dies.get(ref)
    if d is None or d['tag'] != 'DW_TAG_array_type':
        return None
    for c in d['children']:
        if c['tag'] != 'DW_TAG_subrange_type':
            continue
        ub = c['attrs'].get('DW_AT_upper_bound')
        if ub is not None:
            try:
                return int(ub.split()[0]) + 1
            except ValueError:
                return None
        cnt = c['attrs'].get('DW_AT_count')
        if cnt is not None:
            try:
                return int(cnt.split()[0])
            except ValueError:
                return None
    return None


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
    if tag == 'DW_TAG_base_type':
        return nm or 'int'
    if tag == 'DW_TAG_typedef':
        # If this typedef aliases a struct/union, spell it as `struct Tag` so a
        # forward declaration suffices. Otherwise keep the typedef name (MeReal,
        # MeU8, ...), which the public headers provide.
        u, hops = subref, 0
        while u is not None and hops < 8:
            hops += 1
            d2 = dies.get(u)
            if d2 is None:
                break
            if d2['tag'] in ('DW_TAG_structure_type', 'DW_TAG_class_type',
                             'DW_TAG_union_type'):
                un = d2['attrs'].get('DW_AT_name')
                if un:
                    kw = 'union' if d2['tag'] == 'DW_TAG_union_type' else 'struct'
                    return f'{kw} {un}'
                break
            if d2['tag'] == 'DW_TAG_pointer_type':
                inner = d2['attrs'].get('DW_AT_type')
                mi = REF_RE.search(inner) if inner else None
                if mi:
                    t2 = type_name(dies, int(mi.group(1), 16), depth + 1)
                    if t2.startswith(('struct ', 'union ')):
                        return t2 + ' *'
                break
            if d2['tag'] == 'DW_TAG_base_type':
                break
            nxt = d2['attrs'].get('DW_AT_type')
            m2 = REF_RE.search(nxt) if nxt else None
            u = int(m2.group(1), 16) if m2 else None
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


def declarator(dies, ref, name, depth=0):
    """Build a correct C declaration for `name` of the type at `ref`.

    C declarator syntax wraps the name rather than prefixing a type string, so
    naive concatenation produces nonsense for pointer-to-array (`int [] *p`
    instead of `int (*p)[3]`) and function pointers. This walks the type chain
    outward-in, parenthesising where precedence requires it."""
    if ref is None or depth > 16:
        return f'void {name}'
    d = dies.get(ref)
    if d is None:
        return f'void {name}'
    tag = d['tag']
    sub = d['attrs'].get('DW_AT_type')
    subref = int(REF_RE.search(sub).group(1), 16) if sub and REF_RE.search(sub) else None

    if tag == 'DW_TAG_pointer_type':
        inner = dies.get(subref, {}).get('tag') if subref is not None else None
        # `*` binds looser than `[]` and `()`, so those need parentheses.
        star = f'(*{name})' if inner in ('DW_TAG_array_type', 'DW_TAG_subroutine_type') \
               else f'*{name}'
        return declarator(dies, subref, star, depth + 1)
    if tag == 'DW_TAG_array_type':
        n = array_extent(dies, ref)
        return declarator(dies, subref, f'{name}[{n if n else ""}]', depth + 1)
    if tag == 'DW_TAG_subroutine_type':
        return declarator(dies, subref, f'{name}()', depth + 1)
    if tag in ('DW_TAG_const_type', 'DW_TAG_volatile_type'):
        q = 'const' if tag == 'DW_TAG_const_type' else 'volatile'
        inner = dies.get(subref, {}).get('tag') if subref is not None else None
        if inner == 'DW_TAG_pointer_type':          # `T * const p`
            return declarator(dies, subref, f'{q} {name}', depth + 1)
        return f'{q} ' + declarator(dies, subref, name, depth + 1)

    spelling = type_name(dies, ref)
    sep = '' if spelling.endswith('*') else ' '
    return f'{spelling}{sep}{name}'


def emit(dies, die):
    a = die['attrs']
    name = a.get('DW_AT_name', '<anon>')
    size = a.get('DW_AT_byte_size', '?')
    kw = 'union' if die['tag'] == 'DW_TAG_union_type' else 'struct'
    lines = [f'/* {name} — {size} bytes, recovered from DWARF */',
             f'{kw} {name} {{']
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
        decl = declarator(dies, mref, mn) + ';'
        lines.append(f'    {decl}'.ljust(52) +
                     (f'/* +0x{off:x} */' if off is not None else '/* +? */'))
    lines.append('};')
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


# ---------------------------------------------------------------------------
# C++ inheritance recovered from RTTI
# ---------------------------------------------------------------------------
def rtti_bases(obj):
    """{class: base} for polymorphic classes in `obj`, read from RTTI.

    A derived class that adds no data members gets no DW_TAG_member list of its
    own — gcc emits the layout only where the class is DEFINED, and for these
    "_vanilla" implementation classes that CU may not exist at all. But the
    Itanium ABI's typeinfo object for a single-inheritance class contains a
    pointer to its base's typeinfo, and that relocation is right there in the
    object. So the inheritance graph is machine-readable even when the layout
    is not."""
    import subprocess as _sp
    out = _sp.run(['readelf', '-S', '--wide', obj], capture_output=True, text=True).stdout
    bases = {}
    for sect in re.findall(r'(\.gnu\.linkonce\.[dr]\._ZTI\S+)', out):
        cls = sect.split('._ZTI', 1)[1]
        rel = _sp.run(['objdump', '-r', f'--section={sect}', obj],
                      capture_output=True, text=True).stdout
        for line in rel.splitlines():
            # Only relocation ROWS; the "RELOCATION RECORDS FOR [...]" header
            # names the section itself and would match the class as its own base.
            if not re.match(r'^[0-9a-f]{8}\s', line):
                continue
            m = re.search(r'_ZTI(\S+)', line)
            if m and m.group(1) != cls:
                bases[demangle_tag(cls)] = demangle_tag(m.group(1))
                break
    return bases


def demangle_tag(mangled_len_name):
    """`26keaMatrix_pcSparse_vanilla` -> `keaMatrix_pcSparse_vanilla`."""
    m = re.match(r'(\d+)(.*)', mangled_len_name)
    return m.group(2)[:int(m.group(1))] if m else mangled_len_name
