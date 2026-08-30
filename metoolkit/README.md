# `metoolkit/` — the MathEngine Karma SDK, as UT2004 shipped it

The **v3369 drop** lifted verbatim out of `engine-ut2004/Thirdparty/metoolkit`. This is the exact
SDK the recovery is measured against: every object in `lab/allobj/` is a byte-identical member of
`lib.rel/linux_single_gcc3.2/*.a`, so this directory is the oracle, not a convenience copy.

**MathEngine Karma 1.2.1, build 631** (`include/MeVersion.h`). Later acquired by Criterion and
rebranded RenderWare Physics.

## `lib.rel` vs `lib.chk`

Two build *configurations* of the same sources, in MathEngine's Xbox-XDK naming:

- **`lib.rel` — release.** What UT2004 links, and what the recovery targets.
- **`lib.chk` — checked.** Compiled with `_MECHECK` defined, so `MdtCheckMacros.h` and `MeAssert.h`
  expand to real validation. Measurably fatter and it pulls in `__assert_fail` + `MeWarning`:
  `MdtWorld.o` is 15,736 bytes checked against 9,876 release.

They are not different libraries, so `metoolkit_decomp/` does **not** reproduce the split — the
recovered code ships as source, and a consumer that wants the checked behaviour defines `_MECHECK`.

## Configurations in this drop

```
linux_single_gcc2.95   linux_single_gcc3.1   linux_single_gcc3.2   linux_hx_single
macos_single           win32_single_msvcrt   xbox_single_libcmt
win_amd64_single       win_amd64_whidbey_single
```

Two of these carry the whole project:

- **`lib.rel/linux_single_gcc3.2`** — what `engine-ut2004` links, built by gcc 3.2.3 (Gentoo 1.4)
  **with `-g3`**. It is the *only* config in the drop carrying DWARF, which is why the recovery
  starts here and nowhere else. That is also why it is ~10× the size of the others.
- **`lib.rel/win_amd64_single`** — a genuine 64-bit build of the same sources, and therefore the
  **LP64 oracle**: it answers "what does this struct look like at 64-bit pointer width" from
  MathEngine's own compiler rather than from inference. It is LLP64, not LP64 — see
  `decomp/proven.txt` `LP64-BAKED-SIZES`. Read by `decomp/tools/amd64_oracle.py`.

## ⚠ This is not interchangeable with other Karma 1.2 drops

`sigmaco/metoolkit-karma-v1.2` preserves a different 1.2.1 build-631 drop. Compared byte-for-byte
on 2026-08-30:

| | |
|---|---|
| byte-identical | `lib.rel/linux_single_gcc2.95`, `lib.{rel,chk}/linux_single_gcc3.1` |
| differ | `lib.chk/linux_single_gcc2.95`, `macos_single`, `win32_single_msvcrt`, `xbox_single_libcmt` |
| **absent there** | `linux_single_gcc3.2`, `linux_hx_single`, `win_amd64_single`, `win_amd64_whidbey_single` |
| `include/` | same sources; 115 of 120 differ only by CVS `$Name`/`$Date`/`$Revision` and CRLF. The 5 real deltas all favour this copy — `MePrecision.h` has the LINUX `uintptr_t` and MACOSX blocks, `MeMath.h` has the PS2 inline asm in a form modern gcc accepts |

The two configs that matter most are the two that are missing there. Swapping drops would mean
redoing the recovery against different binaries.

## The one local modification

`include/MeCall.h` carries an `ME_FORCE_CDECL` guard — the only edit to the vendor drop, and it is
load-bearing rather than cosmetic. MinGW-w64 predefines `WIN32`, which would make MathEngine's
header declare the entire API `__stdcall`; the recovered sources were decompiled from the Linux
i386 build and are **cdecl**, so a Windows build would have the engine's C++ call them one way and
have them return the other — stack corruption on every Karma call, with nothing to diagnose it.
The guard is `#if defined(WIN32) && !defined(ME_FORCE_CDECL)`, and the consumer defines
`ME_FORCE_CDECL` alongside the recovered library. The comment in the file has the full reasoning.

Everything else here is untouched, `vssver.scc` files and all.
