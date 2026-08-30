/* ==== MeLoadWholeFileHandle ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

char * MeLoadWholeFileHandle(int handle,int *size)

{
  int iVar1;
  char *pcVar2;
  int iVar3;
  void *destination;
  
                    /* Unresolved local var: int fileSize@[DW_OP_reg6(ESI)]
                       Unresolved local var: int read@[DW_OP_reg3(EBX)] */
  iVar1 = MeLseek(handle,0,2);
  pcVar2 = (char *)(*_MeMemoryAPI)(iVar1);
  MeInfo(0x27,"Reading: file size %d bytes");
  MeLseek(handle,0,0);
  iVar3 = MeRead(handle,pcVar2,iVar1);
  if (iVar3 != iVar1) {
    MeWarning(0,"MeLoadWholeFile bad length: size: %d, read: %d");
  }
  MeClose(handle);
  *size = iVar3;
  return pcVar2;
}


/* ==== MeSaveWholeFileHandle ==== */

void MeSaveWholeFileHandle(int handle,char *data,int size)

{
  MeInfo(0x27,"MeSaveWholeFile: file size %d bytes");
  MeWrite(handle,data,size);
  return;
}


