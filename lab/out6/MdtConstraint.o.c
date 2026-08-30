/* ==== BaseConstraintReset ==== */

void BaseConstraintReset(MdtConstraintID c)

{
                    /* Unresolved local var: int i@[???] */
  (c->head).flags = 0;
  (c->head).userData = (void *)0x0;
  (c->head).bodyindex[1] = -1;
  (c->head).bodyindex[0] = -1;
  (c->head).resultForce[0][0] = 0.0;
  (c->head).resultTorque[0][0] = 0.0;
  (c->head).resultForce[1][0] = 0.0;
  (c->head).resultTorque[1][0] = 0.0;
  (c->head).resultForce[0][1] = 0.0;
  (c->head).resultTorque[0][1] = 0.0;
  (c->head).resultForce[1][1] = 0.0;
  (c->head).resultTorque[1][1] = 0.0;
  (c->head).resultForce[0][2] = 0.0;
  (c->head).resultTorque[0][2] = 0.0;
  (c->head).resultForce[1][2] = 0.0;
  (c->head).resultTorque[1][2] = 0.0;
  (c->head).resultForce[0][3] = 0.0;
  (c->head).resultTorque[0][3] = 0.0;
  (c->head).resultForce[1][3] = 0.0;
  (c->head).resultTorque[1][3] = 0.0;
  (c->head).worldAngVel[0] = 0.0;
  (c->head).worldLinVel[0] = 0.0;
  (c->head).worldAngVel[1] = 0.0;
  (c->head).worldLinVel[1] = 0.0;
  (c->head).worldAngVel[2] = 0.0;
  (c->head).worldLinVel[2] = 0.0;
  (c->head).ref1[0][0] = 1.0;
  (c->head).ref1[0][1] = 0.0;
  (c->head).ref1[0][2] = 0.0;
  (c->head).ref1[0][3] = 0.0;
  (c->head).ref1[1][0] = 0.0;
  (c->head).ref1[1][1] = 1.0;
  (c->head).ref1[1][2] = 0.0;
  (c->head).ref1[1][3] = 0.0;
  (c->head).ref1[2][0] = 0.0;
  (c->head).ref1[2][1] = 0.0;
  (c->head).ref1[2][2] = 1.0;
  (c->head).ref1[2][3] = 0.0;
  (c->head).ref1[3][0] = 0.0;
  (c->head).ref1[3][1] = 0.0;
  (c->head).ref1[3][2] = 0.0;
  (c->head).ref1[3][3] = 1.0;
  (c->head).ref2[0][0] = 1.0;
  (c->head).ref2[0][1] = 0.0;
  (c->head).ref2[0][2] = 0.0;
  (c->head).ref2[0][3] = 0.0;
  (c->head).ref2[1][0] = 0.0;
  (c->head).ref2[1][1] = 1.0;
  (c->head).ref2[1][2] = 0.0;
  (c->head).ref2[1][3] = 0.0;
  (c->head).ref2[2][0] = 0.0;
  (c->head).ref2[2][1] = 0.0;
  (c->head).ref2[2][2] = 1.0;
  (c->head).ref2[2][3] = 0.0;
  (c->head).ref2[3][0] = 0.0;
  (c->head).ref2[3][1] = 0.0;
  (c->head).ref2[3][2] = 0.0;
  (c->head).ref2[3][3] = 1.0;
  MeDictNodeInit(&(c->head).worldNode,(void *)(c->head).sortKey);
  (c->head).worldNode.data = c;
  MeDictNodeInit((c->head).bodyNode,(void *)(c->head).sortKey);
  (c->head).bodyNode[0].data = c;
  MeDictNodeInit((c->head).bodyNode + 1,(void *)(c->head).sortKey);
  (c->head).bodyNode[1].data = c;
  return;
}


/* ==== MdtConstraintBodySetAxes ==== */

void MdtConstraintBodySetAxes
               (MdtConstraintID c,uint bodyindex,MeReal px,MeReal py,MeReal pz,MeReal ox,MeReal oy,
               MeReal oz)

