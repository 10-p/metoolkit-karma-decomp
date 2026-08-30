/* ==== writeIntArrayToFile ==== */

void writeIntArrayToFile(int file,char *desc,int num_elts,int *array)

{
  int *piVar1;
  bool bVar2;
  int iVar3;
  uint uVar4;
  undefined4 extraout_ECX;
  int iVar5;
  uint uVar6;
  char buf [256];
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar5 = 0;
  iVar3 = sprintf(buf,"%s\n",desc);
  MeWrite(file,buf,iVar3);
  if (num_elts != 0) {
    uVar4 = num_elts & 3;
    iVar3 = 0;
    if (uVar4 != 0) {
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        if (bVar2) {
          iVar3 = sprintf(buf,"%d\n",*array,extraout_ECX);
          uVar4 = MeWrite(file,buf,iVar3);
        }
        uVar6 = (uint)bVar2;
        iVar5 = uVar6 + 1;
        iVar3 = sprintf(buf,"%d\n",array[uVar6],uVar4);
        uVar4 = MeWrite(file,buf,iVar3);
      }
      piVar1 = array + iVar5;
      iVar5 = iVar5 + 1;
      iVar3 = sprintf(buf,"%d\n",*piVar1,uVar4);
      iVar3 = MeWrite(file,buf,iVar3);
      if (iVar5 == num_elts) {
        return;
      }
    }
    do {
      iVar3 = sprintf(buf,"%d\n",array[iVar5],iVar3);
      MeWrite(file,buf,iVar3);
      iVar3 = sprintf(buf,"%d\n",array[iVar5 + 1]);
      MeWrite(file,buf,iVar3);
      iVar3 = sprintf(buf,"%d\n",array[iVar5 + 2]);
      MeWrite(file,buf,iVar3);
      iVar3 = iVar5 + 3;
      iVar5 = iVar5 + 4;
      iVar3 = sprintf(buf,"%d\n",array[iVar3]);
      iVar3 = MeWrite(file,buf,iVar3);
    } while (iVar5 != num_elts);
  }
  return;
}


/* ==== writeMdtKeaBodyIndexPairArrayToFile ==== */

void writeMdtKeaBodyIndexPairArrayToFile(int file,char *desc,int num_elts,int (*array) [2])

{
  int *piVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 uVar6;
  uint uVar7;
  uint uVar8;
  char buf [256];
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  uVar7 = 0;
  uVar3 = num_elts * 2;
  iVar4 = sprintf(buf,"%s\n",desc);
  MeWrite(file,buf,iVar4);
  if (uVar3 != 0) {
    uVar5 = uVar3 & 3;
    iVar4 = 0;
    if (uVar5 != 0) {
      uVar6 = extraout_EDX;
      if (1 < uVar5) {
        bVar2 = 2 < uVar5;
        if (bVar2) {
          iVar4 = sprintf(buf,"%d\n",(*array)[0],uVar5);
          uVar5 = MeWrite(file,buf,iVar4);
        }
        uVar8 = (uint)bVar2;
        uVar7 = uVar8 + 1;
        iVar4 = sprintf(buf,"%d\n",(*array)[uVar8],uVar5);
        MeWrite(file,buf,iVar4);
        uVar6 = extraout_EDX_00;
      }
      piVar1 = *array + uVar7;
      uVar7 = uVar7 + 1;
      iVar4 = sprintf(buf,"%d\n",*piVar1,uVar6);
      iVar4 = MeWrite(file,buf,iVar4);
      if (uVar7 == uVar3) {
        return;
      }
    }
    do {
      iVar4 = sprintf(buf,"%d\n",(*array)[uVar7],iVar4);
      MeWrite(file,buf,iVar4);
      iVar4 = sprintf(buf,"%d\n",(*array)[uVar7 + 1]);
      MeWrite(file,buf,iVar4);
      iVar4 = sprintf(buf,"%d\n",array[1][uVar7]);
      MeWrite(file,buf,iVar4);
      iVar4 = uVar7 + 1;
      uVar7 = uVar7 + 4;
      iVar4 = sprintf(buf,"%d\n",array[1][iVar4]);
      iVar4 = MeWrite(file,buf,iVar4);
    } while (uVar7 != uVar3);
  }
  return;
}


/* ==== writeFloatArrayToFile ==== */

void writeFloatArrayToFile(int file,char *desc,int num_elts,MeReal *array)

{
  MeReal *pMVar1;
  bool bVar2;
  int iVar3;
  uint uVar4;
  undefined4 extraout_EDX;
  int iVar5;
  uint uVar6;
  char buf [256];
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar5 = 0;
  iVar3 = sprintf(buf,"%s\n",desc);
  MeWrite(file,buf,iVar3);
  if (num_elts != 0) {
    uVar4 = num_elts & 3;
    iVar3 = 0;
    if (uVar4 != 0) {
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        if (bVar2) {
          iVar3 = sprintf(buf,"%08x\n",*array,extraout_EDX);
          uVar4 = MeWrite(file,buf,iVar3);
        }
        uVar6 = (uint)bVar2;
        iVar5 = uVar6 + 1;
        iVar3 = sprintf(buf,"%08x\n",array[uVar6],uVar4);
        uVar4 = MeWrite(file,buf,iVar3);
      }
      pMVar1 = array + iVar5;
      iVar5 = iVar5 + 1;
      iVar3 = sprintf(buf,"%08x\n",*pMVar1,uVar4);
      iVar3 = MeWrite(file,buf,iVar3);
      if (iVar5 == num_elts) {
        return;
      }
    }
    do {
      iVar3 = sprintf(buf,"%08x\n",array[iVar5],iVar3);
      MeWrite(file,buf,iVar3);
      iVar3 = sprintf(buf,"%08x\n",array[iVar5 + 1]);
      MeWrite(file,buf,iVar3);
      iVar3 = sprintf(buf,"%08x\n",array[iVar5 + 2]);
      MeWrite(file,buf,iVar3);
      iVar3 = iVar5 + 3;
      iVar5 = iVar5 + 4;
      iVar3 = sprintf(buf,"%08x\n",array[iVar3]);
      iVar3 = MeWrite(file,buf,iVar3);
    } while (iVar5 != num_elts);
  }
  return;
}


/* ==== writeFloatVec4ToFile ==== */

void writeFloatVec4ToFile(int file,char *desc,MeReal *array)

{
  int iVar1;
  char buf [256];
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar1 = sprintf(buf,"%s\n",desc);
  MeWrite(file,buf,iVar1);
  iVar1 = sprintf(buf,"%08x\n",*array);
  MeWrite(file,buf,iVar1);
  iVar1 = sprintf(buf,"%08x\n",array[1]);
  MeWrite(file,buf,iVar1);
  iVar1 = sprintf(buf,"%08x\n",array[2]);
  MeWrite(file,buf,iVar1);
  iVar1 = sprintf(buf,"%08x\n",array[3]);
  MeWrite(file,buf,iVar1);
  return;
}


/* ==== writeFloatToFile ==== */

void writeFloatToFile(int file,char *desc,MeReal value)

{
  int iVar1;
  char buf [256];
  
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar1 = sprintf(buf,"%s\n",desc);
  MeWrite(file,buf,iVar1);
  iVar1 = sprintf(buf,"%08x\n",value);
  MeWrite(file,buf,iVar1);
  return;
}


/* ==== writeIntToFile ==== */

void writeIntToFile(int file,char *desc,int value)

