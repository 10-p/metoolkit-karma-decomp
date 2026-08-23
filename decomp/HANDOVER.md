# HANDOVER — Karma decompilation, for whoever picks this up next

You are resuming a project to recover Karma (MathEngine `metoolkit`, UT2004's physics
library) from shipped binaries as portable C. Read this whole file before touching
anything. It is written for someone with no memory of how any of it came to be.

Branch: **`karma/decompile`**. `main` is untouched.

---

## 1. Why this project exists

UT2004's physics is Karma, which ships as **binary-only static libraries**
(`Thirdparty/metoolkit/lib.rel/...`). There is no source anywhere — the
`sigmaco/metoolkit-karma-v1.2` repo is headers and `.a`/`.lib` only. The web (wasm) and
Android builds therefore ship with `NO_KARMA` and no vehicle or ragdoll physics.

The full argument for decompilation over emulation is in
[`../docs/KARMA-ON-WASM.md`](../docs/KARMA-ON-WASM.md) (read at least Part I §2 and Part II).
One sentence:

> The engine and Karma **share one address space in both directions** — the engine holds raw
> pointers into Karma's heap (`MdtBodyGetTransformPtr`) and Karma holds function pointers
> into engine code (`KTriListGenerator`, the allocator). Any out-of-address-space emulator
> must marshal that traffic thousands of times per frame, which is an integration rewrite,
> not a performance tax.

The libraries carry **full DWARF-2 debug info** — file names, line tables, parameter names,
complete types. That is what makes this tractable. Ghidra consumes it directly.

---

## 2. Status

```
compile:  93 objects  (89 clean + 4 with prelude TODOs)  = 62.9% of 148 attempted
gate:     93/93 clean on all three substitute scenes
wasm32:   93/93 compile, 93/93 exported symbol sets byte-identical to i386
review:   16 objects held back by eight safety detectors
fail:     39 objects do not compile
```

Reproduce all of that with the commands in §4. The whole pipeline is about a minute.

**Validated against the real game** (evidence in `proven.txt`, which is the ledger — a line
in it is what releases an object from quarantine):

| object | evidence |
|---|---|
| `IxSphereTriList` | 1,763,276 real calls, 0 structural divergences. Released. One known limit, §8 |
| `IxSphylPrimitives` | 74,921 real calls, 1 structural divergence (0.0013%). Released, §8 |
| `IxSphereSphere` | 128,885 real + 300k synthetic, 0 structural |
| `McdGjk` | 16,457 real calls of `McdGjkCgIntersect`, 0 structural — but see §8, a much larger run since |
| `IxConvexPrimitives` | 1,685 real calls, **all bit-identical**, + 300k synthetic |
| `IxBoxBox` | 35,427 real + 500k synthetic, **1 count divergence**, §8 |
| `IxBoxSphere` | 5,101 real calls, 0 structural. The census said this pair was never called |
| `IxConvexTriList` | 6,857 real calls, 0 structural. **Released**, §8 — this was the top task |


---

## 3. The census — this decides what to work on

**38 interaction pairs are registered. The game calls eleven of them. Ever.**

A sweep over 25+ runs and 18 maps, using `KD_CENSUS=1` so it perturbs nothing:

| pair | total calls | object | status |
|---|---:|---|---|
| Sphere × TriangleList | 3,710,224 | `IxSphereTriList` | ✅ |
| Sphyl × TriangleList | 786,072 | `IxSphylPrimitives` | ✅ |
| Sphyl × Sphyl | 334,850 | `IxSphylPrimitives` | ✅ |
| Sphere × Sphere | 257,887 | `IxSphereSphere` | ✅ |
| Box × ConvexMesh | 185,067 | `McdGjk` (`McdGjkCgIntersect`) | ✅ |
| Box × Box | 167,649 | `IxBoxBox` | ✅ |
| Sphyl × Sphere | 148,031 | `IxSphylPrimitives` | ✅ |
| Sphyl × ConvexMesh | 11,280 | `IxConvexPrimitives` | ✅ |
| ConvexMesh × TriangleList | 4,950 | `IxConvexTriList` | ✅ |
| Box × Sphere | 5,101 | `IxBoxSphere` | ✅ |
| Sphere × ConvexMesh | 2,274 | `McdGjk` | ✅ |
| ConvexMesh × ConvexMesh | 39 | `McdGjk` | ✅ |

**Registered on every map and called ZERO times, across every run so far:** every
`Aggregate` pair, every `Cylinder` pair, `Box×Plane`, `Box×TriangleList`,
`Sphere×Plane`, `Sphyl×Box`, `Sphyl×Plane`, `ConvexMesh×Plane`.

Absorb that before picking up work. UT2004 gives its physics actors sphere, sphyl,
convex-mesh and triangle-list geometry and essentially nothing else, so **whole objects in
the "not compiling" pile are for collisions the game never makes.**

Two cautions:

- Read "zero times" as "not in 25 runs", not "impossible". **Two pairs have already come
  off that list.** `ConvexMesh×ConvexMesh` did it with 39 calls on `ONS-UCMP-ABC-ECE`;
  `Box×Sphere` did it on 2026-08-23 with **5,101 calls in a single match on the same map**,
  after 25 runs across 18 maps had shown none. It was already recovered and clean, so it
  cost nothing — this time.
- It cuts both ways. `McdSphylBoxIntersect` had a real bug (§8) in a pair that is never
  called, and `IxSpherePlane` sits in the validated set for another. Neither was wasted —
  the sphyl bug was in shared code — but **"validated" is not "load-bearing"** without
  checking this table.

Re-run the census after any new map. It is cheap, non-perturbing, and it has changed
priorities every single time.


---

## 4. The pipeline

