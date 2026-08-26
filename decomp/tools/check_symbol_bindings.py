#!/usr/bin/env python3
"""
check_symbol_bindings.py — does each recovered object export the same INTERFACE
as the shipped one, not just the same set of names?

WHY THIS EXISTS. wasm_check.sh already compared exported symbols, and it missed
this for the whole life of the project, because it compared names and threw the
binding letter away — and it compared the recovered wasm build against the
recovered native build, never against the object being replaced. So a symbol
that shipped WEAK and came back GLOBAL passed every gate.

That is not cosmetic. gcc emits `putchar` weakly into three separate members of
libMdtKea.a (keaDebug.o, keaMatrix_tester.o, keaPrintBasicTypes.o) so that
libc's strong definition wins and none of them is ever used. The recovered
keaMatrix_tester exported it GLOBAL, which makes decompiled Karma the putchar
the entire engine calls — and it was linked and executed 901 times in a scene
census before anyone noticed. Recover two of the three and the link fails
outright.

WHAT IT CHECKS, and what it deliberately tolerates:

  binding  T <-> W        ERROR. The binding is part of the interface.
  section  .data <-> .bss OK ONLY IF the shipped bytes are all zero and no
                          relocation targets the symbol. A zero-initialised
                          object is identical in either section; a non-zero one
                          that lands in .bss has silently lost its initialiser,
                          and nothing downstream would say so.
  section  .rodata -> .data
                          WARN. Same value, no longer const. Harmless unless
                          something depends on the page being read-only.
  size                    ERROR when it shrinks: the recovered definition is
                          smaller than the storage the original callers index.

The zero-and-unrelocated test is done here rather than taken on trust, because
"I checked those by hand once" is exactly the kind of claim this project has
had to withdraw twice.

  check_symbol_bindings.py <recovered-obj-dir> <shipped-obj-dir>
"""
import os
import subprocess
import sys

SECTION_OF = {'D': '.data', 'd': '.data', 'R': '.rodata', 'r': '.rodata',
              'B': '.bss', 'b': '.bss'}
DATA_KINDS = set('DdRrBb')
CODE_KINDS = set('TtWw')


def nm_map(obj):
    """name -> (kind, size). --print-size, so a missing size reads as None."""
    out = subprocess.run(['nm', '--print-size', '--defined-only', obj],
                         capture_output=True, text=True).stdout
    syms = {}
    for line in out.splitlines():
        p = line.split()
        if len(p) == 4:
            syms[p[3]] = (p[2], int(p[1], 16), int(p[0], 16))
        elif len(p) == 3:
            syms[p[2]] = (p[1], None, int(p[0], 16))
    return syms


def section_bytes(obj, section):
    tmp = '/tmp/.kd_binding_sec.bin'
    r = subprocess.run(['objcopy', '-O', 'binary', '--only-section=' + section,
                        obj, tmp], capture_output=True)
    if r.returncode != 0 or not os.path.exists(tmp):
        return b''
    data = open(tmp, 'rb').read()
    os.unlink(tmp)
    return data


def relocated_sections(obj):
    """Sections that any relocation writes into.

    A symbol whose bytes are zero but which a relocation fills in at link time
    is NOT zero-initialised — MeMessage's handler table is exactly this shape,
    ten R_386_32 entries over an all-zero .data. Checking the bytes alone would
    call it benign."""
    out = subprocess.run(['readelf', '-rW', obj], capture_output=True,
                         text=True).stdout
    hit = {}
    current = None
    for line in out.splitlines():
        if line.startswith("Relocation section '"):
            current = line.split("'")[1]
            if current.startswith('.rel.'):
                current = current[4:]
            elif current.startswith('.rela.'):
                current = current[5:]
        elif current and line[:8].strip() and line[0].isdigit() or \
                (current and line[:1].isdigit()):
            p = line.split()
            if p and all(c in '0123456789abcdef' for c in p[0]):
                hit.setdefault(current, set()).add(int(p[0], 16))
    return hit


def is_zero_fill(obj, name, kind, value, size):
    """Would this symbol be identical in .bss? Bytes all zero AND unrelocated."""
    section = SECTION_OF.get(kind)
    if section == '.bss':
        return True
    if section is None:
        return False
    raw = section_bytes(obj, section)
    n = size if size else 4
    chunk = raw[value:value + n]
    if len(chunk) < n or any(chunk):
        return False
    relocs = relocated_sections(obj).get(section, set())
    return not any(value <= off < value + n for off in relocs)


