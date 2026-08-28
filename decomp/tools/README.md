# `karma-decomp/tools/` — every generator and analyser, with the command

**One block per tool: what it answers, and the exact invocation.** Nothing here is a guess — if a
command is written down it has been run. Read `../HANDOVER.md` first for what the project is; this
file is the index you come back to.

Conventions used throughout:

```bash
cd /home/ion/engines/engine-ut2004/karma-decomp
MT=$(realpath ../Thirdparty/metoolkit)     # the shipped SDK
LIB=$MT/lib.rel/linux_single_gcc3.2        # its i386 archives
LAB=/home/ion/tools/karma-lab              # Ghidra dumps + extracted .o, OUTSIDE this repo
```

`/tmp/kd_out` (recovered `.c`) and `/tmp/kd_build` (compiled `.o`) are the pipeline's outputs.
**`/tmp` is volatile — a crash wipes it.** Anything you want to survive, copy out. One casualty of
that already: the per-archive member tree `reachable.py` needs. Four lines rebuild it, and `$LIB`
must be absolute because the loop `cd`s:

```bash
rm -rf /tmp/kd_members && mkdir -p /tmp/kd_members
for a in "$LIB"/*.a; do n=$(basename "$a" .a); mkdir -p /tmp/kd_members/$n
  (cd /tmp/kd_members/$n && ar x "$a"); done
# -> 16 archives, 192 members
```

**Every command below has been run.** Where a tool takes a directory, the shape of that directory is
stated, because two of them want different shapes and one used to fail silently when given the wrong
one. Writing this file caught six invocations that were wrong as transcribed from the docstrings —
`gen_typedb` takes `--public-headers`, not `--metoolkit`; `gen_vtables` takes `-o`, not `--out`;
`reachable.py` has no `--why`; `ghidra_clean` takes `--object`, not `--obj`; the Ghidra dumps are
named `<object>.o.c`, not `<object>.c`; and `dwarf_structs --type` must be pointed at the object
that DEFINES the class. **Read the rule, then run it** applies to documentation too.

---

## The pipeline — run these to have anything at all

### `recover.py` — drives everything and classifies the result

The whole recovery in one command, ~95 s for 153 objects. Generates each prelude, cleans each
Ghidra dump into C, compiles it, and sorts the outcome into OK / TODO / REVIEW / FAIL.

```bash
rm -rf /tmp/kd_out /tmp/kd_build
python3 tools/recover.py \
  --dump-dir $LAB/out14 \
  --obj-dir  $LAB/allobj \
  --out-dir  /tmp/kd_out \
  --build-dir /tmp/kd_build \
  --metoolkit $MT \
  --protos   $LAB/kd_protos11.h
```

**`out14` and `kd_protos11.h` go together** — mixing either with the older pair gets `McdSpace`
wrong in opposite directions (`../proven.txt`). **`--build-dir` is not optional** if you care about
the objects you already have: it defaults to `/tmp/kd_build` and will overwrite the baseline you
were about to compare against.

It also reads `../proven.txt`: the first word of any **non-comment** line is an object name, and
naming an object there RELEASES it from quarantine. Prose notes must be commented or they become
silent releases.

### `ghidra_clean.py` — a Ghidra dump becomes compilable C

The heart of the project, 7,880 lines: the pre-passes, the repair loop, and every rule that turns
Ghidra's output into C that compiles and means the same thing. `recover.py` drives it; you call it
directly only when debugging a rule.

```bash
# NOTE THE DUMP FILENAME: Ghidra writes `<object>.o.c`, not `<object>.c`.
python3 tools/gen_prelude.py $LAB/allobj/McdSpace.o --protos $LAB/kd_protos11.h \
        --corpus $LAB/allobj --include-dir $MT/include --dump $LAB/out14/McdSpace.o.c \
        -o /tmp/McdSpace.prelude.h --exports-out /tmp/McdSpace.exports.h
python3 tools/gen_vtables.py $LAB/allobj/McdSpace.o -o /tmp/McdSpace.vtables.h

python3 tools/ghidra_clean.py $LAB/out14/McdSpace.o.c -o /tmp/McdSpace.c \
        --object $LAB/allobj/McdSpace.o --prelude /tmp/McdSpace.prelude.h \
        --exports /tmp/McdSpace.exports.h --vtables /tmp/McdSpace.vtables.h \
        --protos $LAB/kd_protos11.h --metoolkit-include $MT/include
#   -> /tmp/McdSpace.c: 34 functions (1 static)
```

The result will **not** match the pipeline's output for that object, and that is expected rather
than a fault: `recover.py` also passes the field map and the compile flags, and runs the repair loop
against real diagnostics. Use this to debug a rule, not to produce a build input.

> **Read the rule, then RUN it.** Six rules have looked correct and silently declined; reading alone
> has a 0/6 record here. Call the rule directly on the offending line.

### `gen_prelude.py` — the per-object prelude, read out of the binary

Imports (with demangled signatures and `KD_MANGLED` asm labels), file-scope statics with values read
from `.data`/`.rodata`, and explicit TODOs for anything it cannot determine.

```bash
python3 tools/gen_prelude.py $LAB/allobj/McdSpace.o \
        --protos $LAB/kd_protos11.h --corpus $LAB/allobj \
        --include-dir $MT/include --dump $LAB/out14/McdSpace.o.c \
        -o /tmp/McdSpace.prelude.h --exports-out /tmp/McdSpace.exports.h
#   -> 26 imports, 0 statics, 1 TODO(s) needing a human
```

