# `decomp/docs/STATE.md` — the running record

Where the recovery is, what is decided, and what is a trap. **Newest first.** This is the file to
read before resuming; `../HANDOVER.md` is the depth behind it and `../proven.txt` is the evidence.

> **Provenance.** Everything below the 2.44 entry was written in the ufront monorepo's
> `docs/migration/STATE.md` between 2026-08-28 and 2026-08-29, while the recovery still lived on
> `engine-ut2004`'s `karma/decompile` branch. It moved here verbatim in stage 2.44 so the project
> carries its own history and the monorepo stops carrying ghost activity. Paths in those entries
> are as they were written: `engine-ut2004/karma-decomp/` is this repository's `decomp/`,
> `Thirdparty/metoolkit` is `../metoolkit`, and `/home/ion/tools/karma-lab` is `../lab`.

---

★★★ **NEWEST: 2026-08-30 (later still) — THE LP64 PORT IS DONE, and "done" now means
something the old gate could not express: with the arithmetic held fixed, `scene_chain` and
`scene_ragdoll` are BYTE-IDENTICAL at i386 and LP64 over all 901 rows.**

**The located defect was real and it was not the cause.** The entry above named
`trilistgeom[3].mRefCtAndID` and specified its repair. Both stand — the repair is written and
landed. But `scene_ragdoll` is capsules on a **plane**; there is no triangle list in it, so that
site never runs there. Fixing it moved the run-to-run variance from 4 distinct outcomes in 10 to
9 in 40 and no further. What a census of the scene's own path found instead:

```c
McdSphylPlaneIntersect                     /* 7,298 calls a run */
  pvVar9 = McdModelGetGeometry(p->model1);
  fVar2  = -*(float *)((kd_iptr)pvVar9 + 0x14);
```

`McdSphyl::mRadius` and `mHalfHeight` are 0x10 and 0x14 here and **32 and 36** at LP64, so at
64-bit the capsule's radius comes out of the middle of its own base class — bytes 16..19 of
`_McdGeometry`, the low half of the `prev` **pointer**. An address, read as a float, straight into
the contact solver.

⚠ **And it had been reported for months**, in a line nobody costed: `fix_literal_offsets`' own
note, *"44 declined — an `Ix*` function handles TWO geometries, so per-file inference cannot
work"*. Those 44 were not a tail.

**What landed.**

| | |
|---|---|
| `tools/interaction_types.py` | NEW, evidence only. The type is written down in the REGISTRATION: `McdSphylGetTypeId()` returns 5, `McdPlaneGetTypeId()` 3, and `McdSphylPlaneRegisterInteraction` installs `intersectFn` then calls `SetInteractions(frame, 5, 3)`. 26 interaction functions typed with nothing inferred |
| `fix_literal_offsets.py` | consumes it, and grew one more site shape — `(base + iVar12 * 4 + 0x10)`, an indexed array MEMBER the old pattern could not see. That one is `McdBox::mR[i]`, and at LP64 it made `McdSphylBoxIntersect` produce **two fewer contacts** than i386. 835 → 936 rewrites |
| `tools/fix_index_layout.py` | NEW — the residue `fix_derived_fields` cannot type, per VARIABLE rather than per file, with a FRAME test and a measurement of whether the index is wrong before touching it. 38 re-spelled, **33 measured as already correct**, 13 declined |
| `tools/fix_narrow_loads.py` | applied at last, 44 sites in 9 objects, wired in last on purpose |

★ **THE MEASUREMENT THAT REPLACED THE GATE.** `-mfpmath=387` was believed to make the two widths
compute the same numbers — `lp64_run.sh`'s header said so. **It does not.** It equalises the
FORMAT, not the EXCESS PRECISION, and an 80-bit x87 register is spilled at different points by the
two ABIs. The old 1e-5 floor was absorbing that: the same sources at the **same width** differing
only in `-mfpmath` first differ at 2.0e-07 and reach a worst of **1.97e+02** — the identical worst
the i386/LP64 pair reached. With `-ffloat-store` at both widths the arithmetic is identical and the
comparison becomes a bit comparison with no tolerance at all.

**Proven, on the tree this landed with:**

```
i386 acceptance   145 object(s), 0 compile failure(s), 0 byte difference(s)
determinism       100 runs of the plain -m64 ragdoll -> ONE distinct output;
                  setarch -R gives the same one
-ffloat-store     chain BYTE-IDENTICAL 901 rows · ragdoll BYTE-IDENTICAL 901 rows
                  boxes differs at line 96  <- LP64-CONTACT-ORDER, pre-existing
wasm32            all 146 shipped sources compile BYTE-IDENTICALLY before and after
amd64 oracle      22 of 22 rewritten offsets confirmed against MathEngine's own build
truncations       181 -> 134 at aarch64, 0 at armv7a
lp64_pipeline.sh  -> PASS
```

✅ **The web build is measurably unchanged**, not argued to be: `sizeof(void *)` is 4 on wasm32 and
every rewritten expression folds back to the constant it replaced, and this time that was compiled
and compared rather than reasoned about. No re-stamp needed.

⚠ **What is NOT closed.** `scene_boxes_on_plane` is byte-identical for 94 steps and then differs by
2.0e-04 — an **order**, not a value: the same two contacts arrive swapped at the two widths. It
predates every pass in this pipeline (the same comparison on the previous tree differs at the same
line) and is pinned by name in `lp64_run.sh`, so its presence does not fail the gate and any change
to it does. `proven.txt` `LP64-CONTACT-ORDER`. And `ptrwidth_check` still counts 134 truncations —
none of them moves any scene, which is a measurement over three scenes, not a proof over the corpus.

Evidence: `../proven.txt` `LP64-CLOSED` and `LP64-CONTACT-ORDER`.

---

★★★ **PREVIOUS: 2026-08-30 (later) — THE LP64 DEFECT IS LOCATED.** ⚠ Its closing claim — "the
LP64 port is not done" — was right, and its implied claim that THIS site explained the
nondeterminism was not. See the entry above.
This contradicts what this file and `HANDOVER.md` have implied since 2026-08-29.

**The site, in the shipped sources:**

```c
IxBoxTriList.c:140   pcVar4 = (code *)trilistgeom[3].mRefCtAndID;
                     count  = (*(int (*)(...))pcVar4)(...);
```

`trilistgeom` is an `McdGeometry *`; the object behind it is an `McdTriangleList`, and byte 48 of
that object is the triangle-generator **callback**. Ghidra rendered `[base + 48]` as an index through
the type *it* chose. Measured with a probe over the shipped `kd_types.h`:

```
sizeof(struct _McdGeometry)   i386 16    LP64 32
trilistgeom[3] starts at      i386 48    LP64 96
```

So at 64-bit the call target is read from byte 96 — a different field, whose contents depend on what
the allocator put there. **That is the address dependence**, and it is on the ragdoll's own path
(`IxSphylPrimitives:1527`, `IxSphereTriList:129` read the same way).

⚠ **This is the example `layout_check.py`'s own header uses, verbatim**, and that tool still ends its
report with *"What is NOT in doubt: at least one site is measurably wrong on arm64 and nothing in the
toolchain says so."* It was right. `lp64_pipeline.sh` printed `-> PASS` over the top of it because
nothing it ran could see a wrong VALUE that did not also crash.

**The repair is mechanical and is not yet written.** `NAME[k].field`, where `k` is non-zero and
`sizeof(T)` differs between widths, must become `*(FT *)((char *)NAME + K)` with
`K = k*sizeof_i386(T) + offsetof_i386(field)` — a no-op at i386 by construction, correct at LP64.
`layout_check.py` already locates the sites and measures both sizes, so the pass has its inputs;
per this project's own standard each `K` should be confirmed against `amd64_oracle.py` rather than
computed and trusted.

