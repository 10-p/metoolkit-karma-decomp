/* ==== McdTriangleListGetTypeId ==== */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

MeI16 McdTriangleListGetTypeId(void)

{
  return 6;
}


/* ==== McdTriangleListRegisterType ==== */

/* WARNING: Unknown calling convention */

void McdTriangleListRegisterType(McdFramework *frame)

{
  McdFrameworkRegisterGeometryType
            (frame,6,"McdTriangleList",McdTriangleListDestroy,McdTriangleListUpdateAABB,
             McdTriangleListGetBSphere,McdTriangleListMaximumPoint,McdTriangleListGetMassProperties,
             McdTriangleListDebugDraw);
  return;
}


/* ==== McdTriangleListCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdTriangleListID
McdTriangleListCreate
          (McdFramework *frame,MeReal *min,MeReal *max,int maxCount,McdTriangleListFnPtr f)

{
  McdTriangleListID pMVar1;
  
                    /* Unresolved local var: McdTriangleList * tl@[DW_OP_reg3(EBX)] */
  pMVar1 = (McdTriangleListID)0x0;
  if (f != (McdTriangleListFnPtr)0x0) {
    pMVar1 = (McdTriangleListID)(*_McdGeometryDeinit)(0x38,0x10);
    McdGeometryInit(pMVar1,frame,6);
                    /* Unresolved local var: McdTriangleList * b@[???] */
    pMVar1[1].mRefCtAndID = (MeU32)(*max + *min);
    pMVar1[1].prev = (McdGeometryID)(max[1] + min[1]);
    pMVar1[1].next = (McdGeometryID)(max[2] + min[2]);
    pMVar1[1].mRefCtAndID = (MeU32)((float)pMVar1[1].mRefCtAndID * 0.5);
    pMVar1[1].prev = (McdGeometryID)((float)pMVar1[1].prev * 0.5);
    pMVar1[1].next = (McdGeometryID)((float)pMVar1[1].next * 0.5);
    pMVar1[1].frame = (McdFrameworkID)(*max - *min);
    pMVar1[2].mRefCtAndID = (MeU32)(max[1] - min[1]);
    pMVar1[2].prev = (McdGeometryID)(max[2] - min[2]);
    pMVar1[1].frame = (McdFrameworkID)((float)pMVar1[1].frame * 0.5);
    pMVar1[2].mRefCtAndID = (MeU32)((float)pMVar1[2].mRefCtAndID * 0.5);
    pMVar1[2].prev = (McdGeometryID)((float)pMVar1[2].prev * 0.5);
    pMVar1[3].mRefCtAndID = (MeU32)f;
    pMVar1[2].next = (McdGeometryID)maxCount;
  }
  return pMVar1;
}


/* ==== McdTriangleListSetBoundingBox ==== */

void McdTriangleListSetBoundingBox(McdTriangleListID g,MeReal *min,MeReal *max)

{
                    /* Unresolved local var: McdTriangleList * b@[???] */
  g[1].mRefCtAndID = (MeU32)(*max + *min);
  g[1].prev = (McdGeometryID)(max[1] + min[1]);
  g[1].next = (McdGeometryID)(max[2] + min[2]);
  g[1].mRefCtAndID = (MeU32)((float)g[1].mRefCtAndID * 0.5);
  g[1].prev = (McdGeometryID)((float)g[1].prev * 0.5);
  g[1].next = (McdGeometryID)((float)g[1].next * 0.5);
  g[1].frame = (McdFrameworkID)(*max - *min);
  g[2].mRefCtAndID = (MeU32)(max[1] - min[1]);
  g[2].prev = (McdGeometryID)(max[2] - min[2]);
  g[1].frame = (McdFrameworkID)((float)g[1].frame * 0.5);
  g[2].mRefCtAndID = (MeU32)((float)g[2].mRefCtAndID * 0.5);
  g[2].prev = (McdGeometryID)((float)g[2].prev * 0.5);
  return;
}


