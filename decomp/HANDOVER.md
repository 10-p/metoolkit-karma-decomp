# HANDOVER — Karma decompilation, for whoever picks this up next

You are resuming a project to recover Karma (MathEngine `metoolkit`, UT2004's physics
library) from shipped binaries as portable C. Read this whole file before touching
anything. It is written for someone with no memory of how any of it came to be.

Branch: **`karma/decompile`**. `main` is untouched. 14 commits, most recent `daffc3e`.

---

## 1. Why this project exists

UT2004's physics is Karma, which ships as **binary-only static libraries**
(`Thirdparty/metoolkit/lib.rel/...`). There is no source, anywhere — the
`sigmaco/metoolkit-karma-v1.2` repo is headers and `.a`/`.lib` only. The web (wasm) and
Android builds therefore ship with `NO_KARMA` and no vehicle/ragdoll physics.

The full analysis of *why decompilation rather than emulation* is in
[`../docs/KARMA-ON-WASM.md`](../docs/KARMA-ON-WASM.md). Read at least Part I §2 and
Part II. The one-sentence version:

> The engine and Karma **share one address space in both directions** — the engine holds
> raw pointers into Karma's heap (`MdtBodyGetTransformPtr`) and Karma holds function
> pointers into engine code (`KTriListGenerator`, the allocator). Any out-of-address-space
> emulator (v86, QEMU, blink-as-a-VM) must marshal that traffic thousands of times per
> frame, which is an integration rewrite, not a performance tax.

The libraries carry **full DWARF-2 debug info** — original file names, line tables,
parameter names, local names, complete types. That is what makes this tractable at all.
Ghidra consumes it directly.

---

## 2. Current status

```
compile:  72 objects  (69 clean + 3 with prelude TODOs)  = 48.6% of 148 attempted
gate:     72 of 72 clean on BOTH scenes — bit-identical on scene_chain, and no
          crash on scene_boxes_on_plane (the two divergences there are IxBoxBox
          and IxBoxPlane, both on the collision path, both expected)
review:   20 objects held back by six safety detectors
fail:     56 objects do not compile
```

**Run both scenes.** `scene_chain` is collision-free and is the authoritative *trajectory*
signal; `scene_boxes_on_plane` diverges by design, but it is the only thing in the cheap
tier that exercises the geometry dispatch, and it is what caught the `__regparm` parameter
shift after the collision-free scene had passed it.

**Validated against the real game:**

| object | evidence |
|---|---|
| `IxSphereTriList` | 680,045 real calls, 0 structural divergences, worst delta 5.7e-06. **Released from quarantine** — see `proven.txt` |
| `IxSphereSphere` | 120k+ calls across runs, 0 structural divergences |
| `IxBoxBox` | 300k synthetic pairs + ~2k real, 0 structural divergences |
| `IxSphylPrimitives` | 78k real calls, **10 structural divergences in 49k (0.02%)**, all borderline contacts. Still quarantined — see §8 |

---

## 3. The census — this decides what to work on

Do not work alphabetically. The shadow harness counts every collision call the game
actually makes. Combined across both test maps, by source object:

| calls | object | status |
|---:|---|---|
| 941,805 | `IxSphereTriList` | ✅ validated |
| 122,527 | `IxSphereSphere` | ✅ validated |
| 36,828 | `IxSphylPrimitives` (ragdolls) | quarantined, 0.02% divergence |
| 6,290 | `IxConvexPrimitives` (vehicles) | compiles, unvalidated |
| 1,522 | `IxBoxBox` | ✅ validated |

**37 interaction pairs are registered; the maps exercise 9.** `IxBoxBox` — which was
polished first and proven on 300k synthetic pairs — gets ~1.5k real calls. Re-run the
census after any new map; it is cheap and it has repeatedly changed priorities.

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
   └─► ghidra_clean.py  ──► <obj>.c          (compilable)
                                │
                                ▼
                        recover.py (drives all of the above, compiles, classifies)
                                │
              ┌─────────────────┴─────────────────┐
              ▼                                   ▼
     substitute_test.sh                   kd_shadow.c (in-game)
     (breadth: swap object in,            (precision: run both
      run scene, diff trajectory)          impls on real inputs)