Carries `ITANIUM_ABI_FUNCS`: `operator new`/`delete` and friends, whose C shape is read off the
mangled name. Without it the fallback emits `extern int f()`, which is inert on i386 and a **trap**
on wasm32 — `../proven.txt` `WASM-SIGMISMATCH`.

### `gen_protos.py` — prototypes for Ghidra to consume

Ghidra has no signature for a function an object merely *imports*, so it guesses the arity from the
call site and gets it wrong — dropping arguments and inventing writes to unrelated stack slots.
This emits real prototypes from DWARF for Ghidra to apply.

```bash
python3 tools/gen_protos.py $LAB/allobj -o /tmp/kd_protos.h
```

Use **`kd_protos11.h`**, not the plain output: it is this file plus the API-struct prototypes (§5a)
and every C++ function re-declared under its **mangled** name (§5b). The mangled half is
load-bearing — short names collide (`Link::Remove()` vs `CxSmallSort::Remove(int)`), and that
collision alone held 16 symbols.

### `gen_typedb.py` — one header of every Karma-internal type

gcc 3.2 emits a class's full layout only in the CU that defines it; every other CU has a
declaration-only DIE with a bogus size (`keaMatrix` is 4 bytes in one object and its real 20 in
another). This unions the DWARF across all objects and keeps the richest definition.

```bash
python3 tools/gen_typedb.py $LAB/allobj --public-headers $MT/include -o include/kd_types.h
```

Re-run after any DWARF-side change (`../HANDOVER.md` §4, "Regenerate the type database").

### `gen_vtables.py` — the C++ ABI data Ghidra does not decompile

gcc emits three COMDAT data objects per polymorphic class (`_ZTS` type string, `_ZTI` typeinfo,
`_ZTV` vtable) and anything that constructs the class references them. Ghidra decompiles code, not
data, so it emits none of them. This re-emits all three as plain C, with slot contents taken from
the relocation records.

```bash
python3 tools/gen_vtables.py $LAB/allobj/keaMatrix_PcSparse.o -o /tmp/x.vtables.h
#   -> /tmp/x.vtables.h: 3 C++ ABI object(s)
```

### `gen_vtable_callsites.py` — which function a virtual call actually reaches

Ghidra emits a call through a function pointer with **every argument dropped**. This computes, per
call site, the address and the resolved signature, as a table `DumpDecomp.java` applies during the
dump.

```bash
python3 tools/gen_vtable_callsites.py $LAB/allobj -o /tmp/callsites.json \
        --protos-out /tmp/kd_callsite_protos.h
python3 tools/gen_vtable_callsites.py $LAB/allobj -o /tmp/cs.json --only keaRbdCore_unified
```

`../HANDOVER.md` §5a has `KARMA_VTABLE_CALLSITES`, the environment variable that feeds the result
back into a Ghidra re-run.

> **The object that motivates a tool is the worst thing to validate it on.** This one read 9/9
> applied on its motivating object because that object's only relevant function starts at offset 0,
> where the wrong address formula and the right one agree. The second object put six signatures in
> `.eh_frame`.

### `vtable_slots.py` — resolve one virtual call, checked against the machine code

When a polymorphic object is a **local**, Ghidra loses the vptr entirely and emits
`(**(code **)(_vanillaFunctions + 0x10))()`. This says which function slot `+0x10` is, and can
verify the answer against the disassembly before you believe it.

```bash
python3 tools/vtable_slots.py $LAB/allobj keaFunctions_Vanilla
#   -> +0x10  keaFunctions_Vanilla::platformInit()
#      +0x14  keaFunctions_Vanilla::allocateMemory(keaTempMemory*, MdtKeaConstraints, int)

# with the vptr stores in one function checked too:
python3 tools/vtable_slots.py $LAB/allobj keaFunctions_Vanilla \
        $LAB/allobj/keaRbdCore_unified.o MdtKeaAddConstraintForces
```

### `dwarf_structs.py` — real member names for a C++ class

Ghidra leaves `this` untyped for some C++ classes and renders members as `field_0x14`. Correct, but
unreviewable. This reads the names, offsets and types out of `.debug_info`.

```bash
python3 tools/dwarf_structs.py $LAB/allobj/keaMatrix_PcSparse.o --list
python3 tools/dwarf_structs.py $LAB/allobj/keaMatrix_PcSparse.o --type keaMatrix_pcSparse
```

**Point it at the object that DEFINES the class, not one that merely uses it** — that is the whole
reason `gen_typedb.py` exists. `keaMatrix_PcSparse.o` has `keaMatrix_pcSparse` at its real 52 bytes;
`keaMatrix_PcSparse_vanilla.o` does not carry the type at all and the tool exits 1 saying so.

---

## The gates — run all of these, every time

`../HANDOVER.md` §4 has the full nine-gate block. These are the ones that live in `tools/`.

### `check_frame_bounds.py` — a defect no behavioural test can find

Ghidra renders a stack slot as `*(T *)((int)&local + K)`. When the frame was shifted by an alloca
and the shift is dropped, `K` has to land **inside** `local` or the code silently corrupts an
unrelated local. The compiler cannot see it — the cast launders the bounds.

```bash
python3 tools/check_frame_bounds.py /tmp/kd_out/allobj /tmp/kd_build
```

**Pass the build dir too.** Without it, every held object's violations count as failures — and a
held object's violations are the detector *working*. Only a violation in an object that is IN the
build sets the exit status.

