# Karma decompilation — status

**What we are building:** UT2004's physics engine, rebuilt from the shipped binaries, so we
can drop it in and give the **web and Android** versions vehicles and ragdolls. Today they
have no physics at all. Updated 2026-08-25.

---

## The one number

**20.**

That is how many pieces of the original library the game still needs from us. It was 27 at
the start of the week. When it reaches zero and the tests stay green, we ship.

We deliberately do not count "modules rebuilt" — most of the original library is code
UT2004 never touches, so rebuilding it would be zero progress.

---

## Where we are

| | |
|---|---|
| **Original pieces still needed** | **20** (was 27) |
| Collision — does A hit B | ✅ done, proven in live matches |
| Movement — how things fall and swing | ✅ done, and the game runs on it |
| Runs on PC | ✅ |
| Runs on web / Android | ❌ **never tried — not once** |

---

## This week

**The movement half finished.** The last module was out by one part in a hundred million;
it is now exact, and the game has been rebuilt on our version and played.

**We built the measure above.** Progress used to be counted in modules rebuilt, which never
answered "how close are we". The new count traces every function the game calls through to
whoever provides it. We checked it against reality rather than trusting it: we deleted all
the original pieces, tried to build the game, and confirmed it had predicted all 111 things
that went missing.

**Seven of the twenty-seven were closed** — and one correction alone accounted for three of
them, plus **four modules that had been passing every test while computing the wrong
answer.** One was working out a sphere's weight from its radius read as a whole number.

---

## The two risks

**1. We cannot yet test the thing we are building it for.** Nothing has ever run on web or
Android. It compiles for both, which is not the same thing.

**2. There is an error we can prove is harmless on PC and harmful on web.** A quirk in how
the decompiler writes arithmetic changes the answer only on machines without the PC's spare
precision — which is exactly web and Android. Measured: zero difference in two million
tests on PC, a 31% difference on the target. We fixed three cases in one module; there are
152 other modules and **no PC test can see this**. It would show up as web physics quietly
drifting away from PC physics.

Neither is a reason to slow down. Both are reasons the next milestone is *running* on web,
not rebuilding more.

---

## Confidence

Every claim traces to a recorded measurement that can be re-run. The working rule is that a
passing test is not believed until we have shown it could have failed. Four times this week
that caught our own tests — including a module that passed all nine checks and then killed
the game on start-up.
