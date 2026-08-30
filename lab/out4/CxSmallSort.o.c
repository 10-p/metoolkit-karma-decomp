/* ==== lsda_exception_table_00010000 ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* (LSDA) Exception Table */

void CxSmallSort::lsda_exception_table_00010000(int inNObjects,int inNPairs)

{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  undefined1 *puVar4;
  
  uVar2 = inNPairs * (inNPairs + -1) >> 1;
  *(undefined ***)inNObjects = &PTR__CxSmallSort_00011f20;
  iVar3 = (uVar2 >> 1) + (uVar2 & 1);
  *(int *)(inNObjects + 0x28) = iVar3;
  puVar1 = (undefined1 *)(*_MeMemoryAPI)(iVar3);
  puVar4 = puVar1 + *(int *)(inNObjects + 0x28);
  *(undefined1 **)(inNObjects + 0x24) = puVar1;
  if (puVar4 <= puVar1) goto code_r0x00010076;
  uVar2 = (int)puVar4 - (int)puVar1 & 3;
  if (puVar1 + 1 < puVar4) {
    if (uVar2 != 0) {
      if (1 < uVar2) {
        if (2 < uVar2) {
          *puVar1 = 0;
          puVar1 = puVar1 + 1;
        }
        *puVar1 = 0;
        puVar1 = puVar1 + 1;
      }
      goto code_r0x00010055;
    }
  }
  else {
code_r0x00010055:
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
    if (puVar4 <= puVar1) goto code_r0x00010076;
  }
  do {
    *puVar1 = 0;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1 = puVar1 + 4;
  } while (puVar1 < puVar4);
code_r0x00010076:
  *(int *)(inNObjects + 0x30) = inNObjects + 0x30;
  *(int *)(inNObjects + 0x34) = inNObjects + 0x30;
  *(undefined4 *)(inNObjects + 0x38) = 0;
  *(undefined4 *)(inNObjects + 0x3c) = 0;
  *(undefined1 *)(inNObjects + 0x40) = 0;
  *(int *)(inNObjects + 0x44) = inNObjects + 0x44;
  *(int *)(inNObjects + 0x48) = inNObjects + 0x44;
  *(undefined4 *)(inNObjects + 0x4c) = 0;
  *(undefined4 *)(inNObjects + 0x50) = 0;
  *(undefined1 *)(inNObjects + 0x54) = 0;
  *(int *)(inNObjects + 0x58) = inNObjects + 0x58;
  *(int *)(inNObjects + 0x5c) = inNObjects + 0x58;
  *(undefined4 *)(inNObjects + 0x60) = 0;
  *(undefined4 *)(inNObjects + 100) = 0;
  *(undefined1 *)(inNObjects + 0x68) = 0;
  *(int *)(inNObjects + 0x70) = inNObjects + 0x6c;
  *(int *)(inNObjects + 0x6c) = inNObjects + 0x6c;
  *(undefined4 *)(inNObjects + 0x74) = 0;
  *(undefined4 *)(inNObjects + 0x78) = 0;
  *(undefined1 *)(inNObjects + 0x7c) = 0;
  *(int *)(inNObjects + 0x80) = inNObjects + 0x80;
  *(int *)(inNObjects + 0x84) = inNObjects + 0x80;
  *(undefined4 *)(inNObjects + 0x88) = 0;
  *(undefined4 *)(inNObjects + 0x8c) = 0;
  *(undefined1 *)(inNObjects + 0x90) = 0;
  *(int *)(inNObjects + 0x94) = inNObjects + 0x94;
  *(int *)(inNObjects + 0x98) = inNObjects + 0x94;
  *(undefined4 *)(inNObjects + 0x9c) = 0;
  *(undefined4 *)(inNObjects + 0xa0) = 0;
  *(undefined1 *)(inNObjects + 0xa4) = 0;
  *(int *)(inNObjects + 0xac) = inNObjects + 0xa8;
  *(int *)(inNObjects + 0xa8) = inNObjects + 0xa8;
  *(int *)(inNObjects + 0xb4) = inNObjects + 0xb0;
  *(int *)(inNObjects + 0xb0) = inNObjects + 0xb0;
  *(int *)(inNObjects + 0xbc) = inNObjects + 0xb8;
  *(int *)(inNObjects + 0xb8) = inNObjects + 0xb8;
  *(int *)(inNObjects + 0xc4) = inNObjects + 0xc0;
  *(int *)(inNObjects + 0xc0) = inNObjects + 0xc0;
  return;
}


/* ==== CxSmallSort ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: void CxSmallSort(CxSmallSort * this, int inNObjects, int inNPairs) */

void CxSmallSort::CxSmallSort(int inNObjects,int inNPairs)

{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  undefined1 *puVar4;
  
                    /* Unresolved local var: MeU8 * elem@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeU8 * stop@[DW_OP_reg2(EDX)] */
  uVar2 = inNPairs * (inNPairs + -1) >> 1;
  *(undefined ***)inNObjects = &PTR__CxSmallSort_00011f20;
  iVar3 = (uVar2 >> 1) + (uVar2 & 1);
  *(int *)(inNObjects + 0x28) = iVar3;
  puVar1 = (undefined1 *)(*_MeMemoryAPI)(iVar3);
  puVar4 = puVar1 + *(int *)(inNObjects + 0x28);
  *(undefined1 **)(inNObjects + 0x24) = puVar1;
  if (puVar4 <= puVar1) goto LAB_00010206;
  uVar2 = (int)puVar4 - (int)puVar1 & 3;
  if (puVar1 + 1 < puVar4) {
    if (uVar2 != 0) {
      if (1 < uVar2) {
        if (2 < uVar2) {
          *puVar1 = 0;
          puVar1 = puVar1 + 1;
        }
        *puVar1 = 0;
        puVar1 = puVar1 + 1;
      }
      goto LAB_000101e5;
    }
  }
  else {
LAB_000101e5:
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
    if (puVar4 <= puVar1) goto LAB_00010206;
  }
  do {
    *puVar1 = 0;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1 = puVar1 + 4;
  } while (puVar1 < puVar4);
LAB_00010206:
  *(int *)(inNObjects + 0x30) = inNObjects + 0x30;
  *(int *)(inNObjects + 0x34) = inNObjects + 0x30;
  *(undefined4 *)(inNObjects + 0x38) = 0;
  *(undefined4 *)(inNObjects + 0x3c) = 0;
  *(undefined1 *)(inNObjects + 0x40) = 0;
  *(int *)(inNObjects + 0x44) = inNObjects + 0x44;
  *(int *)(inNObjects + 0x48) = inNObjects + 0x44;
  *(undefined4 *)(inNObjects + 0x4c) = 0;
  *(undefined4 *)(inNObjects + 0x50) = 0;
  *(undefined1 *)(inNObjects + 0x54) = 0;
  *(int *)(inNObjects + 0x58) = inNObjects + 0x58;
  *(int *)(inNObjects + 0x5c) = inNObjects + 0x58;
  *(undefined4 *)(inNObjects + 0x60) = 0;
  *(undefined4 *)(inNObjects + 100) = 0;
  *(undefined1 *)(inNObjects + 0x68) = 0;
  *(int *)(inNObjects + 0x70) = inNObjects + 0x6c;
  *(int *)(inNObjects + 0x6c) = inNObjects + 0x6c;
  *(undefined4 *)(inNObjects + 0x74) = 0;
  *(undefined4 *)(inNObjects + 0x78) = 0;
  *(undefined1 *)(inNObjects + 0x7c) = 0;
  *(int *)(inNObjects + 0x80) = inNObjects + 0x80;
  *(int *)(inNObjects + 0x84) = inNObjects + 0x80;
  *(undefined4 *)(inNObjects + 0x88) = 0;
  *(undefined4 *)(inNObjects + 0x8c) = 0;
  *(undefined1 *)(inNObjects + 0x90) = 0;
  *(int *)(inNObjects + 0x94) = inNObjects + 0x94;
  *(int *)(inNObjects + 0x98) = inNObjects + 0x94;
  *(undefined4 *)(inNObjects + 0x9c) = 0;
  *(undefined4 *)(inNObjects + 0xa0) = 0;
  *(undefined1 *)(inNObjects + 0xa4) = 0;
  *(int *)(inNObjects + 0xac) = inNObjects + 0xa8;
  *(int *)(inNObjects + 0xa8) = inNObjects + 0xa8;
  *(int *)(inNObjects + 0xb4) = inNObjects + 0xb0;
  *(int *)(inNObjects + 0xb0) = inNObjects + 0xb0;
  *(int *)(inNObjects + 0xbc) = inNObjects + 0xb8;
  *(int *)(inNObjects + 0xb8) = inNObjects + 0xb8;
  *(int *)(inNObjects + 0xc4) = inNObjects + 0xc0;
  *(int *)(inNObjects + 0xc0) = inNObjects + 0xc0;
  return;
}


/* ==== ~CxSmallSort ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: void ~CxSmallSort(CxSmallSort * this, int __in_chrg) */

void CxSmallSort::~CxSmallSort(int __in_chrg)

{
  *(undefined ***)__in_chrg = &PTR__CxSmallSort_00011f20;
  if (*(int *)(__in_chrg + 0x24) != 0) {
    (*__Unwind_Resume)(*(int *)(__in_chrg + 0x24));
  }
  return;
}


/* ==== ~CxSmallSort ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: void ~CxSmallSort(CxSmallSort * this, int __in_chrg) */

void CxSmallSort::~CxSmallSort(int __in_chrg)

{
  *(undefined ***)__in_chrg = &PTR__CxSmallSort_00011f20;
  if (*(int *)(__in_chrg + 0x24) != 0) {
    (*__Unwind_Resume)(*(int *)(__in_chrg + 0x24));
  }
  return;
}


/* ==== ~CxSmallSort ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: void ~CxSmallSort(CxSmallSort * this, int __in_chrg) */

void CxSmallSort::~CxSmallSort(int __in_chrg)

