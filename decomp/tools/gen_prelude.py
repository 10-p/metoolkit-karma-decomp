#!/usr/bin/env python3
"""
gen_prelude.py — generate the per-object prelude skeleton for a Karma object.

The prelude is the one hand-written part of recovering an object. Everything it
needs is machine-readable, and the two mistakes that are easy to make by hand —
guessing a static's value, and guessing a C++ mangled name — are exactly the two
things this tool reads straight out of the binary.

It emits:
  * `#include` suggestions, by finding which metoolkit header declares each import
  * extern declarations for imports, with the demangled signature as a comment and
    a KD_MANGLED() asm label for C++ ones (the raw name, verbatim, never inferred)
  * definitions for file-scope statics, with values READ from .data/.rodata
  * explicit TODO markers for .bss statics, whose values come from a C++ static
    constructor rather than from the section bytes

Output is a starting point, not a finished file: anything marked TODO needs a
human. But nothing it emits is a guess.
"""
import argparse
import os
import re
import struct
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import dwarf_structs

# libc / compiler-runtime imports that need no declaration from us.
IGNORED_IMPORTS = {
    '__gxx_personality_v0', '__cxa_pure_virtual', '_Unwind_Resume',
    '__divdi3', '__udivdi3', '__fixunssfdi', '_GLOBAL_OFFSET_TABLE_',
    'memcpy', 'memset', 'malloc', 'free', 'calloc', 'realloc', 'abort', 'exit',
    'printf', 'fprintf', 'sprintf', 'snprintf', 'sqrtf', 'sqrt', 'fabs', 'fabsf',
    'stderr', 'stdout',
}


def run(*cmd):
    return subprocess.run(cmd, capture_output=True, text=True).stdout


def demangle(name):
    out = run('c++filt', name).strip()
    return out if out else name


def nm_rows(obj):
    """(value, size, kind, name) for every symbol; size may be None."""
    rows = []
    for line in run('nm', '--print-size', '--defined-only', obj).splitlines():
        p = line.split()
        if len(p) == 4:
            rows.append((int(p[0], 16), int(p[1], 16), p[2], p[3]))
        elif len(p) == 3:
            rows.append((int(p[0], 16), None, p[1], p[2]))
    return rows


def undefined(obj):
    names = []
    for line in run('nm', '--undefined-only', obj).splitlines():
        p = line.split()
        if p and p[-1] not in IGNORED_IMPORTS:
            names.append(p[-1])
    return sorted(set(names))


_data_decl_cache = {}


def extern_data_declaration(corpus, name):
    """`extern void *poolstack[3];` for an imported DATA symbol, from DWARF.

    An undefined symbol that is a VARIABLE has no prototype in kd_protos.h —
    that header is functions only — so it used to fall through to
    `/* TODO: declare X */` and the object failed to compile. keaMemory, the
    solver's allocator, imports four of them (`pool_ptr`, `pool_max`,
    `poolstack`, `poolstack_ptr`) and failed on nothing else.

    The type is not guessed. The object that DEFINES the symbol carries a
    DW_TAG_variable DIE for it with a full type chain, and dwarf_structs
    already renders that as a C declarator. `poolstack` comes back as
    `void *poolstack[3]`, which is the array `keaPushPoolFrame` indexes.

    THE SIZE CHECK IS THE POINT. A declaration that disagrees with the shipped
    object about how big a symbol is does not fail to compile — it silently
    reads or writes the wrong number of bytes at link time, which is the class
    of defect this project treats as worse than not compiling. So the DWARF
    declarator is accepted only when the type's size matches the `st_size` the
    defining object records, and a mismatch returns None (the caller emits the
    TODO, as before). Returns None rather than a guess whenever the defining
    object cannot be found, carries no DWARF, or names no such variable."""
    key = (corpus, name)
    if key in _data_decl_cache:
        return _data_decl_cache[key]
    _data_decl_cache[key] = None
    if not corpus or not os.path.isdir(corpus):
        return None

    # Find the object that defines `name` as an OBJECT (data), not a function.
    owner = size = None
    for fn in sorted(os.listdir(corpus)):
        if not fn.endswith('.o'):
            continue
        path = os.path.join(corpus, fn)
        for line in run('readelf', '-sW', path).splitlines():
            p = line.split()
            # num: value size type bind vis ndx name
            if len(p) >= 8 and p[3] == 'OBJECT' and p[6] != 'UND' and p[7] == name:
                owner, size = path, int(p[2])
                break
        if owner:
            break
    if not owner:
        return None

    dies = dwarf_structs.parse(owner)
    ref = None
    for die in dies.values():
        if die['tag'] != 'DW_TAG_variable':
            continue
        if die['attrs'].get('DW_AT_name') != name:
            continue
        t = die['attrs'].get('DW_AT_type')
        m = re.search(r'<0x([0-9a-f]+)>', t) if t else None
        if m:
            ref = int(m.group(1), 16)
        break
    if ref is None:
        return None

    decl = dwarf_structs.declarator(dies, ref, name)
    got = dwarf_structs.type_size(dies, ref)
    if got is None or got != size:
        return None
    _data_decl_cache[key] = (f'extern {decl};', os.path.basename(owner), size)
    return _data_decl_cache[key]


