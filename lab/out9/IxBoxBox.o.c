/* ==== __static_initialization_and_destruction_0 ==== */

void __static_initialization_and_destruction_0(int __initialize_p,int __priority)

{
  if ((__priority == 0xffff) && (__initialize_p == 1)) {
    Vec4NegOne.v[0] = -1.0;
    Vec4NegOne.v[1] = -1.0;
    Vec4NegOne.v[2] = -1.0;
    Vec4NegOne.v[3] = -1.0;
    Vec4One.v[0] = 1.0;
    Vec4One.v[1] = 1.0;
    Vec4One.v[2] = 1.0;
    Vec4One.v[3] = 1.0;
    Vec4Zero.v[0] = 0.0;
    Vec4Zero.v[1] = 0.0;
    Vec4Zero.v[2] = 0.0;
    Vec4Zero.v[3] = 0.0;
  }
  return;
}


/* ==== McdBoxBoxIntersect ==== */

MeBool McdBoxBoxIntersect(McdModelPair *p,McdIntersectResult *result)

{
  McdFramework *pMVar1;
  uint uVar2;
  bool bVar3;
  float *pfVar4;
  float *pfVar5;
  void *pvVar6;
  void *pvVar7;
  lsVec3 *inR1;
  lsVec3 *inR2;
  int iVar8;
  McdContact *pMVar9;
  MeBool MVar10;
  lsVec3 *plVar11;
  uint uVar12;
  float fVar13;
  float fVar14;
  lsVec3 *rB;
  lsVec3 *rA;
  McdBoxID geometry2;
  lsVec3 *verts;
  MeReal separation;
  MeReal PN;
  MeI16 dims;
  lsVec3 footprint [48];
  lsVec3 normal;
  lsTransform tAB;
  
                    /* Unresolved local var: lsTransform * tA@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * tB@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal eps@[???]
                       Unresolved local var: McdBoxID geometry1@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdFramework * fwk@[DW_OP_reg6(ESI)]
                       Unresolved local var: uint vertexCount@[???]
                       Unresolved local var: MeVector3 * v@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdContact * c@[DW_OP_reg1(ECX)]
                       Unresolved local var: uint i@[DW_OP_reg6(ESI)] */
  pfVar4 = McdModelGetTransformPtr(p->model1);
  pfVar5 = McdModelGetTransformPtr(p->model2);
  fVar13 = McdModelGetContactTolerance(p->model1);
  fVar14 = McdModelGetContactTolerance(p->model2);
  pvVar6 = McdModelGetGeometry(p->model1);
  pvVar7 = McdModelGetGeometry(p->model2);
  result->touch = 0;
  result->contactCount = 0;
  inR1 = McdBoxGetRadii(pvVar6);
  inR2 = McdBoxGetRadii(pvVar7);
  tAB.row[0].v.v[0] = pfVar4[2] * pfVar5[2] + pfVar4[1] * pfVar5[1] + *pfVar4 * *pfVar5;
  tAB.row[0].v.v[1] = pfVar4[6] * pfVar5[2] + pfVar4[5] * pfVar5[1] + pfVar4[4] * *pfVar5;
  tAB.row[0].v.v[2] = pfVar4[10] * pfVar5[2] + pfVar4[9] * pfVar5[1] + pfVar4[8] * *pfVar5;
  tAB.row[1].v.v[0] = pfVar4[2] * pfVar5[6] + pfVar4[1] * pfVar5[5] + *pfVar4 * pfVar5[4];
  tAB.row[1].v.v[1] = pfVar4[6] * pfVar5[6] + pfVar4[5] * pfVar5[5] + pfVar4[4] * pfVar5[4];
  tAB.row[1].v.v[2] = pfVar4[10] * pfVar5[6] + pfVar4[9] * pfVar5[5] + pfVar4[8] * pfVar5[4];
  tAB.row[2].v.v[0] = pfVar4[2] * pfVar5[10] + pfVar4[1] * pfVar5[9] + *pfVar4 * pfVar5[8];
  tAB.row[2].v.v[1] = pfVar4[6] * pfVar5[10] + pfVar4[5] * pfVar5[9] + pfVar4[4] * pfVar5[8];
  tAB.row[2].v.v[2] = pfVar4[10] * pfVar5[10] + pfVar4[9] * pfVar5[9] + pfVar4[8] * pfVar5[8];
  normal.v[1] = pfVar5[0xd] - pfVar4[0xd];
  normal.v[2] = pfVar5[0xe] - pfVar4[0xe];
  normal.v[0] = pfVar5[0xc] - pfVar4[0xc];
  tAB.row[3].v.v[0] = normal.v[2] * pfVar4[2] + normal.v[0] * *pfVar4 + normal.v[1] * pfVar4[1];
  tAB.row[3].v.v[1] = normal.v[2] * pfVar4[6] + normal.v[0] * pfVar4[4] + normal.v[1] * pfVar4[5];
  tAB.row[3].v.v[2] = normal.v[2] * pfVar4[10] + normal.v[0] * pfVar4[8] + normal.v[1] * pfVar4[9];
  pMVar1 = p->model1->frame;
  bVar3 = McdVanillaOverlapOBBs(&separation,&normal,&PN,&dims,fVar14 + fVar13,inR1,inR2,&tAB);
  MVar10 = 0;
  if (bVar3) {
    iVar8 = 0x2f;
    do {
      iVar8 = iVar8 + -0x18;
    } while (iVar8 != -1);
    plVar11 = footprint;
    verts = plVar11;
    BoxBoxIntersect(&verts,&tAB,inR1,inR2,pMVar1->mScale);
    uVar2 = ((int)verts - (int)plVar11 >> 2) * -0x55555555;
    result->contactCount = 0;
    pMVar9 = result->contacts;
    result->normal[0] = normal.v[2] * pfVar4[8] + normal.v[0] * *pfVar4 + normal.v[1] * pfVar4[4];
    result->normal[1] = normal.v[2] * pfVar4[9] + normal.v[0] * pfVar4[1] + normal.v[1] * pfVar4[5];
    result->normal[2] = normal.v[2] * pfVar4[10] + normal.v[0] * pfVar4[2] + normal.v[1] * pfVar4[6]
    ;
    uVar12 = 0;
    if (uVar2 != 0) {
      do {
                    /* Unresolved local var: MeReal s@[DW_OP_reg15(ST4)] */
        fVar13 = (normal.v[2] * plVar11->v[2] +
                 plVar11->v[0] * normal.v[0] + normal.v[1] * plVar11->v[1]) - PN;
        if (fVar13 < separation * 0.01) {
          pMVar9->position[0] =
               plVar11->v[0] * *pfVar4 + plVar11->v[1] * pfVar4[4] + plVar11->v[2] * pfVar4[8] +
               pfVar4[0xc];
          pMVar9->position[1] =
               pfVar4[9] * plVar11->v[2] + pfVar4[5] * plVar11->v[1] + pfVar4[1] * plVar11->v[0] +
               pfVar4[0xd];
          pMVar9->position[2] =
               pfVar4[10] * plVar11->v[2] + pfVar4[6] * plVar11->v[1] + pfVar4[2] * plVar11->v[0] +
               pfVar4[0xe];
          pMVar9->separation = fVar13;
          pMVar9->dims = dims;
          pMVar9->normal[0] = result->normal[0];
          pMVar9->normal[1] = result->normal[1];
          pMVar9->normal[2] = result->normal[2];
          result->contactCount = result->contactCount + 1;
          pMVar9 = pMVar9 + 1;
        }
        uVar12 = uVar12 + 1;
        plVar11 = plVar11 + 1;
      } while (uVar12 < uVar2);
    }
    MVar10 = 1;
    result->touch = 1;
  }
  return MVar10;
}


