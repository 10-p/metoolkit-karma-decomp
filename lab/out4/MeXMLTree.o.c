/* ==== PElementGetAttributeValue ==== */

char * PElementGetAttributeValue(PElement *elem,char *attr)

{
  AttributeNode *pAVar1;
  Attribute *pAVar2;
  int iVar3;
  
                    /* Unresolved local var: AttributeNode * node@[DW_OP_reg3(EBX)] */
  pAVar1 = elem->attrHead;
  while( true ) {
    if (pAVar1 == (AttributeNode *)0x0) {
      return (char *)0x0;
    }
                    /* Unresolved local var: Attribute * at@[DW_OP_reg6(ESI)] */
    pAVar2 = pAVar1->current;
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    iVar3 = strcmp(pAVar2->attr,attr);
    if (iVar3 == 0) break;
    pAVar1 = pAVar1->next;
  }
  return pAVar2->value;
}


/* ==== PElementCompareAttributes ==== */

PElement * PElementCompareAttributes(PElement *e,void *attr,void *attrVal)

{
  char *__s1;
  int iVar1;
  
                    /* Unresolved local var: char * temp@[DW_OP_reg0(EAX)] */
  __s1 = PElementGetAttributeValue(e,attr);
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  if ((__s1 != (char *)0x0) && (iVar1 = strcmp(__s1,attrVal), iVar1 == 0)) {
    return e;
  }
  return (PElement *)0x0;
}


/* ==== PElementGetNext ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

PElement * PElementGetNext(PElementIt *it)

{
  PElementNode *pPVar1;
  PElement *pPVar2;
  PElement *pPVar3;
  PElementNode *pPVar4;
  
                    /* Unresolved local var: PElement * e@[DW_OP_reg3(EBX)]
                       Unresolved local var: PElementNode * node@[DW_OP_reg2(EDX)]
                       Unresolved local var: PElementNode * child@[DW_OP_reg3(EBX)] */
  pPVar3 = (PElement *)0x0;
  pPVar1 = it->stackHead;
  pPVar2 = pPVar1->current;
  if (pPVar2 != (PElement *)0x0) {
    it->stackHead = pPVar1->next;
    (*_strlen)(pPVar1);
    for (pPVar1 = pPVar2->childHead; pPVar1 != (PElementNode *)0x0; pPVar1 = pPVar1->next) {
      pPVar4 = (PElementNode *)(*_MeMemoryAPI)(8);
      pPVar4->current = pPVar1->current;
      if (it->stackHead == (PElementNode *)0x0) {
        it->stackHead = pPVar4;
        pPVar4->next = (PElementNode *)0x0;
      }
      else {
        pPVar4->next = it->stackHead;
        it->stackHead = pPVar4;
      }
    }
    pPVar3 = (PElement *)0x0;
    if (it->stackHead != (PElementNode *)0x0) {
      pPVar3 = it->stackHead->current;
    }
  }
  return pPVar3;
}


/* ==== PElementLookup ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

PElement * PElementLookup(PElement *root,char *attr,char *val)

{
  PElementNode *pPVar1;
  PElementIt PVar2;
  PElement *elem;
  char *__s1;
  int iVar3;
  undefined4 extraout_ECX;
  char *pcVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  PElementIt it;
  
                    /* Unresolved local var: PElement * e@[DW_OP_reg6(ESI)] */
                    /* Unresolved local var: PElementNode * node@[DW_OP_reg0(EAX)] */
  it.stackHead = (PElementNode *)(*_MeMemoryAPI)(8);
  (it.stackHead)->current = root;
  (it.stackHead)->next = (PElementNode *)0x0;
  do {
    elem = PElementGetNext(&it);
    PVar2 = it;
    if (elem == (PElement *)0x0) {
      while (PVar2.stackHead != (PElementNode *)0x0) {
                    /* Unresolved local var: PElementNode * temp@[DW_OP_reg3(EBX)] */
        pPVar1 = (PVar2.stackHead)->next;
        (*_strlen)(PVar2.stackHead);
        PVar2.stackHead = pPVar1;
      }
      return (PElement *)0x0;
    }
                    /* Unresolved local var: char * v@[DW_OP_reg0(EAX)] */
    __s1 = PElementGetAttributeValue(elem,attr);
  } while ((__s1 == (char *)0x0) ||
          (pcVar4 = val, uVar5 = extraout_ECX, uVar6 = extraout_ECX, iVar3 = strcmp(__s1,val),
          PVar2 = it, iVar3 != 0
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */));
  while (PVar2.stackHead != (PElementNode *)0x0) {
                    /* Unresolved local var: PElementNode * temp@[DW_OP_reg3(EBX)] */
    pPVar1 = (PVar2.stackHead)->next;
    (*_strlen)(PVar2.stackHead,pcVar4,uVar5,uVar6);
    PVar2.stackHead = pPVar1;
  }
  return elem;
}