{
  int iVar1;
  char buf [256];
  
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar1 = sprintf(buf,"%s\n",desc);
  MeWrite(file,buf,iVar1);
  iVar1 = sprintf(buf,"%d\n",value);
  MeWrite(file,buf,iVar1);
  return;
}


/* ==== readIntArrayFromFile ==== */

void readIntArrayFromFile(MeStream stream,char *desc,int num_elts,int *array)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar4;
  int *piVar5;
  int *piVar6;
  int i;
  char buf [256];
  
  MeStreamReadLine(buf,0x100,stream);
  i = 0;
  if (num_elts != 0) {
    uVar2 = num_elts & 3;
    iVar3 = 0;
    if (uVar2 != 0) {
      uVar4 = extraout_ECX;
      piVar6 = array;
      if (1 < uVar2) {
        bVar1 = 2 < uVar2;
        piVar5 = array;
        if (bVar1) {
          MeStreamReadLine(buf,0x100,stream);
          piVar5 = array + 1;
          uVar2 = sscanf(buf,"%d\n",array,uVar2);
        }
        i = (int)bVar1;
        MeStreamReadLine(buf,0x100,stream);
        piVar6 = piVar5 + 1;
        sscanf(buf,"%d\n",piVar5,uVar2);
        i = i + 1;
        uVar4 = extraout_ECX_00;
      }
      MeStreamReadLine(buf,0x100,stream);
      array = piVar6 + 1;
      iVar3 = sscanf(buf,"%d\n",piVar6,uVar4);
      i = i + 1;
      if (i == num_elts) {
        return;
      }
    }
    do {
      MeStreamReadLine(buf,0x100,stream);
      sscanf(buf,"%d\n",array,iVar3);
      MeStreamReadLine(buf,0x100,stream);
      piVar6 = array + 2;
      sscanf(buf,"%d\n",array + 1);
      MeStreamReadLine(buf,0x100,stream);
      piVar5 = array + 3;
      array = array + 4;
      sscanf(buf,"%d\n",piVar6);
      MeStreamReadLine(buf,0x100,stream);
      iVar3 = sscanf(buf,"%d\n",piVar5);
      i = i + 4;
    } while (i != num_elts);
  }
  return;
}


/* ==== readMdtKeaBodyIndexPairArrayFromFile ==== */

void readMdtKeaBodyIndexPairArrayFromFile
               (MeStream stream,char *desc,int num_elts,MdtKeaBodyIndexPair *array)

{
  MdtKeaBodyIndexPair *paiVar1;
  bool bVar2;
  uint uVar3;
  MdtKeaBodyIndexPair *paiVar4;
  uint uVar5;
  int iVar6;
  int *piVar7;
  int *piVar8;
  int i;
  char buf [256];
  
  uVar3 = num_elts * 2;
  MeStreamReadLine(buf,0x100,stream);
  i = 0;
  if (uVar3 != 0) {
    uVar5 = uVar3 & 3;
    iVar6 = 0;
    if (uVar5 != 0) {
      piVar8 = *array;
      if (1 < uVar5) {
        bVar2 = 2 < uVar5;
        piVar7 = *array;
        if (bVar2) {
          MeStreamReadLine(buf,0x100,stream);
          piVar7 = *array + 1;
          sscanf(buf,"%d\n",array,uVar5);
        }
        i = (int)bVar2;
        MeStreamReadLine(buf,0x100,stream);
        piVar8 = piVar7 + 1;
        sscanf(buf,"%d\n",piVar7);
        uVar5 = i + 1;
        i = uVar5;
      }
      MeStreamReadLine(buf,0x100,stream);
      array = (MdtKeaBodyIndexPair *)(piVar8 + 1);
      iVar6 = sscanf(buf,"%d\n",piVar8,uVar5);
      i = i + 1;
      if (i == uVar3) {
        return;
      }
    }
    do {
      MeStreamReadLine(buf,0x100,stream);
      sscanf(buf,"%d\n",array,iVar6);
      MeStreamReadLine(buf,0x100,stream);
      paiVar1 = array + 1;
      sscanf(buf,"%d\n",*array + 1);
      MeStreamReadLine(buf,0x100,stream);
      paiVar4 = array + 1;
      array = array + 2;
      sscanf(buf,"%d\n",paiVar1);
      MeStreamReadLine(buf,0x100,stream);
      iVar6 = sscanf(buf,"%d\n",*paiVar4 + 1);
      i = i + 4;
    } while (i != uVar3);
  }
  return;
}


/* ==== readFloatArrayFromFile ==== */

void readFloatArrayFromFile(MeStream stream,char *desc,int num_elts,MeReal *array)

{
  bool bVar1;
  uint uVar2;
  undefined4 extraout_ECX;
  int unaff_EBX;
  MeReal *pMVar3;
  MeReal *pMVar4;
  undefined4 uVar5;
  int i;
  char buf [256];
  
  MeStreamReadLine(buf,0x100,stream);
  i = 0;
  if (num_elts != 0) {
    uVar2 = num_elts & 3;
    if (uVar2 != 0) {
      pMVar4 = array;
      if (1 < uVar2) {
        bVar1 = 2 < uVar2;
        pMVar3 = array;
        if (bVar1) {
          uVar5 = extraout_ECX;
          MeStreamReadLine(buf,0x100,stream);
          pMVar3 = array + 1;
          uVar2 = sscanf(buf,"%08x\n",array,uVar5);
        }
        i = (int)bVar1;
        MeStreamReadLine(buf,0x100,stream);
        pMVar4 = pMVar3 + 1;
        uVar2 = sscanf(buf,"%08x\n",pMVar3,uVar2);
        i = i + 1;
      }
      MeStreamReadLine(buf,0x100,stream);
      array = pMVar4 + 1;
      sscanf(buf,"%08x\n",pMVar4,uVar2);
      i = i + 1;
      unaff_EBX = num_elts;
      if (i == num_elts) {
        return;
      }
    }
    do {
      MeStreamReadLine(buf,0x100,stream);
      sscanf(buf,"%08x\n",array,unaff_EBX);
      MeStreamReadLine(buf,0x100,stream);
      pMVar4 = array + 2;
      sscanf(buf,"%08x\n",array + 1);
      MeStreamReadLine(buf,0x100,stream);
      pMVar3 = array + 3;
      array = array + 4;
      sscanf(buf,"%08x\n",pMVar4);
      MeStreamReadLine(buf,0x100,stream);
      sscanf(buf,"%08x\n",pMVar3);
      i = i + 4;
      unaff_EBX = num_elts;
    } while (i != num_elts);
  }
  return;
}


/* ==== readFloatVec4FromFile ==== */

void readFloatVec4FromFile(MeStream stream,char *desc,MeReal *array)

{
  char buf [256];
  
                    /* Unresolved local var: int i@[???] */
  MeStreamReadLine(buf,0x100,stream);
  MeStreamReadLine(buf,0x100,stream);
  sscanf(buf,"%08x\n",array);
  MeStreamReadLine(buf,0x100,stream);
  sscanf(buf,"%08x\n",array + 1);
  MeStreamReadLine(buf,0x100,stream);
  sscanf(buf,"%08x\n",array + 2);
  MeStreamReadLine(buf,0x100,stream);
  sscanf(buf,"%08x\n",array + 3);
  return;
}


/* ==== readFloatFromFile ==== */

void readFloatFromFile(MeStream stream,char *desc,MeReal *value)

{
  char buf [256];
  
  MeStreamReadLine(buf,0x100,stream);
  MeStreamReadLine(buf,0x100,stream);
  sscanf(buf,"%08x\n",value);
  return;
}


/* ==== readIntFromFile ==== */

