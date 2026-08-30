/* ==== McdSphylGetTypeId ==== */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

MeI16 McdSphylGetTypeId(void)

{
  return 5;
}


/* ==== McdSphylRegisterType ==== */

/* WARNING: Unknown calling convention */

void McdSphylRegisterType(McdFramework *frame)

{
  McdFrameworkRegisterGeometryType
            (frame,5,"McdSphyl",McdSphylDestroy,McdSphylUpdateAABB,McdSphylGetBSphere,
             McdSphylMaximumPoint,McdSphylGetMassProperties,McdSphylDebugDraw);
  return;
}


/* ==== McdSphylCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

McdSphylID McdSphylCreate(McdFramework *frame,MeReal r,MeReal h)

{
  McdSphylID pMVar1;
  
                    /* Unresolved local var: McdSphylID s@[DW_OP_reg3(EBX)] */
  pMVar1 = (McdSphylID)(*_McdGeometryDeinit)(0x18,0x10);
  if (pMVar1 != (McdSphylID)0x0) {
    McdGeometryInit(pMVar1,frame,5);
                    /* Unresolved local var: McdSphyl * sphyl@[???] */
    if (r < 0.0) {
      r = 0.0;
    }
    pMVar1[1].mRefCtAndID = (MeU32)r;
                    /* Unresolved local var: McdSphyl * sphyl@[???] */
    if (h < 0.0) {
      h = 0.0;
    }
    pMVar1[1].prev = (McdGeometryID)(h * 0.5);
  }
  return pMVar1;
}


/* ==== McdSphylSetRadius ==== */

void McdSphylSetRadius(McdSphylID s,MeReal r)

{
                    /* Unresolved local var: McdSphyl * sphyl@[???] */
  if (r < 0.0) {
    r = 0.0;
  }
  s[1].mRefCtAndID = (MeU32)r;
  return;
}


/* ==== McdSphylGetRadius ==== */

MeReal McdSphylGetRadius(McdSphylID s)

{
  return (MeReal)s[1].mRefCtAndID;
}


/* ==== McdSphylSetHeight ==== */

void McdSphylSetHeight(McdSphylID s,MeReal h)

{
                    /* Unresolved local var: McdSphyl * sphyl@[???] */
  if (h < 0.0) {
    h = 0.0;
  }
  s[1].prev = (McdGeometryID)(h * 0.5);
  return;
}


/* ==== McdSphylGetHeight ==== */

MeReal McdSphylGetHeight(McdSphylID s)

{
  return (float)s[1].prev + (float)s[1].prev;
}


/* ==== McdSphylDestroy ==== */

/* WARNING: Unknown calling convention */

void McdSphylDestroy(McdSphylID s)

{
  McdGeometryDeinit(s);
  return;
}


/* ==== McdSphylUpdateAABB ==== */

void McdSphylUpdateAABB(McdGeometryInstanceID ins,MeMatrix4Ptr finalTM,MeBool tight)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  void *pvVar8;
  void *pvVar9;
  MeReal fMax [3];
  MeReal fMin [3];
  MeReal absAx [3];
  
                    /* Unresolved local var: McdSphyl * s@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg0(EAX)] */
  pvVar8 = McdGeometryInstanceGetGeometry(ins);
  pvVar9 = McdGeometryInstanceGetTransformPtr(ins);
  fVar1 = ABS(*(float *)((int)pvVar9 + 0x20)) * *(float *)((int)pvVar8 + 0x14) +
          *(float *)((int)pvVar8 + 0x10);
  fVar3 = ABS(*(float *)((int)pvVar9 + 0x24)) * *(float *)((int)pvVar8 + 0x14) +
          *(float *)((int)pvVar8 + 0x10);
  fVar4 = ABS(*(float *)((int)pvVar9 + 0x28)) * *(float *)((int)pvVar8 + 0x14) +
          *(float *)((int)pvVar8 + 0x10);
  fVar2 = *(float *)((int)pvVar9 + 0x30) - fVar1;
  ins->min[0] = fVar2;
  ins->min[1] = *(float *)((int)pvVar9 + 0x34) - fVar3;
  ins->min[2] = *(float *)((int)pvVar9 + 0x38) - fVar4;
  ins->max[0] = fVar1 + *(float *)((int)pvVar9 + 0x30);
  ins->max[1] = fVar3 + *(float *)((int)pvVar9 + 0x34);
  ins->max[2] = fVar4 + *(float *)((int)pvVar9 + 0x38);
  if (finalTM != (MeMatrix4Ptr)0x0) {
    fVar1 = ABS(finalTM[2][0]) * *(float *)((int)pvVar8 + 0x14) + *(float *)((int)pvVar8 + 0x10);
    fVar4 = ABS(finalTM[2][1]) * *(float *)((int)pvVar8 + 0x14) + *(float *)((int)pvVar8 + 0x10);
    fVar6 = ABS(finalTM[2][2]) * *(float *)((int)pvVar8 + 0x14) + *(float *)((int)pvVar8 + 0x10);
    fVar3 = finalTM[3][0] + fVar1;
    fVar1 = finalTM[3][0] - fVar1;
    fVar5 = finalTM[3][1] + fVar4;
    fVar4 = finalTM[3][1] - fVar4;
    fVar7 = finalTM[3][2] + fVar6;
    fVar6 = finalTM[3][2] - fVar6;
    if (fVar2 < fVar1) {
      fVar1 = fVar2;
    }
    ins->min[0] = fVar1;
    if (ins->min[1] < fVar4) {
      fVar4 = ins->min[1];
    }
    ins->min[1] = fVar4;
    if (ins->min[2] < fVar6) {
      fVar6 = ins->min[2];
    }
    ins->min[2] = fVar6;
    if (fVar3 < ins->max[0]) {
      fVar3 = ins->max[0];
    }
    ins->max[0] = fVar3;
    if (fVar5 < ins->max[1]) {
      fVar5 = ins->max[1];
    }
    ins->max[1] = fVar5;
    if (fVar7 < ins->max[2]) {
      fVar7 = ins->max[2];
    }
    ins->max[2] = fVar7;
  }
  return;
}


