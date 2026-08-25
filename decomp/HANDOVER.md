# HANDOVER — Karma decompilation, for whoever picks this up next

## THE GOAL, and it is narrower than it looks

**We are shipping a DROP-IN REPLACEMENT for `metoolkit` — Karma — SO THAT UT2004 RUNS ON
WEB AND ANDROID.** That is the entire purpose. Read the next four sentences before any
other page of this file, because they decide what is worth doing.

- **The deliverable is a set of `.a` files the UT2004 engine links against instead of
  MathEngine's, built from source we own.** Nothing else is being delivered.
- **Nobody else will ever use this.** It does not have to be a good physics library, a
  general-purpose one, or a faithful reproduction of MathEngine's product. It has to make
  UT2004 behave.
- **Therefore: an object the engine never reaches is OUT OF SCOPE, not a to-do.** §3b
  retires 31 of them permanently and §3 shows twenty-three of thirty-seven collision pairs
  are never called. Recovering those would be zero progress.
- **And therefore the metric is §3c, not the object count.** `tools/dropin_gap.py` says how
  many SHIPPED members the engine still needs: **20 members, 148 symbols**. When that
  reaches zero and the gates below stay green, the job is done. "122 of 153 objects
  recovered" is not the number to quote and never was.

Branch: **`karma/decompile`**. `main` is untouched. **Do not merge.**

**If you want the non-technical picture — read [`STATUS-EXEC.md`](STATUS-EXEC.md).** It is
one page, written for someone who knows the product and not the internals. Keep it in step
with this file and `proven.txt`: if a number here moves, move it there. **The web/Android
integration agent has its own file, [`HANDOVER-WEB.md`](HANDOVER-WEB.md)** — that is the
half of the project this file does not cover.

You are resuming a project to recover Karma (MathEngine `metoolkit`) from shipped binaries
as portable C. This file is written for someone with no memory of how any of it came to be.

### Read this first — the thirty-second version

- Karma is UT2004's physics library, shipped binary-only. We are recovering it as portable
  C from the DWARF the shipped `.a` files carry. §1.
- **The collision layer drives a real match** — for the twelve pairs the census had seen.
  All twelve are recovered, measured against the shipped original on live inputs, and the
  engine has run on them with no shipped `.a` for those objects. §3, §7b, `proven.txt`.
- **A ZERO IS A SAMPLE SIZE, NOT A PROPERTY.** `IxCylinderTriList` was released on
  2026-08-24 with "0 structural in 71,417 real calls". On 2026-08-25 the same function on
  the same map, run longer, reads **37 `count_diff` in 153,391** — with a self-test over
  248,777 calls in the identical configuration coming back 100% bit-identical, so it is the
  code and not the harness. The divergence is off-by-one contacts on a cylinder RESTING on
  level geometry, which is the regime the game lives in. `proven.txt`, and §3's table now
  says so.
- **`IxCylinderCylinder` is still unreleased, and its charge has shrunk to one thing.** Its
  925/1,126 `dims_diff` is a field the engine does not read for this pair and the shipped
  library does not reproduce either (§11 item 0). What is left is `count_diff`, and no
  detector holds the object, so it is in the build. The quarantine only catches what a
  detector recognises. §3, `proven.txt`.
- **The cylinder map ask is answered and withdrawn** — not by finding a map to ask for, but
  by parsing the assets (`tools/find_cylinder_geom.py`): 114 cylinder collision elements
  ship, in stock maps. **There are no outstanding asks that need the project owner.** §3.
- **The solver's virtual calls now carry their arguments** — the blocker this file used to
  call "the whole job". All nineteen vtable dispatches in `keaRbdCore_unified` and
  `keaLCPSolver` are restored from relocation data, and it cost only 2 changed dumps out of
  153. §5a, §11 item 1.
- **`libMdtKea` IS RECOVERED. EVERY OBJECT IN IT REPRODUCES THE SHIPPED LIBRARY EXACTLY.**
  The driver (`keaRbdCore_unified`), the LCP (`keaLCPSolver`), `keaLCP_new`, the allocator
  (`keaMemory`), the three compute kernels and — as of 2026-08-25, fifth session —
  `keaIntegrate_pc` are all **bit-identical on all three scenes**. `keaLCP_new` and
  `keaIntegrate_pc` are also RELEASED, so the build is **115 objects**. §11 item 2.
- **AND THE ENGINE RUNS ON IT.** `build-subst115` links 115 recovered objects in place of
  the shipped members — 14 of `libMdtKea`'s 20 — and gdb breakpoint counts on
  `test-karma-1` read **`MdtKeaAddConstraintForces` 301 calls** and
  **`keaLCPSolver::solveLCP` 85 calls**. The recovered solver executes in the real engine on
  a real map, which nothing before this had shown. §7d.
- **`keaIntegrate_pc`'s defect was A ROUNDING GHIDRA DROPPED, and there was a second one no
  gate here can see.** The shipped code spills `dq[1..3]` to 32-bit slots and reloads them;
  Ghidra folds store and reload into one expression, so on i386 gcc keeps the chain at 80
  bits and the last bit drifts. And Ghidra prints a right-leaning float `+` chain FLAT, so C
  re-parses it left-leaning over the wrong operands — **exactly inert on x87 (0 in 2,000,000
  samples) and 31% divergent under storage precision, which is what wasm32, armv7 and arm64
  give.** §11 item 2a. Ground truth was not an inference: `KDynStep.cpp:618` is MathEngine's
  integrator copied verbatim into UT2004.
- **AND "the engine calls `MdtKeaIntegrateSystem` directly" — WHICH THIS FILE SAID — IS
  WRONG.** `KDynStep.cpp:869` branches on `bUseSafeTime`, which defaults to 1 and is only
  cleared by the console command `KSAFETIME 0`; the default branch uses the engine's OWN
  integrator. Measured both ways: 0 calls by default, 51 with `-EXEC` forcing the toggle.
  §7d, §3a.
- **`keaLCP_new`'s frame was materialised, and the reason this file said twice not to touch
  it was HALF WRONG.** The "61 sites with mixed byte and element strides" was a misreading:
  `auStack_3c[i - uVar6]` is a `uint *` indexed in elements, i.e.
  `(int)auStack_3c + 4*i - 4*uVar6` — region FOUR in byte terms, consistent with everything
  beside it. What was real is that `auStack_3c` is a TWELVE-BYTE local with six alloca'd
  regions below it. `materialise_shifted_frame` gives that area real storage, with every
  offset READ from `<object>.locals` or from a `stack0xHHHH` name. §11 item 2, `proven.txt`.
- **THE SOLVER DRIVER IS RECOVERED, AND IT IS BIT-IDENTICAL ON ALL THREE SCENES.**
  `keaRbdCore_unified` — `MdtKeaAddConstraintForces`, 900 calls per 900 steps, the object
  this file has called "the single object standing between the recovered kernels and a
  solver that runs" since it was written — reproduces the shipped library EXACTLY on
  `scene_chain`, `scene_ragdoll` AND `scene_boxes_on_plane`, with `scene_census` confirming
  it ran 900 times. Bit-identical on a CONTACT scene is not something else here has managed.
  The last step was a FLOAT PROMOTION that `proven.txt` predicted the same morning: Ghidra
  dispatches through `code *`, which is unprototyped, so a `float` argument goes as a
  DOUBLE. The control is not subtle — put those four dispatches back on `code *` and step 0
  of the collision-free scene reads **4.900e+14**. §11 item 2, `proven.txt`.
- **AND `keaLCPSolver` IS EXACT — the defect was three casts, and finding it needed a new
  instrument.** It was quarantined the same day on `first@1 = 3.772e+00`; `test/bisect_object.sh`
  put ONE recovered function in the link at a time and named `PrincipalSubmatrix`, where Ghidra
  had rendered three float stores as int CONVERSIONS — `(&this_00->AinvStride)[k] =
  (int)this->clampedValues[k]`, which destroys the value and compiles without a warning. The
  object is now bit-identical on all three scenes and on all fifteen of its functions
  individually. **115 objects.** §8, `proven.txt`.
- **BOTH CYLINDER DEFECTS NOW REPRODUCE OFFLINE.** `IxCylinderTriList`'s live divergence — the
  one WHAT REMAINS row 7 said the synthetic tier could not see — reproduces under `KD_CORNER=1`
  at iteration 23624 with **count 30/32**, where the default mesh reads 0.
  `IxCylinderCylinder`'s single `count_diff` is iteration 194376, **count 4/2**, and it is NOT
  the rounding instability §11 item 0 describes: that probe reports 0 `count_diff` for the
  shipped library against itself. `proven.txt`.
- **AND THE READING THAT NEEDED CORRECTING, kept because the lesson is general.**
  Its collision-scene divergence is not "at the noise floor": **max-delta-over-900-steps is
  the wrong statistic** once contact amplifies a last bit without bound, because then
  everything on the path reaches metres. The FIRST step it differs on discriminates, and
  `substitute_test.sh` now prints it — released objects read `first@0 = 1.043e-06`,
  `keaLCPSolver` reads **`first@1 = 3.772e+00`**: on step 1 of the ragdoll a body teleports
  3.7 metres. Nothing held it, so `recover.py` gains **`MEASURED_WRONG`**, a quarantine that
  holds an object on EVIDENCE rather than on a pattern — the hole §11 item 0 has described
  for several sessions. `proven.txt`.
- **AND THE VPTR STORE IS WRITTEN, AND `keaLCPSolver` AND `keaMemory` COMPILE.** §11 item 3
  was deferred because "no gate could see whether the rewrite was right or wrong". There is
  now a gate — `test/vptr_ab.sh`, the ninth — and it does not measure the trajectory, it
  measures the DISPATCH: devirtualise every repaired call site and diff. Bit-identical on
  all three scenes; a rotated slot and a +12 address point, built as controls, both segfault
  on the two scenes that reach the dispatch, and on the third — which does not reach it —
  **the gate says so rather than reading green**. §11 item 3, `proven.txt`.
- **`keaMemory` — the allocator — is bit-identical on all three scenes and the plausible
  wrong repair segfaults on all three.** That second clause is the measurement; the first
  alone would not be. The wrong repair is the one that supplies the cast Ghidra elided: it
  compiles, and it reads a `MeReal` out of the pool that the original never touches.
  §11 item 3a.
- **What is left of the solver is NOTHING — this row is discharged.** `keaIntegrate_pc` was
  `first@13 = 1.500e-08`; it is now bit-identical on all three scenes and over 450,000
  bodies of a per-function A/B on both code paths (`test/ab_integrate.sh`). Expression
  reassociation WAS one of the two defects and it was not the one that moved the number —
  see §11 item 2a. `keaLCP_new` is released too, and that took accounting for a **second**
  detector nobody had seen: `GHIDRA_STACK_GUESS` firing on the output of
  `materialise_shifted_frame`. §11 item 2, `proven.txt`.
- **The Ghidra-side attack on that is REFUTED, in three minutes, and it cost nothing.**
  Disabling DWARF variable import leaves the `in_stack_`/`stack0x` frame model
  **byte-identical**; all it removes is names. Ghidra's native stack recovery and its DWARF
  importer agree. Dead end 18, and §11 item 2 has what the experiment found instead — which
  is worth more than what it was looking for.
- **THE METRIC THAT MEASURES THE GOAL IS THE DROP-IN GAP, and it is new.** Object count is a
  bad progress metric and this file has said so for months without offering a better one.
  `tools/dropin_gap.py` walks the symbol closure from the ENGINE's own objects and reports
  every shipped member the engine still needs: **20 members, 148 symbols**, down from 27/192
  in one session. It is checked against a real link of the engine with every shipped member
  deleted — 111 undefined symbols, all 111 predicted. **122 objects compile.** §3c.
- **AND SIX MEMBERS CLOSED IN ONE RUN, off three generator fixes — plus FOUR objects that
  already compiled and were silently wrong.** `McdSphere` computed a sphere's mass
  properties from its radius read as an INTEGER; `McdCylinder`, `McdConvexMesh` and
  `McdPlaneIntersect` had the same defect. Ghidra addresses a derived geometry's fields as
  `g[1].<field-of-the-base>` — right byte offset, wrong type — and turns some of them into
  CONVERSIONS. Dead end 9 blamed the compiler; the compiler was right. §9 dead end 9.
- **`MeAssetDBXMLIO` PASSED EVERY GATE AND KILLED THE ENGINE.** Released on a correct
  argument, clean on all nine gates, and then a null in a relocated handler table took
  `KCreateAssetDB` down before a match started — §7c's defect in a second object. **For
  anything on the `.ka` path the offline gates are not evidence.** §3c, `proven.txt`.
- **`MdtWorld` is the first object out of §11 item 2, and the engine does not use it.**
  `collapse_outgoing_aggregate_copy` takes it FAIL → ok, bit-identical on all three scenes,
  with the gate proven sensitive to one part per million in the aggregate the repair passes
  — and then `KDynStep.cpp` turns out to reimplement the stepping loop and call the kea
  entry points directly, so `MdtWorldStep` is a third member of the §3a family. The method
  is validated; the object is not on the path. **`MdtKeaIntegrateSystem` turned out to be a
  FOURTH member of that family** — §7d. §11 item 2, `proven.txt`.
- **They compile for wasm32, armv7 AND arm64 — and arm64 is a lie.** Identical symbol sets
  on all three, but arm64 emits **2,436 pointer-truncation diagnostics across 69 of the 115
  objects** where armv7 emits **zero**, because the recovery puns pointers through 4-byte
  slots. There **is** now a gate for it — `test/ptrwidth_check.sh`, 13 seconds — and §6b's
  old "920 vs 23" figure was two different diagnostic sets added together. §6b.
- **`IxCylinderCylinder`'s 925 `dims_diff` is not a defect to fix.** Two independent
  reasons. The shipped library, run against ITSELF with one body moved a ten-millionth of a
  metre, changes its own answer **more often** than our recovery differs from it (7,798
  against 6,432 on the same 200,000 pairs), and no other pair in the driver does this at
  all. And **for this pair the field is never read**: the engine reads `dims` only when one
  model is the level TriangleList, and Karma's single branch on it is false for all three
  values the divergence involves. What is actually left is **3 `count_diff` in 24,111**.
  §11 item 0.

- **All 108 go into the engine at once and it plays a match**, indistinguishable from stock
  on the same map. That is up from the eight of §7b, and getting there took one real fix: a
  static pointer table whose relocations were never applied, which passed all seven gates
  and then segfaulted on the engine's first `.ka` file. §7c. **That figure is a past
  measurement at 108 and again at 110 (`proven.txt`, out11); the 112-object build has NOT
  been in the engine — the two new objects are `libMdtKea`, which the collision run does
  not reach anyway.**
