/* ==== McdConvexMeshGetTypeId ==== */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

MeI16 McdConvexMeshGetTypeId(void)

{
  return 7;
}


/* ==== McdConvexMeshRegisterType ==== */

/* WARNING: Unknown calling convention */

void McdConvexMeshRegisterType(McdFramework *frame)

{
  McdFrameworkRegisterGeometryType
            (frame,7,"McdConvexMesh",McdConvexMeshDestroy,McdConvexMeshUpdateAABB,
             McdConvexMeshGetBSphere,McdConvexMeshMaximumPoint,McdConvexMeshGetMassProperties,
             McdConvexMeshDebugDraw);
  return;
}


/* ==== MeBoundingSphereCalc2 ==== */

void MeBoundingSphereCalc2(McdCnvVertex *points,int numPoints,MeReal *centre,MeReal *radius)

{
  int iVar1;
  McdCnvVertex *pMVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  McdCnvVertex *pMVar8;
  int iVar9;
  float __result;
  MeReal r2;
  MeReal diff [3];
  int maxIx [3];
  int minIx [3];
  MeReal max [3];
  MeReal min [3];
  
                    /* Unresolved local var: MeReal r@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal d2@[DW_OP_reg18(ST7)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[DW_OP_reg1(ECX)] */
  if (numPoints == 0) {
    return;
  }
  max[0] = points->position[0];
  maxIx[0] = 0;
  minIx[0] = 0;
  max[1] = points->position[1];
  iVar9 = 1;
  maxIx[1] = 0;
  minIx[1] = 0;
  max[2] = points->position[2];
  maxIx[2] = 0;
  minIx[2] = 0;
  min[0] = max[0];
  min[1] = max[1];
  min[2] = max[2];
  if (1 < numPoints) {
    do {
      pMVar8 = points + iVar9;
      if (min[0] <= pMVar8->position[0]) {
        if (max[0] < pMVar8->position[0]) {
          maxIx[0] = iVar9;
          max[0] = pMVar8->position[0];
        }
      }
      else {
        minIx[0] = iVar9;
        min[0] = pMVar8->position[0];
      }
      if (min[1] <= pMVar8->position[1]) {
        if (max[1] < pMVar8->position[1]) {
          maxIx[1] = iVar9;
          max[1] = pMVar8->position[1];
        }
      }
      else {
        minIx[1] = iVar9;
        min[1] = pMVar8->position[1];
      }
      if (min[2] <= pMVar8->position[2]) {
        if (max[2] < pMVar8->position[2]) {
          maxIx[2] = iVar9;
          max[2] = pMVar8->position[2];
        }
      }
      else {
        minIx[2] = iVar9;
        min[2] = pMVar8->position[2];
      }
      iVar9 = iVar9 + 1;
    } while (iVar9 < numPoints);
  }
                    /* Unresolved local var: MeReal tmpd2@[???] */
  pMVar8 = points + maxIx[0];
  pMVar2 = points + minIx[0];
  fVar3 = pMVar8->position[0] - pMVar2->position[0];
                    /* Unresolved local var: MeReal m@[???] */
  fVar6 = pMVar8->position[1] - pMVar2->position[1];
  fVar5 = pMVar8->position[2] - pMVar2->position[2];
  fVar4 = fVar5 * fVar5 + fVar3 * fVar3 + fVar6 * fVar6;
  if (fVar4 <= 0.0) {
    fVar4 = 0.0;
  }
  else {
    *centre = pMVar2->position[0] + fVar3 * 0.5;
    centre[1] = fVar6 * 0.5 + pMVar2->position[1];
    centre[2] = fVar5 * 0.5 + pMVar2->position[2];
  }
  iVar9 = 1;
  do {
    pMVar8 = points + maxIx[iVar9];
    pMVar2 = points + minIx[iVar9];
    fVar6 = pMVar8->position[0] - pMVar2->position[0];
    fVar5 = pMVar8->position[1] - pMVar2->position[1];
    fVar7 = pMVar8->position[2] - pMVar2->position[2];
    fVar3 = fVar7 * fVar7 + fVar6 * fVar6 + fVar5 * fVar5;
    if (fVar4 < fVar3) {
      pMVar8 = points + minIx[iVar9];
      *centre = fVar6 * 0.5 + pMVar8->position[0];
      centre[1] = fVar5 * 0.5 + pMVar8->position[1];
      centre[2] = fVar7 * 0.5 + pMVar8->position[2];
      fVar4 = fVar3;
    }
    iVar1 = iVar9 + 1;
    pMVar8 = points + maxIx[iVar1];
    pMVar2 = points + minIx[iVar1];
    fVar6 = pMVar8->position[0] - pMVar2->position[0];
    fVar5 = pMVar8->position[1] - pMVar2->position[1];
    fVar7 = pMVar8->position[2] - pMVar2->position[2];
    fVar3 = fVar7 * fVar7 + fVar6 * fVar6 + fVar5 * fVar5;
    if (fVar4 < fVar3) {
      pMVar8 = points + minIx[iVar1];
      *centre = fVar6 * 0.5 + pMVar8->position[0];
      centre[1] = fVar5 * 0.5 + pMVar8->position[1];
      centre[2] = fVar7 * 0.5 + pMVar8->position[2];
      fVar4 = fVar3;
    }
    iVar9 = iVar9 + 2;
  } while (iVar9 < 3);
                    /* Unresolved local var: float __result@[???] */
  fVar3 = SQRT(fVar4) * 0.5;
  r2 = fVar3 * fVar3;
  if (0 < numPoints) {
    if ((numPoints & 1U) != 0) {
                    /* Unresolved local var: MeReal pr2@[???] */
      fVar6 = points->position[0] - *centre;
      fVar4 = points->position[1] - centre[1];
                    /* Unresolved local var: MeReal m@[DW_OP_reg11(ST0)] */
      fVar7 = points->position[2] - centre[2];
      fVar5 = fVar7 * fVar7 + fVar6 * fVar6 + fVar4 * fVar4;
      if (r2 < fVar5) {
                    /* Unresolved local var: MeReal pr@[???] */
        fVar5 = SQRT(fVar5);
        fVar3 = (fVar3 + fVar5) * 0.5;
        r2 = fVar3 * fVar3;
        fVar5 = (1.0 / fVar5) * (fVar5 - fVar3);
        *centre = fVar5 * fVar6 + *centre;
        centre[1] = centre[1] + fVar5 * fVar4;
        centre[2] = fVar5 * fVar7 + centre[2];
      }
      points = points + 1;
      numPoints = numPoints + -1;
      if (numPoints == 0) goto LAB_000104ee;
    }
    do {
      fVar6 = points->position[0] - *centre;
      fVar5 = points->position[1] - centre[1];
      fVar7 = points->position[2] - centre[2];
      fVar4 = fVar7 * fVar7 + fVar6 * fVar6 + fVar5 * fVar5;
      if (r2 < fVar4) {
        fVar4 = SQRT(fVar4);
        fVar3 = (fVar3 + fVar4) * 0.5;
        r2 = fVar3 * fVar3;
        fVar4 = (1.0 / fVar4) * (fVar4 - fVar3);
        *centre = *centre + fVar6 * fVar4;
        centre[1] = centre[1] + fVar5 * fVar4;
        centre[2] = fVar4 * fVar7 + centre[2];
      }
      fVar6 = points[1].position[0] - *centre;
      fVar4 = points[1].position[1] - centre[1];
      fVar7 = points[1].position[2] - centre[2];
      fVar5 = fVar7 * fVar7 + fVar6 * fVar6 + fVar4 * fVar4;
      if (r2 < fVar5) {
        fVar5 = SQRT(fVar5);
        fVar3 = (fVar3 + fVar5) * 0.5;
        r2 = fVar3 * fVar3;
        fVar5 = (1.0 / fVar5) * (fVar5 - fVar3);
        *centre = fVar5 * fVar6 + *centre;
        centre[1] = centre[1] + fVar5 * fVar4;
        centre[2] = fVar5 * fVar7 + centre[2];
      }
      points = points + 2;
      numPoints = numPoints + -2;
    } while (numPoints != 0);
  }
LAB_000104ee:
  *radius = fVar3;
  return;
}


