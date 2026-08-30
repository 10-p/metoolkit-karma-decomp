  @ 00010fb0 ==== */

/* WARNING: Removing unreachable block (ram,0x0001186d) */
/* WARNING: Type propagation algorithm not settling */

bool McdVanillaOverlapOBBs
               (MeReal *outSep,lsVec3 *outN,MeReal *outPN,MeI16 *outDims,MeReal inEps,lsVec3 *inR1,
               lsVec3 *inR2,lsTransform *inT12)

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
  uint uVar10;
  MeReal MVar11;
  bool bVar12;
  byte bVar13;
  ushort uVar14;
  uint uVar15;
  lsTransformRow *plVar16;
  uint uVar17;
  int iVar18;
  int iVar19;
  lsTransformRow *plVar20;
  uint uVar21;
  sbyte local_d8;
  int j;
  MeReal nRLen;
  MeU8 normInfo;
  MeReal saveNormD;
  MeReal PN;
  MeReal maxSeparation;
  lsVec3 outVec;
  lsTransform arot;
  
                    /* Unresolved local var: MeReal eps2@[???]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: lsVec3 * pos@[???]
                       Unresolved local var: MeReal threshold@[DW_OP_reg17(ST6)]
                       Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
                    /* Unresolved local var: MeReal sumR@[???]
                       Unresolved local var: MeReal normD@[???]
                       Unresolved local var: MeReal separation@[???] */
  plVar16 = inT12->row + 3;
  maxSeparation = -3.4028235e+38;
  saveNormD = 1.0;
  nRLen = 1.0;
  normInfo = '\x0f';
  arot.row[0].v.v[0] = ABS(inT12->row[0].v.v[0]);
  arot.row[0].v.v[1] = ABS(inT12->row[0].v.v[1]);
  arot.row[0].v.v[2] = ABS(inT12->row[0].v.v[2]);
  arot.row[1].v.v[0] = ABS(inT12->row[1].v.v[0]);
  arot.row[1].v.v[1] = ABS(inT12->row[1].v.v[1]);
  arot.row[1].v.v[2] = ABS(inT12->row[1].v.v[2]);
  arot.row[2].v.v[0] = ABS(inT12->row[2].v.v[0]);
  arot.row[2].v.v[1] = ABS(inT12->row[2].v.v[1]);
  arot.row[2].v.v[2] = ABS(inT12->row[2].v.v[2]);
  fVar1 = inR2->v[0];
  fVar2 = inR2->v[1];
  fVar3 = inR2->v[2];
  fVar6 = inT12->row[3].v.v[0];
  fVar4 = ABS(fVar6) -
          (arot.row[0].v.v[0] * fVar1 + inR1->v[0] + arot.row[1].v.v[0] * fVar2 +
          arot.row[2].v.v[0] * fVar3);
  if (fVar4 <= -3.4028235e+38) {
LAB_000110bd:
    fVar6 = inT12->row[3].v.v[1];
    fVar4 = ABS(fVar6) -
            (fVar3 * arot.row[2].v.v[1] +
            fVar2 * arot.row[1].v.v[1] + fVar1 * arot.row[0].v.v[1] + inR1->v[1]);
    if (maxSeparation < fVar4) {
      PN = -inR1->v[1] - fVar4;
      normInfo = '\r';
      saveNormD = fVar6;
      maxSeparation = fVar4;
      if (inEps < fVar4) goto LAB_00011840;
    }
    fVar6 = inT12->row[3].v.v[2];
    fVar1 = ABS(fVar6) -
            (fVar1 * arot.row[0].v.v[2] + inR1->v[2] + fVar2 * arot.row[1].v.v[2] +
            fVar3 * arot.row[2].v.v[2]);
    if (maxSeparation < fVar1) {
      PN = -inR1->v[2] - fVar1;
      normInfo = '\x0e';
      saveNormD = fVar6;
      maxSeparation = fVar1;
      if (inEps < fVar1) goto LAB_00011840;
    }
                    /* Unresolved local var: MeReal sumR@[???]
                       Unresolved local var: MeReal normD@[???]
                       Unresolved local var: MeReal aNormD@[???]
                       Unresolved local var: MeReal separation@[???] */
    fVar1 = inR1->v[0];
    fVar2 = inR1->v[1];
    fVar3 = inR1->v[2];
    fVar6 = (plVar16->v).v[0];
    fVar4 = inT12->row[3].v.v[1];
    fVar5 = inT12->row[3].v.v[2];
    fVar9 = fVar5 * inT12->row[0].v.v[2] +
            fVar4 * inT12->row[0].v.v[1] + fVar6 * inT12->row[0].v.v[0];
    fVar8 = ABS(fVar9);
    fVar7 = fVar8 - (fVar1 * arot.row[0].v.v[0] + inR2->v[0] + fVar2 * arot.row[0].v.v[1] +
                    fVar3 * arot.row[0].v.v[2]);
    if (maxSeparation < fVar7) {
      PN = inR2->v[0] - fVar8;
      normInfo = '\x03';
      saveNormD = fVar9;
      maxSeparation = fVar7;
      if (inEps < fVar7) goto LAB_00011840;
    }
    iVar18 = 1;
    do {
      plVar20 = inT12->row + iVar18;
      fVar7 = fVar5 * (plVar20->v).v[2] + fVar4 * (plVar20->v).v[1] + fVar6 * (plVar20->v).v[0];
      fVar8 = ABS(fVar7);
      fVar9 = fVar8 - (fVar3 * arot.row[iVar18].v.v[2] +
                      fVar1 * arot.row[iVar18].v.v[0] + inR2->v[iVar18] +
                      fVar2 * arot.row[iVar18].v.v[1]);
      if (maxSeparation < fVar9) {
        PN = inR2->v[iVar18] - fVar8;
        normInfo = (char)iVar18 * '\x04' | 3;
        saveNormD = fVar7;
        maxSeparation = fVar9;
        if (inEps < fVar9) goto LAB_00011840;
      }
      plVar20 = inT12->row + iVar18 + 1;
      fVar7 = fVar5 * (plVar20->v).v[2] + fVar4 * (plVar20->v).v[1] + fVar6 * (plVar20->v).v[0];
      fVar8 = ABS(fVar7);
      fVar9 = fVar8 - (fVar1 * arot.row[iVar18 + 1].v.v[0] + inR2->v[iVar18 + 1] +
                       fVar2 * arot.row[iVar18 + 1].v.v[1] + fVar3 * arot.row[iVar18 + 1].v.v[2]);
      if (maxSeparation < fVar9) {
        PN = inR2->v[iVar18 + 1] - fVar8;
        normInfo = (char)iVar18 * '\x04' + 4U | 3;
        saveNormD = fVar7;
        maxSeparation = fVar9;
        if (inEps < fVar9) goto LAB_00011840;
      }
      iVar18 = iVar18 + 2;
    } while (iVar18 < 3);
    j = 0;
    fVar1 = maxSeparation + inEps;
    do {
                    /* Unresolved local var: int j1@[???]
                       Unresolved local var: int j2@[???]
                       Unresolved local var: lsVec3 * aj@[???]
                       Unresolved local var: lsVec3 * aj1@[???]
                       Unresolved local var: lsVec3 * aj2@[???]
                       Unresolved local var: lsVec3 * tj@[???] */
      uVar17 = 1 << ((byte)j & 0x1f) & 3;
      local_d8 = (sbyte)uVar17;
      uVar15 = 1 << local_d8 & 3;
      iVar19 = 0;
      iVar18 = 0;
      do {
                    /* Unresolved local var: MeReal rLen@[DW_OP_reg13(ST2)] */
        fVar2 = *(float *)((int)inT12->row[j].v.v + iVar18);
        fVar2 = 1.0 - fVar2 * fVar2;
        if (1e-06 < fVar2) {
                    /* Unresolved local var: int i1@[???]
                       Unresolved local var: int i2@[???]
                       Unresolved local var: MeReal rA@[???]
                       Unresolved local var: MeReal rB@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal normD@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal aNormD@[???]
                       Unresolved local var: MeReal separation@[DW_OP_reg12(ST1)] */
          uVar21 = 1 << ((byte)iVar19 & 0x1f) & 3;
          local_d8 = (sbyte)uVar21;
          uVar10 = 1 << local_d8 & 3;
          fVar4 = *(float *)((int)arot.row[uVar17].v.v + iVar18) * inR2->v[uVar15] +
                  *(float *)((int)arot.row[uVar15].v.v + iVar18) * inR2->v[uVar17];
          fVar5 = inT12->row[j].v.v[uVar21] * (plVar16->v).v[uVar10] -
                  inT12->row[j].v.v[uVar10] * (plVar16->v).v[uVar21];
          fVar3 = ABS(fVar5);
          fVar6 = (fVar3 - (arot.row[j].v.v[uVar10] * inR1->v[uVar21] +
                           arot.row[j].v.v[uVar21] * inR1->v[uVar10])) - fVar4;
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
          fVar2 = SQRT(fVar2);
          if (inEps * fVar2 < fVar6) {
            return false;
          }
          if (fVar1 * fVar2 < fVar6) {
            nRLen = 1.0 / fVar2;
            normInfo = (byte)(j << 2) | (byte)iVar19;
            fVar1 = fVar6 * nRLen;
            PN = nRLen * (fVar4 - fVar3);
            saveNormD = fVar5;
            maxSeparation = fVar1;
          }
        }
        iVar19 = iVar19 + 1;
        iVar18 = iVar18 + 4;
      } while (iVar19 < 3);
      j = j + 1;
    } while (j < 3);
    *outSep = maxSeparation;
    *outPN = PN;
    bVar13 = normInfo & 0xc;
    if (bVar13 == 0xc) {
                    /* Unresolved local var: MeI8 axis@[???]
                       Unresolved local var: MeI16 dimB@[DW_OP_reg3(EBX)] */
      uVar15 = normInfo & 3;
      uVar14 = (ushort)(arot.row[0].v.v[uVar15] < 0.0001);
      if (arot.row[1].v.v[uVar15] < 0.0001) {
        uVar14 = uVar14 + 1;
      }
      if (arot.row[2].v.v[uVar15] < 0.0001) {
        uVar14 = uVar14 + 1;
      }
      MVar11 = 1.0;
      outN->v[0] = 0.0;
      outN->v[1] = 0.0;
      outN->v[2] = 0.0;
      if (0.0 <= saveNormD) {
        MVar11 = -1.0;
      }
      outN->v[uVar15] = MVar11;
      *outDims = uVar14 << 8 | 2;
    }
    else if ((normInfo & 3) == 3) {
                    /* Unresolved local var: MeI8 axis@[???]
                       Unresolved local var: MeI16 dimA@[DW_OP_reg3(EBX)] */
      bVar13 = bVar13 >> 2;
      uVar14 = (ushort)(arot.row[(char)bVar13].v.v[0] < 0.0001);
      if (arot.row[(char)bVar13].v.v[1] < 0.0001) {
        uVar14 = uVar14 + 1;
      }
      if (arot.row[(char)bVar13].v.v[2] < 0.0001) {
        uVar14 = uVar14 + 1;
      }
      if (saveNormD <= 0.0) {
        plVar16 = inT12->row + (char)bVar13;
        outVec.v[0] = (plVar16->v).v[0];
        outVec.v[1] = (plVar16->v).v[1];
        MVar11 = (plVar16->v).v[2];
      }
      else {
        plVar16 = inT12->row + (char)bVar13;
        outVec.v[0] = -(plVar16->v).v[0];
        outVec.v[1] = -(plVar16->v).v[1];
        MVar11 = -(plVar16->v).v[2];
      }
      outN->v[0] = outVec.v[0];
      outN->v[1] = outVec.v[1];
      outN->v[2] = MVar11;
      *outDims = uVar14 | 0x200;
    }
    else {
      if (0.0 <= saveNormD) {
        uVar17 = 1 << (sbyte)(normInfo & 3) & 3;
        outVec.v[normInfo & 3] = 0.0;
        uVar15 = 1 << (sbyte)uVar17 & 3;
        outVec.v[uVar17] = inT12->row[0].v.v[(normInfo & 0xc) + uVar15];
        MVar11 = -inT12->row[0].v.v[(normInfo & 0xc) + uVar17];
      }
      else {
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
        uVar17 = 1 << (normInfo & 3) & 3;
        uVar15 = 1 << (sbyte)uVar17 & 3;
        outVec.v[normInfo & 0xffffff03] = 0.0;
        outVec.v[uVar17] = -inT12->row[bVar13 >> 2].v.v[uVar15];
        MVar11 = inT12->row[bVar13 >> 2].v.v[uVar17];
      }
      outVec.v[uVar15] = MVar11;
      outN->v[0] = outVec.v[0];
      outN->v[1] = outVec.v[1];
      outN->v[0] = outVec.v[0] * nRLen;
      outN->v[1] = outVec.v[1] * nRLen;
      outN->v[2] = outVec.v[2] * nRLen;
      *outDims = 0x101;
    }
    bVar12 = true;
  }
  else {
    PN = -inR1->v[0] - fVar4;
    normInfo = '\f';
    saveNormD = fVar6;
    maxSeparation = fVar4;
    if (fVar4 <= inEps) goto LAB_000110bd;
LAB_00011840:
    bVar12 = false;
  }
  return bVar12;
}

