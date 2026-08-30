/*=============================================================================
    kd_shadow.c — shadow-differential harness, driven by the real game.

    WHAT THIS HOOKS, AND WHY THERE
    ------------------------------
    The first attempt renamed each shipped intersection function to orig_* and
    defined a replacement under the original name. That does not work, and the
    reason is worth recording: nothing in UT2004 calls McdBoxBoxIntersect by
    name. Karma registers it as a FUNCTION POINTER in an interaction table:

        interactions.intersectFn = McdBoxBoxIntersect;
        McdFrameworkSetInteractions(frame, kBox, kBox, &interactions);

    Renaming the symbol also rewrote that internal reference, so the table went
    on holding the original and the replacement was never linked, let alone
    called.

    So hook the REGISTRATION instead. One interposed function,
    McdFrameworkSetInteractions, sees every (geometry type, geometry type) pair
    the engine ever installs, and can substitute a thunk for any of them. That
    is one hook for the whole collision matrix — no per-function code, and it
    picks up pairs automatically as new objects are recovered.

    WHAT IT DOES
    ------------
    For each registered pair, the thunk runs the ORIGINAL into the caller's real
    result and, if a recovered implementation exists for that function, runs it
    into a scratch buffer and compares. The engine only ever consumes the
    original's output.

    That is NOT the same as gameplay being unchanged, which this comment used to
    claim. Running an intersection function a second time is only free if it
    writes solely through its output parameter, and McdModelPair carries
    m_cachedData, responseData and phase for it to write to. So the second call
    gets a COPY of the pair as its first argument, a scratch contact buffer with
    a canary after it, and — since copying the pair only copies the cache
    POINTER — the 60-byte cache block rewound to what the original consumed and
    restored to what the original left.

    Until 2026-08-23 the copy went into `scratch.pair` while the real pair was
    still passed as the first argument, so it did not do the job it was
    documented as doing: everything the recovered function wrote landed in
    engine state, and for McdGjkCgIntersect, which warm-starts from the cache
    block, the two implementations were feeding each other across frames.

    The harness IS implicated in an intermittent SIGSEGV in the engine's own
    KHandleCollisions — 4 crashes in 14 harness runs against 0 in 4 stock runs
    of the same map, and 0 in 5 with the second call off. That was measured
    before the isolation above; re-measure it rather than trusting it.
    HANDOVER.md §7 lists what else has not been tried.

    KD_CENSUS=1 turns the second call off entirely, KD_ONLY=<substring> narrows
    it to one function, and KD_SHARECACHE=1 restores the old shared-state
    behaviour for A/B. Reach for those first when a session misbehaves.

    Pairs with no recovered counterpart are still counted. That census answers
    the question that has to come first: which parts of the collision matrix
    does the game actually exercise?

    Output: $KD_SHADOW_OUT (CSV, rewritten periodically, not only at exit).
=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <MePrecision.h>
#include <McdCTypes.h>
#include <McdModel.h>
#include <McdGeometryTypes.h>

#define KD_MAX_PAIRS    96      /* 9 built-in geometry types => <= 81 ordered pairs */
#define KD_SCRATCH_MAX  64
#define KD_GUARD        8       /* trailing contacts, filled with a canary */
#define KD_CANARY       0xA5

/* Bytes of McdModelPair::m_cachedData to snapshot around the second call.
   McdCacheHello is the ONLY thing in the whole library that assigns
   m_cachedData, and it takes the block from a fixed pool built with
   `MePoolFixed(&frame->cachePool, 100, 0x3c, 0x10)` — 0x3c = 60 bytes per
   element. So 60 is the size of every cache block there is, not a guess at
   one. (McdBatch moves the pointer between pairs; it never allocates.) */
#define KD_CACHE_BYTES  60
#define KD_LOG_MAX      60

typedef int (MEAPI *kd_intersect_fn)(McdModelPair *, McdIntersectResult *);

