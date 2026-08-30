/* ==== McdSpaceAxisSortCreate ==== */

McdSpaceID McdSpaceAxisSortCreate(McdFramework *fwk,int axes,int objectCount,int pairCount)

{
  McdSpaceID pMVar1;
  
                    /* Unresolved local var: CxSmallSort * s@[DW_OP_reg3(EBX)] */
  pMVar1 = _ZN11CxSmallSort3NewEP13_McdFrameworkhii(fwk,(char)axes,objectCount,pairCount);
  *(code **)(pMVar1 + 200) = McdModelUpdatePathCompatible;
  McdModelPairManagerSetPoolFullHandler(*(void **)(pMVar1 + 0x2c),defaultPoolFullHandler);
  _ZN11CxSmallSort11UpdateBeginEv(pMVar1);
  return pMVar1;
}


/* ==== McdSpaceDestroy ==== */

void McdSpaceDestroy(McdSpaceID s)

{
  _ZN11CxSmallSort6DeleteEv(s);
  return;
}


/* ==== McdSpaceBuild ==== */

void McdSpaceBuild(McdSpaceID s)

{
  return;
}


/* ==== McdSpaceIsChanging ==== */

MeBool McdSpaceIsChanging(McdSpaceID s)

{
  return (uint)(*(int *)(s + 0xcc) != 0);
}


/* ==== McdSpaceInsertModel ==== */

MeBool McdSpaceInsertModel(McdSpaceID s,McdModelID_conflict cm)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  
                    /* Unresolved local var: uint result@[DW_OP_reg7(EDI)] */
  pcVar2 = McdModelGetGeometry(cm);
  if (*pcVar2 != '\0') {
    if (cm->mSpace != (McdSpaceID)0x0) {
      return (uint)(cm->mSpace == s);
    }
    iVar3 = _ZN11CxSmallSort6InsertEP9_McdModelPK15McdCullingTablejj(s,cm,(void *)0x0,0,0);
    cVar1 = _ZN11CxSmallSort7ValidIDEi(s,iVar3);
    if (cVar1 != '\0') {
      cm->mSpace = s;
      cm->mSpaceID = iVar3;
      return 1;
    }
  }
  return 0;
}


/* ==== McdSpaceInsertModelWithCulling ==== */

MeBool McdSpaceInsertModelWithCulling
                 (McdSpaceID s,McdModelID_conflict cm,McdCullingTable *table,int cullingIndex,
                 int cullingID)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  
                    /* Unresolved local var: uint result@[DW_OP_reg7(EDI)] */
  if (cm->mSpace == (McdSpaceID)0x0) {
    iVar2 = _ZN11CxSmallSort6InsertEP9_McdModelPK15McdCullingTablejj
                      (s,cm,table,cullingIndex,cullingID);
    cVar1 = _ZN11CxSmallSort7ValidIDEi(s,iVar2);
    if (cVar1 == '\0') {
      uVar3 = 0;
    }
    else {
      cm->mSpace = s;
      cm->mSpaceID = iVar2;
      uVar3 = 1;
    }
  }
  else {
    uVar3 = (uint)(cm->mSpace == s);
  }
  return uVar3;
}


/* ==== McdSpaceSetModelCullingParameters ==== */

void McdSpaceSetModelCullingParameters
               (McdSpaceID s,McdModelID_conflict cm,McdCullingTable *table,int cullingIndex,
               int cullingID)

{
  _ZN11CxSmallSort20SetCullingParametersEiPK15McdCullingTablejj
            (s,cm->mSpaceID,table,cullingIndex,cullingID);
  return;
}


/* ==== McdSpaceUpdateModel ==== */

void McdSpaceUpdateModel(McdModelID_conflict cm)

{
  _ZN11CxSmallSort12UpdateSingleEif(cm->mSpace,cm->mSpaceID,0.0);
  return;
}


/* ==== McdSpaceFreezeModel ==== */

MeBool McdSpaceFreezeModel(McdModelID_conflict m)

