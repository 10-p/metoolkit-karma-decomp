/* ==== McdConvexHullPlaneCut ==== */

void McdConvexHullPlaneCut
               (McdConvexHull *hull,MeReal *norm,MeReal dp,int flags,int maxVert,int *numVert,
               MeVector3 *outVert)

{
  MeVector3 *paMVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  McdCnvVertex *pMVar6;
  ushort uVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  byte bVar16;
  int iVar11;
  int *piVar12;
  int iVar13;
  McdCnvVertex *pMVar14;
  int *piVar15;
  McdCnvVertex *pMVar17;
  int iVar18;
  bool bVar19;
  MeReal MVar20;
  int m;
  int v;
  MeReal dv;
  McdCnvVertex *vert;
  int start;
  MeDictNode nodemem [200];
  MeSet set;
  MeReal temp [3];
  
                    /* Unresolved local var: MeReal du@[DW_OP_reg13(ST2)]
                       Unresolved local var: int nv@[???]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int u@[DW_OP_reg2(EDX)] */
  *numVert = 0;
  if ((0 < maxVert) &&
     (MVar20 = McdConvexHullMaximumPoint(hull,norm,0,3.4028235e+38,&start), dp <= MVar20)) {
    pMVar6 = hull->vertex;
    MeSetInit(&set,nodemem,200,0);
    MeSetAdd(&set,start);
    while (set.next != (MeDictNode *)0x0) {
      iVar11 = MeSetIteratorNext(&set);
      piVar12 = (int *)(iVar11 * 0x10);
      pMVar17 = pMVar6 + iVar11;
      fVar8 = pMVar17->position[2] * norm[2] +
              pMVar17->position[1] * norm[1] + *norm * pMVar17->position[0];
      if ((flags & 2U) != 0) {
        paMVar1 = outVert + *numVert;
        (*paMVar1)[0] = pMVar17->position[0];
        (*paMVar1)[1] = pMVar17->position[1];
        (*paMVar1)[2] = pMVar17->position[2];
        iVar18 = *numVert;
        *numVert = iVar18 + 1;
        piVar12 = numVert;
        if (iVar18 + 1 == maxVert) {
          return;
        }
      }
      piVar12 = (int *)McdCnvVertexGetCount(hull,iVar11,piVar12,piVar12);
      if (0 < (int)piVar12) {
        iVar18 = 0;
        piVar15 = piVar12;
        do {
          iVar13 = McdCnvVertexGetNeighbor(hull,iVar11,iVar18,piVar15);
          pMVar14 = pMVar6 + iVar13;
          fVar4 = pMVar14->position[2] * norm[2] +
                  pMVar14->position[1] * norm[1] + pMVar14->position[0] * *norm;
          piVar15 = (int *)CONCAT22((short)((uint)pMVar14 >> 0x10),
                                    (ushort)(fVar4 < dp) << 8 |
                                    (ushort)(NAN(fVar4) || NAN(dp)) << 10 |
                                    (ushort)(fVar4 == dp) << 0xe);
          if (fVar4 < dp) {
            if ((flags & 1U) != 0) {
              fVar9 = 1.0 / (fVar8 - fVar4);
              pMVar14 = pMVar6 + iVar13;
              fVar10 = (fVar8 - dp) * fVar9;
              fVar2 = pMVar14->position[1];
              fVar3 = pMVar14->position[2];
              fVar9 = (dp - fVar4) * fVar9;
              fVar4 = pMVar17->position[1];
              fVar5 = pMVar17->position[2];
              paMVar1 = outVert + *numVert;
              (*paMVar1)[0] = fVar9 * pMVar17->position[0] + pMVar14->position[0] * fVar10;
              (*paMVar1)[1] = fVar9 * fVar4 + fVar2 * fVar10;
              (*paMVar1)[2] = fVar9 * fVar5 + fVar3 * fVar10;
              iVar13 = *numVert;
              if (iVar13 != 0) {
                if (1e-06 <= ABS((outVert + iVar13)[-1][0])) {
                  fVar4 = (outVert + iVar13)[-1][0];
                  fVar4 = ABS((1.0 / fVar4) * (outVert[iVar13][0] - fVar4));
                  bVar16 = fVar4 < 1e-06 | (byte)((ushort)((ushort)NAN(fVar4) << 10) >> 8);
                }
                else {
                  fVar4 = ABS(outVert[iVar13][0]);
                  bVar16 = fVar4 < 1e-06 | (byte)((ushort)((ushort)NAN(fVar4) << 10) >> 8);
                }
                if (bVar16 != 0) {
                  if (1e-06 <= ABS(outVert[iVar13 + -1][1])) {
                    fVar4 = ABS((1.0 / outVert[iVar13 + -1][1]) *
                                (outVert[iVar13][1] - outVert[iVar13 + -1][1]));
                    bVar16 = fVar4 < 1e-06 | (byte)((ushort)((ushort)NAN(fVar4) << 10) >> 8);
                  }
                  else {
                    bVar16 = ABS(outVert[iVar13][1]) < 1e-06 |
                             (byte)((ushort)((ushort)NAN(ABS(outVert[iVar13][1])) << 10) >> 8);
                  }
                  if (bVar16 != 0) {
                    paMVar1 = outVert + iVar13;
                    if (1e-06 <= ABS(paMVar1[-1][2])) {
                      paMVar1 = outVert + iVar13;
                      fVar4 = ABS(((*paMVar1)[2] - paMVar1[-1][2]) * (1.0 / paMVar1[-1][2]));
                      piVar15 = (int *)CONCAT22((short)((uint)paMVar1 >> 0x10),
                                                (ushort)(fVar4 < 1e-06) << 8 |
                                                (ushort)NAN(fVar4) << 10 |
                                                (ushort)(fVar4 == 1e-06) << 0xe);
                      bVar19 = fVar4 >= 1e-06;
                    }
                    else {
                      fVar4 = ABS((*paMVar1)[2]);
                      uVar7 = (ushort)(fVar4 < 1e-06) << 8 | (ushort)NAN(fVar4) << 10;
                      piVar15 = (int *)CONCAT22((short)((uint)paMVar1 >> 0x10),
                                                uVar7 | (ushort)(fVar4 == 1e-06) << 0xe);
                      bVar19 = uVar7 == 0;
                    }
                    if (!bVar19) goto LAB_00010301;
                  }
                }
              }
              *numVert = iVar13 + 1;
              piVar15 = numVert;
              if (iVar13 + 1 == maxVert) {
                return;
              }
            }
          }
          else {
            piVar15 = (int *)MeSetAdd(&set,iVar13,pMVar6,pMVar6);
          }
LAB_00010301:
          iVar18 = iVar18 + 1;
        } while (iVar18 < (int)piVar12);
      }
    }
  }
  return;
}


