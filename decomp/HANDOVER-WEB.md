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

- **93 recovered objects**, all of them compiling for i386 *and* for wasm32.
- Six of them validated against the real game over millions of live collision calls, with
  the evidence recorded per object in `karma-decomp/proven.txt`.
- **The whole set already compiles under Emscripten**, and its exported symbol sets are
  **byte-identical to the i386 build for all 93 objects** — same names, same bindings,
  nothing added or dropped. So the ABI surface the engine links against does not change
  between targets, which was the thing most likely to turn this into a rewrite. See §4.
- **Nothing has ever been EXECUTED under wasm.** "It compiles" is not "it works", and the
  runtime hazards in §4 are still open. That line is yours to cross.

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
89 of 89 recovered objects compiled for wasm32.  0 failures.
```

Stronger than that — the **exported symbol sets are byte-identical to i386 for
all 89 objects**. Same names, same bindings, nothing added or dropped. So the ABI
surface the engine links against does not change between the two targets, which
was the thing most likely to turn this into a rewrite.

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
   Currently **93/93 clean on all three** on i386. **Getting that under a wasm build is your
   first milestone.**
2. **`test/difftest_pair.sh`** — drives one interaction directly over randomised transforms,
   seeded so anything it finds reproduces. Eleven pairs wired up. Four switches, and read
   `HANDOVER.md` §4 on `KD_SPREAD` before quoting any number from it: divergence rates here
   are a strong function of contact regime, and a figure without its regime is meaningless.
3. **`test/wasm_check.sh`** — compiles the whole set for wasm32 and diffs the exported
   symbols against the native build. Currently 93/93 and 93/93. **Run this after any change
   to the recovery pipeline**; it is the cheapest possible early warning that a change has
   broken portability.
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
`phase` for it to write to. Measured on one map: stock Karma 0 crashes in 4 runs, harness
4 in 14, harness with the second call suppressed 0 in 5. The cause is not yet found.
`KD_CENSUS=1` turns the second call off and perturbs nothing measurable — use it when you
want call counts from a build you do not want to disturb.

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
- **93 of ~150 objects compile**, 39 do not. But read `HANDOVER.md` §3 before reading that
  as 60% done — see the next bullet, it is the most important thing in this file for
  planning purposes.
- **16 objects are deliberately quarantined** by eight safety detectors. They compile but
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

**38 collision-interaction pairs are registered. The game calls ELEVEN of them.** A census
over 25 runs and 18 maps (`HANDOVER.md` §3) found that UT2004 gives its physics actors
sphere, sphyl, convex-mesh and triangle-list geometry and essentially nothing else. Every
`Aggregate` pair, every `Cylinder` pair, `Box×Plane`, `Box×TriangleList`, `Sphere×Plane`,
`Sphyl×Box`, `Sphyl×Plane` and `ConvexMesh×Plane` are registered on every map and called
**zero** times.

**All eleven are now validated against the real game** — the last, `ConvexMesh ×
TriangleList`, on 2026-08-23. Treat the count as provisional though: two pairs have already
moved off the never-called list, `Box×Sphere` most recently at 5,101 calls in one match
after 25 runs had shown none.

So "how much of Karma do I need for the web build to run" is not 150 objects and not even
93 — it is the collision path for eleven pairs, plus the solver (`libMdtKea`, untouched),
plus the framework objects that hold them together. That is a much smaller target than the
compile count suggests, and it is the number to plan against.

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

Compilation is no longer step one — that is done. Start at execution.

1. **Run `scene_chain.c` under node/wasm** with the recovered objects linked, and diff the
   trajectory against the native i386 run of the same scene. This is the smallest thing that
   proves the recovered code *executes* correctly, and it needs no engine at all — the scene
   is a standalone `main()` against the metoolkit API. Expect float-level differences, not
   bit-identity.
2. **Then `scene_ragdoll.c`**, and calibrate against the vendor-vs-vendor number in §5. That
   scene exercises the sphyl path, which is the second busiest thing in the game.
3. **Then the runtime hazards in §4** in this order, because they are ordered by how hard
   they are to debug after the fact: `alloca` stack sizing (hazard 1 — 12 objects use it,
   including the two busiest functions in the game), then function-pointer signature
   matching in the wasm table (hazard 4 — a mismatch is a *trap*, not a silently-wrong call,
   which is a gift you should take).
4. **Only then** wire into the engine's wasm build behind `WITH_KARMA`. Note the crux: you
   cannot fall back to the shipped `.a` for what is not yet recovered, because it is x86.
   Use the census (§6) to scope what "enough" means.
5. **Feed anything you learn back to the recovery side.** If a construct is unportable it is
   almost always cheaper to fix the *generator* than to patch the output — the whole
   pipeline regenerates in about a minute, and `test/wasm_check.sh` tells you in one command
   whether a generator change kept portability.

Read `karma-decomp/HANDOVER.md` for how the recovery pipeline works, and
`docs/KARMA-ON-WASM.md` Part I §2 for the full architectural analysis behind §2 above.
