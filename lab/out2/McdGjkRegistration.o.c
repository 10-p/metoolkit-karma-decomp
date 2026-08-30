/* ==== McdConvexMeshConvexMeshRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

MeBool McdConvexMeshConvexMeshRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)&McdCacheHello;
  interactions.goodbyeFn = (McdGoodbyeFn)&McdCacheGoodbye;
  interactions.intersectFn = (McdIntersectFn)&McdGjkCgIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,7,7,&interactions);
  return 1;
}


/* ==== McdConvexMeshPlaneRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

MeBool McdConvexMeshPlaneRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)&McdCacheHello;
  interactions.goodbyeFn = (McdGoodbyeFn)&McdCacheGoodbye;
  interactions.intersectFn = (McdIntersectFn)&McdGjkCgIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,7,3,&interactions);
  return 1;
}


/* ==== McdSphylConvexMeshRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

MeBool McdSphylConvexMeshRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdSphylConvexMeshIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,5,7,&interactions);
  return 1;
}


/* ==== McdSphereConvexMeshRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

MeBool McdSphereConvexMeshRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)&McdCacheHello;
  interactions.goodbyeFn = (McdGoodbyeFn)&McdCacheGoodbye;
  interactions.intersectFn = (McdIntersectFn)&McdGjkCgIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,1,7,&interactions);
  return 1;
}


/* ==== McdCylinderConvexMeshRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

MeBool McdCylinderConvexMeshRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)&McdCacheHello;
  interactions.goodbyeFn = (McdGoodbyeFn)&McdCacheGoodbye;
  interactions.intersectFn = (McdIntersectFn)&McdGjkCgIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,4,7,&interactions);
  return 1;
}


/* ==== McdBoxConvexMeshRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

MeBool McdBoxConvexMeshRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)&McdCacheHello;
  interactions.goodbyeFn = (McdGoodbyeFn)&McdCacheGoodbye;
  interactions.intersectFn = (McdIntersectFn)&McdGjkCgIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,2,7,&interactions);
  return 1;
}


