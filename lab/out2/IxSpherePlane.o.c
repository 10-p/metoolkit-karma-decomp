/* ==== McdSpherePlaneIntersect ==== */

int McdSpherePlaneIntersect(McdModelPair *p,McdIntersectResult *result)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  McdContact *pMVar6;
  int iVar7;
  int iVar8;
  undefined4 uVar9;
  int iVar10;
  longdouble lVar11;
  longdouble lVar12;
  longdouble lVar13;
  longdouble lVar14;
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
  iVar7 = McdModelGetTransformPtr(p->model1);
  iVar8 = McdModelGetTransformPtr(p->model2);
  lVar11 = (longdouble)McdModelGetContactTolerance(p->model1);
  lVar12 = (longdouble)McdModelGetContactTolerance(p->model2);
  uVar9 = McdModelGetGeometry(p->model1);
  McdModelGetGeometry(p->model2);
  result->contactCount = 0;
  result->touch = 0;
  lVar13 = (longdouble)McdSphereGetRadius(uVar9);
  iVar10 = 0;
  fVar1 = *(float *)(iVar8 + 0x20);
  fVar2 = *(float *)(iVar8 + 0x24);
  fVar3 = *(float *)(iVar8 + 0x28);
  lVar14 = (((longdouble)*(float *)(iVar7 + 0x38) - (longdouble)*(float *)(iVar8 + 0x38)) *
            (longdouble)fVar3 +
           ((longdouble)*(float *)(iVar7 + 0x30) - (longdouble)*(float *)(iVar8 + 0x30)) *
           (longdouble)fVar1 +
           ((longdouble)*(float *)(iVar7 + 0x34) - (longdouble)*(float *)(iVar8 + 0x34)) *
           (longdouble)fVar2) - lVar13;
  if (lVar14 <= (longdouble)(float)(lVar12 + (longdouble)(float)lVar11)) {
    if (0 < result->contactMaxCount) {
      result->contacts->dims = 0x203;
      pMVar6 = result->contacts;
      pMVar6->normal[0] = fVar1;
      pMVar6->normal[1] = fVar2;
      pMVar6->normal[2] = fVar3;
      result->contacts->separation = (float)lVar14;
      pMVar6 = result->contacts;
      fVar4 = *(float *)(iVar7 + 0x34);
      fVar5 = *(float *)(iVar7 + 0x38);
      pMVar6->position[0] =
           (float)((longdouble)*(float *)(iVar7 + 0x30) - lVar13 * (longdouble)fVar1);
      pMVar6->position[1] = (float)((longdouble)fVar4 - lVar13 * (longdouble)fVar2);
      pMVar6->position[2] = (float)((longdouble)fVar5 - lVar13 * (longdouble)fVar3);
      result->normal[0] = fVar1;
      result->normal[1] = fVar2;
      result->normal[2] = fVar3;
      result->contactCount = 1;
    }
    iVar10 = 1;
    result->touch = 1;
  }
  return iVar10;
}


/* ==== McdSpherePlaneSafeTime ==== */

/* WARNING: Unknown calling convention */

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
  int iVar10;
  float *pfVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  longdouble lVar14;
  longdouble lVar15;
  longdouble lVar16;
  longdouble lVar17;
  longdouble lVar18;
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
  pfVar9 = (float *)McdModelGetLinearVelocityPtr(p->model1);
  iVar10 = McdModelGetTransformPtr(p->model1);
  pfVar11 = (float *)McdModelGetLinearVelocityPtr(p->model2);
  McdModelGetTransformPtr(p->model2);
  uVar12 = McdModelGetGeometry(p->model1);
  uVar13 = McdModelGetGeometry(p->model2);
  fVar2 = *pfVar9;
  fVar3 = pfVar9[1];
  fVar4 = pfVar9[2];
  fVar5 = *pfVar11;
  fVar6 = pfVar11[1];
  fVar7 = pfVar11[2];
  lVar14 = (longdouble)McdSphereGetRadius(uVar12);
  uVar12 = McdModelGetTransformPtr(p->model2,pfVar11,&normal);
  uVar12 = McdPlaneGetNormal(uVar13,uVar12);
  uVar12 = McdModelGetTransformPtr(p->model2,uVar12,iVar10 + 0x30);
  lVar15 = (longdouble)McdPlaneGetDistanceToPoint(uVar13,uVar12);
  lVar17 = (longdouble)(fVar4 - fVar7) * (longdouble)normal.v[2] +
           (longdouble)(fVar3 - fVar6) * (longdouble)normal.v[1] +
           (longdouble)(fVar2 - fVar5) * (longdouble)normal.v[0];
  lVar16 = (longdouble)normal.v[0] * lVar17;
  lVar18 = lVar17 * (longdouble)normal.v[1];
  lVar17 = lVar17 * (longdouble)normal.v[2];
  lVar16 = SQRT(lVar16 * lVar16 + lVar18 * lVar18 + lVar17 * lVar17);
  bVar8 = (longdouble)0.001 * (longdouble)fVar1 <= (longdouble)maxTime * lVar16;
  if (bVar8) {
    result->time = (float)((lVar15 - (longdouble)(float)lVar14) / lVar16);
  }
  return (uint)bVar8;
}


/* ==== McdSpherePlaneRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

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


