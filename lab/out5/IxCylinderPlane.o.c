/* ==== McdCylinderPlaneIntersect ==== */

/* WARNING: Unknown calling convention */

MeBool McdCylinderPlaneIntersect(McdModelPair *p,McdIntersectResult *result)

{
  float fVar1;
  float fVar2;
  float fVar3;
  bool bVar4;
  float *pfVar5;
  float *pfVar6;
  void *pvVar7;
  lsVec3 *plVar8;
  MeBool MVar9;
  McdContact *pMVar10;
  float fVar11;
  float fVar12;
  float inRCyl;
  float inHHCyl;
  MeI16 dimA;
  MeReal rCyl;
  MeReal separation;
  lsVec3 *verts;
  lsVec3 footprint [12];
  lsVec3 diff;
  lsTransform tBA;
  
                    /* Unresolved local var: lsTransform * tA@[DW_OP_reg3(EBX)]
                       Unresolved local var: lsTransform * tB@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal eps@[???]
                       Unresolved local var: McdCylinderID geometry1@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdPlaneID geometry2@[???]
                       Unresolved local var: McdFramework * fwk@[???]
                       Unresolved local var: MeReal hhCyl@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal ZZ2@[DW_OP_reg11(ST0)]
                       Unresolved local var: lsVec3 * v@[DW_OP_reg1(ECX)]
                       Unresolved local var: McdContact * c@[DW_OP_reg3(EBX)] */
  pfVar5 = McdModelGetTransformPtr(p->model1);
  pfVar6 = McdModelGetTransformPtr(p->model2);
  fVar11 = McdModelGetContactTolerance(p->model1);
  fVar12 = McdModelGetContactTolerance(p->model2);
  pvVar7 = McdModelGetGeometry(p->model1);
  McdModelGetGeometry(p->model2);
  result->touch = 0;
  result->contactCount = 0;
  inRCyl = McdCylinderGetRadius(pvVar7);
  inHHCyl = McdCylinderGetHalfHeight(pvVar7);
  tBA.row[0].v.v[0] = pfVar6[2] * pfVar5[2] + pfVar6[1] * pfVar5[1] + *pfVar6 * *pfVar5;
  tBA.row[0].v.v[1] = pfVar6[6] * pfVar5[2] + pfVar6[5] * pfVar5[1] + pfVar6[4] * *pfVar5;
  tBA.row[0].v.v[2] = pfVar6[10] * pfVar5[2] + pfVar6[9] * pfVar5[1] + pfVar6[8] * *pfVar5;
  tBA.row[1].v.v[0] = pfVar6[2] * pfVar5[6] + pfVar6[1] * pfVar5[5] + *pfVar6 * pfVar5[4];
  tBA.row[1].v.v[1] = pfVar6[6] * pfVar5[6] + pfVar6[5] * pfVar5[5] + pfVar6[4] * pfVar5[4];
  tBA.row[1].v.v[2] = pfVar6[10] * pfVar5[6] + pfVar6[9] * pfVar5[5] + pfVar6[8] * pfVar5[4];
  tBA.row[2].v.v[0] = pfVar6[2] * pfVar5[10] + pfVar6[1] * pfVar5[9] + *pfVar6 * pfVar5[8];
  tBA.row[2].v.v[1] = pfVar6[6] * pfVar5[10] + pfVar6[5] * pfVar5[9] + pfVar6[4] * pfVar5[8];
  tBA.row[2].v.v[2] = pfVar6[10] * pfVar5[10] + pfVar6[9] * pfVar5[9] + pfVar6[8] * pfVar5[8];
  fVar2 = pfVar5[0xd] - pfVar6[0xd];
  fVar3 = pfVar5[0xe] - pfVar6[0xe];
  fVar1 = pfVar5[0xc] - pfVar6[0xc];
  plVar8 = footprint;
  tBA.row[3].v.v[0] = fVar3 * pfVar6[2] + fVar1 * *pfVar6 + fVar2 * pfVar6[1];
  tBA.row[3].v.v[1] = fVar3 * pfVar6[6] + fVar1 * pfVar6[4] + fVar2 * pfVar6[5];
  tBA.row[3].v.v[2] = fVar1 * pfVar6[8] + fVar2 * pfVar6[9] + fVar3 * pfVar6[10];
  verts = plVar8;
  bVar4 = OverlapCylPlane(&separation,&verts,fVar12 + fVar11,inRCyl,inHHCyl,&tBA);
  MVar9 = 0;
  if (bVar4) {
    result->normal[0] = pfVar6[8];
    result->normal[1] = pfVar6[9];
    result->normal[2] = pfVar6[10];
    dimA = 2;
    if ((tBA.row[2].v.v[2] * tBA.row[2].v.v[2] <= 0.999999) &&
       (dimA = 3, 1e-06 <= tBA.row[2].v.v[2] * tBA.row[2].v.v[2])) {
      dimA = 1;
    }
    result->contactCount = 0;
    pMVar10 = result->contacts;
    if ((plVar8 != verts) && (0 < result->contactMaxCount)) {
      do {
                    /* Unresolved local var: MeReal s@[DW_OP_reg13(ST2)] */
        fVar1 = plVar8->v[2];
        if (fVar1 < separation * 0.01) {
          pMVar10->position[0] =
               fVar1 * pfVar6[8] + pfVar6[4] * plVar8->v[1] + *pfVar6 * plVar8->v[0] + pfVar6[0xc];
          pMVar10->position[1] =
               pfVar6[9] * plVar8->v[2] + pfVar6[5] * plVar8->v[1] + pfVar6[1] * plVar8->v[0] +
               pfVar6[0xd];
          pMVar10->position[2] =
               pfVar6[10] * plVar8->v[2] + pfVar6[6] * plVar8->v[1] + pfVar6[2] * plVar8->v[0] +
               pfVar6[0xe];
          pMVar10->separation = fVar1;
          pMVar10->dims = dimA | 0x200;
          pMVar10->normal[0] = result->normal[0];
          pMVar10->normal[1] = result->normal[1];
          pMVar10->normal[2] = result->normal[2];
          result->contactCount = result->contactCount + 1;
          pMVar10 = pMVar10 + 1;
        }
        plVar8 = plVar8 + 1;
      } while ((plVar8 != verts) && (result->contactCount < result->contactMaxCount));
    }
    MVar9 = 1;
    result->touch = 1;
  }
  return MVar9;
}


