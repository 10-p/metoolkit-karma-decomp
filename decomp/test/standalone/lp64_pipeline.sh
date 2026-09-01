#!/usr/bin/env bash
# lp64_pipeline.sh — the whole LP64 post-pass chain, with its gate.
#
#   ./lp64_pipeline.sh [scene.c ...]
#
# §4's 95-second `recover.py` output is NOT arm64-correct source; the post-passes
# in `tools/` are what make it so, and they EDIT IN PLACE. This runs them on a
# copy in the right order, applies the i386 acceptance test that says they were
# no-ops on the shipped target, and only then runs the LP64 harness. Doing those
# by hand is four commands with two ways to get the order wrong, and the copy is
# the one that is easy to forget.
set -u
# ⚠ WITHOUT pipefail A PASS CAN CRASH AND THIS SCRIPT REPORTS PASS. Every
# pass below is piped into `head`/`tail` to keep the log readable, and
# `|| exit 2` then tests the exit status of `tail`, which is always 0. A
# Python traceback went straight through it and the acceptance test ran on a
# tree the pass had not finished editing — 145/145 and PASS, on a repair that
# had thrown IndexError.
set -o pipefail
. "$(cd "$(dirname "$0")" && pwd)/../../lib/kd-paths.sh"
MT="$METOOLKIT_DIR"
INC="$KD_MT_INC"
SRC="${KD_OUT_SRC:-/tmp/kd_out}"
DST="${KD_OUT:-/tmp/kd_lp64}"
BUILD="${KD_BUILD:-/tmp/kd_build}"

[ -d "$SRC/allobj" ] || { echo "  no recovery at $SRC — run tools/recover.py first"; exit 2; }
rm -rf "$DST" && cp -a "$SRC" "$DST" || exit 2
echo "== post-passes on a COPY ($DST) =="
python3 "$KD_ROOT/tools/fix_baked_sizeof.py" "$DST/allobj" "$BUILD" "$MT" || exit 2
python3 "$KD_ROOT/tools/fix_strides.py"   "$DST/allobj" "$BUILD" "$MT" || exit 2
# ⚠ TWICE, AND THE SECOND RUN IS NOT BELT-AND-BRACES. Each run resolves one
# more link of a type chain: a call names `pvVar2`, `pvVar2` names the field
# `first`, and only then can the local that READS `first` be typed. The pass is
# idempotent — a repaired site has no literal left to match — so the second run
# sees only what the first could not resolve.
python3 "$KD_ROOT/tools/fix_literal_offsets.py" "$DST/allobj" "$BUILD" "$MT" | tail -3 || exit 2
python3 "$KD_ROOT/tools/fix_literal_offsets.py" "$DST/allobj" "$BUILD" "$MT" | tail -3 || exit 2
python3 "$KD_ROOT/tools/fix_derived_fields.py" "$DST/allobj" "$BUILD" "$MT" | head -2 || exit 2
# ⚠ AFTER fix_derived_fields, WHICH IS THE SAME DEFECT CLASS AND MUST GO FIRST.
# That pass types the base pointer PER FILE and names the concrete field with
# its own declared type, which is the better repair wherever a file has one
# geometry in it. What it cannot type it reports, and its own README records why:
# "44 where the concrete type is genuinely ambiguous — an Ix* function handles
# TWO geometries, so per-file inference cannot work". Those 44 are where the
# located defect lives (`trilistgeom[3].mRefCtAndID`, byte 48 here and 96
# there), so this pass takes the residue and types it PER VARIABLE instead.
# Running it first would take the sites away from the better repair.
python3 "$KD_ROOT/tools/fix_index_layout.py" "$DST/allobj" "$BUILD" "$MT" | head -4 || exit 2
python3 "$KD_ROOT/tools/fix_arena_carve.py" "$DST/allobj" "$BUILD" "$MT" | head -3 || exit 2
python3 "$KD_ROOT/tools/fix_vtable_offsets.py" "$DST/allobj" "$BUILD" "$MT" | head -2 || exit 2
python3 "$KD_ROOT/tools/fix_ptrwidth.py"    "$DST/allobj" "$BUILD" "$MT" || exit 2
# AFTER fix_ptrwidth: it widens the CASTS, and these two key on what it wrote.
# fix_narrow_pointers first — it widens the locals whose masks the next pass has
# to recognise, and a mask on a widened local has no cast in it to match.
python3 "$KD_ROOT/tools/fix_narrow_pointers.py" "$DST/allobj" "$BUILD" "$MT" | head -4 || exit 2
python3 "$KD_ROOT/tools/fix_align_masks.py" "$DST/allobj" "$BUILD" "$MT" | head -2 || exit 2
python3 "$KD_ROOT/tools/fix_frame_slots.py" "$DST/allobj" "$BUILD" "$MT" | head -2 || exit 2
# AFTER fix_narrow_pointers TOO, and for a different reason: this one LEARNS the
# element size from the allocations that pass repairs. Run it earlier and every
# pool name reads as a four-byte array — `NAZ` and `NR` are spelled identically
# until rule G widens one of them — so it would print a clean, wrong zero. It
# refuses that case rather than printing it.
python3 "$KD_ROOT/tools/fix_pool_reserve.py" "$DST/allobj" "$BUILD" "$MT" | head -2 || exit 2
# ---- LAST, AND DELIBERATELY SO. `fix_narrow_loads` reads clang's
# -Wint-to-pointer-cast diagnostics over the FINISHED text, so running it here
# gives it the most complete state and stops it from rewriting a shape an
# earlier pass still has to recognise: it turns `*(int *)p` into `*(kd_iptr *)p`,
# which is exactly the spelling `fix_narrow_pointers` and `fix_align_masks` key
# on. It is the other half of the index repair above — that one fixes the
# ADDRESS a pointer is read from, this one the WIDTH it is read at, and the
# triangle generator needs both.
python3 "$KD_ROOT/tools/fix_narrow_loads.py" "$DST/allobj" "$BUILD" "$MT" | head -4 || exit 2

