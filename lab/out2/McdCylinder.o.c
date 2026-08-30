/* ==== McdCylinderGetTypeId ==== */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

MeI16 McdCylinderGetTypeId(void)

{
  return 4;
}


/* ==== McdCylinderRegisterType ==== */

/* WARNING: Unknown calling convention */

void McdCylinderRegisterType(McdFramework *frame)

{
  McdFrameworkRegisterGeometryType
            (frame,4,"McdCylinder",McdCylinderDestroy,McdCylinderUpdateAABB,McdCylinderGetBSphere,
             McdCylinderMaximumPoint,McdCylinderGetMassProperties,McdCylinderDebugDraw);
  return;
}


/* ==== McdCylinderCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdCylinderID McdCylinderCreate(McdFramework *frame,MeReal inRadius,MeReal inHeight)

{
  McdGeometryID pMVar1;
  McdCylinderID pMVar2;
  undefined4 extraout_ECX;
  
                    /* Unresolved local var: McdCylinderID c@[DW_OP_reg3(EBX)] */
  pMVar2 = (McdCylinderID)(*_McdGeometryDeinit)(0x1c,0x10);
  if (pMVar2 != (McdCylinderID)0x0) {
    McdGeometryInit(pMVar2,frame,4,extraout_ECX);
                    /* Unresolved local var: McdCylinder * c@[???]
                       Unresolved local var: float __result@[???] */
    pMVar2[1].mRefCtAndID = (MeU32)inRadius;
    pMVar1 = (McdGeometryID)(inHeight * 0.5);
    pMVar2[1].prev = pMVar1;
    pMVar2[1].next = (McdGeometryID)SQRT((float)pMVar1 * (float)pMVar1 + inRadius * inRadius);
  }
  return pMVar2;
}


/* ==== McdCylinderGetRadius ==== */

MeReal McdCylinderGetRadius(McdCylinderID g)

{
  return (MeReal)g[1].mRefCtAndID;
}


/* ==== McdCylinderGetHeight ==== */

MeReal McdCylinderGetHeight(McdCylinderID g)

{
  return (float)g[1].prev + (float)g[1].prev;
}


/* ==== McdCylinderGetHalfHeight ==== */

MeReal McdCylinderGetHalfHeight(McdCylinderID g)

{
  return (MeReal)g[1].prev;
}


/* ==== McdCylinderSetRadius ==== */

void McdCylinderSetRadius(McdCylinderID g,MeReal r)

{
                    /* Unresolved local var: McdCylinder * c@[???]
                       Unresolved local var: float __result@[???] */
  g[1].mRefCtAndID = (MeU32)r;
  g[1].next = (McdGeometryID)SQRT(r * r + (float)g[1].prev * (float)g[1].prev);
  return;
}


/* ==== McdCylinderSetHeight ==== */

void McdCylinderSetHeight(McdCylinderID g,MeReal h)

{
  McdGeometryID pMVar1;
  
                    /* Unresolved local var: McdCylinder * c@[???]
                       Unresolved local var: float __result@[???] */
  pMVar1 = (McdGeometryID)(h * 0.5);
  g[1].prev = pMVar1;
  g[1].next = (McdGeometryID)
              SQRT((float)pMVar1 * (float)pMVar1 + (float)g[1].mRefCtAndID * (float)g[1].mRefCtAndID
                  );
  return;
}


/* ==== McdCylinderSetGeometricalParameters ==== */

void McdCylinderSetGeometricalParameters(McdGeometry *g,MeReal inRadius,MeReal inHeight)

{
  McdGeometryID pMVar1;
  
                    /* Unresolved local var: McdCylinder * c@[???] */
  pMVar1 = (McdGeometryID)(inHeight * 0.5);
  g[1].mRefCtAndID = (MeU32)inRadius;
  g[1].prev = pMVar1;
                    /* Unresolved local var: float __result@[???] */
  g[1].next = (McdGeometryID)SQRT((float)pMVar1 * (float)pMVar1 + inRadius * inRadius);
  return;
}


