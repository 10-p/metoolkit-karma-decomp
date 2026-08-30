/* ==== MdtWorldCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MdtWorldID MdtWorldCreate(uint maxBodies,uint maxConstraints,MeReal lengthScale,MeReal massScale)

{
  MdtWorldID pMVar1;
  MdtWorldID w;
  MeCPUResources p_Var2;
  char *pcVar3;
  
                    /* Unresolved local var: MdtWorld * w@[DW_OP_reg6(ESI)]
                       Unresolved local var: int largestConstraintSize@[???] */
  pMVar1 = (MdtWorldID)0x0;
  if (maxBodies != 0) {
    w = (*_MeMemoryAPI)(0x234);
    pMVar1 = (MdtWorldID)0x0;
    if (w != (MdtWorldID)0x0) {
      (w->params).lengthScale = lengthScale;
      (w->params).massScale = massScale;
      (w->params).defaultDensity = massScale / (lengthScale * lengthScale * lengthScale);
      p_Var2 = MdtKeaQueryCPUResources();
      (w->keaParams).cpu_resources = p_Var2;
      MeChunkInit(&w->keaPool,0x10);
      w->maxBodies = maxBodies;
      w->nBodies = 0;
      w->nEnabledBodies = 0;
      MeChunkInit(&w->keaTMChunk,0x10);
      (*_MePoolAPI)(&w->bodyPool,w->maxBodies,0x240,0x10);
      w->maxConstraints = maxConstraints;
      w->nEnabledConstraints = 0;
      (*_MePoolAPI)(&w->constraintPool,w->maxConstraints,0x1ec,0);
      MeDictInit(w,1000000,MdtDictCompare);
      MeDictAllowDupes(w);
      MeDictInit(&w->enabledBodyDict,1000000,MdtDictCompare);
      MeDictAllowDupes(&w->enabledBodyDict);
      MeDictInit(&w->constraintDict,1000000,MdtDictCompare);
      MeDictAllowDupes(&w->constraintDict);
      MeChunkInit(&w->partOutChunk,0);
      MeChunkInit(&w->keaConstraintsChunk,0);
      w->bodyDisableCallback = (MdtBodyCallbackCBPtr)0x0;
      w->bodyEnableCallback = (MdtBodyCallbackCBPtr)0x0;
      w->contactGroupDestroyCallback = (MdtContactGroupDestroyCallbackCBPtr)0x0;
      MdtWorldReset(w);
      pcVar3 = MeToolkitVersionString();
      w->toolkitVersionString = pcVar3;
      pMVar1 = w;
    }
  }
  return pMVar1;
}


/* ==== MdtWorldReset ==== */

void MdtWorldReset(MdtWorldID w)

{
  float fVar1;
  float fVar2;
  
  fVar1 = (w->params).massScale;
  (w->keaParams).gamma = 0.2;
  w->constantGamma = 0.2;
  w->minSafeTime = 0.001;
  (w->partitionParams).velrot_thresh = 0.001;
  (w->partitionParams).alive_time_thresh = 0.2;
  (w->partitionParams).lodParams.frictionRatio = 0.0;
  (w->partitionParams).lodParams.zeroRowBonus = 150.0;
  (w->partitionParams).lodParams.toWorldBonus = 100.0;
  (w->partitionParams).lodParams.rowCountBias = 40.0;
  (w->partitionParams).lodParams.nonAutoBonus = 40.0;
  (w->partitionParams).maxMatrixSize = 0x7ffffffc;
  (w->partitionParams).autoDisable = 1;
  (w->params).gravity[0] = 0.0;
  (w->params).gravity[1] = 0.0;
  (w->params).gravity[2] = 0.0;
  (w->partitionParams).debugOptions = 0;
  fVar2 = (w->params).lengthScale;
  (w->partitionParams).accrot_thresh = 0.05;
  (w->keaParams).epsilon = (1.0 / fVar1) * 0.01;
  (w->keaParams).velocityZeroTol = fVar2 * 0.03;
  (w->partitionParams).vel_thresh = fVar2 * 0.02;
  (w->partitionParams).acc_thresh = fVar2 * 0.05;
  (w->partitionParams).lodParams.normVelBias = fVar2 * 6.5;
  (w->partitionParams).lodParams.penetrationBias = fVar2 * 15.0;
  (w->params).matrixSizeLog = (int *)0x0;
  (w->params).matrixSizeLogSize = 0;
  (w->keaParams).max_iterations = 10;
  (w->keaParams).debug.readKeaInputData = 0;
  (w->keaParams).debug.writeKeaInputData = 0;
  (w->keaParams).debug.writeKeaInterData = 0;
  (w->keaParams).debug.writeKeaOutputData = 0;
  w->checkSim = 0;
  w->simErrorCallback = MdtDefaultSimErrorCallBack;
  w->simErrorUserData = (void *)0x0;
  return;
}


/* ==== MdtWorldStep ==== */

void MdtWorldStep(MdtWorldID w,MeReal stepSize)

