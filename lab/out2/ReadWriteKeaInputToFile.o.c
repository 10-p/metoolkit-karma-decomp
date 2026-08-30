/* ==== writeIntArrayToFile ==== */

void writeIntArrayToFile(int file,char *desc,int num_elts,int *array)

{
  int *piVar1;
  bool bVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  undefined4 extraout_ECX;
  int iVar6;
  uint uVar7;
  char buf [256];
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar6 = 0;
  iVar3 = sprintf(buf,"%s\n",desc);
  MeWrite(file,buf,iVar3);
  if (num_elts != 0) {
    uVar4 = num_elts & 3;
    uVar5 = 0;
    if (uVar4 != 0) {
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        if (bVar2) {
          iVar3 = sprintf(buf,"%d\n",*array,extraout_ECX);
          uVar4 = MeWrite(file,buf,iVar3);
        }
        uVar7 = (uint)bVar2;
        iVar6 = uVar7 + 1;
        iVar3 = sprintf(buf,"%d\n",array[uVar7],uVar4);
        uVar4 = MeWrite(file,buf,iVar3);
      }
      piVar1 = array + iVar6;
      iVar6 = iVar6 + 1;
      iVar3 = sprintf(buf,"%d\n",*piVar1,uVar4);
      uVar5 = MeWrite(file,buf,iVar3);
      if (iVar6 == num_elts) {
        return;
      }
    }
    do {
      iVar3 = sprintf(buf,"%d\n",array[iVar6],uVar5);
      MeWrite(file,buf,iVar3);
      iVar3 = sprintf(buf,"%d\n",array[iVar6 + 1]);
      MeWrite(file,buf,iVar3);
      iVar3 = sprintf(buf,"%d\n",array[iVar6 + 2]);
      MeWrite(file,buf,iVar3);
      iVar3 = iVar6 + 3;
      iVar6 = iVar6 + 4;
      iVar3 = sprintf(buf,"%d\n",array[iVar3]);
      uVar5 = MeWrite(file,buf,iVar3);
    } while (iVar6 != num_elts);
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
  undefined4 uVar6;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
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
    uVar6 = 0;
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
      uVar6 = MeWrite(file,buf,iVar4);
      if (uVar7 == uVar3) {
        return;
      }
    }
    do {
      iVar4 = sprintf(buf,"%d\n",(*array)[uVar7],uVar6);
      MeWrite(file,buf,iVar4);
      iVar4 = sprintf(buf,"%d\n",(*array)[uVar7 + 1]);
      MeWrite(file,buf,iVar4);
      iVar4 = sprintf(buf,"%d\n",array[1][uVar7]);
      MeWrite(file,buf,iVar4);
      iVar4 = uVar7 + 1;
      uVar7 = uVar7 + 4;
      iVar4 = sprintf(buf,"%d\n",array[1][iVar4]);
      uVar6 = MeWrite(file,buf,iVar4);
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
  undefined4 uVar5;
  undefined4 extraout_EDX;
  int iVar6;
  uint uVar7;
  char buf [256];
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar6 = 0;
  iVar3 = sprintf(buf,"%s\n",desc);
  MeWrite(file,buf,iVar3);
  if (num_elts != 0) {
    uVar4 = num_elts & 3;
    uVar5 = 0;
    if (uVar4 != 0) {
      if (1 < uVar4) {
        bVar2 = 2 < uVar4;
        if (bVar2) {
          iVar3 = sprintf(buf,"%08x\n",*array,extraout_EDX);
          uVar4 = MeWrite(file,buf,iVar3);
        }
        uVar7 = (uint)bVar2;
        iVar6 = uVar7 + 1;
        iVar3 = sprintf(buf,"%08x\n",array[uVar7],uVar4);
        uVar4 = MeWrite(file,buf,iVar3);
      }
      pMVar1 = array + iVar6;
      iVar6 = iVar6 + 1;
      iVar3 = sprintf(buf,"%08x\n",*pMVar1,uVar4);
      uVar5 = MeWrite(file,buf,iVar3);
      if (iVar6 == num_elts) {
        return;
      }
    }
    do {
      iVar3 = sprintf(buf,"%08x\n",array[iVar6],uVar5);
      MeWrite(file,buf,iVar3);
      iVar3 = sprintf(buf,"%08x\n",array[iVar6 + 1]);
      MeWrite(file,buf,iVar3);
      iVar3 = sprintf(buf,"%08x\n",array[iVar6 + 2]);
      MeWrite(file,buf,iVar3);
      iVar3 = iVar6 + 3;
      iVar6 = iVar6 + 4;
      iVar3 = sprintf(buf,"%08x\n",array[iVar3]);
      uVar5 = MeWrite(file,buf,iVar3);
    } while (iVar6 != num_elts);
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

/* WARNING: Unknown calling convention */

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
          MeStreamReadLine(buf,0x100,stream,uVar2);
          piVar5 = array + 1;
          uVar2 = sscanf(buf,"%d\n",array);
        }
        i = (int)bVar1;
        MeStreamReadLine(buf,0x100,stream,uVar2);
        piVar6 = piVar5 + 1;
        sscanf(buf,"%d\n",piVar5);
        i = i + 1;
        uVar4 = extraout_ECX_00;
      }
      MeStreamReadLine(buf,0x100,stream,uVar4);
      array = piVar6 + 1;
      iVar3 = sscanf(buf,"%d\n",piVar6);
      i = i + 1;
      if (i == num_elts) {
        return;
      }
    }
    do {
      MeStreamReadLine(buf,0x100,stream,iVar3);
      sscanf(buf,"%d\n",array);
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
  uint uVar2;
  MdtKeaBodyIndexPair *paiVar3;
  uint uVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  int i;
  char buf [256];
  
  uVar2 = num_elts * 2;
  MeStreamReadLine(buf,0x100,stream);
  i = 0;
  if (uVar2 != 0) {
    uVar4 = uVar2 & 3;
    iVar5 = 0;
    if (uVar4 != 0) {
      piVar7 = *array;
      if (1 < uVar4) {
        piVar6 = *array;
        if (2 < uVar4) {
          MeStreamReadLine(buf,0x100,stream,uVar4);
          piVar6 = *array + 1;
          sscanf(buf,"%d\n",array);
        }
        i = (int)(2 < uVar4);
        MeStreamReadLine(buf,0x100,stream);
        piVar7 = piVar6 + 1;
        sscanf(buf,"%d\n",piVar6);
        uVar4 = i + 1;
        i = uVar4;
      }
      MeStreamReadLine(buf,0x100,stream,uVar4);
      array = (MdtKeaBodyIndexPair *)(piVar7 + 1);
      iVar5 = sscanf(buf,"%d\n",piVar7);
      i = i + 1;
      if (i == uVar2) {
        return;
      }
    }
    do {
      MeStreamReadLine(buf,0x100,stream,iVar5);
      sscanf(buf,"%d\n",array);
      MeStreamReadLine(buf,0x100,stream);
      paiVar1 = array + 1;
      sscanf(buf,"%d\n",*array + 1);
      MeStreamReadLine(buf,0x100,stream);
      paiVar3 = array + 1;
      array = array + 2;
      sscanf(buf,"%d\n",paiVar1);
      MeStreamReadLine(buf,0x100,stream);
      iVar5 = sscanf(buf,"%d\n",*paiVar3 + 1);
      i = i + 4;
    } while (i != uVar2);
  }
  return;
}


