#!/usr/bin/env python3
"""amd64_oracle.py — read the LP64 layouts off a 64-bit build of the SAME source.

    amd64_oracle.py --list
    amd64_oracle.py --object McdSphere
    amd64_oracle.py --function McdSphereGetRadius
    amd64_oracle.py --consts McdInit
    amd64_oracle.py --selftest

WHAT THIS IS FOR.

Everything else in this project reads the i386 build, because the i386 build is
all there was. That is why the recovery encodes 32-BIT STRUCT LAYOUTS and why
`layout_check.py` can only BOUND the arm64 job: 128 of 151 structs change size at
64-bit pointer width, `sizeof(_McdGeometry)` is 16 here and 32 there, and a
hardcoded offset then addresses somebody else's memory with nothing truncated and
clang silent.

`metoolkit/lib.rel/win_amd64_single/*.lib` in this repository are a 64-bit build
of the same MathEngine source, out of the UT2004 v3369 drop. They are real x86-64
COFF, NOT stripped: 1,089 symbols over 189 members in 15 archives. Their field
offsets and their allocation constants ARE the 64-bit layouts. That converts
"discover 128 layouts by running ASan until it stops crashing" into "read them
off a binary that has them".

    McdSphereGetRadius   i386   flds  0x10(%edx)
                         amd64  movss 0x20(%rcx),%xmm0

★ THE ONE THING THAT WILL BITE YOU: IT IS LLP64, NOT LP64.

The shipped 64-bit build is MSVC for Windows, where `long` is FOUR bytes. Android
and Linux are LP64, where it is eight. So a struct containing a `long` is a
different size in the two, and **the constants read out of this library are the
right oracle for WHICH TYPE a site means and the wrong number to paste into the
source**:

    sizeof(MdtBody)    i386 576    win64 696    linux64 704
    sizeof(MdtWorld)   i386 564    win64 856    linux64 880

The repair is therefore always `sizeof(T)` — which the compiler recomputes per
target — and this library's job is to say what T is. `x86_64-w64-mingw32-gcc`
reproduces the win64 column EXACTLY (856 and 696, verified against the immediates
in `MdtWorldCreate`), which is what makes that check mechanical rather than a
matter of opinion: if a candidate type's MinGW `sizeof` equals the constant the
shipped amd64 code passes, that is the type.

⚠ USE THE `.lib` MEMBERS, NOT THE LOOSE `.obj`. The `*.obj` files sitting BESIDE
the archives in the same directory are MSVC LTCG (magic `0000ffff...6486`) and
disassemble to nothing. The first pass over that directory reported it unusable
on the strength of those.

⚠ THERE IS NO TYPE INFORMATION IN HERE — I CHECKED, AND THE SECTION EXISTS. Every
member carries a `.debug$T`, which is where CodeView puts struct layouts, and
reading one out would have made this whole tool unnecessary. They are all 0x40 to
0x44 bytes: a `/Zi` build, whose types live in a PDB that was not shipped. The
section being present is not the section being populated.

⚠ `objdump --disassemble=NAME` PRINTS NOTHING HERE. The objects are compiled
`/Gy`, so every function is its own COMDAT and every one of those sections is
called `.text` at VMA 0. Selecting by name or by section both fail; the whole
object must be disassembled and split on the `<name>:` labels, which is what this
does. C symbols are plain, C++ symbols are MSVC-mangled (`?t@lsTransform@@...`).

WHAT IT CANNOT DO. It reads code, so it can only see a field an instruction
touches; a field nothing in this archive reads has no entry here. And MSVC x64
schedules instructions freely, so the ORDER of the constants in a function is not
the order of the arguments — membership is the check, position is not.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths

# The 64-bit half of the SDK, which ships in this repository (18 MB — the note
# that used to sit here calling it an unvendorable 1.4 GB was measuring the whole
# v3369 tree, not this directory). Overridable all the same.
LIBDIR = kd_paths.AMD64_LIB
WORK = os.environ.get('KD_AMD64_WORK', '/tmp/kd_amd64lib')

_CACHE = {}


def extract():
    """Unpack every .lib member once. Returns {member-stem: path}."""
    if _CACHE:
        return _CACHE
    if not os.path.isdir(LIBDIR):
        sys.exit('amd64_oracle: no library at %s — set KD_AMD64_LIB.' % LIBDIR)
    libs = sorted(f for f in os.listdir(LIBDIR) if f.endswith('.lib'))
    if not libs:
        sys.exit('amd64_oracle: %s holds no .lib archives. The loose .obj files\n'
                 '  there are MSVC LTCG and disassemble to nothing — see the header.'
                 % LIBDIR)
    for lib in libs:
        d = os.path.join(WORK, lib[:-4])
        if not os.path.isdir(d):
            os.makedirs(d, exist_ok=True)
            subprocess.run(['ar', 'x', os.path.join(LIBDIR, lib)], cwd=d,
                           capture_output=True)
        for m in sorted(os.listdir(d)):
            if m.endswith('.obj'):
                # `version.obj` is in every archive; keep the first and qualify
                # the rest, so a lookup by stem stays unambiguous.
                stem = m[:-4]
                _CACHE.setdefault(stem, os.path.join(d, m))
                _CACHE['%s/%s' % (lib[:-4], stem)] = os.path.join(d, m)
    return _CACHE


# ★ THE CHECK BUILD, AND IT IS THE BETTER ORACLE FOR LAYOUT.
# `lib.chk/win_amd64_whidbey_single/*.obj` are ALSO real pe-x86-64 (magic `6486`,
# unlike the LTCG `.obj` beside the release archives) and they are UNOPTIMISED —
# one function per COMDAT, every struct access a literal displacement, and
# nothing folded or inlined. That matters for the layout question specifically:
#
#   McdBoxMaximumPointNew   release .lib   touches 0,4,8,16,20,24,32,36,40,48,…
#                           check .obj     touches 32,36,40
#
# The release build's set is so rich that several candidate structs fit it and
# the answer becomes ambiguous; the check build names exactly the three floats
# the source reads. Prefer it, and fall back to the archives for anything the
# check directory does not carry.
CHKDIR = os.environ.get(
    'KD_AMD64_CHK',
    os.path.join(kd_paths.METOOLKIT_DIR, 'lib.chk', 'win_amd64_whidbey_single'))
_CHK = {}


def extract_chk():
    """{member-stem: path} for the unoptimised check build, if it is present."""
    if _CHK or not os.path.isdir(CHKDIR):
        return _CHK
    for m in sorted(os.listdir(CHKDIR)):
        if m.endswith('.obj'):
            _CHK[m[:-4]] = os.path.join(CHKDIR, m)
    return _CHK


def find_function(fnname):
    """(symbol, lines) for `fnname` as a SUBSTRING, check build first.

    ⚠ SUBSTRING, NOT AN EXACT KEY: C symbols are plain but C++ ones are
    MSVC-mangled (`?McdBoxMaximumPointNew@@YAXPEAU_McdGeometryInstance@@QEBMQEAM@Z`),
    so `find()`'s exact lookup answers for about half the corpus."""
    for table in (extract_chk(), extract()):
        for stem, path in sorted(table.items()):
            if '/' in stem:
                continue
            for name, lines in disasm(path).items():
                if fnname in name:
                    return name, lines
    return None, None


