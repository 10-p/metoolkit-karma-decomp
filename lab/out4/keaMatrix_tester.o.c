/* ==== checkNorm ==== */

void checkNorm(MeReal *a,MeReal *b,int numElts,char *desc)

{
  int iVar1;
  float *pfVar2;
  float *pfVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
  MeReal norm;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar10 = 0;
  norm = 0.0;
  if (numElts != 0) {
    uVar9 = numElts & 3;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        if (2 < uVar9) {
          norm = (*a - *b) * (*a - *b);
        }
        uVar9 = (uint)(2 < uVar9);
        iVar10 = uVar9 + 1;
        norm = (a[uVar9] - b[uVar9]) * (a[uVar9] - b[uVar9]) + norm;
      }
      pfVar2 = b + iVar10;
      pfVar3 = a + iVar10;
      iVar10 = iVar10 + 1;
      norm = (*pfVar3 - *pfVar2) * (*pfVar3 - *pfVar2) + norm;
      if (iVar10 == numElts) goto LAB_000100a8;
    }
    do {
      pfVar2 = b + iVar10;
      pfVar3 = a + iVar10;
      iVar1 = iVar10 + 1;
      iVar4 = iVar10 + 1;
      iVar5 = iVar10 + 2;
      iVar6 = iVar10 + 2;
      iVar7 = iVar10 + 3;
      iVar8 = iVar10 + 3;
      iVar10 = iVar10 + 4;
      norm = (a[iVar7] - b[iVar8]) * (a[iVar7] - b[iVar8]) +
             (a[iVar5] - b[iVar6]) * (a[iVar5] - b[iVar6]) +
             (a[iVar1] - b[iVar4]) * (a[iVar1] - b[iVar4]) +
             (*pfVar3 - *pfVar2) * (*pfVar3 - *pfVar2) + norm;
    } while (iVar10 != numElts);
  }
LAB_000100a8:
  if (norm <= 10.0) {
    return;
  }
  iVar10 = 0;
  if (numElts != 0) {
    uVar9 = numElts & 3;
    if (uVar9 != 0) {
      if (1 < uVar9) {
        if (2 < uVar9) {
          printf("%12.6f %12.6f %12.6f\n",(double)*a,(double)*b,(double)(*a - *b));
        }
        uVar9 = (uint)(2 < uVar9);
        iVar10 = uVar9 + 1;
        printf("%12.6f %12.6f %12.6f\n",(double)a[uVar9],(double)b[uVar9],
               (double)(a[uVar9] - b[uVar9]));
      }
      pfVar2 = a + iVar10;
      pfVar3 = b + iVar10;
      iVar10 = iVar10 + 1;
      printf("%12.6f %12.6f %12.6f\n",(double)*pfVar2,(double)*pfVar3,(double)(*pfVar2 - *pfVar3));
      if (iVar10 == numElts) goto LAB_000101cc;
    }
    do {
      printf("%12.6f %12.6f %12.6f\n",(double)a[iVar10],(double)b[iVar10],
             (double)(a[iVar10] - b[iVar10]));
      printf("%12.6f %12.6f %12.6f\n",(double)a[iVar10 + 1],(double)b[iVar10 + 1],
             (double)(a[iVar10 + 1] - b[iVar10 + 1]));
      printf("%12.6f %12.6f %12.6f\n",(double)a[iVar10 + 2],(double)b[iVar10 + 2],
             (double)(a[iVar10 + 2] - b[iVar10 + 2]));
      iVar1 = iVar10 + 3;
      iVar10 = iVar10 + 4;
      printf("%12.6f %12.6f %12.6f\n",(double)a[iVar1],(double)b[iVar1],
             (double)(a[iVar1] - b[iVar1]));
    } while (iVar10 != numElts);
  }
LAB_000101cc:
  putchar(10);
  MeWarning(0,"%s bad norm %e\n");
  return;
}


/* ==== allocate ==== */

/* DWARF original prototype: void allocate(keaMatrix_tester * this, int n) */