/* ==== McdConvexMeshTriangleListIntersect ==== */

/* WARNING: Unknown calling convention */

MeBool McdConvexMeshTriangleListIntersect(McdModelPair *p,McdIntersectResult *result)

{
  float fVar1;
  code *pcVar2;
  uint uVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  int iVar18;
  McdConvexHull *pMVar19;
  McdConvexMesh *pMVar20;
  float *pfVar21;
  float *pfVar22;
  int iVar23;
  McdTriangleFlags MVar24;
  MeBool MVar25;
  int iVar26;
  longdouble lVar27;
  int aiStack_150 [3];
  MeReal aMStack_144 [2];
  McdModelID_conflict pMStack_13c;
  MeReal *apMStack_138 [3];
  undefined1 auStack_12c [12];
  float local_120;
  float local_11c;
  float local_118;
  float local_114;
  float local_110;
  float local_10c;
  float local_108;
  float local_104;
  float local_100;
  float local_fc;
  float local_f8;
  MeI32 i;
  MeReal *local_f0;
  McdConvexHull *convex;
  McdConvexMesh *convexMesh;
  MeReal eps;
  MeReal convexRadius;
  MeReal vectors [4] [3];
  McdUserTriangle tri;
  MeReal tmp_1 [3];
  MeReal convexCenterRelative [3];
  MeReal convexCenter [3];
  MeReal relTM [4] [4];
  
                    /* Unresolved local var: MeMatrix4Ptr tm1@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeMatrix4Ptr tm2@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdTriangleListID trilistGeom@[???]
                       Unresolved local var: McdTriangleList * triList@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeI32 count@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeReal n2@[???] */
  pMStack_13c = p->model1;
  aMStack_144[1] = 9.32354e-41;
  pfVar21 = (float *)McdModelGetTransformPtr();
  pMStack_13c = p->model2;
  aMStack_144[1] = 9.32564e-41;
  pfVar22 = (float *)McdModelGetTransformPtr();
  pMStack_13c = p->model1;
  aMStack_144[1] = 9.3276e-41;
  lVar27 = (longdouble)McdModelGetContactTolerance();
  eps = (MeReal)lVar27;
  pMStack_13c = p->model2;
  aMStack_144[1] = 9.33027e-41;
  lVar27 = (longdouble)McdModelGetContactTolerance();
  eps = (MeReal)(lVar27 + (longdouble)eps);
  pMStack_13c = p->model1;
  aMStack_144[1] = 9.33363e-41;
  convexMesh = (McdConvexMesh *)McdModelGetGeometry();
  convex = &convexMesh->mHull;
  pMStack_13c = p->model2;
  aMStack_144[1] = 9.33783e-41;
  iVar23 = McdModelGetGeometry();
  result->contactCount = 0;
  result->touch = 0;
  fVar1 = pfVar21[2] * pfVar22[2] + pfVar21[1] * pfVar22[1] + *pfVar21 * *pfVar22;
  fVar4 = pfVar21[6] * pfVar22[2] + pfVar21[5] * pfVar22[1] + pfVar21[4] * *pfVar22;
  fVar5 = pfVar21[10] * pfVar22[2] + pfVar21[9] * pfVar22[1] + pfVar21[8] * *pfVar22;
  fVar6 = pfVar21[2] * pfVar22[6] + pfVar21[1] * pfVar22[5] + *pfVar21 * pfVar22[4];
  fVar7 = pfVar21[6] * pfVar22[6] + pfVar21[5] * pfVar22[5] + pfVar21[4] * pfVar22[4];
  fVar8 = pfVar21[10] * pfVar22[6] + pfVar21[9] * pfVar22[5] + pfVar21[8] * pfVar22[4];
  fVar9 = pfVar21[2] * pfVar22[10] + pfVar21[1] * pfVar22[9] + *pfVar21 * pfVar22[8];
  fVar10 = pfVar21[6] * pfVar22[10] + pfVar21[5] * pfVar22[9] + pfVar21[4] * pfVar22[8];
  apMStack_138[1] = &convexRadius;
  fVar11 = pfVar21[10] * pfVar22[10] + pfVar21[9] * pfVar22[9] + pfVar21[8] * pfVar22[8];
  convexCenter[0] = pfVar22[0xc] - pfVar21[0xc];
  convexCenter[1] = pfVar22[0xd] - pfVar21[0xd];
  convexCenter[2] = pfVar22[0xe] - pfVar21[0xe];
  apMStack_138[0] = convexCenter;
  fVar12 = convexCenter[2] * pfVar21[2] + convexCenter[0] * *pfVar21 + convexCenter[1] * pfVar21[1];
  fVar14 = convexCenter[2] * pfVar21[6] +
           convexCenter[0] * pfVar21[4] + convexCenter[1] * pfVar21[5];
  fVar13 = convexCenter[0] * pfVar21[8] + convexCenter[1] * pfVar21[9] +
           convexCenter[2] * pfVar21[10];
  pMStack_13c = (McdModelID_conflict)convexMesh;
  aMStack_144[1] = 9.39641e-41;
  McdConvexMeshGetBSphere();
  fVar15 = convexCenter[0] - fVar12;
  fVar17 = convexCenter[1] - fVar14;
  fVar16 = convexCenter[2] - fVar13;
  convexCenterRelative[0] = fVar16 * fVar5 + fVar15 * fVar1 + fVar17 * fVar4;
  convexCenterRelative[1] = fVar15 * fVar6 + fVar17 * fVar7 + fVar16 * fVar8;
  convexCenterRelative[2] = fVar15 * fVar9 + fVar17 * fVar10 + fVar16 * fVar11;
  iVar18 = -(*(int *)(iVar23 + 0x28) * 0x18 + 0xfU & 0xfffffff0);
  *(undefined1 **)(iVar23 + 0x34) = auStack_12c + iVar18;
  *(int *)((int)&pMStack_13c + iVar18) = *(int *)(iVar23 + 0x28);
  *(MeReal *)((int)aMStack_144 + iVar18 + 4) = convexRadius;
  *(MeReal **)((int)aMStack_144 + iVar18) = convexCenterRelative;
  *(undefined4 *)((int)aiStack_150 + iVar18 + 8) = *(undefined4 *)(iVar23 + 0x34);
  *(McdModelPair **)((int)aiStack_150 + iVar18 + 4) = p;
  pcVar2 = *(code **)(iVar23 + 0x30);
  *(undefined4 *)((int)aiStack_150 + iVar18) = 0x106a1;
  i = (*pcVar2)();
  MVar25 = 0;
  if (i != 0) {
    local_f0 = result->normal;
    result->normal[0] = 0.0;
    result->normal[1] = 0.0;
    result->normal[2] = 0.0;
    if (0 < i) {
      iVar26 = 0;
      do {
                    /* Unresolved local var: MeI32 flags@[DW_OP_reg1(ECX)] */
        MVar24 = *(McdTriangleFlags *)(*(int *)(iVar23 + 0x34) + 0x14 + iVar26);
        tri.triangleData =
             *(anon_union_4_2_43add64d_for_triangleData *)(*(int *)(iVar23 + 0x34) + 0x10 + iVar26);
        tri.vertices[0] = vectors + 1;
        tri.normal = vectors;
        tri.vertices[2] = vectors + 3;
        tri.vertices[1] = vectors + 2;
        pfVar22 = *(float **)(*(int *)(iVar23 + 0x34) + 0xc + iVar26);
        local_f8 = fVar6;
        local_fc = fVar9;
        local_100 = fVar9 * pfVar22[2] + fVar6 * pfVar22[1] + *pfVar22 * fVar1;
        vectors[0][0] = local_100;
        local_104 = fVar7;
        local_108 = fVar10;
        local_10c = fVar10 * pfVar22[2] + fVar7 * pfVar22[1] + *pfVar22 * fVar4;
        vectors[0][1] = local_10c;
        local_110 = fVar8;
        local_114 = fVar11;
        vectors[0][2] = fVar11 * pfVar22[2] + *pfVar22 * fVar5 + fVar8 * pfVar22[1];
        pfVar22 = *(float **)(*(int *)(iVar23 + 0x34) + iVar26);
        vectors[1][0] = fVar9 * pfVar22[2] + *pfVar22 * fVar1 + fVar6 * pfVar22[1] + fVar12;
        local_118 = fVar12;
        local_11c = fVar14;
        vectors[1][1] = *pfVar22 * fVar4 + fVar7 * pfVar22[1] + fVar10 * pfVar22[2] + fVar14;
        vectors[1][2] = fVar11 * pfVar22[2] + *pfVar22 * fVar5 + fVar8 * pfVar22[1] + fVar13;
        local_120 = fVar13;
        if (((MVar24 & kMcdTriangleTwoSided) == 0) ||
           (0.0 <= (convexCenter[0] - vectors[1][0]) * local_100 +
                   (convexCenter[1] - vectors[1][1]) * local_10c +
                   (convexCenter[2] - vectors[1][2]) * vectors[0][2])) {
          pfVar22 = *(float **)(*(int *)(iVar23 + 0x34) + 4 + iVar26);
          vectors[3][0] = fVar9 * pfVar22[2] + *pfVar22 * fVar1 + fVar6 * pfVar22[1] + fVar12;
          vectors[3][1] = fVar10 * pfVar22[2] + *pfVar22 * fVar4 + fVar7 * pfVar22[1] + fVar14;
          vectors[3][2] = fVar11 * pfVar22[2] + *pfVar22 * fVar5 + fVar8 * pfVar22[1] + fVar13;
          pfVar22 = *(float **)(*(int *)(iVar23 + 0x34) + 8 + iVar26);
          vectors[2][0] = fVar1 * *pfVar22 + fVar6 * pfVar22[1] + fVar9 * pfVar22[2] + fVar12;
          vectors[2][1] = fVar4 * *pfVar22 + fVar7 * pfVar22[1] + fVar10 * pfVar22[2] + fVar14;
          vectors[0][2] = vectors[0][2] * -1.0;
          vectors[0][0] = local_100 * -1.0;
          vectors[2][2] = fVar5 * *pfVar22 + fVar8 * pfVar22[1] + fVar11 * pfVar22[2] + fVar13;
          vectors[0][1] = local_10c * -1.0;
          uVar3 = *(uint *)(*(int *)(iVar23 + 0x34) + 0x14 + iVar26);
          pfVar22 = (float *)((int)(uVar3 & 0x10) >> 2);
          MVar24 = MVar24 & ~(kMcdTriangleUseEdge2|kMcdTriangleUseEdge0) | (uVar3 & 4) << 2 |
                   (uint)pfVar22;
        }
        else {
          pfVar22 = *(float **)(*(int *)(iVar23 + 0x34) + 4 + iVar26);
          vectors[2][0] = fVar9 * pfVar22[2] + *pfVar22 * fVar1 + fVar6 * pfVar22[1] + fVar12;
          vectors[2][1] = fVar10 * pfVar22[2] + *pfVar22 * fVar4 + fVar7 * pfVar22[1] + fVar14;
          vectors[2][2] = fVar11 * pfVar22[2] + *pfVar22 * fVar5 + fVar8 * pfVar22[1] + fVar13;
          pfVar22 = *(float **)(*(int *)(iVar23 + 0x34) + 8 + iVar26);
          vectors[3][0] = fVar1 * *pfVar22 + fVar6 * pfVar22[1] + fVar9 * pfVar22[2] + fVar12;
          vectors[3][1] = fVar4 * *pfVar22 + fVar7 * pfVar22[1] + fVar10 * pfVar22[2] + fVar14;
          vectors[3][2] = fVar11 * pfVar22[2] + fVar5 * *pfVar22 + fVar8 * pfVar22[1] + fVar13;
        }
        *(float **)((int)apMStack_138 + iVar18 + 8) = pfVar22;
        *(float **)((int)apMStack_138 + iVar18 + 4) = pfVar22;
        *(McdIntersectResult **)((int)apMStack_138 + iVar18) = result;
        pMVar20 = convexMesh;
        tri.flags = MVar24;
        *(MeReal *)((int)&pMStack_13c + iVar18) = eps;
        iVar26 = iVar26 + 0x18;
        *(float **)((int)aMStack_144 + iVar18 + 4) = pfVar21;
        pMVar19 = convex;
        *(MeReal *)((int)aMStack_144 + iVar18) = pMVar20->mFatness;
        *(McdConvexHull **)((int)aiStack_150 + iVar18 + 8) = pMVar19;
        *(McdUserTriangle **)((int)aiStack_150 + iVar18 + 4) = &tri;
        *(undefined4 *)((int)aiStack_150 + iVar18) = 0x10a91;
        GenerateTriangleContact
                  (*(McdUserTriangle **)((int)aiStack_150 + iVar18 + 4),
                   *(McdConvexHull **)((int)aiStack_150 + iVar18 + 8),
                   *(MeReal *)((int)aMStack_144 + iVar18),
                   *(MeVector4 **)((int)aMStack_144 + iVar18 + 4),
                   *(MeReal *)((int)&pMStack_13c + iVar18),
                   *(McdIntersectResult **)((int)apMStack_138 + iVar18));
        i = i + -1;
      } while (i != 0);
    }
    fVar1 = result->normal[0];
    fVar4 = local_f0[2] * local_f0[2] + fVar1 * fVar1 + local_f0[1] * local_f0[1];
    if (1.4399999e-14 < fVar4) {
                    /* Unresolved local var: float __result@[???] */
      fVar4 = 1.0 / SQRT(fVar4);
      result->normal[0] = fVar1 * fVar4;
      local_f0[1] = local_f0[1] * fVar4;
      local_f0[2] = fVar4 * local_f0[2];
    }
    MVar25 = result->touch;
  }
  return MVar25;
}


