# `karma-decomp/test/` — every gate and harness, with the command

**One block per script: what it answers, and the exact invocation.** `../HANDOVER.md` §4 has the
nine-gate block you run every time; this file covers all 36 files, including the ones you only reach
for when something is wrong.

```bash
cd /home/ion/engines/engine-ut2004/karma-decomp
MT=../Thirdparty/metoolkit                 # the shipped SDK
LIB=$MT/lib.rel/linux_single_gcc3.2        # its i386 archives
```

`/tmp/kd_out/allobj` is the recovered `.c`, `/tmp/kd_build` the compiled `.o`. **`/tmp` is
volatile.** Copy out anything you want to keep.

> **Read the per-object lines, never the summary.** `substitute_test.sh`'s
> `substituted and ran cleanly : 140` is a CRASH count, not bit-identity — `pass` is incremented in
> all three branches including `[ diverg]`, so the summary cannot go down for a numerical difference
> at all. This file's own project quoted it as bit-identity once and a bisect had to catch it.

---

## The nine gates — run all of them, every time

### `ons_smoke.sh` — does this build survive five minutes of Onslaught?

```bash
./test/ons_smoke.sh <binary> <label> [seconds]          # 300 s default
ONS_MAP=ONS-Adara ONS_RENDERER=-OPENGLRENDERER ./test/ons_smoke.sh ...
```

Exit 0 = survived, 1 = crashed or died early. **ONS is the gametype that spawns VEHICLES**, and
vehicles plus ragdolls are most of what Karma exists for; a DM map exercises almost none of it.
This is the gate that caught `McdGjkFaceQueueInit` writing float bit patterns into
`_McdGjkFace.fi[]` — a fault the 51/51 browser suite could not see, because that suite is almost
entirely DM.

⚠ **The renderer is not the same on every target.** Windows has no GLES; passing the default
`-GL4ESRENDERER` there produces a crash that looks like a physics bug and is not one.

### `ktrace_run.sh` / `ktrace_diff.py` / `ktrace_score.py` / `ktrace_subst.sh` — does it BEHAVE?

The gate the other twelve could not be. Everything else here compares CALLS — shadow divergences,
per-pair difftests, per-object scene trajectories. This compares the TRAJECTORY THE CALLS ADD UP
TO, on a real map, against a `legacy-karma` build linking MathEngine's originals.

```bash
# 1. the oracle, from the build that links the shipped archives
KD_FRAMES=600 KD_GAME=Onslaught.ONSOnslaughtGame \
  ./test/ktrace_run.sh ../build-legacy-karma/Source/SDLLaunch/ut2004-legacykarma-pixo.bin legacy600 120
# 2. the candidate
KD_FRAMES=600 KD_GAME=Onslaught.ONSOnslaughtGame \
  ./test/ktrace_run.sh ../build-native/Source/SDLLaunch/ut2004-pixo.bin cand 120
# 3. read it
python3 test/ktrace_diff.py  /tmp/ktrace-legacy600.csv /tmp/ktrace-cand.csv   # per body
python3 test/ktrace_score.py /tmp/ktrace-legacy600.csv /tmp/ktrace-cand.csv   # MATCH / N MISMATCH

# and to LOCALISE — both controls first, then the complement
./test/ktrace_subst.sh ctl-all  ALL          # must MATCH   — the mechanic works
./test/ktrace_subst.sh ctl-none NONE         # must MISMATCH — the defect is present
./test/ktrace_subst.sh keep-mcd ALL '^@Mcd'  # keep OUR Mcd, take the rest from theirs
./test/ktrace_subst.sh fwd      NONE McdSphere McdTriangleList
```

★ **`-FIXEDFPS` IS NOT OPTIONAL AND THE ENGINE ENFORCES IT.** `KTickLevelKarma` derives its
timestep AND its substep count from `DeltaSeconds`, so two runs at different frame rates integrate
different equations. The hook `appErrorf`s rather than write a trace that would be diffed in good
faith. `ktrace_run.sh` passes it; a browser run must too.

**Read `first`, not `restZ`, when the two traces have different lengths.** `restZ` is the LAST frame
of each file, so comparing a 600-frame run against an 85-frame one compares different instants and
reads like catastrophe. Truncate the longer one first.

`packages/e2e/tools/ktrace-probe.cjs` in the monorepo is the wasm side, driving the launcher FORM
because `?map=` deliberately refuses arbitrary engine switches. Two things cost a run each there:
a console filter containing "Karma" matches every ONS HUD warning on a map called `test-karma-1`
and floods CDP until the trace crawls; and the match PAUSES itself (`ULevel::Tick` guards Karma with
`IsPaused()`), which stops the trace dead at 85 frames while the HUD — which ticks on the render
path, outside that guard — keeps logging every frame. **HUD output is not evidence the level is
running.** 85 frames still answers the question the tool exists for, because the defect it was built
to catch is born at the first ground contact (frame 25) and is over by frame 30.