void readIntFromFile(MeStream stream,char *desc,int *value)

{
  char buf [256];
  
  MeStreamReadLine(buf,0x100,stream);
  MeStreamReadLine(buf,0x100,stream);
  sscanf(buf,"%d\n",value);
  return;
}


/* ==== writeKeaInputToFile ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void writeKeaInputToFile(MdtKeaConstraints constraints,MdtKeaParameters parameters,
                        MdtKeaBody **blist,int num_bodies)

{
  int *piVar1;
  MeReal *pMVar2;
  MeReal MVar3;
  MdtKeaBody *pMVar4;
  bool bVar5;
  char *pcVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 uVar10;
  undefined4 extraout_ECX_04;
  undefined4 extraout_ECX_05;
  undefined4 extraout_ECX_06;
  undefined4 extraout_ECX_07;
  undefined4 extraout_ECX_08;
  undefined4 extraout_ECX_09;
  undefined4 extraout_ECX_10;
  undefined4 uVar11;
  undefined4 extraout_ECX_11;
  undefined4 extraout_ECX_12;
  undefined4 extraout_ECX_13;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 extraout_EDX_01;
  undefined4 extraout_EDX_02;
  undefined4 extraout_EDX_03;
  undefined4 extraout_EDX_04;
  undefined4 extraout_EDX_05;
  undefined4 extraout_EDX_06;
  undefined4 extraout_EDX_07;
  undefined4 extraout_EDX_08;
  undefined4 extraout_EDX_09;
  undefined4 extraout_EDX_10;
  undefined4 extraout_EDX_11;
  undefined4 extraout_EDX_12;
  undefined4 extraout_EDX_13;
  undefined4 extraout_EDX_14;
  int iVar12;
  uint uVar13;
  uint uVar14;
  char *pcVar15;
  int iVar16;
  int iVar17;
  int local_23c;
  int local_238;
  MdtKeaBody **local_230;
  int file;
  int j;
  int i;
  char buf_1 [256];
  char buf [256];
  
                    /* Unresolved local var: int k@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  pcVar6 = (char *)MeOpen(*(void **)(_gDebug + 4),2);
  printf("-- writing kea input to file %s\n",*(undefined4 *)(_gDebug + 4));
  iVar7 = sprintf(buf_1,"%s\n","gamma");
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%08x\n",parameters.gamma);
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","epsilon");
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%08x\n",parameters.epsilon);
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","stepsize");
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%08x\n",parameters.stepsize);
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","max_iterations");
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",parameters.max_iterations);
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","num_partitions");
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.num_partitions);
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","max_partitions");
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.max_partitions);
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar12 = 0;
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","num_rows_exc_padding_partition");
  iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
  if (constraints.num_partitions != 0) {
    uVar8 = constraints.num_partitions & 3;
    iVar7 = 0;
    if (uVar8 != 0) {
      if (1 < uVar8) {
        uVar11 = extraout_ECX;
        if (2 < uVar8) {
          iVar7 = sprintf(buf_1,"%d\n",*constraints.num_rows_exc_padding_partition,uVar8);
          MeWrite((int)pcVar6,buf_1,iVar7);
          uVar11 = extraout_ECX_13;
        }
        uVar8 = (uint)(2 < uVar8);
        iVar12 = uVar8 + 1;
        iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding_partition[uVar8],uVar11);
        uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
      }
      piVar1 = constraints.num_rows_exc_padding_partition + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar8);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_partitions) goto LAB_00011080;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding_partition[iVar12],iVar7);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding_partition[iVar12 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding_partition[iVar12 + 2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding_partition[iVar7]);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_partitions);
  }
LAB_00011080:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","num_rows_inc_padding_partition",iVar7);
  iVar12 = 0;
  iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
  if (constraints.num_partitions != 0) {
    uVar8 = constraints.num_partitions & 3;
    uVar11 = extraout_EDX;
    if (uVar8 != 0) {
      if (1 < uVar8) {
        bVar5 = 2 < uVar8;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%d\n",*constraints.num_rows_inc_padding_partition,extraout_ECX_00);
          uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding_partition[uVar13],uVar8);
        uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
      }
      piVar1 = constraints.num_rows_inc_padding_partition + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar8);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      uVar11 = extraout_EDX_00;
      if (iVar12 == constraints.num_partitions) goto LAB_0001120b;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding_partition[iVar12],uVar11);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding_partition[iVar12 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding_partition[iVar12 + 2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding_partition[iVar7]);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      uVar11 = extraout_EDX_01;
    } while (iVar12 != constraints.num_partitions);
  }
LAB_0001120b:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","num_constraints_partition",iVar7);
  iVar12 = 0;
  iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
  if (constraints.num_partitions != 0) {
    uVar8 = constraints.num_partitions & 3;
    iVar7 = 0;
    if (uVar8 != 0) {
      if (1 < uVar8) {
        bVar5 = 2 < uVar8;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%d\n",*constraints.num_constraints_partition,uVar8);
          uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints_partition[uVar13],uVar8);
        uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
      }
      piVar1 = constraints.num_constraints_partition + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar8);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_partitions) goto LAB_00011396;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints_partition[iVar12],iVar7);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints_partition[iVar12 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints_partition[iVar12 + 2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints_partition[iVar7]);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_partitions);
  }
LAB_00011396:
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","num_rows_inc_padding",iVar7);
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding);
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","num_rows_exc_padding");
  MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding);
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","max_rows_inc_padding");
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.max_rows_inc_padding);
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","num_constraints");
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints);
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","max_constraints");
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.max_constraints);
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf,"Jstore\n");
  MeWrite((int)pcVar6,buf,iVar7);
  i = 0;
  local_238 = 0;
  local_23c = 0;
  while( true ) {
    iVar7 = constraints.num_rows_inc_padding;
    if (constraints.num_rows_inc_padding < 0) {
      iVar7 = constraints.num_rows_inc_padding + 3;
    }
    iVar7 = iVar7 >> 2;
    if (i == iVar7) break;
    j = 0;
    do {
      iVar12 = 0;
      iVar16 = local_23c + j;
      do {
        iVar12 = iVar12 + 3;
        iVar7 = sprintf(buf,"%08x; ",(*constraints.Jstore)[0].col[0][iVar16],iVar7);
        MeWrite((int)pcVar6,buf,iVar7);
        iVar7 = sprintf(buf,"%08x; ",(*constraints.Jstore)[0].col[1][iVar16]);
        MeWrite((int)pcVar6,buf,iVar7);
        pMVar2 = (*constraints.Jstore)[0].col[2] + iVar16;
        iVar16 = iVar16 + 0xc;
        iVar7 = sprintf(buf,"%08x; ",*pMVar2);
        iVar7 = MeWrite((int)pcVar6,buf,iVar7);
      } while (iVar12 != 6);
      iVar16 = 0;
      iVar12 = local_238;
      do {
        iVar17 = j * 4;
        iVar16 = iVar16 + 3;
        iVar7 = sprintf(buf,"%08x; ",
                        *(undefined4 *)((int)(*constraints.Jstore)[1].col[0] + iVar12 + iVar17),
                        iVar7);
        MeWrite((int)pcVar6,buf,iVar7);
        iVar7 = sprintf(buf,"%08x; ",
                        *(undefined4 *)((int)(*constraints.Jstore)[1].col[1] + iVar17 + iVar12));
        MeWrite((int)pcVar6,buf,iVar7);
        iVar17 = iVar17 + iVar12;
        iVar12 = iVar12 + 0x30;
        iVar7 = sprintf(buf,"%08x; ",*(undefined4 *)((int)(*constraints.Jstore)[1].col[2] + iVar17))
        ;
        iVar7 = MeWrite((int)pcVar6,buf,iVar7);
      } while (iVar16 != 6);
      iVar7 = sprintf(buf,"\n",6,6);
      iVar7 = MeWrite((int)pcVar6,buf,iVar7);
      j = j + 1;
    } while (j != 4);
    iVar7 = sprintf(buf,"\n",iVar7,iVar7);
    MeWrite((int)pcVar6,buf,iVar7);
    local_238 = local_238 + 0xc0;
    local_23c = local_23c + 0x30;
    i = i + 1;
  }
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n",&DAT_000164e6,iVar7);
  iVar12 = 0;
  iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
  if (constraints.num_rows_exc_padding != 0) {
    uVar8 = constraints.num_rows_exc_padding & 3;
    uVar11 = extraout_EDX_02;
    if (uVar8 != 0) {
      if (1 < uVar8) {
        bVar5 = 2 < uVar8;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.xi,extraout_ECX_01);
          uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.xi[uVar13],uVar8);
        uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
      }
      pMVar2 = constraints.xi + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar8);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      uVar11 = extraout_EDX_03;
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_0001199b;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xi[iVar12],uVar11);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xi[iVar12 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xi[iVar12 + 2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xi[iVar7]);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      uVar11 = extraout_EDX_04;
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_0001199b:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n",&DAT_000164e9,iVar7);
  iVar12 = 0;
  iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
  if (constraints.num_rows_exc_padding != 0) {
    uVar8 = constraints.num_rows_exc_padding & 3;
    iVar7 = 0;
    if (uVar8 != 0) {
      if (1 < uVar8) {
        bVar5 = 2 < uVar8;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.c,uVar8);
          uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.c[uVar13],uVar8);
        uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
      }
      pMVar2 = constraints.c + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar8);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_00011b26;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.c[iVar12],iVar7);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.c[iVar12 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.c[iVar12 + 2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.c[iVar7]);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_00011b26:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n",&DAT_000164eb,iVar7);
  iVar12 = 0;
  MeWrite((int)pcVar6,buf_1,iVar7);
  uVar11 = extraout_EDX_05;
  if (constraints.num_rows_exc_padding != 0) {
    uVar8 = constraints.num_rows_exc_padding & 3;
    uVar10 = extraout_ECX_02;
    if (uVar8 != 0) {
      if (1 < uVar8) {
        bVar5 = 2 < uVar8;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.lo,0);
          uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.lo[uVar13],uVar8);
        uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
      }
      pMVar2 = constraints.lo + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar8);
      MeWrite((int)pcVar6,buf_1,iVar7);
      uVar10 = extraout_ECX_03;
      uVar11 = extraout_EDX_06;
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_00011cb1;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.lo[iVar12],uVar10);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.lo[iVar12 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.lo[iVar12 + 2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.lo[iVar7]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      uVar10 = extraout_ECX_04;
      uVar11 = extraout_EDX_07;
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_00011cb1:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n",&DAT_000164ee,uVar11);
  iVar12 = 0;
  iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
  if (constraints.num_rows_exc_padding != 0) {
    uVar8 = constraints.num_rows_exc_padding & 3;
    iVar7 = 0;
    if (uVar8 != 0) {
      uVar11 = extraout_EDX_08;
      if (1 < uVar8) {
        bVar5 = 2 < uVar8;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.hi,uVar8);
          uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.hi[uVar13],uVar8);
        MeWrite((int)pcVar6,buf_1,iVar7);
        uVar11 = extraout_EDX_09;
      }
      pMVar2 = constraints.hi + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar11);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_00011e3c;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.hi[iVar12],iVar7);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.hi[iVar12 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.hi[iVar12 + 2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.hi[iVar7]);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_00011e3c:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","slipfactor",iVar7);
  iVar12 = 0;
  MeWrite((int)pcVar6,buf_1,iVar7);
  uVar11 = extraout_ECX_05;
  if (constraints.num_rows_exc_padding != 0) {
    uVar8 = constraints.num_rows_exc_padding & 3;
    iVar7 = 0;
    if (uVar8 != 0) {
      if (1 < uVar8) {
        uVar11 = extraout_EDX_10;
        if (2 < uVar8) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.slipfactor,uVar8);
          MeWrite((int)pcVar6,buf_1,iVar7);
          uVar11 = extraout_EDX_14;
        }
        uVar8 = (uint)(2 < uVar8);
        iVar12 = uVar8 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.slipfactor[uVar8],uVar11);
        uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
      }
      pMVar2 = constraints.slipfactor + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar8);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      uVar11 = extraout_ECX_06;
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_00011fc7;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.slipfactor[iVar12],iVar7);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.slipfactor[iVar12 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.slipfactor[iVar12 + 2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.slipfactor[iVar7]);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      uVar11 = extraout_ECX_07;
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_00011fc7:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","xgamma",uVar11);
  iVar12 = 0;
  iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
  if (constraints.num_rows_exc_padding != 0) {
    uVar8 = constraints.num_rows_exc_padding & 3;
    iVar7 = 0;
    if (uVar8 != 0) {
      uVar11 = extraout_ECX_08;
      if (1 < uVar8) {
        bVar5 = 2 < uVar8;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.xgamma,uVar8);
          uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.xgamma[uVar13],uVar8);
        MeWrite((int)pcVar6,buf_1,iVar7);
        uVar11 = extraout_ECX_09;
      }
      pMVar2 = constraints.xgamma + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar11);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_00012152;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xgamma[iVar12],iVar7);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xgamma[iVar12 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xgamma[iVar12 + 2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xgamma[iVar7]);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_00012152:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","Jsize",iVar7);
  iVar12 = 0;
  iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
  if (constraints.num_constraints != 0) {
    uVar8 = constraints.num_constraints & 3;
    iVar7 = 0;
    if (uVar8 != 0) {
      if (1 < uVar8) {
        uVar11 = extraout_ECX_10;
        if (2 < uVar8) {
          iVar7 = sprintf(buf_1,"%d\n",*constraints.Jsize,uVar8);
          MeWrite((int)pcVar6,buf_1,iVar7);
          uVar11 = extraout_ECX_12;
        }
        uVar8 = (uint)(2 < uVar8);
        iVar12 = uVar8 + 1;
        iVar7 = sprintf(buf_1,"%d\n",constraints.Jsize[uVar8],uVar11);
        uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
      }
      piVar1 = constraints.Jsize + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar8);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_constraints) goto LAB_000122dd;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jsize[iVar12],iVar7);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jsize[iVar12 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jsize[iVar12 + 2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jsize[iVar7]);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_constraints);
  }
LAB_000122dd:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n",&DAT_00016509,iVar7);
  iVar12 = 0;
  iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
  if (constraints.num_constraints != 0) {
    uVar8 = constraints.num_constraints & 3;
    uVar11 = extraout_EDX_11;
    if (uVar8 != 0) {
      if (1 < uVar8) {
        bVar5 = 2 < uVar8;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%d\n",*constraints.Jofs,extraout_ECX_11);
          uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%d\n",constraints.Jofs[uVar13],uVar8);
        uVar8 = MeWrite((int)pcVar6,buf_1,iVar7);
      }
      piVar1 = constraints.Jofs + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar8);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      uVar11 = extraout_EDX_12;
      if (iVar12 == constraints.num_constraints) goto LAB_00012468;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jofs[iVar12],uVar11);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jofs[iVar12 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jofs[iVar12 + 2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jofs[iVar7]);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      uVar11 = extraout_EDX_13;
    } while (iVar12 != constraints.num_constraints);
  }
LAB_00012468:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  uVar8 = constraints.num_constraints * 2;
  iVar7 = sprintf(buf_1,"%s\n","Jbody",iVar7);
  uVar13 = 0;
  iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
  if (uVar8 != 0) {
    uVar9 = uVar8 & 3;
    iVar7 = 0;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        bVar5 = 2 < uVar9;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%d\n",(*constraints.Jbody)[0],uVar9);
          uVar9 = MeWrite((int)pcVar6,buf_1,iVar7);
        }
        uVar14 = (uint)bVar5;
        uVar13 = uVar14 + 1;
        iVar7 = sprintf(buf_1,"%d\n",(*constraints.Jbody)[uVar14],uVar9);
        uVar9 = MeWrite((int)pcVar6,buf_1,iVar7);
      }
      piVar1 = *constraints.Jbody + uVar13;
      uVar13 = uVar13 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar9);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
      if (uVar13 == uVar8) goto LAB_000125f8;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",(*constraints.Jbody)[uVar13],iVar7);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",(*constraints.Jbody)[uVar13 + 1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jbody[1][uVar13]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = uVar13 + 1;
      uVar13 = uVar13 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jbody[1][iVar7]);
      iVar7 = MeWrite((int)pcVar6,buf_1,iVar7);
    } while (uVar13 != uVar8);
  }
LAB_000125f8:
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","num_bodies",iVar7);
  MeWrite((int)pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",num_bodies);
  MeWrite((int)pcVar6,buf_1,iVar7);
  i = 0;
  if (num_bodies != 0) {
    local_230 = blist;
    pcVar15 = pcVar6;
    do {
      iVar7 = sprintf(buf,"body %d\n",i,pcVar15);
      MeWrite((int)pcVar6,buf,iVar7);
      MVar3 = (*local_230)->invmass;
      iVar7 = sprintf(buf_1,"%s\n","invmass");
      MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",MVar3);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = (*local_230)->flags;
      iVar12 = sprintf(buf_1,"%s\n","flags");
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      MeWrite((int)pcVar6,buf_1,iVar12);
      iVar7 = sprintf(buf_1,"%d\n",iVar7);
      MeWrite((int)pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","force");
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->force[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->force[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->force[2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->force[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","torque");
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->torque[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->torque[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->torque[2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->torque[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","invI0");
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI0[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI0[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI0[2]);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI0[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","invI1");
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI1[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI1[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI1[2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI1[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","invI2");
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI2[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI2[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI2[2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI2[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n",&DAT_00016555);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I0[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I0[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I0[2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I0[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n",&DAT_00016558);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I1[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I1[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I1[2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I1[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n",&DAT_0001655b);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I2[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I2[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I2[2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I2[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n",&DAT_0001655e);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->vel[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->vel[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->vel[2]);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->vel[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","velrot");
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->velrot[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->velrot[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->velrot[2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->velrot[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n",&DAT_00016569);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->qrot[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->qrot[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->qrot[2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->qrot[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","accel");
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->accel[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->accel[1]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->accel[2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->accel[3]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","fastSpinAxis");
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->fastSpinAxis[0]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->fastSpinAxis[1]);
      pcVar15 = buf_1;
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->fastSpinAxis[2]);
      MeWrite((int)pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->fastSpinAxis[3]);
      MeWrite((int)pcVar6,pcVar15,iVar7);
      i = i + 1;
      local_230 = local_230 + 1;
    } while (i != num_bodies);
  }
  MeClose((int)pcVar6);
  return;
}


/* ==== readKeaInputFromFile ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void readKeaInputFromFile
               (MdtKeaConstraints *constraints,MdtKeaParameters *parameters,MdtKeaBody **blist,
               int *num_bodies)

{
  uint uVar1;
  bool bVar2;
  MeReal *pMVar3;
  uint uVar4;
  int iVar5;
  void *pvVar6;
  MdtKeaBody *pMVar7;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  int iVar8;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 extraout_ECX_04;
  undefined4 extraout_ECX_05;
  undefined4 extraout_ECX_06;
  undefined4 extraout_ECX_07;
  undefined4 extraout_ECX_08;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 extraout_EDX_01;
  undefined4 extraout_EDX_02;
  undefined4 extraout_EDX_03;
  undefined4 extraout_EDX_04;
  undefined4 extraout_EDX_05;
  undefined4 extraout_EDX_06;
  undefined4 extraout_EDX_07;
  undefined4 extraout_EDX_08;
  undefined4 extraout_EDX_09;
  undefined4 extraout_EDX_10;
  undefined4 extraout_EDX_11;
  undefined4 extraout_EDX_12;
  undefined4 extraout_EDX_13;
  undefined4 extraout_EDX_14;
  undefined4 extraout_EDX_15;
  undefined4 extraout_EDX_16;
  undefined4 extraout_EDX_17;
  undefined4 extraout_EDX_18;
  MeReal *pMVar9;
  int *piVar10;
  int iVar11;
  MeReal *pMVar12;
  MeReal *pMVar13;
  int *piVar14;
  int *piVar15;
  uint uVar16;
  int iVar17;
  undefined4 uVar18;
  int local_25c;
  MdtKeaBody **local_258;
  MeStream stream;
  int i;
  char buf_1 [256];
  char buf [256];
  
                    /* Unresolved local var: int j@[DW_OP_reg7(EDI)]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  pMVar3 = MeStreamOpen(*(void **)(_gDebug + 0xc),0);
  if (pMVar3 == (MeReal *)0x0) {
    MeFatalError(0,"file %s could not be opened");
    return;
  }
  printf("-- reading kea input from file %s\n",*(undefined4 *)(_gDebug + 0xc));
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%08x\n",&parameters->gamma);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%08x\n",&parameters->epsilon);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%08x\n",parameters);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%d\n",&parameters->max_iterations);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%d\n",constraints);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%d\n",&constraints->max_partitions);
  uVar1 = constraints->num_partitions;
  piVar10 = constraints->num_rows_exc_padding_partition;
  uVar16 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    uVar18 = extraout_ECX;
    if (uVar4 != 0) {
      piVar15 = piVar10;
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        piVar14 = piVar10;
        if (bVar2) {
          piVar14 = piVar10 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3);
          uVar4 = sscanf(buf_1,"%d\n",piVar10,uVar4);
        }
        uVar16 = bVar2 + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3);
        piVar15 = piVar14 + 1;
        uVar4 = sscanf(buf_1,"%d\n",piVar14,uVar4);
      }
      uVar16 = uVar16 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar10 = piVar15 + 1;
      sscanf(buf_1,"%d\n",piVar15,uVar4);
      uVar18 = extraout_ECX_00;
      if (uVar16 == uVar1) goto LAB_00013b0c;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%d\n",piVar10,uVar18);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar15 = piVar10 + 2;
      sscanf(buf_1,"%d\n",piVar10 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar14 = piVar10 + 3;
      piVar10 = piVar10 + 4;
      sscanf(buf_1,"%d\n",piVar15);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%d\n",piVar14);
      uVar18 = extraout_ECX_01;
    } while (uVar16 != uVar1);
  }
LAB_00013b0c:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_partitions;
  piVar10 = constraints->num_rows_inc_padding_partition;
  uVar16 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    iVar5 = 0;
    if (uVar4 != 0) {
      uVar18 = extraout_EDX;
      piVar15 = piVar10;
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        piVar14 = piVar10;
        if (bVar2) {
          piVar14 = piVar10 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3);
          sscanf(buf_1,"%d\n",piVar10,uVar4);
        }
        uVar16 = bVar2 + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3);
        piVar15 = piVar14 + 1;
        sscanf(buf_1,"%d\n",piVar14,piVar10);
        uVar18 = extraout_EDX_00;
      }
      uVar16 = uVar16 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar10 = piVar15 + 1;
      iVar5 = sscanf(buf_1,"%d\n",piVar15,uVar18);
      if (uVar16 == uVar1) goto LAB_00013cbc;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%d\n",piVar10,iVar5);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar15 = piVar10 + 2;
      sscanf(buf_1,"%d\n",piVar10 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar14 = piVar10 + 3;
      piVar10 = piVar10 + 4;
      sscanf(buf_1,"%d\n",piVar15);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar5 = sscanf(buf_1,"%d\n",piVar14);
    } while (uVar16 != uVar1);
  }
LAB_00013cbc:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_partitions;
  piVar10 = constraints->num_constraints_partition;
  uVar16 = 0;
  pvVar6 = MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    pvVar6 = (void *)0x0;
    if (uVar4 != 0) {
      piVar15 = piVar10;
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        piVar14 = piVar10;
        if (bVar2) {
          uVar18 = extraout_EDX_01;
          MeStreamReadLine(buf_1,0x100,pMVar3);
          piVar14 = piVar10 + 1;
          uVar4 = sscanf(buf_1,"%d\n",piVar10,uVar18);
        }
        uVar16 = bVar2 + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3);
        piVar15 = piVar14 + 1;
        uVar4 = sscanf(buf_1,"%d\n",piVar14,uVar4);
      }
      uVar16 = uVar16 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar10 = piVar15 + 1;
      pvVar6 = (void *)sscanf(buf_1,"%d\n",piVar15,uVar4);
      if (uVar16 == uVar1) goto LAB_00013e6c;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%d\n",piVar10,pvVar6);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar15 = piVar10 + 2;
      sscanf(buf_1,"%d\n",piVar10 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar14 = piVar10 + 3;
      piVar10 = piVar10 + 4;
      sscanf(buf_1,"%d\n",piVar15);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pvVar6 = (void *)sscanf(buf_1,"%d\n",piVar14);
    } while (uVar16 != uVar1);
  }
LAB_00013e6c:
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%d\n",&constraints->num_rows_inc_padding,pvVar6);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%d\n",&constraints->num_rows_exc_padding);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%d\n",&constraints->max_rows_inc_padding);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%d\n",&constraints->num_constraints);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%d\n",&constraints->max_constraints);
  MeStreamReadLine(buf,0x100,pMVar3);
  i = 0;
  local_25c = 0;
  while( true ) {
    iVar5 = constraints->num_rows_inc_padding;
    if (iVar5 < 0) {
      iVar5 = iVar5 + 3;
    }
    iVar5 = iVar5 >> 2;
    if (i == iVar5) break;
    iVar17 = 0;
    iVar11 = local_25c;
    do {
      iVar17 = iVar17 + 2;
      pMVar9 = pMVar3;
      MeStreamReadLine(buf,0x100,pMVar3);
      iVar8 = (int)(*constraints->Jstore)[0].col[0] + iVar11;
      sscanf(buf,"%08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x;\n",iVar8,
             iVar8 + 0x300,iVar8 + 0x600,iVar8 + 0x900,iVar8 + 0xc00,iVar8 + 0xf00,iVar8 + 0x1200,
             iVar8 + 0x1500,iVar8 + 0x1800,iVar8 + 0x1b00,iVar8 + 0x1e00,iVar8 + 0x2100,pMVar9,iVar5
            );
      MeStreamReadLine(buf,0x100,pMVar3);
      iVar5 = (int)constraints->Jstore[1][0].col[0] + iVar11;
      iVar5 = sscanf(buf,"%08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x;\n"
                     ,iVar5,iVar5 + 0x300,iVar5 + 0x600,iVar5 + 0x900,iVar5 + 0xc00,iVar5 + 0xf00,
                     iVar5 + 0x1200,iVar5 + 0x1500,iVar5 + 0x1800,iVar5 + 0x1b00,iVar5 + 0x1e00,
                     iVar5 + 0x2100);
      iVar11 = iVar11 + 0x180;
    } while (iVar17 != 4);
    MeStreamReadLine(buf,0x100,pMVar3);
    local_25c = local_25c + 0x2400;
    i = i + 1;
  }
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar9 = constraints->xi;
  uVar16 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    iVar5 = 0;
    pMVar13 = pMVar9;
    if (uVar4 != 0) {
      uVar18 = extraout_EDX_02;
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        if (bVar2) {
          pMVar13 = pMVar9 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3);
          sscanf(buf_1,"%08x\n",pMVar9,uVar4);
        }
        uVar16 = bVar2 + 1;
        pMVar12 = pMVar9;
        MeStreamReadLine(buf_1,0x100,pMVar3);
        pMVar9 = pMVar13 + 1;
        sscanf(buf_1,"%08x\n",pMVar13,pMVar12);
        uVar18 = extraout_EDX_03;
      }
      uVar16 = uVar16 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar13 = pMVar9 + 1;
      iVar5 = sscanf(buf_1,"%08x\n",pMVar9,uVar18);
      if (uVar16 == uVar1) goto LAB_00014364;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar13,iVar5);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar9 = pMVar13 + 2;
      sscanf(buf_1,"%08x\n",pMVar13 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar12 = pMVar13 + 3;
      pMVar13 = pMVar13 + 4;
      sscanf(buf_1,"%08x\n",pMVar9);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar5 = sscanf(buf_1,"%08x\n",pMVar12);
    } while (uVar16 != uVar1);
  }
LAB_00014364:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar9 = constraints->c;
  uVar16 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    iVar5 = 0;
    pMVar13 = pMVar9;
    if (uVar4 != 0) {
      if (1 < uVar4) {
        uVar18 = extraout_EDX_04;
        if (2 < uVar4) {
          pMVar12 = pMVar9;
          MeStreamReadLine(buf_1,0x100,pMVar3);
          pMVar13 = pMVar9 + 1;
          sscanf(buf_1,"%08x\n",pMVar9,pMVar12);
          uVar18 = extraout_EDX_18;
        }
        uVar16 = (2 < uVar4) + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3);
        pMVar9 = pMVar13 + 1;
        uVar4 = sscanf(buf_1,"%08x\n",pMVar13,uVar18);
      }
      uVar16 = uVar16 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar13 = pMVar9 + 1;
      iVar5 = sscanf(buf_1,"%08x\n",pMVar9,uVar4);
      if (uVar16 == uVar1) goto LAB_00014511;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar13,iVar5);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar9 = pMVar13 + 2;
      sscanf(buf_1,"%08x\n",pMVar13 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar12 = pMVar13 + 3;
      pMVar13 = pMVar13 + 4;
      sscanf(buf_1,"%08x\n",pMVar9);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar5 = sscanf(buf_1,"%08x\n",pMVar12);
    } while (uVar16 != uVar1);
  }
LAB_00014511:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar9 = constraints->lo;
  uVar16 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    uVar18 = extraout_EDX_05;
    if (uVar4 != 0) {
      pMVar13 = pMVar9;
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        pMVar12 = pMVar9;
        if (bVar2) {
          MeStreamReadLine(buf_1,0x100,pMVar3);
          pMVar12 = pMVar9 + 1;
          uVar4 = sscanf(buf_1,"%08x\n",pMVar9,uVar4);
          pMVar9 = pMVar3;
        }
        uVar16 = bVar2 + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3);
        pMVar13 = pMVar12 + 1;
        sscanf(buf_1,"%08x\n",pMVar12,uVar4);
      }
      uVar16 = uVar16 + 1;
      pMVar12 = pMVar9;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar9 = pMVar13 + 1;
      sscanf(buf_1,"%08x\n",pMVar13,pMVar12);
      uVar18 = extraout_EDX_06;
      if (uVar16 == uVar1) goto LAB_000146be;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar9,uVar18);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar13 = pMVar9 + 2;
      sscanf(buf_1,"%08x\n",pMVar9 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar12 = pMVar9 + 3;
      pMVar9 = pMVar9 + 4;
      sscanf(buf_1,"%08x\n",pMVar13);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar12);
      uVar18 = extraout_EDX_07;
    } while (uVar16 != uVar1);
  }
LAB_000146be:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar9 = constraints->hi;
  uVar16 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    iVar5 = 0;
    pMVar13 = pMVar9;
    if (uVar4 != 0) {
      if (1 < uVar4) {
        uVar18 = extraout_ECX_02;
        if (2 < uVar4) {
          uVar18 = 0;
          pMVar13 = pMVar9 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3);
          sscanf(buf_1,"%08x\n",pMVar9,uVar18);
          uVar18 = extraout_ECX_08;
        }
        uVar16 = (2 < uVar4) + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3);
        pMVar9 = pMVar13 + 1;
        uVar4 = sscanf(buf_1,"%08x\n",pMVar13,uVar18);
      }
      uVar16 = uVar16 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar13 = pMVar9 + 1;
      iVar5 = sscanf(buf_1,"%08x\n",pMVar9,uVar4);
      if (uVar16 == uVar1) goto LAB_0001486c;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar13,iVar5);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar9 = pMVar13 + 2;
      sscanf(buf_1,"%08x\n",pMVar13 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar12 = pMVar13 + 3;
      pMVar13 = pMVar13 + 4;
      sscanf(buf_1,"%08x\n",pMVar9);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar5 = sscanf(buf_1,"%08x\n",pMVar12);
    } while (uVar16 != uVar1);
  }
LAB_0001486c:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar9 = constraints->slipfactor;
  uVar16 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    uVar18 = extraout_ECX_03;
    if (uVar4 != 0) {
      pMVar13 = pMVar9;
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        pMVar12 = pMVar9;
        if (bVar2) {
          MeStreamReadLine(buf_1,0x100,pMVar3);
          pMVar12 = pMVar9 + 1;
          uVar4 = sscanf(buf_1,"%08x\n",pMVar9,uVar4);
        }
        uVar16 = bVar2 + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3);
        pMVar13 = pMVar12 + 1;
        uVar4 = sscanf(buf_1,"%08x\n",pMVar12,uVar4);
      }
      uVar16 = uVar16 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar9 = pMVar13 + 1;
      sscanf(buf_1,"%08x\n",pMVar13,uVar4);
      uVar18 = extraout_ECX_04;
      if (uVar16 == uVar1) goto LAB_00014a1c;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar9,uVar18);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar13 = pMVar9 + 2;
      sscanf(buf_1,"%08x\n",pMVar9 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar12 = pMVar9 + 3;
      pMVar9 = pMVar9 + 4;
      sscanf(buf_1,"%08x\n",pMVar13);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar12);
      uVar18 = extraout_ECX_05;
    } while (uVar16 != uVar1);
  }
LAB_00014a1c:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar9 = constraints->xgamma;
  uVar16 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    iVar5 = 0;
    if (uVar4 != 0) {
      uVar18 = extraout_EDX_08;
      pMVar13 = pMVar9;
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        pMVar12 = pMVar9;
        if (bVar2) {
          pMVar12 = pMVar9 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3);
          sscanf(buf_1,"%08x\n",pMVar9,uVar4);
        }
        uVar16 = bVar2 + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3);
        pMVar13 = pMVar12 + 1;
        sscanf(buf_1,"%08x\n",pMVar12,pMVar9);
        uVar18 = extraout_EDX_09;
      }
      uVar16 = uVar16 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar9 = pMVar13 + 1;
      iVar5 = sscanf(buf_1,"%08x\n",pMVar13,uVar18);
      if (uVar16 == uVar1) goto LAB_00014bcc;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar9,iVar5);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar13 = pMVar9 + 2;
      sscanf(buf_1,"%08x\n",pMVar9 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar12 = pMVar9 + 3;
      pMVar9 = pMVar9 + 4;
      sscanf(buf_1,"%08x\n",pMVar13);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar5 = sscanf(buf_1,"%08x\n",pMVar12);
    } while (uVar16 != uVar1);
  }
LAB_00014bcc:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_constraints;
  piVar10 = constraints->Jsize;
  uVar16 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    iVar5 = 0;
    if (uVar4 != 0) {
      piVar15 = piVar10;
      if (1 < uVar4) {
        uVar18 = extraout_EDX_10;
        piVar14 = piVar10;
        if (2 < uVar4) {
          MeStreamReadLine(buf_1,0x100,pMVar3);
          piVar14 = piVar10 + 1;
          sscanf(buf_1,"%d\n",piVar10,piVar15);
          uVar18 = extraout_EDX_17;
        }
        uVar16 = (2 < uVar4) + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3);
        piVar15 = piVar14 + 1;
        uVar4 = sscanf(buf_1,"%d\n",piVar14,uVar18);
      }
      uVar16 = uVar16 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar10 = piVar15 + 1;
      iVar5 = sscanf(buf_1,"%d\n",piVar15,uVar4);
      if (uVar16 == uVar1) goto LAB_00014d7c;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%d\n",piVar10,iVar5);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar15 = piVar10 + 2;
      sscanf(buf_1,"%d\n",piVar10 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar14 = piVar10 + 3;
      piVar10 = piVar10 + 4;
      sscanf(buf_1,"%d\n",piVar15);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar5 = sscanf(buf_1,"%d\n",piVar14);
    } while (uVar16 != uVar1);
  }
LAB_00014d7c:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_constraints;
  piVar10 = constraints->Jofs;
  uVar16 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    uVar18 = extraout_EDX_11;
    if (uVar4 != 0) {
      piVar15 = piVar10;
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        piVar14 = piVar10;
        if (bVar2) {
          piVar14 = piVar10 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3);
          uVar4 = sscanf(buf_1,"%d\n",piVar10,uVar4);
        }
        piVar10 = (int *)buf_1;
        uVar16 = bVar2 + 1;
        MeStreamReadLine(piVar10,0x100,pMVar3);
        piVar15 = piVar14 + 1;
        sscanf(buf_1,"%d\n",piVar14,uVar4);
      }
      uVar16 = uVar16 + 1;
      piVar14 = piVar10;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar10 = piVar15 + 1;
      sscanf(buf_1,"%d\n",piVar15,piVar14);
      uVar18 = extraout_EDX_12;
      if (uVar16 == uVar1) goto LAB_00014f2c;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%d\n",piVar10,uVar18);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar15 = piVar10 + 2;
      sscanf(buf_1,"%d\n",piVar10 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar14 = piVar10 + 3;
      piVar10 = piVar10 + 4;
      sscanf(buf_1,"%d\n",piVar15);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%d\n",piVar14);
      uVar18 = extraout_EDX_13;
    } while (uVar16 != uVar1);
  }
LAB_00014f2c:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  piVar10 = *constraints->Jbody;
  uVar1 = constraints->num_constraints * 2;
  uVar16 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3);
  uVar18 = extraout_EDX_14;
  if (uVar1 != 0) {
    uVar4 = uVar1 & 3;
    iVar5 = 0;
    if (uVar4 != 0) {
      piVar15 = piVar10;
      if (1 < uVar4) {
        uVar18 = extraout_ECX_06;
        piVar14 = piVar10;
        if (2 < uVar4) {
          uVar18 = 0;
          piVar14 = piVar10 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3);
          sscanf(buf_1,"%d\n",piVar10,uVar18);
          uVar18 = extraout_ECX_07;
        }
        uVar16 = (2 < uVar4) + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3);
        piVar15 = *(MdtKeaBodyIndexPair *)piVar14 + 1;
        uVar4 = sscanf(buf_1,"%d\n",piVar14,uVar18);
      }
      uVar16 = uVar16 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar10 = piVar15 + 1;
      iVar5 = sscanf(buf_1,"%d\n",piVar15,uVar4);
      uVar18 = extraout_EDX_15;
      if (uVar16 == uVar1) goto LAB_000150dc;
    }
    do {
      uVar16 = uVar16 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%d\n",piVar10,iVar5);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar5 = (int)(piVar10 + 2);
      sscanf(buf_1,"%d\n",piVar10 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar11 = (int)(piVar10 + 3);
      piVar10 = piVar10 + 4;
      sscanf(buf_1,"%d\n",iVar5);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar5 = sscanf(buf_1,"%d\n",iVar11);
      uVar18 = extraout_EDX_16;
    } while (uVar16 != uVar1);
  }
LAB_000150dc:
  MeStreamReadLine(buf_1,0x100,pMVar3);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%d\n",num_bodies,uVar18);
  i = 0;
  if (*num_bodies != 0) {
    local_258 = blist;
    do {
      pMVar7 = (MdtKeaBody *)(*_MeMemoryAPI)(0xf0);
      *local_258 = pMVar7;
      MeStreamReadLine(buf,0x100,pMVar3);
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",&pMVar7->invmass);
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%d\n",&pMVar7->flags);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->force);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->force + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->force + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->force + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->torque);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->torque + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->torque + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->torque + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI0);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI0 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI0 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI0 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI1 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI1 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI1 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI2 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI2 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->invI2 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I0);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I0 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I0 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I0 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I1 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I1 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I1 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I2 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I2 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->I2 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->vel);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->vel + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->vel + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->vel + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->velrot);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->velrot + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->velrot + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->velrot + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->qrot);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->qrot + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->qrot + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->qrot + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->accel);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->accel + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->accel + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
                    /* Unresolved local var: int i@[???] */
      sscanf(buf_1,"%08x\n",pMVar7->accel + 3);
      pMVar7 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->fastSpinAxis);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->fastSpinAxis + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->fastSpinAxis + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar7->fastSpinAxis + 3);
      i = i + 1;
      local_258 = local_258 + 1;
    } while (i != *num_bodies);
  }
  MeStreamClose(pMVar3);
  return;
}


