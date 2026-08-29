# `generated/` — the recovered Karma sources, checked in on purpose

**These 145 `.c` files are build artefacts, and they are in git deliberately.** Without them a
clone cannot build the `wasm-karmadecomp-*` presets: they are produced by `tools/recover.py`
from Ghidra dumps that live **outside this repository** (`/home/ion/tools/karma-lab/out14`,
~1 GB), and nobody downloading the repo has those. 3.1 MB of C is a much smaller thing to
carry than an unbuildable preset.

They are **not** to be hand-edited. Every fix goes into the GENERATOR — see
`../HANDOVER.md` §4 and the "What has been working" section: every object recovered in this
project came from fixing `tools/ghidra_clean.py` and re-running, never from editing output.
A hand edit here is silently lost on the next regeneration and there is no gate that would
notice.

## Regenerating them

★ **THESE ARE `recover.py` PLUS THE LP64 POST-PASSES, and that changed on 2026-08-29.** They used
to be the raw `recover.py` output, which encodes i386 struct LAYOUTS — fine for wasm32, i686 and
armeabi-v7a, and somebody else's memory on arm64 and x86_64. The post-passes in `../tools/fix_*.py`
are what make the same text correct at both widths, and they are no-ops at 32-bit pointer width
**by measurement**: all 145 objects compile byte-identical at `-m32` before and after.

```bash
cd karma-decomp
rm -rf /tmp/kd_out /tmp/kd_build
python3 tools/recover.py \
  --dump-dir /home/ion/tools/karma-lab/out14 \
  --obj-dir  /home/ion/tools/karma-lab/allobj \
  --out-dir  /tmp/kd_out --build-dir /tmp/kd_build \
  --metoolkit ../Thirdparty/metoolkit \
  --protos   /home/ion/tools/karma-lab/kd_protos11.h        # ~95 s

# the post-passes, the i386 acceptance test and the LP64 harness, in order
./test/lp64_pipeline.sh                                     # must end `-> PASS`

# then refresh this directory from the POST-PASSED tree
rm -f generated/allobj/*.c
for o in /tmp/kd_build/*.o; do cp "/tmp/kd_lp64/allobj/$(basename "$o" .o).c" generated/allobj/; done
```

⚠ **`/tmp/kd_lp64`, not `/tmp/kd_out`.** Copying the raw tree here is not a visible mistake: it
builds, every 32-bit target is byte-identical, every gate that runs at i386 passes, and only the
64-bit ABIs are wrong. `lp64_pipeline.sh`'s own acceptance test catches the reverse mistake — a
post-pass that is NOT a no-op at i386.

⚠ **AND THIS DIRECTORY IS WHAT THE WEB BUILDS.** The `wasm-karmadecomp-*` presets compile these
files, so a change here is a change to the browser build even when the i386 objects are identical.
`sizeof(void *)` is 4 on wasm32, so every rewritten expression folds back to the constant it
replaced — but that is an argument, not a measurement, and the owner's standing rule is that a
shared/web-code change needs the verify + re-stamp gate before it is called safe.

`out14` and `kd_protos11.h` **go together** — mixing either with the older pair gets `McdSpace`
wrong in opposite directions (`../proven.txt`).

## What is here and what is not

* **145 `.c` files** — exactly the objects `recover.py` accepted into the build. The three it
  holds (`MeASELoad`, `MeFGeometryFromMesh`, `McduDebugDraw`) are absent, and `../HANDOVER.md`
  §13 shows nothing in the engine can reach them.
* **No headers.** The recovered sources include only `kd_compat.h`, `kd_karma.h` and
  `kd_types.h`, which are real sources in `../include/`. The per-object `*.prelude.h` /
  `*.exports.h` / `*.vtables.h` the pipeline emits are intermediates — they are inlined into
  the `.c` and are not needed to compile.
* **The convex-hull replacement is not here either.** `../src/McdConvexCreateHull/kd_convexhull.c`
  is hand-written, not recovered, and lives with the other real sources.

## The one thing to check after regenerating

The drop-in gap must stay at zero, and the objects must be byte-identical to what was
committed:

```bash
python3 tools/dropin_gap.py ../build-native-karma /tmp/kd_build /home/ion/tools/karma-lab/allobj
```