/* ==== McdConvexMeshCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdGeometry * __regparm1 McdConvexMeshCreate(McdFramework *frame,McdConvexHull *poly,MeReal fatness)

{
  McdGeometry *pMVar1;
  McdFrameworkID in_stack_0000000c;
  
                    /* Unresolved local var: McdConvexMesh * c@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdGeometry * g@[???] */
  pMVar1 = (McdGeometry *)(*_McdComputeHull)(0x40,0x10,frame,frame);
  McdGeometryInit(pMVar1,poly,7);
  pMVar1[1].mRefCtAndID = *(MeU32 *)fatness;
  pMVar1[1].prev = *(McdGeometryID *)((int)fatness + 4);
  pMVar1[1].next = *(McdGeometryID *)((int)fatness + 8);
  pMVar1[1].frame = *(McdFrameworkID *)((int)fatness + 0xc);
  pMVar1[2].mRefCtAndID = *(MeU32 *)((int)fatness + 0x10);
  pMVar1[2].prev = *(McdGeometryID *)((int)fatness + 0x14);
  pMVar1[2].next = *(McdGeometryID *)((int)fatness + 0x18);
  pMVar1[2].frame = in_stack_0000000c;
  MeBoundingSphereCalc2
            ((McdCnvVertex *)pMVar1[1].mRefCtAndID,pMVar1[2].mRefCtAndID,(MeReal *)&pMVar1[3].prev,
             (MeReal *)(pMVar1 + 3));
  pMVar1[3].mRefCtAndID = (MeU32)((float)in_stack_0000000c + (float)pMVar1[3].mRefCtAndID);
  return pMVar1;
}