> It only reads **constant** offsets. `McdContact` indexed an 8-byte scratch slot as `count+1`
> 16-byte structs with a variable index and this reported 0 throughout.

### `check_symbol_bindings.py` — same INTERFACE, not just the same names

Does each recovered object export what the shipped one exported — name, **binding** and size?
`wasm_check.sh` compared names and threw the binding letter away for the project's whole life, so
an object that shipped `putchar` WEAK and came back GLOBAL made decompiled Karma the `putchar` for
the entire process.

```bash
python3 tools/check_symbol_bindings.py /tmp/kd_build $LAB/allobj
```

### `dropin_gap.py` — THE metric

Which SHIPPED members does the engine still need? Walks the symbol closure from the engine's own
object files, resolving against the recovered build first and the shipped archives only then.
**That set is the work**; object counts are not.

```bash
python3 tools/dropin_gap.py ../build-native-karma /tmp/kd_build $LAB/allobj
#   -> 0 shipped member(s), 0 symbol(s); 134 recovered member(s) in the closure

python3 tools/dropin_gap.py ../build-native-karma /tmp/kd_build $LAB/allobj \
        --status /tmp/kd_recover.log            # annotate each gap member with why it is held
python3 tools/dropin_gap.py ../build-native-karma /tmp/kd_build $LAB/allobj \
        --verify-against /tmp/real_link_undefined.txt   # check the walk against a REAL link
```

Third argument is the **flat** directory of shipped `.o` (`karma-lab/allobj`), not the per-archive
tree. Needs an engine build only for its object files, so it costs seconds after the first. **It is
ZERO** and has been checked against a real link twice over.

> ### ⚠ IT MUST BE `build-native-karma`, AND THIS IS WHY THAT TREE IS KEPT ON DISK
>
> The seed is "every symbol the ENGINE's objects import that metoolkit defines". An engine built
> **without** Karma — which is every preset except `native-karma` and `karma-ref` — imports none of
> them. The seed is empty, the walk visits nothing, and the tool prints
>
> ```
>   0 shipped member(s), 0 symbol(s); 0 recovered member(s) in the closure
> ```
>
> **which is the project's headline number, arrived at vacuously.** Point it at `build-native`
> instead of `build-native-karma` and the metric that measures the whole deliverable congratulates
> you. It now refuses that case (exit 1) — confirmed by feeding it a directory holding one engine
> object that touches no Karma symbol. A real closure is **134** recovered members; the count on
> that line is what tells the two apart.
>
> `../../build-native-karma/` is therefore the one build tree deliberately kept in the working
> directory. Everything else under `build-*/` is reproducible from `BUILD.md` and was deleted.
> Rebuild it with `cmake --preset native-karma && cmake --build --preset native-karma -j"$(nproc)"`.

### `reachable.py` — is this object in scope at all?

Which Karma objects can UT2004 actually reach, at symbol level. An object nothing reaches is **out
of scope, not a to-do** — §3b retires 31 permanently on this tool's output.

```bash
python3 tools/reachable.py ../build-native-karma /tmp/kd_members
#   -> 543 engine objects, 178 members, 147 reached, 31 NOT reachable at all
```

Second argument is the **per-archive** tree (see the four lines at the top), *not* `karma-lab/allobj`.
Given the flat directory it now exits 1; it used to print an empty table and exit 0, which reads
exactly like "nothing is dead".

Its 31 cross-check against a completely different instrument: `MdtFixedPath`, `MdtLinear1`,
`MdtLinear2`, `MdtSpring` and `MdtUserConstraint` are on this list, and `wasm_members.py` finds the
wasm linker pulling in none of them.

> Reachability is necessary, not sufficient. `MdtLOD` had a complete, correct, breakpoint-measured
> unreachability argument and was one signature away from shipping a function that segfaults on its
> first real call. See the standing order.

### `code_call_check.py` — every call still made through the unprototyped `code` type

`kd_compat.h` has `typedef int code();` — no parameter list, `int` result — and Ghidra dispatches
every function-pointer call through it. Free on i386 (the caller cleans the stack and may ignore
`%eax`); on wasm32 the result and the arity are part of a function's type, so each one is a
`call_indirect` trap waiting for the right code path.

```bash
python3 tools/code_call_check.py generated/allobj      # 0
python3 tools/code_call_check.py /tmp/kd_out/allobj
```

Exit status is the site count, so it composes into a gate run.

> **IT DOES NOT MATCH SPELLINGS, AND THAT IS THE WHOLE DESIGN.** This class was declared closed
> **twice** on the strength of `grep '(\*\*(code \*\*)'`, and a running engine trapped both times.
> Ghidra spells one dispatch at least five ways; the site that reached a live match is
> `(*(*(code **)((*(char **)&vanillaAMatrix))))(…)` — two dereferences split, base in its own
> parentheses. So this **parses**: every parenthesised group in *callee* position (a cast's own
> parentheses excluded), asked whether it dispatches through `code`. A sixth spelling is caught
> for free.

A zero means no call goes through `code` and nothing more — a call typed through a
*wrong-but-concrete* prototype is invisible here. It and `wasm_indirect_check.py` see different
halves: this reads the source and catches sites nothing has executed; that reads the linked
`.wasm` and catches types no table function has. `proven.txt` `WASM-INDIRECT-SIGS`.

### `wasm_indirect_check.py` — the traps `wasm-ld` structurally cannot report

