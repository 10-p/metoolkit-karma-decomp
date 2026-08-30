/* ==== MdtHingeCreate ==== */

/* WARNING: Unknown calling convention */

MdtHingeID MdtHingeCreate(MdtWorldID w)

{
  MdtHingeID pMVar1;
  int iVar2;
  MdtHingeID pMVar3;
  
                    /* Unresolved local var: MdtHinge * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtHingeID)0x0;
  if (pMVar1 != (MdtHingeID)0x0) {
    (pMVar1->head).tag = 1;
    (pMVar1->head).bclFunction = MdtBclAddHinge;
    (pMVar1->head).maxRows = 6;
    (pMVar1->head).setBodyFunc = BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    MdtLimitReset(&pMVar1->limit);
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtHingeReset ==== */

void MdtHingeReset(MdtHingeID j)

{
  int iVar1;
  
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  MdtLimitReset(&j->limit);
  BaseConstraintReset(j);
  return;
}


/* ==== MdtHingeQuaConstraint ==== */

MdtConstraintID MdtHingeQuaConstraint(MdtHingeID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastHinge ==== */

MdtHingeID MdtConstraintDCastHinge(MdtConstraintID c)

{
  return (MdtHingeID)(((c->head).tag != 1) - 1 & (uint)c);
}


/* ==== MdtHingeGetLimit ==== */

/* WARNING: Unknown calling convention */

MdtLimitID MdtHingeGetLimit(MdtHingeID j)

{
  return &j->limit;
}


/* ==== MdtHingeSetLimit ==== */

void MdtHingeSetLimit(MdtHingeID j,MdtLimitID NewLimit)

{
  void *pvVar1;
  int iVar2;
  MdtBclLimit *pMVar3;
  float fVar4;
  float fVar5;
  
                    /* Unresolved local var: MdtLimitID Limit@[???] */
  pMVar3 = &j->limit;
  MdtLimitReset(pMVar3);
  pvVar1 = MdtLimitGetLowerLimit(NewLimit);
  MdtLimitSetLowerLimit(pMVar3,pvVar1);
  pvVar1 = MdtLimitGetUpperLimit(NewLimit);
  MdtLimitSetUpperLimit(pMVar3,pvVar1);
  iVar2 = MdtLimitIsActive(NewLimit);
  MdtLimitActivateLimits(pMVar3,iVar2);
  fVar4 = MdtLimitGetMotorMaxForce(NewLimit);
  fVar5 = MdtLimitGetMotorDesiredVelocity(NewLimit);
  MdtLimitSetLimitedForceMotor(pMVar3,fVar5,fVar4);
  iVar2 = MdtLimitIsMotorized(NewLimit);
  MdtLimitActivateMotor(pMVar3,iVar2);
  return;
}


