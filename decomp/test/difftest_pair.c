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
#include <McdTriangleList.h>
#include <McdGeometryTypes.h>
#include <McdConvexMesh.h>
#include <MstTypes.h>
#include <MstUniverse.h>
#include <MstUtils.h>

/* The engine hands these functions a 400-contact result buffer (maxCount 400 in
   every shadow-harness dump), and so does this driver. It was 64, which was
   fine only because both boxes had one fixed size: once reshape() lets a box
   reach 2.2 on a side it overlaps enough of the test mesh for
   McdBoxTriangleListIntersect to write past the end, and the run dies with
   "stack smashing detected" and no output, having discarded every result
   printed before it. Match the engine rather than pick a number. */
#define MAXC 400

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
IX(GjkCg,           "McdGjkCgIntersect")
IX(SphereTriList,   "McdSphereTriangleListIntersect")
IX(SphylTriList,    "McdSphylTriangleListIntersect")
IX(BoxTriList,      "McdBoxTriangleListIntersect")
IX(ConvexTriList,   "McdConvexMeshTriangleListIntersect")
/* Cylinder was believed unreachable until 2026-08-24, when a census measured
   Cylinder x TriangleList at 59,366 calls and Cylinder x Cylinder at 24,267
   (HANDOVER.md §3). Both pairs had no row here at all, which is precisely the
   state IxConvexTriList was in before it was released — "compiles and passes
   the scenes" and nothing else. These two rows are the deterministic half. */
IX(CylCyl,          "McdCylinderCylinderIntersect")
IX(CylTriList,      "McdCylinderTriangleListIntersect")

/* ---- geometry factories -------------------------------------------------- */
static float kd_spread = 1.0f;
/* KD_ORIGIN shifts the whole scene away from 0. It matters more than it looks:
   f32 spacing at |x|=2 is about 2e-7, and at |x|=260 — an ordinary UT2004 world
   coordinate — it is 1.5e-5, seventy times coarser, against a contact tolerance
   of 0.00475. A test that only ever runs near the origin is testing a precision
   regime the game never uses. The first structural divergence ever seen in
   IxBoxBox was at world (260, 8, 10), after 300,000 synthetic pairs near the
   origin found none. */
static float kd_origin = 0.0f;
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

/* Two shapes, for the same reason mk_box has two: a single fixed size hid the
   IxBoxBox divergence for the project's whole life (§8). The dimensions are the
   order of magnitude UT2004 ships — the cylinder collision elements in the
   assets run from r=0.32 h=2.24 to r=2.6 h=33.28 (tools/find_cylinder_geom.py). */
static McdGeometryID mk_cylinder(McdFrameworkID fw, int seed)
{
    return seed ? (McdGeometryID)McdCylinderCreate(fw, 0.40f, 1.7f)
                : (McdGeometryID)McdCylinderCreate(fw, 0.72f, 1.1f);
}

/* Every box, sphyl and sphere in this driver had ONE fixed size for its whole
   life, and that is
   named in proven.txt as the likeliest reason IxBoxBox's single real divergence
   never reproduced: 1 count_diff in 1,299 in-game calls at world
   (259.9, 8.0, 10.2), against 300,000 synthetic pairs near the origin and
   200,000 more at KD_ORIGIN=260, all clean. Two boxes of fixed proportion
   approaching at random angles is a narrower test than it looks — the ratio of
   the two half-extents decides which features can meet at all.

   So resize every box geometry each iteration. KD_FIXEDSHAPE=1 restores the old
   behaviour; note that varying the shape draws from the same RNG, so the whole
   transform sequence differs and numbers either side of this switch are not
   comparable iteration by iteration, only in aggregate. */
static int kd_fixedshape;

/* Declared here because reshape() uses them; built after the ConvexMesh type is
   registered, and documented at build_hulls() below. */
#define NHULLS 24
static McdGeometryID kd_hull[2][NHULLS];
static int kd_hulls_built;

