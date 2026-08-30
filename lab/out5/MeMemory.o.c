/* ==== MallocCreate ==== */

/* WARNING: Unknown calling convention */

void * MallocCreate(size_t bytes)

{
  void *pvVar1;
  
  pvVar1 = malloc(bytes);
  return pvVar1;
}


/* ==== Calloc ==== */

/* WARNING: Unknown calling convention */

void * Calloc(size_t bytes)

{
  void *pvVar1;
  
  pvVar1 = calloc(1,bytes);
  return pvVar1;
}


/* ==== MallocDestroy ==== */

/* WARNING: Unknown calling convention */

void MallocDestroy(void *block)

{
  free(block);
  return;
}


/* ==== MallocResize ==== */

void * MallocResize(void *block,size_t bytes)

{
  void *pvVar1;
  
  pvVar1 = realloc(block,bytes);
  return pvVar1;
}


/* ==== MallocCreateAligned ==== */

void * MallocCreateAligned(size_t bytes,uint alignment)

{
  void *pvVar1;
  void *pvVar2;
  uint uVar3;
  
                    /* Unresolved local var: void * offsetStore@[???]
                       Unresolved local var: int extra@[???]
                       Unresolved local var: uint offset@[DW_OP_reg3(EBX)]
                       Unresolved local var: void * block@[DW_OP_reg6(ESI)] */
  pvVar1 = malloc(bytes + alignment + 4);
  pvVar2 = (void *)0x0;
  if (pvVar1 != (void *)0x0) {
    uVar3 = alignment - (uint)pvVar1 % alignment;
    uVar3 = uVar3 & (uVar3 == alignment) - 1;
    if ((alignment & 3) != 0) {
      MeFatalError(0,"MallocCreateAligned: Alignment must be a multiple of sizeof(unsigned int).");
    }
    for (; uVar3 < 4; uVar3 = uVar3 + alignment) {
    }
    pvVar2 = (void *)((int)pvVar1 + uVar3);
    *(uint *)((int)pvVar2 + -4) = uVar3;
  }
  return pvVar2;
}


/* ==== MallocDestroyAligned ==== */

void MallocDestroyAligned(void *block)

{
                    /* Unresolved local var: void * offsetStore@[???]
                       Unresolved local var: void * start@[???]
                       Unresolved local var: uint offset@[???]
                       Unresolved local var: int extra@[???] */
  free((void *)((int)block - *(int *)((int)block + -4)));
  return;
}


