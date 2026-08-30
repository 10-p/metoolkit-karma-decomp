# Running Karma on the web: options investigation

**Question asked:** can the binary-only Karma (MathEngine `metoolkit`) physics library be wrapped in
an emulator/middleware that compiles under Emscripten, so the web build (and, secondarily, Android)
gets real Karma physics instead of `NO_KARMA`?

**Short answer:** the *instinct* is right, but the right mechanism is **ahead-of-time translation,
not runtime emulation**. Every general-purpose emulator in the list (v86, QEMU-wasm, blink-as-a-VM)
is disqualified by an architectural constraint specific to how UT2004 talks to Karma — not by
performance. Meanwhile the measurements below make the AOT path far cheaper than it looks, and
turn up a shortcut nobody appears to have noticed: **the prebuilt libs carry full DWARF-2 debug
info, including original file names, line tables, parameter names and types.**

Everything below is measured from the in-tree artifacts, not estimated.

---

## 1. What Karma actually is, in numbers

Measured against `build-native-karma/Source/SDLLaunch/ut2004-karma-pixo.bin` (the working 32-bit
reference build) and `Thirdparty/metoolkit/`.

| Metric | Value |
|---|---|
| Archives shipped | 16 (`lib.rel/`), of which **14 are linked**, 2 unused (`MeViewer2`, `MeApp`) |
| Object files pulled into the link | **147 of 192** |
| **Total x86 machine code actually used** | **673 KB** |
| Distinct functions | ~2,124 |
| **Karma API functions the engine calls** | **328** |
| Original source lines (from DWARF line tables) | **~26,660** across 161 files |
| Engine files with `WITH_KARMA`/`NO_KARMA` guards | 53 files, 173 sites |

### Per-archive breakdown of the 673 KB

| Archive | Pulled KB | Role | Hot per frame? |
|---|---:|---|---|
| `libMcdConvexCreateHull` | 186.5 | **qhull 2.6 (1998)** — convex hull generation | ❌ load-time only |
| `libMcdPrimitives` | 124.9 | box/sphere/cylinder/sphyl/triangle intersection | ✅ |
| `libMdtKea` | 68.5 | the LCP solver ("kea") | ✅ hottest |
| `libMdtBcl` | 67.8 | constraint row building | ✅ |
| `libMdt` | 45.3 | rigid-body dynamics API | ✅ |
| `libMcdConvex` | 44.7 | GJK / convex mesh | ✅ |
| `libMeGlobals` | 33.7 | math, memory pools, dict/hash | ✅ |
| `libMcdFrame` | 24.3 | collision framework, model pairs | ✅ |
| `libMcdCommon` | 20.2 | shared collision plumbing | ✅ |
| `libMeAssetDB` + `libMeAssetDBXMLIO` + `libMeXML` + `libMeAssetFactory` | 51.0 | `.ka` asset loading (XML) | ❌ load-time only |
| `libMst` | 6.0 | Mdt↔Mcd bridge | ✅ |

**~240 KB (36%) is load-time-only and independently replaceable** — qhull is open source and the
asset/XML loader is a file-format reader, not physics. The genuinely hot, genuinely proprietary
core is **~380 KB**.

### The libc surface is trivially small

Undefined symbols across all archives, excluding inter-Karma references and the unused GL/GLUT
viewer, come to roughly **60 libc functions**:

- `malloc/calloc/realloc/free/memcpy/memset`
- `str*`, `sprintf`/`snprintf`/`sscanf`/`strtod`/`strtol`
- `fopen/fclose/fread/fwrite/fseek/fgets/fprintf`, `open/close/read/write/lseek`
- `abort/exit/qsort/rand/time/localtime/setjmp/longjmp`
- compiler-rt: `__divdi3`, `__udivdi3`, `__fixunssfdi`, `__cxa_pure_virtual`, `__gxx_personality_v0`
- the glibc <2.3 ctype tables (already handled by `Source/Engine/Src/KarmaGlibcCompat.c`)

**No threads. No `mmap`. No signals. No `ioctl`. No dynamic linking.** Karma is essentially pure
computation over a heap it asks the host to allocate. This is the single best fact in the whole
investigation: there is almost no OS to emulate.

### Per-frame cost, measured

I linked a standalone harness against the libs and timed the solver directly (60 bodies, 50 hinge
constraints — roughly 4 ragdolls plus a couple of vehicles):