/* ==== PElementCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

PElement * PElementCreate(int type,char *name,void *data,CDataFreeFunc func,char *attrs)

{
  char cVar1;
  bool bVar2;
  PElement *e;
  ushort **ppuVar3;
  char *attr;
  char *pcVar4;
  char *next;
  char *val;
  char *a;
  
                    /* Unresolved local var: PElement * elem@[DW_OP_reg6(ESI)] */
  e = (PElement *)(*_MeMemoryAPI)(0x14);
  e->type = type;
  e->childHead = (PElementNode *)0x0;
  e->cdata = data;
  e->freeFunc = func;
  e->attrHead = (AttributeNode *)0x0;
  if (attrs != (char *)0x0) {
    cVar1 = *attrs;
    next = attrs;
    while (cVar1 != '\0') {
                    /* Unresolved local var: char * ptr@[DW_OP_reg3(EBX)] */
      ppuVar3 = __ctype_b_loc();
      attr = next;
      while (((*ppuVar3)[*attr] & 0x2000) != 0) {
        attr = attr + 1;
      }
      ppuVar3 = __ctype_b_loc();
      pcVar4 = attr;
      while (cVar1 = *pcVar4, ((*ppuVar3)[cVar1] & 0x2000) == 0) {
        if (cVar1 == '=') goto LAB_0001023d;
        pcVar4 = pcVar4 + 1;
      }
      bVar2 = false;
      if (cVar1 == '=') {
LAB_0001023d:
        *pcVar4 = '\0';
        if ((pcVar4[1] == '\"') || (bVar2 = false, pcVar4[1] == '\'')) {
          val = pcVar4 + 2;
          pcVar4 = strchr(val,0x22);
          bVar2 = false;
          if (pcVar4 != (char *)0x0) {
            *pcVar4 = '\0';
            next = pcVar4 + 1;
            bVar2 = true;
          }
        }
      }
      if (!bVar2) {
        return e;
      }
      PElementAddAttribute(e,attr,val);
      cVar1 = *next;
    }
  }
  return e;
}


/* ==== PElementDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void PElementDestroy(PElement *e)

{
  AttributeNode *pAVar1;
  AttributeNode *pAVar2;
  
                    /* Unresolved local var: AttributeNode * node@[DW_OP_reg6(ESI)]
                       Unresolved local var: AttributeNode * temp@[DW_OP_reg3(EBX)] */
  pAVar2 = e->attrHead;
  while (pAVar2 != (AttributeNode *)0x0) {
    (*_strlen)(pAVar2->current->attr);
    (*_strlen)(pAVar2->current->value);
    (*_strlen)(pAVar2->current);
    pAVar1 = pAVar2->next;
    (*_strlen)(pAVar2);
    pAVar2 = pAVar1;
  }
  if ((e->cdata != (void *)0x0) && (e->freeFunc != (CDataFreeFunc)0x0)) {
    (*e->freeFunc)(e->cdata);
  }
  (*_strlen)(e);
  return;
}