/* ==== McdCylinderPlaneRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

MeBool McdCylinderPlaneRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdCylinderPlaneIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,4,3,&interactions);
  return 1;
}


/* ==== OverlapCylPlane ==== */

bool OverlapCylPlane(MeReal *outSep,lsVec3 **outPos,MeReal inEps,MeReal inRCyl,MeReal inHHCyl,
                    lsTransform *inT12)

{
  float fVar1;
  float fVar2;
  float fVar3;
  lsVec3 vCross;
  lsVec3 nPerp;
  lsVec3 nPara;
  
                    /* Unresolved local var: lsVec3 * cylAxis@[???]
                       Unresolved local var: MeReal rZ@[???] */
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
  nPara.v[0] = inT12->row[2].v.v[1];
  nPara.v[1] = -inT12->row[2].v.v[0];
  if (1e-15 <= nPara.v[1] * nPara.v[1] + nPara.v[0] * nPara.v[0]) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
    fVar1 = SQRT(nPara.v[1] * nPara.v[1] + nPara.v[0] * nPara.v[0]);
    if (fVar1 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
      fVar1 = 1.0 / fVar1;
      nPara.v[0] = nPara.v[0] * fVar1;
      nPara.v[1] = nPara.v[1] * fVar1;
    }
    fVar1 = inT12->row[2].v.v[2];
    fVar2 = inT12->row[2].v.v[1];
    nPerp.v[0] = nPara.v[1] * fVar1 - fVar2 * 0.0;
    fVar3 = inT12->row[2].v.v[0];
    nPerp.v[1] = fVar3 * 0.0 - nPara.v[0] * fVar1;
    nPerp.v[2] = fVar2 * nPara.v[0] - fVar3 * nPara.v[1];
  }
  else {
    nPara.v[0] = 0.0;
    nPara.v[1] = 1.0;
    nPerp.v[0] = 1.0;
    nPerp.v[1] = 0.0;
    nPerp.v[2] = 0.0;
    fVar1 = inT12->row[2].v.v[2];
  }
  nPara.v[2] = 0.0;
  fVar1 = inT12->row[3].v.v[2] - (ABS(nPerp.v[2]) * inRCyl + ABS(fVar1) * inHHCyl);
  *outSep = fVar1;
  if (fVar1 <= inEps) {
    CylPlaneIntersect(outPos,inT12,inRCyl,inHHCyl,&nPara,&nPerp,(lsVec3 *)(inT12->row + 2));
  }
  return fVar1 <= inEps;
}


/* ==== CylPlaneIntersect ==== */

