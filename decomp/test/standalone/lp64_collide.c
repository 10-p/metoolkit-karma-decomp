/*  lp64_collide.c — WHICH COLLISION RESULT DIFFERS BETWEEN i386 AND LP64?
 *
 *  WHY THIS EXISTS. `test/lp64_run.sh` now compares each scene's TRAJECTORY
 *  against the same sources built at i386, and two scenes diverge while
 *  `scene_chain` — which is collision-free — matches over all 900 steps. That
 *  localises the remaining pointer-width defect to the collision half and no
 *  further: a trajectory is the sum of everything, so the first differing
 *  POSITION names nothing.
 *
 *  This prints the collision half directly. It builds the ragdoll
 *  configuration — the one that diverges at step 1 — registers an intersect
 *  callback on the bridge, and dumps every McdIntersectResult the pipeline
 *  produces: which two models, whether they touch, how many contacts, and each
 *  contact's position, normal, separation and dims. Built at both widths from
 *  the same sources and diffed, the first differing line names the PAIR and the
 *  FIELD, which names the interaction object.
 *
 *  ⚠ THE CALLBACK IS THE POINT — it runs inside the real pipeline, after the
 *  real broadphase and the real batch. A driver that called the intersect
 *  functions directly would test the geometry objects and skip McdSpace,
 *  McdBatch, McdModelPairManager and MstBridge, which is where a shared defect
 *  would live: `scene_boxes_on_plane` diverges too and it makes no Sphyl call,
 *  so the plumbing is at least as likely as any one geometry.
 *
 *  MODELS ARE IDENTIFIED BY A TAG, not by their address, because the addresses
 *  differ between widths by construction. Every model gets `McdModelSetUserData`
 *  at creation and the callback prints the tag.
 *
 *  Ordering is part of the answer, not noise to be sorted away: if the two
 *  widths report the same contacts in a different ORDER, that is the broadphase
 *  or the pair manager, and sorting the output would hide exactly that.
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
#include <MdtConstraint.h>
#include <MdtBSJoint.h>
#include <McdCTypes.h>
#include <McdFrame.h>
#include <McdModel.h>
#include <McdModelPair.h>
#include <McdInteractions.h>
#include <McdPrimitives.h>
#include <McdSphyl.h>
#include <McdBox.h>
#include <McdPlane.h>
#include <MstTypes.h>
#include <MstUniverse.h>
#include <MstUtils.h>
#include <MstBridge.h>
#include <MstModelDynamics.h>

#define NLIMB 9
#define NBOX  3
#define STEPS 40          /* the divergence is at step 1; 40 is plenty */

static int g_step = 0;
static long g_pairs = 0;

static int tag_of(McdModelID m)
{
    return m ? (int)(intptr_t)McdModelGetUserData(m) : -1;
}

/*  ⚠ TWO CALLBACKS, BECAUSE "no intersect results" HAS THREE CAUSES and only
    one of them is the one you want. The per-PAIR callback fires once the
    broadphase and the batch have produced a pair and before the interaction
    table dispatches it; the INTERSECT callback fires with the result. So:

      pairs 0, results 0   the broadphase or the batch produced nothing
      pairs N, results 0   the pair reached the interaction table and the
                           dispatch did not reach an intersect function
      pairs N, results N   collision ran; compare the numbers

    Without the first count, "0 results" also reads as "the bridge's callback
    table is indexed wrongly at this width", which would be a defect in the
    INSTRUMENT rather than in the thing measured. */
static MeBool MEAPI pair_probe(McdIntersectResult *r, MdtContactGroupID g)
{
    (void)r; (void)g;
    g_pairs++;
    return 1;                       /* 1 = carry on and intersect this pair */
}

/*  ⚠ PRINT WITH %.9g AND NOTHING CLEVERER. The two builds are the same source
    with the same -mfpmath, so an identical result must print identically; a
    rounded field would hide the last-bit differences that say "this is float
    noise" rather than "this is a layout defect". */
static void MEAPI probe(McdIntersectResult *r)
{
    McdModelID m1 = 0, m2 = 0;
    int i;
    if (r->pair) McdModelPairGetModels(r->pair, &m1, &m2);
    printf("P2,%d,%d,%d,%d,%d", g_step, tag_of(m1), tag_of(m2),
           r->touch, r->contactCount);
    for (i = 0; i < r->contactCount && i < r->contactMaxCount; i++) {
        McdContact *c = &r->contacts[i];
        printf(",%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%d",
               (double)c->position[0], (double)c->position[1],
               (double)c->position[2], (double)c->normal[0],
               (double)c->normal[1], (double)c->normal[2],
               (double)c->separation, (int)c->dims);
    }
    printf("\n");
}