void keaMatrix_tester::allocate(int n)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  uint in_stack_00000008;
  
                    /* Unresolved local var: int c16c12n@[DW_OP_reg3(EBX)] */
  *(uint *)(n + 4) = in_stack_00000008;
  uVar4 = in_stack_00000008;
  if ((in_stack_00000008 & 3) != 0) {
    if ((int)in_stack_00000008 < 0) {
      uVar4 = in_stack_00000008 + 3;
    }
    uVar4 = (uVar4 & 0xfffffffc) + 4;
  }
  *(uint *)(n + 8) = uVar4;
  (**(code **)**(undefined4 **)(n + 0x14))(*(undefined4 *)(n + 0x14));
  (**(code **)**(undefined4 **)(n + 0x18))(*(undefined4 *)(n + 0x18));
  uVar4 = in_stack_00000008;
  if ((int)in_stack_00000008 % 0xc != 0) {
    uVar4 = (in_stack_00000008 - (int)in_stack_00000008 % 0xc) + 0xc;
  }
  if ((uVar4 & 0xf) == 0) {
    if ((int)in_stack_00000008 % 0xc != 0) {
      in_stack_00000008 = (in_stack_00000008 - (int)in_stack_00000008 % 0xc) + 0xc;
    }
  }
  else {
    uVar4 = in_stack_00000008;
    if ((int)in_stack_00000008 % 0xc != 0) {
      uVar4 = (in_stack_00000008 - (int)in_stack_00000008 % 0xc) + 0xc;
    }
    if ((int)in_stack_00000008 % 0xc == 0) {
      iVar2 = (int)in_stack_00000008 % 0x10;
    }
    else {
      iVar2 = in_stack_00000008 - (int)in_stack_00000008 % 0xc;
      uVar1 = iVar2 + 0xc;
      uVar5 = uVar1;
      if ((int)uVar1 < 0) {
        uVar5 = iVar2 + 0x1b;
      }
      iVar2 = uVar1 - (uVar5 & 0xfffffff0);
    }
    in_stack_00000008 = (uVar4 - iVar2) + 0x10;
  }
  uVar4 = in_stack_00000008 * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar2 = uVar4 + 0x40;
  }
  else {
    iVar2 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar2,"cached");
  *(undefined4 *)(n + 0x1c) = uVar3;
  uVar4 = in_stack_00000008 * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar2 = uVar4 + 0x40;
  }
  else {
    iVar2 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar2,"cached");
  *(undefined4 *)(n + 0x20) = uVar3;
  uVar4 = in_stack_00000008 * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar2 = uVar4 + 0x40;
  }
  else {
    iVar2 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar2,"Qrhs");
  *(undefined4 *)(n + 0x24) = uVar3;
  uVar4 = in_stack_00000008 * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar2 = uVar4 + 0x40;
  }
  else {
    iVar2 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar2,"Qrhs");
  *(undefined4 *)(n + 0x28) = uVar3;
  uVar4 = in_stack_00000008 * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar2 = uVar4 + 0x40;
  }
  else {
    iVar2 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar2,"x");
  *(undefined4 *)(n + 0x2c) = uVar3;
  uVar4 = in_stack_00000008 * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar2 = uVar4 + 0x40;
  }
  else {
    iVar2 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar2,"x");
  *(undefined4 *)(n + 0x30) = uVar3;
  uVar4 = in_stack_00000008 * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar2 = uVar4 + 0x40;
  }
  else {
    iVar2 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar2,"w");
  *(undefined4 *)(n + 0x34) = uVar3;
  uVar4 = in_stack_00000008 * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar2 = uVar4 + 0x40;
  }
  else {
    iVar2 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar2,"w");
  *(undefined4 *)(n + 0x38) = uVar3;
  uVar4 = in_stack_00000008 * in_stack_00000008 * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar2 = uVar4 + 0x40;
  }
  else {
    iVar2 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar2,"Ainv");
  *(undefined4 *)(n + 0x3c) = uVar3;
  uVar4 = in_stack_00000008 * in_stack_00000008 * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar2 = uVar4 + 0x40;
  }
  else {
    iVar2 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  iVar2 = keaPoolAlloc(iVar2,"Ainv");
  *(uint *)(n + 0x1c) = *(int *)(n + 0x1c) + 0x3fU & 0xffffffc0;
  *(uint *)(n + 0x20) = *(int *)(n + 0x20) + 0x3fU & 0xffffffc0;
  *(uint *)(n + 0x24) = *(int *)(n + 0x24) + 0x3fU & 0xffffffc0;
  *(uint *)(n + 0x28) = *(int *)(n + 0x28) + 0x3fU & 0xffffffc0;
  *(uint *)(n + 0x2c) = *(int *)(n + 0x2c) + 0x3fU & 0xffffffc0;
  *(uint *)(n + 0x30) = *(int *)(n + 0x30) + 0x3fU & 0xffffffc0;
  *(uint *)(n + 0x34) = *(int *)(n + 0x34) + 0x3fU & 0xffffffc0;
  *(uint *)(n + 0x38) = *(int *)(n + 0x38) + 0x3fU & 0xffffffc0;
  *(uint *)(n + 0x3c) = *(int *)(n + 0x3c) + 0x3fU & 0xffffffc0;
  *(uint *)(n + 0x40) = iVar2 + 0x3fU & 0xffffffc0;
  return;
}


