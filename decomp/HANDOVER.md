# HANDOVER — Karma decompilation, for whoever picks this up next

You are resuming a project to recover Karma (MathEngine `metoolkit`, UT2004's physics
library) from shipped binaries as portable C. Read this whole file before touching
anything. It is written for someone with no memory of how any of it came to be.

Branch: **`karma/decompile`**. `main` is untouched. **Do not merge.**

### Read this first — the thirty-second version

- Karma is UT2004's physics library, shipped binary-only. We are recovering it as portable
  C from the DWARF the shipped `.a` files carry. §1.
- **The collision layer drives a real match** — for the twelve pairs the census had seen.
  All twelve are recovered, measured against the shipped original on live inputs, and the
  engine has run on them with no shipped `.a` for those objects. §3, §7b, `proven.txt`.
- **The census moved again on 2026-08-24 — three more pairs — and both cylinder pairs have
  now been MEASURED.** `IxCylinderTriList` came out of quarantine clean: 71,417 real calls
  and 200,000 synthetic pairs, 0 structural divergences, self-test clean. **`IxCylinderCylinder`
  came back WRONG** — 925 `dims_diff` in 24,111 real calls — **and no detector holds it, so
  it is in the build.** That is the sharper finding: the quarantine only catches what a
  detector recognises. §3, `proven.txt`.
- **The cylinder map ask is answered and withdrawn** — not by finding a map to ask for, but
  by parsing the assets (`tools/find_cylinder_geom.py`): 114 cylinder collision elements
  ship, in stock maps. **There are no outstanding asks that need the project owner.** §3.
- **The solver's virtual calls now carry their arguments** — the blocker this file used to
  call "the whole job". All nineteen vtable dispatches in `keaRbdCore_unified` and
  `keaLCPSolver` are restored from relocation data, and it cost only 2 changed dumps out of
  153. §5a, §11 item 1.
- **What now blocks the solver is one thing: arguments that are genuinely missing, because
  the DWARF does not say where the frames live.** Not "Ghidra failed to use the debug info"
  — the `DW_AT_location` attributes are absent from the abbrevs those functions use. §11
  item 2 has the readelf command that shows it. `MdtBcl`, `MeMath` and `MdtWorld` are now
  blocked on exactly this too — at **one, one and ten** errors, far cheaper subjects to
  validate a fix on than any kea object. §13.
- **109 objects compile**, 25 are quarantined by detectors, 14 do not. Object count is a bad
  progress metric — read §3 before using it.
- **They compile for wasm32, armv7 AND arm64 — and arm64 is a lie.** Identical symbol sets
  on all three, but arm64 emits 920 pointer/int conversion diagnostics against armv7's 23,
  because the recovery puns pointers through 4-byte slots and arm64 pointers are 8. No gate
  can see it. §6b.
- **All 108 go into the engine at once and it plays a match**, indistinguishable from stock
  on the same map. That is up from the eight of §7b, and getting there took one real fix: a
  static pointer table whose relocations were never applied, which passed all seven gates
  and then segfaulted on the engine's first `.ka` file. §7c.
- **One whole family the engine never asks Karma about**, and one that looked like a second.
  Every `Aggregate` *pair* is intercepted by UT2004's own dispatcher, as is
  `Box × TriangleList`. But the aggregate dispatcher **recurses into `KIntersect` per
  element**, so aggregate ELEMENTS reach Karma normally — which is exactly how the cylinder
  pairs above arrive. §3a.
- **The most important habit here is checking that a test is testing.** Most of the real
  bugs in this project were found that way, not by writing new code — and on 2026-08-24 it
  also caught a *search* that could not have found what it was looking for. §4a, §12's
  closing section, and dead ends 9 and 10.

### WHAT REMAINS — the whole list, ordered, 2026-08-24

Everything else in this file is detail. This is the work.

| # | what | where | blocked on |
|---|---|---|---|
| 1 | **The solver's frames.** 8 objects: `keaRbdCore_unified`, `keaMemory`, `keaIntegrate_pc`, `keaLCPSolver`, `keaLCP_new`, `MdtWorld`, `MdtBcl`, `MeMath`. The DWARF declares no `DW_AT_location`. **The untried move is a Ghidra re-dump with DWARF *variable* import disabled** — Ghidra's native stack recovery may beat the un-placeable variables its importer creates. | §11 item 2 | nothing. Ghidra is installed, a re-run is 75–120 min, and `MdtBcl`/`MeMath` are ONE error each so the answer is immediate |
| 2 | **`IxCylinderCylinder` is wrong and is in the build.** 925 `dims_diff` in 24,111 real calls, a SAT tie-break at `OverlapCylCyl:1275`. No detector holds it. | §11 item 0 | nothing |
| 3 | **arm64 truncates pointers.** Compiles 109/109 with identical symbols and is not trustworthy. armv7 is fine. | §6b | a generator-wide change to pointer-width slots |
| 4 | **Nothing has EXECUTED on wasm32, armv7 or arm64.** | `HANDOVER-WEB.md` | the web agent |
| 5 | **GJK's warm cache path has never been tested** — the busiest pair family in the census, cold-path only. | §11 item 4 | nothing |
| 6 | **The tail: 14 objects, 271 errors — and it is finished.** 3 DEAD, 2 leave-alones, 3 dead end 9, 4 are item 1 above, 2 low-value profilers. | §13 | do not start here |

**IS A COLLISION PAIR USED? YES — THIS IS ANSWERED, DEFINITIVELY, AND IT IS §3.**
37 pairs are registered. **15 are called.** Of the 22 that are not:

- **2 families CANNOT EVER be called**, proven from the engine source, not from a census:
  `Box × TriangleList` and every `Aggregate` PAIR are intercepted by `KIntersect` before
  Karma is consulted (§3a). No map can change this.
- **the rest have simply never been seen** in 30+ runs across 20+ maps. That is not proof,
  and the list has moved **five times** — most recently three Cylinder pairs on
  2026-08-24, one of which landed on a quarantined object. **Re-run the census on any new
  map before trusting anything about it.**

14 of the 15 called pairs are validated against the shipped original on live inputs. The
fifteenth is item 2 above.

Reproduce the whole state in about a minute with §4.

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
compile:  109 objects (103 clean + 6 with prelude TODOs)  = 73.6% of 148 attempted
scenes:   109/109 run clean on all three substitute scenes, and all 109 TOGETHER
          are bit-identical on the collision-free one — but read §4a before
          reading anything into that number
wasm32:   109/109 compile, exported symbol sets byte-identical to i386
armv7:    109/109 compile, symbol sets identical — a real 32-bit-pointer port
arm64:    109/109 compile, symbol sets identical, and NOT TRUSTED (§6b)
bindings: 109/109 export what the SHIPPED object exported, binding included (§8)
difftest: 14 pairs (Cylinder x Cylinder and Cylinder x TriangleList added
          2026-08-24), reproducing the documented baseline exactly — IxBoxBox
          1 count, IxSphereTriList 137 dims, IxCylinderCylinder 1 count + 20
          dims, quarantined IxBoxTriList as documented, the rest 0
review:   25 objects held back by recover.py's eight safety detectors (§8;
          the ninth, symbol bindings, is a gate rather than a detector because
          it needs the shipped object to compare against)
fail:     14 objects do not compile
dumps:    out9 is current (§5a). It differs from out8 in 52 of 153 dumps, and
          every object that already compiled is byte-identical.
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

**37 interaction pairs are registered on a typical map. The game calls fourteen of them.**
25 of the 37 have a recovered counterpart staged; **thirteen of the fourteen that are called
do, and the fourteenth is quarantined.**

Produced with `KD_CENSUS=1`, which counts calls and runs nothing twice, so it perturbs
nothing and can be pointed at any map safely (§6). Call counts below are the running total
over every instrumented match to date; treat the ORDER as solid and the absolute numbers as
"how much traffic this pair gets", not as a constant.

### Is a pair used? The definitive answer, three ways

This is the question that decides everything, so here is the whole answer in one place.
Three independent instruments, and they are **not** interchangeable:

| instrument | answers | limit |
|---|---|---|
| the census (`KD_CENSUS=1`) | did it EXECUTE on the maps we ran | only as good as the maps seen; **four** pairs have already moved off "never called" |
| `tools/reachable.py` (§3b) | can the LINKER reach the object at all | interaction functions are address-taken by their registrars, so it says "reachable" for pairs the game never calls |
| `tools/find_cylinder_geom.py` (§3) | does any shipped ASSET define this geometry | only tells you the content exists, not that it is simulated — `DM-Insidious` has a cylinder element and never collides it |
| **the engine source** (`KIntersect`, §3a) | **can the game reach it, ever** | **not none, and this file used to say none.** Only as good as how far you follow the call graph — `KIntersect` dispatches to `KAggregateGenericIntersect`, which calls `KIntersect` again. §3a |

**Settled permanently, from the engine source:**

- **`Box × TriangleList` — CANNOT fire.** `KIntersect` calls the engine's own
  `KBoxTriangleListIntersect` instead. `IxBoxTriList` is measurably wrong and it does not
  matter: it is dead code. The long-standing request for a map that exercises it is
  **withdrawn as unanswerable**.
- **Every `Aggregate` PAIR — CANNOT fire.** Same dispatcher, `KAggregateGenericIntersect`.
  The family needs no recovery. **But read §3a before drawing the obvious conclusion: that
  dispatcher recurses into `KIntersect` once per ELEMENT, so aggregate elements reach Karma
  normally.** That is how the cylinder pairs below arrive.

**Settled by 25+ runs across 18 maps, but NOT proven impossible** — these are registered
and have never been seen to execute. Treat as "no evidence of use", not "cannot be used":
`Sphere × Plane`, `Box × Plane`, `Box × Cylinder`, `Cylinder × {Plane, Sphere,
ConvexMesh}`, `Sphyl × {Plane, Box, Cylinder}`, `ConvexMesh × Plane`.

**The cylinder question is CLOSED, and the answer was yes.** It stood open for several
sessions as "the only surviving map ask". It did not need a map:

- **The code path is real and it fires.** `Cylinder × TriangleList` 59,366 calls and
  `Cylinder × Cylinder` 24,267 calls in a 300 s census on `test-simple-physics`, alongside
  `Sphyl × TriangleList` 95,544 — so the match was ticking and the counts are in the same
  range as pairs we already trust.
- **Shipped assets define cylinders.** `tools/find_cylinder_geom.py` parses the packages:
  **114 cylinder collision elements** across the tree (vs 107 sphere, 172 box, 32,191
  convex), in stock maps including `AS-BP2-Acatana`, `DM-Insidious`, `ONS-CBP2-Mirage`,
  `DOM-UCMP2-1on1-Smelter`.
- **Whether *stock gameplay* ever simulates one: five maps say no.** A census sweep of the
  stock maps my parser says contain cylinder elements — `DM-Insidious`, `ONS-CBP2-Mirage`,
  `DM-Urotsukidoji`, `DOM-UCMP2-1on1-Smelter`, `AS-BP2-Acatana` — produced **zero cylinder
  calls on every one**, while ticking properly (Smelter alone: 6 pairs, 91,618 calls;
  Mirage: Box × ConvexMesh 19,737). So shipped maps carry the geometry and do not simulate
  it, and the pair is reached by `test-simple-physics.ut2`, a custom map. Not proof — a
  sixth map could differ — but it is five maps of evidence, and nothing depends on the
  answer now that both objects have been measured.