{
  float fVar1;
  MeReal (*paMVar2) [4];
  MdtBody *pMVar3;
  MeVector3 r_axis;
  MeVector3 oaxis;
  MeVector3 paxis;
  
                    /* Unresolved local var: MeReal magSqr@[???] */
                    /* Unresolved local var: MeReal m@[???] */
  oaxis[1] = oy;
  oaxis[2] = oz;
  paxis[0] = px;
  paxis[1] = py;
  paxis[2] = pz;
  oaxis[0] = ox;
  if (0.001 <= ABS((px * px + py * py + pz * pz) - 1.0)) {
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
    fVar1 = py * py + px * px + pz * pz;
    if (fVar1 <= 0.0) {
      paxis[0] = 1.0;
      paxis[1] = 0.0;
      paxis[2] = 0.0;
    }
    else {
      fVar1 = 1.0 / SQRT(fVar1);
      paxis[0] = px * fVar1;
      paxis[1] = py * fVar1;
      paxis[2] = fVar1 * pz;
    }
  }
                    /* Unresolved local var: MeReal m@[???] */
  if (0.001 <= ABS((ox * ox + oy * oy + oz * oz) - 1.0)) {
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal k@[DW_OP_reg14(ST3)] */
    fVar1 = ox * ox + oy * oy + oz * oz;
    if (fVar1 <= 0.0) {
      oaxis[0] = 1.0;
      oaxis[1] = 0.0;
      oaxis[2] = 0.0;
    }
    else {
      fVar1 = 1.0 / SQRT(fVar1);
      oaxis[0] = ox * fVar1;
      oaxis[1] = oy * fVar1;
      oaxis[2] = oz * fVar1;
    }
  }
  r_axis[0] = paxis[1] * oaxis[2] - paxis[2] * oaxis[1];
  r_axis[1] = paxis[2] * oaxis[0] - oaxis[2] * paxis[0];
  r_axis[2] = paxis[0] * oaxis[1] - paxis[1] * oaxis[0];
  if (bodyindex == 0) {
    ConvertCOMVector((void *)0x0,paxis,(c->head).mdtbody[0],(c->head).ref1);
    ConvertCOMVector((void *)0x0,oaxis,(c->head).mdtbody[0],(c->head).ref1 + 1);
    paMVar2 = (c->head).ref1;
    pMVar3 = (c->head).mdtbody[0];
  }
  else {
    ConvertCOMVector((void *)0x0,paxis,(c->head).mdtbody[1],(c->head).ref2);
    ConvertCOMVector((void *)0x0,oaxis,(c->head).mdtbody[1],(c->head).ref2 + 1);
    paMVar2 = (c->head).ref2;
    pMVar3 = (c->head).mdtbody[1];
  }
  ConvertCOMVector((void *)0x0,r_axis,pMVar3,paMVar2 + 2);
  return;
}


/* ==== MdtConstraintBodySetAxesRel ==== */

void MdtConstraintBodySetAxesRel
               (MdtConstraintID c,uint bodyindex,MeReal px,MeReal py,MeReal pz,MeReal ox,MeReal oy,
               MeReal oz)