def disasm(objpath):
    """objdump -d, split into {function-name: [lines]}."""
    r = subprocess.run(['objdump', '-d', objpath], capture_output=True, text=True)
    out, cur = {}, None
    for line in r.stdout.split('\n'):
        m = re.match(r'^[0-9a-f]+ <(.+)>:$', line.strip())
        if m:
            cur = m.group(1)
            out[cur] = []
        elif cur and line.strip():
            out[cur].append(line)
    return out


def find(function):
    """(member-path, [lines]) for the object that defines this function."""
    for stem, path in sorted(extract().items()):
        if '/' in stem:
            continue
        d = disasm(path)
        if function in d:
            return path, d[function]
    return None, None


# `mov $0x1c,%r8d` and `lea 0x50(%r12),%r8d` are the SAME argument in two
# schedulings — MSVC keeps a zeroed register around and reaches small constants
# through it. Both spellings have to count or the confirmation misses half the
# call sites it exists to confirm.
#
# ★ THE DESTINATION WIDTH IS WHAT SEPARATES A SIZE FROM AN ADDRESS, and reading
# every `lea` cost a real false positive: `lea 0x68(%rbp),%rcx` in `McdInit` is
# `&framework->modelPool`, and counting 0x68 as a constant made 104 "a size this
# function passes", which made `RPerformanceBar` a candidate for a pool whose
# elements are `McdGeometryInstance` and turned a clean answer into an ambiguity.
# An integer materialised out of a zeroed register has a 32-BIT destination
# (`%r8d`, `%eax`); an address computation has a 64-bit one (REX.W, `%rcx`).
IMM = re.compile(r'\$0x([0-9a-f]+)')
LEA = re.compile(r'\slea\s+(-?)(?:0x)?([0-9a-f]+)\(%([a-z0-9]+)\),%([a-z0-9]+)\b')
MOVI = re.compile(r'\smov\s+\$0x([0-9a-f]+),%([a-z0-9]+)\b')
XORZ = re.compile(r'\sxor\s+%([a-z0-9]+),%([a-z0-9]+)\b')
DISP = re.compile(r'(?:^|[\s,])(?:0x)?([0-9a-f]+)\(%r[a-z0-9]+\)')
DST32 = re.compile(r'^(?:e[a-z]{2}|r\d+d)$')

