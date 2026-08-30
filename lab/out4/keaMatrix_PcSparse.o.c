/* ==== makeFromPcSparsePSM ==== */

/* DWARF original prototype: void makeFromPcSparsePSM(keaMatrix_pcSparse * this, MeReal * Qrhs,
   keaMatrix_pcSparse * A, MeReal * b, MeReal * clampedValues, int * unclamped, int * clamped, int
   numUnclamped, int numClamped, int n_blocks) */

void keaMatrix_pcSparse::makeFromPcSparsePSM
               (MeReal *Qrhs,keaMatrix_pcSparse *A,MeReal *b,MeReal *clampedValues,int *unclamped,
               int *clamped,int numUnclamped,int numClamped,int n_blocks)

{
  MeReal *pMVar1;
  MeReal MVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  MeReal *pMVar6;
  int *piVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  MeReal MVar12;
  int iVar13;
  int in_stack_00000028;
  int local_40;
  MeReal *local_3c;
  MeReal *ri;
  int mp;
  int mb;
  int k;
  int r;
  int i;
  
                    /* Unresolved local var: int j@[DW_OP_reg7(EDI)] */
  MVar12 = Qrhs[5];
  MVar2 = Qrhs[2];
  ri = (MeReal *)Qrhs[4];
  i = 0;
  if (0 < (int)MVar12) {
    local_40 = 0;
    do {
      iVar13 = 0;
      if (0 < (int)MVar12) {
        uVar3 = (uint)MVar12 & 3;
        pMVar6 = ri + local_40 * 0x10;
        if ((int)MVar12 < 2) {
LAB_0001006a:
          iVar9 = iVar13 + local_40;
          iVar13 = iVar13 + 1;
          *(MeReal **)((int)Qrhs[8] + iVar9 * 4) = pMVar6;
          pMVar6 = pMVar6 + 0x10;
          if ((int)MVar12 <= iVar13) goto LAB_000100d0;
        }
        else if (uVar3 != 0) {
          if (1 < uVar3) {
            if (2 < uVar3) {
              *(MeReal **)((int)Qrhs[8] + local_40 * 4) = pMVar6;
              pMVar6 = pMVar6 + 0x10;
            }
            iVar13 = (2 < uVar3) + 1;
            *(MeReal **)((int)Qrhs[8] + (local_40 + (uint)(2 < uVar3)) * 4) = pMVar6;
            pMVar6 = pMVar6 + 0x10;
          }
          goto LAB_0001006a;
        }
        do {
          *(MeReal **)((int)Qrhs[8] + (iVar13 + local_40) * 4) = pMVar6;
          *(MeReal **)((int)Qrhs[8] + (local_40 + 1 + iVar13) * 4) = pMVar6 + 0x10;
          *(MeReal **)((int)Qrhs[8] + (local_40 + 2 + iVar13) * 4) = pMVar6 + 0x20;
          iVar9 = local_40 + 3 + iVar13;
          pMVar1 = pMVar6 + 0x30;
          iVar13 = iVar13 + 4;
          pMVar6 = pMVar6 + 0x40;
          *(MeReal **)((int)Qrhs[8] + iVar9 * 4) = pMVar1;
        } while (iVar13 < (int)MVar12);
      }
LAB_000100d0:
      *(undefined4 *)((int)Qrhs[0xc] + i * 4) = 0;
      *(uint *)((int)Qrhs[0xb] + i * 4) = (int)MVar12 - 1;
      i = i + 1;
      local_40 = local_40 + (int)MVar12;
    } while (i < (int)MVar12);
  }
  i = 0;
  r = 0;
  if (0 < numClamped) {
    do {
      uVar3 = clamped[i];
      k = 0;
      if (0 < numClamped) {
        iVar13 = (int)uVar3 >> 2;
        iVar9 = iVar13 * in_stack_00000028;
        iVar11 = (uVar3 & 3) * 4;
        local_3c = ri + r;
        if ((numClamped < 2) || (iVar4 = r, (numClamped & 1U) != 0)) {
                    /* Unresolved local var: int m@[???]
                       Unresolved local var: MeReal * rj@[DW_OP_reg2(EDX)] */
          uVar10 = *clamped;
          if ((int)uVar3 < (int)uVar10) {
            iVar4 = ((int)uVar10 >> 2) * in_stack_00000028 + iVar13;
          }
          else {
            iVar4 = ((int)uVar10 >> 2) + iVar9;
          }
          iVar4 = *(int *)((int)b[7] + iVar4 * 4);
          if (iVar4 == 0) {
            *local_3c = 0.0;
          }
          else {
            if ((int)uVar3 < (int)uVar10) {
              iVar5 = iVar11 + (uVar10 & 3);
            }
            else {
              iVar5 = (uVar3 & 3) + (uVar10 & 3) * 4;
            }
            ri[r] = *(MeReal *)(iVar4 + iVar5 * 4);
          }
          k = 1;
          local_3c = local_3c + 4;
          iVar4 = r + 4;
          if (numClamped < 2) goto LAB_000102b7;
        }
        do {
          uVar10 = clamped[k];
          if ((int)uVar3 < (int)uVar10) {
            iVar5 = ((int)uVar10 >> 2) * in_stack_00000028 + iVar13;
          }
          else {
            iVar5 = ((int)uVar10 >> 2) + iVar9;
          }
          iVar5 = *(int *)((int)b[7] + iVar5 * 4);
          if (iVar5 == 0) {
            *local_3c = 0.0;
          }
          else {
            if ((int)uVar3 < (int)uVar10) {
              iVar8 = iVar11 + (uVar10 & 3);
            }
            else {
              iVar8 = (uVar3 & 3) + (uVar10 & 3) * 4;
            }
            ri[iVar4] = *(MeReal *)(iVar5 + iVar8 * 4);
          }
          uVar10 = clamped[k + 1];
          if ((int)uVar3 < (int)uVar10) {
            piVar7 = (int *)((int)b[7] + (((int)uVar10 >> 2) * in_stack_00000028 + iVar13) * 4);
          }
          else {
            piVar7 = (int *)((int)b[7] + (((int)uVar10 >> 2) + iVar9) * 4);
          }
          if (*piVar7 == 0) {
            local_3c[4] = 0.0;
          }
          else {
            if ((int)uVar3 < (int)uVar10) {
              iVar5 = iVar11 + (uVar10 & 3);
            }
            else {
              iVar5 = (uVar3 & 3) + (uVar10 & 3) * 4;
            }
            ri[iVar4 + 4] = *(MeReal *)(*piVar7 + iVar5 * 4);
          }
          k = k + 2;
          local_3c = local_3c + 8;
          iVar4 = iVar4 + 8;
        } while (k < numClamped);
      }
LAB_000102b7:
      MVar12 = (MeReal)k;
      if (k < (int)MVar2) {
        iVar13 = k * 4;
        k = (int)MVar2 - k;
        pMVar6 = ri + r + iVar13;
        uVar3 = -k & 3;
        MVar12 = MVar2;
        if (uVar3 != 0) {
          if (uVar3 < 3) {
            if (uVar3 < 2) {
              k = k + -1;
              *pMVar6 = 0.0;
              pMVar6 = pMVar6 + 4;
            }
            *pMVar6 = 0.0;
            pMVar6 = pMVar6 + 4;
            k = k + -1;
          }
          *pMVar6 = 0.0;
          pMVar6 = pMVar6 + 4;
          k = k + -1;
          if (k == 0) goto LAB_0001033a;
        }
        do {
          *pMVar6 = 0.0;
          pMVar6[4] = 0.0;
          pMVar6[8] = 0.0;
          pMVar6[0xc] = 0.0;
          pMVar6 = pMVar6 + 0x10;
          k = k + -4;
        } while (k != 0);
      }
LAB_0001033a:
      k = (int)MVar12;
      i = i + 1;
      r = r + 1U & 3;
      ri = ri + (r - 1U & (uint)MVar2) * 4;
    } while (i < numClamped);
  }
  if ((r != 0) && (r != 4)) {
    if ((r & 1U) != 0) {
      k = 0;
      MVar12 = (MeReal)k;
      if (0 < (int)MVar2) {
        pMVar6 = ri + r;
        uVar3 = -(int)MVar2 & 3;
        k = (int)MVar2;
        if (uVar3 == 0) goto LAB_000103e5;
        if (uVar3 < 3) {
          if (uVar3 < 2) {
            *pMVar6 = 0.0;
            pMVar6 = pMVar6 + 4;
            k = (int)MVar2 - 1;
          }
          *pMVar6 = 0.0;
          pMVar6 = pMVar6 + 4;
          k = k + -1;
        }
        *pMVar6 = 0.0;
        pMVar6 = pMVar6 + 4;
        for (k = k + -1; MVar12 = MVar2, k != 0; k = k + -4) {
LAB_000103e5:
          *pMVar6 = 0.0;
          pMVar6[4] = 0.0;
          pMVar6[8] = 0.0;
          pMVar6[0xc] = 0.0;
          pMVar6 = pMVar6 + 0x10;
        }
      }
      k = (int)MVar12;
      r = r + 1;
      if (r == 4) goto LAB_00010522;
    }
    do {
      if (0 < (int)MVar2) {
        pMVar6 = ri + r;
        uVar3 = -(int)MVar2 & 3;
        k = (int)MVar2;
        if (uVar3 != 0) {
          if (uVar3 < 3) {
            if (uVar3 < 2) {
              *pMVar6 = 0.0;
              pMVar6 = pMVar6 + 4;
              k = (int)MVar2 - 1;
            }
            *pMVar6 = 0.0;
            pMVar6 = pMVar6 + 4;
            k = k + -1;
          }
          *pMVar6 = 0.0;
          pMVar6 = pMVar6 + 4;
          k = k + -1;
          if (k == 0) goto LAB_00010494;
        }
        do {
          *pMVar6 = 0.0;
          pMVar6[4] = 0.0;
          pMVar6[8] = 0.0;
          pMVar6[0xc] = 0.0;
          pMVar6 = pMVar6 + 0x10;
          k = k + -4;
        } while (k != 0);
      }
LAB_00010494:
      k = 0;
      MVar12 = (MeReal)k;
      if (0 < (int)MVar2) {
        pMVar6 = ri + r + 1;
        uVar3 = -(int)MVar2 & 3;
        k = (int)MVar2;
        if (uVar3 == 0) goto LAB_000104e5;
        if (uVar3 < 3) {
          if (uVar3 < 2) {
            *pMVar6 = 0.0;
            pMVar6 = pMVar6 + 4;
            k = (int)MVar2 - 1;
          }
          *pMVar6 = 0.0;
          pMVar6 = pMVar6 + 4;
          k = k + -1;
        }
        *pMVar6 = 0.0;
        pMVar6 = pMVar6 + 4;
        for (k = k + -1; MVar12 = MVar2, k != 0; k = k + -4) {
LAB_000104e5:
          *pMVar6 = 0.0;
          pMVar6[4] = 0.0;
          pMVar6[8] = 0.0;
          pMVar6[0xc] = 0.0;
          pMVar6 = pMVar6 + 0x10;
        }
      }
      k = (int)MVar12;
      r = r + 2;
    } while (r != 4);
  }
LAB_00010522:
  iVar13 = k + -1;
  iVar9 = r + -1;
  uVar3 = numClamped - 1U & 3;
  i = uVar3 + 1;
  if ((uint)i < 4) {
    uVar10 = -i & 3;
    if (uVar3 + 2 < 4) {
      if (uVar10 != 0) {
        if (1 < uVar10) {
          if (2 < uVar10) {
            i = uVar3 + 2;
            ri[iVar9 + iVar13 * 4] = 1.0;
            iVar13 = k + -2;
            iVar9 = r + -2;
          }
          r = iVar9;
          k = iVar13;
          ri[r + k * 4] = 1.0;
          i = i + 1;
          iVar13 = k + -1;
          iVar9 = r + -1;
        }
        goto LAB_00010559;
      }
    }
    else {
LAB_00010559:
      r = iVar9;
      k = iVar13;
      ri[r + k * 4] = 1.0;
      i = i + 1;
      iVar13 = k + -1;
      iVar9 = r + -1;
      if (3 < (uint)i) goto LAB_000105d0;
    }
    do {
      r = iVar9;
      k = iVar13;
      ri[r + k * 4] = 1.0;
      ri[r + -5 + k * 4] = 1.0;
      ri[r + -10 + k * 4] = 1.0;
      ri[r + -0xf + k * 4] = 1.0;
      i = i + 4;
      iVar13 = k + -4;
      iVar9 = r + -4;
    } while (i < 4);
  }
LAB_000105d0:
  i = 0;
  if (0 < numClamped) {
    uVar3 = numClamped & 3;
    if (numClamped < 2) {
LAB_000105ef:
      *(MeReal *)(&A->super_keaMatrix + i * 4) = clampedValues[clamped[i]];
      i = i + 1;
      if (numClamped <= i) goto LAB_00010663;
    }
    else if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
          *(MeReal *)A = clampedValues[*clamped];
        }
        i = (int)(2 < uVar3);
        *(MeReal *)(&A->super_keaMatrix + i * 4) = clampedValues[clamped[i]];
        i = i + 1;
      }
      goto LAB_000105ef;
    }
    do {
      *(MeReal *)(&A->super_keaMatrix + i * 4) = clampedValues[clamped[i]];
      *(MeReal *)(&A->field_0x4 + i * 4) = clampedValues[clamped[i + 1]];
      *(MeReal *)(&A->field_0x8 + i * 4) = clampedValues[clamped[i + 2]];
      *(MeReal *)(&A->field_0xc + i * 4) = clampedValues[clamped[i + 3]];
      i = i + 4;
    } while (i < numClamped);
  }
