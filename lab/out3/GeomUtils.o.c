/* ==== LineDistance ==== */

MeBool LineDistance(MeReal *p0,MeReal *d0,MeReal *p1,MeReal *d1,MeReal *s,MeReal *t,MeReal eps)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  MeReal det;
  MeReal diff [3];
  
                    /* Unresolved local var: MeReal b@[???]
                       Unresolved local var: MeReal d@[DW_OP_reg14(ST3)] */
  fVar4 = *d1 * *d0 + d0[1] * d1[1] + d0[2] * d1[2];
  fVar3 = *d0 * (*p1 - *p0) + d0[1] * (p1[1] - p0[1]) + d0[2] * (p1[2] - p0[2]);
  fVar2 = 1.0 - fVar4 * fVar4;
  if (fVar2 <= eps) {
    *s = fVar3;
    *t = 0.0;
  }
  else {
                    /* Unresolved local var: MeReal e@[???] */
    fVar1 = *d1 * (*p1 - *p0) + d1[1] * (p1[1] - p0[1]) + d1[2] * (p1[2] - p0[2]);
    fVar3 = (1.0 / fVar2) * (fVar3 - fVar4 * fVar1);
    *s = fVar3;
    *t = fVar3 * fVar4 - fVar1;
  }
  return (uint)(eps < fVar2);
}


/* ==== NSegmentSegment ==== */

MeBool NSegmentSegment(MeReal *p0,MeReal *d0,MeReal s0min,MeReal s0max,MeReal *p1,MeReal *d1,
                      MeReal s1min,MeReal s1max,MeReal *s0,MeReal *s1,MeReal *ds0)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  MeReal MVar8;
  bool bVar9;
  MeReal d0r;
  MeReal det;
  MeReal d0d1;
  MeReal d1d1;
  MeReal r [3];
  
                    /* Unresolved local var: MeReal d1r@[DW_OP_reg16(ST5)] */
  fVar1 = *d1;
  fVar2 = d1[1];
  fVar3 = d1[2];
  fVar7 = fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1;
  fVar5 = d0[2] * fVar3 + d0[1] * fVar2 + *d0 * fVar1;
  fVar6 = fVar7 - fVar5 * fVar5;
  fVar4 = d0[1] * (p1[1] - p0[1]) + *d0 * (*p1 - *p0) + d0[2] * (p1[2] - p0[2]);
  fVar1 = fVar1 * (*p1 - *p0) + fVar2 * (p1[1] - p0[1]) + fVar3 * (p1[2] - p0[2]);
  if (fVar6 <= fVar7 * 0.00025) {
                    /* Unresolved local var: MeReal s1SgnScaleMin@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal s1SgnScaleMax@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal s0min1@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal s0max1@[DW_OP_reg12(ST1)] */
    fVar2 = s1max;
    if (0.0 < fVar5) {
      fVar2 = s1min;
      s1min = s1max;
    }
    fVar3 = fVar5 * s1min + fVar4;
    fVar4 = fVar5 * fVar2 + fVar4;
    if (s0min < fVar3) {
      if (fVar4 < s0max) {
        if (s0min < fVar4) {
          s0min = fVar4;
        }
        *s0 = s0min;
        if (s0max < fVar3) {
          fVar3 = s0max;
        }
        *ds0 = fVar3 - *s0;
        *s1 = (fVar5 * *s0 - fVar1) / fVar7;
      }
      else {
        *s0 = s0max;
        *ds0 = 0.0;
        *s1 = fVar2;
      }
    }
    else {
      *s0 = s0min;
      *ds0 = 0.0;
      *s1 = s1min;
    }
    return 0;
  }
                    /* Unresolved local var: MeReal s0numInf@[DW_OP_reg12(ST1)] */
  fVar2 = fVar4 * fVar7 - fVar5 * fVar1;
  MVar8 = s0min;
  if ((fVar2 < s0min * fVar6) || (bVar9 = false, MVar8 = s0max, s0max * fVar6 < fVar2)) {
    *s0 = MVar8;
    bVar9 = true;
  }
  if (bVar9) {
                    /* Unresolved local var: MeReal s1num@[DW_OP_reg12(ST1)] */
    fVar1 = fVar5 * *s0 - fVar1;
    if ((fVar1 < s1min * fVar7) || (bVar9 = false, s1min = s1max, s1max * fVar7 < fVar1)) {
      *s1 = s1min;
      bVar9 = true;
    }
    if (!bVar9) {
      *s1 = fVar1 / fVar7;
      goto LAB_0001023a;
    }
  }
  else {
    fVar1 = fVar4 * fVar5 - fVar1;
    if ((fVar1 < s1min * fVar6) || (bVar9 = false, s1min = s1max, s1max * fVar6 < fVar1)) {
      bVar9 = true;
      *s1 = s1min;
    }
    if (!bVar9) {
                    /* Unresolved local var: MeReal invDet@[DW_OP_reg11(ST0)] */
      *s0 = fVar2 * (1.0 / fVar6);
      *s1 = (1.0 / fVar6) * fVar1;
      goto LAB_0001023a;
    }
  }
                    /* Unresolved local var: MeReal s0num@[DW_OP_reg11(ST0)] */
  fVar4 = fVar5 * *s1 + fVar4;
  if (fVar4 < s0max) {
    s0max = fVar4;
  }
  if (s0max < s0min) {
    s0max = s0min;
  }
  *s0 = s0max;
