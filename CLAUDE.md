# metoolkit-karma-decomp — agent guide

**What this is:** MathEngine's **Karma 1.2** physics SDK as UT2004 shipped it, plus a
source-level recovery of it. Karma shipped as binary-only 32-bit x86 archives, which is why
UT2004's web and Android ports had no vehicles and no ragdolls. `metoolkit_decomp/` is 145 of
those objects recovered to portable C.

```
metoolkit/              the v3369 SDK drop, verbatim — the ORACLE, never edit
metoolkit_decomp/       the product: include/ + src/<Lib>/ + a CMake package
metoolkit_decomp_dummy/ empty placeholders, for building with physics off
decomp/                 the recovery toolchain, its gates, and its written record
lab/                    the Ghidra dumps + the 153 shipped objects the tools read
```

## Start here (the standard loop)

1. Read this file → it points at `decomp/`.
2. **`decomp/docs/STATE.md` first** — where the work is, what is decided, what is a trap.
   Then `decomp/HANDOVER.md` (§4 is the nine-gate block) and `decomp/proven.txt`.
3. Make the change **and its docs together**.
4. Run the gates: `decomp/test/run-standalone.sh`. Anything touching `metoolkit_decomp/`
   needs the consumer re-validated too — see "The web is downstream" below.
5. Update `decomp/docs/STATE.md`.

## The rules

- **Never hand-edit `metoolkit_decomp/src/`.** Those are generated. Every fix goes into the
  generator (`decomp/tools/ghidra_clean.py`) and the tree is regenerated. Every object recovered
  in this project came from fixing the generator and re-running, never from editing output —
  and a hand edit is silently lost on the next regeneration with no gate to notice.
- **Never edit `metoolkit/`.** It is the yardstick. The one existing modification
  (`MeCall.h`'s `ME_FORCE_CDECL` guard) is documented in `metoolkit/README.md`.
- **`proven.txt` is the ledger, and its format is load-bearing.** The first word of any
  NON-COMMENT line RELEASES an object from quarantine. Prose must be commented. The count
  must stay at 30: `grep -vcE '^\s*#|^\s*$' decomp/proven.txt`.
- **Paths come from the shim, never from `..` arithmetic.** Shell sources
  `decomp/lib/kd-paths.sh`; Python imports `decomp/tools/kd_paths.py`. Both find the root by
  walking up for a marker, so a file can move without breaking. Anything needing UT2004 calls
  `kd_require_ut2004` and fails loudly rather than guessing.
- **A harness that needs the game lives in `decomp/test/ut2004/`, everything else in
  `decomp/test/standalone/`.** The standalone tier must run from a bare clone with no
  environment set; that is what CI checks.

## The traps that break SILENTLY

These have each cost this project real time. They are not hypothetical.

- **Byte-identity cannot validate a TYPE or a pointer-width cast.** `kd_uptr` *is*
  `unsigned int` at i386 and `sizeof(T)` folds to the same literal, so a wrongly-chosen type
  compiles byte-identically. Seven had shipped before a gate existed that could see them.
  145/145 is necessary and never sufficient; `lp64_pipeline.sh` is the other half.
- **The i386 acceptance test needs the file to keep its NAME.** gcc records the source basename
  in an `STT_FILE` symbol, so compiling identical text from `/tmp/t.c` yields a different object.
- **Split from `/tmp/kd_lp64`, not `/tmp/kd_out`.** The raw recovery encodes i386 struct
  *layouts*. Using it is not a visible mistake: it builds, every 32-bit target is byte-identical,
  every i386 gate passes, and only the 64-bit ABIs are wrong.
- **`--build-dir` is not optional.** It defaults to `/tmp/kd_build`, which is the baseline the
  acceptance test compares against — a run that forgets it overwrites what it was about to be
  measured against.
- **`out14` and `kd_protos11.h` are a PAIR.** Mixing generations gets `McdSpace` wrong in
  *opposite* directions.
- **`KD_OUT` means different trees in different harnesses** — `/tmp/kd_lp64` in the ones that
  exercise the post-passes, `/tmp/kd_out` in the ones that read the raw recovery. The shim
  deliberately does not default it. See the comment in `decomp/lib/kd-paths.sh`.
- **`scene_ragdoll`'s ASan verdict is measured-unstable** — BLOWN UP 3 of 5 at LP64 and 5 of 5
  at i386, with zero sanitizer errors either way; the scene sits on its `escaped > 1e3`
  threshold and ASan's layout tips it. A single `-> FAIL` from `lp64_pipeline.sh` is not a
  result. Re-run, and read the no-sanitizer section. `proven.txt` `LP64-THREE-CLEAN`.
- **Read the per-object lines, never a summary.** `substitute_test.sh`'s
  "substituted and ran cleanly : 140" is a CRASH count, not bit-identity — `pass` increments in
  all three branches including `[ diverg]`. This project quoted it as bit-identity once and a
  bisect had to catch it.
- **Per-object gates are blind to integration defects.** `difftest_pair.sh` substitutes the
  interaction object and links the GEOMETRY objects from the shipped library, so a broken
  `McdSphereGetRadius` is never in the loop. "145 objects pass" is a statement about 145
  separate links, not about the program. `ktrace` is the gate that compares the trajectory the
  calls add up to.

## The web is downstream

`metoolkit_decomp/` is what `engine-ut2004` compiles for its wasm build, which ships. A change
here is a change to the browser artifact even when every i386 object is byte-identical —
`sizeof(void *)` is 4 on wasm32 so rewritten expressions fold back to the constants they
replaced, but that is an argument, not a measurement. The standing rule is that a shared or
web-facing change needs the browser E2E re-run and re-stamped before it is called safe.

`engine-ut2004` consumes this repository via FetchContent at `main`, unpinned.

⚠ **A wasm32 A/B MUST use the flags the product uses** — `-std=gnu99 -fno-strict-aliasing`
(`metoolkit_decomp/CMakeLists.txt`'s `MD_C_FLAGS`), plus `-O2 -g0` so DWARF line numbers cannot
be what differs. The recovery reinterprets storage the way the original machine code did, so a
**strict-aliasing** probe is not compiling the thing that ships: it reported `McdAggregate.c` as
one byte difference on a change that is byte-identical under the real flags.

## Build / test

```bash
# the whole recovery, from a bare clone, no environment needed
python3 decomp/tools/recover.py --out-dir /tmp/kd_out --build-dir /tmp/kd_build   # ~95 s
./decomp/test/standalone/lp64_pipeline.sh                                        # post-passes + gates
python3 decomp/tools/split_libraries.py /tmp/kd_lp64/allobj                       # refresh the product

./decomp/test/run-standalone.sh          # every gate that needs nothing but this repo
./decomp/test/run-ut2004.sh              # the rest; needs UT2004_* set
```

`decomp/tools/README.md` and `decomp/test/README.md` document every tool and harness with a
*run* command. `decomp/docs/STATE.md` is the running record.