LAB_00010663:
  if ((int)MVar2 <= i) goto LAB_000106d2;
  uVar3 = (int)MVar2 - i & 3;
  if (i + 1 < (int)MVar2) {
    if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
          *(undefined4 *)(&A->super_keaMatrix + i * 4) = 0;
          i = i + 1;
        }
        *(undefined4 *)(&A->super_keaMatrix + i * 4) = 0;
        i = i + 1;
      }
      goto LAB_00010682;
    }
  }
  else {
LAB_00010682:
    *(undefined4 *)(&A->super_keaMatrix + i * 4) = 0;
    i = i + 1;
    if ((int)MVar2 <= i) goto LAB_000106d2;
  }
  do {
    *(undefined4 *)(&A->super_keaMatrix + i * 4) = 0;
    *(undefined4 *)(&A->field_0x4 + i * 4) = 0;
    *(undefined4 *)(&A->field_0x8 + i * 4) = 0;
    *(undefined4 *)(&A->field_0xc + i * 4) = 0;
    i = i + 4;
  } while (i < (int)MVar2);
LAB_000106d2:
  i = 0;
  if (0 < n_blocks) {
    do {
      uVar3 = *(uint *)(numUnclamped + i * 4);
      if (((float)unclamped[uVar3] != 0.0) && (k = 0, 0 < numClamped)) {
        iVar13 = (int)uVar3 >> 2;
        iVar9 = iVar13 * in_stack_00000028;
        iVar11 = (uVar3 & 3) * 4;
        if ((numClamped < 2) || ((numClamped & 1U) != 0)) {
                    /* Unresolved local var: int m@[???]
                       Unresolved local var: MeReal * rj@[DW_OP_reg2(EDX)] */
          uVar10 = *clamped;
          if ((int)uVar3 < (int)uVar10) {
            iVar4 = ((int)uVar10 >> 2) * in_stack_00000028 + iVar13;
            MVar12 = b[7];
          }
          else {
            iVar4 = ((int)uVar10 >> 2) + iVar9;
            MVar12 = b[7];
          }
          iVar4 = *(int *)((int)MVar12 + iVar4 * 4);
          if (iVar4 != 0) {
            if ((int)uVar3 < (int)uVar10) {
              iVar5 = iVar11 + (uVar10 & 3);
            }
            else {
              iVar5 = (uVar3 & 3) + (uVar10 & 3) * 4;
            }
            *(float *)A = *(float *)A - (float)unclamped[uVar3] * *(float *)(iVar4 + iVar5 * 4);
          }
          k = 1;
          if (numClamped < 2) goto LAB_00010860;
        }
        do {
          uVar10 = clamped[k];
          if ((int)uVar3 < (int)uVar10) {
            iVar4 = ((int)uVar10 >> 2) * in_stack_00000028 + iVar13;
            MVar12 = b[7];
          }
          else {
            iVar4 = ((int)uVar10 >> 2) + iVar9;
            MVar12 = b[7];
          }
          iVar4 = *(int *)((int)MVar12 + iVar4 * 4);
          if (iVar4 != 0) {
            if ((int)uVar3 < (int)uVar10) {
              iVar5 = iVar11 + (uVar10 & 3);
            }
            else {
              iVar5 = (uVar3 & 3) + (uVar10 & 3) * 4;
            }
            *(float *)(&A->super_keaMatrix + k * 4) =
                 *(float *)(&A->super_keaMatrix + k * 4) -
                 (float)unclamped[uVar3] * *(float *)(iVar4 + iVar5 * 4);
          }
          uVar10 = clamped[k + 1];
          if ((int)uVar3 < (int)uVar10) {
            piVar7 = (int *)((int)b[7] + (((int)uVar10 >> 2) * in_stack_00000028 + iVar13) * 4);
          }
          else {
            piVar7 = (int *)((int)b[7] + (((int)uVar10 >> 2) + iVar9) * 4);
          }
          if (*piVar7 != 0) {
            if ((int)uVar3 < (int)uVar10) {
              iVar4 = iVar11 + (uVar10 & 3);
            }
            else {
              iVar4 = (uVar3 & 3) + (uVar10 & 3) * 4;
            }
            *(float *)(&A->field_0x4 + k * 4) =
                 *(float *)(&A->field_0x4 + k * 4) -
                 (float)unclamped[uVar3] * *(float *)(*piVar7 + iVar4 * 4);
          }
          k = k + 2;
        } while (k < numClamped);
      }
LAB_00010860:
      i = i + 1;
    } while (i < n_blocks);
  }
  return;
}


