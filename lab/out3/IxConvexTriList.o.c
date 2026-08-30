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
  int iVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  byte bVar15;
  void *pvVar11;
  int iVar12;
  void *pvVar13;
  McdCnvVertex *pMVar14;
  McdCnvVertex *pMVar16;
  int iVar17;
  MeReal MVar18;
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
     (MVar18 = McdConvexHullMaximumPoint(hull,norm,0,3.4028235e+38,&start), dp <= MVar18)) {
    pMVar6 = hull->vertex;
    MeSetInit(&set,nodemem,200,(void *)0x0);
    MeSetAdd(&set,(void *)start);
    while (set.next != (MeDictNode *)0x0) {
      pvVar11 = MeSetIteratorNext(&set);
      pMVar16 = pMVar6 + (int)pvVar11;
      fVar8 = pMVar16->position[2] * norm[2] +
              pMVar16->position[1] * norm[1] + *norm * pMVar16->position[0];
      if ((flags & 2U) != 0) {
        paMVar1 = outVert + *numVert;
        (*paMVar1)[0] = pMVar16->position[0];
        (*paMVar1)[1] = pMVar16->position[1];
        (*paMVar1)[2] = pMVar16->position[2];
        iVar12 = *numVert;
        *numVert = iVar12 + 1;
        if (iVar12 + 1 == maxVert) {
          return;
        }
      }
      iVar12 = McdCnvVertexGetCount(hull,(int)pvVar11);
      if (0 < iVar12) {
        iVar17 = 0;
        do {
          pvVar13 = (void *)McdCnvVertexGetNeighbor(hull,(int)pvVar11,iVar17);
          pMVar14 = pMVar6 + (int)pvVar13;
          fVar4 = pMVar14->position[2] * norm[2] +
                  pMVar14->position[1] * norm[1] + pMVar14->position[0] * *norm;
          if (dp <= fVar4) {
            MeSetAdd(&set,pvVar13);
          }
          else if ((flags & 1U) != 0) {
            fVar9 = 1.0 / (fVar8 - fVar4);
            pMVar14 = pMVar6 + (int)pvVar13;
            fVar10 = (fVar8 - dp) * fVar9;
            fVar2 = pMVar14->position[1];
            fVar3 = pMVar14->position[2];
            fVar9 = (dp - fVar4) * fVar9;
            fVar4 = pMVar16->position[1];
            fVar5 = pMVar16->position[2];
            paMVar1 = outVert + *numVert;
            (*paMVar1)[0] = fVar9 * pMVar16->position[0] + pMVar14->position[0] * fVar10;
            (*paMVar1)[1] = fVar9 * fVar4 + fVar2 * fVar10;
            (*paMVar1)[2] = fVar9 * fVar5 + fVar3 * fVar10;
            iVar7 = *numVert;
            if (iVar7 != 0) {
              if (1e-06 <= ABS((outVert + iVar7)[-1][0])) {
                fVar4 = (outVert + iVar7)[-1][0];
                fVar4 = ABS((1.0 / fVar4) * (outVert[iVar7][0] - fVar4));
                bVar15 = fVar4 < 1e-06 | (byte)((ushort)((ushort)NAN(fVar4) << 10) >> 8);
              }
              else {
                fVar4 = ABS(outVert[iVar7][0]);
                bVar15 = fVar4 < 1e-06 | (byte)((ushort)((ushort)NAN(fVar4) << 10) >> 8);
              }
              if (bVar15 != 0) {
                if (1e-06 <= ABS(outVert[iVar7 + -1][1])) {
                  fVar4 = ABS((1.0 / outVert[iVar7 + -1][1]) *
                              (outVert[iVar7][1] - outVert[iVar7 + -1][1]));
                  bVar15 = fVar4 < 1e-06 | (byte)((ushort)((ushort)NAN(fVar4) << 10) >> 8);
                }
                else {
                  bVar15 = ABS(outVert[iVar7][1]) < 1e-06 |
                           (byte)((ushort)((ushort)NAN(ABS(outVert[iVar7][1])) << 10) >> 8);
                }
                if (bVar15 != 0) {
                  if (1e-06 <= ABS(outVert[iVar7 + -1][2])) {
                    fVar4 = (outVert[iVar7][2] - outVert[iVar7 + -1][2]) *
                            (1.0 / outVert[iVar7 + -1][2]);
                  }
                  else {
                    fVar4 = outVert[iVar7][2];
                  }
                  if (ABS(fVar4) < 1e-06) goto LAB_00010301;
                }
              }
            }
            *numVert = iVar7 + 1;
            if (iVar7 + 1 == maxVert) {
              return;
            }
          }
LAB_00010301:
          iVar17 = iVar17 + 1;
        } while (iVar17 < iVar12);
      }
    }
  }
  return;
}