```
metoolkit .a
   │  ar x
   ▼
.o files ──► Ghidra headless ──► per-object .c dumps + stats.csv
   │            (DumpDecomp.java, ParseKarmaHeaders.java)
   │
   ├─► gen_protos.py    ──► kd_protos.h      (2218 prototypes, for GHIDRA to consume)
   ├─► gen_typedb.py    ──► kd_types.h + kd_types_fields.json
   ├─► gen_prelude.py   ──► <obj>.prelude.h + <obj>.exports.h
   ├─► gen_vtables.py   ──► <obj>.vtables.h  (C++ ABI data)
   └─► ghidra_clean.py  ──► <obj>.c          (compilable, via the repair loop §7a)
                                │
                                ▼
                        recover.py (drives all of the above, compiles, classifies)
                                │            (check_frame_bounds.py is one of its detectors)
                                │
        ┌───────────────────────┼───────────────────────┐
        ▼                       ▼                       ▼
 substitute_test.sh      difftest_pair.sh         kd_shadow.c (in-game)
 (breadth: does it       (depth: drive one        (truth: real inputs from
  crash on a scene)       interaction over         a real match)
                          randomised transforms)
```

### Run the whole thing

```bash
cd /home/ion/engines/engine-ut2004/karma-decomp
rm -rf /tmp/kd_out /tmp/kd_build
python3 tools/recover.py \
  --dump-dir /home/ion/tools/karma-lab/out5 \
  --obj-dir  /home/ion/tools/karma-lab/allobj \
  --out-dir  /tmp/kd_out \
  --metoolkit ../Thirdparty/metoolkit \
  --protos /home/ion/tools/karma-lab/kd_protos.h
```

Recovered `.c` lands in `/tmp/kd_out/allobj/`, objects in `/tmp/kd_build/`. `recover.py`
prints a per-object table and a summary. **`out5` is the current dump directory** (§5).

### Gate what came out — all four, every time

```bash
MT=../Thirdparty/metoolkit
LIB=$MT/lib.rel/linux_single_gcc3.2

# breadth: swap each object into a scene, diff the trajectory
./test/substitute_test.sh /tmp/kd_build $LIB test/scene_chain.c
./test/substitute_test.sh /tmp/kd_build $LIB test/scene_boxes_on_plane.c
./test/substitute_test.sh /tmp/kd_build $LIB test/scene_ragdoll.c

# depth: drive one interaction, 300k randomised transforms
./test/difftest_pair.sh /tmp/kd_build $MT            # all pairs
./test/difftest_pair.sh /tmp/kd_build $MT McdBoxBoxIntersect

# portability: §12 item 6, the actual goal
./test/wasm_check.sh /tmp/kd_out/allobj /tmp/kd_build $MT

# frame bounds: a defect no behavioural test can find, §8. Costs a second.
python3 tools/check_frame_bounds.py /tmp/kd_out/allobj
```

`scene_chain` is collision-free and is the authoritative *trajectory* signal.
`scene_boxes_on_plane` diverges by design but exercises the geometry dispatch — it caught
the `__regparm` parameter shift after the collision-free scene had passed it.
`scene_ragdoll` is a nine-capsule ragdoll on ball-socket joints dropped onto a plane and
boxes, because the other two make **not one Sphyl call** between them.

**`difftest_pair.sh` has six switches and the first is not optional:**

- **`KD_SELFTEST=1`** — run the ORIGINAL as both sides. Anything it reports is a fault in
  the driver. Run it before believing any divergence. Skipping the shadow harness's
  equivalent produced one wrong conclusion in a single day of work.
- **`KD_GENARGS=1`** — print what each side handed the triangle-list generator when they
  differ, rather than inferring it from the contacts. This is what found the `-0` radius in
  §8 after everything else had been eliminated. Use it whenever a TriangleList pair
  diverges: `pos` and `radius` are an output of the recovered code, and the contact
  comparison cannot see them.
- **`KD_TRIFLAGS=<n>`** — the flags the generated triangles carry. The default is what
  UT2004 sets (`UseSmallestPenetration | UseEdges` = 29); `KD_TRIFLAGS=0` restores the old
  behaviour, which skips the whole edge-contact path. Useful for telling "the edge path
  diverges" from "the face path diverges", and for nothing else.
- **`KD_SPREAD=<n>`** — scale how far apart the bodies scatter, which moves between contact
  **regimes**, and this matters more than it sounds. At the default the TriangleList tests
  run at 92% touching with six to eleven simultaneous contacts — deep interpenetration. A
  body resting on level geometry is one or two contacts at ~6%.
  `McdSphereTriangleListIntersect` has 40 feature-classification divergences in 50,000 at
  the deep end and **none** at the shallow end, which is why 1.76 M real calls never saw
  one. **Always report which regime a number came from.**
- **`KD_ORIGIN=<n>`** — shift the scene away from 0. f32 spacing at |x|=2 is ~2e-7 and at
  |x|=260 — an ordinary UT2004 world coordinate — it is 1.5e-5, against a contact tolerance
  of 0.00475. A test that only runs near the origin tests a precision regime the game never
  uses.
- **`KD_SKEW=1`** — nudge the test mesh off its axis-aligned grid, to tell "disagrees at an
  exact feature boundary" from "disagrees".


Adding a pair is one `IX(...)` line and one table row; the geometry factories are shared.

### Regenerate the type database (after any DWARF-side change)

```bash
python3 tools/gen_typedb.py /tmp/karmaprobe/members \
  --public-headers ../Thirdparty/metoolkit/include \
  --exclude libMeViewer2 --exclude libMeApp --exclude libMcdConvexCreateHull \
  --include MePrecision.h --include MeMath.h --include McdCTypes.h --include MdtTypes.h \
  --include McdModelPairContainer.h --include McdSpace.h --include McdGeometry.h \
  --include MdtKea.h --include MeProfile.h \
  -o include/kd_types.h
```