# ---- IMMEDIATELY AFTER fix_narrow_loads, BECAUSE IT IS THE OTHER HALF OF THE
# SAME LOAD. That pass widened `*(int *)((char *)&McdGjkBinarySubset + 0x3c)` to
# `*(kd_iptr *)` — the right WIDTH at the wrong ADDRESS, because `0x3c` is
# element 15 of a pointer array only while pointers are four bytes. This pass
# fixes the address; run it first and there is no narrow load left for the
# diagnostic above to report. Two sites, both confirmed against MathEngine's own
# amd64 build (`McdGjkBinarySubset + 0x78` = 15*8), and one of them is the arm64
# Onslaught SIGSEGV.
python3 "$KD_ROOT/tools/fix_global_array_index.py" "$DST/allobj" "$BUILD" "$MT" | tail -3 || exit 2

# ---- AFTER the passes above, because it reads what they wrote: the cursor's
# initialiser has to already be spelled as an offsetof against a NAMED field
# (`fix_literal_offsets`/`fix_derived_fields`) and cast through `kd_iptr`
# (`fix_ptrwidth`) before this one can tell which field a list head came from.
# Before `fix_block_copy` because this repair is a plain re-spelling with no
# preprocessor conditional in it.
python3 "$KD_ROOT/tools/fix_list_walk.py" "$DST/allobj" "$BUILD" "$MT" | tail -4 || exit 2

# ---- It anchors on a POOL, not on a field. `(MePoolFixedAPI.init)(pool, n,
# sizeof(*(McdCache *)0), 16)` is the only place `m_cachedData`'s type is written
# down at all — the oracle declares it `void *` and says the rest in a comment.
# It also types the WORD-INDEXED triangle walks, which is the ragdoll path.
python3 "$KD_ROOT/tools/fix_word_indexed_struct.py" "$DST/allobj" "$BUILD" "$MT" | tail -3 || exit 2