**And grep cannot answer questions like this**, which is worth knowing before trying.
`FKAggregateGeom` is serialised by a hand-written `operator<<` with no field names, so
`CylinderElems` appears in no package — and neither does `BoxElems`, nor `KarmaProps`.
Searching for it returns 0 across all 2,015 packages and reads exactly like "UT2004 ships
no cylinders". What caught it was searching for something that MUST be there and getting
zero for that too. §4a's rule, applied to a search instead of a test.

### Called — twelve are recovered and validated, two are not

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
| **Cylinder × TriangleList** | **71,417** | `McdCylinderTriangleListIntersect` | `IxCylinderTriList` | ✅ **RELEASED 2026-08-24** — 71,417 real + 200k synth, 0 structural |
| ConvexMesh × TriangleList | 49,064 | `McdConvexMeshTriangleListIntersect` | `IxConvexTriList` | ✅ released 2026-08-23 |
| **Cylinder × Cylinder** | **24,111** | `McdCylinderCylinderIntersect` | `IxCylinderCylinder` | ⛔ **MEASURED WRONG — 925 dims_diff in 24,111 real calls, and NOTHING HOLDS IT** |
| **Cylinder × ConvexMesh** | **244** | `McdGjkCgIntersect` | `McdGjk` | ✅ already released; a fifth pair off the never-called list |
| Sphere × ConvexMesh | 9,741 | `McdGjkCgIntersect` | `McdGjk` | ✅ |
| Box × Sphere | 5,154 | `McdBoxSphereIntersect` | `IxBoxSphere` | ✅ |
| ConvexMesh × ConvexMesh | 4,337 | `McdGjkCgIntersect` | `McdGjk` | ✅ |

### Never called — zero, on every map, in every run so far

| pair | function | recovered? |
|---|---|---|
| Box × Plane | `McdBoxPlaneIntersect` | yes |
| Box × Cylinder | `McdBoxCylinderIntersect` | yes |
| Box × TriangleList | `McdBoxTriangleListIntersect` | quarantined and **badly wrong** — but **unreachable**, §3a |
| Sphere × Plane | `McdSpherePlaneIntersect` | yes |
| Cylinder × Plane | `McdCylinderPlaneIntersect` | yes |
| Cylinder × Sphere | `McdCylinderSphereIntersect` | yes |
| Cylinder × ConvexMesh | `McdGjkCgIntersect` | yes (same function as the called GJK pairs) |
| Sphyl × Plane | `McdSphylPlaneIntersect` | yes |
| Sphyl × Box | `McdSphylBoxIntersect` | yes — had a real bug, fixed, §8 |
| Sphyl × Cylinder | `McdSphylCylinderIntersect` | yes |
| ConvexMesh × Plane | `McdGjkCgIntersect` | yes |
| Aggregate × {Null, Sphere, Box, Plane, Cylinder, Sphyl, TriangleList, ConvexMesh, Aggregate, …} | `McdAggregateGenericIntersect` + unidentified | no — and **unreachable as a pair**, §3a |

**Why the shape of that list makes sense — and where it was wrong.** UT2004 gives its
physics actors sphere, sphyl, convex-mesh and triangle-list geometry mostly, `Plane` is
registered but the level is a TriangleList rather than a plane, and Box × TriangleList and
Aggregate PAIRS are intercepted by the engine before Karma is consulted (§3a). That much
holds. What this section used to add — "It never uses Karma's `Cylinder` type at all" — was
an inference from the census, and it is **false**: 114 cylinder collision elements ship, and
two cylinder pairs have now been measured executing. **Whole objects in the "not compiling"
pile are still for collisions the game never makes**, so object count remains the wrong
progress metric; just do not read a census zero as a proof.

### If one of those pairs ever fires, which are dangerous?

Registered-but-never-called is not the same as harmless. Ranked by what happens on the
first real call:

| pair | if it fires |
|---|---|
| **Box × TriangleList** | **CANNOT FIRE — the engine bypasses it.** `IxBoxTriList` is quarantined *and* measurably wrong (139,961 count / 12,060 dims divergences in 200,000 synthetic pairs; the ret/touch count varies run to run — 2,254 and 1,463 have both been recorded), but UT2004 never dispatches to it: `KIntersect` calls its own `KBoxTriangleListIntersect` instead. §3a. It is dead code, not a hazard. |
| ~~Cylinder × TriangleList~~ | **NO LONGER HYPOTHETICAL — it fires, 59,366 times. Moved to the called table above.** This row used to read "quarantined; never measured. Genuinely unreached rather than bypassed — same posture, unknown magnitude." Two of those three clauses were right and the important one was wrong. |
| Sphyl × Box, Sphyl × Plane, Sphere × Plane, Box × Plane, Box × Cylinder, Cylinder × {Plane, Sphere}, ConvexMesh × Plane, Cylinder × ConvexMesh | recovered, compiling, clean on the synthetic driver — `McdSphylBoxIntersect`, `McdSphylPlaneIntersect` and the GJK variants are all in the difftest and read 0 structural. These would be fine. |
| Aggregate × anything | **not recovered, and does not need to be** — the engine bypasses this family too, §3a. A closed gap, not an open one. |

The honest one-liner: **fourteen pairs are used; twelve of them are validated, and the two
that are not are both Cylinder pairs found on 2026-08-24 — one of them quarantined. Of the
twenty-three that are not used, two families (Box × TriangleList and Aggregate pairs) are
structurally unreachable because the engine implements them itself, and the rest would be
fine.**

### 3a. Two families the engine never asks Karma about — and the hole in that

`Source/Engine/Src/KFarfield.cpp`, `KIntersect()` at line 936, is the **single** dispatcher:
the only two references to `interactions->intersectFn` in the whole engine are its null
check and its call. And before that call it intercepts two cases:

```c
if(type1 == kMcdGeometryTypeAggregate || type2 == kMcdGeometryTypeAggregate)
    result->touch = KAggregateGenericIntersect(p, result);
else if(type1 == kMcdGeometryTypeBox && type2 == kMcdGeometryTypeTriangleList)
    result->touch = KBoxTriangleListIntersect(p, result);
else
    result->touch = (*interactions->intersectFn)(p, result);   // <- Karma
```

So `McdBoxTriangleListIntersect` and `McdAggregateGenericIntersect` are **registered and
unreachable**. That is exactly what the census has been reporting for 25+ runs across 18
maps, and the zero is now explained rather than merely observed — **no map can move these
off the never-called list.**

> ### ⚠ AND HERE IS THE HOLE, found 2026-08-24
>
> "The engine intercepts Aggregate, therefore aggregate geometry never reaches Karma" does
> **not** follow, and this file asserted it for two sessions. `KAggregateGenericIntersect`
> is not a terminal handler. It loops over the aggregate's elements and, for each one that
> overlaps, **calls `KIntersect` again** (`KFarfield.cpp:878`) with a dummy pair whose
> model1 is the ELEMENT:
>
> ```c
> for(i=0, element = ins->child; i<g->elementCountMax; i++, element = element->next)
>     if (g->elementTable[i].mGeometry && McdGeometryInstanceOverlap(instance2,element))
>     {
>         dummyModel.mInstance = *element;
>         KIntersect(&dummyPair, &dummyResult);      // <- back into the dispatcher
>     }
> ```
>
> On that inner call `type1` is the element's real type — Cylinder, Sphere, Box, ConvexMesh
> — so it matches neither interception and goes **straight to Karma's `intersectFn`**.
>
> This is not a corner case. `KUtils.cpp:762` wraps EVERY primitive collision element in an
> `McdAggregate`, with **no single-element shortcut**, so every sphere, box and cylinder
> collision volume in the game arrives at Karma through this path.
>
> What survives: the Aggregate PAIR itself never reaches Karma, so
> `McdAggregateGenericIntersect` still needs no recovery, and Box × TriangleList is still
> intercepted at whatever depth it occurs. What does not survive: any argument of the form
> "the geometry is in an aggregate, so Karma never sees it."

Two consequences worth carrying:

- **`IxBoxTriList` is not a live crash risk.** `proven.txt` withdrew its clean result and
  §8 recorded it as "a defect waiting for a map". There is no such map — the interception
  is by TYPE PAIR and applies on the inner call too. It stays quarantined and stays out of
  `/tmp/kd_build`, but as dead code rather than as a hazard.
- **The Aggregate family is not a coverage gap.** §12 item 1 asks for every pair the game
  calls; the game never calls Karma for an Aggregate *pair* at all. Its elements are
  another matter, and they are counted under their own types in §3's table.

The general lesson, now with a correction attached: **the census tells you a pair is not
called; the engine source tells you whether it *can* be — if you read the whole call
graph.** Reading `KIntersect` was right. Stopping at `KIntersect` was not.

### How much to trust "never called"

Read it as "not in 25+ runs across 18 maps", not "impossible". **Four pairs have already
come off that list:**

- `ConvexMesh × ConvexMesh` — appeared with 39 calls on `ONS-UCMP-ABC-ECE`.
- `Box × Sphere` — appeared on 2026-08-23 with **5,101 calls in a single match**, on a map
  that had been run before. Not a trickle; a pair the game genuinely uses.
- `Cylinder × TriangleList` — **59,366 calls**, 2026-08-24, on `test-simple-physics`.
- `Cylinder × Cylinder` — **24,267 calls**, same run.

The first two were already recovered and clean, so both cost nothing. **The third was not:
`IxCylinderTriList` is quarantined by the reconstructed-frame detector and has never been
measured.** So the run of luck is over, and the insurance was already written down here:
**re-run the census on any new map before doing anything else with it, and if a pair moves,
check whether its object is quarantined before trusting the match.**

It cuts the other way too. `McdSphylBoxIntersect` had a real bug in a pair that is never
called — not wasted, because the fix was in shared code — and `IxSpherePlane` sits in the
validated set for another. **"Validated" is not "load-bearing"** without checking this table.

### Running a census sweep — how to answer "is this pair used?" from scratch

`KD_CENSUS=1` is safe on anything; see §6 for the loop. The CSV lists **every registered
pair**, called or not, so the zero rows are evidence, not absence of evidence.

```bash
BIN=/home/ion/engines/engine-ut2004/build-shadow-karma/Source/SDLLaunch/ut2004-karma-pixo.bin
KD_CENSUS=1 KD_BIN=$BIN KD_SHADOW_OUT=/tmp/census_$MAP.csv \
  timeout 260 ./test/run_map.sh "$MAP" 200 "$COMMON_URL" >/dev/null 2>&1
awk -F, 'NR>1 && $5+0>0 {print $1"x"$2, $5}' /tmp/census_$MAP.csv
```

Column 5 is the call count. A pair that prints is used on that map; a pair in the CSV with
0 is registered and not used; a pair absent entirely is not even registered, which has
never happened. **Check the `Game class is` line in the log before believing a zero** — a
match that never kicked off registers everything and calls nothing, and looks exactly like
a map with no physics (§6).

## 3b. What is in scope at all — reachability, and its two different meanings