def corpus_imports(ship_dir):
    """Every symbol any object in the corpus leaves UNDEFINED.

    This is what decides whether a missing export matters. A shipped GLOBAL that
    the recovered object does not define is a hole in the interface, but only a
    hole somebody falls into if something actually imports it — five such
    symbols exist today and NOTHING references any of them (three CxSmallSort
    C++ ABI variants, McdBoxGetXYAABB, McdTriangleListGetBoundingBox), so they
    are warnings. `gDebug` has fifteen importers, so it is an error."""
    out = set()
    for fn in sorted(os.listdir(ship_dir)):
        if not fn.endswith('.o'):
            continue
        txt = subprocess.run(['nm', '--undefined-only', os.path.join(ship_dir, fn)],
                             capture_output=True, text=True).stdout
        for line in txt.splitlines():
            parts = line.split()
            if parts:
                out.add(parts[-1])
    return out


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        return 2
    rec_dir, ship_dir = sys.argv[1], sys.argv[2]
    errors, warns, checked = [], [], 0
    imported = corpus_imports(ship_dir)

    for fn in sorted(os.listdir(rec_dir)):
        if not fn.endswith('.o'):
            continue
        rec, ship = os.path.join(rec_dir, fn), os.path.join(ship_dir, fn)
        if not os.path.exists(ship):
            continue
        checked += 1
        r, s = nm_map(rec), nm_map(ship)
        for name, (skind, ssize, svalue) in sorted(s.items()):
            if name not in r:
                # NOT "a link error, not ours" — that is what this line used to
                # say, and it is how three renamed exports got past this gate.
                # A file-wide rename went through KD_MANGLED("gDebugDataFile")
                # and keaRbdCore_unified started exporting `kd_gDebug`,
                # `kd_gDebugDataFile` and `kd_gPartition` instead of the names
                # the shipped object exports. Every per-symbol check below
                # passed, because it only ever looked at symbols present in
                # BOTH, and a name that vanished was skipped here.
                if skind.isupper():
                    (errors if name in imported else warns).append(
                        '%s: %s (%s) is EXPORTED by the shipped object and '
                        'MISSING from the recovered one%s'
                        % (fn, name, skind,
                           '' if name in imported else ' — nothing imports it'))
                continue
            rkind, rsize, _ = r[name]
            if skind == rkind:
                pass
            elif skind in CODE_KINDS and rkind in CODE_KINDS:
                errors.append('%s: %s binding %s -> %s (weak is part of the '
                              'interface)' % (fn, name, skind, rkind))
            elif skind in DATA_KINDS and rkind in DATA_KINDS:
                s_sec, r_sec = SECTION_OF.get(skind), SECTION_OF.get(rkind)
                if r_sec == '.bss' and s_sec != '.bss':
                    if is_zero_fill(ship, name, skind, svalue, ssize):
                        pass                   # identical either way
                    else:
                        errors.append('%s: %s %s -> .bss but the shipped bytes '
                                      'are not zero — the initialiser is LOST'
                                      % (fn, name, s_sec))
                elif s_sec == '.rodata' and r_sec == '.data':
                    warns.append('%s: %s .rodata -> .data (same value, no '
                                 'longer const)' % (fn, name))
                elif s_sec != r_sec:
                    warns.append('%s: %s %s -> %s' % (fn, name, s_sec, r_sec))
            else:
                errors.append('%s: %s changed kind %s -> %s'
                              % (fn, name, skind, rkind))
            if ssize and rsize and rsize < ssize and skind in DATA_KINDS:
                errors.append('%s: %s is %d bytes, shipped is %d — callers index '
                              'past the end' % (fn, name, rsize, ssize))

    for w in warns:
        print('  [ warn ] ' + w)
    for e in errors:
        print('  [ERROR ] ' + e)
    print()
    print('  objects compared        : %d' % checked)
    print('  interface errors        : %d' % len(errors))
    print('  tolerated differences   : %d' % len(warns))
    return 1 if errors else 0


if __name__ == '__main__':
    sys.exit(main())
