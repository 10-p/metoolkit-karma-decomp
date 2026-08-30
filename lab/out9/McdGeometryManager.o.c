/* ==== McdGMCreateGeometry ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdGeometryID McdGMCreateGeometry(McdGeomMan *gm,MeFGeometry *fg,char *assetName)

{
  int iVar1;
  char *pcVar2;
  size_t sVar3;
  size_t sVar4;
  char *__dest;
  McdGeometryID pMVar5;
  void *pvVar6;
  char *__s;
  size_t sVar7;
  char *__dest_00;
  void *pvVar8;
  float fVar9;
  float fVar10;
  char *agName;
  char *agHashName;
  McdGeometryID agGeom;
  MeFPrimitiveIt it;
  MeVector3 dims;
  
                    /* Unresolved local var: int nPrimitives@[DW_OP_reg7(EDI)] */
  iVar1 = MeFGeometryGetPrimitiveCount(fg);
  pcVar2 = MeFGeometryGetName(fg);
  sVar3 = strlen(assetName);
  sVar4 = strlen(pcVar2);
  __dest = (*_MeMemoryAPI)(sVar4 + 3 + sVar3);
  *__dest = '\0';
  strcat(__dest,assetName);
  strcat(__dest,"::");
  strcat(__dest,pcVar2);
  pMVar5 = MeHashLookup(__dest,gm->name2geom);
  if (pMVar5 == (McdGeometryID)0x0) {
                    /* Unresolved local var: MeFPrimitive * prim@[DW_OP_reg7(EDI)] */
    pMVar5 = McdAggregateCreate(gm->fwk,iVar1);
    MeFGeometryInitPrimitiveIterator(fg,&it);
    while (pvVar6 = MeFGeometryGetPrimitive(&it), pvVar6 != (void *)0x0) {
                    /* Unresolved local var: McdGeometryID primGeom@[DW_OP_reg3(EBX)]
                       Unresolved local var: char * hashName@[DW_OP_reg6(ESI)]
                       Unresolved local var: char * primName@[DW_OP_reg3(EBX)] */
      __s = MeFPrimitiveGetName(pvVar6);
      sVar3 = strlen(assetName);
      sVar4 = strlen(pcVar2);
      sVar7 = strlen(__s);
      __dest_00 = (*_MeMemoryAPI)(sVar7 + 5 + sVar3 + sVar4);
      *__dest_00 = '\0';
      strcat(__dest_00,assetName);
      strcat(__dest_00,"::");
      strcat(__dest_00,pcVar2);
      strcat(__dest_00,"::");
      strcat(__dest_00,__s);
      pvVar8 = MeHashLookup(__dest_00,gm->name2geom);
      if (pvVar8 == (void *)0x0) {
                    /* Unresolved local var: MeFPrimitiveType type@[DW_OP_reg0(EAX)] */
        iVar1 = MeFPrimitiveGetType(pvVar6);
        if (iVar1 == 2) {
          MeFPrimitiveGetDimensions(pvVar6,dims);
          pvVar8 = McdBoxCreate(gm->fwk,dims[0],dims[1],dims[2]);
        }
        else if (iVar1 == 1) {
          fVar10 = MeFPrimitiveGetRadius(pvVar6);
          pvVar8 = McdSphereCreate(gm->fwk,fVar10);
        }
        else if (iVar1 == 3) {
          fVar10 = MeFPrimitiveGetHeight(pvVar6);
          fVar9 = MeFPrimitiveGetRadius(pvVar6);
          pvVar8 = McdCylinderCreate(gm->fwk,fVar9,fVar10);
        }
        else if (iVar1 == 4) {
          fVar10 = MeFPrimitiveGetHeight(pvVar6);
          fVar9 = MeFPrimitiveGetRadius(pvVar6);
          pvVar8 = McdSphylCreate(gm->fwk,fVar9,fVar10);
        }
        else if (iVar1 == 5) {
          pvVar8 = McdPlaneCreate(gm->fwk);
        }
        else {
          if (iVar1 != 6) {
            pcVar2 = "McdGMCreateGeometry: Unknown Geometry type.";
            goto LAB_00010246;
          }
          iVar1 = MeFPrimitiveGetVertexCount(pvVar6);
          if (0 < iVar1) {
            fVar10 = 0.0;
            iVar1 = MeFPrimitiveGetVertexCount(pvVar6);
            pvVar8 = MeFPrimitiveGetVertexArray(pvVar6);
            pvVar8 = McdConvexMeshCreateHull(gm->fwk,pvVar8,iVar1,fVar10);
          }
        }
        if (pvVar8 == (void *)0x0) {
          pcVar2 = "McdGMCreateGeometry: Failed to create primitive.";
          goto LAB_00010246;
        }
        MeHashInsert(__dest_00,pvVar8,gm->name2geom);
        MeHashInsert(pvVar8,__dest_00,gm->geom2name);
      }
      else {
        (*_McdAggregateCreate)(__dest_00);
      }
      pvVar6 = MeFPrimitiveGetTransformPtr(pvVar6);
      McdAggregateAddElement(pMVar5,pvVar8,pvVar6);
    }
    if (pMVar5 == (McdGeometryID)0x0) {
      pcVar2 = "McdGMCreateGeometry: Failed to create aggregate.";
LAB_00010246:
      MeWarning(0,pcVar2);
      pMVar5 = (McdGeometryID)0x0;
    }
    else {
      MeHashInsert(__dest,pMVar5,gm->name2geom);
      MeHashInsert(pMVar5,__dest,gm->geom2name);
    }
  }
  else {
    (*_McdAggregateCreate)(__dest);
  }
  return pMVar5;
}


