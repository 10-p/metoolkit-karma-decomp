/* ==== MdtContactGroupCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MdtContactGroupID_conflict MdtContactGroupCreate(MdtWorldID w)

{
  MdtContactGroupID_conflict pMVar1;
  int iVar2;
  void *pvVar3;
  MdtContactID_conflict pMVar4;
  MdtContactGroupID_conflict pMVar5;
  MdtContactID_conflict pMVar6;
  
                    /* Unresolved local var: MdtContactGroup * c@[DW_OP_reg7(EDI)] */
  pMVar1 = BaseConstraintCreate(w);
  pMVar4 = _MdtContactInvalidID;
  pMVar5 = (MdtContactGroupID_conflict)0x0;
  if (pMVar1 != (MdtContactGroupID_conflict)0x0) {
                    /* Unresolved local var: MdtContactID.conflict contact@[DW_OP_reg6(ESI)]
                       Unresolved local var: MdtContactID.conflict next@[DW_OP_reg3(EBX)] */
    (pMVar1->head).tag = 5;
    (pMVar1->head).bclFunction = MdtBclAddContactGroup;
    (pMVar1->head).maxRows = 0;
    (pMVar1->head).setBodyFunc = BaseConstraintSetBodies;
    (pMVar1->head).setAxisFunc = BaseConstraintSetAxis;
    pMVar1->first = pMVar4;
    pMVar1->last = pMVar4;
    iVar2 = MdtConstraintIsEnabled(pMVar1);
    if (iVar2 != 0) {
      MdtConstraintDisable(pMVar1);
    }
    pMVar6 = pMVar1->first;
    if (pMVar6 != pMVar4) {
      do {
        pvVar3 = MdtContactQuaConstraint(pMVar6);
        iVar2 = MdtConstraintIsEnabled(pvVar3);
        if (iVar2 != 0) {
          pvVar3 = MdtContactQuaConstraint(pMVar6);
          MdtConstraintDisable(pvVar3);
        }
        pMVar4 = MdtContactGetNext(pMVar6);
        pvVar3 = MdtContactQuaConstraint(pMVar6);
        MdtConstraintDestroy(pvVar3);
        pMVar6 = pMVar4;
      } while (pMVar4 != _MdtContactInvalidID);
    }
    BaseConstraintReset(pMVar1);
    pMVar4 = _MdtContactInvalidID;
    pMVar1->first = _MdtContactInvalidID;
    pMVar1->last = pMVar4;
    pMVar1->count = 0;
    pMVar1->swapped = 0;
    pMVar1->normalForce = 0.0;
    pMVar1->generator = (void *)0x0;
    pMVar5 = pMVar1;
  }
  return pMVar5;
}


/* ==== MdtContactGroupDestroy ==== */

void MdtContactGroupDestroy(MdtContactGroupID_conflict g)

{
  MdtContactID pMVar1;
  MdtContactGroupDestroyCallbackCBPtr p_Var2;
  MdtContactID_conflict pMVar3;
  void *pvVar4;
  
                    /* Unresolved local var: MdtContactID.conflict contact@[DW_OP_reg0(EAX)]
                       Unresolved local var: MdtContactID.conflict next@[DW_OP_reg3(EBX)] */
  pMVar3 = g->first;
  while (pMVar3 != (MdtContactID_conflict)0x0) {
    pMVar1 = pMVar3->nextContact;
    pvVar4 = MdtContactQuaConstraint(pMVar3);
    MdtConstraintDestroy(pvVar4);
    pMVar3 = pMVar1;
  }
  p_Var2 = ((g->head).world)->contactGroupDestroyCallback;
  if (p_Var2 != (MdtContactGroupDestroyCallbackCBPtr)0x0) {
    (*p_Var2)(g);
  }
  MdtConstraintDestroy(g);
  return;
}


/* ==== MdtContactGroupReset ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MdtContactGroupReset(MdtContactGroupID_conflict c)

{
  int iVar1;
  void *pvVar2;
  MdtContactID_conflict pMVar3;
  MdtContactID_conflict pMVar4;
  
                    /* Unresolved local var: MdtContactID.conflict contact@[DW_OP_reg6(ESI)]
                       Unresolved local var: MdtContactID.conflict next@[DW_OP_reg3(EBX)] */
  iVar1 = MdtConstraintIsEnabled(c);
  if (iVar1 != 0) {
    MdtConstraintDisable(c);
  }
  pMVar4 = c->first;
  if (pMVar4 != _MdtContactInvalidID) {
    do {
      pvVar2 = MdtContactQuaConstraint(pMVar4);
      iVar1 = MdtConstraintIsEnabled(pvVar2);
      if (iVar1 != 0) {
        pvVar2 = MdtContactQuaConstraint(pMVar4);
        MdtConstraintDisable(pvVar2);
      }
      pMVar3 = MdtContactGetNext(pMVar4);
      pvVar2 = MdtContactQuaConstraint(pMVar4);
      MdtConstraintDestroy(pvVar2);
      pMVar4 = pMVar3;
    } while (pMVar3 != _MdtContactInvalidID);
  }
  BaseConstraintReset(c);
  pMVar4 = _MdtContactInvalidID;
  c->first = _MdtContactInvalidID;
  c->last = pMVar4;
  c->count = 0;
  c->swapped = 0;
  c->normalForce = 0.0;
  c->generator = (void *)0x0;
  return;
}


/* ==== MdtContactGroupSetGenerator ==== */

