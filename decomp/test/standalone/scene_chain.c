/* Same scenario, built twice: i386 (x87, 80-bit intermediates) vs x86-64 (SSE, pure f32).
   Both are SHIPPED MathEngine builds of the SAME source. Dumps CSV for trajectory diffing. */
#include <stdio.h>
#include <MePrecision.h>
#include <MdtTypes.h>
#include <MdtWorld.h>
#include <MdtBody.h>
#include <MdtConstraint.h>
#include <MdtHinge.h>
#include <MdtBSJoint.h>

#define NB 12      /* ragdoll-sized articulated chain */
#define STEPS 900  /* 15 s at 60 Hz */

int main(void){
    MdtWorldID w = MdtWorldCreate(64, 128, 1.0f, 1.0f);
    MdtWorldSetGravity(w, 0, 0, -9.81f);
    MdtBodyID b[NB];
    for(int i=0;i<NB;i++){
        b[i]=MdtBodyCreate(w); MdtBodyEnable(b[i]);
        /* deliberately irregular start: exercises the solver, makes divergence visible */
        MdtBodySetPosition(b[i], 0.37f*i, 0.11f*(i%3), 10.0f - 0.5f*i);
        MdtBodySetMass(b[i], 1.0f + 0.13f*i);
        MdtBodySetLinearVelocity(b[i], 0.21f*(i%5), -0.17f*(i%4), 0.0f);
        MdtBodySetAngularVelocity(b[i], 0.9f*(i%3), 0.4f*(i%7), 1.3f*(i%2));
    }
    /* ball-socket chain = ragdoll topology */
    for(int i=0;i+1<NB;i++){
        MdtBSJointID j = MdtBSJointCreate(w);
        MdtBSJointSetBodies(j, b[i], b[i+1]);
        MdtBSJointSetPosition(j, 0.37f*i+0.185f, 0.11f*(i%3), 10.0f-0.5f*i-0.25f);
        MdtBSJointEnable(j);
    }
    printf("step");
    for(int i=0;i<NB;i++) printf(",b%d_x,b%d_y,b%d_z", i,i,i);
    printf("\n");
    for(int s=0; s<STEPS; s++){
        MdtWorldStep(w, 1.0f/60.0f);
        printf("%d", s);
        for(int i=0;i<NB;i++){
            MeMatrix4Ptr tm = MdtBodyGetTransformPtr(b[i]);
            printf(",%.9g,%.9g,%.9g", (double)tm[3][0],(double)tm[3][1],(double)tm[3][2]);
        }
        printf("\n");
    }
    return 0;
}
