/* ==== MeFGeometryLookupPrimitive ==== */

MeFPrimitive * MeFGeometryLookupPrimitive(MeFGeometry *fg,char *name)

{
  MeFPrimitiveNode *pMVar1;
  MeFPrimitive *pMVar2;
  int iVar3;
  
                    /* Unresolved local var: MeFPrimitiveIt it@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeFPrimitive * p@[DW_OP_reg3(EBX)] */
  pMVar1 = fg->nilPrimitive->prev;
  pMVar2 = pMVar1->current;
  pMVar1 = pMVar1->prev;
  while( true ) {
    if (pMVar2 == (MeFPrimitive *)0x0) {
      return (MeFPrimitive *)0x0;
    }
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    iVar3 = strcmp(pMVar2->id,name);
    if (iVar3 == 0) break;
                    /* Unresolved local var: MeFPrimitive * prim@[DW_OP_reg0(EAX)] */
    pMVar2 = pMVar1->current;
    pMVar1 = pMVar1->prev;
  }
  return pMVar2;
}


/* ==== MeFGeometryRename ==== */

void MeFGeometryRename(MeFGeometry *fg,char *newName)

{
  int iVar1;
  
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar1 = strcmp(fg->id,newName);
  if (iVar1 != 0) {
    if (fg->asset != (MeFAsset *)0x0) {
      MeFAssetResolveGeometryReferences(fg->asset,fg->id,newName);
      MeHashDelete(fg->id,fg->asset->nameToGeometry);
    }
    _FSetStringProperty(fg,newName);
    if (fg->asset != (MeFAsset *)0x0) {
      MeHashInsert(fg->id,fg,fg->asset->nameToGeometry);
    }
  }
  return;
}


/* ==== MeFGeometryInsertPrimitive ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFGeometryInsertPrimitive(MeFGeometry *geometry,MeFPrimitive *p)

{
  MeFGeometry *pMVar1;
  MeFPrimitiveNode *pMVar2;
  MeFPrimitiveNode *pMVar3;
  int iVar4;
  MeFPrimitiveNode *pMVar5;
  
                    /* Unresolved local var: MeFPrimitiveNode * node@[DW_OP_reg7(EDI)] */
  pMVar1 = p->geometry;
  if (pMVar1 != (MeFGeometry *)0x0) {
    if (pMVar1 == geometry) {
      return;
    }
    if (pMVar1 != (MeFGeometry *)0x0) {
      MeFGeometryRemovePrimitive(p);
    }
  }
  pMVar3 = (*_MeMemoryAPI)(0xc);
  pMVar3->current = p;
                    /* Unresolved local var: MeFPrimitiveNode * temp@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeFPrimitive * tempPrim@[DW_OP_reg0(EAX)] */
  pMVar2 = geometry->nilPrimitive;
  do {
    pMVar5 = pMVar2;
    pMVar2 = pMVar5->next;
    if (pMVar2->current == (MeFPrimitive *)0x0) break;
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    iVar4 = strcmp(pMVar2->current->id,p->id);
  } while (0 < iVar4);
  pMVar3->next = pMVar2;
  pMVar2 = pMVar5->next;
  pMVar5->next = pMVar3;
  pMVar2->prev = pMVar3;
  pMVar3->prev = pMVar5;
  p->geometry = geometry;
  geometry->primCount = geometry->primCount + 1;
  return;
}


/* ==== MeFGeometryRemovePrimitive ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFGeometryRemovePrimitive(MeFPrimitive *p)

{
  MeFPrimitiveNode *pMVar1;
  MeFPrimitive *pMVar2;
  MeFPrimitiveNode *pMVar3;
  
                    /* Unresolved local var: MeFPrimitiveNode * node@[DW_OP_reg1(ECX)] */
  if (p->geometry != (MeFGeometry *)0x0) {
    pMVar1 = p->geometry->nilPrimitive;
    pMVar3 = pMVar1->prev;
    if (pMVar3 != pMVar1) {
      pMVar2 = pMVar3->current;
      while ((pMVar2 != p && (pMVar3 = pMVar3->prev, pMVar3 != pMVar1))) {
        pMVar2 = pMVar3->current;
      }
    }
    pMVar1 = pMVar3->prev;
    pMVar1->next = pMVar3->next;
    pMVar3->next->prev = pMVar1;
    (*_MeFPrimitiveScale)(pMVar3);
    p->geometry->primCount = p->geometry->primCount + -1;
    p->geometry = (MeFGeometry *)0x0;
  }
  return;
}


