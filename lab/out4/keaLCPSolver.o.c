/* ==== allocate ==== */

/* DWARF original prototype: void allocate(keaLCPSolver * this, int size) */

void keaLCPSolver::allocate(int size)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  uint in_stack_00000008;
  
  *(uint *)(size + 4) = in_stack_00000008;
  iVar5 = (int)(in_stack_00000008 + 3) >> 2;
  *(int *)(size + 8) = iVar5;
  *(int *)(size + 0xc) = iVar5 << 2;
  uVar2 = in_stack_00000008;
  if ((int)in_stack_00000008 % 0xc != 0) {
    uVar2 = (in_stack_00000008 - (int)in_stack_00000008 % 0xc) + 0xc;
  }
  if ((uVar2 & 0xf) == 0) {
    if ((int)in_stack_00000008 % 0xc != 0) {
      in_stack_00000008 = (in_stack_00000008 - (int)in_stack_00000008 % 0xc) + 0xc;
    }
  }
  else {
    uVar2 = in_stack_00000008;
    if ((int)in_stack_00000008 % 0xc != 0) {
      uVar2 = (in_stack_00000008 - (int)in_stack_00000008 % 0xc) + 0xc;
    }
    if ((int)in_stack_00000008 % 0xc == 0) {
      iVar5 = (int)in_stack_00000008 % 0x10;
    }
    else {
      iVar5 = in_stack_00000008 - (int)in_stack_00000008 % 0xc;
      uVar1 = iVar5 + 0xc;
      uVar4 = uVar1;
      if ((int)uVar1 < 0) {
        uVar4 = iVar5 + 0x1b;
      }
      iVar5 = uVar1 - (uVar4 & 0xfffffff0);
    }
    in_stack_00000008 = (uVar2 - iVar5) + 0x10;
  }
  *(uint *)(size + 0x14) = in_stack_00000008;
  *(uint *)size = in_stack_00000008;
  uVar2 = *(int *)(size + 0x14) * 4;
  if ((uVar2 & 0x3f) == 0) {
    iVar5 = uVar2 + 0x40;
  }
  else {
    iVar5 = (uVar2 - (uVar2 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar5,"cached");
  *(undefined4 *)(size + 0x20) = uVar3;
  uVar2 = *(int *)(size + 0x14) * 4;
  if ((uVar2 & 0x3f) == 0) {
    iVar5 = uVar2 + 0x40;
  }
  else {
    iVar5 = (uVar2 - (uVar2 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar5,"x");
  *(undefined4 *)(size + 0x2c) = uVar3;
  uVar2 = *(int *)(size + 0x14) * 4;
  if ((uVar2 & 0x3f) == 0) {
    iVar5 = uVar2 + 0x40;
  }
  else {
    iVar5 = (uVar2 - (uVar2 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar5,"w");
  *(undefined4 *)(size + 0x30) = uVar3;
  uVar2 = *(int *)(size + 0x14) * 4;
  if ((uVar2 & 0x3f) == 0) {
    iVar5 = uVar2 + 0x40;
  }
  else {
    iVar5 = (uVar2 - (uVar2 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar5,"upper");
  *(undefined4 *)(size + 0x34) = uVar3;
  uVar2 = *(int *)(size + 0x14) * 4;
  if ((uVar2 & 0x3f) == 0) {
    iVar5 = uVar2 + 0x40;
  }
  else {
    iVar5 = (uVar2 - (uVar2 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar5,"lower");
  *(undefined4 *)(size + 0x38) = uVar3;
  uVar2 = *(int *)(size + 0x14) * 4;
  if ((uVar2 & 0x3f) == 0) {
    iVar5 = uVar2 + 0x40;
  }
  else {
    iVar5 = (uVar2 - (uVar2 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar5,"initialsolve");
  *(undefined4 *)(size + 0x3c) = uVar3;
  uVar2 = *(int *)(size + 0x14) * 4;
  if ((uVar2 & 0x3f) == 0) {
    iVar5 = uVar2 + 0x40;
  }
  else {
    iVar5 = (uVar2 - (uVar2 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar5,"clampedvalues");
  *(undefined4 *)(size + 0x40) = uVar3;
  uVar2 = *(int *)(size + 0x14) * *(int *)(size + 0x14) * 4;
  if ((uVar2 & 0x3f) == 0) {
    iVar5 = uVar2 + 0x40;
  }
  else {
    iVar5 = (uVar2 - (uVar2 & 0x3f)) + 0x80;
  }
  uVar3 = keaPoolAlloc(iVar5,"Ainv");
  *(undefined4 *)(size + 0x44) = uVar3;
  uVar2 = *(int *)(size + 0x14) * 4;
  if ((uVar2 & 0x3f) == 0) {
    iVar5 = uVar2 + 0x40;
  }
  else {
    iVar5 = (uVar2 - (uVar2 & 0x3f)) + 0x80;
  }
  iVar5 = keaPoolAlloc(iVar5,"Qrhs");
  *(uint *)(size + 0x20) = *(int *)(size + 0x20) + 0x3fU & 0xffffffc0;
  *(uint *)(size + 0x2c) = *(int *)(size + 0x2c) + 0x3fU & 0xffffffc0;
  *(uint *)(size + 0x30) = *(int *)(size + 0x30) + 0x3fU & 0xffffffc0;
  *(uint *)(size + 0x34) = *(int *)(size + 0x34) + 0x3fU & 0xffffffc0;
  *(uint *)(size + 0x38) = *(int *)(size + 0x38) + 0x3fU & 0xffffffc0;
  *(uint *)(size + 0x3c) = *(int *)(size + 0x3c) + 0x3fU & 0xffffffc0;
  *(uint *)(size + 0x40) = *(int *)(size + 0x40) + 0x3fU & 0xffffffc0;
  *(uint *)(size + 0x44) = *(int *)(size + 0x44) + 0x3fU & 0xffffffc0;
  *(uint *)(size + 0x48) = iVar5 + 0x3fU & 0xffffffc0;
  return;
}


/* ==== makeXandW ==== */

/* DWARF original prototype: void makeXandW(keaLCPSolver * this, MeReal * b, int * unclamped, int
   numUnclamped, int * clamped, int numClamped) */

void keaLCPSolver::makeXandW(MeReal *b,int *unclamped,int numUnclamped,int *clamped,int numClamped)

{
  MeReal MVar1;
  MeReal *pMVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  MeReal *in_stack_00000018;
  int iVar6;
  keaMatrix_pcSparse_vanilla vanillaQMatrix;
  
  pMVar2 = (MeReal *)b[1];
  if (in_stack_00000018 != pMVar2) {
    if (in_stack_00000018 == (MeReal *)0x0) {
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)] */
      iVar4 = 0;
      if (0 < (int)pMVar2) {
        do {
          *(undefined4 *)((int)b[0xb] + iVar4 * 4) = *(undefined4 *)((int)b[0xf] + iVar4 * 4);
          iVar4 = iVar4 + 1;
          pMVar2 = (MeReal *)b[1];
        } while (iVar4 < (int)pMVar2);
      }
      iVar4 = 0;
      if ((int)pMVar2 < 1) {
        return;
      }
      do {
        *(undefined4 *)((int)b[0xc] + iVar4 * 4) = 0;
        iVar4 = iVar4 + 1;
      } while (iVar4 < (int)b[1]);
      return;
    }
                    /* Unresolved local var: int nrhs@[DW_OP_reg3(EBX)] */
    uVar5 = 0;
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
    iVar4 = 0;
    if ((int)in_stack_00000018 < 1) goto LAB_0001043c;
    MVar1 = b[8];
    uVar3 = (uint)in_stack_00000018 & 3;
    if ((int)in_stack_00000018 < 2) {
LAB_000103f4:
      if (*(int *)((int)MVar1 + *(int *)(numClamped + iVar4 * 4) * 4) == 0) {
        uVar5 = uVar5 + 1;
      }
      iVar4 = iVar4 + 1;
      if ((int)in_stack_00000018 <= iVar4) goto LAB_0001043c;
    }
    else if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
                    /* Unresolved local var: int j@[???] */
          uVar5 = (uint)(*(int *)((int)MVar1 + *(int *)numClamped * 4) == 0);
        }
        uVar3 = (uint)(2 < uVar3);
        if (*(int *)((int)MVar1 + *(int *)(numClamped + uVar3 * 4) * 4) == 0) {
          uVar5 = uVar5 + 1;
        }
        iVar4 = uVar3 + 1;
      }
      goto LAB_000103f4;
    }
    do {
      if (*(int *)((int)MVar1 + *(int *)(numClamped + iVar4 * 4) * 4) == 0) {
        uVar5 = uVar5 + 1;
      }
      if (*(int *)((int)MVar1 + *(int *)(numClamped + 4 + iVar4 * 4) * 4) == 0) {
        uVar5 = uVar5 + 1;
      }
      if (*(int *)((int)MVar1 + *(int *)(numClamped + 8 + iVar4 * 4) * 4) == 0) {
        uVar5 = uVar5 + 1;
      }
      if (*(int *)((int)MVar1 + *(int *)(numClamped + 0xc + iVar4 * 4) * 4) == 0) {
        uVar5 = uVar5 + 1;
      }
      iVar4 = iVar4 + 4;
    } while (iVar4 < (int)in_stack_00000018);
LAB_0001043c:
    iVar4 = (int)in_stack_00000018 + uVar5;
    if ((int)clamped < iVar4) {
      PrincipalSubmatrix((int *)b,numUnclamped,clamped,numClamped,in_stack_00000018);
      return;
    }
                    /* Unresolved local var: keaMatrix * Q@[???] */
    _vanillaQMatrix = (undefined4 *)&__gxx_personality_v0;
    iVar6 = iVar4;
    (**(code **)(*(int *)b[9] + 0x18))(b[9],b[0x11],b[8],numClamped);
    (*(code *)*_vanillaQMatrix)(&vanillaQMatrix,in_stack_00000018,iVar4,iVar6);
    (*(code *)_vanillaQMatrix[2])
              (&vanillaQMatrix,b[0x12],b[0x11],b[0x10],b[0xf],numUnclamped,numClamped,clamped,
               in_stack_00000018,*b);
    (*(code *)_vanillaQMatrix[3])(&vanillaQMatrix);
    (*(code *)_vanillaQMatrix[4])(&vanillaQMatrix,b[0x12],b[0x12]);
    (**(code **)(*(int *)b[9] + 0x20))((int *)b[9]);
    PrincipalPivotTransformMakeW
              (b,(MeReal *)b[0xc],(int *)b[0x12],(int *)numClamped,numUnclamped,
               (int)in_stack_00000018);
    PrincipalPivotTransformMakeX
              (b,(MeReal *)b[0xb],(MeReal *)b[0xf],(MeReal *)b[0x11],(int *)b[0x12],
               (int *)numClamped,numUnclamped,(int)in_stack_00000018,(int)clamped,(int)b[1],
               (int)b[3]);
    return;
  }
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar4 = 0;
  (**(code **)(*(int *)b[9] + 0x14))(b[9],b[0xc],b[0x10]);
  if ((int)in_stack_00000018 < 1) goto LAB_0001060b;
  uVar5 = (uint)in_stack_00000018 & 3;
  if ((int)in_stack_00000018 < 2) {
LAB_000105af:
    iVar6 = *(int *)(numClamped + iVar4 * 4);
    iVar4 = iVar4 + 1;
    *(undefined4 *)((int)b[0xb] + iVar6 * 4) = *(undefined4 *)((int)b[0x10] + iVar6 * 4);
    if ((int)in_stack_00000018 <= iVar4) goto LAB_0001060b;
  }
  else if (uVar5 != 0) {
    if (1 < uVar5) {
      if (2 < uVar5) {
        *(undefined4 *)((int)b[0xb] + *(int *)numClamped * 4) =
             *(undefined4 *)((int)b[0x10] + *(int *)numClamped * 4);
      }
      iVar6 = *(int *)(numClamped + (uint)(2 < uVar5) * 4);
      iVar4 = (2 < uVar5) + 1;
      *(undefined4 *)((int)b[0xb] + iVar6 * 4) = *(undefined4 *)((int)b[0x10] + iVar6 * 4);
    }
    goto LAB_000105af;
  }
  do {
    iVar6 = *(int *)(numClamped + iVar4 * 4);
    *(undefined4 *)((int)b[0xb] + iVar6 * 4) = *(undefined4 *)((int)b[0x10] + iVar6 * 4);
    iVar6 = *(int *)(numClamped + 4 + iVar4 * 4);
    *(undefined4 *)((int)b[0xb] + iVar6 * 4) = *(undefined4 *)((int)b[0x10] + iVar6 * 4);
    iVar6 = *(int *)(numClamped + 8 + iVar4 * 4);
    *(undefined4 *)((int)b[0xb] + iVar6 * 4) = *(undefined4 *)((int)b[0x10] + iVar6 * 4);
    iVar6 = *(int *)(numClamped + 0xc + iVar4 * 4);
    iVar4 = iVar4 + 4;
    *(undefined4 *)((int)b[0xb] + iVar6 * 4) = *(undefined4 *)((int)b[0x10] + iVar6 * 4);
  } while (iVar4 < (int)in_stack_00000018);
LAB_0001060b:
  iVar4 = 0;
  if (0 < (int)b[1]) {
    do {
      *(float *)((int)b[0xc] + iVar4 * 4) =
           *(float *)((int)b[0xc] + iVar4 * 4) - (float)unclamped[iVar4];
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)b[1]);
  }
  return;
}


/* ==== PrincipalSubmatrix ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: void PrincipalSubmatrix(keaLCPSolver * this, int * unclamped, int
   numUnclamped, int * clamped, int numClamped, MeReal * b) */

void keaLCPSolver::PrincipalSubmatrix
               (int *unclamped,int numUnclamped,int *clamped,int numClamped,MeReal *b)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  keaMatrix_pcSparse_vanilla *pkVar4;
  undefined4 *puVar5;
  int iVar6;
  float *in_stack_00000018;
  keaMatrix_pcSparse_vanilla vanillaQMatrix;
  
                    /* Unresolved local var: keaMatrix * Q@[???]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  pkVar4 = &vanillaQMatrix;
  _vanillaQMatrix = &__gxx_personality_v0;
  (*___gxx_personality_v0)(pkVar4,clamped);
  iVar6 = unclamped[2];
  keaMatrix_pcSparse::makeFromPcSparsePSM
            ((float *)pkVar4,(keaMatrix_pcSparse *)unclamped[0x12],(float *)unclamped[9],
             in_stack_00000018,(int *)unclamped[0x10],(int *)numUnclamped,numClamped,(int)clamped,
             (int)b);
  (**(code **)(_vanillaQMatrix + 0xc))(pkVar4,iVar6);
  (**(code **)(_vanillaQMatrix + 0x10))(pkVar4,unclamped[0x12],unclamped[0x12]);
  puVar5 = (undefined4 *)(unclamped[1] * 4 + unclamped[0xb]);
  for (uVar3 = unclamped[3] - unclamped[1] & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
    *puVar5 = 0;
    puVar5 = puVar5 + 1;
  }
  iVar6 = 0;
  if (0 < (int)b) {
    uVar3 = (uint)b & 3;
    if ((int)b < 2) {
LAB_0001070e:
      iVar2 = *(int *)(numClamped + iVar6 * 4);
      pkVar4 = (keaMatrix_pcSparse_vanilla *)unclamped[0xb];
      iVar6 = iVar6 + 1;
      *(undefined4 *)(pkVar4 + iVar2 * 4) = *(undefined4 *)(unclamped[0x10] + iVar2 * 4);
      if ((int)b <= iVar6) goto LAB_00010779;
    }
    else if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
          *(undefined4 *)(unclamped[0xb] + *(int *)numClamped * 4) =
               *(undefined4 *)(unclamped[0x10] + *(int *)numClamped * 4);
        }
        iVar2 = *(int *)(numClamped + (uint)(2 < uVar3) * 4);
        iVar6 = (2 < uVar3) + 1;
        *(undefined4 *)(unclamped[0xb] + iVar2 * 4) = *(undefined4 *)(unclamped[0x10] + iVar2 * 4);
      }
      goto LAB_0001070e;
    }
    do {
      iVar2 = *(int *)(numClamped + iVar6 * 4);
      *(undefined4 *)(unclamped[0xb] + iVar2 * 4) = *(undefined4 *)(unclamped[0x10] + iVar2 * 4);
      iVar2 = *(int *)(numClamped + 4 + iVar6 * 4);
      *(undefined4 *)(unclamped[0xb] + iVar2 * 4) = *(undefined4 *)(unclamped[0x10] + iVar2 * 4);
      iVar2 = *(int *)(numClamped + 8 + iVar6 * 4);
      *(undefined4 *)(unclamped[0xb] + iVar2 * 4) = *(undefined4 *)(unclamped[0x10] + iVar2 * 4);
      iVar2 = *(int *)(numClamped + 0xc + iVar6 * 4);
      pkVar4 = (keaMatrix_pcSparse_vanilla *)unclamped[0xb];
      iVar6 = iVar6 + 4;
      *(undefined4 *)(pkVar4 + iVar2 * 4) = *(undefined4 *)(unclamped[0x10] + iVar2 * 4);
    } while (iVar6 < (int)b);
  }
LAB_00010779:
  iVar6 = 0;
  if ((int)clamped < 1) goto LAB_00010809;
  uVar3 = (uint)clamped & 3;
  if ((int)clamped < 2) {
LAB_00010794:
    pkVar4 = (keaMatrix_pcSparse_vanilla *)unclamped[0xb];
    iVar2 = iVar6 * 4;
    iVar1 = iVar6 * 4;
    iVar6 = iVar6 + 1;
    *(undefined4 *)(pkVar4 + *(int *)(numUnclamped + iVar2) * 4) =
         *(undefined4 *)(unclamped[0x12] + iVar1);
    if ((int)clamped <= iVar6) goto LAB_00010809;
  }
  else if (uVar3 != 0) {
    if (1 < uVar3) {
      if (2 < uVar3) {
        *(undefined4 *)(unclamped[0xb] + *(int *)numUnclamped * 4) = *(undefined4 *)unclamped[0x12];
      }
      uVar3 = (uint)(2 < uVar3);
      iVar6 = uVar3 + 1;
      *(undefined4 *)(unclamped[0xb] + *(int *)(numUnclamped + uVar3 * 4) * 4) =
           *(undefined4 *)(unclamped[0x12] + uVar3 * 4);
    }
    goto LAB_00010794;
  }
  do {
    *(undefined4 *)(unclamped[0xb] + *(int *)(numUnclamped + iVar6 * 4) * 4) =
         *(undefined4 *)(unclamped[0x12] + iVar6 * 4);
    *(undefined4 *)(unclamped[0xb] + *(int *)(numUnclamped + (iVar6 + 1) * 4) * 4) =
         *(undefined4 *)(unclamped[0x12] + (iVar6 + 1) * 4);
    iVar2 = iVar6 + 3;
    *(undefined4 *)(unclamped[0xb] + *(int *)(numUnclamped + (iVar6 + 2) * 4) * 4) =
         *(undefined4 *)(unclamped[0x12] + (iVar6 + 2) * 4);
    iVar6 = iVar6 + 4;
    pkVar4 = *(keaMatrix_pcSparse_vanilla **)(numUnclamped + iVar2 * 4);
    *(undefined4 *)(unclamped[0xb] + (int)pkVar4 * 4) = *(undefined4 *)(unclamped[0x12] + iVar2 * 4)
    ;
  } while (iVar6 < (int)clamped);
LAB_00010809:
  iVar6 = 0;
  (**(code **)(*(int *)unclamped[9] + 0x14))(unclamped[9],unclamped[0xc],unclamped[0xb],pkVar4);
  if (0 < unclamped[1]) {
    do {
      *(float *)(unclamped[0xc] + iVar6 * 4) =
           *(float *)(unclamped[0xc] + iVar6 * 4) - in_stack_00000018[iVar6];
      iVar6 = iVar6 + 1;
    } while (iVar6 < unclamped[1]);
  }
  return;
}


/* ==== PrincipalPivotTransformMakeW ==== */

/* DWARF original prototype: void PrincipalPivotTransformMakeW(keaLCPSolver * this, MeReal * w,
   MeReal * Qrhs, int * clamped, int * unclamped, int numClamped, int numUnclamped) */

void keaLCPSolver::PrincipalPivotTransformMakeW
               (MeReal *w,MeReal *Qrhs,int *clamped,int *unclamped,int numClamped,int numUnclamped)

{
  int iVar1;
  int *piVar2;
  MeReal *pMVar3;
  uint uVar4;
  int iVar5;
  uint in_stack_0000001c;
  
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)] */
  iVar5 = 0;
  if (0 < numUnclamped) {
    uVar4 = numUnclamped & 3;
    if (numUnclamped < 2) {
LAB_000108f6:
      piVar2 = unclamped + iVar5;
      pMVar3 = (MeReal *)(clamped + iVar5);
      iVar5 = iVar5 + 1;
      Qrhs[*piVar2] = *pMVar3;
      if (numUnclamped <= iVar5) goto LAB_0001093a;
    }
    else if (uVar4 != 0) {
      if (1 < uVar4) {
        if (2 < uVar4) {
          Qrhs[*unclamped] = (MeReal)*clamped;
        }
        uVar4 = (uint)(2 < uVar4);
        iVar5 = uVar4 + 1;
        Qrhs[unclamped[uVar4]] = (MeReal)clamped[uVar4];
      }
      goto LAB_000108f6;
    }
    do {
      Qrhs[unclamped[iVar5]] = (MeReal)clamped[iVar5];
      Qrhs[unclamped[iVar5 + 1]] = (MeReal)clamped[iVar5 + 1];
      Qrhs[unclamped[iVar5 + 2]] = (MeReal)clamped[iVar5 + 2];
      iVar1 = iVar5 + 3;
      iVar5 = iVar5 + 4;
      Qrhs[unclamped[iVar1]] = (MeReal)clamped[iVar1];
    } while (iVar5 < numUnclamped);
  }
LAB_0001093a:
  iVar5 = 0;
  if ((int)in_stack_0000001c < 1) {
    return;
  }
  uVar4 = in_stack_0000001c & 3;
  if (1 < (int)in_stack_0000001c) {
    if (uVar4 == 0) goto LAB_00010960;
    if (1 < uVar4) {
      if (2 < uVar4) {
        Qrhs[*(int *)numClamped] = 0.0;
      }
      iVar5 = (2 < uVar4) + 1;
      Qrhs[*(int *)(numClamped + (uint)(2 < uVar4) * 4)] = 0.0;
    }
  }
  iVar1 = iVar5 * 4;
  iVar5 = iVar5 + 1;
  Qrhs[*(int *)(numClamped + iVar1)] = 0.0;
  if ((int)in_stack_0000001c <= iVar5) {
    return;
  }
LAB_00010960:
  do {
    Qrhs[*(int *)(numClamped + iVar5 * 4)] = 0.0;
    Qrhs[*(int *)(numClamped + 4 + iVar5 * 4)] = 0.0;
    Qrhs[*(int *)(numClamped + 8 + iVar5 * 4)] = 0.0;
    iVar1 = iVar5 * 4;
    iVar5 = iVar5 + 4;
    Qrhs[*(int *)(numClamped + 0xc + iVar1)] = 0.0;
  } while (iVar5 < (int)in_stack_0000001c);
  return;
}


/* ==== PrincipalPivotTransformMakeX ==== */

/* DWARF original prototype: void PrincipalPivotTransformMakeX(keaLCPSolver * this, MeReal * x,
   MeReal * initialSolve, MeReal * Ainv, MeReal * Qrhs, int * clamped, int * unclamped, int
   numClamped, int numUnclamped, int n, int n_padded, int AinvStride) */

void keaLCPSolver::PrincipalPivotTransformMakeX
               (MeReal *x,MeReal *initialSolve,MeReal *Ainv,MeReal *Qrhs,int *clamped,int *unclamped
               ,int numClamped,int numUnclamped,int n,int n_padded,int AinvStride)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  MeReal *pMVar5;
  int in_stack_00000030;
  int i;
  
                    /* Unresolved local var: int j@[DW_OP_reg2(EDX)]
                       Unresolved local var: int k@[???] */
  pMVar5 = initialSolve + n_padded;
  for (uVar3 = AinvStride - n_padded & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pMVar5 = 0.0;
    pMVar5 = pMVar5 + 1;
  }
  i = 0;
  if (0 < n) {
    uVar3 = n & 3;
    if (n < 2) {
LAB_00010a3d:
      iVar2 = *(int *)(numClamped + i * 4);
      initialSolve[iVar2] = Ainv[iVar2];
      i = i + 1;
      if (n <= i) goto LAB_00010aa9;
    }
    else if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
          initialSolve[*(int *)numClamped] = Ainv[*(int *)numClamped];
        }
        i = (int)(2 < uVar3);
        iVar2 = *(int *)(numClamped + i * 4);
        initialSolve[iVar2] = Ainv[iVar2];
        i = i + 1;
      }
      goto LAB_00010a3d;
    }
    do {
      iVar2 = *(int *)(numClamped + i * 4);
      initialSolve[iVar2] = Ainv[iVar2];
      iVar2 = *(int *)(numClamped + 4 + i * 4);
      initialSolve[iVar2] = Ainv[iVar2];
      iVar2 = *(int *)(numClamped + 8 + i * 4);
      initialSolve[iVar2] = Ainv[iVar2];
      iVar2 = *(int *)(numClamped + 0xc + i * 4);
      i = i + 4;
      initialSolve[iVar2] = Ainv[iVar2];
    } while (i < n);
  }
LAB_00010aa9:
  i = 0;
  if (numUnclamped < 1) {
    return;
  }
  do {
                    /* Unresolved local var: MeReal * p@[DW_OP_reg1(ECX)] */
    iVar2 = in_stack_00000030 * unclamped[i];
    iVar4 = 0;
    if (0 < n) {
      uVar3 = n & 3;
      if (n < 2) {
LAB_00010aec:
        iVar1 = *(int *)(numClamped + iVar4 * 4);
        iVar4 = iVar4 + 1;
        initialSolve[iVar1] = (float)*clamped * Qrhs[iVar2 + iVar1] + initialSolve[iVar1];
        if (n <= iVar4) goto LAB_00010b49;
      }
      else if (uVar3 != 0) {
        if (1 < uVar3) {
          if (2 < uVar3) {
            iVar4 = *(int *)numClamped;
            initialSolve[iVar4] = (float)*clamped * Qrhs[iVar2 + iVar4] + initialSolve[iVar4];
          }
          iVar1 = *(int *)(numClamped + (uint)(2 < uVar3) * 4);
          iVar4 = (2 < uVar3) + 1;
          initialSolve[iVar1] = (float)*clamped * Qrhs[iVar2 + iVar1] + initialSolve[iVar1];
        }
        goto LAB_00010aec;
      }
      do {
        iVar1 = *(int *)(numClamped + iVar4 * 4);
        initialSolve[iVar1] = (float)*clamped * Qrhs[iVar2 + iVar1] + initialSolve[iVar1];
        iVar1 = *(int *)(numClamped + 4 + iVar4 * 4);
        initialSolve[iVar1] = (float)*clamped * Qrhs[iVar2 + iVar1] + initialSolve[iVar1];
        iVar1 = *(int *)(numClamped + 8 + iVar4 * 4);
        initialSolve[iVar1] = (float)*clamped * Qrhs[iVar2 + iVar1] + initialSolve[iVar1];
        iVar1 = *(int *)(numClamped + 0xc + iVar4 * 4);
        iVar4 = iVar4 + 4;
        initialSolve[iVar1] = (float)*clamped * Qrhs[iVar2 + iVar1] + initialSolve[iVar1];
      } while (iVar4 < n);
    }
LAB_00010b49:
    i = i + 1;
    clamped = clamped + 1;
  } while (i < numUnclamped);
  i = 0;
  if (numUnclamped < 1) {
    return;
  }
  uVar3 = numUnclamped & 3;
  if (1 < numUnclamped) {
    if (uVar3 == 0) goto LAB_00010ba0;
    if (1 < uVar3) {
      if (2 < uVar3) {
        initialSolve[*unclamped] = *(MeReal *)((int)x[0x10] + *unclamped * 4);
      }
      i = (int)(2 < uVar3);
      initialSolve[unclamped[i]] = *(MeReal *)((int)x[0x10] + unclamped[i] * 4);
      i = i + 1;
    }
  }
  initialSolve[unclamped[i]] = *(MeReal *)((int)x[0x10] + unclamped[i] * 4);
  i = i + 1;
  if (numUnclamped <= i) {
    return;
  }
LAB_00010ba0:
  do {
    initialSolve[unclamped[i]] = *(MeReal *)((int)x[0x10] + unclamped[i] * 4);
    initialSolve[unclamped[i + 1]] = *(MeReal *)((int)x[0x10] + unclamped[i + 1] * 4);
    initialSolve[unclamped[i + 2]] = *(MeReal *)((int)x[0x10] + unclamped[i + 2] * 4);
    iVar2 = i + 3;
    i = i + 4;
    initialSolve[unclamped[iVar2]] = *(MeReal *)((int)x[0x10] + unclamped[iVar2] * 4);
  } while (i < numUnclamped);
  return;
}


/* ==== PrincipalPivotTransform ==== */

/* DWARF original prototype: void PrincipalPivotTransform(keaLCPSolver * this, int * unclamped, int
   numUnclamped, int * clamped, int numClamped) */

void keaLCPSolver::PrincipalPivotTransform
               (int *unclamped,int numUnclamped,int *clamped,int numClamped)

{
  int in_stack_00000014;
  int iVar1;
  keaMatrix_pcSparse_vanilla vanillaQMatrix;
  
                    /* Unresolved local var: keaMatrix * Q@[???] */
  _vanillaQMatrix = (undefined4 *)&__gxx_personality_v0;
  (**(code **)(*(int *)unclamped[9] + 0x18))(unclamped[9],unclamped[0x11],unclamped[8],numClamped);
  (*(code *)*_vanillaQMatrix)(&vanillaQMatrix);
  iVar1 = *unclamped;
  (*(code *)_vanillaQMatrix[2])
            (&vanillaQMatrix,unclamped[0x12],unclamped[0x11],unclamped[0x10],unclamped[0xf],
             numUnclamped,numClamped,clamped);
  (*(code *)_vanillaQMatrix[3])(&vanillaQMatrix,iVar1);
  (*(code *)_vanillaQMatrix[4])(&vanillaQMatrix,unclamped[0x12],unclamped[0x12]);
  (**(code **)(*(int *)unclamped[9] + 0x20))((int *)unclamped[9]);
  PrincipalPivotTransformMakeW
            ((MeReal *)unclamped,(MeReal *)unclamped[0xc],(int *)unclamped[0x12],(int *)numClamped,
             numUnclamped,in_stack_00000014);
  PrincipalPivotTransformMakeX
            ((MeReal *)unclamped,(MeReal *)unclamped[0xb],(MeReal *)unclamped[0xf],
             (MeReal *)unclamped[0x11],(int *)unclamped[0x12],(int *)numClamped,numUnclamped,
             in_stack_00000014,(int)clamped,unclamped[1],unclamped[3]);
  return;
}


/* ==== setUpper ==== */

/* DWARF original prototype: void setUpper(keaLCPSolver * this, MeReal * upper) */

void keaLCPSolver::setUpper(MeReal *upper)

{
  MeReal in_stack_00000008;
  
  upper[0xd] = in_stack_00000008;
  return;
}


/* ==== setLower ==== */

/* DWARF original prototype: void setLower(keaLCPSolver * this, MeReal * lower) */

void keaLCPSolver::setLower(MeReal *lower)

{
  MeReal in_stack_00000008;
  
  lower[0xe] = in_stack_00000008;
  return;
}


/* ==== getFirstBadIndex ==== */

/* DWARF original prototype: int getFirstBadIndex(keaLCPSolver * this) */

int keaLCPSolver::getFirstBadIndex(void)

{
  uint uVar1;
  float *pfVar2;
  float *pfVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int in_stack_00000004;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  iVar6 = 0;
  uVar1 = *(uint *)(in_stack_00000004 + 4);
  if (0 < (int)uVar1) {
    pfVar2 = *(float **)(in_stack_00000004 + 0x2c);
    uVar5 = uVar1 & 3;
    pfVar3 = *(float **)(in_stack_00000004 + 0x34);
    if (1 < (int)uVar1) {
      if (uVar5 == 0) goto LAB_00010e40;
      if (1 < uVar5) {
        if (2 < uVar5) {
          if (*pfVar3 < *pfVar2) {
            return 0;
          }
          if (*pfVar2 < **(float **)(in_stack_00000004 + 0x38)) {
            return 0;
          }
          iVar6 = 1;
        }
        if (pfVar3[iVar6] < pfVar2[iVar6]) {
          return iVar6;
        }
        if (pfVar2[iVar6] < *(float *)(*(int *)(in_stack_00000004 + 0x38) + iVar6 * 4)) {
          return iVar6;
        }
        iVar6 = iVar6 + 1;
      }
    }
    if ((pfVar2[iVar6] <= pfVar3[iVar6]) &&
       (*(float *)(*(int *)(in_stack_00000004 + 0x38) + iVar6 * 4) <= pfVar2[iVar6])) {
      for (iVar6 = iVar6 + 1; iVar6 < (int)uVar1; iVar6 = iVar6 + 4) {
LAB_00010e40:
        if (pfVar3[iVar6] < pfVar2[iVar6]) {
          return iVar6;
        }
        iVar4 = *(int *)(in_stack_00000004 + 0x38);
        if (pfVar2[iVar6] < *(float *)(iVar4 + iVar6 * 4)) {
          return iVar6;
        }
        iVar7 = iVar6 + 1;
        if (pfVar3[iVar7] < pfVar2[iVar7]) {
          return iVar7;
        }
        if (pfVar2[iVar7] < *(float *)(iVar4 + iVar7 * 4)) {
          return iVar7;
        }
        iVar7 = iVar6 + 2;
        if (pfVar3[iVar7] < pfVar2[iVar7]) {
          return iVar7;
        }
        if (pfVar2[iVar7] < *(float *)(iVar4 + iVar7 * 4)) {
          return iVar7;
        }
        iVar7 = iVar6 + 3;
        if (pfVar3[iVar7] < pfVar2[iVar7]) {
          return iVar7;
        }
        if (pfVar2[iVar7] < *(float *)(iVar4 + iVar7 * 4)) {
          return iVar7;
        }
      }
    }
  }
  return iVar6;
}


/* ==== copyXtoInitialSolve ==== */

/* DWARF original prototype: void copyXtoInitialSolve(keaLCPSolver * this) */

void keaLCPSolver::copyXtoInitialSolve(void)

{
  uint uVar1;
  int iVar2;
  int in_stack_00000004;
  
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)] */
  iVar2 = 0;
  while( true ) {
    uVar1 = *(uint *)(in_stack_00000004 + 4);
    if ((uVar1 & 3) != 0) {
      if ((int)uVar1 < 0) {
        uVar1 = uVar1 + 3;
      }
      uVar1 = (uVar1 & 0xfffffffc) + 4;
    }
    if ((int)uVar1 <= iVar2) break;
    *(undefined4 *)(*(int *)(in_stack_00000004 + 0x3c) + iVar2 * 4) =
         *(undefined4 *)(*(int *)(in_stack_00000004 + 0x2c) + iVar2 * 4);
    iVar2 = iVar2 + 1;
  }
  return;
}


