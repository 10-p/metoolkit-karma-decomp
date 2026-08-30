/* ==== printV3 ==== */

void printV3(char *str,lsVec3 *V3)

{
  printf("%s: %f %f %f \n",str,(double)V3->v[0],(double)V3->v[1],(double)V3->v[2]);
  return;
}


/* ==== MovingBoxBoxIntersect ==== */

/* WARNING: Type propagation algorithm not settling */

uint MovingBoxBoxIntersect
               (MeReal *ExtBox0,lsTransform *tm0,lsVec3 *V0,MeReal *ExtBox1,lsTransform *tm1,
               lsVec3 *V1,MeReal dt,MeReal *T,lsVec3 *P)

{
  lsTransformRow *plVar1;
  lsTransformRow *plVar2;
  lsTransformRow *plVar3;
  lsTransformRow *plVar4;
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
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float fVar20;
  float fVar21;
  float fVar22;
  float fVar23;
  float fVar24;
  float fVar25;
  float fVar26;
  float fVar27;
  float fVar28;
  float fVar29;
  float fVar30;
  float fVar31;
  float fVar32;
  float fVar33;
  float fVar34;
  float fVar35;
  float fVar36;
  float fVar37;
  float local_164;
  float local_160;
  float local_154;
  float local_150;
  float local_14c;
  float local_148;
  MeReal R;
  MeReal BR0 [3];
  MeReal AR1 [3];
  MeReal BD1 [3];
  MeReal BD0 [3];
  MeReal AD1 [3];
  MeReal AD0 [3];
  MeReal fAB [3] [3];
  MeReal AB [3] [3];
  lsVec3 D1;
  lsVec3 D0;
  lsVec3 W;
  lsVec3 *B [3];
  lsVec3 *A [3];
  
                    /* Unresolved local var: MeReal R0@[???]
                       Unresolved local var: MeReal R1@[???]
                       Unresolved local var: MeReal R01@[DW_OP_reg17(ST6)]
                       Unresolved local var: uint type@[???]
                       Unresolved local var: int side@[???] */
  plVar1 = tm0->row + 2;
  plVar2 = tm0->row + 1;
  plVar3 = tm1->row + 1;
  plVar4 = tm1->row + 2;
  fVar8 = tm1->row[3].v.v[1] - tm0->row[3].v.v[1];
  fVar10 = tm1->row[3].v.v[2] - tm0->row[3].v.v[2];
  fVar6 = tm1->row[3].v.v[0] - tm0->row[3].v.v[0];
  fVar5 = (V1->v[0] - V0->v[0]) * dt + fVar6;
  fVar7 = (V1->v[1] - V0->v[1]) * dt + fVar8;
  fVar9 = (V1->v[2] - V0->v[2]) * dt + fVar10;
  *T = 0.0;
  fVar11 = tm1->row[0].v.v[2] * tm0->row[0].v.v[2] +
           tm1->row[0].v.v[1] * tm0->row[0].v.v[1] + tm1->row[0].v.v[0] * tm0->row[0].v.v[0];
  fVar12 = ABS(fVar11);
  fVar19 = tm1->row[1].v.v[2] * tm0->row[0].v.v[2] +
           tm1->row[1].v.v[1] * tm0->row[0].v.v[1] + (plVar3->v).v[0] * tm0->row[0].v.v[0];
  fVar18 = ABS(fVar19);
  fVar17 = tm1->row[2].v.v[2] * tm0->row[0].v.v[2] +
           tm1->row[2].v.v[1] * tm0->row[0].v.v[1] + (plVar4->v).v[0] * tm0->row[0].v.v[0];
  fVar16 = ABS(fVar17);
  fVar15 = fVar6 * tm0->row[0].v.v[0] + fVar8 * tm0->row[0].v.v[1] + fVar10 * tm0->row[0].v.v[2];
  fVar14 = fVar7 * tm0->row[0].v.v[1] + fVar5 * tm0->row[0].v.v[0] + fVar9 * tm0->row[0].v.v[2];
  local_164 = *ExtBox1;
  local_14c = ExtBox1[1];
  local_154 = ExtBox1[2];
  fVar13 = *ExtBox0 + local_164 * fVar12 + local_14c * fVar18 + fVar16 * local_154;
                    /* Unresolved local var: MeReal tmp@[DW_OP_reg12(ST1)] */
  if (fVar15 <= fVar13) {
    if (fVar15 < -fVar13) {
      if (fVar14 < -fVar13) {
        return 0;
      }
      fVar13 = (-dt * (fVar13 + fVar15)) / (fVar14 - fVar15);
      goto LAB_00010297;
    }
  }
  else {
    if (fVar13 < fVar14) {
      return 0;
    }
    fVar13 = ((fVar13 - fVar15) * dt) / (fVar14 - fVar15);
LAB_00010297:
    if (*T < fVar13) {
      *T = fVar13;
      local_164 = *ExtBox1;
      local_14c = ExtBox1[1];
      local_154 = ExtBox1[2];
    }
  }
  fVar13 = tm1->row[0].v.v[2] * tm0->row[1].v.v[2] +
           tm1->row[0].v.v[1] * tm0->row[1].v.v[1] + tm1->row[0].v.v[0] * (plVar2->v).v[0];
  fVar20 = ABS(fVar13);
  fVar26 = tm1->row[1].v.v[2] * tm0->row[1].v.v[2] +
           tm1->row[1].v.v[1] * tm0->row[1].v.v[1] + (plVar3->v).v[0] * (plVar2->v).v[0];
  fVar25 = ABS(fVar26);
  fVar24 = tm1->row[2].v.v[2] * tm0->row[1].v.v[2] +
           tm1->row[2].v.v[1] * tm0->row[1].v.v[1] + (plVar4->v).v[0] * (plVar2->v).v[0];
  fVar23 = ABS(fVar24);
  fVar27 = fVar10 * tm0->row[1].v.v[2] + fVar8 * tm0->row[1].v.v[1] + fVar6 * (plVar2->v).v[0];
  fVar22 = fVar7 * tm0->row[1].v.v[1] + fVar5 * (plVar2->v).v[0] + fVar9 * tm0->row[1].v.v[2];
  fVar21 = ExtBox0[1] + local_164 * fVar20 + fVar25 * local_14c + fVar23 * local_154;
                    /* Unresolved local var: MeReal tmp@[DW_OP_reg12(ST1)] */
  if (fVar27 <= fVar21) {
    if (fVar27 < -fVar21) {
      if (fVar22 < -fVar21) {
        return 0;
      }
      fVar21 = (-dt * (fVar21 + fVar27)) / (fVar22 - fVar27);
      goto LAB_0001047c;
    }
  }
  else {
    if (fVar21 < fVar22) {
      return 0;
    }
    fVar21 = ((fVar21 - fVar27) * dt) / (fVar22 - fVar27);
LAB_0001047c:
    if (*T < fVar21) {
      *T = fVar21;
      local_164 = *ExtBox1;
      local_14c = ExtBox1[1];
      local_154 = ExtBox1[2];
    }
  }
  fVar21 = tm1->row[0].v.v[2] * tm0->row[2].v.v[2] +
           tm1->row[0].v.v[1] * tm0->row[2].v.v[1] + tm1->row[0].v.v[0] * (plVar1->v).v[0];
  fVar28 = ABS(fVar21);
  fVar34 = tm1->row[1].v.v[2] * tm0->row[2].v.v[2] +
           tm1->row[1].v.v[1] * tm0->row[2].v.v[1] + (plVar3->v).v[0] * (plVar1->v).v[0];
  fVar33 = ABS(fVar34);
  fVar32 = tm1->row[2].v.v[2] * tm0->row[2].v.v[2] +
           tm1->row[2].v.v[1] * tm0->row[2].v.v[1] + (plVar4->v).v[0] * (plVar1->v).v[0];
  fVar31 = ABS(fVar32);
  fVar35 = fVar10 * tm0->row[2].v.v[2] + fVar8 * tm0->row[2].v.v[1] + fVar6 * (plVar1->v).v[0];
  fVar30 = fVar7 * tm0->row[2].v.v[1] + fVar5 * (plVar1->v).v[0] + fVar9 * tm0->row[2].v.v[2];
  local_150 = ExtBox0[2];
  fVar29 = local_150 + local_164 * fVar28 + fVar33 * local_14c + fVar31 * local_154;
                    /* Unresolved local var: MeReal tmp@[DW_OP_reg12(ST1)] */
  if (fVar35 <= fVar29) {
    if (fVar35 < -fVar29) {
      if (fVar30 < -fVar29) {
        return 0;
      }
      fVar29 = (-dt * (fVar29 + fVar35)) / (fVar30 - fVar35);
      goto LAB_00010662;
    }
  }
  else {
    if (fVar29 < fVar30) {
      return 0;
    }
    fVar29 = ((fVar29 - fVar35) * dt) / (fVar30 - fVar35);
LAB_00010662:
    if (*T < fVar29) {
      *T = fVar29;
      local_164 = *ExtBox1;
      local_150 = ExtBox0[2];
    }
  }
  fVar29 = fVar10 * tm1->row[0].v.v[2] + fVar6 * tm1->row[0].v.v[0] + fVar8 * tm1->row[0].v.v[1];
  fVar36 = fVar9 * tm1->row[0].v.v[2] + fVar5 * tm1->row[0].v.v[0] + fVar7 * tm1->row[0].v.v[1];
  local_160 = *ExtBox0;
  local_148 = ExtBox0[1];
  fVar37 = local_164 + local_150 * fVar28 + local_148 * fVar20 + local_160 * fVar12;
                    /* Unresolved local var: MeReal tmp@[DW_OP_reg12(ST1)] */
  if (fVar29 <= fVar37) {
    if (fVar29 < -fVar37) {
      if (fVar36 < -fVar37) {
        return 0;
      }
      fVar29 = (-dt * (fVar37 + fVar29)) / (fVar36 - fVar29);
      goto LAB_000107a6;
    }
  }
  else {
    if (fVar37 < fVar36) {
      return 0;
    }
    fVar29 = ((fVar37 - fVar29) * dt) / (fVar36 - fVar29);
LAB_000107a6:
    if (*T < fVar29) {
      *T = fVar29;
      local_160 = *ExtBox0;
      local_148 = ExtBox0[1];
      local_150 = ExtBox0[2];
    }
  }
  fVar29 = fVar10 * tm1->row[1].v.v[2] + fVar6 * (plVar3->v).v[0] + fVar8 * tm1->row[1].v.v[1];
  fVar36 = fVar9 * tm1->row[1].v.v[2] + fVar5 * (plVar3->v).v[0] + fVar7 * tm1->row[1].v.v[1];
  fVar37 = local_150 * fVar33 + local_160 * fVar18 + local_148 * fVar25 + ExtBox1[1];
                    /* Unresolved local var: MeReal tmp@[DW_OP_reg12(ST1)] */
  if (fVar29 <= fVar37) {
    if (fVar29 < -fVar37) {
      if (fVar36 < -fVar37) {
        return 0;
      }
      fVar29 = (-dt * (fVar37 + fVar29)) / (fVar36 - fVar29);
      goto LAB_000108e6;
    }
  }
  else {
    if (fVar37 < fVar36) {
      return 0;
    }
    fVar29 = ((fVar37 - fVar29) * dt) / (fVar36 - fVar29);
LAB_000108e6:
    if (*T < fVar29) {
      *T = fVar29;
      local_160 = *ExtBox0;
      local_148 = ExtBox0[1];
      local_150 = ExtBox0[2];
    }
  }
  fVar6 = fVar6 * (plVar4->v).v[0] + fVar8 * tm1->row[2].v.v[1] + fVar10 * tm1->row[2].v.v[2];
  fVar5 = fVar5 * (plVar4->v).v[0] + fVar7 * tm1->row[2].v.v[1] + fVar9 * tm1->row[2].v.v[2];
  local_154 = ExtBox1[2];
  fVar7 = local_154 + local_150 * fVar31 + local_160 * fVar16 + local_148 * fVar23;
                    /* Unresolved local var: MeReal tmp@[DW_OP_reg12(ST1)] */
  if (fVar6 <= fVar7) {
    if (fVar6 < -fVar7) {
      if (fVar5 < -fVar7) {
        return 0;
      }
      fVar5 = (-dt * (fVar7 + fVar6)) / (fVar5 - fVar6);
      goto LAB_00010a3a;
    }
  }
  else {
    if (fVar7 < fVar5) {
      return 0;
    }
    fVar5 = ((fVar7 - fVar6) * dt) / (fVar5 - fVar6);
LAB_00010a3a:
    if (*T < fVar5) {
      *T = fVar5;
      local_154 = ExtBox1[2];
      local_148 = ExtBox0[1];
      local_150 = ExtBox0[2];
    }
  }
  fVar5 = local_148 * fVar28 + local_150 * fVar20 + local_154 * fVar18 + ExtBox1[1] * fVar16;
  fVar6 = fVar35 * fVar13 - fVar27 * fVar21;
                    /* Unresolved local var: MeReal q@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal tmp@[DW_OP_reg12(ST1)] */
  if (fVar6 <= fVar5) {
    if (fVar6 < -fVar5) {
      fVar7 = fVar30 * fVar13 - fVar21 * fVar22;
      if (fVar7 < -fVar5) {
        return 0;
      }
      fVar5 = -dt * (fVar5 + fVar6);
      goto LAB_00010b40;
    }
  }
  else {
    fVar7 = fVar30 * fVar13 - fVar21 * fVar22;
    if (fVar5 < fVar7) {
      return 0;
    }
    fVar5 = (fVar5 - fVar6) * dt;
LAB_00010b40:
    fVar5 = fVar5 / (fVar7 - fVar6);
    if (*T < fVar5) {
      *T = fVar5;
      local_154 = ExtBox1[2];
      local_148 = ExtBox0[1];
      local_150 = ExtBox0[2];
    }
  }
  local_164 = *ExtBox1;
  fVar5 = local_148 * fVar33 + local_150 * fVar25 + local_154 * fVar12 + fVar16 * local_164;
  fVar6 = fVar35 * fVar26 - fVar27 * fVar34;
                    /* Unresolved local var: MeReal q@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal tmp@[DW_OP_reg12(ST1)] */
  if (fVar6 <= fVar5) {
    if (fVar6 < -fVar5) {
      fVar7 = fVar30 * fVar26 - fVar34 * fVar22;
      if (fVar7 < -fVar5) {
        return 0;
      }
      fVar5 = -dt * (fVar5 + fVar6);
      goto LAB_00010c6f;
    }
  }
  else {
    fVar7 = fVar30 * fVar26 - fVar34 * fVar22;
    if (fVar5 < fVar7) {
      return 0;
    }
    fVar5 = (fVar5 - fVar6) * dt;
LAB_00010c6f:
    fVar5 = fVar5 / (fVar7 - fVar6);
    if (*T < fVar5) {
      *T = fVar5;
      local_164 = *ExtBox1;
      local_148 = ExtBox0[1];
      local_150 = ExtBox0[2];
    }
  }
  local_14c = ExtBox1[1];
  fVar5 = local_148 * fVar31 + local_150 * fVar23 + local_14c * fVar12 + local_164 * fVar18;
  fVar6 = fVar35 * fVar24 - fVar27 * fVar32;
                    /* Unresolved local var: MeReal q@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal tmp@[DW_OP_reg12(ST1)] */
  if (fVar6 <= fVar5) {
    if (fVar6 < -fVar5) {
      fVar7 = fVar24 * fVar30 - fVar32 * fVar22;
      if (fVar7 < -fVar5) {
        return 0;
      }
      fVar5 = -dt * (fVar5 + fVar6);
      goto LAB_00010dba;
    }
  }
  else {
    fVar7 = fVar30 * fVar24 - fVar32 * fVar22;
    if (fVar5 < fVar7) {
      return 0;
    }
    fVar5 = (fVar5 - fVar6) * dt;
LAB_00010dba:
    fVar5 = fVar5 / (fVar7 - fVar6);
    if (*T < fVar5) {
      *T = fVar5;
      local_14c = ExtBox1[1];
      local_150 = ExtBox0[2];
    }
  }
  local_160 = *ExtBox0;
  local_154 = ExtBox1[2];
  fVar5 = local_160 * fVar28 + fVar12 * local_150 + local_154 * fVar25 + local_14c * fVar23;
  fVar6 = fVar15 * fVar21 - fVar35 * fVar11;
                    /* Unresolved local var: MeReal q@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal tmp@[DW_OP_reg12(ST1)] */
  if (fVar6 <= fVar5) {
    if (fVar6 < -fVar5) {
      fVar7 = fVar14 * fVar21 - fVar11 * fVar30;
      if (fVar7 < -fVar5) {
        return 0;
      }
      fVar5 = -dt * (fVar5 + fVar6);
      goto LAB_00010ee2;
    }
  }
  else {
    fVar7 = fVar14 * fVar21 - fVar11 * fVar30;
    if (fVar5 < fVar7) {
      return 0;
    }
    fVar5 = (fVar5 - fVar6) * dt;
LAB_00010ee2:
    fVar5 = fVar5 / (fVar7 - fVar6);
    if (*T < fVar5) {
      *T = fVar5;
      local_154 = ExtBox1[2];
      local_160 = *ExtBox0;
      local_150 = ExtBox0[2];
    }
  }
  local_164 = *ExtBox1;
  fVar5 = local_160 * fVar33 + local_150 * fVar18 + fVar23 * local_164 + local_154 * fVar20;
  fVar6 = fVar15 * fVar34 - fVar35 * fVar19;
                    /* Unresolved local var: MeReal q@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal tmp@[DW_OP_reg12(ST1)] */
  if (fVar6 <= fVar5) {
    if (fVar6 < -fVar5) {
      fVar7 = fVar14 * fVar34 - fVar19 * fVar30;
      if (fVar7 < -fVar5) {
        return 0;
      }
      fVar5 = -dt * (fVar5 + fVar6);
      goto LAB_00011006;
    }
  }
  else {
    fVar7 = fVar14 * fVar34 - fVar19 * fVar30;
    if (fVar5 < fVar7) {
      return 0;
    }
    fVar5 = (fVar5 - fVar6) * dt;
LAB_00011006:
    fVar5 = fVar5 / (fVar7 - fVar6);
    if (*T < fVar5) {
      *T = fVar5;
      local_164 = *ExtBox1;
      local_160 = *ExtBox0;
      local_150 = ExtBox0[2];
    }
  }
  local_14c = ExtBox1[1];
  fVar5 = local_160 * fVar31 + local_150 * fVar16 + local_14c * fVar20 + local_164 * fVar25;
  fVar6 = fVar15 * fVar32 - fVar35 * fVar17;
                    /* Unresolved local var: MeReal q@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal tmp@[DW_OP_reg11(ST0)] */
  if (fVar6 <= fVar5) {
    if (fVar6 < -fVar5) {
      fVar7 = fVar14 * fVar32 - fVar17 * fVar30;
      if (fVar7 < -fVar5) {
        return 0;
      }
      fVar5 = -dt * (fVar5 + fVar6);
      goto LAB_0001113a;
    }
  }
  else {
    fVar7 = fVar14 * fVar32 - fVar17 * fVar30;
    if (fVar5 < fVar7) {
      return 0;
    }
    fVar5 = (fVar5 - fVar6) * dt;
LAB_0001113a:
    fVar5 = fVar5 / (fVar7 - fVar6);
    if (*T < fVar5) {
      *T = fVar5;
      local_14c = ExtBox1[1];
      local_160 = *ExtBox0;
    }
  }
  local_148 = ExtBox0[1];
  local_154 = ExtBox1[2];
  fVar6 = local_160 * fVar20 + local_148 * fVar12 + local_154 * fVar33 + local_14c * fVar31;
  fVar5 = fVar27 * fVar11 - fVar15 * fVar13;
                    /* Unresolved local var: MeReal q@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal tmp@[DW_OP_reg11(ST0)] */
  if (fVar5 <= fVar6) {
    if (fVar5 < -fVar6) {
      fVar7 = fVar11 * fVar22 - fVar13 * fVar14;
      if (fVar7 < -fVar6) {
        return 0;
      }
      fVar6 = -dt * (fVar6 + fVar5);
      goto LAB_00011253;
    }
  }
  else {
    fVar7 = fVar11 * fVar22 - fVar13 * fVar14;
    if (fVar6 < fVar7) {
      return 0;
    }
    fVar6 = (fVar6 - fVar5) * dt;
LAB_00011253:
    fVar6 = fVar6 / (fVar7 - fVar5);
    if (*T < fVar6) {
      *T = fVar6;
      local_154 = ExtBox1[2];
      local_160 = *ExtBox0;
      local_148 = ExtBox0[1];
    }
  }
  local_164 = *ExtBox1;
  fVar5 = local_160 * fVar25 + local_148 * fVar18 + local_154 * fVar28 + fVar31 * local_164;
  fVar6 = fVar27 * fVar19 - fVar15 * fVar26;
                    /* Unresolved local var: MeReal q@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal tmp@[DW_OP_reg11(ST0)] */
  if (fVar6 <= fVar5) {
    if (-fVar5 <= fVar6) goto LAB_000113c6;
    fVar7 = fVar19 * fVar22 - fVar26 * fVar14;
    if (fVar7 < -fVar5) {
      return 0;
    }
    fVar5 = -dt * (fVar5 + fVar6);
  }
  else {
    fVar7 = fVar19 * fVar22 - fVar26 * fVar14;
    if (fVar5 < fVar7) {
      return 0;
    }
    fVar5 = (fVar5 - fVar6) * dt;
  }
  fVar5 = fVar5 / (fVar7 - fVar6);
  if (*T < fVar5) {
    *T = fVar5;
    local_164 = *ExtBox1;
    local_160 = *ExtBox0;
    local_148 = ExtBox0[1];
  }
LAB_000113c6:
  fVar5 = fVar27 * fVar17 - fVar15 * fVar24;
  fVar6 = local_160 * fVar23 + local_148 * fVar16 + fVar28 * ExtBox1[1] + local_164 * fVar33;
                    /* Unresolved local var: MeReal q@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal tmp@[DW_OP_reg11(ST0)] */
  if (fVar5 <= fVar6) {
    if (-fVar6 <= fVar5) {
      return 1;
    }
    fVar7 = fVar17 * fVar22 - fVar24 * fVar14;
    if (fVar7 < -fVar6) {
      return 0;
    }
    fVar6 = -dt * (fVar6 + fVar5);
  }
  else {
    fVar7 = fVar17 * fVar22 - fVar24 * fVar14;
    if (fVar6 < fVar7) {
      return 0;
    }
    fVar6 = (fVar6 - fVar5) * dt;
  }
  fVar6 = fVar6 / (fVar7 - fVar5);
  if (*T < fVar6) {
    *T = fVar6;
  }
  return 1;
}