/* ==== writeLambdaToFile ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void writeLambdaToFile(char *filename,MeReal *lambda,int num_elts)

{
  MeReal *pMVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar6;
  int iVar7;
  uint uVar8;
  char buf [256];
  
                    /* Unresolved local var: int file@[DW_OP_reg7(EDI)] */
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = 0;
  iVar3 = MeOpen(filename,1);
  printf("-- writing kea output data (lambda) to file %s\n",*(undefined4 *)(_gDebug + 0x1c));
  iVar4 = sprintf(buf,"%s\n","lambda");
  MeWrite(iVar3,buf,iVar4);
  if (num_elts != 0) {
    uVar5 = num_elts & 3;
    iVar4 = 0;
    if (uVar5 != 0) {
      uVar6 = extraout_ECX;
      if (1 < uVar5) {
        bVar2 = 2 < uVar5;
        if (bVar2) {
          iVar4 = sprintf(buf,"%08x\n",*lambda,uVar5);
          uVar5 = MeWrite(iVar3,buf,iVar4);
        }
        uVar8 = (uint)bVar2;
        iVar7 = uVar8 + 1;
        iVar4 = sprintf(buf,"%08x\n",lambda[uVar8],uVar5);
        MeWrite(iVar3,buf,iVar4);
        uVar6 = extraout_ECX_00;
      }
      pMVar1 = lambda + iVar7;
      iVar7 = iVar7 + 1;
      iVar4 = sprintf(buf,"%08x\n",*pMVar1,uVar6);
      iVar4 = MeWrite(iVar3,buf,iVar4);
      if (iVar7 == num_elts) goto LAB_00016344;
    }
    do {
      iVar4 = sprintf(buf,"%08x\n",lambda[iVar7],iVar4);
      MeWrite(iVar3,buf,iVar4);
      iVar4 = sprintf(buf,"%08x\n",lambda[iVar7 + 1]);
      MeWrite(iVar3,buf,iVar4);
      iVar4 = sprintf(buf,"%08x\n",lambda[iVar7 + 2]);
      MeWrite(iVar3,buf,iVar4);
      iVar4 = iVar7 + 3;
      iVar7 = iVar7 + 4;
      iVar4 = sprintf(buf,"%08x\n",lambda[iVar4]);
      iVar4 = MeWrite(iVar3,buf,iVar4);
    } while (iVar7 != num_elts);
  }