/* ==== readFloatArrayFromFile ==== */

/* WARNING: Unknown calling convention */

void readFloatArrayFromFile(MeStream stream,char *desc,int num_elts,MeReal *array)

{
  bool bVar1;
  uint uVar2;
  undefined4 extraout_ECX;
  int unaff_EBX;
  MeReal *pMVar3;
  MeReal *pMVar4;
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
          MeStreamReadLine(buf,0x100,stream,extraout_ECX);
          pMVar3 = array + 1;
          uVar2 = sscanf(buf,"%08x\n",array);
        }
        i = (int)bVar1;
        MeStreamReadLine(buf,0x100,stream,uVar2);
        pMVar4 = pMVar3 + 1;
        uVar2 = sscanf(buf,"%08x\n",pMVar3);
        i = i + 1;
      }
      MeStreamReadLine(buf,0x100,stream,uVar2);
      array = pMVar4 + 1;
      sscanf(buf,"%08x\n",pMVar4);
      i = i + 1;
      unaff_EBX = num_elts;
      if (i == num_elts) {
        return;
      }
    }
    do {
      MeStreamReadLine(buf,0x100,stream,unaff_EBX);
      sscanf(buf,"%08x\n",array);
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
/* WARNING: Unknown calling convention */

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
  undefined4 uVar8;
  uint uVar9;
  undefined4 uVar10;
  uint uVar11;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 extraout_ECX_04;
  undefined4 extraout_ECX_05;
  undefined4 extraout_ECX_06;
  undefined4 extraout_ECX_07;
  undefined4 extraout_ECX_08;
  undefined4 extraout_ECX_09;
  undefined4 extraout_ECX_10;
  undefined4 extraout_ECX_11;
  undefined4 extraout_ECX_12;
  undefined4 extraout_ECX_13;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 extraout_EDX_01;
  undefined4 extraout_EDX_02;
  undefined4 extraout_EDX_03;
  undefined4 extraout_EDX_04;
  int iVar12;
  uint uVar13;
  uint uVar14;
  char *pcVar15;
  int iVar16;
  int iVar17;
  undefined8 uVar18;
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
  pcVar6 = (char *)MeOpen(*(undefined4 *)(_gDebug + 4),2);
  printf("-- writing kea input to file %s\n",*(undefined4 *)(_gDebug + 4));
  iVar7 = sprintf(buf_1,"%s\n","gamma");
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%08x\n",parameters.gamma);
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","epsilon");
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%08x\n",parameters.epsilon);
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","stepsize");
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%08x\n",parameters.stepsize);
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","max_iterations");
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",parameters.max_iterations);
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","num_partitions");
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.num_partitions);
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","max_partitions");
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.max_partitions);
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar12 = 0;
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","num_rows_exc_padding_partition");
  uVar8 = MeWrite(pcVar6,buf_1,iVar7);
  if (constraints.num_partitions != 0) {
    uVar9 = constraints.num_partitions & 3;
    uVar8 = 0;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        uVar8 = extraout_ECX;
        if (2 < uVar9) {
          iVar7 = sprintf(buf_1,"%d\n",*constraints.num_rows_exc_padding_partition,uVar9);
          MeWrite(pcVar6,buf_1,iVar7);
          uVar8 = extraout_ECX_13;
        }
        uVar9 = (uint)(2 < uVar9);
        iVar12 = uVar9 + 1;
        iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding_partition[uVar9],uVar8);
        uVar9 = MeWrite(pcVar6,buf_1,iVar7);
      }
      piVar1 = constraints.num_rows_exc_padding_partition + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar9);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_partitions) goto LAB_00011080;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding_partition[iVar12],uVar8);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding_partition[iVar12 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding_partition[iVar12 + 2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding_partition[iVar7]);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_partitions);
  }
LAB_00011080:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","num_rows_inc_padding_partition",uVar8);
  iVar12 = 0;
  uVar18 = MeWrite(pcVar6,buf_1,iVar7);
  uVar10 = (undefined4)((ulonglong)uVar18 >> 0x20);
  uVar8 = (undefined4)uVar18;
  if (constraints.num_partitions != 0) {
    uVar9 = constraints.num_partitions & 3;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        bVar5 = 2 < uVar9;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%d\n",*constraints.num_rows_inc_padding_partition,extraout_ECX_00);
          uVar9 = MeWrite(pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding_partition[uVar13],uVar9);
        uVar9 = MeWrite(pcVar6,buf_1,iVar7);
      }
      piVar1 = constraints.num_rows_inc_padding_partition + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar9);
      uVar18 = MeWrite(pcVar6,buf_1,iVar7);
      uVar10 = (undefined4)((ulonglong)uVar18 >> 0x20);
      uVar8 = (undefined4)uVar18;
      if (iVar12 == constraints.num_partitions) goto LAB_0001120b;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding_partition[iVar12],uVar10);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding_partition[iVar12 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding_partition[iVar12 + 2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding_partition[iVar7]);
      uVar18 = MeWrite(pcVar6,buf_1,iVar7);
      uVar10 = (undefined4)((ulonglong)uVar18 >> 0x20);
      uVar8 = (undefined4)uVar18;
    } while (iVar12 != constraints.num_partitions);
  }
LAB_0001120b:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","num_constraints_partition",uVar8);
  iVar12 = 0;
  uVar8 = MeWrite(pcVar6,buf_1,iVar7);
  if (constraints.num_partitions != 0) {
    uVar9 = constraints.num_partitions & 3;
    uVar8 = 0;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        bVar5 = 2 < uVar9;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%d\n",*constraints.num_constraints_partition,uVar9);
          uVar9 = MeWrite(pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints_partition[uVar13],uVar9);
        uVar9 = MeWrite(pcVar6,buf_1,iVar7);
      }
      piVar1 = constraints.num_constraints_partition + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar9);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_partitions) goto LAB_00011396;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints_partition[iVar12],uVar8);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints_partition[iVar12 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints_partition[iVar12 + 2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints_partition[iVar7]);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_partitions);
  }