/* ==== PElementDestroyChildren ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void PElementDestroyChildren(PElement *e,PElement *parent,void *userdata)

{
  PElement *pPVar1;
  AttributeNode *pAVar2;
  PElementNode *pPVar3;
  PElementNode *pPVar4;
  AttributeNode *pAVar5;
  PElementNode *node;
  
                    /* Unresolved local var: PElementNode * temp@[DW_OP_reg3(EBX)] */
  pPVar4 = e->childHead;
  while (pPVar4 != (PElementNode *)0x0) {
                    /* Unresolved local var: AttributeNode * node@[DW_OP_reg6(ESI)]
                       Unresolved local var: AttributeNode * temp@[DW_OP_reg3(EBX)] */
    pPVar1 = pPVar4->current;
    pAVar5 = pPVar1->attrHead;
    while (pAVar5 != (AttributeNode *)0x0) {
      (*_strlen)(pAVar5->current->attr);
      (*_strlen)(pAVar5->current->value);
      (*_strlen)(pAVar5->current);
      pAVar2 = pAVar5->next;
      (*_strlen)(pAVar5);
      pAVar5 = pAVar2;
    }
    if ((pPVar1->cdata != (void *)0x0) && (pPVar1->freeFunc != (CDataFreeFunc)0x0)) {
      (*pPVar1->freeFunc)(pPVar1->cdata);
    }
    (*_strlen)(pPVar1);
    pPVar3 = pPVar4->next;
    (*_strlen)(pPVar4);
    pPVar4 = pPVar3;
  }
  return;
}


/* ==== PElementAddAttribute ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void PElementAddAttribute(PElement *e,char *attr,char *val)

{
  Attribute *pAVar1;
  AttributeNode *pAVar2;
  size_t sVar3;
  char *pcVar4;
  
                    /* Unresolved local var: Attribute * at@[DW_OP_reg3(EBX)]
                       Unresolved local var: AttributeNode * node@[DW_OP_reg7(EDI)] */
  pAVar1 = (Attribute *)(*_MeMemoryAPI)(8);
  pAVar2 = (AttributeNode *)(*_MeMemoryAPI)(8);
  sVar3 = strlen(attr);
  pcVar4 = (char *)(*_MeMemoryAPI)(sVar3 + 1);
  pAVar1->attr = pcVar4;
  strcpy(pcVar4,attr);
  sVar3 = strlen(val);
  pcVar4 = (char *)(*_MeMemoryAPI)(sVar3 + 1);
  pAVar1->value = pcVar4;
  strcpy(pcVar4,val);
  pAVar2->current = pAVar1;
  if (e->attrHead == (AttributeNode *)0x0) {
    e->attrHead = pAVar2;
    pAVar2->next = (AttributeNode *)0x0;
  }
  else {
    pAVar2->next = e->attrHead;
    e->attrHead = pAVar2;
  }
  return;
}


/* ==== PElementParseAttributes ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void PElementParseAttributes(PElement *e,char *attrs)

{
  char cVar1;
  bool bVar2;
  ushort **ppuVar3;
  Attribute *pAVar4;
  AttributeNode *pAVar5;
  size_t sVar6;
  char *pcVar7;
  char *pcVar8;
  char *next;
  char *val;
  char *a;
  
  if (attrs != (char *)0x0) {
    cVar1 = *attrs;
    next = attrs;
    while (cVar1 != '\0') {
                    /* Unresolved local var: char * ptr@[DW_OP_reg3(EBX)] */
      ppuVar3 = __ctype_b_loc();
      pcVar7 = next;
      while (((*ppuVar3)[*pcVar7] & 0x2000) != 0) {
        pcVar7 = pcVar7 + 1;
      }
      ppuVar3 = __ctype_b_loc();
      pcVar8 = pcVar7;
      while (cVar1 = *pcVar8, ((*ppuVar3)[cVar1] & 0x2000) == 0) {
        if (cVar1 == '=') goto LAB_000104e0;
        pcVar8 = pcVar8 + 1;
      }
      bVar2 = false;
      if (cVar1 == '=') {
LAB_000104e0:
        *pcVar8 = '\0';
        if ((pcVar8[1] == '\"') || (bVar2 = false, pcVar8[1] == '\'')) {
          val = pcVar8 + 2;
          pcVar8 = strchr(val,0x22);
          bVar2 = false;
          if (pcVar8 != (char *)0x0) {
            *pcVar8 = '\0';
            next = pcVar8 + 1;
            bVar2 = true;
          }
        }
      }
      if (!bVar2) {
        return;
      }
                    /* Unresolved local var: Attribute * at@[DW_OP_reg3(EBX)]
                       Unresolved local var: AttributeNode * node@[DW_OP_reg7(EDI)] */
      pAVar4 = (Attribute *)(*_MeMemoryAPI)(8);
      pAVar5 = (AttributeNode *)(*_MeMemoryAPI)(8);
      sVar6 = strlen(pcVar7);
      pcVar8 = (char *)(*_MeMemoryAPI)(sVar6 + 1);
      pAVar4->attr = pcVar8;
      strcpy(pcVar8,pcVar7);
      sVar6 = strlen(val);
      pcVar7 = (char *)(*_MeMemoryAPI)(sVar6 + 1);
      pAVar4->value = pcVar7;
      strcpy(pcVar7,val);
      pAVar5->current = pAVar4;
      if (e->attrHead == (AttributeNode *)0x0) {
        e->attrHead = pAVar5;
        pAVar5->next = (AttributeNode *)0x0;
      }
      else {
        pAVar5->next = e->attrHead;
        e->attrHead = pAVar5;
      }
      cVar1 = *next;
    }
  }
  return;
}


