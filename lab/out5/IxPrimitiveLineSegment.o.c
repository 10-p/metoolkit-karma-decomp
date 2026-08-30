/* ==== IxBoxLineSegment ==== */

int IxBoxLineSegment(McdModelID_conflict model,MeReal *inOrig,MeReal *inDest,
                    McdLineSegIntersectResult *info)

{
  byte *pbVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  void *pvVar11;
  float *pfVar12;
  MeReal *pMVar13;
  OverlapStatus OVar14;
  int iVar15;
  McdGeometry *box;
  MeReal dist;
  FaceId faceId;
  lsVec3 insctPoint;
  lsVec3 dirLS;
  lsVec3 outInst;
  lsVec3 inMaxB;
  lsVec3 minB;
  lsVec3 p1_inboxcs;
  lsVec3 p0_inboxcs;
  
                    /* Unresolved local var: lsTransform * gtm@[DW_OP_reg6(ESI)]
                       Unresolved local var: lsVec3 * r@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[???]
                       Unresolved local var: MeReal normDir@[???]
                       Unresolved local var: MeReal norDirInv@[???]
                       Unresolved local var: OverlapStatus bIsct@[DW_OP_reg2(EDX)]
                       Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
  pvVar11 = McdModelGetGeometry(model);
  pfVar12 = McdModelGetTransformPtr(model);
  fVar2 = *inOrig - pfVar12[0xc];
  fVar3 = inOrig[1] - pfVar12[0xd];
  fVar4 = inOrig[2] - pfVar12[0xe];
  p0_inboxcs.v[0] = fVar4 * pfVar12[2] + fVar3 * pfVar12[1] + fVar2 * *pfVar12;
  p0_inboxcs.v[1] = fVar4 * pfVar12[6] + fVar2 * pfVar12[4] + fVar3 * pfVar12[5];
  p0_inboxcs.v[2] = fVar4 * pfVar12[10] + fVar2 * pfVar12[8] + fVar3 * pfVar12[9];
  minB.v[0] = *inDest - pfVar12[0xc];
  minB.v[1] = inDest[1] - pfVar12[0xd];
  minB.v[2] = inDest[2] - pfVar12[0xe];
  fVar2 = minB.v[0] * *pfVar12;
  fVar10 = minB.v[1] * pfVar12[1];
  fVar3 = minB.v[2] * pfVar12[2];
  fVar6 = minB.v[1] * pfVar12[5];
  fVar8 = minB.v[0] * pfVar12[4];
  fVar7 = minB.v[2] * pfVar12[6];
  fVar5 = minB.v[1] * pfVar12[9];
  fVar9 = minB.v[0] * pfVar12[8];
  fVar4 = minB.v[2] * pfVar12[10];
  pMVar13 = McdBoxGetRadii(pvVar11);
  minB.v[0] = -*pMVar13;
  inMaxB.v[0] = *pMVar13;
  minB.v[1] = -pMVar13[1];
  inMaxB.v[1] = pMVar13[1];
  insctPoint.v[0] = (fVar3 + fVar10 + fVar2) - p0_inboxcs.v[0];
  insctPoint.v[1] = (fVar7 + fVar8 + fVar6) - p0_inboxcs.v[1];
  minB.v[2] = -pMVar13[2];
  insctPoint.v[2] = (fVar4 + fVar9 + fVar5) - p0_inboxcs.v[2];
  inMaxB.v[2] = pMVar13[2];
  fVar2 = SQRT(insctPoint.v[2] * insctPoint.v[2] +
               insctPoint.v[0] * insctPoint.v[0] + insctPoint.v[1] * insctPoint.v[1]);
  iVar15 = 0;
  if (1e-06 <= fVar2) {
    fVar3 = 0.0;
    if (fVar2 != 0.0) {
      fVar3 = 1.0 / fVar2;
    }
    dirLS.v[0] = insctPoint.v[0] * fVar3;
    dirLS.v[1] = insctPoint.v[1] * fVar3;
    dirLS.v[2] = fVar3 * insctPoint.v[2];
    OVar14 = IxRayAABB(minB.v,inMaxB.v,p0_inboxcs.v,dirLS.v,insctPoint.v,&dist,&faceId);
    if ((OVar14 == DISJOINT) || (fVar2 < dist)) {
      iVar15 = 0;
    }
    else {
      info->position[0] =
           insctPoint.v[2] * pfVar12[8] + insctPoint.v[1] * pfVar12[4] + insctPoint.v[0] * *pfVar12
           + pfVar12[0xc];
      info->position[1] =
           insctPoint.v[2] * pfVar12[9] +
           insctPoint.v[1] * pfVar12[5] + insctPoint.v[0] * pfVar12[1] + pfVar12[0xd];
      info->position[2] =
           insctPoint.v[2] * pfVar12[10] +
           insctPoint.v[0] * pfVar12[2] + insctPoint.v[1] * pfVar12[6] + pfVar12[0xe];
      if (OVar14 == INTERSECT) {
                    /* Unresolved local var: lsVec3 * axis@[DW_OP_reg1(ECX)] */
        pfVar12 = pfVar12 + ((uint)faceId & 0xffff) * 4;
        info->normal[0] = *pfVar12;
        info->normal[1] = pfVar12[1];
        info->normal[2] = pfVar12[2];
        if (faceId.minside != 0) {
          pbVar1 = (byte *)((int)info->normal + 3);
          *pbVar1 = *pbVar1 ^ 0x80;
          pbVar1 = (byte *)((int)info->normal + 7);
          *pbVar1 = *pbVar1 ^ 0x80;
          pbVar1 = (byte *)((int)info->normal + 0xb);
          *pbVar1 = *pbVar1 ^ 0x80;
        }
      }
      iVar15 = 1;
      info->distance = dist;
    }
  }
  return iVar15;
}