typedef struct {
    int              t1, t2;
    kd_intersect_fn  orig;
    kd_intersect_fn  rec;          /* NULL when we have no recovered version */
    const char      *name;         /* NULL when unidentified */
    unsigned long    calls, identical, fp_only;
    unsigned long    ret_diff, count_diff, dims_diff;
    unsigned long    overrun;       /* wrote past the buffer it was handed */
    unsigned long    nonfinite;     /* the engine's own input was NaN; see kd_compare */
    double           worst_delta;
} kd_pair;

static kd_pair       kd_pairs[KD_MAX_PAIRS];
static int           kd_npairs;
static int           kd_logged;
static FILE         *kd_log;
static unsigned long kd_since_flush;

/* KD_ONLY=<substring>: shadow only functions whose name contains it. Everything
   else is COUNTED but not run twice, exactly as under KD_CENSUS.
   
   THIS WAS DOCUMENTED AND NOT IMPLEMENTED — in this file's own header, and in
   HANDOVER.md §7 as one of three switches to "reach for first when a session
   misbehaves" — from the day it was written until 2026-08-25. Every run that
   set it shadowed the whole matrix and looked exactly like a narrowed one. §7's
   crash table has a row reading "narrowed (KD_ONLY)"; that row was a full run.
   Check that a switch does something before believing a result that rests on
   it. */
static const char *kd_only;
static int kd_only_read;

/* The divergence log stops at KD_LOG_MAX entries, and the KINDS COMPETE: a pair
   with a thousand `dims` divergences fills the log before a pair with nine
   `count` ones is ever reached, so the interesting case is crowded out by the
   boring one. KD_LOGMAX raises the cap. */
static int kd_logmax = KD_LOG_MAX;
static int kd_logmax_read;
/* How deeply the thunks nest. Aggregates dispatch to their children's
   interactions, so a shadowed call can sit inside another one, and each frame
   carries a ~2.9 KB scratch buffer. Whether that is enough to matter is a
   question about the maximum depth, which is cheaper to measure than to
   bisect. */
static int kd_depth, kd_maxdepth;

#define KD_FLUSH_EVERY 10000    /* small enough to read a run in progress */

static const char *kd_typename(int t)
{
    switch (t) {
    case kMcdGeometryTypeNull:         return "Null";
    case kMcdGeometryTypeSphere:       return "Sphere";
    case kMcdGeometryTypeBox:          return "Box";
    case kMcdGeometryTypePlane:        return "Plane";
    case kMcdGeometryTypeCylinder:     return "Cylinder";
    case kMcdGeometryTypeSphyl:        return "Sphyl";
    case kMcdGeometryTypeTriangleList: return "TriangleList";
    case kMcdGeometryTypeConvexMesh:   return "ConvexMesh";
    case kMcdGeometryTypeAggregate:    return "Aggregate";
    default:                           return "?";
    }
}

static void kd_flush(void)
{
    const char *path = getenv("KD_SHADOW_OUT");
    if (!path) path = "/tmp/kd_shadow.csv";
    FILE *f = fopen(path, "w");
    if (!f) return;
    fprintf(f, "type1,type2,function,shadowed,calls,identical,fp_only,"
               "ret_diff,count_diff,dims_diff,overrun,nonfinite,worst_delta\n");
    for (int i = 0; i < kd_npairs; i++) {
        kd_pair *s = &kd_pairs[i];
        fprintf(f, "%s,%s,%s,%s,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%.6e\n",
                kd_typename(s->t1), kd_typename(s->t2),
                s->name ? s->name : "(unidentified)",
                s->rec ? "yes" : "no",
                s->calls, s->identical, s->fp_only,
                s->ret_diff, s->count_diff, s->dims_diff, s->overrun,
                s->nonfinite, s->worst_delta);
    }
    fprintf(f, "# max thunk nesting depth: %d\n", kd_maxdepth);
    fclose(f);
    if (kd_log) fflush(kd_log);
}

static void kd_init(void)
{
    static int done;
    if (done) return;
    done = 1;
    atexit(kd_flush);
    const char *d = getenv("KD_SHADOW_DIVERGENCES");
    kd_log = fopen(d ? d : "/tmp/kd_shadow_divergences.txt", "w");
}

