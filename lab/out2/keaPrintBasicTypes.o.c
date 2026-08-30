/* ==== printMeReal ==== */

/* WARNING: Unknown calling convention */

void printMeReal(MeReal x,char *desc)

{
  printf("%s ",desc);
  printf("% 09.6f\n",(double)x);
  return;
}


/* ==== printColMajorMat ==== */

void printColMajorMat(MeReal *A,int rows,int cols,char *desc)

{
  float fVar1;
  uint uVar2;
  float *pfVar3;
  float *pfVar4;
  float *pfVar5;
  int iVar6;
  int i;
  int cols_local;
  int rows_local;
  MeReal *A_local;
  
                    /* Unresolved local var: int j@[DW_OP_reg7(EDI)] */
  printf("%s \n",desc);
  i = 0;
  if (rows != 0) {
    do {
      iVar6 = 0;
      if (cols != 0) {
        pfVar3 = A + i;
        uVar2 = cols & 3;
        if (uVar2 != 0) {
          if (1 < uVar2) {
            if (2 < uVar2) {
              fVar1 = *pfVar3;
              pfVar3 = pfVar3 + rows;
              printf("% 6.2f ",(double)fVar1);
            }
            fVar1 = *pfVar3;
            pfVar3 = pfVar3 + rows;
            iVar6 = (2 < uVar2) + 1;
            printf("% 6.2f ",(double)fVar1);
          }
          fVar1 = *pfVar3;
          iVar6 = iVar6 + 1;
          pfVar3 = pfVar3 + rows;
          printf("% 6.2f ",(double)fVar1);
          if (iVar6 == cols) goto LAB_0001012a;
        }
        do {
          pfVar4 = pfVar3 + rows;
          iVar6 = iVar6 + 4;
          printf("% 6.2f ",(double)*pfVar3);
          pfVar5 = pfVar4 + rows;
          printf("% 6.2f ",(double)*pfVar4);
          printf("% 6.2f ",(double)*pfVar5);
          pfVar3 = pfVar5 + rows + rows;
          printf("% 6.2f ",(double)pfVar5[rows]);
        } while (iVar6 != cols);
      }
LAB_0001012a:
      putchar(10);
      i = i + 1;
    } while (i != rows);
  }
  putchar(10);
  return;
}


/* ==== printIntMat ==== */

void printIntMat(int *A,int rows,int cols,char *desc)

{
  int *piVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int extraout_EDX;
  int *piVar6;
  int iVar7;
  int local_18;
  int i;
  
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)] */
  putchar(10);
  printf("%s \n",desc);
  i = 0;
  if (rows != 0) {
    local_18 = 0;
    iVar5 = rows;
    do {
      iVar7 = 0;
      if (cols != 0) {
        piVar6 = A + local_18;
        uVar3 = cols & 3;
        iVar4 = 0;
        if (uVar3 != 0) {
          if (1 < uVar3) {
            bVar2 = 2 < uVar3;
            if (bVar2) {
              iVar7 = *piVar6;
              piVar6 = piVar6 + 1;
              uVar3 = printf("% 2d ",iVar7,iVar5,iVar5);
            }
            iVar7 = bVar2 + 1;
            iVar5 = *piVar6;
            piVar6 = piVar6 + 1;
            uVar3 = printf("% 2d ",iVar5,uVar3,uVar3);
          }
          iVar7 = iVar7 + 1;
          iVar5 = *piVar6;
          piVar6 = piVar6 + 1;
          iVar4 = printf("% 2d ",iVar5,uVar3,uVar3);
          if (iVar7 == cols) goto LAB_00010264;
        }
        do {
          iVar7 = iVar7 + 4;
          printf("% 2d ",*piVar6,iVar4,iVar4);
          printf("% 2d ",piVar6[1]);
          printf("% 2d ",piVar6[2]);
          piVar1 = piVar6 + 3;
          piVar6 = piVar6 + 4;
          iVar4 = printf("% 2d ",*piVar1);
        } while (iVar7 != cols);
      }
LAB_00010264:
      putchar(10);
      i = i + 1;
      local_18 = local_18 + cols;
      iVar5 = extraout_EDX;
    } while (i != rows);
  }
  return;
}


