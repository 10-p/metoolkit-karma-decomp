/* ==== MdtSpring6Create ==== */

MdtSpring6ID MdtSpring6Create(MdtWorldID w)

{
  MdtSpring6ID pMVar1;
  int iVar2;
  MdtSpring6ID pMVar3;
  
                    /* Unresolved local var: MdtSpring6 * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtSpring6ID)0x0;
  if (pMVar1 != (MdtSpring6ID)0x0) {
                    /* Unresolved local var: int i@[???] */
    (pMVar1->head).tag = 0xd;
    (pMVar1->head).bclFunction = MdtBclAddSpring6;
    (pMVar1->head).maxRows = 6;
    (pMVar1->head).setBodyFunc = Spring6SetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    pMVar1->linearStiffness[0] = 3.4028235e+38;
    pMVar1->linearDamping[0] = 0.0;
    pMVar1->angularStiffness[0] = 3.4028235e+38;
    pMVar1->angularDamping[0] = 0.0;
    pMVar1->worldLinearVel[0] = 0.0;
    pMVar1->linearStiffness[1] = 3.4028235e+38;
    pMVar1->linearDamping[1] = 0.0;
    pMVar1->angularStiffness[1] = 3.4028235e+38;
    pMVar1->angularDamping[1] = 0.0;
    pMVar1->worldLinearVel[1] = 0.0;
    (pMVar1->head).bodyindex[1] = 0;
    (pMVar1->head).bodyindex[0] = 0;
    pMVar1->worldAngularVel[0] = 0.0;
    pMVar1->worldAngularVel[1] = 0.0;
    pMVar1->linearStiffness[2] = 3.4028235e+38;
    pMVar1->linearDamping[2] = 0.0;
    pMVar1->angularStiffness[2] = 3.4028235e+38;
    pMVar1->angularDamping[2] = 0.0;
    pMVar1->worldLinearVel[2] = 0.0;
    pMVar1->worldAngularVel[2] = 0.0;
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtSpring6Reset ==== */

void MdtSpring6Reset(MdtSpring6ID j)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[???] */
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  (j->head).bodyindex[1] = 0;
  j->linearStiffness[0] = 3.4028235e+38;
  j->linearDamping[0] = 0.0;
  j->angularStiffness[0] = 3.4028235e+38;
  j->angularDamping[0] = 0.0;
  j->worldLinearVel[0] = 0.0;
  j->linearStiffness[1] = 3.4028235e+38;
  j->linearDamping[1] = 0.0;
  j->angularStiffness[1] = 3.4028235e+38;
  j->angularDamping[1] = 0.0;
  j->worldLinearVel[1] = 0.0;
  (j->head).bodyindex[0] = 0;
  j->worldAngularVel[0] = 0.0;
  j->worldAngularVel[1] = 0.0;
  j->linearStiffness[2] = 3.4028235e+38;
  j->linearDamping[2] = 0.0;
  j->angularStiffness[2] = 3.4028235e+38;
  j->angularDamping[2] = 0.0;
  j->worldLinearVel[2] = 0.0;
  j->worldAngularVel[2] = 0.0;
  BaseConstraintReset(j);
  return;
}


/* ==== MdtSpring6QuaConstraint ==== */

