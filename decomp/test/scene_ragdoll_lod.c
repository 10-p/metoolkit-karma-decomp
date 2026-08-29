/*  scene_ragdoll_lod.c — scene_ragdoll with the LEVEL-OF-DETAIL path forced on.
 *
 *  WHY THIS EXISTS. `MdtLODLastPartition` is the one symbol the engine imports
 *  from `MdtLOD`, and it is called at exactly one site, KDynStep.cpp:310, under
 *
 *      if (po->info[p].rowCount > params->maxMatrixSize)
 *
 *  with `maxMatrixSize` set to 0x7ffffffc by MdtWorldCreate and lowered by
 *  nothing — engine source 0 hits with a control at 2, engine objects 0,
 *  metoolkit 0. Reaching that comparison needs on the order of 350 million
 *  constraint rows in one partition, so IN THE GAME THE FUNCTION CANNOT RUN,
 *  and a breakpoint on it fires zero times in a 235 s match while the control
 *  breakpoint fires >100,000 times. proven.txt has both.
 *
 *  That is a fine reachability argument and it is NOT a measurement of the
 *  code. `MdtWorldSetMaxMatrixSize` is a public API, so the guard can simply be
 *  made true here: one line, and the recovered function runs on a real
 *  partition built by the real solver, with the ragdoll's joints and contacts
 *  as its input. That turns "cannot execute, so ship it" into "executes, and
 *  the trajectory matches the shipped library's".
 *
 *  Everything below this block is scene_ragdoll.c unchanged. Its own header
 *  follows and still applies.
 *
 *  ORIGINAL HEADER
 *  ===============
 *  scene_ragdoll.c — a scripted scene that actually exercises the sphyl path.
 *
 *  WHY THIS EXISTS
 *  ---------------
 *  The two existing scenes are the wrong instrument for `IxSphylPrimitives`.
 *  `scene_chain` is collision-free by design and `scene_boxes_on_plane` is boxes.
 *  Neither makes a single Sphyl call, so an object that is 463,782 calls of the
 *  real game's collision load was being gated on evidence that never touched it.
 *
 *  This builds what a UT2004 ragdoll actually is — a chain of capsules on
 *  ball-socket joints — and drops it onto a plane with boxes in the way, so it
 *  generates Sphyl x Plane, Sphyl x Box and Sphyl x Sphyl contacts, which are
 *  three of the four functions in the object.
 *
 *  WHAT TO EXPECT FROM IT, AND WHAT NOT TO
 *  ---------------------------------------
 *  Do NOT expect bit-identical trajectories once contact starts. §10 of
 *  HANDOVER.md settles that: MathEngine's own i386 and x86-64 builds of the SAME
 *  source diverge by 111 m after 15 s of collision, because contact make/break is
 *  discontinuous. Demanding better of the recovered code than the vendor got
 *  from itself is not a standard, it is a superstition.
 *
 *  What this is for is the question that actually matters and that the shadow
 *  harness structurally cannot answer: the shadow lets the engine consume the
 *  ORIGINAL's output every frame, so an error never gets to compound. Here the
 *  recovered code drives, and an error that would destabilise a ragdoll has 15
 *  seconds to do it. So the signal is:
 *
 *      does it stay PHYSICALLY PLAUSIBLE — no NaN, nothing launched to
 *      infinity, bodies settling rather than jittering forever
 *
 *  which is what the summary at the end reports, alongside the trajectory for
 *  diffing.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <MePrecision.h>
#include <MdtTypes.h>
#include <MdtWorld.h>
#include <MdtBody.h>
#include <MdtConstraint.h>
#include <MdtBSJoint.h>
#include <McdCTypes.h>
#include <McdFrame.h>
#include <McdModel.h>
#include <McdPrimitives.h>
#include <McdSphyl.h>
#include <McdBox.h>
#include <McdPlane.h>
#include <MstTypes.h>
#include <MstUniverse.h>
#include <MstUtils.h>
#include <MstBridge.h>
#include <MstModelDynamics.h>

#define NLIMB 9          /* a UT2004 ragdoll is about this many bones */
#define NBOX  3
#define STEPS 900        /* 15 s at 60 Hz */