def dwarf_static_declaration(obj, name, size):
    """`MeFAssetCreateFromFile MeFAssetCreateFunc[1]` for a file-scope static.

    The fallback for a .data/.bss static is a guess from its BYTES, and for a
    four-byte slot of zeros that guess is `static float x = 0.0f;`. For a
    function-pointer hook — which is what a zeroed .data word usually is, since
    something installs it at run time — that is wrong in a way that stops the
    object compiling the moment the code calls through it:

        MeFAssetCreateFunc      declared  static float
        (*MeFAssetCreateFunc[0])(...)     "subscripted value is neither array
                                           nor pointer nor vector"

    The object's OWN DWARF says exactly what it is, array extent included, and
    that is the same source the imported-symbol path already uses. Restricted to
    ALL-ZERO bytes, so a static with a real initialiser keeps the byte-exact
    rendering and nothing is lost to a prettier type."""
    dies = dwarf_structs.parse(obj)
    for die in dies.values():
        if die['tag'] != 'DW_TAG_variable':
            continue
        if die['attrs'].get('DW_AT_name') != name:
            continue
        t = die['attrs'].get('DW_AT_type')
        m = re.search(r'<0x([0-9a-f]+)>', t) if t else None
        if not m:
            return None
        ref = int(m.group(1), 16)
        if dwarf_structs.type_size(dies, ref) != size:
            return None
        return dwarf_structs.declarator(dies, ref, name)
    return None


def section_bytes(obj, section):
    tmp = f'/tmp/.kd_sect_{os.getpid()}.bin'
    r = subprocess.run(['objcopy', '-O', 'binary', f'--only-section={section}', obj, tmp],
                       capture_output=True)
    if r.returncode != 0 or not os.path.exists(tmp):
        return b''
    data = open(tmp, 'rb').read()
    os.unlink(tmp)
    return data


def section_relocs(obj, section):
    """{offset: symbol} for a section's relocation records."""
    out = run('objdump', '-r', f'--section={section}', obj)
    rel = {}
    for line in out.splitlines():
        p = line.split()
        if len(p) >= 3 and re.match(r'^[0-9a-f]{8}$', p[0]):
            rel[int(p[0], 16)] = p[2]
    return rel


def text_symbol_at(obj, offset):
    """Which .text function starts at `offset`? (for R_386_32 against .text)"""
    best = None
    for value, size, kind, name in nm_rows(obj):
        if kind in 'tT' and value == offset:
            if best is None or kind == 'T':
                best = name
    return best


