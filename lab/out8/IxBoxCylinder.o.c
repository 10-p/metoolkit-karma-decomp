/* ==== BoxCylIntersect ==== */

void BoxCylIntersect(lsVec3 **outList,lsTransform *tAB,lsVec3 *inRA,MeReal inRCyl,MeReal inHHCyl,
                    lsVec3 *nPara,lsVec3 *nPerp,lsVec3 *cylAxis,MeReal scale)

{
  lsTransformRow *plVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  lsVec3 *plVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float eps;
  bool bVar18;
  int iVar19;
  lsVec3 *plVar20;
  MeReal tOut_17;
  MeReal tIn_17;
  MeReal tOut_16;
  MeReal tIn_16;
  MeReal tOut_15;
  MeReal tIn_15;
  MeReal tOut_14;
  MeReal tIn_14;
  MeReal tOut_13;
  MeReal tIn_13;
  MeReal tOut_12;
  MeReal tIn_12;
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
  lsVec3 *bVerts;
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
  lsVec3 local_23c;
  float local_22c;
  float local_228;
  float local_224;
  float local_21c;
  float local_218;
  float local_214;
  lsVec3 local_20c;
  lsVec3 d2;
  lsVec3 d1;
  lsVec3 d0;
  lsVec3 invA2;
  lsVec3 invA1;
  lsVec3 a2;
  lsVec3 a1;
  lsVec3 bVertList [24];
  lsVec3 a0;
  lsTransform tBA;
  
                    /* Unresolved local var: lsVec3 * cylPos@[???]
                       Unresolved local var: lsVec3 * vert@[DW_OP_reg3(EBX)] */
  bVerts = bVertList;
  fVar2 = tAB->row[0].v.v[0];
  plVar1 = tAB->row + 3;
  fVar3 = tAB->row[1].v.v[0];
  fVar4 = tAB->row[2].v.v[0];
  fVar5 = tAB->row[0].v.v[1];
  fVar6 = tAB->row[1].v.v[1];
  fVar7 = tAB->row[2].v.v[1];
  fVar8 = tAB->row[0].v.v[2];
  fVar9 = tAB->row[1].v.v[2];
  fVar10 = tAB->row[2].v.v[2];
  fVar16 = -tAB->row[3].v.v[0];
  fVar14 = -tAB->row[3].v.v[1];
  fVar11 = -tAB->row[3].v.v[2];
  fVar13 = fVar2 * fVar16 + fVar5 * fVar14 + fVar8 * fVar11;
  fVar15 = fVar6 * fVar14 + fVar3 * fVar16 + fVar9 * fVar11;
  fVar14 = fVar16 * tAB->row[2].v.v[0] + fVar14 * tAB->row[2].v.v[1] + fVar11 * tAB->row[2].v.v[2];
  fVar11 = inRCyl * nPara->v[1];
  fVar17 = inRCyl * nPara->v[0];
  fVar16 = inRCyl * nPara->v[2];
  a1.v[0] = inRCyl * nPerp->v[0];
  a1.v[1] = inRCyl * nPerp->v[1];
  a1.v[2] = inRCyl * nPerp->v[2];
  a2.v[1] = inHHCyl * cylAxis->v[1];
  a2.v[0] = inHHCyl * cylAxis->v[0];
  a2.v[2] = inHHCyl * cylAxis->v[2];
  invA1.v[0] = 0.0;
  if (a1.v[0] != 0.0) {
    invA1.v[0] = 1.0 / a1.v[0];
  }
  invA1.v[1] = 0.0;
  if (a1.v[1] != 0.0) {
    invA1.v[1] = 1.0 / a1.v[1];
  }
  invA1.v[2] = 0.0;
  if (a1.v[2] != 0.0) {
    invA1.v[2] = 1.0 / a1.v[2];
  }
  invA2.v[0] = 0.0;
  if (a2.v[0] != 0.0) {
    invA2.v[0] = 1.0 / a2.v[0];
  }
  invA2.v[1] = 0.0;
  if (a2.v[1] != 0.0) {
    invA2.v[1] = 1.0 / a2.v[1];
  }
  invA2.v[2] = 0.0;
  if (a2.v[2] != 0.0) {
    invA2.v[2] = 1.0 / a2.v[2];
  }
  d0.v[2] = fVar16 + tAB->row[3].v.v[2];
  d0.v[0] = fVar17 + (plVar1->v).v[0];
  d0.v[1] = fVar11 + tAB->row[3].v.v[1];
  eps = scale * 1e-06;
  tIn = -1.0;
  tOut = 1.0;
  iVar19 = McdVanillaSegmentCubeIntersect(&tIn,&tOut,&d0,&a2,&invA2,inRA,eps);
  if (iVar19 != 0) {
    plVar20 = *outList;
    *outList = plVar20 + 1;
    plVar20->v[0] = tIn * a2.v[0] + d0.v[0];
    plVar20->v[1] = tIn * a2.v[1] + d0.v[1];
    plVar20->v[2] = tIn * a2.v[2] + d0.v[2];
    plVar20 = *outList;
    *outList = plVar20 + 1;
    plVar20->v[0] = tOut * a2.v[0] + d0.v[0];
    plVar20->v[1] = tOut * a2.v[1] + d0.v[1];
    plVar20->v[2] = tOut * a2.v[2] + d0.v[2];
  }
  d0.v[0] = a1.v[0] + (plVar1->v).v[0];
  d0.v[1] = a1.v[1] + tAB->row[3].v.v[1];
  d0.v[2] = a1.v[2] + tAB->row[3].v.v[2];
  tIn_1 = -1.0;
  tOut_1 = 1.0;
  iVar19 = McdVanillaSegmentCubeIntersect(&tIn_1,&tOut_1,&d0,&a2,&invA2,inRA,eps);
  if (iVar19 != 0) {
    plVar20 = *outList;
    *outList = plVar20 + 1;
    plVar20->v[0] = tIn_1 * a2.v[0] + d0.v[0];
    plVar20->v[1] = tIn_1 * a2.v[1] + d0.v[1];
    plVar20->v[2] = tIn_1 * a2.v[2] + d0.v[2];
    plVar20 = *outList;
    *outList = plVar20 + 1;
    plVar20->v[0] = tOut_1 * a2.v[0] + d0.v[0];
    plVar20->v[1] = tOut_1 * a2.v[1] + d0.v[1];
    plVar20->v[2] = tOut_1 * a2.v[2] + d0.v[2];
  }
  d0.v[0] = (plVar1->v).v[0] - fVar17;
  d0.v[1] = tAB->row[3].v.v[1] - fVar11;
  d0.v[2] = tAB->row[3].v.v[2] - fVar16;
  tIn_2 = -1.0;
  tOut_2 = 1.0;
  iVar19 = McdVanillaSegmentCubeIntersect(&tIn_2,&tOut_2,&d0,&a2,&invA2,inRA,eps);
  if (iVar19 != 0) {
    plVar20 = *outList;
    *outList = plVar20 + 1;
    plVar20->v[0] = tIn_2 * a2.v[0] + d0.v[0];
    plVar20->v[1] = tIn_2 * a2.v[1] + d0.v[1];
    plVar20->v[2] = tIn_2 * a2.v[2] + d0.v[2];
    plVar20 = *outList;
    *outList = plVar20 + 1;
    plVar20->v[0] = tOut_2 * a2.v[0] + d0.v[0];
    plVar20->v[1] = tOut_2 * a2.v[1] + d0.v[1];
    plVar20->v[2] = tOut_2 * a2.v[2] + d0.v[2];
  }
  d0.v[0] = (plVar1->v).v[0] - a1.v[0];
  d0.v[1] = tAB->row[3].v.v[1] - a1.v[1];
  d0.v[2] = tAB->row[3].v.v[2] - a1.v[2];
  tIn_3 = -1.0;
  tOut_3 = 1.0;
  iVar19 = McdVanillaSegmentCubeIntersect(&tIn_3,&tOut_3,&d0,&a2,&invA2,inRA,eps);
  if (iVar19 != 0) {
    plVar20 = *outList;
    *outList = plVar20 + 1;
    plVar20->v[0] = tIn_3 * a2.v[0] + d0.v[0];
    plVar20->v[1] = tIn_3 * a2.v[1] + d0.v[1];
    plVar20->v[2] = tIn_3 * a2.v[2] + d0.v[2];
    plVar20 = *outList;
    *outList = plVar20 + 1;
    plVar20->v[0] = tOut_3 * a2.v[0] + d0.v[0];
    plVar20->v[1] = tOut_3 * a2.v[1] + d0.v[1];
    plVar20->v[2] = tOut_3 * a2.v[2] + d0.v[2];
  }
  d0.v[0] = a2.v[0] + (plVar1->v).v[0];
  d0.v[1] = a2.v[1] + tAB->row[3].v.v[1];
  d0.v[2] = a2.v[2] + tAB->row[3].v.v[2];
  tIn_4 = -1.0;
  tOut_4 = 1.0;
  iVar19 = McdVanillaSegmentCubeIntersect(&tIn_4,&tOut_4,&d0,&a1,&invA1,inRA,eps);
  if (iVar19 != 0) {
    if ((-1.0 < tIn_4) && (tIn_4 < 1.0)) {
      plVar20 = *outList;
      *outList = plVar20 + 1;
      plVar20->v[0] = tIn_4 * a1.v[0] + d0.v[0];
      plVar20->v[1] = tIn_4 * a1.v[1] + d0.v[1];
      plVar20->v[2] = tIn_4 * a1.v[2] + d0.v[2];
    }
    if ((-1.0 < tOut_4) && (tOut_4 < 1.0)) {
      plVar20 = *outList;
      *outList = plVar20 + 1;
      plVar20->v[0] = tOut_4 * a1.v[0] + d0.v[0];
      plVar20->v[1] = tOut_4 * a1.v[1] + d0.v[1];
      plVar20->v[2] = tOut_4 * a1.v[2] + d0.v[2];
    }
  }
  d0.v[2] = tAB->row[3].v.v[2] - a2.v[2];
  d0.v[0] = (plVar1->v).v[0] - a2.v[0];
  d0.v[1] = tAB->row[3].v.v[1] - a2.v[1];
  tIn_5 = -1.0;
  tOut_5 = 1.0;
  iVar19 = McdVanillaSegmentCubeIntersect(&tIn_5,&tOut_5,&d0,&a1,&invA1,inRA,scale * 1e-06);
  if (iVar19 != 0) {
    if ((-1.0 < tIn_5) && (tIn_5 < 1.0)) {
      plVar20 = *outList;
      *outList = plVar20 + 1;
      plVar20->v[0] = tIn_5 * a1.v[0] + d0.v[0];
      plVar20->v[1] = tIn_5 * a1.v[1] + d0.v[1];
      plVar20->v[2] = tIn_5 * a1.v[2] + d0.v[2];
    }
    if ((-1.0 < tOut_5) && (tOut_5 < 1.0)) {
      plVar20 = *outList;
      *outList = plVar20 + 1;
      plVar20->v[0] = tOut_5 * a1.v[0] + d0.v[0];
      plVar20->v[1] = tOut_5 * a1.v[1] + d0.v[1];
      plVar20->v[2] = tOut_5 * a1.v[2] + d0.v[2];
    }
  }
  fVar11 = inRA->v[0];
  d0.v[0] = fVar11 * fVar2;
  d0.v[1] = fVar11 * fVar3;
  d0.v[2] = fVar11 * fVar4;
  fVar2 = inRA->v[1];
  d1.v[0] = fVar2 * fVar5;
  d1.v[1] = fVar2 * fVar6;
  d1.v[2] = fVar2 * fVar7;
  fVar2 = inRA->v[2];
  local_21c = d1.v[0] + fVar13;
  local_218 = d1.v[1] + fVar15;
  local_214 = d1.v[2] + fVar14;
  d2.v[0] = fVar2 * fVar8;
  d2.v[1] = fVar2 * fVar9;
  d2.v[2] = fVar2 * fVar10;
  local_20c.v[0] = local_21c + d2.v[0];
  local_20c.v[1] = local_218 + d2.v[1];
  local_20c.v[2] = local_214 + d2.v[2];
  tOut_6 = 1.0;
  tIn_6 = -1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_6,&tOut_6,&local_20c,&d0,inRCyl,inHHCyl,scale);
  if (bVar18) {
    if (-1.0 < tIn_6) {
      bVerts->v[0] = tIn_6 * d0.v[0] + local_20c.v[0];
      bVerts->v[1] = tIn_6 * d0.v[1] + local_20c.v[1];
      bVerts->v[2] = tIn_6 * d0.v[2] + local_20c.v[2];
      bVerts = bVerts + 1;
    }
    if (tOut_6 < 1.0) {
      bVerts->v[0] = tOut_6 * d0.v[0] + local_20c.v[0];
      bVerts->v[1] = tOut_6 * d0.v[1] + local_20c.v[1];
      bVerts->v[2] = tOut_6 * d0.v[2] + local_20c.v[2];
      bVerts = bVerts + 1;
    }
  }
  local_22c = d1.v[0] + fVar13;
  local_228 = d1.v[1] + fVar15;
  local_224 = d1.v[2] + fVar14;
  tOut_7 = 1.0;
  local_23c.v[0] = local_22c - d2.v[0];
  local_23c.v[1] = local_228 - d2.v[1];
  local_23c.v[2] = local_224 - d2.v[2];
  tIn_7 = -1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_7,&tOut_7,&local_23c,&d0,inRCyl,inHHCyl,scale);
  if (bVar18) {
    if (-1.0 < tIn_7) {
      local_20c.v[0] = tIn_7 * d0.v[0];
      local_20c.v[1] = tIn_7 * d0.v[1];
      local_20c.v[2] = tIn_7 * d0.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
    if (tOut_7 < 1.0) {
      local_20c.v[0] = tOut_7 * d0.v[0];
      local_20c.v[1] = tOut_7 * d0.v[1];
      local_20c.v[2] = tOut_7 * d0.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
  }
  local_22c = fVar13 - d1.v[0];
  local_228 = fVar15 - d1.v[1];
  local_224 = fVar14 - d1.v[2];
  tOut_8 = 1.0;
  local_23c.v[0] = local_22c - d2.v[0];
  local_23c.v[1] = local_228 - d2.v[1];
  local_23c.v[2] = local_224 - d2.v[2];
  tIn_8 = -1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_8,&tOut_8,&local_23c,&d0,inRCyl,inHHCyl,scale);
  if (bVar18) {
    if (-1.0 < tIn_8) {
      local_20c.v[0] = tIn_8 * d0.v[0];
      local_20c.v[1] = tIn_8 * d0.v[1];
      local_20c.v[2] = tIn_8 * d0.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
    if (tOut_8 < 1.0) {
      local_20c.v[0] = tOut_8 * d0.v[0];
      local_20c.v[1] = tOut_8 * d0.v[1];
      local_20c.v[2] = tOut_8 * d0.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
  }
  local_22c = fVar13 - d1.v[0];
  local_228 = fVar15 - d1.v[1];
  local_224 = fVar14 - d1.v[2];
  tOut_9 = 1.0;
  local_23c.v[0] = local_22c + d2.v[0];
  local_23c.v[1] = local_228 + d2.v[1];
  local_23c.v[2] = local_224 + d2.v[2];
  tIn_9 = -1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_9,&tOut_9,&local_23c,&d0,inRCyl,inHHCyl,scale);
  if (bVar18) {
    if (-1.0 < tIn_9) {
      local_20c.v[0] = tIn_9 * d0.v[0];
      local_20c.v[1] = tIn_9 * d0.v[1];
      local_20c.v[2] = tIn_9 * d0.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
    if (tOut_9 < 1.0) {
      local_20c.v[0] = tOut_9 * d0.v[0];
      local_20c.v[1] = tOut_9 * d0.v[1];
      local_20c.v[2] = tOut_9 * d0.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
  }
  local_22c = d2.v[0] + fVar13;
  local_228 = d2.v[1] + fVar15;
  local_224 = d2.v[2] + fVar14;
  tOut_10 = 1.0;
  local_23c.v[0] = local_22c + d0.v[0];
  local_23c.v[1] = local_228 + d0.v[1];
  local_23c.v[2] = local_224 + d0.v[2];
  tIn_10 = -1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_10,&tOut_10,&local_23c,&d1,inRCyl,inHHCyl,scale);
  if (bVar18) {
    if (-1.0 < tIn_10) {
      local_20c.v[0] = tIn_10 * d1.v[0];
      local_20c.v[1] = tIn_10 * d1.v[1];
      local_20c.v[2] = tIn_10 * d1.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
    if (tOut_10 < 1.0) {
      local_20c.v[0] = tOut_10 * d1.v[0];
      local_20c.v[1] = tOut_10 * d1.v[1];
      local_20c.v[2] = tOut_10 * d1.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
  }
  local_22c = d2.v[0] + fVar13;
  local_228 = d2.v[1] + fVar15;
  local_224 = d2.v[2] + fVar14;
  tOut_11 = 1.0;
  local_23c.v[0] = local_22c - d0.v[0];
  local_23c.v[1] = local_228 - d0.v[1];
  local_23c.v[2] = local_224 - d0.v[2];
  tIn_11 = -1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_11,&tOut_11,&local_23c,&d1,inRCyl,inHHCyl,scale);
  if (bVar18) {
    if (-1.0 < tIn_11) {
      local_20c.v[0] = tIn_11 * d1.v[0];
      local_20c.v[1] = tIn_11 * d1.v[1];
      local_20c.v[2] = tIn_11 * d1.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
    if (tOut_11 < 1.0) {
      local_20c.v[0] = tOut_11 * d1.v[0];
      local_20c.v[1] = tOut_11 * d1.v[1];
      local_20c.v[2] = tOut_11 * d1.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
  }
  local_22c = fVar13 - d2.v[0];
  local_228 = fVar15 - d2.v[1];
  local_224 = fVar14 - d2.v[2];
  tOut_12 = 1.0;
  local_23c.v[0] = local_22c - d0.v[0];
  local_23c.v[1] = local_228 - d0.v[1];
  local_23c.v[2] = local_224 - d0.v[2];
  tIn_12 = -1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_12,&tOut_12,&local_23c,&d1,inRCyl,inHHCyl,scale);
  if (bVar18) {
    if (-1.0 < tIn_12) {
      local_20c.v[0] = tIn_12 * d1.v[0];
      local_20c.v[1] = tIn_12 * d1.v[1];
      local_20c.v[2] = tIn_12 * d1.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
    if (tOut_12 < 1.0) {
      local_20c.v[0] = tOut_12 * d1.v[0];
      local_20c.v[1] = tOut_12 * d1.v[1];
      local_20c.v[2] = tOut_12 * d1.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
  }
  local_22c = fVar13 - d2.v[0];
  local_228 = fVar15 - d2.v[1];
  local_224 = fVar14 - d2.v[2];
  tOut_13 = 1.0;
  local_23c.v[0] = local_22c + d0.v[0];
  local_23c.v[1] = local_228 + d0.v[1];
  local_23c.v[2] = local_224 + d0.v[2];
  tIn_13 = -1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_13,&tOut_13,&local_23c,&d1,inRCyl,inHHCyl,scale);
  if (bVar18) {
    if (-1.0 < tIn_13) {
      local_20c.v[0] = tIn_13 * d1.v[0];
      local_20c.v[1] = tIn_13 * d1.v[1];
      local_20c.v[2] = tIn_13 * d1.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
    if (tOut_13 < 1.0) {
      local_20c.v[0] = tOut_13 * d1.v[0];
      local_20c.v[1] = tOut_13 * d1.v[1];
      local_20c.v[2] = tOut_13 * d1.v[2];
      local_21c = local_20c.v[0] + local_23c.v[0];
      local_218 = local_20c.v[1] + local_23c.v[1];
      local_214 = local_20c.v[2] + local_23c.v[2];
      bVerts->v[0] = local_21c;
      bVerts->v[1] = local_218;
      bVerts->v[2] = local_214;
      bVerts = bVerts + 1;
    }
  }
  local_22c = d0.v[0] + fVar13;
  local_228 = d0.v[1] + fVar15;
  local_224 = d0.v[2] + fVar14;
  local_23c.v[0] = local_22c + d1.v[0];
  local_23c.v[1] = local_228 + d1.v[1];
  local_23c.v[2] = local_224 + d1.v[2];
  tIn_14 = -1.0;
  tOut_14 = 1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_14,&tOut_14,&local_23c,&d2,inRCyl,inHHCyl,scale);
  if (bVar18) {
    bVerts->v[0] = tIn_14 * d2.v[0] + local_23c.v[0];
    bVerts->v[1] = tIn_14 * d2.v[1] + local_23c.v[1];
    bVerts->v[2] = tIn_14 * d2.v[2] + local_23c.v[2];
    local_20c.v[0] = tOut_14 * d2.v[0];
    local_20c.v[1] = tOut_14 * d2.v[1];
    local_20c.v[2] = tOut_14 * d2.v[2];
    local_21c = local_20c.v[0] + local_23c.v[0];
    local_218 = local_20c.v[1] + local_23c.v[1];
    local_214 = local_20c.v[2] + local_23c.v[2];
    bVerts[1].v[0] = local_21c;
    bVerts[1].v[1] = local_218;
    bVerts[1].v[2] = local_214;
    bVerts = bVerts + 2;
  }
  local_22c = d0.v[0] + fVar13;
  local_228 = d0.v[1] + fVar15;
  local_224 = d0.v[2] + fVar14;
  local_23c.v[0] = local_22c - d1.v[0];
  local_23c.v[1] = local_228 - d1.v[1];
  local_23c.v[2] = local_224 - d1.v[2];
  tIn_15 = -1.0;
  tOut_15 = 1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_15,&tOut_15,&local_23c,&d2,inRCyl,inHHCyl,scale);
  if (bVar18) {
    bVerts->v[0] = tIn_15 * d2.v[0] + local_23c.v[0];
    bVerts->v[1] = tIn_15 * d2.v[1] + local_23c.v[1];
    bVerts->v[2] = tIn_15 * d2.v[2] + local_23c.v[2];
    local_20c.v[0] = tOut_15 * d2.v[0];
    local_20c.v[1] = tOut_15 * d2.v[1];
    local_20c.v[2] = tOut_15 * d2.v[2];
    local_21c = local_20c.v[0] + local_23c.v[0];
    local_218 = local_20c.v[1] + local_23c.v[1];
    local_214 = local_20c.v[2] + local_23c.v[2];
    bVerts[1].v[0] = local_21c;
    bVerts[1].v[1] = local_218;
    bVerts[1].v[2] = local_214;
    bVerts = bVerts + 2;
  }
  local_22c = fVar13 - d0.v[0];
  local_228 = fVar15 - d0.v[1];
  local_224 = fVar14 - d0.v[2];
  local_23c.v[0] = local_22c - d1.v[0];
  local_23c.v[1] = local_228 - d1.v[1];
  local_23c.v[2] = local_224 - d1.v[2];
  tIn_16 = -1.0;
  tOut_16 = 1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_16,&tOut_16,&local_23c,&d2,inRCyl,inHHCyl,scale);
  if (bVar18) {
    bVerts->v[0] = tIn_16 * d2.v[0] + local_23c.v[0];
    bVerts->v[1] = tIn_16 * d2.v[1] + local_23c.v[1];
    bVerts->v[2] = tIn_16 * d2.v[2] + local_23c.v[2];
    local_20c.v[0] = tOut_16 * d2.v[0];
    local_20c.v[1] = tOut_16 * d2.v[1];
    local_20c.v[2] = tOut_16 * d2.v[2];
    local_21c = local_20c.v[0] + local_23c.v[0];
    local_218 = local_20c.v[1] + local_23c.v[1];
    local_214 = local_20c.v[2] + local_23c.v[2];
    bVerts[1].v[0] = local_21c;
    bVerts[1].v[1] = local_218;
    bVerts[1].v[2] = local_214;
    bVerts = bVerts + 2;
  }
  local_22c = fVar13 - d0.v[0];
  local_228 = fVar15 - d0.v[1];
  local_224 = fVar14 - d0.v[2];
  local_23c.v[0] = local_22c + d1.v[0];
  local_23c.v[1] = local_228 + d1.v[1];
  local_23c.v[2] = local_224 + d1.v[2];
  tIn_17 = -1.0;
  tOut_17 = 1.0;
  bVar18 = McdVanillaSegmentCylinderIntersect(&tIn_17,&tOut_17,&local_23c,&d2,inRCyl,inHHCyl,scale);
  if (bVar18) {
    bVerts->v[0] = tIn_17 * d2.v[0] + local_23c.v[0];
    bVerts->v[1] = tIn_17 * d2.v[1] + local_23c.v[1];
    bVerts->v[2] = tIn_17 * d2.v[2] + local_23c.v[2];
    bVerts[1].v[0] = tOut_17 * d2.v[0] + local_23c.v[0];
    bVerts[1].v[1] = tOut_17 * d2.v[1] + local_23c.v[1];
    bVerts[1].v[2] = tOut_17 * d2.v[2] + local_23c.v[2];
    bVerts = bVerts + 2;
  }
  for (plVar20 = bVertList; plVar20 != bVerts; plVar20 = plVar20 + 1) {
    plVar12 = *outList;
    *outList = plVar12 + 1;
    plVar12->v[0] =
         tAB->row[2].v.v[0] * plVar20->v[2] +
         tAB->row[1].v.v[0] * plVar20->v[1] + tAB->row[0].v.v[0] * plVar20->v[0] +
         tAB->row[3].v.v[0];
    plVar12->v[1] =
         tAB->row[2].v.v[1] * plVar20->v[2] +
         tAB->row[1].v.v[1] * plVar20->v[1] + tAB->row[0].v.v[1] * plVar20->v[0] +
         tAB->row[3].v.v[1];
    plVar12->v[2] =
         tAB->row[2].v.v[2] * plVar20->v[2] +
         tAB->row[1].v.v[2] * plVar20->v[1] + tAB->row[0].v.v[2] * plVar20->v[0] +
         tAB->row[3].v.v[2];
  }
  return;
}


/* ==== OverlapOBBCyl ==== */

bool OverlapOBBCyl(MeReal *outSep,lsVec3 *outN,MeReal *outPN,lsVec3 **outPos,MeReal inEps,
                  lsVec3 *inROBB,MeReal inRCyl,MeReal inHHCyl,lsTransform *inT12,MeReal scale)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  byte bVar13;
  uint uVar14;
  lsVec3 *plVar15;
  lsTransformRow *cylAxis;
  uint uVar16;
  lsVec3 *plVar17;
  int iVar18;
  MeReal *pMVar19;
  bool bVar20;
  lsVec3 *local_1b4;
  lsVec3 *local_1b0;
  float local_188;
  MeReal PN;
  MeU8 normInfo;
  MeReal normalSign;
  MeReal maxSeparation;
  bool apart;
  int i;
  lsVec3 vCross;
  lsVec3 nPerp;
  lsVec3 nPara;
  lsVec3 nPerpN [3];
  lsVec3 nParaN [3];
  lsVec3 nBExET [3];
  lsVec3 nBExCA [3];
  lsVec3 nCW;
  lsVec3 d2;
  lsVec3 d;
  lsVec3 sr;
  
                    /* Unresolved local var: MeReal separation@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal sumR@[???]
                       Unresolved local var: MeReal normD@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal aNormD@[???]
                       Unresolved local var: lsVec3 * cylAxis@[???]
                       Unresolved local var: lsVec3 * cylPos@[???]
                       Unresolved local var: MeReal rZAxis@[???]
                       Unresolved local var: lsVec3 * posList@[DW_OP_reg6(ESI)]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
  iVar18 = 0;
  plVar17 = nPerpN;
  cylAxis = inT12->row + 2;
  plVar15 = nParaN;
  maxSeparation = -3.4028235e+38;
  normalSign = 1.0;
  bVar20 = true;
  normInfo = '\x0f';
  i = 0;
  local_1b4 = plVar17;
  local_1b0 = plVar15;
  do {
                    /* Unresolved local var: int i1@[???]
                       Unresolved local var: int i2@[???]
                       Unresolved local var: MeReal rCyl@[???] */
    uVar16 = 1 << ((byte)i & 0x1f) & 3;
    uVar14 = 1 << (sbyte)uVar16 & 3;
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
    *(undefined4 *)((int)nPara.v + iVar18) = 0;
    nPara.v[uVar16] = (cylAxis->v).v[uVar14];
    nPara.v[uVar14] = -(cylAxis->v).v[uVar16];
    plVar15->v[0] = nPara.v[0];
    plVar15->v[1] = nPara.v[1];
    plVar15->v[2] = nPara.v[2];
    if (1e-15 <= nPara.v[2] * nPara.v[2] + nPara.v[1] * nPara.v[1] + nPara.v[0] * nPara.v[0]) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
      fVar1 = plVar15->v[0];
      fVar2 = plVar15->v[1];
      fVar3 = plVar15->v[2];
      fVar4 = SQRT(fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1);
      if (fVar4 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg12(ST1)] */
        fVar4 = 1.0 / fVar4;
        plVar15->v[0] = fVar1 * fVar4;
        plVar15->v[1] = fVar2 * fVar4;
        plVar15->v[2] = fVar4 * fVar3;
      }
      fVar1 = inT12->row[2].v.v[2];
      fVar2 = inT12->row[2].v.v[1];
      nPara.v[0] = plVar15->v[1] * fVar1 - plVar15->v[2] * fVar2;
      fVar3 = (cylAxis->v).v[0];
      nPara.v[1] = plVar15->v[2] * fVar3 - fVar1 * plVar15->v[0];
      nPara.v[2] = fVar2 * plVar15->v[0] - fVar3 * plVar15->v[1];
      plVar17->v[0] = nPara.v[0];
      plVar17->v[1] = nPara.v[1];
      plVar17->v[2] = nPara.v[2];
    }
    else {
      plVar15->v[0] = 0.0;
      plVar15->v[1] = 0.0;
      plVar15->v[2] = 0.0;
      local_1b0->v[uVar14] = 1.0;
      plVar17->v[0] = 0.0;
      plVar17->v[1] = 0.0;
      plVar17->v[2] = 0.0;
      local_1b4->v[uVar16] = 1.0;
    }
    local_188 = ABS(*(float *)((int)local_1b4->v + iVar18));
    fVar1 = *(float *)((int)inT12->row[3].v.v + iVar18);
    fVar2 = ABS(fVar1) -
            (ABS(*(float *)((int)(cylAxis->v).v + iVar18)) * inHHCyl + local_188 * inRCyl +
            *(float *)((int)inROBB->v + iVar18));
    if (maxSeparation < fVar2) {
      normalSign = 1.0;
      if (0.0 < fVar1) {
        normalSign = -1.0;
      }
      normInfo = (byte)i | 0xc;
      PN = -*(float *)((int)inROBB->v + iVar18);
      bVar20 = inEps < fVar2;
      PN = PN - fVar2;
      maxSeparation = fVar2;
      if (bVar20) goto LAB_00012744;
    }
    local_1b0 = local_1b0 + 1;
    local_1b4 = local_1b4 + 1;
    i = i + 1;
    iVar18 = iVar18 + 4;
    plVar17 = plVar17 + 1;
    plVar15 = plVar15 + 1;
  } while (i < 3);
  fVar1 = inROBB->v[0];
  fVar2 = inROBB->v[1];
  fVar3 = inROBB->v[2];
  fVar4 = inT12->row[3].v.v[0];
  fVar5 = inT12->row[3].v.v[1];
  fVar6 = (cylAxis->v).v[0];
  fVar7 = inT12->row[2].v.v[1];
  fVar8 = inT12->row[3].v.v[2];
  fVar9 = inT12->row[2].v.v[2];
  fVar12 = fVar8 * fVar9 + fVar4 * fVar6 + fVar5 * fVar7;
  fVar11 = ABS(fVar12);
  fVar10 = fVar11 - (ABS(inT12->row[2].v.v[0]) * fVar1 + inHHCyl + fVar2 * ABS(inT12->row[2].v.v[1])
                    + ABS(inT12->row[2].v.v[2]) * fVar3);
  if (fVar10 <= maxSeparation) {
LAB_000123e5:
    i = 0;
    plVar17 = nBExCA;
    pMVar19 = nPara.v;
    do {
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
      uVar14 = 1 << ((byte)i & 0x1f) & 3;
      uVar16 = 1 << (sbyte)uVar14 & 3;
      *pMVar19 = 0.0;
      nPara.v[uVar14] = -(cylAxis->v).v[uVar16];
      nPara.v[uVar16] = (cylAxis->v).v[uVar14];
                    /* Unresolved local var: MeReal t@[???] */
      plVar17->v[0] = nPara.v[0];
      plVar17->v[1] = nPara.v[1];
      plVar17->v[2] = nPara.v[2];
      fVar10 = SQRT(nPara.v[0] * nPara.v[0] + nPara.v[1] * nPara.v[1] + nPara.v[2] * nPara.v[2]);
      if (fVar10 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg12(ST1)] */
        fVar11 = 1.0 / fVar10;
        plVar17->v[0] = nPara.v[0] * fVar11;
        plVar17->v[1] = nPara.v[1] * fVar11;
        plVar17->v[2] = fVar11 * nPara.v[2];
      }
      if (1e-15 < fVar10) {
        fVar12 = plVar17->v[0] * fVar4 + plVar17->v[1] * fVar5 + plVar17->v[2] * fVar8;
        fVar11 = ABS(fVar12);
        fVar10 = fVar11 - (ABS(plVar17->v[0]) * fVar1 + inRCyl + ABS(plVar17->v[1]) * fVar2 +
                          ABS(plVar17->v[2]) * fVar3);
        if (maxSeparation < fVar10) {
          normalSign = 1.0;
          if (0.0 < fVar12) {
            normalSign = -1.0;
          }
          PN = inRCyl - fVar11;
          normInfo = (byte)i | 8;
          bVar20 = inEps < fVar10;
          maxSeparation = fVar10;
          if (bVar20) goto LAB_00012744;
        }
      }
      plVar17 = plVar17 + 1;
      i = i + 1;
      pMVar19 = pMVar19 + 1;
    } while (i < 3);
    fVar10 = fVar9 * -fVar8 + -fVar4 * fVar6 + fVar7 * -fVar5;
    nCW.v[0] = -fVar4 - fVar6 * fVar10;
    nCW.v[1] = -fVar5 - fVar7 * fVar10;
    nCW.v[2] = -fVar8 - fVar10 * fVar9;
                    /* Unresolved local var: MeReal t@[???] */
    fVar6 = SQRT(nCW.v[0] * nCW.v[0] + nCW.v[1] * nCW.v[1] + nCW.v[2] * nCW.v[2]);
    if (fVar6 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg13(ST2)] */
      fVar7 = 1.0 / fVar6;
      nCW.v[0] = nCW.v[0] * fVar7;
      nCW.v[1] = nCW.v[1] * fVar7;
      nCW.v[2] = fVar7 * nCW.v[2];
    }
    if (1e-15 < fVar6) {
      fVar5 = fVar4 * nCW.v[0] + nCW.v[1] * fVar5 + nCW.v[2] * fVar8;
      fVar4 = ABS(fVar5);
      fVar1 = fVar4 - (ABS(nCW.v[1]) * fVar2 + ABS(nCW.v[0]) * fVar1 + inRCyl +
                      ABS(nCW.v[2]) * fVar3);
      if (maxSeparation < fVar1) {
        normalSign = 1.0;
        if (0.0 < fVar5) {
          normalSign = -1.0;
        }
        PN = inRCyl - fVar4;
        normInfo = '\x03';
        bVar20 = inEps < fVar1;
        maxSeparation = fVar1;
      }
    }
  }
  else {
    normalSign = 1.0;
    if (0.0 < fVar12) {
      normalSign = -1.0;
    }
    PN = inHHCyl - fVar11;
    normInfo = '\v';
    bVar20 = inEps < fVar10;
    maxSeparation = fVar10;
    if (fVar10 <= inEps) goto LAB_000123e5;
  }
