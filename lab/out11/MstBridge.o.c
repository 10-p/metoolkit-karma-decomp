/* ==== MstBridgeCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MstBridgeID MstBridgeCreate(McdFrameworkID frame,uint maxMaterials)

{
  MstBridgeID pMVar1;
  MstMaterialPair *pMVar2;
  uint uVar3;
  McdModelPairContainer *pMVar4;
  McdBatchContext *pMVar5;
  McdContact *pMVar6;
  uint uVar7;
  MstBridgeID b;
  uint i;
  
                    /* Unresolved local var: uint nElements@[???]
                       Unresolved local var: MstMaterialPair * p@[DW_OP_reg6(ESI)] */
  pMVar1 = (*_MeMemoryAPI)(0x1c);
  if (pMVar1 == (MstBridgeID)0x0) {
    return (MstBridgeID)0x0;
  }
  uVar7 = (int)((maxMaterials + 1) * maxMaterials) >> 1;
  pMVar1->maxMaterials = maxMaterials;
  pMVar2 = (*_MeMemoryAPI)(uVar7 * 0x54);
  pMVar1->materialPairArray = pMVar2;
  i = 0;
  if (uVar7 == 0) goto LAB_00010143;
  uVar3 = uVar7 & 3;
  if (uVar7 < 2) {
LAB_00010070:
    MdtContactParamsReset(pMVar2);
    pMVar2->contactCB = (MstPerContactCBPtr)0x0;
    pMVar2->intersectCB = (MstIntersectCBPtr)0x0;
    pMVar2->pairCB = (MstPerPairCBPtr)0x0;
    i = i + 1;
    pMVar2 = pMVar2 + 1;
    if (uVar7 <= i) goto LAB_00010143;
  }
  else if (uVar3 != 0) {
    if (1 < uVar3) {
      if (2 < uVar3) {
        MdtContactParamsReset(pMVar2);
        pMVar2->contactCB = (MstPerContactCBPtr)0x0;
        pMVar2->intersectCB = (MstIntersectCBPtr)0x0;
        pMVar2->pairCB = (MstPerPairCBPtr)0x0;
        pMVar2 = pMVar2 + 1;
      }
      i = (uint)(2 < uVar3);
      MdtContactParamsReset(pMVar2);
      pMVar2->contactCB = (MstPerContactCBPtr)0x0;
      pMVar2->intersectCB = (MstIntersectCBPtr)0x0;
      pMVar2->pairCB = (MstPerPairCBPtr)0x0;
      pMVar2 = pMVar2 + 1;
      i = i + 1;
    }
    goto LAB_00010070;
  }
  do {
    MdtContactParamsReset(pMVar2);
    pMVar2->contactCB = (MstPerContactCBPtr)0x0;
    pMVar2->intersectCB = (MstIntersectCBPtr)0x0;
    pMVar2->pairCB = (MstPerPairCBPtr)0x0;
    MdtContactParamsReset(pMVar2 + 1);
    pMVar2[1].contactCB = (MstPerContactCBPtr)0x0;
    pMVar2[1].intersectCB = (MstIntersectCBPtr)0x0;
    pMVar2[1].pairCB = (MstPerPairCBPtr)0x0;
    MdtContactParamsReset(pMVar2 + 2);
    pMVar2[2].contactCB = (MstPerContactCBPtr)0x0;
    pMVar2[2].intersectCB = (MstIntersectCBPtr)0x0;
    pMVar2[2].pairCB = (MstPerPairCBPtr)0x0;
    MdtContactParamsReset(pMVar2 + 3);
    pMVar2[3].contactCB = (MstPerContactCBPtr)0x0;
    pMVar2[3].intersectCB = (MstIntersectCBPtr)0x0;
    pMVar2[3].pairCB = (MstPerPairCBPtr)0x0;
    i = i + 4;
    pMVar2 = pMVar2 + 4;
  } while (i < uVar7);
LAB_00010143:
  pMVar1->usedMaterials = 1;
  pMVar4 = McdModelPairContainerCreate(0x32);
  pMVar1->pairs = pMVar4;
  pMVar5 = McdBatchContextCreate(frame);
  pMVar1->context = pMVar5;
  pMVar1->contactsMaxCount = 0x32;
  pMVar6 = (*_MeMemoryAPI)(2000);
  pMVar1->contacts = pMVar6;
  return pMVar1;
}


/* ==== MstBridgeDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MstBridgeDestroy(MstBridgeID b)

{
  McdModelPairContainerDestroy(b->pairs);
  McdBatchContextDestroy(b->context);
  (*_McdBatchContextCreate)(b->contacts);
  (*_McdBatchContextCreate)(b->materialPairArray);
  (*_McdBatchContextCreate)(b);
  return;
}


/* ==== MstBridgeUpdateTransitions ==== */

void MstBridgeUpdateTransitions(MstBridgeID b,McdSpaceID s,MdtWorldID w)

