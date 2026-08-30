/* ==== MdtAngular3Create ==== */

/* WARNING: Unknown calling convention */

MdtAngular3ID MdtAngular3Create(MdtWorldID w)

{
  MdtAngular3ID pMVar1;
  int iVar2;
  MdtAngular3ID pMVar3;
  
                    /* Unresolved local var: MdtAngular3 * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtAngular3ID)0x0;
  if (pMVar1 != (MdtAngular3ID)0x0) {
    (pMVar1->head).tag = 0xc;
    (pMVar1->head).bclFunction = MdtBclAddAngular3;
    (pMVar1->head).maxRows = 3;
    (pMVar1->head).setBodyFunc = Angular3SetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    (pMVar1->head).bodyindex[1] = 0;
    (pMVar1->head).bodyindex[0] = 0;
    pMVar1->bEnableRotation = 0;
    pMVar1->stiffness = 3.4028235e+38;
    pMVar1->damping = 3.4028235e+38;
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtAngular3Reset ==== */

void MdtAngular3Reset(MdtAngular3ID j)

{
  int iVar1;
  
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  (j->head).bodyindex[1] = 0;
  (j->head).bodyindex[0] = 0;
  j->bEnableRotation = 0;
  j->stiffness = 3.4028235e+38;
  j->damping = 3.4028235e+38;
  BaseConstraintReset(j);
  return;
}


/* ==== MdtAngular3QuaConstraint ==== */

MdtConstraintID MdtAngular3QuaConstraint(MdtAngular3ID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastAngular3 ==== */

MdtAngular3ID MdtConstraintDCastAngular3(MdtConstraintID c)

{
  return (MdtAngular3ID)(((c->head).tag != 0xc) - 1 & (uint)c);
}


/* ==== MdtAngular3GetStiffness ==== */

MeReal MdtAngular3GetStiffness(MdtAngular3ID j)

{
  return j->stiffness;
}


/* ==== MdtAngular3GetDamping ==== */

MeReal MdtAngular3GetDamping(MdtAngular3ID j)

{
  return j->damping;
}


/* ==== MdtAngular3RotationIsEnabled ==== */

MeBool MdtAngular3RotationIsEnabled(MdtAngular3ID j)

{
  return j->bEnableRotation;
}


/* ==== Angular3SetBodies ==== */

void Angular3SetBodies(MdtConstraintID c,MdtBodyID b1,MdtBodyID b2)

{
  int iVar1;
  MeMatrix4 tm;
  MeVector3 v;
  
                    /* Unresolved local var: MdtAngular3ID j@[???] */
  iVar1 = (c->head).tag;
  BaseConstraintSetBodies(c,b1,b2);
  MdtBodyGetTransform(b1,tm);
  MdtConstraintSetAxis((void *)((iVar1 != 0xc) - 1 & (uint)c),tm[0][0],tm[0][1],tm[0][2]);
  return;
}


/* ==== MdtAngular3EnableRotation ==== */

void MdtAngular3EnableRotation(MdtAngular3ID j,MeBool NewRotationState)

{
  j->bEnableRotation = NewRotationState;
  return;
}


/* ==== MdtAngular3SetStiffness ==== */

void MdtAngular3SetStiffness(MdtAngular3ID j,MeReal s)

{
  j->stiffness = s;
  return;
}


/* ==== MdtAngular3SetDamping ==== */

void MdtAngular3SetDamping(MdtAngular3ID j,MeReal d)

{
  j->damping = d;
  return;
}


