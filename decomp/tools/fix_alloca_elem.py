#!/usr/bin/env python3
"""fix_alloca_elem.py — an `alloca`'d ARRAY OF POINTERS, strided at four bytes.

    fix_alloca_elem.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT, and it is `LP64-ARM64-PLAYS` item (3) in the two files that entry
could not reach:

    kd_alloca_iVar3 = (char *)alloca((size_t)(group->count) * 4 + 0);   MdtLOD.c:799
    *(MdtContactID *)((kd_iptr)(kd_alloca_iVar3) + iVar8 * 4) = pMVar2;
    ...
    *(undefined4 *)(&(*kd_argslot_ffffffcc)) = 4;                       /* qsort's size */
    qsort(*(void **)(&(*kd_argslot_ffffffc4)), *(size_t *)(...),
          *(size_t *)(&(*kd_argslot_ffffffcc)), ...);

`MdtContactID` is `MdtContact *`, so the block is an array of POINTERS: `4` is
`sizeof(element)` at i386 and EIGHT at LP64. The allocation is half the size it
needs, every stride lands mid-element, and `qsort` is told its elements are four
bytes — so the comparator is handed the top half of one pointer welded to the
bottom half of the next. Nothing is truncated and no diagnostic fires.

★ `fix_baked_sizeof` DECLINED THIS BY NAME AND SAID SO, WHICH IS WHY IT IS HERE.
Its `QSORT` rule requires the base to be a plain identifier — "`MdtLOD.c`'s
`qsort(*(void **)(&(*kd_argslot_ffffffc4)), ...)` is not one, and is declined and
reported rather than rewritten through a cast whose pointee is `void`". That was
the right call: `void` has no size. The missing piece is not a better guess at
the cast, it is the ELEMENT TYPE, and this pass finds it written down.

THREE PLACES THE TYPE IS DECLARED, and a site with none of them is declined:

  c  THE CALLEE'S SIGNATURE NAMES IT, and it is asked FIRST because it is the
     oracle. `MeAssetFactory.c` casts everything to `void *`, and the block is
     argument two of `MeFAssetGetPartsSortedByName(const MeFAsset *const,
     MeFAssetPart **)`. The argument is matched through the argslot it travels in.
  a  AN ELEMENT ACCESS NAMES IT.  `*(MdtContactID *)(base + i * 4) = …`
  b  A CAST OF THE BASE ITSELF NAMES IT — never of a SLOT.
     ⚠⚠ AN ARGSLOT IS NOT EVIDENCE. `kd_argslot_ffffffa8` is cast to `MeHash **`,
     `MeFJoint **`, `char **`, `void **` AND `MeFAssetPart **` inside one
     function, because outgoing slots are reused; taking the first cast off one
     picks a type at random. Measured: it chose `MeAssetFactory *`.
     ★ And Ghidra's own element cast is not authoritative either — it reads that
     same array as `MeAssetFactory **`. Both are pointers, so the rewrite is
     numerically right under either; the NAME is not, and a pass that writes down
     the wrong type has recorded a wrong reason for a right answer.

⚠ AND `* 4` IS NOT A CLASS. Eight `alloca`s in the corpus carry a literal 4 and
several of them are honest `int` arrays — `keaMatrix_PcSparse_vanilla`'s
`numClamped * 4` is one. The element type has to be found, its i386 `sizeof` has
to EQUAL the literal, and its LP64 `sizeof` has to differ, or the site is left
exactly as it was. Two independent facts, the same standard `fix_baked_sizeof`
holds itself to.

WHAT IT REWRITES, per file, all-or-nothing, byte-identity at i386 the gate:

  A  the allocation      alloca(n * 4)           ->  n * (int)sizeof(E)
  B  every stride        base + i * 4            ->  i * (int)sizeof(E)
  C  qsort's element size, when its base is that block, through the slot

⚠ RUN IT ON A COPY: it edits in place, after `fix_ptrwidth` (which writes the
`kd_iptr` these sites are spelled with) and after `fix_frame_slots` (whose
sixth rule widens the slot `qsort`'s size argument travels in — a size stored
four bytes wide and read as a `size_t` is a SECOND defect on the same line).
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

HERE = kd_paths.MD
WORK = '/tmp/kd_allocaelem'
CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']
HEAD = ('#include "%s/include/kd_compat.h"\n#include "%s/include/kd_karma.h"\n'
        '#include "%s/include/kd_types.h"\n' % (HERE, HERE, HERE))
_SZ = re.compile(r'char \(\*\)\[(\d+)\]')
# `kd_alloca_X = (char *)alloca(<count> * K + <tail>)`, in either spelling.
ALLOCA = re.compile(r'(?P<base>kd_\w*alloca\w*|kd_blk\d+_\w+)[ \t]*=[ \t]*\(char \*\)[ \t]*'
                    r'alloca\((?P<pre>\(size_t\)\([^()]*\)|\([^()]*\)|[^*;]*?)'
                    r'[ \t]*\*[ \t]*(?P<k>0x[0-9a-fA-F]+|\d+)')
NOISE = ('void', 'undefined', 'undefined1', 'undefined2', 'undefined4',
         'undefined8', 'char', 'uchar', 'byte')
INC_G = None
CACHE = {}


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def measure(expr, bits='-m32'):
    key = (expr, bits)
    if key in CACHE:
        return CACHE[key]
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'p.c')
    open(src, 'w').write(HEAD + 'char kd_probe[sizeof(%s)];\nint kd_force = &kd_probe;\n' % expr)
    r = subprocess.run(['gcc', bits, '-DLINUX'] + includes(INC_G)
                       + ['-c', '-o', os.devnull, src], capture_output=True, text=True)
    m = _SZ.search(r.stderr)
    CACHE[key] = int(m.group(1)) if m else None
    return CACHE[key]


def compiles_identically(fn, text, build, inc):
    """⚠ THE FILE MUST KEEP ITS NAME — gcc records the basename in STT_FILE."""
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


def api_params(inc):
    """{function -> [declared parameter types]} out of the oracle headers."""
    out = {}
    for dp, _, files in os.walk(inc):
        for f in files:
            if not f.endswith('.h'):
                continue
            t = open(os.path.join(dp, f), errors='ignore').read()
            t = re.sub(r'/\*.*?\*/', ' ', t, flags=re.S)
            for m in re.finditer(r'(?<![\w])(\w+)\s*\(([^();]*)\)\s*;', t):
                ps = [p.strip() for p in m.group(2).split(',') if p.strip()]
                out.setdefault(m.group(1), ps)
    return out


def slot_of(text, base):
    """The argslot a block is stored into, so its uses can be followed."""
    m = re.search(r'\*\([^)]*\)\(&\(\*(?P<s>kd_argslot_\w+)\)\)[ \t]*=[ \t]*\(?%s\b'
                  % re.escape(base), text)
    if m:
        return m.group('s')
    m = re.search(r'\*\([^)]*\)\(&\(\*(?P<s>kd_argslot_\w+)\)\)[ \t]*=[ \t]*'
                  r'\(%s[ \t]*=' % re.escape(base), text)
    return m.group('s') if m else None


def calls(text):
    """(name, [argument texts]) for every call, with BALANCED arguments.

    ⚠ `(\\w+)\\s*\\(([^();]*)\\)` CANNOT SEE THESE CALLS AT ALL. Every argument in
    the recovered sources is itself parenthesised —
    `MeFAssetGetPartsSortedByName(*(void **)(&(*kd_argslot_ffffffa4)), …)` — so a
    class that excludes `(` matches nothing, the oracle evidence never fires and
    the pass falls through to Ghidra's own guess with no decline printed. That
    cost `MeAssetFactory` its correct element type twice."""
    out = []
    for m in re.finditer(r'(?<![\w>.])([A-Za-z_]\w*)\s*\(', text):
        i, depth = m.end() - 1, 0
        while i < len(text):
            if text[i] == '(':
                depth += 1
            elif text[i] == ')':
                depth -= 1
                if depth == 0:
                    break
            elif text[i] == ';':
                i = -1
                break
            i += 1
        if i < 0 or i >= len(text):
            continue
        inner, args, d, start = text[m.end():i], [], 0, 0
        for j, ch in enumerate(inner):
            if ch == '(':
                d += 1
            elif ch == ')':
                d -= 1
            elif ch == ',' and d == 0:
                args.append(inner[start:j].strip())
                start = j + 1
        args.append(inner[start:].strip())
        out.append((m.group(1), args))
    return out


def element_type(text, base, slot, apis):
    """The element type, from the three places it can be declared. See the
    module docstring; a base with none of them yields None and is declined.

    ⚠⚠ THE ORACLE GOES FIRST, AND AN ARGSLOT IS NOT EVIDENCE AT ALL.
    `kd_argslot_ffffffa8` in `MeAssetFactory.c` is cast to `MeHash **`,
    `MeFJoint **`, `char **`, `void **` and `MeFAssetPart **` in one function —
    a reused outgoing slot names nothing, and taking the first cast off it
    chooses at random. And Ghidra's own element cast is not authoritative
    either: it reads that array as `MeAssetFactory **` while
    `MeFAssetGetPartsSortedByName(const MeFAsset *const, MeFAssetPart **)` — the
    signature in the oracle — says `MeFAssetPart **`. Both are pointers, so the
    SIZE is the same either way and the rewrite is correct under both; the name
    is not, and a pass that writes down the wrong type has recorded a wrong
    reason for a right answer."""
    cands = []
    # (c) the callee's declared parameter type, matched through the slot the
    #     block travels in. The oracle, so it is asked first.
    if slot:
        for name, args in calls(text):
            hit = [i for i, a in enumerate(args) if slot in a]
            ps = apis.get(re.sub(r'^kd_', '', name)) or apis.get(name)
            if not hit or not ps or hit[0] >= len(ps):
                continue
            d = re.match(r'(?:const\s+)?([A-Za-z_]\w*)\s*\*\s*(?:const\s+)?\*\s*'
                         r'(?:const\s+)?\w*\s*$', ps[hit[0]])
            if d and d.group(1) not in NOISE:
                cands.append(d.group(1) + ' *')
    # (a) an element access names it — `*(T *)(base + i*K)` and `*(T **)(...)`
    for m in re.finditer(r'\*\((?P<t>[A-Za-z_]\w*)[ \t]*(?P<star>\*?)[ \t]*\*\)[ \t]*\('
                         r'\(kd_iptr\)[ \t]*\(?%s\)?[ \t]*\+' % re.escape(base), text):
        if m.group('t') not in NOISE:
            cands.append(m.group('t') + (' *' if m.group('star') else ''))
    # (b) a cast of the BASE ITSELF — never of a slot, which is shared scratch
    for m in re.finditer(r'\((?P<t>[A-Za-z_]\w*)[ \t]*\*\*\)[ \t]*\(\(kd_iptr\)[ \t]*'
                         r'\(?%s\)?\)' % re.escape(base), text):
        if m.group('t') not in NOISE:
            cands.append(m.group('t') + ' *')
    for c in cands:                      # first that measures as a real type wins
        if measure(c) is not None:
            return c
    return None


def plan(text, fn, notes, apis, quiet=False):
    edits, label = [], []
    for a in ALLOCA.finditer(text):
        base, K = a.group('base'), int(a.group('k'), 0)
        slot = slot_of(text, base)
        E = element_type(text, base, slot, apis)
        if not E:
            if not quiet:
                notes.append('%-24s %s: no declared element type — declined'
                             % (fn, base))
            continue
        s32, s64 = measure(E), measure(E, '-m64')
        if None in (s32, s64) or s32 != K or s64 == K:
            continue                     # ⚠ `* 4` IS NOT A CLASS — see the docstring
        SZ = '(int)sizeof(%s)' % E
        edits.append((a.start('k'), a.end('k'), SZ))
        for m in re.finditer(r'(?<![\w])%s\)?[ \t]*\+[ \t]*[A-Za-z_]\w*[ \t]*\*[ \t]*'
                             r'(?P<k>0x[0-9a-fA-F]+|\d+)' % re.escape(base), text):
            if int(m.group('k'), 0) == K:
                edits.append((m.start('k'), m.end('k'), SZ))
        # C: qsort's element size, through the slot its base travels in
        if slot:
            for name, args in calls(text):
                if name not in ('qsort', 'bsearch') or len(args) < 3:
                    continue
                if not any(slot in a for a in args):
                    continue
                sm = re.search(r'kd_argslot_\w+', args[2])
                if not sm:
                    continue
                for st in re.finditer(r'\*\([^)]*\)\(&\(\*%s\)\)[ \t]*=[ \t]*'
                                      r'(?P<k>0x[0-9a-fA-F]+|\d+)[ \t]*;'
                                      % re.escape(sm.group(0)), text):
                    if int(st.group('k'), 0) == K:
                        edits.append((st.start('k'), st.end('k'), SZ))
        label.append('%s -> %s' % (base, E))
    if not edits:
        return None
    edits = list({(a, b): (a, b, r) for a, b, r in edits}.values())
    return edits, '; '.join(label)


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    global INC_G
    INC_G = os.path.join(root, 'include')
    apis = api_params(INC_G)

    # ---- THE SELF-CHECK. A probe that cannot compile measures nothing, every
    # site drops out, and "0 rewritten" reads exactly like "there were none".
    if measure('MdtContactID') != 4 or measure('MdtContactID', '-m64') != 8:
        sys.exit('fix_alloca_elem: SELF-CHECK FAILED — sizeof(MdtContactID) '
                 'measured %r/%r, want 4/8.'
                 % (measure('MdtContactID'), measure('MdtContactID', '-m64')))
    if 'MeFAssetGetPartsSortedByName' not in apis:
        sys.exit('fix_alloca_elem: SELF-CHECK FAILED — the oracle signatures did '
                 'not load, so evidence (c) can never fire and would report a '
                 'clean zero.')

    done = declined = 0
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        p = plan(text, fn, notes, apis)
        if not p:
            continue
        edits, label = p
        cand = text
        for st, en, rep in sorted(edits, key=lambda x: -x[0]):
            cand = cand[:st] + rep + cand[en:]
        # ⚠ ALL OR NOTHING PER FILE: an allocation grown without its strides, or
        # strides without the allocation, is a different wrong rather than none.
        if compiles_identically(fn, cand, build, INC_G):
            open(path, 'w').write(cand)
            done += len(edits)
            notes.append('%-24s %s  %d edit(s)' % (fn, label, len(edits)))
        else:
            declined += 1
            notes.append('%-24s %s  DECLINED: not byte-identical at i386'
                         % (fn, label))
    for x in notes:
        print('  ' + x)
    print('fix_alloca_elem: %d edit(s), %d declined' % (done, declined))


if __name__ == '__main__':
    main()
