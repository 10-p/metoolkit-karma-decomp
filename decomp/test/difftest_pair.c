/*  difftest_pair.c — precision-tier gate, one interaction at a time.
 *
 *  Runs the ORIGINAL shipped intersection function and the recovered one side
 *  by side on identical, real McdModelPairs, and compares the full
 *  McdIntersectResult including every contact.
 *
 *  WHY THIS EXISTS ALONGSIDE THE SHADOW HARNESS
 *  --------------------------------------------
 *  The shadow harness is better evidence — real inputs, from a real match — but
 *  it can only test what the match happens to do. `McdSphylConvexMeshIntersect`
 *  is a ragdoll limb against a vehicle hull, and twenty minutes of bots dying on
 *  DM-BB-VehicleWar-test-physics produced 350,000 Sphyl x TriangleList calls and
 *  ZERO of it. Waiting for the right accident is not a plan.
 *
 *  So this drives the pair directly. It is weaker evidence and it is labelled as
 *  such in proven.txt: synthetic transforms are not gameplay, and IxBoxBox is
 *  the standing reminder — proven on 300,000 synthetic pairs, and the census
 *  later showed the game makes about 1,500 real calls to it.
 *
 *  ADDING A PAIR is one line in the table below. The geometry factories are
 *  shared, so a new interaction usually needs no new code at all.
 *
 *  Build with test/difftest_pair.sh, which stages the recovered object with its
 *  symbols prefixed rec_ so both implementations coexist in one process and
 *  share the same supporting library.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
#include <McdSphere.h>
#include <McdSphyl.h>
#include <McdPlane.h>
#include <McdConvexMesh.h>
#include <MstTypes.h>
#include <MstUniverse.h>
#include <MstUtils.h>

#define MAXC 64

typedef int (MEAPI *ix_fn)(McdModelPair *, McdIntersectResult *);
typedef McdGeometryID (*geom_fn)(McdFrameworkID, int seed);

/* ---- the pairs under test ------------------------------------------------
   Both spellings are asm labels because the id is not always the symbol: the
   convex-mesh interactions are C++ and ship mangled. */
#define IX(id, sym) \
    extern int MEAPI ix_orig_##id(McdModelPair *, McdIntersectResult *) __asm__(sym); \
    extern int MEAPI ix_rec_##id(McdModelPair *, McdIntersectResult *) __asm__("rec_" sym);

IX(SphylConvexMesh, "_Z27McdSphylConvexMeshIntersectP13_McdModelPairP19_McdIntersectResult")
IX(BoxBox,          "McdBoxBoxIntersect")
IX(SphereSphere,    "McdSphereSphereIntersect")
IX(SphylSphyl,      "McdSphylSphylIntersect")
IX(SphylSphere,     "McdSphylSphereIntersect")
IX(SphylBox,        "McdSphylBoxIntersect")
IX(SphylPlane,      "McdSphylPlaneIntersect")

/* ---- geometry factories -------------------------------------------------- */
static uint32_t rs = 0xC0FFEEu;
static float frnd(float lo, float hi)
{
    rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5;
    return lo + (hi - lo) * ((rs >> 8) * (1.0f / 16777216.0f));
}

static McdGeometryID mk_sphyl(McdFrameworkID fw, int seed)
{
    (void)seed;
    return (McdGeometryID)McdSphylCreate(fw, 0.35f, 1.1f);
}

static McdGeometryID mk_box(McdFrameworkID fw, int seed)
{
    return seed ? (McdGeometryID)McdBoxCreate(fw, 0.8f, 0.6f, 1.3f)
                : (McdGeometryID)McdBoxCreate(fw, 1.0f, 1.4f, 0.7f);
}

static McdGeometryID mk_plane(McdFrameworkID fw, int seed)
{
    (void)seed;
    return (McdGeometryID)McdPlaneCreate(fw);
}

static McdGeometryID mk_sphere(McdFrameworkID fw, int seed)
{
    return (McdGeometryID)McdSphereCreate(fw, seed ? 0.6f : 0.9f);
}

/* A random convex polyhedron: points scattered on an ellipsoid, hulled. This is
   the shape a vehicle collision mesh actually has — a dozen-odd faces, no
   symmetry — rather than a box standing in for one. */
static McdGeometryID mk_convex(McdFrameworkID fw, int seed)
{
    MeVector3 v[24];
    float sx = seed ? 1.3f : 0.9f, sy = seed ? 0.7f : 1.1f, sz = seed ? 1.0f : 0.6f;
    for (int i = 0; i < 24; i++) {
        float a = frnd(-3.14159f, 3.14159f), z = frnd(-1.0f, 1.0f);
        float r = sqrtf(1.0f - z * z);
        v[i][0] = sx * r * cosf(a);
        v[i][1] = sy * r * sinf(a);
        v[i][2] = sz * z;
    }
    return (McdGeometryID)McdConvexMeshCreateHull(fw, v, 24, 0.0f);
}