static void reshape(McdModelID m, int side)
{
    if (kd_fixedshape) return;
    McdGeometryID g = McdModelGetGeometry(m);
    switch (McdGeometryGetTypeId(g)) {
    case kMcdGeometryTypeBox:
        McdBoxSetDimensions(g, frnd(0.3f, 2.2f), frnd(0.3f, 2.2f),
                            frnd(0.3f, 2.2f));
        break;
    /* Sphyl and sphere were fixed for exactly as long as the box was, and the
       same argument applies with more force: Sphyl x TriangleList, Sphyl x Sphyl
       and Sphyl x Sphere together are over 2 M real calls, and UT2004 varies
       these dimensions enormously — a ragdoll forearm and a vehicle collision
       hull are both sphyls. What decides which features can meet is the RATIO of
       radius to height, and one fixed pair of sphyls approaching at random
       angles never varies it at all.

       The ranges bracket the game's: a UT2004 ragdoll limb sphyl is roughly
       r 0.2-0.5, h 0.5-2.0 in Karma units, and vehicle hulls run larger. Going
       wider than the game does is deliberate — the point of the synthetic tier
       is corner cases the census cannot reach. */
    case kMcdGeometryTypeSphyl:
        McdSphylSetRadius(g, frnd(0.15f, 1.1f));
        McdSphylSetHeight(g, frnd(0.3f, 2.6f));
        break;
    case kMcdGeometryTypeSphere:
        McdSphereSetRadius(g, frnd(0.2f, 1.6f));
        break;
    case kMcdGeometryTypeConvexMesh:
        /* Swap in a different pre-built hull rather than rebuilding one. */
        if (kd_hulls_built)
            McdModelSetGeometry(m, kd_hull[side & 1][(int)frnd(0, NHULLS - 0.001f)]);
        break;
    default:
        break;
    }
}

static McdGeometryID mk_sphere(McdFrameworkID fw, int seed)
{
    return (McdGeometryID)McdSphereCreate(fw, seed ? 0.6f : 0.9f);
}

/* ---- a triangle-list geometry, which is what a UT2004 level IS ------------
   TriangleList is the busiest geometry in the game by a wide margin —
   McdSphereTriangleListIntersect alone is 1.8 M calls — and until now it could
   only be reached through the shadow harness, because the triangles come from a
   CALLBACK the engine owns rather than from the geometry. So the two busiest
   functions in Karma had no reproducible test at all.

   The callback contract is simple enough to satisfy honestly: fill in pointers
   to vertices and a normal, return how many. This builds a bumpy patch rather
   than a flat one, so the sphere lands on faces, edges and vertices instead of
   only faces — the edge and vertex cases are where the contact-dimension
   classification has to be right. */
#define NTRI 32
static MeVector3 kd_vert[NTRI][3];
static MeVector3 kd_norm[NTRI];

static void build_mesh(void)
{
    for (int t = 0; t < NTRI; t++) {
        int gx = t % 8, gy = t / 8;
        float x0 = -2.0f + gx * 0.5f, y0 = -1.0f + gy * 0.5f;
        /* a deterministic bump, so faces are not coplanar */
        float h[4];
        for (int k = 0; k < 4; k++) {
            float px = x0 + (k & 1) * 0.5f, py = y0 + ((k >> 1) & 1) * 0.5f;
            h[k] = 0.18f * sinf(2.1f * px) * cosf(1.7f * py);
        }
        int flip = (t & 1);
        int idx[3] = { 0, flip ? 3 : 1, flip ? 1 : 2 };
        for (int v = 0; v < 3; v++) {
            int k = idx[v];
            kd_vert[t][v][0] = x0 + (k & 1) * 0.5f;
            kd_vert[t][v][1] = y0 + ((k >> 1) & 1) * 0.5f;
            kd_vert[t][v][2] = h[k];
        }
        /* KD_SKEW=1 nudges the grid off its exact axis-aligned lines. Real
           level geometry is not on a 0.5 grid; this mesh was, which made a
           sphere land exactly on a shared edge far more often than anything in
           a game would. Use it to tell "disagrees at an exact feature boundary"
           from "disagrees". */
        if (getenv("KD_SKEW")) {
            for (int v = 0; v < 3; v++) {
                kd_vert[t][v][0] += 0.037f * sinf(3.3f * t + 1.7f * v);
                kd_vert[t][v][1] += 0.041f * cosf(2.9f * t + 2.3f * v);
            }
        }
        MeVector3 e1, e2;
        for (int j = 0; j < 3; j++) {
            e1[j] = kd_vert[t][1][j] - kd_vert[t][0][j];
            e2[j] = kd_vert[t][2][j] - kd_vert[t][0][j];
        }
        kd_norm[t][0] = e1[1] * e2[2] - e1[2] * e2[1];
        kd_norm[t][1] = e1[2] * e2[0] - e1[0] * e2[2];
        kd_norm[t][2] = e1[0] * e2[1] - e1[1] * e2[0];
        float n = sqrtf(kd_norm[t][0] * kd_norm[t][0] + kd_norm[t][1] * kd_norm[t][1]
                      + kd_norm[t][2] * kd_norm[t][2]);
        if (n < 1e-9f) n = 1.0f;
        for (int j = 0; j < 3; j++) kd_norm[t][j] /= n;
        if (kd_norm[t][2] < 0) for (int j = 0; j < 3; j++) kd_norm[t][j] = -kd_norm[t][j];
    }
}