/* ==== makeFromJMJT ==== */

/* DWARF original prototype: void makeFromJMJT(keaMatrix_tester * this, MeReal * JM, MeReal * Js,
   int * num_in_strip, int * block2body, MeReal * slipfactor, MeReal epsilon, MeReal hinv) */

void keaMatrix_tester::makeFromJMJT
               (MeReal *JM,MeReal *Js,int *num_in_strip,int *block2body,MeReal *slipfactor,
               MeReal epsilon,MeReal hinv)

{
  (**(code **)(*(int *)JM[5] + 4))(JM[5],Js,num_in_strip,block2body,slipfactor,epsilon,hinv);
  (**(code **)(*(int *)JM[6] + 4))(JM[6],Js,num_in_strip,block2body,slipfactor,epsilon,hinv);
  return;
}


/* ==== makeFromColMajorPSM ==== */

/* DWARF original prototype: void makeFromColMajorPSM(keaMatrix_tester * this, MeReal * Qrhs, MeReal
   * Ainv, MeReal * clampedValues, MeReal * initialSolve, int * unclamped, int * clamped, int
   numUnclamped, int numClamped, int n_padded) */

void keaMatrix_tester::makeFromColMajorPSM
               (MeReal *Qrhs,MeReal *Ainv,MeReal *clampedValues,MeReal *initialSolve,int *unclamped,
               int *clamped,int numUnclamped,int numClamped,int n_padded)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  undefined4 in_stack_00000028;
  
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)] */
  (**(code **)(*(int *)Qrhs[5] + 8))
            (Qrhs[5],Qrhs[9],clampedValues,initialSolve,unclamped,clamped,numUnclamped,numClamped,
             n_padded);
  (**(code **)(*(int *)Qrhs[6] + 8))
            (Qrhs[6],Qrhs[10],clampedValues,initialSolve,unclamped,clamped,numUnclamped,numClamped,
             n_padded,in_stack_00000028);
  checkNorm((MeReal *)Qrhs[10],(MeReal *)Qrhs[9],n_padded,"make from col major PSM");
  iVar3 = 0;
  if (n_padded != 0) {
    uVar2 = n_padded & 3;
    if (uVar2 != 0) {
      if (1 < uVar2) {
        if (2 < uVar2) {
          *Ainv = *(MeReal *)Qrhs[10];
        }
        uVar2 = (uint)(2 < uVar2);
        Ainv[uVar2] = *(MeReal *)((int)Qrhs[10] + uVar2 * 4);
        iVar3 = uVar2 + 1;
      }
      Ainv[iVar3] = *(MeReal *)((int)Qrhs[10] + iVar3 * 4);
      iVar3 = iVar3 + 1;
      if (iVar3 == n_padded) {
        return;
      }
    }
    do {
      Ainv[iVar3] = *(MeReal *)((int)Qrhs[10] + iVar3 * 4);
      Ainv[iVar3 + 1] = *(MeReal *)((int)Qrhs[10] + (iVar3 + 1) * 4);
      Ainv[iVar3 + 2] = *(MeReal *)((int)Qrhs[10] + (iVar3 + 2) * 4);
      iVar1 = iVar3 + 3;
      iVar3 = iVar3 + 4;
      Ainv[iVar1] = *(MeReal *)((int)Qrhs[10] + iVar1 * 4);
    } while (iVar3 != n_padded);
  }
  return;
}


