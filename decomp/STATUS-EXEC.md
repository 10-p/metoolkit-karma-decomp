# Karma decompilation — status

Rebuilding UT2004's physics engine from the shipped binaries, so the web and Android
builds can have vehicles and ragdolls instead of nothing. Updated 2026-08-25.

---

## Where we are

**Collision detection — does A hit B — is done.** The game plays full matches on our
rebuilt code, indistinguishable from the original.

**Movement — how things fall, bounce and swing — is not.** The mathematics is finished
and proven exact. The control code around it was five modules short a week ago; it is now
**two**, with nine known problems between them. There is still **no configuration in which
the game runs on our physics engine.** That is the project, and it is much closer.

**Nothing has run on web or Android yet.** It compiles for both — not the same thing —
and one of the two Android targets is known to be wrong.

---

## Numbers

| | |
|---|---|
| Modules rebuilt and in use | **112** of a 153-module working set |
| Rebuilt but held back until proven | 16 |
| Not yet rebuilt | 20 |
| Movement modules still missing | **2** — was 5 |
| Collision types the game uses | **15** — 13 proven, 1 small defect, 1 open |
| Platforms building | PC ✅ · Web ✅ · Android 32-bit ✅ · Android 64-bit ⚠ |
| Platforms **running** | PC only |

Module count is a poor progress measure — a few of the 20 remaining are worth more than
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
- The memory allocator and the equation solver, both rebuilt this week, both checked by
  building the wrong version on purpose and confirming it fails.

## Left

| | size | |
|---|---|---|
| **The solver's control code** — 2 modules, 9 known problems | large | the whole remaining project |
| **64-bit Android is wrong** — 2,291 known issues | medium | compiles and looks fine; it is not |
| **Nothing has executed on web or Android** | medium | separate workstream |
| Two small collision defects (~1 in 4,000, ~1 in 10,000) | small | |
| The remaining modules | small | nothing cheap left |

---

## This week

**The movement half went from five missing modules to two.** Two were rebuilt — the memory
allocator and the equation solver — and the remaining three shrank from 29 known problems
to 9. One of the two had been blocked for months on a repair nobody could check; we built
the check first, then made the repair, and the check catches both ways of getting it wrong.

**The part worth noting is a repair we did not make.** The obvious fix for the allocator
would have compiled cleanly and been wrong — it would have read a number out of memory the
original never touches. We built it anyway, as a control, and it crashed immediately, which
is how we know the fix we did take is right rather than merely plausible.

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
