/* ==== KeaIntegrateSystem_vanilla ==== */

void KeaIntegrateSystem_vanilla
               (MdtKeaBody **blist,MdtKeaTransformation *tlist,int num_bodies,
               MdtKeaParameters parameters)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  MdtKeaBody **ppMVar10;
  MdtKeaBody *pMVar11;
  int iVar12;
  MeReal dq [4];
  MeReal myw [3];
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int j@[???] */
  iVar12 = 0;
  if (num_bodies == 0) {
    return;
  }
  ppMVar10 = blist;
  if ((num_bodies & 1U) != 0) {
    pMVar11 = *blist;
    pMVar11->vel[0] = pMVar11->accel[0] * parameters.stepsize + pMVar11->vel[0];
    pMVar11 = *blist;
    pMVar11->vel[1] = pMVar11->accel[1] * parameters.stepsize + pMVar11->vel[1];
    pMVar11 = *blist;
    iVar12 = 1;
    pMVar11->vel[2] = pMVar11->accel[2] * parameters.stepsize + pMVar11->vel[2];
    pMVar11 = *blist;
    pMVar11->velrot[0] = pMVar11->accelrot[0] * parameters.stepsize + pMVar11->velrot[0];
    pMVar11 = *blist;
    pMVar11->velrot[1] = pMVar11->accelrot[1] * parameters.stepsize + pMVar11->velrot[1];
    pMVar11 = *blist;
    pMVar11->velrot[2] = pMVar11->accelrot[2] * parameters.stepsize + pMVar11->velrot[2];
    ppMVar10 = blist + 1;
    if (num_bodies == 1) goto LAB_000101ea;
  }
  do {
    pMVar11 = *ppMVar10;
    iVar12 = iVar12 + 2;
    pMVar11->vel[0] = pMVar11->accel[0] * parameters.stepsize + pMVar11->vel[0];
    pMVar11 = *ppMVar10;
    pMVar11->vel[1] = pMVar11->accel[1] * parameters.stepsize + pMVar11->vel[1];
    pMVar11 = *ppMVar10;
    pMVar11->vel[2] = pMVar11->accel[2] * parameters.stepsize + pMVar11->vel[2];
    pMVar11 = *ppMVar10;
    pMVar11->velrot[0] = pMVar11->accelrot[0] * parameters.stepsize + pMVar11->velrot[0];
    pMVar11 = *ppMVar10;
    pMVar11->velrot[1] = pMVar11->accelrot[1] * parameters.stepsize + pMVar11->velrot[1];
    pMVar11 = *ppMVar10;
    pMVar11->velrot[2] = pMVar11->accelrot[2] * parameters.stepsize + pMVar11->velrot[2];
    pMVar11 = ppMVar10[1];
    pMVar11->vel[0] = pMVar11->accel[0] * parameters.stepsize + pMVar11->vel[0];
    pMVar11 = ppMVar10[1];
    pMVar11->vel[1] = pMVar11->accel[1] * parameters.stepsize + pMVar11->vel[1];
    pMVar11 = ppMVar10[1];
    pMVar11->vel[2] = pMVar11->accel[2] * parameters.stepsize + pMVar11->vel[2];
    pMVar11 = ppMVar10[1];
    pMVar11->velrot[0] = pMVar11->accelrot[0] * parameters.stepsize + pMVar11->velrot[0];
    pMVar11 = ppMVar10[1];
    pMVar11->velrot[1] = pMVar11->accelrot[1] * parameters.stepsize + pMVar11->velrot[1];
    pMVar11 = ppMVar10[1];
    ppMVar10 = ppMVar10 + 2;
    pMVar11->velrot[2] = pMVar11->accelrot[2] * parameters.stepsize + pMVar11->velrot[2];
  } while (iVar12 != num_bodies);