/* --- recovered counterparts ---------------------------------------------
   Identified by the ORIGINAL function's address, so the table needs no
   knowledge of how the engine registers anything. Adding a newly recovered
   function is one line. */
/* Generated by make_shadow_metoolkit.sh from the objects actually staged, so a
   quarantined object cannot leave a dangling rec_* reference at link time, and
   a newly recovered interaction is picked up with no edit here. Each entry
   carries the real ELF symbols, because the id is not always the symbol: the
   convex-mesh interactions are C++ and ship mangled, and declaring them by
   their readable name links against nothing. */
#include "kd_recovered_list.h"

#define KD_DECL(id, osym, rsym) \
    extern int MEAPI kd_orig_##id(McdModelPair *, McdIntersectResult *) __asm__(osym); \
    extern int MEAPI kd_rec_##id(McdModelPair *, McdIntersectResult *) __asm__(rsym);
KD_RECOVERED_LIST(KD_DECL)

static const struct { kd_intersect_fn orig, rec; const char *name; } kd_known[] = {
#define KD_ENTRY(id, osym, rsym) { kd_orig_##id, kd_rec_##id, #id },
    KD_RECOVERED_LIST(KD_ENTRY)
};

/* Did the engine hand this pair a non-finite transform?

   It does. A 900 s ONS match produced 21 McdSphylSphylIntersect and
   McdSphylSphereIntersect divergences in one early burst, all reading
   `ret 1/0 touch 1/0 count 1/0`, and every one of them had tm1 and tm2 entirely
   NaN — a ragdoll had gone non-finite in the engine, well upstream of Karma.

   "Does this touch" has no right answer for a NaN transform, so two
   implementations answering differently is not a defect in either. Counting it
   as one is worse than useless: it is 21 fabricated structural divergences
   against a released object, in exactly the shape a real tolerance bug takes,
   and there is no way to tell them apart from the CSV. So count them in their
   own column and leave the verdict columns alone. */
static int kd_nonfinite_pair(const McdModelPair *p)
{
    if (!p) return 0;
    MeMatrix4Ptr t[2];
    t[0] = McdModelGetTransformPtr(p->model1);
    t[1] = McdModelGetTransformPtr(p->model2);
    for (int m = 0; m < 2; m++) {
        if (!t[m]) continue;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                double v = (double)t[m][i][j];
                if (v != v || v > 1e30 || v < -1e30) return 1;
            }
    }
    return 0;
}

/* --- comparison ---------------------------------------------------------
   Discrete fields (return value, touch, contact count, dimensionality) are
   DECISIONS and must match exactly. Continuous fields get float last-bit
   tolerance, because the recovered code is built by a different compiler than
   the shipped object. See docs/KARMA-ON-WASM.md II.5. */
