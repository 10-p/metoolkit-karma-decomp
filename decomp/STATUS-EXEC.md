# Karma decompilation — status

**What this is:** UT2004's physics engine, rebuilt from the shipped binaries so the **web
and Android** versions can have vehicles and ragdolls. Today they have no physics at all.
Updated 2026-08-26.

---

## The one number

**6.**

Pieces of the original library the game still needs from us. It was 27 at the start of last
week, 20 the day before yesterday, and 8 yesterday.

**And 3 of the 6 we have decided not to rebuild** — pieces the game links but never calls,
or where rebuilding would mean guessing. Those decisions are on the record. So the real
remaining work is **3 pieces**.

---

## Where we are

| | |
|---|---|
| **Pieces still needed** | **6** — of which 3 are deliberately not being rebuilt |
| Collision — does A hit B | done, proven in live matches |
| Movement — how things fall and swing | done, and the game runs on it |
| Loading vehicle and ragdoll files | done this week, and the game runs on it |
| Runs on PC | yes |
| Runs on web / Android | **never tried — not once** |

---

## This week

**The gap fell by more than three quarters.** Not by rebuilding more pieces — by fixing
faults in our *translator*, each of which repaired several pieces at once, including some
nobody was looking at.

**We built a tool that answers a question we had been judging by eye.** The translator often
says "this value came from somewhere I could not follow". We now settle it by asking the
compiler: build the piece twice with that value set differently, and if the machine code is
identical, the value cannot matter.

**Today: two more pieces, and one deliberately not shipped.** One piece the translator had
described two contradictory ways at once; the contradiction was settled by reading the
original machine code. Another had two scratch buffers where the translator modelled one, so
what it produced would have written past the end of a four-byte slot — it had been held back
by a safety check, correctly. A third was rebuilt, looked clean on every check, and was
**reverted**: it would have corrupted memory in a way none of our checks can see. The
reasoning is on the record so the next attempt starts from the answer.

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

It also caught one of *today's own repairs* that passed every check and was still wrong —
which is why that piece was reverted rather than shipped. **A check that cannot fail is not
evidence**, and the first version of one of today's fixes did nothing at all while passing
every test written for it.
