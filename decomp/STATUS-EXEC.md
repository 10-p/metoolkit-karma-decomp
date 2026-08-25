# Karma decompilation — executive summary

**What this is:** a plain-language status report on rebuilding UT2004's physics engine
from the shipped binaries, so that it can run on platforms where it currently cannot.
No technical background assumed. Updated 2026-08-25.

The two companion documents are for engineers: `HANDOVER.md` (how everything works and
what to do next) and `proven.txt` (the evidence ledger — what has been tested, on what,
with what result).

---

## The one-paragraph version

UT2004's physics is a third-party library called **Karma**. Epic shipped it as machine
code only — there is no source, and there never will be. That is why the web and Android
builds of the game ship with **no vehicle physics and no ragdolls**. We are reconstructing
Karma as ordinary portable source code by reading the shipped binaries.

**The collision half is essentially finished and has been proven inside the real game.**
The engine has played full matches driving on our reconstructed code, indistinguishable
from stock. **The movement half — the "solver" — is not finished:** its mathematics is
proven exact, and the control code around it is not yet rebuilt, so there is no
configuration in which the game runs on our solver. Nothing has run on web or Android
yet; it compiles for both, which is not the same thing.

---

## Progress at a glance

| | count | |
|---|---:|---|
| Modules in the shipped library | **192** | the whole product |
| Proven never used by UT2004 | **31** | a demo viewer, unused joint types — permanently out of scope |
| **Our working set** | **153** | the modules extracted and analysed |
| **Rebuilt, checked and in use** | **110** | up from 109 last week |
| Rebuilt but **held back pending proof** | **16** | they work; we have not proven they are right, so they stay out |
| **Not yet rebuilt** | **22** | see "what is left" below |
| Contain no code (data only) | **5** | |

So **126 of 153 modules rebuild**, and we allow **110** of those into the product. The
gap between those two numbers is deliberate: a module that compiles is a hypothesis, not
a result, and this project has twice had one that looked finished and corrupted memory in
a real match.

**Please do not read 110/153 as "72% done."** It is the number we have, and it is
misleading in both directions. Some of the 22 remaining modules are worth more than
thirty of the finished ones, and some finished ones are for situations the game never
creates. (The 192 and the 153 also do not subtract cleanly — some archive members are
duplicated across archives or hold data rather than code.) **The functional view below is
the one to judge by.**

---

## The functional view — what actually matters

### Collision detection: does object A touch object B, and where?

The game asks Karma this question about specific *pairs* of shapes — a sphere against
terrain, a capsule against a vehicle hull, and so on. **37 such pairs exist. The game
actually uses 15 of them** (we measured this by instrumenting real matches across 20+
maps; the other 22 either never occur or are handled by the engine itself).

| status | count | |
|---|---:|---|
| ✅ Working and proven against the original in live matches | **13** | including every high-traffic pair |
| ⚠ Working with a small measured defect | **1** | cylinder vs terrain — see below |
| ⛔ Not yet resolved | **1** | cylinder vs cylinder — see below |

Scale of the proof: individual pairs have been checked over **1.7 million real calls** in
running matches with zero disagreements, and the whole set has been checked over millions
more synthetic cases.

**Cylinder vs terrain (⚠).** Released last week on a 5-minute sample that showed zero
problems. Longer runs show it disagrees with the original roughly **1 time in 4,000** —
it reports one extra contact point where the original reports none. The rate is in the
same band as two other pairs we already accepted, and the effect is small, but the "zero"
we published was a sample-size artefact and has been corrected in the ledger.

**Cylinder vs cylinder (⛔).** Disagrees with the original on about 3% of calls — but
only about a *label* attached to the contact, not its position or force. We established
two things this week: the game **never reads that label** for this pair, and the original
library **does not reproduce it either** (nudge one object by a ten-millionth of a metre
and the shipped code changes its own answer more often than we differ from it). What
genuinely remains is a much smaller disagreement — roughly **1 call in 10,000** — about
the number of contact points. It is not released.

### The solver: how things actually move

This is the part that turns "these two objects are touching" into "so they bounce like
this." It is one library, and we split it into arithmetic and plumbing.

| | status |
|---|---|
| **The arithmetic** — the numerical core that computes forces | ✅ **Proven exact.** Reproduces the original bit-for-bit over 900 consecutive simulation steps of a chaotic scene, on a test sensitive enough to catch an error of one part in a million. |
| **The plumbing** — the code that feeds the arithmetic and applies its results | ⏳ **5 modules short.** Four do not yet rebuild; the fifth does and is measured to be slightly wrong, so it is held back. |