{
  *(undefined ***)__in_chrg = &PTR__CxSmallSort_00011f20;
  if (*(int *)(__in_chrg + 0x24) != 0) {
    (*__Unwind_Resume)(*(int *)(__in_chrg + 0x24));
  }
  operator_delete((void *)__in_chrg);
  return;
}


/* ==== Delete ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: void Delete(CxSmallSort * this) */

void CxSmallSort::Delete(void)

{
  undefined4 *in_stack_00000004;
  
  (*__Unwind_Resume)(in_stack_00000004[4]);
  (*__Unwind_Resume)(in_stack_00000004[3]);
  (*__Unwind_Resume)(in_stack_00000004[5]);
  McdModelPairManagerDestroy((void *)in_stack_00000004[0xb]);
  (**(code **)*in_stack_00000004)();
  (*__Unwind_Resume)();
  return;
}


/* ==== New ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

CxSmallSort * CxSmallSort::New(McdFramework *fwk,MeU8 inAxes,int inNObjects,int inNPairs)

{
  Link *pLVar1;
  Link *pLVar2;
  CxSmallSort *inNObjects_00;
  MeI32 *pMVar3;
  CxSmallSortRep *pCVar4;
  uint uVar5;
  McdModelPairManagerID pMVar6;
  MeU8 MVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  undefined4 uVar11;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: CxSmallSort * ns@[DW_OP_reg3(EBX)] */
  if (((6 < (byte)(inAxes - 1)) || (inNObjects < 1)) || (inNPairs < 1)) {
    return (CxSmallSort *)0x0;
  }
  inNObjects_00 = (CxSmallSort *)(*_MeMemoryAPI)(0xd4);
  if (inNObjects_00 == (CxSmallSort *)0x0) {
    return (CxSmallSort *)0x0;
  }
  uVar11 = 0;
  iVar9 = inNObjects;
  iVar10 = inNPairs;
  CxSmallSort((int)inNObjects_00,inNObjects);
  inNObjects_00->mModelMax = inNObjects;
  inNObjects_00->mAABBUpdateFn = (McdUpdateAABBFnPtr)0x0;
  inNObjects_00->mUserData = (void *)0x0;
  inNObjects_00->mFramework = fwk;
  pMVar3 = (MeI32 *)(*_MeMemoryAPI)(inNObjects << 2,iVar9,iVar10,uVar11);
  inNObjects_00->mFreeIDs = pMVar3;
  pCVar4 = (CxSmallSortRep *)(*_MeMemoryAPI)(inNObjects * 0x98);
  inNObjects_00->mRepList = pCVar4;
  for (uVar8 = (uint)(inNObjects * 0x98) >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
    (pCVar4->super_Link).mNext = (Link *)0x0;
    pCVar4 = (CxSmallSortRep *)&(pCVar4->super_Link).mPrev;
  }
  uVar8 = 0;
  if (inNObjects < 1) goto LAB_0001059a;
  iVar9 = 0;
  uVar5 = inNObjects & 3;
  if (inNObjects < 2) {
LAB_000104e2:
    inNObjects_00->mFreeIDs[uVar8] = (uVar8 ^ 0xffffffff) + inNObjects_00->mModelMax;
    uVar8 = uVar8 + 1;
    *(undefined4 *)((int)inNObjects_00->mRepList->mAABBMarkers + iVar9 + -0x18) = 0xffffffff;
    iVar9 = iVar9 + 0x98;
    if (inNObjects <= (int)uVar8) goto LAB_0001059a;
  }
  else if (uVar5 != 0) {
    if (1 < uVar5) {
      if (2 < uVar5) {
        *inNObjects_00->mFreeIDs = inNObjects_00->mModelMax + -1;
        iVar9 = 0x98;
        inNObjects_00->mRepList->mID = -1;
      }
      uVar8 = (uint)(2 < uVar5);
      inNObjects_00->mFreeIDs[uVar8] = (uVar8 ^ 0xffffffff) + inNObjects_00->mModelMax;
      uVar8 = uVar8 + 1;
      *(undefined4 *)((int)inNObjects_00->mRepList->mAABBMarkers + iVar9 + -0x18) = 0xffffffff;
      iVar9 = iVar9 + 0x98;
    }
    goto LAB_000104e2;
  }
  do {
    inNObjects_00->mFreeIDs[uVar8] = (uVar8 ^ 0xffffffff) + inNObjects_00->mModelMax;
    *(undefined4 *)((int)inNObjects_00->mRepList->mAABBMarkers + iVar9 + -0x18) = 0xffffffff;
    inNObjects_00->mFreeIDs[uVar8 + 1] = (uVar8 + 1 ^ 0xffffffff) + inNObjects_00->mModelMax;
    *(undefined4 *)((int)inNObjects_00->mRepList[1].mAABBMarkers + iVar9 + -0x18) = 0xffffffff;
    inNObjects_00->mFreeIDs[uVar8 + 2] = (uVar8 + 2 ^ 0xffffffff) + inNObjects_00->mModelMax;
    uVar5 = uVar8 + 3;
    uVar8 = uVar8 + 4;
    *(undefined4 *)((int)inNObjects_00->mRepList[2].mAABBMarkers + iVar9 + -0x18) = 0xffffffff;
    inNObjects_00->mFreeIDs[uVar5] = (uVar5 ^ 0xffffffff) + inNObjects_00->mModelMax;
    *(undefined4 *)((int)inNObjects_00->mRepList[3].mAABBMarkers + iVar9 + -0x18) = 0xffffffff;
    iVar9 = iVar9 + 0x260;
  } while ((int)uVar8 < inNObjects);
LAB_0001059a:
  inNObjects_00->mFreeIDCount = inNObjects_00->mModelMax;
  pMVar3 = (MeI32 *)(*_MeMemoryAPI)(inNObjects << 2);
                    /* Unresolved local var: MeU8 axes@[DW_OP_reg2(EDX)] */
  inNObjects_00->mReleasedIDs = pMVar3;
  inNObjects_00->mAxes = inAxes;
  inNObjects_00->mReleasedIDCount = 0;
  inNObjects_00->mNAxes = '\0';
  if (inAxes != '\0') {
    MVar7 = '\0';
    do {
      MVar7 = MVar7 + '\x01';
      inAxes = inAxes & inAxes - 1;
    } while (inAxes != 0);
    inNObjects_00->mNAxes = MVar7;
  }
  pLVar1 = inNObjects_00->mSort[0].mRoot.mNext;
  inNObjects_00->mMinBound[0].super_Link.mPrev = &inNObjects_00->mSort[0].mRoot;
  inNObjects_00->mMinBound[0].mOrdinate = -3.4028235e+38;
  inNObjects_00->mMaxBound[0].mOrdinate = 3.4028235e+38;
  inNObjects_00->mMinBound[0].mType = 0xff;
  inNObjects_00->mMaxBound[0].mType = 0xff;
  pLVar1->mPrev = &inNObjects_00->mMinBound[0].super_Link;
  pLVar2 = inNObjects_00->mSort[0].mRoot.mPrev;
  inNObjects_00->mMinBound[0].super_Link.mNext = pLVar1;
  inNObjects_00->mMaxBound[0].super_Link.mNext = &inNObjects_00->mSort[0].mRoot;
  inNObjects_00->mSort[0].mRoot.mNext = &inNObjects_00->mMinBound[0].super_Link;
  pLVar2->mNext = &inNObjects_00->mMaxBound[0].super_Link;
  inNObjects_00->mMaxBound[0].super_Link.mPrev = pLVar2;
  (inNObjects_00->mMaxBound[0].super_Link.mNext)->mPrev = &inNObjects_00->mMaxBound[0].super_Link;
  inNObjects_00->mMinBound[1].mOrdinate = -3.4028235e+38;
  inNObjects_00->mMaxBound[1].mOrdinate = 3.4028235e+38;
  inNObjects_00->mMinBound[1].mType = 0xff;
  inNObjects_00->mMaxBound[1].mType = 0xff;
  pLVar1 = inNObjects_00->mSort[1].mRoot.mNext;
  inNObjects_00->mMinBound[1].super_Link.mPrev = &inNObjects_00->mSort[1].mRoot;
  inNObjects_00->mMinBound[1].super_Link.mNext = pLVar1;
  inNObjects_00->mSort[1].mRoot.mNext = &inNObjects_00->mMinBound[1].super_Link;
  pLVar1 = inNObjects_00->mMinBound[1].super_Link.mNext;
  inNObjects_00->mMaxBound[1].super_Link.mNext = &inNObjects_00->mSort[1].mRoot;
  pLVar1->mPrev = &inNObjects_00->mMinBound[1].super_Link;
  pLVar1 = inNObjects_00->mSort[1].mRoot.mPrev;
  inNObjects_00->mMaxBound[1].super_Link.mPrev = pLVar1;
  pLVar1->mNext = &inNObjects_00->mMaxBound[1].super_Link;
  (inNObjects_00->mMaxBound[1].super_Link.mNext)->mPrev = &inNObjects_00->mMaxBound[1].super_Link;
  inNObjects_00->mMinBound[2].mOrdinate = -3.4028235e+38;
  inNObjects_00->mMaxBound[2].mOrdinate = 3.4028235e+38;
  inNObjects_00->mMinBound[2].mType = 0xff;
  inNObjects_00->mMaxBound[2].mType = 0xff;
  inNObjects_00->mMinBound[2].super_Link.mNext = inNObjects_00->mSort[2].mRoot.mNext;
  inNObjects_00->mSort[2].mRoot.mNext = &inNObjects_00->mMinBound[2].super_Link;
  pLVar1 = inNObjects_00->mMinBound[2].super_Link.mNext;
  inNObjects_00->mMinBound[2].super_Link.mPrev = &inNObjects_00->mSort[2].mRoot;
  inNObjects_00->mMaxBound[2].super_Link.mNext = &inNObjects_00->mSort[2].mRoot;
  pLVar1->mPrev = &inNObjects_00->mMinBound[2].super_Link;
  pLVar1 = inNObjects_00->mSort[2].mRoot.mPrev;
  inNObjects_00->mMaxBound[2].super_Link.mPrev = pLVar1;
  pLVar1->mNext = &inNObjects_00->mMaxBound[2].super_Link;
  (inNObjects_00->mMaxBound[2].super_Link.mNext)->mPrev = &inNObjects_00->mMaxBound[2].super_Link;
  pMVar6 = McdModelPairManagerCreate(inNPairs);
  inNObjects_00->mChanging = 1;
  inNObjects_00->mManager = pMVar6;
  return inNObjects_00;
}


