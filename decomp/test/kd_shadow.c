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
    original's output, so gameplay is bit-for-bit unchanged.

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
#define KD_LOG_MAX      60

typedef int (MEAPI *kd_intersect_fn)(McdModelPair *, McdIntersectResult *);

typedef struct {
    int              t1, t2;
    kd_intersect_fn  orig;
    kd_intersect_fn  rec;          /* NULL when we have no recovered version */
    const char      *name;         /* NULL when unidentified */
    unsigned long    calls, identical, fp_only;
    unsigned long    ret_diff, count_diff, dims_diff;
    double           worst_delta;
} kd_pair;

static kd_pair       kd_pairs[KD_MAX_PAIRS];
static int           kd_npairs;
static int           kd_logged;
static FILE         *kd_log;
static unsigned long kd_since_flush;
#define KD_FLUSH_EVERY 100000

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
               "ret_diff,count_diff,dims_diff,worst_delta\n");
    for (int i = 0; i < kd_npairs; i++) {
        kd_pair *s = &kd_pairs[i];
        fprintf(f, "%s,%s,%s,%s,%lu,%lu,%lu,%lu,%lu,%lu,%.6e\n",
                kd_typename(s->t1), kd_typename(s->t2),
                s->name ? s->name : "(unidentified)",
                s->rec ? "yes" : "no",
                s->calls, s->identical, s->fp_only,
                s->ret_diff, s->count_diff, s->dims_diff, s->worst_delta);
    }
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
#define KD_RECOVERED_LIST(X)             \
    X(McdBoxBoxIntersect)                \
    X(McdBoxSphereIntersect)             \
    X(McdSpherePlaneIntersect)           \
    X(McdSphereSphereIntersect)          \
    X(McdCylinderPlaneIntersect)

#define KD_DECL(fn) \
    extern int MEAPI fn(McdModelPair *, McdIntersectResult *); \
    extern int MEAPI rec_##fn(McdModelPair *, McdIntersectResult *);
KD_RECOVERED_LIST(KD_DECL)

static const struct { kd_intersect_fn orig, rec; const char *name; } kd_known[] = {
#define KD_ENTRY(fn) { fn, rec_##fn, #fn },
    KD_RECOVERED_LIST(KD_ENTRY)
};

/* --- comparison ---------------------------------------------------------
   Discrete fields (return value, touch, contact count, dimensionality) are
   DECISIONS and must match exactly. Continuous fields get float last-bit
   tolerance, because the recovered code is built by a different compiler than
   the shipped object. See docs/KARMA-ON-WASM.md II.5. */
static void kd_compare(kd_pair *s, int a, int b,
                       const McdIntersectResult *ra, const McdIntersectResult *rb)
{
    int bad = 0;
    if (a != b || ra->touch != rb->touch)          { s->ret_diff++;   bad = 1; }
    else if (ra->contactCount != rb->contactCount) { s->count_diff++; bad = 1; }

    double worst = 0.0;
    if (!bad) {
        int n = ra->contactCount;
        if (n > rb->contactMaxCount) n = rb->contactMaxCount;
        for (int i = 0; i < n; i++) {
            const McdContact *x = &ra->contacts[i], *y = &rb->contacts[i];
            if (x->dims != y->dims) { s->dims_diff++; bad = 1; break; }
            for (int k = 0; k < 3; k++) {
                double d = fabs((double)x->position[k] - y->position[k]);
                double e = fabs((double)x->normal[k]   - y->normal[k]);
                if (d > worst) worst = d;
                if (e > worst) worst = e;
            }
            double sp = fabs((double)x->separation - y->separation);
            if (sp > worst) worst = sp;
        }
        for (int k = 0; k < 3; k++) {
            double d = fabs((double)ra->normal[k] - rb->normal[k]);
            if (d > worst) worst = d;
        }
    }
    if (worst > s->worst_delta) s->worst_delta = worst;

    if (bad) {
        if (kd_log && kd_logged < KD_LOG_MAX) {
            kd_logged++;
            fprintf(kd_log, "%s (%s vs %s): ret %d/%d touch %d/%d count %d/%d\n",
                    s->name ? s->name : "?", kd_typename(s->t1), kd_typename(s->t2),
                    a, b, ra->touch, rb->touch, ra->contactCount, rb->contactCount);
        }
    } else if (worst == 0.0) s->identical++;
    else                     s->fp_only++;
}

static int kd_dispatch(int slot, McdModelPair *p, McdIntersectResult *r)
{
    kd_pair *s = &kd_pairs[slot];
    s->calls++;
    if (++kd_since_flush >= KD_FLUSH_EVERY) { kd_since_flush = 0; kd_flush(); }

    int a = s->orig(p, r);
    if (s->rec) {
        McdContact         scratch_c[KD_SCRATCH_MAX];
        McdIntersectResult scratch = *r;
        int cap = r->contactMaxCount;
        if (cap > KD_SCRATCH_MAX) cap = KD_SCRATCH_MAX;
        memset(scratch_c, 0, sizeof scratch_c);
        scratch.contacts = scratch_c;
        scratch.contactMaxCount = cap;
        scratch.contactCount = 0;
        scratch.touch = 0;
        int b = s->rec(p, &scratch);
        kd_compare(s, a, b, r, &scratch);
    }
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
