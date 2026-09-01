# `decomp/tools/` — every generator and analyser, with the command

**One block per tool: what it answers, and the exact invocation.** Nothing here is a guess — if a
command is written down it has been run. Read `../HANDOVER.md` first for what the project is; this
file is the index you come back to.

Conventions used throughout:

```bash
cd decomp
MT=$(realpath ../metoolkit)     # the shipped SDK, IN this repository
LIB=$MT/lib.rel/linux_single_gcc3.2        # its i386 archives
LAB=../lab                      # Ghidra dumps + the 153 shipped .o
```

**You do not have to set those.** Every tool resolves them itself through `kd_paths.py`, which
finds the repository by walking up for a marker rather than by counting `..` — so a file can move
without breaking. The variables above exist so the commands below read clearly. Override with
`METOOLKIT_DIR`, `KD_LAB_DIR`, `KD_DUMP_DIR`, `KD_OBJ_DIR`, `KD_PROTOS`; anything that needs the
game takes `UT2004_ENGINE_DIR` / `UT2004_BUILD_DIR` / `UT2004_ASSETS_DIR` / `UT2004_RUN_DIR` and
fails loudly rather than guessing.

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

## Where things are — read this before adding a tool

### `kd_paths.py` — the one place a tool learns where anything is

```python
import kd_paths
kd_paths.METOOLKIT_DIR   kd_paths.MT_INC   kd_paths.MT_LIB   kd_paths.AMD64_LIB
kd_paths.MD              kd_paths.MD_INC   kd_paths.MD_SRC
kd_paths.LAB_DIR         kd_paths.DUMP_DIR kd_paths.OBJ_DIR  kd_paths.PROTOS
kd_paths.require_metoolkit()  kd_paths.require_lab()  kd_paths.require_ut2004('assets')
```

`decomp/lib/kd-paths.sh` is the shell half and every name agrees. **Do not hardcode a path and do
not compute one with `..` arithmetic.** Before stage 2.44 half the harnesses resolved the SDK as
`$HERE/../Thirdparty` and half as `$HERE/../../Thirdparty`, because `HERE` meant the project root
in some scripts and the test directory in others; both were right and both broke the moment a file
moved. These resolve the root by walking up for a marker, so depth is not a variable.

⚠ `HERE` in the `fix_*.py` passes is `kd_paths.MD` — the PRODUCT root, because `HERE/include` is
where `kd_compat.h`, `kd_karma.h` and `kd_types.h` live and those are what their size and offset
probes have to see. When the headers moved in 2.44, `fix_baked_sizeof.py` caught it itself:
`SELF-CHECK FAILED — sizeof(*(MdtWorldID)0) measured None`. That is what the self-checks are for;
`ghidra_clean.py` had three more references that no self-check covered and 14 objects stopped
compiling until they were found.

### `split_libraries.py` — lay the recovery out as MathEngine's sixteen libraries

```bash
python3 tools/split_libraries.py /tmp/kd_lp64/allobj          # refresh metoolkit_decomp/src
python3 tools/split_libraries.py /tmp/kd_lp64/allobj --check  # the gate: is the tree current?
python3 tools/split_libraries.py --map                        # object -> archive, from the SDK
```

The map is **read off the shipped archives** with `ar t`, never guessed. All 145 recovered objects
map to exactly one, so there is no tie to break.

⚠ **THE BUILD DIRECTORY IS THE AUTHORITY ON WHAT SHIPS.** `recover.py` writes a `.c` for every
object it attempts, including the three that do not compile (`MeASELoad`,
`MeFGeometryFromMesh`, `McduDebugDraw`). This iterates the `.o`, not the `.c`, and says out loud
what it left out — laying out the other three would hand every consumer sources that do not build,
and the split itself would succeed.

⚠ **SPLIT FROM `/tmp/kd_lp64`, NOT `/tmp/kd_out`.** The raw recovery encodes i386 struct layouts.
Using it is not a visible mistake: it builds, every 32-bit target is byte-identical, every i386
gate passes, and only the 64-bit ABIs are wrong.

⚠ `version` is in EVERY archive — MathEngine compiled a per-library version stamp — so "this
member is in more than one archive" is a normal fact about the SDK. It is only an error for an
object being laid out, which is where the check lives.

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
python3 tools/gen_typedb.py $LAB/allobj --public-headers $MT/include -o ../metoolkit_decomp/include/kd_types.h
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
python3 tools/dropin_gap.py $UT2004_ENGINE_DIR/build-native-karma /tmp/kd_build $LAB/allobj
#   -> 0 shipped member(s), 0 symbol(s); 134 recovered member(s) in the closure

python3 tools/dropin_gap.py $UT2004_ENGINE_DIR/build-native-karma /tmp/kd_build $LAB/allobj \
        --status /tmp/kd_recover.log            # annotate each gap member with why it is held
python3 tools/dropin_gap.py $UT2004_ENGINE_DIR/build-native-karma /tmp/kd_build $LAB/allobj \
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
> `$UT2004_ENGINE_DIR/build-native-karma/` is therefore the one build tree deliberately kept in the working
> directory. Everything else under `build-*/` is reproducible from `BUILD.md` and was deleted.
> Rebuild it with `cmake --preset native-karma && cmake --build --preset native-karma -j"$(nproc)"`.

### `reachable.py` — is this object in scope at all?

Which Karma objects can UT2004 actually reach, at symbol level. An object nothing reaches is **out
of scope, not a to-do** — §3b retires 31 permanently on this tool's output.

```bash
python3 tools/reachable.py $UT2004_ENGINE_DIR/build-native-karma /tmp/kd_members
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
python3 tools/code_call_check.py metoolkit_decomp/src      # 0
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
cmake --preset native -B $UT2004_ENGINE_DIR/build-native-census -DKD_CENSUS=ON -DCMAKE_EXE_LINKER_FLAGS=-no-pie
cmake --build $UT2004_ENGINE_DIR/build-native-census -j"$(nproc)"

cd $UT2004_RUN_DIR/System && cp <binary> census.bin
KD_INSTR_OUT=/tmp/c1.txt timeout 150 xvfb-run -a ./census.bin \
  "test-karma-1?game=Onslaught.ONSOnslaughtGame?TimeLimit=0?SpectatorOnly=1?NumBots=2" \
  -SOFTWARERENDERER -nohomedir
# … repeat per map; the union is what matters

python3 tools/census_report.py /tmp/c1.txt /tmp/c2.txt … $UT2004_ENGINE_DIR/build-native-census/…/ut2004-pixo.bin
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
        $UT2004_ENGINE_DIR/build-native-census/Source/SDLLaunch/ut2004-pixo.bin \
        --build $UT2004_ENGINE_DIR/build-native-karma --members /tmp/kd_members
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

~1 min, needs no arm64 hardware. It **bounds** the job; `test/standalone/lp64_run.sh` names the individual
defects by running them. `--emit-sites` lists what the SUSPECT column counts — object, line, column
and the element type each site resolves through — which is what `fix_index_layout.py` was written
against.

### `interaction_types.py` — which two geometries does this `Ix*` function handle?

```bash
python3 tools/interaction_types.py /tmp/kd_out/allobj
#   -> 26 interaction functions typed by their registration
```

Evidence only; it edits nothing. `fix_literal_offsets.py` types an opaque pointer from the geometry
a **file** registers and gives up when a file has two — which is every `Ix*` object, and which its
own notes recorded as *"44 declined, an Ix\* function handles TWO geometries"*. One of those 44 is
`McdSphylPlaneIntersect`, which `scene_ragdoll` runs 7,298 times a step:

```c
pvVar9 = McdModelGetGeometry(p->model1);
fVar2  = -*(float *)((kd_iptr)pvVar9 + 0x14);      /* McdSphyl::mHalfHeight */
```

0x10 and 0x14 are `mRadius` and `mHalfHeight` at i386 and **32 and 36** at LP64, so at 64-bit the
capsule's radius comes out of the middle of its own base class — bytes 16..19 of `_McdGeometry`,
the low half of the `prev` **pointer**. It is an address read as a float, it changes with ASLR, and
it is the read `proven.txt` LP64-ADDRESS-DEPENDENT measured and could not name.

★ **The type is written down, in the registration.** `McdSphylGetTypeId()` returns 5,
`McdPlaneGetTypeId()` returns 3, and `McdSphylPlaneRegisterInteraction` installs
`intersectFn = McdSphylPlaneIntersect` then calls
`McdFrameworkSetInteractions(frame, 5, 3, &interactions)`. So `model1` is an `McdSphyl` and
`model2` an `McdPlane`, with nothing inferred. `fix_literal_offsets.py` and `fix_index_layout.py`
both consume this. A function registered for two different pairs is dropped, not arbitrated.

⚠⚠ **A SECOND ACCESSOR ON THE SAME TYPE IS NOT A CONFLICT, AND TREATING IT AS ONE COST THE WHOLE
CONVEX-MESH FAMILY** (2026-08-31). `McdConvexMesh.c` exports both `McdConvexMeshGetTypeId` and
`McdConvexMeshMeshGetTypeId`, and both return 7. Stripping `GetTypeId` gives the tags
`McdConvexMesh` and `McdConvexMeshMesh`; the "claimed by two tags is evidence about neither" rule
read that as a conflict, and **id 7 dropped out of the table entirely** — so
`McdSphylConvexMeshIntersect` and its seven siblings were never typed, and `fix_literal_offsets`
left every geometry offset in `IxConvexPrimitives.c` at its i386 value:

```c
ConvexHullNSegment((McdConvexHull *)((kd_iptr)pvVar4 + 0x10), pos, axis, ...)
```

`McdConvexMesh::mHull` is at **16 here and 32 there**, so the hull pointer was sixteen bytes short
and `ConvexHullVoronoiRegion` faulted on the face table. Measured on the LP64 vehicle at frame 27
of a `ktrace` run. ★ **The tie-break is the TYPE DATABASE, not a name rule**: a tag that names a
struct the oracle declares is a type, one that does not is an accessor whose name happens to end
that way. `McdConvexMesh` has a body; `McdConvexMeshMesh` does not. One survivor types the id;
none or two leaves the conflict standing. 26 → 28 functions typed.

⚠ **AND A LATENT CRASH CAME WITH IT.** `out[fn] = None` is the dropped marker, and a THIRD
registration of the same function then indexed `None[0]`. Nothing had ever been registered three
times until the ConvexMesh family started being typed — `McdNullIntersect` is registered against
every type there is.

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
python3 tools/wasm_members.py $UT2004_ENGINE_DIR/build-wasm-karmadecomp-debug
python3 tools/wasm_members.py $UT2004_ENGINE_DIR/build-wasm-karmadecomp-debug -v    # per-member counts
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
have run. **Run them on a copy**, in this order — or just use `test/standalone/lp64_pipeline.sh`, which does
the copy, every pass, the acceptance test and the harness in the right order.

```bash
./test/standalone/lp64_pipeline.sh                      # all of the below, gated

# or by hand — THE ORDER IS LOAD-BEARING, see each tool's block below:
cp -a /tmp/kd_out /tmp/kd_lp64
python3 tools/fix_baked_sizeof.py    /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_strides.py         /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_literal_offsets.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_derived_fields.py  /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_index_layout.py    /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_arena_carve.py     /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_vtable_offsets.py  /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_ptrwidth.py        /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_narrow_pointers.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_align_masks.py     /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_frame_slots.py     /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_pool_reserve.py    /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_narrow_loads.py    /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_global_array_index.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_list_walk.py       /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_word_indexed_struct.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_baked_sizeof.py    /tmp/kd_lp64/allobj /tmp/kd_build $MT --field-allocas-only
python3 tools/fix_element_stride.py  /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_member_base_walk.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_callback_context.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_alloca_elem.py     /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/fix_block_copy.py      /tmp/kd_lp64/allobj /tmp/kd_build $MT
python3 tools/check_frame_bounds.py  /tmp/kd_lp64/allobj /tmp/kd_build   # must read 0
KD_OUT=/tmp/kd_lp64 ./test/standalone/lp64_run.sh
```

⚠ **`fix_list_walk` and `fix_block_copy` go after `fix_narrow_loads`, in that order, and
they are the two passes the OFFLINE SCENES COULD NOT HAVE FOUND.** Both defects were found
by running UT2004 on the x86-64 Linux vehicle (`-DUT_ALLOW_64BIT=ON`) with all three scenes
already byte-identical at both widths. `fix_list_walk` needs the cursor's initialiser already
spelled as an offsetof against a **named** field, which is `fix_literal_offsets` /
`fix_derived_fields`' output, and cast through `kd_iptr`, which is `fix_ptrwidth`'s.

⚠ **`fix_block_copy` goes LAST, after even `fix_narrow_loads`.** It is the only pass that
does not re-spell an expression: it keeps the i386 text verbatim under
`#if __SIZEOF_POINTER__ == 4` and puts the corrected body in the `#else`. Every pass above
reads these sources as plain C, so running it earlier hands them a preprocessor conditional
to parse and a second, hand-written copy of each repaired loop to "repair" again.

