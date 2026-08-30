/* ==== McdCylinderTriangleListIntersect ==== */

int McdCylinderTriangleListIntersect(McdModelPair *p,McdIntersectResult *result)

{
  McdContact *pMVar1;
  float fVar2;
  float fVar3;
  McdTriangleListFnPtr p_Var4;
  int iVar5;
  int iVar6;
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
  MeReal *pMVar18;
  MeReal *pMVar19;
  MeReal *pMVar20;
  MeReal MVar21;
  MeReal MVar22;
  MeReal MVar23;
  McdFramework *pMVar24;
  McdTriangleList *pMVar25;
  bool bVar26;
  float *pfVar27;
  float *pfVar28;
  uint uVar29;
  lsVec3 *plVar30;
  undefined4 *puVar31;
  float fVar32;
  int aiStackY_2a0 [6];
  MeReal aMStackY_288 [2];
  float fStackY_280;
  int iStack_270;
  McdUserTriangle MStack_26c;
  float *local_254;
  MeReal cylHH;
  MeReal cylRadius;
  MeReal eps;
  McdFramework *fwk;
  McdTriangleList *triList;
  McdCylinderID cylgeom;
  int count;
  int j;
  MeReal separation;
  MeReal PN;
  lsVec3 *verts;
  MeI16 dims;
  MeReal cylBSRad;
  lsVec3 diff;
  MeReal cylBSCentre [3];
  MeReal edge [3] [3];
  lsVec3 v;
  lsVec3 normal;
  lsVec3 footprint [18];
  McdUserTriangle ct;
  lsVec3 vector [4];
  lsVec3 cylPosTrans;
  lsTransform triToCyl;
  lsVec3 temp;
  
                    /* Unresolved local var: McdUserTriangle * triNE@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * cylTM@[DW_OP_reg6(ESI)]
                       Unresolved local var: lsTransform * triListTM@[DW_OP_reg7(EDI)] */
  fStackY_280 = 9.18677e-41;
  cylgeom = McdModelGetGeometry(p->model1);
  fStackY_280 = 9.18901e-41;
  triList = McdModelGetGeometry(p->model2);
  fwk = p->model1->frame;
  fStackY_280 = 9.19238e-41;
  eps = McdModelGetContactTolerance(p->model1);
  fStackY_280 = 9.19462e-41;
  fVar32 = McdModelGetContactTolerance(p->model2);
  eps = fVar32 + eps;
  result->contactCount = 0;
  result->touch = 0;
  fStackY_280 = 9.20051e-41;
  cylRadius = McdCylinderGetRadius(cylgeom);
  fStackY_280 = 9.20317e-41;
  cylHH = McdCylinderGetHalfHeight(cylgeom);
  fStackY_280 = 9.20527e-41;
  pfVar27 = McdModelGetTransformPtr(p->model1);
  fStackY_280 = 9.20695e-41;
  pfVar28 = McdModelGetTransformPtr(p->model2);
  fStackY_280 = 9.2113e-41;
  McdCylinderGetBSphere(cylgeom,cylBSCentre,&cylBSRad);
  pMVar25 = triList;
  MVar21 = eps;
  local_254 = pfVar28 + 0xc;
  cylBSRad = eps + cylBSRad;
  diff.v[0] = pfVar27[0xc] - pfVar28[0xc];
  diff.v[1] = pfVar27[0xd] - pfVar28[0xd];
  diff.v[2] = pfVar27[0xe] - pfVar28[0xe];
  cylPosTrans.v[0] = diff.v[2] * pfVar28[2] + diff.v[1] * pfVar28[1] + diff.v[0] * *pfVar28;
  cylPosTrans.v[1] = diff.v[2] * pfVar28[6] + diff.v[0] * pfVar28[4] + diff.v[1] * pfVar28[5];
  cylPosTrans.v[2] = diff.v[2] * pfVar28[10] + diff.v[0] * pfVar28[8] + diff.v[1] * pfVar28[9];
  iVar6 = -(triList->triangleMaxCount * 0x18 + 0xfU & 0xfffffff0);
  triList->list = (McdUserTriangle *)((int)&MStack_26c + iVar6);
  *(int *)(&stack0xfffffd84 + iVar6) = triList->triangleMaxCount;
  *(float *)((int)&fStackY_280 + iVar6) = MVar21 + cylBSRad;
  *(lsVec3 **)((int)aMStackY_288 + iVar6 + 4) = &cylPosTrans;
  *(McdUserTriangle **)((int)aMStackY_288 + iVar6) = pMVar25->list;
  *(McdModelPair **)((int)aiStackY_2a0 + iVar6 + 0x14) = p;
  p_Var4 = triList->triangleListGenerator;
  *(undefined4 *)((int)aiStackY_2a0 + iVar6 + 0x10) = 0x101ab;
  count = (*p_Var4)(*(void **)((int)aiStackY_2a0 + iVar6 + 0x14),
                    *(void **)((int)aMStackY_288 + iVar6),*(void **)((int)aMStackY_288 + iVar6 + 4),
                    *(float *)((int)&fStackY_280 + iVar6),*(int *)(&stack0xfffffd84 + iVar6));
  MStack_26c.flags = (McdTriangleFlags)result->normal;
  result->normal[0] = 0.0;
  result->normal[1] = 0.0;
  result->normal[2] = 0.0;
  uVar29 = 0;
  if (count != 0) {
    ct.normal = vector[0].v;
    ct.vertices[0] = vector[1].v;
    ct.vertices[1] = vector[2].v;
    ct.vertices[2] = vector[3].v;
    fVar32 = pfVar27[2] * pfVar28[2] + pfVar27[1] * pfVar28[1] + *pfVar27 * *pfVar28;
    fVar7 = pfVar27[6] * pfVar28[2] + pfVar27[5] * pfVar28[1] + pfVar27[4] * *pfVar28;
    fVar8 = pfVar27[10] * pfVar28[2] + pfVar27[9] * pfVar28[1] + pfVar27[8] * *pfVar28;
    fVar9 = pfVar27[2] * pfVar28[6] + pfVar27[1] * pfVar28[5] + *pfVar27 * pfVar28[4];
    fVar10 = pfVar27[6] * pfVar28[6] + pfVar27[5] * pfVar28[5] + pfVar27[4] * pfVar28[4];
    fVar11 = pfVar27[10] * pfVar28[6] + pfVar27[9] * pfVar28[5] + pfVar27[8] * pfVar28[4];
    fVar12 = pfVar27[2] * pfVar28[10] + pfVar27[1] * pfVar28[9] + *pfVar27 * pfVar28[8];
    fVar13 = pfVar27[6] * pfVar28[10] + pfVar27[5] * pfVar28[9] + pfVar27[4] * pfVar28[8];
    fVar14 = pfVar27[10] * pfVar28[10] + pfVar27[9] * pfVar28[9] + pfVar27[8] * pfVar28[8];
    diff.v[1] = local_254[1] - pfVar27[0xd];
    diff.v[2] = local_254[2] - pfVar27[0xe];
    diff.v[0] = pfVar28[0xc] - pfVar27[0xc];
    fVar15 = diff.v[2] * pfVar27[2] + diff.v[0] * *pfVar27 + diff.v[1] * pfVar27[1];
    fVar17 = diff.v[2] * pfVar27[6] + diff.v[0] * pfVar27[4] + diff.v[1] * pfVar27[5];
    j = 0;
    fVar16 = diff.v[0] * pfVar27[8] + diff.v[1] * pfVar27[9] + diff.v[2] * pfVar27[10];
    if ((0 < count) && (result->contactCount < 400)) {
      MStack_26c.triangleData.ptr = (void *)0x0;
      do {
        pMVar24 = fwk;
        puVar31 = (undefined4 *)((int)triList->list->vertices + (int)MStack_26c.triangleData.ptr);
        pfVar28 = (float *)*puVar31;
        (*ct.vertices[0])[0] = fVar12 * pfVar28[2] + fVar32 * *pfVar28 + fVar9 * pfVar28[1] + fVar15
        ;
        (*ct.vertices[0])[1] = fVar13 * pfVar28[2] + fVar10 * pfVar28[1] + fVar7 * *pfVar28 + fVar17
        ;
        (*ct.vertices[0])[2] = fVar14 * pfVar28[2] + fVar11 * pfVar28[1] + fVar8 * *pfVar28 + fVar16
        ;
        pfVar28 = (float *)puVar31[1];
        (*ct.vertices[1])[0] = fVar12 * pfVar28[2] + fVar9 * pfVar28[1] + fVar32 * *pfVar28 + fVar15
        ;
        (*ct.vertices[1])[1] = fVar13 * pfVar28[2] + fVar10 * pfVar28[1] + fVar7 * *pfVar28 + fVar17
        ;
        (*ct.vertices[1])[2] = fVar14 * pfVar28[2] + fVar11 * pfVar28[1] + fVar8 * *pfVar28 + fVar16
        ;
        pfVar28 = (float *)puVar31[2];
        (*ct.vertices[2])[0] = fVar12 * pfVar28[2] + fVar9 * pfVar28[1] + fVar32 * *pfVar28 + fVar15
        ;
        (*ct.vertices[2])[1] = fVar13 * pfVar28[2] + fVar10 * pfVar28[1] + fVar7 * *pfVar28 + fVar17
        ;
        (*ct.vertices[2])[2] = fVar14 * pfVar28[2] + fVar11 * pfVar28[1] + fVar8 * *pfVar28 + fVar16
        ;
        pfVar28 = (float *)puVar31[3];
        (*ct.normal)[0] = fVar12 * pfVar28[2] + fVar9 * pfVar28[1] + fVar32 * *pfVar28;
        (*ct.normal)[1] = fVar13 * pfVar28[2] + fVar10 * pfVar28[1] + fVar7 * *pfVar28;
        (*ct.normal)[2] = fVar14 * pfVar28[2] + fVar11 * pfVar28[1] + fVar8 * *pfVar28;
        ct.flags = puVar31[5];
        ct.triangleData = *(anon_union_4_2_43add64d_for_triangleData *)(puVar31 + 4);
        edge[0][0] = (*ct.vertices[1])[0] - (*ct.vertices[0])[0];
        edge[0][1] = (*ct.vertices[1])[1] - (*ct.vertices[0])[1];
        edge[0][2] = (*ct.vertices[1])[2] - (*ct.vertices[0])[2];
        edge[1][0] = (*ct.vertices[2])[0] - (*ct.vertices[1])[0];
        edge[1][1] = (*ct.vertices[2])[1] - (*ct.vertices[1])[1];
        edge[1][2] = (*ct.vertices[2])[2] - (*ct.vertices[1])[2];
        edge[2][0] = (*ct.vertices[0])[0] - (*ct.vertices[2])[0];
        edge[2][1] = (*ct.vertices[0])[1] - (*ct.vertices[2])[1];
        fVar2 = (*ct.vertices[2])[2];
        fVar3 = (*ct.vertices[0])[2];
        verts = footprint;
        *(lsVec3 **)((int)&iStack_270 + iVar6) = footprint;
        edge[2][2] = fVar3 - fVar2;
        *(MeReal *)(&stack0xfffffd8c + iVar6) = pMVar24->mScale;
        MVar21 = cylHH;
        *(MeReal (**) [3])(&stack0xfffffd88 + iVar6) = edge;
        MVar22 = cylRadius;
        *(McdUserTriangle **)(&stack0xfffffd84 + iVar6) = &ct;
        MVar23 = eps;
        *(MeReal *)((int)&fStackY_280 + iVar6) = MVar21;
        *(MeReal *)((int)aMStackY_288 + iVar6 + 4) = MVar22;
        *(MeReal *)((int)aMStackY_288 + iVar6) = MVar23;
        *(MeI16 **)((int)aiStackY_2a0 + iVar6 + 0x14) = &dims;
        *(lsVec3 ***)((int)aiStackY_2a0 + iVar6 + 0x10) = &verts;
        *(MeReal **)((int)aiStackY_2a0 + iVar6 + 0xc) = &PN;
        *(lsVec3 **)((int)aiStackY_2a0 + iVar6 + 8) = &normal;
        *(MeReal **)((int)aiStackY_2a0 + iVar6 + 4) = &separation;
        *(undefined4 *)((int)aiStackY_2a0 + iVar6) = 0x10658;
        bVar26 = McdVanillaOverlapCylTri
                           (*(MeReal **)((int)aiStackY_2a0 + iVar6 + 4),
                            *(lsVec3 **)((int)aiStackY_2a0 + iVar6 + 8),
                            *(MeReal **)((int)aiStackY_2a0 + iVar6 + 0xc),
                            *(lsVec3 ***)((int)aiStackY_2a0 + iVar6 + 0x10),
                            *(MeI16 **)((int)aiStackY_2a0 + iVar6 + 0x14),
                            *(MeReal *)((int)aMStackY_288 + iVar6),
                            *(MeReal *)((int)aMStackY_288 + iVar6 + 4),
                            *(MeReal *)((int)&fStackY_280 + iVar6),
                            *(McdUserTriangle **)(&stack0xfffffd84 + iVar6),
                            *(MeReal (**) [3])(&stack0xfffffd88 + iVar6),
                            *(MeReal *)(&stack0xfffffd8c + iVar6));
        if (bVar26) {
                    /* Unresolved local var: lsVec3 * v@[DW_OP_reg3(EBX)] */
          diff.v[0] = normal.v[2] * pfVar27[8] + normal.v[0] * *pfVar27 + normal.v[1] * pfVar27[4];
          diff.v[1] = normal.v[2] * pfVar27[9] + normal.v[0] * pfVar27[1] + normal.v[1] * pfVar27[5]
          ;
          diff.v[2] = normal.v[2] * pfVar27[10] +
                      normal.v[0] * pfVar27[2] + normal.v[1] * pfVar27[6];
          if (((ct.flags & kMcdTriangleTwoSided) == 0) &&
             (normal.v[0] * (*ct.normal)[0] + normal.v[1] * (*ct.normal)[1] +
              normal.v[2] * (*ct.normal)[2] < 0.0)) {
            diff.v[0] = diff.v[0] * -1.0;
            diff.v[1] = diff.v[1] * -1.0;
            diff.v[2] = diff.v[2] * -1.0;
          }
          plVar30 = footprint;
          if (plVar30 != verts) {
            iVar5 = result->contactCount;
            while (iVar5 < 400) {
                    /* Unresolved local var: McdContact * c@[DW_OP_reg2(EDX)] */
              pMVar1 = result->contacts + iVar5;
              result->contactCount = iVar5 + 1;
                    /* Unresolved local var: lsVec3 * cpos@[???] */
              pMVar1->position[0] =
                   pfVar27[8] * plVar30->v[2] +
                   pfVar27[4] * plVar30->v[1] + *pfVar27 * plVar30->v[0] + pfVar27[0xc];
              pMVar1->position[1] =
                   pfVar27[9] * plVar30->v[2] +
                   pfVar27[5] * plVar30->v[1] + pfVar27[1] * plVar30->v[0] + pfVar27[0xd];
              pMVar18 = plVar30->v;
              pMVar19 = plVar30->v;
              pMVar20 = plVar30->v;
              plVar30 = plVar30 + 1;
              pMVar1->position[2] =
                   pfVar27[10] * pMVar20[2] + pfVar27[6] * pMVar19[1] + pfVar27[2] * *pMVar18 +
                   pfVar27[0xe];
              pMVar1->dims = dims;
              pMVar1->separation = separation;
              pMVar1->element2 = *(anon_union_4_2_43add64d_for_element2 *)(puVar31 + 4);
              pMVar1->normal[0] = diff.v[0];
              pMVar1->normal[1] = diff.v[1];
              pMVar1->normal[2] = diff.v[2];
              if (plVar30 == verts) break;
              iVar5 = result->contactCount;
            }
          }
          result->normal[0] = diff.v[0] + result->normal[0];
          *(float *)(MStack_26c.flags + 4) = diff.v[1] + *(MeReal *)(MStack_26c.flags + 4);
          *(float *)(MStack_26c.flags + 8) = diff.v[2] + *(MeReal *)(MStack_26c.flags + 8);
        }
        j = j + 1;
        MStack_26c.triangleData.ptr = (void *)(MStack_26c.triangleData.tag + 0x18);
      } while ((j < count) && (result->contactCount < 400));
    }
    *(McdTriangleFlags *)(&stack0xfffffd84 + iVar6) = MStack_26c.flags;
    *(undefined4 *)((int)&fStackY_280 + iVar6) = 0x10890;
    MeVector3Normalize(*(MeReal **)(&stack0xfffffd84 + iVar6));
    uVar29 = (uint)(0 < result->contactCount);
    result->touch = uVar29;
  }
  return uVar29;
}