**What was done, and what it did not achieve.** `tools/fix_narrow_loads.py` is new: it widens the
LOAD rather than only the address arithmetic (`(T *)(*(int *)addr)` → `*(kd_iptr *)addr`), 44 sites
in 9 objects. Those are real defects — and `fix_ptrwidth.py`'s header was wrong to predict the
remainder would be literal addresses; of the 151 remaining diagnostics **not one is a literal.**
⚠ **It did not fix the nondeterminism** (8 runs after applying it: still 5 distinct outcomes), so the
pass is **not applied to the shipped tree** — it belongs in the same landing as the index repair.

✅ **NOTHING SHIPPING IS AFFECTED.** Every target that ships Karma today is 32-bit — wasm32, i686
native, i386 Windows — and armeabi-v7a reports 0 truncations. Android's two 64-bit ABIs default to
NO_KARMA. All 145 objects are still byte-identical at `-m32`; the browser suite is still 55/55.

Evidence: `../proven.txt` `LP64-INDEX-LAYOUT` and `LP64-ADDRESS-DEPENDENT`.

---

★★★ **PREVIOUS: 2026-08-30 (after the 2.44 move) — THE LP64 BUILD IS NONDETERMINISTIC, and
that is a real open defect, not a flaky gate.**

Chasing why `lp64_pipeline.sh` reports FAIL intermittently produced a much worse answer than
"unstable threshold". Measured on `scene_ragdoll`, plain `-m64 -O0`, **no sanitizer**, the flags the
library ships with, running **the same binary** each time:

```
-m64, ASLR on , 8 runs : escaped 450 (x4) · non-finite 8100 (x1) · escaped 0 with
                         "motion in last 1 s" = 1158.250897 / 1068.705962 / 1158.250756
-m64, ASLR off, 3 runs : escaped 8100, motion 1.56e15 — identical every time
-m32,           11 runs: escaped 7, motion 1327.174694 — IDENTICAL 11 of 11, ASLR on and off
```

A fixed-input rigid-body simulation is a pure function; two runs differing means it read something
that varies, and with the inputs fixed the only variable is the **address space**. Disabling ASLR
makes it deterministic — that is the confirmation. **The recovered Karma has an address-dependent
read at 64-bit pointer width, and does not at 32-bit.**

**The suspect is named and counted.** `ptrwidth_check.sh` still reports **181 truncation warnings
across 37 objects at aarch64** (armv7a: 0), worst `McdAggregate` 26, `MdtWorld` 12,
`IxConvexTriList` 11, `McdTriangleList` 10, `IxCylinderTriList` 10. The ragdoll is capsules on a
triangle-list floor, so those TriList objects are **on its path** — which refutes the standing claim,
in `HANDOVER.md` §6b and in the engine's CMake note, that "none is on a path the scenes reach".

★ **WHY EVERY GATE MISSED IT** — the reusable part. The plain section only asked "did it exit 0";
the trajectory diff compared ONE 64-bit run against ONE 32-bit run, so run-to-run variance presented
as a small float difference and was read as the float floor; the ASan ragdoll verdict WAS failing and
was written off as a threshold; and i386 byte-identity cannot see it by construction, because the
defect does not exist at 32-bit. `lp64_run.sh` now runs each plain scene **twice and diffs** — four
lines, and it fails immediately.

⚠ **This is the prime suspect for the Android SIGSEGV.** Both Android 64-bit ABIs carry the same
read, and Android's address space is not Linux's: a truncated pointer that perturbs a float here can
be an unmapped address there. Not yet proven.

✅ **No 32-bit target is affected.** wasm32, i686 and armeabi-v7a are untouched — all 145 objects are
still byte-identical at `-m32` and the browser suite is still 55/55. **The web build was never at
risk from this**, and the LP64 post-passes remain no-ops there by measurement.

Full evidence: `../proven.txt` `LP64-ADDRESS-DEPENDENT`.

---

★★★ **PREVIOUS: 2026-08-30 (stage 2.44) — the recovery LEFT engine-ut2004 and is its own repository.**
`10-p/metoolkit-karma-decomp`. Nothing about the recovered code changed: all 145 objects are
byte-identical to what `engine-ut2004`'s `karma/decompile` carried, and the LP64 gate reproduces the
same first-difference magnitudes it did there.

**Why.** The decomp was never engine code. It had grown to 292 commits and 250 files inside the
engine repo, and had pushed six legs of narrative into the ufront monorepo's migration state as well
— so two repositories were carrying a third project's history. It also could not be run by anyone
else: 230 absolute `/home/ion/...` paths across 166 files, a 96 MB Ghidra lab in no repository at
all, and a pipeline that resolved the SDK differently depending on which directory a script lived in.

**What moved, and how.**

| | |
|---|---|
| history | `git subtree split --prefix=karma-decomp` → 292 commits, re-prefixed to `decomp/` with `git filter-repo`. The subtree's HEAD tree is byte-identical to `karma-decomp/` at `7028ede` |
| the SDK | `engine-ut2004/Thirdparty/metoolkit` → `metoolkit/`, all nine configurations, 96 MB. `engine-ut2004` will FetchContent it |
| the lab | `/home/ion/tools/karma-lab` → `lab/`, 77 MB tracked. It was in NO repository and the pipeline cannot run without it |
| the product | `generated/allobj/*.c` → `metoolkit_decomp/src/<Lib>/*.c`, split by the archive each object shipped in, with a CMake package |
| the headers | `decomp/include/kd_*.h` → `metoolkit_decomp/include/` — they ship with the product, every recovered source includes them |
| this record | the six karma/LP64 legs out of the monorepo's `STATE.md` into `decomp/docs/STATE.md` |

★ **THE SPLIT IS READ OFF THE SDK, NOT GUESSED.** `ar t` over
`metoolkit/lib.rel/linux_single_gcc3.2/*.a` says which library each object shipped in; all 145 map to
exactly one. Two things that surfaced doing it, both of which would have shipped silently:

1. **`version.o` is in EVERY archive** — MathEngine compiled a per-library version stamp — so
   "member in more than one archive" is a normal fact about the SDK. It is only an error for an
   object being laid out, which is where the check now lives.
2. **The build directory is the authority on what ships, not the source directory.** `recover.py`
   writes a `.c` for every object it attempts, including the three that do not compile
   (`MeASELoad`, `MeFGeometryFromMesh`, `McduDebugDraw`). The first split laid out 149 sources. The
   split SUCCEEDS either way; the failure lands in the consumer's compile.

★★ **THE HEADLINE GATE WAS FAILING FOR A REASON IT COULD NOT ATTRIBUTE, and that is fixed.**
`lp64_pipeline.sh` set `KD_SKIP_CONTROL=1`, skipping the i386 control — while `lp64_run.sh`'s own
header says "without it none of the rows below mean anything". `scene_ragdoll`'s ASan verdict is
measured-unstable (`proven.txt` `LP64-THREE-CLEAN`: BLOWN UP 3 of 5 at LP64, 5 of 5 at i386, zero
sanitizer errors either way), and the control is the only evidence that can excuse it. Without one,
a coin flip in the scene became `-> FAIL` in the headline gate.

**Measured here, five runs on unchanged sources:** with the control ON, `lp64_run.sh` read **PASS
5 of 5** — the ragdoll blew up in 2 of those 5 and was correctly excused both times, and the
no-sanitizer LP64 run was clean at 901 rows in all five. The default is now control-ON.
⚠ This means the FAIL seen at the start of this session was never a regression, and the "ends
`-> PASS`" recorded on 2026-08-29 was one of the runs where the coin landed the other way.

★ **THE MOVE WAS PROVEN BY BYTE-IDENTITY, NOT BY ARGUMENT.** After the relocation, a full
`recover.py` + `lp64_pipeline.sh` from the new repository with **zero flags and zero environment**:

