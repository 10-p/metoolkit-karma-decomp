/* ==== MeQuaternionFromTM ==== */

void MeQuaternionFromTM(MeReal *q,MeReal (*tm) [4])

{
  int iVar1;
  int iVar2;
  float fVar3;
  float fVar4;
  int k;
  int i;
  MeReal qt [4];
  
                    /* Unresolved local var: MeReal * t@[???]
                       Unresolved local var: MeReal tr@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal s@[DW_OP_reg14(ST3)]
                       Unresolved local var: int j@[DW_OP_reg7(EDI)] */
  fVar3 = tm[1][1] + (*tm)[0] + tm[2][2];
  if (fVar3 <= 0.0) {
    i = (int)((*tm)[0] < tm[1][1]);
    if (*(float *)((int)tm + i * 0x14) < tm[2][2]) {
      i = 2;
    }
    iVar1 = MeQuaternionFromTM::nxt[i];
    iVar2 = MeQuaternionFromTM::nxt[iVar1];
    fVar3 = SQRT(((*(float *)((int)tm + i * 0x14) - *(float *)((int)tm + iVar1 * 0x14)) -
                 *(float *)((int)tm + iVar2 * 0x14)) + 1.0);
    qt[i] = fVar3 * 0.5;
    if (fVar3 != 0.0) {
      fVar3 = (1.0 / fVar3) * 0.5;
    }
    qt[3] = (tm[iVar1][iVar2] - tm[iVar2][iVar1]) * fVar3;
    qt[iVar1] = (tm[iVar1][i] + tm[i][iVar1]) * fVar3;
    qt[iVar2] = (tm[iVar2][i] + tm[i][iVar2]) * fVar3;
    q[1] = qt[0];
    q[2] = qt[1];
    q[3] = qt[2];
    *q = qt[3];
  }
  else {
    fVar3 = SQRT(fVar3 + 1.0);
    fVar4 = (1.0 / fVar3) * 0.5;
    *q = fVar3 * 0.5;
    q[1] = (tm[1][2] - tm[2][1]) * fVar4;
    q[2] = (tm[2][0] - (*tm)[2]) * fVar4;
    q[3] = ((*tm)[1] - tm[1][0]) * fVar4;
  }
  return;
}


/* ==== MeQuaternionSlerp ==== */

void MeQuaternionSlerp(MeReal *q,MeReal *from,MeReal *to,MeReal howFar)

