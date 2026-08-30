/* ==== McdModelPairContainerCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdModelPairContainer * McdModelPairContainerCreate(int size)

{
  McdModelPairContainer *pMVar1;
  McdModelPair **ppMVar2;
  
                    /* Unresolved local var: McdModelPairContainer * pairs@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdModelPair * * array@[DW_OP_reg0(EAX)] */
  pMVar1 = (*_MeMemoryAPI)(0x1c);
  ppMVar2 = (*_MeMemoryAPI)(size * 4);
  pMVar1->size = size;
  pMVar1->helloFirst = size;
  pMVar1->array = ppMVar2;
  pMVar1->helloEndStayingFirst = size;
  pMVar1->goodbyeFirst = 0;
  pMVar1->goodbyeEnd = 0;
  pMVar1->stayingEnd = size;
  return pMVar1;
}


/* ==== McdModelPairContainerInit ==== */

void McdModelPairContainerInit(McdModelPairContainer *a,McdModelPair **array,int size)

{
  a->array = array;
  a->size = size;
  a->goodbyeFirst = 0;
  a->goodbyeEnd = 0;
  a->helloFirst = size;
  a->helloEndStayingFirst = size;
  a->stayingEnd = size;
  return;
}


/* ==== McdModelPairContainerReset ==== */

void McdModelPairContainerReset(McdModelPairContainer *a)

{
  int iVar1;
  
  iVar1 = a->size;
  a->goodbyeFirst = 0;
  a->goodbyeEnd = 0;
  a->helloFirst = iVar1;
  a->helloEndStayingFirst = iVar1;
  a->stayingEnd = iVar1;
  return;
}


/* ==== McdModelPairContainerDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdModelPairContainerDestroy(McdModelPairContainer *a)

{
  (*_DAT_0001100c)(a->array);
  (*_DAT_0001100c)(a);
  return;
}


/* ==== McdModelPairContainerGetGoodbyeCount ==== */

int McdModelPairContainerGetGoodbyeCount(McdModelPairContainer *a)

{
  return a->goodbyeEnd;
}


/* ==== McdModelPairContainerGetHelloCount ==== */

int McdModelPairContainerGetHelloCount(McdModelPairContainer *a)

{
  return a->helloEndStayingFirst - a->helloFirst;
}


/* ==== McdModelPairContainerGetStayingCount ==== */

int McdModelPairContainerGetStayingCount(McdModelPairContainer *a)

{
  return a->stayingEnd - a->helloEndStayingFirst;
}


/* ==== McdModelPairContainerInsertHelloPair ==== */

MeBool McdModelPairContainerInsertHelloPair(McdModelPairContainer *m,McdModelPairID p)

{
  int iVar1;
  bool bVar2;
  
  iVar1 = m->helloFirst;
  bVar2 = m->goodbyeEnd != iVar1;
  if (bVar2) {
    m->helloFirst = iVar1 + -1;
    m->array[iVar1 + -1] = p;
  }
  return (uint)bVar2;
}


/* ==== McdModelPairContainerInsertGoodbyePair ==== */

MeBool McdModelPairContainerInsertGoodbyePair(McdModelPairContainer *m,McdModelPairID p)

{
  int iVar1;
  bool bVar2;
  
  iVar1 = m->goodbyeEnd;
  bVar2 = iVar1 != m->helloFirst;
  if (bVar2) {
    m->array[iVar1] = p;
    m->goodbyeEnd = m->goodbyeEnd + 1;
  }
  return (uint)bVar2;
}


/* ==== McdModelPairContainerInsertStayingPair ==== */

MeBool McdModelPairContainerInsertStayingPair(McdModelPairContainer *m,McdModelPairID p)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  
  iVar1 = m->helloFirst;
  bVar3 = m->goodbyeEnd != iVar1;
  if (bVar3) {
    iVar2 = m->helloEndStayingFirst + -1;
    m->helloFirst = iVar1 + -1;
    m->helloEndStayingFirst = iVar2;
    m->array[iVar1 + -1] = m->array[iVar2];
    m->array[m->helloEndStayingFirst] = p;
  }
  return (uint)bVar3;
}


/* ==== McdModelPairContainerRemovePair ==== */

void McdModelPairContainerRemovePair(McdModelPairContainer *m,McdModelPairID *p)

{
  McdModelPair **ppMVar1;
  int iVar2;
  int iVar3;
  
                    /* Unresolved local var: int i@[???] */
  ppMVar1 = m->array;
  iVar3 = (int)p - (int)ppMVar1 >> 2;
  if ((-1 < iVar3) || (iVar3 < m->size)) {
    if (iVar3 < m->goodbyeEnd) {
      iVar2 = m->goodbyeEnd + -1;
      m->goodbyeEnd = iVar2;
      ppMVar1[iVar3] = ppMVar1[iVar2];
    }
    else if (iVar3 < m->helloEndStayingFirst) {
      if (m->helloFirst <= iVar3) {
        ppMVar1[iVar3] = ppMVar1[m->helloFirst];
        m->helloFirst = m->helloFirst + 1;
      }
    }
    else {
      ppMVar1[iVar3] = ppMVar1[m->helloEndStayingFirst];
      m->array[m->helloEndStayingFirst] = m->array[m->helloFirst];
      m->helloFirst = m->helloFirst + 1;
      m->helloEndStayingFirst = m->helloEndStayingFirst + 1;
    }
  }
  return;
}


/* ==== McdModelPairContainerGetGoodbyeArray ==== */

McdModelPair ** McdModelPairContainerGetGoodbyeArray(McdModelPairContainer *a,int *count)

{
  *count = a->goodbyeEnd;
  return a->array;
}


/* ==== McdModelPairContainerGetHelloArray ==== */

McdModelPair ** McdModelPairContainerGetHelloArray(McdModelPairContainer *a,int *count)

{
  *count = a->helloEndStayingFirst - a->helloFirst;
  return a->array + a->helloFirst;
}


/* ==== McdModelPairContainerGetStayingArray ==== */

McdModelPair ** McdModelPairContainerGetStayingArray(McdModelPairContainer *a,int *count)

{
  *count = a->stayingEnd - a->helloEndStayingFirst;
  return a->array + a->helloEndStayingFirst;
}


/* ==== McdModelPairContainerIteratorInit ==== */

void McdModelPairContainerIteratorInit
               (McdModelPairContainerIterator *iter,McdModelPairContainer *pairs)

{
  iter->container = pairs;
  iter->count = pairs->helloEndStayingFirst;
  return;
}