static void kd_compare(kd_pair *s, int a, int b,
                       const McdIntersectResult *ra, const McdIntersectResult *rb)
{
    int bad = 0;

    if (a != b || ra->touch != rb->touch)          { bad = 1; }
    else if (ra->contactCount != rb->contactCount) { bad = 2; }
    if (bad && kd_nonfinite_pair(ra->pair)) {
        s->nonfinite++;
        return;                 /* no answer is right; do not score it */
    }
    if (bad == 1) s->ret_diff++;
    else if (bad == 2) s->count_diff++;

    double worst = 0.0;
    if (!bad) {
        int n = ra->contactCount;
        if (n > rb->contactMaxCount) n = rb->contactMaxCount;
        for (int i = 0; i < n; i++) {
            const McdContact *x = &ra->contacts[i], *y = &rb->contacts[i];
            if (x->dims != y->dims) {
                if (kd_nonfinite_pair(ra->pair)) { s->nonfinite++; return; }
                s->dims_diff++; bad = 1; break;
            }
            for (int k = 0; k < 3; k++) {
                double d = fabs((double)x->position[k] - y->position[k]);
                double e = fabs((double)x->normal[k]   - y->normal[k]);
                if (d > worst) worst = d;
                if (e > worst) worst = e;
            }
            double sp = fabs((double)x->separation - y->separation);
            if (sp > worst) worst = sp;
        }
        /* result->normal is the AVERAGE contact normal. Its only consumer is
           McdContactSimplify(result->normal, contacts, contactCount, ...) in
           KFarfield.cpp, which is handed the count alongside it — so with zero
           contacts the field cannot influence anything.
           It differs there for a real reason: with no contacts the accumulator
           stays {0,0,0}, and MeVector3Normalize maps a zero vector to {1,0,0},
           so whether the function reached that call at all becomes visible in a
           field nobody reads. Comparing it would report a divergence that has no
           effect on the simulation. */
        if (ra->contactCount > 0) {
            for (int k = 0; k < 3; k++) {
                double d = fabs((double)ra->normal[k] - rb->normal[k]);
                if (d > worst) worst = d;
            }
        }
    }
    if (worst > s->worst_delta) s->worst_delta = worst;

    if (bad) {
        if (kd_log && kd_logged < kd_logmax) {
            kd_logged++;
            fprintf(kd_log, "%s (%s vs %s): ret %d/%d touch %d/%d count %d/%d maxCount %d\n",
                    s->name ? s->name : "?", kd_typename(s->t1), kd_typename(s->t2),
                    a, b, ra->touch, rb->touch,
                    ra->contactCount, rb->contactCount, ra->contactMaxCount);
            /* Dump the INPUTS, so the case can be replayed deterministically in
               test/difftest_*.c. A divergence found in a live match is useless
               if it cannot be reproduced — the match never plays the same way
               twice. */
            if (ra->pair) {
                MeMatrix4Ptr t1 = McdModelGetTransformPtr(ra->pair->model1);
                MeMatrix4Ptr t2 = McdModelGetTransformPtr(ra->pair->model2);
                fprintf(kd_log, "  tm1:");
                for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)
                    fprintf(kd_log, " %.9g", (double)t1[i][j]);
                fprintf(kd_log, "\n  tm2:");
                for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)
                    fprintf(kd_log, " %.9g", (double)t2[i][j]);
                fprintf(kd_log, "\n  tol: %.9g %.9g\n",
                        (double)McdModelGetContactTolerance(ra->pair->model1),
                        (double)McdModelGetContactTolerance(ra->pair->model2));
            }
            fflush(kd_log);
        }
    } else if (worst == 0.0) s->identical++;
    else {
        s->fp_only++;
        /* A delta this large is not float noise — the same run gets 1 ULP on
           Sphere x Sphere. Dump both sides in full so the wrong component is
           identifiable rather than merely counted. */
        if (worst > 1e-3 && kd_log && kd_logged < kd_logmax) {
            kd_logged++;
            fprintf(kd_log, "NUMERIC %s worst=%.6g count=%d\n",
                    s->name ? s->name : "?", worst, ra->contactCount);
            int n = ra->contactCount;
            if (n > rb->contactMaxCount) n = rb->contactMaxCount;
            if (n > 3) n = 3;
            for (int i = 0; i < n; i++) {
                const McdContact *x = &ra->contacts[i], *y = &rb->contacts[i];
                fprintf(kd_log,
                        "  [%d] pos  orig %.9g %.9g %.9g | rec %.9g %.9g %.9g\n"
                        "      norm orig %.9g %.9g %.9g | rec %.9g %.9g %.9g\n"
                        "      sep  orig %.9g | rec %.9g   dims %d/%d\n",
                        i,
                        (double)x->position[0], (double)x->position[1], (double)x->position[2],
                        (double)y->position[0], (double)y->position[1], (double)y->position[2],
                        (double)x->normal[0], (double)x->normal[1], (double)x->normal[2],
                        (double)y->normal[0], (double)y->normal[1], (double)y->normal[2],
                        (double)x->separation, (double)y->separation, x->dims, y->dims);
            }
            fprintf(kd_log, "  avgN orig %.9g %.9g %.9g | rec %.9g %.9g %.9g\n",
                    (double)ra->normal[0], (double)ra->normal[1], (double)ra->normal[2],
                    (double)rb->normal[0], (double)rb->normal[1], (double)rb->normal[2]);
            fflush(kd_log);
        }
    }
}