LAB_00011396:
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","num_rows_inc_padding",uVar8);
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_inc_padding);
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","num_rows_exc_padding");
  MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%d\n",constraints.num_rows_exc_padding);
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","max_rows_inc_padding");
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.max_rows_inc_padding);
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","num_constraints");
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.num_constraints);
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%s\n","max_constraints");
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",constraints.max_constraints);
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf,"Jstore\n");
  MeWrite(pcVar6,buf,iVar7);
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
        MeWrite(pcVar6,buf,iVar7);
        iVar7 = sprintf(buf,"%08x; ",(*constraints.Jstore)[0].col[1][iVar16]);
        MeWrite(pcVar6,buf,iVar7);
        pMVar2 = (*constraints.Jstore)[0].col[2] + iVar16;
        iVar16 = iVar16 + 0xc;
        iVar7 = sprintf(buf,"%08x; ",*pMVar2);
        iVar7 = MeWrite(pcVar6,buf,iVar7);
      } while (iVar12 != 6);
      iVar16 = 0;
      iVar12 = local_238;
      do {
        iVar17 = j * 4;
        iVar16 = iVar16 + 3;
        iVar7 = sprintf(buf,"%08x; ",
                        *(undefined4 *)((int)(*constraints.Jstore)[1].col[0] + iVar12 + iVar17),
                        iVar7);
        MeWrite(pcVar6,buf,iVar7);
        iVar7 = sprintf(buf,"%08x; ",
                        *(undefined4 *)((int)(*constraints.Jstore)[1].col[1] + iVar17 + iVar12));
        MeWrite(pcVar6,buf,iVar7);
        iVar17 = iVar17 + iVar12;
        iVar12 = iVar12 + 0x30;
        iVar7 = sprintf(buf,"%08x; ",*(undefined4 *)((int)(*constraints.Jstore)[1].col[2] + iVar17))
        ;
        iVar7 = MeWrite(pcVar6,buf,iVar7);
      } while (iVar16 != 6);
      iVar7 = sprintf(buf,"\n",6,6);
      iVar7 = MeWrite(pcVar6,buf,iVar7);
      j = j + 1;
    } while (j != 4);
    iVar7 = sprintf(buf,"\n",iVar7,iVar7);
    MeWrite(pcVar6,buf,iVar7);
    local_238 = local_238 + 0xc0;
    local_23c = local_23c + 0x30;
    i = i + 1;
  }
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n",&DAT_000164e6,iVar7);
  iVar12 = 0;
  uVar18 = MeWrite(pcVar6,buf_1,iVar7);
  uVar10 = (undefined4)((ulonglong)uVar18 >> 0x20);
  uVar8 = (undefined4)uVar18;
  if (constraints.num_rows_exc_padding != 0) {
    uVar9 = constraints.num_rows_exc_padding & 3;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        bVar5 = 2 < uVar9;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.xi,extraout_ECX_01);
          uVar9 = MeWrite(pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.xi[uVar13],uVar9);
        uVar9 = MeWrite(pcVar6,buf_1,iVar7);
      }
      pMVar2 = constraints.xi + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar9);
      uVar18 = MeWrite(pcVar6,buf_1,iVar7);
      uVar10 = (undefined4)((ulonglong)uVar18 >> 0x20);
      uVar8 = (undefined4)uVar18;
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_0001199b;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xi[iVar12],uVar10);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xi[iVar12 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xi[iVar12 + 2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xi[iVar7]);
      uVar18 = MeWrite(pcVar6,buf_1,iVar7);
      uVar10 = (undefined4)((ulonglong)uVar18 >> 0x20);
      uVar8 = (undefined4)uVar18;
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_0001199b:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n",&DAT_000164e9,uVar8);
  iVar12 = 0;
  uVar8 = MeWrite(pcVar6,buf_1,iVar7);
  if (constraints.num_rows_exc_padding != 0) {
    uVar9 = constraints.num_rows_exc_padding & 3;
    uVar8 = 0;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        bVar5 = 2 < uVar9;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.c,uVar9);
          uVar9 = MeWrite(pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.c[uVar13],uVar9);
        uVar9 = MeWrite(pcVar6,buf_1,iVar7);
      }
      pMVar2 = constraints.c + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar9);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_00011b26;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.c[iVar12],uVar8);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.c[iVar12 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.c[iVar12 + 2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.c[iVar7]);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_00011b26:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n",&DAT_000164eb,uVar8);
  iVar12 = 0;
  MeWrite(pcVar6,buf_1,iVar7);
  uVar8 = extraout_EDX;
  if (constraints.num_rows_exc_padding != 0) {
    uVar9 = constraints.num_rows_exc_padding & 3;
    uVar10 = extraout_ECX_02;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        bVar5 = 2 < uVar9;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.lo,0);
          uVar9 = MeWrite(pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.lo[uVar13],uVar9);
        uVar9 = MeWrite(pcVar6,buf_1,iVar7);
      }
      pMVar2 = constraints.lo + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar9);
      MeWrite(pcVar6,buf_1,iVar7);
      uVar10 = extraout_ECX_03;
      uVar8 = extraout_EDX_00;
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_00011cb1;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.lo[iVar12],uVar10);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.lo[iVar12 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.lo[iVar12 + 2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.lo[iVar7]);
      MeWrite(pcVar6,buf_1,iVar7);
      uVar10 = extraout_ECX_04;
      uVar8 = extraout_EDX_01;
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_00011cb1:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n",&DAT_000164ee,uVar8);
  iVar12 = 0;
  uVar18 = MeWrite(pcVar6,buf_1,iVar7);
  uVar10 = (undefined4)((ulonglong)uVar18 >> 0x20);
  uVar8 = (undefined4)uVar18;
  if (constraints.num_rows_exc_padding != 0) {
    uVar9 = constraints.num_rows_exc_padding & 3;
    uVar8 = 0;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        bVar5 = 2 < uVar9;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.hi,uVar9);
          uVar9 = MeWrite(pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.hi[uVar13],uVar9);
        MeWrite(pcVar6,buf_1,iVar7);
        uVar10 = extraout_EDX_02;
      }
      pMVar2 = constraints.hi + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar10);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_00011e3c;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.hi[iVar12],uVar8);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.hi[iVar12 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.hi[iVar12 + 2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.hi[iVar7]);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_00011e3c:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","slipfactor",uVar8);
  iVar12 = 0;
  MeWrite(pcVar6,buf_1,iVar7);
  uVar8 = extraout_ECX_05;
  if (constraints.num_rows_exc_padding != 0) {
    uVar9 = constraints.num_rows_exc_padding & 3;
    uVar10 = 0;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        uVar8 = extraout_EDX_03;
        if (2 < uVar9) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.slipfactor,uVar9);
          MeWrite(pcVar6,buf_1,iVar7);
          uVar8 = extraout_EDX_04;
        }
        uVar9 = (uint)(2 < uVar9);
        iVar12 = uVar9 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.slipfactor[uVar9],uVar8);
        uVar9 = MeWrite(pcVar6,buf_1,iVar7);
      }
      pMVar2 = constraints.slipfactor + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar9);
      uVar10 = MeWrite(pcVar6,buf_1,iVar7);
      uVar8 = extraout_ECX_06;
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_00011fc7;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.slipfactor[iVar12],uVar10);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.slipfactor[iVar12 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.slipfactor[iVar12 + 2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.slipfactor[iVar7]);
      uVar10 = MeWrite(pcVar6,buf_1,iVar7);
      uVar8 = extraout_ECX_07;
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_00011fc7:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","xgamma",uVar8);
  iVar12 = 0;
  uVar8 = MeWrite(pcVar6,buf_1,iVar7);
  if (constraints.num_rows_exc_padding != 0) {
    uVar9 = constraints.num_rows_exc_padding & 3;
    uVar8 = 0;
    if (uVar9 != 0) {
      uVar8 = extraout_ECX_08;
      if (1 < uVar9) {
        bVar5 = 2 < uVar9;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%08x\n",*constraints.xgamma,uVar9);
          uVar9 = MeWrite(pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%08x\n",constraints.xgamma[uVar13],uVar9);
        MeWrite(pcVar6,buf_1,iVar7);
        uVar8 = extraout_ECX_09;
      }
      pMVar2 = constraints.xgamma + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%08x\n",*pMVar2,uVar8);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_rows_exc_padding) goto LAB_00012152;
    }
    do {
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xgamma[iVar12],uVar8);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xgamma[iVar12 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xgamma[iVar12 + 2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%08x\n",constraints.xgamma[iVar7]);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_rows_exc_padding);
  }
LAB_00012152:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","Jsize",uVar8);
  iVar12 = 0;
  uVar8 = MeWrite(pcVar6,buf_1,iVar7);
  if (constraints.num_constraints != 0) {
    uVar9 = constraints.num_constraints & 3;
    uVar8 = 0;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        uVar8 = extraout_ECX_10;
        if (2 < uVar9) {
          iVar7 = sprintf(buf_1,"%d\n",*constraints.Jsize,uVar9);
          MeWrite(pcVar6,buf_1,iVar7);
          uVar8 = extraout_ECX_12;
        }
        uVar9 = (uint)(2 < uVar9);
        iVar12 = uVar9 + 1;
        iVar7 = sprintf(buf_1,"%d\n",constraints.Jsize[uVar9],uVar8);
        uVar9 = MeWrite(pcVar6,buf_1,iVar7);
      }
      piVar1 = constraints.Jsize + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar9);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
      if (iVar12 == constraints.num_constraints) goto LAB_000122dd;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jsize[iVar12],uVar8);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jsize[iVar12 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jsize[iVar12 + 2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jsize[iVar7]);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
    } while (iVar12 != constraints.num_constraints);
  }
LAB_000122dd:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n",&DAT_00016509,uVar8);
  iVar12 = 0;
  uVar18 = MeWrite(pcVar6,buf_1,iVar7);
  uVar10 = (undefined4)((ulonglong)uVar18 >> 0x20);
  uVar8 = (undefined4)uVar18;
  if (constraints.num_constraints != 0) {
    uVar9 = constraints.num_constraints & 3;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        bVar5 = 2 < uVar9;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%d\n",*constraints.Jofs,extraout_ECX_11);
          uVar9 = MeWrite(pcVar6,buf_1,iVar7);
        }
        uVar13 = (uint)bVar5;
        iVar12 = uVar13 + 1;
        iVar7 = sprintf(buf_1,"%d\n",constraints.Jofs[uVar13],uVar9);
        uVar9 = MeWrite(pcVar6,buf_1,iVar7);
      }
      piVar1 = constraints.Jofs + iVar12;
      iVar12 = iVar12 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar9);
      uVar18 = MeWrite(pcVar6,buf_1,iVar7);
      uVar10 = (undefined4)((ulonglong)uVar18 >> 0x20);
      uVar8 = (undefined4)uVar18;
      if (iVar12 == constraints.num_constraints) goto LAB_00012468;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jofs[iVar12],uVar10);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jofs[iVar12 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jofs[iVar12 + 2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = iVar12 + 3;
      iVar12 = iVar12 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jofs[iVar7]);
      uVar18 = MeWrite(pcVar6,buf_1,iVar7);
      uVar10 = (undefined4)((ulonglong)uVar18 >> 0x20);
      uVar8 = (undefined4)uVar18;
    } while (iVar12 != constraints.num_constraints);
  }
