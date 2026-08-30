/* ==== MdtKeaAddConstraintForces ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MdtKeaAddConstraintForces
               (MdtKeaConstraints pconstraints,MdtKeaBody **blist,MdtKeaTransformation *tlist,
               int num_bodies,MdtKeaParameters parameters)

{
  MdtKeaConstraints MVar1;
  kd_agg92 kVar2;
  MdtKeaParameters MVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  MdtKeaParameters *pMVar11;
  undefined4 *puVar12;
  int *piVar13;
  MeReal *pMVar14;
  undefined4 *puVar15;
  MdtKeaConstraints *pMVar16;
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
  MdtKeaBody **ppMVar17;
  undefined4 uVar18;
  keaFunctions_Vanilla *pkVar19;
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
    pMVar11 = &parameters;
    pMVar14 = (MeReal *)&stack0xfffffe34;
    ppMVar17 = blist;
    for (iVar8 = 0x13; iVar8 != 0; iVar8 = iVar8 + -1) {
      *pMVar14 = pMVar11->stepsize;
      pMVar11 = (MdtKeaParameters *)&pMVar11->epsilon;
      pMVar14 = pMVar14 + 1;
    }
    puVar15 = (undefined4 *)&stack0xfffffdd8;
    puVar12 = (undefined4 *)register0x00000010;
    for (iVar8 = 0x17; puVar12 = puVar12 + 1, iVar8 != 0; iVar8 = iVar8 + -1) {
      *puVar15 = *puVar12;
      puVar15 = puVar15 + 1;
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
    MVar3.debug.writeKeaInputData = in_stack_fffffe50;
    MVar3.stepsize = (MeReal)in_stack_fffffe30._0_4_;
    MVar3.epsilon = (MeReal)in_stack_fffffe30._4_4_;
    MVar3.gamma = (MeReal)in_stack_fffffe30._8_4_;
    MVar3.max_iterations = in_stack_fffffe30._12_4_;
    MVar3.velocityZeroTol = (MeReal)in_stack_fffffe30._16_4_;
    MVar3.memory_pool = (void *)in_stack_fffffe30._20_4_;
    MVar3.memory_pool_size = in_stack_fffffe30._24_4_;
    MVar3.cpu_resources = (MeCPUResources)in_stack_fffffe30._28_4_;
    MVar3.debug.writeKeaInputDataFilename = (char *)in_stack_fffffe54;
    MVar3.debug.readKeaInputData = in_stack_fffffe58;
    MVar3.debug.readKeaInputDataFilename = (char *)in_stack_fffffe5c;
    MVar3.debug.writeKeaInterData = in_stack_fffffe60;
    MVar3.debug.writeKeaInterDataFilename = (char *)in_stack_fffffe64;
    MVar3.debug.writeKeaOutputData = in_stack_fffffe68;
    MVar3.debug.writeKeaOutputDataFilename = (char *)in_stack_fffffe6c;
    MVar3.debug.frame = in_stack_fffffe70;
    MVar3.debug.badFrame = in_stack_fffffe74;
    MVar3.debug.badPartition = in_stack_fffffe78;
    gDebugDataFile = keaFunctions::checkPrintDebugInput(MVar1,MVar3,in_stack_fffffe7c,(int)ppMVar17)
    ;
    pkVar19 = &vanillaFunctions;
    (**(code **)(_vanillaFunctions + 0x10))(&vanillaFunctions);
    uVar18 = 0x100b3;
    keaFunctions::initPool(pkVar19,(int)parameters.memory_pool);
    puVar15 = (undefined4 *)&stack0xfffffe2c;
    puVar12 = (undefined4 *)register0x00000010;
    iVar8 = num_bodies;
    for (iVar9 = 0x17; puVar12 = puVar12 + 1, iVar9 != 0; iVar9 = iVar9 + -1) {
      *puVar15 = *puVar12;
      puVar15 = puVar15 + 1;
    }
    kVar2._kd[4] = in_stack_fffffe30[0];
    kVar2._kd[5] = in_stack_fffffe30[1];
    kVar2._kd[6] = in_stack_fffffe30[2];
    kVar2._kd[7] = in_stack_fffffe30[3];
    kVar2._kd[8] = in_stack_fffffe30[4];
    kVar2._kd[9] = in_stack_fffffe30[5];
    kVar2._kd[10] = in_stack_fffffe30[6];
    kVar2._kd[0xb] = in_stack_fffffe30[7];
    kVar2._kd[0xc] = in_stack_fffffe30[8];
    kVar2._kd[0xd] = in_stack_fffffe30[9];
    kVar2._kd[0xe] = in_stack_fffffe30[10];
    kVar2._kd[0xf] = in_stack_fffffe30[0xb];
    kVar2._kd[0x10] = in_stack_fffffe30[0xc];
    kVar2._kd[0x11] = in_stack_fffffe30[0xd];
    kVar2._kd[0x12] = in_stack_fffffe30[0xe];
    kVar2._kd[0x13] = in_stack_fffffe30[0xf];
    kVar2._kd[0x14] = in_stack_fffffe30[0x10];
    kVar2._kd[0x15] = in_stack_fffffe30[0x11];
    kVar2._kd[0x16] = in_stack_fffffe30[0x12];
    kVar2._kd[0x17] = in_stack_fffffe30[0x13];
    kVar2._kd[0x18] = in_stack_fffffe30[0x14];
    kVar2._kd[0x19] = in_stack_fffffe30[0x15];
    kVar2._kd[0x1a] = in_stack_fffffe30[0x16];
    kVar2._kd[0x1b] = in_stack_fffffe30[0x17];
    kVar2._kd[0x1c] = in_stack_fffffe30[0x18];
    kVar2._kd[0x1d] = in_stack_fffffe30[0x19];
    kVar2._kd[0x1e] = in_stack_fffffe30[0x1a];
    kVar2._kd[0x1f] = in_stack_fffffe30[0x1b];
    kVar2._kd[0x20] = in_stack_fffffe30[0x1c];
    kVar2._kd[0x21] = in_stack_fffffe30[0x1d];
    kVar2._kd[0x22] = in_stack_fffffe30[0x1e];
    kVar2._kd[0x23] = in_stack_fffffe30[0x1f];
    kVar2._kd._0_4_ = in_stack_fffffe2c;
    kVar2._kd[0x24] = (char)in_stack_fffffe50;
    kVar2._kd[0x25] = (char)((uint)in_stack_fffffe50 >> 8);
    kVar2._kd[0x26] = (char)((uint)in_stack_fffffe50 >> 0x10);
    kVar2._kd[0x27] = (char)((uint)in_stack_fffffe50 >> 0x18);
    kVar2._kd[0x28] = (char)in_stack_fffffe54;
    kVar2._kd[0x29] = (char)((uint)in_stack_fffffe54 >> 8);
    kVar2._kd[0x2a] = (char)((uint)in_stack_fffffe54 >> 0x10);
    kVar2._kd[0x2b] = (char)((uint)in_stack_fffffe54 >> 0x18);
    kVar2._kd[0x2c] = (char)in_stack_fffffe58;
    kVar2._kd[0x2d] = (char)((uint)in_stack_fffffe58 >> 8);
    kVar2._kd[0x2e] = (char)((uint)in_stack_fffffe58 >> 0x10);
    kVar2._kd[0x2f] = (char)((uint)in_stack_fffffe58 >> 0x18);
    kVar2._kd[0x30] = (char)in_stack_fffffe5c;
    kVar2._kd[0x31] = (char)((uint)in_stack_fffffe5c >> 8);
    kVar2._kd[0x32] = (char)((uint)in_stack_fffffe5c >> 0x10);
    kVar2._kd[0x33] = (char)((uint)in_stack_fffffe5c >> 0x18);
    kVar2._kd[0x34] = (char)in_stack_fffffe60;
    kVar2._kd[0x35] = (char)((uint)in_stack_fffffe60 >> 8);
    kVar2._kd[0x36] = (char)((uint)in_stack_fffffe60 >> 0x10);
    kVar2._kd[0x37] = (char)((uint)in_stack_fffffe60 >> 0x18);
    kVar2._kd[0x38] = (char)in_stack_fffffe64;
    kVar2._kd[0x39] = (char)((uint)in_stack_fffffe64 >> 8);
    kVar2._kd[0x3a] = (char)((uint)in_stack_fffffe64 >> 0x10);
    kVar2._kd[0x3b] = (char)((uint)in_stack_fffffe64 >> 0x18);
    kVar2._kd[0x3c] = (char)in_stack_fffffe68;
    kVar2._kd[0x3d] = (char)((uint)in_stack_fffffe68 >> 8);
    kVar2._kd[0x3e] = (char)((uint)in_stack_fffffe68 >> 0x10);
    kVar2._kd[0x3f] = (char)((uint)in_stack_fffffe68 >> 0x18);
    kVar2._kd[0x40] = (char)in_stack_fffffe6c;
    kVar2._kd[0x41] = (char)((uint)in_stack_fffffe6c >> 8);
    kVar2._kd[0x42] = (char)((uint)in_stack_fffffe6c >> 0x10);
    kVar2._kd[0x43] = (char)((uint)in_stack_fffffe6c >> 0x18);
    kVar2._kd[0x44] = (char)in_stack_fffffe70;
    kVar2._kd[0x45] = (char)((uint)in_stack_fffffe70 >> 8);
    kVar2._kd[0x46] = (char)((uint)in_stack_fffffe70 >> 0x10);
    kVar2._kd[0x47] = (char)((uint)in_stack_fffffe70 >> 0x18);
    kVar2._kd[0x48] = (char)in_stack_fffffe74;
    kVar2._kd[0x49] = (char)((uint)in_stack_fffffe74 >> 8);
    kVar2._kd[0x4a] = (char)((uint)in_stack_fffffe74 >> 0x10);
    kVar2._kd[0x4b] = (char)((uint)in_stack_fffffe74 >> 0x18);
    kVar2._kd[0x4c] = (char)in_stack_fffffe78;
    kVar2._kd[0x4d] = (char)((uint)in_stack_fffffe78 >> 8);
    kVar2._kd[0x4e] = (char)((uint)in_stack_fffffe78 >> 0x10);
    kVar2._kd[0x4f] = (char)((uint)in_stack_fffffe78 >> 0x18);
    kVar2._kd._80_4_ = in_stack_fffffe7c;
    kVar2._kd[0x54] = (char)uVar18;
    kVar2._kd[0x55] = (char)((uint)uVar18 >> 8);
    kVar2._kd[0x56] = (char)((uint)uVar18 >> 0x10);
    kVar2._kd[0x57] = (char)((uint)uVar18 >> 0x18);
    kVar2._kd._88_4_ = pkVar19;
    (**(code **)(_vanillaFunctions + 0x14))(&vanillaFunctions,&mem,kVar2,iVar8);
    (**(code **)(_vanillaFunctions + 4))
              (&vanillaFunctions,mem.invIworld,mem.vhmf,blist,tlist,num_bodies,parameters.stepsize);
    keaFunctions::makejlenandbl2body
              ((int *)&vanillaFunctions,mem.jlen_12padded,mem.jlen,*mem.bl2body_12padded,
               *mem.bl2body,*mem.bl2cbody,*pconstraints.Jbody,pconstraints.Jsize,
               pconstraints.num_rows_inc_padding_partition,
               pconstraints.num_rows_exc_padding_partition,
               (int)pconstraints.num_constraints_partition,pconstraints.num_constraints);
    iVar8 = pconstraints.num_rows_exc_padding;
    if ((pconstraints.num_rows_exc_padding & 3U) != 0) {
      uVar7 = pconstraints.num_rows_exc_padding;
      if (pconstraints.num_rows_exc_padding < 0) {
        uVar7 = pconstraints.num_rows_exc_padding + 3;
      }
      iVar8 = (uVar7 & 0xfffffffc) + 4;
    }
    (**(code **)_vanillaFunctions)
              (&vanillaFunctions,mem.rhs,mem.jm,pconstraints.Jstore,pconstraints.xgamma,
               pconstraints.c,pconstraints.xi,mem.invIworld,mem.bl2body,mem.jlen,mem.vhmf,num_bodies
               ,iVar8,pconstraints.num_rows_inc_padding,parameters.stepsize,parameters.gamma);
    pMVar16 = &constraints;
    piVar13 = (int *)register0x00000010;
    for (iVar8 = 0x17; piVar13 = piVar13 + 1, iVar8 != 0; iVar8 = iVar8 + -1) {
      pMVar16->num_partitions = *piVar13;
      pMVar16 = (MdtKeaConstraints *)&pMVar16->max_partitions;
    }
    jlen_12padded = mem.jlen_12padded;
    bl2body_12padded = mem.bl2body_12padded;
    jm = mem.jm;
    rhs = mem.rhs;
    gPartition = 0;
    iVar8 = constraints.num_partitions;
    if (constraints.num_partitions != 0) {
      do {
        if (0 < constraints.num_constraints_partition[gPartition]) {
                    /* Unresolved local var: int num_constraints@[DW_OP_reg7(EDI)]
                       Unresolved local var: int num_rows@[DW_OP_reg3(EBX)]
                       Unresolved local var: int ceil4_num_rows@[DW_OP_reg6(ESI)]
                       Unresolved local var: int ceil12_num_rows@[DW_OP_reg2(EDX)] */
          if ((parameters.debug.writeKeaInterData != 0) && (gDebug->frame == gDebug->badFrame)) {
            writeIntToFile(gDebugDataFile,"partition",gPartition);
          }
          keaPushPoolFrame();
          uVar7 = constraints.num_rows_exc_padding_partition[gPartition];
          iVar8 = constraints.num_constraints_partition[gPartition];
          uVar4 = uVar7;
          if ((uVar7 & 3) != 0) {
            if ((int)uVar7 < 0) {
              uVar4 = uVar7 + 3;
            }
            uVar4 = (uVar4 & 0xfffffffc) + 4;
          }
          if ((int)uVar7 % 0xc != 0) {
            uVar7 = (uVar7 - (int)uVar7 % 0xc) + 0xc;
          }
          uVar5 = uVar4;
          if ((int)uVar4 < 0) {
            uVar5 = uVar4 + 3;
          }
          if ((int)uVar7 < 0) {
            uVar7 = uVar7 + 3;
          }
                    /* Unresolved local var: keaMatrix * A@[???] */
          _vanillaAMatrix = keaLCPSolver::solveLCP;
          (*_solveLCP)();
          keaLCPSolver::allocate((int)&kSolver);
          (**(code **)(_vanillaAMatrix + 4))
                    (&vanillaAMatrix,jm,constraints.Jstore,jlen_12padded,bl2body_12padded,
                     constraints.slipfactor,parameters.epsilon,1.0 / parameters.stepsize);
          (**(code **)(_vanillaAMatrix + 0xc))(&vanillaAMatrix);
          (**(code **)(_vanillaAMatrix + 0x1c))(&vanillaAMatrix);
          uVar10 = 0;
          keaLCPSolver::solveLCP
                    ((keaMatrix *)&kSolver,(float *)&vanillaAMatrix,rhs,constraints.lo,
                     (int)constraints.hi,(_MeCPUResources *)parameters.max_iterations,
                     (float)parameters.cpu_resources);
          if (uVar4 != 0) {
            uVar6 = uVar4 & 3;
            if (uVar6 != 0) {
              if (1 < uVar6) {
                if (2 < uVar6) {
                  *constraints.lambda = *kSolver.x;
                }
                uVar10 = (uint)(2 < uVar6);
                constraints.lambda[uVar10] = kSolver.x[uVar10];
                uVar10 = uVar10 + 1;
              }
              constraints.lambda[uVar10] = kSolver.x[uVar10];
              uVar10 = uVar10 + 1;
              if (uVar10 == uVar4) goto LAB_0001040f;
            }
            do {
              constraints.lambda[uVar10] = kSolver.x[uVar10];
              constraints.lambda[uVar10 + 1] = kSolver.x[uVar10 + 1];
              constraints.lambda[uVar10 + 2] = kSolver.x[uVar10 + 2];
              iVar9 = uVar10 + 3;
              uVar10 = uVar10 + 4;
              constraints.lambda[iVar9] = kSolver.x[iVar9];
            } while (uVar10 != uVar4);
          }