MdtConstraintID MdtSpring6QuaConstraint(MdtSpring6ID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastSpring6 ==== */

MdtSpring6ID MdtConstraintDCastSpring6(MdtConstraintID c)

{
  return (MdtSpring6ID)(((c->head).tag != 0xd) - 1 & (uint)c);
}


/* ==== MdtSpring6GetLinearStiffness ==== */

MeReal MdtSpring6GetLinearStiffness(MdtSpring6ID j,int axisindex)

{
  MeReal MVar1;
  
  MVar1 = 0.0;
  if ((uint)axisindex < 3) {
    MVar1 = j->linearStiffness[axisindex];
  }
  return MVar1;
}


/* ==== MdtSpring6GetLinearDamping ==== */

MeReal MdtSpring6GetLinearDamping(MdtSpring6ID j,int axisindex)

{
  MeReal MVar1;
  
  MVar1 = 0.0;
  if ((uint)axisindex < 3) {
    MVar1 = j->linearDamping[axisindex];
  }
  return MVar1;
}


/* ==== MdtSpring6GetAngularStiffness ==== */

MeReal MdtSpring6GetAngularStiffness(MdtSpring6ID j,int axisindex)

{
  MeReal MVar1;
  
  MVar1 = 0.0;
  if ((uint)axisindex < 3) {
    MVar1 = j->angularStiffness[axisindex];
  }
  return MVar1;
}


/* ==== MdtSpring6GetAngularDamping ==== */

MeReal MdtSpring6GetAngularDamping(MdtSpring6ID j,int axisindex)

{
  MeReal MVar1;
  
  MVar1 = 0.0;
  if ((uint)axisindex < 3) {
    MVar1 = j->angularDamping[axisindex];
  }
  return MVar1;
}


/* ==== MdtSpring6GetWorldLinearVelocity ==== */

void MdtSpring6GetWorldLinearVelocity(MdtSpring6ID j,MeReal *v)

{
  *v = j->worldLinearVel[0];
  v[1] = j->worldLinearVel[1];
  v[2] = j->worldLinearVel[2];
  return;
}


/* ==== MdtSpring6GetWorldAngularVelocity ==== */

void MdtSpring6GetWorldAngularVelocity(MdtSpring6ID j,MeReal *v)

{
  *v = j->worldAngularVel[0];
  v[1] = j->worldAngularVel[1];
  v[2] = j->worldAngularVel[2];
  return;
}


/* ==== MdtSpring6SetLinearStiffness ==== */

void MdtSpring6SetLinearStiffness(MdtSpring6ID j,int axisindex,MeReal s)

{
  if ((uint)axisindex < 3) {
    j->linearStiffness[axisindex] = s;
  }
  return;
}


/* ==== MdtSpring6SetLinearDamping ==== */

void MdtSpring6SetLinearDamping(MdtSpring6ID j,int axisindex,MeReal d)

{
  if ((uint)axisindex < 3) {
    j->linearDamping[axisindex] = d;
  }
  return;
}


/* ==== MdtSpring6SetAngularStiffness ==== */

void MdtSpring6SetAngularStiffness(MdtSpring6ID j,int axisindex,MeReal s)

{
  if ((uint)axisindex < 3) {
    j->angularStiffness[axisindex] = s;
  }
  return;
}


/* ==== MdtSpring6SetAngularDamping ==== */

void MdtSpring6SetAngularDamping(MdtSpring6ID j,int axisindex,MeReal d)

{
  if ((uint)axisindex < 3) {
    j->angularDamping[axisindex] = d;
  }
  return;
}


/* ==== MdtSpring6SetWorldLinearVelocity ==== */

void MdtSpring6SetWorldLinearVelocity(MdtSpring6ID j,MeReal vx,MeReal vy,MeReal vz)

{
  j->worldLinearVel[0] = vx;
  j->worldLinearVel[1] = vy;
  j->worldLinearVel[2] = vz;
  return;
}


/* ==== MdtSpring6SetWorldAngularVelocity ==== */

void MdtSpring6SetWorldAngularVelocity(MdtSpring6ID j,MeReal vx,MeReal vy,MeReal vz)

{
  j->worldAngularVel[0] = vx;
  j->worldAngularVel[1] = vy;
  j->worldAngularVel[2] = vz;
  return;
}


/* ==== Spring6SetBodies ==== */

void Spring6SetBodies(MdtConstraintID c,MdtBodyID b1,MdtBodyID b2)

{
  MeMatrix4 tm;
  
                    /* Unresolved local var: MdtSpring6ID j@[???] */
  BaseConstraintSetBodies(c,b1,b2);
  MdtBodyGetTransform(b1,tm);
  MdtConstraintSetAxis(c,tm[0][0],tm[0][1],tm[0][2]);
  return;
}


