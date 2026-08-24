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
compile:  98 objects  (93 clean + 5 with prelude TODOs)  = 66.2% of 148 attempted
scenes:   98/98 run clean on all three substitute scenes, and all 98 TOGETHER
          are bit-identical on the collision-free one — but read §4a before
          reading anything into that number
wasm32:   98/98 compile, 98/98 exported symbol sets byte-identical to i386
bindings: 98/98 export what the SHIPPED object exported, binding included (§8)
difftest: self-test 12/12, and the real run reproduces the documented baseline
          exactly (§8) — IxBoxBox 1 count, IxSphereTriList 137 dims, the rest 0
review:   19 objects held back by recover.py's eight safety detectors (§8;
          the ninth, symbol bindings, is a gate rather than a detector because
          it needs the shipped object to compare against)
fail:     31 objects do not compile
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
| `IxConvexTriList` | 20,791 real calls over four matches, 0 structural. **Released**, §8 |

**And, as of 2026-08-24, three solver kernels** — the first `libMdtKea` evidence in the
project. `keaCalcJinvMandRHS_vanilla`, `keaCalcConstraintForces_vanilla` and
`keaCalcIworldandNonInertialForceandVhmf_vanilla` reproduce the shipped library
bit-for-bit over 900 compounding steps on all three scenes, on a gate measured to be
sensitive to 4e-4 m (chain) and 2.8–113 m (ragdoll, boxes). Details and the four objects
still blocking the solver are in `proven.txt` under "libMdtKea, the solver", and the
honest summary is §12.


---

## 3. The census — which collision pairs the game actually uses

This is the single most useful table in the project. It decides what to work on, and it has
changed priorities every time it has been re-run.

**37 interaction pairs are registered on a typical map. The game calls twelve of them.**
25 of the 37 have a recovered counterpart staged; the twelve that are called all do.

Produced with `KD_CENSUS=1`, which counts calls and runs nothing twice, so it perturbs
nothing and can be pointed at any map safely (§6). Call counts below are the running total
over every instrumented match to date; treat the ORDER as solid and the absolute numbers as
"how much traffic this pair gets", not as a constant.

### Called — all twelve are recovered and validated

| pair | calls observed | function | object | status |
|---|---:|---|---|---|
| Sphere × TriangleList | 11,018,910 | `McdSphereTriangleListIntersect` | `IxSphereTriList` | ✅ released, one known limit §8 |
| Sphyl × TriangleList | 1,258,577 | `McdSphylTriangleListIntersect` | `IxSphylPrimitives` | ✅ released |
| Sphere × Sphere | 866,203 | `McdSphereSphereIntersect` | `IxSphereSphere` | ✅ |
| Box × ConvexMesh | 608,280 | `McdGjkCgIntersect` | `McdGjk` | ✅ released, open defect §8 |
| Sphyl × Sphyl | 572,616 | `McdSphylSphylIntersect` | `IxSphylPrimitives` | ✅ |
| Sphyl × Sphere | 218,426 | `McdSphylSphereIntersect` | `IxSphylPrimitives` | ✅ |
| Box × Box | 150,146 | `McdBoxBoxIntersect` | `IxBoxBox` | ✅ released, 1-in-500k §8 |
| Sphyl × ConvexMesh | 62,698 | `McdSphylConvexMeshIntersect` | `IxConvexPrimitives` | ✅ |
| ConvexMesh × TriangleList | 49,064 | `McdConvexMeshTriangleListIntersect` | `IxConvexTriList` | ✅ released 2026-08-23 |
| Sphere × ConvexMesh | 9,741 | `McdGjkCgIntersect` | `McdGjk` | ✅ |
| Box × Sphere | 5,154 | `McdBoxSphereIntersect` | `IxBoxSphere` | ✅ |
| ConvexMesh × ConvexMesh | 4,337 | `McdGjkCgIntersect` | `McdGjk` | ✅ |

### Never called — zero, on every map, in every run so far

| pair | function | recovered? |
|---|---|---|
| Box × Plane | `McdBoxPlaneIntersect` | yes |
| Box × Cylinder | `McdBoxCylinderIntersect` | yes |
| Box × TriangleList | `McdBoxTriangleListIntersect` | quarantined, and **badly wrong**, §8 |
| Sphere × Plane | `McdSpherePlaneIntersect` | yes |
| Cylinder × Plane | `McdCylinderPlaneIntersect` | yes |
| Cylinder × Sphere | `McdCylinderSphereIntersect` | yes |
| Cylinder × Cylinder | `McdCylinderCylinderIntersect` | yes |
| Cylinder × TriangleList | `McdCylinderTriangleListIntersect` | quarantined |
| Cylinder × ConvexMesh | `McdGjkCgIntersect` | yes (same function as the called GJK pairs) |
| Sphyl × Plane | `McdSphylPlaneIntersect` | yes |
| Sphyl × Box | `McdSphylBoxIntersect` | yes — had a real bug, fixed, §8 |
| Sphyl × Cylinder | `McdSphylCylinderIntersect` | yes |
| ConvexMesh × Plane | `McdGjkCgIntersect` | yes |
| Aggregate × {Null, Sphere, Box, Plane, Cylinder, Sphyl, TriangleList, ConvexMesh, Aggregate, …} | `McdAggregateGenericIntersect` + unidentified | no |

**Why the shape of that list makes sense.** UT2004 gives its physics actors sphere, sphyl,
convex-mesh and triangle-list geometry and essentially nothing else. It never uses Karma's
`Cylinder` or `Aggregate` types at all, and `Plane` is registered but the level is a
TriangleList, not a plane. So **whole objects in the "not compiling" pile are for collisions
the game never makes**, and object count is the wrong progress metric.

### How much to trust "never called"

Read it as "not in 25+ runs across 18 maps", not "impossible". **Two pairs have already come
off that list:**

- `ConvexMesh × ConvexMesh` — appeared with 39 calls on `ONS-UCMP-ABC-ECE`.
- `Box × Sphere` — appeared on 2026-08-23 with **5,101 calls in a single match**, on a map
  that had been run before. Not a trickle; a pair the game genuinely uses.

Both were already recovered and clean, so both cost nothing. That is luck, not a plan. The
cheap insurance is: **re-run the census on any new map before doing anything else with it**,
and if a pair moves, check whether its object is quarantined before trusting the match.

It cuts the other way too. `McdSphylBoxIntersect` had a real bug in a pair that is never
called — not wasted, because the fix was in shared code — and `IxSpherePlane` sits in the
validated set for another. **"Validated" is not "load-bearing"** without checking this table.

