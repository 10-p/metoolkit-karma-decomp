/* ==== printFactoriserInput ==== */

/* WARNING: Removing unreachable block (ram,0x000104a0) */
/* WARNING: Removing unreachable block (ram,0x00010498) */

void printFactoriserInput(MeReal *newA,MeReal *lo,MeReal *hi,int ceil4_num_rows)

{
  uint uVar1;
  MeReal *pMVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 in_ECX;
  undefined4 extraout_ECX;
  int iVar5;
  int iVar6;
  int i;
  MeReal *hi_local;
  MeReal *lo_local;
  
                    /* Unresolved local var: int j@[DW_OP_reg3(EBX)] */
  i = 0;
  pMVar2 = hi;
  if (ceil4_num_rows != 0) {
    do {
      iVar5 = 0;
      if (ceil4_num_rows != 0) {
        uVar1 = ceil4_num_rows & 3;
        if (uVar1 != 0) {
          iVar6 = iVar5;
          if (1 < uVar1) {
            if (2 < uVar1) {
              MeInfo(0,"%12.6f ",(double)newA[i * 4]);
            }
            uVar1 = (uint)(2 < uVar1);
            iVar6 = uVar1 + 1;
            MeInfo(0,"%12.6f ",(double)newA[uVar1 + i * 4]);
          }
          iVar5 = iVar6 + 1;
          pMVar2 = (MeReal *)MeInfo(0,"%12.6f ",(double)newA[iVar6 + i * 4]);
          if (iVar5 == ceil4_num_rows) goto LAB_000101cc;
        }
        do {
          iVar6 = iVar5;
          if (iVar5 < 0) {
            iVar6 = iVar5 + 3;
          }
          MeInfo(0,"%12.6f ",
                 (double)newA[iVar5 + (iVar6 >> 2) * -4 + ((iVar6 >> 2) * ceil4_num_rows + i) * 4]);
          iVar6 = iVar5 + 1;
          iVar3 = iVar6;
          if (iVar6 < 0) {
            iVar3 = iVar5 + 4;
          }
          MeInfo(0,"%12.6f ",
                 (double)newA[iVar6 + (iVar3 >> 2) * -4 + ((iVar3 >> 2) * ceil4_num_rows + i) * 4]);
          iVar6 = iVar5 + 2;
          iVar3 = iVar6;
          if (iVar6 < 0) {
            iVar3 = iVar5 + 5;
          }
          MeInfo(0,"%12.6f ",
                 (double)newA[iVar6 + (iVar3 >> 2) * -4 + ((iVar3 >> 2) * ceil4_num_rows + i) * 4]);
          iVar6 = iVar5 + 3;
          iVar3 = iVar6;
          if (iVar6 < 0) {
            iVar3 = iVar5 + 6;
          }
          iVar5 = iVar5 + 4;
          pMVar2 = (MeReal *)
                   MeInfo(0,"%12.6f ",
                          (double)newA[iVar6 + (iVar3 >> 2) * -4 +
                                       ((iVar3 >> 2) * ceil4_num_rows + i) * 4]);
        } while (iVar5 != ceil4_num_rows);
      }
LAB_000101cc:
      pMVar2 = (MeReal *)MeInfo(0,&DAT_00011f87,pMVar2,pMVar2);
      i = i + 1;
      in_ECX = extraout_ECX;
    } while (i != ceil4_num_rows);
  }
  MeInfo(0,&DAT_00011f87,in_ECX,in_ECX);
  uVar4 = MeInfo(0,"lo         =");
  i = 0;
  if (ceil4_num_rows != 0) {
    uVar1 = ceil4_num_rows & 3;
    if (uVar1 != 0) {
      if (1 < uVar1) {
        if (2 < uVar1) {
          MeInfo(0,"%17.6f ",(double)*lo);
        }
        i = (int)(2 < uVar1);
        MeInfo(0,"%17.6f ",(double)lo[i]);
        i = i + 1;
      }
      uVar4 = MeInfo(0,"%17.6f ",(double)lo[i]);
      i = i + 1;
      if (i == ceil4_num_rows) goto LAB_000102fd;
    }
    do {
      MeInfo(0,"%17.6f ",(double)lo[i]);
      MeInfo(0,"%17.6f ",(double)lo[i + 1]);
      MeInfo(0,"%17.6f ",(double)lo[i + 2]);
      uVar4 = MeInfo(0,"%17.6f ",(double)lo[i + 3]);
      i = i + 4;
    } while (i != ceil4_num_rows);
  }
LAB_000102fd:
  MeInfo(0,&DAT_00011f9d,uVar4,uVar4);
  MeInfo(0,"hi         =");
  i = 0;
  if (ceil4_num_rows != 0) {
    uVar1 = ceil4_num_rows & 3;
    if (uVar1 != 0) {
      if (1 < uVar1) {
        if (2 < uVar1) {
          MeInfo(0,"%17.6f ",(double)*hi);
        }
        i = (int)(2 < uVar1);
        MeInfo(0,"%17.6f ",(double)hi[i]);
        i = i + 1;
      }
      MeInfo(0,"%17.6f ",(double)hi[i]);
      i = i + 1;
      if (i == ceil4_num_rows) goto LAB_0001040d;
    }
    do {
      MeInfo(0,"%17.6f ",(double)hi[i]);
      MeInfo(0,"%17.6f ",(double)hi[i + 1]);
      MeInfo(0,"%17.6f ",(double)hi[i + 2]);
      MeInfo(0,"%17.6f ",(double)hi[i + 3]);
      i = i + 4;
    } while (i != ceil4_num_rows);
  }
LAB_0001040d:
  MeInfo();
  return;
}


/* ==== printJlenandBl2BodyOutput ==== */

/* WARNING: Unknown calling convention */

