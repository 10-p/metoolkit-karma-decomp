/* ==== MdtWorldCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MdtWorldID MdtWorldCreate(uint maxBodies,uint maxConstraints,MeReal lengthScale,MeReal massScale)

{
  MdtWorldID pMVar1;
  MdtWorldID w;
  MeCPUResources p_Var2;
  char *pcVar3;
  undefined4 extraout_ECX;
  MeChunk *pMVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  
                    /* Unresolved local var: MdtWorld * w@[DW_OP_reg6(ESI)]
                       Unresolved local var: int largestConstraintSize@[???] */
  pMVar1 = (MdtWorldID)0x0;
  if (maxBodies != 0) {
    w = (MdtWorldID)(*_MeMemoryAPI)(0x234);
    pMVar1 = (MdtWorldID)0x0;
    if (w != (MdtWorldID)0x0) {
      (w->params).lengthScale = lengthScale;
      (w->params).massScale = massScale;
      (w->params).defaultDensity = massScale / (lengthScale * lengthScale * lengthScale);
      p_Var2 = MdtKeaQueryCPUResources();
      (w->keaParams).cpu_resources = p_Var2;
      uVar6 = extraout_ECX;
      uVar7 = extraout_ECX;
      MeChunkInit(&w->keaPool,0x10);
      w->maxBodies = maxBodies;
      w->nBodies = 0;
      w->nEnabledBodies = 0;
      pMVar4 = &w->keaTMChunk;
      uVar5 = 0x10;
      MeChunkInit(pMVar4,0x10);
      (*_MePoolAPI)(&w->bodyPool,w->maxBodies,0x240,0x10,pMVar4,uVar5,uVar6,uVar7);
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
  MdtPartitionOutput *po_00;
  MeDict *pMVar2;
  void *pvVar3;
  MdtKeaConstraints *keaCon_00;
  uint uVar4;
  void *pvVar5;
  uint uVar6;
  int iVar7;
  MdtKeaParameters *pMVar8;
  MdtKeaConstraints *pMVar9;
  MdtKeaParameters *pMVar10;
  MdtKeaForcePair *pMVar11;
  int *piVar12;
  MeReal *pMVar13;
  int aiStackY_fc [23];
  MdtBody **ppMStackY_a0;
  MdtBody **ppMStackY_9c;
  void *pvStackY_98;
  MeReal aMStackY_94 [13];
  undefined4 uStackY_60;
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
  pMVar2 = &w->enabledBodyDict;
  uStackY_60 = 0x10381;
  MdtUpdatePartitions(pMVar2,po_00,MdtAutoDisableLastPartition,&w->partitionParams);
  if (0 < po_00->nPartitions) {
    pvVar3 = MeChunkGetMem(&w->keaTMChunk,po_00->totalBodies << 6);
    uStackY_60 = 0x103d5;
    keaCon_00 = MdtKeaConstraintsCreateFromChunk
                          (&w->keaConstraintsChunk,po_00->nPartitions,
                           (po_00->overallInfo).jointCount + (po_00->overallInfo).contactCount,
                           (po_00->overallInfo).rowCount);
    pMVar10 = &w->keaParams;
    uStackY_60 = 0x10401;
    uVar4 = MdtPackAllPartitions(po_00,stepSize,&w->params,pMVar10,pvVar3,keaCon_00);
    keabodyArray_00 = po_00->bodies;
    MVar1 = w->constantGamma;
    (w->keaParams).stepsize = stepSize;
    (w->keaParams).gamma = MVar1;
    uVar4 = MdtKeaMemoryRequired
                      (keaCon_00->num_rows_exc_padding_partition,keaCon_00->num_partitions,uVar4,
                       po_00->totalBodies);
    pvVar5 = MeChunkGetMem(&w->keaPool,uVar4);
    (w->keaParams).memory_pool = pvVar5;
    (w->keaParams).memory_pool_size = uVar4;
    MdtFlushCache(0);
    pMVar8 = pMVar10;
    pMVar13 = aMStackY_94;
    for (iVar7 = 0x13; iVar7 != 0; iVar7 = iVar7 + -1) {
      *pMVar13 = pMVar8->stepsize;
      pMVar8 = (MdtKeaParameters *)&pMVar8->epsilon;
      pMVar13 = pMVar13 + 1;
    }
    pvStackY_98 = (void *)po_00->totalBodies;
    pMVar9 = keaCon_00;
    piVar12 = aiStackY_fc;
    ppMStackY_a0 = keabodyArray_00;
    ppMStackY_9c = pvVar3;
    for (iVar7 = 0x17; iVar7 != 0; iVar7 = iVar7 + -1) {
      *piVar12 = pMVar9->num_partitions;
      pMVar9 = (MdtKeaConstraints *)&pMVar9->max_partitions;
      piVar12 = piVar12 + 1;
    }
    MdtKeaAddConstraintForces();
    if (w->checkSim != 0) {
      CheckSim(w,(MdtKeaBody **)keabodyArray_00,po_00->totalBodies,keaCon_00);
    }
    pMVar13 = aMStackY_94 + 1;
    for (iVar7 = 0x13; iVar7 != 0; iVar7 = iVar7 + -1) {
      *pMVar13 = pMVar10->stepsize;
      pMVar10 = (MdtKeaParameters *)&pMVar10->epsilon;
      pMVar13 = pMVar13 + 1;
    }
    aMStackY_94[0] = (MeReal)po_00->totalBodies;
    ppMStackY_a0 = (MdtBody **)0x104d8;
    ppMStackY_9c = keabodyArray_00;
    pvStackY_98 = pvVar3;
    MdtKeaIntegrateSystem();
    MeChunkPutMem(&w->keaPool,(w->keaParams).memory_pool);
    uVar4 = 0;
    if (0 < po_00->nPartitions) {
      do {
        uVar6 = uVar4 + 1;
        MdtUnpackBodies((void *)(po_00->bodiesStart[uVar4] * 0x40 + (int)pvVar3),uVar4,po_00);
        uVar4 = uVar6;
      } while ((int)uVar6 < po_00->nPartitions);
    }
    MeChunkPutMem(&w->keaTMChunk,pvVar3);
    pMVar11 = keaCon_00->force;
    uVar4 = 0;
    if (0 < po_00->nPartitions) {
      do {
                    /* Unresolved local var: int conCount@[DW_OP_reg0(EAX)] */
        uVar6 = MdtUnpackForces(pMVar11,uVar4,po_00);
        pMVar11 = pMVar11 + uVar6;
        o = (w->partitionParams).debugOptions;
        if ((o & (MdtDebugDrawContactForce|MdtDebugDrawContacts)) != 0) {
          DrawPartitionContacts(po_00,uVar4,o);
        }
        uVar4 = uVar4 + 1;
      } while ((int)uVar4 < po_00->nPartitions);
    }
    MeChunkPutMem(&w->keaConstraintsChunk,keaCon_00);
  }
  MeChunkPutMem(&w->partOutChunk,po_00);
  for (pvVar3 = MeDictFirst(pMVar2); pvVar3 != (void *)0x0; pvVar3 = MeDictNext(pMVar2,pvVar3)) {
    MdtBodyResetForces(*(void **)((int)pvVar3 + 0x14));
  }
  return;
}


/* ==== MdtWorldStepSafeTime ==== */

void MdtWorldStepSafeTime(MdtWorldID w,MeReal stepSize)

{
  MdtKeaBody **keabodyArray_00;
  void *pvVar1;
  MeDict *pMVar2;
  void *pvVar3;
  MdtKeaConstraints *keaCon_00;
  uint uVar4;
  void *pvVar5;
  int iVar6;
  MdtKeaParameters *pMVar7;
  MdtKeaConstraints *pMVar8;
  MdtKeaParameters *pMVar9;
  int *piVar10;
  MeReal *pMVar11;
  float fVar12;
  int aiStackY_10c [23];
  MdtKeaBody **ppMStackY_b0;
  MdtKeaBody **ppMStackY_ac;
  void *pvStackY_a8;
  MeReal aMStackY_a4 [13];
  undefined4 uStackY_70;
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
  pvVar1 = MdtPartOutCreateFromChunk(&w->partOutChunk,w->nBodies,w->nEnabledConstraints);
  pMVar2 = &w->enabledBodyDict;
  uStackY_70 = 0x1065e;
  MdtUpdatePartitions(pMVar2,pvVar1,MdtAutoDisableLastPartition,&w->partitionParams);
  partitionindex = 0;
  if (0 < *(int *)((int)pvVar1 + 0x2c)) {
    pMVar7 = &w->keaParams;
    local_48 = 0;
    do {
                    /* Unresolved local var: int conCount@[???]
                       Unresolved local var: int partStart@[DW_OP_reg1(ECX)]
                       Unresolved local var: int sizeRequired@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal safeTime@[DW_OP_reg11(ST0)] */
      pvVar3 = MeChunkGetMem(&w->keaTMChunk,
                             *(int *)(*(int *)((int)pvVar1 + 0x1c) + partitionindex * 4) << 6);
      fVar12 = MdtPartitionGetSafeTime(pvVar1,partitionindex);
      if (fVar12 < w->minSafeTime) {
        fVar12 = w->minSafeTime;
      }
      timeStep = stepSize;
      if (fVar12 < stepSize) {
        timeStep = fVar12;
      }
      keaCon_00 = MdtKeaConstraintsCreateFromChunk
                            (&w->keaConstraintsChunk,1,
                             *(int *)(*(int *)((int)pvVar1 + 0x28) + 8 + local_48) +
                             *(int *)(*(int *)((int)pvVar1 + 0x28) + 4 + local_48),
                             *(int *)(*(int *)((int)pvVar1 + 0x28) + local_48));
      uStackY_70 = 0x10761;
      uVar4 = MdtPackPartition(pvVar1,partitionindex,timeStep,&w->params,pMVar7,pvVar3,keaCon_00);
      keabodyArray_00 =
           (MdtKeaBody **)
           (*(int *)((int)pvVar1 + 0x14) +
           *(int *)(*(int *)((int)pvVar1 + 0x18) + partitionindex * 4) * 4);
      (w->keaParams).gamma = (w->constantGamma / stepSize) * timeStep;
      (w->keaParams).stepsize = timeStep;
      uVar4 = MdtKeaMemoryRequired
                        (keaCon_00->num_rows_exc_padding_partition,keaCon_00->num_partitions,uVar4,
                         *(int *)(*(int *)((int)pvVar1 + 0x1c) + partitionindex * 4));
      pvVar5 = MeChunkGetMem(&w->keaPool,uVar4);
      (w->keaParams).memory_pool = pvVar5;
      (w->keaParams).memory_pool_size = uVar4;
      MdtFlushCache(0);
      pMVar9 = pMVar7;
      pMVar11 = aMStackY_a4;
      for (iVar6 = 0x13; iVar6 != 0; iVar6 = iVar6 + -1) {
        *pMVar11 = pMVar9->stepsize;
        pMVar9 = (MdtKeaParameters *)&pMVar9->epsilon;
        pMVar11 = pMVar11 + 1;
      }
      pvStackY_a8 = *(void **)(*(int *)((int)pvVar1 + 0x1c) + partitionindex * 4);
      pMVar8 = keaCon_00;
      piVar10 = aiStackY_10c;
      ppMStackY_b0 = keabodyArray_00;
      ppMStackY_ac = pvVar3;
      for (iVar6 = 0x17; iVar6 != 0; iVar6 = iVar6 + -1) {
        *piVar10 = pMVar8->num_partitions;
        pMVar8 = (MdtKeaConstraints *)&pMVar8->max_partitions;
        piVar10 = piVar10 + 1;
      }
      MdtKeaAddConstraintForces();
      if (w->checkSim != 0) {
        CheckSim(w,keabodyArray_00,*(int *)(*(int *)((int)pvVar1 + 0x1c) + partitionindex * 4),
                 keaCon_00);
      }
      pMVar9 = pMVar7;
      pMVar11 = aMStackY_a4 + 1;
      for (iVar6 = 0x13; iVar6 != 0; iVar6 = iVar6 + -1) {
        *pMVar11 = pMVar9->stepsize;
        pMVar9 = (MdtKeaParameters *)&pMVar9->epsilon;
        pMVar11 = pMVar11 + 1;
      }
      aMStackY_a4[0] = *(MeReal *)(*(int *)((int)pvVar1 + 0x1c) + partitionindex * 4);
      ppMStackY_b0 = (MdtKeaBody **)0x10853;
      ppMStackY_ac = keabodyArray_00;
      pvStackY_a8 = pvVar3;
      MdtKeaIntegrateSystem();
      MeChunkPutMem(&w->keaPool,(w->keaParams).memory_pool);
      MdtUnpackBodies(pvVar3,partitionindex,pvVar1);
      MdtUnpackForces(keaCon_00->force,partitionindex,pvVar1);
      MeChunkPutMem(&w->keaConstraintsChunk,keaCon_00);
      MeChunkPutMem(&w->keaTMChunk,pvVar3);
      partitionindex = partitionindex + 1;
      local_48 = local_48 + 0xc;
    } while (partitionindex < *(int *)((int)pvVar1 + 0x2c));
  }
  MeChunkPutMem(&w->partOutChunk,pvVar1);
  for (pvVar1 = MeDictFirst(pMVar2); pvVar1 != (void *)0x0; pvVar1 = MeDictNext(pMVar2,pvVar1)) {
    pvVar3 = *(void **)((int)pvVar1 + 0x14);
    MdtBodyResetForces(pvVar3);
    *(undefined4 *)((int)pvVar3 + 0x238) = 0x7f7fffff;
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
  undefined4 extraout_EAX;
  void *pvVar2;
  undefined4 uVar3;
  int iVar4;
  float fVar5;
  uint uVar6;
  MeReal *pMVar7;
  undefined4 uVar8;
  MeVector3 end;
  MeVector3 end_1;
  MeVector3 start;
  
                    /* Unresolved local var: int c@[DW_OP_reg7(EDI)] */
  iVar4 = po->constraintsStart[pid];
  if (iVar4 < po->constraintsSize[pid] + iVar4) {
    do {
                    /* Unresolved local var: MdtConstraintID constraint@[???]
                       Unresolved local var: MdtContactGroupID.conflict group@[DW_OP_reg0(EAX)] */
      pvVar1 = MdtConstraintDCastContactGroup(po->constraints[iVar4]);
                    /* Unresolved local var: MdtContactID.conflict contact@[DW_OP_reg3(EBX)] */
      if ((pvVar1 != (void *)0x0) &&
         (pvVar1 = *(void **)((int)pvVar1 + 0x164), pvVar1 != (void *)0x0)) {
        do {
          MdtContactGetPosition(pvVar1,start);
          uVar3 = extraout_EAX;
          if ((o & MdtDebugDrawContacts) != 0) {
            pMVar7 = end_1;
            uVar8 = extraout_EAX;
            MdtContactGetNormal(pvVar1,pMVar7);
            fVar5 = MdtContactGetPenetration(pvVar1);
            fVar5 = fVar5 * 50.0 + 1.0;
            end_1[1] = end_1[1] * fVar5 + start[1];
            end_1[0] = end_1[0] * fVar5 + start[0];
            end_1[2] = fVar5 * end_1[2] + start[2];
            uVar3 = (*_MeDebugDrawAPI)(start,end_1,0x3f800000,0,0,pMVar7,uVar3,uVar8);
          }
          if ((o & MdtDebugDrawContactForce) != 0) {
            pMVar7 = end;
            uVar6 = 0;
            pvVar2 = MdtContactQuaConstraint(pvVar1);
            MdtConstraintGetForce(pvVar2,uVar6,pMVar7);
            end[1] = end[1] + start[1];
            end[0] = start[0] + end[0];
            end[2] = end[2] + start[2];
            (*_MeDebugDrawAPI)(start,end,0,0x3f800000,0,uVar6,pMVar7,uVar3);
          }
          pvVar1 = *(void **)((int)pvVar1 + 0x1dc);
        } while (pvVar1 != (void *)0x0);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < po->constraintsSize[pid] + po->constraintsStart[pid]);
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