/* ==== McdConvexMeshTriangleListRegisterInteraction ==== */

MeBool McdConvexMeshTriangleListRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  MeReal n [3];
  MeReal v [3] [3];
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdConvexMeshTriangleListIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.swap = 0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,7,6,&interactions);
  McdFrameworkRegisterTermAction(frame,McdConvexMeshTriangleListTermAction);
  return 1;
}


/* ==== McdConvexHullMaximumPoint ==== */

MeReal McdConvexHullMaximumPoint
                 (McdConvexHull *hull,MeReal *inDir,int hint,MeReal minDist,int *outIndex)

{
  int iVar1;
  McdCnvVertex *pMVar2;
  float fVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  McdCnvVertex *pMVar8;
  int iVar9;
  int iVar10;
  ushort uVar11;
  MeReal d1;
  McdCnvVertex *vert;
  int n;
  int prev;
  int m;
  int t;
  int i;
  
                    /* Unresolved local var: int j@[DW_OP_reg3(EBX)]
                       Unresolved local var: int k@[???]
                       Unresolved local var: int next@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeReal d2@[???] */
  iVar1 = hull->numVertex;
  pMVar2 = hull->vertex;
  if ((hint < 0) || (iVar1 <= hint)) {
    hint = 0;
  }
  pMVar8 = pMVar2 + hint;
  t = -2;
  i = -1;
  d1 = pMVar8->position[2] * inDir[2] +
       pMVar8->position[1] * inDir[1] + pMVar8->position[0] * *inDir;
  if ((-2 < iVar1) && (pMVar8 = pMVar2, hint != -1)) {
    while (iVar10 = hint,
          uVar11 = (ushort)(d1 < minDist) << 8 | (ushort)(NAN(d1) || NAN(minDist)) << 10 |
                   (ushort)(d1 == minDist) << 0xe,
          uVar4 = CONCAT22((short)((uint)pMVar8 >> 0x10),uVar11), hint = iVar10,
          (char)(uVar11 >> 8) != '\0') {
      uVar5 = McdCnvVertexGetCount(hull,iVar10,uVar4,uVar4);
      iVar9 = 0;
      if (0 < (int)uVar5) {
        if (((int)uVar5 < 2) || (iVar6 = 0, (uVar5 & 1) != 0)) {
          iVar9 = McdCnvVertexGetNeighbor(hull,iVar10,0,uVar5 & 1);
          iVar6 = iVar9;
          if (iVar9 != i) {
            pMVar8 = pMVar2 + iVar9;
            fVar3 = pMVar8->position[2] * inDir[2] +
                    pMVar8->position[1] * inDir[1] + pMVar8->position[0] * *inDir;
            uVar11 = (ushort)(fVar3 < d1) << 8 | (ushort)(NAN(fVar3) || NAN(d1)) << 10 |
                     (ushort)(fVar3 == d1) << 0xe;
            iVar6 = CONCAT22((short)((uint)pMVar8 >> 0x10),uVar11);
            if ((char)(uVar11 >> 8) == '\0') {
              hint = iVar9;
              d1 = fVar3;
            }
          }
          iVar9 = 1;
          if ((int)uVar5 < 2) goto LAB_00010eb3;
        }
        do {
          iVar6 = McdCnvVertexGetNeighbor(hull,iVar10,iVar9,iVar6);
          if (iVar6 != i) {
            pMVar8 = pMVar2 + iVar6;
            fVar3 = pMVar8->position[2] * inDir[2] +
                    pMVar8->position[1] * inDir[1] + pMVar8->position[0] * *inDir;
            if (d1 < fVar3) {
              hint = iVar6;
              d1 = fVar3;
            }
          }
          iVar7 = McdCnvVertexGetNeighbor(hull,iVar10,iVar9 + 1,iVar6);
          iVar6 = iVar7;
          if (iVar7 != i) {
            pMVar8 = pMVar2 + iVar7;
            fVar3 = pMVar8->position[2] * inDir[2] +
                    pMVar8->position[1] * inDir[1] + pMVar8->position[0] * *inDir;
            uVar11 = (ushort)(fVar3 < d1) << 8 | (ushort)(NAN(fVar3) || NAN(d1)) << 10 |
                     (ushort)(fVar3 == d1) << 0xe;
            iVar6 = CONCAT22((short)((uint)pMVar8 >> 0x10),uVar11);
            if ((char)(uVar11 >> 8) == '\0') {
              hint = iVar7;
              d1 = fVar3;
            }
          }
          iVar9 = iVar9 + 2;
        } while (iVar9 < (int)uVar5);
      }
LAB_00010eb3:
      pMVar8 = (McdCnvVertex *)(t + 1);
      if ((iVar1 <= (int)pMVar8) || (t = (int)pMVar8, i = iVar10, iVar10 == hint)) break;
    }
  }
  *outIndex = hint;
  return d1;
}


