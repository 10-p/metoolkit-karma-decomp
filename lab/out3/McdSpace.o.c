/* ==== McdSpaceAxisSortCreate ==== */

/* WARNING: Unknown calling convention */

McdSpaceID McdSpaceAxisSortCreate(McdFramework *fwk,int axes,int objectCount,int pairCount)

{
  McdSpaceID pMVar1;
  
                    /* Unresolved local var: CxSmallSort * s@[DW_OP_reg3(EBX)] */
  pMVar1 = (McdSpaceID)CxSmallSort::New((_McdFramework *)fwk,(uchar)axes,objectCount,pairCount);
  *(code **)(pMVar1 + 200) = McdModelUpdatePathCompatible;
  McdModelPairManagerSetPoolFullHandler(*(void **)(pMVar1 + 0x2c),defaultPoolFullHandler);
  CxSmallSort::UpdateBegin();
  return pMVar1;
}


/* ==== McdSpaceDestroy ==== */

/* WARNING: Unknown calling convention */

void McdSpaceDestroy(McdSpaceID s)

{
  CxSmallSort::Delete();
  return;
}


/* ==== McdSpaceBuild ==== */

/* WARNING: Unknown calling convention */

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
  MeI32 MVar3;
  
                    /* Unresolved local var: uint result@[DW_OP_reg7(EDI)] */
  pcVar2 = McdModelGetGeometry(cm);
  if (*pcVar2 != '\0') {
    if (cm->mSpace != (McdSpaceID)0x0) {
      return (uint)(cm->mSpace == s);
    }
    MVar3 = CxSmallSort::Insert((_McdModel *)s,(McdCullingTable *)cm,0,0);
    cVar1 = CxSmallSort::ValidID((int)s);
    if (cVar1 != '\0') {
      cm->mSpace = s;
      cm->mSpaceID = MVar3;
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
  MeI32 MVar2;
  uint uVar3;
  
                    /* Unresolved local var: uint result@[DW_OP_reg7(EDI)] */
  if (cm->mSpace == (McdSpaceID)0x0) {
    MVar2 = CxSmallSort::Insert((_McdModel *)s,(McdCullingTable *)cm,(uint)table,cullingIndex);
    cVar1 = CxSmallSort::ValidID((int)s);
    if (cVar1 == '\0') {
      uVar3 = 0;
    }
    else {
      cm->mSpace = s;
      cm->mSpaceID = MVar2;
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
  CxSmallSort::SetCullingParameters((int)s,(McdCullingTable *)cm->mSpaceID,(uint)table,cullingIndex)
  ;
  return;
}


/* ==== McdSpaceUpdateModel ==== */

void McdSpaceUpdateModel(McdModelID_conflict cm)

{
  CxSmallSort::UpdateSingle((int)cm->mSpace,(float)cm->mSpaceID);
  return;
}


/* ==== McdSpaceFreezeModel ==== */

MeBool McdSpaceFreezeModel(McdModelID_conflict m)

{
  uint uVar1;
  
  uVar1 = CxSmallSort::SetDynamic((int)m->mSpace,SUB41(m->mSpaceID,0));
  return uVar1 & 0xff;
}


/* ==== McdSpaceUnfreezeModel ==== */

MeBool McdSpaceUnfreezeModel(McdModelID_conflict cm)

{
  uint uVar1;
  
  uVar1 = CxSmallSort::SetDynamic((int)cm->mSpace,SUB41(cm->mSpaceID,0));
  return uVar1 & 0xff;
}


/* ==== McdSpaceModelIsFrozen ==== */

MeBool McdSpaceModelIsFrozen(McdModelID_conflict m)

{
  uint uVar1;
  
  uVar1 = CxSmallSort::GetDynamic((int)m->mSpace);
  return (uVar1 ^ 1) & 0xff;
}


/* ==== McdSpaceUpdateAll ==== */

/* WARNING: Unknown calling convention */

void McdSpaceUpdateAll(McdSpaceID s)

{
  CxSmallSort::UpdateAll((float)s);
  return;
}


/* ==== McdSpacePathUpdateAll ==== */

void McdSpacePathUpdateAll(McdSpaceID s,MeReal duration)

{
  MeProfileStartSectionFn("McdSpacePathUpdate",'\0');
  *(code **)(s + 200) = McdModelUpdatePath;
  CxSmallSort::UpdateAll((float)s);
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
  CxSmallSort::SetData(s);
  return;
}


/* ==== McdSpaceGetUserData ==== */

/* WARNING: Unknown calling convention */

void * McdSpaceGetUserData(McdSpaceID s)

{
  void *pvVar1;
  
  pvVar1 = (void *)CxSmallSort::GetData();
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
  
  iVar1 = CxSmallSort::getPairs((McdSpacePairIterator *)s,(_McdModelPairContainer *)iter);
  return iVar1;
}


/* ==== McdSpaceGetTransitions ==== */

int McdSpaceGetTransitions(McdSpaceID s,McdSpacePairIterator *iter,McdModelPairContainer *a)

{
  int iVar1;
  
  iVar1 = CxSmallSort::getTransitions((McdSpacePairIterator *)s,(_McdModelPairContainer *)iter);
  return iVar1;
}


/* ==== McdSpaceGetLineSegIntersections ==== */

/* WARNING: Unknown calling convention */

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
      cVar4 = CxSmallSort::ValidID((int)space);
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

/* WARNING: Unknown calling convention */

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
      cVar4 = CxSmallSort::ValidID((int)space);
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

/* WARNING: Unknown calling convention */

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
      cVar4 = CxSmallSort::ValidID((int)space);
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
  char cVar1;
  MeBool MVar2;
  
  MVar2 = 0;
  if (cm->mSpace != (McdSpaceID)0x0) {
    cVar1 = CxSmallSort::Remove((int)cm->mSpace);
    MVar2 = 0;
    if (cVar1 != '\0') {
      cm->mSpace = (McdSpaceID)0x0;
      cm->mSpaceID = -1;
      MVar2 = 1;
    }
  }
  return MVar2;
}


/* ==== McdSpaceEnablePair ==== */

MeBool __regparm1 McdSpaceEnablePair(McdModelID_conflict cm1,McdModelID_conflict cm2)

{
  uint uVar1;
  int in_stack_00000008;
  
  uVar1 = CxSmallSort::SetPairOverlapEnabled
                    ((int)cm2->mSpace,cm2->mSpaceID,
                     SUB41(*(undefined4 *)(in_stack_00000008 + 0x1c),0));
  return uVar1 & 0xff;
}


/* ==== McdSpaceDisablePair ==== */

MeBool __regparm1 McdSpaceDisablePair(McdModelID_conflict m1,McdModelID_conflict m2)

{
  uint uVar1;
  int in_stack_00000008;
  
  uVar1 = CxSmallSort::SetPairOverlapEnabled
                    ((int)m2->mSpace,m2->mSpaceID,SUB41(*(undefined4 *)(in_stack_00000008 + 0x1c),0)
                    );
  return uVar1 & 0xff;
}


/* ==== McdSpacePairIsEnabled ==== */

MeBool McdSpacePairIsEnabled(McdModelID_conflict m1,McdModelID_conflict m2)

{
  uint uVar1;
  
  uVar1 = CxSmallSort::GetPairOverlapEnabled((int)m1->mSpace,m1->mSpaceID);
  return uVar1 & 0xff;
}


/* ==== McdSpaceBeginChanges ==== */

/* WARNING: Unknown calling convention */

void McdSpaceBeginChanges(McdSpaceID space)

{
  CxSmallSort::UpdateBegin();
  return;
}


/* ==== McdSpaceEndChanges ==== */

/* WARNING: Unknown calling convention */

void McdSpaceEndChanges(McdSpaceID space)

{
  CxSmallSort::UpdateEnd();
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
    while (cVar2 = CxSmallSort::ValidID((int)space), cVar2 == '\0') {
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

/* WARNING: Unknown calling convention */

void defaultPoolFullHandler(McdModelID_conflict model1,McdModelID_conflict model2)

{
  return;
}


