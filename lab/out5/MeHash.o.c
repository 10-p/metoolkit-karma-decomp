/* ==== MeHashStringCompare ==== */

int MeHashStringCompare(void *k1,void *k2)

{
  int iVar1;
  
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar1 = strcmp(k1,k2);
  return iVar1;
}


/* ==== MeHashInsert ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void * MeHashInsert(void *key,void *datum,MeHash *table)

{
  int iVar1;
  int iVar2;
  MeHashBucket *pMVar3;
  void *pvVar4;
  MeHashBucket *pMVar5;
  int hashVal;
  
                    /* Unresolved local var: MeHashBucket * bucket@[DW_OP_reg3(EBX)]
                       Unresolved local var: int cmp@[DW_OP_reg0(EAX)] */
  iVar1 = (*table->hash)(key,table->size);
  pMVar3 = table->table[iVar1];
  while( true ) {
    if (pMVar3 == (MeHashBucket *)0x0) {
      pMVar3 = (MeHashBucket *)(*_MeMemoryAPI)(0xc);
      pvVar4 = (void *)0x0;
      if (pMVar3 != (MeHashBucket *)0x0) {
        pMVar3->key = key;
        pMVar3->datum = datum;
        pMVar3->next = (MeHashBucket *)0x0;
        table->table[iVar1] = pMVar3;
        table->population = table->population + 1;
        pvVar4 = datum;
      }
      return pvVar4;
    }
    iVar2 = (*table->compare)(key,pMVar3->key);
    if (iVar2 == 0) {
      return pMVar3->datum;
    }
    table->collisions = table->collisions + 1;
    if (iVar2 < 0) break;
    if ((pMVar3->next == (MeHashBucket *)0x0) ||
       (iVar2 = (*table->compare)(key,pMVar3->next->key), iVar2 < 0)) {
                    /* Unresolved local var: MeHashBucket * newBucket@[DW_OP_reg2(EDX)] */
      pMVar5 = (MeHashBucket *)(*_MeMemoryAPI)(0xc);
      if (pMVar5 == (MeHashBucket *)0x0) {
        return (void *)0x0;
      }
      pMVar5->key = key;
      pMVar5->next = pMVar3->next;
      pMVar5->datum = datum;
      pMVar3->next = pMVar5;
      goto LAB_000100dc;
    }
    pMVar3 = pMVar3->next;
  }
                    /* Unresolved local var: MeHashBucket * newBucket@[DW_OP_reg2(EDX)] */
  pMVar5 = (MeHashBucket *)(*_MeMemoryAPI)(0xc);
  if (pMVar5 == (MeHashBucket *)0x0) {
    return (void *)0x0;
  }
  pMVar5->key = key;
  pMVar5->next = pMVar3;
  pMVar5->datum = datum;
  table->table[iVar1] = pMVar5;
LAB_000100dc:
  table->population = table->population + 1;
  return datum;
}


/* ==== MeHashDelete ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void * MeHashDelete(void *key,MeHash *table)

{
  void *pvVar1;
  MeHashBucket *pMVar2;
  MeHashBucket *pMVar3;
  int iVar4;
  int iVar5;
  int hashVal;
  
                    /* Unresolved local var: MeHashBucket * bucket@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeHashBucket * last@[DW_OP_reg6(ESI)]
                       Unresolved local var: void * datum@[DW_OP_reg6(ESI)]
                       Unresolved local var: int cmp@[DW_OP_reg0(EAX)] */
  iVar4 = (*table->hash)(key,table->size);
  pMVar2 = (MeHashBucket *)0x0;
  pMVar3 = table->table[iVar4];
  while( true ) {
    if (pMVar3 == (MeHashBucket *)0x0) {
      return (void *)0x0;
    }
    iVar5 = (*table->compare)(key,pMVar3->key);
    if (iVar5 == 0) break;
    if (iVar5 < 0) {
      return (void *)0x0;
    }
    pMVar2 = pMVar3;
    pMVar3 = pMVar3->next;
  }
  if (pMVar2 == (MeHashBucket *)0x0) {
    table->table[iVar4] = pMVar3->next;
  }
  else {
    pMVar2->next = pMVar3->next;
  }
  pvVar1 = pMVar3->datum;
  if (table->freeDatum != (MeHashFreeFunc)0x0) {
    (*table->freeDatum)(pvVar1);
  }
  if (table->freeKey != (MeHashFreeFunc)0x0) {
    (*table->freeKey)(pMVar3->key);
  }
  (*_DAT_00011010)(pMVar3);
  table->population = table->population + -1;
  return pvVar1;
}


