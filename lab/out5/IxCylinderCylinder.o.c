/* ==== CylCylIntersect ==== */

void CylCylIntersect(lsVec3 **outList,lsTransform *tAB,MeReal inR1,MeReal inHH1,lsVec3 *perp1,
                    lsVec3 *para1,bool doCyl1,MeReal inR2,MeReal inHH2,lsVec3 *perp2,lsVec3 *para2,
                    bool doCyl2,MeReal scale)

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
  lsVec3 *plVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  MeReal *pMVar20;
  bool bVar21;
  lsVec3 *plVar22;
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
  lsVec3 *bVerts;
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
  lsVec3 bVertList [12];
  lsVec3 e0;
  lsVec3 e1;
  lsVec3 e2;
  lsVec3 d2;
  lsVec3 d1;
  lsVec3 d0;
  lsTransform tBA;
  
  fVar1 = tAB->row[0].v.v[0];
  fVar2 = tAB->row[1].v.v[0];
  fVar3 = tAB->row[2].v.v[0];
  fVar4 = tAB->row[0].v.v[1];
  fVar5 = tAB->row[1].v.v[1];
  fVar6 = tAB->row[2].v.v[1];
  fVar7 = tAB->row[0].v.v[2];
  fVar8 = tAB->row[1].v.v[2];
  fVar9 = tAB->row[2].v.v[2];
  fVar10 = tAB->row[3].v.v[0];
  fVar11 = tAB->row[3].v.v[1];
  fVar12 = tAB->row[3].v.v[2];
  d0.v[0] = -fVar10;
  d0.v[1] = -fVar11;
  d0.v[2] = -fVar12;
  fVar14 = fVar1 * d0.v[0] + fVar4 * d0.v[1] + fVar7 * d0.v[2];
  fVar16 = fVar2 * d0.v[0] + fVar5 * d0.v[1] + fVar8 * d0.v[2];
  fVar15 = d0.v[2] * fVar9 + d0.v[0] * fVar3 + d0.v[1] * fVar6;
  if (doCyl2) {
    d0.v[0] = inR2 * perp2->v[0];
    d0.v[1] = inR2 * perp2->v[1];
    d0.v[2] = inR2 * perp2->v[2];
    fVar17 = inR2 * para2->v[0];
    fVar18 = inR2 * para2->v[1];
    fVar19 = inR2 * para2->v[2];
    d2.v[0] = inHH2 * fVar3;
    e2.v[0] = fVar10 + d2.v[0];
    d2.v[1] = inHH2 * fVar6;
    d2.v[2] = inHH2 * fVar9;
    e2.v[1] = fVar11 + d2.v[1];
    e2.v[2] = fVar12 + d2.v[2];
    tIn_6 = -1.0;
    tOut_6 = 1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn_6,&tOut_6,&e2,&d0,inR1,inHH1,scale);
    if (bVar21) {
      if (-1.0 < tIn_6) {
        plVar22 = *outList;
        *outList = plVar22 + 1;
        plVar22->v[0] = tIn_6 * d0.v[0] + e2.v[0];
        plVar22->v[1] = tIn_6 * d0.v[1] + e2.v[1];
        plVar22->v[2] = tIn_6 * d0.v[2] + e2.v[2];
      }
      if (tOut_6 < 1.0) {
        plVar22 = *outList;
        *outList = plVar22 + 1;
        plVar22->v[0] = tOut_6 * d0.v[0] + e2.v[0];
        plVar22->v[1] = tOut_6 * d0.v[1] + e2.v[1];
        plVar22->v[2] = tOut_6 * d0.v[2] + e2.v[2];
      }
    }
    e0.v[0] = tAB->row[3].v.v[0] - d2.v[0];
    e0.v[1] = tAB->row[3].v.v[1] - d2.v[1];
    e0.v[2] = tAB->row[3].v.v[2] - d2.v[2];
    tIn_7 = -1.0;
    tOut_7 = 1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn_7,&tOut_7,&e0,&d0,inR1,inHH1,scale);
    if (bVar21) {
      if (-1.0 < tIn_7) {
        plVar22 = *outList;
        *outList = plVar22 + 1;
        e2.v[0] = tIn_7 * d0.v[0];
        e2.v[1] = tIn_7 * d0.v[1];
        e2.v[2] = tIn_7 * d0.v[2];
        plVar22->v[0] = e2.v[0] + e0.v[0];
        plVar22->v[1] = e2.v[1] + e0.v[1];
        plVar22->v[2] = e2.v[2] + e0.v[2];
      }
      if (tOut_7 < 1.0) {
        plVar22 = *outList;
        *outList = plVar22 + 1;
        e2.v[0] = tOut_7 * d0.v[0];
        e2.v[1] = tOut_7 * d0.v[1];
        e2.v[2] = tOut_7 * d0.v[2];
        plVar22->v[0] = e2.v[0] + e0.v[0];
        plVar22->v[1] = e2.v[1] + e0.v[1];
        plVar22->v[2] = e2.v[2] + e0.v[2];
      }
    }
    e0.v[0] = d0.v[0] + tAB->row[3].v.v[0];
    e0.v[1] = d0.v[1] + tAB->row[3].v.v[1];
    e0.v[2] = d0.v[2] + tAB->row[3].v.v[2];
    tIn_8 = -1.0;
    tOut_8 = 1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn_8,&tOut_8,&e0,&d2,inR1,inHH1,scale);
    if (bVar21) {
      plVar22 = *outList;
      *outList = plVar22 + 1;
      plVar22->v[0] = tIn_8 * d2.v[0] + e0.v[0];
      plVar22->v[1] = tIn_8 * d2.v[1] + e0.v[1];
      plVar22->v[2] = tIn_8 * d2.v[2] + e0.v[2];
      plVar22 = *outList;
      *outList = plVar22 + 1;
      e2.v[0] = tOut_8 * d2.v[0];
      e2.v[1] = tOut_8 * d2.v[1];
      e2.v[2] = tOut_8 * d2.v[2];
      plVar22->v[0] = e2.v[0] + e0.v[0];
      plVar22->v[1] = e2.v[1] + e0.v[1];
      plVar22->v[2] = e2.v[2] + e0.v[2];
    }
    e0.v[0] = tAB->row[3].v.v[0] - d0.v[0];
    e0.v[1] = tAB->row[3].v.v[1] - d0.v[1];
    e0.v[2] = tAB->row[3].v.v[2] - d0.v[2];
    tIn_9 = -1.0;
    tOut_9 = 1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn_9,&tOut_9,&e0,&d2,inR1,inHH1,scale);
    if (bVar21) {
      plVar22 = *outList;
      *outList = plVar22 + 1;
      plVar22->v[0] = tIn_9 * d2.v[0] + e0.v[0];
      plVar22->v[1] = tIn_9 * d2.v[1] + e0.v[1];
      plVar22->v[2] = tIn_9 * d2.v[2] + e0.v[2];
      plVar22 = *outList;
      *outList = plVar22 + 1;
      e2.v[0] = tOut_9 * d2.v[0];
      e2.v[1] = tOut_9 * d2.v[1];
      e2.v[2] = tOut_9 * d2.v[2];
      plVar22->v[0] = e2.v[0] + e0.v[0];
      plVar22->v[1] = e2.v[1] + e0.v[1];
      plVar22->v[2] = e2.v[2] + e0.v[2];
    }
    e0.v[0] = fVar17 + tAB->row[3].v.v[0];
    e0.v[1] = fVar18 + tAB->row[3].v.v[1];
    e0.v[2] = fVar19 + tAB->row[3].v.v[2];
    tIn_10 = -1.0;
    tOut_10 = 1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn_10,&tOut_10,&e0,&d2,inR1,inHH1,scale);
    if (bVar21) {
      plVar22 = *outList;
      *outList = plVar22 + 1;
      plVar22->v[0] = tIn_10 * d2.v[0] + e0.v[0];
      plVar22->v[1] = tIn_10 * d2.v[1] + e0.v[1];
      plVar22->v[2] = tIn_10 * d2.v[2] + e0.v[2];
      plVar22 = *outList;
      *outList = plVar22 + 1;
      e2.v[0] = tOut_10 * d2.v[0];
      e2.v[1] = tOut_10 * d2.v[1];
      e2.v[2] = tOut_10 * d2.v[2];
      plVar22->v[0] = e2.v[0] + e0.v[0];
      plVar22->v[1] = e2.v[1] + e0.v[1];
      plVar22->v[2] = e2.v[2] + e0.v[2];
    }
    e0.v[1] = tAB->row[3].v.v[1] - fVar18;
    e0.v[2] = tAB->row[3].v.v[2] - fVar19;
    e0.v[0] = tAB->row[3].v.v[0] - fVar17;
    tIn_11 = -1.0;
    tOut_11 = 1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn_11,&tOut_11,&e0,&d2,inR1,inHH1,scale);
    if (bVar21) {
      plVar22 = *outList;
      *outList = plVar22 + 1;
      plVar22->v[0] = tIn_11 * d2.v[0] + e0.v[0];
      plVar22->v[1] = tIn_11 * d2.v[1] + e0.v[1];
      plVar22->v[2] = tIn_11 * d2.v[2] + e0.v[2];
      plVar22 = *outList;
      *outList = plVar22 + 1;
      plVar22->v[0] = tOut_11 * d2.v[0] + e0.v[0];
      plVar22->v[1] = tOut_11 * d2.v[1] + e0.v[1];
      plVar22->v[2] = tOut_11 * d2.v[2] + e0.v[2];
    }
  }
  if (doCyl1) {
                    /* Unresolved local var: lsVec3 * vert@[DW_OP_reg3(EBX)] */
    bVerts = bVertList;
    e0.v[0] = (fVar7 * perp1->v[2] + perp1->v[0] * fVar1 + fVar4 * perp1->v[1]) * inR1;
    e0.v[1] = (fVar8 * perp1->v[2] + perp1->v[0] * fVar2 + fVar5 * perp1->v[1]) * inR1;
    e0.v[2] = (fVar6 * perp1->v[1] + perp1->v[0] * fVar3 + fVar9 * perp1->v[2]) * inR1;
    fVar1 = (fVar1 * para1->v[0] + fVar4 * para1->v[1] + fVar7 * para1->v[2]) * inR1;
    e2.v[0] = inHH1 * fVar7;
    tOut = 1.0;
    e2.v[1] = inHH1 * fVar8;
    e2.v[2] = inHH1 * fVar9;
    fVar2 = (fVar2 * para1->v[0] + fVar5 * para1->v[1] + fVar8 * para1->v[2]) * inR1;
    fVar3 = (fVar9 * para1->v[2] + fVar6 * para1->v[1] + fVar3 * para1->v[0]) * inR1;
    d2.v[0] = e2.v[0] + fVar14;
    d2.v[1] = e2.v[1] + fVar16;
    d2.v[2] = e2.v[2] + fVar15;
    tIn = -1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn,&tOut,&d2,&e0,inR2,inHH2,scale);
    plVar22 = bVerts;
    if (bVar21) {
      if (-1.0 < tIn) {
        d0.v[0] = tIn * e0.v[0];
        d0.v[1] = tIn * e0.v[1];
        d0.v[2] = tIn * e0.v[2];
        fVar5 = d0.v[1] + d2.v[1];
        fVar4 = d0.v[2] + d2.v[2];
        pMVar20 = bVerts->v;
        bVerts = bVerts + 1;
        *pMVar20 = d0.v[0] + d2.v[0];
        plVar22->v[1] = fVar5;
        plVar22->v[2] = fVar4;
      }
      if (tOut < 1.0) {
        d0.v[0] = tOut * e0.v[0];
        d0.v[1] = tOut * e0.v[1];
        d0.v[2] = tOut * e0.v[2];
        fVar5 = d0.v[1] + d2.v[1];
        fVar4 = d0.v[2] + d2.v[2];
        bVerts->v[0] = d0.v[0] + d2.v[0];
        bVerts->v[1] = fVar5;
        bVerts->v[2] = fVar4;
        bVerts = bVerts + 1;
      }
    }
    tOut_1 = 1.0;
    d2.v[0] = fVar14 - e2.v[0];
    d2.v[1] = fVar16 - e2.v[1];
    d2.v[2] = fVar15 - e2.v[2];
    tIn_1 = -1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn_1,&tOut_1,&d2,&e0,inR2,inHH2,scale);
    if (bVar21) {
      if (-1.0 < tIn_1) {
        d0.v[0] = tIn_1 * e0.v[0];
        d0.v[1] = tIn_1 * e0.v[1];
        d0.v[2] = tIn_1 * e0.v[2];
        fVar5 = d0.v[1] + d2.v[1];
        fVar4 = d0.v[2] + d2.v[2];
        bVerts->v[0] = d0.v[0] + d2.v[0];
        bVerts->v[1] = fVar5;
        bVerts->v[2] = fVar4;
        bVerts = bVerts + 1;
      }
      if (tOut_1 < 1.0) {
        d0.v[0] = tOut_1 * e0.v[0];
        d0.v[1] = tOut_1 * e0.v[1];
        d0.v[2] = tOut_1 * e0.v[2];
        fVar5 = d0.v[1] + d2.v[1];
        fVar4 = d0.v[2] + d2.v[2];
        bVerts->v[0] = d0.v[0] + d2.v[0];
        bVerts->v[1] = fVar5;
        bVerts->v[2] = fVar4;
        bVerts = bVerts + 1;
      }
    }
    d2.v[0] = e0.v[0] + fVar14;
    d2.v[1] = e0.v[1] + fVar16;
    d2.v[2] = e0.v[2] + fVar15;
    tIn_2 = -1.0;
    tOut_2 = 1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn_2,&tOut_2,&d2,&e2,inR2,inHH2,scale);
    if (bVar21) {
      fVar5 = tIn_2 * e2.v[1] + d2.v[1];
      fVar4 = tIn_2 * e2.v[2] + d2.v[2];
      bVerts->v[0] = tIn_2 * e2.v[0] + d2.v[0];
      bVerts->v[1] = fVar5;
      bVerts->v[2] = fVar4;
      d0.v[0] = tOut_2 * e2.v[0];
      d0.v[1] = tOut_2 * e2.v[1];
      d0.v[2] = tOut_2 * e2.v[2];
      fVar5 = d0.v[1] + d2.v[1];
      fVar4 = d0.v[2] + d2.v[2];
      bVerts[1].v[0] = d0.v[0] + d2.v[0];
      bVerts[1].v[1] = fVar5;
      bVerts[1].v[2] = fVar4;
      bVerts = bVerts + 2;
    }
    d2.v[0] = fVar14 - e0.v[0];
    d2.v[1] = fVar16 - e0.v[1];
    d2.v[2] = fVar15 - e0.v[2];
    tIn_3 = -1.0;
    tOut_3 = 1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn_3,&tOut_3,&d2,&e2,inR2,inHH2,scale);
    if (bVar21) {
      fVar5 = tIn_3 * e2.v[1] + d2.v[1];
      fVar4 = tIn_3 * e2.v[2] + d2.v[2];
      bVerts->v[0] = tIn_3 * e2.v[0] + d2.v[0];
      bVerts->v[1] = fVar5;
      bVerts->v[2] = fVar4;
      d0.v[0] = tOut_3 * e2.v[0];
      d0.v[1] = tOut_3 * e2.v[1];
      d0.v[2] = tOut_3 * e2.v[2];
      fVar5 = d0.v[1] + d2.v[1];
      fVar4 = d0.v[2] + d2.v[2];
      bVerts[1].v[0] = d0.v[0] + d2.v[0];
      bVerts[1].v[1] = fVar5;
      bVerts[1].v[2] = fVar4;
      bVerts = bVerts + 2;
    }
    d2.v[0] = fVar1 + fVar14;
    d2.v[1] = fVar2 + fVar16;
    d2.v[2] = fVar3 + fVar15;
    tIn_4 = -1.0;
    tOut_4 = 1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn_4,&tOut_4,&d2,&e2,inR2,inHH2,scale);
    if (bVar21) {
      fVar5 = tIn_4 * e2.v[1] + d2.v[1];
      fVar4 = tIn_4 * e2.v[2] + d2.v[2];
      bVerts->v[0] = tIn_4 * e2.v[0] + d2.v[0];
      bVerts->v[1] = fVar5;
      bVerts->v[2] = fVar4;
      d0.v[0] = tOut_4 * e2.v[0];
      d0.v[1] = tOut_4 * e2.v[1];
      d0.v[2] = tOut_4 * e2.v[2];
      fVar5 = d0.v[1] + d2.v[1];
      fVar4 = d0.v[2] + d2.v[2];
      bVerts[1].v[0] = d0.v[0] + d2.v[0];
      bVerts[1].v[1] = fVar5;
      bVerts[1].v[2] = fVar4;
      bVerts = bVerts + 2;
    }
    d2.v[0] = fVar14 - fVar1;
    d2.v[1] = fVar16 - fVar2;
    d2.v[2] = fVar15 - fVar3;
    tIn_5 = -1.0;
    tOut_5 = 1.0;
    bVar21 = McdVanillaSegmentCylinderIntersect(&tIn_5,&tOut_5,&d2,&e2,inR2,inHH2,scale);
    if (bVar21) {
      fVar2 = tIn_5 * e2.v[1] + d2.v[1];
      fVar1 = tIn_5 * e2.v[2] + d2.v[2];
      bVerts->v[0] = tIn_5 * e2.v[0] + d2.v[0];
      bVerts->v[1] = fVar2;
      bVerts->v[2] = fVar1;
      d0.v[0] = tOut_5 * e2.v[0];
      d0.v[1] = tOut_5 * e2.v[1];
      d0.v[2] = tOut_5 * e2.v[2];
      bVerts[1].v[0] = tOut_5 * e2.v[0] + d2.v[0];
      bVerts[1].v[1] = tOut_5 * e2.v[1] + d2.v[1];
      bVerts[1].v[2] = tOut_5 * e2.v[2] + d2.v[2];
      bVerts = bVerts + 2;
    }
    for (plVar22 = bVertList; plVar22 != bVerts; plVar22 = plVar22 + 1) {
      plVar13 = *outList;
      *outList = plVar13 + 1;
      plVar13->v[0] =
           tAB->row[2].v.v[0] * plVar22->v[2] +
           tAB->row[1].v.v[0] * plVar22->v[1] + tAB->row[0].v.v[0] * plVar22->v[0] +
           tAB->row[3].v.v[0];
      plVar13->v[1] =
           tAB->row[2].v.v[1] * plVar22->v[2] +
           tAB->row[1].v.v[1] * plVar22->v[1] + tAB->row[0].v.v[1] * plVar22->v[0] +
           tAB->row[3].v.v[1];
      plVar13->v[2] =
           tAB->row[2].v.v[2] * plVar22->v[2] +
           tAB->row[1].v.v[2] * plVar22->v[1] + tAB->row[0].v.v[2] * plVar22->v[0] +
           tAB->row[3].v.v[2];
    }
  }
  return;
}


