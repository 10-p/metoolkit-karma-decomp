/* ==== MdtWorldCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MdtWorldID MdtWorldCreate(uint maxBodies,uint maxConstraints,MeReal lengthScale,MeReal massScale)

{
  MdtWorldID pMVar1;
  MdtWorldID w;
  MeCPUResources p_Var2;
  char *pcVar3;
  undefined4 extraout_ECX;
  
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
      p_Var2 = (MeCPUResources)MdtKeaQueryCPUResources();
      (w->keaParams).cpu_resources = p_Var2;
      MeChunkInit(&w->keaPool,0x10,extraout_ECX,extraout_ECX);
      w->maxBodies = maxBodies;
      w->nBodies = 0;
      w->nEnabledBodies = 0;
      MeChunkInit(&w->keaTMChunk,0x10);
      (*_MePoolAPI)(&w->bodyPool,w->maxBodies,0x240,0x10);
      w->maxConstraints = maxConstraints;
      w->nEnabledConstraints = 0;
      (*_MePoolAPI)(&w->constraintPool,w->maxConstraints,0x1ec,0);
      MeDictInit(w,1000000,&MdtDictCompare);
      MeDictAllowDupes(w);
      MeDictInit(&w->enabledBodyDict,1000000,&MdtDictCompare);
      MeDictAllowDupes(&w->enabledBodyDict);
      MeDictInit(&w->constraintDict,1000000,&MdtDictCompare);
      MeDictAllowDupes(&w->constraintDict);
      MeChunkInit(&w->partOutChunk,0);
      MeChunkInit(&w->keaConstraintsChunk,0);
      w->bodyDisableCallback = (MdtBodyCallbackCBPtr)0x0;
      w->bodyEnableCallback = (MdtBodyCallbackCBPtr)0x0;
      w->contactGroupDestroyCallback = (MdtContactGroupDestroyCallbackCBPtr)0x0;
      MdtWorldReset(w);
      pcVar3 = (char *)MeToolkitVersionString();
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
  w->simErrorCallback = (MdtSimErrorCBPtr)&MdtDefaultSimErrorCallBack;
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
  undefined4 uVar2;
  MdtKeaConstraints *keaCon_00;
  uint uVar3;
  void *pvVar4;
  int iVar5;
  MdtKeaParameters *pMVar6;
  MdtKeaConstraints *pMVar7;
  MdtKeaParameters *pMVar8;
  int *piVar9;
  MeReal *pMVar10;
  int aiStackY_fc [23];
  MdtBody **ppMStackY_a0;
  MdtBody **ppMStackY_9c;
  int iStackY_98;
  MeReal aMStackY_94 [13];
  undefined4 uStackY_60;
  MdtPartitionOutput *pMStackY_5c;
  MdtPartitionOutput *pMStackY_58;
  MdtWorldParams *pMStackY_54;
  MdtKeaParameters *pMStackY_50;
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
  pMStackY_50 = (MdtKeaParameters *)0x1035c;
  po_00 = (MdtPartitionOutput *)MdtPartOutCreateFromChunk();
  pMStackY_50 = (MdtKeaParameters *)&w->partitionParams;
  pMStackY_54 = (MdtWorldParams *)&MdtAutoDisableLastPartition;
  pMStackY_5c = (MdtPartitionOutput *)&w->enabledBodyDict;
  uStackY_60 = 0x10381;
  pMStackY_58 = po_00;
  MdtUpdatePartitions();
  if (0 < po_00->nPartitions) {
    pMStackY_50 = (MdtKeaParameters *)0x103ad;
    uVar2 = MeChunkGetMem();
    pMStackY_50 = (MdtKeaParameters *)(po_00->overallInfo).rowCount;
    pMStackY_54 = (MdtWorldParams *)
                  ((po_00->overallInfo).jointCount + (po_00->overallInfo).contactCount);
    pMStackY_5c = (MdtPartitionOutput *)&w->keaConstraintsChunk;
    pMStackY_58 = (MdtPartitionOutput *)po_00->nPartitions;
    uStackY_60 = 0x103d5;
    keaCon_00 = (MdtKeaConstraints *)MdtKeaConstraintsCreateFromChunk();
    pMVar8 = &w->keaParams;
    pMStackY_54 = &w->params;
    pMStackY_58 = (MdtPartitionOutput *)stepSize;
    uStackY_60 = 0x10401;
    pMStackY_5c = po_00;
    pMStackY_50 = pMVar8;
    MdtPackAllPartitions();
    keabodyArray_00 = po_00->bodies;
    MVar1 = w->constantGamma;
    (w->keaParams).stepsize = stepSize;
    (w->keaParams).gamma = MVar1;
    pMStackY_50 = (MdtKeaParameters *)0x10439;
    uVar3 = MdtKeaMemoryRequired();
    pMStackY_50 = (MdtKeaParameters *)0x10455;
    pvVar4 = (void *)MeChunkGetMem();
    (w->keaParams).memory_pool = pvVar4;
    (w->keaParams).memory_pool_size = uVar3;
    pMStackY_50 = (MdtKeaParameters *)0x10470;
    MdtFlushCache();
    pMVar6 = pMVar8;
    pMVar10 = aMStackY_94;
    for (iVar5 = 0x13; iVar5 != 0; iVar5 = iVar5 + -1) {
      *pMVar10 = pMVar6->stepsize;
      pMVar6 = (MdtKeaParameters *)&pMVar6->epsilon;
      pMVar10 = pMVar10 + 1;
    }
    iStackY_98 = po_00->totalBodies;
    pMVar7 = keaCon_00;
    piVar9 = aiStackY_fc;
    ppMStackY_a0 = keabodyArray_00;
    ppMStackY_9c = (MdtBody **)uVar2;
    for (iVar5 = 0x17; iVar5 != 0; iVar5 = iVar5 + -1) {
      *piVar9 = pMVar7->num_partitions;
      pMVar7 = (MdtKeaConstraints *)&pMVar7->max_partitions;
      piVar9 = piVar9 + 1;
    }
    MdtKeaAddConstraintForces();
    if (w->checkSim != 0) {
      pMStackY_50 = (MdtKeaParameters *)0x10606;
      CheckSim(w,(MdtKeaBody **)keabodyArray_00,po_00->totalBodies,keaCon_00);
    }
    pMVar10 = aMStackY_94 + 1;
    for (iVar5 = 0x13; iVar5 != 0; iVar5 = iVar5 + -1) {
      *pMVar10 = pMVar8->stepsize;
      pMVar8 = (MdtKeaParameters *)&pMVar8->epsilon;
      pMVar10 = pMVar10 + 1;
    }
    aMStackY_94[0] = (MeReal)po_00->totalBodies;
    ppMStackY_a0 = (MdtBody **)0x104d8;
    ppMStackY_9c = keabodyArray_00;
    iStackY_98 = uVar2;
    MdtKeaIntegrateSystem();
    iVar5 = 0;
    pMStackY_50 = (MdtKeaParameters *)0x104f0;
    MeChunkPutMem();
    if (0 < po_00->nPartitions) {
      do {
        iVar5 = iVar5 + 1;
        pMStackY_50 = (MdtKeaParameters *)0x105e1;
        MdtUnpackBodies();
      } while (iVar5 < po_00->nPartitions);
    }
    pMStackY_50 = (MdtKeaParameters *)0x1050e;
    MeChunkPutMem();
    iVar5 = 0;
    if (0 < po_00->nPartitions) {
      do {
                    /* Unresolved local var: int conCount@[DW_OP_reg0(EAX)] */
        pMStackY_50 = (MdtKeaParameters *)0x1058c;
        MdtUnpackForces();
        o = (w->partitionParams).debugOptions;
        if ((o & (MdtDebugDrawContactForce|MdtDebugDrawContacts)) != 0) {
          pMStackY_50 = (MdtKeaParameters *)0x105be;
          DrawPartitionContacts(po_00,iVar5,o);
        }
        iVar5 = iVar5 + 1;
      } while (iVar5 < po_00->nPartitions);
    }
    pMStackY_50 = (MdtKeaParameters *)0x10530;
    MeChunkPutMem();
  }
  pMStackY_50 = (MdtKeaParameters *)0x10542;
  MeChunkPutMem();
  pMStackY_50 = (MdtKeaParameters *)0x1054c;
  iVar5 = MeDictFirst();
  while (iVar5 != 0) {
    pMStackY_50 = (MdtKeaParameters *)0x10561;
    MdtBodyResetForces();
    pMStackY_50 = (MdtKeaParameters *)0x1056d;
    iVar5 = MeDictNext();
  }
  return;
}