/* The flags UT2004 puts on a level triangle. This is not decoration: they are
   the switch on more than half of what a TriangleList interaction does.

   KTriListGen.cpp sets `kMcdTriangleUseSmallestPenetration` plus all three
   UseEdge bits on every triangle it hands to Karma. This driver used to pass 0,
   and 0 means the edge-contact loop in McdConvexMeshTriangleListIntersect's
   GenerateTriangleContact — the half that calls ConvexHullNSegment and two of
   the three AccumulateSphylContacts sites — is skipped entirely, along with the
   two-sided branch and the edge-flag swap in the caller. So the object could
   pass 200,000 synthetic pairs while being a quarter wrong in a live match,
   which is exactly what happened.

   KD_TRIFLAGS overrides it, so the old all-zero behaviour is still reachable
   for telling "the edge path diverges" from "the face path diverges". */
static McdTriangleFlags kd_triflags =
    (McdTriangleFlags)(kMcdTriangleUseSmallestPenetration | kMcdTriangleUseEdges);

/* What the interaction handed the generator, per side. A TriangleList
   interaction works out a bounding sphere for the other body and asks the
   engine for triangles near it, so `pos` and `radius` are an OUTPUT of the
   recovered code that the contact comparison cannot see: if a recovery queries
   the wrong part of the level it gets the wrong triangles and every downstream
   number is wrong for a reason that never appears in the contacts.

   KD_GENARGS=1 compares them directly rather than through their consequences,
   which is the difference between "the recovery diverges" and knowing where. */
static int kd_gen_side, kd_gen_check;
static MeVector3 kd_gen_pos[2];
static MeReal kd_gen_radius[2];
static int kd_gen_n[2], kd_gen_calls[2];