/* ==== EndCapMin ==== */

MeReal EndCapMin(MeReal *theta)

{
  longdouble lVar1;
  longdouble lVar2;
  longdouble lVar3;
  longdouble lVar4;
  
                    /* Unresolved local var: MeReal c1@[???]
                       Unresolved local var: MeReal s1@[???]
                       Unresolved local var: MeReal c2@[???]
                       Unresolved local var: MeReal s2@[???]
                       Unresolved local var: float __result@[DW_OP_reg17(ST6)] */
                    /* Unresolved local var: float __result@[DW_OP_reg16(ST5)] */
  lVar1 = (longdouble)fcos((longdouble)*theta);
                    /* Unresolved local var: float __result@[DW_OP_reg14(ST3)] */
  lVar2 = (longdouble)fsin((longdouble)*theta);
  lVar3 = (longdouble)fcos((longdouble)theta[1]);
                    /* Unresolved local var: float __result@[DW_OP_reg15(ST4)] */
  lVar4 = (longdouble)fsin((longdouble)theta[1]);
  return (MeReal)((((longdouble)dispDotX2 * lVar3 + (longdouble)dispDotY2 * lVar4) *
                   (longdouble)cylR2 -
                  ((longdouble)dispDotX1 * lVar1 + (longdouble)dispDotY1 * lVar2) *
                  (longdouble)cylR1) -
                 (lVar1 * lVar3 * (longdouble)X1DotX2 + lVar1 * lVar4 * (longdouble)X1DotY2 +
                  lVar3 * lVar2 * (longdouble)Y1DotX2 + lVar2 * lVar4 * (longdouble)Y1DotY2) *
                 (longdouble)cylR1 * (longdouble)cylR2);
}