void printJlenandBl2BodyOutput(int *jlen,int *bl2body,int *bl2cony,int num_strips)

{
  int *piVar1;
  bool bVar2;
  int iVar3;
  int *piVar4;
  uint uVar5;
  undefined4 extraout_EDX;
  int *piVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int j@[DW_OP_reg6(ESI)] */
  iVar8 = 0;
  puts("Jlen and Bl2Body Output");
  puts("-----------------------");
  iVar9 = num_strips;
  printf("num_strips=%d\n",num_strips);
  puts("bl2body:");
  if (num_strips != 0) {
    do {
      iVar7 = 0;
      iVar3 = printf("body:",iVar9);
      if (jlen[iVar8] != 0) {
        piVar6 = bl2body + iVar8 * 8;
        do {
          iVar7 = iVar7 + 1;
          iVar9 = *piVar6;
          piVar6 = piVar6 + 1;
          iVar3 = printf("% d ",iVar9,iVar3,iVar3);
        } while (iVar7 != jlen[iVar8]);
      }
      iVar3 = 0;
      printf("\ncnst:",iVar9);
      if (jlen[iVar8] != 0) {
        piVar6 = bl2cony + iVar8 * 8;
        piVar4 = jlen;
        do {
          iVar3 = iVar3 + 1;
          iVar9 = *piVar6;
          piVar6 = piVar6 + 1;
          piVar4 = (int *)printf("% d ",iVar9,piVar4,piVar4);
        } while (iVar3 != jlen[iVar8]);
      }
      iVar8 = iVar8 + 1;
      putchar(10);
    } while (iVar8 != num_strips);
  }
  iVar9 = 0;
  puts("jlen:");
  if (num_strips != 0) {
    uVar5 = num_strips & 3;
    iVar8 = 0;
    piVar6 = jlen;
    if (uVar5 != 0) {
      piVar4 = jlen;
      if (1 < uVar5) {
        bVar2 = 2 < uVar5;
        if (bVar2) {
          piVar6 = jlen + 1;
          uVar5 = printf("%08x: %2d\n",jlen,*jlen,extraout_EDX);
        }
        uVar10 = (uint)bVar2;
        iVar9 = uVar10 + 1;
        piVar4 = piVar6 + 1;
        uVar5 = printf("%08x: %2d\n",piVar6,jlen[uVar10],uVar5);
      }
      piVar1 = jlen + iVar9;
      iVar9 = iVar9 + 1;
      piVar6 = piVar4 + 1;
      iVar8 = printf("%08x: %2d\n",piVar4,*piVar1,uVar5);
      if (iVar9 == num_strips) {
        return;
      }
    }
    do {
      printf("%08x: %2d\n",piVar6,jlen[iVar9],iVar8);
      printf("%08x: %2d\n",piVar6 + 1,jlen[iVar9 + 1]);
      printf("%08x: %2d\n",piVar6 + 2,jlen[iVar9 + 2]);
      piVar4 = piVar6 + 3;
      iVar8 = iVar9 + 3;
      iVar9 = iVar9 + 4;
      piVar6 = piVar6 + 4;
      iVar8 = printf("%08x: %2d\n",piVar4,jlen[iVar8]);
    } while (iVar9 != num_strips);
  }
  return;
}


/* ==== printCalcIworldNonInertialForceandVhmfInput ==== */

/* WARNING: Unknown calling convention */

void printCalcIworldNonInertialForceandVhmfInput(MdtKeaBody *blist,int num_bodies)

{
  MeReal *pMVar1;
  int iVar2;
  MdtKeaBody *pMVar3;
  int iVar4;
  MeReal *pMVar5;
  int iVar6;
  MdtKeaBody *local_1c;
  int local_18;
  int i;
  
                    /* Unresolved local var: int j@[DW_OP_reg3(EBX)] */
  puts("CalcIworldNonInertialForceandVhmf Input");
  puts("---------------------------------------");
  puts("v");
  i = 0;
  if (num_bodies != 0) {
    local_18 = 0;
    do {
      iVar6 = i * 8;
      iVar4 = 0;
      pMVar5 = blist->vel + local_18 * 4;
      do {
        printf("v[%3d]=%12.6f\n",iVar4 + iVar6,(double)*pMVar5);
        printf("v[%3d]=%12.6f\n",iVar6 + 1 + iVar4,(double)pMVar5[1]);
        printf("v[%3d]=%12.6f\n",iVar6 + 2 + iVar4,(double)pMVar5[2]);
        pMVar1 = pMVar5 + 3;
        iVar2 = iVar6 + 3 + iVar4;
        iVar4 = iVar4 + 4;
        pMVar5 = pMVar5 + 4;
        printf("v[%3d]=%12.6f\n",iVar2,(double)*pMVar1);
      } while (iVar4 != 8);
      putchar(10);
      i = i + 1;
      local_18 = local_18 + 0xf;
    } while (i != num_bodies);
  }
  puts("f");
  i = 0;
  if (num_bodies != 0) {
    local_1c = blist;
    do {
      iVar6 = i * 8;
      iVar4 = 0;
      pMVar3 = local_1c;
      do {
        printf("f[%3d]=%12.6f\n",iVar4 + iVar6,(double)pMVar3->force[0]);
        printf("f[%3d]=%12.6f\n",iVar6 + 1 + iVar4,(double)pMVar3->force[1]);
        printf("f[%3d]=%12.6f\n",iVar6 + 2 + iVar4,(double)pMVar3->force[2]);
        iVar2 = iVar6 + 3 + iVar4;
        iVar4 = iVar4 + 4;
        printf("f[%3d]=%12.6f\n",iVar2,(double)pMVar3->force[3]);
        pMVar3 = (MdtKeaBody *)pMVar3->force;
      } while (iVar4 != 8);
      putchar(10);
      i = i + 1;
      local_1c = local_1c + 1;
    } while (i != num_bodies);
  }
  return;
}


