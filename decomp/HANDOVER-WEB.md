# HANDOVER — Karma on the web, for the integration agent

## WHY THIS EXISTS, in four sentences

UT2004's physics is Karma, which ships as **binary-only x86 static libraries**. The web and
Android builds therefore ship with `NO_KARMA` and have no vehicles and no ragdolls. Another
workstream is recovering Karma from those binaries as portable C — **not to make a physics
library, but to produce a DROP-IN REPLACEMENT UT2004 links instead of MathEngine's**, so
that the game can have physics on your targets. **Nobody else will ever link this**, so
"correct" means "UT2004 behaves", not "faithful to MathEngine's product".

**Your half is the half that has never been tested: nothing this project has produced has
executed on wasm32, armv7 or arm64. Not one instruction.** That is the largest unknown in
the whole effort, and it is yours.

You are picking up the **web (WebAssembly) and Android** side of getting UT2004's physics
working outside x86. You do not need the history of how the source was obtained, only what
it is, what constrains it, and where the sharp edges are. This document is self-contained.

---

## 1. What you are being handed

UT2004's physics engine is **Karma** (MathEngine `metoolkit`, 2003). It ships as
**binary-only static libraries** — there is no upstream source and never was one publicly.
The web and Android builds of this engine therefore currently define `NO_KARMA` and have no
vehicle or ragdoll physics at all.

A separate effort (`karma-decomp/`, branch `karma/decompile`) is recovering Karma as
**portable C** from those binaries, using the DWARF-2 debug information the shipped
libraries happen to contain. That effort produces ordinary `.c` files.

**Your job is to make those `.c` files build and run correctly under Emscripten (and, as a
freebie, the Android NDK), and to integrate the result into the engine's web build.**

What exists today:

- **137 recovered objects**, all compiling for i386, wasm32, armv7 *and* arm64 — but see
  §3's pointer-size section: **arm64 is not trustworthy and wasm32 is**, for the same reason.
- **The full collision-detection path the game actually uses is recovered and validated** —
  all twelve interaction pairs UT2004 calls, each measured against the shipped original on
  real inputs from a live match. Evidence per object in `karma-decomp/proven.txt`. §6 has
  the census that says "twelve" and why that is the number to plan against.
- **The whole set already compiles under Emscripten**, and its exported symbol NAMES are
  **byte-identical to the i386 build for all 137 objects** — nothing added or dropped. So
  the ABI surface the engine links against does not change between targets, which was the
  thing most likely to turn this into a rewrite. See §4. (Names only: `wasm_check.sh`
  discards the binding letter. That gap let a recovered object export a *global* `putchar`
  for months — see "One thing that changed on your side of the fence" below.)

What does **not** exist, and you need to know this before planning anything:

- **Nothing has ever been EXECUTED under wasm.** "It compiles" is not "it works", and the
  runtime hazards in §4 are still open. That line is yours to cross, and it is §8 step 1.
- ~~**The solver's control flow is not recovered.**~~ **RECOVERED, 2026-08-25 — this
  blocker is GONE.** Every object in `libMdtKea` now reproduces the shipped library
  bit-for-bit on all three test scenes, and the engine has been rebuilt against them and
  RUN: the recovered `MdtKeaAddConstraintForces` executes 301 times and
  `keaLCPSolver::solveLCP` 85 times on a real map (`HANDOVER.md` §7d). Six of the twenty
  shipped members are still original and only one of them is on the path
  (`keaMatrix_PcSparse_vanilla`, about one rounding step out). **So "a wasm build cannot run
  on recovered Karma alone" is no longer true on the recovery side** — what is left is
  yours.
- **Nothing has run end to end under wasm.** Natively, the collision layer has: all eight
  objects behind the pairs the game calls were put in the driving seat — no shadow
  harness, the engine consuming the recovered code's answer every frame — through full ONS
  matches. The solver now has too, on a smaller run (§7d). Everything else called
  "validated" was measured by a shadow harness that feeds the engine the ORIGINAL's answer
  every frame, so a recovered error never compounds. Do not assume "validated" means "has
  been run on".

> ### ⚠ AND ONE NEW HAZARD THAT IS SPECIFICALLY YOURS, found 2026-08-25
>
> **There is a class of arithmetic error in the recovered code that is provably harmless on
> i386 and changes results on wasm32.** Ghidra prints right-leaning floating-point `+`
> chains without the parentheses they need, so C re-parses them in a different order. Float
> addition is not associative, so that is a real change.
>
> On i386 it is **exactly inert**: the x87 register carries 64 mantissa bits and a float
> product needs only 48, so these sums come out identical whatever the order — measured **0
> differences in 2,000,000 samples** under `-mfpmath=387`. Under `-mfpmath=sse`, which is
> the same storage-precision arithmetic **wasm32, armv7 and arm64 all use**, the identical
> probe differs in **31%** of samples.
>
> **Every gate on the recovery side is structurally blind to this.** Three instances were
> found and fixed in `keaIntegrate_pc` by reading the shipped machine code; 152 other
> objects have not been examined. `HANDOVER.md` §11 item 2a.
>
> **What this means for you:** if wasm physics diverges from native and you cannot reproduce
> it in any native test, this is the first hypothesis, not the last. It is also the reason
> not to treat "bit-identical on i386" as a promise about wasm. Any A/B you build that
> compares wasm output against native output is measuring something the recovery side
> cannot, and is therefore worth more than it looks.


Reproduce the wasm result in one command:

```bash
cd karma-decomp
./test/wasm_check.sh /tmp/kd_out/allobj /tmp/kd_build ../Thirdparty/metoolkit
```

(after running the recovery once — `HANDOVER.md` §4 — to populate those two directories).

---

## 2. The single most important architectural fact

**The engine and Karma share one address space, in both directions.**

- The engine holds **raw pointers into Karma's heap**. `MdtBodyGetTransformPtr(body)`
  returns a pointer to the live 4×4 transform *inside* Karma's body struct, and the engine
  dereferences it every frame for every body. `McdCTypes.h:305` defines `struct _McdModel`
  in full — the structs are transparent, not opaque handles.
- Karma holds **function pointers into engine code**: `KWorld->bodyDisableCallback`,
  `KAssetFactory->modelCreateFunc`, `MeSetInfoHandler`, the allocator itself
  (`KMemCreate`/`KMemDestroy`/`KMemResize`), and `KTriListGenerator`, which Karma calls
  *during* collision detection to ask the engine for static-mesh triangles.

The boundary is crossed thousands of times per frame with pointers that must be valid on
both sides. This rules out any design where Karma lives somewhere else — a Web Worker with
its own memory, a separate wasm module with its own linear memory, an emulator. **Karma's
memory must be the same Emscripten linear memory as the engine's.**

There is one piece of good luck here: `KMemCreate`/`KMemDestroy`/`KMemResize` already route
all of Karma's allocation through the engine. Point those at the Emscripten allocator and
Karma's heap *is* native wasm memory by construction. That part is already done in
`Source/Engine/Src/KarmaSupport.cpp`.

---

## 3. Why the recovered code should port cleanly — and where it will not

### Pointer size — why i386 and not x86-64

wasm32 has **4-byte pointers**. The engine shares structs with Karma, so the struct layouts
must match what the engine's C++ sees. Only a **32-bit** Karma has the right layout. That is
why the recovery targets the i386 build and not the (also shipped) x86-64 one, whose structs
have 8-byte pointers and different padding throughout.

**Consequence for you:** the recovered C assumes 32-bit pointers in places, because it came
from 32-bit code. wasm32 matches. **wasm64/memory64 does not** — do not enable it without
re-deriving the type database from a 64-bit source.

