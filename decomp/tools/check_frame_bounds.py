#!/usr/bin/env python3
"""Report recovered code that addresses outside a local it names.

Ghidra renders a stack slot as `*(T *)((int)&local + K)`. When the frame was
shifted by an alloca, ghidra_clean drops the shift, and K then has to land
inside `local` for the result to mean anything. Where it does not, the object
compiles, passes every synthetic scene, and corrupts an unrelated local in a
real match — which is what IxConvexTriList did for a session and a half, and
what IxSphylPrimitives was released carrying.

The compiler cannot see this: the cast launders the bounds. So check the text.

    python3 tools/check_frame_bounds.py /tmp/kd_out/allobj

Reports every `(&)?NAME + K` (K possibly negative) where NAME is a local
declared in the same function and K is outside [0, sizeof(NAME)). Sizes come
from the declaration, which is Ghidra's own idea of how big the variable is —
the same thing GCC will allocate — so an out-of-range K is exactly a write past
the object GCC laid out.

Exit status is 1 if anything is reported, so it can gate a pipeline change.
"""
import os
import re
import sys

# `  MeReal aMStack_144 [2];` / `  int local_38;` / `  undefined1 auStack_12c [12];`
DECL = re.compile(r'^\s{2,4}(\w[\w ]*?)\s*(\**)\s*(\w+)\s*(?:\[\s*(\d+)\s*\]'
                  r'\s*(?:\[\s*(\d+)\s*\])?)?\s*;\s*$')
# `(int)&name + 0x10`, `&name + 4`, `name + -0x1c`. The `&` is captured because
# it decides whether this is a stack object at all: a plain `p + 0x14` on a
# pointer local is ordinary pointer arithmetic into someone else's memory and
# has nothing to say about the frame.
REF = re.compile(r'(?:\(int\)\s*)?(&)?\b(\w+)\s*\+\s*'
                 r'(-?(?:0x[0-9a-fA-F]+|\d+))\b')

WIDTH = {
    'char': 1, 'undefined1': 1, 'byte': 1, 'bool': 1, 'MeI8': 1, 'MeU8': 1,
    'short': 2, 'undefined2': 2, 'ushort': 2, 'MeI16': 2, 'MeU16': 2,
    'int': 4, 'uint': 4, 'undefined4': 4, 'long': 4, 'ulong': 4, 'float': 4,
    'MeI32': 4, 'MeU32': 4, 'MeReal': 4, 'MeBool': 4, 'code': 4,
    'double': 8, 'undefined8': 8, 'longlong': 8, 'ulonglong': 8,
}


def function_bodies(text):
    """Yield (name, body) for each top-level `... name(...)\\n{ ... }`."""
    for m in re.finditer(r'^(?:static\s+)?[\w *]+?(\w+)\s*\([^;{]*?\)\s*\n?\{',
                         text, re.M):
        depth, i = 0, text.index('{', m.start())
        for j in range(i, len(text)):
            if text[j] == '{':
                depth += 1
            elif text[j] == '}':
                depth -= 1
                if depth == 0:
                    yield m.group(1), text[i:j]
                    break


def locals_of(body):
    """name -> (byte size, is_array), for declarations at the head of a function.

    Pointer locals are left out: `p + 0x14` on a `void *p` is arithmetic on
    whatever p points at, which is none of this checker's business.
    """
    out = {}
    for line in body.splitlines():
        if not line.strip() or line.lstrip().startswith(('/*', '*', '//')):
            continue
        m = DECL.match(line)
        if not m:
            # Declarations stop at the first statement; keep scanning anyway,
            # because Ghidra sometimes emits a stray blank-ish line between
            # them and a false stop would silence the whole function.
            continue
        base, stars, name, n1, n2 = m.groups()
        base = base.strip()
        if base in ('return', 'goto', 'break', 'continue', 'else', 'do'):
            continue
        if stars:
            continue                      # a pointer: not a frame object
        width = WIDTH.get(base.split()[-1])
        if width is None:
            continue                      # struct or unknown: no opinion
        out[name] = (width * int(n1 or 1) * int(n2 or 1), bool(n1))
    return out


# `pMVar6 = (McdContact *)&type1;` / `pMVar6 = aMStackY_501c;`
# and the materialised form ghidra_clean emits for the same area,
# `pMVar6 = (McdContact *)(kd_argarea_pMVar6 + 0x24);`
PTR_TO_LOCAL = re.compile(r'^\s*(\w+)\s*=\s*(?:\([\w ]*\*+\)\s*)?(&)?(\w+)\s*;\s*$')
PTR_TO_LOCAL_OFF = re.compile(
    r'^\s*(\w+)\s*=\s*(?:\([\w ]*\*+\)\s*)?\(\s*(\w+)\s*\+\s*'
    r'(-?(?:0x[0-9a-fA-F]+|\d+))\s*\)\s*;\s*$')


