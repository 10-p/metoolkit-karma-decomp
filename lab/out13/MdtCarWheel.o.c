/* ==== MdtCarWheelCreate ==== */

MdtCarWheelID MdtCarWheelCreate(MdtWorldID w)

{
  MdtCarWheelID pMVar1;
  int iVar2;
  MdtCarWheelID pMVar3;
  
                    /* Unresolved local var: MdtCarWheel * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtCarWheelID)0x0;
  if (pMVar1 != (MdtCarWheelID)0x0) {
                    /* Unresolved local var: MdtConstraintID c@[???] */
    (pMVar1->head).tag = 3;
    (pMVar1->head).bclFunction = MdtBclAddCarWheel;
    (pMVar1->head).maxRows = 6;
    (pMVar1->head).setBodyFunc = BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    (pMVar1->head).ref1[3][0] = 0.0;
    (pMVar1->head).ref1[3][1] = 0.0;
    (pMVar1->head).ref1[3][2] = 0.0;
    MdtConstraintBodySetAxesRel(pMVar1,0,0.0,0.0,1.0,0.0,1.0,0.0);
    MdtConstraintBodySetAxesRel(pMVar1,1,0.0,0.0,1.0,0.0,1.0,0.0);
    pMVar1->skp = 0.0;
    pMVar1->skd = 0.0;
    pMVar1->svel = 0.0;
    pMVar1->sfmax = 1e+09;
    pMVar1->hvel = 0.0;
    pMVar1->hfmax = 0.0;
    pMVar1->slock = 0;
    pMVar1->shi = 1e+09;
    pMVar1->slo = -1e+09;
    pMVar1->sref = 0.0;
    pMVar1->slsoft = 0.0;
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtCarWheelReset ==== */

void MdtCarWheelReset(MdtCarWheelID j)

{
  int iVar1;
  
                    /* Unresolved local var: MdtConstraintID c@[???] */
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  (j->head).ref1[3][0] = 0.0;
  (j->head).ref1[3][1] = 0.0;
  (j->head).ref1[3][2] = 0.0;
  MdtConstraintBodySetAxesRel(j,0,0.0,0.0,1.0,0.0,1.0,0.0);
  MdtConstraintBodySetAxesRel(j,1,0.0,0.0,1.0,0.0,1.0,0.0);
  j->skp = 0.0;
  j->skd = 0.0;
  j->svel = 0.0;
  j->sfmax = 1e+09;
  j->hvel = 0.0;
  j->hfmax = 0.0;
  j->slock = 0;
  j->shi = 1e+09;
  j->slo = -1e+09;
  j->sref = 0.0;
  j->slsoft = 0.0;
  BaseConstraintReset(j);
  return;
}


/* ==== MdtCarWheelQuaConstraint ==== */

MdtConstraintID MdtCarWheelQuaConstraint(MdtCarWheelID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastCarWheel ==== */

MdtCarWheelID MdtConstraintDCastCarWheel(MdtConstraintID c)

{
  return (MdtCarWheelID)(((c->head).tag != 3) - 1 & (uint)c);
}


/* ==== MdtCarWheelGetSteeringAngle ==== */

MeReal MdtCarWheelGetSteeringAngle(MdtCarWheelID j)

{
  longdouble lVar1;
  MeVector3 foo;
  MeVector3 h2;
  MeVector3 h;
  
  MdtConvertVector((j->head).mdtbody[0],(j->head).ref1 + 1,(void *)0x0,h2);
  MdtConvertVector((j->head).mdtbody[1],(j->head).ref2 + 1,(void *)0x0,h);
  MdtConvertVector((j->head).mdtbody[0],(j->head).ref1 + 2,(void *)0x0,foo);
                    /* Unresolved local var: longdouble __value@[DW_OP_reg13(ST2)] */
  lVar1 = (longdouble)
          fpatan(-((longdouble)h[0] * (longdouble)foo[0] + (longdouble)h[1] * (longdouble)foo[1] +
                  (longdouble)h[2] * (longdouble)foo[2]),
                 (longdouble)h[0] * (longdouble)h2[0] + (longdouble)h[1] * (longdouble)h2[1] +
                 (longdouble)h[2] * (longdouble)h2[2]);
  return (float)lVar1;
}


/* ==== MdtCarWheelGetSteeringAngleRate ==== */

MeReal MdtCarWheelGetSteeringAngleRate(MdtCarWheelID j)

{
  MdtBody *pMVar1;
  MdtBody *pMVar2;
  MeVector3 w_rel;
  MeVector3 sa;
  
                    /* Unresolved local var: int k@[???] */
  MdtConvertVector((j->head).mdtbody[0],(j->head).ref1,(void *)0x0,sa);
  pMVar1 = (j->head).mdtbody[0];
  pMVar2 = (j->head).mdtbody[1];
  return ((pMVar2->keaBody).velrot[2] - (pMVar1->keaBody).velrot[2]) * sa[2] +
         ((pMVar2->keaBody).velrot[0] - (pMVar1->keaBody).velrot[0]) * sa[0] +
         ((pMVar2->keaBody).velrot[1] - (pMVar1->keaBody).velrot[1]) * sa[1];
}


/* ==== MdtCarWheelGetSteeringAxis ==== */

void MdtCarWheelGetSteeringAxis(MdtCarWheelID j,MeReal *v)

{
  MdtConvertVector((j->head).mdtbody[0],(j->head).ref1,(void *)0x0,v);
  return;
}


/* ==== MdtCarWheelGetHingeAngle ==== */

MeReal MdtCarWheelGetHingeAngle(MdtCarWheelID j)

{
  longdouble lVar1;
  MeVector3 s3;
  MeVector3 s2;
  MeVector3 s1;
  
  MdtConvertVector((j->head).mdtbody[0],(j->head).ref1,(void *)0x0,s1);
  MdtConvertVector((j->head).mdtbody[1],(j->head).ref2,(void *)0x0,s2);
  MdtConvertVector((j->head).mdtbody[1],(j->head).ref2 + 1,(void *)0x0,s3);
                    /* Unresolved local var: longdouble __value@[DW_OP_reg13(ST2)] */
  lVar1 = (longdouble)
          fpatan((longdouble)s2[2] * (longdouble)s3[2] +
                 (longdouble)s2[0] * (longdouble)s3[0] + (longdouble)s2[1] * (longdouble)s3[1],
                 (longdouble)s2[0] * (longdouble)s1[0] + (longdouble)s2[1] * (longdouble)s1[1] +
                 (longdouble)s2[2] * (longdouble)s1[2]);
  return (float)lVar1;
}


/* ==== MdtCarWheelGetHingeAngleRate ==== */

MeReal MdtCarWheelGetHingeAngleRate(MdtCarWheelID j)

{
  MdtBody *pMVar1;
  MdtBody *pMVar2;
  MeVector3 w_rel;
  MeVector3 ha;
  
                    /* Unresolved local var: int k@[???] */
  MdtConvertVector((j->head).mdtbody[1],(j->head).ref2 + 1,(void *)0x0,ha);
  pMVar1 = (j->head).mdtbody[0];
  pMVar2 = (j->head).mdtbody[1];
  return ((pMVar2->keaBody).velrot[2] - (pMVar1->keaBody).velrot[2]) * ha[2] +
         ((pMVar2->keaBody).velrot[0] - (pMVar1->keaBody).velrot[0]) * ha[0] +
         ((pMVar2->keaBody).velrot[1] - (pMVar1->keaBody).velrot[1]) * ha[1];
}


/* ==== MdtCarWheelGetHingeAxis ==== */

void MdtCarWheelGetHingeAxis(MdtCarWheelID j,MeReal *v)

{
  MdtConvertVector((j->head).mdtbody[1],(j->head).ref2 + 1,(void *)0x0,v);
  return;
}


/* ==== MdtCarWheelGetSuspensionHeight ==== */

MeReal MdtCarWheelGetSuspensionHeight(MdtCarWheelID j)

{
  MdtBody *pMVar1;
  MdtBody *pMVar2;
  MeVector3 p_rel;
  MeVector3 sa;
  
                    /* Unresolved local var: int k@[???] */
  MdtConvertVector((j->head).mdtbody[0],(j->head).ref1,(void *)0x0,sa);
  pMVar1 = (j->head).mdtbody[1];
  pMVar2 = (j->head).mdtbody[0];
  return (pMVar2->comTM[3][2] - pMVar1->comTM[3][2]) * sa[2] +
         (pMVar2->comTM[3][0] - pMVar1->comTM[3][0]) * sa[0] +
         (pMVar2->comTM[3][1] - pMVar1->comTM[3][1]) * sa[1];
}


/* ==== MdtCarWheelGetSuspensionRate ==== */

MeReal MdtCarWheelGetSuspensionRate(MdtCarWheelID j)

{
  MdtBody *pMVar1;
  MdtBody *pMVar2;
  MeVector3 at_cross_sa;
  MeVector3 at;
  MeVector3 v_rel;
  MeVector3 sa;
  
                    /* Unresolved local var: int k@[???] */
  MdtConvertVector((j->head).mdtbody[0],(j->head).ref1,(void *)0x0,sa);
  MdtConvertPositionVector((j->head).mdtbody[0],(j->head).ref1 + 3,(void *)0x0,at);
  pMVar1 = (j->head).mdtbody[1];
  pMVar2 = (j->head).mdtbody[0];
  return (sa[2] * at[1] - at[2] * sa[1]) * (pMVar2->keaBody).velrot[0] +
         (at[2] * sa[0] - sa[2] * at[0]) * (pMVar2->keaBody).velrot[1] +
         (at[0] * sa[1] - at[1] * sa[0]) * (pMVar2->keaBody).velrot[2] +
         ((pMVar2->keaBody).vel[0] - (pMVar1->keaBody).vel[0]) * sa[0] +
         ((pMVar2->keaBody).vel[1] - (pMVar1->keaBody).vel[1]) * sa[1] +
         ((pMVar2->keaBody).vel[2] - (pMVar1->keaBody).vel[2]) * sa[2];
}


/* ==== MdtCarWheelGetSteeringMotorDesiredVelocity ==== */

MeReal MdtCarWheelGetSteeringMotorDesiredVelocity(MdtCarWheelID j)

{
  return j->svel;
}


/* ==== MdtCarWheelGetSteeringMotorMaxForce ==== */

MeReal MdtCarWheelGetSteeringMotorMaxForce(MdtCarWheelID j)

{
  return j->sfmax;
}


/* ==== MdtCarWheelIsSteeringLocked ==== */

MeBool MdtCarWheelIsSteeringLocked(MdtCarWheelID j)

{
  return j->slock;
}


/* ==== MdtCarWheelGetHingeMotorDesiredVelocity ==== */

MeReal MdtCarWheelGetHingeMotorDesiredVelocity(MdtCarWheelID j)

{
  return j->hvel;
}


/* ==== MdtCarWheelGetHingeMotorMaxForce ==== */

MeReal MdtCarWheelGetHingeMotorMaxForce(MdtCarWheelID j)

{
  return j->hfmax;
}


/* ==== MdtCarWheelGetSuspensionHighLimit ==== */

MeReal MdtCarWheelGetSuspensionHighLimit(MdtCarWheelID j)

{
  return j->shi;
}


/* ==== MdtCarWheelGetSuspensionLowLimit ==== */

MeReal MdtCarWheelGetSuspensionLowLimit(MdtCarWheelID j)

{
  return j->slo;
}


/* ==== MdtCarWheelGetSuspensionLimitSoftness ==== */

MeReal MdtCarWheelGetSuspensionLimitSoftness(MdtCarWheelID j)

{
  return j->slsoft;
}


/* ==== MdtCarWheelGetSuspensionReference ==== */

MeReal MdtCarWheelGetSuspensionReference(MdtCarWheelID j)

{
  return j->sref;
}


/* ==== MdtCarWheelGetSuspensionKp ==== */

MeReal MdtCarWheelGetSuspensionKp(MdtCarWheelID j)

{
  return j->skp;
}


/* ==== MdtCarWheelGetSuspensionKd ==== */

MeReal MdtCarWheelGetSuspensionKd(MdtCarWheelID j)

{
  return j->skd;
}


/* ==== MdtCarWheelSetSteeringLimitedForceMotor ==== */

void MdtCarWheelSetSteeringLimitedForceMotor
               (MdtCarWheelID j,MeReal desiredVelocity,MeReal forceLimit)

{
  j->svel = desiredVelocity;
  j->sfmax = forceLimit;
  return;
}


/* ==== MdtCarWheelSetSteeringLock ==== */

void MdtCarWheelSetSteeringLock(MdtCarWheelID j,MeBool lock)

{
  j->slock = lock;
  return;
}


/* ==== MdtCarWheelSetHingeLimitedForceMotor ==== */

void MdtCarWheelSetHingeLimitedForceMotor(MdtCarWheelID j,MeReal desiredVelocity,MeReal forceLimit)

{
  j->hvel = desiredVelocity;
  j->hfmax = forceLimit;
  return;
}


/* ==== MdtCarWheelSetSuspension ==== */

void MdtCarWheelSetSuspension
               (MdtCarWheelID j,MeReal Kp,MeReal Kd,MeReal limit_softness,MeReal lolimit,
               MeReal hilimit,MeReal reference)

{
  j->skp = Kp;
  j->skd = Kd;
  j->slsoft = limit_softness;
  j->slo = lolimit;
  j->shi = hilimit;
  j->sref = reference;
  return;
}