```

### Run the whole thing

```bash
cd /home/ion/engines/engine-ut2004/karma-decomp
rm -rf /tmp/kd_out /tmp/kd_build
python3 tools/recover.py \
  --dump-dir /home/ion/tools/karma-lab/out3 \
  --obj-dir  /home/ion/tools/karma-lab/allobj \
  --out-dir  /tmp/kd_out \
  --metoolkit ../Thirdparty/metoolkit \
  --protos /home/ion/tools/karma-lab/kd_protos.h
```

Recovered `.c` lands in `/tmp/kd_out/allobj/`, objects in `/tmp/kd_build/`.
`recover.py` prints a per-object table and a summary.

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

**`kd_types.h` fails GLOBALLY, not locally.** Twice this session a change to it took the
build from 50+ objects to **zero**. Always re-run `recover.py` immediately after touching
it, and never assume a change is safe because it looks additive.

---

## 5. Ghidra

Installed at `/home/ion/tools/ghidra_12.1.3_PUBLIC`. Java 21. Headless only.

```bash
cd /home/ion/tools/karma-lab
export KARMA_PROTOS=/home/ion/tools/karma-lab/kd_protos.h
export KARMA_OUTDIR=/home/ion/tools/karma-lab/out3
export KD_CALLSITE_SIG=trilist          # only when re-doing TriangleList objects
rm -rf gproj && mkdir -p gproj
timeout 7200 /home/ion/tools/ghidra_12.1.3_PUBLIC/support/analyzeHeadless \
  gproj Proj -import /home/ion/tools/karma-lab/allobj \
  -scriptPath /home/ion/tools/karma-lab/gscripts \
  -preScript ParseKarmaHeaders.java \
  -postScript DumpDecomp.java -deleteProject
```

Scripts live in `tools/gscripts/` and **must be copied** to
`/home/ion/tools/karma-lab/gscripts/` before running — Ghidra reads them from there.

### `ParseKarmaHeaders.java`

Parses `kd_protos.h` and applies the signatures. This matters enormously: without a
prototype, Ghidra guesses a call's arity from the call site **and gets it wrong** — it
decided `McdModelGetGeometry` took no arguments and emitted the pushed arguments as
writes to unrelated stack variables, with a pointer misread as a denormal float.

It also materialises a `Function` at each undefined symbol, because a relocatable `.o`
has none and both `getFunctions()` and `getExternalFunctions()` miss them.

`KD_CALLSITE_SIG=trilist` additionally applies `McdTriangleListFnPtr` at indirect call
sites **inside functions whose name contains `TriangleList`**. Scoped by function name,
not by object, because other objects call different callbacks through pointers and the
wrong signature is worse than none.

### Things about Ghidra that are settled — do not re-test

- Only the default `decompile` simplification style produces C. `normalize`, `firstpass`,
  `register`, `paramid` all fail outright.
- The "Decompiler Parameter ID" analyzer does **not** fix call arity.
- metoolkit's own headers do **not** survive Ghidra's C parser (MEAPI/MEPUBLIC macros).
  That is why `gen_protos.py` generates a flat, dependency-free prototype header instead.

---

## 6. Running the game

### Sandbox

`/home/ion/karma-run` — bulk content symlinked from the **read-only**
`/home/ion/ut2004-assets`, with `System/` and `Maps/` copied so they are writable.
Recreate with the recipe in `README.md` if lost. **Never write to `ut2004-assets`.**

### Build the instrumented engine

```bash
cd /home/ion/engines/engine-ut2004
cmake -S . -B build-shadow-karma \
  -DCMAKE_C_COMPILER=gcc-13 -DCMAKE_CXX_COMPILER=g++-13 \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo -DUT_GFX_BACKEND=gl4es \
  -DUSE_PIXOMATIC=ON -DBUILD_KARMA_REF=ON \
  -DMETOOLKIT_DIR=/home/ion/karma-run/shadow-metoolkit