**Karma is being recovered FOR UT2004, not reproduced.** An object nothing reaches does not
need to compile, and saying "out of scope" is a better answer than leaving it in a tail
that never shrinks. But "reachable" means two different things and neither subsumes the
other:

| question | instrument | what it misses |
|---|---|---|
| can the linker reach it? | `tools/reachable.py` | interaction functions are ADDRESS-TAKEN by their registrars, so every one counts as reachable even if the game never calls it |
| does the game execute it? | the census, §3 | only what the maps it has seen happen to do |

`tools/reachable.py` walks a symbol-level closure from the engine's own object files
through all 192 archive members. Result: **147 reachable, 31 not.** The 31 retire
permanently:

- `MeViewer2`/`MeApp`/`Render_*`/`RMenu`/`RMouseCam`/`RGeometry*`/`RConvex`/`Init_ogl` —
  MathEngine's own demo viewer, never linked.
- `McduDebugDraw` — **and this one is worth noting**, because dead end 10 spent effort
  trying to make it compile. It is unreachable, so that work would have been worth zero
  even had it succeeded.
- `MeASELoad`, `MeFGeometryFromMesh`, `MeCommandLine`, `MeMisc`, `MeString`, `MeBounding`,
  `MePrecision`, `MeSimpleFile`, `MeMemoryCpp`.
- `MdtFixedPath`, `MdtLinear1`, `MdtLinear2`, `MdtSpring`, `MdtUserConstraint` — Karma's
  other CONSTRAINT types. This is the joint-side analogue of the collision census: UT2004
  does not use them.

That takes the failing pile from 27 to **24 genuinely in scope** (22 after `MeChunk` and
`MeXMLParser` were fixed later the same day), and it re-sorts them:

| group | objects | verdict |
|---|---|---|
| **core Karma** | `McdBatch` `McdBox` `McdContact` `McdSpace` `McdSphere` `McdSphyl` `McdTriangleList` `MdtBcl` `MdtWorld` `McdMessage` `MeMath` `MeProfile` (~~`MeChunk`~~ — fixed) | required — geometry types and dynamics core |
| ~~**`.ka` asset loading**~~ | ~~`MeAssetDBXMLIO` `MeAssetDBXMLInput_1_0` `MeAssetFactory` `MeFAsset` `MeXMLOutput` `MeXMLParser`~~ | **DONE — all 9 compile**, and it turned out RECOVERABLE rather than replace-only. §8c |
| solver | `keaLCP_new` | required, §11 item 2 |
| platform / misc | `MeProfile_linux` `MeSimpleFile_linux` `mesffnmin` | low value |
| unknown | `McdCylinder` | reachable (`KUtils.cpp:796` calls `McdCylinderCreate` from a cylinder collision element) but the census has never seen a cylinder pair called. Depends on whether any shipped asset defines one — this is the surviving map question |

### 8c. The `.ka` asset cluster — recovered, not replaced

**All nine objects compile**: `MeChunk`, `MeXMLParser`, `MeXMLTree`, `MeXMLOutput`,
`MeAssetDB`, `MeAssetDBXMLIO`, `MeAssetDBXMLInput_1_0`, `MeAssetFactory`, `MeFAsset`.

§11 item 8 called for replacing this. It turned out to be **recoverable**, and much more
cheaply than replacing it: 37 compile errors across seven objects, but only ~5 distinct
causes, each a generator fix that also helped elsewhere.

| cause | fix | also fixed |
|---|---|---|
| calls through API structs dropped their arguments | §5a, 412 sites | `MdtLOD`, `MeProfile` |
| `ghidra_memory_map` honoured empty sections' alignment | §5's own example disproved it | 4 objects' data refs |
| `__ctype_b_loc` is glibc-only | portable table in `kd_compat.h` | wasm for the whole cluster |
| the alloca idiom written across two statements | `materialise_alloca_frame` | `MdtLOD` |
| `(MeFAsset *)` — an explicit star — never matched `ANY_CAST` | separate pattern | — |
| relocated `.rodata` tables | `materialise_relocated_data` | — |
| a zeroed static has no evidence of its own type | DWARF, not the bytes | — |

**The last two are worth carrying**, because both are about where type information actually
lives:

- A **relocated** `.rodata` word's value is in the relocation record, not the bytes — the
  section holds a zero. `materialise_data_refs` refuses such sections for exactly that
  reason, so rebuilding them needed a sibling that applies the relocations. Karma's XML
  handler tables are entirely this: `{"ASSET", MeXMLActionCallback, Handle_Asset_1_0, …}`
  then an `MeXMLActionEnd` sentinel, verified word for word against the object.
- A **zero-initialised static** carries no evidence of its own type, so guessing from the
  bytes gave `static float x = 0.0f;` for what is really
  `MeFAssetCreateFromFile MeFAssetCreateFunc[1]` — a function-pointer hook something
  installs at run time. That is why Ghidra subscripts it. The object's own DWARF says so.

**None of the nine is validated** — most are quarantined by the detectors, which is the
right state. Compiling is the start of the evidence chain, not the end.

### The asset loader is not peripheral

§11 item 8 and §12 item 4 describe `MeAssetDB`/`MeXML`/`MeAssetFactory` as "`.ka` XML
parsing, not physics", which is literally true and misleading. It is how **every ragdoll and
vehicle gets instanced**:

```
KCreateAssetDB()   -> MeAssetDBCreate() -> MeAssetDBXMLInputCreate()
                   -> parses ../KarmaData/*.ka
KSkeletal.cpp:388  -> KAssetInstanceCreate(level->KAssetFactory, skelAss, ...)
```

So "replace, don't recover" is still the right strategy — it is a bounded XML parser, not
physics — but it is **not optional** for §12 item 7. Six of the 24 failing objects are this
cluster, and it is the largest single group.

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
  --dump-dir /home/ion/tools/karma-lab/out9 \
  --obj-dir  /home/ion/tools/karma-lab/allobj \
  --out-dir  /tmp/kd_out \
  --metoolkit ../Thirdparty/metoolkit \
  --protos /home/ion/tools/karma-lab/kd_protos9.h
```

**`kd_protos9.h`, not `kd_protos.h`** — it is `kd_protos.h` with the API-struct
prototypes appended; see §5a. Using the plain one silently loses 412 call-site
signatures.

Recovered `.c` lands in `/tmp/kd_out/allobj/`, objects in `/tmp/kd_build/`. `recover.py`
prints a per-object table and a summary. **`out9` is the current dump directory** (§5).
`out8` is the previous one and is kept deliberately — it is the fallback if a pipeline
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
# KD_CENSUS_VALIDATED is not optional: without it this sweeps in the quarantined
# objects too, and MdtPartition alone segfaults the scene before any census is
# written (§4a). A gate that cannot pass is not a gate.
KD_CENSUS_VALIDATED=/tmp/kd_build \
  ./test/scene_census.sh   /tmp/kd_out/allobj $LIB test/scene_chain.c
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

All 99 validated objects substituted **together**:

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

Takes 1–2 hours (`out6` took about 75 minutes for 153 objects; `out8` about the same). Scripts live in
`tools/gscripts/` and **must be copied** to `/home/ion/tools/karma-lab/gscripts/` —
Ghidra reads them from there. **Write to a NEW output directory** and keep the old one
until the new dumps have passed all **seven** gates; a re-run changes every object at
once — `out6` differs from `out5` in 103 of 153 dumps.

`out5`–`out9` are all on disk. **`out9` is current.**

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

### `out8` — vtable call-site signatures, and a re-run that changed almost nothing

`out8` is current. It adds one thing: **the C++ virtual calls in `keaRbdCore_unified` and
`keaLCPSolver` now carry their arguments.** That was the blocker §11 used to call "the
whole job", and §5a explains the machinery.

**It differs from `out6` in 2 of 153 dumps** — only the two targeted objects. Compare
`out5 → out6`, which changed 103 of 153. Nothing else in the corpus moved, no
classification changed, and all seven gates read exactly as before. A re-run does *not*
have to be a big-bang: this one was surgical because the change is applied at 19 specific
addresses rather than to a global setting.

### `out9` — the API-struct call sites, and 412 recovered allocation sizes

`out9` is current. It extends the call-site machinery from C++ vtables to the other
indirect-call shape in this corpus, and that one is far larger: **412 sites against 19.**

Karma reaches its allocator and pools through file-scope structs of function pointers, and
gcc sets the first argument up with `push` but every subsequent one by rewriting the same
outgoing slot:

```
push $0x5c            ; first call: Ghidra sees an argument
call *0x4             ; R_386_32 MeMemoryAPI -> createZeroed
movl $0xc,(%esp)      ; second call: same slot, no push
call *0x0             ; -> create ... and the size is DROPPED
```

An allocation with no size is a real defect. GCC's arity check is the only reason it
surfaces, which also means **nothing that already compiled could have been hiding it** —
and indeed all 101 previously-built objects came through byte-identical.

`ApiStructs` in `gen_vtable_callsites.py` resolves offset → member → signature from the
referencing object's **own DWARF**, which carries the struct declaration even though the
struct is defined elsewhere. No hand-written table. A free confirmation that the mapping is
right: the six `MeMemoryAPI` slots the corpus actually calls (`+0/4/8/c/10/14`) are exactly
the six members the struct declares.

**52 of 153 dumps changed and not one already-compiling object's `.o` moved.** Error counts
only fell — `MeFAsset` 10 → 4, `MeAssetFactory` 7 → 3, `MdtLOD` 9 → 4, `MeProfile` 83 → 79.
Zero objects got worse.

> **A reclassification that looks like a regression.** `MdtLOD` went `review → FAIL` here
> and that is not a step backwards: it never compiled in either dump. `recover.py`
> classifies by the FIRST error's pattern, and the mislabelled-symbol error that this fix
> resolved was masking a `stack0x` one underneath. Check the error COUNT, not the label.

### `KARMA_VTABLE_CALLSITES` — the environment for a re-run

`out8` needs one more variable than §5's recipe. Generate the table first, then export it:

```bash
cd /home/ion/tools/karma-lab
python3 .../tools/gen_vtable_callsites.py allobj \
    -o kd_vtable_callsites9.txt --protos-out /tmp/kd_api_protos.h
cat kd_protos.h /tmp/kd_api_protos.h > kd_protos9.h      # <-- BOTH halves
export KARMA_VTABLE_CALLSITES=/home/ion/tools/karma-lab/kd_vtable_callsites9.txt
export KARMA_PROTOS=/home/ion/tools/karma-lab/kd_protos9.h
```

Without the table `DumpDecomp.java` prints `VTABLE: KARMA_VTABLE_CALLSITES not set,
skipping` and you silently get `out6` behaviour back.

**And the prototypes half is just as load-bearing, in a way that is easy to miss.** One
whole Ghidra run was spent finding out that appending prototypes rendered *faithfully* —
`void *f(size_t)`, `void f(struct MePool *)` — does nothing: `kd_protos.h` is FLAT and
declares no typedefs (§5 records why), so Ghidra's C parser rejects both. **The failure is
silent where it counts**: the header still parses, the other 2487 prototypes still load, and
the only symptom is `no prototype for kd_MeMemoryAPI_destroy` in the run log with all 412
sites skipped. They are rendered with `gen_protos.simple_type` now, so each one parses
standalone — `grep 'kd_Me.*API_' kd_protos9.h` should show 16.

## 5a. Recovering the arguments to a C++ virtual call

**This is how §11 item 1 was solved, and the technique generalises.**

Ghidra has no signature for a call through a function pointer, so it drops **every
argument**. For the solver driver that was not cosmetic — it was the whole reason the
object could not be recovered:

```c
before   (**(code **)(_vanillaFunctions + 4))();
after    (**(code **)(_vanillaFunctions + 4))
                   (&vanillaFunctions,mem.invIworld,mem.vhmf,blist,tlist,
                    num_bodies,parameters.stepsize);
