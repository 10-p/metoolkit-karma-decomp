#!/usr/bin/env python3
"""wasm_indirect_check.py — find `call_indirect` sites that CANNOT succeed.

WHY THIS EXISTS
---------------
`wasm-ld` type-checks DIRECT calls only. It caught the `_ZdlPv` defect
(proven.txt WASM-SIGMISMATCH) and reported nothing about the indirect half,
which that entry closes by naming:

    "Karma dispatches through function-pointer tables (the API structs, every
     vtable), and a wasm `call_indirect` type-checks AT RUNTIME — a wrong
     prototype there is a trap nothing static will report."

This is that missing static report, and it is exact rather than heuristic.

WHAT IT PROVES, AND WHAT IT DOES NOT
------------------------------------
A `call_indirect (type T)` succeeds only if the function it reaches at run time
has type T EXACTLY. So if **no function in the table has type T at all**, that
call site can never succeed on any input — it is a guaranteed trap, reachable or
not. That is a proof, not a suspicion, and it needs nothing to execute.

The converse does NOT hold: a site whose type T exists in the table may still be
wrong, because it may reach a DIFFERENT function whose type is also in the table.
So a clean run here is a floor, not a ceiling — it retires the unsatisfiable
sites and says nothing about the rest. Treat a zero the way this project treats
every zero: as a sample size until something else confirms it.

Both defects found on the first two wasm boots WOULD have been caught here:

  * MeXMLElementProcess called `MeXMLHandler.fn` with FOUR arguments where the
    vendor declares two and three  ->  (i32,i32,i32,i32)->i32, not in the table.
  * MdtKeaAddConstraintForces called two vtable slots through `code *`
    (`typedef int code()`), so the call wanted an i32 RESULT from methods
    returning void  ->  (i32)->i32 against platformInit's (i32)->().

USAGE
    python3 tools/wasm_indirect_check.py <build-dir-or-wasm>

Needs `wasm-objdump` (wabt). Use a `-g` build: without a name section the
report can still count sites but cannot say which function each is in, which is
most of its value.
"""
import re
import subprocess
import sys
from pathlib import Path

WASM_IN_BUILD = Path('Source/SDLLaunch/SDLLaunch.wasm')


def resolve(arg):
    p = Path(arg)
    if p.is_dir():
        cand = p / WASM_IN_BUILD
        if not cand.exists():
            cand = p / 'SDLLaunch.wasm'
        p = cand
    if not p.exists():
        sys.exit(f'{arg}: no .wasm found (looked for {WASM_IN_BUILD})')
    return p


def objdump(wasm, flag):
    try:
        return subprocess.run(['wasm-objdump', flag, str(wasm)],
                              capture_output=True, text=True, check=True).stdout
    except FileNotFoundError:
        sys.exit('wasm-objdump not found — install wabt.')


TYPE_RE = re.compile(r'^ - type\[(\d+)\] (.+)$')
FUNC_RE = re.compile(r'^ - func\[(\d+)\] sig=(\d+)')
ELEM_RE = re.compile(r'^\s+- elem\[\d+\] = ref\.func:(\d+)')
IMPORT_FUNC_RE = re.compile(r'^ - func\[(\d+)\] sig=(\d+)')
HDR_RE = re.compile(r'^[0-9a-f]+ func\[(\d+)\](?: <(.+)>)?:')
CI_RE = re.compile(r'call_indirect \d+ \(type (\d+)\)')


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__.strip().splitlines()[-3].strip())
    wasm = resolve(sys.argv[1])

    x = objdump(wasm, '-x').splitlines()
    types, func_sig, table_funcs, section = {}, {}, set(), None
    for line in x:
        if re.match(r'^[A-Z][A-Za-z]*\[', line):
            section = line.split('[')[0]
            continue
        if section == 'Type':
            m = TYPE_RE.match(line)
            if m:
                types[int(m.group(1))] = m.group(2).strip()
        elif section in ('Function', 'Import'):
            m = FUNC_RE.match(line)
            if m:
                func_sig[int(m.group(1))] = int(m.group(2))
        elif section == 'Elem':
            m = ELEM_RE.match(line)
            if m:
                table_funcs.add(int(m.group(1)))

    if not types or not func_sig:
        sys.exit(f'{wasm}: parsed {len(types)} type(s) and {len(func_sig)} function(s) — '
                 'that is not a module this tool can read. REFUSING rather than '
                 'printing a reassuring zero.')
    if not table_funcs:
        sys.exit(f'{wasm}: no element segment — nothing is in the table, so every '
                 'indirect call would report as unsatisfiable. REFUSING.')

    table_types = {func_sig[f] for f in table_funcs if f in func_sig}

    # Walk the disassembly, attributing each call_indirect to its function.
    d = objdump(wasm, '-d')
    cur_idx, cur_name, sites, total = None, None, {}, 0
    for line in d.splitlines():
        h = HDR_RE.match(line)
        if h:
            cur_idx = int(h.group(1))
            cur_name = h.group(2) or f'func[{h.group(1)}]'
            continue
        m = CI_RE.search(line)
        if m:
            total += 1
            t = int(m.group(1))
            if t not in table_types:
                sites.setdefault((cur_name, t), 0)
                sites[(cur_name, t)] += 1

    named = sum(1 for f in table_funcs if f in func_sig)
    print(f'{wasm}')
    print(f'  {len(types)} type(s), {len(func_sig)} function(s), '
          f'{named} in the table across {len(table_types)} distinct type(s)')
    print(f'  {total} call_indirect site(s)')
    if not sites:
        print('  0 UNSATISFIABLE — every call_indirect asks for a type some table '
              'function has.')
        print('  (A floor, not a ceiling: see the docstring. This cannot see a site '
              'that reaches the')
        print('   wrong function of a type that IS in the table.)')
        return 0

    n = sum(sites.values())
    print(f'\n  {n} UNSATISFIABLE call_indirect site(s) in '
          f'{len({k[0] for k in sites})} function(s) — each one traps if reached:\n')
    for (fn, t), c in sorted(sites.items(), key=lambda kv: (-kv[1], kv[0])):
        times = '' if c == 1 else f'  x{c}'
        print(f'    {types.get(t, "?"):<44}  {fn}{times}')
    return 1


if __name__ == '__main__':
    sys.exit(main())