/* ==== McdConvexMeshCreateHull ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

McdConvexMeshID
McdConvexMeshCreateHull
          (McdFramework *frame,MeReal (*vertices) [3],int vertexCount,MeReal fatnessRadius)

{
  int iVar1;
  McdConvexMeshID pMVar2;
  McdConvexHull poly;
  
                    /* Unresolved local var: int ok@[DW_OP_reg0(EAX)] */
  iVar1 = McdComputeHull(&poly,vertexCount,vertices);
  pMVar2 = (McdConvexMeshID)0x0;
  if (iVar1 != 0) {
                    /* Unresolved local var: McdConvexMesh * c@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdGeometry * g@[???] */
    pMVar2 = (McdConvexMeshID)(*_McdComputeHull)(0x40,0x10,0,0);
    McdGeometryInit(pMVar2,frame,7);
    pMVar2[1].mRefCtAndID = (MeU32)poly.vertex;
    pMVar2[1].prev = (McdGeometryID)poly.face;
    pMVar2[1].next = (McdGeometryID)poly.edge;
    pMVar2[1].frame = (McdFrameworkID)poly.edgeIndex;
    pMVar2[2].mRefCtAndID = poly.numVertex;
    pMVar2[2].prev = (McdGeometryID)poly.numFace;
    pMVar2[2].next = (McdGeometryID)poly.numEdge;
    pMVar2[2].frame = (McdFrameworkID)fatnessRadius;
    MeBoundingSphereCalc2
              ((McdCnvVertex *)pMVar2[1].mRefCtAndID,pMVar2[2].mRefCtAndID,(MeReal *)&pMVar2[3].prev
               ,(MeReal *)(pMVar2 + 3));
    pMVar2[3].mRefCtAndID = (MeU32)(fatnessRadius + (float)pMVar2[3].mRefCtAndID);
  }
  return pMVar2;
}


/* ==== McdConvexMeshSetPolyhedron ==== */

/* WARNING: Unknown calling convention */

void McdConvexMeshSetPolyhedron(McdConvexMeshID g,McdConvexHull *poly,MeReal fatness)

