/* ==== McdBoxGetTypeId ==== */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

MeI16 McdBoxGetTypeId(void)

{
  return 2;
}


/* ==== McdBoxRegisterType ==== */

/* WARNING: Unknown calling convention */

void McdBoxRegisterType(McdFramework *frame)

{
  McdFrameworkRegisterGeometryType
            (frame,2,"McdBox",McdBoxDestroy,McdBoxUpdateAABB,McdBoxGetBSphere,McdBoxMaximumPoint,
             McdBoxGetMassProperties,McdBoxDebugDraw);
  return;
}


/* ==== McdBoxCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

McdBoxID McdBoxCreate(McdFramework *frame,MeReal dx,MeReal dy,MeReal dz)

{
  float fVar1;
  McdGeometryID pMVar2;
  McdGeometryID pMVar3;
  McdBoxID pMVar4;
  McdBoxID pMVar5;
  undefined4 extraout_ECX;
  
                    /* Unresolved local var: McdBoxID b@[DW_OP_reg3(EBX)] */
  pMVar4 = (McdBoxID)(*_McdGeometryDeinit)(0x20,0x10);
  pMVar5 = (McdBoxID)0x0;
  if (pMVar4 != (McdBoxID)0x0) {
    McdGeometryInit(pMVar4,frame,2,extraout_ECX);
                    /* Unresolved local var: McdBox * b@[???]
                       Unresolved local var: float __result@[???] */
    fVar1 = dx * 0.5;
    pMVar4[1].mRefCtAndID = (MeU32)fVar1;
    pMVar3 = (McdGeometryID)(dy * 0.5);
    pMVar4[1].prev = pMVar3;
    pMVar2 = (McdGeometryID)(dz * 0.5);
    pMVar4[1].next = pMVar2;
    pMVar4[1].frame =
         (McdFrameworkID)
         SQRT((float)pMVar2 * (float)pMVar2 + fVar1 * fVar1 + (float)pMVar3 * (float)pMVar3);
    pMVar5 = pMVar4;
  }
  return pMVar5;
}


/* ==== McdBoxSetDimensions ==== */

void McdBoxSetDimensions(McdGeometryID g,MeReal dx,MeReal dy,MeReal dz)

{
  McdGeometryID pMVar1;
  float fVar2;
  McdGeometryID pMVar3;
  
                    /* Unresolved local var: McdBox * b@[???]
                       Unresolved local var: float __result@[???] */
  pMVar1 = (McdGeometryID)(dy * 0.5);
  fVar2 = dx * 0.5;
  pMVar3 = (McdGeometryID)(dz * 0.5);
  g[1].prev = pMVar1;
  g[1].mRefCtAndID = (MeU32)fVar2;
  g[1].next = pMVar3;
  g[1].frame = (McdFrameworkID)
               SQRT((float)pMVar3 * (float)pMVar3 + fVar2 * fVar2 + (float)pMVar1 * (float)pMVar1);
  return;
}


/* ==== McdBoxGetDimensions ==== */

void McdBoxGetDimensions(McdGeometryID g,MeReal *dx,MeReal *dy,MeReal *dz)

{
                    /* Unresolved local var: McdBox * b@[???] */
  *dx = (float)g[1].mRefCtAndID + (float)g[1].mRefCtAndID;
  *dy = (float)g[1].prev + (float)g[1].prev;
  *dz = (float)g[1].next + (float)g[1].next;
  return;
}


/* ==== McdBoxGetBSphereRadius ==== */

MeReal McdBoxGetBSphereRadius(McdGeometryID g)

{
  return (MeReal)g[1].frame;
}


/* ==== McdBoxGetRadii ==== */

/* WARNING: Unknown calling convention */

MeReal * McdBoxGetRadii(McdBoxID g)

{
                    /* Unresolved local var: McdBox * b@[???] */
  return (MeReal *)(g + 1);
}


/* ==== McdBoxDestroy ==== */

/* WARNING: Unknown calling convention */

void McdBoxDestroy(McdGeometry *g)

{
  McdGeometryDeinit();
  return;
}


/* ==== McdBoxUpdateAABB ==== */