/* ==== factorize ==== */

/* DWARF original prototype: void factorize(keaMatrix_tester * this) */

void keaMatrix_tester::factorize(void)

{
  int in_stack_00000004;
  
  (**(code **)(**(int **)(in_stack_00000004 + 0x14) + 0xc))(*(int **)(in_stack_00000004 + 0x14));
  (**(code **)(**(int **)(in_stack_00000004 + 0x18) + 0xc))(*(int **)(in_stack_00000004 + 0x18));
  return;
}


/* ==== solve ==== */

/* DWARF original prototype: void solve(keaMatrix_tester * this, MeReal * x, MeReal * rhs) */

void keaMatrix_tester::solve(MeReal *x,MeReal *rhs)

{
  MeReal MVar1;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  (**(code **)(*(int *)x[5] + 0x10))(x[5],x[0xb]);
  (**(code **)(*(int *)x[6] + 0x10))(x[6],x[0xc]);
  checkNorm((MeReal *)x[0xc],(MeReal *)x[0xb],(int)x[2],"solve");
  MVar1 = 0.0;
  if (x[2] != 0.0) {
    do {
      rhs[(int)MVar1] = *(MeReal *)((int)x[0xc] + (int)MVar1 * 4);
      MVar1 = (MeReal)((int)MVar1 + 1);
    } while (MVar1 != x[2]);
  }
  return;
}


/* ==== multiply ==== */

/* DWARF original prototype: void multiply(keaMatrix_tester * this, MeReal * b, MeReal * x) */

void keaMatrix_tester::multiply(MeReal *b,MeReal *x)

{
  MeReal MVar1;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  (**(code **)(*(int *)b[5] + 0x14))(b[5],b[0xd]);
  (**(code **)(*(int *)b[6] + 0x14))(b[6],b[0xe]);
  checkNorm((MeReal *)b[0xe],(MeReal *)b[0xd],(int)b[2],"multiply");
  MVar1 = 0.0;
  if (b[2] != 0.0) {
    do {
      x[(int)MVar1] = *(MeReal *)((int)b[0xe] + (int)MVar1 * 4);
      MVar1 = (MeReal)((int)MVar1 + 1);
    } while (MVar1 != b[2]);
  }
  return;
}


/* ==== solveUnits ==== */

/* DWARF original prototype: void solveUnits(keaMatrix_tester * this, MeReal * Ainv, int * cached,
   int * clamped, int numClamped, int AinvStride) */