`/tmp/karmaprobe/members` is just extracted archive members — the same shape as
`/home/ion/tools/karma-lab/allobj`, which is what to widen it with if a type is missing.

**`kd_types.h` fails GLOBALLY, not locally.** Three times now a change to it took the build
from 90+ objects to **zero**. Always re-run `recover.py` immediately after touching it, and
never assume a change is safe because it looks additive. The most recent instance: adding a
base class as a by-value member without adding inheritance to the dependency graph, so the
derived type was emitted above its base.

---

## 5. Ghidra

Installed at `/home/ion/tools/ghidra_12.1.3_PUBLIC`. Java 21. Headless only.

```bash
cd /home/ion/tools/karma-lab
cp /home/ion/engines/engine-ut2004/karma-decomp/tools/gscripts/*.java gscripts/
export KARMA_PROTOS=/home/ion/tools/karma-lab/kd_protos.h
export KARMA_OUTDIR=/home/ion/tools/karma-lab/out6      # a NEW directory
export KD_CALLSITE_SIG=trilist
rm -rf gproj6 && mkdir -p gproj6 out6
timeout 7200 /home/ion/tools/ghidra_12.1.3_PUBLIC/support/analyzeHeadless \
  gproj6 Proj -import /home/ion/tools/karma-lab/allobj \
  -scriptPath /home/ion/tools/karma-lab/gscripts \
  -preScript ParseKarmaHeaders.java \
  -postScript DumpDecomp.java -deleteProject
```

Takes 1–2 hours. Scripts live in `tools/gscripts/` and **must be copied** to
`/home/ion/tools/karma-lab/gscripts/` — Ghidra reads them from there. **Write to a NEW
output directory** and keep the old one until the new dumps have passed all four gates; a
re-run changes every object at once.

### `ParseKarmaHeaders.java` (preScript)

Parses `kd_protos.h` and applies the signatures. This matters enormously: without a
prototype Ghidra guesses a call's arity from the call site **and gets it wrong** — it
decided `McdModelGetGeometry` took no arguments and emitted the pushed arguments as writes
to unrelated stack variables, with a pointer misread as a denormal float.

It also materialises a `Function` at each undefined symbol, because a relocatable `.o` has
none and both `getFunctions()` and `getExternalFunctions()` miss them.

### `DumpDecomp.java` (postScript)

Decompiles every function, and does two repairs first.

**`applyCallsiteOverrides()`** applies `McdTriangleListFnPtr` at indirect call sites inside
functions whose name contains `TriangleList` (`KD_CALLSITE_SIG=trilist`). Without it Ghidra
emits `(*fn)()` with every argument dropped. Scoped by function name, not by object, because
other objects call different callbacks through pointers and a wrong signature is worse than
none.

**`forceCdecl()`** rewrites functions Ghidra tagged `__regparm1`/`__regparm2` to `__cdecl`.
That misdetection is not cosmetic — see §10. A first attempt forced `__cdecl` on
*everything* and made things worse (gcc's i386 C++ ABI passes `this` as the first stack
argument, so `__thiscall` is already right), costing five kea objects that had been
compiling. **Fix the misdetection, not the convention system.**

```
out3 (before)     85 clean + 4 TODO,  46 fail
out4 (blanket)    85 clean + 1 TODO,  47 fail   <- worse
out5 (targeted)   88 clean + 4 TODO,  39 fail   <- current
```

### Settled — do not re-test

- Only the default `decompile` simplification style produces C. `normalize`, `firstpass`,
  `register`, `paramid` all fail outright.
- The "Decompiler Parameter ID" analyzer does **not** fix call arity.
- metoolkit's own headers do **not** survive Ghidra's C parser (MEAPI/MEPUBLIC macros).
  That is why `gen_protos.py` generates a flat, dependency-free prototype header.

### Ghidra's invented memory map, inverted

This is the single most useful piece of machinery here, so understand it before changing it.

A relocatable `.o` has no addresses, so Ghidra invents them: allocatable sections
consecutively from **0x10000** in section-header order, each aligned to its own
`sh_addralign`, then a synthetic **EXTERNAL block at the next 0x1000 boundary** with one
four-byte slot per undefined symbol in ELF symbol-table order. `ghidra_memory_map()`
reproduces it.

That inverts three whole classes of unresolvable name:

- **`DAT_00010405`** — an unnamed data reference. Maps to a section offset; the bytes are
  read from the object. Verified: MeXMLOutput's `.text` is 0x405 bytes, and
  `MeStreamWrite(&DAT_00010405,1,1,...)` writes offset 0 of `.rodata.str1.1`, whose first
  byte is `'<'`. The next two write `'>'` at +2 and `"</"` at +0x12 — an XML writer.
- **`_McdGeometryDeinit`** — a relocation with an **addend** landed in a neighbour's EXTERNAL
  slot and Ghidra reported the neighbour. `call *0x8` with `R_386_32 MeMemoryAPI` is
  `MeMemoryAPI.createAligned`. Resolution is **per function**, because the block is a
  fiction: two relocations that collide in it are unrelated addresses at link time and get
  the same printed name. `MdtWorld` has exactly this — `_MePoolAPI` is `MePoolAPI.init` in
  `MdtWorldCreate` and `MeMemoryAPI.destroy` in `MdtWorldDestroy`. Where two candidates
  survive, the rule declines rather than picks. Checked across the corpus: all 291 named
  external references resolve to a symbol that is actually undefined in that object.
- **`PTR__CxSmallSort_00011f20`** — a pointer-sized slot at a known address, resolved to a
  real symbol plus offset. This one is `_ZTV11CxSmallSort + 8`, the Itanium ABI address
  point a constructor stores. Section-aware: a first attempt matched on offset alone and put
  a vtable pointer on a constructor, because a `.text` offset can equal a `.rodata` one.

