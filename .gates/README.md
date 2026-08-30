# `.gates/` — the checks, why each exists, and how to get past it

Four hard gates and one advisory. Every one of them exists because the thing it
checks has already gone wrong once in this project and produced a **green**
result while being wrong. That is the selection criterion: a gate here catches a
failure that looks like a success.

```bash
./.gates/install.sh          # symlink them into .git/hooks (idempotent)
```

| gate | when | what it catches |
|---|---|---|
| `proven-count.sh` | pre-commit, hard | prose in `proven.txt` that forgot its `#`. The first word of a non-comment line **RELEASES an object from quarantine**, so a stray sentence quietly releases whatever word it starts with. The count must stay at **30**; a real release edits `EXPECT` in the same commit |
| `no-abs-paths.sh` | pre-commit, hard | a new `/home/...` in a tool, harness or run command. The 2.44 relocation removed 230 of them; one new one makes "runs from a bare clone" false again, and nothing else would notice until somebody else tried |
| `product-in-sync.sh` | pre-commit, hard | a hand edit to `metoolkit_decomp/src`, which is **generated**. A hand edit compiles, passes every i386 gate, and vanishes on the next regeneration |
| `doc-sync.sh` | pre-commit, hard | a tool or harness changing without its documented run command. This is how a command in `test/README.md` stops working |
| `conventional-commits.sh` | commit-msg, warn | `type(scope): subject` |

**Bypass is `git commit --no-verify`, and it is a legitimate move** — note why in
the commit body. A gate that cannot be bypassed gets deleted instead.

## What is deliberately NOT a gate

- **The i386 acceptance test** (all 145 objects byte-identical at `-m32`). It
  needs a full recovery run — ~95 s for `recover.py` plus the post-passes — which
  is too slow for a hook and would train people to bypass. It is the first thing
  `lp64_pipeline.sh` does, and CI runs it.
- **The LP64 harness.** Same reason, plus `scene_ragdoll`'s ASan verdict is
  measured-unstable (BLOWN UP 3 of 5 at LP64, 5 of 5 at i386, zero sanitizer
  errors either way). A gate that fails on a coin flip is a gate people learn to
  ignore. `lp64_pipeline.sh` runs the i386 control by default precisely so the
  instability is *attributable* rather than fatal — see the ★ comment in that
  file.
- **Anything needing UT2004.** No hook can assume a game install. That tier is
  `decomp/test/run-ut2004.sh` and it fails loudly rather than guessing a path.

## CI

`.github/workflows/gates.yml` runs the standalone tier on every push: the four
hard gates, then `decomp/test/run-standalone.sh`. It needs `gcc-multilib` (the
whole project is `-m32`) and it does **not** have the Android NDK, so
`ptrwidth_check` reports SKIP there rather than failing — a missing toolchain is
not a defect in this repository.