- **One whole family the engine never asks Karma about**, and one that looked like a second.
  Every `Aggregate` *pair* is intercepted by UT2004's own dispatcher, as is
  `Box × TriangleList`. But the aggregate dispatcher **recurses into `KIntersect` per
  element**, so aggregate ELEMENTS reach Karma normally — which is exactly how the cylinder
  pairs above arrive. §3a.
- **The most important habit here is checking that a test is testing.** Most of the real
  bugs in this project were found that way, not by writing new code — and on 2026-08-24 it
  also caught a *search* that could not have found what it was looking for. §4a, §12's
  closing section, and dead ends 9 and 10.

### WHAT REMAINS — the whole list, ordered, 2026-08-25 (fifth session)

Everything else in this file is detail. This is the work.

| # | what | where | blocked on |
|---|---|---|---|
| 0 | **THE DROP-IN GAP: 20 shipped members, 148 symbols.** This is the goal and the metric. `tools/dropin_gap.py`, checked against a real link (111 of 111 symbols predicted). Group the 21 by CAUSE, not size: `extraout_*` is 7 members, a mislabelled `_X` is 3, one `stack0x` is 2, a guessed frame is 2, the profilers 2, four singletons. **Run the engine on anything on the `.ka` path** — `MeAssetDBXMLIO` passed all nine gates and killed init. | §3c | nothing |
| 1 | ~~**The solver.**~~ **DONE. `libMdtKea` is recovered whole and the engine has RUN on it.** Every object in it is bit-identical on all three scenes; `keaIntegrate_pc` and `keaLCP_new` are released, so the build is 115. `build-subst115` executes the recovered `MdtKeaAddConstraintForces` 301 times and `keaLCPSolver::solveLCP` 85 times on `test-karma-1`. | §11 item 2, §7d | — |
| 1a | **THE ASSOCIATION DEFECT IS A CORPUS-WIDE LEAD AND NOTHING HERE CAN SEE IT.** Ghidra prints right-leaning float `+` chains flat. On x87 that is EXACTLY inert (0 in 2,000,000 samples); under storage precision — wasm32, armv7, arm64 — it differs in **31%**. Three sites were found and fixed in one object by reading the disassembly. **Nobody has looked at the other 152.** Every gate in this project is structurally blind to it, so the first symptom will be wasm physics that drifts from native. | §11 item 2a | an instrument that does not exist yet — start from the 220-site scan |
| 2 | ~~**`IxCylinderCylinder` is wrong and is in the build.**~~ **RE-FRAMED — `dims` is not a measurement for this pair.** The shipped library disagrees with itself, under a 1e-7 m nudge, more often than we disagree with it. Leave it in the build; do not release it either. | §11 item 0 | a live match scoring ret/count/position/separation with dims read separately |
| 3 | **arm64 truncates pointers.** 2,680 diagnostics across 73 of 121 objects; armv7 zero. **There is now a gate** — `test/ptrwidth_check.sh`. | §6b | a generator-wide change to pointer-width slots |
| 4 | **Nothing has EXECUTED on wasm32, armv7 or arm64.** | `HANDOVER-WEB.md` | the web agent |
| 5 | ~~**GJK's warm cache path has never been tested.**~~ **DONE 2026-08-25** — `KD_WARM=<K>`, 0 ret / 0 count / 0 dims over 200,000 pairs, cache verified live. §11 item 4. The 3 `ret_diff` seen in a live match are still not reproduced, and this narrows where they can be. | §11 item 4 | — |
| 7 | **`IxCylinderTriList` diverges in a live match — and it now REPRODUCES offline.** 37 `count_diff` in 153,391 live; `KD_CORNER=1` gets it at iteration 23624 with **count 30/32** where the default 4×2 patch reads 0. So it is deterministic and debuggable without a match. Two hypotheses already refuted (the normal-accumulation alias; the three 'missing' statics, which GCC inlined). Next: the `footprint`→`verts` range that `McdVanillaOverlapCylTri` fills — two extra contacts means two extra points. | `proven.txt` | nothing |
| 8 | **`IxCylinderCylinder`'s remaining charge is `count_diff` only, and it is LOCATED.** Iteration 194376, **count 4/2** — we keep two contacts where the shipped library keeps four, and our contact [1] carries the shipped [0]'s separation, so the FOOTPRINT differs upstream of the emission filter. It is a real defect, not the rounding instability of §11 item 0: that probe reports **0 `count_diff`** for the shipped library against itself. | §11 item 0 | nothing |
| 9 | **`keaMatrix_PcSparse_vanilla` is FIVE OF SIX, and the sixth is located.** The last still-shipped `libMdtKea` member on the path. `solve` was fixed by the same dropped-rounding repair as `keaIntegrate_pc` (`MeReal tmp[4]`, accumulated through memory); four others were already exact. **What is left is `factorize`, whose arithmetic is not in `factorize`** — the shipped function has ZERO float instructions and calls the file-statics `vc_strip_Cholesky` / `vc_dstrip_Cholesky`, which gcc inlined in the recovery. Blanket-rounding their 62 float stores makes it WORSE (`first@10` → `first@7`), so read them statement by statement with `bisect_object.sh`'s `factorize` row as the gate. The other five members are `keaStuff`/`version` (no functions) and three debug/IO objects. | `proven.txt`, §11 item 2a | nothing |
| 6 | **The tail: 12 objects in the FAIL bucket and NOTHING in it blocks anything.** 3 DEAD (`MeASELoad` 126, `MeFGeometryFromMesh` 20, `McduDebugDraw` 1), 2 documented leave-alones (`McdSpace` 22, `MeSimpleFile_linux` 1), 3 dead end 9 (`McdTriangleList` 6, `McdBox` 2, `McdSphyl` 1), 2 low-value profilers (`MeProfile` 29, `MeProfile_linux` 13) and `MdtBcl`/`MeMath` at one `stack0x` each. **197 of the errors belong to objects nobody should touch.** Sort by §13's verdict column, never by the count. | §13 | do not start here |


### WHERE MY HEAD IS — the next moves, in order

Written 2026-08-25 (fifth session), and the framing changed this session. Until now this
list was "which object next". **It should be "which GAP MEMBER next", and they are not the
same list** — §3c. Seven members closed in one session and every one of them fell to a
GENERATOR fix that also repaired objects nobody was looking at.

**THE METHOD THAT HAS PRODUCED EVERY RECOVERY, and it is the only thing in this section
that matters if you read nothing else.** Group the failures by their normalised error
across ALL blocked objects, find the ONE upstream cause, fix the GENERATOR, then diff the
blast radius — every already-compiling object's `.o` must stay byte-identical, and when it
does not, **disassemble before assuming regression**. Four times now the changed objects
turned out to have been silently wrong for months. And when a rule looks right and does
nothing, **call it directly on the offending line**; that has now found twelve.

**MOVE 0 — work §3c's table, by CAUSE.** 20 members, 148 symbols. The families:

| family | members | what it needs |
|---|---:|---|
| `extraout_*` | 7 | NOT one cause. Some were gcc's variadic padding (fixed). `MeFAsset`'s sit on a reconstructed frame. Each needs its own diagnosis — see how `keaLCP_new` and `keaIntegrate_pc` were released |
| mislabelled `_X` | 3 | `MeFAssetPart` needs a CONTROL-FLOW repair (below); `McdGjkPenetrationDepth` and `ReadWriteKeaInputToFile` are `_DAT_*`/`_gDebug` data references |
| one `stack0x` | 2 | `MdtBcl` (15 symbols!) and `MeMath`, both diagnosed in §5c. `MdtBcl`'s is the function EPILOGUE rendered as a pointer assignment — a different shape from `MdtWorld`'s, so a fix for one will not show up on the other |
| guessed frame | 2 | `MdtPartition` is the object that detector exists for; `keaMatrix_PcSparse_vanilla` is MOVE 1 |
| profilers | 2 | `MeProfile` 4 symbols, `MeProfile_linux` 6. Low value per §3b and **they are in the gap anyway**, so they have to be done |
| singletons | 4 | `IxBoxTriList`, `McdContact`, `MdtLOD`, `MeAssetDBXMLOutput_1_0` — one symbol each |
| `McdSpace` | 1 | 16 symbols and the hardest: `struct _McdSpace`'s layout is in NOBODY's DWARF, including its own object (§13). Inferring it is the guess the detectors exist to stop. **Leave it last** |

**The two biggest are `MeFAsset` (38 symbols) and `MeFAssetPart` (21), and both are now
diagnosed rather than merely failing.** `MeFAssetPart`'s `_DAT_00000050` is not a name
problem: the shipped `MeFAssetPartEnableCollision` jumps to 0x775 when `asset == NULL`
(`712: test %ebx,%ebx; 714: je 775`) and Ghidra routed that edge into the function BODY,
so it renders `((MeFAsset *)0)->maxParts` as an absolute address. Fixing it means
correcting the control flow, not resolving a symbol. `MeFAsset` has `kd_argslot_*` — the
reconstructed-frame family — underneath its `extraout_*`.

**MOVE 1 — `keaMatrix_PcSparse_vanilla::factorize`, the last `libMdtKea` object.** Five of
six functions are exact; `solve` fell to the same dropped-rounding repair as
`keaIntegrate_pc`. `factorize`'s arithmetic is not in `factorize`: the shipped function is
0xcc bytes with ZERO float instructions and calls the file-statics `vc_strip_Cholesky` and
`vc_dstrip_Cholesky`, which gcc inlined in the recovery. Blanket-rounding their 62 float
stores makes it WORSE (`first@10` → `first@7`) — dead end 17 in miniature. Read them
statement by statement with `test/bisect_object.sh`'s `factorize` row as the gate, **and
check that script's TWO controls read identical first** (§4's instrument list).

**MOVE 2 — the association defect, §11 item 2a, and it is the largest unquantified risk in
the project.** Ghidra prints right-leaning float `+` chains flat. On i386 that is EXACTLY
inert — 0 differences in 2,000,000 samples — and under storage precision, which is what
wasm32, armv7 and arm64 all give, it differs in **31%**. Three sites were fixed in one
object of 153 by reading the disassembly; **nobody has looked at the other 152, and no gate
here can.** Do not ship a corpus-wide rewrite: reversing all 41 flat chains in
`keaMatrix_PcSparse_vanilla` changed the object file and changed nothing measurable, so on
i386 the rule is *unfalsifiable*. What is needed is an oracle that reads `fadd`/`faddp`
order out of the machine code — or a wasm-vs-native A/B, which is the web agent's side.

**MOVE 3 — the two cylinder defects.** Both reproduce offline; neither needs a match.
`IxCylinderTriList` under `KD_CORNER=1` at iteration 23624 (count 30/32);
`IxCylinderCylinder` at iteration 194376 (count 4/2). Details and refuted hypotheses in
`proven.txt`.

**MOVE 4 — arm64, and we have the toolchain.** The Android NDK is installed and
`test/ptrwidth_check.sh` is a 13-second gate. armv7 reads 0; arm64 reads **2,680
truncations across 73 of 122 objects**, because the recovery puns pointers through 4-byte
slots. **This is a real deliverable, not a footnote — Android is half the point of the
project.** The fix is generator-wide (widen the punned slots), not a flag.

### What NOT to do

- **Do not grind objects that are not in §3c's table.** They are out of scope. §3b.
- **Do not read a max-delta as a verdict on a contact scene** — read the first differing
  step. Believing the maximum cost this project a session.
- **Do not attempt `MeMath`'s `stack0x` blind** — §5c proves the target is never written.
- **Do not reach for `-ffloat-store`** — dead end 17 measured it five orders of magnitude
  worse.
- **Do not trust the offline gates for anything on the `.ka` path.** §3c's warning box:
  `MeAssetDBXMLIO` passed all nine and killed engine init. Run the engine.
- **Do not release an object because a detector's complaint looks inert.** That is exactly
  what happened to `MeAssetDBXMLIO`, and the complaint WAS inert — the object was broken
  for an unrelated reason the detector had been accidentally shielding.

### IS A COLLISION PAIR USED BY UT2004? ANSWERED, DEFINITIVELY — and here is the answer

This is the question that decides what is in scope, so the whole answer is in one place.
**37 pairs are registered. FIFTEEN are called. Twenty-two are not, and two of those
twenty-two can never be.** §3 has the table; §3a has the proof for the two families.

| | pairs | status |
|---|---:|---|
| **CALLED and recovered and validated** | 13 | evidence per object in `proven.txt` |
| **CALLED, recovered, measurably imperfect** | 2 | `IxCylinderTriList` (37 `count_diff` in 153,391 live), `IxCylinderCylinder` (1 `count_diff` in 24,111). Both reproduce offline; MOVE 3 |
| **NEVER CALLED — and CANNOT be, from the engine source** | 2 families | `Box × TriangleList` and every `Aggregate` PAIR. `KIntersect` (`KFarfield.cpp:936`) intercepts both before Karma is consulted. **No map can change this.** OUT OF SCOPE, permanently |
| **NEVER CALLED in 30+ runs across 20+ maps** | the rest | "no evidence of use", NOT "impossible". The list has moved **five times**. Re-run the census (§3, `KD_CENSUS=1`) on any new map before trusting a zero |

**The one trap, and it cost two sessions.** "The engine intercepts Aggregate, therefore
aggregate geometry never reaches Karma" is FALSE. `KAggregateGenericIntersect` recurses
into `KIntersect` once per ELEMENT (`KFarfield.cpp:878`), and `KUtils.cpp:762` wraps EVERY
primitive collision volume in an `McdAggregate` with no single-element shortcut. So the
aggregate PAIR never reaches Karma and every aggregate ELEMENT does, under its own type.
§3a. **Read the whole call graph, not just the dispatcher.**

**So: the collision half of the deliverable is DONE for thirteen of fifteen pairs, and the
two that are not are small, located and offline-reproducible.** No map question is
outstanding; the cylinder ask was closed by parsing the assets (§3).

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
compile:  122 objects in the build, and the MEASURED_WRONG quarantine (§8)
          is empty — read §4a before quoting any of the numbers below
scenes:   122/122 run clean on all three substitute scenes, EVERY ONE of them
          is individually bit-identical on the collision-free scene, and all
          122 TOGETHER are bit-identical on it — but read §4a before reading
          anything into that number
wasm32:   122/122 compile, exported symbol sets byte-identical to i386
armv7:    122/122 compile, symbol sets identical — a real 32-bit-pointer port,
          and 0 pointer-truncation diagnostics (test/ptrwidth_check.sh)
arm64:    122/122 compile, symbol sets identical, and NOT TRUSTED — 2,680
          pointer-truncation diagnostics across 73 of the 122 objects (§6b)
bindings: 122/122 export what the SHIPPED object exported, binding included (§8)
difftest: 14 pairs (Cylinder x Cylinder and Cylinder x TriangleList added
          2026-08-24), reproducing the documented baseline exactly — IxBoxBox
          1 count, IxSphereTriList 137 dims, IxCylinderCylinder 1 count + 20
          dims, quarantined IxBoxTriList as documented, the rest 0.
          KD_GRID=1 puts it in the game's AXIS-ALIGNED regime, where
          IxCylinderCylinder goes to 6,385 dims — and where the shipped
          library stops reproducing ITSELF (§11 item 0)
