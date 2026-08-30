/* ==== MeFAssetPartRename ==== */

void MeFAssetPartRename(MeFAssetPart *part,char *newName)

{
  int iVar1;
  
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar1 = strcmp(part->id,newName);
  if (iVar1 != 0) {
    if (part->asset != (MeFAsset *)0x0) {
      MeFAssetResolvePartReferences(part->asset,part->id,newName);
      MeHashDelete(part->id,part->asset->nameToPart);
    }
    _FSetStringProperty(part,newName);
    if (part->asset != (MeFAsset *)0x0) {
      MeHashInsert(part->id,part,part->asset->nameToPart);
    }
  }
  return;
}


/* ==== MeFAssetPartCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFAssetPart * MeFAssetPartCreate(char *name,MeFModel *model,MeMatrix4Ptr relTM)

{
  MeFAssetPart *pMVar1;
  
                    /* Unresolved local var: MeFAssetPart * p@[DW_OP_reg6(ESI)] */
  pMVar1 = (MeFAssetPart *)(*_MeMatrix4IsTM)(0x68);
  pMVar1->id = (char *)0x0;
  pMVar1->asset = (MeFAsset *)0x0;
  pMVar1->graphicHint = (char *)0x0;
  pMVar1->model = (char *)0x0;
  pMVar1->parent = (char *)0x0;
  pMVar1->tm[0][0] = 1.0;
  pMVar1->tm[0][1] = 0.0;
  pMVar1->tm[0][2] = 0.0;
  pMVar1->tm[0][3] = 0.0;
  pMVar1->tm[1][0] = 0.0;
  pMVar1->tm[1][1] = 1.0;
  pMVar1->tm[1][2] = 0.0;
  pMVar1->tm[1][3] = 0.0;
  pMVar1->tm[2][0] = 0.0;
  pMVar1->tm[2][1] = 0.0;
  pMVar1->tm[2][2] = 1.0;
  pMVar1->tm[2][3] = 0.0;
  pMVar1->tm[3][0] = 0.0;
  pMVar1->tm[3][1] = 0.0;
  pMVar1->tm[3][2] = 0.0;
  pMVar1->tm[3][3] = 1.0;
  pMVar1->graphicScale = 1.0;
  pMVar1->graphicOffset[0] = 0.0;
  pMVar1->graphicOffset[1] = 0.0;
  pMVar1->graphicOffset[2] = 0.0;
  pMVar1->index = -1;
  _FSetStringProperty(pMVar1,name);
  _FSetStringProperty(&pMVar1->model,model->id);
  if (relTM != (MeMatrix4Ptr)0x0) {
    MeMatrix4IsTM(relTM,0.001);
    pMVar1->tm[0][0] = (*relTM)[0];
    pMVar1->tm[0][1] = (*relTM)[1];
    pMVar1->tm[0][2] = (*relTM)[2];
    pMVar1->tm[0][3] = (*relTM)[3];
    pMVar1->tm[1][0] = relTM[1][0];
    pMVar1->tm[1][1] = relTM[1][1];
    pMVar1->tm[1][2] = relTM[1][2];
    pMVar1->tm[1][3] = relTM[1][3];
    pMVar1->tm[2][0] = relTM[2][0];
    pMVar1->tm[2][1] = relTM[2][1];
    pMVar1->tm[2][2] = relTM[2][2];
    pMVar1->tm[2][3] = relTM[2][3];
    pMVar1->tm[3][0] = relTM[3][0];
    pMVar1->tm[3][1] = relTM[3][1];
    pMVar1->tm[3][2] = relTM[3][2];
    pMVar1->tm[3][3] = relTM[3][3];
  }
  return pMVar1;
}