{
  MdtBody **keabodyArray_00;
  MeReal MVar1;
  MdtDebugDrawOptions o;
  kd_agg76 kVar2;
  kd_agg76 kVar3;
  MdtPartitionOutput *po_00;
  MeDict *pMVar4;
  void *pvVar5;
  MdtKeaConstraints *keaCon_00;
  uint uVar6;
  MdtBody **ppMVar7;
  void *pvVar8;
  uint uVar9;
  int iVar10;
  int iVar11;
  MdtKeaParameters *pMVar12;
  MdtKeaConstraints *pMVar13;
  MdtKeaParameters *pMVar14;
  MdtKeaForcePair *pMVar15;
  int *piVar16;
  MeReal *pMVar17;
  kd_agg92 in_stack_ffffff04;
  MeReal in_stack_ffffff6c;
  undefined1 in_stack_ffffff70 [48];
  undefined4 uVar18;
  MdtPartitionOutput *pMVar19;
  MeReal MVar20;
  MdtWorldParams *pMVar21;
  undefined4 uVar22;
  MdtWorldID pMVar23;
  MdtBody **ppMVar24;
  MdtKeaConstraints *keaCon;
  MdtKeaTransformation *keatmArray;
  MdtPartitionOutput *po;
  MdtKeaBody **keabodyArray;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int sizeRequired@[DW_OP_reg3(EBX)]
                       Unresolved local var: int maxRows@[DW_OP_reg0(EAX)]
                       Unresolved local var: int totalConCount@[???]
                       Unresolved local var: MdtKeaForcePair * forceArray@[DW_OP_reg6(ESI)]
                       Unresolved local var: MdtBody * b@[???]
                       Unresolved local var: MeDictNode * node@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDict * dict@[???] */
  po_00 = MdtPartOutCreateFromChunk(&w->partOutChunk,w->nBodies,w->nEnabledConstraints);
  pMVar4 = &w->enabledBodyDict;
  MdtUpdatePartitions(pMVar4,po_00,MdtAutoDisableLastPartition,&w->partitionParams);
  if (0 < po_00->nPartitions) {
    pvVar5 = MeChunkGetMem(&w->keaTMChunk,po_00->totalBodies << 6);
    keaCon_00 = MdtKeaConstraintsCreateFromChunk
                          (&w->keaConstraintsChunk,po_00->nPartitions,
                           (po_00->overallInfo).jointCount + (po_00->overallInfo).contactCount,
                           (po_00->overallInfo).rowCount);
    pMVar14 = &w->keaParams;
    pMVar21 = &w->params;
    uVar18 = 0x10401;
    pMVar19 = po_00;
    MVar20 = stepSize;
    uVar6 = MdtPackAllPartitions(po_00,stepSize,pMVar21,pMVar14,pvVar5,keaCon_00);
    keabodyArray_00 = po_00->bodies;
    MVar1 = w->constantGamma;
    (w->keaParams).stepsize = stepSize;
    (w->keaParams).gamma = MVar1;
    ppMVar7 = (MdtBody **)
              MdtKeaMemoryRequired
                        (keaCon_00->num_rows_exc_padding_partition,keaCon_00->num_partitions,uVar6,
                         po_00->totalBodies);
    ppMVar24 = ppMVar7;
    pvVar8 = MeChunkGetMem(&w->keaPool,(int)ppMVar7);
    (w->keaParams).memory_pool = pvVar8;
    (w->keaParams).memory_pool_size = (uint)ppMVar7;
    pMVar23 = (MdtWorldID)0x0;
    uVar22 = 0x10470;
    MdtFlushCache(0);
    pMVar12 = pMVar14;
    pMVar17 = (MeReal *)&stack0xffffff6c;
    for (iVar10 = 0x13; iVar10 != 0; iVar10 = iVar10 + -1) {
      *pMVar17 = pMVar12->stepsize;
      pMVar12 = (MdtKeaParameters *)&pMVar12->epsilon;
      pMVar17 = pMVar17 + 1;
    }
    iVar10 = po_00->totalBodies;
    pMVar13 = keaCon_00;
    piVar16 = (int *)&stack0xffffff04;
    ppMVar7 = keabodyArray_00;
    pvVar8 = pvVar5;
    for (iVar11 = 0x17; iVar11 != 0; iVar11 = iVar11 + -1) {
      *piVar16 = pMVar13->num_partitions;
      pMVar13 = (MdtKeaConstraints *)&pMVar13->max_partitions;
      piVar16 = piVar16 + 1;
    }
    kVar2._kd[4] = in_stack_ffffff70[0];
    kVar2._kd[5] = in_stack_ffffff70[1];
    kVar2._kd[6] = in_stack_ffffff70[2];
    kVar2._kd[7] = in_stack_ffffff70[3];
    kVar2._kd[8] = in_stack_ffffff70[4];
    kVar2._kd[9] = in_stack_ffffff70[5];
    kVar2._kd[10] = in_stack_ffffff70[6];
    kVar2._kd[0xb] = in_stack_ffffff70[7];
    kVar2._kd[0xc] = in_stack_ffffff70[8];
    kVar2._kd[0xd] = in_stack_ffffff70[9];
    kVar2._kd[0xe] = in_stack_ffffff70[10];
    kVar2._kd[0xf] = in_stack_ffffff70[0xb];
    kVar2._kd[0x10] = in_stack_ffffff70[0xc];
    kVar2._kd[0x11] = in_stack_ffffff70[0xd];
    kVar2._kd[0x12] = in_stack_ffffff70[0xe];
    kVar2._kd[0x13] = in_stack_ffffff70[0xf];
    kVar2._kd[0x14] = in_stack_ffffff70[0x10];
    kVar2._kd[0x15] = in_stack_ffffff70[0x11];
    kVar2._kd[0x16] = in_stack_ffffff70[0x12];
    kVar2._kd[0x17] = in_stack_ffffff70[0x13];
    kVar2._kd[0x18] = in_stack_ffffff70[0x14];
    kVar2._kd[0x19] = in_stack_ffffff70[0x15];
    kVar2._kd[0x1a] = in_stack_ffffff70[0x16];
    kVar2._kd[0x1b] = in_stack_ffffff70[0x17];
    kVar2._kd[0x1c] = in_stack_ffffff70[0x18];
    kVar2._kd[0x1d] = in_stack_ffffff70[0x19];
    kVar2._kd[0x1e] = in_stack_ffffff70[0x1a];
    kVar2._kd[0x1f] = in_stack_ffffff70[0x1b];
    kVar2._kd[0x20] = in_stack_ffffff70[0x1c];
    kVar2._kd[0x21] = in_stack_ffffff70[0x1d];
    kVar2._kd[0x22] = in_stack_ffffff70[0x1e];
    kVar2._kd[0x23] = in_stack_ffffff70[0x1f];
    kVar2._kd[0x24] = in_stack_ffffff70[0x20];
    kVar2._kd[0x25] = in_stack_ffffff70[0x21];
    kVar2._kd[0x26] = in_stack_ffffff70[0x22];
    kVar2._kd[0x27] = in_stack_ffffff70[0x23];
    kVar2._kd[0x28] = in_stack_ffffff70[0x24];
    kVar2._kd[0x29] = in_stack_ffffff70[0x25];
    kVar2._kd[0x2a] = in_stack_ffffff70[0x26];
    kVar2._kd[0x2b] = in_stack_ffffff70[0x27];
    kVar2._kd[0x2c] = in_stack_ffffff70[0x28];
    kVar2._kd[0x2d] = in_stack_ffffff70[0x29];
    kVar2._kd[0x2e] = in_stack_ffffff70[0x2a];
    kVar2._kd[0x2f] = in_stack_ffffff70[0x2b];
    kVar2._kd[0x30] = in_stack_ffffff70[0x2c];
    kVar2._kd[0x31] = in_stack_ffffff70[0x2d];
    kVar2._kd[0x32] = in_stack_ffffff70[0x2e];
    kVar2._kd[0x33] = in_stack_ffffff70[0x2f];
    kVar2._kd._0_4_ = in_stack_ffffff6c;
    kVar2._kd[0x34] = (char)uVar18;
    kVar2._kd[0x35] = (char)((uint)uVar18 >> 8);
    kVar2._kd[0x36] = (char)((uint)uVar18 >> 0x10);
    kVar2._kd[0x37] = (char)((uint)uVar18 >> 0x18);
    kVar2._kd._56_4_ = pMVar19;
    kVar2._kd._60_4_ = MVar20;
    kVar2._kd._64_4_ = pMVar21;
    kVar2._kd[0x44] = (char)uVar22;
    kVar2._kd[0x45] = (char)((uint)uVar22 >> 8);
    kVar2._kd[0x46] = (char)((uint)uVar22 >> 0x10);
    kVar2._kd[0x47] = (char)((uint)uVar22 >> 0x18);
    kVar2._kd._72_4_ = pMVar23;
    MdtKeaAddConstraintForces(in_stack_ffffff04,ppMVar7,pvVar8,iVar10,kVar2);
    if (w->checkSim != 0) {
      uVar22 = 0x10606;
      pMVar23 = w;
      ppMVar24 = keabodyArray_00;
      CheckSim(w,(MdtKeaBody **)keabodyArray_00,po_00->totalBodies,keaCon_00);
    }
    pMVar17 = (MeReal *)&stack0xffffff70;
    for (iVar10 = 0x13; iVar10 != 0; iVar10 = iVar10 + -1) {
      *pMVar17 = pMVar14->stepsize;
      pMVar14 = (MdtKeaParameters *)&pMVar14->epsilon;
      pMVar17 = pMVar17 + 1;
    }
    kVar3._kd[0x30] = (char)uVar18;
    kVar3._kd[0x31] = (char)((uint)uVar18 >> 8);
    kVar3._kd[0x32] = (char)((uint)uVar18 >> 0x10);
    kVar3._kd[0x33] = (char)((uint)uVar18 >> 0x18);
    kVar3._kd[0] = in_stack_ffffff70[0];
    kVar3._kd[1] = in_stack_ffffff70[1];
    kVar3._kd[2] = in_stack_ffffff70[2];
    kVar3._kd[3] = in_stack_ffffff70[3];
    kVar3._kd[4] = in_stack_ffffff70[4];
    kVar3._kd[5] = in_stack_ffffff70[5];
    kVar3._kd[6] = in_stack_ffffff70[6];
    kVar3._kd[7] = in_stack_ffffff70[7];
    kVar3._kd[8] = in_stack_ffffff70[8];
    kVar3._kd[9] = in_stack_ffffff70[9];
    kVar3._kd[10] = in_stack_ffffff70[10];
    kVar3._kd[0xb] = in_stack_ffffff70[0xb];
    kVar3._kd[0xc] = in_stack_ffffff70[0xc];
    kVar3._kd[0xd] = in_stack_ffffff70[0xd];
    kVar3._kd[0xe] = in_stack_ffffff70[0xe];
    kVar3._kd[0xf] = in_stack_ffffff70[0xf];
    kVar3._kd[0x10] = in_stack_ffffff70[0x10];
    kVar3._kd[0x11] = in_stack_ffffff70[0x11];
    kVar3._kd[0x12] = in_stack_ffffff70[0x12];
    kVar3._kd[0x13] = in_stack_ffffff70[0x13];
    kVar3._kd[0x14] = in_stack_ffffff70[0x14];
    kVar3._kd[0x15] = in_stack_ffffff70[0x15];
    kVar3._kd[0x16] = in_stack_ffffff70[0x16];
    kVar3._kd[0x17] = in_stack_ffffff70[0x17];
    kVar3._kd[0x18] = in_stack_ffffff70[0x18];
    kVar3._kd[0x19] = in_stack_ffffff70[0x19];
    kVar3._kd[0x1a] = in_stack_ffffff70[0x1a];
    kVar3._kd[0x1b] = in_stack_ffffff70[0x1b];
    kVar3._kd[0x1c] = in_stack_ffffff70[0x1c];
    kVar3._kd[0x1d] = in_stack_ffffff70[0x1d];
    kVar3._kd[0x1e] = in_stack_ffffff70[0x1e];
    kVar3._kd[0x1f] = in_stack_ffffff70[0x1f];
    kVar3._kd[0x20] = in_stack_ffffff70[0x20];
    kVar3._kd[0x21] = in_stack_ffffff70[0x21];
    kVar3._kd[0x22] = in_stack_ffffff70[0x22];
    kVar3._kd[0x23] = in_stack_ffffff70[0x23];
    kVar3._kd[0x24] = in_stack_ffffff70[0x24];
    kVar3._kd[0x25] = in_stack_ffffff70[0x25];
    kVar3._kd[0x26] = in_stack_ffffff70[0x26];
    kVar3._kd[0x27] = in_stack_ffffff70[0x27];
    kVar3._kd[0x28] = in_stack_ffffff70[0x28];
    kVar3._kd[0x29] = in_stack_ffffff70[0x29];
    kVar3._kd[0x2a] = in_stack_ffffff70[0x2a];
    kVar3._kd[0x2b] = in_stack_ffffff70[0x2b];
    kVar3._kd[0x2c] = in_stack_ffffff70[0x2c];
    kVar3._kd[0x2d] = in_stack_ffffff70[0x2d];
    kVar3._kd[0x2e] = in_stack_ffffff70[0x2e];
    kVar3._kd[0x2f] = in_stack_ffffff70[0x2f];
    kVar3._kd._52_4_ = pMVar19;
    kVar3._kd._56_4_ = MVar20;
    kVar3._kd._60_4_ = pMVar21;
    kVar3._kd[0x40] = (char)uVar22;
    kVar3._kd[0x41] = (char)((uint)uVar22 >> 8);
    kVar3._kd[0x42] = (char)((uint)uVar22 >> 0x10);
    kVar3._kd[0x43] = (char)((uint)uVar22 >> 0x18);
    kVar3._kd._68_4_ = pMVar23;
    kVar3._kd._72_4_ = ppMVar24;
    MdtKeaIntegrateSystem(keabodyArray_00,pvVar5,po_00->totalBodies,kVar3);
    MeChunkPutMem(&w->keaPool,(w->keaParams).memory_pool);
    uVar6 = 0;
    if (0 < po_00->nPartitions) {
      do {
        uVar9 = uVar6 + 1;
        MdtUnpackBodies((void *)(po_00->bodiesStart[uVar6] * 0x40 + (int)pvVar5),uVar6,po_00);
        uVar6 = uVar9;
      } while ((int)uVar9 < po_00->nPartitions);
    }
    MeChunkPutMem(&w->keaTMChunk,pvVar5);
    pMVar15 = keaCon_00->force;
    uVar6 = 0;
    if (0 < po_00->nPartitions) {
      do {
                    /* Unresolved local var: int conCount@[DW_OP_reg0(EAX)] */
        uVar9 = MdtUnpackForces(pMVar15,uVar6,po_00);
        pMVar15 = pMVar15 + uVar9;
        o = (w->partitionParams).debugOptions;
        if ((o & (MdtDebugDrawContactForce|MdtDebugDrawContacts)) != 0) {
          DrawPartitionContacts(po_00,uVar6,o);
        }
        uVar6 = uVar6 + 1;
      } while ((int)uVar6 < po_00->nPartitions);
    }
    MeChunkPutMem(&w->keaConstraintsChunk,keaCon_00);
  }
  MeChunkPutMem(&w->partOutChunk,po_00);
  for (pvVar5 = MeDictFirst(pMVar4); pvVar5 != (void *)0x0; pvVar5 = MeDictNext(pMVar4,pvVar5)) {
    MdtBodyResetForces(*(void **)((int)pvVar5 + 0x14));
  }
  return;
}


