/* ==== checkForCycles ==== */

int checkForCycles(int *I,int *C,int *hilohere,int *clampedhere,int iteration,int n)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int MASK@[DW_OP_reg3(EBX)] */
  iVar4 = 0;
  uVar3 = 0x7fffffff >> (0x1eU - (char)iteration & 0x1f);
  if (0 < n) {
    uVar2 = n & 3;
    if (1 < n) {
      if (uVar2 == 0) goto LAB_00010055;
      if (1 < uVar2) {
        if (2 < uVar2) {
          if (*I == 0) {
            uVar3 = uVar3 & (*clampedhere ^ 0xffffffffU);
          }
          else {
            if ((uVar3 & *clampedhere) == 0) {
              return 0;
            }
            uVar3 = uVar3 & *clampedhere & (*C & *hilohere | (*C | *hilohere) ^ 0xffffffffU);
          }
          if (uVar3 == 0) {
            return 0;
          }
          iVar4 = 1;
        }
        if (I[iVar4] == 0) {
          uVar3 = uVar3 & (clampedhere[iVar4] ^ 0xffffffffU);
        }
        else {
          if ((uVar3 & clampedhere[iVar4]) == 0) {
            return 0;
          }
          uVar3 = uVar3 & clampedhere[iVar4] &
                  (C[iVar4] & hilohere[iVar4] | (C[iVar4] | hilohere[iVar4]) ^ 0xffffffffU);
        }
        if (uVar3 == 0) {
          return 0;
        }
        iVar4 = iVar4 + 1;
      }
    }
    if (I[iVar4] == 0) {
      uVar3 = uVar3 & (clampedhere[iVar4] ^ 0xffffffffU);
    }
    else {
      if ((uVar3 & clampedhere[iVar4]) == 0) {
        return 0;
      }
      uVar3 = uVar3 & clampedhere[iVar4] &
              (C[iVar4] & hilohere[iVar4] | (C[iVar4] | hilohere[iVar4]) ^ 0xffffffffU);
    }
    if ((uVar3 != 0) && (iVar4 = iVar4 + 1, iVar4 < n)) {
LAB_00010055:
      while( true ) {
        if (I[iVar4] == 0) {
          uVar3 = uVar3 & (clampedhere[iVar4] ^ 0xffffffffU);
        }
        else {
          if ((uVar3 & clampedhere[iVar4]) == 0) {
            return 0;
          }
          uVar3 = uVar3 & clampedhere[iVar4] &
                  (C[iVar4] & hilohere[iVar4] | (C[iVar4] | hilohere[iVar4]) ^ 0xffffffffU);
        }
        if (uVar3 == 0) {
          return 0;
        }
        iVar1 = iVar4 + 1;
        if (I[iVar1] == 0) {
          uVar3 = uVar3 & (clampedhere[iVar1] ^ 0xffffffffU);
        }
        else {
          if ((uVar3 & clampedhere[iVar1]) == 0) {
            return 0;
          }
          uVar3 = uVar3 & clampedhere[iVar1] &
                  (C[iVar1] & hilohere[iVar1] | (C[iVar1] | hilohere[iVar1]) ^ 0xffffffffU);
        }
        if (uVar3 == 0) {
          return 0;
        }
        iVar1 = iVar4 + 2;
        if (I[iVar1] == 0) {
          uVar3 = uVar3 & (clampedhere[iVar1] ^ 0xffffffffU);
        }
        else {
          if ((uVar3 & clampedhere[iVar1]) == 0) {
            return 0;
          }
          uVar3 = uVar3 & clampedhere[iVar1] &
                  (C[iVar1] & hilohere[iVar1] | (C[iVar1] | hilohere[iVar1]) ^ 0xffffffffU);
        }
        if (uVar3 == 0) {
          return 0;
        }
        iVar1 = iVar4 + 3;
        if (I[iVar1] == 0) {
          uVar3 = uVar3 & (clampedhere[iVar1] ^ 0xffffffffU);
        }
        else {
          if ((uVar3 & clampedhere[iVar1]) == 0) {
            return 0;
          }
          uVar3 = uVar3 & clampedhere[iVar1] &
                  (C[iVar1] & hilohere[iVar1] | (C[iVar1] | hilohere[iVar1]) ^ 0xffffffffU);
        }
        if (uVar3 == 0) break;
        iVar4 = iVar4 + 4;
        if (n <= iVar4) {
          return uVar3;
        }
      }
      return 0;
    }
  }
  return uVar3;
}