/* ==== MeHashGetDatum ==== */

void * MeHashGetDatum(MeHashIterator *i)

{
  MeHash *pMVar1;
  MeHashBucket *pMVar2;
  int iVar3;
  
  if (i->bucket == (MeHashBucket *)0x0) {
    pMVar1 = i->table;
    i->index = 0;
    if (0 < pMVar1->size) {
      do {
        pMVar2 = pMVar1->table[i->index];
        if (pMVar2 != (MeHashBucket *)0x0) goto LAB_000101e5;
        iVar3 = i->index + 1;
        i->index = iVar3;
      } while (iVar3 < pMVar1->size);
    }
  }
  else {
    pMVar2 = i->bucket->next;
    if (pMVar2 != (MeHashBucket *)0x0) {
LAB_000101e5:
      i->bucket = pMVar2;
      return pMVar2->datum;
    }
    pMVar1 = i->table;
    iVar3 = i->index + 1;
    i->index = iVar3;
    if (iVar3 < pMVar1->size) {
      do {
        pMVar2 = pMVar1->table[i->index];
        if (pMVar2 != (MeHashBucket *)0x0) goto LAB_000101e5;
        iVar3 = i->index + 1;
        i->index = iVar3;
      } while (iVar3 < pMVar1->size);
    }
  }
  i->index = 0;
  i->bucket = (MeHashBucket *)0x0;
  return (void *)0x0;
}


/* ==== MeHashString ==== */

int MeHashString(void *string,int size)

{
  char cVar1;
  uint uVar2;
  
                    /* Unresolved local var: uint ret_val@[DW_OP_reg3(EBX)]
                       Unresolved local var: char * s@[DW_OP_reg1(ECX)] */
  uVar2 = 0;
  cVar1 = *(char *)string;
  while (cVar1 != '\0') {
    string = (void *)((int)string + 1);
    uVar2 = (int)cVar1 + uVar2 * 0x21;
    cVar1 = *(char *)string;
  }
  return uVar2 % (uint)size;
}


/* ==== MeHashInt ==== */

int MeHashInt(void *i,int size)

{
  return (uint)i % (uint)size;
}


/* ==== MeHashIntCompare ==== */

int MeHashIntCompare(void *k1,void *k2)

{
  return (uint)(k1 != k2);
}


/* ==== MeHashCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeHash * MeHashCreate(int size)

{
  MeHash *pMVar1;
  MeHashBucket **ppMVar2;
  uint uVar3;
  
                    /* Unresolved local var: MeHash * table@[DW_OP_reg3(EBX)] */
  pMVar1 = (MeHash *)(*_MeMemoryAPI)(0x20);
  uVar3 = size - 1;
  pMVar1->population = 0;
  pMVar1->hash = MeHashString;
  pMVar1->compare = MeHashStringCompare;
  pMVar1->freeDatum = (MeHashFreeFunc)0x0;
  pMVar1->freeKey = (MeHashFreeFunc)0x0;
  pMVar1->collisions = 0;
  if (uVar3 < 0x3fffffff) {
    ppMVar2 = (MeHashBucket **)(*_MeMemoryAPI)(size * 4);
    pMVar1->table = ppMVar2;
    if (ppMVar2 != (MeHashBucket **)0x0) {
      pMVar1->size = size;
      if (size == 0) {
        return pMVar1;
      }
      for (; pMVar1->table[uVar3] = (MeHashBucket *)0x0, uVar3 != 0; uVar3 = uVar3 - 1) {
      }
      return pMVar1;
    }
  }
  pMVar1->size = 0;
  return (MeHash *)0x0;
}


