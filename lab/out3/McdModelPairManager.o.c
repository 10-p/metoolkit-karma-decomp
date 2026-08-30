/* ==== McdModelPairManagerHashCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdModelPairManagerHash * McdModelPairManagerHashCreate(int size,int buckets)

{
  McdModelPairManagerHash *pMVar1;
  int iVar2;
  McdModelPairManagerLink **ppMVar3;
  
                    /* Unresolved local var: McdModelPairManagerHash * hash@[DW_OP_reg6(ESI)]
                       Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  pMVar1 = (McdModelPairManagerHash *)(*_MeMemoryAPI)(0xc);
  pMVar1->size = size;
  pMVar1->bucketCount = 1;
  if (1 < buckets) {
    iVar2 = 1;
    do {
      buckets = buckets >> 1;
      iVar2 = iVar2 * 2;
    } while (1 < buckets);
    pMVar1->bucketCount = iVar2;
  }
  ppMVar3 = (McdModelPairManagerLink **)(*_MeMemoryAPI)(pMVar1->bucketCount << 2);
  iVar2 = 0;
  pMVar1->bucket = ppMVar3;
  if (0 < pMVar1->bucketCount) {
    do {
      pMVar1->bucket[iVar2] = (McdModelPairManagerLink *)0x0;
      iVar2 = iVar2 + 1;
    } while (iVar2 < pMVar1->bucketCount);
  }
  return pMVar1;
}


/* ==== McdModelPairManagerHashDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdModelPairManagerHashDestroy(McdModelPairManagerHash *hash)

{
  (*___gxx_personality_v0)(hash->bucket);
  (*___gxx_personality_v0)(hash);
  return;
}


/* ==== McdModelPairManagerCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdModelPairManagerID McdModelPairManagerCreate(int count)

{
  McdModelPairManagerID pMVar1;
  MePool *pMVar2;
  McdModelPairManagerHash *pMVar3;
  
                    /* Unresolved local var: McdModelPairManagerID manager@[DW_OP_reg3(EBX)] */
  pMVar1 = (McdModelPairManagerID)(*_MeMemoryAPI)(0x38);
  pMVar2 = (MePool *)(*_MeMemoryAPI)(0x1c);
  pMVar1->linkPool = pMVar2;
  (*_MePoolAPI)(pMVar2,count,0x14,0);
  pMVar2 = (MePool *)(*_MeMemoryAPI)(0x1c);
  pMVar1->pairPool = pMVar2;
  (*_MePoolAPI)(pMVar2,count,0x1c,0);
  pMVar3 = McdModelPairManagerHashCreate(count,count / 10);
  (pMVar1->goodbyeList).phasePrev = &pMVar1->goodbyeList;
  (pMVar1->goodbyeList).phaseNext = &pMVar1->goodbyeList;
  pMVar1->hash = pMVar3;
  (pMVar1->helloList).phasePrev = (McdModelPairManagerLink *)pMVar1;
  (pMVar1->helloList).phaseNext = (McdModelPairManagerLink *)pMVar1;
  return pMVar1;
}


/* ==== McdModelPairManagerDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdModelPairManagerDestroy(McdModelPairManagerID manager)

{
  McdModelPairManagerHash *pMVar1;
  
  pMVar1 = manager->hash;
  (*___gxx_personality_v0)(pMVar1->bucket);
  (*___gxx_personality_v0)(pMVar1);
  (*_McdModelPairReset)(manager->linkPool);
  (*___gxx_personality_v0)(manager->linkPool);
  (*_McdModelPairReset)(manager->pairPool);
  (*___gxx_personality_v0)(manager->pairPool);
  (*___gxx_personality_v0)(manager);
  return;
}


/* ==== McdModelPairManagerGetTransitions ==== */

MeBool McdModelPairManagerGetTransitions
                 (McdModelPairManagerID manager,McdSpacePairIterator *iter,McdModelPairContainer *a)

