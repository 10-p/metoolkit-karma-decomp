#!/usr/bin/env python3
"""
gen_vtables.py — re-emit an object's C++ ABI data (vtable, typeinfo, type string)
as plain C.

Recovering a polymorphic class's METHODS is not enough. gcc also emits three
COMDAT data objects per class, and anything that constructs the class references
them:

    _ZTS<class>   the type string, e.g. "9keaMatrix"
    _ZTI<class>   the typeinfo:  { &__cxxabiv1::__class_type_info vtable + 8,
                                   &_ZTS<class> [, &_ZTI<base>] }
    _ZTV<class>   the vtable:    { 0, &_ZTI<class>, &method0, &method1, ... }

Ghidra's decompiler output contains none of that — it decompiles code, not
COMDAT data — so the recovered object linked with correct method symbols and
still failed with "undefined reference to keaMatrix::writebackMatrixChol()"
from whoever built the vtable.

Everything needed is in the object: the layout is fixed by the Itanium ABI, the
slot contents are relocation records, and the type string is section bytes. So
this reads them and writes the equivalent C.

Emitted as `weak` because the originals are COMDAT: several objects may define
the same vtable and the linker must be free to keep one.
"""
import argparse
import os
import re
import subprocess
import sys


def run(*cmd):
    return subprocess.run(cmd, capture_output=True, text=True).stdout


def cxx_sections(obj):
    """Section names holding C++ ABI data, keyed by kind."""
    out = run('readelf', '-S', '--wide', obj)
    found = {'ZTV': {}, 'ZTI': {}, 'ZTS': {}}
    for m in re.finditer(r'(\.gnu\.linkonce\.[dr]\._ZT([VIS])(\S+))', out):
        found[f'ZT{m.group(2)}'][m.group(3)] = m.group(1)
    return found


def section_data(obj, section):
    tmp = f'/tmp/.kd_vt_{os.getpid()}.bin'
    r = subprocess.run(['objcopy', '-O', 'binary', f'--only-section={section}', obj, tmp],
                       capture_output=True)
    if r.returncode != 0 or not os.path.exists(tmp):
        return b''
    data = open(tmp, 'rb').read()
    os.unlink(tmp)
    return data


def section_relocs(obj, section):
    rel = {}
    for line in run('objdump', '-r', f'--section={section}', obj).splitlines():
        p = line.split()
        if len(p) >= 3 and re.match(r'^[0-9a-f]{8}$', p[0]):
            rel[int(p[0], 16)] = p[2]
    return rel


