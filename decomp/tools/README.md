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
one.

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
python3 tools/ghidra_clean.py $LAB/out14/McdSpace.c -o /tmp/one.c \
        --obj $LAB/allobj/McdSpace.o --prelude /tmp/kd_out/allobj/McdSpace.prelude.h
```

> **Read the rule, then RUN it.** Six rules have looked correct and silently declined; reading alone
> has a 0/6 record here. Call the rule directly on the offending line.

### `gen_prelude.py` — the per-object prelude, read out of the binary

Imports (with demangled signatures and `KD_MANGLED` asm labels), file-scope statics with values read
from `.data`/`.rodata`, and explicit TODOs for anything it cannot determine.

```bash
python3 tools/gen_prelude.py $LAB/allobj/McdSpace.o \
        --protos $LAB/kd_protos11.h --corpus $LAB/allobj \
        --include-dir $MT/include --dump $LAB/out14/McdSpace.c \
        -o /tmp/McdSpace.prelude.h --exports-out /tmp/McdSpace.exports.h
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
have run. **Run them on a copy**, in this order.

```bash
cp -a /tmp/kd_out /tmp/kd_lp64
python3 tools/fix_baked_sizeof.py /tmp/kd_lp64/allobj /tmp/kd_build          # 98 sites
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

That test earns its keep: it caught `fix_baked_sizeof` changing `CxSmallSort` by 64 bytes, because
`count * sizeof(T)` is unsigned where `count * 0x98` was `int`. The fix is `(int)sizeof(...)`; the
lesson is that "no-op by construction" still gets measured.

### `fix_ptrwidth.py` — widen the punned casts

The recovery puns pointers through integer slots constantly (`*(T *)((int)base + K)`). Lossless at
32-bit; truncating at 64. Takes its site list from **clang's own diagnostics**, because nothing here
executes arm64 code and a heuristic wrong 1% of the time would be undetectable.

### `fix_baked_sizeof.py` — an allocation size frozen at the i386 value

`(MeMemoryAPI.create)(0x234)` where `0x234` is 564 is `sizeof(MdtWorld)` **on i386**; at LP64 the
struct is 880, so the first allocation is 316 bytes short and the next statement writes past it.
Nothing is truncated, no cast is narrowed, and neither static gate counts this shape at all. 105
literal-sized allocations across 41 objects.

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
`/home/ion/tools/ghidra_12.1.3_PUBLIC`; a single-object re-dump is ~50 s. `../HANDOVER.md` §5 has
the invocation and `KD_GHIDRA_OPTS`.
