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

int __thiscall
keaLCPSolver::solveLCP
          (keaLCPSolver *this,keaMatrix *A,MeReal *b,MeReal *lower,MeReal *upper,int max_iterations,
          MeCPUResources cpuType,MeReal velocityZeroTol)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  undefined4 extraout_EDX;
  uint uVar6;
  int *piVar7;
  MeReal *pMVar8;
  uint *puVar9;
  undefined4 *puVar10;
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
  this->velocityZeroTol = velocityZeroTol;
  this->A = A;
  this->cpuType = cpuType;
  apMStack_48[1] = b;
  apMStack_48[0] = this->x;
  iStackY_50 = 0x1024c;
  (**(code **)(*(int *)A + 0x10))();
  apMStack_48[0] = upper;
  iStackY_50 = 0x1025b;
  keaLCPSolver::setUpper((float *)this);
  apMStack_48[0] = lower;
  iStackY_50 = 0x1026a;
  keaLCPSolver::setLower((float *)this);
  iStackY_50 = 0x10274;
  iVar2 = keaLCPSolver::getFirstBadIndex();
  if (iVar2 != this->n) {
                    /* Unresolved local var: int * clampedhere@[DW_OP_reg6(ESI)]
                       Unresolved local var: int * hilohere@[DW_OP_reg3(EBX)] */
    uVar6 = this->n * 4 + 0xfU & 0xfffffff0;
    unclamped = (int *)((int)auStack_3c + uVar6 * -6);
    clamped = (int *)((int)auStack_3c + uVar6 * -5);
    C = (int *)((int)auStack_3c + uVar6 * -2);
    I = (int *)((int)auStack_3c - uVar6);
    *(keaLCPSolver **)(&stack0xffffffb4 + uVar6 * -6) = this;
    *(undefined4 *)((int)&iStackY_50 + uVar6 * -6) = 0x102b4;
    keaLCPSolver::copyXtoInitialSolve();
    piVar7 = I;
    *(int **)((int)apMStack_48 + uVar6 * -6 + 4) = C;
    *(int **)((int)apMStack_48 + uVar6 * -6) = piVar7;
    *(keaLCPSolver **)(&stack0xffffffb4 + uVar6 * -6) = this;
    *(undefined4 *)((int)&iStackY_50 + uVar6 * -6) = 0x102c8;
    keaLCPSolver::getClampIndices
              (*(int **)(&stack0xffffffb4 + uVar6 * -6),*(int **)((int)apMStack_48 + uVar6 * -6));
    piVar7 = this->cached;
    for (uVar5 = this->n & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
      *piVar7 = 0;
      piVar7 = piVar7 + 1;
    }
    pMVar8 = this->clampedValues;
    for (uVar5 = this->c16c12n & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
      *pMVar8 = 0.0;
      pMVar8 = pMVar8 + 1;
    }
    puVar9 = auStack_3c + -uVar6;
    for (uVar5 = this->n & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
      *puVar9 = 0;
      puVar9 = puVar9 + 1;
    }
    puVar10 = (undefined4 *)((int)auStack_3c + uVar6 * -3);
    for (uVar5 = this->n & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
      *puVar10 = 0;
      puVar10 = puVar10 + 1;
    }
    iteration = 0;
    iVar2 = max_iterations;
    while (iVar2 != 0) {
      inCycle = 0;
      iVar2 = 0;
      *(undefined4 *)((int)&uStack_40 + uVar6 * -6) = 0x1032a;
      keaPushPoolFrame();
      uVar5 = this->n;
      auStack_3c[2] = uVar5;
      if (0 < (int)uVar5) {
        uVar3 = uVar5 & 3;
        if ((int)uVar5 < 2) {
LAB_00010348:
          *(uint *)((int)auStack_3c + iVar2 * 4 + uVar6 * -3) =
               (I[iVar2] & 1U) + *(int *)((int)auStack_3c + iVar2 * 4 + uVar6 * -3) * 2;
          uVar5 = auStack_3c[iVar2 - uVar6];
          auStack_3c[iVar2 - uVar6] = (C[iVar2] & 1U) + uVar5 * 2;
          iVar2 = iVar2 + 1;
          if ((int)auStack_3c[2] <= iVar2) goto LAB_0001041b;
        }
        else if (uVar3 != 0) {
          if (1 < uVar3) {
            if (2 < uVar3) {
              *(uint *)((int)auStack_3c + uVar6 * -3) =
                   (*I & 1U) + *(int *)((int)auStack_3c + uVar6 * -3) * 2;
              auStack_3c[-uVar6] = (*C & 1U) + auStack_3c[-uVar6] * 2;
            }
            uVar5 = (uint)(2 < uVar3);
            *(uint *)((int)auStack_3c + uVar5 * 4 + uVar6 * -3) =
                 (I[uVar5] & 1U) + *(int *)((int)auStack_3c + uVar5 * 4 + uVar6 * -3) * 2;
            auStack_3c[uVar5 - uVar6] = (C[uVar5] & 1U) + auStack_3c[uVar5 - uVar6] * 2;
            iVar2 = uVar5 + 1;
          }
          goto LAB_00010348;
        }
        do {
          *(uint *)((int)auStack_3c + iVar2 * 4 + uVar6 * -3) =
               (I[iVar2] & 1U) + *(int *)((int)auStack_3c + iVar2 * 4 + uVar6 * -3) * 2;
          auStack_3c[iVar2 - uVar6] = (C[iVar2] & 1U) + auStack_3c[iVar2 - uVar6] * 2;
          iVar4 = iVar2 + 1;
          *(uint *)((int)auStack_3c + iVar4 * 4 + uVar6 * -3) =
               (I[iVar4] & 1U) + *(int *)((int)auStack_3c + iVar4 * 4 + uVar6 * -3) * 2;
          auStack_3c[iVar4 - uVar6] = (C[iVar4] & 1U) + auStack_3c[iVar4 - uVar6] * 2;
          iVar4 = iVar2 + 2;
          *(uint *)((int)auStack_3c + iVar4 * 4 + uVar6 * -3) =
               (I[iVar4] & 1U) + *(int *)((int)auStack_3c + iVar4 * 4 + uVar6 * -3) * 2;
          auStack_3c[iVar4 - uVar6] = (C[iVar4] & 1U) + auStack_3c[iVar4 - uVar6] * 2;
          iVar4 = iVar2 + 3;
          iVar2 = iVar2 + 4;
          *(uint *)((int)auStack_3c + iVar4 * 4 + uVar6 * -3) =
               (I[iVar4] & 1U) + *(int *)((int)auStack_3c + iVar4 * 4 + uVar6 * -3) * 2;
          uVar5 = auStack_3c[iVar4 - uVar6];
          auStack_3c[iVar4 - uVar6] = (C[iVar4] & 1U) + uVar5 * 2;
        } while (iVar2 < (int)auStack_3c[2]);
      }
LAB_0001041b:
      piVar7 = C;
      *(uint *)((int)&uStack_40 + uVar6 * -6) = uVar5;
      piVar1 = I;
      *(int **)((int)apMStack_48 + uVar6 * -6 + 4) = piVar7;
      *(int **)((int)apMStack_48 + uVar6 * -6) = piVar1;
      *(int **)(&stack0xffffffb4 + uVar6 * -6) = &numUnclamped;
      piVar7 = unclamped;
      *(int **)((int)&iStackY_50 + uVar6 * -6) = &numClamped;
      piVar1 = clamped;
      *(int **)((int)&iStackY_54 + uVar6 * -6) = piVar7;
      *(int **)((int)aiStackY_60 + uVar6 * -6 + 8) = piVar1;
      *(keaLCPSolver **)((int)aiStackY_60 + uVar6 * -6 + 4) = this;
      numClamped = 0;
      numUnclamped = 0;
      *(undefined4 *)((int)aiStackY_60 + uVar6 * -6) = 0x1044b;
      keaLCPSolver::setClampedValues
                (*(int **)((int)aiStackY_60 + uVar6 * -6 + 4),
                 *(int **)((int)aiStackY_60 + uVar6 * -6 + 8),
                 *(int **)((int)&iStackY_54 + uVar6 * -6),*(int **)((int)&iStackY_50 + uVar6 * -6),
                 *(int **)(&stack0xffffffb4 + uVar6 * -6),*(int **)((int)apMStack_48 + uVar6 * -6));
      piVar7 = clamped;
      *(int *)((int)apMStack_48 + uVar6 * -6) = numClamped;
      iVar2 = numUnclamped;
      *(int **)(&stack0xffffffb4 + uVar6 * -6) = piVar7;
      piVar7 = unclamped;
      *(int *)((int)&iStackY_50 + uVar6 * -6) = iVar2;
      *(int **)((int)&iStackY_54 + uVar6 * -6) = piVar7;
      *(MeReal **)((int)aiStackY_60 + uVar6 * -6 + 8) = b;
      *(keaLCPSolver **)((int)aiStackY_60 + uVar6 * -6 + 4) = this;
      *(undefined4 *)((int)aiStackY_60 + uVar6 * -6) = 0x1046b;
      keaLCPSolver::makeXandW
                (*(float **)((int)aiStackY_60 + uVar6 * -6 + 4),
                 *(int **)((int)aiStackY_60 + uVar6 * -6 + 8),
                 *(int *)((int)&iStackY_54 + uVar6 * -6),*(int **)((int)&iStackY_50 + uVar6 * -6),
                 *(int *)(&stack0xffffffb4 + uVar6 * -6));
      iVar2 = numClamped;
      *(int *)((int)apMStack_48 + uVar6 * -6 + 4) = numUnclamped;
      piVar7 = unclamped;
      *(int *)((int)apMStack_48 + uVar6 * -6) = iVar2;
      piVar1 = clamped;
      *(int **)(&stack0xffffffb4 + uVar6 * -6) = piVar7;
      piVar7 = C;
      *(int **)((int)&iStackY_50 + uVar6 * -6) = piVar1;
      piVar1 = I;
      *(int **)((int)&iStackY_54 + uVar6 * -6) = piVar7;
      *(int **)((int)aiStackY_60 + uVar6 * -6 + 8) = piVar1;
      *(keaLCPSolver **)((int)aiStackY_60 + uVar6 * -6 + 4) = this;
      *(undefined4 *)((int)aiStackY_60 + uVar6 * -6) = 0x1048f;
      iVar2 = keaLCPSolver::blockMurtyChooseNewIndices
                        (*(int **)((int)aiStackY_60 + uVar6 * -6 + 4),
                         *(int **)((int)aiStackY_60 + uVar6 * -6 + 8),
                         *(int **)((int)&iStackY_54 + uVar6 * -6),
                         *(int **)((int)&iStackY_50 + uVar6 * -6),
                         *(int *)(&stack0xffffffb4 + uVar6 * -6),
                         *(int *)((int)apMStack_48 + uVar6 * -6));
      if (iVar2 != 0) {
        *(int *)((int)&uStack_40 + uVar6 * -6) = iVar2;
        *(int *)((int)apMStack_48 + uVar6 * -6 + 4) = iVar2;
        iVar4 = iteration;
        piVar1 = I;
        *(int *)((int)apMStack_48 + uVar6 * -6) = this->n;
        piVar7 = C;
        *(int *)(&stack0xffffffb4 + uVar6 * -6) = iVar4;
        *(uint *)((int)&iStackY_50 + uVar6 * -6) = (int)auStack_3c + uVar6 * -3;
        *(uint **)((int)&iStackY_54 + uVar6 * -6) = auStack_3c + -uVar6;
        *(int **)((int)aiStackY_60 + uVar6 * -6 + 8) = piVar7;
        *(int **)((int)aiStackY_60 + uVar6 * -6 + 4) = piVar1;
        *(undefined4 *)((int)aiStackY_60 + uVar6 * -6) = 0x104e2;
        iVar4 = checkForCycles(*(int **)((int)aiStackY_60 + uVar6 * -6 + 4),
                               *(int **)((int)aiStackY_60 + uVar6 * -6 + 8),
                               *(int **)((int)&iStackY_54 + uVar6 * -6),
                               *(int **)((int)&iStackY_50 + uVar6 * -6),
                               *(int *)(&stack0xffffffb4 + uVar6 * -6),
                               *(int *)((int)apMStack_48 + uVar6 * -6));
        inCycle = iVar4;
        if (iVar4 != 0) {
          *(undefined4 *)((int)&uStack_40 + uVar6 * -6) = extraout_EDX;
          *(undefined4 *)((int)apMStack_48 + uVar6 * -6 + 4) = extraout_EDX;
          piVar7 = C;
          *(uint **)((int)apMStack_48 + uVar6 * -6) = auStack_3c + -uVar6;
          piVar1 = I;
          *(uint *)(&stack0xffffffb4 + uVar6 * -6) = (int)auStack_3c + uVar6 * -3;
          *(int **)((int)&iStackY_50 + uVar6 * -6) = piVar7;
          *(int **)((int)&iStackY_54 + uVar6 * -6) = piVar1;
          *(int *)((int)aiStackY_60 + uVar6 * -6 + 8) = iVar4;
          *(keaLCPSolver **)((int)aiStackY_60 + uVar6 * -6 + 4) = this;
          *(undefined4 *)((int)aiStackY_60 + uVar6 * -6) = 0x10502;
          inCycle = keaLCPSolver::commonPivot
                              (*(int *)((int)aiStackY_60 + uVar6 * -6 + 4),
                               *(int **)((int)aiStackY_60 + uVar6 * -6 + 8),
                               *(int **)((int)&iStackY_54 + uVar6 * -6),
                               *(int **)((int)&iStackY_50 + uVar6 * -6),
                               *(int **)(&stack0xffffffb4 + uVar6 * -6));
        }
      }
      *(undefined4 *)((int)&uStack_40 + uVar6 * -6) = 0x1049d;
      keaPopPoolFrame();
      iteration = iteration + 1;
      if (iteration == max_iterations) {
        return iteration;
      }
      if (inCycle != 0) {
        return iteration;
      }
    }
  }
  return iteration;
}


