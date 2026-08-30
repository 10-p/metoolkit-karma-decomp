/* ==== McdNearby ==== */

MeBool McdNearby(McdModelPair *p)

{
  uint uVar1;
  uint uVar2;
  MeReal maxCorner2 [3];
  MeReal minCorner2 [3];
  MeReal maxCorner1 [3];
  MeReal minCorner1 [3];
  
                    /* Unresolved local var: int axis@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeBool disjointNotFound@[DW_OP_reg6(ESI)] */
  McdModelGetAABB(p->model1,minCorner1,maxCorner1);
  McdModelGetAABB(p->model2,minCorner2,maxCorner2);
  if (minCorner2[0] <= maxCorner1[0]) {
    uVar1 = (maxCorner2[0] < minCorner1[0]) - 1 & 1;
  }
  else {
    uVar1 = 0;
  }
  uVar2 = 0;
  if (uVar1 != 0) {
    if (minCorner2[1] <= maxCorner1[1]) {
      uVar1 = uVar1 & (maxCorner2[1] < minCorner1[1]) - 1;
    }
    else {
      uVar1 = 0;
    }
    uVar2 = 0;
    if (uVar1 != 0) {
      if (minCorner2[2] <= maxCorner1[2]) {
        uVar2 = uVar1 & (maxCorner2[2] < minCorner1[2]) - 1;
      }
      else {
        uVar2 = 0;
      }
    }
  }
  return uVar2;
}


/* ==== McdHello ==== */

void McdHello(McdModelPair *p)

{
  McdModelID_conflict pMVar1;
  McdFramework *pMVar2;
  McdHelloCallbackFnPtr p_Var3;
  undefined4 *puVar4;
  McdRequest *pMVar5;
  
                    /* Unresolved local var: McdFramework * frame@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdInteractions * interactions@[DW_OP_reg6(ESI)] */
  pMVar1 = p->model1;
  pMVar2 = pMVar1->frame;
  puVar4 = (undefined4 *)
           McdFrameworkGetInteractions
                     (pMVar1->frame,(char)((pMVar1->mInstance).mGeometry)->mRefCtAndID,
                      (char)((p->model2->mInstance).mGeometry)->mRefCtAndID);
  if (puVar4[4] != 0) {
                    /* Unresolved local var: McdModelID.conflict tmp@[DW_OP_reg2(EDX)] */
    pMVar1 = p->model1;
    p->model1 = p->model2;
    p->model2 = pMVar1;
  }
  pMVar5 = (McdRequest *)McdFrameworkGetDefaultRequestPtr(pMVar2);
  p->request = pMVar5;
  p_Var3 = pMVar2->mHelloCallbackFnPtr;
  if (p_Var3 != (McdHelloCallbackFnPtr)0x0) {
    (*p_Var3)(p);
  }
  if ((code *)*puVar4 != (code *)0x0) {
    (*(code *)*puVar4)(p);
  }
  return;
}


/* ==== McdGoodbye ==== */

void McdGoodbye(McdModelPair *p)

{
  short sVar1;
  int iVar2;
  
                    /* Unresolved local var: McdInteractions * interactions@[DW_OP_reg0(EAX)] */
  sVar1 = McdModelGetGeometryType(p->model2);
  sVar1 = McdModelGetGeometryType(p->model1,(int)sVar1,(int)sVar1);
  iVar2 = McdFrameworkGetInteractions(p->model1->frame,(int)sVar1);
  if (*(code **)(iVar2 + 4) != (code *)0x0) {
    (**(code **)(iVar2 + 4))(p);
  }
  return;
}


/* ==== McdIntersect ==== */

MeBool McdIntersect(McdModelPair *p,McdIntersectResult *result)