```
recovery          136 ok + 9 TODO = 145 compiled, 3 fail, 5 skip   (98.0%)  — the baseline exactly
i386 acceptance   145 object(s), 0 compile failure(s), 0 byte difference(s)
frame bounds      0 out-of-range reference(s)
LP64 vs i386      chain 9.0e-10 @4 · boxes 1.9e-09 @52 · ragdoll 2.2e-07 @0   — the float floor
                  -> PASS
```

and every one of the 145 generated sources is **byte-identical to the tree `karma/decompile`
carried**, except two deliberately-changed lines: the provenance header now reads
`decomp/tools/ghidra_clean.py` and `lab/allobj/X.o` instead of one developer's home directory.

⚠ **TWO REGRESSIONS WERE INTRODUCED AND CAUGHT DURING THE MOVE, and the way they were caught is
the reusable part.** Moving the three `kd_*.h` into `metoolkit_decomp/include/` broke every tool
that builds a probe against them:

- `fix_baked_sizeof.py` **caught itself**: `SELF-CHECK FAILED — sizeof(*(MdtWorldID)0) measured
  None, want 564. The size probe is not measuring anything.` That is what those self-checks exist
  for and it stopped the pipeline dead.
- `ghidra_clean.py` had **three more** references that no self-check covered, and the only symptom
  was the recovery quietly dropping from 145 compiled to 131 — `did not compile 3 → 17`. A count
  in a summary was the entire evidence. If the run had not been compared against a known baseline
  it would have read as a plausible result.

**Heads:** `metoolkit-karma-decomp` `main` — pushed. engine-ut2004 `karma/decompile @ 7028ede`
still present and untouched; it is deleted only after the engine is rewired and re-stamped
(stage 2.44 Phase 4–6).

---

★★★ **PREVIOUS: 2026-08-29 (sixth leg) — `generated/` is LP64-correct and checked in; Android
still crashes with Karma on, and those are two different things.** engine-ut2004
`karma/decompile`; the monorepo is untouched except this file.

**Done.** `karma-decomp/generated/allobj` now carries the LP64 post-passes — it used to be the raw
`recover.py` output, which is why every 64-bit target was gated off. All 145 objects still compile
**byte-identical at -m32**, so wasm32, i686 and armeabi-v7a get exactly the object code they had;
`lp64_pipeline.sh` ends `-> PASS`. The CMake `UT_PTR_BYTES` hard-error is a STATUS line now, and
the arm64 APK builds, links and carries the repair visibly — `McdHello` at arm64 does
`mov x8, #0x20`, which is `swap` at its LP64 offset of 32 rather than the i386 16.

★ **THE WEB IS RE-STAMPED (2026-08-30): ut2004 55/55 in 40.5m on the RTX 3090.** `generated/` is
what the wasm presets compile, so the browser build's source changed even though the i386 objects
are identical; `sizeof(void *)` is 4 on wasm32 so every rewritten expression folds back to the
constant it replaced — that was the argument, and this run is the measurement. serve-web is built
from `build-wasm-perf` at engine `7028ede`, wasm `sha256-c8033b9b…` (a genuinely different binary
from the `sha256-63ea3a60…` that last passed, which is why no carry was available). ut99, unreal1
and ut99-436-compat do not link Karma and their artifacts are byte-untouched, so their 39/39,
31/31 and 37/37 stand.

⚠ **One test is now marked FLAKY and retried, and that is a debt.**
`config: a setting the ENGINE writes survives a full reload` intermittently reads back a null
IndexedDB row after a `persistNow()` that reported files harvested. The first full sweep this
session was 54/55 on it and was **not** stamped — the cause was chased first. It is not the
2026-08-28 `persistNow` race (fixed) and **not this change**: the artifact that passed 51/51 at
`f8058ea` was rebuilt byte-identical and fails it too, the same artifact both failed and passed on
consecutive isolated runs, and there are zero commits under `packages/` since that stamp. Owner's
call: mark it clearly, fix it later. The 55/55 was achieved without the retry firing; put the
retry back to zero when the cause is found.

**Not done: Karma on Android SIGSEGVs at startup.** Measured on the API-34 x86_64 emulator, both
arms verified by symbol count in `libUT2004.so` rather than by which flag was passed:

```
Karma ON   SIGSEGV on the engine thread the moment "Initializing GL4ESDrv..." returns   243 lines
Karma OFF  same APK, same ini, same map: reaches UT2k4MainMenu.Opened()                1713 lines
```

⚠ The obvious suspect is in **both** logs and is not the cause: `Package not found: SDL2Drv` /
`GL4ESDrv` appears five times in the run that reaches the menu. I read the first failure as a
renderer problem on that evidence and was wrong — only the control said otherwise.

⚠ **Whether it is LP64 or Android-integration is not known**, and Karma has never been enabled on
Android at *any* pointer width, so a 32-bit ABI separates the two. Neither is reachable here: the
API-34 image refuses x86 ("ABI x86 not supported on this device") and the x86_64 emulator cannot
execute ARM, so armeabi-v7a needs the **OnePlus 6, which was not attached**. That is the next
measurement and it is one command on a connected device.

⚠ The engine installs its own SIGSEGV handler, so there is no tombstone and no backtrace yet.
⚠ And the gradle/CMake cache reported "BUILD SUCCESSFUL in 1s, 32 up-to-date" while shipping the
*previous* physics — twice. `rm -rf app/build/intermediates/cxx` is what reconfigures; every arm
above was checked with `llvm-nm | grep -c McdBoxPlaneIntersect` before it was believed.

**The Android default stays NO_KARMA** — an APK without vehicles beats one that does not start.
`./gradlew assembleDebug -PwithKarma` reproduces the crash.

★★★ **PREVIOUS: 2026-08-29 (fifth leg) — the LP64 pipeline PASSES.** All three scenes run clean at
64-bit pointer width on the plain build (901 rows each, five runs of five), and every trajectory
now first differs at the float floor. All in `engine-ut2004/karma-decomp` (`karma/decompile`); the
monorepo is untouched except this file, and `generated/allobj` is byte-for-byte what it was, so
nothing the browser executes changed.

```
plain LP64 vs plain i386, first differing step / magnitude
  scene_chain            step   4    9.0e-10   noise, amplifying
  scene_boxes_on_plane   step  52    1.9e-09   noise, amplifying
  scene_ragdoll          step   0    2.2e-07   noise, amplifying
```

i386 acceptance **145/145 byte-identical** throughout, `check_frame_bounds` 0. Eight more defects
came out this leg, each invisible until the one before it was repaired — seventeen in the chain.

★ **READ THE FIRST DIFFERING STEP, NOT THE MAXIMUM.** On a contact scene a last-bit difference
amplifies without bound and all three reach metres by step 900. `scene_boxes_on_plane` went
1.5e-01 at step 45 → 1.5e-03 at step 52 → **1.9e-09**, level with the collision-free scene.

★ **AND THE HARNESS NOW GATES WHAT IT MEASURES.** The plain trajectory comparison used to print
and never fail; it fails above **1e-05** on the first differing step — two orders above the noise
and two below the smallest real defect seen. Confirmed both ways: reverting the `McdHello` repair
takes boxes to 1.5e-03 and ragdoll to 4.9e+00 and the run to FAIL, restoring it returns PASS.
⚠ And the gate used to flip on a coin toss, because `scene_ragdoll`'s own ASan verdict is unstable
— i386 BLOWN UP 5/5 against LP64 3/5, zero sanitizer errors either way. The harness keeps the
control's exit code now and refuses to blame pointer width for a verdict the control fails too.

★★ **THE ONE THAT CLOSED IT: a struct addressed as an array of 4-byte words.**

```c
undefined4 *puVar4 = McdFrameworkGetInteractions(frame, type1, type2);
if (puVar4[4] != 0) { /* swap p->model1 and p->model2 */ }
```

