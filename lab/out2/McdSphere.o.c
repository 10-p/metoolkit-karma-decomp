/* ==== McdSphereGetTypeId ==== */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

MeI16 McdSphereGetTypeId(void)

{
  return 1;
}


/* ==== McdSphereRegisterType ==== */

/* WARNING: Unknown calling convention */

void McdSphereRegisterType(McdFramework *frame)

{
  McdFrameworkRegisterGeometryType
            (frame,1,"McdSphere",McdSphereDestroy,McdSphereUpdateAABB,McdSphereGetBSphere,
             McdSphereMaximumPoint,McdSphereGetMassProperties,McdSphereDebugDraw);
  return;
}


/* ==== McdSphereCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdSphereID McdSphereCreate(McdFramework *frame,MeReal inRadius)

{
  McdSphereID pMVar1;
  undefined4 extraout_ECX;
  
                    /* Unresolved local var: McdSphereID s@[DW_OP_reg3(EBX)] */
  pMVar1 = (McdSphereID)(*_McdGeometryDeinit)(0x14,0x10);
  if (pMVar1 != (McdSphereID)0x0) {
    McdGeometryInit(pMVar1,frame,1,extraout_ECX);
                    /* Unresolved local var: McdSphere * s@[???] */
    pMVar1[1].mRefCtAndID = (MeU32)inRadius;
  }
  return pMVar1;
}


/* ==== McdSphereSetRadius ==== */

void McdSphereSetRadius(McdSphereID g,MeReal inRadius)

{
                    /* Unresolved local var: McdSphere * s@[???] */
  g[1].mRefCtAndID = (MeU32)inRadius;
  return;
}


/* ==== McdSphereGetRadius ==== */

MeReal McdSphereGetRadius(McdSphereID g)

{
  return (MeReal)g[1].mRefCtAndID;
}


/* ==== McdSphereDestroy ==== */

/* WARNING: Unknown calling convention */

void McdSphereDestroy(McdSphereID g)

{
  McdGeometryDeinit();
  return;
}


/* ==== McdSphereUpdateAABB ==== */

void McdSphereUpdateAABB(McdGeometryInstanceID ins,MeMatrix4Ptr finalTM,MeBool tight)

{
  float fVar1;
  float fVar2;
  int iVar3;
  int iVar4;
  MeReal r [3];
  
                    /* Unresolved local var: McdSphere * s@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg0(EAX)] */
  iVar3 = McdGeometryInstanceGetGeometry(ins);
  iVar4 = McdGeometryInstanceGetTransformPtr(ins);
  fVar2 = *(float *)(iVar3 + 0x10);
  ins->min[0] = *(MeReal *)(iVar4 + 0x30);
  ins->min[1] = *(MeReal *)(iVar4 + 0x34);
  ins->min[2] = *(MeReal *)(iVar4 + 0x38);
  ins->max[0] = *(MeReal *)(iVar4 + 0x30);
  ins->max[1] = *(MeReal *)(iVar4 + 0x34);
  ins->max[2] = *(MeReal *)(iVar4 + 0x38);
  if (finalTM != (MeMatrix4Ptr)0x0) {
    fVar1 = finalTM[3][0];
    if (ins->min[0] < fVar1) {
      fVar1 = ins->min[0];
    }
    ins->min[0] = fVar1;
    fVar1 = finalTM[3][1];
    if (ins->min[1] < fVar1) {
      fVar1 = ins->min[1];
    }
    ins->min[1] = fVar1;
    fVar1 = finalTM[3][2];
    if (ins->min[2] < fVar1) {
      fVar1 = ins->min[2];
    }
    ins->min[2] = fVar1;
    fVar1 = finalTM[3][0];
    if (fVar1 < ins->max[0]) {
      fVar1 = ins->max[0];
    }
    ins->max[0] = fVar1;
    fVar1 = finalTM[3][1];
    if (fVar1 < ins->max[1]) {
      fVar1 = ins->max[1];
    }
    ins->max[1] = fVar1;
    fVar1 = finalTM[3][2];
    if (fVar1 < ins->max[2]) {
      fVar1 = ins->max[2];
    }
    ins->max[2] = fVar1;
  }
  ins->min[0] = ins->min[0] - fVar2;
  ins->min[1] = ins->min[1] - fVar2;
  ins->min[2] = ins->min[2] - fVar2;
  ins->max[0] = fVar2 + ins->max[0];
  ins->max[1] = fVar2 + ins->max[1];
  ins->max[2] = fVar2 + ins->max[2];
  return;
}


/* ==== McdSphereGetBSphere ==== */

void McdSphereGetBSphere(McdSphereID g,MeReal *center,MeReal *radius)

{
                    /* Unresolved local var: McdSphere * s@[???] */
  *center = 0.0;
  center[2] = 0.0;
  center[1] = 0.0;
  *radius = (MeReal)g[1].mRefCtAndID;
  return;
}