/* ==== MeFAssetPartCreateCopy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFAssetPart * MeFAssetPartCreateCopy(MeFAssetPart *part,MeBool recurse)

{
  MeReal MVar1;
  MeReal MVar2;
  MeFAssetPart *pMVar3;
  
                    /* Unresolved local var: MeFAssetPart * copy@[DW_OP_reg3(EBX)] */
  pMVar3 = (MeFAssetPart *)(*_MeMatrix4IsTM)(0x68);
  _FSetStringProperty(pMVar3,part->id);
  _FSetStringProperty(&pMVar3->model,part->model);
  pMVar3->tm[0][0] = part->tm[0][0];
  pMVar3->tm[0][1] = part->tm[0][1];
  pMVar3->tm[0][2] = part->tm[0][2];
  pMVar3->tm[0][3] = part->tm[0][3];
  pMVar3->tm[1][0] = part->tm[1][0];
  pMVar3->tm[1][1] = part->tm[1][1];
  pMVar3->tm[1][2] = part->tm[1][2];
  pMVar3->tm[1][3] = part->tm[1][3];
  pMVar3->tm[2][0] = part->tm[2][0];
  pMVar3->tm[2][1] = part->tm[2][1];
  pMVar3->tm[2][2] = part->tm[2][2];
  pMVar3->tm[2][3] = part->tm[2][3];
  pMVar3->tm[3][0] = part->tm[3][0];
  pMVar3->tm[3][1] = part->tm[3][1];
  pMVar3->tm[3][2] = part->tm[3][2];
  pMVar3->tm[3][3] = part->tm[3][3];
  _FSetStringProperty(&pMVar3->graphicHint,part->graphicHint);
  _FSetStringProperty(&pMVar3->parent,part->parent);
  pMVar3->graphicScale = part->graphicScale;
  MVar1 = part->graphicOffset[1];
  MVar2 = part->graphicOffset[2];
  pMVar3->graphicOffset[0] = part->graphicOffset[0];
  pMVar3->graphicOffset[1] = MVar1;
  pMVar3->graphicOffset[2] = MVar2;
  pMVar3->index = -1;
  return pMVar3;
}


/* ==== MeFAssetPartDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFAssetPartDestroy(MeFAssetPart *p)

{
  (*_MeFAssetLookupGeometry)(p->id);
  (*_MeFAssetLookupGeometry)(p->model);
  if (p->graphicHint != (char *)0x0) {
    (*_MeFAssetLookupGeometry)(p->graphicHint);
  }
  if (p->parent != (char *)0x0) {
    (*_MeFAssetLookupGeometry)(p->parent);
  }
  (*_MeFAssetLookupGeometry)(p);
  return;
}


/* ==== MeFAssetPartGetName ==== */

char * MeFAssetPartGetName(MeFAssetPart *p)

{
  return p->id;
}


/* ==== MeFAssetPartGetModelName ==== */

char * MeFAssetPartGetModelName(MeFAssetPart *p)

{
  return p->model;
}


/* ==== MeFAssetPartGetModel ==== */

MeFModel * MeFAssetPartGetModel(MeFAssetPart *p)

{
  MeFModel *pMVar1;
  
                    /* Unresolved local var: MeFModel * m@[DW_OP_reg0(EAX)] */
  pMVar1 = MeFAssetLookupModel(p->asset,p->model);
  return pMVar1;
}


/* ==== MeFAssetPartGetGeometry ==== */

MeFGeometry * MeFAssetPartGetGeometry(MeFAssetPart *p)

{
  void *pvVar1;
  MeFGeometry *pMVar2;
  
                    /* Unresolved local var: MeFModel * m@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeFGeometry * g@[DW_OP_reg6(ESI)] */
  pMVar2 = (MeFGeometry *)0x0;
  if (p->asset == (MeFAsset *)0x0) {
    pMVar2 = (MeFGeometry *)0x0;
  }
  else {
    pvVar1 = MeFAssetLookupModel(p->asset,p->model);
    if (*(void **)((int)pvVar1 + 0xc) != (void *)0x0) {
      pMVar2 = MeFAssetLookupGeometry(p->asset,*(void **)((int)pvVar1 + 0xc));
    }
  }
  return pMVar2;
}


/* ==== MeFAssetPartGetTransformPtr ==== */

MeMatrix4Ptr MeFAssetPartGetTransformPtr(MeFAssetPart *p)

{
  return p->tm;
}


/* ==== MeFAssetPartGetPosition ==== */

void MeFAssetPartGetPosition(MeFAssetPart *part,MeReal *pos)

{
  *pos = part->tm[3][0];
  pos[1] = part->tm[3][1];
  pos[2] = part->tm[3][2];
  return;
}


/* ==== MeFAssetPartGetGraphicHint ==== */

char * MeFAssetPartGetGraphicHint(MeFAssetPart *p)

{
  return p->graphicHint;
}


/* ==== MeFAssetPartGetGraphicScale ==== */

MeReal MeFAssetPartGetGraphicScale(MeFAssetPart *p)

{
  return p->graphicScale;
}


/* ==== MeFAssetPartGetGraphicOffset ==== */

void MeFAssetPartGetGraphicOffset(MeFAssetPart *p,MeReal *v)