/* ==== MeFGeometryCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFGeometry * MeFGeometryCreate(char *name)

{
  MeFGeometry *pMVar1;
  MeFPrimitiveNode *pMVar2;
  
                    /* Unresolved local var: MeFGeometry * fg@[DW_OP_reg3(EBX)] */
  pMVar1 = (*_MeMemoryAPI)(0x24);
  pMVar2 = (*_MeMemoryAPI)(0xc);
  pMVar1->nilPrimitive = pMVar2;
  pMVar1->id = (char *)0x0;
  pMVar1->asset = (MeFAsset *)0x0;
  pMVar2->prev = pMVar2;
  pMVar2->next = pMVar2;
  pMVar1->nilPrimitive->current = (MeFPrimitive *)0x0;
  pMVar1->graphicHint = (char *)0x0;
  pMVar1->graphicScale = 1.0;
  pMVar1->graphicOffset[0] = 0.0;
  pMVar1->graphicOffset[1] = 0.0;
  pMVar1->graphicOffset[2] = 0.0;
  pMVar1->primCount = 0;
  _FSetStringProperty(pMVar1,name);
  return pMVar1;
}


/* ==== MeFGeometryCreateCopy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFGeometry * MeFGeometryCreateCopy(MeFGeometry *geometry,MeBool recurse)

{
  MeFPrimitive **ppMVar1;
  MeReal MVar2;
  MeReal MVar3;
  MeFPrimitiveNode *pMVar4;
  MeFGeometry *geometry_00;
  MeFPrimitiveNode *pMVar5;
  MeFPrimitive *pMVar6;
  
                    /* Unresolved local var: MeFGeometry * copy@[DW_OP_reg6(ESI)] */
  geometry_00 = (*_MeFPrimitiveCreateCopy)(0x24);
  pMVar5 = (*_MeMemoryAPI)(0xc);
  geometry_00->nilPrimitive = pMVar5;
  pMVar5->prev = pMVar5;
  pMVar5->next = pMVar5;
  geometry_00->nilPrimitive->current = (MeFPrimitive *)0x0;
  _FSetStringProperty(geometry_00,geometry->id);
  _FSetStringProperty(&geometry_00->graphicHint,geometry->graphicHint);
  geometry_00->graphicScale = geometry->graphicScale;
  MVar2 = geometry->graphicOffset[1];
  MVar3 = geometry->graphicOffset[2];
  geometry_00->graphicOffset[0] = geometry->graphicOffset[0];
  geometry_00->graphicOffset[1] = MVar2;
  geometry_00->graphicOffset[2] = MVar3;
  if (recurse != 0) {
                    /* Unresolved local var: MeFPrimitive * prim@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeFPrimitiveIt it@[DW_OP_reg3(EBX)] */
    pMVar5 = geometry->nilPrimitive->prev;
    pMVar4 = pMVar5->prev;
    pMVar6 = pMVar5->current;
    while (pMVar6 != (MeFPrimitive *)0x0) {
                    /* Unresolved local var: MeFPrimitive * copyPrim@[DW_OP_reg0(EAX)] */
      pMVar6 = MeFPrimitiveCreateCopy(pMVar6,recurse);
      MeFGeometryInsertPrimitive(geometry_00,pMVar6);
      ppMVar1 = &pMVar4->current;
      pMVar4 = pMVar4->prev;
                    /* Unresolved local var: MeFPrimitive * prim@[DW_OP_reg0(EAX)] */
      pMVar6 = *ppMVar1;
    }
  }
  return geometry_00;
}


/* ==== MeFGeometryDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFGeometryDestroy(MeFGeometry *fg)

{
  MeFPrimitive *p;
  
  if (fg != (MeFGeometry *)0x0) {
                    /* Unresolved local var: MeFPrimitiveIt it@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeFPrimitive * prim@[DW_OP_reg3(EBX)] */
    p = fg->nilPrimitive->prev->current;
    while (p != (MeFPrimitive *)0x0) {
      MeFGeometryRemovePrimitive(p);
      MeFPrimitiveDestroy(p);
      p = fg->nilPrimitive->prev->current;
    }
    (*_MeFPrimitiveScale)(fg->id);
    (*_MeFPrimitiveScale)(fg->nilPrimitive);
    if (fg->graphicHint != (char *)0x0) {
      (*_MeFPrimitiveScale)(fg->graphicHint);
    }
    (*_MeFPrimitiveScale)(fg);
  }
  return;
}


/* ==== MeFGeometryGetName ==== */

char * MeFGeometryGetName(MeFGeometry *fg)

{
  return fg->id;
}


/* ==== MeFGeometryGetGraphicHint ==== */