/* ==== MdtWorldStepSafeTime ==== */

void MdtWorldStepSafeTime(MdtWorldID w,MeReal stepSize)

{
  MdtKeaBody **keabodyArray_00;
  kd_agg76 kVar1;
  kd_agg76 kVar2;
  void *pvVar3;
  MeDict *pMVar4;
  void *pvVar5;
  MdtKeaConstraints *keaCon_00;
  uint uVar6;
  MdtKeaBody **ppMVar7;
  void *pvVar8;
  int iVar9;
  int iVar10;
  MdtKeaParameters *pMVar11;
  MdtKeaConstraints *pMVar12;
  MdtKeaParameters *pMVar13;
  int *piVar14;
  MeReal *pMVar15;
  float fVar16;
  kd_agg92 in_stack_fffffef4;
  MeReal in_stack_ffffff5c;
  undefined1 in_stack_ffffff60 [48];
  undefined4 uVar17;
  void *pvVar18;
  int iVar19;
  MeReal MVar20;
  undefined4 uVar21;
  MdtWorldID pMVar22;
  MdtKeaBody **ppMVar23;
  int local_48;
  MdtKeaConstraints *keaCon;
  MdtKeaTransformation *keatmArray;
  MdtKeaBody **keabodyArray;
  MeReal timeStep;
  MdtPartitionOutput *po;
  int partitionindex;
  
                    /* Unresolved local var: int constraintRows@[DW_OP_reg0(EAX)]
                       Unresolved local var: MdtBody * b@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDictNode * node@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeDict * dict@[???] */
  pvVar3 = MdtPartOutCreateFromChunk(&w->partOutChunk,w->nBodies,w->nEnabledConstraints);
  pMVar4 = &w->enabledBodyDict;
  MdtUpdatePartitions(pMVar4,pvVar3,MdtAutoDisableLastPartition,&w->partitionParams);
  partitionindex = 0;
  if (0 < *(int *)((int)pvVar3 + 0x2c)) {
    pMVar11 = &w->keaParams;
    local_48 = 0;
    do {
                    /* Unresolved local var: int conCount@[???]
                       Unresolved local var: int partStart@[DW_OP_reg1(ECX)]
                       Unresolved local var: int sizeRequired@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal safeTime@[DW_OP_reg11(ST0)] */
      pvVar5 = MeChunkGetMem(&w->keaTMChunk,
                             *(int *)(*(int *)((int)pvVar3 + 0x1c) + partitionindex * 4) << 6);
      fVar16 = MdtPartitionGetSafeTime(pvVar3,partitionindex);
      if (fVar16 < w->minSafeTime) {
        fVar16 = w->minSafeTime;
      }
      timeStep = stepSize;
      if (fVar16 < stepSize) {
        timeStep = fVar16;
      }
      keaCon_00 = MdtKeaConstraintsCreateFromChunk
                            (&w->keaConstraintsChunk,1,
                             *(int *)(*(int *)((int)pvVar3 + 0x28) + 8 + local_48) +
                             *(int *)(*(int *)((int)pvVar3 + 0x28) + 4 + local_48),
                             *(int *)(*(int *)((int)pvVar3 + 0x28) + local_48));
      uVar17 = 0x10761;
      pvVar18 = pvVar3;
      iVar19 = partitionindex;
      MVar20 = timeStep;
      uVar6 = MdtPackPartition(pvVar3,partitionindex,timeStep,&w->params,pMVar11,pvVar5,keaCon_00);
      keabodyArray_00 =
           (MdtKeaBody **)
           (*(int *)((int)pvVar3 + 0x14) +
           *(int *)(*(int *)((int)pvVar3 + 0x18) + partitionindex * 4) * 4);
      (w->keaParams).gamma = (w->constantGamma / stepSize) * timeStep;
      (w->keaParams).stepsize = timeStep;
      ppMVar7 = (MdtKeaBody **)
                MdtKeaMemoryRequired
                          (keaCon_00->num_rows_exc_padding_partition,keaCon_00->num_partitions,uVar6
                           ,*(int *)(*(int *)((int)pvVar3 + 0x1c) + partitionindex * 4));
      ppMVar23 = ppMVar7;
      pvVar8 = MeChunkGetMem(&w->keaPool,(int)ppMVar7);
      (w->keaParams).memory_pool = pvVar8;
      (w->keaParams).memory_pool_size = (uint)ppMVar7;
      pMVar22 = (MdtWorldID)0x0;
      uVar21 = 0x107df;
      MdtFlushCache(0);
      pMVar13 = pMVar11;
      pMVar15 = (MeReal *)&stack0xffffff5c;
      for (iVar9 = 0x13; iVar9 != 0; iVar9 = iVar9 + -1) {
        *pMVar15 = pMVar13->stepsize;
        pMVar13 = (MdtKeaParameters *)&pMVar13->epsilon;
        pMVar15 = pMVar15 + 1;
      }
      iVar9 = *(int *)(*(int *)((int)pvVar3 + 0x1c) + partitionindex * 4);
      pMVar12 = keaCon_00;
      piVar14 = (int *)&stack0xfffffef4;
      ppMVar7 = keabodyArray_00;
      pvVar8 = pvVar5;
      for (iVar10 = 0x17; iVar10 != 0; iVar10 = iVar10 + -1) {
        *piVar14 = pMVar12->num_partitions;
        pMVar12 = (MdtKeaConstraints *)&pMVar12->max_partitions;
        piVar14 = piVar14 + 1;
      }
      kVar1._kd[4] = in_stack_ffffff60[0];
      kVar1._kd[5] = in_stack_ffffff60[1];
      kVar1._kd[6] = in_stack_ffffff60[2];
      kVar1._kd[7] = in_stack_ffffff60[3];
      kVar1._kd[8] = in_stack_ffffff60[4];
      kVar1._kd[9] = in_stack_ffffff60[5];
      kVar1._kd[10] = in_stack_ffffff60[6];
      kVar1._kd[0xb] = in_stack_ffffff60[7];
      kVar1._kd[0xc] = in_stack_ffffff60[8];
      kVar1._kd[0xd] = in_stack_ffffff60[9];
      kVar1._kd[0xe] = in_stack_ffffff60[10];
      kVar1._kd[0xf] = in_stack_ffffff60[0xb];
      kVar1._kd[0x10] = in_stack_ffffff60[0xc];
      kVar1._kd[0x11] = in_stack_ffffff60[0xd];
      kVar1._kd[0x12] = in_stack_ffffff60[0xe];
      kVar1._kd[0x13] = in_stack_ffffff60[0xf];
      kVar1._kd[0x14] = in_stack_ffffff60[0x10];
      kVar1._kd[0x15] = in_stack_ffffff60[0x11];
      kVar1._kd[0x16] = in_stack_ffffff60[0x12];
      kVar1._kd[0x17] = in_stack_ffffff60[0x13];
      kVar1._kd[0x18] = in_stack_ffffff60[0x14];
      kVar1._kd[0x19] = in_stack_ffffff60[0x15];
      kVar1._kd[0x1a] = in_stack_ffffff60[0x16];
      kVar1._kd[0x1b] = in_stack_ffffff60[0x17];
      kVar1._kd[0x1c] = in_stack_ffffff60[0x18];
      kVar1._kd[0x1d] = in_stack_ffffff60[0x19];
      kVar1._kd[0x1e] = in_stack_ffffff60[0x1a];
      kVar1._kd[0x1f] = in_stack_ffffff60[0x1b];
      kVar1._kd[0x20] = in_stack_ffffff60[0x1c];
      kVar1._kd[0x21] = in_stack_ffffff60[0x1d];
      kVar1._kd[0x22] = in_stack_ffffff60[0x1e];
      kVar1._kd[0x23] = in_stack_ffffff60[0x1f];
      kVar1._kd[0x24] = in_stack_ffffff60[0x20];
      kVar1._kd[0x25] = in_stack_ffffff60[0x21];
      kVar1._kd[0x26] = in_stack_ffffff60[0x22];
      kVar1._kd[0x27] = in_stack_ffffff60[0x23];
      kVar1._kd[0x28] = in_stack_ffffff60[0x24];
      kVar1._kd[0x29] = in_stack_ffffff60[0x25];
      kVar1._kd[0x2a] = in_stack_ffffff60[0x26];
      kVar1._kd[0x2b] = in_stack_ffffff60[0x27];
      kVar1._kd[0x2c] = in_stack_ffffff60[0x28];
      kVar1._kd[0x2d] = in_stack_ffffff60[0x29];
      kVar1._kd[0x2e] = in_stack_ffffff60[0x2a];
      kVar1._kd[0x2f] = in_stack_ffffff60[0x2b];
      kVar1._kd[0x30] = in_stack_ffffff60[0x2c];
      kVar1._kd[0x31] = in_stack_ffffff60[0x2d];
      kVar1._kd[0x32] = in_stack_ffffff60[0x2e];
      kVar1._kd[0x33] = in_stack_ffffff60[0x2f];
      kVar1._kd._0_4_ = in_stack_ffffff5c;
      kVar1._kd[0x34] = (char)uVar17;
      kVar1._kd[0x35] = (char)((uint)uVar17 >> 8);
      kVar1._kd[0x36] = (char)((uint)uVar17 >> 0x10);
      kVar1._kd[0x37] = (char)((uint)uVar17 >> 0x18);
      kVar1._kd._56_4_ = pvVar18;
      kVar1._kd[0x3c] = (char)iVar19;
      kVar1._kd[0x3d] = (char)((uint)iVar19 >> 8);
      kVar1._kd[0x3e] = (char)((uint)iVar19 >> 0x10);
      kVar1._kd[0x3f] = (char)((uint)iVar19 >> 0x18);
      kVar1._kd._64_4_ = MVar20;
      kVar1._kd[0x44] = (char)uVar21;
      kVar1._kd[0x45] = (char)((uint)uVar21 >> 8);
      kVar1._kd[0x46] = (char)((uint)uVar21 >> 0x10);
      kVar1._kd[0x47] = (char)((uint)uVar21 >> 0x18);
      kVar1._kd._72_4_ = pMVar22;
      MdtKeaAddConstraintForces(in_stack_fffffef4,ppMVar7,pvVar8,iVar9,kVar1);
      if (w->checkSim != 0) {
        uVar21 = 0x10941;
        pMVar22 = w;
        ppMVar23 = keabodyArray_00;
        CheckSim(w,keabodyArray_00,*(int *)(*(int *)((int)pvVar3 + 0x1c) + partitionindex * 4),
                 keaCon_00);
      }
      pMVar13 = pMVar11;
      pMVar15 = (MeReal *)&stack0xffffff60;
      for (iVar9 = 0x13; iVar9 != 0; iVar9 = iVar9 + -1) {
        *pMVar15 = pMVar13->stepsize;
        pMVar13 = (MdtKeaParameters *)&pMVar13->epsilon;
        pMVar15 = pMVar15 + 1;
      }
      in_stack_ffffff5c = *(MeReal *)(*(int *)((int)pvVar3 + 0x1c) + partitionindex * 4);
      kVar2._kd[0x30] = (char)uVar17;
      kVar2._kd[0x31] = (char)((uint)uVar17 >> 8);
      kVar2._kd[0x32] = (char)((uint)uVar17 >> 0x10);
      kVar2._kd[0x33] = (char)((uint)uVar17 >> 0x18);
      kVar2._kd[0] = in_stack_ffffff60[0];
      kVar2._kd[1] = in_stack_ffffff60[1];
      kVar2._kd[2] = in_stack_ffffff60[2];
      kVar2._kd[3] = in_stack_ffffff60[3];
      kVar2._kd[4] = in_stack_ffffff60[4];
      kVar2._kd[5] = in_stack_ffffff60[5];
      kVar2._kd[6] = in_stack_ffffff60[6];
      kVar2._kd[7] = in_stack_ffffff60[7];
      kVar2._kd[8] = in_stack_ffffff60[8];
      kVar2._kd[9] = in_stack_ffffff60[9];
      kVar2._kd[10] = in_stack_ffffff60[10];
      kVar2._kd[0xb] = in_stack_ffffff60[0xb];
      kVar2._kd[0xc] = in_stack_ffffff60[0xc];
      kVar2._kd[0xd] = in_stack_ffffff60[0xd];
      kVar2._kd[0xe] = in_stack_ffffff60[0xe];
      kVar2._kd[0xf] = in_stack_ffffff60[0xf];
      kVar2._kd[0x10] = in_stack_ffffff60[0x10];
      kVar2._kd[0x11] = in_stack_ffffff60[0x11];
      kVar2._kd[0x12] = in_stack_ffffff60[0x12];
      kVar2._kd[0x13] = in_stack_ffffff60[0x13];
      kVar2._kd[0x14] = in_stack_ffffff60[0x14];
      kVar2._kd[0x15] = in_stack_ffffff60[0x15];
      kVar2._kd[0x16] = in_stack_ffffff60[0x16];
      kVar2._kd[0x17] = in_stack_ffffff60[0x17];
      kVar2._kd[0x18] = in_stack_ffffff60[0x18];
      kVar2._kd[0x19] = in_stack_ffffff60[0x19];
      kVar2._kd[0x1a] = in_stack_ffffff60[0x1a];
      kVar2._kd[0x1b] = in_stack_ffffff60[0x1b];
      kVar2._kd[0x1c] = in_stack_ffffff60[0x1c];
      kVar2._kd[0x1d] = in_stack_ffffff60[0x1d];
      kVar2._kd[0x1e] = in_stack_ffffff60[0x1e];
      kVar2._kd[0x1f] = in_stack_ffffff60[0x1f];
      kVar2._kd[0x20] = in_stack_ffffff60[0x20];
      kVar2._kd[0x21] = in_stack_ffffff60[0x21];
      kVar2._kd[0x22] = in_stack_ffffff60[0x22];
      kVar2._kd[0x23] = in_stack_ffffff60[0x23];
      kVar2._kd[0x24] = in_stack_ffffff60[0x24];
      kVar2._kd[0x25] = in_stack_ffffff60[0x25];
      kVar2._kd[0x26] = in_stack_ffffff60[0x26];
      kVar2._kd[0x27] = in_stack_ffffff60[0x27];
      kVar2._kd[0x28] = in_stack_ffffff60[0x28];
      kVar2._kd[0x29] = in_stack_ffffff60[0x29];
      kVar2._kd[0x2a] = in_stack_ffffff60[0x2a];
      kVar2._kd[0x2b] = in_stack_ffffff60[0x2b];
      kVar2._kd[0x2c] = in_stack_ffffff60[0x2c];
      kVar2._kd[0x2d] = in_stack_ffffff60[0x2d];
      kVar2._kd[0x2e] = in_stack_ffffff60[0x2e];
      kVar2._kd[0x2f] = in_stack_ffffff60[0x2f];
      kVar2._kd._52_4_ = pvVar18;
      kVar2._kd[0x38] = (char)iVar19;
      kVar2._kd[0x39] = (char)((uint)iVar19 >> 8);
      kVar2._kd[0x3a] = (char)((uint)iVar19 >> 0x10);
      kVar2._kd[0x3b] = (char)((uint)iVar19 >> 0x18);
      kVar2._kd._60_4_ = MVar20;
      kVar2._kd[0x40] = (char)uVar21;
      kVar2._kd[0x41] = (char)((uint)uVar21 >> 8);
      kVar2._kd[0x42] = (char)((uint)uVar21 >> 0x10);
      kVar2._kd[0x43] = (char)((uint)uVar21 >> 0x18);
      kVar2._kd._68_4_ = pMVar22;
      kVar2._kd._72_4_ = ppMVar23;
      MdtKeaIntegrateSystem(keabodyArray_00,pvVar5,(int)in_stack_ffffff5c,kVar2);
      MeChunkPutMem(&w->keaPool,(w->keaParams).memory_pool);
      MdtUnpackBodies(pvVar5,partitionindex,pvVar3);
      MdtUnpackForces(keaCon_00->force,partitionindex,pvVar3);
      MeChunkPutMem(&w->keaConstraintsChunk,keaCon_00);
      MeChunkPutMem(&w->keaTMChunk,pvVar5);
      partitionindex = partitionindex + 1;
      local_48 = local_48 + 0xc;
    } while (partitionindex < *(int *)((int)pvVar3 + 0x2c));
  }
  MeChunkPutMem(&w->partOutChunk,pvVar3);
  for (pvVar3 = MeDictFirst(pMVar4); pvVar3 != (void *)0x0; pvVar3 = MeDictNext(pMVar4,pvVar3)) {
    pvVar5 = *(void **)((int)pvVar3 + 0x14);
    MdtBodyResetForces(pvVar5);
    *(undefined4 *)((int)pvVar5 + 0x238) = 0x7f7fffff;
  }
  return;
}