Until the plumbing is done there is **no configuration in which the game runs on our
solver**, however good the arithmetic is. This is the single largest remaining item.

### Platforms

| target | builds? | runs? |
|---|---|---|
| PC / Linux, 32-bit (the original) | ✅ 110/110 | ✅ **plays full matches** |
| Web (WebAssembly) | ✅ 110/110 | ❌ not once |
| Android, 32-bit | ✅ 110/110 | ❌ not once |
| Android, 64-bit | ✅ 110/110 | ⛔ **do not trust** |

The 64-bit Android build is the one to watch. It compiles and looks identical to the
others by every surface measure, and it is **wrong**: the reconstructed code stores
memory addresses in slots that are half the size they need to be on 64-bit. We added a
check this week that counts the problem precisely — **2,218 occurrences across 66 of the
110 modules**, against **zero** on 32-bit. Fixing it is a broad change, not a switch.

---

## Milestones — the seven things that mean "done"

| # | milestone | status |
|---|---|---|
| 1 | Every collision pair the game uses is rebuilt and proven | **13 of 15** |
| 2 | "Proven" means measured against the original in a live match | ✅ standard established and applied |
| 3 | Every rebuilt module runs cleanly in test scenes, and the tests are shown to be capable of catching errors | ✅ |
| 4 | Two large components replaced rather than reconstructed | ✅ **done** — the hull builder shrank from 1.4 MB to 10 KB and is validated four ways; the asset loader turned out to be reconstructable, which is a better outcome |
| 5 | Nothing accepted without evidence on the record | ✅ holding — 16 working modules are still held out of the product for want of proof |
| 6 | Builds for web and Android as well as PC | ⚠ web and 32-bit Android done; **64-bit Android not trustworthy** |
| 7 | The game runs with **no original Karma code at all** | ⏳ **collision half done** (full matches played), solver half blocked on item 1 above |

---

## What is left, in priority order

| # | item | size | blocked on |
|---|---|---|---|
| 1 | **The solver's plumbing** — 5 modules. Without these the game cannot run on our solver. | large | nothing external; two approaches tried and refuted this week, a third is working |
| 2 | **64-bit Android correctness** — 2,218 issues across 66 modules | medium | a broad change to how addresses are stored |
| 3 | **Nothing has ever executed on web or Android.** Compiling is not running. | medium | a separate workstream (the web team) |
| 4 | **Cylinder vs terrain** — 1 disagreement in 4,000 | small | reproducing it outside a live match, which we cannot yet do |
| 5 | **Cylinder vs cylinder** — roughly 1 disagreement in 10,000 | small | — |
| 6 | The other unbuilt modules | small | of the 22, three are unreachable, two are deliberate leave-alones, three hit a known dead end, two are low-value profiling code, and the rest are items 1 and 4–5 above — **there is nothing cheap left here** |

---

## Movement this week

- **+1 module rebuilt** (109 → 110), and more importantly the *method* for the solver's
  plumbing was found and validated. Two plausible approaches were tried and disproven
  first; the third reconstructs code that matches the game's own source line for line.
- **Cylinder vs cylinder was re-diagnosed** from "wrong and in the build" to "the
  disputed value is one nothing reads, and the original does not reproduce it either."
- **A defect was found in something we had already signed off** (cylinder vs terrain),
  by running the test longer. The record was corrected.
- **A test switch that had been documented for months turned out never to have been
  implemented** — every run that used it was silently doing something else. Now fixed.
- **Three new test regimes added**, all modelling situations the game creates and our
  tests never did. All the accepted modules pass them unchanged, which is reassurance
  rather than a finding.
- **The 64-bit Android problem is now measurable** rather than merely suspected.

---

## How much to trust this

The project's working rule is that **a green result is not believed until we have shown
the test could have gone red**. That sounds pedantic and it has repeatedly been the
difference between real progress and imaginary progress. Concretely, this week alone:

- a test switch that did nothing while appearing to work;
- a "zero defects" result that was a sample size, not a property;
- a repair that compiled cleanly and would have corrupted memory, caught because we
  deliberately kept it flagged rather than letting it look finished;
- a compiler warning check that reported "all clear" on the one platform we already
  knew was broken, because of a flag ordering mistake.

Every claim in this document traces to a measurement recorded in `proven.txt` with the
numbers attached, so any of it can be re-checked rather than taken on trust.

**The honest summary:** the collision half is finished to a standard we can defend and has
driven real matches. The solver's mathematics is finished to a higher standard than that.
The solver's plumbing is the project. Web and Android are compile-clean and unexercised,
and 64-bit Android has a known defect that no amount of testing on PC will reveal.
