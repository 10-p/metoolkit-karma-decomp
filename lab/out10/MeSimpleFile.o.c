/* ==== MeLoadWholeFileHandle ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

char * MeLoadWholeFileHandle(int handle,int *size)

{
  uint uVar1;
  char *pcVar2;
  uint uVar3;
  void *destination;
  
                    /* Unresolved local var: int fileSize@[DW_OP_reg6(ESI)]
                       Unresolved local var: int read@[DW_OP_reg3(EBX)] */
  uVar1 = MeLseek(handle,0,2);
  pcVar2 = (*_MeMemoryAPI)(uVar1);
  MeInfo(0x27,"Reading: file size %d bytes");
  MeLseek(handle,0,0);
  uVar3 = MeRead(handle,pcVar2,uVar1);
  if (uVar3 != uVar1) {
    MeWarning(0,"MeLoadWholeFile bad length: size: %d, read: %d");
  }
  MeClose(handle);
  *size = uVar3;
  return pcVar2;
}


/* ==== MeSaveWholeFileHandle ==== */

void MeSaveWholeFileHandle(int handle,char *data,int size)

{
  MeInfo(0x27,"MeSaveWholeFile: file size %d bytes");
  MeWrite(handle,data,size);
  return;
}


