#!/usr/bin/env python3
"""fix_vtable_offsets.py — an Itanium-ABI VTABLE addressed in 4-byte words.

    fix_vtable_offsets.py <kd_out/allobj> <kd_build> [metoolkit-root]

THE DEFECT, and it is two halves of one thing. Ghidra decompiles a virtual call
as pointer arithmetic on the vptr, in bytes, with the byte counts frozen at the
i386 build's pointer size:

    (*(char **)&vanillaFunctions) = (char *)((char *)&vtable_for_keaFunctions_Vanilla[0] + 8);
    (**(void (**)(void *))((char *)(*(char **)&vanillaFunctions) + 16))(&vanillaFunctions);

An Itanium vtable is `[offset-to-top, typeinfo, slot0, slot1, ...]` in
POINTER-SIZED words, so the address point is `2 * sizeof(void *)` — 8 here, 16
at LP64 — and slot n is at `n * sizeof(void *)` — 4n here, 8n there.

★ WHAT IT DOES AT LP64, WORKED THROUGH, because the symptom names the wrong
function and the wrong file. `+ 8` leaves the vptr at `&vtable[1]`, one word
short; `+ 16` from there is `&vtable[3]`, and `vtable[3]` is
`calcIworldandNonInertialForceandVhmf`. So the call meant for `platformInit`,
which takes no arguments, enters a seven-argument function instead. It reads its
`blist` parameter out of a register nobody set and the sanitizer reports a
stack-buffer-underflow in the CALLEE:

    #0 keaFunctions_Vanilla::calcIworldandNonInertialForceandVhmf ...:52
    #1 MdtKeaAddConstraintForces  keaRbdCore_unified.c:163

and line 163 is the `platformInit` call. The arithmetic predicts the observed
callee exactly, which is what makes this a measurement rather than a suspicion.

THE REPAIR substitutes the constant and nothing else — the same shape as
`fix_literal_offsets.py`, for the same reason: re-spelling the expression is
what breaks byte-identity, and replacing a literal with a constant expression
that folds to it cannot.

    + 8    (address point)  ->  + (2 * (int)sizeof(void *))
    + 16   (slot 4)         ->  + (4 * (int)sizeof(void *))

The slot INDEX is derived, not read off a table: the shipped build is i386, so a
byte offset of K is slot K/4, and a K that is not a multiple of 4 is not a slot
offset at all and is declined.

WHICH `+ K` IS A VTABLE SLOT — the discriminator, and it is evidence rather than
a pattern. A local is a polymorphic object exactly when the corpus stores a
vtable address point into its first word:

    (*(char **)&vanillaQMatrix) = (char *)((char *)&vtable_for_keaMatrix_pcSparse_vanilla[0] + 8);

so `(*(char **)&vanillaQMatrix) + K` is a slot lookup and any other `+ K` is
not. Three objects in this corpus qualify — `vanillaFunctions`,
`vanillaAMatrix`, `vanillaQMatrix` — and all thirteen slot sites are on them.

⚠ AND THE VTABLE ITSELF IS FINE. `gen_vtables.py` emits `const void *[N]`, which
is pointer-sized on every target; only the arithmetic against it was frozen. The
`_ZTI` typeinfo objects carry the same `+ 8` for their own vptr and get the same
repair.

⚠ NOTHING IN THE GATE SET CAN SEE THIS. There is no truncation, no struct
changes size, and `code_call_check.py` reads 0 because these are called through
a concrete `void (**)(...)` prototype rather than through `code`. A call that
reaches the WRONG function is exactly the case that tool's own docstring records
it cannot see.

Run it on a COPY: it edits in place.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fix_narrow_pointers as FNP           # noqa: E402  (type resolution, reused)

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = '/tmp/kd_vtable'

# `&SYM[0] + 8` where SYM is a vtable — the ADDRESS POINT.
APOINT = re.compile(r'&\s*(?P<sym>(?:vtable_for_|kd_ZTV|kd_ext__ZTV)\w+)\s*\[0\]\s*\+\s*'
                    r'(?P<off>\d+)')
# ⚠ THE SEARCH HAS TO RECOGNISE ITS OWN OUTPUT. Run twice on the same tree the
# pattern above finds nothing, because the first run replaced every literal —
# and "not one address point in the corpus" is the loudest possible way to say
# "already done". This is what the refusal counts.
APOINT_ANY = re.compile(r'&\s*(?:vtable_for_|kd_ZTV|kd_ext__ZTV)\w+\s*\[0\]\s*\+')
# `(*(T **)&OBJ) = ... ` — a vptr store, which is what says OBJ is polymorphic.
VPTR_STORE = re.compile(r'\(\*\((?:char|undefined4|void)\s*\*\*\)&(?P<obj>\w+)\)\s*=[^;]*'
                        r'&\s*(?:vtable_for_|kd_ZTV|kd_ext__ZTV)\w+\s*\[0\]')
# `(char *)(*(T **)&OBJ) + K` — a SLOT lookup off that vptr.
# ⚠ THE ALTERNATION MUST MATCH `VPTR_STORE`'s, AND FOR A SESSION IT DID NOT.
# This read `char **` only while the store accepted `char|undefined4|void`, so on
# `keaLCPSolver.c` — where Ghidra spells the store `(*(undefined4 **)&vanillaQMatrix)`
# — the ADDRESS POINT was scaled and the eight SLOT reads were not. Half-applying
# this repair is worse than not applying it: with the address point at `&vtable[2]`
# and the slots still four bytes apart, `+ 8` reaches slot ONE instead of slot two,
# so `makeFromColMajorPSM` (nine arguments, called with nine) dispatched into
# `makeFromJMJT` (seven), and `+ 12` is not even a slot boundary at LP64. The
# ragdoll scene segfaults at `keaMatrix_PcSparse_vanilla.c:303` writing through a
# NAZ entry that the wrong callee never filled in. `paired_objs` below refuses the
# case rather than leaving it half-done.
VSLOT = re.compile(r'\(char \*\)\(\*\((?:char|undefined4|void)\s*\*\*\)&(?P<obj>\w+)\)'
                   r'\s*\+\s*(?P<off>\d+)')
# The same store, but only the ones whose address point this pass has scaled (or
# will scale). A slot read off an object with no scaled store is not this defect.
VSLOT_ANY = re.compile(r'\(\*\((?:char|undefined4|void)\s*\*\*\)&(?P<obj>\w+)\)'
                       r'\s*\+\s*(?P<off>\d+)\b')

# ★ AND A SECOND DISPATCH SPELLING, which `kd_types.h`'s own note warns about
# and which this tool did not read for a session:
#
#     (**(void (**)(keaMatrix *, MeReal *, const MeReal *))(*(int *)A + 0x10))(A, this->x, b);
#
# The object arrives as a POINTER rather than a local, so there is no vptr store
# to key on; Ghidra reads the vptr as an `int` — which truncates at LP64 before
# the slot arithmetic even happens — and `0x10` is slot 4 in i386 words. It is
# ONE site in this corpus and it is the one both `scene_chain` and
# `scene_ragdoll` were still failing on, intermittently, through `solveLCP`.
#
# The evidence that `A` is polymorphic is its own declaration: the struct its
# type resolves to begins with a `code **_vptr_...`. A struct that does not is
# not dispatching, and `*(int *)contact2 + 0x178` — which has the identical
# shape — is a field read and correctly left to fix_literal_offsets.
# ⚠ THE OBJECT IS USUALLY A MEMBER, NOT A BARE LOCAL, and matching only a bare
# identifier is why this read ONE site where the corpus has twenty-two. Every
# other one is `this->A`, `this->suspect`, `this->correct` — the solver reaches
# its matrix through a field, which is the ordinary case, not the exception.
# The two MdtLOD sites that share the shape are excluded twice over: `contact2`
# is not polymorphic, and they are not in callee position.
VSLOT_PTR = re.compile(r'\(\*\((?P<cast>int|uint|unsigned int|undefined4|kd_[iu]ptr)'
                       r'\s*\*\)\s*(?P<obj>[A-Za-z_]\w*(?:(?:->|\.)[A-Za-z_]\w*)*)\s*\+\s*'
                       r'(?P<off>0x[0-9a-fA-F]+|\d+)\)(?=\)\s*\()')
VPTR_FIRST = re.compile(r'^\s*code\s*\*\*\s*_vptr_')

I386_PTR = 4            # the shipped build's pointer size: what K was measured in
CFLAGS = ['-m32', '-O2', '-fno-pic', '-fno-strict-aliasing', '-std=gnu99',
          '-w', '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX']


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def compiles_identically(fn, text, build, inc):
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


def accept_edits(fn, text, edits, build, inc):
    """As many edits as keep the i386 object byte-identical; see fix_literal_offsets."""
    if not edits:
        return text, []
    cand = text
    for start, end, rep, _n in edits:            # descending by position
        cand = cand[:start] + rep + cand[end:]
    if compiles_identically(fn, cand, build, inc):
        return cand, edits
    if len(edits) == 1:
        return text, []
    mid = len(edits) // 2
    text, a = accept_edits(fn, text, edits[:mid], build, inc)
    text, b = accept_edits(fn, text, edits[mid:], build, inc)
    return text, a + b


def selftest(inc):
    """The claim is `2 * sizeof(void *)` is 8 here and 16 there. Ask."""
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(WORK, 'selftest.c')
    open(src, 'w').write('#include <stdio.h>\nint main(void){'
                         'printf("%d %d\\n", (int)(2 * sizeof(void *)),'
                         ' (int)(4 * sizeof(void *))); return 0; }\n')
    got = {}
    for bits in ('-m32', '-m64'):
        exe = os.path.join(WORK, 'selftest' + bits)
        if subprocess.run(['gcc', bits, '-O2', '-w', '-o', exe, src],
                          capture_output=True).returncode:
            sys.exit('fix_vtable_offsets: SELF-CHECK could not build at %s' % bits)
        got[bits] = subprocess.run([exe], capture_output=True, text=True).stdout.split()
    if got['-m32'] != ['8', '16'] or got['-m64'] != ['16', '32']:
        sys.exit('fix_vtable_offsets: SELF-CHECK FAILED — address point / slot 4 read '
                 '%r at i386 and %r at LP64 (want 8,16 and 16,32).'
                 % (got['-m32'], got['-m64']))


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else os.path.join(
        HERE, '..', 'Thirdparty', 'metoolkit')
    inc = os.path.join(root, 'include')
    selftest(inc)

    files = [f for f in sorted(os.listdir(srcdir))
             if f.endswith('.c') and os.path.exists(
                 os.path.join(build, f[:-2] + '.o'))]

    fixed = declined = 0
    notes = []
    # ---- A SEARCH THAT COMES BACK EMPTY HAS TO SAY WHETHER IT COULD FIND
    # ANYTHING, AND IT HAS TO SAY SO BEFORE IT EDITS. Checking afterwards left a
    # half-rewritten tree behind every refusal, which is worse than the zero it
    # was guarding against. It counts address points in EITHER spelling, so a
    # second run on a tree this has already repaired reads as "nothing to do"
    # rather than as a catastrophe.
    seen_apoints = sum(len(APOINT_ANY.findall(
        open(os.path.join(srcdir, f), errors='ignore').read())) for f in files)
    if not seen_apoints:
        sys.exit('fix_vtable_offsets: NOT ONE vtable address point in the corpus. '
                 'gen_vtables.py emits them and ghidra_clean.py references them, so '
                 'zero means the pattern stopped matching, not that there is nothing '
                 'to do.')
    for fn in files:
        path = os.path.join(srcdir, fn)
        text0 = open(path, errors='ignore').read()
        # Which locals in THIS file are polymorphic objects? The vptr store is
        # the evidence; nothing else makes `p + 12` a slot lookup.
        objs = {m.group('obj') for m in VPTR_STORE.finditer(text0)}

        edits = []
        for m in APOINT.finditer(text0):
            k = int(m.group('off'))
            if k != 2 * I386_PTR:
                notes.append('%-26s %s[0] + %d is not an address point — declined'
                             % (fn, m.group('sym'), k))
                declined += 1
                continue
            edits.append((m.start('off'), m.end('off'), '(2 * (int)sizeof(void *))',
                          '%-26s %-42s address point +%d' % (fn, m.group('sym'), k)))
        for m in VSLOT.finditer(text0):
            k = int(m.group('off'))
            if m.group('obj') not in objs:
                continue                     # not a vptr: see VPTR_STORE
            if k % I386_PTR:
                notes.append('%-26s %s + %d is not a slot boundary — declined'
                             % (fn, m.group('obj'), k))
                declined += 1
                continue
            edits.append((m.start('off'), m.end('off'),
                          '(%d * (int)sizeof(void *))' % (k // I386_PTR),
                          '%-26s %-42s slot %d (+%d)'
                          % (fn, m.group('obj'), k // I386_PTR, k)))
        # ⚠ THE STORE AND THE SLOTS ARE ONE REPAIR. Every slot read off a
        # polymorphic local must be scaled together with the address point that
        # produced its vptr; scaling one and not the other moves every call by a
        # slot. Refuse rather than half-apply — a spelling this pass cannot read
        # is a rule that stopped matching, not an object with nothing to do.
        missed = [m for m in VSLOT_ANY.finditer(text0)
                  if m.group('obj') in objs and not any(
                      v.start('off') == m.start('off')
                      for v in VSLOT.finditer(text0))]
        if missed:
            sys.exit('fix_vtable_offsets: %s has %d slot read(s) off a polymorphic '
                     'local (%s) in a spelling VSLOT does not match — e.g. %r. The '
                     'address point would be scaled and these would not, which moves '
                     'every one of those calls by a slot.'
                     % (fn, len(missed), ', '.join(sorted({m.group('obj') for m in missed})),
                        text0[missed[0].start():missed[0].end()]))
        for m in VSLOT_PTR.finditer(text0):
            k = int(m.group('off'), 0)
            tag = FNP.tag_of(
                FNP.lvalue_type(text0, m.start(), m.group('obj'), inc) or '', inc)
            body = FNP.headers(inc)[0].get(tag or '', '')
            if not VPTR_FIRST.match(body):
                continue                     # not a polymorphic object: see VSLOT_PTR
            if k % I386_PTR:
                notes.append('%-26s %s + %d is not a slot boundary — declined'
                             % (fn, m.group('obj'), k))
                declined += 1
                continue
            edits.append((m.start(), m.end(),
                          '(*(kd_iptr *)%s + (%d * (int)sizeof(void *)))'
                          % (m.group('obj'), k // I386_PTR),
                          '%-26s %-42s slot %d (+%d) through a %s vptr read'
                          % (fn, '%s (%s)' % (m.group('obj'), tag), k // I386_PTR,
                             k, m.group('cast'))))
        if not edits:
            continue
        edits.sort(key=lambda e: -e[0])
        text, ok = accept_edits(fn, text0, edits, build, inc)
        fixed += len(ok)
        declined += len(edits) - len(ok)
        keep = {id(e) for e in ok}
        for e in edits:
            notes.append(e[3] + ('' if id(e) in keep
                                 else '   DECLINED: not byte-identical at i386'))
        if text != text0:
            open(path, 'w').write(text)


    print('  vtable offsets rewritten in pointer-sized words : %d' % fixed)
    print('  declined (reported, not guessed)                : %d' % declined)
    for n in notes:
        print('     %s' % n)
    return 0


if __name__ == '__main__':
    sys.exit(main())
