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

## Start here

* **[`HANDOVER.md`](HANDOVER.md)** — everything needed to resume the recovery work:
  pipeline, Ghidra invocation, running and instrumenting the game, dead ends, and what
  "complete" means. Read this first.
* **[`HANDOVER-WEB.md`](HANDOVER-WEB.md)** — self-contained brief for the wasm/Android
  integration, written for someone with no history on the project.
* [`proven.txt`](proven.txt) — which objects a real match has validated, with the evidence.

## Status

**Current, 2026-08-24 — everything below this block is historical narrative kept for the
reasoning, and its numbers are the numbers of the day it was written. `HANDOVER.md` is the
live document; when the two disagree, HANDOVER wins.**

| | |
|---|---|
| objects recovered | **109 compile** (73.6% of 148 attempted), 25 quarantined by detectors, 14 fail |
| collision pairs the game calls | 15, of which **14 are validated** against the shipped original on live inputs |
| the one that is not | `IxCylinderCylinder` — measured wrong, 925 `dims_diff` in 24,111 real calls, and no detector holds it |
| the engine ON recovered Karma | **runs** — all 108 objects substituted, plays a match, indistinguishable from stock (§7c) |
| wasm32 | 109/109 compile, byte-identical exported symbol sets. **Nothing executed yet** |
| arm64/armv7 | not attempted — no cross-compiler on this machine |
| the solver | arithmetic proven bit-exact over 900 steps; its **control flow does not compile**, blocked on frames the DWARF does not describe (§11 item 2) |

| milestone | |
|---|---|
| Milestone 1 — one object end to end | ✅ **done** (`McdPrimitives/IxBoxBox`) |
| Milestone 2 — `MdtKea` C++/vtable spike | ✅ **done — no blocker** |
| Milestone 3 — scale validation | ✅ **done** — 109 objects, seven gates, engine runs on them |
| Milestone 4 — the grind (~2,100 functions) | 🔶 **collision done, solver blocked** — §11 item 2 |
| Milestone 5 — wasm + Android bring-up | ⬜ — compiles, never executed. `HANDOVER-WEB.md` |

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
compile:      no human input needed :  33 / 148        (was 14/96)
              prelude has TODOs     :   2 / 148
              needs human review    :  37 / 148        (see below)
              did not compile       :  76 / 148

substitute:   trajectory bit-identical to the shipped library : 35 / 35
              (collision scene: 34 bit-identical, 1 diverged — see below)
              link or run failed                              :  0 / 35
