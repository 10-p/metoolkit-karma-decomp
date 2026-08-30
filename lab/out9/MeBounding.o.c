/* ==== MeBoundingSphereCalc ==== */

void MeBoundingSphereCalc(MeVector3 *points,int numPoints,MeReal *centre,MeReal *radius)

{
  MeVector3 *paMVar1;
  MeVector3 *paMVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float *pfVar8;
  int iVar9;
  float __result;
  MeReal r2;
  MeVector3 cToP;
  MeVector3 diff;
  int maxIx [3];
  int minIx [3];
  MeVector3 max;
  MeVector3 min;
  
                    /* Unresolved local var: MeReal r@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal d2@[DW_OP_reg16(ST5)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[DW_OP_reg1(ECX)] */
  if (numPoints == 0) {
    return;
  }
  max[0] = (*points)[0];
  maxIx[0] = 0;
  minIx[0] = 0;
  max[1] = (*points)[1];
  maxIx[1] = 0;
  minIx[1] = 0;
  max[2] = (*points)[2];
  maxIx[0] = 1;
  maxIx[2] = 0;
  minIx[2] = 0;
  if (1 < numPoints) {
    iVar9 = 0xc;
    min[0] = max[0];
    min[1] = max[1];
    min[2] = max[2];
    do {
      pfVar8 = (float *)((int)*points + iVar9);
      if (min[0] <= *pfVar8) {
        if (max[0] < *pfVar8) {
          max[0] = *pfVar8;
        }
      }
      else {
        minIx[0] = maxIx[0];
        min[0] = *pfVar8;
      }
      if (min[1] <= pfVar8[1]) {
        if (max[1] < pfVar8[1]) {
          maxIx[1] = maxIx[0];
          max[1] = pfVar8[1];
        }
      }
      else {
        minIx[1] = maxIx[0];
        min[1] = pfVar8[1];
      }
      if (min[2] <= pfVar8[2]) {
        if (max[2] < pfVar8[2]) {
          maxIx[2] = maxIx[0];
          max[2] = pfVar8[2];
        }
      }
      else {
        minIx[2] = maxIx[0];
        min[2] = pfVar8[2];
      }
      maxIx[0] = maxIx[0] + 1;
      iVar9 = iVar9 + 0xc;
    } while (maxIx[0] < numPoints);
  }
  fVar5 = 0.0;
  iVar9 = 0;
  do {
                    /* Unresolved local var: MeReal tmpd2@[???] */
    paMVar1 = points + maxIx[iVar9];
    paMVar2 = points + minIx[iVar9];
    fVar3 = (*paMVar1)[0] - (*paMVar2)[0];
                    /* Unresolved local var: MeReal m@[DW_OP_reg12(ST1)] */
    fVar7 = (*paMVar1)[1] - (*paMVar2)[1];
    fVar6 = (*paMVar1)[2] - (*paMVar2)[2];
    fVar4 = fVar6 * fVar6 + fVar3 * fVar3 + fVar7 * fVar7;
    if (fVar5 < fVar4) {
      paMVar1 = points + minIx[iVar9];
      *centre = fVar3 * 0.5 + (*paMVar1)[0];
      centre[1] = fVar7 * 0.5 + (*paMVar1)[1];
      centre[2] = fVar6 * 0.5 + (*paMVar1)[2];
      fVar5 = fVar4;
    }
    iVar9 = iVar9 + 1;
  } while (iVar9 < 3);
                    /* Unresolved local var: float __result@[???] */
  fVar5 = SQRT(fVar5) * 0.5;
  r2 = fVar5 * fVar5;
  if (0 < numPoints) {
    if ((numPoints & 1U) != 0) {
                    /* Unresolved local var: MeReal pr2@[???] */
      fVar6 = (*points)[0] - *centre;
      fVar3 = (*points)[1] - centre[1];
                    /* Unresolved local var: MeReal m@[DW_OP_reg11(ST0)] */
      fVar7 = (*points)[2] - centre[2];
      fVar4 = fVar7 * fVar7 + fVar6 * fVar6 + fVar3 * fVar3;
      if (r2 < fVar4) {
                    /* Unresolved local var: MeReal pr@[???] */
        fVar4 = SQRT(fVar4);
        fVar5 = (fVar5 + fVar4) * 0.5;
        r2 = fVar5 * fVar5;
        fVar4 = (1.0 / fVar4) * (fVar4 - fVar5);
        *centre = fVar4 * fVar6 + *centre;
        centre[1] = centre[1] + fVar4 * fVar3;
        centre[2] = fVar4 * fVar7 + centre[2];
      }
      points = points + 1;
      numPoints = numPoints + -1;
      if (numPoints == 0) goto LAB_000103a9;
    }
    do {
      fVar6 = (*points)[0] - *centre;
      fVar4 = (*points)[1] - centre[1];
      fVar7 = (*points)[2] - centre[2];
      fVar3 = fVar7 * fVar7 + fVar6 * fVar6 + fVar4 * fVar4;
      if (r2 < fVar3) {
        fVar3 = SQRT(fVar3);
        fVar5 = (fVar5 + fVar3) * 0.5;
        r2 = fVar5 * fVar5;
        fVar3 = (1.0 / fVar3) * (fVar3 - fVar5);
        *centre = *centre + fVar6 * fVar3;
        centre[1] = centre[1] + fVar4 * fVar3;
        centre[2] = fVar3 * fVar7 + centre[2];
      }
      fVar6 = points[1][0] - *centre;
      fVar3 = points[1][1] - centre[1];
      fVar7 = points[1][2] - centre[2];
      fVar4 = fVar7 * fVar7 + fVar6 * fVar6 + fVar3 * fVar3;
      if (r2 < fVar4) {
        fVar4 = SQRT(fVar4);
        fVar5 = (fVar5 + fVar4) * 0.5;
        r2 = fVar5 * fVar5;
        fVar4 = (1.0 / fVar4) * (fVar4 - fVar5);
        *centre = fVar4 * fVar6 + *centre;
        centre[1] = centre[1] + fVar4 * fVar3;
        centre[2] = fVar4 * fVar7 + centre[2];
      }
      points = points + 2;
      numPoints = numPoints + -2;
    } while (numPoints != 0);
  }
LAB_000103a9:
  *radius = fVar5;
  return;
}