void McdBoxUpdateAABB(McdGeometryInstanceID ins,MeVector4 *finalTM,MeBool tight)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  int iVar7;
  float *pfVar8;
  MeReal max [3];
  MeReal min [3];
  MeReal rFit [3];
  
                    /* Unresolved local var: McdBox * b@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg0(EAX)] */
  iVar7 = McdGeometryInstanceGetGeometry(ins);
  pfVar8 = (float *)McdGeometryInstanceGetTransformPtr(ins);
  fVar1 = ABS(pfVar8[8]) * *(float *)(iVar7 + 0x18) +
          ABS(pfVar8[4]) * *(float *)(iVar7 + 0x14) + ABS(*pfVar8) * *(float *)(iVar7 + 0x10);
  fVar2 = ABS(pfVar8[9]) * *(float *)(iVar7 + 0x18) +
          ABS(pfVar8[1]) * *(float *)(iVar7 + 0x10) + ABS(pfVar8[5]) * *(float *)(iVar7 + 0x14);
  fVar3 = ABS(pfVar8[10]) * *(float *)(iVar7 + 0x18) +
          ABS(pfVar8[6]) * *(float *)(iVar7 + 0x14) + ABS(pfVar8[2]) * *(float *)(iVar7 + 0x10);
  ins->min[0] = pfVar8[0xc] - fVar1;
  ins->min[1] = pfVar8[0xd] - fVar2;
  ins->min[2] = pfVar8[0xe] - fVar3;
  ins->max[0] = fVar1 + pfVar8[0xc];
  ins->max[1] = fVar2 + pfVar8[0xd];
  ins->max[2] = fVar3 + pfVar8[0xe];
  if (finalTM != (MeVector4 *)0x0) {
    fVar1 = ABS(finalTM[2][0]) * *(float *)(iVar7 + 0x18) +
            ABS(finalTM[1][0]) * *(float *)(iVar7 + 0x14) +
            ABS((*finalTM)[0]) * *(float *)(iVar7 + 0x10);
    fVar3 = ABS(finalTM[2][1]) * *(float *)(iVar7 + 0x18) +
            ABS(finalTM[1][1]) * *(float *)(iVar7 + 0x14) +
            ABS((*finalTM)[1]) * *(float *)(iVar7 + 0x10);
    fVar5 = ABS(finalTM[2][2]) * *(float *)(iVar7 + 0x18) +
            ABS(finalTM[1][2]) * *(float *)(iVar7 + 0x14) +
            ABS((*finalTM)[2]) * *(float *)(iVar7 + 0x10);
    fVar2 = finalTM[3][0] + fVar1;
    fVar1 = finalTM[3][0] - fVar1;
    fVar4 = finalTM[3][1] + fVar3;
    fVar3 = finalTM[3][1] - fVar3;
    fVar6 = finalTM[3][2] + fVar5;
    fVar5 = finalTM[3][2] - fVar5;
    if (ins->min[0] < fVar1) {
      fVar1 = ins->min[0];
    }
    ins->min[0] = fVar1;
    if (ins->min[1] < fVar3) {
      fVar3 = ins->min[1];
    }
    ins->min[1] = fVar3;
    if (ins->min[2] < fVar5) {
      fVar5 = ins->min[2];
    }
    ins->min[2] = fVar5;
    if (fVar2 < ins->max[0]) {
      fVar2 = ins->max[0];
    }
    ins->max[0] = fVar2;
    if (fVar4 < ins->max[1]) {
      fVar4 = ins->max[1];
    }
    ins->max[1] = fVar4;
    if (fVar6 < ins->max[2]) {
      fVar6 = ins->max[2];
    }
    ins->max[2] = fVar6;
  }
  return;
}


/* ==== McdBoxGetXYAABB ==== */

void McdBoxGetXYAABB(McdGeometry *g,lsTransform *tm,MeReal *bounds)

{
  float fVar1;
  float fVar2;
  
                    /* Unresolved local var: McdBox * b@[???]
                       Unresolved local var: MeReal[2] rFit@[DW_OP_reg0(EAX)] */
  fVar1 = ABS(tm->row[2].v.v[0]) * (float)g[1].next +
          ABS(tm->row[1].v.v[0]) * (float)g[1].prev +
          ABS(tm->row[0].v.v[0]) * (float)g[1].mRefCtAndID;
  fVar2 = ABS(tm->row[0].v.v[1]) * (float)g[1].mRefCtAndID +
          ABS(tm->row[1].v.v[1]) * (float)g[1].prev + (float)g[1].next * ABS(tm->row[2].v.v[1]);
  *bounds = tm->row[3].v.v[0] - fVar1;
  bounds[1] = fVar1 + tm->row[3].v.v[0];
  bounds[2] = tm->row[3].v.v[1] - fVar2;
  bounds[3] = fVar2 + tm->row[3].v.v[1];
  return;
}


/* ==== McdBoxGetBSphere ==== */

