/* ==== MdtLinear1SetPosition ==== */

void MdtLinear1SetPosition(MdtLinear1ID j,MeReal x,MeReal y,MeReal z)

{
  void *pvVar1;
  MeVector3 p2;
  MeVector3 Body2Pos;
  MeVector3 p1p2;
  MeVector3 normal;
  MeVector3 position;
  
  if ((j->head).mdtbody[1] == (MdtBody *)0x0) {
    normal[2] = z;
    normal[0] = x;
    normal[1] = y;
  }
  else {
    MdtBodyGetPosition((j->head).mdtbody[1],Body2Pos);
    normal[0] = x - Body2Pos[0];
    normal[1] = y - Body2Pos[1];
    normal[2] = z - Body2Pos[2];
  }
  position[2] = z;
  position[0] = x;
  position[1] = y;
  ConvertCOMPositionVector((void *)0x0,position,(j->head).mdtbody[0],j->pos1);
  if ((j->head).mdtbody[1] == (MdtBody *)0x0) {
    j->pos2[2] = z;
    j->pos2[0] = x;
    j->pos2[1] = y;
  }
  else {
    ConvertCOMPositionVector((void *)0x0,position,(j->head).mdtbody[1],j->pos2);
  }
  pvVar1 = MdtConstraintGetBody(j,0);
  MdtBodyGetPosition(pvVar1,p1p2);
  if ((j->head).mdtbody[1] != (MdtBody *)0x0) {
    pvVar1 = MdtConstraintGetBody(j,1);
    MdtBodyGetPosition(pvVar1,p2);
                    /* Unresolved local var: uint i@[???] */
    p1p2[0] = p1p2[0] - p2[0];
    p1p2[1] = p1p2[1] - p2[1];
    p1p2[2] = p1p2[2] - p2[2];
  }
  j->displacement = p1p2[2] * normal[2] + p1p2[1] * normal[1] + p1p2[0] * normal[0];
  MdtConstraintSetPosition(j,x,y,z);
  return;
}


/* ==== MdtLinear1Create ==== */

MdtLinear1ID MdtLinear1Create(MdtWorldID w)

{
  MdtLinear1ID pMVar1;
  int iVar2;
  MdtLinear1ID pMVar3;
  
                    /* Unresolved local var: MdtLinear1 * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtLinear1ID)0x0;
  if (pMVar1 != (MdtLinear1ID)0x0) {
                    /* Unresolved local var: int i@[???] */
    (pMVar1->head).tag = 10;
    (pMVar1->head).bclFunction = MdtBclAddLinear1;
    (pMVar1->head).maxRows = 1;
    (pMVar1->head).setBodyFunc = BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    pMVar1->pos2[0] = 0.0;
    pMVar1->pos1[0] = 0.0;
    pMVar1->pos2[1] = 0.0;
    pMVar1->pos1[1] = 0.0;
    pMVar1->pos2[2] = 0.0;
    pMVar1->pos1[2] = 0.0;
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtLinear1Reset ==== */

void MdtLinear1Reset(MdtLinear1ID j)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[???] */
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  j->pos2[0] = 0.0;
  j->pos1[0] = 0.0;
  j->pos2[1] = 0.0;
  j->pos1[1] = 0.0;
  j->pos2[2] = 0.0;
  j->pos1[2] = 0.0;
  BaseConstraintReset(j);
  return;
}


/* ==== MdtLinear1QuaConstraint ==== */

MdtConstraintID MdtLinear1QuaConstraint(MdtLinear1ID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastLinear1 ==== */

MdtLinear1ID MdtConstraintDCastLinear1(MdtConstraintID c)

{
  return (MdtLinear1ID)(((c->head).tag != 10) - 1 & (uint)c);
}


/* ==== MdtLinear1GetPosition ==== */

void MdtLinear1GetPosition(MdtLinear1ID j,MeReal *position)

{
  ConvertCOMPositionVector((j->head).mdtbody[0],j->pos1,(void *)0x0,position);
  return;
}