{
  MeWarning(0,"McdConvexMeshSetPolyhedron is deprecated and does nothing.");
  return;
}


/* ==== McdConvexMeshDestroy ==== */

void McdConvexMeshDestroy(McdConvexMeshID g)

{
                    /* Unresolved local var: McdConvexMesh * c@[???] */
  McdDeallocateHull(g + 1);
  McdGeometryDeinit(g);
  return;
}


/* ==== McdConvexMeshUpdateAABB ==== */

/* WARNING: Unknown calling convention */

void McdConvexMeshUpdateAABB(McdGeometryInstanceID ins,MeMatrix4Ptr finalTM,MeBool tight)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  void *pvVar6;
  MeReal *pMVar7;
  MeReal *pMVar8;
  MeReal *pMVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  float fVar13;
  McdConvexMesh *c;
  int h_1;
  int h;
  MeReal localDir_1 [3];
  MeReal max [3];
  MeReal localDir [3];
  
                    /* Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg6(ESI)]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  pvVar6 = McdGeometryInstanceGetGeometry(ins);
  pMVar7 = McdGeometryInstanceGetTransformPtr(ins);
  pMVar9 = ins->min;
  pMVar8 = ins->max;
  localDir[0] = *pMVar7;
  localDir[1] = pMVar7[4];
  localDir[2] = pMVar7[8];
  fVar13 = McdConvexMeshMaximumPointLocal(pvVar6,localDir,0,3.4028235e+38,&h);
  ins->max[0] = fVar13;
  localDir[0] = localDir[0] * -1.0;
  localDir[1] = localDir[1] * -1.0;
  localDir[2] = localDir[2] * -1.0;
  fVar13 = McdConvexMeshMaximumPointLocal(pvVar6,localDir,0,3.4028235e+38,&h);
  ins->min[0] = fVar13;
  iVar10 = 1;
  do {
    localDir[0] = pMVar7[iVar10];
    localDir[1] = pMVar7[iVar10 + 4];
    localDir[2] = pMVar7[iVar10 + 8];
    fVar13 = McdConvexMeshMaximumPointLocal(pvVar6,localDir,0,3.4028235e+38,&h);
    pMVar8[iVar10] = fVar13;
    localDir[0] = localDir[0] * -1.0;
    localDir[1] = localDir[1] * -1.0;
    localDir[2] = localDir[2] * -1.0;
    fVar13 = McdConvexMeshMaximumPointLocal(pvVar6,localDir,0,3.4028235e+38,&h);
    pMVar9[iVar10] = fVar13;
    iVar11 = iVar10 + 1;
    iVar12 = iVar10 + 2;
    localDir[0] = pMVar7[iVar11];
    localDir[1] = pMVar7[iVar10 + 5];
    localDir[2] = pMVar7[iVar10 + 9];
    fVar13 = McdConvexMeshMaximumPointLocal(pvVar6,localDir,0,3.4028235e+38,&h);
    pMVar8[iVar11] = fVar13;
    localDir[0] = localDir[0] * -1.0;
    localDir[1] = localDir[1] * -1.0;
    localDir[2] = localDir[2] * -1.0;
    fVar13 = McdConvexMeshMaximumPointLocal(pvVar6,localDir,0,3.4028235e+38,&h);
    pMVar9[iVar11] = fVar13;
    iVar10 = iVar12;
  } while (iVar12 < 3);
  *pMVar9 = (pMVar7[0xc] - *pMVar9) - *(float *)((int)pvVar6 + 0x2c);
  ins->min[1] = (pMVar7[0xd] - ins->min[1]) - *(float *)((int)pvVar6 + 0x2c);
  ins->min[2] = (pMVar7[0xe] - ins->min[2]) - *(float *)((int)pvVar6 + 0x2c);
  *pMVar8 = *(float *)((int)pvVar6 + 0x2c) + pMVar7[0xc] + *pMVar8;
  ins->max[1] = *(float *)((int)pvVar6 + 0x2c) + pMVar7[0xd] + ins->max[1];
  ins->max[2] = *(float *)((int)pvVar6 + 0x2c) + pMVar7[0xe] + ins->max[2];
  if (finalTM != (MeMatrix4Ptr)0x0) {
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)] */
    localDir_1[2] = finalTM[2][0];
    localDir_1[0] = (*finalTM)[0];
    localDir_1[1] = finalTM[1][0];
    max[0] = McdConvexMeshMaximumPointLocal(pvVar6,localDir_1,0,3.4028235e+38,&h_1);
    localDir_1[0] = localDir_1[0] * -1.0;
    localDir_1[1] = localDir_1[1] * -1.0;
    localDir_1[2] = localDir_1[2] * -1.0;
    localDir[0] = McdConvexMeshMaximumPointLocal(pvVar6,localDir_1,0,3.4028235e+38,&h_1);
    iVar10 = 1;
    do {
      localDir_1[0] = (*finalTM)[iVar10];
      localDir_1[1] = finalTM[1][iVar10];
      localDir_1[2] = finalTM[2][iVar10];
      fVar13 = McdConvexMeshMaximumPointLocal(pvVar6,localDir_1,0,3.4028235e+38,&h_1);
      max[iVar10] = fVar13;
      localDir_1[0] = localDir_1[0] * -1.0;
      localDir_1[1] = localDir_1[1] * -1.0;
      localDir_1[2] = localDir_1[2] * -1.0;
      fVar13 = McdConvexMeshMaximumPointLocal(pvVar6,localDir_1,0,3.4028235e+38,&h_1);
      localDir[iVar10] = fVar13;
      iVar11 = iVar10 + 2;
      localDir_1[1] = finalTM[1][iVar10 + 1];
      localDir_1[0] = (*finalTM)[iVar10 + 1];
      localDir_1[2] = finalTM[2][iVar10 + 1];
      fVar13 = McdConvexMeshMaximumPointLocal(pvVar6,localDir_1,0,3.4028235e+38,&h_1);
      max[iVar10 + 1] = fVar13;
      localDir_1[0] = localDir_1[0] * -1.0;
      localDir_1[1] = localDir_1[1] * -1.0;
      localDir_1[2] = localDir_1[2] * -1.0;
      fVar13 = McdConvexMeshMaximumPointLocal(pvVar6,localDir_1,0,3.4028235e+38,&h_1);
      localDir[iVar10 + 1] = fVar13;
      iVar10 = iVar11;
    } while (iVar11 < 3);
    fVar1 = (finalTM[3][0] - localDir[0]) - *(float *)((int)pvVar6 + 0x2c);
    fVar3 = (finalTM[3][1] - localDir[1]) - *(float *)((int)pvVar6 + 0x2c);
    fVar5 = (finalTM[3][2] - localDir[2]) - *(float *)((int)pvVar6 + 0x2c);
    fVar13 = finalTM[3][0] + *(float *)((int)pvVar6 + 0x2c) + max[0];
    fVar2 = finalTM[3][1] + *(float *)((int)pvVar6 + 0x2c) + max[1];
    fVar4 = finalTM[3][2] + *(float *)((int)pvVar6 + 0x2c) + max[2];
    if (ins->min[0] < fVar1) {
      fVar1 = ins->min[0];
    }
    *pMVar9 = fVar1;
    if (ins->min[1] < fVar3) {
      fVar3 = ins->min[1];
    }
    ins->min[1] = fVar3;
    if (ins->min[2] < fVar5) {
      fVar5 = ins->min[2];
    }
    ins->min[2] = fVar5;
    if (fVar13 < ins->max[0]) {
      fVar13 = ins->max[0];
    }
    *pMVar8 = fVar13;
    if (fVar2 < ins->max[1]) {
      fVar2 = ins->max[1];
    }
    ins->max[1] = fVar2;
    if (fVar4 < ins->max[2]) {
      fVar4 = ins->max[2];
    }
    ins->max[2] = fVar4;
  }
  return;
}