{
  float fVar1;
  float fVar2;
  float fVar3;
  MeVector3 r_axis;
  MeVector3 oaxis;
  MeVector3 paxis;
  
                    /* Unresolved local var: MeReal magSqr@[???] */
                    /* Unresolved local var: MeReal m@[???] */
  paxis[0] = px;
  paxis[1] = py;
  paxis[2] = pz;
  if (0.001 <= ABS((px * px + py * py + pz * pz) - 1.0)) {
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
    fVar1 = py * py + px * px + pz * pz;
    if (fVar1 <= 0.0) {
      paxis[0] = 1.0;
      paxis[1] = 0.0;
      paxis[2] = 0.0;
    }
    else {
      fVar1 = 1.0 / SQRT(fVar1);
      paxis[1] = py * fVar1;
      paxis[0] = px * fVar1;
      paxis[2] = fVar1 * pz;
    }
  }
                    /* Unresolved local var: MeReal m@[???] */
  oaxis[0] = ox;
  oaxis[2] = oz;
  oaxis[1] = oy;
  if (0.001 <= ABS((ox * ox + oy * oy + oz * oz) - 1.0)) {
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal k@[DW_OP_reg14(ST3)] */
    fVar1 = oy * oy + ox * ox + oz * oz;
    if (fVar1 <= 0.0) {
      oaxis[0] = 1.0;
      oaxis[1] = 0.0;
      oaxis[2] = 0.0;
    }
    else {
      fVar1 = 1.0 / SQRT(fVar1);
      oaxis[1] = oy * fVar1;
      oaxis[2] = oz * fVar1;
      oaxis[0] = ox * fVar1;
    }
  }
  fVar2 = paxis[1] * oaxis[2] - paxis[2] * oaxis[1];
  fVar3 = paxis[2] * oaxis[0] - paxis[0] * oaxis[2];
  fVar1 = paxis[0] * oaxis[1] - paxis[1] * oaxis[0];
  if (bodyindex == 0) {
    (c->head).ref1[0][0] = paxis[0];
    (c->head).ref1[0][1] = paxis[1];
    (c->head).ref1[0][2] = paxis[2];
    (c->head).ref1[1][0] = oaxis[0];
    (c->head).ref1[1][1] = oaxis[1];
    (c->head).ref1[1][2] = oaxis[2];
    (c->head).ref1[2][0] = fVar2;
    (c->head).ref1[2][1] = fVar3;
    (c->head).ref1[2][2] = fVar1;
  }
  else {
    (c->head).ref2[0][0] = paxis[0];
    (c->head).ref2[0][1] = paxis[1];
    (c->head).ref2[0][2] = paxis[2];
    (c->head).ref2[1][0] = oaxis[0];
    (c->head).ref2[1][1] = oaxis[1];
    (c->head).ref2[1][2] = oaxis[2];
    (c->head).ref2[2][0] = fVar2;
    (c->head).ref2[2][1] = fVar3;
    (c->head).ref2[2][2] = fVar1;
  }
  return;
}


/* ==== BaseConstraintCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MdtConstraintID BaseConstraintCreate(MdtWorldID w)

{
  MdtConstraintID pMVar1;
  MdtConstraintID pMVar2;
  
                    /* Unresolved local var: MdtConstraintID c@[DW_OP_reg0(EAX)] */
  pMVar1 = (MdtConstraintID)(*_MdtConstraintDCastContactGroup)(&w->constraintPool);
  pMVar2 = (MdtConstraintID)0x0;
  if (pMVar1 != (MdtConstraintID)0x0) {
    (pMVar1->head).mdtbody[1] = (MdtBody *)0x0;
    (pMVar1->head).mdtbody[0] = (MdtBody *)0x0;
    (pMVar1->head).bclFunction = (MdtBclAddConstraintFn)0x0;
    (pMVar1->head).maxRows = 0;
    (pMVar1->head).world = w;
    (pMVar1->head).flags = 0;
    (pMVar1->head).sortKey = 0;
    pMVar2 = pMVar1;
  }
  return pMVar2;
}


/* ==== BaseConstraintSetBodies ==== */

void BaseConstraintSetBodies(MdtConstraintID c,MdtBodyID b1,MdtBodyID b2)

{
  if (((c->head).flags & 1) == 0) {
    (c->head).mdtbody[0] = b1;
    (c->head).mdtbody[1] = b2;
  }
  return;
}


/* ==== BaseConstraintSetAxis ==== */

void BaseConstraintSetAxis(MdtConstraintID c,MeReal px,MeReal py,MeReal pz)

{
  MeVector3 r_axis2;
  MeVector3 r_axis1;
  MeVector3 axis;
  
                    /* Unresolved local var: MeReal magSqr@[???]
                       Unresolved local var: MeReal m@[???] */
  axis[0] = px;
  axis[1] = py;
  axis[2] = pz;
  if (0.001 <= ABS((px * px + py * py + pz * pz) - 1.0)) {
    MeVector3Normalize(axis);
  }
  MeVector3PlaneSpace(axis,r_axis1,r_axis2);
  ConvertCOMVector((void *)0x0,axis,(c->head).mdtbody[0],(c->head).ref1);
  ConvertCOMVector((void *)0x0,r_axis1,(c->head).mdtbody[0],(c->head).ref1 + 1);
  ConvertCOMVector((void *)0x0,r_axis2,(c->head).mdtbody[0],(c->head).ref1 + 2);
  ConvertCOMVector((void *)0x0,axis,(c->head).mdtbody[1],(c->head).ref2);
  ConvertCOMVector((void *)0x0,r_axis1,(c->head).mdtbody[1],(c->head).ref2 + 1);
  ConvertCOMVector((void *)0x0,r_axis2,(c->head).mdtbody[1],(c->head).ref2 + 2);
  return;
}