# ---- AFTER fix_word_indexed_struct (moved 2026-08-31), and that ORDER IS THE
# GATE. This pass keys on the field offsetof spelling
# (`((McdAggregate *)0)->elementTable`); a file that already had the member typed
# writes plain C instead — `triList->list` in IxCylinderTriList, where the arm64
# tombstone lands. Accepting that spelling on its own is TOO BROAD, measured: it
# took the pass from 56 rewrites to 262 and turned the LP64 harness red — four
# AddressSanitizer errors against a CLEAN i386 control, plus a trajectory
# divergence at step 111 — while i386 stayed 145/145 the whole time. So the
# direct spelling counts only where the pass above has already typed an access as
# `((E *)v)->`: an independent conclusion, with its own frame test and its own
# two-compiler gate, that this file walks an array of E.
# ---- `fix_baked_sizeof` A SECOND TIME, and only its FIELD-TYPED alloca rule.
# That rule sizes a stack triangle array from the member the block is stored
# into — `McdTriangleList::list`, declared `McdUserTriangle *` in the oracle —
# and in the raw recovery that statement reads `pMVar9[3].prev = ...`. The field
# has no NAME until the two passes above have run, so the rule cannot fire on
# the first invocation and the array stays at 24 bytes per element instead of
# 48. `--field-allocas-only` keeps the other four rules switched off so nothing
# is re-litigated over text five passes have since rewritten.
python3 "$KD_ROOT/tools/fix_baked_sizeof.py" "$DST/allobj" "$BUILD" "$MT" --field-allocas-only | tail -4 || exit 2

python3 "$KD_ROOT/tools/fix_element_stride.py" "$DST/allobj" "$BUILD" "$MT" | tail -3 || exit 2

# ---- AND ITS SIBLING, for the array gcc addresses from the MIDDLE of its first
# element. `result->contacts->normal + cur + 0x18` is byte 36 of an `McdContact`,
# which is `element2` at i386 and the high half of `element1` at LP64 — an
# eight-byte write across two fields. It is `KPerContactCB` faulting in SIX of
# eight gametypes, and it is what `McdSphylTriangleListIntersect`'s repair made
# reachable. It runs after `fix_element_stride` because it is the same question
# asked from a member's address rather than the element's, and the two must not
# both claim a literal.
python3 "$KD_ROOT/tools/fix_member_base_walk.py" "$DST/allobj" "$BUILD" "$MT" | tail -8 || exit 2

# ---- AND THE OTHER PLACE A LAYOUT HIDES: a callback's `void *` CONTEXT, which
# Ghidra has no type for at all, so every field of it is a baked byte offset.
# ⚠ NO STATIC GATE IN THIS PROJECT CAN SEE THAT CLASS — nothing is truncated and
# no cast is narrowed, so `MePoolx.c` is on neither the 91 aarch64 diagnostics nor
# the 52 open ones while `MePoolxDictNodeAllocate` reads `numfree` out of
# `numrec` and stores eight bytes into a four-byte index. It is the vehicle
# crash: `MeDictInsert` <- `MeSetAdd` <- `McdConvexMeshPlaneCut`.
python3 "$KD_ROOT/tools/fix_callback_context.py" "$DST/allobj" "$BUILD" "$MT" | tail -6 || exit 2

# ---- AND THE `alloca`'d ARRAY OF POINTERS, which `fix_baked_sizeof` DECLINED BY
# NAME: its qsort rule needs the base to be a plain identifier, and `MdtLOD`'s
# travels in an argslot. The missing piece was never a better guess at the cast,
# it was the ELEMENT TYPE — and that is declared, in the callee's signature.
# ⚠ AFTER `fix_frame_slots`, whose sixth rule widens the slot qsort's size
# argument travels in: a size stored four bytes wide and read as a `size_t` is a
# SECOND defect on the same line, and fixing either alone leaves the other.
python3 "$KD_ROOT/tools/fix_alloca_elem.py" "$DST/allobj" "$BUILD" "$MT" | tail -10 || exit 2

# ---- AFTER EVEN THAT ONE, and for a reason none of the others have: this pass
# does not re-spell an expression, it puts the corrected body behind
# `#if __SIZEOF_POINTER__ == 4` and leaves the i386 text VERBATIM. Every pass
# above reads the sources as plain C; running this one earlier would hand them a
# preprocessor conditional to parse and a second, hand-written copy of each
# repaired loop to "repair" again. Last means nothing else has to know about it.
python3 "$KD_ROOT/tools/fix_block_copy.py" "$DST/allobj" "$BUILD" "$MT" | tail -4 || exit 2