/* ==== MeHashDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void MeHashDestroy(MeHash *table)

{
  int iVar1;
  MeHashBucket *pMVar2;
  MeHashBucket **ppMVar3;
  uint uVar4;
  
                    /* Unresolved local var: MeHashBucket * bucket@[???]
                       Unresolved local var: int size@[DW_OP_reg7(EDI)] */
  iVar1 = table->size;
  uVar4 = iVar1 - 1;
  if (uVar4 != 0xffffffff) {
    ppMVar3 = table->table;
    if (((uVar4 ^ 0xffffffff) & 1) != 0) {
      pMVar2 = ppMVar3[uVar4];
      while (pMVar2 != (MeHashBucket *)0x0) {
        ppMVar3[uVar4] = pMVar2->next;
        if (table->freeDatum != (MeHashFreeFunc)0x0) {
          (*table->freeDatum)(pMVar2->datum);
        }
        if (table->freeKey != (MeHashFreeFunc)0x0) {
          (*table->freeKey)(pMVar2->key);
        }
        (*_DAT_00011010)(pMVar2);
        ppMVar3 = table->table;
        pMVar2 = ppMVar3[uVar4];
      }
      uVar4 = iVar1 - 2;
      if (uVar4 == 0xffffffff) goto LAB_00010478;
    }
    do {
      pMVar2 = ppMVar3[uVar4];
      while (pMVar2 != (MeHashBucket *)0x0) {
        ppMVar3[uVar4] = pMVar2->next;
        if (table->freeDatum != (MeHashFreeFunc)0x0) {
          (*table->freeDatum)(pMVar2->datum);
        }
        if (table->freeKey != (MeHashFreeFunc)0x0) {
          (*table->freeKey)(pMVar2->key);
        }
        (*_DAT_00011010)(pMVar2);
        ppMVar3 = table->table;
        pMVar2 = ppMVar3[uVar4];
      }
      iVar1 = uVar4 - 1;
      pMVar2 = ppMVar3[iVar1];
      while (pMVar2 != (MeHashBucket *)0x0) {
        ppMVar3[iVar1] = pMVar2->next;
        if (table->freeDatum != (MeHashFreeFunc)0x0) {
          (*table->freeDatum)(pMVar2->datum);
        }
        if (table->freeKey != (MeHashFreeFunc)0x0) {
          (*table->freeKey)(pMVar2->key);
        }
        (*_DAT_00011010)(pMVar2);
        ppMVar3 = table->table;
        pMVar2 = ppMVar3[iVar1];
      }
      uVar4 = uVar4 - 2;
    } while (uVar4 != 0xffffffff);
  }
LAB_00010478:
  table->population = 0;
  (*_DAT_00011010)(table->table);
  (*_DAT_00011010)(table);
  return;
}