/* ==== IxRayAABB ==== */

/* WARNING: Restarted to delay deadcode elimination for space: stack */

OverlapStatus
IxRayAABB(MeReal *inMinB,MeReal *inMaxB,MeReal *inOrig,MeReal *inDir,MeReal *outInsct,MeReal *outLen
         ,FaceId *outFaceId)

{
  float fVar1;
  float fVar2;
  bool bVar3;
  uint uVar4;
  int i;
  MeReal candPlane [3];
  MeReal maxT [3];
  uint iQuad [3];
  
                    /* Unresolved local var: bool insideBox@[DW_OP_reg2(EDX)]
                       Unresolved local var: int iPlane@[DW_OP_reg2(EDX)] */
  bVar3 = true;
  if (*inMinB <= *inOrig) {
    if (*inMaxB < *inOrig) {
      iQuad[0] = 0;
      candPlane[0] = *inMaxB;
      goto LAB_0001035d;
    }
    iQuad[0] = 2;
  }
  else {
    iQuad[0] = 1;
    candPlane[0] = *inMinB;
LAB_0001035d:
    bVar3 = false;
  }
  if (inMinB[1] <= inOrig[1]) {
    if (inMaxB[1] < inOrig[1]) {
      iQuad[1] = 0;
      candPlane[1] = inMaxB[1];
      goto LAB_00010387;
    }
    iQuad[1] = 2;
  }
  else {
    iQuad[1] = 1;
    candPlane[1] = inMinB[1];
LAB_00010387:
    bVar3 = false;
  }
  if (inMinB[2] <= inOrig[2]) {
    if (inMaxB[2] < inOrig[2]) {
      iQuad[2] = 0;
      candPlane[2] = inMaxB[2];
      goto LAB_000103b1;
    }
    iQuad[2] = 2;
  }
  else {
    iQuad[2] = 1;
    candPlane[2] = inMinB[2];
LAB_000103b1:
    bVar3 = false;
  }
  if (bVar3) {
    *outInsct = *inOrig;
    outInsct[1] = inOrig[1];
    outInsct[2] = inOrig[2];
    *outLen = 0.0;
    return INSIDE;
  }
  if ((iQuad[0] == 2) || (*inDir == 0.0)) {
    maxT[0] = -1.0;
  }
  else {
    maxT[0] = (candPlane[0] - *inOrig) * (1.0 / *inDir);
  }
  if ((iQuad[1] == 2) || (inDir[1] == 0.0)) {
    maxT[1] = -1.0;
  }
  else {
    maxT[1] = (candPlane[1] - inOrig[1]) * (1.0 / inDir[1]);
  }
  if ((iQuad[2] == 2) || (inDir[2] == 0.0)) {
    maxT[2] = -1.0;
  }
  else {
    maxT[2] = (candPlane[2] - inOrig[2]) * (1.0 / inDir[2]);
  }
  fVar1 = maxT[0];
  if (maxT[0] < maxT[1]) {
    fVar1 = maxT[1];
  }
  uVar4 = (uint)(maxT[0] < maxT[1]);
  if (fVar1 < maxT[2]) {
    uVar4 = 2;
    fVar1 = maxT[2];
  }
  if (fVar1 < 0.0) {
    return DISJOINT;
  }
  if (uVar4 == 0) {
    *outInsct = candPlane[0];
LAB_00010500:
    if (uVar4 == 1) {
      outInsct[1] = candPlane[1];
    }
    else {
      fVar2 = fVar1 * inDir[1] + inOrig[1];
      outInsct[1] = fVar2;
      if ((fVar2 < inMinB[1]) || (inMaxB[1] < fVar2)) goto LAB_000104f3;
    }
    if (uVar4 == 2) {
      outInsct[2] = candPlane[2];
    }
    else {
      fVar2 = fVar1 * inDir[2] + inOrig[2];
      outInsct[2] = fVar2;
      if ((fVar2 < inMinB[2]) || (inMaxB[2] < fVar2)) goto LAB_000104f3;
    }
    outFaceId->axis = (MeU16)uVar4;
    outFaceId->minside = (ushort)(iQuad[uVar4] == 1);
    *outLen = fVar1;
    i = 1;
  }
  else {
    fVar2 = fVar1 * *inDir + *inOrig;
    *outInsct = fVar2;
    if ((*inMinB <= fVar2) && (fVar2 <= *inMaxB)) goto LAB_00010500;
LAB_000104f3:
    i = 0;
  }
  return i;
}