/* ==== McdCylinderDestroy ==== */

/* WARNING: Unknown calling convention */

void McdCylinderDestroy(McdGeometry *g)

{
  McdGeometryDeinit();
  return;
}


/* ==== McdCylinderUpdateAABB ==== */

void McdCylinderUpdateAABB(McdGeometryInstanceID ins,MeMatrix4Ptr finalTM,MeBool tight)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  int iVar9;
  MeReal s2z;
  MeReal sz_1;
  MeReal sz;
  MeReal max [3];
  MeReal min [3];
  MeReal rFit [3];
  
                    /* Unresolved local var: McdCylinder * c@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal Zx@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal Zy@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal Zz@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal s2x@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal s2y@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal sx@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal sy@[DW_OP_reg13(ST2)] */
  iVar8 = McdGeometryInstanceGetGeometry(ins);
  iVar9 = McdGeometryInstanceGetTransformPtr(ins);
  fVar1 = *(float *)(iVar9 + 0x20);
  fVar2 = *(float *)(iVar9 + 0x24);
  fVar3 = *(float *)(iVar9 + 0x28);
  fVar5 = 1.0 - fVar1 * fVar1;
  fVar6 = 1.0 - fVar2 * fVar2;
  fVar7 = 1.0 - fVar3 * fVar3;
  fVar4 = 0.0;
  if (0.0 < fVar5) {
    fVar4 = SQRT(fVar5);
  }
  fVar5 = 0.0;
  if (0.0 < fVar6) {
    fVar5 = SQRT(fVar6);
  }
  sz = 0.0;
  if (0.0 < fVar7) {
    sz = SQRT(fVar7);
  }
  fVar1 = *(float *)(iVar8 + 0x10) * fVar4 + ABS(fVar1) * *(float *)(iVar8 + 0x14);
  fVar2 = ABS(fVar2) * *(float *)(iVar8 + 0x14) + fVar5 * *(float *)(iVar8 + 0x10);
  fVar3 = ABS(fVar3) * *(float *)(iVar8 + 0x14) + sz * *(float *)(iVar8 + 0x10);
  ins->min[0] = *(float *)(iVar9 + 0x30) - fVar1;
  ins->min[1] = *(float *)(iVar9 + 0x34) - fVar2;
  ins->min[2] = *(float *)(iVar9 + 0x38) - fVar3;
  ins->max[0] = fVar1 + *(float *)(iVar9 + 0x30);
  ins->max[1] = fVar2 + *(float *)(iVar9 + 0x34);
  ins->max[2] = fVar3 + *(float *)(iVar9 + 0x38);
  if (finalTM != (MeMatrix4Ptr)0x0) {
                    /* Unresolved local var: MeReal Zx@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal Zy@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal Zz@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal s2x@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal s2y@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal s2z@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal sy@[DW_OP_reg12(ST1)] */
    fVar1 = finalTM[2][0];
    fVar2 = finalTM[2][1];
    fVar3 = finalTM[2][2];
    fVar5 = 1.0 - fVar1 * fVar1;
    s2z = 0.0;
    fVar6 = 1.0 - fVar2 * fVar2;
    fVar4 = 1.0 - fVar3 * fVar3;
    if (0.0 < fVar5) {
      s2z = SQRT(fVar5);
    }
    fVar5 = 0.0;
    if (0.0 < fVar6) {
      fVar5 = SQRT(fVar6);
    }
    sz_1 = 0.0;
    if (0.0 < fVar4) {
      sz_1 = SQRT(fVar4);
    }
    fVar6 = s2z * *(float *)(iVar8 + 0x10) + ABS(fVar1) * *(float *)(iVar8 + 0x14);
    fVar4 = *(float *)(iVar8 + 0x10) * fVar5 + ABS(fVar2) * *(float *)(iVar8 + 0x14);
    fVar1 = sz_1 * *(float *)(iVar8 + 0x10) + ABS(fVar3) * *(float *)(iVar8 + 0x14);
    fVar5 = finalTM[3][0] + fVar6;
    fVar6 = finalTM[3][0] - fVar6;
    fVar3 = finalTM[3][1] + fVar4;
    fVar4 = finalTM[3][1] - fVar4;
    fVar2 = finalTM[3][2] + fVar1;
    fVar1 = finalTM[3][2] - fVar1;
    if (ins->min[0] < fVar6) {
      fVar6 = ins->min[0];
    }
    ins->min[0] = fVar6;
    if (ins->min[1] < fVar4) {
      fVar4 = ins->min[1];
    }
    ins->min[1] = fVar4;
    if (ins->min[2] < fVar1) {
      fVar1 = ins->min[2];
    }
    ins->min[2] = fVar1;
    if (fVar5 < ins->max[0]) {
      fVar5 = ins->max[0];
    }
    ins->max[0] = fVar5;
    if (fVar3 < ins->max[1]) {
      fVar3 = ins->max[1];
    }
    ins->max[1] = fVar3;
    if (fVar2 < ins->max[2]) {
      fVar2 = ins->max[2];
    }
    ins->max[2] = fVar2;
  }
  return;
}


