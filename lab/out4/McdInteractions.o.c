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
  puVar4 = McdFrameworkGetInteractions
                     (pMVar1->frame,(uint)(byte)((pMVar1->mInstance).mGeometry)->mRefCtAndID,
                      (uint)(byte)((p->model2->mInstance).mGeometry)->mRefCtAndID);
  if (puVar4[4] != 0) {
                    /* Unresolved local var: McdModelID.conflict tmp@[DW_OP_reg2(EDX)] */
    pMVar1 = p->model1;
    p->model1 = p->model2;
    p->model2 = pMVar1;
  }
  pMVar5 = McdFrameworkGetDefaultRequestPtr(pMVar2);
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
  void *pvVar3;
  
                    /* Unresolved local var: McdInteractions * interactions@[DW_OP_reg0(EAX)] */
  sVar1 = McdModelGetGeometryType(p->model2);
  iVar2 = (int)sVar1;
  sVar1 = McdModelGetGeometryType(p->model1);
  pvVar3 = McdFrameworkGetInteractions(p->model1->frame,(int)sVar1,iVar2);
  if (*(code **)((int)pvVar3 + 4) != (code *)0x0) {
    (**(code **)((int)pvVar3 + 4))(p);
  }
  return;
}


/* ==== McdIntersect ==== */

MeBool McdIntersect(McdModelPair *p,McdIntersectResult *result)

{
  code *pcVar1;
  McdRequest *pMVar2;
  short sVar3;
  void *pvVar4;
  void *pvVar5;
  McdContact *pMVar6;
  int iVar7;
  McdContact aMStackY_501c [511];
  undefined4 uStackY_40;
  int type1;
  McdFramework *frame;
  McdContact *outContacts;
  
                    /* Unresolved local var: McdInteractions * interactions@[DW_OP_reg3(EBX)]
                       Unresolved local var: int type2@[DW_OP_reg6(ESI)] */
  frame = p->model1->frame;
  sVar3 = McdModelGetGeometryType(p->model1);
  type1 = (int)sVar3;
  sVar3 = McdModelGetGeometryType(p->model2);
  result->pair = p;
  iVar7 = (int)sVar3;
  pvVar4 = McdFrameworkGetInteractions(frame,type1,iVar7);
  pvVar5 = pvVar4;
  if ((*(int *)((int)pvVar4 + 8) == 0) &&
     (pvVar5 = (void *)McdFrameworkGetInteractionsWarned(frame,type1,iVar7), pvVar5 == (void *)0x0))
  {
    McdFrameworkGetTypeName(frame,iVar7);
    McdFrameworkGetTypeName(frame,type1);
    MeWarning(1,"No test registered between %s and %s");
    uStackY_40 = 0x102e8;
    McdFrameworkSetInteractionsWarned(frame,type1,iVar7,1);
    result->touch = 0;
    result->contactCount = 0;
    return 0;
  }
  pMVar6 = (McdContact *)&type1;
  if (*(int *)((int)pvVar4 + 0x14) != 0) {
    outContacts = result->contacts;
    pMVar6 = aMStackY_501c;
    result->contacts = aMStackY_501c;
  }
  *(void **)((int)pMVar6 + -4) = pvVar5;
  *(void **)((int)pMVar6 + -8) = pvVar5;
  *(McdIntersectResult **)((int)pMVar6 + -0xc) = result;
  *(McdModelPair **)((int)pMVar6 + -0x10) = p;
  pcVar1 = *(code **)((int)pvVar4 + 8);
  *(undefined4 *)((int)pMVar6 + -0x14) = 0x10232;
  iVar7 = (*pcVar1)();
  result->touch = iVar7;
  if (*(int *)((int)pvVar4 + 0x14) != 0) {
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
    iVar7 = McdContactSimplify(*(void **)((int)pMVar6 + -0x20),*(void **)((int)pMVar6 + -0x1c),
                               *(int *)((int)pMVar6 + -0x18),*(void **)((int)pMVar6 + -0x14),
                               *(int *)((int)pMVar6 + -0x10),*(int *)((int)pMVar6 + -0xc),
                               *(float *)((int)pMVar6 + -8));
    result->contactCount = iVar7;
    result->contacts = outContacts;
  }
  return 1;
}