★ **THE GUARD IS A LAST RESORT AND THE TWO NEW PASSES SHOW BOTH SIDES.** `fix_list_walk`
needs none — `((PElementNode *)p)->next` *is* `p[1]` at i386, same address and same four-byte
access, so one spelling serves both widths and the object is reproduced exactly.
`fix_block_copy` cannot have one, and that was measured rather than assumed: its rodata
template has **eight slots per handler at LP64 while the destination has six words**, and the
per-field expansion compiles to a different i386 object (22800 → 22824 bytes). Because
`__SIZEOF_POINTER__` is 4 on **wasm32** as well, the guard also makes the web artifact
provably unchanged — verified at 146/146 byte-identical, not argued.

⚠ **`fix_index_layout` goes AFTER `fix_derived_fields`, not before.** They are the same defect
class; the first types the base pointer per FILE and names the concrete field with its own declared
type, which is the better repair, and the second takes only what it reports as ambiguous. Running
them the other way round takes the sites away from the better repair.

⚠ **`fix_narrow_loads` goes LAST.** It reads clang's diagnostics over the finished text and writes
`*(kd_iptr *)p`, which is the spelling `fix_narrow_pointers` and `fix_align_masks` key on.

⚠ **The last four are ordered and not interchangeable.** `fix_ptrwidth` writes the `kd_iptr`
that the next two key on; `fix_narrow_pointers` widens the locals whose alignment masks
`fix_align_masks` then has to recognise, and a mask on a widened local has no cast in it to
match. `fix_pool_reserve` is last for a different reason: it **learns** the element size from
the allocations `fix_narrow_pointers` repairs, and before that pass `NAZ` and `NR` are spelled
identically, so running it early makes it print a clean, wrong zero. It refuses that case.

`fix_ptrwidth.py`, `fix_narrow_pointers.py`, `fix_narrow_loads.py` and `fix_index_layout.py` need the Android NDK; they default to
`$ANDROID_HOME/ndk/30.0.14904198/...` and take `KD_NDK` to override.

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

**A fourth spelling: `alloca`.** `addedBodies = (MdtBody **)(... alloca((size_t)(n) * 4 + 0))` — the
`4` is `sizeof(MdtBody *)` at i386, so these pointer arrays come back **half size** at LP64 and every
write past the midpoint runs off the end. That is what `MdtUpdatePartitions` was still failing on
after its arena was repaired, at four lines none of them near the allocation. The element type is in
the cast that consumes the block, or in the variable it lands in. 24 sites carry a baked multiplier;
the declines are honest (`float * is 4, the stride is 12` is a `MeVector3` buffer whose variable is
typed `float *`). ⚠ Confirm-only, never veto — the count is a runtime value and MSVC strength-reduces
the multiply.

Current output: **125 allocations + 7 pool strides rewritten, 0 pool sites declined**, every one
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

★ **A FIFTH SPELLING — `qsort`'s ELEMENT SIZE, AND IT IS THE ARM64 RAGDOLL CRASH** (2026-08-31).

```c
qsort(partArray, asset->partCount, 4, _MeFAssetPartSortFunc);   MeFAsset.c:920
```

`partArray` is `MeFAssetPart **` — an array of POINTERS — so `4` is `sizeof(*partArray)` on i386
and eight at LP64. qsort walks in four-byte steps over eight-byte elements, so every "element" the
comparator is handed after the first is the top half of one pointer welded to the bottom half of
the next. Nothing truncates and no diagnostic fires; the comparator just dereferences a spliced
address. Measured on a OnePlus 6 loading a ragdoll:

```
signal 11 (SIGSEGV), SEGV_MAPERR, fault addr 0x6390f87800000073
#00 <the comparator>  #01 local_qsort+1012  #02 MeFAssetGetPartsSortedByName
#04 KInitSkeletonKarma  #05 KInitActorKarma  #06 AActor::setPhysics
```

`libUT2004.so` sits around `0x73_00000000` there, so an element reads `0x00000073_6390f878` and
the fault address is those two words exchanged. Four sites in `MeFAsset.c` — parts, geometries,
models and joints — all on the ragdoll-creation path.

⚠ **THE TYPE NEEDS NO LOOKUP AND MUST NOT HAVE ONE.** `sizeof(*partArray)` names the element
through the array itself: no struct to resolve, no typedef to guess, and the same expression at
every pointer width. The first argument therefore has to be a plain identifier; `MdtLOD.c`'s
`qsort(*(void **)(&(*kd_argslot_ffffffc4)), ...)` is not one and is declined and reported rather
than rewritten through a cast whose pointee is `void`.

⚠⚠ **"ONLY REWRITE WHAT MOVES" NEEDED ITS OWN MEASUREMENT, AND THE FIRST ONE WAS BROKEN BY THIS
PROJECT'S OWN `STT_FILE` TRAP.** With no type to compare, `moves_at_lp64` compiles the file at
`-m64` before and after; written as `a_<fn>` and `b_<fn>` the two objects differ **on the source
name alone**, so it returned True for everything and the test silently stopped testing. It passed
`McdPolygonSort`, which sorts `MeVector3` — twelve bytes at every pointer width and already
correct. Two directories, one name; that site is now correctly left alone.

★ **AND `MePoolxInit` IS THE SAME FACT ONE ARGUMENT ALONG**, so it goes through the same code.

```c
MePoolxInit(&s->nodepool, nodemem, 0x18, maxnode);          MeSet.c
```

`MePoolxInit(MePoolx *p, void *memory, int recsize, int numrec)` is a **direct call with a
different argument order** from `MePoolAPI.init`, so neither `POOL_INIT` nor `ALLOC` matches it.
`0x18` is 24 is `sizeof(MeDictNode)` at i386 and **48** at LP64, so the pool hands out node
addresses 24 bytes apart over 48-byte nodes and every node overlaps the one before it. Reached from
`McdConvexMeshPlaneCut` ← `McdGeometryInstanceGetSlice` — a convex mesh being sliced, i.e. a
**vehicle** — and measured on a OnePlus 6 as `SEGV_ACCERR` inside `MeSetAdd+68` two minutes into an
Onslaught match, and as `MeDictInsert` on ONS-Torlan at x86-64.

⚠ **Its sibling call is CORRECT and must be left alone.** `McdGjkPenetrationDepth` passes `0x2c`,
and `McdGjkFace` is 44 bytes at **both** widths — all ints and floats. Two calls in the corpus, one
moves and one does not, which is exactly what the `-m64` measurement is for.

★ **A SIXTH — THE `alloca` WHOSE TYPE IS ON THE FIELD IT IS STORED INTO** (`--field-allocas-only`).

```c
(*(McdGeometryID *)((char *)pMVar9 + KD_OFFSET(McdTriangleList, list)))
    = (McdGeometryID)(kd_alloca_iVar3 = (char *)alloca((size_t)(n) * 0x18 + 0));
```

`ALLOCA` cannot see this — it wants the statement to open with a variable or a cast, and this one
opens with a dereferenced offsetof. `0x18` is 24 is `sizeof(McdUserTriangle)` at i386 and **48** at
LP64, so the triangle array is half the size the generator is about to fill. The type is
**declared**: `McdUserTriangle *list;` in the oracle — the assignment target names the type exactly
as it does for `MeMemoryAPI.create`, only spelled as an offsetof — and the literal must equal that
type's i386 size or the site declines.

⚠ **IT NEEDS A SECOND INVOCATION, LATE.** In the raw recovery that statement reads
`pMVar9[3].prev = ...`; the field has no NAME until `fix_literal_offsets` and `fix_index_layout`
have run, which is long after this pass. `--field-allocas-only` switches the other five rules off
so nothing is re-litigated over text five passes have since rewritten.

⚠ **WHY NOT LEAVE IT TO `fix_element_stride`.** That pass repairs three of the four triangle-list
allocas and cannot repair the fourth, for a good reason: its anchor is gated on
`fix_word_indexed_struct` having already typed an access in the same file as `((E *)v)->`, and
broadening that gate is measured-unsafe (`proven.txt` LP64-ANDROID-ARM64). `IxSphylPrimitives`
walks its triangles through BYTE cursors, so no such access exists and the gate correctly refuses.
This rule needs no gate of that kind because it does not infer the type at all.

### `fix_strides.py` — a table-zeroing loop counting in 4-byte words

Ghidra renders `memset(table, 0, n)` over a struct table as a loop that walks ONE FIELD at a time
and counts in words. That is correct on i386 for one reason — every field of `McdInteractions` is
four bytes there — and at LP64 the step `&p->goodbyeFn` becomes eight while `>> 2` still divides by
four, so the loop walks twice the length of the table.