static int MEAPI kd_trigen(McdModelPair *pair, McdUserTriangle *tri,
                           MeVector3 pos, MeReal radius, int maxTriangles)
{
    (void)pair;
    /* Cull to the query sphere, because the engine's generator does
       (KTriListQuery in KTriListGen.cpp) and because ignoring pos and radius
       makes the driver blind to how they were computed. Every TriangleList
       interaction works out a bounding sphere for the other body and hands it
       over here; with an ignore-the-arguments generator both sides get the
       same 32 triangles regardless and the error cannot show up.

       Cull on the triangle's centroid against radius plus the triangle's own
       extent, so the test is a real filter without being a knife edge that
       turns a last-bit difference in radius into a whole triangle appearing. */
    int n = 0;
    for (int t = 0; t < NTRI && n < maxTriangles; t++) {
        float cx = 0, cy = 0, cz = 0, ext = 0;
        for (int v = 0; v < 3; v++) {
            cx += kd_vert[t][v][0]; cy += kd_vert[t][v][1]; cz += kd_vert[t][v][2];
        }
        cx /= 3.0f; cy /= 3.0f; cz /= 3.0f;
        for (int v = 0; v < 3; v++) {
            float dx = kd_vert[t][v][0] - cx, dy = kd_vert[t][v][1] - cy,
                  dz = kd_vert[t][v][2] - cz;
            float d = dx * dx + dy * dy + dz * dz;
            if (d > ext) ext = d;
        }
        ext = sqrtf(ext);
        float dx = cx - pos[0], dy = cy - pos[1], dz = cz - pos[2];
        float lim = radius + ext;
        if (dx * dx + dy * dy + dz * dz > lim * lim) continue;
        tri[n].vertices[0] = &kd_vert[t][0];
        tri[n].vertices[1] = &kd_vert[t][1];
        tri[n].vertices[2] = &kd_vert[t][2];
        tri[n].normal      = &kd_norm[t];
        tri[n].triangleData.tag = t;
        tri[n].flags = kd_triflags;
        n++;
    }
    if (kd_gen_check && kd_gen_calls[kd_gen_side] == 0) {
        for (int j = 0; j < 3; j++) kd_gen_pos[kd_gen_side][j] = pos[j];
        kd_gen_radius[kd_gen_side] = radius;
        kd_gen_n[kd_gen_side] = n;
    }
    kd_gen_calls[kd_gen_side]++;
    return n;
}

static McdGeometryID mk_trilist(McdFrameworkID fw, int seed)
{
    (void)seed;
    MeVector3 lo = { -2.5f, -1.5f, -0.5f }, hi = { 2.5f, 1.5f, 0.5f };
    return (McdGeometryID)McdTriangleListCreate(fw, lo, hi, 64, kd_trigen);
}

/* A random convex polyhedron: points scattered on an ellipsoid, hulled. This is
   the shape a vehicle collision mesh actually has — a dozen-odd faces, no
   symmetry — rather than a box standing in for one. */
static McdGeometryID mk_convex_n(McdFrameworkID fw, int seed, int nv,
                                 float sx, float sy, float sz)
{
    MeVector3 v[64];
    if (nv > 64) nv = 64;
    for (int i = 0; i < nv; i++) {
        float a = frnd(-3.14159f, 3.14159f), z = frnd(-1.0f, 1.0f);
        float r = sqrtf(1.0f - z * z);
        v[i][0] = sx * r * cosf(a);
        v[i][1] = sy * r * sinf(a);
        v[i][2] = sz * z;
    }
    (void)seed;
    return (McdGeometryID)McdConvexMeshCreateHull(fw, v, nv, 0.0f);
}

static McdGeometryID mk_convex(McdFrameworkID fw, int seed)
{
    float sx = seed ? 1.3f : 0.9f, sy = seed ? 0.7f : 1.1f, sz = seed ? 1.0f : 0.6f;
    return mk_convex_n(fw, seed, 24, sx, sy, sz);
}

/* A POOL of hulls, because a convex mesh cannot be resized in place the way a
   box or a sphyl can — the hull has to be rebuilt, and rebuilding one per
   iteration would put qhull in the inner loop of a 200,000-iteration test.
   Twenty-four hulls per side, with the vertex count swept from 6 to 40 and the
   ellipsoid axes varied, is enough to stop the shape being a constant.

   The vertex count is the part that matters most and it is the part that has
   been fixed at 24 for the whole life of this driver. UT2004's convex meshes
   are nothing like uniform: a vehicle collision hull is a handful of vertices
   and a static-mesh hull can be dozens, and how many vertices a hull has
   decides how many candidate faces GJK has to separate. Box x ConvexMesh is the
   busiest pair in the census at 608,280 real calls and McdGjk still has two
   unexplained count divergences (proven.txt), so a fixed hull is exactly the
   blind spot the fixed BOX turned out to be. */