/* ==== printCalcIworldNonInertialForceandVhmfOutput ==== */

/* WARNING: Unknown calling convention */

void printCalcIworldNonInertialForceandVhmfOutput(MeReal *vhmf,MeReal *invIworld,int num_bodies)

{
  int iVar1;
  float *pfVar2;
  int iVar3;
  int iVar4;
  int local_18;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int j@[DW_OP_reg6(ESI)]
                       Unresolved local var: int k@[???] */
  iVar4 = 0;
  puts("CalcIworldNonInertialForceandVhmf Output");
  puts("----------------------------------------");
  puts("invIworld");
  if (num_bodies != 0) {
    local_18 = 0;
    do {
      iVar3 = 0;
      do {
        iVar1 = local_18 + iVar3;
        iVar3 = iVar3 + 1;
        pfVar2 = invIworld + iVar1 * 4;
        printf("%12.6f ",SUB84((double)*pfVar2,0),(int)((ulonglong)(double)*pfVar2 >> 0x20));
        printf("%12.6f ",SUB84((double)pfVar2[1],0),(int)((ulonglong)(double)pfVar2[1] >> 0x20));
        printf("%12.6f ",SUB84((double)pfVar2[2],0),(int)((ulonglong)(double)pfVar2[2] >> 0x20));
        printf("%12.6f ",SUB84((double)pfVar2[3],0),(int)((ulonglong)(double)pfVar2[3] >> 0x20));
        putchar(10);
      } while (iVar3 != 3);
      iVar4 = iVar4 + 1;
      putchar(10);
      local_18 = local_18 + 3;
    } while (iVar4 != num_bodies);
  }
  iVar4 = 0;
  puts("vhmf");
  if (num_bodies << 3 != 0) {
    do {
      printf("vhmf[%d]=%12.6f\n",iVar4,SUB84((double)vhmf[iVar4],0),
             (int)((ulonglong)(double)vhmf[iVar4] >> 0x20));
      printf("vhmf[%d]=%12.6f\n",iVar4 + 1,SUB84((double)vhmf[iVar4 + 1],0),
             (int)((ulonglong)(double)vhmf[iVar4 + 1] >> 0x20));
      printf("vhmf[%d]=%12.6f\n",iVar4 + 2,SUB84((double)vhmf[iVar4 + 2],0),
             (int)((ulonglong)(double)vhmf[iVar4 + 2] >> 0x20));
      iVar3 = iVar4 + 3;
      iVar4 = iVar4 + 4;
      printf("vhmf[%d]=%12.6f\n",iVar3,SUB84((double)vhmf[iVar3],0),
             (int)((ulonglong)(double)vhmf[iVar3] >> 0x20));
    } while (iVar4 != num_bodies << 3);
  }
  return;
}


/* ==== printJinvMandrhsInput ==== */

void printJinvMandrhsInput
               (MeReal *rhs,MdtKeaJBlock (*jmstore) [2],MdtKeaJBlock (*jstore) [2],MeReal *xgamma,
               MeReal *c,MeReal *xi,MdtKeaInverseMassMatrix *invIworld,int (*bl2body) [8],int *jlen,
               MdtKeaVelocity *vhmf,int num_bodies,int num_rows_exc_padding,int num_rows_inc_padding
               ,MeReal stepsize,MeReal gamma)

{
  MeReal gamma_local;
  MeReal stepsize_local;
  int num_bodies_local;
  MdtKeaVelocity *vhmf_local;
  int *jlen_local;
  int (*bl2body_local) [8];
  MdtKeaInverseMassMatrix *invIworld_local;
  MeReal *xi_local;
  MeReal *c_local;
  MeReal *xgamma_local;
  MdtKeaJBlock (*jstore_local) [2];
  MdtKeaJBlock (*jmstore_local) [2];
  
  puts("JinvMandrhs Input");
  puts("-----------------");
  printPtr(rhs,"rhs");
  printPtr(jmstore,"jm");
  printMdtKeaJBlockPairArray(*jstore,num_rows_inc_padding);
  printVec(xgamma,num_rows_exc_padding,"xgamma");
  printVec(c,num_rows_exc_padding,"c");
  printVec(xi,num_rows_exc_padding,"xi");
  printInvMassMatrixArray(invIworld,num_bodies,"invIworld");
  printMdtKeaBl2BodyRowArray(*bl2body,num_rows_exc_padding);
  if (num_rows_exc_padding < 0) {
    num_rows_exc_padding = num_rows_exc_padding + 3;
  }
  printIntVec(jlen,num_rows_exc_padding >> 2,"jlen");
  printMdtKeaVelocityArray(vhmf,num_bodies,"vhmf");
  printMeReal(stepsize,"stepsize");
  printMeReal(gamma,"gamma");
  return;
}


/* ==== printJinvMandrhsOutput ==== */

void printJinvMandrhsOutput
               (MeReal *rhs,MdtKeaJBlock (*jm) [2],int num_rows,int *jlen,int num_rows_inc_padding)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int num_rows_inc_padding_local;
  MdtKeaJBlock (*jm_local) [2];
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  puts("JinvMandrhs Output");
  puts("-----------------");
  iVar2 = 0;
  printf("rhs=%p\n",rhs);
  printf("jm =%p\n",jm);
  if (num_rows != 0) {
    uVar1 = num_rows & 3;
    iVar3 = iVar2;
    if (uVar1 != 0) {
      if (1 < uVar1) {
        if (2 < uVar1) {
          printf("rhs[%3d]=%12.6f\n",0,(double)*rhs);
        }
        uVar1 = (uint)(2 < uVar1);
        iVar2 = uVar1 + 1;
        printf("rhs[%3d]=%12.6f\n",uVar1,(double)rhs[uVar1]);
      }
      iVar3 = iVar2 + 1;
      printf("rhs[%3d]=%12.6f\n",iVar2,(double)rhs[iVar2]);
      if (iVar3 == num_rows) goto LAB_00010c74;
    }
    do {
      printf("rhs[%3d]=%12.6f\n",iVar3,(double)rhs[iVar3]);
      printf("rhs[%3d]=%12.6f\n",iVar3 + 1,(double)rhs[iVar3 + 1]);
      printf("rhs[%3d]=%12.6f\n",iVar3 + 2,(double)rhs[iVar3 + 2]);
      iVar2 = iVar3 + 3;
      iVar3 = iVar3 + 4;
      printf("rhs[%3d]=%12.6f\n",iVar2,(double)rhs[iVar2]);
    } while (iVar3 != num_rows);
  }
