# `metoolkit_decomp/` — Karma, recovered, as source you can compile anywhere

This is the deliverable. `decomp/` is the machinery that produced it and you do
not need to read a line of it to use this.

MathEngine shipped Karma as **binary-only 32-bit x86 archives**. That is a hard
ceiling: no amount of build engineering links an i386 ELF `.a` into a WebAssembly
module, an ARM64 Android APK, or a modern 64-bit native binary. UT2004 on any of
those targets therefore shipped with no physics at all — no vehicles, no
ragdolls. These are 145 of those objects recovered to portable C from the DWARF
in `metoolkit/lib.rel/linux_single_gcc3.2`, plus a hand-written convex hull
standing in for qhull. They compile and run wherever a C compiler does.

## Using it

```cmake
include(FetchContent)
FetchContent_Declare(metoolkit_karma_decomp
  GIT_REPOSITORY https://github.com/10-p/metoolkit-karma-decomp.git
  GIT_TAG        main)
FetchContent_MakeAvailable(metoolkit_karma_decomp)

target_link_libraries(YourEngine PRIVATE metoolkit_decomp::all)
```

That one line brings the implementation, Karma's public headers, and
`ME_FORCE_CDECL`. Nothing else is required.

The subdirectory is `metoolkit_decomp/`, so if you vendor rather than fetch:
`add_subdirectory(third_party/metoolkit-karma-decomp/metoolkit_decomp)`.

### Targets

| target | what it is |
|---|---|
| `metoolkit_decomp::all` | **use this.** One static archive, every object |
| `metoolkit_decomp::<Lib>` | one OBJECT library per shipped archive — `McdPrimitives`, `MdtKea`, `MeGlobals`, … |
| `metoolkit_decomp_dummy::all` | the link-line placeholder, no physics — see `../metoolkit_decomp_dummy/` |

### The one thing that will silently corrupt your stack

`ME_FORCE_CDECL` is attached to the targets as an INTERFACE define, so linking
them is enough. It matters because MathEngine's Windows build declared the whole
API `__stdcall` and MinGW-w64 predefines `WIN32`, while **these sources are
cdecl, every one of them** — they were decompiled from the Linux i386 build. A
consumer that gets `__stdcall` declarations calls them one way and they return
the other, which is stack corruption on every Karma call with nothing to
diagnose it. It used to be the consumer's job to remember; now it is impossible
to forget.

## Layout

```
include/     kd_compat.h · kd_karma.h · kd_types.h  — what every source includes
             kd_types_fields.json                   — the field map, tool data,
                                                      emitted beside kd_types.h
src/<Lib>/   the sources, grouped as MathEngine grouped them
library-map.txt   object -> library, generated, do not hand-edit
```

`library-map.txt` is not a judgement call: `decomp/tools/split_libraries.py`
reads it off `ar t` over the shipped archives. All 145 objects map, none appears
in two archives, so there was no tie to break.

```
McdPrimitives 27   Mdt            26   MeGlobals      23   MdtKea 18
McdConvex     13   McdFrame       12   MeAssetDB       7   McdCommon 5
Mst            4   MeAssetDBXMLIO  3   MeAssetFactory  3   MeXML  3
MdtBcl         1   McdConvexCreateHull  (the hand-written hull)
```

**`MeApp` and `MeViewer2` get no target, and that is a finding rather than a
gap.** They are the sample-application and viewer libraries; nothing in UT2004
links them and no object of theirs was ever recovered.

## These sources are generated. Do not edit them.

Every fix goes into the generator — `decomp/tools/ghidra_clean.py` — and the
tree is regenerated. Every object recovered in this project came from fixing the
generator and re-running, never from editing output. A hand edit here is
silently lost on the next regeneration and no gate would notice.

```bash
python3 decomp/tools/recover.py --out-dir /tmp/kd_out --build-dir /tmp/kd_build
./decomp/test/standalone/lp64_pipeline.sh            # must end `-> PASS`
python3 decomp/tools/split_libraries.py /tmp/kd_lp64/allobj
```

⚠ **Split from `/tmp/kd_lp64`, not `/tmp/kd_out`.** The raw recovery encodes
i386 struct *layouts* — not merely 32-bit pointers but the sizes and byte
offsets those pointers produce. Laying out the raw tree here is not a visible
mistake: it builds, every 32-bit target is byte-identical, every gate that runs
at i386 passes, and only the 64-bit ABIs are wrong.

`split_libraries.py --check` verifies the checked-in tree is what the source
tree would produce; it is a gate, and CI runs it.

## What it is measured against

The recovered objects are held to the shipped library, not to a style guide:

- **i386 acceptance** — all 145 recompile **byte-identical** at `-m32` against
  the archive members they came from. That is the gate on every generator change.
- **LP64** — `lp64_pipeline.sh` runs three physics scenes at 64-bit pointer
  width and every trajectory first differs at the float floor (9.0e-10, 1.9e-09,
  2.2e-07 against an i386 control).
- **Live** — a full UT2004 Onslaught match against a metoolkit with every
  shipped member deleted, and the browser build's whole E2E suite.

⚠ **Byte-identity cannot validate a type or a pointer-width cast.** `kd_uptr`
*is* `unsigned int` at i386 and `sizeof(T)` folds to the same literal, so a
wrongly-chosen type still compiles identically. That is why the LP64 harness
exists as a separate gate, and why "145/145 byte-identical" is necessary and
never sufficient. `decomp/proven.txt` records every class of defect that got
past one gate and was caught by another.

## Provenance and licence

Recovered from the Karma 1.2.1 (build 631) SDK as shipped with UT2004 v3369; the
drop it was recovered from is in `../metoolkit/`. This is MathEngine's work,
reconstructed — see the repository root `README.md` for what that means and does
not mean.