/* ==== makeFromColMajorPSM ==== */

/* DWARF original prototype: void makeFromColMajorPSM(keaMatrix_pcSparse * this, MeReal * Qrhs,
   MeReal * Ainv, MeReal * clampedValues, MeReal * initialSolve, int * unclamped, int * clamped, int
   numUnclamped, int numClamped, int AinvStride) */

void keaMatrix_pcSparse::makeFromColMajorPSM
               (MeReal *Qrhs,MeReal *Ainv,MeReal *clampedValues,MeReal *initialSolve,int *unclamped,
               int *clamped,int numUnclamped,int numClamped,int AinvStride)

{
  MeReal *pMVar1;
  int iVar2;
  MeReal MVar3;
  MeReal MVar4;
  uint uVar5;
  uint uVar6;
  MeReal *pMVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  int in_stack_00000028;
  int local_2c;
  MeReal *ri;
  int mp;
  int mb;
  int k;
  int i;
  
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)]
                       Unresolved local var: int r@[DW_OP_reg3(EBX)] */
  MVar3 = Qrhs[5];
  MVar4 = Qrhs[2];
  ri = (MeReal *)Qrhs[4];
  i = 0;
  if (0 < (int)MVar3) {
    local_2c = 0;
    do {
      iVar10 = 0;
      if (0 < (int)MVar3) {
        uVar5 = (uint)MVar3 & 3;
        pMVar7 = ri + local_2c * 0x10;
        if ((int)MVar3 < 2) {
LAB_00010bba:
          iVar9 = iVar10 + local_2c;
          iVar10 = iVar10 + 1;
          *(MeReal **)((int)Qrhs[8] + iVar9 * 4) = pMVar7;
          pMVar7 = pMVar7 + 0x10;
          if ((int)MVar3 <= iVar10) goto LAB_00010c20;
        }
        else if (uVar5 != 0) {
          if (1 < uVar5) {
            if (2 < uVar5) {
              *(MeReal **)((int)Qrhs[8] + local_2c * 4) = pMVar7;
              pMVar7 = pMVar7 + 0x10;
            }
            iVar10 = (2 < uVar5) + 1;
            *(MeReal **)((int)Qrhs[8] + (local_2c + (uint)(2 < uVar5)) * 4) = pMVar7;
            pMVar7 = pMVar7 + 0x10;
          }
          goto LAB_00010bba;
        }
        do {
          *(MeReal **)((int)Qrhs[8] + (iVar10 + local_2c) * 4) = pMVar7;
          *(MeReal **)((int)Qrhs[8] + (local_2c + 1 + iVar10) * 4) = pMVar7 + 0x10;
          *(MeReal **)((int)Qrhs[8] + (local_2c + 2 + iVar10) * 4) = pMVar7 + 0x20;
          iVar9 = local_2c + 3 + iVar10;
          pMVar1 = pMVar7 + 0x30;
          iVar10 = iVar10 + 4;
          pMVar7 = pMVar7 + 0x40;
          *(MeReal **)((int)Qrhs[8] + iVar9 * 4) = pMVar1;
        } while (iVar10 < (int)MVar3);
      }
LAB_00010c20:
      *(undefined4 *)((int)Qrhs[0xc] + i * 4) = 0;
      *(uint *)((int)Qrhs[0xb] + i * 4) = (int)MVar3 - 1;
      i = i + 1;
      local_2c = local_2c + (int)MVar3;
    } while (i < (int)MVar3);
  }
  uVar5 = 0;
  i = 0;
  if (0 < AinvStride) {
    do {
                    /* Unresolved local var: MeReal * rj@[DW_OP_reg1(ECX)] */
      iVar10 = *(int *)(numUnclamped + i * 4);
      iVar9 = in_stack_00000028 * iVar10;
      k = 0;
      if (0 < AinvStride) {
        pMVar7 = ri + uVar5;
        uVar6 = AinvStride & 3;
        if (AinvStride < 2) {
LAB_00010c9e:
          *pMVar7 = clampedValues[iVar9 + *(int *)(numUnclamped + k * 4)];
          pMVar7 = pMVar7 + 4;
          k = k + 1;
          if (AinvStride <= k) goto LAB_00010d12;
        }
        else if (uVar6 != 0) {
          if (1 < uVar6) {
            if (2 < uVar6) {
              *pMVar7 = clampedValues[iVar9 + *(int *)numUnclamped];
              pMVar7 = pMVar7 + 4;
            }
            k = (int)(2 < uVar6);
            *pMVar7 = clampedValues[iVar9 + *(int *)(numUnclamped + k * 4)];
            pMVar7 = pMVar7 + 4;
            k = k + 1;
          }
          goto LAB_00010c9e;
        }
        do {
          *pMVar7 = clampedValues[iVar9 + *(int *)(numUnclamped + k * 4)];
          pMVar7[4] = clampedValues[iVar9 + *(int *)(numUnclamped + 4 + k * 4)];
          pMVar7[8] = clampedValues[iVar9 + *(int *)(numUnclamped + 8 + k * 4)];
          pMVar7[0xc] = clampedValues[iVar9 + *(int *)(numUnclamped + 0xc + k * 4)];
          pMVar7 = pMVar7 + 0x10;
          k = k + 4;
        } while (k < AinvStride);
      }
LAB_00010d12:
      Ainv[i] = initialSolve[iVar10] - (float)unclamped[iVar10];
      MVar3 = (MeReal)k;
      if (k < (int)MVar4) {
        pMVar7 = ri + uVar5 + k * 4;
        k = (int)MVar4 - k;
        uVar6 = -k & 3;
        MVar3 = MVar4;
        if (uVar6 != 0) {
          if (uVar6 < 3) {
            if (uVar6 < 2) {
              k = k + -1;
              *pMVar7 = 0.0;
              pMVar7 = pMVar7 + 4;
            }
            *pMVar7 = 0.0;
            pMVar7 = pMVar7 + 4;
            k = k + -1;
          }
          *pMVar7 = 0.0;
          pMVar7 = pMVar7 + 4;
          k = k + -1;
          if (k == 0) goto LAB_00010daa;
        }
        do {
          *pMVar7 = 0.0;
          pMVar7[4] = 0.0;
          pMVar7[8] = 0.0;
          pMVar7[0xc] = 0.0;
          pMVar7 = pMVar7 + 0x10;
          k = k + -4;
        } while (k != 0);
      }
LAB_00010daa:
      k = (int)MVar3;
      uVar5 = uVar5 + 1 & 3;
      i = i + 1;
      ri = ri + (uVar5 - 1 & (uint)MVar4) * 4;
    } while (i < AinvStride);
  }
  if ((uVar5 != 0) && (uVar5 != 4)) {
    if ((uVar5 & 1) != 0) {
      k = 0;
      MVar3 = (MeReal)k;
      if (0 < (int)MVar4) {
        uVar6 = -(int)MVar4 & 3;
        pMVar7 = ri + uVar5;
        k = (int)MVar4;
        if (uVar6 == 0) goto LAB_00010e3e;
        if (uVar6 < 3) {
          if (uVar6 < 2) {
            *pMVar7 = 0.0;
            pMVar7 = pMVar7 + 4;
            k = (int)MVar4 - 1;
          }
          *pMVar7 = 0.0;
          pMVar7 = pMVar7 + 4;
          k = k + -1;
        }
        *pMVar7 = 0.0;
        pMVar7 = pMVar7 + 4;
        for (k = k + -1; MVar3 = MVar4, k != 0; k = k + -4) {
LAB_00010e3e:
          *pMVar7 = 0.0;
          pMVar7[4] = 0.0;
          pMVar7[8] = 0.0;
          pMVar7[0xc] = 0.0;
          pMVar7 = pMVar7 + 0x10;
        }
      }
      k = (int)MVar3;
      uVar5 = uVar5 + 1;
      if (uVar5 == 4) goto LAB_00010f6a;
    }
    do {
      if (0 < (int)MVar4) {
        uVar6 = -(int)MVar4 & 3;
        pMVar7 = ri + uVar5;
        k = (int)MVar4;
        if (uVar6 != 0) {
          if (uVar6 < 3) {
            if (uVar6 < 2) {
              *pMVar7 = 0.0;
              pMVar7 = pMVar7 + 4;
              k = (int)MVar4 - 1;
            }
            *pMVar7 = 0.0;
            pMVar7 = pMVar7 + 4;
            k = k + -1;
          }
          *pMVar7 = 0.0;
          pMVar7 = pMVar7 + 4;
          k = k + -1;
          if (k == 0) goto LAB_00010ee4;
        }
        do {
          *pMVar7 = 0.0;
          pMVar7[4] = 0.0;
          pMVar7[8] = 0.0;
          pMVar7[0xc] = 0.0;
          pMVar7 = pMVar7 + 0x10;
          k = k + -4;
        } while (k != 0);
      }
LAB_00010ee4:
      k = 0;
      MVar3 = (MeReal)k;
      if (0 < (int)MVar4) {
        uVar6 = -(int)MVar4 & 3;
        pMVar7 = ri + uVar5 + 1;
        k = (int)MVar4;
        if (uVar6 == 0) goto LAB_00010f34;
        if (uVar6 < 3) {
          if (uVar6 < 2) {
            *pMVar7 = 0.0;
            pMVar7 = pMVar7 + 4;
            k = (int)MVar4 - 1;
          }
          *pMVar7 = 0.0;
          pMVar7 = pMVar7 + 4;
          k = k + -1;
        }
        *pMVar7 = 0.0;
        pMVar7 = pMVar7 + 4;
        for (k = k + -1; MVar3 = MVar4, k != 0; k = k + -4) {
LAB_00010f34:
          *pMVar7 = 0.0;
          pMVar7[4] = 0.0;
          pMVar7[8] = 0.0;
          pMVar7[0xc] = 0.0;
          pMVar7 = pMVar7 + 0x10;
        }
      }
      k = (int)MVar3;
      uVar5 = uVar5 + 2;
    } while (uVar5 != 4);
  }