/* ==== solveLCP ==== */

/* DWARF original prototype: int solveLCP(keaLCPSolver * this, keaMatrix * A, MeReal * b, MeReal *
   lower, MeReal * upper, int max_iterations, MeCPUResources cpuType, MeReal velocityZeroTol) */

int keaLCPSolver::solveLCP
              (keaMatrix *A,MeReal *b,MeReal *lower,MeReal *upper,int max_iterations,
              MeCPUResources cpuType,MeReal velocityZeroTol)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  MeCPUResources p_Var6;
  uint uVar7;
  undefined4 extraout_EDX;
  uint uVar8;
  uint *puVar9;
  undefined4 *puVar10;
  undefined4 in_stack_00000020;
  int aiStackY_60 [3];
  int iStackY_54;
  int iStackY_50;
  MeReal *apMStack_48 [2];
  uint uStack_40;
  uint auStack_3c [3];
  int *unclamped;
  int *clamped;
  int *C;
  int *I;
  int inCycle;
  int iteration;
  int numClamped;
  int numUnclamped;
  
                    /* Unresolved local var: int firstBad@[DW_OP_reg0(EAX)]
                       Unresolved local var: int someIndicesSwitched@[DW_OP_reg7(EDI)]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  *(undefined4 *)(A + 0x1c) = in_stack_00000020;
  *(MeReal **)(A + 0x24) = b;
  *(MeReal *)(A + 0x18) = velocityZeroTol;
  apMStack_48[1] = lower;
  apMStack_48[0] = *(MeReal **)(A + 0x2c);
  iStackY_50 = 0x1024c;
  (**(code **)((int)*b + 0x10))();
  apMStack_48[0] = (MeReal *)max_iterations;
  iStackY_50 = 0x1025b;
  keaLCPSolver::setUpper((float *)A);
  apMStack_48[0] = upper;
  iStackY_50 = 0x1026a;
  keaLCPSolver::setLower((float *)A);
  iStackY_50 = 0x10274;
  iVar4 = keaLCPSolver::getFirstBadIndex();
  if (iVar4 != *(int *)(A + 4)) {
                    /* Unresolved local var: int * clampedhere@[DW_OP_reg6(ESI)]
                       Unresolved local var: int * hilohere@[DW_OP_reg3(EBX)] */
    uVar8 = *(int *)(A + 4) * 4 + 0xfU & 0xfffffff0;
    unclamped = (int *)((int)auStack_3c + uVar8 * -6);
    clamped = (int *)((int)auStack_3c + uVar8 * -5);
    C = (int *)((int)auStack_3c + uVar8 * -2);
    I = (int *)((int)auStack_3c - uVar8);
    *(keaMatrix **)(&stack0xffffffb4 + uVar8 * -6) = A;
    *(undefined4 *)((int)&iStackY_50 + uVar8 * -6) = 0x102b4;
    keaLCPSolver::copyXtoInitialSolve();
    piVar2 = I;
    *(int **)((int)apMStack_48 + uVar8 * -6 + 4) = C;
    *(int **)((int)apMStack_48 + uVar8 * -6) = piVar2;
    *(keaMatrix **)(&stack0xffffffb4 + uVar8 * -6) = A;
    *(undefined4 *)((int)&iStackY_50 + uVar8 * -6) = 0x102c8;
    keaLCPSolver::getClampIndices
              (*(int **)(&stack0xffffffb4 + uVar8 * -6),*(int **)((int)apMStack_48 + uVar8 * -6));
    puVar10 = *(undefined4 **)(A + 0x20);
    for (uVar7 = *(uint *)(A + 4) & 0x3fffffff; uVar7 != 0; uVar7 = uVar7 - 1) {
      *puVar10 = 0;
      puVar10 = puVar10 + 1;
    }
    puVar10 = *(undefined4 **)(A + 0x40);
    for (uVar7 = *(uint *)(A + 0x14) & 0x3fffffff; uVar7 != 0; uVar7 = uVar7 - 1) {
      *puVar10 = 0;
      puVar10 = puVar10 + 1;
    }
    puVar9 = auStack_3c + -uVar8;
    for (uVar7 = *(uint *)(A + 4) & 0x3fffffff; uVar7 != 0; uVar7 = uVar7 - 1) {
      *puVar9 = 0;
      puVar9 = puVar9 + 1;
    }
    puVar10 = (undefined4 *)((int)auStack_3c + uVar8 * -3);
    for (uVar7 = *(uint *)(A + 4) & 0x3fffffff; uVar7 != 0; uVar7 = uVar7 - 1) {
      *puVar10 = 0;
      puVar10 = puVar10 + 1;
    }
    iteration = 0;
    p_Var6 = cpuType;
    while (p_Var6 != (MeCPUResources)0x0) {
      inCycle = 0;
      iVar4 = 0;
      *(undefined4 *)((int)&uStack_40 + uVar8 * -6) = 0x1032a;
      keaPushPoolFrame();
      uVar7 = *(uint *)(A + 4);
      auStack_3c[2] = uVar7;
      if (0 < (int)uVar7) {
        uVar5 = uVar7 & 3;
        if ((int)uVar7 < 2) {
LAB_00010348:
          *(uint *)((int)auStack_3c + iVar4 * 4 + uVar8 * -3) =
               (I[iVar4] & 1U) + *(int *)((int)auStack_3c + iVar4 * 4 + uVar8 * -3) * 2;
          uVar7 = auStack_3c[iVar4 - uVar8];
          auStack_3c[iVar4 - uVar8] = (C[iVar4] & 1U) + uVar7 * 2;
          iVar4 = iVar4 + 1;
          if ((int)auStack_3c[2] <= iVar4) goto LAB_0001041b;
        }
        else if (uVar5 != 0) {
          if (1 < uVar5) {
            if (2 < uVar5) {
              *(uint *)((int)auStack_3c + uVar8 * -3) =
                   (*I & 1U) + *(int *)((int)auStack_3c + uVar8 * -3) * 2;
              auStack_3c[-uVar8] = (*C & 1U) + auStack_3c[-uVar8] * 2;
            }
            uVar7 = (uint)(2 < uVar5);
            *(uint *)((int)auStack_3c + uVar7 * 4 + uVar8 * -3) =
                 (I[uVar7] & 1U) + *(int *)((int)auStack_3c + uVar7 * 4 + uVar8 * -3) * 2;
            auStack_3c[uVar7 - uVar8] = (C[uVar7] & 1U) + auStack_3c[uVar7 - uVar8] * 2;
            iVar4 = uVar7 + 1;
          }
          goto LAB_00010348;
        }
        do {
          *(uint *)((int)auStack_3c + iVar4 * 4 + uVar8 * -3) =
               (I[iVar4] & 1U) + *(int *)((int)auStack_3c + iVar4 * 4 + uVar8 * -3) * 2;
          auStack_3c[iVar4 - uVar8] = (C[iVar4] & 1U) + auStack_3c[iVar4 - uVar8] * 2;
          iVar1 = iVar4 + 1;
          *(uint *)((int)auStack_3c + iVar1 * 4 + uVar8 * -3) =
               (I[iVar1] & 1U) + *(int *)((int)auStack_3c + iVar1 * 4 + uVar8 * -3) * 2;
          auStack_3c[iVar1 - uVar8] = (C[iVar1] & 1U) + auStack_3c[iVar1 - uVar8] * 2;
          iVar1 = iVar4 + 2;
          *(uint *)((int)auStack_3c + iVar1 * 4 + uVar8 * -3) =
               (I[iVar1] & 1U) + *(int *)((int)auStack_3c + iVar1 * 4 + uVar8 * -3) * 2;
          auStack_3c[iVar1 - uVar8] = (C[iVar1] & 1U) + auStack_3c[iVar1 - uVar8] * 2;
          iVar1 = iVar4 + 3;
          iVar4 = iVar4 + 4;
          *(uint *)((int)auStack_3c + iVar1 * 4 + uVar8 * -3) =
               (I[iVar1] & 1U) + *(int *)((int)auStack_3c + iVar1 * 4 + uVar8 * -3) * 2;
          uVar7 = auStack_3c[iVar1 - uVar8];
          auStack_3c[iVar1 - uVar8] = (C[iVar1] & 1U) + uVar7 * 2;
        } while (iVar4 < (int)auStack_3c[2]);
      }
LAB_0001041b:
      piVar2 = C;
      *(uint *)((int)&uStack_40 + uVar8 * -6) = uVar7;
      piVar3 = I;
      *(int **)((int)apMStack_48 + uVar8 * -6 + 4) = piVar2;
      *(int **)((int)apMStack_48 + uVar8 * -6) = piVar3;
      *(int **)(&stack0xffffffb4 + uVar8 * -6) = &numUnclamped;
      piVar2 = unclamped;
      *(int **)((int)&iStackY_50 + uVar8 * -6) = &numClamped;
      piVar3 = clamped;
      *(int **)((int)&iStackY_54 + uVar8 * -6) = piVar2;
      *(int **)((int)aiStackY_60 + uVar8 * -6 + 8) = piVar3;
      *(keaMatrix **)((int)aiStackY_60 + uVar8 * -6 + 4) = A;
      numClamped = 0;
      numUnclamped = 0;
      *(undefined4 *)((int)aiStackY_60 + uVar8 * -6) = 0x1044b;
      keaLCPSolver::setClampedValues
                (*(int **)((int)aiStackY_60 + uVar8 * -6 + 4),
                 *(int **)((int)aiStackY_60 + uVar8 * -6 + 8),
                 *(int **)((int)&iStackY_54 + uVar8 * -6),*(int **)((int)&iStackY_50 + uVar8 * -6),
                 *(int **)(&stack0xffffffb4 + uVar8 * -6),*(int **)((int)apMStack_48 + uVar8 * -6));
      piVar2 = clamped;
      *(int *)((int)apMStack_48 + uVar8 * -6) = numClamped;
      iVar4 = numUnclamped;
      *(int **)(&stack0xffffffb4 + uVar8 * -6) = piVar2;
      piVar2 = unclamped;
      *(int *)((int)&iStackY_50 + uVar8 * -6) = iVar4;
      *(int **)((int)&iStackY_54 + uVar8 * -6) = piVar2;
      *(MeReal **)((int)aiStackY_60 + uVar8 * -6 + 8) = lower;
      *(keaMatrix **)((int)aiStackY_60 + uVar8 * -6 + 4) = A;
      *(undefined4 *)((int)aiStackY_60 + uVar8 * -6) = 0x1046b;
      keaLCPSolver::makeXandW
                (*(float **)((int)aiStackY_60 + uVar8 * -6 + 4),
                 *(int **)((int)aiStackY_60 + uVar8 * -6 + 8),
                 *(int *)((int)&iStackY_54 + uVar8 * -6),*(int **)((int)&iStackY_50 + uVar8 * -6),
                 *(int *)(&stack0xffffffb4 + uVar8 * -6));
      iVar4 = numClamped;
      *(int *)((int)apMStack_48 + uVar8 * -6 + 4) = numUnclamped;
      piVar2 = unclamped;
      *(int *)((int)apMStack_48 + uVar8 * -6) = iVar4;
      piVar3 = clamped;
      *(int **)(&stack0xffffffb4 + uVar8 * -6) = piVar2;
      piVar2 = C;
      *(int **)((int)&iStackY_50 + uVar8 * -6) = piVar3;
      piVar3 = I;
      *(int **)((int)&iStackY_54 + uVar8 * -6) = piVar2;
      *(int **)((int)aiStackY_60 + uVar8 * -6 + 8) = piVar3;
      *(keaMatrix **)((int)aiStackY_60 + uVar8 * -6 + 4) = A;
      *(undefined4 *)((int)aiStackY_60 + uVar8 * -6) = 0x1048f;
      p_Var6 = (MeCPUResources)
               keaLCPSolver::blockMurtyChooseNewIndices
                         (*(int **)((int)aiStackY_60 + uVar8 * -6 + 4),
                          *(int **)((int)aiStackY_60 + uVar8 * -6 + 8),
                          *(int **)((int)&iStackY_54 + uVar8 * -6),
                          *(int **)((int)&iStackY_50 + uVar8 * -6),
                          *(int *)(&stack0xffffffb4 + uVar8 * -6),
                          *(int *)((int)apMStack_48 + uVar8 * -6));
      if (p_Var6 != (MeCPUResources)0x0) {
        *(MeCPUResources *)((int)&uStack_40 + uVar8 * -6) = p_Var6;
        *(MeCPUResources *)((int)apMStack_48 + uVar8 * -6 + 4) = p_Var6;
        iVar4 = iteration;
        piVar3 = I;
        *(undefined4 *)((int)apMStack_48 + uVar8 * -6) = *(undefined4 *)(A + 4);
        piVar2 = C;
        *(int *)(&stack0xffffffb4 + uVar8 * -6) = iVar4;
        *(uint *)((int)&iStackY_50 + uVar8 * -6) = (int)auStack_3c + uVar8 * -3;
        *(uint **)((int)&iStackY_54 + uVar8 * -6) = auStack_3c + -uVar8;
        *(int **)((int)aiStackY_60 + uVar8 * -6 + 8) = piVar2;
        *(int **)((int)aiStackY_60 + uVar8 * -6 + 4) = piVar3;
        *(undefined4 *)((int)aiStackY_60 + uVar8 * -6) = 0x104e2;
        iVar4 = checkForCycles(*(int **)((int)aiStackY_60 + uVar8 * -6 + 4),
                               *(int **)((int)aiStackY_60 + uVar8 * -6 + 8),
                               *(int **)((int)&iStackY_54 + uVar8 * -6),
                               *(int **)((int)&iStackY_50 + uVar8 * -6),
                               *(int *)(&stack0xffffffb4 + uVar8 * -6),
                               *(int *)((int)apMStack_48 + uVar8 * -6));
        inCycle = iVar4;
        if (iVar4 != 0) {
          *(undefined4 *)((int)&uStack_40 + uVar8 * -6) = extraout_EDX;
          *(undefined4 *)((int)apMStack_48 + uVar8 * -6 + 4) = extraout_EDX;
          piVar2 = C;
          *(uint **)((int)apMStack_48 + uVar8 * -6) = auStack_3c + -uVar8;
          piVar3 = I;
          *(uint *)(&stack0xffffffb4 + uVar8 * -6) = (int)auStack_3c + uVar8 * -3;
          *(int **)((int)&iStackY_50 + uVar8 * -6) = piVar2;
          *(int **)((int)&iStackY_54 + uVar8 * -6) = piVar3;
          *(int *)((int)aiStackY_60 + uVar8 * -6 + 8) = iVar4;
          *(keaMatrix **)((int)aiStackY_60 + uVar8 * -6 + 4) = A;
          *(undefined4 *)((int)aiStackY_60 + uVar8 * -6) = 0x10502;
          inCycle = keaLCPSolver::commonPivot
                              (*(int *)((int)aiStackY_60 + uVar8 * -6 + 4),
                               *(int **)((int)aiStackY_60 + uVar8 * -6 + 8),
                               *(int **)((int)&iStackY_54 + uVar8 * -6),
                               *(int **)((int)&iStackY_50 + uVar8 * -6),
                               *(int **)(&stack0xffffffb4 + uVar8 * -6));
        }
      }
      *(undefined4 *)((int)&uStack_40 + uVar8 * -6) = 0x1049d;
      keaPopPoolFrame();
      iteration = iteration + 1;
      if ((MeCPUResources)iteration == cpuType) {
        return (int)(MeCPUResources)iteration;
      }
      if (inCycle != 0) {
        return (int)(MeCPUResources)iteration;
      }
    }
  }
  return iteration;
}


