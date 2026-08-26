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
played. Separately, one of the two remaining accuracy defects was found and fixed: a
rounding the original performed and our rebuild did not, which was enough to change how many
contact points a cylinder generates. The same fingerprint appears in 229 other places, now
listed. All three had been attempted before and had failed in a way that was hard to see:
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

Our notes said this was 95% fixed. That was true of the problem we were measuring, and the
problem we were measuring was the easier half. Rebuilt code carries the original's memory
layout baked into it, and **128 of 151 data structures change shape on a 64-bit machine**.
Nothing looks wrong and nothing warns.

**On your question about testing 64-bit: yes, and it was the single most useful thing done
today.** We had written down that we could not test this without an Android device. That was
wrong — an ordinary 64-bit PC build has the same memory layout as 64-bit Android, and we have
one of those. It took twenty minutes, and it failed on the *first line of the first test*:
the code asks the system for 564 bytes of memory and then writes 880 bytes into it, because
564 was the right answer on a 32-bit machine. We also proved the same code is clean at 32-bit,
so that is definitely the 64-bit problem and not a general bug.

That turns an unknown into a work queue: instead of a count of places something *might* be
wrong, we now get a named file, a line number and a stack trace, one at a time, until it runs.
**The version that works today is 32-bit** — which covers web and the large majority of
Android devices — and 64-bit is now ordinary debugging rather than research.

**2. One accuracy item is closed as far as it is worth closing, and it should stop being
re-asked.** Fourteen of the fifteen collision routines the game uses now match the original
on every decision. The fifteenth disagrees on **one contact in two hundred thousand** and on
a descriptive *label* twenty times in two hundred thousand — and the original library
disagrees with *itself* on that same label sixteen times if you move a body by a tenth of a
micron. We spent a session trying to close it: four candidate fixes and one blanket approach
were tried and every one made it the same or worse. Closing it properly means matching the
original's arithmetic bit for bit inside one large routine, which is days of work for a
difference nothing in the game reads. It is written down so it is not rediscovered.

**3. We still cannot test on the thing we are building it for.** Nothing has ever run on web
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