/* ==== printMat44 ==== */

void printMat44(MeReal *A,char *desc)

{
  float *pfVar1;
  int iVar2;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  iVar2 = 0;
  putchar(10);
  printf("%s \n",desc);
  do {
    pfVar1 = A + iVar2;
    iVar2 = iVar2 + 1;
    printf("% 09.6f ",(double)*pfVar1);
    printf("% 09.6f ",(double)pfVar1[4]);
    printf("% 09.6f ",(double)pfVar1[8]);
    printf("% 09.6f ",(double)pfVar1[0xc]);
    putchar(10);
  } while (iVar2 != 4);
  return;
}


/* ==== printMat46 ==== */

void printMat46(MeReal *A,char *desc)

{
  float *pfVar1;
  int iVar2;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  iVar2 = 0;
  putchar(10);
  printf("%s \n",desc);
  do {
    pfVar1 = A + iVar2;
    iVar2 = iVar2 + 1;
    printf("% 09.6f ",(double)*pfVar1);
    printf("% 09.6f ",(double)pfVar1[4]);
    printf("% 09.6f ",(double)pfVar1[8]);
    printf("% 09.6f ",(double)pfVar1[0xc]);
    printf("% 09.6f ",(double)pfVar1[0x10]);
    printf("% 09.6f ",(double)pfVar1[0x14]);
    putchar(10);
  } while (iVar2 != 4);
  return;
}


/* ==== printMat412 ==== */

/* WARNING: Unknown calling convention */

void printMat412(MeReal *A,char *desc)

{
  float *pfVar1;
  float *pfVar2;
  int iVar3;
  int iVar4;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int j@[DW_OP_reg6(ESI)] */
  iVar4 = 0;
  putchar(10);
  printf("%s \n",desc);
  do {
    iVar3 = 0;
    pfVar2 = A + iVar4;
    do {
      iVar3 = iVar3 + 6;
      printf("% 09.6f ",(double)*pfVar2);
      printf("% 09.6f ",(double)pfVar2[4]);
      printf("% 09.6f ",(double)pfVar2[8]);
      printf("% 09.6f ",(double)pfVar2[0xc]);
      printf("% 09.6f ",(double)pfVar2[0x10]);
      pfVar1 = pfVar2 + 0x14;
      pfVar2 = pfVar2 + 0x18;
      printf("% 09.6f ",(double)*pfVar1);
    } while (iVar3 != 0xc);
    iVar4 = iVar4 + 1;
    putchar(10);
  } while (iVar4 != 4);
  return;
}


/* ==== printIntVec ==== */

void printIntVec(int *A,int numElts,char *desc)

{
  int *piVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  undefined4 extraout_EDX;
  int iVar5;
  uint uVar6;
  
                    /* Unresolved local var: int j@[DW_OP_reg3(EBX)] */
  iVar5 = 0;
  printf("%s \n",desc);
  if (numElts != 0) {
    uVar3 = numElts & 3;
    iVar4 = 0;
    if (uVar3 != 0) {
      if (1 < uVar3) {
        bVar2 = 2 < uVar3;
        if (bVar2) {
          uVar3 = printf("% 2d ",*A,extraout_EDX,extraout_EDX);
        }
        uVar6 = (uint)bVar2;
        iVar5 = uVar6 + 1;
        uVar3 = printf("% 2d ",A[uVar6],uVar3,uVar3);
      }
      piVar1 = A + iVar5;
      iVar5 = iVar5 + 1;
      iVar4 = printf("% 2d ",*piVar1,uVar3,uVar3);
      if (iVar5 == numElts) goto LAB_0001058d;
    }
    do {
      printf("% 2d ",A[iVar5],iVar4,iVar4);
      printf("% 2d ",A[iVar5 + 1]);
      printf("% 2d ",A[iVar5 + 2]);
      iVar4 = iVar5 + 3;
      iVar5 = iVar5 + 4;
      iVar4 = printf("% 2d ",A[iVar4]);
    } while (iVar5 != numElts);
  }
LAB_0001058d:
  putchar(10);
  return;
}


/* ==== printVec4 ==== */

void printVec4(MeReal *A,char *desc)