```

which is `calcIworldandNonInertialForceandVhmf`'s demangled signature exactly.

The mechanism is `HighFunctionDBUtil.writeOverride`, the same one
`KD_CALLSITE_SIG=trilist` already used. What was missing was *which* signature, because
the vtable lives in a different object and therefore a different Ghidra program. So the
resolution happens in Python — `tools/gen_vtable_callsites.py` — and Java just consumes a
table of addresses. `ParseKarmaHeaders` has already loaded `kd_protos.h` into the
DataTypeManager, so the method name is enough to find the prototype.

A site is resolved from the two-instruction idiom gcc 3.2 emits:

```
mov  -0xb8(%ebp),%ecx        ; the vptr, out of a local
call *0x10(%ecx)             ; slot +0x10 of that class's vtable
```

by walking back to the `mov` that defined the register and looking the frame offset up in
the vptr stores `vtable_slots.vptr_stores()` reads out of the **relocations**. Nothing is
inferred from decompiled text.

**Why it is allowed to be believed** — a wrong signature here calls the right function
with the wrong arguments and still compiles, so every step is checked:

- `vptr_stores()` refuses any store whose addend is not the **+8 ABI address point**, so a
  multiple-inheritance secondary vtable cannot pass as a primary one. Ghidra's own ELF
  loader independently agrees, logging `External Location = _ZTV20keaFunctions_Vanilla+0x8`.
- the slot must **exist** in that class's vtable, read from relocation records in the
  object that defines it. An offset that is not a slot is refused, not rounded.
- the backward register walk refuses if anything else writes the register in between.
- the 19 resolved sites reproduce the 19 dispatches in the old dump **one-for-one**, and
  the slot mapping agrees with `scene_census.sh`'s count of the four kernels running 900
  times each in 900 steps — a measurement taken months earlier for an unrelated reason.

**The bug worth not repeating.** The first version computed the site as `section base +
function offset + call offset`. objdump's offsets are **section**-relative, so the
function offset was counted twice. `keaRbdCore_unified` hid it completely — its only
relevant function is at offset 0, where the two agree, and it read 9/9 applied — while
`keaLCPSolver`'s later sites were thrown past the end of `.text` into `.eh_frame` and
**six sites were being applied at addresses that were simply wrong**. `_is_indirect_call()`
now checks the bytes (`0xFF` with reg field `/2`) before the address is used. The general
lesson is the one in §12: the object that motivates the tool is the worst possible thing
to validate it on.

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

> ### ⚠ CHECK THIS FIRST — the asset tree unmounts on reboot
>
> `/home/ion/ut2004-assets` (~19 GB, `$UT_ASSETS_DIR`) is a **mount, and it drops on
> reboot**, leaving the directory empty. Every bulk-content symlink in `/home/ion/karma-run`
> then dangles and **no map run of any kind works** — the census, the shadow harness,
> `crash_ab.sh`, every in-game measurement in this file. **Only the project owner can
> remount it.** Ask; do not try to work around it.
>
> ```bash
> ls /home/ion/ut2004-assets      # empty => stop and ask for a remount
> ```
>
> **The symptom does not look like an asset problem**, which is the reason for this box. The
> engine dies during startup, long before the level loads, with a backtrace that reads like
> an engine bug:
>
> ```
> Warning: Failed to load 'AS_FX_TX': Can't find file for package 'AS_FX_TX'
> Signal: SIGSEGV
>   ULinkerLoad::FindExportIndex / ULinkerLoad::Create / UEngine::Init
> ```
>
> `run_map.sh` reports `gametype actually used: ?` and writes no CSV, which is
> indistinguishable from the "started but never ticked" case below. It is not that. Check
> `System/UT2004.log` for `Can't find file for package` before anything else.
>
> **Do not repoint the symlinks at `/home/ion/epic-sources/ut2004-v3186-assets`.** Tried on
> 2026-08-24: it gets further — `AS_FX_TX` resolves — but the engine is build 3369 and that
> tree is 3186, so 3369-era packages are missing (`ONSNewTank-A.ukx` stops it, and it exists
> nowhere else on disk), and it has no community content, so the CBP2/UCMP maps §3 and §7b
> rely on would not work even at a matching version.

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

## 6b. arm64 and armv7 — one of them is a lie

**Measured 2026-08-24 (fourth session), and the headline is that the green result is the
dangerous one.**

The toolchain is the Android NDK, which IS installed — an earlier session wrongly reported
no cross-compiler because it only looked at `/usr/bin` and `/opt`:

```bash
NDK=/home/ion/Android/Sdk/ndk/30.0.14904198/toolchains/llvm/prebuilt/linux-x86_64/bin
$NDK/armv7a-linux-androideabi21-clang   ...   # 32-bit pointers
$NDK/aarch64-linux-android21-clang      ...   # 64-bit pointers
```

with the same flags §4 uses for i386, minus `-m32`.

| target | compiles | exported symbols vs i386 | pointer/int diagnostics* |
|---|---|---|---:|
| wasm32 | 109/109 | identical | — |
| **armv7** | **109/109** | **identical** | **23** |
| **arm64** | **109/109** | **identical** | **920** |

\* with `-Wno-int-conversion` and `-Wno-incompatible-pointer-types` REMOVED, over five
released objects (`IxSphereTriList`, `McdGjk`, `IxBoxBox`, `IxConvexTriList`,
`IxSphylPrimitives`).

**Why arm64 must not be trusted despite passing everything.** §4 downgrades those two
diagnostics deliberately, and says why: they are "safe only because every target is
32-bit-pointer (i386 and wasm32)". Decompiled code puns pointers through integer slots
constantly — Ghidra recovers a stack slot as `undefined4` and the code stores a pointer in
it. On a 32-bit target that is lossless. **On arm64 it truncates a 64-bit pointer to 32
bits**, and the 40× jump in diagnostics is exactly that happening, 897 more times.

It compiles. The symbol sets are byte-identical. Every one of the seven gates would pass,
because not one of them executes arm64 code. This is §12's closing checklist in its purest
form: *the measurement cannot see the failure.*

**So the honest status is: armv7 is a real port, arm64 is not.** armv7 shares the
32-bit-pointer assumption the whole recovery rests on, so it is in the same class as wasm32
and i386. arm64 needs the pointer punning fixed at the source — Ghidra's `undefined4` slots
would have to become pointer-width — which is a generator-wide change, not a flag.

**Nothing has been executed on any non-i386 target.** Compiling is not running; see
`HANDOVER-WEB.md`, which says the same thing about wasm.

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
> of its own there and 0-of-4 at 240 s on a different map was luck. §7b has the numbers.
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
all crashes in `KHandleCollisions` 4 times in 11 across two maps (§7b), so "ran the full 900 s"
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

## 7b. Driving, not shadowing — the engine ON recovered Karma

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

## 7c. All 108 objects at once — and the defect that only this found

**2026-08-24 (third session).** §7b substituted the eight objects behind the called
collision pairs. This substitutes **everything that compiles — all 108** — which is a much
larger claim and it took one real fix to get there.

```bash
./test/make_substituted_metoolkit.sh /tmp/kd_build ../Thirdparty/metoolkit /tmp/mt_subst
cmake --preset native-karma -B build-subst108 -DMETOOLKIT_DIR=/tmp/mt_subst
cmake --build build-subst108 -j"$(nproc)"
```

**Result: the engine initialises, loads `DM-Insidious`, reaches `START MATCH` under
`xDeathMatch`, and plays for 300 s.** Against a stock control on the same map and duration
it is *indistinguishable*: same exit status, same gametype, and the same pre-existing HUD
fault (`UCanvas::DrawTileStretched` → `FCanvasUtil::DrawTile` → `FCanvasVertex`) at the
same four frames. Substitution verified at the machine-code level, not from the link —
`McdBoxBoxIntersect` 416→1116 instructions, `McdSphereSphereIntersect` 239→204,
`keaFunctions_Vanilla::calcJinvMandRHS` 145→355 — so recovered **solver arithmetic** is in
this binary too, not only collision.

**The defect this found, and nothing else could have.** At 108 objects the engine
segfaulted in `fread` during `KCreateAssetDB`, before any match, with a backtrace made
entirely of SHIPPED functions. `MeFileSearch`'s `MeDefaultFileLocations` is
`const char *[22]` and its `.rodata` bytes are relocation ADDENDS, so emitting them raw
gave the file-search loop 0, 1, 0xc, 0x1a as directory prefixes. Fixed in
`gen_prelude.static_reloc_definition`. **It passed all seven gates before and after** —
the offline scenes never open a `.ka` file, so no gate could see it. Same shape as
`IxSphereTriList`: compiles, gates clean, dies on first real use.

**`test/try_subst.sh`** is the bisect harness that found it — link a chosen subset, run 60 s,
report. Two things about it are worth keeping:

- its success test is **"reached `START MATCH`"**, not "did it SIGSEGV". The stock engine
  also faults under Xvfb in `UCanvas::DrawTile`, so a SIGSEGV grep reports a crash for a
  build that started a match perfectly well. That cost a cycle before it was noticed, and
  it is §12's checklist again: *the measurement could not distinguish the two outcomes.*
- bisecting is affordable because the crash is at **init**, so 60 s settles each cycle.

**What this still does not show.** No trajectory comparison — the substituted build carries
no shadow instrumentation, so "indistinguishable" here means it starts, runs and ends like
stock, not that its contacts match. The four kea objects in §11 item 2 are still shipped,
so this is not item 7 either. But it is the largest recovered surface the engine has ever
run on, and it is reproducible in about four minutes.

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

**The quarantine has now been measured, not just argued for.** Substituting all 99
validated objects into `scene_chain` at once is bit-identical over 900 steps; adding the
quarantined objects that compile turns that into an immediate SIGSEGV. It is
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
12. **Validating an address-computing tool on the object it was written for.**
   `gen_vtable_callsites.py` computed call sites as `section base + function offset + call
   offset`; objdump's offsets are section-relative, so the function offset was counted
   twice. It read **9/9 applied** on `keaRbdCore_unified`, because that object's only
   relevant function starts at offset 0 and the two formulas coincide there. The second
   object put six signatures at addresses inside `.eh_frame`. Check an address against the
   bytes it is supposed to point at, and pick a validation subject where the wrong answer
   would *differ* (§5a).
13. **Expecting the DWARF to describe a frame just because it names the variables.** For
   `MdtKeaAddConstraintForces` the abbrevs carry `DW_AT_name` and `DW_AT_type` and **no
   `DW_AT_location`** — while other functions in the same object have them. Before planning
   any frame-recovery work, run the `readelf --debug-dump=abbrev` in §11 item 2 and find out
   whether the information exists at all.
