/* ==== McdSphereSphereIntersect ==== */

int McdSphereSphereIntersect(McdModelPair *p,McdIntersectResult *result)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  McdContact *pMVar5;
  float fVar6;
  float fVar7;
  void *pvVar8;
  void *pvVar9;
  void *pvVar10;
  void *pvVar11;
  int iVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
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
  pvVar8 = McdModelGetTransformPtr(p->model1);
  pvVar9 = McdModelGetTransformPtr(p->model2);
  fVar13 = McdModelGetContactTolerance(p->model1);
  fVar14 = McdModelGetContactTolerance(p->model2);
  pvVar10 = McdModelGetGeometry(p->model1);
  pvVar11 = McdModelGetGeometry(p->model2);
  result->contactCount = 0;
  result->touch = 0;
  fVar15 = McdSphereGetRadius(pvVar10);
  fVar16 = McdSphereGetRadius(pvVar11);
  fVar1 = fVar15 + fVar16;
  fVar7 = *(float *)((int)pvVar8 + 0x30) - *(float *)((int)pvVar9 + 0x30);
  fVar4 = *(float *)((int)pvVar8 + 0x34) - *(float *)((int)pvVar9 + 0x34);
  fVar3 = *(float *)((int)pvVar8 + 0x38) - *(float *)((int)pvVar9 + 0x38);
  fVar2 = fVar7 * fVar7 + fVar4 * fVar4 + fVar3 * fVar3;
  fVar6 = 0.0;
  if (fVar2 == 0.0) {
    normal.v[0] = 0.0;
    normal.v[1] = 0.0;
    normal.v[2] = 1.0;
  }
  else {
    fVar2 = SQRT(fVar2);
    if (fVar2 != 0.0) {
      fVar6 = 1.0 / fVar2;
    }
    normal.v[0] = fVar7 * fVar6;
    normal.v[1] = fVar4 * fVar6;
    normal.v[2] = fVar6 * fVar3;
  }
  iVar12 = 0;
  if (fVar2 - fVar1 <= fVar14 + fVar13) {
    if (result->contactMaxCount < 1) {
      result->contactCount = 0;
    }
    else {
                    /* Unresolved local var: MeReal invSumR@[???]
                       Unresolved local var: MeReal wA@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal wB@[DW_OP_reg12(ST1)] */
      fVar6 = 0.0;
      result->contacts->dims = 0x303;
      pMVar5 = result->contacts;
      pMVar5->normal[0] = normal.v[0];
      pMVar5->normal[1] = normal.v[1];
      pMVar5->normal[2] = normal.v[2];
      result->contacts->separation = fVar2 - fVar1;
      if (fVar1 != 0.0) {
        fVar6 = 1.0 / fVar1;
      }
      fVar16 = fVar16 * fVar6;
      pMVar5 = result->contacts;
      fVar6 = fVar6 * fVar15;
      fVar1 = *(float *)((int)pvVar8 + 0x34);
      fVar2 = *(float *)((int)pvVar8 + 0x38);
      fVar3 = *(float *)((int)pvVar9 + 0x34);
      fVar4 = *(float *)((int)pvVar9 + 0x38);
      pMVar5->position[0] =
           fVar16 * *(float *)((int)pvVar8 + 0x30) + fVar6 * *(float *)((int)pvVar9 + 0x30);
      pMVar5->position[1] = fVar16 * fVar1 + fVar6 * fVar3;
      pMVar5->position[2] = fVar16 * fVar2 + fVar6 * fVar4;
      result->normal[0] = normal.v[0];
      result->normal[1] = normal.v[1];
      result->normal[2] = normal.v[2];
      result->contactCount = 1;
    }
    iVar12 = 1;
    result->touch = 1;
  }
  return iVar12;
}


/* ==== McdSphereSphereSafeTime ==== */

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
  void *pvVar14;
  float *pfVar15;
  void *pvVar16;
  int iVar17;
  float fVar18;
  float fVar19;
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
  pfVar13 = McdModelGetLinearVelocityPtr(p->model1);
  pvVar14 = McdModelGetTransformPtr(p->model1);
  pfVar15 = McdModelGetLinearVelocityPtr(p->model2);
  pvVar16 = McdModelGetTransformPtr(p->model2);
  fVar1 = *(float *)((int)pvVar14 + 0x30);
  fVar2 = *(float *)((int)pvVar14 + 0x34);
  fVar3 = *(float *)((int)pvVar14 + 0x38);
  fVar4 = *(float *)((int)pvVar16 + 0x30);
  fVar5 = *(float *)((int)pvVar16 + 0x34);
  fVar6 = *(float *)((int)pvVar16 + 0x38);
  fVar7 = *pfVar13;
  fVar8 = pfVar13[1];
  fVar9 = pfVar13[2];
  fVar10 = *pfVar15;
  fVar11 = pfVar15[1];
  fVar12 = pfVar15[2];
  pvVar14 = McdModelGetGeometry(p->model1);
  pvVar16 = McdModelGetGeometry(p->model2);
  fVar18 = McdSphereGetRadius(pvVar14);
  fVar19 = McdSphereGetRadius(pvVar16);
  fVar10 = fVar10 - fVar7;
  fVar11 = fVar11 - fVar8;
  fVar4 = fVar4 - fVar1;
  fVar5 = fVar5 - fVar2;
  fVar6 = fVar6 - fVar3;
  fVar12 = fVar12 - fVar9;
  iVar17 = 0;
  if (0.001 <= SQRT(fVar10 * fVar10 + fVar11 * fVar11 + fVar12 * fVar12) * maxTime) {
    if (0.0 <= fVar4 * fVar10 + fVar5 * fVar11 + fVar6 * fVar12) {
                    /* Unresolved local var: float __result@[???] */
      result->time = (fVar19 - (SQRT(fVar6 * fVar6 + fVar5 * fVar5 + fVar4 * fVar4) - fVar18)) /
                     SQRT(fVar12 * fVar12 + fVar10 * fVar10 + fVar11 * fVar11);
      iVar17 = 0;
    }
    else {
                    /* Unresolved local var: float __result@[???] */
      iVar17 = 1;
      result->time = ((SQRT(fVar6 * fVar6 + fVar5 * fVar5 + fVar4 * fVar4) - fVar18) - fVar19) /
                     SQRT(fVar12 * fVar12 + fVar11 * fVar11 + fVar10 * fVar10);
    }
  }
  return iVar17;
}


/* ==== McdSphereSphereRegisterInteraction ==== */

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