> ### ⚠ THIS IS NOW MEASURED, NOT ASSERTED — and it is the strongest warning in this file
>
> All 115 objects compile for **arm64** (64-bit pointers) as well as armv7 and wasm32.
> Every one of them compiles. The exported symbol sets are **byte-identical to i386 on all
> three**. Every behavioural gate the project has passes. And arm64 is wrong.
>
> **There is now a gate for it — `test/ptrwidth_check.sh`, 13 seconds, no arm64 hardware
> needed**, because truncation is a compile-time fact. It enables exactly three clang
> diagnostics on top of `-Wno-everything`, so the count is those three and nothing else:
>
> ```
> -Wint-to-pointer-cast        cast to 'T *' from a smaller integer type
> -Wpointer-to-int-cast        cast to a smaller integer type from 'T *'
> -Wvoid-pointer-to-int-cast   ... from 'void *'
> ```
>
> | target | pointer-TRUNCATION diagnostics |
> |---|---:|
> | armv7 (32-bit) | **0** across 0 objects |
> | arm64 (64-bit) | **2,436** across **69 of the 115** objects |
>
> That is decompiled code punning pointers through `undefined4` slots — lossless at 32
> bits, **silently truncating at 64**.
>
> **CORRECTION to what this box used to say.** It read "920 against armv7's 23", and those
> were two different diagnostic SETS added together, measured on five objects. Re-measured
> on the same five: armv7 68 warnings total / 23 pointer-int / **0 truncation**; arm64 968
> total / **the same 23** pointer-int / **897 truncation**. 920 was 897 + 23. The
> conclusion is unchanged and now measured apples-to-apples, corpus-wide.
>
> One trap in re-measuring it, because it reads as good news: `-w` followed by
> `-Wint-to-pointer-cast` reports **zero on both targets** — clang's `-w` wins over a later
> `-W`. Use `-Wno-everything`, which is designed to be overridden.
>
> **What this means for you concretely:**
>
> 1. **wasm32 is safe** and shares armv7's number — zero — not arm64's. Nothing here says
>    wasm32 is at risk. Run `test/ptrwidth_check.sh` yourself before believing that; it
>    takes 13 seconds and it is the cheapest insurance in this document.
> 2. **Never enable memory64.** The "do not enable without re-deriving" above now has a
>    number attached: you would be turning on **2,436** truncation sites across two thirds
>    of the library, and *the build would still be green*.
> 3. **The general lesson, which applies to everything you do here:** identical exported
>    symbols and a clean compile prove nothing about a target you have not executed. That is
>    the same trap `HANDOVER.md` §12 documents three times over, and it caught this project
>    again on 2026-08-24 with a static pointer table whose relocations were never applied —
>    it passed all seven gates and segfaulted on the engine's first asset load.

### Floating point — already settled, do not re-litigate

- `MeReal` is `float`. wasm f32 maps 1:1.
- The original i386 build computes in **x87 with 80-bit intermediates**. The recovered C is
  plain `float`/`double` arithmetic, which is what wasm gives you. This is *correct*, not a
  compromise: MathEngine themselves shipped a pure-f32 build (`lib.rel/linux_hx_single`,
  x86-64/SSE).
- Measured divergence between those two shipped builds: **0.25 mm over 15 s** of
  articulated simulation without collisions, and **unbounded with collisions** (111 m at
  15 s), because contact make/break is discontinuous.
- Therefore **bit-matching the reference was never achievable by anyone**, including
  MathEngine. Do not set that as an acceptance criterion.
- UT2004 already knows this: `Actor.uc` defines `KRigidBodyState`, `KUpdateState()` and
  `bSmoothKarmaStateUpdates=True`. The server replicates rigid-body state and clients
  interpolate toward it. **The game never relied on cross-machine physics determinism**,
  which is why a wasm client diverging from a native server is not a correctness problem.

One thing to watch: `kd_compat.h` maps Ghidra's `longdouble` to **`double`**, deliberately,
because on wasm C `long double` is a software-emulated 128-bit quad and catastrophically
slow. Do not "fix" that to `long double`.

### Compiler flags that are load-bearing

```
-fno-strict-aliasing      REQUIRED. Not a nicety.
-Wno-int-conversion       Ghidra types some pointer-valued locals as undefined4 (4 bytes).
-Wno-incompatible-pointer-types
```

`-fno-strict-aliasing` is not stylistic. Decompiled code type-puns constantly — Ghidra
recovers a stack slot as `MeReal x[2]` and the code stores a pointer through it. Under `-O2`
strict aliasing, GCC is entitled to assume those cannot alias **and deletes the stores**.
That exact bug produced `KTriListGenerator(pair, 0, 0, 0, 0)` and a segfault on the first
call in a live match. Clang under Emscripten will do the same thing.

The `-Wno-int-conversion` allowance is safe *only* because every target has 32-bit pointers
(i386 and wasm32). It is a latent 64-bit bug. If anyone proposes wasm64, this is the first
thing that breaks.

---

## 4. What the recovered code needs from the build

Sources live in `/tmp/kd_out/allobj/*.c` when regenerated (see `HANDOVER.md` §4 for the
command). They include, in this order:

```c
#include "kd_compat.h"   // Ghidra's type/macro vocabulary; the longdouble + ROUND decisions
#include "kd_karma.h"    // umbrella over 108 public metoolkit headers
#include "kd_types.h"    // Karma-internal types recovered from DWARF
#include <stdbool.h>
#include <stdarg.h>
```

Include paths needed:

```
karma-decomp/include
Thirdparty/metoolkit/include
Thirdparty/metoolkit/include/{McdCommon,McdPrimitives,McdFrame,MeGlobals,MdtBcl,MdtKea,Mst,MeApp}
```

Plus `-DLINUX`, which is how `MePrecision.h` selects `MeReal = float` and the pointer
typedefs. Under Emscripten you will need to keep `-DLINUX` (metoolkit has no wasm branch)
and verify `MePrecision.h` picks a sane `MeUintPtr` — it uses `uintptr_t` on the `LINUX`
branch, which is correct for wasm32.

### It compiles. All of it.

This had never been tried, so it was worth doing before anything else:

```
emcc 5.0.7, no -m32, otherwise the same flags as the native build
115 of 115 recovered objects compiled for wasm32.  0 failures.
```

Stronger than that — the **exported symbol names are byte-identical to i386 for
all 115 objects**, nothing added or dropped. So the ABI surface the engine links
against does not change between the two targets, which was the thing most likely
to turn this into a rewrite.

Read "names" literally. `wasm_check.sh` compares `awk '$2 ~ /^[TDBRWV]$/{print $3}'`
— the binding letter in `$2` is used to filter and then thrown away, so a symbol
that is weak on one target and global on the other passes. That is not
hypothetical; see the note at the end of this section.

Re-run it yourself with `test/wasm_check.sh` (§5). It takes seconds and it is the
cheapest early warning that a recovery-side change has broken portability.

Two of the five hazards below are settled by that, and the other three are
runtime questions that compiling cannot answer. **Do not read "it compiles" as
"it works"** — nothing below has been executed under wasm.

### Things that may bite under Emscripten specifically

1. **`alloca` — 12 objects use it**, and the two that matter are `IxSphereTriList` (the
   busiest function in the game) and `IxSphylPrimitives` (the second). The rest are pool
   and kea code. Recovered code uses real `alloca()` for variable-length stack allocations
   (Karma sizes a triangle buffer by triangle count). Emscripten supports it, but the
   default wasm stack is small (64 KB unless `-sSTACK_SIZE` is raised). Karma's alloca is
   `triangleCount * 24` bytes and the count comes from map geometry. **Raise the stack and
   test with dense static meshes.** A stack overflow here will present as memory corruption,
   not a clean trap, unless you build with `-sASSERTIONS` / `-sSTACK_OVERFLOW_CHECK=2`.
2. **`__asm__("symbol")` labels — SETTLED, they work.** The recovered code exports functions
   under their real ELF names via asm labels
   (`void kd_McdBoxBoxIntersect(...) __asm__("McdBoxBoxIntersect")`), because Ghidra
   recovers the parameter types the *code* uses rather than how the public header spells
   them. LLVM honours this: `llvm-nm /tmp/kd_wasm/IxBoxBox.o` shows
   `T McdBoxBoxIntersect`, not `kd_McdBoxBoxIntersect`. The feared large mechanical rename
   is not needed.
3. **C++ ABI data emitted from C — compiles, binding differs.** `gen_vtables.py` re-emits
   `_ZTV*`/`_ZTI*`/`_ZTS*` as C arrays with asm labels and `__attribute__((weak))`, because
   the originals are COMDAT. keaMatrix.o produces all three under both targets, but the
   binding is not the same: i386 gives `V` (weak object, COMDAT-eligible) and wasm32 gives
   plain `W`. That is unlikely to matter while nothing else defines them, and it will
   matter the moment something does. **Check it at link time, not compile time**, and only
   `libMdtKea` is affected.
4. **Function pointers and the wasm table.** Karma stores engine callbacks as function
   pointers and calls them indirectly. In wasm those become table indices with **strict
   signature matching** — a mismatch is a runtime trap, not a silently-wrong call as on
   x86. This is arguably a *benefit* (it will catch signature errors the native build
   tolerates) but expect trapping where native "worked".
5. **No `-m32`.** Every command in the existing docs passes `-m32`. Drop it for wasm; wasm32
   is already 32-bit.