/* ==== McdCylinderTriangleListRegisterInteraction ==== */

MeBool McdCylinderTriangleListRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdCylinderTriangleListIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,4,6,&interactions);
  return 1;
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


/* ==== McdVanillaOverlapCylTri ==== */

bool McdVanillaOverlapCylTri
               (MeReal *outSep,lsVec3 *outN,MeReal *outPN,lsVec3 **outPos,MeI16 *outDims,
               MeReal inEps,MeReal inRCyl,MeReal inHHCyl,McdUserTriangle *inTri,MeReal (*edge) [3],
               MeReal scale)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  MeVector3 *paMVar6;
  MeVector3 *paMVar7;
  lsVec3 *plVar8;
  MeVector3 *paMVar9;
  lsVec3 *plVar10;
  lsVec3 *plVar11;
  uint uVar12;
  McdTriangleFlags MVar13;
  ushort uVar14;
  int iVar15;
  int iVar16;
  bool bVar17;
  lsVec3 *posList;
  MeReal minCoord;
  MeReal nRLen;
  MeReal PN;
  MeU8 normInfo;
  MeReal normalSign;
  MeReal maxSeparation;
  MeReal eps2;
  MeReal tOut;
  MeReal tIn;
  MeReal tOut_1;
  MeReal tIn_1;
  MeReal tOut_2;
  MeReal tIn_2;
  lsVec3 axb [3];
  lsVec3 outVec_1;
  lsVec3 outVec;
  lsVec3 nPerp;
  lsVec3 nPara;
  MeReal sqE [3];
  lsVec3 aE [3];
  lsVec3 nPerpN;
  lsVec3 nParaN;
  
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal separation@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal sumR@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal normD@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeI16 dimA@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal maxCoord@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal ZZ2@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal triD@[???] */
  nRLen = 1.0;
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
  paMVar6 = inTri->normal;
  nParaN.v[0] = -(*paMVar6)[1];
  nParaN.v[1] = (*paMVar6)[0];
  nParaN.v[2] = 0.0;
  if (1e-15 <= nParaN.v[1] * nParaN.v[1] + nParaN.v[0] * nParaN.v[0]) {
                    /* Unresolved local var: MeReal t@[???]
                       Unresolved local var: float __result@[DW_OP_reg14(ST3)] */
    fVar1 = SQRT(nParaN.v[1] * nParaN.v[1] + nParaN.v[0] * nParaN.v[0] + 0.0);
    if (fVar1 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg14(ST3)] */
      fVar1 = 1.0 / fVar1;
      nParaN.v[0] = nParaN.v[0] * fVar1;
      nParaN.v[1] = nParaN.v[1] * fVar1;
      nParaN.v[2] = fVar1 * 0.0;
    }
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
    nPerpN.v[1] = -nParaN.v[0];
  }
  else {
    nPerpN.v[1] = 0.0;
    nParaN.v[0] = 0.0;
    nParaN.v[1] = 1.0;
  }
  fVar1 = (*paMVar6)[2];
  paMVar7 = inTri->vertices[0];
  fVar2 = (*paMVar6)[0] * (*paMVar7)[0] + (*paMVar6)[1] * (*paMVar7)[1] + fVar1 * (*paMVar7)[2];
  normalSign = 1.0;
  fVar1 = ABS(fVar1) * inHHCyl +
          ABS(nParaN.v[1] * (*paMVar6)[0] + nPerpN.v[1] * (*paMVar6)[1] + fVar1 * 0.0) * inRCyl;
  maxSeparation = ABS(fVar2) - fVar1;
  if (0.0 < fVar2) {
    normalSign = -1.0;
  }
  bVar17 = false;
  normInfo = '\f';
  if (maxSeparation <= inEps) {
    if (((inTri->flags & kMcdTriangleTwoSided) == 0) && (normalSign < 0.0)) {
      maxSeparation = -maxSeparation;
      normalSign = 1.0;
      maxSeparation = maxSeparation - (fVar1 + fVar1);
    }
    nPara.v[0] = nParaN.v[0];
    nPerp.v[0] = nParaN.v[1];
    nPerp.v[1] = nPerpN.v[1];
    nPara.v[1] = nParaN.v[1];
    nPara.v[2] = nParaN.v[2];
    fVar1 = normalSign * (*paMVar6)[0];
    fVar2 = normalSign * (*paMVar6)[1];
    fVar3 = normalSign * (*paMVar6)[2];
    outN->v[0] = fVar1;
    outN->v[1] = fVar2;
    outN->v[2] = fVar3;
    paMVar6 = inTri->vertices[0];
    uVar14 = 2;
    *outPN = (fVar3 * (*paMVar6)[2] + fVar1 * (*paMVar6)[0] + fVar2 * (*paMVar6)[1]) - maxSeparation
    ;
    fVar1 = outN->v[2] * outN->v[2];
    if ((fVar1 <= 0.999999) && (uVar14 = 3, 1e-06 <= fVar1)) {
      uVar14 = 1;
    }
    *outDims = uVar14 | 0x200;
    fVar1 = (*inTri->vertices[0])[2];
    fVar2 = (*inTri->vertices[1])[2];
    minCoord = fVar1;
    if (fVar2 < fVar1) {
      minCoord = fVar2;
    }
    fVar2 = (*inTri->vertices[1])[2];
    if (fVar1 < fVar2) {
      fVar1 = fVar2;
    }
    fVar2 = (*inTri->vertices[2])[2];
    if (fVar2 < minCoord) {
      minCoord = fVar2;
    }
    fVar2 = (*inTri->vertices[2])[2];
    if (fVar1 < fVar2) {
      fVar1 = fVar2;
    }
    fVar2 = (minCoord + fVar1) * 0.5;
    fVar1 = ABS(fVar2) - ((fVar1 - minCoord) * 0.5 + inHHCyl);
    if (inEps < fVar1 - maxSeparation) {
      normalSign = 1.0;
      if (0.0 < fVar2) {
        normalSign = -1.0;
      }
      normInfo = '\v';
      maxSeparation = fVar1;
      if (inEps < fVar1) {
        return false;
      }
    }
    fVar1 = (*edge)[0];
    fVar2 = (*edge)[1];
    aE[0].v[0] = ABS(fVar1);
    fVar3 = (*edge)[2];
    aE[0].v[1] = ABS(fVar2);
    aE[0].v[2] = ABS(fVar3);
    fVar4 = edge[1][0];
    aE[1].v[0] = ABS(fVar4);
    aE[1].v[1] = ABS(edge[1][1]);
    aE[1].v[2] = ABS(edge[1][2]);
    fVar5 = edge[2][0];
    sqE[0] = fVar1 * fVar1 + fVar2 * fVar2 + fVar3 * fVar3;
    aE[2].v[0] = ABS(fVar5);
    aE[2].v[2] = ABS(edge[2][2]);
    aE[2].v[1] = ABS(edge[2][1]);
    iVar16 = 0;
    iVar15 = 0;
    sqE[1] = edge[1][2] * edge[1][2] + fVar4 * fVar4 + edge[1][1] * edge[1][1];
    sqE[2] = edge[2][2] * edge[2][2] + fVar5 * fVar5 + edge[2][1] * edge[2][1];
    do {
                    /* Unresolved local var: int i1@[???]
                       Unresolved local var: MeReal sR@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal rLen@[DW_OP_reg11(ST0)] */
      uVar12 = 1 << ((byte)iVar15 & 0x1f) & 3;
      fVar1 = *(float *)((int)*edge + iVar16 + 4);
      fVar2 = *(float *)((int)*edge + iVar16);
      fVar3 = *(float *)((int)aE[0].v + iVar16 + 8);
      fVar4 = (edge[uVar12][0] * fVar1 - edge[uVar12][1] * fVar2) * 0.5;
      fVar3 = sqE[iVar15] - fVar3 * fVar3;
      fVar1 = ((*inTri->vertices[iVar15])[0] * fVar1 - fVar2 * (*inTri->vertices[iVar15])[1]) +
              fVar4;
      if (inEps * inEps < fVar3) {
                    /* Unresolved local var: MeReal aNormD@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal rB@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal separation@[DW_OP_reg13(ST2)]
                       Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
        fVar5 = 1.0 / SQRT(fVar3);
        fVar2 = ABS(fVar4) * fVar5;
        fVar4 = ABS(fVar1) * fVar5;
        fVar3 = fVar4 - (inRCyl + fVar2);
        if (inEps < fVar3 - maxSeparation) {
          normalSign = 1.0;
          if (0.0 < fVar1) {
            normalSign = -1.0;
          }
          PN = fVar2 - fVar4;
          normInfo = (byte)iVar15 | 8;
          nRLen = fVar5;
          maxSeparation = fVar3;
          if (inEps < fVar3) {
            return false;
          }
        }
      }
      iVar15 = iVar15 + 1;
      iVar16 = iVar16 + 0xc;
    } while (iVar15 < 3);
    *outSep = maxSeparation;
    plVar8 = *outPos;
    if ((inTri->flags & kMcdTriangleUseSmallestPenetration) != 0) {
      if ((normInfo & 3) == 3) {
                    /* Unresolved local var: MeI16 dimB@[???] */
        nPerp.v[0] = 1.0;
        nPerp.v[1] = 0.0;
        nPara.v[0] = 0.0;
        nPara.v[1] = normalSign;
        nPara.v[2] = 0.0;
        outN->v[0] = 0.0;
        outN->v[2] = normalSign;
        outN->v[1] = 0.0;
        *outPN = -inHHCyl;
        uVar14 = (ushort)((*inTri->vertices[0])[2] - minCoord <= inEps);
        if ((*inTri->vertices[1])[2] - minCoord <= inEps) {
          uVar14 = uVar14 + 1;
        }
        if (inEps < (*inTri->vertices[2])[2] - minCoord) {
          uVar14 = uVar14 - 1;
        }
        *outDims = uVar14 << 8 | 2;
      }
      else if (normInfo != '\f') {
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
        nPerp.v[0] = nRLen * edge[normInfo & 3][1];
        nPerp.v[1] = -edge[normInfo & 3][0] * nRLen;
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
        nPara.v[0] = -nPerp.v[1];
        nPara.v[2] = 0.0;
        outN->v[0] = nPerp.v[0] * normalSign;
        outN->v[1] = nPerp.v[1] * normalSign;
        outN->v[2] = 0.0;
        *outPN = PN;
        *outDims = 0x103;
        nPara.v[1] = nPerp.v[0];
      }
    }
    nPerp.v[2] = 0.0;
    paMVar6 = inTri->vertices[1];
    paMVar7 = inTri->vertices[0];
    axb[0].v[0] = (*paMVar6)[2] * (*paMVar7)[1] - (*paMVar6)[1] * (*paMVar7)[2];
    axb[0].v[1] = (*paMVar6)[0] * (*paMVar7)[2] - (*paMVar6)[2] * (*paMVar7)[0];
    axb[0].v[2] = (*paMVar6)[1] * (*paMVar7)[0] - (*paMVar6)[0] * (*paMVar7)[1];
    paMVar9 = inTri->vertices[2];
    axb[1].v[0] = (*paMVar9)[2] * (*paMVar6)[1] - (*paMVar9)[1] * (*paMVar6)[2];
    axb[1].v[1] = (*paMVar9)[0] * (*paMVar6)[2] - (*paMVar9)[2] * (*paMVar6)[0];
    axb[1].v[2] = (*paMVar9)[1] * (*paMVar6)[0] - (*paMVar9)[0] * (*paMVar6)[1];
    axb[2].v[0] = (*paMVar7)[2] * (*paMVar9)[1] - (*paMVar7)[1] * (*paMVar9)[2];
    axb[2].v[1] = (*paMVar7)[0] * (*paMVar9)[2] - (*paMVar7)[2] * (*paMVar9)[0];
    axb[2].v[2] = (*paMVar7)[1] * (*paMVar9)[0] - (*paMVar7)[0] * (*paMVar9)[1];
    paMVar6 = inTri->normal;
    McdVanillaAddTriCylSegmentPoints
              (outPos,inRCyl,inHHCyl,&nPerp,&nPara,inTri,edge,axb,
               (*paMVar6)[2] * (*paMVar7)[2] +
               (*paMVar6)[1] * (*paMVar7)[1] + (*paMVar6)[0] * (*paMVar7)[0]);
    MVar13 = inTri->flags;
    if ((MVar13 & kMcdTriangleUseEdge0) != 0) {
      plVar10 = (lsVec3 *)inTri->vertices[0];
      tOut_2 = 1.0;
      tIn_2 = 0.0;
      bVar17 = McdVanillaSegmentCylinderIntersect
                         (&tIn_2,&tOut_2,plVar10,(lsVec3 *)edge,inRCyl,inHHCyl,scale);
      if (bVar17) {
        plVar11 = *outPos;
        *outPos = plVar11 + 1;
        fVar1 = (*edge)[1];
        fVar2 = (*edge)[2];
        fVar3 = plVar10->v[1];
        fVar4 = plVar10->v[2];
        plVar11->v[0] = tIn_2 * (*edge)[0] + plVar10->v[0];
        plVar11->v[1] = tIn_2 * fVar1 + fVar3;
        plVar11->v[2] = tIn_2 * fVar2 + fVar4;
        if (tOut_2 < 1.0) {
          plVar11 = *outPos;
          *outPos = plVar11 + 1;
          fVar1 = (*edge)[1];
          fVar2 = (*edge)[2];
          fVar3 = plVar10->v[1];
          fVar4 = plVar10->v[2];
          plVar11->v[0] = tOut_2 * (*edge)[0] + plVar10->v[0];
          plVar11->v[1] = tOut_2 * fVar1 + fVar3;
          plVar11->v[2] = tOut_2 * fVar2 + fVar4;
        }
      }
      MVar13 = inTri->flags;
    }
    if ((MVar13 & kMcdTriangleUseEdge1) != 0) {
      plVar10 = (lsVec3 *)inTri->vertices[1];
      tOut_1 = 1.0;
      tIn_1 = 0.0;
      bVar17 = McdVanillaSegmentCylinderIntersect
                         (&tIn_1,&tOut_1,plVar10,(lsVec3 *)(edge + 1),inRCyl,inHHCyl,scale);
      if (bVar17) {
        plVar11 = *outPos;
        *outPos = plVar11 + 1;
        fVar1 = edge[1][1];
        fVar2 = edge[1][2];
        fVar3 = plVar10->v[1];
        fVar4 = plVar10->v[2];
        plVar11->v[0] = tIn_1 * edge[1][0] + plVar10->v[0];
        plVar11->v[1] = tIn_1 * fVar1 + fVar3;
        plVar11->v[2] = tIn_1 * fVar2 + fVar4;
        if (tOut_1 < 1.0) {
          plVar11 = *outPos;
          *outPos = plVar11 + 1;
          fVar1 = edge[1][1];
          fVar2 = edge[1][2];
          fVar3 = plVar10->v[1];
          fVar4 = plVar10->v[2];
          plVar11->v[0] = tOut_1 * edge[1][0] + plVar10->v[0];
          plVar11->v[1] = tOut_1 * fVar1 + fVar3;
          plVar11->v[2] = tOut_1 * fVar2 + fVar4;
        }
      }
      MVar13 = inTri->flags;
    }
    if ((MVar13 & kMcdTriangleUseEdge2) != 0) {
      plVar10 = (lsVec3 *)inTri->vertices[2];
      tOut = 1.0;
      tIn = 0.0;
      bVar17 = McdVanillaSegmentCylinderIntersect
                         (&tIn,&tOut,plVar10,(lsVec3 *)(edge + 2),inRCyl,inHHCyl,scale);
      if (bVar17) {
        plVar11 = *outPos;
        *outPos = plVar11 + 1;
        fVar1 = edge[2][1];
        fVar2 = edge[2][2];
        fVar3 = plVar10->v[1];
        fVar4 = plVar10->v[2];
        plVar11->v[0] = tIn * edge[2][0] + plVar10->v[0];
        plVar11->v[1] = tIn * fVar1 + fVar3;
        plVar11->v[2] = tIn * fVar2 + fVar4;
        if (tOut < 1.0) {
          plVar11 = *outPos;
          *outPos = plVar11 + 1;
          fVar1 = edge[2][1];
          fVar2 = edge[2][2];
          fVar3 = plVar10->v[1];
          fVar4 = plVar10->v[2];
          plVar11->v[0] = tOut * edge[2][0] + plVar10->v[0];
          plVar11->v[1] = tOut * fVar1 + fVar3;
          plVar11->v[2] = tOut * fVar2 + fVar4;
        }
      }
    }
    bVar17 = *outPos != plVar8;
  }
  return bVar17;
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
            goto LAB_000119c9;
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
LAB_000119c9:
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