/* ==== MdtWorldDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MdtWorldDestroy(MdtWorldID w)

{
  MeChunkTerm(&w->keaTMChunk);
  (*_MdtDictCompare)(&w->bodyPool);
  MeChunkTerm(&w->partOutChunk);
  (*_MdtDictCompare)(&w->constraintPool);
  MeChunkTerm(&w->keaConstraintsChunk);
  MeChunkTerm(&w->keaPool);
  (*_MePoolAPI)(w);
  return;
}


/* ==== MdtWorldGetMaxBodies ==== */

int MdtWorldGetMaxBodies(MdtWorldID w)

{
  return w->maxBodies;
}


/* ==== MdtWorldGetMaxConstraints ==== */

int MdtWorldGetMaxConstraints(MdtWorldID w)

{
  return w->maxConstraints;
}


/* ==== MdtWorldGetMaxMemoryPoolUsed ==== */

int MdtWorldGetMaxMemoryPoolUsed(MdtWorldID w)

{
  int iVar1;
  
  iVar1 = MeChunkGetMaxSize(&w->keaPool);
  return iVar1;
}


/* ==== MdtWorldGetKeaPoolChunk ==== */

MeChunk * MdtWorldGetKeaPoolChunk(MdtWorldID w)

{
  return &w->keaPool;
}