/* ==== Insert ==== */

/* DWARF original prototype: MeI32 Insert(CxSmallSort * this, McdModelID.conflict inData,
   McdCullingTable * cullingTable, MeU32 cullingIndex, MeU32 cullingID) */

MeI32 CxSmallSort::Insert(McdModelID_conflict inData,McdCullingTable *cullingTable,
                         MeU32 cullingIndex,MeU32 cullingID)

{
  McdGeometryInstance *pMVar1;
  MeMatrix4Ptr *ppaMVar2;
  McdSpaceID pMVar3;
  MeReal MVar4;
  int *piVar5;
  undefined4 *puVar6;
  uint uVar7;
  MeReal MVar8;
  McdSpaceID pMVar9;
  McdSpaceID pMVar10;
  int iVar11;
  int iVar12;
  byte *pbVar13;
  int iVar14;
  McdModelIntersectFnPtr *pp_Var15;
  undefined4 in_stack_00000014;
  CxSmallSortRep *rep;
  MeI32 id;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: CxSmallSortMarker * marker@[DW_OP_reg1(ECX)] */
  if (inData->mSpaceID == 0) {
    return -1;
  }
  iVar12 = inData->mSpaceID + -1;
  inData->mSpaceID = iVar12;
  MVar4 = inData->linearVelocity[iVar12];
  pMVar3 = inData->mSpace + (int)MVar4 * 0x98;
  *(MeReal *)(pMVar3 + 8) = MVar4;
  *(McdCullingTable **)(pMVar3 + 0xc) = cullingTable;
  *(undefined4 *)(pMVar3 + 0x18) = in_stack_00000014;
  *(MeU32 *)(pMVar3 + 0x1c) = cullingID;
  *(MeU32 *)(pMVar3 + 0x14) = cullingIndex;
  *(undefined4 *)(pMVar3 + 0x28) = 0x7f7fffff;
  *(McdSpaceID *)(pMVar3 + 0x2c) = pMVar3;
  pMVar3[0x30] = (McdSpace)0x0;
  uVar7 = (inData[1].mRefTM == (MeMatrix4Ptr)&inData[1].mRelTM) - 1 & (uint)inData[1].mRefTM;
  piVar5 = *(int **)(uVar7 + 4);
  *(uint *)(pMVar3 + 0x20) = uVar7;
  *(int **)(pMVar3 + 0x24) = piVar5;
  *piVar5 = (int)(pMVar3 + 0x20);
  *(McdSpaceID *)(*(int *)(pMVar3 + 0x20) + 4) = pMVar3 + 0x20;
  *(undefined4 *)(pMVar3 + 0x3c) = 0x7f7fffff;
  *(McdSpaceID *)(pMVar3 + 0x40) = pMVar3;
  pMVar3[0x44] = (McdSpace)0x1;
  pp_Var15 = &inData[1].mIntersectFn;
  uVar7 = (inData[1].mRefTM == (MeMatrix4Ptr)&inData[1].mRelTM) - 1 & (uint)inData[1].mRefTM;
  *(uint *)(pMVar3 + 0x34) = uVar7;
  piVar5 = *(int **)(uVar7 + 4);
  *(int **)(pMVar3 + 0x38) = piVar5;
  *piVar5 = (int)(pMVar3 + 0x34);
  iVar12 = 1;
  *(McdSpaceID *)(*(int *)(pMVar3 + 0x34) + 4) = pMVar3 + 0x34;
  pMVar9 = pMVar3 + 0x48;
  do {
    *(uint *)(pMVar9 + 8) = 0x7f7fffff;
    *(McdSpaceID *)(pMVar9 + 0xc) = pMVar3;
    pMVar9[0x10] = (McdSpace)0x0;
    uVar7 = ((McdModelIntersectFnPtr *)pp_Var15[1] == pp_Var15) - 1 & (uint)pp_Var15[1];
    *(uint *)pMVar9 = uVar7;
    puVar6 = *(undefined4 **)(uVar7 + 4);
    *(undefined4 **)(pMVar9 + 4) = puVar6;
    *puVar6 = pMVar9;
    *(McdSpaceID *)(*(uint *)pMVar9 + 4) = pMVar9;
    pMVar10 = pMVar9 + 0x14;
    *(uint *)(pMVar9 + 0x1c) = 0x7f7fffff;
    *(McdSpaceID *)(pMVar9 + 0x20) = pMVar3;
    pMVar9[0x24] = (McdSpace)0x1;
    uVar7 = ((McdModelIntersectFnPtr *)pp_Var15[1] == pp_Var15) - 1 & (uint)pp_Var15[1];
    *(uint *)pMVar10 = uVar7;
    puVar6 = *(undefined4 **)(uVar7 + 4);
    *(undefined4 **)(pMVar9 + 0x18) = puVar6;
    *puVar6 = pMVar10;
    *(McdSpaceID *)(*(uint *)pMVar10 + 4) = pMVar10;
    pMVar10 = pMVar9 + 0x28;
    pMVar1 = (McdGeometryInstance *)(pp_Var15 + 2);
    *(uint *)(pMVar9 + 0x30) = 0x7f7fffff;
    *(McdSpaceID *)(pMVar9 + 0x34) = pMVar3;
    pMVar9[0x38] = (McdSpace)0x0;
    uVar7 = ((McdGeometryInstance *)pp_Var15[3] == pMVar1) - 1 & (uint)pp_Var15[3];
    *(uint *)pMVar10 = uVar7;
    puVar6 = *(undefined4 **)(uVar7 + 4);
    *(undefined4 **)(pMVar9 + 0x2c) = puVar6;
    *puVar6 = pMVar10;
    *(McdSpaceID *)(*(uint *)pMVar10 + 4) = pMVar10;
    pMVar10 = pMVar9 + 0x3c;
    *(uint *)(pMVar9 + 0x44) = 0x7f7fffff;
    *(McdSpaceID *)(pMVar9 + 0x48) = pMVar3;
    pMVar9[0x4c] = (McdSpace)0x1;
    ppaMVar2 = (MeMatrix4Ptr *)(pp_Var15 + 3);
    pp_Var15 = pp_Var15 + 4;
    uVar7 = ((McdGeometryInstance *)*ppaMVar2 == pMVar1) - 1 & (uint)*ppaMVar2;
    *(uint *)pMVar10 = uVar7;
    puVar6 = *(undefined4 **)(uVar7 + 4);
    *(undefined4 **)(pMVar9 + 0x40) = puVar6;
    *puVar6 = pMVar10;
    *(McdSpaceID *)(*(uint *)pMVar10 + 4) = pMVar10;
    pMVar9 = pMVar9 + 0x50;
    iVar12 = iVar12 + -2;
  } while (-1 < iVar12);
  iVar12 = 0;
  SetDynamic((MeI32)inData,SUB41(MVar4,0));
  if ((int)MVar4 < 1) goto LAB_00010a6f;
  uVar7 = (uint)MVar4 & 3;
  if ((int)MVar4 < 2) {
LAB_00010954:
    iVar14 = iVar12 + 2;
    iVar11 = (int)inData->mPadding * iVar12;
    iVar12 = iVar12 + 1;
    uVar7 = (iVar11 + (int)MVar4) - (iVar14 * iVar12 >> 1);
    pbVar13 = (byte *)(((int)uVar7 >> 1) + (int)inData->mBody);
    *pbVar13 = *pbVar13 & (byte)(0xf0 >> (sbyte)((uVar7 & 1) << 2));
    if ((int)MVar4 <= iVar12) goto LAB_00010a6f;
  }
  else if (uVar7 != 0) {
    if (1 < uVar7) {
      if (2 < uVar7) {
                    /* Unresolved local var: MeU8 * mem@[???]
                       Unresolved local var: MeU8 shift@[???] */
        pbVar13 = (byte *)(((int)((int)MVar4 - 1U) >> 1) + (int)inData->mBody);
        *pbVar13 = *pbVar13 & (byte)(0xf0 >> (sbyte)(((int)MVar4 - 1U & 1) << 2));
      }
      uVar7 = (uint)(2 < uVar7);
      iVar12 = uVar7 + 1;
      uVar7 = ((int)inData->mPadding * uVar7 + (int)MVar4) - ((int)((uVar7 + 2) * iVar12) >> 1);
      pbVar13 = (byte *)(((int)uVar7 >> 1) + (int)inData->mBody);
      *pbVar13 = *pbVar13 & (byte)(0xf0 >> (sbyte)((uVar7 & 1) << 2));
    }
    goto LAB_00010954;
  }
  do {
    uVar7 = ((int)inData->mPadding * iVar12 + (int)MVar4) - ((iVar12 + 2) * (iVar12 + 1) >> 1);
    pbVar13 = (byte *)(((int)uVar7 >> 1) + (int)inData->mBody);
    *pbVar13 = *pbVar13 & (byte)(0xf0 >> (sbyte)((uVar7 & 1) << 2));
    uVar7 = ((int)inData->mPadding * (iVar12 + 1) + (int)MVar4) - ((iVar12 + 3) * (iVar12 + 2) >> 1)
    ;
    pbVar13 = (byte *)(((int)uVar7 >> 1) + (int)inData->mBody);
    *pbVar13 = *pbVar13 & (byte)(0xf0 >> (sbyte)((uVar7 & 1) << 2));
    iVar14 = iVar12 + 3;
    uVar7 = ((int)inData->mPadding * (iVar12 + 2) + (int)MVar4) - ((iVar12 + 4) * iVar14 >> 1);
    pbVar13 = (byte *)(((int)uVar7 >> 1) + (int)inData->mBody);
    *pbVar13 = *pbVar13 & (byte)(0xf0 >> (sbyte)((uVar7 & 1) << 2));
    iVar11 = iVar12 + 5;
    iVar12 = iVar12 + 4;
    uVar7 = ((int)inData->mPadding * iVar14 + (int)MVar4) - (iVar11 * iVar12 >> 1);
    pbVar13 = (byte *)(((int)uVar7 >> 1) + (int)inData->mBody);
    *pbVar13 = *pbVar13 & (byte)(0xf0 >> (sbyte)((uVar7 & 1) << 2));
  } while (iVar12 < (int)MVar4);
LAB_00010a6f:
  iVar12 = (int)MVar4 + 1;
  MVar8 = inData->mPadding;
  if (iVar12 < (int)MVar8) {
    iVar14 = ((int)MVar4 + 2U) * iVar12;
    do {
                    /* Unresolved local var: MeU8 * mem@[???]
                       Unresolved local var: MeU8 shift@[???] */
      uVar7 = ((int)MVar8 * (int)MVar4 + iVar12) - (iVar14 >> 1);
      iVar12 = iVar12 + 1;
      pbVar13 = (byte *)(((int)uVar7 >> 1) + (int)inData->mBody);
      *pbVar13 = *pbVar13 & (byte)(0xf0 >> (sbyte)((uVar7 & 1) << 2));
      MVar8 = inData->mPadding;
    } while (iVar12 < (int)MVar8);
  }
  return (MeI32)MVar4;
}