/* ==== MdtConstraintDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MdtConstraintDestroy(MdtConstraintID c)

{
  uint uVar1;
  MdtBody *pMVar2;
  MdtWorldID pMVar3;
  MeDictNode *unaff_ESI;
  
  uVar1 = (c->head).flags;
  if ((uVar1 & 1) != 0) {
                    /* Unresolved local var: int i@[???] */
    (c->head).flags = uVar1 & 0xfffffffe;
    MeDictDelete(&((c->head).world)->constraintDict,&(c->head).worldNode);
    pMVar3 = (c->head).world;
    pMVar3->nEnabledConstraints = pMVar3->nEnabledConstraints + -1;
    unaff_ESI = (c->head).bodyNode;
    pMVar2 = (c->head).mdtbody[0];
    if (pMVar2 != (MdtBody *)0x0) {
      MeDictDelete(&pMVar2->constraintDict,unaff_ESI);
    }
    pMVar2 = (c->head).mdtbody[1];
    if (pMVar2 != (MdtBody *)0x0) {
      MeDictDelete(&pMVar2->constraintDict,(c->head).bodyNode + 1);
    }
  }
  if ((c->head).tag == 5) {
    pMVar3 = (c->head).world;
    if (pMVar3->contactGroupDestroyCallback == (MdtContactGroupDestroyCallbackCBPtr)0x0)
    goto LAB_0001089d;
    (*pMVar3->contactGroupDestroyCallback)((MdtContactGroupID_conflict)c);
  }
  pMVar3 = (c->head).world;
LAB_0001089d:
  (c->head).tag = -1;
  (*_ConvertCOMPositionVector)(&pMVar3->constraintPool,c,unaff_ESI,unaff_ESI);
  return;
}


/* ==== MdtConstraintEnable ==== */

void MdtConstraintEnable(MdtConstraintID c)

{
  uint uVar1;
  MdtWorldID pMVar2;
  MdtBody *pMVar3;
  
                    /* Unresolved local var: MdtWorldID w@[DW_OP_reg6(ESI)]
                       Unresolved local var: int i@[???] */
  uVar1 = (c->head).flags;
  pMVar2 = (c->head).world;
  if (((uVar1 & 1) == 0) &&
     (((c->head).mdtbody[0] != (MdtBody *)0x0 || ((c->head).mdtbody[1] != (MdtBody *)0x0)))) {
    (c->head).flags = uVar1 | 1;
    MeDictInsert(&pMVar2->constraintDict,&(c->head).worldNode,(void *)(c->head).sortKey);
    pMVar2->nEnabledConstraints = pMVar2->nEnabledConstraints + 1;
    pMVar3 = (c->head).mdtbody[0];
    if (pMVar3 != (MdtBody *)0x0) {
      MeDictInsert(&pMVar3->constraintDict,(c->head).bodyNode,(void *)(c->head).sortKey);
    }
    pMVar3 = (c->head).mdtbody[1];
    if (pMVar3 != (MdtBody *)0x0) {
      MeDictInsert(&pMVar3->constraintDict,(c->head).bodyNode + 1,(void *)(c->head).sortKey);
    }
  }
  return;
}


/* ==== MdtConstraintDisable ==== */

void MdtConstraintDisable(MdtConstraintID c)

