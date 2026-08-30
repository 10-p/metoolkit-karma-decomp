# `lab/` — the Ghidra working set the recovery reads

Everything `decomp/tools/recover.py` needs that is **not** produced by this repo. Until 2026-08-30
this lived at `/home/ion/tools/karma-lab`, outside every repository, and a disk failure would have
ended the project. It is here now.

Nothing in this directory is authored by hand. It is Ghidra output plus the shipped objects Ghidra
was pointed at — but it is **not reproducible on demand** either: regenerating it means re-running
the Ghidra headless analyser over 153 objects with the scripts in `gscripts/`, which takes hours and
is sensitive to the Ghidra version (`ghidra_12.1.3_PUBLIC`).

## What the pipeline actually consumes

| path | what it is | consumed by |
|---|---|---|
| `allobj/` | **153 shipped `.o`** — byte-identical members of `metoolkit/lib.rel/linux_single_gcc3.2/*.a`. The DWARF in them is the whole reason this recovery was possible; no other config in the drop carries it. | `recover.py --obj-dir`, every gate that compares against the original |
| `out14/` | the **current** Ghidra dump — 307 files, a `<obj>.o.c` and `<obj>.o.locals` per object | `recover.py --dump-dir` |
| `kd_protos11.h` | the prototype header handed **to** Ghidra so it types call sites correctly | `recover.py --protos` |
| `gscripts/` | the four Ghidra headless scripts that produced the dumps: `DumpDecomp.java`, `EnableParamID.java`, `ParseKarmaHeaders.java`, `TryStyles.java` | regenerating a dump |

## ⚠ `out14` and `kd_protos11.h` go together

Mixing either with an older pair gets `McdSpace` wrong **in opposite directions** — see
`decomp/proven.txt`. The pairing is:

```
out14 ↔ kd_protos11.h      ← current, the only pair you should use
out13 ↔ kd_protos10.h
out9  ↔ kd_protos9.h
```

`out5`–`out13` are kept **deliberately**, as a bisect fallback for when a recovery result changes
and it is not obvious whether the dump or the tooling moved. They are not dead weight; delete them
and that bisect becomes impossible.

## Everything else

`sub/`, `hot/`, `ovr/`, `exp*/`, `gproj*/`, `tri/`, `dcol*/`, `*.csv`, `*.log`, `obb_*.c`,
`difftest.c`, `divcol.c`, `diverge.c` are per-investigation scratch — the working set of specific
defects (`proven.txt` names them where they mattered). Kept because the reasoning in `HANDOVER.md`
refers to them, not because anything runs them.

The compiled i386 test binaries that used to sit at this level (`dcol32`, `difftest_boxbox`, `div32`
and five others, ~19 MB) are **gitignored on purpose**: nothing references them, and their sources
are tracked here. Rebuild from the `.c` if you need one.

## Where it is used from

`decomp` resolves this directory as `$KD_LAB_DIR`, defaulting to `<repo>/lab`. Every tool and
harness takes the override; see `decomp/README.md`.