/* ==== Remove ==== */

/* DWARF original prototype: bool Remove(CxSmallSort * this, MeI32 inID) */

bool CxSmallSort::Remove(MeI32 inID)

{
  byte *pbVar1;
  int *piVar2;
  bool bVar3;
  uint uVar4;
  uint uVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int in_stack_00000008;
  int local_30;
  CxSmallSortRep *rep;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: CxSmallSortMarker * marker@[???] */
  bVar3 = false;
  if (((-1 < in_stack_00000008) && (in_stack_00000008 < *(int *)(inID + 0x18))) &&
     (*(int *)(*(int *)(inID + 0xc) + 8 + in_stack_00000008 * 0x98) == in_stack_00000008)) {
    bVar3 = true;
  }
  if (bVar3) {
    SetDynamic(inID,SUB41(in_stack_00000008,0));
    piVar6 = (int *)(*(int *)(inID + 0xc) + in_stack_00000008 * 0x98);
    iVar9 = piVar6[8];
    *(int *)(iVar9 + 4) = piVar6[9];
    piVar2 = (int *)piVar6[9];
    piVar6[9] = (int)(piVar6 + 8);
    *piVar2 = iVar9;
    piVar6[8] = (int)(piVar6 + 8);
    iVar9 = piVar6[0xd];
    *(int *)(iVar9 + 4) = piVar6[0xe];
    piVar2 = (int *)piVar6[0xe];
    piVar6[0xe] = (int)(piVar6 + 0xd);
    *piVar2 = iVar9;
    piVar6[0xd] = (int)(piVar6 + 0xd);
    iVar9 = piVar6[0x12];
    *(int *)(iVar9 + 4) = piVar6[0x13];
    piVar2 = (int *)piVar6[0x13];
    piVar6[0x13] = (int)(piVar6 + 0x12);
    *piVar2 = iVar9;
    piVar6[0x12] = (int)(piVar6 + 0x12);
    iVar9 = piVar6[0x17];
    *(int *)(iVar9 + 4) = piVar6[0x18];
    piVar2 = (int *)piVar6[0x18];
    piVar6[0x18] = (int)(piVar6 + 0x17);
    *piVar2 = iVar9;
    piVar6[0x17] = (int)(piVar6 + 0x17);
    iVar9 = piVar6[0x1c];
    *(int *)(iVar9 + 4) = piVar6[0x1d];
    piVar2 = (int *)piVar6[0x1d];
    piVar6[0x1d] = (int)(piVar6 + 0x1c);
    *piVar2 = iVar9;
    piVar6[0x1c] = (int)(piVar6 + 0x1c);
    iVar9 = piVar6[0x21];
    *(int *)(iVar9 + 4) = piVar6[0x22];
    piVar2 = (int *)piVar6[0x22];
    piVar6[0x22] = (int)(piVar6 + 0x21);
    *piVar2 = iVar9;
    piVar6[0x21] = (int)(piVar6 + 0x21);
    iVar9 = 0;
    if (0 < in_stack_00000008) {
      do {
                    /* Unresolved local var: MeU32 index@[???] */
        iVar10 = iVar9 + 1;
        uVar4 = (*(int *)(inID + 0x18) * iVar9 + in_stack_00000008) - ((iVar9 + 2) * iVar10 >> 1);
        iVar8 = *(int *)(inID + 0x24);
        uVar5 = uVar4 & 1;
        iVar7 = (int)uVar4 >> 1;
        if (*(byte *)(inID + 4) ==
            ((byte)((int)(uint)*(byte *)(iVar7 + iVar8) >> (sbyte)(uVar5 << 2)) & 0xf)) {
          McdModelPairManagerDeactivate
                    (*(void **)(inID + 0x2c),*(void **)(*(int *)(inID + 0xc) + 0xc + iVar9 * 0x98),
                     *(void **)(*(int *)(inID + 0xc) + 0xc + in_stack_00000008 * 0x98));
          iVar8 = *(int *)(inID + 0x24);
        }
                    /* Unresolved local var: MeU8 * mem@[???]
                       Unresolved local var: MeU8 shift@[???] */
        pbVar1 = (byte *)(iVar7 + iVar8);
        *pbVar1 = *pbVar1 & (byte)(0xf0 >> (sbyte)(uVar5 << 2));
        iVar9 = iVar10;
      } while (iVar10 < in_stack_00000008);
    }
    iVar10 = in_stack_00000008 + 1;
    iVar9 = *(int *)(inID + 0x18);
    if (iVar10 < iVar9) {
      iVar8 = (in_stack_00000008 + 2) * iVar10;
      local_30 = iVar10 * 0x98;
      do {
                    /* Unresolved local var: MeU32 index@[???] */
        iVar7 = *(int *)(inID + 0x24);
        uVar4 = (iVar10 + iVar9 * in_stack_00000008) - (iVar8 >> 1);
        uVar5 = uVar4 & 1;
        iVar9 = (int)uVar4 >> 1;
        if (*(byte *)(inID + 4) ==
            ((byte)((int)(uint)*(byte *)(iVar9 + iVar7) >> (sbyte)(uVar5 << 2)) & 0xf)) {
          McdModelPairManagerDeactivate
                    (*(void **)(inID + 0x2c),
                     *(void **)(*(int *)(inID + 0xc) + 0xc + in_stack_00000008 * 0x98),
                     *(void **)(*(int *)(inID + 0xc) + 0xc + local_30));
          iVar7 = *(int *)(inID + 0x24);
        }
                    /* Unresolved local var: MeU8 * mem@[???]
                       Unresolved local var: MeU8 shift@[???] */
        iVar10 = iVar10 + 1;
        pbVar1 = (byte *)(iVar9 + iVar7);
        *pbVar1 = *pbVar1 & (byte)(0xf0 >> (sbyte)(uVar5 << 2));
        local_30 = local_30 + 0x98;
        iVar9 = *(int *)(inID + 0x18);
      } while (iVar10 < iVar9);
    }
    if ((char)piVar6[4] != '\0') {
      iVar9 = *piVar6;
      *(int *)(iVar9 + 4) = piVar6[1];
      piVar2 = (int *)piVar6[1];
      piVar6[1] = (int)piVar6;
      *piVar2 = iVar9;
      *piVar6 = (int)piVar6;
    }
    *(int *)(*(int *)(inID + 0x14) + *(int *)(inID + 0x20) * 4) = piVar6[2];
    *(int *)(inID + 0x20) = *(int *)(inID + 0x20) + 1;
    piVar6[2] = -1;
    piVar6[3] = 0;
  }
  return true;
}


/* ==== SetDynamic ==== */

/* DWARF original prototype: bool SetDynamic(CxSmallSort * this, MeI32 inID, bool inIsDynamic) */

bool CxSmallSort::SetDynamic(MeI32 inID,bool inIsDynamic)

{
  int iVar1;
  bool bVar2;
  bool bVar3;
  int *piVar4;
  int *piVar5;
  int3 in_stack_00000009;
  char in_stack_0000000c;
  
  bVar2 = false;
  if (((-1 < in_stack_00000009) && (_inIsDynamic < *(int *)(inID + 0x18))) &&
     (*(int *)(*(int *)(inID + 0xc) + 8 + _inIsDynamic * 0x98) == _inIsDynamic)) {
    bVar2 = true;
  }
  bVar3 = false;
  if (bVar2) {
    iVar1 = *(int *)(inID + 0xc);
    _inIsDynamic = _inIsDynamic * 0x98;
    if (*(char *)(iVar1 + 0x10 + _inIsDynamic) != in_stack_0000000c) {
      if (in_stack_0000000c == '\0') {
        *(undefined1 *)(iVar1 + 0x10 + _inIsDynamic) = 0;
        piVar5 = (int *)(_inIsDynamic + *(int *)(inID + 0xc));
        iVar1 = *piVar5;
        *(int *)(iVar1 + 4) = piVar5[1];
        piVar4 = (int *)piVar5[1];
        piVar5[1] = (int)piVar5;
        *piVar4 = iVar1;
        *piVar5 = (int)piVar5;
      }
      else {
        *(undefined1 *)(iVar1 + 0x10 + _inIsDynamic) = 1;
        piVar4 = (int *)(_inIsDynamic + *(int *)(inID + 0xc));
        piVar4[1] = inID + 0xc0;
        *piVar4 = *(int *)(inID + 0xc0);
        *(int **)(inID + 0xc0) = piVar4;
        *(int **)(*piVar4 + 4) = piVar4;
      }
    }
    bVar3 = true;
  }
  return bVar3;
}


