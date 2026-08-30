/* ==== PrismaticSetAxis ==== */

void PrismaticSetAxis(MdtConstraintID c,MeReal px,MeReal py,MeReal pz)

{
  float fVar1;
  uint uVar2;
  MeVector3 rel_pos;
  MeVector3 r_axis2;
  MeVector3 r_axis1;
  MeVector3 axis;
  
                    /* Unresolved local var: MdtPrismaticID j@[???]
                       Unresolved local var: MeReal magSqr@[???] */
  axis[1] = py;
  axis[0] = px;
  uVar2 = ((c->head).tag != 2) - 1 & (uint)c;
  axis[2] = pz;
  rel_pos[0] = *(float *)(*(int *)(uVar2 + 0x54) + 0x120);
  rel_pos[1] = *(float *)(*(int *)(uVar2 + 0x54) + 0x124);
  rel_pos[2] = *(float *)(*(int *)(uVar2 + 0x54) + 0x128);
  if ((c->head).mdtbody[1] != (MdtBody *)0x0) {
    rel_pos[0] = rel_pos[0] - *(float *)(*(int *)(uVar2 + 0x58) + 0x120);
    rel_pos[1] = rel_pos[1] - *(float *)(*(int *)(uVar2 + 0x58) + 0x124);
    rel_pos[2] = rel_pos[2] - *(float *)(*(int *)(uVar2 + 0x58) + 0x128);
  }
                    /* Unresolved local var: MeReal m@[???] */
  if (0.001 <= ABS((px * px + py * py + pz * pz) - 1.0)) {
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal k@[DW_OP_reg13(ST2)] */
    fVar1 = py * py + px * px + pz * pz;
    if (fVar1 <= 0.0) {
      axis[0] = 1.0;
      axis[1] = 0.0;
      axis[2] = 0.0;
    }
    else {
      fVar1 = 1.0 / SQRT(fVar1);
      axis[0] = px * fVar1;
      axis[1] = py * fVar1;
      axis[2] = pz * fVar1;
    }
  }
  if (ABS(axis[0]) <= ABS(axis[1])) {
                    /* Unresolved local var: MeReal scale@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[???] */
    fVar1 = 1.0 / SQRT(axis[1] * axis[1] + axis[2] * axis[2]);
    r_axis1[0] = 0.0;
    r_axis1[2] = axis[1] * fVar1;
    r_axis1[1] = -axis[2] * fVar1;
    r_axis2[0] = axis[1] * r_axis1[2] - axis[2] * r_axis1[1];
    r_axis2[2] = axis[0] * r_axis1[1];
    r_axis2[1] = -axis[0] * r_axis1[2];
  }
  else {
                    /* Unresolved local var: MeReal scale@[DW_OP_reg13(ST2)]
                       Unresolved local var: float __result@[???] */
    fVar1 = 1.0 / SQRT(axis[2] * axis[2] + axis[0] * axis[0]);
    r_axis1[1] = 0.0;
    r_axis1[0] = axis[2] * fVar1;
    r_axis1[2] = -axis[0] * fVar1;
    r_axis2[0] = axis[1] * r_axis1[2];
    r_axis2[1] = axis[2] * r_axis1[0] - r_axis1[2] * axis[0];
    r_axis2[2] = -axis[1] * r_axis1[0];
  }
  ConvertCOMVector((void *)0x0,axis,*(void **)(uVar2 + 0x54),(void *)(uVar2 + 0xbc));
  ConvertCOMVector((void *)0x0,r_axis1,*(void **)(uVar2 + 0x54),(void *)(uVar2 + 0xcc));
  ConvertCOMVector((void *)0x0,r_axis2,*(void **)(uVar2 + 0x54),(void *)(uVar2 + 0xdc));
  ConvertCOMVector((void *)0x0,axis,*(void **)(uVar2 + 0x58),(void *)(uVar2 + 0xfc));
  ConvertCOMVector((void *)0x0,r_axis1,*(void **)(uVar2 + 0x58),(void *)(uVar2 + 0x10c));
  ConvertCOMVector((void *)0x0,r_axis2,*(void **)(uVar2 + 0x58),(void *)(uVar2 + 0x11c));
  ConvertCOMVector((void *)0x0,rel_pos,*(void **)(uVar2 + 0x58),(void *)(uVar2 + 300));
  return;
}


/* ==== MdtPrismaticCreate ==== */

/* WARNING: Unknown calling convention */

MdtPrismaticID MdtPrismaticCreate(MdtWorldID w)

{
  MdtPrismaticID pMVar1;
  int iVar2;
  MdtPrismaticID pMVar3;
  
                    /* Unresolved local var: MdtPrismatic * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtPrismaticID)0x0;
  if (pMVar1 != (MdtPrismaticID)0x0) {
    (pMVar1->head).tag = 2;
    (pMVar1->head).bclFunction = MdtBclAddPrismatic;
    (pMVar1->head).maxRows = 6;
    (pMVar1->head).setBodyFunc = BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = PrismaticSetAxis;
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


/* ==== MdtPrismaticReset ==== */

void MdtPrismaticReset(MdtPrismaticID j)

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


/* ==== MdtPrismaticQuaConstraint ==== */

MdtConstraintID MdtPrismaticQuaConstraint(MdtPrismaticID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastPrismatic ==== */

MdtPrismaticID MdtConstraintDCastPrismatic(MdtConstraintID c)

{
  return (MdtPrismaticID)(((c->head).tag != 2) - 1 & (uint)c);
}


/* ==== MdtPrismaticGetLimit ==== */

/* WARNING: Unknown calling convention */

MdtLimitID MdtPrismaticGetLimit(MdtPrismaticID j)

{
  return &j->limit;
}


/* ==== MdtPrismaticSetLimit ==== */

void MdtPrismaticSetLimit(MdtPrismaticID j,MdtLimitID NewLimit)

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


