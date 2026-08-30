/* ==== McdConvexMeshConvexMeshRegisterInteraction ==== */

MeBool McdConvexMeshConvexMeshRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = McdCacheHello;
  interactions.goodbyeFn = McdCacheGoodbye;
  interactions.intersectFn = McdGjkCgIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,7,7,&interactions);
  return 1;
}


/* ==== McdConvexMeshPlaneRegisterInteraction ==== */

MeBool McdConvexMeshPlaneRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = McdCacheHello;
  interactions.goodbyeFn = McdCacheGoodbye;
  interactions.intersectFn = McdGjkCgIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,7,3,&interactions);
  return 1;
}


/* ==== McdSphylConvexMeshRegisterInteraction ==== */

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

MeBool McdSphereConvexMeshRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = McdCacheHello;
  interactions.goodbyeFn = McdCacheGoodbye;
  interactions.intersectFn = McdGjkCgIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,1,7,&interactions);
  return 1;
}


/* ==== McdCylinderConvexMeshRegisterInteraction ==== */

MeBool McdCylinderConvexMeshRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = McdCacheHello;
  interactions.goodbyeFn = McdCacheGoodbye;
  interactions.intersectFn = McdGjkCgIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,4,7,&interactions);
  return 1;
}


/* ==== McdBoxConvexMeshRegisterInteraction ==== */

MeBool McdBoxConvexMeshRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = McdCacheHello;
  interactions.goodbyeFn = McdCacheGoodbye;
  interactions.intersectFn = McdGjkCgIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,2,7,&interactions);
  return 1;
}


