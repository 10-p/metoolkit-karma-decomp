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
  MeWarning(0,"%s bad norm %e\n",desc,(double)norm);
  return;
}


/* ==== allocate ==== */

/* DWARF original prototype: void allocate(keaMatrix_tester * this, int n) */

void __thiscall keaMatrix_tester::allocate(keaMatrix_tester *this,int n)

{
  int iVar1;
  int *piVar2;
  MeReal *pMVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  
                    /* Unresolved local var: int c16c12n@[DW_OP_reg3(EBX)] */
  *(int *)&this->field_0x4 = n;
  iVar1 = n;
  if ((n & 3U) != 0) {
    uVar4 = n;
    if (n < 0) {
      uVar4 = n + 3;
    }
    iVar1 = (uVar4 & 0xfffffffc) + 4;
  }
  *(int *)&this->field_0x8 = iVar1;
  (*(code *)**(undefined4 **)this->suspect)(this->suspect,n);
  (*(code *)**(undefined4 **)this->correct)(this->correct,n);
  uVar4 = n;
  if (n % 0xc != 0) {
    uVar4 = (n - n % 0xc) + 0xc;
  }
  if ((uVar4 & 0xf) == 0) {
    if (n % 0xc != 0) {
      n = (n - n % 0xc) + 0xc;
    }
  }
  else {
    iVar1 = n;
    if (n % 0xc != 0) {
      iVar1 = (n - n % 0xc) + 0xc;
    }
    if (n % 0xc == 0) {
      iVar5 = n % 0x10;
    }
    else {
      iVar5 = n - n % 0xc;
      uVar4 = iVar5 + 0xc;
      uVar6 = uVar4;
      if ((int)uVar4 < 0) {
        uVar6 = iVar5 + 0x1b;
      }
      iVar5 = uVar4 - (uVar6 & 0xfffffff0);
    }
    n = (iVar1 - iVar5) + 0x10;
  }
  uVar4 = n * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar1 = uVar4 + 0x40;
  }
  else {
    iVar1 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  piVar2 = (int *)keaPoolAlloc(iVar1,"cached");
  this->suspectCached = piVar2;
  uVar4 = n * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar1 = uVar4 + 0x40;
  }
  else {
    iVar1 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  piVar2 = (int *)keaPoolAlloc(iVar1,"cached");
  this->correctCached = piVar2;
  uVar4 = n * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar1 = uVar4 + 0x40;
  }
  else {
    iVar1 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar1,"Qrhs");
  this->suspectQrhs = pMVar3;
  uVar4 = n * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar1 = uVar4 + 0x40;
  }
  else {
    iVar1 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar1,"Qrhs");
  this->correctQrhs = pMVar3;
  uVar4 = n * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar1 = uVar4 + 0x40;
  }
  else {
    iVar1 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar1,"x");
  this->suspectX = pMVar3;
  uVar4 = n * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar1 = uVar4 + 0x40;
  }
  else {
    iVar1 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar1,"x");
  this->correctX = pMVar3;
  uVar4 = n * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar1 = uVar4 + 0x40;
  }
  else {
    iVar1 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar1,"w");
  this->suspectB = pMVar3;
  uVar4 = n * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar1 = uVar4 + 0x40;
  }
  else {
    iVar1 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar1,"w");
  this->correctB = pMVar3;
  uVar4 = n * n * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar1 = uVar4 + 0x40;
  }
  else {
    iVar1 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar1,"Ainv");
  this->suspectAinv = pMVar3;
  uVar4 = n * n * 4;
  if ((uVar4 & 0x3f) == 0) {
    iVar1 = uVar4 + 0x40;
  }
  else {
    iVar1 = (uVar4 - (uVar4 & 0x3f)) + 0x80;
  }
  iVar1 = keaPoolAlloc(iVar1,"Ainv");
  this->suspectCached = (int *)((int)this->suspectCached + 0x3fU & 0xffffffc0);
  this->correctCached = (int *)((int)this->correctCached + 0x3fU & 0xffffffc0);
  this->suspectQrhs = (MeReal *)((int)this->suspectQrhs + 0x3fU & 0xffffffc0);
  this->correctQrhs = (MeReal *)((int)this->correctQrhs + 0x3fU & 0xffffffc0);
  this->suspectX = (MeReal *)((int)this->suspectX + 0x3fU & 0xffffffc0);
  this->correctX = (MeReal *)((int)this->correctX + 0x3fU & 0xffffffc0);
  this->suspectB = (MeReal *)((int)this->suspectB + 0x3fU & 0xffffffc0);
  this->correctB = (MeReal *)((int)this->correctB + 0x3fU & 0xffffffc0);
  this->suspectAinv = (MeReal *)((int)this->suspectAinv + 0x3fU & 0xffffffc0);
  this->correctAinv = (MeReal *)(iVar1 + 0x3fU & 0xffffffc0);
  return;
}


