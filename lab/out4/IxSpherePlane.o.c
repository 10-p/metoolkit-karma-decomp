/* ==== McdSpherePlaneIntersect ==== */

int McdSpherePlaneIntersect(McdModelPair *p,McdIntersectResult *result)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  McdContact *pMVar5;
  void *pvVar6;
  void *pvVar7;
  void *pvVar8;
  int iVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  MeReal eps;
  lsTransform *tA;
  lsVec3 normal;
  lsVec3 disp;
  
                    /* Unresolved local var: lsTransform * tB@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdSphereID geometry1@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdPlaneID geometry2@[???]
                       Unresolved local var: MeReal rA@[DW_OP_reg17(ST6)]
                       Unresolved local var: lsVec3 * pA@[???]
                       Unresolved local var: MeReal separation@[DW_OP_reg13(ST2)] */
  pvVar6 = McdModelGetTransformPtr(p->model1);
  pvVar7 = McdModelGetTransformPtr(p->model2);
  fVar10 = McdModelGetContactTolerance(p->model1);
  fVar11 = McdModelGetContactTolerance(p->model2);
  pvVar8 = McdModelGetGeometry(p->model1);
  McdModelGetGeometry(p->model2);
  result->contactCount = 0;
  result->touch = 0;
  fVar12 = McdSphereGetRadius(pvVar8);
  iVar9 = 0;
  fVar1 = *(float *)((int)pvVar7 + 0x20);
  fVar2 = *(float *)((int)pvVar7 + 0x24);
  fVar3 = *(float *)((int)pvVar7 + 0x28);
  fVar4 = ((*(float *)((int)pvVar6 + 0x38) - *(float *)((int)pvVar7 + 0x38)) * fVar3 +
          (*(float *)((int)pvVar6 + 0x30) - *(float *)((int)pvVar7 + 0x30)) * fVar1 +
          (*(float *)((int)pvVar6 + 0x34) - *(float *)((int)pvVar7 + 0x34)) * fVar2) - fVar12;
  if (fVar4 <= fVar11 + fVar10) {
    if (0 < result->contactMaxCount) {
      result->contacts->dims = 0x203;
      pMVar5 = result->contacts;
      pMVar5->normal[0] = fVar1;
      pMVar5->normal[1] = fVar2;
      pMVar5->normal[2] = fVar3;
      result->contacts->separation = fVar4;
      pMVar5 = result->contacts;
      fVar4 = *(float *)((int)pvVar6 + 0x34);
      fVar10 = *(float *)((int)pvVar6 + 0x38);
      pMVar5->position[0] = *(float *)((int)pvVar6 + 0x30) - fVar12 * fVar1;
      pMVar5->position[1] = fVar4 - fVar12 * fVar2;
      pMVar5->position[2] = fVar10 - fVar12 * fVar3;
      result->normal[0] = fVar1;
      result->normal[1] = fVar2;
      result->normal[2] = fVar3;
      result->contactCount = 1;
    }
    iVar9 = 1;
    result->touch = 1;
  }
  return iVar9;
}


/* ==== McdSpherePlaneSafeTime ==== */

int McdSpherePlaneSafeTime(McdModelPair *p,MeReal maxTime,McdSafeTimeResult *result)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  bool bVar8;
  float *pfVar9;
  void *pvVar10;
  float *pfVar11;
  void *pvVar12;
  void *pvVar13;
  float fVar14;
  float fVar15;
  lsVec3 *plVar16;
  MeReal radS;
  McdPlaneID geometry2;
  MeReal *velocityS;
  MeReal scale;
  lsVec3 velocity;
  lsVec3 normal;
  lsVec3 velP;
  lsVec3 velS;
  
                    /* Unresolved local var: lsTransform * xformS@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeReal * velocityP@[DW_OP_reg6(ESI)]
                       Unresolved local var: lsTransform * xformP@[???]
                       Unresolved local var: lsVec3 * posS@[???]
                       Unresolved local var: lsVec3 * posP@[???]
                       Unresolved local var: McdSphereID geometry1@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal dist@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal vel_norm@[???]
                       Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
  result->pair = p;
  result->time = maxTime;
  fVar1 = p->model1->frame->mScale;
  pfVar9 = McdModelGetLinearVelocityPtr(p->model1);
  pvVar10 = McdModelGetTransformPtr(p->model1);
  pvVar10 = (void *)((int)pvVar10 + 0x30);
  pfVar11 = McdModelGetLinearVelocityPtr(p->model2);
  McdModelGetTransformPtr(p->model2);
  pvVar12 = McdModelGetGeometry(p->model1);
  pvVar13 = McdModelGetGeometry(p->model2);
  fVar2 = *pfVar9;
  fVar3 = pfVar9[1];
  fVar4 = pfVar9[2];
  fVar5 = *pfVar11;
  fVar6 = pfVar11[1];
  fVar7 = pfVar11[2];
  fVar14 = McdSphereGetRadius(pvVar12);
  plVar16 = &normal;
  pvVar12 = McdModelGetTransformPtr(p->model2);
  McdPlaneGetNormal(pvVar13,pvVar12,plVar16);
  pvVar12 = McdModelGetTransformPtr(p->model2);
  fVar15 = McdPlaneGetDistanceToPoint(pvVar13,pvVar12,pvVar10);
  fVar2 = (fVar4 - fVar7) * normal.v[2] +
          (fVar3 - fVar6) * normal.v[1] + (fVar2 - fVar5) * normal.v[0];
  fVar2 = SQRT(normal.v[0] * fVar2 * normal.v[0] * fVar2 + fVar2 * normal.v[1] * fVar2 * normal.v[1]
               + fVar2 * normal.v[2] * fVar2 * normal.v[2]);
  bVar8 = fVar1 * 0.001 <= maxTime * fVar2;
  if (bVar8) {
    result->time = (fVar15 - fVar14) / fVar2;
  }
  return (uint)bVar8;
}


/* ==== McdSpherePlaneRegisterInteraction ==== */

MeBool McdSpherePlaneRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdSpherePlaneIntersect;
  interactions.safetimeFn = McdSpherePlaneSafeTime;
  interactions.cull = 0;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,1,3,&interactions);
  return 1;
}


