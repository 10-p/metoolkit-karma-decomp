/* ==== MdtUserConstraintCreate ==== */

MdtUserConstraintID MdtUserConstraintCreate(MdtWorldID w)

{
  MdtUserConstraintID pMVar1;
  MdtUserConstraintID pMVar2;
  
                    /* Unresolved local var: MdtUserConstraint * j@[DW_OP_reg3(EBX)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar2 = (MdtUserConstraintID)0x0;
  if (pMVar1 != (MdtUserConstraintID)0x0) {
    (pMVar1->head).tag = 0x10;
    pMVar1->userConstraint = (void *)0x0;
    (pMVar1->head).bclFunction = (MdtBclAddConstraintFn)0x0;
    (pMVar1->head).maxRows = 6;
    (pMVar1->head).setBodyFunc = BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    BaseConstraintReset(pMVar1);
    pMVar2 = pMVar1;
  }
  return pMVar2;
}


/* ==== MdtUserConstraintQuaConstraint ==== */

MdtConstraintID MdtUserConstraintQuaConstraint(MdtUserConstraintID j)

{
  return (MdtConstraintID)j;
}


/* ==== MdtConstraintDCastUserConstraint ==== */

MdtUserConstraintID MdtConstraintDCastUserConstraint(MdtConstraintID c)

{
  return (MdtUserConstraintID)(((c->head).tag != 0x10) - 1 & (uint)c);
}


/* ==== MdtUserConstraintGetFunction ==== */

MdtBclAddConstraintFn MdtUserConstraintGetFunction(MdtUserConstraintID j)

{
  return (j->head).bclFunction;
}


/* ==== MdtUserConstraintGetConstraintData ==== */

void * MdtUserConstraintGetConstraintData(MdtUserConstraintID j)

{
  return j->userConstraint;
}


/* ==== MdtUserConstraintSetFunction ==== */

void MdtUserConstraintSetFunction(MdtUserConstraintID j,MdtBclAddConstraintFn f)

{
  (j->head).bclFunction = f;
  return;
}


/* ==== MdtUserConstraintSetConstraintData ==== */

void MdtUserConstraintSetConstraintData(MdtUserConstraintID j,void *d)

{
  j->userConstraint = d;
  return;
}