void McdBoxGetBSphere(McdGeometry *g,MeReal *center,MeReal *radius)

{
                    /* Unresolved local var: McdBox * b@[???] */
  *center = 0.0;
  center[2] = 0.0;
  center[1] = 0.0;
  *radius = (MeReal)g[1].frame;
  return;
}


/* ==== localSignZ ==== */

MeReal localSignZ(MeReal x)

{
  MeReal MVar1;
  
  MVar1 = 0.0;
  if ((x != 0.0) && (MVar1 = 1.0, x < 0.0)) {
    MVar1 = -1.0;
  }
  return MVar1;
}


/* ==== McdBoxMaximumPoint ==== */

void McdBoxMaximumPoint(McdGeometryInstanceID ins,MeReal *inDir,MeReal *outPoint)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  int iVar6;
  float *pfVar7;
  lsVec3 sR;
  lsVec3 n;
  
                    /* Unresolved local var: McdBox * b@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * tm@[DW_OP_reg1(ECX)] */
  iVar6 = McdGeometryInstanceGetGeometry(ins);
  pfVar7 = (float *)McdGeometryInstanceGetTransformPtr(ins);
  fVar1 = pfVar7[2] * inDir[2] + pfVar7[1] * inDir[1] + *pfVar7 * *inDir;
  fVar4 = pfVar7[6] * inDir[2] + pfVar7[5] * inDir[1] + pfVar7[4] * *inDir;
  fVar5 = pfVar7[10] * inDir[2] + pfVar7[9] * inDir[1] + pfVar7[8] * *inDir;
  fVar3 = 0.0;
  if ((fVar1 != 0.0) && (fVar3 = 1.0, fVar1 < 0.0)) {
    fVar3 = -1.0;
  }
  fVar1 = 0.0;
  fVar3 = fVar3 * *(float *)(iVar6 + 0x10);
  fVar2 = fVar1;
  if ((fVar4 != 0.0) && (fVar2 = 1.0, fVar4 < 0.0)) {
    fVar2 = -1.0;
  }
  fVar2 = fVar2 * *(float *)(iVar6 + 0x14);
  if ((fVar5 != 0.0) && (fVar1 = 1.0, fVar5 < 0.0)) {
    fVar1 = -1.0;
  }
  fVar1 = fVar1 * *(float *)(iVar6 + 0x18);
  *outPoint = pfVar7[8] * fVar1 + pfVar7[4] * fVar2 + *pfVar7 * fVar3 + pfVar7[0xc];
  outPoint[1] = pfVar7[9] * fVar1 + pfVar7[5] * fVar2 + pfVar7[1] * fVar3 + pfVar7[0xd];
  outPoint[2] = pfVar7[10] * fVar1 + pfVar7[6] * fVar2 + pfVar7[2] * fVar3 + pfVar7[0xe];
  return;
}


/* ==== McdBoxGetMassProperties ==== */

MeI16 McdBoxGetMassProperties(McdGeometry *g,MeVector4 *relTM,MeVector3 *m,MeReal *volume)

{
  McdGeometryID pMVar1;
  McdGeometryID pMVar2;
  float fVar3;
  
                    /* Unresolved local var: McdBox * b@[???]
                       Unresolved local var: MeReal Box_coef@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal rx@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal rz@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal ry@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal mass@[???] */
  pMVar1 = g[1].prev;
  pMVar2 = g[1].next;
  fVar3 = (float)g[1].mRefCtAndID;
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???] */
  (*m)[1] = 0.0;
  (*m)[2] = 0.0;
  m[1][0] = 0.0;
  m[1][2] = 0.0;
  m[2][0] = 0.0;
  m[2][1] = 0.0;
  (*m)[0] = ((float)pMVar1 * (float)pMVar1 + (float)pMVar2 * (float)pMVar2) * 0.33333334;
  m[1][1] = ((float)pMVar2 * (float)pMVar2 + fVar3 * fVar3) * 0.33333334;
  m[2][2] = (fVar3 * fVar3 + (float)pMVar1 * (float)pMVar1) * 0.33333334;
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
  *volume = fVar3 * (float)pMVar1 * (float)pMVar2 * 8.0;
  return 0;
}


/* ==== McdBoxMaximumPointLocal ==== */

void McdBoxMaximumPointLocal(McdBoxID g,MeReal *inDir,MeReal *outPoint)