LAB_00010c74:
  putchar(10);
  printMdtKeaJBlockPairArray(*jm,num_rows_inc_padding);
  return;
}


/* ==== printCalculateConstraintForcesOutput ==== */

void printCalculateConstraintForcesOutput
               (MdtKeaBody **blist,MdtKeaForcePair *cforces,int num_bodies,int num_constraints)

{
  puts("calculateConstraintAndResultantForces Output");
  puts("--------------------------------------------");
  printMdtKeaForcePairArray(cforces,num_constraints);
  return;
}


/* ==== printCalcJinvMJTInput ==== */

/* WARNING: Unknown calling convention */

void printCalcJinvMJTInput
               (blocktobodyandlen *jinfo,blocktobodyandlen *jminfo,MeReal *J,MeReal *JM,
               int num_blocks)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float *pfVar4;
  float *pfVar5;
  float *pfVar6;
  float *pfVar7;
  float *pfVar8;
  float *pfVar9;
  float *pfVar10;
  float *pfVar11;
  float fVar12;
  float fVar13;
  int iVar14;
  int *piVar15;
  int *piVar16;
  int iVar17;
  int iVar18;
  int local_34;
  int local_30;
  int *local_2c;
  int local_28;
  int local_24;
  blocktobodyandlen *pjinfo;
  blocktobodyandlen *pjminfo;
  int chunk;
  int i;
  
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)]
                       Unresolved local var: int k@[???]
                       Unresolved local var: int strip@[DW_OP_reg7(EDI)]
                       Unresolved local var: int block@[DW_OP_reg6(ESI)]
                       Unresolved local var: int elt@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeReal * pJM@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal * pJ@[DW_OP_reg3(EBX)] */
  puts("CalcJinvMJT input");
  puts("-----------------");
  putchar(10);
  printf("num_blocks=%d\n",num_blocks);
  putchar(10);
  puts("norms of jm strips");
  pjminfo = jminfo;
  chunk = 0;
  if (num_blocks != 0) {
    do {
      iVar18 = 0;
      do {
        iVar17 = 0;
        if (pjminfo->len[iVar18] != 0) {
          do {
                    /* Unresolved local var: MeReal norm@[DW_OP_reg12(ST1)] */
            fVar13 = 0.0;
            iVar14 = 0;
            do {
              fVar12 = *JM;
              iVar14 = iVar14 + 0xc;
              pfVar1 = JM + 1;
              pfVar2 = JM + 2;
              pfVar3 = JM + 3;
              pfVar4 = JM + 4;
              pfVar5 = JM + 5;
              pfVar6 = JM + 6;
              pfVar7 = JM + 7;
              pfVar8 = JM + 8;
              pfVar9 = JM + 9;
              pfVar10 = JM + 10;
              pfVar11 = JM + 0xb;
              JM = JM + 0xc;
              fVar13 = *pfVar11 * *pfVar11 +
                       *pfVar10 * *pfVar10 +
                       *pfVar9 * *pfVar9 +
                       *pfVar8 * *pfVar8 +
                       *pfVar7 * *pfVar7 +
                       *pfVar6 * *pfVar6 +
                       *pfVar5 * *pfVar5 +
                       *pfVar4 * *pfVar4 +
                       *pfVar3 * *pfVar3 +
                       *pfVar2 * *pfVar2 + *pfVar1 * *pfVar1 + fVar12 * fVar12 + fVar13;
            } while (iVar14 != 0x18);
            iVar17 = iVar17 + 1;
            printf("%8.2f ",(double)fVar13);
          } while (iVar17 != pjminfo->len[iVar18]);
        }
        iVar18 = iVar18 + 1;
        putchar(10);
      } while (iVar18 != 3);
      chunk = chunk + 1;
      pjminfo = pjminfo + 1;
    } while (chunk != num_blocks);
  }
  iVar18 = puts("norms of j strips");
  chunk = 0;
  pjinfo = jinfo;
  if (num_blocks != 0) {
    do {
      iVar17 = 0;
      do {
        iVar18 = 0;
        if (pjinfo->len[iVar17] != 0) {
          do {
                    /* Unresolved local var: MeReal norm@[DW_OP_reg12(ST1)] */
            fVar13 = 0.0;
            iVar14 = 0;
            do {
              fVar12 = *J;
              iVar14 = iVar14 + 0xc;
              pfVar1 = J + 1;
              pfVar2 = J + 2;
              pfVar3 = J + 3;
              pfVar4 = J + 4;
              pfVar5 = J + 5;
              pfVar6 = J + 6;
              pfVar7 = J + 7;
              pfVar8 = J + 8;
              pfVar9 = J + 9;
              pfVar10 = J + 10;
              pfVar11 = J + 0xb;
              J = J + 0xc;
              fVar13 = *pfVar11 * *pfVar11 +
                       *pfVar10 * *pfVar10 +
                       *pfVar9 * *pfVar9 +
                       *pfVar8 * *pfVar8 +
                       *pfVar7 * *pfVar7 +
                       *pfVar6 * *pfVar6 +
                       *pfVar5 * *pfVar5 +
                       *pfVar4 * *pfVar4 +
                       *pfVar3 * *pfVar3 +
                       *pfVar2 * *pfVar2 + *pfVar1 * *pfVar1 + fVar12 * fVar12 + fVar13;
            } while (iVar14 != 0x18);
            iVar18 = iVar18 + 1;
            printf("%8.2f ",(double)fVar13);
          } while (iVar18 != pjinfo->len[iVar17]);
        }
        iVar17 = iVar17 + 1;
        iVar18 = putchar(10);
      } while (iVar17 != 3);
      chunk = chunk + 1;
      pjinfo = pjinfo + 1;
    } while (chunk != num_blocks);
  }
  piVar15 = (int *)printf("jmblocktobodyandlen is at %p\n",jminfo,iVar18,iVar18);
  i = 0;
  if (num_blocks != 0) {
    local_30 = 0;
    local_34 = 0;
    local_2c = &jminfo->qwc;
    do {
      iVar18 = 0;
      do {
        iVar17 = iVar18 * 0x20 + local_34;
        iVar18 = iVar18 + 1;
        printf("%2d",*(undefined4 *)((int)jminfo->blocktobody + iVar17),piVar15,piVar15);
        printf("%2d",*(undefined4 *)((int)jminfo->blocktobody + iVar17 + 4));
        printf("%2d",*(undefined4 *)((int)jminfo->blocktobody + iVar17 + 8));
        printf("%2d",*(undefined4 *)((int)jminfo->blocktobody + iVar17 + 0xc));
        printf("%2d",*(undefined4 *)((int)jminfo->blocktobody + iVar17 + 0x10));
        printf("%2d",*(undefined4 *)((int)jminfo->blocktobody + iVar17 + 0x14));
        printf("%2d",*(undefined4 *)((int)jminfo->blocktobody + iVar17 + 0x18));
        printf("%2d");
        piVar15 = (int *)putchar(10);
      } while (iVar18 != 3);
      piVar16 = jminfo->len + local_30 * 4;
      printf("%2d",*piVar16,piVar15,piVar15);
      printf("%2d",piVar16[1]);
      printf("%2d",piVar16[2]);
      putchar(10);
      printf("%2d\n");
      i = i + 1;
      piVar15 = local_2c + 0x1c;
      local_30 = local_30 + 7;
      local_34 = local_34 + 0x70;
      local_2c = piVar15;
    } while (i != num_blocks);
  }
  iVar18 = printf("jblocktobodyandlen is at %p\n",jinfo,piVar15,piVar15);
  i = 0;
  if (num_blocks != 0) {
    local_24 = 0;
    local_28 = 0;
    do {
      iVar17 = 0;
      do {
        iVar14 = iVar17 * 0x20 + local_28;
        iVar17 = iVar17 + 1;
        printf("%2d",*(undefined4 *)((int)jinfo->blocktobody + iVar14),iVar18,iVar18);
        printf("%2d",*(undefined4 *)((int)jinfo->blocktobody + iVar14 + 4));
        printf("%2d",*(undefined4 *)((int)jinfo->blocktobody + iVar14 + 8));
        printf("%2d",*(undefined4 *)((int)jinfo->blocktobody + iVar14 + 0xc));
        printf("%2d",*(undefined4 *)((int)jinfo->blocktobody + iVar14 + 0x10));
        printf("%2d",*(undefined4 *)((int)jinfo->blocktobody + iVar14 + 0x14));
        printf("%2d",*(undefined4 *)((int)jinfo->blocktobody + iVar14 + 0x18));
        printf("%2d");
        iVar18 = putchar(10);
      } while (iVar17 != 3);
      piVar15 = jinfo->len + local_24 * 4;
      printf("%2d",*piVar15,3,3);
      printf("%2d",piVar15[1]);
      printf("%2d",piVar15[2]);
      putchar(10);
      printf("%2d\n");
      iVar18 = i + 1;
      local_24 = local_24 + 7;
      local_28 = local_28 + 0x70;
      i = iVar18;
    } while (iVar18 != num_blocks);
  }
  return;
}


