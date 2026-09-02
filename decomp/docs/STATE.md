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


# ⚠⚠ OPEN ITEMS — 2026-09-02 (LATE). READ THIS BLOCK FIRST.

★★★ **THE CENSUS'S TWO NAMED REAL DEFECTS ARE REPAIRED, AND THE TWO DOC-VS-CODE GAPS ARE CLOSED.**
The block below closed the hover bike; this one closes the list it left behind.

```
truncation census   UNEXPLAINED 38 -> 30      i386 acceptance 145/145
geometry-offset class: 8 known-unrepaired -> 6, and the 6 have named reasons
run-standalone 12/12 · lp64_pipeline PASS · stack_shift PASS
LP64 ktrace still BYTE-IDENTICAL to the SSE-32 control, 3/3 runs (md5 c31ed77b7323)
```

1. ✅ **`IxBoxTriList` — A POINTER PARKED IN A FABRICATED STACK AGGREGATE.** Ghidra models unrelated
   stack slots as one `McdGeometry MStack_24c;` and then walks whatever each slot holds through
   *that* struct's members. `MStack_24c.next` holds `result->normal`:

   ```
   McdGeometry   i386  prev 4   next  8        LP64  prev 8   next 16
   ```

   so `->prev`/`->next` are `normal[1]`/`normal[2]` at i386 and `normal[2]`/**off the end of a
   three-float vector** at LP64 — written EIGHT bytes wide, because the lvalue is pointer-typed.
   Box-vs-TriangleList is every vehicle on level geometry. New pass
   `fix_slot_pointer_walk.py`, 2 sites, i386 byte-identical.

   ★ **AND ITS SIBLING SLOT IS NOT A DEFECT, WHICH IS WHY THE RULE MEASURES.** `MStack_24c.frame`
   is walked the same way, and `McdFramework::geometryRegisteredCount` is byte 4 and `firstModel`
   byte 8 at **both** widths. Rewriting those would have turned two correct statements into
   expressions that merely agree at i386 — `fix_member_base_walk`'s lesson, applied before it cost
   anything this time.

2. ✅ **`MstModelDynamics` / `MstUtils` — A `void *` SLOT WHOSE TYPE ONLY ITS SETTER KNOWS.**
   `MdtContactGroup::generator` is declared `void *`, so no accessor can type it. The evidence is
   the single call that stores into it: `MdtContactGroupSetGenerator(pMVar5, pair)` in `MstUtils.c`,
   inside `createContactGroup(MdtWorldID w, McdModelPairID pair)`.

   ```
   McdModelPair   i386  model1 0  model2 4  …  responseData 24
                  LP64  model1 0  model2 8  …  responseData 48
   ```

   So `piVar4[1]` was the **high half of `model1`**, not `model2`, and `c->generator + 0x18` was
   `request` rather than `responseData`. ★ The first means `McdModelSetBody` silently dropped every
   contact group whose **second** model was the one being re-parented. New pass
   `fix_setter_typed_slot.py`, 3 sites, and it typed a second slot on the way
   (`MeXMLInput::userdata` is an `MeAssetDBXMLInput *`).

   ⚠ **ONE OF THE THREE NEEDED A WIDTH GUARD, AND THAT IS A MEASUREMENT.** No spelling of
   `(McdModelID)piVar4[1] != pMVar1` reproduces the i386 object — member, address, `&`, and operand
   swap were all tried — because the i386 original genuinely performs a **four**-byte load where
   LP64 needs an **eight**-byte one. `fix_block_copy`'s precedent applies: the i386 text is kept
   verbatim under `#if __SIZEOF_POINTER__ == 4`. ★ That is stronger than an argument about the web
   build — `__SIZEOF_POINTER__` is 4 on wasm32 too, so the shipped artefact keeps the original text
   **by construction**.

3. ✅ **`fix_callback_context` NEVER SAW THE COMMONEST CONTEXT SHAPE.** Its registration rule
   required the context argument to be a bare identifier, so
   `MdtBodyForAllConstraints(pMVar1, transferContactGroups, &bp)` — a **stack struct passed by
   address**, which is how most of them are written — was not merely unrepaired, it was never
   reported. With `&VAR` admitted, `transferContactGroups` types its `ccbdata` as `BodyData` and
   takes 10 edits; the pass goes 0 → 50 corpus-wide.

   ```
   BodyData   i386  oldBody 0  newBody 4  model 8  space 12
              LP64  oldBody 0  newBody 8  model 16 space 24
   ```

4. ✅ **THE TWO DOC-VS-CODE GAPS ARE CLOSED — both promised a measurement and implemented a
   pattern match.**

   - **`int-return`** claimed "a function whose recovered prototype returns a non-pointer" and was a
     hardcoded list of eleven names. It now reads the prototype — the file's own prelude first, then
     the SDK headers, stripping `MEAPI`. ⚠ An unresolvable callee stays UNEXPLAINED; `None` is not
     "benign". ★ **And the class's OTHER half — "and nothing dereferences it" — was never checked at
     all.** It is now, with a measured exception: `&VAR->M` where `M` is at offset 0 at both widths
     is the IDENTITY, not a dereference (`MdtPartition`'s `&pMVar9->left`).
   - **`count-to-id`** claimed "a named field whose size is 4 at both widths" and was a regex that
     could not even parse a dotted path, so `->mHull.numFace` never matched. It now resolves the
     destination and asks the compiler for `sizeof` at both widths.

   ⚠ **THE CAST SPELLING WAS SILENTLY GATING THE LOOKUP.** While the rule was a name list,
   restricting `CALL_RHS` to `(void|char|int|…) *` was harmless; once the verdict came from the
   prototype it suppressed it — which is what kept
   `ppMVar7 = (MdtBody **)MdtKeaMemoryRequired(...)` open while `MdtKea.h` declared it `int MEAPI`.

   ★ **AND THE NEAR-MISSES ARE PRINTED NOW.** Three sites are a rule away from benign and say so
   inline (`keaDebug`'s `piVar15` is `printf`'s return here and a real `int *` sixty lines down —
   Ghidra reuses one name for two live ranges, and a whole-function rule cannot separate them).
   A number with reasons attached is triage; a bare 30 is not.

5. **STILL OPEN — 30, and none of them is known to be a defect.** `MdtPartition` 4 and `keaDebug` 2
   are the near-misses above. `MeXMLParser` 4 are a character value and a loop bound in `char *`.
   `MstModelDynamics` 150 is `*piVar4` = `model1`, which is byte 0 at **both** widths — correct, and
   deliberately left alone by the "only rewrite what moves" rule. The rest —
   `CxSmallSort` 3, `IxConvexTriList` 2, `IxCylinderTriList` 2, `MdtLOD` 2, `McdAggregate`,
   `McdContact`, `McdInteractions`, `McdPlaneIntersect`, `MdtConstraint`, `MeFAsset`, `MePool`,
   `keaLCPSolver`, `keaMatrix_tester` 2 — still need a verdict each, one at a time.

6. ✅ **`McdBatch` — THE AGGREGATE'S ELEMENT TABLE AND LOOP BOUND, AT i386 OFFSETS.** The `if`
   dispatcher is a dispatcher too, and the instance is not always the first argument:

   ```
   bVar3 = (byte)ins1_00->mGeometry->mRefCtAndID;
   if (bVar3 == 8) { … kd_McdBatchFlattenAggregate(context, 0, ins1_00, ins2_00, …); }

   McdAggregate  i386  elementTable 16  elementCount 20  elementCountMax 24
                 LP64  elementTable 32  elementCount 40  elementCountMax 44
   ```

   so `+0x10` and `+0x18` were `McdGeometry`'s `next` and `frame` **pointers** at LP64 — an element
   table and a loop bound taken out of two addresses. `fix_typeid_dispatch` grew an `if`-dispatcher
   rule that tracks the ARGUMENT POSITION and maps it to the callee's parameter; **2 of the 5 sites
   repaired** (the `elementCountMax` loop bound), i386 byte-identical.

   ⚠⚠ **THE OTHER 3 ARE STILL OPEN AND NEED A DIFFERENT REPAIR.** `*(int *)(pvVar8 + 0x10)` reads a
   POINTER four bytes wide; fixing the offset alone is not enough, the LOAD WIDTH has to change and
   the value then feeds `*(int *)(<that> + 0x40 + cursor)`. That is `fix_narrow_loads`' territory,
   not an address re-spelling, and no spelling of it reproduces the i386 object — the same wall as
   `piVar4[1]`. ★ The partial repair is strictly less wrong, not more: the loop bound was a
   pointer's low half and is now the real count.

   ⚠ **AND SELF-RECURSION IS EXCLUDED FROM THE "no other caller" TEST BY ASSUMPTION, NOT
   MEASUREMENT.** `McdBatchFlattenAggregate` calls itself through INVERTED guards (`if (bVar5 != 8)`
   with the work in the complement) that the pass deliberately does not read. What is relied on
   instead: the callee dereferences the element table unconditionally, so a non-aggregate reaching
   it would already break the i386 build, which runs. That is empirical and it is stated in the
   pass's docstring rather than buried.

7. ⚠ **STILL OPEN IN THE GEOMETRY-OFFSET CLASS: 3 + 3.** `McdBoxMaximumPointNew`'s three box reads
   (nothing in the corpus calls it and no branch encloses it — it is dead here, and a name-based
   rule was deliberately not invented), and `McdBatch`'s three `elementTable` reads above.

---

# ⚠⚠ OPEN ITEMS — 2026-09-02 (EVENING). READ THIS BLOCK FIRST.

★★★★★ **THE HOVER BIKE IS FIXED, AND THE LP64 TRACE IS NOW BYTE-IDENTICAL TO THE 32-BIT
CONTROL.** Not "behaviourally identical at a tolerance" — the same file, md5 `c31ed77b7323`, on
the SSE-32 build and on three consecutive ASLR-on LP64 runs.

```
SSE-32 control            md5 c31ed77b7323   K=1396      15/15 bodies, STATE and CONTACTS
LP64 + repair, runs 1-3   md5 c31ed77b7323   K=1396      40 frames, test-karma-1, Onslaught
i386 acceptance 145/145 · lp64_pipeline PASS · run-standalone 12/12 · stack_shift PASS
web E2E 55/55, 45.0m, on serve-web SDLLaunch.wasm sha256 6a380872… (ufront 2.48)
```

⚠ **THE WEB HALF IS STAMPED, AND IT NEEDED AN IDLE MACHINE.** Two E2E runs failed the file-browser
spec on a 15 s UI timeout while this session was compiling the engine and re-running the pipeline
on the same box; the third, with the machine idle, is 55/55. Do not run the browser suite and a
build at the same time and then reason about the result.

1. ★★★ **THE DEFECT WAS A CONCRETE GEOMETRY'S OWN FIELDS READ AT i386 OFFSETS — and it is a
   class, not a site.** `offsetof`, measured at both widths rather than argued:

   ```
   McdGeometry   i386 sizeof 16                LP64 sizeof 32
   McdBox.mR     i386 16 / 20 / 24             LP64 32 / 36 / 40
   ```

   `McdGjkMaximumPoint`'s Box arm reads `fVar10 = *(float *)((kd_iptr)pvVar9 + 0x10);`, so **at
   LP64 the box's half-extents come out of `McdGeometry`'s `next` and `frame` pointers.**

   ★ **That is why the build was not merely wrong but not the same twice.** `mR[1]` lands on a
   pointer's **high half** — a tiny denormal that changes with the heap layout. `McdBoxGetSlice`
   carries the same defect spelled `pMVar5[1].prev` / `.next`, and `.next` resolves to LP64 offset
   **48 — past the end of a 48-byte `McdBox`**. Box-vs-ConvexMesh is the only pair in the map that
   reaches either function, which is exactly why one body of fifteen misbehaved and fourteen did
   not.

2. ⚠⚠⚠ **"THE PHYSICS DEPENDS ON THE CONTENTS OF THE STACK" WAS WRONG, AND `stack_shift.sh` WAS
   MEASURING ASLR.** The previous block made stack-dependence a headline finding and a new gate.
   The gate is useful; its stated mechanism was not the one operating.

   ```
   ASLR OFF, environment padded 0 / 4 KB / 20 KB / 64 KB    4 of 4 IDENTICAL
   ASLR ON,  no padding at all, same binary, 4 runs         flips between TWO outcomes
   ```

   The environment size was never the variable — it was a proxy that moved addresses. ★ **And the
   two outcomes are why "frame 9" was frame 9 in one session and frame 1 in the next**: every
   measurement this project took of that body was one toss of a coin, including the `-KTRACECONTACTS`
   run the last block built its headline on. The *kind* of finding (a discrete contact difference)
   survived; the frame number never meant anything.

   ⚠⚠ **AND THE RECORDED MISTAKE REPEATED INSIDE THE HOUR.** `-ftrivial-auto-var-init=zero` and
   `=pattern` produced **byte-identical traces**, which reads cleanly as "no uninitialised
   automatic variable is consumed" — and it was one run each, both landing on the same side of the
   coin. Four repeat runs showed *both* builds flipping between *the same two hashes*.
   **ONE SAMPLE THAT AGREES IS SILENCE, NOT EVIDENCE** — and knowing the rule is not the same as
   applying it.

   ★ The method that actually worked: **`setarch --addr-no-randomize` makes the LP64 build
   deterministic**, which turns a coin toss into a repeatable measurement. Every native A/B in this
   project should be taken under it.

3. ✅ **THE REPAIR IS IN THE GENERATOR, AND IT FOUND MORE THAN IT WAS AIMED AT.**
   `fix_typeid_dispatch.py` gains a second **site shape** and two more **evidence sources**:

   - **the baked-offset site** — `*(T *)((kd_iptr)BASE + K)`. ⚠ `fix_literal_offsets` matches this
     exactly and declines it: the base is a `void *` from an accessor, and its "which struct do all
     this file's offsets land on" inference is ambiguous ({0x10,0x14,0x18} fits dozens). The
     missing ingredient was never the pattern — it was the type.
   - **the type id through an INSTANCE** — `b = McdGeometryInstanceGetGeometry(ins)` beside
     `v = (byte)ins->mGeometry->mRefCtAndID`. ⚠ The existing `TYPEVAR` cannot see that: its
     `(\w+)->mRefCtAndID` captures `mGeometry`, a *field* name, so the site's base never matched
     and every such arm declined silently.
   - **a file-local DISPATCHER** — `switch((char)ins->mGeometry->mRefCtAndID)` whose `case '\x02':`
     calls `kd_McdBoxGetSlice(ins,...)`. The callee's `ins` *is* a box because that is the only way
     control reaches it. ⚠ Any caller of the same name outside a case arm disqualifies it; one
     unguarded caller makes the whole claim false.

   **18 sites repaired, every one i386 byte-identical — including 11 in `McdCylinderGetSlice`**
   that nothing had ever looked at. The repair is an **address re-spelling**, never a member read:
   `fix_callback_context` measured that naming two members of one object lets gcc schedule the
   loads differently and the i386 object stops matching.

   ⚠ **STILL OPEN, AND REPORTED RATHER THAN SKIPPED**: the three box reads in
   `McdBoxMaximumPointNew`, and **five more of the same shape in `McdBatch.c`**. Nothing in the
   corpus calls `McdBoxMaximumPointNew` and no branch encloses it, so there is no evidence and the
   pass declines — deliberately, rather than inventing a name-based rule. ★ Those eight sites are
   *visible* now: the pass reports "geometry base X: no type-id evidence" instead of skipping
   silently, which is how this class stayed invisible for three sessions. `McdBatch`'s are the next
   thing to look at, and a cross-file dispatcher rule is what would resolve them.

4. **WHAT THE PREVIOUS BLOCK GOT RIGHT, AND SHOULD BE KEPT.** The `-KTRACECONTACTS` instrument and
   `ktrace_contacts.py` are what turned "the trajectories differ" into "one width generates a
   contact the other does not", and that framing is what made the geometry offsets worth looking
   at. `KD_IXPROBE` localised it to one interaction out of 733. Both stay.

   ⚠ `ktrace_contacts.py --body NAME` crashed with a `TypeError`: the parser only accepts
   `--body=NAME`, and the space-separated form its own docstring documents became an ignored
   positional. Fixed, because a tool that silently compares *all* bodies when asked for one is
   worse than a tool that crashes.

5. **STILL OPEN, UNCHANGED FROM THE BLOCK BELOW** — the truncation census (item 6 there):
   `IxBoxTriList` 2 and `MstModelDynamics` 3 both have evidence gathered and are REAL; then
   `CxSmallSort` 3, `IxCylinderTriList` 148, `MdtLOD` 2, `McdAggregate`, `McdInteractions`,
   `MePool`, `keaLCPSolver`, `keaMatrix_tester` 2. Plus the two doc-vs-code gaps, `count-to-id` and
   `int-return`, which promise a measurement and implement a regex.

---

# ⚠⚠ OPEN ITEMS — 2026-09-02 (SUPERSEDED by the block above; kept for its measurements)

> ★ Items 1 and 2 of this block are the ones the evening block corrects: frame 9 is **fixed**, and
> the "stack dependence" it reports was **ASLR**. The measurements are real; the mechanism named
> for them was not. Item 3's two repairs stand and remain byte-identical.

★★★ **THE HOVER BIKE IS NOW A LOCATED DEFECT RATHER THAN A NUMBER, AND THERE IS A SECOND ONE
UNDERNEATH IT THAT NOTHING IN THIS PROJECT COULD PREVIOUSLY SEE.** Two generator repairs landed,
both i386-byte-identical, 145/145 and `lp64_pipeline.sh` PASS. **Neither of them fixes frame 9, and
this block says so in the same breath as reporting them, because the last five sessions each began
by believing a repair on the right code path was the repair.**

```
1  frame 9 is a DISCRETE difference, not arithmetic   ktrace_contacts.py, new
       ONSHoverBike3 vs StaticMeshActor2, Box vs ConvexMesh, McdGjkCgIntersect
       32-bit  touch=1 n=3        64-bit  touch=0 n=0        inputs BIT-IDENTICAL
2  LP64 physics DEPENDS ON THE STACK                  stack_shift.sh, new
       32-bit  15/15 bit-identical at four env sizes
       LP64    ONSHoverBike3 moves; the other fourteen bit-identical
3  Windows x64 BUILDS AND PLAYS                       2651 frames, then a Karma fault
4  libsamplerate fragility REMOVED                    no /tmp, no root, in-tree
```

1. ★★ **FRAME 9 IS A CONTACT THAT ONE WIDTH GENERATES AND THE OTHER DOES NOT — MEASURED, NOT
   INFERRED.** The named open item from the previous block asked for contacts per body per frame at
   both widths. The engine now has `-KTRACECONTACTS` (`C` rows = groups/contacts per body,
   `K` rows = each contact's position, normal, penetration) and `ktrace_contacts.py` reads them.
   It answered on the first run:

   ```
   frame 8   32-bit groups=0 contacts=0     64-bit groups=0 contacts=0
   frame 9   32-bit groups=1 contacts=3     64-bit groups=0 contacts=0    <- the defect
             world  n=(0.00195408,-0.707112,0.707099) pen=0   x3, one 45° face
   ```

   ★ **And the 1.06 this file has quoted for two days is those contacts' y-impulse**: at frame 9 the
   32-bit bike takes `vy = -1.06071818` and the 64-bit one stays at `vy = 0`. Not a float floor, not
   a tolerance verdict — an impulse that one build applies and the other does not.

   **Localised to one interaction and no further, with the engine ruled out.** A sequence probe on
   `KIntersect` (every nearfield pair, its geometry types, `touch` and `contactCount`) is
   **identical for 733 lines** and first differs at exactly this pair:

   ```
   -  IX ONSHoverBike3(t2) StaticMeshActor2(t7) touch=1 n=3      (32-bit)
   +  IX ONSHoverBike3(t2) StaticMeshActor2(t7) touch=0 n=0      (LP64)
   ```

   `t2` is Box and `t7` is ConvexMesh, so the function is `McdGjkCgIntersect` — and this is the
   **only** Box-vs-ConvexMesh pair in the map (29 calls, 5 of which touch at 32-bit and 0 at LP64).
   The engine's own two contact-group paths, repulsor and wheel, log **identical sequences at both
   widths**, and the triangle-list generator returns `ntri=0` for this body on both. So the wrong
   answer is computed inside the recovered GJK, from inputs that are bit-identical up to it.

2. ⚠⚠ **THE LP64 BUILD'S PHYSICS DEPENDS ON THE CONTENTS OF THE STACK, AND THE 32-BIT BUILD'S DOES
   NOT.** Run the same binary with 20 KB more environment — nothing else changed — and:

   ```
   32-bit (SSE)   15 of 15 bodies BIT-IDENTICAL, at four environment sizes
   LP64           ONSHoverBike3 diverges; the other fourteen BIT-IDENTICAL
   ```

   ★ **No instrument this project had could see this.** The i386 acceptance test compiles the same
   text at 32-bit, where the read is in range and the object is byte-identical. `ktrace` against a
   control reports only *that* the widths differ, which reads as arithmetic. And **ASan is blind to
   it by construction** — it reports out-of-bounds and use-after-free, not reads of uninitialised
   memory; it ran the failing binary for fifteen frames and reported nothing. `stack_shift.sh` is
   the gate that can, and it belongs in the standing set.

   ⚠⚠ **AND THE FIRST ATTRIBUTION FROM IT WAS WRONG, IN THIS PROJECT'S OWN RECORDED WAY.** A build
   with `-ftrivial-auto-var-init=zero` on **C only** PASSED, which pointed straight at an
   uninitialised local in the recovery — the engine is C++ and would have been untouched. That was
   one run. The same binary FAILED the same test half an hour later, and swapping **all 146** Karma
   objects from that build into the plain one still failed. **A single passing run is silence, not
   evidence**; the harness now samples four environment sizes and requires all of them to agree.
   What is established is the measurement, not the attribution: something on the physics path reads
   memory it did not write, it is one body, and it is not present at 32-bit.

3. ✅ **TWO GENERATOR REPAIRS, AND NEITHER IS THE FRAME-9 CAUSE.**

   **(a) `fix_typeid_dispatch.py` (NEW) — GJK's convex-mesh margin.** `fix_derived_fields` declines
   a **polymorphic** function because its per-file type inference has no answer there:
   `McdGjkMaximumPoint.c  4 site(s): 38 concrete type(s) fit [16, 44]`. But `McdGjkFatness` branches
   on the type id and the source therefore *states* the type. Reading the branch instead of
   inferring:

   ```
   McdConvexMesh   i386   mHull 16   mFatness 44   mBSRadius 48   mBSCenter 52
                   LP64   mHull 32   mFatness 80   mBSRadius 84   mBSCenter 88
   pMVar3[2].frame ->     i386 byte 44 = mFatness      LP64 byte 88 = mBSCenter[0]
   ```

   GJK was shrinking every convex mesh by a bounding-sphere coordinate. Verified in the artefact:
   `cmpb $0x7` → `add $0x50,%rax`. ⚠⚠ **AND IT DOES NOT MOVE FRAME 9.** On this map the hull's
   fatness and its bounding-sphere centre X are both ≈ 0, so the wrong read returns the right
   number. It was found on exactly the right code path, in exactly the right function, and it is a
   different defect. **Bisect before believing — five for five now.**

   **(b) `fix_narrow_pointers` — a local whose address escapes to a callee that writes it narrow.**

   ```c
   int start;                                             /* the RAW recovery */
   McdConvexMeshMaximumPointLocal(conv,norm,0,dp,&start); /* int *outIndex — FOUR bytes */
   MeSetAdd(&set,(void *)start);                          /* the cast rule A keyed on */
   pMVar14 = pMVar3 + (kd_iptr)pvVar9;                    /* index a vertex array by the key */
   ```

   Rule A widened `int` to `kd_iptr` on the strength of the cast, but the only thing that *writes*
   `start` is the callee, and it writes four bytes. At LP64 the top half is stack garbage and the
   set key carries it. ★ **This is the Windows x64 crash, digit for digit**: `EXCEPTION_ACCESS_
   VIOLATION`, *"Tried to READ address 0xffffffff"*, in `McdConvexMeshPlaneCut`
   (`McdPlaneIntersect.c:213`) — which is the line above. The pass now resolves the callee's
   parameter type and declines; blast radius 39 rule-A widenings → 37. A static sweep for the same
   shape across the corpus finds **0 remaining**.

4. ✅ **WINDOWS x64 IS BUILT AND PLAYS A MATCH — THE LAST UNTESTED TARGET.** ⚠ The artefact this
   file previously called "the Windows 64-bit binary, built with Karma, never run" was **PE32
   i386**; there was no x64 build. There is now (`build-windows64`, `MINGW_TARGET_ARCH=x86_64`,
   `UT_ALLOW_64BIT=ON`, `USE_KARMA_DECOMP=ON`), and getting it to compile took three engine fixes,
   **none of which is about pointer width in the way it first reads**:

   - **`PTRINT` was four bytes with eight-byte pointers.** MinGW targets **LLP64**, where `long` is
     4 and a pointer is 8 — the one data model this tree had never built for. `<windows.h>`'s
     `DWORD` is `unsigned long` too, so PTRINT and DWORD became the *same type* and `UnArc.cpp`
     failed with *"redefinition of `operator<<(FArchive&, PTRINT&)`"*, which says nothing about
     width. ⚠ Widening it to `uintptr_t` everywhere is the obvious fix and would have **broken web
     and i386 Linux**, where `uintptr_t` IS `unsigned int` IS `DWORD`. Only LLP64 moves.
   - **`UT_MSVC_NO_MMX`** — three inner loops were guarded on `_WIN64` when the real question is
     *"is this MSVC?"*. MSVC's x64 compiler dropped the MMX intrinsics; GCC did not. MinGW defines
     `_WIN64` and is GCC, so it took MSVC's branch and failed on `m128i_u32` while the helper the
     other branch needs had been `#ifdef`'d away by the same test.
   - **the crash handler printed 32-bit addresses.** `%08x` truncated the image base, so the base
     read `0x40000000` where the PE's is `0x140000000`, and every rva it printed symbolised to
     `?? ??:0`. Seven candidate frames, seven nothings.

   **The run, and frame advance is verified in the DATA, not the log:** ONS-Torlan, Onslaught, 6
   bots, spectator, `-OPENGLRENDERER`, `-FIXEDFPS=30 -KTRACE`. **2651 physics frames** — 88 s of
   simulated time — with Manta, Hover Bike, PRV and RV all moving between frames 1000 and 2600, and
   66481 contact rows. Then the fault in item 3(b). ⚠ **`-OPENGLRENDERER` is not optional**: Windows
   has no GLES and the `-GL4ESRENDERER` default produces a crash that reads exactly like a physics
   bug (`proven.txt` records it producing a confident wrong conclusion in both directions at once).