static void build_hulls(McdFrameworkID fw)
{
    for (int s = 0; s < 2; s++)
        for (int i = 0; i < NHULLS; i++)
            kd_hull[s][i] = mk_convex_n(fw, s, 6 + (i * 34) / (NHULLS - 1),
                                        frnd(0.5f, 1.6f), frnd(0.5f, 1.6f),
                                        frnd(0.5f, 1.6f));
    kd_hulls_built = 1;
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
    /* Box x ConvexMesh goes through GJK, and the census puts it at 77,424 real
       calls — the busiest pair in the game. McdGjkCgIntersect keeps a cache on
       the pair (McdCacheHello/Goodbye are registered alongside it), and this
       driver hands it a zeroed McdModelPair every iteration, so it is being
       exercised on the cold path only. That is a real limit on this evidence,
       not a reason to skip it. */
    { "McdGjkCgIntersect",           ix_orig_GjkCg,           ix_rec_GjkCg,
      mk_box,   mk_convex, 1.8f },
    /* The two busiest functions in the game, and a third that has never had any
       test at all. The trilist is fixed at the origin; only the other body
       moves, which is what happens in a level. */
    { "McdSphereTriangleListIntersect", ix_orig_SphereTriList, ix_rec_SphereTriList,
      mk_sphere, mk_trilist, 1.0f },
    { "McdSphylTriangleListIntersect",  ix_orig_SphylTriList,  ix_rec_SphylTriList,
      mk_sphyl,  mk_trilist, 1.0f },
    { "McdBoxTriangleListIntersect",    ix_orig_BoxTriList,    ix_rec_BoxTriList,
      mk_box,    mk_trilist, 1.0f },
    /* ConvexMesh x TriangleList is the tenth and last pair the census says the
       game calls, and the only one that had no entry here — which is exactly
       why "compiles and passes the scenes" was all IxConvexTriList ever had.
       Its in-game divergence is measured on ONS-UCMP-ABC-ECE, the one map found
       that reaches it, over a match that is non-deterministic and crashes about
       half the time. This row is the deterministic half of that evidence. */
    { "McdConvexMeshTriangleListIntersect", ix_orig_ConvexTriList, ix_rec_ConvexTriList,
      mk_convex, mk_trilist, 1.0f },
    /* The two Cylinder pairs the census caught executing on 2026-08-24 — 59,366
       and 24,267 calls. IxCylinderTriList is QUARANTINED by the reconstructed-
       frame detector and has never been measured by anything; IxCylinderCylinder
       compiles and has never been measured either. Neither row existed before. */
    { "McdCylinderCylinderIntersect",    ix_orig_CylCyl,    ix_rec_CylCyl,
      mk_cylinder, mk_cylinder, 1.4f },
    { "McdCylinderTriangleListIntersect", ix_orig_CylTriList, ix_rec_CylTriList,
      mk_cylinder, mk_trilist,  1.0f },
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
    tm[3][0] = kd_origin + frnd(-spread, spread);
    tm[3][1] = kd_origin + frnd(-spread, spread);
    tm[3][2] = kd_origin + frnd(-spread, spread);
    tm[3][3] = 1;
}