{
  byte bVar1;
  
  bVar1 = _ZN11CxSmallSort10SetDynamicEib(m->mSpace,m->mSpaceID,'\0');
  return (uint)bVar1;
}


/* ==== McdSpaceUnfreezeModel ==== */

MeBool McdSpaceUnfreezeModel(McdModelID_conflict cm)

{
  byte bVar1;
  
  bVar1 = _ZN11CxSmallSort10SetDynamicEib(cm->mSpace,cm->mSpaceID,'\x01');
  return (uint)bVar1;
}


/* ==== McdSpaceModelIsFrozen ==== */

MeBool McdSpaceModelIsFrozen(McdModelID_conflict m)

{
  byte bVar1;
  
  bVar1 = _ZN11CxSmallSort10GetDynamicEi(m->mSpace,m->mSpaceID);
  return bVar1 ^ 1;
}


/* ==== McdSpaceUpdateAll ==== */

void McdSpaceUpdateAll(McdSpaceID s)

{
  _ZN11CxSmallSort9UpdateAllEf(s,0.0);
  return;
}


/* ==== McdSpacePathUpdateAll ==== */

void McdSpacePathUpdateAll(McdSpaceID s,MeReal duration)

{
  MeProfileStartSectionFn("McdSpacePathUpdate",'\0');
  *(code **)(s + 200) = McdModelUpdatePath;
  _ZN11CxSmallSort9UpdateAllEf(s,duration);
  *(code **)(s + 200) = McdModelUpdatePathCompatible;
  MeProfileEndSectionFn("McdSpacePathUpdate");
  return;
}


/* ==== McdSpaceSetAABBFn ==== */

void McdSpaceSetAABBFn(McdSpaceID s,McdSpaceUpdateAABBFnPtr updateAABBFn)

{
  *(McdSpaceUpdateAABBFnPtr *)(s + 200) = updateAABBFn;
  return;
}


/* ==== McdSpaceSetUserData ==== */

void McdSpaceSetUserData(McdSpaceID s,void *data)

{
  _ZN11CxSmallSort7SetDataEPv(s,data);
  return;
}


/* ==== McdSpaceGetUserData ==== */

void * McdSpaceGetUserData(McdSpaceID s)

{
  void *pvVar1;
  
  pvVar1 = _ZN11CxSmallSort7GetDataEv(s);
  return pvVar1;
}


/* ==== McdSpacePairIteratorBegin ==== */

void McdSpacePairIteratorBegin(McdSpaceID s,McdSpacePairIterator *iter)

{
  iter->count = -1;
  iter->ptr = (void *)0x0;
  return;
}


/* ==== McdSpaceGetPairs ==== */

int McdSpaceGetPairs(McdSpaceID s,McdSpacePairIterator *iter,McdModelPairContainer *a)

{
  int iVar1;
  
  iVar1 = _ZN11CxSmallSort8getPairsEP20McdSpacePairIteratorP22_McdModelPairContainer(s,iter,a);
  return iVar1;
}


/* ==== McdSpaceGetTransitions ==== */

int McdSpaceGetTransitions(McdSpaceID s,McdSpacePairIterator *iter,McdModelPairContainer *a)

{
  int iVar1;
  
  iVar1 = _ZN11CxSmallSort14getTransitionsEP20McdSpacePairIteratorP22_McdModelPairContainer
                    (s,iter,a);
  return iVar1;
}


/* ==== McdSpaceGetLineSegIntersections ==== */

int McdSpaceGetLineSegIntersections
              (McdSpaceID space,MeReal *inOrig,MeReal *inDest,McdLineSegIntersectResult *outList,
              int inMaxListSize)

