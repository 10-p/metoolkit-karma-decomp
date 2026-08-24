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


def array_dims(dies, ref):
    """Every dimension of a DW_TAG_array_type, outermost first, or None.

    DWARF represents a multi-dimensional array as ONE array_type with several
    DW_TAG_subrange_type children — `MeReal x[24][2][3]` is one DIE with three
    subranges, not three nested DIEs. Reading only the first is a silent
    truncation: McdSphere's `sphereDraw` came out as `MeReal[24]`, 96 bytes,
    against a symbol the object records as 576, and the code indexes it as
    `sphereDraw[0][0]`."""
    d = dies.get(ref)
    if d is None or d['tag'] != 'DW_TAG_array_type':
        return None
    dims = []
    for c in d['children']:
        if c['tag'] != 'DW_TAG_subrange_type':
            continue
        ub = c['attrs'].get('DW_AT_upper_bound')
        cnt = c['attrs'].get('DW_AT_count')
        try:
            if ub is not None:
                dims.append(int(ub.split()[0]) + 1)
            elif cnt is not None:
                dims.append(int(cnt.split()[0]))
            else:
                dims.append(None)          # flexible / unknown extent
        except ValueError:
            dims.append(None)
    return dims or None


def array_extent(dies, ref):
    """Total element count of a DW_TAG_array_type (or None).

    The PRODUCT of every dimension, so it is the count a size calculation
    wants. See array_dims for why the first subrange is not enough."""
    dims = array_dims(dies, ref)
    if not dims or any(x is None for x in dims):
        return None
    n = 1
    for x in dims:
        n *= x
    return n


def type_size(dies, ref, depth=0):
    """Size in bytes of the type at DIE offset `ref`, or None if not derivable.

    Exists so a generated declaration can be CHECKED against the `st_size` the
    shipped object records for the symbol, rather than trusted. A declaration
    that disagrees with the object about a symbol's size still compiles and
    still links; it just reads or writes the wrong bytes. Returning None on
    anything not positively derivable is deliberate — the caller is expected to
    decline rather than to fall back to a guess."""
    if ref is None or depth > 16:
        return None
    d = dies.get(ref)
    if d is None:
        return None
    a = d['attrs']
    tag = d['tag']
    sub = a.get('DW_AT_type')
    subref = int(REF_RE.search(sub).group(1), 16) if sub and REF_RE.search(sub) else None

    if tag == 'DW_TAG_pointer_type':
        # DW_AT_byte_size is present on i386 pointer DIEs, but default to the
        # target's pointer width rather than failing if it is absent.
        bs = a.get('DW_AT_byte_size')
        return int(bs.split()[0]) if bs else 4
    if tag == 'DW_TAG_array_type':
        n = array_extent(dies, ref)
        elem = type_size(dies, subref, depth + 1)
        return None if (n is None or elem is None) else n * elem
    if tag in ('DW_TAG_typedef', 'DW_TAG_const_type', 'DW_TAG_volatile_type'):
        return type_size(dies, subref, depth + 1)
    bs = a.get('DW_AT_byte_size')
    if bs is not None:
        try:
            return int(bs.split()[0])
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
        # Every dimension, outermost first: `x[24][2][3]`, not `x[24]`.
        dims = array_dims(dies, ref) or [None]
        sub = ''.join('[%s]' % ('' if n is None else n) for n in dims)
        return declarator(dies, subref, f'{name}{sub}', depth + 1)
    if tag == 'DW_TAG_subroutine_type':
        # Write the parameter list out. `f(...)` with an EMPTY list is a function
        # type with no prototype, so C applies the default argument promotions at
        # every call through it — a float goes across as a double and a callee
        # that does have a prototype reads the low half. That is exactly the
        # defect that made IxConvexTriList wrong for three sessions
        # (HANDOVER.md 8), and emitting `()` here builds it into the type
        # database by construction.
        params = []
        varargs = False
        for c in d.get('children', []):
            if c['tag'] == 'DW_TAG_unspecified_parameters':
                varargs = True
                continue
            if c['tag'] != 'DW_TAG_formal_parameter':
                continue
            pt = c['attrs'].get('DW_AT_type')
            pm = REF_RE.search(pt) if pt else None
            params.append(declarator(dies, int(pm.group(1), 16) if pm else None,
                                     '', depth + 1).strip())
        if varargs:
            params.append('...')
        plist = ', '.join(params) if params else 'void'
        return declarator(dies, subref, f'{name}({plist})', depth + 1)
    if tag in ('DW_TAG_const_type', 'DW_TAG_volatile_type'):
        q = 'const' if tag == 'DW_TAG_const_type' else 'volatile'
        inner = dies.get(subref, {}).get('tag') if subref is not None else None
        if inner == 'DW_TAG_pointer_type':          # `T * const p`
            return declarator(dies, subref, f'{q} {name}', depth + 1)
        return f'{q} ' + declarator(dies, subref, name, depth + 1)

    spelling = type_name(dies, ref)
    sep = '' if spelling.endswith('*') else ' '
    return f'{spelling}{sep}{name}'


