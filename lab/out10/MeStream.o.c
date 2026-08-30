/* ==== MeStreamOpen ==== */

MeStream MeStreamOpen(char *filename,MeOpenMode_enum mode)

{
  MeStream p_Var1;
  
  p_Var1 = MeStreamOpenPrefixed(filename,mode,0);
  return p_Var1;
}


/* ==== MeStreamOpenWithSearch ==== */

MeStream MeStreamOpenWithSearch(char *filename,MeOpenMode_enum mode)

{
  MeStream p_Var1;
  
  p_Var1 = MeStreamOpenPrefixed(filename,mode,1);
  return p_Var1;
}


/* ==== MeStreamOpenAsMemBuffer ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeStream MeStreamOpenAsMemBuffer(uint initialSize)

{
  MeStream p_Var1;
  char *pcVar2;
  MeStream p_Var3;
  
                    /* Unresolved local var: MeStream s@[DW_OP_reg3(EBX)] */
  p_Var1 = (*_MeMemoryAPI)(0x20);
  p_Var3 = (MeStream)0x0;
  if (p_Var1 != (MeStream)0x0) {
    p_Var1->filename = (char *)0x0;
    p_Var1->handle = (void *)0x0;
    p_Var1->bUseMemblock = 1;
    pcVar2 = (*_MeMemoryAPI)(initialSize);
    p_Var1->bufSize = initialSize;
    p_Var1->buffer = pcVar2;
    p_Var1->bufLength = 0;
    p_Var1->curIndex = 0;
    p_Var1->bGrowFast = 1;
    p_Var3 = p_Var1;
  }
  return p_Var3;
}


/* ==== MeStreamCreateFromMemBuffer ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeStream MeStreamCreateFromMemBuffer(char *buffer,uint bufLength,uint bufSize)

{
  MeStream p_Var1;
  MeStream p_Var2;
  
                    /* Unresolved local var: MeStream s@[DW_OP_reg2(EDX)] */
  p_Var1 = (*_MeMemoryAPI)(0x20);
  p_Var2 = (MeStream)0x0;
  if (p_Var1 != (MeStream)0x0) {
    p_Var1->filename = (char *)0x0;
    p_Var1->handle = (void *)0x0;
    p_Var1->bUseMemblock = 1;
    p_Var1->buffer = buffer;
    p_Var1->bufSize = bufSize;
    p_Var1->bufLength = bufLength;
    p_Var1->curIndex = 0;
    p_Var1->bGrowFast = 1;
    p_Var2 = p_Var1;
  }
  return p_Var2;
}


/* ==== MeStreamMemBufferFreeSlackSpace ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeStreamMemBufferFreeSlackSpace(MeStream stream)

{
  char *__dest;
  
                    /* Unresolved local var: char * newBuf@[DW_OP_reg3(EBX)] */
  if (((stream != (MeStream)0x0) && (stream->bUseMemblock != 0)) &&
     (stream->bufLength != stream->bufSize)) {
    __dest = (*_MeMemoryAPI)(stream->bufLength);
    memcpy(__dest,stream->buffer,stream->bufLength);
    (*_fread)(stream->buffer);
    stream->buffer = __dest;
    stream->bufSize = stream->bufLength;
    stream->curIndex = stream->bufLength;
  }
  return;
}


/* ==== MeStreamMemBufferUseConservativeGrowth ==== */

void MeStreamMemBufferUseConservativeGrowth(MeStream stream,MeBool bConsGrowth)

{
  if ((stream != (MeStream)0x0) && (stream->bUseMemblock != 0)) {
    stream->bGrowFast = (uint)(bConsGrowth == 0);
  }
  return;
}


/* ==== MeStreamClose ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeStreamClose(MeStream stream)

{
  if (stream != (MeStream)0x0) {
    if (stream->bUseMemblock == 0) {
      fclose(stream->handle);
    }
    else if (stream->buffer != (char *)0x0) {
      (*_fread)(stream->buffer);
    }
    (*_fread)(stream);
  }
  return;
}


/* ==== MeStreamRead ==== */

size_t MeStreamRead(void *buffer,size_t size,size_t count,MeStream stream)

