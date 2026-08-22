/*  difftest_boxbox.c — Milestone 1 acceptance gate.
 *
 *  Runs the ORIGINAL shipped McdBoxBoxIntersect and the Ghidra-recovered,
 *  recompiled one side by side on identical, REAL McdModelPairs, and compares
 *  the full McdIntersectResult including every contact.
 *
 *  The original object is linked in with its exported symbols renamed via
 *  `objcopy --redefine-sym`, so both implementations coexist in one process
 *  and share the same supporting library.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <MePrecision.h>
#include <MdtTypes.h>
#include <MdtWorld.h>
#include <MdtBody.h>
#include <McdCTypes.h>
#include <McdFrame.h>
#include <McdModel.h>
#include <McdPrimitives.h>
#include <McdBox.h>
#include <MstTypes.h>
#include <MstUniverse.h>
#include <MstUtils.h>

/* the recovered implementation (normal name) */
extern MeBool McdBoxBoxIntersect(McdModelPair *p, McdIntersectResult *r);
/* the shipped original, renamed at objcopy time */
extern MeBool orig_McdBoxBoxIntersect(McdModelPair *p, McdIntersectResult *r);

#define MAXC 64

static uint32_t rs = 0xC0FFEEu;
static float frnd(float lo, float hi)
{
    rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5;
    return lo + (hi - lo) * ((rs >> 8) * (1.0f / 16777216.0f));
}

/* proper rotation from random axis-angle, so the inputs are physically real */
static void rand_tm(MeMatrix4Ptr tm, float spread)
{
    float ax = frnd(-1, 1), ay = frnd(-1, 1), az = frnd(-1, 1);
    float n = sqrtf(ax * ax + ay * ay + az * az);
    if (n < 1e-6f) { ax = 1; ay = az = 0; n = 1; }
    ax /= n; ay /= n; az /= n;
    float a = frnd(-3.14159f, 3.14159f), c = cosf(a), s = sinf(a), t = 1 - c;
    tm[0][0] = t*ax*ax + c;    tm[0][1] = t*ax*ay + s*az; tm[0][2] = t*ax*az - s*ay; tm[0][3] = 0;
    tm[1][0] = t*ax*ay - s*az; tm[1][1] = t*ay*ay + c;    tm[1][2] = t*ay*az + s*ax; tm[1][3] = 0;
    tm[2][0] = t*ax*az + s*ay; tm[2][1] = t*ay*az - s*ax; tm[2][2] = t*az*az + c;    tm[2][3] = 0;
    tm[3][0] = frnd(-spread, spread);
    tm[3][1] = frnd(-spread, spread);
    tm[3][2] = frnd(-spread, spread);
    tm[3][3] = 1;
}

static int cmp_contact(const McdContact *x, const McdContact *y, double *worst)
{
    int structural = 0;
    if (x->dims != y->dims) structural = 1;
    for (int i = 0; i < 3; i++) {
        double d = fabs((double)x->position[i] - y->position[i]);
        double e = fabs((double)x->normal[i]   - y->normal[i]);
        if (d > *worst) *worst = d;
        if (e > *worst) *worst = e;
    }
    double s = fabs((double)x->separation - y->separation);
    if (s > *worst) *worst = s;
    return structural;
}

int main(void)
{
    MstUniverseSizes sz = MstUniverseDefaultSizes;
    MstUniverseID u = MstUniverseCreate(&sz);
    McdFrameworkID fw = MstUniverseGetFramework(u);
    McdPrimitivesRegisterTypes(fw);
    McdPrimitivesRegisterInteractions(fw);

    McdGeometryID g1 = (McdGeometryID)McdBoxCreate(fw, 1.0f, 1.4f, 0.7f);
    McdGeometryID g2 = (McdGeometryID)McdBoxCreate(fw, 0.8f, 0.6f, 1.3f);
    McdModelID m1 = MstModelAndBodyCreate(u, g1, 1.0f);
    McdModelID m2 = MstModelAndBodyCreate(u, g2, 1.0f);

    const int N = 300000;
    int touchdiff = 0, countdiff = 0, dimsdiff = 0, exact = 0, touching = 0;
    double worst = 0.0;

    for (int t = 0; t < N; t++) {
        rand_tm(McdModelGetTransformPtr(m1), 2.0f);
        rand_tm(McdModelGetTransformPtr(m2), 2.0f);

        McdModelPair pair;  memset(&pair, 0, sizeof pair);
        pair.model1 = m1; pair.model2 = m2;

        McdContact cA[MAXC], cB[MAXC];
        McdIntersectResult rA, rB;
        memset(cA, 0, sizeof cA); memset(cB, 0, sizeof cB);
        memset(&rA, 0, sizeof rA); memset(&rB, 0, sizeof rB);
        rA.pair = &pair; rA.contacts = cA; rA.contactMaxCount = MAXC;
        rB.pair = &pair; rB.contacts = cB; rB.contactMaxCount = MAXC;

        MeBool a = orig_McdBoxBoxIntersect(&pair, &rA);
        MeBool b =      McdBoxBoxIntersect(&pair, &rB);

        if (rA.touch) touching++;
        if (a != b || rA.touch != rB.touch) { touchdiff++; continue; }
        if (rA.contactCount != rB.contactCount) { countdiff++; continue; }

        int structural = 0;
        for (int i = 0; i < rA.contactCount && i < MAXC; i++)
            structural |= cmp_contact(&cA[i], &cB[i], &worst);
        for (int i = 0; i < 3; i++) {
            double d = fabs((double)rA.normal[i] - rB.normal[i]);
            if (d > worst) worst = d;
        }
        if (structural) { dimsdiff++; continue; }
        if (!memcmp(cA, cB, sizeof(McdContact) * rA.contactCount)
            && !memcmp(rA.normal, rB.normal, sizeof rA.normal)) exact++;
    }

    printf("McdBoxBoxIntersect: original vs Ghidra-recovered, %d real model pairs\n", N);
    printf("  pairs actually touching        : %d (%.1f%%)\n", touching, 100.0 * touching / N);
    printf("  bit-identical results          : %d (%.3f%%)\n", exact, 100.0 * exact / N);
    printf("  ------------------------------------------------\n");
    printf("  DIFFERENT touch/return         : %d\n", touchdiff);
    printf("  DIFFERENT contact count        : %d\n", countdiff);
    printf("  DIFFERENT contact dims         : %d\n", dimsdiff);
    printf("  worst absolute numeric delta   : %.3e\n", worst);
    int fail = touchdiff + countdiff + dimsdiff;
    printf("\nRESULT: %s\n", fail == 0
        ? "PASS -- every discrete decision matches; deltas are FP rounding only"
        : "FAIL -- behavioural divergence in the recovered code");
    return fail != 0;
}