5. ✅ **`-L/tmp/kd_lib64` IS GONE.** SDL2 finds the *architecture-independent* `samplerate.h` from
   `libsamplerate0-dev:i386`, appends a bare `-lsamplerate`, and the 64-bit link dies with
   `cannot find -lsamplerate` — a message that names nothing. The workaround was a hand-made symlink
   in a directory that does not survive a reboot. `ut_provide_samplerate_linker_name()` in the
   engine's `CMakeLists.txt` now probes with the compiler, and provides the linker name from the
   runtime SONAME inside the build tree — no root, no apt, nothing outside `${CMAKE_BINARY_DIR}`.
   If the runtime library is missing too it disables SDL's use of it and says which package to
   install. Verified on a fresh configure with no `-L` anywhere.

6. **THE TRUNCATION CENSUS — 85 / 38 OPEN (was 83 / 44), AND THE TRIAGE FOUND THE WINDOWS CRASH.** ⚠ The number is
   still a bound on nothing (`MePoolx` was the vehicle crash and produces zero diagnostics), but
   working through it was not academic: **`McdPlaneIntersect` is the largest single object on the
   list — 16 sites, declined by every pass — and it is where Windows x64 died.** Verdicts so far,
   with the reason rather than a label:

   ⚠ **The total went UP, 83 → 85, and that is the `start` repair working**: restoring
   `int start;` restores the diagnostics that the wrong widening had silenced. A census that only
   ever falls is measuring the wrong thing.

   **Two new MECHANICAL classes, so the verdicts live in the tool and not in this paragraph**
   (`ptrwidth_classify.py`, 44 → 38 open):

   - **`fbits-in-local`** (7: `McdBox` 4, `McdCylinder` 3) — `(Ptr)KD_FBITS(float)` into a BARE
     LOCAL whose every other use is `*(float *)&VAR`. An `int`→pointer cast **widens** and cannot
     truncate; `KD_FBITS` returns `unsigned int`, so the bits sit in the low four bytes, which is
     where `*(float *)&` looks on a little-endian target — and every target here is little-endian.
   - **`int-in-local`** — the same shape with an integer instead of a float. ★ The rule asks the
     DIAGNOSTIC which direction it is (`-Wint-to-pointer-cast` is a widening) rather than
     inspecting the right-hand side for things that look like addresses; the first version tested
     for `&`, `->` and `[` and rejected the commonest shape of the class, where `->` is a member
     *read*.

   ⚠⚠ **AND BOTH RULES DELIBERATELY LET `IxBoxTriList` THROUGH.** It uses the very same `KD_FBITS`
   macro and IS a defect, so the "every use" test is what separates them: a local that is
   dereferenced, indexed through, or handed to a call as a pointer does not qualify. Verified as a
   unit test on both sites.

   - **benign, still classified by hand**: `MeFAsset` 1 and `McdContact` 1 (same `fbits` shape but
     into a struct-slot member, which the rule does not cover); `MdtPartition` 4 — integers in
     pointer locals, and the one use that looks like a dereference, `(kd_iptr)&pMVar9->left`, is
     the **identity**: `left` is `MeDictNode`'s FIRST member, measured, so the offset is 0 at every
     width. `MeXMLParser` 4 are a character value and a loop bound carried in `char *`.
     `McdConvexMesh` 4 are `int`→ID→`int` round trips into 4-byte members — the existing
     `count-to-id` class, which does not match because the destination is a DOTTED path
     (`->mHull.numFace`) and whose rule is a regex rather than the size check its docstring claims.
     `MdtWorld` 2 are `MdtKeaMemoryRequired`, a size-returning call parked in a pointer local —
     `int-return`, whose rule is a hardcoded name list rather than the prototype lookup its
     docstring claims. ★ **Those last two are doc-vs-code gaps worth closing**: both classes
     promise a measurement and implement a pattern match.
   - ★ **REAL, and still open**: `IxBoxTriList` 2 — `MStack_24c.next` holds `result->normal`
     (a `MeReal *`) and `->prev`/`->next` are `normal[1]`/`normal[2]` at i386 but byte 8 and byte
     **16** at LP64, written eight bytes wide, off the end of a three-float vector. On the
     Box-vs-TriangleList path, i.e. every vehicle on level geometry.
   - ★ **REAL, and still open**: `MstModelDynamics` 3 — `transferContactGroups` reads the contact
     group's generator as `int *`, so `*piVar4` and `piVar4[1]` are a truncated `model1` and the
     wrong field at LP64; and its `ccbdata` is a `BodyData *` read at i386 offsets (`+4`, `+8`,
     `+0xc` are `newBody`/`model`/`space` here and `oldBody`-high/`newBody`/`newBody`-high there).
     Reachable: `McdModelSetBody` is called from `KarmaSupport.cpp` and `KSkeletal.cpp`.
   - **not yet resolved**: `CxSmallSort` 3, `IxCylinderTriList` 2, `MdtLOD` 2, `McdAggregate` 1,
     `McdInteractions` 1, `MdtConstraint` 1, `MePool` 1, `keaLCPSolver` 1, `keaMatrix_tester` 2.
     ⚠ `IxCylinderTriList` 296 was checked and IS benign — `triangleData` is a union used as a byte
     OFFSET cursor seeded from `(void *)0x0`, so `.tag` holds the whole value — which is the kind of
     answer the other nine still need, one at a time.