```bash
python3 tools/fix_strides.py /tmp/kd_lp64/allobj /tmp/kd_build
#   -> 10 repaired, 0 declined   (4 loops + 6 byte cursors)
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

**The second shape is a self-advancing pointer**, and it was the last thing between the collision
side and a clean LP64 run: `pCVar5 = (CxSmallSortRep *)&pCVar5->mAABBMarkers[0].mOrdinate;` must
step two markers per axis. At i386 `offsetof(mAABBMarkers) + offsetof(mOrdinate)` is 40 and
`2 * sizeof(marker)` is also 40; at LP64 they are 72 and 80. The shipped amd64 `_Update` does
`add $0x50,%rbx` — 80 — which is the confirmation.

★ **The spelling that keeps byte-identity is the reusable part.** `(char *)p + 2 * sizeof(marker)`
is the obvious repair and it is **not** byte-identical: gcc re-allocates registers across the whole
function, 508 differing instructions, for the same address. Four spellings were tried and all four
differed. What works is to **keep the expression Ghidra wrote and add a correction term that is
provably zero on the shipped target** — the delta folds away at i386 and corrects the stride
everywhere else. Anchor-and-correct is general for this family.

⚠ **This tool reported a repair it did not make.** `path` is the output file and the new rule
shadowed it with the field path it was building, so the repaired text went to a file called
`mAABBMarkers[0].mOrdinate` in the working directory. It printed "4 repaired", `CxSmallSort.c` was
untouched on disk, and the harness kept reporting the same SEGV against a fix the report said had
landed. **Diff the source; do not read the summary.**

★ **A THIRD SHAPE: A TABLE WALKED BY A BYTE CURSOR.** gcc unrolled
`for (i = 0; i < 16; i++) pools[i].contactCount = 0;` four ways and Ghidra rendered the induction
variable as a byte cursor:

```c
iVar10 = 0xc;
do {
  *(undefined4 *)((int)&context->pools->contactCount    + iVar10) = 0;
  *(undefined4 *)((int)&context->pools[1].contactCount  + iVar10) = 0;
  ...
  iVar10 = iVar10 + 0x30;
} while (-1 < iVar11);
```

`0xc` is one `McdBatchContactPool` and `0x30` is four — 12 bytes each at i386 and **16** at LP64,
because the struct carries a `contacts` pointer. ⚠ **The element type is the struct that OWNS the
named field**, not the field's own type: `contactCount` is an `int`; what the cursor strides over
is the pool containing it.

⚠ **A Ghidra temporary is reused across the whole function.** `iVar10` is the pool cursor inside
one loop and an entry index three statements later, so a function-wide scan for its literal
assignments finds `iVar10 = iVar10 + 1` and declines the site as "not all multiples". The
cursor's arithmetic is a property of its **loop**, so that is the span that gets read.

⚠ **And the cast is not part of the pattern.** This pass runs *before* `fix_ptrwidth`, so the
sites say `(int)&…` here and `(kd_iptr)&…` afterwards; writing the later spelling into the
matcher made it find nothing at all — the same silent miss `fix_frame_slots`' trailing addend had.

⚠ **It is LATENT, and that is why it is written down.** Repairing it alone changed not one number
in any of the three scenes. Real by measurement — 12 against 16, and the cursor provably misses —
and **not** the cause of the divergence it was found while chasing.

> ⚠ **The step is not always a direct member.** `CxSmallSort::New` walks a base-class subobject —
> `pCVar4 = (CxSmallSortRep *)&(pCVar4->super_Link).mPrev;` — so the parenthesised, dotted path has
> to be part of the pattern. Matching only `&p->field` found McdFrame's loop and silently missed
> the one that actually overruns the `CxSmallSortRep` table by 800 bytes. Three sites total.

★ **A THIRD SHAPE, added 2026-08-30: the element type Ghidra lost from a cursor.**
`McdContactSimplify` walks the contact array with a local Ghidra put in the wrong stack slot — it
merged an `McdContact *` into an unrelated `McdContactLink` and used its `next` field:

```c
MStack_9c.next = (_McdContactLink *)((kd_iptr)MStack_9c.next + 0x28);   /* sizeof(McdContact) */
... *(float *)&((&(MStack_9c.next)->contact)[2]) ...                    /* index in POINTER units */
```

40 here and **48** there, and an index that steps 4 bytes here and 8 there while dereferencing a
four-byte float. It was the last thing between the three scenes and bit-identity: `boxes` matched
for 94 steps and then diverged, presenting as two contacts with *equal separation* in the opposite
order. The element type is taken from what the cursor was **assigned from**, and the literal must
equal that type's i386 size or the site declines — which is what correctly declines the other six
self-advancing cursors in the corpus (`poly1 + 4` over an `MeVector3` is one float, not one
element).

### `fix_literal_offsets.py` — a struct field addressed by a baked byte offset

```bash
python3 tools/fix_literal_offsets.py /tmp/kd_lp64/allobj /tmp/kd_build
#   -> 834 rewritten, 13 declined, 855 out of scope   (the second run adds 33 more)
```

`*(code **)((char *)p + 200)` is `CxSmallSort::mAABBUpdateFn` **on i386**; at LP64 it is 392.

★ **THE REPAIR SUBSTITUTES THE LITERAL AND NOTHING ELSE**, and the `*(T *)&p->FIELD` this file
used to prescribe is **not** byte-identical on the object that matters. Re-spelling the whole
address lets gcc common the base pointer across the function — `MdtBcl` came back **268 bytes
smaller**. What works is to leave the expression Ghidra wrote exactly where it is, including
the `(int)` cast `fix_ptrwidth.py` will widen later, and put a target-computed `offsetof` where
the number was. All 600 MdtBcl sites in **one** compile, byte-identical. It also means the cast
never has to be parsed, so `(MeReal (*) [4])((int)p + K)` and the 52 bare address computations
get the same repair as the 548 dereferences.

⚠ **`layout_check.py`'s "OFFSET 128" is two classes added together.** The other ~100 are
Ghidra's invented stack frames (`(int)&local + K`), which are a decompilation defect rather
than a layout one. Quoting 128 as LP64 exposure overstates it 4.7×.

⚠ **The declared type is often the wrong one**, and there are now three ways to get the right
one, tried in this order:

1. **the offsets themselves** — the struct on which every offset used against that declared
   type in this file lands on a real field. `pMVar1` is `McdSpaceID`; the object is really a
   `CxSmallSort`. Confirmed against the shipped amd64 build, which touches `0x50` and `0x188`
   — exactly `mManager` and `mAABBUpdateFn` at 64-bit.
2. **the installer** — `MdtBcl.h` declares `void *const constraint`, so that is the ORIGINAL
   API and no header can type it. Each of the sixteen `MdtBclAdd*` is registered as a callback
   on the type it takes (`(pMVar1->head).bclFunction = MdtBclAddBSJoint;`), corpus-wide,
   because the installer and the function it names are in different objects. Which PARAMETER
   is the one whose every baked offset lands on that type — `constraint` reads 21 of 21,
   `params` has no baked offsets at all. One hop further types `MdtBclAddContactGroup`'s list
   walker from the call it hands each element to.
3. **the callee's declared return type**, the only one that is not an inference at all —
   `MeDictFirst` returns `MeDictNode *`, and `0x14` is `data`, at 20 here and **40** there.
   ⚠ Per FUNCTION, not per variable: Ghidra assigns `pvVar5` from `MeChunkGetMem` at line 218
   and from `MeDictFirst` at 284, and a function whose known assignments name different
   structs is declined.

★ **The base is not always a variable, and the one that is not was the whole partitioner
failure.** `if ((*(byte *)((int)pMVar9->data + 0x1ec) & 2) == 0)` — `0x1ec` is
`offsetof(MdtBody, flags)` at i386 and **556** at LP64, so `MdtUpdatePartitions`'s VISITED test
reads a byte 64 short of the flag. `pMVar9->data` is a `void *` and carries no type; what names
it is the same FIELD read into a typed local elsewhere in the function. The field is the key,
not the variable.

⚠ **THE OFFSET MAP IS NESTED NOW, AND TWO THINGS HAD TO BE FIXED BEFORE IT WORKED.**
`MdtBSJoint` is *nothing but* `MdtConstraintHeader head`, so a flat map of its own members has
one entry. And `int bodyindex[MdtKeaMAXBODYCONSTRAINT];` did not match `\[\d*\]` at all, so
`+0xb4` — the field every one of the sixteen reads FIRST — was simply not in the map, which
read as "nothing fits" rather than as a missing member.

⚠ **AND RAISING THE DEPTH ALONE COSTS TWENTY REPAIRS.** Three levels of nesting means almost
anything covers almost any offset: `McdSpace.c`'s six went from fitting ONE type to fitting
**twenty-two**, declined as ambiguous, and the output fell from 22 rewrites to 2 — a silent
regression the byte-identity gate cannot see, because **a decline compiles perfectly**. The fit
is asked in three tiers, narrowest first: direct member starts, then array elements, then the
nested map. Expanding arrays *alone* brought the ambiguity back (three types fit), which is why
the narrowest tier is members' own offsets and nothing else.

⚠ **Parameters are not declarations**, and missing that repaired 2 of 21 sites. And **1,046
sites are out of scope, not declined** — a `void *` base carries no structure, and reporting
those as refusals buries the ones that can be repaired.

> The map is built for **all 191 struct tags at once**, in five gcc runs rather than 13,744, by
> putting thousands of probes in one file and reading the line numbers back out of the
> diagnostics. Expressions that do not compile — `flags[0]` on a scalar — simply have no answer
> on their line, and that silence is exactly the signal array-ness is read from. Cross-checked
> against the one-process-per-probe version: 88 paths for `MdtBSJoint`, 123 for `MdtContact`,
> both ways.

★★ **AND AN ENTIRE TYPE FAMILY WAS MISSING FROM THAT MAP.** The SDK declares every concrete
geometry as an **anonymous** struct typedef —

```c
typedef struct { McdGeometry m_g; MeReal mR[3]; MeReal mRadius; } McdBox;
```

— with no tag before the brace, and the map was built by scanning for `struct NAME {`. So
`McdBox`, `McdSphere`, `McdCylinder`, `McdSphyl`, `McdConvexMesh`, `McdAggregate` and `McdNull`
all had **empty** offset maps, every site addressing one read as "out of scope, the base carries
no structure", and **412 sites across 16 objects** sat in that bucket unexamined.

What it cost: `McdBoxUpdateAABB` read the box half-extents at the i386 `+0x10/0x14/0x18`.
`_McdGeometry` is 16 bytes here and **32** there, so at LP64 those land inside the base and read
**zero** — the box's AABB came back as a POINT, the broadphase never reported the box/plane pair
until the box CENTRE was through the plane, and `scene_boxes_on_plane` diverged at first contact.
Measured, not inferred: the AABB reads `-0.505,-0.405,z-0.305 .. +0.505,+0.405,z+0.305` at i386
against `-0.005,-0.005,z-0.005 .. +0.005,+0.005,z+0.005` at LP64 — half-extents exactly zero plus
the 0.005 margin — and a call counter on `McdBoxPlaneIntersect` reads `0,1,2,3,4` across steps
43–47 at i386 against `0,0,0,0,1` at LP64. **The narrow phase was never wrong; it was never
called.** `_KW` records how each name has to be spelled in a probe, because
`((struct McdBox *)0)` does not compile and `((McdBox *)0)` does.

**A fourth source of type, and it is the installer again one indirection shallower.** `McdBox.c`
calls `McdFrameworkRegisterGeometryType(frame, 2, "McdBox", ...)` and hands it that file's own
destroy/updateAABB/getBSphere, so the object under every geometry pointer in that file **is** an
`McdBox`. It is needed because the DECLARED type is the base class and the base class is where
the growth happens: `McdGeometryInstanceGetGeometry` returns `McdGeometry *`, so the callee-return
rule reads `_McdGeometry`, finds that none of `+0x10/0x14/0x18` land on it, and declines —
correctly, on the type it was given. ⚠ **One registration per file or nothing:** the `Ix*`
interaction objects register none and handle **two** geometries (`IxSphylPrimitives` has 227 such
sites), so a per-file answer there would be a guess and they are reported instead.

★★ **AND A STRUCT ADDRESSED AS AN ARRAY OF 4-BYTE WORDS — the site that closed the last
trajectory divergence.**

```c
undefined4 *puVar4;
puVar4 = McdFrameworkGetInteractions(frame, type1, type2);
if (puVar4[4] != 0) { /* swap p->model1 and p->model2 */ }
```

`_McdInteractions` opens with four function pointers, so `swap` is the fifth WORD at i386 and
`[4]` is exactly it. At LP64 those pointers occupy 32 bytes, `swap` moves to `[8]`, and `[4]`
reads the upper half of `intersectFn` — non-zero, so **`McdHello` swapped every pair it was
given**, and `scene_boxes_on_plane` saw the same box pair arrive at `McdBoxBoxIntersect` with an
exactly negated normal.

It is `fix_vtable_offsets`' family — a table walked in the shipped target's pointer size — but on
an *ordinary* struct, and invisible to every rule above because **an array subscript is the whole
expression**: no cast and no `+ K` to key on. What types it is the callee's own declared return
type, the one source here that is not an inference. ⚠ The index must land on a real field, which
is what keeps it off an honest `int *` walk. ⚠ And the call's `(` is often on the *next* line —
requiring it on the same one made the first version of this scan report zero with the site in
front of it.

⚠ **AND A DECLINE IS ONLY A DECLINE IF NOTHING LATER TYPES THE VARIABLE.** The rules run in order
and a later one routinely answers what an earlier one could not — `pvVar7` is declined by the
callee-return rule and then typed by the registration rule. Printed as written, the report claimed
a decline on a site repaired in the same run. Declines are keyed by the variable they are about
and emitted at the end, only if it is still untyped.

⚠⚠ **AND THE CALLEE-RETURN RULE USED TO READ THE CALLING CONVENTION AS THE RETURN TYPE**
(2026-08-31). metoolkit's headers put a long return type on its OWN LINE:

```c
MEPUBLIC
McdGeometryInstanceID
                  MEAPI McdModelGetGeometryInstance(McdModelID cm);
```

The one-line pattern matched the SECOND line and recorded the type as `MEAPI`, which resolves to
no struct — so the function read as "returns something untypeable" rather than as unmatched, and
**nothing said so**. Eight accessors were affected and one of them is the arm64 ragdoll crash:
`IxAggregateLineSegment` keeps its base as `pvVar7 = McdModelGetGeometryInstance(model)` and then
reads `+ 0x30`, which is `McdGeometryInstance::child` at i386 and **72** at LP64 — so the child
instance came back NULL and the struct copy two lines later faulted on address 0. Its sibling
`+ 4` is `mTM`, 4 here and 8 there. Confirmed against MathEngine's own amd64 build, which reads
`mov 0x48(%rdi),%rbp` and `mov 0x8(%rcx),%rdx` at those two sites. The type may now sit on the line
above, provided `MEAPI` opens the next one.

### `fix_derived_fields.py` — a derived struct's field addressed as an index past its base

```bash
python3 tools/fix_derived_fields.py /tmp/kd_lp64/allobj /tmp/kd_build
#   -> 186 of 281 rewritten, 95 declined
```

`*(float *)&(g[1].mRefCtAndID)` is `((McdSphere *)g)->mRadius`. The geometry types derive from
`McdGeometry` by prefixing it and Ghidra has only `McdGeometry *`, so it addresses a derived field
as `k * 16 + {0,4,8,12}` — exactly right at i386. At LP64 the base is 32 and its fields are at
`{0,8,16,24}`, which move *differently* from the derived struct: for `McdBox`, `[1].prev` lands at
40 where `mR[1]` is at 36, and `[1].next`/`[1].frame` run off the end.

★ **The access type has to change too**, which is what separates this from `fix_literal_offsets.py`.
The recovery stores through the *base* field's type — `pMVar4[1].prev` is a **pointer** — into what
is really a 4-byte `MeReal`. Four shapes each need the value adapted, and the candidates are offered
to the compiler rather than chosen by reasoning; the read and write cases are mirror images, because
Ghidra types the local as `McdGeometryID` and every use spells it `*(float *)&pMVar1` — a float in a
pointer's clothing.

★ **Here the byte-identity gate is strong**, unlike in `fix_baked_sizeof.py`: a wrong field is a
different *address* and a wrong type is a different *store width*, and either changes the i386
object. All 186 were compiled and compared.

⚠ **Two measurement bugs the self-checks caught, both of which would have read as "nothing here".**
`MeVector3 center;` is a `MeReal[3]` **behind a typedef**, so scanning for `TYPE NAME[N];` recorded
only its first element; `McdTriangleList`'s map then had holes, its offsets could not all land, the
concrete type came back ambiguous and **all 70 sites declined** — a decline that looks like a
decision. Array-ness is now measured, with an `esz >= 2` floor because GNU C gives `sizeof(void)` and
`sizeof(function)` as 1. And registering the bare member name at element 0's offset let `mR` win
over `mR[0]`, which is not an assignable lvalue.

**What is left (95):** 44 where the concrete type is genuinely ambiguous — an `Ix*` function handles
**two** geometries, so per-file inference cannot work and those need per-site typing — and 51 inside
resolved objects declined on byte-identity (`McdAggregate` 28, `McdTriangleList` 12, `McdConvexMesh`
6, `McdCylinder` 3, `McdBox` 2).

### `fix_index_layout.py` — the residue `fix_derived_fields.py` cannot type

```bash
python3 tools/fix_index_layout.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> 65 re-spelled, 2 already correct at LP64, 19 declined
```

⚠⚠ **THE ACCEPTANCE GATE COULD NOT EXPRESS THE ANSWER** (2026-08-31). `flo.accept_edits`
bisects a position-ordered list, so it can only accept a subset that is contiguous in position.
gcc common-subexpression-eliminates the two loads of `g[1].mRefCtAndID` in
`McdAggregateDestroy` — *first only* and *second only* both change the object, **both together
reproduce it exactly** — and four `g[1].prev` edits sit between them, so every subset the
bisect tried contained one without the other. It declined all six and the table pointer went on
being read four bytes wide. `accept_by_field` makes the atom a **field** rather than a
position; sites naming the same field are exactly the ones gcc folds together.

★ **AN INDEX CAN LAND ON THE RIGHT ADDRESS AND STILL BE A DEFECT** (2026-08-31). The `same`
short-circuit used to compare only the **offset**, and `mRefCtAndID` is the first member of
`McdGeometry`, so `geom[1].mRefCtAndID` is the byte just past the base — 16 here, 32 there — and
`McdAggregate::elementTable` is at 16 and 32 too. The offset agrees at both widths; the **width**
does not (4/4 against 4/**8**), so the store kept the low half of a pointer. It now requires the
width to agree as well. `CxSmallSort` is unaffected: `mPrev` and `mRep` are both pointers, so
both scale and those sites still skip.

⚠ **AND THE FIRST VERSION OF THAT RULE WAS WORSE THAN THE DEFECT, WITH EVERY GATE GREEN.** A
field can resolve to a **struct** that begins at the same address — `McdConvexMesh::mHull` — and
a struct has no access width, so the access type fell through to `MeU32` and wrote four bytes of
an eight-byte pointer *while widening the matching read* to `kd_uptr`. i386 was 145/145, the
scenes were bit-identical, the pipeline passed. Only **running the game** caught it: the crash
moved to `MeBoundingSphereCalc2` with `points = 0x59620c50`. `descend_to_scalar` is the honest
version — it narrows a struct-typed field to the member that *begins* there (`mHull` →
`mHull.vertex`), and the caller verifies the offset did not move at either width before using
it. What it cannot narrow is skipped, not guessed.

Same defect class as `fix_derived_fields.py`, which runs **first** and does the better repair
wherever a file registers one geometry. This one takes what that pass reports as ambiguous and
types the pointer **per variable** instead of per file. Three things it adds:

* **the frame.** Knowing the object is a `C` is not knowing the pointer points at its *start*.
  `CxSmallSort` is full of pointers that do not — `pLVar2 = &pCVar1->mAABBMarkers[2].super_Link`,
  `pLVar12 = (inMarker->super_Link).mNext` — and every offset used against them lands on a real
  field of `CxSmallSortRep`, so an all-offsets-land test says "it is a Rep" and it is not. A first
  version of this tool rewrote them; the rewrite is byte-identical at i386, so the acceptance test
  could not see it either. The frame must now come from a source that means *a whole object*: a
  parameter of the base type, a call with a declared return type, an allocation whose `sizeof`
  names the type, an alias, or a field read whose field is **not** a member of the base type
  itself — that last clause is what tells `ins->mGeometry` from `x->mNext`.
* **the registration**, via `interaction_types.py` — the only thing that can type an `Ix*` object.
* **a measurement before the rewrite.** Half the remaining sites are already RIGHT at LP64:
  `Link` is 8 bytes here and 16 there and `CxSmallSortMarker`'s fields are 0,4,8,12,16 here and
  0,8,16,24,32 there — everything doubles, so `pLVar2[1].mPrev` lands on `mRep` at both widths.
  The geometry family does not scale that way because `MeReal center[3]` does not grow. So the
  pass computes what the index gives at LP64 and where the field really is, and touches only the
  sites where those disagree.

⚠ **`CxSmallSort`'s reps are NOT repaired and some of them are wrong.** `mCullingIndex` is at 28
here and 52 there while the index says 56 — the scaling breaks at the second of two adjacent
4-byte fields. Those need per-**assignment** typing, not per-variable: Ghidra reuses one `Link *`
for a marker and then for that marker's rep in the same function. Reported, not skipped silently.

```bash
python3 tools/fix_arena_carve.py /tmp/kd_lp64/allobj /tmp/kd_build
#   -> 6 edits, 0 declined
```

`MdtPartOutCreateFromChunk` asks for one block and cuts it into seven arrays. Three things are
frozen at i386: `0x40` is `sizeof(MdtPartitionOutput)` (64 → 96), `0x20` is the per-body total
(32 → 36, because `bodies` is a *pointer* array and the other four are `int`), and the cursor is
pointer-sized so `+ maxBodies` steps 4 here and 8 there — right for the one pointer array it walks
and wrong for the four `int` ones. Every downstream `po->bodies[i]` is correct code indexing arrays
that overlap.

★ **This class was recorded as "almost certainly not" byte-identical. It is** — the third wrong
prediction of that kind in one session, all three from reasoning about gcc instead of asking it.
The obvious repair does fail: `info` and `constraints` **share** the subexpression
`(pppMVar2 + maxBodies)`, so rewriting both changes what gcc CSEs — five of six edits compose and
the sixth breaks it, while each is byte-identical alone. **Anchor-and-correct** fixes it: keep the
original expression verbatim and add `maxBodies * (int)(sizeof(int) - sizeof(T **))`, which is zero
here. Two for two now on cases that looked unfixable under the gate.

**The rule, derivable and not yet generalised:** `X->field = (T *)cursor;` *names* the element type
of the array at the cursor, so the next advance must be `count * sizeof(T)`. Every element type is
written down in the cast that consumes it.

⚠ **One of six.** The other carve-ups (`MdtKeaConstraintsCreateFromChunk`, four in `MdtWorld.c`) are
a different shape — a dozen sizes accumulated across thirty lines with alignment rounding and
branches — and need a static analysis, not a pattern match. Edits are keyed to exact source text and
applied **all or nothing**, because a half-carved arena overlaps *differently* rather than not at
all and would still pass byte-identity piece by piece.

---

### `fix_vtable_offsets.py` — an Itanium vtable addressed in 4-byte words

```bash
python3 tools/fix_vtable_offsets.py /tmp/kd_lp64/allobj /tmp/kd_build
#   -> 56 rewritten, 0 declined
```

The address point is `2 * sizeof(void *)` and slot n is `n * sizeof(void *)`; Ghidra bakes both
at the i386 build's four. At LP64 `+ 8` leaves the vptr at `&vtable[1]` and `+ 16` from there is
`&vtable[3]`, so the call meant for `platformInit` — which takes no arguments — enters
`calcIworldandNonInertialForceandVhmf`, which takes seven, and it reads `blist` out of a
register nobody set. ★ **The arithmetic predicts the observed callee exactly**, which is what
makes it a measurement rather than a suspicion.

⚠ **No gate here can see it.** No truncation, no struct changes size, and `code_call_check.py`
reads 0 because the dispatch goes through a concrete `void (**)(...)` prototype rather than
through `code` — the exact case that tool's own docstring records it cannot catch.

⚠ **Two spellings, and the second was found a day after the first**, by which time both scenes
had been chased through three downstream functions. `kd_types.h`'s own note warns about
`(**(...)(*(int *)A + 0x10))(A, x, b)`: the object arrives as a POINTER, so there is no vptr
store to key on, and the vptr is read as an `int` — truncating — before the slot arithmetic.
`0x10` is slot 4, `solve`; the code was reaching slot 2, `makeFromColMajorPSM`, a nine-argument
function called with three. What says `A` is polymorphic is its own struct: the first member is
`code **_vptr_`. `*(int *)contact2 + 0x178` has the identical shape, is a field read, and is
correctly left alone.

⚠ **Its refusal used to run after the edits.** "Not one address point in the corpus" is what
this prints instead of a zero — and it printed it on the *second* run over the same tree,
having already rewritten every literal on the first. The count is taken before anything is
written now, and it counts the repaired spelling too: **a search has to recognise its own
output.**

★★ **AND THE STORE AND THE SLOTS ARE ONE REPAIR — HALF-APPLIED IS WORSE THAN NOT APPLIED.**
`VPTR_STORE` accepted `char|undefined4|void` while `VSLOT`, its own partner, read `char **`
only. On `keaLCPSolver.c`, where Ghidra spells the store `(*(undefined4 **)&vanillaQMatrix)`,
the ADDRESS POINT was scaled and the eight SLOT reads were not — so at LP64 the vptr sat
correctly at `&vtable[2]` with the slots still four bytes apart, `+ 8` reached slot **one**, and
`makeFromColMajorPSM` (nine arguments, called with nine) dispatched into `makeFromJMJT`
(seven). It left `NAZ[0]` NULL and the SEGV landed two functions later at
`keaMatrix_PcSparse_vanilla.c:303`. The measurement that pins it: the i386 slots are
`+0`=allocate(1 arg), `+8`=makeFromColMajorPSM(9), `+12`=factorize(0), `+16`=solve(2), and the
four call sites pass exactly 1, 9, 0 and 2 arguments. The alternations match now, and a slot
read the pass cannot parse is a **refusal**, not a skip.

⚠ **The object is usually a MEMBER, not a bare local.** `VSLOT_PTR` matched a bare identifier,
which is why it read **one** site where the corpus has twenty-two — every real one is
`this->A`, `this->suspect`, `this->correct`. The solver reaches its matrix through a field;
that is the ordinary case, not the exception. The two MdtLOD sites that share the shape stay
excluded twice over: `contact2` is not polymorphic, and they are not in callee position.

### `fix_narrow_pointers.py` — a pointer held in something four bytes wide

```bash
python3 tools/fix_narrow_pointers.py /tmp/kd_lp64/allobj /tmp/kd_build
#   -> 13 locals + 50 accesses + 29 values + 47 pointer-array walks, 0 declined
#      + 8 negative offsets (I) + 27 negative offset CHAINS (I2) + 1 pointer
#        compared against a narrowed address (J)
```

**This is the class `fix_ptrwidth.py` has been reporting as "N diagnostic(s) remain".** That one
widens the punned CASTS at the columns clang names; here there is no cast to rewrite, or the
cast is already as wide as it can be and what is narrow is the VARIABLE. Six shapes, each with
its own evidence and none of them a pattern:

| | shape | what says it is a pointer |
|---|---|---|
| A | `int iVar3; iVar3 = keaPoolAlloc(...)` | `-Wint-conversion` |
| B | `*(undefined4 *)&this->mLP = ...` | the member is a `MeReal *` |
| C | the value stored into a widened pointer | it is stored into one |
| D | `(T *)iVar10` with `iVar10` an `int` | `-Wint-to-pointer-cast` |
| E | `iVar2 = *(kd_iptr *)&this->NAZ;` | the RHS is a pointer-width read |
| F/G | `NCZ[z]` walked — and allocated — at 4 | `NCZ` is a `MeReal **` |

The repair is a type substitution and nothing else, which is what keeps it byte-identical:
`kd_iptr` **is** `int` at 32-bit pointer width, not merely the same size.

⚠ **B is decided by measuring the target, not by its name.** `*(int *)&this->m_blocks` has the
identical shape and is correct — `m_blocks` is an `int` on every target. 153 sites have the
shape; 50 have a target that grows.

⚠ **F without G is worse than neither.** Repairing the stride while the block is still allocated
at four bytes an element makes the walk correct and runs it off the end; what comes back is a
NULL where a matrix block should be, and the SEGV is at `*AcholMatrix = ...` three hundred lines
from the allocation.

★ **The rules feed each other, so the pass runs in rounds.** Widening an access (B) is what
reveals that the value stored through it is a narrow local (C), and widening that reveals the
next. A single pass leaves `*(kd_uptr *)&this->matrixChol = uVar1;` with `uVar1` four bytes wide
— the same defect one step back, reading as a clean run.

⚠ **Order.** After `fix_ptrwidth.py`, which writes the `kd_iptr` this keys on, and **before**
`fix_align_masks.py`, because a mask on a *widened local* has no cast in it to recognise:
`iVar3 + 0xf & 0xfffffff0` with `iVar3` declared `kd_iptr`. That ordering is worth five more
masks.

> **The site that proves the class**, and how it was found.
> `pool_ptr = *(undefined4 *)((char *)poolstack + (-4) + poolstack_ptr * 4);` —
> `void *poolstack[3]`, with the element size, the element *offset* and the access width all
> frozen at four. Every allocation after the first `keaPopPoolFrame` comes back with its top
> half gone, on the **second** simulation step, which is why the first one looked clean. A
> conditional hardware watchpoint named the line in one command:
> `watch -l pool_ptr if ((unsigned long)pool_ptr) < 0x100000000`. Two rounds of reading the
> allocator had not.

**I2 — the same negative offset without the multiply.** `NEG_MUL` matched `* -N`, which was
only the spelling that happened to be looked at first. `auStack_3c + -uVar6`,
`auStack_3c[iVar2 - uVar6]` and `coef + (-1 - order)` are the identical defect: an additive
chain evaluated in 32-bit unsigned arithmetic whose value is meant to be negative. 27 sites.

★ **The cast goes on the FIRST IDENTIFIER, not around the result.**
`(kd_uptr)((uVar3 - 2) - order)` is the defect with a cast on it — the subtraction still
happens in 32 bits and the zero-extension still happens afterwards. `((kd_uptr)uVar3 - 2) - order`
is the repair; a leading literal-only prefix (`-1 - order`) converts exactly at either width.

⚠ **Byte-identity cannot gate this class** — `kd_uptr` IS `unsigned int` at i386, so a site
chosen wrongly compiles to the same object. What makes it safe is that the repair reproduces
the *i386 answer* at any width in **both** directions: where the chain is positive the widened
arithmetic gives the same small value, and where it wraps the 64-bit wrap lands exactly where
the 32-bit one did. So the discriminators are about not editing arithmetic that is not an
offset at all — the base must resolve to a **pointer** (without that test the same shape
matches `fVar9 * -fVar8` in IxBoxCylinder and `-lVar2 * lVar4` in MeMath), and one operand must
be 32-bit **unsigned** (a signed chain sign-extends and is already right, which is also what
keeps ordinary `arr[i - 1]` out).

⚠ **It addresses a DIFFERENT ARRAY from the line above it, and that is the trap.** In
`solveLCP`, `auStack_3c + -uVar6` is ELEMENT arithmetic on a `uint *` and lands at `-4*uVar6`,
while `(kd_iptr)auStack_3c - uVar6` five lines earlier is BYTE arithmetic and lands at
`-1*uVar6`. Measured both ways and confirmed against the shipped object: the i386 `solveLCP`
does **six successive `sub %edi,%esp`**, one per block, so the frame is a partition of six and
reading the two spellings as the same block would have aliased two live arrays. The self-check
compiles that six-block frame at both widths and reads the byte deltas back.

**J — a pointer compared against a 32-bit-narrowed address.**

```c
if (pool_max < (uint)(size + (kd_iptr)pool_ptr)) { ... }
```

Both are `void *`; the sum is an ADDRESS and the `(uint)` cuts its top half off, so the kea
pool reads as exhausted the moment it is handed memory above 4 GB. `scene_boxes_on_plane` dies
at step 60 with *"Memory pool size exceeded when allocating NCZ"* against a pool that has room.

⚠ **Nothing in the compiler can see this** — measured under `-Wall`, `-Wextra`,
`-Wint-conversion` and `-Weverything`, clang says nothing. `fix_ptrwidth`, whose site list IS
clang's diagnostics, structurally cannot reach it.

⚠ **It is an ADDRESS, not a DIFFERENCE**, and that is the whole discriminator.
`(int)((kd_iptr)pLVar4 - uVar3)` in CxSmallSort and the nine
`(int)((kd_iptr)p + (… - (kd_iptr)buffer))` in MeProfile/MeXMLOutput are COUNTS, correct at
every width. 31 sites share the shape; one is compared against something that resolves to a
pointer. ⚠ And its first run read **zero**, because a file-scope `extern void *pool_max;`
resolves in no function region, so every candidate was dropped as "not a pointer" — a search
that could not find anything, reporting nothing to find. The self-check asks the resolver for
that exact shape now.

### `fix_pool_reserve.py` — a pool reservation frozen at the i386 element size

```bash
python3 tools/fix_pool_reserve.py /tmp/kd_lp64/allobj /tmp/kd_build
#   -> 8 reservations scaled, 0 declined
```

`fix_narrow_pointers` rule G repairs the ALLOCATION — `keaPoolAlloc(n << 2, "NAZ")` becomes
`n * sizeof(void *)`. Nothing repaired the other half. The kea pool is one arena, sized up
front by `MdtKeaMemoryRequired`, and that function computes the same sizes a **second time**,
so at LP64 every allocation asks for twice what the arena reserved and the pool runs dry
mid-step: a 5,664-byte pool, 32 bytes left, a 128-byte `NCZ` request at step 60.

★ **The rounding ADDEND is elements too, and scaling only the stride fixes nothing.**
`(uVar27 & 0xfffffff0) * 4 + 0x40` is Ghidra's rendering of `((n + 15) & ~15) * 4`: the `0x40`
is `16 * 4`, the sixteen elements the round-up adds. With these matrices `n` is under 16, so
`uVar27 & 0xfffffff0` is **zero** and the addend IS the whole reservation. The first patch
scaled the `* 4` alone, changed the pool by not one byte, and read exactly like a wrong
diagnosis. `McdContactSimplify`'s alloca taught the same lesson from the other end.

**Which terms, and it is not a guess in either half.** The element size per pool NAME is read
off the allocation itself; which reservation is which is anchored on the names Ghidra recovered
from DWARF (`ANAZ`, `ANCZ`, `ANR`, `ANC`, `QNC` — each ends with a pool name); and the two the
decompiler left unnamed are typed by their derivation, with the named five **confirming** that
split rather than being assumed to fit it. A group with no anchor is refused.

⚠ **The discriminator reached for first over-matched.** "The count is a self-multiplication, so
it is the blocks×blocks pointer array" also fits `iVar2 = iVar12 * 4` in the same function —
the A matrix, `c4size * c4size` MeReals, a squared count holding **floats**. What separates
them is that a rounded reservation is a **pair of branches** assigning the same local; a lone
`X = n * 4` is a plain array size.

⚠ **And the repair is offered to the compiler, not derived.** `uVar27 << 2` →
`uVar27 * (int)sizeof(void *)` is the obvious rewrite and is **not** byte-identical;
`(uVar27 << 2) + uVar27 * ((int)sizeof(void *) - 4)` is. For the addend sites the direct
rewrite *is* identical. Which one gcc schedules the same way is a property of the surrounding
function, so each candidate is compiled and the first that reproduces the baseline is kept.

### `fix_narrow_loads.py` — widen the LOAD, not just the address arithmetic

```bash
python3 tools/fix_narrow_loads.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> 44 narrow pointer LOAD(s) widened in 9 object(s)
#   -> 7 named-field LOAD(s) widened by measurement (rule C)
```

★ **RULE C — THE EARLIER REPAIR HID THE REMAINING ONE** (2026-08-31). Rules A and B fire on
clang's `-Wint-to-pointer-cast`, so they need the loaded value cast to a **pointer**. Once
`fix_narrow_pointers` has widened the destination local to `kd_iptr`, the assignment is
integer-to-integer and **clang says nothing at all** — the address is right, the local is wide,
and the load still takes four bytes of an eight-byte pointer. So rule C is **measured, not
diagnosed**: the address must be an offsetof naming a real field, the destination must be
declared pointer-width, and `sizeof(T::F)` must be **4 at i386 and 8 at LP64**. Two sites
qualify; the other narrow loads into widened locals do not name a field and are left alone,
because widening a load of a genuine `int` field would read four bytes past it.

★ **AND THE DESTINATION IS PROMOTED WHEN IT IS STILL AN `int`.** `MdtBcl3`'s solver holds an
`MdtBody *` in `iVar15`, a local Ghidra also uses as a row index three statements earlier, so
`fix_narrow_pointers` will not widen it and the "already pointer-width" test skipped it.
`kd_iptr` **is** `int` at 32-bit pointer width, so promoting the declaration is a no-op on every
shipping target by construction. ⚠ Signedness survives it — `uint → kd_iptr` is a *sign change*,
not a widening, and it cost `MdtBcl` its byte-identity on three otherwise-innocent
declarations.

`fix_ptrwidth.py` widens the casts clang says narrow a pointer and its header predicted that what
remained would be *"a genuinely integer-valued address (Ghidra's `(code *)0x10074` and the like)"*.
⚠ **That prediction is wrong, and it was measured wrong.** Of the 151 `-Wint-to-pointer-cast`
diagnostics left at aarch64, **not one is a literal.** They are the other half of the same round
trip:

```c
(float **)(*(int *)((kd_iptr)pvVar22 + 0x34))     /* a 4-byte LOAD of a pointer */
```

The ADDRESS was widened; the LOAD WIDTH was not, so at LP64 it returns the low half of a stored
pointer. `kd_iptr` *is* `int` at 32-bit pointer width, so every rewrite is a no-op on i386, wasm32
and armv7 by construction — verified anyway.

It runs **last** in `lp64_pipeline.sh`, deliberately: it reads diagnostics over the finished text,
and `*(kd_iptr *)p` is exactly the spelling `fix_narrow_pointers` and `fix_align_masks` key on.
A narrow struct FIELD is declined rather than patched — that is a layout question, not a load-width
one, and widening the access would read past the field.

★ **RULE E — THE SAME LOAD WITH NO DESTINATION AT ALL** (2026-08-31). Rule C requires the value to
land in a local declared pointer-width, and that was only ever a PROXY: the justification is the
measurement — the address names a real field whose size is 4 at i386 and 8 at LP64 — and a field
that grew is a truncated load wherever it appears. An INLINE one has no destination to inspect:

```c
*(kd_iptr *)(*(int *)((kd_iptr)pvVar6 + KD_OFFSET(McdAggregate, elementTable))
             + KD_OFFSET(McdAggregateElement, mGeometry) + local_10c)
```

**This is the arm64 ragdoll crash after the `qsort` one, and the disassembly says it exactly.**
`IxAggregateLineSegment+304` on a OnePlus 6:

```
a0ab30: ldr w8, [x8, #0x20]     <- elementTable, at its CORRECT LP64 offset 32
a0ab34: add w9, w8, #0x40       <-   ...+ offsetof(mGeometry)
a0ab3c: ldr x8, [x8, w9, sxtw]  <- FAULT, fault addr 0xffffffffd3b89558
```

The offset is right — `fix_literal_offsets` fixed that — and the WIDTH is four bytes of an
eight-byte pointer, sign-extended by `sxtw` into the address the tombstone reports. Reached from
`KInitSkeletonKarma` → `USkeletalMesh::LineCheck`, so it is on the ragdoll-creation path and
nothing offline runs it. Ten sites; `IxSphylPrimitives::McdSphyl::mRadius` is 4/4 and declines.

⚠⚠ **AN ASSIGNMENT TARGET IS NOT A LOAD, AND WIDENING ONE ALONE CORRUPTS.** The first version of
rule E matched both sides of `=` and turned
`*(undefined4 *)(c + KD_OFFSET(MdtContact, head.mdtbody[1])) = uVar1;` into an EIGHT-byte store of
a four-byte `uVar1`, zeroing the high half of a pointer that had been valid. **i386 stayed 145/145
and the three scenes stayed bit-identical**; the LP64 harness went red with an AddressSanitizer
error in `MdtBodyGetCenterOfMassPosition` and two trajectories that stopped at row 52 and row 0.
★ The LP64 harness is the only gate that sees this — the third time this project has been told so.
A store is now widened only when its SOURCE is a load this rule is widening too (that pair is a
whole-pointer copy and is the complete repair); a store whose source is a narrow local is a
different defect and is declined and reported.

### `fix_list_walk.py` — a linked list walked through a four-byte cursor

```bash
python3 tools/fix_list_walk.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> fix_list_walk: 1 repaired, 2 declined
```

Ghidra loses the node type of a list and walks it through `undefined4 *`, so both the link and
the payload are read in four-byte units:

```c
for (puVar1 = *(undefined4 **)((kd_iptr)pvVar2 + KD_OFFSET(PElement, childHead));
     puVar1 != (undefined4 *)0x0;
     puVar1 = (undefined4 *)puVar1[1])                       /* `next` is at byte 4 */
  ... (PElement *)*puVar1 ...                                /* `current`, TRUNCATED */
```

At i386 that is right twice over. At LP64 `next` moves to byte 8, so `puVar1[1]` reads the
**top half of `current`** and calls it the next node, and `*puVar1` keeps only the low 32 bits
of a pointer.

★ **THIS IS FRAME (2) FROM RUNNING THE GAME, AND THE TRUNCATION IS IN THE CRASH ITSELF.** gdb
caught `KaFileCreate_1_0` with `e = 0x58cabf90` beside `parent = 0x555558ce4030`. Every
pointer in that process is `0x5555_5xxxxxxx`, so a 32-bit `e` is not a wild value — it is a
measurement of where the top half went.

**THE NODE TYPE IS DECLARED, NOT INFERRED.** The cursor is initialised from a *named* field and
the oracle header says what that field is — `PElementNode *childHead;` — so the type comes from
`metoolkit/include`, which is the yardstick and is never edited. A cursor initialised from a
**baked** offset instead is declined and reported: `McdAggregate`'s two are, and they need
their base typed first.

⚠ **NO `#if` HERE, UNLIKE `fix_block_copy`, AND THE DIFFERENCE IS THE LESSON.**
`((PElementNode *)p)->next` **is** `p[1]` at i386 — same address, same four-byte access — so
one spelling serves both widths. Measured, not assumed: the repair reproduces the baseline
object exactly.

### `fix_block_copy.py` — a whole-struct copy rendered as a word loop

```bash
python3 tools/fix_block_copy.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> fix_block_copy: 15 repaired, 1 already correct, 0 declined
```

`rep movsd` is how a 32-bit compiler copies a struct, and Ghidra renders it as a loop that
walks one field at a time and counts in **words**, with the cursor stepped by
`p = (T *)&p->secondField` — four bytes at i386, **eight** at LP64, while the literal trip
count stays put. `dummyModel = *p->model1;` becomes a loop that copies 240 bytes into a
208-byte struct.

| type | n | step 32/64 | sizeof 32/64 | n·step64 | |
|---|---|---|---|---|---|
| `McdModel` | 30 | 4 / 8 | 120 / 208 | 240 | overrun |
| `McdGeometryInstance` | 13 | 4 / 8 | 52 / 80 | 104 | overrun |
| `MdtKeaDebugDataRequest` | 11 | 4 / 8 | 44 / 80 | 88 | overrun |
| `MdtKeaParameters` | 19 | 4 / 4 | 76 / 128 | 76 | **short copy** |
| `MeXMLHandler` | 8 | 4 / 8 | 32 / 48 | 64 | overrun |
| `MdtBclContactParams` | 18 | 4 / 4 | 72 / 72 | 72 | **already correct** |

⚠ **THE LAST ROW IS WHY THIS PASS MEASURES INSTEAD OF MATCHING.** `MdtBclContactParams` holds
no pointers, so its size and step are the same at both widths and the loop is already right.
It matches the pattern perfectly and must be left alone.

★ **FRAME (1) FROM RUNNING THE GAME.** SIGSEGV in `__strcmp_avx2` under `MeXMLElementProcess`
from `Handle_GeometryPrimitive_1_0` — a `strcmp` against a NULL `name`, because the handler
table being searched was built by one of these loops and never held its `MeXMLActionEnd`
sentinel.

**TWO SHAPES.** Both ends real memory → one struct assignment. Source `&PTR_s_RADIUS_...` →
**not a copy at all**: that is `kd_relsec_rodata[]`, *one slot per original 4-byte word*, so at
LP64 the source has **eight slots per handler** and the destination **six words**. Only a
field-by-field read is correct — word *j* of element *i* is `slot[i*W + j]` — and it requires
every field of the struct to be exactly 4 bytes at i386, which the pass verifies from the type
database rather than assuming.

★ **WHY THE REPAIR IS GUARDED.** The single-spelling repair every other pass uses was tried
first and **measured**: the per-field expansion compiles to a different i386 object
(22800 → 22824 bytes), and no one spelling can serve both widths here. So the i386 text is kept
**verbatim** under `#if __SIZEOF_POINTER__ == 4`. That is a proof rather than a concession —
the preprocessor deletes the new branch on every 4-byte-pointer target, so the shipped 32-bit
object is byte-identical by construction and **so is wasm32**, where `__SIZEOF_POINTER__` is
also 4. Verified at **146/146 byte-identical**, not argued.

### `fix_global_array_index.py` — a GLOBAL array of pointers indexed by a baked byte offset

```bash
python3 tools/fix_global_array_index.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> fix_global_array_index: 3 occurrence(s) repaired, 0 declined, 6 out of scope
```

Ghidra renders `TABLE[k]` on a file-scope array as byte arithmetic against the symbol, frozen at
the i386 pointer size:

```c
kd_mask = *(int *)((*(kd_iptr *)((char *)&McdGjkBinarySubset + 0x3c)) + 4 + i * 4);
```

`extern const int *McdGjkBinarySubset[16]` is an array of **pointers**. `0x3c` is 60 is element 15
— the entry for the full four-point simplex, the only one this code wants. At LP64 the elements
are eight bytes, so byte 60 is not an element at all: an eight-byte load there returns
`(element[8] & 0xffffffff) << 32 | (element[7] >> 32)`, the low half of one pointer spliced onto
the high half of another. Both halves are real, so nothing is truncated and no diagnostic fires.

★ **THIS IS THE ARM64 ONSLAUGHT CRASH AND THE FAULT ADDRESS SPELLS IT OUT.** The owner played an
ONS match on a OnePlus 6; the vehicles landed and then

```
Fatal signal 11 (SIGSEGV), SEGV_MAPERR, fault addr 0x40546a00000075
#00 McdGjkFaceQueueInit  #01 McdGjkPenetrationDepth  #02 McdGjkTest ... #05 KIntersect
```

`libUT2004.so` maps around `0x75_00000000` there, so a table entry reads `0x00000075_0040546a`
— and the faulting address is those two words the wrong way round.

★★ **CONFIRMED AGAINST MATHENGINE'S OWN 64-BIT BUILD**, not inferred.
`metoolkit/lib.rel/win_amd64_single/McdConvex.lib` reads `mov rax, QWORD PTR [rip+0x78]` at
**both** references — `0x78` is 120 is `15 * 8` against our `0x3c` = `15 * 4`, in
`McdGjkFaceQueueInit` and in `McdGjkPenetrationDepth`, which are exactly the two sites this
repairs. Two for two, read off a binary.

The repair is the element, not the byte: `(char *)&NAME + K` → `(char *)&NAME[K / 4]`, a no-op at
i386 and at wasm32 by construction and `K/4 * 8` at LP64. Both gates are run per site (gcc `-m32`
byte-identity **and** an emcc self-comparison), because gcc alone does not certify the web.

⚠ **THE ARRAY TEST IS NOT OPTIONAL, AND `MeProfile_linux.c` IS WHY.** It carries six sites of the
same shape — `(char *)&frameTime + 0x4` through `+ 0x14`, and `(char *)&clockSpeed + 0x4` — and
every one is CORRECT: `frameTime` is a struct of six `MeI32`s and `clockSpeed` is an `MeI64` whose
`+ 4` is its own high word. A pass keying on the shape rather than on the declaration would have
"repaired" all six. They are reported as out of scope.

⚠ **THE SECOND SITE IS A VTABLE AND `fix_vtable_offsets` CANNOT SEE IT.** `CxSmallSort.c` writes
its address point as `#define PTR__CxSmallSort_00011f20 (*(void **)((char *)&kd_ZTV11CxSmallSort
+ 0x8))`. An Itanium vtable's address point is `2 * sizeof(void *)` — 8 here, 16 there — and that
pass knows the arithmetic; its *discriminator* is a local into which the corpus stores an address
point, and this one is a macro over the vtable symbol. At LP64 the vptr ended up at `&vtable[1]`,
the typeinfo pointer, and `CxSmallSort.c:287` calls through it. Latent on x86-64 only because that
destructor path is not taken.

### `ptrwidth_classify.py` — which of the aarch64 truncation warnings are DEFECTS

```bash
python3 tools/ptrwidth_classify.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> aarch64 truncation diagnostics: 94 across 28 object(s)
#        fbits-to-4byte  10   medict-key 15   int-return 11   count-to-id 3
#        UNEXPLAINED     55   <- the ones that are open
```

`ptrwidth_check.sh` **counts**. It cannot say which of its warnings is a defect, and the count has
been quoted as if every one were — "98 truncations across 28 objects" reads as ninety-eight bugs.
It is not, and without this nobody can tell whether a drop from 98 to 94 removed four defects or
four false alarms. This sorts every site into a NAMED class with a rule behind it and puts
everything else in `UNEXPLAINED`, which is the number that matters.

| class | why it is benign |
|---|---|
| `fbits-to-4byte` | `*(MeU32 *)(...) = (McdGeometryID)KD_FBITS(x)` — a float's four bytes into a four-byte slot. The pointer typedef is a Ghidra artefact; the DESTINATION is `MeU32` at every width |
| `medict-key` | `MeDict`'s own API takes its key as `void *` (`MeDict.h`) and the engine stores an `MeI32`. At LP64 that WIDENS, and insert and compare use the identical conversion |
| `int-return` | Ghidra kept a call's return register in a pointer-typed local. The callee returns an int and nothing dereferences it |
| `count-to-id` | a count through an ID typedef into an `int` field — the same artefact with a different source |

⚠ **`KD_FBITS` ALONE IS NOT A CLASS**, and assuming it was is what this exists to stop.
`(MStack_24c.next)->prev = (McdGeometryID)KD_FBITS(...)` in `IxBoxTriList` uses the same macro and
is a REAL defect: `MStack_24c` is a fabricated `McdGeometry` whose `next` holds a `MeVector3 *`, so
`->prev` is `normal[1]` at i386 and byte **16** — off the end of a three-float vector — at LP64,
written eight bytes wide. The macro says only that a float's bits are going somewhere; where they
go is the whole question.

### `fix_element_stride.py` — a table's ELEMENT SIZE frozen at the i386 value

```bash
python3 tools/fix_element_stride.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> fix_element_stride: 56 site(s) rewritten, 0 declined
```

A struct holds a pointer to an array of another struct and every walk carries the element size
as a literal:

```
McdAggregateElement { MeMatrix4 mRelTM; McdGeometryID mGeometry; }
    i386  sizeof 68 (0x44)   mGeometry at 64 (0x40)
    LP64  sizeof 72          mGeometry at 64
```

★ **THE OFFSET IS RIGHT AT BOTH WIDTHS AND THE STRIDE IS NOT**, which is why this survives
every pass that checks offsets — sixteen floats do not grow, so only the SIZE moves. This is
frame (6), and the crash named it in one value: `McdGeometryIncrementReferenceCount` with
`g = 0x3f800000596203b0`, where `0x3f800000` is **1.0f** — a matrix diagonal in the top half of
a pointer.

⚠ **IT IS NOT ONLY `i * 0x44`.** `McdAggregateCreate` zeroes the table with gcc's four-at-a-time
unrolling and every offset in it is baked — `0x40, 0x84, 200, 0x10c`, step `0x110`, which is
`64 + k·68` and `4·68`. At LP64 the slots are at 64, 136, 208, 280, so the loop zeroed the
middle of the matrices and left **every** `mGeometry` holding malloc garbage. ★ And
`undefined4` clears four bytes of an eight-byte pointer, so fixing the stride without the width
still ships a dangling pointer — the first attempt moved the crash instead of removing it.

**THE DECOMPOSITION IS THE MEASUREMENT.** A literal `L` on the table's additive chain is an
offset into it only if `L % sizeof_i386(E)` lands on a member **start** of E. `q` elements plus
that member is the same address at every width; a remainder that is not a member start is
ordinary arithmetic and is left alone, which keeps loop counters and `+ 4`s out of scope. The
element type is **declared** — `McdAggregateElement *elementTable;` is in the oracle — and the
size must actually change at LP64 or there is nothing to repair. 117 fields in the corpus fit
the shape; exactly one has its i386 element size present as a literal.

⚠ **Signedness is part of the repair.** `*(int *)(...) != 0` widened through `kd_uptr` is a
different i386 object, and three lines declined the whole file on that alone. `int` widens to
`kd_iptr`. And only the **outermost** access is widened — these lines carry a second, nested
`*(int *)` that loads the table pointer itself, which is `fix_narrow_loads`' job.

★ **A SECOND SOURCE OF EVIDENCE FOR THE GATED ANCHOR, AND A MODE THAT GOES WITH IT.**
`McdSphylTriangleListIntersect` walks its triangles through **byte cursors**, so
`fix_word_indexed_struct` has nothing to type in it and the `((E *)v)->` evidence never appears
— while its stride `local_5c + 0x18` is `sizeof(McdUserTriangle)` at i386 and **48** at LP64.
The file does carry `alloca(n * (int)sizeof(*(McdUserTriangle *)0))`, which
`fix_baked_sizeof --field-allocas-only` wrote only because the field the block is stored into is
declared `McdUserTriangle *` in the oracle. That is another pass's measured conclusion, so it
counts as evidence — `element_evidence()`, asked in `rewrite()` **and** in `main()`'s pre-filter,
because the last time those two spellings disagreed the pre-filter silently refused the one file
the rule existed for.

⚠⚠ **AND THAT EVIDENCE LICENCES EXACTLY ONE RULE — MEASURED, NOT ASSUMED.** `alloca(n *
sizeof(E))` says the **block** is an array of E and nothing about any other pointer in the file.
Run without that restriction, the table/cursor fixpoint flooded to fifty names — every float on
a line that mentioned the anchor — and rules B and D rewrote **twelve reads of a transform
matrix**:

```c
*(float *)((kd_iptr)pvVar7 + 0x30)   /* McdModelGetTransformPtr, translation row */
    ->  *(float *)((kd_iptr)pvVar7 + 2 * (int)sizeof(McdUserTriangle))
```

`0x30` is 48 is `2 * 24`, so it survives "whole elements only"; at LP64 it becomes 96 and reads
off the end of the transform. ★ **All sixteen sites passed the i386 byte-identity gate**, because
`2 * sizeof(E)` *is* `0x30` there. So in this mode the pass runs rule **C** only — a cursor's own
step and seed — and the cursor is whatever is added to the table expression **after** the anchor's
own match, never the fixpoint.

### `fix_member_base_walk.py` — an array addressed from the MIDDLE of its first element

```bash
python3 tools/fix_member_base_walk.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> fix_member_base_walk: 14 edit(s) in 5 file(s), 0 declined
```

gcc keeps the contact array in a register pointing at the first element's `normal`, not at the
element, and Ghidra writes every later field as an offset from **that**:

```c
iVar31 = iVar32 * 0x28;                                    /* 40 = sizeof at i386 */
*(__typeof__(tri->triangleData) *)
    ((kd_iptr)result->contacts->normal + iVar31 + 0x18) = tri->triangleData;
iVar31 = iVar31 + 0x28;
```

```
McdContact   i386  sizeof 40   normal 12  separation 24  element1 32  element2 36
             LP64  sizeof 48   normal 12  separation 24  element1 32  element2 40
```

★ **SO THE OFFSET IS RELATIVE TO A MEMBER, AND THAT CHANGES THE ARITHMETIC.** `+ 0x18` is byte
36 of the element, which is `element2` here and the **high half of `element1`** there — and the
write is eight bytes wide at LP64, so it lands across two fields and corrupts both. That is
`KPerContactCB` faulting in **six of eight gametypes**, and it is what
`McdSphylTriangleListIntersect`'s own repair made reachable.

⚠⚠ **THE OBVIOUS DECOMPOSITION IS WRONG IN A WAY i386 CANNOT SHOW YOU.** `fix_element_stride`'s
rule D reads a literal as `q*sizeof(E) + offsetof(member at L % sizeof(E))`. Here `L` is 24 and
byte 24 of `McdContact` is `separation` — a real member, at 24 at **both** widths. That
decomposition compiles to the same 24 it replaced, passes the byte-identity gate, and is still
four bytes short at LP64. The remainder has to be taken from `b + L`, where `b` is the offset of
the member the chain is **rooted** at.

★ **ONLY REWRITE WHAT MOVES — 4 of the 8 offset sites.** The same shape carries `+ 0xc` (byte 24,
`separation`) and `+ 0x14` (byte 32, `element1`) and both are already right, because neither
member moves. `McdBatch.c` writes `+ 0x14` and `+ 0x18` four lines apart. The LP64 value is
computed for every site and one that does not move is left exactly as it was.

⚠ **THE LITERAL IS OPTIONAL, AND THE PITCH IS THE POINT.** `McdGjk` and `McdPlaneIntersect` write
`normal` at `+ cur`, `+ cur + 4`, `+ cur + 8`, `+ cur + 0xc` — every one correct at LP64. Their
only defect is `cur = cur + 0x28`, walking 40 bytes over 48-byte elements. A rule that needed an
offset to move before it looked at the stride skipped both files and reported a clean nothing.
And `McdPlaneIntersect` advances its cursor inside the loop's own **condition**, so a `^…;$`
pattern finds it not at all.

⚠ **ONE FILE CAN WALK TWO ARRAYS THIS WAY.** `McdBatch.c` steps `pMVar1->start->normal` over
`McdBatchEntry` (sizeof 96 → 168) and `pMVar3->contacts->normal` over `McdContact` in the same
function, with different cursors. Sites are grouped by element type so the second array cannot
be given the first one's `sizeof`.

⚠ **A MEMBER NAME IS NOT A TYPE, AND THE ORACLE KEYS BY TAG.** Four structs here declare a
pointer called `contacts`, so each hop is resolved through the oracle — and
`typedef struct _McdIntersectResult {…} McdIntersectResult;` is stored under the **tag** while the
recovered source names the **typedef**, so asking once with the spelling in front of you returns
nothing. `fix_narrow_loads.resolve_member` asks once and answers `None` for every site in
`IxSphylPrimitives`; that is why this pass resolves the chain itself, trying both spellings at
every hop.

### `fix_callback_context.py` — a callback's `void *` context, read at i386 offsets

```bash
python3 tools/fix_callback_context.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> fix_callback_context: 18 edit(s), 0 declined
```

A callback takes its state as a `void *`, so Ghidra has no type for it and addresses every field
by a baked byte offset:

```c
static MeDictNode *MePoolxDictNodeAllocate(void *pool)
{
  if (*(int *)((kd_iptr)pool + 0xc) != 0) {                          /* numfree */
    pMVar2 = (MeDictNode *)(*(kd_iptr *)pool +
                            *(int *)((kd_iptr)pool + 0x10) * 4);     /* ifree   */
    ...
    *(MeDictNode **)((kd_iptr)pool + 0x10) = pMVar2->left;
```

```
MePoolx   i386  mem 0  isize 4  numrec 8   numfree 12  ifree 16   sizeof 20
          LP64  mem 0  isize 8  numrec 12  numfree 16  ifree 20   sizeof 24
```

⚠⚠ **NO STATIC GATE IN THIS PROJECT CAN SEE THIS CLASS.** Nothing is truncated and no cast is
narrowed, so `ptrwidth_check` and `ptrwidth_classify` report the file as clean — `MePoolx` is on
neither the 91 aarch64 diagnostics nor the 52 open ones. **Only running the game finds it.**

★ **THE TOMBSTONE IS THE DEFECT, FIELD BY FIELD.** gdb at the `SIGSEGV` in `MeDictInsert` ←
`MeSetAdd` ← `McdConvexMeshPlaneCut` — a convex mesh being sliced, i.e. a **vehicle**:

```
nodemem      = 0x7ffffffc66d0                 (MeDictNode[200], 8-aligned)
set.nodepool = { mem = 0x7ffffffc66d0, isize = 12,
                 numrec = 196, numfree = 32767, ifree = -235024 }
```

`numrec` should be 200 and is 196 — decremented four times by the code that means `numfree`.
`numfree` should be 200 and is **32767**, the high half of a stack address, because
`*(MeDictNode **)(pool + 0x10)` stores eight bytes into a four-byte index. The node handed back
is `0x…cc`, which a `MeDictNode` can never be.

**THE TYPE IS DECLARED — IN THE REGISTRATION**, the same evidence chain `interaction_types` uses:
`MeDictSetAllocator(d, MePoolxDictNodeAllocate, MePoolxDictNodeDeallocate, p)` passes the
function's address and its context in one call, so the candidates are that call's typed pointer
arguments. ⚠ There are **two** and position does not say which — `d` is an `MeDict *`. The
tie-break is a measurement: every offset used against the parameter must land on a top-level
member **start** of the candidate at i386. `MeDict`'s members are 0, 24, 28, … so `4` lands inside
`nilnode` and it is refused. Exactly one survivor, or the site is declined.

⚠⚠ **AND THE MEMBER SPELLING IS NOT BYTE-IDENTICAL, WHICH IS WHY THIS RE-SPELLS THE ADDRESS.**
Measured: `((MePoolx *)pool)->numfree` alone is identical, `->ifree` alone is identical, and the
two **together** are not — gcc schedules the member form differently once both loads are visible
as fields of one object. A typed local (`MePoolx *p = pool;`) does not rescue it either. The
address form keeps every expression's *type* exactly as it was and changes only how the address is
computed, which is `fix_literal_offsets`' own rule and the reason it survives the gate.

**RULE D — THE POOL'S OWN MEMORY IS DECLARED, SO THE LINK IN IT IS TOO.** `MePoolx::mem` is
`int *`, so a record handed out of it holds a four-byte free-list index in its first word. Ghidra
typed the record `MeDictNode *` and read `->left`, which is eight bytes at LP64. Only offset zero,
only where the member is 4 bytes here and 8 there, and a constant compared against a re-spelled
site is cast to the new type so `0xffffffff` stays `-1` rather than becoming 4294967295.

⚠ **THE GREEDY RETURN TYPE ATE THE NAME — AGAIN.** `^static\s+[A-Za-z_][\w ]*\**\s*(\w+)\s*\(`
reads `static void MePoolxDictNodeDeallocate(` as a function called **`e`**. Its sibling
`static MeDictNode * MePoolxDictNodeAllocate(` survives only because the `*` forces the split — so
one callback was typed, the other silently was not, and **no decline was printed for the one it
dropped**. Same family as `fix_frame_slots`' `aiStack_9cb0` → `iStack_9cb0`. There is a self-check
over both spellings now.

### `fix_alloca_elem.py` — an `alloca`'d ARRAY OF POINTERS, strided at four bytes

```bash
python3 tools/fix_alloca_elem.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> fix_alloca_elem: 28 edit(s), 0 declined
```

This is `LP64-ARM64-PLAYS` item (3) in the two files that entry could not reach:

```c
kd_alloca_iVar3 = (char *)alloca((size_t)(group->count) * 4 + 0);        /* MdtLOD.c */
*(MdtContactID *)((kd_iptr)(kd_alloca_iVar3) + iVar8 * 4) = pMVar2;
*(undefined4 *)(&(*kd_argslot_ffffffcc)) = 4;                            /* qsort's size */
qsort(*(void **)(&(*kd_argslot_ffffffc4)), …, *(size_t *)(&(*kd_argslot_ffffffcc)), …);
```

`MdtContactID` is `MdtContact *`, so the block is an array of **pointers**: `4` is
`sizeof(element)` here and **eight** at LP64. The allocation is half the size it needs, every
stride lands mid-element, and `qsort` is told its elements are four bytes — so the comparator is
handed the top half of one pointer welded to the bottom half of the next.

★ **`fix_baked_sizeof` DECLINED THIS BY NAME AND SAID SO**, which is why it is a separate pass:
its `QSORT` rule requires a plain identifier as the base, and *"`MdtLOD.c`'s
`qsort(*(void **)(&(*kd_argslot_ffffffc4)), …)` is not one, and is declined and reported rather
than rewritten through a cast whose pointee is `void`"*. That was right — `void` has no size. The
missing piece was never a better guess at the cast; it was the **element type**, written down
elsewhere.

**THE ORACLE IS ASKED FIRST.** `MeAssetFactory.c` casts everything to `void *`, and the block is
argument two of `MeFAssetGetPartsSortedByName(const MeFAsset *const, MeFAssetPart **)` — the
signature in the oracle, matched through the argslot the block travels in.

⚠⚠ **AN ARGSLOT IS NOT EVIDENCE.** `kd_argslot_ffffffa8` is cast to `MeHash **`, `MeFJoint **`,
`char **`, `void **` *and* `MeFAssetPart **` inside one function, because outgoing slots are
reused; taking the first cast off one picks a type at random — measured, it chose
`MeAssetFactory *`. ★ And Ghidra's own element cast is not authoritative either: it reads that
same array as `MeAssetFactory **`. Both are pointers, so the rewrite is numerically right under
either — the **name** is not, and a pass that writes down the wrong type has recorded a wrong
reason for a right answer.

⚠ **`(\w+)\s*\(([^();]*)\)` CANNOT SEE THESE CALLS AT ALL.** Every argument in the recovered
sources is itself parenthesised, so a character class that excludes `(` matches nothing, the
oracle evidence never fires, and the pass falls through to Ghidra's guess **with no decline
printed**. That cost `MeAssetFactory` its correct element type twice. The scanner balances
parentheses now.

⚠ **AND `* 4` IS NOT A CLASS.** Eight `alloca`s in the corpus carry a literal 4 and several are
honest `int` arrays — `keaMatrix_PcSparse_vanilla`'s `numClamped * 4` is one. The element type has
to be *found*, its i386 `sizeof` has to equal the literal, and its LP64 `sizeof` has to differ, or
the site is left exactly as it was. Four bases are declined and reported for want of a declared
type.

### `fix_word_indexed_struct.py` — a struct reached through a WORD-INDEXED pointer

```bash
python3 tools/fix_word_indexed_struct.py /tmp/kd_lp64/allobj /tmp/kd_build $MT
#   -> fix_word_indexed_struct: 3 site(s) rewritten, 0 declined
```

`McdCache` comes out of a pool as a `void *`, Ghidra types the local `undefined4 *`, and every
field is addressed by **word index** — `puVar3[0xd] = &p->model1->mInstance`.

```
struct _McdCache { MeVector3 normal, location, offset;
                   MeReal fat1, fat2, padding, separation;
                   McdGeometryInstanceID ins1, ins2; }
    i386   0 12 24 36 40 44 48 52 56
    LP64   0 12 24 36 40 44 48 56 64
```

Three floats do not grow, so everything up to `separation` keeps its offset. The two **pointers**
move — 52→56 and 56→64 — and they are eight bytes wide, so `puVar3[0xd] = ptr` writes half a
pointer and `puVar3[0xe]` reads it back short. ★ That is the Onslaught crash:
`McdGjkFatness (ins = 0xfffcf878)`, the low half of the stack address `0x7ffffffcf878`.

**THE TYPE COMES FROM THE POOL.** `m_cachedData` is declared `void *` in the oracle and the rest
is a *comment*, so `(MePoolFixedAPI.init)(pool, 100, (int)sizeof(*(McdCache *)0), 16)` is the
only place the type is written down at all — the same evidence chain `fix_baked_sizeof` uses for
its pool form.

⚠ **ONLY REWRITE WHAT MOVES.** Re-spelling all twelve indices was **not** byte-identical: five
are `MeReal` fields, and `puVar3[9] = fVar5` through `undefined4` is an *integer* store where
`->fat1 = fVar5` is a float one. Measuring offset and width first leaves three sites — the two
pointer stores and the read-back — which are the three the crash is about.

⚠ **AND NEITHER FIELD SOURCE CAN SEE THE POINTERS ALONE.** `flo.offsets_of` expands arrays but
its member regex does not parse a comma-separated declarator, and `McdCache` ends
`McdGeometryInstanceID ins1, ins2;`. The type database has them but without array expansion. The
union resolves it; the self-check is what proves the union works.

⚠⚠ **THE MEMBER SPELLING IS NOT ALWAYS AVAILABLE, AND `IxBoxTriList` IS WHY** (2026-08-31).
`((McdUserTriangle *)p)->vertices[1]` is byte-identical under gcc at `-m32` and **not** under
emcc, so the whole variable declined and four truncated triangle reads stayed in the shipped
source. Bisected site by site, the three that break it are exactly the three where the rewrite
turns an INTEGER load into a POINTER load: `p[1]` yields `undefined4` and is converted to
`float *`, while `->vertices[1]` yields `MeVector3 *` directly. Same address, same width, same
result — and clang tees one extra local, three bytes of wasm, on a statement four lines away.

★ **SO KEEP THE VALUE AN INTEGER OF POINTER WIDTH.** `((kd_uptr *)p)[1]` is `unsigned int` at
i386 and wasm32 — exactly what `undefined4` was — so the conversion that follows is the same
int-to-pointer conversion it always was and nothing downstream moves. Measured: all four sites
byte-identical under BOTH compilers, where every member spelling tried (`->vertices[1]`,
`((MeVector3 **)p)[1]`, `*(MeVector3 **)&...`, `(kd_uptr)...->vertices[1]`) failed under emcc.

The pass tries the member spelling first and falls back to this one, so seventeen sites that do
not need the opaque form do not get it, and the log says which was used (`[member]` /
`[word-index]`). **The fallback's guard is three measurements, not a shape**: offset `4k` at i386,
offset `8k` at LP64, and `sizeof` 4 here and 8 there — otherwise `[k]` in pointer units addresses
something else. `McdUserTriangle::flags` is an `MeU32`, fails the width test, and keeps the member
spelling, which emcc accepts.

### `fix_align_masks.py` — an alignment mask frozen at 32 bits

```bash
python3 tools/fix_align_masks.py /tmp/kd_lp64/allobj /tmp/kd_build
#   -> 36 widened, 0 declined
```

`(kd_iptr)cursor + 0x3f & 0xffffffc0` rounds an allocation up to a cache line. `0xffffffc0` is
`~63` **in 32 bits** and its type is `unsigned int`, so at LP64 it widens with ZEROS and the AND
does not round the pointer down — it **cuts the top half off it**. Nothing else here can see
that: the expression's type is already `intptr_t` and stays `intptr_t`, so `fix_ptrwidth.py` has
nothing to report, and no struct changes size.

★ **The first evidence was a register dump, and that is the reusable part:**

```
SEGV on unknown address 0x00004c1e01c0 ... WRITE
rbx = 0x000000004c1e01c0   rdi = 0x000072b84c1e0134
```

the faulting address IS the low half of a live pointer. ⚠ The report landed **two files away**
on the first statement of `MdtBclAddBSJoint` — correct code writing through a pointer
`MdtKeaConstraintsCreateFromChunk` had computed. Every baked offset in that statement was
repaired first, on the strength of the file and line in the report, and the scene failed in
exactly the same place afterwards. **Read the registers.**

The repair is **anchor-and-correct**: `0xffffffc0` → `(0xffffffc0 | ~(kd_uptr)0xffffffffU)`,
zero at 32-bit pointer width and `0xffffffff00000000` at LP64. The self-check compiles it at
both widths and reads the two values back rather than asserting the claim.

⚠ **Which masks — the discriminator is narrower than it looks.** The literal must have the shape
of `~(align - 1)` (`flags & 0xfffffffd` is a bit being cleared in an integer and is correct
everywhere) **and** the `&`'s own operand must be pointer-derived.
`(void *)((kd_iptr)p + (uVar8 & 0xfffffffc))` has a pointer in the statement, a mask of the
right shape and a pointer cast around the result, and is CORRECT — the mask is on a count. The
scan walks back from the `&` to its own enclosing parenthesis and asks about that span alone.
---

### `fix_frame_slots.py` — Ghidra's invented argument area, addressed in 4-byte words

```bash
python3 tools/fix_frame_slots.py /tmp/kd_lp64/allobj /tmp/kd_build
#   -> 22 areas scaled, 0 declined on byte-identity (7 declined as ambiguous)
```

★★ **AND ITS TRAILING-ADDEND RULE HAD SILENTLY STOPPED MATCHING.** `FRAME_OFF2` required a
literal `(int)` cast in front of the frame group:

```c
*(int **)((int)(kd_frame + 0) + 8) = clamped;      /* what the rule was written for */
*(int **)((kd_iptr)(kd_frame + 0) + 8) = clamped;  /* what it actually sees */
```

`fix_ptrwidth` runs **four passes earlier** and rewrites that cast, so 20 sites never matched
and **no decline was printed** — a pattern that does not fire has nothing to report. `this`
goes to argument word one and `clamped` to word two, so at LP64 the write of `this` lands on
top of `clamped`'s low half and the read comes back `0x00007fff00007fff`, the two pointers'
high halves interleaved; that is the `rsi` in the `setClampedValues` SEGV, and the arithmetic
predicts it exactly. The matcher is **structural** now — it walks out to the frame group's own
closing parenthesis and asks what follows it — so no cast spelling can blind it, and its
self-check runs it under `(int)`, `(kd_iptr)`, `(kd_uptr)` and no cast at all, plus once over
its own output.

The i386 code pushes its arguments, so Ghidra fabricates a local to hold the outgoing area and
writes each word into it at a constant byte offset — then reads the same offsets back as the
call's arguments. Every offset is an argument WORD and every word is four bytes because that is
what a pointer weighed on the shipped target. At LP64 slots 4 and 5 overlap and the write to
slot 5 runs four bytes off the end of a 24-byte array. That is `MstUtils.c:91`; `McdBatch.c:829`
is its consequence one call deeper.

★ **`proven.txt` called this class unfixable under the byte-identity gate, and that was a
prediction.** LP64-TWO-REMAIN reasoned: "It is a PIPELINE change, so `metoolkit_decomp/src` moves and
byte-identity CANNOT be the gate." True of the repair it had in mind — converting the shape
upstream in `recover.py` — and not true of scaling the offsets **and the object** together in a
post-pass:

```
+ 0x14                ->  + (5 * (int)sizeof(void *))
int aiStack_9cb0[6];  ->  int aiStack_9cb0[6 * (int)(sizeof(void *) / 4)];
```

Both fold to the original text at 32-bit pointer width. Measured on `MstUtils`, the object whose
upstream attempt "knocked it out of the build entirely": 123 offsets scaled, all three areas
widened, object byte-identical. **Never decline a repair on a prediction.**

⚠ **Two refusals are the whole safety.** Only Ghidra's own `<type><Stack|StackY>_<hex>` names;
every offset must be a multiple of four; and the name must appear nowhere except inside one of
these casts, because if the recovery uses `fStack_9c98` as the float it is declared to be then
widening it changes that read. Seven areas decline there and are reported.

⚠ **All or nothing per area, and RECOMPUTE between areas.** A half-scaled area overlaps
*differently* rather than not at all. And the areas interleave — `MstUtils` writes
`aiStack_9cb0` and `aMStack_9c94` in alternate statements — so offsets taken against the original
text are stale the moment one area lands, and the next area's declaration edit falls in the
middle of an expression. It reads as "not byte-identical", which is **a decline on a repair that
was never tried**.

⚠ **A greedy regex ate the first letter of every name.** `aiStack_9cb0` was captured as
`iStack_9cb0`, a name that appears nowhere — so the area had no sites and was skipped SILENTLY.
The tool reported nine clean repairs while the object it was written for was untouched.

★★ **AND IT BLINDS `check_frame_bounds.py` IF YOU LET IT — measured, not argued.** That detector
exists for exactly this defect and reads *constant* offsets and *constant* array bounds; after
this pass both are constant expressions. A deliberate `+ 0x400` into `aiStack_9cb0` is caught
before the post-pass and **missed** after it. The gate has been taught both spellings, re-checked
against the same deliberate violation, and `lp64_pipeline.sh` now runs it after the post-passes
so the zero is stated rather than assumed.

★ **A FOURTH SPELLING: THE AREA BELOW AN `alloca`, REACHED THROUGH A CURSOR.** gcc puts the
allocated block immediately *above* the outgoing argument area, so Ghidra spells the area as
NEGATIVE offsets from the allocation:

```c
kd_alloca_iVar3 = (char *)alloca(n * (int)sizeof(*(McdUserTriangle *)0) + 0);
puVar12 = (undefined1 *)kd_alloca_iVar3;
*(int *)    (puVar12 + -0x20) = (kd_iptr)&(triList->list)->mRefCtAndID + i;   /* tri  */
*(MeReal **)(puVar12 + -0x1c) = relPos;
GenerateTriangleContact(*(McdUserTriangle **)(puVar12 + -0x20), …);
```

`ghidra_clean.py`'s `materialise_alloca_relative_slots` gives that shape real storage where the
offsets hang off `kd_alloca_*` **directly**; here the pointer is copied into a local first, so it
never matched. It is two defects at LP64: the slots **overlap** (`-0x1c` and `-0x20` are four
bytes apart and the pointers written into them are eight), and `tri` is stored through
`*(int *)` and read through `*(McdUserTriangle **)` — four bytes of eight, then an eight-byte
read of the truncation. `GenerateTriangleContact` dereferences `tri->vertices[0]`, which is
`McdSphylTriangleListIntersect`'s `SEGV_MAPERR` on the device and 6 of 8 gametypes at x86-64.

★ **THE STORAGE IS THE HARD HALF, AND THE `alloca` IS WHERE IT COMES FROM.** Scaling the offsets
alone doubles the area to 72 bytes below a block whose base is where the allocation *starts* —
below it is not ours at either width. So the allocation grows by the scaled area and the base
walks up past it, on one statement:

```
alloca(SIZE)   ->   alloca(SIZE + PAD) + PAD
PAD = (int)(sizeof(void *) / 8) * (nslots * (int)sizeof(void *))
```

`4 / 8` is zero in integer arithmetic, so `PAD` is **0 at i386 and on wasm32** and the statement
folds back to `alloca(SIZE) + 0` on every shipping target — the byte-identity gate is what says
so. At LP64 it is 72 bytes and the area moves inside the allocation. In the artefact:
`add $0x48,%rax`, twice.

⚠ **THE CURSOR MUST BE A CURSOR AND NOTHING ELSE.** The block itself is real memory handed to
callees — here the triangle array the generator fills — so a positive offset or a bare use means
the name is the ARRAY, and the site is declined rather than guessed at.

★ **A FIFTH SPELLING: A FRAME SLOT'S MEMBER HOLDING A POINTER.** Where Ghidra's invented local is
a **struct**, one of its members can be the spill slot for a pointer — and the struct is a real
declared type, so the member cannot be widened the way an `int aiStack_9cb0[6]` can:

```c
McdUserTriangle MStack_26c;
MStack_26c.flags = (McdTriangleFlags)result->normal;
*(float *)(MStack_26c.flags + 4) = diff.v[1] + *(MeReal *)(MStack_26c.flags + 4);
```

`McdUserTriangle::flags` is four bytes at **both** widths, so at LP64 the address of
`result->normal` is cut in half. It is `ptrwidth_classify`'s worst single object —
`IxCylinderTriList` **6 of 6 UNEXPLAINED** — and it is on the ragdoll-vs-**world** path, where the
first Android Karma tombstone landed. ★ The repair is **storage, not a re-spelling**, and that was
measured: rewriting the reads as `result->normal[1]` is exact — the line right above them already
does — and is *not* byte-identical, with or without the now-dead store. Giving the member its own
`kd_iptr` local and leaving every expression otherwise verbatim **is**. ⚠ Only if `&NAME` is never
taken; otherwise a callee reads the member and splitting it out silently stops updating it.

★ **A SIXTH SPELLING: A SLOT STORED NARROWER THAN THE NEXT READ OF IT**, which the fifth exposes.
An argument slot is *reused* across calls, so the same address carries an `int` for one call and a
pointer for the next:

```c
*(int *)             (&(*kd_argslot_fffffd84)) = triList->triangleMaxCount;
… McdCylinderIntersect(…, *(int *)(&(*kd_argslot_fffffd84)));            /* fine  */
*(McdTriangleFlags *)(&(*kd_argslot_fffffd84)) = kd_slot_MStack_26c_flags;
MeVector3Normalize(  *(MeReal **)(&(*kd_argslot_fffffd84)));             /* FOUR  */
```

⚠ **GROUPING BY ADDRESS WOULD BE WRONG** — widening every access to that slot takes the
`triangleMaxCount` store with it, and that one is a genuine `int` read back as a genuine `int`.
The pairing is a **store and the next read of the same address with no store in between**, which
is the only thing that makes the two widths comparable. Six sites across five functions, including
22 in `MeFAssetCreateCopy` — the ragdoll asset loader — where an `alloca` pointer was stored four
bytes wide into a slot read as a pointer.

⚠ **AND THE TYPE GROUP HAS TO ADMIT ITS OWN `*`s.** Written `[\w ]*?` the site pattern matches
`*(int *)` and **not** `*(McdUserTriangle **)`, so the pass sees the four-byte stores, misses
every pointer load, concludes no slot carries a pointer and widens nothing — the one access the
rule exists for. It found zero sites in the file it was written for and reported **no decline**,
because a group that never forms has nothing to decline.
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
`test/standalone/trace_cylcyl.sh` is the worked use.

### `find_cylinder_geom.py` — does any shipped asset define this geometry?

Answers a scoping question that retires five registered collision pairs. **Grep cannot answer it**:
`FKAggregateGeom` is serialised by a hand-written `operator<<` that writes four `TArray`s in a fixed
order with **no field names**, so `CylinderElems` appears in no package. This parses the packages
instead.

```bash
python3 tools/find_cylinder_geom.py $UT2004_ASSETS_DIR
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
`$GHIDRA_HOME` (Java 21, headless only). `../HANDOVER.md` §5 has the
full-corpus invocation and `KD_GHIDRA_OPTS`. **The scripts must be COPIED to
`lab/gscripts/`** — Ghidra reads them from there, not from this repo.

A single-object re-dump, which is the form you actually want when testing a hypothesis:

```bash
cd lab
cp decomp/tools/gscripts/*.java gscripts/
mkdir -p one_obj out_verify gproj_verify && cp allobj/McdSpace.o one_obj/
export KARMA_PROTOS=lab/kd_protos11.h
export KARMA_OUTDIR=lab/out_verify
$GHIDRA_HOME/support/analyzeHeadless \
  gproj_verify Proj -import lab/one_obj \
  -scriptPath lab/gscripts \
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