void CylPlaneIntersect(lsVec3 **outList,lsTransform *tAB,MeReal inR,MeReal inHH,lsVec3 *nPerp,
                      lsVec3 *nPara,lsVec3 *cylAxis)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  lsVec3 *plVar5;
  lsVec3 *plVar6;
  MeBool MVar7;
  lsVec3 *listBase;
  MeReal local_b4;
  MeReal local_b0;
  MeReal tOut_5;
  MeReal tIn_5;
  MeReal tOut_4;
  MeReal tIn_4;
  MeReal tOut_3;
  MeReal tIn_3;
  MeReal tOut_2;
  MeReal tIn_2;
  MeReal tOut_1;
  MeReal tIn_1;
  MeReal tOut;
  MeReal tIn;
  lsVec3 local_7c;
  float local_6c;
  float local_68;
  float local_64;
  lsVec3 local_5c;
  lsVec3 d2;
  lsVec3 d1;
  lsVec3 d0;
  
                    /* Unresolved local var: MeReal invD0Z@[???]
                       Unresolved local var: MeReal invD1Z@[???]
                       Unresolved local var: MeReal invD2Z@[???] */
  fVar1 = inR * nPara->v[0];
  fVar2 = inR * nPara->v[1];
  fVar3 = inR * nPara->v[2];
  d1.v[0] = inR * nPerp->v[0];
  d1.v[1] = inR * nPerp->v[1];
  d1.v[2] = inR * nPerp->v[2];
  d2.v[1] = inHH * cylAxis->v[1];
  d2.v[0] = inHH * cylAxis->v[0];
  d2.v[2] = inHH * cylAxis->v[2];
  local_b0 = 0.0;
  if (d1.v[2] != 0.0) {
    local_b0 = 1.0 / d1.v[2];
  }
  local_b4 = 0.0;
  if (d2.v[2] != 0.0) {
    local_b4 = 1.0 / d2.v[2];
  }
  plVar5 = *outList;
  local_5c.v[0] = fVar1 + tAB->row[3].v.v[0];
  local_5c.v[1] = fVar2 + tAB->row[3].v.v[1];
  local_5c.v[2] = fVar3 + tAB->row[3].v.v[2];
  tIn = -1.0;
  tOut = 1.0;
  MVar7 = McdVanillaSegmentPlaneIntersect(&tIn,&tOut,&local_5c,&d2,local_b4);
  if (MVar7 != 0) {
    plVar6 = *outList;
    *outList = plVar6 + 1;
    plVar6->v[0] = tIn * d2.v[0] + local_5c.v[0];
    plVar6->v[1] = tIn * d2.v[1] + local_5c.v[1];
    plVar6->v[2] = tIn * d2.v[2] + local_5c.v[2];
    plVar6 = *outList;
    *outList = plVar6 + 1;
    local_6c = tOut * d2.v[0];
    local_68 = tOut * d2.v[1];
    local_64 = tOut * d2.v[2];
    plVar6->v[0] = local_6c + local_5c.v[0];
    plVar6->v[1] = local_68 + local_5c.v[1];
    plVar6->v[2] = local_64 + local_5c.v[2];
  }
  local_7c.v[0] = d1.v[0] + tAB->row[3].v.v[0];
  local_7c.v[1] = d1.v[1] + tAB->row[3].v.v[1];
  local_7c.v[2] = d1.v[2] + tAB->row[3].v.v[2];
  tIn_1 = -1.0;
  tOut_1 = 1.0;
  MVar7 = McdVanillaSegmentPlaneIntersect(&tIn_1,&tOut_1,&local_7c,&d2,local_b4);
  if (MVar7 != 0) {
    plVar6 = *outList;
    *outList = plVar6 + 1;
    plVar6->v[0] = tIn_1 * d2.v[0] + local_7c.v[0];
    plVar6->v[1] = tIn_1 * d2.v[1] + local_7c.v[1];
    plVar6->v[2] = tIn_1 * d2.v[2] + local_7c.v[2];
    plVar6 = *outList;
    *outList = plVar6 + 1;
    local_5c.v[0] = tOut_1 * d2.v[0];
    local_5c.v[1] = tOut_1 * d2.v[1];
    local_5c.v[2] = tOut_1 * d2.v[2];
    local_6c = local_5c.v[0] + local_7c.v[0];
    local_68 = local_5c.v[1] + local_7c.v[1];
    local_64 = local_5c.v[2] + local_7c.v[2];
    plVar6->v[0] = local_6c;
    plVar6->v[1] = local_68;
    plVar6->v[2] = local_64;
  }
  local_7c.v[0] = tAB->row[3].v.v[0] - fVar1;
  local_7c.v[1] = tAB->row[3].v.v[1] - fVar2;
  local_7c.v[2] = tAB->row[3].v.v[2] - fVar3;
  tIn_2 = -1.0;
  tOut_2 = 1.0;
  MVar7 = McdVanillaSegmentPlaneIntersect(&tIn_2,&tOut_2,&local_7c,&d2,local_b4);
  if (MVar7 != 0) {
    plVar6 = *outList;
    *outList = plVar6 + 1;
    plVar6->v[0] = tIn_2 * d2.v[0] + local_7c.v[0];
    plVar6->v[1] = tIn_2 * d2.v[1] + local_7c.v[1];
    plVar6->v[2] = tIn_2 * d2.v[2] + local_7c.v[2];
    plVar6 = *outList;
    *outList = plVar6 + 1;
    local_5c.v[0] = tOut_2 * d2.v[0];
    local_5c.v[1] = tOut_2 * d2.v[1];
    local_5c.v[2] = tOut_2 * d2.v[2];
    local_6c = local_5c.v[0] + local_7c.v[0];
    local_68 = local_5c.v[1] + local_7c.v[1];
    local_64 = local_5c.v[2] + local_7c.v[2];
    plVar6->v[0] = local_6c;
    plVar6->v[1] = local_68;
    plVar6->v[2] = local_64;
  }
  local_7c.v[0] = tAB->row[3].v.v[0] - d1.v[0];
  local_7c.v[1] = tAB->row[3].v.v[1] - d1.v[1];
  local_7c.v[2] = tAB->row[3].v.v[2] - d1.v[2];
  tIn_3 = -1.0;
  tOut_3 = 1.0;
  MVar7 = McdVanillaSegmentPlaneIntersect(&tIn_3,&tOut_3,&local_7c,&d2,local_b4);
  if (MVar7 != 0) {
    plVar6 = *outList;
    *outList = plVar6 + 1;
    plVar6->v[0] = tIn_3 * d2.v[0] + local_7c.v[0];
    plVar6->v[1] = tIn_3 * d2.v[1] + local_7c.v[1];
    plVar6->v[2] = tIn_3 * d2.v[2] + local_7c.v[2];
    plVar6 = *outList;
    *outList = plVar6 + 1;
    local_5c.v[0] = tOut_3 * d2.v[0];
    local_5c.v[1] = tOut_3 * d2.v[1];
    local_5c.v[2] = tOut_3 * d2.v[2];
    local_6c = local_5c.v[0] + local_7c.v[0];
    local_68 = local_5c.v[1] + local_7c.v[1];
    local_64 = local_5c.v[2] + local_7c.v[2];
    plVar6->v[0] = local_6c;
    plVar6->v[1] = local_68;
    plVar6->v[2] = local_64;
  }
  local_7c.v[0] = d2.v[0] + tAB->row[3].v.v[0];
  local_7c.v[1] = d2.v[1] + tAB->row[3].v.v[1];
  local_7c.v[2] = d2.v[2] + tAB->row[3].v.v[2];
  tIn_4 = -1.0;
  tOut_4 = 1.0;
  MVar7 = McdVanillaSegmentPlaneIntersect(&tIn_4,&tOut_4,&local_7c,&d1,local_b0);
  if (MVar7 != 0) {
    if (-1.0 < tIn_4) {
      plVar6 = *outList;
      *outList = plVar6 + 1;
      local_5c.v[0] = tIn_4 * d1.v[0];
      local_5c.v[1] = tIn_4 * d1.v[1];
      local_5c.v[2] = tIn_4 * d1.v[2];
      local_6c = local_5c.v[0] + local_7c.v[0];
      local_68 = local_5c.v[1] + local_7c.v[1];
      local_64 = local_5c.v[2] + local_7c.v[2];
      plVar6->v[0] = local_6c;
      plVar6->v[1] = local_68;
      plVar6->v[2] = local_64;
    }
    if (tOut_4 < 1.0) {
      plVar6 = *outList;
      *outList = plVar6 + 1;
      local_5c.v[0] = tOut_4 * d1.v[0];
      local_5c.v[1] = tOut_4 * d1.v[1];
      local_5c.v[2] = tOut_4 * d1.v[2];
      local_6c = local_5c.v[0] + local_7c.v[0];
      local_68 = local_5c.v[1] + local_7c.v[1];
      local_64 = local_5c.v[2] + local_7c.v[2];
      plVar6->v[0] = local_6c;
      plVar6->v[1] = local_68;
      plVar6->v[2] = local_64;
    }
  }
  local_7c.v[0] = tAB->row[3].v.v[0] - d2.v[0];
  local_7c.v[1] = tAB->row[3].v.v[1] - d2.v[1];
  local_7c.v[2] = tAB->row[3].v.v[2] - d2.v[2];
  tIn_5 = -1.0;
  tOut_5 = 1.0;
  MVar7 = McdVanillaSegmentPlaneIntersect(&tIn_5,&tOut_5,&local_7c,&d1,local_b0);
  if (MVar7 != 0) {
    if (-1.0 < tIn_5) {
      plVar6 = *outList;
      *outList = plVar6 + 1;
      plVar6->v[0] = tIn_5 * d1.v[0] + local_7c.v[0];
      plVar6->v[1] = tIn_5 * d1.v[1] + local_7c.v[1];
      plVar6->v[2] = tIn_5 * d1.v[2] + local_7c.v[2];
    }
    if (tOut_5 < 1.0) {
      plVar6 = *outList;
      *outList = plVar6 + 1;
      plVar6->v[0] = tOut_5 * d1.v[0] + local_7c.v[0];
      plVar6->v[1] = tOut_5 * d1.v[1] + local_7c.v[1];
      plVar6->v[2] = tOut_5 * d1.v[2] + local_7c.v[2];
    }
  }
  plVar6 = *outList;
  if (plVar6 != plVar5) {
    return;
  }
                    /* Unresolved local var: MeReal n2@[DW_OP_reg12(ST1)]
                       Unresolved local var: lsVec3 * v0@[???] */
  fVar1 = cylAxis->v[2];
  *outList = plVar6 + 1;
  fVar1 = fVar1 * fVar1;
  fVar2 = tAB->row[3].v.v[0];
  plVar6->v[0] = fVar2;
  fVar3 = tAB->row[3].v.v[1];
  plVar6->v[1] = fVar3;
  fVar4 = tAB->row[3].v.v[2];
  plVar6->v[2] = fVar4;
  if (fVar1 <= 1.0) {
    if (fVar1 < 1e-10) goto LAB_00010d56;
    fVar1 = plVar6->v[1];
    fVar3 = plVar6->v[0] - d1.v[0];
    fVar2 = plVar6->v[2];
    plVar6->v[0] = fVar3;
    fVar1 = fVar1 - d1.v[1];
    plVar6->v[1] = fVar1;
    fVar2 = fVar2 - d1.v[2];
    plVar6->v[2] = fVar2;
    if (cylAxis->v[2] < 0.0) {
      plVar6->v[0] = fVar3 + d2.v[0];
      plVar6->v[1] = fVar1 + d2.v[1];
      plVar6->v[2] = fVar2 + d2.v[2];
      return;
    }
  }
  else if (cylAxis->v[2] < 0.0) {
    plVar6->v[0] = fVar2 + d2.v[0];
    plVar6->v[1] = fVar3 + d2.v[1];
    plVar6->v[2] = fVar4 + d2.v[2];
    return;
  }
  plVar6->v[0] = plVar6->v[0] - d2.v[0];
  plVar6->v[1] = plVar6->v[1] - d2.v[1];
  inR = d2.v[2];
