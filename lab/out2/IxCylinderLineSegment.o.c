/* ==== IxCylinderLineSegment ==== */

int IxCylinderLineSegment
              (McdModelID_conflict model,MeReal *inOrig,MeReal *inDest,
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
  float *pfVar13;
  undefined4 uVar14;
  int iVar15;
  longdouble lVar16;
  longdouble lVar17;
  MeReal s;
  MeReal hh;
  MeReal r;
  lsVec3 ip;
  lsVec3 normal;
  lsVec3 dir;
  lsVec3 p1;
  lsVec3 p0;
  
                    /* Unresolved local var: lsTransform * tm@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdCylinderID inCylinder@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal s1@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal pt@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal dx@[???]
                       Unresolved local var: MeReal dy@[???]
                       Unresolved local var: MeReal dz@[???]
                       Unresolved local var: MeReal x0@[???]
                       Unresolved local var: MeReal y0@[???]
                       Unresolved local var: MeReal z0@[???] */
  pfVar13 = (float *)McdModelGetTransformPtr(model);
  uVar14 = McdModelGetGeometry(model);
  lVar16 = (longdouble)McdCylinderGetRadius(uVar14);
  lVar17 = (longdouble)McdCylinderGetHalfHeight(uVar14);
  iVar15 = 0;
  fVar1 = (float)lVar17;
  if ((longdouble)1e-06 <= lVar17) {
    fVar2 = *inOrig - pfVar13[0xc];
    fVar5 = inOrig[1] - pfVar13[0xd];
    fVar9 = inOrig[2] - pfVar13[0xe];
    fVar3 = fVar9 * pfVar13[2] + fVar5 * pfVar13[1] + fVar2 * *pfVar13;
    fVar10 = fVar9 * pfVar13[6] + fVar2 * pfVar13[4] + fVar5 * pfVar13[5];
    fVar11 = fVar9 * pfVar13[10] + fVar5 * pfVar13[9] + fVar2 * pfVar13[8];
    fVar2 = *inDest - pfVar13[0xc];
    fVar5 = inDest[1] - pfVar13[0xd];
    fVar6 = inDest[2] - pfVar13[0xe];
    fVar9 = (fVar6 * pfVar13[6] + fVar2 * pfVar13[4] + fVar5 * pfVar13[5]) - fVar10;
    s = 3.4028235e+38;
    fVar12 = (fVar2 * pfVar13[8] + fVar5 * pfVar13[9] + fVar6 * pfVar13[10]) - fVar11;
    fVar2 = (fVar6 * pfVar13[2] + fVar5 * pfVar13[1] + fVar2 * *pfVar13) - fVar3;
    normal.v[0] = 0.0;
    fVar5 = (float)lVar16 * (float)lVar16;
    normal.v[1] = 0.0;
    normal.v[2] = 0.0;
    if (((fVar5 < fVar3 * fVar3 + fVar10 * fVar10) || (fVar1 < fVar11)) || (fVar11 < -fVar1)) {
                    /* Unresolved local var: MeReal A@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal B@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal C@[???]
                       Unresolved local var: MeReal root@[DW_OP_reg12(ST1)] */
      if (1e-06 < ABS(fVar12)) {
        fVar6 = (fVar1 - fVar11) / fVar12;
        if (((0.0 <= fVar6) && (fVar6 < 3.4028235e+38)) &&
           (fVar4 = fVar6 * fVar2 + fVar3, fVar7 = fVar6 * fVar9 + fVar10,
           fVar7 * fVar7 + fVar4 * fVar4 <= fVar5)) {
          normal.v[2] = 1.0;
          s = fVar6;
        }
        fVar6 = (-fVar1 - fVar11) * (1.0 / fVar12);
        if (((0.0 <= fVar6) && (fVar6 < s)) &&
           (fVar4 = fVar6 * fVar2 + fVar3, fVar7 = fVar6 * fVar9 + fVar10,
           fVar7 * fVar7 + fVar4 * fVar4 <= fVar5)) {
          normal.v[2] = -1.0;
          s = fVar6;
        }
      }
      fVar6 = fVar3 * fVar2 + fVar9 * fVar10;
      fVar4 = fVar2 * fVar2 + fVar9 * fVar9;
      fVar6 = fVar6 + fVar6;
      fVar5 = fVar6 * fVar6 - ((fVar3 * fVar3 + fVar10 * fVar10) - fVar5) * fVar4 * 4.0;
      if ((0.0 <= fVar5) && (1e-12 < ABS(fVar4))) {
                    /* Unresolved local var: MeReal z1@[DW_OP_reg13(ST2)]
                       Unresolved local var: float __result@[DW_OP_reg15(ST4)] */
        fVar5 = SQRT(fVar5);
        fVar7 = (1.0 / (fVar4 + fVar4)) * (fVar5 - fVar6);
        if ((0.0 <= fVar7) &&
           (((fVar7 < s && (fVar8 = fVar12 * fVar7 + fVar11, fVar8 <= fVar1)) && (-fVar1 <= fVar8)))
           ) {
          normal.v[2] = 0.0;
          s = fVar7;
        }
        fVar5 = (1.0 / (fVar4 + fVar4)) * (-fVar6 - fVar5);
        if (((0.0 <= fVar5) && (fVar5 < s)) &&
           ((fVar6 = fVar12 * fVar5 + fVar11, fVar6 <= fVar1 && (-fVar1 <= fVar6)))) {
          normal.v[2] = 0.0;
          s = fVar5;
        }
      }
    }
    else {
      s = 0.0;
      normal.v[2] = 1.0;
    }
    if (1.0 <= s) {
      iVar15 = 0;
    }
    else {
                    /* Unresolved local var: float __result@[???]
                       Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
      fVar3 = fVar3 + s * fVar2;
      fVar10 = fVar10 + s * fVar9;
      fVar11 = fVar11 + s * fVar12;
      info->position[0] =
           fVar3 * *pfVar13 + fVar10 * pfVar13[4] + fVar11 * pfVar13[8] + pfVar13[0xc];
      info->position[1] =
           fVar11 * pfVar13[9] + fVar3 * pfVar13[1] + fVar10 * pfVar13[5] + pfVar13[0xd];
      info->position[2] =
           fVar3 * pfVar13[2] + fVar10 * pfVar13[6] + fVar11 * pfVar13[10] + pfVar13[0xe];
      info->distance = SQRT(fVar2 * fVar2 + fVar9 * fVar9 + fVar12 * fVar12) * s;
                    /* Unresolved local var: MeReal t@[???] */
      if ((normal.v[2] == 0.0) &&
         (fVar1 = SQRT(normal.v[2] * normal.v[2] + fVar3 * fVar3 + fVar10 * fVar10),
         normal.v[0] = fVar3, normal.v[1] = fVar10, fVar1 != 0.0)) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
        fVar1 = 1.0 / fVar1;
        normal.v[2] = fVar1 * normal.v[2];
        normal.v[0] = fVar3 * fVar1;
        normal.v[1] = fVar10 * fVar1;
      }
      iVar15 = 1;
      info->normal[0] = normal.v[2] * pfVar13[8] + normal.v[1] * pfVar13[4] + normal.v[0] * *pfVar13
      ;
      info->normal[1] =
           normal.v[2] * pfVar13[9] + normal.v[0] * pfVar13[1] + normal.v[1] * pfVar13[5];
      info->normal[2] =
           normal.v[2] * pfVar13[10] + normal.v[0] * pfVar13[2] + normal.v[1] * pfVar13[6];
    }
  }
  return iVar15;
}


/* ==== McdCylinderLineSegmentRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

void McdCylinderLineSegmentRegisterInteraction(McdFramework *frame)

{
  McdFrameworkSetLineSegInteraction(frame,4,0x10000);
  return;
}