---

# OPEN ITEMS — 2026-09-01 (SUPERSEDED by the block above; kept for its measurements)

> ⚠ Items 4 and 6 of this block are the ones the 2026-09-02 block acts on: the hover bike
> is now located rather than open-ended, and the truncation census has verdicts. Item 3's
> "the Windows 64-bit binary is built with Karma and has never been run" was **wrong** —
> that artefact was PE32 i386 and no x64 build existed. There is one now.

★★★ **THE x86-64 SWEEP IS 8 OF 8, WITH NO KARMA FRAME IN IT.** The named open item from the
previous block is closed, and closing it uncovered five more, each of which was the next crash.
See the entry below. ⚠ **The four "engine UI" failures this block reported for most of the day
were a poisoned run-tree ini in the harness — see item 1**; with the viewport pinned they are
gone.

```
DM-Rankin  DM-DE-Ironic  CTF-FaceClassic  BR-Anubis
ONS-Torlan  ONS-Primeval  VCTF-BE-Dystopia  AS-Convoy
    8 of 8, 150 s each, 0 signals, viewport=1280x720
```

1. ✅ **THE SWEEP IS 8 OF 8, AND THE "ENGINE UI BUG" WAS MY OWN POISONED RUN TREE.** ⚠⚠ This item
   said three different things before it was right, and the first two are worth keeping as the
   lesson: *"the engine's 2D UI under Xvfb"*, then *"not Xvfb — renderer-independent,
   display-independent, width-independent, intermittent"*. Both were descriptions of a symptom
   nobody had found the cause of, and **the cause was in the harness, not the engine**:

   ```
   /tmp/kd_run64/System/UT2004.ini   [SDL2Drv.SDL2Client]
       WindowedViewportX=2
       WindowedViewportY=28          <- and /tmp/kd_runkt had 2x1
   ```

   ★ **The engine WRITES the viewport back on exit**, so one run that came up with a degenerate
   window poisoned every run after it, and the value drifted (`2x1` → `2x28`) as it fed on itself.
   Every native measurement I took today was reading it.

   **The two crash families are both consequences of a 2-pixel-wide window, and both are ordinary
   engine code behaving correctly on absurd input:**

   - `UGUITabControl::PreDraw` `SIGFPE` — `UnGUI.cpp`, `ExtraPerButton = … (BorderOffsets[0] +
     BorderOffsets[2]) / Count`. `BorderOffsets` is `INT` and `Count` is the buttons-placed-in-this-row
     counter, so this is an **integer** divide — `idivl -0x8c(%rbp)` in the disassembly. At a
     RowWidth of 2 **no tab fits in a row**, the row is flushed with `Count == 0`, and the divide
     traps. ★ Not a float exception, so nothing Karma computes can reach it.
   - `FCanvasVertex` after `REALLOC FAILED: NewSize=2554592016` — 2.55 GB, and
     `2554592016 / 24 == 106441334` exactly, i.e. 106 M `FCanvasVertex` of 24 bytes each appended to
     `FCanvasUtil::Vertices` by `DrawTile`.

   **Re-measured with `WindowedViewportX/Y` pinned to 1280×720, and the whole class is gone:**

   ```
   DM-Rankin  DM-DE-Ironic  CTF-FaceClassic  BR-Anubis
   ONS-Torlan  ONS-Primeval  VCTF-BE-Dystopia  AS-Convoy
       8 of 8, 150 s each, 0 signals, viewport=1280x720
   ```

   ⚠ **`sweep64.sh` now re-pins the ini before every map**, because the engine will write it back
   again on the next degenerate exit. ★ And this is why the 32-bit control looked like it
   "confirmed" an engine bug: both binaries read the *same poisoned ini*. **A control that shares
   the defect with the thing it is controlling is not a control.**

2. **KARMA IS NOW THE DEFAULT ON EVERY TARGET, INCLUDING ANDROID** (engine-ut2004, 2026-09-01).
   `Android/app/build.gradle` used to force `-DUSE_KARMA_DECOMP=OFF` unless you passed
   `-PwithKarma`, so every ordinary APK shipped with **no vehicles and no ragdolls**. It is
   `-PnoKarma` now, and that is the opt-OUT. The stale *"Android at 64-bit still crashes with Karma
   on"* notes in `CMakeLists.txt` and `BUILD.md` are corrected with the measurements below.

3. **x86_64 IS IN THE APK AND HAS STILL NEVER BEEN RUN.** ✅ arm64 is measured (23 minutes) and
   ✅ **armeabi-v7a is too** — `adb install --abi armeabi-v7a` puts the 32-bit slice on the same
   OnePlus 6 (`primaryCpuAbi=armeabi-v7a`), and it played the same spectator ONS-Dria match for
   **508 s, 0 signals, frame advance verified**. ★ That is the attribution control: arm64 is not
   passing because 64-bit is lenient — **both widths play**.

4. ⚠⚠ **`ONSHoverBike3` DIVERGES AT FRAME 9 AND IT IS NOT THE FLOAT FLOOR — THIS IS THE ONE REAL
   OPEN DEFECT LEFT.** `ktrace` has produced two comparisons: `test-karma-1` (8 bodies) is a MATCH,
   and `ONS-Torlan` (15 bodies, five hover bikes, an RV, a Scorpion) is `1 MISMATCH`. This file
   first called that MISMATCH a tolerance verdict; **that was tested and disproven** — see the
   entry below. With a 32-bit engine built `-mfpmath=sse` so both sides use the same FP model,
   14 of 15 bodies are **bit-identical over all 89 frames** and `ONSHoverBike3` is bit-identical
   for 8 frames and then steps by **1.06**. Exact agreement followed by a step change of order one
   is not arithmetic. ⚠ It is also **not** `IxConvexLineSegment` (repaired below; the trace is
   bit-for-bit unchanged). ★ **Next measurement: contacts per body per frame at both widths.**
   `-KTRACE` records bodies only, so the engine needs a hook it does not have.

5. **WHAT A "TRUNCATION" IS, BECAUSE THE NUMBER GETS QUOTED WITHOUT IT.** It is one of exactly
   three clang diagnostics — `-Wint-to-pointer-cast`, `-Wpointer-to-int-cast`,
   `-Wvoid-pointer-to-int-cast` — raised when the recovered sources are compiled **for aarch64**.
   The recovery puns pointers through integer slots constantly, because Ghidra recovers a stack
   slot as `undefined4` and the code stores a pointer in it. At 32-bit pointer width that is
   lossless; at 64-bit it drops the top half.

   ★ **So it is a 64-bit-only concern, and armv7 measures 0** — the same sources through the same
   compiler at 32-bit pointer width are clean, which is what makes the arm64 number mean anything.
   Every shipping target (wasm32, i686, armv7, i386 Windows) is unaffected by construction.

   ⚠ **"UNEXPLAINED" DOES NOT MEAN "CRASHES".** It means `ptrwidth_classify.py` has four named
   benign classes and 48 sites fit none of them, so it cannot certify them either way. It is a
   **static** count, not a crash list — e.g. `MdtPartition`'s 4 are `int -> MeDictNode * -> int`
   round trips through a reused local, value-preserving at both widths.

   ⚠⚠ **AND IT IS A BOUND ON NOTHING.** `MePoolx` *was* the vehicle crash this session and produces
   **zero** of these diagnostics, because nothing there is truncated and no cast is narrowed. A
   clean census is a statement about one shape of one class. **Playing the game is the stronger
   evidence, and that is why the long runs below are the headline and this number is not.**