{
  uint uVar1;
  MdtWorldID pMVar2;
  MdtBody *pMVar3;
  
                    /* Unresolved local var: int i@[???] */
  uVar1 = (c->head).flags;
  if ((uVar1 & 1) != 0) {
    (c->head).flags = uVar1 & 0xfffffffe;
    MeDictDelete(&((c->head).world)->constraintDict,&(c->head).worldNode);
    pMVar2 = (c->head).world;
    pMVar2->nEnabledConstraints = pMVar2->nEnabledConstraints + -1;
    pMVar3 = (c->head).mdtbody[0];
    if (pMVar3 != (MdtBody *)0x0) {
      MeDictDelete(&pMVar3->constraintDict,(c->head).bodyNode);
    }
    pMVar3 = (c->head).mdtbody[1];
    if (pMVar3 != (MdtBody *)0x0) {
      MeDictDelete(&pMVar3->constraintDict,(c->head).bodyNode + 1);
    }
  }
  return;
}


/* ==== MdtConstraintIsEnabled ==== */

MeBool MdtConstraintIsEnabled(MdtConstraintID c)

{
  return (c->head).flags & 1;
}


/* ==== MdtConstraintGetWorld ==== */

MdtWorldID MdtConstraintGetWorld(MdtConstraintID c)

{
  return (c->head).world;
}


/* ==== MdtConstraintGetRowCount ==== */

int MdtConstraintGetRowCount(MdtConstraintID c)

{
  int iVar1;
  void *pvVar2;
  int iVar3;
  
                    /* Unresolved local var: int rows@[DW_OP_reg3(EBX)]
                       Unresolved local var: MdtContactGroupID.conflict group@[DW_OP_reg0(EAX)] */
  iVar3 = 0;
  pvVar2 = MdtConstraintDCastContactGroup(c);
  if (pvVar2 == (void *)0x0) {
    iVar3 = (c->head).maxRows;
  }
  else {
                    /* Unresolved local var: MdtContact * contact@[DW_OP_reg0(EAX)] */
    for (iVar1 = *(int *)((int)pvVar2 + 0x164); iVar1 != 0; iVar1 = *(int *)(iVar1 + 0x1dc)) {
      if (*(int *)(iVar1 + 0x194) == 0) {
        iVar3 = iVar3 + 1;
      }
      else if (*(int *)(iVar1 + 0x194) == 1) {
        iVar3 = iVar3 + 2;
      }
      else {
        iVar3 = iVar3 + 3;
      }
    }
  }
  return iVar3;
}


/* ==== MdtConstraintGetBody ==== */

MdtBodyID MdtConstraintGetBody(MdtConstraintID c,uint bodyindex)

{
  return (c->head).mdtbody[bodyindex];
}


/* ==== MdtConstraintGetUserData ==== */

void * MdtConstraintGetUserData(MdtConstraintID c)

{
  return (c->head).userData;
}


/* ==== MdtConstraintGetForce ==== */

void MdtConstraintGetForce(MdtConstraintID c,uint bodyindex,MeReal *v)

{
  *v = (c->head).resultForce[bodyindex][0];
  v[1] = (c->head).resultForce[bodyindex][1];
  v[2] = (c->head).resultForce[bodyindex][2];
  return;
}


/* ==== MdtConstraintGetTorque ==== */

void MdtConstraintGetTorque(MdtConstraintID c,uint bodyindex,MeReal *v)

{
  *v = (c->head).resultTorque[bodyindex][0];
  v[1] = (c->head).resultTorque[bodyindex][1];
  v[2] = (c->head).resultTorque[bodyindex][2];
  return;
}


/* ==== MdtConstraintGetPosition ==== */

void MdtConstraintGetPosition(MdtConstraintID c,MeReal *p)

{
  ConvertCOMPositionVector((c->head).mdtbody[1],(c->head).ref2 + 3,(void *)0x0,p);
  return;
}


/* ==== MdtConstraintBodyGetPosition ==== */

void MdtConstraintBodyGetPosition(MdtConstraintID c,uint bodyindex,MeReal *a)

{
  MeReal (*paMVar1) [4];
  MdtBody *pMVar2;
  
  if (bodyindex == 0) {
    paMVar1 = (c->head).ref1;
    pMVar2 = (c->head).mdtbody[0];
  }
  else {
    paMVar1 = (c->head).ref2;
    pMVar2 = (c->head).mdtbody[1];
  }
  ConvertCOMPositionVector(pMVar2,paMVar1 + 3,(void *)0x0,a);
  return;
}