/* ==== McdSphereMaximumPoint ==== */

void McdSphereMaximumPoint(McdGeometryInstanceID ins,MeReal *inDir,MeReal *outPoint)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  int iVar6;
  int iVar7;
  
                    /* Unresolved local var: McdSphere * s@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * tm@[DW_OP_reg0(EAX)]
                       Unresolved local var: lsVec3 * n@[???]
                       Unresolved local var: lsVec3 * p@[???] */
  iVar6 = McdGeometryInstanceGetGeometry(ins);
  iVar7 = McdGeometryInstanceGetTransformPtr(ins);
  fVar1 = *(float *)(iVar6 + 0x10);
  fVar2 = inDir[1];
  fVar3 = inDir[2];
  fVar4 = *(float *)(iVar7 + 0x34);
  fVar5 = *(float *)(iVar7 + 0x38);
  *outPoint = fVar1 * *inDir + *(float *)(iVar7 + 0x30);
  outPoint[1] = fVar1 * fVar2 + fVar4;
  outPoint[2] = fVar1 * fVar3 + fVar5;
  return;
}


/* ==== McdSphereGetMassProperties ==== */

MeI16 McdSphereGetMassProperties(McdSphereID g,MeVector4 *relTM,MeVector3 *m,MeReal *volume)

{
  float fVar1;
  
                    /* Unresolved local var: McdSphere * s@[???]
                       Unresolved local var: MeReal Sph_coef@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal mass@[???] */
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???] */
  (*m)[0] = 0.0;
  m[1][1] = 0.0;
  m[2][2] = 0.0;
  (*m)[1] = 0.0;
  (*m)[2] = 0.0;
  m[1][0] = 0.0;
  m[1][2] = 0.0;
  m[2][0] = 0.0;
  m[2][1] = 0.0;
  (*m)[0] = (float)g[1].mRefCtAndID * 0.4 * (float)g[1].mRefCtAndID;
  m[1][1] = (float)g[1].mRefCtAndID * 0.4 * (float)g[1].mRefCtAndID;
  m[2][2] = (float)g[1].mRefCtAndID * 0.4 * (float)g[1].mRefCtAndID;
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
  fVar1 = (float)g[1].mRefCtAndID;
  *volume = fVar1 * fVar1 * fVar1 * 4.1887903;
  return 0;
}


/* ==== McdSphereDebugDraw ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdSphereDebugDraw(McdGeometryID geom,MeReal (*tm) [4],MeReal *colour)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  int iVar7;
  int iVar8;
  MeReal radius;
  MeReal wv2 [3];
  MeReal wv1 [3];
  MeReal lv [3];
  
                    /* Unresolved local var: McdSphereID sphere@[???]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  if ((char)geom->mRefCtAndID == '\x01') {
    fVar1 = (float)geom[1].mRefCtAndID;
    iVar8 = 0x17;
    iVar7 = 0;
    do {
      fVar2 = fVar1 * *(float *)((int)McdSphereDebugDraw::sphereDraw[0][0] + iVar7 + 4);
      fVar3 = fVar1 * *(float *)((int)McdSphereDebugDraw::sphereDraw[0][0] + iVar7);
      fVar5 = fVar1 * *(float *)((int)McdSphereDebugDraw::sphereDraw[0][0] + iVar7 + 8);
      wv1[0] = tm[2][0] * fVar5 + fVar3 * (*tm)[0] + fVar2 * tm[1][0] + tm[3][0];
      wv1[1] = fVar5 * tm[2][1] + fVar2 * tm[1][1] + fVar3 * (*tm)[1] + tm[3][1];
      fVar6 = fVar1 * *(float *)((int)McdSphereDebugDraw::sphereDraw[0][1] + iVar7 + 4);
      fVar4 = fVar1 * *(float *)((int)McdSphereDebugDraw::sphereDraw[0][1] + iVar7);
      wv1[2] = fVar3 * (*tm)[2] + fVar2 * tm[1][2] + fVar5 * tm[2][2] + tm[3][2];
      fVar2 = fVar1 * *(float *)((int)McdSphereDebugDraw::sphereDraw[0][1] + iVar7 + 8);
      iVar7 = iVar7 + 0x18;
      wv2[0] = (*tm)[0] * fVar4 + tm[1][0] * fVar6 + tm[2][0] * fVar2 + tm[3][0];
      wv2[1] = (*tm)[1] * fVar4 + tm[1][1] * fVar6 + tm[2][1] * fVar2 + tm[3][1];
      wv2[2] = fVar2 * tm[2][2] + fVar4 * (*tm)[2] + fVar6 * tm[1][2] + tm[3][2];
      (*_MeDebugDrawAPI)(wv1,wv2,colour[1],colour[2],colour[3]);
      iVar8 = iVar8 + -1;
    } while (-1 < iVar8);
  }
  return;
}


