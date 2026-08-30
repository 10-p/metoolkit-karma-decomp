/* ==== McdBoxTriangleListIntersect ==== */

MeBool McdBoxTriangleListIntersect(McdModelPair *p,McdIntersectResult *result)

{
  float fVar1;
  float fVar2;
  McdGeometryID pMVar3;
  code *pcVar4;
  int iVar5;
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
  McdContact *pMVar17;
  MeReal *pMVar18;
  MeReal *pMVar19;
  MeReal *pMVar20;
  MeReal MVar21;
  McdTriangleListID pMVar22;
  MeVector3 *paMVar23;
  bool bVar24;
  float *pfVar25;
  float *pfVar26;
  MeBool MVar27;
  int iVar28;
  lsVec3 *plVar29;
  McdContact *pMVar30;
  undefined4 *puVar31;
  float fVar32;
  int aiStackY_280 [6];
  MeReal MStackY_268;
  int iStackY_264;
  float fStackY_260;
  undefined4 uStack_250;
  McdGeometry MStack_24c;
  McdFramework *fwk;
  MeReal eps;
  McdTriangleListID trilistgeom;
  McdBoxID boxgeom;
  int j;
  int count;
  lsVec3 *boxRadii;
  MeReal separation;
  MeReal PN;
  lsVec3 *verts;
  MeI16 dims;
  MeReal boxRadius;
  lsVec3 diff;
  lsVec3 v;
  lsVec3 normal;
  lsVec3 footprint [18];
  MeReal edge [3] [3];
  McdUserTriangle ct;
  lsVec3 vector [4];
  lsVec3 boxPosTrans;
  lsTransform triToBox;
  MeReal boxCentre [3];
  
                    /* Unresolved local var: McdUserTriangle * triNE@[DW_OP_reg6(ESI)]
                       Unresolved local var: lsTransform * boxTM@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * triListTM@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdTriangleList * triList@[???]
                       Unresolved local var: McdContact * c@[DW_OP_reg3(EBX)] */
  j = 0;
  fStackY_260 = 9.18817e-41;
  boxgeom = McdModelGetGeometry(p->model1);
  fStackY_260 = 9.19042e-41;
  trilistgeom = McdModelGetGeometry(p->model2);
  fStackY_260 = 9.19252e-41;
  eps = McdModelGetContactTolerance(p->model1);
  fStackY_260 = 9.19476e-41;
  fVar32 = McdModelGetContactTolerance(p->model2);
  eps = fVar32 + eps;
  fStackY_260 = 9.1977e-41;
  pfVar25 = McdModelGetTransformPtr(p->model1);
  fStackY_260 = 9.19938e-41;
  pfVar26 = McdModelGetTransformPtr(p->model2);
  fwk = p->model1->frame;
  result->contactCount = 0;
  result->touch = 0;
  fStackY_260 = 9.20527e-41;
  boxRadii = McdBoxGetRadii(boxgeom);
  MStack_24c.frame = (McdFrameworkID)(pfVar26 + 0xc);
  diff.v[1] = pfVar25[0xd] - pfVar26[0xd];
  diff.v[2] = pfVar25[0xe] - pfVar26[0xe];
  diff.v[0] = pfVar25[0xc] - pfVar26[0xc];
  boxPosTrans.v[0] = diff.v[2] * pfVar26[2] + diff.v[0] * *pfVar26 + diff.v[1] * pfVar26[1];
  boxPosTrans.v[1] = diff.v[2] * pfVar26[6] + diff.v[0] * pfVar26[4] + diff.v[1] * pfVar26[5];
  boxPosTrans.v[2] = diff.v[2] * pfVar26[10] + diff.v[0] * pfVar26[8] + diff.v[1] * pfVar26[9];
  fStackY_260 = 9.22867e-41;
  McdBoxGetBSphere(boxgeom,boxCentre,&boxRadius);
  pMVar22 = trilistgeom;
  MVar21 = eps;
  pMVar3 = trilistgeom[2].next;
  iVar5 = -((int)pMVar3 * 0x18 + 0xfU & 0xfffffff0);
  trilistgeom[3].prev = (McdGeometryID)((int)&MStack_24c + iVar5);
  *(McdGeometryID *)(&stack0xfffffda4 + iVar5) = pMVar3;
  *(float *)((int)&fStackY_260 + iVar5) = MVar21 + boxRadius;
  *(lsVec3 **)((int)&iStackY_264 + iVar5) = &boxPosTrans;
  *(McdGeometryID *)((int)&MStackY_268 + iVar5) = pMVar22[3].prev;
  *(McdModelPair **)((int)aiStackY_280 + iVar5 + 0x14) = p;
  pcVar4 = (code *)trilistgeom[3].mRefCtAndID;
  *(undefined4 *)((int)aiStackY_280 + iVar5 + 0x10) = 0x1018e;
  count = (*pcVar4)();
  MStack_24c.next = (McdGeometryID)result->normal;
  result->normal[0] = 0.0;
  result->normal[1] = 0.0;
  result->normal[2] = 0.0;
  MVar27 = 0;
  if (count != 0) {
    ct.normal = vector[0].v;
    ct.vertices[0] = vector[1].v;
    ct.vertices[1] = vector[2].v;
    ct.vertices[2] = vector[3].v;
    pMVar30 = result->contacts;
    fVar32 = pfVar25[2] * pfVar26[2] + pfVar25[1] * pfVar26[1] + *pfVar25 * *pfVar26;
    fVar6 = pfVar25[6] * pfVar26[2] + pfVar25[5] * pfVar26[1] + pfVar25[4] * *pfVar26;
    fVar7 = pfVar25[10] * pfVar26[2] + pfVar25[9] * pfVar26[1] + pfVar25[8] * *pfVar26;
    fVar8 = pfVar25[2] * pfVar26[6] + pfVar25[1] * pfVar26[5] + *pfVar25 * pfVar26[4];
    fVar9 = pfVar25[6] * pfVar26[6] + pfVar25[5] * pfVar26[5] + pfVar25[4] * pfVar26[4];
    fVar10 = pfVar25[10] * pfVar26[6] + pfVar25[9] * pfVar26[5] + pfVar25[8] * pfVar26[4];
    fVar11 = pfVar25[2] * pfVar26[10] + pfVar25[1] * pfVar26[9] + *pfVar25 * pfVar26[8];
    fVar12 = pfVar25[6] * pfVar26[10] + pfVar25[5] * pfVar26[9] + pfVar25[4] * pfVar26[8];
    fVar13 = pfVar25[10] * pfVar26[10] + pfVar25[9] * pfVar26[9] + pfVar25[8] * pfVar26[8];
    diff.v[0] = pfVar26[0xc] - pfVar25[0xc];
    diff.v[1] = (float)(MStack_24c.frame)->geometryRegisteredCount - pfVar25[0xd];
    diff.v[2] = (float)(MStack_24c.frame)->firstModel - pfVar25[0xe];
    fVar14 = diff.v[2] * pfVar25[2] + diff.v[1] * pfVar25[1] + diff.v[0] * *pfVar25;
    fVar16 = diff.v[2] * pfVar25[6] + diff.v[0] * pfVar25[4] + diff.v[1] * pfVar25[5];
    fVar15 = diff.v[0] * pfVar25[8] + diff.v[1] * pfVar25[9] + diff.v[2] * pfVar25[10];
    if (j < count) {
      MStack_24c.prev = (McdGeometryID)0x0;
      do {
        paMVar23 = ct.vertices[2];
        puVar31 = (undefined4 *)((int)&(trilistgeom[3].prev)->mRefCtAndID + (int)MStack_24c.prev);
        pfVar26 = (float *)*puVar31;
        (*ct.vertices[0])[0] = fVar11 * pfVar26[2] + fVar32 * *pfVar26 + fVar8 * pfVar26[1] + fVar14
        ;
        (*ct.vertices[0])[1] = fVar12 * pfVar26[2] + fVar9 * pfVar26[1] + fVar6 * *pfVar26 + fVar16;
        (*ct.vertices[0])[2] = fVar13 * pfVar26[2] + fVar10 * pfVar26[1] + fVar7 * *pfVar26 + fVar15
        ;
        pfVar26 = (float *)puVar31[1];
        (*ct.vertices[1])[0] = fVar11 * pfVar26[2] + fVar8 * pfVar26[1] + fVar32 * *pfVar26 + fVar14
        ;
        (*ct.vertices[1])[1] = fVar12 * pfVar26[2] + fVar9 * pfVar26[1] + fVar6 * *pfVar26 + fVar16;
        (*ct.vertices[1])[2] = fVar13 * pfVar26[2] + fVar10 * pfVar26[1] + fVar7 * *pfVar26 + fVar15
        ;
        pfVar26 = (float *)puVar31[2];
        (*ct.vertices[2])[0] = fVar11 * pfVar26[2] + fVar8 * pfVar26[1] + fVar32 * *pfVar26 + fVar14
        ;
        (*ct.vertices[2])[1] = fVar12 * pfVar26[2] + fVar9 * pfVar26[1] + fVar6 * *pfVar26 + fVar16;
        (*ct.vertices[2])[2] = fVar13 * pfVar26[2] + fVar10 * pfVar26[1] + fVar7 * *pfVar26 + fVar15
        ;
        pfVar26 = (float *)puVar31[3];
        (*ct.normal)[0] = fVar11 * pfVar26[2] + fVar8 * pfVar26[1] + fVar32 * *pfVar26;
        (*ct.normal)[1] = fVar12 * pfVar26[2] + fVar9 * pfVar26[1] + fVar6 * *pfVar26;
        (*ct.normal)[2] = fVar13 * pfVar26[2] + fVar10 * pfVar26[1] + fVar7 * *pfVar26;
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
        fVar1 = (*ct.vertices[2])[2];
        fVar2 = (*ct.vertices[0])[2];
        *(MeVector3 **)((int)&uStack_250 + iVar5) = ct.vertices[2];
        verts = footprint;
        *(MeVector3 **)(&stack0xfffffdac + iVar5) = paMVar23;
        edge[2][2] = fVar2 - fVar1;
        *(MeReal *)(&stack0xfffffda8 + iVar5) = fwk->mScale;
        *(MeReal (**) [3])(&stack0xfffffda4 + iVar5) = edge;
        plVar29 = boxRadii;
        *(McdUserTriangle **)((int)&fStackY_260 + iVar5) = &ct;
        MVar21 = eps;
        *(lsVec3 **)((int)&iStackY_264 + iVar5) = plVar29;
        *(MeReal *)((int)&MStackY_268 + iVar5) = MVar21;
        *(MeI16 **)((int)aiStackY_280 + iVar5 + 0x14) = &dims;
        *(lsVec3 ***)((int)aiStackY_280 + iVar5 + 0x10) = &verts;
        *(MeReal **)((int)aiStackY_280 + iVar5 + 0xc) = &PN;
        *(lsVec3 **)((int)aiStackY_280 + iVar5 + 8) = &normal;
        *(MeReal **)((int)aiStackY_280 + iVar5 + 4) = &separation;
        *(undefined4 *)((int)aiStackY_280 + iVar5) = 0x10622;
        bVar24 = McdVanillaOverlapOBBTri
                           (*(MeReal **)((int)aiStackY_280 + iVar5 + 4),
                            *(lsVec3 **)((int)aiStackY_280 + iVar5 + 8),
                            *(MeReal **)((int)aiStackY_280 + iVar5 + 0xc),
                            *(lsVec3 ***)((int)aiStackY_280 + iVar5 + 0x10),
                            *(MeI16 **)((int)aiStackY_280 + iVar5 + 0x14),
                            *(MeReal *)((int)&MStackY_268 + iVar5),
                            *(lsVec3 **)((int)&iStackY_264 + iVar5),
                            *(McdUserTriangle **)((int)&fStackY_260 + iVar5),
                            *(MeReal (**) [3])(&stack0xfffffda4 + iVar5),
                            *(MeReal *)(&stack0xfffffda8 + iVar5));
        if (bVar24) {
                    /* Unresolved local var: lsVec3 * v@[DW_OP_reg1(ECX)] */
          plVar29 = footprint;
          diff.v[0] = normal.v[2] * pfVar25[8] + normal.v[1] * pfVar25[4] + normal.v[0] * *pfVar25;
          diff.v[1] = normal.v[2] * pfVar25[9] + normal.v[0] * pfVar25[1] + normal.v[1] * pfVar25[5]
          ;
          diff.v[2] = normal.v[2] * pfVar25[10] +
                      normal.v[0] * pfVar25[2] + normal.v[1] * pfVar25[6];
          if (plVar29 != verts) {
            iVar28 = result->contactCount;
            pMVar17 = pMVar30;
            do {
              pMVar30 = pMVar17;
              if (399 < iVar28) break;
                    /* Unresolved local var: lsVec3 * cpos@[???] */
              pMVar17->position[0] =
                   pfVar25[8] * plVar29->v[2] +
                   pfVar25[4] * plVar29->v[1] + *pfVar25 * plVar29->v[0] + pfVar25[0xc];
              pMVar17->position[1] =
                   pfVar25[9] * plVar29->v[2] +
                   pfVar25[5] * plVar29->v[1] + pfVar25[1] * plVar29->v[0] + pfVar25[0xd];
              pMVar18 = plVar29->v;
              pMVar19 = plVar29->v;
              pMVar20 = plVar29->v;
              plVar29 = plVar29 + 1;
              pMVar17->position[2] =
                   pfVar25[10] * pMVar20[2] + pfVar25[6] * pMVar19[1] + pfVar25[2] * *pMVar18 +
                   pfVar25[0xe];
              pMVar17->dims = dims;
              pMVar17->separation = separation;
              pMVar17->element2 = *(anon_union_4_2_43add64d_for_element2 *)(puVar31 + 4);
              pMVar17->normal[0] = diff.v[0];
              pMVar30 = pMVar17 + 1;
              pMVar17->normal[1] = diff.v[1];
              pMVar17->normal[2] = diff.v[2];
              iVar28 = result->contactCount + 1;
              result->contactCount = iVar28;
              pMVar17 = pMVar30;
            } while (plVar29 != verts);
          }
          result->normal[0] = diff.v[0] + result->normal[0];
          (MStack_24c.next)->prev = (McdGeometryID)(diff.v[1] + (float)(MStack_24c.next)->prev);
          (MStack_24c.next)->next = (McdGeometryID)(diff.v[2] + (float)(MStack_24c.next)->next);
        }
        j = j + 1;
        MStack_24c.prev = (McdGeometryID)&MStack_24c.prev[1].next;
      } while (j < count);
    }
    if (result->contactCount < 1) {
      result->touch = 0;
    }
    else {
      *(McdGeometryID *)(&stack0xfffffda4 + iVar5) = MStack_24c.next;
      *(undefined4 *)((int)&fStackY_260 + iVar5) = 0x107e1;
      MeVector3Normalize(*(MeReal **)(&stack0xfffffda4 + iVar5));
      result->touch = 1;
    }
    MVar27 = result->touch;
  }
  return MVar27;
}