{
  longdouble lVar1;
  longdouble lVar2;
  longdouble lVar3;
  longdouble lVar4;
  MeVector4 temp;
  
                    /* Unresolved local var: MeReal angle@[???]
                       Unresolved local var: MeReal c@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal s@[???]
                       Unresolved local var: MeReal startWeight@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal endWeight@[DW_OP_reg13(ST2)]
                       Unresolved local var: float __result@[???] */
  temp[0] = *to;
  temp[1] = to[1];
  temp[2] = to[2];
  temp[3] = to[3];
  lVar4 = (longdouble)from[2] * (longdouble)temp[2] +
          (longdouble)*from * (longdouble)temp[0] + (longdouble)from[1] * (longdouble)temp[1] +
          (longdouble)from[3] * (longdouble)temp[3];
  if (lVar4 < (longdouble)0.0) {
    lVar1 = (longdouble)-1.0;
    lVar4 = -lVar4;
    temp[0] = (MeReal)((longdouble)temp[0] * lVar1);
    temp[1] = (MeReal)((longdouble)temp[1] * lVar1);
    temp[2] = (MeReal)((longdouble)temp[2] * lVar1);
    temp[3] = (MeReal)((longdouble)temp[3] * lVar1);
  }
  lVar1 = (longdouble)1;
  if (lVar1 - lVar4 <= (longdouble)0.05) {
    lVar4 = (longdouble)howFar;
    lVar3 = lVar1 - (longdouble)howFar;
  }
  else {
                    /* Unresolved local var: longdouble __result@[???]
                       Unresolved local var: longdouble __value@[???] */
    lVar4 = (longdouble)fpatan(SQRT((longdouble)1 - lVar4 * lVar4),lVar4);
    lVar4 = (longdouble)(float)lVar4;
                    /* Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
    lVar2 = (longdouble)fsin(lVar4);
    lVar3 = (longdouble)fsin((lVar1 - (longdouble)howFar) * lVar4);
    lVar3 = lVar3 * (lVar1 / lVar2);
    lVar4 = (longdouble)fsin((longdouble)howFar * lVar4);
    lVar4 = lVar4 * (lVar1 / lVar2);
  }
  q[1] = (float)(lVar4 * (longdouble)temp[1] + (longdouble)from[1] * lVar3);
  q[2] = (float)(lVar3 * (longdouble)from[2] + lVar4 * (longdouble)temp[2]);
  q[3] = (float)(lVar3 * (longdouble)from[3] + lVar4 * (longdouble)temp[3]);
  *q = (float)(lVar4 * (longdouble)temp[0] + lVar3 * (longdouble)*from);
  return;
}


/* ==== MeMatrix3SymmetricInvert ==== */

MeBool MeMatrix3SymmetricInvert(MeVector3 *a)

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
  
                    /* Unresolved local var: MeReal r@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal a00@[???]
                       Unresolved local var: MeReal a10@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal a20@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal a11@[???]
                       Unresolved local var: MeReal a21@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal a22@[???] */
  fVar1 = a[1][0];
  if ((fVar1 == (*a)[1]) && (fVar2 = a[2][0], fVar2 == (*a)[2])) {
    fVar3 = a[2][1];
    if (fVar3 == a[1][2]) {
      fVar4 = a[1][1];
      fVar5 = a[2][2];
      fVar6 = (*a)[0];
      fVar9 = (fVar1 * fVar3 - fVar4 * fVar2) * fVar2 +
              (fVar4 * fVar5 - fVar3 * fVar3) * fVar6 + (fVar3 * fVar2 - fVar1 * fVar5) * fVar1;
      if (fVar9 == 0.0) {
        return 0;
      }
      fVar9 = 1.0 / fVar9;
      fVar8 = (fVar3 * fVar2 - fVar1 * fVar5) * fVar9;
      fVar7 = (fVar1 * fVar3 - fVar4 * fVar2) * fVar9;
      fVar10 = (fVar2 * fVar1 - fVar3 * fVar6) * fVar9;
      (*a)[0] = (fVar4 * fVar5 - a[1][2] * fVar3) * fVar9;
      a[1][0] = fVar8;
      a[2][0] = fVar7;
      a[1][1] = (fVar5 * fVar6 - fVar2 * fVar2) * fVar9;
      a[2][1] = fVar10;
      (*a)[1] = fVar8;
      a[2][2] = fVar9 * (fVar6 * fVar4 - fVar1 * fVar1);
      (*a)[2] = fVar7;
      a[1][2] = fVar10;
      return 1;
    }
  }
  return 0;
}


/* ==== MeMatrix3LUDecompose ==== */

void MeMatrix3LUDecompose(MeVector3 *L,MeVector3 *U,MeReal (*a) [3],uint *SwappedRow,MeReal *scale)

{
  float fVar1;
  MeReal MVar2;
  MeReal MVar3;
  MeReal MVar4;
  float fVar5;
  MeVector3 temp_1;
  MeVector3 temp;
  MeMatrix3 b;
  
                    /* Unresolved local var: uint i@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal r@[DW_OP_reg12(ST1)] */
  *SwappedRow = 0;
  b[2][0] = (*a)[0];
  b[2][1] = (*a)[1];
  b[2][2] = (*a)[2];
  b[1][0] = a[1][0];
  b[1][1] = a[1][1];
  b[1][2] = a[1][2];
  MVar2 = a[2][0];
  MVar3 = a[2][1];
  MVar4 = a[2][2];
  (*L)[1] = 0.0;
  (*L)[2] = 0.0;
  L[1][2] = 0.0;
  U[1][0] = 0.0;
  U[2][0] = 0.0;
  U[2][1] = 0.0;
                    /* Unresolved local var: MeReal temp@[???] */
  fVar5 = ABS((*a)[1]);
  fVar1 = (*a)[0];
  *scale = fVar5;
  if (fVar5 < ABS(fVar1)) {
    fVar5 = ABS(fVar1);
  }
  *scale = fVar5;
  if (fVar5 < ABS((*a)[2])) {
    fVar5 = ABS((*a)[2]);
  }
  *scale = fVar5;
  fVar5 = ABS(a[1][1]);
  fVar1 = a[1][0];
  scale[1] = fVar5;
  if (fVar5 < ABS(fVar1)) {
    fVar5 = ABS(fVar1);
  }
  scale[1] = fVar5;
  if (fVar5 < ABS(a[1][2])) {
    fVar5 = ABS(a[1][2]);
  }
  scale[1] = fVar5;
  fVar5 = ABS(a[2][1]);
  fVar1 = a[2][0];
  scale[2] = fVar5;
  if (fVar5 < ABS(fVar1)) {
    fVar5 = ABS(fVar1);
  }
  scale[2] = fVar5;
  if (fVar5 < ABS(a[2][2])) {
    fVar5 = ABS(a[2][2]);
  }
  scale[2] = fVar5;
  if (*scale != 0.0) {
    *scale = 1.0 / *scale;
  }
  if (scale[1] != 0.0) {
    scale[1] = 1.0 / scale[1];
  }
  if (scale[2] != 0.0) {
    scale[2] = 1.0 / scale[2];
  }
  fVar1 = *scale;
  b[0][0] = b[2][0];
  b[0][1] = b[2][1];
  b[0][2] = b[2][2];
  if (ABS((*a)[0] * fVar1) < 1.2e-07) {
    if (ABS(a[1][0] * scale[1]) < 1.2e-07) {
      if (ABS(a[2][0] * scale[2]) < 1.2e-07) goto LAB_00010661;
                    /* Unresolved local var: MeReal temp@[???] */
      *scale = scale[2];
      scale[2] = fVar1;
      *SwappedRow = 2;
      b[0][0] = MVar2;
      b[0][1] = MVar3;
      b[0][2] = MVar4;
    }
    else {
                    /* Unresolved local var: MeReal temp@[???] */
      *scale = scale[1];
      scale[1] = fVar1;
      *SwappedRow = 1;
      b[0][0] = b[1][0];
      b[0][1] = b[1][1];
      b[0][2] = b[1][2];
      b[1][0] = b[2][0];
      b[1][1] = b[2][1];
      b[1][2] = b[2][2];
      b[2][0] = MVar2;
      b[2][1] = MVar3;
      b[2][2] = MVar4;
    }
    fVar1 = *scale;
    MVar2 = b[2][0];
    MVar3 = b[2][1];
    MVar4 = b[2][2];
  }
LAB_00010661:
  b[2][2] = MVar4;
  b[2][1] = MVar3;
  b[2][0] = MVar2;
  MVar4 = b[2][2];
  MVar3 = b[2][1];
  MVar2 = b[1][0];
  if (((fVar1 != 0.0) && (fVar5 = scale[1], fVar5 != 0.0)) &&
     (ABS(fVar1 * (b[0][0] * b[1][1] - b[0][1] * b[1][0]) * fVar5) < 1.2e-07)) {
    b[1][0] = b[2][0];
    b[2][0] = MVar2;
    b[2][2] = b[1][2];
    b[1][2] = MVar4;
    b[2][1] = b[1][1];
                    /* Unresolved local var: MeReal temp@[???] */
    MVar2 = scale[2];
    scale[2] = fVar5;
    scale[1] = MVar2;
    *SwappedRow = *SwappedRow + 3;
    b[1][1] = MVar3;
  }
  fVar1 = 0.0;
  if (b[0][0] != 0.0) {
    fVar1 = 1.0 / b[0][0];
  }
  (*L)[0] = 1.0;
  (*U)[0] = b[0][0];
  L[1][1] = 1.0;
  L[1][0] = b[1][0] * fVar1;
  (*U)[1] = b[0][1];
  L[2][2] = 1.0;
  L[2][0] = fVar1 * b[2][0];
  (*U)[2] = b[0][2];
  fVar1 = b[1][1] - b[0][1] * L[1][0];
  U[1][1] = fVar1;
  U[1][2] = b[1][2] - b[0][2] * L[1][0];
  if ((scale[1] == 0.0) || (ABS(scale[1] * fVar1) < 1.2e-07)) {
    L[2][1] = 0.0;
    fVar1 = L[2][0];
  }
  else {
    fVar1 = L[2][0];
    L[2][1] = (b[2][1] - b[0][1] * fVar1) / U[1][1];
  }
  U[2][2] = (b[2][2] - fVar1 * b[0][2]) - L[2][1] * U[1][2];
  return;
}


/* ==== MeMatrix3Invert ==== */

MeBool MeMatrix3Invert(MeVector3 *a)

{
  MeReal *pMVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  MeReal MVar10;
  MeReal MVar11;
  MeReal MVar12;
  MeBool MVar13;
  uint swap;
  MeMatrix3 U;
  MeMatrix3 L;
  MeVector3 scale;
  
                    /* Unresolved local var: uint i@[???] */
  MVar13 = 0;
  MeMatrix3LUDecompose(L,U,a,&swap,scale);
  MVar11 = L[1][2];
  MVar10 = L[0][1];
  MVar12 = L[0][0];
  if (1.2e-07 <= ABS(scale[0] * U[0][0])) {
    MVar13 = 0;
    fVar2 = 1.0 / U[0][0];
    if (1.2e-07 <= ABS(scale[1] * U[1][1])) {
      MVar13 = 0;
      fVar4 = 1.0 / U[1][1];
      if (1.2e-07 <= ABS(scale[2] * U[2][2])) {
        fVar3 = 1.0 / U[2][2];
        fVar5 = -fVar2 * fVar4 * U[0][1];
        fVar7 = fVar2 * fVar4 * fVar3 * (U[1][2] * U[0][1] - U[1][1] * U[0][2]);
        fVar6 = -fVar4 * fVar3 * U[1][2];
        fVar8 = L[1][0] * L[2][1];
        fVar9 = -L[2][1];
        L[1][0] = -L[1][0];
        L[2][1] = fVar9;
        L[2][0] = fVar8 - L[2][0];
        if (2 < swap) {
                    /* Unresolved local var: uint i@[???]
                       Unresolved local var: MeReal temp@[???] */
          L[0][1] = L[0][2];
          L[0][2] = MVar10;
          L[1][2] = L[1][1];
          L[1][1] = MVar11;
          swap = swap - 3;
          L[2][1] = L[2][2];
          L[2][2] = fVar9;
        }
        if (swap != 0) {
                    /* Unresolved local var: uint i@[???] */
          pMVar1 = L[0] + swap;
                    /* Unresolved local var: MeReal temp@[???] */
          L[0][0] = *pMVar1;
          *pMVar1 = MVar12;
          MVar12 = L[1][0];
          L[1][0] = L[1][swap];
          L[1][swap] = MVar12;
          MVar12 = L[2][swap];
          L[2][swap] = L[2][0];
          L[2][0] = MVar12;
        }
        MVar13 = 1;
        (*a)[0] = L[2][0] * fVar7 + L[1][0] * fVar5 + fVar2 * L[0][0];
        (*a)[1] = fVar7 * L[2][1] + L[1][1] * fVar5 + fVar2 * L[0][1];
        (*a)[2] = fVar2 * L[0][2] + fVar5 * L[1][2] + fVar7 * L[2][2];
        a[1][0] = L[2][0] * fVar6 + U[1][0] * L[0][0] + L[1][0] * fVar4;
        a[1][1] = fVar6 * L[2][1] + L[1][1] * fVar4 + U[1][0] * L[0][1];
        a[1][2] = fVar6 * L[2][2] + U[1][0] * L[0][2] + fVar4 * L[1][2];
        a[2][0] = L[0][0] * U[2][0] + L[1][0] * U[2][1] + L[2][0] * fVar3;
        a[2][1] = L[1][1] * U[2][1] + L[0][1] * U[2][0] + L[2][1] * fVar3;
        a[2][2] = U[2][0] * L[0][2] + U[2][1] * L[1][2] + fVar3 * L[2][2];
      }
    }
  }
  return MVar13;
}


/* ==== MeMatrix4TMUpdateFromVelocities ==== */

void MeMatrix4TMUpdateFromVelocities
               (MeVector4 *aTransformReturn,MeReal aEpsilon,MeReal aTimeStep,MeReal *aVelocity,
               MeReal *aAngularVelocity,MeReal (*aTransform) [4])

{
  float fVar1;
  float fVar2;
  float fVar3;
  MeVector4 *paMVar4;
  undefined1 *puVar5;
  int iVar6;
  int iVar7;
  longdouble lVar8;
  longdouble lVar9;
  MeReal eR [3] [3];
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal t@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal f@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal h@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal w@[???] */
  lVar8 = (longdouble)aTimeStep;
                    /* Unresolved local var: float __result@[DW_OP_reg14(ST3)] */
  lVar9 = SQRT((longdouble)aAngularVelocity[2] * (longdouble)aAngularVelocity[2] +
               (longdouble)aAngularVelocity[1] * (longdouble)aAngularVelocity[1] +
               (longdouble)*aAngularVelocity * (longdouble)*aAngularVelocity);
  if ((lVar8 < (longdouble)aEpsilon) || (lVar9 < (longdouble)aEpsilon)) {
    (*aTransformReturn)[0] = (*aTransform)[0];
    (*aTransformReturn)[1] = (*aTransform)[1];
    (*aTransformReturn)[2] = (*aTransform)[2];
    (*aTransformReturn)[3] = (*aTransform)[3];
    aTransformReturn[1][0] = aTransform[1][0];
    aTransformReturn[1][1] = aTransform[1][1];
    aTransformReturn[1][2] = aTransform[1][2];
    aTransformReturn[1][3] = aTransform[1][3];
    aTransformReturn[2][0] = aTransform[2][0];
    aTransformReturn[2][1] = aTransform[2][1];
    aTransformReturn[2][2] = aTransform[2][2];
    aTransformReturn[2][3] = aTransform[2][3];
    aTransformReturn[3][0] = aTransform[3][0];
    aTransformReturn[3][1] = aTransform[3][1];
    aTransformReturn[3][2] = aTransform[3][2];
    aTransformReturn[3][3] = aTransform[3][3];
    aTransformReturn[3][0] = (float)((longdouble)*aVelocity * lVar8 + (longdouble)aTransform[3][0]);
    aTransformReturn[3][1] =
         (float)((longdouble)aVelocity[1] * lVar8 + (longdouble)aTransform[3][1]);
    aTransformReturn[3][2] =
         (float)(lVar8 * (longdouble)aVelocity[2] + (longdouble)aTransform[3][2]);
  }
  else {
    aTransformReturn[3][0] = (float)((longdouble)*aVelocity * lVar8 + (longdouble)aTransform[3][0]);
    aTransformReturn[3][1] =
         (float)((longdouble)aVelocity[1] * lVar8 + (longdouble)aTransform[3][1]);
    fVar1 = aVelocity[2];
    lVar9 = lVar8 * lVar9;
    fVar2 = aTransform[3][2];
    aTransformReturn[3][3] = 1.0;
    (*aTransformReturn)[3] = 0.0;
    aTransformReturn[1][3] = 0.0;
    aTransformReturn[3][2] = (float)((longdouble)fVar1 * lVar8 + (longdouble)fVar2);
    aTransformReturn[2][3] = 0.0;
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
    fcos(lVar9);
                    /* Unresolved local var: float __result@[???] */
    fsin(lVar9);
    puVar5 = &stack0xfffffff4;
    iVar6 = 0;
    do {
                    /* Unresolved local var: int j@[???] */
      fVar1 = *(float *)(puVar5 + -0x30);
      fVar2 = *(float *)(puVar5 + -0x2c);
      fVar3 = *(float *)(puVar5 + -0x28);
      iVar7 = iVar6 + 1;
      puVar5 = puVar5 + 0xc;
      paMVar4 = aTransformReturn + iVar6;
      (*paMVar4)[0] = fVar3 * aTransform[2][0] + fVar2 * aTransform[1][0] + fVar1 * (*aTransform)[0]
      ;
      (*paMVar4)[1] = fVar3 * aTransform[2][1] + fVar1 * (*aTransform)[1] + fVar2 * aTransform[1][1]
      ;
      (*paMVar4)[2] = fVar3 * aTransform[2][2] + fVar1 * (*aTransform)[2] + fVar2 * aTransform[1][2]
      ;
      iVar6 = iVar7;
    } while (iVar7 < 3);
  }
  return;
}


/* ==== MeMatrix4TMUpdateFromVelocitiesAndAcceler ==== */

void MeMatrix4TMUpdateFromVelocitiesAndAcceler
               (MeVector4 *aTransformReturn,MeReal aEpsilon,MeReal aTimeStep,MeReal *aVelocity,
               MeReal *aAcceler,MeReal *aAngularVelocity,MeReal *aAngularAcceler,
               MeReal (*aTransform) [4])

{
  float fVar1;
  float fVar2;
  float fVar3;
  undefined1 *puVar4;
  int iVar5;
  int iVar6;
  MeVector4 *paMVar7;
  longdouble lVar8;
  longdouble lVar9;
  longdouble lVar10;
  MeReal h;
  MeVector3 eAngular;
  MeReal eR [3] [3];
  undefined1 local_1c [12];
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal t@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal f@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal w@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal a@[???] */
  lVar8 = (longdouble)aTimeStep;
                    /* Unresolved local var: float __result@[???] */
  lVar10 = SQRT((longdouble)aAngularVelocity[2] * (longdouble)aAngularVelocity[2] +
                (longdouble)*aAngularVelocity * (longdouble)*aAngularVelocity +
                (longdouble)aAngularVelocity[1] * (longdouble)aAngularVelocity[1]) +
           SQRT((longdouble)aAngularAcceler[2] * (longdouble)aAngularAcceler[2] +
                (longdouble)aAngularAcceler[1] * (longdouble)aAngularAcceler[1] +
                (longdouble)*aAngularAcceler * (longdouble)*aAngularAcceler) * lVar8;
  if ((lVar8 < (longdouble)aEpsilon) || (lVar10 < (longdouble)aEpsilon)) {
    (*aTransformReturn)[0] = (*aTransform)[0];
    (*aTransformReturn)[1] = (*aTransform)[1];
    (*aTransformReturn)[2] = (*aTransform)[2];
    (*aTransformReturn)[3] = (*aTransform)[3];
    aTransformReturn[1][0] = aTransform[1][0];
    aTransformReturn[1][1] = aTransform[1][1];
    aTransformReturn[1][2] = aTransform[1][2];
    aTransformReturn[1][3] = aTransform[1][3];
    aTransformReturn[2][0] = aTransform[2][0];
    aTransformReturn[2][1] = aTransform[2][1];
    aTransformReturn[2][2] = aTransform[2][2];
    aTransformReturn[2][3] = aTransform[2][3];
    aTransformReturn[3][0] = aTransform[3][0];
    aTransformReturn[3][1] = aTransform[3][1];
    aTransformReturn[3][2] = aTransform[3][2];
    aTransformReturn[3][3] = aTransform[3][3];
    lVar10 = (longdouble)0.5;
    aTransformReturn[3][0] =
         (float)((longdouble)*aAcceler * lVar8 * lVar8 * lVar10 +
                (longdouble)*aVelocity * lVar8 + (longdouble)aTransform[3][0]);
    aTransformReturn[3][1] =
         (float)((longdouble)aAcceler[1] * lVar8 * lVar8 * lVar10 +
                (longdouble)aVelocity[1] * lVar8 + (longdouble)aTransform[3][1]);
    aTransformReturn[3][2] =
         (float)((longdouble)aAcceler[2] * lVar8 * lVar8 * lVar10 +
                (longdouble)aVelocity[2] * lVar8 + (longdouble)aTransform[3][2]);
  }
  else {
    lVar9 = (longdouble)0.5;
    aTransformReturn[3][0] =
         (float)((longdouble)*aAcceler * lVar8 * lVar8 * lVar9 +
                (longdouble)*aVelocity * lVar8 + (longdouble)aTransform[3][0]);
    aTransformReturn[3][1] =
         (float)((longdouble)aAcceler[1] * lVar8 * lVar8 * lVar9 +
                (longdouble)aVelocity[1] * lVar8 + (longdouble)aTransform[3][1]);
    fVar1 = aAcceler[2];
    fVar2 = aVelocity[2];
    fVar3 = aTransform[3][2];
    aTransformReturn[3][3] = 1.0;
    (*aTransformReturn)[3] = 0.0;
    aTransformReturn[1][3] = 0.0;
    aTransformReturn[2][3] = 0.0;
    aTransformReturn[3][2] =
         (float)((longdouble)fVar2 * lVar8 + (longdouble)fVar3 +
                (longdouble)fVar1 * lVar8 * lVar8 * lVar9);
    lVar10 = lVar10 * lVar8;
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
    fcos(lVar10);
                    /* Unresolved local var: float __result@[???] */
    fsin(lVar10);
    puVar4 = local_1c;
    iVar5 = 0;
    do {
                    /* Unresolved local var: int j@[???] */
      fVar1 = *(float *)(puVar4 + -0x30);
      fVar2 = *(float *)(puVar4 + -0x2c);
      fVar3 = *(float *)(puVar4 + -0x28);
      iVar6 = iVar5 + 1;
      puVar4 = puVar4 + 0xc;
      paMVar7 = aTransformReturn + iVar5;
      (*paMVar7)[0] = fVar3 * aTransform[2][0] + fVar2 * aTransform[1][0] + fVar1 * (*aTransform)[0]
      ;
      (*paMVar7)[1] = fVar3 * aTransform[2][1] + fVar1 * (*aTransform)[1] + fVar2 * aTransform[1][1]
      ;
      (*paMVar7)[2] = fVar3 * aTransform[2][2] + fVar1 * (*aTransform)[2] + fVar2 * aTransform[1][2]
      ;
      iVar5 = iVar6;
    } while (iVar6 < 3);
  }
  return;
}


/* ==== MeVector3AreaOfTriangle ==== */

MeReal MeVector3AreaOfTriangle(MeReal *v1,MeReal *v2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  
                    /* Unresolved local var: MeReal m@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[???] */
  fVar1 = *v1;
  fVar2 = v1[1];
  fVar3 = v1[2];
                    /* Unresolved local var: MeReal m@[DW_OP_reg13(ST2)] */
  fVar4 = *v2;
  fVar5 = v2[1];
  fVar6 = v2[2];
  fVar7 = fVar1 * fVar4 + fVar5 * fVar2 + fVar6 * fVar3;
  return SQRT((fVar4 * fVar4 + fVar5 * fVar5 + fVar6 * fVar6) *
              (fVar1 * fVar1 + fVar2 * fVar2 + fVar3 * fVar3) - fVar7 * fVar7) * 0.5;
}


/* ==== MeVector3Swap ==== */

void MeVector3Swap(MeReal *a,MeReal *b)

{
  MeReal MVar1;
  MeReal MVar2;
  MeReal MVar3;
  MeVector3 temp;
  
  MVar1 = *a;
  *a = *b;
  MVar2 = a[1];
  MVar3 = a[2];
  a[1] = b[1];
  a[2] = b[2];
  *b = MVar1;
  b[1] = MVar2;
  b[2] = MVar3;
  return;
}


/* ==== MeQuaternionFiniteRotation ==== */

void MeQuaternionFiniteRotation(MeReal *q,MeReal *w,MeReal h)

{
  float fVar1;
  float fVar2;
  int iVar3;
  longdouble lVar4;
  longdouble lVar5;
  longdouble lVar6;
  longdouble lVar7;
  longdouble lVar8;
  longdouble lVar9;
  longdouble lVar10;
  longdouble lVar11;
  float __result;
  MeReal newq0;
  MeVector3 nw;
  MeVector3 tmp;
  
                    /* Unresolved local var: int i@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeReal s@[???]
                       Unresolved local var: MeReal c@[???]
                       Unresolved local var: MeReal wlen1@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal wlen@[???]
                       Unresolved local var: float __result@[DW_OP_reg17(ST6)] */
                    /* Unresolved local var: MeReal m@[???]
                       Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
  lVar4 = (longdouble)*w;
  lVar5 = (longdouble)w[1];
  lVar6 = (longdouble)w[2];
  lVar7 = SQRT(lVar6 * lVar6 + lVar5 * lVar5 + lVar4 * lVar4);
  if ((longdouble)0.0001 <= ABS(lVar7)) {
    lVar8 = (longdouble)1 / lVar7;
    lVar4 = lVar4 * lVar8;
    lVar6 = lVar8 * lVar6;
    fVar1 = (float)(lVar5 * lVar8);
    lVar7 = lVar7 * (longdouble)h * (longdouble)0.5;
    nw[0] = (float)lVar4;
    nw[1] = (float)(lVar5 * lVar8);
    nw[2] = (float)lVar6;
    lVar5 = (longdouble)fsin(lVar7);
    lVar7 = (longdouble)fcos(lVar7);
    lVar9 = (longdouble)*q;
    iVar3 = 0;
    lVar11 = lVar7 * lVar9;
    lVar8 = (longdouble)q[2];
    fVar2 = q[1];
    lVar10 = (longdouble)q[3];
    tmp[0] = (float)((longdouble)fVar1 * lVar10 - lVar6 * lVar8);
    tmp[1] = (float)(lVar6 * (longdouble)q[1] - lVar10 * lVar4);
    tmp[2] = (float)(lVar4 * lVar8 - (longdouble)fVar1 * (longdouble)q[1]);
    while( true ) {
      q[iVar3 + 1] = (float)(lVar5 * (longdouble)tmp[iVar3] +
                            (longdouble)(float)lVar7 * (longdouble)q[iVar3 + 1] +
                            lVar9 * lVar5 * (longdouble)nw[iVar3]);
      iVar3 = iVar3 + 1;
      if (2 < iVar3) break;
      lVar9 = (longdouble)*q;
    }
    *q = (float)((longdouble)(float)lVar11 -
                (lVar10 * lVar6 + (longdouble)fVar2 * lVar4 + (longdouble)fVar1 * lVar8) * lVar5);
  }
  return;
}


/* ==== MeQuaternionForRotation ==== */

void MeQuaternionForRotation(MeReal *q,MeReal *v1,MeReal *v2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  MeReal cosRotationAngle;
  MeVector3 normalized;
  MeVector3 rotationAxis;
  
                    /* Unresolved local var: MeReal dot@[DW_OP_reg12(ST1)]
                       Unresolved local var: float __result@[???] */
  fVar1 = *v1;
  fVar2 = v1[1];
  fVar3 = *v2;
  fVar4 = v2[1];
  fVar5 = v1[2];
  fVar6 = v2[2];
  fVar7 = fVar5 * fVar6 + fVar1 * fVar3 + fVar2 * fVar4;
  if ((ABS(fVar7 - 1.0) < 1e-06) || (ABS(-1.0 - fVar7) < 1e-06)) {
    normalized[0] = fVar1;
    normalized[1] = fVar2;
    normalized[2] = fVar5;
    MeVector3Normalize(normalized);
    if ((0.5 <= normalized[2]) || (normalized[2] <= -0.5)) {
      rotationAxis[0] = -normalized[2];
      rotationAxis[1] = 0.0;
      rotationAxis[2] = normalized[0];
    }
    else {
      rotationAxis[0] = -normalized[1];
      rotationAxis[1] = normalized[0];
      rotationAxis[2] = 0.0;
    }
  }
  else {
    rotationAxis[0] = fVar2 * fVar6 - fVar5 * fVar4;
    rotationAxis[1] = fVar5 * fVar3 - fVar6 * fVar1;
    rotationAxis[2] = fVar1 * fVar4 - fVar2 * fVar3;
  }
  MeVector3Normalize(rotationAxis);
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
  fVar1 = SQRT(0.5 - fVar7 * 0.5);
  *q = SQRT(fVar7 * 0.5 + 0.5);
  q[1] = rotationAxis[0] * fVar1;
  q[2] = rotationAxis[1] * fVar1;
  q[3] = rotationAxis[2] * fVar1;
  return;
}


/* ==== MeMatrixFPrint ==== */

void MeMatrixFPrint(FILE *file,MeReal *A,int n,int m,char *format)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  float *pfVar4;
  int iVar5;
  char *pcVar6;
  double dVar7;
  int width;
  int i;
  char s [100];
  
                    /* Unresolved local var: int j@[DW_OP_reg7(EDI)] */
  sprintf(s,format,0);
  pcVar6 = s;
  do {
    width = (int)pcVar6;
    uVar1 = *(uint *)width + 0xfefefeff & (*(uint *)width ^ 0xffffffff);
    uVar2 = uVar1 & 0x80808080;
    pcVar6 = (char *)(width + 4);
  } while (uVar2 == 0);
  iVar5 = width + 4;
  if ((uVar1 & 0x8080) == 0) {
    uVar2 = uVar2 >> 0x10;
    iVar5 = width + 6;
  }
  width = iVar5;
  uVar1 = ((width + -3) - (uint)CARRY1((byte)uVar2,(byte)uVar2)) - (int)s;
  i = 0;
  if (0 < n) {
    do {
      iVar5 = 0;
      if (0 < m) {
        iVar3 = m + -1;
        pfVar4 = A + i;
        if ((m < 2) || ((m & 1U) != 0)) {
          if (*pfVar4 == 0.0) {
            dVar7 = (double)(ulonglong)uVar1;
            pcVar6 = "%*d";
          }
          else {
            dVar7 = (double)*pfVar4;
            pcVar6 = format;
          }
          fprintf((FILE *)file,pcVar6,dVar7);
          if (0 < iVar3) {
            fputc(0x20,(FILE *)file);
          }
          iVar5 = 1;
          pfVar4 = pfVar4 + n;
          if (m < 2) goto LAB_0001165a;
        }
        do {
          if (*pfVar4 == 0.0) {
            dVar7 = (double)(ulonglong)uVar1;
            pcVar6 = "%*d";
          }
          else {
            dVar7 = (double)*pfVar4;
            pcVar6 = format;
          }
          fprintf((FILE *)file,pcVar6,dVar7);
          if (iVar5 < iVar3) {
            fputc(0x20,(FILE *)file);
          }
          pfVar4 = pfVar4 + n;
          if (*pfVar4 == 0.0) {
            dVar7 = (double)(ulonglong)uVar1;
            pcVar6 = "%*d";
          }
          else {
            dVar7 = (double)*pfVar4;
            pcVar6 = format;
          }
          fprintf((FILE *)file,pcVar6,dVar7);
          if (iVar5 + 1 < iVar3) {
            fputc(0x20,(FILE *)file);
          }
          iVar5 = iVar5 + 2;
          pfVar4 = pfVar4 + n;
        } while (iVar5 < m);
      }
LAB_0001165a:
      fputc(10,(FILE *)file);
      i = i + 1;
    } while (i < n);
  }
  return;
}


/* ==== MeMatrixPrint ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeMatrixPrint(MeReal *A,int n,int m,char *format)

{
  FILE *__stream;
  uint uVar1;
  uint uVar2;
  int iVar3;
  float *pfVar4;
  int iVar5;
  char *pcVar6;
  double dVar7;
  int width;
  int i;
  char s [100];
  
  __stream = _stdout;
                    /* Unresolved local var: int j@[DW_OP_reg7(EDI)] */
  sprintf(s,format,0);
  pcVar6 = s;
  do {
    width = (int)pcVar6;
    uVar1 = *(uint *)width + 0xfefefeff & (*(uint *)width ^ 0xffffffff);
    uVar2 = uVar1 & 0x80808080;
    pcVar6 = (char *)(width + 4);
  } while (uVar2 == 0);
  iVar5 = width + 4;
  if ((uVar1 & 0x8080) == 0) {
    uVar2 = uVar2 >> 0x10;
    iVar5 = width + 6;
  }
  width = iVar5;
  uVar1 = ((width + -3) - (uint)CARRY1((byte)uVar2,(byte)uVar2)) - (int)s;
  i = 0;
  if (0 < n) {
    do {
      iVar5 = 0;
      if (0 < m) {
        iVar3 = m + -1;
        pfVar4 = A + i;
        if ((m < 2) || ((m & 1U) != 0)) {
          if (*pfVar4 == 0.0) {
            dVar7 = (double)(ulonglong)uVar1;
            pcVar6 = "%*d";
          }
          else {
            dVar7 = (double)*pfVar4;
            pcVar6 = format;
          }
          fprintf(__stream,pcVar6,dVar7);
          if (0 < iVar3) {
            fputc(0x20,__stream);
          }
          iVar5 = 1;
          pfVar4 = pfVar4 + n;
          if (m < 2) goto LAB_000118bf;
        }
        do {
          if (*pfVar4 == 0.0) {
            dVar7 = (double)(ulonglong)uVar1;
            pcVar6 = "%*d";
          }
          else {
            dVar7 = (double)*pfVar4;
            pcVar6 = format;
          }
          fprintf(__stream,pcVar6,dVar7);
          if (iVar5 < iVar3) {
            fputc(0x20,__stream);
          }
          pfVar4 = pfVar4 + n;
          if (*pfVar4 == 0.0) {
            dVar7 = (double)(ulonglong)uVar1;
            pcVar6 = "%*d";
          }
          else {
            dVar7 = (double)*pfVar4;
            pcVar6 = format;
          }
          fprintf(__stream,pcVar6,dVar7);
          if (iVar5 + 1 < iVar3) {
            fputc(0x20,__stream);
          }
          iVar5 = iVar5 + 2;
          pfVar4 = pfVar4 + n;
        } while (iVar5 < m);
      }
LAB_000118bf:
      fputc(10,__stream);
      i = i + 1;
    } while (i < n);
  }
  return;
}


