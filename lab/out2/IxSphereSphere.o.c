/* ==== McdSphereSphereIntersect ==== */

int McdSphereSphereIntersect(McdModelPair *p,McdIntersectResult *result)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  McdContact *pMVar5;
  int iVar6;
  int iVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  int iVar10;
  longdouble lVar11;
  longdouble lVar12;
  longdouble lVar13;
  longdouble lVar14;
  longdouble lVar15;
  longdouble lVar16;
  longdouble lVar17;
  longdouble lVar18;
  longdouble lVar19;
  longdouble lVar20;
  longdouble lVar21;
  MeReal rA;
  MeReal eps;
  lsTransform *tA;
  lsVec3 normal;
  
                    /* Unresolved local var: lsTransform * tB@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdSphereID geometry1@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdSphereID geometry2@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal rB@[DW_OP_reg16(ST5)]
                       Unresolved local var: lsVec3 * pA@[???]
                       Unresolved local var: lsVec3 * pB@[???]
                       Unresolved local var: MeReal sumR@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal lenN@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal separation@[DW_OP_reg13(ST2)] */
  iVar6 = McdModelGetTransformPtr(p->model1);
  iVar7 = McdModelGetTransformPtr(p->model2);
  lVar11 = (longdouble)McdModelGetContactTolerance(p->model1);
  lVar12 = (longdouble)McdModelGetContactTolerance(p->model2);
  uVar8 = McdModelGetGeometry(p->model1);
  uVar9 = McdModelGetGeometry(p->model2);
  result->contactCount = 0;
  result->touch = 0;
  lVar13 = (longdouble)McdSphereGetRadius(uVar8);
  lVar14 = (longdouble)McdSphereGetRadius(uVar9);
  lVar21 = (longdouble)(float)lVar13 + lVar14;
  lVar15 = (longdouble)*(float *)(iVar6 + 0x30) - (longdouble)*(float *)(iVar7 + 0x30);
  lVar16 = (longdouble)*(float *)(iVar6 + 0x34) - (longdouble)*(float *)(iVar7 + 0x34);
  lVar20 = (longdouble)0;
  lVar17 = (longdouble)*(float *)(iVar6 + 0x38) - (longdouble)*(float *)(iVar7 + 0x38);
  lVar19 = lVar15 * lVar15 + lVar16 * lVar16 + lVar17 * lVar17;
  if (lVar19 == lVar20) {
    normal.v[2] = 1.0;
    lVar15 = lVar20;
    lVar16 = lVar20;
  }
  else {
    lVar19 = SQRT(lVar19);
    lVar18 = lVar20;
    if (lVar19 != lVar20) {
      lVar18 = (longdouble)1 / lVar19;
    }
    lVar15 = (longdouble)(float)lVar15 * lVar18;
    lVar16 = (longdouble)(float)lVar16 * lVar18;
    normal.v[2] = (MeReal)(lVar18 * (longdouble)(float)lVar17);
  }
  normal.v[1] = (MeReal)lVar16;
  normal.v[0] = (MeReal)lVar15;
  iVar10 = 0;
  if (lVar19 - lVar21 <= (longdouble)(float)(lVar12 + (longdouble)(float)lVar11)) {
    if (result->contactMaxCount < 1) {
      result->contactCount = 0;
    }
    else {
                    /* Unresolved local var: MeReal invSumR@[???]
                       Unresolved local var: MeReal wA@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal wB@[DW_OP_reg12(ST1)] */
      result->contacts->dims = 0x303;
      pMVar5 = result->contacts;
      pMVar5->normal[0] = normal.v[0];
      pMVar5->normal[1] = normal.v[1];
      pMVar5->normal[2] = normal.v[2];
      result->contacts->separation = (float)(lVar19 - lVar21);
      if (lVar21 != lVar20) {
        lVar20 = (longdouble)1 / lVar21;
      }
      lVar14 = lVar14 * lVar20;
      pMVar5 = result->contacts;
      lVar20 = lVar20 * (longdouble)(float)lVar13;
      fVar1 = *(float *)(iVar6 + 0x34);
      fVar2 = *(float *)(iVar6 + 0x38);
      fVar3 = *(float *)(iVar7 + 0x34);
      fVar4 = *(float *)(iVar7 + 0x38);
      pMVar5->position[0] =
           (float)(lVar14 * (longdouble)*(float *)(iVar6 + 0x30) +
                  lVar20 * (longdouble)*(float *)(iVar7 + 0x30));
      pMVar5->position[1] = (float)(lVar14 * (longdouble)fVar1 + lVar20 * (longdouble)fVar3);
      pMVar5->position[2] = (float)(lVar14 * (longdouble)fVar2 + lVar20 * (longdouble)fVar4);
      result->normal[0] = normal.v[0];
      result->normal[1] = normal.v[1];
      result->normal[2] = normal.v[2];
      result->contactCount = 1;
    }
    iVar10 = 1;
    result->touch = 1;
  }
  return iVar10;
}