```

**Every object that reaches the validated set reproduces the reference physics bit-for-bit.** Not "compiles", not
"doesn't crash" — identical trajectories. That is the number worth tracking, and it is why the
compile rate on its own is a poor proxy: an earlier iteration compiled 40 objects of which only 26
worked. Fewer compiling and more correct is the right trade.

> **CORRECTION, 2026-08-24 — do not cite the paragraph above.** The numbers in this
> section are a historical snapshot (`33/148`; it is `94/148` now), but the *claim* is the
> problem, not the arithmetic. `trajectory bit-identical` has three possible meanings and
> this gate cannot separate them: the recovery is exact, the object never executed, or it
> executed and nothing it computes is measured. Measured across all three scenes, **only
> eight of 103 objects have demonstrated sensitivity on any scene** — for the rest, that
> line is about the link, not the code. See `HANDOVER.md` §4a, and
> `test/scene_census.sh` / `test/gate_sensitivity.sh`, which exist to tell the three apart.
> The real evidence for the released collision objects is the shadow harness and
> `difftest_pair`, recorded in `proven.txt`.

The trajectory check is only this sharp because baseline and substituted build are **both i386/x87**,
so a correct recovery has no reason to differ at all. It does NOT transfer across builds — see
[`../docs/KARMA-ON-WASM.md`](../docs/KARMA-ON-WASM.md) §II.3.

An object that is itself on the collision path will always diverge on a *collision* scene, because
contact make/break amplifies a last-bit difference without bound. `IxBoxBox` does exactly that, and
it is proven correct on 300,000 real model pairs. So the collision-free scene is the authoritative
trajectory signal, and the per-function gate is the verdict.

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

- **76 objects do not compile.** No dominant cause left — a long tail (individual missing types,
  `conflicting declaration`, Ghidra artifacts like `stack0xffffffb4`).
- **36 objects need the mislabelled-symbol review above.**
- **37 objects need review**, 36 for the mislabelled-symbol issue above and one for a second,
  nastier pattern found by the gate:

  `MdtPartition` compiled, linked, and exported a symbol set **identical to the original** — then
  segfaulted in `MeDictNext(dict, NULL)`. Ghidra had failed to model the function's stack frame and
  invented a local array, routing call arguments through it at computed offsets:

  ```c
  *(MeDict **)((int)aiStack_50 + iVar8 + iVar16 + 4) = dict;
  pMVar9 = MeDictFirst(*(void **)((int)aiStack_50 + iVar8 + iVar16 + 4));   /* garbage */
  ```

  It is the same failure as the call-arity bug, one level deeper. `recover.py` now detects the shape
  (`aiStack_NN + var`), classifies the object REVIEW, and **deletes the object file** so broken code
  cannot reach the validated set. Silently-wrong code is the worst outcome available; this converts
  it into a flagged one.
- **The precise gate is still validated on one function.** `difftest_boxbox.c` proves the method;
  scaling it has not been done.

#### Shadow testing against the real game

`test/kd_shadow.c` + `test/make_shadow_metoolkit.sh` + `test/run_map.sh` run UT2004 headless under
Xvfb with a shadow of the recovered code. Every collision call in real gameplay becomes a test case;
the engine only ever consumes the ORIGINAL's result, so behaviour is unchanged.

**Hook the registration, not the functions.** The first design renamed each intersection function to
`orig_*` and defined a replacement under the original name. It never ran — nothing calls
`McdBoxBoxIntersect` by name. Karma installs it as a function POINTER in an interaction table, so
renaming the symbol also rewrote the table's own reference. Interposing
`McdFrameworkSetInteractions` instead sees every `(geometry type, geometry type)` pair the engine
installs: one hook for the whole collision matrix, no per-function code, and new pairs are picked up
automatically as more objects are recovered.

Recovered objects are staged with **every** defined symbol prefixed `rec_`, not just the one under
test — otherwise their siblings collide with the shipped archive and the linker silently picks one of
the two, which would mean measuring a half-recovered build.

##### Result: the census is the finding

`test-karma-1`, 150 s (10 KActors, 2 hinges, a cone limit, a ball-socket joint, an ONSRV):

| pair | calls | identical | fp-only | **structural diffs** | worst delta |
|---|---:|---:|---:|---:|---:|
| Sphere × Sphere *(recovered)* | 121,691 | 92,365 | 29,326 | **0** | 5.96e-08 |
| Sphere × TriangleList | 935,762 | — | — | — | *not recovered* |

`McdSphereSphereIntersect` is validated on 121k real inputs with zero divergence in any discrete
field and a worst numeric delta of one ULP of f32.

**37 interaction pairs are registered; the map exercises two.** Sphere × TriangleList alone is 88% of
all calls. `IxBoxBox` — the object polished first, proven on 300k *synthetic* pairs — is called
**zero** times. Effort should follow the census, not the alphabet: `IxSphereTriList` is the next
object to recover, not whatever comes next in the archive.

##### The census, with vehicles and ragdolls running

The blocker below was a travel-URL problem, not a harness one (see "Travel URLs"). With a complete
URL, ONS-Torlan exercises **9 of 37** pairs instead of 0. Combined across both maps, by source object:

| calls | object | status |
|---:|---|---|
| **941,805** | `IxSphereTriList` | not yet recovered |
| 122,527 | `IxSphereSphere` | ✅ recovered, **0 structural divergence** |
| 36,828 | `IxSphylPrimitives` (ragdolls) | not yet recovered |
| 6,290 | `IxConvexPrimitives` (vehicles) | not yet recovered |
| 1,522 | `IxBoxBox` | ✅ recovered, **0 structural divergence** |

Both recovered functions hold up on real vehicle-and-ragdoll gameplay with zero divergence in any
discrete field.

And the priority is unambiguous: **two objects carry 89% of all collision traffic**, and both failed
on the same Ghidra error — `stack0xNNNNNNNN undeclared`.

##### The alloca pattern — and the wrong fix for it

Two objects carry 89% of collision traffic and both failed on the same Ghidra
error, `stack0xNNNNNNNN undeclared`. It marks a variable-length stack allocation.
The assembly says exactly what it is:

```
mov 0x28(%ecx),%edx        ; count = geom->maxTriangles
lea (%edx,%edx,2),%eax     ; x3
lea 0xf(,%eax,8),%edx      ; x8  -> count*24 + 15
and $0xfffffff0,%edx       ; round up to 16
sub %edx,%esp              ; alloca(count * 24)
```

**The first fix was a fixed 64 KB buffer, and it was wrong in the worst possible
way.** It compiled. It passed the substitute gate bit-identically — because the
scripted scenes never exercise `Sphere × TriangleList`. Then a real match
segfaulted on the *first* call into recovered code, handing a wild pointer
straight into the engine's own `KTriListGenerator` callback:

```
rec_McdSphereTriangleListIntersect+0x205
  -> KTriListGenerator(McdModelPair*, McdUserTriangle*, ...)
     -> KME2UPosition(FVector*, const float*)   <- SIGSEGV