⚠ **This found what nothing else could, and `difftest_pair.sh` shows why.** That driver substitutes
the Ix* INTERACTION object and links the GEOMETRY objects from the shipped library, so a broken
`McdSphereGetRadius` is never in the loop — it reads byte-identical before and after the repair.
"145 objects pass" is a statement about 145 separate links, not about the program. See
`../proven.txt` `MCD-GEOM-FLOAT-FIELDS`.

### `gjk_bisect.sh` / `gjk_bisect_complement.sh` — WHICH function of an object is wrong?

```bash
./test/gjk_bisect.sh [seconds]              # take F from theirs: does it fix the crash?
./test/gjk_bisect_complement.sh [seconds]   # keep F of ours: does it break theirs?
```

The forward form has a blind spot the complement does not: with TWO defective functions, swapping
one still leaves the other, so every single swap reads "no fix" while the whole object reads
"fixed". That is exactly what McdGjkPenetrationDepth did.

⚠ **Read a positive as a lead, not a verdict.** These measure a COMBINATION, and a mixed pair can
fail where either whole pair succeeds — the complement named the driver defective and repairing
`FaceQueueInit` alone took the match to a clean 300 s with the driver untouched.

Both carry a control (the forward one takes nothing from theirs and must still crash; the
complement takes nothing of ours and must survive) and both refuse to run against a build
directory left holding a hybrid.

### `substitute_test.sh` — breadth: does each object crash a scene?

Swaps each recovered object into the link in place of the shipped one and runs a physics scene.
Cheap and broad: catches crashes, hangs, NaNs and wild divergence across many objects at once.

```bash
./test/substitute_test.sh /tmp/kd_build $LIB test/scene_chain.c
./test/substitute_test.sh /tmp/kd_build $LIB test/scene_boxes_on_plane.c
./test/substitute_test.sh /tmp/kd_build $LIB test/scene_ragdoll.c

grep -c 'trajectory bit-identical' <output>     # the number that means something
grep '^  \[ diverg\]'              <output>     # and the ones that do not
```

Three tiers, not two — `delta == 0` prints `[  ok  ]`, `delta < 1e-3` **also** prints `[  ok  ]`
(a tolerance band), anything else prints `[ diverg]`.

**It prints the FIRST DIFFERING STEP beside the maximum, and on a contact scene that is the column
to read.** A last-bit difference amplifies without bound there, so the maximum stops
discriminating — every object on the path reaches metres. Released objects first differ at `1e-06`
to `1e-08`; `keaLCPSolver` first differed at **3.772e+00**, which is not amplification. Believing
the maximum cost this project a session.

### `difftest_pair.sh` — depth: one interaction, 200k randomised transforms

Runs the shipped intersection function and the recovered one side by side on identical real
`McdModelPair`s and compares the whole `McdIntersectResult`, every contact.

```bash
./test/difftest_pair.sh /tmp/kd_build $MT                          # all pairs
./test/difftest_pair.sh /tmp/kd_build $MT McdBoxBoxIntersect
KD_CORNER=1 ./test/difftest_pair.sh /tmp/kd_build $MT McdCylinderTriangleListIntersect 30000
KD_GRID=1   ./test/difftest_pair.sh /tmp/kd_build $MT McdCylinderCylinderIntersect
KD_JITTER=1 ./test/difftest_pair.sh /tmp/kd_build $MT McdCylinderCylinderIntersect
```

- `KD_CORNER=1` is the cylinder–TriangleList repro the DEFAULT mesh cannot see: the 4×2 patch reads
  0 and this found the live defect at iteration 23624.
- `KD_GRID=1` makes the cylinders **axis-aligned**, which is what the game's are. Without it
  `IxCylinderCylinder` read 0.01% divergence here and 3.8% in a live match.
- `KD_JITTER=1` runs the SHIPPED library against ITSELF on inputs differing by 1e-7 m. **Can the
  original reproduce what you are asking the recovery to reproduce?** For
  `McdCylinderCylinderIntersect`'s `dims` it cannot — it disagrees with itself 7,798 times where our
  recovery disagrees 6,432. Every other pair reads 0, so the probe discriminates.

Verified, 20,000 pairs:

```
McdBoxBoxIntersect: 20000 pairs, 3609 touching (18.0%)
  bit-identical          : 17478 (87.390%)
  DIFFERENT ret/touch    : 0     contact count: 0     contact dims: 0
  -> PASS -- every discrete decision matches
```

> **`../HANDOVER.md` §4 still tells you to stage the quarantined `IxBoxTriList` into a COPY of the
> build dir or this will not link. That is obsolete.** `IxBoxTriList` was released in the eighth
> session and is one of the 145 — the command above links straight against `/tmp/kd_build`. The
> underlying rule has not changed, though: if you ever compile a HELD object, put it in a copy, or
> the other gates start reporting on an object the detectors are deliberately holding back.

### `wasm_check.sh` — portability: does the recovered set build for wasm32?

