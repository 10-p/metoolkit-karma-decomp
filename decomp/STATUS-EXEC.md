# Karma decompilation — status

**What this is:** UT2004's physics engine, rebuilt from the shipped binaries so the **web
and Android** versions can have vehicles and ragdolls. Today they have no physics at all.
Updated 2026-08-26.

---

## The one number

**4.**

Pieces of the original library the game still needs from us. It was 27 at the start of last
week, 20 the day before yesterday, and 8 yesterday.

**And 2 of the 4 we have decided not to rebuild** — pieces the game links but never calls,
or where rebuilding would mean guessing. The other 2 we attempted today and deliberately
did not ship. So the real remaining work is **2 pieces**, and both are documented down to
the machine instruction.

**And one of the two we had written off is now half solved.** A rotation
calculation the decompiler had thrown away was rebuilt and checked against the
original on **a million random inputs — identical every time**. The catch is
that its twin, which no check flags at all, is wrong on *every* input; shipping
the fixed half would quietly ship the broken one, so neither goes in until both
are done.

Counted in the units that matter — the individual functions the game still needs from the
original — it went from **35 to 11 today**, and 9 of the 11 are ones we have decided
against.

---

## Where we are

| | |
|---|---|
| **Pieces still needed** | **4** — of which 2 are refused and 2 are attempted-and-held |
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

**Today: four more pieces, and two deliberately not shipped.** The largest of the four had
been parked for two sessions behind a reason that turned out to be true but irrelevant —
we thought we needed information nobody has, and we never needed it. One wrong line in a
lookup table was the whole blocker, and it accounted for **16 of the 35** functions
outstanding this morning. With the third, the
**solver library is complete** — every piece of the maths engine that computes how bodies
push on each other is now ours, and the game runs a full match on it. One piece the translator had
described two contradictory ways at once; the contradiction was settled by reading the
original machine code. Another had two scratch buffers where the translator modelled one, so
what it produced would have written past the end of a four-byte slot — it had been held back
by a safety check, correctly. Two others were rebuilt, looked clean on every check, and were
**not shipped**: one would have corrupted memory in a way none of our checks can see. The
reasoning is on the record so the next attempt starts from the answer.

**And we caught ourselves misreading our own test.** One summary line reports "ran without
crashing"; we read it as "produces identical results". A sharper tool showed one piece did
not — though the parts the game actually uses are identical, which is why the decision
stands. The correction is written down more prominently than the original claim.

---

## Android got most of the way this week

**The Android 64-bit build had a defect in 7,771 places, and 95% of it is now fixed** —
down to 363. The rebuilt code passes addresses around in slots that are the right size on
web and on 32-bit Android and **too small on 64-bit Android**, where they would be silently
cut in half. It was found by asking the Android compiler, which already knew; it is fixed by
asking the same compiler to point at every site rather than by guessing at them.

The PC build is unchanged **to the byte** — all 139 pieces compile to identical machine
code — so nothing that already worked could have moved.

What is left is 363 places of a different kind, which need judgement one at a time rather
than a sweep: some of them would be made *worse* by the same fix.

---

## The two risks

**1. We cannot yet test the thing we are building it for.** Nothing has ever run on web or
Android. It compiles for both, which is not the same thing — and the Android fix above is a
step towards *correct*, still not towards *tested*.

**2. An error that is harmless on PC and harmful on web — now counted, for the first time.**
A quirk in how the decompiler writes arithmetic changes the answer only on machines without
the PC's spare precision — exactly web and Android. Zero difference in two million PC tests,
31% on the target.

We have now measured how much of it there could be: **575 places, in 51 of the pieces**, of
which we can rank **59 as most likely** because they match the one case where we have the
original code to compare against. That is the difference between "we don't know" and "here
is the list".

**No PC test can settle any of them** — on a PC the error is exactly invisible, so we cannot
even prove a fix works. The one thing that would settle all 575 at once is running the same
scene on web and on PC and comparing. That is now the highest-value item on the web side.

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