/* ==== McdBoxTriangleListRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

MeBool McdBoxTriangleListRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdBoxTriangleListIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,2,6,&interactions);
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


/* ==== McdVanillaOverlapOBBTri ==== */

bool McdVanillaOverlapOBBTri
               (MeReal *outSep,lsVec3 *outN,MeReal *outPN,lsVec3 **outPos,MeI16 *outDims,
               MeReal inEps,lsVec3 *inR,McdUserTriangle *inTri,MeReal (*edge) [3],MeReal scale)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  MeVector3 *paMVar6;
  MeVector3 *paMVar7;
  lsVec3 *plVar8;
  float fVar9;
  float *pfVar10;
  uint uVar11;
  uint uVar12;
  MeReal MVar13;
  ushort uVar14;
  int iVar15;
  byte bVar16;
  uint uVar17;
  float *pfVar18;
  sbyte local_f8;
  lsVec3 *local_ec;
  int local_e8;
  int local_e4;
  MeReal PN;
  MeReal nRLen;
  MeU8 normInfo;
  MeReal normalSign;
  MeReal maxSeparation;
  MeReal eps2;
  MeReal eps;
  int i;
  lsVec3 outVec;
  lsVec3 aNorm;
  lsVec3 minCoord;
  MeReal sqE [3];
  lsVec3 aE [3];
  
                    /* Unresolved local var: int j@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeReal sumR@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal normD@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal aNormD@[???]
                       Unresolved local var: lsVec3 * posList@[DW_OP_reg3(EBX)]
                       Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
  eps = inEps;
  if (inEps < 0.0) {
    eps = 0.0;
  }
  nRLen = 1.0;
  paMVar6 = inTri->normal;
  fVar1 = (*paMVar6)[1];
  fVar2 = (*paMVar6)[2];
  fVar3 = ABS((*paMVar6)[0]);
  paMVar7 = inTri->vertices[0];
  fVar4 = inR->v[0] * fVar3 + ABS(fVar1) * inR->v[1] + ABS(fVar2) * inR->v[2];
  normalSign = 1.0;
  fVar9 = (*paMVar7)[2] * (*paMVar6)[2] +
          (*paMVar7)[1] * (*paMVar6)[1] + (*paMVar7)[0] * (*paMVar6)[0];
  fVar5 = ABS(fVar9);
  PN = -fVar5;
  maxSeparation = fVar5 - fVar4;
  if (0.0 < fVar9) {
    normalSign = -1.0;
  }
  normInfo = '\x03';
  if (inEps < maxSeparation) {
    return false;
  }
  if (((inTri->flags & kMcdTriangleTwoSided) == 0) && (normalSign < 0.0)) {
    maxSeparation = -maxSeparation;
    normalSign = 1.0;
    maxSeparation = maxSeparation - (fVar4 + fVar4);
  }
  fVar4 = normalSign * (*paMVar6)[0];
  fVar5 = normalSign * (*paMVar6)[1];
  fVar9 = normalSign * (*paMVar6)[2];
  outN->v[0] = fVar4;
  outN->v[1] = fVar5;
  outN->v[2] = fVar9;
  paMVar6 = inTri->vertices[0];
  *outPN = fVar9 * (*paMVar6)[2] + fVar4 * (*paMVar6)[0] + fVar5 * (*paMVar6)[1];
  uVar14 = (ushort)(fVar3 < 0.0001);
  if (ABS(fVar1) < 0.0001) {
    uVar14 = (fVar3 < 0.0001) + 1;
  }
  if (ABS(fVar2) < 0.0001) {
    uVar14 = uVar14 + 1;
  }
  iVar15 = 0;
  *outDims = uVar14 | 0x200;
  i = 0;
  paMVar6 = inTri->normal;
  do {
                    /* Unresolved local var: MeReal maxCoord@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal separation@[DW_OP_reg12(ST1)] */
    if (ABS(*(float *)((int)*paMVar6 + iVar15)) <= 1.0 - inEps) {
      pfVar18 = (float *)((int)minCoord.v + iVar15);
      pfVar10 = (float *)((int)*inTri->vertices[0] + iVar15);
      fVar1 = *pfVar10;
      *pfVar18 = fVar1;
      fVar2 = *pfVar10;
      paMVar7 = inTri->vertices[1];
      fVar3 = *(float *)((int)*paMVar7 + iVar15);
      if (fVar3 < fVar1) {
        *pfVar18 = fVar3;
      }
      fVar1 = *(float *)((int)*paMVar7 + iVar15);
      if (fVar2 < fVar1) {
        fVar2 = fVar1;
      }
      pfVar18 = (float *)((int)minCoord.v + iVar15);
      paMVar7 = inTri->vertices[2];
      fVar1 = *(float *)((int)*paMVar7 + iVar15);
      if (fVar1 < *pfVar18) {
        *pfVar18 = fVar1;
      }
      fVar1 = *(float *)((int)*paMVar7 + iVar15);
      if (fVar2 < fVar1) {
        fVar2 = fVar1;
      }
      fVar1 = *(float *)((int)minCoord.v + iVar15);
      fVar4 = (fVar2 + fVar1) * 0.5;
      fVar3 = *(float *)((int)inR->v + iVar15);
      fVar1 = ABS(fVar4) - ((fVar2 - fVar1) * 0.5 + fVar3);
      if (maxSeparation < fVar1) {
        normalSign = 1.0;
        PN = -fVar3 - fVar1;
        if (0.0 < fVar4) {
          normalSign = -1.0;
        }
        normInfo = (byte)i | 0xc;
        maxSeparation = fVar1;
        if (inEps < fVar1) {
          return false;
        }
      }
    }
    iVar15 = iVar15 + 4;
    i = i + 1;
  } while (i < 3);
  fVar1 = (*edge)[0];
  fVar2 = (*edge)[1];
  fVar3 = (*edge)[2];
  aE[0].v[0] = ABS(fVar1);
  aE[0].v[1] = ABS(fVar2);
  aE[0].v[2] = ABS(fVar3);
  fVar4 = edge[1][0];
  aE[1].v[0] = ABS(fVar4);
  aE[1].v[1] = ABS(edge[1][1]);
  aE[1].v[2] = ABS(edge[1][2]);
  fVar5 = edge[2][0];
  outVec.v[0] = ABS(fVar5);
  outVec.v[1] = ABS(edge[2][1]);
  sqE[0] = fVar1 * fVar1 + fVar2 * fVar2 + fVar3 * fVar3;
  outVec.v[2] = ABS(edge[2][2]);
  aE[2].v[0] = outVec.v[0];
  aE[2].v[1] = outVec.v[1];
  aE[2].v[2] = outVec.v[2];
  local_ec = aE;
  sqE[1] = fVar4 * fVar4 + edge[1][1] * edge[1][1] + edge[1][2] * edge[1][2];
  i = 0;
  local_e4 = 0;
  local_e8 = 0;
  sqE[2] = edge[2][2] * edge[2][2] + fVar5 * fVar5 + edge[2][1] * edge[2][1];
  do {
                    /* Unresolved local var: int i1@[???] */
    uVar11 = 1 << ((byte)i & 0x1f) & 3;
    iVar15 = 0;
    pfVar18 = (float *)((int)aE[0].v + local_e8);
    do {
                    /* Unresolved local var: int j1@[???]
                       Unresolved local var: int j2@[???]
                       Unresolved local var: MeReal sR@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal rB@[???]
                       Unresolved local var: MeReal rLen@[DW_OP_reg11(ST0)] */
      uVar12 = 1 << ((byte)iVar15 & 0x1f) & 3;
      local_f8 = (sbyte)uVar12;
      uVar17 = 1 << local_f8 & 3;
      fVar2 = (edge[uVar11][uVar12] * (*edge)[local_e4 + uVar17] -
              edge[uVar11][uVar17] * (*edge)[local_e4 + uVar12]) * 0.5;
      fVar1 = sqE[i] - *pfVar18 * *pfVar18;
      fVar3 = ((*edge)[local_e4 + uVar17] * (*inTri->vertices[i])[uVar12] -
              (*edge)[local_e4 + uVar12] * (*inTri->vertices[i])[uVar17]) + fVar2;
      fVar2 = ABS(fVar2);
      if (eps * eps * sqE[i] < fVar1) {
                    /* Unresolved local var: MeReal aNormD@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal separation@[DW_OP_reg15(ST4)]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
        fVar1 = SQRT(fVar1);
        fVar4 = 0.0;
        if (fVar1 != 0.0) {
          fVar4 = 1.0 / fVar1;
        }
        fVar1 = ABS(fVar3) * fVar4;
        fVar5 = fVar1 - (local_ec->v[uVar17] * inR->v[uVar12] + local_ec->v[uVar12] * inR->v[uVar17]
                        + fVar2) * fVar4;
        if (eps < fVar5 - maxSeparation) {
          PN = fVar2 * fVar4 - fVar1;
          normalSign = 1.0;
          if (0.0 < fVar3) {
            normalSign = -1.0;
          }
          normInfo = (byte)(i << 2) | (byte)iVar15;
          nRLen = fVar4;
          maxSeparation = fVar5;
          if (inEps < fVar5) {
            return false;
          }
        }
      }
      iVar15 = iVar15 + 1;
      pfVar18 = pfVar18 + 1;
    } while (iVar15 < 3);
    i = i + 1;
    local_e4 = local_e4 + 3;
    local_e8 = local_e8 + 0xc;
    local_ec = local_ec + 1;
  } while (i < 3);
  *outSep = maxSeparation;
  if ((inTri->flags & kMcdTriangleUseSmallestPenetration) == 0) {
LAB_000110d5:
    if (normInfo != '\x03') goto LAB_000110de;
  }
  else {
    if ((normInfo & 0xc) == 0xc) {
                    /* Unresolved local var: MeI8 axis@[???]
                       Unresolved local var: MeI16 dimB@[???] */
      outN->v[1] = 0.0;
      outN->v[0] = 0.0;
      outN->v[2] = 0.0;
      uVar11 = normInfo & 3;
      outN->v[uVar11] = normalSign;
      *outPN = -inR->v[uVar11] - maxSeparation;
      uVar14 = (ushort)((*inTri->vertices[0])[uVar11] - minCoord.v[uVar11] <= eps);
      if ((*inTri->vertices[1])[uVar11] - minCoord.v[uVar11] <= eps) {
        uVar14 = uVar14 + 1;
      }
      if (eps < (*inTri->vertices[2])[uVar11] - minCoord.v[uVar11]) {
        uVar14 = uVar14 - 1;
      }
      *outDims = uVar14 << 8 | 2;
      goto LAB_000110d5;
    }
    if (normInfo != '\x03') {
      if (normalSign <= 0.0) {
        uVar12 = normInfo >> 2 & 3;
        uVar17 = 1 << (sbyte)(normInfo & 3) & 3;
        uVar11 = 1 << (sbyte)uVar17 & 3;
        outVec.v[normInfo & 3] = 0.0;
        outVec.v[uVar17] = -edge[uVar12][uVar11];
        MVar13 = edge[uVar12][uVar17];
      }
      else {
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
        bVar16 = (normInfo & 0xc) >> 2;
        uVar12 = 1 << (sbyte)(normInfo & 3) & 3;
        uVar11 = 1 << (sbyte)uVar12 & 3;
        outVec.v[normInfo & 3] = 0.0;
        outVec.v[uVar12] = edge[bVar16][uVar11];
        MVar13 = -edge[bVar16][uVar12];
      }
      outVec.v[uVar11] = MVar13;
      outN->v[0] = outVec.v[0];
      outN->v[1] = outVec.v[1];
      outN->v[2] = outVec.v[2];
      *outPN = PN;
      *outDims = 0x101;
      outN->v[0] = nRLen * outN->v[0];
      outN->v[1] = nRLen * outN->v[1];
      outN->v[2] = nRLen * outN->v[2];
      goto LAB_000110d5;
    }
  }
  *outPN = *outPN - maxSeparation;
LAB_000110de:
  plVar8 = *outPos;
  McdVanillaBoxTriIntersect(outPos,inR,inTri,edge,scale);
  return *outPos != plVar8;
}