def exported_data_definition(obj, name, value, size, sect, data, c_name, sections):
    """Rebuild an EXPORTED data symbol as a C definition.

    These are globals the object publishes — MeMemoryAPI, MeDebugDrawAPI,
    MdtContactInvalidID. Dropping them makes the archive lose the symbol and
    every user of it fails to link, which is exactly what happened.

    Raw bytes are not enough: MeMemoryAPI is 24 bytes of ZEROES plus six
    R_386_32 relocations against .text, i.e. a table of function pointers whose
    targets live only in the relocation records. So read those too.

    A relocation can also point at a SECTION rather than a symbol, which is what
    the linker gets for a string or float constant that never had a name of its
    own. `(void *)&.rodata` is not C. The bytes are in the object and the addend
    says where, so the section is materialised alongside the table and the entry
    points into it. `sections` collects what has to be emitted."""
    rel = section_relocs(obj, sect)
    entries, has_ptr = [], False
    for off in range(value, value + (size or 0), 4):
        if off in rel:
            has_ptr = True
            addend = struct.unpack('<I', data[off:off + 4])[0] if off + 4 <= len(data) else 0
            target = rel[off]
            if target == '.text':
                fn = text_symbol_at(obj, addend)
                entries.append(f'(void *)&{fn}' if fn else f'/* .text+0x{addend:x} */ 0')
            elif target.startswith('.'):
                c = 'kd_exp' + re.sub(r'\W', '_', target)
                sections[target] = c
                entries.append(f'(void *)&{c}[0x{addend:x}]')
            else:
                entries.append(f'(void *)&{target}')
        else:
            word = struct.unpack('<I', data[off:off + 4])[0] if off + 4 <= len(data) else 0
            entries.append(f'(void *)0x{word:x}u')
    n = len(entries)
    lines = [f'/* {name} — EXPORTED {sect} symbol, {size} bytes'
             + (', rebuilt from relocations */' if has_ptr else ' */')]
    # An asm label keeps our spelling from clashing with the public header's
    # declaration, exactly as for exported functions.
    lines.append(f'void *kd_{c_name}[{n}] KD_MANGLED("{name}") = {{')
    lines.append('    ' + ', '.join(entries))
    lines.append('};')
    return '\n'.join(lines)