{
  int iVar1;
  MeBool MVar2;
  undefined1 *puVar3;
  int iVar4;
  McdModelPairManagerID pMVar5;
  McdModelPairManagerLink *pMVar6;
  
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: McdModelPairManagerLink * link@[???] */
  iVar4 = 0;
  a->goodbyeFirst = 0;
  a->helloFirst = 0;
  iVar1 = iter->count;
  if (iVar1 == -2) {
LAB_00010232:
    puVar3 = iter->ptr;
    pMVar6 = &manager->goodbyeList;
    if (((McdModelPairManagerLink *)puVar3 != pMVar6) && (0 < a->size)) {
      do {
        a->array[iVar4] = *(McdModelPair **)(puVar3 + 0xc);
        iVar4 = iVar4 + 1;
        puVar3 = *(undefined1 **)iter->ptr;
        iter->ptr = puVar3;
        if ((McdModelPairManagerLink *)puVar3 == pMVar6) break;
      } while (iVar4 < a->size);
    }
    a->goodbyeEnd = iVar4;
    pMVar5 = iter->ptr;
    if (pMVar5 == (McdModelPairManagerID)pMVar6) {
      iter->count = -3;
      iter->ptr = (manager->helloList).phaseNext;
      a->helloFirst = iVar4;
      goto LAB_000101dc;
    }
  }
  else {
    if (-2 < iVar1) {
      if (iVar1 != -1) goto LAB_000101bd;
      pMVar6 = (manager->goodbyeList).phaseNext;
      iter->count = -2;
      iter->ptr = pMVar6;
      goto LAB_00010232;
    }
    if (iVar1 != -3) goto LAB_000101bd;
LAB_000101dc:
    pMVar5 = iter->ptr;
  }
  if (pMVar5 != manager) {
    if (iVar4 < a->size) {
      do {
        a->array[iVar4] = (pMVar5->helloList).pair;
        iVar4 = iVar4 + 1;
        pMVar5 = *(McdModelPairManagerID *)iter->ptr;
        iter->ptr = pMVar5;
        if (pMVar5 == manager) break;
      } while (iVar4 < a->size);
    }
    if (pMVar5 != manager) goto LAB_000101bd;
  }
  iter->count = -4;
LAB_000101bd:
  MVar2 = 0;
  a->helloEndStayingFirst = iVar4;
  a->stayingEnd = iVar4;
  if ((iVar4 == a->size) && (-4 < iter->count)) {
    MVar2 = 1;
  }
  return MVar2;
}


/* ==== McdModelPairManagerGetPairs ==== */

MeBool McdModelPairManagerGetPairs
                 (McdModelPairManagerID manager,McdSpacePairIterator *iter,McdModelPairContainer *a)