/* ==== GetDynamic ==== */

/* DWARF original prototype: bool GetDynamic(CxSmallSort * this, MeI32 inID) */

bool CxSmallSort::GetDynamic(MeI32 inID)

{
  bool bVar1;
  undefined1 uVar2;
  int in_stack_00000008;
  
  bVar1 = false;
  if (((-1 < in_stack_00000008) && (in_stack_00000008 < *(int *)(inID + 0x18))) &&
     (*(int *)(*(int *)(inID + 0xc) + 8 + in_stack_00000008 * 0x98) == in_stack_00000008)) {
    bVar1 = true;
  }
  uVar2 = 0;
  if (bVar1) {
    uVar2 = *(undefined1 *)(*(int *)(inID + 0xc) + 0x10 + in_stack_00000008 * 0x98);
  }
  return (bool)uVar2;
}


/* ==== SetData ==== */

/* DWARF original prototype: void SetData(CxSmallSort * this, void * inData) */

void CxSmallSort::SetData(void *inData)

{
  undefined4 in_stack_00000008;
  
  *(undefined4 *)((int)inData + 0xd0) = in_stack_00000008;
  return;
}


/* ==== GetData ==== */

/* DWARF original prototype: void * GetData(CxSmallSort * this) */

void * CxSmallSort::GetData(void)

{
  int in_stack_00000004;
  
  return *(void **)(in_stack_00000004 + 0xd0);
}


/* ==== ValidID ==== */

/* DWARF original prototype: bool ValidID(CxSmallSort * this, MeI32 inID) */

bool CxSmallSort::ValidID(MeI32 inID)

{
  bool bVar1;
  int in_stack_00000008;
  
  bVar1 = false;
  if (((-1 < in_stack_00000008) && (in_stack_00000008 < *(int *)(inID + 0x18))) &&
     (*(int *)(*(int *)(inID + 0xc) + 8 + in_stack_00000008 * 0x98) == in_stack_00000008)) {
    bVar1 = true;
  }
  return bVar1;
}


/* ==== GetPairOverlapEnabled ==== */

/* DWARF original prototype: bool GetPairOverlapEnabled(CxSmallSort * this, MeI32 inID1, MeI32
   inID2) */

bool CxSmallSort::GetPairOverlapEnabled(MeI32 inID1,MeI32 inID2)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  uint in_stack_0000000c;
  
                    /* Unresolved local var: MeI32 index@[???] */
  bVar1 = false;
  if (inID2 != in_stack_0000000c) {
                    /* Unresolved local var: int o@[DW_OP_reg0(EAX)]
                       Unresolved local var: int d@[DW_OP_reg3(EBX)]
                       Unresolved local var: int rowIndex@[DW_OP_reg2(EDX)]
                       Unresolved local var: int colIndex@[???] */
    uVar2 = (int)(in_stack_0000000c - inID2) >> 0x1f;
    uVar3 = (uVar2 ^ 0xffffffff) & inID2 | uVar2 & in_stack_0000000c;
    uVar2 = (*(int *)(inID1 + 0x18) * uVar3 +
            (uVar2 & inID2 | (uVar2 ^ 0xffffffff) & in_stack_0000000c)) -
            ((int)((uVar3 + 2) * (uVar3 + 1)) >> 1);
    bVar1 = (bool)(((byte)(((int)(uint)*(byte *)(((int)uVar2 >> 1) + *(int *)(inID1 + 0x24)) >>
                            (sbyte)((uVar2 & 1) << 2) & 0xfU) >> 2) ^ 1) & 1);
  }
  return bVar1;
}


/* ==== SetPairOverlapEnabled ==== */

/* DWARF original prototype: bool SetPairOverlapEnabled(CxSmallSort * this, MeI32 inID1, MeI32
   inID2, bool enable) */

bool CxSmallSort::SetPairOverlapEnabled(MeI32 inID1,MeI32 inID2,bool enable)

{
  byte bVar1;
  bool bVar2;
  int iVar3;
  sbyte sVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  undefined3 in_stack_0000000d;
  char in_stack_00000010;
  MeI32 l;
  
                    /* Unresolved local var: MeI32 i1@[???]
                       Unresolved local var: MeI32 i2@[???]
                       Unresolved local var: MeU32 index@[???]
                       Unresolved local var: MeU8 state@[DW_OP_reg6(ESI)] */
  bVar2 = false;
  if (inID2 != _enable) {
                    /* Unresolved local var: MeI32 o@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)] */
    uVar6 = (int)(_enable - inID2) >> 0x1f;
    uVar7 = uVar6 & inID2 | (uVar6 ^ 0xffffffff) & _enable;
    uVar8 = (uVar6 ^ 0xffffffff) & inID2 | uVar6 & _enable;
    iVar5 = (uVar8 * *(int *)(inID1 + 0x18) + uVar7) - ((int)((uVar8 + 2) * (uVar8 + 1)) >> 1);
    iVar3 = iVar5 >> 1;
    bVar1 = (byte)iVar5 & 1;
    iVar5 = *(int *)(inID1 + 0x24);
    sVar4 = bVar1 * '\x04';
    uVar6 = (int)(uint)*(byte *)(iVar3 + iVar5) >> sVar4;
    if (in_stack_00000010 == '\0') {
                    /* Unresolved local var: MeU8 * mem@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeU8 shift@[???] */
      if (((uVar6 & 4) == 0) &&
         (sVar4 = bVar1 * '\x04',
         *(byte *)(iVar5 + iVar3) =
              (byte)(0xf0 >> sVar4) & *(byte *)(iVar5 + iVar3) | (byte)((uVar6 & 0xf | 4) << sVar4),
         (char)(uVar6 & 0xf) == *(char *)(inID1 + 4))) {
        McdModelPairManagerDeactivate
                  (*(void **)(inID1 + 0x2c),*(void **)(*(int *)(inID1 + 0xc) + 0xc + uVar8 * 0x98),
                   *(void **)(*(int *)(inID1 + 0xc) + 0xc + uVar7 * 0x98));
      }
    }
    else {
                    /* Unresolved local var: MeU8 * mem@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeU8 shift@[???] */
      if (((uVar6 & 4) != 0) &&
         (*(byte *)(iVar5 + iVar3) =
               *(byte *)(iVar5 + iVar3) & (byte)(0xf0 >> sVar4) | (byte)((uVar6 & 0xb) << sVar4),
         (char)(uVar6 & 0xb) == *(char *)(inID1 + 4))) {
        McdModelPairManagerActivate
                  (*(void **)(inID1 + 0x2c),*(void **)(*(int *)(inID1 + 0xc) + 0xc + uVar8 * 0x98),
                   *(void **)(*(int *)(inID1 + 0xc) + 0xc + uVar7 * 0x98));
      }
    }
    bVar2 = true;
  }
  return bVar2;
}


/* ==== getPairs ==== */

/* DWARF original prototype: int getPairs(CxSmallSort * this, McdSpacePairIterator * iter,
   McdModelPairContainer * a) */

int CxSmallSort::getPairs(McdSpacePairIterator *iter,McdModelPairContainer *a)

{
  int iVar1;
  void *in_stack_0000000c;
  
  iVar1 = McdModelPairManagerGetPairs(iter[5].ptr,a,in_stack_0000000c);
  return iVar1;
}


/* ==== getTransitions ==== */

/* DWARF original prototype: int getTransitions(CxSmallSort * this, McdSpacePairIterator * iter,
   McdModelPairContainer * a) */

int CxSmallSort::getTransitions(McdSpacePairIterator *iter,McdModelPairContainer *a)

{
  int iVar1;
  void *in_stack_0000000c;
  
  iVar1 = McdModelPairManagerGetTransitions(iter[5].ptr,a,in_stack_0000000c);
  return iVar1;
}


/* ==== MoveStartMarkerUp ==== */

/* DWARF original prototype: void MoveStartMarkerUp(CxSmallSort * this, CxSmallSortMarker *
   inMarker) */

void CxSmallSort::MoveStartMarkerUp(CxSmallSortMarker *inMarker)