/* KD_SELFTEST=1 runs the ORIGINAL as both sides.
   Any divergence it reports is a bug in this harness, not in the recovered code:
   these functions are supposed to write only through their output parameter, and
   if calling one twice on identical inputs gives two answers then something else
   is shared — a per-frame pool, framework scratch, a cache on the pair — and the
   comparison is meaningless for that function. Always run this before believing
   a divergence. */
static int kd_selftest = -1;

/* KD_CENSUS=1 counts calls and runs NOTHING twice.
   The point is bisection. If a session crashes with the harness in and not
   without it, this says whether the fault is in interposing the registration
   and routing every call through a thunk, or in calling the intersection
   function a second time — which is an assumption, not a fact: these functions
   are documented to write only through their output parameter, and a cached
   interaction (GJK keeps state on the McdModelPair) would not. */
static int kd_census = -1;

/* KD_SHARECACHE=1 restores the pre-2026-08-23 behaviour: the second call gets
   the engine's real McdModelPair and its cache block, and writes to both. Kept
   only so the change can be measured against what it replaced. */
static int kd_sharecache = -1;


static int kd_dispatch(int slot, McdModelPair *p, McdIntersectResult *r)
{
    kd_pair *s = &kd_pairs[slot];
    if (kd_selftest < 0) {
        const char *e = getenv("KD_SELFTEST");
        kd_selftest = (e && *e == '1') ? 1 : 0;
    }
    if (kd_census < 0) {
        const char *e = getenv("KD_CENSUS");
        kd_census = (e && *e == '1') ? 1 : 0;
    }
    if (kd_sharecache < 0) {
        const char *e = getenv("KD_SHARECACHE");
        kd_sharecache = (e && *e == '1') ? 1 : 0;
    }
    if (!kd_only_read) {
        kd_only_read = 1;
        kd_only = getenv("KD_ONLY");
        if (kd_only && !*kd_only) kd_only = 0;
    }
    if (!kd_logmax_read) {
        kd_logmax_read = 1;
        const char *e = getenv("KD_LOGMAX");
        if (e) { kd_logmax = atoi(e); if (kd_logmax < 1) kd_logmax = 1; }
    }
    /* Narrowed: count it, do not run it twice. Matched on the recovered
       function's own name, which is what the CSV reports, so what you type is
       what you see. A pair whose name we could not identify never matches, and
       is therefore excluded rather than silently included. */
    int narrowed = kd_only && !(s->name && strstr(s->name, kd_only));
    s->calls++;
    if (++kd_since_flush >= KD_FLUSH_EVERY) { kd_since_flush = 0; kd_flush(); }

    /* Snapshot BEFORE the original runs, because that is the state the
       original is about to consume and therefore the state the recovered has
       to be given if the two are to be compared at all. See the cache note
       below. */
    McdModelPair  pair_before = *p;
    unsigned char cache_before[KD_CACHE_BYTES];
    int have_cache = (s->rec && !kd_census && !narrowed && !kd_sharecache
                      && p->m_cachedData != 0);
    if (have_cache) memcpy(cache_before, p->m_cachedData, KD_CACHE_BYTES);

    if (++kd_depth > kd_maxdepth) kd_maxdepth = kd_depth;
    int a = s->orig(p, r);
    if (s->rec && !kd_census && !narrowed) {
        /* The scratch buffer carries a canary past its end.
           A recovered function that writes more contacts than it was given room
           for does not fail here — it smashes whatever is next, and the engine
           dies somewhere unrelated a few frames later. That is exactly what
           happened with IxSphylPrimitives staged: SIGSEGV inside
           McdModelGetGeometryType, called from the engine's own
           KHandleCollisions, walking a pair container this harness never
           touches. Half an hour to attribute, and the answer was in a function
           that had already returned.

           Checking a canary turns that into a counted, attributed defect with
           the inputs logged, which is the whole point of a shadow harness: a
           quarantined object can be MEASURED without taking the session with
           it. It is not a guarantee — a wild write far past the end still lands
           wherever it lands — but the overflow-by-a-few case is the common one
           and it is now caught at the call that caused it. */
        /* The second call gets a COPY of the McdModelPair, not the caller's,
           and — this is the part that was missing until 2026-08-23 — the copy
           is passed as the FIRST ARGUMENT too, and the cache the pair points
           at is rewound and restored around the call.

           The harness header claims gameplay is bit-for-bit unchanged, and
           that rests on these functions writing only through their output
           parameter. They do not: McdModelPair carries m_cachedData,
           responseData and phase, and a cached interaction is entitled to
           update them. The old code copied the pair into `scratch.pair` but
           still handed `p` — the engine's real pair — to the recovered
           function, so everything the recovered wrote landed in engine state.
           The copy was in the wrong place to do the job it was documented as
           doing.

           `m_cachedData` is worse than the struct fields, because copying the
           struct copies the POINTER: both calls write the same 60-byte block.
           McdGjkCgIntersect keeps its warm-start there, so on frame N the
           original stepped the cache and then the recovered stepped it AGAIN,
           and on frame N+1 the original read a cache the recovered had last
           touched. That is a feedback loop between the two implementations
           through engine state, and it showed: 3 ret_diff and 15 count_diff
           in 72,167 Box x ConvexMesh calls, all of them the same two actors
           over consecutive frames of one persistent contact.

           So: rewind the block to what the original consumed, run the
           recovered against that, then put back what the original left. The
           recovered sees the same input, the engine sees only the original's
           output, and the comparison means what it says.

           KD_SHARECACHE=1 restores the old behaviour for A/B measurement.

           Measured on ONS-UCMP-ABC, 240 s per run, before this change: stock
           0 crashes in 4, harness 3 in 8, and 0 in 5 with the second call
           suppressed (KD_CENSUS=1) or narrowed to one function (KD_ONLY).
           Intermittent, so none of those alone is proof, but the direction was
           consistent and this is the mechanism they pointed at. */
        McdContact         scratch_c[KD_SCRATCH_MAX + KD_GUARD];
        McdModelPair       pair_copy = pair_before;
        McdIntersectResult scratch = *r;
        unsigned char      cache_after[KD_CACHE_BYTES];
        McdModelPair      *rec_pair = p;
        if (!kd_sharecache) {
            scratch.pair = &pair_copy;
            rec_pair = &pair_copy;
        } else if (r->pair) {
            pair_copy = *r->pair;
            scratch.pair = &pair_copy;
        }
        /* Only if the original left the pointer alone; if it allocated or
           freed the block, cache_before describes something else. */
        int restore = have_cache && p->m_cachedData == pair_before.m_cachedData;
        if (restore) {
            memcpy(cache_after, p->m_cachedData, KD_CACHE_BYTES);
            memcpy(p->m_cachedData, cache_before, KD_CACHE_BYTES);
        }
        int cap = r->contactMaxCount;
        if (cap > KD_SCRATCH_MAX) cap = KD_SCRATCH_MAX;
        memset(scratch_c, 0, sizeof(McdContact) * KD_SCRATCH_MAX);
        memset(&scratch_c[KD_SCRATCH_MAX], KD_CANARY,
               sizeof(McdContact) * KD_GUARD);
        scratch.contacts = scratch_c;
        scratch.contactMaxCount = cap;
        scratch.contactCount = 0;
        scratch.touch = 0;
        int b = kd_selftest ? s->orig(rec_pair, &scratch)
                            : s->rec (rec_pair, &scratch);
        if (restore) memcpy(p->m_cachedData, cache_after, KD_CACHE_BYTES);

        const unsigned char *g = (const unsigned char *)&scratch_c[KD_SCRATCH_MAX];
        size_t gn = sizeof(McdContact) * KD_GUARD, i;
        for (i = 0; i < gn; i++) if (g[i] != KD_CANARY) break;
        if (i < gn || scratch.contactCount < 0 || scratch.contactCount > cap) {
            s->overrun++;
            if (kd_log && kd_logged < kd_logmax) {
                kd_logged++;
                fprintf(kd_log, "OVERRUN %s: wrote past a %d-contact buffer "
                                "(reported count %d)\n",
                        s->name ? s->name : "?", cap, scratch.contactCount);
                fflush(kd_log);
            }
        } else {
            kd_compare(s, a, b, r, &scratch);
        }
    }
    kd_depth--;
    return a;                       /* the engine always sees the original */
}

