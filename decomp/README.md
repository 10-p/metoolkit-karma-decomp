# karma-decomp — recovering Karma from the shipped binaries

Recovers the proprietary, binary-only Karma (MathEngine `metoolkit`) physics library as portable C,
so the web (wasm) and Android builds can have real Karma physics instead of `NO_KARMA`.

**Why this and not an emulator:** the engine and Karma share one address space in both directions —
the engine holds raw pointers into Karma's heap and Karma holds function pointers into engine code —
so no out-of-address-space emulator (v86, QEMU-system, blink-as-a-VM) can work without rewriting the
integration layer. Full analysis, measurements and the rejected alternatives are in
[`../docs/KARMA-ON-WASM.md`](../docs/KARMA-ON-WASM.md).

**Why it's tractable:** the shipped archives carry **full DWARF-2 debug info** — original file names,
line tables, parameter names, local names, complete types. Ghidra consumes that directly. Measured
over the entire hot path (100 objects, 381.5 KB), **1092 of 1092 functions decompile**, with two x87
stack leaks totalling 235 bytes.

## Status

| | |
|---|---|
| Milestone 1 — one object end to end | ✅ **done** (`McdPrimitives/IxBoxBox`) |
| Milestone 2 — `MdtKea` C++/vtable spike | ⬜ next |
| Milestone 3 — scale validation | ⬜ |
| Milestone 4 — the grind (~2,100 functions) | ⬜ |
| Milestone 5 — wasm + Android bring-up | ⬜ |

### Milestone 1 result

`IxBoxBox` recovered, recompiled, substituted into the link in place of the shipped object, and
differential-tested against the original on **300,000 real `McdModelPair`s**:

```
  pairs actually touching        : 29747 (9.9%)
  bit-identical results          : 279540 (93.180%)
  DIFFERENT touch/return         : 0
  DIFFERENT contact count        : 0
  DIFFERENT contact dims         : 0
  worst absolute numeric delta   : 8.702e-06

RESULT: PASS -- every discrete decision matches; deltas are FP rounding only
```

Every discrete decision — overlap yes/no, contact count, contact dimensionality — matches exactly.
The residual is float last-bits, at micrometre scale.

## The recipe

Per object, three automated steps and one small manual one.

### 1. Decompile (automated)

```bash
KARMA_OUTDIR=/path/to/out \
  $GHIDRA/support/analyzeHeadless <proj-dir> <proj> \
    -import <object-or-dir> \
    -scriptPath karma-decomp/tools/gscripts \
    -postScript DumpDecomp.java -deleteProject
```

Emits one `.c` per object plus `stats.csv` with per-function quality metrics.

### 2. Write the prelude (manual — the only hand step)

Declare what the object imports, plus its file-scope statics. `nm --undefined-only` gives the import
list; `nm --defined-only` distinguishes exported (`T`) from static (`t`).

**Read constants out of the object. Never infer them.** Both values guessed while building the
`IxBoxBox` prelude were wrong:

```bash
# function-local statics live in .data — read the bytes
objcopy -O binary --only-section=.data IxBoxBox.o /tmp/d.bin
python3 -c "import struct;print(struct.unpack('<f',open('/tmp/d.bin','rb').read()[:4]))"
#   -> 0.2f   (guessed 0.5f)

# C++ imports need the exact mangled name for the asm label
nm --undefined-only IxBoxBox.o | grep Moving
#   -> _Z21MovingBoxBoxIntersectPKfPK11lsTransformRK6lsVec3S0_S3_S6_fRfRS4_
#      (guessed ...S1_S4_S7_...)
```

This step is the obvious next automation target: imports, their DWARF types, and `.data` constants
are all machine-readable. See "Known gaps" below.

### 3. Clean (automated)

