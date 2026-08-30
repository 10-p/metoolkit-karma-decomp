/* ==== MeChunkInit ==== */

void MeChunkInit(MeChunk *chunk,int alignment)

{
  if ((uint)alignment < 4) {
    alignment = 4;
  }
  chunk->alignment = alignment;
  chunk->memBase = (void *)0x0;
  chunk->isInUse = 0;
  chunk->maxUsed = 0;
  chunk->memSize = 0;
  chunk->mode = kMeChunkModeKeepOnPut;
  return;
}


/* ==== MeChunkTerm ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeChunkTerm(MeChunk *chunk)

{
  if (chunk->memBase != (void *)0x0) {
    (*_DAT_00011010)(chunk->memBase);
    chunk->memBase = (void *)0x0;
    chunk->memSize = 0;
  }
  return;
}


/* ==== MeChunkSetMode ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeChunkSetMode(MeChunk *chunk,kMeChunkMode mode)

{
  if (chunk->mode != mode) {
    if (((chunk->isInUse == 0) && (chunk->mode == kMeChunkModeKeepOnPut)) &&
       (chunk->memBase != (void *)0x0)) {
      (*_DAT_00011010)(chunk->memBase);
    }
    chunk->mode = mode;
  }
  return;
}


/* ==== MeChunkGetMem ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void * MeChunkGetMem(MeChunk *chunk,int size)

{
  void *pvVar1;
  
  if (chunk->memSize < size) {
    if (chunk->memBase != (void *)0x0) {
      (*_DAT_00011010)(chunk->memBase);
      chunk->memBase = (void *)0x0;
      chunk->memSize = 0;
    }
    pvVar1 = (*_DAT_00011008)(size,chunk->alignment);
    chunk->memSize = size;
    chunk->memBase = pvVar1;
    if (chunk->maxUsed < size) {
      chunk->maxUsed = size;
    }
  }
  chunk->isInUse = 1;
  return chunk->memBase;
}


/* ==== MeChunkPutMem ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeChunkPutMem(MeChunk *chunk,void *mem)

{
  if (chunk->isInUse != 0) {
    if (chunk->mode == kMeChunkModeFreeOnPut) {
      (*_DAT_00011010)(mem);
      chunk->memBase = (void *)0x0;
      chunk->memSize = 0;
    }
    chunk->isInUse = 0;
  }
  return;
}


/* ==== MeChunkGetMaxSize ==== */

int MeChunkGetMaxSize(MeChunk *chunk)

{
  return chunk->maxUsed;
}


/* ==== MeChunkGetCurrentSize ==== */

int MeChunkGetCurrentSize(MeChunk *chunk)

{
  return chunk->memSize;
}


/* ==== MeChunkIsInUse ==== */

MeBool MeChunkIsInUse(MeChunk *chunk)

{
  return chunk->isInUse;
}


/* ==== MeChunkGetAlignment ==== */

int MeChunkGetAlignment(MeChunk *chunk)

{
  return chunk->alignment;
}