`_McdInteractions` opens with four function pointers, so `swap` is the fifth WORD at i386 and
`[4]` is exactly it. At LP64 those pointers take 32 bytes, `swap` moves to `[8]`, and `[4]` reads
the upper half of `intersectFn` — non-zero, so **`McdHello` swapped every pair it was given**.
Invisible to every offset rule because an array subscript is the whole expression: no cast, no
`+ K` to key on.

★★ **How it was found is the reusable part.** The symptom was a box pair reaching
`McdBoxBoxIntersect` with an exactly negated normal and the same penetration to seven digits.
Five instruments, each eliminating a layer: the AABBs (identical), the sweep's own reports
(identical), `CxSmallSort::Insert` (same ids, same order), the pair manager's 56 creations
(identical — `(1,2)` created as `(box0, box1)` at both widths), and finally **the same pair
object read back as (8,7) having been created as (7,8)**. Same object, different contents — that
is what said a *writer* had swapped it, and there was exactly one. ⚠ The first hypothesis was the
broadphase, and every broadphase layer measured identical; the writer was two subsystems away.
⚠ An instrument lied first: `p->model1->mSpaceID` read as garbage at both widths and proved
nothing, while the box's own `mR[0]` is stable across widths and made the swap legible.
⚠ And two *empty* files compared equal — a link failure left both AABB dumps empty and `diff -q`
called them identical. Assert the measurement is non-empty before believing it.

★★ **THE BIGGEST FINDING: AN ENTIRE TYPE FAMILY WAS INVISIBLE TO THE OFFSET MAP.** The SDK
declares every concrete geometry as an *anonymous* struct typedef — `typedef struct { McdGeometry
m_g; MeReal mR[3]; ... } McdBox;`, no tag before the brace — and `fix_literal_offsets` built its
map by scanning for `struct NAME {`. So McdBox, McdSphere, McdCylinder, McdSphyl, McdConvexMesh,
McdAggregate and McdNull all had **empty** offset maps and 412 sites across 16 objects sat in the
"out of scope, the base carries no structure" bucket unexamined. `McdBoxUpdateAABB` therefore read
the box half-extents at the i386 `+0x10/0x14/0x18`; `_McdGeometry` is 16 bytes here and **32**
there, so those land inside the base and read **zero**, the box's AABB came back as a POINT, and
the broadphase never reported the box/plane pair until the box CENTRE was through the plane.
Measured both ways: the AABB reads `±0.505,±0.405,z±0.305` at i386 against `±0.005,±0.005,z±0.005`
at LP64, and a call counter on `McdBoxPlaneIntersect` reads `0,1,2,3,4` across steps 43–47 at i386
against `0,0,0,0,1` at LP64. **The narrow phase was never wrong; it was never called.**

★★ **THE TRAJECTORY REFERENCE HAD BEEN THE WRONG BUILD.** `lp64_run.sh` diffed against an i386
control built WITH AddressSanitizer, and ran the plain scenes against no reference at all. ASan
changes the numbers, so the plain rows need a **plain i386 control** — which the harness now
builds, and which is what makes the first-differing-step reading mean anything.
⚠ And `0.00e+00 over 901 rows` was an artefact of the comparator: `nan > x` is False, so every
non-finite sample failed the divergence test *and* left the worst delta at zero. Counted now.
⚠⚠ **A conclusion was drawn through that broken instrument and it was wrong.** On the strength of
that `0.00e+00` I recorded that the sanitized ragdoll blew up "at both widths" from an
uninitialised read. Repairing the `McdHello` swap removed the NaNs *and* the spinning together —
the NaN storm was downstream of a real LP64 defect. What is true, measured five runs each with
zero sanitizer errors: the ragdoll's own ASan verdict is unstable **at i386 too** (BLOWN UP 5/5
there, 3/5 at LP64), because the scene sits on its `escaped > 1e3` threshold and ASan's layout
tips it. The plain rows are the pointer-width verdict.

★★ **AND TWO OF THE SIX WERE RULES THAT ALREADY EXISTED AND HAD SILENTLY STOPPED MATCHING**, both
because they keyed on a spelling an EARLIER PASS rewrites — `fix_frame_slots`' trailing addend
required a literal `(int)` where `fix_ptrwidth` writes `(kd_iptr)` four passes earlier (20 sites,
no match, **no decline printed**), and `fix_vtable_offsets`' slot read matched `(*(char **)&obj)`
while its own partner rule accepted `char|undefined4|void`. A pattern that does not fire has
nothing to report, so both read as objects with nothing to do. Both matchers are structural now
and both **refuse** rather than half-apply — half-applying that vtable repair is worse than not
applying it, because the address point moved and the slots did not, so `+8` reached slot ONE and
`makeFromColMajorPSM` (nine arguments, called with nine) dispatched into `makeFromJMJT` (seven).

**The rest of the chain** (full detail in `proven.txt` `LP64-THREE-CLEAN`): the unary spelling of
the unsigned-wrap offset (27 sites — and `auStack_3c + -uVar6` addresses a *different* array from
`(kd_iptr)auStack_3c - uVar6` five lines above it, confirmed against six successive `sub
%edi,%esp` in the shipped object); a pointer compared against a 32-bit-narrowed address that
**no compiler diagnostic can see** (`-Wall`, `-Wextra`, `-Wint-conversion`, `-Weverything` all
silent); and the kea pool then being genuinely undersized, where **the rounding addend is elements
too** — `(n & ~15) * 4 + 0x40` has `0x40 == 16 * 4`, and with these matrices `n < 16` so the
addend IS the whole reservation. Scaling only the stride changed the pool by not one byte and read
exactly like a wrong diagnosis.

**Also repaired, and latent:** a table walked by a byte cursor with a baked stride —
`iVar10 = iVar10 + 0x30` in `McdBatchFlatten`, where `0xc` is one `McdBatchContactPool` and `0x30`
is four, 12 bytes each here and **16** there. Six cursors across four objects. ⚠ It is written
down *because* it is latent: repairing it alone changed not one number in any scene. Real by
measurement, and **not** the cause of the divergence it was found while chasing. Bisect before
believing — four for four now.

**New tool:** `tools/fix_pool_reserve.py`. **Changed:** `fix_literal_offsets` (anonymous typedefs,
geometry-registration typing, the word-subscript rule, revocable declines), `fix_strides` (the
byte-cursor shape), `fix_narrow_pointers` (rules I2 and J, anonymous typedefs), 
`fix_vtable_offsets`, `fix_frame_slots`, `lp64_run.sh`, `lp64_pipeline.sh`.

**Heads:** engine-ut2004 `karma/decompile @ 7028ede` — committed, **not pushed**. Monorepo
`main` — this file only. No E2E was warranted: native-only post-pass tooling over a copy in `/tmp`, and
`generated/allobj` is unchanged.

★★★ **PREVIOUS: 2026-08-29 (fourth leg) — the COLLISION HALF was dead at 64-bit pointer width,
and unpicking it was a chain of nine defects, each invisible until the one before it was
repaired.** All in `engine-ut2004/karma-decomp` (`karma/decompile @ ae7eab3`); the monorepo is
untouched except this file and `generated/allobj` is byte-for-byte what it was.

```
scene_chain            ASan clean, plain clean, trajectory matches all 900 rows
scene_boxes_on_plane   ASan clean, plain SIGSEGV after 51 rows
scene_ragdoll          ASan clean, plain SIGSEGV at row 0
```

i386 acceptance **145/145 byte-identical** throughout, `check_frame_bounds` 0. Still FAIL, and
both remaining crashes are the SAME site: `keaLCPSolver::solveLCP`, `keaLCP_new.c:270`.