{
  uint uVar1;
  size_t __n;
  size_t local_34;
  ushort __cwtmp;
  ushort __cw;
  
  local_34 = 0;
  if (stream != (MeStream)0x0) {
    if (stream->bUseMemblock == 0) {
      local_34 = fread(buffer,size,count,stream->handle);
    }
    else {
      __n = count * size;
      uVar1 = stream->curIndex;
      if (__n + uVar1 < stream->bufLength) {
        memcpy(buffer,stream->buffer + uVar1,__n);
        stream->curIndex = stream->curIndex + __n;
        local_34 = count;
      }
      else {
                    /* Unresolved local var: MeReal maxCount@[???]
                       Unresolved local var: uint retCount@[???] */
                    /* Unresolved local var: longdouble __value@[DW_OP_reg11(ST0)] */
        local_34 = (size_t)(longlong)
                           ROUND(ROUND((double)(stream->bufLength - uVar1) / (double)size));
        memcpy(buffer,stream->buffer + uVar1,local_34 * size);
        stream->curIndex = stream->curIndex + local_34 * size;
      }
    }
  }
  return local_34;
}


/* ==== MeStreamReadLine ==== */

char * MeStreamReadLine(char *string,int n,MeStream stream)

{
  uint uVar1;
  char *pcVar2;
  uint uVar3;
  uint uVar4;
  char *string_local;
  
                    /* Unresolved local var: uint endIndex@[DW_OP_reg3(EBX)] */
  if (stream->bUseMemblock == 0) {
    pcVar2 = fgets(string,n,stream->handle);
    return pcVar2;
  }
  uVar3 = stream->bufLength - 1;
  uVar1 = stream->curIndex;
  pcVar2 = (char *)0x0;
  if (uVar1 < uVar3) {
    pcVar2 = stream->buffer;
    uVar4 = uVar1;
    if ((pcVar2[uVar1] != '\n') && (n != 2)) {
      while (uVar4 = uVar4 + 1, uVar4 != uVar3) {
        if ((pcVar2[uVar4] == '\n') || (uVar4 - uVar1 == n + -2)) break;
      }
    }
    memcpy(string,pcVar2 + uVar1,(uVar4 - uVar1) + 1);
    string[(uVar4 - stream->curIndex) + 1] = '\0';
    stream->curIndex = uVar4 + 1;
    pcVar2 = string;
  }
  return pcVar2;
}


/* ==== MeStreamWrite ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

size_t MeStreamWrite(void *buffer,size_t size,size_t count,MeStream stream)

{
  char *__dest;
  size_t sVar1;
  size_t sVar2;
  uint uVar3;
  uint uVar4;
  uint newbufsize;
  size_t count_local;
  void *buffer_local;
  
  sVar1 = 0;
  if (stream != (MeStream)0x0) {
    if (stream->bUseMemblock == 0) {
      sVar2 = fwrite(buffer,size,count,stream->handle);
      return sVar2;
    }
    uVar3 = stream->curIndex;
    uVar4 = stream->bufSize;
    if (uVar4 < count * size + uVar3) {
                    /* Unresolved local var: char * newbuf@[DW_OP_reg3(EBX)] */
      if ((stream->bGrowFast == 0) || (uVar4 <= count * size)) {
        uVar4 = uVar4 + count * size;
      }
      else {
        uVar4 = uVar4 * 2;
      }
      __dest = (*_MeMemoryAPI)(uVar4);
      memcpy(__dest,stream->buffer,stream->bufLength);
      (*_fread)(stream->buffer);
      stream->buffer = __dest;
      uVar3 = stream->curIndex;
      stream->bufSize = uVar4;
    }
    uVar4 = count * size;
    memcpy(stream->buffer + uVar3,buffer,uVar4);
                    /* Unresolved local var: uint charsToLength@[DW_OP_reg2(EDX)] */
    uVar3 = (stream->bufLength - stream->curIndex) + 1;
    stream->curIndex = stream->curIndex + uVar4;
    sVar1 = count;
    if (uVar3 <= uVar4) {
      stream->bufLength = (uVar4 - uVar3) + 1 + stream->bufLength;
    }
  }
  return sVar1;
}


/* ==== MeStreamRewind ==== */

void MeStreamRewind(MeStream stream)

{
  if (stream != (MeStream)0x0) {
    if (stream->bUseMemblock == 0) {
      fseek(stream->handle,0,0);
    }
    else {
      stream->curIndex = 0;
    }
  }
  return;
}