6. **THE TRUNCATION CENSUS IS 83 / 44 OPEN** (was 91 / 52 this morning): the `MStack_26c.flags`
   repair took `IxCylinderTriList` — the worst single object, 6 of 6 — off the list, and the
   accessor-typed repairs took `IxConvexLineSegment` (3 of 3) and `MdtWorld` (3 of 3) with it. ⚠ **But that
   number is not a measure of how much is left.** `MePoolx` was the vehicle crash and is on NEITHER
   list: nothing there is truncated and no cast is narrowed. `ptrwidth_classify` counts one shape of
   one class. Worst remaining: `McdBox` 4, `McdConvexMesh` 4, `MdtPartition` 4, `MeXMLParser` 4,
   `CxSmallSort` 3, `IxConvexLineSegment` 3.

7. ✅ **THE WEB IS 55/55 AND AS-Convoy BOOTS** — `yarn e2e --target ut2004`, 55 passed, 0 failed,
   44.9 m, and `PROBE_MAP=AS-Convoy` reaches `Bringing Level AS-Convoy.myLevel up for play` and
   runs the full 150 s window with 0 abort / 0 `GetFontSizeIndex`. Recorded in the monorepo,
   `docs/migration/STATE.md` 2.46. ⚠ The AS-Convoy verdict is a BOOT, not a match.

8. ⚠ **THE WEB ARTEFACT CHANGED — BUT NOT BECAUSE OF KARMA.** Every Karma repair in this session
   is still wasm32-**byte-identical** (both of the last two were compiled at wasm32 before and
   after and compared). What moved `SDLLaunch.wasm` from `12d70e6c…` to `26f3411e…` is the
   **engine-side viewport clamp** in `SDL2Drv/Src/SDL2Viewport.cpp`, which is compiled into the web
   build like any other engine file. That earns a re-stamp on its own account.

9. **THE ORIGINAL WEB ENGINE ARTEFACT CLAIM, FOR THE KARMA HALF.**
   `targets/ut2004/file-manifest.json` was regenerated against the tree as it now is and verified
   entry by entry: **6167 entries, 0 missing, 0 size mismatches**, 174 files on disk correctly
   excluded (executables, logs, user `.ini`s). And `SDLLaunch.wasm` is **byte-identical** through
   all six repairs — `sha256 12d70e6c…` before and after, measured under **clang**. So the browser
   suite tests the manifest fix, not a new engine.

---


★★★ **NEWEST: 2026-09-01 — THE x86-64 SWEEP HAS NO KARMA FRAME LEFT IN IT, AND arm64 PLAYS AN
ONSLAUGHT MATCH FOR AS LONG AS IT IS LEFT RUNNING.** The named open item —
`McdSphylTriangleListIntersect` → `GenerateTriangleContact`, 6 of 8 gametypes and the device at
140 s — is closed. Closing it uncovered five more, each of which was the next crash, and all six
are repaired. **Every remaining sweep failure reproduces identically on the shipping 32-bit
build.**

```
ALL EIGHT GAMETYPES          150 s each, 0 signals, viewport=1280x720
   DM-Rankin  DM-DE-Ironic  CTF-FaceClassic  BR-Anubis
   ONS-Torlan  ONS-Primeval  VCTF-BE-Dystopia  AS-Convoy
arm64 device   ONS-Dria Onslaught, bots + vehicles + ragdolls, spectator
               140 s  ->  1397 s (23 MINUTES), 0 signals, still running
armv7 device   the SAME match on the 32-bit slice: 508 s, 0 signals
linux x86-64   ONS-Torlan Onslaught, 1500 s (25 MINUTES), 0 signals, 0 Critical
               Error, 0 REALLOC FAILED — as a SPECTATOR *and* as a PLAYER
ktrace         i386 vs LP64, 200 frames, 8 bodies:  rest 8/8  sleep 8/8  MATCH
web            yarn e2e --target ut2004: 55 passed, 0 failed, 44.9m
```

★★ **"START A MATCH AND LEAVE IT RUNNING" IS THE CRITERION, AND THREE OF THE FOUR TARGETS MEET
IT.** Android arm64 23 min, Android armv7 8.5 min, Linux x86-64 25 min — all Onslaught, all with
vehicles and ragdolls, all 0 signals. ⚠ **The Windows 64-bit binary is built with Karma and has
still never been run.**

★ **AND THE LINUX RUN IS CLEAN AS A PLAYER TOO, WHICH IT WAS NOT AN HOUR EARLIER.** The first
attempt died at **17 m 45 s** with `SIGFPE` in `UGUITabControl::PreDraw` ← `PreDrawControls` ←
`UGameEngine::Draw` ← `USDL2Viewport::Repaint`, zero Karma frames — and that was the poisoned
viewport of item 1, not the GUI's fault. Re-run with the ini pinned: **1500 s, 0 signals,
0 Critical Error, 0 REALLOC FAILED**, player-controlled. So the spectator flag is a convenience on
Linux, not a requirement; on the *device* it is still what stops the mid-game map GUI pausing the
level, which is a different problem.

★★ **arm64 PLAYS FOR AS LONG AS IT IS LEFT RUNNING — 1397 s, 0 signals, frame advance verified
every 60 s.** ⚠⚠ **AND THE FIRST ATTEMPT AT THAT NUMBER WAS WRONG IN THIS PROJECT'S OWN RECORDED
WAY.** A player run reached "9+ minutes signal-free" and the screenshot at 13 minutes was
**pixel-identical** to the one before it: the player had died, the Onslaught map GUI had opened,
and `ULevel::Tick` guards Karma with `IsPaused()`. The log kept growing the whole time. **HUD
output is not evidence the level is running** — `test/README.md` says so about `ktrace_run.sh`, and
it is just as true of a device run. The measurement above is a **spectator** (`?SpectatorOnly=1`,
so nothing can open that GUI) with a two-screenshot frame-advance check at every sample.

★★ **AND `ktrace` HAS FINALLY COMPARED THE TWO WIDTHS — the first time this project has had a
64-bit-vs-32-bit trajectory at all.** `test-karma-1`, no bots, 200 frames, 8 bodies, **100 %
overlap**:

```
7 of 8 bodies never differ by more than 0.0001 in position or orientation
KBox_Light   first differs at frame 82;  rest Z  0.884 (i386) vs 0.879 (LP64)
ktrace_score  rest 8/8  sleep 8/8  gone 0  ->  MATCH
```

★★★ **AND A SECOND ktrace, ON ONS-Torlan WITH VEHICLES — 15 bodies INCLUDING FIVE HOVER BIKES, AN
RV AND A SCORPION. ⚠⚠ ITS ONE MISMATCH IS AN OPEN DEFECT, AND THIS FILE CALLED IT THE FLOAT FLOOR
FIRST, WHICH WAS WRONG.**

```
32-bit vs 32-bit   15 of 15 identical      64-bit vs 64-bit   15 of 15 identical
x87-32  vs  SSE-64   12/15 identical; ONSHoverBike3 first differs at FRAME 9
SSE-32  vs  SSE-64   14/15 BIT-IDENTICAL over all 89 frames;
                     ONSHoverBike3 BIT-IDENTICAL to frame 8, then |diff| 1.06 at frame 9
```

★ **THE FLOAT-FLOOR READING WAS TESTED AND DISPROVEN.** The first reading was that x87's 80-bit
intermediates on i386 versus SSE at LP64 seeded a difference that a balanced vehicle amplified —
and the arithmetic supported it: the frame-1 seed was `wz` differing by `1.54e-9`, **0.21 of one
float32 ULP** of the smallest quantity feeding it. So a 32-bit engine was built with
`-mfpmath=sse -msse2` to remove the variable (verified in the artefact: `MdtBclAddAngular3` goes
from **623 x87 / 0 SSE** to **1 x87 / 224 SSE**).

**It half-worked, and the half that failed is the finding.** With the FP model held fixed the
frame-1 seed vanishes — SSE-32 and SSE-64 are **bit-identical, all 13 state columns, for 8
frames** — and then `ONSHoverBike3` jumps by **1.06** at frame 9. **A float floor amplifying looks
like a small difference growing; this is exact agreement followed by a step change of order one.**
14 of the 15 bodies stay bit-identical over all 89 frames, including the RV, the Scorpion, four
other hover bikes and every box.

⚠ **AND IT IS NOT `IxConvexLineSegment` EITHER**, which was the obvious suspect: a hover bike is
held up by repulsor **line checks**, and that object had `3 of 3` unexplained truncations reading
`McdConvexHull` at i386 offsets (`+4` is `face` here and the **high half of `vertex`** at LP64;
`+0x14` is `numFace` here and the high half of `edge` there — measured). It is repaired below, and
the trace afterwards is **bit-for-bit the same**: still frame 9, still 1.061. Not on this bike's
path.

**What is left is a tightly bounded lead rather than an explanation:** one body, one frame, with
the arithmetic proven identical up to it. It is either a discrete decision flipping (a contact
that exists at one width and not the other) or the first entry into code whose rounding differs at
the two widths — and the trace cannot separate those, because `-KTRACE` records bodies, not
contacts. ★ Counting contacts per body per frame at both widths is the next measurement, and it
needs a hook the engine does not have yet.

⚠ **The 5 mm on `KBox_Light` is the same thing on `test-karma-1`, and the reason is the one
`lp64_run.sh` documents**: the shipping 32-bit build computes in x87 with excess
precision and the LP64 build in SSE. The offline scenes already establish BIT-identity once the
arithmetic is held fixed with `-ffloat-store`; here it is not held fixed, so a divergence after
82 frames is expected. ⚠ **And the FIRST attempt at 600 frames could not be compared at all** —
the **32-bit** side stopped at 223 (37 %), in `FCanvasVertex` under Xvfb, and `ktrace_diff.py`
correctly refused a verdict below 90 % overlap. That is the same renderer crash the sweep is
attributing, arriving from a third direction. ★ Note which side is short: the LP64 run captured
all 600.

**THE SIX REPAIRS.** Each bought the next; none was reachable from the one before.

| | | |
|---|---|---|
| 1 | the outgoing argument area BELOW an `alloca`, through a cursor | `fix_frame_slots`, 4th spelling |
| 2 | the same function's triangle stride, `local_5c + 0x18` | `fix_element_stride`, new evidence |
| 3 | `McdContact::element2` at `contacts->normal + 0x18` | `fix_member_base_walk.py` (NEW) |
| 4 | `MePoolx` read at i386 offsets through a `void *` | `fix_callback_context.py` (NEW) |
| 5 | a frame slot's MEMBER holding a pointer; a slot stored narrower than read | `fix_frame_slots`, 5th + 6th |
| 6 | an `alloca`'d array of POINTERS, and `qsort`'s element size | `fix_alloca_elem.py` (NEW) |

★★ **(1) THE STORAGE WAS THE HARD HALF, AND THE `alloca` IS WHERE IT COMES FROM.** The area is
nine four-byte words at `puVar12 + -4 … -0x24`, below a block whose base is where the allocation
*starts* — so scaling the offsets alone doubles it to 72 bytes of nobody's memory. The allocation
grows by the scaled area and the base walks up past it, on one statement:

```
alloca(SIZE)  ->  alloca(SIZE + PAD) + PAD
PAD = (int)(sizeof(void *) / 8) * (9 * (int)sizeof(void *))
```

`4 / 8` is **zero** in integer arithmetic, so `PAD` is 0 at i386 *and on wasm32* and the statement
folds back to `alloca(SIZE) + 0` on every shipping target. In the artefact: `add $0x48,%rax`,
twice. ⚠ And the type group has to admit its own `*`s — written `[\w ]*?` the site pattern matches
`*(int *)` and **not** `*(McdUserTriangle **)`, so the pass saw the four-byte stores, missed every
pointer load, concluded no slot carried a pointer and widened nothing. Zero sites, **no decline**:
a group that never forms has nothing to decline.

⚠⚠ **(2) THE EVIDENCE LICENCES EXACTLY ONE RULE, AND THAT IS A MEASUREMENT.**
`fix_element_stride` could not see `IxSphylPrimitives` — it walks its triangles through byte
cursors, so `fix_word_indexed_struct` has nothing to type in it. The file *does* carry
`alloca(n * (int)sizeof(*(McdUserTriangle *)0))`, which `fix_baked_sizeof --field-allocas-only`
wrote only because the field the block is stored into is declared `McdUserTriangle *`. Run without
restricting that evidence to rule C, the table/cursor fixpoint flooded to **fifty names** and rules
B and D rewrote **twelve reads of a transform matrix**:

```c
*(float *)((kd_iptr)pvVar7 + 0x30)   /* McdModelGetTransformPtr, translation row */
    ->  *(float *)((kd_iptr)pvVar7 + 2 * (int)sizeof(McdUserTriangle))
```

`0x30` is 48 is `2 * 24`, so it survives "whole elements only"; at LP64 it becomes 96 and reads off
the end of the transform. ★ **All sixteen sites passed the i386 byte-identity gate**, because
`2 * sizeof(E)` *is* `0x30` there. **Fifth time** 32-bit byte-identity could not see a semantic
change at 64 — a rule is only ever as wide as its evidence, and the gate cannot narrow it for you.

★★ **(3) ONE DEFECT, SIX GAMETYPES, ONE ADDRESS.** With (1) repaired, all six remaining maps died
at `KPerContactCB+0x57c` ← `KHandleCollisions` ← `KUpdateContacts` — the engine reading the
contacts Karma had just produced, which (1) made it produce for the first time.