/* ==== McdIntersectAt ==== */

MeBool McdIntersectAt(McdModelPair *p,McdIntersectResult *result,MeReal time)

{
  McdModelID_conflict pMVar1;
  McdModelID_conflict pMVar2;
  void *pvVar3;
  void *pvVar4;
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
  pvVar3 = McdModelGetTransformPtr(pMVar1);
  pvVar4 = McdModelGetTransformPtr(pMVar2);
  MeMatrix4TMUpdateFromVelocities
            (tm1,0.0001,time,pMVar1->linearVelocity,pMVar1->angularVelocity,pvVar3);
  MeMatrix4TMUpdateFromVelocities
            (tm2,0.0001,time,pMVar2->linearVelocity,pMVar2->angularVelocity,pvVar4);
  McdModelSetTransformPtr(pMVar1,tm1);
  McdModelSetTransformPtr(pMVar2,tm2);
  MVar5 = McdIntersect(p,result);
  McdModelSetTransformPtr(pMVar1,pvVar3);
  McdModelSetTransformPtr(pMVar2,pvVar4);
  return MVar5;
}


/* ==== McdSafeTime ==== */

MeBool McdSafeTime(McdModelPair *p,MeReal maxTime,McdSafeTimeResult *result)

{
  MeReal *pMVar1;
  void *pvVar2;
  
                    /* Unresolved local var: int ret@[???]
                       Unresolved local var: McdInteractions * interactions@[DW_OP_reg3(EBX)] */
  pvVar2 = McdFrameworkGetInteractions
                     (p->model1->frame,(uint)(byte)((p->model1->mInstance).mGeometry)->mRefCtAndID,
                      (uint)(byte)((p->model2->mInstance).mGeometry)->mRefCtAndID);
  if ((((*(code **)((int)pvVar2 + 0xc) == (code *)0x0) ||
       (p->model1->linearVelocity == (MeReal *)0x0)) || (p->model2->linearVelocity == (MeReal *)0x0)
      ) || ((p->model1->angularVelocity == (MeReal *)0x0 ||
            (pMVar1 = p->model2->angularVelocity, pMVar1 == (MeReal *)0x0)))) {
    result->time = maxTime;
  }
  else {
    (**(code **)((int)pvVar2 + 0xc))(p,maxTime,result,pMVar1);
  }
  return (uint)(*(int *)((int)pvVar2 + 0xc) != 0);
}


/* ==== McdLineSegIntersect ==== */

uint McdLineSegIntersect(McdModelID_conflict cm,MeReal *inOrig,MeReal *inDest,
                        McdLineSegIntersectResult *outOverlap)

{
  void *pvVar1;
  code *pcVar2;
  int iVar3;
  uint uVar4;
  
                    /* Unresolved local var: McdLineSegIntersectFnPtr theFn@[DW_OP_reg0(EAX)] */
  uVar4 = 0;
  outOverlap->model = cm;
  pvVar1 = McdModelGetGeometry(cm);
  pcVar2 = McdGeometryGetLineSegIntersectFnPtr(pvVar1);
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
  void *pvVar4;
  int iVar5;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)] */
  iVar5 = 0;
  if (0 < pairs->goodbyeEnd) {
    do {
                    /* Unresolved local var: McdInteractions * interactions@[DW_OP_reg0(EAX)] */
      pMVar1 = pairs->array[iVar5];
      sVar2 = McdModelGetGeometryType(pMVar1->model2);
      iVar3 = (int)sVar2;
      sVar2 = McdModelGetGeometryType(pMVar1->model1);
      pvVar4 = McdFrameworkGetInteractions(pMVar1->model1->frame,(int)sVar2,iVar3);
      if (*(code **)((int)pvVar4 + 4) != (code *)0x0) {
        (**(code **)((int)pvVar4 + 4))(pMVar1);
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < pairs->goodbyeEnd);
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