/* ==== McdBoxBoxSafeTime ==== */

int McdBoxBoxSafeTime(McdModelPair *p,MeReal maxTime,McdSafeTimeResult *result)

{
  int iVar1;
  int iVar2;
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
  float *pfVar14;
  float *pfVar15;
  float *pfVar16;
  float *pfVar17;
  MeReal *pMVar18;
  MeReal *pMVar19;
  void *pvVar20;
  void *pvVar21;
  float *pfVar22;
  float *pfVar23;
  int iVar24;
  int iVar25;
  MeReal q0q2_1;
  MeReal q0q1_1;
  MeReal q0t2;
  MeReal q3sq_1;
  MeReal q1sq_1;
  MeReal q1q2;
  MeReal q0q2;
  MeReal q0q1;
  MeReal q3sq;
  MeReal q2sq;
  MeReal q1sq;
  int bIsect;
  MeReal DeltaTime;
  MeReal *ext1;
  MeReal *ext0;
  MeMatrix4Ptr tm1Ptr;
  MeMatrix4Ptr tm0Ptr;
  lsVec3 *W1;
  lsVec3 *W0;
  lsVec3 *V1;
  lsVec3 *V0;
  MeReal T;
  MeReal q1 [4];
  MeReal q0 [4];
  MeReal DqOverDt1 [4];
  MeReal DqOverDt0 [4];
  MeReal tm1i [4] [4];
  MeReal tm0i [4] [4];
  lsVec3 P;
  
                    /* Unresolved local var: McdBoxID geometry0@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdBoxID geometry1@[DW_OP_reg3(EBX)]
                       Unresolved local var: int NumSubIntervals@[DW_OP_reg7(EDI)]
                       Unresolved local var: uint ixt@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[???]
                       Unresolved local var: float __result@[???] */
  result->pair = p;
  result->time = maxTime;
  pfVar14 = McdModelGetLinearVelocityPtr(p->model1);
  pfVar15 = McdModelGetLinearVelocityPtr(p->model2);
  pfVar16 = McdModelGetAngularVelocityPtr(p->model1);
  pfVar17 = McdModelGetAngularVelocityPtr(p->model2);
  if ((((pfVar14 != (float *)0x0) && (pfVar15 != (float *)0x0)) && (pfVar16 != (float *)0x0)) &&
     (pfVar17 != (float *)0x0)) {
    pMVar18 = McdModelGetTransformPtr(p->model1);
    pMVar19 = McdModelGetTransformPtr(p->model2);
    pvVar20 = McdModelGetGeometry(p->model1);
    pvVar21 = McdModelGetGeometry(p->model2);
    pfVar22 = McdBoxGetRadii(pvVar20);
    pfVar23 = McdBoxGetRadii(pvVar21);
                    /* Unresolved local var: float __result@[???] */
    iVar2 = (int)ROUND(((SQRT(pfVar17[2] * pfVar17[2] +
                              pfVar17[1] * pfVar17[1] + *pfVar17 * *pfVar17) +
                        SQRT(pfVar16[2] * pfVar16[2] + pfVar16[1] * pfVar16[1] + *pfVar16 * *pfVar16
                            )) * maxTime) / McdBoxBoxSafeTime::maxAngle);
    iVar1 = iVar2 + 1;
    fVar3 = maxTime / (float)iVar1;
    tm0i[0][0] = *pMVar18;
    tm0i[0][1] = pMVar18[1];
    tm0i[0][2] = pMVar18[2];
    tm0i[0][3] = pMVar18[3];
    tm0i[1][0] = pMVar18[4];
    tm0i[1][1] = pMVar18[5];
    tm0i[1][2] = pMVar18[6];
    tm0i[1][3] = pMVar18[7];
    tm0i[2][0] = pMVar18[8];
    tm0i[2][1] = pMVar18[9];
    tm0i[2][2] = pMVar18[10];
    tm0i[2][3] = pMVar18[0xb];
    tm0i[3][0] = pMVar18[0xc];
    tm0i[3][1] = pMVar18[0xd];
    tm0i[3][2] = pMVar18[0xe];
    tm0i[3][3] = pMVar18[0xf];
    tm1i[0][0] = *pMVar19;
    tm1i[0][1] = pMVar19[1];
    tm1i[0][2] = pMVar19[2];
    tm1i[0][3] = pMVar19[3];
    tm1i[1][0] = pMVar19[4];
    tm1i[1][1] = pMVar19[5];
    tm1i[1][2] = pMVar19[6];
    tm1i[1][3] = pMVar19[7];
    tm1i[2][0] = pMVar19[8];
    tm1i[2][1] = pMVar19[9];
    tm1i[2][2] = pMVar19[10];
    tm1i[2][3] = pMVar19[0xb];
    tm1i[3][0] = pMVar19[0xc];
    tm1i[3][1] = pMVar19[0xd];
    tm1i[3][2] = pMVar19[0xe];
    tm1i[3][3] = pMVar19[0xf];
    MeQuaternionFromTM(q0,tm0i);
    iVar25 = 0;
    MeQuaternionFromTM(q1,tm1i);
    if (0 < iVar1) {
      do {
        iVar24 = MovingBoxBoxIntersect
                           (pfVar22,(lsTransform *)tm0i,(lsVec3 *)pfVar14,pfVar23,
                            (lsTransform *)tm1i,(lsVec3 *)pfVar15,fVar3,&T,(lsVec3 *)&P);
        if (iVar24 != 0) {
          result->time = (float)iVar25 * fVar3 + T + 1e-06;
          return 1;
        }
        if (iVar25 < iVar2) {
                    /* Unresolved local var: MeReal s0@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal s1@[DW_OP_reg11(ST0)] */
          fVar4 = ((-q0[1] * *pfVar16 - q0[2] * pfVar16[1]) - q0[3] * pfVar16[2]) * 0.5 * fVar3 +
                  q0[0];
          fVar7 = q1[0] + ((-q1[1] * *pfVar17 - q1[2] * pfVar17[1]) - q1[3] * pfVar17[2]) * 0.5 *
                          fVar3;
          fVar10 = q0[1] + fVar3 * ((q0[3] * pfVar16[1] + q0[0] * *pfVar16) - q0[2] * pfVar16[2]) *
                                   0.5;
          fVar11 = q1[1] + fVar3 * ((q1[3] * pfVar17[1] + q1[0] * *pfVar17) - q1[2] * pfVar17[2]) *
                                   0.5;
          fVar8 = q1[2] + (-q1[3] * *pfVar17 + q1[0] * pfVar17[1] + q1[1] * pfVar17[2]) * 0.5 *
                          fVar3;
          fVar6 = fVar3 * (q0[1] * pfVar16[2] + q0[0] * pfVar16[1] + -q0[3] * *pfVar16) * 0.5 +
                  q0[2];
          fVar12 = q0[3] + fVar3 * (q0[0] * pfVar16[2] + (q0[2] * *pfVar16 - q0[1] * pfVar16[1])) *
                                   0.5;
          fVar9 = q1[3] + ((q1[2] * *pfVar17 - q1[1] * pfVar17[1]) + q1[0] * pfVar17[2]) * 0.5 *
                          fVar3;
                    /* Unresolved local var: float __result@[DW_OP_reg16(ST5)] */
          fVar5 = 1.0 / SQRT(fVar12 * fVar12 + fVar6 * fVar6 + fVar10 * fVar10 + fVar4 * fVar4);
                    /* Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
          fVar13 = 1.0 / SQRT(fVar9 * fVar9 + fVar8 * fVar8 + fVar11 * fVar11 + fVar7 * fVar7);
          q1[0] = fVar7 * fVar13;
          q1[2] = fVar8 * fVar13;
          q0[2] = fVar6 * fVar5;
          q0[1] = fVar10 * fVar5;
          q0[0] = fVar4 * fVar5;
          q0[3] = fVar5 * fVar12;
          q1[1] = fVar11 * fVar13;
          q1[3] = fVar13 * fVar9;
                    /* Unresolved local var: MeReal q0sq@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q0t2@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal q1t2@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q0q3@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal q1q3@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q2q3@[DW_OP_reg14(ST3)] */
          fVar4 = q0[0] + q0[0];
          fVar10 = q0[3] * q0[3];
                    /* Unresolved local var: MeReal q0sq@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal q2sq@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q1t2@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q0q3@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q1q2@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q1q3@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q2q3@[DW_OP_reg18(ST7)] */
          fVar8 = q0[2] * q0[2];
          fVar7 = (q0[1] + q0[1]) * q0[3];
          fVar5 = (q0[1] + q0[1]) * q0[2];
          fVar6 = (q0[2] + q0[2]) * q0[3];
          tm0i[0][0] = ((q0[1] * q0[1] + q0[0] * q0[0]) - fVar8) - fVar10;
          fVar9 = q0[0] * q0[0] - q0[1] * q0[1];
          tm0i[1][0] = fVar5 - fVar4 * q0[3];
          tm0i[0][1] = fVar4 * q0[3] + fVar5;
          tm0i[2][0] = fVar4 * q0[2] + fVar7;
          tm0i[0][2] = fVar7 - fVar4 * q0[2];
          tm0i[1][1] = (fVar8 + fVar9) - fVar10;
          tm0i[2][2] = (fVar9 - fVar8) + fVar10;
          tm0i[2][1] = fVar6 - q0[1] * fVar4;
          tm0i[1][2] = fVar6 + q0[1] * fVar4;
          fVar9 = q1[0] + q1[0];
          fVar10 = q1[3] * q1[3];
          fVar4 = q1[2] * q1[2];
          fVar5 = (q1[1] + q1[1]) * q1[2];
          fVar7 = q1[0] * q1[0] - q1[1] * q1[1];
          tm1i[0][0] = ((q1[1] * q1[1] + q1[0] * q1[0]) - fVar4) - fVar10;
          fVar6 = (q1[1] + q1[1]) * q1[3];
          fVar8 = (q1[2] + q1[2]) * q1[3];
          tm1i[0][1] = fVar5 + fVar9 * q1[3];
          tm1i[1][0] = fVar5 - fVar9 * q1[3];
          tm1i[2][0] = fVar9 * q1[2] + fVar6;
          tm1i[0][2] = fVar6 - fVar9 * q1[2];
          tm1i[1][1] = (fVar7 + fVar4) - fVar10;
          tm1i[2][2] = (fVar7 - fVar4) + fVar10;
          tm1i[2][1] = fVar8 - fVar9 * q1[1];
          tm1i[1][2] = fVar8 + fVar9 * q1[1];
          tm0i[3][0] = fVar3 * *pfVar14 + tm0i[3][0];
          tm1i[3][0] = fVar3 * *pfVar15 + tm1i[3][0];
          tm0i[3][1] = fVar3 * pfVar14[1] + tm0i[3][1];
          tm1i[3][1] = fVar3 * pfVar15[1] + tm1i[3][1];
          tm0i[3][2] = fVar3 * pfVar14[2] + tm0i[3][2];
          tm1i[3][2] = fVar3 * pfVar15[2] + tm1i[3][2];
        }
        iVar25 = iVar25 + 1;
      } while (iVar25 < iVar1);
    }
  }
  return 0;
}