/* ==== McdTriangleListGetBoundingBox ==== */

void McdTriangleListGetBoundingBox(McdTriangleListID g,MeReal *min,MeReal *max)

{
                    /* Unresolved local var: McdTriangleList * b@[???] */
  *max = (float)g[1].frame + (float)g[1].mRefCtAndID;
  max[1] = (float)g[2].mRefCtAndID + (float)g[1].prev;
  max[2] = (float)g[2].prev + (float)g[1].next;
  *min = (float)g[1].mRefCtAndID - (float)g[1].frame;
  min[1] = (float)g[1].prev - (float)g[2].mRefCtAndID;
  min[2] = (float)g[1].next - (float)g[2].prev;
  return;
}


/* ==== McdTriangleListSetMaxTriangles ==== */

void McdTriangleListSetMaxTriangles(McdTriangleListID g,int max)

{
                    /* Unresolved local var: McdTriangleList * b@[???] */
  g[2].next = (McdGeometryID)max;
  return;
}


/* ==== McdTriangleListGetMaxTriangles ==== */

int McdTriangleListGetMaxTriangles(McdTriangleListID g)

{
                    /* Unresolved local var: McdTriangleList * b@[???] */
  return (int)g[2].next;
}


/* ==== McdTriangleListSetGenerator ==== */

void McdTriangleListSetGenerator(McdTriangleListID g,McdTriangleListFnPtr f)

{
                    /* Unresolved local var: McdTriangleList * b@[???] */
  g[3].mRefCtAndID = (MeU32)f;
  return;
}


/* ==== McdTriangleListGetBoundingBox ==== */

McdTriangleListFnPtr McdTriangleListGetBoundingBox(McdTriangleListID g)

{
                    /* Unresolved local var: McdTriangleList * b@[???] */
  return (McdTriangleListFnPtr)g[3].mRefCtAndID;
}


/* ==== McdTriangleListDestroy ==== */

void McdTriangleListDestroy(McdGeometry *g)

{
  if (g != (McdGeometry *)0x0) {
    McdGeometryDeinit(g);
    return;
  }
  return;
}


/* ==== McdTriangleListUpdateAABB ==== */