★★ **THE INSTRUMENT CHANGE IS THE HEADLINE, because it is what found the last three.**
`lp64_run.sh` counted AddressSanitizer errors and nothing else. It now also runs every scene
**without** the sanitizer — and that is not belt-and-braces. **ASan HIDES this entire class:** a
pointer that lost its top 32 bits addresses somewhere in the low 4 GB, and under ASan that
region is MAPPED (its shadow and its allocator live there), so the load succeeds and the run
reads clean. Measured: with ASan reporting "no sanitizer error" on all three scenes, the plain
build segfaulted on two. It also builds the i386 control for **every** scene and diffs the
trajectory, and reports a short run as INCOMPLETE rather than matching — the first version
zipped the traces, so a scene that timed out at row 20 reported "matches" on 20 rows of 900.

**The chain**, in the order it had to be unpicked (full detail in `proven.txt`
`LP64-COLLISION-CHAIN`):

1. **`McdIntersect` dispatched through a NULL** — the interaction table's `intersectFn` read at
   `+8`, which is 16 at LP64, so every dispatch went through `goodbyeFn`. ⚠ The cause was **one
   `\s` in a regex**: `MEPUBLIC` sits on its own line above the declaration, `\s` crosses
   newlines, and the return type came out as `MEPUBLIC McdInteractions*` — which resolves to no
   struct, so the site declined silently. One character, and the whole collision half was dead.
2. **the star is not always written** — `MdtContactGroupID MEAPI MdtConstraintDCastContactGroup`
   returns a handle typedef, so the partitioner reported `contactCount = 0` on every step while
   the packer packed them, and the row budget it sized the kea arena from was 24 where the run
   needed 74.
3. **the kea arena's `+ 0x5c`** is `sizeof(MdtKeaConstraints)`, 92 → 152. ⚠ `proven.txt` said
   this carve-up needed "a static analysis, not a pattern match"; measured, every other term is
   width-independent and it is ONE literal.
4. **`alloca(n * 0x10 + 0x10)`** is `sizeof(McdContactLink)`; the element type is in the cast
   that consumes the block on a LATER line, and the trailing addend is an element too.
5–7. **three more spellings of the invented argument area** — `kd_argarea_X[N]` addressed
   forwards, the same addressed downwards from a derived pointer, and a whole materialised
   `char *kd_frame = alloca(...)` whose trailing constants are the argument words.
8. **a 4-byte read of an 8-byte pointer field**, then offsets off the result with no cast on the
   base for any pattern to key on. Each half needs the other pass's output to be recognisable,
   so `fix_literal_offsets` is now run **twice** — each run resolves one more link of the chain.
9. **`uVar6 * -2` on a `uint`** — unsigned, so it wraps correctly at i386 and moves the pointer
   **four gigabytes** the wrong way at LP64. ⚠ `(kd_iptr)` is the wrong repair and was the first
   tried: it makes the multiply signed and all eight sites decline on byte-identity. `(kd_uptr)`
   **is** `unsigned int` at i386. Preserve signedness.

**Heads:** engine-ut2004 `karma/decompile @ ae7eab3` — committed, **not pushed**. Monorepo
`main` — this file only. No E2E was warranted: native-only post-pass tooling over a copy in
`/tmp`, and `generated/allobj` is unchanged.

★★★ **PREVIOUS: 2026-08-29 (third leg) — MEMORY SAFETY IS CLEAN ON ALL THREE SCENES AT 64-BIT
POINTER WIDTH, and what is left is a TRAJECTORY divergence localised to the collision side.**
All in `engine-ut2004/karma-decomp` (`karma/decompile`); the monorepo is untouched except this
file, and `generated/allobj` is untouched, so nothing the browser executes changed.

With the i386 control now built and run for **every** scene (it was the first scene only, and the
first scene creates no collision models):

```
scene_chain            0 ASan errors   trajectory MATCHES the i386 control (worst 4.9e-04)
scene_boxes_on_plane   0 ASan errors   diverges >1% at step 33
scene_ragdoll          0 ASan errors   diverges >1% at step 1, own verdict BLOWN UP
```

i386 acceptance **145/145 byte-identical** throughout. Still FAIL — but the whole memory-safety
front is closed, and what remains is *located*: both divergences begin at the scene's FIRST
CONTACT, and `scene_chain` is COLLISION-FREE and matches over all 900 steps. **The remaining
defect is on the collision side**, not in the dynamics, the solver or the arithmetic.

**Six classes closed, five new tools.** Full accounting in `proven.txt` `LP64-COLLISION-SIDE`;
each class has its own entry.

★ **1. A `void *` API is typed by its INSTALLER**, and that closed `MdtBcl` — 600 baked offsets
in one object. `MdtBcl.h` declares `void *const constraint`, so it is the ORIGINAL API and no
header can type it; but each of the sixteen `MdtBclAdd*` is *registered as a callback on the type
it takes*, corpus-wide. Which parameter is not guessed either: the one whose every baked offset
lands on a real field of that type.

⚠★ **Raising the offset map's nesting depth alone costs twenty repairs — a regression no gate
here can see.** `McdSpace.c`'s six offsets went from fitting ONE type to fitting twenty-two,
declined as ambiguous, and the tool fell from 22 rewrites to 2. **A decline compiles perfectly.**
The fit is now asked in three tiers, narrowest first.

★ **And the prescribed repair was not byte-identical on the object that mattered.**
`*(T *)&p->FIELD` re-spells the whole address and gcc then commons the base pointer across the
function — `MdtBcl` came back 268 bytes smaller. Substituting the LITERAL and nothing else does
all 600 in one compile.

**2. An Itanium vtable addressed in 4-byte words** (26 sites). At LP64 `+ 8` then `+ 16` reaches
`vtable[3]`, so the call meant for `platformInit` — no arguments — **entered a seven-argument
function** and read its `blist` out of a register nobody set. The arithmetic predicts the observed
callee exactly.

**3. An alignment mask frozen at 32 bits** (36 sites). `& 0xffffffc0` is an `unsigned int`, so at
LP64 it **cuts the top half off the pointer** instead of rounding it down. ★ The first evidence
was a register dump: the faulting address IS the low half of a live pointer. ⚠ The report landed
two files away on correct code; every baked offset in that statement was repaired first, on the
strength of the file and line. **Read the registers, not the line number.**

**4. A pointer held in something four bytes wide** (139 sites) — the class `fix_ptrwidth.py` has
been reporting as "N diagnostic(s) remain". Narrow locals, narrow access types, and pointer arrays
walked — and allocated — four bytes at a time.

**5. Ghidra's invented argument area, addressed in 4-byte words** (22 areas) — and ★ **this
corrects a prediction in `proven.txt`**, which called the class unfixable under the byte-identity
gate because "generated/allobj moves". True of the repair it had in mind (upstream in
`recover.py`); not true of scaling the offsets **and the object** together in a post-pass, where
both fold to the original text at 32-bit width. Measured on `MstUtils`, the object the upstream
attempt "knocked out of the build entirely": byte-identical. **Fourth time this session a class
called unfixable turned out to be fixable — never decline a repair on a prediction.**

⚠★ **And it blinds a gate if you let it.** `check_frame_bounds.py` exists for exactly this defect
and reads *constant* offsets and bounds; after the pass both are constant expressions. A
deliberate out-of-range offset is caught before and **missed after** — which is how the blinding
was confirmed rather than argued. The gate has been taught both spellings and re-checked against
the same deliberate violation.

★★ **6. And the scenes themselves had a defect the control found on its first run.**
`scene_ragdoll` handed `MstFixedModelCreate` a loop-iteration `MeMatrix4`, and that call keeps the
POINTER — so every obstacle transform was **dead stack for the whole run, at every pointer
width**, for as long as the scene has existed. 24 sanitizer reports, all in `McdSphylBoxIntersect`.
⚠ And I nearly reported the opposite: the control binary exits 0 and its own verdict reads
"plausible", so a first pass called it clean and used it as a trajectory reference. **Check the
stderr, not the exit code** — the scene's verdict and the sanitizer's are different questions.