6. **`-sALLOW_MEMORY_GROWTH` and the pointers §2 is about.** Karma's heap *is* the engine's
   linear memory, and the engine holds raw pointers into Karma structs
   (`MdtBodyGetTransformPtr` returns a pointer to the live transform and the engine
   dereferences it every frame). Growth does not invalidate those — a wasm pointer is an
   offset, and offsets survive a grow. **What it does invalidate is every JS-side
   `HEAPF32`/`HEAPU8` view**, because the underlying `ArrayBuffer` is detached and
   replaced. Any glue that caches a typed array across a frame boundary is a
   use-after-detach waiting to happen. Re-acquire views after anything that can allocate,
   or size the heap up front and turn growth off.
7. **Do not enable `-sMEMORY64`.** The recovered structs are 32-bit-pointer layouts by
   construction (§3), and the engine's C++ shares those structs. memory64 changes pointer
   size and every offset in the type database is then wrong. This is not a flag to
   experiment with.
8. **Threads change the answer to §2, not just the performance.** Karma is single-threaded
   within a world step and `kd_compat.h` maps the x86 `LOCK` prefix to a no-op on that
   basis. If you ever put the physics step on a worker, that assumption has to be revisited
   *and* the shared-memory design in §2 becomes a `SharedArrayBuffer` question with COOP/COEP
   headers attached. It is a much larger change than it looks; the current design deliberately
   keeps Karma on the main heap.