LAB_00012468:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  uVar9 = constraints.num_constraints * 2;
  iVar7 = sprintf(buf_1,"%s\n","Jbody",uVar8);
  uVar13 = 0;
  uVar8 = MeWrite(pcVar6,buf_1,iVar7);
  if (uVar9 != 0) {
    uVar11 = uVar9 & 3;
    uVar8 = 0;
    if (uVar11 != 0) {
      if (1 < uVar11) {
        bVar5 = 2 < uVar11;
        if (bVar5) {
          iVar7 = sprintf(buf_1,"%d\n",(*constraints.Jbody)[0],uVar11);
          uVar11 = MeWrite(pcVar6,buf_1,iVar7);
        }
        uVar14 = (uint)bVar5;
        uVar13 = uVar14 + 1;
        iVar7 = sprintf(buf_1,"%d\n",(*constraints.Jbody)[uVar14],uVar11);
        uVar11 = MeWrite(pcVar6,buf_1,iVar7);
      }
      piVar1 = *constraints.Jbody + uVar13;
      uVar13 = uVar13 + 1;
      iVar7 = sprintf(buf_1,"%d\n",*piVar1,uVar11);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
      if (uVar13 == uVar9) goto LAB_000125f8;
    }
    do {
      iVar7 = sprintf(buf_1,"%d\n",(*constraints.Jbody)[uVar13],uVar8);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",(*constraints.Jbody)[uVar13 + 1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jbody[1][uVar13]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = uVar13 + 1;
      uVar13 = uVar13 + 4;
      iVar7 = sprintf(buf_1,"%d\n",constraints.Jbody[1][iVar7]);
      uVar8 = MeWrite(pcVar6,buf_1,iVar7);
    } while (uVar13 != uVar9);
  }
