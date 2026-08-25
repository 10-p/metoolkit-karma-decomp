# Karma decompilation — status

Rebuilding UT2004's physics engine from the shipped binaries, so the web and Android
builds can have vehicles and ragdolls instead of nothing. Updated 2026-08-25.

---

## Where we are

**Collision detection — does A hit B — is done.** The game plays full matches on our
rebuilt code, indistinguishable from the original.

**Movement — how things fall, bounce and swing — is not.** The mathematics is finished
and proven exact. The control code around it is five modules short, so there is currently
**no configuration in which the game runs on our physics engine.** That is the project.

**Nothing has run on web or Android yet.** It compiles for both — not the same thing —
and one of the two Android targets is known to be wrong.

---

## Numbers

| | |
|---|---|
| Modules rebuilt and in use | **110** of a 153-module working set |
| Rebuilt but held back until proven | 16 |
| Not yet rebuilt | 22 |
| Collision types the game uses | **15** — 13 proven, 1 small defect, 1 open |
| Platforms building | PC ✅ · Web ✅ · Android 32-bit ✅ · Android 64-bit ⚠ |
| Platforms **running** | PC only |

Module count is a poor progress measure — a few of the 22 remaining are worth more than
thirty finished ones. Judge by the two rows above it.

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

## Left

| | size | |
|---|---|---|
| **The solver's control code** — 5 modules | large | the whole remaining project |
| **64-bit Android is wrong** — 2,218 known issues | medium | compiles and looks fine; it is not |
| **Nothing has executed on web or Android** | medium | separate workstream |
| Two small collision defects (~1 in 4,000, ~1 in 10,000) | small | |
| The remaining modules | small | nothing cheap left |

---

## This week

Two dead ends disproven and a third approach found that works — it rebuilds code matching
the game's own source line for line. One module recovered. One item closed: a
never-before-tested code path in the busiest part of the physics, now clean.

Three corrections, which is the part worth noting. A component we had signed off turned
out to have a small defect visible only in longer runs. A test switch documented for
months had never actually been implemented. And our measurement of the 64-bit Android
problem was wrong, so we built a proper check for it. **All three were found by re-testing
things already marked done** — deliberate practice, not bad luck.

---

## Confidence

Every claim above traces to a recorded measurement that can be re-run. The working rule is
that a passing test is not believed until we have shown it could have failed, which has
repeatedly been the difference between real and imaginary progress.

**The half that decides whether things touch is finished and proven. The half that decides
how they move is the work that remains.**
