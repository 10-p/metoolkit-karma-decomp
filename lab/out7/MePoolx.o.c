/* ==== MePoolxGetZeroed ==== */

void * MePoolxGetZeroed(MePoolx *p)

{
  int iVar1;
  int *__s;
  
                    /* Unresolved local var: void * r@[???] */
                    /* Unresolved local var: int * result@[DW_OP_reg6(ESI)] */
  __s = (int *)0x0;
  if (p->numfree != 0) {
    iVar1 = p->numfree + -1;
    __s = p->mem + p->ifree;
    p->numfree = iVar1;
    if (iVar1 != 0) {
      if (*__s == -1) {
        iVar1 = p->ifree + p->isize;
        p->ifree = iVar1;
        p->mem[iVar1] = -1;
      }
      else {
        p->ifree = *__s;
      }
    }
  }
  if (__s != (int *)0x0) {
                    /* Unresolved local var: void * __s@[???] */
    memset(__s,0,p->isize << 2);
  }
  return __s;
}


/* ==== MePoolxInit ==== */

void MePoolxInit(MePoolx *p,void *memory,int recsize,int numrec)

{
  p->numrec = numrec;
  p->mem = memory;
  p->isize = (uint)recsize >> 2;
  p->numfree = numrec;
  p->ifree = 0;
  *(undefined4 *)memory = 0xffffffff;
  return;
}


/* ==== MePoolxGet ==== */

void * MePoolxGet(MePoolx *p)

{
  int iVar1;
  int *piVar2;
  
                    /* Unresolved local var: int * result@[DW_OP_reg6(ESI)] */
  piVar2 = (int *)0x0;
  if (p->numfree != 0) {
    iVar1 = p->numfree + -1;
    piVar2 = p->mem + p->ifree;
    p->numfree = iVar1;
    if (iVar1 != 0) {
      if (*piVar2 == -1) {
        iVar1 = p->ifree + p->isize;
        p->ifree = iVar1;
        p->mem[iVar1] = -1;
      }
      else {
        p->ifree = *piVar2;
      }
    }
  }
  return piVar2;
}


/* ==== MePoolxPut ==== */

void MePoolxPut(MePoolx *p,void *rec)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[???] */
  iVar1 = (int)rec - (int)p->mem >> 2;
  p->mem[iVar1] = p->ifree;
  p->ifree = iVar1;
  p->numfree = p->numfree + 1;
  return;
}


/* ==== MePoolxUseWithDict ==== */

/* WARNING: Unknown calling convention */

void MePoolxUseWithDict(MePoolx *p,MeDict *d)

{
  MeDictSetAllocator(d,MePoolxDictNodeAllocate,MePoolxDictNodeDeallocate,p);
  return;
}


/* ==== MePoolxDictNodeAllocate ==== */

MeDictNode * MePoolxDictNodeAllocate(void *pool)

{
  int iVar1;
  MeDictNode *pMVar2;
  
                    /* Unresolved local var: int * result@[DW_OP_reg6(ESI)] */
  pMVar2 = (MeDictNode *)0x0;
  if (*(int *)((int)pool + 0xc) != 0) {
    iVar1 = *(int *)((int)pool + 0xc) + -1;
    pMVar2 = (MeDictNode *)(*(int *)pool + *(int *)((int)pool + 0x10) * 4);
    *(int *)((int)pool + 0xc) = iVar1;
    if (iVar1 != 0) {
      if (pMVar2->left == (MeDictNode *)0xffffffff) {
        iVar1 = *(int *)((int)pool + 0x10) + *(int *)((int)pool + 4);
        *(int *)((int)pool + 0x10) = iVar1;
        *(undefined4 *)(*(int *)pool + iVar1 * 4) = 0xffffffff;
      }
      else {
        *(MeDictNode **)((int)pool + 0x10) = pMVar2->left;
      }
    }
  }
  return pMVar2;
}


/* ==== MePoolxDictNodeDeallocate ==== */

void MePoolxDictNodeDeallocate(MeDictNode *node,void *pool)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[???] */
  iVar1 = (int)node - *(int *)pool >> 2;
  *(undefined4 *)(*(int *)pool + iVar1 * 4) = *(undefined4 *)((int)pool + 0x10);
  *(int *)((int)pool + 0x10) = iVar1;
  *(int *)((int)pool + 0xc) = *(int *)((int)pool + 0xc) + 1;
  return;
}


