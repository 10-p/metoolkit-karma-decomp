/* ==== McdSphylConvexMeshIntersect ==== */

MeBool McdSphylConvexMeshIntersect(McdModelPair *p,McdIntersectResult *result)

{
  int iVar1;
  float *pfVar2;
  int iVar3;
  int iVar4;
  longdouble lVar5;
  longdouble lVar6;
  longdouble lVar7;
  longdouble lVar8;
  longdouble lVar9;
  longdouble lVar10;
  longdouble lVar11;
  longdouble lVar12;
  MeReal ds;
  McdConvexHull *convex;
  McdConvexMesh *convexMesh;
  McdSphyl *sphyl;
  MeReal eps;
  MeReal s;
  VoronoiRegionType regionType;
  MeReal n [3];
  MeReal sp [3];
  MeReal cp [3];
  MeReal pos [3];
  MeReal axis [3];
  
                    /* Unresolved local var: MeMatrix4Ptr tm1@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeMatrix4Ptr tm2@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeReal n2@[???]
                       Unresolved local var: MeReal sep@[DW_OP_reg14(ST3)] */
  iVar1 = McdModelGetTransformPtr(p->model1);
  pfVar2 = (float *)McdModelGetTransformPtr(p->model2);
  lVar5 = (longdouble)McdModelGetContactTolerance(p->model1);
  lVar6 = (longdouble)McdModelGetContactTolerance(p->model2);
  iVar3 = McdModelGetGeometry(p->model1);
  iVar4 = McdModelGetGeometry(p->model2);
  result->contactCount = 0;
  result->touch = 0;
  axis[0] = pfVar2[2] * *(float *)(iVar1 + 0x28) +
            pfVar2[1] * *(float *)(iVar1 + 0x24) + *pfVar2 * *(float *)(iVar1 + 0x20);
  axis[1] = pfVar2[6] * *(float *)(iVar1 + 0x28) +
            pfVar2[5] * *(float *)(iVar1 + 0x24) + pfVar2[4] * *(float *)(iVar1 + 0x20);
  axis[2] = pfVar2[10] * *(float *)(iVar1 + 0x28) +
            pfVar2[9] * *(float *)(iVar1 + 0x24) + pfVar2[8] * *(float *)(iVar1 + 0x20);
  cp[0] = *(float *)(iVar1 + 0x30) - pfVar2[0xc];
  cp[1] = *(float *)(iVar1 + 0x34) - pfVar2[0xd];
  cp[2] = *(float *)(iVar1 + 0x38) - pfVar2[0xe];
  pos[0] = cp[2] * pfVar2[2] + cp[0] * *pfVar2 + cp[1] * pfVar2[1];
  pos[1] = cp[2] * pfVar2[6] + cp[0] * pfVar2[4] + cp[1] * pfVar2[5];
  pos[2] = cp[0] * pfVar2[8] + cp[1] * pfVar2[9] + cp[2] * pfVar2[10];
  lVar7 = (longdouble)
          ConvexHullNSegment((McdConvexHull *)(iVar4 + 0x10),pos,axis,-*(float *)(iVar3 + 0x14),
                             *(float *)(iVar3 + 0x14),cp,&s,&regionType);
  lVar8 = (longdouble)s;
  lVar9 = (lVar8 * (longdouble)axis[0] + (longdouble)pos[0]) - (longdouble)cp[0];
  lVar10 = (lVar8 * (longdouble)axis[1] + (longdouble)pos[1]) - (longdouble)cp[1];
  lVar8 = (lVar8 * (longdouble)axis[2] + (longdouble)pos[2]) - (longdouble)cp[2];
  n[0] = (MeReal)lVar9;
  n[1] = (MeReal)lVar10;
  n[2] = (MeReal)lVar8;
  lVar12 = lVar9 * lVar9 + lVar10 * lVar10 + lVar8 * lVar8;
  lVar11 = (longdouble)(float)(lVar6 + (longdouble)(float)lVar5);
  if (lVar12 <= lVar11 * lVar11) {
    lVar12 = (longdouble)
             SegmentConvexHullSep
                       (pos,axis,-*(float *)(iVar3 + 0x14),*(float *)(iVar3 + 0x14),
                        (McdConvexHull *)(iVar4 + 0x10),n,&s,&regionType);
  }
  else {
                    /* Unresolved local var: MeReal nInv@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
    lVar11 = (longdouble)1.0 / SQRT(lVar12);
    n[0] = (MeReal)(lVar9 * lVar11);
    lVar12 = lVar12 * lVar11;
    n[1] = (MeReal)(lVar10 * lVar11);
    n[2] = (MeReal)(lVar8 * lVar11);
  }
  lVar8 = (longdouble)*(float *)(iVar4 + 0x2c);
  lVar12 = lVar12 - (lVar8 + (longdouble)*(float *)(iVar3 + 0x10));
  if (lVar12 < (longdouble)(float)(lVar6 + (longdouble)(float)lVar5)) {
    cp[0] = (MeReal)(lVar8 * (longdouble)n[0] + (longdouble)cp[0]);
    cp[2] = (MeReal)(lVar8 * (longdouble)n[2] + (longdouble)cp[2]);
    cp[1] = (MeReal)(lVar8 * (longdouble)n[1] + (longdouble)cp[1]);
    result->normal[0] = 0.0;
    result->normal[1] = 0.0;
    result->normal[2] = 0.0;
    AccumulateSphylContacts
              (cp,n,(float)lVar12,3,(undefined2)regionType,(float *)(iVar1 + 0x20),(float)lVar7,
               pfVar2,result);
  }
  return result->touch;
}