vptr:     the repaired vtable dispatches are bit-identical to a devirtualised
          control on all three scenes, and both wrong versions of the repair
          segfault where the dispatch runs (test/vptr_ab.sh, §11 item 3)
review:   18 objects recover.py labels "needs review" — and that label covers
          TWO different states, which matters when quoting it. 12 of them
          COMPILE and are held out of the build by the safety detectors (§8);
          the other 6 do NOT compile and their first error merely matches a
          detector's pattern. So: 135 of 153 rebuild, 122 are in the build.
fail:     8 more do not compile, for a total of 14 that do not
dropin:   THE METRIC THAT MEASURES THE GOAL — 20 shipped members / 148
          symbols the engine still needs (tools/dropin_gap.py, §3c). Was
          27 / 192 before this session's second half
solver:   libMdtKea IS RECOVERED. Every object in it is bit-identical on all
          three scenes, and the engine EXECUTES the recovered driver (301
          calls) and LCP (85 calls) on a real map (§7d). Six members are still
          shipped; only keaMatrix_PcSparse_vanilla is on the path (§4a)
BLIND SPOT: no gate here can see expression ASSOCIATION. It is exactly inert
          on x87 and 31% divergent under storage precision, i.e. on wasm32,
          armv7 and arm64 (§11 item 2a). Three sites fixed in one object of
          153; the rest are unexamined
dumps:    out13 is current (§5e) — out12's dumps with ONE changed, the virtual
          call through a PARAMETER's vptr. out12 = out11 with two changed.
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
| **Cylinder × TriangleList** | **160,030** | `McdCylinderTriangleListIntersect` | `IxCylinderTriList` | ⚠ released 2026-08-24 on **0 structural in 71,417 — and that figure does not replicate.** 37 `count_diff` in 153,391, self-test clean on 248,777. `proven.txt` |
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

## 3c. THE DROP-IN GAP — the metric that measures the goal

**The end state is a drop-in library: the engine linking against recovered
source and NO shipped `.a` member at all.** Object counts do not measure that.
"121 of 153 recovered" says nothing about whether the remaining 32 are ones the
engine reaches, and §3b is emphatic that most are not.

```bash
python3 tools/dropin_gap.py <engine-build-dir> /tmp/kd_build \
        /home/ion/tools/karma-lab/allobj --status <recover.py output>
```

It walks the symbol closure from the ENGINE's own object files, resolving each
symbol against the RECOVERED build first and only then against the shipped
archives, and reports every shipped member the walk still had to reach. **That
set is the work.** It shrinks when an object is recovered *and released*, and by
nothing at all when an object is recovered and quarantined — which is the
distinction an object count hides.

**It is checked against ground truth rather than asserted.** A real link of the
engine with every shipped member deleted reports 111 undefined symbols; the walk
predicts all 111. Pass `--verify-against <file>` to re-run that check. (gcc
prints demangled names and `nm` mangled ones, so the comparison demangles both
sides — without that three C++ symbols look like a hole in the walk when they
are a hole in the comparison.)

### Where it stands — 20 members, 148 symbols

| member | syms | why it is not in the build |
|---|---:|---|
| `MeFAsset` | 38 | `extraout_*`, and a reconstructed frame underneath it |
| `MeFAssetPart` | 21 | `_DAT_00000050` — a hoisted null-deref load |
| `McdSpace` | 16 | the `struct _McdSpace` layout is in nobody's DWARF (§13) |
| `MdtBcl` | 15 | one `stack0x`: the epilogue rendered as a pointer assignment |
| `MeXMLParser` | 12 | `extraout_*` |
| `MeMath` | 8 | one `stack0x`; §5c proves the target is never written |
| `MeProfile_linux` | 6 | four families at once — `rdtsc`, a type mismatch, three `_X` externals |
| `McdInteractions` | 5 | one argument-less indirect call |
| `MeAssetFactory` | 5 | `extraout_*` |
| `MdtPartition` | 4 | 30 guessed-frame sites — the object the detector exists for |
| `MeProfile` | 4 | the exported-DATA rename gap (dead end 10) |
| `MstUtils` · `ReadWriteKeaInputToFile` | 3 each | `extraout_*` · `_gDebug` |
| `MeAssetDBXMLInput_1_0` | 2 | `extraout_*` |
| six more | 1 each | `IxBoxTriList`, `McdContact`, `McdGjkPenetrationDepth`, `MdtLOD`, `MeAssetDBXMLOutput_1_0`, `keaMatrix_PcSparse_vanilla` |

**Group them by cause, not by size** — that is what has been working. The
families are: `extraout_*` (7 members), a mislabelled `_X` (3), one `stack0x`
(2), a guessed frame (2), the profilers (2), and four singletons.