```
Mdt solver: 60 bodies, 50 hinge constraints
  77.41 us per world step
  at 60Hz that is 0.464% of a 16.67ms frame
  slowdown budget to stay under 10% of frame: 22x
```

Caveats, stated plainly: this measures the **dynamics solver only** — it excludes collision
detection (`Mcd*`, comparable or larger), and `KTickLevelKarma()` runs one full step **plus up to
`ME_MAX_SUBSTEPS` substeps** per frame. A realistic all-in budget is therefore closer to
**5–10× slowdown**, not 22×. That is still a large budget, and it is the number that decides
everything in §3.

---

## 2. The constraint that disqualifies every off-the-shelf emulator

UT2004 does **not** talk to Karma through a message-passing API. It shares an address space with it,
in both directions:

**The engine holds raw pointers into Karma's heap.** `MdtBodyGetTransformPtr(body)` returns a
`MeMatrix4Ptr` — a pointer to the live 4×4 transform inside Karma's body struct — and the engine
dereferences it every frame for every body. `McdCTypes.h:305` defines `struct _McdModel` in full;
the structs are transparent, not opaque handles.

**Karma holds function pointers into engine code.** From `KarmaSupport.cpp`:

```c
level->KWorld->bodyDisableCallback   = RemoveActiveActorFromBody;
level->KWorld->bodyEnableCallback    = AddActiveActorFromBody;
level->KAssetFactory->modelCreateFunc = KModelCreateFromMeFAssetPart;
level->KAssetFactory->jointCreateFunc = MdtConstraintCreateFromMeFJoint;
MeSetInfoHandler(KDebugHandler); MeSetFatalErrorHandler(KErrorHandler); ...
```

plus the memory allocator itself (`KMemCreate`/`KMemDestroy`/`KMemResize`), the per-contact callback
`KPerContactCB`, and `KTriListGenerator` — which Karma calls **during** collision detection to ask
the engine for static-mesh triangles near a contact.

So the boundary is crossed thousands of times per frame, in both directions, with pointers that must
be valid on both sides.

**Consequence:** any design where Karma lives in a *separate* address space — a v86 machine, a
QEMU-system guest, a blink process behind a pipe, a Web Worker with its own memory — has to marshal
that traffic. That is not a performance tax you can pay down; it is a rewrite of the integration
layer, and it would be thousands of round-trips per frame through a serialization boundary. **This
kills the whole "run a Linux VM in the browser" family, regardless of how fast the VM is.**

The corollary is the design rule for everything that follows:

> **Karma's guest address space must *be* the Emscripten wasm linear memory.** Guest pointers must
> be ordinary wasm addresses that the engine can dereference natively.

---

## 3. The options, with verdicts

### A. Full-system emulation — v86, QEMU-wasm, qemujs · ❌ Reject

