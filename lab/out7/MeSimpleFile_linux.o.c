/* ==== MeOpenRaw ==== */

int MeOpenRaw(char *filename,MeOpenMode_enum mode)

{
  int iVar1;
  undefined4 in_EDX;
  undefined4 extraout_EDX;
  int unaff_EBX;
  undefined4 unaff_ESI;
  
                    /* Unresolved local var: int flag@[DW_OP_reg3(EBX)]
                       Unresolved local var: int m@[DW_OP_reg6(ESI)]
                       Unresolved local var: int file@[DW_OP_reg0(EAX)] */
  if (mode == kMeOpenModeWRONLY) {
    unaff_EBX = 0x241;
LAB_00010049:
    unaff_ESI = 0x1a0;
  }
  else {
    if (mode != kMeOpenModeRDONLY) {
      if (mode == kMeOpenModeRDWR) {
        unaff_EBX = 2;
        goto LAB_00010049;
      }
      if (mode != kMeOpenModeRDBINARY) {
        MeFatalError(3,"Invalid mode parameter %d passed to MeOpen()");
        in_EDX = extraout_EDX;
        goto LAB_0001002d;
      }
    }
    unaff_EBX = 0;
  }
LAB_0001002d:
  iVar1 = open(filename,unaff_EBX,unaff_ESI,in_EDX);
  return iVar1;
}


/* ==== MeLseek ==== */

int MeLseek(int file,int offset,MeSeekOrigin_enum origin)

{
  __off_t _Var1;
  int unaff_EDI;
  
                    /* Unresolved local var: int o@[DW_OP_reg7(EDI)] */
  if (origin == kMeSeekCUR) {
    unaff_EDI = 1;
  }
  else if (origin == kMeSeekSET) {
    unaff_EDI = 0;
  }
  else if (origin == kMeSeekEND) {
    unaff_EDI = 2;
  }
  else {
    MeFatalError(3,"Invalid origin parameter %d passed to MeLseek()");
  }
  _Var1 = lseek(file,offset,unaff_EDI);
  return _Var1;
}


/* ==== MeRead ==== */

int MeRead(int file,void *buf,int count)

{
  ssize_t sVar1;
  
  sVar1 = read(file,buf,count);
  return sVar1;
}


/* ==== MeWrite ==== */

int MeWrite(int file,void *buf,int count)

{
  ssize_t sVar1;
  
  sVar1 = write(file,buf,count);
  return sVar1;
}


/* ==== MeClose ==== */

/* WARNING: Unknown calling convention */

int MeClose(int file)

{
  int iVar1;
  
  iVar1 = close(file);
  return iVar1;
}