{
                    /* Unresolved local var: int j@[???] */
  printf("%s \n",desc);
  printf(" 0x%08x % 09.6f\n",A,(double)*A);
  printf(" 0x%08x % 09.6f\n",A + 1,(double)A[1]);
  printf(" 0x%08x % 09.6f\n",A + 2,(double)A[2]);
  printf(" 0x%08x % 09.6f\n",A + 3,(double)A[3]);
  putchar(10);
  return;
}


/* ==== printVec3 ==== */

void printVec3(MeReal *A,char *desc)

{
                    /* Unresolved local var: int j@[???] */
  printf("%s \n",desc);
  printf("% 09.6f ",(double)*A);
  printf("% 09.6f ",(double)A[1]);
  printf("% 09.6f ",(double)A[2]);
  putchar(10);
  return;
}


/* ==== printVec ==== */

void printVec(MeReal *A,int numElts,char *desc)

{
  float *pfVar1;
  int iVar2;
  uint uVar3;
  MeReal *pMVar4;
  MeReal *pMVar5;
  int iVar6;
  int numElts_local;
  
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)] */
  iVar6 = 0;
  printf("%s \n",desc);
  if (numElts != 0) {
    uVar3 = numElts & 3;
    pMVar4 = A;
    if (uVar3 != 0) {
      pMVar5 = A;
      if (1 < uVar3) {
        if (2 < uVar3) {
          pMVar4 = A + 1;
          printf("%08x % 09.6f\n",A,(double)*A);
        }
        uVar3 = (uint)(2 < uVar3);
        iVar6 = uVar3 + 1;
        pMVar5 = pMVar4 + 1;
        printf("%08x % 09.6f\n",pMVar4,(double)A[uVar3]);
      }
      pfVar1 = A + iVar6;
      iVar6 = iVar6 + 1;
      pMVar4 = pMVar5 + 1;
      printf("%08x % 09.6f\n",pMVar5,(double)*pfVar1);
      if (iVar6 == numElts) goto LAB_00010788;
    }
    do {
      printf("%08x % 09.6f\n",pMVar4,(double)A[iVar6]);
      printf("%08x % 09.6f\n",pMVar4 + 1,(double)A[iVar6 + 1]);
      printf("%08x % 09.6f\n",pMVar4 + 2,(double)A[iVar6 + 2]);
      iVar2 = iVar6 + 3;
      pMVar5 = pMVar4 + 3;
      iVar6 = iVar6 + 4;
      pMVar4 = pMVar4 + 4;
      printf("%08x % 09.6f\n",pMVar5,(double)A[iVar2]);
    } while (iVar6 != numElts);
  }
LAB_00010788:
  putchar(10);
  return;
}


/* ==== printPtr ==== */

/* WARNING: Unknown calling convention */

void printPtr(void *ptr,char *desc)

{
  printf("%20s %08x\n",desc,ptr);
  return;
}


/* ==== printInvMassMatrix ==== */

/* WARNING: Unknown calling convention */

void printInvMassMatrix(MdtKeaInverseMassMatrix invM,char *desc)

{
                    /* Unresolved local var: int i@[???] */
  puts(desc);
  putchar(10);
  printf("% 09.6f ",(double)invM.invI0[0]);
  printf("% 09.6f ",(double)invM.invI0[1]);
  printf("% 09.6f ",(double)invM.invI0[2]);
  printf(" % 014f ",(double)invM.invmass);
  putchar(10);
  printf("% 09.6f ",(double)invM.invI1[0]);
  printf("% 09.6f ",(double)invM.invI1[1]);
  printf("% 09.6f ",(double)invM.invI1[2]);
  putchar(10);
  printf("% 09.6f ",(double)invM.invI2[0]);
  printf("% 09.6f ",(double)invM.invI2[1]);
  printf("% 09.6f ",(double)invM.invI2[2]);
  putchar(10);
  return;
}


/* ==== printInvMassMatrixArray ==== */

/* WARNING: Unknown calling convention */

void printInvMassMatrixArray(MdtKeaInverseMassMatrix *invM,int numElts,char *desc)