void McdTriangleListUpdateAABB(McdGeometryInstanceID ins,MeMatrix4Ptr finalTM,MeBool tight)

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
  float *pfVar10;
  float local_74;
  float local_6c;
  float local_68;
  float local_64;
  float local_60;
  MeReal max [3];
  MeReal min [3];
  MeReal tCentre [3];
  MeReal rFit [3];
  
                    /* Unresolved local var: McdTriangleList * b@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg0(EAX)] */
  pvVar9 = McdGeometryInstanceGetGeometry(ins);
  pfVar10 = McdGeometryInstanceGetTransformPtr(ins);
  local_60 = ABS(*pfVar10);
  fVar1 = ABS(pfVar10[8]) * *(float *)((int)pvVar9 + 0x24) +
          ABS(pfVar10[4]) * *(float *)((int)pvVar9 + 0x20) +
          local_60 * *(float *)((int)pvVar9 + 0x1c);
  local_64 = ABS(pfVar10[1]);
  fVar2 = ABS(pfVar10[9]) * *(float *)((int)pvVar9 + 0x24) +
          ABS(pfVar10[5]) * *(float *)((int)pvVar9 + 0x20) +
          local_64 * *(float *)((int)pvVar9 + 0x1c);
  fVar3 = ABS(pfVar10[10]) * *(float *)((int)pvVar9 + 0x24) +
          ABS(pfVar10[6]) * *(float *)((int)pvVar9 + 0x20) +
          ABS(pfVar10[2]) * *(float *)((int)pvVar9 + 0x1c);
  fVar5 = pfVar10[8] * *(float *)((int)pvVar9 + 0x18) +
          pfVar10[4] * *(float *)((int)pvVar9 + 0x14) + *pfVar10 * *(float *)((int)pvVar9 + 0x10) +
          pfVar10[0xc];
  fVar6 = pfVar10[9] * *(float *)((int)pvVar9 + 0x18) +
          pfVar10[5] * *(float *)((int)pvVar9 + 0x14) + pfVar10[1] * *(float *)((int)pvVar9 + 0x10)
          + pfVar10[0xd];
  fVar4 = pfVar10[10] * *(float *)((int)pvVar9 + 0x18) +
          pfVar10[6] * *(float *)((int)pvVar9 + 0x14) + pfVar10[2] * *(float *)((int)pvVar9 + 0x10)
          + pfVar10[0xe];
  ins->min[0] = fVar5 - fVar1;
  ins->min[1] = fVar6 - fVar2;
  ins->min[2] = fVar4 - fVar3;
  ins->max[0] = fVar5 + fVar1;
  ins->max[1] = fVar6 + fVar2;
  ins->max[2] = fVar4 + fVar3;
  if (finalTM != (MeMatrix4Ptr)0x0) {
    fVar3 = finalTM[2][0] * *(float *)((int)pvVar9 + 0x18) +
            finalTM[1][0] * *(float *)((int)pvVar9 + 0x14) +
            *(float *)((int)pvVar9 + 0x10) * (*finalTM)[0] + finalTM[3][0];
    fVar2 = finalTM[2][1] * *(float *)((int)pvVar9 + 0x18) +
            *(float *)((int)pvVar9 + 0x14) * finalTM[1][1] +
            *(float *)((int)pvVar9 + 0x10) * (*finalTM)[1] + finalTM[3][1];
    fVar8 = *(float *)((int)pvVar9 + 0x10) * (*finalTM)[2] +
            *(float *)((int)pvVar9 + 0x14) * finalTM[1][2] +
            *(float *)((int)pvVar9 + 0x18) * finalTM[2][2] + finalTM[3][2];
    local_68 = ABS(finalTM[1][0]);
    local_6c = ABS(finalTM[2][0]);
    fVar1 = local_6c * *(float *)((int)pvVar9 + 0x24) +
            local_68 * *(float *)((int)pvVar9 + 0x20) +
            ABS((*finalTM)[0]) * *(float *)((int)pvVar9 + 0x1c);
    local_74 = ABS(finalTM[2][1]);
    fVar4 = local_74 * *(float *)((int)pvVar9 + 0x24) +
            ABS((*finalTM)[1]) * *(float *)((int)pvVar9 + 0x1c) +
            ABS(finalTM[1][1]) * *(float *)((int)pvVar9 + 0x20);
    fVar5 = fVar2 - fVar4;
    fVar4 = fVar4 + fVar2;
    fVar6 = ABS((*finalTM)[2]) * *(float *)((int)pvVar9 + 0x1c) +
            ABS(finalTM[1][2]) * *(float *)((int)pvVar9 + 0x20) +
            ABS(finalTM[2][2]) * *(float *)((int)pvVar9 + 0x24);
    fVar2 = fVar3 - fVar1;
    fVar7 = fVar8 - fVar6;
    fVar1 = fVar1 + fVar3;
    fVar8 = fVar8 + fVar6;
    if (ins->min[0] < fVar2) {
      fVar2 = ins->min[0];
    }
    ins->min[0] = fVar2;
    if (ins->min[1] < fVar5) {
      fVar5 = ins->min[1];
    }
    ins->min[1] = fVar5;
    if (ins->min[2] < fVar7) {
      fVar7 = ins->min[2];
    }
    ins->min[2] = fVar7;
    if (fVar1 < ins->max[0]) {
      fVar1 = ins->max[0];
    }
    ins->max[0] = fVar1;
    if (fVar4 < ins->max[1]) {
      fVar4 = ins->max[1];
    }
    ins->max[1] = fVar4;
    if (fVar8 < ins->max[2]) {
      fVar8 = ins->max[2];
    }
    ins->max[2] = fVar8;
  }
  return;
}