⚠ **THE HARNESS WAS REPORTING A FAILURE AS PASS.** `lp64_run.sh` counted AddressSanitizer errors
and nothing else, so ragdoll exiting 1 on its own "BLOWN UP" verdict was a green row. It now fails
on a non-zero exit, keeps the report (`KD_KEEP=1`), and diffs every scene's trajectory against the
i386 control. ⚠ The control's flags must match down to `-O` and `-mfpmath`: an `-O2` reference
against the `-O1` LP64 build measures x87 excess-precision rounding, and it read plausibly either
way.

**Heads:** engine-ut2004 `karma/decompile` — committed, **not pushed**. Monorepo `main` — this
file only. **No E2E was run this leg** and none was warranted: the LP64 work is native-only
post-pass tooling over a copy in `/tmp`, and `generated/allobj` is byte-for-byte what it was.

★★★ **PREVIOUS: 2026-08-28 (second leg) — Android's LP64 blocker has an ORACLE, one defect class is
closed, and the coverage number everyone has been quoting was the wrong one.** Two pieces of work,
both in `engine-ut2004/karma-decomp`; the monorepo is untouched except this file.

**1. LP64 — the amd64 library is a usable oracle, and it is LLP64.** The owner supplied UT2004
v3369's `metoolkit/lib.rel/win_amd64_single/*.lib`: a 64-bit build of the same MathEngine source,
real x86-64 COFF, not stripped. `tools/amd64_oracle.py` reads it — `McdSphereGetRadius` reads
`0x20` there against `0x10` here — with five self-checks that fail loudly rather than return
nothing. ★ **The trap that would have produced wrong numbers: it is MSVC for Windows, so `long` is
4 bytes there and 8 on Android.** `sizeof(MdtBody)` is 576 / **696** / **704** across i386 / win64 /
Android. So it is the right oracle for *which type a site means* and the wrong number to paste —
the repair is always `sizeof(T)`. `x86_64-w64-mingw32-gcc` reproduces the win64 column exactly,
which is what makes the check mechanical.

With it, the **baked-size class is closed**: 102 allocations + **7 pool element strides** now
`sizeof(T)`. The pool form is the one that mattered — `MePoolAPI.init(&w->bodyPool, n, 0x240, 0x10)`
is `sizeof(MdtBody)` at i386, so at LP64 every body overlaps the next and the crash surfaces two
files away in a red-black tree walk. `lp64_run.sh` went from **failing on the first statement of
the first scene** to getting through world creation, framework creation, every pool and the
interaction table. **It still FAILS** — three classes remain, counted not estimated, in
`proven.txt` `LP64-BAKED-SIZES`.

★ **And the next failure is fully root-caused and small**: `proven.txt` `LP64-REBUILT-DATA`.
`gen_prelude.py` rebuilds exported data symbols as `void *[]`, so `MstUniverseDefaultSizes` — which
`MstTypes.h` declares as 7 ints and 2 floats, 36 bytes on every target — is 36 bytes at i386 and
**72 at LP64**, and every other field reads 0. Chased symptom to cause: ASan SEGV → `CxSmallSort::New`
returning NULL → a probe printing `nobj=0 npairs=0`. Five symbols in two objects; the fix is to emit
a 4-byte slot when no slot carries a relocation (`readelf -r` settles it per symbol), and i386 stays
byte-identical by construction. **No existing gate can see it** — `check_symbol_bindings.py` compares
sizes against the shipped *i386* object, where `void *[9]` is 36 bytes and matches.

★★ **The method lesson, and it changed a tool.** `fix_baked_sizeof.py` said its rewrite was "a no-op
on i386 by construction" and prescribed one spelling. The next site wanted the opposite:
`count << 2` is byte-identical as `(int)(count * sizeof(T))` and **differs** as the prescribed
`count * (int)sizeof(T)`. Both are the same value on every target; which one gcc schedules the same
way is a property of the surrounding function. **The tool now compiles every candidate and keeps
whichever reproduces the baseline object byte for byte**, declining the site if neither does — so
the corpus-wide acceptance test confirms rather than discovers. (⚠ that test only works if the file
keeps its NAME: gcc records the source basename in an `STT_FILE` symbol, so identical text compiled
from `/tmp/t.c` yields a different object and reads exactly like a codegen change.)

**2. Coverage triage — 301, not 1,204.** `tools/cold_triage.py` intersects the census with
`reachable.py`: of 1,877 functions, 673 have run and **301 (16.0%) are reachable, referenced and
never measured** — the rest of the cold set is unreachable object (71), unreferenced symbol (682) or
file-local (150, where symbol reachability has nothing to say and folding them in would be an
artefact). Two corrections fell out: **148 of the "2,025 recovered functions" are
`__x86.get_pc_thunk.*`**, in every `nm`-based count this project has printed; and
**`COVERAGE-CENSUS`'s ragdoll lead is withdrawn** — `MdtLimit` enters 8 of 34 on four of five runs,
so "never executed on ANY native run" was a property of the run set. ⚠ Also measured: **adding maps
buys almost nothing** — `test-karma-1` alone reaches 658 of the union's 673.

⚠ **The trap that nearly published a wrong number:** the first triage ran against a census tree whose
objects were **four commits old**. It resolved cleanly and reported plausible figures for a build
that no longer existed. A census trace is a list of runtime addresses and means nothing except
against the binary that produced it; `cmake` will not rebuild what it thinks is current, so the
failure is silent. Rebuilt and re-ran all five.

**New instruments** (all with self-checks that refuse rather than return zero):
`tools/amd64_oracle.py`, `tools/cold_triage.py`, `test/lp64_pipeline.sh` (copy → both post-passes →
i386 acceptance → harness, stopping before the harness if the acceptance test is not clean).

**Heads:** engine-ut2004 `karma/decompile` — committed, **not pushed**. Monorepo `main` — this file
only. **No E2E was run this leg** (nothing the browser executes changed: the LP64 work is native-only
post-pass tooling and the recovery's `generated/allobj` is byte-identical, verified by re-running
`recover.py`). i386 acceptance 145/145, `amd64_oracle --selftest` 5/5.

★★ **SECOND PASS (same day) — the baked-size class is not one class, it is five, and the i386 gate
could never have validated the most important part of it.**

`lp64_run.sh` now gets `scene_boxes_on_plane` and `scene_ragdoll` from **aborting on the first
statement of the first scene** to **one SEGV each**. `scene_chain` remains a separate front in
`MdtPartition`/`MdtMainLoop`. Still FAIL; the movement is the result.

★ **The gate was blind and seven wrong rewrites had shipped.** `fix_baked_sizeof.py` read the
allocation target's type from the whole FILE, and Ghidra names a local `pMVar1` in every function it
decompiles — `McdModelPairManager.c` declares it four different ways. `MeXMLTree.c:285` and `:286`
had `Attribute` and `AttributeNode` **swapped with each other**. Every one passed byte-identity,
*by construction*: the rewrite is only offered when `literal == sizeof_i386(T)`, so any type of the
right i386 size gives the identical i386 object and a different size at 64-bit — which is the entire
point of the change. **A gate that can only see the target you are not changing proves nothing about
the change.** Fixed by scoping the lookup to the site's own function *and* confirming the chosen type
against the shipped amd64 build (107 of 120 confirm; 13 sit in functions that build lacks and are
counted apart rather than called confirmed).

**Four more classes closed, all verified per site:**
- **rebuilt data symbols** emitted as `void *[]` — `MstUniverseDefaultSizes` is 36 bytes on every
  target and 72 at LP64, so every other field read 0 (5 symbols);
- **base-versus-derived allocations** — `McdBoxCreate` reads as `2 * sizeof(McdGeometry)`, right at
  i386, while the amd64 build passes `0x30`; all seven concrete geometry types now allocate their own
  `sizeof`;
- **word-counted table loops** (3) — recorded here as *possibly unfixable* under the byte-identity
  gate. **That was never measured and it was wrong**: the trip count is constant-folded and both
  candidate spellings reproduce the object exactly;