# `%eax` and `%rax` are the same register; MSVC materialises a small integer in
# the 32-bit half and reads the 64-bit one two instructions later.
_WIDE = {'eax': 'rax', 'ebx': 'rbx', 'ecx': 'rcx', 'edx': 'rdx', 'esi': 'rsi',
         'edi': 'rdi', 'ebp': 'rbp', 'esp': 'rsp'}


def _canon(r):
    if r in _WIDE:
        return _WIDE[r]
    if re.match(r'^r\d+d$', r):
        return r[:-1]
    return r


def constants(lines):
    """Every integer constant this function materialises, as a set.

    ★ THE HALF THAT NEEDS DATAFLOW, and leaving it out declined a real site.
    `McdCacheHello` initialises its pool with

        mov  $0x64,%edx           poolSize   = 100
        lea  -0x1c(%rdx),%r8d     structSize =  72
        lea  -0x54(%rdx),%r9d     alignment  =  16

    — MSVC parks one constant in a register and reaches the other two by
    ARITHMETIC ON IT, including negative displacements. `sizeof(McdCache)` at
    64-bit is 72 and the number 72 appears NOWHERE in the instruction stream.
    A pattern match over immediates reports "this function passes no such size",
    which reads exactly like "that is not the type" and is not the same claim.
    So this constant-propagates `mov $imm` and `xor r,r` forward, conservatively:
    any other write to a register forgets it."""
    out, val = set(), {}
    for l in lines:
        body = l.split('\t')[-1] if '\t' in l else l
        body = ' ' + body.strip()
        for m in IMM.finditer(body):
            out.add(int(m.group(1), 16))
        m = MOVI.search(body)
        if m:
            val[_canon(m.group(2))] = int(m.group(1), 16)
            continue
        m = XORZ.search(body)
        if m and m.group(1) == m.group(2):
            val[_canon(m.group(1))] = 0
            continue
        m = LEA.search(body)
        if m:
            sign, disp, src, dst = m.group(1), int(m.group(2), 16), m.group(3), m.group(4)
            base = val.get(_canon(src))
            # A 32-BIT DESTINATION is what separates a size from an address:
            # `lea 0x50(%r12),%r8d` (r12 == 0) is the integer 80, and
            # `lea 0x68(%rbp),%rcx` is `&framework->modelPool`. Counting the
            # second made 104 "a size McdInit passes" and turned one clean
            # answer into a two-way ambiguity.
            if base is not None and DST32.match(dst):
                out.add(base + (-disp if sign else disp))
            val.pop(_canon(dst), None)
            continue
        # anything else that writes a register forgets what was in it
        dst = body.rsplit(',', 1)[-1].strip()
        if dst.startswith('%'):
            val.pop(_canon(dst[1:]), None)
    return out


def displacements(lines):
    """Every struct-field byte offset this function touches, as a set."""
    out = set()
    for l in lines:
        body = l.split('\t')[-1] if '\t' in l else l
        if ' lea ' in ' ' + body:
            continue
        for m in DISP.finditer(body):
            out.add(int(m.group(1), 16))
    return out


