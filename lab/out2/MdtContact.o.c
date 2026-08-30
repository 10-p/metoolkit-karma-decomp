/* ==== MdtContactCreate ==== */

/* WARNING: Unknown calling convention */

MdtContactID_conflict MdtContactCreate(MdtWorldID w)

{
  MdtContactID_conflict pMVar1;
  int iVar2;
  MdtContactID_conflict pMVar3;
  
                    /* Unresolved local var: MdtContact * c@[DW_OP_reg3(EBX)] */
  pMVar1 = (MdtContactID_conflict)BaseConstraintCreate(w);
  pMVar3 = (MdtContactID_conflict)0x0;
  if (pMVar1 != (MdtContactID_conflict)0x0) {
                    /* Unresolved local var: int i@[???] */
    (pMVar1->head).tag = 4;
    (pMVar1->head).bclFunction = (MdtBclAddConstraintFn)&MdtBclAddContact;
    (pMVar1->head).maxRows = 3;
    pMVar1->nextContact = (MdtContactID)0x0;
    pMVar1->prevContact = (MdtContactID)0x0;
    (pMVar1->head).setBodyFunc = (MdtConstraintSetBodyFnPtr)&BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = (MdtConstraintSetAxisFnPtr)&BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    pMVar1->penetration = 0.0;
    pMVar1->cpos[0] = 0.0;
    pMVar1->normal[0] = 0.0;
    pMVar1->worldVel[0] = 0.0;
    pMVar1->cpos[1] = 0.0;
    pMVar1->direction[1] = 0.0;
    pMVar1->worldVel[1] = 0.0;
    pMVar1->cpos[2] = 0.0;
    pMVar1->normal[2] = 0.0;
    pMVar1->direction[2] = 0.0;
    pMVar1->worldVel[2] = 0.0;
    pMVar1->direction[0] = 1.0;
    pMVar1->normal[1] = 1.0;
    pMVar1->nextContact = (MdtContactID)0x0;
    pMVar1->prevContact = (MdtContactID)0x0;
    pMVar1->contactGroup = (MdtContactGroupID)0x0;
    pMVar1->importance = 3.4028235e+38;
    MdtContactParamsReset(&pMVar1->params);
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtContactReset ==== */

void MdtContactReset(MdtContactID_conflict c)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[???] */
  iVar1 = MdtConstraintIsEnabled(c);
  if (iVar1 != 0) {
    MdtConstraintDisable(c);
  }
  c->penetration = 0.0;
  c->cpos[0] = 0.0;
  c->normal[0] = 0.0;
  c->worldVel[0] = 0.0;
  c->cpos[1] = 0.0;
  c->direction[1] = 0.0;
  c->worldVel[1] = 0.0;
  c->cpos[2] = 0.0;
  c->normal[2] = 0.0;
  c->direction[2] = 0.0;
  c->worldVel[2] = 0.0;
  c->direction[0] = 1.0;
  c->normal[1] = 1.0;
  c->nextContact = (MdtContactID)0x0;
  c->prevContact = (MdtContactID)0x0;
  c->contactGroup = (MdtContactGroupID)0x0;
  c->importance = 3.4028235e+38;
  MdtContactParamsReset(&c->params);
  BaseConstraintReset();
  return;
}


/* ==== MdtConstraintDCastContact ==== */

MdtContactID_conflict MdtConstraintDCastContact(MdtConstraintID c)

{
  return (MdtContactID_conflict)(((c->head).tag != 4) - 1 & (uint)c);
}


/* ==== MdtContactGetPosition ==== */

void MdtContactGetPosition(MdtContactID_conflict c,MeReal *v)

{
  *v = c->cpos[0];
  v[1] = c->cpos[1];
  v[2] = c->cpos[2];
  return;
}


/* ==== MdtContactGetNormal ==== */

void MdtContactGetNormal(MdtContactID_conflict c,MeReal *v)

{
  *v = c->normal[0];
  v[1] = c->normal[1];
  v[2] = c->normal[2];
  return;
}


/* ==== MdtContactGetPenetration ==== */

MeReal MdtContactGetPenetration(MdtContactID_conflict c)

{
  return c->penetration;
}


/* ==== MdtContactGetDirection ==== */

void MdtContactGetDirection(MdtContactID_conflict c,MeReal *v)

{
  *v = c->direction[0];
  v[1] = c->direction[1];
  v[2] = c->direction[2];
  return;
}


/* ==== MdtContactGetWorldVelocity ==== */

void MdtContactGetWorldVelocity(MdtContactID_conflict c,MeReal *v)

{
  *v = c->worldVel[0];
  v[1] = c->worldVel[1];
  v[2] = c->worldVel[2];
  return;
}


/* ==== MdtContactGetParams ==== */

/* WARNING: Unknown calling convention */

MdtContactParamsID MdtContactGetParams(MdtContactID_conflict c)

{
  return &c->params;
}


/* ==== MdtContactGetNext ==== */

MdtContactID_conflict MdtContactGetNext(MdtContactID_conflict c)

{
  return c->nextContact;
}


/* ==== MdtContactGetContactGroup ==== */

MdtContactGroupID_conflict MdtContactGetContactGroup(MdtContactID_conflict c)

{
  return c->contactGroup;
}


/* ==== MdtContactGetRelativeVelocity ==== */

/* WARNING: Unknown calling convention */

void MdtContactGetRelativeVelocity(MdtContactID_conflict c,MeReal *v)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 extraout_EDX;
  MeVector3 vel1;
  MeVector3 vel0;
  MeVector3 pos1;
  MeVector3 pos0;
  
                    /* Unresolved local var: MdtBody * b0@[DW_OP_reg3(EBX)]
                       Unresolved local var: MdtBody * b1@[DW_OP_reg7(EDI)] */
  uVar1 = MdtConstraintGetBody(c,0);
  iVar2 = MdtConstraintGetBody(c,1);
  MdtBodyGetCenterOfMassPosition(uVar1,pos0);
  pos0[0] = c->cpos[0] - pos0[0];
  pos0[1] = c->cpos[1] - pos0[1];
  pos0[2] = c->cpos[2] - pos0[2];
  MdtBodyGetVelocityAtPoint(uVar1,c->cpos,vel0);
  if (iVar2 == 0) {
    vel1[2] = 0.0;
    vel1[1] = 0.0;
    vel1[0] = 0.0;
  }
  else {
    MdtBodyGetCenterOfMassPosition(iVar2,pos1,extraout_EDX,extraout_EDX);
    pos1[0] = c->cpos[0] - pos1[0];
    pos1[1] = c->cpos[1] - pos1[1];
    pos1[2] = c->cpos[2] - pos1[2];
    MdtBodyGetVelocityAtPoint(iVar2,c->cpos,vel1);
  }
  *v = vel0[0] - vel1[0];
  v[1] = vel0[1] - vel1[1];
  v[2] = vel0[2] - vel1[2];
  return;
}