`wasm-ld` type-checks **direct** calls only, which is how `_ZdlPv` was caught (`proven.txt`
`WASM-SIGMISMATCH`) and why the indirect half of that class reached a running engine. On wasm a
function's type includes its **result** and its **arity**, and `call_indirect` checks it at run
time — so a surplus argument or a wrong return type is inert on i386 cdecl and a trap here.

```bash
python3 tools/wasm_indirect_check.py build-wasm-karmadecomp-debug
```

Needs `wasm-objdump` (wabt), ~5 s. For every `call_indirect` it asks whether **any** function in
the table has that type; if none does, that site can never succeed on any input, reachable or
not. That is a proof, not a suspicion.

> **READ WHAT IT CANNOT DO, because the number is reassuring.** The converse does not hold: a
> site whose type IS in the table may still reach the wrong function. It reads 0 today and it
> would **not** have caught `McdInteractions`' `goodbyeFn` — `(i32) -> i32` is a type hundreds of
> table functions have, and the callee `McdCacheGoodbye` is `(i32) -> nil`. That one was found by
> reading the vendor's struct against the call site. A floor, not a ceiling.

Use the `-g` presets: without a name section it can still count sites but cannot say which
function each is in, which is most of the value. It **refuses** rather than printing a zero when
handed a module it cannot parse or one with no element segment.

### `census_report.py` — which recovered functions has the REAL GAME ever entered?

**The denominator for everything else in `proven.txt`.** Every other tool here answers "does this
object reproduce the original"; none answers "was this code reached", and the two produce the
identical line of output. A whole-library A/B that reads bit-identical proves the recovery exact
*on the paths the map exercised* and says nothing whatever about the rest.

```bash
cmake --preset native -B ../build-native-census -DKD_CENSUS=ON -DCMAKE_EXE_LINKER_FLAGS=-no-pie
cmake --build ../build-native-census -j"$(nproc)"

cd /home/ion/karma-run/System && cp <binary> census.bin
KD_INSTR_OUT=/tmp/c1.txt timeout 150 xvfb-run -a ./census.bin \
  "test-karma-1?game=Onslaught.ONSOnslaughtGame?TimeLimit=0?SpectatorOnly=1?NumBots=2" \
  -SOFTWARERENDERER -nohomedir
# … repeat per map; the union is what matters

python3 tools/census_report.py /tmp/c1.txt /tmp/c2.txt … ../build-native-census/…/ut2004-pixo.bin
```

Reads **555 of 2,025 functions (27.4%)** across four maps and two gametypes today. It names every
cold function, per object.

⚠ **`-no-pie` MUST GO ON THE EXECUTABLE.** `kd_instr.c` records RUNTIME addresses and `nm` prints
link-time offsets; in a PIE they differ by the load bias, nothing resolves, and the report reads
"0 of 2025 entered" — indistinguishable from "the physics never ran". Putting the flag on the
Engine static library instead changes nothing and the build still succeeds. The report now
REFUSES a zero rather than printing one.

⚠ **`SpectatorOnly=1` is what makes a match actually run.** Without it the level loads and
`LevelInfo->Pauser` is set, `ULevel::Tick` skips the Karma tick entirely, and the census counts
almost nothing while the HUD — which ticks on the render path, outside that guard — keeps logging
every frame.

⚠ **RE-BUILD THE INSTRUMENTED BINARY BEFORE EVERY CENSUS.** A census trace is a list of RUNTIME
ADDRESSES; it means nothing except against the binary that produced it, and `nm` will happily
resolve it against a different one and give you a plausible, wrong table. The build tree survives
between sessions and `cmake` will not rebuild what it thinks is current, so the failure mode is
silent. Measured here: a census tree whose objects were four commits old resolved cleanly and
reported `MdtLimit` entering 8 of 34 functions, which is a statement about a build that no longer
exists. Check `ls -lat build-native-census/**/*.c.o | head -1` against `git log -1` first.

### `cold_triage.py` — separate DEAD Karma from UNTESTED Karma

`census_report.py` says 36% of the recovery has ever run and leaves 64% looking untested. It is
not: a large part of the cold set is code UT2004 cannot reach on any input. This intersects the
census with `reachable.py`'s symbol closure and reports the number that is actually worth quoting.

```bash
python3 tools/cold_triage.py /tmp/cy-*.txt \
        ../build-native-census/Source/SDLLaunch/ut2004-pixo.bin \
        --build ../build-native-karma --members /tmp/kd_members
```

Four verdicts, and the two weak ones are marked weak rather than folded in: **DEAD-OBJECT** (its
member is in `reachable.py`'s unreachable set), **DEAD-SYMBOL** (reachable object, but no
relocation anywhere in the closure names this symbol), **FILE-LOCAL** (a static — symbol
reachability has *nothing* to say, and calling it dead would be an artefact), and **UNVALIDATED**
— reachable, referenced, never executed. That last one is the number.

⚠ **DEAD-SYMBOL IS EVIDENCE, NOT PROOF.** It inherits `reachable.py`'s own blind spot: a table the
ENGINE fills in, and anything entered only through a function pointer the engine computed, has no
relocation to find. And the census side is a lower bound — a function absent from every recorded
run was not reached *by those runs*.

> **It also found that the denominator is wrong.** 148 of the 2,025 symbols `nm` reports are
> `__x86.get_pc_thunk.*` — three instructions gcc emits per object to load the PC — so every count
> in this project, including `proven.txt`'s, is 7.3% larger than the recovery is. This tool
> excludes them and prints the figure both ways so the two can be reconciled.