Compiles everything for wasm32 and diffs the exported symbols against the native build. §12 item 6.

```bash
./test/wasm_check.sh /tmp/kd_out/allobj /tmp/kd_build $MT
```

### `ptrwidth_check.sh` — the pointer-width gate, no arm64 hardware needed

Truncation is a compile-time diagnostic, so this needs nothing to execute. ~13 s.

```bash
./test/ptrwidth_check.sh /tmp/kd_out/allobj /tmp/kd_build $MT
```

Measured on the ninth session's pipeline output, all 145 objects:

```
  armv7a                                0 truncation warning(s) across  0 object(s)
  aarch64                            7714 truncation warning(s) across 95 object(s)
     worst: MdtBcl:2390 MdtLOD:609 IxSphylPrimitives:600 MstUtils:371 MeProfile:308
```

armv7 must read **0** — it is a 32-bit-pointer target, so the recovery's core assumption holds and a
non-zero there would mean the diagnostic is firing on something other than pointer width. arm64's
number falls to ~435 across 48 once `tools/fix_ptrwidth.py` has run, and **that remainder is a
layout defect, not truncation**; `tools/layout_check.py` is its gate.

> `../HANDOVER.md` quotes 6,981/89 in §4 and 7,771/89 in §12 for this same raw number. Neither
> matches; 7,714 across 95 is what the gate prints today. Re-measure rather than quote.

> **A file clang REJECTS emits no warnings**, so this used to report "0 truncations across 0
> objects" for armv7 while two objects were not being compiled at all (`MeDict` on `ulong`,
> `MeSimpleFile_linux` on `__off_t`). It now prints a DID NOT COMPILE line and exits 1. That branch
> is easy to confirm — append `this is not C;` to one source in a scratch directory and the line
> appears.

### `scene_census.sh` — did the code under test even RUN?

Counts every function entry through `-finstrument-functions`, file-local functions included, with no
sampling and no mutation of the code under test. This is what separates "the recovery is exact" from
"the object never executed" — `substitute_test.sh` prints the same line for both.

```bash
KD_CENSUS_VALIDATED=/tmp/kd_build \
  ./test/scene_census.sh /tmp/kd_out/allobj $LIB test/scene_chain.c
```

**`KD_CENSUS_VALIDATED` is not optional.** Without it this sweeps in the quarantined objects too, and
`MdtPartition` alone segfaults the scene before any census is written. A gate that cannot pass is not
a gate — and this one could not, for a while, and read exactly like a regression in whatever you had
just touched.

### `gate_sensitivity.sh` — could the gate have SEEN a wrong answer?

Rebuilds the object with float intermediates forced to storage precision and reports how far the
trajectory then moves — roughly the smallest error the scene could have caught.

```bash
./test/gate_sensitivity.sh /tmp/kd_out/allobj $LIB test/scene_ragdoll.c
```

**Read it as a ONE-SIDED test.** Non-zero proves the gate can see the object. **Zero proves
nothing:** `keaCalcAcceleration_vanilla` reads zero under every rounding probe and is emphatically
not irrelevant — forcing its outputs to 7777 moves `scene_chain` by 8.8e5 m. Its arithmetic is
exact, so no rounding probe can reach it. Settle a zero by mutating that object's outputs by hand.

### `vptr_ab.sh` — does the repaired vtable dispatch reach what it claims?

Six static checks stand behind `fix_vptr_store`, and every one is about the object file. None
observes the dispatch *running*. This devirtualises it and diffs, with a rotated slot and a +12
address point as controls the scene MUST notice.

```bash
./test/vptr_ab.sh /tmp/kd_out/allobj /tmp/kd_build $LIB keaLCPSolver \
    test/scene_chain.c test/scene_ragdoll.c test/scene_boxes_on_plane.c
```

### `make_dropin_metoolkit.sh` — CHECK 2, the deliverable

A metoolkit tree with **no shipped member in it**: the hull archive swapped wholesale, all 145
recovered objects substituted, and then every member still MathEngine's `ar d`'d out.

```bash
./test/make_dropin_metoolkit.sh /tmp/kd_build $MT /home/ion/karma-run/dropin-metoolkit
ar t /home/ion/karma-run/dropin-metoolkit/lib.rel/linux_single_gcc3.2/*.a | sort   # zero shipped
cmake -B ../build-dropin-karma --preset native-karma \
      -DMETOOLKIT_DIR=/home/ion/karma-run/dropin-metoolkit
```

Then play it (§6). Success is "reached `START MATCH`" and ran to the timeout, **against a stock
control on the same map in the same run**.

### The per-function A/Bs

Sharper than any scene, because they name the FIELD that differs instead of the step. Each has a
deliberate wrong-variant control recorded in `../proven.txt`, and the control is what makes the clean
reading evidence.

