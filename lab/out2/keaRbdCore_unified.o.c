/* ==== MdtKeaAddConstraintForces ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void MdtKeaAddConstraintForces
               (MdtKeaConstraints pconstraints,MdtKeaBody **blist,MdtKeaTransformation *tlist,
               int num_bodies,MdtKeaParameters parameters)

{
  MdtKeaConstraints MVar1;
  MdtKeaParameters MVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  MdtKeaParameters *pMVar9;
  undefined4 *puVar10;
  int *piVar11;
  MeReal *pMVar12;
  undefined4 *puVar13;
  MdtKeaConstraints *pMVar14;
  undefined1 in_stack_fffffdd8 [72];
  undefined4 in_stack_fffffe20;
  undefined4 in_stack_fffffe24;
  undefined4 in_stack_fffffe28;
  MdtKeaBodyIndexPair *in_stack_fffffe2c;
  undefined1 in_stack_fffffe30 [32];
  undefined4 in_stack_fffffe50;
  undefined4 in_stack_fffffe54;
  undefined4 in_stack_fffffe58;
  undefined4 in_stack_fffffe5c;
  undefined4 in_stack_fffffe60;
  undefined4 in_stack_fffffe64;
  undefined4 in_stack_fffffe68;
  undefined4 in_stack_fffffe6c;
  undefined4 in_stack_fffffe70;
  undefined4 in_stack_fffffe74;
  int in_stack_fffffe78;
  MdtKeaBody **in_stack_fffffe7c;
  int ceil3_num_strips;
  int num_strips;
  MeReal *rhs;
  MdtKeaJBlockPair *jm;
  int (*bl2body_12padded) [8];
  int *jlen_12padded;
  keaMatrix_pcSparse_vanilla vanillaAMatrix;
  keaLCPSolver kSolver;
  keaFunctions_Vanilla vanillaFunctions;
  MdtKeaConstraints constraints;
  keaTempMemory mem;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: keaFunctions * keaFuncs@[???] */
  _vanillaFunctions = keaFunctions::initPool;
  if (num_bodies != 0) {
    gDebug = &parameters.debug;
    pMVar9 = &parameters;
    pMVar12 = (MeReal *)&stack0xfffffe34;
    for (iVar6 = 0x13; iVar6 != 0; iVar6 = iVar6 + -1) {
      *pMVar12 = pMVar9->stepsize;
      pMVar9 = (MdtKeaParameters *)&pMVar9->epsilon;
      pMVar12 = pMVar12 + 1;
    }
    puVar13 = (undefined4 *)&stack0xfffffdd8;
    puVar10 = (undefined4 *)register0x00000010;
    for (iVar6 = 0x17; puVar10 = puVar10 + 1, iVar6 != 0; iVar6 = iVar6 + -1) {
      *puVar13 = *puVar10;
      puVar13 = puVar13 + 1;
    }
    MVar1.max_partitions = in_stack_fffffdd8._0_4_;
    MVar1.num_rows_exc_padding_partition = (int *)in_stack_fffffdd8._4_4_;
    MVar1.num_rows_inc_padding_partition = (int *)in_stack_fffffdd8._8_4_;
    MVar1.num_constraints_partition = (int *)in_stack_fffffdd8._12_4_;
    MVar1.num_rows_inc_padding = in_stack_fffffdd8._16_4_;
    MVar1.num_rows_exc_padding = in_stack_fffffdd8._20_4_;
    MVar1.max_rows_inc_padding = in_stack_fffffdd8._24_4_;
    MVar1.max_rows_exc_padding = in_stack_fffffdd8._28_4_;
    MVar1.num_constraints = in_stack_fffffdd8._32_4_;
    MVar1.max_constraints = in_stack_fffffdd8._36_4_;
    MVar1.Jstore = (MdtKeaJBlockPair *)in_stack_fffffdd8._40_4_;
    MVar1.xi = (MeReal *)in_stack_fffffdd8._44_4_;
    MVar1.c = (MeReal *)in_stack_fffffdd8._48_4_;
    MVar1.lo = (MeReal *)in_stack_fffffdd8._52_4_;
    MVar1.hi = (MeReal *)in_stack_fffffdd8._56_4_;
    MVar1.lambda = (MeReal *)in_stack_fffffdd8._60_4_;
    MVar1.force = (MdtKeaForcePair *)in_stack_fffffdd8._64_4_;
    MVar1.slipfactor = (MeReal *)in_stack_fffffdd8._68_4_;
    MVar1.num_partitions = (int)&vanillaFunctions;
    MVar1.xgamma = (MeReal *)in_stack_fffffe20;
    MVar1.Jsize = (int *)in_stack_fffffe24;
    MVar1.Jofs = (int *)in_stack_fffffe28;
    MVar1.Jbody = in_stack_fffffe2c;
    MVar2.debug.writeKeaInputData = in_stack_fffffe50;
    MVar2.stepsize = (MeReal)in_stack_fffffe30._0_4_;
    MVar2.epsilon = (MeReal)in_stack_fffffe30._4_4_;
    MVar2.gamma = (MeReal)in_stack_fffffe30._8_4_;
    MVar2.max_iterations = in_stack_fffffe30._12_4_;
    MVar2.velocityZeroTol = (MeReal)in_stack_fffffe30._16_4_;
    MVar2.memory_pool = (void *)in_stack_fffffe30._20_4_;
    MVar2.memory_pool_size = in_stack_fffffe30._24_4_;
    MVar2.cpu_resources = (MeCPUResources)in_stack_fffffe30._28_4_;
    MVar2.debug.writeKeaInputDataFilename = (char *)in_stack_fffffe54;
    MVar2.debug.readKeaInputData = in_stack_fffffe58;
    MVar2.debug.readKeaInputDataFilename = (char *)in_stack_fffffe5c;
    MVar2.debug.writeKeaInterData = in_stack_fffffe60;
    MVar2.debug.writeKeaInterDataFilename = (char *)in_stack_fffffe64;
    MVar2.debug.writeKeaOutputData = in_stack_fffffe68;
    MVar2.debug.writeKeaOutputDataFilename = (char *)in_stack_fffffe6c;
    MVar2.debug.frame = in_stack_fffffe70;
    MVar2.debug.badFrame = in_stack_fffffe74;
    MVar2.debug.badPartition = in_stack_fffffe78;
    gDebugDataFile = keaFunctions::checkPrintDebugInput(MVar1,MVar2,in_stack_fffffe7c,(int)blist);
    (**(code **)(_vanillaFunctions + 0x10))();
    keaFunctions::initPool(&vanillaFunctions,(int)parameters.memory_pool);
    puVar13 = (undefined4 *)&stack0xfffffe2c;
    puVar10 = (undefined4 *)register0x00000010;
    for (iVar6 = 0x17; puVar10 = puVar10 + 1, iVar6 != 0; iVar6 = iVar6 + -1) {
      *puVar13 = *puVar10;
      puVar13 = puVar13 + 1;
    }
    (**(code **)(_vanillaFunctions + 0x14))();
    (**(code **)(_vanillaFunctions + 4))();
    keaFunctions::makejlenandbl2body
              ((int *)&vanillaFunctions,mem.jlen_12padded,mem.jlen,*mem.bl2body_12padded,
               *mem.bl2body,*mem.bl2cbody,*pconstraints.Jbody,pconstraints.Jsize,
               pconstraints.num_rows_inc_padding_partition,
               pconstraints.num_rows_exc_padding_partition,
               (int)pconstraints.num_constraints_partition,pconstraints.num_constraints);
    (**(code **)_vanillaFunctions)();
    pMVar14 = &constraints;
    piVar11 = (int *)register0x00000010;
    for (iVar6 = 0x17; piVar11 = piVar11 + 1, iVar6 != 0; iVar6 = iVar6 + -1) {
      pMVar14->num_partitions = *piVar11;
      pMVar14 = (MdtKeaConstraints *)&pMVar14->max_partitions;
    }
    rhs = mem.rhs;
    gPartition = 0;
    iVar6 = constraints.num_partitions;
    if (constraints.num_partitions != 0) {
      do {
        if (0 < constraints.num_constraints_partition[gPartition]) {
                    /* Unresolved local var: int num_constraints@[DW_OP_reg7(EDI)]
                       Unresolved local var: int num_rows@[DW_OP_reg3(EBX)]
                       Unresolved local var: int ceil4_num_rows@[DW_OP_reg6(ESI)]
                       Unresolved local var: int ceil12_num_rows@[DW_OP_reg2(EDX)] */
          if ((parameters.debug.writeKeaInterData != 0) && (gDebug->frame == gDebug->badFrame)) {
            writeIntToFile();
          }
          keaPushPoolFrame();
          uVar3 = constraints.num_rows_exc_padding_partition[gPartition];
          iVar6 = constraints.num_constraints_partition[gPartition];
          if ((uVar3 & 3) != 0) {
            if ((int)uVar3 < 0) {
              uVar3 = uVar3 + 3;
            }
            uVar3 = (uVar3 & 0xfffffffc) + 4;
          }
          uVar4 = uVar3;
          if ((int)uVar3 < 0) {
            uVar4 = uVar3 + 3;
          }
                    /* Unresolved local var: keaMatrix * A@[???] */
          _vanillaAMatrix = keaLCPSolver::solveLCP;
          (*_solveLCP)();
          keaLCPSolver::allocate((int)&kSolver);
          (**(code **)(_vanillaAMatrix + 4))();
          (**(code **)(_vanillaAMatrix + 0xc))();
          (**(code **)(_vanillaAMatrix + 0x1c))();
          uVar8 = 0;
          keaLCPSolver::solveLCP
                    ((keaMatrix *)&kSolver,(float *)&vanillaAMatrix,rhs,constraints.lo,
                     (int)constraints.hi,(_MeCPUResources *)parameters.max_iterations,
                     (float)parameters.cpu_resources);
          if (uVar3 != 0) {
            uVar5 = uVar3 & 3;
            if (uVar5 != 0) {
              if (1 < uVar5) {
                if (2 < uVar5) {
                  *constraints.lambda = *kSolver.x;
                }
                uVar8 = (uint)(2 < uVar5);
                constraints.lambda[uVar8] = kSolver.x[uVar8];
                uVar8 = uVar8 + 1;
              }
              constraints.lambda[uVar8] = kSolver.x[uVar8];
              uVar8 = uVar8 + 1;
              if (uVar8 == uVar3) goto LAB_0001040f;
            }
            do {
              constraints.lambda[uVar8] = kSolver.x[uVar8];
              constraints.lambda[uVar8 + 1] = kSolver.x[uVar8 + 1];
              constraints.lambda[uVar8 + 2] = kSolver.x[uVar8 + 2];
              iVar7 = uVar8 + 3;
              uVar8 = uVar8 + 4;
              constraints.lambda[iVar7] = kSolver.x[iVar7];
            } while (uVar8 != uVar3);
          }
LAB_0001040f:
          iVar7 = constraints.num_rows_inc_padding_partition[gPartition];
          if (iVar7 < 0) {
            iVar7 = iVar7 + 3;
          }
          rhs = rhs + ((int)uVar4 >> 2) * 4;
          constraints.Jstore = constraints.Jstore + (iVar7 >> 2);
          constraints.Jbody = constraints.Jbody + iVar6;
          constraints.xi = constraints.xi + uVar3;
          constraints.lo = constraints.lo + uVar3;
          constraints.c = constraints.c + uVar3;
          constraints.slipfactor = constraints.slipfactor + uVar3;
          constraints.hi = constraints.hi + uVar3;
          constraints.lambda = constraints.lambda + uVar3;
          constraints.xgamma = constraints.xgamma + uVar3;
          constraints.Jsize = constraints.Jsize + iVar6;
          constraints.Jofs = constraints.Jofs + iVar6;
          constraints.force = constraints.force + iVar6;
          keaPopPoolFrame();
          iVar6 = constraints.num_partitions;
        }
        gPartition = gPartition + 1;
      } while (gPartition != iVar6);
    }
    (**(code **)(_vanillaFunctions + 8))();
    (**(code **)(_vanillaFunctions + 0xc))();
    keaFunctions::keaCloseDebugDataFile((int)&vanillaFunctions);
  }
  return;
}