LAB_000101ea:
  iVar12 = 0;
  if (num_bodies != 0) {
    do {
                    /* Unresolved local var: MeReal s@[DW_OP_reg12(ST1)] */
      tlist->pos[0] = parameters.stepsize * (*blist)->vel[0] + tlist->pos[0];
      tlist->pos[1] = parameters.stepsize * (*blist)->vel[1] + tlist->pos[1];
      fVar1 = (*blist)->vel[2];
      tlist->pos[3] = 1.0;
      tlist->pos[2] = parameters.stepsize * fVar1 + tlist->pos[2];
      pMVar11 = *blist;
      myw[0] = pMVar11->velrot[0];
      myw[1] = pMVar11->velrot[1];
      myw[2] = pMVar11->velrot[2];
      if ((pMVar11->flags & 1U) != 0) {
                    /* Unresolved local var: MeReal rot@[???] */
        fVar1 = myw[2] * pMVar11->fastSpinAxis[2] +
                myw[0] * pMVar11->fastSpinAxis[0] + myw[1] * pMVar11->fastSpinAxis[1];
        MeQuaternionFiniteRotation(pMVar11->qrot,pMVar11->fastSpinAxis,parameters.stepsize * fVar1);
        pMVar11 = *blist;
        myw[0] = myw[0] - pMVar11->fastSpinAxis[0] * fVar1;
        myw[1] = myw[1] - pMVar11->fastSpinAxis[1] * fVar1;
        myw[2] = myw[2] - fVar1 * pMVar11->fastSpinAxis[2];
      }
      fVar1 = pMVar11->qrot[0];
      fVar2 = pMVar11->qrot[3];
      fVar3 = pMVar11->qrot[2];
      fVar4 = pMVar11->qrot[3];
      fVar5 = pMVar11->qrot[0];
      fVar6 = pMVar11->qrot[1];
      fVar7 = pMVar11->qrot[2];
      fVar8 = pMVar11->qrot[1];
      fVar9 = pMVar11->qrot[0];
      pMVar11->qrot[0] =
           ((-pMVar11->qrot[1] * myw[0] - pMVar11->qrot[2] * myw[1]) - pMVar11->qrot[3] * myw[2]) *
           0.5 * parameters.stepsize + pMVar11->qrot[0];
      (*blist)->qrot[1] =
           ((fVar1 * myw[0] + fVar2 * myw[1]) - fVar3 * myw[2]) * 0.5 * parameters.stepsize +
           (*blist)->qrot[1];
      (*blist)->qrot[2] =
           (fVar6 * myw[2] + -fVar4 * myw[0] + fVar5 * myw[1]) * 0.5 * parameters.stepsize +
           (*blist)->qrot[2];
      (*blist)->qrot[3] =
           (myw[2] * fVar9 + (myw[0] * fVar7 - myw[1] * fVar8)) * 0.5 * parameters.stepsize +
           (*blist)->qrot[3];
      pMVar11 = *blist;
      fVar1 = pMVar11->qrot[0];
      fVar2 = pMVar11->qrot[1];
      fVar3 = pMVar11->qrot[2];
      fVar4 = pMVar11->qrot[3];
                    /* Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
      tlist->R0[3] = 0.0;
      fVar1 = 1.0 / SQRT(fVar4 * fVar4 + fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1);
      tlist->R1[3] = 0.0;
      tlist->R2[3] = 0.0;
      iVar12 = iVar12 + 1;
      (*blist)->qrot[0] = (*blist)->qrot[0] * fVar1;
      (*blist)->qrot[1] = (*blist)->qrot[1] * fVar1;
      (*blist)->qrot[2] = (*blist)->qrot[2] * fVar1;
      (*blist)->qrot[3] = fVar1 * (*blist)->qrot[3];
      pMVar11 = *blist;
      tlist->R0[0] = ((pMVar11->qrot[1] * pMVar11->qrot[1] + pMVar11->qrot[0] * pMVar11->qrot[0]) -
                     pMVar11->qrot[2] * pMVar11->qrot[2]) - pMVar11->qrot[3] * pMVar11->qrot[3];
      pMVar11 = *blist;
      tlist->R1[0] = (pMVar11->qrot[1] + pMVar11->qrot[1]) * pMVar11->qrot[2] -
                     (pMVar11->qrot[0] + pMVar11->qrot[0]) * pMVar11->qrot[3];
      pMVar11 = *blist;
      tlist->R2[0] = (pMVar11->qrot[1] + pMVar11->qrot[1]) * pMVar11->qrot[3] +
                     (pMVar11->qrot[0] + pMVar11->qrot[0]) * pMVar11->qrot[2];
      pMVar11 = *blist;
      tlist->R0[1] = (pMVar11->qrot[0] + pMVar11->qrot[0]) * pMVar11->qrot[3] +
                     (pMVar11->qrot[1] + pMVar11->qrot[1]) * pMVar11->qrot[2];
      pMVar11 = *blist;
      tlist->R1[1] = (pMVar11->qrot[2] * pMVar11->qrot[2] +
                     (pMVar11->qrot[0] * pMVar11->qrot[0] - pMVar11->qrot[1] * pMVar11->qrot[1])) -
                     pMVar11->qrot[3] * pMVar11->qrot[3];
      pMVar11 = *blist;
      tlist->R2[1] = (pMVar11->qrot[2] + pMVar11->qrot[2]) * pMVar11->qrot[3] +
                     pMVar11->qrot[1] * pMVar11->qrot[0] * -2.0;
      pMVar11 = *blist;
      tlist->R0[2] = (pMVar11->qrot[1] + pMVar11->qrot[1]) * pMVar11->qrot[3] +
                     pMVar11->qrot[2] * pMVar11->qrot[0] * -2.0;
      pMVar11 = *blist;
      tlist->R1[2] = (pMVar11->qrot[2] + pMVar11->qrot[2]) * pMVar11->qrot[3] +
                     (pMVar11->qrot[0] + pMVar11->qrot[0]) * pMVar11->qrot[1];
      pMVar11 = *blist;
      blist = blist + 1;
      tlist->R2[2] = pMVar11->qrot[3] * pMVar11->qrot[3] +
                     ((pMVar11->qrot[0] * pMVar11->qrot[0] - pMVar11->qrot[1] * pMVar11->qrot[1]) -
                     pMVar11->qrot[2] * pMVar11->qrot[2]);
      tlist = tlist + 1;
    } while (iVar12 != num_bodies);
  }
  return;
}


/* ==== MdtKeaIntegrateSystem ==== */

void MdtKeaIntegrateSystem
               (MdtKeaBody **blist,MdtKeaTransformation *tlist,int num_bodies,
               MdtKeaParameters parameters)

{
  int iVar1;
  MdtKeaParameters *pMVar2;
  MeReal *pMVar3;
  MdtKeaParameters in_stack_ffffffa0;
  
  pMVar2 = &parameters;
  pMVar3 = (MeReal *)&stack0xffffffa0;
                    /* Unresolved local var: pIntegratorFunc KeaIntegrateSystem@[???] */
  for (iVar1 = 0x13; iVar1 != 0; iVar1 = iVar1 + -1) {
    *pMVar3 = pMVar2->stepsize;
    pMVar2 = (MdtKeaParameters *)&pMVar2->epsilon;
    pMVar3 = pMVar3 + 1;
  }
  KeaIntegrateSystem_vanilla(blist,tlist,num_bodies,in_stack_ffffffa0);
  return;
}


