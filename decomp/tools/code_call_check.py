#!/usr/bin/env python3
"""code_call_check.py — every CALL still made through the unprototyped `code` type.

WHY THIS EXISTS, AND WHY IT IS NOT A GREP
-----------------------------------------
`kd_compat.h` has `typedef int code();` — a function type with no parameter list and an
`int` result. Ghidra dispatches every function-pointer call through it. On i386 that costs
nothing: the caller cleans the stack, so a surplus argument is never read, and a caller that
ignores %eax may call a `void` function through it quite happily. On wasm32 a function's
type INCLUDES its result and its arity, and `call_indirect` checks it at run time, so every
one of these is a trap waiting for the right code path.

**This project has now been caught twice believing a `grep` for this.** The count was
declared zero on the strength of `grep '(\\*\\*(code \\*\\*)'`, and a running engine then
trapped on

    (*(*(code **)((*(char **)&vanillaAMatrix))))(&vanillaAMatrix, uVar10)

which is the same defect spelled with the two dereferences split and the base in its own
parentheses. Ghidra has at least five spellings for one dispatch; a pattern that enumerates
them is a pattern that will miss the sixth.

So this does not match spellings. It parses parentheses: for every parenthesised group that
is IMMEDIATELY FOLLOWED by an argument list — i.e. a group in callee position — it asks
whether that group dispatches through `code`. A cast's own parentheses are excluded, since
`(code **)(expr)` is a cast and not a call. That is structural, and a new spelling is caught
for free.

USAGE
    python3 tools/code_call_check.py karma-decomp/generated/allobj
    python3 tools/code_call_check.py /tmp/kd_out/allobj

Exit status is the number of sites (0 = clean), so it composes into a gate run.

WHAT A ZERO MEANS. That no call is made through `code` — nothing more. A call typed through
a WRONG-but-concrete prototype is invisible here, and so is a call through an
`undefined4`-typed slot. It is a floor, like tools/wasm_indirect_check.py, and the two see
different halves: this one reads the source and catches sites nothing has executed;
that one reads the linked .wasm and catches types no table function has.
"""
import glob
import os
import re
import sys

CODE_CAST = re.compile(r'\(\s*code\s*\*+\s*\)')
# A cast is a parenthesised group containing only a type. Those are not callees, they are
# what a callee is cast WITH, and `(code **)(p + 4)` would otherwise read as a call.
TYPE_ONLY = re.compile(r'^\(\s*[A-Za-z_][\w\s]*\*+\s*\)$')


def call_sites(text):
    """[(line, source of the callee expression)] for calls through `code`."""
    stack, hits, seen = [], [], set()
    for i, ch in enumerate(text):
        if ch == '(':
            stack.append(i)
        elif ch == ')' and stack:
            open_at = stack.pop()
            group = text[open_at:i + 1]
            if TYPE_ONLY.match(' '.join(group.split())):
                continue
            j = i + 1
            while j < len(text) and text[j] in ' \t\n':
                j += 1
            if j < len(text) and text[j] == '(' and CODE_CAST.search(group):
                line = text.count('\n', 0, open_at) + 1
                if line not in seen:        # nested groups, same defect
                    seen.add(line)
                    hits.append((line, ' '.join(group.split())[:88]))
    return hits


def main(argv):
    if len(argv) != 2:
        sys.exit('usage: code_call_check.py <dir-of-.c-or-a-single-.c>')
    target = argv[1]
    files = ([target] if target.endswith('.c')
             else sorted(glob.glob(os.path.join(target, '*.c'))))
    if not files:
        sys.exit(f'{target}: no .c files — REFUSING rather than reporting a clean zero.')

    total = 0
    for f in files:
        try:
            hits = call_sites(open(f, errors='ignore').read())
        except OSError as e:
            sys.exit(f'{f}: {e}')
        if hits:
            total += len(hits)
            print(f'--- {os.path.basename(f)}')
            for line, src in hits:
                print(f'    {line:5d}  {src}')

    print(f'\n{len(files)} file(s), {total} call(s) through the unprototyped `code` type.')
    if total:
        print('  Each is inert on i386 and a call_indirect trap on wasm32 if reached.')
        print('  Fix at the GENERATOR (ghidra_clean: _type_float_dispatches /')
        print('  type_code_slot_dispatches), never in a generated file.')
    else:
        print('  Clean. Read the docstring for what that does and does not prove.')
    return total


if __name__ == '__main__':
    sys.exit(min(main(sys.argv), 250))