/* ==== IxSphereLineSegment ==== */

int IxSphereLineSegment(McdModelID_conflict model,MeReal *inOrig,MeReal *inDest,
                       McdLineSegIntersectResult *info)

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
  float fVar13;
  float fVar14;
  float fVar15;
  void *pvVar16;
  void *pvVar17;
  float fVar18;
  MeReal R;
  lsVec3 origToCenter;
  lsVec3 rayDir;
  lsVec3 center;
  
                    /* Unresolved local var: McdSphereID sphere@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsVec3 * p0@[???]
                       Unresolved local var: lsVec3 * p1@[???]
                       Unresolved local var: lsTransform * gtm@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeReal R2@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal normRayDir@[???]
                       Unresolved local var: MeReal normRayDirInv@[???]
                       Unresolved local var: MeReal D@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal L2@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal D2@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal M2@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal Q@[???]
                       Unresolved local var: MeReal t@[DW_OP_reg14(ST3)]
                       Unresolved local var: lsVec3 * p@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeReal RI@[???]
                       Unresolved local var: float __result@[DW_OP_reg17(ST6)] */
  pvVar16 = McdModelGetGeometry(model);
  pvVar17 = McdModelGetTransformPtr(model);
  fVar3 = *(float *)((int)pvVar17 + 0x30);
  fVar4 = *(float *)((int)pvVar17 + 0x34);
  fVar5 = *(float *)((int)pvVar17 + 0x38);
  fVar18 = McdSphereGetRadius(pvVar16);
  fVar6 = fVar18 * fVar18;
  fVar15 = *inDest - *inOrig;
  fVar14 = inDest[1] - inOrig[1];
  fVar13 = inDest[2] - inOrig[2];
  fVar12 = SQRT(fVar15 * fVar15 + fVar14 * fVar14 + fVar13 * fVar13);
  if (fVar12 < 1e-06) {
    return 0;
  }
  fVar8 = 0.0;
  if (fVar12 != 0.0) {
    fVar8 = 1.0 / fVar12;
  }
  fVar1 = inOrig[1];
  fVar9 = fVar3 - *inOrig;
  fVar10 = fVar4 - fVar1;
  fVar2 = inOrig[2];
  fVar11 = fVar5 - fVar2;
  fVar7 = fVar15 * fVar8 * fVar9 + fVar14 * fVar8 * fVar10 + fVar13 * fVar8 * fVar11;
  fVar9 = fVar11 * fVar11 + fVar9 * fVar9 + fVar10 * fVar10;
  if (fVar6 <= fVar9) {
    if ((fVar7 < 0.0) && (fVar6 < fVar9)) {
      return 0;
    }
    fVar9 = fVar9 - fVar7 * fVar7;
    if (fVar6 < fVar9) {
      return 0;
    }
                    /* Unresolved local var: float __result@[???] */
    fVar7 = fVar7 - SQRT(fVar6 - fVar9);
    if (fVar12 < fVar7) {
      return 0;
    }
    info->position[0] = fVar15 * fVar8 * fVar7 + *inOrig;
    info->position[1] = fVar14 * fVar8 * fVar7 + fVar1;
    info->position[2] = fVar13 * fVar8 * fVar7 + fVar2;
    fVar6 = 0.0;
    if (fVar18 != 0.0) {
      fVar6 = 1.0 / fVar18;
    }
                    /* Unresolved local var: int i@[???] */
    info->normal[0] = (info->position[0] - fVar3) * fVar6;
    info->normal[1] = (info->position[1] - fVar4) * fVar6;
    info->distance = fVar7;
    info->normal[2] = (info->position[2] - fVar5) * fVar6;
  }
  else {
    info->distance = 0.0;
    info->position[0] = *inOrig;
    info->position[1] = inOrig[1];
    info->position[2] = inOrig[2];
  }
  return 1;
}