cmake --build build-shadow-karma -j"$(nproc)"
```

**`gcc-13`, not 14.** The UE2.5 code does not compile with GCC 14. This is set by the
`native-karma` preset; a hand-rolled cmake invocation must set it explicitly.

Build takes ~3 min. Run it **from the repo root** — a background job that inherits
`karma-decomp/` as its cwd will fail on the relative path.

### Travel URLs — this is subtle and cost hours

Unreal's URL carries **two kinds of option** and you need both:

- **Game options** → `GameInfo.InitGame()`: `Game=`, `bAutoNumBots=`, `QuickStart=`,
  `bPlayerMustBeReady=`, `TimeLimit=`
- **Per-player login options** → `PreLogin`/`Login`: `Name=`, `Class=`, `Character=`,
  `team=`

With only the game half, the match **parks before kickoff**: the level loads, the process
burns CPU at full tilt, and *nothing ticks*. That reads exactly like a broken harness. The
URL that works:

```
ONS-Torlan.ut2?Name=Player-43ce41?Class=Engine.Pawn?Character=Jakob?team=255
  ?bAutoNumBots=True?Game=Onslaught.ONSOnslaughtGame?QuickStart=True?bPlayerMustBeReady=False
```

`test-karma-1` works on a bare URL only because its KActors fall under gravity with no
agent involved.

### Run headless

```bash
cd /home/ion/karma-run/System
export KD_SHADOW_OUT=/tmp/kd.csv KD_SHADOW_DIVERGENCES=/tmp/kd_div.txt
timeout --signal=TERM 300 xvfb-run -a -s "-screen 0 640x480x24" \
  ./ut2004-shadow.bin "<travel URL>" -GL4ESRENDERER -nohomedir > /tmp/run.log 2>&1
```

`test/run_map.sh` wraps this and prints which gametype the engine *actually* used — a
silent fallback to the wrong gametype is indistinguishable from a broken harness.

**Maps:** `test-karma-1` (10 KActors, 2 hinges, cone limit, ball-socket, an ONSRV — high
volume, reliable), `DM-BB-VehicleWar-test-physics` (all 7 vehicle factories),
`ONS-Torlan` (ragdolls + vehicles, but non-deterministic — see below).

**ONS matches are non-deterministic.** Identical runs give 0 calls or 50,000. A 5-minute
run showed *zero* structural divergences for the sphyl functions; the 11-minute run showed
ten. **Always run longer than feels necessary before concluding anything is clean.**

---

## 7. Instrumenting the game — the shadow harness

`test/kd_shadow.c`. Every collision call runs **both** implementations on the same inputs:
the original into the caller's real result, the recovered one into a scratch buffer. The
engine only ever consumes the original's output, so gameplay is bit-for-bit unchanged and
a session can be played out normally.

### Hook the registration, not the functions

The first design renamed each intersection function to `orig_*` and defined a replacement.
**It never ran.** Nothing calls `McdBoxBoxIntersect` by name — Karma installs it as a
function *pointer* in an interaction table, so renaming the symbol also rewrote the table's
own reference. Interposing `McdFrameworkSetInteractions` sees every
`(geometry type, geometry type)` pair the engine installs: one hook for the whole collision
matrix, no per-function code, and new pairs appear automatically.

### Build it

```bash
./test/make_shadow_metoolkit.sh ../Thirdparty/metoolkit /tmp/kd_build \
    /home/ion/karma-run/shadow-metoolkit
