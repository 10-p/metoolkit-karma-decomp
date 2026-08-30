/* ==== MdtSpringCreate ==== */

/* WARNING: Unknown calling convention */

MdtSpringID MdtSpringCreate(MdtWorldID w)

{
  MdtSpringID pMVar1;
  int iVar2;
  MdtSpringID pMVar3;
  
                    /* Unresolved local var: MdtSpring * j@[DW_OP_reg6(ESI)] */
  pMVar1 = (MdtSpringID)BaseConstraintCreate(w);
  pMVar3 = (MdtSpringID)0x0;
  if (pMVar1 != (MdtSpringID)0x0) {
                    /* Unresolved local var: int i@[???] */
    (pMVar1->head).tag = 0xe;
    (pMVar1->head).bclFunction = (MdtBclAddConstraintFn)&MdtBclAddSpring;
    (pMVar1->head).maxRows = 1;
    (pMVar1->head).setBodyFunc = (MdtConstraintSetBodyFnPtr)&BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = (MdtConstraintSetAxisFnPtr)&BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    pMVar1->pos2[0] = 0.0;
    pMVar1->pos1[0] = 0.0;
    pMVar1->pos2[1] = 0.0;
    pMVar1->pos1[1] = 0.0;
    pMVar1->pos2[2] = 0.0;
    pMVar1->pos1[2] = 0.0;
    MdtLimitReset(&pMVar1->limit);
                    /* Unresolved local var: MdtLimitID Limit@[???] */
    (pMVar1->limit).limit[0].stiffness = 1.0;
    (pMVar1->limit).limit[1].stiffness = 1.0;
    MdtLimitActivateLimits(&pMVar1->limit,1);
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtSpringReset ==== */

void MdtSpringReset(MdtSpringID j)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[???] */
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  j->pos2[0] = 0.0;
  j->pos1[0] = 0.0;
  j->pos2[1] = 0.0;
  j->pos1[1] = 0.0;
  j->pos2[2] = 0.0;
  j->pos1[2] = 0.0;
  MdtLimitReset(&j->limit);
                    /* Unresolved local var: MdtLimitID Limit@[???] */
  (j->limit).limit[0].stiffness = 1.0;
  (j->limit).limit[1].stiffness = 1.0;
  MdtLimitActivateLimits(&j->limit,1);
  BaseConstraintReset();
  return;
}


/* ==== MdtConstraintDCastSpring ==== */

MdtSpringID MdtConstraintDCastSpring(MdtConstraintID c)

{
  return (MdtSpringID)(((c->head).tag != 0xe) - 1 & (uint)c);
}


/* ==== MdtSpringGetPosition ==== */

void __regparm1 MdtSpringGetPosition(MdtSpringID j,MeReal *v,uint bodyindex)

{
  int in_stack_0000000c;
  MeReal MVar1;
  MeReal *pMVar2;
  
  if (in_stack_0000000c == 0) {
    pMVar2 = v + 0x58;
    MVar1 = v[0x15];
  }
  else {
    pMVar2 = v + 0x5b;
    MVar1 = v[0x16];
  }
  ConvertCOMPositionVector(MVar1,pMVar2,0,bodyindex,j,j);
  return;
}


/* ==== MdtSpringGetLimit ==== */

/* WARNING: Unknown calling convention */

MdtLimitID MdtSpringGetLimit(MdtSpringID j)

{
  return &j->limit;
}


/* ==== MdtSpringSetPosition ==== */

void MdtSpringSetPosition(MdtSpringID j,uint bodyindex,MeReal x,MeReal y,MeReal z)

{
  MdtBody *pMVar1;
  MeReal *pMVar2;
  MeVector3 tmp;
  
  tmp[0] = x;
  tmp[1] = y;
  tmp[2] = z;
  if (bodyindex == 0) {
    pMVar2 = j->pos1;
    pMVar1 = (j->head).mdtbody[0];
  }
  else {
    pMVar2 = j->pos2;
    pMVar1 = (j->head).mdtbody[1];
  }
  ConvertCOMPositionVector(0,tmp,pMVar1,pMVar2);
  MdtConstraintBodySetPosition(j,bodyindex,x,y,z);
  return;
}


/* ==== MdtSpringSetLimit ==== */

void MdtSpringSetLimit(MdtSpringID j,MdtLimitID NewLimit)

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


/* ==== MdtSpringSetNaturalLength ==== */

void MdtSpringSetNaturalLength(MdtSpringID j,MeReal NewNaturalLength)

{
                    /* Unresolved local var: MdtLimitID Limit@[???] */
  (j->limit).limit[0].stop = NewNaturalLength;
  (j->limit).limit[1].stop = NewNaturalLength;
  return;
}


/* ==== MdtSpringSetStiffness ==== */

void MdtSpringSetStiffness(MdtSpringID j,MeReal NewStiffness)

{
                    /* Unresolved local var: MdtLimitID Limit@[???] */
  (j->limit).limit[0].stiffness = NewStiffness;
  (j->limit).limit[1].stiffness = NewStiffness;
  return;
}


/* ==== MdtSpringSetDamping ==== */

void MdtSpringSetDamping(MdtSpringID j,MeReal NewDamping)

{
                    /* Unresolved local var: MdtLimitID Limit@[???] */
  (j->limit).limit[0].damping = NewDamping;
  (j->limit).limit[1].damping = NewDamping;
  return;
}