/* ==== MdtWorldGetKeaTMChunk ==== */

MeChunk * MdtWorldGetKeaTMChunk(MdtWorldID w)

{
  return &w->keaTMChunk;
}


/* ==== MdtWorldGetKeaConstraintsChunk ==== */

MeChunk * MdtWorldGetKeaConstraintsChunk(MdtWorldID w)

{
  return &w->keaConstraintsChunk;
}


/* ==== MdtWorldGetPartitionOutputChunk ==== */

MeChunk * MdtWorldGetPartitionOutputChunk(MdtWorldID w)

{
  return &w->partOutChunk;
}


/* ==== MdtWorldGetGravity ==== */

void MdtWorldGetGravity(MdtWorldID w,MeReal *g)

{
  *g = (w->params).gravity[0];
  g[1] = (w->params).gravity[1];
  g[2] = (w->params).gravity[2];
  return;
}


/* ==== MdtWorldGetEpsilon ==== */

MeReal MdtWorldGetEpsilon(MdtWorldID w)

{
  return (w->keaParams).epsilon;
}


/* ==== MdtWorldGetGamma ==== */

MeReal MdtWorldGetGamma(MdtWorldID w)

{
  return w->constantGamma;
}


/* ==== MdtWorldGetMinSafeTime ==== */