/* ==== makeFromJMJT ==== */

/* DWARF original prototype: void makeFromJMJT(keaMatrix_tester * this, MeReal * JM, MeReal * Js,
   int * num_in_strip, int * block2body, MeReal * slipfactor, MeReal epsilon, MeReal hinv) */

void __thiscall
keaMatrix_tester::makeFromJMJT
          (keaMatrix_tester *this,MeReal *JM,MeReal *Js,int *num_in_strip,int *block2body,
          MeReal *slipfactor,MeReal epsilon,MeReal hinv)

{
  (**(code **)(*(int *)this->suspect + 4))
            (this->suspect,JM,Js,num_in_strip,block2body,slipfactor,epsilon,hinv);
  (**(code **)(*(int *)this->correct + 4))
            (this->correct,JM,Js,num_in_strip,block2body,slipfactor,epsilon,hinv);
  return;
}


/* ==== makeFromColMajorPSM ==== */

/* DWARF original prototype: void makeFromColMajorPSM(keaMatrix_tester * this, MeReal * Qrhs, MeReal
   * Ainv, MeReal * clampedValues, MeReal * initialSolve, int * unclamped, int * clamped, int
   numUnclamped, int numClamped, int n_padded) */

void __thiscall
keaMatrix_tester::makeFromColMajorPSM
          (keaMatrix_tester *this,MeReal *Qrhs,MeReal *Ainv,MeReal *clampedValues,
          MeReal *initialSolve,int *unclamped,int *clamped,int numUnclamped,int numClamped,
          int n_padded)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)] */
  (**(code **)(*(int *)this->suspect + 8))
            (this->suspect,this->suspectQrhs,Ainv,clampedValues,initialSolve,unclamped,clamped,
             numUnclamped,numClamped,n_padded);
  (**(code **)(*(int *)this->correct + 8))
            (this->correct,this->correctQrhs,Ainv,clampedValues,initialSolve,unclamped,clamped,
             numUnclamped,numClamped,n_padded);
  checkNorm(this->correctQrhs,this->suspectQrhs,numClamped,"make from col major PSM");
  iVar3 = 0;
  if (numClamped != 0) {
    uVar2 = numClamped & 3;
    if (uVar2 != 0) {
      if (1 < uVar2) {
        if (2 < uVar2) {
          *Qrhs = *this->correctQrhs;
        }
        uVar2 = (uint)(2 < uVar2);
        Qrhs[uVar2] = this->correctQrhs[uVar2];
        iVar3 = uVar2 + 1;
      }
      Qrhs[iVar3] = this->correctQrhs[iVar3];
      iVar3 = iVar3 + 1;
      if (iVar3 == numClamped) {
        return;
      }
    }
    do {
      Qrhs[iVar3] = this->correctQrhs[iVar3];
      Qrhs[iVar3 + 1] = this->correctQrhs[iVar3 + 1];
      Qrhs[iVar3 + 2] = this->correctQrhs[iVar3 + 2];
      iVar1 = iVar3 + 3;
      iVar3 = iVar3 + 4;
      Qrhs[iVar1] = this->correctQrhs[iVar1];
    } while (iVar3 != numClamped);
  }
  return;
}


/* ==== factorize ==== */

/* DWARF original prototype: void factorize(keaMatrix_tester * this) */

void __thiscall keaMatrix_tester::factorize(keaMatrix_tester *this)

{
  (**(code **)(*(int *)this->suspect + 0xc))(this->suspect);
  (**(code **)(*(int *)this->correct + 0xc))(this->correct);
  return;
}