### `amd64_oracle.py` — read the LP64 layouts off a 64-bit build of the same source

Everything else here reads the **i386** build, which is why the recovery encodes 32-bit struct
layouts and why `layout_check.py` can only bound the arm64 job. UT2004 v3369 ships
`metoolkit/lib.rel/win_amd64_single/*.lib`: a 64-bit build of the same MathEngine source, real
x86-64 COFF, **not stripped**, 1,089 symbols over 189 members in 15 archives. Its field offsets and
its allocation constants ARE the 64-bit layouts.

```bash
python3 tools/amd64_oracle.py --selftest          # five facts, asked of the binary
python3 tools/amd64_oracle.py --consts McdInit    # what sizes does this function pass?
python3 tools/amd64_oracle.py --function McdSphereGetRadius
python3 tools/amd64_oracle.py --list
```

```
McdSphereGetRadius   i386   flds  0x10(%edx)
                     amd64  movss 0x20(%rcx),%xmm0
```

★ **IT IS LLP64, NOT LP64, AND THAT CHANGES HOW YOU MAY USE IT.** The shipped 64-bit build is MSVC
for Windows, where `long` is four bytes; Android is eight. `sizeof(MdtBody)` is 576 at i386, **696
at win64 and 704 on Android**. So its constants are the right oracle for *which type a site means*
and the wrong number to paste into the source — the repair is always `sizeof(T)`, and this library's
job is to say what `T` is. `x86_64-w64-mingw32-gcc` reproduces the win64 column exactly (verified
against the immediates in `MdtWorldCreate`: 856 and 696), which is what makes the check mechanical.

⚠ **Use the `.lib` MEMBERS, not the loose `.obj` beside them** — those are MSVC LTCG and
disassemble to nothing. The first pass over that directory declared the whole thing unusable on
their evidence.

⚠ **There is no type information in there, and the section EXISTS.** Every member carries a
`.debug$T`, which is where CodeView puts struct layouts and would have made this tool unnecessary.
All of them are 0x40–0x44 bytes: a `/Zi` build whose types live in a PDB nobody shipped. A section
being present is not a section being populated.

⚠ **`objdump --disassemble=NAME` prints nothing here.** `/Gy` puts every function in its own COMDAT
and every one of those sections is called `.text` at VMA 0, so selecting by name or by section both
fail; the whole object has to be disassembled and split on the `<name>:` labels.

The constant reader **propagates `mov $imm` forward**, because MSVC parks one constant in a register
and reaches the others by arithmetic on it — `McdCacheHello` passes `sizeof(McdCache)` as
`lea -0x1c(%rdx)` with `%rdx == 100`, and the number 72 appears nowhere in the instruction stream. A
`lea` with a 32-BIT destination is an integer; one with a 64-bit destination is an address, and
counting the second made `0x68` (`&framework->modelPool`) read as a size and turned a clean answer
into a two-way ambiguity.

### `layout_check.py` — the arm64 defect the truncation gate cannot see

`ptrwidth_check.sh` counts pointer **truncation**. This counts the defect truncation is a symptom
of: the recovery encodes **32-bit struct layouts**. 128 of 151 structs change size at 64-bit
pointer width — `sizeof(_McdGeometry)` is 16 on i386 and 32 on arm64 — so a hardcoded byte offset
addresses somebody else's memory with nothing truncated and clang silent.

```bash
python3 tools/layout_check.py /tmp/kd_out/allobj /tmp/kd_build
```

~1 min, needs no arm64 hardware. It **bounds** the job; `test/lp64_run.sh` names the individual
defects by running them.

### `assoc_scan.py` — where the association defect can hide

Ghidra prints a right-leaning float `+` chain flat, C parses it left-leaning, and float addition is
not associative. On i386's 80-bit x87 the two are exactly equal — measured 0 in 2,000,000 — and
under storage precision (wasm32, armv7, arm64) they differ in 31%. **Every behavioural gate here
runs on i386 and all of them are structurally blind to it.**

```bash
python3 tools/assoc_scan.py /tmp/kd_out/allobj /tmp/kd_build
python3 tools/assoc_scan.py /tmp/kd_out/allobj /tmp/kd_build -v     # the individual sites
```

575 sites, 51 objects, 66 of them index-permuted dot products. **It decides nothing** — no gate here
can falsify a repair — it bounds and ranks. Read `../proven.txt` `ASSOC-ON-I386` before §11 item 2a.

### `spill_scan.py` — a rounding Ghidra folded away

gcc 3.2 keeps a float intermediate in an x87 register at 80 bits, spills it to a 4-byte slot —
**which rounds it** — and reloads. Ghidra folds the store/reload into one expression, so the
recovery carries 80 bits where the shipped code carried 32. The fingerprint is the variable Ghidra
declared for the slot and never used.

```bash
python3 tools/spill_scan.py /tmp/kd_out/allobj /tmp/kd_build $LAB/allobj
```

229 confirmed, all in the build. **Not a wasm-only concern** — it changed a CONTACT COUNT on i386,
in a live match.

### `wasm_members.py` — is the recovered Karma actually IN the .wasm?

A clean `cmake --build` says the sources compiled and the archive was offered to the linker. It does
not say one member is in the binary: a static archive contributes only what something references.

```bash
python3 tools/wasm_members.py ../build-wasm-karmadecomp-debug
python3 tools/wasm_members.py ../build-wasm-karmadecomp-debug -v    # per-member counts
```