/* ==== McdGMDestroyGeometry ==== */

void McdGMDestroyGeometry(McdGeomMan *gm,McdGeometryID geom)

{
  int iVar1;
  void *pvVar2;
  uint uVar3;
  uint uVar4;
  McdGeometryID pMVar5;
  int gCount;
  int g;
  
  iVar1 = McdGeometryGetReferenceCount(geom);
  if (0 < iVar1) {
    return;
  }
  if ((char)geom->mRefCtAndID != '\b') goto LAB_00010379;
  uVar3 = McdAggregateGetElementCount(geom);
  g = 0;
  if ((int)uVar3 < 1) goto LAB_00010379;
  uVar4 = uVar3 & 3;
  if ((int)uVar3 < 2) {
LAB_000103e3:
    pMVar5 = McdAggregateGetElementGeometry(geom,g);
    if (pMVar5 != (McdGeometryID)0x0) {
      McdAggregateRemoveElement(geom,g);
      McdGMDestroyGeometry(gm,pMVar5);
    }
    g = g + 1;
    if ((int)uVar3 <= g) goto LAB_00010379;
  }
  else if (uVar4 != 0) {
    if (1 < uVar4) {
      if (2 < uVar4) {
                    /* Unresolved local var: McdGeometryID partGeom@[???] */
        pMVar5 = McdAggregateGetElementGeometry(geom,0);
        if (pMVar5 != (McdGeometryID)0x0) {
          McdAggregateRemoveElement(geom,0);
          McdGMDestroyGeometry(gm,pMVar5);
        }
        g = 1;
      }
      pMVar5 = McdAggregateGetElementGeometry(geom,g);
      if (pMVar5 != (McdGeometryID)0x0) {
        McdAggregateRemoveElement(geom,g);
        McdGMDestroyGeometry(gm,pMVar5);
      }
      g = g + 1;
    }
    goto LAB_000103e3;
  }
  do {
    pMVar5 = McdAggregateGetElementGeometry(geom,g);
    if (pMVar5 != (McdGeometryID)0x0) {
      McdAggregateRemoveElement(geom,g);
      McdGMDestroyGeometry(gm,pMVar5);
    }
    pMVar5 = McdAggregateGetElementGeometry(geom,g + 1);
    if (pMVar5 != (McdGeometryID)0x0) {
      McdAggregateRemoveElement(geom,g + 1);
      McdGMDestroyGeometry(gm,pMVar5);
    }
    pMVar5 = McdAggregateGetElementGeometry(geom,g + 2);
    if (pMVar5 != (McdGeometryID)0x0) {
      McdAggregateRemoveElement(geom,g + 2);
      McdGMDestroyGeometry(gm,pMVar5);
    }
    pMVar5 = McdAggregateGetElementGeometry(geom,g + 3);
    if (pMVar5 != (McdGeometryID)0x0) {
      McdAggregateRemoveElement(geom,g + 3);
      McdGMDestroyGeometry(gm,pMVar5);
    }
    g = g + 4;
  } while (g < (int)uVar3);
LAB_00010379:
                    /* Unresolved local var: char * name@[DW_OP_reg0(EAX)] */
  if (((char)geom->mRefCtAndID != '\0') &&
     (pvVar2 = MeHashLookup(geom,gm->geom2name), pvVar2 != (void *)0x0)) {
    MeHashDelete(pvVar2,gm->name2geom);
    MeHashDelete(geom,gm->geom2name);
    McdGeometryDestroy(geom);
    return;
  }
  return;
}


/* ==== McdGMCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdGeomMan * McdGMCreate(McdFrameworkID fwk)

{
  McdGeomMan *pMVar1;
  MeHash *pMVar2;
  McdNullID pMVar3;
  
                    /* Unresolved local var: McdGeomMan * gm@[DW_OP_reg3(EBX)] */
  pMVar1 = (*_MeMemoryAPI)(0x10);
  pMVar1->fwk = fwk;
  pMVar2 = MeHashCreate(0x61);
  pMVar1->name2geom = pMVar2;
  pMVar2 = MeHashCreate(0x61);
  pMVar1->geom2name = pMVar2;
  MeHashSetKeyCompareFunc(pMVar1->geom2name,MeHashIntCompare);
  MeHashSetHashFunc(pMVar1->geom2name,MeHashInt);
  MeHashSetKeyFreeFunc(pMVar1->name2geom,_McdAggregateCreate);
  pMVar3 = McdNullCreate(fwk);
  pMVar1->nullGeom = pMVar3;
  return pMVar1;
}


/* ==== McdGMDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdGMDestroy(McdGeomMan *gm)

{
  MeHashDestroy(gm->name2geom);
  MeHashDestroy(gm->geom2name);
  (*_McdAggregateCreate)(gm);
  return;
}


/* ==== McdGMGetGeomCount ==== */

int McdGMGetGeomCount(McdGeomMan *gm)

{
  int iVar1;
  
  iVar1 = MeHashPopulation(gm->geom2name);
  return iVar1;
}


/* ==== McdGMGetFramework ==== */

McdFrameworkID McdGMGetFramework(McdGeomMan *gm)

{
  return gm->fwk;
}


/* ==== McdGMGetNullGeometry ==== */

McdNullID McdGMGetNullGeometry(McdGeomMan *gm)

{
  return gm->nullGeom;
}