/* ==== McdVanillaBoxTriIntersect ==== */

void McdVanillaBoxTriIntersect
               (lsVec3 **outList,lsVec3 *inRBox,McdUserTriangle *inTri,MeReal (*edge) [3],
               MeReal scale)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  lsVec3 *plVar5;
  lsVec3 *plVar6;
  MeVector3 *paMVar7;
  MeVector3 *paMVar8;
  int iVar9;
  McdTriangleFlags MVar10;
  MeVector3 *paMVar11;
  MeReal triD;
  MeReal tOut_2;
  MeReal tIn_2;
  MeReal tOut_1;
  MeReal tIn_1;
  MeReal tOut;
  MeReal tIn;
  lsVec3 vCross;
  lsVec3 axb [3];
  
  MVar10 = inTri->flags;
  if ((MVar10 & kMcdTriangleUseEdge0) != 0) {
    axb[2].v[2] = 0.0;
    if ((*edge)[0] != 0.0) {
      axb[2].v[2] = 1.0 / (*edge)[0];
    }
    plVar5 = (lsVec3 *)inTri->vertices[0];
    tOut = 1.0;
    tIn = 0.0;
    iVar9 = McdVanillaSegmentCubeIntersect
                      (&tIn,&tOut,plVar5,(lsVec3 *)edge,(lsVec3 *)(axb[2].v + 2),inRBox,
                       scale * 1e-06);
    if (iVar9 != 0) {
      plVar6 = *outList;
      *outList = plVar6 + 1;
      axb[1].v[2] = tIn * (*edge)[1] + plVar5->v[1];
      axb[1].v[1] = tIn * (*edge)[0] + plVar5->v[0];
      axb[2].v[0] = tIn * (*edge)[2] + plVar5->v[2];
      plVar6->v[0] = axb[1].v[1];
      plVar6->v[1] = axb[1].v[2];
      plVar6->v[2] = axb[2].v[0];
      if (tOut < 1.0) {
        plVar6 = *outList;
        *outList = plVar6 + 1;
        axb[1].v[1] = tOut * (*edge)[0];
        axb[1].v[2] = tOut * (*edge)[1];
        axb[2].v[0] = tOut * (*edge)[2];
        fVar1 = plVar5->v[1];
        fVar2 = plVar5->v[2];
        plVar6->v[0] = axb[1].v[1] + plVar5->v[0];
        plVar6->v[1] = axb[1].v[2] + fVar1;
        plVar6->v[2] = axb[2].v[0] + fVar2;
      }
    }
    MVar10 = inTri->flags;
  }
  if ((MVar10 & kMcdTriangleUseEdge1) != 0) {
    axb[0].v[0] = 0.0;
    if (edge[1][0] != 0.0) {
      axb[0].v[0] = 1.0 / edge[1][0];
    }
    axb[0].v[1] = 0.0;
    if (edge[1][1] != 0.0) {
      axb[0].v[1] = 1.0 / edge[1][1];
    }
    axb[0].v[2] = 0.0;
    if (edge[1][2] != 0.0) {
      axb[0].v[2] = 1.0 / edge[1][2];
    }
    plVar5 = (lsVec3 *)inTri->vertices[1];
    tOut_1 = 1.0;
    tIn_1 = 0.0;
    iVar9 = McdVanillaSegmentCubeIntersect
                      (&tIn_1,&tOut_1,plVar5,(lsVec3 *)(edge + 1),axb,inRBox,scale * 1e-06);
    if (iVar9 != 0) {
      plVar6 = *outList;
      *outList = plVar6 + 1;
      axb[2].v[2] = tIn_1 * edge[1][0];
      axb[1].v[2] = tIn_1 * edge[1][1] + plVar5->v[1];
      axb[1].v[1] = axb[2].v[2] + plVar5->v[0];
      axb[2].v[0] = tIn_1 * edge[1][2] + plVar5->v[2];
      plVar6->v[0] = axb[1].v[1];
      plVar6->v[1] = axb[1].v[2];
      plVar6->v[2] = axb[2].v[0];
      if (tOut_1 < 1.0) {
        plVar6 = *outList;
        *outList = plVar6 + 1;
        axb[2].v[2] = tOut_1 * edge[1][0];
        axb[1].v[1] = axb[2].v[2] + plVar5->v[0];
        axb[1].v[2] = tOut_1 * edge[1][1] + plVar5->v[1];
        axb[2].v[0] = tOut_1 * edge[1][2] + plVar5->v[2];
        plVar6->v[0] = axb[1].v[1];
        plVar6->v[1] = axb[1].v[2];
        plVar6->v[2] = axb[2].v[0];
      }
    }
    MVar10 = inTri->flags;
  }
  if ((MVar10 & kMcdTriangleUseEdge2) != 0) {
    axb[0].v[0] = 0.0;
    if (edge[2][0] != 0.0) {
      axb[0].v[0] = 1.0 / edge[2][0];
    }
    axb[0].v[1] = 0.0;
    if (edge[2][1] != 0.0) {
      axb[0].v[1] = 1.0 / edge[2][1];
    }
    axb[0].v[2] = 0.0;
    if (edge[2][2] != 0.0) {
      axb[0].v[2] = 1.0 / edge[2][2];
    }
    plVar5 = (lsVec3 *)inTri->vertices[2];
    tOut_2 = 1.0;
    tIn_2 = 0.0;
    iVar9 = McdVanillaSegmentCubeIntersect
                      (&tIn_2,&tOut_2,plVar5,(lsVec3 *)(edge + 2),axb,inRBox,scale * 1e-06);
    if (iVar9 != 0) {
      plVar6 = *outList;
      *outList = plVar6 + 1;
      fVar1 = edge[2][1];
      fVar2 = edge[2][2];
      fVar3 = plVar5->v[1];
      fVar4 = plVar5->v[2];
      plVar6->v[0] = tIn_2 * edge[2][0] + plVar5->v[0];
      plVar6->v[1] = tIn_2 * fVar1 + fVar3;
      plVar6->v[2] = tIn_2 * fVar2 + fVar4;
      if (tOut_2 < 1.0) {
        plVar6 = *outList;
        *outList = plVar6 + 1;
        fVar1 = edge[2][1];
        fVar2 = edge[2][2];
        fVar3 = plVar5->v[1];
        fVar4 = plVar5->v[2];
        plVar6->v[0] = tOut_2 * edge[2][0] + plVar5->v[0];
        plVar6->v[1] = tOut_2 * fVar1 + fVar3;
        plVar6->v[2] = tOut_2 * fVar2 + fVar4;
      }
    }
  }
  paMVar11 = inTri->vertices[1];
  paMVar7 = inTri->vertices[0];
  axb[0].v[0] = (*paMVar11)[2] * (*paMVar7)[1] - (*paMVar11)[1] * (*paMVar7)[2];
  axb[0].v[1] = (*paMVar11)[0] * (*paMVar7)[2] - (*paMVar11)[2] * (*paMVar7)[0];
  axb[0].v[2] = (*paMVar11)[1] * (*paMVar7)[0] - (*paMVar11)[0] * (*paMVar7)[1];
  paMVar8 = inTri->vertices[2];
  axb[1].v[0] = (*paMVar8)[2] * (*paMVar11)[1] - (*paMVar8)[1] * (*paMVar11)[2];
  axb[1].v[1] = (*paMVar8)[0] * (*paMVar11)[2] - (*paMVar8)[2] * (*paMVar11)[0];
  axb[1].v[2] = (*paMVar8)[1] * (*paMVar11)[0] - (*paMVar8)[0] * (*paMVar11)[1];
  vCross.v[0] = (*paMVar7)[2] * (*paMVar8)[1] - (*paMVar7)[1] * (*paMVar8)[2];
  vCross.v[1] = (*paMVar7)[0] * (*paMVar8)[2] - (*paMVar7)[2] * (*paMVar8)[0];
  vCross.v[2] = (*paMVar7)[1] * (*paMVar8)[0] - (*paMVar7)[0] * (*paMVar8)[1];
  paMVar11 = inTri->normal;
  fVar1 = (*paMVar11)[2] * (*paMVar7)[2] +
          (*paMVar11)[1] * (*paMVar7)[1] + (*paMVar7)[0] * (*paMVar11)[0];
  axb[2].v[0] = vCross.v[0];
  axb[2].v[1] = vCross.v[1];
  axb[2].v[2] = vCross.v[2];
  if ((*paMVar11)[0] != 0.0) {
    McdVanillaAddTriBoxSegmentPoints(outList,inRBox,0,1,2,inTri,edge,axb,fVar1);
    paMVar11 = inTri->normal;
  }
  if ((*paMVar11)[1] != 0.0) {
    McdVanillaAddTriBoxSegmentPoints(outList,inRBox,1,2,0,inTri,edge,axb,fVar1);
    paMVar11 = inTri->normal;
  }
  if ((*paMVar11)[2] != 0.0) {
    McdVanillaAddTriBoxSegmentPoints(outList,inRBox,2,0,1,inTri,edge,axb,fVar1);
  }
  return;
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