LAB_00012744:
  *outSep = maxSeparation;
  *outPN = PN;
  plVar17 = *outPos;
  plVar15 = plVar17;
  if (bVar20) goto LAB_0001289f;
  if ((normInfo & 0xc) == 0xc) {
                    /* Unresolved local var: MeI8 axis@[???] */
    uVar14 = normInfo & 3;
    nPara.v[0] = nParaN[uVar14].v[0];
    nPara.v[1] = nParaN[uVar14].v[1];
    nPara.v[2] = nParaN[uVar14].v[2];
    nPerp.v[0] = nPerpN[uVar14].v[0];
    nPerp.v[1] = nPerpN[uVar14].v[1];
    nPerp.v[2] = nPerpN[uVar14].v[2];
    outN->v[0] = 0.0;
    outN->v[1] = 0.0;
    outN->v[2] = 0.0;
    outN->v[uVar14] = normalSign;
  }
  else {
    bVar13 = normInfo & 3;
    if (bVar13 == 3) {
                    /* Unresolved local var: MeI8 axis@[???] */
      if ((normInfo & 0xc) != 8) {
        nPerp.v[0] = nCW.v[0];
        nPerp.v[1] = nCW.v[1];
        nPerp.v[2] = nCW.v[2];
        fVar1 = inT12->row[2].v.v[1];
        fVar2 = inT12->row[2].v.v[2];
        nPara.v[0] = fVar1 * nCW.v[2] - fVar2 * nCW.v[1];
        fVar3 = (cylAxis->v).v[0];
        nPara.v[1] = fVar2 * nCW.v[0] - nCW.v[2] * fVar3;
        nPara.v[2] = fVar3 * nCW.v[1] - fVar1 * nCW.v[0];
        outN->v[0] = nCW.v[0];
        outN->v[1] = nCW.v[1];
        outN->v[2] = nCW.v[2];
        goto LAB_00012862;
      }
      nPara.v[0] = inT12->row[0].v.v[0];
      nPara.v[1] = inT12->row[0].v.v[1];
      nPara.v[2] = inT12->row[0].v.v[2];
      nPerp.v[0] = inT12->row[1].v.v[0];
      nPerp.v[1] = inT12->row[1].v.v[1];
      nPerp.v[2] = inT12->row[1].v.v[2];
      fVar1 = (cylAxis->v).v[0];
      fVar2 = inT12->row[2].v.v[1];
      fVar3 = inT12->row[2].v.v[2];
    }
    else {
                    /* Unresolved local var: MeI8 axis@[???] */
      nPerp.v[0] = nBExCA[(char)bVar13].v[0];
      nPerp.v[1] = nBExCA[(char)bVar13].v[1];
      nPerp.v[2] = nBExCA[(char)bVar13].v[2];
      fVar1 = inT12->row[2].v.v[1];
      fVar2 = inT12->row[2].v.v[2];
      nPara.v[0] = fVar1 * nPerp.v[2] - fVar2 * nPerp.v[1];
      fVar3 = (cylAxis->v).v[0];
      nPara.v[1] = fVar2 * nPerp.v[0] - nPerp.v[2] * fVar3;
      nPara.v[2] = fVar3 * nPerp.v[1] - fVar1 * nPerp.v[0];
      fVar1 = nBExCA[(char)bVar13].v[0];
      fVar2 = nBExCA[(char)bVar13].v[1];
      fVar3 = nBExCA[(char)bVar13].v[2];
    }
    outN->v[0] = normalSign * fVar1;
    outN->v[1] = normalSign * fVar2;
    outN->v[2] = normalSign * fVar3;
  }