/* ==== MdtConstraintBodyGetPositionRel ==== */

void MdtConstraintBodyGetPositionRel(MdtConstraintID c,uint bodyindex,MeReal *a)

{
  MdtBody *pMVar1;
  MeReal (*paMVar2) [4];
  
                    /* Unresolved local var: MdtBodyID body@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * cpos@[DW_OP_reg1(ECX)] */
  pMVar1 = (c->head).mdtbody[bodyindex];
  if (bodyindex == 0) {
    paMVar2 = (c->head).ref1;
  }
  else {
    paMVar2 = (c->head).ref2;
  }
  paMVar2 = paMVar2 + 3;
  if ((pMVar1 == (MdtBody *)0x0) || (pMVar1->useCom == 0)) {
    *a = (*paMVar2)[0];
    a[1] = (*paMVar2)[1];
    a[2] = (*paMVar2)[2];
  }
  else {
    *a = pMVar1->com[0] + (*paMVar2)[0];
    a[1] = pMVar1->com[1] + (*paMVar2)[1];
    a[2] = pMVar1->com[2] + (*paMVar2)[2];
  }
  return;
}


/* ==== MdtConstraintBodyGetAxis ==== */

void MdtConstraintBodyGetAxis(MdtConstraintID c,uint bodyindex,MeReal *a)

{
  MdtBody *pMVar1;
  MeReal (*paMVar2) [4];
  
  if (bodyindex == 0) {
    paMVar2 = (c->head).ref1;
    pMVar1 = (c->head).mdtbody[0];
  }
  else {
    paMVar2 = (c->head).ref2;
    pMVar1 = (c->head).mdtbody[1];
  }
  ConvertCOMVector(pMVar1,paMVar2,(void *)0x0,a);
  return;
}


/* ==== MdtConstraintBodyGetAxes ==== */

void MdtConstraintBodyGetAxes(MdtConstraintID c,uint bodyindex,MeReal *p,MeReal *o)

{
  MeReal (*paMVar1) [4];
  MdtBody *pMVar2;
  
  if (bodyindex == 0) {
    ConvertCOMVector((c->head).mdtbody[0],(c->head).ref1,(void *)0x0,p);
    paMVar1 = (c->head).ref1;
    pMVar2 = (c->head).mdtbody[0];
  }
  else {
    ConvertCOMVector((c->head).mdtbody[1],(c->head).ref2,(void *)0x0,p);
    paMVar1 = (c->head).ref2;
    pMVar2 = (c->head).mdtbody[1];
  }
  bodyindex = (uint)(paMVar1 + 1);
  ConvertCOMVector(pMVar2,(void *)bodyindex,(void *)0x0,o);
  return;
}


/* ==== MdtConstraintBodyGetAxesRel ==== */

void MdtConstraintBodyGetAxesRel(MdtConstraintID c,uint bodyindex,MeReal *p,MeReal *o)

{
  MeReal MVar1;
  
  if (bodyindex == 0) {
    *p = (c->head).ref1[0][0];
    p[1] = (c->head).ref1[0][1];
    p[2] = (c->head).ref1[0][2];
    *o = (c->head).ref1[1][0];
    o[1] = (c->head).ref1[1][1];
    MVar1 = (c->head).ref1[1][2];
  }
  else {
    *p = (c->head).ref2[0][0];
    p[1] = (c->head).ref2[0][1];
    p[2] = (c->head).ref2[0][2];
    *o = (c->head).ref2[1][0];
    o[1] = (c->head).ref2[1][1];
    MVar1 = (c->head).ref2[1][2];
  }
  o[2] = MVar1;
  return;
}


/* ==== MdtConstraintGetAxes ==== */

void MdtConstraintGetAxes(MdtConstraintID c,MeReal *p,MeReal *o)

{
  ConvertCOMVector((c->head).mdtbody[1],(c->head).ref2,(void *)0x0,p);
  ConvertCOMVector((c->head).mdtbody[1],(c->head).ref2 + 1,(void *)0x0,o);
  return;
}