def pointer_aliases(body, sizes):
    """ptr -> (target local, size, base offset within it).

    THE HOLE THIS CLOSES, and it was shielding a real defect. The docstring
    above says a pointer local is none of this checker's business, because
    `p + 0x14` on a `void *p` is arithmetic on whatever p points at. True — but
    not when p was assigned the address of a LOCAL two lines earlier, which is
    how Ghidra renders an outgoing-argument area whose base it could not name:

        pMVar6 = (McdContact *)&type1;              /* type1 is an int */
        if (...) { pMVar6 = aMStackY_501c; }
        *(void **)((int)pMVar6 + -4)  = pvVar5;     /* BELOW type1 */
        *(McdModelPair **)((int)pMVar6 + -0x10) = p;

    `McdInteractions` writes nine words below a four-byte `int`, and this
    checker reported 0 for it because the offsets go through a pointer. The
    alias only counts where the reference is BYTE arithmetic — `(int)ptr + K` —
    which is the shape in question; `ptr + K` without the cast is element
    arithmetic and genuinely is someone else's memory.

    THE BASE OFFSET IS WHAT KEEPS THIS HONEST AFTER THE REPAIR.
    `ghidra_clean.materialise_pointer_arg_area` fixes the shape above by giving
    the area real storage and pointing the pointer at its TOP —
    `pMVar6 = (McdContact *)(kd_argarea_pMVar6 + 0x24)` — so every negative
    offset lands inside the buffer. Without reading that `+ 0x24` this checker
    would simply stop recognising the pointer, i.e. it would go blind on the
    repaired form and report green either way. It reads it.

    Where a pointer is assigned more than one local, the one leaving the LEAST
    room is kept, because the reference has to be in range for all of them."""
    out = {}
    for line in body.splitlines():
        m = PTR_TO_LOCAL.match(line)
        base = 0
        if not m:
            m = PTR_TO_LOCAL_OFF.match(line)
            if not m:
                continue
            ptr, target = m.group(1), m.group(2)
            base = int(m.group(3), 0)
            amp = None
        else:
            ptr, amp, target = m.group(1), m.group(2), m.group(3)
        ent = sizes.get(target)
        if ent is None or ptr in sizes:
            continue
        size, is_array = ent
        if not is_array and not amp:
            # A scalar's NAME is its value, not its address. `(T *)(i + 2)`
            # where `i` is an int is ordinary arithmetic on a number, and
            # reading it as a frame reference reports MeFAsset's loop counter
            # as a 4-byte object addressed at +5.
            continue
        prev = out.get(ptr)
        if prev is None or (size - base) < (prev[1] - prev[2]):
            out[ptr] = (target, size, base)
    return out


def violations(text):
    """[(function, var, offset, size)] for every out-of-range frame reference.

    recover.py calls this as a detector. It is deliberately a report on the
    TEXT: the cast in `*(T *)((int)&local + K)` launders the bounds, so the
    compiler has nothing to say, and the runtime symptom is corruption of some
    unrelated local three declarations away — a divergence with no obvious
    author, as IxConvexTriList spent a session and a half demonstrating.
    """
    out = []
    for name, body in function_bodies(text):
        sizes = locals_of(body)
        if not sizes:
            continue
        aliases = pointer_aliases(body, sizes)
        seen = set()
        for line in body.splitlines():
            for m in REF.finditer(line):
                amp, var, off = m.group(1), m.group(2), m.group(3)
                off = int(off, 16) if off.lower().lstrip('-').startswith('0x') \
                    else int(off, 10)
                ent = sizes.get(var)
                if ent is None:
                    # A pointer that was handed the address of a local IS a
                    # frame reference, but only under BYTE arithmetic — the
                    # `(int)` cast. See pointer_aliases.
                    alias = aliases.get(var)
                    if alias is None or not m.group(0).lstrip().startswith('(int)'):
                        continue
                    var, size, is_array, amp = alias[0], alias[1], True, None
                    off += alias[2]
                else:
                    size, is_array = ent
                    # A scalar only names a frame object when its address is
                    # taken; an array decays on its own.
                    if not amp and not is_array:
                        continue
                # One past the end is a legal pointer value, and it is
                # exactly what the materialised form produces: the area is
                # addressed downwards from its top, so the base IS buf + size.
                limit = size + 1 if PTR_TO_LOCAL_OFF.match(line) else size
                if 0 <= off < limit or (var, off) in seen:
                    continue
                seen.add((var, off))
                out.append((name, var, off, size))
    return out


def main(argv):
    """check_frame_bounds.py <kd_out/allobj> [kd_build]

    The optional build directory splits the report in two, and the split is the
    difference between a gate and a list. recover.py already calls violations()
    as a DETECTOR, so an object with a violation is held out of the build — and
    a held object's violations are the detector working, not a regression. Only
    a violation in an object that is IN the build is a failure, and only that
    sets the exit status."""
    root = argv[1] if len(argv) > 1 else '/tmp/kd_out/allobj'
    build = argv[2] if len(argv) > 2 else None
    in_build, held = 0, 0
    for fn in sorted(os.listdir(root)):
        if not fn.endswith('.c'):
            continue
        text = open(os.path.join(root, fn), errors='ignore').read()
        rows = violations(text)
        if not rows:
            continue
        shipped = build is None or os.path.exists(
            os.path.join(build, fn[:-2] + '.o'))
        for name, var, off, size in rows:
            print(f'{fn}: {name}: {var} is {size} byte(s), '
                  f'addressed at {off:+#x}'
                  + ('' if shipped else '   [HELD — detector working]'))
        if shipped:
            in_build += len(rows)
        else:
            held += len(rows)
    if held:
        print(f'\n{held} out-of-range frame reference(s) in objects the '
              f'detector is HOLDING — expected, not a failure')
    print(f'\n{in_build} out-of-range frame reference(s) in the build')
    return 1 if in_build else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