{
  McdModelPair **ppMVar1;
  McdModelPair *pMVar2;
  int iVar3;
  McdModelPairManagerLink *unaff_EBX;
  int iVar4;
  int result;
  
                    /* Unresolved local var: McdModelPairManagerLink * link@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdModelPair * pair@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int hesf@[DW_OP_reg1(ECX)] */
  iVar4 = 0;
  iVar3 = a->size;
  a->goodbyeFirst = 0;
  a->goodbyeEnd = 0;
  a->helloFirst = iVar3;
  a->stayingEnd = iVar3;
  if (0 < iVar3) {
    do {
      unaff_EBX = McdModelPairManagerHashGetNext(manager->hash,iter);
      if (unaff_EBX == (McdModelPairManagerLink *)0x0) {
        iVar3 = a->size;
        break;
      }
      if (unaff_EBX->pair->phase == kMcdFFStateGoodbye) {
        a->array[a->goodbyeEnd] = unaff_EBX->pair;
        a->goodbyeEnd = a->goodbyeEnd + 1;
      }
      else {
        iVar3 = a->helloFirst + -1;
        a->helloFirst = iVar3;
        a->array[iVar3] = unaff_EBX->pair;
      }
      iVar4 = iVar4 + 1;
      iVar3 = a->size;
    } while (iVar4 < iVar3);
  }
  iVar4 = a->helloFirst;
  if (iVar4 < iVar3) {
    do {
      ppMVar1 = a->array;
      if (ppMVar1[iVar4]->phase == kMcdFFStateStaying) {
        do {
          if (iVar3 <= iVar4) break;
          iVar3 = iVar3 + -1;
        } while (ppMVar1[iVar3]->phase != kMcdFFStateHello);
        pMVar2 = ppMVar1[iVar3];
        ppMVar1[iVar3] = ppMVar1[iVar4];
        a->array[iVar4] = pMVar2;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < iVar3);
  }
  a->helloEndStayingFirst = iVar3;
  return (uint)(unaff_EBX != (McdModelPairManagerLink *)0x0);
}


/* ==== McdModelPairManagerGetPair ==== */

McdModelPairID
McdModelPairManagerGetPair
          (McdModelPairManagerID manager,McdModelID_conflict m1,McdModelID_conflict m2)

{
  uint uVar1;
  uint uVar2;
  McdModelPairID pMVar3;
  uint uVar4;
  McdModelPairManagerLink *pMVar5;
  uint uVar6;
  
                    /* Unresolved local var: MeU32 id1@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeU32 id2@[DW_OP_reg3(EBX)]
                       Unresolved local var: int key@[???]
                       Unresolved local var: McdModelPairManagerLink * link@[???] */
  uVar4 = m2->mSpaceID;
  uVar1 = m1->mSpaceID;
                    /* Unresolved local var: MeU32 d@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeU32 o@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeU32 l@[???] */
  uVar2 = 0xffffffff - ((int)(uVar4 - uVar1) >> 0x1f);
  uVar6 = (uVar2 ^ 0xffffffff) & uVar1 | uVar2 & uVar4;
  uVar4 = (uVar2 & uVar1 | (uVar2 ^ 0xffffffff) & uVar4) << 0x10 | uVar6;
                    /* Unresolved local var: McdModelPairManagerLink * link@[DW_OP_reg0(EAX)] */
  for (pMVar5 = manager->hash->bucket
                [((int)uVar4 >> 0x10 ^ uVar6 & 0xffff) & manager->hash->bucketCount - 1U];
      pMVar5 != (McdModelPairManagerLink *)0x0; pMVar5 = pMVar5->next) {
    if (pMVar5->key == uVar4) goto LAB_000103d1;
  }
  pMVar5 = (McdModelPairManagerLink *)0x0;
LAB_000103d1:
  pMVar3 = (McdModelPairID)0x0;
  if (pMVar5 != (McdModelPairManagerLink *)0x0) {
    pMVar3 = pMVar5->pair;
  }
  return pMVar3;
}


/* ==== McdModelPairManagerActivate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

MeBool McdModelPairManagerActivate
                 (McdModelPairManagerID manager,McdModelID_conflict m1,McdModelID_conflict m2)

{
  uint uVar1;
  McdModelPairManagerLink *pMVar2;
  McdModelPairManagerHash *pMVar3;
  uint uVar4;
  McdModelPairManagerLink *pMVar5;
  McdModelPairID pMVar6;
  uint uVar7;
  uint uVar8;
  
                    /* Unresolved local var: MeU32 id1@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeU32 id2@[DW_OP_reg7(EDI)]
                       Unresolved local var: int key@[???]
                       Unresolved local var: McdModelPairManagerLink * link@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdModelPairManagerLink * link@[DW_OP_reg0(EAX)] */
  uVar8 = m2->mSpaceID;
  uVar1 = m1->mSpaceID;
                    /* Unresolved local var: MeU32 d@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeU32 o@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeU32 l@[???] */
  uVar4 = 0xffffffff - ((int)(uVar8 - uVar1) >> 0x1f);
  uVar7 = (uVar4 ^ 0xffffffff) & uVar1 | uVar4 & uVar8;
  uVar8 = (uVar4 & uVar1 | (uVar4 ^ 0xffffffff) & uVar8) << 0x10 | uVar7;
  for (pMVar5 = manager->hash->bucket
                [((int)uVar8 >> 0x10 ^ uVar7 & 0xffff) & manager->hash->bucketCount - 1U];
      pMVar5 != (McdModelPairManagerLink *)0x0; pMVar5 = pMVar5->next) {
    if (pMVar5->key == uVar8) goto LAB_00010454;
  }
  pMVar5 = (McdModelPairManagerLink *)0x0;
LAB_00010454:
  if (pMVar5 == (McdModelPairManagerLink *)0x0) {
    pMVar5 = (McdModelPairManagerLink *)(*_DAT_00011010)(manager->linkPool);
    if (pMVar5 == (McdModelPairManagerLink *)0x0) {
      (*manager->poolFullHandler)(m1,m2);
    }
    else {
      pMVar5->key = uVar8;
      pMVar6 = (McdModelPairID)(*_DAT_00011010)(manager->pairPool);
      pMVar5->pair = pMVar6;
      McdModelPairReset(pMVar6,m1,m2);
      pMVar5->pair->phase = kMcdFFStateHello;
                    /* Unresolved local var: int bucket@[???] */
      pMVar3 = manager->hash;
      uVar8 = (pMVar5->key >> 0x10 ^ pMVar5->key & 0xffffU) & pMVar3->bucketCount - 1U;
      pMVar5->next = pMVar3->bucket[uVar8];
      pMVar3->bucket[uVar8] = pMVar5;
      pMVar5->phasePrev = &manager->helloList;
      pMVar2 = (manager->helloList).phaseNext;
      pMVar5->phaseNext = pMVar2;
      pMVar2->phasePrev = pMVar5;
      pMVar5->phasePrev->phaseNext = pMVar5;
    }
  }
  else {
    if (pMVar5->pair->phase != kMcdFFStateGoodbye) {
      return 0;
    }
    pMVar5->pair->phase = kMcdFFStateStaying;
    pMVar2 = pMVar5->phaseNext;
    pMVar2->phasePrev = pMVar5->phasePrev;
    pMVar5->phasePrev->phaseNext = pMVar2;
  }
  return 1;
}