int main(void)
{
    MstUniverseSizes sz = MstUniverseDefaultSizes;
    sz.dynamicBodiesMaxCount    = 64;
    sz.dynamicConstraintsMaxCount = 256;
    MstUniverseID u = MstUniverseCreate(&sz);
    McdFrameworkID fw = MstUniverseGetFramework(u);
    McdPrimitivesRegisterTypes(fw);
    McdPrimitivesRegisterInteractions(fw);

    MdtWorldID w = MstUniverseGetWorld(u);
    MdtWorldSetGravity(w, 0, 0, -9.81f);
    /* THE ONLY CHANGE, and the number is the whole design of the scene.
         96 and above  LOD never triggers: 0 calls. That is the NEGATIVE
                       CONTROL — it says the switch, not the scene, is what
                       makes the function run.
         40            2 calls of MdtLODLastPartition and 48 of
                       ResizeConstraint, and the ragdoll still lands and
                       settles, so the trajectory is a physically meaningful
                       one. This is the default.
          8            900 and 7,200 calls — LOD discards the contact
                       constraints outright and the ragdoll free-falls. The
                       scene's own plausibility check FAILS on that, correctly,
                       and the run is still perfectly deterministic, so it is
                       the right setting for an A/B and the wrong one for
                       substitute_test.sh, which treats a non-zero baseline exit
                       as fatal. test/ab_lod.sh drives both. */
    {
        const char *mm = getenv("KD_MAXMATRIX");
        MdtWorldSetMaxMatrixSize(w, mm ? atoi(mm) : 40);
    }

    MeMatrix4 ident = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };

    /* the ground */
    McdModelID ground = MstFixedModelCreate(u,
        (McdGeometryID)McdPlaneCreate(fw), ident);
    (void)ground;

    /* obstacles, so the limbs hit something other than a flat floor.
       ⚠ THE TRANSFORM MUST OUTLIVE THE LOOP — `MstFixedModelCreate` keeps the
       POINTER. See scene_ragdoll.c for the 24 sanitizer reports this caused. */
    static MeMatrix4 boxtm[NBOX];
    for (int i = 0; i < NBOX; i++) {
        static const MeMatrix4 id = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
        memcpy(boxtm[i], id, sizeof(MeMatrix4));
        boxtm[i][3][0] = -1.5f + 1.5f * i;
        boxtm[i][3][1] =  0.3f * i;
        boxtm[i][3][2] =  0.2f;
        MstFixedModelCreate(u, (McdGeometryID)McdBoxCreate(fw, 1.2f, 1.2f, 0.4f),
                            boxtm[i]);
    }

    /* the ragdoll: capsules on ball-socket joints, dropped at an angle */
    McdModelID limb[NLIMB];
    MdtBodyID  body[NLIMB];
    for (int i = 0; i < NLIMB; i++) {
        McdGeometryID g = (McdGeometryID)McdSphylCreate(fw, 0.16f, 0.45f);
        limb[i] = MstModelAndBodyCreate(u, g, 1.0f);
        body[i] = McdModelGetBody(limb[i]);
        MeMatrix4Ptr tm = McdModelGetTransformPtr(limb[i]);
        /* a small tilt per limb so it lands on its side, not perfectly aligned:
           an axis-aligned drop would miss the edge cases entirely */
        float a = 0.21f * i;
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
    for (int i = 0; i + 1 < NLIMB; i++) {
        MdtBSJointID j = MdtBSJointCreate(w);
        MdtBSJointSetBodies(j, body[i], body[i + 1]);
        MeMatrix4Ptr t1 = McdModelGetTransformPtr(limb[i]);
        MeMatrix4Ptr t2 = McdModelGetTransformPtr(limb[i + 1]);
        MdtBSJointSetPosition(j, 0.5f * (t1[3][0] + t2[3][0]),
                                 0.5f * (t1[3][1] + t2[3][1]),
                                 0.5f * (t1[3][2] + t2[3][2]));
        MdtBSJointEnable(j);
    }

    printf("step");
    for (int i = 0; i < NLIMB; i++) printf(",b%d_x,b%d_y,b%d_z", i, i, i);
    printf("\n");

    int nonfinite = 0, escaped = 0;
    double last[NLIMB][3], moved_late = 0.0;
    for (int s = 0; s < STEPS; s++) {
        MstUniverseStep(u, 1.0f / 60.0f);
        printf("%d", s);
        for (int i = 0; i < NLIMB; i++) {
            MeMatrix4Ptr tm = McdModelGetTransformPtr(limb[i]);
            double x = tm[3][0], y = tm[3][1], z = tm[3][2];
            printf(",%.9g,%.9g,%.9g", x, y, z);
            if (!isfinite(x) || !isfinite(y) || !isfinite(z)) nonfinite++;
            else if (fabs(x) > 1e3 || fabs(y) > 1e3 || fabs(z) > 1e3) escaped++;
            /* how much is still moving in the last second: a ragdoll that has
               settled should be nearly still, one being shaken by a flapping
               contact will not be */
            if (s >= STEPS - 60 && s > 0) {
                double d = fabs(x - last[i][0]) + fabs(y - last[i][1])
                         + fabs(z - last[i][2]);
                if (isfinite(d)) moved_late += d;
            }
            last[i][0] = x; last[i][1] = y; last[i][2] = z;
        }
        printf("\n");
    }

    fprintf(stderr, "non-finite samples : %d\n", nonfinite);
    fprintf(stderr, "escaped (>1e3)     : %d\n", escaped);
    fprintf(stderr, "motion in last 1 s : %.6f\n", moved_late);
    fprintf(stderr, "VERDICT: %s\n",
            (nonfinite == 0 && escaped == 0) ? "plausible" : "BLOWN UP");
    return (nonfinite || escaped) ? 1 : 0;
}