```bash
# MeMath's pure matrix functions. Control is the shipped function against itself; must read 0.
./test/ab_matrix.sh /tmp/kd_out/allobj/MeMath.c $LIB 200000

# keaIntegrate_pc. BOTH code paths — the scenes never set MdtKeaBodyFlagUseFastSpin,
# so the third argument is not optional.
./test/ab_integrate.sh /tmp/kd_out/allobj/keaIntegrate_pc.c 100000 0
./test/ab_integrate.sh /tmp/kd_out/allobj/keaIntegrate_pc.c 100000 1

# McdContactSimplify, whole buffer bitwise. Must be 100% bit-identical.
./test/ab_contact.sh /tmp/kd_build 200000
# KD_SELFTEST drives the SHIPPED function on BOTH sides, so it must report
# 0 differences — it proves the harness is not inventing agreement.
KD_SELFTEST=1 ./test/ab_contact.sh /tmp/kd_build 20000
# The OTHER control is recorded in proven.txt rather than wired in: the seventh
# session's reverted variant, block base 0x10 high, SEGFAULTS.

# MdtLOD, three settings plus a control. KD_MAXMATRIX lowers the engine's own guard
# via the PUBLIC MdtWorldSetMaxMatrixSize, so the function can be made to execute.
./test/ab_lod.sh /tmp/kd_build $MT
KD_MAXMATRIX=8 ./test/ab_lod.sh /tmp/kd_build $MT
```

Verified, ninth session, all four:

```
ab_matrix     control (shipped vs shipped) 50,000 cases, 0 differ · recovered vs shipped 0 differ
ab_contact    50,000 calls bit-identical, 0 return / 0 contact differences · KD_SELFTEST 100%
ab_integrate  90,000 bodies, qrot=0 vel=0 velrot=0 T=0
ab_lod        KD_MAXMATRIX 128 / 40 / 8 — all three trajectory bit-identical, PASS
```

`ab_lod.sh` is the one to read: `MdtLOD` had a complete, correct, breakpoint-measured
unreachability argument and this found that the object **segfaults on its first real call**.

---

## The 64-bit harness

### `lp64_run.sh` — RUN the recovered library at 64-bit pointer width

x86-64 is the same LP64 data model as arm64, so every struct that moves there moves here. This is
the arm64 defect executing on hardware you have — every document in this project said that could not
be done, and the premise ("nothing here can execute arm64") was true and irrelevant.

```bash
./test/lp64_run.sh                                   # all three scenes
./test/lp64_run.sh test/scene_chain.c
KD_OUT=/tmp/kd_lp64 ./test/lp64_run.sh               # after the tools/ post-passes
KD_KEEP=1 ./test/lp64_run.sh                         # keep the objects and the ASan reports
KD_ASAN_TIMEOUT=120 ./test/lp64_run.sh               # the sanitized scenes can SPIN; see below
```

Builds all 145 objects for x86-64 and drives the scenes under AddressSanitizer, **with an i386
control that must read zero** — and then builds and runs the same scenes with **no sanitizer**,
against a **plain i386 control** of their own. Those last two are the pointer-width verdict; the
sanitized rows are a memory-safety check and are the wrong reference for the numbers.

⚠ **`KD_KEEP=1` EXISTS BECAUSE THE SUMMARY IS A LIST OF SITES AND DIAGNOSING ONE NEEDS THE
REPORT.** The work directory was deleted on exit, so the addresses, the allocation and the
registers — the only things that identify a *truncated pointer* as opposed to a wrong offset —
went with it.

⚠ **A CLEAN SANITIZER IS NOT A PASS, AND THIS GATE USED TO SAY IT WAS.** Every scene returns
non-zero when its OWN verdict fails: `scene_ragdoll` exits 1 on "BLOWN UP", bodies past 1e3.
A pointer that is wrong but still IN BOUNDS is invisible to ASan and perfectly visible to the
scene — measured, the identical sources at i386 read "plausible". The exit status now counts.

First run, first statement of the first scene:

```
MdtWorld.c:98  heap-buffer-overflow, WRITE of size 4, 48 bytes past a
564-byte region from (MeMemoryAPI.create)(0x234)
```

`0x234` is 564 is `sizeof(MdtWorld)` on i386; at 64-bit it is 880. The i386 control ran 900 steps
with **zero** errors on the same sources and sanitizer, which is what makes those pointer width
rather than the recovery.

### `lp64_pipeline.sh` — the post-passes, the acceptance test and the harness, in order

`lp64_run.sh` runs whatever is in `$KD_OUT`, and `tools/`'s two post-passes are what make that
LP64-correct — on a COPY, because they edit in place. Doing it by hand is four commands with two
ways to get the order wrong and one easy way to forget the copy and overwrite `/tmp/kd_out`.

```bash
./test/lp64_pipeline.sh                      # all three scenes
./test/lp64_pipeline.sh test/scene_chain.c
```

It **stops before the harness** if the i386 acceptance test is not clean, because a post-pass that
changed the shipped target is a bug in the post-pass, and every LP64 row after it would be measuring
that instead of pointer width.