{
  int iVar1;
  int iVar2;
  McdModelID_conflict pMVar3;
  char cVar4;
  void *pvVar5;
  code *pcVar6;
  int iVar7;
  int iVar8;
  int local_24;
  McdLineSegIntersectResult *local_20;
  int iValidModelCount;
  int iNumOfModelsInSpace;
  int iHit;
  
                    /* Unresolved local var: int id@[DW_OP_reg6(ESI)]
                       Unresolved local var: CxSmallSort * sp@[???]
                       Unresolved local var: int iSpaceSize@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdModelID.conflict cm@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdLineSegIntersectFnPtr theFn@[DW_OP_reg0(EAX)] */
  iVar8 = 0;
  iHit = 0;
  iVar1 = *(int *)(space + 0x18);
  iVar2 = *(int *)(space + 0x1c);
  iValidModelCount = 0;
  if (((0 < inMaxListSize) && (0 < iVar1)) && (0 < iVar1 - iVar2)) {
    local_20 = outList;
    local_24 = 0;
    do {
      cVar4 = _ZN11CxSmallSort7ValidIDEi(space,iVar8);
      if (cVar4 != '\0') {
        pMVar3 = *(McdModelID_conflict *)(*(int *)(space + 0xc) + 0xc + local_24);
        if (((pMVar3 != (McdModelID_conflict)0x0) && (pMVar3->mSpaceID != -1)) &&
           (pMVar3->mSpace == space)) {
          iValidModelCount = iValidModelCount + 1;
          pvVar5 = McdModelGetGeometry(pMVar3);
          pcVar6 = McdGeometryGetLineSegIntersectFnPtr(pvVar5);
          if (pcVar6 != (code *)0x0) {
            iVar7 = (*pcVar6)(pMVar3,inOrig,inDest,local_20);
            if (iVar7 != 0) {
              local_20->model = pMVar3;
              local_20 = local_20 + 1;
              iHit = iHit + 1;
            }
          }
        }
      }
      local_24 = local_24 + 0x98;
      iVar8 = iVar8 + 1;
    } while (((iHit < inMaxListSize) && (iVar8 < iVar1)) && (iValidModelCount < iVar1 - iVar2));
  }
  return iHit;
}


/* ==== McdSpaceGetLineSegFirstIntersection ==== */

int McdSpaceGetLineSegFirstIntersection
              (McdSpaceID space,MeReal *inOrig,MeReal *inDest,McdLineSegIntersectResult *outResult)

{
  int iVar1;
  int iVar2;
  McdModelID_conflict pMVar3;
  char cVar4;
  void *pvVar5;
  code *pcVar6;
  int iVar7;
  int iVar8;
  int local_48;
  int iValidModelCount;
  int iNumOfModelsInSpace;
  McdLineSegIntersectResult tmp;
  
                    /* Unresolved local var: CxSmallSort * sp@[???]
                       Unresolved local var: int iSpaceSize@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdModelID.conflict cm@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdLineSegIntersectFnPtr theFn@[DW_OP_reg2(EDX)] */
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)] */
  iVar1 = *(int *)(space + 0x18);
  iVar2 = *(int *)(space + 0x1c);
  iValidModelCount = 0;
  outResult->distance = 1e+25;
  if ((0 < iVar1) && (0 < iVar1 - iVar2)) {
    local_48 = 0;
    iVar8 = 0;
    do {
      cVar4 = _ZN11CxSmallSort7ValidIDEi(space,iVar8);
      if ((((cVar4 != '\0') &&
           (pMVar3 = *(McdModelID_conflict *)(*(int *)(space + 0xc) + 0xc + local_48),
           pMVar3 != (McdModelID_conflict)0x0)) && (pMVar3->mSpaceID != -1)) &&
         (pMVar3->mSpace == space)) {
        iValidModelCount = iValidModelCount + 1;
        pvVar5 = McdModelGetGeometry(pMVar3);
        pcVar6 = McdGeometryGetLineSegIntersectFnPtr(pvVar5);
        if (((pcVar6 != (code *)0x0) && (iVar7 = (*pcVar6)(pMVar3,inOrig,inDest,&tmp), iVar7 != 0))
           && (tmp.distance < outResult->distance)) {
          outResult->normal[0] = tmp.normal[0];
          outResult->model = pMVar3;
          outResult->normal[1] = tmp.normal[1];
          outResult->position[0] = tmp.position[0];
          outResult->normal[2] = tmp.normal[2];
          outResult->position[1] = tmp.position[1];
          outResult->distance = tmp.distance;
          outResult->position[2] = tmp.position[2];
        }
      }
      iVar8 = iVar8 + 1;
      local_48 = local_48 + 0x98;
    } while ((iVar8 < iVar1) && (iValidModelCount < iVar1 - iVar2));
  }
  return (uint)(outResult->distance < 1e+25);
}


