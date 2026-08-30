/* ==== MdtLinear1SetPosition ==== */

void __regparm1 MdtLinear1SetPosition(MdtLinear1ID j,MeReal x,MeReal y,MeReal z)

{
  undefined4 uVar1;
  float in_stack_00000010;
  MeVector3 p2;
  MeVector3 Body2Pos;
  MeVector3 p1p2;
  MeVector3 normal;
  MeVector3 position;
  
  if (*(int *)((int)x + 0x58) == 0) {
    normal[2] = in_stack_00000010;
    normal[0] = y;
    normal[1] = z;
  }
  else {
    MdtBodyGetPosition(*(undefined4 *)((int)x + 0x58),Body2Pos,j,j);
    normal[0] = y - Body2Pos[0];
    normal[1] = z - Body2Pos[1];
    normal[2] = in_stack_00000010 - Body2Pos[2];
  }
  position[0] = y;
  position[1] = z;
  ConvertCOMPositionVector(0,position,*(undefined4 *)((int)x + 0x54),(int)x + 0x160);
  if (*(int *)((int)x + 0x58) == 0) {
    *(float *)((int)x + 0x174) = in_stack_00000010;
    *(MeReal *)((int)x + 0x16c) = y;
    *(MeReal *)((int)x + 0x170) = z;
  }
  else {
    ConvertCOMPositionVector(0,position,*(undefined4 *)((int)x + 0x58),(int)x + 0x16c);
  }
  uVar1 = MdtConstraintGetBody(x,0,position,position);
  uVar1 = MdtBodyGetPosition(uVar1,p1p2);
  if (*(int *)((int)x + 0x58) != 0) {
    uVar1 = MdtConstraintGetBody(x,1,uVar1,uVar1);
    MdtBodyGetPosition(uVar1,p2);
                    /* Unresolved local var: uint i@[???] */
    p1p2[0] = p1p2[0] - p2[0];
    p1p2[1] = p1p2[1] - p2[1];
    p1p2[2] = p1p2[2] - p2[2];
  }
  *(float *)((int)x + 0x178) = p1p2[2] * normal[2] + p1p2[1] * normal[1] + p1p2[0] * normal[0];
  MdtConstraintSetPosition(x,y,z);
  return;
}


/* ==== MdtLinear1Create ==== */

/* WARNING: Unknown calling convention */

MdtLinear1ID MdtLinear1Create(MdtWorldID w)

{
  MdtLinear1ID pMVar1;
  int iVar2;
  MdtLinear1ID pMVar3;
  
                    /* Unresolved local var: MdtLinear1 * j@[DW_OP_reg3(EBX)] */
  pMVar1 = (MdtLinear1ID)BaseConstraintCreate(w);
  pMVar3 = (MdtLinear1ID)0x0;
  if (pMVar1 != (MdtLinear1ID)0x0) {
                    /* Unresolved local var: int i@[???] */
    (pMVar1->head).tag = 10;
    (pMVar1->head).bclFunction = (MdtBclAddConstraintFn)&MdtBclAddLinear1;
    (pMVar1->head).maxRows = 1;
    (pMVar1->head).setBodyFunc = (MdtConstraintSetBodyFnPtr)&BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = (MdtConstraintSetAxisFnPtr)&BaseConstraintSetAxis;
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
  BaseConstraintReset();
  return;
}


/* ==== MdtConstraintDCastLinear1 ==== */

MdtLinear1ID MdtConstraintDCastLinear1(MdtConstraintID c)

{
  return (MdtLinear1ID)(((c->head).tag != 10) - 1 & (uint)c);
}


/* ==== MdtLinear1GetPosition ==== */

void __regparm1 MdtLinear1GetPosition(MdtLinear1ID j,MeReal *position)

{
  ConvertCOMPositionVector(position[0x15],position + 0x58,0);
  return;
}


