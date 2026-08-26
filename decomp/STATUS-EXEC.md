# Karma decompilation — status

**What we are building:** UT2004's physics engine, rebuilt from the shipped binaries, so we
can drop it in and give the **web and Android** versions vehicles and ragdolls. Today they
have no physics at all. Updated 2026-08-26.

---

## The one number

**8.**

That is how many pieces of the original library the game still needs from us. It was 27 at
the start of last week and 20 yesterday. When it reaches zero and the tests stay green, we
ship.

We deliberately do not count "modules rebuilt" — most of the original library is code
UT2004 never touches, so rebuilding it would be zero progress.

---

## Where we are

| | |
|---|---|
| **Original pieces still needed** | **8** (was 20 yesterday, 27 last week) — and **5 of the 8 are pieces we have decided not to rebuild**, for reasons on the record |
| Collision — does A hit B | ✅ done, proven in live matches |
| Movement — how things fall and swing | ✅ done, and the game runs on it |
| Loading vehicle and ragdoll files | ✅ done this session, and the game runs on it |
| Runs on PC | ✅ |
| Runs on web / Android | ❌ **never tried — not once** |

---

## This session

**The gap fell by more than three quarters.** Not by rebuilding more modules, but by fixing
twenty faults in the *translator* — each one repaired several modules at once, including
some nobody was looking at. The three largest pieces the game still needed are all done,
and the whole file-loading path for vehicles and ragdolls now works.

**We built an instrument that answers a question we had been guessing at.** The translator
often reports "this value came from somewhere I could not follow", and until now that held
a module back on suspicion. We now settle it by asking the compiler: build the module twice
with that value set to two different things, and if the machine code is identical the value
cannot matter. Six modules were released that way, on proof rather than on judgement.

**And it caught us out once, exactly as designed.** One module passed all nine offline
checks and then crashed the game on start-up. The cause was a table of function addresses
written as an address into a *copy of the code* rather than the code itself — the third time
this same fault has appeared, and the third time only running the real game found it.

**We also found a module that had been quietly wrong while passing every test.** It read an
asset's parts out of a three-slot scratch area instead of the real one, so anything with
more than three parts — every ragdoll — read the wrong memory. It happened to be in a
function the game never calls, which is luck, not design; we say so on the record.

---

## The two risks

**1. We cannot yet test the thing we are building it for.** Nothing has ever run on web or
Android. It compiles for both — 137 of 137 modules, which is newly true and was overstated
before — but that is not the same thing.

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
passing test is not believed until we have shown it could have failed. That caught five of
our own checks this session — including one portability check reporting a clean result for
a target it was not building at all, and one interface check that let three renamed exports
through while its entire job was to catch exactly that. Both are fixed, and both now have a
test that deliberately breaks them.
