/* float_sentinel.c — every float-sentinel comparison in the recovery, driven where the operand IS the sentinel.
 *
 * Built three times by float_sentinel.sh and compared there, exactly as lineseg_lp64.c is:
 *
 *   ref    MathEngine's SHIPPED i386 archives       — the reference
 *   c32    the recovered product at i386, SSE math   — wasm32's arithmetic and pointer width
 *   c64    the recovered product at LP64             — native x86-64, Win64, arm64
 *
 * Ghidra prints a float constant as an unsuffixed decimal, which C reads as a DOUBLE: `distance !=
 * 3.4028235e+38` promotes the float and never equals it, so a test for "still the sentinel" is never
 * true. ghidra_clean.type_float_sentinel_comparisons spells these comparisons as floats. Each case below
 * puts the exact sentinel value in front of one of them, through the public entry point the comparison
 * lives behind, and prints what the caller sees.
 *
 * One line per case, floats as 8-hex-digit bit patterns:
 *
 *   <case> <ret> <defined mask> <v0> ... <v7>
 *
 * ⚠ A FIELD THE FUNCTION DID NOT WRITE IS NOT A RESULT. Each case runs twice with its outputs and the
 * stack beneath the call filled with two different patterns; a value that differs between the two runs
 * is marked undefined (bit 0 ret, bit 1+k value k). An `McdSpace` query that misses writes only the
 * distance, and the comparator compares defined values only.
 *
 * The sites with no case here — the comparison's sentinel cannot reach an output, or only through state
 * the shipped code leaves uninitialised — are listed with the reason in ../../docs/STATE.md.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "MeMath.h"
#include "McdFrame.h"
#include "McdModel.h"
#include "McdGeometry.h"
#include "McdPrimitives.h"
#include "McdSphere.h"
#include "McdBox.h"
#include "McdCylinder.h"
#include "McdSphyl.h"
#include "McdAggregate.h"
#include "McdConvexMesh.h"
#include "McdNull.h"
#include "McdInteractions.h"
#include "McdSpace.h"
#include "McdContact.h"
#include "McdModelPair.h"
#include "MdtTypes.h"
#include "MdtMainLoop.h"

#define NV 8

static unsigned int bits(float f) { unsigned int u; memcpy(&u, &f, 4); return u; }

static void __attribute__((noinline)) poison_stack(unsigned char v)
{
    volatile unsigned char buf[16384];
    int i;
    for (i = 0; i < (int)sizeof buf; i++) buf[i] = v;
}

typedef int (*case_fn)(unsigned char pat, float *v);

/* Runs a case twice under two poison patterns and prints the first run with its defined mask. */
static void run(const char *name, case_fn fn)
{
    float v1[NV], v2[NV];
    int r1, r2, k, mask = 0;
    memset(v1, 0, sizeof v1); memset(v2, 0, sizeof v2);
    poison_stack(0x3f); r1 = fn(0x3f, v1);
    poison_stack(0x41); r2 = fn(0x41, v2);
    if (r1 == r2) mask |= 1;
    for (k = 0; k < NV; k++)
        if (bits(v1[k]) == bits(v2[k])) mask |= 2 << k;
    printf("%s %d %03x", name, r1, mask);
    for (k = 0; k < NV; k++) printf(" %08x", bits(v1[k]));
    printf("\n");
}

static McdFrameworkID fw;

static void identity(MeMatrix4 tm, float x, float y, float z)
{
    MeMatrix4TMMakeIdentity(tm);
    tm[3][0] = x; tm[3][1] = y; tm[3][2] = z;
}

/* ---- IxAggregateLineSegment: `return distance != MEINFINITY` (McdAggregate) ----------------------- */

static McdModelID agg_model;
static MeMatrix4 agg_tm, agg_rel;

static int agg_case(unsigned char pat, float *v, float y)
{
    McdLineSegIntersectResult r;
    MeReal o[3] = { -3, y, 0 }, d[3] = { 3, y, 0 };
    McdLineSegIntersectFnPtr fn = McdGeometryGetLineSegIntersectFnPtr(McdModelGetGeometry(agg_model));
    int ret, k;
    memset(&r, pat, sizeof r);
    McdModelUpdate(agg_model);
    ret = fn(agg_model, o, d, &r);
    v[0] = r.distance;
    for (k = 0; k < 3; k++) { v[1 + k] = r.position[k]; v[4 + k] = r.normal[k]; }
    return ret;
}
static int agg_miss(unsigned char p, float *v) { return agg_case(p, v, 5.0f); }
static int agg_hit(unsigned char p, float *v) { return agg_case(p, v, 0.25f); }

/* ---- McdSpaceGetLineSegFirst[Enabled]Intersection: `return distance < 1e25f` (McdSpace) ----------- */

static McdSpaceID space;
static McdModelID sm[2];
static MeMatrix4 stm[2];

static int MEAPI accept_all(McdModelID m, void *data) { (void)m; (void)data; return 1; }
static int MEAPI reject_all(McdModelID m, void *data) { (void)m; (void)data; return 0; }