9. **Exceptions and `longjmp`.** The recovered C has neither, but it is linked beside UE2.5
   C++ which does. If the engine build uses `-fwasm-exceptions` and the Karma objects are
   compiled without, the link is still fine — they contain no landing pads — but
   `__gxx_personality_v0` appears as an undefined symbol in a few shipped objects
   (`McdGjkRegistration`'s original has it). The recovered ones do not; if you see it,
   something pulled in a shipped `.a`.

---

## 4a. How to iterate on this project without breaking it

You will need to change the recovered C. **Do not edit `/tmp/kd_out/allobj/*.c`.** Those
files are *generated*, and the next `recover.py` run overwrites them. Everything is
regenerated from the Ghidra dumps in about a minute, so the loop is:

```bash
cd karma-decomp
# 1. change the GENERATOR (tools/ghidra_clean.py, tools/gen_*.py), not the output
rm -rf /tmp/kd_out /tmp/kd_build
python3 tools/recover.py --dump-dir /home/ion/tools/karma-lab/out5 \
  --obj-dir /home/ion/tools/karma-lab/allobj --out-dir /tmp/kd_out \
  --metoolkit ../Thirdparty/metoolkit --protos /home/ion/tools/karma-lab/kd_protos.h
# 2. check the BLAST RADIUS before checking correctness
diff -rq /tmp/kd_out.baseline/allobj /tmp/kd_out/allobj
# 3. run every gate (§5). wasm_check.sh included, every time.
```

Three rules that were each learned the hard way and are cheap to keep:

- **Snapshot before you change the generator** (`cp -r /tmp/kd_out /tmp/kd_out.baseline`) and
  diff afterwards. A one-line regex change is supposed to touch three objects; if it touches
  thirty, stop. This caught a change that silently regressed an unrelated object twice.
- **`recover.py`'s classification table is the regression test.** If the counts move
  (89 ok / 4 todo / 16 review / 39 fail today), you changed something you did not mean to.
- **A change that makes an object compile is not progress.** Eight detectors deliberately
  hold objects OUT of the validated set because compiling-and-wrong is worse than
  not-compiling. Do not remove one to raise a number; the release path is a line of evidence
  in `proven.txt`, nothing else.

If a construct turns out to be unportable, **fix the generator, not the file.** That is why
the whole set regenerates in a minute and why `wasm_check.sh` exists as a one-command check
that a generator change kept portability.

---

## 4b. Web-specific hazards learned the hard way

These are recent and none of them is in any older document.

### Unprototyped indirect calls — x86 corrupts silently, wasm will TRAP

`kd_compat.h` has `typedef int code();` — a function type with **no parameter list**. Ghidra
uses `code *` for every call through a function pointer, and a call through it is
*unprototyped*, so C applies the default argument promotions. A `float` argument is pushed
as an 8-byte `double` and the callee reads the wrong half.

This was live in all four TriangleList interactions: the bounding-sphere radius Karma passes
to the engine's `KTriListGenerator` arrived as `2048`, `8.796e+12`, `6.019e-36` and `-0`
instead of ~1. On x86 that silently queried the wrong part of the level. **On wasm the same
construct is a table-index signature mismatch, which traps.**

Fixed in `ghidra_clean.prototype_indirect_calls()`, which synthesises a prototype from the
casts Ghidra already wrote on the arguments. Two consequences for you:

- Good news: wasm's strict signature matching is a **gift**. It converts this whole bug class
  from "silently wrong physics" into "immediate trap with a stack trace". Take it. Do not
  work around a trap here — it is telling you a signature is wrong.
- If you hit such a trap, the fix is almost certainly in the generator, and the pattern to
  copy already exists.

**A SECOND instance of exactly this was found on 2026-08-24, and it says the class is not
closed.** The first was in the generated C (`typedef int code();`). The second was in the
TYPE DATABASE: `dwarf_structs.declarator()` rendered every function type with an empty
parameter list, so `kd_types.h` defined the broadphase's AABB-update callback as

```c
typedef void (*McdUpdateAABBFnPtr)();      /* no prototype */
```

and `CxSmallSort` calls it with a `MeReal`. The recovered object emitted `flds` then
**`fstpl`** — storing the float as an 8-byte double — against a callee taking a float. The
shipped `CxSmallSort.o` contains zero `fstpl`. Fixed, and `grep -c 'void (\*)()'
karma-decomp/include/kd_types.h` should stay at **0** — that is the tripwire.

Why you should care more than the recovery side does: on x86 this is wrong physics in the
broadphase; **on wasm it is a trap**, and it is in a code path none of the three offline
scenes reaches. If your wasm driver traps somewhere that looks unrelated to what you were
testing, check the signature of every function pointer on the path before anything else.

### Out-of-range frame writes — harmless on one stack layout, corruption on another

`tools/check_frame_bounds.py` reports every `(&)?NAME + K` where K falls outside the local
`NAME` declares. Three existed until recently, from collapsing an alloca frame shift onto a
negative offset — writes 28 and 32 bytes *below* an 8-byte array.

The reason this matters to you specifically: those writes were **completely invisible**
behaviourally. The cast launders the bounds so the compiler says nothing, the scene tests
passed, and 200,000 synthetic pairs produced *byte-identical* results with and without the
defect, because GCC happened to leave those eight bytes free. **A different compiler and a
different stack layout — which is exactly what you are — is where a latent one turns into
corruption.** Run `check_frame_bounds.py` after any generator change; it is a one-second
text check and there is no runtime equivalent.

### `alloca`, with numbers this time

Hazard 1 in §4 said "raise the stack". Here is what to raise it to.

`McdConvexMeshTriangleListIntersect` allocas `triangleMaxCount * 24` bytes, and the engine
sets `triangleMaxCount` to **400** (every in-game divergence dump reads `maxCount 400`). That
is **9,600 bytes on the stack per call**, held for the whole call, with
`GenerateTriangleContact`'s own `hull->numVertex * 12` alloca nesting inside it. The
Sphere/Sphyl/Box TriangleList intersects do the same thing.

Default Emscripten stack is 64 KB. Karma is called from inside the engine's own call stack,
which is already deep. **Budget at least `-sSTACK_SIZE=1MB` and build the first wasm runs
with `-sASSERTIONS -sSTACK_OVERFLOW_CHECK=2`**, because without them a stack overflow here
presents as memory corruption rather than a clean trap.

### Contact buffers are 400, not 64

The engine hands these functions a 400-contact result buffer. Anything you write that calls
them — a replay driver, a scene harness — must match. `difftest_pair.c` used 64 for the
project's whole life and only survived because its test bodies were small; the moment they
were allowed to vary in size it died with "stack smashing detected" and *no output*, having
discarded everything printed before it to a redirected stdout. If a wasm harness of yours
dies with no output, suspect a buffer before you suspect wasm.

### A portability bug the native build cannot see: glibc-only names

`McdPolygonIntersection` compiled for i386 and **not** for wasm32, on
`use of undeclared identifier '__compar_fn_t'` — glibc's name for `qsort`'s comparator,
which Ghidra emits because that is what the DWARF calls it. Emscripten's libc does not have
it. Fixed in `kd_compat.h` behind glibc's own `__COMPAR_FN_T` guard.

This is the first time `test/wasm_check.sh` has caught something the native build could
not, and it is exactly what that gate is for. **Run it after every generator change.** Any
glibc-internal spelling — `__ctype_b_loc`, `__strtol_internal`, `_IO_putc`,
`__compar_fn_t` — is a candidate for the same treatment; `kd_compat.h` already maps several
back to portable equivalents and that list is the pattern to extend.

### The three shapes of indirect call, and why wasm is unforgiving about all of them

This is the single class of defect that has produced the most real bugs in this project,
and **wasm is where it stops being survivable.** On x86 a call through a wrong-arity
pointer corrupts quietly under caller-cleanup cdecl; wasm32 type-checks every indirect call
against its table signature and **traps**. So every one of these had to be fixed at the
source, and if you see a new one, fix it the same way rather than papering over it:

| shape | what Ghidra did | fix |
|---|---|---|
| C++ virtual call through a local's vptr | dropped **every argument** | `gen_vtable_callsites.py`, 19 sites, §5a of `HANDOVER.md` |
| call through a global API struct (`MeMemoryAPI.create`) | dropped the **size argument** — gcc reuses one `(%esp)` slot with `movl` instead of `push` | same tool, **412 sites** |
| call through a `void *` struct member whose signature varies by a discriminator | not callable in C at all | cast via `__typeof__` of the actual arguments, never a hand-written signature |

That last row is the one to internalise. `MeXMLHandler.fn` is deliberately `void *` because
it is either an `MeXMLCallback` or an `MeXMLParseFn` depending on `->type`. The cast is
built from `__typeof__` of the argument expressions **precisely so a float stays a float** —
writing a plausible signature by hand is how the `-0` radius bug survived three sessions.

### Data whose value is not in the bytes

Two cases you will meet if you regenerate anything, and both look like the code is fine:

- **A relocated `.rodata` word.** Its value lives in the relocation record; the section
  holds a zero. Karma's XML handler tables are entirely this. `materialise_data_refs`
  REFUSES relocated sections for that reason, and `materialise_relocated_data` is the
  sibling that applies the relocations. If you ever see a table of null function pointers,
  this is why.
- **A zero-initialised static.** It carries no evidence of its own type, so guessing from
  the bytes gives `static float x = 0.0f;` for what is really a function-pointer hook
  installed at run time. The object's own DWARF is the authority.

### Test stubs must depend on their arguments

The single most expensive lesson of this project. `difftest_pair`'s triangle generator
ignored `pos` and `radius` and set triangle `flags = 0`. Consequences: a completely wrong
radius was undetectable for months, and the entire edge-contact half of one interaction had
**never executed in any test**. If you write a wasm-side stub for `KTriListGenerator` or the
allocator, make it *use* what it is given, and add the equivalent of `KD_GENARGS=1` — a mode
that compares what each side passed the callback, rather than only what came back.

---

## 5. Validation — use what exists, do not invent your own

**Five** gates already exist and all of them work. Reuse them rather than eyeballing
physics.

1. **`test/substitute_test.sh`** — swaps one recovered object into the link, runs a scripted
   scene, compares the **trajectory** against baseline. On a collision-free scene a correct
   recovery is *bit-identical*; anything else is a bug. There are three scenes and you want
   all of them: `scene_chain.c` (collision-free, the authoritative trajectory signal),
   `scene_boxes_on_plane.c` (exercises the geometry dispatch), `scene_ragdoll.c` (nine
   capsules on ball-socket joints — the other two make **not one Sphyl call** between them).
   Currently **115/115 clean on all three** on i386 — but read `HANDOVER.md` §4a before
   putting weight on that number: only eight of 103 objects have measurable sensitivity on
   any of these scenes, and `test/scene_census.sh` and `test/gate_sensitivity.sh` exist to
   say which. **Getting that under a wasm build is your first milestone.**
2. **`test/difftest_pair.sh`** — drives one interaction directly over randomised transforms,
   seeded so anything it finds reproduces. **Twelve pairs wired up — every one the game
   calls.** Needs no solver and no engine, which makes it the natural basis for your first
   wasm driver (§8 step 1). Six switches; read `HANDOVER.md` §4 on `KD_SPREAD` before
   quoting any number from it, because divergence rates here are a strong function of
   contact regime and a figure without its regime is meaningless. `KD_GENARGS=1` and
   `KD_FIXEDSHAPE=1` are the two that found real bugs most recently.
3. **`test/wasm_check.sh`** — compiles the whole set for wasm32 and diffs the exported
   symbols against the native build. Currently 115/115. **Run this after any change
   to the recovery pipeline**; it is the cheapest possible early warning that a change has
   broken portability. It compares NAMES only — see the note at the end of §4.
4. **`test/kd_shadow.c`** — the in-game shadow harness. Runs both implementations on the
   same inputs and compares. Structural fields (return value, contact count, contact
   dimensionality, buffer overrun) must match **exactly**; float deltas are expected.
5. **`KD_SELFTEST=1`** — runs the *original* as both sides. Any divergence it reports is a
   harness bug, not a recovery bug. **Run it before believing any divergence you see, and
   before believing any crash** — a SIGSEGV was misattributed to recovered code for half a
   day because this was skipped, and it reproduces with no recovered code executing.

7. **`test/ptrwidth_check.sh <out>/allobj <build>`** — the pointer-width gate. armv7
   must read **0**; arm64 currently reads **2,436 across 69 of 115**. This is the only
   gate in the project that can see a defect on a target nobody has executed, and it is
   the one that matters most to you. 13 seconds.

One warning that will bite you if you reuse the shadow harness naively: **it perturbs the
engine.** Running an intersection function a second time is only free if it writes solely
through its output parameter, and `McdModelPair` carries `m_cachedData`, `responseData` and
`phase` for it to write to — and `m_cachedData` is a *pointer*, so copying the struct does
not isolate it. `McdGjkCgIntersect` warm-starts from that block, so for months both
implementations were stepping the same cache and feeding each other across frames.

That is now fixed (the copy is the first argument, and the cache block is rewound and
restored around the second call), and it took the `KHandleCollisions` crash from 5 of 5 runs
to 1 of 3, and GJK's structural divergences from 18 to 5. **Both reduced, neither
explained.** `KD_SHARECACHE=1` puts the old behaviour back for A/B. `KD_CENSUS=1` turns the
second call off entirely and perturbs nothing measurable — use it when you want call counts
from a build you do not want to disturb.

The general lesson, which applies directly to anything you build on the wasm side: **a
harness that shares mutable state with the thing it measures will lie to you, quietly, for
as long as you let it.**

For wasm you cannot run the shadow harness against a native original in-process. Two options,
in order of preference:

- **Record-and-replay.** Run the native shadow harness, dump inputs and outputs for N calls,
  then replay the same inputs through the wasm build and compare. The harness already dumps
  full input transforms on divergence; extend it to dump *all* calls behind a flag.
- **Trajectory comparison at the scene level** using `scene_chain.c` (collision-free, so
  divergence is not amplified) compiled for both native and wasm. Expect float-level
  differences, not bit-identity, because the arithmetic differs (x87 vs f32).

**Do not use the collision scene for trajectory comparison.** Past first contact, any two
builds diverge without bound. That is physics, not a bug.

**And do not demand bit-identity of your wasm build against anything.** The yardstick that
matters is what the vendor already tolerated. On the ragdoll scene, MathEngine's own two
shipped builds of their own source — i386/x87 and x86-64/SSE — diverge by **3.283 m over
15 seconds**. The recovered code against the shipped i386 build diverges by 3.677 m on the
same scene. If your wasm build lands in that neighbourhood and the divergence is *bounded*
(it plateaus in the first three seconds rather than growing) with the same residual energy,
that is a pass. That comparison is the single most useful calibration in this project and
`scene_ragdoll.c` reproduces it in seconds:

```bash
# native i386 vs the vendor's own x86-64 build — your reference for "how much is normal"
gcc -m32 ... -o /tmp/rag_i386 test/scene_ragdoll.c  <linux_single_gcc3.2/*.a>
gcc -m64 ... -o /tmp/rag_hx   test/scene_ragdoll.c  <linux_hx_single/*.a>
```

---

## 6. What is not done, and what you should not assume

- **Nothing has been RUN under wasm.** The whole set compiles and exports identical
  symbols, and that is all §4 hazards 2 and 3 settle. Hazards 1, 4 and 5 are runtime and
  entirely open.
- **115 of ~150 objects compile**, 17 do not. But read `HANDOVER.md` §3b then §3 before reading that
  as 60% done — see the next bullet, it is the most important thing in this file for
  planning purposes.
- **25 objects are deliberately quarantined** by eight safety detectors. They compile but
  are known-or-suspected wrong. Do not include them to raise a coverage number.
- **AND THE QUARANTINE IS NOT A GUARANTEE, which is new on 2026-08-24.** It only holds
  objects some *detector* recognises. `IxCylinderCylinder` is measured wrong on a live pair
  — 925 structural divergences in 24,111 real calls — and **no detector objects, so it is
  in `/tmp/kd_build` and in any build you make from it.** "Not quarantined" means "nothing
  automated complained", not "validated". The list of what is actually validated is
  `proven.txt`, and nothing else. The proof
  of why: `IxConvexTriList` compiled, passed all three substitute scenes and 200,000
  synthetic pairs, and was **46% wrong in a live match** for two sessions — returning *no
  contacts* where the original returned three. It is fixed and released now, but it was
  only ever the quarantine that kept it out of the validated set.
- **`libMcdConvexCreateHull` is qhull 2.6 (1998)** — 186 KB, open source, load-time only.
  **Do not recover it; replace it** with modern qhull, which builds under Emscripten without
  drama. Better still, precompute convex hulls offline and ship them as data — that matches
  this project's "engine differences are DATA, not code" rule.
- **`MeAssetDB`/`MeXML`/`MeAssetFactory`** (51 KB) is `.ka` XML asset loading, not physics.
  Rewrite or bypass rather than recover.
- **`MeViewer2`/`MeApp`** (74 KB) are never linked by UT2004. Skip entirely.

That last three points remove ~36% of the total binary footprint without recovering a line.

### The number that should drive your planning

**Not the object count. `tools/dropin_gap.py`.**

```bash
python3 tools/dropin_gap.py <engine-build-dir> /tmp/kd_build \
        /home/ion/tools/karma-lab/allobj --status <recover.py output>
```

It walks the symbol closure from the ENGINE's own object files, resolving each symbol
against the recovered build first and only then against the shipped archives, and prints
**every shipped member the engine still needs**. That is the distance to the deliverable.
**8 members, 35 symbols** today — 25 of them in members deliberately not attempted — from 20/148 one session ago and 27/192 the week before.
It is checked against ground
truth rather than trusted: a real link of the engine with every shipped member deleted
reports 111 undefined symbols, and the walk predicts all 111.

`HANDOVER.md` §3c has the table and groups the 20 by cause.

**And the scope is smaller than "Karma" suggests, in two ways worth internalising:**

- **37 collision pairs are registered; UT2004 calls FIFTEEN.** Thirteen are validated
  against the real game. Two families — `Box × TriangleList` and every `Aggregate` PAIR —
  **can never be called**, proven from the engine source: `KIntersect` intercepts them
  before Karma is consulted. The rest have simply never been seen in 30+ runs across 20+
  maps, which is "no evidence of use", not "impossible" — the list has moved five times.
- **31 objects are unreachable from the engine at all** and are retired permanently
  (`HANDOVER.md` §3b) — MathEngine's demo viewer, its unused constraint types, its ASE
  loader. They will never be recovered and that is correct.

So "how much of Karma does the web build need" is not 153 objects and not 137. It is the
137 already recovered plus the 8 in the gap, and the gap is the only number that moves.

### The gap that decides your schedule — REWRITTEN, the old blocker is GONE

**Every version of this section before 2026-08-25 said the solver's control flow was not
recovered and that you could not reach "the engine runs on recovered physics" by yourself.
That is no longer true.** `libMdtKea` is recovered whole: every object in it reproduces the
shipped library bit-for-bit on all three test scenes, and the engine has been rebuilt
against them and RUN — the recovered `MdtKeaAddConstraintForces` executes 301 times and
`keaLCPSolver::solveLCP` 85 times on a real map, with the substitution verified at the
machine-code level, not from the link (`HANDOVER.md` §7d).

**So the recovery side no longer blocks you. What blocks the project now is on YOUR side.**

| | |
|---|---|
| **the deliverable** | UT2004 linking NO shipped `metoolkit` member, on wasm32 and Android, and playing |
| **recovery-side remainder** | 8 shipped members / 35 symbols, tracked by `tools/dropin_gap.py` (`HANDOVER.md` §3c). Shrinking steadily — 19 members and 113 symbols closed in the sixth session, which cut it by 76% |
| **YOUR remainder** | **nothing has ever EXECUTED on wasm32, armv7 or arm64.** Not one instruction. That is the single largest unknown in the whole project |

**Sequence your work as if the physics is coming, because it is.** The right thing to
front-load is not "wait for the solver" — it is **getting anything at all to run under
wasm**, because every hazard in §4b is unmeasured until something executes, and because of
the next item, which is new and is specifically yours.

### ⚠ THE HAZARD THAT ONLY YOU CAN SEE — and it is measured

**There is a class of arithmetic error in the recovered code that is PROVABLY HARMLESS on
i386 and changes results on wasm32, armv7 and arm64.**

Ghidra prints right-leaning floating-point `+` chains without the parentheses they need, so
C re-parses them in a different order. Float addition is not associative, so that is a real
change of program.

- On i386 it is **exactly inert**: the x87 register carries 64 mantissa bits and a float
  product needs only 48, so these sums come out identical whatever the order — **measured 0
  differences in 2,000,000 samples** under `-mfpmath=387`.
- Under `-mfpmath=sse`, which is the same storage-precision arithmetic **wasm32, armv7 and
  arm64 all use**, the identical probe differs in **31%** of samples.

**Every gate on the recovery side is structurally blind to it by construction.** Three
instances were found and fixed in one object (`keaIntegrate_pc`) by reading the shipped
machine code against Ghidra's output; **152 other objects have not been examined**, and a
corpus-wide rewrite cannot be validated on i386 — reversing all 41 flat chains in a second
object changed the object file and changed nothing measurable.

**What this means for you, concretely:**

1. If wasm physics diverges from native and you cannot reproduce it in ANY native test,
   this is the first hypothesis, not the last. `HANDOVER.md` §11 item 2a.
2. **Do not treat "bit-identical on i386" as a promise about wasm.** It is not one, and now
   there is a measured reason why.
3. **A wasm-vs-native A/B is worth more than it looks.** It can see something the entire
   recovery-side gate set cannot. If you build one thing beyond bring-up, build that.

### Android is half the deliverable, and it is in worse shape than wasm

The Android NDK is installed and `test/ptrwidth_check.sh` is a 13-second gate that needs no
device — pointer truncation is a compile-time diagnostic.

| target | compiles | symbols | truncations |
|---|---|---|---|
| wasm32 | 137/137 | identical to i386 | — (32-bit) |
| **armv7** | 137/137 | identical | **0** — a real 32-bit-pointer port |
| **arm64** | 137/137 | identical | **7,457 across 87 of 137 objects** |

**arm64 compiling is a lie.** The recovery puns pointers through 4-byte slots, which is
sound on every 32-bit target and silently truncates on a 64-bit one. The fix is
generator-wide, on the recovery side — but **decide early whether arm64 is a target at
all**, because armv7 works today and a 32-bit Android build may be the whole answer.

### One thing that changed on your side of the fence

`test/wasm_check.sh` compares each object's exported symbols between the wasm32 and i386
builds, and it discards the binding letter — it compares names only. That is how a
recovered object exporting a **global `putchar`** (weak in the shipped library, so that
libc's wins) passed every gate for months. The recovery side now has
`tools/check_symbol_bindings.py`, which compares the recovered object against the
**shipped** one including binding and size.

`wasm_check.sh` itself is unchanged and still name-only. If you extend it, comparing
bindings between the two targets is worth doing: weak/COMDAT handling is exactly the area
where `wasm-ld` and GNU `ld` differ, and §4b already flags COMDAT for `keaMatrix.o`.


The honest one-line summary of the project's state: *the collision layer and the solver are
both recovered and both run inside the real engine on i386; 8 shipped members remain, and
nothing has ever executed on any of your three targets.* Do not read 137-of-153 as 90% —
read `tools/dropin_gap.py`.

---

## 7. Legal note, because it affects where output can go

Karma is proprietary MathEngine code (now CM Labs lineage). Decompiling and redistributing a
derived *source* form is a materially different posture from linking the shipped binaries —
and this engine repo feeds a **public** monorepo. The existing work keeps recovered sources
on a private branch. Confirm the intended distribution before any of it lands somewhere
public; the prior discussion floated a separate-repo split (the `NFSIISE` model) precisely
so it can be taken down independently.

---

## 8. Suggested order of work

Compilation is no longer step one — that is done. **And neither is waiting: the solver is
recovered, so steps 1 through 3 are ALL runnable today.** Every earlier version of this
section told you step 3 was blocked on `libMdtKea`; it is not, as of 2026-08-25.

**Read §6's two boxes before you plan** — the association hazard (which only your targets
can see) and the arm64 pointer-truncation table. They change what is worth doing first.

1. **Prove the recovered collision functions EXECUTE under wasm, in isolation.** Write a
   standalone wasm driver that calls one interaction function directly with hand-built
   geometry and transforms — the same shape as `test/difftest_pair.c`, which already does
   exactly this natively and needs no solver, no `MstUniverse` stepping, no engine. Run it
   under node. This is the smallest thing that crosses the "never executed" line, and it is
   available today.
   - Reuse `difftest_pair.c`'s geometry factories and its seeded RNG so the *inputs* are
     identical to the native run.
   - You cannot run both implementations in one process (the original is x86), so use
     **record-and-replay**: dump inputs and outputs from the native run, replay the inputs
     through wasm, compare. §5 has the shape of it.
2. **Hunt the §4b hazards deliberately, in that driver**, before anything larger. In order of
   how hard they are to debug later: `alloca` stack sizing (§4b has real numbers — budget
   `-sSTACK_SIZE=1MB`), then function-pointer signature traps, then anything
   `check_frame_bounds.py` has to say. Everything you find here goes back to the generator
   and is then fixed for the solver too, before it is written.
3. **Then `scene_chain.c` and `scene_ragdoll.c` under wasm — AVAILABLE NOW.** These step a
   full `MstUniverse` and so need the solver, which is why every earlier version of this
   file deferred them. The solver is recovered, so they run today, and **they are the
   sharpest instrument you have**: `scene_chain` over 900 steps is bit-identical on i386
   between recovered and shipped, so ANY divergence you see under wasm is a wasm-side fact —
   either a portability bug or §6's association hazard. That is a much stronger signal than
   step 1's record-and-replay.
   `scene_chain` is collision-free and is the authoritative trajectory signal;
   `scene_ragdoll` is the one that calibrates against the vendor-vs-vendor divergence in §5,
   which is the single most useful number in this project for deciding whether a wasm
   result is acceptable.
4. **Only then** wire into the engine's wasm build behind `WITH_KARMA`. The crux: you cannot
   fall back to the shipped `.a` for what is not yet recovered, because it is x86. Use the
   census (§6) to scope what "enough" means — it is twelve collision pairs, not 150 objects.
5. **Feed everything back to the recovery side, continuously.** If a construct is unportable
   it is almost always cheaper to fix the *generator* than to patch the output — §4a. The
   whole pipeline regenerates in about a minute and `test/wasm_check.sh` tells you in one
   command whether a generator change kept portability. This is the highest-leverage thing
   you can do while the solver is still being recovered.

Read `karma-decomp/HANDOVER.md` for how the recovery pipeline works, and
`docs/KARMA-ON-WASM.md` Part I §2 for the full architectural analysis behind §2 above.

---

## 9. What changed on the recovery side, and what it means for you

A log of the things that would otherwise surprise you, newest first. If you have read an
older copy of this file, start here.

### 2026-08-26 (sixth session) — 137 objects, and TWO OF YOUR TARGETS WERE NOT COMPILING

- **READ THIS ONE FIRST, because it is about your side.** `test/ptrwidth_check.sh` counts
  `warning:` lines, and a file clang REJECTS emits none — so armv7 reported **"0 truncation
  warnings across 0 objects" while two objects were failing outright**, and wasm32's
  "122/122" in the previous entry was really **121 of 122**. The two are `MeDict`
  (`unknown type name 'ulong'`) and `MeSimpleFile_linux` (`use of undeclared identifier
  '__off_t'`) — glibc spellings bionic and emscripten do not extend. Both are now
  typedef'd in `karma-decomp/include/kd_compat.h`. **If you have an older wasm artefact,
  it is missing `MeSimpleFile_linux`.**
- **137 objects**, drop-in gap **8 members / 35 symbols** — cut by 76% in one session, and
  25 of the 35 are in members deliberately not attempted. wasm32 **137/137** with
  byte-identical exported symbol sets, armv7 **137/137 with 0** truncations, arm64 **7,457
  across 87 of 137** (up from 2,680/73, because `MdtBcl` alone adds 3,242).
- **A COMPAT SHIM THAT IS A STAND-IN ON YOUR TARGETS.** `rdtsc` is an x86 INSTRUCTION, not
  a function; off x86 `kd_compat.h` supplies a monotonic counter instead of a cycle count.
  It feeds `MeProfileTimerResult.cpuCycles` and nothing else, and the whole profiler chain
  is measured at ZERO calls in a real match — `MstUniverseStep` is a sixth member of the
  §3a family, code UT2004 links and reimplements. So it cannot affect physics, but if you
  ever read Karma's profiler output on wasm, the units are wrong by design.
- **The asset loader is now recovered end to end and the engine runs on it.**
  `MeFAsset` (38 symbols), `MeFAssetPart` (21), `MeAssetDBXMLInput_1_0` and
  `MeAssetDBXMLOutput_1_0`, `MeAssetFactory` and `MeXMLParser` are all in the build — the
  whole `.ka` loading path. That matters to you because it is what loads vehicles and
  ragdolls, the reason this project exists. And `MeXMLParser` is MEASURED rather than
  argued: the engine executes `MeXMLElementProcess` 1,462 times and `MeXMLParseMeReal`
  2,167 times on a real map.
- **A defect that only the real engine found, for the third time.** A table of function
  pointers came out addressing a *copy of `.text`* rather than the code, passed all nine
  offline gates, and segfaulted in `MeXMLElementProcess` before a match started. If you see
  a fault inside XML parsing on wasm, this is the shape to suspect first.
- **New instrument you can borrow: `recover.prove_inert`.** It decides whether a value the
  decompiler could not account for actually reaches anything, by compiling the object with
  that value set to three different constants and diffing the object files. It is
  target-agnostic and would work as well under emscripten as under gcc.

### 2026-08-25 (fifth session, last) — 122 objects, and this file was stale in one big way

- **The section that told you the solver blocks you is WRONG and has been rewritten.**
  `libMdtKea` is recovered whole and the engine runs on it. **§8 steps 1–3 are all runnable
  today**, and step 3 — `scene_chain` under wasm — is the sharpest instrument you have,
  because it is bit-identical on i386 over 900 steps, so any divergence under wasm is a
  wasm-side fact.
- **122 objects**, drop-in gap **20 members / 148 symbols**. arm64 **2,680 truncations
  across 73 of 122**; armv7 **0**.
- **Four objects that already compiled were silently WRONG and are now fixed** —
  `McdSphere` computed a sphere's mass properties from its radius read as an integer;
  `McdCylinder`, `McdConvexMesh` and `McdPlaneIntersect` had the same defect. **Any wasm
  build you made before today contains them.** Rebuild.
- **`MeAssetDBXMLIO` passed all nine offline gates and killed engine init**, calling through
  a null in a relocated handler table on the `.ka` path — the path that instances every
  ragdoll and vehicle. Second time that exact defect has got that far. You have no
  equivalent of "run the engine and watch it die at init" on wasm; **building one is worth
  more than it looks.**

### 2026-08-25 (fifth session, later) — 121 objects, and a metric for the finish line

- **`tools/dropin_gap.py` counts what is actually left**: the shipped members the engine
  still needs, resolved symbol by symbol against the recovered build. **20 members, 148
  symbols**, down from 27/192 the same day. Checked against a real link of the engine with
  every shipped member deleted — 111 undefined symbols, all 111 predicted. `HANDOVER.md`
  §3c. Use it rather than the object count when you need to know how close this is.
- **122 objects** (was 115). wasm32 **122/122** with identical exported symbol sets, armv7
  **0** truncations, arm64 **2,680 across 73 of 122**.
- **Four objects that already compiled were silently WRONG**, and one of them is on your
  path: `McdSphere` computed a sphere's mass properties from its radius read as an integer,
  and `McdCylinder`, `McdConvexMesh` and `McdPlaneIntersect` had the same defect. If you
  have a wasm build from before this, it has them.
- **And a warning that applies to you more than to anyone.** `MeAssetDBXMLIO` passed every
  offline gate and then killed the engine during `KCreateAssetDB`, calling through a null
  in a relocated handler table. That is the `.ka` asset path — the one that instances every
  ragdoll and vehicle — and it is the SECOND time this exact defect has shipped past all
  nine gates. On wasm you have no equivalent of "run the engine and watch it die at init"
  yet; building one is worth more than it looks.

### 2026-08-25 (fifth session) — the solver blocker is GONE, and a hazard that is yours

**Two things here matter to you and one of them is new work.**

- **`libMdtKea` is recovered whole.** `keaIntegrate_pc` is bit-identical on all three scenes
  and over 450,000 bodies of a per-function A/B; `keaLCP_new` is released. The engine was
  rebuilt against them and RUN — recovered `MdtKeaAddConstraintForces` 301 calls,
  `keaLCPSolver::solveLCP` 85 calls on a real map (`HANDOVER.md` §7d). **The line in §1 that
  said a wasm build cannot run on recovered Karma alone is withdrawn.**
- **⚠ A NEW HAZARD, AND IT IS SPECIFIC TO YOUR TARGETS.** Ghidra prints right-leaning float
  `+` chains without the parentheses they need. On i386 that is **exactly inert** — 0
  differences in 2,000,000 samples under `-mfpmath=387`, because x87 carries 64 mantissa
  bits and a float product needs 48. Under `-mfpmath=sse`, i.e. the arithmetic wasm32,
  armv7 and arm64 all use, the same probe differs in **31%**. Three instances were fixed in
  one object; **152 objects are unexamined and no recovery-side gate can see this.** §1 has
  the full note; `HANDOVER.md` §11 item 2a has the evidence. If wasm physics ever diverges
  from native in a way no native test reproduces, start here.
- **115 objects** (was 113), wasm32 **115/115** with identical exported symbol sets, armv7
  **0** truncations, arm64 **2,436 across 69 of 115** (up from 2,291/68 — `keaLCP_new`
  contributes 145 of the new sites). **Superseded later the same day — see the entry above.**
- The dump directory is still **`out13`**.

The web workstream's job is still item 4 in WHAT REMAINS: nothing has EXECUTED on wasm32,
armv7 or arm64 — and the hazard above makes the first wasm-vs-native comparison worth more
than it used to be, because it can see something the recovery side cannot.

### 2026-08-25 (fourth session) — the solver is recovered; the dump is out13

Nothing here changes what the web build does, and the object count does not move.

- **`keaLCP_new` — the last `libMdtKea` module — is bit-identical to the shipped library on
  all three test scenes.** It is NOT in the build: a detector still holds it, for a reason
  `proven.txt` shows to be inert. Everything in the solver is now either exact or, in one
  case, out by 1.5e-08.
- **The dump directory is now `out13`, not `out12`** — one of 153 dumps changed. If you
  re-run the recovery, `HANDOVER.md` §4's command has been updated to match.
- 113 objects, wasm32 **113/113** with identical exported symbol sets, armv7 **0**
  truncations, arm64 **2,291 across 68** — all unchanged from the last entry.

The web workstream's job is still item 4 in WHAT REMAINS: nothing has EXECUTED on wasm32,
armv7 or arm64.

### 2026-08-25 (third session) — 113 objects, and the solver is one module away

Nothing here changes what the web build does; both objects that moved are solver-side.

- **`keaLCPSolver` is back IN the build and is bit-identical to the shipped library on all
  three test scenes** — and on all fifteen of its functions taken one at a time. It had been
  pulled the same day as measurably wrong; the defect was three lines.
- **113 objects, wasm32 113/113** with identical exported symbol sets. arm64 is back to
  **2,291 across 68 of 113** — the same figure as before keaLCPSolver was pulled, because it
  is the same object returning. armv7 remains **0**.
- The dump directory is still **`out12`**.

Nothing about the web build's blockers has changed: it still has no Karma at all, and item 4
in `HANDOVER.md`'s WHAT REMAINS — nothing has EXECUTED on wasm32, armv7 or arm64 — is still
the web workstream's whole job.

### 2026-08-25 (second session, late) — the solver DRIVER is recovered

Still nothing here changes what the web build does — the two objects that moved are both
`libMdtKea`, i.e. the SOLVER, and the web build has no Karma at all yet. Three things are
worth knowing anyway.

- **`keaRbdCore_unified` — the solver driver — is in the build and reproduces the shipped
  library bit-for-bit on all three test scenes.** That is the object every previous note
  called the single thing standing in the way.
- **`keaLCPSolver` came OUT of the build**, measured wrong, so the object count is
  unchanged at 112 and the set is not. If you are diffing symbol lists against an earlier
  run, that is why.
- **arm64 is 2,258 truncation sites across 67 of 112**, down from 2,291/68 purely because
  keaLCPSolver left. armv7 remains **0** and wasm32 remains **112/112** with identical
  exported symbol sets.

The current dump directory is now **`out12`**, not `out11` — it differs in 2 of 153 dumps.
If you re-run the recovery, `HANDOVER.md` §4's command has been updated to match.

### 2026-08-25 (second session) — 112 objects, and the solver is two modules away

Nothing here changes what the web build does. Two figures move and one is worth a look.

- **112 objects, up from 110.** `keaLCPSolver` and `keaMemory` — both `libMdtKea`, i.e.
  the SOLVER, not collision — now compile and are in the build. Neither is reachable from
  anything the web build currently runs, because the web build has no Karma at all yet.
- **arm64 is worse in proportion, not in kind: 2,291 truncation sites across 68 of 112,**
  up from 2,218 across 66 of 110. armv7 is still **0**. The two new objects contribute 73
  sites between them, which is the ordinary rate — nothing special about them.
- **wasm32 is 112/112 with byte-identical exported symbol sets**, unchanged in character.

There is now a **ninth gate**, `test/vptr_ab.sh`, and it is i386-only by construction — it
compares a vtable dispatch against a devirtualised control by running two physics scenes.
It is not something the web agent needs to run, but if you re-dump Ghidra output it is on
the list `HANDOVER.md` §5 says must pass before adopting a new dump directory.

### 2026-08-25 — 110 objects, `out11`, and a gate that watches your pointer width

- **The dump directory is now `out11`.** `out9` → `out10` gave every C++ function a
  prototype under its **mangled** name — 75 mangled symbols across 21 objects had none, so
  Ghidra was guessing their call arity, which is exactly the defect the prototype header
  exists to prevent. 4 of 153 dumps changed and **every already-compiling object stayed
  byte-identical**. `out10` → `out11` adds a per-object `.locals` table (Ghidra's stack
  frame assignments) and changes no `.c` at all. **Nothing here affects the ABI you link
  against.**
- **110 objects, up from 109.** `MdtWorld` was recovered — and then turned out to be a
  Karma entry point UT2004 never calls, because `KDynStep.cpp` reimplements the whole
  stepping loop. Worth knowing when you scope: **the engine reimplements more of Karma
  than the API surface suggests** — collision dispatch for two whole families, the
  safe-time stepper, and the solver driver.
- **`test/ptrwidth_check.sh` exists and you should run it.** It is the gate §3's warning
  box used to say could not exist. 13 seconds, no arm64 hardware. armv7 reads **0**,
  arm64 reads **2,218 across 66 of 110 objects**. If you ever build for a 64-bit-pointer
  target — memory64 included — this is the number that tells you not to.
- **§3's arm64 figure was measured wrongly and is corrected.** "920 vs 23" added two
  different diagnostic sets together. The conclusion did not change; the measurement did.
  Read the box, it is short.
- **GJK's warm-start cache is now tested.** `McdGjkCgIntersect` warm-starts from a 60-byte
  block on the model pair, and until this week nothing had ever exercised that path in a
  test — the busiest pair family in the game, cold-path only. `KD_WARM=<K>` in
  `difftest_pair` runs a pair as a track of K consecutive frames with the cache live:
  **0 structural divergences over 200,000 pairs.** If you see GJK misbehave under wasm,
  the cache is no longer an unexamined suspect.
- **Three new test regimes**, all modelling situations the game creates and the tests did
  not: `KD_GRID=1` (axis-aligned poses), `KD_FLAT=1` (a coplanar floor rather than a bumpy
  patch), `KD_CORNER=1` (three perpendicular faces — a body resting in a corner). Every
  released object passes all three unchanged. **Use them**: if a wasm result differs from
  native, running the same pair through these three costs seconds and tells you whether
  the input distribution is the variable.
- **A released object was un-released, and the reason is one you will meet.**
  `IxCylinderTriList` (cylinder vs terrain) was signed off on a 5-minute sample showing
  zero divergences. Longer runs show ~1 in 4,000. **A zero is a sample size, not a
  property**, until the next run does not move it. Budget for that when you validate on
  wasm.
- **`IxCylinderCylinder` is re-framed, not fixed.** Its 3% `dims` divergence is a field
  UT2004 does not read for that pair, and one the shipped library does not reproduce
  either. What remains is ~1 in 10,000 on contact count. It is in the build.
- **A harness switch that was documented for months had never been implemented.**
  `KD_ONLY` in `kd_shadow.c` — "narrows the comparison to one function" — did nothing;
  every run that set it instrumented everything. Fixed. The general point is the one this
  project keeps paying for: **verify a switch by its effect, not its presence.**


older copy of this file, this is the diff.

**2026-08-24, fourth session.** Four things, and the first two change what you should
believe about your own target.

- **arm64 compiles, has identical symbols, and is wrong.** The full measurement is the
  warning box in §3. The short version: a clean cross-compile with matching exported
  symbols is not evidence about a target nobody has executed, and 64-bit pointers truncate
  through this code ~900 times. **wasm32 is not affected** — it is 32-bit, and it measures
  like armv7 (0 truncation diagnostics), not like arm64 (2,218). Do not enable memory64.
- **A defect got all the way through all seven gates and killed the engine at startup.**
  `MeFileSearch`'s `MeDefaultFileLocations` is a `const char *[22]` whose `.rodata` bytes
  are *relocation addends*, not addresses. Emitted raw, the file-search loop walked
  `0, 1, 0xc, 0x1a` as directory prefixes and the engine segfaulted in `fread` loading its
  first `.ka` asset — with a backtrace made entirely of *shipped* functions. Fixed
  (`gen_prelude.static_reloc_definition`). **Why you care:** the offline scenes never open
  a file, so no gate could see it. Your wasm build loads assets through a completely
  different path; if you see something similar, this is the shape.
- **The whole engine now runs on all recovered objects** on native i386 — not the 8 of
  §7b, everything. It plays a match and is indistinguishable from stock. That is the
  clearest signal yet that the recovered code is usable as a unit, and it is the state your
  wasm build should be aiming to reproduce. `HANDOVER.md` §7c has the four-minute recipe.
- **109 objects, 14 fail.** One more pair was measured and released
  (`Cylinder × TriangleList`, 71,417 real calls, 0 divergences); one was measured and found
  WRONG (`Cylinder × Cylinder`, 925 divergences) **and nothing holds it out of the build** —
  so if you build from `/tmp/kd_build`, that object is in it. `HANDOVER.md` §11 item 0.

**2026-08-24, third session.** The biggest change to your side of the line since this file
was written. Read all five points.

- **106 objects** (was 98 two sessions ago), **17 fail**. wasm32 106/106 with
  byte-identical exported symbol sets.
- **`libMcdConvexCreateHull` (qhull, 1.4 MB) IS REPLACED** by
  `src/McdConvexCreateHull/kd_convexhull.c`, ~600 lines of plain C, **10 KB**, and it is
  validated at four tiers including a live ONS match with 15,425 real GJK calls and zero
  structural divergences. **For you this is the single biggest win in the file**: it is
  ordinary portable C with no third-party build integration, it compiles for wasm32 with a
  symbol set identical to i386, and it removes 1.4 MB of 1998 global-state C from the link.
  Stage it with `test/make_hull_lib.sh`. Its acceptance test is `test/hull_probe.sh`
  (100,633 invariant checks) and `test/hull_ab.sh` (same-solid geometry diff) — **run both
  if you ever touch it**, because a hull that is subtly wrong does not fail at load; it
  degrades GJK silently, and GJK's support function hill-climbs that structure ~685,000
  times a match.
- **The `.ka` asset pipeline is now recovered, 9 objects of 9** — `MeChunk`, `MeXMLParser`,
  `MeXMLTree`, `MeXMLOutput`, `MeAssetDB`, `MeAssetDBXMLIO`, `MeAssetDBXMLInput_1_0`,
  `MeAssetFactory`, `MeFAsset`. **This matters to you more than the object count suggests:**
  it is the path `KCreateAssetDB` parses `../KarmaData/*.ka` through and `KSkeletal.cpp:388`
  instances **every ragdoll** through. Without it there are no ragdolls, on any target.
- **A portability shim you must not remove: `__ctype_b_loc` in `kd_compat.h`.** `<ctype.h>`
  declares it only on glibc; Emscripten's musl has no such function, so `MeXMLTree`,
  `MeXMLParser` and `MeFAsset` compiled for i386 and not for wasm32. The recovered code does
  not call `isspace()` — it indexes glibc's TABLE directly, the way the shipped object does
  — so the shim rebuilds the table from the C library's own predicates. Verified entry by
  entry against real glibc: **0 of 384 differ**. It is `#ifndef __GLIBC__`, so i386 is
  untouched.
- **Still nothing has been EXECUTED on wasm.** That remains the single largest unknown on
  your side, and it has not moved.

**2026-08-24, second session.**

- **99 objects** (was 98). The new one is `MeStream`, from a generic fix: an imported
  DATA symbol now gets a real `extern` declaration recovered from the DWARF of whichever
  object DEFINES it, size-checked against that object's symbol table. Four objects gained
  declarations this way.
- **The dump directory moved from `out6` to `out8`**, and this time it barely moved: **2
  of 153 dumps changed**, both of them solver objects that do not compile either way. Every
  compiled object is byte-identical. Nothing you cached is stale.
- **`wasm_check.sh` is 99/99 with byte-identical symbol sets.** Still nothing has been
  EXECUTED on wasm; that remains the single largest unknown on your side.
- **Two collision families are unreachable, not merely uncalled.** UT2004's dispatcher
  (`KIntersect`, `KFarfield.cpp:936`) intercepts `Box × TriangleList` and every `Aggregate`
  pair and calls its own code instead of Karma's. So `IxBoxTriList` — which is measurably
  wrong — is dead code and cannot be reached from any map, and the Aggregate family needs
  no recovery at all. `HANDOVER.md` §3a. If you are building a wasm coverage argument, this
  removes two families from it rather than leaving them as open risk.
- **The solver's virtual calls now carry their arguments** (`HANDOVER.md` §5a). It still
  does not compile, and the reason is now known precisely: **the DWARF for those functions
  declares no `DW_AT_location`**, so the frame is undescribed rather than merely
  unmodelled. Do not expect a solver on wasm soon.

**2026-08-24, first session.**

- **98 objects** (was 93). The new ones are `CxSmallSort`, `MstModelDynamics`,
  `McdGjkRegistration`, `McdPolygonIntersection`, `IxPrimitiveLineSegment` — the last is
  the raycast path, and `McdGjkRegistration` installs the ConvexMesh interactions you will
  need for anything vehicle-shaped.
- **The dump directory moved from `out5` to `out6`.** 103 of 153 Ghidra dumps changed and
  **not one compiled object did** — all 98 are byte-identical across the two. If you had
  cached anything keyed on the dumps, it is stale; if you cached objects, it is not.
- **A second unprototyped-indirect-call bug**, this time in the type database. §4b. This is
  the one most likely to bite you, because on wasm it traps rather than corrupting.
- **`__compar_fn_t`** — the first portability defect `wasm_check.sh` caught that the native
  build could not. §4b.
- **Three new gates on the recovery side**, and the reason they exist matters to you:
  `scene_census.sh` (which recovered functions a scene actually RUNS),
  `gate_sensitivity.sh` (whether the gate could have SEEN an error), and
  `check_symbol_bindings.py` (exported name **and binding** vs the shipped object). The
  first two exist because `substitute_test.sh` reported "trajectory bit-identical" for
  every object and **only eight of 103 had any measurable sensitivity** — for the rest that
  line was about the link, not the code. Do not build a wasm gate that can be green for the
  same reason; make it report what it executed.
- **The engine now drives on recovered collision code.** Eleven alternating 420 s ONS
  matches per arm across two maps, stock vs substituted: indistinguishable, 4 crashes each,
  same site — and that site turns out to be a bug in stock Karma, not ours. So "the
  recovered collision layer works in a real game" is settled natively. It is not settled on
  wasm, and that is still your line to cross.
- **The solver moved, but not in a way that unblocks you.** Three `libMdtKea` compute
  kernels are now proven bit-identical over 900 compounding steps. The four objects that
  CALL them still do not compile, all for one reason — Ghidra cannot model their frames —
  so §8 step 3 is exactly as blocked as before. Ask before assuming a date.
