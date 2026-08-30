/* ==== keaPushPoolFrame ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void keaPushPoolFrame(void)

{
  *(undefined4 *)(&poolstack + _poolstack_ptr * 4) = _pool_ptr;
  _poolstack_ptr = _poolstack_ptr + 1;
  return;
}


/* ==== keaPopPoolFrame ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void keaPopPoolFrame(void)

{
  _pool_ptr = *(undefined4 *)(&pool_ptr + _poolstack_ptr * 4);
  _poolstack_ptr = _poolstack_ptr + -1;
  return;
}


/* ==== initPool ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: void initPool(keaFunctions * this, void * ptr, int size) */

void __thiscall keaFunctions::initPool(keaFunctions *this,void *ptr,int size)

{
  _poolstack_ptr = 0;
  _pool_ptr = ptr;
  _pool_max = (int)ptr + size;
  return;
}


/* ==== keaPoolAlloc ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void * keaPoolAlloc(int size,char *name)

{
  void *pvVar1;
  
                    /* Unresolved local var: void * addr@[DW_OP_reg0(EAX)] */
  if ((size & 0xfU) != 0) {
    MeFatalError(3,"keaPoolAlloc: allocating %s of size %d. Size must be a multiple of 16 bytes.\n",
                 name,size);
  }
  if (_pool_max < (uint)(size + (int)_pool_ptr)) {
    MeFatalError(3,
                 "Kea error:Memory pool size exceeded when allocating %s of size %d\nPool ends at %08x\nIf memory were allocated, pool would end at %08x"
                 ,name,size,_pool_max,size + (int)_pool_ptr,_pool_ptr,_pool_ptr);
  }
  pvVar1 = _pool_ptr;
  _pool_ptr = (void *)((int)_pool_ptr + size);
  return pvVar1;
}


/* ==== vanillaAllocateMemory ==== */