LAB_0001023a:
  *ds0 = 0.0;
  return 1;
}


/* ==== NSegmentNSegment ==== */

MeBool NSegmentNSegment(MeReal *p0,MeReal *d0,MeReal s0min,MeReal s0max,MeReal *p1,MeReal *d1,
                       MeReal s1min,MeReal s1max,MeReal *s0,MeReal *s1,MeReal *ds0)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  MeReal MVar6;
  bool bVar7;
  MeReal d0r;
  MeReal det;
  MeReal d0d1;
  MeReal r [3];
  
                    /* Unresolved local var: MeReal d1r@[DW_OP_reg18(ST7)] */
  fVar4 = d0[2] * d1[2] + d0[1] * d1[1] + *d0 * *d1;
  fVar5 = 1.0 - fVar4 * fVar4;
  fVar3 = d0[2] * (p1[2] - p0[2]) + d0[1] * (p1[1] - p0[1]) + *d0 * (*p1 - *p0);
  fVar1 = d1[2] * (p1[2] - p0[2]) + (*p1 - *p0) * *d1 + d1[1] * (p1[1] - p0[1]);
  if (fVar5 <= 0.00025) {
                    /* Unresolved local var: MeReal s1SgnScaleMin@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal s1SgnScaleMax@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal s0min1@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal s0max1@[DW_OP_reg12(ST1)] */
    fVar5 = s1max;
    if (0.0 < fVar4) {
      fVar5 = s1min;
      s1min = s1max;
    }
    fVar2 = fVar4 * s1min + fVar3;
    fVar3 = fVar4 * fVar5 + fVar3;
    if (s0min < fVar2) {
      if (fVar3 < s0max) {
        if (s0min < fVar3) {
          s0min = fVar3;
        }
        *s0 = s0min;
        if (s0max < fVar2) {
          fVar2 = s0max;
        }
        *ds0 = fVar2 - *s0;
        *s1 = fVar4 * *s0 - fVar1;
      }
      else {
        *s0 = s0max;
        *ds0 = 0.0;
        *s1 = fVar5;
      }
    }
    else {
      *s0 = s0min;
      *ds0 = 0.0;
      *s1 = s1min;
    }
    return 0;
  }
                    /* Unresolved local var: MeReal s0numInf@[DW_OP_reg13(ST2)] */
  fVar2 = fVar3 - fVar4 * fVar1;
  MVar6 = s0min;
  if ((fVar2 < s0min * fVar5) || (bVar7 = false, MVar6 = s0max, s0max * fVar5 < fVar2)) {
    *s0 = MVar6;
    bVar7 = true;
  }
  if (bVar7) {
                    /* Unresolved local var: MeReal s1num@[DW_OP_reg11(ST0)] */
    fVar1 = fVar4 * *s0 - fVar1;
    if ((fVar1 < s1min) || (s1min = s1max, s1max < fVar1)) {
      bVar7 = true;
      *s1 = s1min;
    }
    else {
      bVar7 = false;
      *s1 = fVar1;
    }
    if (!bVar7) goto LAB_00010569;
                    /* Unresolved local var: MeReal s0num@[DW_OP_reg11(ST0)] */
    fVar1 = *s1;
  }
  else {
    fVar1 = fVar3 * fVar4 - fVar1;
    if ((fVar1 < s1min * fVar5) || (bVar7 = false, s1min = s1max, s1max * fVar5 < fVar1)) {
      bVar7 = true;
      *s1 = s1min;
    }
    if (!bVar7) {
                    /* Unresolved local var: MeReal invDet@[DW_OP_reg11(ST0)] */
      *s0 = fVar2 * (1.0 / fVar5);
      *s1 = (1.0 / fVar5) * fVar1;
      goto LAB_00010569;
    }
    fVar1 = *s1;
  }
                    /* Unresolved local var: MeReal s1numInf@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal s0num@[DW_OP_reg11(ST0)] */
  fVar3 = fVar4 * fVar1 + fVar3;
  if (fVar3 < s0max) {
    s0max = fVar3;
  }
  if (s0max < s0min) {
    s0max = s0min;
  }
  *s0 = s0max;
LAB_00010569:
  *ds0 = 0.0;
  return 1;
}