MeReal MdtWorldGetMinSafeTime(MdtWorldID w)

{
  return w->minSafeTime;
}


/* ==== MdtWorldGetTotalBodies ==== */

int MdtWorldGetTotalBodies(MdtWorldID w)

{
  return w->nBodies;
}


/* ==== MdtWorldGetEnabledBodies ==== */

int MdtWorldGetEnabledBodies(MdtWorldID w)

{
  return w->nEnabledBodies;
}


/* ==== MdtWorldGetTotalConstraints ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int MdtWorldGetTotalConstraints(MdtWorldID w)

{
  int iVar1;
  
  iVar1 = (*_MdtDefaultSimErrorCallBack)(&w->constraintPool);
  return iVar1;
}


/* ==== MdtWorldGetEnabledConstraints ==== */

int MdtWorldGetEnabledConstraints(MdtWorldID w)

{
  return w->nEnabledConstraints;
}


/* ==== MdtWorldGetAutoDisable ==== */

MeBool MdtWorldGetAutoDisable(MdtWorldID w)

{
  return (w->partitionParams).autoDisable;
}


/* ==== MdtWorldGetAutoDisableVelocityThreshold ==== */

MeReal MdtWorldGetAutoDisableVelocityThreshold(MdtWorldID w)

{
  return (w->partitionParams).vel_thresh;
}


/* ==== MdtWorldGetAutoDisableAngularVelocityThreshold ==== */

MeReal MdtWorldGetAutoDisableAngularVelocityThreshold(MdtWorldID w)

{
  return (w->partitionParams).velrot_thresh;
}


/* ==== MdtWorldGetAutoDisableAccelerationThreshold ==== */

MeReal MdtWorldGetAutoDisableAccelerationThreshold(MdtWorldID w)

{
  return (w->partitionParams).acc_thresh;
}


/* ==== MdtWorldGetAutoDisableAngularAccelerationThreshold ==== */

MeReal MdtWorldGetAutoDisableAngularAccelerationThreshold(MdtWorldID w)

{
  return (w->partitionParams).accrot_thresh;
}


/* ==== MdtWorldGetAutoDisableAliveTime ==== */

MeReal MdtWorldGetAutoDisableAliveTime(MdtWorldID w)

{
  return (w->partitionParams).alive_time_thresh;
}


/* ==== MdtWorldGetMaxMatrixSize ==== */

int MdtWorldGetMaxMatrixSize(MdtWorldID w)

{
  return (w->partitionParams).maxMatrixSize;
}


/* ==== MdtWorldGetMaxLCPIterations ==== */

int MdtWorldGetMaxLCPIterations(MdtWorldID w)

{
  return (w->keaParams).max_iterations;
}


/* ==== MdtWorldGetLODParams ==== */