---

## 6. Running the game

### Sandbox

`/home/ion/karma-run` — bulk content symlinked from the **read-only**
`/home/ion/ut2004-assets`, with `System/` and `Maps/` copied so they are writable. Recreate
with the recipe in `README.md` if lost. **Never write to `ut2004-assets`.**

### Build the instrumented engine

```bash
cd /home/ion/engines/engine-ut2004
./karma-decomp/test/make_shadow_metoolkit.sh Thirdparty/metoolkit /tmp/kd_build \
    /home/ion/karma-run/shadow-metoolkit
cmake -S . -B build-shadow-karma \
  -DCMAKE_C_COMPILER=gcc-13 -DCMAKE_CXX_COMPILER=g++-13 \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo -DUT_GFX_BACKEND=gl4es \
  -DUSE_PIXOMATIC=ON -DBUILD_KARMA_REF=ON \
  -DMETOOLKIT_DIR=/home/ion/karma-run/shadow-metoolkit
cmake --build build-shadow-karma -j"$(nproc)"
```

**`gcc-13`, not 14.** UE2.5 does not compile with GCC 14. Build takes ~3 min. The binary is
`build-shadow-karma/Source/SDLLaunch/ut2004-karma-pixo.bin`.

To measure a **quarantined** object, compile it into `/tmp/kd_build/` by hand first (it is
deliberately absent), then re-stage:

```bash
INC=Thirdparty/metoolkit/include
gcc -m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w \
    -Wno-int-conversion -Wno-incompatible-pointer-types -DLINUX \
    -Ikarma-decomp/include -I$INC -I$INC/McdCommon -I$INC/McdPrimitives \
    -I$INC/McdFrame -I$INC/MeGlobals -I$INC/MdtBcl -I$INC/MdtKea -I$INC/Mst -I$INC/MeApp \
    -c -o /tmp/kd_build/IxConvexTriList.o /tmp/kd_out/allobj/IxConvexTriList.c
```

Two other build flavours, both used and both worth keeping:

- **stock control** — same engine, `-DMETOOLKIT_DIR=/home/ion/karma-run/stock-metoolkit`
  (just a copy of the shipped `.a` files). This is how you tell "the harness did it" from
  "the game does it". It settled the `KHandleCollisions` crash, §7.
- **substituted** — `test/make_substituted_metoolkit.sh` puts recovered objects in the
  archive *in place of* the shipped ones, so the engine actually runs on them. A different
  and harder question than the shadow harness, which feeds the engine the original's answer
  every frame so an error never gets to compound. Definition-of-done item 7 in miniature.

### Run a map

```bash
cd /home/ion/engines/engine-ut2004/karma-decomp
KD_BIN=/home/ion/engines/engine-ut2004/build-shadow-karma/Source/SDLLaunch/ut2004-karma-pixo.bin \
KD_SHADOW_OUT=/tmp/kd.csv KD_SHADOW_DIVERGENCES=/tmp/kd_div.txt \
./test/run_map.sh ONS-CBP2-Tropica 900 \
  '?Name=Player1?Class=Engine.Pawn?Character=Jakob?team=0?NumBots=8?MinPlayers=9?bAutoNumBots=False?QuickStart=True?bPlayerMustBeReady=False'
```

Unreal's URL carries **two kinds of option** and you need both: game options (`Game=`,
`NumBots=`, `QuickStart=`, `bPlayerMustBeReady=`, `TimeLimit=`) go to `GameInfo.InitGame()`,
and per-player login options (`Name=`, `Class=`, `Character=`, `team=`) go to
`PreLogin`/`Login`. With only the game half the match **parks before kickoff**: the level
loads, the process burns CPU, and nothing ticks. Bots are what generate physics —
`?NumBots=8?MinPlayers=9?bAutoNumBots=False`; `bAutoNumBots=True` alone has been seen to
produce none.

`run_map.sh` reads the **last** `Game class is` line. The first is always the Entry level's
`GameInfo`, which reads exactly like a silent fallback to the wrong gametype and cost one
investigation an hour.

### Which maps

**Community maps exercise far more of the collision matrix than Epic's.** This came from the
project owner and it is the single most useful operational fact here — Epic's maps are
optimised, CBP2/UCMP/BE-/SPAC- ones are not.

| map | what it gives |
|---|---|
| `test-karma-1` | ~1.7 M Sphere/TriangleList calls in five minutes, reliable, no bots needed |
| `ONS-CBP2-Tropica` | the general workhorse: Sphyl×TriangleList, Sphyl×Sphyl, Box×ConvexMesh, Sphyl×ConvexMesh |
| `ONS-UCMP-ABC-ECE` | the **only** map found that exercises ConvexMesh×TriangleList and ConvexMesh×ConvexMesh |
| `DM-BE-Clearing`, `DM-CBP1-Arkanos`, `DM-BE-Ruins` | Sphere×ConvexMesh, Sphyl×ConvexMesh |
| `DM-BB-VehicleWar-test-physics` | heavy ragdoll traffic; never once produced Sphyl×ConvexMesh in an hour |
| `ONS-Torlan` | refused to start a ticking match three times. Do not rely on it |

**A match that prints `START MATCH` is not a match that ticks.** The same binary and URL
produced 165,000 collision calls in two minutes on one run and zero in ten minutes on the
next, at 150% CPU, with an empty CSV. There is no way to tell from the log, so: **check
`$KD_SHADOW_OUT` after two minutes. If it has no rows at all, kill it and start again.**

### Census sweep

`KD_CENSUS=1` counts calls and runs nothing twice, so it perturbs nothing and can be pointed
at any map safely. That is how §3 was produced:

