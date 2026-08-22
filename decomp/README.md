# karma-decomp — recovering Karma from the shipped binaries

Recovers the proprietary, binary-only Karma (MathEngine `metoolkit`) physics library as portable C,
so the web (wasm) and Android builds can have real Karma physics instead of `NO_KARMA`.

**Why this and not an emulator:** the engine and Karma share one address space in both directions —
the engine holds raw pointers into Karma's heap and Karma holds function pointers into engine code —
so no out-of-address-space emulator (v86, QEMU-system, blink-as-a-VM) can work without rewriting the
integration layer. Full analysis, measurements and the rejected alternatives are in
[`../docs/KARMA-ON-WASM.md`](../docs/KARMA-ON-WASM.md).

**Why it's tractable:** the shipped archives carry **full DWARF-2 debug info** — original file names,
line tables, parameter names, local names, complete types. Ghidra consumes that directly. Measured
over the entire hot path (100 objects, 381.5 KB), **1092 of 1092 functions decompile**, with two x87
stack leaks totalling 235 bytes.

## Status

| | |
|---|---|
| Milestone 1 — one object end to end | ✅ **done** (`McdPrimitives/IxBoxBox`) |
| Milestone 2 — `MdtKea` C++/vtable spike | ✅ **done — no blocker** |
| Milestone 3 — scale validation | 🔶 **advanced, not finished** — 27% compile, 26/40 pass the breadth gate |
| Milestone 4 — the grind (~2,100 functions) | ⬜ |
| Milestone 5 — wasm + Android bring-up | ⬜ |

### Milestone 1 result

`IxBoxBox` recovered, recompiled, substituted into the link in place of the shipped object, and
differential-tested against the original on **300,000 real `McdModelPair`s**:

```
  pairs actually touching        : 29747 (9.9%)
  bit-identical results          : 279540 (93.180%)
  DIFFERENT touch/return         : 0
  DIFFERENT contact count        : 0
  DIFFERENT contact dims         : 0
  worst absolute numeric delta   : 8.702e-06

RESULT: PASS -- every discrete decision matches; deltas are FP rounding only
```

Every discrete decision — overlap yes/no, contact count, contact dimensionality — matches exactly.
The residual is float last-bits, at micrometre scale.

### Milestone 2 result — C++ and vtables are not a blocker

`MdtKea` is the hottest 68.5 KB *and* the only C++-with-vtables code in Karma (`keaMatrix`,
`keaMatrix_pcSparse`, `keaMatrix_pcSparse_vanilla`, `keaLCPSolver`, `keaFunctions_Vanilla`). It was
attacked second precisely because it was the most likely place to find a blocker. There isn't one.

1. **`this` is stack-passed cdecl.** GCC's i386 C++ ABI passes `this` as the first stack argument —
   unlike MSVC, which uses `ecx`. Verified in the disassembly of `keaMatrix_tester::factorize`:
   `mov 0x8(%ebp),%ebx`. **A C function with an explicit `this` parameter is ABI-identical** to the
   original method — no thunks, no wrappers.
2. **Virtual dispatch is explicit and mechanical.** Ghidra renders it as
   `(**(code **)(*(int *)obj + N))(obj, ...)`. The vptr points at C++ slot 2 (Itanium ABI reserves
   slot 0 for offset-to-top and slot 1 for the typeinfo pointer), so `N` is a plain byte offset into
   a C `struct ..._vtbl` of function pointers.
3. **All class layouts are in DWARF** — but only in the CU that *defines* each class, not the ones
   that use it. `keaMatrix_PcSparse_vanilla.o` shows `field_0x14`; `keaMatrix_PcSparse.o` has the
   names. **Union DWARF across all objects to build the type database.**
4. **We own the vtables.** Only `libMdtKea` references `_ZTV*kea*` / `_ZTI*kea*`, so no other
   archive constrains the layout.
5. **All 20 `MdtKea` objects decompile, with zero x87 stack leaks.**

The recovered hierarchy is in [`src/MdtKea/keaMatrix.h`](src/MdtKea/keaMatrix.h), with
`_Static_assert`s on every size and offset — they pass, so the layout is byte-identical to the
shipped one.

Two independent cross-checks corroborate the recovery, which is what makes it trustworthy:

- **Vtable slots vs. call sites.** `keaMatrix_tester::factorize` calls `vptr+0x0c` and `::solve`
  calls `vptr+0x10`; the vtable relocations put `factorize()` and `solve()` at exactly those offsets.
- **Field offsets vs. string literals.** `keaMatrix_pcSparse_vanilla::allocate` labels each
  `keaPoolAlloc` with the field it fills — `"A"`, `"Achol"`, `"rsD"`, `"NAZ"`, `"NCZ"`, `"NR"`,
  `"NC"` — and those labels land on exactly the DWARF members at `+0xc`, `+0x10`, `+0x18`, `+0x1c`,
  `+0x20`, `+0x2c`, `+0x30`. The original authors annotated their own layout.

### Milestone 3 status — batch recovery + breadth gate (advanced, not finished)

`tools/recover.py` runs the whole recipe over every object; `test/substitute_test.sh` then swaps each
recovered object into the link and runs a physics scene. Both halves matter, because **compiling is
not the same as working**.