/*  PHASE 1 — the interaction functions, driven directly. `place` builds a
    transform from a rotation about Y and a translation, so every case below is
    a pure number and reproduces exactly at both widths. */
static void place(MeMatrix4 m, double a, double x, double y, double z)
{
    memset(m, 0, sizeof(MeMatrix4));
    m[0][0] = (MeReal)cos(a);  m[0][2] = (MeReal)-sin(a);
    m[1][1] = 1;
    m[2][0] = (MeReal)sin(a);  m[2][2] = (MeReal)cos(a);
    m[3][0] = (MeReal)x; m[3][1] = (MeReal)y; m[3][2] = (MeReal)z; m[3][3] = 1;
}

static void one_pair(McdFrameworkID fw, const char *name,
                     McdGeometryID g1, McdGeometryID g2,
                     double a1, double z1, double a2, double z2)
{
    static MeMatrix4 t1, t2;
    McdContact contacts[64];
    McdIntersectResult r;
    McdModelID m1 = McdModelCreate(g1), m2 = McdModelCreate(g2);
    McdModelPairID p;
    int i;

    place(t1, a1, 0, 0, z1);
    place(t2, a2, 0, 0, z2);
    McdModelSetTransformPtr(m1, t1);
    McdModelSetTransformPtr(m2, t2);
    McdModelUpdate(m1);
    McdModelUpdate(m2);

    p = McdModelPairCreate(m1, m2);
    memset(&r, 0, sizeof(r));
    r.pair = p;
    r.contacts = contacts;
    r.contactMaxCount = 64;
    McdIntersect(p, &r);

    printf("P1,%s,%.9g,%.9g,%d,%d", name, z1, z2, r.touch, r.contactCount);
    for (i = 0; i < r.contactCount && i < r.contactMaxCount; i++)
        printf(",%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%d",
               (double)contacts[i].position[0], (double)contacts[i].position[1],
               (double)contacts[i].position[2], (double)contacts[i].normal[0],
               (double)contacts[i].normal[1], (double)contacts[i].normal[2],
               (double)contacts[i].separation, (int)contacts[i].dims);
    printf("\n");
    McdModelPairDestroy(p);
    McdModelDestroy(m1);
    McdModelDestroy(m2);
}

static void direct_pairs(McdFrameworkID fw)
{
    /*  A sweep through contact rather than a single pose: the interesting
        failures are at the boundary, and a pair that is deeply interpenetrating
        agrees far more often than one that is barely touching. */
    double z;
    for (z = 0.40; z > 0.02; z -= 0.02) {
        McdGeometryID plane = (McdGeometryID)McdPlaneCreate(fw);
        McdGeometryID sphyl = (McdGeometryID)McdSphylCreate(fw, 0.16f, 0.45f);
        McdGeometryID box   = (McdGeometryID)McdBoxCreate(fw, 1.2f, 1.2f, 0.4f);
        McdGeometryID sphy2 = (McdGeometryID)McdSphylCreate(fw, 0.16f, 0.45f);
        McdGeometryID box2  = (McdGeometryID)McdBoxCreate(fw, 1.2f, 1.2f, 0.4f);
        one_pair(fw, "sphyl-plane", sphyl, plane, 0.21, z,      0.0, 0.0);
        one_pair(fw, "box-plane",   box,   plane, 0.00, z,      0.0, 0.0);
        one_pair(fw, "sphyl-box",   sphyl, box,   0.21, z + .4, 0.0, 0.0);
        one_pair(fw, "sphyl-sphyl", sphyl, sphy2, 0.21, z,      0.6, 0.0);
        one_pair(fw, "box-box",     box,   box2,  0.00, z + .4, 0.0, 0.0);
        McdGeometryDestroy(plane); McdGeometryDestroy(sphyl);
        McdGeometryDestroy(box);   McdGeometryDestroy(sphy2);
        McdGeometryDestroy(box2);
    }
}

