/* ==== MeOpenPrefixed ==== */

int MeOpenPrefixed(char *filename,MeOpenMode_enum mode,uint allPrefixes)

{
  size_t __n;
  int iVar1;
  uint *puVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  char *__s;
  int iVar6;
  char fullname [4096];
  
                    /* Unresolved local var: uint prefixlen@[???]
                       Unresolved local var: char * prefix2@[DW_OP_reg6(ESI)]
                       Unresolved local var: uint i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int f@[DW_OP_reg2(EDX)] */
  iVar6 = 0;
  __s = "";
  do {
    fullname[0] = '\0';
    __n = strlen(__s);
    iVar1 = strncmp(filename,__s,__n);
    puVar2 = (uint *)fullname;
    if (iVar1 != 0) {
      do {
        puVar3 = puVar2;
        puVar2 = puVar3 + 1;
        uVar4 = *puVar3 + 0xfefefeff & (*puVar3 ^ 0xffffffff);
        uVar5 = uVar4 & 0x80808080;
      } while (uVar5 == 0);
      if ((uVar4 & 0x8080) == 0) {
        uVar5 = uVar5 >> 0x10;
        puVar2 = (uint *)((int)puVar3 + 6);
      }
      strncat(fullname,__s,
              0x1000 - (int)((int)puVar2 +
                            ((-3 - (uint)CARRY1((byte)uVar5,(byte)uVar5)) - (int)fullname)));
      puVar2 = (uint *)fullname;
    }
    do {
      puVar3 = puVar2;
      puVar2 = puVar3 + 1;
      uVar4 = *puVar3 + 0xfefefeff & (*puVar3 ^ 0xffffffff);
      uVar5 = uVar4 & 0x80808080;
    } while (uVar5 == 0);
    if ((uVar4 & 0x8080) == 0) {
      uVar5 = uVar5 >> 0x10;
      puVar2 = (uint *)((int)puVar3 + 6);
    }
    strncat(fullname,filename,
            0x1000 - (int)((int)puVar2 +
                          ((-3 - (uint)CARRY1((byte)uVar5,(byte)uVar5)) - (int)fullname)));
    fullname[0xfff] = '\0';
    iVar1 = MeOpenRaw(fullname,mode);
    if (-1 < iVar1) {
      return iVar1;
    }
    if (allPrefixes == 0) {
      return iVar1;
    }
    iVar6 = iVar6 + 1;
    __s = MeDefaultFileLocations[iVar6];
  } while (__s != (char *)0x0);
  return iVar1;
}


/* ==== MeGetDefaultFileLocation ==== */

char * MeGetDefaultFileLocation(int i)

{
  return MeDefaultFileLocations[i];
}


/* ==== MeOpenWithSearch ==== */

int MeOpenWithSearch(char *filename,MeOpenMode_enum mode)

{
  int iVar1;
  
  iVar1 = MeOpenPrefixed(filename,mode,1);
  return iVar1;
}


/* ==== MeOpen ==== */

int MeOpen(char *filename,MeOpenMode_enum mode)

{
  int iVar1;
  
  iVar1 = MeOpenPrefixed(filename,mode,0);
  return iVar1;
}