/* ==== MdtWorldStepSafeTime ==== */

/* WARNING: Unknown calling convention */

void MdtWorldStepSafeTime(MdtWorldID w,MeReal stepSize)

{
  MdtKeaBody **keabodyArray_00;
  MeDict *pMVar1;
  undefined4 uVar2;
  MdtKeaConstraints *keaCon_00;
  uint uVar3;
  void *pvVar4;
  int iVar5;
  MdtKeaConstraints *pMVar6;
  MdtKeaParameters *pMVar7;
  int *piVar8;
  MeReal *pMVar9;
  longdouble lVar10;
  int aiStackY_10c [23];
  MdtKeaBody **ppMStackY_b0;
  MdtKeaBody **ppMStackY_ac;
  undefined4 uStackY_a8;
  MeReal aMStackY_a4 [13];
  undefined4 uStackY_70;
  MeDict *pMStackY_6c;
  MeDict *pMStackY_68;
  undefined *puStackY_64;
  MdtPartitionParams *pMStackY_60;
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
  pMStackY_60 = (MdtPartitionParams *)0x10639;
  pMVar1 = (MeDict *)MdtPartOutCreateFromChunk();
  pMStackY_60 = &w->partitionParams;
  puStackY_64 = &MdtAutoDisableLastPartition;
  pMStackY_6c = &w->enabledBodyDict;
  uStackY_70 = 0x1065e;
  pMStackY_68 = pMVar1;
  MdtUpdatePartitions();
  partitionindex = 0;
  if (0 < (int)pMVar1->context) {
    do {
                    /* Unresolved local var: int conCount@[???]
                       Unresolved local var: int partStart@[DW_OP_reg1(ECX)]
                       Unresolved local var: int sizeRequired@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal safeTime@[DW_OP_reg11(ST0)] */
      pMStackY_60 = (MdtPartitionParams *)0x106d3;
      uVar2 = MeChunkGetMem();
      pMStackY_60 = (MdtPartitionParams *)0x106e5;
      lVar10 = (longdouble)MdtPartitionGetSafeTime();
      if (lVar10 < (longdouble)w->minSafeTime) {
        lVar10 = (longdouble)w->minSafeTime;
      }
      timeStep = stepSize;
      if (lVar10 < (longdouble)stepSize) {
        timeStep = (MeReal)lVar10;
      }
      pMStackY_60 = (MdtPartitionParams *)0x10740;
      keaCon_00 = (MdtKeaConstraints *)MdtKeaConstraintsCreateFromChunk();
      puStackY_64 = (undefined *)timeStep;
      pMStackY_68 = (MeDict *)partitionindex;
      uStackY_70 = 0x10761;
      pMStackY_6c = pMVar1;
      pMStackY_60 = (MdtPartitionParams *)&w->params;
      MdtPackPartition();
      keabodyArray_00 =
           (MdtKeaBody **)
           ((int)(pMVar1->nilnode).data + *(int *)(pMVar1->nodecount + partitionindex * 4) * 4);
      (w->keaParams).gamma = (w->constantGamma / stepSize) * timeStep;
      (w->keaParams).stepsize = timeStep;
      pMStackY_60 = (MdtPartitionParams *)0x107b1;
      uVar3 = MdtKeaMemoryRequired();
      pMStackY_60 = (MdtPartitionParams *)0x107c4;
      pvVar4 = (void *)MeChunkGetMem();
      (w->keaParams).memory_pool = pvVar4;
      (w->keaParams).memory_pool_size = uVar3;
      pMStackY_60 = (MdtPartitionParams *)0x107df;
      MdtFlushCache();
      pMVar7 = &w->keaParams;
      pMVar9 = aMStackY_a4;
      for (iVar5 = 0x13; iVar5 != 0; iVar5 = iVar5 + -1) {
        *pMVar9 = pMVar7->stepsize;
        pMVar7 = (MdtKeaParameters *)&pMVar7->epsilon;
        pMVar9 = pMVar9 + 1;
      }
      uStackY_a8 = *(undefined4 *)(pMVar1->maxcount + partitionindex * 4);
      pMVar6 = keaCon_00;
      piVar8 = aiStackY_10c;
      ppMStackY_b0 = keabodyArray_00;
      ppMStackY_ac = (MdtKeaBody **)uVar2;
      for (iVar5 = 0x17; iVar5 != 0; iVar5 = iVar5 + -1) {
        *piVar8 = pMVar6->num_partitions;
        pMVar6 = (MdtKeaConstraints *)&pMVar6->max_partitions;
        piVar8 = piVar8 + 1;
      }
      MdtKeaAddConstraintForces();
      if (w->checkSim != 0) {
        pMStackY_60 = (MdtPartitionParams *)0x10941;
        CheckSim(w,keabodyArray_00,*(int *)(pMVar1->maxcount + partitionindex * 4),keaCon_00);
      }
      pMVar7 = &w->keaParams;
      pMVar9 = aMStackY_a4 + 1;
      for (iVar5 = 0x13; iVar5 != 0; iVar5 = iVar5 + -1) {
        *pMVar9 = pMVar7->stepsize;
        pMVar7 = (MdtKeaParameters *)&pMVar7->epsilon;
        pMVar9 = pMVar9 + 1;
      }
      aMStackY_a4[0] = *(MeReal *)(pMVar1->maxcount + partitionindex * 4);
      ppMStackY_b0 = (MdtKeaBody **)0x10853;
      ppMStackY_ac = keabodyArray_00;
      uStackY_a8 = uVar2;
      MdtKeaIntegrateSystem();
      pMStackY_60 = (MdtPartitionParams *)0x10869;
      MeChunkPutMem();
      pMStackY_60 = (MdtPartitionParams *)0x1087a;
      MdtUnpackBodies();
      pMStackY_60 = (MdtPartitionParams *)0x1088e;
      MdtUnpackForces();
      pMStackY_60 = (MdtPartitionParams *)0x1089a;
      MeChunkPutMem();
      pMStackY_60 = (MdtPartitionParams *)0x108a9;
      MeChunkPutMem();
      partitionindex = partitionindex + 1;
    } while (partitionindex < (int)pMVar1->context);
  }
  pMStackY_60 = (MdtPartitionParams *)0x108da;
  MeChunkPutMem();
  pMStackY_60 = (MdtPartitionParams *)0x108e4;
  iVar5 = MeDictFirst();
  while (iVar5 != 0) {
    iVar5 = *(int *)(iVar5 + 0x14);
    pMStackY_60 = (MdtPartitionParams *)0x108fc;
    MdtBodyResetForces();
    *(undefined4 *)(iVar5 + 0x238) = 0x7f7fffff;
    pMStackY_60 = (MdtPartitionParams *)0x10912;
    iVar5 = MeDictNext();
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

/* WARNING: Unknown calling convention */

int MdtWorldGetMaxMemoryPoolUsed(MdtWorldID w)

{
  int iVar1;
  
  iVar1 = MeChunkGetMaxSize();
  return iVar1;
}


/* ==== MdtWorldGetKeaPoolChunk ==== */

/* WARNING: Unknown calling convention */

MeChunk * MdtWorldGetKeaPoolChunk(MdtWorldID w)

{
  return &w->keaPool;
}


/* ==== MdtWorldGetKeaTMChunk ==== */

/* WARNING: Unknown calling convention */

MeChunk * MdtWorldGetKeaTMChunk(MdtWorldID w)

{
  return &w->keaTMChunk;
}


/* ==== MdtWorldGetKeaConstraintsChunk ==== */

/* WARNING: Unknown calling convention */

MeChunk * MdtWorldGetKeaConstraintsChunk(MdtWorldID w)

{
  return &w->keaConstraintsChunk;
}


/* ==== MdtWorldGetPartitionOutputChunk ==== */

/* WARNING: Unknown calling convention */

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
/* WARNING: Unknown calling convention */

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

/* WARNING: Unknown calling convention */

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
  int iVar1;
  
                    /* Unresolved local var: MeDict * dict@[???]
                       Unresolved local var: MeDictNode * node@[DW_OP_reg3(EBX)] */
  for (iVar1 = MeDictFirst(w); iVar1 != 0; iVar1 = MeDictNext(w,iVar1)) {
    MdtBodyResetForces(*(undefined4 *)(iVar1 + 0x14));
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
  int iVar1;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDict * dict@[DW_OP_reg6(ESI)] */
  for (iVar1 = MeDictFirst(&w->constraintDict); iVar1 != 0;
      iVar1 = MeDictNext(&w->constraintDict,iVar1)) {
    (*cb)(*(MdtConstraintID *)(iVar1 + 0x14),ccbdata);
  }
  return;
}


/* ==== DrawPartitionContacts ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void DrawPartitionContacts(MdtPartitionOutput *po,int pid,MdtDebugDrawOptions o)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  longdouble lVar5;
  MeVector3 end;
  MeVector3 end_1;
  MeVector3 start;
  
                    /* Unresolved local var: int c@[DW_OP_reg7(EDI)] */
  iVar4 = po->constraintsStart[pid];
  if (iVar4 < po->constraintsSize[pid] + iVar4) {
    do {
                    /* Unresolved local var: MdtConstraintID constraint@[???]
                       Unresolved local var: MdtContactGroupID.conflict group@[DW_OP_reg0(EAX)] */
      iVar1 = MdtConstraintDCastContactGroup(po->constraints[iVar4]);
                    /* Unresolved local var: MdtContactID.conflict contact@[DW_OP_reg3(EBX)] */
      if ((iVar1 != 0) && (iVar3 = *(int *)(iVar1 + 0x164), iVar3 != 0)) {
        do {
          iVar1 = MdtContactGetPosition(iVar3,start,iVar1,iVar1);
          if ((o & MdtDebugDrawContacts) != 0) {
            MdtContactGetNormal(iVar3,end_1,iVar1,iVar1);
            lVar5 = (longdouble)MdtContactGetPenetration(iVar3);
            lVar5 = (longdouble)1 + lVar5 * (longdouble)50.0;
            end_1[0] = (MeReal)((longdouble)end_1[0] * lVar5 + (longdouble)start[0]);
            end_1[1] = (MeReal)((longdouble)end_1[1] * lVar5 + (longdouble)start[1]);
            end_1[2] = (MeReal)(lVar5 * (longdouble)end_1[2] + (longdouble)start[2]);
            iVar1 = (*_MeDebugDrawAPI)(start,end_1,0x3f800000,0,0);
          }
          if ((o & MdtDebugDrawContactForce) != 0) {
            uVar2 = MdtContactQuaConstraint(iVar3,0,end,iVar1);
            MdtConstraintGetForce(uVar2);
            end[1] = end[1] + start[1];
            end[0] = start[0] + end[0];
            end[2] = end[2] + start[2];
            iVar1 = (*_MeDebugDrawAPI)(start,end,0,0x3f800000,0);
          }
          iVar3 = *(int *)(iVar3 + 0x1dc);
        } while (iVar3 != 0);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < po->constraintsSize[pid] + po->constraintsStart[pid]);
  }
  return;
}


/* ==== CheckSim ==== */

/* WARNING: Unknown calling convention */

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


