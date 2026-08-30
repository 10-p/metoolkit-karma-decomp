/* ==== MeIDPoolRequestID ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int MeIDPoolRequestID(MeIDPool *pool)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  bool bVar4;
  MeU32 *pMVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  MeU32 MVar10;
  uint uVar11;
  uint uVar12;
  uint *p;
  MeU32 mask;
  uint found;
  
                    /* Unresolved local var: uint i@[DW_OP_reg1(ECX)]
                       Unresolved local var: uint n@[DW_OP_reg3(EBX)] */
  bVar4 = false;
  uVar2 = pool->maxIDs;
  if (pool->assignedIDs == uVar2) {
                    /* Unresolved local var: uint oldMaxBlocks@[DW_OP_reg6(ESI)] */
    uVar6 = pool->maxBlocks;
    uVar11 = uVar2 + 0x20;
    uVar8 = 1;
    pool->maxIDs = uVar11;
    if (0x1f < (int)uVar11) {
      uVar8 = uVar11;
      if ((int)uVar11 < 0) {
        uVar8 = uVar2 + 0x3f;
      }
      uVar11 = uVar11 + ((int)uVar8 >> 5) * -0x20;
      uVar8 = uVar11 + ((int)uVar8 >> 5);
    }
    pool->maxBlocks = uVar8;
    if (uVar6 < uVar8) {
                    /* Unresolved local var: MeU32 offset@[???] */
      pMVar5 = (MeU32 *)(*_DAT_00011014)(pool->IDbitfield,uVar8 << 2,uVar11,uVar11);
      uVar6 = uVar6 * 4;
      pool->IDbitfield = pMVar5;
      if ((int)uVar6 < 0) {
        uVar6 = uVar6 + 3;
      }
                    /* Unresolved local var: void * __s@[DW_OP_reg0(EAX)] */
      memset((void *)((int)pMVar5 + (uVar6 & 0xfffffffc)),0,4);
      pool->block = pool->block + 1;
    }
  }
  uVar2 = pool->maxIDs;
  do {
    mask = 0x80000000;
    uVar8 = pool->block;
    uVar11 = uVar8 * 0x20;
    puVar1 = pool->IDbitfield + uVar8;
    uVar6 = 0;
    MVar10 = mask;
    if (uVar11 < uVar2) {
      uVar3 = *puVar1;
      uVar9 = uVar2 & 3;
      if (uVar11 + 1 < uVar2) {
        uVar7 = uVar6;
        uVar12 = uVar11;
        if (uVar9 != 0) {
          if (uVar9 < 2) goto LAB_0001008f;
          if (uVar9 < 3) {
LAB_0001019b:
            MVar10 = mask;
            if ((mask & uVar3) != 0) {
              uVar6 = uVar6 + 1;
              mask = mask >> 1;
              uVar11 = uVar11 + 1;
              MVar10 = mask;
              if (0x1f < uVar6) goto LAB_00010111;
              goto LAB_0001008f;
            }
          }
          else if ((int)uVar3 < 0) {
            uVar6 = 1;
            mask = 0x40000000;
            uVar11 = uVar11 + 1;
            goto LAB_0001019b;
          }
          goto LAB_00010165;
        }
LAB_000100b0:
        while( true ) {
          uVar6 = uVar7;
          uVar11 = uVar12;
          MVar10 = mask;
          if ((mask & uVar3) == 0) goto LAB_00010165;
          uVar6 = uVar7 + 1;
          MVar10 = mask >> 1;
          uVar11 = uVar12 + 1;
          if (0x1f < uVar6) break;
          if ((MVar10 & uVar3) == 0) goto LAB_00010165;
          uVar6 = uVar7 + 2;
          MVar10 = mask >> 2;
          uVar11 = uVar12 + 2;
          if (0x1f < uVar6) break;
          if ((MVar10 & uVar3) == 0) goto LAB_00010165;
          uVar6 = uVar7 + 3;
          MVar10 = mask >> 3;
          uVar11 = uVar12 + 3;
          if (0x1f < uVar6) break;
          if ((MVar10 & uVar3) == 0) goto LAB_00010165;
          uVar6 = uVar7 + 4;
          mask = mask >> 4;
          uVar11 = uVar12 + 4;
          MVar10 = mask;
          if ((0x1f < uVar6) || (uVar7 = uVar6, uVar12 = uVar11, uVar2 <= uVar11)) break;
        }
      }
      else {
LAB_0001008f:
        MVar10 = mask;
        if ((mask & uVar3) == 0) {
LAB_00010165:
          mask = MVar10;
          bVar4 = true;
          MVar10 = mask;
        }
        else {
          uVar6 = uVar6 + 1;
          mask = mask >> 1;
          uVar11 = uVar11 + 1;
          MVar10 = mask;
          if ((uVar6 < 0x20) && (uVar7 = uVar6, uVar12 = uVar11, uVar11 < uVar2)) goto LAB_000100b0;
        }
      }
    }
LAB_00010111:
    mask = MVar10;
    if ((!bVar4) || (uVar6 == 0x1f)) {
      uVar8 = uVar8 + 1;
      pool->block = uVar8;
    }
    if (pool->maxBlocks <= uVar8) {
      pool->block = 0;
    }
    if (bVar4) {
      *puVar1 = *puVar1 | mask;
      pool->assignedIDs = pool->assignedIDs + 1;
      return uVar11;
    }
  } while( true );
}