LAB_0001040f:
          iVar9 = constraints.num_rows_inc_padding_partition[gPartition];
          if (iVar9 < 0) {
            iVar9 = iVar9 + 3;
          }
          rhs = rhs + ((int)uVar5 >> 2) * 4;
          jm = jm + (iVar9 >> 2);
          jlen_12padded = jlen_12padded + ((int)uVar7 >> 2);
          bl2body_12padded = bl2body_12padded + ((int)uVar7 >> 2);
          constraints.Jstore = constraints.Jstore + (iVar9 >> 2);
          constraints.Jbody = constraints.Jbody + iVar8;
          constraints.xi = constraints.xi + uVar4;
          constraints.lo = constraints.lo + uVar4;
          constraints.c = constraints.c + uVar4;
          constraints.slipfactor = constraints.slipfactor + uVar4;
          constraints.hi = constraints.hi + uVar4;
          constraints.lambda = constraints.lambda + uVar4;
          constraints.xgamma = constraints.xgamma + uVar4;
          constraints.Jsize = constraints.Jsize + iVar8;
          constraints.Jofs = constraints.Jofs + iVar8;
          constraints.force = constraints.force + iVar8;
          keaPopPoolFrame();
          iVar8 = constraints.num_partitions;
        }
        gPartition = gPartition + 1;
      } while (gPartition != iVar8);
    }
    (**(code **)(_vanillaFunctions + 8))
              (&vanillaFunctions,blist,pconstraints.force,pconstraints.Jstore,pconstraints.Jbody,
               pconstraints.lambda,mem.bl2body,mem.bl2cbody,mem.jlen,
               pconstraints.num_rows_exc_padding,pconstraints.num_rows_inc_padding,
               pconstraints.num_constraints,num_bodies);
    (**(code **)(_vanillaFunctions + 0xc))(&vanillaFunctions,blist,mem.invIworld,num_bodies);
    keaFunctions::keaCloseDebugDataFile((int)&vanillaFunctions);
  }
  return;
}


/* ==== MdtKeaQueryCPUResources ==== */

MeCPUResources MdtKeaQueryCPUResources(void)

{
  return (MeCPUResources)0x0;
}


