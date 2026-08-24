# HANDOVER — Karma on the web, for the integration agent

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

- **98 recovered objects**, all of them compiling for i386 *and* for wasm32.
- **The full collision-detection path the game actually uses is recovered and validated** —
  all twelve interaction pairs UT2004 calls, each measured against the shipped original on
  real inputs from a live match. Evidence per object in `karma-decomp/proven.txt`. §6 has
  the census that says "twelve" and why that is the number to plan against.
- **The whole set already compiles under Emscripten**, and its exported symbol NAMES are
  **byte-identical to the i386 build for all 98 objects** — nothing added or dropped. So
  the ABI surface the engine links against does not change between targets, which was the
  thing most likely to turn this into a rewrite. See §4. (Names only: `wasm_check.sh`
  discards the binding letter. That gap let a recovered object export a *global* `putchar`
  for months — see "One thing that changed on your side of the fence" below.)

What does **not** exist, and you need to know this before planning anything:

- **Nothing has ever been EXECUTED under wasm.** "It compiles" is not "it works", and the
  runtime hazards in §4 are still open. That line is yours to cross, and it is §8 step 1.
- **The solver's control flow is not recovered.** `libMdtKea` is Karma's LCP solver — the
  code that turns contacts into motion. Three of its compute kernels are now proven
  bit-identical over 900 compounding steps, but the four objects that *call* them — the
  driver, the allocator, the integrator and the LCP — still do not compile. Collision
  detection tells you what touched; without the solver nothing moves. **A wasm build cannot
  run on recovered Karma alone today**, and closing that gap is recovery-side work, not
  yours. See "The gap that decides your schedule" below before planning around it.
- **Nothing has run end to end under wasm.** Natively, the collision layer has: all eight
  objects behind the twelve pairs the game calls were put in the driving seat — no shadow
  harness, the engine consuming the recovered code's answer every frame — through full ONS
  matches. The solver has not, and cannot until the four objects above compile. Everything
  else called "validated" was measured by a shadow harness that feeds the engine the
  ORIGINAL's answer every frame, so a recovered error never compounds. Do not assume
  "validated" means "has been run on".


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
98 of 98 recovered objects compiled for wasm32.  0 failures.
```

Stronger than that — the **exported symbol names are byte-identical to i386 for
all 98 objects**, nothing added or dropped. So the ABI surface the engine links
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
   Currently **98/98 clean on all three** on i386 — but read `HANDOVER.md` §4a before
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
   symbols against the native build. Currently 98/98 and 98/98. **Run this after any change
   to the recovery pipeline**; it is the cheapest possible early warning that a change has
   broken portability. It compares NAMES only — see the note at the end of §4.
4. **`test/kd_shadow.c`** — the in-game shadow harness. Runs both implementations on the
   same inputs and compares. Structural fields (return value, contact count, contact
   dimensionality, buffer overrun) must match **exactly**; float deltas are expected.
5. **`KD_SELFTEST=1`** — runs the *original* as both sides. Any divergence it reports is a
   harness bug, not a recovery bug. **Run it before believing any divergence you see, and
   before believing any crash** — a SIGSEGV was misattributed to recovered code for half a
   day because this was skipped, and it reproduces with no recovered code executing.

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
- **98 of ~150 objects compile**, 31 do not. But read `HANDOVER.md` §3 before reading that
  as 60% done — see the next bullet, it is the most important thing in this file for
  planning purposes.
- **19 objects are deliberately quarantined** by eight safety detectors. They compile but
  are known-or-suspected wrong. Do not include them to raise a coverage number. The proof
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

**37 collision-interaction pairs are registered. The game calls TWELVE of them.** A census
over 25 runs and 18 maps (`HANDOVER.md` §3) found that UT2004 gives its physics actors
sphere, sphyl, convex-mesh and triangle-list geometry and essentially nothing else. Every
`Aggregate` pair, every `Cylinder` pair, `Box×Plane`, `Box×TriangleList`, `Sphere×Plane`,
`Sphyl×Box`, `Sphyl×Plane` and `ConvexMesh×Plane` are registered on every map and called
**zero** times.

**All twelve are now validated against the real game** — the last, `ConvexMesh ×
TriangleList`, on 2026-08-23. Treat the count as provisional though: two pairs have already
moved off the never-called list, `Box×Sphere` most recently at 5,101 calls in one match
after 25 runs had shown none.

So "how much of Karma do I need for the web build to run" is not 150 objects and not even
98 — it is the collision path for twelve pairs, plus the solver, plus the framework objects
that hold them together. That is a much smaller target than the compile count suggests, and
it is the number to plan against.

### The gap that decides your schedule

**The collision path is done and drives a real match. The solver's arithmetic is proven;
its control flow is not recovered.**

`libMdtKea` is Karma's LCP solver: it takes the contacts collision detection produced and
works out how bodies actually move. As of 2026-08-24 three of its compute kernels —
`keaCalcJinvMandRHS_vanilla`, `keaCalcConstraintForces_vanilla`,
`keaCalcIworldandNonInertialForceandVhmf_vanilla` — reproduce the shipped library
bit-for-bit over 900 compounding solver steps, plus `MdtUtils`. That is real and it is new.

**It does not change your schedule.** Four objects still do not compile, and they are the
ones that *call* the kernels: the driver `keaRbdCore_unified`, the allocator `keaMemory`,
the integrator `keaIntegrate_pc` and the LCP itself (`keaLCPSolver` + `keaLCP_new`). Each
runs 900 times per 900 steps. Until they compile, **there is still no configuration in
which the engine runs on recovered Karma alone**, on any target.

The difference from the previous version of this section is the shape of the remaining
work, not its existence: it is four named objects with each blocker diagnosed to the line
(`HANDOVER.md` §11 items 1–3), not an open-ended expanse. Ask before assuming a date.

This is recovery-side work, not yours, but it constrains you in two ways worth planning
around:

1. **You cannot reach "the engine runs on wasm with recovered physics" by yourself.** Do not
   sequence your work as if you can. What you *can* do, entirely without the solver, is
   everything in §8 steps 1–3 — proving the recovered collision code executes correctly
   under wasm, using standalone scenes that link the shipped solver natively... except that
   the shipped solver is x86, so under wasm even the scenes need the recovered solver.
   **Which means §8 step 1 in its stated form is blocked on `libMdtKea` too.** See §8 for
   what is actually runnable today.
2. **What you learn early is worth more than what you learn late.** Every portability
   problem you find in the collision code now is a problem the solver recovery will avoid by
   construction, because the fix goes in the generator and the solver has not been generated
   yet. Front-load the hazard hunting in §4b.

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


The honest one-line summary of the project's state: *the collision layer is proven and
drives a real match; the solver's arithmetic is proven and cannot yet be reached; the
solver's control flow is untouched.* Do not read 98-of-150 as 65%.

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

Compilation is no longer step one — that is done. But read §6's "gap that decides your
schedule" first: **the solver's control flow is not recovered, so nothing runs end to end
yet, on any target.** That reshapes the order. Start with what is runnable without a
solver — and note that step 1 IS runnable today and nothing about it is blocked on the
recovery side.

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
3. **Then `scene_chain.c` and `scene_ragdoll.c` under wasm** — but note these step a full
   `MstUniverse`, so they need the solver. They become available when `libMdtKea` does.
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
older copy of this file, this is the diff.

**2026-08-24.**

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