{
  *v = p->graphicOffset[0];
  v[1] = p->graphicOffset[1];
  v[2] = p->graphicOffset[2];
  return;
}


/* ==== MeFAssetPartGetParentPart ==== */

MeFAssetPart * MeFAssetPartGetParentPart(MeFAssetPart *part)

{
  MeFAssetPart *pMVar1;
  
                    /* Unresolved local var: MeFAssetPart * p@[DW_OP_reg0(EAX)] */
  pMVar1 = (MeFAssetPart *)0x0;
  if (((part->asset != (MeFAsset *)0x0) && (part != (MeFAssetPart *)0x0)) &&
     (part->parent != (char *)0x0)) {
    pMVar1 = MeFAssetLookupPart(part->asset,part->parent);
  }
  return pMVar1;
}


/* ==== MeFAssetPartGetParentPartName ==== */

char * MeFAssetPartGetParentPartName(MeFAssetPart *part)

{
  return part->parent;
}


/* ==== MeFAssetPartIsCollisionEnabled ==== */

MeBool MeFAssetPartIsCollisionEnabled(MeFAssetPart *p1,MeFAssetPart *p2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  
                    /* Unresolved local var: MeU32 * p@[???]
                       Unresolved local var: MeU32 mask@[DW_OP_reg0(EAX)] */
  uVar2 = 1;
  if (p1 != p2) {
                    /* Unresolved local var: int index@[DW_OP_reg1(ECX)]
                       Unresolved local var: int block@[DW_OP_reg3(EBX)]
                       Unresolved local var: int stride@[???]
                       Unresolved local var: int row@[DW_OP_reg2(EDX)]
                       Unresolved local var: int col@[DW_OP_reg0(EAX)]
                       Unresolved local var: int size@[DW_OP_reg1(ECX)] */
    iVar4 = p1->index;
    iVar1 = p2->index;
    iVar3 = iVar1;
    if (iVar1 < iVar4) {
      iVar3 = iVar4;
      iVar4 = iVar1;
    }
    iVar3 = iVar3 + iVar4 * p1->asset->maxParts;
    iVar4 = iVar3;
    if (iVar3 < 0) {
      iVar4 = iVar3 + 0x1f;
    }
    uVar2 = (uint)((p1->asset->disabledColArray[iVar4 >> 5] & 0x80000000U >> ((byte)iVar3 & 0x1f))
                  == 0);
  }
  return uVar2;
}


/* ==== MeFAssetPartGetDisabledCollisionIndex ==== */

int MeFAssetPartGetDisabledCollisionIndex(MeFAssetPart *part)

{
  return part->index;
}


/* ==== MeFAssetPartSetGraphicHint ==== */

void MeFAssetPartSetGraphicHint(MeFAssetPart *part,char *hint)

{
  _FSetStringProperty(&part->graphicHint,hint);
  return;
}


/* ==== MeFAssetPartSetGraphicScale ==== */

void MeFAssetPartSetGraphicScale(MeFAssetPart *part,MeReal scale)

{
  part->graphicScale = scale;
  return;
}


/* ==== MeFAssetPartScale ==== */

void MeFAssetPartScale(MeFAssetPart *part,MeReal scale)

{
  part->tm[3][0] = part->tm[3][0] * scale;
  part->tm[3][1] = part->tm[3][1] * scale;
  part->tm[3][2] = part->tm[3][2] * scale;
  part->graphicScale = part->graphicScale * scale;
  part->graphicOffset[0] = part->graphicOffset[0] * scale;
  part->graphicOffset[1] = part->graphicOffset[1] * scale;
  part->graphicOffset[2] = scale * part->graphicOffset[2];
  return;
}


/* ==== MeFAssetPartSetGraphicOffset ==== */

void MeFAssetPartSetGraphicOffset(MeFAssetPart *part,MeReal dx,MeReal dy,MeReal dz)

{
  part->graphicOffset[0] = dx;
  part->graphicOffset[1] = dy;
  part->graphicOffset[2] = dz;
  return;
}


/* ==== MeFAssetPartSetTransform ==== */

void MeFAssetPartSetTransform(MeFAssetPart *part,MeMatrix4Ptr tm)