/* ==== McdConvexMeshGetBSphere ==== */

void McdConvexMeshGetBSphere(McdGeometryID g,MeReal *center,MeReal *radius)

{
                    /* Unresolved local var: McdConvexMesh * c@[???] */
  *center = (MeReal)g[3].prev;
  center[1] = (MeReal)g[3].next;
  center[2] = (MeReal)g[3].frame;
  *radius = (MeReal)g[3].mRefCtAndID;
  return;
}


/* ==== McdConvexMeshMaximumPoint ==== */

/* WARNING: Unknown calling convention */

void McdConvexMeshMaximumPoint(McdGeometryInstanceID ins,MeReal *inDir,MeReal *outPoint)

{
  McdConvexMeshMaximumPointNew(ins,inDir,outPoint);
  return;
}


/* ==== McdConvexMeshGetXYAABB ==== */

/* WARNING: Unknown calling convention */

void McdConvexMeshGetXYAABB(McdGeometryID g,MeVector4 *_tm,MeReal *bounds)

{
  return;
}


/* ==== McdConvexMeshMeshGetTypeId ==== */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int McdConvexMeshMeshGetTypeId(void)

{
  return 7;
}


/* ==== McdConvexMeshGetPolygonVertexCount ==== */

/* WARNING: Unknown calling convention */

