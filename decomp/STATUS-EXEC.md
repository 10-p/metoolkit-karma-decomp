# Karma decompilation — status

**What this is:** UT2004's physics engine, rebuilt from the shipped binaries so the **web
and Android** versions can have vehicles and ragdolls. Today they have no physics at all.
Updated 2026-08-27.

---

## The one number

# 0

Pieces of the original library the game still needs from us. It was 27 at the start of last
week, 20 on Monday, 3 yesterday, and **none today**.

**And it is no longer a calculation.** We built the game against a physics library with
**every one of MathEngine's original pieces deleted** — not disabled, deleted — and it
built, started, and played a full match. That test had never been run before today.

---

## Where we are

| | |
|---|---|
| **Still needed from the original** | **nothing** |
| Collision — does A hit B | done, proven in live matches |
| Movement — how things fall and swing | done, and the game runs on it |
| Loading vehicle and ragdoll files | done, and the game runs on it |
| **Runs on PC with none of the original library** | **yes — 5-minute match, no faults, indistinguishable from the original** |
| Runs on web | compiles; **never executed** |
| Runs on Android, 32-bit | compiles; **never executed** |
| Runs on Android, 64-bit | **no — see the risk below** |

---

## Today

**The milestone.** The last three pieces closed, and then the whole thing was assembled and
played. All three had been attempted before and had failed in a way that was hard to see:
each one *compiled*, and passed every check we had.

None of them was solved by thinking harder about it. Each was solved by **building a new
instrument that could see the fault**, and all three instruments are now permanent:

- one that isolates a single hidden function inside a file so a fault can be blamed on it;
- one that runs our version and the original side by side on a million random inputs;
- one that **switches on a code path the game normally never reaches**, so it can be tested
  rather than argued about.

**That third one is the important story.** We had a complete, correct, measured argument
that a particular function can never run in the game — and we were one signature away from
shipping it on that basis. Forcing it to run showed it **crashes on its first real call**.
Three separate faults came out, none of which any existing check could see. Two of them
turned out to be present in *another* piece we shipped weeks ago, which our own notes had
already flagged as working "by luck, not design".

---

## The two risks

**1. 64-bit Android will not work yet, and we had the wrong picture of why.**

Our notes said this was 95% fixed. That was true of the problem we were measuring and the
problem we were measuring was the easier half. Rebuilt code carries the original's memory
layout baked into it, and **52 of the 62 data structures change shape on a 64-bit machine**.
Nothing looks wrong, nothing warns, and the code reads the wrong memory. We built the check
that measures this today; it did not exist before. **The version that works is 32-bit** —
which covers web and the large majority of Android devices, but not all of them, and this is
now the main piece of engineering left.

**2. We still cannot test on the thing we are building it for.** Nothing has ever run on web
or Android. It compiles for both; that is not the same thing, and today's finding is exactly
what "compiles is not runs" looks like. Closing this is the next milestone, and it is the web
side's job.

---

## Confidence

Every claim traces to a recorded measurement that can be re-run. The working rule is that a
passing test is not believed until we have shown it could have failed — and today three of
the new checks were *deliberately broken first* to prove they would catch the fault.

That rule is what produced today's result. It also produced today's least comfortable
finding: a number we had been reporting as 95% complete turned out to be measuring the wrong
thing. We would rather find that ourselves than have a device find it.
