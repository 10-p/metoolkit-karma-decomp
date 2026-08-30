/* ==== McdBoxPlaneIntersect ==== */

MeBool McdBoxPlaneIntersect(McdModelPair *p,McdIntersectResult *result)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float *pfVar4;
  float *pfVar5;
  void *pvVar6;
  lsVec3 *inRBox;
  MeBool MVar7;
  lsVec3 *plVar8;
  byte bVar9;
  ushort uVar10;
  McdContact *pMVar11;
  float fVar12;
  float fVar13;
  MeReal eps;
  MeI16 dims;
  lsVec3 *verts;
  MeReal separation;
  lsVec3 diff;
  lsTransform tBA;
  lsVec3 footprint [24];
  
                    /* Unresolved local var: lsTransform * tA@[DW_OP_reg3(EBX)]
                       Unresolved local var: lsTransform * tB@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdGeometry * geometry1@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdFramework * fwk@[???]
                       Unresolved local var: lsVec3 * rA@[DW_OP_reg1(ECX)]
                       Unresolved local var: lsVec3 * v@[DW_OP_reg1(ECX)]
                       Unresolved local var: McdContact * c@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal scaledSep@[DW_OP_reg14(ST3)] */
  pfVar4 = McdModelGetTransformPtr(p->model1);
  pfVar5 = McdModelGetTransformPtr(p->model2);
  pvVar6 = McdModelGetGeometry(p->model1);
  fVar12 = McdModelGetContactTolerance(p->model1);
  fVar13 = McdModelGetContactTolerance(p->model2);
  plVar8 = footprint;
  inRBox = McdBoxGetRadii(pvVar6);
  tBA.row[0].v.v[0] = pfVar5[2] * pfVar4[2] + pfVar5[1] * pfVar4[1] + *pfVar5 * *pfVar4;
  tBA.row[0].v.v[1] = pfVar5[6] * pfVar4[2] + pfVar5[5] * pfVar4[1] + pfVar5[4] * *pfVar4;
  tBA.row[0].v.v[2] = pfVar5[10] * pfVar4[2] + pfVar5[9] * pfVar4[1] + pfVar5[8] * *pfVar4;
                    /* Unresolved local var: MeReal separation@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeI16 dimA@[???] */
  diff.v[0] = ABS(tBA.row[0].v.v[2]);
  tBA.row[1].v.v[0] = pfVar5[2] * pfVar4[6] + pfVar5[1] * pfVar4[5] + *pfVar5 * pfVar4[4];
  tBA.row[1].v.v[1] = pfVar5[6] * pfVar4[6] + pfVar5[5] * pfVar4[5] + pfVar5[4] * pfVar4[4];
  tBA.row[1].v.v[2] = pfVar5[10] * pfVar4[6] + pfVar5[9] * pfVar4[5] + pfVar5[8] * pfVar4[4];
  diff.v[1] = ABS(tBA.row[1].v.v[2]);
  tBA.row[2].v.v[0] = pfVar5[2] * pfVar4[10] + pfVar5[1] * pfVar4[9] + *pfVar5 * pfVar4[8];
  tBA.row[2].v.v[1] = pfVar5[6] * pfVar4[10] + pfVar5[5] * pfVar4[9] + pfVar5[4] * pfVar4[8];
  tBA.row[2].v.v[2] = pfVar5[10] * pfVar4[10] + pfVar5[9] * pfVar4[9] + pfVar5[8] * pfVar4[8];
  fVar2 = pfVar4[0xd] - pfVar5[0xd];
  fVar1 = pfVar4[0xe] - pfVar5[0xe];
  fVar3 = pfVar4[0xc] - pfVar5[0xc];
  diff.v[2] = ABS(tBA.row[2].v.v[2]);
  bVar9 = 0;
  tBA.row[3].v.v[0] = fVar1 * pfVar5[2] + fVar3 * *pfVar5 + fVar2 * pfVar5[1];
  tBA.row[3].v.v[1] = fVar1 * pfVar5[6] + fVar3 * pfVar5[4] + fVar2 * pfVar5[5];
  tBA.row[3].v.v[2] = fVar3 * pfVar5[8] + fVar2 * pfVar5[9] + fVar1 * pfVar5[10];
  fVar1 = tBA.row[3].v.v[2] -
          (diff.v[0] * inRBox->v[0] + diff.v[1] * inRBox->v[1] + diff.v[2] * inRBox->v[2]);
  verts = plVar8;
  if (fVar1 <= fVar13 + fVar12) {
    uVar10 = (ushort)(diff.v[0] < 0.0001);
    if (diff.v[1] < 0.0001) {
      uVar10 = uVar10 + 1;
    }
    if (diff.v[2] < 0.0001) {
      uVar10 = uVar10 + 1;
    }
    dims = uVar10 | 0x200;
    separation = fVar1;
    McdVanillaBoxPlaneIntersect(&verts,&tBA,inRBox);
    bVar9 = 1;
  }
  result->touch = (uint)bVar9;
  if (bVar9 == 0) {
    result->contactCount = 0;
    MVar7 = 0;
  }
  else {
    result->normal[0] = pfVar5[8];
    result->normal[1] = pfVar5[9];
    result->normal[2] = pfVar5[10];
    pMVar11 = result->contacts;
    result->contactCount = 0;
    if ((plVar8 != verts) && (0 < result->contactMaxCount)) {
      do {
                    /* Unresolved local var: MeReal s@[DW_OP_reg13(ST2)] */
        fVar1 = plVar8->v[2];
        if (fVar1 < separation * 0.01) {
          pMVar11->position[0] =
               fVar1 * pfVar5[8] + pfVar5[4] * plVar8->v[1] + *pfVar5 * plVar8->v[0] + pfVar5[0xc];
          pMVar11->position[1] =
               pfVar5[9] * plVar8->v[2] + pfVar5[5] * plVar8->v[1] + pfVar5[1] * plVar8->v[0] +
               pfVar5[0xd];
          pMVar11->position[2] =
               pfVar5[10] * plVar8->v[2] + pfVar5[6] * plVar8->v[1] + pfVar5[2] * plVar8->v[0] +
               pfVar5[0xe];
          pMVar11->separation = fVar1;
          pMVar11->dims = dims;
          pMVar11->normal[0] = result->normal[0];
          pMVar11->normal[1] = result->normal[1];
          pMVar11->normal[2] = result->normal[2];
          result->contactCount = result->contactCount + 1;
          pMVar11 = pMVar11 + 1;
        }
        plVar8 = plVar8 + 1;
      } while ((plVar8 != verts) && (result->contactCount < result->contactMaxCount));
    }
    MVar7 = 1;
  }
  return MVar7;
}