/* ==== McdSpaceGetLineSegFirstEnabledIntersection ==== */

int McdSpaceGetLineSegFirstEnabledIntersection
              (McdSpaceID space,MeReal *inOrig,MeReal *inDest,
              McdLineSegIntersectEnableCallback filterCB,void *filterData,
              McdLineSegIntersectResult *outResult)

{
  int iVar1;
  int iVar2;
  McdModel_conflict *pMVar3;
  char cVar4;
  int iVar5;
  void *pvVar6;
  code *pcVar7;
  int iVar8;
  int local_48;
  int iValidModelCount;
  int iNumOfModelsInSpace;
  McdLineSegIntersectResult tmp;
  
                    /* Unresolved local var: CxSmallSort * sp@[???]
                       Unresolved local var: int iSpaceSize@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdModelID.conflict cm@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdLineSegIntersectFnPtr theFn@[DW_OP_reg2(EDX)] */
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)] */
  iVar1 = *(int *)(space + 0x18);
  iVar2 = *(int *)(space + 0x1c);
  iValidModelCount = 0;
  outResult->distance = 1e+25;
  if ((0 < iVar1) && (0 < iVar1 - iVar2)) {
    local_48 = 0;
    iVar8 = 0;
    do {
      cVar4 = _ZN11CxSmallSort7ValidIDEi(space,iVar8);
      if ((((cVar4 != '\0') &&
           (pMVar3 = *(McdModel_conflict **)(*(int *)(space + 0xc) + 0xc + local_48),
           pMVar3 != (McdModel_conflict *)0x0)) && (pMVar3->mSpaceID != -1)) &&
         (pMVar3->mSpace == space)) {
        iValidModelCount = iValidModelCount + 1;
        iVar5 = (*filterCB)(pMVar3,filterData);
        if (iVar5 != 0) {
          pvVar6 = McdModelGetGeometry(pMVar3);
          pcVar7 = McdGeometryGetLineSegIntersectFnPtr(pvVar6);
          if (((pcVar7 != (code *)0x0) && (iVar5 = (*pcVar7)(pMVar3,inOrig,inDest,&tmp), iVar5 != 0)
              ) && (tmp.distance < outResult->distance)) {
            outResult->normal[0] = tmp.normal[0];
            outResult->model = pMVar3;
            outResult->normal[1] = tmp.normal[1];
            outResult->position[0] = tmp.position[0];
            outResult->normal[2] = tmp.normal[2];
            outResult->position[1] = tmp.position[1];
            outResult->distance = tmp.distance;
            outResult->position[2] = tmp.position[2];
          }
        }
      }
      iVar8 = iVar8 + 1;
      local_48 = local_48 + 0x98;
    } while ((iVar8 < iVar1) && (iValidModelCount < iVar1 - iVar2));
  }
  return (uint)(outResult->distance < 1e+25);
}


/* ==== McdSpaceRemoveModel ==== */

MeBool McdSpaceRemoveModel(McdModelID_conflict cm)

{
  char extraout_AL;
  MeBool MVar1;
  
  MVar1 = 0;
  if (cm->mSpace != (McdSpaceID)0x0) {
    _ZN11CxSmallSort6RemoveEi(cm->mSpace);
    MVar1 = 0;
    if (extraout_AL != '\0') {
      cm->mSpace = (McdSpaceID)0x0;
      cm->mSpaceID = -1;
      MVar1 = 1;
    }
  }
  return MVar1;
}


/* ==== McdSpaceEnablePair ==== */

MeBool McdSpaceEnablePair(McdModelID_conflict cm1,McdModelID_conflict cm2)

