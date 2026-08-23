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
compile:  54 objects  (51 clean + 3 with prelude TODOs)  = 36.5% of 148 attempted
gate:     54 of 54 bit-identical trajectories, 0 failures
review:   49 objects held back by four safety detectors
fail:     45 objects do not compile
```

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

## 8. The four detectors — why objects are held back

`recover.py` refuses to put an object in the validated set when it matches any of these.
All four exist for one reason: **code that compiles and crashes is worse than code that
does not compile.**

| detector | pattern | why |
|---|---|---|
| mislabelled symbol | `(*_McdGeometryDeinit)(0x1c, 0x10)` | Ghidra mis-resolved a relocation-with-addend against a data symbol; the call target is simply wrong |
| guessed stack frame | `(int)aiStack_50 + iVar8` | Ghidra invented a local array and routed call arguments through it at computed offsets |
| argument-less indirect call | `(*fn)()` | no signature for a function pointer, so every argument is dropped and the callee reads the stack |
| reconstructed frame | `kd_argslot_` | this pipeline rebuilt the frame by inference; it can read perfectly and still be wrong |

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
- `__thiscall` and `__regparmN` are both no-ops here — verified against prologues. GCC's
  i386 C++ ABI passes `this` on the stack.

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
