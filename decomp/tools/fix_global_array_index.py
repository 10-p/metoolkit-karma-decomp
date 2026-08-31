#!/usr/bin/env python3
"""fix_global_array_index.py — a GLOBAL array of POINTERS indexed by a baked byte offset.

    fix_global_array_index.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT. Ghidra renders `TABLE[k]` on a file-scope array as byte arithmetic
against the symbol, with the byte count frozen at the i386 build's pointer size:

    kd_mask = *(int *)((*(kd_iptr *)((char *)&McdGjkBinarySubset + 0x3c)) + 4 + i * 4);

`McdGjkBinarySubset` is declared `const int *McdGjkBinarySubset[16]` — an array
of POINTERS. At i386 `0x3c` is 60 is element 15, which is the entry for the full
four-point simplex and the only one this code ever wants. At LP64 the elements
are eight bytes, so byte 60 is not an element at all: it straddles elements 7 and
8, and an eight-byte load there returns

    (element[8] & 0xffffffff) << 32  |  (element[7] >> 32)

— the low half of one pointer spliced onto the high half of another. Both halves
are real, so nothing is truncated, nothing is uninitialised, and no diagnostic
fires; the value is simply somebody else's address with its words exchanged.

★ THIS IS THE ARM64 ONSLAUGHT CRASH, AND THE FAULT ADDRESS SPELLS IT OUT. The
owner played an ONS match on a OnePlus 6; the vehicles landed correctly and then:

    Fatal signal 11 (SIGSEGV), SEGV_MAPERR, fault addr 0x40546a00000075
    #00 McdGjkFaceQueueInit  #01 McdGjkPenetrationDepth  #02 McdGjkTest
    #03 McdGjkCgIntersect2   #04 McdGjkCgIntersect       #05 KIntersect

`libUT2004.so` is mapped around `0x75_00000000` on Android, so a table entry
looks like `0x00000075_0040546a`. Its low half is `0x0040546a`, its high half is
`0x00000075`, and the faulting address is those two words the wrong way round —
`0x0040546a_00000075`. That is not a resemblance, it is the splice this defect
performs, read straight off the tombstone.

★★ AND THE REPAIR IS CONFIRMED AGAINST MATHENGINE'S OWN 64-BIT BUILD, not
inferred. `metoolkit/lib.rel/win_amd64_single/McdConvex.lib` is a 64-bit build of
the same source (see `amd64_oracle.py`), and both references in it read:

    160: 48 8b 05 78 00 00 00   mov rax, QWORD PTR [rip+0x78]   <- McdGjkBinarySubset + 0x78
    16f: 8b 7c 28 04            mov edi, DWORD PTR [rax+rbp*1+0x4]

`0x78` is 120 is `15 * 8`, against our `0x3c` = `15 * 4`, in BOTH
`McdGjkFaceQueueInit` and `McdGjkPenetrationDepth` — the same two sites this pass
repairs. Two for two, read off a binary rather than argued.

THE REPAIR is the element, not the byte:

    (char *)&NAME + K     ->     (char *)&NAME[K / 4]

which is a no-op at i386 BY CONSTRUCTION — `sizeof(NAME[0])` is 4 there, so
`&NAME[K/4]` is the same address `+ K` names — and is `K/4 * 8` at LP64. It is
also a no-op on wasm32 for the same reason, and that is measured here rather than
argued: see `wasm_unchanged` below.

WHAT COUNTS AS A SITE, and the rule is deliberately narrow.

  * the symbol must be DECLARED as an array with a POINTER element type, either
    in the file being repaired or in the oracle headers. `sizeof(T *)` is 4 at
    i386 and 8 at LP64 on every target this project ships, so the element size is
    known without inference. An array of non-pointers, or a symbol that is not an
    array at all, is out of scope and is reported as such;
  * `K` must be an exact multiple of 4 — the i386 element size — and land inside
    the declared extent. A `K` that is not a multiple of 4 was never an element
    index and a `K` past the end is not one either.

⚠ `MeProfile_linux.c` IS WHY THE ARRAY TEST IS NOT OPTIONAL. It carries six of
these — `(char *)&frameTime + 0x4` through `+ 0x14`, and `(char *)&clockSpeed +
0x4` — and every one of them is CORRECT. `frameTime` is an
`MeProfileTimerResult`, a struct of six `MeI32`s, and `clockSpeed` is an `MeI64`
whose `+ 4` is its own high word. Neither is an array, neither holds a pointer,
and neither moves at LP64. A pass that keyed on the SHAPE rather than on the
declaration would have "repaired" all six.

⚠ THE SECOND SITE IS A VTABLE, AND `fix_vtable_offsets` CANNOT SEE IT.
`CxSmallSort.c` writes its address point as

    #define PTR__CxSmallSort_00011f20 (*(void **)((char *)&kd_ZTV11CxSmallSort + 0x8))
    this->_vptr_CxSmallSort = (_func_int_varargs **)&PTR__CxSmallSort_00011f20;

An Itanium vtable is `[offset-to-top, typeinfo, slot0, ...]` in pointer-sized
words, so the address point is `2 * sizeof(void *)` — 8 here, 16 there. That pass
knows the arithmetic; its DISCRIMINATOR is a local into which the corpus stores
an address point (`vanillaFunctions`, `vanillaAMatrix`, `vanillaQMatrix`), and
this one is a macro over the vtable symbol with no such local. At LP64 the vptr
ends up at `&vtable[1]`, which is the typeinfo pointer, and `CxSmallSort.c:287`
does `(**this->_vptr_CxSmallSort)(this)` — a call into `_ZTI11CxSmallSort`.
It is latent on x86-64 only because that destructor path is not taken; the
declaration is right here in the file and needs no special case.

⚠ RUN IT ON A COPY: it edits in place.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

HERE = kd_paths.MD
WORK = '/tmp/kd_globalarray'

I386_PTR = 4          # the shipped build's pointer size, and so its element size

CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types',
          '-DLINUX']

EMCC = os.path.join(os.path.expanduser('~'), 'emsdk', 'upstream', 'emscripten', 'emcc')

# `(char *)&NAME + 0x3c`  /  `(char *)&NAME + 60`
SITE = re.compile(r'\(char \*\)&(?P<n>\w+) \+ (?P<k>0x[0-9a-fA-F]+|\d+)\b')


def decl_re(name):
    """`... TYPE *NAME[N]` — an array whose element is a pointer.

    The stars are REQUIRED. That is the whole evidence: an element that is a
    pointer is 4 bytes at i386 and 8 at LP64 on every target here, so the
    element size needs no inference and no probe."""
    return re.compile(
        r'(?:^|[;{}\n])[^;{}\n]*?\b\w+\s*\*+\s*' + re.escape(name)
        + r'\s*\[\s*(?P<n>\d+)\s*\]', re.M)


def array_extent(name, texts):
    """Declared extent of NAME if it is an array of pointers, else None."""
    rx = decl_re(name)
    for t in texts:
        m = rx.search(t)
        if m:
            return int(m.group('n'))
    return None


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def compiles_identically(fn, text, build, inc):
    """Does this source still produce the baseline .o, byte for byte?

    ⚠ The file has to keep its NAME: gcc records the source basename in an
    STT_FILE symbol, so identical text from a scratch name is a different
    object and reads exactly like a codegen change."""
    ref = os.path.join(build, fn[:-2] + '.o')
    if not os.path.exists(ref):
        return False
    d = os.path.join(WORK, 'ident')
    os.makedirs(d, exist_ok=True)
    src = os.path.join(d, fn)
    open(src, 'w').write(text)
    obj = os.path.join(d, fn[:-2] + '.probe.o')
    if subprocess.run(['gcc'] + CFLAGS + ['-I' + os.path.join(HERE, 'include')]
                      + includes(inc) + ['-c', '-o', obj, src],
                      capture_output=True).returncode:
        return False
    return open(ref, 'rb').read() == open(obj, 'rb').read()


def wasm_unchanged(fn, before, after, inc):
    """Does the rewrite leave the wasm32 object identical too?

    ⚠⚠ THE i386 ACCEPTANCE TEST CANNOT SPEAK FOR wasm32 — i386 is gcc and
    wasm32 is clang, and `fix_word_indexed_struct` has already found a repair
    that is byte-identical under one and not the other (proven.txt
    LP64-TWO-COMPILERS). The web artifact ships, so gcc alone does not certify.
    There is no wasm baseline to diff against, so this compares the candidate
    against the text it replaces. If emcc is missing the site DECLINES."""
    if not os.path.exists(EMCC):
        return False
    d = os.path.join(WORK, 'wasm')
    os.makedirs(d, exist_ok=True)
    objs = []
    for tag, txt in (('a', before), ('b', after)):
        src = os.path.join(d, tag + '_' + fn)
        open(src, 'w').write(txt)
        obj = os.path.join(d, tag + '.o')
        if subprocess.run(
                [EMCC, '-O2', '-g0', '-c', '-std=gnu99', '-fno-strict-aliasing',
                 '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types',
                 '-DLINUX', '-I' + os.path.join(HERE, 'include')] + includes(inc)
                + ['-o', obj, src], capture_output=True).returncode:
            return False
        objs.append(open(obj, 'rb').read())
    return objs[0] == objs[1]


def oracle_text(inc):
    """Every oracle header concatenated, comments stripped.

    metoolkit/ is the yardstick and is never edited, so a declaration read from
    it is evidence rather than inference."""
    parts = []
    for root, _, files in os.walk(inc):
        for fn in files:
            if fn.endswith('.h'):
                try:
                    t = open(os.path.join(root, fn), errors='ignore').read()
                except OSError:
                    continue
                parts.append(re.sub(r'/\*.*?\*/', '', t, flags=re.S))
    return '\n'.join(parts)


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')

    oracle = oracle_text(inc)

    # ---- THE SELF-CHECK. A header set that did not parse leaves every lookup
    # empty, every site out of scope, and "0 repaired" reads exactly like "there
    # was nothing to repair" — the failure mode this project has hit four times.
    n = array_extent('McdGjkBinarySubset', [oracle])
    if n != 16:
        sys.exit('fix_global_array_index: SELF-CHECK FAILED — McdGjkBinarySubset '
                 'read as extent %r, want 16 pointers. The oracle headers did '
                 'not parse.' % n)

    fixed = skipped = declined = 0
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        out = text
        for m in SITE.finditer(text):
            name, ktxt = m.group('n'), m.group('k')
            k = int(ktxt, 0)
            ext = array_extent(name, [text, oracle])
            if ext is None:
                skipped += 1
                notes.append('%-26s %-24s +%-6s out of scope: not a declared '
                             'array of pointers' % (fn, name, ktxt))
                continue
            if k % I386_PTR or not 0 <= k // I386_PTR < ext:
                declined += 1
                notes.append('%-26s %-24s +%-6s DECLINED: not an element index '
                             'in [0,%d)' % (fn, name, ktxt, ext))
                continue
            idx = k // I386_PTR
            # replace-all: the same table entry is reached from more than one
            # function in a file, and every occurrence is the same defect.
            n_occ = out.count(m.group(0))
            cand = out.replace(m.group(0), '(char *)&%s[%d]' % (name, idx))
            if cand == out:
                continue                      # already rewritten by an earlier match
            if (compiles_identically(fn, cand, build, inc)
                    and wasm_unchanged(fn, out, cand, inc)):
                out = cand
                fixed += n_occ
                notes.append('%-26s %-24s +%-6s -> &%s[%d]   (i386 %d, LP64 %d) '
                             '%d occurrence(s)'
                             % (fn, name, ktxt, name, idx, k, idx * 8, n_occ))
            else:
                declined += 1
                notes.append('%-26s %-24s +%-6s DECLINED: not byte-identical at '
                             'i386 OR wasm32' % (fn, name, ktxt))
        if out != text:
            open(path, 'w').write(out)

    for t in notes:
        print('  ' + t)
    print('fix_global_array_index: %d occurrence(s) repaired, %d declined, '
          '%d out of scope' % (fixed, declined, skipped))


if __name__ == '__main__':
    main()
