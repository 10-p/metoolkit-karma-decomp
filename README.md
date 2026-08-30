# metoolkit-karma-decomp

**MathEngine Karma 1.2 — the shipped SDK, and a source-level recovery of it.**

Karma was the physics engine behind Unreal Engine 2's ragdolls and vehicles
(MathEngine, later Criterion, later rebranded RenderWare Physics). Epic shipped
it with UT2004 as **binary-only 32-bit x86 archives**, and that is a hard
ceiling: no build engineering links an i386 ELF `.a` into WebAssembly, an ARM64
Android APK, or a modern 64-bit binary. Ports of UT2004 to any of those
therefore shipped with no physics at all — no vehicles, no ragdolls.

This repository holds two things:

| | |
|---|---|
| **`metoolkit/`** | the SDK as UT2004 v3369 shipped it — headers and all nine build configurations, preserved verbatim |
| **`metoolkit_decomp/`** | **145 of its objects recovered to portable C**, plus a hand-written convex hull replacing qhull. Compiles and runs wherever a C compiler does |

Plus the machinery: **`decomp/`** (the recovery toolchain, its gates and its
written record) and **`lab/`** (the Ghidra working set it reads).

## Using the recovered library

```cmake
FetchContent_Declare(metoolkit_karma_decomp
  GIT_REPOSITORY https://github.com/10-p/metoolkit-karma-decomp.git
  GIT_TAG        main)
FetchContent_MakeAvailable(metoolkit_karma_decomp)
target_link_libraries(YourEngine PRIVATE metoolkit_decomp::all)
```

`metoolkit_decomp/README.md` is the consumer documentation, including the one
define that will silently corrupt your stack if you get it from somewhere else.

There are no releases and there will not be. This is source.

## How good is the recovery?

Held to the shipped library, not to a style guide:

- **All 145 objects recompile BYTE-IDENTICAL at `-m32`** against the archive
  members they were recovered from. That is the gate on every change to the
  generator.
- **The drop-in gap is zero.** UT2004 plays a full Onslaught match — vehicles,
  ragdolls, the lot — linked against a metoolkit with every shipped member
  deleted.
- **64-bit is a separate, passing gate.** Three physics scenes at LP64 pointer
  width, every trajectory first differing at the float floor.
- The browser build ships it and passes UT2004's whole E2E suite.

⚠ And the honest limit, recorded because it cost this project real time:
**byte-identity cannot validate a type or a pointer-width cast.** `kd_uptr` *is*
`unsigned int` at i386, so a wrongly-chosen type compiles identically. Seven had
shipped before a gate that could see them existed. `decomp/proven.txt` is the
ledger of every class of defect that got past one gate and was caught by another.

## Layout

```
metoolkit/              the v3369 SDK drop, verbatim (96 MB)
metoolkit_decomp/       the recovered library — include/ + src/<Lib>/ + CMake
metoolkit_decomp_dummy/ empty placeholders, for building with physics off
decomp/                 the recovery toolchain, gates, and written record
lab/                    the Ghidra dumps and shipped objects the tools read
```

Start at `CLAUDE.md` if you are going to work on the recovery;
`decomp/README.md` and `decomp/HANDOVER.md` are the depth.

## Provenance, and what this is not

The Karma SDK in `metoolkit/` is MathEngine's work, from a 2004 game
distribution. The recovered sources in `metoolkit_decomp/` are a reconstruction
of that same work — they are not independently invented, and nothing here claims
otherwise. It is published for game preservation and for the UT2004 web and
mobile ports, which cannot exist without it.

There is a separate, unrelated preservation of a **different** Karma 1.2 drop at
[`sigmaco/metoolkit-karma-v1.2`](https://github.com/sigmaco/metoolkit-karma-v1.2).
It is not interchangeable with this one and `metoolkit/README.md` has the
byte-level comparison — the short version is that the two configurations this
recovery depends on, `linux_single_gcc3.2` (the only one carrying DWARF) and
`win_amd64_single` (the 64-bit oracle), are not in it.