/* ==== MeHashReset ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void MeHashReset(MeHash *table)

{
  int iVar1;
  MeHashBucket *pMVar2;
  MeHashBucket **ppMVar3;
  uint uVar4;
  
                    /* Unresolved local var: MeHashBucket * bucket@[???]
                       Unresolved local var: int size@[DW_OP_reg7(EDI)] */
  iVar1 = table->size;
  uVar4 = iVar1 - 1;
  if (uVar4 != 0xffffffff) {
    ppMVar3 = table->table;
    if (((uVar4 ^ 0xffffffff) & 1) != 0) {
      pMVar2 = ppMVar3[uVar4];
      while (pMVar2 != (MeHashBucket *)0x0) {
        ppMVar3[uVar4] = pMVar2->next;
        if (table->freeDatum != (MeHashFreeFunc)0x0) {
          (*table->freeDatum)(pMVar2->datum);
        }
        if (table->freeKey != (MeHashFreeFunc)0x0) {
          (*table->freeKey)(pMVar2->key);
        }
        (*_DAT_00011010)(pMVar2);
        ppMVar3 = table->table;
        pMVar2 = ppMVar3[uVar4];
      }
      uVar4 = iVar1 - 2;
      if (uVar4 == 0xffffffff) goto LAB_00010618;
    }
    do {
      pMVar2 = ppMVar3[uVar4];
      while (pMVar2 != (MeHashBucket *)0x0) {
        ppMVar3[uVar4] = pMVar2->next;
        if (table->freeDatum != (MeHashFreeFunc)0x0) {
          (*table->freeDatum)(pMVar2->datum);
        }
        if (table->freeKey != (MeHashFreeFunc)0x0) {
          (*table->freeKey)(pMVar2->key);
        }
        (*_DAT_00011010)(pMVar2);
        ppMVar3 = table->table;
        pMVar2 = ppMVar3[uVar4];
      }
      iVar1 = uVar4 - 1;
      pMVar2 = ppMVar3[iVar1];
      while (pMVar2 != (MeHashBucket *)0x0) {
        ppMVar3[iVar1] = pMVar2->next;
        if (table->freeDatum != (MeHashFreeFunc)0x0) {
          (*table->freeDatum)(pMVar2->datum);
        }
        if (table->freeKey != (MeHashFreeFunc)0x0) {
          (*table->freeKey)(pMVar2->key);
        }
        (*_DAT_00011010)(pMVar2);
        ppMVar3 = table->table;
        pMVar2 = ppMVar3[iVar1];
      }
      uVar4 = uVar4 - 2;
    } while (uVar4 != 0xffffffff);
  }
LAB_00010618:
  table->population = 0;
  return;
}


/* ==== MeHashSetHashFunc ==== */

void MeHashSetHashFunc(MeHash *table,MeHashFunc hash)

{
  table->hash = hash;
  return;
}


/* ==== MeHashSetKeyCompareFunc ==== */

void MeHashSetKeyCompareFunc(MeHash *table,MeHashCompareFunc compare)

{
  table->compare = compare;
  return;
}


/* ==== MeHashSetKeyFreeFunc ==== */

void MeHashSetKeyFreeFunc(MeHash *table,MeHashFreeFunc freeKey)

{
  table->freeKey = freeKey;
  return;
}


/* ==== MeHashSetDatumFreeFunc ==== */

void MeHashSetDatumFreeFunc(MeHash *table,MeHashFreeFunc freeDatum)

{
  table->freeDatum = freeDatum;
  return;
}


/* ==== MeHashLookup ==== */

void * MeHashLookup(void *key,MeHash *table)

{
  MeHashBucket *pMVar1;
  int iVar2;
  
                    /* Unresolved local var: MeHashBucket * bucket@[DW_OP_reg3(EBX)]
                       Unresolved local var: int cmp@[DW_OP_reg0(EAX)] */
  iVar2 = (*table->hash)(key,table->size);
  pMVar1 = table->table[iVar2];
  while( true ) {
    if (pMVar1 == (MeHashBucket *)0x0) {
      return (void *)0x0;
    }
    iVar2 = (*table->compare)(key,pMVar1->key);
    if (iVar2 == 0) break;
    if (iVar2 < 0) {
      return (void *)0x0;
    }
    pMVar1 = pMVar1->next;
  }
  return pMVar1->datum;
}


/* ==== MeHashPopulation ==== */

int MeHashPopulation(MeHash *table)

{
  return table->population;
}


/* ==== MeHashInitIterator ==== */

MeHashIterator * MeHashInitIterator(MeHashIterator *i,MeHash *table)

{
  if (i != (MeHashIterator *)0x0) {
    i->bucket = (MeHashBucket *)0x0;
    i->index = 0;
    i->table = table;
  }
  return i;
}