/* ==== MeMatrix3IsIdentity ==== */

MeBool MeMatrix3IsIdentity(MeReal (*a) [3],MeReal tolerance)

{
  float fVar1;
  uint uVar2;
  MeVector3 Test;
  
                    /* Unresolved local var: uint i@[DW_OP_reg2(EDX)] */
  uVar2 = 0;
                    /* Unresolved local var: int i@[???] */
  Test[0] = 0.0;
  Test[1] = 0.0;
  Test[2] = 0.0;
  while( true ) {
    Test[uVar2] = 1.0;
    fVar1 = Test[0];
    if (tolerance <= ABS((*a)[0])) {
      fVar1 = (Test[0] - (*a)[0]) / (*a)[0];
    }
    if (tolerance <= ABS(fVar1)) {
      return 0;
    }
    fVar1 = Test[1];
    if (tolerance <= ABS((*a)[1])) {
      fVar1 = (Test[1] - (*a)[1]) / (*a)[1];
    }
    if (tolerance <= ABS(fVar1)) break;
    fVar1 = Test[2];
    if (tolerance <= ABS((*a)[2])) {
      fVar1 = (Test[2] - (*a)[2]) / (*a)[2];
    }
    if (tolerance <= ABS(fVar1)) {
      return 0;
    }
    Test[uVar2] = 0.0;
    uVar2 = uVar2 + 1;
    a = a + 1;
    if (2 < uVar2) {
      return 1;
    }
  }
  return 0;
}


