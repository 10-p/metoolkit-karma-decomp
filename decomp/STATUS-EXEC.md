# Karma decompilation — status

**What this is:** UT2004's physics engine, rebuilt from the shipped binaries so the **web
and Android** versions can have vehicles and ragdolls. Today they have no physics at all.
Updated 2026-08-26.

---

## The one number

# 3

Pieces of the original library the game still needs from us — and each is now a **single
function**. It was 27 at the start of last week, 20 on Monday, and **35 individual
functions this morning**.

---

## Where we are

| | |
|---|---|
| **Still needed from the original** | **3 functions** |
| Collision — does A hit B | done, proven in live matches |
| Movement — how things fall and swing | done, and the game runs on it |
| Loading vehicle and ragdoll files | done, and the game runs on it |
| Runs on PC | yes — a full match, no crashes, matching the original |
| Runs on web / Android | **never tried — not once** |

---

## Today

Two pieces we had **twice written off** turned out to be recoverable. Together they were
24 of the 35 functions outstanding this morning.

- The larger had been parked for two sessions because we believed we needed information
  nobody has. We never needed it. **One wrong line in a lookup table was the whole blocker.**
- The other needed a rotation calculation the decompiler had silently deleted. We rebuilt it
  and checked it against the original on **a million random inputs — identical every time**.

**The lesson, now written into the handover:** most of today's progress came from re-testing
a *stated reason* for being stuck, not from writing new code.

---

## The two risks

**1. We cannot yet test the thing we are building it for.** Nothing has ever run on web or
Android. It compiles for both; that is not the same thing. Closing this is the milestone
after the last 3 functions.

**2. An error that is harmless on PC and harmful on web.** A quirk in how the decompiler
writes arithmetic changes the answer only on machines without the PC's spare precision —
exactly web and Android. We have now **counted it: 575 places, 59 ranked most likely.** No
PC test can settle any of them; running the same scene on web and on PC settles all 575 at
once. That is the highest-value item on the web side.

---

## Confidence

Every claim traces to a recorded measurement that can be re-run. The working rule is that a
passing test is not believed until we have shown it could have failed.

This week that caught **five of our own checks** and, today, **three of our own
conclusions** — including a repair that passed every test we have and was still wrong. We
did not ship it. On your instruction we now build and prove any piece we are unsure about,
rather than assume the game will not reach it.