```bash
python3 karma-decomp/tools/ghidra_clean.py out/IxBoxBox.o.c \
  -o src/McdPrimitives/IxBoxBox.c \
  --object <original>/IxBoxBox.o \
  --prelude src/McdPrimitives/IxBoxBox.prelude.h \
  --drop __static_initialization_and_destruction_0 \
  --drop _GLOBAL__I_McdBoxBoxIntersect
```

Strips Ghidra's advisory comments, marks originally-`static` functions static (read from the real
object, so the exported ABI matches exactly), flattens C++ `Foo::bar` to `Foo__bar`, and emits
forward declarations so definition order doesn't matter.

C++ static-constructor scaffolding is dropped: for `IxBoxBox` it only filled three constant vectors,
which become plain C initializers in the prelude.

### 4. Validate (automated) — this is the acceptance gate

Rename the original's exports so both implementations coexist in one process, then compare on real
inputs:

```bash
objcopy --redefine-sym McdBoxBoxIntersect=orig_McdBoxBoxIntersect ... orig.o
gcc -m32 ... difftest_boxbox.c recovered.o orig.o -Wl,--start-group <archives> -Wl,--end-group
```

For `static` functions, `objcopy --globalize-symbol` makes them callable first.

Because Karma is pure computation with no syscalls, **every function is a deterministic function of
its inputs — so the shipped archive is an oracle for its own replacement.**

## Layout

```
include/kd_compat.h                  Ghidra type/macro vocabulary; the longdouble decision
tools/ghidra_clean.py                Ghidra dump -> compilable C
tools/gscripts/DumpDecomp.java       Ghidra headless decompile + quality stats
src/<Archive>/<Object>.c             generated — do not edit by hand
src/<Archive>/<Object>.prelude.h     hand-written imports + statics
test/difftest_<object>.c             differential gate vs the shipped binary
```

## Load-bearing decisions

**`longdouble` → `double`.** Ghidra emits `longdouble` where the gcc 3.2 i386 build genuinely
computed at x87 80-bit. We map it to `double`, not C `long double`: on wasm `long double` is a
software-emulated 128-bit quad, and MathEngine themselves shipped a pure-f32 build
(`lib.rel/linux_hx_single`, x86-64/SSE), so f32 intermediates are vendor-blessed and f64 is strictly
better than that.

**`ROUND` → `rintf`, not a cast.** Ghidra emits `ROUND` for x87 `fistp`, which converts using the
current rounding mode — round-to-nearest, ties-to-even. `(int)x` truncates and would be a real
behavioural bug.

**Decompile i386, not x86-64.** wasm32 has 4-byte pointers, and the engine shares structs with
Karma, so only a 32-bit Karma has the right layout. The i386 build is also the only one (with
gcc2.95) carrying DWARF.

**Acceptance is per-function, not per-trajectory.** Past first contact, *any* two builds of Karma
diverge without bound — MathEngine's own i386 and x86-64 builds differ by 111 m after 15 s. UT2004
knows this and replicates `KRigidBodyState` rather than relying on determinism. So trajectory
diffing cannot validate a replacement; differential testing per function can.

## Known gaps

- **`-Wno-int-conversion` is required.** Ghidra types some pointer-valued locals as `undefined4`.
  Harmless on any 32-bit-pointer target (i386, wasm32) but latent, and worth fixing properly by
  retyping pointer-valued `undefined4` locals in `ghidra_clean.py`.
- **Prelude generation is manual.** Should be generated from `nm` + DWARF + `.data`.
- **C++/vtable code is unvalidated.** Everything proven so far is C. `MdtKea` (the LCP solver,
  68.5 KB, the hottest code) uses virtual dispatch and RTTI — Milestone 2.
- **Two x87 stack leaks** remain, both unnamed helpers in `MdtBcl.o` (143 and 92 bytes).
- `writeKeaInputToFile` / `readKeaInputFromFile` (~22 KB) are solver debug serialisation — stub them
  rather than recover them. `MeViewer2` and `MeApp` (74 KB) are already unused; skip entirely.