static int space_case(unsigned char pat, float *v, float y, int enabled, McdLineSegIntersectEnableCallback cb)
{
    McdLineSegIntersectResult r;
    MeReal o[3] = { -3, y, 0 }, d[3] = { 6, y, 0 };
    int ret, k;
    memset(&r, pat, sizeof r);
    ret = enabled ? McdSpaceGetLineSegFirstEnabledIntersection(space, o, d, cb, 0, &r)
                  : McdSpaceGetLineSegFirstIntersection(space, o, d, &r);
    v[0] = r.distance;
    for (k = 0; k < 3; k++) { v[1 + k] = r.position[k]; v[4 + k] = r.normal[k]; }
    /* which model: 0, 1, or a value that is neither (unwritten) */
    v[7] = r.model == sm[0] ? 0.0f : r.model == sm[1] ? 1.0f : -1.0f;
    return ret;
}
static int space_miss(unsigned char p, float *v) { return space_case(p, v, 5.0f, 0, 0); }
static int space_hit(unsigned char p, float *v) { return space_case(p, v, 0.1f, 0, 0); }
static int space_en_miss(unsigned char p, float *v) { return space_case(p, v, 5.0f, 1, accept_all); }
static int space_en_hit(unsigned char p, float *v) { return space_case(p, v, 0.1f, 1, accept_all); }
static int space_en_rejected(unsigned char p, float *v) { return space_case(p, v, 0.1f, 1, reject_all); }

/* ---- McdContactSimplify: the deepest contact starts at MEINFINITY (McdContact) --------------------- */