int McdConvexMeshGetPolygonVertexCount(McdConvexMeshID g,int polyID)

{
  int iVar1;
  
                    /* Unresolved local var: McdConvexMesh * c@[???] */
  iVar1 = McdCnvFaceGetCount(g + 1,polyID);
  return iVar1;
}


/* ==== McdConvexMeshGetPolyhedron ==== */

/* WARNING: Unknown calling convention */

McdConvexHull * McdConvexMeshGetPolyhedron(McdConvexMeshID g)

{
  return (McdConvexHull *)(g + 1);
}


/* ==== McdConvexMeshGetPolygonVertex ==== */

void McdConvexMeshGetPolygonVertex(McdConvexMeshID m,int polyID,int vertexID,MeReal *coords)

{
  MeReal *pMVar1;
  
                    /* Unresolved local var: McdConvexMesh * c@[???] */
  pMVar1 = McdCnvFaceGetVertexPosition(m + 1,polyID,vertexID);
  *coords = *pMVar1;
  coords[1] = pMVar1[1];
  coords[2] = pMVar1[2];
  return;
}


/* ==== McdConvexMeshGetPolygonVertexPtr ==== */

MeReal * McdConvexMeshGetPolygonVertexPtr(McdConvexMeshID m,int polyID,int vertexID)

{
  MeReal *pMVar1;
  
                    /* Unresolved local var: McdConvexMesh * c@[???] */
  pMVar1 = McdCnvFaceGetVertexPosition(m + 1,polyID,vertexID);
  return pMVar1;
}


/* ==== McdConvexMeshGetFatness ==== */

MeReal McdConvexMeshGetFatness(McdConvexMeshID g)

{
                    /* Unresolved local var: McdConvexMesh * c@[???] */
  return (MeReal)g[2].frame;
}


/* ==== McdConvexMeshGetPolygonNormal ==== */

void McdConvexMeshGetPolygonNormal(McdConvexMeshID m,int polyID,MeReal *coords)

{
  McdGeometryID pMVar1;
  
                    /* Unresolved local var: McdConvexMesh * c@[???] */
  pMVar1 = m[1].prev + polyID;
  *coords = (MeReal)pMVar1->mRefCtAndID;
  coords[1] = (MeReal)pMVar1->prev;
  coords[2] = (MeReal)pMVar1->next;
  return;
}


/* ==== McdConvexMeshGetPolygonCount ==== */

int McdConvexMeshGetPolygonCount(McdConvexMeshID m)

{
                    /* Unresolved local var: McdConvexMesh * c@[???] */
  return (int)m[2].prev;
}


/* ==== McdConvexMeshDebugDraw ==== */

/* WARNING: Unknown calling convention */

void McdConvexMeshDebugDraw(McdGeometryID g,MeReal (*tm) [4],MeReal *colour)

{
  return;
}