/* ==== McdCylinderGetXYAABB ==== */

void McdCylinderGetXYAABB(McdGeometry *g,MeVector4 *_tm,MeReal *bounds)

{
  float fVar1;
  float fVar2;
  float fVar3;
  McdGeometryID pMVar4;
  MeReal rFit [3];
  
                    /* Unresolved local var: McdCylinder * c@[???]
                       Unresolved local var: lsTransform * tm@[???]
                       Unresolved local var: MeReal Zx@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal Zy@[DW_OP_reg14(ST3)]
                       Unresolved local var: float __result@[???] */
  fVar1 = _tm[2][0];
  fVar2 = _tm[2][1];
  fVar3 = (float)g[1].mRefCtAndID;
  pMVar4 = g[1].prev;
  fVar1 = SQRT(1.0 - fVar1 * fVar1) * fVar3 + ABS(fVar1) * (float)pMVar4;
  *bounds = _tm[3][0] - fVar1;
  fVar2 = SQRT(1.0 - fVar2 * fVar2) * fVar3 + (float)pMVar4 * ABS(fVar2);
  bounds[1] = fVar1 + _tm[3][0];
  bounds[2] = _tm[3][1] - fVar2;
  bounds[3] = fVar2 + _tm[3][1];
  return;
}


/* ==== McdCylinderGetBSphere ==== */

void McdCylinderGetBSphere(McdGeometry *g,MeReal *center,MeReal *radius)

{
                    /* Unresolved local var: McdCylinder * c@[???] */
  *center = 0.0;
  center[2] = 0.0;
  center[1] = 0.0;
  *radius = (MeReal)g[1].next;
  return;
}


/* ==== McdCylinderGetBSphereRadius ==== */

MeReal McdCylinderGetBSphereRadius(McdGeometryID g)

{
  return (MeReal)g[1].next;
}


/* ==== McdCylinderMaximumPoint ==== */