```bash
for m in ONS-CBP2-Yorda DM-BE-Clearing ONS-UCMP-ABC-ECE ...; do
  KD_CENSUS=1 KD_BIN=$BIN KD_SHADOW_OUT=/tmp/sweep_$m.csv \
    timeout 260 ./test/run_map.sh "$m" 200 "$COMMON_URL" >/dev/null 2>&1
  awk -F, 'NR>1 && $5+0>0 {print $1"x"$2}' /tmp/sweep_$m.csv | tr '\n' ' '
done
```

---

## 7. Instrumenting the game — the shadow harness

`test/kd_shadow.c`. Every collision call runs the original into the caller's real result
and, if a recovered implementation exists, the recovered one into a scratch buffer, then
compares.

### Hook the registration, not the functions

The first design renamed each intersection function to `orig_*`. **It never ran.** Nothing
calls `McdBoxBoxIntersect` by name — Karma installs it as a function *pointer* in an
interaction table, so renaming the symbol also rewrote the table's own reference.
Interposing `McdFrameworkSetInteractions` sees every pair the engine installs: one hook for
the whole matrix, and new pairs appear automatically.

### The comparison list is derived, not written

`make_shadow_metoolkit.sh` works out which functions it can compare. An interaction function
is one Karma installs, and Karma says which those are: `McdFooBarIntersect` is installed by
`McdFooBarRegisterInteraction`. **But not always** — `McdGjkCgIntersect` is installed by five
registrars that do not share its name, so the busiest pair in the game was silently never
compared until it fell back to the DWARF prototype (`int f(void *, void *)`). A candidate
that is never registered never matches at run time, so widening is safe where narrowing was
not.

Entries carry real ELF symbols, because the id is not always the symbol: the convex-mesh
interactions are C++ and ship mangled.

### Reading the output

`$KD_SHADOW_OUT` is a CSV rewritten every 10,000 dispatches (not only at exit, so a crash
still yields data). Columns: `type1,type2,function,shadowed,calls,identical,fp_only,
ret_diff,count_diff,dims_diff,overrun,worst_delta`, plus a trailing `# max thunk nesting
depth` comment line.

- `ret_diff` / `count_diff` / `dims_diff` are **decisions**. Any non-zero is a real defect.
- `overrun` is worse: the recovered function wrote past the buffer it was handed. The
  scratch buffer carries a canary, because without one an overflow corrupts whatever is next
  and the engine dies somewhere unrelated a few frames later.
- `fp_only` with a small `worst_delta` is float noise and expected.
- `$KD_SHADOW_DIVERGENCES` dumps the **full input transforms** for each divergence.

### `KD_SELFTEST=1` — run this before believing anything

Runs the **original as both sides**. Any divergence it reports is a bug in the harness.
Baseline: `test-karma-1` gives 1,763,102 calls, 0 divergence.

It settles crashes too, and skipping that cost half a day. A SIGSEGV in
`McdModelGetGeometryType` was written up as an overrun caused by `IxSphylPrimitives`. It is
not — it reproduces under `KD_SELFTEST=1` with no recovered code executing.

### The harness perturbs the engine, and the cause is still open

`kd_shadow.c` used to claim gameplay is bit-for-bit unchanged because the engine only
consumes the original's output. That rests on these functions writing only through their
output parameter, and **they do not**: `McdModelPair` carries `m_cachedData`, `responseData`
and `phase`.

Alternating 240 s runs on `ONS-UCMP-ABC`, counting SIGSEGVs in the engine's own
`KHandleCollisions`:

| build | crashes |
|---|---|
| stock Karma, no harness | **0 of 4** |
| harness, second call live | **4 of 14** |
| harness, second call off (`KD_CENSUS=1`) or narrowed (`KD_ONLY`) | **0 of 5** |

So the harness is implicated and the second call is where to look. It is intermittent, no
row is significant alone, and **the mechanism has not been found.**

One hypothesis was tested and **rejected**: giving the second call a *copy* of the
`McdModelPair` moved the rate from 3-in-8 to 1-in-6, which is nothing at that sample size.
The copy is kept anyway because the claim above is only true with it, and it is free —
re-measured against the baseline on `test-karma-1`, worst delta `5.722046e-06`, the same
figure to every digit.

Also **ruled out**: stack pressure. `kd_dispatch` puts ~2.9 KB on the stack per shadowed
call and aggregates dispatch to child pairs, so the frames could nest. They do not — the
harness counts the nesting and a full match reports **max depth 1**. Four minutes to measure
against the half hour a bisection trial costs.

Still untried, in order of promise:

1. **A shared contact pool.** If an intersection function bumps a per-frame allocator as
   well as filling the caller's array, calling it twice double-counts it. That would corrupt
   whatever is next in memory, which is what the backtrace looks like.
2. **A per-function bisect with `KD_ONLY`** over enough runs to mean something. Four each
   was not; at a 1-in-3 base rate you need on the order of ten.

Two switches exist for exactly this:

- **`KD_CENSUS=1`** — count calls, run nothing twice. Perturbs nothing measurable.
- **`KD_ONLY=<substring>`** — shadow only functions whose name contains it, no rebuild
  needed between attempts.

---

## 7a. The compile-feedback loop

`ghidra_clean.py` emits C, **compiles it, rewrites only the lines GCC rejected, and
recompiles**, until it settles. This replaced whole-file regexes, and the reason is dead end
6 in §9: `(float)x->member` is a legal conversion when the member is an int and a bit
reinterpretation when it is a pointer. The text is identical. The compiler is the only thing
in the pipeline that knows which.

**The verification is the whole design.** An edit is kept only if the diagnostic the rule
claimed got rarer *and* the total error count did not grow. Otherwise it is reverted and the
batch retried one edit at a time. A rule that misreads a line costs one compile.

Two things that were subtly wrong and are worth not re-introducing:

- Counting *presence* of a diagnostic is not enough — three `(float)ptr` casts in one
  expression are three diagnostics with identical text and line. It counts **occurrences**.