Current answer: **125 of 146 contribute at least one symbol**, 3 define only data (which linking
makes unmeasurable this way), 18 are unreferenced — and those 18 are exactly the profiling, debug,
XML-output and unused-constraint objects §3b retires.

> **It refuses a `-g0` build.** Every `*-perf` preset strips the name section, so `llvm-nm` returns
> nothing and every member scores zero — a measurement artefact that reads exactly like a
> catastrophic finding. It was the tool's own first output. Point it at a `*-debug` tree.

---

## The arm64 post-passes — NOT part of the 95-second pipeline

They need the NDK, they **edit in place**, and §4's output is not arm64-correct source until they
have run. **Run them on a copy**, in this order — or just use `test/lp64_pipeline.sh`, which does
the copy, both passes, the acceptance test and the harness in the right order.

```bash
./test/lp64_pipeline.sh                      # all of the below, gated

# or by hand:
cp -a /tmp/kd_out /tmp/kd_lp64
python3 tools/fix_baked_sizeof.py /tmp/kd_lp64/allobj /tmp/kd_build   # 102 allocs + 7 pool strides
python3 tools/fix_ptrwidth.py    /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> 3864 narrow pointer cast(s) widened in 94 object(s)
KD_OUT=/tmp/kd_lp64 ./test/lp64_run.sh
```

`fix_ptrwidth.py` needs the Android NDK; it defaults to
`/home/ion/Android/Sdk/ndk/30.0.14904198/...` and takes `KD_NDK` to override.

**The acceptance test for each is that all 145 objects recompile BYTE-IDENTICAL at i386** —
`intptr_t` *is* `int` at 32-bit pointer width, so both are no-ops there by construction. Run it:

```bash
INC=$MT/include
CF="-m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion \
    -Wno-incompatible-pointer-types -DLINUX -Iinclude -I$INC -I$INC/McdCommon \
    -I$INC/McdPrimitives -I$INC/McdFrame -I$INC/MeGlobals -I$INC/MdtBcl \
    -I$INC/MdtKea -I$INC/Mst -I$INC/MeApp"
for o in /tmp/kd_build/*.o; do b=$(basename "$o" .o)
  gcc $CF -c -o /tmp/x.o /tmp/kd_lp64/allobj/$b.c && cmp -s "$o" /tmp/x.o || echo "DIFFERS: $b"
done
#   last run: 145 objects, 0 compile failures, 0 byte differences
```

⚠ **THE FILE HAS TO KEEP ITS NAME.** gcc records the source basename in an `STT_FILE` symbol, so
compiling identical text out of `/tmp/t.c` yields a different object. That reads exactly like a
codegen change and cost a bisection round here: the disassembly was identical and `cmp` still
disagreed. The loop above is right because it compiles `$b.c`; a scratch file is not.

That test earns its keep: it caught `fix_baked_sizeof` changing `CxSmallSort` by 64 bytes, because
`count * sizeof(T)` is unsigned where `count * 0x98` was `int`. The fix is `(int)sizeof(...)`; the
lesson is that "no-op by construction" still gets measured.

> ★ **AND THE SEQUEL IS WHY `fix_baked_sizeof.py` NOW COMPILES EVERY SITE ITSELF.** The very next
> site to need this wanted the OPPOSITE spelling. `pMVar1->bucketCount << 2` in
> `McdModelPairManagerHashCreate` is byte-identical as `(int)(count * sizeof(T))` and DIFFERS as the
> prescribed `count * (int)sizeof(T)`. Both compute the same value on every target; which one gcc
> schedules the same way is a property of the surrounding function, not a rule anyone can write
> down. So the tool now tries each candidate, keeps whichever reproduces the baseline object byte
> for byte, and declines the site if neither does. The corpus-wide test above stops being the thing
> that catches this tool's mistakes and becomes a confirmation of something already established.

### `fix_ptrwidth.py` — widen the punned casts

The recovery puns pointers through integer slots constantly (`*(T *)((int)base + K)`). Lossless at
32-bit; truncating at 64. Takes its site list from **clang's own diagnostics**, because nothing here
executes arm64 code and a heuristic wrong 1% of the time would be undetectable.

### `fix_baked_sizeof.py` — an allocation size frozen at the i386 value
`(MeMemoryAPI.create)(0x234)` where `0x234` is 564 is `sizeof(MdtWorld)` **on i386**; at LP64 the
struct is 880, so the first allocation is 316 bytes short and the next statement writes past it.
Nothing is truncated, no cast is narrowed, and neither static gate counts this shape at all.

It now covers three spellings of one defect:

| shape | example | why the first version missed it |
|---|---|---|
| `create(LITERAL)` | `create(0x234)` | — |
| `create(COUNT * K)` | `create(count * count * 0x1c)` | the count had to be ONE identifier |
| `MePoolAPI.init(pool, n, K, align)` | `init(&w->bodyPool, n, 0x240, 0x10)` | not an assignment, and the size is argument **three** |

**The pool form is the one that matters most and it is the hardest to type.** `0x240` is
`sizeof(MdtBody)` at i386, so at LP64 every body in the pool overlaps the one before it — and the
crash surfaces two files away in `MeDictInsert`, with nothing in the backtrace pointing at the pool.
There is no assignment target to take a type from, so the element type is pinned by three facts:

1. **what `getStruct` hands back**, corpus-wide (the pool is initialised in one object and drawn
   from in another), following a local through the member it was just stored into;