LAB_00012862:
  BoxCylIntersect(outPos,inT12,inROBB,inRCyl,inHHCyl,&nPara,&nPerp,(lsVec3 *)cylAxis,scale);
  plVar15 = *outPos;
LAB_0001289f:
  return plVar15 != plVar17;
}


/* ==== McdBoxCylinderIntersect ==== */

MeBool McdBoxCylinderIntersect(McdModelPair *p,McdIntersectResult *result)

{
  McdFramework *pMVar1;
  bool bVar2;
  float *pfVar3;
  float *pfVar4;
  void *pvVar5;
  void *pvVar6;
  lsVec3 *inROBB;
  int iVar7;
  ushort uVar8;
  lsVec3 *plVar9;
  uint uVar10;
  McdContact *pMVar11;
  float fVar12;
  float fVar13;
  float inRCyl;
  float inHHCyl;
  MeI16 dimB;
  MeI16 dimA;
  MeReal rCyl;
  lsVec3 *rA;
  McdFramework *fwk;
  McdBoxID geometry1;
  MeReal eps;
  MeReal separation;
  MeReal PN;
  lsVec3 *verts;
  lsVec3 footprint [48];
  lsVec3 normal;
  lsTransform tAB;
  
                    /* Unresolved local var: lsTransform * tA@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * tB@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdCylinderID geometry2@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal hhCyl@[DW_OP_reg1(ECX)]
                       Unresolved local var: bool touch@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeReal ZZ1@[???]
                       Unresolved local var: MeReal ZZ2@[DW_OP_reg12(ST1)]
                       Unresolved local var: lsVec3 * v@[DW_OP_reg1(ECX)]
                       Unresolved local var: McdContact * c@[DW_OP_reg3(EBX)] */
  pfVar3 = McdModelGetTransformPtr(p->model1);
  pfVar4 = McdModelGetTransformPtr(p->model2);
  fVar12 = McdModelGetContactTolerance(p->model1);
  fVar13 = McdModelGetContactTolerance(p->model2);
  pvVar5 = McdModelGetGeometry(p->model1);
  pvVar6 = McdModelGetGeometry(p->model2);
  pMVar1 = p->model1->frame;
  result->contactCount = 0;
  result->touch = 0;
  inROBB = McdBoxGetRadii(pvVar5);
  inRCyl = McdCylinderGetRadius(pvVar6);
  inHHCyl = McdCylinderGetHalfHeight(pvVar6);
  iVar7 = 0x2f;
  tAB.row[0].v.v[0] = pfVar3[2] * pfVar4[2] + pfVar3[1] * pfVar4[1] + *pfVar3 * *pfVar4;
  tAB.row[0].v.v[1] = pfVar3[6] * pfVar4[2] + pfVar3[5] * pfVar4[1] + pfVar3[4] * *pfVar4;
  tAB.row[0].v.v[2] = pfVar3[10] * pfVar4[2] + pfVar3[9] * pfVar4[1] + pfVar3[8] * *pfVar4;
  tAB.row[1].v.v[0] = pfVar3[2] * pfVar4[6] + pfVar3[1] * pfVar4[5] + *pfVar3 * pfVar4[4];
  tAB.row[1].v.v[1] = pfVar3[6] * pfVar4[6] + pfVar3[5] * pfVar4[5] + pfVar3[4] * pfVar4[4];
  tAB.row[1].v.v[2] = pfVar3[10] * pfVar4[6] + pfVar3[9] * pfVar4[5] + pfVar3[8] * pfVar4[4];
  tAB.row[2].v.v[0] = pfVar3[2] * pfVar4[10] + pfVar3[1] * pfVar4[9] + *pfVar3 * pfVar4[8];
  tAB.row[2].v.v[1] = pfVar3[6] * pfVar4[10] + pfVar3[5] * pfVar4[9] + pfVar3[4] * pfVar4[8];
  tAB.row[2].v.v[2] = pfVar3[10] * pfVar4[10] + pfVar3[9] * pfVar4[9] + pfVar3[8] * pfVar4[8];
  normal.v[1] = pfVar4[0xd] - pfVar3[0xd];
  normal.v[2] = pfVar4[0xe] - pfVar3[0xe];
  normal.v[0] = pfVar4[0xc] - pfVar3[0xc];
  tAB.row[3].v.v[0] = normal.v[2] * pfVar3[2] + normal.v[0] * *pfVar3 + normal.v[1] * pfVar3[1];
  tAB.row[3].v.v[1] = normal.v[2] * pfVar3[6] + normal.v[0] * pfVar3[4] + normal.v[1] * pfVar3[5];
  tAB.row[3].v.v[2] = normal.v[2] * pfVar3[10] + normal.v[0] * pfVar3[8] + normal.v[1] * pfVar3[9];
  do {
    iVar7 = iVar7 + -0x18;
  } while (iVar7 != -1);
  plVar9 = footprint;
  verts = plVar9;
  bVar2 = OverlapOBBCyl(&separation,&normal,&PN,&verts,fVar13 + fVar12,inROBB,inRCyl,inHHCyl,&tAB,
                        pMVar1->mScale);
  uVar10 = 0;
  if (bVar2) {
    uVar8 = (ushort)(normal.v[0] < 0.0001);
    if (normal.v[1] < 0.0001) {
      uVar8 = uVar8 + 1;
    }
    if (normal.v[2] < 0.0001) {
      uVar8 = uVar8 + 1;
    }
    dimB = 2;
    fVar12 = normal.v[2] * tAB.row[2].v.v[2] +
             normal.v[1] * tAB.row[2].v.v[1] + normal.v[0] * tAB.row[2].v.v[0];
    fVar12 = fVar12 * fVar12;
    if ((fVar12 <= 0.999999) && (dimB = 3, 1e-06 <= fVar12)) {
      dimB = 1;
    }
    result->normal[0] = normal.v[2] * pfVar3[8] + normal.v[1] * pfVar3[4] + normal.v[0] * *pfVar3;
    result->normal[1] = normal.v[2] * pfVar3[9] + normal.v[0] * pfVar3[1] + normal.v[1] * pfVar3[5];
    result->normal[2] = normal.v[0] * pfVar3[2] + normal.v[1] * pfVar3[6] + normal.v[2] * pfVar3[10]
    ;
    result->contactCount = 0;
    pMVar11 = result->contacts;
    if (plVar9 == verts) {
      iVar7 = result->contactCount;
    }
    else if (result->contactMaxCount < 1) {
      iVar7 = 0;
    }
    else {
      do {
                    /* Unresolved local var: MeReal s@[DW_OP_reg15(ST4)] */
        fVar12 = (normal.v[2] * plVar9->v[2] +
                 plVar9->v[0] * normal.v[0] + normal.v[1] * plVar9->v[1]) - PN;
        if (fVar12 < separation * 0.01) {
          pMVar11->position[0] =
               plVar9->v[0] * *pfVar3 + plVar9->v[1] * pfVar3[4] + plVar9->v[2] * pfVar3[8] +
               pfVar3[0xc];
          pMVar11->position[1] =
               pfVar3[9] * plVar9->v[2] + pfVar3[5] * plVar9->v[1] + pfVar3[1] * plVar9->v[0] +
               pfVar3[0xd];
          pMVar11->position[2] =
               pfVar3[10] * plVar9->v[2] + pfVar3[6] * plVar9->v[1] + pfVar3[2] * plVar9->v[0] +
               pfVar3[0xe];
          pMVar11->separation = fVar12;
          pMVar11->dims = dimB << 2 | uVar8;
          pMVar11->normal[0] = result->normal[0];
          pMVar11->normal[1] = result->normal[1];
          pMVar11->normal[2] = result->normal[2];
          result->contactCount = result->contactCount + 1;
          pMVar11 = pMVar11 + 1;
        }
        plVar9 = plVar9 + 1;
        if (plVar9 == verts) {
          iVar7 = result->contactCount;
          break;
        }
        iVar7 = result->contactCount;
      } while (iVar7 < result->contactMaxCount);
    }
    uVar10 = (uint)(0 < iVar7);
    result->touch = uVar10;
  }
  return uVar10;
}


