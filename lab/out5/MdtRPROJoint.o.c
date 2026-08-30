/* ==== MdtRPROJointCreate ==== */

/* WARNING: Unknown calling convention */

MdtRPROJointID MdtRPROJointCreate(MdtWorldID w)

{
  MdtRPROJointID pMVar1;
  int iVar2;
  MdtRPROJointID pMVar3;
  
                    /* Unresolved local var: MdtRPROJoint * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtRPROJointID)0x0;
  if (pMVar1 != (MdtRPROJointID)0x0) {
                    /* Unresolved local var: int i@[???] */
    (pMVar1->head).tag = 7;
    (pMVar1->head).bclFunction = MdtBclAddRPROJoint;
    (pMVar1->head).maxRows = 6;
    (pMVar1->head).setBodyFunc = BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    pMVar1->q_rel[0] = 1.0;
    pMVar1->q2[0] = 1.0;
    pMVar1->q1[0] = 1.0;
    pMVar1->q_rel[1] = 0.0;
    pMVar1->omega[0] = 0.0;
    pMVar1->q2[1] = 0.0;
    pMVar1->q1[1] = 0.0;
    pMVar1->p_rel[0] = 0.0;
    pMVar1->use_q2 = 0;
    pMVar1->q_rel[2] = 0.0;
    pMVar1->omega[1] = 0.0;
    pMVar1->q2[2] = 0.0;
    pMVar1->q1[2] = 0.0;
    pMVar1->p_rel[1] = 0.0;
    pMVar1->use_q1 = 0;
    pMVar1->q_rel[3] = 0.0;
    pMVar1->omega[2] = 0.0;
    pMVar1->q2[3] = 0.0;
    pMVar1->q1[3] = 0.0;
    pMVar1->p_rel[2] = 0.0;
    pMVar1->v_rel[0] = 0.0;
    pMVar1->v_rel[1] = 0.0;
    pMVar1->v_rel[2] = 0.0;
    pMVar1->angular_fmax[0] = 3.4028235e+38;
    pMVar1->angular_fmax[1] = 3.4028235e+38;
    pMVar1->angular_fmax[2] = 3.4028235e+38;
    pMVar1->linear_fmax[0] = 3.4028235e+38;
    pMVar1->linear_fmax[1] = 3.4028235e+38;
    pMVar1->linear_fmax[2] = 3.4028235e+38;
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtRPROJointReset ==== */

void MdtRPROJointReset(MdtRPROJointID j)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[???] */
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  j->v_rel[0] = 0.0;
  j->q_rel[0] = 1.0;
  j->q2[0] = 1.0;
  j->q1[0] = 1.0;
  j->q_rel[1] = 0.0;
  j->omega[0] = 0.0;
  j->q2[1] = 0.0;
  j->q1[1] = 0.0;
  j->p_rel[0] = 0.0;
  j->use_q2 = 0;
  j->q_rel[2] = 0.0;
  j->omega[1] = 0.0;
  j->q2[2] = 0.0;
  j->q1[2] = 0.0;
  j->p_rel[1] = 0.0;
  j->use_q1 = 0;
  j->v_rel[1] = 0.0;
  j->q_rel[3] = 0.0;
  j->omega[2] = 0.0;
  j->q2[3] = 0.0;
  j->q1[3] = 0.0;
  j->p_rel[2] = 0.0;
  j->v_rel[2] = 0.0;
  j->angular_fmax[0] = 3.4028235e+38;
  j->angular_fmax[1] = 3.4028235e+38;
  j->angular_fmax[2] = 3.4028235e+38;
  j->linear_fmax[0] = 3.4028235e+38;
  j->linear_fmax[1] = 3.4028235e+38;
  j->linear_fmax[2] = 3.4028235e+38;
  BaseConstraintReset(j);
  return;
}


/* ==== MdtRPROJointQuaConstraint ==== */