{
  float fVar1;
  int iVar2;
  uint uVar3;
  undefined4 *puVar4;
  int iVar5;
  uint uVar6;
  byte bVar7;
  uint uVar8;
  sbyte sVar9;
  int iVar10;
  int *piVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  int iVar16;
  byte *pbVar17;
  int *piVar18;
  int *in_stack_00000008;
  MeU8 state;
  MeU32 o;
  MeU8 *mem;
  MeI32 l;
  CxSmallSortRep *rep;
  MeReal inOrdinate;
  MeI32 inID;
  
                    /* Unresolved local var: CxSmallSortMarker * marker@[DW_OP_reg7(EDI)]
                       Unresolved local var: CxSmallSortRep * inRep@[???] */
  iVar2 = in_stack_00000008[3];
  piVar11 = (int *)*in_stack_00000008;
  uVar3 = *(uint *)(iVar2 + 8);
  fVar1 = (float)in_stack_00000008[2];
  piVar18 = piVar11;
  if (((float)piVar11[2] < fVar1) && ((char)piVar11[4] != -1)) {
    do {
      if ((char)piVar18[4] == '\x01') {
                    /* Unresolved local var: MeI32 i1@[???]
                       Unresolved local var: MeI32 i2@[???] */
        iVar5 = piVar18[3];
                    /* Unresolved local var: MeU32 index@[???]
                       Unresolved local var: MeI32 o@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)] */
        uVar14 = *(uint *)(iVar5 + 8);
        uVar13 = (int)(uVar14 - uVar3) >> 0x1f;
        uVar15 = uVar3 & (uVar13 ^ 0xffffffff) | uVar14 & uVar13;
        uVar14 = uVar13 & uVar3 | (uVar13 ^ 0xffffffff) & uVar14;
        iVar10 = (uVar14 + uVar15 * (int)inMarker[1].super_Link.mPrev) -
                 ((int)((uVar15 + 2) * (uVar15 + 1)) >> 1);
                    /* Unresolved local var: MeU8 shift@[???] */
        iVar16 = iVar10 >> 1;
        sVar9 = ((byte)iVar10 & 1) * '\x04';
        bVar7 = (byte)((int)(uint)*(byte *)(iVar16 + *(int *)&inMarker[1].mType) >> sVar9) & 0xf;
        pbVar17 = (byte *)(iVar16 + *(int *)&inMarker[1].mType);
        *pbVar17 = (byte)(0xf0 >> sVar9) & *pbVar17 | (byte)((bVar7 - 1 & 0xff) << sVar9);
        if (*(byte *)&(inMarker->super_Link).mPrev == bVar7) {
          iVar10 = *(int *)(iVar2 + 0x14);
          if (((iVar10 != 0) && (iVar10 == *(int *)(iVar5 + 0x14))) &&
             (*(int *)(iVar2 + 0x18) == *(int *)(iVar5 + 0x18))) {
                    /* Unresolved local var: MeU32 d@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeU32 l@[???]
                       Unresolved local var: MeU32 index@[DW_OP_reg0(EAX)] */
            uVar13 = *(uint *)(iVar5 + 0x1c);
            uVar6 = *(uint *)(iVar2 + 0x1c);
            uVar12 = 0xffffffff - ((int)(uVar13 - uVar6) >> 0x1f);
            uVar8 = uVar13 & uVar12 | (uVar12 ^ 0xffffffff) & uVar6;
            uVar13 = (uVar8 * (uVar8 + 1) >> 1) + (uVar12 & uVar6 | (uVar12 ^ 0xffffffff) & uVar13);
            if ((*(uint *)(iVar10 + 8 + (uVar13 >> 5) * 4) & 1 << ((byte)uVar13 & 0x1f)) != 0)
            goto LAB_000111db;
          }
          McdModelPairManagerDeactivate
                    (inMarker[2].super_Link.mPrev,inMarker->mRep[uVar15].mModel,
                     inMarker->mRep[uVar14].mModel);
        }
      }
LAB_000111db:
      piVar18 = (int *)*piVar18;
      if (fVar1 <= (float)piVar18[2]) {
        piVar11 = (int *)*in_stack_00000008;
        goto LAB_000111f5;
      }
    } while (*(char *)(piVar18 + 4) != -1);
    piVar11 = (int *)*in_stack_00000008;
  }
LAB_000111f5:
  piVar11[1] = in_stack_00000008[1];
  *(int **)in_stack_00000008[1] = piVar11;
  puVar4 = (undefined4 *)piVar18[1];
  *in_stack_00000008 = (int)piVar18;
  in_stack_00000008[1] = (int)puVar4;
  *puVar4 = in_stack_00000008;
  *(int **)(*in_stack_00000008 + 4) = in_stack_00000008;
  return;
}


/* ==== MoveStartMarkerDown ==== */

/* DWARF original prototype: void MoveStartMarkerDown(CxSmallSort * this, CxSmallSortMarker *
   inMarker) */

void CxSmallSort::MoveStartMarkerDown(CxSmallSortMarker *inMarker)

{
  float fVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  char cVar6;
  uint uVar7;
  sbyte sVar8;
  int *piVar9;
  int iVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  byte *pbVar15;
  uint uVar16;
  int *piVar17;
  int *in_stack_00000008;
  MeU8 state;
  MeI32 l;
  CxSmallSortRep *rep;
  MeReal inOrdinate;
  MeI32 inID;
  
                    /* Unresolved local var: CxSmallSortMarker * marker@[DW_OP_reg7(EDI)]
                       Unresolved local var: CxSmallSortRep * inRep@[???] */
  iVar2 = in_stack_00000008[3];
  piVar9 = (int *)in_stack_00000008[1];
  uVar3 = *(uint *)(iVar2 + 8);
  fVar1 = (float)in_stack_00000008[2];
  piVar17 = piVar9;
  if ((fVar1 <= (float)piVar9[2]) && ((char)piVar9[4] != -1)) {
    do {
      if ((char)piVar17[4] == '\x01') {
                    /* Unresolved local var: MeI32 i1@[???]
                       Unresolved local var: MeI32 i2@[???] */
        iVar4 = piVar17[3];
                    /* Unresolved local var: MeU32 index@[???]
                       Unresolved local var: MeI32 o@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)] */
        uVar12 = *(uint *)(iVar4 + 8);
        uVar11 = (int)(uVar12 - uVar3) >> 0x1f;
        uVar16 = uVar3 & (uVar11 ^ 0xffffffff) | uVar11 & uVar12;
        uVar12 = uVar11 & uVar3 | (uVar11 ^ 0xffffffff) & uVar12;
        iVar10 = (uVar12 + uVar16 * (int)inMarker[1].super_Link.mPrev) -
                 ((int)((uVar16 + 2) * (uVar16 + 1)) >> 1);
        iVar13 = iVar10 >> 1;
                    /* Unresolved local var: MeU8 * mem@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeU8 shift@[???] */
        pbVar15 = (byte *)(*(int *)&inMarker[1].mType + iVar13);
        sVar8 = ((byte)iVar10 & 1) * '\x04';
        cVar6 = ((byte)((int)(uint)*(byte *)(iVar13 + *(int *)&inMarker[1].mType) >> sVar8) & 0xf) +
                1;
        *pbVar15 = *pbVar15 & (byte)(0xf0 >> sVar8) | cVar6 << sVar8;
        if (cVar6 == *(char *)&(inMarker->super_Link).mPrev) {
          iVar10 = *(int *)(iVar2 + 0x14);
          if (((iVar10 != 0) && (iVar10 == *(int *)(iVar4 + 0x14))) &&
             (*(int *)(iVar2 + 0x18) == *(int *)(iVar4 + 0x18))) {
                    /* Unresolved local var: MeU32 d@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeU32 o@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeU32 l@[???]
                       Unresolved local var: MeU32 index@[DW_OP_reg0(EAX)] */
            uVar11 = *(uint *)(iVar4 + 0x1c);
            uVar5 = *(uint *)(iVar2 + 0x1c);
            uVar14 = 0xffffffff - ((int)(uVar11 - uVar5) >> 0x1f);
            uVar7 = uVar11 & uVar14 | (uVar14 ^ 0xffffffff) & uVar5;
            uVar11 = (uVar7 * (uVar7 + 1) >> 1) + (uVar14 & uVar5 | (uVar14 ^ 0xffffffff) & uVar11);
            if ((*(uint *)(iVar10 + 8 + (uVar11 >> 5) * 4) & 1 << ((byte)uVar11 & 0x1f)) != 0)
            goto LAB_000113cc;
          }
          McdModelPairManagerActivate
                    (inMarker[2].super_Link.mPrev,inMarker->mRep[uVar16].mModel,
                     inMarker->mRep[uVar12].mModel);
        }
      }
LAB_000113cc:
      piVar17 = (int *)piVar17[1];
      if ((float)piVar17[2] < fVar1) {
        piVar9 = (int *)in_stack_00000008[1];
        goto LAB_000113e8;
      }
    } while ((char)piVar17[4] != -1);
    piVar9 = (int *)in_stack_00000008[1];
  }
LAB_000113e8:
  iVar2 = *in_stack_00000008;
  *(int **)(iVar2 + 4) = piVar9;
  piVar9 = (int *)in_stack_00000008[1];
  in_stack_00000008[1] = (int)piVar17;
  *piVar9 = iVar2;
  *in_stack_00000008 = *piVar17;
  *piVar17 = (int)in_stack_00000008;
  *(int **)(*in_stack_00000008 + 4) = in_stack_00000008;
  return;
}


/* ==== MoveEndMarkerUp ==== */

/* DWARF original prototype: void MoveEndMarkerUp(CxSmallSort * this, CxSmallSortMarker * inMarker)
    */

void CxSmallSort::MoveEndMarkerUp(CxSmallSortMarker *inMarker)