{
  byte bVar1;
  
  bVar1 = _ZN11CxSmallSort21SetPairOverlapEnabledEiib
                    (cm1->mSpace,cm1->mSpaceID,cm2->mSpaceID,'\x01');
  return (uint)bVar1;
}


/* ==== McdSpaceDisablePair ==== */

MeBool McdSpaceDisablePair(McdModelID_conflict m1,McdModelID_conflict m2)

{
  byte bVar1;
  
  bVar1 = _ZN11CxSmallSort21SetPairOverlapEnabledEiib(m1->mSpace,m1->mSpaceID,m2->mSpaceID,'\0');
  return (uint)bVar1;
}


/* ==== McdSpacePairIsEnabled ==== */

MeBool McdSpacePairIsEnabled(McdModelID_conflict m1,McdModelID_conflict m2)

{
  byte bVar1;
  
  bVar1 = _ZN11CxSmallSort21GetPairOverlapEnabledEii(m1->mSpace,m1->mSpaceID,m2->mSpaceID);
  return (uint)bVar1;
}


/* ==== McdSpaceBeginChanges ==== */

void McdSpaceBeginChanges(McdSpaceID space)

{
  _ZN11CxSmallSort11UpdateBeginEv(space);
  return;
}


/* ==== McdSpaceEndChanges ==== */

void McdSpaceEndChanges(McdSpaceID space)

{
  _ZN11CxSmallSort9UpdateEndEv(space);
  return;
}


/* ==== McdSpaceModelIteratorBegin ==== */

void McdSpaceModelIteratorBegin(McdSpaceID param_1,McdSpaceModelIterator *it)

{
  it->it = 0;
  it->count = 0;
  return;
}


/* ==== McdSpaceGetModel ==== */

MeBool McdSpaceGetModel(McdSpaceID space,McdSpaceModelIterator *it,McdModelID_conflict *model)

{
  int iVar1;
  char cVar2;
  
                    /* Unresolved local var: CxSmallSort * s@[???]
                       Unresolved local var: int size@[DW_OP_reg6(ESI)]
                       Unresolved local var: int freeCount@[???] */
  *model = (McdModelID_conflict)0x0;
  iVar1 = *(int *)(space + 0x18);
  if (it->count < iVar1 - *(int *)(space + 0x1c)) {
    while (cVar2 = _ZN11CxSmallSort7ValidIDEi(space,it->it), cVar2 == '\0') {
      if (iVar1 <= it->it) {
        return 0;
      }
      it->it = it->it + 1;
    }
    if (it->it < iVar1) {
      *model = *(McdModelID_conflict *)(*(int *)(space + 0xc) + 0xc + it->it * 0x98);
      it->count = it->count + 1;
      it->it = it->it + 1;
      return 1;
    }
  }
  return 0;
}


/* ==== McdSpaceGetModelCount ==== */

int McdSpaceGetModelCount(McdSpaceID space)

{
  return *(int *)(space + 0x18) - *(int *)(space + 0x1c);
}


/* ==== McdSpaceSetPoolFullHandler ==== */

void McdSpaceSetPoolFullHandler(McdSpaceID space,McdSpacePoolErrorFnPtr handler)

{
  if (handler == (McdSpacePoolErrorFnPtr)0x0) {
    handler = defaultPoolFullHandler;
  }
  McdModelPairManagerSetPoolFullHandler(*(void **)(space + 0x2c),handler);
  return;
}


/* ==== McdSpaceGetPoolFullHandler ==== */

McdSpacePoolErrorFnPtr McdSpaceGetPoolFullHandler(McdSpaceID space)

{
  McdSpacePoolErrorFnPtr p_Var1;
  
  p_Var1 = McdModelPairManagerGetPoolFullHandler(*(void **)(space + 0x2c));
  return p_Var1;
}


/* ==== defaultPoolFullHandler ==== */

void defaultPoolFullHandler(McdModelID_conflict model1,McdModelID_conflict model2)

{
  return;
}