/* ==== McdBoxBoxRegisterInteraction ==== */

MeBool McdBoxBoxRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdBoxBoxIntersect;
  interactions.safetimeFn = McdBoxBoxSafeTime;
  interactions.cull = 1;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,2,2,&interactions);
  return 1;
}


/* ==== McdVanillaOverlapOBBs ==== */

/* WARNING: Removing unreachable block (ram,0x0001186d) */
/* WARNING: Type propagation algorithm not settling */

bool McdVanillaOverlapOBBs
               (MeReal *outSep,lsVec3 *outN,MeReal *outPN,MeI16 *outDims,MeReal inEps,lsVec3 *inR1,
               lsVec3 *inR2,lsTransform *inT12)

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
  uint uVar10;
  MeReal MVar11;
  bool bVar12;
  byte bVar13;
  ushort uVar14;
  uint uVar15;
  lsTransformRow *plVar16;
  uint uVar17;
  int iVar18;
  int iVar19;
  lsTransformRow *plVar20;
  uint uVar21;
  sbyte local_d8;
  int j;
  MeReal nRLen;
  MeU8 normInfo;
  MeReal saveNormD;
  MeReal PN;
  MeReal maxSeparation;
  lsVec3 outVec;
  lsTransform arot;
  
                    /* Unresolved local var: MeReal eps2@[???]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: lsVec3 * pos@[???]
                       Unresolved local var: MeReal threshold@[DW_OP_reg17(ST6)]
                       Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
                    /* Unresolved local var: MeReal sumR@[???]
                       Unresolved local var: MeReal normD@[???]
                       Unresolved local var: MeReal separation@[???] */
  plVar16 = inT12->row + 3;
  maxSeparation = -3.4028235e+38;
  saveNormD = 1.0;
  nRLen = 1.0;
  normInfo = '\x0f';
  arot.row[0].v.v[0] = ABS(inT12->row[0].v.v[0]);
  arot.row[0].v.v[1] = ABS(inT12->row[0].v.v[1]);
  arot.row[0].v.v[2] = ABS(inT12->row[0].v.v[2]);
  arot.row[1].v.v[0] = ABS(inT12->row[1].v.v[0]);
  arot.row[1].v.v[1] = ABS(inT12->row[1].v.v[1]);
  arot.row[1].v.v[2] = ABS(inT12->row[1].v.v[2]);
  arot.row[2].v.v[0] = ABS(inT12->row[2].v.v[0]);
  arot.row[2].v.v[1] = ABS(inT12->row[2].v.v[1]);
  arot.row[2].v.v[2] = ABS(inT12->row[2].v.v[2]);
  fVar1 = inR2->v[0];
  fVar2 = inR2->v[1];
  fVar3 = inR2->v[2];
  fVar6 = inT12->row[3].v.v[0];
  fVar4 = ABS(fVar6) -
          (arot.row[0].v.v[0] * fVar1 + inR1->v[0] + arot.row[1].v.v[0] * fVar2 +
          arot.row[2].v.v[0] * fVar3);
  if (fVar4 <= -3.4028235e+38) {
LAB_000110bd:
    fVar6 = inT12->row[3].v.v[1];
    fVar4 = ABS(fVar6) -
            (fVar3 * arot.row[2].v.v[1] +
            fVar2 * arot.row[1].v.v[1] + fVar1 * arot.row[0].v.v[1] + inR1->v[1]);
    if (maxSeparation < fVar4) {
      PN = -inR1->v[1] - fVar4;
      normInfo = '\r';
      saveNormD = fVar6;
      maxSeparation = fVar4;
      if (inEps < fVar4) goto LAB_00011840;
    }
    fVar6 = inT12->row[3].v.v[2];
    fVar1 = ABS(fVar6) -
            (fVar1 * arot.row[0].v.v[2] + inR1->v[2] + fVar2 * arot.row[1].v.v[2] +
            fVar3 * arot.row[2].v.v[2]);
    if (maxSeparation < fVar1) {
      PN = -inR1->v[2] - fVar1;
      normInfo = '\x0e';
      saveNormD = fVar6;
      maxSeparation = fVar1;
      if (inEps < fVar1) goto LAB_00011840;
    }
                    /* Unresolved local var: MeReal sumR@[???]
                       Unresolved local var: MeReal normD@[???]
                       Unresolved local var: MeReal aNormD@[???]
                       Unresolved local var: MeReal separation@[???] */
    fVar1 = inR1->v[0];
    fVar2 = inR1->v[1];
    fVar3 = inR1->v[2];
    fVar6 = (plVar16->v).v[0];
    fVar4 = inT12->row[3].v.v[1];
    fVar5 = inT12->row[3].v.v[2];
    fVar9 = fVar5 * inT12->row[0].v.v[2] +
            fVar4 * inT12->row[0].v.v[1] + fVar6 * inT12->row[0].v.v[0];
    fVar8 = ABS(fVar9);
    fVar7 = fVar8 - (fVar1 * arot.row[0].v.v[0] + inR2->v[0] + fVar2 * arot.row[0].v.v[1] +
                    fVar3 * arot.row[0].v.v[2]);
    if (maxSeparation < fVar7) {
      PN = inR2->v[0] - fVar8;
      normInfo = '\x03';
      saveNormD = fVar9;
      maxSeparation = fVar7;
      if (inEps < fVar7) goto LAB_00011840;
    }
    iVar18 = 1;
    do {
      plVar20 = inT12->row + iVar18;
      fVar7 = fVar5 * (plVar20->v).v[2] + fVar4 * (plVar20->v).v[1] + fVar6 * (plVar20->v).v[0];
      fVar8 = ABS(fVar7);
      fVar9 = fVar8 - (fVar3 * arot.row[iVar18].v.v[2] +
                      fVar1 * arot.row[iVar18].v.v[0] + inR2->v[iVar18] +
                      fVar2 * arot.row[iVar18].v.v[1]);
      if (maxSeparation < fVar9) {
        PN = inR2->v[iVar18] - fVar8;
        normInfo = (char)iVar18 * '\x04' | 3;
        saveNormD = fVar7;
        maxSeparation = fVar9;
        if (inEps < fVar9) goto LAB_00011840;
      }
      plVar20 = inT12->row + iVar18 + 1;
      fVar7 = fVar5 * (plVar20->v).v[2] + fVar4 * (plVar20->v).v[1] + fVar6 * (plVar20->v).v[0];
      fVar8 = ABS(fVar7);
      fVar9 = fVar8 - (fVar1 * arot.row[iVar18 + 1].v.v[0] + inR2->v[iVar18 + 1] +
                       fVar2 * arot.row[iVar18 + 1].v.v[1] + fVar3 * arot.row[iVar18 + 1].v.v[2]);
      if (maxSeparation < fVar9) {
        PN = inR2->v[iVar18 + 1] - fVar8;
        normInfo = (char)iVar18 * '\x04' + 4U | 3;
        saveNormD = fVar7;
        maxSeparation = fVar9;
        if (inEps < fVar9) goto LAB_00011840;
      }
      iVar18 = iVar18 + 2;
    } while (iVar18 < 3);
    j = 0;
    fVar1 = maxSeparation + inEps;
    do {
                    /* Unresolved local var: int j1@[???]
                       Unresolved local var: int j2@[???]
                       Unresolved local var: lsVec3 * aj@[???]
                       Unresolved local var: lsVec3 * aj1@[???]
                       Unresolved local var: lsVec3 * aj2@[???]
                       Unresolved local var: lsVec3 * tj@[???] */
      uVar17 = 1 << ((byte)j & 0x1f) & 3;
      local_d8 = (sbyte)uVar17;
      uVar15 = 1 << local_d8 & 3;
      iVar19 = 0;
      iVar18 = 0;
      do {
                    /* Unresolved local var: MeReal rLen@[DW_OP_reg13(ST2)] */
        fVar2 = *(float *)((int)inT12->row[j].v.v + iVar18);
        fVar2 = 1.0 - fVar2 * fVar2;
        if (1e-06 < fVar2) {
                    /* Unresolved local var: int i1@[???]
                       Unresolved local var: int i2@[???]
                       Unresolved local var: MeReal rA@[???]
                       Unresolved local var: MeReal rB@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal normD@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal aNormD@[???]
                       Unresolved local var: MeReal separation@[DW_OP_reg12(ST1)] */
          uVar21 = 1 << ((byte)iVar19 & 0x1f) & 3;
          local_d8 = (sbyte)uVar21;
          uVar10 = 1 << local_d8 & 3;
          fVar4 = *(float *)((int)arot.row[uVar17].v.v + iVar18) * inR2->v[uVar15] +
                  *(float *)((int)arot.row[uVar15].v.v + iVar18) * inR2->v[uVar17];
          fVar5 = inT12->row[j].v.v[uVar21] * (plVar16->v).v[uVar10] -
                  inT12->row[j].v.v[uVar10] * (plVar16->v).v[uVar21];
          fVar3 = ABS(fVar5);
          fVar6 = (fVar3 - (arot.row[j].v.v[uVar10] * inR1->v[uVar21] +
                           arot.row[j].v.v[uVar21] * inR1->v[uVar10])) - fVar4;
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
          fVar2 = SQRT(fVar2);
          if (inEps * fVar2 < fVar6) {
            return false;
          }
          if (fVar1 * fVar2 < fVar6) {
            nRLen = 1.0 / fVar2;
            normInfo = (byte)(j << 2) | (byte)iVar19;
            fVar1 = fVar6 * nRLen;
            PN = nRLen * (fVar4 - fVar3);
            saveNormD = fVar5;
            maxSeparation = fVar1;
          }
        }
        iVar19 = iVar19 + 1;
        iVar18 = iVar18 + 4;
      } while (iVar19 < 3);
      j = j + 1;
    } while (j < 3);
    *outSep = maxSeparation;
    *outPN = PN;
    bVar13 = normInfo & 0xc;
    if (bVar13 == 0xc) {
                    /* Unresolved local var: MeI8 axis@[???]
                       Unresolved local var: MeI16 dimB@[DW_OP_reg3(EBX)] */
      uVar15 = normInfo & 3;
      uVar14 = (ushort)(arot.row[0].v.v[uVar15] < 0.0001);
      if (arot.row[1].v.v[uVar15] < 0.0001) {
        uVar14 = uVar14 + 1;
      }
      if (arot.row[2].v.v[uVar15] < 0.0001) {
        uVar14 = uVar14 + 1;
      }
      MVar11 = 1.0;
      outN->v[0] = 0.0;
      outN->v[1] = 0.0;
      outN->v[2] = 0.0;
      if (0.0 <= saveNormD) {
        MVar11 = -1.0;
      }
      outN->v[uVar15] = MVar11;
      *outDims = uVar14 << 8 | 2;
    }
    else if ((normInfo & 3) == 3) {
                    /* Unresolved local var: MeI8 axis@[???]
                       Unresolved local var: MeI16 dimA@[DW_OP_reg3(EBX)] */
      bVar13 = bVar13 >> 2;
      uVar14 = (ushort)(arot.row[(char)bVar13].v.v[0] < 0.0001);
      if (arot.row[(char)bVar13].v.v[1] < 0.0001) {
        uVar14 = uVar14 + 1;
      }
      if (arot.row[(char)bVar13].v.v[2] < 0.0001) {
        uVar14 = uVar14 + 1;
      }
      if (saveNormD <= 0.0) {
        plVar16 = inT12->row + (char)bVar13;
        outVec.v[0] = (plVar16->v).v[0];
        outVec.v[1] = (plVar16->v).v[1];
        MVar11 = (plVar16->v).v[2];
      }
      else {
        plVar16 = inT12->row + (char)bVar13;
        outVec.v[0] = -(plVar16->v).v[0];
        outVec.v[1] = -(plVar16->v).v[1];
        MVar11 = -(plVar16->v).v[2];
      }
      outN->v[0] = outVec.v[0];
      outN->v[1] = outVec.v[1];
      outN->v[2] = MVar11;
      *outDims = uVar14 | 0x200;
    }
    else {
      if (0.0 <= saveNormD) {
        uVar17 = 1 << (sbyte)(normInfo & 3) & 3;
        outVec.v[normInfo & 3] = 0.0;
        uVar15 = 1 << (sbyte)uVar17 & 3;
        outVec.v[uVar17] = inT12->row[0].v.v[(normInfo & 0xc) + uVar15];
        MVar11 = -inT12->row[0].v.v[(normInfo & 0xc) + uVar17];
      }
      else {
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
        uVar17 = 1 << (normInfo & 3) & 3;
        uVar15 = 1 << (sbyte)uVar17 & 3;
        outVec.v[normInfo & 0xffffff03] = 0.0;
        outVec.v[uVar17] = -inT12->row[bVar13 >> 2].v.v[uVar15];
        MVar11 = inT12->row[bVar13 >> 2].v.v[uVar17];
      }
      outVec.v[uVar15] = MVar11;
      outN->v[0] = outVec.v[0];
      outN->v[1] = outVec.v[1];
      outN->v[0] = outVec.v[0] * nRLen;
      outN->v[1] = outVec.v[1] * nRLen;
      outN->v[2] = outVec.v[2] * nRLen;
      *outDims = 0x101;
    }
    bVar12 = true;
  }
  else {
    PN = -inR1->v[0] - fVar4;
    normInfo = '\f';
    saveNormD = fVar6;
    maxSeparation = fVar4;
    if (fVar4 <= inEps) goto LAB_000110bd;
LAB_00011840:
    bVar12 = false;
  }
  return bVar12;
}