{
  float fVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  uint uVar6;
  char cVar7;
  uint uVar8;
  sbyte sVar9;
  int iVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  int *piVar14;
  uint uVar15;
  byte *pbVar16;
  uint uVar17;
  int *piVar18;
  int *in_stack_00000008;
  MeU8 state;
  MeI32 l;
  CxSmallSortRep *rep;
  MeReal inOrdinate;
  MeI32 inID;
  
                    /* Unresolved local var: CxSmallSortMarker * marker@[DW_OP_reg7(EDI)]
                       Unresolved local var: CxSmallSortRep * inRep@[???] */
  iVar2 = in_stack_00000008[3];
  piVar14 = (int *)*in_stack_00000008;
  uVar3 = *(uint *)(iVar2 + 8);
  fVar1 = (float)in_stack_00000008[2];
  piVar18 = piVar14;
  if (((float)piVar14[2] <= fVar1) && ((char)piVar14[4] != -1)) {
    do {
      if ((char)piVar14[4] == '\0') {
                    /* Unresolved local var: MeI32 i1@[???]
                       Unresolved local var: MeI32 i2@[???] */
        iVar4 = piVar14[3];
                    /* Unresolved local var: MeU32 index@[???]
                       Unresolved local var: MeI32 o@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)] */
        uVar12 = *(uint *)(iVar4 + 8);
        uVar11 = (int)(uVar12 - uVar3) >> 0x1f;
        uVar17 = uVar3 & (uVar11 ^ 0xffffffff) | uVar11 & uVar12;
        uVar12 = uVar11 & uVar3 | (uVar11 ^ 0xffffffff) & uVar12;
        iVar10 = (uVar12 + uVar17 * (int)inMarker[1].super_Link.mPrev) -
                 ((int)((uVar17 + 2) * (uVar17 + 1)) >> 1);
        iVar13 = iVar10 >> 1;
                    /* Unresolved local var: MeU8 * mem@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeU8 shift@[???] */
        pbVar16 = (byte *)(*(int *)&inMarker[1].mType + iVar13);
        sVar9 = ((byte)iVar10 & 1) * '\x04';
        cVar7 = ((byte)((int)(uint)*(byte *)(iVar13 + *(int *)&inMarker[1].mType) >> sVar9) & 0xf) +
                1;
        *pbVar16 = *pbVar16 & (byte)(0xf0 >> sVar9) | cVar7 << sVar9;
        if (cVar7 == *(char *)&(inMarker->super_Link).mPrev) {
          iVar10 = *(int *)(iVar2 + 0x14);
          if (((iVar10 != 0) && (iVar10 == *(int *)(iVar4 + 0x14))) &&
             (*(int *)(iVar2 + 0x18) == *(int *)(iVar4 + 0x18))) {
                    /* Unresolved local var: MeU32 d@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeU32 o@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeU32 l@[???]
                       Unresolved local var: MeU32 index@[DW_OP_reg0(EAX)] */
            uVar11 = *(uint *)(iVar4 + 0x1c);
            uVar6 = *(uint *)(iVar2 + 0x1c);
            uVar15 = 0xffffffff - ((int)(uVar11 - uVar6) >> 0x1f);
            uVar8 = uVar11 & uVar15 | (uVar15 ^ 0xffffffff) & uVar6;
            uVar11 = (uVar8 * (uVar8 + 1) >> 1) + (uVar15 & uVar6 | (uVar15 ^ 0xffffffff) & uVar11);
            if ((*(uint *)(iVar10 + 8 + (uVar11 >> 5) * 4) & 1 << ((byte)uVar11 & 0x1f)) != 0)
            goto LAB_0001165c;
          }
          McdModelPairManagerActivate
                    (inMarker[2].super_Link.mPrev,inMarker->mRep[uVar17].mModel,
                     inMarker->mRep[uVar12].mModel);
        }
      }
LAB_0001165c:
      piVar18 = (int *)*piVar14;
      if (fVar1 < (float)piVar18[2]) {
        piVar14 = (int *)*in_stack_00000008;
        goto LAB_0001167a;
      }
      piVar14 = piVar18;
    } while (*(char *)(piVar18 + 4) != -1);
    piVar14 = (int *)*in_stack_00000008;
  }
LAB_0001167a:
  piVar14[1] = in_stack_00000008[1];
  *(int **)in_stack_00000008[1] = piVar14;
  puVar5 = (undefined4 *)piVar18[1];
  *in_stack_00000008 = (int)piVar18;
  in_stack_00000008[1] = (int)puVar5;
  *puVar5 = in_stack_00000008;
  *(int **)(*in_stack_00000008 + 4) = in_stack_00000008;
  return;
}


/* ==== MoveEndMarkerDown ==== */

/* DWARF original prototype: void MoveEndMarkerDown(CxSmallSort * this, CxSmallSortMarker *
   inMarker) */

void CxSmallSort::MoveEndMarkerDown(CxSmallSortMarker *inMarker)

{
  Link *pLVar1;
  Link *pLVar2;
  Link *pLVar3;
  CxSmallSortRep *pCVar4;
  McdCullingTable *pMVar5;
  Link *pLVar6;
  byte bVar7;
  uint uVar8;
  sbyte sVar9;
  CxSmallSortMarker *pCVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  byte *pbVar14;
  int iVar15;
  uint uVar16;
  int iVar17;
  Link *in_stack_00000008;
  MeU8 state;
  MeI32 l;
  CxSmallSortRep *rep;
  MeReal inOrdinate;
  MeI32 inID;
  CxSmallSortMarker *marker;
  
                    /* Unresolved local var: CxSmallSortRep * inRep@[???] */
  pCVar10 = (CxSmallSortMarker *)in_stack_00000008->mPrev;
  pLVar2 = in_stack_00000008[1].mPrev;
  pLVar3 = pLVar2[1].mNext;
  pLVar1 = in_stack_00000008[1].mNext;
  marker = pCVar10;
  if (((float)pLVar1 < pCVar10->mOrdinate) && (pCVar10->mType != 0xff)) {
    do {
                    /* Unresolved local var: MeU8 delta@[???] */
      if (marker->mType == '\0') {
                    /* Unresolved local var: MeI32 i1@[???]
                       Unresolved local var: MeI32 i2@[???] */
        pCVar4 = marker->mRep;
                    /* Unresolved local var: MeU32 index@[???]
                       Unresolved local var: MeI32 o@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)] */
        uVar12 = pCVar4->mID;
        uVar11 = (int)(uVar12 - (int)pLVar3) >> 0x1f;
        uVar16 = (uint)pLVar3 & (uVar11 ^ 0xffffffff) | uVar11 & uVar12;
        uVar12 = uVar11 & (uint)pLVar3 | (uVar11 ^ 0xffffffff) & uVar12;
        iVar17 = (uVar12 + uVar16 * (int)inMarker[1].super_Link.mPrev) -
                 ((int)((uVar16 + 2) * (uVar16 + 1)) >> 1);
        iVar15 = iVar17 >> 1;
                    /* Unresolved local var: MeU8 * mem@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeU8 shift@[???] */
        pbVar14 = (byte *)(*(int *)&inMarker[1].mType + iVar15);
        sVar9 = ((byte)iVar17 & 1) * '\x04';
        bVar7 = (byte)((int)(uint)*(byte *)(iVar15 + *(int *)&inMarker[1].mType) >> sVar9) & 0xf;
        *pbVar14 = (byte)(0xf0 >> sVar9) & *pbVar14 | (byte)((bVar7 - 1 & 0xff) << sVar9);
        if (*(byte *)&(inMarker->super_Link).mPrev == bVar7) {
          pMVar5 = (McdCullingTable *)pLVar2[2].mPrev;
          if (((pMVar5 != (McdCullingTable *)0x0) && (pMVar5 == pCVar4->mCullingTable)) &&
             (pLVar2[3].mNext == (Link *)pCVar4->mCullingID)) {
                    /* Unresolved local var: MeU32 d@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeU32 o@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeU32 l@[???]
                       Unresolved local var: MeU32 index@[DW_OP_reg0(EAX)] */
            uVar11 = pCVar4->mCullingIndex;
            pLVar6 = pLVar2[3].mPrev;
            uVar13 = 0xffffffff - ((int)(uVar11 - (int)pLVar6) >> 0x1f);
            uVar8 = uVar11 & uVar13 | (uVar13 ^ 0xffffffff) & (uint)pLVar6;
            uVar11 = (uVar8 * (uVar8 + 1) >> 1) +
                     (uVar13 & (uint)pLVar6 | (uVar13 ^ 0xffffffff) & uVar11);
            if ((pMVar5->array[uVar11 >> 5] & 1 << ((byte)uVar11 & 0x1f)) != 0) goto LAB_00011857;
          }
          McdModelPairManagerDeactivate
                    (inMarker[2].super_Link.mPrev,inMarker->mRep[uVar16].mModel,
                     inMarker->mRep[uVar12].mModel);
        }
      }
LAB_00011857:
      marker = (CxSmallSortMarker *)(marker->super_Link).mPrev;
      if (marker->mOrdinate <= (float)pLVar1) {
        pCVar10 = (CxSmallSortMarker *)in_stack_00000008->mPrev;
        goto LAB_0001187d;
      }
    } while (marker->mType != 0xff);
    pCVar10 = (CxSmallSortMarker *)in_stack_00000008->mPrev;
  }
LAB_0001187d:
  pLVar1 = in_stack_00000008->mNext;
  pLVar1->mPrev = &pCVar10->super_Link;
  in_stack_00000008->mPrev->mNext = pLVar1;
  in_stack_00000008->mPrev = &marker->super_Link;
  in_stack_00000008->mNext = (marker->super_Link).mNext;
  (marker->super_Link).mNext = in_stack_00000008;
  in_stack_00000008->mNext->mPrev = in_stack_00000008;
  return;
}


/* ==== _Update ==== */

/* DWARF original prototype: void _Update(CxSmallSort * this, MeI32 inID, MeVector3Ptr min,
   MeVector3Ptr max) */

void CxSmallSort::_Update(MeI32 inID,MeVector3Ptr min,MeVector3Ptr max)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  bool bVar5;
  int iVar6;
  int iVar7;
  int in_stack_00000010;
  MeReal oldmax;
  MeReal oldmin;
  CxSmallSortMarker *endm;
  CxSmallSortMarker *startm;
  int maxDone;
  
                    /* Unresolved local var: CxSmallSortRep * rep@[???] */
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)] */
  iVar7 = 0;
  iVar6 = *(int *)(inID + 0xc) + (int)min * 0x98;
  do {
    if (((int)(uint)*(byte *)(inID + 5) >> ((byte)iVar7 & 0x1f) & 1U) != 0) {
      bVar5 = false;
      fVar1 = *(float *)(iVar6 + 0x28);
      fVar2 = *(float *)(iVar6 + 0x3c);
      *(MeReal *)(iVar6 + 0x28) = max[iVar7];
      *(undefined4 *)(iVar6 + 0x3c) = *(undefined4 *)(in_stack_00000010 + iVar7 * 4);
      fVar3 = max[iVar7];
      if (fVar2 <= fVar3) {
        fVar4 = *(float *)(in_stack_00000010 + iVar7 * 4);
        if (fVar4 < fVar2) {
          MoveEndMarkerDown((CxSmallSortMarker *)inID);
LAB_00011a95:
          fVar3 = max[iVar7];
        }
        else if (fVar2 < fVar4) {
          MoveEndMarkerUp((CxSmallSortMarker *)inID);
          goto LAB_00011a95;
        }
        bVar5 = true;
      }
      if (fVar3 < fVar1) {
        MoveStartMarkerDown((CxSmallSortMarker *)inID);
      }
      else if (fVar1 < fVar3) {
        MoveStartMarkerUp((CxSmallSortMarker *)inID);
      }
      if (!bVar5) {
        fVar1 = *(float *)(in_stack_00000010 + iVar7 * 4);
        if (fVar1 < fVar2) {
          MoveEndMarkerDown((CxSmallSortMarker *)inID);
        }
        else if (fVar2 < fVar1) {
          MoveEndMarkerUp((CxSmallSortMarker *)inID);
        }
      }
    }
    iVar7 = iVar7 + 1;
    iVar6 = iVar6 + 0x28;
    if (2 < iVar7) {
      return;
    }
  } while( true );
}