/* ==== PElementInsert ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void PElementInsert(PElement *e,PElement *parent)

{
  PElementNode *pPVar1;
  
                    /* Unresolved local var: PElementNode * node@[DW_OP_reg2(EDX)] */
  pPVar1 = (PElementNode *)(*_MeMemoryAPI)(8);
  pPVar1->current = e;
  if (parent->childHead == (PElementNode *)0x0) {
    parent->childHead = pPVar1;
    pPVar1->next = (PElementNode *)0x0;
  }
  else {
    pPVar1->next = parent->childHead;
    parent->childHead = pPVar1;
  }
  return;
}


/* ==== PElementFind ==== */

PElement * PElementFind(PElement *root,PElementCompareCB cb,void *k1,void *k2)

{
  PElementNode *pPVar1;
  PElement *pPVar2;
  PElement *pPVar3;
  
                    /* Unresolved local var: PElementNode * node@[DW_OP_reg3(EBX)] */
  pPVar1 = root->childHead;
  while( true ) {
    if (pPVar1 == (PElementNode *)0x0) {
      return (PElement *)0x0;
    }
                    /* Unresolved local var: PElement * ret@[DW_OP_reg0(EAX)]
                       Unresolved local var: PElement * e@[DW_OP_reg6(ESI)] */
    pPVar3 = pPVar1->current;
    pPVar2 = (*cb)(pPVar3,k1,k2);
    if (pPVar2 != (PElement *)0x0) break;
    pPVar3 = PElementFind(pPVar3,cb,k1,k2);
    if (pPVar3 != (PElement *)0x0) {
      return pPVar3;
    }
    pPVar1 = pPVar1->next;
  }
  return pPVar2;
}


/* ==== PElementTraverseAll ==== */

void PElementTraverseAll(PElement *root,PElementCB cb,MeBool rootFirst,void *userdata)

{
  PElementNode *pPVar1;
  PElement *root_00;
  
                    /* Unresolved local var: PElementNode * node@[DW_OP_reg3(EBX)] */
  for (pPVar1 = root->childHead; pPVar1 != (PElementNode *)0x0; pPVar1 = pPVar1->next) {
                    /* Unresolved local var: PElement * e@[DW_OP_reg6(ESI)] */
    root_00 = pPVar1->current;
    if (rootFirst != 0) {
      (*cb)(root_00,root,userdata);
    }
    PElementTraverseAll(root_00,cb,rootFirst,userdata);
    if (rootFirst == 0) {
      (*cb)(root_00,root,userdata);
    }
  }
  return;
}


/* ==== PElementInitIterator ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void PElementInitIterator(PElement *root,PElementIt *it)

{
  PElementNode *pPVar1;
  
                    /* Unresolved local var: PElementNode * node@[DW_OP_reg0(EAX)] */
  pPVar1 = (PElementNode *)(*_MeMemoryAPI)(8);
  pPVar1->current = root;
  pPVar1->next = (PElementNode *)0x0;
  it->stackHead = pPVar1;
  return;
}


/* ==== PElementIteratorDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void PElementIteratorDestroy(PElementIt *it)

{
  PElementNode *pPVar1;
  PElementNode *pPVar2;
  
                    /* Unresolved local var: PElementNode * node@[DW_OP_reg0(EAX)] */
  pPVar2 = it->stackHead;
  while (pPVar2 != (PElementNode *)0x0) {
                    /* Unresolved local var: PElementNode * temp@[DW_OP_reg3(EBX)] */
    pPVar1 = pPVar2->next;
    (*_strlen)(pPVar2);
    pPVar2 = pPVar1;
  }
  return;
}


