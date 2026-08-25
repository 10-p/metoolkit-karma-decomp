# Karma decompilation — status

Rebuilding UT2004's physics engine from the shipped binaries, so the web and Android
builds can have vehicles and ragdolls instead of nothing. Updated 2026-08-25.

---

## Where we are

**Collision detection — does A hit B — is done.** The game plays full matches on our
rebuilt code, indistinguishable from the original.

**Movement — how things fall, bounce and swing — is one module away.** The mathematics was
already proven exact. This week the four modules around it were rebuilt — the driver, the
equation solver, its inner loop and the memory allocator — and **all four reproduce the
original exactly, step for step, on all three test scenes**. The control code was five
modules short a week ago.

The one that remains is not missing: it is rebuilt and runs, and disagrees with the
original by **one part in a hundred million** on the first step where it differs at all —
a single rounding step, not a fault. Until that is settled there is still no configuration
in which the game runs on our physics engine, and it is the only reason.

**Nothing has run on web or Android yet.** It compiles for both — not the same thing —
and one of the two Android targets is known to be wrong.

---

## Numbers

| | |
|---|---|
| Modules rebuilt and in use | **113** of a 153-module working set |
| Rebuilt but held back until proven | 17 |
| Not yet rebuilt | 18 |
| Movement modules rebuilt exactly | **6 of 7** — was 2 of 7 a week ago |
| Collision types the game uses | **15** — 13 proven, 1 small defect, 1 open |
| Platforms building | PC ✅ · Web ✅ · Android 32-bit ✅ · Android 64-bit ⚠ |
| Platforms **running** | PC only |

Module count is a poor progress measure — a few of the 18 remaining are worth more than
thirty finished ones. Judge by the "movement modules" row.

---

## Done

- Collision detection for every shape pair the game actually uses, measured against the
  original inside live matches — one pair verified over 1.7 million calls, zero
  disagreements.
- The engine playing full matches on our code, indistinguishable from stock.
- The solver's mathematics: reproduces the original **exactly** over 900 consecutive
  simulation steps, on a test sensitive to one part in a million.
- Two large components dealt with — the hull builder replaced (1.4 MB of third-party code
  down to 10 KB) and the asset loader rebuilt.
- Builds for web and 32-bit Android with an interface identical to the original.
- **The solver's driver, its equation solver, that solver's inner loop and its memory
  allocator** — rebuilt this week, all four reproducing the original exactly on all three
  test scenes. Each checked by building a wrong version on purpose and confirming it fails.

## Left

| | size | |
|---|---|---|
| **One movement module, out by one part in 100 million** | small | the last thing between us and a running physics engine |
| **64-bit Android is wrong** — 2,291 known issues | medium | compiles and looks fine; it is not |
| **Nothing has executed on web or Android** | medium | separate workstream |
| Two small collision defects (~1 in 4,000, ~1 in 10,000) | small | both now reproducible in seconds on a desk, which they were not |
| The remaining modules | small | nothing cheap left |

---

## This week

**The movement half went from five missing modules to none missing and one imperfect.**
Four were rebuilt — the driver, the equation solver, its inner loop and the memory
allocator — and every one of them reproduces the original exactly. The driver had been
blocked for months on a repair nobody could check; we built the check first, then made the
repair, and the check catches both ways of getting it wrong.

The last of the four had been marked "do not attempt" twice, on the grounds that its memory
layout was inconsistent. Looking again, it was not — the note had misread one line of it.
The layout was recoverable, and once it was, a single call turned out to be missing its
three arguments. Putting them back made the module exact.

**The part worth noting is a measurement we had been reading wrong.** One module looked
acceptable for a whole session because we were judging it by its WORST disagreement with
the original over 900 simulation steps — and in a scene with collisions, everything drifts
apart eventually, so that number tells you almost nothing. Judged by the FIRST step it
disagrees on, the picture inverted: modules we had signed off differ by about a millionth
of a millimetre and grow from there, while this one put a body 3.7 metres out of place
immediately.

That module is now exact, and the sequence is the point. The measurement held it back; a
new tool then narrowed it from fifteen functions to one by putting a single rebuilt function
at a time into an otherwise-original build; and the fault turned out to be three lines where
a decimal number was being stored as a whole number — which destroys the value, and which
no compiler warns about. **Three lines, in a module of nine hundred.**

We also finished the job on two long-standing collision defects, in the sense that matters
most: both now reproduce on a desk in seconds. One of them had only ever been seen inside a
live match, and the test rig could not produce the situation it needed — a cylinder resting
in a corner — until we built a corner for it.

And: the obvious fix for the allocator would have compiled cleanly and been wrong. We built
it anyway, as a control, and it crashed immediately — which is how we know the fix we did
take is right rather than merely plausible.

Earlier in the week: two dead ends disproven, one module recovered, and three corrections
to things already marked done — a signed-off component with a defect visible only in longer
runs, a test switch documented for months and never implemented, and a wrong measurement of
the 64-bit Android problem. **All found by re-testing what was already green** — deliberate
practice, not bad luck.

---

## Confidence

Every claim above traces to a recorded measurement that can be re-run. The working rule is
that a passing test is not believed until we have shown it could have failed, which has
repeatedly been the difference between real and imaginary progress.

**The half that decides whether things touch is finished and proven. The half that decides
how they move is the work that remains.**