14. **Grepping an Unreal package for a field name.** `FKAggregateGeom` is serialised by a
   hand-written `operator<<` (`Engine/Inc/KTypes.h:195`) that writes four `TArray`s in a
   fixed order and **no field names**, so `CylinderElems` is in no package — and neither is
   `BoxElems`, nor `KarmaProps`. A search across all 2,015 packages returns 0 for every one
   of them and reads exactly like "UT2004 ships no cylinder collision volumes", which is
   false: there are 114. **Always run a negative search against a known positive first.**
   What IS in the file is the class name `KMeshProps`, because the export table names an
   object's class — see `tools/find_cylinder_geom.py`, which parses instead.
15. **Reading `KIntersect` and stopping there.** It is the single dispatcher and it does
   intercept Aggregate pairs, which is true and was written up as §3a. But
   `KAggregateGenericIntersect` **calls `KIntersect` again per element**
   (`KFarfield.cpp:878`), so "the engine intercepts Aggregate" does not imply "aggregate
   geometry never reaches Karma" — and since `KUtils.cpp:762` wraps every primitive
   collision element in an aggregate with no single-element shortcut, that inner call is
   how MOST geometry reaches Karma. Cost: two sessions of believing `Cylinder` was
   unreachable while a map in the tree was calling it 59,366 times. **Follow the callee,
   not just the caller.**
16. **Reading a wall of unknown identifiers as a vocabulary gap.** `MdtBcl` failed on
   `undefined6`, `uint6`, `int3`, `SCARRY1`, `POPCOUNT`, `uleb128`, `bRam00000018`,
   `func_0x08880001`, `swi` and `in` — a dozen `kd_compat.h` definitions away, apparently.
   They were all in ONE function, `FUN_00021130`, and `swi` is an ARM instruction while
   `in` is x86 port I/O: **neither can occur in an i386 object compiled from C.** The
   address says why — `.text` ends at 0x21013 and 0x21130 is inside `.eh_frame`, so Ghidra
   had decompiled exception-handling metadata as code. Defining the missing names would
   have made noise compile and published bogus symbols. 22 of 31 errors, deleted by
   deleting the non-function (`ghidra_clean.not_code`). **When the vocabulary looks alien,
   check the ADDRESS before extending the vocabulary.**


17. **`-ffloat-store`, to make x87 intermediates round like the original's.** The reasoning
   looks airtight: the shipped `OverlapCylCyl` has **216 `fsts`** against our **96**, so it
   rounds intermediates to 32 bits far more often while ours keeps 80-bit x87 registers,
   and `IxCylinderCylinder`'s divergence is a separating-axis TIE-BREAK
   (`if (maxSeparation < fVar11)` at `IxCylinderCylinder.c:1275`), which is exactly what one
   extra bit of precision flips. Both objects use x87, so it is not an SSE/387 mismatch
   either. It is still wrong:

   | build | count_diff | dims_diff | worst delta |
   |---|---:|---:|---|
   | baseline | 1 | 20 | 6.914e-06 |
   | `-ffloat-store` | 6 | 44 | **8.618e-01** |

   And it took `fsts` **down** to 69, not up. The flag does not do what the instruction
   counts suggest it should, and a five-order-of-magnitude jump in worst delta says it is
   perturbing the arithmetic rather than aligning it. Excess precision is not the cause of
   that divergence; measure before adopting a whole-corpus flag on this reasoning.

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
recovered Karma**, and the collision half of that has been measured (§7b). What blocks it
is the **solver**.

**The solver's blockers have collapsed from four problems into one.** As of the second
session of 2026-08-24 the argument-dropping half is solved (§5a): all nineteen vtable
dispatches in `keaRbdCore_unified` and `keaLCPSolver` carry their arguments. What is left
in all four objects is the same single thing, and item 2 below is now the whole job.

Ordered by what actually moves the project, not by what is easiest:

0. **The cylinder pairs — DONE for one, and the other is the open item.**
   Both were measured on 2026-08-24 (third session) with a shadow build that stages
   `IxCylinderTriList`, on `test-simple-physics`, with `KD_SELFTEST` clean on the same map
   (186,750 and 45,169 calls, 100% bit-identical, so the harness is sound):

   | pair | real calls | verdict |
   |---|---:|---|
   | `Cylinder × TriangleList` | 71,417 | 0 ret / 0 count / 0 dims / 0 overrun → **RELEASED** |
   | `Cylinder × Cylinder` | 24,111 | 0 ret, **3 count, 925 dims** → **WRONG** |
   | `Cylinder × ConvexMesh` | 244 | `McdGjkCgIntersect`, already released, 0 divergences |

   **What is left here is `IxCylinderCylinder`, and the interesting part is not the bug.**
   ret, touch, count, position and separation all agree; only `dims` differs — 259 vs 771,
   one bit of the feature index — on grid-aligned transforms. That is the same shape as
   `IxSphereTriList`'s released known limit, **but the comparison does not transfer**: that
   one was released because the game's regime was the clean one (0 in 1.7 M), and here the
   game's own inputs produce 925 in 24,111.

   **And nothing is holding it.** It compiles, no detector objects, so it is in
   `/tmp/kd_build` and in the 108/109-object build of §7c. The quarantine can only hold what
   a detector recognises; an object that is simply never measured, and is wrong, walks
   straight in. Decide whether to pull it before the next substituted build.

   Reproduce with `test/make_shadow_metoolkit.sh` over a copy of `/tmp/kd_build` with
   `IxCylinderTriList` compiled in, then `KD_RUNTIME=/tmp/kd_runtime ./test/run_map.sh
   test-simple-physics 300 "$COMMON_URL"`. `KD_RUNTIME` keeps the run off
   `/home/ion/karma-run`, which matters if someone else is using it.

1. **DONE — the vtable call sites.** `tools/gen_vtable_callsites.py` +
   `DumpDecomp.applyVtableCallsiteOverrides()`, adopted as `out8`. §5a has the method, the
   checks and the one bug it went through. Items 1 and 3 of the old list were indeed "one
   shared fix, not two", as this file predicted. Nothing to do here; it is written down
   because the technique generalises to any indirect call whose target is pinned by a
   relocation.

2. **THE JOB: the frames — and the DWARF does not describe them.** This is the correction
   that matters most. "Ghidra cannot model these frames" was true and understated. For
   `MdtKeaAddConstraintForces` the debug info gives names and types but **no locations at
   all**, because the abbrevs its DIEs use do not declare `DW_AT_location`:

   ```bash
   readelf --debug-dump=abbrev keaRbdCore_unified.o
   #   abbrev 33: DW_TAG_formal_parameter   (no DW_AT_location)   <- this function
   #   abbrev 34: DW_TAG_variable           (no DW_AT_location)
   #   abbrev 35: DW_TAG_variable           (no DW_AT_location)
   #   abbrev 45: DW_TAG_formal_parameter   has DW_AT_location    <- other functions
   #   abbrev 46: DW_TAG_variable           has DW_AT_location
   ```

   Other functions **in the same object** use 45/46 and are located normally, so this is
   per-function, not a property of the build.

   **But do not read that as "so nothing can be done" — that was too strong, and half a
   day proved it.** Ghidra recovered usable storage for most of these slots anyway; it
   simply spells a slot's **address** differently from its **value**, and only the value
   gets a declaration:

   ```c
   MdtKeaParameters in_stack_ffffffa0;      /* declared, right type, 76 bytes */
   pMVar3 = (MeReal *)&stack0xffffffa0;     /* undeclared — the SAME slot */
   ```

   Across all four objects there are only **six** `stack0x` references and four have a
   covering declaration. `ghidra_clean.fix_stack_address_name()` resolves them, and that
   alone made **`keaIntegrate_pc` compile** — it had been failing on nothing else.
   **It must size-check, and the check is not decoration:** it refuses
   `keaRbdCore_unified`, where the copy writes 92 bytes into a slot Ghidra declared as
   `undefined1[72]`. The check cannot be left to the compiler, because the repair loop
   accepts an edit when the error count does not *grow* — so trading `undeclared` for a
   failed assertion would be kept, not reverted.

   **And then `keaIntegrate_pc` turned out to be wrong**, which is the point of the whole
   exercise: 4.900e-04 m of divergence on `scene_chain` against 6.100e-04 m of
   sensitivity — PROVEN sensitive, not bit-identical, the same footing as
   `keaMatrix_PcSparse_vanilla`. It stays quarantined. Compiling was not the win; being
   **measurable for the first time** was. `proven.txt` has it.

   What is genuinely left is narrower than it was: `register0x00000010` (one reference),
   `keaRbdCore_unified`'s oversized copy, and the genuinely-missing call arguments in
   `keaLCPSolver` (`makeFromPcSparsePSM`, expected 10 have 9 — the call already passes
   `this`, so it is a MIDDLE argument) and `keaLCP_new` (nine calls, each short by one, at
   non-uniform offsets).

   **`MdtWorld` and `MeMath` and `MdtBcl` belong to this item too, and the cheapest
   subjects are the last two — one error each.** `MdtBcl` reached that state on 2026-08-24
   when 30 of its 31 errors turned out to be Ghidra decompiling `.eh_frame` and undecodable
   bytes (dead end 16). `MdtWorld` is next at ten: 6 `stack0x` and 6
   `incompatible type for argument N of MdtKeaAddConstraintForces` /
   `MdtKeaIntegrateSystem`. That is the CALLER side of the same by-value aggregate problem:
   at `MdtWorld.c:253` the copy loop writes 19 `MeReal` — 76 bytes, an `MdtKeaParameters`
   exactly — starting at `&stack0xffffff6c`, where Ghidra declared
   `MeReal in_stack_ffffff6c;` and four bytes. `fix_stack_address_name`'s size check
   refuses it, correctly.

   **What the three cheap ones actually look like**, because they are not all one shape:
   `MdtBcl`'s single site is Ghidra rendering the function EPILOGUE (`lea -0xc(%ebp),%esp`)
   as a pointer assignment and then reading locals through it; `MeMath`'s is genuine loss —
   Ghidra dropped the `fcos`/`fsin` results that build the matrix being read; `MdtWorld`'s
   is outgoing argument marshalling. Only the third is the by-value aggregate case.

   **Why start on these rather than on a kea object:** none of them is in `libMdtKea`,
   `MdtBcl` and `MeMath` have one error each, and the same fix has to work on all of them —
   so a wrong answer shows up cheaply. §12's standing lesson is that the object which
   motivates a tool is the worst thing to validate it on; these are the alternative
   subjects that item did not have before.

   **THE GHIDRA-SIDE ATTACK, which is the one to try next and has never been attempted.**
   Ghidra IS installed (§5, `/home/ion/tools/ghidra_12.1.3_PUBLIC`) and a re-run is 75–120
   minutes, so this is affordable. The hypothesis worth testing:

   > The DWARF for these functions declares names and types with **no `DW_AT_location`**.
   > Ghidra's DWARF importer still creates those variables, so the decompiler is handed a
   > set of typed locals it cannot place — and then has to reconcile them with its own
   > stack analysis. Ghidra's *native* stack-frame recovery, with no DWARF at all, is
   > usually good. **The DWARF may be making this worse, not better.**

   The experiment: dump `keaRbdCore_unified`, `MdtWorld`, `MdtBcl` and `MeMath` with DWARF
   variable import DISABLED (Ghidra's DWARF analyzer has options for exactly this — import
   data types but not variables/parameters), into a NEW output dir, and diff the four
   bodies against `out9`. If the `stack0x`/`in_stack_` mess collapses into ordinary
   `local_NN`, the whole item is solved for all eight objects at once.

   **Cheap and decisive, and it costs nothing if wrong** — a per-object dump comparison,
   with `out9` untouched. §5's rules apply: new directory, keep the old one, seven gates
   before adoption.

   Note this is the FIRST thing in the project that would be validated on a non-kea object:
   `MdtBcl` and `MeMath` are one error each, so the answer shows up in minutes.

   **The one lead that is not a guess.** The by-value *incoming* parameters are pinned by
   the cdecl ABI, not by debug info, and Ghidra already has the signature:

   | parameter | location | size |
   |---|---|---|
   | `pconstraints` (`MdtKeaConstraints`) | `ebp+0x08` | 92 |
   | `blist` | `ebp+0x64` | 4 |
   | `tlist` | `ebp+0x68` | 4 |
   | `num_bodies` | `ebp+0x6c` | 4 |
   | `parameters` (`MdtKeaParameters`) | `ebp+0x70` | 76 |

   **Confirmed against the machine code**, which is why it is worth starting from: the
   prologue does `lea 0x8(%ebp),%esi` and `lea 0x70(%ebp),%esi` — the two by-value
   aggregates — and copies them to outgoing argument areas. So the `in_stack_*` /
   `register0x00000010` copy loops are *outgoing by-value argument passing* that Ghidra
   rendered clumsily, **not** unknowable locals. That reframes the problem from "recover an
   unknown frame" to "make Ghidra model a by-value aggregate argument", which is a much
   smaller question and is Ghidra-side.