/* ==== IxPlaneLineSegment ==== */

int IxPlaneLineSegment(McdModelID_conflict model,MeReal *inOrig,MeReal *inDest,
                      McdLineSegIntersectResult *info)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float *pfVar9;
  int iVar10;
  lsVec3 ip;
  lsVec3 dirLS;
  lsVec3 p1_intrics;
  lsVec3 p0_intrics;
  
                    /* Unresolved local var: McdGeometry * plane@[???]
                       Unresolved local var: lsVec3 * p0@[???]
                       Unresolved local var: lsVec3 * p1@[???]
                       Unresolved local var: lsTransform * gtm@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal t@[DW_OP_reg17(ST6)]
                       Unresolved local var: float __result@[???] */
  McdModelGetGeometry(model);
  pfVar9 = McdModelGetTransformPtr(model);
  fVar1 = *inOrig - pfVar9[0xc];
  fVar6 = inOrig[1] - pfVar9[0xd];
  fVar7 = inOrig[2] - pfVar9[0xe];
  fVar2 = fVar7 * pfVar9[2] + fVar6 * pfVar9[1] + fVar1 * *pfVar9;
  fVar4 = fVar7 * pfVar9[6] + fVar1 * pfVar9[4] + fVar6 * pfVar9[5];
  fVar1 = fVar7 * pfVar9[10] + fVar1 * pfVar9[8] + fVar6 * pfVar9[9];
  fVar8 = *inDest - pfVar9[0xc];
  fVar3 = inDest[1] - pfVar9[0xd];
  fVar7 = inDest[2] - pfVar9[0xe];
  fVar6 = fVar8 * pfVar9[8] + fVar3 * pfVar9[9] + fVar7 * pfVar9[10];
  if ((fVar1 <= 0.0) || (iVar10 = 0, fVar6 <= 0.0)) {
    if (0.0 <= fVar1) {
      fVar6 = fVar6 - fVar1;
      fVar5 = (fVar7 * pfVar9[2] + fVar3 * pfVar9[1] + fVar8 * *pfVar9) - fVar2;
      fVar7 = (fVar7 * pfVar9[6] + fVar8 * pfVar9[4] + fVar3 * pfVar9[5]) - fVar4;
      if ((ABS(fVar6) < 1e-06) && (0.0 < fVar1)) {
        return 0;
      }
      fVar3 = -fVar1 / fVar6;
      fVar2 = fVar5 * fVar3 + fVar2;
      fVar4 = fVar7 * fVar3 + fVar4;
      info->position[0] = fVar2 * *pfVar9 + fVar4 * pfVar9[4] + pfVar9[0xc];
      info->position[1] = fVar4 * pfVar9[5] + fVar2 * pfVar9[1] + pfVar9[0xd];
      info->position[2] = fVar4 * pfVar9[6] + fVar2 * pfVar9[2] + pfVar9[0xe];
      info->normal[0] = pfVar9[8];
      info->normal[1] = pfVar9[9];
      info->normal[2] = pfVar9[10];
      fVar3 = SQRT(fVar6 * fVar6 + fVar5 * fVar5 + fVar7 * fVar7) * fVar3;
      info->distance = fVar3;
      if (fVar1 < 0.0) {
        info->distance = -fVar3;
      }
    }
    else {
      info->position[0] = *inOrig;
      info->position[1] = inOrig[1];
      info->position[2] = inOrig[2];
      info->normal[0] = pfVar9[8];
      info->normal[1] = pfVar9[9];
      info->normal[2] = pfVar9[10];
      info->distance = 0.0;
    }
    iVar10 = 1;
  }
  return iVar10;
}


/* ==== McdBoxLineSegmentRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

void McdBoxLineSegmentRegisterInteraction(McdFramework *frame)

{
  McdFrameworkSetLineSegInteraction(frame,2,IxBoxLineSegment);
  return;
}


/* ==== McdSphereLineSegmentRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

void McdSphereLineSegmentRegisterInteraction(McdFramework *frame)

{
  McdFrameworkSetLineSegInteraction(frame,1,IxSphereLineSegment);
  return;
}


/* ==== McdPlaneLineSegmentRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

void McdPlaneLineSegmentRegisterInteraction(McdFramework *frame)

{
  McdFrameworkSetLineSegInteraction(frame,3,IxPlaneLineSegment);
  return;
}