- **baked field offsets** (22) — `*(T *)((char *)p + K)` → `*(T *)&p->FIELD`, with the concrete type
  inferred from the offsets themselves and confirmed against the amd64 displacements.

⚠ **Two counts this file quoted are wrong.** `layout_check.py`'s "OFFSET 128" is two classes added
together — only 27 are struct-field offsets, the rest are Ghidra's invented stack frames — so it
overstates LP64 exposure 4.7×. And "281 derived-geometry sites" is both an undercount and misnamed:
it came from grepping `_McdGeometry`'s field names, so it finds only the geometry. The shape is
general, and the instance now blocking both scenes is `Link` versus `CxSmallSortMarker` with no
geometry involved.

★★ **THIRD PASS — all three scenes now converge on one front.** One line was the last thing between
the collision side and a clean LP64 run: `pCVar5 = (CxSmallSortRep *)&pCVar5->mAABBMarkers[0].mOrdinate;`
must step two markers per axis, and at i386 `offsetof(mAABBMarkers)+offsetof(mOrdinate)` is 40 —
which is also `2*sizeof(marker)`. At LP64 they are 72 and 80. The shipped amd64 `_Update` does
`add $0x50` (80), which is the confirmation.

⚠ **A confident hypothesis was wrong first, for the third time this session.** The obvious suspect
was the unrolled init loop in `CxSmallSort::New` (baked `0x98` stride, baked `-0x18`). It looks
exactly like the defect and *is* one. Patching all ten of its substitutions changed **nothing**; the
single line above, alone, cleared it.

★ **The spelling that keeps byte-identity is the reusable part.** `(char *)p + 2 * sizeof(marker)` is
the obvious repair and is **not** byte-identical — gcc re-allocates registers across the whole
function, 508 differing instructions, for the same address; four spellings were tried and all four
differed. What works is to **keep the expression Ghidra wrote and add a correction term that is
provably zero on the shipped target**. Anchor-and-correct is general for this family and is what
makes the byte-identity gate survivable where a rewritten expression is not.

⚠ **And the tool reported a repair it did not make** — a local shadowed the output `path`, so the
repaired text went to a file named `mAABBMarkers[0].mOrdinate` in the working directory while the
summary said "4 repaired". Diff the source; do not read the summary.

**Where the scenes stand:** all three run through world creation, the framework, every pool, the
interaction table and the whole broadphase before finding anything. Remaining: `MdtPartition` 7,
`MdtMainLoop` 5, `McdBox` 5, `MstUtils` 4, `McdBatch` 2, `McdSphyl` 1 — and `McdBox`/`McdSphyl` are
the **derived-field accessors**, so that class is now on the critical path rather than merely counted.

★★ **FOURTH PASS — the derived-field class, 186 sites, and the Mcd half is effectively clean.**
`*(float *)&(g[1].mRefCtAndID)` is `((McdSphere *)g)->mRadius`: the geometry types derive from
`McdGeometry` by prefixing it, Ghidra has only `McdGeometry *`, and at LP64 the base's fields move
*differently* from the derived struct's — for `McdBox`, `[1].prev` lands at 40 where `mR[1]` is at
36, and `[1].next`/`[1].frame` run off the end.

★ **The access type has to change too**, which separates this from the offset class: the recovery
stores through the *base* field's type — a **pointer** — into what is really a 4-byte `MeReal`. The
read and write shapes are mirror images, because Ghidra types the local as `McdGeometryID` and every
use spells it `*(float *)&pMVar1`, a float in a pointer's clothing.

★ **And here the byte-identity gate is strong**, unlike in the sizeof class where any type of the
right i386 size passed and seven wrong ones shipped: a wrong field is a different *address*, a wrong
type is a different *store width*, and either changes the object. All 186 were compiled and compared.

⚠ Two measurement bugs the self-checks caught, both of which would have read as "nothing here":
`MeVector3` is a `MeReal[3]` **behind a typedef**, so a syntactic array scan left holes and made
`McdTriangleList` ambiguous — **all 70 of its sites declined, which looks like a decision**; and the
bare member name won over element 0 at the same offset.

**Result:** `scene_ragdoll` now fails **entirely** in `MdtPartition`/`MdtMainLoop` with no collision
site at all, and `scene_boxes_on_plane` has one `McdBox` read left. The Mcd half of the library is
effectively LP64-clean under these scenes.

★★ **FIFTH PASS — the partition arena, and a correction I owe twice over.** This file said the
partitioner "will almost certainly not be byte-identical". **It is.** That is the third wrong
prediction of that kind in one session, all three from reasoning about what gcc would emit instead
of asking it.

`MdtPartOutCreateFromChunk` cuts one block into seven arrays with three things frozen at i386: the
header size (64 → 96), the per-body total (32 → 36, because `bodies` is a *pointer* array and the
other four are `int`), and a pointer-sized cursor whose `+ maxBodies` steps 4 here and 8 there —
right for the one pointer array it walks, wrong for the four `int` ones.

★ The obvious repair genuinely does fail, and the reason is the keeper: `info` and `constraints`
**share** the subexpression `(pppMVar2 + maxBodies)`, so rewriting both changes what gcc CSEs — five
of six edits compose and the sixth breaks it, while each is byte-identical alone. **Anchor-and-correct**
fixes it, the same technique the stride case found; it is now **two for two** on classes that looked
unfixable under the gate.

**Result: `scene_chain` 25 → 3 errors, `scene_ragdoll` 19 → 3 — and ragdoll's three are all Ghidra's
INVENTED STACK FRAMES** (`MstUtils`, with `McdBatch` as a consequence), which is a *decompilation*
defect and belongs upstream in the dump rather than in any post-pass. ⚠ Error COUNTS move between
runs (ASan keeps going, so how far a scene gets changes what it reports) — read the site list.

★★ **SEVENTH PASS — one literal byte offset was the whole partitioner failure.**

    if ((*(byte *)((int)pMVar9->data + 0x1ec) & 2) == 0) goto ...

`0x1ec` is `offsetof(MdtBody, flags)` at i386; at LP64 it is **556**. So `MdtUpdatePartitions`'s
VISITED test reads a byte 64 short of the flag, no body is ever seen as visited, the root loop
re-seeds one already placed, and it surfaces as `addedBodies[12]` overflowing a 12-element buffer.

**`scene_chain` went from 25 AddressSanitizer errors to ONE.** Every `MdtPartition`/`MdtMainLoop`
site is gone. Scenes now: **chain 1, boxes 3, ragdoll 3** — and ragdoll's three are all the
invented-stack-frame class. i386 acceptance 145/145 throughout.

⚠ **The symptom looks nothing like a layout defect, and this file said so in as many words** one
pass earlier: "a COUNT overrunning … the next defect is in the partitioner's BOOKKEEPING … and NOT
in any allocation." The mechanism named was right — it *is* the visited test — and the **category**
was wrong, because the reason it fails is a baked byte offset. Measuring `offsetof(MdtBody, flags)`
at both widths settled it in one command; two rounds of reading the traversal logic had not.

⚠ Two probes misled first, both worth keeping: `local_3c = (MdtBody *)((kd_iptr)&(local_3c->keaBody)
.tag + 1)` is an integer increment spelled as an in-struct address and looks exactly like the stride
class — it is **not** a defect, because that offset is 0 on *both* targets. And adding an `fprintf`
moved the failure, which reads like stack corruption; `MdtPartition` has none of the raw frame-slot
form, `recover.py` having already converted all 44 of its slots to named arrays that are in bounds
at both widths. The probe was only perturbing which out-of-bounds byte got hit.

The repair extends `fix_literal_offsets.py` to a **member-expression base**: `pMVar9->data` is a
`void *` and carries no type, so what names it is the same FIELD read into a typed local elsewhere
in the function. ⚠ And the cast may **not** be elided there — the inferred type belongs to what the
member points at, so eliding it emits `(pMVar9->data)->flags`, which does not compile and declines
in a way that reads like the repair being wrong rather than the spelling.