void McdCylinderMaximumPoint(McdGeometryInstanceID ins,MeReal *inDir,MeReal *outPoint)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  int iVar9;
  int iVar10;
  McdCylinder *c;
  lsVec3 n;
  
                    /* Unresolved local var: lsTransform * tm@[DW_OP_reg2(EDX)]
                       Unresolved local var: lsVec3 * p@[???]
                       Unresolved local var: MeReal nZ@[DW_OP_reg15(ST4)] */
  iVar9 = McdGeometryInstanceGetGeometry(ins);
  iVar10 = McdGeometryInstanceGetTransformPtr(ins);
  n.v[0] = *inDir;
  n.v[1] = inDir[1];
  n.v[2] = inDir[2];
  fVar1 = *(float *)(iVar10 + 0x30);
  *outPoint = fVar1;
  fVar2 = *(float *)(iVar10 + 0x34);
  outPoint[1] = fVar2;
  fVar7 = *(float *)(iVar10 + 0x38);
  outPoint[2] = fVar7;
  fVar3 = *(float *)(iVar10 + 0x20);
  fVar8 = n.v[1] * *(float *)(iVar10 + 0x24) + fVar3 * n.v[0] + n.v[2] * *(float *)(iVar10 + 0x28);
  if (0.0001 < ABS(fVar8)) {
                    /* Unresolved local var: MeReal n2@[???] */
    if (fVar8 <= 0.0) {
      fVar1 = *(float *)(iVar9 + 0x14);
      fVar2 = *(float *)(iVar10 + 0x24);
      fVar7 = *(float *)(iVar10 + 0x28);
      *outPoint = *outPoint - fVar1 * fVar3;
      outPoint[1] = outPoint[1] - fVar1 * fVar2;
      outPoint[2] = outPoint[2] - fVar1 * fVar7;
    }
    else {
      fVar4 = *(float *)(iVar9 + 0x14);
      fVar5 = *(float *)(iVar10 + 0x24);
      fVar6 = *(float *)(iVar10 + 0x28);
      *outPoint = fVar1 + fVar4 * fVar3;
      outPoint[1] = fVar2 + fVar4 * fVar5;
      outPoint[2] = fVar7 + fVar4 * fVar6;
    }
    n.v[0] = n.v[0] - fVar8 * *(float *)(iVar10 + 0x20);
    n.v[1] = n.v[1] - fVar8 * *(float *)(iVar10 + 0x24);
    n.v[2] = n.v[2] - fVar8 * *(float *)(iVar10 + 0x28);
    fVar1 = n.v[1] * n.v[1] + n.v[0] * n.v[0] + n.v[2] * n.v[2];
    if (9.999999e-09 < fVar1) {
                    /* Unresolved local var: float __result@[???] */
      fVar1 = 1.0 / SQRT(fVar1);
      n.v[0] = n.v[0] * fVar1;
      n.v[1] = n.v[1] * fVar1;
      n.v[2] = fVar1 * n.v[2];
    }
  }
  fVar1 = *(float *)(iVar9 + 0x10);
  *outPoint = fVar1 * n.v[0] + *outPoint;
  outPoint[1] = fVar1 * n.v[1] + outPoint[1];
  outPoint[2] = fVar1 * n.v[2] + outPoint[2];
  return;
}


/* ==== McdCylinderMaximumPointLocal ==== */

void McdCylinderMaximumPointLocal(McdGeometry *g,MeReal *inDir,MeReal *outPoint)

{
  float fVar1;
  float fVar2;
  float fVar3;
  lsVec3 n;
  
                    /* Unresolved local var: McdCylinder * c@[???]
                       Unresolved local var: lsVec3 * p@[???] */
  fVar1 = *inDir;
  fVar2 = inDir[1];
  if (0.0001 < ABS(inDir[2])) {
                    /* Unresolved local var: MeReal n2@[DW_OP_reg14(ST3)] */
    if (inDir[2] <= 0.0001) {
      fVar3 = outPoint[2] - (float)g[1].prev;
    }
    else {
      fVar3 = (float)g[1].prev + outPoint[2];
    }
    outPoint[2] = fVar3;
    fVar3 = fVar2 * fVar2 + fVar1 * fVar1;
    if (9.999999e-09 < fVar3) {
                    /* Unresolved local var: MeReal invN@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[???] */
      fVar3 = 1.0 / SQRT(fVar3);
      fVar1 = fVar1 * fVar3;
      fVar2 = fVar3 * fVar2;
    }
  }
  *outPoint = fVar1 * (float)g[1].mRefCtAndID + *outPoint;
  outPoint[1] = fVar2 * (float)g[1].mRefCtAndID + outPoint[1];
  return;
}


/* ==== McdCylinderGetMassProperties ==== */

MeI16 McdCylinderGetMassProperties(McdGeometry *g,MeVector4 *relTM,MeVector3 *m,MeReal *volume)