int main(int argc, char **argv)
{
    /* KD_SELFTEST=1 runs the ORIGINAL as both sides. Any divergence it reports
       is a fault in THIS driver, not in the recovered code — these functions are
       supposed to write only through their output parameter, so calling one
       twice on identical inputs must give the same answer. The shadow harness
       has had this switch from the start and it has already caught one wrong
       conclusion today; a driver without it is a machine for generating
       confident nonsense. */
    const char *og = getenv("KD_ORIGIN");
    kd_origin = og ? (float)atof(og) : 0.0f;
    const char *sp = getenv("KD_SPREAD");
    kd_spread = sp ? (float)atof(sp) : 1.0f;
    if (!(kd_spread > 0)) kd_spread = 1.0f;
    const char *tf = getenv("KD_TRIFLAGS");
    if (tf) kd_triflags = (McdTriangleFlags)strtol(tf, NULL, 0);
    kd_gen_check = getenv("KD_GENARGS") != NULL;
    kd_fixedshape = getenv("KD_FIXEDSHAPE") != NULL;
    const char *e = getenv("KD_SELFTEST");
    int selftest = (e && *e == '1');
    const char *want = (argc > 1 && strcmp(argv[1], "all")) ? argv[1] : NULL;
    int N = argc > 2 ? atoi(argv[2]) : 200000;
    int failures = 0, ran = 0;

    MstUniverseSizes sz = MstUniverseDefaultSizes;
    MstUniverseID u = MstUniverseCreate(&sz);
    McdFrameworkID fw = MstUniverseGetFramework(u);
    McdPrimitivesRegisterTypes(fw);
    McdPrimitivesRegisterInteractions(fw);
    build_mesh();
    McdConvexMeshRegisterType(fw);
    McdConvexMeshPrimitivesRegisterInteractions(fw);
    build_hulls(fw);          /* after the type is registered, before any pair */

    for (int k = 0; k < NPAIRS; k++) {
        if (want && strcmp(want, PAIRS[k].name)) continue;
        ran++;
        rs = 0xC0FFEEu;                      /* same inputs every run */

        McdModelID m1 = MstModelAndBodyCreate(u, PAIRS[k].g1(fw, 0), 1.0f);
        McdModelID m2 = MstModelAndBodyCreate(u, PAIRS[k].g2(fw, 1), 1.0f);

        int retdiff = 0, countdiff = 0, dimsdiff = 0, exact = 0, touching = 0;
        int genargs = 0;
        double worst_ok = 0.0;
        int dumped = 0;

        int fixed2 = (PAIRS[k].g2 == mk_trilist);   /* a level does not move */
        for (int t = 0; t < N; t++) {
            /* KD_SPREAD scales how far apart the bodies are scattered, which
               is how you move between contact REGIMES. The default puts the
               trilist tests at 92% touching with six to eleven contacts each —
               deep interpenetration. A real level is shallow resting contact,
               one or two contacts, ~20% touching. They are different tests and
               they find different things. */
            reshape(m1, 0); reshape(m2, 1);
            rand_tm(McdModelGetTransformPtr(m1), PAIRS[k].spread * kd_spread);
            if (!fixed2) rand_tm(McdModelGetTransformPtr(m2),
                                 PAIRS[k].spread * kd_spread);

            McdModelPair pair;  memset(&pair, 0, sizeof pair);
            pair.model1 = m1; pair.model2 = m2;

            McdContact cA[MAXC], cB[MAXC];
            McdIntersectResult rA, rB;
            memset(cA, 0, sizeof cA); memset(cB, 0, sizeof cB);
            memset(&rA, 0, sizeof rA); memset(&rB, 0, sizeof rB);
            rA.pair = &pair; rA.contacts = cA; rA.contactMaxCount = MAXC;
            rB.pair = &pair; rB.contacts = cB; rB.contactMaxCount = MAXC;

            kd_gen_side = 0; kd_gen_calls[0] = kd_gen_calls[1] = 0;
            int a = PAIRS[k].orig(&pair, &rA);
            kd_gen_side = 1;
            int b = selftest ? PAIRS[k].orig(&pair, &rB)
                             : PAIRS[k].rec (&pair, &rB);

            if (kd_gen_check && kd_gen_calls[0] && kd_gen_calls[1]) {
                int off = (kd_gen_radius[0] != kd_gen_radius[1]
                           || kd_gen_n[0] != kd_gen_n[1]);
                for (int j = 0; j < 3; j++)
                    if (kd_gen_pos[0][j] != kd_gen_pos[1][j]) off = 1;
                if (off) {
                    genargs++;
                    if (genargs <= 4)
                        printf("  GENARGS %s iter %d: orig pos %.9g %.9g %.9g r "
                               "%.9g -> %d tri | rec pos %.9g %.9g %.9g r %.9g "
                               "-> %d tri\n", PAIRS[k].name, t,
                               kd_gen_pos[0][0], kd_gen_pos[0][1], kd_gen_pos[0][2],
                               kd_gen_radius[0], kd_gen_n[0],
                               kd_gen_pos[1][0], kd_gen_pos[1][1], kd_gen_pos[1][2],
                               kd_gen_radius[1], kd_gen_n[1]);
                }
            }
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
        printf("%s%s: %d pairs, %d touching (%.1f%%)\n",
               PAIRS[k].name, selftest ? " [SELFTEST: original vs original]" : "",
               N, touching, 100.0 * touching / N);
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