{
  int iVar1;
  McdSpacePairIterator spaceIter;
  
                    /* Unresolved local var: MeBool pairOverflow@[DW_OP_reg3(EBX)] */
  McdSpaceEndChanges(s);
  McdSpacePairIteratorBegin(s,&spaceIter);
  do {
    McdModelPairContainerReset(b->pairs);
    iVar1 = McdSpaceGetPairs(s,&spaceIter,b->pairs);
    MstHandleTransitions(b->pairs,s,w,b);
  } while (iVar1 != 0);
  McdSpaceBeginChanges(s);
  return;
}


/* ==== MstBridgeUpdateContacts ==== */

void MstBridgeUpdateContacts(MstBridgeID b,McdSpaceID s,MdtWorldID w)

{
  int iVar1;
  McdSpacePairIterator spaceIter;
  
                    /* Unresolved local var: MeBool pairOverflow@[DW_OP_reg3(EBX)] */
  McdSpaceEndChanges(s);
  McdSpacePairIteratorBegin(s,&spaceIter);
  do {
    McdModelPairContainerReset(b->pairs);
    iVar1 = McdSpaceGetPairs(s,&spaceIter,b->pairs);
    MstHandleTransitions(b->pairs,s,w,b);
    MstHandleCollisions(b->pairs,s,w,b);
  } while (iVar1 != 0);
  McdSpaceBeginChanges(s);
  return;
}


/* ==== MstBridgeGetContactParams ==== */

MdtContactParamsID MstBridgeGetContactParams(MstBridgeID b,MstMaterialID m1,MstMaterialID m2)

{
  uint uVar1;
  MstMaterialPair *pMVar2;
  uint uVar3;
  uint uVar4;
  
                    /* Unresolved local var: int index@[???] */
  uVar1 = b->maxMaterials;
  if ((uVar1 < m1) || (uVar1 < m2)) {
    pMVar2 = (MstMaterialPair *)0x0;
  }
  else {
                    /* Unresolved local var: MeI32 o@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeI32 rowIndex@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeI32 colIndex@[???] */
    uVar3 = (int)(m2 - m1) >> 0x1f;
    uVar4 = (uVar3 ^ 0xffffffff) & m1 | m2 & uVar3;
    pMVar2 = b->materialPairArray +
             (((uVar3 & m1 | (uVar3 ^ 0xffffffff) & m2) + uVar1 * uVar4) -
             ((int)((uVar4 + 1) * uVar4) >> 1));
  }
  return &pMVar2->cp;
}


/* ==== MstBridgeGetPerPairCB ==== */

MstPerPairCBPtr MstBridgeGetPerPairCB(MstBridgeID b,MstMaterialID m1,MstMaterialID m2)

{
  uint uVar1;
  MstPerPairCBPtr p_Var2;
  uint uVar3;
  uint uVar4;
  
                    /* Unresolved local var: int index@[???] */
  uVar1 = b->maxMaterials;
  if ((uVar1 < m1) || (uVar1 < m2)) {
    p_Var2 = (MstPerPairCBPtr)0x0;
  }
  else {
                    /* Unresolved local var: MeI32 o@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeI32 rowIndex@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeI32 colIndex@[???] */
    uVar3 = (int)(m2 - m1) >> 0x1f;
    uVar4 = (uVar3 ^ 0xffffffff) & m1 | m2 & uVar3;
    p_Var2 = b->materialPairArray
             [((uVar3 & m1 | (uVar3 ^ 0xffffffff) & m2) + uVar1 * uVar4) -
              ((int)((uVar4 + 1) * uVar4) >> 1)].pairCB;
  }
  return p_Var2;
}


/* ==== MstBridgeGetPerContactCB ==== */

MstPerContactCBPtr MstBridgeGetPerContactCB(MstBridgeID b,MstMaterialID m1,MstMaterialID m2)

{
  uint uVar1;
  MstPerContactCBPtr p_Var2;
  uint uVar3;
  uint uVar4;
  
                    /* Unresolved local var: int index@[???] */
  uVar1 = b->maxMaterials;
  if ((uVar1 < m1) || (uVar1 < m2)) {
    p_Var2 = (MstPerContactCBPtr)0x0;
  }
  else {
                    /* Unresolved local var: MeI32 o@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeI32 rowIndex@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeI32 colIndex@[???] */
    uVar3 = (int)(m2 - m1) >> 0x1f;
    uVar4 = (uVar3 ^ 0xffffffff) & m1 | m2 & uVar3;
    p_Var2 = b->materialPairArray
             [((uVar3 & m1 | (uVar3 ^ 0xffffffff) & m2) + uVar1 * uVar4) -
              ((int)((uVar4 + 1) * uVar4) >> 1)].contactCB;
  }
  return p_Var2;
}


/* ==== MstBridgeGetIntersectCB ==== */

MstIntersectCBPtr MstBridgeGetIntersectCB(MstBridgeID b,MstMaterialID m1,MstMaterialID m2)