3. **Then, and only then, the vptr store.** Both objects also fail on `_vanillaFunctions` /
   `_vanillaQMatrix` being undeclared. That is the **same EXTERNAL-slot addend collision as
   `keaMemory`** (§5) and it is **already inverted exactly and uniquely** by existing
   machinery — `ghidra_clean.relocation_targets(obj, per_function=True)` returns:

   ```
   keaRbdCore_unified  MdtKeaAddConstraintForces
     __ZN12keaFunctions8initPoolEPvi   -> (_ZTV20keaFunctions_Vanilla, 8)
     __ZN12keaLCPSolver8solveLCPE...   -> (_ZTV26keaMatrix_pcSparse_vanilla, 8)
   keaLCPSolver  makeXandW / PrincipalSubmatrix / PrincipalPivotTransform
     ___gxx_personality_v0             -> (_ZTV26keaMatrix_pcSparse_vanilla, 8)
   ```

   `+8` is the Itanium ABI address point, so `_vanillaFunctions = keaFunctions::initPool;`
   is really `vanillaFunctions.vptr = &vtable[2]`, and **`__gxx_personality_v0` is not the
   personality routine at all**. Corpus-wide only these 2 objects import a vtable, so the
   blast radius is contained. The shape that works is
   `#define _vanillaFunctions (*(char **)&vanillaFunctions)` plus an
   `extern void *_ZTV20keaFunctions_Vanilla[];`, checked by requiring the local's declared
   type to equal the class named in the relocation.

   **Deliberately not implemented, and the reason is the point.** Writing it would remove
   an error class from two objects that *still* would not compile, because of item 2 — so
   **no gate could see whether the rewrite was right or wrong**. An unvalidatable generator
   change is the exact risk profile that produced dead ends 9 and 10. Do item 2 first; then
   this becomes verifiable and is an afternoon.


4. **DONE — the callbacks are audited.** Result: one real exposure, closed and measured
   inert; the other two do not exist in this driver.

   - **Shared mutable state.** `difftest_pair.c` gave BOTH implementations the same
     `McdModelPair`, so anything the original wrote into it — `McdGjkCgIntersect` keeps a
     cache there — was read by the recovered one. `kd_shadow.c` has carried the fix for
     this since §7; the synthetic driver never got it. Each side now gets its own pair,
     `KD_SHARECACHE=1` restores the old behaviour. **Measured: every verdict and every
     count is byte-for-byte unchanged** (IxBoxBox 1, IxSphereTriList 137, IxBoxTriList
     1463/139961/12060, IxCylinderCylinder 1/20), because the cache pointer is NULL here so
     nothing was ever written. The exposure was real and currently inert — worth closing
     because the moment anyone warms the cache it stops being inert.
   - **The allocator** is not stubbed: the driver builds a real `MstUniverse`, so Karma's
     own allocator runs. Nothing to audit.
   - **`McdCacheHello`/`Goodbye`** are not stubbed and never called, which is exactly why
     GJK runs cold-path-only here. That remains a documented limit on this evidence, and
     it is the one piece of §11 item 4 still open: **warming the GJK cache is untested
     ground on the busiest pair family in the census.**

5. **Chase what is left of the `IxBoxBox` and `McdGjk` divergences** (§8). Both reproduce
   synthetically once box dimensions vary — 1 and 2 count divergences in 200,000 — so they
   can be worked on deterministically, without waiting for a match.

6. **Settle the sensitivity zeros that matter.** §4a lists which objects the scenes cannot
   see. For anything you intend to rely on, mutate its outputs and confirm the gate moves
   before quoting a bit-identical result — `keaCalcAcceleration_vanilla` is the worked
   example of a zero that means nothing.

7. **Grind the tail.** **DONE, as far as it goes — do not start here.** 14 objects and
   271 errors remain, and §13 triages every one: 3 DEAD, 2 documented leave-alones, 3 dead
   end 9, **4 the §11 item 2 frame problem**, 2 low-value profilers. There is no cheap
   object left that is not one of those, so the next move is item 0 or item 2, not this.
   The distribution over the 14, re-measured after the third session of 2026-08-24:

   271 errors in total, and note the shape: **`MeASELoad`, `MeFGeometryFromMesh` and
   `McduDebugDraw` account for 147 of them and all three are DEAD** (§3b), while `McdSpace`
   is another 50 and is a documented leave-alone. So **197 of 271 belong to objects nobody
   should touch**, and of the remaining 74, sixty are the two profilers. Sort by §13's
   verdict column, never by error count — that is the whole lesson of this item.

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

8. **Replace, don't recover** — but scope it from §8a first, because "load-time only" is
   **wrong** and it was the reason this looked cheap. `libMcdConvexCreateHull` is qhull 2.6
   (1998), 186 KB, open source. `MeAssetDB`/`MeXML`/`MeAssetFactory` (51 KB) is `.ka` XML
   parsing, not physics, and is still the easy one. `MeViewer2`/`MeApp` (74 KB) are never
   linked.

### 8a. What a qhull replacement actually has to reproduce

Scoped 2026-08-24, before writing any code, and the scoping changed the plan twice.

**The good news first.** The entire hull API has exactly **one** consumer in the whole
library — `McdConvexMesh.o` — and only one call reaches qhull at all:

```
KUtils.cpp:819  McdConvexMeshCreateHull(fwk, verts, n, 0)
                    -> McdComputeHull(&poly, n, verts)      <- the only reference in metoolkit
```

`IxConvexPrimitives`, `McdGjk`, `ConvexGeomUtils`, `IxConvexTriList` and
`McdConvexMeshMassProps` import **zero** `McdCnv*` symbols.

**The bad news, and why the "load-time only" framing is misleading.** The hull is *built*
at load time and then **kept**. `McdConvexMeshCreateHull` stores the whole structure —
`vertex`, `face`, `edge` *and* `edgeIndex` — straight into the geometry object:

```c
pMVar2[1].mRefCtAndID = (MeU32)poly.vertex;
pMVar2[1].prev        = poly.face;
pMVar2[1].next        = poly.edge;
pMVar2[1].frame       = poly.edgeIndex;
```

and `McdConvexMeshMaximumPointLocal` — **the GJK support function** — hill-climbs it on
every query via `McdCnvVertexGetNeighbor`, i.e.
`edge[edgeIndex[vertex[v].firstEdgeIndex + i]].toVert`. Per the census that is Box ×
ConvexMesh (608,280), Sphyl × ConvexMesh (62,698), Sphere × ConvexMesh (9,741) and
ConvexMesh × ConvexMesh (4,337) — **the busiest pair in the game sits on this adjacency
structure.** A replacement that gets the topology subtly wrong does not fail at load; it
degrades GJK silently.

So the deliverable is not "call a hull library". It is the **complete** structure
documented in `McdQHullTypes.h`:

- every edge stored **twice**, once per direction;
- edges grouped by face and ordered **anti-clockwise** within each face;
- `edgeIndex[]` giving each vertex's outgoing edges in ACW order;
- `firstEdge` / `firstEdgeIndex` with the **sentinel** last face and vertex
  (`firstEdge == numEdges`);
- `rightFace`, `leftFace`, `invLength`, outward `normal`;
- coplanar triangles **merged** into polygonal faces — Karma's faces are polygons.

**How to validate it, since the usual gate cannot.** `difftest_pair` compares against the
shipped library, and a differently-indexed but geometrically identical hull makes that
comparison apples-to-oranges. Three tiers that do work:

1. **Structural invariants on the hull alone** — every edge paired with its reverse, Euler
   `V - E/2 + F == 2`, every face's edges forming a closed ACW ring, sentinels consistent,
   normals outward, every input point inside or on the hull.
2. **Geometric A/B against the shipped `McdComputeHull`** on the same point sets, compared
   as *sets* of vertices and face planes rather than by index. Build the harness against
   the shipped `.a` first — it is ground truth and it is sitting right there.
3. **Then** the collision difftest, with the new hull on **both** sides, which isolates
   "is the hull right" from "is the collision code right".

That third point is the one to hold on to: with the same hull on both sides the existing
convex difftest still works unchanged, so this is testable — it just cannot be tested by
diffing against the shipped hull.

### 8b. The replacement, and how far it is validated

`src/McdConvexCreateHull/kd_convexhull.c` — ~600 lines of plain C, all 15 exported
functions, **1,416,194 bytes → 10,382**. Stage it into a metoolkit tree with
`test/make_hull_lib.sh`, which also asserts the exported symbol set still matches the
shipped archive.

| tier | how | result |
|---|---|---|
| 1 — is it a valid hull? | `KD_HULL_IMPL=… test/hull_probe.sh` | **100,633 checks, 0 failures**; identical V/F/E to shipped on all six shapes; every degenerate case and the 1e-6/1e-5 cutoff match |
| 2 — is it the same solid? | `test/hull_ab.sh` | 894 canonical lines, **1 differing** (one normal component by 1e-4), **all six volumes identical** |
| 3 — does collision behave? | full `difftest_pair` A/B, same driver and build | 12 pairs, 2.4 M pairs: **identical except two borderline pairs flipping to touching** |

Tier 3's two differences are `McdGjkCgIntersect` (46983 → 46984 touching, 0 → 1 count) and
`McdConvexMeshTriangleListIntersect` (175353 → 175354 touching), which is what a 1e-4 change
in a face normal does. **Attributed, not assumed**: `KD_SELFTEST` on GJK with the new hull is
200,000 pairs 100% bit-identical, so the hull and harness are sound and that count divergence
is the *recovered* `McdGjk`'s own residual on an input the shipped hull never produced —
precisely the "low and sample-dependent" behaviour `proven.txt` already records.