void keaMatrix_tester::solveUnits
               (MeReal *Ainv,int *cached,int *clamped,int numClamped,int AinvStride)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  MeReal MVar4;
  uint uVar5;
  int iVar6;
  int in_stack_00000018;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  MVar4 = 0.0;
  if (Ainv[1] != 0.0) {
    do {
      *(int *)((int)Ainv[7] + (int)MVar4 * 4) = clamped[(int)MVar4];
      *(int *)((int)Ainv[8] + (int)MVar4 * 4) = clamped[(int)MVar4];
      MVar4 = (MeReal)((int)MVar4 + 1);
    } while (MVar4 != Ainv[1]);
  }
  uVar5 = 0;
  uVar1 = in_stack_00000018 * in_stack_00000018;
  uVar2 = 0;
  if (uVar1 != 0) {
    uVar2 = uVar1 & 3;
    if (uVar2 != 0) {
      if (1 < uVar2) {
        if (2 < uVar2) {
          *(int *)Ainv[0xf] = *cached;
          *(int *)Ainv[0x10] = *cached;
        }
        uVar5 = (uint)(2 < uVar2);
        *(int *)((int)Ainv[0xf] + uVar5 * 4) = cached[uVar5];
        *(int *)((int)Ainv[0x10] + uVar5 * 4) = cached[uVar5];
        uVar5 = uVar5 + 1;
      }
      *(int *)((int)Ainv[0xf] + uVar5 * 4) = cached[uVar5];
      *(int *)((int)Ainv[0x10] + uVar5 * 4) = cached[uVar5];
      uVar5 = uVar5 + 1;
      if (uVar5 == uVar1) goto LAB_0001095a;
    }
    do {
      *(int *)((int)Ainv[0xf] + uVar5 * 4) = cached[uVar5];
      *(int *)((int)Ainv[0x10] + uVar5 * 4) = cached[uVar5];
      iVar6 = uVar5 + 1;
      *(int *)((int)Ainv[0xf] + iVar6 * 4) = cached[iVar6];
      *(int *)((int)Ainv[0x10] + iVar6 * 4) = cached[iVar6];
      iVar6 = uVar5 + 2;
      *(int *)((int)Ainv[0xf] + iVar6 * 4) = cached[iVar6];
      *(int *)((int)Ainv[0x10] + iVar6 * 4) = cached[iVar6];
      iVar6 = uVar5 + 3;
      uVar5 = uVar5 + 4;
      *(int *)((int)Ainv[0xf] + iVar6 * 4) = cached[iVar6];
      uVar2 = cached[iVar6];
      *(uint *)((int)Ainv[0x10] + iVar6 * 4) = uVar2;
    } while (uVar5 != uVar1);
  }
LAB_0001095a:
  uVar1 = uVar2;
  (**(code **)(*(int *)Ainv[5] + 0x18))(Ainv[5],Ainv[0xf],Ainv[7],numClamped,AinvStride);
  MVar4 = 0.0;
  (**(code **)(*(int *)Ainv[6] + 0x18))
            (Ainv[6],Ainv[0x10],Ainv[8],numClamped,AinvStride,in_stack_00000018,uVar2,uVar1);
  if (Ainv[1] != 0.0) {
    do {
      clamped[(int)MVar4] = *(int *)((int)Ainv[8] + (int)MVar4 * 4);
      MVar4 = (MeReal)((int)MVar4 + 1);
    } while (MVar4 != Ainv[1]);
  }
  iVar6 = 0;
  if (AinvStride != 0) {
    uVar2 = AinvStride & 3;
    if (uVar2 != 0) {
      if (1 < uVar2) {
        if (2 < uVar2) {
          iVar6 = in_stack_00000018 * *(int *)numClamped * 4;
          checkNorm((MeReal *)(iVar6 + (int)Ainv[0x10]),(MeReal *)((int)Ainv[0xf] + iVar6),
                    in_stack_00000018,"solveunits");
        }
        uVar2 = (uint)(2 < uVar2);
        iVar6 = uVar2 + 1;
        iVar3 = in_stack_00000018 * *(int *)(numClamped + uVar2 * 4) * 4;
        checkNorm((MeReal *)(iVar3 + (int)Ainv[0x10]),(MeReal *)((int)Ainv[0xf] + iVar3),
                  in_stack_00000018,"solveunits");
      }
      iVar3 = in_stack_00000018 * *(int *)(numClamped + iVar6 * 4) * 4;
      iVar6 = iVar6 + 1;
      checkNorm((MeReal *)(iVar3 + (int)Ainv[0x10]),(MeReal *)((int)Ainv[0xf] + iVar3),
                in_stack_00000018,"solveunits");
      if (iVar6 == AinvStride) goto LAB_00010b05;
    }
    do {
      iVar3 = in_stack_00000018 * *(int *)(numClamped + iVar6 * 4) * 4;
      checkNorm((MeReal *)(iVar3 + (int)Ainv[0x10]),(MeReal *)((int)Ainv[0xf] + iVar3),
                in_stack_00000018,"solveunits");
      iVar3 = in_stack_00000018 * *(int *)(numClamped + 4 + iVar6 * 4) * 4;
      checkNorm((MeReal *)(iVar3 + (int)Ainv[0x10]),(MeReal *)((int)Ainv[0xf] + iVar3),
                in_stack_00000018,"solveunits");
      iVar3 = in_stack_00000018 * *(int *)(numClamped + 8 + iVar6 * 4) * 4;
      checkNorm((MeReal *)(iVar3 + (int)Ainv[0x10]),(MeReal *)((int)Ainv[0xf] + iVar3),
                in_stack_00000018,"solveunits");
      iVar3 = in_stack_00000018 * *(int *)(numClamped + 0xc + iVar6 * 4) * 4;
      iVar6 = iVar6 + 4;
      checkNorm((MeReal *)(iVar3 + (int)Ainv[0x10]),(MeReal *)((int)Ainv[0xf] + iVar3),
                in_stack_00000018,"solveunits");
    } while (iVar6 != AinvStride);
  }