def defined_symbols(obj):
    """mangled -> flattened C name our recovered source defines it as."""
    rows = []
    for line in run('nm', '--defined-only', obj).splitlines():
        p = line.split()
        if len(p) >= 3 and p[-2] in 'TtWw':
            rows.append(p[-1])
    if not rows:
        return {}
    dem = run('c++filt', *rows).split('\n')
    out = {}
    for mangled, d in zip(rows, dem):
        base = d.split('(')[0].strip()
        out[mangled] = 'kd_' + re.sub(r'[^A-Za-z0-9_]', '_', base.replace('::', '__'))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('object')
    ap.add_argument('-o', '--output', required=True)
    args = ap.parse_args()

    obj = args.object
    sect = cxx_sections(obj)
    if not any(sect.values()):
        open(args.output, 'w').write('/* no C++ ABI data in this object */\n')
        print(f'{args.output}: none')
        return

    defined = defined_symbols(obj)
    out, externs, n = [], set(), 0
    out.append('/* C++ ABI data (vtable / typeinfo / type string), re-emitted from the')
    out.append(' * original object by tools/gen_vtables.py. Layout is fixed by the Itanium')
    out.append(' * ABI; slot contents come from the relocation records. `weak` because the')
    out.append(' * originals are COMDAT. */')
    out.append('')

    # Which C++ ABI objects does THIS object define? A derived class's typeinfo
    # points at its base's, which normally lives in a different object
    # (keaMatrix_PcSparse references _ZTI9keaMatrix, defined in keaMatrix.o).
    # Referencing it without a declaration is an undeclared identifier.
    local_zti = set(sect['ZTI'])
    local_zts = set(sect['ZTS'])
    ext_abi = set()

    def abi_ref(kind, cls):
        """`kd_ZTI<cls>` / `kd_ZTS<cls>`, declaring it extern if not ours."""
        if kind == 'ZTI' and cls not in local_zti:
            ext_abi.add(('ZTI', cls))
        elif kind == 'ZTS' and cls not in local_zts:
            ext_abi.add(('ZTS', cls))
        return f'kd_{kind}{cls}'

    def cname(sym):
        """C identifier to use for a symbol referenced by a slot."""
        if sym in defined:
            return defined[sym]
        safe = 'kd_ext_' + re.sub(r'[^A-Za-z0-9_]', '_', sym)
        externs.add((safe, sym))
        return safe

    body = []
    for cls, s in sorted(sect['ZTS'].items()):
        data = section_data(obj, s)
        text = data.split(b'\0')[0].decode('ascii', 'replace')
        body.append(f'__attribute__((weak)) const char kd_ZTS{cls}[]')
        body.append(f'    __asm__("_ZTS{cls}") = "{text}";')
        body.append('')
        n += 1

    for cls, s in sorted(sect['ZTI'].items()):
        rel = section_relocs(obj, s)
        entries = []
        for off in sorted(rel):
            sym = rel[off]
            if sym.startswith('_ZTVN10__cxxabiv1'):
                # Points 8 bytes into the abi's own type_info vtable.
                entries.append(f'(const void *)((const char *)&{cname(sym)}[0] + 8)')
            elif sym.startswith('_ZTS'):
                entries.append(f'(const void *){abi_ref("ZTS", sym[4:])}')
            elif sym.startswith('_ZTI'):
                entries.append(f'(const void *){abi_ref("ZTI", sym[4:])}')
            else:
                entries.append(f'(const void *)&{cname(sym)}')
        body.append(f'__attribute__((weak)) const void *kd_ZTI{cls}[{len(entries)}]')
        body.append(f'    __asm__("_ZTI{cls}") = {{')
        body.append('        ' + ',\n        '.join(entries))
        body.append('    };')
        body.append('')
        n += 1

    for cls, s in sorted(sect['ZTV'].items()):
        data = section_data(obj, s)
        rel = section_relocs(obj, s)
        slots = (len(data) or (max(rel) + 4 if rel else 0)) // 4
        entries = []
        for i in range(slots):
            off = i * 4
            sym = rel.get(off)
            if sym is None:
                entries.append('(const void *)0')          # offset-to-top
            elif sym.startswith('_ZTI'):
                entries.append(f'(const void *){abi_ref("ZTI", sym[4:])}')
            else:
                entries.append(f'(const void *)&{cname(sym)}')
        body.append(f'__attribute__((weak)) const void *kd_ZTV{cls}[{len(entries)}]')
        body.append(f'    __asm__("_ZTV{cls}") = {{')
        body.append('        ' + ',\n        '.join(entries))
        body.append('    };')
        body.append('')
        n += 1

    if ext_abi:
        out.append('/* C++ ABI objects defined in OTHER objects (a base class\'s'
                   ' typeinfo) */')
        for kind, cls in sorted(ext_abi):
            if kind == 'ZTS':
                out.append(f'extern const char kd_ZTS{cls}[] __asm__("_ZTS{cls}");')
            else:
                out.append(f'extern const void *kd_ZTI{cls}[] __asm__("_ZTI{cls}");')
        out.append('')

    if externs:
        out.append('/* symbols the tables point at that this object does not define */')
        for safe, sym in sorted(externs):
            if sym.startswith('_ZTVN10__cxxabiv1'):
                out.append(f'extern const void *{safe}[] __asm__("{sym}");')
            else:
                out.append(f'extern void {safe}(void) __asm__("{sym}");')
        out.append('')

    open(args.output, 'w').write('\n'.join(out + body) + '\n')
    print(f'{args.output}: {n} C++ ABI object(s)')


if __name__ == '__main__':
    main()
