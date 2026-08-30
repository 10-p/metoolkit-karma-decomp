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
  float fVar11;
  code *pcVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  int iVar16;
  MeReal MVar17;
  McdTriangleListID pMVar18;
  MeReal MVar19;
  MeVector3 *paMVar20;
  bool bVar21;
  McdModelID_conflict pMVar22;
  int iVar23;
  float *pfVar24;
  uint uVar25;
  undefined4 *puVar26;
  McdContact *pMVar27;
  longdouble lVar28;
  int aiStack_180 [5];
  MeReal aMStack_16c [2];
  int iStack_164;
  float fStack_160;
  McdModelID_conflict local_15c [4];
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
  local_15c[0] = p->model1;
  fStack_160 = 9.18677e-41;
  pMVar22 = (McdModelID_conflict)McdModelGetGeometry();
  local_15c[0] = p->model2;
  fStack_160 = 9.18845e-41;
  trilistgeom = (McdTriangleListID)McdModelGetGeometry();
  fwk = p->model1->frame;
  fStack_160 = 9.19182e-41;
  local_15c[0] = pMVar22;
  lVar28 = (longdouble)McdSphereGetRadius();
  sphereRadius = (MeReal)lVar28;
  local_15c[0] = p->model1;
  fStack_160 = 9.19392e-41;
  iVar23 = McdModelGetTransformPtr();
  local_13c = (float *)(iVar23 + 0x30);
  local_15c[0] = p->model2;
  fStack_160 = 9.19686e-41;
  pfVar24 = (float *)McdModelGetTransformPtr();
  fVar3 = *pfVar24;
  fVar4 = pfVar24[1];
  fVar5 = pfVar24[2];
  fVar6 = pfVar24[4];
  fVar7 = pfVar24[5];
  fVar8 = pfVar24[6];
  fVar9 = pfVar24[8];
  fVar10 = pfVar24[9];
  fVar11 = pfVar24[10];
  fVar13 = pfVar24[0xc] - *(float *)(iVar23 + 0x30);
  fVar14 = pfVar24[0xd] - local_13c[1];
  fVar15 = pfVar24[0xe] - local_13c[2];
  local_15c[0] = p->model1;
  fStack_160 = 9.22461e-41;
  lVar28 = (longdouble)McdModelGetContactTolerance();
  eps = (MeReal)lVar28;
  local_15c[0] = p->model2;
  fStack_160 = 9.22685e-41;
  lVar28 = (longdouble)McdModelGetContactTolerance();
  pMVar18 = trilistgeom;
  eps = (MeReal)(lVar28 + (longdouble)eps);
  iVar16 = -((int)trilistgeom[2].next * 0x18 + 0xfU & 0xfffffff0);
  trilistgeom[3].prev = (McdGeometryID)(&stack0xfffffeb4 + iVar16);
  diff.v[0] = *(float *)(iVar23 + 0x30) - pfVar24[0xc];
  diff.v[1] = local_13c[1] - pfVar24[0xd];
  diff.v[2] = local_13c[2] - pfVar24[0xe];
  transPos.v[0] = diff.v[2] * pfVar24[2] + diff.v[1] * pfVar24[1] + diff.v[0] * *pfVar24;
  transPos.v[1] = diff.v[2] * pfVar24[6] + diff.v[0] * pfVar24[4] + diff.v[1] * pfVar24[5];
  fVar1 = sphereRadius + eps;
  transPos.v[2] = diff.v[0] * pfVar24[8] + diff.v[1] * pfVar24[9] + diff.v[2] * pfVar24[10];
  *(McdGeometryID *)((int)local_15c + iVar16) = trilistgeom[2].next;
  *(float *)((int)local_15c + iVar16 + -4) = fVar1;
  *(lsVec3 **)((int)&iStack_164 + iVar16) = &transPos;
  *(McdGeometryID *)((int)aMStack_16c + iVar16 + 4) = pMVar18[3].prev;
  *(McdModelPair **)((int)aMStack_16c + iVar16) = p;
  pcVar12 = (code *)pMVar18[3].mRefCtAndID;
  *(undefined4 *)((int)aiStack_180 + iVar16 + 0x10) = 0x1020f;
  count = (*pcVar12)();
  uVar25 = 0;
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
    pMVar27 = result->contacts;
    i = 0;
    if ((0 < count) && (result->contactCount < 200)) {
      local_144 = 0;
      do {
        paMVar20 = ct.vertices[0];
        puVar26 = (undefined4 *)((int)&(trilistgeom[3].prev)->mRefCtAndID + local_144);
        pfVar24 = (float *)*puVar26;
        (*ct.vertices[0])[0] = fVar9 * pfVar24[2] + fVar3 * *pfVar24 + fVar6 * pfVar24[1] + fVar13;
        (*paMVar20)[1] = fVar10 * pfVar24[2] + fVar7 * pfVar24[1] + fVar4 * *pfVar24 + fVar14;
        (*paMVar20)[2] = fVar11 * pfVar24[2] + fVar8 * pfVar24[1] + fVar5 * *pfVar24 + fVar15;
        paMVar20 = ct.vertices[1];
        pfVar24 = (float *)puVar26[1];
        (*ct.vertices[1])[0] = fVar9 * pfVar24[2] + fVar6 * pfVar24[1] + fVar3 * *pfVar24 + fVar13;
        (*paMVar20)[1] = fVar10 * pfVar24[2] + fVar7 * pfVar24[1] + fVar4 * *pfVar24 + fVar14;
        (*paMVar20)[2] = fVar11 * pfVar24[2] + fVar8 * pfVar24[1] + fVar5 * *pfVar24 + fVar15;
        paMVar20 = ct.vertices[2];
        pfVar24 = (float *)puVar26[2];
        (*ct.vertices[2])[0] = fVar9 * pfVar24[2] + fVar6 * pfVar24[1] + fVar3 * *pfVar24 + fVar13;
        (*paMVar20)[1] = fVar10 * pfVar24[2] + fVar7 * pfVar24[1] + fVar4 * *pfVar24 + fVar14;
        (*paMVar20)[2] = fVar11 * pfVar24[2] + fVar8 * pfVar24[1] + fVar5 * *pfVar24 + fVar15;
        paMVar20 = ct.normal;
        pfVar24 = (float *)puVar26[3];
        (*ct.normal)[0] = fVar9 * pfVar24[2] + fVar6 * pfVar24[1] + fVar3 * *pfVar24;
        (*paMVar20)[1] = fVar10 * pfVar24[2] + fVar7 * pfVar24[1] + fVar4 * *pfVar24;
        (*paMVar20)[2] = fVar11 * pfVar24[2] + fVar8 * pfVar24[1] + fVar5 * *pfVar24;
        ct.flags = puVar26[5];
        ct.triangleData = *(anon_union_4_2_43add64d_for_triangleData *)(puVar26 + 4);
        edge[0][0] = (*ct.vertices[1])[0] - (*ct.vertices[0])[0];
        edge[0][1] = (*ct.vertices[1])[1] - (*ct.vertices[0])[1];
        edge[0][2] = (*ct.vertices[1])[2] - (*ct.vertices[0])[2];
        edge[1][0] = (*ct.vertices[2])[0] - (*ct.vertices[1])[0];
        edge[1][1] = (*ct.vertices[2])[1] - (*ct.vertices[1])[1];
        edge[1][2] = (*ct.vertices[2])[2] - (*ct.vertices[1])[2];
        edge[2][0] = (*ct.vertices[0])[0] - (*ct.vertices[2])[0];
        edge[2][1] = (*ct.vertices[0])[1] - (*ct.vertices[2])[1];
        edge[2][2] = (*ct.vertices[0])[2] - (*ct.vertices[2])[2];
        *(MeReal *)((int)local_15c + iVar16) = fwk->mScale;
        *(MeReal (**) [3])((int)local_15c + iVar16 + -4) = edge;
        MVar19 = sphereRadius;
        *(McdUserTriangle **)((int)&iStack_164 + iVar16) = &ct;
        MVar17 = eps;
        *(MeReal *)((int)aMStack_16c + iVar16 + 4) = MVar19;
        *(MeReal *)((int)aMStack_16c + iVar16) = MVar17;
        *(MeI16 **)((int)aiStack_180 + iVar16 + 0x10) = &dims;
        *(lsVec3 **)((int)aiStack_180 + iVar16 + 0xc) = &pos;
        *(lsVec3 **)((int)aiStack_180 + iVar16 + 8) = &normal;
        *(MeReal **)((int)aiStack_180 + iVar16 + 4) = &separation;
        *(undefined4 *)((int)aiStack_180 + iVar16) = 0x10583;
        bVar21 = McdVanillaOverlapSphereTri
                           (*(MeReal **)((int)aiStack_180 + iVar16 + 4),
                            *(lsVec3 **)((int)aiStack_180 + iVar16 + 8),
                            *(lsVec3 **)((int)aiStack_180 + iVar16 + 0xc),
                            *(MeI16 **)((int)aiStack_180 + iVar16 + 0x10),
                            *(MeReal *)((int)aMStack_16c + iVar16),
                            *(MeReal *)((int)aMStack_16c + iVar16 + 4),
                            *(McdUserTriangle **)((int)&iStack_164 + iVar16),
                            *(MeReal (**) [3])((int)local_15c + iVar16 + -4),
                            *(MeReal *)((int)local_15c + iVar16));
        if (bVar21) {
          fVar1 = local_13c[1];
          fVar2 = local_13c[2];
          diff.v[0] = *local_13c + pos.v[0];
          diff.v[2] = fVar2 + pos.v[2];
          diff.v[1] = fVar1 + pos.v[1];
          pMVar27->position[0] = *local_13c + pos.v[0];
          pMVar27->position[1] = fVar1 + pos.v[1];
          pMVar27->position[2] = fVar2 + pos.v[2];
          pMVar27->dims = dims;
          pMVar27->normal[0] = normal.v[0];
          pMVar27->normal[1] = normal.v[1];
          pMVar27->normal[2] = normal.v[2];
          pMVar27->separation = separation;
          result->normal[0] = normal.v[0] + result->normal[0];
          result->normal[1] = normal.v[1] + result->normal[1];
          result->normal[2] = normal.v[2] + result->normal[2];
          pMVar27->element2 = *(anon_union_4_2_43add64d_for_element2 *)(puVar26 + 4);
          pMVar27 = pMVar27 + 1;
          result->contactCount = result->contactCount + 1;
        }
        i = i + 1;
        local_144 = local_144 + 0x18;
      } while ((i < count) && (result->contactCount < 200));
    }
    *(MeReal **)((int)local_15c + iVar16) = result->normal;
    *(undefined4 *)((int)local_15c + iVar16 + -4) = 0x10674;
    MeVector3Normalize(*(MeReal **)((int)local_15c + iVar16));
    uVar25 = (uint)(0 < result->contactCount);
    result->touch = uVar25;
  }
  return uVar25;
}


/* ==== McdSphereTriangleListRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

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


