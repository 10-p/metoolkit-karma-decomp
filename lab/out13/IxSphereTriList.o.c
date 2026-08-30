/* ==== McdSphereTriangleListIntersect ==== */

int McdSphereTriangleListIntersect(McdModelPair *p,McdIntersectResult *result)

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
  code *pcVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  int iVar15;
  MeReal MVar16;
  McdTriangleListID pMVar17;
  MeReal MVar18;
  MeVector3 *paMVar19;
  bool bVar20;
  void *pvVar21;
  float *pfVar22;
  uint uVar23;
  undefined4 *puVar24;
  McdContact *pMVar25;
  float fVar26;
  int aiStackY_180 [5];
  MeReal aMStackY_16c [2];
  int iStackY_164;
  float fStackY_160;
  int local_144;
  MeReal eps;
  float *local_13c;
  McdFramework *fwk;
  McdTriangleListID trilistgeom;
  int count;
  MeReal sphereRadius;
  int i;
  MeReal separation;
  MeI16 dims;
  lsVec3 diff;
  lsTransform triToSphere;
  MeReal edge [3] [3];
  lsVec3 vector [4];
  lsVec3 transPos;
  lsVec3 normal;
  lsVec3 pos;
  McdUserTriangle ct;
  
                    /* Unresolved local var: lsVec3 * spherePos@[???]
                       Unresolved local var: McdUserTriangle * triNE@[DW_OP_reg3(EBX)]
                       Unresolved local var: lsTransform * triListTM@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdSphereID spheregeom@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdTriangleList * triList@[???]
                       Unresolved local var: McdContact * c@[DW_OP_reg6(ESI)] */
  fStackY_160 = 9.18677e-41;
  pvVar21 = McdModelGetGeometry(p->model1);
  fStackY_160 = 9.18845e-41;
  trilistgeom = McdModelGetGeometry(p->model2);
  fwk = p->model1->frame;
  fStackY_160 = 9.19182e-41;
  sphereRadius = McdSphereGetRadius(pvVar21);
  fStackY_160 = 9.19392e-41;
  pvVar21 = McdModelGetTransformPtr(p->model1);
  local_13c = (float *)((int)pvVar21 + 0x30);
  fStackY_160 = 9.19686e-41;
  pfVar22 = McdModelGetTransformPtr(p->model2);
  fVar2 = *pfVar22;
  fVar3 = pfVar22[1];
  fVar4 = pfVar22[2];
  fVar5 = pfVar22[4];
  fVar6 = pfVar22[5];
  fVar7 = pfVar22[6];
  fVar8 = pfVar22[8];
  fVar9 = pfVar22[9];
  fVar10 = pfVar22[10];
  fVar12 = pfVar22[0xc] - *(float *)((int)pvVar21 + 0x30);
  fVar13 = pfVar22[0xd] - local_13c[1];
  fVar14 = pfVar22[0xe] - local_13c[2];
  fStackY_160 = 9.22461e-41;
  eps = McdModelGetContactTolerance(p->model1);
  fStackY_160 = 9.22685e-41;
  fVar26 = McdModelGetContactTolerance(p->model2);
  pMVar17 = trilistgeom;
  eps = fVar26 + eps;
  iVar15 = -((int)trilistgeom[2].next * 0x18 + 0xfU & 0xfffffff0);
  trilistgeom[3].prev = (McdGeometryID)(&stack0xfffffeb4 + iVar15);
  diff.v[0] = *(float *)((int)pvVar21 + 0x30) - pfVar22[0xc];
  diff.v[1] = local_13c[1] - pfVar22[0xd];
  diff.v[2] = local_13c[2] - pfVar22[0xe];
  transPos.v[0] = diff.v[2] * pfVar22[2] + diff.v[1] * pfVar22[1] + diff.v[0] * *pfVar22;
  fVar26 = sphereRadius + eps;
  transPos.v[1] = diff.v[2] * pfVar22[6] + diff.v[0] * pfVar22[4] + diff.v[1] * pfVar22[5];
  transPos.v[2] = diff.v[0] * pfVar22[8] + diff.v[1] * pfVar22[9] + diff.v[2] * pfVar22[10];
  *(McdGeometryID *)(&stack0xfffffea4 + iVar15) = trilistgeom[2].next;
  *(float *)((int)&fStackY_160 + iVar15) = fVar26;
  *(lsVec3 **)((int)&iStackY_164 + iVar15) = &transPos;
  *(McdGeometryID *)((int)aMStackY_16c + iVar15 + 4) = pMVar17[3].prev;
  *(McdModelPair **)((int)aMStackY_16c + iVar15) = p;
  pcVar11 = (code *)pMVar17[3].mRefCtAndID;
  *(undefined4 *)((int)aiStackY_180 + iVar15 + 0x10) = 0x1020f;
  count = (*pcVar11)(*(void **)((int)aMStackY_16c + iVar15),
                     *(void **)((int)aMStackY_16c + iVar15 + 4),
                     *(void **)((int)&iStackY_164 + iVar15),*(float *)((int)&fStackY_160 + iVar15),
                     *(int *)(&stack0xfffffea4 + iVar15));
  uVar23 = 0;
  result->contactCount = 0;
  result->touch = 0;
  result->normal[0] = 0.0;
  result->normal[1] = 0.0;
  result->normal[2] = 0.0;
  if (count != 0) {
    ct.vertices[1] = vector[2].v;
    ct.normal = vector[0].v;
    ct.vertices[0] = vector[1].v;
    ct.vertices[2] = vector[3].v;
    pMVar25 = result->contacts;
    i = 0;
    if ((0 < count) && (result->contactCount < 200)) {
      local_144 = 0;
      do {
        paMVar19 = ct.vertices[0];
        puVar24 = (undefined4 *)((int)&(trilistgeom[3].prev)->mRefCtAndID + local_144);
        pfVar22 = (float *)*puVar24;
        (*ct.vertices[0])[0] = fVar8 * pfVar22[2] + fVar2 * *pfVar22 + fVar5 * pfVar22[1] + fVar12;
        (*paMVar19)[1] = fVar9 * pfVar22[2] + fVar6 * pfVar22[1] + fVar3 * *pfVar22 + fVar13;
        (*paMVar19)[2] = fVar10 * pfVar22[2] + fVar7 * pfVar22[1] + fVar4 * *pfVar22 + fVar14;
        paMVar19 = ct.vertices[1];
        pfVar22 = (float *)puVar24[1];
        (*ct.vertices[1])[0] = fVar8 * pfVar22[2] + fVar5 * pfVar22[1] + fVar2 * *pfVar22 + fVar12;
        (*paMVar19)[1] = fVar9 * pfVar22[2] + fVar6 * pfVar22[1] + fVar3 * *pfVar22 + fVar13;
        (*paMVar19)[2] = fVar10 * pfVar22[2] + fVar7 * pfVar22[1] + fVar4 * *pfVar22 + fVar14;
        paMVar19 = ct.vertices[2];
        pfVar22 = (float *)puVar24[2];
        (*ct.vertices[2])[0] = fVar8 * pfVar22[2] + fVar5 * pfVar22[1] + fVar2 * *pfVar22 + fVar12;
        (*paMVar19)[1] = fVar9 * pfVar22[2] + fVar6 * pfVar22[1] + fVar3 * *pfVar22 + fVar13;
        (*paMVar19)[2] = fVar10 * pfVar22[2] + fVar7 * pfVar22[1] + fVar4 * *pfVar22 + fVar14;
        paMVar19 = ct.normal;
        pfVar22 = (float *)puVar24[3];
        (*ct.normal)[0] = fVar8 * pfVar22[2] + fVar5 * pfVar22[1] + fVar2 * *pfVar22;
        (*paMVar19)[1] = fVar9 * pfVar22[2] + fVar6 * pfVar22[1] + fVar3 * *pfVar22;
        (*paMVar19)[2] = fVar10 * pfVar22[2] + fVar7 * pfVar22[1] + fVar4 * *pfVar22;
        ct.flags = puVar24[5];
        ct.triangleData = *(anon_union_4_2_43add64d_for_triangleData *)(puVar24 + 4);
        edge[0][0] = (*ct.vertices[1])[0] - (*ct.vertices[0])[0];
        edge[0][1] = (*ct.vertices[1])[1] - (*ct.vertices[0])[1];
        edge[0][2] = (*ct.vertices[1])[2] - (*ct.vertices[0])[2];
        edge[1][0] = (*ct.vertices[2])[0] - (*ct.vertices[1])[0];
        edge[1][1] = (*ct.vertices[2])[1] - (*ct.vertices[1])[1];
        edge[1][2] = (*ct.vertices[2])[2] - (*ct.vertices[1])[2];
        edge[2][0] = (*ct.vertices[0])[0] - (*ct.vertices[2])[0];
        edge[2][1] = (*ct.vertices[0])[1] - (*ct.vertices[2])[1];
        edge[2][2] = (*ct.vertices[0])[2] - (*ct.vertices[2])[2];
        *(MeReal *)(&stack0xfffffea4 + iVar15) = fwk->mScale;
        *(MeReal (**) [3])((int)&fStackY_160 + iVar15) = edge;
        MVar18 = sphereRadius;
        *(McdUserTriangle **)((int)&iStackY_164 + iVar15) = &ct;
        MVar16 = eps;
        *(MeReal *)((int)aMStackY_16c + iVar15 + 4) = MVar18;
        *(MeReal *)((int)aMStackY_16c + iVar15) = MVar16;
        *(MeI16 **)((int)aiStackY_180 + iVar15 + 0x10) = &dims;
        *(lsVec3 **)((int)aiStackY_180 + iVar15 + 0xc) = &pos;
        *(lsVec3 **)((int)aiStackY_180 + iVar15 + 8) = &normal;
        *(MeReal **)((int)aiStackY_180 + iVar15 + 4) = &separation;
        *(undefined4 *)((int)aiStackY_180 + iVar15) = 0x10583;
        bVar20 = McdVanillaOverlapSphereTri
                           (*(MeReal **)((int)aiStackY_180 + iVar15 + 4),
                            *(lsVec3 **)((int)aiStackY_180 + iVar15 + 8),
                            *(lsVec3 **)((int)aiStackY_180 + iVar15 + 0xc),
                            *(MeI16 **)((int)aiStackY_180 + iVar15 + 0x10),
                            *(MeReal *)((int)aMStackY_16c + iVar15),
                            *(MeReal *)((int)aMStackY_16c + iVar15 + 4),
                            *(McdUserTriangle **)((int)&iStackY_164 + iVar15),
                            *(MeReal (**) [3])((int)&fStackY_160 + iVar15),
                            *(MeReal *)(&stack0xfffffea4 + iVar15));
        if (bVar20) {
          fVar26 = local_13c[1];
          fVar1 = local_13c[2];
          diff.v[0] = *local_13c + pos.v[0];
          diff.v[2] = fVar1 + pos.v[2];
          diff.v[1] = fVar26 + pos.v[1];
          pMVar25->position[0] = *local_13c + pos.v[0];
          pMVar25->position[1] = fVar26 + pos.v[1];
          pMVar25->position[2] = fVar1 + pos.v[2];
          pMVar25->dims = dims;
          pMVar25->normal[0] = normal.v[0];
          pMVar25->normal[1] = normal.v[1];
          pMVar25->normal[2] = normal.v[2];
          pMVar25->separation = separation;
          result->normal[0] = normal.v[0] + result->normal[0];
          result->normal[1] = normal.v[1] + result->normal[1];
          result->normal[2] = normal.v[2] + result->normal[2];
          pMVar25->element2 = *(anon_union_4_2_43add64d_for_element2 *)(puVar24 + 4);
          pMVar25 = pMVar25 + 1;
          result->contactCount = result->contactCount + 1;
        }
        i = i + 1;
        local_144 = local_144 + 0x18;
      } while ((i < count) && (result->contactCount < 200));
    }
    *(MeReal **)(&stack0xfffffea4 + iVar15) = result->normal;
    *(undefined4 *)((int)&fStackY_160 + iVar15) = 0x10674;
    MeVector3Normalize(*(MeReal **)(&stack0xfffffea4 + iVar15));
    uVar23 = (uint)(0 < result->contactCount);
    result->touch = uVar23;
  }
  return uVar23;
}