LAB_00010d56:
  plVar6->v[2] = plVar6->v[2] - inR;
  return;
}


/* ==== McdVanillaSegmentPlaneIntersect ==== */

MeBool McdVanillaSegmentPlaneIntersect
                 (MeReal *tInMax,MeReal *tOutMin,lsVec3 *orig,lsVec3 *disp,MeReal invDisp)

{
  float fVar1;
  float fVar2;
  float fVar3;
  
                    /* Unresolved local var: MeReal den@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal t@[DW_OP_reg11(ST0)] */
  fVar3 = -orig->v[2] * invDisp;
  if (disp->v[2] <= 0.0) {
    if (disp->v[2] < 0.0) {
      fVar2 = *tInMax;
      if (fVar3 <= fVar2) {
        fVar1 = *tOutMin;
      }
      else {
        *tInMax = fVar3;
        fVar1 = *tOutMin;
        fVar2 = fVar3;
      }
      goto LAB_00010e51;
    }
    if (0.0 < orig->v[2]) {
      return 0;
    }
    fVar1 = *tOutMin;
  }
  else {
    fVar1 = *tOutMin;
    if (fVar3 < fVar1) {
      *tOutMin = fVar3;
      fVar1 = fVar3;
    }
  }
  fVar2 = *tInMax;
LAB_00010e51:
  return (uint)(fVar2 <= fVar1);
}