Also: **wasm32 compiles with a symbol set identical to i386**, clean under `-Wall -Wextra`.
The shipped archive additionally exports `dfacet`/`dvertex`, qhull's debugger helpers;
nothing in any of the sixteen archives references them, so they are deliberately absent.

**Tier 4 — in the game. DONE 2026-08-24**, once the asset mount came back.
`build-shadowhull-karma` is the shadow harness plus the new hull, verified to contain **zero
`qh_` symbols** against 361 in the shadow build (because "it linked" is not evidence the
archive swap took).

| | new hull | shipped-qhull control |
|---|---:|---:|
| Box × ConvexMesh | 6,713 | 5,563 |
| Sphyl × ConvexMesh | 1,820 | 2,729 |
| Sphere × ConvexMesh | 240 | 1,084 |

Same 8 pairs firing, 39 census rows both sides, no crash either side; counts differ because
ONS is non-deterministic. **This is the acceptance test, and the obvious one would have been
wrong** — "the match ran" shows nothing, because `McdConvexMeshCreateHull` only checks the
return value, so if every hull had failed the geometry would silently not exist and the match
would run perfectly well. Non-zero Box × ConvexMesh is the proof.

Then the shadow harness on the same map, 440 s, full distance:

```
Box x ConvexMesh  15,425 calls  14,463 bit-identical  962 float-noise
                  0 ret  0 count  0 dims  0 overrun  0 nonfinite   worst 4.749e-04
Box x Box          1,075 calls     946 bit-identical  129 float-noise   worst 1.526e-05
$KD_SHADOW_DIVERGENCES empty.
```

The recovered collision code agrees with the shipped original on **15,425 real calls whose
geometry came out of the replacement hull** — both "the hull is good" and "GJK still works on
hulls this builds", on the busiest pair in the census. The 4.7e-04 is penetration depth in an
iterative algorithm, the regime `proven.txt` already documents for GJK.

**So the qhull half of §12 item 4 is done at all four tiers.** What is left of item 4 is the
asset loader, which §3b shows is a larger and more load-bearing thing than §11 item 8
implies.

**Tier 1 exists: `test/hull_probe.sh`.** It runs the SHIPPED `McdComputeHull` and checks
every claim the header makes in prose. **98,899 checks, 0 failures**, so the contract above
is verified rather than assumed, and the same checker is the acceptance test for a
replacement. What it settled:

| claim | verdict |
|---|---|
| sentinel face and vertex with `firstEdge == numEdges` | present, and everything that iterates depends on it |
| `V - E/2 + F == 2` | holds on all six shapes |
| coplanar triangles **merge** into polygons | yes — a cube is `F=6` with 4-vertex faces, not 12 triangles |
| redundant coplanar + interior input points are discarded | yes — cube + 6 face-centre + 5 interior points gives the identical `V=8 F=6` hull |
| every directed edge has its reverse | yes |
| `invLength == 1/|edge|`, normals unit and outward | yes |
| `edgeIndex` entries all START at their vertex | yes — this is what GJK hill-climbs |
| every input point inside or on the hull | yes |
| **winding is ACW seen from OUTSIDE** | yes, by Newell's normal agreeing in sign with the face normal |

That last row is the one a replacement is most likely to get backwards, and checks on ring
closure and outward normals **cannot see it** — both hold for either handedness.

**Two conventions the header is ambiguous about, settled by measurement.** It says the edge
array is "sorted by leftFace and ACW" and also that an edge records "the face on its right
side"; those two sentences disagree about which field groups the array, and reversing it
mirrors every adjacency a replacement builds.

- face `f`'s edge range contains exactly the edges with **`leftFace == f`** — measured, on
  every shape;
- an edge and its reverse carry the same two faces **swapped**.

Note also that the hull **reindexes**: `vertex[]` is the hull's own array and does not
follow input order, which is precisely why the acceptance test cannot be an index-wise
diff.

**Degenerate input, which for UT2004 is ordinary** — its collision volumes are boxes and
low-poly prisms, so duplicate and coplanar vertices are the normal case:

| input | shipped behaviour |
|---|---|
| 1 point, coplanar triangle, coplanar square, collinear | `return 0` — no hull |
| tetrahedron with every vertex duplicated | `return 1`, duplicates collapsed, `V=4 F=4 E=12` |
| unit square extruded by `t` | rejected at `t <= 1e-6`, hull built from `t >= 1e-5` |

`McdConvexMeshCreateHull` only checks the return value and yields NULL, so the primitive is
silently not created. **That cutoff matters in both directions**: a replacement that accepts
a box the shipped code rejects invents collision geometry the game never had, and one that
rejects more removes geometry it did have. Neither shows up as a crash.

(The shipped code also leaks qhull's diagnostics — `- normal: - vertices:center point` — to
stdout on degenerate input. A replacement should simply be silent.)


### What needs the project owner, and nothing else will do

**As of 2026-08-24 this list is empty.** All three content asks that stood here are
answered. That is worth stating plainly, because "we need a map" was the standing excuse for
several open questions and it turned out to be answerable from the repo twice and from the
asset files once.

- ~~**A map that exercises `Box × TriangleList`.**~~ **WITHDRAWN — no such map can exist.**
  See §3a: UT2004 intercepts this pair in its own dispatcher and never calls Karma's
  function. Answered from the engine source.
- ~~**A map that exercises `Aggregate` geometry.**~~ **WITHDRAWN for the same reason** —
  `KIntersect` routes every Aggregate *pair* to the engine's own
  `KAggregateGenericIntersect`. (But note §3a's correction: that function recurses per
  ELEMENT, so aggregate elements do reach Karma.)
- ~~**A map with `Cylinder` collision geometry.**~~ **ANSWERED 2026-08-24, and the answer
  is that they are already here.** `tools/find_cylinder_geom.py` parses the shipped
  packages and finds **114 cylinder collision elements**, and a census on
  `test-simple-physics` measured `Cylinder × TriangleList` at 59,366 calls. §3. It did not
  need a new map; it needed reading the serialiser instead of grepping for a field name
  that the format does not store.
- **More community maps are still WELCOME but no longer blocking.** `CBP2`/`UCMP`/`BE-`/
  `SPAC-` reach pairs Epic's optimised maps never do; that fact came from the project owner
  and it has been the single most productive operational input to this project.
  `ONS-UCMP-ABC-ECE` is still the ONLY known map that reaches `ConvexMesh × TriangleList`.
  One question a map *could* still settle, though nothing depends on it: whether stock Epic
  gameplay ever simulates a cylinder, as opposed to merely shipping the geometry.

Everything else — code, tests, measurement, tooling — is self-service.

**Not an ask, but do the check anyway.** If `/home/ion/ut2004-assets` is empty the tree has
unmounted, which happens on reboot and which only the owner can undo. Per the owner
(2026-08-24) this is expected, unavoidable and rare — **so it is an operational note, not an
outstanding dependency, and this section is empty without it.** The reason to keep checking
is purely that the failure is misleading: an empty asset dir crashes the engine in a way
that reads as an engine bug, and one session was spent debugging it as one. It costs one
`ls` before any map run. If it is empty, say so and stop rather than diagnosing. §6 has the
box.

---

## 12. What "complete" looks like

**Complete** is not "every object recovered". It is:

1. Every object the census (§3) shows the game *actually calls* is recovered and validated.
   **This was marked done on 2026-08-23 and is RE-OPENED as of 2026-08-24.** Twelve of the
   fourteen called pairs are validated; `Cylinder × TriangleList` (59,366 calls) is
   quarantined and unmeasured, and `Cylinder × Cylinder` (24,267) compiles but has never
   been measured. This file said to "treat any new entry in the census as re-opening it" —
   this is that.
2. Validated means: 0 `ret_diff`, 0 `count_diff`, 0 `dims_diff`, 0 `overrun` across a
   multi-hour in-game session, with `KD_SELFTEST` clean on the same session, and
   `proven.txt` carrying the evidence.
3. All three `substitute_test.sh` scenes clean for every recovered object — **and, per
   §4a, checked with `scene_census.sh` and `gate_sensitivity.sh`, because "clean" on a
   scene that never ran the object is not a result.**
4. qhull and the asset loader replaced rather than recovered.
5. No detector suppressed, no object released without a line in `proven.txt`.
6. The whole set builds as ordinary C for **wasm32 and arm64/armv7**, not just i386.
   **wasm32 is done** — 106/106 compile with byte-identical exported symbols
   (`test/wasm_check.sh`). **armv7 and arm64 both compile 109/109 with the Android NDK
   (§6b) — but only armv7 is believed CORRECT, and arm64 is believed WRONG for a
   structural reason. Read §6b before touching either.**
   Nothing has been *executed* under wasm. See `HANDOVER-WEB.md`.
7. The engine runs with `WITH_KARMA=1` against recovered Karma with **no shipped `.a` in the
   link at all**. `test/make_substituted_metoolkit.sh` builds that tree.
   **The collision half is done** — all eight objects behind the twelve called pairs, in
   the driving seat, through full ONS matches (§7b). The solver half is not, and cannot be
   until §11 items 1–3 compile.

### The seven items, and exactly where each one stands