/* ==== printFactoriseps2sparse_Output ==== */

void printFactoriseps2sparse_Output(MeReal *A,int *rlist,int *rlist_len,int num_blocks)

{
  float *pfVar1;
  float fVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  MeReal *pMVar13;
  int iVar14;
  int num_blocks_local;
  int *rlist_len_local;
  int *rlist_local;
  MeReal *A_local;
  
                    /* Unresolved local var: int i@[DW_OP_reg0(EAX)]
                       Unresolved local var: int j@[DW_OP_reg6(ESI)]
                       Unresolved local var: int k@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeReal * pa@[DW_OP_reg3(EBX)] */
  puts("ps2Sparse factoriser output");
  puts("---------------------------");
  puts("norms of Achol blocks (not necessarily in the order they are the actual matrix)");
  iVar11 = 0;
  pMVar13 = A;
  if (num_blocks != 0) {
    do {
      iVar14 = 0;
      iVar11 = iVar11 + 1;
      if (iVar11 != 0) {
        do {
                    /* Unresolved local var: MeReal norm@[DW_OP_reg12(ST1)] */
          fVar2 = 0.0;
          iVar12 = 0;
          do {
            pfVar1 = pMVar13 + iVar12;
            iVar3 = iVar12 + 1;
            iVar4 = iVar12 + 2;
            iVar5 = iVar12 + 3;
            iVar6 = iVar12 + 4;
            iVar7 = iVar12 + 5;
            iVar8 = iVar12 + 6;
            iVar9 = iVar12 + 7;
            iVar10 = iVar12 + 8;
            iVar12 = iVar12 + 9;
            fVar2 = pMVar13[iVar10] * pMVar13[iVar10] +
                    pMVar13[iVar9] * pMVar13[iVar9] +
                    pMVar13[iVar8] * pMVar13[iVar8] +
                    pMVar13[iVar7] * pMVar13[iVar7] +
                    pMVar13[iVar6] * pMVar13[iVar6] +
                    pMVar13[iVar5] * pMVar13[iVar5] +
                    pMVar13[iVar4] * pMVar13[iVar4] +
                    pMVar13[iVar3] * pMVar13[iVar3] + *pfVar1 * *pfVar1 + fVar2;
          } while (iVar12 != 0x90);
          iVar14 = iVar14 + 1;
          pMVar13 = pMVar13 + 0x90;
          printf("%12.6f ",(double)fVar2);
        } while (iVar14 != iVar11);
      }
      putchar(10);
    } while (iVar11 != num_blocks);
  }
  printPS2SparseMatrix(A,rlist,rlist_len,num_blocks);
  return;
}