/* ==== MdtConstraintGetFirst ==== */

MdtConstraintID MdtConstraintGetFirst(MdtWorldID w)

{
  void *pvVar1;
  MdtConstraintID pMVar2;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg0(EAX)] */
  pvVar1 = MeDictFirst(&w->constraintDict);
  pMVar2 = (MdtConstraintID)0x0;
  if (pvVar1 != (void *)0x0) {
    pMVar2 = *(MdtConstraintID *)((int)pvVar1 + 0x14);
  }
  return pMVar2;
}


/* ==== MdtConstraintGetNext ==== */

MdtConstraintID MdtConstraintGetNext(MdtConstraintID c)

{
  void *pvVar1;
  MdtConstraintID pMVar2;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg0(EAX)] */
  pvVar1 = MeDictNext(&((c->head).world)->constraintDict,&(c->head).worldNode);
  pMVar2 = (MdtConstraintID)0x0;
  if (pvVar1 != (void *)0x0) {
    pMVar2 = *(MdtConstraintID *)((int)pvVar1 + 0x14);
  }
  return pMVar2;
}


/* ==== MdtConstraintGetSortKey ==== */

MeI32 MdtConstraintGetSortKey(MdtConstraintID c)

{
  return (c->head).sortKey;
}


/* ==== MdtConstraintGetWorldLinearVelocity ==== */

void MdtConstraintGetWorldLinearVelocity(MdtConstraintID c,MeReal *lv)

{
  *lv = (c->head).worldLinVel[0];
  lv[1] = (c->head).worldLinVel[1];
  lv[2] = (c->head).worldLinVel[2];
  return;
}


/* ==== MdtConstraintGetWorldAngularVelocity ==== */

void MdtConstraintGetWorldAngularVelocity(MdtConstraintID c,MeReal *av)

{
  *av = (c->head).worldAngVel[0];
  av[1] = (c->head).worldAngVel[1];
  av[2] = (c->head).worldAngVel[2];
  return;
}


/* ==== MdtConstraintSetBodies ==== */

void MdtConstraintSetBodies(MdtConstraintID c,MdtBodyID b1,MdtBodyID b2)

{
  (*(c->head).setBodyFunc)(c,b1,b2);
  return;
}


/* ==== MdtConstraintSetUserData ==== */

void MdtConstraintSetUserData(MdtConstraintID c,void *d)

{
  (c->head).userData = d;
  return;
}


/* ==== MdtConstraintBodySetPosition ==== */

void MdtConstraintBodySetPosition(MdtConstraintID c,uint bodyindex,MeReal x,MeReal y,MeReal z)

{
  MeReal (*paMVar1) [4];
  MdtBody *pMVar2;
  MeVector3 pos;
  
  pos[0] = x;
  pos[1] = y;
  pos[2] = z;
  if (bodyindex == 0) {
    paMVar1 = (c->head).ref1;
    pMVar2 = (c->head).mdtbody[0];
  }
  else {
    paMVar1 = (c->head).ref2;
    pMVar2 = (c->head).mdtbody[1];
  }
  ConvertCOMPositionVector((void *)0x0,pos,pMVar2,paMVar1 + 3);
  return;
}


/* ==== MdtConstraintBodySetPositionRel ==== */

void MdtConstraintBodySetPositionRel(MdtConstraintID c,uint bodyindex,MeReal x,MeReal y,MeReal z)

{
  MdtBody *pMVar1;
  MeReal (*paMVar2) [4];
  
                    /* Unresolved local var: MdtBodyID body@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeVector3Ptr cpos@[DW_OP_reg2(EDX)] */
  pMVar1 = (c->head).mdtbody[bodyindex];
  if (bodyindex == 0) {
    paMVar2 = (c->head).ref1;
  }
  else {
    paMVar2 = (c->head).ref2;
  }
  paMVar2 = paMVar2 + 3;
  (*paMVar2)[0] = x;
  (*paMVar2)[1] = y;
  (*paMVar2)[2] = z;
  if ((pMVar1 != (MdtBody *)0x0) && (pMVar1->useCom != 0)) {
    (*paMVar2)[0] = x - pMVar1->com[0];
    (*paMVar2)[1] = y - pMVar1->com[1];
    (*paMVar2)[2] = z - pMVar1->com[2];
  }
  return;
}