def emit_enum(die):
    """C definition for a DW_TAG_enumeration_type.

    Enums were missing from the type database entirely, which is why
    IxConvexPrimitives could not compile: VoronoiRegionType is an internal enum
    with no declaration in metoolkit's public headers. There are 466
    enumeration DIEs across the SDK, so this was not a one-off gap."""
    name = die['attrs'].get('DW_AT_name', '<anon>')
    lines = [f'/* {name} — enum, recovered from DWARF */', f'enum {name} {{']
    vals = []
    for c in die['children']:
        if c['tag'] != 'DW_TAG_enumerator':
            continue
        n = c['attrs'].get('DW_AT_name')
        v = c['attrs'].get('DW_AT_const_value', '0').split()[0]
        if n:
            vals.append(f'    {n} = {v},')
    lines += vals or ['    /* no enumerators recorded */']
    lines.append('};')
    return '\n'.join(lines)


def emit(dies, die, base_die=None, base_dies=None):
    """C definition. If `base_die` is given, its members are emitted first.

    A derived class's DWARF lists only the members it ADDS, starting at the
    offset where the base ends (keaMatrix_pcSparse begins at +0x14). Emitting
    just those puts every field at the wrong offset AND leaves the base's fields
    — m_numRows, matrix, matrixChol — with no declaration at all, so recovered
    code referencing them fails with "has no member named". Splicing the base in
    fixes both at once, and the inheritance comes from RTTI."""
    a = die['attrs']
    name = a.get('DW_AT_name', '<anon>')
    size = a.get('DW_AT_byte_size', '?')
    kw = 'union' if die['tag'] == 'DW_TAG_union_type' else 'struct'
    lines = [f'/* {name} — {size} bytes, recovered from DWARF */',
             f'{kw} {name} {{']
    has_vptr = False
    first_off = None
    # DWARF states the base class outright, in a DW_TAG_inheritance child. The
    # RTTI route above only reaches POLYMORPHIC bases, so a plain one — Link,
    # which CxSmallSortMarker derives from — was left as a comment saying
    # "embed the base yourself", and 200 references to it across the corpus
    # failed to compile.
    #
    # The member is named `super_<Base>` because that is what Ghidra calls it,
    # so the recovered code needs no rewriting. It also puts the base at [0,
    # sizeof(Base)) where it belongs, which the comment could not do.
    inherit_name = None
    if base_die is None:
        for c in die['children']:
            if c['tag'] != 'DW_TAG_inheritance':
                continue
            bt = c['attrs'].get('DW_AT_type', '')
            m = REF_RE.search(bt)
            if not m:
                continue
            bd = dies.get(int(m.group(1), 16))
            bn = bd['attrs'].get('DW_AT_name') if bd else None
            if bn:
                inherit_name = re.sub(r'[^A-Za-z0-9_]', '_', bn)
            break
    # Each member carries the DIE table it must be resolved against. DIE offsets
    # are per-compilation-unit, so a base member's DW_AT_type refers into the
    # BASE's table; merging the two tables lets same-numbered entries collide and
    # silently mistypes fields (int m_numRows came out as long double).
    members = [(c, dies) for c in die['children']]
    if base_die is not None:
        members = [(c, base_dies or dies) for c in base_die['children']] + members
    if inherit_name:
        lines.append(f'    struct {inherit_name} super_{inherit_name};'.ljust(52)
                     + '/* +0x0  base class */')
    for c, cdies in members:
        if c['tag'] != 'DW_TAG_member':
            continue
        mn = c['attrs'].get('DW_AT_name', '<anon>')
        mt = c['attrs'].get('DW_AT_type')
        mref = int(REF_RE.search(mt).group(1), 16) if mt and REF_RE.search(mt) else None
        dies_here = cdies
        loc = c['attrs'].get('DW_AT_data_member_location', '')
        mo = OFF_RE.search(loc)
        off = int(mo.group(1)) if mo else None
        if first_off is None:
            first_off = off
        # gcc names the vtable pointer `_vptr.Klass`, which is not a legal C
        # identifier. Ghidra names the same member `_vptr_Klass`, which is —
        # and is what the recovered code references — so use that spelling
        # rather than inventing a third one. Calling it `vptr` meant every
        # `this->_vptr_CxSmallSort` failed with "has no member named".
        if mn.startswith('_vptr'):
            has_vptr = True
            # `code **`, not a pointer to a named vtbl struct. Ghidra calls
            # through this member directly — `(**this->_vptr_X)(this)` — and a
            # struct nobody ever defines makes that "invalid use of undefined
            # type". kd_compat.h's `code` is an unprototyped function type,
            # which is exactly what a vtable slot is here, and the generated
            # source includes kd_compat.h before this header.
            lines.append(f'    code **_vptr_{name};'.ljust(52) +
                         (f'/* +0x{off:x} */' if off is not None else ''))
            continue
        mn = re.sub(r'[^A-Za-z0-9_]', '_', mn)
        decl = declarator(dies_here, mref, mn) + ';'
        lines.append(f'    {decl}'.ljust(52) +
                     (f'/* +0x{off:x} */' if off is not None else '/* +? */'))
    lines.append('};')
    if first_off and base_die is None:
        lines.append(f'/* NOTE: first member is at +0x{first_off:x}, not 0 — {name} DERIVES from a')
        lines.append(f' * base class occupying [0, 0x{first_off:x}). Embed the base as the first')
        lines.append(' * member in C, or repeat its fields, to keep the layout identical. */')
    if has_vptr:
        lines.append(f'/* NOTE: {name} is polymorphic. Ghidra renders virtual calls as')
        lines.append(' *   (**(code **)(*(int *)obj + N))(obj, ...)   i.e. vtable slot N/4.')
        lines.append(' * The vptr is `code **` so those calls compile as written; the')
        lines.append(' * slot NUMBER carries the meaning, and gen_vtables.py re-emits the')
        lines.append(' * table itself from the object\'s relocations. */')
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