LAB_00016344:
  MeClose(iVar3);
  return;
}


/* ==== keaCloseDebugDataFile ==== */

/* DWARF original prototype: void keaCloseDebugDataFile(keaFunctions * this, int file) */

void keaFunctions::keaCloseDebugDataFile(int file)

{
  return;
}


/* ==== checkPrintDebugInput ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: int checkPrintDebugInput(keaFunctions * this, MdtKeaConstraints
   constraints, MdtKeaParameters parameters, MdtKeaBody * * blist, int num_bodies) */

int keaFunctions::checkPrintDebugInput
              (MdtKeaConstraints constraints,MdtKeaParameters parameters,MdtKeaBody **blist,
              int num_bodies)

{
  int iVar1;
  MeReal *pMVar2;
  int **ppiVar3;
  MeReal *pMVar4;
  int *piVar5;
  int in_stack_000000b4;
  MdtKeaConstraints in_stack_ffffff44;
  MdtKeaParameters in_stack_ffffffa0;
  
  if ((*_gDebug != 0) && (_gDebug[8] == _gDebug[9])) {
    pMVar2 = &parameters.epsilon;
    pMVar4 = (MeReal *)&stack0xffffffa0;
    for (iVar1 = 0x13; iVar1 != 0; iVar1 = iVar1 + -1) {
      *pMVar4 = *pMVar2;
      pMVar2 = pMVar2 + 1;
      pMVar4 = pMVar4 + 1;
    }
    ppiVar3 = (int **)&constraints.max_partitions;
    piVar5 = (int *)&stack0xffffff44;
    for (iVar1 = 0x17; iVar1 != 0; iVar1 = iVar1 + -1) {
      *piVar5 = (int)*ppiVar3;
      ppiVar3 = ppiVar3 + 1;
      piVar5 = piVar5 + 1;
    }
    writeKeaInputToFile(in_stack_ffffff44,in_stack_ffffffa0,(MdtKeaBody **)num_bodies,
                        in_stack_000000b4);
  }
  return 0;
}