/* ==== MdtConstraintSetPosition ==== */

void MdtConstraintSetPosition(MdtConstraintID c,MeReal x,MeReal y,MeReal z)

{
  MeVector3 pos_1;
  MeVector3 pos;
  
  pos[0] = x;
  pos[2] = z;
  pos[1] = y;
  ConvertCOMPositionVector((void *)0x0,pos,(c->head).mdtbody[0],(c->head).ref1 + 3);
  pos_1[0] = x;
  pos_1[1] = y;
  pos_1[2] = z;
  ConvertCOMPositionVector((void *)0x0,pos_1,(c->head).mdtbody[1],(c->head).ref2 + 3);
  return;
}


/* ==== MdtConstraintSetAxis ==== */

void MdtConstraintSetAxis(MdtConstraintID c,MeReal px,MeReal py,MeReal pz)

{
  (*(c->head).setAxisFunc)(c,px,py,pz);
  return;
}


/* ==== MdtConstraintSetAxes ==== */

void MdtConstraintSetAxes
               (MdtConstraintID c,MeReal px,MeReal py,MeReal pz,MeReal ox,MeReal oy,MeReal oz)

{
  MdtConstraintBodySetAxes(c,0,px,py,pz,ox,oy,oz);
  MdtConstraintBodySetAxes(c,1,px,py,pz,ox,oy,oz);
  return;
}


/* ==== MdtConstraintSetSortKey ==== */

void MdtConstraintSetSortKey(MdtConstraintID c,MeI32 key)

{
  if (((c->head).flags & 1) == 0) {
    (c->head).sortKey = key;
  }
  return;
}


/* ==== MdtConstraintSetWorldLinearVelocity ==== */

void MdtConstraintSetWorldLinearVelocity(MdtConstraintID c,MeReal vx,MeReal vy,MeReal vz)

{
  (c->head).worldLinVel[0] = vx;
  (c->head).worldLinVel[1] = vy;
  (c->head).worldLinVel[2] = vz;
  return;
}


/* ==== MdtConstraintSetWorldAngularVelocity ==== */

void MdtConstraintSetWorldAngularVelocity(MdtConstraintID c,MeReal ax,MeReal ay,MeReal az)

{
  (c->head).worldAngVel[0] = ax;
  (c->head).worldAngVel[1] = ay;
  (c->head).worldAngVel[2] = az;
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


/* ==== MeVector3PlaneSpace ==== */

void MeVector3PlaneSpace(MeReal *n,MeReal *a,MeReal *b)

{
  float fVar1;
  float fVar2;
  
  fVar1 = *n;
  fVar2 = n[1];
  if (ABS(fVar1) <= ABS(fVar2)) {
                    /* Unresolved local var: float __result@[???] */
    fVar1 = 1.0 / SQRT(n[2] * n[2] + fVar2 * fVar2);
    *a = 0.0;
    fVar2 = -n[2] * fVar1;
    a[1] = fVar2;
    fVar1 = fVar1 * n[1];
    a[2] = fVar1;
    *b = fVar1 * n[1] - fVar2 * n[2];
    b[1] = -*n * a[2];
    fVar1 = a[1] * *n;
  }
  else {
                    /* Unresolved local var: MeReal scale@[DW_OP_reg12(ST1)]
                       Unresolved local var: float __result@[???] */
    fVar2 = n[2];
    a[1] = 0.0;
    fVar1 = 1.0 / SQRT(fVar2 * fVar2 + fVar1 * fVar1);
    *a = fVar2 * fVar1;
    fVar1 = -*n * fVar1;
    a[2] = fVar1;
    *b = fVar1 * n[1];
    b[1] = *a * n[2] - a[2] * *n;
    fVar1 = -n[1] * *a;
  }
                    /* Unresolved local var: MeReal scale@[DW_OP_reg11(ST0)] */
  b[2] = fVar1;
  return;
}