| # | item | state |
|---|---|---|
| 1 | every pair the census shows the game calling is recovered and validated | **RE-OPENED 2026-08-24, and now 14 of 15.** `IxCylinderTriList` was measured and released the same day (71,417 real calls, 0 structural). `Cylinder × ConvexMesh` was already covered by `McdGjk`. **The one open pair is `Cylinder × Cylinder`, measured WRONG at 925 dims_diff in 24,111 real calls.** §3, `proven.txt`. |
| 2 | validated = 0 ret/count/dims/overrun in a live match, `KD_SELFTEST` clean, evidence on the line | **DONE** for those twelve, and it is the standard the two new pairs have to meet. |
| 3 | all three scenes clean for every recovered object, *and* checked for sensitivity | **DONE**, and the sensitivity check (§4a) is what makes it mean anything. |
| 4 | qhull and the asset loader **replaced**, not recovered | **DONE, both halves — but the asset half was RECOVERED, not replaced (§8c), which is a better outcome: exact rather than equivalent.** Qhull, all four tiers: `src/McdConvexCreateHull/kd_convexhull.c` replaces all 15 exported functions — 1.4 MB → 10 KB: 100,633 invariant checks, identical geometry and volumes, a collision A/B differing on 2 borderline pairs in 2.4 M, and **a live ONS match with 15,425 real GJK calls and 0 structural divergences**. wasm32 clean with an identical symbol set. The asset loader is 9 of 9 recovered (§8c) and needs no replacement. |
| 5 | no detector suppressed, nothing released without evidence | **HOLDING, and a NEW HOLE FOUND.** 25 objects quarantined and the quarantine is load-bearing (§4a: `MdtPartition` alone turns a bit-identical scene into a SIGSEGV). But `IxCylinderCylinder` is measured wrong on a live pair and **no detector holds it**, so it is in the build. The quarantine only catches what a detector recognises — "not held" is not "validated". §11 item 0. |
| 6 | builds as ordinary C for wasm32 **and arm64/armv7** | **wasm32 DONE** (109/109, byte-identical symbol sets). **armv7 DONE** (109/109, symbol sets identical, and it is a 32-bit-pointer target so the recovery's core assumption holds). **arm64 COMPILES AND IS NOT TRUSTED** — 109/109 with identical symbol sets, and 920 pointer/int conversion diagnostics against armv7's 23 on the same five objects. That 40x ratio is 64-bit pointers being truncated into 4-byte slots. §6b. Nothing has been *executed* on any of the three. |
| 7 | engine runs on recovered Karma with **no shipped `.a` in the link at all** | **COLLISION HALF DONE for the twelve validated pairs** (§7b, two maps, 11 runs/arm, indistinguishable from stock) — but those runs were on maps with no cylinder traffic, so they do not cover the two new pairs. **SOLVER HALF BLOCKED**, on one problem — the arguments are recovered (§5a) and what remains is the frames, §11 item 2. |

**So what is left, in one sentence each:**

- **One cylinder pair, measured wrong and unheld.** `IxCylinderCylinder`, 925 `dims_diff`
  in 24,111 real calls, with no detector holding it out of the build. Its sibling
  `IxCylinderTriList` was measured and released the same day. §3, §11 item 0.
- **The solver's frames** — `keaRbdCore_unified`, `keaMemory`, `keaIntegrate_pc`,
  `keaLCPSolver`+`keaLCP_new`, **and `MdtBcl`, `MeMath` and `MdtWorld`, which are new and
  are the cheapest subjects to validate a fix on — one error, one error and ten.**
  Their virtual calls now carry arguments (§5a); what is left
  is that **the DWARF carries no `DW_AT_location` for these functions' variables**, so the
  frame is not merely unmodelled but undescribed. This is the only thing between here and
  item 7. §11 item 2.
- ~~**qhull and the asset loader**~~ — **both done.** Qhull is replaced and validated at
  four tiers including a live match (§8a, §8b); the asset loader turned out to be
  RECOVERABLE and is 9 of 9 (§8c).
- **arm64** — untried, needs a cross-compiler.
- **Executing anything on wasm** — the web agent's job. `HANDOVER-WEB.md`.
- **The tail** — 14 objects, 271 errors, fully triaged in §13, and **effectively finished**:
  3 unreachable, 2 documented leave-alones, 3 dead end 9, **4 the §11 item 2 frame problem**
  and 2 low-value profilers. Nothing cheap is left in it.


### Where the project actually stands — read this before estimating anything

**Done, to a real standard:** the collision-detection *interaction* layer, for the twelve
pairs that had been seen called as of 2026-08-23. Recovered, compiling for i386 and wasm32,
each one measured against the shipped original on real inputs from a live match, evidence in
`proven.txt`. **Two more called pairs appeared on 2026-08-24 and neither is at that
standard** — §3.

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
runs on recovered kea**, however good the kernels are.

That statement is unchanged, but the reason behind it is now much narrower. The
argument-dropping blocker is gone (§5a), and the remaining failure in all four is one
thing: **the DWARF for these particular functions declares no `DW_AT_location`**, so
nothing — Ghidra or otherwise — can read the frame out of the debug info. §11 item 2 has
the `readelf` that shows it and the one lead that is not a guess.

**Never executed on wasm.** 106/106 compile with byte-identical exported symbols. Not one
instruction has run. See `HANDOVER-WEB.md`.

**Run end to end, now, for the collision layer** — §7b. Not for the solver.

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

The second session of 2026-08-24 added two more of exactly this shape, and both are worth
carrying because they are about *validating on the wrong subject*:

- `gen_vtable_callsites.py` computed call-site addresses wrongly (§5a) and read **9/9
  applied** on the object it was written for, because that object's only relevant function
  starts at offset 0 — where the wrong formula and the right one agree. The second object
  put six signatures at addresses in `.eh_frame`. **The object that motivates a tool is the
  worst possible thing to validate it on.**
- `scene_census.sh` — one of the seven gates, and the instrument §4a's whole argument rests
  on — could not pass at all. It swept in the quarantined objects, `MdtPartition` segfaulted
  the scene, and it printed `FATAL: no census written`. Pre-existing, and it reads exactly
  like a regression in whatever you last touched.

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
- **If a SEARCH came back empty, can it find anything at all?** (Added 2026-08-24. Grepping
  2,015 asset packages for `CylinderElems` returned 0 and looked like the answer to a
  question that had been open for sessions. The same grep returns 0 for `BoxElems` and
  `KarmaProps`, which certainly are in there — the field names simply are not in the file
  format. Run the negative search against a known positive before believing it.)
- **Does the code path you are calling unreachable actually terminate there?** (Added
  2026-08-24. `KIntersect` intercepts Aggregate pairs — true. The interceptor then calls
  `KIntersect` again per element — also true, and not noticed for two sessions. An
  interception is not a dead end until you have read what it dispatches to.)

That checklist is worth more than the next ten objects.

---

## 13. The 14 remaining failures, triaged

Written so the next session starts from a decision, not from re-deriving one. Counts are
error counts under `out9`. **"DEAD" means `tools/reachable.py` proves nothing in the engine
can reach it — §3b — so it is out of scope, not a to-do.**

| object | reach | errs | verdict |
|---|---|---:|---|
| `MeASELoad` | **DEAD** | 126 | out of scope |
| `MeFGeometryFromMesh` | **DEAD** | 20 | out of scope |
| `McduDebugDraw` | **DEAD** | 1 | out of scope — and dead end 10 already burned time here |
| `McdSpace` | live | 50 | **documented leave-alone**, §11 item 7: the `struct _McdSpace` layout is not in the DWARF *anywhere*, including its own object, where the DIE is `DW_AT_declaration: 1`. Inferring it is the guess the detectors exist to stop |
| `MeSimpleFile_linux` | live | 1 | **documented leave-alone**, §11 item 7: `-D_FORTIFY_SOURCE=0` makes it compile with three garbage arguments. The compile error is the useful signal |
| `McdSphyl` | live | 1 | **dead end 9.** `(float)s[1].mRefCtAndID + (float)s[1].prev` — GCC rejects only the second cast, and the original loads *both* as floats. The compiler is reasoning about Ghidra's types and is wrong |
| `McdTriangleList` | live | 6 | **dead end 9, same shape.** `(float)g[1].prev` etc. |
| `McdBox` | live | 2 | **dead end 9**, at lines 235/236. Not the cheap win the error count makes it look |
| `keaLCP_new` | live | 10 | **solver, do not text-repair.** §11 item 2 — nine calls short by one argument at non-uniform frame offsets |
| `MdtWorld` | live | **10** | **RECLASSIFIED — this is the solver-frame problem, §11 item 2, and it is the cheapest place to attack it.** 6 `stack0x` + 6 `incompatible type for argument N` of `MdtKeaAddConstraintForces`/`MdtKeaIntegrateSystem`. `fix_stack_address_name` refuses correctly: the copy at line 253 writes 19 `MeReal` = 76 bytes (an `MdtKeaParameters`) and Ghidra declared 4. Outgoing by-value aggregate passing, exactly as §11 item 2 predicts — but in a NON-kea object, so a fix can be validated somewhere cheap |
| `MdtBcl` | live | **1** | **31 → 1** on 2026-08-24: 22 errors were `FUN_00021130`, which is `.eh_frame` decompiled as code, and 2 more were `halt_baddata`-only functions (see below). The one left is `&stack0xfffffff4` used as a base to read a locals block at −0x80…−0x68 — and it is Ghidra rendering the function's **epilogue** (`lea -0xc(%ebp),%esp`) as a pointer assignment. §11 item 2's family. **Tied with `MeMath` for the cheapest validation subject in the project** |
| `MeMath` | live | **1** | one `stack0x` at line 495, genuine frame loss rather than a name: Ghidra also dropped the `fcos`/`fsin` results that build the matrix being read. Same family as `MdtBcl` and `MdtWorld` |
| `MeProfile_linux` | live | 13 | open, **low value** (§3b puts it under platform/misc) — and §13 used to undersell it as "an `rdtsc` shim". It needs that, plus `struct timeval` as a bare tag, plus the §5 EXTERNAL-slot collision (`_select` and `_clockSpeed` are neighbours of `frameTime`, not real symbols). Four families for a profiler timer |
| `MeProfile` | live | **29** | open, **low value** — 11 `request for member`, 5 `weightingData` (§11 item 7: a variable, not a type), 5 of the exported-DATA rename gap (`frameTime`/`clockSpeed` vs `kd_*`; dead end 10), 2 `__divdi3`/`__udivdi3` |

**Recovered on 2026-08-24 (third session): `McdContact`, `McdMessage`, `McdBatch`,
`mesffnmin`** — 17 failures down to 14, 106 objects up to 108, and **271 errors across the
14**. `McdContact` compiles but is now held by the `extraout_EAX` detector, which is a
different and real defect: that is a **reclassification**, not a release.

**The tail is now essentially exhausted, and that is the useful summary.** Of the 14:
3 are DEAD, 2 are documented leave-alones, 3 are dead end 9, **4 are the §11 item 2 frame
problem** (`MdtBcl` 1, `MeMath` 1, `MdtWorld` 10, `keaLCP_new` 10) and 2 are low-value
profilers. There is no cheap object left that is not one of those. **Work item 2, or work
the cylinder pairs (§11 item 0) — grinding the tail is done.**

### What has been working, and what has not

Every object recovered has come from finding a **shared cause** and fixing the generator,
never from editing a generated file. The pattern that keeps paying:

1. group the remaining errors by normalised message across all failing objects;
2. pick the biggest group;
3. find the ONE thing upstream that produces it;
4. fix that, then **diff the blast radius** — the acceptance test is that every
   already-compiling object's `.o` stays byte-identical.

That last step is not ceremony. It caught a regression in an earlier session
(`fix_float_as_pointer` widened to explicit `(T *)` casts put three new errors into
`McdCylinder`, which had none), and on 2026-08-24 it caught two objects changing that
**should** have changed — `McdNull` and `McdPlane`, whose `warn` flag went from a denormal
float to the `int` the DWARF declares. When the acceptance test trips, disassemble before
deciding: those two now emit the shipped `mov`/`test`/`jne` instead of a float compare with
an extra NaN branch, which is convergence, not drift.

**Two failure modes to expect, because both keep happening:** a rule that looks right and
silently declines or mis-scans, and a fix that looks like a regression but is a
reclassification (`recover.py` labels by the FIRST error's pattern — check the error COUNT).

The first one has now cost six rules, and **calling the rule directly on the offending line
found every one of them**: the newline stop in `scan_unary_forward`, the `ANY_CAST` missing
star, the `_match_bracket` off-by-one, `fix_field_offset` declining on a subscripted base,
`scan_postfix_backward` stopping at a base name ending in a digit, and the `::` branch of
`gen_prelude` skipping its own sanitising pass. **Read the rule, then run it. Reading alone
has a 0/6 record here.**

And a third, new on 2026-08-24: **a search that cannot find what it is looking for.**
Grepping 2,015 asset packages for `CylinderElems` returned 0 and read like an answer. It was
not — the field name is not in the file format at all (§3). The control that caught it was
searching for something that MUST be present and getting 0 for that too. Before believing a
negative search, run it against a known positive.