### Running a census sweep

`KD_CENSUS=1` is safe on anything; see §6 for the loop. The CSV lists **every registered
pair**, called or not, so the zero rows are evidence, not absence of evidence.

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
  --dump-dir /home/ion/tools/karma-lab/out6 \
  --obj-dir  /home/ion/tools/karma-lab/allobj \
  --out-dir  /tmp/kd_out \
  --metoolkit ../Thirdparty/metoolkit \
  --protos /home/ion/tools/karma-lab/kd_protos.h
```

Recovered `.c` lands in `/tmp/kd_out/allobj/`, objects in `/tmp/kd_build/`. `recover.py`
prints a per-object table and a summary. **`out6` is the current dump directory** (§5).
`out5` is the previous one and is kept deliberately — it is the fallback if a pipeline
change ever has to be bisected against the dumps.

### Gate what came out — all seven, every time

```bash
MT=../Thirdparty/metoolkit
LIB=$MT/lib.rel/linux_single_gcc3.2

# breadth: swap each object into a scene, diff the trajectory
./test/substitute_test.sh /tmp/kd_build $LIB test/scene_chain.c
./test/substitute_test.sh /tmp/kd_build $LIB test/scene_boxes_on_plane.c
./test/substitute_test.sh /tmp/kd_build $LIB test/scene_ragdoll.c

# depth: drive one interaction, 200k randomised transforms
./test/difftest_pair.sh /tmp/kd_build $MT            # all pairs
./test/difftest_pair.sh /tmp/kd_build $MT McdBoxBoxIntersect

# portability: §12 item 6, the actual goal
./test/wasm_check.sh /tmp/kd_out/allobj /tmp/kd_build $MT

# frame bounds: a defect no behavioural test can find, §8. Costs a second.
python3 tools/check_frame_bounds.py /tmp/kd_out/allobj

# interface: does each object export what the SHIPPED one exported — name,
# BINDING and size? §8. Costs two seconds and caught a global putchar.
python3 tools/check_symbol_bindings.py /tmp/kd_build /home/ion/tools/karma-lab/allobj

# and the two that say what the scene gate above actually proved — §4a
./test/scene_census.sh     /tmp/kd_out/allobj $LIB test/scene_chain.c
./test/gate_sensitivity.sh /tmp/kd_out/allobj $LIB test/scene_ragdoll.c
```

`difftest_pair.sh` needs the quarantined `IxBoxTriList` staged or it will not link
(`undefined reference to rec_McdBoxTriangleListIntersect`). Compile it into a **copy** of
`/tmp/kd_build` — not into `/tmp/kd_build` itself, or the other six gates start reporting
on an object the detectors are holding back:

```bash
cp -a /tmp/kd_build /tmp/kd_build_dt
INC=$MT/include
gcc -m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w \
    -Wno-int-conversion -Wno-incompatible-pointer-types -DLINUX \
    -Ikarma-decomp/include -I$INC -I$INC/McdCommon -I$INC/McdPrimitives \
    -I$INC/McdFrame -I$INC/MeGlobals -I$INC/MdtBcl -I$INC/MdtKea -I$INC/Mst -I$INC/MeApp \
    -c -o /tmp/kd_build_dt/IxBoxTriList.o /tmp/kd_out/allobj/IxBoxTriList.c
./test/difftest_pair.sh /tmp/kd_build_dt $MT
```

`scene_chain` is collision-free and is the authoritative *trajectory* signal.
`scene_boxes_on_plane` diverges by design but exercises the geometry dispatch — it caught
the `__regparm` parameter shift after the collision-free scene had passed it.
`scene_ragdoll` is a nine-capsule ragdoll on ball-socket joints dropped onto a plane and
boxes, because the other two make **not one Sphyl call** between them.

**A collision-free scene is not a solver-free scene.** `scene_chain` drives
`keaLCPSolver::solveLCP` 900 times, `MdtKeaAddConstraintForces` 900 times and
`keaPoolAlloc` 14,400 times — measured with gdb breakpoint counts on the stock build, not
assumed. It is the right instrument for `libMdtKea` and it reaches the LCP.

---

## 4a. What "trajectory bit-identical" does and does not mean

`substitute_test.sh` printed `trajectory bit-identical` for all 93 objects on all three
scenes, and §2 used to summarise that as "93/93 clean". That line has **three** possible
meanings and the gate cannot tell them apart:

1. the recovery reproduces the shipped code exactly;
2. the object never executed in this scene;
3. it executed, and nothing it computes reaches the numbers being compared.

For nine decompiled `libMdtKea` objects reproducing 900 steps of a chaotic twelve-body
chain to the last bit, (1) was not the likeliest reading. Two instruments separate them.

**`test/scene_census.sh`** — `kd_instr.c` counts every function entry through GCC's
`-finstrument-functions`, file-local functions included, with no sampling and no mutation
of the code under test. It is `KD_CENSUS` for the offline scenes, and it answers "ran" vs
"never ran" exactly. It also links every recovered object at once and diffs the result,
which is the combined question `substitute_test.sh` (one object at a time, by design)
never asks.

**`test/gate_sensitivity.sh`** — rebuilds the object with float intermediates forced to
storage precision and reports how far the trajectory then moves. That is roughly the
smallest error the scene could have caught.

**Read the sensitivity number as a ONE-SIDED test.** A non-zero delta proves the gate can
see the object. A zero **proves nothing**, and the worked example is worth carrying:
`keaCalcAcceleration_vanilla` reads zero under every rounding probe tried, and it is
emphatically not irrelevant — it computes `accel[i] = invmass * force[i]` plus a torque
dot product, and forcing those outputs to 7777 moves `scene_chain` by 8.8e5 m. One
multiply rounds identically at any precision and most torque terms in these scenes are
zero, so the arithmetic is exact and no rounding probe can reach it. Settle a zero by
mutating that object's outputs by hand.

### What the two instruments actually found

**Of 103 objects probed across all three scenes, exactly eight have demonstrated
sensitivity on any scene.** The scenes do not exercise most of the recovered set:

| object | scene where it is proven | recovered | sensitivity |
|---|---|---:|---:|
| `keaCalcConstraintForces_vanilla` | all three | 0 | 4.3e-04 … 6.8e+01 |
| `keaCalcIworldandNonInertialForceandVhmf_vanilla` | all three | 0 | 4.9e-04 … 1.1e+02 |
| `keaCalcJinvMandRHS_vanilla` | all three | 0 | 3.7e-04 … 4.0e+00 |
| `MdtUtils` | ragdoll | 0 | 4.1e+00 |
| `IxSphylPrimitives` | ragdoll | 3.677 | 4.0e+00 |
| `IxBoxBox` | boxes_on_plane | 69.8 | 2.0e+01 |
| `GeomUtils` | ragdoll | 4.222 | 3.8e+00 |
| `keaMatrix_PcSparse_vanilla` | chain | **4.28e-04** | 3.7e-04 |

Six of the eight released collision objects — `IxSphereTriList`, `IxSphereSphere`,
`McdGjk`, `IxConvexPrimitives`, `IxBoxSphere`, `IxConvexTriList` — are **not demonstrated
on any scene**. That is not a problem: their evidence is the shadow harness and
`difftest_pair`, which is the right tier for them. It does mean "93/93 clean on three
scenes" was carrying far more weight in the summary than it can bear, which is why §2 now
says what it says.

The last row is the one to look at twice. `keaMatrix_PcSparse_vanilla` is the only kea
object that does **not** reproduce the original: 4.28e-04 m of divergence against
3.70e-04 m of sensitivity, i.e. about one rounding step wrong. It is quarantined
(guessed stack frame) and must stay there.

### The combined test, and what it says about the quarantine

All 98 validated objects substituted **together**:

| scene | result |
|---|---|
| `scene_chain` (collision-free) | **bit-identical** over 900 steps |
| `scene_boxes_on_plane` | max delta 2.135e+01 m |
| `scene_ragdoll` | max delta 4.127e+00 m |

The two collision scenes are the documented unbounded post-contact divergence (§10), and
4.13 m on the ragdoll sits beside the **3.283 m that MathEngine's own i386 and x86-64
builds differ by on the same scene**. Nothing went non-finite; all three ran to
completion.

Adding the ten quarantined objects that compile turns `scene_chain` from bit-identical
into an immediate SIGSEGV with zero rows of trajectory. Bisecting: it is **`MdtPartition`**
— precisely the object §8 cites as the reason the guessed-stack-frame detector exists.
Every other quarantined object that compiles is harmless on this scene. That is the first
end-to-end measurement of the quarantine policy rather than an assertion about it.


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
export KARMA_OUTDIR=/home/ion/tools/karma-lab/out8      # a NEW directory
export KD_CALLSITE_SIG=trilist
rm -rf gproj8 && mkdir -p gproj8 out8
timeout 9000 /home/ion/tools/ghidra_12.1.3_PUBLIC/support/analyzeHeadless \
  gproj8 Proj -import /home/ion/tools/karma-lab/allobj \
  -scriptPath /home/ion/tools/karma-lab/gscripts \
  -preScript ParseKarmaHeaders.java \
  -postScript DumpDecomp.java -deleteProject
```