Where it stands as of 2026-08-29, with both i386 controls clean:

| | first thing it hits |
|---|---|
| start of the LP64 work | `MdtWorld.c:98` — the FIRST STATEMENT of the first scene |
| start of this session | `MdtBcl.c:519` (chain, ragdoll) and `MstUtils` (boxes) |
| after the collision chain | no memory error on any scene; the PLAIN build still SIGSEGV on two |
| now | **all three plain scenes clean, 901 rows, 5 runs of 5; one trajectory defect left** |

```
== LP64, no sanitizer ==
  scene_chain            exit 0  901 rows   vs PLAIN i386: first difference step 4  (9.0e-10)
  scene_boxes_on_plane   exit 0  901 rows   vs PLAIN i386: first difference step 52 (1.5e-03)
  scene_ragdoll          exit 0  901 rows   vs PLAIN i386: first difference step 0  (2.2e-07)
```

★ **READ THE FIRST DIFFERING STEP, NOT THE MAXIMUM** — on a contact scene a last-bit difference
amplifies without bound and all three reach metres by step 900. chain and ragdoll first differ at
1e-9 and 1e-7, which is float noise amplifying; boxes was **bit-identical for 44 steps** and then
jumped to 1.5e-01 in one step, which is not. Repairing that (the geometry AABB family) took it to
step 52 at 1.5e-03 — 100× smaller, seven steps later, and still five orders above the floor.
**Still FAIL, and that is the honest reading.**

★★ **THE REFERENCE HAD BEEN THE WRONG BUILD.** The control was built WITH AddressSanitizer and
the plain scenes were run against no reference at all. ASan's poison is `0xbe` rather than
whatever the plain heap held, so an **uninitialised read** comes back as garbage under it: the
sanitized ragdoll fills with 8,100 non-finite samples and reads BLOWN UP at *both* widths — its
LP64 and i386 traces agreeing to `0.00e+00` — while the plain LP64 run of the same sources is
clean. Diffing plain against sanitized attributes an ASan artefact to pointer width. There is a
**plain i386 control** now, and the plain rows are the pointer-width verdict.

⚠ **`0.00e+00 over 901 rows` WAS PART OF THAT ARTEFACT.** `nan > x` is False, so every non-finite
sample failed the divergence test *and* left the worst delta at zero: it read "matches the control
over all 901 rows" on a trace that was 8,100 NaNs. Non-finite samples are counted and reported.

⚠ **The sanitized contact scenes also SPIN** — state `R` with utime climbing and RSS flat, so a
loop in user code and not a stall, and data-dependent: 3 runs in 20 past 30 s on one build, both
contact scenes every time on the next. Same cause: NaN into a convergence
loop. The ASan timeout is `KD_ASAN_TIMEOUT` (120 s) and the harness says out loud that a timeout
there is not the pointer-width verdict.

⚠ **THE ERROR COUNT WAS ONLY HALF OF WHAT THIS COULD SAY.** A pointer that is wrong but still IN
BOUNDS is invisible to the sanitizer and perfectly visible in the numbers. The control is now
built for EVERY scene, its trajectory kept, and diffed — and a non-zero exit fails the run,
because `scene_ragdoll`'s own "BLOWN UP" verdict was being reported as PASS.

⚠ **AND THE CONTROL FOUND A DEFECT IN THE SCENES THEMSELVES ON ITS FIRST RUN.** `scene_ragdoll`
handed `MstFixedModelCreate` a loop-iteration `MeMatrix4`, and that call keeps the POINTER — so
every obstacle transform was dead stack for the whole run, at every pointer width, for as long as
the scene has existed. 24 sanitizer reports, all in `McdSphylBoxIntersect`. It was missed because
the control was built for the FIRST SCENE ONLY, and the first scene creates no collision models.
See `../proven.txt` `SCENE-RAGDOLL-DANGLING`.

★ **NECESSARY, NOT SUFFICIENT, and the header of `lp64_run.sh` says why.** x86-64 tolerates
unaligned access where arm64 **faults**; these are OFFLINE SCENES; and nothing at 64-bit has yet
exercised a real match, the drop-in link or the `.ka` asset path. A clean run here is the gate
that had to be passed before an arm64 build was worth attempting, not evidence that one will work.

★ **The six defects this leg closed were each invisible until the one before it was repaired**,
and two of them were rules that already existed and had **stopped matching** because an earlier
pass rewrites the spelling they keyed on — `fix_frame_slots`' trailing addend on `(int)` vs
`(kd_iptr)`, and `fix_vtable_offsets`' slot read on `char **` vs `undefined4 **`. Neither printed
a decline, because a pattern that does not fire has nothing to report. The largest was an entire
type family missing from `fix_literal_offsets`' offset map — the SDK's geometries are *anonymous*
struct typedefs. See `../proven.txt` `LP64-THREE-CLEAN`.