static const struct {
    const char *name;
    ix_fn       orig, rec;
    geom_fn     g1, g2;
    float       spread;         /* how far apart to scatter them */
} PAIRS[] = {
    { "McdSphylConvexMeshIntersect", ix_orig_SphylConvexMesh, ix_rec_SphylConvexMesh,
      mk_sphyl, mk_convex, 1.6f },
    { "McdBoxBoxIntersect",          ix_orig_BoxBox,          ix_rec_BoxBox,
      mk_box,   mk_box,    2.0f },
    { "McdSphereSphereIntersect",    ix_orig_SphereSphere,    ix_rec_SphereSphere,
      mk_sphere, mk_sphere, 1.6f },
    /* IxSphylPrimitives: quarantined, and this is how the threshold-flapping
       question in HANDOVER.md 8 gets a sample size the game cannot provide. */
    { "McdSphylSphylIntersect",      ix_orig_SphylSphyl,      ix_rec_SphylSphyl,
      mk_sphyl, mk_sphyl,  1.4f },
    { "McdSphylSphereIntersect",     ix_orig_SphylSphere,     ix_rec_SphylSphere,
      mk_sphyl, mk_sphere, 1.4f },
    { "McdSphylBoxIntersect",        ix_orig_SphylBox,        ix_rec_SphylBox,
      mk_sphyl, mk_box,    1.6f },
    { "McdSphylPlaneIntersect",      ix_orig_SphylPlane,      ix_rec_SphylPlane,
      mk_sphyl, mk_plane,  1.2f },
};
#define NPAIRS ((int)(sizeof PAIRS / sizeof PAIRS[0]))

/* Dump a divergence with its inputs. The RNG is seeded, so iteration number
   plus transforms is enough to replay it exactly — which is the difference
   between a finding and a rumour. */
#define DUMP_MAX 4
static void dump(int *n, const char *fn, const char *what, int iter,
                 McdModelID m1, McdModelID m2,
                 const McdIntersectResult *ra, const McdIntersectResult *rb)
{
    if (*n >= DUMP_MAX) return;
    (*n)++;
    MeMatrix4Ptr t1 = McdModelGetTransformPtr(m1);
    MeMatrix4Ptr t2 = McdModelGetTransformPtr(m2);
    printf("  !! %s: %s differs at iteration %d "
           "(ret %d/%d touch %d/%d count %d/%d)\n",
           fn, what, iter, 0, 0, ra->touch, rb->touch,
           ra->contactCount, rb->contactCount);
    printf("     tm1:");
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)
        printf(" %.9g", (double)t1[i][j]);
    printf("\n     tm2:");
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)
        printf(" %.9g", (double)t2[i][j]);
    printf("\n");
    int nc = ra->contactCount < rb->contactCount ? ra->contactCount : rb->contactCount;
    for (int i = 0; i < nc && i < 4; i++)
        printf("     [%d] dims %d/%d  sep %.9g/%.9g  pos %.6g %.6g %.6g / %.6g %.6g %.6g\n",
               i, ra->contacts[i].dims, rb->contacts[i].dims,
               (double)ra->contacts[i].separation, (double)rb->contacts[i].separation,
               (double)ra->contacts[i].position[0], (double)ra->contacts[i].position[1],
               (double)ra->contacts[i].position[2],
               (double)rb->contacts[i].position[0], (double)rb->contacts[i].position[1],
               (double)rb->contacts[i].position[2]);
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