/* ==== solve ==== */

/* DWARF original prototype: void solve(keaMatrix_tester * this, MeReal * x, MeReal * rhs) */

void __thiscall keaMatrix_tester::solve(keaMatrix_tester *this,MeReal *x,MeReal *rhs)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  (**(code **)(*(int *)this->suspect + 0x10))(this->suspect,this->suspectX,rhs);
  (**(code **)(*(int *)this->correct + 0x10))(this->correct,this->correctX,rhs);
  checkNorm(this->correctX,this->suspectX,*(int *)&this->field_0x8,"solve");
  iVar1 = 0;
  if (*(int *)&this->field_0x8 != 0) {
    do {
      x[iVar1] = this->correctX[iVar1];
      iVar1 = iVar1 + 1;
    } while (iVar1 != *(int *)&this->field_0x8);
  }
  return;
}


/* ==== multiply ==== */

/* DWARF original prototype: void multiply(keaMatrix_tester * this, MeReal * b, MeReal * x) */

void __thiscall keaMatrix_tester::multiply(keaMatrix_tester *this,MeReal *b,MeReal *x)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  (**(code **)(*(int *)this->suspect + 0x14))(this->suspect,this->suspectB,x);
  (**(code **)(*(int *)this->correct + 0x14))(this->correct,this->correctB,x);
  checkNorm(this->correctB,this->suspectB,*(int *)&this->field_0x8,"multiply");
  iVar1 = 0;
  if (*(int *)&this->field_0x8 != 0) {
    do {
      b[iVar1] = this->correctB[iVar1];
      iVar1 = iVar1 + 1;
    } while (iVar1 != *(int *)&this->field_0x8);
  }
  return;
}


/* ==== solveUnits ==== */

/* DWARF original prototype: void solveUnits(keaMatrix_tester * this, MeReal * Ainv, int * cached,
   int * clamped, int numClamped, int AinvStride) */

void __thiscall
keaMatrix_tester::solveUnits
          (keaMatrix_tester *this,MeReal *Ainv,int *cached,int *clamped,int numClamped,
          int AinvStride)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  MeReal MVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar6 = 0;
  if (*(int *)&this->field_0x4 != 0) {
    do {
      this->suspectCached[iVar6] = cached[iVar6];
      this->correctCached[iVar6] = cached[iVar6];
      iVar6 = iVar6 + 1;
    } while (iVar6 != *(int *)&this->field_0x4);
  }
  uVar7 = 0;
  uVar3 = AinvStride * AinvStride;
  MVar4 = 0.0;
  if (uVar3 != 0) {
    MVar4 = (MeReal)(uVar3 & 3);
    if (MVar4 != 0.0) {
      if (1 < (uint)MVar4) {
        if (2 < (uint)MVar4) {
          *this->suspectAinv = *Ainv;
          *this->correctAinv = *Ainv;
        }
        uVar7 = (uint)(2 < (uint)MVar4);
        this->suspectAinv[uVar7] = Ainv[uVar7];
        this->correctAinv[uVar7] = Ainv[uVar7];
        uVar7 = uVar7 + 1;
      }
      this->suspectAinv[uVar7] = Ainv[uVar7];
      this->correctAinv[uVar7] = Ainv[uVar7];
      uVar7 = uVar7 + 1;
      if (uVar7 == uVar3) goto LAB_0001095a;
    }
    do {
      this->suspectAinv[uVar7] = Ainv[uVar7];
      this->correctAinv[uVar7] = Ainv[uVar7];
      iVar6 = uVar7 + 1;
      this->suspectAinv[iVar6] = Ainv[iVar6];
      this->correctAinv[iVar6] = Ainv[iVar6];
      iVar6 = uVar7 + 2;
      this->suspectAinv[iVar6] = Ainv[iVar6];
      this->correctAinv[iVar6] = Ainv[iVar6];
      iVar6 = uVar7 + 3;
      uVar7 = uVar7 + 4;
      this->suspectAinv[iVar6] = Ainv[iVar6];
      MVar4 = Ainv[iVar6];
      this->correctAinv[iVar6] = MVar4;
    } while (uVar7 != uVar3);
  }