/* ==== McdBoxCylinderSafeTime ==== */

int McdBoxCylinderSafeTime(McdModelPair *p,MeReal maxTime,McdSafeTimeResult *result)

{
  void *pvVar1;
  void *pvVar2;
  lsVec3 *plVar3;
  lsVec3 *plVar4;
  lsTransform *plVar5;
  lsTransform *plVar6;
  float *pfVar7;
  int iVar8;
  float fVar9;
  float fVar10;
  MeReal radCyl2;
  lsVec3 *V1;
  lsVec3 *V0;
  McdBoxID geometry1;
  MeReal T;
  lsVec3 P;
  MeReal ext2 [3];
  
                    /* Unresolved local var: McdCylinderID geometry2@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal hheightCyl2@[DW_OP_reg6(ESI)]
                       Unresolved local var: lsTransform * tm1@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * tm2@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal * ext1@[DW_OP_reg0(EAX)]
                       Unresolved local var: uint ixt@[DW_OP_reg2(EDX)]
                       Unresolved local var: float __result@[???] */
  result->pair = p;
  result->time = maxTime;
  pvVar1 = McdModelGetGeometry(p->model1);
  pvVar2 = McdModelGetGeometry(p->model2);
  plVar3 = McdModelGetLinearVelocityPtr(p->model1);
  plVar4 = McdModelGetLinearVelocityPtr(p->model2);
  fVar9 = McdCylinderGetRadius(pvVar2);
  fVar10 = McdCylinderGetHalfHeight(pvVar2);
  plVar5 = McdModelGetTransformPtr(p->model1);
  plVar6 = McdModelGetTransformPtr(p->model2);
  pfVar7 = McdBoxGetRadii(pvVar1);
  ext2[0] = fVar9 * 0.8284271;
  ext2[1] = fVar9 * 0.8284271;
  ext2[2] = fVar10;
  iVar8 = MovingBoxBoxIntersect(pfVar7,plVar5,plVar3,ext2,plVar6,plVar4,maxTime,&T,(lsVec3 *)&P);
  if (iVar8 == 0) {
    result->time = maxTime;
  }
  else {
    result->time = T;
  }
  return iVar8;
}