```
McdContact   i386  sizeof 40   normal 12  separation 24  element1 32  element2 36
             LP64  sizeof 48   normal 12  separation 24  element1 32  element2 40
```

gcc keeps the array in a register pointing at the first element's `normal`, so `+ 0x18` is byte 36
— `element2` here, the **high half of `element1`** there — and the write is eight bytes wide, so it
lands across two fields. ⚠⚠ **The obvious decomposition is wrong in a way i386 cannot show you**:
`fix_element_stride`'s rule D reads `L` as `q*sizeof(E) + offsetof(member at L % sizeof(E))`, and
byte 24 of `McdContact` is `separation` — a *real* member, at 24 at **both** widths. That compiles
to the same 24 it replaced, passes the gate, and is still four bytes short. The remainder has to be
taken from `b + L`, where `b` is the member the chain is rooted at. ★ **Only 4 of the 8 offset
sites move**: `+ 0xc` (`separation`) and `+ 0x14` (`element1`) are already right, and `McdBatch.c`
writes `+ 0x14` and `+ 0x18` four lines apart. ⚠ And the literal is optional — `McdGjk` and
`McdPlaneIntersect` have no offset that moves at all; their only defect is `cur = cur + 0x28`,
walking 40 bytes over 48-byte elements, and `McdPlaneIntersect` advances its cursor inside the
loop's own **condition**.

★★★ **(4) NO STATIC GATE IN THIS PROJECT CAN SEE THE CLASS THAT WAS CRASHING THE VEHICLES.**
`MePoolxDictNodeAllocate(void *pool)` has no type for its context, so every field is a baked byte
offset:

```
MePoolx   i386  mem 0  isize 4  numrec 8   numfree 12  ifree 16   sizeof 20
          LP64  mem 0  isize 8  numrec 12  numfree 16  ifree 20   sizeof 24
```

`0xc` is `numrec` there, `0x10` is `numfree`, and `4` is the **high half of `mem`**. Nothing is
truncated and no cast is narrowed: **`MePoolx` is on neither the 91 aarch64 diagnostics nor the 52
open ones.** Only running the game found it. ★ And the tombstone is the defect field by field —
gdb at the `SIGSEGV` in `MeDictInsert` ← `MeSetAdd` ← `McdConvexMeshPlaneCut`, a convex mesh being
sliced, i.e. a **vehicle**:

```
nodemem      = 0x7ffffffc66d0                      (MeDictNode[200], 8-aligned)
set.nodepool = { mem = 0x7ffffffc66d0, isize = 12,
                 numrec = 196, numfree = 32767, ifree = -235024 }
```

`numrec` should be 200 and is 196 — decremented four times by the code that means `numfree`.
`numfree` should be 200 and is **32767**, the high half of a stack address, because
`*(MeDictNode **)(pool + 0x10)` stores eight bytes into a four-byte index. The node handed back is
`0x…cc`, which a `MeDictNode` can never be. ⚠ **A bigger stack is not the fix** — `ulimit -s
262144` moved ONS-Torlan past it and AS-Convoy still faulted in `MeSetAdd`, which is what ruled
out the `SEGV_ACCERR` explanation from the previous session.

The type is declared **in the registration** — `MeDictSetAllocator(d, alloc, free, p)` — and ⚠
there are two candidates and position does not say which; the tie-break is a measurement (every
offset must land on a top-level member start at i386, and `MeDict` has none at 4). ⚠⚠ **And the
member spelling is NOT byte-identical, which is why this re-spells the address instead**:
`->numfree` alone is identical, `->ifree` alone is identical, and the two *together* are not — gcc
schedules the member form differently once both loads are visible as fields of one object, and a
typed local does not rescue it. ⚠ The greedy return type ate the name **again**: `static void
MePoolxDictNodeDeallocate(` read as a function called **`e`**, so one callback was typed and the
other silently was not, with no decline printed. Same family as `aiStack_9cb0` → `iStack_9cb0`.

★ **(5) THE REPAIR IS STORAGE, NOT A RE-SPELLING, AND THAT WAS MEASURED.**
`MStack_26c.flags = (McdTriangleFlags)result->normal` — four bytes at *both* widths, so the address
is cut in half. It is `ptrwidth_classify`'s worst single object (`IxCylinderTriList` 6 of 6
UNEXPLAINED) and it is on the ragdoll-vs-**world** path. Rewriting the reads as `result->normal[1]`
is exact — the line above them already does — and is **not** byte-identical, with or without the
now-dead store; its own `kd_iptr` local **is**. ⚠ And for the sixth rule, grouping by address would
be wrong: an argument slot is *reused*, so the same address carries an `int` for one call and a
pointer for the next. The pairing is a store and the **next** read with no store in between.

⚠ **(6) `fix_baked_sizeof` DECLINED THIS BY NAME AND WAS RIGHT TO.** Its `qsort` rule needs a plain
identifier as the base and `MdtLOD`'s travels in an argslot — `void` has no size. The missing piece
was never a better guess at the cast, it was the **element type**, and the oracle has it:
`MeFAssetGetPartsSortedByName(const MeFAsset *const, MeFAssetPart **)`. ⚠⚠ **An argslot is not
evidence**: `kd_argslot_ffffffa8` is cast to `MeHash **`, `MeFJoint **`, `char **`, `void **` *and*
`MeFAssetPart **` inside one function, and taking the first cast off it chose `MeAssetFactory *`.
⚠ And `(\w+)\s*\(([^();]*)\)` cannot see these calls at all — every argument is itself
parenthesised — so the oracle evidence never fired and the pass fell through to Ghidra's guess
**with no decline printed**, twice.

★★ **THE WEB ARTEFACT IS BYTE-IDENTICAL THROUGH ALL SIX, MEASURED UNDER CLANG RATHER THAN ARGUED.**
`SDLLaunch.wasm` is `sha256 12d70e6c44d951af73d8acc407db15db95ebbcdbeaeb3642448ad2448444da07`
before and after: `sizeof(void *) / 8` is 0 at wasm32, `-(k * sizeof(void *))` folds to the
constant it replaced, `kd_iptr` **is** `int`, and every `sizeof(E)` folds to its literal. The
existing web stamp still covers the engine; the only web change this session is the manifest.

⚠⚠⚠ **AND A RED SWEEP IS STILL NOT EVIDENCE UNTIL THE BASELINE IS CHECKED — BUT A BASELINE THAT
SHARES THE DEFECT IS NOT A BASELINE.** For most of this day four maps died in `FCanvasVertex` or
`UGUITabControl::PreDraw`, **the shipping 32-bit build died in the same two functions on the same
maps**, and that agreement was read as "the engine, not us". It was not. Both binaries were
reading the same **poisoned `UT2004.ini`** out of the scratch run tree —
`WindowedViewportX=2, WindowedViewportY=28`, which the engine had written back itself after an
earlier degenerate run. With the viewport pinned the sweep is **8 of 8**. ★ The 32-bit control
only isolates what the two builds do NOT share; it says nothing about what they do. ⚠ **The asset tree also moved under this
measurement**: a JB2004 map pack plus ChaosUT2 / BallisticWeapons / AlienSwarm were installed on
2026-08-31, so "7 gametypes clean" and this run are not measurements of the same game. The 32-bit
control is the only comparable baseline, and it is what says which failures are ours.

```
i386 acceptance   145 object(s), 0 compile failure(s), 0 byte difference(s)
-ffloat-store     chain · boxes · ragdoll — ALL THREE BIT-IDENTICAL, 901 rows each
frame bounds      0 out-of-range reference(s)
lp64_pipeline.sh  -> PASS
wasm              SDLLaunch.wasm sha256 UNCHANGED (12d70e6c…), compared not argued
manifest          6167 entries, 0 missing, 0 size mismatch, 174 correctly excluded
```

Evidence: `../proven.txt` `LP64-VEHICLE-CLEAN`.

---

★★★ **NEWEST: 2026-08-31 (evening) — ARM64 PLAYS AN ONSLAUGHT MATCH, AND PLAYING IT FOUND EIGHT
MORE LP64 DEFECTS IN ONE SESSION.** The device loads `ONS-Dria`, starts the match, spawns vehicles
and bots, and survives 40 s → 80 s → **120 s** across the repairs below before dying in the next
one. It is not signal-free yet; the remaining chain is fully diagnosed and is item 1 of the block
above.

**THE INSTRUMENT CAME FIRST, AND IT IS THE REASON ANY OF THIS WAS FINDABLE.** `GameActivity` now
forwards a `com.ut2004.android.ARGS` string extra onto the engine's command line (engine-ut2004,
ufront 2.45), so one `adb` command starts a match:

```bash
adb shell setprop debug.ut2004.nofault 1
adb shell am start -n com.ut2004.android/.MainActivity --es com.ut2004.android.ARGS \
  'ONS-Dria?Game=Onslaught.ONSOnslaughtGame?bAutoNumBots=True?QuickStart=True?bPlayerMustBeReady=False'
```

The launcher's ten-second countdown does the rest. Before it the activity could only ever reach the
menu, so *"does arm64 PLAY?"* was not a question anyone could ask in a scripted run — which is
exactly why every crash below had gone unseen. ⚠ **The URL must be `argv[1]`.** `UGameEngine::Init`
takes the FIRST token as the travel URL and, since ufront 2.38, rejects a global option in that slot
and falls straight back to `DefaultLocalMap` **without looking at the next token** — so
`CPUSPEED=3000 ONS-Dria?…` reaches the main menu. Measured on the device before the order was
swapped.

**THE EIGHT DEFECTS.** Each one bought the next; none was reachable from the one before.

| | | |
|---|---|---|
| 1 | `McdGjkBinarySubset + 0x3c` | **the owner's reported ONS crash**, `fix_global_array_index.py` (NEW) |
| 2 | `IxBoxTriList`'s triangle walk | declined since LP64-TWO-COMPILERS; the fix is a TYPE |
| 3 | `qsort(partArray, n, 4, …)` | the ragdoll asset loader, ×4 in `MeFAsset.c` |
| 4 | an INLINE narrow load of a grown field | `fix_narrow_loads` rule E, 10 sites |
| 5 | `MEAPI` read as a return type | 8 accessors; `McdGeometryInstance::child` |
| 6 | a second `GetTypeId` read as a conflict | the entire ConvexMesh interaction family |
| 7 | the triangle `alloca` in `IxSphylPrimitives` | typed from the field it is stored into |
| 8 | `MePoolxInit(pool, mem, 0x18, n)` | `sizeof(MeDictNode)`, 24 here and **48** there |

…and a ninth that is **not a recovery defect at all**: the SDL thread's **1 MB default stack**.
Karma sizes several arrays on the stack from a run-time count — `MeDictNode nodemem[200]` in
`McdConvexMeshPlaneCut`, an `alloca` of `triangleMaxCount` `McdUserTriangle`s in every `Ix*TriList`
— and at LP64 **every one of them doubles**. ★ `SIGSEGV` with `code 2 (SEGV_ACCERR)` rather than
`SEGV_MAPERR` is what names it: a guard page is mapped `PROT_NONE`, so hitting it is a PERMISSION
fault, and that distinguishes stack exhaustion from a wild pointer. `SDLActivity` now asks for
16 MB. Confirmed by the crash CLASS changing, not by argument: `MeSetAdd`/ACCERR disappeared and the
device moved on to `McdSphylTriangleListIntersect`/MAPERR — the site x86-64 was already stuck at.

★★ **(1) IS THE ONE THE OWNER SAW, AND THE TOMBSTONE IS THE DEFECT DIGIT FOR DIGIT.**
`extern const int *McdGjkBinarySubset[16]` is an array of POINTERS, so `0x3c` is element 15 at i386
and **nothing at all** at LP64 — an eight-byte load at byte 60 splices the low half of `element[8]`
onto the high half of `element[7]`. `libUT2004.so` maps around `0x75_00000000`, so an entry reads
`0x00000075_0040546a`, and `fault addr 0x40546a00000075` is those two words exchanged. Both halves
are real addresses, so **nothing is truncated and `ptrwidth_check` cannot see this class at all.**

★ **AND MATHENGINE'S OWN 64-BIT BUILD CONFIRMS THE REPAIR RATHER THAN THE REPAIR BEING ARGUED.**
`metoolkit/lib.rel/win_amd64_single/McdConvex.lib` reads `mov rax, QWORD PTR [rip+0x78]` at **both**
references — `0x78` is `15 * 8` against our `0x3c` = `15 * 4` — in `McdGjkFaceQueueInit` and in
`McdGjkPenetrationDepth`, which are exactly the two sites the pass repairs. Two for two.

⚠ **THE SHAPE ALONE WOULD HAVE BEEN A DISASTER.** `MeProfile_linux.c` carries six sites of the same
spelling and **all six are correct**: `frameTime` is a struct of six `MeI32`s and `clockSpeed + 4`
is its own high word. The rule keys on the DECLARATION being an array of pointers, and reports the
six as out of scope. The third repaired occurrence is `CxSmallSort`'s vtable address point, which
`fix_vtable_offsets` cannot see because it is a macro over the vtable symbol rather than a local.