- `max_rounds` was 30 and **binding**, silently. Most rules fix one construct per pass and
  GCC reports an undeclared name once per function, so an object with thirty of something
  needs thirty rounds. McdBox stopped at 29 errors; running the loop a second time took it
  to 8. A cap that stops early looks exactly like a rule that does not work. It is 90 now.

Rules live in `REPAIR_RULES`, each paired with the diagnostic it claims. Some are marked
`multiline` (offered the whole statement, because Ghidra wraps long expressions) or
`file_wide` (resolve a *name* rather than a line).

---

## 8. The detectors — why objects are held back

`recover.py` refuses to put an object in the validated set when it matches any of these.
They exist for one reason: **code that compiles and crashes is worse than code that does not
compile.**

| detector | pattern | why |
|---|---|---|
| mislabelled symbol | `(*_McdGeometryDeinit)(0x1c, 0x10)` | mostly repaired now (§5); what remains is what the inversion could not explain |
| guessed stack frame | `(int)aiStack_50 + iVar8` | Ghidra invented a local array and routed call arguments through it |
| argument-less indirect call | `(*fn)()` | no signature for a function pointer, so every argument is dropped |
| reconstructed frame | `kd_argslot_` | rebuilt by inference; it can read perfectly and still be wrong |
| shifted parameter list | `__regparm1` | §10 |
| unaccounted value | `in_stack_0000000c`, `extraout_ECX` | a value read before anything assigns it. Dead stores excluded; only a read that reaches something counts |
| lost store | `x.f = x.f;` | a save-and-restore Ghidra reordered. See the sphyl entry below |
| **out-of-range frame reference** | `(int)afStack_11c + -0x1c` | `tools/check_frame_bounds.py`. An address outside the local it names — GCC cannot see it through the cast, and the runtime symptom is corruption of an unrelated local |

`proven.txt` records which objects a real match has released, **with the evidence on the
line**. That is the only way out. Do not remove a detector to make a number go up.

### `IxConvexTriList` — released, and the argument nobody was looking at

It was the last pair the game calls with nothing behind it, and it sat at 46%, then 18%,
then 24% wrong across three sessions, always with the same signature: `ret 1/0 touch 1/0
count 3/0`, the recovered code finding **no contacts** where the original finds three.

It was one argument. `code` is `typedef int code();` — a function type with **no parameter
list** — so the indirect call to the engine's `McdTriangleListFnPtr` was unprototyped, and C
applied the default argument promotions. The `float` radius went across as an 8-byte double
and the callee, which does have a prototype, read the low half:

| pair | radius the generator received | should have been |
|---|---|---|
| Sphere × TriangleList | 2048 | 0.91 |
| Sphyl × TriangleList | 8.796e+12 | 0.91 |
| Box × TriangleList | 6.019e-36 | 0.939 |
| ConvexMesh × TriangleList | **-0** | 1.019 |

`KTriListGenerator` turns that into a sphere query against the level. A radius landing too
**large** is harmless — the query returns a superset of the right triangles and a superset
gives the same contacts — and one landing too **small** returns nothing at all. That is why
`IxSphereTriList` and `IxSphylPrimitives` passed 1.76 M and 74,921 real calls carrying the
identical defect, and why `IxConvexTriList` did not. The ~76% of its calls that agreed
anyway are actors with a cached `KTriList`, where the generator skips the query and ignores
radius entirely.

```
McdConvexMeshTriangleListIntersect, ONS-UCMP-ABC-ECE, 900 s
  before  1,876 calls  272 ret_diff  172 count_diff
  after   6,857 calls    0 ret_diff    0 count_diff  0 dims  0 overrun
  and     2,369 more on a second match, same verdict
```

**The lesson is about the test, not the code, and it is the reason this took three
sessions.** Two blind spots in `difftest_pair.c` hid it completely, and both are now fixed:

- **The generator ignored `pos` and `radius`.** Every TriangleList interaction computes a
  bounding sphere for the other body and hands it over, so those two arguments are an
  *output* of the recovered code that the contact comparison cannot see. With a generator
  that returns the same 32 triangles regardless, a radius of `-0` is invisible. It now culls
  to the query sphere, as `KTriListQuery` does, and **`KD_GENARGS=1` compares what each side
  passed directly** — which is what actually found this, after a day of reading frames.
- **The triangles carried `flags = 0`.** UT2004 sets `UseSmallestPenetration | UseEdges` on
  every level triangle (`KTriListGen.cpp`), and with zero the entire edge-contact half of
  `GenerateTriangleContact` — two of its three `AccumulateSphylContacts` sites, and
  `ConvexHullNSegment` — never executed in any test ever run.

When a recovered function calls back into the engine, **its arguments are part of the
answer.** Nothing downstream will tell you they were wrong.

### The out-of-range frame reference — a defect no behavioural test can find

`base + negVar - K` is the outgoing-argument area *below* an alloca'd block. Collapsing the
frame shift left `(int)afStack_11c + -0x1c` — a write 28 bytes under an 8-byte array. Three
of these existed: two in `IxConvexTriList` and one in `IxSphylPrimitives`, which had been
**released** carrying it.

What makes them worth a detector of their own is that nothing else could see them. The cast
launders the bounds, so GCC says nothing. The substitute scenes pass. And `difftest_pair`
produced **byte-identical results with and without the defect** over 200,000 pairs, because
GCC happened to leave those eight bytes free in that build. Only a live match, where the
frame is laid out differently, turns it into corruption — and then it presents as an
unexplained divergence in some unrelated value.

So the check is on the text: `tools/check_frame_bounds.py` reports every `(&)?NAME + K`
where K falls outside what NAME declares. It finds exactly those three on the old output and
nothing now. Run it after any change to `materialise_alloca_frame`.

