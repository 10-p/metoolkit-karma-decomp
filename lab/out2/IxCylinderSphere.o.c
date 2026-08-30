/* ==== OverlapCylSphere ==== */

bool OverlapCylSphere(MeReal *outSep,lsVec3 *outN,lsVec3 *outPos,MeI16 *outDims,MeReal *outCylRoC,
                     MeReal inEps,MeReal inRCyl,MeReal inHHCyl,MeReal inRSphere,lsVec3 *inPos)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  MeReal MVar7;
  char cVar8;
  lsVec3 n;
  lsVec3 edgePoint;
  
                    /* Unresolved local var: MeReal separation@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal Dend@[???]
                       Unresolved local var: MeReal Dwall@[???]
                       Unresolved local var: MeReal normWall@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal len@[???]
                       Unresolved local var: MeReal maxSeparation@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeU8 normInfo@[DW_OP_reg3(EBX)]
                       Unresolved local var: float __result@[DW_OP_reg14(ST3)] */
  cVar8 = '\0';
  fVar1 = inPos->v[2];
  fVar4 = (ABS(fVar1) - inRSphere) - inHHCyl;
  if (inEps < fVar4) {
    return false;
  }
                    /* Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
  fVar2 = inPos->v[0];
  fVar3 = inPos->v[1];
  fVar5 = SQRT(fVar3 * fVar3 + fVar2 * fVar2);
  fVar6 = (fVar5 - inRCyl) - inRSphere;
  if ((fVar4 < fVar6) && (cVar8 = '\x01', fVar4 = fVar6, inEps < fVar6)) {
    return false;
  }
  if (fVar4 < -inRSphere) {
    return false;
  }
  if ((inRCyl < fVar5) && (inHHCyl < ABS(fVar1))) {
    if (fVar1 <= 0.0) {
      inHHCyl = -inHHCyl;
    }
    n.v[0] = (inRCyl / fVar5) * fVar2 - fVar2;
    n.v[1] = (inRCyl / fVar5) * fVar3 - fVar3;
                    /* Unresolved local var: MeReal t@[???] */
    n.v[2] = inHHCyl - fVar1;
    fVar1 = SQRT(n.v[0] * n.v[0] + n.v[1] * n.v[1] + n.v[2] * n.v[2]);
    if (fVar1 != 0.0) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
      fVar2 = 1.0 / fVar1;
      n.v[0] = n.v[0] * fVar2;
      n.v[1] = n.v[1] * fVar2;
      n.v[2] = fVar2 * n.v[2];
    }
    fVar1 = fVar1 - inRSphere;
    if ((fVar4 < fVar1) && (cVar8 = '\x02', fVar4 = fVar1, inEps < fVar1)) {
      return false;
    }
  }
  *outSep = fVar4;
  if (cVar8 == '\0') {
    if (inPos->v[2] <= 0.0) {
      MVar7 = 1.0;
    }
    else {
      MVar7 = -1.0;
    }
    outN->v[0] = 0.0;
    outN->v[2] = MVar7;
    outN->v[1] = 0.0;
    *outDims = 0x302;
  }
  else {
    if (cVar8 == '\x01') {
                    /* Unresolved local var: MeReal norm@[???] */
      fVar1 = 0.0;
      if (fVar5 != 0.0) {
        fVar1 = 1.0 / fVar5;
      }
      fVar4 = inPos->v[1];
      fVar2 = inPos->v[0];
      outN->v[2] = 0.0;
      outN->v[1] = -fVar1 * fVar4;
      outN->v[0] = -fVar1 * fVar2;
      *outDims = 0x303;
      *outCylRoC = inRCyl;
      goto LAB_000101e6;
    }
    outN->v[0] = n.v[0];
    outN->v[1] = n.v[1];
    outN->v[2] = n.v[2];
    *outDims = 0x301;
  }
  *outCylRoC = 0.0;
LAB_000101e6:
  fVar1 = outN->v[1];
  fVar4 = outN->v[2];
  fVar2 = inPos->v[1];
  fVar3 = inPos->v[2];
  outPos->v[0] = inRSphere * outN->v[0] + inPos->v[0];
  outPos->v[1] = inRSphere * fVar1 + fVar2;
  outPos->v[2] = inRSphere * fVar4 + fVar3;
  return true;
}


/* ==== McdCylinderSphereIntersect ==== */

