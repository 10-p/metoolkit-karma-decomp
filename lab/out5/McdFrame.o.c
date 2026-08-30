/* ==== McdInit ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

McdFrameworkID McdInit(int geoTypeMaxCount,int modelCount,int instanceCount,MeReal scale)

{
  McdFrameworkID pMVar1;
  char *pcVar2;
  McdGeometryVTable *pMVar3;
  McdInteractions *pMVar4;
  McdRequest *pMVar5;
  int iVar6;
  uint uVar7;
  
                    /* Unresolved local var: McdFramework * f@[DW_OP_reg3(EBX)] */
  pMVar1 = (McdFrameworkID)(*_MeMemoryAPI)(0x90);
  pMVar1->termActions = (McdTermActionLink *)0x0;
  pcVar2 = MeToolkitVersionString();
  pMVar1->toolkitVersionString = pcVar2;
  pMVar1->geometryRegisteredCountMax = geoTypeMaxCount + 9;
  pMVar1->geometryRegisteredCount = 0;
  pMVar3 = (McdGeometryVTable *)(*_MeMemoryAPI)((geoTypeMaxCount + 9) * 0x28);
  pMVar1->geometryVTableTable = pMVar3;
  for (iVar6 = (pMVar1->geometryRegisteredCountMax * 5 & 0x1fffffffU) << 1; iVar6 != 0;
      iVar6 = iVar6 + -1) {
    pMVar3->destroy = (McdGeometryDestroyFnPtr)0x0;
    pMVar3 = (McdGeometryVTable *)&pMVar3->getAABB;
  }
  (*_MePoolAPI)(&pMVar1->modelPool,modelCount,0x78,0);
  (*_MePoolAPI)(&pMVar1->instancePool,instanceCount,0x34,0);
  pMVar4 = (McdInteractions *)
           (*_MeMemoryAPI)(pMVar1->geometryRegisteredCountMax * pMVar1->geometryRegisteredCountMax *
                           0x1c);
  pMVar1->interactionTable = pMVar4;
  for (uVar7 = (uint)(pMVar1->geometryRegisteredCountMax * pMVar1->geometryRegisteredCountMax * 0x1c
                     ) >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
    pMVar4->helloFn = (McdHelloFn)0x0;
    pMVar4 = (McdInteractions *)&pMVar4->goodbyeFn;
  }
  pMVar1->mHelloCallbackFnPtr = (McdHelloCallbackFnPtr)0x0;
  (pMVar1->cachePool).t = MePoolNULL;
  pMVar5 = (McdRequest *)(*_MeMemoryAPI)(8);
  pMVar1->request = pMVar5;
  pMVar5->contactMaxCount = 4;
  pMVar1->request->faceNormalsFirst = 0;
  pMVar1->defaultRequest = pMVar1->request;
  pMVar1->firstGeometry = (McdGeometryID)0x0;
  pMVar1->firstModel = (McdModelID_conflict)0x0;
  pMVar1->modelCount = 0;
  pMVar1->geometryCount = 0;
  pMVar1->mDefaultPadding = scale * 0.005;
  pMVar1->mScale = scale;
  return pMVar1;
}


/* ==== McdFrameworkDestroyAllModelsAndGeometries ==== */

void McdFrameworkDestroyAllModelsAndGeometries(McdFrameworkID f)

{
  McdModelID_conflict pMVar1;
  McdGeometryID pMVar2;
  McdGeometryID pMVar3;
  bool bVar4;
  McdGeometryID pMVar5;
  int iVar6;
  int guard;
  
                    /* Unresolved local var: McdGeometryID g@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdGeometryID nextG@[DW_OP_reg6(ESI)] */
  guard = 0;
  pMVar1 = f->firstModel;
  while (pMVar1 != (McdModelID_conflict)0x0) {
    McdModelDestroy(f->firstModel);
    pMVar1 = f->firstModel;
  }
  pMVar2 = f->firstGeometry;
  pMVar5 = pMVar2;
  do {
    if (pMVar2 == (McdGeometryID)0x0) {
      return;
    }
    pMVar3 = pMVar5->next;
    iVar6 = McdGeometryGetReferenceCount(pMVar5);
    if (iVar6 == 0) {
      McdGeometryDestroy(pMVar5);
    }
    pMVar2 = f->firstGeometry;
    pMVar5 = pMVar3;
  } while ((pMVar3 != pMVar2) || (bVar4 = guard < 0x65, guard = guard + 1, bVar4));
  MeWarning(1,
            "Failed to destroy all geometries because reference counts could not be reduced to zero"
           );
  return;
}


/* ==== McdFrameworkGetModelCount ==== */

int McdFrameworkGetModelCount(McdFrameworkID f)

{
  return f->modelCount;
}


/* ==== McdFrameworkGetGeometryCount ==== */

int McdFrameworkGetGeometryCount(McdFrameworkID f)

{
  return f->geometryCount;
}