/* ==== BoxBoxIntersect ==== */

void BoxBoxIntersect(lsVec3 **outList,lsTransform *tAB,lsVec3 *inRA,lsVec3 *inRB,MeReal scale)

{
  lsVec3 *plVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  lsVec3 *plVar5;
  lsVec3 *bVert;
  lsVec3 bVertList [24];
  lsTransform tBA;
  
  McdVanillaBoxEdgeBoxIntersect(outList,tAB,inRA,inRB,scale);
  tBA.row[0].v.v[0] = tAB->row[0].v.v[0];
                    /* Unresolved local var: lsVec3 * vert@[DW_OP_reg6(ESI)] */
  plVar5 = bVertList;
  tBA.row[0].v.v[1] = tAB->row[1].v.v[0];
  tBA.row[0].v.v[2] = tAB->row[2].v.v[0];
  tBA.row[1].v.v[0] = tAB->row[0].v.v[1];
  tBA.row[1].v.v[1] = tAB->row[1].v.v[1];
  tBA.row[1].v.v[2] = tAB->row[2].v.v[1];
  tBA.row[2].v.v[0] = tAB->row[0].v.v[2];
  tBA.row[2].v.v[1] = tAB->row[1].v.v[2];
  tBA.row[2].v.v[2] = tAB->row[2].v.v[2];
  fVar2 = -tAB->row[3].v.v[1];
  fVar4 = -tAB->row[3].v.v[0];
  fVar3 = -tAB->row[3].v.v[2];
  tBA.row[3].v.v[0] =
       tBA.row[0].v.v[0] * fVar4 + tBA.row[1].v.v[0] * fVar2 + tBA.row[2].v.v[0] * fVar3;
  tBA.row[3].v.v[1] =
       tBA.row[1].v.v[1] * fVar2 + tBA.row[0].v.v[1] * fVar4 + tBA.row[2].v.v[1] * fVar3;
  tBA.row[3].v.v[2] =
       fVar3 * tAB->row[2].v.v[2] + fVar4 * tAB->row[2].v.v[0] + fVar2 * tAB->row[2].v.v[1];
  bVert = plVar5;
  McdVanillaBoxEdgeBoxIntersect(&bVert,&tBA,inRB,inRA,scale);
  for (; plVar5 != bVert; plVar5 = plVar5 + 1) {
    plVar1 = *outList;
    *outList = plVar1 + 1;
    plVar1->v[0] = tAB->row[2].v.v[0] * plVar5->v[2] +
                   tAB->row[1].v.v[0] * plVar5->v[1] + tAB->row[0].v.v[0] * plVar5->v[0] +
                   tAB->row[3].v.v[0];
    plVar1->v[1] = tAB->row[2].v.v[1] * plVar5->v[2] +
                   tAB->row[1].v.v[1] * plVar5->v[1] + tAB->row[0].v.v[1] * plVar5->v[0] +
                   tAB->row[3].v.v[1];
    plVar1->v[2] = tAB->row[2].v.v[2] * plVar5->v[2] +
                   tAB->row[1].v.v[2] * plVar5->v[1] + tAB->row[0].v.v[2] * plVar5->v[0] +
                   tAB->row[3].v.v[2];
  }
  return;
}