/* ==== McdModelPairManagerDeactivate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeBool McdModelPairManagerDeactivate
                 (McdModelPairManagerID manager,McdModelID_conflict m1,McdModelID_conflict m2)

{
  uint uVar1;
  McdModelPairManagerHash *pMVar2;
  McdModelPairID pMVar3;
  McdModelPairPhase MVar4;
  McdModelPairManagerLink *pMVar5;
  McdModelPairManagerLink *pMVar6;
  McdModelPairManagerLink **ppMVar7;
  uint uVar8;
  uint uVar9;
  McdModelPairManagerLink *pMVar10;
  uint uVar11;
  
                    /* Unresolved local var: MeU32 id1@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeU32 id2@[DW_OP_reg3(EBX)]
                       Unresolved local var: int key@[???]
                       Unresolved local var: McdModelPairManagerLink * link@[???]
                       Unresolved local var: McdModelPairManagerLink * link@[DW_OP_reg0(EAX)] */
  uVar11 = m1->mSpaceID;
  uVar1 = m2->mSpaceID;
                    /* Unresolved local var: MeU32 d@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeU32 o@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeU32 l@[???] */
  uVar8 = 0xffffffff - ((int)(uVar1 - uVar11) >> 0x1f);
  uVar9 = (uVar8 ^ 0xffffffff) & uVar11 | uVar8 & uVar1;
  uVar11 = (uVar8 & uVar11 | (uVar8 ^ 0xffffffff) & uVar1) << 0x10 | uVar9;
  pMVar2 = manager->hash;
  for (pMVar10 = pMVar2->bucket[((int)uVar11 >> 0x10 ^ uVar9 & 0xffff) & pMVar2->bucketCount - 1U];
      pMVar10 != (McdModelPairManagerLink *)0x0; pMVar10 = pMVar10->next) {
    if (pMVar10->key == uVar11) goto LAB_000105a2;
  }
  pMVar10 = (McdModelPairManagerLink *)0x0;
LAB_000105a2:
  if (pMVar10 != (McdModelPairManagerLink *)0x0) {
    pMVar3 = pMVar10->pair;
    MVar4 = pMVar3->phase;
    if (1 < MVar4 - kMcdFFStateHello) {
      return 0;
    }
    if (MVar4 == kMcdFFStateHello) {
                    /* Unresolved local var: int bucket@[???]
                       Unresolved local var: McdModelPairManagerLink * * linkptr@[DW_OP_reg2(EDX)]
                       Unresolved local var: McdModelPairManagerLink * link@[???] */
      ppMVar7 = pMVar2->bucket + (((int)uVar11 >> 0x10 ^ uVar9 & 0xffff) & pMVar2->bucketCount - 1U)
      ;
      for (pMVar5 = *ppMVar7; pMVar5 != (McdModelPairManagerLink *)0x0; pMVar5 = pMVar5->next) {
        if (pMVar5->key == uVar11) {
          *ppMVar7 = pMVar5->next;
          break;
        }
        ppMVar7 = &pMVar5->next;
      }
      pMVar5 = pMVar10->phaseNext;
      pMVar5->phasePrev = pMVar10->phasePrev;
      pMVar6 = pMVar10->phasePrev;
      pMVar6->phaseNext = pMVar5;
      (*_DAT_00011014)(manager->pairPool,pMVar10->pair,pMVar6,pMVar6);
      (*_DAT_00011014)(manager->linkPool,pMVar10);
    }
    else {
      pMVar5 = (manager->goodbyeList).phaseNext;
      pMVar10->phasePrev = &manager->goodbyeList;
      pMVar10->phaseNext = pMVar5;
      pMVar5->phasePrev = pMVar10;
      pMVar10->phasePrev->phaseNext = pMVar10;
      pMVar3->phase = kMcdFFStateGoodbye;
    }
  }
  return 1;
}


