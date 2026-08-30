/* ==== MdtLinear2SetDirection ==== */

void MdtLinear2SetDirection(MdtLinear2ID j,MeReal x,MeReal y,MeReal z)

{
  MdtBody *pMVar1;
  MeVector3 vector2;
  MeVector3 vector1;
  MeVector3 direction;
  
  direction[0] = x;
  direction[1] = y;
  direction[2] = z;
  if ((j->head).mdtbody[1] == (MdtBody *)0x0) {
    j->direction[2] = z;
    j->direction[0] = x;
    j->direction[1] = y;
    pMVar1 = (MdtBody *)0x0;
  }
  else {
    ConvertCOMVector((void *)0x0,direction,(j->head).mdtbody[1],j->direction);
    pMVar1 = (j->head).mdtbody[1];
  }
  vector1[1] = 0.0;
  vector1[2] = 0.0;
  if (x == 0.0) {
    vector1[0] = 1.0;
  }
  else if (y == 0.0) {
    vector1[0] = 0.0;
    vector1[1] = 1.0;
  }
  else {
    vector1[0] = 1.0 / x;
    vector1[1] = -(1.0 / y);
  }
  vector2[0] = direction[1] * 0.0 - direction[2] * vector1[1];
  vector2[1] = direction[2] * vector1[0] - direction[0] * 0.0;
  vector2[2] = direction[0] * vector1[1] - direction[1] * vector1[0];
  if (pMVar1 == (MdtBody *)0x0) {
                    /* Unresolved local var: uint i@[???] */
    j->vec1[0] = vector1[0];
    j->vec2[0] = vector2[0];
    j->vec1[1] = vector1[1];
    j->vec2[1] = vector2[1];
    j->vec1[2] = 0.0;
    j->vec2[2] = vector2[2];
  }
  else {
    ConvertCOMVector((void *)0x0,vector1,(j->head).mdtbody[1],j->vec1);
    ConvertCOMVector((void *)0x0,vector2,(j->head).mdtbody[1],j->vec2);
  }
  MdtConstraintSetAxis(j,x,y,z);
  return;
}


/* ==== MdtLinear2Create ==== */

MdtLinear2ID MdtLinear2Create(MdtWorldID w)

{
  MdtLinear2ID pMVar1;
  int iVar2;
  MdtLinear2ID pMVar3;
  
                    /* Unresolved local var: MdtLinear2 * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtLinear2ID)0x0;
  if (pMVar1 != (MdtLinear2ID)0x0) {
                    /* Unresolved local var: int i@[???] */
    (pMVar1->head).tag = 0xb;
    (pMVar1->head).bclFunction = MdtBclAddLinear2;
    (pMVar1->head).maxRows = 2;
    (pMVar1->head).setBodyFunc = Linear2SetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    pMVar1->pos2[0] = 0.0;
    pMVar1->pos1[0] = 0.0;
    pMVar1->vec2[0] = 0.0;
    pMVar1->vec1[0] = 0.0;
    pMVar1->direction[0] = 0.0;
    pMVar1->pos2[1] = 0.0;
    pMVar1->pos1[1] = 0.0;
    pMVar1->vec2[1] = 0.0;
    pMVar1->vec1[1] = 0.0;
    pMVar1->direction[1] = 0.0;
    pMVar1->pos2[2] = 0.0;
    pMVar1->pos1[2] = 0.0;
    pMVar1->vec2[2] = 0.0;
    pMVar1->vec1[2] = 0.0;
    pMVar1->direction[2] = 0.0;
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtLinear2Reset ==== */

void MdtLinear2Reset(MdtLinear2ID j)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[???] */
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  j->pos2[0] = 0.0;
  j->pos1[0] = 0.0;
  j->vec2[0] = 0.0;
  j->vec1[0] = 0.0;
  j->direction[0] = 0.0;
  j->pos2[1] = 0.0;
  j->pos1[1] = 0.0;
  j->vec2[1] = 0.0;
  j->vec1[1] = 0.0;
  j->direction[1] = 0.0;
  j->pos2[2] = 0.0;
  j->pos1[2] = 0.0;
  j->vec2[2] = 0.0;
  j->vec1[2] = 0.0;
  j->direction[2] = 0.0;
  BaseConstraintReset(j);
  return;
}


/* ==== MdtLinear2QuaConstraint ==== */

MdtConstraintID MdtLinear2QuaConstraint(MdtLinear2ID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastLinear2 ==== */

MdtLinear2ID MdtConstraintDCastLinear2(MdtConstraintID c)

{
  return (MdtLinear2ID)(((c->head).tag != 0xb) - 1 & (uint)c);
}


/* ==== MdtLinear2GetPosition ==== */

void MdtLinear2GetPosition(MdtLinear2ID j,MeReal *position)

{
  ConvertCOMPositionVector((j->head).mdtbody[0],j->pos1,(void *)0x0,position);
  return;
}


/* ==== MdtLinear2GetDirection ==== */

void MdtLinear2GetDirection(MdtLinear2ID j,MeReal *direction)

{
  if ((j->head).mdtbody[1] == (MdtBody *)0x0) {
    *direction = j->direction[0];
    direction[1] = j->direction[1];
    direction[2] = j->direction[2];
  }
  else {
    ConvertCOMVector((j->head).mdtbody[1],j->direction,(void *)0x0,direction);
  }
  return;
}


/* ==== Linear2SetBodies ==== */

void Linear2SetBodies(MdtConstraintID c,MdtBodyID b1,MdtBodyID b2)

{
  int iVar1;
  
                    /* Unresolved local var: MdtLinear2ID j@[???] */
  iVar1 = (c->head).tag;
  BaseConstraintSetBodies(c,b1,b2);
  MdtLinear2SetDirection((MdtLinear2ID)((iVar1 != 0xb) - 1 & (uint)c),1.0,0.0,0.0);
  return;
}


/* ==== MdtLinear2SetPosition ==== */

void MdtLinear2SetPosition(MdtLinear2ID j,MeReal x,MeReal y,MeReal z)

{
  MeReal *pMVar1;
  MdtBody *pMVar2;
  MeReal y_local;
  MeReal x_local;
  
  pMVar1 = j->pos2;
  j->pos2[0] = x;
  j->pos2[2] = z;
  j->pos2[1] = y;
  ConvertCOMPositionVector((void *)0x0,pMVar1,(j->head).mdtbody[0],j->pos1);
  pMVar2 = (j->head).mdtbody[1];
  if (pMVar2 != (MdtBody *)0x0) {
    ConvertCOMPositionVector((void *)0x0,pMVar1,pMVar2,pMVar1);
  }
  MdtConstraintSetPosition(j,x,y,z);
  return;
}