/* --- one thunk per slot -------------------------------------------------
   C has no closures, so each registration slot needs its own entry point that
   knows its index. Bounded and generated, never hand-written. */
#define KD_T8(base)                                                        \
    KD_T1(base+0) KD_T1(base+1) KD_T1(base+2) KD_T1(base+3)                \
    KD_T1(base+4) KD_T1(base+5) KD_T1(base+6) KD_T1(base+7)
#define KD_T1(i)                                                           \
    static int MEAPI kd_thunk_##i(McdModelPair *p, McdIntersectResult *r)  \
    { return kd_dispatch(i, p, r); }
#define KD_ROW(n) KD_T1(n##0) KD_T1(n##1) KD_T1(n##2) KD_T1(n##3) KD_T1(n##4) \
                  KD_T1(n##5) KD_T1(n##6) KD_T1(n##7) KD_T1(n##8) KD_T1(n##9)
KD_T1(0) KD_T1(1) KD_T1(2) KD_T1(3) KD_T1(4) KD_T1(5) KD_T1(6) KD_T1(7) KD_T1(8) KD_T1(9)
KD_ROW(1) KD_ROW(2) KD_ROW(3) KD_ROW(4) KD_ROW(5) KD_ROW(6) KD_ROW(7) KD_ROW(8) KD_ROW(9)
#undef KD_T1
#define KD_T1(i) kd_thunk_##i,
#define KD_ROW2(n) KD_T1(n##0) KD_T1(n##1) KD_T1(n##2) KD_T1(n##3) KD_T1(n##4) \
                   KD_T1(n##5) KD_T1(n##6) KD_T1(n##7) KD_T1(n##8) KD_T1(n##9)
static const kd_intersect_fn kd_thunks[] = {
    KD_T1(0) KD_T1(1) KD_T1(2) KD_T1(3) KD_T1(4) KD_T1(5) KD_T1(6) KD_T1(7) KD_T1(8) KD_T1(9)
    KD_ROW2(1) KD_ROW2(2) KD_ROW2(3) KD_ROW2(4) KD_ROW2(5)
    KD_ROW2(6) KD_ROW2(7) KD_ROW2(8) KD_ROW2(9)
};
#define KD_NTHUNKS ((int)(sizeof kd_thunks / sizeof kd_thunks[0]))

/* --- the one interposed function ---------------------------------------- */
extern void MEAPI orig_McdFrameworkSetInteractions(McdFramework *, int, int,
                                                   McdInteractions *);

void MEAPI McdFrameworkSetInteractions(McdFramework *f, int t1, int t2,
                                       McdInteractions *in)
{
    kd_init();
    if (in && in->intersectFn && kd_npairs < KD_MAX_PAIRS
            && kd_npairs < KD_NTHUNKS) {
        int slot = kd_npairs++;
        kd_pair *s = &kd_pairs[slot];
        memset(s, 0, sizeof *s);
        s->t1 = t1; s->t2 = t2;
        s->orig = (kd_intersect_fn)in->intersectFn;
        for (unsigned i = 0; i < sizeof kd_known / sizeof kd_known[0]; i++)
            if (kd_known[i].orig == s->orig) {
                s->rec  = kd_known[i].rec;
                s->name = kd_known[i].name;
                break;
            }
        in->intersectFn = (McdIntersectFn)kd_thunks[slot];
    }
    orig_McdFrameworkSetInteractions(f, t1, t2, in);
    kd_flush();                     /* census is useful even before any call */
}