★★ **(2) `IxBoxTriList` — THE ANSWER WAS A TYPE, NOT A SPELLING.** Bisected site by site: 4 of the
7 rewrites are byte-identical under emcc and 3 are not, and the 3 are exactly those where the repair
turns an **integer** load into a **pointer** one. `p[1]` yields `undefined4` cast to `float *`;
`->vertices[1]` yields `MeVector3 *` directly; same address, same width, same value — and clang tees
one extra local three bytes away on a statement four lines off. Every member spelling tried failed
(`->vertices[1]`, `((MeVector3 **)p)[1]`, `*(MeVector3 **)&…`, `(kd_uptr)…->vertices[1]`).
`((kd_uptr *)p)[1]` is `unsigned int` at i386 **and** wasm32 — exactly what `undefined4` was — so
the conversion that follows is the one that was always there, and nothing downstream moves. **17 →
24 sites, 1 decline → 0.** The fallback is guarded by three measurements (offset `4k` here, `8k`
there, `sizeof` 4/8), so `McdUserTriangle::flags` correctly keeps the member spelling.

★★ **(3) IS WHY THE x86-64 SWEEP LOOKS WORSE THAN IT DID, AND THAT IS PROGRESS.**
`qsort(partArray, asset->partCount, 4, cmp)` sorts an array of POINTERS with the i386 element size,
so the comparator is handed spliced addresses — `fault addr 0x6390f87800000073` under `local_qsort`,
from `MeFAssetGetPartsSortedByName` ← `KInitSkeletonKarma` ← `AActor::setPhysics`. Four sites, all
on ragdoll creation. **Until this landed, no ragdoll was ever built**, so the whole
`USkeletalMesh::LineCheck` → `IxAggregateLineSegment` → `McdSphylTriangleListIntersect` chain was
unreachable and the recorded "7 gametypes clean" was measuring a game with no ragdolls in it.

⚠⚠ **AND "ONLY REWRITE WHAT MOVES" WAS BROKEN BY THIS PROJECT'S OWN `STT_FILE` TRAP.** With no type
to compare, the new test compiles the file at `-m64` before and after; written as `a_<fn>` and
`b_<fn>` the two objects differ **on the source name alone**, so it returned True for everything and
silently stopped testing. It passed `McdPolygonSort`, which sorts `MeVector3` — twelve bytes at
every pointer width and already correct. Two directories, one name.

★★ **(4) RULE E, AND THE HARNESS EARNED ITS KEEP AGAIN.** Rule C's "the destination is declared
pointer-width" was only ever a PROXY for the measurement; an inline load has no destination.
`IxAggregateLineSegment+304` on the device:

```
a0ab30: ldr w8, [x8, #0x20]     <- elementTable, at its CORRECT LP64 offset 32
a0ab3c: ldr x8, [x8, w9, sxtw]  <- FAULT, fault addr 0xffffffffd3b89558
```

The address is right and the width is four bytes of eight, sign-extended into the address the
tombstone reports. ⚠⚠ **The first version of the rule matched both sides of `=`** and turned
`*(undefined4 *)(…mdtbody[1]) = uVar1` into an EIGHT-byte store of a four-byte local, zeroing the
high half of a pointer that had been valid. **i386 stayed 145/145 and the three scenes stayed
BIT-IDENTICAL**; the LP64 harness went red with an ASan error in `MdtBodyGetCenterOfMassPosition`
and trajectories that stopped at row 52 and row 0. ★ **The LP64 harness is the only gate that sees
this — the third time this project has been told so.**

★★ **(5) AND (6) ARE THE SAME LESSON: A PATTERN THAT MATCHES THE WRONG THING REPORTS NOTHING.**

`fix_literal_offsets` read metoolkit's two-line declarations wrong:

```c
MEPUBLIC
McdGeometryInstanceID
                  MEAPI McdModelGetGeometryInstance(McdModelID cm);
```

The one-line pattern matched the SECOND line and recorded the return type as **`MEAPI`**, which
resolves to no struct — so the accessor read as "returns something untypeable" rather than as
unmatched, and nothing said so. Eight accessors. One of them is a ragdoll crash:
`IxAggregateLineSegment` reads `pvVar7 + 0x30`, which is `McdGeometryInstance::child` at i386 and
**72** at LP64, so the child instance came back NULL and the struct copy two lines later faulted on
address 0. Its sibling `+ 4` is `mTM`, 4 here and 8 there. Both confirmed against the amd64 build
(`mov 0x48(%rdi),%rbp`, `mov 0x8(%rcx),%rdx`).

`interaction_types` did the same to a whole family. `McdConvexMesh.c` exports **two** `GetTypeId`
accessors — `McdConvexMeshGetTypeId` and `McdConvexMeshMeshGetTypeId` — and both return 7. The
"a type id claimed by two tags is evidence about neither" rule read that as a conflict and **id 7
left the table entirely**, so `McdSphylConvexMeshIntersect` and seven siblings were never typed and
every geometry offset in `IxConvexPrimitives.c` stayed at its i386 value:

```c
ConvexHullNSegment((McdConvexHull *)((kd_iptr)pvVar4 + 0x10), pos, axis, …)
```

`McdConvexMesh::mHull` is at **16 here and 32 there**, so the hull pointer was sixteen bytes short
and `ConvexHullVoronoiRegion` faulted on the face table. ★ The tie-break is the **type database**:
a tag that names a struct the oracle declares is a type, one that does not is an accessor whose name
happens to end that way. 26 → 28 functions typed. ⚠ A latent crash came with it — `out[fn] = None`
is the dropped marker and a THIRD registration then indexed `None[0]`; nothing had ever been
registered three times until the ConvexMesh family started being typed.

★ **(7) AND (8) ARE BOTH "THE TYPE IS DECLARED, SO DO NOT INFER IT".** The triangle `alloca` in
`IxSphylPrimitives` is stored into `McdTriangleList::list`, declared `McdUserTriangle *`; the
`MePoolxInit` in `MeSet` is handed `MeDictNode *nodemem`. Both literals were the i386 `sizeof`.
⚠ (7) needs a **second, late invocation** of `fix_baked_sizeof` (`--field-allocas-only`), because in
the raw recovery the target reads `pMVar9[3].prev = …` and the field has no NAME until
`fix_literal_offsets` and `fix_index_layout` have run. ⚠ And (8)'s sibling call must be **left
alone**: `McdGjkPenetrationDepth` passes `0x2c`, and `McdGjkFace` is 44 bytes at both widths. Two
calls in the corpus, one moves and one does not — which is what the `-m64` measurement is for.

