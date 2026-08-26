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

```bash
cd karma-decomp
rm -rf /tmp/kd_out /tmp/kd_build
python3 tools/recover.py \
  --dump-dir /home/ion/tools/karma-lab/out14 \
  --obj-dir  /home/ion/tools/karma-lab/allobj \
  --out-dir  /tmp/kd_out --build-dir /tmp/kd_build \
  --metoolkit ../Thirdparty/metoolkit \
  --protos   /home/ion/tools/karma-lab/kd_protos11.h        # ~95 s

# then refresh this directory with exactly the objects that PASSED
rm -f generated/allobj/*.c
for o in /tmp/kd_build/*.o; do cp "/tmp/kd_out/allobj/$(basename "$o" .o).c" generated/allobj/; done
```

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
