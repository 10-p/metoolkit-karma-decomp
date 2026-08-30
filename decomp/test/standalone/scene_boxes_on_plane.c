/* Full collision + contact test: boxes tumbling onto a plane, bouncing, settling.
   Contact make/break is DISCONTINUOUS -- this is the worst case for FP divergence. */
#include <stdio.h>
#include <MePrecision.h>
#include <MdtTypes.h>
#include <MdtWorld.h>
#include <MdtBody.h>
#include <McdFrame.h>
#include <McdPrimitives.h>
#include <McdBox.h>
#include <McdPlane.h>
#include <MstTypes.h>
#include <MstUniverse.h>
#include <MstUtils.h>
#include <MstBridge.h>

#define NB 8
#define STEPS 900
int main(void){
    MstUniverseSizes sz = MstUniverseDefaultSizes;
    MstUniverseID u = MstUniverseCreate(&sz);
    if(!u){ printf("universe create failed\n"); return 1; }
    McdFrameworkID fw = MstUniverseGetFramework(u);
    McdPrimitivesRegisterTypes(fw);
    McdPrimitivesRegisterInteractions(fw);
    MdtWorldID w = MstUniverseGetWorld(u);
    MdtWorldSetGravity(w, 0, 0, -9.81f);

    MeMatrix4 gtm = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
    McdGeometryID plane = (McdGeometryID)McdPlaneCreate(fw);
    MstFixedModelCreate(u, plane, gtm);

    McdModelID m[NB];
    for(int i=0;i<NB;i++){
        McdGeometryID g = (McdGeometryID)McdBoxCreate(fw, 1.0f+0.07f*i, 0.8f, 0.6f+0.05f*i);
        m[i] = MstModelAndBodyCreate(u, g, 1.0f);
        MdtBodyID b = McdModelGetBody(m[i]);
        MdtBodySetPosition(b, 0.31f*i, 0.13f*(i%3), 3.0f + 1.7f*i);
        MdtBodySetAngularVelocity(b, 1.1f*(i%3), 0.7f*(i%4), 0.3f*i);
        MdtBodySetLinearVelocity(b, 0.4f*(i%2), -0.3f*(i%3), 0);
    }
    printf("step");
    for(int i=0;i<NB;i++) printf(",b%d_x,b%d_y,b%d_z",i,i,i);
    printf("\n");
    for(int s=0;s<STEPS;s++){
        MstUniverseStep(u, 1.0f/60.0f);
        printf("%d", s);
        for(int i=0;i<NB;i++){
            MeMatrix4Ptr tm = MdtBodyGetTransformPtr(McdModelGetBody(m[i]));
            printf(",%.9g,%.9g,%.9g",(double)tm[3][0],(double)tm[3][1],(double)tm[3][2]);
        }
        printf("\n");
    }
    return 0;
}