**THE TRUNCATIONS ARE CLASSIFIED, WHICH THEY NEVER WERE.** `tools/ptrwidth_classify.py` is new.
98 → **91**, of which **39** are in four named benign classes (`KD_FBITS` into a four-byte slot 10,
`MeDict`'s own `void *` key 15, a discarded `int` return 11, a count through an ID typedef 3) and
**52 are UNEXPLAINED and open**. ⚠ **`KD_FBITS` is not a class**: `IxBoxTriList` 279/280 use the
same macro to write **eight** bytes at byte 16 of a three-float vector.

```
i386 acceptance   145 object(s), 0 compile failure(s), 0 byte difference(s)
-ffloat-store     chain · boxes · ragdoll — ALL THREE BIT-IDENTICAL, 901 rows each
frame bounds      0 out-of-range reference(s)
wasm32            145 compiled, 0 failures, 145 exported symbol sets identical
lp64_pipeline.sh  -> PASS      run-standalone  12 passed, 0 failed
aarch64           98 -> 91 diagnostics; 39 classified benign, 52 open
arm64 device      ONS-Dria, Karma ON, match running: 40 s -> 80 s -> 120 s
x86-64 sweep      8 gametypes; see OPEN ITEMS 2 for why this number moved
```

✅ **THE WEB IS RE-STAMPED.** `UnUnix.cpp`'s `UT_NO_FAULT_HANDLER` block is in the wasm build and
takes the `getenv()` branch, so the engine artifact changed and had never been checked.
⚠ **AND THE KARMA HALF OF IT IS A MEASURED NO-OP, NOT AN ARGUED ONE**: the `.wasm` built from the
tree with defects (1)–(7) in it is **byte-identical** to the one built before them —
`sha256 12d70e6c…` both times — because every rewrite folds back to the constant it replaced at
`sizeof(void *) == 4`. That is the claim this project has made since 2026-08-30, compiled and
compared rather than reasoned about.

Evidence: `../proven.txt` `LP64-ARM64-PLAYS`.

---

★★★ **PREVIOUS: 2026-08-31 (arm64, closed) — ✅ ANDROID arm64 REACHES THE MAIN MENU WITH KARMA
ON.** `UT2k4MainMenu.Opened()`, 2043 log lines, **zero signals**, still running — against 2039
for the Karma-OFF control. The open item below is closed.

**Two repairs, and the second is the one that mattered.**

**(1) The `struct` keyword, narrowly.** `IxSphereTriList` writes
`((struct McdTriangleList *)0)->list`, one keyword away from the anchor — and simply allowing the
keyword had already been *measured* as unsafe (63 → 147 rewrites, `scene_ragdoll` nondeterministic
and differing from i386). It is now accepted only behind the same evidence as the direct spelling,
plus one more restriction:

⚠ **A gated spelling gets whole elements only.** "The remainder lands on a member start" is a
strong test for a 68-byte struct with two members and a **vacuous** one for an 8-byte struct with
two — every 4-aligned value passes. `MeAssetDBXMLIO` reaches `PElement::childHead → PElementNode`
(sizeof 8) carrying seventeen bare `8`s with nothing to do with any table. Result: **+2 sites**,
exactly the alloca and the cursor — and `MeAssetDBXMLIO` was declined by the byte-identity gate
on top of that.

**(2) ★★ A frame slot that is a SCALAR, not an array — and this is what was actually crashing.**
`fix_frame_slots` scales the fabricated argument areas, and it **declined these by its own rule**
("the name must appear nowhere except inside a frame-slot cast"):

```c
float fStackY_160;                                  /* four bytes  */
*(MeReal (**) [3])((kd_iptr)&fStackY_160) = edge;   /* eight bytes */
```

At LP64 that store runs **four bytes past a four-byte local**, into whatever the compiler put
next — which was the outgoing `McdUserTriangle ct`. The tombstone landed two calls later in
`McdVanillaOverlapSphereTri`, dereferencing an `inTri->vertices[0]` of `0x7200000072`.

⚠ **`check_frame_bounds` cannot see it** — it reads *array* bounds, and this slot is a scalar. It
reported 0 throughout.

The decline rule was right that widening changes what a bare read means; but if **every** other
use is the bare scalar, `var[0]` means exactly what `var` meant, so those are rewritten with it.
6 slots across 6 files.

```
Karma OFF   2039 lines, UT2k4MainMenu.Opened()
Karma ON    1037 -> 1147 -> 2043 lines, SAME MILESTONE, 0 signals, alive
i386 145/145 · three scenes BIT-IDENTICAL · 0 ASan · frame bounds 0 · wasm32 146/146
pipeline PASS · run-standalone 12/12 · gates 6/6
```

Evidence: `../proven.txt` `LP64-ARM64-MENU`.

---

★★★ **PREVIOUS: 2026-08-31 (arm64) — THE FIRST ANDROID KARMA BACKTRACE THIS PROJECT HAS EVER
HAD**, and the vehicle was not the x86-64 build in the end — it was a two-line switch.

`UT_NO_FAULT_HANDLER` (engine `b4cd9eb`; on Android `adb shell setprop debug.ut2004.nofault 1`)
leaves the six hardware-fault signals at their default disposition so `debuggerd` runs. Without
it the engine catches SIGSEGV, writes **one line** and exits — no tombstone, no backtrace. ⚠
Android has no way to set an env var for an app, which is why the variable that works everywhere
else is unreachable exactly where it was needed.

```
Fatal signal 11 (SIGSEGV), fault addr 0x7a   (SDLThread)
#00 McdCylinderTriangleListIntersect+1796
#01 KIntersect  #05 KHandleCollisions  #06 KUpdateContacts  #07 KTickLevelKarma
```

★ **And the static count had been pointing at it all along** — `ptrwidth_check.sh` listed
`IxCylinderTriList` second-worst (10 truncations). It is the ragdoll-vs-**world** path, which
`scene_ragdoll` cannot reach: that scene is capsules on a *plane*, with no triangle list in it.

**The defect:** `0x18` is `sizeof(McdUserTriangle)` at i386 and **48** at LP64, in *both* the
alloca that sizes the triangle array and the cursor that walks it — so the buffer is half what it
needs and every element after the first is read from the middle of the one before. Two new
shapes: a direct-member anchor (`triList->list`, not the offsetof spelling) and a cursor living
in a **union member** rather than a local.

⚠⚠ **THE ANCHOR HAD TO BE EVIDENCE-GATED, TWICE, BECAUSE BROADENING IT BREAKS LP64 WHILE i386
STAYS PERFECT:**

| broadening | rewrites | LP64 result |
|---|--:|---|
| accept `VAR->F` unconditionally | 56 → **262** | 4 ASan errors vs a clean i386 control; divergence at step 111 |
| also accept the `struct` keyword | 63 → **147** | `scene_ragdoll` **nondeterministic** and **differs from i386** |

**i386 was 145/145 byte-identical through both.** The direct spelling is now accepted only where
`fix_word_indexed_struct` has already typed an access in that file as `((E *)v)->` — which is why
that pass now runs **first**.

⚠ **So the `struct` variant is still open, and so is the crash it reaches.** `IxSphereTriList`
writes `((struct McdTriangleList *)0)->list` and keeps its three `0x18` sites; the device now
faults in `McdSphereTriangleListIntersect+2228`, fault addr `0x7200000072` — a spliced value, the
same class one file on. It needs a narrower key than "allow the keyword".

**arm64 progress, on a OnePlus 6 (arm64-v8a, Android 15):**

```
Karma OFF   2039 lines, reaches UT2k4MainMenu.Opened(), still running
Karma ON    1037 -> 1147 lines after this repair
```

Both from the same `-PwithKarma` build, verified by **symbol count in the arm64 .so** rather than
by the gradle log — it reports "up-to-date" far too readily.

```
i386 145/145 · three scenes BIT-IDENTICAL · wasm32 146/146 · lp64_pipeline.sh PASS
```

Evidence: `../proven.txt` `LP64-ANDROID-ARM64`.

---

★★★ **PREVIOUS: 2026-08-31 (final) — SEVEN GAMETYPES CLEAN, AND ⚠⚠ THE i386 GATE CANNOT SPEAK
FOR wasm32.**

**i386 is compiled by GCC and wasm32 by CLANG**, so "byte identical" has always been a statement
about *one* code generator — and nothing in this project said so until now. The triangle-list
repair below is byte-identical under gcc at `-m32` and **not** under emcc: same address, same
width, different instruction selection. The web artifact **ships**, so a repair only gcc
certifies is not certified. `fix_word_indexed_struct` now runs emcc over the source it replaces
and the candidate and requires those to match too. It declined `IxBoxTriList` and kept
`IxSphereTriList` — the *same* repair, and only one of them survives clang.

**The second anchor**, which is what needed the gate. `IxBoxTriList`/`IxSphereTriList` walk the
triangle list through an `undefined4 *`; `McdUserTriangle` is
`{MeVector3 *vertices[3]; MeVector3 *normal; ...}`, so those four words are four **pointers** —
byte 4 here and **eight** there. The type is declared: `McdUserTriangle *list;` in the oracle.
★ The cursor is right at both widths **by coincidence** (`&p[1].next` is 24 here and 48 there,
and so is `sizeof`), so nothing about the stride reveals it. ⚠ And the offline scenes cannot
reach this at all — `scene_ragdoll` is capsules on a *plane*; there is no triangle list in it.

⚠ **Three mistakes on the way, all the same family**, and each presented as `0 rewritten,
0 declined` — a clean zero that reads exactly like "there was nothing to do":

- `if not pools: continue` skipped the whole **file**, and the second anchor needs no pool;
- the site regex matched the **declaration** `undefined4 *puVar31;`, whose rewrite is a syntax
  error, which reads back as "not byte-identical" and declined the five correct sites with it;
- `flo` keys structs by **tag** (`_McdUserTriangle`) and the code names the **typedef**, so
  asking once returned an empty map and `vertices` came back as the array *name*.

⚠ **AS-Convoy is not ours, and that is measured now**: the 32-bit build fails **identically**,
same runaway loop in `Engine.HUD.GetFontSizeIndex`.

```
STRESS   16 runs · 8 maps × 2 · Difficulty 7 · 8–10 bots · 150 s each   ALL CLEAN
SWEEP    DM-Rankin DM-DE-Ironic CTF-FaceClassic BR-Anubis ONS-Torlan
         ONS-Primeval VCTF-BE-Dystopia                                  ALL OK
aarch64  truncations 134 -> 98; of the remainder 21 are KD_FBITS float
         bit-puns and 13 an MeI32 sortKey through a void * parameter
i386 145/145 · three scenes BIT-IDENTICAL · wasm32 146/146 · gates 6/6
```

Evidence: `../proven.txt` `LP64-TWO-COMPILERS`.

---

★★★ **PREVIOUS: 2026-08-31 (last) — SIX GAMETYPES CLEAN, AND ⚠⚠ A GATE THAT COULD NOT FAIL.**

**`lp64_pipeline.sh` reported PASS while a pass was crashing.** Every pass is piped into
`head`/`tail` to keep the log readable, and `|| exit 2` then tests the exit status of **`tail`**,
which is always 0. A Python `IndexError` went straight through it: the acceptance test ran on a
tree the pass had not finished editing and printed 145/145, three scenes bit-identical, PASS.
The only symptom was a summary line missing from the log. `set -o pipefail` is now the second
line of the script. ★ **The gate that cannot fail is the one to check first.**

**Rule D**, and it is why BR-Anubis now survives its own teardown.
`McdBatchContextDestroy` frees the last of every four contact pools through `(void *)*puVar1`,
where `puVar1` is an `undefined4 *` pointing **at** a `contacts` pointer. The other three in the
same loop use `*(void **)(...)` and are fine; this one read four bytes of an eight-byte pointer
and handed the half to `free()`. Rule A cannot see it — it dereferences a *variable*, not a
`*(T *)` cast — so the repair types the **pointer** instead of the load: `void **`, the same
four-byte access at i386.

⚠ **And a member name is not a type.** Four structs here declare a pointer called `contacts`, so
the first version matched on the name and correctly declined as ambiguous. `resolve_member`
follows the **expression**: `context` is declared `McdBatchContext *`, its `pools` is an
`McdBatchContactPool *`, and `[3]` of that is where `.contacts` lives. Every hop is read from the
oracle; an unresolvable hop declines the site.

```
DM-Rankin  DM-DE-Ironic  CTF-FaceClassic  BR-Anubis  ONS-Torlan  VCTF-BE-Dystopia   ALL OK
ONS-Primeval  still faults, now deeper — McdGjkFaceQueueInit
AS-Convoy     ENGINE-side, not Karma
```

Evidence: `../proven.txt` `LP64-PIPEFAIL`.

---

★★★ **PREVIOUS: 2026-08-31 (later) — FIVE GAMETYPES CLEAN. A MAP SWEEP IS A BETTER DETECTOR
THAN ANY SCENE.** Eight map/gametype combinations, 120 s each:

```
DM-Rankin   DM-DE-Ironic   CTF-FaceClassic   ONS-Torlan   ONS-Primeval    ALL OK
BR-Anubis   runs the full 120 s; SIGSEGV in the SHUTDOWN free path
AS-Convoy   ENGINE-side, not Karma — a runaway loop in Engine.HUD.GetFontSizeIndex
```

**Three more repairs.** Rule **G** — a *typed pointer* stepped by a whole element
(`piVar1 = piVar1 + 0x11`, 17 ints = 68 = the i386 element size); `fix_strides`' self-advancing
rule keys on `&p->field`, so a plain `p + K` is invisible to it.

⚠ **R0 was the one that mattered, and it was a single statement.** A variable stored *into* the
field is the table too. `McdAggregateCreate` keeps the fresh allocation in a bare `void *pvVar2`
and zeroes the **first slot** through it, before any load of the field exists to anchor on.
Every other slot was repaired; slot 0 kept a four-byte clear, so its high half held whatever the
previous tenant left.

★ **A WATCHPOINT NAMED THE TENANT.** `tri_push`, in the convex-hull builder, writing
`0x7fff00000000` into memory that later became the element table — exactly the value
`McdGeometryIncrementReferenceCount` then dereferenced on CTF-Face, BR-Anubis *and* AS-Convoy.
One unfixed statement out of fifty-six broke three gametypes. When a value is inexplicable, stop
reading and set a watchpoint.

**`fix_word_indexed_struct.py` is new.** `McdCache` comes out of a pool as a `void *`, Ghidra
types the local `undefined4 *`, and every field is addressed by word index. Its leading floats
keep their offsets at both widths; the two **pointers** at the end move 52→56 and 56→64 and are
eight bytes wide, so `puVar3[0xd] = ptr` wrote half a pointer. That is the Onslaught crash:
`McdGjkFatness (ins = 0xfffcf878)` — the low half of the stack address `0x7ffffffcf878`. The
type comes from the **pool**: `m_cachedData` is declared `void *` in the oracle and the rest is
a comment, so `init(pool, 100, sizeof(*(McdCache *)0), 16)` is the only place it is written down.

⚠ **Only rewrite what moves.** Re-spelling all twelve indices was *not* byte-identical — five
are `MeReal` fields, and `puVar3[9] = fVar5` through `undefined4` is an **integer** store where
`->fat1 = fVar5` is a float one. Measuring offset and width first leaves three sites, which are
the three the crash is about.

⚠ **And `flo.offsets_of` cannot see either pointer**: its member regex does not parse a
comma-separated declarator, and `McdCache` ends `McdGeometryInstanceID ins1, ins2;`. The type
database has them but without array expansion. Neither source alone resolves this struct.

```
i386 145/145 · three scenes BIT-IDENTICAL · wasm32 146/146 · pipeline PASS
run-standalone 12/12 · .gates 6/6
```

Evidence: `../proven.txt` `LP64-SWEEP`.

---

★★★ **PREVIOUS: 2026-08-31 — UT2004 RUNS AT 64-BIT.** A four-bot `DM-Rankin` and a **six-bot
`ONS-Torlan` with vehicles** each ran 200 seconds to the timeout with **zero signals**.
Onslaught is the strong test: its vehicles *are* Karma rigid bodies, and the stack reached
`KWorldStepSafeTime → MdtPackPartition → MdtBclAddAngular3` — the constraint solver stepping,
not initialising.

**Three more defects, and one of them was in a gate.**

**(6) The element SIZE, not the offsets.** `McdAggregateElement` is
`{MeMatrix4 mRelTM; McdGeometryID mGeometry;}` — 68 bytes here, 72 there, with `mGeometry` at
**64 in both**, because sixteen floats do not grow. Every offset check passes and every stride
is wrong. `fix_element_stride.py` is new.

⚠ **And it was not only `i * 0x44`.** `McdAggregateCreate` zeroes the table with gcc's
four-at-a-time unrolling and every offset in it is baked — `0x40, 0x84, 200, 0x10c`, step
`0x110`, which is `64 + k·68` and `4·68`. At LP64 the slots are at 64, 136, 208, 280, so the
loop was zeroing the middle of the matrices and leaving **every** `mGeometry` holding malloc
garbage. ★ **And the width was wrong too** — `undefined4` clears four bytes of an eight-byte
pointer. Fixing the stride alone still ships a dangling pointer, which is why the first attempt
*moved* the crash instead of removing it.

**(7) ⚠⚠ A GATE THAT COULD NOT EXPRESS THE ANSWER — the most interesting failure of the day.**
`flo.accept_edits` bisects a position-ordered list, so it can only ever accept a subset that is
contiguous in position. gcc common-subexpression-eliminates the two loads of
`g[1].mRefCtAndID` in `McdAggregateDestroy`:

```
first only   byte-identical = False
second only  byte-identical = False
both         byte-identical = True
```

Four `g[1].prev` edits sit between them, so **every subset the bisect tried contained one
without the other.** It declined all six, and the table pointer went on being read four bytes
wide. The tool had the right answer and no way to say it. `accept_by_field` makes the atom a
**field** rather than a position — sites naming the same field are exactly the ones gcc folds
together. 63 → 65 re-spelled.

**(8) Rule C, widened.** `MdtBcl3`'s solver holds an `MdtBody *` in `iVar15`, an `int` Ghidra
also uses as a row index three statements earlier, so `fix_narrow_pointers` will not widen it
and rule C's "destination is already pointer-width" test skipped it. `kd_iptr` **is** `int` at
32-bit pointer width, so promoting the declaration is a no-op on every shipping target by
construction. 2 → 7 sites.

⚠ **Signedness survives the promotion.** `uint → kd_iptr` is a *sign change*, not a widening,
and it cost `MdtBcl` its byte-identity on three declarations that had nothing else wrong with
them — the same lesson as the access type in `fix_element_stride`, learned twice in one session.

```
i386 acceptance   145 object(s), 0 compile failure(s), 0 byte difference(s)
-ffloat-store     all three scenes BIT-IDENTICAL
wasm32            146/146 byte-identical at the product's own flags
lp64_pipeline.sh  -> PASS   run-standalone 12/12   .gates 6/6
DM-Rankin  4 bots            200 s, 0 signals
ONS-Torlan 6 bots + vehicles 200 s, 0 signals
```

Evidence: `../proven.txt` `LP64-PLAYS`.

---

★★★ **PREVIOUS: 2026-08-31 (later) — FRAME (5) FIXED. A PASS THAT FIXED HALF A ROUND TRIP MADE
THE OTHER HALF INVISIBLE.** `fix_narrow_loads` rules A and B fire on clang's
`-Wint-to-pointer-cast`, so they need the loaded value to be cast to a **pointer**. Once
`fix_narrow_pointers` has widened the destination local to `kd_iptr`, the assignment is
integer-to-integer and **clang says nothing at all**:

```c
kd_iptr iVar8;                                    /* already widened */
iVar8 = *(int *)((kd_iptr)pvVar6 + KD_OFFSET(McdAggregate, elementTable));
```

The address is right and the local is wide; the **load** still takes four bytes of an eight-byte
pointer. Rule C is therefore **measured, not diagnosed**: it fires only when the address is an
offsetof naming a real field, the destination is declared pointer-width, and `sizeof(T::F)` is
**4 at i386 and 8 at LP64**. Two sites qualify, both `McdAggregate::elementTable`, both the
crash. The other 32 narrow loads into widened locals do not name a field and are left alone —
widening a load of a genuine `int` field would read four bytes past it, so the measurement is
the gate and not the shape.

```
i386 acceptance   145 object(s), 0 compile failure(s), 0 byte difference(s)
-ffloat-store     all three scenes STILL BIT-IDENTICAL
wasm32            146/146 byte-identical at the product's own flags
lp64_pipeline.sh  -> PASS   run-standalone 12/12
```

⚠ **FRAME (6) IS OPEN, FULLY DIAGNOSED, AND ITS REPAIR IS DERIVABLE — it is the next thing to
do.** `McdGeometryIncrementReferenceCount` received `g = 0x3f800000596203b0`: a **spliced**
pointer whose low half is a plausible address and whose high half is `0x3f800000`, which is
**1.0f**. A matrix diagonal is sitting in the top of a pointer.

```
McdAggregateElement { MeMatrix4 mRelTM; McdGeometryID mGeometry; }
    i386  sizeof 68 (0x44)   mGeometry at 64 (0x40)
    LP64  sizeof 72          mGeometry at 64
```

The **offset** `0x40` is right at both widths; the **stride** `i * 0x44` is not. There are 21
occurrences of `0x44` in `McdAggregate.c`, all of them this element stride. The repair is
declared rather than inferred — the oracle says `McdAggregateElement *elementTable;`, so any
stride on an address rooted at `KD_OFFSET(McdAggregate, elementTable)` **must** be
`sizeof(McdAggregateElement)`. `i * 0x44` → `i * (int)sizeof(McdAggregateElement)` folds to 68
at i386 and is byte-identical there by construction. That is **`fix_baked_sizeof`'s class**, not
`fix_narrow_loads`'.

Evidence: `../proven.txt` `LP64-NARROW-LOAD-C`.

---

★★★ **PREVIOUS: 2026-08-31 — FRAMES (3) AND (4) ARE FIXED, AND THE LEVEL COMES UP.** Boot now
reaches `Bringing Level NvidiaLogo.myLevel up for play` and the crash has moved into
`McdModelUpdate` — physics **running**, not initialising. Both frames came from one hole in
`fix_index_layout`: its `same` short-circuit compared only the **offset**.

⚠ **AND MY FIRST REPORT OF FRAME (3) WAS WRONG.** The entry below says `fix_index_layout`
"already reports this site by name, nine times". **It does not.** Those nine lines are its
*repairs* of `geom[1].next -> elementCountMax`; a grep for `declin` had matched the
declined-count **header** and then printed every `McdAggregate` line under it. The sites that
actually survived were the `.mRefCtAndID` ones, which the pass never mentioned at all — it
silently counted them as *already correct*. **Read a tool's output by section, not by grep.**

**AN INDEX CAN LAND ON THE RIGHT ADDRESS AND STILL BE A DEFECT.** `mRefCtAndID` is the first
member of `McdGeometry`, so `geom[1].mRefCtAndID` is the byte just past the base:

```
                       i386        LP64
geom[1].mRefCtAndID    16 + 0 = 16  32 + 0 = 32
McdAggregate::elementTable      16          32     <- the offset AGREES at both widths
sizeof mRefCtAndID               4           4
sizeof elementTable              4           8     <- the WIDTH does not
```

So the store kept the low half of a pointer, and `0x5961f910` showed up in the register file
where every live pointer was `0x5555_5xxxxxxx`.

⚠ **THE FIRST VERSION OF THE REPAIR WAS WORSE THAN THE DEFECT, AND EVERY GATE WAS GREEN.** A
field can resolve to a **struct** that begins at the same address — `McdConvexMesh::mHull` does
— and a struct has no access width, so `w` was its whole *size* and the access type fell
through to `MeU32`. It wrote four bytes of an eight-byte pointer *while widening the matching
read* to `kd_uptr`: one truncating store became a truncating store plus a widened read of it.
i386 was 145/145, all three scenes were bit-identical, `lp64_pipeline.sh` passed. **Only running
the game caught it** — the crash simply moved to `MeBoundingSphereCalc2` with
`points = 0x59620c50`.

`descend_to_scalar` is the honest version: it narrows a struct-typed field to the member that
*begins* at that address (`mHull` → `mHull.vertex`, width 8), and the caller **verifies the
offset did not move at either width** before using it. Store and read now both say `kd_uptr`.
Anything it cannot narrow is skipped, which leaves the site exactly as it was.

⚠ **A METHOD CORRECTION THAT CHANGED A RESULT.** The wasm32 A/B has to compile with the flags
the **product** uses — `-std=gnu99 -fno-strict-aliasing`, `metoolkit_decomp`'s `MD_C_FLAGS`.
Without them `McdAggregate.c` came back as one byte difference; with them it is identical. The
recovery reinterprets storage, so a strict-aliasing probe is not measuring what ships.

```
i386 acceptance   145 object(s), 0 compile failure(s), 0 byte difference(s)
-ffloat-store     chain · boxes · ragdoll — ALL THREE STILL BIT-IDENTICAL
wasm32            146/146 byte-identical, at the product's own flags
lp64_pipeline.sh  -> PASS   run-standalone 12/12   .gates 6/6
```

⚠ **FRAME (5) IS OPEN, and it belongs to a different pass.** `McdAggregateUpdateAABB` holds the
element table in an `int` local and reads it with `*(int *)` — the address is right, the **load**
is four bytes. That is `fix_narrow_loads` / `fix_narrow_pointers` territory, not this one.

Evidence: `../proven.txt` `LP64-INDEX-WIDTH`.

---

★★★ **PREVIOUS: 2026-08-30 (later still) — THE GAME RUNS AT 64-BIT FURTHER THAN IT EVER HAS,
AND THE OFFLINE SCENES WERE NEVER GOING TO GET IT THERE.** The entry below is still true —
all three scenes are byte-identical at both widths — and UT2004 still died at 64-bit before
its first frame. Three crashes, each found by RUNNING the game, two of them fixed here.

⚠ **FIRST, THE VEHICLE DID NOT DO WHAT IT SAID.** `-DUT_ALLOW_64BIT=ON` skips the `-m32`
demotion block, and then **four other places in `engine-ut2004` put `-m32` straight back**: a
bare `add_compile_options(-m32)`, the exe link options in `SDLLaunch` and `UCC`, and — the
worst of them — a `set(CMAKE_C_FLAGS "... -m32" CACHE STRING "" FORCE)` for gl4es that
rewrote the flags **for the whole project**, taking zlib, ogg, vorbis and Speex with it. The
configure printed `Pointer width: 8 bytes` while `file` said `ELF 32-bit LSB relocatable,
Intel i386`. All four are now gated on `UT_PTR_BYTES`, which is the variable that already
existed to be the width the build actually produces.

Two more, found on the way and both real:

| | |
|---|---|
| SDL2's i386 X11 forcing | a documented `-m32` cross-compile workaround, applied unconditionally. At 64-bit the probes link x86-64 objects against i386 libs, every check fails silently, `SDL_VIDEO_DRIVER_X11_SUPPORTS_GENERIC_EVENTS` goes undefined, and SDL emits a fallback `XGenericEventCookie` that collides with the real Xlib one — 6 TUs, 10 hard errors. It is the **mirror image** of the bug the demotion block's own comment warns about |
| `USE_PIXOMATIC=ON` | **impossible at 64-bit and now says so at configure time.** `pixomatic_linux.a` is a prebuilt i386 archive with no source — the same shape of problem Karma itself was. It failed at the last link step after a full 200-TU build |

**THE THREE FRAMES.** Each fix bought the next one; none was reachable from the one before.

```
(1) __strcmp_avx2  <- MeXMLElementProcess <- Handle_GeometryPrimitive_1_0
    strcmp against a NULL name: the handler table never held its sentinel   -> FIXED
(2) KaFileCreate_1_0, e = 0x58cabf90 beside parent = 0x555558ce4030
    every pointer here is 0x5555_5xxxxxxx, so that is a measured truncation -> FIXED
(3) McdAggregateCreate  *(undefined4 *)(pMVar1[1].mRefCtAndID + 0x40 + iVar5)
    ⚠ this line first said "NOT NEW — already reported by name" — IT WAS WRONG,
    see the newest entry. Fixed 2026-08-31 by LP64-INDEX-WIDTH.  -> FIXED
```

Boot moved from *XML parse* to *asset DB built* to **`UGameEngine::LoadMap` doing per-actor
collision**. The remaining defect is in map load, not in start-up.

**WHAT LANDED.**

| | |
|---|---|
| `tools/fix_block_copy.py` | NEW. A whole-struct copy Ghidra rendered as a word loop — `rep movsd` walked one field at a time with the cursor stepped by `&p->secondField`, 4 bytes here and 8 there. **15 repaired, 1 already correct, 0 declined** |
| `tools/fix_list_walk.py` | NEW. A linked list walked through `undefined4 *`, so the link is read at byte 4 when LP64 put it at 8. The node type is **declared, not inferred** — read out of the oracle header. **1 repaired, 2 declined by name** |

⚠ **`MdtBclContactParams` MATCHES THE PATTERN PERFECTLY AND MUST BE LEFT ALONE.** It holds no
pointers, so its size and step are the same at both widths and the loop is already right.
That one row is why the pass measures every site instead of trusting the shape.

★ **THE GUARD, AND WHY IT IS A PROOF RATHER THAN A CONCESSION.** `fix_block_copy` is the
first pass in this chain that does **not** re-spell arithmetic into one width-correct
expression. That was tried first and **measured**: the per-field expansion compiles to a
different i386 object (22800 → 22824 bytes), and no single spelling can work, because at LP64
the rodata template has **eight slots per handler** while the destination has **six words**.
So the i386 text is kept verbatim under `#if __SIZEOF_POINTER__ == 4`. `__SIZEOF_POINTER__`
is 4 on **wasm32** too, so the web artifact cannot have changed — and that was compiled and
compared, not argued: **146/146 byte-identical**. `fix_list_walk` needs no guard, and the
difference is the lesson: `((PElementNode *)p)->next` *is* `p[1]` at i386. Reach for a guard
only when a measurement says no single spelling exists.

```
i386 acceptance   145 object(s), 0 compile failure(s), 0 byte difference(s)
-ffloat-store     chain · boxes · ragdoll — ALL THREE STILL BIT-IDENTICAL, 901 rows each
wasm32            146 source(s), 0 compile failure(s), 0 byte difference(s)
lp64_pipeline.sh  -> PASS      run-standalone  12 passed, 0 failed
```

Evidence: `../proven.txt` `LP64-RUNS-THE-GAME`, `LP64-BLOCK-COPY`, `LP64-LIST-WALK`.

---

★★★ **PREVIOUS: 2026-08-30 (last) — ALL THREE SCENES ARE BYTE-IDENTICAL AT i386 AND LP64.**
`LP64-CONTACT-ORDER` closed the same day it was filed, and the gate's one-scene allowlist went
with it.

`scene_boxes_on_plane` matched for 94 steps and then diverged. The cause was `McdContactSimplify`
walking the contact array with a local Ghidra had put in the **wrong stack slot** — it merged an
`McdContact *` into an unrelated `McdContactLink` local and used its `next` field as the cursor:

```c
MStack_9c.next = (_McdContactLink *)(inContacts + cNum);
... *(float *)&((&(MStack_9c.next)->contact)[2]) ...
MStack_9c.next = (_McdContactLink *)((kd_iptr)MStack_9c.next + 0x28);
```

`0x28` is 40 is `sizeof(McdContact)` — **on i386**. At LP64 it is 48, because the contact's two
`element` unions hold pointers, so the cursor walked 40 bytes at a time over 48-byte elements. And
the index `[2]` steps in units of `sizeof(McdContact *)` — four bytes here, **eight** there — while
what it dereferences is a four-byte float.

★ **THE INSTRUMENT NAMED IT, NOT A READING.** It presented as two contacts with *equal separation*
arriving in the opposite order, and four calls later as a simplify returning 3 contacts where i386
returned 4. Dumping the IN and OUT arrays of every `McdContactSimplify` call at both widths
localised it in one run: the inputs were identical and only the output differed.

**The repair** is `fix_strides.py`'s third shape — the element type comes from what the cursor was
*assigned from* before it started stepping, and the literal must equal that type's i386 size or the
site is declined. Six other self-advancing cursors in the corpus are declined by that test,
correctly: `poly1 + 4` over an `MeVector3` is one float, not one element.

```
i386 acceptance   145 object(s), 0 compile failure(s), 0 byte difference(s)
-ffloat-store     chain · boxes · ragdoll — ALL THREE BYTE-IDENTICAL, 901 rows each
lp64_pipeline.sh  -> PASS, with no allowlist in it
```

⚠ **The allowlist is gone.** `lp64_run.sh` carried that scene by name for exactly one commit. An
exception that outlives its defect is a place for the next one to hide.

Evidence: `../proven.txt` `LP64-CONTACT-ORDER`.

---

★★★ **PREVIOUS: 2026-08-30 (later still) — THE LP64 PORT IS DONE, and "done" now means
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