/* ==== EndCapMinGrad ==== */

void EndCapMinGrad(MeReal *grad,MeReal *theta)

{
  float fVar1;
  longdouble lVar2;
  longdouble lVar3;
  longdouble lVar4;
  float __result;
  
                    /* Unresolved local var: MeReal c1@[???]
                       Unresolved local var: MeReal s1@[???]
                       Unresolved local var: MeReal c2@[???]
                       Unresolved local var: MeReal s2@[???] */
                    /* Unresolved local var: float __result@[DW_OP_reg17(ST6)] */
  lVar2 = (longdouble)fcos((longdouble)*theta);
                    /* Unresolved local var: float __result@[DW_OP_reg18(ST7)] */
  lVar3 = (longdouble)fsin((longdouble)*theta);
  lVar4 = (longdouble)fcos((longdouble)theta[1]);
  fVar1 = (float)lVar4;
                    /* Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
  lVar4 = (longdouble)fsin((longdouble)theta[1]);
  *grad = (float)((longdouble)
                  (float)(((longdouble)dispDotY1 * lVar2 - (longdouble)dispDotX1 * lVar3) *
                         (longdouble)-cylR1) -
                 (longdouble)cylR1 * (longdouble)cylR2 *
                 ((((longdouble)fVar1 * lVar2 * (longdouble)Y1DotX2 +
                   lVar2 * lVar4 * (longdouble)Y1DotY2) -
                  (longdouble)fVar1 * lVar3 * (longdouble)X1DotX2) -
                 lVar3 * lVar4 * (longdouble)X1DotY2));
  grad[1] = (float)((longdouble)
                    (float)(((longdouble)fVar1 * (longdouble)dispDotY2 -
                            lVar4 * (longdouble)dispDotX2) * (longdouble)cylR2) -
                   (longdouble)cylR2 * (longdouble)cylR1 *
                   ((((longdouble)fVar1 * lVar2 * (longdouble)X1DotY2 +
                     (longdouble)fVar1 * lVar3 * (longdouble)Y1DotY2) -
                    (longdouble)(float)(lVar2 * lVar4) * (longdouble)X1DotX2) -
                   lVar3 * lVar4 * (longdouble)Y1DotX2));
  return;
}


/* ==== OverlapCylCyl ==== */

/* WARNING: Type propagation algorithm not settling */

bool OverlapCylCyl(MeReal *outSep,lsVec3 *outN,MeReal *outPN,lsVec3 **outPos,MeI16 *outDims,
                  MeReal inEps,MeReal inR1,MeReal inHH1,MeReal inR2,MeReal inHH2,lsTransform *inT12,
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
  char cVar9;
  char cVar10;
  float fVar11;
  float fVar12;
  bool bVar13;
  longdouble lVar14;
  longdouble lVar15;
  longdouble lVar16;
  longdouble lVar17;
  longdouble lVar18;
  longdouble lVar19;
  longdouble lVar20;
  longdouble lVar21;
  longdouble lVar22;
  longdouble lVar23;
  longdouble lVar24;
  lsVec3 *plVar25;
  lsVec3 *para;
  lsTransformRow *n;
  lsTransformRow *n_00;
  float local_2fc;
  float local_2f8;
  float local_2f4;
  float local_2f0;
  float local_2cc;
  MeReal PN;
  MeReal rc1;
  MeI8 minAxis1;
  MeReal z2;
  MeReal z1;
  MeReal nCapZ;
  MeReal dCap;
  MeI8 normInfo;
  bool apart;
  MeReal maxSeparation;
  MeReal normalSign;
  MeReal theta [2];
  lsVec3 nPara2;
  lsVec3 nPerp2;
  lsVec3 vCross_3;
  lsVec3 nPara1;
  lsVec3 nPerp1;
  lsVec3 aa;
  lsVec3 zCap;
  lsVec3 p2;
  lsVec3 p1;
  lsVec3 para2;
  lsVec3 para1;
  lsVec3 p;
  lsVec3 disp;
  lsVec3 z;
  lsVec3 c;
  lsVec3 pE1E2;
  lsVec3 nE1E2;
  lsVec3 nAA;
  lsVec3 nPerpW2;
  lsVec3 nPerpW1;
  lsVec3 nParaW2;
  lsVec3 nParaW1;
  lsVec3 nW2;
  lsVec3 nW1;
  lsVec3 nPerpA2E1;
  lsVec3 nPerpA1E2;
  lsVec3 nParaA2E1;
  lsVec3 nParaA1E2;
  lsVec3 nA2E1;
  lsVec3 nA1E2;
  lsVec3 nPerpN2;
  lsVec3 nParaN2;
  lsVec3 nPerpN1;
  lsVec3 nParaN1;
  
                    /* Unresolved local var: MeReal rCyl@[???]
                       Unresolved local var: MeReal sumR@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal normD@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal separation@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal disc@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeI8 axis1@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeI8 axis2@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeI8 minAxis2@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal minCapD@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal rc2@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal d2Min@[???]
                       Unresolved local var: MeReal zCapZ@[???]
                       Unresolved local var: MeReal aNormD@[???]
                       Unresolved local var: MeReal sinTheta@[???]
                       Unresolved local var: lsVec3 * posList@[???]
                       Unresolved local var: float __result@[DW_OP_reg13(ST2)]
                       Unresolved local var: float __result@[???]
                       Unresolved local var: float __result@[DW_OP_reg14(ST3)] */
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
  local_2f4 = inT12->row[2].v.v[1];
  fVar12 = -local_2f4;
  fVar1 = inT12->row[2].v.v[0];
  nPerp1.v[2] = -0.0;
  fVar11 = -fVar1;
  nParaN2.v[2] = -0.0;
  if (1e-15 <= local_2f4 * local_2f4 + fVar11 * fVar11) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
    fVar2 = SQRT(fVar11 * fVar11 + local_2f4 * local_2f4 + 0.0);
    nParaN2.v[0] = local_2f4;
    if (fVar2 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
      fVar2 = 1.0 / fVar2;
      fVar11 = fVar11 * fVar2;
      nParaN2.v[2] = fVar2 * -0.0;
      nParaN2.v[0] = local_2f4 * fVar2;
    }
    fVar2 = inT12->row[2].v.v[2];
    fVar3 = inT12->row[2].v.v[1];
    nPerp1.v[0] = fVar11 * fVar2 - nParaN2.v[2] * fVar3;
    nPerp1.v[1] = nParaN2.v[2] * fVar1 - fVar2 * nParaN2.v[0];
    local_2f0 = inT12->row[2].v.v[0];
    nPerp1.v[2] = nParaN2.v[0] * fVar3 - fVar11 * local_2f0;
    nPerpN2.v[0] = nPerp1.v[0];
    nPerpN2.v[1] = nPerp1.v[1];
    nPerpN2.v[2] = nPerp1.v[2];
    nParaN2.v[1] = fVar11;
  }
  else {
    nParaN2.v[0] = 0.0;
    nParaN2.v[2] = 0.0;
    nParaN2.v[1] = 1.0;
    nPerpN2.v[0] = 1.0;
    nPerpN2.v[1] = 0.0;
    nPerpN2.v[2] = 0.0;
    local_2f0 = fVar1;
    nPerp1.v[0] = local_2f4;
    nPerp1.v[1] = fVar11;
  }
  fVar11 = inT12->row[2].v.v[2];
  fVar2 = inT12->row[3].v.v[2];
  fVar3 = ABS(fVar2) - (ABS(fVar11) * inHH2 + ABS(nPerpN2.v[2]) * inR2 + inHH1);
  if (fVar2 <= 0.0) {
    normalSign = 1.0;
  }
  else {
    normalSign = -1.0;
  }
  PN = -inHH1 - fVar3;
  bVar13 = inEps < fVar3;
  normInfo = '\f';
  fVar7 = dispDotX1;
  fVar4 = dispDotY1;
  maxSeparation = fVar3;
  if (fVar3 <= inEps) {
    nParaN1.v[2] = 0.0;
    if (1e-15 <= fVar12 * fVar12 + fVar1 * fVar1) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
      fVar5 = SQRT(fVar1 * fVar1 + fVar12 * fVar12 + 0.0);
      nParaN1.v[0] = fVar12;
      nPerp1.v[0] = fVar1;
      if (fVar5 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
        fVar5 = 1.0 / fVar5;
        nParaN1.v[2] = fVar5 * 0.0;
        nParaN1.v[0] = fVar12 * fVar5;
        nPerp1.v[0] = fVar1 * fVar5;
      }
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
      nPerpN1.v[2] = 0.0;
      nPerp1.v[1] = -nParaN1.v[0];
      nPerp1.v[2] = 0.0;
      nPerpN1.v[0] = nPerp1.v[0];
      nPerpN1.v[1] = nPerp1.v[1];
      nParaN1.v[1] = nPerp1.v[0];
    }
    else {
      nParaN1.v[0] = inT12->row[1].v.v[0];
      nParaN1.v[1] = inT12->row[1].v.v[1];
      nParaN1.v[2] = inT12->row[1].v.v[2];
      nPerpN1.v[0] = inT12->row[0].v.v[0];
      nPerpN1.v[1] = inT12->row[0].v.v[1];
      nPerpN1.v[2] = inT12->row[0].v.v[2];
    }
    local_2fc = inT12->row[3].v.v[0];
    fVar6 = fVar11 * inT12->row[3].v.v[2] + local_2f4 * inT12->row[3].v.v[1] + local_2fc * local_2f0
    ;
    fVar5 = ABS(fVar6);
    maxSeparation =
         fVar5 - (ABS(fVar11) * inHH1 +
                  ABS(local_2f0 * nPerpN1.v[0] + local_2f4 * nPerpN1.v[1] + fVar11 * nPerpN1.v[2]) *
                  inR1 + inHH2);
    if (fVar3 < maxSeparation) {
      if (fVar6 <= 0.0) {
        normalSign = 1.0;
      }
      else {
        normalSign = -1.0;
      }
      PN = inHH2 - fVar5;
      normInfo = '\x03';
      bVar13 = inEps < maxSeparation;
      fVar3 = maxSeparation;
      if (bVar13) goto LAB_00012e26;
    }
    maxSeparation = fVar3;
    nAA.v[2] = 0.0;
    nAA.v[0] = fVar12;
    nAA.v[1] = fVar1;
    if (fVar1 * fVar1 + fVar12 * fVar12 < 1e-15) {
      nAA.v[1] = inT12->row[3].v.v[1];
      nAA.v[0] = local_2fc;
    }
    fVar1 = nAA.v[1] * nAA.v[1] + nAA.v[0] * nAA.v[0] + 0.0;
    if (1e-15 <= fVar1) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
      fVar1 = SQRT(fVar1);
      if (fVar1 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
        fVar1 = 1.0 / fVar1;
        nAA.v[0] = nAA.v[0] * fVar1;
        nAA.v[1] = nAA.v[1] * fVar1;
        nAA.v[2] = fVar1 * 0.0;
      }
      fVar1 = local_2fc * nAA.v[0] + nAA.v[1] * inT12->row[3].v.v[1] +
              nAA.v[2] * inT12->row[3].v.v[2];
      fVar12 = ABS(fVar1) - (inR1 + inR2);
      if (maxSeparation < fVar12) {
        if (fVar1 <= 0.0) {
          normalSign = 1.0;
        }
        else {
          normalSign = -1.0;
        }
        bVar13 = inEps < fVar12;
        PN = -inR1 - fVar12;
        normInfo = '\n';
        maxSeparation = fVar12;
        if (bVar13) goto LAB_00012e26;
      }
    }
    fVar3 = fVar11 * inT12->row[3].v.v[2] + local_2f4 * inT12->row[3].v.v[1] + local_2fc * local_2f0
    ;
    fVar1 = inT12->row[3].v.v[1];
    fVar12 = inT12->row[3].v.v[2];
    if (fVar3 - fVar2 * fVar11 <= 0.0) {
      dCap = fVar3 + inHH2;
      c.v[0] = inHH2 * local_2f0 + local_2fc;
      c.v[1] = inHH2 * local_2f4 + fVar1;
      c.v[2] = inHH2 * fVar11 + fVar12;
    }
    else {
      c.v[2] = fVar12 - inHH2 * fVar11;
      c.v[0] = local_2fc - inHH2 * local_2f0;
      c.v[1] = fVar1 - inHH2 * local_2f4;
      dCap = fVar3 - inHH2;
    }
    nPerp1.v[2] = inHH2 * fVar11;
    nPerp1.v[1] = inHH2 * local_2f4;
    nPerp1.v[0] = inHH2 * local_2f0;
    if (1e-08 < ABS(fVar11)) {
      p.v[0] = 0.0 - c.v[0];
      p.v[1] = 0.0 - c.v[1];
      fVar2 = dCap / fVar11 - c.v[2];
                    /* Unresolved local var: MeReal t@[???] */
      fVar2 = SQRT(p.v[0] * p.v[0] + p.v[1] * p.v[1] + fVar2 * fVar2);
      if (fVar2 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
        fVar2 = 1.0 / fVar2;
        p.v[0] = p.v[0] * fVar2;
        p.v[1] = p.v[1] * fVar2;
      }
      nW1.v[0] = inR2 * p.v[0];
      nW1.v[1] = inR2 * p.v[1];
      fVar2 = nW1.v[0] * nW1.v[0] + nW1.v[1] * nW1.v[1];
      nW1.v[2] = 0.0;
      if (1e-15 < fVar2) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
        fVar2 = SQRT(fVar2);
        if (fVar2 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
          fVar2 = 1.0 / fVar2;
          nW1.v[0] = nW1.v[0] * fVar2;
          nW1.v[1] = fVar2 * nW1.v[1];
        }
        fVar2 = inT12->row[2].v.v[1];
        fVar3 = inT12->row[2].v.v[2];
        nPerp1.v[0] = fVar2 * 0.0 - nW1.v[1] * fVar3;
        nPerp1.v[1] = fVar3 * nW1.v[0] - local_2f0 * 0.0;
        local_2f0 = inT12->row[2].v.v[0];
        nPerp1.v[2] = local_2f0 * nW1.v[1] - fVar2 * nW1.v[0];
        fVar2 = nPerp1.v[0] * nPerp1.v[0] + nPerp1.v[1] * nPerp1.v[1] + nPerp1.v[2] * nPerp1.v[2];
        nParaW2.v[0] = nPerp1.v[0];
        nParaW2.v[1] = nPerp1.v[1];
        nParaW2.v[2] = nPerp1.v[2];
        if (1e-15 < fVar2) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
          fVar2 = SQRT(fVar2);
          if (fVar2 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
            fVar2 = 1.0 / fVar2;
            nParaW2.v[0] = nPerp1.v[0] * fVar2;
            nParaW2.v[1] = nPerp1.v[1] * fVar2;
            nParaW2.v[2] = fVar2 * nPerp1.v[2];
          }
          fVar2 = inT12->row[2].v.v[2];
          fVar3 = inT12->row[2].v.v[1];
          nPerp1.v[0] = nParaW2.v[1] * fVar2 - nParaW2.v[2] * fVar3;
          nPerp1.v[1] = nParaW2.v[2] * local_2f0 - nParaW2.v[0] * fVar2;
          nPerp1.v[2] = nParaW2.v[0] * fVar3 - nParaW2.v[1] * inT12->row[2].v.v[0];
          local_2f0 = inT12->row[2].v.v[0];
          fVar5 = nW1.v[0] * local_2fc + nW1.v[1] * inT12->row[3].v.v[1] +
                  inT12->row[3].v.v[2] * 0.0;
          fVar2 = ABS(fVar5) -
                  (ABS(local_2f0 * nW1.v[0] + fVar3 * nW1.v[1] + fVar2 * 0.0) * inHH2 +
                   ABS(nPerp1.v[2] * 0.0 + nPerp1.v[0] * nW1.v[0] + nPerp1.v[1] * nW1.v[1]) * inR2 +
                  inR1);
          nPerpW2.v[0] = nPerp1.v[0];
          nPerpW2.v[1] = nPerp1.v[1];
          nPerpW2.v[2] = nPerp1.v[2];
          if (maxSeparation < fVar2) {
            normalSign = 1.0;
            if (0.0 < fVar5) {
              normalSign = -1.0;
            }
            bVar13 = inEps < fVar2;
            PN = -inR1 - fVar2;
            normInfo = '\x06';
            maxSeparation = fVar2;
            if (bVar13) goto LAB_00012e26;
          }
        }
      }
    }
    if (fVar11 * dCap - fVar12 <= 0.0) {
      c.v[2] = inHH1;
    }
    else {
      c.v[2] = -inHH1;
    }
    if (1e-08 < ABS(fVar11)) {
                    /* Unresolved local var: MeReal t@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[DW_OP_reg14(ST3)] */
      fVar2 = (c.v[2] - fVar12) / fVar11;
      nPerp1.v[0] = local_2f0 * fVar2 + local_2fc;
      nPerp1.v[1] = local_2f4 * fVar2 + inT12->row[3].v.v[1];
      nPerp1.v[2] = fVar2 * fVar11 + inT12->row[3].v.v[2];
      p.v[0] = nPerp1.v[0] - 0.0;
      p.v[1] = nPerp1.v[1] - 0.0;
      p.v[2] = nPerp1.v[2] - c.v[2];
                    /* Unresolved local var: MeReal t@[???] */
      fVar2 = SQRT(p.v[0] * p.v[0] + p.v[1] * p.v[1] + p.v[2] * p.v[2]);
      if (fVar2 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
        fVar2 = 1.0 / fVar2;
        p.v[0] = p.v[0] * fVar2;
        p.v[1] = p.v[1] * fVar2;
        p.v[2] = fVar2 * p.v[2];
      }
      fVar2 = fVar11 * inR1 * p.v[2] + local_2f0 * inR1 * p.v[0] + local_2f4 * inR1 * p.v[1];
      nW2.v[1] = inR1 * p.v[1] - local_2f4 * fVar2;
      nW2.v[0] = inR1 * p.v[0] - local_2f0 * fVar2;
      nW2.v[2] = inR1 * p.v[2] - fVar2 * fVar11;
      fVar2 = nW2.v[0] * nW2.v[0] + nW2.v[1] * nW2.v[1] + nW2.v[2] * nW2.v[2];
      if (1e-15 < fVar2) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
        fVar2 = SQRT(fVar2);
        if (fVar2 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
          fVar2 = 1.0 / fVar2;
          nW2.v[0] = nW2.v[0] * fVar2;
          nW2.v[1] = nW2.v[1] * fVar2;
          nW2.v[2] = fVar2 * nW2.v[2];
        }
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
        nParaW1.v[0] = -nW2.v[1];
        fVar2 = nParaW1.v[0] * nParaW1.v[0] + nW2.v[0] * nW2.v[0];
        nParaW1.v[1] = nW2.v[0];
        nParaW1.v[2] = 0.0;
        if (1e-15 < fVar2) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
          fVar2 = SQRT(fVar2);
          if (fVar2 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
            fVar2 = 1.0 / fVar2;
            nParaW1.v[0] = nParaW1.v[0] * fVar2;
            nParaW1.v[1] = fVar2 * nW2.v[0];
          }
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
          nPara1.v[1] = -nParaW1.v[0];
          nPara1.v[0] = nParaW1.v[1];
          nPerpW1.v[0] = nParaW1.v[1];
          nPerpW1.v[2] = 0.0;
          nPara1.v[2] = 0.0;
          fVar2 = local_2fc * nW2.v[0] + nW2.v[1] * inT12->row[3].v.v[1] +
                  nW2.v[2] * inT12->row[3].v.v[2];
          fVar3 = ABS(fVar2);
          fVar5 = fVar3 - (ABS(nW2.v[2]) * inHH1 +
                           ABS(nParaW1.v[1] * nW2.v[0] + nPara1.v[1] * nW2.v[1]) * inR1 + inR2);
          nPerpW1.v[1] = nPara1.v[1];
          if (maxSeparation < fVar5) {
            normalSign = 1.0;
            if (0.0 < fVar2) {
              normalSign = -1.0;
            }
            PN = inR2 - fVar3;
            normInfo = '\t';
            bVar13 = inEps < fVar5;
            maxSeparation = fVar5;
            if (bVar13) goto LAB_00012e26;
          }
        }
      }
    }
    fVar3 = fVar11 * inT12->row[3].v.v[2] + local_2f4 * inT12->row[3].v.v[1] + local_2fc * local_2f0
    ;
    fVar2 = inT12->row[3].v.v[2];
    if (fVar3 - fVar12 * fVar11 <= 0.0) {
      dCap = fVar3 + inHH2;
      c.v[0] = inHH2 * local_2f0 + local_2fc;
      c.v[1] = inHH2 * local_2f4 + fVar1;
    }
    else {
      c.v[0] = local_2fc - inHH2 * local_2f0;
      c.v[1] = fVar1 - inHH2 * local_2f4;
      dCap = fVar3 - inHH2;
    }
    fVar12 = local_2f0 * -c.v[0] + local_2f4 * -c.v[1];
    p.v[0] = -c.v[0] - local_2f0 * fVar12;
    p.v[1] = -c.v[1] - local_2f4 * fVar12;
    p.v[2] = 0.0 - fVar12 * fVar11;
    fVar12 = p.v[0] * p.v[0] + p.v[1] * p.v[1] + p.v[2] * p.v[2];
    if (1e-15 < fVar12) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
      fVar12 = SQRT(fVar12);
      if (fVar12 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
        fVar12 = 1.0 / fVar12;
        p.v[0] = p.v[0] * fVar12;
        p.v[1] = p.v[1] * fVar12;
        p.v[2] = fVar12 * p.v[2];
      }
      nPerpA2E1.v[0] = p.v[0];
      nPerpA2E1.v[1] = p.v[1];
      nPerpA2E1.v[2] = p.v[2];
      fVar12 = inT12->row[2].v.v[1];
      fVar3 = inT12->row[2].v.v[2];
      nPara1.v[0] = fVar12 * p.v[2] - fVar3 * p.v[1];
      nPara1.v[1] = fVar3 * p.v[0] - p.v[2] * local_2f0;
      local_2f0 = inT12->row[2].v.v[0];
      nPara1.v[2] = local_2f0 * p.v[1] - fVar12 * p.v[0];
      nA1E2.v[0] = p.v[0] * inR2 + c.v[0];
      nA1E2.v[1] = c.v[1] + p.v[1] * inR2;
      fVar12 = nA1E2.v[0] * nA1E2.v[0] + nA1E2.v[1] * nA1E2.v[1];
      nA1E2.v[2] = 0.0;
      nParaA2E1.v[0] = nPara1.v[0];
      nParaA2E1.v[1] = nPara1.v[1];
      nParaA2E1.v[2] = nPara1.v[2];
      if (1e-15 < fVar12) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
        fVar12 = SQRT(fVar12);
        if (fVar12 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
          fVar12 = 1.0 / fVar12;
          nA1E2.v[0] = nA1E2.v[0] * fVar12;
          nA1E2.v[1] = fVar12 * nA1E2.v[1];
        }
        local_2cc = ABS(inT12->row[2].v.v[2] * 0.0 +
                        inT12->row[2].v.v[1] * nA1E2.v[1] + local_2f0 * nA1E2.v[0]);
        fVar3 = local_2fc * nA1E2.v[0] + nA1E2.v[1] * inT12->row[3].v.v[1] +
                inT12->row[3].v.v[2] * 0.0;
        fVar12 = ABS(fVar3) -
                 (inR1 + ABS(p.v[0] * nA1E2.v[0] + p.v[1] * nA1E2.v[1] + p.v[2] * 0.0) * inR2 +
                         local_2cc * inHH2);
        if (maxSeparation < fVar12) {
          if (fVar3 <= 0.0) {
            normalSign = 1.0;
          }
          else {
            normalSign = -1.0;
          }
          bVar13 = inEps < fVar12;
          PN = -inR1 - fVar12;
          normInfo = '\b';
          maxSeparation = fVar12;
          if (bVar13) goto LAB_00012e26;
        }
      }
    }
    if (fVar11 * dCap - fVar2 <= 0.0) {
      c.v[2] = inHH1;
    }
    else {
      c.v[2] = -inHH1;
    }
    fVar12 = local_2fc * local_2f0 + fVar1 * local_2f4 + (fVar2 - c.v[2]) * fVar11;
    p.v[0] = local_2fc - local_2f0 * fVar12;
    p.v[1] = fVar1 - local_2f4 * fVar12;
    fVar12 = p.v[0] * p.v[0] + p.v[1] * p.v[1];
    if (1e-15 < fVar12) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
      fVar12 = SQRT(fVar12);
      if (fVar12 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
        fVar12 = 1.0 / fVar12;
        p.v[0] = p.v[0] * fVar12;
        p.v[1] = fVar12 * p.v[1];
      }
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
      nPerpA1E2.v[0] = p.v[0];
      nPerpA1E2.v[1] = p.v[1];
      nParaA1E2.v[1] = p.v[0];
      fVar5 = p.v[1] * inR1 + (0.0 - fVar1);
      fVar3 = p.v[0] * inR1 + (0.0 - local_2fc);
      nPerpA1E2.v[2] = 0.0;
      fVar1 = inR1 * 0.0 + (c.v[2] - fVar2);
      nParaA1E2.v[0] = -p.v[1];
      nParaA1E2.v[2] = 0.0;
      fVar12 = local_2f0 * fVar3 + local_2f4 * fVar5 + fVar11 * fVar1;
      nPara1.v[1] = local_2f4 * fVar12;
      nPara1.v[0] = local_2f0 * fVar12;
      nA2E1.v[1] = fVar5 - nPara1.v[1];
      nPara1.v[2] = fVar12 * fVar11;
      nA2E1.v[0] = fVar3 - nPara1.v[0];
      nA2E1.v[2] = fVar1 - nPara1.v[2];
      fVar1 = nA2E1.v[0] * nA2E1.v[0] + nA2E1.v[1] * nA2E1.v[1] + nA2E1.v[2] * nA2E1.v[2];
      if (1e-15 < fVar1) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
        fVar1 = SQRT(fVar1);
        if (fVar1 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
          fVar1 = 1.0 / fVar1;
          nA2E1.v[0] = nA2E1.v[0] * fVar1;
          nA2E1.v[1] = nA2E1.v[1] * fVar1;
          nA2E1.v[2] = fVar1 * nA2E1.v[2];
        }
        fVar1 = local_2fc * nA2E1.v[0] + nA2E1.v[1] * inT12->row[3].v.v[1] +
                nA2E1.v[2] * inT12->row[3].v.v[2];
        fVar12 = ABS(fVar1);
        fVar2 = fVar12 - (ABS(p.v[0] * nA2E1.v[0] + p.v[1] * nA2E1.v[1] + nA2E1.v[2] * 0.0) * inR1 +
                          ABS(nA2E1.v[2]) * inHH1 + inR2);
        if (maxSeparation < fVar2) {
          if (fVar1 <= 0.0) {
            normalSign = 1.0;
          }
          else {
            normalSign = -1.0;
          }
          PN = inR2 - fVar12;
          normInfo = '\x02';
          bVar13 = inEps < fVar2;
          maxSeparation = fVar2;
          if (bVar13) goto LAB_00012e26;
        }
      }
    }
    z2 = -inHH2;
    cVar10 = '\0';
    cVar9 = '\0';
    minAxis1 = '\0';
    fVar1 = 3.4028235e+38;
    local_2f8 = fVar11;
    while( true ) {
      fVar7 = z2 * local_2f0 + local_2fc;
      fVar4 = z2 * local_2f4 + inT12->row[3].v.v[1];
      fVar3 = z2 * local_2f8 + inT12->row[3].v.v[2];
                    /* Unresolved local var: MeReal capD@[???] */
      fVar2 = fVar3 - -inHH1;
      fVar12 = fVar7 * fVar7 + fVar4 * fVar4;
      fVar2 = fVar2 * fVar2 + fVar12;
      if (fVar2 < fVar1) {
        minAxis1 = '\0';
        fVar1 = fVar2;
        cVar10 = cVar9;
      }
      fVar3 = fVar3 - (-inHH1 + inHH1 + inHH1);
      fVar12 = fVar3 * fVar3 + fVar12;
      if (fVar12 < fVar1) {
        minAxis1 = '\x01';
        fVar1 = fVar12;
        cVar10 = cVar9;
      }
      cVar9 = cVar9 + '\x01';
      z2 = z2 + inHH2 + inHH2;
      if ('\x01' < cVar9) break;
      local_2f4 = inT12->row[2].v.v[1];
      local_2f0 = inT12->row[2].v.v[0];
      local_2f8 = inT12->row[2].v.v[2];
      local_2fc = inT12->row[3].v.v[0];
    }
    fVar5 = inHH1 * (float)(int)((uint)(byte)minAxis1 * 2 + -1);
    fVar6 = inHH2 * (float)(cVar10 * 2 + -1);
    X1DotX2 = inT12->row[0].v.v[0];
    X1DotY2 = inT12->row[1].v.v[0];
    Y1DotX2 = inT12->row[0].v.v[1];
    Y1DotY2 = inT12->row[1].v.v[1];
    cylR1 = inR1;
    cylR2 = inR2;
    fVar1 = inT12->row[2].v.v[0];
    fVar12 = inT12->row[2].v.v[1];
    fVar2 = inT12->row[2].v.v[2];
    nPerp1.v[0] = fVar6 * fVar1;
    nPerp1.v[1] = fVar6 * fVar12;
    nPerp1.v[2] = fVar6 * fVar2;
    fVar3 = inT12->row[3].v.v[0];
    dispDotY1 = nPerp1.v[1] + inT12->row[3].v.v[1];
    dispDotX1 = nPerp1.v[0] + fVar3;
    nPara1.v[2] = nPerp1.v[2] + inT12->row[3].v.v[2];
    if (1.0 <= ABS(fVar11)) {
      fVar11 = 0.0;
    }
    else {
      fVar11 = SQRT(1.0 - fVar11 * fVar11);
    }
    nPara1.v[0] = dispDotX1;
    nPara1.v[1] = dispDotY1;
    if ((((ABS(fVar5) <= ABS(nPara1.v[2])) &&
         (fVar8 = nPara1.v[2] - fVar5, inR2 * fVar11 <= ABS(fVar8))) &&
        (fVar1 = (0.0 - fVar3) * fVar1 + (0.0 - inT12->row[3].v.v[1]) * fVar12 +
                 (fVar5 - inT12->row[3].v.v[2]) * fVar2, ABS(fVar6) <= ABS(fVar1))) &&
       (inR1 * fVar11 <= ABS(fVar1 - fVar6))) {
      dispDotX2 = fVar8 * inT12->row[0].v.v[2] +
                  dispDotY1 * inT12->row[0].v.v[1] + dispDotX1 * inT12->row[0].v.v[0];
      dispDotY2 = fVar8 * inT12->row[1].v.v[2] +
                  dispDotX1 * inT12->row[1].v.v[0] + dispDotY1 * inT12->row[1].v.v[1];
      theta[1] = 0.0;
      theta[0] = 0.0;
      FnMinNd(2,theta,0.001,0.001,EndCapMin,EndCapMinGrad);
                    /* Unresolved local var: float __result@[???] */
      lVar14 = (longdouble)fcos((longdouble)theta[0]);
                    /* Unresolved local var: float __result@[???] */
      lVar15 = (longdouble)fsin((longdouble)theta[0]);
      lVar16 = (longdouble)fcos((longdouble)theta[1]);
      lVar16 = lVar16 * (longdouble)inR2;
      lVar17 = lVar16 * (longdouble)inT12->row[0].v.v[0];
      lVar18 = lVar16 * (longdouble)inT12->row[0].v.v[1];
      lVar16 = lVar16 * (longdouble)inT12->row[0].v.v[2];
      vCross_3.v[0] = (MeReal)lVar17;
      vCross_3.v[1] = (MeReal)lVar18;
      vCross_3.v[2] = (MeReal)lVar16;
      fVar1 = inT12->row[3].v.v[0];
                    /* Unresolved local var: float __result@[???] */
      lVar19 = (longdouble)fsin((longdouble)theta[1]);
      lVar19 = lVar19 * (longdouble)inR2;
      lVar20 = lVar19 * (longdouble)inT12->row[1].v.v[0];
      lVar21 = lVar19 * (longdouble)inT12->row[1].v.v[1];
      lVar19 = lVar19 * (longdouble)inT12->row[1].v.v[2];
      nPerp2.v[1] = (MeReal)lVar21;
      nPerp2.v[2] = (MeReal)lVar19;
      nPerp2.v[0] = (MeReal)lVar20;
      fVar11 = inT12->row[2].v.v[0];
      lVar22 = (longdouble)fVar6 * (longdouble)inT12->row[2].v.v[2];
      lVar23 = (longdouble)fVar6 * (longdouble)inT12->row[2].v.v[1];
      lVar19 = (longdouble)(float)(lVar16 + (longdouble)inT12->row[3].v.v[2]) + lVar19 + lVar22;
      lVar24 = (longdouble)fVar6 * (longdouble)fVar11;
      lVar21 = lVar18 + (longdouble)inT12->row[3].v.v[1] + lVar21 + lVar23;
      nPara2.v[2] = (MeReal)lVar22;
      lVar20 = lVar17 + (longdouble)fVar1 + lVar20 + lVar24;
      lVar16 = lVar19 - (longdouble)fVar5;
      lVar17 = lVar20 - lVar14 * (longdouble)inR1;
      lVar18 = lVar21 - lVar15 * (longdouble)inR1;
                    /* Unresolved local var: MeReal t@[???] */
      nPara2.v[0] = (MeReal)lVar24;
      nPara2.v[1] = (MeReal)lVar23;
      lVar22 = SQRT(lVar18 * lVar18 + lVar17 * lVar17 + lVar16 * lVar16);
      if (lVar22 != (longdouble)0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
        lVar22 = (longdouble)1 / lVar22;
        lVar17 = lVar17 * lVar22;
        lVar18 = lVar18 * lVar22;
        lVar16 = lVar22 * lVar16;
      }
      nE1E2.v[2] = (MeReal)lVar16;
      nE1E2.v[1] = (MeReal)lVar18;
      nE1E2.v[0] = (MeReal)lVar17;
      nPerp1.v[2] = (float)lVar19 + fVar5;
      nPerp1.v[1] = (float)lVar21 + (float)(lVar15 * (longdouble)inR1);
      pE1E2.v[2] = nPerp1.v[2] * 0.5;
      nPerp1.v[0] = (float)lVar20 + (float)(lVar14 * (longdouble)inR1);
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
      pE1E2.v[1] = nPerp1.v[1] * 0.5;
      fVar12 = -nE1E2.v[1];
      pE1E2.v[0] = nPerp1.v[0] * 0.5;
      if (1e-15 <= fVar12 * fVar12 + nE1E2.v[0] * nE1E2.v[0]) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
        fVar3 = SQRT(nE1E2.v[0] * nE1E2.v[0] + fVar12 * fVar12 + 0.0);
        fVar2 = nE1E2.v[0];
        if (fVar3 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
          fVar3 = 1.0 / fVar3;
          fVar12 = fVar12 * fVar3;
          fVar2 = nE1E2.v[0] * fVar3;
        }
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
        rc1 = ABS(-fVar12 * nE1E2.v[1] + fVar2 * nE1E2.v[0]) * inR1 + ABS(nE1E2.v[2]) * inHH1;
      }
      else {
        rc1 = inHH1;
      }
      fVar12 = inT12->row[2].v.v[1];
      fVar2 = inT12->row[2].v.v[2];
      fVar3 = fVar12 * nE1E2.v[2] - nE1E2.v[1] * fVar2;
      fVar2 = fVar2 * nE1E2.v[0] - fVar11 * nE1E2.v[2];
      fVar11 = inT12->row[2].v.v[0];
      nPara1.v[2] = fVar11 * nE1E2.v[1] - fVar12 * nE1E2.v[0];
      fVar12 = inHH2;
      nPara1.v[0] = fVar3;
      nPara1.v[1] = fVar2;
      if (1e-15 <= fVar3 * fVar3 + fVar2 * fVar2 + nPara1.v[2] * nPara1.v[2]) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
        fVar12 = SQRT(nPara1.v[2] * nPara1.v[2] + fVar2 * fVar2 + fVar3 * fVar3);
        if (fVar12 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
          fVar12 = 1.0 / fVar12;
          fVar3 = fVar3 * fVar12;
          fVar2 = fVar2 * fVar12;
          nPara1.v[2] = fVar12 * nPara1.v[2];
        }
        fVar12 = inT12->row[2].v.v[2];
        fVar4 = inT12->row[2].v.v[1];
        nPara1.v[0] = fVar2 * fVar12 - nPara1.v[2] * fVar4;
        nPara1.v[1] = nPara1.v[2] * fVar11 - fVar3 * fVar12;
        nPara1.v[2] = fVar3 * fVar4 - fVar2 * inT12->row[2].v.v[0];
        fVar12 = ABS(nPara1.v[2] * nE1E2.v[2] + nPara1.v[0] * nE1E2.v[0] + nPara1.v[1] * nE1E2.v[1])
                 * inR2 + ABS(nE1E2.v[0] * inT12->row[2].v.v[0] + fVar4 * nE1E2.v[1] +
                              fVar12 * nE1E2.v[2]) * inHH2;
      }
      fVar2 = fVar1 * nE1E2.v[0] + nE1E2.v[1] * inT12->row[3].v.v[1] +
              nE1E2.v[2] * inT12->row[3].v.v[2];
      fVar1 = ABS(fVar2);
      fVar11 = fVar1 - (rc1 + fVar12);
      fVar7 = dispDotX1;
      fVar4 = dispDotY1;
      if (maxSeparation < fVar11) {
        if (fVar2 <= 0.0) {
          normalSign = 1.0;
        }
        else {
          normalSign = -1.0;
        }
        PN = fVar12 - fVar1;
        normInfo = minAxis1 | cVar10 << 2;
        bVar13 = inEps < fVar11;
        maxSeparation = fVar11;
      }
    }
  }