void MdtContactGroupSetGenerator(MdtContactGroupID_conflict c,void *generator)

{
  c->generator = generator;
  return;
}


/* ==== MdtContactGroupQuaConstraint ==== */

MdtConstraintID MdtContactGroupQuaConstraint(MdtContactGroupID_conflict c)

{
  return (MdtConstraintID)c;
}


/* ==== MdtConstraintDCastContactGroup ==== */

MdtContactGroupID_conflict MdtConstraintDCastContactGroup(MdtConstraintID c)

{
  return (MdtContactGroupID_conflict)(((c->head).tag != 5) - 1 & (uint)c);
}


/* ==== MdtContactGroupGetFirstContact ==== */

MdtContactID_conflict MdtContactGroupGetFirstContact(MdtContactGroupID_conflict c)

{
  return c->first;
}


/* ==== MdtContactGroupGetGenerator ==== */

void * MdtContactGroupGetGenerator(MdtContactGroupID_conflict c)

{
  return c->generator;
}


/* ==== MdtContactGroupIsSwapped ==== */

MeBool MdtContactGroupIsSwapped(MdtContactGroupID_conflict g)

{
  return g->swapped;
}


/* ==== MdtContactGroupGetCount ==== */

int MdtContactGroupGetCount(MdtContactGroupID_conflict g)

{
  return g->count;
}


/* ==== MdtContactGroupGetNextContact ==== */

MdtContactID_conflict
MdtContactGroupGetNextContact(MdtContactGroupID_conflict g,MdtContactID_conflict c)

{
  return c->nextContact;
}


/* ==== MdtContactGroupGetNormalForce ==== */

MeReal MdtContactGroupGetNormalForce(MdtContactGroupID_conflict g)

{
  return g->normalForce;
}


/* ==== MdtContactGroupAppendContact ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MdtContactGroupAppendContact(MdtContactGroupID_conflict g,MdtContactID_conflict c)

{
  MdtContactID_conflict pMVar1;
  
  pMVar1 = g->first;
  if (pMVar1 == _MdtContactInvalidID) {
    g->last = c;
    g->first = c;
    c->prevContact = pMVar1;
    c->nextContact = pMVar1;
  }
  else {
    c->prevContact = g->last;
    g->last->nextContact = c;
    g->last = c;
  }
  c->contactGroup = g;
  g->count = g->count + 1;
  return;
}


/* ==== MdtContactGroupRemoveContact ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MdtContactGroupRemoveContact(MdtContactGroupID_conflict g,MdtContactID_conflict c)

{
  MdtContactID pMVar1;
  MdtContactID_conflict pMVar2;
  
  if (g->first == c) {
    g->first = c->nextContact;
    pMVar2 = c->prevContact;
  }
  else {
    pMVar2 = c->prevContact;
    pMVar2->nextContact = c->nextContact;
  }
  if (g->last == c) {
    g->last = pMVar2;
  }
  else {
    c->nextContact->prevContact = pMVar2;
  }
  pMVar1 = _MdtContactInvalidID;
  c->prevContact = _MdtContactInvalidID;
  c->nextContact = pMVar1;
  c->contactGroup = (MdtContactGroupID)0x0;
  g->count = g->count + -1;
  return;
}


/* ==== MdtContactGroupCreateContact ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MdtContactID_conflict MdtContactGroupCreateContact(MdtContactGroupID_conflict g)

{
  MdtContactID_conflict pMVar1;
  MdtContactID_conflict pMVar2;
  void *pvVar3;
  MdtBody *pMVar4;
  MdtBody *pMVar5;
  
                    /* Unresolved local var: MdtContactID.conflict c@[DW_OP_reg3(EBX)] */
  pMVar2 = MdtContactCreate((g->head).world);
  if (pMVar2 != _MdtContactInvalidID) {
    pMVar1 = g->first;
    if (pMVar1 == _MdtContactInvalidID) {
      g->last = pMVar2;
      g->first = pMVar2;
      pMVar2->prevContact = pMVar1;
      pMVar2->nextContact = pMVar1;
    }
    else {
      pMVar2->prevContact = g->last;
      g->last->nextContact = pMVar2;
      g->last = pMVar2;
    }
    pMVar2->contactGroup = g;
    g->count = g->count + 1;
    pMVar5 = (g->head).mdtbody[1];
    pMVar4 = (g->head).mdtbody[0];
    pvVar3 = MdtContactQuaConstraint(pMVar2);
    MdtConstraintSetBodies(pvVar3,pMVar4,pMVar5);
  }
  return pMVar2;
}


/* ==== MdtContactGroupDestroyContact ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MdtContactGroupDestroyContact(MdtContactGroupID_conflict g,MdtContactID_conflict c)

{
  MdtContactID pMVar1;
  void *pvVar2;
  MdtContactID_conflict pMVar3;
  
  if (g->first == c) {
    g->first = c->nextContact;
    pMVar3 = c->prevContact;
  }
  else {
    pMVar3 = c->prevContact;
    pMVar3->nextContact = c->nextContact;
  }
  if (g->last == c) {
    g->last = pMVar3;
  }
  else {
    c->nextContact->prevContact = pMVar3;
  }
  pMVar1 = _MdtContactInvalidID;
  c->prevContact = _MdtContactInvalidID;
  c->nextContact = pMVar1;
  c->contactGroup = (MdtContactGroupID)0x0;
  g->count = g->count + -1;
  pvVar2 = MdtContactQuaConstraint(c);
  MdtConstraintDestroy(pvVar2);
  return;
}