/* WARNING: Removing unreachable block (ram,0x0001051e) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void vanillaAllocateMemory(keaTempMemory *mem,MdtKeaConstraints constraints,int num_bodies)

{
  undefined1 *puVar1;
  int *piVar2;
  MdtKeaBl2BodyRow *paiVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  MdtKeaVelocity *pMVar7;
  MdtKeaJBlockPair *paMVar8;
  MeReal (*paMVar9) [4];
  MdtKeaBl2CBodyRow *paiVar10;
  int iVar11;
  int num_rows;
  int total_strips_exc_padding;
  int total_strips_inc_padding;
  
                    /* Unresolved local var: int partition@[DW_OP_reg3(EBX)] */
  iVar11 = 0;
  total_strips_inc_padding = 0;
  total_strips_exc_padding = 0;
  if (constraints.num_partitions != 0) {
    do {
      if (0 < constraints.num_constraints_partition[iVar11]) {
        uVar6 = constraints.num_rows_exc_padding_partition[iVar11];
        if ((int)uVar6 % 0xc == 0) {
          uVar5 = uVar6;
          if ((int)uVar6 < 0) {
            uVar5 = uVar6 + 3;
          }
        }
        else {
          iVar4 = uVar6 - (int)uVar6 % 0xc;
          uVar5 = iVar4 + 0xc;
          if ((int)uVar5 < 0) {
            uVar5 = iVar4 + 0xf;
          }
        }
        total_strips_inc_padding = total_strips_inc_padding + ((int)uVar5 >> 2);
        if ((uVar6 & 3) == 0) {
          if ((int)uVar6 < 0) {
            uVar6 = uVar6 + 3;
          }
        }
        else {
          if ((int)uVar6 < 0) {
            uVar6 = uVar6 + 3;
          }
          uVar5 = uVar6 & 0xfffffffc;
          uVar6 = uVar5 + 4;
          if ((int)uVar6 < 0) {
            uVar6 = uVar5 + 7;
          }
        }
        total_strips_exc_padding = total_strips_exc_padding + ((int)uVar6 >> 2);
      }
      iVar11 = iVar11 + 1;
    } while (iVar11 != constraints.num_partitions);
  }
                    /* Unresolved local var: void * addr@[DW_OP_reg0(EAX)] */
  uVar6 = num_bodies * 0x30;
  iVar11 = uVar6 + 0x40;
  if ((uVar6 & 0x3f) != 0) {
    iVar11 = (uVar6 - (uVar6 & 0x3f)) + 0x80;
  }
  puVar1 = (undefined1 *)((int)_pool_ptr->invI0 + iVar11);
  if (_pool_max < puVar1) {
    MeFatalError(3,
                 "Kea error:Memory pool size exceeded when allocating %s of size %d\nPool ends at %08x\nIf memory were allocated, pool would end at %08x"
                 ,"invIworld",iVar11,_pool_max,puVar1,constraints.num_partitions,
                 constraints.num_partitions);
  }
  pMVar7 = (MdtKeaVelocity *)((int)_pool_ptr->invI0 + iVar11);
  mem->invIworld = _pool_ptr;
  _pool_ptr = (MdtKeaInverseMassMatrix *)pMVar7;
                    /* Unresolved local var: void * addr@[DW_OP_reg0(EAX)] */
  uVar6 = num_bodies * 0x20;
  iVar11 = uVar6 + 0x40;
  if ((uVar6 & 0x3f) != 0) {
    iVar11 = (uVar6 - (uVar6 & 0x3f)) + 0x80;
  }
  puVar1 = (undefined1 *)((int)((MdtKeaVelocity *)_pool_ptr)->velocity + iVar11);
  pMVar7 = (MdtKeaVelocity *)_pool_ptr;
  if (_pool_max < puVar1) {
    MeFatalError(3,
                 "Kea error:Memory pool size exceeded when allocating %s of size %d\nPool ends at %08x\nIf memory were allocated, pool would end at %08x"
                 ,&DAT_0001104a,iVar11,_pool_max,puVar1,_pool_ptr,_pool_ptr);
    pMVar7 = (MdtKeaVelocity *)_pool_ptr;
  }
  _pool_ptr = (MdtKeaInverseMassMatrix *)((int)pMVar7->velocity + iVar11);
  mem->vhmf = pMVar7;
                    /* Unresolved local var: void * addr@[DW_OP_reg0(EAX)] */
  paMVar9 = (*(MdtKeaJBlockPair *)((int)_pool_ptr + total_strips_exc_padding * 5 * 0xc0))[0].col + 4
  ;
  paMVar8 = (MdtKeaJBlockPair *)_pool_ptr;
  if (_pool_max < paMVar9) {
    MeFatalError(3,
                 "Kea error:Memory pool size exceeded when allocating %s of size %d\nPool ends at %08x\nIf memory were allocated, pool would end at %08x"
                 ,&DAT_00011047,total_strips_exc_padding * 0x3c0 + 0x40,_pool_max,paMVar9,pMVar7,
                 pMVar7);
    paMVar8 = (MdtKeaJBlockPair *)_pool_ptr;
  }
  _pool_ptr = (MdtKeaInverseMassMatrix *)(paMVar8[total_strips_exc_padding * 5][0].col + 4);
  mem->jm = paMVar8;
                    /* Unresolved local var: void * addr@[DW_OP_reg0(EAX)] */
  uVar6 = total_strips_exc_padding * 0x10;
  iVar11 = uVar6 + 0x40;
  if ((uVar6 & 0x3f) != 0) {
    iVar11 = (uVar6 - (uVar6 & 0x3f)) + 0x80;
  }
  puVar1 = (undefined1 *)((int)_pool_ptr->invI0[0] + iVar11);
  paMVar9 = _pool_ptr->invI0;
  if (_pool_max < puVar1) {
    MeFatalError(3,
                 "Kea error:Memory pool size exceeded when allocating %s of size %d\nPool ends at %08x\nIf memory were allocated, pool would end at %08x"
                 ,&DAT_00011043,iVar11,_pool_max,puVar1,_pool_ptr,_pool_ptr);
    paMVar9 = _pool_ptr->invI0;
  }
  _pool_ptr = (MdtKeaInverseMassMatrix *)((int)*paMVar9 + iVar11);
  mem->rhs = *paMVar9;
                    /* Unresolved local var: void * addr@[DW_OP_reg0(EAX)] */
  uVar5 = total_strips_inc_padding * 4;
  uVar6 = uVar5 + 0x40;
  if ((uVar5 & 0x3f) != 0) {
    uVar6 = (uVar5 - (uVar5 & 0x3f)) + 0x80;
  }
  if ((uVar6 & 0xc) != 0) {
    MeFatalError(3,"keaPoolAlloc: allocating %s of size %d. Size must be a multiple of 16 bytes.\n",
                 "jlen_12padded",uVar6);
  }
  if (_pool_max < (undefined1 *)(uVar6 + (int)_pool_ptr)) {
    MeFatalError(3,
                 "Kea error:Memory pool size exceeded when allocating %s of size %d\nPool ends at %08x\nIf memory were allocated, pool would end at %08x"
                 ,"jlen_12padded",uVar6,_pool_max,(undefined1 *)(uVar6 + (int)_pool_ptr),paMVar8,
                 paMVar8);
  }
  piVar2 = (int *)((int)_pool_ptr + uVar6);
  mem->jlen_12padded = (int *)_pool_ptr;
  _pool_ptr = (MdtKeaInverseMassMatrix *)piVar2;
                    /* Unresolved local var: void * addr@[DW_OP_reg0(EAX)] */
  uVar5 = total_strips_exc_padding * 4;
  uVar6 = uVar5 + 0x40;
  if ((uVar5 & 0x3f) != 0) {
    uVar6 = (uVar5 - (uVar5 & 0x3f)) + 0x80;
  }
  if ((uVar6 & 0xc) != 0) {
    MeFatalError(3,"keaPoolAlloc: allocating %s of size %d. Size must be a multiple of 16 bytes.\n",
                 &DAT_00011030,uVar6);
  }
  if (_pool_max < (undefined1 *)(uVar6 + (int)_pool_ptr)) {
    MeFatalError(3,
                 "Kea error:Memory pool size exceeded when allocating %s of size %d\nPool ends at %08x\nIf memory were allocated, pool would end at %08x"
                 ,&DAT_00011030,uVar6,_pool_max,(undefined1 *)(uVar6 + (int)_pool_ptr),paMVar9,
                 paMVar9);
  }
  paiVar10 = (MdtKeaBl2CBodyRow *)((int)_pool_ptr + uVar6);
  mem->jlen = (int *)_pool_ptr;
  _pool_ptr = (MdtKeaInverseMassMatrix *)paiVar10;
                    /* Unresolved local var: void * addr@[DW_OP_reg0(EAX)] */
  uVar6 = total_strips_exc_padding * 0x20;
  iVar11 = uVar6 + 0x40;
  if ((uVar6 & 0x3f) != 0) {
    iVar11 = (uVar6 - (uVar6 & 0x3f)) + 0x80;
  }
  puVar1 = (undefined1 *)((int)*(MdtKeaBl2CBodyRow *)_pool_ptr + iVar11);
  paiVar10 = (MdtKeaBl2CBodyRow *)_pool_ptr;
  if (_pool_max < puVar1) {
    MeFatalError(3,
                 "Kea error:Memory pool size exceeded when allocating %s of size %d\nPool ends at %08x\nIf memory were allocated, pool would end at %08x"
                 ,"bl2cbody",iVar11,_pool_max,puVar1,_pool_ptr,_pool_ptr);
    paiVar10 = (MdtKeaBl2CBodyRow *)_pool_ptr;
  }
  _pool_ptr = (MdtKeaInverseMassMatrix *)((int)*paiVar10 + iVar11);
  mem->bl2cbody = paiVar10;
                    /* Unresolved local var: void * addr@[DW_OP_reg0(EAX)] */
  uVar6 = total_strips_inc_padding * 0x20;
  iVar11 = uVar6 + 0x40;
  if ((uVar6 & 0x3f) != 0) {
    iVar11 = (uVar6 - (uVar6 & 0x3f)) + 0x80;
  }
  puVar1 = (undefined1 *)((int)*(MdtKeaBl2BodyRow *)_pool_ptr + iVar11);
  if (_pool_max < puVar1) {
    MeFatalError(3,
                 "Kea error:Memory pool size exceeded when allocating %s of size %d\nPool ends at %08x\nIf memory were allocated, pool would end at %08x"
                 ,"bl2body_12padded",iVar11,_pool_max,puVar1,paiVar10,paiVar10);
  }
  paiVar3 = (MdtKeaBl2BodyRow *)((int)*(MdtKeaBl2BodyRow *)_pool_ptr + iVar11);
  mem->bl2body_12padded = (MdtKeaBl2BodyRow *)_pool_ptr;
  _pool_ptr = (MdtKeaInverseMassMatrix *)paiVar3;
                    /* Unresolved local var: void * addr@[DW_OP_reg0(EAX)] */
  uVar6 = total_strips_exc_padding * 0x20;
  iVar11 = uVar6 + 0x40;
  if ((uVar6 & 0x3f) != 0) {
    iVar11 = (uVar6 - (uVar6 & 0x3f)) + 0x80;
  }
  puVar1 = (undefined1 *)((int)*(MdtKeaBl2BodyRow *)_pool_ptr + iVar11);
  if (_pool_max < puVar1) {
    MeFatalError(3,
                 "Kea error:Memory pool size exceeded when allocating %s of size %d\nPool ends at %08x\nIf memory were allocated, pool would end at %08x"
                 ,"bl2body",iVar11,_pool_max,puVar1,mem,mem);
  }
  iVar11 = (int)*(MdtKeaBl2BodyRow *)_pool_ptr + iVar11;
  mem->bl2body = (MdtKeaBl2BodyRow *)_pool_ptr;
  _pool_ptr = (MdtKeaInverseMassMatrix *)iVar11;
  return;
}


