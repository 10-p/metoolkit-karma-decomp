# Karma decompilation — status

Rebuilding UT2004's physics engine from the shipped binaries, so the web and Android
builds can have vehicles and ragdolls instead of nothing. Updated 2026-08-25.

---

## Where we are

**Collision detection — does A hit B — is done.** The game plays full matches on our
rebuilt code, indistinguishable from the original.

**Movement — how things fall, bounce and swing — is now done too, and the game has run on
it.** All seven modules reproduce the original exactly, step for step, on all three test
scenes. A week ago five of the seven were missing. The engine was rebuilt against them and
**executed our solver 301 times on a real map in a real match** — the first time our
movement code has run inside the game rather than beside it.

**One new risk, and it is the largest open item.** While fixing the last module we found a
class of error that **none of our tests can detect on a PC, and that changes results on web
and Android**. We found and fixed three instances of it in one module by reading the
original machine code. There are 152 other modules and nobody has looked at them. It would
show up as web physics quietly drifting away from PC physics, with no PC test able to
reproduce it.

**Nothing has run on web or Android yet.** It compiles for both — not the same thing —
and one of the two Android targets is known to be wrong.

---

## Numbers

| | |
|---|---|
| Modules rebuilt and in use | **121** of a 153-module working set |
| **Original modules the game still needs** | **21** — the number that measures the goal |
| Rebuilt but held back until proven | 13 |
| Not yet rebuilt | 14 |
| Movement modules rebuilt exactly | **7 of 7** — was 2 of 7 a week ago |
| Movement modules the game runs on | **14 of the 20 shipped**; 1 of the other 6 still matters |
| Collision types the game uses | **15** — 13 proven, 1 small defect, 1 open |
| Platforms building | PC ✅ · Web ✅ · Android 32-bit ✅ · Android 64-bit ⚠ |
| Platforms **running** | PC only |

Module count is a poor progress measure, and we now have a better one. The second row
counts the original modules the game still reaches — the ones that have to be replaced
before the rebuilt library can stand on its own. It went from 27 to 21 this week, and it
falls only when a module is rebuilt **and proven**, not merely rebuilt.

---

## Done

- Collision detection for every shape pair the game actually uses, measured against the
  original inside live matches — one pair verified over 1.7 million calls, zero
  disagreements.
- The engine playing full matches on our code, indistinguishable from stock.
- **The whole movement solver: all seven modules reproduce the original exactly**, over 900
  consecutive simulation steps on three scenes, on tests sensitive to one part in a million.
- **The engine rebuilt against them and run.** We confirmed at the machine-code level that
  the game is executing our code and not the original's, then counted the calls while a
  match was running.
- Two large components dealt with — the hull builder replaced (1.4 MB of third-party code
  down to 10 KB) and the asset loader rebuilt.
- Builds for web and 32-bit Android with an interface identical to the original.

## Left

| | size | |
|---|---|---|
| **An error class our PC tests cannot see** | **large / unknown** | exactly harmless on PC, changes about a third of results on web and Android. 3 fixed, 152 modules unexamined |
| **One movement module still the original's** | small | five of its six functions now reproduce the original exactly; the sixth is located to two helper routines. The last thing between us and using none of the original solver |
| **64-bit Android is wrong** — 2,680 known issues | medium | compiles and looks fine; it is not |
| **Nothing has executed on web or Android** | medium | separate workstream |
| Two small collision defects (~1 in 4,000, ~1 in 10,000) | small | both reproducible in seconds on a desk |
| The remaining modules | small | nothing cheap left |

---

## This week

**The movement half went from five missing modules to none.** The last one was out by one
part in a hundred million, and finding out why took the whole session and produced two
findings, one of them bigger than the module.

