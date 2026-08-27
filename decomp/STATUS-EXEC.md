# Karma decompilation — status

**What this is:** UT2004's physics engine, rebuilt from the shipped binaries so the **web
and Android** versions can have vehicles and ragdolls. Today they have no physics at all.
Updated 2026-08-27.

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
| **The web version of the game builds with physics in it** | yes |
| **Runs on web** | **yes — including a 5-minute Onslaught match with vehicles and bots** |
| **Runs on PC (Linux) on the rebuilt physics** | **yes — 5-minute Onslaught matches on three maps** |
| **Runs on Windows** | **yes — 5-minute Onslaught matches on three maps** |
| Runs on Android, 32-bit | compiles; never executed |
| Runs on Android, 64-bit | **no — see the risk below** |

---

## Today, last — Windows works too, and the last fault was one line

All three 32-bit versions — web, Linux and Windows — now play a five-minute Onslaught match with
vehicles and bots. Windows took four separate fixes, and the final one is worth recording because
of how small it was and how far its effects reached.

The rebuilt code carries annotations marking which functions were originally C++ methods. On Linux
those annotations mean nothing and the file switched them off — but only *if the compiler had not
already defined them*. Windows compilers define them, so the switch-off was skipped and all 140 of
those functions silently changed how they receive their first argument. Every argument after it
arrived one position early. The physics engine read a count where it expected a pointer and stopped.

We found it by printing the same values on both sides of one call, on Linux and on Windows, and
comparing — rather than reasoning about compiler conventions. The difference was immediate and
unambiguous. The fix makes the switch-off unconditional; it changes nothing at all on Linux or the
web (identical machine code, identical web file), which is exactly why only a Windows build could
ever have found it.

**One thing we found along the way that is not ours and is worth a decision:** every PC build of
this game — engine and physics alike — is compiled with optimisation switched *off*, because of a
build rule marked "for now" that catches every configuration. It predates this work. It matters
beyond speed: an unoptimised build kept a dormant defect alive that an optimised one removes by
itself.

## Today, earlier — the rebuilt physics is now the DEFAULT everywhere it is sound

The web, Linux and Windows builds all take the rebuilt physics now rather than the original; the
original stays available side by side, as the thing we measure against. Android is deliberately
still without it, because two of its three processor types are 64-bit and the rebuilt code carries
32-bit memory layouts (see risk 2).

**And a five-minute Onslaught match is now part of accepting a build — which immediately found a
fault the entire 51-test suite could not.** Onslaught is the mode with VEHICLES, and vehicles and
ragdolls are most of what this physics engine is for; the test suite is almost entirely ordinary
deathmatch, so it never touched that code at all. The game crashed 25 seconds in, on both the web
and the PC.

The cause was one routine writing whole numbers into memory as if they were decimals — the
decompiler had guessed the wrong kind of number for a block of data, so "vertex 1" was stored as
the decimal 1.0 and read back as roughly 1,065,000,000. Two of the fields it filled were correct
by pure chance, because the decimal patterns for 0, 1, 2, 4 and 8 happen to be the whole numbers
0, 1, 2, 4 and 8 — which is part of why it looked fine. Fixed at the source; the PC machine code
for the other 144 pieces is unchanged, and Onslaught now runs for five minutes on three maps and
in a browser.

Two things went wrong with our own tooling while chasing it, both worth recording: a search harness
reported that **all nine** candidate routines were the culprit, because it looked for the word
"PASS" in output that prints that word on every run — a check that could not fail. And a harness
that swaps one routine at a time measures a *combination*, so it accused a second routine that
turned out to be innocent.

## Today

**The rebuilt physics now runs in a web browser.** That is the thing every previous version of
this page listed as never done, and it was the largest unknown in the project: the code had been
compiled for the web, checked, and measured, but not one instruction of it had ever been
*executed* outside a PC. The game now starts in a browser, loads its ragdoll definition files,
and runs physics — fifteen minutes without a fault.