/* ==== McdModelPairManagerFlush ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdModelPairManagerFlush(McdModelPairManagerID manager)

{
  McdModelPairManagerLink *pMVar1;
  McdModelPairManagerLink *pMVar2;
  uint uVar3;
  McdModelPairManagerLink *pMVar4;
  McdModelPairManagerID pMVar5;
  McdModelPairManagerLink **ppMVar6;
  McdModelPairManagerLink *pMVar7;
  McdModelPairManagerLink *nextLink;
  
                    /* Unresolved local var: McdModelPairManagerLink * link@[DW_OP_reg6(ESI)] */
  pMVar1 = &manager->goodbyeList;
  pMVar7 = (manager->goodbyeList).phaseNext;
  do {
    if (pMVar7 == pMVar1) {
      (manager->goodbyeList).phaseNext = pMVar1;
      (manager->goodbyeList).phasePrev = pMVar1;
      for (pMVar5 = (McdModelPairManagerID)(manager->helloList).phaseNext; pMVar5 != manager;
          pMVar5 = (McdModelPairManagerID)(pMVar5->helloList).phaseNext) {
        ((pMVar5->helloList).pair)->phase = kMcdFFStateStaying;
      }
      (manager->helloList).phaseNext = &manager->helloList;
      (manager->helloList).phasePrev = &manager->helloList;
      return;
    }
    pMVar2 = pMVar7->phaseNext;
                    /* Unresolved local var: int bucket@[???]
                       Unresolved local var: McdModelPairManagerLink * * linkptr@[DW_OP_reg2(EDX)]
                       Unresolved local var: McdModelPairManagerLink * link@[???] */
    uVar3 = pMVar7->key;
    ppMVar6 = manager->hash->bucket +
              (((int)uVar3 >> 0x10 ^ uVar3 & 0xffff) & manager->hash->bucketCount - 1U);
    for (pMVar4 = *ppMVar6; pMVar4 != (McdModelPairManagerLink *)0x0; pMVar4 = pMVar4->next) {
      if (pMVar4->key == uVar3) {
        *ppMVar6 = pMVar4->next;
        break;
      }
      ppMVar6 = &pMVar4->next;
    }
    (*_DAT_00011014)(manager->pairPool,pMVar7->pair,pMVar4,pMVar4);
    (*_DAT_00011014)(manager->linkPool,pMVar7);
    pMVar7 = pMVar2;
  } while( true );
}


/* ==== McdModelPairManagerGetSize ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int McdModelPairManagerGetSize(McdModelPairManagerID manager)

{
  int iVar1;
  int iVar2;
  
  iVar1 = (*_DAT_00011018)(manager->pairPool);
  iVar2 = (*_DAT_0001101c)(manager->pairPool);
  return iVar1 + iVar2;
}


/* ==== McdModelPairManagerSetPoolFullHandler ==== */

void McdModelPairManagerSetPoolFullHandler
               (McdModelPairManagerID manager,McdSpacePoolErrorFnPtr handler)

{
  manager->poolFullHandler = handler;
  return;
}


/* ==== McdModelPairManagerGetPoolFullHandler ==== */

McdSpacePoolErrorFnPtr McdModelPairManagerGetPoolFullHandler(McdModelPairManagerID manager)

{
  return manager->poolFullHandler;
}


/* ==== McdModelPairManagerHashGetNext ==== */

McdModelPairManagerLink *
McdModelPairManagerHashGetNext(McdModelPairManagerHash *hash,McdSpacePairIterator *i)

{
  McdModelPairManagerLink *pMVar1;
  int iVar2;
  int iVar3;
  
                    /* Unresolved local var: McdModelPairManagerLink * link@[DW_OP_reg2(EDX)] */
  pMVar1 = i->ptr;
  if (pMVar1 == (McdModelPairManagerLink *)0x0) {
    iVar2 = i->count + 1;
    i->count = iVar2;
    iVar3 = hash->bucketCount;
    if (iVar2 < iVar3) {
      pMVar1 = hash->bucket[iVar2];
      while (pMVar1 == (McdModelPairManagerLink *)0x0) {
        iVar2 = i->count + 1;
        i->count = iVar2;
        iVar3 = hash->bucketCount;
        if (iVar3 <= iVar2) break;
        pMVar1 = hash->bucket[iVar2];
      }
    }
    if (i->count == iVar3) {
      return (McdModelPairManagerLink *)0x0;
    }
    pMVar1 = hash->bucket[i->count];
  }
  i->ptr = pMVar1->next;
  return pMVar1;
}