/* ==== printCalcJinvMJTps2sparse_Output ==== */

void printCalcJinvMJTps2sparse_Output(MeReal *A,int *rlist,int *rlist_len,int num_blocks)

{
  float *pfVar1;
  int iVar2;
  bool bVar3;
  float fVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  MeReal *pMVar14;
  int *piVar15;
  int iVar16;
  int local_28;
  int i;
  int num_blocks_local;
  int *rlist_len_local;
  int *rlist_local;
  MeReal *A_local;
  
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)]
                       Unresolved local var: int k@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeReal * pa@[DW_OP_reg3(EBX)] */
  puts("ps2sparse CalcJinvMJT output");
  puts("----------------------------");
  i = 0;
  pMVar14 = A;
  if (num_blocks == 0) goto LAB_000115fa;
  do {
    iVar16 = 0;
    i = i + 1;
    if (i != 0) {
      do {
                    /* Unresolved local var: MeReal norm@[DW_OP_reg12(ST1)] */
        fVar4 = 0.0;
        iVar12 = 0;
        do {
          pfVar1 = pMVar14 + iVar12;
          iVar2 = iVar12 + 1;
          iVar5 = iVar12 + 2;
          iVar6 = iVar12 + 3;
          iVar7 = iVar12 + 4;
          iVar8 = iVar12 + 5;
          iVar9 = iVar12 + 6;
          iVar10 = iVar12 + 7;
          iVar11 = iVar12 + 8;
          iVar12 = iVar12 + 9;
          fVar4 = pMVar14[iVar11] * pMVar14[iVar11] +
                  pMVar14[iVar10] * pMVar14[iVar10] +
                  pMVar14[iVar9] * pMVar14[iVar9] +
                  pMVar14[iVar8] * pMVar14[iVar8] +
                  pMVar14[iVar7] * pMVar14[iVar7] +
                  pMVar14[iVar6] * pMVar14[iVar6] +
                  pMVar14[iVar5] * pMVar14[iVar5] +
                  pMVar14[iVar2] * pMVar14[iVar2] + *pfVar1 * *pfVar1 + fVar4;
        } while (iVar12 != 0x90);
        iVar16 = iVar16 + 1;
        pMVar14 = pMVar14 + 0x90;
        printf("%12.6f ",(double)fVar4);
      } while (iVar16 != i);
    }
    iVar16 = putchar(10);
  } while (i != num_blocks);
  i = 0;
  if (num_blocks == 0) goto LAB_000115fa;
  local_28 = 0;
  if ((num_blocks & 1U) == 0) {
LAB_0001146d:
    do {
      iVar16 = 0;
      if (rlist_len[i] != 0) {
        piVar15 = rlist + local_28;
        iVar12 = local_28;
        do {
          iVar16 = iVar16 + 1;
          iVar2 = *piVar15;
          piVar15 = piVar15 + 1;
          iVar12 = printf("%d ",iVar2,iVar12,iVar12);
        } while (iVar16 != rlist_len[i]);
      }
      putchar(10);
      local_28 = local_28 + num_blocks;
      iVar16 = 0;
      if (rlist_len[i + 1] != 0) {
        piVar15 = rlist + local_28;
        iVar12 = local_28;
        do {
          iVar16 = iVar16 + 1;
          iVar2 = *piVar15;
          piVar15 = piVar15 + 1;
          iVar12 = printf("%d ",iVar2,iVar12,iVar12);
        } while (iVar16 != rlist_len[i + 1]);
      }
      putchar(10);
      iVar16 = i + 2;
      local_28 = local_28 + num_blocks;
      i = iVar16;
    } while (iVar16 != num_blocks);
  }
  else {
    iVar12 = 0;
    piVar15 = rlist;
    if (*rlist_len != 0) {
      do {
        iVar12 = iVar12 + 1;
        iVar16 = printf("%d ",*piVar15,iVar16,iVar16);
        piVar15 = piVar15 + 1;
      } while (iVar12 != *rlist_len);
    }
    putchar(10);
    iVar16 = 1;
    local_28 = num_blocks;
    i = iVar16;
    if (num_blocks != 1) goto LAB_0001146d;
  }
  i = 0;
  if (num_blocks != 0) {
    uVar13 = num_blocks & 3;
    if (uVar13 != 0) {
      if (1 < uVar13) {
        bVar3 = 2 < uVar13;
        if (bVar3) {
          uVar13 = printf("len=%d\n",*rlist_len,uVar13,uVar13);
        }
        i = (int)bVar3;
        uVar13 = printf("len=%d\n",rlist_len[i],uVar13,uVar13);
        i = i + 1;
      }
      iVar16 = rlist_len[i];
      printf("len=%d\n",iVar16,uVar13,uVar13);
      i = i + 1;
      if (i == num_blocks) goto LAB_000115fa;
    }
    do {
      printf("len=%d\n",rlist_len[i],iVar16,iVar16);
      printf("len=%d\n",rlist_len[i + 1]);
      printf("len=%d\n",rlist_len[i + 2]);
      iVar16 = rlist_len[i + 3];
      printf("len=%d\n");
      i = i + 4;
    } while (i != num_blocks);
  }