/* ==== MeMatrix3SwapColumns ==== */

void MeMatrix3SwapColumns(MeVector3 *a,uint col1,uint col2)

{
  MeReal *pMVar1;
  MeReal *pMVar2;
  MeReal MVar3;
  
                    /* Unresolved local var: uint i@[???] */
  pMVar1 = *a + col2;
  pMVar2 = *a + col1;
                    /* Unresolved local var: MeReal temp@[???] */
  MVar3 = *pMVar2;
  *pMVar2 = *pMVar1;
  *pMVar1 = MVar3;
  MVar3 = pMVar2[3];
  pMVar2[3] = pMVar1[3];
  pMVar1[3] = MVar3;
  MVar3 = pMVar2[6];
  pMVar2[6] = pMVar1[6];
  pMVar1[6] = MVar3;
  return;
}


/* ==== MeMatrix3MakeRotationX ==== */

void MeMatrix3MakeRotationX(MeMatrix3Ptr m,MeReal a)

{
  longdouble lVar1;
  longdouble lVar2;
  
                    /* Unresolved local var: MeReal s@[???]
                       Unresolved local var: MeReal c@[???] */
                    /* Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
  lVar1 = (longdouble)fsin((longdouble)a);
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
  lVar2 = (longdouble)fcos((longdouble)a);
  (*m)[0] = 1.0;
  (*m)[1] = 0.0;
  (*m)[2] = 0.0;
  m[1][0] = 0.0;
  m[1][1] = (float)lVar2;
  m[2][1] = (float)lVar1;
  m[2][2] = (float)lVar2;
  m[1][2] = -(float)lVar1;
  m[2][0] = 0.0;
  return;
}


/* ==== MeMatrix3MakeRotationY ==== */

void MeMatrix3MakeRotationY(MeVector3 *m,MeReal a)

{
  longdouble lVar1;
  longdouble lVar2;
  
                    /* Unresolved local var: MeReal s@[???]
                       Unresolved local var: MeReal c@[???] */
                    /* Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
  lVar1 = (longdouble)fsin((longdouble)a);
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
  lVar2 = (longdouble)fcos((longdouble)a);
  (*m)[0] = (float)lVar2;
  (*m)[1] = 0.0;
  m[2][0] = (float)lVar1;
  m[2][2] = (float)lVar2;
  (*m)[2] = -(float)lVar1;
  m[1][0] = 0.0;
  m[1][1] = 1.0;
  m[1][2] = 0.0;
  m[2][1] = 0.0;
  return;
}


/* ==== MeMatrix3Trace ==== */

MeReal MeMatrix3Trace(MeVector3 *m)

{
  return m[1][1] + (*m)[0] + m[2][2];
}


/* ==== MeMatrix3MakeRotationZ ==== */

void MeMatrix3MakeRotationZ(MeVector3 *m,MeReal a)

{
  longdouble lVar1;
  longdouble lVar2;
  
                    /* Unresolved local var: MeReal s@[???]
                       Unresolved local var: MeReal c@[???] */
                    /* Unresolved local var: float __result@[DW_OP_reg12(ST1)] */
  lVar1 = (longdouble)fsin((longdouble)a);
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
  lVar2 = (longdouble)fcos((longdouble)a);
  (*m)[0] = (float)lVar2;
  m[1][0] = (float)lVar1;
  m[1][1] = (float)lVar2;
  (*m)[1] = -(float)lVar1;
  (*m)[2] = 0.0;
  m[1][2] = 0.0;
  m[2][0] = 0.0;
  m[2][1] = 0.0;
  m[2][2] = 1.0;
  return;
}


/* ==== MeMatrix3IsValidOrientationMatrix ==== */

MeBool MeMatrix3IsValidOrientationMatrix(MeReal (*rot) [3],MeReal tolerance)

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
  MeBool MVar10;
  uint uVar11;
  MeMatrix3 tmp;
  MeMatrix3 rotT1;
  MeMatrix3 Product;
  
                    /* Unresolved local var: MeReal determinant@[???] */
  fVar1 = (*rot)[0];
  fVar2 = (*rot)[1];
  fVar3 = (*rot)[2];
  fVar4 = rot[1][0];
  fVar5 = rot[1][1];
  fVar6 = rot[1][2];
  fVar7 = rot[2][0];
  fVar8 = rot[2][1];
  fVar9 = rot[2][2];
  Product[0][0] = fVar1 * fVar1 + fVar2 * fVar2 + fVar3 * fVar3;
  Product[0][1] = fVar3 * fVar6 + fVar1 * fVar4 + fVar2 * fVar5;
  Product[0][2] = fVar3 * fVar9 + fVar2 * fVar8 + fVar7 * fVar1;
  Product[1][0] = fVar4 * fVar1 + fVar5 * fVar2 + fVar6 * fVar3;
  Product[2][0] = fVar1 * fVar7 + fVar2 * fVar8 + fVar3 * fVar9;
  Product[1][1] = fVar4 * fVar4 + fVar5 * fVar5 + fVar6 * fVar6;
  Product[1][2] = fVar8 * fVar5 + fVar4 * fVar7 + fVar6 * fVar9;
  Product[2][1] = fVar4 * fVar7 + fVar5 * fVar8 + fVar6 * fVar9;
  Product[2][2] = fVar9 * fVar9 + fVar7 * fVar7 + fVar8 * fVar8;
  MVar10 = MeMatrix3IsIdentity(Product,tolerance);
  uVar11 = 0;
  if (MVar10 != 0) {
    uVar11 = (uint)(1.0 - tolerance <
                   ((*rot)[1] * rot[1][2] - rot[1][1] * (*rot)[2]) * rot[2][0] +
                   (rot[1][1] * rot[2][2] - rot[1][2] * rot[2][1]) * (*rot)[0] +
                   (rot[2][1] * (*rot)[2] - rot[2][2] * (*rot)[1]) * rot[1][0]);
  }
  return uVar11;
}


/* ==== MeMatrix3FromEulerAngles ==== */

void MeMatrix3FromEulerAngles(MeVector3 *m,MeReal xangle,MeReal yangle,MeReal zangle)

{
  float fVar1;
  longdouble lVar2;
  longdouble lVar3;
  longdouble lVar4;
  longdouble lVar5;
  longdouble lVar6;
  float __result;
  
                    /* Unresolved local var: MeReal cx@[???]
                       Unresolved local var: MeReal sx@[???]
                       Unresolved local var: MeReal cy@[???]
                       Unresolved local var: MeReal sy@[???]
                       Unresolved local var: MeReal cz@[???]
                       Unresolved local var: MeReal sz@[???] */
                    /* Unresolved local var: float __result@[DW_OP_reg14(ST3)] */
  lVar2 = (longdouble)fcos((longdouble)xangle);
                    /* Unresolved local var: float __result@[DW_OP_reg16(ST5)] */
  lVar3 = (longdouble)fsin((longdouble)xangle);
  lVar4 = (longdouble)fcos((longdouble)yangle);
  fVar1 = (float)lVar4;
                    /* Unresolved local var: float __result@[DW_OP_reg18(ST7)] */
  lVar4 = (longdouble)fsin((longdouble)yangle);
                    /* Unresolved local var: float __result@[DW_OP_reg15(ST4)] */
  lVar5 = (longdouble)fcos((longdouble)zangle);
                    /* Unresolved local var: float __result@[DW_OP_reg17(ST6)] */
  lVar6 = (longdouble)fsin((longdouble)zangle);
  (*m)[0] = (float)((longdouble)fVar1 * lVar5);
  (*m)[1] = (float)(-(longdouble)fVar1 * lVar6);
  (*m)[2] = -(float)lVar4;
  m[1][0] = (float)(-lVar3 * lVar4 * lVar5 + lVar2 * lVar6);
  m[1][1] = (float)(lVar3 * lVar4 * lVar6 + lVar2 * lVar5);
  m[1][2] = (float)(-lVar3 * (longdouble)fVar1);
  m[2][0] = (float)(lVar2 * lVar4 * lVar5 + lVar3 * lVar6);
  m[2][1] = (float)(lVar3 * lVar5 + -lVar2 * lVar4 * lVar6);
  m[2][2] = (float)(lVar2 * (longdouble)fVar1);
  return;
}


/* ==== MeMatrix4TMFromEulerAnglesAndPosition ==== */

void MeMatrix4TMFromEulerAnglesAndPosition
               (MeVector4 *tm,MeReal xangle,MeReal yangle,MeReal zangle,MeReal x,MeReal y,MeReal z)

{
  float fVar1;
  longdouble lVar2;
  longdouble lVar3;
  longdouble lVar4;
  longdouble lVar5;
  longdouble lVar6;
  float __result;
  
                    /* Unresolved local var: MeReal cx@[???]
                       Unresolved local var: MeReal sx@[???]
                       Unresolved local var: MeReal cy@[???]
                       Unresolved local var: MeReal sy@[???]
                       Unresolved local var: MeReal cz@[???]
                       Unresolved local var: MeReal sz@[???] */
                    /* Unresolved local var: float __result@[DW_OP_reg14(ST3)] */
  lVar2 = (longdouble)fcos((longdouble)xangle);
                    /* Unresolved local var: float __result@[DW_OP_reg16(ST5)] */
  lVar3 = (longdouble)fsin((longdouble)xangle);
  lVar4 = (longdouble)fcos((longdouble)yangle);
  fVar1 = (float)lVar4;
                    /* Unresolved local var: float __result@[DW_OP_reg18(ST7)] */
  lVar4 = (longdouble)fsin((longdouble)yangle);
                    /* Unresolved local var: float __result@[DW_OP_reg15(ST4)] */
  lVar5 = (longdouble)fcos((longdouble)zangle);
                    /* Unresolved local var: float __result@[DW_OP_reg17(ST6)] */
  lVar6 = (longdouble)fsin((longdouble)zangle);
  (*tm)[0] = (float)((longdouble)fVar1 * lVar5);
  (*tm)[1] = (float)(-(longdouble)fVar1 * lVar6);
  (*tm)[2] = -(float)lVar4;
  (*tm)[3] = 0.0;
  tm[1][0] = (float)(-lVar3 * lVar4 * lVar5 + lVar2 * lVar6);
  tm[1][1] = (float)(lVar3 * lVar4 * lVar6 + lVar2 * lVar5);
  tm[1][3] = 0.0;
  tm[1][2] = (float)(-lVar3 * (longdouble)fVar1);
  tm[2][0] = (float)(lVar2 * lVar4 * lVar5 + lVar3 * lVar6);
  tm[2][1] = (float)(lVar3 * lVar5 + -lVar2 * lVar4 * lVar6);
  tm[3][0] = x;
  tm[3][1] = y;
  tm[2][3] = 0.0;
  tm[2][2] = (float)(lVar2 * (longdouble)fVar1);
  tm[3][2] = z;
  tm[3][3] = 1.0;
  return;
}


/* ==== MeMatrix4IsIdentity ==== */

MeBool MeMatrix4IsIdentity(MeReal (*a) [4],MeReal tolerance)

{
  float fVar1;
  uint uVar2;
  MeVector4 Test;
  
                    /* Unresolved local var: uint i@[DW_OP_reg2(EDX)] */
  uVar2 = 0;
                    /* Unresolved local var: int i@[???] */
  Test[0] = 0.0;
  Test[1] = 0.0;
  Test[2] = 0.0;
  Test[3] = 0.0;
  while( true ) {
    Test[uVar2] = 1.0;
    fVar1 = Test[0];
    if (tolerance <= ABS((*a)[0])) {
      fVar1 = (Test[0] - (*a)[0]) / (*a)[0];
    }
    if (tolerance <= ABS(fVar1)) {
      return 0;
    }
    fVar1 = Test[1];
    if (tolerance <= ABS((*a)[1])) {
      fVar1 = (Test[1] - (*a)[1]) / (*a)[1];
    }
    if (tolerance <= ABS(fVar1)) break;
    fVar1 = Test[2];
    if (tolerance <= ABS((*a)[2])) {
      fVar1 = (Test[2] - (*a)[2]) / (*a)[2];
    }
    if (tolerance <= ABS(fVar1)) {
      return 0;
    }
    fVar1 = Test[3];
    if (tolerance <= ABS((*a)[3])) {
      fVar1 = (Test[3] - (*a)[3]) / (*a)[3];
    }
    if (tolerance <= ABS(fVar1)) {
      return 0;
    }
    Test[uVar2] = 0.0;
    uVar2 = uVar2 + 1;
    a = a + 1;
    if (3 < uVar2) {
      return 1;
    }
  }
  return 0;
}


/* ==== MeMatrix4IsTM ==== */

MeBool MeMatrix4IsTM(MeReal (*tm) [4],MeReal tolerance)

{
  float fVar1;
  MeBool MVar2;
  MeMatrix3 rot;
  
                    /* Unresolved local var: MeBool bIsTM@[DW_OP_reg0(EAX)]
                       Unresolved local var: uint i@[???] */
  if (tolerance <= 1.0) {
    fVar1 = tm[3][3] - 1.0;
  }
  else {
    fVar1 = tm[3][3];
  }
  if (tolerance <= ABS(fVar1)) {
    MVar2 = 0;
  }
  else if (tolerance <= ABS((*tm)[3])) {
    MVar2 = 0;
  }
  else if (tolerance <= ABS(tm[1][3])) {
    MVar2 = 0;
  }
  else if (ABS(tm[2][3]) < tolerance) {
    rot[0][0] = (*tm)[0];
    rot[0][1] = (*tm)[1];
    rot[0][2] = (*tm)[2];
    rot[1][0] = tm[1][0];
    rot[1][1] = tm[1][1];
    rot[1][2] = tm[1][2];
    rot[2][0] = tm[2][0];
    rot[2][1] = tm[2][1];
    rot[2][2] = tm[2][2];
    MVar2 = MeMatrix3IsValidOrientationMatrix(rot,tolerance);
  }
  else {
    MVar2 = 0;
  }
  return MVar2;
}


/* ==== MeMatrix4TMOrthoNormalize ==== */

void MeMatrix4TMOrthoNormalize(MeVector4 *tm)

{
  float fVar1;
  float fVar2;
  
                    /* Unresolved local var: MeReal dot@[???] */
  MeVector3Normalize(*tm);
  fVar1 = (*tm)[1];
  fVar2 = -(tm[1][2] * (*tm)[2] + fVar1 * tm[1][1] + (*tm)[0] * tm[1][0]);
  tm[1][0] = tm[1][0] + (*tm)[0] * fVar2;
  tm[1][1] = fVar1 * fVar2 + tm[1][1];
  tm[1][2] = fVar2 * (*tm)[2] + tm[1][2];
  MeVector3Normalize(tm[1]);
  fVar1 = (*tm)[2];
  tm[2][0] = tm[1][2] * (*tm)[1] - fVar1 * tm[1][1];
  tm[2][1] = fVar1 * tm[1][0] - tm[1][2] * (*tm)[0];
  tm[2][2] = tm[1][1] * (*tm)[0] - tm[1][0] * (*tm)[1];
  return;
}


/* ==== MeMatrix4TMInvert ==== */

void MeMatrix4TMInvert(MeVector4 *tm)

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
  MeReal MVar11;
  MeVector3 pos;
  
  fVar1 = (*tm)[1];
  fVar2 = tm[3][0];
  fVar3 = tm[3][1];
  fVar4 = (*tm)[2];
  fVar5 = tm[3][2];
  fVar6 = tm[1][2];
  fVar7 = tm[1][0];
  tm[3][0] = -(fVar4 * fVar5 + (*tm)[0] * fVar2 + fVar1 * fVar3);
  fVar8 = tm[1][1];
  fVar9 = tm[2][2];
  fVar10 = tm[2][1];
                    /* Unresolved local var: MeReal temp@[???] */
  tm[2][1] = fVar6;
  tm[1][2] = fVar10;
  tm[3][1] = -(fVar8 * fVar3 + fVar7 * fVar2 + fVar6 * fVar5);
  fVar6 = tm[2][0];
                    /* Unresolved local var: MeReal temp@[???] */
  tm[2][0] = fVar4;
  (*tm)[2] = fVar6;
  tm[3][2] = -(fVar5 * fVar9 + fVar2 * fVar6 + fVar3 * fVar10);
                    /* Unresolved local var: MeReal temp@[???] */
  MVar11 = tm[1][0];
  tm[1][0] = fVar1;
  (*tm)[1] = MVar11;
  return;
}