/* ==== McdVanillaBoxEdgeBoxIntersect ==== */

void McdVanillaBoxEdgeBoxIntersect
               (lsVec3 **outList,lsTransform *tAB,lsVec3 *inRA,lsVec3 *inRB,MeReal scale)

{
  lsTransformRow *plVar1;
  float fVar2;
  lsVec3 *plVar3;
  int iVar4;
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
  lsVec3 x;
  lsVec3 invD2;
  lsVec3 invD1;
  lsVec3 invD0;
  lsVec3 d2;
  lsVec3 d1;
  lsVec3 d0;
  
  fVar2 = inRB->v[0];
  d0.v[0] = tAB->row[0].v.v[0] * fVar2;
  d0.v[1] = tAB->row[0].v.v[1] * fVar2;
  d0.v[2] = fVar2 * tAB->row[0].v.v[2];
  fVar2 = inRB->v[1];
  d1.v[1] = tAB->row[1].v.v[1] * fVar2;
  d1.v[2] = tAB->row[1].v.v[2] * fVar2;
  d1.v[0] = fVar2 * tAB->row[1].v.v[0];
  fVar2 = inRB->v[2];
  d2.v[0] = tAB->row[2].v.v[0] * fVar2;
  d2.v[1] = tAB->row[2].v.v[1] * fVar2;
  d2.v[2] = fVar2 * tAB->row[2].v.v[2];
  invD0.v[0] = 0.0;
  if (d0.v[0] != 0.0) {
    invD0.v[0] = 1.0 / d0.v[0];
  }
  invD0.v[1] = 0.0;
  if (d0.v[1] != 0.0) {
    invD0.v[1] = 1.0 / d0.v[1];
  }
  invD0.v[2] = 0.0;
  if (d0.v[2] != 0.0) {
    invD0.v[2] = 1.0 / d0.v[2];
  }
  invD1.v[0] = 0.0;
  if (d1.v[0] != 0.0) {
    invD1.v[0] = 1.0 / d1.v[0];
  }
  invD1.v[1] = 0.0;
  if (d1.v[1] != 0.0) {
    invD1.v[1] = 1.0 / d1.v[1];
  }
  invD1.v[2] = 0.0;
  if (d1.v[2] != 0.0) {
    invD1.v[2] = 1.0 / d1.v[2];
  }
  invD2.v[0] = 0.0;
  if (d2.v[0] != 0.0) {
    invD2.v[0] = 1.0 / d2.v[0];
  }
  invD2.v[1] = 0.0;
  if (d2.v[1] != 0.0) {
    invD2.v[1] = 1.0 / d2.v[1];
  }
  invD2.v[2] = 0.0;
  if (d2.v[2] != 0.0) {
    invD2.v[2] = 1.0 / d2.v[2];
  }
                    /* Unresolved local var: lsVec3 * t@[???]
                       Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  plVar1 = tAB->row + 3;
  x.v[0] = d1.v[0] + tAB->row[3].v.v[0] + d2.v[0];
  x.v[1] = d2.v[1] + d1.v[1] + tAB->row[3].v.v[1];
  x.v[2] = d1.v[2] + tAB->row[3].v.v[2] + d2.v[2];
  tIn = -1.0;
  tOut = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn,&tOut,&x,&d0,&invD0,inRA,scale * 1e-06);
  if (iVar4 != 0) {
    if ((-1.0 < tIn) && (tIn < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tIn * d0.v[0] + x.v[0];
      plVar3->v[1] = tIn * d0.v[1] + x.v[1];
      plVar3->v[2] = tIn * d0.v[2] + x.v[2];
      *outList = *outList + 1;
    }
    if ((-1.0 < tOut) && (tOut < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tOut * d0.v[0] + x.v[0];
      plVar3->v[1] = tOut * d0.v[1] + x.v[1];
      plVar3->v[2] = tOut * d0.v[2] + x.v[2];
      *outList = *outList + 1;
    }
  }
  x.v[2] = (tAB->row[3].v.v[2] - d1.v[2]) + d2.v[2];
                    /* Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  x.v[0] = ((plVar1->v).v[0] - d1.v[0]) + d2.v[0];
  x.v[1] = (tAB->row[3].v.v[1] - d1.v[1]) + d2.v[1];
  tIn_1 = -1.0;
  tOut_1 = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn_1,&tOut_1,&x,&d0,&invD0,inRA,scale * 1e-06);
  if (iVar4 != 0) {
    if ((-1.0 < tIn_1) && (tIn_1 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tIn_1 * d0.v[0] + x.v[0];
      plVar3->v[1] = tIn_1 * d0.v[1] + x.v[1];
      plVar3->v[2] = tIn_1 * d0.v[2] + x.v[2];
      *outList = *outList + 1;
    }
    if ((-1.0 < tOut_1) && (tOut_1 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tOut_1 * d0.v[0] + x.v[0];
      plVar3->v[1] = tOut_1 * d0.v[1] + x.v[1];
      plVar3->v[2] = tOut_1 * d0.v[2] + x.v[2];
      *outList = *outList + 1;
    }
  }
  x.v[2] = (d1.v[2] + tAB->row[3].v.v[2]) - d2.v[2];
                    /* Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  x.v[0] = (d1.v[0] + (plVar1->v).v[0]) - d2.v[0];
  x.v[1] = (d1.v[1] + tAB->row[3].v.v[1]) - d2.v[1];
  tIn_2 = -1.0;
  tOut_2 = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn_2,&tOut_2,&x,&d0,&invD0,inRA,scale * 1e-06);
  if (iVar4 != 0) {
    if ((-1.0 < tIn_2) && (tIn_2 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tIn_2 * d0.v[0] + x.v[0];
      plVar3->v[1] = tIn_2 * d0.v[1] + x.v[1];
      plVar3->v[2] = tIn_2 * d0.v[2] + x.v[2];
      *outList = *outList + 1;
    }
    if ((-1.0 < tOut_2) && (tOut_2 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tOut_2 * d0.v[0] + x.v[0];
      plVar3->v[1] = tOut_2 * d0.v[1] + x.v[1];
      plVar3->v[2] = tOut_2 * d0.v[2] + x.v[2];
      *outList = *outList + 1;
    }
  }
  x.v[2] = (tAB->row[3].v.v[2] - d1.v[2]) - d2.v[2];
                    /* Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  x.v[0] = ((plVar1->v).v[0] - d1.v[0]) - d2.v[0];
  x.v[1] = (tAB->row[3].v.v[1] - d1.v[1]) - d2.v[1];
  tIn_3 = -1.0;
  tOut_3 = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn_3,&tOut_3,&x,&d0,&invD0,inRA,scale * 1e-06);
  if (iVar4 != 0) {
    if ((-1.0 < tIn_3) && (tIn_3 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tIn_3 * d0.v[0] + x.v[0];
      plVar3->v[1] = tIn_3 * d0.v[1] + x.v[1];
      plVar3->v[2] = tIn_3 * d0.v[2] + x.v[2];
      *outList = *outList + 1;
    }
    if ((-1.0 < tOut_3) && (tOut_3 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tOut_3 * d0.v[0] + x.v[0];
      plVar3->v[1] = tOut_3 * d0.v[1] + x.v[1];
      plVar3->v[2] = tOut_3 * d0.v[2] + x.v[2];
      *outList = *outList + 1;
    }
  }
  x.v[2] = d0.v[2] + tAB->row[3].v.v[2] + d2.v[2];
                    /* Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  x.v[0] = d0.v[0] + (plVar1->v).v[0] + d2.v[0];
  x.v[1] = d0.v[1] + tAB->row[3].v.v[1] + d2.v[1];
  tIn_4 = -1.0;
  tOut_4 = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn_4,&tOut_4,&x,&d1,&invD1,inRA,scale * 1e-06);
  if (iVar4 != 0) {
    if ((-1.0 < tIn_4) && (tIn_4 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tIn_4 * d1.v[0] + x.v[0];
      plVar3->v[1] = tIn_4 * d1.v[1] + x.v[1];
      plVar3->v[2] = tIn_4 * d1.v[2] + x.v[2];
      *outList = *outList + 1;
    }
    if ((-1.0 < tOut_4) && (tOut_4 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tOut_4 * d1.v[0] + x.v[0];
      plVar3->v[1] = tOut_4 * d1.v[1] + x.v[1];
      plVar3->v[2] = tOut_4 * d1.v[2] + x.v[2];
      *outList = *outList + 1;
    }
  }
  x.v[2] = (tAB->row[3].v.v[2] - d0.v[2]) + d2.v[2];
                    /* Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  x.v[0] = ((plVar1->v).v[0] - d0.v[0]) + d2.v[0];
  x.v[1] = (tAB->row[3].v.v[1] - d0.v[1]) + d2.v[1];
  tIn_5 = -1.0;
  tOut_5 = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn_5,&tOut_5,&x,&d1,&invD1,inRA,scale * 1e-06);
  if (iVar4 != 0) {
    if ((-1.0 < tIn_5) && (tIn_5 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tIn_5 * d1.v[0] + x.v[0];
      plVar3->v[1] = tIn_5 * d1.v[1] + x.v[1];
      plVar3->v[2] = tIn_5 * d1.v[2] + x.v[2];
      *outList = *outList + 1;
    }
    if ((-1.0 < tOut_5) && (tOut_5 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tOut_5 * d1.v[0] + x.v[0];
      plVar3->v[1] = tOut_5 * d1.v[1] + x.v[1];
      plVar3->v[2] = tOut_5 * d1.v[2] + x.v[2];
      *outList = *outList + 1;
    }
  }
  x.v[2] = (d0.v[2] + tAB->row[3].v.v[2]) - d2.v[2];
                    /* Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  x.v[0] = (d0.v[0] + (plVar1->v).v[0]) - d2.v[0];
  x.v[1] = (d0.v[1] + tAB->row[3].v.v[1]) - d2.v[1];
  tIn_6 = -1.0;
  tOut_6 = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn_6,&tOut_6,&x,&d1,&invD1,inRA,scale * 1e-06);
  if (iVar4 != 0) {
    if ((-1.0 < tIn_6) && (tIn_6 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tIn_6 * d1.v[0] + x.v[0];
      plVar3->v[1] = tIn_6 * d1.v[1] + x.v[1];
      plVar3->v[2] = tIn_6 * d1.v[2] + x.v[2];
      *outList = *outList + 1;
    }
    if ((-1.0 < tOut_6) && (tOut_6 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tOut_6 * d1.v[0] + x.v[0];
      plVar3->v[1] = tOut_6 * d1.v[1] + x.v[1];
      plVar3->v[2] = tOut_6 * d1.v[2] + x.v[2];
      *outList = *outList + 1;
    }
  }
  x.v[2] = (tAB->row[3].v.v[2] - d0.v[2]) - d2.v[2];
                    /* Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  x.v[0] = ((plVar1->v).v[0] - d0.v[0]) - d2.v[0];
  x.v[1] = (tAB->row[3].v.v[1] - d0.v[1]) - d2.v[1];
  tIn_7 = -1.0;
  tOut_7 = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn_7,&tOut_7,&x,&d1,&invD1,inRA,scale * 1e-06);
  if (iVar4 != 0) {
    if ((-1.0 < tIn_7) && (tIn_7 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tIn_7 * d1.v[0] + x.v[0];
      plVar3->v[1] = tIn_7 * d1.v[1] + x.v[1];
      plVar3->v[2] = tIn_7 * d1.v[2] + x.v[2];
      *outList = *outList + 1;
    }
    if ((-1.0 < tOut_7) && (tOut_7 < 1.0)) {
      plVar3 = *outList;
      plVar3->v[0] = tOut_7 * d1.v[0] + x.v[0];
      plVar3->v[1] = tOut_7 * d1.v[1] + x.v[1];
      plVar3->v[2] = tOut_7 * d1.v[2] + x.v[2];
      *outList = *outList + 1;
    }
  }
  x.v[2] = d0.v[2] + tAB->row[3].v.v[2] + d1.v[2];
                    /* Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  x.v[0] = d0.v[0] + (plVar1->v).v[0] + d1.v[0];
  fVar2 = scale * 1e-06;
  x.v[1] = d0.v[1] + tAB->row[3].v.v[1] + d1.v[1];
  tIn_8 = -1.0;
  tOut_8 = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn_8,&tOut_8,&x,&d2,&invD2,inRA,fVar2);
  if (iVar4 != 0) {
    plVar3 = *outList;
    plVar3->v[0] = tIn_8 * d2.v[0] + x.v[0];
    plVar3->v[1] = tIn_8 * d2.v[1] + x.v[1];
    plVar3->v[2] = tIn_8 * d2.v[2] + x.v[2];
    plVar3 = *outList;
    *outList = plVar3 + 1;
    plVar3[1].v[0] = tOut_8 * d2.v[0] + x.v[0];
    plVar3[1].v[1] = tOut_8 * d2.v[1] + x.v[1];
    plVar3[1].v[2] = tOut_8 * d2.v[2] + x.v[2];
    *outList = *outList + 1;
  }
                    /* Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  x.v[0] = ((plVar1->v).v[0] - d0.v[0]) + d1.v[0];
  x.v[1] = (tAB->row[3].v.v[1] - d0.v[1]) + d1.v[1];
  x.v[2] = (tAB->row[3].v.v[2] - d0.v[2]) + d1.v[2];
  tIn_9 = -1.0;
  tOut_9 = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn_9,&tOut_9,&x,&d2,&invD2,inRA,fVar2);
  if (iVar4 != 0) {
    plVar3 = *outList;
    plVar3->v[0] = tIn_9 * d2.v[0] + x.v[0];
    plVar3->v[1] = tIn_9 * d2.v[1] + x.v[1];
    plVar3->v[2] = tIn_9 * d2.v[2] + x.v[2];
    plVar3 = *outList;
    *outList = plVar3 + 1;
    plVar3[1].v[0] = tOut_9 * d2.v[0] + x.v[0];
    plVar3[1].v[1] = tOut_9 * d2.v[1] + x.v[1];
    plVar3[1].v[2] = tOut_9 * d2.v[2] + x.v[2];
    *outList = *outList + 1;
  }
                    /* Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  x.v[0] = (d0.v[0] + (plVar1->v).v[0]) - d1.v[0];
  x.v[1] = (d0.v[1] + tAB->row[3].v.v[1]) - d1.v[1];
  x.v[2] = (d0.v[2] + tAB->row[3].v.v[2]) - d1.v[2];
  tIn_10 = -1.0;
  tOut_10 = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn_10,&tOut_10,&x,&d2,&invD2,inRA,fVar2);
  if (iVar4 != 0) {
    plVar3 = *outList;
    plVar3->v[0] = tIn_10 * d2.v[0] + x.v[0];
    plVar3->v[1] = tIn_10 * d2.v[1] + x.v[1];
    plVar3->v[2] = tIn_10 * d2.v[2] + x.v[2];
    plVar3 = *outList;
    *outList = plVar3 + 1;
    plVar3[1].v[0] = tOut_10 * d2.v[0] + x.v[0];
    plVar3[1].v[1] = tOut_10 * d2.v[1] + x.v[1];
    plVar3[1].v[2] = tOut_10 * d2.v[2] + x.v[2];
    *outList = *outList + 1;
  }
                    /* Unresolved local var: MeBool cubeIntersected@[DW_OP_reg0(EAX)] */
  x.v[0] = ((plVar1->v).v[0] - d0.v[0]) - d1.v[0];
  x.v[1] = (tAB->row[3].v.v[1] - d0.v[1]) - d1.v[1];
  x.v[2] = (tAB->row[3].v.v[2] - d0.v[2]) - d1.v[2];
  tIn_11 = -1.0;
  tOut_11 = 1.0;
  iVar4 = McdVanillaSegmentCubeIntersect(&tIn_11,&tOut_11,&x,&d2,&invD2,inRA,fVar2);
  if (iVar4 != 0) {
    plVar3 = *outList;
    plVar3->v[0] = tIn_11 * d2.v[0] + x.v[0];
    plVar3->v[1] = tIn_11 * d2.v[1] + x.v[1];
    plVar3->v[2] = tIn_11 * d2.v[2] + x.v[2];
    plVar3 = *outList;
    *outList = plVar3 + 1;
    plVar3[1].v[0] = tOut_11 * d2.v[0] + x.v[0];
    plVar3[1].v[1] = tOut_11 * d2.v[1] + x.v[1];
    plVar3[1].v[2] = tOut_11 * d2.v[2] + x.v[2];
    *outList = *outList + 1;
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
  
                    /* Unresolved local var: MeReal tIn@[???]
                       Unresolved local var: MeReal tOut@[DW_OP_reg11(ST0)]
                       Unresolved local var: int j@[???] */
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


/* ==== _GLOBAL__I_McdBoxBoxIntersect ==== */

void _GLOBAL__I_McdBoxBoxIntersect(void)

{
  __static_initialization_and_destruction_0(1,0xffff);
  return;
}