void MdtWorldGetLODParams(MdtWorldID w,MdtLODParams *lodParams)

{
  lodParams->frictionRatio = (w->partitionParams).lodParams.frictionRatio;
  lodParams->zeroRowBonus = (w->partitionParams).lodParams.zeroRowBonus;
  lodParams->toWorldBonus = (w->partitionParams).lodParams.toWorldBonus;
  lodParams->rowCountBias = (w->partitionParams).lodParams.rowCountBias;
  lodParams->penetrationBias = (w->partitionParams).lodParams.penetrationBias;
  lodParams->normVelBias = (w->partitionParams).lodParams.normVelBias;
  lodParams->nonAutoBonus = (w->partitionParams).lodParams.nonAutoBonus;
  return;
}


/* ==== MdtWorldGetCheckSim ==== */

MeBool MdtWorldGetCheckSim(MdtWorldID w)

{
  return w->checkSim;
}


/* ==== MdtWorldGetSimErrorCB ==== */

MdtSimErrorCBPtr MdtWorldGetSimErrorCB(MdtWorldID w)

{
  return w->simErrorCallback;
}


/* ==== MdtWorldGetDefaultDensity ==== */

MeReal MdtWorldGetDefaultDensity(MdtWorldID w)

{
  return (w->params).defaultDensity;
}


/* ==== MdtWorldSetEpsilon ==== */

void MdtWorldSetEpsilon(MdtWorldID w,MeReal e)

{
  (w->keaParams).epsilon = e;
  return;
}


/* ==== MdtWorldSetGamma ==== */

void MdtWorldSetGamma(MdtWorldID w,MeReal g)

{
  (w->keaParams).gamma = g;
  w->constantGamma = g;
  return;
}


/* ==== MdtWorldSetGammaWithRefTimeStep ==== */

void MdtWorldSetGammaWithRefTimeStep(MdtWorldID w,MeReal aGamma,MeReal aRefStep,MeReal aTimeStep)

{
  float fVar1;
  
  fVar1 = (aTimeStep * aGamma) / aRefStep;
  (w->keaParams).gamma = fVar1;
  w->constantGamma = fVar1;
  return;
}


/* ==== MdtWorldSetMinSafeTime ==== */

void MdtWorldSetMinSafeTime(MdtWorldID w,MeReal t)

{
  w->minSafeTime = t;
  return;
}


/* ==== MdtWorldSetAutoDisable ==== */

void MdtWorldSetAutoDisable(MdtWorldID w,MeBool d)

{
  (w->partitionParams).autoDisable = d;
  return;
}


/* ==== MdtWorldSetAutoDisableVelocityThreshold ==== */

void MdtWorldSetAutoDisableVelocityThreshold(MdtWorldID w,MeReal vt)

{
  (w->partitionParams).vel_thresh = vt;
  return;
}


/* ==== MdtWorldSetAutoDisableAngularVelocityThreshold ==== */

void MdtWorldSetAutoDisableAngularVelocityThreshold(MdtWorldID w,MeReal avt)

{
  (w->partitionParams).velrot_thresh = avt;
  return;
}


/* ==== MdtWorldSetAutoDisableAccelerationThreshold ==== */

void MdtWorldSetAutoDisableAccelerationThreshold(MdtWorldID w,MeReal at)

{
  (w->partitionParams).acc_thresh = at;
  return;
}


/* ==== MdtWorldSetAutoDisableAngularAccelerationThreshold ==== */

void MdtWorldSetAutoDisableAngularAccelerationThreshold(MdtWorldID w,MeReal aat)

{
  (w->partitionParams).accrot_thresh = aat;
  return;
}


/* ==== MdtWorldSetAutoDisableAliveTime ==== */

void MdtWorldSetAutoDisableAliveTime(MdtWorldID w,MeReal aw)

{
  (w->partitionParams).alive_time_thresh = aw;
  return;
}


/* ==== MdtWorldSetMaxMatrixSize ==== */

void MdtWorldSetMaxMatrixSize(MdtWorldID w,int size)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  bool bVar4;
  
  if ((size & 3U) == 0) {
    iVar1 = size + -3;
    bVar4 = iVar1 == 0;
    iVar3 = size;
  }
  else {
    uVar2 = size;
    if (size < 0) {
      uVar2 = size + 3;
    }
    iVar3 = (uVar2 & 0xfffffffc) + 4;
    iVar1 = (uVar2 & 0xfffffffc) + 1;
    bVar4 = iVar3 == 3;
  }
  if (bVar4 || SBORROW4(iVar3,3) != iVar1 < 0) {
    size = 4;
  }
  else if ((size & 3U) != 0) {
    if (size < 0) {
      size = size + 3;
    }
    size = (size & 0xfffffffcU) + 4;
  }
  (w->partitionParams).maxMatrixSize = size;
  return;
}


/* ==== MdtWorldSetGravity ==== */

void MdtWorldSetGravity(MdtWorldID w,MeReal gx,MeReal gy,MeReal gz)

{
  (w->params).gravity[0] = gx;
  (w->params).gravity[1] = gy;
  (w->params).gravity[2] = gz;
  return;
}


/* ==== MdtWorldSetDebugDrawing ==== */

void MdtWorldSetDebugDrawing(MdtWorldID w,MdtDebugDrawOptions drawOptions)

{
  (w->partitionParams).debugOptions = drawOptions;
  return;
}


/* ==== MdtWorldSetKeaDebugRequest ==== */

void MdtWorldSetKeaDebugRequest(MdtWorldID w,MdtKeaDebugDataRequest debugDataRequest)

{
  int iVar1;
  MdtKeaDebugDataRequest *pMVar2;
  MdtKeaDebugDataRequest *pMVar3;
  
  pMVar2 = &debugDataRequest;
  pMVar3 = &(w->keaParams).debug;
  for (iVar1 = 0xb; iVar1 != 0; iVar1 = iVar1 + -1) {
    pMVar3->writeKeaInputData = pMVar2->writeKeaInputData;
    pMVar2 = (MdtKeaDebugDataRequest *)&pMVar2->writeKeaInputDataFilename;
    pMVar3 = (MdtKeaDebugDataRequest *)&pMVar3->writeKeaInputDataFilename;
  }
  return;
}


/* ==== MdtWorldResetForces ==== */

void MdtWorldResetForces(MdtWorldID w)

{
  void *pvVar1;
  
                    /* Unresolved local var: MeDict * dict@[???]
                       Unresolved local var: MeDictNode * node@[DW_OP_reg3(EBX)] */
  for (pvVar1 = MeDictFirst(w); pvVar1 != (void *)0x0; pvVar1 = MeDictNext(w,pvVar1)) {
    MdtBodyResetForces(*(void **)((int)pvVar1 + 0x14));
  }
  return;
}


/* ==== MdtWorldSetMatrixSizeLog ==== */

void MdtWorldSetMatrixSizeLog(MdtWorldID w,int *sizeLog,int logSize)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)] */
  (w->params).matrixSizeLog = sizeLog;
  iVar1 = 0;
  (w->params).matrixSizeLogSize = logSize;
  if (0 < logSize) {
    do {
      (w->params).matrixSizeLog[iVar1] = 0;
      iVar1 = iVar1 + 1;
    } while (iVar1 < (w->params).matrixSizeLogSize);
  }
  return;
}