/* ==== allocateMemory ==== */

/* DWARF original prototype: void allocateMemory(keaFunctions_Vanilla * this, keaTempMemory * mem,
   MdtKeaConstraints constraints, int num_bodies) */

void __thiscall
keaFunctions_Vanilla::allocateMemory
          (keaFunctions_Vanilla *this,keaTempMemory *mem,MdtKeaConstraints constraints,
          int num_bodies)

{
  int iVar1;
  MdtKeaConstraints *pMVar2;
  int *piVar3;
  MdtKeaConstraints in_stack_ffffff88;
  
  pMVar2 = &constraints;
  piVar3 = (int *)&stack0xffffff88;
  for (iVar1 = 0x17; iVar1 != 0; iVar1 = iVar1 + -1) {
    *piVar3 = pMVar2->num_partitions;
    pMVar2 = (MdtKeaConstraints *)&pMVar2->max_partitions;
    piVar3 = piVar3 + 1;
  }
  vanillaAllocateMemory(mem,in_stack_ffffff88,num_bodies);
  return;
}


/* ==== MdtKeaMemoryRequired ==== */

int MdtKeaMemoryRequired(int *num_rows_exc_padding,int num_partitions,int max_rows,int max_bodies)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  int iVar19;
  int iVar20;
  int iVar21;
  int iVar22;
  int iVar23;
  int iVar24;
  uint uVar25;
  uint uVar26;
  uint uVar27;
  int iVar28;
  int iVar29;
  int iVar30;
  int QNC;
  int QrsD;
  int Qrhs;
  int Ainv;
  int clampedValues;
  int initialSolve;
  int lower;
  int upper;
  int w;
  int x;
  int cached;
  int ANC;
  int ANR;
  int ANCZ;
  int ANAZ;
  int A;
  int Arhs;
  int ArsD;
  int bl2body;
  int bl2body_12padded;
  int bl2cbody;
  int jlen;
  int jlen_12padded;
  int rhs;
  int jm;
  int vhmf;
  int invIworld;
  int c16c12size;
  int c4size;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int total_strips_inc_padding@[DW_OP_reg7(EDI)]
                       Unresolved local var: int total_strips_exc_padding@[DW_OP_reg6(ESI)]
                       Unresolved local var: int AChol@[???]
                       Unresolved local var: int Q@[DW_OP_reg6(ESI)]
                       Unresolved local var: int QChol@[???]
                       Unresolved local var: int QNAZ@[DW_OP_reg7(EDI)]
                       Unresolved local var: int QNCZ@[DW_OP_reg3(EBX)]
                       Unresolved local var: int QNR@[DW_OP_reg1(ECX)]
                       Unresolved local var: int total@[DW_OP_reg0(EAX)] */
  if ((max_rows & 3U) == 0) {
    c4size = max_rows;
  }
  else {
    uVar27 = max_rows;
    if (max_rows < 0) {
      uVar27 = max_rows + 3;
    }
    c4size = (uVar27 & 0xfffffffc) + 4;
  }
  uVar27 = max_rows;
  if (max_rows % 0xc != 0) {
    uVar27 = (max_rows - max_rows % 0xc) + 0xc;
  }
  if ((uVar27 & 0xf) == 0) {
    c16c12size = max_rows;
    if (max_rows % 0xc != 0) {
      c16c12size = (max_rows - max_rows % 0xc) + 0xc;
    }
  }
  else {
    iVar29 = max_rows;
    if (max_rows % 0xc != 0) {
      iVar29 = (max_rows - max_rows % 0xc) + 0xc;
    }
    if (max_rows % 0xc == 0) {
      iVar28 = max_rows % 0x10;
    }
    else {
      iVar28 = max_rows - max_rows % 0xc;
      uVar27 = iVar28 + 0xc;
      uVar25 = uVar27;
      if ((int)uVar27 < 0) {
        uVar25 = iVar28 + 0x1b;
      }
      iVar28 = uVar27 - (uVar25 & 0xfffffff0);
    }
    c16c12size = (iVar29 - iVar28) + 0x10;
  }
  iVar29 = 0;
  iVar30 = 0;
  iVar28 = 0;
  if (num_partitions != 0) {
    if ((num_partitions & 1U) != 0) {
                    /* Unresolved local var: int num_rows@[???] */
      uVar27 = *num_rows_exc_padding;
      if ((int)uVar27 % 0xc == 0) {
        uVar25 = uVar27;
        if ((int)uVar27 < 0) {
          uVar25 = uVar27 + 3;
        }
      }
      else {
        iVar29 = uVar27 - (int)uVar27 % 0xc;
        uVar25 = iVar29 + 0xc;
        if ((int)uVar25 < 0) {
          uVar25 = iVar29 + 0xf;
        }
      }
      iVar30 = (int)uVar25 >> 2;
      if ((uVar27 & 3) == 0) {
        if ((int)uVar27 < 0) {
          uVar27 = uVar27 + 3;
        }
      }
      else {
        if ((int)uVar27 < 0) {
          uVar27 = uVar27 + 3;
        }
        uVar25 = uVar27 & 0xfffffffc;
        uVar27 = uVar25 + 4;
        if ((int)uVar27 < 0) {
          uVar27 = uVar25 + 7;
        }
      }
      iVar29 = (int)uVar27 >> 2;
      iVar28 = 1;
      if (num_partitions == 1) goto LAB_000107e3;
    }
    do {
      uVar27 = num_rows_exc_padding[iVar28];
      if ((int)uVar27 % 0xc == 0) {
        uVar25 = uVar27;
        if ((int)uVar27 < 0) {
          uVar25 = uVar27 + 3;
        }
      }
      else {
        iVar4 = uVar27 - (int)uVar27 % 0xc;
        uVar25 = iVar4 + 0xc;
        if ((int)uVar25 < 0) {
          uVar25 = iVar4 + 0xf;
        }
      }
      if ((uVar27 & 3) == 0) {
        if ((int)uVar27 < 0) {
          uVar27 = uVar27 + 3;
        }
      }
      else {
        if ((int)uVar27 < 0) {
          uVar27 = uVar27 + 3;
        }
        uVar5 = uVar27 & 0xfffffffc;
        uVar27 = uVar5 + 4;
        if ((int)uVar27 < 0) {
          uVar27 = uVar5 + 7;
        }
      }
      uVar5 = num_rows_exc_padding[iVar28 + 1];
      if ((int)uVar5 % 0xc == 0) {
        uVar26 = uVar5;
        if ((int)uVar5 < 0) {
          uVar26 = uVar5 + 3;
        }
      }
      else {
        iVar4 = uVar5 - (int)uVar5 % 0xc;
        uVar26 = iVar4 + 0xc;
        if ((int)uVar26 < 0) {
          uVar26 = iVar4 + 0xf;
        }
      }
      iVar30 = iVar30 + ((int)uVar25 >> 2) + ((int)uVar26 >> 2);
      if ((uVar5 & 3) == 0) {
        if ((int)uVar5 < 0) {
          uVar5 = uVar5 + 3;
        }
      }
      else {
        if ((int)uVar5 < 0) {
          uVar5 = uVar5 + 3;
        }
        uVar25 = uVar5 & 0xfffffffc;
        uVar5 = uVar25 + 4;
        if ((int)uVar5 < 0) {
          uVar5 = uVar25 + 7;
        }
      }
      iVar28 = iVar28 + 2;
      iVar29 = iVar29 + ((int)uVar27 >> 2) + ((int)uVar5 >> 2);
    } while (iVar28 != num_partitions);
  }