/* ==== MeStreamOpenPrefixed ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeStream MeStreamOpenPrefixed(char *filename,MeOpenMode_enum mode,uint allPrefixes)

{
  uint uVar1;
  MeStream p_Var2;
  char *pcVar3;
  size_t __n;
  int iVar4;
  FILE *pFVar5;
  uint *puVar6;
  uint *puVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  uint prefixlen;
  char *m;
  MeStream s;
  char fullname [4096];
  
                    /* Unresolved local var: char * prefix2@[DW_OP_reg3(EBX)]
                       Unresolved local var: uint i@[DW_OP_reg7(EDI)] */
  p_Var2 = (*_MeMemoryAPI)(0x20);
  p_Var2->bUseMemblock = 0;
  if (mode == kMeOpenModeWRONLY) {
    m = "w";
  }
  else if (mode == kMeOpenModeRDONLY) {
    m = "r";
  }
  else if (mode == kMeOpenModeRDWR) {
    m = "a+";
  }
  else if (mode == kMeOpenModeRDBINARY) {
    m = "rb";
  }
  if ((_MeFilePrefix == (char *)0x0) || (*_MeFilePrefix == '\0')) {
    fullname[0] = '\0';
    prefixlen = 0;
  }
  else {
    strncpy(fullname,_MeFilePrefix,0x1000);
    fullname[0xfff] = '\0';
    pcVar3 = fullname;
    do {
      prefixlen = (uint)pcVar3;
      uVar8 = *(uint *)prefixlen + 0xfefefeff & (*(uint *)prefixlen ^ 0xffffffff);
      uVar9 = uVar8 & 0x80808080;
      pcVar3 = (char *)(prefixlen + 4);
    } while (uVar9 == 0);
    uVar1 = prefixlen + 4;
    if ((uVar8 & 0x8080) == 0) {
      uVar9 = uVar9 >> 0x10;
      uVar1 = prefixlen + 6;
    }
    prefixlen = uVar1;
    prefixlen = ((prefixlen - 3) - (uint)CARRY1((byte)uVar9,(byte)uVar9)) - (int)fullname;
  }
  iVar10 = 0;
  while (pcVar3 = MeGetDefaultFileLocation(iVar10), pcVar3 != (char *)0x0) {
    fullname[prefixlen] = '\0';
    __n = strlen(pcVar3);
    iVar4 = strncmp(filename,pcVar3,__n);
    if (iVar4 != 0) {
      puVar6 = (uint *)fullname;
      do {
        puVar7 = puVar6;
        puVar6 = puVar7 + 1;
        uVar8 = *puVar7 + 0xfefefeff & (*puVar7 ^ 0xffffffff);
        uVar9 = uVar8 & 0x80808080;
      } while (uVar9 == 0);
      if ((uVar8 & 0x8080) == 0) {
        uVar9 = uVar9 >> 0x10;
        puVar6 = (uint *)((int)puVar7 + 6);
      }
      strncat(fullname,pcVar3,
              0x1000 - (int)((int)puVar6 +
                            ((-3 - (uint)CARRY1((byte)uVar9,(byte)uVar9)) - (int)fullname)));
    }
    puVar6 = (uint *)fullname;
    do {
      puVar7 = puVar6;
      puVar6 = puVar7 + 1;
      uVar8 = *puVar7 + 0xfefefeff & (*puVar7 ^ 0xffffffff);
      uVar9 = uVar8 & 0x80808080;
    } while (uVar9 == 0);
    if ((uVar8 & 0x8080) == 0) {
      uVar9 = uVar9 >> 0x10;
      puVar6 = (uint *)((int)puVar7 + 6);
    }
    strncat(fullname,filename,
            0x1000 - ((int)puVar6 + ((-3 - (uint)CARRY1((byte)uVar9,(byte)uVar9)) - (int)fullname)))
    ;
    fullname[0xfff] = '\0';
    pFVar5 = fopen(fullname,m);
    p_Var2->handle = pFVar5;
    if ((pFVar5 != (FILE *)0x0) || (allPrefixes == 0)) break;
    iVar10 = iVar10 + 1;
  }
  if (p_Var2->handle == (void *)0x0) {
    (*_fread)(p_Var2);
    p_Var2 = (MeStream)0x0;
  }
  else {
    p_Var2->filename = filename;
  }
  return p_Var2;
}


