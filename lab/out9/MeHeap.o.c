/* ==== MeHeapPop ==== */

void * MeHeapPop(MeHeap *h)

{
  void **ppvVar1;
  void *pvVar2;
  void *pvVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int p;
  void *result;
  
                    /* Unresolved local var: void * t@[DW_OP_reg1(ECX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  pvVar3 = (void *)0x0;
  if (0 < h->used) {
    ppvVar1 = h->mem;
    pvVar3 = ppvVar1[1];
    ppvVar1[1] = ppvVar1[h->used];
    iVar6 = 2;
    iVar5 = h->used + -1;
    h->used = iVar5;
    p = 1;
    if (1 < iVar5) {
      do {
        iVar5 = (*h->cmp)(h->mem[p],h->mem[iVar6]);
        if (iVar5 == 0) {
          iVar5 = iVar6;
          if ((iVar6 + 1 <= h->used) &&
             (iVar4 = (*h->cmp)(h->mem[iVar6 + 1],h->mem[iVar6]), iVar4 != 0)) {
            iVar5 = iVar6 + 1;
          }
        }
        else {
          if (h->used < iVar6 + 1) {
            return pvVar3;
          }
          iVar4 = (*h->cmp)(h->mem[p],h->mem[iVar6 + 1]);
          iVar5 = iVar6 + 1;
          if (iVar4 != 0) {
            return pvVar3;
          }
        }
        ppvVar1 = h->mem;
        pvVar2 = ppvVar1[iVar5];
        ppvVar1[iVar5] = ppvVar1[p];
        h->mem[p] = pvVar2;
        iVar6 = iVar5 * 2;
        p = iVar5;
      } while (iVar6 <= h->used);
    }
  }
  return pvVar3;
}


/* ==== MeHeapInit ==== */

void MeHeapInit(MeHeap *h,void **memory,int capacity,MeHeapComparisonFnPtr cmp)

{
  h->capacity = capacity;
  if (cmp == (MeHeapComparisonFnPtr)0x0) {
    cmp = MeHeapDefaultCompare;
  }
  h->cmp = cmp;
  h->mem = memory + -1;
  h->used = 0;
  return;
}


/* ==== MeHeapPush ==== */

int MeHeapPush(MeHeap *h,void *item)

{
  void **ppvVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
                    /* Unresolved local var: void * t@[DW_OP_reg1(ECX)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int p@[DW_OP_reg3(EBX)] */
  if (h->used < h->capacity) {
    iVar4 = h->used + 1;
    h->used = iVar4;
    h->mem[iVar4] = item;
    while ((iVar3 = iVar4 >> 1, iVar3 != 0 &&
           (iVar5 = (*h->cmp)(h->mem[iVar4],h->mem[iVar3]), iVar5 != 0))) {
      ppvVar1 = h->mem;
      pvVar2 = ppvVar1[iVar4];
      ppvVar1[iVar4] = ppvVar1[iVar3];
      h->mem[iVar3] = pvVar2;
      iVar4 = iVar3;
    }
    iVar4 = 1;
  }
  else {
    iVar4 = 0;
  }
  return iVar4;
}


/* ==== MeHeapCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeHeap * MeHeapCreate(int capacity,MeHeapComparisonFnPtr cmp)

{
  MeHeap *pMVar1;
  void *pvVar2;
  
                    /* Unresolved local var: MeHeap * h@[DW_OP_reg6(ESI)]
                       Unresolved local var: void * * m@[DW_OP_reg0(EAX)] */
  pMVar1 = (*_MeMemoryAPI)(0x10);
  pvVar2 = (*_MeMemoryAPI)(capacity * 4);
  pMVar1->capacity = capacity;
  if (cmp == (MeHeapComparisonFnPtr)0x0) {
    cmp = MeHeapDefaultCompare;
  }
  pMVar1->cmp = cmp;
  pMVar1->mem = (void **)((int)pvVar2 + -4);
  pMVar1->used = 0;
  return pMVar1;
}


/* ==== MeHeapDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeHeapDestroy(MeHeap *h)

{
  (*_DAT_0001100c)(h->mem + 1);
  (*_DAT_0001100c)(h);
  return;
}


/* ==== MeHeapDefaultCompare ==== */

int MeHeapDefaultCompare(void *item1,void *item2)

{
  return *(int *)item1 - *(int *)item2;
}