/* ==== McdTriangleListGetXYAABB ==== */

void McdTriangleListGetXYAABB(McdGeometry *g,lsTransform *tm,MeReal *bounds)

{
  float fVar1;
  float fVar2;
  
                    /* Unresolved local var: McdTriangleList * b@[???]
                       Unresolved local var: MeReal[2] rFit@[DW_OP_reg0(EAX)] */
  fVar1 = ABS(tm->row[2].v.v[0]) * (float)g[2].prev +
          ABS(tm->row[1].v.v[0]) * (float)g[2].mRefCtAndID +
          ABS(tm->row[0].v.v[0]) * (float)g[1].frame;
  fVar2 = ABS(tm->row[0].v.v[1]) * (float)g[1].frame +
          ABS(tm->row[1].v.v[1]) * (float)g[2].mRefCtAndID +
          (float)g[2].prev * ABS(tm->row[2].v.v[1]);
  *bounds = tm->row[3].v.v[0] - fVar1;
  bounds[1] = fVar1 + tm->row[3].v.v[0];
  bounds[2] = tm->row[3].v.v[1] - fVar2;
  bounds[3] = fVar2 + tm->row[3].v.v[1];
  return;
}


/* ==== McdTriangleListGetBSphere ==== */

void McdTriangleListGetBSphere(McdGeometry *g,MeReal *center,MeReal *radius)

{
                    /* Unresolved local var: McdTriangleList * b@[???] */
  *center = (MeReal)g[1].mRefCtAndID;
  center[1] = (MeReal)g[1].prev;
  center[2] = (MeReal)g[1].next;
                    /* Unresolved local var: MeReal m@[DW_OP_reg12(ST1)]
                       Unresolved local var: float __result@[???] */
  *radius = SQRT((float)g[2].prev * (float)g[2].prev +
                 (float)g[2].mRefCtAndID * (float)g[2].mRefCtAndID +
                 (float)g[1].frame * (float)g[1].frame);
  return;
}


/* ==== McdTriangleListMaximumPoint ==== */

/* WARNING: Unknown calling convention */

void McdTriangleListMaximumPoint(McdGeometryInstanceID ins,MeReal *inDir,MeReal *outPoint)

{
  if (McdTriangleListMaximumPoint::warn != 0) {
    MeWarning(0,"McdTriangleListMaximumPoint: Don\'t know how to get maximum point!");
  }
  McdTriangleListMaximumPoint::warn = 0;
  return;
}


/* ==== McdTriangleListGetMassProperties ==== */

/* WARNING: Unknown calling convention */

MeI16 McdTriangleListGetMassProperties(McdGeometry *g,MeVector4 *relTM,MeVector3 *m,MeReal *volume)

{
  if (McdTriangleListGetMassProperties::warn != 0) {
    MeWarning(0,"McdTriangleListGetMassProperties: Don\'t know how to get mass props!");
  }
  McdTriangleListGetMassProperties::warn = 0;
  return 0;
}


/* ==== McdTriangleListGetUserData ==== */

void * McdTriangleListGetUserData(McdTriangleListID g)

{
  return g[2].frame;
}


/* ==== McdTriangleListSetUserData ==== */

void McdTriangleListSetUserData(McdTriangleListID g,void *data)

{
  g[2].frame = data;
  return;
}


/* ==== McdTriangleListDebugDraw ==== */

/* WARNING: Unknown calling convention */

void McdTriangleListDebugDraw(McdGeometryID geom,MeReal (*tm) [4],MeReal *colour)

{
  if (McdTriangleListDebugDraw::warn != 0) {
    MeWarning(0,"McdTriangleListDebugDraw: Don\'t know how to draw a TriangleList!");
  }
  McdTriangleListDebugDraw::warn = 0;
  return;
}


