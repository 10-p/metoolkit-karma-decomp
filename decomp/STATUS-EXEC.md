# Karma decompilation — status

**What this is:** UT2004's physics engine, rebuilt from the shipped binaries so the web, Android
and 64-bit versions can have vehicles and ragdolls. When this started they had no physics at all.

**Updated 2026-09-02.**

---

# ✅ COMPLETE FOR UT2004

Every code path UT2004 exercises is recovered, repaired and measured. Every target the game ships
on runs on the rebuilt physics. **If someone asks "what is the state of Karma", the answer is: done
for UT2004** — and the open items below are all about being a *general-purpose Karma SDK*, which
was never the goal.

## Where we are

| | |
|---|---|
| **Pieces of the original library the game still needs from us** | **none** |
| **Can MathEngine's original object code still be linked at all?** | **No — the build path was removed.** It is not "we choose not to"; there is no longer a way to. |
| Collision — does A hit B | done, proven in live matches |
| Movement — how things fall and swing | done, and the game runs on it |
| Loading vehicle and ragdoll files | done, and the game runs on it |
| **Web (wasm32)** | **ships** — 55/55 browser suite, Onslaught with vehicles and bots |
| **Linux 32-bit** | **plays** — the drop-in with every original piece deleted |
| **Linux 64-bit** | **plays** — 25-minute Onslaught match, 8 of 8 gametypes clean |
| **Windows 64-bit** | **plays** — and its physics is the 32-bit control's, to the last digit |
| **Android arm64** | **plays** — 23-minute Onslaught match on a real device |
| **Android armv7** | **plays** — the same match on the 32-bit slice |

## How "complete" was decided, so it can be re-checked rather than believed

Not by counting objects. By measuring which code the game **enters** and closing that set.

```
execution census   660 functions entered over two Onslaught matches
                   111 of 147 recovered objects entered; 36 never entered
open defects in the entered set                                     0
declined-but-unrepaired sites in the entered set                    0
   398 decline lines, 368 in entered objects, 262 already carrying a
   measured verdict, and the remaining layout exposure is 47 sites whose
   base is declared a pointer to exactly the type it indexes
```

The gates behind that, all green on 2026-09-02:

```
i386 acceptance      145 objects, 0 compile failures, 0 byte differences
run-standalone       12 passed, 0 failed
LP64 trajectory      byte-identical to the 32-bit control (md5 c31ed77b7323, K=1396),
                     and identical across two runs of the same binary
Windows x64          same gate, differs only at the 1e-17 floor (mingw libm vs glibc)
web artefact         sha256 6a380872… — the file the 55/55 suite passed on
```

---

## What is still open — and all of it is BEYOND UT2004

None of the following affects the game. They are listed so that "complete" is a precise claim and
not a vague one: this is a **UT2004 physics backend**, not a validated general-purpose Karma.

**1. Code UT2004 never enters (36 recovered objects).** Measured, not assumed. It includes five
constraint types the game never instantiates (`MdtLinear1`, `MdtLinear2`, `MdtPrismatic`,
`MdtSpring`, `MdtUniversal`, `MdtCarWheel`, `MdtRPROJoint`, …), the XML asset writer, the profiler,
debug draw, and `MdtLOD`. It compiles and it is byte-identical at i386; **it has never executed.**
A different game driving Karma differently would be relying on code no measurement here covers.

**2. `MdtLOD` carries two known 64-bit defects.** `bodyArray` is read four bytes wide where the
field is eight, and the partition cursor steps `0x14`, which is the element size at 32-bit and half
of it at 64-bit. UT2004 cannot reach them — the engine's only call site is guarded by
`rowCount > maxMatrixSize` with `maxMatrixSize = 0x7ffffffc`, and a breakpoint fires **zero** times
in a 235-second match. ★ **Anyone calling `MdtWorldSetMaxMatrixSize` with a real bound makes both
of them live.**

**3. Three objects do not compile** — `McduDebugDraw`, `MeASELoad`, `MeFGeometryFromMesh` — and five
more have no usable dump (`McdVanillaCore`, `MeMemoryCpp`, `MeString`, `keaStuff`, `version`). The
engine's only references to them are dead-stripped: the linked binary contains no `MeASEObject*`
symbol at all. A consumer that *does* call them has nothing to link.

**4. The convex hull is a replacement, not a recovery.** 13 shipped members (qhull) are stood in for
by a hand-written `kd_convexhull.c`, validated against the original's contract (894 canonical lines,
agreeing except last-digit float noise in one normal component). It is correct for what UT2004 asks
of it; it is not qhull.

**5. Nine objects compile with `TODO`s in their prelude**, and 13 unmodelled `extraout_*` values are
proven inert rather than modelled.

**6. Two truncation-census rows are closed as "near-miss"** (`MdtPartition` 143, `keaDebug` 665/684)
because a whole-function rule cannot separate two live ranges of one Ghidra local. The live-range
guard added on 2026-09-02 is the tool to revisit them with. Both are in code the game does not enter.

---

## The risks worth remembering

- **Per-object gates are blind to integration defects.** `145/145 · 0 byte differences` has been
  green for a crash, an uncompilable branch, a doubled pointer stride and a heap under-allocation.
  The trajectory trace against the 32-bit control is what caught every one of them, and it is now a
  gate that runs (`ktrace_gate.sh`) rather than a step someone remembers.
- **A green offline scene is not a running game.** `MeAssetDBXMLIO` passed every offline gate and
  killed the engine before a match started.
- **Reachability arguments have been wrong here before.** Item 2 above is an argument backed by a
  breakpoint; if the guard ever changes, it stops being one.