```

A turned-into-a-crash compile error is worse than a compile error. The fix now
emits a **real `alloca()`**, which reproduces the original exactly and needs no
size assumption — and only at the *defining* use, `dest = (T)(&stack0xH + negVar)`.
Other `stack0xH` sites are stores into the shifted frame, Ghidra's rendering of
outgoing call arguments it failed to model; substituting there would hand out a
fresh block per store. Leaving them unresolved is deliberate — the object fails
to compile and is held back, which is the honest outcome.

##### Why IxSphereTriList is still held back — the real root cause

The alloca was a symptom, not the cause. With it restored correctly the object
compiles, and it still crashes a real match in the same place. The actual line is:

```c
count = (*pcVar11)();          /* Karma calling the ENGINE's KTriListGenerator */
```

**An indirect call emitted with no arguments.** Ghidra has no signature for a call
through a function pointer, so it drops every argument; the callee then reads
whatever is on the stack. `kd_protos.h` cannot help — it resolves calls by NAME,
and this call has none.

That is what killed it: Karma calls back into `KTriListGenerator` through
`McdTriangleListFnPtr`, got garbage for `pos`, and died in `KME2UPosition`. It
compiled, and it passed the substitute gate, because the scripted scenes never
reach `Sphere × TriangleList`.

Levers tried and exhausted:

| lever | result |
|---|---|
| DWARF-derived prototypes (`gen_protos.py`) | fixes NAMED calls; cannot reach a call through a pointer |
| "Decompiler Parameter ID" analyzer | no effect on arity |
| Alternate simplification styles (`normalize`, `firstpass`, `register`, `paramid`) | none produce C at all; only `decompile` works |
| Real `alloca()` instead of a fixed buffer | correct, and necessary — but not the cause |

What remains is a Ghidra **call-site signature override**
(`HighFunctionDBUtil.writeOverride`) applied per indirect call site, or hand
reconstruction of the argument list from the assembly. Both are per-site work.
Only 2 objects hit this, so the blast radius is small — but they are the two that
carry most of the game's collision traffic.

`recover.py` detects the shape and holds such objects back.

##### The old alloca note

That error marks a VARIABLE-LENGTH stack allocation:

```c
iVar15 = -((int)count * 0x18 + 0xfU & 0xfffffff0);   /* round up to 16, negate */
ptr    = (McdGeometryID)(&stack0xfffffeb4 + iVar15); /* sp -= size             */
```

Ghidra cannot model a frame whose size changes at runtime, so it names the stack location instead of
allocating it. **21 objects** hit this, including both top-traffic ones.

`ghidra_clean.py` now materialises a backing buffer per function. The addresses are meaningful
*relative to each other*, so one local buffer plus a per-symbol offset reproduces the layout exactly,
and being a local it stays per-invocation like the original. The one thing it adds that the original
did not have is a size cap (`KD_ALLOCA_FRAME`, 64 KB) — an assumption, but one the shadow harness
will expose as divergence rather than hide.

##### Travel URLs

A map loading with the right gametype is not enough. UT2004's travel URL carries **two** kinds of
option, and both matter:

* **game options**, parsed by `GameInfo.InitGame()` — `Game=`, `bAutoNumBots=`, `QuickStart=`,
  `bPlayerMustBeReady=`
* **per-player login options**, consumed in `PreLogin`/`Login` — `Name=`, `Class=`, `Character=`,
  `team=`

Supplying only the first leaves the match parked before kickoff: the level loads, the process burns
CPU at full tilt, and **nothing ticks**. A complete URL is what unblocked it:

```
ONS-Torlan.ut2?Name=Player-43ce41?Class=Engine.Pawn?Character=Jakob?team=255
  ?bAutoNumBots=True?Game=Onslaught.ONSOnslaughtGame?QuickStart=True?bPlayerMustBeReady=False
```

##### Still open

`test-karma-1` works because its KActors fall under gravity with no agent involved. `ONS-Torlan` and
`DM-BB-VehicleWar` load correctly, with the right gametype, and then **never tick at all** — 0 Octree
events against 598,571 for `test-karma-1`. One player joins, no bots, so the match presumably never
leaves its pre-match state. `?NumBots`, `?MinPlayers`, `?bAutoNumBots=False` and `?QuickStart` made
no difference.

Until that is solved, vehicle and ragdoll paths (cylinder, sphyl, convex mesh) stay unexercised — and
those are exactly the ones the census says matter.

#### Three assets, one still unused


1. **UT2004 itself is a correct, human-written caller.** `Source/Engine/Src/Karma*.cpp` and `K*.cpp`
   use ~328 Karma API functions with the right types and conventions — better evidence of intended
   signatures than Ghidra's inference, and a map of which functions actually matter.
2. **The `native-karma` build can be instrumented.** Shadow-differential testing: wrap each recovered
   function so every call runs BOTH the original and the recovered version on the same inputs, into
   separate output buffers, and compares. The game keeps using the original's result, so behaviour is
   unchanged — but every call becomes a test case with *real* inputs. This is the scaled precise gate.
3. **Bespoke test levels** can drive specific physics paths (ragdolls, each vehicle type, each
   geometry pair) far more thoroughly than a scripted scene.

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
