/* ==== allocate ==== */

/* DWARF original prototype: void allocate(keaLCPSolver * this, int size) */

void __thiscall keaLCPSolver::allocate(keaLCPSolver *this,int size)

{
  uint uVar1;
  int *piVar2;
  MeReal *pMVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  
  this->n = size;
  iVar6 = size + 3 >> 2;
  this->n_blocks = iVar6;
  this->n_padded = iVar6 << 2;
  uVar1 = size;
  if (size % 0xc != 0) {
    uVar1 = (size - size % 0xc) + 0xc;
  }
  if ((uVar1 & 0xf) == 0) {
    if (size % 0xc != 0) {
      size = (size - size % 0xc) + 0xc;
    }
  }
  else {
    iVar6 = size;
    if (size % 0xc != 0) {
      iVar6 = (size - size % 0xc) + 0xc;
    }
    if (size % 0xc == 0) {
      iVar4 = size % 0x10;
    }
    else {
      iVar4 = size - size % 0xc;
      uVar1 = iVar4 + 0xc;
      uVar5 = uVar1;
      if ((int)uVar1 < 0) {
        uVar5 = iVar4 + 0x1b;
      }
      iVar4 = uVar1 - (uVar5 & 0xfffffff0);
    }
    size = (iVar6 - iVar4) + 0x10;
  }
  this->c16c12n = size;
  this->AinvStride = size;
  uVar1 = this->c16c12n * 4;
  if ((uVar1 & 0x3f) == 0) {
    iVar6 = uVar1 + 0x40;
  }
  else {
    iVar6 = (uVar1 - (uVar1 & 0x3f)) + 0x80;
  }
  piVar2 = (int *)keaPoolAlloc(iVar6,"cached");
  this->cached = piVar2;
  uVar1 = this->c16c12n * 4;
  if ((uVar1 & 0x3f) == 0) {
    iVar6 = uVar1 + 0x40;
  }
  else {
    iVar6 = (uVar1 - (uVar1 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar6,"x");
  this->x = pMVar3;
  uVar1 = this->c16c12n * 4;
  if ((uVar1 & 0x3f) == 0) {
    iVar6 = uVar1 + 0x40;
  }
  else {
    iVar6 = (uVar1 - (uVar1 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar6,"w");
  this->w = pMVar3;
  uVar1 = this->c16c12n * 4;
  if ((uVar1 & 0x3f) == 0) {
    iVar6 = uVar1 + 0x40;
  }
  else {
    iVar6 = (uVar1 - (uVar1 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar6,"upper");
  this->upper = pMVar3;
  uVar1 = this->c16c12n * 4;
  if ((uVar1 & 0x3f) == 0) {
    iVar6 = uVar1 + 0x40;
  }
  else {
    iVar6 = (uVar1 - (uVar1 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar6,"lower");
  this->lower = pMVar3;
  uVar1 = this->c16c12n * 4;
  if ((uVar1 & 0x3f) == 0) {
    iVar6 = uVar1 + 0x40;
  }
  else {
    iVar6 = (uVar1 - (uVar1 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar6,"initialsolve");
  this->initialSolve = pMVar3;
  uVar1 = this->c16c12n * 4;
  if ((uVar1 & 0x3f) == 0) {
    iVar6 = uVar1 + 0x40;
  }
  else {
    iVar6 = (uVar1 - (uVar1 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar6,"clampedvalues");
  this->clampedValues = pMVar3;
  uVar1 = this->c16c12n * this->c16c12n * 4;
  if ((uVar1 & 0x3f) == 0) {
    iVar6 = uVar1 + 0x40;
  }
  else {
    iVar6 = (uVar1 - (uVar1 & 0x3f)) + 0x80;
  }
  pMVar3 = (MeReal *)keaPoolAlloc(iVar6,"Ainv");
  this->Ainv = pMVar3;
  uVar1 = this->c16c12n * 4;
  if ((uVar1 & 0x3f) == 0) {
    iVar6 = uVar1 + 0x40;
  }
  else {
    iVar6 = (uVar1 - (uVar1 & 0x3f)) + 0x80;
  }
  iVar6 = keaPoolAlloc(iVar6,"Qrhs");
  this->cached = (int *)((int)this->cached + 0x3fU & 0xffffffc0);
  this->x = (MeReal *)((int)this->x + 0x3fU & 0xffffffc0);
  this->w = (MeReal *)((int)this->w + 0x3fU & 0xffffffc0);
  this->upper = (MeReal *)((int)this->upper + 0x3fU & 0xffffffc0);
  this->lower = (MeReal *)((int)this->lower + 0x3fU & 0xffffffc0);
  this->initialSolve = (MeReal *)((int)this->initialSolve + 0x3fU & 0xffffffc0);
  this->clampedValues = (MeReal *)((int)this->clampedValues + 0x3fU & 0xffffffc0);
  this->Ainv = (MeReal *)((int)this->Ainv + 0x3fU & 0xffffffc0);
  this->Qrhs = (MeReal *)(iVar6 + 0x3fU & 0xffffffc0);
  return;
}


/* ==== makeXandW ==== */

/* DWARF original prototype: void makeXandW(keaLCPSolver * this, MeReal * b, int * unclamped, int
   numUnclamped, int * clamped, int numClamped) */

void __thiscall
keaLCPSolver::makeXandW
          (keaLCPSolver *this,MeReal *b,int *unclamped,int numUnclamped,int *clamped,int numClamped)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  keaMatrix_pcSparse_vanilla vanillaQMatrix;
  
  iVar4 = this->n;
  if (numClamped != iVar4) {
    if (numClamped == 0) {
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)] */
      iVar3 = 0;
      if (0 < iVar4) {
        do {
          this->x[iVar3] = this->initialSolve[iVar3];
          iVar3 = iVar3 + 1;
          iVar4 = this->n;
        } while (iVar3 < iVar4);
      }
      iVar3 = 0;
      if (iVar4 < 1) {
        return;
      }
      do {
        this->w[iVar3] = 0.0;
        iVar3 = iVar3 + 1;
      } while (iVar3 < this->n);
      return;
    }
                    /* Unresolved local var: int nrhs@[DW_OP_reg3(EBX)] */
    uVar5 = 0;
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
    iVar4 = 0;
    if (numClamped < 1) goto LAB_0001043c;
    piVar1 = this->cached;
    uVar2 = numClamped & 3;
    if (numClamped < 2) {
LAB_000103f4:
      if (piVar1[clamped[iVar4]] == 0) {
        uVar5 = uVar5 + 1;
      }
      iVar4 = iVar4 + 1;
      if (numClamped <= iVar4) goto LAB_0001043c;
    }
    else if (uVar2 != 0) {
      if (1 < uVar2) {
        if (2 < uVar2) {
                    /* Unresolved local var: int j@[???] */
          uVar5 = (uint)(piVar1[*clamped] == 0);
        }
        uVar2 = (uint)(2 < uVar2);
        if (piVar1[clamped[uVar2]] == 0) {
          uVar5 = uVar5 + 1;
        }
        iVar4 = uVar2 + 1;
      }
      goto LAB_000103f4;
    }
    do {
      if (piVar1[clamped[iVar4]] == 0) {
        uVar5 = uVar5 + 1;
      }
      if (piVar1[clamped[iVar4 + 1]] == 0) {
        uVar5 = uVar5 + 1;
      }
      if (piVar1[clamped[iVar4 + 2]] == 0) {
        uVar5 = uVar5 + 1;
      }
      if (piVar1[clamped[iVar4 + 3]] == 0) {
        uVar5 = uVar5 + 1;
      }
      iVar4 = iVar4 + 4;
    } while (iVar4 < numClamped);
LAB_0001043c:
    iVar4 = numClamped + uVar5;
    if (numUnclamped < iVar4) {
      PrincipalSubmatrix(this,unclamped,numUnclamped,clamped,numClamped,b);
      return;
    }
                    /* Unresolved local var: keaMatrix * Q@[???] */
    _vanillaQMatrix = (undefined4 *)&__gxx_personality_v0;
    (**(code **)(*(int *)this->A + 0x18))
              (this->A,this->Ainv,this->cached,clamped,numClamped,this->AinvStride,iVar4,iVar4);
    (*(code *)*_vanillaQMatrix)((keaLCPSolver *)&vanillaQMatrix,numClamped);
    (*(code *)_vanillaQMatrix[2])
              ((keaLCPSolver *)&vanillaQMatrix,this->Qrhs,this->Ainv,this->clampedValues,
               this->initialSolve,unclamped,clamped,numUnclamped,numClamped,this->AinvStride);
    (*(code *)_vanillaQMatrix[3])((keaLCPSolver *)&vanillaQMatrix);
    (*(code *)_vanillaQMatrix[4])((keaLCPSolver *)&vanillaQMatrix,this->Qrhs,this->Qrhs);
    (**(code **)(*(int *)this->A + 0x20))(this->A);
    PrincipalPivotTransformMakeW(this,this->w,this->Qrhs,clamped,unclamped,numClamped,numUnclamped);
    PrincipalPivotTransformMakeX
              (this,this->x,this->initialSolve,this->Ainv,this->Qrhs,clamped,unclamped,numClamped,
               numUnclamped,this->n,this->n_padded,this->AinvStride);
    return;
  }
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar4 = 0;
  (**(code **)(*(int *)this->A + 0x14))(this->A,this->w,this->clampedValues);
  if (numClamped < 1) goto LAB_0001060b;
  uVar5 = numClamped & 3;
  if (numClamped < 2) {
LAB_000105af:
    piVar1 = clamped + iVar4;
    iVar4 = iVar4 + 1;
    this->x[*piVar1] = this->clampedValues[*piVar1];
    if (numClamped <= iVar4) goto LAB_0001060b;
  }
  else if (uVar5 != 0) {
    if (1 < uVar5) {
      if (2 < uVar5) {
        this->x[*clamped] = this->clampedValues[*clamped];
      }
      iVar4 = (2 < uVar5) + 1;
      this->x[clamped[2 < uVar5]] = this->clampedValues[clamped[2 < uVar5]];
    }
    goto LAB_000105af;
  }
  do {
    this->x[clamped[iVar4]] = this->clampedValues[clamped[iVar4]];
    this->x[clamped[iVar4 + 1]] = this->clampedValues[clamped[iVar4 + 1]];
    this->x[clamped[iVar4 + 2]] = this->clampedValues[clamped[iVar4 + 2]];
    iVar3 = iVar4 + 3;
    iVar4 = iVar4 + 4;
    this->x[clamped[iVar3]] = this->clampedValues[clamped[iVar3]];
  } while (iVar4 < numClamped);
LAB_0001060b:
  iVar4 = 0;
  if (0 < this->n) {
    do {
      this->w[iVar4] = this->w[iVar4] - b[iVar4];
      iVar4 = iVar4 + 1;
    } while (iVar4 < this->n);
  }
  return;
}


/* ==== PrincipalSubmatrix ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: void PrincipalSubmatrix(keaLCPSolver * this, int * unclamped, int
   numUnclamped, int * clamped, int numClamped, MeReal * b) */

void __thiscall
keaLCPSolver::PrincipalSubmatrix
          (keaLCPSolver *this,int *unclamped,int numUnclamped,int *clamped,int numClamped,MeReal *b)

{
  int iVar1;
  int *piVar2;
  uint uVar3;
  keaMatrix_pcSparse_vanilla *pkVar4;
  MeReal *pMVar5;
  int iVar6;
  keaMatrix_pcSparse_vanilla vanillaQMatrix;
  
                    /* Unresolved local var: keaMatrix * Q@[???]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  pkVar4 = &vanillaQMatrix;
  _vanillaQMatrix = &__gxx_personality_v0;
  (*___gxx_personality_v0)(pkVar4,numUnclamped);
  keaMatrix_pcSparse::makeFromPcSparsePSM
            ((float *)pkVar4,(keaMatrix_pcSparse *)this->Qrhs,(float *)this->A,b,
             (int *)this->clampedValues,unclamped,(int)clamped,numUnclamped,numClamped);
  (**(code **)(_vanillaQMatrix + 0xc))(pkVar4);
  (**(code **)(_vanillaQMatrix + 0x10))(pkVar4,this->Qrhs,this->Qrhs);
  pMVar5 = this->x + this->n;
  for (uVar3 = this->n_padded - this->n & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pMVar5 = 0.0;
    pMVar5 = pMVar5 + 1;
  }
  iVar6 = 0;
  if (0 < numClamped) {
    uVar3 = numClamped & 3;
    if (numClamped < 2) {
LAB_0001070e:
      piVar2 = clamped + iVar6;
      pkVar4 = (keaMatrix_pcSparse_vanilla *)this->x;
      iVar6 = iVar6 + 1;
      *(MeReal *)(pkVar4 + *piVar2 * 4) = this->clampedValues[*piVar2];
      if (numClamped <= iVar6) goto LAB_00010779;
    }
    else if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
          this->x[*clamped] = this->clampedValues[*clamped];
        }
        iVar6 = (2 < uVar3) + 1;
        this->x[clamped[2 < uVar3]] = this->clampedValues[clamped[2 < uVar3]];
      }
      goto LAB_0001070e;
    }
    do {
      this->x[clamped[iVar6]] = this->clampedValues[clamped[iVar6]];
      this->x[clamped[iVar6 + 1]] = this->clampedValues[clamped[iVar6 + 1]];
      this->x[clamped[iVar6 + 2]] = this->clampedValues[clamped[iVar6 + 2]];
      iVar1 = iVar6 + 3;
      pkVar4 = (keaMatrix_pcSparse_vanilla *)this->x;
      iVar6 = iVar6 + 4;
      *(MeReal *)(pkVar4 + clamped[iVar1] * 4) = this->clampedValues[clamped[iVar1]];
    } while (iVar6 < numClamped);
  }
LAB_00010779:
  iVar6 = 0;
  if (numUnclamped < 1) goto LAB_00010809;
  uVar3 = numUnclamped & 3;
  if (numUnclamped < 2) {
LAB_00010794:
    pkVar4 = (keaMatrix_pcSparse_vanilla *)this->x;
    piVar2 = unclamped + iVar6;
    pMVar5 = this->Qrhs + iVar6;
    iVar6 = iVar6 + 1;
    *(MeReal *)(pkVar4 + *piVar2 * 4) = *pMVar5;
    if (numUnclamped <= iVar6) goto LAB_00010809;
  }
  else if (uVar3 != 0) {
    if (1 < uVar3) {
      if (2 < uVar3) {
        this->x[*unclamped] = *this->Qrhs;
      }
      uVar3 = (uint)(2 < uVar3);
      iVar6 = uVar3 + 1;
      this->x[unclamped[uVar3]] = this->Qrhs[uVar3];
    }
    goto LAB_00010794;
  }
  do {
    this->x[unclamped[iVar6]] = this->Qrhs[iVar6];
    this->x[unclamped[iVar6 + 1]] = this->Qrhs[iVar6 + 1];
    iVar1 = iVar6 + 3;
    this->x[unclamped[iVar6 + 2]] = this->Qrhs[iVar6 + 2];
    iVar6 = iVar6 + 4;
    pkVar4 = (keaMatrix_pcSparse_vanilla *)unclamped[iVar1];
    this->x[(int)pkVar4] = this->Qrhs[iVar1];
  } while (iVar6 < numUnclamped);
LAB_00010809:
  iVar6 = 0;
  (**(code **)(*(int *)this->A + 0x14))(this->A,this->w,this->x,pkVar4);
  if (0 < this->n) {
    do {
      this->w[iVar6] = this->w[iVar6] - b[iVar6];
      iVar6 = iVar6 + 1;
    } while (iVar6 < this->n);
  }
  return;
}


/* ==== PrincipalPivotTransformMakeW ==== */

/* DWARF original prototype: void PrincipalPivotTransformMakeW(keaLCPSolver * this, MeReal * w,
   MeReal * Qrhs, int * clamped, int * unclamped, int numClamped, int numUnclamped) */

void __thiscall
keaLCPSolver::PrincipalPivotTransformMakeW
          (keaLCPSolver *this,MeReal *w,MeReal *Qrhs,int *clamped,int *unclamped,int numClamped,
          int numUnclamped)

{
  int iVar1;
  int *piVar2;
  MeReal *pMVar3;
  uint uVar4;
  int iVar5;
  
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)] */
  iVar5 = 0;
  if (0 < numClamped) {
    uVar4 = numClamped & 3;
    if (numClamped < 2) {
LAB_000108f6:
      piVar2 = clamped + iVar5;
      pMVar3 = Qrhs + iVar5;
      iVar5 = iVar5 + 1;
      w[*piVar2] = *pMVar3;
      if (numClamped <= iVar5) goto LAB_0001093a;
    }
    else if (uVar4 != 0) {
      if (1 < uVar4) {
        if (2 < uVar4) {
          w[*clamped] = *Qrhs;
        }
        uVar4 = (uint)(2 < uVar4);
        iVar5 = uVar4 + 1;
        w[clamped[uVar4]] = Qrhs[uVar4];
      }
      goto LAB_000108f6;
    }
    do {
      w[clamped[iVar5]] = Qrhs[iVar5];
      w[clamped[iVar5 + 1]] = Qrhs[iVar5 + 1];
      w[clamped[iVar5 + 2]] = Qrhs[iVar5 + 2];
      iVar1 = iVar5 + 3;
      iVar5 = iVar5 + 4;
      w[clamped[iVar1]] = Qrhs[iVar1];
    } while (iVar5 < numClamped);
  }
LAB_0001093a:
  iVar5 = 0;
  if (numUnclamped < 1) {
    return;
  }
  uVar4 = numUnclamped & 3;
  if (1 < numUnclamped) {
    if (uVar4 == 0) goto LAB_00010960;
    if (1 < uVar4) {
      if (2 < uVar4) {
        w[*unclamped] = 0.0;
      }
      iVar5 = (2 < uVar4) + 1;
      w[unclamped[2 < uVar4]] = 0.0;
    }
  }
  piVar2 = unclamped + iVar5;
  iVar5 = iVar5 + 1;
  w[*piVar2] = 0.0;
  if (numUnclamped <= iVar5) {
    return;
  }
LAB_00010960:
  do {
    w[unclamped[iVar5]] = 0.0;
    w[unclamped[iVar5 + 1]] = 0.0;
    w[unclamped[iVar5 + 2]] = 0.0;
    iVar1 = iVar5 + 3;
    iVar5 = iVar5 + 4;
    w[unclamped[iVar1]] = 0.0;
  } while (iVar5 < numUnclamped);
  return;
}


/* ==== PrincipalPivotTransformMakeX ==== */

/* DWARF original prototype: void PrincipalPivotTransformMakeX(keaLCPSolver * this, MeReal * x,
   MeReal * initialSolve, MeReal * Ainv, MeReal * Qrhs, int * clamped, int * unclamped, int
   numClamped, int numUnclamped, int n, int n_padded, int AinvStride) */

void __thiscall
keaLCPSolver::PrincipalPivotTransformMakeX
          (keaLCPSolver *this,MeReal *x,MeReal *initialSolve,MeReal *Ainv,MeReal *Qrhs,int *clamped,
          int *unclamped,int numClamped,int numUnclamped,int n,int n_padded,int AinvStride)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  MeReal *pMVar5;
  int i;
  
                    /* Unresolved local var: int j@[DW_OP_reg2(EDX)]
                       Unresolved local var: int k@[???] */
  pMVar5 = x + n;
  for (uVar3 = n_padded - n & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pMVar5 = 0.0;
    pMVar5 = pMVar5 + 1;
  }
  i = 0;
  if (0 < numUnclamped) {
    uVar3 = numUnclamped & 3;
    if (numUnclamped < 2) {
LAB_00010a3d:
      x[unclamped[i]] = initialSolve[unclamped[i]];
      i = i + 1;
      if (numUnclamped <= i) goto LAB_00010aa9;
    }
    else if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
          x[*unclamped] = initialSolve[*unclamped];
        }
        i = (int)(2 < uVar3);
        x[unclamped[i]] = initialSolve[unclamped[i]];
        i = i + 1;
      }
      goto LAB_00010a3d;
    }
    do {
      x[unclamped[i]] = initialSolve[unclamped[i]];
      x[unclamped[i + 1]] = initialSolve[unclamped[i + 1]];
      x[unclamped[i + 2]] = initialSolve[unclamped[i + 2]];
      iVar2 = i + 3;
      i = i + 4;
      x[unclamped[iVar2]] = initialSolve[unclamped[iVar2]];
    } while (i < numUnclamped);
  }
LAB_00010aa9:
  i = 0;
  if (numClamped < 1) {
    return;
  }
  do {
                    /* Unresolved local var: MeReal * p@[DW_OP_reg1(ECX)] */
    iVar2 = AinvStride * clamped[i];
    iVar4 = 0;
    if (0 < numUnclamped) {
      uVar3 = numUnclamped & 3;
      if (numUnclamped < 2) {
LAB_00010aec:
        iVar1 = unclamped[iVar4];
        iVar4 = iVar4 + 1;
        x[iVar1] = *Qrhs * Ainv[iVar2 + iVar1] + x[iVar1];
        if (numUnclamped <= iVar4) goto LAB_00010b49;
      }
      else if (uVar3 != 0) {
        if (1 < uVar3) {
          if (2 < uVar3) {
            iVar4 = *unclamped;
            x[iVar4] = *Qrhs * Ainv[iVar2 + iVar4] + x[iVar4];
          }
          iVar1 = unclamped[2 < uVar3];
          iVar4 = (2 < uVar3) + 1;
          x[iVar1] = *Qrhs * Ainv[iVar2 + iVar1] + x[iVar1];
        }
        goto LAB_00010aec;
      }
      do {
        iVar1 = unclamped[iVar4];
        x[iVar1] = *Qrhs * Ainv[iVar2 + iVar1] + x[iVar1];
        iVar1 = unclamped[iVar4 + 1];
        x[iVar1] = *Qrhs * Ainv[iVar2 + iVar1] + x[iVar1];
        iVar1 = unclamped[iVar4 + 2];
        x[iVar1] = *Qrhs * Ainv[iVar2 + iVar1] + x[iVar1];
        iVar1 = unclamped[iVar4 + 3];
        iVar4 = iVar4 + 4;
        x[iVar1] = *Qrhs * Ainv[iVar2 + iVar1] + x[iVar1];
      } while (iVar4 < numUnclamped);
    }
LAB_00010b49:
    i = i + 1;
    Qrhs = Qrhs + 1;
  } while (i < numClamped);
  i = 0;
  if (numClamped < 1) {
    return;
  }
  uVar3 = numClamped & 3;
  if (1 < numClamped) {
    if (uVar3 == 0) goto LAB_00010ba0;
    if (1 < uVar3) {
      if (2 < uVar3) {
        x[*clamped] = this->clampedValues[*clamped];
      }
      i = (int)(2 < uVar3);
      x[clamped[i]] = this->clampedValues[clamped[i]];
      i = i + 1;
    }
  }
  x[clamped[i]] = this->clampedValues[clamped[i]];
  i = i + 1;
  if (numClamped <= i) {
    return;
  }
LAB_00010ba0:
  do {
    x[clamped[i]] = this->clampedValues[clamped[i]];
    x[clamped[i + 1]] = this->clampedValues[clamped[i + 1]];
    x[clamped[i + 2]] = this->clampedValues[clamped[i + 2]];
    iVar2 = i + 3;
    i = i + 4;
    x[clamped[iVar2]] = this->clampedValues[clamped[iVar2]];
  } while (i < numClamped);
  return;
}


/* ==== PrincipalPivotTransform ==== */

/* DWARF original prototype: void PrincipalPivotTransform(keaLCPSolver * this, int * unclamped, int
   numUnclamped, int * clamped, int numClamped) */

void __thiscall
keaLCPSolver::PrincipalPivotTransform
          (keaLCPSolver *this,int *unclamped,int numUnclamped,int *clamped,int numClamped)

{
  keaMatrix_pcSparse_vanilla vanillaQMatrix;
  
                    /* Unresolved local var: keaMatrix * Q@[???] */
  _vanillaQMatrix = (undefined4 *)&__gxx_personality_v0;
  (**(code **)(*(int *)this->A + 0x18))
            (this->A,this->Ainv,this->cached,clamped,numClamped,this->AinvStride);
  (*(code *)*_vanillaQMatrix)((keaLCPSolver *)&vanillaQMatrix,numClamped);
  (*(code *)_vanillaQMatrix[2])
            ((keaLCPSolver *)&vanillaQMatrix,this->Qrhs,this->Ainv,this->clampedValues,
             this->initialSolve,unclamped,clamped,numUnclamped,numClamped,this->AinvStride);
  (*(code *)_vanillaQMatrix[3])((keaLCPSolver *)&vanillaQMatrix);
  (*(code *)_vanillaQMatrix[4])((keaLCPSolver *)&vanillaQMatrix,this->Qrhs,this->Qrhs);
  (**(code **)(*(int *)this->A + 0x20))(this->A);
  PrincipalPivotTransformMakeW(this,this->w,this->Qrhs,clamped,unclamped,numClamped,numUnclamped);
  PrincipalPivotTransformMakeX
            (this,this->x,this->initialSolve,this->Ainv,this->Qrhs,clamped,unclamped,numClamped,
             numUnclamped,this->n,this->n_padded,this->AinvStride);
  return;
}


/* ==== setUpper ==== */

/* DWARF original prototype: void setUpper(keaLCPSolver * this, MeReal * upper) */

void __thiscall keaLCPSolver::setUpper(keaLCPSolver *this,MeReal *upper)

{
  this->upper = upper;
  return;
}


/* ==== setLower ==== */

/* DWARF original prototype: void setLower(keaLCPSolver * this, MeReal * lower) */

void __thiscall keaLCPSolver::setLower(keaLCPSolver *this,MeReal *lower)

{
  this->lower = lower;
  return;
}


/* ==== getFirstBadIndex ==== */

/* DWARF original prototype: int getFirstBadIndex(keaLCPSolver * this) */

int __thiscall keaLCPSolver::getFirstBadIndex(keaLCPSolver *this)

{
  uint uVar1;
  float *pfVar2;
  float *pfVar3;
  MeReal *pMVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  iVar6 = 0;
  uVar1 = this->n;
  if (0 < (int)uVar1) {
    pfVar2 = this->x;
    uVar5 = uVar1 & 3;
    pfVar3 = this->upper;
    if (1 < (int)uVar1) {
      if (uVar5 == 0) goto LAB_00010e40;
      if (1 < uVar5) {
        if (2 < uVar5) {
          if (*pfVar3 < *pfVar2) {
            return 0;
          }
          if (*pfVar2 < *this->lower) {
            return 0;
          }
          iVar6 = 1;
        }
        if (pfVar3[iVar6] < pfVar2[iVar6]) {
          return iVar6;
        }
        if (pfVar2[iVar6] < this->lower[iVar6]) {
          return iVar6;
        }
        iVar6 = iVar6 + 1;
      }
    }
    if ((pfVar2[iVar6] <= pfVar3[iVar6]) && (this->lower[iVar6] <= pfVar2[iVar6])) {
      for (iVar6 = iVar6 + 1; iVar6 < (int)uVar1; iVar6 = iVar6 + 4) {
LAB_00010e40:
        if (pfVar3[iVar6] < pfVar2[iVar6]) {
          return iVar6;
        }
        pMVar4 = this->lower;
        if (pfVar2[iVar6] < pMVar4[iVar6]) {
          return iVar6;
        }
        iVar7 = iVar6 + 1;
        if (pfVar3[iVar7] < pfVar2[iVar7]) {
          return iVar7;
        }
        if (pfVar2[iVar7] < pMVar4[iVar7]) {
          return iVar7;
        }
        iVar7 = iVar6 + 2;
        if (pfVar3[iVar7] < pfVar2[iVar7]) {
          return iVar7;
        }
        if (pfVar2[iVar7] < pMVar4[iVar7]) {
          return iVar7;
        }
        iVar7 = iVar6 + 3;
        if (pfVar3[iVar7] < pfVar2[iVar7]) {
          return iVar7;
        }
        if (pfVar2[iVar7] < pMVar4[iVar7]) {
          return iVar7;
        }
      }
    }
  }
  return iVar6;
}


/* ==== copyXtoInitialSolve ==== */

/* DWARF original prototype: void copyXtoInitialSolve(keaLCPSolver * this) */

void __thiscall keaLCPSolver::copyXtoInitialSolve(keaLCPSolver *this)

{
  uint uVar1;
  int iVar2;
  
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)] */
  iVar2 = 0;
  while( true ) {
    uVar1 = this->n;
    if ((uVar1 & 3) != 0) {
      if ((int)uVar1 < 0) {
        uVar1 = uVar1 + 3;
      }
      uVar1 = (uVar1 & 0xfffffffc) + 4;
    }
    if ((int)uVar1 <= iVar2) break;
    this->initialSolve[iVar2] = this->x[iVar2];
    iVar2 = iVar2 + 1;
  }
  return;
}


/* ==== getClampIndices ==== */

/* DWARF original prototype: void getClampIndices(keaLCPSolver * this, int * I, int * C) */

void __thiscall keaLCPSolver::getClampIndices(keaLCPSolver *this,int *I,int *C)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  iVar1 = 0;
  if (0 < this->n) {
    do {
      if (this->lower[iVar1] <= this->x[iVar1]) {
        if (this->x[iVar1] <= this->upper[iVar1]) {
          I[iVar1] = 0;
          goto LAB_00010fb0;
        }
        I[iVar1] = -1;
        C[iVar1] = -1;
      }
      else {
        I[iVar1] = -1;
LAB_00010fb0:
        C[iVar1] = 0;
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < this->n);
  }
  return;
}


/* ==== setClampedValues ==== */

/* DWARF original prototype: void setClampedValues(keaLCPSolver * this, int * clamped, int *
   unclamped, int * csize, int * usize, int * I, int * C) */

void __thiscall
keaLCPSolver::setClampedValues
          (keaLCPSolver *this,int *clamped,int *unclamped,int *csize,int *usize,int *I,int *C)

{
  MeReal *pMVar1;
  int iVar2;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  iVar2 = 0;
  if (0 < this->n) {
    do {
      if (I[iVar2] == 0) {
        unclamped[*usize] = iVar2;
        *usize = *usize + 1;
      }
      else {
        clamped[*csize] = iVar2;
        *csize = *csize + 1;
        if (C[iVar2] == 0) {
          pMVar1 = this->lower;
        }
        else {
          pMVar1 = this->upper;
        }
        this->clampedValues[iVar2] = pMVar1[iVar2];
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < this->n);
  }
  return;
}


/* ==== commonPivot ==== */

/* WARNING: Variable defined which should be unmapped: DMASK */
/* DWARF original prototype: int commonPivot(keaLCPSolver * this, int MASK, int * I, int * C, int *
   clampedhere, int * hilohere) */

int __thiscall
keaLCPSolver::commonPivot(keaLCPSolver *this,int MASK,int *I,int *C,int *clampedhere,int *hilohere)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int DMASK;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  iVar5 = 0;
  DMASK = -1;
  if (0 < this->n) {
    do {
                    /* Unresolved local var: int CM@[DW_OP_reg1(ECX)]
                       Unresolved local var: int count@[DW_OP_reg6(ESI)]
                       Unresolved local var: int cyclesize@[DW_OP_reg3(EBX)]
                       Unresolved local var: int kk@[DW_OP_reg2(EDX)] */
      iVar4 = 0;
      iVar3 = 0;
      uVar1 = clampedhere[iVar5];
      for (iVar2 = MASK; iVar2 != 0; iVar2 = iVar2 >> 1) {
        iVar3 = iVar3 + 1;
        iVar4 = iVar4 + (uVar1 & 1);
        uVar1 = (int)uVar1 >> 1;
      }
      if (iVar3 / 2 < iVar4) {
        I[iVar5] = -1;
        C[iVar5] = (ABS(this->x[iVar5] - this->lower[iVar5]) <
                   ABS(this->x[iVar5] - this->upper[iVar5])) - 1;
      }
      else {
        I[iVar5] = 0;
      }
      DMASK = DMASK & (I[iVar5] ^ 0xffffffffU ^ clampedhere[iVar5]);
      if ((DMASK & I[iVar5]) != 0) {
        DMASK = DMASK & (C[iVar5] & hilohere[iVar5] | (C[iVar5] | hilohere[iVar5]) ^ 0xffffffffU);
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < this->n);
  }
  return DMASK;
}


/* ==== blockMurtyChooseNewIndices ==== */

/* DWARF original prototype: int blockMurtyChooseNewIndices(keaLCPSolver * this, int * I, int * C,
   int * clamped, int * unclamped, int num_clamped, int num_unclamped) */

int __thiscall
keaLCPSolver::blockMurtyChooseNewIndices
          (keaLCPSolver *this,int *I,int *C,int *clamped,int *unclamped,int num_clamped,
          int num_unclamped)

{
  bool bVar1;
  uint uVar2;
  MeReal *pMVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int indexes_switched;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[DW_OP_reg2(EDX)] */
  iVar6 = 0;
  indexes_switched = 0;
  if (0 < num_unclamped) {
    uVar2 = num_unclamped & 3;
    if (num_unclamped < 2) {
LAB_0001118e:
      iVar5 = unclamped[iVar6];
      pMVar3 = this->x;
      if (pMVar3[iVar5] < this->lower[iVar5]) {
        I[iVar5] = -1;
        C[iVar5] = 0;
        indexes_switched = 1;
        pMVar3 = this->x;
      }
      if (this->upper[iVar5] < pMVar3[iVar5]) {
        I[iVar5] = -1;
        C[iVar5] = -1;
        indexes_switched = 1;
      }
      iVar6 = iVar6 + 1;
      if (num_unclamped <= iVar6) goto LAB_00011373;
    }
    else if (uVar2 != 0) {
      if (1 < uVar2) {
        if (2 < uVar2) {
          iVar6 = *unclamped;
          pMVar3 = this->x;
          bVar1 = pMVar3[iVar6] < this->lower[iVar6];
          if (bVar1) {
            I[iVar6] = -1;
            C[iVar6] = 0;
            pMVar3 = this->x;
          }
          indexes_switched = (int)bVar1;
          if (this->upper[iVar6] < pMVar3[iVar6]) {
            I[iVar6] = -1;
            C[iVar6] = -1;
            indexes_switched = 1;
          }
          iVar6 = 1;
        }
        iVar5 = unclamped[iVar6];
        pMVar3 = this->x;
        if (pMVar3[iVar5] < this->lower[iVar5]) {
          I[iVar5] = -1;
          C[iVar5] = 0;
          indexes_switched = 1;
          pMVar3 = this->x;
        }
        if (this->upper[iVar5] < pMVar3[iVar5]) {
          I[iVar5] = -1;
          C[iVar5] = -1;
          indexes_switched = 1;
        }
        iVar6 = iVar6 + 1;
      }
      goto LAB_0001118e;
    }
    do {
      iVar5 = unclamped[iVar6];
      pMVar3 = this->x;
      if (pMVar3[iVar5] < this->lower[iVar5]) {
        I[iVar5] = -1;
        C[iVar5] = 0;
        indexes_switched = 1;
        pMVar3 = this->x;
      }
      if (this->upper[iVar5] < pMVar3[iVar5]) {
        I[iVar5] = -1;
        C[iVar5] = -1;
        indexes_switched = 1;
      }
      iVar5 = unclamped[iVar6 + 1];
      pMVar3 = this->x;
      if (pMVar3[iVar5] < this->lower[iVar5]) {
        I[iVar5] = -1;
        C[iVar5] = 0;
        indexes_switched = 1;
        pMVar3 = this->x;
      }
      if (this->upper[iVar5] < pMVar3[iVar5]) {
        I[iVar5] = -1;
        C[iVar5] = -1;
        indexes_switched = 1;
      }
      iVar5 = unclamped[iVar6 + 2];
      pMVar3 = this->x;
      if (pMVar3[iVar5] < this->lower[iVar5]) {
        I[iVar5] = -1;
        C[iVar5] = 0;
        indexes_switched = 1;
        pMVar3 = this->x;
      }
      if (this->upper[iVar5] < pMVar3[iVar5]) {
        I[iVar5] = -1;
        C[iVar5] = -1;
        indexes_switched = 1;
      }
      iVar5 = unclamped[iVar6 + 3];
      pMVar3 = this->x;
      if (pMVar3[iVar5] < this->lower[iVar5]) {
        I[iVar5] = -1;
        C[iVar5] = 0;
        indexes_switched = 1;
        pMVar3 = this->x;
      }
      if (this->upper[iVar5] < pMVar3[iVar5]) {
        I[iVar5] = -1;
        C[iVar5] = -1;
        indexes_switched = 1;
      }
      iVar6 = iVar6 + 4;
    } while (iVar6 < num_unclamped);
  }
LAB_00011373:
  iVar6 = 0;
  if (num_clamped < 1) {
    return indexes_switched;
  }
  uVar2 = num_clamped & 3;
  if (1 < num_clamped) {
    if (uVar2 == 0) goto LAB_000113d2;
    if (1 < uVar2) {
      if (2 < uVar2) {
        iVar6 = *clamped;
        iVar5 = C[iVar6];
        if ((iVar5 == 0) && (this->w[iVar6] < -this->velocityZeroTol)) {
          I[iVar6] = 0;
          indexes_switched = 1;
          iVar5 = C[iVar6];
        }
        if ((iVar5 == -1) && (this->velocityZeroTol < this->w[iVar6])) {
          I[iVar6] = 0;
          indexes_switched = 1;
        }
        iVar6 = 1;
      }
      iVar5 = clamped[iVar6];
      iVar4 = C[iVar5];
      if ((iVar4 == 0) && (this->w[iVar5] < -this->velocityZeroTol)) {
        I[iVar5] = 0;
        indexes_switched = 1;
        iVar4 = C[iVar5];
      }
      if ((iVar4 == -1) && (this->velocityZeroTol < this->w[iVar5])) {
        I[iVar5] = 0;
        indexes_switched = 1;
      }
      iVar6 = iVar6 + 1;
    }
  }
  iVar5 = clamped[iVar6];
  iVar4 = C[iVar5];
  if ((iVar4 == 0) && (this->w[iVar5] < -this->velocityZeroTol)) {
    I[iVar5] = 0;
    indexes_switched = 1;
    iVar4 = C[iVar5];
  }
  if ((iVar4 == -1) && (this->velocityZeroTol < this->w[iVar5])) {
    I[iVar5] = 0;
    indexes_switched = 1;
  }
  iVar6 = iVar6 + 1;
  if (num_clamped <= iVar6) {
    return indexes_switched;
  }
LAB_000113d2:
  do {
    iVar5 = clamped[iVar6];
    iVar4 = C[iVar5];
    if ((iVar4 == 0) && (this->w[iVar5] < -this->velocityZeroTol)) {
      I[iVar5] = 0;
      indexes_switched = 1;
      iVar4 = C[iVar5];
    }
    if ((iVar4 == -1) && (this->velocityZeroTol < this->w[iVar5])) {
      I[iVar5] = 0;
      indexes_switched = 1;
    }
    iVar5 = clamped[iVar6 + 1];
    iVar4 = C[iVar5];
    if ((iVar4 == 0) && (this->w[iVar5] < -this->velocityZeroTol)) {
      I[iVar5] = 0;
      indexes_switched = 1;
      iVar4 = C[iVar5];
    }
    if ((iVar4 == -1) && (this->velocityZeroTol < this->w[iVar5])) {
      I[iVar5] = 0;
      indexes_switched = 1;
    }
    iVar5 = clamped[iVar6 + 2];
    iVar4 = C[iVar5];
    if ((iVar4 == 0) && (this->w[iVar5] < -this->velocityZeroTol)) {
      I[iVar5] = 0;
      indexes_switched = 1;
      iVar4 = C[iVar5];
    }
    if ((iVar4 == -1) && (this->velocityZeroTol < this->w[iVar5])) {
      I[iVar5] = 0;
      indexes_switched = 1;
    }
    iVar5 = clamped[iVar6 + 3];
    iVar4 = C[iVar5];
    if ((iVar4 == 0) && (this->w[iVar5] < -this->velocityZeroTol)) {
      I[iVar5] = 0;
      indexes_switched = 1;
      iVar4 = C[iVar5];
    }
    if ((iVar4 == -1) && (this->velocityZeroTol < this->w[iVar5])) {
      I[iVar5] = 0;
      indexes_switched = 1;
    }
    iVar6 = iVar6 + 4;
  } while (iVar6 < num_clamped);
  return indexes_switched;
}


/* ==== singleMurtyChooseNewIndices ==== */

/* DWARF original prototype: int singleMurtyChooseNewIndices(keaLCPSolver * this, int * I, int * C,
   int * clamped, int * unclamped, int * iorder, int num_clamped, int num_unclamped) */

int __thiscall
keaLCPSolver::singleMurtyChooseNewIndices
          (keaLCPSolver *this,int *I,int *C,int *clamped,int *unclamped,int *iorder,int num_clamped,
          int num_unclamped)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int indexes_switched;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  iVar3 = 0;
  uVar1 = this->deadIndex;
  if ((int)uVar1 < 1) {
    return 0;
  }
  if (((int)uVar1 < 2) || ((uVar1 & 1) != 0)) {
    iVar2 = *iorder;
    if (I[iVar2] == 0) {
      if (this->x[iVar2] < this->lower[iVar2]) {
LAB_00011860:
        I[iVar2] = -1;
        I = C;
LAB_0001186d:
        I[iVar2] = 0;
        return 1;
      }
      if (this->upper[iVar2] < this->x[iVar2]) {
LAB_000117b2:
        I[iVar2] = -1;
        C[iVar2] = -1;
        return 1;
      }
    }
    else if (((C[iVar2] == 0) && (this->w[iVar2] < -this->velocityZeroTol)) ||
            ((C[iVar2] == -1 && (this->velocityZeroTol < this->w[iVar2])))) goto LAB_0001186d;
    iVar3 = 1;
    if ((int)uVar1 < 2) {
      return 0;
    }
  }
  do {
    iVar2 = iorder[iVar3];
    if (I[iVar2] == 0) {
      if (this->x[iVar2] < this->lower[iVar2]) goto LAB_00011860;
      if (this->upper[iVar2] < this->x[iVar2]) goto LAB_000117b2;
    }
    else if (((C[iVar2] == 0) && (this->w[iVar2] < -this->velocityZeroTol)) ||
            ((C[iVar2] == -1 && (this->velocityZeroTol < this->w[iVar2])))) goto LAB_0001186d;
    iVar2 = iorder[iVar3 + 1];
    if (I[iVar2] == 0) {
      if (this->x[iVar2] < this->lower[iVar2]) goto LAB_00011860;
      if (this->upper[iVar2] < this->x[iVar2]) goto LAB_000117b2;
    }
    else if (((C[iVar2] == 0) && (this->w[iVar2] < -this->velocityZeroTol)) ||
            ((C[iVar2] == -1 && (this->velocityZeroTol < this->w[iVar2])))) goto LAB_0001186d;
    iVar3 = iVar3 + 2;
    if ((int)uVar1 <= iVar3) {
      return 0;
    }
  } while( true );
}