**Getting there took four fixes, and they were all the same mistake.** In each case our
rebuilt code called a function *through a pointer* while describing it slightly wrongly — one
argument too many, or expecting an answer from something that returns nothing. **A PC does not
care about either.** Its calling convention lets the caller push spare arguments and ignore
unwanted answers, so the machine code is correct on a PC whichever way it is written, and every
one of our nine checks passed on all three, before and after. The web is stricter: it verifies
the description against the real function at the moment of the call and stops the program dead
if they differ. So these were not latent risks that the web made worse — they were invisible on
a PC **by construction**, and the web is the only place they can be seen at all.

**The fixes changed almost nothing on the PC, which is how we know they are safe.** Of 145
rebuilt pieces, **140 are byte-for-byte identical** afterwards, and the five that moved changed
in exactly the way predicted. Both physics test scenes give results identical to before.

**One of the four is worth recording as a process failure, not a technical one.** We twice
announced this class of mistake fixed on the strength of a text search, and twice a running
game found another one — because the decompiler writes the same construct at least five
different ways and the search knew four of them. The replacement does not search for spellings;
it parses the code and asks which expressions are being *called*. It found the rest immediately.

**And one of the three turned out to be a PC bug as well** — the first time the web work has
found a fault on the PC side rather than the other way round. A number that should have been
passed as a 4-byte value was being widened to 8 bytes, so the receiving function would have read
it wrong. It is harmless today only because that particular routine is never used by UT2004,
which reimplements it. Nobody was looking for it, and nothing on the PC side would have.

**We also built the check that was missing.** The web toolchain verifies these descriptions only
for functions called *by name*; the whole class of fault above is in functions called *through a
pointer*, which it does not check at all. The new tool inspects the finished web file directly
and proves which of those calls could never succeed. It reports none today — and it is written
to say plainly what it cannot see, because it would not have caught the third fault on its own.

**The game's own automated test suite is the real verdict, and it passes: 51 of 51** — startup,
caching, mod loading, menus, renderers and two-player networked matches. To be sure that number
meant something, we built the game *without* the rebuilt physics from the same source on the same
machine in the same session and ran the identical suite: also 51 of 51. So adding a complete
physics engine costs nothing the test suite can detect.

---

## Previously (2026-08-26)

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

> **That prediction was correct, and running the code closed it — see Today.** The "other way"
> occurred three more times, and every one was found by actually running rather than by any
> check. We have since built a tool for it, which finds some of them without running — but not
> all, and it says which.

**Also today:** the build instructions were wrong. The first two commands in the project's own
setup guide named build configurations that have not existed for some time, so a new person's first
command failed. Rewritten, with a genuine from-scratch path — prerequisites, then the commands —
and then verified by following them. The internal tooling (60 scripts) had no index at all; it now
has one, with every command tested rather than transcribed — which caught several of the
instructions being wrong as written. Two scripts turned out to be superseded and were deleted, and
~15 GB of stale build output was cleared.

**And then every one of those 60 was actually run**, including the expensive ones that need a
playable build of the game — four separate versions of the engine were compiled for it. The whole
project's state was re-measured in a single pass rather than trusted. Three things came out of it:

- **The decompiler stage is reproducible, which nobody had ever checked.** Re-running it on a file
  produces a byte-for-byte identical result. The project had been treating the ~1 GB of decompiler
  output as irreplaceable and archiving every version of it; it is a cache.
- **The one remaining accuracy item is smaller than it looked, in a way that settles it.** We can
  now say what our 1-in-200,000 disagreement is *relative to*: nudge a body by a tenth of a micron
  and **the original library disagrees with itself nearly ten thousand times** in the same test.
  Matching it more closely would mean being more self-consistent than the thing we are copying.
- **The finished physics library was re-assembled, linked into the game, and played** — 180 seconds,
  no faults, beside an unmodified copy of the game running the same map in the same session, with
  an identical physics-warning profile. That is the deliverable, re-proved rather than quoted. A
  further test drove 1.57 million real physics calls through both versions simultaneously and found
  **zero** disagreements on any decision either one makes.

---

## The risks

**1. "It builds" is not "it works" — and this is now measured rather than warned about.** The web
half of this risk is closed: the game runs. It took three faults to get there, none of which any
check on the PC side could have found, which is the strongest evidence the project has produced
for the gap between compiling and working. **Android is still entirely open** — neither the
32-bit nor the 64-bit build has ever executed a single instruction, and the 32-bit one is the
same kind of target as the web, so the same class of fault is plausible there and unmeasured.

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