/* ==== McdSphereTriangleListRegisterInteraction ==== */

MeBool McdSphereTriangleListRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdSphereTriangleListIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,1,6,&interactions);
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


/* ==== McdVanillaOverlapSphereTri ==== */

bool McdVanillaOverlapSphereTri
               (MeReal *outSep,lsVec3 *outN,lsVec3 *outPos,MeI16 *outDims,MeReal inEps,MeReal inR,
               McdUserTriangle *inTri,MeReal (*edge) [3],MeReal scale)

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
  MeVector3 *paMVar13;
  MeVector3 *paMVar14;
  MeVector3 *paMVar15;
  MeVector3 *paMVar16;
  float fVar17;
  float fVar18;
  short sVar19;
  MeReal MVar20;
  bool bVar21;
  int iVar22;
  MeReal d1;
  MeReal d0;
  MeReal maxSeparation;
  MeReal normD;
  MeReal eps;
  lsVec3 vertex;
  lsVec3 point;
  lsVec3 origin;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: lsVec3 * v0@[DW_OP_reg0(EAX)]
                       Unresolved local var: lsVec3 * v1@[DW_OP_reg1(ECX)]
                       Unresolved local var: lsVec3 * v2@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal d2@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal distanceSq@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal distance@[???] */
  fVar12 = scale * 1e-06;
  paMVar13 = inTri->vertices[0];
  paMVar14 = inTri->vertices[1];
  paMVar15 = inTri->vertices[2];
  fVar1 = (*paMVar13)[0];
  paMVar16 = inTri->normal;
  fVar2 = (*paMVar13)[1];
  fVar3 = (*paMVar13)[2];
  bVar21 = false;
  fVar17 = fVar3 * (*paMVar16)[2] + fVar1 * (*paMVar16)[0] + fVar2 * (*paMVar16)[1];
  if (ABS(fVar17) - inR <= inEps) {
    fVar4 = (*paMVar14)[2];
    fVar5 = (*paMVar14)[1];
    fVar6 = (*paMVar14)[0];
    fVar7 = (*paMVar16)[1];
    fVar8 = (*paMVar16)[2];
    fVar18 = fVar8 * (fVar1 * fVar5 - fVar2 * fVar6) +
             (fVar3 * fVar6 - fVar1 * fVar4) * fVar7 +
             (*paMVar16)[0] * (fVar2 * fVar4 - fVar3 * fVar5);
    fVar9 = (*paMVar15)[2];
    fVar10 = (*paMVar15)[1];
    fVar11 = (*paMVar15)[0];
    fVar4 = (fVar6 * fVar10 - fVar5 * fVar11) * fVar8 +
            (fVar4 * fVar11 - fVar6 * fVar9) * fVar7 +
            (fVar5 * fVar9 - fVar4 * fVar10) * (*paMVar16)[0];
    fVar5 = -fVar12;
    fVar1 = (fVar11 * fVar2 - fVar10 * fVar1) * fVar8 +
            (fVar3 * fVar10 - fVar2 * fVar9) * (*paMVar16)[0] +
            (fVar9 * fVar1 - fVar3 * fVar11) * fVar7;
    if ((((fVar5 < fVar18) && (fVar5 < fVar4)) && (fVar5 < fVar1)) ||
       (((fVar18 < fVar12 && (fVar4 < fVar12)) && (fVar1 < fVar12)))) {
      fVar1 = (*paMVar16)[1];
      fVar2 = (*paMVar16)[2];
      outPos->v[0] = fVar17 * (*paMVar16)[0];
      outPos->v[1] = fVar17 * fVar1;
      outPos->v[2] = fVar17 * fVar2;
      *outDims = 0x203;
      if ((inTri->flags & kMcdTriangleTwoSided) == 0) {
        paMVar13 = inTri->normal;
        outN->v[0] = (*paMVar13)[0];
        outN->v[1] = (*paMVar13)[1];
        outN->v[2] = (*paMVar13)[2];
        normD = -fVar17;
        *outSep = normD - inR;
      }
      else {
        if (fVar17 <= 0.0) {
          paMVar13 = inTri->normal;
          origin.v[0] = (*paMVar13)[0];
          origin.v[1] = (*paMVar13)[1];
          MVar20 = (*paMVar13)[2];
        }
        else {
          paMVar13 = inTri->normal;
          origin.v[0] = -(*paMVar13)[0];
          origin.v[1] = -(*paMVar13)[1];
          MVar20 = -(*paMVar13)[2];
        }
        outN->v[0] = origin.v[0];
        outN->v[1] = origin.v[1];
        outN->v[2] = MVar20;
        *outSep = ABS(fVar17) - inR;
      }
    }
    else {
      fVar1 = 3.4028235e+38;
      iVar22 = 0;
      do {
                    /* Unresolved local var: MeI16 d@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal n@[???]
                       Unresolved local var: MeReal num@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal den@[???] */
        paMVar13 = inTri->vertices[iVar22];
        point.v[0] = (*paMVar13)[0];
        point.v[1] = (*paMVar13)[1];
        point.v[2] = (*paMVar13)[2];
        fVar2 = (*edge)[0];
        fVar3 = (*edge)[1];
        fVar12 = (*edge)[2];
        fVar17 = -point.v[2] * fVar12 + -point.v[0] * fVar2 + -point.v[1] * fVar3;
        if (0.0 < fVar17) {
          fVar4 = fVar12 * fVar12 + fVar3 * fVar3 + fVar2 * fVar2;
          if (fVar4 <= fVar17) {
            point.v[0] = point.v[0] + fVar2;
            point.v[1] = point.v[1] + fVar3;
            point.v[2] = point.v[2] + fVar12;
            goto LAB_00010b45;
          }
          sVar19 = 1;
          fVar17 = fVar17 * (1.0 / fVar4);
          point.v[0] = fVar17 * fVar2 + point.v[0];
          point.v[1] = fVar17 * fVar3 + point.v[1];
          point.v[2] = fVar17 * fVar12 + point.v[2];
        }
        else {
LAB_00010b45:
          sVar19 = 0;
        }
        fVar2 = point.v[2] * point.v[2] + point.v[1] * point.v[1] + point.v[0] * point.v[0];
        if (fVar2 < fVar1) {
          outPos->v[0] = point.v[0];
          outPos->v[1] = point.v[1];
          outPos->v[2] = point.v[2];
          *outDims = sVar19 << 8 | 3;
          fVar1 = fVar2;
        }
        iVar22 = iVar22 + 1;
        edge = edge + 1;
      } while (iVar22 < 3);
      if (inR * inR < fVar1) {
        return false;
      }
                    /* Unresolved local var: float __result@[DW_OP_reg15(ST4)] */
                    /* Unresolved local var: MeReal recipT@[DW_OP_reg16(ST5)] */
      fVar12 = 1.0 / SQRT(fVar1);
      fVar2 = outPos->v[1];
      fVar3 = outPos->v[2];
      outN->v[0] = -outPos->v[0] * fVar12;
      outN->v[1] = -fVar2 * fVar12;
      outN->v[2] = -fVar3 * fVar12;
      *outSep = SQRT(fVar1) - inR;
    }
    bVar21 = true;
  }
  return bVar21;
}