### `IxSphylPrimitives` — released, and how the question got answered


It sat quarantined as "is 0.02% threshold flapping acceptable?", which is a question with no
answer because it has no yardstick in it. Two things resolved it.

**Most of it was a bug.** Ghidra emitted `boxP[axis] = boxP[axis];`, which reads as a no-op
and is a save-and-restore around an aggregate overwrite — the machine code stashes that
component in a register before the three stores, and Ghidra folded the save into the restore
so the read moved *after* them. `McdSphylBoxIntersect` returned the right penetration depth
at the wrong point. Fixed: synthetic worst delta 3.59e-01 → 5.17e-05, in-game 11 structural
divergences in 77,202 → 1 in 74,921.

**Then the rest was measured against the right thing.** `scene_ragdoll.c` puts the recovered
code in the driving seat for 15 s:

| comparison | max divergence over 15 s | final |
|---|---|---|
| recovered vs shipped i386 | **3.677 m** | 1.48 m |
| shipped i386 vs shipped x86-64 | **3.283 m** | 1.188 m |

That second row is **MathEngine's own two shipped builds of their own source**. The recovery
differs from the shipped library by the same margin the vendor's builds differ from each
other, the divergence is *bounded*, and all three settle with the same residual energy.
There is no standard on which the recovered object fails and the vendor's x86-64 build
passes.

**The lesson is not about sphyls.** A tolerance question with no yardstick in it cannot be
answered and should not be escalated as a judgement call — find what the original already
tolerates and measure against that. The vendor shipped two builds that disagree; that is the
bar.

### `IxSphereTriList` — a known limit, found after release

The divergence rate is a function of **penetration depth**:

| `KD_SPREAD` | touching | bit-identical | dims divergences |
|---:|---:|---:|---:|
| 1.0 | 91.8% | 9.3% | 40 |
| 2.5 | 24.4% | 76.7% | 6 |
| 4.0 | 6.4% | 93.8% | **0** |

At shallow resting contact — what the game does — there are none, consistent with 0 in
1.76 M real calls. Under deep interpenetration the two disagree about a contact's *feature*
classification while agreeing on position to 1e-4 and separation exactly. Not axis-alignment
degeneracy (`KD_SKEW=1` moved 40 to 38). The release stands; a body spawned inside geometry
reaches the other regime.

### `IxBoxBox` — one unreproduced divergence

**1 count divergence in 1,299 real calls** — both agree the boxes touch, original says 2
contacts, recovered says 4, at world (259.9, 8.0, 10.2). It does not reproduce: 300,000
synthetic pairs near the origin and 200,000 at `KD_ORIGIN=260` are both clean, though the
numeric spread rises 3.5× as coarser f32 spacing predicts. Distance alone is not it — the
driver uses one fixed pair of box sizes and the game's evidently differ. Vary those next.

### `McdGjk` — 18 divergences in the busiest pair, and the harness is the suspect

The first long run of Box × ConvexMesh — 72,167 calls, four times any before it — found
**3 ret_diff and 15 count_diff**, worst delta 2.057. All eighteen are the same two actors
over consecutive frames of one persistent contact.

That shape fits the harness. `McdGjkCgIntersect` keeps a cache on the `McdModelPair`
(`McdCacheHello`/`Goodbye` are registered beside it), and `kd_shadow` gives the second call a
**copy** of the pair (§7). So the original runs with its cache warm across frames and the
recovered runs cold every time, its cache written into a copy that is discarded. For an
iterative algorithm on a persistent contact that is a difference in *input*, not in code.
`KD_SELFTEST` on the same map and length gave 29,899 calls with 0 divergence — but there
both sides are the original, both start from the same copied cache and evolve together,
which is exactly the case the hypothesis says should be clean. A second match gave 13,603
calls with 0 divergence, so it is situational rather than a rate.

**Not settled.** The way to settle it is to give the second call the real pair for this one
function and see whether the divergences go — which is also the shared-state question in
§7, so chase the two together.

### `IxBoxTriList` — its clean synthetic result has been withdrawn

`proven.txt` used to record it as clean in both regimes over 30,000 pairs. Both rows were
measured with the old driver — same 32 triangles every call, `flags = 0`. Against a
generator that culls to the query sphere and sets the flags the engine sets, it is
**2,254 ret_diff, 118,129 count_diff and 11,691 dims_diff in 200,000**, with the driver
self-testing 100% bit-identical on the same inputs. Not chased: the census has
Box × TriangleList at zero calls. Recorded so nobody re-derives the old number.


---

## 9. Dead ends — do not repeat these

1. **Broadening bare-tag aliases to all public tags** → build goes to **zero**. `MePoolAPI`
   is a struct tag *and* an ordinary identifier, so `typedef struct MePoolAPI MePoolAPI;` is
   "redeclared as a different kind of symbol". Only a checked list is safe — there are three
   (`McdErrorDescription`, `MePoolFixed`, `MePoolMalloc`), found by scanning for tags that
   are defined, never typedef'd, and then used bare. Those headers are C++-only, which is
   why `McdFrame` — the collision framework — never compiled.
2. **Adding enums without adding `enum` to public-type detection** → build goes to zero.
3. **A fixed-size buffer for `alloca`** → compiles, passes the substitute gate, then
   segfaults in a real match. Use a real `alloca()`.
4. **`ucc-bin server`** hangs with no output. Use the real game binary.
5. **Ghidra simplification styles other than `decompile`** — all fail.
6. **Blind rewriting of `(float)x->member`** — see §7a.
7. **Forcing `__cdecl` on every function** — §5.
8. **Trusting a clean synthetic run for anything holding `kd_argslot_`** — `IxConvexTriList`
   is the second object to prove this.