LAB_000125f8:
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = sprintf(buf_1,"%s\n","num_bodies",uVar8);
  MeWrite(pcVar6,buf_1,iVar7);
  iVar7 = sprintf(buf_1,"%d\n",num_bodies);
  MeWrite(pcVar6,buf_1,iVar7);
  i = 0;
  if (num_bodies != 0) {
    local_230 = blist;
    pcVar15 = pcVar6;
    do {
      iVar7 = sprintf(buf,"body %d\n",i,pcVar15);
      MeWrite(pcVar6,buf,iVar7);
      MVar3 = (*local_230)->invmass;
      iVar7 = sprintf(buf_1,"%s\n","invmass");
      MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",MVar3);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = (*local_230)->flags;
      iVar12 = sprintf(buf_1,"%s\n","flags");
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      MeWrite(pcVar6,buf_1,iVar12);
      iVar7 = sprintf(buf_1,"%d\n",iVar7);
      MeWrite(pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","force");
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->force[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->force[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->force[2]);
      MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->force[3]);
      MeWrite(pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","torque");
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->torque[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->torque[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->torque[2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->torque[3]);
      MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","invI0");
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI0[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI0[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI0[2]);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI0[3]);
      MeWrite(pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","invI1");
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI1[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI1[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI1[2]);
      MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI1[3]);
      MeWrite(pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","invI2");
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI2[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI2[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI2[2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->invI2[3]);
      MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n",&DAT_00016555);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I0[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I0[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I0[2]);
      MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I0[3]);
      MeWrite(pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n",&DAT_00016558);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I1[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I1[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I1[2]);
      MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I1[3]);
      MeWrite(pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n",&DAT_0001655b);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I2[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I2[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I2[2]);
      MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->I2[3]);
      MeWrite(pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n",&DAT_0001655e);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->vel[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->vel[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->vel[2]);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->vel[3]);
      MeWrite(pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","velrot");
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->velrot[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->velrot[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->velrot[2]);
      MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->velrot[3]);
      MeWrite(pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n",&DAT_00016569);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->qrot[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->qrot[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->qrot[2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->qrot[3]);
      MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","accel");
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->accel[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->accel[1]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->accel[2]);
      MeWrite(pcVar6,buf_1,iVar7);
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->accel[3]);
      MeWrite(pcVar6,buf_1,iVar7);
      pMVar4 = *local_230;
      iVar7 = sprintf(buf_1,"%s\n","fastSpinAxis");
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->fastSpinAxis[0]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->fastSpinAxis[1]);
      pcVar15 = buf_1;
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->fastSpinAxis[2]);
      MeWrite(pcVar6,buf_1,iVar7);
      iVar7 = sprintf(buf_1,"%08x\n",pMVar4->fastSpinAxis[3]);
      MeWrite(pcVar6,pcVar15,iVar7);
      i = i + 1;
      local_230 = local_230 + 1;
    } while (i != num_bodies);
  }
  MeClose(pcVar6);
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
  int iVar4;
  uint uVar5;
  MdtKeaBody *pMVar6;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 extraout_ECX_04;
  undefined4 extraout_ECX_05;
  undefined4 extraout_ECX_06;
  undefined4 uVar7;
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
  undefined4 extraout_EDX_19;
  undefined4 extraout_EDX_20;
  undefined4 extraout_EDX_21;
  MeReal *pMVar8;
  int *piVar9;
  int iVar10;
  MeReal *pMVar11;
  MeReal *pMVar12;
  int *piVar13;
  int *piVar14;
  uint uVar15;
  int iVar16;
  undefined8 uVar17;
  int local_25c;
  MdtKeaBody **local_258;
  MeStream stream;
  int i;
  char buf_1 [256];
  char buf [256];
  
                    /* Unresolved local var: int j@[DW_OP_reg7(EDI)]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  pMVar3 = (MeReal *)MeStreamOpen(*(undefined4 *)(_gDebug + 0xc),0);
  if (pMVar3 == (MeReal *)0x0) {
    MeFatalError(0,"file %s could not be opened",*(undefined4 *)(_gDebug + 0xc),parameters);
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
  piVar9 = constraints->num_rows_exc_padding_partition;
  uVar15 = 0;
  iVar4 = MeStreamReadLine(buf_1,0x100,pMVar3);
  if (uVar1 != 0) {
    uVar5 = uVar1 & 3;
    uVar7 = extraout_ECX;
    if (uVar5 != 0) {
      piVar14 = piVar9;
      if (1 < uVar5) {
        bVar2 = 2 < uVar5;
        piVar13 = piVar9;
        if (bVar2) {
          piVar13 = piVar9 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
          uVar5 = sscanf(buf_1,"%d\n",piVar9);
        }
        uVar15 = bVar2 + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
        piVar14 = piVar13 + 1;
        uVar5 = sscanf(buf_1,"%d\n",piVar13);
      }
      uVar15 = uVar15 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
      piVar9 = piVar14 + 1;
      iVar4 = sscanf(buf_1,"%d\n",piVar14);
      uVar7 = extraout_ECX_00;
      if (uVar15 == uVar1) goto LAB_00013b0c;
    }
    do {
      uVar15 = uVar15 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
      sscanf(buf_1,"%d\n",piVar9);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar14 = piVar9 + 2;
      sscanf(buf_1,"%d\n",piVar9 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar13 = piVar9 + 3;
      piVar9 = piVar9 + 4;
      sscanf(buf_1,"%d\n",piVar14);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar4 = sscanf(buf_1,"%d\n",piVar13);
      uVar7 = extraout_ECX_01;
    } while (uVar15 != uVar1);
  }
LAB_00013b0c:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_partitions;
  piVar9 = constraints->num_rows_inc_padding_partition;
  uVar15 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3,iVar4);
  piVar14 = (int *)buf_1;
  if (uVar1 != 0) {
    uVar5 = uVar1 & 3;
    iVar4 = 0;
    if (uVar5 != 0) {
      uVar7 = extraout_EDX;
      piVar14 = piVar9;
      if (1 < uVar5) {
        piVar13 = piVar9;
        if (2 < uVar5) {
          piVar13 = piVar9 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
          sscanf(buf_1,"%d\n",piVar9);
        }
        uVar15 = (2 < uVar5) + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3,piVar9);
        piVar14 = piVar13 + 1;
        sscanf(buf_1,"%d\n",piVar13);
        uVar7 = extraout_EDX_00;
      }
      uVar15 = uVar15 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
      piVar9 = piVar14 + 1;
      iVar4 = sscanf(buf_1,"%d\n",piVar14);
      piVar14 = piVar9;
      if (uVar15 == uVar1) goto LAB_00013cbc;
    }
    do {
      uVar15 = uVar15 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,iVar4);
      sscanf(buf_1,"%d\n",piVar9);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar14 = piVar9 + 2;
      sscanf(buf_1,"%d\n",piVar9 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar13 = piVar9 + 3;
      piVar9 = piVar9 + 4;
      sscanf(buf_1,"%d\n",piVar14);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar4 = sscanf(buf_1,"%d\n",piVar13);
      piVar14 = piVar9;
    } while (uVar15 != uVar1);
  }
LAB_00013cbc:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_partitions;
  piVar9 = constraints->num_constraints_partition;
  uVar15 = 0;
  uVar17 = MeStreamReadLine(buf_1,0x100,pMVar3,piVar14);
  iVar4 = (int)uVar17;
  if (uVar1 != 0) {
    uVar5 = uVar1 & 3;
    iVar4 = 0;
    if (uVar5 != 0) {
      piVar14 = piVar9;
      if (1 < uVar5) {
        bVar2 = 2 < uVar5;
        piVar13 = piVar9;
        if (bVar2) {
          MeStreamReadLine(buf_1,0x100,pMVar3,(int)((ulonglong)uVar17 >> 0x20));
          piVar13 = piVar9 + 1;
          uVar5 = sscanf(buf_1,"%d\n",piVar9);
        }
        uVar15 = bVar2 + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
        piVar14 = piVar13 + 1;
        uVar5 = sscanf(buf_1,"%d\n",piVar13);
      }
      uVar15 = uVar15 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
      piVar9 = piVar14 + 1;
      iVar4 = sscanf(buf_1,"%d\n",piVar14);
      if (uVar15 == uVar1) goto LAB_00013e6c;
    }
    do {
      uVar15 = uVar15 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,iVar4);
      sscanf(buf_1,"%d\n",piVar9);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar14 = piVar9 + 2;
      sscanf(buf_1,"%d\n",piVar9 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar13 = piVar9 + 3;
      piVar9 = piVar9 + 4;
      sscanf(buf_1,"%d\n",piVar14);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar4 = sscanf(buf_1,"%d\n",piVar13);
    } while (uVar15 != uVar1);
  }
LAB_00013e6c:
  MeStreamReadLine(buf_1,0x100,pMVar3,iVar4);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%d\n",&constraints->num_rows_inc_padding);
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
    iVar4 = constraints->num_rows_inc_padding;
    if (iVar4 < 0) {
      iVar4 = iVar4 + 3;
    }
    iVar4 = iVar4 >> 2;
    if (i == iVar4) break;
    iVar16 = 0;
    iVar10 = local_25c;
    do {
      iVar16 = iVar16 + 2;
      MeStreamReadLine(buf,0x100,pMVar3,iVar4);
      iVar4 = (int)(*constraints->Jstore)[0].col[0] + iVar10;
      sscanf(buf,"%08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x;\n",iVar4,
             iVar4 + 0x300,iVar4 + 0x600,iVar4 + 0x900,iVar4 + 0xc00,iVar4 + 0xf00,iVar4 + 0x1200,
             iVar4 + 0x1500,iVar4 + 0x1800,iVar4 + 0x1b00,iVar4 + 0x1e00,iVar4 + 0x2100);
      MeStreamReadLine(buf,0x100,pMVar3);
      iVar4 = (int)constraints->Jstore[1][0].col[0] + iVar10;
      iVar4 = sscanf(buf,"%08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x;\n"
                     ,iVar4,iVar4 + 0x300,iVar4 + 0x600,iVar4 + 0x900,iVar4 + 0xc00,iVar4 + 0xf00,
                     iVar4 + 0x1200,iVar4 + 0x1500,iVar4 + 0x1800,iVar4 + 0x1b00,iVar4 + 0x1e00,
                     iVar4 + 0x2100);
      iVar10 = iVar10 + 0x180;
    } while (iVar16 != 4);
    MeStreamReadLine(buf,0x100,pMVar3,extraout_EDX_01);
    local_25c = local_25c + 0x2400;
    i = i + 1;
  }
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar8 = constraints->xi;
  uVar15 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3,constraints);
  if (uVar1 != 0) {
    uVar5 = uVar1 & 3;
    iVar4 = 0;
    pMVar12 = pMVar8;
    if (uVar5 != 0) {
      uVar7 = extraout_EDX_02;
      if (1 < uVar5) {
        if (2 < uVar5) {
          pMVar12 = pMVar8 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
          sscanf(buf_1,"%08x\n",pMVar8);
        }
        uVar15 = (2 < uVar5) + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3,pMVar8);
        pMVar8 = pMVar12 + 1;
        sscanf(buf_1,"%08x\n",pMVar12);
        uVar7 = extraout_EDX_03;
      }
      uVar15 = uVar15 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
      pMVar12 = pMVar8 + 1;
      iVar4 = sscanf(buf_1,"%08x\n",pMVar8);
      if (uVar15 == uVar1) goto LAB_00014364;
    }
    do {
      uVar15 = uVar15 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,iVar4);
      sscanf(buf_1,"%08x\n",pMVar12);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar8 = pMVar12 + 2;
      sscanf(buf_1,"%08x\n",pMVar12 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar11 = pMVar12 + 3;
      pMVar12 = pMVar12 + 4;
      sscanf(buf_1,"%08x\n",pMVar8);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar4 = sscanf(buf_1,"%08x\n",pMVar11);
    } while (uVar15 != uVar1);
  }
LAB_00014364:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar8 = constraints->c;
  uVar5 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3,uVar15);
  if (uVar1 != 0) {
    uVar15 = uVar1 & 3;
    iVar4 = 0;
    pMVar12 = pMVar8;
    if (uVar15 != 0) {
      if (1 < uVar15) {
        uVar7 = extraout_EDX_04;
        if (2 < uVar15) {
          MeStreamReadLine(buf_1,0x100,pMVar3,pMVar8);
          pMVar12 = pMVar8 + 1;
          sscanf(buf_1,"%08x\n",pMVar8);
          uVar7 = extraout_EDX_21;
        }
        uVar5 = (2 < uVar15) + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
        pMVar8 = pMVar12 + 1;
        uVar15 = sscanf(buf_1,"%08x\n",pMVar12);
      }
      uVar5 = uVar5 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar15);
      pMVar12 = pMVar8 + 1;
      iVar4 = sscanf(buf_1,"%08x\n",pMVar8);
      if (uVar5 == uVar1) goto LAB_00014511;
    }
    do {
      uVar5 = uVar5 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,iVar4);
      sscanf(buf_1,"%08x\n",pMVar12);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar8 = pMVar12 + 2;
      sscanf(buf_1,"%08x\n",pMVar12 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar11 = pMVar12 + 3;
      pMVar12 = pMVar12 + 4;
      sscanf(buf_1,"%08x\n",pMVar8);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar4 = sscanf(buf_1,"%08x\n",pMVar11);
    } while (uVar5 != uVar1);
  }
LAB_00014511:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar8 = constraints->lo;
  uVar15 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3,constraints);
  if (uVar1 != 0) {
    uVar5 = uVar1 & 3;
    uVar7 = extraout_EDX_05;
    if (uVar5 != 0) {
      pMVar12 = pMVar8;
      if (1 < uVar5) {
        bVar2 = 2 < uVar5;
        pMVar11 = pMVar8;
        if (bVar2) {
          MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
          pMVar11 = pMVar8 + 1;
          uVar5 = sscanf(buf_1,"%08x\n",pMVar8);
          pMVar8 = pMVar3;
        }
        uVar15 = bVar2 + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
        pMVar12 = pMVar11 + 1;
        sscanf(buf_1,"%08x\n",pMVar11);
      }
      uVar15 = uVar15 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,pMVar8);
      pMVar8 = pMVar12 + 1;
      sscanf(buf_1,"%08x\n",pMVar12);
      uVar7 = extraout_EDX_06;
      if (uVar15 == uVar1) goto LAB_000146be;
    }
    do {
      uVar15 = uVar15 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
      sscanf(buf_1,"%08x\n",pMVar8);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar12 = pMVar8 + 2;
      sscanf(buf_1,"%08x\n",pMVar8 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar11 = pMVar8 + 3;
      pMVar8 = pMVar8 + 4;
      sscanf(buf_1,"%08x\n",pMVar12);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar11);
      uVar7 = extraout_EDX_07;
    } while (uVar15 != uVar1);
  }
LAB_000146be:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar8 = constraints->hi;
  uVar15 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3,uVar1);
  uVar7 = extraout_EDX_08;
  if (uVar1 != 0) {
    uVar5 = uVar1 & 3;
    iVar4 = 0;
    pMVar12 = pMVar8;
    if (uVar5 != 0) {
      if (1 < uVar5) {
        uVar7 = extraout_ECX_02;
        if (2 < uVar5) {
          pMVar12 = pMVar8 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3,0);
          sscanf(buf_1,"%08x\n",pMVar8);
          uVar7 = extraout_ECX_08;
        }
        uVar15 = (2 < uVar5) + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
        pMVar8 = pMVar12 + 1;
        uVar5 = sscanf(buf_1,"%08x\n",pMVar12);
      }
      uVar15 = uVar15 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
      pMVar12 = pMVar8 + 1;
      iVar4 = sscanf(buf_1,"%08x\n",pMVar8);
      uVar7 = extraout_EDX_09;
      if (uVar15 == uVar1) goto LAB_0001486c;
    }
    do {
      uVar15 = uVar15 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,iVar4);
      sscanf(buf_1,"%08x\n",pMVar12);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar8 = pMVar12 + 2;
      sscanf(buf_1,"%08x\n",pMVar12 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar11 = pMVar12 + 3;
      pMVar12 = pMVar12 + 4;
      sscanf(buf_1,"%08x\n",pMVar8);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar4 = sscanf(buf_1,"%08x\n",pMVar11);
      uVar7 = extraout_EDX_10;
    } while (uVar15 != uVar1);
  }
LAB_0001486c:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar8 = constraints->slipfactor;
  uVar15 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
  if (uVar1 != 0) {
    uVar5 = uVar1 & 3;
    uVar7 = extraout_ECX_03;
    if (uVar5 != 0) {
      pMVar12 = pMVar8;
      if (1 < uVar5) {
        bVar2 = 2 < uVar5;
        pMVar11 = pMVar8;
        if (bVar2) {
          MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
          pMVar11 = pMVar8 + 1;
          uVar5 = sscanf(buf_1,"%08x\n",pMVar8);
        }
        uVar15 = bVar2 + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
        pMVar12 = pMVar11 + 1;
        uVar5 = sscanf(buf_1,"%08x\n",pMVar11);
      }
      uVar15 = uVar15 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
      pMVar8 = pMVar12 + 1;
      sscanf(buf_1,"%08x\n",pMVar12);
      uVar7 = extraout_ECX_04;
      if (uVar15 == uVar1) goto LAB_00014a1c;
    }
    do {
      uVar15 = uVar15 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
      sscanf(buf_1,"%08x\n",pMVar8);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar12 = pMVar8 + 2;
      sscanf(buf_1,"%08x\n",pMVar8 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar11 = pMVar8 + 3;
      pMVar8 = pMVar8 + 4;
      sscanf(buf_1,"%08x\n",pMVar12);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar11);
      uVar7 = extraout_ECX_05;
    } while (uVar15 != uVar1);
  }
LAB_00014a1c:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_rows_exc_padding;
  pMVar8 = constraints->xgamma;
  uVar15 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3,constraints);
  if (uVar1 != 0) {
    uVar5 = uVar1 & 3;
    iVar4 = 0;
    if (uVar5 != 0) {
      uVar7 = extraout_EDX_11;
      pMVar12 = pMVar8;
      if (1 < uVar5) {
        pMVar11 = pMVar8;
        if (2 < uVar5) {
          pMVar11 = pMVar8 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
          sscanf(buf_1,"%08x\n",pMVar8);
        }
        uVar15 = (2 < uVar5) + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3,pMVar8);
        pMVar12 = pMVar11 + 1;
        sscanf(buf_1,"%08x\n",pMVar11);
        uVar7 = extraout_EDX_12;
      }
      uVar15 = uVar15 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
      pMVar8 = pMVar12 + 1;
      iVar4 = sscanf(buf_1,"%08x\n",pMVar12);
      if (uVar15 == uVar1) goto LAB_00014bcc;
    }
    do {
      uVar15 = uVar15 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,iVar4);
      sscanf(buf_1,"%08x\n",pMVar8);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar12 = pMVar8 + 2;
      sscanf(buf_1,"%08x\n",pMVar8 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      pMVar11 = pMVar8 + 3;
      pMVar8 = pMVar8 + 4;
      sscanf(buf_1,"%08x\n",pMVar12);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar4 = sscanf(buf_1,"%08x\n",pMVar11);
    } while (uVar15 != uVar1);
  }
LAB_00014bcc:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_constraints;
  piVar9 = constraints->Jsize;
  uVar5 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3,uVar15);
  if (uVar1 != 0) {
    uVar15 = uVar1 & 3;
    iVar4 = 0;
    if (uVar15 != 0) {
      piVar14 = piVar9;
      if (1 < uVar15) {
        uVar7 = extraout_EDX_13;
        piVar13 = piVar9;
        if (2 < uVar15) {
          MeStreamReadLine(buf_1,0x100,pMVar3,piVar9);
          piVar13 = piVar9 + 1;
          sscanf(buf_1,"%d\n",piVar9);
          uVar7 = extraout_EDX_20;
        }
        uVar5 = (2 < uVar15) + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
        piVar14 = piVar13 + 1;
        uVar15 = sscanf(buf_1,"%d\n",piVar13);
      }
      uVar5 = uVar5 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar15);
      piVar9 = piVar14 + 1;
      iVar4 = sscanf(buf_1,"%d\n",piVar14);
      if (uVar5 == uVar1) goto LAB_00014d7c;
    }
    do {
      uVar5 = uVar5 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,iVar4);
      sscanf(buf_1,"%d\n",piVar9);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar14 = piVar9 + 2;
      sscanf(buf_1,"%d\n",piVar9 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar13 = piVar9 + 3;
      piVar9 = piVar9 + 4;
      sscanf(buf_1,"%d\n",piVar14);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar4 = sscanf(buf_1,"%d\n",piVar13);
    } while (uVar5 != uVar1);
  }
LAB_00014d7c:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  uVar1 = constraints->num_constraints;
  piVar9 = constraints->Jofs;
  uVar15 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3,uVar1);
  if (uVar1 != 0) {
    uVar5 = uVar1 & 3;
    uVar7 = extraout_EDX_14;
    if (uVar5 != 0) {
      piVar14 = piVar9;
      if (1 < uVar5) {
        bVar2 = 2 < uVar5;
        piVar13 = piVar9;
        if (bVar2) {
          piVar13 = piVar9 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
          uVar5 = sscanf(buf_1,"%d\n",piVar9);
        }
        piVar9 = (int *)buf_1;
        uVar15 = bVar2 + 1;
        MeStreamReadLine(piVar9,0x100,pMVar3,uVar5);
        piVar14 = piVar13 + 1;
        sscanf(buf_1,"%d\n",piVar13);
      }
      uVar15 = uVar15 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,piVar9);
      piVar9 = piVar14 + 1;
      sscanf(buf_1,"%d\n",piVar14);
      uVar7 = extraout_EDX_15;
      if (uVar15 == uVar1) goto LAB_00014f2c;
    }
    do {
      uVar15 = uVar15 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
      sscanf(buf_1,"%d\n",piVar9);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar14 = piVar9 + 2;
      sscanf(buf_1,"%d\n",piVar9 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      piVar13 = piVar9 + 3;
      piVar9 = piVar9 + 4;
      sscanf(buf_1,"%d\n",piVar14);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%d\n",piVar13);
      uVar7 = extraout_EDX_16;
    } while (uVar15 != uVar1);
  }
LAB_00014f2c:
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  piVar9 = *constraints->Jbody;
  uVar1 = constraints->num_constraints * 2;
  uVar15 = 0;
  MeStreamReadLine(buf_1,0x100,pMVar3,constraints);
  uVar7 = extraout_EDX_17;
  if (uVar1 != 0) {
    uVar5 = uVar1 & 3;
    iVar4 = 0;
    if (uVar5 != 0) {
      piVar14 = piVar9;
      if (1 < uVar5) {
        uVar7 = extraout_ECX_06;
        piVar13 = piVar9;
        if (2 < uVar5) {
          piVar13 = piVar9 + 1;
          MeStreamReadLine(buf_1,0x100,pMVar3,0);
          sscanf(buf_1,"%d\n",piVar9);
          uVar7 = extraout_ECX_07;
        }
        uVar15 = (2 < uVar5) + 1;
        MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
        piVar14 = *(MdtKeaBodyIndexPair *)piVar13 + 1;
        uVar5 = sscanf(buf_1,"%d\n",piVar13);
      }
      uVar15 = uVar15 + 1;
      MeStreamReadLine(buf_1,0x100,pMVar3,uVar5);
      piVar9 = piVar14 + 1;
      iVar4 = sscanf(buf_1,"%d\n",piVar14);
      uVar7 = extraout_EDX_18;
      if (uVar15 == uVar1) goto LAB_000150dc;
    }
    do {
      uVar15 = uVar15 + 4;
      MeStreamReadLine(buf_1,0x100,pMVar3,iVar4);
      sscanf(buf_1,"%d\n",piVar9);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar4 = (int)(piVar9 + 2);
      sscanf(buf_1,"%d\n",piVar9 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar10 = (int)(piVar9 + 3);
      piVar9 = piVar9 + 4;
      sscanf(buf_1,"%d\n",iVar4);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      iVar4 = sscanf(buf_1,"%d\n",iVar10);
      uVar7 = extraout_EDX_19;
    } while (uVar15 != uVar1);
  }
LAB_000150dc:
  MeStreamReadLine(buf_1,0x100,pMVar3,uVar7);
  MeStreamReadLine(buf_1,0x100,pMVar3);
  sscanf(buf_1,"%d\n",num_bodies);
  i = 0;
  if (*num_bodies != 0) {
    local_258 = blist;
    do {
      pMVar6 = (MdtKeaBody *)(*_MeMemoryAPI)(0xf0);
      *local_258 = pMVar6;
      MeStreamReadLine(buf,0x100,pMVar3);
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",&pMVar6->invmass);
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%d\n",&pMVar6->flags);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->force);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->force + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->force + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->force + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->torque);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->torque + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->torque + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->torque + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI0);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI0 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI0 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI0 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI1 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI1 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI1 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI2 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI2 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->invI2 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I0);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I0 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I0 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I0 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I1 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I1 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I1 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I2 + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I2 + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->I2 + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->vel);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->vel + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->vel + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->vel + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->velrot);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->velrot + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->velrot + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->velrot + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->qrot);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->qrot + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->qrot + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->qrot + 3);
                    /* Unresolved local var: int i@[???] */
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->accel);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->accel + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->accel + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
                    /* Unresolved local var: int i@[???] */
      sscanf(buf_1,"%08x\n",pMVar6->accel + 3);
      pMVar6 = *local_258;
      MeStreamReadLine(buf_1,0x100,pMVar3);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->fastSpinAxis);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->fastSpinAxis + 1);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->fastSpinAxis + 2);
      MeStreamReadLine(buf_1,0x100,pMVar3);
      sscanf(buf_1,"%08x\n",pMVar6->fastSpinAxis + 3);
      i = i + 1;
      local_258 = local_258 + 1;
    } while (i != *num_bodies);
  }
  MeStreamClose(pMVar3);
  return;
}