> ### ⚠ AND THE GATES DO NOT COVER THIS. RUN THE ENGINE.
>
> `MeAssetDBXMLIO` was released on a correct argument, passed **every** offline
> gate — 121/121 on three scenes, frame bounds 0, bindings 0, difftest at the
> documented baseline — and killed the engine in `KCreateAssetDB` before a match
> started, calling through address 0. `proven.txt` has the backtrace and the
> defect (§7c's, in a second object: a relocated handler table emitted with a
> null where a sibling object's symbol belonged).
>
> **Anything on the `.ka` path has to be run in the engine before it counts.**
> The bisect is affordable — the crash is at init, so a 200 s run settles each
> cycle, and eleven candidates took four cycles. Use `-SOFTWARERENDERER`:
> `-GL4ESRENDERER` and `-OPENGLRENDERER` both fault at the first HUD frame in
> this environment, **for stock too** (§7d).

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
   └─► ghidra_clean.py  ──► <obj>.c          (pre-passes, then the repair loop §7a)
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
  --dump-dir /home/ion/tools/karma-lab/out13 \
  --obj-dir  /home/ion/tools/karma-lab/allobj \
  --out-dir  /tmp/kd_out \
  --build-dir /tmp/kd_build \
  --metoolkit ../Thirdparty/metoolkit \
  --protos /home/ion/tools/karma-lab/kd_protos10.h
```

**`kd_protos10.h`, not `kd_protos.h`** — it is `kd_protos.h` with the API-struct
prototypes appended (§5a) *and* every C++ function re-declared under its mangled
name (§5b). Using the plain one silently loses 412 call-site signatures and 197
C++ prototypes.

**`--build-dir` is not optional if you care about the object you already have.**
It defaults to `/tmp/kd_build`, so a second run comparing a new dump set will
overwrite the baseline you were about to compare against.

Recovered `.c` lands in `/tmp/kd_out/allobj/`, objects in `/tmp/kd_build/`. `recover.py`
prints a per-object table and a summary. **`out13` is the current dump directory** (§5e).
`out5`–`out12` are kept deliberately — they are the fallback if a pipeline change ever has
to be bisected against the dumps.

### Gate what came out — all NINE, every time

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

# the drop-in gap: WHICH SHIPPED MEMBERS DOES THE ENGINE STILL NEED? §3c.
# This is the metric that measures the goal; object counts do not. Needs an
# engine build only for its object files, so it costs seconds after the first.
python3 tools/dropin_gap.py ../build-native-karma /tmp/kd_build \
        /home/ion/tools/karma-lab/allobj --status <recover.py output>

# pointer width: the gate §6b used to say could not exist. 13 seconds, and it
# needs no arm64 hardware — truncation is a compile-time diagnostic.
# armv7 must read 0; arm64 currently reads 2,680 across 73 objects.
./test/ptrwidth_check.sh /tmp/kd_out/allobj /tmp/kd_build

# the ninth: does the repaired vtable dispatch reach the function it claims?
# Devirtualise it and diff, with a rotated slot and a +12 address point as
# controls that the scene MUST notice. §11 item 3.
./test/vptr_ab.sh /tmp/kd_out/allobj /tmp/kd_build $LIB keaLCPSolver \
    test/scene_chain.c test/scene_ragdoll.c test/scene_boxes_on_plane.c

# and the two that say what the scene gate above actually proved — §4a
# KD_CENSUS_VALIDATED is not optional: without it this sweeps in the quarantined
# objects too, and MdtPartition alone segfaults the scene before any census is
# written (§4a). A gate that cannot pass is not a gate.
KD_CENSUS_VALIDATED=/tmp/kd_build \
  ./test/scene_census.sh   /tmp/kd_out/allobj $LIB test/scene_chain.c
./test/gate_sensitivity.sh /tmp/kd_out/allobj $LIB test/scene_ragdoll.c

# per-function A/B for keaIntegrate_pc: the sharpest instrument here, because it
# names the FIELD that differs instead of the step. Both code paths — the scenes
# never set MdtKeaBodyFlagUseFastSpin, so the second argument is not optional.
# §11 item 2a.
./test/ab_integrate.sh /tmp/kd_out/allobj/keaIntegrate_pc.c 100000 0
./test/ab_integrate.sh /tmp/kd_out/allobj/keaIntegrate_pc.c 100000 1
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

### The pre-passes, and why three of them are not repair rules

`ghidra_clean.py` runs some repairs BEFORE the compile-feedback loop, on `body_text`, and
which side of that line a repair belongs on is a real decision rather than a style choice.
`REPAIR_RULES` picks **one** rule per diagnostic — `next((fn for pat, fn in REPAIR_RULES
if pat.search(msg)), None)` — so a second entry with the same pattern is dead code that
looks exactly like a rule that declines. The `undeclared` pattern is already owned by
`fix_mislabelled_external`, so anything else that answers it has to be a pre-pass.

| pre-pass | what it answers | driven by |
|---|---|---|
| `fix_external_base_arithmetic` | `&pool_ptr + i*4` — the addend collision, with byte scale | relocations |
| `collapse_outgoing_aggregate_copy` | by-value aggregate marshalling; §11 item 2's one success | the loop's trip count vs the prototype |
| **`fix_vptr_store`** | `_vanillaQMatrix = &__gxx_personality_v0;` — the vtable address point | relocations + the shipped disassembly + `<obj>.locals` |
| **`resolve_mangled_call_names`** | a call site Ghidra spelled `_ZN12keaLCPSolver8setUpperEPf` | the asm labels this unit is about to emit |
| **`fix_void_pointer_member`** | `pool_ptr->invI0` — a `void *` has no members | the DWARF + "this function has no FP instruction" |
| **`restore_float_text`** | a rounding Ghidra folded away, and a `+` chain it printed flat | `<obj>.locals` + `fst(p)s`/`flds` pairs in the shipped disassembly |
| **`fix_float_load_of_int`** | `(float)g[1].prev` — a four-byte LOAD, not a conversion | the DWARF typedef chain + "this function has no `fild`" |
| **`fix_printf_extra_args`** | `sprintf(b, "%s", x, extraout_EDX)` — gcc's padding word | the format string literal's own arity |
| **`fix_undeclared_underscore_local`** | `_iVar3` used where only `iVar3` is declared | the declared name having no other use in the function |
| **`initialise_unmodelled_locals`** | `unaff_ESI` read on a path that never assigns it | nothing — there was no information in the register to preserve |

The last EIGHT are new on 2026-08-25 and none of them needs a diagnostic: the evidence is
in the object file, so there is no reason to wait for GCC to complain.

**`restore_float_text` is the one to read before adding another.** Its defects produce no
diagnostic AND no gate failure on i386 (§11 item 2a), so a repair that silently declined
would be indistinguishable from success. It therefore **raises** rather than declining: if a
site's text stops matching exactly once, or the disassembly stops showing the spill the
repair restores, the run stops and says so. Four of the five mistakes §12 lists were rules
that silently declined; this is what that lesson looks like encoded.

**One thing the repair loop does that is worth knowing before you debug it.** `applied`
holds every edit TRIED, accepted or rejected, so a line that got a wrong candidate can be
offered a different one — and a rejection used to be permanent. A rewrite that is exactly
right but error-count-NEUTRAL (`_pool_ptr` → `pool_ptr` trades three diagnostics for three
of another kind) was refused on the round it was offered and could never be offered again,
and the loop reported a fixed point that was not one. It now forgets its rejections and
goes round once more, after real progress, up to `max_restarts` times. §11 item 3a.

**`substitute_test.sh` prints the FIRST DIFFERING STEP beside the maximum, and on a
contact scene that is the column to read.** A last-bit difference is amplified without
bound there, so the maximum stops discriminating — every object on the path reaches metres.
Released objects first differ at `1e-06` to `1e-08` and grow; `keaLCPSolver` first differs
at **3.772e+00**, which is not amplification. Believing the maximum cost this project a
session (§11 item 2, `proven.txt`).

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


**`difftest_pair.sh` has nine switches and the first is not optional:**

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
- **`KD_GRID=1`** — make the ROTATIONS axis-aligned: one of the 24 proper rotations whose
  rows are exactly 1/0/-1, with translations still random. **This is the game's regime for
  cylinders and it was missing for the project's whole life.** `IxCylinderCylinder` reads
  20 dims in 200,000 with random rotations and **6,385 in 52,925 touching** with this, a
  factor of 380 — which is why the synthetic tier said 0.01% while a live match said 3.8%.
  The wrong thing was the DISTRIBUTION, not a value. Self-test is 100% bit-identical in
  this regime, so the harness is sound in it.
- **`KD_JITTER=<eps>`** — displace model2 by `eps` BETWEEN the two calls. With
  `KD_SELFTEST=1` that runs the ORIGINAL against ITSELF on inputs differing by `eps`, and
  it answers a question no A/B can: **is the answer a stable property of the geometry, or
  is it decided by rounding?** `eps=0` and `eps=1e-12` are the controls and must read 0.
  §11 item 0 is what it found.
- **`KD_DIMSHIST=1`** — histogram of `(original dims -> recovered dims)` instead of four
  sample dumps, plus how many divergent pairs agree BITWISE on every separation. Four
  samples is enough to notice a divergence and not enough to localise one.


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

Takes 1–2 hours for the whole corpus (`out6` about 75 minutes for 153 objects; `out8` and
`out10` about the same). **A hypothesis does not need the whole corpus** — eight objects is
25 seconds, and §5's `KD_GHIDRA_OPTS` block has the harness. Dead end 18 was settled that
way in three minutes after two sessions of being budgeted at 75–120. Scripts live in
`tools/gscripts/` and **must be copied** to `/home/ion/tools/karma-lab/gscripts/` —
Ghidra reads them from there. **Write to a NEW output directory** and keep the old one
until the new dumps have passed all **nine** gates (§4 — the eighth is
`test/ptrwidth_check.sh` and the ninth `test/vptr_ab.sh`); a re-run changes every object at
once — `out6` differs from `out5` in 103 of 153 dumps.

`out5`–`out13` are all on disk. **`out13` is current** (§5e); it is `out12`'s dumps with
ONE changed, `out12` is `out11`'s with two, and `out11` is `out10`'s plus a `.locals` table.

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

## 5e. `out13` — the virtual call on a PARAMETER

`out13` is current. **It differs from `out12` in ONE of 153 dumps**, plus one `.locals` row,
and it is the last defect in `keaLCP_new`.

§5a resolves a virtual call by walking back from `call *N(%reg)` to the `mov` that loaded
the vptr **out of a frame slot**. `keaLCPSolver::solveLCP` opens with a call whose vptr
comes out of the object a PARAMETER points at:

```
mov (%edi),%esi          ; edi = A, a keaMatrix *
push %ecx                ; b
mov 0x2c(%edx),%eax
push %eax                ; this->x
push %edi                ; A
call *0x10(%esi)
```

There is no frame offset to look up, so the walk found nothing — and the whole loop is
gated on the function CONSTRUCTING a polymorphic local, which `solveLCP` does not do, so it
was skipped before the walk even ran. Ghidra emitted
`(**(code **)(*(int *)A + 0x10))();` with **all three arguments dropped**, and that alone
put step 0 of the collision-free scene 7.844e-03 out.

The new pass has no such gate. The parameter's class comes from the enclosing function's
DEMANGLED signature — `keaLCPSolver::solveLCP(keaMatrix*, …)`, so `ebp+0xc` is a
`keaMatrix *` — stepping only over four-byte parameters, because a by-value aggregate would
make the position a guess.

**`keaMatrix` is ABSTRACT**, and declares slot `+0x10` `__cxa_pure_virtual`, which carries
no signature. The Itanium ABI puts a derived class's override at the SAME slot, so
`_concrete_slot` follows through to the subclasses and **requires them to agree**:
`keaMatrix_pcSparse_vanilla::solve(float*, float const*)` and
`keaMatrix_tester::solve(float*, float const*)` do, and three arguments is what the three
pushes say.

One `.locals` row also went: Ghidra no longer needs `pMStack_44` to model that outgoing
argument, so the slot at −68 is now spelled `stack0xffffffbc` — which
`materialise_shifted_frame` reads from the name.

## 5d. `out12` — the virtual call gcc folded into an absolute address

`out12` is current. **It differs from `out11` in 2 of 153 dumps**, both of them the only
two objects in the corpus that import a vtable, and the `.locals` tables differ only in
ORDER — zero content changes, verified by comparing them sorted.

§5a's machinery finds a virtual call by walking back from `call *0xNN(%reg)` to the `mov`
that loaded the vptr. gcc does not always emit that shape: when it can prove the dynamic
type it folds the load away and the call becomes an ABSOLUTE indirect through the vtable,
with the relocation on the displacement.

```
push %ebx                    ; the size
lea  -0x148(%ebp),%edx       ; &vanillaAMatrix
push %edx                    ; this
movl $0x8,-0x148(%ebp)       R_386_32 _ZTV26keaMatrix_pcSparse_vanilla
call *0x8                    R_386_32 _ZTV26keaMatrix_pcSparse_vanilla
```

There is no register to walk back from, so the site fell through to the API-struct path,
found no struct member at that offset and was skipped — and Ghidra emitted
`vanillaAMatrix.allocate()` with **both arguments dropped**. That was the last error in
`keaRbdCore_unified`.

Nothing is inferred: the relocation names the class and the displacement IS the offset from
the vtable's start, so the slot is `disp - 8` with 8 the Itanium address point. A
displacement below it would be the offset-to-top or the typeinfo pointer, which is a defect
rather than a slot, and is refused. `_vtable_class` inverts `vtable_slots.mangle_class` and
CHECKS the length prefix rather than trusting it.

Regenerate with §5a's recipe, substituting `kd_vtable_callsites12.txt`. The API prototypes
the tool also emits are unchanged, so `kd_protos10.h` stands.

## 5c. `out11` — Ghidra's frame assignments, emitted beside each dump

`out11` is current. Its 153 `.c` files are **byte-identical to `out10`** — the change is
purely additive output — and it adds `<object>.locals`: one row per decompiled local that
has stack storage, with the offset Ghidra assigned it, read out of
`HighFunction.getLocalSymbolMap()`.

**Why that is a different kind of fact from the name.** `fix_stack_address_name` resolves
`&stack0xffffff6c` to the local `in_stack_ffffff6c` on the grounds that both names encode
the same frame offset. That was an assumption about how Ghidra builds names. Over every
`stack0x` site in the corpus the table says:

```
15 sites resolve to a local at exactly that offset
48 have no covering local
 0 resolve to a local at a DIFFERENT offset
```

So the convention holds without exception, and the rule is right for the reason it claimed.
It now reads the size from the table too, and **refuses if the name and the storage
disagree** — checked with a positive control: doctoring one offset by a single byte makes
`keaIntegrate_pc` drop from `review` back to `FAIL`.

**`tools/frame_offsets.py` is what to run first on any new `stack0x`.** It separates three
things that need different answers, and the surviving sites are one of each:

| kind | example | what it means |
|---|---|---|
| SIZE | `MdtWorld` ffffff6c: `MeReal` (4 B) against a 76-byte copy | Ghidra split ONE outgoing argument block into eight locals — `in_stack_ffffff6c`, `in_stack_ffffff70[48]` and six scalars it also uses as ordinary variables. Same in `keaRbdCore_unified` (72 declared, 92 written) |
| NO LOCAL, resolvable | `MdtBcl` at −12 | the accesses land inside `MeReal ref2world[4][4]` at −156, which the line above fills via `MeMatrix4MultiplyMatrix`. Repairable in principle |
| NO LOCAL, not resolvable | `MeMath` at −12 | the mapping IS exact — the nine accesses are exactly `MeReal eR[3][3]` at −60 — and `eR` is **declared and never written**, because Ghidra emitted `fcos`/`fsin` and discarded the results. Repairing the name would buy a compile at the price of reading uninitialised memory |

That last row is the useful one: a refusal that used to be a judgement call is now evidenced.

**What the table did NOT do is unblock anything**, and that is worth saying plainly. Only
six objects still carry `stack0x` after repair — `keaLCP_new` 24, `MeFGeometryFromMesh` 24
(DEAD, §13), `keaRbdCore_unified` 3, `MdtWorld` 4, `MdtBcl` 1, `MeMath` 1 — and every one is
a size mismatch or a missing local, not a naming problem. The remaining work on those is
frame RECONSTRUCTION, which is what the guessed-stack-frame detector exists to quarantine.

**`MdtBcl` is the one that could be done and is not**, recorded so the next person can
weigh it rather than rediscover it: the base points 144 bytes into a 64-byte array, so
rewriting the base is what `check_frame_bounds.py` exists to reject, and rewriting each
access instead needs the loop trip count. One object, in the tail.

## 5b. `out10` — the C++ prototypes that were never being applied

`out10` is current. **4 of 153 dumps differ from `out9`** — `keaRbdCore_unified`,
`keaLCPSolver`, `keaLCP_new` and `McdSpace`, none of which compiles — and all 109
compiled objects' `.o` files are byte-identical. All eight gates read exactly as before.

**What it fixes.** `ParseKarmaHeaders` matches a prototype to an ELF symbol **by name**,
and it runs before Ghidra's demangler analyzer, so the symbol is still
`_ZN12keaFunctions20checkPrintDebugInputE...` while `kd_protos.h`, built from
`DW_AT_name`, says `checkPrintDebugInput`. **They never matched.** 75 undefined mangled
symbols across 21 objects had no prototype and were arity-guessed from the call site —
precisely the failure §5 says that header exists to prevent, sitting in plain sight since
the header was written.

**And the damage was not cosmetic.** In `MdtKeaAddConstraintForces` Ghidra laid out the
outgoing arguments for `keaFunctions::checkPrintDebugInput` with `this` DROPPED:

```
        machine code                    Ghidra's model
this      esp+0    (mov %eax,(%esp))     — not modelled —
constraints esp+4  (lea 0x4(%esp),%edi)  esp+0     <- 92 bytes, one word early
parameters  esp+0x60 (lea 0x60(%esp))    esp+0x5c  <- 76 bytes, one word early
```

which is why `MVar1.num_partitions` — field 0 of `MdtKeaConstraints` — was being assigned
`&vanillaFunctions`, the `this` pointer. Same shape as the `__regparm` shift of §10: the
convention decides the layout, and the body is then wrong by a fixed offset.

**The fix needs nothing on the Ghidra side.** `gen_protos.py` re-emits every prototype
under its `DW_AT_MIPS_linkage_name` as well. A mangled name is a valid C identifier and is
unique by construction, so the existing exact-name lookup simply starts hitting — no
demangling, no name matching, no ambiguity guard. 197 prototypes added, 0 removed or
changed, and Ghidra parses 2,700 FunctionDefinitions instead of 2,503.

Regenerate with the §5a recipe, substituting `kd_protos10.h`:

```bash
python3 tools/gen_protos.py /tmp/kd_allmembers -o /home/ion/tools/karma-lab/kd_protos10_base.h
cat kd_protos10_base.h /tmp/kd_api_protos10.h > kd_protos10.h    # BOTH halves, §5a
```

**Covering C++ FREE functions too is tried and rejected — dead end 19.**

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

### `KD_GHIDRA_OPTS` — changing an analyzer option

`ParseKarmaHeaders` is a **preScript**, and headless order is import → preScript →
analysis → postScript, so it is the one place an analyzer option can still be changed.

```bash
KD_GHIDRA_OPTS='DWARF.Import Local Variable Info=false;DWARF.Ignore Parameter Storage Info=true'
```

`;`-separated `Analyzer.Option Name=value`. The names are the exact strings the analyzer
registers — see `DWARFImportOptions.java` inside
`Ghidra/Features/Base/lib/Base-src.zip`. **An unknown name is reported loudly and not
applied**, because "the option did nothing" and "the option was never applied" are
otherwise the same output, and that is how a refutation becomes a false one. Unset changes
nothing, which is what produced `out9` and `out10`.

**Check the harness before believing the experiment.** Re-dumping eight objects with no
options set must reproduce the current dump directory byte-for-byte. It does; that is what
makes dead end 18 a result rather than a guess.

### Settled — do not re-test

- Only the default `decompile` simplification style produces C. `normalize`, `firstpass`,
  `register`, `paramid` all fail outright.
- The "Decompiler Parameter ID" analyzer does **not** fix call arity.
- **Disabling the DWARF importer's variables or parameters does not touch the frame
  model.** Dead end 18. All three of `Import Local Variable Info=false`,
  `Ignore Parameter Storage Info=true` and `Import Functions=false` were measured.
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

> ### ⚠ READ THIS FIRST — THE RENDERER CHANGED UNDER US, 2026-08-25
>
> `test/run_map.sh` passes **`-GL4ESRENDERER`**, and in this environment that now faults at
> the FIRST HUD frame — `REALLOC FAILED … NewSize=1351188168` in
> `UCanvas::DrawTileStretched`, from a viewport the renderer creates at **2×1**.
> `-OPENGLRENDERER` does the same. **The STOCK, unmodified engine does it too**: same map,
> same URL, logs identical modulo timestamps and addresses, 523 lines each. So it is an
> environment regression, not a recovery defect — but a run that dies at frame one is not a
> physics test.
>
> **`-SOFTWARERENDERER` (Pixomatic) survives**, and is what every engine measurement since
> has used. Four 340 s runs — stock and substituted, both integrator branches — all ran to
> the timeout with no SIGSEGV and no NaN. Either edit `run_map.sh` or invoke the binary
> directly:
>
> ```bash
> cd /tmp/kd_runtime/System
> URL='?Name=Player1?Class=Engine.Pawn?Character=Jakob?team=0?NumBots=4?MinPlayers=5?bAutoNumBots=False?QuickStart=True?bPlayerMustBeReady=False'
> timeout 260 xvfb-run -a -s "-screen 0 640x480x24" ./ut2004-x.bin \
>     "test-karma-1?game=XGame.xDeathMatch?TimeLimit=0$URL" -SOFTWARERENDERER -nohomedir
> ```
>
> **And always run a STOCK control on the same map before reading a crash as yours.** That
> is the only reason this was diagnosed in minutes rather than as a regression.

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

| target | compiles | exported symbols vs i386 | pointer TRUNCATION diagnostics |
|---|---|---|---:|
| wasm32 | 122/122 | identical | — |
| **armv7** | **122/122** | **identical** | **0** across 0 objects |
| **arm64** | **122/122** | **identical** | **2,680** across **73 of 122** objects |

Measured over **all 122 objects in the build** with `test/ptrwidth_check.sh`, which enables
exactly three clang diagnostics on top of `-Wno-everything` so the count is those three and
nothing else:

```
-Wint-to-pointer-cast        cast to 'T *' from a smaller integer type
-Wpointer-to-int-cast        cast to a smaller integer type from 'T *'
-Wvoid-pointer-to-int-cast   ... from 'void *'
```

> **CORRECTION, fifth session.** This table used to read **920 for arm64 against 23 for
> armv7**, and those were two different diagnostic SETS added together. Re-measured on the
> same five objects the original figure came from:
>
> | | total warnings | `-Wint-conversion` + `-Wincompatible-pointer-types` | truncation |
> |---|---:|---:|---:|
> | armv7 | 68 | 23 | **0** |
> | arm64 | 968 | 23 | **897** |
>
> 920 = 897 + 23. The **conclusion is unchanged and is now stronger**: the 23 are shared by
> both targets and are the ones §4 downgrades deliberately, while the truncation class is
> arm64-only and armv7's zero is what makes the arm64 number mean anything — same source,
> same compiler, 32-bit pointers, clean.
>
> **And "no gate can see it" is no longer true.** `test/ptrwidth_check.sh` takes 13 seconds
> and needs no arm64 hardware, because truncation is a compile-time fact. It also names the
> 65 affected objects; the other 44 are clean. Worst offenders: `IxSphylPrimitives` 600,
> `IxConvexTriList` 203, `MdtMainLoop` 139, `McdAggregate` 124, `McdBatch` 112.
>
> One trap in measuring this, and it reads exactly like good news: `-w` followed by
> `-Wint-to-pointer-cast` reports **zero on both targets** — clang's `-w` wins over a later
> `-W` for these. Use `-Wno-everything`, which is designed to be overridden.

**Why arm64 must not be trusted despite passing everything.** §4 downgrades those two
diagnostics deliberately, and says why: they are "safe only because every target is
32-bit-pointer (i386 and wasm32)". Decompiled code puns pointers through integer slots
constantly — Ghidra recovers a stack slot as `undefined4` and the code stores a pointer in
it. On a 32-bit target that is lossless. **On arm64 it truncates a 64-bit pointer to 32
bits**, and the 40× jump in diagnostics is exactly that happening, 897 more times.

It compiles. The symbol sets are byte-identical. Every one of the seven behavioural gates
passes, because not one of them executes arm64 code — which is why the eighth gate had to
be a *textual* one. This was §12's closing checklist in its purest form (*the measurement
cannot see the failure*) and the answer was not "execute on arm64", it was "ask the
compiler a question it already knows the answer to".

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

## 7c. All 109 objects at once — and the defect that only this found

**2026-08-24 (third session).** §7b substituted the eight objects behind the called
collision pairs. This substitutes **everything that compiles** — 108 at the time, 109 after
`IxCylinderTriList` was released the same day — which is a much larger claim, and it took
one real fix to get there. Re-link before quoting it; the binary measured below is the
108-object one.

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

## 7d. The engine ON the recovered SOLVER — and a reachability correction

**2026-08-25 (fifth session).** §7c ran the engine on 108 recovered collision objects. This
runs it on **115, including fourteen of `libMdtKea`'s twenty members** — the driver, the LCP,
`keaLCP_new`, the allocator, the three kernels and `keaIntegrate_pc`.

```bash
./test/make_substituted_metoolkit.sh /tmp/kd_build ../Thirdparty/metoolkit /tmp/mt_subst115
cmake --preset native-karma -B build-subst115 -DMETOOLKIT_DIR=/tmp/mt_subst115
cmake --build build-subst115 -j"$(nproc)"
```

**Substitution verified at the machine-code level, not from the link** — the §7c discipline:

| symbol | stock | subst115 |
|---|---:|---:|
| `MdtKeaIntegrateSystem` | 23 | **40** instructions |
| `KeaIntegrateSystem_vanilla` | 434 | **415** |
| `keaLCPSolver::solveLCP` | 314 | **499** |
| `MdtKeaAddConstraintForces` | 471 | **405** |

**And it executes.** gdb breakpoint counts on `test-karma-1` under `xDeathMatch`:

| symbol | calls |
|---|---:|
| `MdtKeaAddConstraintForces` | **301** |
| `keaLCPSolver::solveLCP` | **85** |
| `MdtKeaIntegrateSystem` | **0** |

The recovered solver runs in the real engine on a real map. Nothing before this had shown
that: §7c's binary carried recovered solver *arithmetic* but the kea entry points were
still shipped.

> ### ⚠ AND THE LAST ROW IS A CORRECTION THIS FILE OWED
>
> **"The engine calls `MdtKeaIntegrateSystem` directly" — which WHERE MY HEAD IS said, and
> which made `keaIntegrate_pc` the designated blocker — is wrong in normal play.**
> `KDynStep.cpp:869` branches on `KGData->bUseSafeTime`. `KarmaSupport.cpp:1721` initialises
> it to **1**, and only the console command `KSAFETIME 0` (`KUtils.cpp:610`) clears it. The
> safe-time branch integrates with the engine's OWN `KIntegrateSystem` (`KDynStep.cpp:618`);
> Kea's integrator is the `else`.
>
> Measured both ways rather than read off the source alone:
>
> ```
> default                                    MdtKeaIntegrateSystem   0 calls
> -EXEC=<file containing `KSAFETIME 0`>      MdtKeaIntegrateSystem  51 calls
>                                            ("Karma Safe-Time Disabled." in the log)
> ```
>
> So `keaIntegrate_pc` is a **fourth member of the §3a family** — code UT2004 links and
> reimplements — alongside `MdtWorldStep` and `KIntersect`'s two intercepts. It still has to
> be correct: it is in the link and the toggle is a shipped feature. But it was never the
> blocker, and **grepping for a call is not enough — read the branch around it.**
>
> The same reading explains where §11 item 2a's ground truth came from. Epic needed a
> variable-timestep integrator, copied MathEngine's into `KDynStep.cpp` verbatim, and left
> the original reachable behind a toggle. That copy is the only place in this project where
> Karma's ORIGINAL SOURCE is available, and it settled an association question no gate here
> could.

**Four 340 s runs, no crash on either side.** `test-karma-1`, `xDeathMatch`,
`-SOFTWARERENDERER`:

| build | branch | exit | SIGSEGV | START MATCH | NaN / Critical |
|---|---|---:|---:|---:|---:|
| `stock-karma` | default | 124 | 0 | 1 | 0 |
| `stock-karma` | `KSAFETIME 0` | 124 | 0 | 1 | 0 |
| **`subst115`** | default | 124 | 0 | 1 | 0 |
| **`subst115`** | `KSAFETIME 0` | 124 | 0 | 1 | 0 |

`exit=124` is the timeout, i.e. all four ran to the end. So the engine on 115 recovered
objects — the whole recovered solver, and `keaIntegrate_pc` on the branch that reaches it —
is as stable as stock over 340 s of live physics. **It is not a trajectory comparison**:
neither binary carries shadow instrumentation, so this says "runs like stock", not "computes
what stock computes".

### Bisecting an engine crash — the recipe, because it is affordable

The crash is at INIT, so a 200 s run settles each cycle and only the archive re-links.
Eleven candidates took four cycles. Copy the recovered build, DELETE the objects to
exclude, rebuild the tree, re-link, run:

```bash
rm -rf /tmp/kd_sel; cp -a /tmp/kd_build /tmp/kd_sel
for x in "$@"; do rm -f "/tmp/kd_sel/$x.o"; done      # names to leave SHIPPED
./test/make_hull_lib.sh ../Thirdparty/metoolkit /tmp/mt_bi
./test/make_substituted_metoolkit.sh /tmp/kd_sel /tmp/mt_bi /tmp/mt_bisect
cmake --build build-subst121 -j"$(nproc)"             # cache points at /tmp/mt_bisect
# then the -SOFTWARERENDERER invocation from §6, and grep for 'START MATCH'
```

**Success is "reached `START MATCH`", never "did not SIGSEGV"** — §7c's lesson, and it
still holds. `test/try_subst.sh` is the older harness for this; it hard-codes
`-GL4ESRENDERER` and `build-subst108`, so update it or use the recipe above.

**What this does NOT show, and the environment changed under it.** On 2026-08-25 the engine
faults at the first HUD frame under Xvfb — `REALLOC FAILED ... NewSize=1351188168` in
`UCanvas::DrawTileStretched`, from a viewport the renderer creates at **2×1**. **The stock
build does exactly the same thing**: same map, same URL, same duration, logs identical
modulo timestamps and addresses, 523 lines each. So it is an environment regression, not a
recovery defect — but it means today's runs show START MATCH and a short tick, not §7c's
300 s of play. `-SOFTWARERENDERER` survives where `-GL4ESRENDERER` and `-OPENGLRENDERER`
do not, and is what the gdb runs above used. **Always run the stock control before reading
a crash as yours.**

---

## 8. The detectors — why objects are held back

> ### `MEASURED_WRONG` — the quarantine that holds on evidence, not on a pattern
>
> Every detector below recognises a SHAPE in the recovered source: a guessed stack frame,
> an out-of-range frame reference, a value Ghidra could not account for. That works because
> those shapes usually precede a defect. It cannot work when an object is clean by every
> shape and simply computes the wrong answer, and §11 item 0 has been recording that hole
> for several sessions — *"the quarantine can only hold what a detector recognises; an
> object that is simply never measured, and is wrong, walks straight in."*
>
> `recover.py`'s `MEASURED_WRONG` closes it. An entry is a claim about EVIDENCE and must
> cite the line in `proven.txt` that carries it; removing one is a release and needs the
> same standard as any other.
>
> **It is EMPTY, and the entry it briefly held is the point.** `keaLCPSolver` went in on
> `first@1 = 3.772e+00` and came out the same day, because the measurement that held it also
> located the defect — `test/bisect_object.sh` named `PrincipalSubmatrix` and the three float
> stores Ghidra had rendered as int conversions. A quarantine that holds on a measurement is
> also a place to start looking.
>
> It is **not** for a small, understood divergence. `IxCylinderCylinder` is measurably
> imperfect and §11 item 0 says explicitly to leave it in the build, because what it gets
> wrong is a field the engine does not read for that pair. This is for an object whose
> output would make any build containing it useless.


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

> ### THE OTHER SIDE: releasing a detector's complaint, and why it needed TWO mechanisms
>
> 2026-08-25 (fifth session). Two objects reached the end of the evidence chain — exact on
> all three scenes, exact per-function — while still tripping a detector. Holding a PROVEN
> object on a shape is the same error as letting an UNPROVEN one through on one.
>
> **`recover.RELEASED_UNMODELLED`** releases the *unaccounted value* detector, and it names
> the **identifiers**, not the object, so a NEW unmodelled read in the same object still
> quarantines it. `keaLCP_new`'s `extraout_EDX` goes into a call's alignment padding;
> `keaIntegrate_pc`'s `in_stack_ffffffa0` is a declared 76-byte outgoing argument area the
> copy loop fills completely. A release whose identifier the detector no longer reports
> prints a **stale-entry warning** rather than rotting quietly — the stale skip-list of §12.
>
> **`KD_MATERIALISED_BASE`** is different and more interesting, because the detector was
> firing on the output of a REPAIR. Releasing `keaLCP_new`'s `extraout_EDX` uncovered
> `GHIDRA_STACK_GUESS` on 20 sites of the form `(int)auStack_3c + uVar6 * -3` — every one of
> them written by `materialise_shifted_frame`. That detector's premise is *"Ghidra INVENTED
> the array, so the storage is not there"*, and the repair falsifies the premise: it
> redeclares the base as a pointer into an `alloca`'d block sized from `<object>.locals` and
> big enough by construction. The repair now marks each base it rewrites and the detector
> skips those, scoped to the function carrying the marker. **`MdtPartition` — the object the
> detector was written for — still reports its 30 sites**, which is the control.
>
> The general shape: when a detector fires on a repair's output, do not release the object,
> **falsify the detector's premise for that site and say how.**

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
   **RE-DIAGNOSED AND FIXED 2026-08-25 — the entry below is right about the symptom and
   wrong about the cause, and the correction unlocked three objects and repaired four more
   that had been silently wrong for months. See `fix_float_load_of_int` and §9's note at the
   end of this entry.**
   It works, in the sense that `McdSphyl` then compiles, and what it compiles is wrong.
   The line is `*radius = (float)s[1].mRefCtAndID + (float)s[1].prev;` and GCC rejects
   only the second cast, so cycling rewrites that one and leaves the first as an
   integer-to-float **conversion**. The original does no such thing —
   `flds 0x10(%ecx); fadds 0x14(%ecx)` — both operands are floats, and Ghidra mis-typed
   the first member. This is dead end 6 turned around: there the compiler was the only
   thing that knew, and here **the compiler is wrong, because it is reasoning about
   Ghidra's types.**

   > **AND THAT LAST SENTENCE IS THE PART THAT WAS WRONG.** The compiler is not reasoning
   > about Ghidra's types badly — it complains about EXACTLY ONE of the two casts, the one
   > whose operand is a POINTER, and it is right about that one and silent about the other
   > because `(float)<int>` is legal C. So cycling "fixes" the line and leaves a conversion
   > that destroys the value. The line needs BOTH casts rewritten, and which both is decided
   > by the OPERAND'S TYPE, not by which one gcc named.
   >
   > `fix_float_load_of_int` does that, with the same evidence
   > `fix_int_store_of_float` uses in the other direction: an integer-to-float conversion on
   > i386 is `fild` or `cvtsi2s*`, and `McdBox.o`, `McdSphyl.o` and `McdTriangleList.o`
   > contain NOT ONE in any function. The operand's type comes from the object's own DWARF,
   > through the typedef chain — `McdSphylID` → `McdGeometryID` → `struct _McdGeometry *` —
   > and `_McdGeometry` is 16 bytes, so `s[1].mRefCtAndID` is `McdSphyl::mRadius`, a
   > `MeReal`, which the shipped code reads with `flds 0x10`.
   >
   > All three objects now compile, **and the same rule repaired four objects that had
   > compiled all along and were wrong** — `McdSphere` computed a sphere's mass properties
   > from its radius read as an integer. No gate had caught them.
   >
   > Two of my own checks silently declined on the way, both found by calling them directly
   > on the offending line: the member typer's whole-expression subscript guard rejected
   > `g[1].member` before the member pattern ran, and `fix_int_store_of_float`'s cheap
   > early-out still tested for the literal `(int)` after the regex was widened past it.
   > **A guard that disagrees with the rule it guards is a rule that does not run.**
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

   **And now there is a reason as well as a refutation** — §11 item 0. The separating axes
   are numerically degenerate on axis-aligned input, so the winner is decided by rounding
   *whatever* the rounding mode is. `-ffloat-store` did not fail to align the arithmetic;
   there was nothing to align it to.

18. **Re-dumping with the DWARF importer's variables turned off**, on the theory that
   Ghidra's native stack recovery beats the un-placeable variables its importer creates for
   functions whose DWARF carries no `DW_AT_location`. This was WHAT REMAINS item 1's
   designated move for two sessions. It is refuted, and it took three minutes because the
   experiment is eight objects, not 153 (§5, `KD_GHIDRA_OPTS`):

   | setting | `stack0x` | `in_stack_` | `register0x` |
   |---|---:|---:|---:|
   | (out9, control — byte-identical re-dump) | 25 | 346 | 3 |
   | `DWARF.Import Local Variable Info=false` | **25** | **346** | **3** |
   | `DWARF.Ignore Parameter Storage Info=true` | files byte-identical to out9 | | |
   | `DWARF.Import Functions=false` | 25 | **352** | 3 |

   Not one construct moves. What the first setting changes is **names**: `MeReal myw[3]`
   becomes `local_2c`/`local_28`/`local_24`, three scalars where the DWARF had a correctly
   typed array. Ghidra's native stack recovery and its DWARF importer produce the SAME
   frame model, and the DWARF only supplies names for slots Ghidra had already placed. The
   third setting is the strongest form of the hypothesis and makes `in_stack_` counts go
   **up**.

   Run the control first. Eight objects re-dumped with no options set come out
   byte-identical to `out9`; without that, "the option changed nothing" and "my harness
   differs from the one that made out9" are the same output.

19. **Giving C++ FREE functions their mangled prototypes** the way §5b gives them to class
   members. The route is sound — a subprogram's `DW_AT_low_pc` and the ELF symbol's value
   are offsets into the same `.text`, so the symbol at the concrete instance's `low_pc` is
   the linkage name, no mangling re-implemented — and it lifts coverage from 40 of 75
   undefined mangled symbols to 66. **It also takes the build from 109 objects to 105.**

   Those functions (`MovingBoxBoxIntersect`, `AccumulateSphylContacts`,
   `ConvexHullNSegment`, `PolynomialRoots`) ARE declared in the importing object's own
   DWARF, so Ghidra already had their real signatures — and `gen_protos.simple_type` turns
   every pointer into `void *`, so applying ours *overwrote* good types with worse ones.
   `lsVec3 *` became `void *` in `IxBoxBox`, `IxConvexTriList`, `IxCylinderCylinder` and
   `IxSphylPrimitives` — four objects with live in-game evidence in `proven.txt` — and they
   stopped compiling.

   **The distinction that matters is not C-vs-C++, it is whether Ghidra already had a
   signature.** Class members do not have this problem, measured rather than argued: with
   the member route alone all 109 compiled objects stay byte-identical.

20. **The OVERLAY repair for the fragmented solver frames.** §11 item 2 establishes that
   those frames are described and fragmented rather than undescribed — every `stack0x`
   site starts a contiguous run of declared locals covering at least what the copy loop
   writes. The obvious repair follows: alias the run into one packed struct so that
   `&stack0xNNNN` and every `in_stack_*` read see the same storage, taking offsets, sizes
   and types from `<object>.locals` so nothing is inferred.

   **It works, and it is wrong.** On `MdtWorld` it takes **10 errors to 0** — all four
   `stack0x` sites resolved — and then:

   ```
   ./test/substitute_test.sh /tmp/kd_mw $LIB test/scene_chain.c
     [ NaN  ] MdtWorld
   ```

   NaN over 900 steps, and the ragdoll scene will not run at all. **Both variants fail**:
   aliasing the whole run, and aliasing only the `in_stack_*` members while leaving
   Ghidra's invented `uVar18`/`pMVar19` as independent variables and padding their slots.
   So it is not simply "the copy clobbers live values" — the argument block itself is not
   the contiguous run, however it is sliced.

   **The near-miss is the part to carry.** An overlay RENAMES `in_stack_*` away, and
   `recover.py`'s two detectors match `\bkd_argslot_\w+\b` and
   `in_stack_[0-9a-f]+|extraout_|unaff_`. A block named anything else matches NEITHER, so
   this object — which goes NaN on the first scene that runs it — compiled clean and
   would have been classified `ok` and put straight into the build. Naming the block
   `kd_argslot_frameNNNN` restores the quarantine (297 hits). **Any future attempt must
   keep a detector-visible name**, and that is a general rule for any repair that renames
   a detector's pattern out of existence.

   And one thing no naming fix can reach: in `MdtWorldStepSafeTime` Ghidra passes a frame
   slot where the original passes `po->totalBodies`, so `num_bodies` is genuinely gone
   from that function regardless.

   **`MdtWorld` did exactly what §12 asks a validation subject to do** — a non-kea object
   where a wrong answer is one command away, and it refuted the repair in one. That is the
   reason to keep choosing subjects that way, not a reason to stop.

21. **`-w` followed by `-Wsomething` in clang.** `-w` wins, the diagnostic never fires, and
   the run reports zero. That is how a first pass at §6b's pointer-truncation gate read
   "arm64: 0 warnings" — the most reassuring possible output for the most dangerous known
   defect in the project. Use `-Wno-everything`, which is designed to be overridden.

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
   straight in.

   Reproduce with `test/make_shadow_metoolkit.sh` over a copy of `/tmp/kd_build` with
   `IxCylinderTriList` compiled in, then `KD_RUNTIME=/tmp/kd_runtime ./test/run_map.sh
   test-simple-physics 300 "$COMMON_URL"`. `KD_RUNTIME` keeps the run off
   `/home/ion/karma-run`, which matters if someone else is using it.

   > ### RE-FRAMED, fifth session: `dims` IS NOT A MEASUREMENT FOR THIS PAIR
   >
   > **The shipped library does not reproduce it either.** `KD_JITTER=<eps>` runs the
   > ORIGINAL against ITSELF with model2 displaced by `eps` between the two calls. On the
   > same 200,000 axis-aligned pairs:
   >
   > | | count_diff | dims_diff | worst delta |
   > |---|---:|---:|---|
   > | nudge 0 (control) | 0 | **0** | 0.000e+00 |
   > | nudge 1e-12 (control) | 0 | **0** | 0.000e+00 |
   > | nudge 1e-9 | 0 | 10 | 1.788e-07 |
   > | nudge **1e-7** | 0 | **7,798** | 7.997e-01 |
   > | **our recovery, no nudge at all** | 1 | **6,432** | 8.000e-01 |
   >
   > A ten-millionth of a metre is below f32 resolution at ordinary UT2004 world
   > coordinates. The shipped library disagrees with itself more often than we disagree
   > with it, and produces the same 8.0e-01 worst delta we are charged with.
   >
   > **And only this pair.** Same probe, same nudge, grid regime: `McdBoxBoxIntersect` 0 of
   > 28,401 touching, `McdSphereSphereIntersect` 0 of 76,407, `McdSphylSphylIntersect` 0 of
   > 90,168, `McdSphylBoxIntersect` 0 of 62,078, and — the sibling released the same week —
   > `McdCylinderTriangleListIntersect` **0 of 147,389**. So this is not "float comparisons
   > are unstable", it is a property of THIS function's separating-axis set.
   >
   > **Why.** Instrumenting the recovered `OverlapCylCyl` to print every candidate:
   >
   > ```
   > cand 10 sep -0.301695902     cand 8 sep -0.301695913
   > cand  6 sep -0.301695973     cand 2 sep -0.301695922
   > cand  9 sep -0.301695914
   > ```
   >
   > Five candidates agreeing to seven significant figures. Over 57,079 calls, **29.7%
   > choose their axis on a margin of 1e-6 relative or less** and 7.4% on an exact tie. For
   > an axis-aligned cylinder pair the candidate axes are geometrically DEGENERATE, so
   > `normInfo` — and therefore `dims` — is decided by rounding.
   >
   > **The obvious hypothesis is refuted.** `<` → `<=` at all six `maxSeparation <` sites
   > and both endpoint min-search sites, one at a time under `KD_GRID=1`: no change at all,
   > except at `:1008` which gets WORSE (6,385 → 7,563). The shipped code agrees — at
   > `0x26aa` it is `fcoms; fnstsw %ax; test $0x45,%ah; jne`, i.e. take the new candidate
   > iff STRICTLY greater, which is what the recovery already does.
   >
   > **And then the stronger answer, which is not statistical: FOR THIS PAIR THE
   > DIVERGING FIELD IS NEVER READ.** `McdContact::dims` has three consumers in the engine
   > and one in Karma, and all four were read rather than sampled:
   >
   > | consumer | when it runs |
   > |---|---|
   > | `KarmaSupport.cpp:303` `dims & 0x00FF` | `if(!a1)` — and the comment above it says "the ONLY model without an Actor must be the 'world', which must be the tri-list" |
   > | `KarmaSupport.cpp:319` `dims >> 8` | the mirror case, model2 is the TriangleList |
   > | `KarmaSupport.cpp:535` | inside `#if 0` |
   > | `McdContactSimplify` | `if ((char)dims == 2 \|\| dims>>8 == 2)` — the only branch on this field in all 153 dumps |
   >
   > A Cylinder × Cylinder contact is two Actors and no TriangleList, so neither live engine
   > branch runs. And the Karma branch is **false for 0x103, 0x301 and 0x303** — the only
   > three values the divergence involves — so it takes the same path either way. (What the
   > field carries at 303/319 is the TRIANGLE FLAGS, not the "0/1/2 point/line/surface" the
   > header documents. Karma overloads it per side.)
   >
   > **A fourth yardstick, partly explored, recorded so it is not re-derived.**
   > `lib.rel/` ships FOUR Linux builds of the same source — `linux_single_gcc2.95`,
   > `linux_single_gcc3.1`, `linux_single_gcc3.2` (all i386) and `linux_hx_single`
   > (x86-64) — with identical exported symbol sets. Pointing `difftest_pair.sh` at a
   > directory of gcc3.1 members runs **MathEngine's own build against MathEngine's own
   > build** through the existing harness, with no code changes:
   >
   > ```bash
   > mkdir /tmp/vendor && cd /tmp/vendor
   > ar x .../lib.rel/linux_single_gcc3.1/libMcdPrimitives.a   # + libMcdConvex.a
   > ./test/difftest_pair.sh /tmp/vendor ../Thirdparty/metoolkit
   > ```
   >
   > It works, and the useful rows are the CONTROLS: gcc3.1 vs gcc3.2 reads **0 count,
   > 0 dims** on `McdCylinderTriangleListIntersect` and on `McdBoxBoxIntersect`, in both
   > regimes — so the vendor's own builds ARE reproducible for the pairs we release. Two
   > limits: **`gcc2.95` cannot be linked at all** (pre-Itanium mangling —
   > `NSegmentSegment__FPCfT0ffT0T0ffPfN28`), and gcc3.1's `IxCylinderCylinder.o`
   > **segfaults on the first call** in this hybrid link, so the one number this was aimed
   > at is not available from it. Extracting whole archives instead of individual members
   > also segfaults — the prefixed registrars and the shipped ones then disagree about
   > geometry type ids.
   >
   > **Scope, stated honestly:** that branch IS live for this pair in general —
   > `OverlapCylCyl` also emits `2` and `0x200`, both of which make it true. The claim is
   > that the measured divergences never reach those values, which is about the histogram,
   > not a proof over all inputs.
   >
   > **Disposition, and it is on the line in `proven.txt`.** Do NOT pull it from the
   > substituted build. Do NOT release it either — the **3 `count_diff` in 24,111 real
   > calls** is 0.012%, is not explained by any of this, and `count` IS a decision the
   > engine acts on. **And as of 2026-08-25 it is LOCATED**: difftest iteration 194376,
   > count 4/2, with our contact [1] carrying the shipped contact [0]'s separation — so the
   > footprint differs upstream of the emission filter. Note that the jitter probe above
   > reports **0 `count_diff`**, so none of the `dims` reasoning extends to it. That is the whole remaining charge, and it is what a live match should
   > be scored on, with `dims` reported separately the way `nonfinite` already is (§7).

1. **DONE — the vtable call sites.** `tools/gen_vtable_callsites.py` +
   `DumpDecomp.applyVtableCallsiteOverrides()`, adopted as `out8`. §5a has the method, the
   checks and the one bug it went through. Items 1 and 3 of the old list were indeed "one
   shared fix, not two", as this file predicted. Nothing to do here; it is written down
   because the technique generalises to any indirect call whose target is pinned by a
   relocation.

2. **THE JOB: the frames — DONE. `libMdtKea` is recovered.** Every object in it is
   bit-identical to the shipped library on all three scenes. `keaIntegrate_pc` was the last
   and it came out on 2026-08-25 (fifth session) — **item 2a below has it, and the second of
   its two defects is the most important open lead in this file.**

   `keaLCP_new` was the one before, and it came out on 2026-08-25 in two steps. The frame was
   materialised — and **the "61 sites with mixed byte and element strides" that justified
   leaving it alone was a MISREADING**: `auStack_3c[i - uVar6]` is a `uint *` indexed in
   elements, i.e. `(int)auStack_3c + 4*i - 4*uVar6`, which is region FOUR in byte terms and
   consistent with everything beside it. What was real is that `auStack_3c` is a
   twelve-byte local with six alloca'd regions below it, and
   `materialise_shifted_frame` gives that area real storage from offsets READ out of
   `<object>.locals` and the `stack0xHHHH` names. Then it compiled and was 7.844e-03 out at
   step 0, and `bisect_object.sh` named one call with three arguments dropped — a virtual
   call on a PARAMETER, which §5a could not see (§5e, `out13`).

   **The frame problem that is left is one `stack0x` each in `MdtBcl` and `MeMath`**, both
   diagnosed in §5c and neither on anybody's path. What follows is the background that led
   here, and it still stands.

   "Ghidra cannot model these frames" was true and understated. For
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

   **THE GHIDRA-SIDE ATTACK IS TRIED AND REFUTED — dead end 18.** It was cheap and
   decisive exactly as advertised, and the answer was no. Disabling DWARF variable import,
   parameter storage, or function import entirely leaves `stack0x`, `in_stack_` and
   `register0x` counts UNCHANGED on all eight objects. Ghidra's native stack recovery
   produces the same frame model; the DWARF only supplies names for slots it had already
   placed, and turning it off replaces `MeReal myw[3]` with three untyped scalars. The
   experiment took three minutes, not 75–120, because it is eight objects rather than 153.

   **WHAT THE EXPERIMENT FOUND INSTEAD, and it is worth more than what it was looking for:
   the frame is not lost. It is the OUTGOING ARGUMENT AREA, and every part of it is
   pinned by the cdecl ABI rather than by debug info.** Read against the disassembly of
   `MdtKeaAddConstraintForces`:

   | Ghidra spells it | it is | confirmed by |
   |---|---|---|
   | `register0x00000010` | the ENTRY value of ESP (x86 register-space offset 0x10), so `+1` word is `&pconstraints` at `ebp+8` | `lea 0x8(%ebp),%esi` in the prologue |
   | `&stack0xfffffdd8` | the outgoing argument slot at `esp+4` | `lea 0x4(%esp),%edi` then `rep movsl` with `ecx=0x17` = 92 bytes |
   | `&stack0xfffffe34` | the outgoing argument slot at `esp+0x60` | `lea 0x60(%esp),%edi`, `ecx=0x13` = 76 bytes |
   | `in_stack_NNNN` | the VALUE of the same slot whose ADDRESS is `stack0xNNNN` | they encode the same frame offset |

   So the copy loops are `constraints = pconstraints` and `parameters = parameters`
   marshalled for a call, and the `MVar1.field = in_stack_...` wall is Ghidra reassembling
   the by-value aggregate to pass it. Nothing there is unknowable.

   **One consequence has already been fixed and is in `out10`.** Ghidra was laying that
   call's argument list out with `this` dropped, so every by-value aggregate was read one
   word early — §5b. That was not a frame problem at all; it was a missing prototype,
   because `ParseKarmaHeaders` matches ELF symbols by name and every C++ function in the
   corpus is mangled.

   > ### AND THE FRAMES ARE NOT UNDESCRIBED. They are described and FRAGMENTED.
   >
   > 2026-08-25, from `<object>.locals` (§5c) via `tools/frame_offsets.py --cover`. Every
   > `stack0x` site in the solver objects sits at the start of a **contiguous run of
   > declared locals** covering at least what the copy loop writes:
   >
   > | object / site | copy writes | contiguous described |
   > |---|---:|---|
   > | `MdtWorld` ffffff6c | 76 B | **80 B in 9 locals** |
   > | `MdtWorld` ffffff04 | 92 B | 92 B in 1 |
   > | `keaRbdCore_unified` fffffdd8 | 92 B | **176 B in 20 locals** |
   > | `keaRbdCore_unified` fffffe2c | — | 92 B in 16 |
   > | `keaIntegrate_pc` ffffffa0 | 76 B | 76 B in 1 |
   > | `keaMemory` ffffff88 | 92 B | 92 B in 1 |
   >
   > `fix_stack_address_name` declines on the fragmented ones because it size-checks ONE
   > local — `MdtWorld`'s ffffff6c is a 4-byte `MeReal` — and it is right to: writing 76
   > bytes there is a 72-byte overflow. **But the other 72 bytes are not missing.** They
   > are `in_stack_ffffff70[48]` and six scalars Ghidra also uses as ordinary variables, in
   > that order, with no gap.
   >
   > **So the repair is an OVERLAY, not a reconstruction.** Alias the contiguous run into
   > one block so `&stack0xNNNN` and every `in_stack_*` read see the same storage. Offsets,
   > sizes and types all come from the table, so nothing is inferred, and the recovered
   > function does not need to match the original's frame LAYOUT — only to be
   > self-consistent.
   >
   > **What has to be got right, and is why it is written down rather than done:** the run
   > includes locals the decompiler also treats as SSA VALUES (`uVar18`, `pMVar19`), and
   > giving those aliased storage is faithful only if Ghidra means they live at that offset
   > for the whole function. There is a worked reason to think aliasing HELPS rather than
   > hurts — `MdtWorld` reads `kVar2._kd[0x44] = (char)uVar22` where `uVar22` was last
   > assigned a stale return address, and under aliasing it would instead read what the
   > copy loop put there, which is what the machine code does — but that is an argument,
   > not a measurement.
   >
   > **Validate on `MdtWorld`.** Non-kea, ten errors, and `scene_chain` drives
   > `MdtWorldStep` 900 times, so a wrong answer shows up on a gate rather than in a
   > review column. That is the subject §12's standing lesson asks for and item 2 did not
   > have.
   >
   > **AND THAT WAS DONE, AND THE OVERLAY IS REFUTED — dead end 20.** It takes `MdtWorld`
   > from 10 errors to 0 and then goes **NaN on `scene_chain`**, in both the
   > alias-everything and the alias-only-`in_stack_*` forms. The frames really are
   > fragmented; the argument block is just not the contiguous run, however it is sliced.
   > What is left of this lead is the fact, not the repair.

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

2a. **`keaIntegrate_pc` — TWO DEFECTS, and the second one no gate here can see.**
   2026-08-25, fifth session. The object is now bit-identical on all three scenes and over
   450,000 bodies of a per-function A/B on both code paths.

   **THE DROPPED ROUNDING.** The shipped function declares `MeReal dq[4]` at `ebp-0x38` and
   spills three of its four elements to 32-bit slots:

   ```
   2ca: fstps -0x34(%ebp)     dq[1], ROUNDED TO 32 BITS
   327: flds  -0x34(%ebp)     reloaded
   32c: fmuls 0x14(%ebp)      * stepsize
   ```

   Ghidra folds the store and its reload into one expression — `qrot[1] = EXPR * 0.5 *
   stepsize + qrot[1]` — so the local is left **declared and unused** and the
   round-to-storage-precision is gone. On i386 gcc then keeps the whole chain in an 80-bit
   x87 register. That is the entire `first@13 = 1.500e-08`: routing `dq[1..3]` back through
   `KD_F32()` takes the A/B from **52,902 divergent bodies in 400,000 to ZERO**. `dq[0]` must
   NOT be rounded — the shipped code stores it with `fsts` and keeps using `st(0)`, so that
   store is dead. The same shape is on the fast-spin path, which the engine reaches
   (`KConstraint.cpp:357` sets the flag).

   **This is not `-ffloat-store`** (dead end 17). That flag rounds everywhere, including the
   many places the original did not, and it made `IxCylinderCylinder` five orders of
   magnitude worse. This rounds at three sites, each shown by the disassembly to be a 4-byte
   spill AND reload of a slot the DWARF names. `ghidra_clean.restore_float_text` checks that
   evidence and **raises** if it is absent.

   > ### ⚠ THE ASSOCIATION DEFECT — INVISIBLE TO EVERY GATE IN THIS PROJECT
   >
   > Ghidra prints a right-leaning float `+` chain **flat**: `a + b + c` for a tree that is
   > `a + (b + c)`. C then parses it left-leaning, over the wrong operand order. Float
   > addition is not associative, so this is a real arithmetic change — and the text is
   > genuinely ambiguous, because Ghidra prints the same `a + b + c` for both trees.
   >
   > **Ground truth here was not an inference.** `Source/Engine/Src/KDynStep.cpp:618` is
   > `KIntegrateSystem` — MathEngine's own integrator, copied VERBATIM into UT2004 by Epic.
   > Its association is the shipped `fadd` order and not Ghidra's:
   >
   > | | |
   > |---|---|
   > | Ghidra | `dq[2] = (q1*w2 + -q3*w0 + q0*w1) * 0.5` |
   > | KDynStep.cpp:657 | `dq[2] = 0.5*( -q3*w0 + q0*w1 + q1*w2 )` |
   >
   > Three sites in this one function: the fast-spin dot product, `dq[2]`, and the
   > normalisation sum.
   >
   > **AND ON i386 IT IS EXACTLY INERT.** A float product needs 48 mantissa bits and the x87
   > register has 64, so these sums are exact whatever the association — measured **0
   > differences in 2,000,000 samples** under `-mfpmath=387`. The same probe under
   > `-mfpmath=sse`, which is the arithmetic **wasm32, armv7 and arm64 all give**, differs in
   > **31%** (621,410 and 624,401 of 2,000,000 on the two sites tested).
   >
   > So: the three substitute scenes, `difftest_pair`, `scene_census`, `gate_sensitivity`
   > and the whole 450,000-body A/B are **structurally blind** to it, and the port target is
   > exactly where it bites. The first symptom will be wasm physics that drifts from native
   > with no offline test able to reproduce it.
   >
   > **Three sites in one object of 153 have been examined. Nobody has looked at the rest.**
   > A corpus scan for declared-but-unused float locals — the fingerprint of the *rounding*
   > half — finds **220 sites across 47 objects**, and only the ones whose slots show a
   > `fst(p)s`/`flds` pair in the shipped disassembly are real. The association half has no
   > textual fingerprint at all and needs an oracle that reads `fadd`/`faddp` order out of
   > the machine code. That is WHAT REMAINS row 1a and WHERE MY HEAD IS MOVE 2.

3. **DONE 2026-08-25 — the vptr store, and it is MEASURED.** Both objects failed on
   `_vanillaFunctions` / `_vanillaQMatrix` being undeclared. That is the **same
   EXTERNAL-slot addend collision as `keaMemory`** (§5) and it was **already inverted
   exactly and uniquely** by existing machinery —
   `ghidra_clean.relocation_targets(obj, per_function=True)`:

   ```
   keaRbdCore_unified  MdtKeaAddConstraintForces
     __ZN12keaFunctions8initPoolEPvi   -> (_ZTV20keaFunctions_Vanilla, 8)
     __ZN12keaLCPSolver8solveLCPE...   -> (_ZTV26keaMatrix_pcSparse_vanilla, 8)
   keaLCPSolver  makeXandW / PrincipalSubmatrix / PrincipalPivotTransform
     ___gxx_personality_v0             -> (_ZTV26keaMatrix_pcSparse_vanilla, 8)
   ```

   `+8` is the Itanium ABI address point, so `_vanillaFunctions = keaFunctions::initPool;`
   is really `vanillaFunctions.vptr = &vtable[2]`, and **`__gxx_personality_v0` is not the
   personality routine at all**. Corpus-wide only these 2 objects import a vtable.

   **`ghidra_clean.fix_vptr_store` is a PRE-PASS, not a REPAIR_RULES entry**, and that is
   not a detail: nothing about it is driven by a diagnostic, and the `undeclared` pattern
   is already spoken for by `fix_mislabelled_external` — a second entry with that pattern
   is dead code, per the note above `REPAIR_RULES`. It aliases `_<local>` to the local's
   first word, so nothing about the frame is reconstructed.

   **Six static checks, each read from the object**, are in its docstring. The one that
   matters settles the only thing NOT read — the SCALE. Ghidra types the base
   `undefined4 *` in `makeXandW`, so `[2]` is the third WORD, and leaves it byte-like in
   `PrincipalSubmatrix`, so `+ 0xc` is the fourth. Read the second at word scale and the
   offsets are 48 and 64 in a table whose last slot is 32; read the first at byte scale and
   `[2]` is not a multiple of four. Either error is refused rather than compiled.

   **And then the measurement, which is `test/vptr_ab.sh` and is the ninth gate.** It
   rebuilds the object with every repaired dispatch replaced by a DIRECT call to the symbol
   that slot holds — from `vtable_slots.py`, i.e. from the relocations in the object that
   DEFINES the vtable, not read back out of the rewrite — so the two builds differ in
   exactly one way: one loads a function pointer out of memory at run time and the other
   does not.

   | scene | devirtualised control | rotated slot | address point +12 |
   |---|---|---|---|
   | `scene_chain` | bit-identical | **BLIND** | **BLIND** |
   | `scene_ragdoll` | bit-identical | crashes | crashes |
   | `scene_boxes_on_plane` | bit-identical | crashes | crashes |

   The `BLIND` row is the point of the gate, not a gap in it. `substitute_test.sh` prints
   `keaLCPSolver — trajectory bit-identical` on `scene_chain`, and `scene_census` says only
   4 of 15 functions ran there — none of the three the repair touched. §4a's distinction,
   applied to a rewrite instead of to an object.

   **A site whose arguments Ghidra DROPPED is skipped, not rewritten.** `keaRbdCore_unified`
   has one — `(*__ZN12keaLCPSolver8solveLCP...)()`, zero arguments where the slot wants two.
   Rewriting it would compile into a call to the right function with the wrong stack, which
   is the one outcome this project treats as worse than not compiling, so the site keeps its
   error. Any OTHER arity mismatch declines the whole store.

   **A trap for whoever unblocks `keaRbdCore_unified`.** Those dispatches go through
   `code *`, which is UNPROTOTYPED, and two of its slots take a float scalar
   (`calcJinvMandRHS`, `calcIworldandNonInertialForceandVhmf`). On i386 a float argument to
   an unprototyped function is promoted to DOUBLE — eight bytes where the callee reads
   four. `keaLCPSolver` is safe because none of its four slots takes a float.

3a. **DONE 2026-08-25 — `keaMemory`, the allocator, and a `void *` has no members.**
   The handover called this "the EXTERNAL-slot addend collision". It is not:
   `relocation_targets` resolves `_pool_ptr` to `('pool_ptr', 0)`, the IDENTITY reading, and
   `fix_mislabelled_external` had the right rewrite all along. Two other things stopped it,
   and both are general.

   **The repair loop's rejection memory outlived its reason.** `applied` holds every edit
   TRIED, so a line that got a wrong candidate can be offered another — and a REJECTION is
   then permanent. `_pool_ptr` → `pool_ptr` exchanges three `undeclared` diagnostics for
   three of a different kind, so on the round it was first offered the count did not FALL
   and it was refused; by the time the surrounding repairs made it a strict improvement it
   could never be offered again. The loop now forgets its rejections and goes round once
   more, at most `max_restarts` times and only after real progress. That anecdote was
   already in the `max_rounds` comment — McdBox, 29 errors, 8 on a second run by hand.

   **And then the typing.** Ghidra gives an untyped external its own working type and spells
   the POINTER through it:

   ```c
   if (pool_max < (undefined1 *)((int)pool_ptr->invI0    + iVar10))
   piVar4    =            (int *)((int)pool_ptr->invI0[0] + iVar10);
   mem->rhs  = pool_ptr->invI0;
   ```

   The shipped block is `mov pool_ptr,%edx; lea (%ebx,%edx,1),%eax; cmp; ja; add %ebx,%edx;
   mov %edx,pool_ptr; mov %esi,0xc(%ecx)` — pointer arithmetic and a store, with **no load
   through the pointer anywhere in the function**.

   **THE OBVIOUS REPAIR IS WRONG AND IT COMPILES.** Supplying the cast Ghidra elided —
   `((MdtKeaInverseMassMatrix *)pool_ptr)->invI0` — is right for the two array-decay
   spellings and wrong for `->invI0[0]`, which then reads a `MeReal` out of the pool. That
   variant was built and run: it **segfaults on all three scenes**, which is both the reason
   not to take it and the reason the bit-identical result below means something.

   `fix_void_pointer_member` drops the member access, on four reads: the extern's DWARF type
   is `void *`; every cast Ghidra assigns to the name agrees on one struct; that struct's
   DWARF **in this object** puts the member at offset 0 as an array of a floating type; and
   the shipped function contains **no floating-point instruction**, so the only other
   reading of `->M[0]` is refuted by the object rather than argued away.

   | | |
   |---|---|
   | all three scenes | trajectory **bit-identical** |
   | `scene_census` (chain) | `vanillaAllocateMemory` 900 calls, `keaPoolAlloc` 14,400 |
   | the cast repair, as a control | **segfaults on all three** |
   | `gate_sensitivity` | "no float arithmetic" — it cannot speak here, which is the same fact the fourth check rests on |

   **The fourth check was wrong first, in the way this project keeps meeting.** It matched
   the mnemonic out of the whole objdump line and read the last hex BYTE as one, so
   `c7 45 f0` came back as the x87 `f0`; every function looked as if it did float work and
   the rule declined everywhere, looking exactly like a rule that does not apply.

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
   - **`McdCacheHello`/`Goodbye`** were not stubbed and never called, which is why GJK
     ran cold-path-only here. **CLOSED 2026-08-25 — `KD_WARM=<K>`**, which runs the pair
     as a TRACK of K consecutive frames with the cache allocated once per track and the
     bodies nudged a few millimetres per frame instead of re-scattered. On the busiest
     pair family in the census: **0 ret, 0 count, 0 dims over 200,000 pairs**, self-test
     14/14 clean in the same mode.

     **The cache was verified LIVE before the zeros were believed** — `KD_WARMDEBUG=1`
     shows one block address for a whole track with the witness normal carried across
     frames and refined. A warm run whose cache never populated reads identically from
     the summary, and that is the failure this project keeps meeting.

     **And it refuted a prediction this file made.** §7 says the shared-cache exposure is
     "real and currently inert — the moment anyone warms the cache it stops being inert".
     It does not: `KD_WARM=8 KD_SHARECACHE=1` reads exactly the same as isolated, with the
     switch verified wired (both sides show the SAME block address). The reading is not
     "sharing does not matter" — it is that the recovered `McdGjk` leaves the cache in the
     same state the original does, so there is nothing to read differently. It agrees on
     60 bytes of internal state it is never asked about, over eight compounding frames.

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

## 12. What FULLY COMPLETE looks like

**Complete is not "every object recovered". It is "UT2004 links no shipped `metoolkit`
member, on all three targets, and plays."** Here is the whole definition, as a checklist
you can run.

### The finish line, in seven checks

| # | check | how you know | today |
|---|---|---|---|
| 1 | **`tools/dropin_gap.py` reports ZERO shipped members.** | §3c. It is checked against a real link — 111 of 111 undefined symbols predicted. | **20 members, 148 symbols** |
| 2 | **The engine LINKS with every shipped member deleted** and plays a match on i386. | `make_hull_lib.sh` + `make_substituted_metoolkit.sh` into a tree with the rest `ar d`'d, then §6. Success is "reached `START MATCH`" and ran to the timeout, against a STOCK control on the same map. | links today only because 20 members are still shipped |
| 3 | **Every pair the census shows the game calling is validated** — 0 `ret_diff`, 0 `count_diff`, 0 `dims_diff`, 0 `overrun` over a multi-hour session, `KD_SELFTEST` clean, evidence on a line in `proven.txt`. | §3, §7 | **13 of 15**; two cylinder pairs measurably imperfect and located |
| 4 | **All nine gates green** on the whole build, every time. | §4's gate list | green at 122 objects |
| 5 | **wasm32 and armv7 build and RUN**, and arm64 either runs or is retired. | `test/wasm_check.sh`, `test/ptrwidth_check.sh`, and the web agent | compiles on all three; **nothing has EXECUTED on any of them**; arm64 has 2,680 pointer truncations |
| 6 | **The association defect is settled corpus-wide.** | §11 item 2a | 3 sites in 1 object of 153; **no gate here can see it** |
| 7 | **No detector suppressed, nothing released without evidence.** | §8, `proven.txt` | holding |

**Checks 1 and 2 are the deliverable. 3–7 are what stop it being a deliverable that
crashes.** If you are ever unsure whether a piece of work counts as progress, ask which of
these seven it moves; if the answer is none, it is out of scope.

### What is explicitly NOT required

- **Objects the engine cannot reach.** §3b retires 31 permanently — MathEngine's demo
  viewer, its other constraint types, `MeASELoad`. They are out of scope, not a backlog.
- **Collision pairs UT2004 never calls.** 22 of 37, two of them provably unreachable (§3a).
- **Bit-exactness with MathEngine where the engine cannot observe the difference.**
  `IxCylinderCylinder`'s 925 `dims_diff` is a field the engine does not read for that pair,
  and the shipped library does not reproduce it either (§11 item 0).
- **Being a good general-purpose physics library.** Nobody else will link this.

### The seven items, and exactly where each one stands

| # | item | state |
|---|---|---|
| 1 | every pair the census shows the game calling is recovered and validated | **13 of 15.** The two open ones are `IxCylinderTriList` (37 `count_diff` in 153,391 live — its 2026-08-24 release on "0 structural in 71,417" did NOT replicate) and `IxCylinderCylinder` (1 `count_diff` in 24,111). **Both now reproduce OFFLINE** and neither needs a match. §3, `proven.txt`, MOVE 3. |
| 2 | validated = 0 ret/count/dims/overrun in a live match, `KD_SELFTEST` clean, evidence on the line | **DONE** for those twelve, and it is the standard the two new pairs have to meet. |
| 3 | all three scenes clean for every recovered object, *and* checked for sensitivity | **DONE**, and the sensitivity check (§4a) is what makes it mean anything. |
| 4 | qhull and the asset loader **replaced**, not recovered | **DONE, both halves — but the asset half was RECOVERED, not replaced (§8c), which is a better outcome: exact rather than equivalent.** Qhull, all four tiers: `src/McdConvexCreateHull/kd_convexhull.c` replaces all 15 exported functions — 1.4 MB → 10 KB: 100,633 invariant checks, identical geometry and volumes, a collision A/B differing on 2 borderline pairs in 2.4 M, and **a live ONS match with 15,425 real GJK calls and 0 structural divergences**. wasm32 clean with an identical symbol set. The asset loader is 9 of 9 recovered (§8c) and needs no replacement. |
| 5 | no detector suppressed, nothing released without evidence | **HOLDING, and the hole found here has been re-framed rather than closed.** 22 objects quarantined and the quarantine is load-bearing (§4a: `MdtPartition` alone turns a bit-identical scene into a SIGSEGV). `IxCylinderCylinder` is still un-held and still unreleased — but its 925 `dims_diff` is now known to be a label the shipped library **does not reproduce against itself** under a 1e-7 m nudge (§11 item 0), so it is not the defect it looked like. The general point stands unchanged: "not held" is not "validated". |
| 6 | builds as ordinary C for wasm32 **and arm64/armv7** | **wasm32 DONE** (122/122, byte-identical symbol sets). **armv7 DONE** (122/122, symbol sets identical, **0 pointer-truncation diagnostics** — it is a 32-bit-pointer target so the recovery's core assumption holds). **arm64 COMPILES AND IS NOT TRUSTED** — 122/122 with identical symbol sets and **2,680 pointer-truncation diagnostics across 73 of the 122 objects**, measured corpus-wide by `test/ptrwidth_check.sh`, which is the gate this row used to say could not exist. The old "920 vs 23" figure was two diagnostic sets added together; §6b has the correction. Nothing has been *executed* on any of the three — **and there is now a second reason not to trust that they would agree if they did: §11 item 2a's association defect is exactly inert on x87 and 31% divergent on all three of these targets.** |
| 7 | engine runs on recovered Karma with **no shipped `.a` in the link at all** — THE DELIVERABLE, and §3c is now the way to measure it (20 members, 148 symbols left) | **COLLISION HALF DONE for the twelve validated pairs** (§7b, two maps, 11 runs/arm, indistinguishable from stock) — but those runs were on maps with no cylinder traffic, so they do not cover the two new pairs. **SOLVER HALF NO LONGER BLOCKED, AND MEASURED: 2026-08-25 the engine EXECUTES the recovered `MdtKeaAddConstraintForces` 301 times and `keaLCPSolver::solveLCP` 85 times on `test-karma-1`** (§7d), with substitution verified at the machine-code level. `libMdtKea` is recovered whole. **What is left of this row is six shipped members, of which exactly one is on the path — `keaMatrix_PcSparse_vanilla`, ~one rounding step out (§4a)** — and the fact that the runs above show START MATCH and a short tick rather than 300 s of play, because the environment's renderer now faults at the first HUD frame **for stock too**. §7d. |

**So what is left, in one sentence each:**

- **One cylinder pair, unheld and unreleased — but not for the reason it looked like.**
  `IxCylinderCylinder`'s 925 `dims_diff` in 24,111 real calls is a feature LABEL on
  geometrically degenerate separating axes, and the shipped library re-rolls it whenever a
  body moves 1e-7 m. What is genuinely unaccounted for is **3 `count_diff` in 24,111**.
  Its sibling `IxCylinderTriList` was measured and released the same day, and is stable
  under the same probe. §3, §11 item 0.
- ~~**The solver's frames**~~ — **DONE. `libMdtKea` is recovered whole and the engine has
  run on it** (§7d). What replaces this row is **`keaMatrix_PcSparse_vanilla`**, the one
  still-shipped member on the path, about one rounding step out (§4a) — and the rounding
  lead of §11 item 2a is the first thing to try on it, since it carries the same
  declared-but-unused float local.
- **THE ASSOCIATION DEFECT, which is new and is the largest unquantified risk in the
  project.** Ghidra prints right-leaning float `+` chains flat; on x87 that is exactly inert
  and on wasm32/armv7/arm64 it differs in 31% of samples. Three sites fixed in one object of
  153, **the other 152 unexamined, and no gate here can see it.** §11 item 2a.
- ~~**qhull and the asset loader**~~ — **both done.** Qhull is replaced and validated at
  four tiers including a live match (§8a, §8b); the asset loader turned out to be
  RECOVERABLE and is 9 of 9 (§8c).
- **arm64** — compiles, and is measured to truncate pointers in 73 of 122 objects (2,680 sites).
  `test/ptrwidth_check.sh`, §6b. The fix is generator-wide, not a flag.
- **Executing anything on wasm** — the web agent's job. `HANDOVER-WEB.md`.
- **The tail** — 8 objects (was 12; the three dead-end-9 rows and `MeSimpleFile_linux` were
  recovered on 2026-08-25). **And it is no longer "nothing blocks anything":** five of the
  eight are in the drop-in gap — `McdSpace` 16 symbols, `MdtBcl` 15, `MeMath` 8,
  `MeProfile_linux` 6, `MeProfile` 4. §3c, §13.
- **The drop-in gap itself** — 20 shipped members, 148 symbols, grouped by cause in §3c.
  This is the goal; everything above is a route to it.


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

**THE SOLVER'S PLUMBING IS RECOVERED.** Every object in `libMdtKea` is **bit-identical to
the shipped library on all three scenes** — the driver (`keaRbdCore_unified`), the LCP
(`keaLCPSolver`, on all fifteen of its functions individually as well), `keaLCP_new` and the
allocator (`keaMemory`) — with the three compute kernels that already were. Each has a
positive control: put the driver's dispatches back on `code *` and step 0 reads 4.9e+14;
take the allocator's plausible wrong repair and it segfaults; drop `keaLCP_new`'s three
restored arguments again and step 0 reads 7.8e-03.

**One object is left, and it is a single last bit.** `keaIntegrate_pc` reads
`first@13 = 1.500e-08`, and the engine calls `MdtKeaIntegrateSystem` directly, so it is
required. Until it is right there is still **no configuration in which the engine runs on
recovered kea** — but the distance is one last bit, not four objects and an unreadable
frame.

The frame problem is down to **one `stack0x` each in `MdtBcl` and `MeMath`**, neither on
anybody's path. §11 item 2.

**Never executed on wasm.** 122/122 compile with byte-identical exported symbols. Not one
instruction has run. See `HANDOVER-WEB.md`.

**Run end to end, now, for the collision layer** — §7b. Not for the solver.

So the honest summary is: **the collision layer is proven and drives a real match; the
solver's arithmetic is proven and cannot yet be reached; the solver's control flow is
untouched.** Do not read 98/148 as 66% of the way there — the denominator is wrong in both
directions (a third of those objects are for collisions the game never makes, and the two
kea objects left in §11 are worth more than the whole rest of the tail put together).

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
- **Is the test drawing from the distribution the game draws from?** (Added 2026-08-24,
  fifth session. `difftest_pair` generated only random axis-angle rotations for its whole
  life. `IxCylinderCylinder` read 0.01% divergence there and 3.8% in a live match, and the
  difference was entirely that the game's cylinders are AXIS-ALIGNED. `KD_GRID=1` closes
  the gap to 12%. This is one level up from "is any input fixed that the game varies?" —
  the value was varying, and varying wrongly.)
- **Can the ORIGINAL reproduce the thing you are asking the recovery to reproduce?**
  (Added 2026-08-24, fifth session. `KD_JITTER` runs the shipped library against itself on
  inputs differing by 1e-7 m. For `McdCylinderCylinderIntersect`'s `dims` it cannot — it
  disagrees with itself 7,798 times where our recovery disagrees 6,432. Every other pair in
  the driver reads 0, so the probe discriminates. A gate that demands more consistency than
  the reference has is measuring the wrong thing.)
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

## 13. The 8 remaining failures, triaged

Written so the next session starts from a decision, not from re-deriving one. Counts are
error counts as of 2026-08-25 (second session). **"DEAD" means `tools/reachable.py` proves
nothing in the engine can reach it — §3b — so it is out of scope, not a to-do.**

**`MdtWorld` and then `keaRbdCore_unified` left this table by being RECOVERED**, the second
of them from 18 errors on 2026-08-25. Note that on the way it passed THROUGH this bucket by
getting better: `recover.py` classifies by the FIRST error's pattern, so an object can move
`review → FAIL` while improving. Sort by the verdict column, never by the bucket.

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
| `MdtBcl` | live | **1** | **31 → 1** on 2026-08-24: 22 errors were `FUN_00021130`, which is `.eh_frame` decompiled as code, and 2 more were `halt_baddata`-only functions (see below). The one left is `&stack0xfffffff4` used as a base to read a locals block at −0x80…−0x68 — and it is Ghidra rendering the function's **epilogue** (`lea -0xc(%ebp),%esp`) as a pointer assignment. §11 item 2's family. Cheapest object in the tail, but **NOT a validation subject for §11 item 2** — its single error is an epilogue rendered as a pointer assignment, a different shape from `MdtWorld`'s outgoing by-value marshalling, so a fix for one will not show up here |
| `MeMath` | live | **1** | one `stack0x` at line 495, genuine frame loss rather than a name: Ghidra also dropped the `fcos`/`fsin` results that build the matrix being read. Same family as `MdtBcl` and `MdtWorld` |
| `MeProfile_linux` | live | 13 | open, **low value** (§3b puts it under platform/misc) — and §13 used to undersell it as "an `rdtsc` shim". It needs that, plus `struct timeval` as a bare tag, plus the §5 EXTERNAL-slot collision (`_select` and `_clockSpeed` are neighbours of `frameTime`, not real symbols). Four families for a profiler timer |
| `MeProfile` | live | **29** | open, **low value** — 11 `request for member`, 5 `weightingData` (§11 item 7: a variable, not a type), 5 of the exported-DATA rename gap (`frameTime`/`clockSpeed` vs `kd_*`; dead end 10), 2 `__divdi3`/`__udivdi3` |

**Recovered on 2026-08-24 (third session): `McdContact`, `McdMessage`, `McdBatch`,
`mesffnmin`** — 17 failures down to 14, 106 objects up to 108, and **271 errors across the
14**. `McdContact` compiles but is now held by the `extraout_EAX` detector, which is a
different and real defect: that is a **reclassification**, not a release.

**FOUR MORE LEFT THIS TABLE ON 2026-08-25 and the count is 8, not 12.** `McdBox`,
`McdSphyl` and `McdTriangleList` — the three dead-end-9 rows — were all recovered by one
generator fix once dead end 9 was re-diagnosed (§9), and `MeSimpleFile_linux` by another.
So the rows below marked "dead end 9" and the `MeSimpleFile_linux` leave-alone are
DISCHARGED; they are kept because the reasoning that held them is what had to be corrected.

Of the 8 that remain: 3 are DEAD, 1 is a documented leave-alone (`McdSpace`), 2 are the
§11 item 2 frame problem (`MdtBcl`, `MeMath`) and 2 are low-value profilers. **But
"nothing in it blocks anything" is NO LONGER TRUE** — §3c changed how this is read.
`McdSpace` (16 symbols), `MdtBcl` (15), `MeMath` (8), `MeProfile_linux` (6) and `MeProfile`
(4) are all in the DROP-IN GAP, so five of the eight are on the path to the goal after all.
Sort by §3c's symbol count now, not by the error count and not by this table's order.

**And note what is NOT in this table but is still shipped.** Six `libMdtKea` members never
reach the FAIL bucket because they are held earlier or have no functions:
`keaStuff` and `version` (no functions), `keaDebug` and `keaPrintBasicTypes`
(`extraout_EDX`), `ReadWriteKeaInputToFile` (a mislabelled `_gDebug`), and
**`keaMatrix_PcSparse_vanilla`**, which is quarantined on a guessed frame and is the only
one of the six the solver actually calls. It is WHAT REMAINS row 9, not a tail object.

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