9. **Trusting a test whose callback ignores its arguments.** `difftest_pair`'s triangle
   generator ignored `pos` and `radius` and set `flags = 0`. That hid a `-0` radius in four
   objects and left the entire edge-contact path of `GenerateTriangleContact` unexecuted, for
   the whole life of the project. When a recovered function calls back into the engine, the
   arguments it passes are part of its answer — make the stub depend on them (§8).


---

## 10. Facts established by measurement (do not re-derive)

- **`MeReal` is `float`.** f32 semantics are vendor-blessed: MathEngine shipped a pure-f32
  x86-64 build (`lib.rel/linux_hx_single`).
- **x87 vs f32 diverges by 0.25 mm over 15 s** without collisions, and **without bound**
  with them (111 m at 15 s) — contact make/break is discontinuous.
- **Bit-matching the reference was never achievable by anyone**, including MathEngine: their
  own i386 and x86-64 builds disagree by 111 m after 15 s.
- **UT2004 knows this.** `Actor.uc` has `KRigidBodyState`, `KUpdateState()`,
  `bSmoothKarmaStateUpdates=True` — the server replicates rigid-body state and clients
  interpolate. It never relied on cross-machine determinism.
- Therefore **trajectory diffing cannot validate a replacement past first contact**. Use a
  collision-free scene for trajectory comparison and the per-function gate for the verdict.
- `-fno-strict-aliasing` is **required**. Under `-O2` GCC deleted argument stores and
  `KTriListGenerator` received `(pair, 0, 0, 0, 0)`.
- `__thiscall` is a no-op here — GCC's i386 C++ ABI passes `this` on the stack.
- **`__regparmN` is NOT a no-op.** An earlier version of this file said it was, on the
  strength of one prologue. The collision scene disagreed: Ghidra lays the parameter list out
  to match the convention it detected, so the body is shifted by N and the last incoming
  argument is dropped. `McdGeometryGetMassProperties` passed three of its four arguments,
  each one position off, and segfaulted. Now fixed at the source (§5).

---

## 11. What to do next

**§12 item 1 is done.** Every pair the census shows the game calling is recovered and
validated against a live match. What is left is the confidence around that, then the tail.

1. **Settle the `McdGjk` divergences and the harness perturbation together** (§8, §7). They
   are probably the same thing: the second call gets a *copy* of the `McdModelPair`, and
   `McdGjkCgIntersect` keeps a cache on it. Give the second call the real pair for that one
   function and re-measure — 18 divergences in 72,167 on the busiest pair in the game is the
   largest open question about a released object. The shared-contact-pool hypothesis in §7
   is the other half.
2. **Chase the `IxBoxBox` count divergence** (§8) — vary the box dimensions in
   `difftest_pair.c`.
3. **Audit the other callbacks the way the triangle generator was audited** (§8). It is now
   the only stub in `difftest_pair.c` that depends on its arguments. Anything else Karma
   calls back into — the allocator, `McdCacheHello`/`Goodbye` — has the same exposure and
   has never been checked.
4. **Grind the tail.** 39 objects, but **read §3 first** — a large part of the pile is
   geometry the game never collides. What remains, by size: `stack0xNNNN` (~20 references,
   a real value Ghidra lost — do not paper over it), `too few arguments` (14, genuinely
   dropped arguments), types nothing defines (`MeASEObject`, `Mesh2GeometryType`,
   `BodyData` — they are in the DWARF, and `gen_typedb.py` takes object directories, so
   widening its input is the route), and `subscripted value` (16, all DebugDraw and XML —
   a file-scope static emitted as `float x[n]` and indexed `x[i][j]`).
5. **`libMdtKea`** — the LCP solver, the hottest code, C++ with vtables. Layouts and vtables
   are recovered (`src/MdtKea/keaMatrix.h`) but no object has been validated.
6. **Replace, don't recover:** `libMcdConvexCreateHull` is qhull 2.6 (1998) — 186 KB,
   load-time only, open source. Swap in modern qhull. `MeAssetDB`/`MeXML`/`MeAssetFactory`
   (51 KB) is `.ka` XML parsing, not physics. `MeViewer2`/`MeApp` (74 KB) are never linked.


---

## 12. What "complete" looks like

**Complete** is not "every object recovered". It is:

1. Every object the census (§3) shows the game *actually calls* is recovered and validated.
   **That is eleven pairs, and all eleven are done** — the last, `IxConvexTriList`, on
   2026-08-23. This is the item that was open for the whole project; treat any new entry in
   the census (§3 has had two) as re-opening it.
2. Validated means: 0 `ret_diff`, 0 `count_diff`, 0 `dims_diff`, 0 `overrun` across a
   multi-hour in-game session, with `KD_SELFTEST` clean on the same session, and
   `proven.txt` carrying the evidence.
3. All three `substitute_test.sh` scenes clean for every recovered object.
4. qhull and the asset loader replaced rather than recovered.
5. No detector suppressed, no object released without a line in `proven.txt`.
6. The whole set builds as ordinary C for **wasm32 and arm64/armv7**, not just i386.
   **wasm32 is done** — 93/93 compile with byte-identical exported symbols
   (`test/wasm_check.sh`). arm64 has not been tried; no cross-compiler is installed here.
   Nothing has been *executed* under wasm. See `HANDOVER-WEB.md`.
7. The engine runs with `WITH_KARMA=1` against recovered Karma with **no shipped `.a` in the
   link at all**. `test/make_substituted_metoolkit.sh` does this per object; a 300 s
   ragdoll-heavy ONS match on recovered sphyl gave 0 crashes, 0 NaN, 0 Karma warnings.

With (1) closed, **(7) is the next real milestone** and the one that actually delivers
physics on the web. The shadow harness structurally cannot test it: it feeds the engine the
original's answer every frame, so a recovered error never gets to compound. Item 2's
in-game numbers say the recovered code *agrees*; item 7 asks whether it can *drive*.