/* ==== writeLambdaToFile ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void writeLambdaToFile(char *filename,MeReal *lambda,int num_elts)

{
  MeReal *pMVar1;
  bool bVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  int iVar7;
  uint uVar8;
  char buf [256];
  
                    /* Unresolved local var: int file@[DW_OP_reg7(EDI)] */
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg0(EAX)] */
  iVar7 = 0;
  uVar3 = MeOpen(filename,1);
  printf("-- writing kea output data (lambda) to file %s\n",*(undefined4 *)(_gDebug + 0x1c));
  iVar4 = sprintf(buf,"%s\n","lambda");
  MeWrite(uVar3,buf,iVar4);
  if (num_elts != 0) {
    uVar5 = num_elts & 3;
    uVar6 = 0;
    if (uVar5 != 0) {
      uVar6 = extraout_ECX;
      if (1 < uVar5) {
        bVar2 = 2 < uVar5;
        if (bVar2) {
          iVar4 = sprintf(buf,"%08x\n",*lambda,uVar5);
          uVar5 = MeWrite(uVar3,buf,iVar4);
        }
        uVar8 = (uint)bVar2;
        iVar7 = uVar8 + 1;
        iVar4 = sprintf(buf,"%08x\n",lambda[uVar8],uVar5);
        MeWrite(uVar3,buf,iVar4);
        uVar6 = extraout_ECX_00;
      }
      pMVar1 = lambda + iVar7;
      iVar7 = iVar7 + 1;
      iVar4 = sprintf(buf,"%08x\n",*pMVar1,uVar6);
      uVar6 = MeWrite(uVar3,buf,iVar4);
      if (iVar7 == num_elts) goto LAB_00016344;
    }
    do {
      iVar4 = sprintf(buf,"%08x\n",lambda[iVar7],uVar6);
      MeWrite(uVar3,buf,iVar4);
      iVar4 = sprintf(buf,"%08x\n",lambda[iVar7 + 1]);
      MeWrite(uVar3,buf,iVar4);
      iVar4 = sprintf(buf,"%08x\n",lambda[iVar7 + 2]);
      MeWrite(uVar3,buf,iVar4);
      iVar4 = iVar7 + 3;
      iVar7 = iVar7 + 4;
      iVar4 = sprintf(buf,"%08x\n",lambda[iVar4]);
      uVar6 = MeWrite(uVar3,buf,iVar4);
    } while (iVar7 != num_elts);
  }
