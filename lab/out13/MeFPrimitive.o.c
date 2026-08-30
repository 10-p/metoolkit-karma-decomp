/* ==== MeFPrimitiveRename ==== */

void MeFPrimitiveRename(MeFPrimitive *p,char *newName)

{
  int iVar1;
  
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar1 = strcmp(p->id,newName);
  if (iVar1 == 0) {
    return;
  }
  _FSetStringProperty(p,newName);
  return;
}


/* ==== MeFPrimitiveCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFPrimitive * MeFPrimitiveCreate(char *name,MeFPrimitiveType type)

{
  MeFPrimitive *pMVar1;
  MeVector3 *paMVar2;
  
                    /* Unresolved local var: MeFPrimitive * p@[DW_OP_reg3(EBX)] */
  pMVar1 = (*_DAT_0001100c)(100);
  pMVar1->id = (char *)0x0;
  pMVar1->geometry = (MeFGeometry *)0x0;
  pMVar1->nVertices = 0;
  pMVar1->maxVertices = 1;
  paMVar2 = (*_MeMemoryAPI)(0xc);
  pMVar1->vertices = paMVar2;
  pMVar1->dims[2] = 1.0;
  pMVar1->dims[1] = 1.0;
  pMVar1->dims[0] = 1.0;
  pMVar1->tm[0][0] = 1.0;
  pMVar1->type = kMeFPrimitiveTypeUnknown;
  pMVar1->tm[1][1] = 1.0;
  pMVar1->tm[2][2] = 1.0;
  pMVar1->tm[3][3] = 1.0;
  pMVar1->tm[0][1] = 0.0;
  pMVar1->tm[0][2] = 0.0;
  pMVar1->tm[0][3] = 0.0;
  pMVar1->tm[1][0] = 0.0;
  pMVar1->tm[1][2] = 0.0;
  pMVar1->tm[1][3] = 0.0;
  pMVar1->tm[2][0] = 0.0;
  pMVar1->tm[2][1] = 0.0;
  pMVar1->tm[2][3] = 0.0;
  pMVar1->tm[3][0] = 0.0;
  pMVar1->tm[3][1] = 0.0;
  pMVar1->tm[3][2] = 0.0;
  _FSetStringProperty(pMVar1,name);
  pMVar1->type = type;
  return pMVar1;
}


/* ==== MeFPrimitiveCreateCopy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFPrimitive * MeFPrimitiveCreateCopy(MeFPrimitive *p,MeBool recurse)

{
  int iVar1;
  MeFPrimitive *pMVar2;
  MeReal *pMVar3;
  uint uVar4;
  MeReal *pMVar5;
  
                    /* Unresolved local var: MeFPrimitive * copy@[DW_OP_reg3(EBX)] */
  pMVar2 = (*_DAT_0001100c)(100);
  _FSetStringProperty(pMVar2,p->id);
  pMVar2->type = p->type;
  pMVar2->dims[0] = p->dims[0];
  pMVar2->dims[1] = p->dims[1];
  pMVar2->dims[2] = p->dims[2];
  pMVar2->tm[0][0] = p->tm[0][0];
  pMVar2->tm[0][1] = p->tm[0][1];
  pMVar2->tm[0][2] = p->tm[0][2];
  pMVar2->tm[0][3] = p->tm[0][3];
  pMVar2->tm[1][0] = p->tm[1][0];
  pMVar2->tm[1][1] = p->tm[1][1];
  pMVar2->tm[1][2] = p->tm[1][2];
  pMVar2->tm[1][3] = p->tm[1][3];
  pMVar2->tm[2][0] = p->tm[2][0];
  pMVar2->tm[2][1] = p->tm[2][1];
  pMVar2->tm[2][2] = p->tm[2][2];
  pMVar2->tm[2][3] = p->tm[2][3];
  pMVar2->tm[3][0] = p->tm[3][0];
  pMVar2->tm[3][1] = p->tm[3][1];
  pMVar2->tm[3][2] = p->tm[3][2];
  pMVar2->tm[3][3] = p->tm[3][3];
  iVar1 = p->nVertices;
  pMVar2->maxVertices = p->maxVertices;
  pMVar2->nVertices = iVar1;
  pMVar3 = (*_MeMemoryAPI)(p->maxVertices * 0xc);
  pMVar2->vertices = (MeVector3 *)pMVar3;
  uVar4 = p->nVertices * 3 & 0x3fffffff;
  pMVar5 = *p->vertices;
  for (; uVar4 != 0; uVar4 = uVar4 - 1) {
    *pMVar3 = *pMVar5;
    pMVar5 = pMVar5 + 1;
    pMVar3 = pMVar3 + 1;
  }
  return pMVar2;
}