LAB_00010b05:
  uVar1 = 0;
  uVar2 = in_stack_00000018 * in_stack_00000018;
  if (uVar2 != 0) {
    uVar5 = uVar2 & 3;
    if (uVar5 != 0) {
      if (1 < uVar5) {
        if (2 < uVar5) {
          *cached = *(int *)Ainv[0x10];
        }
        uVar1 = (uint)(2 < uVar5);
        cached[uVar1] = *(int *)((int)Ainv[0x10] + uVar1 * 4);
        uVar1 = uVar1 + 1;
      }
      cached[uVar1] = *(int *)((int)Ainv[0x10] + uVar1 * 4);
      uVar1 = uVar1 + 1;
      if (uVar1 == uVar2) {
        return;
      }
    }
    do {
      cached[uVar1] = *(int *)((int)Ainv[0x10] + uVar1 * 4);
      cached[uVar1 + 1] = *(int *)((int)Ainv[0x10] + (uVar1 + 1) * 4);
      cached[uVar1 + 2] = *(int *)((int)Ainv[0x10] + (uVar1 + 2) * 4);
      iVar6 = uVar1 + 3;
      uVar1 = uVar1 + 4;
      cached[iVar6] = *(int *)((int)Ainv[0x10] + iVar6 * 4);
    } while (uVar1 != uVar2);
  }
  return;
}


/* ==== writebackMatrixChol ==== */

/* DWARF original prototype: void writebackMatrixChol(keaMatrix_tester * this) */

void keaMatrix_tester::writebackMatrixChol(void)

{
  int in_stack_00000004;
  
  (**(code **)(**(int **)(in_stack_00000004 + 0x14) + 0x1c))(*(int **)(in_stack_00000004 + 0x14));
  (**(code **)(**(int **)(in_stack_00000004 + 0x18) + 0x1c))(*(int **)(in_stack_00000004 + 0x18));
  return;
}


/* ==== prefetchMatrixChol ==== */

/* DWARF original prototype: void prefetchMatrixChol(keaMatrix_tester * this) */

void keaMatrix_tester::prefetchMatrixChol(void)

{
  int in_stack_00000004;
  
  (**(code **)(**(int **)(in_stack_00000004 + 0x14) + 0x20))(*(int **)(in_stack_00000004 + 0x14));
  (**(code **)(**(int **)(in_stack_00000004 + 0x18) + 0x20))(*(int **)(in_stack_00000004 + 0x18));
  return;
}


/* ==== putchar ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int putchar(int __c)

{
  int iVar1;
  
  iVar1 = _IO_putc(__c,_stdout);
  return iVar1;
}


