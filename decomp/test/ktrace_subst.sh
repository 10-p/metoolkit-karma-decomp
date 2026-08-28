#!/usr/bin/env bash
# ktrace_subst.sh — build the engine with a chosen SET of recovered objects replaced by MathEngine's
# shipped ones, record a Karma state trace, and score it against the legacy reference.
#
#   ktrace_subst.sh <label> <object> [object ...]      # named objects from theirs
#   ktrace_subst.sh <label> ALL                        # every one of the 145 — POSITIVE CONTROL
#   ktrace_subst.sh <label> NONE                       # pure recovered   — NEGATIVE CONTROL
#   ktrace_subst.sh <label> @Mdt                       # every object whose name starts Mdt
#
# WHAT IT ANSWERS. proven.txt localised the ONS crash by substituting one object at a time and
# asking "does the crash go away". This asks the harder question the owner's report needs: does the
# BEHAVIOUR come back. The metric is karma-decomp/test/ktrace_score.py against a reference trace
# recorded from the `legacy-karma` build, which links MathEngine's originals.
#
# ⚠ RUN BOTH CONTROLS BEFORE BELIEVING ANY ROW. ALL must score MATCH and NONE must score a
# mismatch; if ALL does not match, the substitution mechanic is not doing what it claims and every
# other row is void. This project has twice been bitten by a check that could not fail.
#
# ⚠ A SUBSTITUTION HARNESS MEASURES A COMBINATION. proven.txt ONS-CRASH-GJKPD: the complement bisect
# named a second function defective and repairing the first alone fixed the match with that one
# untouched. A mixed pair can fail where either whole pair succeeds. Read a positive as a lead.
#
# HOW THE SWAP IS MADE. cmake REBUILDS libKarmaDecomp.a from its object files, so `ar`-swapping a
# member is discarded and the test silently measures nothing. The compiled .o is replaced instead
# and touched, so it is newer than its .c and cmake does not recompile over the top.
set -uo pipefail

ENG=/home/ion/engines/engine-ut2004
OBJDIR="$ENG/build-native/Source/Engine/CMakeFiles/KarmaDecomp.dir/__/__/karma-decomp/generated/allobj"
SHIPDIR=/home/ion/tools/karma-lab/allobj
SRCDIR="$ENG/karma-decomp/generated/allobj"
PRISTINE=/tmp/ktrace-subst-pristine
REF="${KD_REF:-/tmp/ktrace-legacy600.csv}"
FRAMES="${KD_FRAMES:-600}"
BIN="$ENG/build-native/Source/SDLLaunch/ut2004-pixo.bin"

LABEL="${1:?usage: ktrace_subst.sh <label> <object|ALL|NONE|@prefix> ...}"; shift
[ -f "$REF" ] || { echo "no reference trace at $REF — record one from build-legacy-karma first"; exit 2; }