/* ==== McdBoxPlaneSafeTime ==== */

int McdBoxPlaneSafeTime(McdModelPair *p,MeReal maxTime,McdSafeTimeResult *result)

{
  bool bVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  void *pvVar5;
  void *pvVar6;
  float *pfVar7;
  float *pfVar8;
  void *pvVar9;
  float fVar10;
  lsVec3 *plVar11;
  lsVec3 *plVar12;
  MeMatrix4Ptr planeTM;
  lsVec3 closestPoint;
  lsVec3 planeN;
  lsVec3 relV;
  
                    /* Unresolved local var: McdBoxID boxG@[???]
                       Unresolved local var: McdPlaneID planeG@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal relVnorm@[???]
                       Unresolved local var: MeReal dist@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[???] */
  result->pair = p;
  McdModelGetGeometry(p->model1);
  pvVar5 = McdModelGetGeometry(p->model2);
  pvVar6 = McdModelGetTransformPtr(p->model2);
  pfVar7 = McdModelGetLinearVelocityPtr(p->model1);
  pfVar8 = McdModelGetLinearVelocityPtr(p->model2);
  fVar2 = *pfVar7 - *pfVar8;
  fVar4 = pfVar7[1] - pfVar8[1];
  fVar3 = pfVar7[2] - pfVar8[2];
  bVar1 = 0.0001 <= SQRT(fVar2 * fVar2 + fVar4 * fVar4 + fVar3 * fVar3) * maxTime;
  if (bVar1) {
    plVar11 = &planeN;
    pvVar9 = McdModelGetTransformPtr(p->model2);
    McdPlaneGetNormal(pvVar5,pvVar9,plVar11);
    plVar11 = &planeN;
    planeN.v[0] = planeN.v[0] * -1.0;
    planeN.v[1] = planeN.v[1] * -1.0;
    planeN.v[2] = planeN.v[2] * -1.0;
    plVar12 = &closestPoint;
    pvVar9 = McdModelGetGeometryInstance(p->model1);
    McdBoxMaximumPoint(pvVar9,plVar11,plVar12);
    fVar10 = McdPlaneGetDistanceToPoint(pvVar5,pvVar6,&closestPoint);
    result->time = fVar10 / (fVar3 * planeN.v[2] + fVar4 * planeN.v[1] + fVar2 * planeN.v[0]);
  }
  else {
    result->time = maxTime;
  }
  return (uint)bVar1;
}