LAB_00012e26:
  dispDotY1 = fVar4;
  dispDotX1 = fVar7;
  *outSep = maxSeparation;
  *outPN = PN;
  if (bVar13) {
    return false;
  }
  if (normInfo == '\f') {
    para = &nPara1;
    outN->v[0] = 0.0;
    outN->v[1] = 0.0;
    outN->v[2] = normalSign;
    *outDims = 2;
    nPara2.v[2] = nParaN2.v[2];
    nPara2.v[0] = nParaN2.v[0];
    nPerp2.v[0] = nPerpN2.v[0];
    nPerp2.v[1] = nPerpN2.v[1];
    n_00 = inT12->row + 2;
    nPara2.v[1] = nParaN2.v[1];
    n = (lsTransformRow *)&vCross_3;
    nPerp2.v[2] = nPerpN2.v[2];
    plVar25 = &nPerp1;
  }
  else {
    if (normInfo != '\x03') {
      cVar10 = (char)(normInfo & 0xcU) >> 2;
      if ((normInfo & 3U) == 2) {
        if (cVar10 == '\x02') {
          outN->v[0] = normalSign * nAA.v[0];
          outN->v[1] = normalSign * nAA.v[1];
          outN->v[2] = normalSign * nAA.v[2];
          *outDims = 0x303;
          n = inT12->row + 2;
          vCross_3.v[0] = 0.0;
          vCross_3.v[1] = 0.0;
          vCross_3.v[2] = 1.0;
          CylPerpAndPara(&nPerp1,&nPara1,&vCross_3,(lsVec3 *)n,(lsVec3 *)(inT12->row + 3));
          n_00 = (lsTransformRow *)&vCross_3;
          plVar25 = &nPerp2;
          para = &nPara2;
          goto LAB_000133e8;
        }
        if (cVar10 == '\0') {
          nPerp1.v[2] = nPerpA1E2.v[2];
          nPerp1.v[0] = nPerpA1E2.v[0];
          nPara1.v[0] = nParaA1E2.v[0];
          nPerp1.v[1] = nPerpA1E2.v[1];
          nPara1.v[1] = nParaA1E2.v[1];
          nPara1.v[2] = nParaA1E2.v[2];
          CylPerpAndPara(&nPerp2,&nPara2,(lsVec3 *)(inT12->row + 2),&nA2E1,
                         (lsVec3 *)(inT12->row + 3));
          vCross_3.v[0] = normalSign * nA2E1.v[0];
          vCross_3.v[1] = normalSign * nA2E1.v[1];
          vCross_3.v[2] = normalSign * nA2E1.v[2];
          outN->v[0] = vCross_3.v[0];
          outN->v[1] = vCross_3.v[1];
          outN->v[2] = vCross_3.v[2];
          *outDims = 0x301;
          goto LAB_00013034;
        }
        nPerp1.v[1] = nW1.v[1];
        nPara1.v[1] = nW1.v[0];
        nPerp1.v[2] = nW1.v[2];
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
        nPara1.v[0] = -nW1.v[1];
        nPerp2.v[0] = nPerpW2.v[0];
        nPerp2.v[1] = nPerpW2.v[1];
        nPara1.v[2] = 0.0;
        nPerp2.v[2] = nPerpW2.v[2];
        nPara2.v[0] = nParaW2.v[0];
        nPara2.v[1] = nParaW2.v[1];
        nPara2.v[2] = nParaW2.v[2];
        nPerp1.v[0] = nPara1.v[1];
        nW2.v[0] = nW1.v[0];
        nW2.v[1] = nW1.v[1];
        nW2.v[2] = nW1.v[2];
      }
      else {
        if (cVar10 != '\x02') {
          outN->v[0] = normalSign * nE1E2.v[0];
          outN->v[1] = normalSign * nE1E2.v[1];
          outN->v[2] = normalSign * nE1E2.v[2];
          plVar25 = *outPos;
          *outPos = plVar25 + 1;
          plVar25->v[0] = pE1E2.v[0];
          plVar25->v[1] = pE1E2.v[1];
          plVar25->v[2] = pE1E2.v[2];
          *outDims = 0;
          return true;
        }
        if ((normInfo & 3U) == 0) {
          vCross_3.v[0] = 0.0;
          vCross_3.v[1] = 0.0;
          vCross_3.v[2] = 1.0;
          CylPerpAndPara(&nPerp1,&nPara1,&vCross_3,&nA1E2,(lsVec3 *)(inT12->row + 3));
          nPerp2.v[2] = nPerpA2E1.v[2];
          vCross_3.v[0] = normalSign * nA1E2.v[0];
          nPerp2.v[0] = nPerpA2E1.v[0];
          nPerp2.v[1] = nPerpA2E1.v[1];
          vCross_3.v[1] = normalSign * nA1E2.v[1];
          vCross_3.v[2] = normalSign * nA1E2.v[2];
          nPara2.v[0] = nParaA2E1.v[0];
          nPara2.v[1] = nParaA2E1.v[1];
          nPara2.v[2] = nParaA2E1.v[2];
          outN->v[0] = vCross_3.v[0];
          outN->v[1] = vCross_3.v[1];
          outN->v[2] = vCross_3.v[2];
          *outDims = 0x103;
          goto LAB_00013034;
        }
        nPerp1.v[0] = nPerpW1.v[0];
        nPerp1.v[1] = nPerpW1.v[1];
        nPerp1.v[2] = nPerpW1.v[2];
        nPerp2.v[1] = nW2.v[1];
        nPerp2.v[2] = nW2.v[2];
        nPerp2.v[0] = nW2.v[0];
        nPara1.v[0] = nParaW1.v[0];
        nPara1.v[1] = nParaW1.v[1];
        nPara1.v[2] = nParaW1.v[2];
        fVar1 = inT12->row[2].v.v[1];
        fVar11 = inT12->row[2].v.v[2];
        nPara2.v[0] = fVar1 * nW2.v[2] - fVar11 * nW2.v[1];
        nPara2.v[1] = fVar11 * nW2.v[0] - inT12->row[2].v.v[0] * nW2.v[2];
        nPara2.v[2] = inT12->row[2].v.v[0] * nW2.v[1] - fVar1 * nW2.v[0];
      }
      vCross_3.v[2] = nW2.v[2] * normalSign;
      vCross_3.v[1] = nW2.v[1] * normalSign;
      vCross_3.v[0] = nW2.v[0] * normalSign;
      outN->v[0] = vCross_3.v[0];
      outN->v[1] = vCross_3.v[1];
      outN->v[2] = vCross_3.v[2];
      *outDims = 0x303;
      goto LAB_00013034;
    }
    fVar1 = inT12->row[2].v.v[1];
    n_00 = (lsTransformRow *)&vCross_3;
    n = inT12->row + 2;
    fVar11 = inT12->row[2].v.v[2];
    para = &nPara2;
    outN->v[0] = normalSign * inT12->row[2].v.v[0];
    outN->v[1] = normalSign * fVar1;
    outN->v[2] = normalSign * fVar11;
    *outDims = 0x200;
    nPara1.v[0] = nParaN1.v[0];
    nPara1.v[2] = nParaN1.v[2];
    nPara1.v[1] = nParaN1.v[1];
    nPerp1.v[1] = nPerpN1.v[1];
    nPerp1.v[0] = nPerpN1.v[0];
    nPerp1.v[2] = nPerpN1.v[2];
    plVar25 = &nPerp2;
  }