{
  float fVar1;
  float fVar2;
  
                    /* Unresolved local var: McdBox * b@[???] */
  fVar2 = 0.0;
  if ((*inDir != 0.0) && (fVar2 = 1.0, *inDir < 0.0)) {
    fVar2 = -1.0;
  }
  fVar1 = 0.0;
  *outPoint = fVar2 * (float)g[1].mRefCtAndID;
  fVar2 = fVar1;
  if ((inDir[1] != 0.0) && (fVar2 = 1.0, inDir[1] < 0.0)) {
    fVar2 = -1.0;
  }
  outPoint[1] = fVar2 * (float)g[1].prev;
  if ((inDir[2] != 0.0) && (fVar1 = 1.0, inDir[2] < 0.0)) {
    fVar1 = -1.0;
  }
  outPoint[2] = fVar1 * (float)g[1].next;
  return;
}


/* ==== McdBoxGetXYAABB ==== */

void McdBoxGetXYAABB(McdBoxID g,MeVector4 *_tm,MeReal *bounds)

{
  float fVar1;
  float fVar2;
  
                    /* Unresolved local var: McdBox * b@[???]
                       Unresolved local var: lsTransform * tm@[???]
                       Unresolved local var: MeReal[2] rFit@[DW_OP_reg0(EAX)] */
  fVar1 = ABS(_tm[2][0]) * (float)g[1].next +
          ABS(_tm[1][0]) * (float)g[1].prev + ABS((*_tm)[0]) * (float)g[1].mRefCtAndID;
  fVar2 = ABS((*_tm)[1]) * (float)g[1].mRefCtAndID + ABS(_tm[1][1]) * (float)g[1].prev +
          (float)g[1].next * ABS(_tm[2][1]);
  *bounds = _tm[3][0] - fVar1;
  bounds[1] = fVar1 + _tm[3][0];
  bounds[2] = _tm[3][1] - fVar2;
  bounds[3] = fVar2 + _tm[3][1];
  return;
}


/* ==== McdBoxDebugDraw ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdBoxDebugDraw(McdGeometryID geom,MeReal (*tm) [4],MeReal *colour)

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
  MeReal lv [3];
  MeReal wv2 [3];
  MeReal wv1 [3];
  MeReal rads [3];
  
                    /* Unresolved local var: McdBoxID box@[???]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  if ((char)geom->mRefCtAndID == '\x02') {
                    /* Unresolved local var: McdBox * b@[???] */
    iVar9 = 0;
    iVar10 = 0xb;
    fVar1 = ((float)geom[1].mRefCtAndID + (float)geom[1].mRefCtAndID) * 0.5;
    fVar2 = ((float)geom[1].prev + (float)geom[1].prev) * 0.5;
    fVar3 = ((float)geom[1].next + (float)geom[1].next) * 0.5;
    do {
      fVar4 = fVar2 * *(float *)((int)McdBoxDebugDraw::boxDraw[0][0] + iVar9 + 4);
      fVar5 = fVar1 * *(float *)((int)McdBoxDebugDraw::boxDraw[0][0] + iVar9);
      fVar7 = fVar3 * *(float *)((int)McdBoxDebugDraw::boxDraw[0][0] + iVar9 + 8);
      wv1[0] = tm[2][0] * fVar7 + fVar5 * (*tm)[0] + fVar4 * tm[1][0] + tm[3][0];
      wv1[1] = fVar7 * tm[2][1] + fVar4 * tm[1][1] + fVar5 * (*tm)[1] + tm[3][1];
      fVar8 = fVar2 * *(float *)((int)McdBoxDebugDraw::boxDraw[0][1] + iVar9 + 4);
      fVar6 = fVar1 * *(float *)((int)McdBoxDebugDraw::boxDraw[0][1] + iVar9);
      wv1[2] = fVar5 * (*tm)[2] + fVar4 * tm[1][2] + fVar7 * tm[2][2] + tm[3][2];
      fVar4 = fVar3 * *(float *)((int)McdBoxDebugDraw::boxDraw[0][1] + iVar9 + 8);
      iVar9 = iVar9 + 0x18;
      wv2[0] = (*tm)[0] * fVar6 + tm[1][0] * fVar8 + tm[2][0] * fVar4 + tm[3][0];
      wv2[1] = (*tm)[1] * fVar6 + tm[1][1] * fVar8 + tm[2][1] * fVar4 + tm[3][1];
      wv2[2] = fVar6 * (*tm)[2] + fVar8 * tm[1][2] + fVar4 * tm[2][2] + tm[3][2];
      (*_MeDebugDrawAPI)(wv1,wv2,colour[1],colour[2],colour[3]);
      iVar10 = iVar10 + -1;
    } while (-1 < iVar10);
  }
  return;
}