/* ==== McdBoxPlaneRegisterInteraction ==== */

MeBool McdBoxPlaneRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdBoxPlaneIntersect;
  interactions.safetimeFn = McdBoxPlaneSafeTime;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,2,3,&interactions);
  return 1;
}


/* ==== McdVanillaBoxPlaneIntersect ==== */

void McdVanillaBoxPlaneIntersect(lsVec3 **outList,lsTransform *tAB,lsVec3 *inRBox)

{
  float fVar1;
  float fVar2;
  float fVar3;
  lsVec3 *plVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  MeBool MVar8;
  MeReal local_110;
  MeReal local_108;
  MeReal local_104;
  MeReal tOut_11;
  MeReal tIn_11;
  MeReal tOut_10;
  MeReal tIn_10;
  MeReal tOut_9;
  MeReal tIn_9;
  MeReal tOut_8;
  MeReal tIn_8;
  MeReal tOut_7;
  MeReal tIn_7;
  MeReal tOut_6;
  MeReal tIn_6;
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
  lsVec3 anon_var_0_5;
  lsVec3 anon_var_0_6;
  lsVec3 local_7c;
  lsVec3 anon_var_0_4;
  lsVec3 anon_var_0_3;
  lsVec3 anon_var_0_2;
  lsVec3 anon_var_0_1;
  lsVec3 anon_var_0;
  
                    /* Unresolved local var: lsVec3 * t@[???]
                       Unresolved local var: lsVec3 * d0@[???]
                       Unresolved local var: lsVec3 * d1@[???]
                       Unresolved local var: lsVec3 * d2@[???]
                       Unresolved local var: MeReal invD0Z@[???]
                       Unresolved local var: MeReal invD1Z@[???]
                       Unresolved local var: MeReal invD2Z@[???] */
  fVar1 = inRBox->v[0];
  anon_var_0.v[0] = fVar1 * tAB->row[0].v.v[0];
  anon_var_0.v[1] = fVar1 * tAB->row[0].v.v[1];
  anon_var_0.v[2] = fVar1 * tAB->row[0].v.v[2];
  fVar1 = inRBox->v[1];
  anon_var_0_1.v[0] = fVar1 * tAB->row[1].v.v[0];
  anon_var_0_1.v[1] = fVar1 * tAB->row[1].v.v[1];
  anon_var_0_1.v[2] = fVar1 * tAB->row[1].v.v[2];
  fVar1 = inRBox->v[2];
  anon_var_0_2.v[2] = fVar1 * tAB->row[2].v.v[2];
  anon_var_0_2.v[0] = fVar1 * tAB->row[2].v.v[0];
  anon_var_0_2.v[1] = fVar1 * tAB->row[2].v.v[1];
  local_104 = 0.0;
  if (anon_var_0.v[2] != 0.0) {
    local_104 = 1.0 / anon_var_0.v[2];
  }
  local_108 = 0.0;
  if (anon_var_0_1.v[2] != 0.0) {
    local_108 = 1.0 / anon_var_0_1.v[2];
  }
  local_110 = 0.0;
  if (anon_var_0_2.v[2] != 0.0) {
    local_110 = 1.0 / anon_var_0_2.v[2];
  }
                    /* Unresolved local var: lsVec3 * t_p_d1@[???]
                       Unresolved local var: lsVec3 * t_m_d1@[???] */
  fVar1 = tAB->row[3].v.v[0];
  fVar6 = anon_var_0_1.v[0] + fVar1;
  fVar2 = tAB->row[3].v.v[2];
  fVar3 = tAB->row[3].v.v[1];
  fVar7 = anon_var_0_1.v[1] + fVar3;
  fVar5 = fVar2 + anon_var_0_1.v[2];
  anon_var_0_4.v[2] = fVar2 - anon_var_0_1.v[2];
  anon_var_0_4.v[0] = fVar1 - anon_var_0_1.v[0];
  anon_var_0_4.v[1] = fVar3 - anon_var_0_1.v[1];
  local_7c.v[0] = fVar6 + anon_var_0_2.v[0];
  local_7c.v[1] = fVar7 + anon_var_0_2.v[1];
  tIn = -1.0;
  local_7c.v[2] = fVar5 + anon_var_0_2.v[2];
  tOut = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn,&tOut,&local_7c,&anon_var_0,local_104);
  if (MVar8 != 0) {
    if (-1.0 < tIn) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      plVar4->v[0] = tIn * anon_var_0.v[0] + local_7c.v[0];
      plVar4->v[1] = tIn * anon_var_0.v[1] + local_7c.v[1];
      plVar4->v[2] = tIn * anon_var_0.v[2] + local_7c.v[2];
    }
    if (tOut < 1.0) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      plVar4->v[0] = tOut * anon_var_0.v[0] + local_7c.v[0];
      plVar4->v[1] = tOut * anon_var_0.v[1] + local_7c.v[1];
      plVar4->v[2] = tOut * anon_var_0.v[2] + local_7c.v[2];
    }
  }
  tIn_1 = -1.0;
  anon_var_0_5.v[0] = fVar6 - anon_var_0_2.v[0];
  anon_var_0_5.v[1] = fVar7 - anon_var_0_2.v[1];
  anon_var_0_5.v[2] = fVar5 - anon_var_0_2.v[2];
  tOut_1 = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn_1,&tOut_1,&anon_var_0_5,&anon_var_0,local_104);
  if (MVar8 != 0) {
    if (-1.0 < tIn_1) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      local_7c.v[0] = tIn_1 * anon_var_0.v[0];
      local_7c.v[1] = tIn_1 * anon_var_0.v[1];
      local_7c.v[2] = tIn_1 * anon_var_0.v[2];
      plVar4->v[0] = local_7c.v[0] + anon_var_0_5.v[0];
      plVar4->v[1] = local_7c.v[1] + anon_var_0_5.v[1];
      plVar4->v[2] = local_7c.v[2] + anon_var_0_5.v[2];
    }
    if (tOut_1 < 1.0) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      local_7c.v[0] = tOut_1 * anon_var_0.v[0];
      local_7c.v[1] = tOut_1 * anon_var_0.v[1];
      local_7c.v[2] = tOut_1 * anon_var_0.v[2];
      plVar4->v[0] = local_7c.v[0] + anon_var_0_5.v[0];
      plVar4->v[1] = local_7c.v[1] + anon_var_0_5.v[1];
      plVar4->v[2] = local_7c.v[2] + anon_var_0_5.v[2];
    }
  }
  anon_var_0_5.v[0] = anon_var_0_4.v[0] - anon_var_0_2.v[0];
  anon_var_0_5.v[1] = anon_var_0_4.v[1] - anon_var_0_2.v[1];
  anon_var_0_5.v[2] = anon_var_0_4.v[2] - anon_var_0_2.v[2];
  tIn_2 = -1.0;
  tOut_2 = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn_2,&tOut_2,&anon_var_0_5,&anon_var_0,local_104);
  if (MVar8 != 0) {
    if (-1.0 < tIn_2) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      local_7c.v[0] = tIn_2 * anon_var_0.v[0];
      local_7c.v[1] = tIn_2 * anon_var_0.v[1];
      local_7c.v[2] = tIn_2 * anon_var_0.v[2];
      plVar4->v[0] = local_7c.v[0] + anon_var_0_5.v[0];
      plVar4->v[1] = local_7c.v[1] + anon_var_0_5.v[1];
      plVar4->v[2] = local_7c.v[2] + anon_var_0_5.v[2];
    }
    if (tOut_2 < 1.0) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      local_7c.v[0] = tOut_2 * anon_var_0.v[0];
      local_7c.v[1] = tOut_2 * anon_var_0.v[1];
      local_7c.v[2] = tOut_2 * anon_var_0.v[2];
      plVar4->v[0] = local_7c.v[0] + anon_var_0_5.v[0];
      plVar4->v[1] = local_7c.v[1] + anon_var_0_5.v[1];
      plVar4->v[2] = local_7c.v[2] + anon_var_0_5.v[2];
    }
  }
  anon_var_0_5.v[0] = anon_var_0_2.v[0] + anon_var_0_4.v[0];
  anon_var_0_5.v[1] = anon_var_0_2.v[1] + anon_var_0_4.v[1];
  anon_var_0_5.v[2] = anon_var_0_2.v[2] + anon_var_0_4.v[2];
  tIn_3 = -1.0;
  tOut_3 = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn_3,&tOut_3,&anon_var_0_5,&anon_var_0,local_104);
  if (MVar8 != 0) {
    if (-1.0 < tIn_3) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      plVar4->v[0] = tIn_3 * anon_var_0.v[0] + anon_var_0_5.v[0];
      plVar4->v[1] = tIn_3 * anon_var_0.v[1] + anon_var_0_5.v[1];
      plVar4->v[2] = tIn_3 * anon_var_0.v[2] + anon_var_0_5.v[2];
    }
    if (tOut_3 < 1.0) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      plVar4->v[0] = tOut_3 * anon_var_0.v[0] + anon_var_0_5.v[0];
      plVar4->v[1] = tOut_3 * anon_var_0.v[1] + anon_var_0_5.v[1];
      plVar4->v[2] = tOut_3 * anon_var_0.v[2] + anon_var_0_5.v[2];
    }
  }
                    /* Unresolved local var: lsVec3 * t_p_d0@[???]
                       Unresolved local var: lsVec3 * t_m_d0@[???] */
  fVar1 = tAB->row[3].v.v[0];
  fVar3 = fVar1 - anon_var_0.v[0];
  anon_var_0_5.v[0] = fVar1 + anon_var_0.v[0];
  fVar1 = tAB->row[3].v.v[1];
  fVar2 = tAB->row[3].v.v[2];
  anon_var_0_5.v[1] = fVar1 + anon_var_0.v[1];
  anon_var_0_5.v[2] = anon_var_0.v[2] + fVar2;
  fVar1 = fVar1 - anon_var_0.v[1];
  fVar2 = fVar2 - anon_var_0.v[2];
  local_7c.v[0] = anon_var_0_5.v[0] + anon_var_0_2.v[0];
  local_7c.v[1] = anon_var_0_5.v[1] + anon_var_0_2.v[1];
  local_7c.v[2] = anon_var_0_5.v[2] + anon_var_0_2.v[2];
  tIn_4 = -1.0;
  tOut_4 = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn_4,&tOut_4,&local_7c,&anon_var_0_1,local_108);
  if (MVar8 != 0) {
    if (-1.0 < tIn_4) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      anon_var_0_4.v[0] = tIn_4 * anon_var_0_1.v[0] + local_7c.v[0];
      anon_var_0_4.v[1] = tIn_4 * anon_var_0_1.v[1] + local_7c.v[1];
      anon_var_0_4.v[2] = tIn_4 * anon_var_0_1.v[2] + local_7c.v[2];
      plVar4->v[0] = anon_var_0_4.v[0];
      plVar4->v[1] = anon_var_0_4.v[1];
      plVar4->v[2] = anon_var_0_4.v[2];
    }
    if (tOut_4 < 1.0) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      anon_var_0_4.v[0] = tOut_4 * anon_var_0_1.v[0] + local_7c.v[0];
      anon_var_0_4.v[1] = tOut_4 * anon_var_0_1.v[1] + local_7c.v[1];
      anon_var_0_4.v[2] = tOut_4 * anon_var_0_1.v[2] + local_7c.v[2];
      plVar4->v[0] = anon_var_0_4.v[0];
      plVar4->v[1] = anon_var_0_4.v[1];
      plVar4->v[2] = anon_var_0_4.v[2];
    }
  }
  local_7c.v[0] = anon_var_0_2.v[0] + fVar3;
  local_7c.v[1] = anon_var_0_2.v[1] + fVar1;
  local_7c.v[2] = anon_var_0_2.v[2] + fVar2;
  tIn_5 = -1.0;
  tOut_5 = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn_5,&tOut_5,&local_7c,&anon_var_0_1,local_108);
  if (MVar8 != 0) {
    if (-1.0 < tIn_5) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      anon_var_0_4.v[0] = tIn_5 * anon_var_0_1.v[0] + local_7c.v[0];
      anon_var_0_4.v[1] = tIn_5 * anon_var_0_1.v[1] + local_7c.v[1];
      anon_var_0_4.v[2] = tIn_5 * anon_var_0_1.v[2] + local_7c.v[2];
      plVar4->v[0] = anon_var_0_4.v[0];
      plVar4->v[1] = anon_var_0_4.v[1];
      plVar4->v[2] = anon_var_0_4.v[2];
    }
    if (tOut_5 < 1.0) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      anon_var_0_4.v[0] = tOut_5 * anon_var_0_1.v[0] + local_7c.v[0];
      anon_var_0_4.v[1] = tOut_5 * anon_var_0_1.v[1] + local_7c.v[1];
      anon_var_0_4.v[2] = tOut_5 * anon_var_0_1.v[2] + local_7c.v[2];
      plVar4->v[0] = anon_var_0_4.v[0];
      plVar4->v[1] = anon_var_0_4.v[1];
      plVar4->v[2] = anon_var_0_4.v[2];
    }
  }
  local_7c.v[0] = fVar3 - anon_var_0_2.v[0];
  local_7c.v[1] = fVar1 - anon_var_0_2.v[1];
  local_7c.v[2] = fVar2 - anon_var_0_2.v[2];
  tIn_6 = -1.0;
  tOut_6 = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn_6,&tOut_6,&local_7c,&anon_var_0_1,local_108);
  if (MVar8 != 0) {
    if (-1.0 < tIn_6) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      anon_var_0_4.v[0] = tIn_6 * anon_var_0_1.v[0] + local_7c.v[0];
      anon_var_0_4.v[1] = tIn_6 * anon_var_0_1.v[1] + local_7c.v[1];
      anon_var_0_4.v[2] = tIn_6 * anon_var_0_1.v[2] + local_7c.v[2];
      plVar4->v[0] = anon_var_0_4.v[0];
      plVar4->v[1] = anon_var_0_4.v[1];
      plVar4->v[2] = anon_var_0_4.v[2];
    }
    if (tOut_6 < 1.0) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      anon_var_0_4.v[0] = tOut_6 * anon_var_0_1.v[0] + local_7c.v[0];
      anon_var_0_4.v[1] = tOut_6 * anon_var_0_1.v[1] + local_7c.v[1];
      anon_var_0_4.v[2] = tOut_6 * anon_var_0_1.v[2] + local_7c.v[2];
      plVar4->v[0] = anon_var_0_4.v[0];
      plVar4->v[1] = anon_var_0_4.v[1];
      plVar4->v[2] = anon_var_0_4.v[2];
    }
  }
  tIn_7 = -1.0;
  local_7c.v[0] = anon_var_0_5.v[0] - anon_var_0_2.v[0];
  local_7c.v[1] = anon_var_0_5.v[1] - anon_var_0_2.v[1];
  local_7c.v[2] = anon_var_0_5.v[2] - anon_var_0_2.v[2];
  tOut_7 = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn_7,&tOut_7,&local_7c,&anon_var_0_1,local_108);
  if (MVar8 != 0) {
    if (-1.0 < tIn_7) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      anon_var_0_4.v[0] = tIn_7 * anon_var_0_1.v[0] + local_7c.v[0];
      anon_var_0_4.v[1] = tIn_7 * anon_var_0_1.v[1] + local_7c.v[1];
      anon_var_0_4.v[2] = tIn_7 * anon_var_0_1.v[2] + local_7c.v[2];
      plVar4->v[0] = anon_var_0_4.v[0];
      plVar4->v[1] = anon_var_0_4.v[1];
      plVar4->v[2] = anon_var_0_4.v[2];
    }
    if (tOut_7 < 1.0) {
      plVar4 = *outList;
      *outList = plVar4 + 1;
      anon_var_0_4.v[0] = tOut_7 * anon_var_0_1.v[0] + local_7c.v[0];
      anon_var_0_4.v[1] = tOut_7 * anon_var_0_1.v[1] + local_7c.v[1];
      anon_var_0_4.v[2] = tOut_7 * anon_var_0_1.v[2] + local_7c.v[2];
      plVar4->v[0] = anon_var_0_4.v[0];
      plVar4->v[1] = anon_var_0_4.v[1];
      plVar4->v[2] = anon_var_0_4.v[2];
    }
  }
  local_7c.v[0] = anon_var_0_1.v[0] + anon_var_0_5.v[0];
  local_7c.v[1] = anon_var_0_1.v[1] + anon_var_0_5.v[1];
  local_7c.v[2] = anon_var_0_1.v[2] + anon_var_0_5.v[2];
  tIn_8 = -1.0;
  tOut_8 = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn_8,&tOut_8,&local_7c,&anon_var_0_2,local_110);
  if (MVar8 != 0) {
    plVar4 = *outList;
    *outList = plVar4 + 1;
    plVar4->v[0] = tIn_8 * anon_var_0_2.v[0] + local_7c.v[0];
    plVar4->v[1] = tIn_8 * anon_var_0_2.v[1] + local_7c.v[1];
    plVar4->v[2] = tIn_8 * anon_var_0_2.v[2] + local_7c.v[2];
    plVar4 = *outList;
    *outList = plVar4 + 1;
    anon_var_0_4.v[0] = tOut_8 * anon_var_0_2.v[0] + local_7c.v[0];
    anon_var_0_4.v[1] = tOut_8 * anon_var_0_2.v[1] + local_7c.v[1];
    anon_var_0_4.v[2] = tOut_8 * anon_var_0_2.v[2] + local_7c.v[2];
    plVar4->v[0] = anon_var_0_4.v[0];
    plVar4->v[1] = anon_var_0_4.v[1];
    plVar4->v[2] = anon_var_0_4.v[2];
  }
  local_7c.v[0] = anon_var_0_5.v[0] - anon_var_0_1.v[0];
  local_7c.v[1] = anon_var_0_5.v[1] - anon_var_0_1.v[1];
  local_7c.v[2] = anon_var_0_5.v[2] - anon_var_0_1.v[2];
  tIn_9 = -1.0;
  tOut_9 = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn_9,&tOut_9,&local_7c,&anon_var_0_2,local_110);
  if (MVar8 != 0) {
    plVar4 = *outList;
    *outList = plVar4 + 1;
    plVar4->v[0] = tIn_9 * anon_var_0_2.v[0] + local_7c.v[0];
    plVar4->v[1] = tIn_9 * anon_var_0_2.v[1] + local_7c.v[1];
    plVar4->v[2] = tIn_9 * anon_var_0_2.v[2] + local_7c.v[2];
    plVar4 = *outList;
    *outList = plVar4 + 1;
    anon_var_0_4.v[0] = tOut_9 * anon_var_0_2.v[0] + local_7c.v[0];
    anon_var_0_4.v[1] = tOut_9 * anon_var_0_2.v[1] + local_7c.v[1];
    anon_var_0_4.v[2] = tOut_9 * anon_var_0_2.v[2] + local_7c.v[2];
    plVar4->v[0] = anon_var_0_4.v[0];
    plVar4->v[1] = anon_var_0_4.v[1];
    plVar4->v[2] = anon_var_0_4.v[2];
  }
  local_7c.v[0] = fVar3 - anon_var_0_1.v[0];
  local_7c.v[1] = fVar1 - anon_var_0_1.v[1];
  local_7c.v[2] = fVar2 - anon_var_0_1.v[2];
  tIn_10 = -1.0;
  tOut_10 = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn_10,&tOut_10,&local_7c,&anon_var_0_2,local_110);
  if (MVar8 != 0) {
    plVar4 = *outList;
    *outList = plVar4 + 1;
    plVar4->v[0] = tIn_10 * anon_var_0_2.v[0] + local_7c.v[0];
    plVar4->v[1] = tIn_10 * anon_var_0_2.v[1] + local_7c.v[1];
    plVar4->v[2] = tIn_10 * anon_var_0_2.v[2] + local_7c.v[2];
    plVar4 = *outList;
    *outList = plVar4 + 1;
    anon_var_0_4.v[0] = tOut_10 * anon_var_0_2.v[0] + local_7c.v[0];
    anon_var_0_4.v[1] = tOut_10 * anon_var_0_2.v[1] + local_7c.v[1];
    anon_var_0_4.v[2] = tOut_10 * anon_var_0_2.v[2] + local_7c.v[2];
    plVar4->v[0] = anon_var_0_4.v[0];
    plVar4->v[1] = anon_var_0_4.v[1];
    plVar4->v[2] = anon_var_0_4.v[2];
  }
  local_7c.v[0] = anon_var_0_1.v[0] + fVar3;
  local_7c.v[1] = anon_var_0_1.v[1] + fVar1;
  local_7c.v[2] = anon_var_0_1.v[2] + fVar2;
  tIn_11 = -1.0;
  tOut_11 = 1.0;
  MVar8 = McdVanillaSegmentPlaneIntersect(&tIn_11,&tOut_11,&local_7c,&anon_var_0_2,local_110);
  if (MVar8 != 0) {
    plVar4 = *outList;
    *outList = plVar4 + 1;
    plVar4->v[0] = tIn_11 * anon_var_0_2.v[0] + local_7c.v[0];
    plVar4->v[1] = tIn_11 * anon_var_0_2.v[1] + local_7c.v[1];
    plVar4->v[2] = tIn_11 * anon_var_0_2.v[2] + local_7c.v[2];
    plVar4 = *outList;
    *outList = plVar4 + 1;
    plVar4->v[0] = tOut_11 * anon_var_0_2.v[0] + local_7c.v[0];
    plVar4->v[1] = tOut_11 * anon_var_0_2.v[1] + local_7c.v[1];
    plVar4->v[2] = tOut_11 * anon_var_0_2.v[2] + local_7c.v[2];
  }
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
      goto LAB_00011741;
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
LAB_00011741:
  return (uint)(fVar2 <= fVar1);
}