**The first was a rounding step the decompiler had quietly deleted.** The original code
writes three intermediate results to memory as ordinary decimals and reads them back; the
PC's floating-point unit keeps values at higher precision in its registers, so removing that
write leaves the numbers slightly *more* accurate than the original — and therefore wrong.
The decompiler had folded the write away and left the variable it belonged to sitting there,
declared and unused, which is the fingerprint. Putting the three roundings back took the
error from 52,902 disagreements in 400,000 test cases to zero.

**The second is the one to carry forward.** The decompiler also drops brackets from chains
of additions, which changes the order the additions happen in. On a PC that is *provably
harmless* — we measured zero difference across two million random cases, because the PC's
registers carry enough spare precision to absorb it. On web and Android there is no spare
precision, and the same two million cases differ in **31% of them**. So every test we have
is blind to it by construction, and the platform we are building for is exactly where it
bites. We fixed the three instances in this module; the other 152 modules have not been
looked at.

We could fix them confidently because of an unexpected piece of luck: Epic copied
MathEngine's movement code into UT2004's own source verbatim, so for this one module the
original source is available and settled the question outright.

**Then the engine was rebuilt on the finished solver and run**, and that produced a
correction to something we believed. We had recorded that the game calls the original's
integrator directly, and used it to decide what to work on. It does not — the call sits
behind a switch that is off by default, and by default the game uses its own copy. Measured
both ways: zero calls normally, fifty-one with the switch forced. The module still has to be
right, but it was never the blocker we had it down as. **Reading the branch around a call,
not just the call, is the lesson.**

**Four five-minute matches, ours and the original's, on both settings, no crash on any of
them.** That is the stability result. A caution on how it was obtained: the machine's
headless renderer has started crashing at the first frame under two of its three modes.
**The unmodified game does exactly the same thing**, so it is the environment and not our
code, and switching to the third mode makes both run clean. Running the unmodified control
before reading a crash as ours is the whole reason we know that.

The last original movement module went from "wrong by an unknown amount somewhere" to "five
of its six functions exactly right, and the sixth narrowed to two helper routines" — but
only after we found that the tool meant to attribute a fault to a function **was not
attributing anything at all**. It reported the same number for all six, including one that
does no arithmetic and therefore could not have produced it.

**And then we built the measure this project has been missing.** Until now progress was
counted in modules rebuilt, which does not answer the question that matters: how many of
the ORIGINAL modules does the game still need? A new tool answers it directly, by tracing
every function the engine calls through to whoever provides it — 21 today, 27 at the start
of the week. It is checked against reality rather than trusted: we deleted every original
module, tried to build the game, and confirmed the tool had predicted all 111 things that
went missing.

Six of those 21 were closed this week, and three of them by a single correction. A note in
our own records had said, for months, that a particular family of failures was the
compiler's fault and should be left alone. It was not: the compiler complains about exactly
one of two problems on the line and is silent about the second, so fixing what it named
left the real error in place. Correcting that recovered three modules — **and repaired four
more that had been compiling, passing every test, and computing the wrong answer.** One of
them was working out a sphere's mass from its radius read as a whole number.

**The reason we found that last group at all is the reason to keep running the real game.**
A module we had just cleared on all nine of our offline tests then killed the game during
start-up, because a table of function addresses had a blank where an address belonged. It
is the second time that exact defect has appeared. For anything touching the game's asset
files, our offline tests are not evidence — only the game is.

---

## Confidence

Every claim above traces to a recorded measurement that can be re-run. The working rule is
that a passing test is not believed until we have shown it could have failed, which has
repeatedly been the difference between real and imaginary progress. Three times this week
that rule caught our own tests: one check could not have matched anything it was looking
for; one deliberately-planted fault went undetected because the test never exercised the
code it was planted in; one attribution tool turned out to be attributing nothing; and a
module that passed all nine offline tests killed the game on start-up.

**Both halves are now rebuilt and proven on PC, and the game runs on them. What we cannot
yet claim is that they behave the same way on web and Android — and this week we found a
specific, measured reason to doubt it.**

**The finish line is now countable: 21 original modules left, from 27.**
