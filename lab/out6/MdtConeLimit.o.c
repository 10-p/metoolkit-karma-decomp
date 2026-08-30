/* ==== MdtConeLimitCreate ==== */

MdtConeLimitID MdtConeLimitCreate(MdtWorldID w)

{
  MdtConeLimitID pMVar1;
  int iVar2;
  MdtConeLimitID pMVar3;
  
                    /* Unresolved local var: MdtConeLimit * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtConeLimitID)0x0;
  if (pMVar1 != (MdtConeLimitID)0x0) {
    (pMVar1->head).tag = 0xf;
    (pMVar1->head).bclFunction = MdtBclAddConeLimit;
    (pMVar1->head).maxRows = 1;
    (pMVar1->head).setBodyFunc = BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    pMVar1->cone_half_angle = 3.1415927;
    pMVar1->cos_cone_half_angle = -1.0;
    pMVar1->stiffness = 3.4028235e+38;
    pMVar1->damping = 3.4028235e+38;
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtConeLimitReset ==== */

void MdtConeLimitReset(MdtConeLimitID j)

{
  int iVar1;
  
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  j->cone_half_angle = 3.1415927;
  j->cos_cone_half_angle = -1.0;
  j->stiffness = 3.4028235e+38;
  j->damping = 3.4028235e+38;
  BaseConstraintReset(j);
  return;
}


/* ==== MdtConeLimitQuaConstraint ==== */

MdtConstraintID MdtConeLimitQuaConstraint(MdtConeLimitID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastConeLimit ==== */

MdtConeLimitID MdtConstraintDCastConeLimit(MdtConstraintID c)

{
  return (MdtConeLimitID)(((c->head).tag != 0xf) - 1 & (uint)c);
}


/* ==== MdtConeLimitSetConeHalfAngle ==== */

void MdtConeLimitSetConeHalfAngle(MdtConeLimitID c,MeReal theta)

{
  longdouble lVar1;
  
  c->cone_half_angle = theta;
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
  lVar1 = (longdouble)fcos((longdouble)theta);
  c->cos_cone_half_angle = (float)lVar1;
  return;
}


/* ==== MdtConeLimitGetConeHalfAngle ==== */

MeReal MdtConeLimitGetConeHalfAngle(MdtConeLimitID c)

{
  return c->cone_half_angle;
}


/* ==== MdtConeLimitSetStiffness ==== */

void MdtConeLimitSetStiffness(MdtConeLimitID c,MeReal kp)

{
  c->stiffness = kp;
  return;
}


/* ==== MdtConeLimitGetStiffness ==== */

MeReal MdtConeLimitGetStiffness(MdtConeLimitID c)

{
  return c->stiffness;
}


/* ==== MdtConeLimitSetDamping ==== */

void MdtConeLimitSetDamping(MdtConeLimitID c,MeReal kd)

{
  c->damping = kd;
  return;
}


/* ==== MdtConeLimitGetDamping ==== */

MeReal MdtConeLimitGetDamping(MdtConeLimitID c)

{
  return c->damping;
}