```

Recovered objects are staged with **every** defined symbol prefixed `rec_`, not just the
function under test — otherwise their siblings collide with the shipped archive and the
linker silently picks one, meaning you would be measuring a half-recovered build. The
comparison list is *generated* from the objects actually staged; hardcoding it leaves a
dangling `rec_*` reference and the whole engine fails to link.

To test a **quarantined** object, compile it into `/tmp/kd_build/` by hand first:

```bash
INC=../Thirdparty/metoolkit/include
gcc -m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w \
    -Wno-int-conversion -Wno-incompatible-pointer-types -DLINUX \
    -Iinclude -I$INC -I$INC/McdCommon -I$INC/McdPrimitives -I$INC/McdFrame \
    -I$INC/MeGlobals -I$INC/MdtBcl -I$INC/MdtKea -I$INC/Mst -I$INC/MeApp \
    -c -o /tmp/kd_build/IxSphylPrimitives.o /tmp/kd_out/allobj/IxSphylPrimitives.c
```

### `KD_SELFTEST=1` — run this before believing any divergence

Runs the **original as both sides**. Any divergence it reports is a bug in the harness, not
the recovery: these functions are supposed to write only through their output parameter, so
if calling one twice on identical inputs gives two answers, something else is shared.

Baseline: `test-karma-1` self-test gives **1,763,102 calls, 0 divergence**. The harness is
sound. That is what let us treat the `McdSphereTriangleListIntersect` divergence as real
and worth explaining rather than tolerating.

### Reading the output

`$KD_SHADOW_OUT` is a CSV rewritten periodically (not only at exit, so a crash still yields
data). Columns: `type1,type2,function,shadowed,calls,identical,fp_only,ret_diff,count_diff,
dims_diff,worst_delta`.

- `ret_diff` / `count_diff` / `dims_diff` are **decisions**. Any non-zero is a real defect.
- `fp_only` with a small `worst_delta` is float noise and expected.
- `$KD_SHADOW_DIVERGENCES` dumps the **full input transforms** for each divergence, so a
  case found in a live match can be replayed. The match never plays the same way twice, so
  a divergence you cannot reproduce is nearly worthless.

---

## 7a. The compile-feedback loop

`ghidra_clean.py` emits C, **compiles it, rewrites only the lines GCC rejected, and
recompiles**, until it settles. This replaced a set of whole-file regexes, and the reason
is dead end 6 in §9: `(float)x->member` is a legal conversion when the member is an int and
a bit reinterpretation when it is a pointer. The text is identical. The compiler is the
only thing in the pipeline that knows which is which.

**The verification is the whole design.** An edit is kept only if the diagnostic the rule
claimed is gone *and* the total error count did not grow. Otherwise it is reverted, and
the batch is retried one edit at a time in case a single bad rewrite masked good ones. So a
rule that misreads a line costs one compile and nothing else, and the file is never left
worse than as generated. Counting alone is not enough, incidentally: GCC reports an
undeclared name once per function, so fixing the first of several occurrences leaves the
count unchanged while making real progress.

Rules live in `REPAIR_RULES`, each paired with the diagnostic it claims. Adding one is
cheap. **Removing the verification is not.**

### The mislabelled-external rule — worth understanding before you touch it

This is the one that freed most of the objects, and it is the one with a trap in it.

A relocatable `.o` has no addresses for its imports, so Ghidra invents them: every
undefined symbol gets a four-byte slot in a synthetic EXTERNAL block, in ELF symbol-table
order. A relocation with an **addend** then lands in a neighbour's slot, and Ghidra reports
the neighbour:

```
5b: ff 15 08 00 00 00   call *0x8
    5d: R_386_32 MeMemoryAPI          ->  (*_McdGeometryDeinit)(0x20,0x10)