{
  code *pcVar1;
  McdRequest *pMVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  McdContact *pMVar6;
  int iVar7;
  McdContact aMStack_501c [511];
  undefined4 uStack_40;
  McdFramework *pMStack_3c;
  int iStack_38;
  int iStack_34;
  undefined4 uStack_30;
  McdFramework *pMStack_2c;
  char *pcStack_28;
  int iStack_24;
  int iStack_20;
  int type1;
  McdFramework *frame;
  McdContact *outContacts;
  
                    /* Unresolved local var: McdInteractions * interactions@[DW_OP_reg3(EBX)]
                       Unresolved local var: int type2@[DW_OP_reg6(ESI)] */
  pMStack_2c = (McdFramework *)p->model1;
  frame = ((McdModelID_conflict)pMStack_2c)->frame;
  uStack_30 = 0x101dc;
  sVar3 = McdModelGetGeometryType();
  type1 = (int)sVar3;
  pMStack_2c = (McdFramework *)p->model2;
  uStack_30 = 0x101ed;
  sVar3 = McdModelGetGeometryType();
  result->pair = p;
  iVar7 = (int)sVar3;
  pcStack_28 = (char *)type1;
  pMStack_2c = frame;
  uStack_30 = 0x10206;
  iStack_24 = iVar7;
  iVar4 = McdFrameworkGetInteractions();
  iVar5 = iVar4;
  if (*(int *)(iVar4 + 8) == 0) {
    pcStack_28 = (char *)type1;
    pMStack_2c = frame;
    uStack_30 = 0x1029f;
    iStack_24 = iVar7;
    iStack_20 = iVar4;
    iVar5 = McdFrameworkGetInteractionsWarned();
    if (iVar5 == 0) {
      pMStack_2c = frame;
      uStack_30 = 0x102b6;
      pcStack_28 = (char *)iVar7;
      iStack_24 = iVar5;
      iStack_20 = iVar5;
      iStack_20 = McdFrameworkGetTypeName();
      pcStack_28 = (char *)type1;
      pMStack_2c = frame;
      uStack_30 = 0x102c8;
      iStack_24 = iVar4;
      iStack_24 = McdFrameworkGetTypeName();
      pcStack_28 = "No test registered between %s and %s";
      pMStack_2c = (McdFramework *)0x1;
      uStack_30 = 0x102d8;
      MeWarning();
      uStack_30 = 1;
      iStack_38 = type1;
      pMStack_3c = frame;
      uStack_40 = 0x102e8;
      iStack_34 = iVar7;
      McdFrameworkSetInteractionsWarned();
      result->touch = 0;
      result->contactCount = 0;
      return 0;
    }
  }
  pMVar6 = (McdContact *)&type1;
  if (*(int *)(iVar4 + 0x14) != 0) {
    outContacts = result->contacts;
    pMVar6 = aMStack_501c;
    result->contacts = aMStack_501c;
  }
  *(int *)((int)pMVar6 + -4) = iVar5;
  *(int *)((int)pMVar6 + -8) = iVar5;
  *(McdIntersectResult **)((int)pMVar6 + -0xc) = result;
  *(McdModelPair **)((int)pMVar6 + -0x10) = p;
  pcVar1 = *(code **)(iVar4 + 8);
  *(undefined4 *)((int)pMVar6 + -0x14) = 0x10232;
  iVar5 = (*pcVar1)();
  result->touch = iVar5;
  if (*(int *)(iVar4 + 0x14) != 0) {
    *(McdModelPair **)((int)pMVar6 + -4) = p;
    *(MeReal *)((int)pMVar6 + -8) = frame->mScale;
    pMVar2 = p->request;
    *(int *)((int)pMVar6 + -0xc) = pMVar2->faceNormalsFirst;
    *(int *)((int)pMVar6 + -0x10) = pMVar2->contactMaxCount;
    *(McdContact **)((int)pMVar6 + -0x14) = outContacts;
    *(int *)((int)pMVar6 + -0x18) = result->contactCount;
    *(McdContact **)((int)pMVar6 + -0x1c) = result->contacts;
    *(MeReal **)((int)pMVar6 + -0x20) = result->normal;
    *(undefined4 *)((int)pMVar6 + -0x24) = 0x1027d;
    iVar5 = McdContactSimplify();
    result->contactCount = iVar5;
    result->contacts = outContacts;
  }
  return 1;
}


/* ==== McdIntersectAt ==== */

MeBool McdIntersectAt(McdModelPair *p,McdIntersectResult *result,MeReal time)

{
  McdModelID_conflict pMVar1;
  McdModelID_conflict pMVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  MeBool MVar5;
  MeMatrix4Ptr tm2save;
  MeMatrix4Ptr tm1save;
  MeReal tm2 [4] [4];
  MeReal tm1 [4] [4];
  
                    /* Unresolved local var: McdModelID.conflict m1@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdModelID.conflict m2@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeBool ret@[DW_OP_reg3(EBX)] */
  pMVar1 = p->model1;
  pMVar2 = p->model2;
  uVar3 = McdModelGetTransformPtr(pMVar1);
  uVar4 = McdModelGetTransformPtr(pMVar2);
  MeMatrix4TMUpdateFromVelocities
            (tm1,0x38d1b717,time,pMVar1->linearVelocity,pMVar1->angularVelocity,uVar3);
  MeMatrix4TMUpdateFromVelocities
            (tm2,0x38d1b717,time,pMVar2->linearVelocity,pMVar2->angularVelocity,uVar4);
  McdModelSetTransformPtr(pMVar1,tm1);
  McdModelSetTransformPtr(pMVar2,tm2);
  MVar5 = McdIntersect(p,result);
  McdModelSetTransformPtr(pMVar1,uVar3);
  McdModelSetTransformPtr(pMVar2,uVar4);
  return MVar5;
}


/* ==== McdSafeTime ==== */

MeBool McdSafeTime(McdModelPair *p,MeReal maxTime,McdSafeTimeResult *result)

