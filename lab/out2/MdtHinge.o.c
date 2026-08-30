/* ==== MdtHingeCreate ==== */

/* WARNING: Unknown calling convention */

MdtHingeID MdtHingeCreate(MdtWorldID w)

{
  MdtHingeID pMVar1;
  int iVar2;
  MdtHingeID pMVar3;
  
                    /* Unresolved local var: MdtHinge * j@[DW_OP_reg3(EBX)] */
  pMVar1 = (MdtHingeID)BaseConstraintCreate(w);
  pMVar3 = (MdtHingeID)0x0;
  if (pMVar1 != (MdtHingeID)0x0) {
    (pMVar1->head).tag = 1;
    (pMVar1->head).bclFunction = (MdtBclAddConstraintFn)&MdtBclAddHinge;
    (pMVar1->head).maxRows = 6;
    (pMVar1->head).setBodyFunc = (MdtConstraintSetBodyFnPtr)&BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = (MdtConstraintSetAxisFnPtr)&BaseConstraintSetAxis;
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
  BaseConstraintReset();
  return;
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
  undefined4 uVar1;
  MdtBclLimit *pMVar2;
  longdouble lVar3;
  
                    /* Unresolved local var: MdtLimitID Limit@[???] */
  pMVar2 = &j->limit;
  MdtLimitReset(pMVar2);
  uVar1 = MdtLimitGetLowerLimit(NewLimit);
  MdtLimitSetLowerLimit(pMVar2,uVar1);
  uVar1 = MdtLimitGetUpperLimit(NewLimit);
  MdtLimitSetUpperLimit(pMVar2,uVar1);
  uVar1 = MdtLimitIsActive(NewLimit);
  MdtLimitActivateLimits(pMVar2,uVar1);
  lVar3 = (longdouble)MdtLimitGetMotorMaxForce(NewLimit);
  lVar3 = (longdouble)MdtLimitGetMotorDesiredVelocity(NewLimit,uVar1,(float)lVar3);
  MdtLimitSetLimitedForceMotor(pMVar2,(float)lVar3);
  MdtLimitIsMotorized(NewLimit);
  MdtLimitActivateMotor();
  return;
}