/* ==== McdVanillaAddTriBoxSegmentPoints ==== */

void McdVanillaAddTriBoxSegmentPoints
               (lsVec3 **outList,lsVec3 *inRBox,int i0,int i1,int i2,McdUserTriangle *inTri,
               MeReal (*edge) [3],lsVec3 *axb,MeReal inTriD)

{
  float fVar1;
  float fVar2;
  float fVar3;
  MeVector3 *paMVar4;
  bool bVar5;
  bool bVar6;
  bool bVar7;
  float fVar8;
  MeReal *pMVar9;
  MeReal *pMVar10;
  MeReal *pMVar11;
  MeReal *pMVar12;
  uint uVar13;
  uint uVar14;
  MeReal *pMVar15;
  MeReal *pMVar16;
  MeReal *pMVar17;
  bool ccw;
  lsVec3 dn;
  lsVec3 x [3];
  
                    /* Unresolved local var: MeReal r0@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal den@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal recipDen@[???] */
  fVar1 = inRBox->v[i0];
  paMVar4 = inTri->normal;
  fVar8 = 0.0;
  fVar2 = (*paMVar4)[i0];
  if (fVar2 != 0.0) {
    fVar8 = 1.0 / fVar2;
  }
  bVar5 = fVar2 < 0.0;
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
  uVar13 = 1 << ((byte)i0 & 0x1f) & 3;
  pMVar12 = dn.v + i0;
  uVar14 = 1 << (sbyte)uVar13 & 3;
  pMVar9 = dn.v + uVar13;
  *pMVar12 = 0.0;
  *pMVar9 = (*edge)[uVar14];
  pMVar15 = dn.v + uVar14;
  *pMVar15 = -(*edge)[uVar13];
  x[0].v[0] = dn.v[0];
  x[0].v[1] = dn.v[1];
  x[0].v[2] = dn.v[2];
  pMVar10 = x[0].v + i1;
  fVar2 = inRBox->v[i1];
  *pMVar10 = *pMVar10 * fVar2;
  pMVar11 = x[0].v + i2;
  fVar3 = inRBox->v[i2];
  *pMVar11 = *pMVar11 * fVar3;
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
  *pMVar12 = 0.0;
  *pMVar9 = edge[1][uVar14];
  *pMVar15 = -edge[1][uVar13];
  x[1].v[1] = dn.v[1];
  x[1].v[0] = dn.v[0];
  pMVar16 = x[1].v + i1;
  x[1].v[2] = dn.v[2];
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
  *pMVar16 = *pMVar16 * fVar2;
  pMVar17 = x[1].v + i2;
  *pMVar17 = *pMVar17 * fVar3;
  *pMVar12 = 0.0;
  *pMVar9 = edge[2][uVar14];
  *pMVar15 = -edge[2][uVar13];
  x[2].v[0] = dn.v[0];
  x[2].v[1] = dn.v[1];
  x[2].v[2] = dn.v[2];
  pMVar9 = x[2].v + i1;
  *pMVar9 = *pMVar9 * fVar2;
  pMVar15 = x[2].v + i2;
  *pMVar15 = *pMVar15 * fVar3;
  *pMVar12 = 0.0;
  dn.v[i1] = fVar2 * (*paMVar4)[i1];
  fVar3 = fVar3 * (*paMVar4)[i2];
  dn.v[i2] = fVar3;
  bVar6 = (axb->v[i0] - *pMVar10) - *pMVar11 < 0.0;
  if ((((bVar5 == bVar6) && (bVar7 = (axb[1].v[i0] - *pMVar16) - *pMVar17 < 0.0, bVar7 == bVar6)) &&
      ((axb[2].v[i0] - *pMVar9) - *pMVar15 < 0.0 == bVar7)) &&
     ((fVar2 = ((inTriD - dn.v[i1]) - fVar3) * fVar8, -fVar1 <= fVar2 && (fVar2 <= fVar1)))) {
    (*outList)->v[i0] = fVar2;
    (*outList)->v[i1] = inRBox->v[i1];
    (*outList)->v[i2] = inRBox->v[i2];
    *outList = *outList + 1;
  }
  bVar6 = (axb->v[i0] - x[0].v[i1]) + x[0].v[i2] < 0.0;
                    /* Unresolved local var: MeReal r@[DW_OP_reg12(ST1)] */
  if (((bVar5 == bVar6) && (bVar7 = (axb[1].v[i0] - x[1].v[i1]) + x[1].v[i2] < 0.0, bVar7 == bVar6))
     && (((axb[2].v[i0] - x[2].v[i1]) + x[2].v[i2] < 0.0 == bVar7 &&
         ((fVar2 = ((inTriD - dn.v[i1]) + dn.v[i2]) * fVar8, -fVar1 <= fVar2 && (fVar2 <= fVar1)))))
     ) {
    (*outList)->v[i0] = fVar2;
    (*outList)->v[i1] = inRBox->v[i1];
    (*outList)->v[i2] = -inRBox->v[i2];
    *outList = *outList + 1;
  }
  bVar6 = x[0].v[i1] + axb->v[i0] + x[0].v[i2] < 0.0;
                    /* Unresolved local var: MeReal r@[DW_OP_reg12(ST1)] */
  if ((((bVar5 == bVar6) && (bVar7 = x[1].v[i1] + axb[1].v[i0] + x[1].v[i2] < 0.0, bVar7 == bVar6))
      && (x[2].v[i1] + axb[2].v[i0] + x[2].v[i2] < 0.0 == bVar7)) &&
     ((fVar2 = (inTriD + dn.v[i1] + dn.v[i2]) * fVar8, -fVar1 <= fVar2 && (fVar2 <= fVar1)))) {
    (*outList)->v[i0] = fVar2;
    (*outList)->v[i1] = -inRBox->v[i1];
    (*outList)->v[i2] = -inRBox->v[i2];
    *outList = *outList + 1;
  }
  bVar6 = (x[0].v[i1] + axb->v[i0]) - x[0].v[i2] < 0.0;
  if (((bVar5 == bVar6) && (bVar5 = (x[1].v[i1] + axb[1].v[i0]) - x[1].v[i2] < 0.0, bVar5 == bVar6))
     && (((x[2].v[i1] + axb[2].v[i0]) - x[2].v[i2] < 0.0 == bVar5 &&
         ((fVar8 = ((inTriD + dn.v[i1]) - dn.v[i2]) * fVar8, -fVar1 <= fVar8 && (fVar8 <= fVar1)))))
     ) {
    (*outList)->v[i0] = fVar8;
    (*outList)->v[i1] = -inRBox->v[i1];
    (*outList)->v[i2] = inRBox->v[i2];
    *outList = *outList + 1;
  }
  return;
}


