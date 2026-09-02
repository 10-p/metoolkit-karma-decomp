# `decomp/docs/census-verdicts.md` — every truncation-census site, with its verdict

`ptrwidth_classify.py` sorts the aarch64 truncation diagnostics into named benign classes and
leaves the rest in `UNEXPLAINED`. **`UNEXPLAINED` means *unclassified*, not *broken*** — the class
rules are deliberately narrow, so a site with no rule lands there whether it is a defect or
obviously fine. That number has been read as "open bugs" often enough that this file exists: it is
a verdict for **every one of the 30**, with the reason, so the only thing left in the column is
work.

⚠ **THE CENSUS IS NOT THE SCOREBOARD FOR THIS PORT, AND 2026-09-02 IS THE PROOF.** The hover
bike's box half-extents, `McdBatchFlattenAggregate`'s element table, `IxBoxTriList`'s fabricated
stack slot and `MdtContactGroup::generator` were all real, all shipped, and all produced **ZERO**
census diagnostics — nothing about them is truncated. The instrument that tracks correctness is
`ktrace` against the SSE-32 control. Read this file as "which diagnostics are noise", nothing more.

★ **AND NONE OF THE 30 IS A MEASURED DIVERGENCE.** With all thirty present the LP64 trace of
`test-karma-1` under Onslaught is byte-identical to the 32-bit control over 40 frames
(md5 `c31ed77b7323`). Two of them are nonetheless real truncations — see the second table.

---

## NON-IMPORTANT — 24 sites, closed

Each row is closed by a rule, not by inspection-and-a-shrug. The three recurring ones:

- **W** — an `int`→pointer cast **WIDENS**. It cannot lose a bit, at any width.
- **LE** — float bits parked in a wider slot and read back through `*(float *)&`. `KD_FBITS`
  returns `unsigned int`, so the bytes sit in the **low four**, which is exactly where a
  little-endian read looks. Every target here is little-endian.
- **ID** — `&p->FIRST_MEMBER` is the identity: offset 0 at both widths, measured.