/* ==== MdtWorldSetMaxLCPIterations ==== */

void MdtWorldSetMaxLCPIterations(MdtWorldID w,int mi)

{
  (w->keaParams).max_iterations = mi;
  return;
}


/* ==== MdtWorldSetLODParams ==== */

void MdtWorldSetLODParams(MdtWorldID w,MdtLODParams *lodParams)

{
  (w->partitionParams).lodParams.frictionRatio = lodParams->frictionRatio;
  (w->partitionParams).lodParams.zeroRowBonus = lodParams->zeroRowBonus;
  (w->partitionParams).lodParams.toWorldBonus = lodParams->toWorldBonus;
  (w->partitionParams).lodParams.rowCountBias = lodParams->rowCountBias;
  (w->partitionParams).lodParams.penetrationBias = lodParams->penetrationBias;
  (w->partitionParams).lodParams.normVelBias = lodParams->normVelBias;
  (w->partitionParams).lodParams.nonAutoBonus = lodParams->nonAutoBonus;
  return;
}


/* ==== MdtWorldSetSimErrorCB ==== */

void MdtWorldSetSimErrorCB(MdtWorldID w,MdtSimErrorCBPtr cb,void *secbdata)

{
  w->simErrorCallback = cb;
  w->simErrorUserData = secbdata;
  return;
}


/* ==== MdtWorldSetCheckSim ==== */

void MdtWorldSetCheckSim(MdtWorldID w,MeBool c)

{
  w->checkSim = c;
  return;
}


/* ==== MdtWorldForAllConstraints ==== */

void MdtWorldForAllConstraints(MdtWorldID w,MdtConstraintIteratorCBPtr cb,void *ccbdata)

{
  void *pvVar1;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDict * dict@[DW_OP_reg6(ESI)] */
  for (pvVar1 = MeDictFirst(&w->constraintDict); pvVar1 != (void *)0x0;
      pvVar1 = MeDictNext(&w->constraintDict,pvVar1)) {
    (*cb)(*(MdtConstraintID *)((int)pvVar1 + 0x14),ccbdata);
  }
  return;
}


/* ==== DrawPartitionContacts ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void DrawPartitionContacts(MdtPartitionOutput *po,int pid,MdtDebugDrawOptions o)

{
  void *pvVar1;
  void *pvVar2;
  int iVar3;
  float fVar4;
  uint uVar5;
  MeReal *pMVar6;
  MeVector3 end;
  MeVector3 end_1;
  MeVector3 start;
  
                    /* Unresolved local var: int c@[DW_OP_reg7(EDI)] */
  iVar3 = po->constraintsStart[pid];
  if (iVar3 < po->constraintsSize[pid] + iVar3) {
    do {
                    /* Unresolved local var: MdtConstraintID constraint@[???]
                       Unresolved local var: MdtContactGroupID.conflict group@[DW_OP_reg0(EAX)] */
      pvVar1 = MdtConstraintDCastContactGroup(po->constraints[iVar3]);
                    /* Unresolved local var: MdtContactID.conflict contact@[DW_OP_reg3(EBX)] */
      if ((pvVar1 != (void *)0x0) &&
         (pvVar1 = *(void **)((int)pvVar1 + 0x164), pvVar1 != (void *)0x0)) {
        do {
          MdtContactGetPosition(pvVar1,start);
          if ((o & MdtDebugDrawContacts) != 0) {
            MdtContactGetNormal(pvVar1,end_1);
            fVar4 = MdtContactGetPenetration(pvVar1);
            fVar4 = fVar4 * 50.0 + 1.0;
            end_1[1] = end_1[1] * fVar4 + start[1];
            end_1[0] = end_1[0] * fVar4 + start[0];
            end_1[2] = fVar4 * end_1[2] + start[2];
            (*_MeDebugDrawAPI)(start,end_1,1.0,0.0,0.0);
          }
          if ((o & MdtDebugDrawContactForce) != 0) {
            pMVar6 = end;
            uVar5 = 0;
            pvVar2 = MdtContactQuaConstraint(pvVar1);
            MdtConstraintGetForce(pvVar2,uVar5,pMVar6);
            end[1] = end[1] + start[1];
            end[0] = start[0] + end[0];
            end[2] = end[2] + start[2];
            (*_MeDebugDrawAPI)(start,end,0.0,1.0,0.0);
          }
          pvVar1 = *(void **)((int)pvVar1 + 0x1dc);
        } while (pvVar1 != (void *)0x0);
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < po->constraintsSize[pid] + po->constraintsStart[pid]);
  }
  return;
}


/* ==== CheckSim ==== */

void CheckSim(MdtWorldID w,MdtKeaBody **keabodyArray,int nBodies,MdtKeaConstraints *keaCon)

{
  MdtKeaBody *pMVar1;
  bool bVar2;
  MdtKeaForcePair *pMVar3;
  int iVar4;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeBool isError@[DW_OP_reg3(EBX)] */
  iVar4 = 0;
  bVar2 = false;
  if (0 < nBodies) {
    do {
                    /* Unresolved local var: MdtKeaBody * b@[DW_OP_reg2(EDX)] */
      pMVar1 = keabodyArray[iVar4];
      if (((uint)pMVar1->accel[0] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)pMVar1->accelrot[0] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)pMVar1->force[0] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)pMVar1->torque[0] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)pMVar1->accel[1] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)pMVar1->accelrot[1] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)pMVar1->force[1] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)pMVar1->torque[1] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)pMVar1->accel[2] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)pMVar1->accelrot[2] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)pMVar1->force[2] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)pMVar1->torque[2] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      iVar4 = iVar4 + 1;
    } while ((iVar4 < nBodies) && (!bVar2));
  }
  if (keaCon->num_constraints < 1) {
LAB_000113b2:
    if (!bVar2) {
      return;
    }
  }
  else if (!bVar2) {
    iVar4 = 0;
    do {
                    /* Unresolved local var: MdtKeaForcePair * fp@[DW_OP_reg2(EDX)] */
      pMVar3 = keaCon->force + iVar4;
      if (((uint)(pMVar3->primary_body).force[0] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)(pMVar3->primary_body).torque[0] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)(pMVar3->secondary_body).force[0] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)(pMVar3->secondary_body).torque[0] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)(pMVar3->primary_body).force[1] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)(pMVar3->primary_body).torque[1] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)(pMVar3->secondary_body).force[1] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)(pMVar3->secondary_body).torque[1] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)(pMVar3->primary_body).force[2] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)(pMVar3->primary_body).torque[2] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)(pMVar3->secondary_body).force[2] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      if (((uint)(pMVar3->secondary_body).torque[2] & 0x7f800000) == 0x7f800000) {
        bVar2 = true;
      }
      iVar4 = iVar4 + 1;
      if (keaCon->num_constraints <= iVar4) goto LAB_000113b2;
    } while (!bVar2);
  }
  if (w->simErrorCallback != (MdtSimErrorCBPtr)0x0) {
    (*w->simErrorCallback)(keaCon,keabodyArray,nBodies,w->simErrorUserData);
  }
  return;
}