/* ==== MeMatrix4TMCompound ==== */

void MeMatrix4TMCompound(MeVector4 *A,MeReal (*B) [4],MeReal (*C) [4])

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  
  (*A)[0] = C[2][0] * (*B)[2] + C[1][0] * (*B)[1] + (*C)[0] * (*B)[0];
  (*A)[1] = C[2][1] * (*B)[2] + C[1][1] * (*B)[1] + (*C)[1] * (*B)[0];
  fVar1 = (*C)[2];
  fVar2 = C[1][2];
  fVar3 = (*B)[0];
  fVar4 = (*B)[1];
  fVar5 = C[2][2];
  fVar6 = (*B)[2];
  (*A)[3] = 0.0;
  (*A)[2] = fVar5 * fVar6 + fVar2 * fVar4 + fVar1 * fVar3;
  A[1][0] = C[2][0] * B[1][2] + C[1][0] * B[1][1] + (*C)[0] * B[1][0];
  A[1][1] = C[2][1] * B[1][2] + C[1][1] * B[1][1] + (*C)[1] * B[1][0];
  fVar1 = (*C)[2];
  fVar2 = C[1][2];
  fVar3 = B[1][0];
  fVar4 = B[1][1];
  fVar5 = C[2][2];
  fVar6 = B[1][2];
  A[1][3] = 0.0;
  A[1][2] = fVar5 * fVar6 + fVar2 * fVar4 + fVar1 * fVar3;
  A[2][0] = C[2][0] * B[2][2] + C[1][0] * B[2][1] + (*C)[0] * B[2][0];
  A[2][1] = C[2][1] * B[2][2] + C[1][1] * B[2][1] + (*C)[1] * B[2][0];
  fVar1 = (*C)[2];
  fVar2 = C[1][2];
  fVar3 = B[2][0];
  fVar4 = B[2][1];
  fVar5 = C[2][2];
  fVar6 = B[2][2];
  A[2][3] = 0.0;
  A[2][2] = fVar5 * fVar6 + fVar2 * fVar4 + fVar1 * fVar3;
  A[3][0] = C[2][0] * B[3][2] + C[1][0] * B[3][1] + (*C)[0] * B[3][0] + C[3][0];
  A[3][1] = C[2][1] * B[3][2] + C[1][1] * B[3][1] + (*C)[1] * B[3][0] + C[3][1];
  fVar1 = (*C)[2];
  fVar2 = C[1][2];
  fVar3 = B[3][0];
  fVar4 = B[3][1];
  fVar5 = C[2][2];
  fVar6 = B[3][2];
  fVar7 = C[3][2];
  A[3][3] = 1.0;
  A[3][2] = fVar5 * fVar6 + fVar2 * fVar4 + fVar1 * fVar3 + fVar7;
  return;
}


/* ==== MeVector3Normalize ==== */

MeReal MeVector3Normalize(MeReal *v)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float local_8;
  
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
  fVar1 = *v;
  fVar2 = v[1];
  fVar3 = v[2];
  local_8 = fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1;
  if (local_8 <= 0.0) {
    *v = 1.0;
    v[1] = 0.0;
    v[2] = 0.0;
    local_8 = 0.0;
  }
  else {
    local_8 = SQRT(local_8);
    fVar4 = 1.0 / local_8;
    *v = fVar1 * fVar4;
    v[1] = fVar2 * fVar4;
    v[2] = fVar4 * fVar3;
  }
  return local_8;
}