int main(int argc, char **argv)
{
    const char *want = (argc > 1 && strcmp(argv[1], "all")) ? argv[1] : NULL;
    int N = argc > 2 ? atoi(argv[2]) : 200000;
    int failures = 0, ran = 0;

    MstUniverseSizes sz = MstUniverseDefaultSizes;
    MstUniverseID u = MstUniverseCreate(&sz);
    McdFrameworkID fw = MstUniverseGetFramework(u);
    McdPrimitivesRegisterTypes(fw);
    McdPrimitivesRegisterInteractions(fw);
    McdConvexMeshRegisterType(fw);
    McdConvexMeshPrimitivesRegisterInteractions(fw);

    for (int k = 0; k < NPAIRS; k++) {
        if (want && strcmp(want, PAIRS[k].name)) continue;
        ran++;
        rs = 0xC0FFEEu;                      /* same inputs every run */

        McdModelID m1 = MstModelAndBodyCreate(u, PAIRS[k].g1(fw, 0), 1.0f);
        McdModelID m2 = MstModelAndBodyCreate(u, PAIRS[k].g2(fw, 1), 1.0f);

        int retdiff = 0, countdiff = 0, dimsdiff = 0, exact = 0, touching = 0;
        double worst_ok = 0.0;
        int dumped = 0;

        for (int t = 0; t < N; t++) {
            rand_tm(McdModelGetTransformPtr(m1), PAIRS[k].spread);
            rand_tm(McdModelGetTransformPtr(m2), PAIRS[k].spread);

            McdModelPair pair;  memset(&pair, 0, sizeof pair);
            pair.model1 = m1; pair.model2 = m2;

            McdContact cA[MAXC], cB[MAXC];
            McdIntersectResult rA, rB;
            memset(cA, 0, sizeof cA); memset(cB, 0, sizeof cB);
            memset(&rA, 0, sizeof rA); memset(&rB, 0, sizeof rB);
            rA.pair = &pair; rA.contacts = cA; rA.contactMaxCount = MAXC;
            rB.pair = &pair; rB.contacts = cB; rB.contactMaxCount = MAXC;

            int a = PAIRS[k].orig(&pair, &rA);
            int b = PAIRS[k].rec (&pair, &rB);

            if (rA.touch) touching++;
            if (a != b || rA.touch != rB.touch) {
                retdiff++; dump(&dumped, PAIRS[k].name, "ret/touch", t, m1, m2,
                                &rA, &rB); continue;
            }
            if (rA.contactCount != rB.contactCount) {
                countdiff++; dump(&dumped, PAIRS[k].name, "contact count", t, m1, m2,
                                  &rA, &rB); continue;
            }

            double pair_worst = 0.0;
            int structural = 0;
            for (int i = 0; i < rA.contactCount && i < MAXC; i++) {
                if (cA[i].dims != cB[i].dims) { structural = 1; break; }
                for (int j = 0; j < 3; j++) {
                    double d = fabs((double)cA[i].position[j] - cB[i].position[j]);
                    double e = fabs((double)cA[i].normal[j]   - cB[i].normal[j]);
                    if (d > pair_worst) pair_worst = d;
                    if (e > pair_worst) pair_worst = e;
                }
                double s = fabs((double)cA[i].separation - cB[i].separation);
                if (s > pair_worst) pair_worst = s;
            }
            if (rA.contactCount > 0)
                for (int j = 0; j < 3; j++) {
                    double d = fabs((double)rA.normal[j] - rB.normal[j]);
                    if (d > pair_worst) pair_worst = d;
                }
            if (structural) {
                dimsdiff++; dump(&dumped, PAIRS[k].name, "contact dims", t, m1, m2,
                                 &rA, &rB); continue;
            }
            /* Only pairs that AGREED contribute to the numeric spread. Letting a
               structural divergence set it conflates "these two answers differ
               by a rounding error" with "these two answers are different
               answers", and the second one is not a delta at all.
               A delta this large is not rounding either — the same run gets one
               ULP on Sphere x Sphere — so dump it with its inputs rather than
               burying it in a maximum. */
            if (pair_worst > worst_ok) worst_ok = pair_worst;
            if (pair_worst > 1e-3)
                dump(&dumped, PAIRS[k].name, "contact position", t, m1, m2, &rA, &rB);
            if (!memcmp(cA, cB, sizeof(McdContact) * rA.contactCount)) exact++;
        }

        int bad = retdiff + countdiff + dimsdiff;
        failures += bad;
        printf("%s: %d pairs, %d touching (%.1f%%)\n",
               PAIRS[k].name, N, touching, 100.0 * touching / N);
        printf("  bit-identical          : %d (%.3f%%)\n", exact, 100.0 * exact / N);
        printf("  DIFFERENT ret/touch    : %d\n", retdiff);
        printf("  DIFFERENT contact count: %d\n", countdiff);
        printf("  DIFFERENT contact dims : %d\n", dimsdiff);
        printf("  worst numeric delta    : %.3e  (over pairs that agreed)\n", worst_ok);
        printf("  -> %s\n\n", bad == 0
               ? "PASS -- every discrete decision matches"
               : "FAIL -- behavioural divergence");
    }

    if (!ran) {
        fprintf(stderr, "no such pair; known pairs are:\n");
        for (int k = 0; k < NPAIRS; k++) fprintf(stderr, "  %s\n", PAIRS[k].name);
        return 2;
    }
    return failures != 0;
}