```

Nothing is lost — the relocation records the true base and the addend sits in the
instruction stream — so reading both back inverts it exactly:
`MeMemoryAPI.createAligned(0x20, 0x10)`, which is what McdNullCreate does.

**The trap:** Ghidra's block is a fiction, so two relocations that collide in it describe
two *unrelated* addresses at link time, and Ghidra prints the same name at both. `MdtWorld`
has exactly this: `_MePoolAPI` means `MePoolAPI.init` in `MdtWorldCreate` and
`MeMemoryAPI.destroy` in `MdtWorldDestroy`. So resolution is done **per function**, against
the relocations inside that function's own byte range, and if two candidates survive the
rule declines rather than picks.

At a call site the rewrite must name a struct member, because that is what supplies the
prototype — calling through an unprototyped pointer would default-promote a float argument
to double and change the ABI silently. No member, no rewrite.

---

## 8. The detectors — why objects are held back

`recover.py` refuses to put an object in the validated set when it matches any of these.
They all exist for one reason: **code that compiles and crashes is worse than code that
does not compile.**

| detector | pattern | why |
|---|---|---|
| mislabelled symbol | `(*_McdGeometryDeinit)(0x1c, 0x10)` | Ghidra mis-resolved a relocation-with-addend against a data symbol. *Mostly repaired now* — see §7a; what remains is what the repair could not explain |
| guessed stack frame | `(int)aiStack_50 + iVar8` | Ghidra invented a local array and routed call arguments through it at computed offsets |
| argument-less indirect call | `(*fn)()` | no signature for a function pointer, so every argument is dropped and the callee reads the stack |
| reconstructed frame | `kd_argslot_` | this pipeline rebuilt the frame by inference; it can read perfectly and still be wrong |
| **shifted parameter list** | `__regparm1` | Ghidra laid the parameters out for a convention that is not in use; the body is off by N and loses an argument. Found by the collision scene, not by reading |
| **unaccounted value** | `in_stack_0000000c`, `extraout_ECX` | a value read before anything assigns it — an incoming argument or a register Ghidra could not model. Dead stores are excluded; only a read that reaches something counts |

`proven.txt` records which objects a real match has released from quarantine, **with the
evidence on the line**. That is the only way out of quarantine. Do not remove a detector to
make a number go up.

### `IxSphylPrimitives` — the open judgement call

49,282 calls of `McdSphylTriangleListIntersect`: 6 `ret_diff`, 4 `count_diff`, worst delta
4.0e-04. Every captured case is a **borderline contact** (`ret 1/0 count 2/0`,
`ret 0/1 count 0/2`), and `dims_diff` is 0 throughout — the two never disagree about what
*kind* of contact exists, only whether it does.

At world coordinates ~250 with tolerance 0.00475, a 4.0e-04 delta is a relative error near
1.6e-06: f32 noise landing on a discrete threshold. For contrast
`McdSphereTriangleListIntersect` shows 0 structural divergences in 680k calls.

**Whether 0.02% threshold flapping is acceptable is a judgement call, not a measurement.**
Someone has to make it. My read: it is acceptable — the engine already tolerates contacts
appearing and disappearing frame to frame, and UT2004 replicates `KRigidBodyState` rather
than relying on determinism (see §10).

---

## 9. Dead ends — do not repeat these

1. **Broadening bare-tag aliases to all public tags** → build goes to **zero**. `MePoolAPI`
   is a struct tag *and* an ordinary identifier, so `typedef struct MePoolAPI MePoolAPI;`
   is "redeclared as a different kind of symbol". Only `_`-prefixed tags are safe.
2. **Adding enums without adding `enum` to public-type detection** → build goes to zero
   (redefines `MePoolType`).
3. **A fixed-size buffer for `alloca`** → compiles, passes the substitute gate, then
   segfaults in a real match. The allocation *size* comes from the same frame Ghidra failed
   to model. Use a real `alloca()`.
4. **`ucc-bin server`** hangs with no output. Use the real game binary.
5. **Ghidra simplification styles other than `decompile`** — all fail.
6. **Blind rewriting of `(float)x->member`** — if the member is genuinely an `int`,
   `(float)i` is a legal *conversion* and rewriting it to `*(float*)&i` silently
   reinterprets bits. Only rewrite lines the compiler actually flags. **This is the next
   task; see §11.**

---

## 10. Facts established by measurement (do not re-derive)

- **`MeReal` is `float`.** f32 semantics are vendor-blessed: MathEngine shipped a pure-f32
  x86-64 build (`lib.rel/linux_hx_single`).
- **x87 vs f32 diverges by 0.25 mm over 15 s** without collisions. With collisions it
  diverges **without bound** (111 m at 15 s) — contact make/break is discontinuous.
- **Bit-matching the reference was never achievable by anyone**, including MathEngine:
  their own i386 and x86-64 builds disagree by 111 m after 15 s.
- **UT2004 knows this.** `Actor.uc` has `KRigidBodyState`, `KUpdateState()`,
  `bSmoothKarmaStateUpdates=True` — the server replicates rigid-body state and clients
  interpolate. It never relied on cross-machine determinism.
- Therefore **trajectory diffing cannot validate a replacement past first contact**. Use a
  collision-free scene for trajectory comparison, and the per-function gate for the verdict.
- `-fno-strict-aliasing` is **required**, not a nicety. Decompiled code type-puns
  constantly; under `-O2` GCC deleted argument stores and `KTriListGenerator` received
  `(pair, 0, 0, 0, 0)`.
- `__thiscall` is a no-op here — verified against prologues. GCC's i386 C++ ABI passes
  `this` on the stack.
- **`__regparmN` is NOT a no-op.** An earlier version of this file said it was, on the
  strength of one prologue. That was wrong, and the collision scene proved it: Ghidra
  lays the parameter list out to match the convention it detected, so in a `__regparmN`
  function every parameter in the body is shifted by N and the last incoming argument is
  dropped. `McdGeometryGetMassProperties` passed three of its four arguments, each one
  position off, and segfaulted. 9 objects / 19 functions are affected; none of them are
  on the collision path. `recover.py` now holds them back.

---

## 11. What to do next

In order:

1. **Compile-feedback rewriting.** Instead of pattern-matching risky constructs, compile,
   parse the specific GCC error and line, and rewrite only that line. This makes the
   `(float)x->member` class safe and generalises to the rest of the flat tail.
2. **Validate `IxConvexPrimitives`** (vehicles, 6,290 calls) through the shadow harness. It
   compiles but has never been run.
3. **Make the call on `IxSphylPrimitives`** (§8).
4. **Grind the tail.** 45 objects, no class larger than 7, mostly individual Ghidra
   artifacts.
5. **`libMdtKea`** — the LCP solver, the hottest code, C++ with vtables. Layouts and
   vtables are recovered (`src/MdtKea/keaMatrix.h`) but no object has been validated.
6. **Replace, don't recover:** `libMcdConvexCreateHull` is qhull 2.6 (1998) — 186 KB,
   load-time only, and open source. Swap in modern qhull. `MeAssetDB`/`MeXML`/
   `MeAssetFactory` (51 KB) is `.ka` XML parsing, not physics.
   `MeViewer2`/`MeApp` (74 KB) are never linked — skip entirely.

## 12. What "complete" looks like

**Complete** is not "every object recovered". It is:

1. Every object the census shows the game *actually calls* is recovered and validated.
2. Validated means: 0 `ret_diff`, 0 `count_diff`, 0 `dims_diff` across a multi-hour
   in-game session, with `KD_SELFTEST` clean on the same session, and `proven.txt` carrying
   the evidence.
3. `substitute_test.sh` bit-identical on the collision-free scene for every recovered
   object.
4. qhull and the asset loader replaced rather than recovered.
5. No detector suppressed, no object released from quarantine without a line in
   `proven.txt`.
6. The whole set builds as ordinary C for **wasm32 and arm64/armv7**, not just i386 — see
   `HANDOVER-WEB.md`. Nobody has compiled any of this for wasm yet. That is the actual
   goal; i386 is the proving ground.
7. The engine runs with `WITH_KARMA=1` against recovered Karma with no shipped `.a` in the
   link at all.

Getting to (7) with 20 well-chosen objects beats getting to 90% coverage of objects nobody
calls.