⚠ **FIVE RUNS IS THE MEASUREMENT, NOT ONE.** Three separate times this session a single run read
"clean" and the next read three errors, on sources that had not changed: ASLR moves what an
out-of-bounds write lands on, and `-fsanitize-recover` means how far a scene gets changes what it
reports. **Take five, and read the site list.**

See `../proven.txt` `LP64-ONE-REMAIN` for the full accounting, and the `LP64-*` entries for each
class closed.

---

## Bisecting — when an object is known to be wrong

### `bisect_object.sh` — which FUNCTION in the object?

`substitute_test.sh` works one object at a time, which is right for a gate and useless once an
object is known wrong: `keaLCPSolver` has fifteen functions and twelve run on the ragdoll. This
drives the same scene with exactly ONE recovered function in the link.

```bash
./test/bisect_object.sh /tmp/kd_build/keaLCPSolver.o $LIB test/scene_ragdoll.c
./test/bisect_object.sh /tmp/kd_build/keaLCPSolver.o $LIB test/scene_ragdoll.c _ZN12keaLCPSolver8solveLCPEv
```

### `bisect_static.sh` — which FILE-STATIC?

`bisect_object.sh` arbitrates **globals**. `IxBoxTriList` has two, and the defect was in neither.
This measures file-statics through `difftest_pair.sh`.

```bash
./test/bisect_static.sh IxBoxTriList McdBoxTriangleListIntersect --none
./test/bisect_static.sh IxBoxTriList McdBoxTriangleListIntersect --all
./test/bisect_static.sh IxBoxTriList McdBoxTriangleListIntersect McdVanillaBoxTriIntersect 50000
```

Found `IxBoxTriList` in one pass: Ghidra had **deleted two of three reciprocal stores** and pointed
the vector 8 bytes past the end of `lsVec3 axb[3]`. difftest went 1,463/139,961/12,060 → **0/0/0**
over 200,000 pairs.

### `trace_cylcyl.sh` — print both implementations' outputs side by side

For when a divergence has no textual fingerprint and you need the numbers. Uses
`tools/mk_trace_obj.py` rather than `bisect_static.sh`'s mechanism, which produced a `call 0` inside
the shipped code on this object: it carries file-static DATA shared with `EndCapMin` plus a weak
`CylPerpAndPara`, and globalising and renaming all of it changes which code runs.

```bash
./test/trace_cylcyl.sh 2000
```

The last open pair. **Read `../proven.txt` `CYLCYL-GRIND` before re-opening it** — four candidate
fixes and one wholesale lever were tried and every one measured WORSE or made no change.

### `try_subst.sh` — which SUBSET of objects breaks the engine?

The 108-object substitution segfaulted in `KCreateAssetDB` before a match started. That is early
enough that a 60 s run settles it, which makes bisecting 108 objects affordable at ~2 min a cycle.

```bash
./test/try_subst.sh /tmp/kd_build                       # all of them
./test/try_subst.sh /tmp/kd_build MeAssetDBXMLIO MeFAsset   # only these
```

Prints RUNS or CRASHES plus the top frame, and leaves the engine log in place.

### `crash_ab.sh` — is it the build, or the weather?

The `KHandleCollisions` SIGSEGV is situational: the same binary and URL have produced a clean match
and an immediate crash on consecutive attempts. Comparing "four runs of A this morning" against
"four runs of B this afternoon" cannot separate the build from the weather, so this interleaves
A,B,A,B and keeps every log at `/tmp/kd_ab_<arm>_<run>.log`.

```bash
./test/crash_ab.sh test-karma-1 300 11 \
    stock=../build-native-karma/Source/SDLLaunch/ut2004-karma-pixo.bin \
    subst=../build-subst-karma/Source/SDLLaunch/ut2004-karma-pixo.bin
```

---

## Building metoolkit trees

Three different questions, three different trees. `make_dropin_metoolkit.sh` (above) is the fourth
and the deliverable.

### `make_shadow_metoolkit.sh` — a tree that shadow-tests itself

Renames the shipped function `orig_X` and the recovered one `rec_X`, and defines `X` in
`kd_shadow.c` to call both and compare. Every existing caller — all of UT2004 — therefore calls the
shadow. **Real inputs from a real match**, which is the strongest evidence available here.

```bash
./test/make_shadow_metoolkit.sh $MT /tmp/kd_build /tmp/mt_shadow
```

It cannot prove the recovered code is *usable*: the engine consumes the ORIGINAL's answer, so an
error that would compound frame to frame never gets the chance to.

### `make_substituted_metoolkit.sh` — the engine actually RUNS on recovered code

The harder question. Recovered objects replace the shipped ones outright.

```bash
./test/make_substituted_metoolkit.sh /tmp/kd_build $MT /tmp/mt_subst
./test/make_substituted_metoolkit.sh /tmp/kd_build $MT /tmp/mt_subst McdBox McdSphere
```

### `make_hull_lib.sh` — swap qhull for the replacement

