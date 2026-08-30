/* ==== MstUniverseCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MstUniverseID MstUniverseCreate(MstUniverseSizes *sizes)

{
  MstUniverseID pMVar1;
  MdtWorldID pMVar2;
  McdFrameworkID pMVar3;
  McdSpaceID pMVar4;
  MstBridgeID pMVar5;
  MstUniverseID pMVar6;
  
                    /* Unresolved local var: MstUniverse * u@[DW_OP_reg3(EBX)] */
  pMVar1 = (*_MeMemoryAPI)(0x34);
  pMVar6 = (MstUniverseID)0x0;
  if (pMVar1 != (MstUniverseID)0x0) {
    (pMVar1->sizes).dynamicBodiesMaxCount = sizes->dynamicBodiesMaxCount;
    (pMVar1->sizes).dynamicConstraintsMaxCount = sizes->dynamicConstraintsMaxCount;
    (pMVar1->sizes).collisionUserGeometryTypesMaxCount = sizes->collisionUserGeometryTypesMaxCount;
    (pMVar1->sizes).collisionModelsMaxCount = sizes->collisionModelsMaxCount;
    (pMVar1->sizes).collisionPairsMaxCount = sizes->collisionPairsMaxCount;
    (pMVar1->sizes).collisionGeometryInstancesMaxCount = sizes->collisionGeometryInstancesMaxCount;
    (pMVar1->sizes).materialsMaxCount = sizes->materialsMaxCount;
    (pMVar1->sizes).lengthScale = sizes->lengthScale;
    (pMVar1->sizes).massScale = sizes->massScale;
    pMVar2 = MdtWorldCreate(sizes->dynamicBodiesMaxCount,sizes->dynamicConstraintsMaxCount,
                            sizes->lengthScale,sizes->massScale);
    pMVar1->world = pMVar2;
    MdtWorldSetAutoDisable(pMVar1->world,1);
    pMVar3 = McdInit(sizes->collisionUserGeometryTypesMaxCount,sizes->collisionModelsMaxCount,
                     sizes->collisionGeometryInstancesMaxCount,sizes->lengthScale);
    pMVar1->frame = pMVar3;
    McdPrimitivesRegisterTypes(pMVar3);
    McdPrimitivesRegisterInteractions(pMVar1->frame);
    McdAggregateRegisterType(pMVar1->frame);
    McdAggregateRegisterInteractions(pMVar1->frame);
    McdNullRegisterType(pMVar1->frame);
    pMVar4 = McdSpaceAxisSortCreate
                       (pMVar1->frame,_McdAllAxes,sizes->collisionModelsMaxCount,
                        sizes->collisionPairsMaxCount);
    pMVar1->space = pMVar4;
    pMVar5 = MstBridgeCreate(pMVar1->frame,sizes->materialsMaxCount);
    pMVar1->bridge = pMVar5;
    MstBridgeSetModelPairBufferSize(pMVar1->bridge,(pMVar1->sizes).collisionPairsMaxCount);
    MstSetWorldHandlers(pMVar1->world);
    pMVar6 = pMVar1;
  }
  return pMVar6;
}


/* ==== MstUniverseDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MstUniverseDestroy(MstUniverseID u)

{
  MdtWorldDestroy(u->world);
  McdSpaceDestroy(u->space);
  McdFrameworkDestroyAllModelsAndGeometries(u->frame);
  McdTerm(u->frame);
  MstBridgeDestroy(u->bridge);
  (*_McdInit)(u);
  return;
}


/* ==== MstUniverseStep ==== */

void MstUniverseStep(MstUniverseID u,MeReal stepSize)

{
  MeProfileStartSectionFn("Collision",'\0');
  McdSpaceUpdateAll(u->space);
  MstBridgeUpdateContacts(u->bridge,u->space,u->world);
  MeProfileEndSectionFn("Collision");
  MeProfileStartSectionFn("Dynamics",'\0');
  MdtWorldStep(u->world,stepSize);
  MeProfileEndSectionFn("Dynamics");
  return;
}


/* ==== MstUniverseGetWorld ==== */

MdtWorldID MstUniverseGetWorld(MstUniverseID u)

{
  return u->world;
}


/* ==== MstUniverseGetSpace ==== */

McdSpaceID MstUniverseGetSpace(MstUniverseID u)

{
  return u->space;
}


/* ==== MstUniverseGetBridge ==== */

MstBridgeID MstUniverseGetBridge(MstUniverseID u)

{
  return u->bridge;
}


/* ==== MstUniverseGetFramework ==== */

McdFrameworkID MstUniverseGetFramework(MstUniverseID u)

{
  return u->frame;
}