/* ==== MeFPrimitiveDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFPrimitiveDestroy(MeFPrimitive *p)

{
  if (p != (MeFPrimitive *)0x0) {
    (*_DAT_00011014)(p->vertices);
    (*_DAT_00011014)(p->id);
    (*_DAT_00011014)(p);
  }
  return;
}


/* ==== MeFPrimitiveGetType ==== */

MeFPrimitiveType MeFPrimitiveGetType(MeFPrimitive *p)

{
  return p->type;
}


/* ==== MeFPrimitiveGetName ==== */

char * MeFPrimitiveGetName(MeFPrimitive *p)

{
  return p->id;
}


/* ==== MeFPrimitiveGetRadius ==== */

MeReal MeFPrimitiveGetRadius(MeFPrimitive *p)

{
  return p->dims[0];
}


/* ==== MeFPrimitiveGetDimensions ==== */

void MeFPrimitiveGetDimensions(MeFPrimitive *p,MeReal *dims)

{
  *dims = p->dims[0];
  dims[1] = p->dims[1];
  dims[2] = p->dims[2];
  return;
}


/* ==== MeFPrimitiveGetHeight ==== */

MeReal MeFPrimitiveGetHeight(MeFPrimitive *p)

{
  return p->dims[1];
}


/* ==== MeFPrimitiveInitVertexIterator ==== */

void MeFPrimitiveInitVertexIterator(MeFPrimitive *p,MeFVertexIt *it)

{
  it->currentVertex = 0;
  it->prim = p;
  return;
}


/* ==== MeFPrimitiveGetVertex ==== */

MeReal * MeFPrimitiveGetVertex(MeFVertexIt *it)

{
  int iVar1;
  MeVector3 *paMVar2;
  
                    /* Unresolved local var: MeReal * v@[???] */
  iVar1 = it->currentVertex;
  if (iVar1 == it->prim->nVertices) {
    paMVar2 = (MeVector3 *)0x0;
  }
  else {
    paMVar2 = it->prim->vertices;
    it->currentVertex = iVar1 + 1;
    paMVar2 = paMVar2 + iVar1;
  }
  return *paMVar2;
}


/* ==== MeFPrimitiveGetVertexCount ==== */

int MeFPrimitiveGetVertexCount(MeFPrimitive *p)

{
  return p->nVertices;
}


/* ==== MeFPrimitiveGetVertexArray ==== */

MeVector3 * MeFPrimitiveGetVertexArray(MeFPrimitive *p)

{
  return p->vertices;
}


/* ==== MeFPrimitiveGetTransformPtr ==== */

MeMatrix4Ptr MeFPrimitiveGetTransformPtr(MeFPrimitive *p)

{
  return p->tm;
}


/* ==== MeFPrimitiveSetRadius ==== */

void MeFPrimitiveSetRadius(MeFPrimitive *p,MeReal r)

{
  p->dims[0] = r;
  return;
}


/* ==== MeFPrimitiveSetDimensions ==== */

void MeFPrimitiveSetDimensions(MeFPrimitive *p,MeReal dx,MeReal dy,MeReal dz)

{
  p->dims[0] = dx;
  p->dims[1] = dy;
  p->dims[2] = dz;
  return;
}