LAB_000133e8:
  vCross_3.v[2] = 1.0;
  vCross_3.v[1] = 0.0;
  vCross_3.v[0] = 0.0;
  CylPerpAndPara(plVar25,para,(lsVec3 *)n,(lsVec3 *)n_00,(lsVec3 *)(inT12->row + 3));
LAB_00013034:
  CylCylIntersect(outPos,inT12,inR1,inHH1,&nPerp1,&nPara1,true,inR2,inHH2,&nPerp2,&nPara2,true,scale
                 );
  return true;
}


/* ==== McdCylinderCylinderIntersect ==== */

/* WARNING: Unknown calling convention */

MeBool McdCylinderCylinderIntersect(McdModelPair *p,McdIntersectResult *result)

{
  McdFramework *pMVar1;
  bool bVar2;
  float *pfVar3;
  float *pfVar4;
  void *pvVar5;
  void *pvVar6;
  int iVar7;
  McdContact *pMVar8;
  uint uVar9;
  lsVec3 *plVar10;
  float fVar11;
  float fVar12;
  float inR1;
  float inHH1;
  float inR2;
  float inHH2;
  MeReal rCylB;
  MeReal hhCylA;
  MeReal rCylA;
  McdFramework *fwk;
  McdCylinderID geometry2;
  MeReal eps;
  MeReal separation;
  MeReal PN;
  lsVec3 *verts;
  MeI16 dims;
  lsVec3 footprint [48];
  lsVec3 normal;
  lsTransform tAB;
  
                    /* Unresolved local var: lsTransform * tA@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * tB@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdCylinderID geometry1@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal hhCylB@[DW_OP_reg1(ECX)]
                       Unresolved local var: lsVec3 * v@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdContact * c@[DW_OP_reg1(ECX)] */
  pfVar3 = McdModelGetTransformPtr(p->model1);
  pfVar4 = McdModelGetTransformPtr(p->model2);
  fVar11 = McdModelGetContactTolerance(p->model1);
  fVar12 = McdModelGetContactTolerance(p->model2);
  pvVar5 = McdModelGetGeometry(p->model1);
  pvVar6 = McdModelGetGeometry(p->model2);
  pMVar1 = p->model1->frame;
  result->touch = 0;
  result->contactCount = 0;
  inR1 = McdCylinderGetRadius(pvVar5);
  inHH1 = McdCylinderGetHalfHeight(pvVar5);
  inR2 = McdCylinderGetRadius(pvVar6);
  inHH2 = McdCylinderGetHalfHeight(pvVar6);
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
  plVar10 = footprint;
  verts = plVar10;
  bVar2 = OverlapCylCyl(&separation,&normal,&PN,&verts,&dims,fVar12 + fVar11,inR1,inHH1,inR2,inHH2,
                        &tAB,pMVar1->mScale);
  uVar9 = 0;
  if (bVar2) {
    result->normal[0] = normal.v[2] * pfVar3[8] + normal.v[1] * pfVar3[4] + normal.v[0] * *pfVar3;
    result->normal[1] = normal.v[2] * pfVar3[9] + normal.v[0] * pfVar3[1] + normal.v[1] * pfVar3[5];
    result->normal[2] = normal.v[2] * pfVar3[10] + normal.v[0] * pfVar3[2] + normal.v[1] * pfVar3[6]
    ;
    result->contactCount = 0;
    pMVar8 = result->contacts;
    if (footprint == verts) {
      iVar7 = result->contactCount;
    }
    else if (result->contactMaxCount < 1) {
      iVar7 = 0;
    }
    else {
      do {
                    /* Unresolved local var: MeReal s@[DW_OP_reg15(ST4)] */
        fVar11 = (normal.v[2] * plVar10->v[2] +
                 plVar10->v[0] * normal.v[0] + normal.v[1] * plVar10->v[1]) - PN;
        if (fVar11 < separation * 0.01) {
          pMVar8->position[0] =
               plVar10->v[0] * *pfVar3 + plVar10->v[1] * pfVar3[4] + plVar10->v[2] * pfVar3[8] +
               pfVar3[0xc];
          pMVar8->position[1] =
               pfVar3[9] * plVar10->v[2] + pfVar3[5] * plVar10->v[1] + pfVar3[1] * plVar10->v[0] +
               pfVar3[0xd];
          pMVar8->position[2] =
               pfVar3[10] * plVar10->v[2] + pfVar3[6] * plVar10->v[1] + pfVar3[2] * plVar10->v[0] +
               pfVar3[0xe];
          pMVar8->separation = fVar11;
          pMVar8->dims = dims;
          pMVar8->normal[0] = result->normal[0];
          pMVar8->normal[1] = result->normal[1];
          pMVar8->normal[2] = result->normal[2];
          result->contactCount = result->contactCount + 1;
          pMVar8 = pMVar8 + 1;
        }
        plVar10 = plVar10 + 1;
        if (plVar10 == verts) {
          iVar7 = result->contactCount;
          break;
        }
        iVar7 = result->contactCount;
      } while (iVar7 < result->contactMaxCount);
    }
    uVar9 = (uint)(0 < iVar7);
    result->touch = uVar9;
  }
  return uVar9;
}