{
  float fVar1;
  float fVar2;
  float fVar3;
  
                    /* Unresolved local var: McdCylinder * c@[???]
                       Unresolved local var: MeReal r@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal l@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal mass@[???] */
  fVar1 = (float)g[1].mRefCtAndID;
  fVar2 = (float)g[1].prev + (float)g[1].prev;
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???] */
  (*m)[1] = 0.0;
  (*m)[2] = 0.0;
  m[1][0] = 0.0;
  m[1][2] = 0.0;
  m[2][0] = 0.0;
  fVar3 = fVar2 * fVar2 * 0.083333336 + fVar1 * fVar1 * 0.25;
  m[2][1] = 0.0;
  (*m)[0] = fVar3;
  m[1][1] = fVar3;
  m[2][2] = fVar1 * fVar1 * 0.5;
  (*relTM)[0] = 1.0;
  (*relTM)[1] = 0.0;
  (*relTM)[2] = 0.0;
  (*relTM)[3] = 0.0;
  relTM[1][0] = 0.0;
  relTM[1][1] = 1.0;
  relTM[1][2] = 0.0;
  relTM[1][3] = 0.0;
  relTM[2][0] = 0.0;
  relTM[2][1] = 0.0;
  relTM[2][2] = 1.0;
  relTM[2][3] = 0.0;
  relTM[3][0] = 0.0;
  relTM[3][1] = 0.0;
  relTM[3][2] = 0.0;
  relTM[3][3] = 1.0;
  *volume = fVar1 * fVar1 * fVar2 * 3.1415927;
  return 0;
}


/* ==== McdCylinderDebugDraw ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdCylinderDebugDraw(McdGeometryID geom,MeReal (*tm) [4],MeReal *colour)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  int iVar9;
  MeReal lv [3];
  MeReal wv2 [3];
  MeReal wv1 [3];
  MeReal rads [3];
  
                    /* Unresolved local var: McdCylinderID cyl@[???]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  if ((char)geom->mRefCtAndID == '\x04') {
    fVar1 = (float)geom[1].mRefCtAndID;
    iVar9 = 0x13;
    fVar2 = ((float)geom[1].prev + (float)geom[1].prev) * 0.5;
    iVar8 = 0;
    do {
      fVar3 = fVar1 * *(float *)((int)McdCylinderDebugDraw::cylDraw[0][0] + iVar8 + 4);
      fVar4 = fVar1 * *(float *)((int)McdCylinderDebugDraw::cylDraw[0][0] + iVar8);
      fVar6 = fVar2 * *(float *)((int)McdCylinderDebugDraw::cylDraw[0][0] + iVar8 + 8);
      wv1[0] = tm[2][0] * fVar6 + fVar4 * (*tm)[0] + fVar3 * tm[1][0] + tm[3][0];
      wv1[1] = fVar6 * tm[2][1] + fVar3 * tm[1][1] + fVar4 * (*tm)[1] + tm[3][1];
      fVar7 = fVar1 * *(float *)((int)McdCylinderDebugDraw::cylDraw[0][1] + iVar8 + 4);
      fVar5 = fVar1 * *(float *)((int)McdCylinderDebugDraw::cylDraw[0][1] + iVar8);
      wv1[2] = fVar4 * (*tm)[2] + fVar3 * tm[1][2] + fVar6 * tm[2][2] + tm[3][2];
      fVar3 = fVar2 * *(float *)((int)McdCylinderDebugDraw::cylDraw[0][1] + iVar8 + 8);
      iVar8 = iVar8 + 0x18;
      wv2[0] = (*tm)[0] * fVar5 + tm[1][0] * fVar7 + tm[2][0] * fVar3 + tm[3][0];
      wv2[1] = (*tm)[1] * fVar5 + tm[1][1] * fVar7 + tm[2][1] * fVar3 + tm[3][1];
      wv2[2] = fVar5 * (*tm)[2] + fVar7 * tm[1][2] + fVar3 * tm[2][2] + tm[3][2];
      (*_MeDebugDrawAPI)(wv1,wv2,colour[1],colour[2],colour[3]);
      iVar9 = iVar9 + -1;
    } while (-1 < iVar9);
  }
  return;
}