{
  MeReal *pMVar1;
  float *pfVar2;
  MdtKeaInverseMassMatrix *pMVar3;
  MdtKeaInverseMassMatrix *pMVar4;
  int local_1c;
  MeReal *local_18;
  int j;
  
                    /* Unresolved local var: int i@[???] */
  puts(desc);
  putchar(10);
  j = 0;
  if (numElts != 0) {
    local_18 = &invM->invmass;
    local_1c = 0;
    pMVar3 = invM;
    pMVar4 = invM;
    do {
      pfVar2 = (float *)((int)invM->invI0 + local_1c);
      printf("% 09.6f ",(double)*pfVar2);
      printf("% 09.6f ",(double)pfVar2[1]);
      printf("% 09.6f ",(double)pfVar2[2]);
      printf(" % 014f ",(double)*local_18);
      putchar(10);
      printf("% 09.6f ",(double)pMVar3->invI1[0]);
      printf("% 09.6f ",(double)pMVar3->invI1[1]);
      pMVar1 = pMVar3->invI1;
      pMVar3 = pMVar3 + 1;
      printf("% 09.6f ",(double)pMVar1[2]);
      putchar(10);
      printf("% 09.6f ",(double)pMVar4->invI2[0]);
      printf("% 09.6f ",(double)pMVar4->invI2[1]);
      pMVar1 = pMVar4->invI2;
      pMVar4 = pMVar4 + 1;
      printf("% 09.6f ",(double)pMVar1[2]);
      putchar(10);
      j = j + 1;
      local_18 = local_18 + 0xc;
      local_1c = local_1c + 0x30;
    } while (j != numElts);
  }
  return;
}


/* ==== printMdtKeaForcePairArray ==== */

void printMdtKeaForcePairArray(MdtKeaForcePair *cforces,int numConstraints)

{
  int iVar1;
  MdtKeaForcePair *pMVar2;
  int iVar3;
  int iVar4;
  MdtKeaForcePair *cforces_local;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  iVar1 = puts("constraint forces");
  if (numConstraints != 0) {
    iVar3 = 0;
    do {
      printf("Constraint %3d: ",iVar3,iVar1,iVar1);
      iVar4 = iVar3 + 1;
      pMVar2 = cforces + iVar3;
      printf("% 012.06f ",(double)(pMVar2->primary_body).force[0]);
      printf("% 012.06f ",(double)(pMVar2->primary_body).force[1]);
      printf("% 012.06f ",(double)(pMVar2->primary_body).force[2]);
      printf("% 012.06f ",(double)(pMVar2->primary_body).torque[0]);
      printf("% 012.06f ",(double)(pMVar2->primary_body).torque[1]);
      printf("% 012.06f ",(double)(pMVar2->primary_body).torque[2]);
      putchar(10);
      printf("                ");
      printf("% 012.06f ",(double)(pMVar2->secondary_body).force[0]);
      printf("% 012.06f ",(double)(pMVar2->secondary_body).force[1]);
      printf("% 012.06f ",(double)(pMVar2->secondary_body).force[2]);
      printf("% 012.06f ",(double)(pMVar2->secondary_body).torque[0]);
      printf("% 012.06f ",(double)(pMVar2->secondary_body).torque[1]);
      printf("% 012.06f ");
      iVar1 = putchar(10);
      iVar3 = iVar4;
    } while (iVar4 != numConstraints);
  }
  putchar(10);
  return;
}


/* ==== printMdtKeaBl2CBodyRowArray ==== */

void __regparm1 printMdtKeaBl2CBodyRowArray(int (*bl2cbody) [8],int numElts)

{
  undefined4 *puVar1;
  int iVar2;
  int in_stack_00000008;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  iVar2 = 0;
  if (in_stack_00000008 != 0) {
    do {
      puVar1 = (undefined4 *)(iVar2 * 0x20 + numElts);
      iVar2 = iVar2 + 1;
      printf("% 3d ",*puVar1,bl2cbody,bl2cbody);
      printf("% 3d ",puVar1[1]);
      printf("% 3d ",puVar1[2]);
      printf("% 3d ",puVar1[3]);
      printf("% 3d ",puVar1[4]);
      printf("% 3d ",puVar1[5]);
      printf("% 3d ",puVar1[6]);
      printf("% 3d ",puVar1[7]);
      bl2cbody = (int (*) [8])putchar(10);
    } while (iVar2 != in_stack_00000008);
  }
  return;
}