2. **the i386 literal** must equal `sizeof` of that type;
3. **the shipped amd64 build** must pass that type's win64 `sizeof` in the same function
   (`amd64_oracle.py`).

⚠ **FACT 1 IS WRONG FOR ONE OF THE SEVEN AND THE TOOL HAS TO SURVIVE THAT.** `constraintPool`'s
`getStruct` is typed as the base `MdtConstraint` (352 bytes) and the pool is sized for the largest
variant, `MdtContact` (492 → 624). A tool that trusted its primary fact would have sized the
constraint pool for the base class. Facts 2 and 3 catch it, and the fallback — the only type whose
i386 size is the stride AND whose 64-bit size the shipped build is seen to pass — names `MdtContact`
uniquely. **Fact 2 alone would not:** fourteen metoolkit types are 20 bytes at i386.

Current output: **120 allocations + 7 pool strides rewritten, 0 pool sites declined**, every one
compiled and compared against its baseline object.

★ **AND THE BYTE-IDENTITY GATE CANNOT VALIDATE THE TYPE — it never could.** The rewrite is only
offered when `literal == sizeof_i386(T)`, so *any* type of the right i386 size produces the
identical i386 object and a **different** size at 64-bit, which is the whole point of the change.
The first version of this tool read the target's declaration from the whole FILE, and Ghidra names
a local `pMVar1` in every function it decompiles — `McdModelPairManager.c` declares it four
different ways. **Seven sites shipped with the wrong type and all seven passed the gate**;
`MeXMLTree.c:285` and `:286` had `Attribute` and `AttributeNode` swapped with each other. Two
changes fix it: the declaration is read from the site's own function, and the chosen type is
**confirmed against the shipped amd64 build** — its win64 `sizeof` must be a constant that same
function passes. 107 of 120 confirm; 13 sit in functions that build does not contain and are
counted separately rather than quietly called confirmed.

⚠ **The confirmation may only VETO when the count is a compile-time constant.** `create(sizeof(T))`
and `create(4 * sizeof(T))` reach the allocator as an immediate; `create(n * sizeof(T))` does not,
because MSVC strength-reduces a runtime multiply — `n * 48` comes out as `lea (%rax,%rax,2)` then
`shl $4` and the number 48 is nowhere in the function. Vetoing on those rejected five correct sites.

> **The confirmation found a defect of its own.** Ghidra declares the target with the BASE handle
> while the code allocates the DERIVED struct: `McdBoxCreate` reads as `2 * sizeof(McdGeometry)`
> = 32, right at i386, while the amd64 build passes `0x30`. Same for `McdNull`, `McdConvexMesh`,
> and `McdCylinder` (whose `McdCylinderID` is 16 and does not even divide 28). The two-build pin
> names the concrete type and all seven geometry types now allocate their own `sizeof`.

### `fix_word_loops.py` — a table-zeroing loop counting in 4-byte words

Ghidra renders `memset(table, 0, n)` over a struct table as a loop that walks ONE FIELD at a time
and counts in words. That is correct on i386 for one reason — every field of `McdInteractions` is
four bytes there — and at LP64 the step `&p->goodbyeFn` becomes eight while `>> 2` still divides by
four, so the loop walks twice the length of the table.

```bash
python3 tools/fix_word_loops.py /tmp/kd_lp64/allobj /tmp/kd_build
#   -> 2 repaired, 0 declined
```

★ **This was recorded as possibly unfixable under the byte-identity gate, and that was never
measured.** The reasoning — the i386 code is correct only by a coincidence of field widths, so a
repair must change the arithmetic rather than re-spell it — is true and the conclusion does not
follow. The trip count is constant-folded, and `(uint)(A * A * 0x1c) >> 2` and
`(uint)(A * A * sizeof(T)) / (uint)offsetof(T, goodbyeFn)` compile **byte-identically**, because
both fold to `A * A * 7`. Ask the compiler.

Only **two** of the nineteen `p = (T *)&p->field` sites are this defect; in the rest the step IS the
whole struct, which is ordinary array iteration and scales on its own. The tool checks that and
skips them.

> ⚠ **The step is not always a direct member.** `CxSmallSort::New` walks a base-class subobject —
> `pCVar4 = (CxSmallSortRep *)&(pCVar4->super_Link).mPrev;` — so the parenthesised, dotted path has
> to be part of the pattern. Matching only `&p->field` found McdFrame's loop and silently missed
> the one that actually overruns the `CxSmallSortRep` table by 800 bytes. Three sites total.

### `fix_literal_offsets.py` — a struct field addressed by a baked byte offset

```bash
python3 tools/fix_literal_offsets.py /tmp/kd_lp64/allobj /tmp/kd_build
#   -> 22 rewritten, 0 declined, 1545 out of scope
```

`*(code **)((char *)p + 200)` is `CxSmallSort::mAABBUpdateFn` **on i386**; at LP64 it is 392. The
repair is a pure address re-spelling — `*(T *)&p->FIELD` — so the cast and the expression's type are
untouched and only the address computation moves from a number to something the compiler recomputes.

⚠ **`layout_check.py`'s "OFFSET 128" is two classes added together.** Only **27** are struct-field
offsets (21 in `McdSpace.c`, 6 in `ReadWriteKeaInputToFile.c`); the other ~100 are Ghidra's invented
stack frames (`(int)&local + K`, MdtBcl 70 and MdtMainLoop 25), which are a decompilation defect
rather than a layout one. Quoting 128 as LP64 exposure overstates it 4.7×.

