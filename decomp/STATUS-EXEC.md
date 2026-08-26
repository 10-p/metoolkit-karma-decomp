# Karma decompilation — status

**What this is:** UT2004's physics engine, rebuilt from the shipped binaries so the **web
and Android** versions can have vehicles and ragdolls. Today they have no physics at all.
Updated 2026-08-26.

---

## The one number

# 0

Pieces of the original library the game still needs from us. It was 27 at the start of last
week, 20 on Monday, 3 on Tuesday, and **none since**.

**And it is no longer a calculation.** We built the game against a physics library with
**every one of MathEngine's original pieces deleted** — not disabled, deleted — and it
built, started, and played a full match.

---

## Where we are

| | |
|---|---|
| **Still needed from the original** | **nothing** |
| Collision — does A hit B | done, proven in live matches |
| Movement — how things fall and swing | done, and the game runs on it |
| Loading vehicle and ragdoll files | done, and the game runs on it |
| **Runs on PC with none of the original library** | **yes — 5-minute match, no faults, indistinguishable from the original** |
| **The web version of the game builds with physics in it** | **yes — new today** |
| Runs on web | **still never executed** |
| Runs on Android, 32-bit | compiles; never executed |
| Runs on Android, 64-bit | **no — see the risk below** |

---

## Today

**The web version of the game now builds with the rebuilt physics inside it.** That had never
been done: the pieces were known to compile individually, but the game had never been assembled
around them for anything other than a PC. It went together cleanly — including eleven files of
glue code between the game and the physics engine that had only ever been compiled for 1990s
PC hardware.

**What it costs: 756 KB.** The web download goes from 9.4 MB to 10.1 MB — about 7.7% — for a
complete physics engine. We also checked that the physics is genuinely *inside* the file rather
than merely built alongside it, which is a different question and one that a successful build does
not answer: 125 of 146 pieces are in, and the 21 that are not are the diagnostics and unused
features we had already established the game never touches. Two independent methods agree on which
ones those are.

**And the first web build immediately found a fault that PC testing cannot detect.** One line of
our rebuilt code was passing a value to the system in a way that a PC quietly tolerates and the web
does not — the web version turns that exact situation into an instant crash. Every one of our
existing checks reported it as fine, and would always have done: on a PC the two versions of that
line produce *byte-for-byte identical* machine code. We confirmed the crash by reproducing it in a
five-line test before changing anything, fixed it at the source rather than patching the output,
and confirmed that all 145 pieces still produce byte-identical PC code afterwards.

**The uncomfortable part, and it is the honest headline:** the check that caught this can only see
one of the two ways this fault can occur. The other way is invisible until the code actually runs.
So the fix is real and the class of fault is not closed.

**Also today:** the build instructions were wrong. The first two commands in the project's own
setup guide named build configurations that have not existed for some time, so a new person's first
command failed. Rewritten, with a genuine from-scratch path — prerequisites, then the commands —
and then verified by following them. The internal tooling (60 scripts) had no index at all; it now
has one, with every command tested rather than transcribed — which caught several of the
instructions being wrong as written. Two scripts turned out to be superseded and were deleted, and
~15 GB of stale build output was cleared.

---

## The risks

**1. "It builds" is not "it works", and today is the clearest example we have had.** Nothing has
ever run on web or Android. Today moved us from "the pieces compile" to "the game assembles around
them", which is real progress and is still not the same as running. The fault described above is
exactly what the gap between those two looks like, and the remaining half of that fault can only be
found by running.

**2. 64-bit Android will not work yet, and we had the wrong picture of why.**
Our notes said this was 95% fixed. That was true of the problem we were measuring, and the
problem we were measuring was the easier half. Rebuilt code carries the original's memory
layout baked into it, and **128 of 151 data structures change shape on a 64-bit machine**.
Nothing looks wrong and nothing warns.

We can now test this without an Android device — an ordinary 64-bit PC build has the same memory
layout as 64-bit Android. That turns an unknown into a work queue: a named file, a line number and
a stack trace, one at a time, until it runs. **The version that works today is 32-bit** — which
covers web and the large majority of Android devices — and 64-bit is ordinary debugging rather than
research.

**3. One accuracy item is closed as far as it is worth closing, and it should stop being
re-asked.** Fourteen of the fifteen collision routines the game uses now match the original
on every decision. The fifteenth disagrees on **one contact in two hundred thousand** and on
a descriptive *label* twenty times in two hundred thousand — and the original library
disagrees with *itself* on that same label sixteen times if you move a body by a tenth of a
micron. Four candidate fixes and one blanket approach were tried and every one made it the same or
worse. Closing it properly means matching the original's arithmetic bit for bit inside one large
routine, which is days of work for a difference nothing in the game reads. It is written down so it
is not rediscovered.

---

## Confidence

Every claim traces to a recorded measurement that can be re-run. The working rule is that a
passing test is not believed until we have shown it could have failed.

That rule paid twice today. A new tool we wrote to check "is the physics really inside the web
file" confidently reported that **none of it was** — which was the tool measuring the wrong kind of
build, not a finding. It now refuses that case instead of printing a frightening number. And a
long-standing check that reports "nothing is unused" turned out to print exactly the same reassuring
output when pointed at the wrong directory; it now refuses too.

The pattern is worth stating plainly, because it has now produced most of this project's real
findings: **the dangerous output is not the failure, it is the success that was never at risk of
failing.**
