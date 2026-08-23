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

- 54 recovered objects compiling for **i386** and producing bit-identical physics
  trajectories against the shipped library.
- Four of them validated against the real game over hundreds of thousands of live calls.
- **Nothing has ever been compiled for wasm.** Not once. i386 is the proving ground; the
  web is the actual goal. You are the first person to cross that line.

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

### Things that may bite under Emscripten specifically

1. **`alloca`.** Recovered code uses real `alloca()` for variable-length stack allocations
   (Karma sizes a triangle buffer by triangle count). Emscripten supports it, but the
   default wasm stack is small (64 KB unless `-sSTACK_SIZE` is raised). Karma's alloca is
   `triangleCount * 24` bytes and the count comes from map geometry. **Raise the stack and
   test with dense static meshes.** A stack overflow here will present as memory corruption,
   not a clean trap, unless you build with `-sASSERTIONS` / `-sSTACK_OVERFLOW_CHECK=2`.
2. **`__asm__("symbol")` labels.** The recovered code exports functions under their real ELF
   names via asm labels (`void kd_McdBoxBoxIntersect(...) __asm__("McdBoxBoxIntersect")`).
   This is used because Ghidra recovers the parameter types the *code* uses, which often
   differ from how the public header spells them — same ABI, incompatible C types. **Verify
   `wasm-ld` honours asm labels the same way.** If it does not, the fallback is to rename in
   the header instead, but that is a large mechanical change.
3. **C++ ABI data emitted from C.** `gen_vtables.py` re-emits `_ZTV*`/`_ZTI*`/`_ZTS*` as C
   arrays with asm labels and `__attribute__((weak))`, because the originals are COMDAT.
   Emscripten's handling of weak symbols and its own C++ ABI may conflict. Only `libMdtKea`
   needs this; if it fights you, consider hand-writing the four affected classes instead.
4. **Function pointers and the wasm table.** Karma stores engine callbacks as function
   pointers and calls them indirectly. In wasm those become table indices with **strict
   signature matching** — a mismatch is a runtime trap, not a silently-wrong call as on
   x86. This is arguably a *benefit* (it will catch signature errors the native build
   tolerates) but expect trapping where native "worked".
5. **No `-m32`.** Every command in the existing docs passes `-m32`. Drop it for wasm; wasm32
   is already 32-bit.

---

## 5. Validation — use what exists, do not invent your own

Three gates already exist and all of them work. Reuse them rather than eyeballing physics.

1. **`test/substitute_test.sh`** — swaps one recovered object into the link, runs a scripted
   scene, and compares the **trajectory** against baseline. On a collision-free scene a
   correct recovery is *bit-identical*; anything else is a bug. Currently 54/54 clean on
   i386. **Getting this to 54/54 under a wasm build is your first milestone.**
2. **`test/kd_shadow.c`** — the in-game shadow harness. Runs both implementations on the
   same inputs and compares. Structural fields (return value, contact count, contact
   dimensionality) must match **exactly**; float deltas are expected.
3. **`KD_SELFTEST=1`** — runs the *original* as both sides. Any divergence it reports is a
   harness bug, not a recovery bug. Run it before believing any divergence you see.

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

---

## 6. What is not done, and what you should not assume

- **Nothing is wasm-tested.** Every number in `karma-decomp/README.md` is i386.
- **54 of ~150 objects compile.** The recovery is incomplete. You will not be able to link a
  full Karma from recovered sources yet. Plan to link recovered objects *alongside* the
  shipped `.a` (the harness already does exactly this) until coverage is complete.
- **49 objects are deliberately quarantined** by four safety detectors. They compile but are
  known-or-suspected wrong. Do not include them to raise a coverage number — the detectors
  exist because code that compiles and crashes is worse than code that does not compile.
- **`libMcdConvexCreateHull` is qhull 2.6 (1998)** — 186 KB, open source, load-time only.
  **Do not recover it; replace it** with modern qhull, which builds under Emscripten without
  drama. Better still, precompute convex hulls offline and ship them as data — that matches
  this project's "engine differences are DATA, not code" rule.
- **`MeAssetDB`/`MeXML`/`MeAssetFactory`** (51 KB) is `.ka` XML asset loading, not physics.
  Rewrite or bypass rather than recover.
- **`MeViewer2`/`MeApp`** (74 KB) are never linked by UT2004. Skip entirely.

That last three points remove ~36% of the total binary footprint without recovering a line.

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

1. Compile **one** validated object (`IxSphereSphere` — smallest, cleanest, 0 divergences on
   120k+ real calls) under Emscripten. Nothing else. Find out what breaks.
2. Get `scene_chain.c` building and running under node/wasm with that one object substituted.
3. Widen to all 54 objects; get `substitute_test.sh` equivalent to 54/54.
4. Wire into the engine's wasm build behind `WITH_KARMA`, still linking the shipped `.a` for
   everything not yet recovered — except you cannot, because the `.a` is x86. **This is the
   crux:** until recovery covers everything the game calls, the wasm build cannot run Karma
   at all. Use the census in `HANDOVER.md` §3 to know exactly what "everything the game
   calls" means — it is far smaller than "all of Karma".
5. Feed anything you learn back to the recovery side. If a construct is unportable, it is
   usually cheaper to fix the *generator* than to patch the output.

Read `karma-decomp/HANDOVER.md` for how the recovery pipeline works, and
`docs/KARMA-ON-WASM.md` Part I §2 for the full architectural analysis behind §2 above.