★★ **EIGHTH PASS — all three scenes are down to TWO distinct defects.** A guarded third spelling for
the derived-field *read* shape closed `McdBox`'s last sites, and that cascaded: ragdoll's `MstUtils`
pair vanished with it, and `McdBatch.c:829` turns out to be a *consequence* (`*resultCount = 0`,
where `resultCount` arrived through one of MstUtils' frame slots). **Most of what looked like breadth
was one defect seen from several call sites.**

```
scene_chain            1   MdtBcl.c:519
scene_boxes_on_plane   3   MstUtils.c:91,97 + McdBatch.c:829
scene_ragdoll          1   MdtBcl.c:519
```

**What is left — two things, and NEITHER IS FINISHABLE AS AN INCREMENT.** Both were attempted this
leg and both were stopped deliberately, with the blocker measured:
1. **`MdtBcl` is a genuinely `void *` API.** `MdtBcl.h` declares `void *const constraint`, so the Bcl
   layer knowing its layout by offset is the **original design**, not Ghidra losing a type. Measured:
   no struct with a complete body covers the offsets, per file (54) *or* per function (18 for
   `MdtBclAddBSJoint`), even with array elements and nested members expanded. ★ The amd64 build
   **does** have the LP64 displacements — but using them means **aligning two compilers' instruction
   streams**, a different technique that deserves its own design rather than being bolted on.
2. **`MstUtils` is Ghidra's invented stack frames** — a *decompilation* defect, and the answer to
   "why did the conversion decline here" is now known: its slots are the **single-shift** case that
   `ghidra_clean.py`'s `AREA` rule deliberately excludes, and that exclusion is load-bearing. The
   rule's own note records that extending it to one shift *"changed five of those seven objects and
   knocked MstUtils out of the build entirely."* Measured scope: **124 sites across 8 objects, five
   of them released and validated**. It is a **pipeline** change, so `generated/allobj` moves and
   **byte-identity cannot be the gate** — giving scratch its own storage is a different stack layout
   by construction. Validating it means the nine gates plus the behavioural harnesses on those five.

★ For **`MdtBcl` the type IS derivable**, and the evidence is the **installer**, not the disassembly:
each `MdtBclAdd*` is registered as a callback on the very type it takes —
`(pMVar1->head).bclFunction = MdtBclAddBSJoint;` — so `constraint` is an `MdtBSJoint *`. That
resolves all sixteen mechanically. ⚠ What blocks it is the **offset map**, not the type: with the
installer types the offsets still do not all land (`MdtBclAddContact` 27 of 31, `MdtBclAddBSJoint` 4
of 18), and `0xb4`/`0xb8` miss for **every one of the sixteen** — a uniform miss on a shared `head`
field, i.e. a nesting-depth limit in `field_paths`, which recurses one level. Raising it naively
returns **zero** offsets, because the cycle guard caches an empty dict under `(tag, depth)` and a
deeper walk re-enters it. Fix the guard, then the depth, then re-measure the sixteen.

Lower priority, none of it blocking a scene: five more arena carve-ups
(`MdtKeaConstraintsCreateFromChunk`, four in `MdtWorld.c`), one sibling offset site at
`MdtPartition.c:334`, and 92 derived-field sites (44 genuinely ambiguous, 48 declined).

⚠ **Error counts move between runs** — ASan uses `-fsanitize-recover`, so how far a scene gets
changes what it reports; ragdoll has shown 1 and 3 on consecutive runs of the same binary. Read the
site list, not the number.

New passes this leg: `tools/fix_arena_carve.py`, `alloca` support and a member-expression base in
`fix_literal_offsets.py`. Engine head `ca0716f`, still unpushed.

New passes: `tools/fix_strides.py` (renamed from `fix_word_loops.py`), `tools/fix_literal_offsets.py`,
`tools/fix_derived_fields.py`, all wired into `test/lp64_pipeline.sh`. Engine head `3d5d90f`,
still unpushed.

---

★★★ **2026-08-28 (first leg) — the recovered Karma now RUNS RIGHT on the web.** The owner played it and
reported physics that ran and misbehaved: vehicles sinking through terrain until
`bDestroyOnWorldPenetrate` blew them up, every KActor falling through the floor, friction too high,
ragdolls with no joint limits. **All of it is fixed and measured.** Four real defects, one
regression of my own, one pre-existing glue race — and the method matters more than the list.

**Pushed heads (2026-08-28)** — monorepo `main` = `f8058ea`, engine-ut2004 `karma/decompile` =
`669781e`. Both clean fast-forwards, 0 behind. Browser suite **51/51 (36.9m)**, `yarn gates` all
green, `yarn test` 15/15.

| defect | where | how it was found |
|---|---|---|
| numeric conversion where the original moves 4 bytes | `McdSphere`, `McdTriangleList` | complement bisect on a new per-frame body trace |
| `.ka` joint booleans parsed as floats → **joint limits never enabled** | `MeAssetDBXMLInput_1_0` | complement bisect scored on ragdoll invariants |
| an x87 80-bit intermediate rounded to `float` | `MeMath::MeQuaternionFromTM` | same, on the residual |
| **my own `_UNICODE` guard regression** (tenth session) — broke every native ragdoll | `KUtils`, `KSkeletal` | `#error` inside the real TU |
| `persistNow()` resolved before its IndexedDB writes committed | `packages/glue` | the E2E suite going 51→50 twice |

★ **THE METHOD, and it is the reusable part.** Nine gates and 145 objects read green while the game
was visibly broken, because every gate here holds ONE object against the original in isolation —
`difftest_pair` reads byte-identically before and after the fix, since it links the geometry
objects from the shipped library and the broken accessor was never in the loop. **"145 objects
pass" is a statement about 145 separate links, not about the program.** What found all four was
`karma-decomp/test/ktrace_*` (per-frame body state from inside `KTickLevelKarma`) plus
`ktrace_subst.sh` — rebuild with a chosen SET taken from the shipped library, both controls first,
then the COMPLEMENT (keep one family of ours), which has no blind spot when two objects are
defective, and two were.

⚠ **A CONFIDENT, WELL-REASONED HYPOTHESIS WAS WRONG.** The handover's ranked suspects led with
`MdtContactParams` because friction is a dynamics concern; `keep Mdt` MATCHED and the whole family
is innocent. Collision was handing dynamics a wrong radius and every downstream effect looked like
a dynamics bug. Bisect before believing.

★ **NEW ORACLE: the owner supplied UT2004 v3369's 64-bit build.** `metoolkit/lib.rel/
win_amd64_single/*.lib` is real x86-64 COFF, **not stripped, 1,089 symbols** (the loose `.obj`
files beside them are MSVC LTCG and disassemble to nothing). MSVC x64 has no x87, so float
association is written down there — the question `assoc_scan.py` says it "decides nothing" about.
**And it decided that the obvious fix is wrong**: the order is instruction scheduling and differs
per ROW of one function, our sites are mostly already correct, and the two shipped builds sometimes
disagree with each other. Survey recorded in `karma-decomp/assoc-survey.txt`; **nothing changed**,
because wasm already reproduces i386 to 0.007° so there is no acceptance test for that class.
**The same library is the unblock for Android's LP64 struct layouts** — its field offsets *are* the
layouts `layout_check.py` can only bound. That is the recommended next step, with coverage triage
(`tools/census_report.py`: **656 of 2,025 functions have ever executed**) after it.

Detail lives in `engine-ut2004/karma-decomp/proven.txt` — entries `MCD-GEOM-FLOAT-FIELDS`,
`UNICODE-GUARD-REGRESSION`, `COVERAGE-CENSUS`, `ASSOC-SURVEY`.

---