/* ==== McdTerm ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdTerm(McdFrameworkID frame)

{
  McdTermActionLink *pMVar1;
  
                    /* Unresolved local var: McdTermActionLink * actionLink@[DW_OP_reg3(EBX)] */
  for (pMVar1 = frame->termActions; pMVar1 != (McdTermActionLink *)0x0; pMVar1 = pMVar1->next) {
    (*pMVar1->action)(frame);
  }
  if ((frame->cachePool).t != MePoolNULL) {
    (*_McdGeometryGetReferenceCount)(&frame->cachePool);
  }
  frame->geometryRegisteredCountMax = 0;
  frame->geometryRegisteredCount = 0;
  (*_McdGeometryGetReferenceCount)(&frame->modelPool);
  (*_McdGeometryGetReferenceCount)(&frame->instancePool);
  (*_McdGeometryGetReferenceCount)(frame->request);
  (*_McdGeometryGetReferenceCount)(frame->geometryVTableTable);
  (*_McdGeometryGetReferenceCount)(frame->interactionTable);
  if (frame->termActions != (McdTermActionLink *)0x0) {
    (*_McdGeometryGetReferenceCount)(frame->termActions);
  }
  (*_McdGeometryGetReferenceCount)(frame);
  return;
}


/* ==== McdFrameworkRegisterTermAction ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdFrameworkRegisterTermAction(McdFrameworkID frame,McdTermAction action)

{
  McdTermActionLink *pMVar1;
  
                    /* Unresolved local var: McdTermActionLink * newLink@[DW_OP_reg0(EAX)] */
  pMVar1 = (McdTermActionLink *)(*_MeMemoryAPI)(8);
  pMVar1->action = action;
  pMVar1->next = frame->termActions;
  frame->termActions = pMVar1;
  return;
}


/* ==== McdFrameworkSetDefaultContactTolerance ==== */

void McdFrameworkSetDefaultContactTolerance(McdFrameworkID frame,MeReal inPadding)

{
  frame->mDefaultPadding = inPadding;
  return;
}


/* ==== McdFrameworkGetDefaultContactTolerance ==== */

MeReal McdFrameworkGetDefaultContactTolerance(McdFrameworkID frame)

{
  return frame->mDefaultPadding;
}


/* ==== McdFrameworkResetTypes ==== */

void McdFrameworkResetTypes(McdFrameworkID frame)

{
  frame->geometryRegisteredCount = 0;
  return;
}


/* ==== McdFrameworkTypeIsValid ==== */

MeBool McdFrameworkTypeIsValid(McdFrameworkID frame,int id)

{
  MeBool MVar1;
  
  MVar1 = 0;
  if (((-1 < id) && (id < frame->geometryRegisteredCountMax)) &&
     (frame->geometryVTableTable[id].registered != 0)) {
    MVar1 = 1;
  }
  return MVar1;
}


/* ==== McdFrameworkGetTypeName ==== */

char * McdFrameworkGetTypeName(McdFrameworkID frame,int id)

{
  bool bVar1;
  char *pcVar2;
  
  bVar1 = false;
  if (((-1 < id) && (id < frame->geometryRegisteredCountMax)) &&
     (frame->geometryVTableTable[id].registered != 0)) {
    bVar1 = true;
  }
  pcVar2 = "UnregisteredGeometryType";
  if (bVar1) {
    pcVar2 = frame->geometryVTableTable[id].name;
  }
  return pcVar2;
}


/* ==== McdFrameworkShowTypes ==== */

void McdFrameworkShowTypes(McdFrameworkID frame)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar1 = 0;
  MeInfo(0,"McdGeometryShowTypes:");
  MeInfo(0,"      ( #types registered: %d )");
  if (0 < frame->geometryRegisteredCount) {
    do {
      iVar1 = iVar1 + 1;
      MeInfo(0,"   index #%d: %s");
    } while (iVar1 < frame->geometryRegisteredCount);
  }
  MeInfo(0,&DAT_00010760);
  return;
}


/* ==== McdFrameworkTypeIsRegistered ==== */

MeBool McdFrameworkTypeIsRegistered(McdFrameworkID frame,int typeId)

{
  MeBool MVar1;
  
  if (((typeId < 0) || (frame->geometryRegisteredCountMax <= typeId)) ||
     (frame->geometryVTableTable[typeId].registered == 0)) {
    MVar1 = 0;
  }
  else {
    MVar1 = 1;
  }
  return MVar1;
}


/* ==== McdFrameworkGetRegisteredTypeCount ==== */

MeI16 McdFrameworkGetRegisteredTypeCount(McdFrameworkID frame)

{
  return (MeI16)frame->geometryRegisteredCount;
}


/* ==== McdFrameworkGetDefaultRequestPtr ==== */

McdRequest * McdFrameworkGetDefaultRequestPtr(McdFrameworkID frame)

{
  return frame->defaultRequest;
}


/* ==== McdFrameworkSetDefaultRequestPtr ==== */