/* ==== McdBoxCylinderRegisterInteraction ==== */

MeBool McdBoxCylinderRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdBoxCylinderIntersect;
  interactions.safetimeFn = McdBoxCylinderSafeTime;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,2,4,&interactions);
  return 1;
}


/* ==== McdVanillaSegmentCubeIntersect ==== */

int McdVanillaSegmentCubeIntersect
              (MeReal *tInMax,MeReal *tOutMin,lsVec3 *orig,lsVec3 *disp,lsVec3 *invDisp,lsVec3 *inR,
              MeReal eps)

{
  float fVar1;
  float fVar2;
  float fVar3;
  
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal tIn@[???]
                       Unresolved local var: MeReal tOut@[DW_OP_reg11(ST0)] */
  if (eps <= ABS(disp->v[0])) {
    fVar1 = orig->v[0] * invDisp->v[0];
    fVar2 = ABS(invDisp->v[0]) * inR->v[0];
    fVar3 = -fVar1 - fVar2;
    fVar2 = fVar2 - fVar1;
    if (*tInMax < fVar3) {
      *tInMax = fVar3;
    }
    fVar1 = *tOutMin;
    if (fVar2 < fVar1) {
      *tOutMin = fVar2;
      fVar1 = fVar2;
    }
    if (fVar1 < *tInMax) {
      return 0;
    }
  }
  else if (inR->v[0] < ABS(orig->v[0])) {
    return 0;
  }
  if (eps <= ABS(disp->v[1])) {
    fVar1 = orig->v[1] * invDisp->v[1];
    fVar2 = ABS(invDisp->v[1]) * inR->v[1];
    fVar3 = -fVar1 - fVar2;
    fVar2 = fVar2 - fVar1;
    if (*tInMax < fVar3) {
      *tInMax = fVar3;
    }
    fVar1 = *tOutMin;
    if (fVar2 < fVar1) {
      *tOutMin = fVar2;
      fVar1 = fVar2;
    }
    if (fVar1 < *tInMax) {
      return 0;
    }
  }
  else if (inR->v[1] < ABS(orig->v[1])) {
    return 0;
  }
  if (eps <= ABS(disp->v[2])) {
    fVar1 = orig->v[2] * invDisp->v[2];
    fVar2 = ABS(invDisp->v[2]) * inR->v[2];
    fVar3 = -fVar1 - fVar2;
    fVar2 = fVar2 - fVar1;
    if (*tInMax < fVar3) {
      *tInMax = fVar3;
    }
    fVar1 = *tOutMin;
    if (fVar2 < fVar1) {
      *tOutMin = fVar2;
      fVar1 = fVar2;
    }
    if (fVar1 < *tInMax) {
      return 0;
    }
  }
  else if (inR->v[2] < ABS(orig->v[2])) {
    return 0;
  }
  return 1;
}