Takes 1–2 hours (`out6` took about 75 minutes for 153 objects). Scripts live in
`tools/gscripts/` and **must be copied** to `/home/ion/tools/karma-lab/gscripts/` —
Ghidra reads them from there. **Write to a NEW output directory** and keep the old one
until the new dumps have passed all **seven** gates; a re-run changes every object at
once — `out6` differs from `out5` in 103 of 153 dumps.

`out5`, `out6` and `out7` are all on disk. `out6` is current.

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
out5 (targeted)   88 clean + 4 TODO,  39 fail
```

**A second, narrower widening IS on by default, and it is not the same thing.**
`KD_FORCE_CDECL_UNKNOWN` forces `__cdecl` where Ghidra left the convention
**`unknown`** — never where it decided `__thiscall` or `__cdecl`, which is what the
blanket attempt got wrong. `keaRbdCore_unified` has all eleven of its functions in that
state, `keaMemory` 7 of 11, `keaIntegrate_pc` 1 of 3 — the objects blocking the solver.
`KD_FORCE_CDECL_UNKNOWN=0` turns it off for an A/B.

### The three re-runs of 2026-08-24, so nobody repeats them

| dump | what changed | WARNING | `in_stack_` | compiles |
|---|---|---:|---:|---:|
| `out5` | (previous) | 602 | 71 | 94 |
| **`out6`** | new protos + forced cdecl | **265** | 275 | **94** |
| `out7` | new protos only | 602 | 275 | 93 |

(The `compiles` column is what each dump set gave **at the time of the comparison**. Two
later fixes — an anonymous-typedef name and `<ctype.h>` — took the current figure to 95;
they are pipeline-side and apply equally to all three.)

`out6` is adopted, and the strongest thing that can be said for it is not in that table:
**all 94 compiled objects are byte-identical between the two dump sets.** 103 of 153
dumps changed and not one line of the code that actually compiles did. So every piece of
in-game evidence in `proven.txt` — gathered before the re-run — still applies to exactly
the objects it was gathered on. Classification-identical, all seven gates identical, and
602 → 265 decompiler warnings.

Two results in that table are worth carrying:

- The `in_stack_` rise is the **aggregate prototypes**, not the convention — `out7` has
  it without the forced cdecl. It is Ghidra modelling a by-value argument area honestly
  as read-before-written. Nothing the gates measure moves because of it.
- Forcing `__cdecl` on `unknown` is **load-bearing, not cosmetic**: `out7` shows that
  without it the new prototypes *cost* `keaCalcJinvMandRHS_vanilla` and
  `keaRbdCore_unified`.

### Regenerating `kd_protos.h`

Not previously written down, and there is a trap in it:

```bash
# Extract EVERY archive member first. allobj/ is NOT enough.
rm -rf /tmp/kd_allmembers && mkdir -p /tmp/kd_allmembers
for a in ../Thirdparty/metoolkit/lib.rel/linux_single_gcc3.2/*.a; do
    b=$(basename "$a" .a); mkdir -p "/tmp/kd_allmembers/$b"
    (cd "/tmp/kd_allmembers/$b" && ar x "$a")
done
python3 tools/gen_protos.py /tmp/kd_allmembers -o /home/ion/tools/karma-lab/kd_protos.h
```

**Generating it from `allobj/` alone silently loses 376 prototypes** — almost all qhull
(`qh_*`, `facetT`, `McdComputeHull`) from `libMcdConvexCreateHull`, whose members exist
only inside the `.a`. Every caller of those would go back to Ghidra guessing arity, which
is the exact failure this header exists to prevent. From all sixteen archives it is 2487
prototypes, a strict superset of what shipped: 0 lost, 269 gained.

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
ret_diff,count_diff,dims_diff,overrun,nonfinite,worst_delta`, plus a trailing `# max thunk
nesting depth` comment line.

- `ret_diff` / `count_diff` / `dims_diff` are **decisions**. Any non-zero is a real defect.
- `overrun` is worse: the recovered function wrote past the buffer it was handed. The
  scratch buffer carries a canary, because without one an overflow corrupts whatever is next
  and the engine dies somewhere unrelated a few frames later.
- `nonfinite` is a disagreement on an input the engine had already made NaN. Not a defect in
  either implementation — see below — but a sign the engine has a problem of its own.
- `fp_only` with a small `worst_delta` is float noise and expected.
- `$KD_SHADOW_DIVERGENCES` dumps the **full input transforms** for each divergence.

### `KD_SELFTEST=1` — run this before believing anything

Runs the **original as both sides**. Any divergence it reports is a bug in the harness.
Baseline: `test-karma-1` gives 1,763,102 calls, 0 divergence.

It settles crashes too, and skipping that cost half a day. A SIGSEGV in
`McdModelGetGeometryType` was written up as an overrun caused by `IxSphylPrimitives`. It is
not — it reproduces under `KD_SELFTEST=1` with no recovered code executing.

### The harness perturbed the engine, and the copy was in the wrong place

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

> **WITHDRAWN, 2026-08-24. That first row does not replicate.** Five 420 s runs of
> stock Karma with no harness on `ONS-UCMP-ABC-ECE` crash **twice**, in
> `McdModelGetGeometryType` under `KHandleCollisions` — the same site. The engine has a bug
> of its own there and 0-of-4 at 240 s on a different map was luck. §7a has the numbers.
>
> The rest of this section still stands: `m_cachedData` really was shared between the two
> implementations, and fixing it really did take the GJK count divergences from 15 to 2.
> But **the crash-rate argument was measuring the map, not the harness**, so do not cite
> "5-of-5 → 1-of-3" as evidence for anything.


A "copy of the `McdModelPair`" was added in response and reported as not helping. **It was
not doing anything.** The copy went into `scratch.pair` while the *real* pair was still
passed as the first argument to the recovered function — which is the argument every
intersection function actually uses. Everything the recovered code wrote still landed in
engine state. Read the code before trusting a negative result about it.

And copying the struct would not have been enough anyway, because `m_cachedData` is a
**pointer**: both calls write the same 60-byte block. `McdGjkCgIntersect` warm-starts from
that block, so on frame N the original stepped the cache and the recovered stepped it again,
and on frame N+1 the original read a cache the recovered had last touched — a feedback loop
between the two implementations through engine state.

Fixed 2026-08-23: the copy is now the first argument, and the cache block is rewound to what
the original consumed and restored to what the original left. `McdCacheHello` is the only
thing in the library that assigns `m_cachedData` and it takes the block from a fixed pool of
`0x3c`-byte elements, so 60 is the size of every cache block there is, not a guess.

What that changed, on `ONS-UCMP-ABC-ECE`, 900 s per run:

| | GJK divergences | ran the full 900 s |
|---|---|---|
| shared state | 3 ret + 15 count in 75,839 | **0 of 5** — all died in `KHandleCollisions` |
| isolated | 3 ret + 2 count in 65,975 | **2 of 3** |

**The right-hand column is now known to be confounded.** Stock Karma with no harness at
all crashes in `KHandleCollisions` 2 times in 5 on this map (§7a), so "ran the full 900 s"
was never a clean measure of the harness. The GJK column — 15 count divergences down to
2 — is unaffected and is the part of this result to keep.

Read that carefully rather than as a fix. The count divergences dropped 15 → 2 and the
crash stopped being universal, but `ret_diff` is unchanged at 3 and one isolated run still
crashed. The mechanism was real and it was not the whole story. `KD_SHARECACHE=1` exists so
the A/B stays cheap; the sample sizes here are three and five.

**Still open**, and worth keeping in mind before declaring this closed:

1. **A shared contact pool.** If an intersection function bumps a per-frame allocator as
   well as filling the caller's array, calling it twice double-counts it. Never tested, and
   now the leading candidate for what is left.
2. **More runs.** Two clean runs out of three is not a result at this crash rate.


Also **ruled out**: stack pressure. `kd_dispatch` puts ~2.9 KB on the stack per shadowed
call and aggregates dispatch to child pairs, so the frames could nest. They do not — the
harness counts the nesting and a full match reports **max depth 1**. Four minutes to measure
against the half hour a bisection trial costs.

Three switches exist for exactly this:

- **`KD_CENSUS=1`** — count calls, run nothing twice. Perturbs nothing measurable.
- **`KD_ONLY=<substring>`** — shadow only functions whose name contains it, no rebuild
  needed between attempts.
- **`KD_SHARECACHE=1`** — put the shared state back, for A/B against the isolation above.

### `nonfinite` — the engine hands Karma NaN, and it is not a defect

The CSV has a `nonfinite` column because a 900 s match produced 21 `McdSphylSphylIntersect`
and `McdSphylSphereIntersect` divergences in one early burst, all reading
`ret 1/0 touch 1/0 count 1/0` — the exact shape of a real tolerance bug — and every one of
them had `tm1` and `tm2` **entirely NaN**. A ragdoll had gone non-finite in the engine, well
upstream of Karma.

"Does this touch" has no right answer for a NaN transform, so two implementations answering
differently is not a defect in either, and scoring it as one manufactures structural
divergences against a released object with no way to tell them apart from the CSV. They are
counted separately now. **If `nonfinite` is non-zero, that is the engine's problem, not the
recovery's** — but do look at it, because a NaN body is a real bug somewhere.


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

**One trap that took four objects out of the build at once.** A `multiline` or `file_wide`
rule rewrites several lines from ONE diagnostic, but appended a single entry to `tried`,
keyed to the line GCC named. When the batch was rejected, the retry loop looked the owning
entry up by line number — and for every other line the rule touched there is no such
entry, so it raised `StopIteration`. `keaMemory`, `McdBox`, `McdTriangleList` and `MdtLOD`
all failed this way, and all four reported it identically because `recover.py` truncated
the traceback to boilerplate. Edits are now grouped per rule application; retrying half a
rewritten statement would have been wrong even where the lookup succeeded.

---

## 7a. Driving, not shadowing — the engine ON recovered Karma

**Definition-of-done item 7, for the collision layer, done on 2026-08-24.**

`test/make_substituted_metoolkit.sh` replaces archive members outright, so the engine
consumes the recovered code's answer every frame with no original to fall back on. All
eight objects behind the twelve pairs the census says the game calls went in together —
`IxSphereTriList`, `IxSphylPrimitives`, `IxSphereSphere`, `McdGjk`, `IxConvexPrimitives`,
`IxBoxBox`, `IxBoxSphere`, `IxConvexTriList`.

Check the substitution took, and do not take the link's word for it. All eight export
symbol sets identical to the shipped objects (only `sqrtf` and `__stack_chk_fail` newly
imported), the archive index resolves `McdBoxBoxIntersect` to the substituted member, and
— decisively — the *machine code* in the final binary differs for every interaction
sampled:

```
McdBoxBoxIntersect                  stock 419 insns   substituted 1117
McdSphereSphereIntersect            stock 241         substituted  205
McdConvexMeshTriangleListIntersect  stock 700         substituted  610
McdSphylTriangleListIntersect       stock 371         substituted  467
```

### The result, and a correction to §7

`test/crash_ab.sh` alternates two builds on the same map and URL, A,B,A,B, and keeps every
log. 420 s per run, **two maps**, every run reaching kickoff:

| map | runs/arm | stock crashes | substituted crashes | crash site |
|---|---:|---:|---:|---|
| `ONS-UCMP-ABC-ECE` | 5 | **2** | **2** | `McdModelGetGeometryType` |
| `ONS-CBP2-Tropica` | 6 | **2** | **2** | `McdModelGetGeometryType` |
| **total** | **11** | **4** | **4** | |

**Indistinguishable, on both maps, at the same crash site.** Recovered collision code
drives a real Onslaught match with bots and vehicles exactly as well as the shipped
library does. Mean run length 366 s stock against 354 s substituted on Tropica.

And the other half of that table is a correction. §7 recorded the `KHandleCollisions` →
`McdModelGetGeometryType` SIGSEGV as something the shadow harness caused, on the strength
of "stock Karma, no harness: 0 of 4". **It happens to stock, unmodified, shipped Karma
with no harness at all — four times in eleven runs, on two different maps.** The original
measurement was four runs of 240 s on `ONS-UCMP-ABC`; this is eleven runs of 420 s across
`ONS-UCMP-ABC-ECE` and `ONS-CBP2-Tropica`.

That does not make §7's cache-isolation work wrong — `m_cachedData` really was shared, and
the GJK count divergences really did fall from 15 to 2 when it stopped being. It does mean
**the crash-rate half of that argument was measuring the map, not the harness**, and the
"5-of-5 → 1-of-3" figure should not be cited as evidence for anything. The engine has a
bug in its own `KHandleCollisions` and it is not ours.

### If you re-run this

- **`Signal:` is not a crash.** `timeout --signal=TERM` ends a clean run and the engine
  logs `Signal: SIGTERM [terminate]`. Matching on `Signal:` alone marks every good run as
  a crash — it did, for a whole sweep, and the numbers had to be re-derived afterwards.
  Match `SIG(SEGV|BUS|ILL|FPE|ABRT)`.
- Alternate the arms. The crash is situational: run 1 of both arms went the full distance
  and run 5 of both crashed.
- Keep every log (`crash_ab.sh` writes `/tmp/kd_ab_<arm>_<run>.log`) so a verdict can be
  re-derived without re-running seventy minutes of matches.
- Eleven runs per arm across two maps is enough to say "the same" and nowhere near enough
  to say "4 in 11 is the rate".

### What this does NOT show

The **solver** is still shipped in that build. `libMdtKea`, `libMdt` and `libMst` are
stock; only the collision objects are recovered. Item 7 asks for **no shipped `.a` in the
link at all**, and that is blocked on §11 items 1–3. What is settled is that the recovered
collision layer can drive, which the shadow harness structurally could not tell you.

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
| **changed symbol binding** | `putchar` shipped `W`, recovered `T` | `tools/check_symbol_bindings.py`. Not a `recover.py` detector — a gate, because it needs the shipped object to compare against. See below |

`proven.txt` records which objects a real match has released, **with the evidence on the
line**. That is the only way out. Do not remove a detector to make a number go up.

**The quarantine has now been measured, not just argued for.** Substituting all 98
validated objects into `scene_chain` at once is bit-identical over 900 steps; adding the
ten quarantined objects that compile turns that into an immediate SIGSEGV. It is
`MdtPartition` — the object the guessed-stack-frame detector was written for. §4a.

### A weak symbol that comes back global takes over libc

`gcc` emits `putchar` **weakly** into three separate members of `libMdtKea.a` —
`keaDebug.o`, `keaMatrix_tester.o` and `keaPrintBasicTypes.o` — precisely so that libc's
strong definition wins and none of them is ever used. `ghidra_clean.object_symbols()`
collapsed `T` and `W` into one "exported" set, so the recovered `keaMatrix_tester`
exported it **global**, and decompiled Karma silently became the `putchar` the whole
engine calls. It was linked and executed 901 times in a scene census before anything
noticed. Recover a *second* of the three and the link fails outright with a duplicate
symbol — a latent blocker for §12 item 7.

Two C++ functions had it for the same reason: `CylPerpAndPara` and `BracketedRootN` are
defined in headers, so gcc emits them weakly in every translation unit, and weak is how
the ODR gets enforced at link time. Fixed by carrying the binding through to the
declaration as `KD_WEAK`.

**`wasm_check.sh` had been comparing exported symbols for months and could never have
caught this**, for two reasons worth remembering when adding a gate: it compared *names*
with the binding letter thrown away, and it compared the recovered wasm build against the
recovered native build — never against the object being replaced.

`check_symbol_bindings.py` also audits the data-section moves rather than waving them
through. Ten `IxCylinderCylinder` statics, `MdtContactInvalidID` and the four `MeMessage`
levels all move `.data`/`.rodata` → `.bss`, which loses the initialiser if the shipped
bytes are not zero. They are zero — but the tool checks the bytes **and** that no
relocation targets the symbol, because `MeMessage`'s handler table is an all-zero `.data`
filled in by ten `R_386_32` relocations at link time, and a bytes-only check would have
called it benign. Two `.rodata` → `.data` moves are reported as tolerated.


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

### The same defect, a second time, in the TYPE DATABASE

`IxConvexTriList`'s promoted double came from `typedef int code();` in the generated C.
On 2026-08-24 the identical defect turned up somewhere else entirely, and it had been
there just as long: `dwarf_structs.declarator()` rendered every function type with an
**empty parameter list**, so `kd_types.h` defined the broadphase's AABB-update callback as

```c
typedef void (*McdUpdateAABBFnPtr)();      /* no prototype */
```

`CxSmallSort` calls it with a `MeReal`, and the recovered object emitted

```
flds  0x54(%esp)      ; load the float
fstpl (%esp)          ; store it as an 8-byte DOUBLE
call  *0xc8(%edx)
```

against a callee that takes a `float`. The shipped `CxSmallSort.o` contains **zero**
`fstpl` instructions.

`declarator()` now writes the parameter list out. Two things worth taking from this:

- **It was found by fixing a generator, not by testing.** No behavioural gate in this
  project would have caught it — `CxSmallSort` is the broadphase's sorted-pair helper and
  none of the three scenes reaches that callback.
- **Look for the third one.** The pattern is any place the pipeline emits a function type
  without a prototype. `dwarf_structs.type_name()` still returns the literal string
  `'void (*)()'` for a subroutine type used as a struct MEMBER — it has no name to build a
  declarator around. **Checked: zero occurrences in the emitted header today**, because
  every such member in this corpus goes through a named typedef, which `declarator()`
  handles. It is a latent hazard, not a live bug — `grep -c 'void (\*)()' include/kd_types.h`
  should stay at 0, and if it ever does not, a member called through with a float argument
  has exactly this defect.


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

### `IxBoxBox` — reproduced, once the boxes stopped being the same shape

**1 count divergence in 1,299 real calls** — both agree the boxes touch, original says 2
contacts, recovered says 4, at world (259.9, 8.0, 10.2). For two sessions it did not
reproduce: 300,000 synthetic pairs near the origin and 200,000 at `KD_ORIGIN=260`, all
clean. `proven.txt` guessed the reason and the guess was right — **the driver used one fixed
pair of box sizes for its whole life.** Two boxes of fixed proportion approaching at random
angles is narrower than it looks, because the ratio of the half-extents decides which
features can meet at all.

`difftest_pair.c` now resizes every box geometry each iteration (`KD_FIXEDSHAPE=1` restores
the old behaviour), and the divergence class appears:

| `KD_SPREAD` | touching | count divergences in 500,000 |
|---:|---:|---:|
| 1.0 | 18.6% | 1 |
| 1.5 | 6.9% | 1 |
| 2.5 | 1.8% | **0** |

Two shapes, one per regime: a deep 10-vs-11 with different feature classification at
separations of half a box, and — at the shallow end, which is the game's regime — a
`count 1/0`, a single grazing contact the original finds and the recovered does not, both
agreeing that the bodies touch. That second one is the same tolerance-flapping family as
`IxSphylPrimitives`' remaining 1-in-74,921, not a structural error.

The same change also made `McdGjkCgIntersect` show **2 count divergences in 200,000** where
a fixed box had given none at every regime, which is a synthetic counterpart to the in-game
GJK entry in `proven.txt`. A fixed shape was a blind spot for both.

### `McdGjk` — 18 divergences on the busiest pair, and where most of them went

The first long run of Box × ConvexMesh — 72,167 calls, four times any before it — found
**3 ret_diff and 15 count_diff**, worst delta 2.057, all of them the same two actors over
consecutive frames of one persistent contact. GJK warm-starts from `m_cachedData`, and the
harness was letting both implementations write that block (§7). Isolating it took the same
map to **3 ret_diff and 2 count_diff in 65,975** across three runs — most of it, and not all
of it. The synthetic driver agrees there is something left: 2 count divergences in 200,000
Box × ConvexMesh pairs once the box dimensions vary, on a driver that hands a zeroed pair
every iteration so no cache is involved at all. Reproducible now without waiting for a
match. See `proven.txt`.


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
9. **Cycling `fix_pointer_as_float` through the casts on a line until one is accepted.**
   It works, in the sense that `McdSphyl` then compiles, and what it compiles is wrong.
   The line is `*radius = (float)s[1].mRefCtAndID + (float)s[1].prev;` and GCC rejects
   only the second cast, so cycling rewrites that one and leaves the first as an
   integer-to-float **conversion**. The original does no such thing —
   `flds 0x10(%ecx); fadds 0x14(%ecx)` — both operands are floats, and Ghidra mis-typed
   the first member. This is dead end 6 turned around: there the compiler was the only
   thing that knew, and here **the compiler is wrong, because it is reasoning about
   Ghidra's types.** `McdSphyl` stays in the FAIL pile, where the defect is visible; it
   is not the free object it looks like.
10. **Renaming exported DATA symbols in bodies the way functions are renamed.** The gap
   is real — `gen_prelude` emits `void *kd_boxDraw[72] KD_MANGLED("boxDraw")` and the
   bodies still say `boxDraw`, so `McduDebugDraw` fails on nothing else. But applying the
   same rename machinery to data costs more than it gains: it took `McdGjk` (released, on
   the busiest pair in the census) and `MeMessage` out of the build, because a body that
   CALLS through an exported pointer — `(*MeInfoShow)(...)` — resolves through the public
   header, and rewriting it to `kd_MeInfoShow` calls a `void *`. Net −2 objects for +1
   debug-draw object. If you retry it, exclude any name the body dereferences or calls.
11. **Trusting a test whose callback ignores its arguments.** `difftest_pair`'s triangle
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
validated against a live match. The next milestone is **§12 item 7 — the engine running ON
recovered Karma**, and as of 2026-08-24 the collision half of that has been measured
(§7a). What blocks it now is the **solver**, and the solver's blockers are four named
objects rather than a vague expanse.

Ordered by what actually moves the project, not by what is easiest:

1. **`keaRbdCore_unified` — the solver driver.** This is now the single highest-value
   object in the project and it did not look like it before. Three of the recovered kea
   kernels are proven bit-identical (§2, `proven.txt`), and the reason they still cannot
   drive anything is that the object which *calls* them does not compile. Ghidra lost its
   frame completely: the body reconstructs `MdtKeaConstraints` and `MdtKeaParameters`
   field by field out of a copied stack block, and every call into the recovered kernels
   is `(**(code **)(_vanillaFunctions + 0x10))()` — a `keaFunctions_Vanilla` vtable
   dispatch with **every argument dropped**.

   That is a Ghidra-side problem with existing machinery pointed at it. `DumpDecomp.java`
   already applies `McdTriangleListFnPtr` at indirect call sites by function-name match
   (`KD_CALLSITE_SIG=trilist`, §5); the vtable slots of `keaFunctions_Vanilla` are known
   (`gen_vtables.py` recovers C++ ABI data, and the kernels' own mangled names give the
   signatures). Applying slot signatures at `(_vanillaFunctions + N)` call sites is the
   same trick. Budget a Ghidra re-run (1–2 hours, §5, **new output directory**).

2. **`keaMemory` — the allocator**, 14,400 calls per 900 solver steps. It used to crash
   the pipeline; that is fixed and it now reports 13 real errors, twelve of which are one
   defect. `pool_ptr`, `pool_max`, `poolstack` and `poolstack_ptr` are four **contiguous**
   globals in `keaRbdCore_unified.o`'s `.bss` (offsets 0, 0xc, 0x10, 0x14) and Ghidra
   resolved relocations-with-addend against the wrong neighbours — §5. Line 52 pushes to
   `&poolstack + poolstack_ptr*4` and line 61 pops from `&pool_ptr + poolstack_ptr*4`:
   the same address expression against two differently mis-resolved bases. **Declaring
   the four symbols makes it compile and leaves it wrong** — it needs the per-function
   symbol inversion, not a prelude entry.

3. **`keaLCPSolver` — now one fix away from the same place as item 1.** It used to fail on
   `implicit declaration of PrincipalSubmatrix`, because `ghidra_clean`'s rename map was
   keyed on the flattened declarator while Ghidra writes intra-class call sites with the
   bare method name. **Fixed**; the object moved FAIL → review (13 errors → 8) and
   `CxSmallSort` came into the validated set with it. What holds `keaLCPSolver` now is
   `_vanillaQMatrix` — the **same vtable-slot mislabelling as `keaRbdCore_unified`**, so
   items 1 and 3 are one shared fix, not two.

   `keaLCP_new` — `keaLCPSolver::solveLCP` itself — is short by exactly one argument on
   **nine** calls. That looks like a dropped `this` and is not: Ghidra lost the alloca
   frame and emits the call sequence's own stack effects as C assignments, including the
   return address. `proven.txt` has the worked example. **Do not write a text rule for
   it** — the missing arguments are at non-uniform frame offsets, and this is the exact
   shape the guessed-stack-frame detector exists for.

**All four solver blockers are the same underlying problem: Ghidra cannot model these
frames.** `keaRbdCore_unified` has by-value structs of 92 and 76 bytes plus all-`unknown`
conventions; `keaIntegrate_pc` and `keaLCP_new` allocate dynamically; `keaMemory`'s
globals collide in the EXTERNAL block. None is a text-level repair, and three separate
attempts to make one are recorded above and in `proven.txt` so they are not repeated. The
work is Ghidra-side, in `DumpDecomp.java`.

4. **Audit the other callbacks the way the triangle generator was audited** (§8, §12). The
   triangle generator is still the only stub in `difftest_pair.c` that depends on its
   arguments. The allocator and `McdCacheHello`/`Goodbye` have the same exposure and have
   never been checked. Cheap, with a track record.

5. **Chase what is left of the `IxBoxBox` and `McdGjk` divergences** (§8). Both reproduce
   synthetically once box dimensions vary — 1 and 2 count divergences in 200,000 — so they
   can be worked on deterministically, without waiting for a match.

6. **Settle the sensitivity zeros that matter.** §4a lists which objects the scenes cannot
   see. For anything you intend to rely on, mutate its outputs and confirm the gate moves
   before quoting a bit-identical result — `keaCalcAcceleration_vanilla` is the worked
   example of a zero that means nothing.

7. **Grind the tail.** 31 objects, but **read §3 first** — a large part of the pile is
   geometry the game never collides, and **nine of the 34 are one error from
   compiling**, which is where to start. The distribution, re-measured 2026-08-24:

   | diagnostic | count | what it is |
   |---|---:|---|
   | `request for member X in something not a structure` | 135 | almost all `McdSpace`, below |
   | `X undeclared` | 102 | mixed |
   | `unknown type name` | 34 | mostly `MeASE*`, the asset loader |
   | `invalid use of undefined type` | 23 | 22 of them `struct _McdSpace`, below |
   | `subscripted value is neither array nor pointer` | 22 | DebugDraw/XML statics |
   | `too few arguments` | 32 | the lost-frame family; see §11.3 |

   **Two corrections to what this section used to say.** `BodyData` is fixed — it was
   `typedef struct { ... } Foo;`, which has no `DW_AT_name` on the aggregate. And the
   advice "`gen_typedb.py` takes object directories, so widening its input is the route"
   is **wrong**: the input is already all 192 archive members, and the remaining missing
   names are not there to be found. `weightingData` is a variable, not a type;
   `Mesh2GeometryType` is under an archive `--exclude` skips on purpose.

   **`McdSpace` is not a quick win, despite being load-bearing.** 51 errors, and 22 of
   them are `invalid use of undefined type 'struct _McdSpace'`. The layout is not in the
   DWARF *anywhere*, including in `McdSpace.o` itself, where the DIE carries
   `DW_AT_declaration : 1`. It would have to be inferred from the code, which is exactly
   the kind of guess the detectors exist to stop. Same for `MeSimpleFile_linux`: GCC
   rejects `open(filename, unaff_EBX, unaff_ESI, in_EDX)` through glibc's fortify check,
   and `-D_FORTIFY_SOURCE=0` makes it compile with three garbage arguments. Leave it
   failing — the compile error is the useful signal.

8. **Replace, don't recover:** `libMcdConvexCreateHull` is qhull 2.6 (1998) — 186 KB,
   load-time only, open source. Swap in modern qhull. `MeAssetDB`/`MeXML`/`MeAssetFactory`
   (51 KB) is `.ka` XML parsing, not physics. `MeViewer2`/`MeApp` (74 KB) are never linked.


### What needs the project owner, and nothing else will do

Almost everything here can be pushed on alone. **Maps cannot.** The census (§3) is only as
good as the maps it has seen, and two pairs have already moved off the never-called list
because someone ran a map nobody had tried. The specific asks, in order of value:

- **A map that exercises `Box × TriangleList`.** `IxBoxTriList` is quarantined and, we now
  know, badly wrong — 139,961 count divergences in 200,000 synthetic pairs. Zero calls on
  every map run so far. If the game ever reaches it, that object is a live crash risk rather
  than a curiosity, and right now there is no way to find out.
- **A map that exercises `Cylinder` or `Aggregate` geometry**, if any exists. Those are
  whole families of registered-but-never-called pairs. If the answer is "UT2004 never uses
  them", that is worth knowing definitively, because it retires a large chunk of the
  not-compiling pile permanently.
- **More community maps generally.** `CBP2`/`UCMP`/`BE-`/`SPAC-` reach pairs Epic's
  optimised maps never do; that fact came from the project owner and it has been the single
  most productive operational input to this project. `ONS-UCMP-ABC-ECE` is still the ONLY
  known map that reaches `ConvexMesh × TriangleList`.

Everything else — code, tests, measurement, tooling — is self-service.

---

## 12. What "complete" looks like

**Complete** is not "every object recovered". It is:

1. Every object the census (§3) shows the game *actually calls* is recovered and validated.
   **That is twelve pairs, and all twelve are done** — the last, `IxConvexTriList`, on
   2026-08-23. This is the item that was open for the whole project; treat any new entry in
   the census (§3 has had two) as re-opening it, and re-run the census on any new map.
2. Validated means: 0 `ret_diff`, 0 `count_diff`, 0 `dims_diff`, 0 `overrun` across a
   multi-hour in-game session, with `KD_SELFTEST` clean on the same session, and
   `proven.txt` carrying the evidence.
3. All three `substitute_test.sh` scenes clean for every recovered object — **and, per
   §4a, checked with `scene_census.sh` and `gate_sensitivity.sh`, because "clean" on a
   scene that never ran the object is not a result.**
4. qhull and the asset loader replaced rather than recovered.
5. No detector suppressed, no object released without a line in `proven.txt`.
6. The whole set builds as ordinary C for **wasm32 and arm64/armv7**, not just i386.
   **wasm32 is done** — 98/98 compile with byte-identical exported symbols
   (`test/wasm_check.sh`). arm64 has not been tried; no cross-compiler is installed here.
   Nothing has been *executed* under wasm. See `HANDOVER-WEB.md`.
7. The engine runs with `WITH_KARMA=1` against recovered Karma with **no shipped `.a` in the
   link at all**. `test/make_substituted_metoolkit.sh` builds that tree.
   **The collision half is done** — all eight objects behind the twelve called pairs, in
   the driving seat, through full ONS matches (§7a). The solver half is not, and cannot be
   until §11 items 1–3 compile.

With (1) closed, **(7) is the next real milestone** and the one that actually delivers
physics on the web. The shadow harness structurally cannot test it: it feeds the engine the
original's answer every frame, so a recovered error never gets to compound. Item 2's
in-game numbers say the recovered code *agrees*; item 7 asks whether it can *drive*.


### Where the project actually stands — read this before estimating anything

**Done, to a real standard:** the collision-detection *interaction* layer, for the twelve
pairs the game calls. Recovered, compiling for i386 and wasm32, each one measured against
the shipped original on real inputs from a live match, evidence in `proven.txt`.

**The solver is no longer "not started", and that is a correction to what this file used
to say.** Three of `libMdtKea`'s compute kernels — `keaCalcJinvMandRHS_vanilla`,
`keaCalcConstraintForces_vanilla`, `keaCalcIworldandNonInertialForceandVhmf_vanilla` —
reproduce the shipped library **bit-for-bit over 900 compounding steps** on all three
scenes, against a gate measured to be sensitive to 4e-4 m on the chain and 2.8–113 m on
the ragdoll and the boxes. That is a stronger standard than the collision gate in one
respect: it is 900 iterations of feedback, not a single discrete decision. `MdtUtils` is
evidenced the same way on the ragdoll scene.

The earlier reading — "zero objects validated" — was not pessimism, it was the gate being
unable to distinguish a bit-identical recovery from an object that never ran (§4a). Both
statements were derived from the same green output.

**What is genuinely still missing is the solver's plumbing, and it is four objects:**
the driver (`keaRbdCore_unified`), the allocator (`keaMemory`), the integrator
(`keaIntegrate_pc`) and the LCP (`keaLCPSolver` + `keaLCP_new`). Each runs 900 times per
900 steps and none compiles. Until they do there is **no configuration in which the engine
runs on recovered kea**, however good the kernels are. §11 items 1–3 are those objects,
with each blocker diagnosed down to the line.

**Never executed on wasm.** 98/98 compile with byte-identical exported symbols. Not one
instruction has run. See `HANDOVER-WEB.md`.

**Run end to end, now, for the collision layer** — §7a. Not for the solver.

So the honest summary is: **the collision layer is proven and drives a real match; the
solver's arithmetic is proven and cannot yet be reached; the solver's control flow is
untouched.** Do not read 98/148 as 66% of the way there — the denominator is wrong in both
directions (a third of those objects are for collisions the game never makes, and the four
kea objects in §11 are worth more than the other 38 put together).

### The thing that should shape how you work

The three biggest findings of 2026-08-23 — the promoted-double radius, the `IxBoxBox`
divergence, the GJK divergence — were all invisible for the same reason: **the tests were
not testing.** The three biggest findings of 2026-08-24 have the same shape:

- `substitute_test.sh` reported `trajectory bit-identical` for 93 objects, and for **95 of
  103 probed** that line was about the link, not the code (§4a).
- `wasm_check.sh` compared exported symbols for months while discarding the binding
  letter, so a recovered object exporting a **global `putchar`** passed every gate and ran
  901 times in a scene census (§8).
- `recover.py` truncated tracebacks to 90 characters, so **four objects** — including the
  solver's allocator — sat in the FAIL column behind an error message that was pure
  boilerplate, all of them the same single bug in the repair loop's retry path.

None of these was a hard problem. All three were invisible because the output looked like
success. Before trusting a green result, ask:

- Does the stub the code calls back into actually *depend* on the arguments it is given?
  (`KD_GENARGS=1` answers this for the triangle generator. Nothing answers it for the
  allocator or `McdCacheHello`/`Goodbye` yet.)
- Is any input fixed for the whole run that the game varies? (Box dimensions were, for the
  project's whole life. Hull vertex counts and triangle counts still are.)
- Does the harness share mutable state with the thing it measures? (It did, through
  `m_cachedData`, for months.)
- **Did the code under test even execute, and could the measurement have seen it if it
  were wrong?** (`scene_census.sh` and `gate_sensitivity.sh` answer these two. Run them
  before quoting a bit-identical result.)
- **Does the failure message identify the failure?** (Four objects said "Traceback (most
  recent call last):" and nothing else.)

That checklist is worth more than the next ten objects.


