/* ==== MePoolFixedInit ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MePoolFixedInit(MePool *u,int poolSize,int structSize,int alignment)

{
  void *pvVar1;
  void **ppvVar2;
  int iVar3;
  
                    /* Unresolved local var: MePoolFixed * pool@[DW_OP_reg3(EBX)]
                       Unresolved local var: int allocateSize@[DW_OP_reg7(EDI)] */
  iVar3 = 0;
  if (0 < poolSize) {
    if (alignment == 0) {
      pvVar1 = (void *)(*_MeMemoryAPI)(poolSize * structSize);
      (u->u).fixed.createdAligned = 0;
      (u->u).fixed.structArray = pvVar1;
    }
    else {
      iVar3 = structSize % alignment;
      if (iVar3 != 0) {
        structSize = (structSize - iVar3) + alignment;
      }
      pvVar1 = (void *)(*_MeWarning)(poolSize * structSize,alignment,iVar3,iVar3);
      (u->u).fixed.createdAligned = 1;
      (u->u).fixed.structArray = pvVar1;
    }
    ppvVar2 = (void **)(*_MeMemoryAPI)(poolSize << 2);
    (u->u).fixed.freeStructStack = ppvVar2;
    iVar3 = structSize;
  }
  (u->u).fixed.structSize = iVar3;
                    /* Unresolved local var: MePoolFixed * pool@[???]
                       Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: void * p@[DW_OP_reg2(EDX)] */
  iVar3 = 0;
  (u->u).fixed.poolSize = poolSize;
  u->t = MePoolFIXED;
  pvVar1 = (u->u).fixed.structArray;
  if (0 < (u->u).fixed.poolSize) {
    do {
      (u->u).fixed.freeStructStack[iVar3] = pvVar1;
      iVar3 = iVar3 + 1;
      pvVar1 = (void *)((int)pvVar1 + (u->u).fixed.structSize);
    } while (iVar3 < (u->u).fixed.poolSize);
  }
  (u->u).fixed.nextFreeStruct = 0;
  return;
}


/* ==== MePoolFixedDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MePoolFixedDestroy(MePool *u)

{
                    /* Unresolved local var: MePoolFixed * pool@[DW_OP_reg3(EBX)] */
  if (0 < (u->u).fixed.poolSize) {
    if ((u->u).fixed.createdAligned == 0) {
      (*_DAT_0001100c)((u->u).fixed.structArray);
    }
    else {
      (*_DAT_00011010)((u->u).fixed.structArray);
    }
    (*_DAT_0001100c)((u->u).fixed.freeStructStack);
  }
  return;
}


/* ==== MePoolFixedReset ==== */

void MePoolFixedReset(MePool *u)

{
  void *pvVar1;
  int iVar2;
  
                    /* Unresolved local var: MePoolFixed * pool@[DW_OP_reg2(EDX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: void * p@[DW_OP_reg1(ECX)] */
  pvVar1 = (u->u).fixed.structArray;
  iVar2 = 0;
  if (0 < (u->u).fixed.poolSize) {
    do {
      (u->u).fixed.freeStructStack[iVar2] = pvVar1;
      iVar2 = iVar2 + 1;
      pvVar1 = (void *)((int)pvVar1 + (u->u).fixed.structSize);
    } while (iVar2 < (u->u).fixed.poolSize);
  }
  return;
}


/* ==== MePoolFixedGetStruct ==== */

void * MePoolFixedGetStruct(MePool *u)

{
  int iVar1;
  void *pvVar2;
  
                    /* Unresolved local var: MePoolFixed * pool@[DW_OP_reg2(EDX)]
                       Unresolved local var: void * p@[DW_OP_reg3(EBX)] */
  iVar1 = (u->u).fixed.nextFreeStruct;
  if (iVar1 < (u->u).fixed.poolSize) {
    pvVar2 = (u->u).fixed.freeStructStack[iVar1];
    (u->u).fixed.nextFreeStruct = iVar1 + 1;
  }
  else {
    pvVar2 = (void *)0x0;
  }
  return pvVar2;
}


/* ==== MePoolFixedPutStruct ==== */

void MePoolFixedPutStruct(MePool *u,void *s)