LAB_000115fa:
  printPS2SparseMatrix(A,rlist,rlist_len,num_blocks);
  return;
}


/* ==== printLCPInitialSolveOutput ==== */

void printLCPInitialSolveOutput(MeReal *x,int c4n)

{
  float *pfVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar4 = 0;
  puts("LCP Initial Solve Output");
  puts("------------------------");
  if (c4n != 0) {
    uVar3 = c4n & 3;
    if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
          printf("%12.6f\n",(double)*x);
        }
        uVar3 = (uint)(2 < uVar3);
        iVar4 = uVar3 + 1;
        printf("%12.6f\n",(double)x[uVar3]);
      }
      pfVar1 = x + iVar4;
      iVar4 = iVar4 + 1;
      printf("%12.6f\n",(double)*pfVar1);
      if (iVar4 == c4n) {
        return;
      }
    }
    do {
      printf("%12.6f\n",(double)x[iVar4]);
      printf("%12.6f\n",(double)x[iVar4 + 1]);
      printf("%12.6f\n",(double)x[iVar4 + 2]);
      iVar2 = iVar4 + 3;
      iVar4 = iVar4 + 4;
      printf("%12.6f\n",(double)x[iVar2]);
    } while (iVar4 != c4n);
  }
  return;
}


/* ==== printSolvePCSparseSSEOutput ==== */

void printSolvePCSparseSSEOutput(MeReal *x,int m_padded)

{
  puts("PCSparseSSE solve output");
  puts("------------------------");
  printVec(x,m_padded,"solve result");
  putchar(10);
  return;
}


/* ==== printCalcConstraintForcesInput ==== */

void printCalcConstraintForcesInput
               (MeReal *lambda,int (*bl2cbody) [8],int (*bl2body) [8],int c4size)

{
  float *pfVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int (*bl2cbody_local) [8];
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar4 = 0;
  puts("calculateConstraintAndResultantForces Input");
  puts("-------------------------------------------");
  puts("lambda:");
  if (c4size != 0) {
    uVar3 = c4size & 3;
    if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
          printf("% 12.6f\n",(double)*lambda);
        }
        uVar3 = (uint)(2 < uVar3);
        iVar4 = uVar3 + 1;
        printf("% 12.6f\n",(double)lambda[uVar3]);
      }
      pfVar1 = lambda + iVar4;
      iVar4 = iVar4 + 1;
      printf("% 12.6f\n",(double)*pfVar1);
      if (iVar4 == c4size) goto LAB_00011873;
    }
    do {
      printf("% 12.6f\n",(double)lambda[iVar4]);
      printf("% 12.6f\n",(double)lambda[iVar4 + 1]);
      printf("% 12.6f\n",(double)lambda[iVar4 + 2]);
      iVar2 = iVar4 + 3;
      iVar4 = iVar4 + 4;
      printf("% 12.6f\n",(double)lambda[iVar2]);
    } while (iVar4 != c4size);
  }
LAB_00011873:
  putchar(10);
  puts("bl2cbody:");
  if (c4size < 0) {
    c4size = c4size + 3;
  }
  printMdtKeaBl2CBodyRowArray(*bl2cbody,c4size >> 2);
  putchar(10);
  return;
}


/* ==== printMakeFromColMajorPSMInput ==== */

void printMakeFromColMajorPSMInput
               (MeReal *Ainv,MeReal *clampedValues,MeReal *initialSolve,int *unclamped,int *clamped,
               int numUnclamped,int numClamped,int n_padded,int AinvStride)

{
  int *clamped_local;
  MeReal *Ainv_local;
  
  puts("PPT make Q input");
  puts("----------------");
  putchar(10);
  printf("numUnclamped=%d\n",numUnclamped);
  printf("numClamped=%d\n",numClamped);
  putchar(10);
  printIntVec(clamped,numClamped,"clamped");
  printColMajorMat(Ainv,AinvStride,AinvStride,"Ainv");
  putchar(10);
  return;
}


/* ==== printMakeFromColMajorPSMOutput ==== */

void printMakeFromColMajorPSMOutput
               (MeReal *rsD,MeReal **NAZ,MeReal **NCZ,int *NR,int *NC,int m_blocks,int m_padded)