`libMcdConvexCreateHull.a` is qhull 2.6, 1.4 MB. This builds an archive from
`src/McdConvexCreateHull/kd_convexhull.c` instead, about 10 KB, with an identical exported symbol
set. A whole-archive swap, because this one is a REPLACEMENT rather than a recovery.

```bash
./test/make_hull_lib.sh $MT /tmp/mt_newhull
```

### `hull_probe.sh` / `hull_ab.sh` — is the replacement hull right?

Tier 1 proves a hull is **valid** — sentinels, Euler characteristic, closed anticlockwise rings,
paired edges, outward normals. Tier 2 proves it is the **same solid**. Equality is not available as
a test: the hull reindexes, so two correct implementations can disagree about every index.

```bash
./test/hull_probe.sh $LIB                                   # the SHIPPED library: ground truth
KD_HULL_IMPL=src/McdConvexCreateHull/kd_convexhull.c ./test/hull_probe.sh $LIB
./test/hull_ab.sh src/McdConvexCreateHull/kd_convexhull.c $LIB
```

---

## Running the game

### `run_map.sh` — play a map headless under the shadow harness

```bash
./test/run_map.sh test-karma-1 300
./test/run_map.sh DM-Rankin 200 "?Game=XGame.xDeathMatch"
```

Runs under Xvfb, so no GPU or display is needed. The harness rewrites its CSV as the match proceeds,
not only at exit, so a timeout or a crash still yields data.

**Gametype matters more than it looks:** ONS vehicle factories only spawn under the Onslaught
gametype, and a map whose only physics comes from bots and vehicles produces no Karma traffic at all
under the wrong one.

**Use `-SOFTWARERENDERER`.** `-GL4ESRENDERER` and `-OPENGLRENDERER` both fault at the first HUD
frame in this environment — **for stock too**.

`KD_SELFTEST=1` runs the harness against itself and must be clean before you believe anything it
says.

---

## The scenes and the C drivers

| file | what it is |
|---|---|
| `scene_chain.c` | 12-body articulated chain on joints, **collision-free** — the authoritative *trajectory* signal. Not solver-free: it drives `keaLCPSolver::solveLCP` 900 times, `MdtKeaAddConstraintForces` 900 and `keaPoolAlloc` 14,400, measured with gdb breakpoint counts on the stock build |
| `scene_boxes_on_plane.c` | boxes tumbling, bouncing, settling. Diverges by design, but exercises the geometry dispatch — it caught the `__regparm` parameter shift after the collision-free scene had passed it |
| `scene_ragdoll.c` | nine capsules on ball-socket joints dropped onto a plane and boxes. Exists because the other two make **not one Sphyl call** between them, and `IxSphylPrimitives` is 463,782 calls of the real game's load |
| `scene_ragdoll_lod.c` | `scene_ragdoll` with the level-of-detail path forced on, for `ab_lod.sh` |
| `difftest_pair.c` | the precision-tier driver `difftest_pair.sh` builds |
| `ab_matrix.c`, `ab_integrate.c`, `ab_contact.c` | the per-function A/B drivers |
| `hull_probe.c` | the convex-hull contract, every invariant `McdQHullTypes.h` claims in prose |
| `kd_shadow.c` | the in-game shadow harness — hooks the registration, not the functions |
| `kd_instr.c` | the exact per-function call counter `scene_census.sh` links |
| `kd_recovered_list.h` | generated by `make_shadow_metoolkit.sh`; `kd_shadow.c` includes it. Not hand-edited |

---

## What was actually run when this file was written — 2026-08-27, ninth session

**Every script in this directory was run, including the ones that need a built engine and a live
match.** Because "documented" and "works" are different claims, and this project has been bitten by
the difference more than once.