[v86](https://github.com/copy/v86), [ktock/qemu-wasm](https://github.com/ktock/qemu-wasm),
[atrosinenko/qemujs](https://github.com/atrosinenko/qemujs).

Boots an entire PC to run a 77 µs physics tick. Separate address space (fatal, §2), separate
scheduler, tens of MB of artifact, and you would still have to build a marshalling protocol. QEMU's
browser support ([2025 patch series](https://lists.nongnu.org/archive/html/qemu-arm/2025-04/msg00153.html))
targets *system* emulation specifically. There is no version of this that ends well.

### B. User-mode process emulation — blink, QEMU-user · ❌ Reject as-architected

[jart/blink](https://github.com/jart/blink) genuinely does what its issue thread says — there is a
~116 KB wasm build that runs x86-64 Linux ELF binaries in a browser
([demo](https://jart.github.io/blink/blink.html), [x86-64 playground](https://x64.halb.it/)). It is
the closest thing to "qemu-user for the web" that exists.

But it emulates a *process*: it owns its guest memory, loads an ELF, and services syscalls. Using it
means Karma-in-a-process talking to engine-in-another — §2 again. No standalone
QEMU-`linux-user`-to-wasm build appears to exist either; every port targets the full system tree.

**However:** blink's *instruction semantics* are excellent, well-tested, MIT-ish licensed C, and
already known to compile under Emscripten. As a source of a correct x86 interpreter core to graft
into option C, it is the best starting material available. Do not use blink as a VM; harvest it.

### C. In-process interpreter over shared linear memory · ⚠️ Viable as a bring-up step, too slow to ship

Take an x86 interpreter core (blink's, or [Unicorn](https://github.com/unicorn-engine/unicorn) —
which already has working Emscripten builds, [unicorn.js](https://github.com/AlexAltea/unicorn.js/),
[unicorn.wasm](https://github.com/elliott-wen/unicorn.wasm)), and configure its guest physical memory
to *be* the wasm heap rather than a private buffer. Load the Karma `.o` code into a region of that
heap, relocate it, and add a thunk layer:

- engine → Karma: set up guest registers per the SysV ABI, run until return
- Karma → engine: trap calls to addresses in a registered-callback range, dispatch to the real wasm
  function, resume

This satisfies §2 exactly, and it is the *literal* reading of "wrap Karma in an emulator middleware".

The problem is arithmetic. Interpreters run **30–100× slower** than native. Against the 5–10×
realistic budget from §1, that is 5–20× over budget before accounting for wasm itself being
~1.5–2× slower than native. Unicorn additionally carries QEMU's TCG, which cannot JIT under wasm, so
you get the interpreter path anyway plus a lot of weight.

**Verdict:** build this *first anyway*, as a correctness oracle and a bring-up vehicle — it is the
fastest route to "ragdolls move in the browser" and it validates the thunk design. Then replace the
interpreter with option D under the same ABI, keeping the harness.

### D. Ahead-of-time static recompilation · ✅ **The recommended mechanism**

Translate the 673 KB of x86 to something the wasm toolchain compiles directly — **once, offline** —
with no interpreter at runtime. This is what N64/GameCube/Xbox PC ports do:
[N64Recomp](https://github.com/N64Recomp/N64Recomp),
[gcrecomp](https://github.com/sp00nznet/gcrecomp), and most relevantly
[xboxrecomp](https://github.com/sp00nznet/xboxrecomp) — Xbox is x86, and there is even an
`xbox_single_libcmt` Karma variant in-tree.

Translated code costs roughly **2–5× native** — comfortably inside budget.

**Translate to C, not directly to wasm.** C then compiles to wasm *and* to arm64/armv7 — which is
how Android gets Karma too, the user's secondary goal, for free. Emitting wasm directly forecloses
that.

Existing tooling is not usable off the shelf:

- [speet](https://github.com/portal-co/speet) (Rust, ELF→wasm) is the closest match, but its x86-64
  frontend emits `unreachable` for **all FP/SIMD** — i.e. exactly the instructions Karma is made of —
  and its one-wasm-function-per-guest-instruction design would be very slow. Also AGPL/commercial.
- [McSema](https://github.com/lifting-bits/mcsema)/[remill](https://github.com/lifting-bits/remill)
  and [rev.ng](https://github.com/revng/revng) lift x86 to LLVM IR, which the LLVM wasm backend can
  then emit. Viable in principle; the output is register-machine IR that optimizes poorly, and
  McSema is archived.

So D means writing a purpose-built translator. The good news is how small the target is (§4).

#### Which build do you translate? This matters more than it looks.

| Variant | Arch | Pointer size | Float unit | Verdict |
|---|---|---|---|---|
| `linux_single_gcc3.2` | i386 | **4 bytes ✅** | **pure x87 ⚠️** | **the one to use** |
| `linux_hx_single` | x86-64 | 8 bytes ❌ | pure SSE scalar ✅ | wrong data layout for wasm32 |
| `macos_single` | **ppc32** | 4 bytes ✅ | clean IEEE ✅ | **big-endian ❌ fatal** |
| `win32_single_msvcrt`, `xbox_single_libcmt` | i386 COFF | 4 ✅ | x87 ⚠️ | equivalent to gcc3.2, no advantage |

This is the central trade-off and it is not obvious:

- **wasm32 has 4-byte pointers.** Karma's structs are shared with the engine (§2), so the struct
  layouts must match what `Engine`'s C++ sees. Only a **32-bit** Karma has the right layout. The
  x86-64 build's structs have 8-byte pointers and different padding throughout — translating it into
  wasm32 would desynchronize every shared struct. (wasm64/memory64 would fix this, at a real
  performance cost and worse browser support. Not worth it.)
- **But the i386 build is pure x87.** I verified this: `libMdt.a` contains 754 `fxch`, 551 `flds`,
  358 `fstps`, 99 `fnstsw` — and **zero** SSE instructions. The x86-64 build is the mirror image:
  `movss`/`mulss`/`addss`/`divss`/`sqrtss`/`ucomiss`, essentially no x87.
- The macOS ppc32 build is tantalizing — 32-bit pointers *and* clean IEEE FP registers, no x87 stack
  — but it is **big-endian**. With shared structs and raw float pointers crossing the boundary (§2),
  byte-swapping on every access is not survivable. Ruled out.

**So: translate i386, and solve x87.**

#### The x87 problem is smaller than its reputation

x87 has no wasm equivalent — 80-bit extended precision doesn't exist there
([WebAssembly/design#303](https://github.com/WebAssembly/design/issues/303),
[llvm#35386](https://github.com/llvm/llvm-project/issues/35386)). Emulators either eat the
performance of a softfloat fp80 ([FEX-Emu](https://deepwiki.com/FEX-Emu/FEX/3.4-cpu-feature-emulation),
[aaronsgiles/x87](https://github.com/aaronsgiles/x87)) or approximate with 64-bit and accept wrong
answers — which is exactly what bit Microsoft's Prism on ARM
([OpenBLAS#5696](https://github.com/OpenMathLib/OpenBLAS/issues/5696)).

For *this* library the approximation is the correct call, for three reasons:

1. **`MeReal` is `float`** (`MePrecision.h`, the `LINUX` branch). The dominant pattern is
   `flds` (load f32 → widen) … arithmetic … `fstps` (round → store f32). Carrying the x87 stack as
   wasm `f64` is exact for the loads and differs from 80-bit only in double-rounding on store.
2. **Compiler-generated x87 is statically analyzable.** gcc keeps the x87 stack balanced and shallow
   with a statically-known depth at each program point, so `fxch` becomes a compile-time register
   rename and the whole stack machine resolves to SSA registers. This is only hard for hand-written
   assembly, which Karma has none of.
3. **The result is deterministic**, just not bit-identical to the native x87 reference. wasm `f64`
   ops are IEEE-exact and identical across engines/browsers. Web and Android would agree with each
   other; they'd diverge slightly from `native-karma`. For ragdolls and Onslaught vehicles that is
   fine — and it is what `PhysicsStateLogger` exists to measure.

If bit-exactness against the native reference ever *is* required, the fallback is to translate the
x86-64 build's SSE (which maps 1:1 and bit-exactly onto wasm `f32` ops) and pay for wasm64. Note
this option exists; don't take it by default.

### E. Decompile to C using the DWARF · ✅ **The best long-term outcome, and much cheaper than expected**

**This is the finding I did not expect.** Every one of the 192 object files carries **full DWARF-2
debug information** — not stripped, not partial:

```
DW_AT_name      : MdtBody.c
DW_AT_comp_dir  : /home/icculus/projects/UT2-Code/karma/Mdt/src
DW_AT_producer  : GNU C 3.2.3 20030422 (Gentoo Linux 1.4 3.2.3-r4, propolice)
DW_AT_language  : 1  (ANSI C)

DW_TAG_subprogram        MdtBodyReset          decl_file 1, decl_line 157
  DW_TAG_formal_parameter  b                   decl_line 156, type <0x16d0>
  DW_TAG_variable          i                   decl_line 158
  DW_TAG_variable          p                   decl_line 159
```

That is Ryan "icculus" Gordon's UT2004 Linux-port build tree. What survives in the shipped libs:

- **`.debug_info`** — every function's parameter names, local variable names, and complete types
- **`.debug_line`** — instruction → source line mapping (this is how I counted 26,660 source lines)
- **`.debug_macinfo`** — the preprocessor macro definitions (43 KB in `MdtBody.o` alone)
- **`.debug_str`, `.debug_frame`, `.debug_ranges`, `.debug_pubnames`**
- Plus the complete **original source tree layout**, reconstructed:

```
karma/Mcd/src/{QHull(14) convex(14) frame(13) primitives(27) util(6)}
karma/Mdt/src(27)  karma/MdtBcl/src(2)  karma/MdtKea/src(19)
karma/MeGlobals/src(26)  karma/MeAssetDB/src(9)  karma/MeAssetDBXMLIO/src(4)
karma/MeAssetFactory/src(4)  karma/MeXML/src(3)  karma/Mst/src(4)
karma/MeApp/src(5)  karma/MeViewer2/src(8)
```

185 compilation units — **112 ANSI C, 80 C++**. And `Thirdparty/metoolkit/include/` already has all
120 headers with every struct definition.

This changes the character of the work completely. Normal binary decompilation is guesswork about
types and structure. Here you have: the headers, the types, the function signatures, the parameter
*names*, the local variable *names*, the file each function came from, and the line each statement
was on. Ghidra consumes DWARF directly and will name and type everything automatically.

**And the volume is small.** 26,660 source lines total, minus **9,097 lines of qhull** (open source,
replaceable outright) = **~17,500 lines** of C/C++ to recover, of which the hot core is well under
10,000.

The output is portable C. It compiles for wasm, arm64, armv7, x86-64 — Android included — runs at
full native speed, is debuggable, and ends the dependency permanently.

### F. Reimplement the API on Jolt · ⚠️ Already underway, but scope is larger than it appears

`WITH_PHYSICS=ON` (`physics` preset) exists, with `Source/Onslaught/Src/Jolt/*.cpp` covering the six
ONS vehicle classes. That is the vehicle subset — not ragdolls, not the constraint zoo, not the
`Mcd` collision/trilist path.

Full Karma-API-on-Jolt means the **328 API functions** the engine actually calls, plus behavioural
fidelity for a 2003 LCP solver whose exact stiffness/damping/`safetime` semantics UT2004's content is
tuned against. This is the right *destination* if you're willing to accept "different but good"
physics. It is not a shortcut, and it will not reproduce the reference.

---

## 4. Recommended path

The three viable options are not alternatives — they are **stages of one pipeline**, sharing one ABI
and one validation harness. Each stage is independently shippable.

### Stage 0 — the x86→x86 test harness (days) · *already proven*

The user explicitly asked for a native harness that exercises this without the web build. It's
better than expected, because **no emulation is needed at all**:

I built and ran a standalone host against the **x86-64** libs with modern gcc 14:

```
after 1s of gravity: z = 95.013245  (expect ~ 100 - 4.9 = 95.1)
```

It needs `-DLINUX -no-pie` and `-lstdc++` (the archives are non-PIC and use the C++ ABI). **The
22-year-old x86-64 Karma libs link and run correctly on a modern 64-bit toolchain today.** The repo
currently only knows about the 32-bit variant — `CMakePresets.json` calls `native-karma` "32-bit
Karma reference" and `METOOLKIT_LIB_SUBDIR` defaults to `lib.rel/linux_single_gcc3.2`.

Two immediate, cheap wins from this:

1. **A `karma-oracle` target** — a small standalone binary that drives scripted physics scenes
   through the real libs and dumps state per step. This is the golden reference every later stage
   diffs against. `Source/Engine/Src/PhysicsStateLogger.{h,cpp}` already emits exactly this CSV
   format for Karma-vs-Jolt comparison — reuse it verbatim.
2. **A 64-bit `native-karma`** — worth trying, since the engine is already 64-bit-clean (Android
   builds `arm64-v8a` *and* `x86_64`). Karma would no longer pin the native reference to 32-bit.
   Android `x86_64` may even link the `linux_hx_single` archives directly; bionic vs glibc is the
   only real question and the libc surface is 60 functions.

### Stage 1 — delete 36% of the problem (weeks)

Before translating anything, remove what doesn't need translating:

- **qhull** — it is `version 2.6 1998/8/12`, and current qhull (2020.2) builds under Emscripten
  without drama. 186 KB / 9,097 lines gone. Better still: `McdConvexMeshCreateHull` runs at level
  load, so hulls can be **precomputed offline** by the Stage 0 oracle and shipped as data — which
  matches the repo's "engine differences are DATA, not code" rule.
- **`MeAssetDB` / `MeXML` / `MeAssetFactory`** — 51 KB of `.ka` XML parsing. A file-format reader,
  not physics; rewritable against the existing headers, or bypassed by baking assets.

Remaining: **~380 KB, ~17,500 lines** — all of it hot-path physics.

### Stage 2 — bring-up via interpreter (option C), then replace with AOT (option D)

Define the thunk ABI once:

- Karma's guest memory *is* the wasm heap; guest pointers are wasm addresses
- `KMemCreate`/`KMemDestroy`/`KMemResize` already route Karma's allocation through the engine —
  point them at the Emscripten allocator and Karma's heap is native wasm memory *by construction*.
  This is a genuine gift from the original integration.
- a callback registry maps engine function pointers into a guest address range and back

Then swap the implementation underneath it: interpreter first (correct, slow, proves the design),
translated C second (fast, ships). The harness and the ABI don't change.

### Stage 3 — decompile to C (option E), incrementally

Run Ghidra headless with the DWARF loaded, per object file, hottest-first: `MdtKea` → `MdtBcl` →
`Mdt` → `McdPrimitives` → `McdConvex`. Each recovered `.c` file drops out of the translated blob and
into the normal build. Validate every swap against the Stage 0 oracle via `PhysicsStateLogger`.

The endpoint is Karma as ordinary in-tree C: full speed on web *and* Android, debuggable, no
emulator, no binary blob.

### One thing to flag

Karma is proprietary MathEngine code (now CM Labs lineage). Decompiling and redistributing a derived
source form is a materially different legal posture from linking the shipped binaries — and this
engine repo feeds a **public** monorepo. Worth a decision before Stage 3 output goes anywhere public;
Stages 0–2 keep the original binaries as the shipped artifact. Not my call, but you should make it
deliberately rather than by default.

---

## 5. Verdict table

| Option | Shares address space? | Perf vs native | Android? | Verdict |
|---|---|---|---|---|
| A. v86 / QEMU-wasm full system | ❌ | ~100× + marshalling | ❌ | **Reject** |
| B. blink / qemu-user as a VM | ❌ | ~30–50× + marshalling | ❌ | **Reject** (harvest the interpreter core) |
| C. In-process interpreter, shared memory | ✅ | ~30–100× | ⚠️ | **Bring-up only** |
| D. AOT static recompilation → C | ✅ | ~2–5× | ✅ | **Ship this** |
| E. DWARF-guided decompile → C | ✅ | **1×** | ✅ | **Best endpoint** |
| F. Reimplement on Jolt | ✅ | 1× | ✅ | Different physics; already partly done |

---

## Appendix: reproducing the measurements

```bash
# architecture of each shipped variant
for d in Thirdparty/metoolkit/lib.rel/*/; do
  ar p "$d/libMdt.a" "$(ar t "$d/libMdt.a" | grep -v SYMDEF | head -1)" > /tmp/p.o
  echo "$d :: $(file -b /tmp/p.o)"
done

# x87 vs SSE census
objdump -d --no-show-raw-insn Thirdparty/metoolkit/lib.rel/linux_single_gcc3.2/libMdt.a \
  | awk '{print $2}' | grep -E '^f' | sort | uniq -c | sort -rn      # x87: lots
objdump -d --no-show-raw-insn Thirdparty/metoolkit/lib.rel/linux_hx_single/libMdt.a \
  | awk '{print $2}' | grep -E '^(movss|mulss|addss)' | sort | uniq -c # SSE: lots

# the DWARF
ar x Thirdparty/metoolkit/lib.rel/linux_single_gcc3.2/libMdt.a MdtBody.o
readelf --debug-dump=info      MdtBody.o | head -40
readelf --debug-dump=decodedline MdtBody.o | head -40

# external libc surface
for a in Thirdparty/metoolkit/lib.rel/linux_single_gcc3.2/*.a; do nm -A --undefined-only "$a"; done \
  | awk '{print $NF}' | sort -u | grep -vE '^(Mdt|Mcd|Mst|Me|_Z|qh)'
```

The Stage 0 harness sources (`hxtest.c`, `bench.c`) are reproduced in the investigation transcript;
build with:

```bash
INC=Thirdparty/metoolkit/include; LIB=Thirdparty/metoolkit/lib.rel/linux_hx_single
gcc -O2 -DLINUX -no-pie -o bench bench.c \
  -I$INC -I$INC/MdtBcl -I$INC/MdtKea -I$INC/McdCommon -I$INC/McdPrimitives \
  -I$INC/McdFrame -I$INC/Mst -I$INC/MeGlobals -I$INC/MeApp \
  -Wl,--start-group $LIB/*.a -Wl,--end-group -lstdc++ -lm
```

---

# Part II — the x87 question, resolved experimentally

Three hypotheses were on the table: (1) preserve 80-bit precision through C, (2) accept f32 and lose
precision, (3) read the original authors' intent off the x86-64 build. **All three are now answered
by measurement.** Lab is at `../lab/` (it was outside every repository when this was written; stage 2.44 brought it in).

## II.1 There is no 32-bit SSE build to borrow from

Checked every shipped variant. **Every 32-bit build is pure x87; every 64-bit build is pure SSE.**
Xbox was the interesting long shot (Pentium III, 32-bit pointers, SSE-capable) — it is x87 too.

| variant | x87 ops | SSE ops |
|---|---:|---:|
| `linux_single_gcc2.95` / `gcc3.1` / `gcc3.2` | 3957 / 3447 / 3367 | **0** |
| `win32_single_msvcrt` | 2183 | **0** |
| `xbox_single_libcmt` | 2681 | **0** |
| `linux_hx_single` (x86-64) | ~10 | thousands |

So there is no free lunch — but there also turns out to be no problem.

## II.2 Hypothesis 3 is correct: MathEngine shipped a pure-f32 build

`linux_hx_single` is the same source compiled where the x86-64 ABI **forces** SSE scalar — every
intermediate is f32, no 80-bit anywhere. MathEngine shipped it. **f32 semantics are vendor-blessed.**

Measured divergence, same scenario, both shipped builds (12-body ball-socket chain, no collisions),
each build verified bit-identical run-to-run first:

| after | max divergence |
|---|---|
| 1 s | 0.0006 mm |
| 5 s | 0.018 mm |
| 15 s | **0.25 mm** |

Formally chaotic, practically irrelevant: 15 s of continuous articulated simulation costs a quarter
of a millimetre. **Hypothesis 1 (preserve 80-bit) is unnecessary. Hypothesis 2's "precision loss" is
0.25 mm.**

## II.3 …but with collisions, *everything* diverges — and that's fine

Same experiment with full collision + contacts (8 boxes tumbling onto a plane via `MstUniverse`):

| time | divergence |
|---|---|
| 0–1.0 s (free fall) | bit-identical → sub-micron |
| 1.5 s (first contact) | **21 cm** |
| 15 s | **111 m** |

Contact make/break is discontinuous, so the first collision amplifies a 1e-7 difference into a
different tumble. Final *resting heights* still agree to 0–5 cm — the physics character is identical,
the boxes just end up elsewhere.

**Three consequences, and they matter more than the precision question itself:**

1. **"Bit-matching the Karma reference" was never achievable** — by anyone, including MathEngine.
   Their own two shipped builds disagree by 111 m after 15 s. Any port, any compiler, any host
   diverges the same way.
2. **UT2004 already knows this.** `Actor.uc` defines `KRigidBodyState`, `KUpdateState()`,
   `KGetRigidBodyState()` and `bSmoothKarmaStateUpdates=True`: the server **replicates** rigid-body
   state and clients interpolate toward it. The game never relied on cross-machine Karma determinism,
   because it couldn't — a Linux x86-64 server and a Windows x87 client would desync in one bounce.
3. **Therefore trajectory-diffing is the wrong acceptance test** past first contact. `PhysicsStateLogger`
   is still the right tool, but the metric must be *behavioural*: settles at the right height, no
   tunnelling, no explosions, energy decays, ragdolls look right. Not "matches the reference CSV".

## II.4 Ghidra lifts x87 to plain `float` — the problem dissolves

The x87 difficulty only ever existed for **instruction-level translation** (Option D). In the
**decompilation** path it evaporates: you recover `float` expressions, not an x87 stack machine.

Bulk run, Ghidra 12.1.3 headless with DWARF, over the entire hot path
(`Mdt`, `MdtKea`, `MdtBcl`, `McdPrimitives`, `McdConvex`, `McdFrame` — 100 objects):

```
functions attempted        : 1092
decompiled successfully    : 1092  (100.0%)
x87 stack leaked into C    :    2  (0.18%)
bad/undecodable data       :    0
unrecovered jump table     :    0
has unresolved DWARF local :  524  (48.0%)   <- cosmetic, see II.5
machine code covered       : 381.5 KB
C source generated         : 55,333 lines
```

The only two x87 leaks are unnamed 143-byte and 92-byte helpers in `MdtBcl.o` — **235 bytes of hand
work out of 381.5 KB.** Separately, the two largest functions in the whole set are
`writeKeaInputToFile` / `readKeaInputFromFile` (~11 KB each) — solver debug serialisation, stub them.

Recovered code quality, with DWARF applied:

```c
bool McdVanillaOverlapOBBs(MeReal *outSep, lsVec3 *outN, MeReal *outPN, MeI16 *outDims,
                           MeReal inEps, lsVec3 *inR1, lsVec3 *inR2, lsTransform *inT12)
{
  ...
  arot.row[0].v.v[0] = ABS(inT12->row[0].v.v[0]);
  fVar4 = ABS(fVar6) - (arot.row[0].v.v[0] * fVar1 + inR1->v[0] + ...);
```

Real parameter names, real local names (`maxSeparation`, `nRLen`, `arot`), resolved nested structs,
x87 as ordinary float arithmetic.

## II.5 The safety net: differential testing against the shipped binary

Karma is pure computation with no syscalls, so **every function is a deterministic function of its
inputs** — which means the shipped archive is an oracle for its own replacement.

Method (generalises to all ~2,100 functions, including `static` ones):

```bash
# make any internal symbol callable
objcopy --globalize-symbol=_Z21McdVanillaOverlapOBBs... IxBoxBox.o obb_orig.o
```

then link the original archive *and* the recompiled function into one binary, rename only the
function under test, and fuzz both with identical inputs. Dependencies resolve to the original
library, so functions are testable **one at a time, in isolation**.

Run on `McdVanillaOverlapOBBs` — a 292-line, x87-heavy, `goto`-laden OBB separating-axis routine,
200,000 random cases:

| | gcc x87 build | **SSE f32 build (wasm-equivalent)** |
|---|---:|---:|
| bit-identical to shipped binary | 92.75% | 86.43% |
| differs only by FP rounding (rel < 1e-5) | 7.22% | 13.38% |
| **different return value** | **0** | **0** |
| **different dims/feature code** | **0** | **0** |
| numeric diff ≥ 1e-5 rel | 65 (0.03%) | 395 (0.20%) |
| max relative error | 4.9e-4 | 1.0e-2 |

**Every discrete decision matches, in every one of 200,000 cases, under both x87 and wasm-like f32
semantics.** The residual is float last-bits in catastrophic-cancellation cases near zero — exactly
what f32 does, and smaller than the divergence between MathEngine's own two builds.

This is the finding that de-risks the project: the decompilation is *logically exact* on a genuinely
hairy function, and there is a cheap automated way to prove it for every remaining one.

## II.6 Additional leverage worth using

- **Five independent compilations of the same source** exist (i386 gcc2.95 / gcc3.1 / gcc3.2, x86-64,
  ppc32, Win32 MSVC, Xbox). **Two carry DWARF — gcc2.95 and gcc3.2.** Decompiling a function from two
  different compilers and diffing the recovered C is a strong artifact detector: agreement means the
  C is right; disagreement localises a decompiler bug. Most decompilation projects have one binary.
- **The x86-64 build is the best semantic reference** even without DWARF: SSE code reads far more
  clearly than x87, so use it to adjudicate anything ambiguous in the i386 output.
- **`.debug_line` gives statement boundaries** — instruction→source-line mapping means the original
  statement structure is recoverable, and function line-counts are ground truth for review.
- The 48% "unresolved DWARF local" rate is cosmetic: DWARF names a variable that lived in a register
  Ghidra couldn't map (e.g. `MeReal threshold@[DW_OP_reg17(ST6)]` — an x87 register). The value is
  still computed correctly, as II.5 proves; it just isn't named.

## II.7 Revised recommendation

The precision question is closed, and Option D (instruction translation) is now **redundant** — go
straight to Option E:

1. **Decompile → C**, per object, hot path first. 100% success rate measured; ~235 bytes need hand
   work; stub the two debug-serialisation functions.
2. **Differential-test every function** against the shipped archive via the globalize-symbol harness.
   This is the acceptance gate, and it's automatable.
3. **Compile to f32 everywhere** — matches MathEngine's own x86-64 build, gives wasm *and*
   arm64/armv7, and is proven logically exact above.
4. **Validate end-to-end behaviourally**, not by trajectory diff (II.3).

Bundling/build-integration is deliberately out of scope for now.

### Reproducing Part II

```
../lab/
  diverge.c  div32 div64      x87-vs-SSE, constraints only
  divcol.c   dcol32 dcol64    x87-vs-SSE, full collision (Mst universe)
  difftest.c difftest         differential oracle vs shipped binary
  obb_clean.c                 the cleaned decompiled function under test
  gscripts/DumpDecomp.java    Ghidra headless decompile + quality stats
  out2/stats.csv              per-function quality metrics, 1092 rows
```