/* ==== MeIDPoolReset ==== */

void MeIDPoolReset(MeIDPool *pool)

{
                    /* Unresolved local var: void * __s@[???] */
  memset(pool->IDbitfield,0,pool->maxBlocks << 2);
  pool->block = 0;
  pool->assignedIDs = 0;
  return;
}


/* ==== MeIDPoolCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeIDPool * MeIDPoolCreate(void)

{
  MeIDPool *pMVar1;
  MeU32 *pMVar2;
  
                    /* Unresolved local var: MeIDPool * pool@[DW_OP_reg3(EBX)] */
  pMVar1 = (MeIDPool *)(*_MeMemoryAPI)(0x14);
  pMVar1->maxIDs = 0x20;
  pMVar1->maxBlocks = 1;
  pMVar2 = (MeU32 *)(*_memset)(4);
  pMVar1->block = 0;
  pMVar1->IDbitfield = pMVar2;
  pMVar1->assignedIDs = 0;
  return pMVar1;
}


/* ==== MeIDPoolCopy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeIDPoolCopy(MeIDPool *to,MeIDPool *from)

{
  uint uVar1;
  MeU32 *pMVar2;
  uint uVar3;
  MeU32 *pMVar4;
  
  uVar3 = from->maxBlocks;
  uVar1 = from->block;
  to->maxIDs = from->maxIDs;
  to->maxBlocks = uVar3;
  to->assignedIDs = from->assignedIDs;
  to->block = uVar1;
  (*_DAT_0001100c)(to->IDbitfield);
  pMVar2 = (MeU32 *)(*_memset)(to->maxBlocks << 2);
  to->IDbitfield = pMVar2;
  pMVar4 = from->IDbitfield;
  for (uVar3 = to->maxBlocks & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pMVar2 = *pMVar4;
    pMVar4 = pMVar4 + 1;
    pMVar2 = pMVar2 + 1;
  }
  return;
}


/* ==== MeIDPoolIsEmpty ==== */

/* WARNING: Removing unreachable block (ram,0x00010352) */
/* WARNING: Removing unreachable block (ram,0x00010357) */
/* WARNING: Removing unreachable block (ram,0x000103d9) */
/* WARNING: Removing unreachable block (ram,0x0001035c) */
/* WARNING: Removing unreachable block (ram,0x0001036c) */
/* WARNING: Removing unreachable block (ram,0x0001037e) */

MeBool MeIDPoolIsEmpty(MeIDPool *pool)

{
  char *pcVar1;
  char *pcVar2;
  char *pcVar3;
  bool bVar4;
  MeU32 MVar5;
  int iVar6;
  MeBool MVar7;
  MeU32 *pMVar8;
  
                    /* Unresolved local var: char * p@[DW_OP_reg2(EDX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int empty@[DW_OP_reg1(ECX)] */
  bVar4 = true;
  pMVar8 = pool->IDbitfield;
  iVar6 = pool->maxBlocks * 4;
  if (0 < iVar6) {
    do {
      MVar5 = *pMVar8;
      pcVar1 = (char *)((int)pMVar8 + 1);
      pcVar2 = (char *)((int)pMVar8 + 2);
      pcVar3 = (char *)((int)pMVar8 + 3);
      pMVar8 = pMVar8 + 1;
      bVar4 = (bool)(bVar4 & (char)MVar5 == '\0' & *pcVar1 == '\0' & *pcVar2 == '\0' &
                    *pcVar3 == '\0');
      iVar6 = iVar6 + -4;
    } while (iVar6 != 0);
  }
  MVar7 = 0;
  if ((pool->assignedIDs == 0) && (bVar4)) {
    MVar7 = 1;
  }
  return MVar7;
}


/* ==== MeIDPoolReturnID ==== */

void MeIDPoolReturnID(MeIDPool *pool,int id)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  
                    /* Unresolved local var: int block@[DW_OP_reg0(EAX)]
                       Unresolved local var: int stride@[???]
                       Unresolved local var: MeU32 mask@[DW_OP_reg3(EBX)]
                       Unresolved local var: uint * p@[DW_OP_reg0(EAX)] */
  iVar2 = id;
  if (id < 0) {
    iVar2 = id + 0x1f;
  }
  uVar3 = 0x80000000 >> ((byte)id & 0x1f);
  uVar1 = pool->IDbitfield[iVar2 >> 5];
  if ((uVar3 & uVar1) != 0) {
    pool->IDbitfield[iVar2 >> 5] = uVar1 & (uVar3 ^ 0xffffffff);
    pool->assignedIDs = pool->assignedIDs - 1;
  }
  return;
}


/* ==== MeIDPoolDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeIDPoolDestroy(MeIDPool *pool)

{
  (*_DAT_0001100c)(pool->IDbitfield);
  (*_DAT_0001100c)(pool);
  return;
}