{
  part->tm[0][0] = (*tm)[0];
  part->tm[0][1] = (*tm)[1];
  part->tm[0][2] = (*tm)[2];
  part->tm[0][3] = (*tm)[3];
  part->tm[1][0] = tm[1][0];
  part->tm[1][1] = tm[1][1];
  part->tm[1][2] = tm[1][2];
  part->tm[1][3] = tm[1][3];
  part->tm[2][0] = tm[2][0];
  part->tm[2][1] = tm[2][1];
  part->tm[2][2] = tm[2][2];
  part->tm[2][3] = tm[2][3];
  part->tm[3][0] = tm[3][0];
  part->tm[3][1] = tm[3][1];
  part->tm[3][2] = tm[3][2];
  part->tm[3][3] = tm[3][3];
  MeMatrix4TMOrthoNormalize(part->tm);
  MeMatrix4IsTM(tm,0.001);
  return;
}


/* ==== MeFAssetPartSetPosition ==== */

void MeFAssetPartSetPosition(MeFAssetPart *part,MeReal x,MeReal y,MeReal z)

{
  part->tm[3][0] = x;
  part->tm[3][1] = y;
  part->tm[3][2] = z;
  return;
}


/* ==== MeFAssetPartSetParentPart ==== */

void MeFAssetPartSetParentPart(MeFAssetPart *part,MeFAssetPart *parent)

{
  if (parent == (MeFAssetPart *)0x0) {
    parent = (MeFAssetPart *)0x0;
  }
  else {
    parent = (MeFAssetPart *)parent->id;
  }
  _FSetStringProperty(&part->parent,parent);
  return;
}


/* ==== MeFAssetPartSetParentPartByName ==== */

void MeFAssetPartSetParentPartByName(MeFAssetPart *part,char *name)

{
  _FSetStringProperty(&part->parent,name);
  return;
}


/* ==== MeFAssetPartSetModel ==== */

void MeFAssetPartSetModel(MeFAssetPart *part,MeFModel *model)

{
  if (model != (MeFModel *)0x0) {
    _FSetStringProperty(&part->model,model->id);
    return;
  }
  return;
}


/* ==== MeFAssetPartSetModelByName ==== */

void MeFAssetPartSetModelByName(MeFAssetPart *part,char *name)

{
  _FSetStringProperty(&part->model,name);
  return;
}


/* ==== MeFAssetPartEnableCollision ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFAssetPartEnableCollision(MeFAssetPart *p1,MeFAssetPart *p2,MeBool enable)

{
  uint *puVar1;
  MeFAsset *pMVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  
                    /* Unresolved local var: MeU32 * p@[???]
                       Unresolved local var: MeU32 mask@[???] */
  if ((p1 != p2) &&
     ((pMVar2 = p1->asset, iVar5 = _DAT_00000050, pMVar2 == (MeFAsset *)0x0 ||
      (iVar5 = pMVar2->maxParts, pMVar2->partCount <= pMVar2->maxParts)))) {
                    /* Unresolved local var: int index@[DW_OP_reg1(ECX)]
                       Unresolved local var: int block@[DW_OP_reg0(EAX)]
                       Unresolved local var: int stride@[???]
                       Unresolved local var: int row@[DW_OP_reg2(EDX)]
                       Unresolved local var: int col@[DW_OP_reg0(EAX)]
                       Unresolved local var: int size@[???] */
    iVar6 = p1->index;
    iVar3 = p2->index;
    iVar4 = iVar3;
    if (iVar3 < iVar6) {
      iVar4 = iVar6;
      iVar6 = iVar3;
    }
    iVar4 = iVar4 + iVar6 * iVar5;
    iVar5 = iVar4;
    if (iVar4 < 0) {
      iVar5 = iVar4 + 0x1f;
    }
    uVar7 = 0x80000000 >> ((byte)iVar4 & 0x1f);
    puVar1 = pMVar2->disabledColArray + (iVar5 >> 5);
    if (enable == 0) {
      *puVar1 = *puVar1 | uVar7;
    }
    else {
      *puVar1 = *puVar1 & (uVar7 ^ 0xffffffff);
    }
  }
  return;
}


/* ==== MeFAssetPartEnableAllCollisions ==== */

void MeFAssetPartEnableAllCollisions(MeFAssetPart *part)

{
  MeFAssetPart *p1;
  MeFAssetPartIt it;
  
                    /* Unresolved local var: MeFAssetPart * p@[DW_OP_reg0(EAX)] */
  if (part->asset != (MeFAsset *)0x0) {
    MeFAssetInitPartIterator(part->asset,&it);
    while( true ) {
      p1 = MeFAssetGetPart(&it);
      if (p1 == (MeFAssetPart *)0x0) break;
      MeFAssetPartEnableCollision(p1,part,1);
    }
  }
  return;
}