/* ==== MdtContactSetPosition ==== */

void MdtContactSetPosition(MdtContactID_conflict c,MeReal x,MeReal y,MeReal z)

{
  c->cpos[0] = x;
  c->cpos[1] = y;
  c->cpos[2] = z;
  return;
}


/* ==== MdtContactSetNormal ==== */

void MdtContactSetNormal(MdtContactID_conflict c,MeReal x,MeReal y,MeReal z)

{
  c->normal[0] = x;
  c->normal[1] = y;
  c->normal[2] = z;
  return;
}


/* ==== MdtContactSetPenetration ==== */

void MdtContactSetPenetration(MdtContactID_conflict c,MeReal p)

{
  c->penetration = p;
  return;
}


/* ==== MdtContactSetParams ==== */

void MdtContactSetParams(MdtContactID_conflict c,MdtContactParamsID p)

{
  int iVar1;
  MdtBclContactParams *pMVar2;
  
  pMVar2 = &c->params;
  for (iVar1 = 0x12; iVar1 != 0; iVar1 = iVar1 + -1) {
    pMVar2->type = p->type;
    p = (MdtContactParamsID)&p->model1;
    pMVar2 = (MdtBclContactParams *)&pMVar2->model1;
  }
  return;
}


/* ==== MdtContactSetNext ==== */

void MdtContactSetNext(MdtContactID_conflict c,MdtContactID_conflict nc)

{
  c->nextContact = nc;
  return;
}


/* ==== MdtContactSetDirection ==== */

void MdtContactSetDirection(MdtContactID_conflict c,MeReal x,MeReal y,MeReal z)

{
  c->direction[0] = x;
  c->direction[1] = y;
  c->direction[2] = z;
  (c->params).options = (c->params).options | 1;
  return;
}


/* ==== MdtContactSetWorldVelocity ==== */

void MdtContactSetWorldVelocity(MdtContactID_conflict c,MeReal x,MeReal y,MeReal z)

{
  c->worldVel[1] = y;
  c->worldVel[2] = z;
                    /* Unresolved local var: MeReal m@[DW_OP_reg12(ST1)] */
  c->worldVel[0] = x;
  if (c->worldVel[2] * c->worldVel[2] + c->worldVel[1] * c->worldVel[1] + x * x <= 1e-12) {
    (c->params).options = (c->params).options & 0xfffffeff;
  }
  else {
    (c->params).options = (c->params).options | 0x100;
  }
  return;
}