{
  int iVar1;
  
                    /* Unresolved local var: MePoolFixed * pool@[DW_OP_reg0(EAX)] */
  iVar1 = (u->u).fixed.nextFreeStruct;
  if (0 < iVar1) {
    iVar1 = iVar1 + -1;
    (u->u).fixed.nextFreeStruct = iVar1;
    (u->u).fixed.freeStructStack[iVar1] = s;
  }
  return;
}


/* ==== MePoolFixedGetUsed ==== */

int MePoolFixedGetUsed(MePool *u)

{
                    /* Unresolved local var: MePoolFixed * pool@[DW_OP_reg0(EAX)] */
  return (u->u).fixed.nextFreeStruct;
}


/* ==== MePoolFixedGetUnused ==== */

int MePoolFixedGetUnused(MePool *u)

{
                    /* Unresolved local var: MePoolFixed * pool@[DW_OP_reg2(EDX)] */
  return (u->u).fixed.poolSize - (u->u).fixed.nextFreeStruct;
}


/* ==== MePoolMallocInit ==== */

void MePoolMallocInit(MePool *u,int poolSize,int structSize,int alignment)

{
                    /* Unresolved local var: MePoolMalloc * pool@[DW_OP_reg2(EDX)] */
  u->t = MePoolMALLOC;
  (u->u).fixed.structArray = (void *)0x0;
  (u->u).fixed.nextFreeStruct = poolSize;
  (u->u).malloc.structSize = structSize;
  (u->u).fixed.structSize = alignment;
  return;
}


/* ==== MePoolMallocDestroy ==== */

void MePoolMallocDestroy(MePool *u)

{
                    /* Unresolved local var: MePoolMalloc * pool@[???] */
  if ((u->u).fixed.structArray != (void *)0x0) {
    MeFatalError(0,"MePoolMallocDestroy(): %d structs still allocated");
  }
  return;
}


/* ==== MePoolMallocReset ==== */

void MePoolMallocReset(MePool *u)

{
                    /* Unresolved local var: MePoolMalloc * pool@[???] */
  MeWarning(1,&DAT_00010404);
  return;
}


/* ==== MePoolMallocGetStruct ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void * MePoolMallocGetStruct(MePool *u)

{
  int iVar1;
  void *pvVar2;
  void *pvVar3;
  
                    /* Unresolved local var: MePoolMalloc * pool@[DW_OP_reg3(EBX)] */
  if ((u->u).malloc.usedStructs < (u->u).fixed.nextFreeStruct) {
                    /* Unresolved local var: void * p@[DW_OP_reg0(EAX)] */
    iVar1 = (u->u).fixed.structSize;
    if (iVar1 == 0) {
      pvVar2 = (void *)(*_MeMemoryAPI)((u->u).fixed.freeStructStack);
    }
    else {
      pvVar2 = (void *)(*_MeWarning)((u->u).fixed.freeStructStack,iVar1);
    }
    pvVar3 = (void *)0x0;
    if (pvVar2 != (void *)0x0) {
      (u->u).fixed.structArray = (void *)((u->u).malloc.usedStructs + 1);
      pvVar3 = pvVar2;
    }
  }
  else {
    pvVar3 = (void *)0x0;
  }
  return pvVar3;
}


/* ==== MePoolMallocPutStruct ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MePoolMallocPutStruct(MePool *u,void *s)

{
  int iVar1;
  code *pcVar2;
  
                    /* Unresolved local var: MePoolMalloc * pool@[DW_OP_reg3(EBX)] */
  if (s != (void *)0x0) {
    pcVar2 = _DAT_00011010;
    if ((u->u).fixed.structSize == 0) {
      pcVar2 = _DAT_0001100c;
    }
    (*pcVar2)(s);
    iVar1 = (u->u).malloc.usedStructs;
    if (0 < iVar1) {
      (u->u).fixed.structArray = (void *)(iVar1 + -1);
    }
  }
  return;
}


/* ==== MePoolMallocGetUsed ==== */

int MePoolMallocGetUsed(MePool *u)

{
                    /* Unresolved local var: MePoolMalloc * pool@[???] */
  return (int)(u->u).fixed.structArray;
}


/* ==== MePoolMallocGetUnused ==== */

int MePoolMallocGetUnused(MePool *u)

{
                    /* Unresolved local var: MePoolMalloc * pool@[???] */
  return (u->u).fixed.nextFreeStruct - (u->u).malloc.usedStructs;
}


