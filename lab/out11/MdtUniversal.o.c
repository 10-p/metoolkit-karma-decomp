/* ==== MdtUniversalCreate ==== */

MdtUniversalID MdtUniversalCreate(MdtWorldID w)

{
  MdtUniversalID pMVar1;
  int iVar2;
  MdtUniversalID pMVar3;
  
                    /* Unresolved local var: MdtUniversal * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar3 = (MdtUniversalID)0x0;
  if (pMVar1 != (MdtUniversalID)0x0) {
    (pMVar1->head).tag = 8;
    (pMVar1->head).bclFunction = MdtBclAddUniversal;
    (pMVar1->head).maxRows = 4;
    (pMVar1->head).setBodyFunc = BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtUniversalReset ==== */

void MdtUniversalReset(MdtUniversalID j)

{
  int iVar1;
  
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  BaseConstraintReset(j);
  return;
}


/* ==== MdtUniversalQuaConstraint ==== */

MdtConstraintID MdtUniversalQuaConstraint(MdtUniversalID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastUniversal ==== */

MdtUniversalID MdtConstraintDCastUniversal(MdtConstraintID c)

{
  return (MdtUniversalID)(((c->head).tag != 8) - 1 & (uint)c);
}