{
  MeReal *pMVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  int extraout_EDX;
  MeReal **ppMVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  double dVar9;
  int local_2c;
  int local_28;
  int i;
  int m_padded_local;
  int m_blocks_local;
  MeReal **NCZ_local;
  
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)]
                       Unresolved local var: int k@[DW_OP_reg3(EBX)] */
  puts("PCsparse make Q output");
  puts("----------------------");
  i = 0;
  if (m_blocks != 0) {
    iVar8 = 0;
    iVar7 = m_blocks;
    do {
      iVar6 = 0;
      if (m_blocks != 0) {
        ppMVar5 = NCZ + iVar8;
        uVar3 = m_blocks & 3;
        iVar4 = 0;
        if (uVar3 != 0) {
          if (1 < uVar3) {
            bVar2 = 2 < uVar3;
            if (bVar2) {
              pMVar1 = *ppMVar5;
              ppMVar5 = ppMVar5 + 1;
              uVar3 = printf("%08x ",pMVar1,iVar7,iVar7);
            }
            iVar6 = bVar2 + 1;
            pMVar1 = *ppMVar5;
            ppMVar5 = ppMVar5 + 1;
            uVar3 = printf("%08x ",pMVar1,uVar3,uVar3);
          }
          iVar6 = iVar6 + 1;
          pMVar1 = *ppMVar5;
          ppMVar5 = ppMVar5 + 1;
          iVar4 = printf("%08x ",pMVar1,uVar3,uVar3);
          if (iVar6 == m_blocks) goto LAB_00011a6e;
        }
        do {
          iVar6 = iVar6 + 4;
          printf("%08x ",*ppMVar5,iVar4,iVar4);
          printf("%08x ",ppMVar5[1]);
          printf("%08x ",ppMVar5[2]);
          ppMVar5 = ppMVar5 + 4;
          iVar4 = printf("%08x ");
        } while (iVar6 != m_blocks);
      }
LAB_00011a6e:
      putchar(10);
      i = i + 1;
      iVar8 = iVar8 + m_blocks;
      iVar7 = extraout_EDX;
    } while (i != m_blocks);
  }
  putchar(10);
  puts("Q:");
  i = 0;
  if (0 < m_padded) {
    do {
      iVar7 = 0;
      if (0 < m_blocks) {
        local_28 = 0;
        local_2c = 0;
        do {
          if (NCZ[local_2c + (i >> 2)] == (MeReal *)0x0) {
            printf("0.00 ");
            printf("0.00 ");
            printf("0.00 ");
            printf("0.00 ");
          }
          else {
            iVar8 = 0;
            do {
              if (iVar8 + iVar7 * 4 < i) {
                dVar9 = 0.0;
              }
              else {
                dVar9 = (double)NCZ[(i >> 2) + local_28][(i & 3U) * 4 + iVar8];
              }
              iVar8 = iVar8 + 1;
              printf("% 6.2f ",dVar9);
            } while (iVar8 < 4);
          }
          local_28 = local_28 + m_blocks;
          local_2c = local_2c + m_blocks;
          iVar7 = iVar7 + 1;
        } while (iVar7 < m_blocks);
      }
      putchar(10);
      i = i + 1;
    } while (i < m_padded);
  }
  putchar(10);
  return;
}


/* ==== printMakeFromColMajorPSMOutput_ps2smalldense ==== */

void printMakeFromColMajorPSMOutput_ps2smalldense(MeReal *Q,int c4numRows)

{
  puts("PS2 smalldense MakeFromColMajorPSM Output");
  puts("-----------------------------------------");
  printHalfWallpaperMatrix(Q,c4numRows);
  return;
}


/* ==== printMakeFromColMajorPSMOutput_ps2sparse ==== */

void printMakeFromColMajorPSMOutput_ps2sparse
               (MeReal *matrix,int *rlist,int *rlist_len,int num_12_blocks)

{
  MeReal *matrix_local;
  
  puts("PS2 sparse MakeFromColMajorPSM Output");
  puts("-------------------------------------");
  printPS2SparseMatrix(matrix,rlist,rlist_len,num_12_blocks);
  return;
}


/* ==== printFactoriserPS2SmallDenseOutput ==== */

void printFactoriserPS2SmallDenseOutput(MeReal *A,int c4numRows)

{
  puts("PS2SmallDense Factoriser Output");
  puts("-------------------------------");
  printHalfWallpaperMatrix(A,c4numRows);
  return;
}


/* ==== printFactoriserPCSparseOutput ==== */

void printFactoriserPCSparseOutput(MeReal **NCZ,int m_blocks,int m_padded)

{
  int iVar1;
  int iVar2;
  char *__format;
  int local_2c;
  int j;
  int m_padded_local;
  int m_blocks_local;
  MeReal **NCZ_local;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int k@[DW_OP_reg3(EBX)] */
  iVar2 = 0;
  puts("PC Sparse Factoriser Output");
  puts("---------------------------");
  putchar(10);
  if (0 < m_padded) {
    do {
      j = 0;
      if (0 < m_blocks) {
        local_2c = 0;
        do {
          if (NCZ[local_2c + (iVar2 >> 2)] == (MeReal *)0x0) {
            printf("0.00 ");
            printf("0.00 ");
            printf("0.00 ");
            printf("0.00 ");
          }
          else {
            iVar1 = 0;
            do {
              if (iVar1 + j * 4 < iVar2) {
                __format = "x.xx ";
              }
              else {
                __format = "%4.2f ";
              }
              iVar1 = iVar1 + 1;
              printf(__format);
            } while (iVar1 < 4);
          }
          j = j + 1;
          local_2c = local_2c + m_blocks;
        } while (j < m_blocks);
      }
      iVar2 = iVar2 + 1;
      putchar(10);
    } while (iVar2 < m_padded);
  }
  putchar(10);
  putchar(10);
  return;
}


/* ==== printPrinciplePivotTransformOutput ==== */

void printPrinciplePivotTransformOutput(MeReal *x,MeReal *w,int n)

{
  puts("PPT output");
  putchar(10);
  printVec(x,n,"x");
  printVec(w,n,"w");
  putchar(10);
  return;
}


/* ==== printGetFirstBadIndexInput ==== */

void printGetFirstBadIndexInput(MeReal *x,int n)

{
  puts("getFirstBadIndexInput");
  puts("---------------------");
  if ((n & 3U) != 0) {
    if (n < 0) {
      n = n + 3;
    }
    n = (n & 0xfffffffcU) + 4;
  }
  printVec(x,n,"initial solve result");
  putchar(10);
  return;
}


/* ==== printSolveInput ==== */

void printSolveInput(MeReal *rhs,int num_blocks)

{
  puts("solve input");
  puts("-----------");
  printVec(rhs,num_blocks << 2,"rhs");
  putchar(10);
  return;
}


/* ==== printLCPOutput ==== */

/* WARNING: Unknown calling convention */

void printLCPOutput(MeReal *x,int c4numRows)

{
  printVec(x,c4numRows,"lambda");
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