LAB_0001095a:
  (**(code **)(*(int *)this->suspect + 0x18))
            (this->suspect,this->suspectAinv,this->suspectCached,clamped,numClamped,AinvStride,MVar4
             ,MVar4);
  iVar6 = 0;
  (**(code **)(*(int *)this->correct + 0x18))
            (this->correct,this->correctAinv,this->correctCached,clamped,numClamped,AinvStride);
  if (*(int *)&this->field_0x4 != 0) {
    do {
      cached[iVar6] = this->correctCached[iVar6];
      iVar6 = iVar6 + 1;
    } while (iVar6 != *(int *)&this->field_0x4);
  }
  iVar6 = 0;
  if (numClamped != 0) {
    uVar3 = numClamped & 3;
    if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
          checkNorm(this->correctAinv + AinvStride * *clamped,
                    this->suspectAinv + AinvStride * *clamped,AinvStride,"solveunits");
        }
        uVar3 = (uint)(2 < uVar3);
        iVar6 = uVar3 + 1;
        checkNorm(this->correctAinv + AinvStride * clamped[uVar3],
                  this->suspectAinv + AinvStride * clamped[uVar3],AinvStride,"solveunits");
      }
      piVar1 = clamped + iVar6;
      iVar6 = iVar6 + 1;
      checkNorm(this->correctAinv + AinvStride * *piVar1,this->suspectAinv + AinvStride * *piVar1,
                AinvStride,"solveunits");
      if (iVar6 == numClamped) goto LAB_00010b05;
    }
    do {
      checkNorm(this->correctAinv + AinvStride * clamped[iVar6],
                this->suspectAinv + AinvStride * clamped[iVar6],AinvStride,"solveunits");
      checkNorm(this->correctAinv + AinvStride * clamped[iVar6 + 1],
                this->suspectAinv + AinvStride * clamped[iVar6 + 1],AinvStride,"solveunits");
      checkNorm(this->correctAinv + AinvStride * clamped[iVar6 + 2],
                this->suspectAinv + AinvStride * clamped[iVar6 + 2],AinvStride,"solveunits");
      iVar2 = iVar6 + 3;
      iVar6 = iVar6 + 4;
      checkNorm(this->correctAinv + AinvStride * clamped[iVar2],
                this->suspectAinv + AinvStride * clamped[iVar2],AinvStride,"solveunits");
    } while (iVar6 != numClamped);
  }
LAB_00010b05:
  uVar7 = 0;
  uVar3 = AinvStride * AinvStride;
  if (uVar3 != 0) {
    uVar5 = uVar3 & 3;
    if (uVar5 != 0) {
      if (1 < uVar5) {
        if (2 < uVar5) {
          *Ainv = *this->correctAinv;
        }
        uVar7 = (uint)(2 < uVar5);
        Ainv[uVar7] = this->correctAinv[uVar7];
        uVar7 = uVar7 + 1;
      }
      Ainv[uVar7] = this->correctAinv[uVar7];
      uVar7 = uVar7 + 1;
      if (uVar7 == uVar3) {
        return;
      }
    }
    do {
      Ainv[uVar7] = this->correctAinv[uVar7];
      Ainv[uVar7 + 1] = this->correctAinv[uVar7 + 1];
      Ainv[uVar7 + 2] = this->correctAinv[uVar7 + 2];
      iVar6 = uVar7 + 3;
      uVar7 = uVar7 + 4;
      Ainv[iVar6] = this->correctAinv[iVar6];
    } while (uVar7 != uVar3);
  }
  return;
}


/* ==== writebackMatrixChol ==== */

/* DWARF original prototype: void writebackMatrixChol(keaMatrix_tester * this) */

void __thiscall keaMatrix_tester::writebackMatrixChol(keaMatrix_tester *this)

{
  (**(code **)(*(int *)this->suspect + 0x1c))(this->suspect);
  (**(code **)(*(int *)this->correct + 0x1c))(this->correct);
  return;
}


/* ==== prefetchMatrixChol ==== */

/* DWARF original prototype: void prefetchMatrixChol(keaMatrix_tester * this) */

void __thiscall keaMatrix_tester::prefetchMatrixChol(keaMatrix_tester *this)

{
  (**(code **)(*(int *)this->suspect + 0x20))(this->suspect);
  (**(code **)(*(int *)this->correct + 0x20))(this->correct);
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