/* ==== McdSphereSphereSafeTime ==== */

/* WARNING: Unknown calling convention */

int McdSphereSphereSafeTime(McdModelPair *p,MeReal maxTime,McdSafeTimeResult *result)

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
  float *pfVar13;
  int iVar14;
  float *pfVar15;
  int iVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  longdouble lVar19;
  longdouble lVar20;
  MeReal radS1;
  McdSphereID geometry2;
  lsVec3 velocity;
  lsVec3 pose;
  lsVec3 posS2;
  lsVec3 posS1;
  lsVec3 velS2;
  lsVec3 velS1;
  
                    /* Unresolved local var: MeReal * velocityS1@[DW_OP_reg6(ESI)]
                       Unresolved local var: lsTransform * xformS1@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal * velocityS2@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * xformS2@[DW_OP_reg0(EAX)]
                       Unresolved local var: McdSphereID geometry1@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal radS2@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal proxSq@[???]
                       Unresolved local var: MeReal proj@[DW_OP_reg16(ST5)]
                       Unresolved local var: float __result@[???] */
  result->pair = p;
  result->time = maxTime;
  pfVar13 = (float *)McdModelGetLinearVelocityPtr(p->model1);
  iVar14 = McdModelGetTransformPtr(p->model1);
  pfVar15 = (float *)McdModelGetLinearVelocityPtr(p->model2);
  iVar16 = McdModelGetTransformPtr(p->model2);
  fVar1 = *(float *)(iVar14 + 0x30);
  fVar2 = *(float *)(iVar14 + 0x34);
  fVar3 = *(float *)(iVar14 + 0x38);
  fVar4 = *(float *)(iVar16 + 0x30);
  fVar5 = *(float *)(iVar16 + 0x34);
  fVar6 = *(float *)(iVar16 + 0x38);
  fVar7 = *pfVar13;
  fVar8 = pfVar13[1];
  fVar9 = pfVar13[2];
  fVar10 = *pfVar15;
  fVar11 = pfVar15[1];
  fVar12 = pfVar15[2];
  uVar17 = McdModelGetGeometry(p->model1);
  uVar18 = McdModelGetGeometry(p->model2);
  lVar19 = (longdouble)McdSphereGetRadius(uVar17);
  lVar20 = (longdouble)McdSphereGetRadius(uVar18);
  fVar10 = fVar10 - fVar7;
  fVar11 = fVar11 - fVar8;
  fVar4 = fVar4 - fVar1;
  fVar5 = fVar5 - fVar2;
  fVar6 = fVar6 - fVar3;
  fVar12 = fVar12 - fVar9;
  iVar14 = 0;
  if (0.001 <= SQRT(fVar10 * fVar10 + fVar11 * fVar11 + fVar12 * fVar12) * maxTime) {
    if (0.0 <= fVar4 * fVar10 + fVar5 * fVar11 + fVar6 * fVar12) {
                    /* Unresolved local var: float __result@[???] */
      result->time = (float)((lVar20 - (SQRT((longdouble)fVar6 * (longdouble)fVar6 +
                                             (longdouble)fVar5 * (longdouble)fVar5 +
                                             (longdouble)fVar4 * (longdouble)fVar4) -
                                       (longdouble)(float)lVar19)) /
                            SQRT((longdouble)fVar12 * (longdouble)fVar12 +
                                 (longdouble)fVar10 * (longdouble)fVar10 +
                                 (longdouble)fVar11 * (longdouble)fVar11));
      iVar14 = 0;
    }
    else {
                    /* Unresolved local var: float __result@[???] */
      iVar14 = 1;
      result->time = (float)(((SQRT((longdouble)fVar6 * (longdouble)fVar6 +
                                    (longdouble)fVar5 * (longdouble)fVar5 +
                                    (longdouble)fVar4 * (longdouble)fVar4) -
                              (longdouble)(float)lVar19) - lVar20) /
                            SQRT((longdouble)fVar12 * (longdouble)fVar12 +
                                 (longdouble)fVar11 * (longdouble)fVar11 +
                                 (longdouble)fVar10 * (longdouble)fVar10));
    }
  }
  return iVar14;
}


/* ==== McdSphereSphereRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

MeBool McdSphereSphereRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdSphereSphereIntersect;
  interactions.safetimeFn = McdSphereSphereSafeTime;
  interactions.cull = 0;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,1,1,&interactions);
  return 1;
}