/* ==== getClampIndices ==== */

/* DWARF original prototype: void getClampIndices(keaLCPSolver * this, int * I, int * C) */

void keaLCPSolver::getClampIndices(int *I,int *C)

{
  int iVar1;
  int in_stack_0000000c;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  iVar1 = 0;
  if (0 < I[1]) {
    do {
      if (*(float *)(I[0xe] + iVar1 * 4) <= *(float *)(I[0xb] + iVar1 * 4)) {
        if (*(float *)(I[0xb] + iVar1 * 4) <= *(float *)(I[0xd] + iVar1 * 4)) {
          C[iVar1] = 0;
          goto LAB_00010fb0;
        }
        C[iVar1] = -1;
        *(undefined4 *)(in_stack_0000000c + iVar1 * 4) = 0xffffffff;
      }
      else {
        C[iVar1] = -1;
LAB_00010fb0:
        *(undefined4 *)(in_stack_0000000c + iVar1 * 4) = 0;
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < I[1]);
  }
  return;
}


/* ==== setClampedValues ==== */

/* DWARF original prototype: void setClampedValues(keaLCPSolver * this, int * clamped, int *
   unclamped, int * csize, int * usize, int * I, int * C) */

void keaLCPSolver::setClampedValues(int *clamped,int *unclamped,int *csize,int *usize,int *I,int *C)

{
  int iVar1;
  int iVar2;
  int in_stack_0000001c;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  iVar2 = 0;
  if (0 < clamped[1]) {
    do {
      if (C[iVar2] == 0) {
        csize[*I] = iVar2;
        *I = *I + 1;
      }
      else {
        unclamped[*usize] = iVar2;
        *usize = *usize + 1;
        if (*(int *)(in_stack_0000001c + iVar2 * 4) == 0) {
          iVar1 = clamped[0xe];
        }
        else {
          iVar1 = clamped[0xd];
        }
        *(undefined4 *)(clamped[0x10] + iVar2 * 4) = *(undefined4 *)(iVar1 + iVar2 * 4);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < clamped[1]);
  }
  return;
}


/* ==== commonPivot ==== */

/* WARNING: Variable defined which should be unmapped: DMASK */
/* DWARF original prototype: int commonPivot(keaLCPSolver * this, int MASK, int * I, int * C, int *
   clampedhere, int * hilohere) */

int keaLCPSolver::commonPivot(int MASK,int *I,int *C,int *clampedhere,int *hilohere)

{
  float fVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int in_stack_00000018;
  int DMASK;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  iVar6 = 0;
  DMASK = -1;
  if (0 < *(int *)(MASK + 4)) {
    do {
                    /* Unresolved local var: int CM@[DW_OP_reg1(ECX)]
                       Unresolved local var: int count@[DW_OP_reg6(ESI)]
                       Unresolved local var: int cyclesize@[DW_OP_reg3(EBX)]
                       Unresolved local var: int kk@[DW_OP_reg2(EDX)] */
      iVar5 = 0;
      iVar4 = 0;
      uVar2 = hilohere[iVar6];
      for (piVar3 = I; piVar3 != (int *)0x0; piVar3 = (int *)((int)piVar3 >> 1)) {
        iVar4 = iVar4 + 1;
        iVar5 = iVar5 + (uVar2 & 1);
        uVar2 = (int)uVar2 >> 1;
      }
      if (iVar4 / 2 < iVar5) {
        C[iVar6] = -1;
        fVar1 = *(float *)(*(int *)(MASK + 0x2c) + iVar6 * 4);
        clampedhere[iVar6] =
             (ABS(fVar1 - *(float *)(*(int *)(MASK + 0x38) + iVar6 * 4)) <
             ABS(fVar1 - *(float *)(*(int *)(MASK + 0x34) + iVar6 * 4))) - 1;
      }
      else {
        C[iVar6] = 0;
      }
      DMASK = DMASK & (C[iVar6] ^ 0xffffffffU ^ hilohere[iVar6]);
      if ((DMASK & C[iVar6]) != 0) {
        uVar2 = *(uint *)(in_stack_00000018 + iVar6 * 4);
        DMASK = DMASK & (clampedhere[iVar6] & uVar2 | (clampedhere[iVar6] | uVar2) ^ 0xffffffff);
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < *(int *)(MASK + 4));
  }
  return DMASK;
}


/* ==== blockMurtyChooseNewIndices ==== */

/* DWARF original prototype: int blockMurtyChooseNewIndices(keaLCPSolver * this, int * I, int * C,
   int * clamped, int * unclamped, int num_clamped, int num_unclamped) */

int keaLCPSolver::blockMurtyChooseNewIndices
              (int *I,int *C,int *clamped,int *unclamped,int num_clamped,int num_unclamped)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint in_stack_0000001c;
  int indexes_switched;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[DW_OP_reg2(EDX)] */
  iVar5 = 0;
  indexes_switched = 0;
  if (0 < (int)in_stack_0000001c) {
    uVar2 = in_stack_0000001c & 3;
    if ((int)in_stack_0000001c < 2) {
LAB_0001118e:
      iVar4 = *(int *)(num_clamped + iVar5 * 4);
      iVar3 = I[0xb];
      if (*(float *)(iVar3 + iVar4 * 4) < *(float *)(I[0xe] + iVar4 * 4)) {
        C[iVar4] = -1;
        clamped[iVar4] = 0;
        indexes_switched = 1;
        iVar3 = I[0xb];
      }
      if (*(float *)(I[0xd] + iVar4 * 4) < *(float *)(iVar3 + iVar4 * 4)) {
        C[iVar4] = -1;
        clamped[iVar4] = -1;
        indexes_switched = 1;
      }
      iVar5 = iVar5 + 1;
      if ((int)in_stack_0000001c <= iVar5) goto LAB_00011373;
    }
    else if (uVar2 != 0) {
      if (1 < uVar2) {
        if (2 < uVar2) {
          iVar5 = *(int *)num_clamped;
          iVar4 = I[0xb];
          bVar1 = *(float *)(iVar4 + iVar5 * 4) < *(float *)(I[0xe] + iVar5 * 4);
          if (bVar1) {
            C[iVar5] = -1;
            clamped[iVar5] = 0;
            iVar4 = I[0xb];
          }
          indexes_switched = (int)bVar1;
          if (*(float *)(I[0xd] + iVar5 * 4) < *(float *)(iVar4 + iVar5 * 4)) {
            C[iVar5] = -1;
            clamped[iVar5] = -1;
            indexes_switched = 1;
          }
          iVar5 = 1;
        }
        iVar4 = *(int *)(num_clamped + iVar5 * 4);
        iVar3 = I[0xb];
        if (*(float *)(iVar3 + iVar4 * 4) < *(float *)(I[0xe] + iVar4 * 4)) {
          C[iVar4] = -1;
          clamped[iVar4] = 0;
          indexes_switched = 1;
          iVar3 = I[0xb];
        }
        if (*(float *)(I[0xd] + iVar4 * 4) < *(float *)(iVar3 + iVar4 * 4)) {
          C[iVar4] = -1;
          clamped[iVar4] = -1;
          indexes_switched = 1;
        }
        iVar5 = iVar5 + 1;
      }
      goto LAB_0001118e;
    }
    do {
      iVar4 = *(int *)(num_clamped + iVar5 * 4);
      iVar3 = I[0xb];
      if (*(float *)(iVar3 + iVar4 * 4) < *(float *)(I[0xe] + iVar4 * 4)) {
        C[iVar4] = -1;
        clamped[iVar4] = 0;
        indexes_switched = 1;
        iVar3 = I[0xb];
      }
      if (*(float *)(I[0xd] + iVar4 * 4) < *(float *)(iVar3 + iVar4 * 4)) {
        C[iVar4] = -1;
        clamped[iVar4] = -1;
        indexes_switched = 1;
      }
      iVar4 = *(int *)(num_clamped + 4 + iVar5 * 4);
      iVar3 = I[0xb];
      if (*(float *)(iVar3 + iVar4 * 4) < *(float *)(I[0xe] + iVar4 * 4)) {
        C[iVar4] = -1;
        clamped[iVar4] = 0;
        indexes_switched = 1;
        iVar3 = I[0xb];
      }
      if (*(float *)(I[0xd] + iVar4 * 4) < *(float *)(iVar3 + iVar4 * 4)) {
        C[iVar4] = -1;
        clamped[iVar4] = -1;
        indexes_switched = 1;
      }
      iVar4 = *(int *)(num_clamped + 8 + iVar5 * 4);
      iVar3 = I[0xb];
      if (*(float *)(iVar3 + iVar4 * 4) < *(float *)(I[0xe] + iVar4 * 4)) {
        C[iVar4] = -1;
        clamped[iVar4] = 0;
        indexes_switched = 1;
        iVar3 = I[0xb];
      }
      if (*(float *)(I[0xd] + iVar4 * 4) < *(float *)(iVar3 + iVar4 * 4)) {
        C[iVar4] = -1;
        clamped[iVar4] = -1;
        indexes_switched = 1;
      }
      iVar4 = *(int *)(num_clamped + 0xc + iVar5 * 4);
      iVar3 = I[0xb];
      if (*(float *)(iVar3 + iVar4 * 4) < *(float *)(I[0xe] + iVar4 * 4)) {
        C[iVar4] = -1;
        clamped[iVar4] = 0;
        indexes_switched = 1;
        iVar3 = I[0xb];
      }
      if (*(float *)(I[0xd] + iVar4 * 4) < *(float *)(iVar3 + iVar4 * 4)) {
        C[iVar4] = -1;
        clamped[iVar4] = -1;
        indexes_switched = 1;
      }
      iVar5 = iVar5 + 4;
    } while (iVar5 < (int)in_stack_0000001c);
  }
LAB_00011373:
  iVar5 = 0;
  if (num_unclamped < 1) {
    return indexes_switched;
  }
  uVar2 = num_unclamped & 3;
  if (1 < num_unclamped) {
    if (uVar2 == 0) goto LAB_000113d2;
    if (1 < uVar2) {
      if (2 < uVar2) {
        iVar5 = *unclamped;
        iVar4 = clamped[iVar5];
        if ((iVar4 == 0) && (*(float *)(I[0xc] + iVar5 * 4) < -(float)I[7])) {
          C[iVar5] = 0;
          indexes_switched = 1;
          iVar4 = clamped[iVar5];
        }
        if ((iVar4 == -1) && ((float)I[7] < *(float *)(I[0xc] + iVar5 * 4))) {
          C[iVar5] = 0;
          indexes_switched = 1;
        }
        iVar5 = 1;
      }
      iVar4 = unclamped[iVar5];
      iVar3 = clamped[iVar4];
      if ((iVar3 == 0) && (*(float *)(I[0xc] + iVar4 * 4) < -(float)I[7])) {
        C[iVar4] = 0;
        indexes_switched = 1;
        iVar3 = clamped[iVar4];
      }
      if ((iVar3 == -1) && ((float)I[7] < *(float *)(I[0xc] + iVar4 * 4))) {
        C[iVar4] = 0;
        indexes_switched = 1;
      }
      iVar5 = iVar5 + 1;
    }
  }
  iVar4 = unclamped[iVar5];
  iVar3 = clamped[iVar4];
  if ((iVar3 == 0) && (*(float *)(I[0xc] + iVar4 * 4) < -(float)I[7])) {
    C[iVar4] = 0;
    indexes_switched = 1;
    iVar3 = clamped[iVar4];
  }
  if ((iVar3 == -1) && ((float)I[7] < *(float *)(I[0xc] + iVar4 * 4))) {
    C[iVar4] = 0;
    indexes_switched = 1;
  }
  iVar5 = iVar5 + 1;
  if (num_unclamped <= iVar5) {
    return indexes_switched;
  }
LAB_000113d2:
  do {
    iVar4 = unclamped[iVar5];
    iVar3 = clamped[iVar4];
    if ((iVar3 == 0) && (*(float *)(I[0xc] + iVar4 * 4) < -(float)I[7])) {
      C[iVar4] = 0;
      indexes_switched = 1;
      iVar3 = clamped[iVar4];
    }
    if ((iVar3 == -1) && ((float)I[7] < *(float *)(I[0xc] + iVar4 * 4))) {
      C[iVar4] = 0;
      indexes_switched = 1;
    }
    iVar4 = unclamped[iVar5 + 1];
    iVar3 = clamped[iVar4];
    if ((iVar3 == 0) && (*(float *)(I[0xc] + iVar4 * 4) < -(float)I[7])) {
      C[iVar4] = 0;
      indexes_switched = 1;
      iVar3 = clamped[iVar4];
    }
    if ((iVar3 == -1) && ((float)I[7] < *(float *)(I[0xc] + iVar4 * 4))) {
      C[iVar4] = 0;
      indexes_switched = 1;
    }
    iVar4 = unclamped[iVar5 + 2];
    iVar3 = clamped[iVar4];
    if ((iVar3 == 0) && (*(float *)(I[0xc] + iVar4 * 4) < -(float)I[7])) {
      C[iVar4] = 0;
      indexes_switched = 1;
      iVar3 = clamped[iVar4];
    }
    if ((iVar3 == -1) && ((float)I[7] < *(float *)(I[0xc] + iVar4 * 4))) {
      C[iVar4] = 0;
      indexes_switched = 1;
    }
    iVar4 = unclamped[iVar5 + 3];
    iVar3 = clamped[iVar4];
    if ((iVar3 == 0) && (*(float *)(I[0xc] + iVar4 * 4) < -(float)I[7])) {
      C[iVar4] = 0;
      indexes_switched = 1;
      iVar3 = clamped[iVar4];
    }
    if ((iVar3 == -1) && ((float)I[7] < *(float *)(I[0xc] + iVar4 * 4))) {
      C[iVar4] = 0;
      indexes_switched = 1;
    }
    iVar5 = iVar5 + 4;
  } while (iVar5 < num_unclamped);
  return indexes_switched;
}


/* ==== singleMurtyChooseNewIndices ==== */

/* DWARF original prototype: int singleMurtyChooseNewIndices(keaLCPSolver * this, int * I, int * C,
   int * clamped, int * unclamped, int * iorder, int num_clamped, int num_unclamped) */

int keaLCPSolver::singleMurtyChooseNewIndices
              (int *I,int *C,int *clamped,int *unclamped,int *iorder,int num_clamped,
              int num_unclamped)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int indexes_switched;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  iVar3 = 0;
  uVar1 = I[4];
  if ((int)uVar1 < 1) {
    return 0;
  }
  if (((int)uVar1 < 2) || ((uVar1 & 1) != 0)) {
    iVar2 = *(int *)num_clamped;
    if (C[iVar2] == 0) {
      if (*(float *)(I[0xb] + iVar2 * 4) < *(float *)(I[0xe] + iVar2 * 4)) {
LAB_00011860:
        C[iVar2] = -1;
        C = clamped;
LAB_0001186d:
        C[iVar2] = 0;
        return 1;
      }
      if (*(float *)(I[0xd] + iVar2 * 4) < *(float *)(I[0xb] + iVar2 * 4)) {
LAB_000117b2:
        C[iVar2] = -1;
        clamped[iVar2] = -1;
        return 1;
      }
    }
    else if (((clamped[iVar2] == 0) && (*(float *)(I[0xc] + iVar2 * 4) < -(float)I[7])) ||
            ((clamped[iVar2] == -1 && ((float)I[7] < *(float *)(I[0xc] + iVar2 * 4)))))
    goto LAB_0001186d;
    iVar3 = 1;
    if ((int)uVar1 < 2) {
      return 0;
    }
  }
  do {
    iVar2 = *(int *)(num_clamped + iVar3 * 4);
    if (C[iVar2] == 0) {
      if (*(float *)(I[0xb] + iVar2 * 4) < *(float *)(I[0xe] + iVar2 * 4)) goto LAB_00011860;
      if (*(float *)(I[0xd] + iVar2 * 4) < *(float *)(I[0xb] + iVar2 * 4)) goto LAB_000117b2;
    }
    else if (((clamped[iVar2] == 0) && (*(float *)(I[0xc] + iVar2 * 4) < -(float)I[7])) ||
            ((clamped[iVar2] == -1 && ((float)I[7] < *(float *)(I[0xc] + iVar2 * 4)))))
    goto LAB_0001186d;
    iVar2 = *(int *)(num_clamped + 4 + iVar3 * 4);
    if (C[iVar2] == 0) {
      if (*(float *)(I[0xb] + iVar2 * 4) < *(float *)(I[0xe] + iVar2 * 4)) goto LAB_00011860;
      if (*(float *)(I[0xd] + iVar2 * 4) < *(float *)(I[0xb] + iVar2 * 4)) goto LAB_000117b2;
    }
    else if (((clamped[iVar2] == 0) && (*(float *)(I[0xc] + iVar2 * 4) < -(float)I[7])) ||
            ((clamped[iVar2] == -1 && ((float)I[7] < *(float *)(I[0xc] + iVar2 * 4)))))
    goto LAB_0001186d;
    iVar3 = iVar3 + 2;
    if ((int)uVar1 <= iVar3) {
      return 0;
    }
  } while( true );
}