⚠ **The declared type is the wrong one, for the fourth time.** `pMVar1` is `McdSpaceID` =
`McdSpace *`, and `McdSpace` is opaque — the object is really a `CxSmallSort`. The concrete type is
inferred from the offsets: the struct on which **every** offset used against that declared type in
the file lands on a real field. Requiring all of them is what makes it a measurement; one offset
alone fits dozens of structs, as the tool's own decline lines show. Confirmed against the shipped
build — amd64 `McdSpaceAxisSortCreate` touches `0x50` and `0x188`, exactly `mManager` and
`mAABBUpdateFn` at 64-bit.

⚠ **Parameters are not declarations**, and missing that repaired 2 of 21 sites. Ghidra writes locals
as `T x;` at the top of a body and parameters in the signature; the other 19 were reported as
"declined" for want of a type written down three lines above them. And **1,545 sites are out of
scope, not declined** — a `void *` base carries no structure, and reporting those as refusals buries
the ones that can be repaired.

---

## The investigative tools — reach for these when something is wrong

### `frame_offsets.py` — what does Ghidra say lives at this stack offset?

Ghidra spells a stack slot's address `&stack0xffffff6c` and does not declare it. `DumpDecomp.java`
emits `<object>.locals` beside each dump — one row per local with stack storage and the offset
Ghidra **assigned** it, out of `HighFunction.getLocalSymbolMap()`. This joins the two, so a
`stack0x` site is answered from Ghidra's own symbol map rather than from its naming convention.

```bash
python3 tools/frame_offsets.py $LAB/out14 MdtBcl MeMath
```

### `mk_trace_obj.py` — turn one function into a self-comparing shadow

Rewrites one exported function of a recovered object so it calls both the recovered body and the
**shipped** one on identical inputs, reports the first divergences, and returns the shipped answer.

```bash
python3 tools/mk_trace_obj.py /tmp/kd_out/allobj/IxCylinderCylinder.c \
        kd_OverlapCylCyl _Z13OverlapCylCylPK11lsTransformS1_ffff
```

**No renaming and no mixed link** — an earlier attempt used `objcopy --redefine-sym`, which renames
the call sites in the same object too, so the shipped caller went straight to the original and the
wrapper was never reached. It printed nothing and looked like a harness that had run.
`test/trace_cylcyl.sh` is the worked use.

### `find_cylinder_geom.py` — does any shipped asset define this geometry?

Answers a scoping question that retires five registered collision pairs. **Grep cannot answer it**:
`FKAggregateGeom` is serialised by a hand-written `operator<<` that writes four `TArray`s in a fixed
order with **no field names**, so `CylinderElems` appears in no package. This parses the packages
instead.

```bash
python3 tools/find_cylinder_geom.py /home/ion/ut2004-assets
#   -> ...  KMeshProps143  {'sphere': 0, 'box': 0, 'cylinder': 1, 'convex': 0}
```

Answer on the shipped content: **114 cylinder collision elements exist.** Note the limit — it tells
you the content exists, not that it is ever simulated (`DM-Insidious` has one and never collides
it).

> This is the standing example of a search that cannot find what it is looking for. Grepping 2,015
> packages for `CylinderElems` returned 0 and read like an answer. **Run a negative search against a
> known positive first** — the same grep returns 0 for `BoxElems`, which certainly are in there.

### `gscripts/` — the Ghidra headless scripts

`ParseKarmaHeaders.java` (preScript, consumes `kd_protos11.h`) and `DumpDecomp.java` (postScript,
emits the per-function dump, `stats.csv` and `<object>.locals`). Ghidra lives at
`/home/ion/tools/ghidra_12.1.3_PUBLIC` (Java 21, headless only). `../HANDOVER.md` §5 has the
full-corpus invocation and `KD_GHIDRA_OPTS`. **The scripts must be COPIED to
`/home/ion/tools/karma-lab/gscripts/`** — Ghidra reads them from there, not from this repo.

A single-object re-dump, which is the form you actually want when testing a hypothesis:

```bash
cd /home/ion/tools/karma-lab
cp /home/ion/engines/engine-ut2004/karma-decomp/tools/gscripts/*.java gscripts/
mkdir -p one_obj out_verify gproj_verify && cp allobj/McdSpace.o one_obj/
export KARMA_PROTOS=/home/ion/tools/karma-lab/kd_protos11.h
export KARMA_OUTDIR=/home/ion/tools/karma-lab/out_verify
/home/ion/tools/ghidra_12.1.3_PUBLIC/support/analyzeHeadless \
  gproj_verify Proj -import /home/ion/tools/karma-lab/one_obj \
  -scriptPath /home/ion/tools/karma-lab/gscripts \
  -preScript ParseKarmaHeaders.java -postScript DumpDecomp.java -deleteProject
#   -> out_verify/McdSpace.o.{c,locals} + stats.csv
```

> **THE FRONT END IS REPRODUCIBLE, and that had never been demonstrated.** The dump above came back
> **byte-identical to `out14/McdSpace.o.{c,locals}`** — same Ghidra, same scripts, same
> `kd_protos11.h`, same answer. §5 warns that a re-run "changes every object at once" and that
> `out5`–`out14` must be kept; that is true when the PROTOS or the SCRIPTS change, which is what
> those re-runs were. With the inputs held fixed the decompiler is deterministic, so `out14` is a
> cache rather than an irreplaceable artefact. Full corpus is 75–120 minutes; one object is under a
> minute.