/* ==== printMdtKeaBl2BodyRowArray ==== */

void __regparm1 printMdtKeaBl2BodyRowArray(int (*bl2cbody) [8],int numElts)

{
  undefined4 *puVar1;
  int iVar2;
  int in_stack_00000008;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  iVar2 = 0;
  if (in_stack_00000008 != 0) {
    do {
      puVar1 = (undefined4 *)(iVar2 * 0x20 + numElts);
      iVar2 = iVar2 + 1;
      printf("% 3d ",*puVar1,bl2cbody,bl2cbody);
      printf("% 3d ",puVar1[1]);
      printf("% 3d ",puVar1[2]);
      printf("% 3d ",puVar1[3]);
      printf("% 3d ",puVar1[4]);
      printf("% 3d ",puVar1[5]);
      printf("% 3d ",puVar1[6]);
      printf("% 3d ",puVar1[7]);
      bl2cbody = (int (*) [8])putchar(10);
    } while (iVar2 != in_stack_00000008);
  }
  return;
}


/* ==== printMdtKeaJBlockPairArray ==== */

void printMdtKeaJBlockPairArray(MdtKeaJBlock (*J) [2],int num_rows_inc_padding)

{
  MeReal *pMVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int local_24;
  int l;
  int i;
  int num_rows_inc_padding_local;
  MdtKeaJBlock (*J_local) [2];
  
                    /* Unresolved local var: int j@[DW_OP_reg7(EDI)]
                       Unresolved local var: int k@[???] */
  i = 0;
  local_24 = 0;
  while( true ) {
    iVar3 = num_rows_inc_padding;
    if (num_rows_inc_padding < 0) {
      iVar3 = num_rows_inc_padding + 3;
    }
    if (i == iVar3 >> 2) break;
    l = 0;
    do {
      iVar4 = 0;
      iVar3 = 0;
      do {
        iVar2 = local_24 + iVar3;
        iVar4 = iVar4 + 1;
        iVar3 = iVar3 + 0x18;
        pMVar1 = (*J)[0].col[0] + iVar2 + l;
        printf("% 6.3f",(double)*pMVar1);
        printf("% 6.3f",(double)pMVar1[4]);
        printf("% 6.3f",(double)pMVar1[8]);
        printf("% 6.3f",(double)pMVar1[0xc]);
        printf("% 6.3f",(double)pMVar1[0x10]);
        printf("% 6.3f",(double)pMVar1[0x14]);
        printf("| ");
      } while (iVar4 != 2);
      putchar(10);
      l = l + 1;
    } while (l != 4);
    putchar(10);
    local_24 = local_24 + 0x30;
    i = i + 1;
  }
  putchar(10);
  return;
}


/* ==== printMdtKeaVelocityArray ==== */

void printMdtKeaVelocityArray(MdtKeaVelocity *v,int num_bodies,char *desc)

{
  MdtKeaVelocity *pMVar1;
  int iVar2;
  MdtKeaVelocity *v_local;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  iVar2 = 0;
  puts(desc);
  if (num_bodies != 0) {
    do {
      pMVar1 = v + iVar2;
      iVar2 = iVar2 + 1;
      printf("% 12.6f ",(double)pMVar1->velocity[0]);
      printf("% 12.6f ",(double)pMVar1->velocity[1]);
      printf("% 12.6f ",(double)pMVar1->velocity[2]);
      putchar(0x20);
      printf("% 12.6f ",(double)pMVar1->angVelocity[0]);
      printf("% 12.6f ",(double)pMVar1->angVelocity[1]);
      printf("% 12.6f ",(double)pMVar1->angVelocity[2]);
      putchar(10);
      putchar(10);
    } while (iVar2 != num_bodies);
  }
  putchar(10);
  return;
}


/* ==== printHalfWallpaperMatrix ==== */

void printHalfWallpaperMatrix(MeReal *A,int c4numRows)