# Snapshot the pristine recovered objects ONCE. A second snapshot taken after a previous run would
# capture the SUBSTITUTED tree as "pristine" and quietly make every later restore a no-op.
if [ ! -d "$PRISTINE" ]; then
    mkdir -p "$PRISTINE"
    cp "$OBJDIR"/*.c.o "$PRISTINE/" || exit 1
    echo "snapshotted $(ls "$PRISTINE" | wc -l) pristine recovered objects -> $PRISTINE"
fi

restore() {
    cp "$PRISTINE"/*.c.o "$OBJDIR/" 2>/dev/null
    for f in "$OBJDIR"/*.c.o; do touch "$f"; done
}
trap restore EXIT

# Resolve the object list.
OBJS=()
for a in "$@"; do
    case "$a" in
        ALL)  for f in "$PRISTINE"/*.c.o; do OBJS+=("$(basename "$f" .c.o)"); done ;;
        NONE) : ;;
        @*)   p="${a#@}"; for f in "$PRISTINE"/${p}*.c.o; do [ -e "$f" ] && OBJS+=("$(basename "$f" .c.o)"); done ;;
        # `^@Prefix` / `^Name` REMOVE from the set built so far. This is what makes the COMPLEMENT
        # form possible — `ALL ^@Mdt` keeps only Mdt of ours and takes the other 118 from theirs —
        # and the complement is the direction with no blind spot when there is MORE THAN ONE
        # defective object: a forward swap of one leaves the other, so every single forward swap
        # reads "no fix" while the whole set reads "fixed". proven.txt ONS-CRASH-GJKPD is the
        # standing example, and it cost a run.
        ^@*)  p="${a#^@}"; NEW=(); for o in "${OBJS[@]}"; do case "$o" in ${p}*) ;; *) NEW+=("$o") ;; esac; done; OBJS=("${NEW[@]+"${NEW[@]}"}") ;;
        ^*)   p="${a#^}";  NEW=(); for o in "${OBJS[@]}"; do [ "$o" = "$p" ] || NEW+=("$o"); done; OBJS=("${NEW[@]+"${NEW[@]}"}") ;;
        *)    OBJS+=("$a") ;;
    esac
done

restore
missing=0
for o in "${OBJS[@]}"; do
    if [ ! -f "$SHIPDIR/$o.o" ]; then
        echo "  !! no shipped object for '$o' — it has no counterpart in $SHIPDIR"
        missing=1; continue
    fi
    cp "$SHIPDIR/$o.o" "$OBJDIR/$o.c.o" && touch "$OBJDIR/$o.c.o"
done
[ "$missing" = 1 ] && { echo "refusing to run with an unresolved object name"; exit 2; }

echo "=== $LABEL: ${#OBJS[@]} object(s) taken from MathEngine's shipped library ==="
( cd "$ENG" && cmake --build build-native -j"$(nproc)" ) > "/tmp/ktrace-subst-$LABEL.build.log" 2>&1 || {
    echo "  BUILD FAILED — $(grep -m1 -iE 'undefined reference|error:' "/tmp/ktrace-subst-$LABEL.build.log")"
    exit 3; }

# KD_SCENARIO=ragdoll switches the whole harness to the RAGDOLL question: one bot on
# test-karma-1, bone bodies traced, and scored on the two invariants the owner's report is about
# (self-intersection and range of motion) rather than on where the scripted actors came to rest.
# The default scenario cannot see a ragdoll defect at all — it scores rest position and sleep state,
# and both arms MATCH 30/30 while the ragdolls differ by 38%.
if [ "${KD_SCENARIO:-}" = "ragdoll" ]; then
    KD_FRAMES="${KD_RAG_FRAMES:-3000}" KD_EVERY=1 KD_BONES=1 KD_GAME=Onslaught.ONSOnslaughtGame \
        KD_URLOPTS="?SpectatorOnly=1?NumBots=1" \
        "$ENG/karma-decomp/test/ktrace_run.sh" "$BIN" "$LABEL" 300 > "/tmp/ktrace-subst-$LABEL.run.log" 2>&1
else
    KD_FRAMES="$FRAMES" KD_GAME=Onslaught.ONSOnslaughtGame \
        "$ENG/karma-decomp/test/ktrace_run.sh" "$BIN" "$LABEL" 120 > "/tmp/ktrace-subst-$LABEL.run.log" 2>&1
fi
if [ ! -s "/tmp/ktrace-$LABEL.csv" ]; then
    echo "  NO TRACE — $(tail -3 "/tmp/ktrace-subst-$LABEL.run.log" | tr '\n' ' ')"
    exit 3
fi

if [ "${KD_SCENARIO:-}" = "ragdoll" ]; then
    python3 "$ENG/karma-decomp/test/ragdoll_score.py" "${KD_RAG_REF:-/tmp/rd-L.csv}" "/tmp/ktrace-$LABEL.csv"
else
    python3 "$ENG/karma-decomp/test/ktrace_score.py" "$REF" "/tmp/ktrace-$LABEL.csv"
fi