{
  MeReal *pMVar1;
  int iVar2;
  
                    /* Unresolved local var: int ret@[???]
                       Unresolved local var: McdInteractions * interactions@[DW_OP_reg3(EBX)] */
  iVar2 = McdFrameworkGetInteractions
                    (p->model1->frame,(char)((p->model1->mInstance).mGeometry)->mRefCtAndID,
                     (char)((p->model2->mInstance).mGeometry)->mRefCtAndID);
  if ((((*(code **)(iVar2 + 0xc) == (code *)0x0) || (p->model1->linearVelocity == (MeReal *)0x0)) ||
      (p->model2->linearVelocity == (MeReal *)0x0)) ||
     ((p->model1->angularVelocity == (MeReal *)0x0 ||
      (pMVar1 = p->model2->angularVelocity, pMVar1 == (MeReal *)0x0)))) {
    result->time = maxTime;
  }
  else {
    (**(code **)(iVar2 + 0xc))(p,maxTime,result,pMVar1);
  }
  return (uint)(*(int *)(iVar2 + 0xc) != 0);
}


/* ==== McdLineSegIntersect ==== */

uint McdLineSegIntersect(McdModelID_conflict cm,MeReal *inOrig,MeReal *inDest,
                        McdLineSegIntersectResult *outOverlap)

{
  undefined4 uVar1;
  code *pcVar2;
  int iVar3;
  uint uVar4;
  
                    /* Unresolved local var: McdLineSegIntersectFnPtr theFn@[DW_OP_reg0(EAX)] */
  uVar4 = 0;
  outOverlap->model = cm;
  uVar1 = McdModelGetGeometry(cm);
  pcVar2 = (code *)McdGeometryGetLineSegIntersectFnPtr(uVar1);
  if (pcVar2 != (code *)0x0) {
    iVar3 = (*pcVar2)(cm,inOrig,inDest,outOverlap);
    if (iVar3 != 0) {
      uVar4 = 1;
    }
  }
  return uVar4;
}


/* ==== McdSetHelloCallback ==== */

void McdSetHelloCallback(McdFramework *frame,McdHelloCallbackFnPtr f)

{
  frame->mHelloCallbackFnPtr = f;
  return;
}


/* ==== McdGetHelloCallback ==== */

McdHelloCallbackFnPtr McdGetHelloCallback(McdFramework *frame)

{
  return frame->mHelloCallbackFnPtr;
}


/* ==== McdHelloEach ==== */

void McdHelloEach(McdModelPairContainer *pairs)

{
  McdModelPair **ppMVar1;
  int iVar2;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar2 = pairs->helloFirst;
  if (iVar2 < pairs->helloEndStayingFirst) {
    do {
      ppMVar1 = pairs->array + iVar2;
      iVar2 = iVar2 + 1;
      McdHello(*ppMVar1);
    } while (iVar2 < pairs->helloEndStayingFirst);
  }
  return;
}


/* ==== McdGoodbyeEach ==== */

void McdGoodbyeEach(McdModelPairContainer *pairs)

{
  McdModelPair *pMVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)] */
  iVar4 = 0;
  if (0 < pairs->goodbyeEnd) {
    do {
                    /* Unresolved local var: McdInteractions * interactions@[DW_OP_reg0(EAX)] */
      pMVar1 = pairs->array[iVar4];
      sVar2 = McdModelGetGeometryType(pMVar1->model2);
      sVar2 = McdModelGetGeometryType(pMVar1->model1,(int)sVar2,(int)sVar2);
      iVar3 = McdFrameworkGetInteractions(pMVar1->model1->frame,(int)sVar2);
      if (*(code **)(iVar3 + 4) != (code *)0x0) {
        (**(code **)(iVar3 + 4))(pMVar1);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < pairs->goodbyeEnd);
  }
  return;
}


/* ==== McdIntersectEach ==== */

MeBool McdIntersectEach(McdModelPairContainer *pairs,McdModelPairContainerIterator *pairsIter,
                       McdIntersectResult *resultArray,int resultArraySize,int *resultCount,
                       McdContact *contactArray,int contactArraySize,int *contactCount)

{
  McdModelPair *p;
  int iVar1;
  int iVar2;
  int freeContactCount;
  MeBool overflow;
  int resultIndex;
  
                    /* Unresolved local var: McdModelPair * pair@[DW_OP_reg1(ECX)]
                       Unresolved local var: McdIntersectResult * result@[???]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int contactIndex@[DW_OP_reg7(EDI)] */
  iVar2 = 0;
  resultIndex = 0;
  overflow = 0;
  iVar1 = pairsIter->count;
  if (iVar1 < pairs->stayingEnd) {
    do {
      p = pairs->array[iVar1];
      if ((contactArraySize - iVar2 < p->request->contactMaxCount) ||
         (resultArraySize <= resultIndex)) {
        overflow = 1;
        break;
      }
      iVar1 = iVar1 + 1;
      resultArray->contacts = contactArray + iVar2;
      resultArray->contactMaxCount = contactArraySize - iVar2;
      McdIntersect(p,resultArray);
      resultIndex = resultIndex + 1;
      iVar2 = iVar2 + resultArray->contactCount;
      resultArray = resultArray + 1;
    } while (iVar1 < pairs->stayingEnd);
  }
  pairsIter->count = iVar1;
  *resultCount = resultIndex;
  *contactCount = iVar2;
  return overflow;
}