| site | verdict | why |
|---|---|---|
| `CxSmallSort` 956 / 1044 / 1133 | **W** | `(Link *)pCVar2->mCullingID` — an id widened into a pointer for a comparison. Both sides of the compare go through the identical conversion. |
| `IxConvexTriList` 86 | **W** | `MeSetAdd(&set,(void *)start)` — `start` is an `int` vertex index; `MeDict` takes its key as `void *` and insert and compare use the same conversion. |
| `IxConvexTriList` 343 | **W** | `(float *)((int)(uVar2 & 0x10) >> 2)` — a flag bit (0 or 4) carried in a pointer local and consumed as `(kd_uptr)pfVar21` in an integer OR. |
| `IxCylinderTriList` 296 | measured | `triangleData` is a **union used as a byte cursor** seeded from `(void *)0x0`, so `.tag` holds the whole value. Verified 2026-09-01. |
| `McdContact` 578 | **LE** | `MStack_9c.prev = (_McdContactLink *)KD_FBITS(fVar21)` — float bits into a struct slot, read back as a float. |
| `McdPlaneIntersect` 209 | **W** | the `MeSetAdd` shape above. |
| `MdtConstraint` 608 | **ID** | `bodyindex = (kd_uptr)(paMVar1 + 1)` handed on as `void *` — a pointer widened to `kd_uptr` and back, unchanged. |
| `MdtPartition` 147 / 167 / 179 | **W** | integer arithmetic (`jointCount + 1`, `nPartitions`) carried in a `MeDictNode *` local and converted straight back. |
| `MdtPartition` 143 | **W** + near-miss | `MdtConstraintGetRowCount()` returns an `int`; the one use that looks like a dereference, `(kd_iptr)&pMVar9->left`, is **ID** (`left` is `MeDictNode`'s first member). ⚠ Stays out of `int-return` only because Ghidra reuses `pMVar9` for a real node pointer elsewhere in the function, and a whole-function rule cannot separate two live ranges. |
| `MeFAsset` 206 | **LE** | `KD_FBITS(asset->massScale)` into a local, read back via `*(float *)&`. |
| `MePool` 240 | measured | the union's `fixed.structArray` / `malloc.usedStructs` arms. The shipped amd64 build reads this object at 4, 8 and 12 with **four-byte** accesses (`movslq 0x4(%rax)`, `cmpl 0xc(%rax)`), i.e. the integer arm — so the four-byte write is the right one. |
| `MeXMLParser` 283 / 365 / 447 | **W** | `(ushort *)(int)*pcVar7` — a **character value** in a pointer local, tested against 0. |
| `MeXMLParser` 535 | **W** | `pcVar6 < (char *)action->max` — a loop bound; both operands go through the same widening. |
| `keaDebug` 665 / 684 | **W** + near-miss | `printf` / `kd_putchar` return values in a dead local. ⚠ Same two-live-ranges limitation as `MdtPartition` 143: `piVar15` is a real `int *` sixty lines later. |
| `keaLCPSolver` 421 | **W** | `unclamped[iVar1]` — an **index** in a pointer local, used as `this->x[(kd_iptr)this_00]`. |
| `keaMatrix_tester` 203 / 204 | **unreachable** | ⚠ These genuinely DO truncate — `*(int *)this->suspect` reads a **vtable pointer** four bytes wide. Closed on REACHABILITY, not on width: nothing in the corpus references `keaMatrix_tester`, and neither does the engine (`nm` shows only outgoing `U` symbols; no caller anywhere). It is a dead test harness. ★ If anything ever calls it, this row is a defect again. |

---

## THE WORK — 6 sites

| site | status | evidence |
|---|---|---|
| `McdAggregate` 486 | ✅ **CLOSED** (`fix_narrow_loads` rule D, second shape) | `McdGeometryGetMassProperties((void *)*puVar1, …)` reads `McdAggregateElement::mGeometry` through an `undefined4 *`. The shipped amd64 build: `mov 0x20(%rcx),%rcx` then `mov 0x40(%rcx,%rax,1),%rax` — **eight** bytes. Ours reads four: a truncated geometry pointer. |
| `MstModelDynamics` 150 | ✅ **CLOSED** (`fix_setter_typed_slot` clause 5 now tests WIDTH as well as offset) | `otherModel = (McdModelID)*piVar4` with `piVar4` an `int *`. amd64: `mov (%rax),%rax` — **eight** bytes. ⚠⚠ Left alone earlier by `fix_setter_typed_slot` clause 5, *"the member must actually move"* — which is about OFFSETS. `model1` is byte 0 at both widths and the **WIDTH still changes**. No rule here was asking that question. |
| `McdInteractions` 104 | **unresolved** | `(code *)*puVar4 != 0` — a four-byte read of a function pointer. The oracle declines: `McdHello` uses **both** `mov (%rax),%rax` and `mov (%rax),%eax` at that displacement, so it cannot say which this site is. |
| `MdtLOD` 224 | open | `*(MdtBaseConstraint **)local_6c.rowCount` — an `int` field used as an **address**. |
| `MdtLOD` 517 | open | `local_6c.conArray = (MdtBaseConstraint **)local_6c.bodyCount` — same shape. |
| `IxCylinderTriList` 148 | open | `kd_slot_MStack_26c_flags = (McdTriangleFlags)result->normal` — a **pointer** stored into a four-byte enum slot. |

## Outside the census entirely — 6 geometry-offset sites

The census cannot see these at all, because nothing is truncated.

| site | status |
|---|---|
| `McdBatch` ×3 | `*(int *)(p + elementTable)` reads a pointer four bytes wide; the oracle says `mov 0x20(%rcx),%rcx` — **eight**. Needs the LOAD WIDTH changed, not the offset. |
| `IxSphereTriList` ×3 | Quarantined in `fix_typeid_dispatch`. Two faults: a displacement match that is **not** a type match, and a pipeline hazard — editing the file makes a later all-or-nothing-per-file pass drop its whole bundle, losing a `sizeof` repair and under-allocating the triangle list. |
