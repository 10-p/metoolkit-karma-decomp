/* ==== MdtSkeletalCreate ==== */

/* WARNING: Unknown calling convention */

MdtSkeletalID MdtSkeletalCreate(MdtWorldID w)

{
  MdtSkeletalID pMVar1;
  int iVar2;
  MdtSkeletalID pMVar3;
  longdouble lVar4;
  longdouble lVar5;
  
                    /* Unresolved local var: MdtSkeletal * j@[DW_OP_reg3(EBX)]
                       Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtSkeletalID)0x0;
  if (pMVar1 != (MdtSkeletalID)0x0) {
    (pMVar1->head).tag = 9;
    (pMVar1->head).bclFunction = MdtBclAddSkeletal;
    (pMVar1->head).maxRows = 5;
    (pMVar1->head).setBodyFunc = BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    lVar4 = (longdouble)0.3926991;
    pMVar1->combinedLimits = 0;
    pMVar1->twistOption = MdtSkeletalTwistOptionFixed;
    pMVar1->coneOption = MdtSkeletalConeOptionCone;
    lVar5 = (longdouble)fcos(lVar4);
    pMVar1->cos_half_twist_limit_angle = (float)lVar5;
                    /* Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
    lVar5 = (longdouble)fcos(lVar4);
    pMVar1->cos_half_cone_limit_angle_1 = (float)lVar5;
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
    lVar4 = (longdouble)fcos(lVar4);
    pMVar1->cos_half_cone_limit_angle_2 = (float)lVar4;
    pMVar1->twist_stiffness = 3.4028235e+38;
    pMVar1->cone_stiffness = 3.4028235e+38;
    pMVar1->twist_damping = 3.4028235e+38;
    pMVar1->cone_damping = 3.4028235e+38;
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtSkeletalReset ==== */

void MdtSkeletalReset(MdtSkeletalID j)

{
  int iVar1;
  longdouble lVar2;
  longdouble lVar3;
  
                    /* Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  lVar2 = (longdouble)0.3926991;
  j->combinedLimits = 0;
  j->twistOption = MdtSkeletalTwistOptionFixed;
  j->coneOption = MdtSkeletalConeOptionCone;
  lVar3 = (longdouble)fcos(lVar2);
  j->cos_half_twist_limit_angle = (float)lVar3;
                    /* Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
  lVar3 = (longdouble)fcos(lVar2);
  j->cos_half_cone_limit_angle_1 = (float)lVar3;
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
  lVar2 = (longdouble)fcos(lVar2);
  j->cos_half_cone_limit_angle_2 = (float)lVar2;
  j->twist_stiffness = 3.4028235e+38;
  j->cone_stiffness = 3.4028235e+38;
  j->twist_damping = 3.4028235e+38;
  j->cone_damping = 3.4028235e+38;
  BaseConstraintReset(j);
  return;
}


/* ==== MdtSkeletalQuaConstraint ==== */

MdtConstraintID MdtSkeletalQuaConstraint(MdtSkeletalID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastSkeletal ==== */

MdtSkeletalID MdtConstraintDCastSkeletal(MdtConstraintID c)

{
  return (MdtSkeletalID)(((c->head).tag != 9) - 1 & (uint)c);
}


/* ==== MdtSkeletalGetConeOption ==== */

MdtSkeletalConeOption MdtSkeletalGetConeOption(MdtSkeletalID j)

{
  return j->coneOption;
}


/* ==== MdtSkeletalGetConePrimaryLimitAngle ==== */

MeReal MdtSkeletalGetConePrimaryLimitAngle(MdtSkeletalID j)

{
  longdouble lVar1;
  
                    /* Unresolved local var: longdouble __result@[???]
                       Unresolved local var: longdouble __value@[???] */
  lVar1 = (longdouble)j->cos_half_cone_limit_angle_1;
  lVar1 = (longdouble)fpatan(SQRT((longdouble)1 - lVar1 * lVar1),lVar1);
  return (float)lVar1 + (float)lVar1;
}


/* ==== MdtSkeletalGetConeSecondaryLimitAngle ==== */

MeReal MdtSkeletalGetConeSecondaryLimitAngle(MdtSkeletalID j)

{
  longdouble lVar1;
  
                    /* Unresolved local var: longdouble __result@[???]
                       Unresolved local var: longdouble __value@[???] */
  lVar1 = (longdouble)j->cos_half_cone_limit_angle_2;
  lVar1 = (longdouble)fpatan(SQRT((longdouble)1 - lVar1 * lVar1),lVar1);
  return (float)lVar1 + (float)lVar1;
}


/* ==== MdtSkeletalGetConeStiffness ==== */

MeReal MdtSkeletalGetConeStiffness(MdtSkeletalID j)

{
  return j->cone_stiffness;
}


/* ==== MdtSkeletalGetConeDamping ==== */

MeReal MdtSkeletalGetConeDamping(MdtSkeletalID j)

{
  return j->cone_damping;
}


/* ==== MdtSkeletalGetTwistOption ==== */

MdtSkeletalTwistOption MdtSkeletalGetTwistOption(MdtSkeletalID j)

{
  return j->twistOption;
}


/* ==== MdtSkeletalGetTwistLimitAngle ==== */

MeReal MdtSkeletalGetTwistLimitAngle(MdtSkeletalID j)

{
  longdouble lVar1;
  
                    /* Unresolved local var: longdouble __result@[???]
                       Unresolved local var: longdouble __value@[???] */
  lVar1 = (longdouble)j->cos_half_twist_limit_angle;
  lVar1 = (longdouble)fpatan(SQRT((longdouble)1 - lVar1 * lVar1),lVar1);
  return (float)lVar1 + (float)lVar1;
}


/* ==== MdtSkeletalGetTwistStiffness ==== */

MeReal MdtSkeletalGetTwistStiffness(MdtSkeletalID j)

{
  return j->twist_stiffness;
}


/* ==== MdtSkeletalGetTwistDamping ==== */

MeReal MdtSkeletalGetTwistDamping(MdtSkeletalID j)

{
  return j->twist_damping;
}


/* ==== MdtSkeletalZeroTwist ==== */

void MdtSkeletalZeroTwist(MdtSkeletalID j)

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
  float fVar10;
  float fVar11;
  MeReal q0q2;
  MeReal q0q1;
  MeReal q3sq;
  MeReal q2sq;
  MeReal q1sq;
  MeVector3 axis;
  MeVector4 qRot;
  MeMatrix4 rot;
  
                    /* Unresolved local var: MeReal cosTheta@[DW_OP_reg12(ST1)] */
  fVar1 = (j->head).ref1[0][0];
  fVar2 = (j->head).ref1[0][1];
  fVar3 = (j->head).ref2[0][0];
  fVar4 = (j->head).ref1[0][2];
  fVar5 = fVar4 * (j->head).ref2[0][2] + fVar2 * (j->head).ref2[0][1] + fVar1 * fVar3;
  if (fVar5 <= 0.999999) {
    if (-0.999999 < fVar5) {
                    /* Unresolved local var: float __result@[DW_OP_reg14(ST3)] */
      fVar2 = SQRT((1.0 - fVar5) * 0.5);
      axis[0] = (j->head).ref2[0][2] * (j->head).ref1[0][1] -
                (j->head).ref2[0][1] * (j->head).ref1[0][2];
      axis[1] = fVar3 * (j->head).ref1[0][2] - fVar1 * (j->head).ref2[0][2];
      axis[2] = (j->head).ref2[0][1] * (j->head).ref1[0][0] -
                (j->head).ref2[0][0] * (j->head).ref1[0][1];
      MeVector3Normalize(axis);
      axis[0] = axis[0] * fVar2;
      axis[1] = axis[1] * fVar2;
      axis[2] = axis[2] * fVar2;
      fVar4 = SQRT(1.0 - fVar2 * fVar2);
                    /* Unresolved local var: MeReal q0sq@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q0t2@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q1t2@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q0q3@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal q1q2@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal q1q3@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q2q3@[DW_OP_reg13(ST2)] */
      fVar11 = fVar4 + fVar4;
      fVar9 = axis[1] * axis[1];
      fVar10 = axis[2] * axis[2];
      fVar5 = fVar4 * fVar4 - axis[0] * axis[0];
      fVar6 = (axis[0] + axis[0]) * axis[1];
      fVar8 = (axis[0] + axis[0]) * axis[2];
      fVar7 = (axis[1] + axis[1]) * axis[2];
      fVar1 = (j->head).ref1[2][1];
      fVar2 = (j->head).ref1[2][0];
      fVar3 = (j->head).ref1[2][2];
      fVar4 = (((axis[0] * axis[0] + fVar4 * fVar4) - fVar9) - fVar10) * fVar2 +
              (fVar6 - fVar11 * axis[2]) * fVar1 + (fVar11 * axis[1] + fVar8) * fVar3;
      (j->head).ref2[2][0] = fVar4;
      fVar6 = fVar3 * (fVar7 - axis[0] * fVar11) +
              fVar2 * (fVar6 + fVar11 * axis[2]) + fVar1 * ((fVar9 + fVar5) - fVar10);
      (j->head).ref2[2][1] = fVar6;
      fVar5 = fVar2 * (fVar8 - fVar11 * axis[1]) + fVar1 * (fVar7 + axis[0] * fVar11) +
              fVar3 * ((fVar5 - fVar9) + fVar10);
      (j->head).ref2[2][2] = fVar5;
      fVar1 = (j->head).ref2[0][2];
      fVar2 = (j->head).ref2[0][1];
      fVar3 = (j->head).ref2[0][0];
      (j->head).ref2[1][0] = fVar6 * fVar1 - fVar5 * fVar2;
      (j->head).ref2[1][2] = fVar4 * fVar2 - fVar6 * fVar3;
      (j->head).ref2[1][1] = fVar5 * fVar3 - fVar1 * fVar4;
      MeVector3Normalize((j->head).ref2[1]);
      fVar1 = (j->head).ref2[0][1];
      fVar2 = (j->head).ref2[0][2];
      fVar3 = (j->head).ref2[1][2];
      fVar4 = (j->head).ref2[1][1];
      fVar5 = (j->head).ref2[0][0];
      (j->head).ref2[2][0] = fVar1 * fVar3 - fVar2 * fVar4;
      fVar6 = (j->head).ref2[1][0];
      (j->head).ref2[2][1] = fVar2 * fVar6 - fVar3 * fVar5;
      (j->head).ref2[2][2] = fVar5 * fVar4 - fVar6 * fVar1;
    }
  }
  else {
    (j->head).ref2[0][0] = fVar1;
    (j->head).ref2[0][1] = fVar2;
    (j->head).ref2[0][2] = fVar4;
    (j->head).ref2[0][3] = (j->head).ref1[0][3];
    (j->head).ref2[1][0] = (j->head).ref1[1][0];
    (j->head).ref2[1][1] = (j->head).ref1[1][1];
    (j->head).ref2[1][2] = (j->head).ref1[1][2];
    (j->head).ref2[1][3] = (j->head).ref1[1][3];
    (j->head).ref2[2][0] = (j->head).ref1[2][0];
    (j->head).ref2[2][1] = (j->head).ref1[2][1];
    (j->head).ref2[2][2] = (j->head).ref1[2][2];
    (j->head).ref2[2][3] = (j->head).ref1[2][3];
    (j->head).ref2[3][0] = (j->head).ref1[3][0];
    (j->head).ref2[3][1] = (j->head).ref1[3][1];
    (j->head).ref2[3][2] = (j->head).ref1[3][2];
    (j->head).ref2[3][3] = (j->head).ref1[3][3];
  }
  return;
}


/* ==== MdtSkeletalSetConeOption ==== */

void MdtSkeletalSetConeOption(MdtSkeletalID j,MdtSkeletalConeOption co)

{
  j->coneOption = co;
  return;
}


/* ==== MdtSkeletalSetConePrimaryLimitAngle ==== */

void MdtSkeletalSetConePrimaryLimitAngle(MdtSkeletalID j,MeReal theta)

{
  longdouble lVar1;
  
                    /* Unresolved local var: float __result@[???] */
  lVar1 = (longdouble)fcos((longdouble)0.5 * (longdouble)theta);
  j->cos_half_cone_limit_angle_1 = (float)lVar1;
  return;
}


/* ==== MdtSkeletalSetConeSecondaryLimitAngle ==== */

void MdtSkeletalSetConeSecondaryLimitAngle(MdtSkeletalID j,MeReal theta)

{
  longdouble lVar1;
  
                    /* Unresolved local var: float __result@[???] */
  lVar1 = (longdouble)fcos((longdouble)0.5 * (longdouble)theta);
  j->cos_half_cone_limit_angle_2 = (float)lVar1;
  return;
}


/* ==== MdtSkeletalSetConeStiffness ==== */

void MdtSkeletalSetConeStiffness(MdtSkeletalID j,MeReal kp)

{
  j->cone_stiffness = kp;
  return;
}


/* ==== MdtSkeletalSetConeDamping ==== */

void MdtSkeletalSetConeDamping(MdtSkeletalID j,MeReal kd)

{
  j->cone_damping = kd;
  return;
}


/* ==== MdtSkeletalSetTwistOption ==== */

void MdtSkeletalSetTwistOption(MdtSkeletalID j,MdtSkeletalTwistOption to)

{
  j->twistOption = to;
  return;
}


/* ==== MdtSkeletalSetTwistLimitAngle ==== */

void MdtSkeletalSetTwistLimitAngle(MdtSkeletalID j,MeReal theta)

{
  longdouble lVar1;
  
                    /* Unresolved local var: float __result@[???] */
  lVar1 = (longdouble)fcos((longdouble)0.5 * (longdouble)theta);
  j->cos_half_twist_limit_angle = (float)lVar1;
  return;
}


/* ==== MdtSkeletalSetTwistStiffness ==== */

void MdtSkeletalSetTwistStiffness(MdtSkeletalID j,MeReal kp)

{
  j->twist_stiffness = kp;
  return;
}


/* ==== MdtSkeletalSetTwistDamping ==== */

void MdtSkeletalSetTwistDamping(MdtSkeletalID j,MeReal kd)

{
  j->twist_damping = kd;
  return;
}


/* ==== MeVector3Normalize ==== */

MeReal MeVector3Normalize(MeReal *v)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float local_8;
  
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
  fVar1 = *v;
  fVar2 = v[1];
  fVar3 = v[2];
  local_8 = fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1;
  if (local_8 <= 0.0) {
    *v = 1.0;
    v[1] = 0.0;
    v[2] = 0.0;
    local_8 = 0.0;
  }
  else {
    local_8 = SQRT(local_8);
    fVar4 = 1.0 / local_8;
    *v = fVar1 * fVar4;
    v[1] = fVar2 * fVar4;
    v[2] = fVar4 * fVar3;
  }
  return local_8;
}