MdtConstraintID MdtRPROJointQuaConstraint(MdtRPROJointID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastRPROJoint ==== */

MdtRPROJointID MdtConstraintDCastRPROJoint(MdtConstraintID c)

{
  return (MdtRPROJointID)(((c->head).tag != 7) - 1 & (uint)c);
}


/* ==== MdtRPROJointGetAttachmentPosition ==== */

void MdtRPROJointGetAttachmentPosition(MdtRPROJointID j,uint bodyindex,MeReal *position)

{
  MeReal (*paMVar1) [4];
  
                    /* Unresolved local var: MeReal * pos@[DW_OP_reg2(EDX)]
                       Unresolved local var: uint i@[???] */
  if (bodyindex == 0) {
    paMVar1 = (j->head).ref1;
  }
  else {
    paMVar1 = (j->head).ref2;
  }
  paMVar1 = paMVar1 + 3;
  *position = (*paMVar1)[0];
  position[1] = (*paMVar1)[1];
  position[2] = (*paMVar1)[2];
  return;
}


/* ==== MdtRPROJointGetAttachmentQuaternion ==== */

void MdtRPROJointGetAttachmentQuaternion(MdtRPROJointID j,uint bodyindex,MeReal *quaternion)

{
  MeReal *pMVar1;
  
                    /* Unresolved local var: MeReal * q@[DW_OP_reg2(EDX)]
                       Unresolved local var: uint i@[???] */
  if (bodyindex == 0) {
    pMVar1 = j->q1;
  }
  else {
    pMVar1 = j->q2;
  }
  *quaternion = *pMVar1;
  quaternion[1] = pMVar1[1];
  quaternion[2] = pMVar1[2];
  quaternion[3] = pMVar1[3];
  return;
}


/* ==== MdtRPROJointGetRelativePosition ==== */

void MdtRPROJointGetRelativePosition(MdtRPROJointID j,MeReal *rel_pos)

{
  *rel_pos = j->p_rel[0];
  rel_pos[1] = j->p_rel[1];
  rel_pos[2] = j->p_rel[2];
  return;
}


/* ==== MdtRPROJointGetRelativeVelocity ==== */

void MdtRPROJointGetRelativeVelocity(MdtRPROJointID j,MeReal *rel_vel)

{
  *rel_vel = j->v_rel[0];
  rel_vel[1] = j->v_rel[1];
  rel_vel[2] = j->v_rel[2];
  return;
}


/* ==== MdtRPROJointGetRelativeQuaternion ==== */

void MdtRPROJointGetRelativeQuaternion(MdtRPROJointID j,MeReal *q)

{
                    /* Unresolved local var: int i@[???] */
  *q = j->q_rel[0];
  q[1] = j->q_rel[1];
  q[2] = j->q_rel[2];
  q[3] = j->q_rel[3];
  return;
}


/* ==== MdtRPROJointGetRelativeAngularVelocity ==== */

void MdtRPROJointGetRelativeAngularVelocity(MdtRPROJointID j,MeReal *w)

{
                    /* Unresolved local var: int i@[???] */
  *w = j->omega[0];
  w[1] = j->omega[1];
  w[2] = j->omega[2];
  return;
}


/* ==== MdtRPROJointGetAngularStrength ==== */

void MdtRPROJointGetAngularStrength(MdtRPROJointID j,MeReal *v)

{
                    /* Unresolved local var: int i@[???] */
  *v = j->angular_fmax[0];
  v[1] = j->angular_fmax[1];
  v[2] = j->angular_fmax[2];
  return;
}


/* ==== MdtRPROJointGetLinearStrength ==== */

void MdtRPROJointGetLinearStrength(MdtRPROJointID j,MeReal *v)

{
                    /* Unresolved local var: int i@[???] */
  *v = j->linear_fmax[0];
  v[1] = j->linear_fmax[1];
  v[2] = j->linear_fmax[2];
  return;
}


/* ==== MdtRPROJointSetAttachmentPosition ==== */

void MdtRPROJointSetAttachmentPosition(MdtRPROJointID j,MeReal x,MeReal y,MeReal z,uint bodyindex)

{
  MeReal (*paMVar1) [4];
  
                    /* Unresolved local var: MeReal * pos@[DW_OP_reg2(EDX)] */
  if (bodyindex == 0) {
    paMVar1 = (j->head).ref1;
  }
  else {
    paMVar1 = (j->head).ref2;
  }
  paMVar1 = paMVar1 + 3;
  (*paMVar1)[0] = x;
  (*paMVar1)[1] = y;
  (*paMVar1)[2] = z;
  return;
}


/* ==== MdtRPROJointSetAttachmentQuaternion ==== */

void MdtRPROJointSetAttachmentQuaternion
               (MdtRPROJointID j,MeReal q0,MeReal q1,MeReal q2,MeReal q3,uint bodyindex)

{
  MeReal *pMVar1;
  
                    /* Unresolved local var: MeReal * q@[DW_OP_reg2(EDX)] */
  pMVar1 = j->q1;
  if (bodyindex == 0) {
    j->use_q1 = 1;
  }
  else {
    pMVar1 = j->q2;
    j->use_q2 = 1;
  }
  *pMVar1 = q0;
  pMVar1[1] = q1;
  pMVar1[2] = q2;
  pMVar1[3] = q3;
  return;
}


/* ==== MdtRPROJointSetRelativePosition ==== */

void MdtRPROJointSetRelativePosition(MdtRPROJointID j,MeReal rpx,MeReal rpy,MeReal rpz)

{
  MeVector3 rel_pos;
  
  j->p_rel[0] = rpx;
  j->p_rel[1] = rpy;
  j->p_rel[2] = rpz;
  return;
}


/* ==== MdtRPROJointSetRelativeVelocity ==== */

void MdtRPROJointSetRelativeVelocity(MdtRPROJointID j,MeReal rvx,MeReal rvy,MeReal rvz)

{
  MeVector3 rel_vel;
  
  j->v_rel[0] = rvx;
  j->v_rel[1] = rvy;
  j->v_rel[2] = rvz;
  return;
}


/* ==== MdtRPROJointSetRelativeQuaternion ==== */

void MdtRPROJointSetRelativeQuaternion(MdtRPROJointID j,MeReal *q)

{
  j->q_rel[0] = *q;
  j->q_rel[1] = q[1];
  j->q_rel[2] = q[2];
  j->q_rel[3] = q[3];
  return;
}


/* ==== MdtRPROJointSetRelativeAngularVelocity ==== */

void MdtRPROJointSetRelativeAngularVelocity(MdtRPROJointID j,MeReal *w)

{
                    /* Unresolved local var: int i@[???] */
  j->omega[0] = *w;
  j->omega[1] = w[1];
  j->omega[2] = w[2];
  return;
}


/* ==== MdtRPROJointSetAngularStrength ==== */

void MdtRPROJointSetAngularStrength(MdtRPROJointID j,MeReal sX,MeReal sY,MeReal sZ)

{
  j->angular_fmax[0] = sX;
  j->angular_fmax[1] = sY;
  j->angular_fmax[2] = sZ;
  return;
}


/* ==== MdtRPROJointSetLinearStrength ==== */

void MdtRPROJointSetLinearStrength(MdtRPROJointID j,MeReal sX,MeReal sY,MeReal sZ)

{
  j->linear_fmax[0] = sX;
  j->linear_fmax[1] = sY;
  j->linear_fmax[2] = sZ;
  return;
}