```
compile:      no human input needed :  38 / 148        (was 14/96)
              prelude has TODOs     :   2 / 148
              needs human review    :  36 / 148        (see below)
              did not compile       :  72 / 148
              -> 27.0% compile      (was 14.6%)

substitute:   ran the scene cleanly :  26 / 40
              crashed / NaN / short :  14 / 40
```

**35% of the objects that compile are still broken.** That is the single most useful number here, and
it is why the breadth gate exists at all.

The `IxBoxBox` acceptance gate still passes after every change above (91.14% bit-identical,
0 structural differences), regenerated end-to-end through the new pipeline.

#### What moved the compile rate

Each fix shifted the whole batch at once, which is why batching was worth it over hand-fixing:

| fix | effect |
|---|---|
| deduplicate functions + asm labels for exports | "conflicting declaration" 17 objects → 1 |
| feed Ghidra DWARF-derived prototypes | "missing function declaration" 28 → 7 |
| `kd_protos.h` for plain imports, data decls in `header_declaring` | closed the `BaseConstraint*` / `MeMemoryAPI` TODOs |
| downgrade pointer/int type-spelling diagnostics | +18 objects |

**Giving Ghidra prototypes was the important one, and not for the reason expected.** Without a
signature for an imported function, Ghidra guesses the arity from the call site and gets it wrong —
in `IxBoxTriList` it decided `McdModelGetGeometry` takes no arguments and emitted the pushed
arguments as writes to unrelated stack variables:

```c
pMStack_25c = p->model1;
fStack_260  = 9.18817e-41;              /* a pointer misread as a denormal float */
boxgeom = (McdBoxID)McdModelGetGeometry();
```

That is a correctness defect, not a cosmetic one. Enabling "Decompiler Parameter ID" does not fix it.
Feeding metoolkit's own headers to Ghidra's C parser does not work either — they are layered with
`MEAPI`/`MEPUBLIC` macros it rejects. So `tools/gen_protos.py` generates 2218 prototypes from the
same DWARF instead, with simplified types, and `gscripts/ParseKarmaHeaders.java` applies them. After
that the call reads `McdModelGetGeometry(p->model1)` — and the spurious `longdouble` disappears too,
because the return type is finally known.

#### The REVIEW category — do not paper over this one

36 objects contain an indirect call through a symbol Ghidra resolved to the **wrong name**:

```c
pMVar2 = (McdCylinderID)(*_McdGeometryDeinit)(0x1c, 0x10);   /* size, alignment */
```

`McdGeometryDeinit` is a real function, but those arguments are obviously an allocator call — Ghidra
mis-resolved a relocation-with-addend against a data symbol. Declaring the symbol to make it compile
would produce silently wrong code, so `recover.py` classifies these separately and leaves them
failing. They need a human.

#### What is still open

- **72 objects do not compile.** No dominant cause left — it is now a long tail (missing individual
  types, `conflicting declaration`, Ghidra artifacts like `stack0xffffffb4`).
- **36 objects need the mislabelled-symbol review above.**
- **14 of 40 compiling objects fail the breadth gate.**
- **The precise gate is still validated on one function.** `difftest_boxbox.c` proves the method;
  scaling it — auto-driving from DWARF signatures, and capture/replay for deep pointer graphs — has
  not been done.

## The recipe## The recipe

Per object, three automated steps and one small manual one.

### 1. Decompile (automated)

```bash
KARMA_OUTDIR=/path/to/out \
  $GHIDRA/support/analyzeHeadless <proj-dir> <proj> \
    -import <object-or-dir> \
    -scriptPath karma-decomp/tools/gscripts \
    -postScript DumpDecomp.java -deleteProject
```

Emits one `.c` per object plus `stats.csv` with per-function quality metrics.

### 2. Write the prelude (mostly generated)

`tools/gen_prelude.py` produces the skeleton, reading everything it can out of the object:

```bash
python3 tools/gen_prelude.py <obj>.o --include-dir ../Thirdparty/metoolkit/include -o <obj>.prelude.h
```

It finds which metoolkit header declares each import, emits `KD_MANGLED()` asm labels with the
**exact** mangled name for C++ imports, reads static initialisers straight out of `.data`/`.rodata`,
and marks `.bss` statics TODO (their values come from a C++ static constructor, not the section
bytes). Anything it can't determine is marked TODO. **Nothing it emits is a guess.**

That matters, because both values guessed by hand while building the `IxBoxBox` prelude were wrong:

```bash
# function-local statics live in .data — read the bytes
objcopy -O binary --only-section=.data IxBoxBox.o /tmp/d.bin
python3 -c "import struct;print(struct.unpack('<f',open('/tmp/d.bin','rb').read()[:4]))"
#   -> 0.2f   (guessed 0.5f)

# C++ imports need the exact mangled name for the asm label
nm --undefined-only IxBoxBox.o | grep Moving
#   -> _Z21MovingBoxBoxIntersectPKfPK11lsTransformRK6lsVec3S0_S3_S6_fRfRS4_
#      (guessed ...S1_S4_S7_...)
```