{
  uint uVar1;
  MstIntersectCBPtr p_Var2;
  uint uVar3;
  uint uVar4;
  
                    /* Unresolved local var: int index@[???] */
  uVar1 = b->maxMaterials;
  if ((uVar1 < m1) || (uVar1 < m2)) {
    p_Var2 = (MstIntersectCBPtr)0x0;
  }
  else {
                    /* Unresolved local var: MeI32 o@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeI32 rowIndex@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeI32 colIndex@[???] */
    uVar3 = (int)(m2 - m1) >> 0x1f;
    uVar4 = (uVar3 ^ 0xffffffff) & m1 | m2 & uVar3;
    p_Var2 = b->materialPairArray
             [((uVar3 & m1 | (uVar3 ^ 0xffffffff) & m2) + uVar1 * uVar4) -
              ((int)((uVar4 + 1) * uVar4) >> 1)].intersectCB;
  }
  return p_Var2;
}


/* ==== MstBridgeGetNewMaterial ==== */

MstMaterialID MstBridgeGetNewMaterial(MstBridgeID b)

{
  MstMaterialID MVar1;
  
                    /* Unresolved local var: int newm@[DW_OP_reg0(EAX)] */
  MVar1 = b->usedMaterials;
  b->usedMaterials = MVar1 + 1;
  return MVar1;
}


/* ==== MstBridgeSetPerPairCB ==== */

void MstBridgeSetPerPairCB(MstBridgeID b,MstMaterialID m1,MstMaterialID m2,MstPerPairCBPtr cb)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
                    /* Unresolved local var: int index@[???] */
  uVar1 = b->maxMaterials;
  if ((m1 <= uVar1) && (m2 <= uVar1)) {
                    /* Unresolved local var: MeI32 o@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeI32 rowIndex@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeI32 colIndex@[???] */
    uVar2 = (int)(m2 - m1) >> 0x1f;
    uVar3 = (uVar2 ^ 0xffffffff) & m1 | m2 & uVar2;
    b->materialPairArray
    [((uVar2 & m1 | (uVar2 ^ 0xffffffff) & m2) + uVar1 * uVar3) - ((int)((uVar3 + 1) * uVar3) >> 1)]
    .pairCB = cb;
  }
  return;
}


/* ==== MstBridgeSetPerContactCB ==== */

void MstBridgeSetPerContactCB(MstBridgeID b,MstMaterialID m1,MstMaterialID m2,MstPerContactCBPtr cb)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
                    /* Unresolved local var: int index@[???] */
  uVar1 = b->maxMaterials;
  if ((m1 <= uVar1) && (m2 <= uVar1)) {
                    /* Unresolved local var: MeI32 o@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeI32 rowIndex@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeI32 colIndex@[???] */
    uVar2 = (int)(m2 - m1) >> 0x1f;
    uVar3 = (uVar2 ^ 0xffffffff) & m1 | m2 & uVar2;
    b->materialPairArray
    [((uVar2 & m1 | (uVar2 ^ 0xffffffff) & m2) + uVar1 * uVar3) - ((int)((uVar3 + 1) * uVar3) >> 1)]
    .contactCB = cb;
  }
  return;
}


/* ==== MstBridgeSetIntersectCB ==== */

void MstBridgeSetIntersectCB(MstBridgeID b,MstMaterialID m1,MstMaterialID m2,MstIntersectCBPtr cb)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
                    /* Unresolved local var: int index@[???] */
  uVar1 = b->maxMaterials;
  if ((m1 <= uVar1) && (m2 <= uVar1)) {
                    /* Unresolved local var: MeI32 o@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeI32 rowIndex@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeI32 colIndex@[???] */
    uVar2 = (int)(m2 - m1) >> 0x1f;
    uVar3 = (uVar2 ^ 0xffffffff) & m1 | m2 & uVar2;
    b->materialPairArray
    [((uVar2 & m1 | (uVar2 ^ 0xffffffff) & m2) + uVar1 * uVar3) - ((int)((uVar3 + 1) * uVar3) >> 1)]
    .intersectCB = cb;
  }
  return;
}


/* ==== MstBridgeSetModelPairBufferSize ==== */

void MstBridgeSetModelPairBufferSize(MstBridgeID b,uint s)

{
  McdModelPairContainer *pMVar1;
  
  McdModelPairContainerDestroy(b->pairs);
  pMVar1 = McdModelPairContainerCreate(s);
  b->pairs = pMVar1;
  return;
}


/* ==== MstBridgeSetContactBufferSize ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MstBridgeSetContactBufferSize(MstBridgeID b,uint s)

{
  McdContact *pMVar1;
  
  (*_McdBatchContextCreate)(b->contacts);
  b->contactsMaxCount = s;
  pMVar1 = (*_MeMemoryAPI)(s * 0x28);
  b->contacts = pMVar1;
  return;
}