# ---- THE DETECTOR THIS PASS COULD BLIND. check_frame_bounds reads CONSTANT
# offsets and constant array bounds, and fix_frame_slots replaces both with
# constant EXPRESSIONS. It has been taught the new spelling; running it here
# says so out loud rather than leaving a zero to be trusted.
echo "== frame bounds after the post-passes =="
python3 "$KD_ROOT/tools/check_frame_bounds.py" "$DST/allobj" "$BUILD" | tail -1 || exit 1

# ---- THE ACCEPTANCE TEST. `intptr_t` IS `int` at 32-bit pointer width and the
# rewritten sizes ARE the constants they replaced, so both passes are no-ops
# here BY CONSTRUCTION — which is exactly the kind of claim that stops being
# true quietly. It has already caught `fix_baked_sizeof` changing CxSmallSort by
# 64 bytes and `McdModelPairManager` by a scheduling difference.
#
# ⚠ THE FILE MUST KEEP ITS NAME. gcc records the source basename in an STT_FILE
# symbol, so compiling identical text from `/tmp/t.c` yields a different object
# and reads exactly like a codegen change.
echo "== i386 acceptance: every object byte-identical =="
IF="-I$INC -I$INC/McdCommon -I$INC/McdPrimitives -I$INC/McdFrame -I$INC/MeGlobals"
IF="$IF -I$INC/MdtBcl -I$INC/MdtKea -I$INC/Mst -I$INC/MeApp -I$KD_MD_INC"
CF="-m32 -O2 -fno-pic -fno-strict-aliasing -std=gnu99 -w -Wno-int-conversion"
CF="$CF -Wno-incompatible-pointer-types -DLINUX $IF"
W=$(mktemp -d); trap 'rm -rf "$W"' EXIT
bad=0; fail=0; n=0
for o in "$BUILD"/*.o; do
    b=$(basename "$o" .o); n=$((n+1))
    cp "$DST/allobj/$b.c" "$W/$b.c"
    if gcc $CF -c -o "$W/$b.o" "$W/$b.c" 2>/dev/null; then
        cmp -s "$o" "$W/$b.o" || { echo "  DIFFERS: $b"; bad=$((bad+1)); }
    else
        echo "  DID NOT COMPILE: $b"; fail=$((fail+1))
    fi
done
echo "  $n object(s), $fail compile failure(s), $bad byte difference(s)"
[ "$fail" = 0 ] && [ "$bad" = 0 ] || { echo "  -> STOP: the post-passes are not no-ops at i386."; exit 1; }

echo "== LP64 =="
# ★ THE i386 CONTROL RUNS BY DEFAULT, AND THAT CHANGED ON 2026-08-30. It used to
# be skipped here, which made this gate report FAIL for a reason it could not
# attribute.
#
# WHY. `scene_ragdoll`'s own verdict is unstable under ASan: it sits on its
# `escaped > 1e3` threshold and the sanitizer's layout is enough to tip it. That
# is documented and MEASURED — BLOWN UP 3 of 5 at LP64 and 5 of 5 at i386, with
# zero sanitizer errors either way (proven.txt LP64-THREE-CLEAN). lp64_run.sh
# already knows how to handle it: when a scene's own verdict fails AND the i386
# control's fails too, the result is not attributable to pointer width and is not
# counted. Skipping the control removed the only evidence that could excuse it,
# so a coin flip in the scene became a FAIL in the headline gate.
#
# RE-MEASURED HERE, 2026-08-30, five runs on unchanged sources: with the control
# ON, lp64_run.sh read PASS 5 of 5 — the ragdoll blew up under ASan in 2 of
# those 5 and was correctly excused both times, and the no-sanitizer LP64 run was
# clean at 901 rows in all five. With the control OFF the same tree reads FAIL
# whenever the coin lands that way.
#
# The harness's own header says it plainly — "without it none of the rows below
# mean anything". Set KD_SKIP_CONTROL=1 to go back to the fast, unattributable
# form; it costs an i386 ASan build of the corpus, which is about a minute.
KD_OUT="$DST" KD_SKIP_CONTROL="${KD_SKIP_CONTROL:-0}" "$KD_T_STD/lp64_run.sh" "$@"