def declared_names(umbrella, include_dir):
    """Every function name the umbrella declares, AFTER preprocessing.

    Grepping header text is not enough: metoolkit declares whole families
    through macros. McdBox.h contains only

        MCD_DECLARE_GEOMETRY_TYPE(McdBox);

    which expands to McdBoxGetBSphere, McdBoxGetMassProperties and friends. A
    textual search finds none of them, so the prelude re-declares them with
    simplified types and the translation unit fails with "conflicting types".

    One preprocessor run over the umbrella settles it for every object; the
    result is cached because it does not change between objects."""
    if not umbrella or not os.path.exists(umbrella):
        return set()
    cache = f'/tmp/.kd_declared_{os.path.getmtime(umbrella):.0f}.txt'
    if os.path.exists(cache):
        return set(open(cache).read().split())
    inc = include_dir or ''
    args = ['gcc', '-m32', '-E', '-P', '-DLINUX', '-x', 'c', umbrella,
            '-I' + os.path.dirname(umbrella), '-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        args.append('-I' + os.path.join(inc, d))
    out = subprocess.run(args, capture_output=True, text=True).stdout
    names = set(re.findall(r'\b([A-Za-z_]\w*)\s*\(', out))
    try:
        open(cache, 'w').write('\n'.join(sorted(names)))
    except OSError:
        pass
    return names


def header_declaring(name, include_dir):
    """Which metoolkit header declares `name`? (cheap grep; good enough)"""
    if not include_dir:
        return None
    for root, _, files in os.walk(include_dir):
        for f in files:
            if not f.endswith('.h'):
                continue
            try:
                txt = open(os.path.join(root, f), errors='ignore').read()
            except OSError:
                continue
            # Match a data declaration too, not just a function: MeMemoryAPI is
            # `extern MeMemoryAPIStruct MeMemoryAPI;` and an anchor on `NAME(`
            # misses it entirely, which then reads as an undeclared import.
            if re.search(r'\b' + re.escape(name) + r'\s*[;(\[]', txt):
                return f
    return None


CTOR_RE = re.compile(r'void\s+__static_initialization_and_destruction_0\b.*?\n\{(.*?)\n\}',
                     re.S)
STORE_RE = re.compile(r'^\s*([A-Za-z_]\w*)((?:\.\w+|\[\d+\])*)\s*=\s*([^;]+);\s*$')


def ctor_initialisers(dump_path):
    """Recover .bss statics' values from the C++ static constructor.

    gcc puts a file-scope object with a non-trivial initialiser in .bss and
    fills it at load time from __static_initialization_and_destruction_0, so the
    section bytes are all zero and the real values only exist as stores in that
    function. Returns {name: [(subscript, value), ...]} plus a flag for anything
    that is not a plain constant store."""
    try:
        txt = open(dump_path, errors='ignore').read()
    except OSError:
        return {}, False
    m = CTOR_RE.search(txt)
    if not m:
        return {}, False
    out, messy = {}, False
    for line in m.group(1).split('\n'):
        line = line.strip()
        if not line or line.startswith(('if', '}', '{', 'return', '/*')):
            continue
        sm = STORE_RE.match(line)
        if not sm:
            messy = True
            continue
        name, subscript, value = sm.group(1), sm.group(2), sm.group(3).strip()
        # Only plain numeric constants are safe to turn into an initialiser.
        if not re.fullmatch(r'-?[0-9.]+(?:[eE][-+]?\d+)?[fF]?', value):
            messy = True
            continue
        out.setdefault(name, []).append((subscript, value))
    return out, messy


def chunk_of(data, off, size):
    """The section bytes a static occupies, or b'' if they are not there."""
    return data[off:off + size] if size and off + size <= len(data) else b''


def render_value(data, off, size):
    """Best-effort literal for a static's initializer."""
    chunk = data[off:off + size] if size else b''
    if not chunk:
        return None, None
    if size == 4:
        f, = struct.unpack('<f', chunk)
        i, = struct.unpack('<i', chunk)
        return f'{f!r}f', f'float={f!r}  int={i}  bytes={chunk.hex()}'
    if size == 8:
        d, = struct.unpack('<d', chunk)
        return f'{d!r}', f'double={d!r}  bytes={chunk.hex()}'
    if size % 4 == 0 and size <= 64:
        floats = struct.unpack('<' + 'f' * (size // 4), chunk)
        return ('{ ' + ', '.join(f'{v!r}f' for v in floats) + ' }',
                f'as floats: {floats}  bytes={chunk.hex()}')
    return None, f'bytes={chunk.hex()}'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('object')
    ap.add_argument('-o', '--output')
    ap.add_argument('--include-dir', help='metoolkit include/ root, for #include hints')
    ap.add_argument('--dump', help='Ghidra .c dump, used to recover .bss statics '
                                   'from the C++ static constructor')
    ap.add_argument('--protos', help='kd_protos.h from tools/gen_protos.py, used to '
                                     'give C++-mangled imports a real signature')
    ap.add_argument('--corpus', help='directory of every archive member. An imported '
                                     'VARIABLE has no prototype in kd_protos.h, so its '
                                     'declaration is recovered from the DWARF of whichever '
                                     'object DEFINES it — and size-checked against that '
                                     "object's symbol table before being emitted.")
    ap.add_argument('--umbrella', help='kd_karma.h; headers it already includes are '
                                       'NOT re-included here. Several metoolkit '
                                       'headers (MeSet.h among them) have no include '
                                       'guard at all, so including one twice redefines '
                                       'everything in it.')
    ap.add_argument('--exports-out', help='write exported DATA symbols here instead of '
                                          'into the prelude. They initialise from function '
                                          'addresses, so they must be emitted AFTER the '
                                          'forward declarations, not before them.')
    args = ap.parse_args()

    obj = args.object
    rows = nm_rows(obj)
    ctor_inits, ctor_messy = ctor_initialisers(args.dump) if args.dump else ({}, False)
    protos = {}
    if args.protos:
        # `float Foo(void *, int);` -> protos['Foo'] = the whole line
        for line in open(args.protos, errors='ignore'):
            m = re.match(r'\s*([\w \*]+?)\s*\b([A-Za-z_]\w*)\s*\(', line)
            if m:
                protos[m.group(2)] = line.strip()
    out = []
    w = out.append

    w(f'/*  Prelude for {os.path.basename(obj)} — generated by tools/gen_prelude.py.')
    w(' *')
    w(' *  Values below were READ from the object, not inferred. Anything the tool')
    w(' *  could not determine is marked TODO and needs a human.')
    w(' */')
    w('')

    # ---- imports ----------------------------------------------------------
    imports = undefined(obj)
    declared = declared_names(args.umbrella, args.include_dir)
    _ = protos  # parsed above; used for both mangled and plain imports
    hdrs = set()
    decls = []
    for name in imports:
        dem = demangle(name)
        if dem != name:                       # C++ mangled
            decls.append((name, dem, True))
        elif name in declared:
            pass                     # the umbrella already declares it
        else:
            h = header_declaring(name, args.include_dir)
            if h:
                hdrs.add(h)
            else:
                decls.append((name, dem, False))

    umbrella = set()
    if args.umbrella and os.path.exists(args.umbrella):
        umbrella = set(re.findall(r'#include\s+<([^>]+)>',
                                  open(args.umbrella, errors='ignore').read()))
    hdrs -= umbrella
    if hdrs:
        w('/* --- imports declared by headers the umbrella does not cover --- */')
        for h in sorted(hdrs):
            w(f'#include <{h}>')
        w('')

    if decls:
        w('/* --- imports needing an explicit declaration --- */')
        for name, dem, mangled in decls:
            w(f'/* {dem} */')
            if mangled:
                # The demangled form is `Ns::Foo(int, float&)`; the DWARF-derived
                # prototype database is keyed on the bare name.
                qualified = dem.split('(')[0].strip()
                short = re.sub(r'.*::', '', qualified)
                # ghidra_clean flattens `Foo::bar` to `Foo__bar` in the BODY, so
                # the declaration has to use the same spelling or the call is an
                # implicit declaration of an unrelated function and the object
                # fails to link. keaLCP_new lost seven keaLCPSolver methods this
                # way.
                cname = qualified.replace('::', '__')
                proto = protos.get(short)
                # A C++ symbol whose demangled name a PUBLIC header also
                # declares. McdConvexMesh.h expands
                # MCD_DECLARE_INTERSECT_INTERACTION(Sphyl, ConvexMesh) to
                # `MeBool McdSphylConvexMeshIntersect(McdModelPair *,
                # McdIntersectResult *)`, and the object imports the MANGLED
                # _Z27McdSphylConvexMeshIntersect... — so declaring ours under
                # the same C spelling is "conflicting types" and the object
                # fails. Only the mangled symbol exists in the library, so
                # deferring to the header's declaration would link against a
                # name that is not there.
                #
                # Declare under kd_ext_ and let the macro redirect the body. The
                # macro is emitted AFTER kd_karma.h, so the header's own
                # declaration is already parsed and unaffected.
                shadowed = cname in declared
                emit_name = ('kd_ext_' + cname) if shadowed else cname
                if proto:
                    decl = re.sub(r'\b' + re.escape(short) + r'\s*\(',
                                  emit_name + '(', proto[:-1].strip(), count=1)
                    w(f'extern {decl}')
                    w(f'    KD_MANGLED("{name}");')
                    if shadowed:
                        w(f'#define {cname} {emit_name}')
                else:
                    w('/* TODO: fill in the C signature matching the demangled form above. */')
                    w(f'extern int {re.sub(r"[^A-Za-z0-9_]", "_", cname)}()')
                    w(f'    KD_MANGLED("{name}");')
            else:
                proto = protos.get(name)
                if proto:
                    w(f'extern {proto[:-1].strip()};')
                else:
                    # Not a function. An imported VARIABLE has no entry in
                    # kd_protos.h, so this used to be an unconditional TODO and
                    # the object failed to compile — keaMemory, the solver's
                    # allocator, failed on nothing else. Recover the declaration
                    # from the DWARF of whichever object defines it, size-checked
                    # against that object's symbol table.
                    dd = extern_data_declaration(args.corpus, name)
                    if dd:
                        decl, owner, size = dd
                        w(f'/* {size} bytes, defined in {owner}; type from its DWARF */')
                        w(decl)
                    else:
                        w(f'/* TODO: declare {name} */')
            w('')

    # ---- file-scope statics ----------------------------------------------
    data = {s: section_bytes(obj, s) for s in ('.data', '.rodata', '.bss')}
    kind_to_section = {'d': '.data', 'r': '.rodata', 'b': '.bss',
                       'D': '.data', 'R': '.rodata', 'B': '.bss'}
    statics = [r for r in rows if r[2] in 'drb']
    # Uppercase kinds are GLOBAL data the object exports. They were being
    # skipped entirely, so the recovered object published no such symbol and
    # every reference to it failed to link.
    exported_data = [r for r in rows if r[2] in 'DRB']
    if statics:
        w('/* --- file-scope statics --- */')
        if ctor_messy:
            w('/* NOTE: the static constructor contains stores the tool could not read as')
            w(' * plain constants. Re-check it by hand before trusting this section. */')
    for value, size, kind, name in sorted(statics, key=lambda r: (r[2], r[0])):
        sect = kind_to_section[kind]
        c_name = re.sub(r'[^A-Za-z0-9_]', '_', demangle(name))
        # Ghidra renders C++ function-local statics as `Func::var`; ghidra_clean
        # flattens that to `Func__var`. Match it here.
        dem = demangle(name)
        m = re.match(r'(.+)::(.+)$', dem)
        if m:
            c_name = f'{m.group(1)}__{m.group(2)}'
        if kind == 'b':
            w(f'/* {name}  ({sect}, {size} bytes) */')
            inits = ctor_inits.get(c_name) or ctor_inits.get(name)
            n = max(1, (size or 4)) // 4
            if inits:
                # Stores are `Name.v[0] = -1.0;` etc; index by position.
                vals = ['0.0f'] * n
                for i, (_, v) in enumerate(inits[:n]):
                    vals[i] = v if v.lower().endswith('f') else v + 'f'
                w(f'/* recovered from __static_initialization_and_destruction_0 */')
                w(f'static float {c_name}[{n}] = {{ ' + ', '.join(vals) + ' };')
            else:
                w('/* TODO: zero in the object — value comes from the C++ static constructor,')
                w(' * which the tool could not read. Check')
                w(' * __static_initialization_and_destruction_0 in the decompiled dump. */')
                w(f'static float {c_name}[{n}]; /* TODO: initializer */')
            w('')
            continue
        lit, note = render_value(data.get(sect, b''), value, size or 0)
        w(f'/* {name}  ({sect}+0x{value:x}, {size} bytes) */')
        # An all-zero slot carries no evidence of its own type, so prefer what
        # the DWARF says over a guess from the bytes. See
        # dwarf_static_declaration: this is what makes a zeroed function-pointer
        # hook come out as a function pointer rather than as a float.
        raw_bytes = chunk_of(data.get(sect, b''), value, size or 0)
        if size and raw_bytes and not any(raw_bytes):
            decl = dwarf_static_declaration(obj, name, size)
            if decl:
                qual0 = 'const ' if sect == '.rodata' else ''
                w(f'/* type from this object\'s DWARF; bytes are all zero */')
                w(f'static {qual0}{decl};')
                w('')
                continue
        if note:
            w(f'/* read from object: {note} */')
        # `const` only for .rodata. A .data static is writable by definition, and
        # several of them are file-scope scratch: IxCylinderCylinder keeps its
        # dot products there, and marking them const made ten assignments in
        # CylCylIntersect fail to compile.
        qual = 'const ' if sect == '.rodata' else ''
        if lit and size == 4:
            w(f'static {qual}float {c_name} = {lit};')
        elif lit:
            w(f'static {qual}float {c_name}[] = {lit};')
        elif chunk_of(data.get(sect, b''), value, size):
            # Too big for a float guess — McdGjk4Di is a 328-byte table of
            # simplex indices. Emit the bytes, which are exact and carry no
            # guess about the element type. If some object does use it as
            # something other than bytes the compiler says so, which is better
            # than a TODO nobody reads.
            raw = chunk_of(data.get(sect, b''), value, size)
            w(f'/* element type unknown; bytes are exact */')
            w(f'static {qual}unsigned char {c_name}[{len(raw)}] = {{ '
              + ', '.join('0x%02x' % b for b in raw) + ' };')
        else:
            w(f'/* TODO: {c_name} — tool could not render a literal */')
        w('')

    exp_out = []
    exp_sections = {}
    if exported_data:
        w2 = exp_out.append if args.exports_out else w
        prev_w = w
        w = w2
        w('/* --- exported data symbols --- */')
        for value, size, kind, name in sorted(exported_data, key=lambda r: (r[2], r[0])):
            sect = kind_to_section[kind]
            c_name = re.sub(r'[^A-Za-z0-9_]', '_', demangle(name))
            if kind == 'B':
                inits = ctor_inits.get(c_name) or ctor_inits.get(name)
                n = max(1, (size or 4)) // 4
                w(f'/* {name} — EXPORTED {sect} symbol, {size} bytes */')
                if inits:
                    vals = ['0.0f'] * n
                    for i, (_, v) in enumerate(inits[:n]):
                        vals[i] = v if v.lower().endswith('f') else v + 'f'
                    w(f'float kd_{c_name}[{n}] KD_MANGLED("{name}") = {{ '
                      + ', '.join(vals) + ' };')
                else:
                    w(f'float kd_{c_name}[{n}] KD_MANGLED("{name}");')
            else:
                w(exported_data_definition(obj, name, value, size, sect,
                                           data.get(sect, b''), c_name,
                                           exp_sections))
            w('')
        w = prev_w

    if exp_sections:
        # Emitted BEFORE the tables that index them, and read straight out of
        # the object — these are the unnamed constants gcc parked in .rodata.
        head = []
        for sname, cname in sorted(exp_sections.items()):
            raw = section_bytes(obj, sname)
            head.append(f'/* {sname}: {len(raw)} bytes, for the tables below */')
            head.append(f'static const unsigned char {cname}[{max(len(raw), 1)}] = {{ '
                        + ', '.join('0x%02x' % b for b in raw) + ' };')
        exp_out = head + [''] + exp_out

    if args.exports_out:
        open(args.exports_out, 'w').write('\n'.join(exp_out) + '\n')

    text = '\n'.join(out)
    if args.output:
        open(args.output, 'w').write(text)
        n_todo = text.count('TODO')
        print(f'{args.output}: {len(imports)} imports, {len(statics)} statics, '
              f'{n_todo} TODO(s) needing a human')
    else:
        print(text)


if __name__ == '__main__':
    main()