/* ==== McdVanillaAddTriCylSegmentPoints ==== */

void McdVanillaAddTriCylSegmentPoints
               (lsVec3 **outList,MeReal inRCyl,MeReal inHHCyl,lsVec3 *e0,lsVec3 *e1,
               McdUserTriangle *inTri,MeReal (*edge) [3],lsVec3 *axb,MeReal inTriD)

{
  float fVar1;
  float fVar2;
  float fVar3;
  lsVec3 *plVar4;
  bool bVar5;
  bool bVar6;
  bool bVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  MeVector3 *paMVar17;
  MeReal dz;
  MeReal x2;
  MeReal x1;
  MeReal p2;
  bool ccwPerp;
  bool ccw;
  MeReal y [3];
  MeReal x [3];
  
                    /* Unresolved local var: MeReal den@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal denPerp@[???] */
  paMVar17 = inTri->normal;
  fVar1 = (*paMVar17)[2];
  bVar5 = fVar1 < 0.0;
  if (fVar1 != 0.0) {
                    /* Unresolved local var: MeReal recipDen@[???]
                       Unresolved local var: MeReal dx@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal dy@[DW_OP_reg14(ST3)] */
    fVar1 = 1.0 / fVar1;
    fVar10 = (e0->v[0] * (*edge)[1] - e0->v[1] * (*edge)[0]) * inRCyl;
    fVar3 = (e0->v[0] * edge[1][1] - e0->v[1] * edge[1][0]) * inRCyl;
    fVar12 = inRCyl * (e0->v[0] * edge[2][1] - e0->v[1] * edge[2][0]);
    fVar9 = ((*edge)[1] * e1->v[0] - (*edge)[0] * e1->v[1]) * inRCyl;
    fVar8 = (edge[1][1] * e1->v[0] - edge[1][0] * e1->v[1]) * inRCyl;
    fVar11 = (edge[2][1] * e1->v[0] - edge[2][0] * e1->v[1]) * inRCyl;
    fVar2 = axb->v[2];
    fVar13 = ((*paMVar17)[0] * e1->v[0] + (*paMVar17)[1] * e1->v[1] + (*paMVar17)[2] * e1->v[2]) *
             inRCyl;
    bVar6 = fVar2 - fVar10 < 0.0;
    fVar14 = ((*paMVar17)[0] * e0->v[0] + (*paMVar17)[1] * e0->v[1] + (*paMVar17)[2] * e0->v[2]) *
             inRCyl;
    if ((((bVar5 == bVar6) && (bVar7 = axb[1].v[2] - fVar3 < 0.0, bVar7 == bVar6)) &&
        (axb[2].v[2] - fVar12 < 0.0 == bVar7)) &&
       ((fVar15 = (inTriD - fVar14) * fVar1, -inHHCyl <= fVar15 && (fVar15 <= inHHCyl)))) {
      (*outList)->v[0] = e0->v[0] * inRCyl;
      (*outList)->v[1] = inRCyl * e0->v[1];
      (*outList)->v[2] = fVar15;
      *outList = *outList + 1;
      fVar2 = axb->v[2];
    }
    bVar6 = fVar2 + fVar10 < 0.0;
    if (((bVar5 == bVar6) && (bVar7 = fVar3 + axb[1].v[2] < 0.0, bVar7 == bVar6)) &&
       ((fVar12 + axb[2].v[2] < 0.0 == bVar7 &&
        ((fVar3 = (inTriD + fVar14) * fVar1, -inHHCyl <= fVar3 && (fVar3 <= inHHCyl)))))) {
      (*outList)->v[0] = -inRCyl * e0->v[0];
      (*outList)->v[1] = -inRCyl * e0->v[1];
      (*outList)->v[2] = fVar3;
      *outList = *outList + 1;
      fVar2 = axb->v[2];
    }
    bVar6 = fVar2 - fVar9 < 0.0;
    if ((((bVar5 == bVar6) && (bVar7 = axb[1].v[2] - fVar8 < 0.0, bVar7 == bVar6)) &&
        (axb[2].v[2] - fVar11 < 0.0 == bVar7)) &&
       ((fVar3 = (inTriD - fVar13) * fVar1, -inHHCyl <= fVar3 && (fVar3 <= inHHCyl)))) {
      (*outList)->v[0] = inRCyl * e1->v[0];
      (*outList)->v[1] = inRCyl * e1->v[1];
      (*outList)->v[2] = fVar3;
      *outList = *outList + 1;
      fVar2 = axb->v[2];
    }
    bVar6 = fVar9 + fVar2 < 0.0;
    if (((bVar5 == bVar6) && (bVar5 = fVar8 + axb[1].v[2] < 0.0, bVar5 == bVar6)) &&
       ((fVar11 + axb[2].v[2] < 0.0 == bVar5 &&
        ((fVar1 = (fVar13 + inTriD) * fVar1, -inHHCyl <= fVar1 && (fVar1 <= inHHCyl)))))) {
      (*outList)->v[0] = -inRCyl * e1->v[0];
      (*outList)->v[1] = -inRCyl * e1->v[1];
      (*outList)->v[2] = fVar1;
      *outList = *outList + 1;
    }
    paMVar17 = inTri->normal;
  }
  fVar1 = e0->v[0];
  fVar2 = e0->v[1];
  fVar3 = e0->v[2];
  fVar8 = (*paMVar17)[2] * fVar3 + (*paMVar17)[0] * fVar1 + (*paMVar17)[1] * fVar2;
  if (fVar8 != 0.0) {
                    /* Unresolved local var: MeReal recipDenPerp@[???]
                       Unresolved local var: MeReal p0@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal p1@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal x0@[DW_OP_reg18(ST7)] */
    fVar11 = axb->v[2] * fVar3 + axb->v[0] * fVar1 + axb->v[1] * fVar2;
    fVar10 = axb[1].v[2] * fVar3 + axb[1].v[1] * fVar2 + axb[1].v[0] * fVar1;
    fVar9 = axb[2].v[2] * fVar3 + axb[2].v[1] * fVar2 + axb[2].v[0] * fVar1;
    fVar12 = ((*edge)[1] * fVar1 - (*edge)[0] * fVar2) * inHHCyl;
    fVar14 = inHHCyl * (edge[1][1] * fVar1 - edge[1][0] * fVar2);
    fVar15 = inHHCyl * (edge[2][1] * fVar1 - edge[2][0] * fVar2);
    fVar13 = (*paMVar17)[2] * inHHCyl;
    bVar5 = fVar11 + fVar12 < 0.0;
    if ((((fVar8 < 0.0 == bVar5) && (bVar6 = fVar14 + fVar10 < 0.0, bVar6 == bVar5)) &&
        (fVar9 + fVar15 < 0.0 == bVar6)) &&
       ((fVar16 = (inTriD - fVar13) * (1.0 / fVar8), -inRCyl <= fVar16 && (fVar16 <= inRCyl)))) {
      plVar4 = *outList;
      plVar4->v[0] = fVar1 * fVar16;
      plVar4->v[1] = fVar2 * fVar16;
      plVar4->v[2] = fVar3 * fVar16;
      (*outList)->v[2] = inHHCyl + (*outList)->v[2];
      *outList = *outList + 1;
    }
    bVar5 = fVar11 - fVar12 < 0.0;
    if (((fVar8 < 0.0 == bVar5) && (bVar6 = fVar10 - fVar14 < 0.0, bVar6 == bVar5)) &&
       ((fVar9 - fVar15 < 0.0 == bVar6 &&
        ((fVar1 = (inTriD + fVar13) * (1.0 / fVar8), -inRCyl <= fVar1 && (fVar1 <= inRCyl)))))) {
      fVar2 = e0->v[1];
      plVar4 = *outList;
      fVar3 = e0->v[2];
      plVar4->v[0] = fVar1 * e0->v[0];
      plVar4->v[1] = fVar1 * fVar2;
      plVar4->v[2] = fVar1 * fVar3;
      (*outList)->v[2] = (*outList)->v[2] - inHHCyl;
      *outList = *outList + 1;
    }
  }
  return;
}


