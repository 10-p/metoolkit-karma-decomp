/* ==== MeSetInit ==== */

void MeSetInit(MeSet *s,MeDictNode *nodemem,int maxnode,MeDictCompareFn cmp)

{
  MePoolxInit(&s->nodepool,nodemem,0x18,maxnode);
  if (cmp == (MeDictCompareFn)0x0) {
    cmp = MeSetDefaultCompare;
  }
  MeDictInit(s,maxnode,cmp);
  MePoolxUseWithDict(&s->nodepool,s);
  s->last = (MeDictNode *)0x0;
  s->next = (MeDictNode *)0x0;
  return;
}


/* ==== MeSetSize ==== */

int MeSetSize(MeSet *s)

{
  return (s->dict).nodecount;
}


/* ==== MeSetContains ==== */

int MeSetContains(MeSet *s,void *item)

{
  void *pvVar1;
  
  pvVar1 = MeDictLookup(s,item);
  return (uint)(pvVar1 != (void *)0x0);
}


/* ==== MeSetAdd ==== */

int MeSetAdd(MeSet *s,void *item)

{
  void *pvVar1;
  MeDictNode *pMVar2;
  int iVar3;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg6(ESI)] */
  pvVar1 = MeDictLookup(s,item);
  iVar3 = 0;
  if (pvVar1 == (void *)0x0) {
    pMVar2 = (*(s->dict).allocnode)((s->dict).context);
    iVar3 = 0;
    if (pMVar2 != (MeDictNode *)0x0) {
      if (s->next == (MeDictNode *)0x0) {
        s->next = pMVar2;
      }
      if (s->last != (MeDictNode *)0x0) {
        s->last->data = pMVar2;
      }
      s->last = pMVar2;
      MeDictNodeInit(pMVar2,(void *)0x0);
      MeDictInsert(s,pMVar2,item);
      iVar3 = 1;
    }
  }
  return iVar3;
}


/* ==== MeSetRemove ==== */

int MeSetRemove(MeSet *s,void *item)

{
  void *pvVar1;
  
                    /* Unresolved local var: MeDictNode * dn@[DW_OP_reg0(EAX)] */
  pvVar1 = MeDictLookup(s,item);
  if (pvVar1 != (void *)0x0) {
    MeDictDeleteFree(s,pvVar1);
  }
  return (uint)(pvVar1 != (void *)0x0);
}


/* ==== MeSetPopFirst ==== */

void * MeSetPopFirst(MeSet *s)

{
  void *pvVar1;
  void *pvVar2;
  
                    /* Unresolved local var: void * item@[DW_OP_reg3(EBX)] */
  pvVar1 = MeDictFirst(s);
  pvVar2 = (void *)0x0;
  if (pvVar1 != (void *)0x0) {
    pvVar2 = *(void **)((int)pvVar1 + 0x10);
    MeDictDeleteFree(s,pvVar1);
  }
  return pvVar2;
}


/* ==== MeSetPop ==== */

void * MeSetPop(MeSet *s)

{
  MeDictNode *pMVar1;
  void *pvVar2;
  
                    /* Unresolved local var: void * item@[DW_OP_reg3(EBX)] */
  pvVar2 = (void *)0x0;
  pMVar1 = (s->dict).nilnode.left;
  if (pMVar1 != (MeDictNode *)0x0) {
    pvVar2 = pMVar1->key;
    MeDictDeleteFree(s,pMVar1);
  }
  return pvVar2;
}


/* ==== MeSetIteratorNext ==== */

void * MeSetIteratorNext(MeSet *s)

{
  MeDictNode *pMVar1;
  void *pvVar2;
  
                    /* Unresolved local var: void * k@[DW_OP_reg2(EDX)] */
  pvVar2 = (void *)0x0;
  pMVar1 = s->next;
  if (pMVar1 != (MeDictNode *)0x0) {
    pvVar2 = pMVar1->key;
    s->next = pMVar1->data;
  }
  return pvVar2;
}


/* ==== MeSetIsFull ==== */

int MeSetIsFull(MeSet *s)

{
  return (uint)((s->dict).nodecount == (s->dict).maxcount);
}


/* ==== MeSetCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeSet * MeSetCreate(MeDictCompareFn cmp)

{
  MeSet *pMVar1;
  
                    /* Unresolved local var: MeSet * s@[DW_OP_reg3(EBX)] */
  pMVar1 = (*_MeDictFreeNodes)(0x50);
  if (cmp == (MeDictCompareFn)0x0) {
    cmp = MeSetDefaultCompare;
  }
  MeDictInit(pMVar1,0x7fffffff,cmp);
  (pMVar1->nodepool).numrec = -999;
  return pMVar1;
}


/* ==== MeSetDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeSetDestroy(MeSet *s)

{
  if ((s->nodepool).numrec == -999) {
    MeDictFreeNodes(s);
    (*_DAT_0001102c)(s);
  }
  return;
}


/* ==== MeSetDefaultCompare ==== */

int MeSetDefaultCompare(void *item1,void *item2)

{
  return (int)item1 - (int)item2;
}