{
  float *pfVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  MeReal *pRowBlock;
  int k;
  int i;
  int c4numRows_local;
  
                    /* Unresolved local var: int j@[DW_OP_reg7(EDI)]
                       Unresolved local var: int l@[???] */
  pRowBlock = A;
  i = 0;
  while( true ) {
    iVar2 = c4numRows;
    if (c4numRows < 0) {
      iVar2 = c4numRows + 3;
    }
    if (i == iVar2 >> 2) break;
    iVar2 = i + 1;
    k = 0;
    do {
      iVar4 = 0;
      iVar3 = k;
      if (iVar2 != 0) {
        do {
          iVar4 = iVar4 + 1;
          pfVar1 = pRowBlock + iVar3;
          printf("% 6.2f ",(double)*pfVar1);
          printf("% 6.2f ",(double)pfVar1[4]);
          printf("% 6.2f ",(double)pfVar1[8]);
          printf("% 6.2f ",(double)pfVar1[0xc]);
          iVar3 = iVar3 + 0x10;
        } while (iVar4 != iVar2);
      }
      putchar(10);
      k = k + 1;
    } while (k != 4);
    putchar(10);
    pRowBlock = pRowBlock + i * 0x10 + 0x10;
    i = iVar2;
  }
  putchar(10);
  return;
}


/* ==== printPS2SparseMatrix ==== */

void printPS2SparseMatrix(MeReal *matrix,int *rlist,int *rlist_len,int num_12_blocks)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int rlist_index;
  int j;
  int i;
  int num_12_blocks_local;
  int *rlist_len_local;
  int *rlist_local;
  MeReal *matrix_local;
  
                    /* Unresolved local var: int k@[DW_OP_reg3(EBX)]
                       Unresolved local var: int l@[DW_OP_reg7(EDI)] */
  i = 0;
  if (num_12_blocks != 0) {
    do {
      iVar4 = 0;
      iVar5 = i + 1;
      do {
        rlist_index = rlist_len[i] + -1;
        j = 0;
        if (num_12_blocks != 0) {
          do {
            if (rlist[num_12_blocks * i + rlist_index] == j) {
                    /* Unresolved local var: int destcol@[???]
                       Unresolved local var: int sourcecol@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * source@[DW_OP_reg6(ESI)] */
              iVar1 = i - rlist_index;
              iVar6 = (i * iVar5) / 2 + iVar1;
              iVar7 = 0;
              do {
                iVar2 = iVar4;
                if (iVar4 < 0) {
                  iVar2 = iVar4 + 3;
                }
                printf("% 6.2f ",
                       (double)matrix[iVar6 * 0x90 + iVar7 * 4 + (iVar2 >> 2) * 0x2c + iVar4],iVar1)
                ;
                iVar2 = iVar4;
                if (iVar4 < 0) {
                  iVar2 = iVar4 + 3;
                }
                printf("% 6.2f ",
                       (double)matrix[iVar6 * 0x90 + (iVar7 + 1) * 4 + (iVar2 >> 2) * 0x2c + iVar4],
                       iVar1);
                iVar2 = iVar7 + 2;
                iVar3 = iVar4;
                if (iVar4 < 0) {
                  iVar3 = iVar4 + 3;
                }
                iVar7 = iVar7 + 3;
                iVar1 = printf("% 6.2f ",
                               (double)matrix[iVar6 * 0x90 + iVar2 * 4 + (iVar3 >> 2) * 0x2c + iVar4
                                             ],iVar1);
              } while (iVar7 != 0xc);
              rlist_index = rlist_index + -1;
            }
            else {
              printf("% 0000.00 ");
              printf("% 0000.00 ");
              printf("% 0000.00 ");
              printf("% 0000.00 ");
              printf("% 0000.00 ");
              printf("% 0000.00 ");
              printf("% 0000.00 ");
              printf("% 0000.00 ");
              printf("% 0000.00 ");
              printf("% 0000.00 ");
              printf("% 0000.00 ");
              printf("% 0000.00 ");
            }
            putchar(0x20);
            j = j + 1;
          } while (j != num_12_blocks);
        }
        iVar4 = iVar4 + 1;
        putchar(10);
      } while (iVar4 != 0xc);
      putchar(10);
      i = iVar5;
    } while (iVar5 != num_12_blocks);
  }
  putchar(10);
  return;
}


/* ==== putchar ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

int putchar(int __c)

{
  int iVar1;
  
  iVar1 = _IO_putc(__c,_stdout);
  return iVar1;
}


