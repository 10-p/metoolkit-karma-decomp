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
  float *pfVar11;
  void *pvVar12;
  int iVar13;
  float fVar14;
  float fVar15;
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
  pfVar11 = McdModelGetTransformPtr(model);
  pvVar12 = McdModelGetGeometry(model);
  fVar14 = McdCylinderGetRadius(pvVar12);
  fVar15 = McdCylinderGetHalfHeight(pvVar12);
  iVar13 = 0;
  if (1e-06 <= fVar15) {
    fVar1 = *inOrig - pfVar11[0xc];
    fVar9 = inOrig[1] - pfVar11[0xd];
    fVar7 = inOrig[2] - pfVar11[0xe];
    fVar2 = fVar7 * pfVar11[2] + fVar9 * pfVar11[1] + fVar1 * *pfVar11;
    fVar8 = fVar7 * pfVar11[6] + fVar1 * pfVar11[4] + fVar9 * pfVar11[5];
    fVar9 = fVar7 * pfVar11[10] + fVar9 * pfVar11[9] + fVar1 * pfVar11[8];
    fVar1 = *inDest - pfVar11[0xc];
    fVar4 = inDest[1] - pfVar11[0xd];
    fVar3 = inDest[2] - pfVar11[0xe];
    fVar7 = (fVar3 * pfVar11[6] + fVar1 * pfVar11[4] + fVar4 * pfVar11[5]) - fVar8;
    s = 3.4028235e+38;
    fVar10 = (fVar1 * pfVar11[8] + fVar4 * pfVar11[9] + fVar3 * pfVar11[10]) - fVar9;
    fVar1 = (fVar3 * pfVar11[2] + fVar4 * pfVar11[1] + fVar1 * *pfVar11) - fVar2;
    normal.v[0] = 0.0;
    fVar14 = fVar14 * fVar14;
    normal.v[1] = 0.0;
    normal.v[2] = 0.0;
    if (((fVar14 < fVar2 * fVar2 + fVar8 * fVar8) || (fVar15 < fVar9)) || (fVar9 < -fVar15)) {
                    /* Unresolved local var: MeReal A@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal B@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal C@[???]
                       Unresolved local var: MeReal root@[DW_OP_reg12(ST1)] */
      if (1e-06 < ABS(fVar10)) {
        fVar4 = (fVar15 - fVar9) / fVar10;
        if (((0.0 <= fVar4) && (fVar4 < 3.4028235e+38)) &&
           (fVar3 = fVar4 * fVar1 + fVar2, fVar5 = fVar4 * fVar7 + fVar8,
           fVar5 * fVar5 + fVar3 * fVar3 <= fVar14)) {
          normal.v[2] = 1.0;
          s = fVar4;
        }
        fVar4 = (-fVar15 - fVar9) * (1.0 / fVar10);
        if (((0.0 <= fVar4) && (fVar4 < s)) &&
           (fVar3 = fVar4 * fVar1 + fVar2, fVar5 = fVar4 * fVar7 + fVar8,
           fVar5 * fVar5 + fVar3 * fVar3 <= fVar14)) {
          normal.v[2] = -1.0;
          s = fVar4;
        }
      }
      fVar4 = fVar2 * fVar1 + fVar7 * fVar8;
      fVar3 = fVar1 * fVar1 + fVar7 * fVar7;
      fVar4 = fVar4 + fVar4;
      fVar14 = fVar4 * fVar4 - ((fVar2 * fVar2 + fVar8 * fVar8) - fVar14) * fVar3 * 4.0;
      if ((0.0 <= fVar14) && (1e-12 < ABS(fVar3))) {
                    /* Unresolved local var: MeReal z1@[DW_OP_reg13(ST2)]
                       Unresolved local var: float __result@[DW_OP_reg15(ST4)] */
        fVar14 = SQRT(fVar14);
        fVar5 = (1.0 / (fVar3 + fVar3)) * (fVar14 - fVar4);
        if ((0.0 <= fVar5) &&
           (((fVar5 < s && (fVar6 = fVar10 * fVar5 + fVar9, fVar6 <= fVar15)) && (-fVar15 <= fVar6))
           )) {
          normal.v[2] = 0.0;
          s = fVar5;
        }
        fVar14 = (1.0 / (fVar3 + fVar3)) * (-fVar4 - fVar14);
        if (((0.0 <= fVar14) && (fVar14 < s)) &&
           ((fVar4 = fVar10 * fVar14 + fVar9, fVar4 <= fVar15 && (-fVar15 <= fVar4)))) {
          normal.v[2] = 0.0;
          s = fVar14;
        }
      }
    }
    else {
      s = 0.0;
      normal.v[2] = 1.0;
    }
    if (1.0 <= s) {
      iVar13 = 0;
    }
    else {
                    /* Unresolved local var: float __result@[???]
                       Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
      fVar2 = fVar2 + s * fVar1;
      fVar8 = fVar8 + s * fVar7;
      fVar9 = fVar9 + s * fVar10;
      info->position[0] = fVar2 * *pfVar11 + fVar8 * pfVar11[4] + fVar9 * pfVar11[8] + pfVar11[0xc];
      info->position[1] =
           fVar9 * pfVar11[9] + fVar2 * pfVar11[1] + fVar8 * pfVar11[5] + pfVar11[0xd];
      info->position[2] =
           fVar2 * pfVar11[2] + fVar8 * pfVar11[6] + fVar9 * pfVar11[10] + pfVar11[0xe];
      info->distance = SQRT(fVar1 * fVar1 + fVar7 * fVar7 + fVar10 * fVar10) * s;
                    /* Unresolved local var: MeReal t@[???] */
      if ((normal.v[2] == 0.0) &&
         (fVar14 = SQRT(normal.v[2] * normal.v[2] + fVar2 * fVar2 + fVar8 * fVar8),
         normal.v[0] = fVar2, normal.v[1] = fVar8, fVar14 != 0.0)) {
                    /* Unresolved local var: MeReal recipX@[DW_OP_reg11(ST0)] */
        fVar14 = 1.0 / fVar14;
        normal.v[2] = fVar14 * normal.v[2];
        normal.v[0] = fVar2 * fVar14;
        normal.v[1] = fVar8 * fVar14;
      }
      iVar13 = 1;
      info->normal[0] = normal.v[2] * pfVar11[8] + normal.v[1] * pfVar11[4] + normal.v[0] * *pfVar11
      ;
      info->normal[1] =
           normal.v[2] * pfVar11[9] + normal.v[0] * pfVar11[1] + normal.v[1] * pfVar11[5];
      info->normal[2] =
           normal.v[2] * pfVar11[10] + normal.v[0] * pfVar11[2] + normal.v[1] * pfVar11[6];
    }
  }
  return iVar13;
}


/* ==== McdCylinderLineSegmentRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

void McdCylinderLineSegmentRegisterInteraction(McdFramework *frame)

{
  McdFrameworkSetLineSegInteraction(frame,4,IxCylinderLineSegment);
  return;
}