/* WARNING: Unknown calling convention */

int McdCylinderSphereIntersect(McdModelPair *p,McdIntersectResult *result)

{
  McdContact *pMVar1;
  bool bVar2;
  float *pfVar3;
  float *pfVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
  longdouble lVar8;
  longdouble lVar9;
  longdouble lVar10;
  longdouble lVar11;
  longdouble lVar12;
  MeReal hhCyl;
  MeReal rCyl;
  McdSphereID geometry2;
  MeReal separation;
  MeI16 dims;
  MeReal cylRoC;
  lsVec3 normal;
  lsVec3 pos;
  lsTransform tAB;
  
                    /* Unresolved local var: lsTransform * tA@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * tB@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal eps@[???]
                       Unresolved local var: McdCylinderID geometry1@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal rB@[DW_OP_reg11(ST0)] */
  pfVar3 = (float *)McdModelGetTransformPtr(p->model1);
  pfVar4 = (float *)McdModelGetTransformPtr(p->model2);
  lVar8 = (longdouble)McdModelGetContactTolerance(p->model1);
  lVar9 = (longdouble)McdModelGetContactTolerance(p->model2);
  result->contactCount = 0;
  result->touch = 0;
  uVar5 = McdModelGetGeometry(p->model1);
  uVar6 = McdModelGetGeometry(p->model2);
  lVar10 = (longdouble)McdCylinderGetRadius(uVar5);
  lVar11 = (longdouble)McdCylinderGetHalfHeight(uVar5);
  lVar12 = (longdouble)McdSphereGetRadius(uVar6);
  tAB.row[0].v.v[0] = pfVar3[2] * pfVar4[2] + pfVar3[1] * pfVar4[1] + *pfVar3 * *pfVar4;
  tAB.row[0].v.v[1] = pfVar3[6] * pfVar4[2] + pfVar3[5] * pfVar4[1] + pfVar3[4] * *pfVar4;
  tAB.row[0].v.v[2] = pfVar3[10] * pfVar4[2] + pfVar3[9] * pfVar4[1] + pfVar3[8] * *pfVar4;
  tAB.row[1].v.v[0] = pfVar3[2] * pfVar4[6] + pfVar3[1] * pfVar4[5] + *pfVar3 * pfVar4[4];
  tAB.row[1].v.v[1] = pfVar3[6] * pfVar4[6] + pfVar3[5] * pfVar4[5] + pfVar3[4] * pfVar4[4];
  tAB.row[1].v.v[2] = pfVar3[10] * pfVar4[6] + pfVar3[9] * pfVar4[5] + pfVar3[8] * pfVar4[4];
  tAB.row[2].v.v[0] = pfVar3[2] * pfVar4[10] + pfVar3[1] * pfVar4[9] + *pfVar3 * pfVar4[8];
  tAB.row[2].v.v[1] = pfVar3[6] * pfVar4[10] + pfVar3[5] * pfVar4[9] + pfVar3[4] * pfVar4[8];
  tAB.row[2].v.v[2] = pfVar3[10] * pfVar4[10] + pfVar3[9] * pfVar4[9] + pfVar3[8] * pfVar4[8];
  pos.v[1] = pfVar4[0xd] - pfVar3[0xd];
  pos.v[2] = pfVar4[0xe] - pfVar3[0xe];
  pos.v[0] = pfVar4[0xc] - pfVar3[0xc];
  tAB.row[3].v.v[0] = pos.v[2] * pfVar3[2] + pos.v[0] * *pfVar3 + pos.v[1] * pfVar3[1];
  tAB.row[3].v.v[1] = pos.v[2] * pfVar3[6] + pos.v[0] * pfVar3[4] + pos.v[1] * pfVar3[5];
  tAB.row[3].v.v[2] = pos.v[2] * pfVar3[10] + pos.v[0] * pfVar3[8] + pos.v[1] * pfVar3[9];
  bVar2 = OverlapCylSphere(&separation,&normal,&pos,&dims,&cylRoC,
                           (float)(lVar9 + (longdouble)(float)lVar8),(float)lVar10,(float)lVar11,
                           (float)lVar12,(lsVec3 *)(tAB.row + 3));
  iVar7 = 0;
  if (bVar2) {
    if (0 < result->contactMaxCount) {
      result->normal[0] = normal.v[2] * pfVar3[8] + normal.v[1] * pfVar3[4] + normal.v[0] * *pfVar3;
      result->normal[1] =
           normal.v[2] * pfVar3[9] + normal.v[0] * pfVar3[1] + normal.v[1] * pfVar3[5];
      result->normal[2] =
           normal.v[2] * pfVar3[10] + normal.v[0] * pfVar3[2] + normal.v[1] * pfVar3[6];
      result->contacts->dims = dims;
      result->contacts->separation = separation;
      pMVar1 = result->contacts;
      pMVar1->position[0] =
           pfVar3[8] * pos.v[2] + pfVar3[4] * pos.v[1] + *pfVar3 * pos.v[0] + pfVar3[0xc];
      pMVar1->position[1] =
           pfVar3[9] * pos.v[2] + pfVar3[5] * pos.v[1] + pfVar3[1] * pos.v[0] + pfVar3[0xd];
      pMVar1->position[2] =
           pfVar3[10] * pos.v[2] + pfVar3[6] * pos.v[1] + pfVar3[2] * pos.v[0] + pfVar3[0xe];
      pMVar1 = result->contacts;
      pMVar1->normal[0] = result->normal[0];
      pMVar1->normal[1] = result->normal[1];
      pMVar1->normal[2] = result->normal[2];
      result->contactCount = 1;
    }
    iVar7 = 1;
    result->touch = 1;
  }
  return iVar7;
}