static int simplify_case(unsigned char pat, float *v, int n, const float *sep)
{
    McdContact src[8], dst[8];
    MeReal normal[3] = { 0, 0, 1 };
    int i, ret;
    memset(src, 0, sizeof src);
    memset(dst, pat, sizeof dst);
    for (i = 0; i < n; i++) {
        src[i].position[0] = (float)i; src[i].position[1] = 0.5f * (float)i; src[i].position[2] = 0;
        src[i].normal[2] = 1;
        src[i].separation = sep[i];
    }
    ret = McdContactSimplify(normal, src, n, dst, 4, 0, 1.0f);
    for (i = 0; i < 2; i++) {
        v[4 * i + 0] = dst[i].position[0]; v[4 * i + 1] = dst[i].position[1];
        v[4 * i + 2] = dst[i].normal[2];   v[4 * i + 3] = dst[i].separation;
    }
    return ret;
}
/* Three contacts: the unrolled loop's FIRST step is the one comparing against the constant. */
static const float sep_all_inf[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
static const float sep_last_real[3] = { FLT_MAX, FLT_MAX, -0.01f };
static int simplify_inf(unsigned char p, float *v) { return simplify_case(p, v, 3, sep_all_inf); }
static int simplify_mixed(unsigned char p, float *v) { return simplify_case(p, v, 3, sep_last_real); }

/* ---- MdtPartitionGetSafeTime: the minimum starts at MEINFINITY (MdtMainLoop) ----------------------- */

static int safetime_case(unsigned char pat, float *v, float t0)
{
    MdtPartitionOutput po;
    MdtBody body[3];
    MdtBody *list[3];
    int start = 0, size = 3, i;          /* size & 3 == 3: the unrolled step that compares the constant */
    (void)pat;
    memset(&po, 0, sizeof po);
    memset(body, 0, sizeof body);
    for (i = 0; i < 3; i++) { list[i] = &body[i]; body[i].safeTime = FLT_MAX; }
    body[0].safeTime = t0;
    po.bodies = list; po.bodiesStart = &start; po.bodiesSize = &size;
    v[0] = MdtPartitionGetSafeTime(&po, 0);
    return 0;
}
static int safetime_inf(unsigned char p, float *v) { return safetime_case(p, v, FLT_MAX); }
static int safetime_real(unsigned char p, float *v) { return safetime_case(p, v, 0.5f); }

/* ---- pair tests whose separating-axis search starts at -MEINFINITY ---------------------------------
 *
 * McdVanillaOverlapOBBs (IxBoxBox) and McdSphylBoxIntersect's deep branch keep the largest axis
 * separation, seeded with -MEINFINITY, and adopt an axis only when it is larger. An axis separation of
 * exactly -FLT_MAX is that seed: a box whose half-extents are FLT_MAX around the other shape. The public
 * API cannot build one (McdBoxCreate halves full lengths, so FLT_MAX at most / 2), so the radii are
 * written through McdBoxGetRadii — the shipped library's own pointer to them. Absurd geometry, chosen
 * because it is the ONLY input that reaches these comparisons' sentinel; UT2004 cannot produce it. */

extern int MEAPI McdBoxBoxIntersect(McdModelPair *, McdIntersectResult *);
extern int MEAPI McdSphylBoxIntersect(McdModelPair *, McdIntersectResult *);

static McdModelID giant_box, small_box, small_sphyl;
static MeMatrix4 gtm, btm, ytm;

static int pair_case(unsigned char pat, float *v, McdModelID a, McdModelID b,
                     int (MEAPI *fn)(McdModelPair *, McdIntersectResult *))
{
    McdModelPair pair;
    McdIntersectResult r;
    McdContact c[8];
    int ret;
    memset(&pair, 0, sizeof pair);
    memset(&r, 0, sizeof r);
    memset(c, pat, sizeof c);
    pair.model1 = a; pair.model2 = b;
    r.pair = &pair; r.contacts = c; r.contactMaxCount = 8;
    McdModelUpdate(a); McdModelUpdate(b);
    ret = fn(&pair, &r);
    v[0] = (float)r.touch; v[1] = (float)r.contactCount;
    v[2] = r.normal[0]; v[3] = r.normal[1]; v[4] = r.normal[2];
    v[5] = c[0].normal[0]; v[6] = c[0].normal[1]; v[7] = c[0].normal[2];
    return ret;
}
static int boxbox_giant(unsigned char p, float *v) { return pair_case(p, v, giant_box, small_box, McdBoxBoxIntersect); }
static int sphylbox_giant(unsigned char p, float *v) { return pair_case(p, v, small_sphyl, giant_box, McdSphylBoxIntersect); }

int main(void)
{
    fw = McdInit(16, 256, 256, 1);
    McdPrimitivesRegisterTypes(fw);
    McdConvexMeshRegisterType(fw);
    McdAggregateRegisterType(fw);
    McdNullRegisterType(fw);
    McdPrimitivesRegisterInteractions(fw);
    McdConvexMeshPrimitivesRegisterInteractions(fw);
    McdAggregateRegisterInteractions(fw);
    printf("# float_sentinel pointer=%d\n", (int)sizeof(void *));

    /* an aggregate bone: one sphere of radius 0.5 at the origin */
    {
        McdGeometryID g = (McdGeometryID)McdAggregateCreate(fw, 1);
        identity(agg_rel, 0, 0, 0);
        McdAggregateAddElement((McdAggregateID)g, (McdGeometryID)McdSphereCreate(fw, 0.5f), agg_rel);
        agg_model = McdModelCreate(g);
        identity(agg_tm, 0, 0, 0);
        McdModelSetTransformPtr(agg_model, agg_tm);
    }
    run("aggregate-miss", agg_miss);
    run("aggregate-hit", agg_hit);

    /* a space of two spheres on the x axis. ⚠ i386 only: at LP64 McdSpace's line-segment queries step
       the space's CxSmallSortRep list at its i386 size (0x98) and fault — a pointer-width defect of
       their own, in an object UT2004 never enters (../../docs/STATE.md). The comparator reports the
       skip; it is not a pass. */
    if (sizeof(void *) != 4) {
        static const char *skipped[] = { "space-first-miss", "space-first-hit", "space-enabled-miss",
                                         "space-enabled-hit", "space-enabled-rejected" };
        int i;
        for (i = 0; i < 5; i++) printf("%s skip-lp64\n", skipped[i]);
    } else {
        int i;
        space = McdSpaceAxisSortCreate(fw, McdAllAxes, 16, 16);
        for (i = 0; i < 2; i++) {
            sm[i] = McdModelCreate((McdGeometryID)McdSphereCreate(fw, 0.5f));
            identity(stm[i], 3.0f * (float)i, 0, 0);
            McdModelSetTransformPtr(sm[i], stm[i]);
            McdSpaceInsertModel(space, sm[i]);
            McdModelUpdate(sm[i]);
        }
        McdSpaceUpdateAll(space);
        run("space-first-miss", space_miss);
        run("space-first-hit", space_hit);
        run("space-enabled-miss", space_en_miss);
        run("space-enabled-hit", space_en_hit);
        run("space-enabled-rejected", space_en_rejected);
    }

    run("simplify-all-infinite", simplify_inf);
    run("simplify-one-real", simplify_mixed);

    run("safetime-infinite", safetime_inf);
    run("safetime-real", safetime_real);

    {
        McdGeometryID g = (McdGeometryID)McdBoxCreate(fw, 1, 1, 1);
        MeReal *R = McdBoxGetRadii((McdBoxID)g);
        R[0] = R[1] = R[2] = FLT_MAX;
        giant_box = McdModelCreate(g);
        identity(gtm, 0, 0, 0);
        McdModelSetTransformPtr(giant_box, gtm);
        small_box = McdModelCreate((McdGeometryID)McdBoxCreate(fw, 0.2f, 0.2f, 0.2f));
        identity(btm, 0, 0, 0);
        McdModelSetTransformPtr(small_box, btm);
        small_sphyl = McdModelCreate((McdGeometryID)McdSphylCreate(fw, 0.1f, 0.2f));
        identity(ytm, 0, 0, 0);
        McdModelSetTransformPtr(small_sphyl, ytm);
    }
    run("boxbox-giant", boxbox_giant);
    run("sphylbox-giant", sphylbox_giant);
    return 0;
}