def field_displacements(lines):
    """`displacements`, minus the ones that are not struct fields at all.

    ⚠ THE STACK BASES SWAMP AN UNOPTIMISED BUILD. `displacements()` accepts any
    `%r..` base, which is right for the release archives where locals live in
    registers — but the CHECK build spills everything, so `0x20(%rsp)` and
    `0x8(%rbp)` outnumber the real field reads and the set stops discriminating
    between candidate structs. `%rip` is the literal pool. None of the three can
    be a field of an object, so none of them belongs in a layout answer."""
    out = set()
    for l in lines:
        body = l.split('\t')[-1] if '\t' in l else l
        if ' lea ' in ' ' + body:
            continue
        for m in re.finditer(r'(?:^|[\s,])(?:0x)?([0-9a-f]+)\(%(r[a-z0-9]+)\)', body):
            if m.group(2) in ('rsp', 'rbp', 'rip'):
                continue
            out.add(int(m.group(1), 16))
    return out


# ---- THE SELF-CHECK. Three facts this file asserts in prose, asked of the
# binary. Without it a wrong LIBDIR, a broken objdump or an empty archive all
# report "nothing found", which reads exactly like "there is nothing to find".
SELFTEST = [
    # function,             what,                  want
    ('McdSphereGetRadius', 'displacement', 0x20),   # i386 reads 0x10 here
    ('McdSphylGetRadius',  'displacement', 0x20),
    ('MdtWorldCreate',     'constant',     0x2b8),  # sizeof(MdtBody) at win64
    ('MdtWorldCreate',     'constant',     0x358),  # sizeof(MdtWorld) at win64
    # …and one that ONLY the constant propagation above can see: 72 is
    # sizeof(McdCache) at 64-bit and it is reached as `lea -0x1c(%rdx)` with
    # %rdx == 100. A regex over immediates cannot find it.
    ('McdCacheHello',      'constant',     0x48),
]


def selftest():
    bad = 0
    for fn, kind, want in SELFTEST:
        path, lines = find(fn)
        if not lines:
            print('  %-22s NOT FOUND' % fn)
            bad += 1
            continue
        have = constants(lines) if kind == 'constant' else displacements(lines)
        ok = want in have
        print('  %-22s %-12s 0x%-6x %s' % (fn, kind, want, 'ok' if ok else 'MISSING'))
        bad += 0 if ok else 1
    if bad:
        print('\n  SELF-CHECK FAILED (%d) — nothing this tool says is worth reading.'
              % bad)
    else:
        print('\n  self-check ok: the archive is readable and the LP64 offsets are in it.')
    return 1 if bad else 0


def main():
    a = sys.argv[1:]
    if not a or a[0] in ('-h', '--help'):
        print(__doc__)
        return 0
    if a[0] == '--selftest':
        return selftest()
    if a[0] == '--list':
        seen = set()
        for stem, path in sorted(extract().items()):
            if '/' in stem or stem in seen:
                continue
            seen.add(stem)
            print('  %-34s %s' % (stem, os.path.relpath(path, WORK)))
        print('  -> %d member(s)' % len(seen))
        return 0
    if a[0] == '--object':
        path = extract().get(a[1])
        if not path:
            sys.exit('amd64_oracle: no member %r (try --list)' % a[1])
        for fn, lines in disasm(path).items():
            print('%s  disp=%s' % (
                fn, ' '.join('0x%x' % d for d in sorted(displacements(lines)))))
        return 0
    if a[0] == '--function':
        path, lines = find(a[1])
        if not lines:
            sys.exit('amd64_oracle: %s defined in no member' % a[1])
        print('/* %s */' % os.path.relpath(path, WORK))
        print('\n'.join(lines))
        return 0
    if a[0] == '--consts':
        path, lines = find(a[1])
        if not lines:
            sys.exit('amd64_oracle: %s defined in no member' % a[1])
        print('  %s  (%s)' % (a[1], os.path.relpath(path, WORK)))
        print('    constants     : %s'
              % ' '.join('0x%x' % c for c in sorted(constants(lines))))
        print('    displacements : %s'
              % ' '.join('0x%x' % d for d in sorted(displacements(lines))))
        return 0
    sys.exit('amd64_oracle: unknown mode %r' % a[0])


if __name__ == '__main__':
    sys.exit(main())