/* ==== McdCylinderSphereSafeTime ==== */

int McdCylinderSphereSafeTime(McdModelPair *p,MeReal maxTime,McdSafeTimeResult *result)

{
  float fVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  lsVec3 *plVar4;
  lsVec3 *plVar5;
  lsTransform *plVar6;
  lsTransform *plVar7;
  int iVar8;
  longdouble lVar9;
  longdouble lVar10;
  longdouble lVar11;
  MeReal radS;
  MeReal hheightCyl;
  MeReal radCyl;
  lsVec3 *V1;
  lsVec3 *V0;
  MeReal T;
  lsVec3 P;
  MeReal ext2 [3];
  MeReal ext1 [3];
  
                    /* Unresolved local var: McdCylinderID geometry1@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdSphereID geometry2@[DW_OP_reg7(EDI)]
                       Unresolved local var: lsTransform * tm1@[DW_OP_reg6(ESI)]
                       Unresolved local var: lsTransform * tm2@[DW_OP_reg0(EAX)]
                       Unresolved local var: uint ixt@[DW_OP_reg2(EDX)]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
  result->pair = p;
  result->time = maxTime;
  uVar2 = McdModelGetGeometry(p->model1);
  uVar3 = McdModelGetGeometry(p->model2);
  plVar4 = (lsVec3 *)McdModelGetLinearVelocityPtr(p->model1);
  plVar5 = (lsVec3 *)McdModelGetLinearVelocityPtr(p->model2);
  lVar9 = (longdouble)McdCylinderGetRadius(uVar2);
  lVar10 = (longdouble)McdCylinderGetHalfHeight(uVar2);
  lVar11 = (longdouble)McdSphereGetRadius(uVar3);
                    /* Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
  fVar1 = (float)lVar11 * 0.8284271;
  plVar6 = (lsTransform *)McdModelGetTransformPtr(p->model1);
  plVar7 = (lsTransform *)McdModelGetTransformPtr(p->model2);
  ext2[0] = fVar1;
  ext2[1] = fVar1;
  ext2[2] = fVar1;
  ext1[0] = (float)lVar9 * 0.8284271;
  ext1[1] = (float)lVar9 * 0.8284271;
  ext1[2] = (float)lVar10;
  iVar8 = MovingBoxBoxIntersect(ext1,plVar6,plVar4,ext2,plVar7,plVar5,maxTime,&T,&P);
  if (iVar8 == 0) {
    result->time = maxTime;
  }
  else {
    result->time = T;
  }
  return iVar8;
}


/* ==== McdCylinderSphereRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

MeBool McdCylinderSphereRegisterInteraction(McdFramework *frame)

{
  McdInteractions interactions;
  
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdCylinderSphereIntersect;
  interactions.safetimeFn = McdCylinderSphereSafeTime;
  interactions.cull = 0;
  interactions.warned = 0;
  McdFrameworkSetInteractions(frame,4,1,&interactions);
  return 1;
}


