/* ==== MdtBSJointCreate ==== */

/* WARNING: Unknown calling convention */

MdtBSJointID MdtBSJointCreate(MdtWorldID w)

{
  MdtBSJointID pMVar1;
  int iVar2;
  MdtBSJointID pMVar3;
  
                    /* Unresolved local var: MdtBSJoint * j@[DW_OP_reg3(EBX)] */
  pMVar1 = (MdtBSJointID)BaseConstraintCreate(w);
  pMVar3 = (MdtBSJointID)0x0;
  if (pMVar1 != (MdtBSJointID)0x0) {
    (pMVar1->head).tag = 0;
    (pMVar1->head).bclFunction = (MdtBclAddConstraintFn)&MdtBclAddBSJoint;
    (pMVar1->head).maxRows = 3;
    (pMVar1->head).setBodyFunc = (MdtConstraintSetBodyFnPtr)&BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = (MdtConstraintSetAxisFnPtr)&BaseConstraintSetAxis;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    BaseConstraintReset(pMVar1);
    pMVar3 = pMVar1;
  }
  return pMVar3;
}


/* ==== MdtBSJointReset ==== */

void MdtBSJointReset(MdtBSJointID j)

{
  int iVar1;
  
  iVar1 = MdtConstraintIsEnabled(j);
  if (iVar1 != 0) {
    MdtConstraintDisable(j);
  }
  BaseConstraintReset();
  return;
}


/* ==== MdtConstraintDCastBSJoint ==== */

MdtBSJointID MdtConstraintDCastBSJoint(MdtConstraintID c)

{
  return (MdtBSJointID)(((c->head).tag != 0) - 1 & (uint)c);
}