int main(void)
{
    MstUniverseSizes sz = MstUniverseDefaultSizes;
    sz.dynamicBodiesMaxCount      = 64;
    sz.dynamicConstraintsMaxCount = 256;
    MstUniverseID u = MstUniverseCreate(&sz);
    McdFrameworkID fw = MstUniverseGetFramework(u);
    McdPrimitivesRegisterTypes(fw);
    McdPrimitivesRegisterInteractions(fw);

    /*  ---- PHASE 1: the interaction functions, with NO pipeline around them.
        `McdIntersect` on a pair built by hand skips McdSpace, McdBatch,
        McdModelPairManager and MstBridge entirely. If this half is identical at
        both widths and the scene below is not, the defect is in the plumbing
        and not in any geometry — which is the split that matters, because
        `scene_boxes_on_plane` diverges too and makes no Sphyl call. */
    direct_pairs(fw);

    int i, s, tag = 0;
    MdtWorldID w = MstUniverseGetWorld(u);
    MdtWorldSetGravity(w, 0, 0, -9.81f);

    /*  Every model carries the DEFAULT material, so one registration covers
        every pair the bridge will ever dispatch. */
    MstBridgeSetIntersectCB(MstUniverseGetBridge(u), 0, 0, probe);
    MstBridgeSetPerPairCB(MstUniverseGetBridge(u), 0, 0, pair_probe);

    static MeMatrix4 ident = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
    McdModelID ground = MstFixedModelCreate(u,
        (McdGeometryID)McdPlaneCreate(fw), ident);
    McdModelSetUserData(ground, (void *)(intptr_t)(tag++));

    static MeMatrix4 boxtm[NBOX];
    for (i = 0; i < NBOX; i++) {
        static const MeMatrix4 id = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
        McdModelID b;
        memcpy(boxtm[i], id, sizeof(MeMatrix4));
        boxtm[i][3][0] = -1.5f + 1.5f * i;
        boxtm[i][3][1] =  0.3f * i;
        boxtm[i][3][2] =  0.2f;
        b = MstFixedModelCreate(u,
                (McdGeometryID)McdBoxCreate(fw, 1.2f, 1.2f, 0.4f), boxtm[i]);
        McdModelSetUserData(b, (void *)(intptr_t)(tag++));
    }

    McdModelID limb[NLIMB];
    MdtBodyID  body[NLIMB];
    for (i = 0; i < NLIMB; i++) {
        McdGeometryID g = (McdGeometryID)McdSphylCreate(fw, 0.16f, 0.45f);
        MeMatrix4Ptr tm;
        float a = 0.21f * i;
        limb[i] = MstModelAndBodyCreate(u, g, 1.0f);
        McdModelSetUserData(limb[i], (void *)(intptr_t)(tag++));
        body[i] = McdModelGetBody(limb[i]);
        tm = McdModelGetTransformPtr(limb[i]);
        tm[0][0] = cosf(a); tm[0][2] = -sinf(a);
        tm[2][0] = sinf(a); tm[2][2] =  cosf(a);
        tm[3][0] = -1.2f + 0.30f * i;
        tm[3][1] =  0.08f * (i % 3);
        tm[3][2] =  3.4f + 0.55f * i;
        McdModelUpdate(limb[i]);
        MdtBodyEnable(body[i]);
        MdtBodySetLinearVelocity(body[i], 0.19f * (i % 5), -0.13f * (i % 4), -1.0f);
        MdtBodySetAngularVelocity(body[i], 0.7f * (i % 3), 0.5f * (i % 7), 1.1f * (i % 2));
    }
    for (i = 0; i + 1 < NLIMB; i++) {
        MdtBSJointID j = MdtBSJointCreate(w);
        MeMatrix4Ptr t1 = McdModelGetTransformPtr(limb[i]);
        MeMatrix4Ptr t2 = McdModelGetTransformPtr(limb[i + 1]);
        MdtBSJointSetBodies(j, body[i], body[i + 1]);
        MdtBSJointSetPosition(j, 0.5f * (t1[3][0] + t2[3][0]),
                                 0.5f * (t1[3][1] + t2[3][1]),
                                 0.5f * (t1[3][2] + t2[3][2]));
        MdtBSJointEnable(j);
    }

    printf("P2HEAD,step,m1,m2,touch,n,contacts...\n");
    for (s = 0; s < STEPS; s++) {
        g_step = s;
        MstUniverseStep(u, 1.0f / 60.0f);
    }
    printf("P2PAIRS,%ld\n", g_pairs);
    return 0;
}
