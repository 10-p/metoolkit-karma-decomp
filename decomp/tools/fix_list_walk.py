#!/usr/bin/env python3
"""fix_list_walk.py — a LINKED LIST walked through a four-byte cursor.

    fix_list_walk.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT. Ghidra loses the node type of a list and renders the walk through
`undefined4 *`, so both the link and the payload are read in FOUR-BYTE UNITS:

    for (puVar1 = *(undefined4 **)((kd_iptr)pvVar2 + KD_OFFSET(PElement, childHead));
         puVar1 != (undefined4 *)0x0;
         puVar1 = (undefined4 *)puVar1[1]) {                  <- `next` is at byte 4
      pMVar5 = (*MeFAssetCreateFunc[0])(i->db, i->IDPool, (PElement *)*puVar1);
    }                                                         <- `current`, TRUNCATED

At i386 that is right twice over: `PElementNode::next` really is at byte 4, and
`*puVar1` really is the whole of `current`. At LP64 `next` moves to byte 8, so
`puVar1[1]` reads the TOP HALF OF `current` and calls it the next node — and
`*puVar1` keeps only the low 32 bits of a pointer.

★ THIS IS THE SECOND FRAME THE 64-BIT VEHICLE PRODUCED, and the truncation is
visible in the crash itself. gdb caught `KaFileCreate_1_0` with

    e = 0x58cabf90

next to `parent = 0x555558ce4030` — every real pointer in that process is
`0x5555_5xxxxxxx`, so a 32-bit `e` is not a wild value, it is a measurement of
where the top half went. `fix_block_copy` had to land first for the parse to get
this far at all; before that the run died in `strcmp` inside the XML parser.

THE NODE TYPE IS DECLARED, NOT INFERRED. The cursor is initialised from a NAMED
field — `PElement::childHead` — and the oracle header says what that field is:

    struct PElement    { ...; PElementNode *childHead; ...; };
    struct PElementNode{ PElement *current; PElementNode *next; };

So the type comes from `metoolkit/include`, which is the yardstick and is never
edited, and the field at each index comes from the type database. Index 1 is
byte 4 is `next`; index 0 is `current`. A site whose cursor is initialised from
a BAKED offset instead of a named field is declined and reported — two of the
three in the corpus are, in `McdAggregate`, and they need the base typed first.

WHY THERE IS NO `#if` HERE, UNLIKE `fix_block_copy`. That pass had to guard
because at LP64 its source and destination genuinely have different numbers of
words per element, so no single spelling can serve both widths. This one does
have a single spelling: `((PElementNode *)p)->next` IS `p[1]` at i386 — same
address, same four-byte access — and is the correct eight-byte link at LP64. It
was measured, not assumed: the repair reproduces the baseline object exactly.

⚠ RUN IT ON A COPY: it edits in place.
"""
import json
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

HERE = kd_paths.MD
WORK = '/tmp/kd_listwalk'

CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types',
          '-DLINUX']

# `p = (undefined4 *)p[K];`  — the self-advance that names the link offset
ADVANCE = re.compile(
    r'(?P<p>\b\w+) = \(undefined4 \*\)(?P=p)\[(?P<k>\d+)\]')
# `p = *(undefined4 **)((kd_iptr)BASE + ((int)((char *)&((struct T *)0)->F - (char *)0)))`
INIT = re.compile(
    r'(?P<p>\b\w+) = \*\(undefined4 \*\*\)\(\(kd_iptr\)[^)]+\ \+ '
    r'\(\(int\)\(\(char \*\)&\(\((?:struct )?(?P<st>\w+) \*\)0\)->(?P<f>\w+)'
    r' - \(char \*\)0\)\)\)')
# a struct member declared as `TYPE *name;` inside `struct S { ... };`
STRUCT_BODY = re.compile(r'struct\s+(?P<s>\w+)\s*\{(?P<body>[^{}]*)\}', re.S)
MEMBER = re.compile(r'(?P<ty>\w+)\s*\*\s*(?P<n>\w+)\s*;')


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


def oracle_members(inc):
    """{struct: {member: pointed-to type}} read from the ORACLE headers.

    metoolkit/ is the yardstick and is never edited, so a declaration read from
    it is evidence rather than inference — the same standing `interaction_types`
    has for reading a registration."""
    out = {}
    for root, _, files in os.walk(inc):
        for fn in files:
            if not fn.endswith('.h'):
                continue
            try:
                txt = open(os.path.join(root, fn), errors='ignore').read()
            except OSError:
                continue
            txt = re.sub(r'/\*.*?\*/', '', txt, flags=re.S)
            for m in STRUCT_BODY.finditer(txt):
                d = out.setdefault(m.group('s'), {})
                for mm in MEMBER.finditer(m.group('body')):
                    d.setdefault(mm.group('n'), mm.group('ty'))
    return out


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')

    members = oracle_members(inc)
    # ---- THE SELF-CHECK. A header set that did not parse leaves every lookup
    # empty, every site declines, and "0 repaired" reads exactly like "there
    # were none to repair".
    if members.get('PElement', {}).get('childHead') != 'PElementNode':
        sys.exit('fix_list_walk: SELF-CHECK FAILED — PElement::childHead read '
                 'as %r, want PElementNode. The oracle headers did not parse.'
                 % members.get('PElement', {}).get('childHead'))

    tf = os.path.join(HERE, 'include', 'kd_types_fields.json')
    typedb = json.load(open(tf)) if os.path.exists(tf) else {}

    fixed = declined = 0
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(
                os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        # where each cursor was initialised, and from which named field
        inits = {m.group('p'): (m.group('st'), m.group('f'))
                 for m in INIT.finditer(text)}
        out = text
        for m in ADVANCE.finditer(text):
            p, k = m.group('p'), int(m.group('k'))
            if p not in inits:
                declined += 1
                notes.append('%-26s DECLINED: %s is not initialised from a '
                             'named field (baked offset?)' % (fn, p))
                continue
            st, fld = inits[p]
            node = members.get(st, {}).get(fld)
            if not node:
                declined += 1
                notes.append('%-26s DECLINED: no declaration for %s::%s'
                             % (fn, st, fld))
                continue
            flds = typedb.get(node) or typedb.get('_' + node)
            if not flds:
                declined += 1
                notes.append('%-26s DECLINED: %s not in the type database'
                             % (fn, node))
                continue
            link = flds.get(str(k * 4))
            head = flds.get('0')
            if not link or not head:
                declined += 1
                notes.append('%-26s DECLINED: %s has no field at byte %d'
                             % (fn, node, k * 4))
                continue
            out = out.replace('%s = (undefined4 *)%s[%d]' % (p, p, k),
                              '%s = (undefined4 *)((%s *)%s)->%s'
                              % (p, node, p, link))
            out = out.replace('*%s)' % p, '((%s *)%s)->%s)' % (node, p, head))
            notes.append('%-26s %-16s [%d] -> ->%s , *p -> ->%s'
                         % (fn, node, k, link, head))
            fixed += 1
        if out != text:
            if compiles_identically(fn, out, build, inc):
                open(path, 'w').write(out)
            else:
                fixed -= 1
                declined += 1
                notes.append('%-26s DECLINED: repair is NOT byte-identical at '
                             'i386' % fn)

    for n in notes:
        print('  ' + n)
    print('fix_list_walk: %d repaired, %d declined' % (fixed, declined))


if __name__ == '__main__':
    main()