void McdFrameworkSetDefaultRequestPtr(McdFrameworkID frame,McdRequest *r)

{
  frame->defaultRequest = r;
  return;
}


/* ==== McdFrameworkRegisterGeometryType ==== */

void McdFrameworkRegisterGeometryType
               (McdFramework *frame,McdGeometryType typeId,char *typeName,
               McdGeometryDestroyFnPtr f_destroy,McdGeometryGetAABBFnPtr f_getAABB,
               McdGeometryGetBSphereFnPtr f_getBSphere,McdGeometryMaximumPointFnPtr f_maximumPoint,
               McdGeometryGetMassPropertiesFnPtr f_getInertiaMatrix,
               McdGeometryDebugDrawFnPtr f_debugDraw)

{
  int iVar1;
  
  iVar1 = (int)typeId;
  if (frame->geometryRegisteredCountMax <= iVar1) {
    McdError(&gMcdCoreErrorList,0x15," in McdGeometryRegisterType.","McdGeometryRegisterType",
             "McdFrame.cpp",399);
  }
  frame->geometryVTableTable[iVar1].registered = 1;
  frame->geometryVTableTable[iVar1].destroy = f_destroy;
  frame->geometryVTableTable[iVar1].getAABB = f_getAABB;
  frame->geometryVTableTable[iVar1].getBSphere = f_getBSphere;
  frame->geometryVTableTable[iVar1].maximumPoint = f_maximumPoint;
  frame->geometryVTableTable[iVar1].getMassProperties = f_getInertiaMatrix;
  frame->geometryVTableTable[iVar1].debugDraw = f_debugDraw;
  frame->geometryVTableTable[iVar1].name = typeName;
  frame->geometryRegisteredCount = frame->geometryRegisteredCount + 1;
  return;
}


/* ==== McdFrameworkSetInteractions ==== */

void McdFrameworkSetInteractions
               (McdFramework *frame,int geoType1,int geoType2,McdInteractions *interactions)

{
  McdInteractions *pMVar1;
  
                    /* Unresolved local var: McdInteractions * element@[DW_OP_reg2(EDX)] */
  pMVar1 = frame->interactionTable + frame->geometryRegisteredCountMax * geoType2 + geoType1;
  pMVar1->helloFn = interactions->helloFn;
  pMVar1->goodbyeFn = interactions->goodbyeFn;
  pMVar1->intersectFn = interactions->intersectFn;
  pMVar1->safetimeFn = interactions->safetimeFn;
  pMVar1->swap = interactions->swap;
  pMVar1->cull = interactions->cull;
  pMVar1->warned = interactions->warned;
  pMVar1->swap = 0;
  if (geoType1 != geoType2) {
    pMVar1 = frame->interactionTable + frame->geometryRegisteredCountMax * geoType1 + geoType2;
    pMVar1->helloFn = interactions->helloFn;
    pMVar1->goodbyeFn = interactions->goodbyeFn;
    pMVar1->intersectFn = interactions->intersectFn;
    pMVar1->safetimeFn = interactions->safetimeFn;
    pMVar1->swap = interactions->swap;
    pMVar1->cull = interactions->cull;
    pMVar1->warned = interactions->warned;
    pMVar1->swap = 1;
  }
  return;
}


/* ==== McdFrameworkGetInteractions ==== */

McdInteractions * McdFrameworkGetInteractions(McdFramework *frame,int geoType1,int geoType2)

{
  return frame->interactionTable + frame->geometryRegisteredCountMax * geoType2 + geoType1;
}


/* ==== McdFrameworkGetInteractionsWarned ==== */

MeBool McdFrameworkGetInteractionsWarned(McdFramework *framework,int geoType1,int geoType2)

{
  return framework->interactionTable[framework->geometryRegisteredCountMax * geoType2 + geoType1].
         warned;
}


/* ==== McdFrameworkSetInteractionsWarned ==== */

void McdFrameworkSetInteractionsWarned
               (McdFramework *framework,int geoType1,int geoType2,MeBool warned)

{
  framework->interactionTable[framework->geometryRegisteredCountMax * geoType2 + geoType1].warned =
       warned;
  return;
}


/* ==== McdFrameworkSetLineSegInteraction ==== */

void McdFrameworkSetLineSegInteraction
               (McdFramework *frame,int geoType,McdLineSegIntersectFnPtr isectfn)

{
  frame->geometryVTableTable[geoType].lineSegIntersect = isectfn;
  return;
}


/* ==== McdFrameworkGetLineSegInteraction ==== */

McdLineSegIntersectFnPtr McdFrameworkGetLineSegInteraction(McdGeometryID geometry)

{
                    /* Unresolved local var: McdGeometryType geoType@[???]
                       Unresolved local var: McdFrameworkID frame@[DW_OP_reg2(EDX)] */
  return geometry->frame->geometryVTableTable[(byte)geometry->mRefCtAndID].lineSegIntersect;
}


