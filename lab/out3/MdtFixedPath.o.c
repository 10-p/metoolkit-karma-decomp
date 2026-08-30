/* ==== MdtFixedPathCreate ==== */

/* WARNING: Unknown calling convention */

MdtFixedPathID MdtFixedPathCreate(MdtWorldID w)

{
  MdtFixedPathID pMVar1;
  int iVar2;
  MdtFixedPathID pMVar3;
  
                    /* Unresolved local var: MdtFixedPath * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtFixedPathID)0x0;
  if (pMVar1 != (MdtFixedPathID)0x0) {
                    /* Unresolved local var: int i@[???] */
    (pMVar1->head).tag = 6;
    (pMVar1->head).bclFunction = MdtBclAddFixedPath;
    (pMVar1->head).maxRows = 3;
    (pMVar1->head).setBodyFunc = BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    pMVar1->pos2[0] = 0.0;
    pMVar1->pos1[0] = 0.0;
    pMVar1->vel2[0] = 0.0;
    pMVar1->vel1[0] = 0.0;
    pMVar1->pos2[1] = 0.0;
    pMVar1->pos1[1] = 0.0;
    pMVar1->vel2[1] = 0.0;
    pMVar1->vel1[1] = 0.0;
    pMVar1->pos2[2] = 0.0;
    pMVar1->pos1[2] = 0.0;
    pMVar1->vel2[2] = 0.0;
    pMVar1->vel1[2] = 0.0;
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtFixedPathReset ==== */

void MdtFixedPathReset(MdtFixedPathID j)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[???] */
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  j->pos2[0] = 0.0;
  j->pos1[0] = 0.0;
  j->vel2[0] = 0.0;
  j->vel1[0] = 0.0;
  j->pos2[1] = 0.0;
  j->pos1[1] = 0.0;
  j->vel2[1] = 0.0;
  j->vel1[1] = 0.0;
  j->pos2[2] = 0.0;
  j->pos1[2] = 0.0;
  j->vel2[2] = 0.0;
  j->vel1[2] = 0.0;
  BaseConstraintReset(j);
  return;
}


/* ==== MdtFixedPathQuaConstraint ==== */

MdtConstraintID MdtFixedPathQuaConstraint(MdtFixedPathID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastFixedPath ==== */

MdtFixedPathID MdtConstraintDCastFixedPath(MdtConstraintID c)

{
  return (MdtFixedPathID)(((c->head).tag != 6) - 1 & (uint)c);
}


/* ==== MdtFixedPathGetPosition ==== */

void MdtFixedPathGetPosition(MdtFixedPathID j,uint bodyindex,MeReal *position)

{
  MeReal *pMVar1;
  
                    /* Unresolved local var: MeReal * pos@[DW_OP_reg2(EDX)]
                       Unresolved local var: uint i@[???] */
  if (bodyindex == 0) {
    pMVar1 = j->pos1;
  }
  else {
    pMVar1 = j->pos2;
  }
  *position = *pMVar1;
  position[1] = pMVar1[1];
  position[2] = pMVar1[2];
  return;
}


/* ==== MdtFixedPathGetVelocity ==== */

void MdtFixedPathGetVelocity(MdtFixedPathID j,uint bodyindex,MeReal *velocity)

{
  MeReal *pMVar1;
  
                    /* Unresolved local var: MeReal * vel@[DW_OP_reg2(EDX)]
                       Unresolved local var: uint i@[???] */
  if (bodyindex == 0) {
    pMVar1 = j->vel1;
  }
  else {
    pMVar1 = j->vel2;
  }
  *velocity = *pMVar1;
  velocity[1] = pMVar1[1];
  velocity[2] = pMVar1[2];
  return;
}


/* ==== MdtFixedPathSetPosition ==== */

void MdtFixedPathSetPosition(MdtFixedPathID j,uint bodyindex,MeReal x,MeReal y,MeReal z)

{
  MeReal *pMVar1;
  MeReal (*paMVar2) [4];
  
                    /* Unresolved local var: MeReal * pos@[DW_OP_reg0(EAX)] */
  pMVar1 = j->pos1;
  if (bodyindex != 0) {
    pMVar1 = j->pos2;
  }
  *pMVar1 = x;
  pMVar1[1] = y;
  pMVar1[2] = z;
                    /* Unresolved local var: MeReal * pos@[DW_OP_reg0(EAX)] */
  paMVar2 = (j->head).ref1;
  if (bodyindex != 0) {
    paMVar2 = (j->head).ref2;
  }
  paMVar2 = paMVar2 + 3;
  (*paMVar2)[0] = x;
  (*paMVar2)[1] = y;
  (*paMVar2)[2] = z;
  return;
}


/* ==== MdtFixedPathSetVelocity ==== */

void MdtFixedPathSetVelocity(MdtFixedPathID j,uint bodyindex,MeReal dx,MeReal dy,MeReal dz)

{
  MeReal *pMVar1;
  
                    /* Unresolved local var: MeReal * vel@[DW_OP_reg2(EDX)] */
  if (bodyindex == 0) {
    pMVar1 = j->vel1;
  }
  else {
    pMVar1 = j->vel2;
  }
  *pMVar1 = dx;
  pMVar1[1] = dy;
  pMVar1[2] = dz;
  return;
}