/* ==== UpdateSingle ==== */

/* DWARF original prototype: bool UpdateSingle(CxSmallSort * this, MeI32 inID, MeReal duration) */

bool CxSmallSort::UpdateSingle(MeI32 inID,MeReal duration)

{
  void *pvVar1;
  bool bVar2;
  char cVar3;
  MeReal max [3];
  MeReal min [3];
  
  bVar2 = false;
  if (((-1 < (int)duration) && ((int)duration < *(int *)(inID + 0x18))) &&
     (*(MeReal *)(*(int *)(inID + 0xc) + 8 + (int)duration * 0x98) == duration)) {
    bVar2 = true;
  }
  cVar3 = '\0';
  if (bVar2) {
    cVar3 = *(char *)(*(int *)(inID + 0xc) + 0x10 + (int)duration * 0x98);
  }
  if (cVar3 != '\0') {
                    /* Unresolved local var: McdModelID.conflict model@[???] */
    pvVar1 = *(void **)(*(int *)(inID + 0xc) + 0xc + (int)duration * 0x98);
    (**(code **)(inID + 200))(pvVar1);
    McdModelGetAABB(pvVar1,min,max);
    _Update(inID,(MeVector3Ptr)duration,min);
  }
  return true;
}


/* ==== UpdateAll ==== */

/* DWARF original prototype: bool UpdateAll(CxSmallSort * this, MeReal duration) */

bool CxSmallSort::UpdateAll(MeReal duration)

{
  undefined4 uVar1;
  undefined4 extraout_EAX;
  code *pcVar2;
  int *piVar3;
  undefined4 in_stack_00000008;
  MeReal max [3];
  MeReal min [3];
  
                    /* Unresolved local var: CxSmallSortRep * rep@[DW_OP_reg3(EBX)] */
  uVar1 = 0;
  pcVar2 = *(code **)((int)duration + 200);
  if (pcVar2 != (code *)0x0) {
    piVar3 = *(int **)((int)duration + 0xc0);
    if (piVar3 != (int *)((int)duration + 0xc0)) {
      while( true ) {
        (*pcVar2)(piVar3[3],in_stack_00000008,uVar1,uVar1);
        McdModelGetAABB((void *)piVar3[3],min,max);
        _Update((MeI32)duration,(MeVector3Ptr)piVar3[2],min);
        piVar3 = (int *)*piVar3;
        if (piVar3 == (int *)((int)duration + 0xc0)) break;
        pcVar2 = *(code **)((int)duration + 200);
        uVar1 = extraout_EAX;
      }
    }
    uVar1 = 1;
  }
  return SUB41(uVar1,0);
}


/* ==== UpdateBegin ==== */

/* DWARF original prototype: bool UpdateBegin(CxSmallSort * this) */

bool CxSmallSort::UpdateBegin(void)

{
  int iVar1;
  int in_stack_00000004;
  
  McdModelPairManagerFlush(*(void **)(in_stack_00000004 + 0x2c));
  iVar1 = *(int *)(in_stack_00000004 + 0x20) + -1;
  *(int *)(in_stack_00000004 + 0x20) = iVar1;
  while (-1 < iVar1) {
    *(undefined4 *)(*(int *)(in_stack_00000004 + 0x10) + *(int *)(in_stack_00000004 + 0x1c) * 4) =
         *(undefined4 *)
          (*(int *)(in_stack_00000004 + 0x14) + *(int *)(in_stack_00000004 + 0x20) * 4);
    iVar1 = *(int *)(in_stack_00000004 + 0x20) + -1;
    *(int *)(in_stack_00000004 + 0x1c) = *(int *)(in_stack_00000004 + 0x1c) + 1;
    *(int *)(in_stack_00000004 + 0x20) = iVar1;
  }
  *(undefined4 *)(in_stack_00000004 + 0x20) = 0;
  *(undefined4 *)(in_stack_00000004 + 0xcc) = 1;
  return true;
}


/* ==== UpdateEnd ==== */

/* DWARF original prototype: bool UpdateEnd(CxSmallSort * this) */

bool CxSmallSort::UpdateEnd(void)

{
  int in_stack_00000004;
  
  *(undefined4 *)(in_stack_00000004 + 0xcc) = 0;
  return true;
}


/* ==== SetCullingParameters ==== */

/* DWARF original prototype: void SetCullingParameters(CxSmallSort * this, MeI32 inID,
   McdCullingTable * table, MeU32 cullingIndex, MeU32 cullingID) */

void CxSmallSort::SetCullingParameters
               (MeI32 inID,McdCullingTable *table,MeU32 cullingIndex,MeU32 cullingID)

{
  int iVar1;
  uint uVar2;
  bool bVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  McdCullingTable *pMVar9;
  int in_stack_00000014;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  CxSmallSortRep *inRep;
  MeI16 i;
  MeU32 oldCullingID;
  MeU32 oldCullingIndex;
  McdCullingTable *oldTable;
  
  local_30 = *(int *)(inID + 0xc);
  iVar6 = (int)table * 0x98;
  iVar1 = *(int *)(local_30 + 0x14 + iVar6);
  pMVar9 = (McdCullingTable *)0x0;
  uVar2 = *(uint *)(local_30 + 0x1c + iVar6);
  iVar6 = *(int *)(local_30 + 0x18 + iVar6);
  i = 0;
  local_34 = *(int *)(inID + 0x18);
  if (0 < local_34) {
    while( true ) {
      bVar3 = false;
      iVar7 = local_30 + (int)pMVar9 * 0x98;
      if (((-1 < (int)pMVar9) && ((int)pMVar9 < local_34)) &&
         (*(McdCullingTable **)(local_30 + 8 + (int)pMVar9 * 0x98) == pMVar9)) {
        bVar3 = true;
      }
      if ((bVar3) && (pMVar9 != table)) {
                    /* Unresolved local var: MeBool oldCullingState@[???]
                       Unresolved local var: MeBool newCullingState@[???]
                       Unresolved local var: MeI32 a@[???]
                       Unresolved local var: MeI32 b@[???] */
        local_28 = 0;
        if ((iVar1 != 0) && ((*(int *)(iVar7 + 0x14) == iVar1 && (*(int *)(iVar7 + 0x18) == iVar6)))
           ) {
                    /* Unresolved local var: MeU32 d@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeU32 o@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeU32 l@[???]
                       Unresolved local var: MeU32 index@[DW_OP_reg0(EAX)] */
          uVar4 = *(uint *)(iVar7 + 0x1c);
          uVar5 = 0xffffffff - ((int)(uVar2 - uVar4) >> 0x1f);
          uVar8 = (uVar5 ^ 0xffffffff) & uVar4 | uVar5 & uVar2;
          uVar4 = (uVar8 * (uVar8 + 1) >> 1) + (uVar5 & uVar4 | uVar2 & (uVar5 ^ 0xffffffff));
          if ((*(uint *)(iVar1 + 8 + (uVar4 >> 5) * 4) & 1 << ((byte)uVar4 & 0x1f)) != 0) {
            local_28 = 1;
          }
        }
        local_2c = 0;
        if (((cullingIndex != 0) && (*(MeU32 *)(iVar7 + 0x14) == cullingIndex)) &&
           (*(int *)(iVar7 + 0x18) == in_stack_00000014)) {
                    /* Unresolved local var: MeU32 d@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeU32 o@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeU32 l@[???]
                       Unresolved local var: MeU32 index@[DW_OP_reg0(EAX)] */
          uVar4 = *(uint *)(iVar7 + 0x1c);
          uVar5 = 0xffffffff - ((int)(cullingID - uVar4) >> 0x1f);
          uVar8 = (uVar5 ^ 0xffffffff) & uVar4 | uVar5 & cullingID;
          uVar4 = (uVar8 * (uVar8 + 1) >> 1) + (uVar5 & uVar4 | cullingID & (uVar5 ^ 0xffffffff));
          if ((*(uint *)(cullingIndex + 8 + (uVar4 >> 5) * 4) & 1 << ((byte)uVar4 & 0x1f)) != 0) {
            local_2c = 1;
          }
        }
                    /* Unresolved local var: MeI32 o@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeI32 d@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeI32 l@[DW_OP_reg3(EBX)] */
        uVar4 = (int)table - (int)pMVar9 >> 0x1f;
        uVar8 = (uVar4 ^ 0xffffffff) & (uint)pMVar9 | (uint)table & uVar4;
        uVar4 = uVar4 & (uint)pMVar9 | (uVar4 ^ 0xffffffff) & (uint)table;
        if (local_2c != local_28) {
          uVar5 = (local_34 * uVar8 + uVar4) - ((int)((uVar8 + 2) * (uVar8 + 1)) >> 1);
          if (*(byte *)(inID + 4) ==
              ((byte)((int)(uint)*(byte *)(((int)uVar5 >> 1) + *(int *)(inID + 0x24)) >>
                     (sbyte)((uVar5 & 1) << 2)) & 0xf)) {
            if (local_28 == 0) {
              McdModelPairManagerDeactivate
                        (*(void **)(inID + 0x2c),*(void **)(local_30 + 0xc + uVar8 * 0x98),
                         *(void **)(local_30 + 0xc + uVar4 * 0x98));
              local_34 = *(int *)(inID + 0x18);
            }
            else {
              McdModelPairManagerActivate
                        (*(void **)(inID + 0x2c),*(void **)(local_30 + 0xc + uVar8 * 0x98),
                         *(void **)(local_30 + 0xc + uVar4 * 0x98));
              local_34 = *(int *)(inID + 0x18);
            }
          }
        }
      }
      i = i + 1;
      pMVar9 = (McdCullingTable *)(int)i;
      if (local_34 <= (int)pMVar9) break;
      local_30 = *(int *)(inID + 0xc);
    }
  }
  return;
}