LAB_000107e3:
  uVar27 = max_bodies * 0x30;
  if ((uVar27 & 0x3f) == 0) {
    iVar28 = uVar27 + 0x40;
  }
  else {
    iVar28 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = max_bodies * 0x20;
  if ((uVar27 & 0x3f) == 0) {
    iVar4 = uVar27 + 0x40;
  }
  else {
    iVar4 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = iVar29 * 0x10;
  if ((uVar27 & 0x3f) == 0) {
    iVar6 = uVar27 + 0x40;
  }
  else {
    iVar6 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = iVar30 * 4;
  if ((uVar27 & 0x3f) == 0) {
    iVar7 = uVar27 + 0x40;
  }
  else {
    iVar7 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = iVar29 * 4;
  if ((uVar27 & 0x3f) == 0) {
    iVar8 = uVar27 + 0x40;
  }
  else {
    iVar8 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = iVar29 * 0x20;
  if ((uVar27 & 0x3f) == 0) {
    iVar9 = uVar27 + 0x40;
  }
  else {
    iVar9 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = iVar30 * 0x20;
  if ((uVar27 & 0x3f) == 0) {
    iVar30 = uVar27 + 0x40;
  }
  else {
    iVar30 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = iVar29 * 0x20;
  if ((uVar27 & 0x3f) == 0) {
    iVar10 = uVar27 + 0x40;
  }
  else {
    iVar10 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = c4size * 4;
  if ((uVar27 & 0xf) == 0) {
    iVar11 = uVar27 + 0x10;
  }
  else {
    iVar11 = (uVar27 - (uVar27 & 0xf)) + 0x20;
  }
  iVar1 = c4size * 4;
  iVar12 = c4size * c4size;
  iVar2 = iVar12 * 4;
  if (iVar12 < 0) {
    iVar12 = iVar12 + 0xf;
  }
  uVar27 = iVar12 >> 4;
  if ((uVar27 & 0xf) == 0) {
    ANAZ = uVar27 << 2;
  }
  else {
    if ((int)uVar27 < 0) {
      uVar27 = uVar27 + 0xf;
    }
    ANAZ = (uVar27 & 0xfffffff0) * 4 + 0x40;
  }
  iVar12 = c4size * c4size;
  if (iVar12 < 0) {
    iVar12 = iVar12 + 0xf;
  }
  uVar27 = iVar12 >> 4;
  if ((uVar27 & 0xf) == 0) {
    ANCZ = uVar27 << 2;
  }
  else {
    if ((int)uVar27 < 0) {
      uVar27 = uVar27 + 0xf;
    }
    ANCZ = (uVar27 & 0xfffffff0) * 4 + 0x40;
  }
  iVar12 = c4size;
  if (c4size < 0) {
    iVar12 = c4size + 3;
  }
  uVar27 = iVar12 >> 2;
  if ((uVar27 & 0xf) == 0) {
    ANR = uVar27 << 2;
  }
  else {
    if ((int)uVar27 < 0) {
      uVar27 = uVar27 + 0xf;
    }
    ANR = (uVar27 & 0xfffffff0) * 4 + 0x40;
  }
  iVar12 = c4size;
  if (c4size < 0) {
    iVar12 = c4size + 3;
  }
  uVar27 = iVar12 >> 2;
  if ((uVar27 & 0xf) == 0) {
    ANC = uVar27 << 2;
  }
  else {
    if ((int)uVar27 < 0) {
      uVar27 = uVar27 + 0xf;
    }
    ANC = (uVar27 & 0xfffffff0) * 4 + 0x40;
  }
  uVar27 = c16c12size * 4;
  if ((uVar27 & 0x3f) == 0) {
    iVar12 = uVar27 + 0x40;
  }
  else {
    iVar12 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = c16c12size * 4;
  if ((uVar27 & 0x3f) == 0) {
    iVar13 = uVar27 + 0x40;
  }
  else {
    iVar13 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = c16c12size * 4;
  if ((uVar27 & 0x3f) == 0) {
    iVar14 = uVar27 + 0x40;
  }
  else {
    iVar14 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = c16c12size * 4;
  if ((uVar27 & 0x3f) == 0) {
    iVar15 = uVar27 + 0x40;
  }
  else {
    iVar15 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = c16c12size * 4;
  if ((uVar27 & 0x3f) == 0) {
    iVar16 = uVar27 + 0x40;
  }
  else {
    iVar16 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = c16c12size * 4;
  if ((uVar27 & 0x3f) == 0) {
    iVar17 = uVar27 + 0x40;
  }
  else {
    iVar17 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = c16c12size * 4;
  if ((uVar27 & 0x3f) == 0) {
    iVar18 = uVar27 + 0x40;
  }
  else {
    iVar18 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = c16c12size * c16c12size * 4;
  if ((uVar27 & 0x3f) == 0) {
    iVar19 = uVar27 + 0x40;
  }
  else {
    iVar19 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = c16c12size * 4;
  if ((uVar27 & 0x3f) == 0) {
    iVar20 = uVar27 + 0x40;
  }
  else {
    iVar20 = (uVar27 - (uVar27 & 0x3f)) + 0x80;
  }
  uVar27 = c4size * 4;
  if ((uVar27 & 0xf) == 0) {
    iVar21 = uVar27 + 0x10;
  }
  else {
    iVar21 = (uVar27 - (uVar27 & 0xf)) + 0x20;
  }
  iVar22 = c4size * c4size;
  iVar3 = iVar22 * 4;
  if (iVar22 < 0) {
    iVar22 = iVar22 + 0xf;
  }
  uVar27 = iVar22 >> 4;
  if ((uVar27 & 0xf) == 0) {
    iVar22 = uVar27 * 4;
  }
  else {
    if ((int)uVar27 < 0) {
      uVar27 = uVar27 + 0xf;
    }
    iVar22 = (uVar27 & 0xfffffff0) * 4 + 0x40;
  }
  iVar23 = c4size * c4size;
  if (iVar23 < 0) {
    iVar23 = iVar23 + 0xf;
  }
  uVar27 = iVar23 >> 4;
  if ((uVar27 & 0xf) == 0) {
    iVar23 = uVar27 * 4;
  }
  else {
    if ((int)uVar27 < 0) {
      uVar27 = uVar27 + 0xf;
    }
    iVar23 = (uVar27 & 0xfffffff0) * 4 + 0x40;
  }
  iVar24 = c4size;
  if (c4size < 0) {
    iVar24 = c4size + 3;
  }
  uVar27 = iVar24 >> 2;
  if ((uVar27 & 0xf) == 0) {
    iVar24 = uVar27 * 4;
  }
  else {
    if ((int)uVar27 < 0) {
      uVar27 = uVar27 + 0xf;
    }
    iVar24 = (uVar27 & 0xfffffff0) * 4 + 0x40;
  }
  if (c4size < 0) {
    c4size = c4size + 3;
  }
  uVar27 = c4size >> 2;
  if ((uVar27 & 0xf) == 0) {
    QNC = uVar27 << 2;
  }
  else {
    if ((int)uVar27 < 0) {
      uVar27 = uVar27 + 0xf;
    }
    QNC = (uVar27 & 0xfffffff0) * 4 + 0x40;
  }
  return iVar28 + iVar4 + iVar29 * 0x3c0 + iVar6 + iVar7 + iVar8 + iVar9 + iVar30 + iVar10 + iVar11
         + iVar1 + 0x50 + (iVar2 + 0x10) * 2 + ANAZ + ANCZ + ANR + ANC + iVar12 + iVar13 + iVar14 +
         iVar15 + iVar16 + iVar17 + iVar18 + iVar19 + iVar20 + iVar21 + (iVar3 + 0x10) * 2 + iVar22
         + iVar23 + iVar24 + QNC;
}