For structs — especially C++ classes, where Ghidra may leave `field_0x14` — recover the real layout:

```bash
python3 tools/dwarf_structs.py <obj>.o --list          # what types does this CU define?
python3 tools/dwarf_structs.py <obj>.o --type keaMatrix
```

A class layout lives only in the CU that **defines** the class, so search across all objects.

### 3. Clean (automated)

```bash
python3 karma-decomp/tools/ghidra_clean.py out/IxBoxBox.o.c \
  -o src/McdPrimitives/IxBoxBox.c \
  --object <original>/IxBoxBox.o \
  --prelude src/McdPrimitives/IxBoxBox.prelude.h \
  --drop __static_initialization_and_destruction_0 \
  --drop _GLOBAL__I_McdBoxBoxIntersect
```

Strips Ghidra's advisory comments, marks originally-`static` functions static (read from the real
object, so the exported ABI matches exactly), flattens C++ `Foo::bar` to `Foo__bar`, and emits
forward declarations so definition order doesn't matter.

C++ static-constructor scaffolding is dropped: for `IxBoxBox` it only filled three constant vectors,
which become plain C initializers in the prelude.

### 4. Validate (automated) — this is the acceptance gate

Rename the original's exports so both implementations coexist in one process, then compare on real
inputs:

```bash
objcopy --redefine-sym McdBoxBoxIntersect=orig_McdBoxBoxIntersect ... orig.o
gcc -m32 ... difftest_boxbox.c recovered.o orig.o -Wl,--start-group <archives> -Wl,--end-group
```

For `static` functions, `objcopy --globalize-symbol` makes them callable first.

Because Karma is pure computation with no syscalls, **every function is a deterministic function of
its inputs — so the shipped archive is an oracle for its own replacement.**

## Layout

```
include/kd_compat.h                  Ghidra type/macro vocabulary; the longdouble + ROUND decisions
tools/gscripts/DumpDecomp.java       Ghidra headless decompile + per-function quality stats
tools/ghidra_clean.py                Ghidra dump -> compilable C
tools/gen_prelude.py                 per-object prelude skeleton, read from the binary
tools/dwarf_structs.py               C struct/class layouts recovered from .debug_info
src/<Archive>/<Object>.c             generated — do not edit by hand
src/<Archive>/<Object>.prelude.h     imports + statics (generated, then completed by hand)
src/MdtKea/keaMatrix.h               recovered C++ hierarchy + explicit vtable (Milestone 2)
test/difftest_<object>.c             differential gate vs the shipped binary
test/scene_*.c                       whole-simulation scenes for coarse regression
```

## Load-bearing decisions

**`longdouble` → `double`.** Ghidra emits `longdouble` where the gcc 3.2 i386 build genuinely
computed at x87 80-bit. We map it to `double`, not C `long double`: on wasm `long double` is a
software-emulated 128-bit quad, and MathEngine themselves shipped a pure-f32 build
(`lib.rel/linux_hx_single`, x86-64/SSE), so f32 intermediates are vendor-blessed and f64 is strictly
better than that.

**`ROUND` → `rintf`, not a cast.** Ghidra emits `ROUND` for x87 `fistp`, which converts using the
current rounding mode — round-to-nearest, ties-to-even. `(int)x` truncates and would be a real
behavioural bug.

**Decompile i386, not x86-64.** wasm32 has 4-byte pointers, and the engine shares structs with
Karma, so only a 32-bit Karma has the right layout. The i386 build is also the only one (with
gcc2.95) carrying DWARF.

**Acceptance is per-function, not per-trajectory.** Past first contact, *any* two builds of Karma
diverge without bound — MathEngine's own i386 and x86-64 builds differ by 111 m after 15 s. UT2004
knows this and replicates `KRigidBodyState` rather than relying on determinism. So trajectory
diffing cannot validate a replacement; differential testing per function can.

## Known gaps

- **`-Wno-int-conversion` is required.** Ghidra types some pointer-valued locals as `undefined4`.
  Harmless on any 32-bit-pointer target (i386, wasm32) but latent, and worth fixing properly by
  retyping pointer-valued `undefined4` locals in `ghidra_clean.py`.
- **Prelude generation is partly automated.** `gen_prelude.py` handles imports, mangled names and
  `.data` statics; `.bss` statics initialised by a C++ static constructor still need a human.
- **No project-wide type database yet.** Class layouts live only in the CU that defines them, so
  `dwarf_structs.py` must currently be pointed at the right object by hand. Unioning DWARF across all
  192 objects into one header set is the obvious Milestone 3 tool.
- **`keaMatrix.h` is a layout proof, not yet a build input.** The hierarchy, vtable and offsets are
  recovered and assert-checked, but no `MdtKea` object has been recovered and differential-tested end
  to end the way `IxBoxBox` was.
- **Two x87 stack leaks** remain, both unnamed helpers in `MdtBcl.o` (143 and 92 bytes).
- `writeKeaInputToFile` / `readKeaInputFromFile` (~22 KB) are solver debug serialisation — stub them
  rather than recover them. `MeViewer2` and `MeApp` (74 KB) are already unused; skip entirely.