/* ==== McdVanillaSegmentCylinderIntersect ==== */

bool McdVanillaSegmentCylinderIntersect
               (MeReal *tInMax,MeReal *tOutMin,lsVec3 *orig,lsVec3 *disp,MeReal inR,MeReal inHH,
               MeReal scale)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  MeReal tExitNumZ;
  MeReal tExitNum;
  MeReal tEnterNum;
  
                    /* Unresolved local var: MeReal X2minusR2@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal XdotDisp@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal r2@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal XdotDisp2@[???]
                       Unresolved local var: MeReal arg@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal test@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal MeSqrtArg@[???]
                       Unresolved local var: MeReal origZ@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal dispZ@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal numZ0@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal denZ@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal tEnterNumZ@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal invR2@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal invZ@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal tIn@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal tOut@[DW_OP_reg11(ST0)] */
  fVar1 = orig->v[0];
  fVar2 = orig->v[1];
  fVar8 = 0.0;
  fVar3 = disp->v[1];
  fVar7 = (fVar1 * fVar1 + fVar2 * fVar2) - inR * inR;
  fVar4 = disp->v[0];
  fVar1 = fVar2 * fVar3 + fVar1 * fVar4;
  fVar3 = fVar4 * fVar4 + fVar3 * fVar3;
  fVar2 = fVar1 * fVar1 - fVar3 * fVar7;
  if (fVar2 < 0.0) {
    return false;
  }
  fVar4 = *tOutMin;
  fVar5 = -fVar1 - fVar4 * fVar3;
  if ((0.0 <= fVar5) && (fVar2 < fVar5 * fVar5)) {
    return false;
  }
  fVar5 = *tInMax;
  fVar6 = fVar5 * fVar3 + fVar1;
  if ((0.0 <= fVar6) && (fVar2 < fVar6 * fVar6)) {
    return false;
  }
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
  fVar10 = -fVar1 - SQRT(fVar2);
  fVar1 = SQRT(fVar2) - fVar1;
  fVar2 = disp->v[2];
  fVar6 = orig->v[2];
  if (0.0 <= fVar2) {
    fVar12 = -fVar6;
  }
  else {
    fVar2 = -fVar2;
    fVar12 = fVar6;
  }
  fVar9 = fVar12 - inHH;
  if (fVar4 * fVar2 < fVar9) {
    return false;
  }
  fVar12 = fVar12 + inHH;
  if (fVar12 < fVar5 * fVar2) {
    return false;
  }
  fVar11 = fVar1 * fVar2;
  if (fVar9 * fVar3 <= fVar11) {
    if (fVar10 * fVar2 <= fVar12 * fVar3) {
      if (scale * scale * 1e-16 <= fVar3) {
        if (scale * 1e-08 <= ABS(fVar2)) {
          fVar8 = 0.0;
          if (fVar2 != 0.0) {
            fVar8 = 1.0 / fVar2;
          }
          fVar7 = 0.0;
          if (fVar3 != 0.0) {
            fVar7 = 1.0 / fVar3;
          }
          if (fVar9 * fVar3 <= fVar10 * fVar2) {
            fVar9 = fVar10 * fVar7;
          }
          else {
            fVar9 = fVar9 * fVar8;
          }
          if (fVar12 * fVar3 < fVar11) {
            fVar8 = fVar12 * fVar8;
            goto LAB_000135b9;
          }
        }
        else {
          fVar7 = 0.0;
          if (inHH < ABS(fVar6)) {
            return false;
          }
          if (fVar3 != 0.0) {
            fVar7 = 1.0 / fVar3;
          }
          fVar9 = fVar10 * fVar7;
        }
        fVar8 = fVar7 * fVar1;
      }
      else {
        if (0.0 < fVar7) {
          return false;
        }
        if (fVar2 != 0.0) {
          fVar8 = 1.0 / fVar2;
        }
        fVar9 = fVar9 * fVar8;
        fVar8 = fVar8 * fVar12;
      }
LAB_000135b9:
      if (fVar5 < fVar9) {
        *tInMax = fVar9;
        fVar4 = *tOutMin;
      }
      if (fVar8 < fVar4) {
        *tOutMin = fVar8;
      }
      return true;
    }
  }
  return false;
}