/* ==== McdCylinderCylinderSafeTime ==== */

int McdCylinderCylinderSafeTime(McdModelPair *p,MeReal maxTime,McdSafeTimeResult *result)

{
  void *pvVar1;
  lsVec3 *plVar2;
  lsVec3 *plVar3;
  lsTransform *plVar4;
  lsTransform *plVar5;
  int iVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  MeReal radCyl2;
  MeReal hheightCyl1;
  MeReal radCyl1;
  lsVec3 *V1;
  lsVec3 *V0;
  MeReal T;
  lsVec3 P;
  MeReal ext2 [3];
  MeReal ext1 [3];
  
                    /* Unresolved local var: McdCylinderID geometry1@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdCylinderID geometry2@[???]
                       Unresolved local var: MeReal hheightCyl2@[DW_OP_reg6(ESI)]
                       Unresolved local var: lsTransform * tm1@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * tm2@[DW_OP_reg0(EAX)]
                       Unresolved local var: uint ixt@[DW_OP_reg2(EDX)]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
  result->pair = p;
  result->time = maxTime;
  pvVar1 = McdModelGetGeometry(p->model1);
  McdModelGetGeometry(p->model2);
  plVar2 = McdModelGetLinearVelocityPtr(p->model1);
  plVar3 = McdModelGetLinearVelocityPtr(p->model2);
  fVar7 = McdCylinderGetRadius(pvVar1);
  fVar8 = McdCylinderGetHalfHeight(pvVar1);
  fVar9 = McdCylinderGetRadius(pvVar1);
  fVar10 = McdCylinderGetHalfHeight(pvVar1);
                    /* Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
  plVar4 = McdModelGetTransformPtr(p->model1);
  plVar5 = McdModelGetTransformPtr(p->model2);
  ext2[0] = fVar9 * 0.8284271;
  ext2[1] = fVar9 * 0.8284271;
  ext2[2] = fVar10;
  ext1[0] = fVar7 * 0.8284271;
  ext1[1] = fVar7 * 0.8284271;
  ext1[2] = fVar8;
  iVar6 = MovingBoxBoxIntersect(ext1,plVar4,plVar2,ext2,plVar5,plVar3,maxTime,&T,(lsVec3 *)&P);
  if (iVar6 == 0) {
    result->time = maxTime;
  }
  else {
    result->time = T;
  }
  return iVar6;
}


/* ==== McdCylinderCylinderRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

MeBool McdCylinderCylinderRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdCylinderCylinderIntersect;
  interactions.safetimeFn = McdCylinderCylinderSafeTime;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,4,4,&interactions);
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
            goto LAB_000148b9;
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
LAB_000148b9:
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


/* ==== CylPerpAndPara ==== */

void CylPerpAndPara(lsVec3 *perp,lsVec3 *para,lsVec3 *axis,lsVec3 *n,lsVec3 *disp)

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
  uint uVar11;
  uint uVar12;
  sbyte local_34;
  int i;
  lsVec3 vCross;
  
  fVar1 = n->v[2] * axis->v[1] - n->v[1] * axis->v[2];
  fVar2 = n->v[0] * axis->v[2] - n->v[2] * axis->v[0];
  fVar3 = n->v[1] * axis->v[0] - n->v[0] * axis->v[1];
  para->v[0] = fVar1;
  para->v[1] = fVar2;
  para->v[2] = fVar3;
  if (fVar3 * fVar3 + fVar1 * fVar1 + fVar2 * fVar2 < 1e-15) {
    vCross.v[0] = disp->v[2] * axis->v[1] - disp->v[1] * axis->v[2];
    vCross.v[1] = disp->v[0] * axis->v[2] - disp->v[2] * axis->v[0];
    vCross.v[2] = disp->v[1] * axis->v[0] - disp->v[0] * axis->v[1];
    para->v[0] = vCross.v[0];
    para->v[1] = vCross.v[1];
    para->v[2] = vCross.v[2];
    if (vCross.v[2] * vCross.v[2] + vCross.v[0] * vCross.v[0] + vCross.v[1] * vCross.v[1] < 1e-15) {
      i = (int)(axis->v[1] < axis->v[0]);
      if (axis->v[2] < axis->v[i]) {
        i = 2;
      }
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
      uVar11 = 1 << (sbyte)i & 3;
      local_34 = (sbyte)uVar11;
      uVar12 = 1 << local_34 & 3;
      vCross.v[i] = 0.0;
      vCross.v[uVar11] = axis->v[uVar12];
      vCross.v[uVar12] = -axis->v[uVar11];
      para->v[0] = vCross.v[0];
      para->v[1] = vCross.v[1];
      para->v[2] = vCross.v[2];
    }
  }
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
  fVar1 = para->v[0];
  fVar2 = para->v[1];
  fVar3 = para->v[2];
  fVar4 = SQRT(fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1);
  if (fVar4 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
    fVar4 = 1.0 / fVar4;
    fVar1 = fVar1 * fVar4;
    fVar2 = fVar2 * fVar4;
    fVar3 = fVar4 * fVar3;
    para->v[0] = fVar1;
    para->v[1] = fVar2;
    para->v[2] = fVar3;
  }
  fVar4 = axis->v[0];
  fVar5 = axis->v[2];
  fVar6 = para->v[2];
  fVar7 = axis->v[1];
  fVar8 = axis->v[0];
  fVar9 = para->v[0];
  fVar10 = para->v[1];
  perp->v[0] = fVar2 * axis->v[2] - fVar3 * axis->v[1];
  perp->v[1] = fVar4 * fVar6 - fVar1 * fVar5;
  perp->v[2] = fVar7 * fVar9 - fVar8 * fVar10;
  return;
}