/* ==== McdConvexMeshTriangleListIntersect ==== */

/* WARNING: Unknown calling convention */

MeBool McdConvexMeshTriangleListIntersect(McdModelPair *p,McdIntersectResult *result)

{
  code *pcVar1;
  uint uVar2;
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
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  int iVar17;
  McdConvexHull *pMVar18;
  McdConvexMesh *pMVar19;
  float *pfVar20;
  float *pfVar21;
  void *pvVar22;
  McdTriangleFlags MVar23;
  MeBool MVar24;
  int iVar25;
  float fVar26;
  int aiStackY_150 [3];
  MeReal aMStackY_144 [2];
  undefined4 uStack_130;
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
  aMStackY_144[1] = 9.32354e-41;
  pfVar20 = McdModelGetTransformPtr(p->model1);
  aMStackY_144[1] = 9.32564e-41;
  pfVar21 = McdModelGetTransformPtr(p->model2);
  aMStackY_144[1] = 9.3276e-41;
  eps = McdModelGetContactTolerance(p->model1);
  aMStackY_144[1] = 9.33027e-41;
  fVar26 = McdModelGetContactTolerance(p->model2);
  eps = fVar26 + eps;
  aMStackY_144[1] = 9.33363e-41;
  convexMesh = McdModelGetGeometry(p->model1);
  convex = &convexMesh->mHull;
  aMStackY_144[1] = 9.33783e-41;
  pvVar22 = McdModelGetGeometry(p->model2);
  result->contactCount = 0;
  result->touch = 0;
  fVar26 = pfVar20[2] * pfVar21[2] + pfVar20[1] * pfVar21[1] + *pfVar20 * *pfVar21;
  fVar3 = pfVar20[6] * pfVar21[2] + pfVar20[5] * pfVar21[1] + pfVar20[4] * *pfVar21;
  fVar4 = pfVar20[10] * pfVar21[2] + pfVar20[9] * pfVar21[1] + pfVar20[8] * *pfVar21;
  fVar5 = pfVar20[2] * pfVar21[6] + pfVar20[1] * pfVar21[5] + *pfVar20 * pfVar21[4];
  fVar6 = pfVar20[6] * pfVar21[6] + pfVar20[5] * pfVar21[5] + pfVar20[4] * pfVar21[4];
  fVar7 = pfVar20[10] * pfVar21[6] + pfVar20[9] * pfVar21[5] + pfVar20[8] * pfVar21[4];
  fVar8 = pfVar20[2] * pfVar21[10] + pfVar20[1] * pfVar21[9] + *pfVar20 * pfVar21[8];
  fVar9 = pfVar20[6] * pfVar21[10] + pfVar20[5] * pfVar21[9] + pfVar20[4] * pfVar21[8];
  fVar10 = pfVar20[10] * pfVar21[10] + pfVar20[9] * pfVar21[9] + pfVar20[8] * pfVar21[8];
  convexCenter[0] = pfVar21[0xc] - pfVar20[0xc];
  convexCenter[1] = pfVar21[0xd] - pfVar20[0xd];
  convexCenter[2] = pfVar21[0xe] - pfVar20[0xe];
  fVar11 = convexCenter[2] * pfVar20[2] + convexCenter[0] * *pfVar20 + convexCenter[1] * pfVar20[1];
  fVar13 = convexCenter[2] * pfVar20[6] +
           convexCenter[0] * pfVar20[4] + convexCenter[1] * pfVar20[5];
  fVar12 = convexCenter[0] * pfVar20[8] + convexCenter[1] * pfVar20[9] +
           convexCenter[2] * pfVar20[10];
  aMStackY_144[1] = 9.39641e-41;
  McdConvexMeshGetBSphere(convexMesh,convexCenter,&convexRadius);
  fVar14 = convexCenter[0] - fVar11;
  fVar16 = convexCenter[1] - fVar13;
  fVar15 = convexCenter[2] - fVar12;
  convexCenterRelative[0] = fVar15 * fVar4 + fVar14 * fVar26 + fVar16 * fVar3;
  convexCenterRelative[1] = fVar14 * fVar5 + fVar16 * fVar6 + fVar15 * fVar7;
  convexCenterRelative[2] = fVar14 * fVar8 + fVar16 * fVar9 + fVar15 * fVar10;
  iVar17 = -(*(int *)((int)pvVar22 + 0x28) * 0x18 + 0xfU & 0xfffffff0);
  *(undefined1 **)((int)pvVar22 + 0x34) = auStack_12c + iVar17;
  *(int *)(&stack0xfffffec4 + iVar17) = *(int *)((int)pvVar22 + 0x28);
  *(MeReal *)((int)aMStackY_144 + iVar17 + 4) = convexRadius;
  *(MeReal **)((int)aMStackY_144 + iVar17) = convexCenterRelative;
  *(undefined4 *)((int)aiStackY_150 + iVar17 + 8) = *(undefined4 *)((int)pvVar22 + 0x34);
  *(McdModelPair **)((int)aiStackY_150 + iVar17 + 4) = p;
  pcVar1 = *(code **)((int)pvVar22 + 0x30);
  *(undefined4 *)((int)aiStackY_150 + iVar17) = 0x106a1;
  i = (*pcVar1)();
  MVar24 = 0;
  if (i != 0) {
    local_f0 = result->normal;
    result->normal[0] = 0.0;
    result->normal[1] = 0.0;
    result->normal[2] = 0.0;
    if (0 < i) {
      iVar25 = 0;
      do {
                    /* Unresolved local var: MeI32 flags@[DW_OP_reg1(ECX)] */
        MVar23 = *(McdTriangleFlags *)(*(int *)((int)pvVar22 + 0x34) + 0x14 + iVar25);
        tri.triangleData =
             *(anon_union_4_2_43add64d_for_triangleData *)
              (*(int *)((int)pvVar22 + 0x34) + 0x10 + iVar25);
        tri.vertices[0] = vectors + 1;
        tri.normal = vectors;
        tri.vertices[2] = vectors + 3;
        tri.vertices[1] = vectors + 2;
        pfVar21 = *(float **)(*(int *)((int)pvVar22 + 0x34) + 0xc + iVar25);
        local_f8 = fVar5;
        local_fc = fVar8;
        local_100 = fVar8 * pfVar21[2] + fVar5 * pfVar21[1] + *pfVar21 * fVar26;
        vectors[0][0] = local_100;
        local_104 = fVar6;
        local_108 = fVar9;
        local_10c = fVar9 * pfVar21[2] + fVar6 * pfVar21[1] + *pfVar21 * fVar3;
        vectors[0][1] = local_10c;
        local_110 = fVar7;
        local_114 = fVar10;
        vectors[0][2] = fVar10 * pfVar21[2] + *pfVar21 * fVar4 + fVar7 * pfVar21[1];
        pfVar21 = *(float **)(*(int *)((int)pvVar22 + 0x34) + iVar25);
        vectors[1][0] = fVar8 * pfVar21[2] + *pfVar21 * fVar26 + fVar5 * pfVar21[1] + fVar11;
        local_118 = fVar11;
        local_11c = fVar13;
        vectors[1][1] = *pfVar21 * fVar3 + fVar6 * pfVar21[1] + fVar9 * pfVar21[2] + fVar13;
        vectors[1][2] = fVar10 * pfVar21[2] + *pfVar21 * fVar4 + fVar7 * pfVar21[1] + fVar12;
        local_120 = fVar12;
        if (((MVar23 & kMcdTriangleTwoSided) == 0) ||
           (0.0 <= (convexCenter[0] - vectors[1][0]) * local_100 +
                   (convexCenter[1] - vectors[1][1]) * local_10c +
                   (convexCenter[2] - vectors[1][2]) * vectors[0][2])) {
          pfVar21 = *(float **)(*(int *)((int)pvVar22 + 0x34) + 4 + iVar25);
          vectors[3][0] = fVar8 * pfVar21[2] + *pfVar21 * fVar26 + fVar5 * pfVar21[1] + fVar11;
          vectors[3][1] = fVar9 * pfVar21[2] + *pfVar21 * fVar3 + fVar6 * pfVar21[1] + fVar13;
          vectors[3][2] = fVar10 * pfVar21[2] + *pfVar21 * fVar4 + fVar7 * pfVar21[1] + fVar12;
          pfVar21 = *(float **)(*(int *)((int)pvVar22 + 0x34) + 8 + iVar25);
          vectors[2][0] = fVar26 * *pfVar21 + fVar5 * pfVar21[1] + fVar8 * pfVar21[2] + fVar11;
          vectors[2][1] = fVar3 * *pfVar21 + fVar6 * pfVar21[1] + fVar9 * pfVar21[2] + fVar13;
          vectors[0][2] = vectors[0][2] * -1.0;
          vectors[0][0] = local_100 * -1.0;
          vectors[2][2] = fVar4 * *pfVar21 + fVar7 * pfVar21[1] + fVar10 * pfVar21[2] + fVar12;
          vectors[0][1] = local_10c * -1.0;
          uVar2 = *(uint *)(*(int *)((int)pvVar22 + 0x34) + 0x14 + iVar25);
          pfVar21 = (float *)((int)(uVar2 & 0x10) >> 2);
          MVar23 = MVar23 & ~(kMcdTriangleUseEdge2|kMcdTriangleUseEdge0) | (uVar2 & 4) << 2 |
                   (uint)pfVar21;
        }
        else {
          pfVar21 = *(float **)(*(int *)((int)pvVar22 + 0x34) + 4 + iVar25);
          vectors[2][0] = fVar8 * pfVar21[2] + *pfVar21 * fVar26 + fVar5 * pfVar21[1] + fVar11;
          vectors[2][1] = fVar9 * pfVar21[2] + *pfVar21 * fVar3 + fVar6 * pfVar21[1] + fVar13;
          vectors[2][2] = fVar10 * pfVar21[2] + *pfVar21 * fVar4 + fVar7 * pfVar21[1] + fVar12;
          pfVar21 = *(float **)(*(int *)((int)pvVar22 + 0x34) + 8 + iVar25);
          vectors[3][0] = fVar26 * *pfVar21 + fVar5 * pfVar21[1] + fVar8 * pfVar21[2] + fVar11;
          vectors[3][1] = fVar3 * *pfVar21 + fVar6 * pfVar21[1] + fVar9 * pfVar21[2] + fVar13;
          vectors[3][2] = fVar10 * pfVar21[2] + fVar4 * *pfVar21 + fVar7 * pfVar21[1] + fVar12;
        }
        *(float **)((int)&uStack_130 + iVar17) = pfVar21;
        *(float **)(&stack0xfffffecc + iVar17) = pfVar21;
        *(McdIntersectResult **)(&stack0xfffffec8 + iVar17) = result;
        pMVar19 = convexMesh;
        tri.flags = MVar23;
        *(MeReal *)(&stack0xfffffec4 + iVar17) = eps;
        iVar25 = iVar25 + 0x18;
        *(float **)((int)aMStackY_144 + iVar17 + 4) = pfVar20;
        pMVar18 = convex;
        *(MeReal *)((int)aMStackY_144 + iVar17) = pMVar19->mFatness;
        *(McdConvexHull **)((int)aiStackY_150 + iVar17 + 8) = pMVar18;
        *(McdUserTriangle **)((int)aiStackY_150 + iVar17 + 4) = &tri;
        *(undefined4 *)((int)aiStackY_150 + iVar17) = 0x10a91;
        GenerateTriangleContact
                  (*(McdUserTriangle **)((int)aiStackY_150 + iVar17 + 4),
                   *(McdConvexHull **)((int)aiStackY_150 + iVar17 + 8),
                   *(MeReal *)((int)aMStackY_144 + iVar17),
                   *(MeVector4 **)((int)aMStackY_144 + iVar17 + 4),
                   *(MeReal *)(&stack0xfffffec4 + iVar17),
                   *(McdIntersectResult **)(&stack0xfffffec8 + iVar17));
        i = i + -1;
      } while (i != 0);
    }
    fVar26 = result->normal[0];
    fVar3 = local_f0[2] * local_f0[2] + fVar26 * fVar26 + local_f0[1] * local_f0[1];
    if (1.4399999e-14 < fVar3) {
                    /* Unresolved local var: float __result@[???] */
      fVar3 = 1.0 / SQRT(fVar3);
      result->normal[0] = fVar26 * fVar3;
      local_f0[1] = local_f0[1] * fVar3;
      local_f0[2] = fVar3 * local_f0[2];
    }
    MVar24 = result->touch;
  }
  return MVar24;
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
  uint uVar4;
  int iVar5;
  McdCnvVertex *pMVar6;
  int iVar7;
  int iVar8;
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
  pMVar6 = pMVar2 + hint;
  t = -2;
  i = -1;
  d1 = pMVar6->position[2] * inDir[2] +
       pMVar6->position[1] * inDir[1] + pMVar6->position[0] * *inDir;
  if ((-2 < iVar1) && (hint != -1)) {
    while (iVar8 = hint, hint = iVar8, d1 <= minDist) {
      uVar4 = McdCnvVertexGetCount(hull,iVar8);
      iVar7 = 0;
      if (0 < (int)uVar4) {
        if (((int)uVar4 < 2) || ((uVar4 & 1) != 0)) {
          iVar7 = McdCnvVertexGetNeighbor(hull,iVar8,0);
          if (iVar7 != i) {
            pMVar6 = pMVar2 + iVar7;
            fVar3 = pMVar6->position[2] * inDir[2] +
                    pMVar6->position[1] * inDir[1] + pMVar6->position[0] * *inDir;
            if (d1 < fVar3) {
              hint = iVar7;
              d1 = fVar3;
            }
          }
          iVar7 = 1;
          if ((int)uVar4 < 2) goto LAB_00010eb3;
        }
        do {
          iVar5 = McdCnvVertexGetNeighbor(hull,iVar8,iVar7);
          if (iVar5 != i) {
            pMVar6 = pMVar2 + iVar5;
            fVar3 = pMVar6->position[2] * inDir[2] +
                    pMVar6->position[1] * inDir[1] + pMVar6->position[0] * *inDir;
            if (d1 < fVar3) {
              hint = iVar5;
              d1 = fVar3;
            }
          }
          iVar5 = McdCnvVertexGetNeighbor(hull,iVar8,iVar7 + 1);
          if (iVar5 != i) {
            pMVar6 = pMVar2 + iVar5;
            fVar3 = pMVar6->position[2] * inDir[2] +
                    pMVar6->position[1] * inDir[1] + pMVar6->position[0] * *inDir;
            if (d1 < fVar3) {
              hint = iVar5;
              d1 = fVar3;
            }
          }
          iVar7 = iVar7 + 2;
        } while (iVar7 < (int)uVar4);
      }
LAB_00010eb3:
      t = t + 1;
      if ((iVar1 <= t) || (i = iVar8, iVar8 == hint)) break;
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
                         (*tri->vertices[iVar14 + 1],*paMVar13,0.0,1.0,(McdConvexHull *)hull,ni,&si,
                          &regionType);
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
    featureType = ConvexHullMaximumFeature((McdConvexHull *)hull,*tri->normal,&featureIndex,eps);
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


/* ==== McdConvexMeshTriangleListTermAction ==== */

/* WARNING: Unknown calling convention */

void McdConvexMeshTriangleListTermAction(McdFrameworkID frame)

{
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