/* ==== MeFPrimitiveSetHeight ==== */

void MeFPrimitiveSetHeight(MeFPrimitive *p,MeReal height)

{
  p->dims[1] = height;
  return;
}


/* ==== MeFPrimitiveAddVertex ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFPrimitiveAddVertex(MeFPrimitive *p,MeReal *pos)

{
  int iVar1;
  MeVector3 *paMVar2;
  
  iVar1 = p->nVertices;
  p->nVertices = iVar1 + 1;
  paMVar2 = p->vertices + iVar1;
  (*paMVar2)[0] = *pos;
  (*paMVar2)[1] = pos[1];
  (*paMVar2)[2] = pos[2];
  iVar1 = p->maxVertices;
  if (iVar1 <= p->nVertices) {
    p->maxVertices = iVar1 * 2;
    paMVar2 = (*_DAT_0001101c)(p->vertices,iVar1 * 0x18);
    p->vertices = paMVar2;
  }
  return;
}


/* ==== MeFPrimitiveSetVertexArray ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFPrimitiveSetVertexArray(MeFPrimitive *p,MeVector3 *vertices,int nVertices)

{
  MeReal *pMVar1;
  uint uVar2;
  
  pMVar1 = (*_DAT_0001101c)(p->vertices,nVertices * 0xc);
  p->vertices = (MeVector3 *)pMVar1;
  for (uVar2 = (uint)(nVertices * 0xc) >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
    *pMVar1 = (MeReal)(*vertices)[0];
    vertices = (MeVector3 *)((int)vertices + 4);
    pMVar1 = pMVar1 + 1;
  }
  p->nVertices = nVertices;
  p->maxVertices = nVertices;
  return;
}


/* ==== MeFPrimitiveSetTransform ==== */

void MeFPrimitiveSetTransform(MeFPrimitive *p,MeMatrix4Ptr tm)

{
  p->tm[0][0] = (*tm)[0];
  p->tm[0][1] = (*tm)[1];
  p->tm[0][2] = (*tm)[2];
  p->tm[0][3] = (*tm)[3];
  p->tm[1][0] = tm[1][0];
  p->tm[1][1] = tm[1][1];
  p->tm[1][2] = tm[1][2];
  p->tm[1][3] = tm[1][3];
  p->tm[2][0] = tm[2][0];
  p->tm[2][1] = tm[2][1];
  p->tm[2][2] = tm[2][2];
  p->tm[2][3] = tm[2][3];
  p->tm[3][0] = tm[3][0];
  p->tm[3][1] = tm[3][1];
  p->tm[3][2] = tm[3][2];
  p->tm[3][3] = tm[3][3];
  return;
}


/* ==== MeFPrimitiveScale ==== */

void MeFPrimitiveScale(MeFPrimitive *p,MeReal scale)

{
  float fVar1;
  MeVector3 *paMVar2;
  MeFVertexIt it;
  
                    /* Unresolved local var: MeReal * v@[???] */
  it.currentVertex = 0;
  while( true ) {
                    /* Unresolved local var: MeReal * v@[???] */
    if (it.currentVertex == p->nVertices) {
      paMVar2 = (MeVector3 *)0x0;
    }
    else {
      paMVar2 = p->vertices + it.currentVertex;
      it.currentVertex = it.currentVertex + 1;
    }
    if (paMVar2 == (MeVector3 *)0x0) break;
    (*paMVar2)[0] = (*paMVar2)[0] * scale;
    (*paMVar2)[1] = (*paMVar2)[1] * scale;
    (*paMVar2)[2] = (*paMVar2)[2] * scale;
  }
  p->dims[0] = p->dims[0] * scale;
  p->dims[1] = p->dims[1] * scale;
  p->dims[2] = p->dims[2] * scale;
  p->tm[3][0] = p->tm[3][0] * scale;
  fVar1 = p->tm[3][2];
  p->tm[3][1] = p->tm[3][1] * scale;
  p->tm[3][2] = scale * fVar1;
  return;
}


