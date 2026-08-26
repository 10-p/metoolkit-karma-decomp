# Karma decompilation — status

**What this is:** UT2004's physics engine, rebuilt from the shipped binaries so the **web
and Android** versions can have vehicles and ragdolls. Today they have no physics at all.
Updated 2026-08-26.

---

## The one number

**8.**

Pieces of the original library the game still needs from us. It was 27 at the start of last
week and 20 yesterday.

**And 5 of the 8 we have decided not to rebuild** — pieces the game links but never calls,
or where rebuilding would mean guessing. Those decisions are on the record. So the real
remaining work is **3 pieces**.

---

## Where we are

| | |
|---|---|
| **Pieces still needed** | **8** — of which 5 are deliberately not being rebuilt |
| Collision — does A hit B | done, proven in live matches |
| Movement — how things fall and swing | done, and the game runs on it |
| Loading vehicle and ragdoll files | done this week, and the game runs on it |
| Runs on PC | yes |
| Runs on web / Android | **never tried — not once** |

---

## This week

**The gap fell by more than three quarters.** Not by rebuilding more pieces — by fixing
twenty faults in our *translator*, each of which repaired several pieces at once, including
some nobody was looking at.

**We built a tool that answers a question we had been judging by eye.** The translator often
says "this value came from somewhere I could not follow". We now settle it by asking the
compiler: build the piece twice with that value set differently, and if the machine code is
identical, the value cannot matter.

---

## The two risks

**1. We cannot yet test the thing we are building it for.** Nothing has ever run on web or
Android. It compiles for both, which is not the same thing.

**2. An error that is harmless on PC and harmful on web.** A quirk in how the decompiler
writes arithmetic changes the answer only on machines without the PC's spare precision —
exactly web and Android. Zero difference in two million PC tests, 31% on the target. We
fixed it in one piece of 153, and **no PC test can see the rest.**

Neither is a reason to slow down. Both are reasons the next milestone is *running* on web,
not rebuilding more.

---

## Confidence

Every claim traces to a recorded measurement that can be re-run. The working rule is that a
passing test is not believed until we have shown it could have failed — which this week
caught **five of our own checks**, including two that were reporting success on things they
were incapable of seeing.