| harness | result |
|---|---|
| `substitute_test.sh` ×3 scenes | 145/145 ran cleanly, 0 crashes; divergences only the known seven |
| `difftest_pair.sh` all 14 pairs | **reproduces the documented baseline EXACTLY** — IxBoxBox 1 count, IxSphereTriList 137 dims, IxCylinderCylinder 1 count + 20 dims, the other eleven 0/0/0. Exits 1 on that aggregate: **the three FAILs are the known limits, not a regression** |
| `difftest_pair.sh` `KD_CORNER=1` | cylinder×TriList 0/0/0 PASS (was 1,463/139,961/12,060 before the dropped-rounding fix) |
| `difftest_pair.sh` `KD_GRID=1` | IxCylinderCylinder 6,385 dims — the documented axis-aligned figure |
| `difftest_pair.sh` `KD_JITTER=1` | **the SHIPPED library against ITSELF: 64,033 ret, 9,744 count, 975 dims.** Our recovery reads 1 count / 20 dims. The reference is less self-consistent than the replacement |
| `wasm_check.sh` | 145/145, exported symbol sets identical |
| `ptrwidth_check.sh` | armv7 0 across 145; arm64 7,714 across 95. DID-NOT-COMPILE branch confirmed by feeding it a broken source |
| `scene_census.sh` ×3 scenes | 103 / 311 / 312 functions executed |
| `gate_sensitivity.sh` ×3 scenes | matches §4a's table (keaCalcConstraintForces 4.3e-04 chain → 6.8e+01 boxes) |
| `vptr_ab.sh` ×3 scenes | control bit-identical on all three; **both wrong versions CRASH** on ragdoll and boxes — `[blind]` on chain, which is the honest reading |
| `lp64_run.sh` ×3 scenes | **FAILS as designed** — i386 control 0 errors, LP64 5 ASan errors per scene in `MdtWorldCreate`/`MallocCreateAligned`. A clean run here would mean the harness had stopped working |
| `ab_matrix.sh` | 50,000 cases, 0 differ; control (shipped vs shipped) 0 differ |
| `ab_contact.sh` | 50,000 calls bit-identical; `KD_SELFTEST` 100% |
| `ab_integrate.sh` | 90,000 bodies, qrot=0 vel=0 velrot=0 T=0 |
| `ab_lod.sh` | `KD_MAXMATRIX` 128 / 40 / 8 — all three trajectory bit-identical |
| `bisect_object.sh` | keaLCPSolver, every function `identical` |
| `bisect_static.sh` `--none` / `--all` | both PASS |
| `trace_cylcyl.sh` | 500 calls, 340 diverged (ret 0, dims 0) — the residue §12 check 3 records |
| `hull_probe.sh` shipped + `KD_HULL_IMPL` | both build hulls at every tolerance |
| `hull_ab.sh` | all volumes identical; the diff is last-digit float noise in one normal |
| `make_hull_lib.sh` | 1,416,194 → 10,382 bytes, 15 symbols identical |
| `make_substituted_metoolkit.sh` | 145 substituted, 0 not library members |
| `make_shadow_metoolkit.sh` | 19 objects staged, 24 functions compared |
| `make_dropin_metoolkit.sh` | 146 members — 145 ours + `kd_convexhull.o`, **ZERO MathEngine**, 33 deleted |
| **the drop-in engine** | links (42,813,220 bytes) and **plays 180 s, 0 faults, with a stock control on the same map showing an IDENTICAL Karma warning profile** (4/2/2/2). Check 2, end to end |
| `crash_ab.sh` | stock vs drop-in, 2 runs each interleaved, 120 s — 0 crashes both arms |
| `run_map.sh` + the SHADOW engine | test-karma-1, 240 s: **1,571,707 real calls, 0 ret_diff / 0 count_diff / 0 dims_diff / 0 overrun / 0 nonfinite** |
| `run_map.sh` `KD_SELFTEST=1` | 850,177 calls shipped-vs-shipped, 100% identical, worst delta exactly 0 — which is what makes the row above evidence |
| `try_subst.sh` | all 145 substituted: **RUNS, reached START MATCH** |

### Three operational facts learned by running them

1. **`try_subst.sh` hardcodes `$ROOT/build-subst108` and only *builds* it — it does not configure
   it.** If that directory does not exist the tool fails. Create it first:
   `cmake --preset native-karma -B build-subst108 -DMETOOLKIT_DIR=/tmp/mt_subst`.
2. **`run_map.sh` hardcodes `-GL4ESRENDERER`, and in this environment it works.** `HANDOVER.md`
   §7d and §3c both say to use `-SOFTWARERENDERER` because GL4ES faults at the first HUD frame
   "for stock too". That did not happen in any of the eight matches run this session, stock or
   recovered. The advice may be stale or environment-dependent; the script's default is fine.
3. **Bot-dependent maps produce NO Karma traffic headless here.** `ONS-CBP2-Tropica` (twice, 240 s
   and 360 s) and `DM-BE-Clearing` all loaded, ticked to "Finished precaching textures" and exercised
   **0 of 38** pairs, with 0 faults. `test-karma-1` is reliable precisely because §6 says it needs no
   bots — its Karma load is scripted actors. Use it unless you specifically need another pair.


---

## Before you trust a green result

The checklist that is worth more than the next ten objects (`../HANDOVER.md` §12 has the full form):

- Does the stub the code calls back into actually *depend* on the arguments it is given?
- Is any input fixed for the whole run that the game varies? (Box dimensions were, for the project's
  whole life. Hull vertex counts and triangle counts still are.)
- Does the harness share mutable state with the thing it measures? (It did, through `m_cachedData`,
  for months.)
- **Did the code under test even execute, and could the measurement have seen it if it were wrong?**
  (`scene_census.sh` and `gate_sensitivity.sh`.)
- Does the failure message identify the failure? (Four objects once said "Traceback (most recent
  call last):" and nothing else.)
- Is the test drawing from the distribution the game draws from? (`KD_GRID`.)
- **Can the ORIGINAL reproduce what you are asking the recovery to reproduce?** (`KD_JITTER`.)
- If a SEARCH came back empty, can it find anything at all?
- Does the code path you are calling unreachable actually terminate there?