LAB_00016344:
  MeClose(uVar3);
  return;
}


/* ==== checkPrintDebugInput ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: int checkPrintDebugInput(keaFunctions * this, MdtKeaConstraints
   constraints, MdtKeaParameters parameters, MdtKeaBody * * blist, int num_bodies) */

int __thiscall
keaFunctions::checkPrintDebugInput
          (keaFunctions *this,MdtKeaConstraints constraints,MdtKeaParameters parameters,
          MdtKeaBody **blist,int num_bodies)

{
  int iVar1;
  MdtKeaParameters *pMVar2;
  MdtKeaConstraints *pMVar3;
  MeReal *pMVar4;
  int *piVar5;
  MdtKeaConstraints in_stack_ffffff44;
  MdtKeaParameters in_stack_ffffffa0;
  
  if ((*_gDebug != 0) && (_gDebug[8] == _gDebug[9])) {
    pMVar2 = &parameters;
    pMVar4 = (MeReal *)&stack0xffffffa0;
    for (iVar1 = 0x13; iVar1 != 0; iVar1 = iVar1 + -1) {
      *pMVar4 = pMVar2->stepsize;
      pMVar2 = (MdtKeaParameters *)&pMVar2->epsilon;
      pMVar4 = pMVar4 + 1;
    }
    pMVar3 = &constraints;
    piVar5 = (int *)&stack0xffffff44;
    for (iVar1 = 0x17; iVar1 != 0; iVar1 = iVar1 + -1) {
      *piVar5 = pMVar3->num_partitions;
      pMVar3 = (MdtKeaConstraints *)&pMVar3->max_partitions;
      piVar5 = piVar5 + 1;
    }
    writeKeaInputToFile(in_stack_ffffff44,in_stack_ffffffa0,blist,num_bodies);
  }
  return 0;
}