char * MeFGeometryGetGraphicHint(MeFGeometry *fg)

{
  return fg->graphicHint;
}


/* ==== MeFGeometryGetGraphicScale ==== */

MeReal MeFGeometryGetGraphicScale(MeFGeometry *fg)

{
  return fg->graphicScale;
}


/* ==== MeFGeometryGetGraphicOffset ==== */

void MeFGeometryGetGraphicOffset(MeFGeometry *geometry,MeReal *v)

{
  *v = geometry->graphicOffset[0];
  v[1] = geometry->graphicOffset[1];
  v[2] = geometry->graphicOffset[2];
  return;
}


/* ==== MeFGeometryInitPrimitiveIterator ==== */

void MeFGeometryInitPrimitiveIterator(MeFGeometry *geometry,MeFPrimitiveIt *it)

{
  it->node = geometry->nilPrimitive->prev;
  return;
}


/* ==== MeFGeometryGetPrimitive ==== */

MeFPrimitive * MeFGeometryGetPrimitive(MeFPrimitiveIt *it)

{
  MeFPrimitive *pMVar1;
  
                    /* Unresolved local var: MeFPrimitive * prim@[DW_OP_reg0(EAX)] */
  pMVar1 = it->node->current;
  it->node = it->node->prev;
  return pMVar1;
}


/* ==== MeFGeometryGetPrimitiveCount ==== */

int MeFGeometryGetPrimitiveCount(MeFGeometry *geometry)

{
  return geometry->primCount;
}


/* ==== MeFGeometrySetGraphicHint ==== */

void MeFGeometrySetGraphicHint(MeFGeometry *fg,char *hint)

{
  _FSetStringProperty(&fg->graphicHint,hint);
  return;
}


/* ==== MeFGeometrySetGraphicScale ==== */

void MeFGeometrySetGraphicScale(MeFGeometry *fg,MeReal scale)

{
  fg->graphicScale = scale;
  return;
}


/* ==== MeFGeometrySetGraphicOffset ==== */

void MeFGeometrySetGraphicOffset(MeFGeometry *geometry,MeReal dx,MeReal dy,MeReal dz)

{
  geometry->graphicOffset[0] = dx;
  geometry->graphicOffset[1] = dy;
  geometry->graphicOffset[2] = dz;
  return;
}


/* ==== MeFGeometryScale ==== */

void MeFGeometryScale(MeFGeometry *geometry,MeReal scale)

{
  MeFPrimitive **ppMVar1;
  MeFPrimitiveNode *pMVar2;
  MeFPrimitive *pMVar3;
  MeFPrimitiveNode *pMVar4;
  
                    /* Unresolved local var: MeFPrimitiveIt it@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeFPrimitive * p@[DW_OP_reg0(EAX)] */
  pMVar2 = geometry->nilPrimitive->prev;
  pMVar4 = pMVar2->prev;
  pMVar3 = pMVar2->current;
  while (pMVar3 != (MeFPrimitive *)0x0) {
    MeFPrimitiveScale(pMVar3,scale);
    ppMVar1 = &pMVar4->current;
    pMVar4 = pMVar4->prev;
                    /* Unresolved local var: MeFPrimitive * prim@[DW_OP_reg0(EAX)] */
    pMVar3 = *ppMVar1;
  }
  geometry->graphicScale = geometry->graphicScale * scale;
  geometry->graphicOffset[0] = geometry->graphicOffset[0] * scale;
  geometry->graphicOffset[1] = geometry->graphicOffset[1] * scale;
  geometry->graphicOffset[2] = scale * geometry->graphicOffset[2];
  return;
}


/* ==== MeFGeometryScaleNoGraphic ==== */

void MeFGeometryScaleNoGraphic(MeFGeometry *geometry,MeReal scale)

{
  MeFPrimitive **ppMVar1;
  MeFPrimitiveNode *pMVar2;
  MeFPrimitive *pMVar3;
  MeFPrimitiveNode *pMVar4;
  
                    /* Unresolved local var: MeFPrimitiveIt it@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeFPrimitive * p@[DW_OP_reg0(EAX)] */
  pMVar2 = geometry->nilPrimitive->prev;
  pMVar4 = pMVar2->prev;
  pMVar3 = pMVar2->current;
  while (pMVar3 != (MeFPrimitive *)0x0) {
    MeFPrimitiveScale(pMVar3,scale);
    ppMVar1 = &pMVar4->current;
    pMVar4 = pMVar4->prev;
                    /* Unresolved local var: MeFPrimitive * prim@[DW_OP_reg0(EAX)] */
    pMVar3 = *ppMVar1;
  }
  return;
}