/* ==== GenerateTriangleContact ==== */

/* WARNING: Unknown calling convention */

void GenerateTriangleContact
               (McdUserTriangle *tri,McdConvexHull *hull,MeReal fatness,MeVector4 *tm,MeReal eps,
               McdIntersectResult *result)

{
  int iVar1;
  float fVar2;
  MeVector3 *paMVar3;
  MeVector3 *paMVar4;
  MeVector3 *paMVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  MeReal MVar9;
  MeReal MVar10;
  byte bVar11;
  uint uVar12;
  MeReal (*paMVar13) [3];
  int iVar14;
  float *pfVar15;
  int iVar16;
  longdouble lVar17;
  longdouble lVar18;
  longdouble lVar19;
  longdouble lVar20;
  longdouble lVar21;
  MeReal MVar22;
  int aiStackY_150 [3];
  float fStackY_144;
  float fStackY_140;
  float afStack_11c [2];
  int local_114;
  MeReal local_110;
  float local_10c;
  MeReal local_f0;
  MeReal local_ec;
  MeI16 segDim;
  MeReal ds;
  float local_e0;
  MeI32 i;
  VoronoiRegionType featureType;
  int count;
  MeReal local_d0;
  MeReal local_cc;
  MeReal local_c8;
  MeReal sep;
  MeReal zeroDisp;
  MeReal dp;
  MeReal es;
  VoronoiRegionType regionType_1;
  int cnvVCount;
  MeI32 featureIndex;
  MeReal si;
  VoronoiRegionType regionType;
  int maxVert;
  MeReal disp [3];
  MeReal c [3];
  MeReal sp [3];
  MeReal ni [3];
  MeReal n [3];
  MeReal e [3] [3];
  
                    /* Unresolved local var: MeI32 ei@[???] */
  paMVar3 = tri->vertices[0];
  paMVar4 = tri->vertices[1];
  e[0][0] = (*paMVar4)[0] - (*paMVar3)[0];
  e[0][1] = (*paMVar4)[1] - (*paMVar3)[1];
  e[0][2] = (*paMVar4)[2] - (*paMVar3)[2];
  paMVar5 = tri->vertices[2];
  e[1][0] = (*paMVar5)[0] - (*paMVar4)[0];
  e[1][1] = (*paMVar5)[1] - (*paMVar4)[1];
  e[1][2] = (*paMVar5)[2] - (*paMVar4)[2];
  e[2][0] = (*paMVar3)[0] - (*paMVar5)[0];
  e[2][1] = (*paMVar3)[1] - (*paMVar5)[1];
  e[2][2] = (*paMVar3)[2] - (*paMVar5)[2];
  paMVar4 = tri->normal;
  dp = ((*paMVar4)[2] * (*paMVar3)[2] +
       (*paMVar4)[1] * (*paMVar3)[1] + (*paMVar4)[0] * (*paMVar3)[0]) - fatness;
  fStackY_140 = 9.74631e-41;
  MVar22 = McdConvexHullMaximumPoint(hull,*paMVar4,-1,3.4028235e+38,&maxVert);
  zeroDisp = -MVar22;
  sep = dp + zeroDisp;
  if (sep < eps) {
    paMVar3 = tri->normal;
                    /* Unresolved local var: MeI32 i@[DW_OP_reg7(EDI)] */
    iVar14 = -1;
    paMVar13 = e;
    n[0] = (*paMVar3)[0] * -1.0;
    n[1] = (*paMVar3)[1] * -1.0;
    n[2] = (*paMVar3)[2] * -1.0;
    iVar16 = 0;
    do {
                    /* Unresolved local var: MeReal sepi@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal width@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal dsep@[DW_OP_reg11(ST0)] */
      fStackY_140 = 9.76467e-41;
      lVar17 = (longdouble)
               SegmentConvexHullSep
                         (*tri->vertices[iVar14 + 1],*paMVar13,0.0,1.0,hull,ni,&si,&regionType);
      lVar18 = (longdouble)si;
      lVar17 = lVar17 - (longdouble)fatness;
      if (((longdouble)0 < lVar18) || (lVar18 < (longdouble)1.0)) {
                    /* Unresolved local var: MeI32 nextEdge@[???] */
        uVar12 = 1 << ((byte)iVar16 & 0x1f) & 3;
        lVar19 = (longdouble)e[uVar12][2] * (longdouble)ni[2] +
                 (longdouble)e[uVar12][1] * (longdouble)ni[1] +
                 (longdouble)e[uVar12][0] * (longdouble)ni[0];
      }
      else {
                    /* Unresolved local var: MeReal wi@[DW_OP_reg13(ST2)] */
        local_c8 = ni[0];
        local_cc = ni[1];
        local_d0 = ni[2];
        lVar19 = (longdouble)0;
        if (lVar19 <= (longdouble)(*paMVar13)[2] * (longdouble)ni[2] +
                      (longdouble)(*paMVar13)[0] * (longdouble)ni[0] +
                      (longdouble)(*paMVar13)[1] * (longdouble)ni[1]) {
                    /* Unresolved local var: int i1@[???] */
          iVar1 = (iVar14 >> 0x1f & 3U) + iVar14;
          lVar20 = -((longdouble)ni[2] * (longdouble)e[iVar1][2] +
                    (longdouble)ni[0] * (longdouble)e[iVar1][0] +
                    (longdouble)ni[1] * (longdouble)e[iVar1][1]);
          bVar11 = lVar20 < (longdouble)0 |
                   (byte)((ushort)((ushort)(NAN(lVar20) || NAN((longdouble)0)) << 10) >> 8);
        }
        else {
          uVar12 = 1 << ((byte)iVar16 & 0x1f) & 3;
          lVar20 = (longdouble)e[uVar12][2] * (longdouble)ni[2] +
                   (longdouble)e[uVar12][1] * (longdouble)ni[1] +
                   (longdouble)e[uVar12][0] * (longdouble)ni[0];
          bVar11 = lVar20 < lVar19 |
                   (byte)((ushort)((ushort)(NAN(lVar20) || NAN(lVar19)) << 10) >> 8);
        }
        if (bVar11 != 0) {
          lVar19 = lVar20;
        }
      }
      if (lVar19 < (longdouble)0) {
        lVar17 = lVar19 + lVar17;
      }
      if ((longdouble)0.00025 < lVar17 - (longdouble)sep) {
        if ((longdouble)eps <= lVar17) {
          return;
        }
        lVar19 = (longdouble)-1.0;
        sep = (float)lVar17;
        n[0] = (float)((longdouble)ni[0] * lVar19);
        n[1] = (float)((longdouble)ni[1] * lVar19);
        n[2] = (float)(lVar19 * (longdouble)ni[2]);
        paMVar3 = tri->vertices[iVar14 + 1];
        lVar20 = lVar18 * (longdouble)(*paMVar13)[0] + (longdouble)(*paMVar3)[0];
        sp[0] = (float)lVar20;
        lVar21 = lVar18 * (longdouble)(*paMVar13)[1] + (longdouble)(*paMVar3)[1];
        sp[1] = (float)lVar21;
        lVar18 = lVar18 * (longdouble)(*paMVar13)[2] + (longdouble)(*paMVar3)[2];
        sp[2] = (MeReal)lVar18;
        zeroDisp = (MeReal)(lVar17 + lVar18 * lVar19 * (longdouble)ni[2] +
                                     lVar21 * (longdouble)ni[1] * lVar19 +
                                     lVar20 * (longdouble)ni[0] * lVar19);
      }
      iVar16 = iVar16 + 1;
      paMVar13 = paMVar13 + 1;
      iVar14 = iVar14 + 1;
    } while (iVar16 < 3);
                    /* Unresolved local var: MeVector3 * cnvV@[DW_OP_reg3(EBX)] */
    count = result->contactCount;
    featureType = ConvexHullMaximumFeature(hull,*tri->normal,&featureIndex,eps);
    MVar22 = dp;
    iVar14 = hull->numVertex;
    cnvVCount = 0;
    iVar16 = -(iVar14 * 0xc + 0xfU & 0xfffffff0);
    pfVar15 = (float *)((int)afStack_11c + iVar16);
    *(VoronoiRegionType *)(&stack0xfffffee0 + iVar16) = featureType;
    *(int *)(&stack0xfffffedc + iVar16) = (int)afStack_11c + iVar16;
    *(int **)(&stack0xfffffed8 + iVar16) = &cnvVCount;
    *(int *)(&stack0xfffffed4 + iVar16) = iVar14;
    *(undefined4 *)(&stack0xfffffed0 + iVar16) = 1;
    *(MeReal *)(&stack0xfffffecc + iVar16) = MVar22;
    *(MeVector3 **)((int)afStack_11c + iVar16 + -0x1c) = tri->normal;
    *(McdConvexHull **)((int)afStack_11c + iVar16 + -0x20) = hull;
    *(undefined4 *)((int)&fStackY_140 + iVar16) = 0x111c5;
    McdConvexHullPlaneCut
              (*(McdConvexHull **)((int)afStack_11c + iVar16 + -0x20),
               *(MeReal **)((int)afStack_11c + iVar16 + -0x1c),
               *(MeReal *)(&stack0xfffffecc + iVar16),*(int *)(&stack0xfffffed0 + iVar16),
               *(int *)(&stack0xfffffed4 + iVar16),*(int **)(&stack0xfffffed8 + iVar16),
               *(MeVector3 **)(&stack0xfffffedc + iVar16));
    local_114 = cnvVCount;
    if (0 < cnvVCount) {
      fVar2 = -fatness;
      i = 0;
      sp[0] = n[0] * fVar2;
      sp[1] = n[1] * fVar2;
      sp[2] = fVar2 * n[2];
      if (0 < cnvVCount) {
        do {
                    /* Unresolved local var: MeI32 j@[DW_OP_reg3(EBX)] */
          iVar14 = 0;
          paMVar13 = e;
          paMVar3 = tri->normal;
          local_e0 = pfVar15[2];
          do {
                    /* Unresolved local var: MeReal dist@[???] */
            paMVar4 = tri->vertices[iVar14];
            ni[0] = *pfVar15 - (*paMVar4)[0];
            ni[1] = pfVar15[1] - (*paMVar4)[1];
            ni[2] = pfVar15[2] - (*paMVar4)[2];
            c[0] = ni[1] * (*paMVar13)[2] - ni[2] * (*paMVar13)[1];
            c[1] = ni[2] * (*paMVar13)[0] - ni[0] * (*paMVar13)[2];
            c[2] = ni[0] * (*paMVar13)[1] - ni[1] * (*paMVar13)[0];
            fVar2 = c[0] * (*paMVar3)[0] + c[1] * (*paMVar3)[1] + c[2] * (*paMVar3)[2];
            if ((0.0 < fVar2) ||
               (fVar2 * fVar2 <
                ((*paMVar13)[0] * (*paMVar13)[0] + (*paMVar13)[1] * (*paMVar13)[1] +
                (*paMVar13)[2] * (*paMVar13)[2]) * eps * eps)) break;
            iVar14 = iVar14 + 1;
            paMVar13 = paMVar13 + 1;
          } while (iVar14 < 3);
          if (iVar14 == 3) {
            *pfVar15 = sp[0] + *pfVar15;
            pfVar15[1] = sp[1] + pfVar15[1];
            pfVar15[2] = sp[2] + pfVar15[2];
            *(McdIntersectResult **)(&stack0xfffffed4 + iVar16) = result;
            MVar22 = sep;
            *(MeVector4 **)(&stack0xfffffed0 + iVar16) = tm;
            *(undefined4 *)(&stack0xfffffecc + iVar16) = 0;
            *(undefined4 *)((int)afStack_11c + iVar16 + -0x1c) = 0;
            *(int *)((int)afStack_11c + iVar16 + -0x20) = (int)(short)(undefined2)featureType;
            *(undefined4 *)((int)&fStackY_140 + iVar16) = 2;
            *(MeReal *)((int)&fStackY_144 + iVar16) = MVar22;
            *(MeReal **)((int)aiStackY_150 + iVar16 + 8) = n;
            *(float **)((int)aiStackY_150 + iVar16 + 4) = pfVar15;
            *(undefined4 *)((int)aiStackY_150 + iVar16) = 0x116cb;
            AccumulateSphylContacts
                      (*(float **)((int)aiStackY_150 + iVar16 + 4),
                       *(float **)((int)aiStackY_150 + iVar16 + 8),
                       *(float *)((int)&fStackY_144 + iVar16),*(short *)((int)&fStackY_140 + iVar16)
                       ,*(short *)((int)afStack_11c + iVar16 + -0x20),
                       *(float **)((int)afStack_11c + iVar16 + -0x1c),
                       *(float *)(&stack0xfffffecc + iVar16),*(float **)(&stack0xfffffed0 + iVar16),
                       *(_McdIntersectResult **)(&stack0xfffffed4 + iVar16));
            local_114 = cnvVCount;
          }
          i = i + 1;
          pfVar15 = pfVar15 + 3;
        } while (i < local_114);
      }
    }
                    /* Unresolved local var: MeI32 i@[DW_OP_reg7(EDI)] */
    iVar14 = 0;
    paMVar13 = e;
    do {
      if (((int)tri->flags >> ((char)iVar14 + 2U & 0x1f) & 1U) != 0) {
                    /* Unresolved local var: MeReal len@[DW_OP_reg6(ESI)] */
        ni[0] = (*paMVar13)[0];
        ni[1] = (*paMVar13)[1];
        MVar22 = (*paMVar13)[2];
        *(MeReal **)(&stack0xfffffed4 + iVar16) = ni;
        ni[2] = MVar22;
        *(undefined4 *)(&stack0xfffffed0 + iVar16) = 0x113a5;
        MVar22 = MeVector3Normalize(*(MeReal **)(&stack0xfffffed4 + iVar16));
        *(VoronoiRegionType **)(&stack0xfffffed0 + iVar16) = &regionType_1;
        local_110 = MVar22;
        *(MeReal **)(&stack0xfffffecc + iVar16) = &es;
        *(MeReal **)((int)afStack_11c + iVar16 + -0x1c) = c;
        MVar22 = local_110;
        *(MeReal *)((int)afStack_11c + iVar16 + -0x20) = local_110;
        *(undefined4 *)((int)&fStackY_140 + iVar16) = 0;
        *(MeReal **)((int)&fStackY_144 + iVar16) = ni;
        *(MeVector3 **)((int)aiStackY_150 + iVar16 + 8) = tri->vertices[iVar14];
        *(McdConvexHull **)((int)aiStackY_150 + iVar16 + 4) = hull;
        *(undefined4 *)((int)aiStackY_150 + iVar16) = 0x113dd;
        lVar18 = (longdouble)
                 ConvexHullNSegment(*(McdConvexHull **)((int)aiStackY_150 + iVar16 + 4),
                                    *(float **)((int)aiStackY_150 + iVar16 + 8),
                                    *(float **)((int)&fStackY_144 + iVar16),
                                    *(float *)((int)&fStackY_140 + iVar16),
                                    *(float *)((int)afStack_11c + iVar16 + -0x20),
                                    *(float **)((int)afStack_11c + iVar16 + -0x1c),
                                    *(float **)(&stack0xfffffecc + iVar16),
                                    *(VoronoiRegionType **)(&stack0xfffffed0 + iVar16));
        fVar2 = (float)lVar18;
        ds = fVar2;
        segDim = 0;
        if ((0.0 < es) && (local_110 = MVar22, es < MVar22)) {
          segDim = 1;
        }
        paMVar3 = tri->vertices[iVar14];
        local_ec = ni[0];
        sp[0] = ni[0] * es + (*paMVar3)[0];
        local_f0 = ni[1];
        sp[1] = ni[1] * es + (*paMVar3)[1];
        disp[0] = c[0] - sp[0];
        disp[1] = c[1] - sp[1];
        sp[2] = es * ni[2] + (*paMVar3)[2];
        disp[2] = c[2] - sp[2];
        if (disp[0] * disp[0] + disp[1] * disp[1] + disp[2] * disp[2] < fatness * fatness + eps) {
                    /* Unresolved local var: MeReal sepi@[DW_OP_reg15(ST4)] */
          if (es == 0.0) {
            ds = 0.0;
            sp[2] = sp[2] + ni[2] * fVar2;
            sp[0] = sp[0] + fVar2 * ni[0];
            sp[1] = sp[1] + fVar2 * ni[1];
          }
          fVar2 = zeroDisp - (n[2] * sp[2] + n[1] * sp[1] + n[0] * sp[0]);
          if (fVar2 < eps) {
            *(McdIntersectResult **)(&stack0xfffffed4 + iVar16) = result;
            *(MeVector4 **)(&stack0xfffffed0 + iVar16) = tm;
            *(undefined4 *)(&stack0xfffffecc + iVar16) = 0;
            *(undefined4 *)((int)afStack_11c + iVar16 + -0x1c) = 0;
            *(int *)((int)afStack_11c + iVar16 + -0x20) = (int)(short)(undefined2)regionType_1;
            *(int *)((int)&fStackY_140 + iVar16) = (int)segDim;
            local_10c = fVar2;
            *(float *)((int)&fStackY_144 + iVar16) = fVar2;
            *(MeReal **)((int)aiStackY_150 + iVar16 + 8) = n;
            *(MeReal **)((int)aiStackY_150 + iVar16 + 4) = sp;
            *(undefined4 *)((int)aiStackY_150 + iVar16) = 0x1165e;
            AccumulateSphylContacts
                      (*(float **)((int)aiStackY_150 + iVar16 + 4),
                       *(float **)((int)aiStackY_150 + iVar16 + 8),
                       *(float *)((int)&fStackY_144 + iVar16),*(short *)((int)&fStackY_140 + iVar16)
                       ,*(short *)((int)afStack_11c + iVar16 + -0x20),
                       *(float **)((int)afStack_11c + iVar16 + -0x1c),
                       *(float *)(&stack0xfffffecc + iVar16),*(float **)(&stack0xfffffed0 + iVar16),
                       *(_McdIntersectResult **)(&stack0xfffffed4 + iVar16));
            fVar2 = local_10c;
          }
          MVar10 = ni[2];
          MVar9 = ni[1];
          MVar22 = ni[0];
          if ((0.0 < ds) &&
             (fVar2 = fVar2 - (ni[2] * n[2] + ni[1] * n[1] + ni[0] * n[0]) * ds, fVar2 < eps)) {
            *(McdIntersectResult **)(&stack0xfffffed4 + iVar16) = result;
            *(MeVector4 **)(&stack0xfffffed0 + iVar16) = tm;
            fVar8 = MVar10 * ds;
            *(undefined4 *)(&stack0xfffffecc + iVar16) = 0;
            *(undefined4 *)((int)afStack_11c + iVar16 + -0x1c) = 0;
            fVar6 = MVar22 * ds;
            *(int *)((int)afStack_11c + iVar16 + -0x20) = (int)(short)(undefined2)regionType_1;
            fVar7 = MVar9 * ds;
            *(int *)((int)&fStackY_140 + iVar16) = (int)segDim;
            *(float *)((int)&fStackY_144 + iVar16) = fVar2;
            *(MeReal **)((int)aiStackY_150 + iVar16 + 8) = n;
            *(MeReal **)((int)aiStackY_150 + iVar16 + 4) = sp;
            sp[0] = fVar6 + sp[0];
            sp[1] = fVar7 + sp[1];
            sp[2] = fVar8 + sp[2];
            *(undefined4 *)((int)aiStackY_150 + iVar16) = 0x1161a;
            AccumulateSphylContacts
                      (*(float **)((int)aiStackY_150 + iVar16 + 4),
                       *(float **)((int)aiStackY_150 + iVar16 + 8),
                       *(float *)((int)&fStackY_144 + iVar16),*(short *)((int)&fStackY_140 + iVar16)
                       ,*(short *)((int)afStack_11c + iVar16 + -0x20),
                       *(float **)((int)afStack_11c + iVar16 + -0x1c),
                       *(float *)(&stack0xfffffecc + iVar16),*(float **)(&stack0xfffffed0 + iVar16),
                       *(_McdIntersectResult **)(&stack0xfffffed4 + iVar16));
          }
        }
      }
      iVar14 = iVar14 + 1;
      paMVar13 = paMVar13 + 1;
    } while (iVar14 < 3);
                    /* Unresolved local var: int c@[DW_OP_reg3(EBX)] */
    if (count < result->contactCount) {
      iVar14 = count * 0x28;
      do {
        count = count + 1;
        *(anon_union_4_2_43add64d_for_triangleData *)((int)result->contacts->normal + iVar14 + 0x18)
             = tri->triangleData;
        iVar14 = iVar14 + 0x28;
      } while (count < result->contactCount);
    }
  }
  return;
}


/* ==== MeVector3Normalize ==== */

MeReal MeVector3Normalize(MeReal *v)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float local_8;
  
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
  fVar1 = *v;
  fVar2 = v[1];
  fVar3 = v[2];
  local_8 = fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1;
  if (local_8 <= 0.0) {
    *v = 1.0;
    v[1] = 0.0;
    v[2] = 0.0;
    local_8 = 0.0;
  }
  else {
    local_8 = SQRT(local_8);
    fVar4 = 1.0 / local_8;
    *v = fVar1 * fVar4;
    v[1] = fVar2 * fVar4;
    v[2] = fVar4 * fVar3;
  }
  return local_8;
}