LAB_00010f6a:
  if (i < (int)MVar4) {
    uVar6 = (int)MVar4 - i & 3;
    if (i + 1 < (int)MVar4) {
      if (uVar6 != 0) {
        if (1 < uVar6) {
          if (2 < uVar6) {
            Ainv[i] = 0.0;
            i = i + 1;
          }
          Ainv[i] = 0.0;
          i = i + 1;
        }
        goto LAB_00010f88;
      }
    }
    else {
LAB_00010f88:
      Ainv[i] = 0.0;
      i = i + 1;
      if ((int)MVar4 <= i) goto LAB_00010fd2;
    }
    do {
      Ainv[i] = 0.0;
      Ainv[i + 1] = 0.0;
      Ainv[i + 2] = 0.0;
      Ainv[i + 3] = 0.0;
      i = i + 4;
    } while (i < (int)MVar4);
  }
LAB_00010fd2:
  iVar10 = k + -1;
  uVar6 = AinvStride - 1U & 3;
  iVar9 = uVar5 - 1;
  i = uVar6 + 1;
  if (3 < (uint)i) {
    return;
  }
  uVar8 = -i & 3;
  if (uVar6 + 2 < 4) {
    if (uVar8 == 0) goto LAB_00011020;
    if (1 < uVar8) {
      if (2 < uVar8) {
        i = uVar6 + 2;
        iVar10 = iVar9 + iVar10 * 4;
        iVar9 = uVar5 - 2;
        ri[iVar10] = 1.0;
        iVar10 = k + -2;
      }
      k = iVar10;
      iVar10 = iVar9 + k * 4;
      iVar9 = iVar9 + -1;
      ri[iVar10] = 1.0;
      i = i + 1;
      iVar10 = k + -1;
    }
  }
  k = iVar10;
  iVar10 = iVar9 + k * 4;
  iVar9 = iVar9 + -1;
  ri[iVar10] = 1.0;
  i = i + 1;
  iVar10 = k + -1;
  if (3 < (uint)i) {
    return;
  }
LAB_00011020:
  do {
    k = iVar10;
    ri[iVar9 + k * 4] = 1.0;
    ri[iVar9 + -5 + k * 4] = 1.0;
    iVar10 = iVar9 + -10;
    iVar2 = iVar9 + -0xf;
    iVar9 = iVar9 + -4;
    ri[iVar10 + k * 4] = 1.0;
    ri[iVar2 + k * 4] = 1.0;
    i = i + 4;
    iVar10 = k + -4;
  } while (i < 4);
  return;
}