/* ==== McdSphylGetBSphere ==== */

void McdSphylGetBSphere(McdSphylID s,MeReal *center,MeReal *radius)

{
                    /* Unresolved local var: McdSphyl * sphyl@[???] */
  *center = 0.0;
  center[2] = 0.0;
  center[1] = 0.0;
  *radius = (float)s[1].mRefCtAndID + (float)s[1].prev;
  return;
}


/* ==== McdSphylMaximumPoint ==== */

void McdSphylMaximumPoint(McdGeometryInstanceID ins,MeReal *inDir,MeReal *outPoint)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  void *pvVar9;
  void *pvVar10;
  McdSphyl *s;
  
                    /* Unresolved local var: MeReal nZ@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg2(EDX)] */
  pvVar9 = McdGeometryInstanceGetGeometry(ins);
  pvVar10 = McdGeometryInstanceGetTransformPtr(ins);
  fVar1 = *inDir;
  fVar2 = inDir[1];
  fVar3 = *(float *)((int)pvVar10 + 0x20);
  fVar4 = *(float *)((int)pvVar10 + 0x30);
  fVar5 = *(float *)((int)pvVar10 + 0x24);
  fVar6 = inDir[2];
  fVar7 = *(float *)((int)pvVar10 + 0x28);
  *outPoint = fVar4;
  fVar8 = *(float *)((int)pvVar10 + 0x34);
  fVar2 = fVar2 * fVar5 + fVar1 * fVar3 + fVar6 * fVar7;
  outPoint[1] = fVar8;
  fVar1 = *(float *)((int)pvVar10 + 0x38);
  outPoint[2] = fVar1;
  if (fVar2 <= 0.0001) {
    if (fVar2 < -0.0001) {
      fVar1 = -*(float *)((int)pvVar9 + 0x14);
      *outPoint = fVar1 * *(float *)((int)pvVar10 + 0x20) + *outPoint;
      outPoint[1] = fVar1 * *(float *)((int)pvVar10 + 0x24) + outPoint[1];
      outPoint[2] = fVar1 * *(float *)((int)pvVar10 + 0x28) + outPoint[2];
    }
  }
  else {
    fVar2 = *(float *)((int)pvVar9 + 0x14);
    *outPoint = fVar2 * *(float *)((int)pvVar10 + 0x20) + fVar4;
    outPoint[1] = fVar2 * *(float *)((int)pvVar10 + 0x24) + fVar8;
    outPoint[2] = fVar2 * *(float *)((int)pvVar10 + 0x28) + fVar1;
  }
  fVar1 = *(float *)((int)pvVar9 + 0x10);
  *outPoint = fVar1 * *inDir + *outPoint;
  outPoint[1] = fVar1 * inDir[1] + outPoint[1];
  outPoint[2] = fVar1 * inDir[2] + outPoint[2];
  return;
}


/* ==== McdSphylGetMassProperties ==== */

MeI16 McdSphylGetMassProperties(McdSphylID s,MeVector4 *relTM,MeVector3 *m,MeReal *volume)

{
  float fVar1;
  float fVar2;
  float fVar3;
  
                    /* Unresolved local var: McdSphyl * sphyl@[???]
                       Unresolved local var: MeReal r@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal l@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal mass@[???] */
  fVar1 = (float)s[1].mRefCtAndID;
  fVar3 = fVar1 * 0.67082 + (float)s[1].prev;
  fVar3 = fVar3 + fVar3;
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???] */
  (*m)[1] = 0.0;
  (*m)[2] = 0.0;
  m[1][0] = 0.0;
  m[1][2] = 0.0;
  m[2][0] = 0.0;
  m[2][1] = 0.0;
  fVar2 = fVar3 * fVar3 * 0.083333336 + fVar1 * fVar1 * 0.25;
  (*m)[0] = fVar2;
  m[1][1] = fVar2;
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
  *volume = fVar1 * fVar1 * fVar3 * 3.1415927;
  return 0;
}


/* ==== McdSphylDebugDraw ==== */

/* WARNING: Unknown calling convention */

void McdSphylDebugDraw(McdGeometryID geom,MeReal (*tm) [4],MeReal *colour)

{
  return;
}


