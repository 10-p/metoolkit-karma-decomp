/* ==== McdBoxGetSlice ==== */

/* WARNING: Removing unreachable block (ram,0x0001015b) */

void McdBoxGetSlice(McdGeometryInstanceID ins,MeReal *normal,MeReal dist,int maxVert,int *numVert,
                   MeVector3 *outVert)

{
  MeVector3 *paMVar1;
  float fVar2;
  float fVar3;
  MeMatrix4Ptr paMVar4;
  McdGeometry *pMVar5;
  int iVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  McdGeometryID pMVar10;
  uint uVar11;
  float fVar12;
  McdGeometryID pMVar13;
  McdGeometryID pMVar14;
  McdGeometryID pMVar15;
  uint uVar16;
  uint uVar17;
  MeReal (*paMVar18) [3];
  MeReal (*local_c0) [3];
  MeReal norm [3];
  MeReal temp [3];
  MeReal v [8] [3];
  MeReal dpp [8];
  
                    /* Unresolved local var: McdBox * box@[DW_OP_reg1(ECX)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[DW_OP_reg1(ECX)]
                       Unresolved local var: int k@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal fat@[DW_OP_reg17(ST6)] */
  fVar2 = *normal;
  fVar12 = normal[1];
  paMVar4 = ins->mTM;
  pMVar5 = ins->mGeometry;
  fVar3 = normal[2];
  norm[0] = fVar3 * (*paMVar4)[2] + fVar2 * (*paMVar4)[0] + fVar12 * (*paMVar4)[1];
  norm[1] = fVar3 * paMVar4[1][2] + fVar2 * paMVar4[1][0] + fVar12 * paMVar4[1][1];
  norm[2] = fVar3 * paMVar4[2][2] + fVar2 * paMVar4[2][0] + fVar12 * paMVar4[2][1];
  fVar2 = dist - (fVar2 * paMVar4[3][0] + fVar12 * paMVar4[3][1] + fVar3 * paMVar4[3][2]);
  if (fVar2 < 0.0) {
    fVar2 = -fVar2;
    norm[0] = norm[0] * -1.0;
    norm[1] = norm[1] * -1.0;
    norm[2] = norm[2] * -1.0;
  }
  fVar12 = (float)pMVar5[1].mRefCtAndID;
  if (norm[0] < 0.0) {
    fVar12 = -fVar12;
  }
  pMVar13 = pMVar5[1].prev;
  if (norm[1] < 0.0) {
    pMVar13 = (McdGeometryID)((uint)pMVar13 ^ 0x80000000);
  }
  pMVar14 = pMVar5[1].next;
  if (norm[2] < 0.0) {
    pMVar14 = (McdGeometryID)((uint)pMVar14 ^ 0x80000000);
  }
  paMVar18 = v;
  uVar17 = 0;
  do {
    pMVar15 = pMVar13;
    if ((uVar17 & 2) != 0) {
      pMVar15 = (McdGeometryID)((uint)pMVar13 ^ 0x80000000);
    }
    pMVar10 = pMVar14;
    if ((uVar17 & 4) != 0) {
      pMVar10 = (McdGeometryID)((uint)pMVar14 ^ 0x80000000);
    }
    (*paMVar18)[0] = fVar12;
    uVar11 = uVar17 + 1;
    (*paMVar18)[1] = (MeReal)pMVar15;
    (*paMVar18)[2] = (MeReal)pMVar10;
    dpp[uVar17] = -999.0;
    fVar3 = fVar12;
    if ((uVar11 & 1) != 0) {
      fVar3 = -fVar12;
    }
    pMVar15 = pMVar13;
    if ((uVar11 & 2) != 0) {
      pMVar15 = (McdGeometryID)((uint)pMVar13 ^ 0x80000000);
    }
    pMVar10 = pMVar14;
    if ((uVar11 & 4) != 0) {
      pMVar10 = (McdGeometryID)((uint)pMVar14 ^ 0x80000000);
    }
    uVar17 = uVar17 + 2;
    paMVar18[1][1] = (MeReal)pMVar15;
    paMVar18[1][0] = fVar3;
    paMVar18[1][2] = (MeReal)pMVar10;
    paMVar18 = paMVar18 + 2;
    dpp[uVar11] = -999.0;
  } while ((int)uVar17 < 8);
  local_c0 = v;
  *numVert = 0;
  uVar17 = 0;
  dpp[0] = (norm[2] * v[0][2] + norm[1] * v[0][1] + norm[0] * v[0][0]) - fVar2;
  do {
    if (0.0 <= dpp[uVar17]) {
      uVar11 = 1;
      do {
        uVar16 = uVar17 | uVar11;
        if (uVar17 != uVar16) {
          fVar12 = dpp[uVar16];
          if (fVar12 == -999.0) {
            fVar12 = (norm[2] * v[uVar16][2] + norm[1] * v[uVar16][1] + norm[0] * v[uVar16][0]) -
                     fVar2;
            dpp[uVar16] = fVar12;
          }
          if (fVar12 < 0.0) {
            fVar12 = dpp[uVar17];
            fVar8 = -dpp[uVar16];
            fVar9 = 1.0 / (dpp[uVar17] - dpp[uVar16]);
            fVar7 = (fVar8 * (*local_c0)[1] + v[uVar16][1] * fVar12) * fVar9;
            fVar3 = (fVar8 * (*local_c0)[2] + fVar12 * v[uVar16][2]) * fVar9;
            fVar9 = (fVar8 * (*local_c0)[0] + v[uVar16][0] * fVar12) * fVar9;
            paMVar1 = outVert + *numVert;
            paMVar4 = ins->mTM;
            (*paMVar1)[0] =
                 fVar3 * paMVar4[2][0] + fVar9 * (*paMVar4)[0] + fVar7 * paMVar4[1][0] +
                 paMVar4[3][0];
            (*paMVar1)[1] =
                 fVar3 * paMVar4[2][1] + fVar9 * (*paMVar4)[1] + fVar7 * paMVar4[1][1] +
                 paMVar4[3][1];
            (*paMVar1)[2] =
                 fVar9 * (*paMVar4)[2] + fVar7 * paMVar4[1][2] + fVar3 * paMVar4[2][2] +
                 paMVar4[3][2];
            iVar6 = *numVert;
            *numVert = iVar6 + 1;
            if (iVar6 + 1 == maxVert) {
              return;
            }
          }
        }
        uVar11 = uVar11 * 2;
      } while ((int)uVar11 < 8);
    }
    uVar17 = uVar17 + 1;
    local_c0 = local_c0 + 1;
  } while ((int)uVar17 < 7);
  return;
}


/* ==== McdConvexMeshPlaneCut ==== */

void McdConvexMeshPlaneCut
               (McdConvexMesh *conv,MeReal *norm,MeReal dp,int flags,MeReal (*tm) [4],int maxVert,
               int *numVert,MeVector3 *outVert)

{
  MeVector3 *paMVar1;
  float fVar2;
  McdCnvVertex *pMVar3;
  ushort uVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  byte bVar15;
  int iVar10;
  int iVar11;
  int *piVar12;
  McdCnvVertex *pMVar13;
  int *piVar14;
  McdCnvVertex *pMVar16;
  int iVar17;
  bool bVar18;
  longdouble lVar19;
  int m;
  int v;
  MeReal dv;
  McdCnvVertex *vert;
  int start;
  MeDictNode nodemem [200];
  MeSet set;
  MeReal temp [3];
  
                    /* Unresolved local var: MeReal du@[DW_OP_reg13(ST2)]
                       Unresolved local var: int nv@[???]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int u@[DW_OP_reg2(EDX)] */
  *numVert = 0;
  if ((0 < maxVert) &&
     (lVar19 = (longdouble)McdConvexMeshMaximumPointLocal(conv,norm,0,dp,&start),
     (longdouble)dp <= lVar19)) {
    pMVar3 = (conv->mHull).vertex;
    MeSetInit(&set,nodemem,200,0);
    MeSetAdd(&set,start);
    while (set.next != (MeDictNode *)0x0) {
      iVar10 = MeSetIteratorNext(&set);
      iVar11 = iVar10 * 0x10;
      pMVar16 = pMVar3 + iVar10;
      fVar5 = norm[2] * pMVar16->position[2] +
              norm[1] * pMVar16->position[1] + *norm * pMVar16->position[0];
      if ((flags & 2U) != 0) {
        iVar11 = *numVert * 3;
        paMVar1 = outVert + *numVert;
        (*paMVar1)[0] =
             pMVar16->position[2] * tm[2][0] +
             pMVar16->position[0] * (*tm)[0] + pMVar16->position[1] * tm[1][0] + tm[3][0];
        (*paMVar1)[1] =
             tm[2][1] * pMVar16->position[2] +
             tm[1][1] * pMVar16->position[1] + (*tm)[1] * pMVar16->position[0] + tm[3][1];
        (*paMVar1)[2] =
             tm[2][2] * pMVar16->position[2] +
             tm[1][2] * pMVar16->position[1] + (*tm)[2] * pMVar16->position[0] + tm[3][2];
        iVar17 = *numVert;
        *numVert = iVar17 + 1;
        if (iVar17 + 1 == maxVert) {
          return;
        }
      }
      iVar17 = 0;
      piVar12 = (int *)McdCnvVertexGetCount(&conv->mHull,iVar10,iVar11,iVar11);
      piVar14 = piVar12;
      if (0 < (int)piVar12) {
        do {
          iVar11 = McdCnvVertexGetNeighbor(&conv->mHull,iVar10,iVar17,piVar14);
          pMVar13 = pMVar3 + iVar11;
          fVar2 = pMVar13->position[2] * norm[2] +
                  pMVar13->position[1] * norm[1] + pMVar13->position[0] * *norm;
          piVar14 = (int *)CONCAT22((short)((uint)pMVar13 >> 0x10),
                                    (ushort)(fVar2 < dp) << 8 |
                                    (ushort)(NAN(fVar2) || NAN(dp)) << 10 |
                                    (ushort)(fVar2 == dp) << 0xe);
          if (fVar2 < dp) {
            if ((flags & 1U) != 0) {
              fVar6 = 1.0 / (fVar5 - fVar2);
              pMVar13 = pMVar3 + iVar11;
              fVar9 = (fVar5 - dp) * fVar6;
              fVar6 = (dp - fVar2) * fVar6;
              fVar8 = fVar6 * pMVar16->position[0] + pMVar13->position[0] * fVar9;
              fVar7 = fVar6 * pMVar16->position[1] + pMVar13->position[1] * fVar9;
              fVar2 = fVar6 * pMVar16->position[2] + fVar9 * pMVar13->position[2];
              paMVar1 = outVert + *numVert;
              (*paMVar1)[0] = fVar2 * tm[2][0] + fVar8 * (*tm)[0] + fVar7 * tm[1][0] + tm[3][0];
              (*paMVar1)[1] = fVar2 * tm[2][1] + fVar8 * (*tm)[1] + fVar7 * tm[1][1] + tm[3][1];
              (*paMVar1)[2] = fVar2 * tm[2][2] + fVar8 * (*tm)[2] + fVar7 * tm[1][2] + tm[3][2];
              iVar11 = *numVert;
              if (iVar11 != 0) {
                if (1e-06 <= ABS((outVert + iVar11)[-1][0])) {
                  fVar2 = (outVert + iVar11)[-1][0];
                  fVar2 = ABS((1.0 / fVar2) * (outVert[iVar11][0] - fVar2));
                  bVar15 = fVar2 < 1e-06 | (byte)((ushort)((ushort)NAN(fVar2) << 10) >> 8);
                }
                else {
                  fVar2 = ABS(outVert[iVar11][0]);
                  bVar15 = fVar2 < 1e-06 | (byte)((ushort)((ushort)NAN(fVar2) << 10) >> 8);
                }
                if (bVar15 != 0) {
                  if (1e-06 <= ABS(outVert[iVar11 + -1][1])) {
                    fVar2 = ABS((1.0 / outVert[iVar11 + -1][1]) *
                                (outVert[iVar11][1] - outVert[iVar11 + -1][1]));
                    bVar15 = fVar2 < 1e-06 | (byte)((ushort)((ushort)NAN(fVar2) << 10) >> 8);
                  }
                  else {
                    bVar15 = ABS(outVert[iVar11][1]) < 1e-06 |
                             (byte)((ushort)((ushort)NAN(ABS(outVert[iVar11][1])) << 10) >> 8);
                  }
                  if (bVar15 != 0) {
                    paMVar1 = outVert + iVar11;
                    if (1e-06 <= ABS(paMVar1[-1][2])) {
                      paMVar1 = outVert + iVar11;
                      fVar2 = ABS(((*paMVar1)[2] - paMVar1[-1][2]) * (1.0 / paMVar1[-1][2]));
                      piVar14 = (int *)CONCAT22((short)((uint)paMVar1 >> 0x10),
                                                (ushort)(fVar2 < 1e-06) << 8 |
                                                (ushort)NAN(fVar2) << 10 |
                                                (ushort)(fVar2 == 1e-06) << 0xe);
                      bVar18 = fVar2 >= 1e-06;
                    }
                    else {
                      fVar2 = ABS((*paMVar1)[2]);
                      uVar4 = (ushort)(fVar2 < 1e-06) << 8 | (ushort)NAN(fVar2) << 10;
                      piVar14 = (int *)CONCAT22((short)((uint)paMVar1 >> 0x10),
                                                uVar4 | (ushort)(fVar2 == 1e-06) << 0xe);
                      bVar18 = uVar4 == 0;
                    }
                    if (!bVar18) goto LAB_000107b2;
                  }
                }
              }
              *numVert = iVar11 + 1;
              piVar14 = numVert;
              if (iVar11 + 1 == maxVert) {
                return;
              }
            }
          }
          else {
            piVar14 = (int *)MeSetAdd(&set,iVar11,norm,norm);
          }
LAB_000107b2:
          iVar17 = iVar17 + 1;
        } while (iVar17 < (int)piVar12);
      }
    }
  }
  return;
}


/* ==== McdConvexMeshGetSlice ==== */

void McdConvexMeshGetSlice
               (McdGeometryInstanceID ins,MeReal *normal,MeReal dist,int maxVert,int *numVert,
               MeVector3 *outVert)

{
  float fVar1;
  float fVar2;
  float fVar3;
  McdConvexMesh *conv;
  MeMatrix4Ptr paMVar4;
  MeReal norm [3];
  
                    /* Unresolved local var: MeReal dp@[DW_OP_reg12(ST1)]
                       Unresolved local var: McdConvexMesh * conv@[DW_OP_reg3(EBX)] */
  conv = (McdConvexMesh *)ins->mGeometry;
  *numVert = 0;
  if (0 < maxVert) {
    fVar1 = *normal;
    fVar2 = normal[1];
    paMVar4 = ins->mTM;
    fVar3 = normal[2];
    norm[0] = fVar3 * (*paMVar4)[2] + fVar1 * (*paMVar4)[0] + fVar2 * (*paMVar4)[1];
    norm[1] = fVar3 * paMVar4[1][2] + fVar1 * paMVar4[1][0] + fVar2 * paMVar4[1][1];
    norm[2] = fVar3 * paMVar4[2][2] + fVar1 * paMVar4[2][0] + fVar2 * paMVar4[2][1];
    fVar1 = dist - (fVar1 * paMVar4[3][0] + fVar2 * paMVar4[3][1] + fVar3 * paMVar4[3][2]);
    if (fVar1 < 0.0) {
      fVar1 = -fVar1;
      norm[0] = norm[0] * -1.0;
      norm[1] = norm[1] * -1.0;
      norm[2] = norm[2] * -1.0;
    }
    McdConvexMeshPlaneCut(conv,norm,fVar1,1,ins->mTM,maxVert,numVert,outVert);
  }
  return;
}


/* ==== McdCylinderGetSlice ==== */

void McdCylinderGetSlice(McdGeometryInstanceID ins,MeReal *normal,MeReal dist,int maxVert,
                        int *numVert,MeVector3 *outVert)

{
  MeVector3 *paMVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  McdGeometryID pMVar5;
  McdGeometry *pMVar6;
  MeMatrix4Ptr paMVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  int iVar15;
  MeReal a;
  MeReal major [3];
  MeReal minor [3];
  MeReal temp [3];
  MeReal norm [3];
  
                    /* Unresolved local var: MeReal dp@[DW_OP_reg17(ST6)]
                       Unresolved local var: McdCylinder * cyl@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal zof@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal b@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal c@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal t@[???] */
  pMVar6 = ins->mGeometry;
  *numVert = 0;
  if (maxVert < 1) {
    return;
  }
  paMVar7 = ins->mTM;
  fVar2 = *normal;
  fVar3 = normal[1];
  fVar4 = normal[2];
  fVar11 = dist - (fVar4 * paMVar7[3][2] + fVar2 * paMVar7[3][0] + fVar3 * paMVar7[3][1]);
  fVar10 = fVar4 * (*paMVar7)[2] + fVar2 * (*paMVar7)[0] + fVar3 * (*paMVar7)[1];
  fVar9 = fVar4 * paMVar7[1][2] + fVar2 * paMVar7[1][0] + fVar3 * paMVar7[1][1];
  fVar8 = fVar4 * paMVar7[2][2] + fVar3 * paMVar7[2][1] + fVar2 * paMVar7[2][0];
  if (ABS(fVar8) < 0.001) {
    if ((float)pMVar6[1].mRefCtAndID + 1e-06 < ABS(fVar11)) {
      return;
    }
    fVar2 = fVar2 * fVar11 + paMVar7[3][0];
    fVar3 = fVar3 * fVar11 + paMVar7[3][1];
    fVar4 = paMVar7[3][2] + fVar11 * fVar4;
    pMVar5 = pMVar6[1].prev;
    (*outVert)[0] = (float)pMVar5 * paMVar7[2][0] + fVar2;
    (*outVert)[1] = (float)pMVar5 * paMVar7[2][1] + fVar3;
    (*outVert)[2] = (float)pMVar5 * paMVar7[2][2] + fVar4;
    iVar15 = *numVert + 1;
    *numVert = iVar15;
    if (maxVert <= iVar15) {
      return;
    }
    fVar8 = -(float)pMVar6[1].prev;
    paMVar1 = outVert + iVar15;
    paMVar7 = ins->mTM;
    (*paMVar1)[0] = fVar8 * paMVar7[2][0] + fVar2;
    (*paMVar1)[1] = fVar8 * paMVar7[2][1] + fVar3;
    (*paMVar1)[2] = fVar8 * paMVar7[2][2] + fVar4;
    goto LAB_00010b19;
  }
                    /* Unresolved local var: int axisN1@[???]
                       Unresolved local var: int axisN2@[???] */
  minor[2] = 0.0;
  minor[1] = -fVar10;
  minor[0] = fVar9;
  MeVector3Normalize(minor);
  fVar2 = fVar10 * minor[1] - fVar9 * minor[0];
  if (0.0 <= fVar2 * fVar8) {
    fVar3 = (float)pMVar6[1].mRefCtAndID;
  }
  else {
    fVar3 = -(float)pMVar6[1].mRefCtAndID;
  }
  fVar3 = fVar3 / fVar8;
  major[2] = fVar3 * fVar2;
  major[1] = (fVar8 * minor[0] - minor[2] * fVar10) * fVar3;
  major[0] = (minor[2] * fVar9 - minor[1] * fVar8) * fVar3;
  fVar2 = (float)pMVar6[1].mRefCtAndID;
  fVar4 = minor[0] * fVar2;
  fVar3 = minor[1] * fVar2;
  fVar2 = fVar2 * minor[2];
  fVar11 = (1.0 / fVar8) * fVar11;
  if (major[2] < 1e-06) {
    if ((float)pMVar6[1].prev + 1e-06 < ABS(fVar11)) {
      return;
    }
    fVar2 = (float)pMVar6[1].mRefCtAndID;
    paMVar1 = outVert + *numVert;
    paMVar7 = ins->mTM;
    (*paMVar1)[0] =
         fVar11 * paMVar7[2][0] + fVar2 * (*paMVar7)[0] + fVar2 * paMVar7[1][0] + paMVar7[3][0];
    (*paMVar1)[1] =
         fVar11 * paMVar7[2][1] + fVar2 * (*paMVar7)[1] + fVar2 * paMVar7[1][1] + paMVar7[3][1];
    (*paMVar1)[2] =
         fVar11 * paMVar7[2][2] + fVar2 * (*paMVar7)[2] + fVar2 * paMVar7[1][2] + paMVar7[3][2];
    iVar15 = *numVert + 1;
    *numVert = iVar15;
    if (maxVert <= iVar15) {
      return;
    }
    fVar2 = (float)pMVar6[1].mRefCtAndID;
    fVar3 = -fVar2;
    paMVar1 = outVert + iVar15;
    paMVar7 = ins->mTM;
    (*paMVar1)[0] =
         fVar11 * paMVar7[2][0] + fVar3 * paMVar7[1][0] + fVar2 * (*paMVar7)[0] + paMVar7[3][0];
    (*paMVar1)[1] =
         fVar11 * paMVar7[2][1] + fVar2 * (*paMVar7)[1] + fVar3 * paMVar7[1][1] + paMVar7[3][1];
    (*paMVar1)[2] =
         fVar11 * paMVar7[2][2] + fVar2 * (*paMVar7)[2] + fVar3 * paMVar7[1][2] + paMVar7[3][2];
    iVar15 = *numVert + 1;
    *numVert = iVar15;
    if (maxVert <= iVar15) {
      return;
    }
    fVar2 = (float)pMVar6[1].mRefCtAndID;
    fVar3 = -fVar2;
    paMVar1 = outVert + iVar15;
    paMVar7 = ins->mTM;
    (*paMVar1)[0] =
         fVar11 * paMVar7[2][0] + fVar2 * paMVar7[1][0] + fVar3 * (*paMVar7)[0] + paMVar7[3][0];
    (*paMVar1)[1] =
         fVar11 * paMVar7[2][1] + fVar3 * (*paMVar7)[1] + fVar2 * paMVar7[1][1] + paMVar7[3][1];
    (*paMVar1)[2] =
         fVar11 * paMVar7[2][2] + fVar3 * (*paMVar7)[2] + fVar2 * paMVar7[1][2] + paMVar7[3][2];
    iVar15 = *numVert + 1;
    *numVert = iVar15;
    if (maxVert <= iVar15) {
      return;
    }
    fVar2 = -(float)pMVar6[1].mRefCtAndID;
    paMVar1 = outVert + iVar15;
    paMVar7 = ins->mTM;
    (*paMVar1)[0] =
         fVar11 * paMVar7[2][0] + fVar2 * paMVar7[1][0] + fVar2 * (*paMVar7)[0] + paMVar7[3][0];
    (*paMVar1)[1] =
         fVar11 * paMVar7[2][1] + fVar2 * (*paMVar7)[1] + fVar2 * paMVar7[1][1] + paMVar7[3][1];
    (*paMVar1)[2] =
         fVar2 * (*paMVar7)[2] + fVar2 * paMVar7[1][2] + fVar11 * paMVar7[2][2] + paMVar7[3][2];
    goto LAB_00010b19;
  }
  pMVar5 = pMVar6[1].prev;
  fVar8 = (1.0 / major[2]) * ((float)pMVar5 - fVar11);
  if (fVar8 < 1.0) {
    if (-1.0 < fVar8) {
      fVar12 = SQRT(1.0 - fVar8 * fVar8);
      fVar13 = fVar8 * major[0] + fVar12 * fVar4;
      fVar10 = fVar3 * fVar12 + fVar8 * major[1];
      fVar9 = fVar8 * major[2] + fVar2 * fVar12 + fVar11;
      paMVar1 = outVert + *numVert;
      paMVar7 = ins->mTM;
      (*paMVar1)[0] =
           fVar9 * paMVar7[2][0] + fVar13 * (*paMVar7)[0] + fVar10 * paMVar7[1][0] + paMVar7[3][0];
      (*paMVar1)[1] =
           fVar9 * paMVar7[2][1] + fVar10 * paMVar7[1][1] + fVar13 * (*paMVar7)[1] + paMVar7[3][1];
      (*paMVar1)[2] =
           fVar9 * paMVar7[2][2] + fVar10 * paMVar7[1][2] + fVar13 * (*paMVar7)[2] + paMVar7[3][2];
      iVar15 = *numVert + 1;
      *numVert = iVar15;
      if (maxVert <= iVar15) {
        return;
      }
      fVar12 = fVar12 * -2.0;
      paMVar1 = outVert + iVar15;
      fVar13 = fVar4 * fVar12 + fVar13;
      fVar10 = fVar3 * fVar12 + fVar10;
      fVar9 = fVar12 * fVar2 + fVar9;
      paMVar7 = ins->mTM;
      (*paMVar1)[0] =
           fVar9 * paMVar7[2][0] + fVar13 * (*paMVar7)[0] + fVar10 * paMVar7[1][0] + paMVar7[3][0];
      (*paMVar1)[1] =
           fVar9 * paMVar7[2][1] + fVar13 * (*paMVar7)[1] + fVar10 * paMVar7[1][1] + paMVar7[3][1];
      (*paMVar1)[2] =
           fVar9 * paMVar7[2][2] + fVar13 * (*paMVar7)[2] + fVar10 * paMVar7[1][2] + paMVar7[3][2];
      goto LAB_00010f33;
    }
  }
  else {
    fVar9 = major[2] + fVar11;
    paMVar1 = outVert + *numVert;
    paMVar7 = ins->mTM;
    (*paMVar1)[0] =
         fVar9 * paMVar7[2][0] + major[0] * (*paMVar7)[0] + major[1] * paMVar7[1][0] + paMVar7[3][0]
    ;
    (*paMVar1)[1] =
         fVar9 * paMVar7[2][1] + major[0] * (*paMVar7)[1] + major[1] * paMVar7[1][1] + paMVar7[3][1]
    ;
    (*paMVar1)[2] =
         fVar9 * paMVar7[2][2] + major[0] * (*paMVar7)[2] + major[1] * paMVar7[1][2] + paMVar7[3][2]
    ;
LAB_00010f33:
    iVar15 = *numVert;
    *numVert = iVar15 + 1;
    if (maxVert <= iVar15 + 1) {
      return;
    }
    pMVar5 = pMVar6[1].prev;
  }
  fVar9 = ((float)pMVar5 + fVar11) * (1.0 / major[2]);
  if (fVar9 < 1.0) {
    if (-1.0 < fVar9) {
                    /* Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
      fVar13 = SQRT(1.0 - fVar9 * fVar9);
      fVar10 = -fVar9;
      fVar14 = fVar13 * fVar4 + major[0] * fVar10;
      fVar12 = fVar3 * fVar13 + major[1] * fVar10;
      fVar10 = major[2] * fVar10 + fVar2 * fVar13 + fVar11;
      paMVar1 = outVert + *numVert;
      paMVar7 = ins->mTM;
      (*paMVar1)[0] =
           fVar10 * paMVar7[2][0] + fVar14 * (*paMVar7)[0] + fVar12 * paMVar7[1][0] + paMVar7[3][0];
      (*paMVar1)[1] =
           fVar10 * paMVar7[2][1] + fVar12 * paMVar7[1][1] + fVar14 * (*paMVar7)[1] + paMVar7[3][1];
      (*paMVar1)[2] =
           fVar10 * paMVar7[2][2] + fVar12 * paMVar7[1][2] + fVar14 * (*paMVar7)[2] + paMVar7[3][2];
      iVar15 = *numVert + 1;
      *numVert = iVar15;
      if (maxVert <= iVar15) {
        return;
      }
      fVar13 = fVar13 * -2.0;
      paMVar1 = outVert + iVar15;
      fVar14 = fVar4 * fVar13 + fVar14;
      fVar12 = fVar3 * fVar13 + fVar12;
      fVar10 = fVar13 * fVar2 + fVar10;
      paMVar7 = ins->mTM;
      (*paMVar1)[0] =
           fVar10 * paMVar7[2][0] + fVar14 * (*paMVar7)[0] + fVar12 * paMVar7[1][0] + paMVar7[3][0];
      (*paMVar1)[1] =
           fVar10 * paMVar7[2][1] + fVar14 * (*paMVar7)[1] + fVar12 * paMVar7[1][1] + paMVar7[3][1];
      (*paMVar1)[2] =
           fVar10 * paMVar7[2][2] + fVar14 * (*paMVar7)[2] + fVar12 * paMVar7[1][2] + paMVar7[3][2];
      goto LAB_00010fe1;
    }
  }
  else {
    fVar2 = fVar11 - major[2];
    fVar12 = -major[0];
    fVar10 = -major[1];
    paMVar1 = outVert + *numVert;
    paMVar7 = ins->mTM;
    (*paMVar1)[0] =
         fVar2 * paMVar7[2][0] + fVar12 * (*paMVar7)[0] + fVar10 * paMVar7[1][0] + paMVar7[3][0];
    (*paMVar1)[1] =
         fVar2 * paMVar7[2][1] + fVar12 * (*paMVar7)[1] + fVar10 * paMVar7[1][1] + paMVar7[3][1];
    (*paMVar1)[2] =
         fVar12 * (*paMVar7)[2] + fVar10 * paMVar7[1][2] + fVar2 * paMVar7[2][2] + paMVar7[3][2];
LAB_00010fe1:
    iVar15 = *numVert;
    *numVert = iVar15 + 1;
    if (maxVert <= iVar15 + 1) {
      return;
    }
  }
  if (fVar8 <= 0.2) {
    return;
  }
  if (fVar9 <= 0.2) {
    return;
  }
  paMVar1 = outVert + *numVert;
  paMVar7 = ins->mTM;
  (*paMVar1)[0] =
       fVar11 * paMVar7[2][0] + fVar4 * (*paMVar7)[0] + fVar3 * paMVar7[1][0] + paMVar7[3][0];
  (*paMVar1)[1] =
       fVar11 * paMVar7[2][1] + fVar4 * (*paMVar7)[1] + fVar3 * paMVar7[1][1] + paMVar7[3][1];
  (*paMVar1)[2] =
       fVar11 * paMVar7[2][2] + fVar4 * (*paMVar7)[2] + fVar3 * paMVar7[1][2] + paMVar7[3][2];
  iVar15 = *numVert + 1;
  *numVert = iVar15;
  if (maxVert <= iVar15) {
    return;
  }
  fVar4 = -fVar4;
  fVar3 = -fVar3;
  paMVar1 = outVert + iVar15;
  paMVar7 = ins->mTM;
  (*paMVar1)[0] =
       fVar11 * paMVar7[2][0] + fVar3 * paMVar7[1][0] + fVar4 * (*paMVar7)[0] + paMVar7[3][0];
  (*paMVar1)[1] =
       fVar11 * paMVar7[2][1] + fVar4 * (*paMVar7)[1] + fVar3 * paMVar7[1][1] + paMVar7[3][1];
  (*paMVar1)[2] =
       fVar11 * paMVar7[2][2] + fVar4 * (*paMVar7)[2] + fVar3 * paMVar7[1][2] + paMVar7[3][2];
LAB_00010b19:
  *numVert = *numVert + 1;
  return;
}


/* ==== McdGeometryInstanceGetSlice ==== */

void McdGeometryInstanceGetSlice
               (McdGeometryInstanceID ins,MeReal *normal,MeReal dist,int maxVert,int *numVert,
               MeVector3 *outVert)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  MeMatrix4Ptr paMVar7;
  
                    /* Unresolved local var: MeReal d2@[DW_OP_reg12(ST1)] */
  *numVert = 0;
  if (0 < maxVert) {
    switch((char)ins->mGeometry->mRefCtAndID) {
    case '\0':
    case '\x05':
    case '\x06':
      break;
    case '\x01':
    case '\x03':
    case '\b':
      *numVert = 1;
      paMVar7 = ins->mTM;
      fVar1 = normal[1];
      fVar2 = paMVar7[3][0];
      fVar3 = paMVar7[3][1];
      fVar4 = *normal;
      fVar5 = normal[2];
      fVar6 = paMVar7[3][2];
      (*outVert)[0] = fVar2;
      fVar4 = dist - (fVar2 * fVar4 + fVar1 * fVar3 + fVar5 * fVar6);
      fVar1 = paMVar7[3][1];
      (*outVert)[1] = fVar1;
      fVar3 = paMVar7[3][2];
      (*outVert)[2] = fVar3;
      (*outVert)[0] = fVar4 * *normal + fVar2;
      (*outVert)[1] = fVar4 * normal[1] + fVar1;
      (*outVert)[2] = fVar4 * normal[2] + fVar3;
      break;
    case '\x02':
      McdBoxGetSlice(ins,normal,dist,maxVert,numVert,outVert);
      return;
    case '\x04':
      McdCylinderGetSlice(ins,normal,dist,maxVert,numVert,outVert);
      return;
    case '\a':
      McdConvexMeshGetSlice(ins,normal,dist,maxVert,numVert,outVert);
      return;
    }
  }
  return;
}


/* ==== McdPlaneIntersectTest ==== */

int McdPlaneIntersectTest
              (McdModelID_conflict modplane,McdModelID_conflict mod,McdIntersectResult *result)

{
  McdGeometryInstance *ins;
  float fVar1;
  float fVar2;
  MeMatrix4Ptr paMVar3;
  float fVar4;
  uint uVar5;
  float *pfVar6;
  float fVar7;
  int iVar8;
  McdContact *pMVar9;
  int iVar10;
  MeVector3 *outVert;
  longdouble lVar11;
  longdouble lVar12;
  longdouble lVar13;
  MeReal eps;
  int numv;
  MeReal vert [100] [3];
  MeReal p [3];
  MeReal neg [3];
  MeReal norm [3];
  
                    /* Unresolved local var: McdGeometryInstanceID plane@[???]
                       Unresolved local var: McdGeometryInstanceID ins@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeReal dist@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal sep@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal fat@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal dot@[???]
                       Unresolved local var: MeReal slice@[???]
                       Unresolved local var: MeReal pad@[???]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  paMVar3 = (modplane->mInstance).mTM;
  norm[0] = paMVar3[2][0];
  norm[1] = paMVar3[2][1];
  norm[2] = paMVar3[2][2];
  fVar4 = norm[1] * paMVar3[3][1];
  fVar2 = norm[0] * paMVar3[3][0];
  fVar7 = norm[2] * paMVar3[3][2];
  fVar1 = ((modplane->mInstance).mGeometry)->frame->mScale;
  ins = &mod->mInstance;
  neg[0] = norm[0] * -1.0;
  neg[1] = norm[1] * -1.0;
  neg[2] = norm[2] * -1.0;
  McdGjkMaximumPoint(ins,neg,p);
  lVar11 = (longdouble)McdGjkFatness(ins);
  lVar12 = (longdouble)(fVar7 + fVar2 + fVar4);
  fVar2 = modplane->mPadding;
  fVar7 = mod->mPadding;
  result->contactCount = 0;
  lVar13 = (((longdouble)p[2] * (longdouble)norm[2] +
            (longdouble)p[0] * (longdouble)norm[0] + (longdouble)p[1] * (longdouble)norm[1]) -
           lVar11) - lVar12;
  result->normal[0] = norm[0];
  uVar5 = (uint)(lVar13 <= (longdouble)fVar2 + (longdouble)fVar7);
  result->normal[1] = norm[1];
  result->normal[2] = norm[2];
  iVar8 = 0;
  result->touch = uVar5;
  if (uVar5 != 0) {
    outVert = vert;
    fVar2 = (float)lVar13;
    iVar8 = 0;
    McdGeometryInstanceGetSlice
              (ins,norm,(float)(lVar12 + lVar11 + (longdouble)(fVar1 * 0.005)),100,&numv,outVert);
    if ((0 < numv) && (0 < result->contactMaxCount)) {
      fVar1 = -(float)lVar11;
      iVar10 = 0;
      do {
        pMVar9 = result->contacts;
        *(MeReal *)((int)pMVar9->normal + iVar10) = norm[0];
        *(MeReal *)((int)pMVar9->normal + iVar10 + 4) = norm[1];
        *(MeReal *)((int)pMVar9->normal + iVar10 + 8) = norm[2];
        pfVar6 = (float *)((int)result->contacts->position + iVar10);
        *pfVar6 = fVar1 * norm[0] + (*outVert)[0];
        pfVar6[1] = fVar1 * norm[1] + (*outVert)[1];
        pfVar6[2] = fVar1 * norm[2] + (*outVert)[2];
        fVar7 = fVar2;
        if (0.0 < fVar2) {
          fVar7 = 0.0;
        }
        iVar8 = iVar8 + 1;
        outVert = outVert + 1;
        *(float *)((int)result->contacts->normal + iVar10 + 0xc) = fVar7;
      } while ((iVar8 < numv) && (iVar10 = iVar10 + 0x28, iVar8 < result->contactMaxCount));
    }
    pMVar9 = result->contacts;
    pMVar9[iVar8].normal[0] = norm[0];
    fVar1 = -(float)lVar11;
    pMVar9[iVar8].normal[1] = norm[1];
    pMVar9[iVar8].normal[2] = norm[2];
    pMVar9 = result->contacts + iVar8;
    pMVar9->position[0] = fVar1 * norm[0] + p[0];
    pMVar9->position[1] = fVar1 * norm[1] + p[1];
    pMVar9->position[2] = fVar1 * norm[2] + p[2];
    if (0.0 < fVar2) {
      fVar2 = 0.0;
    }
    result->contacts[iVar8].separation = fVar2;
    result->contactCount = iVar8 + 1;
    iVar8 = result->touch;
  }
  return iVar8;
}


/* ==== McdPlaneIntersect ==== */

int McdPlaneIntersect(McdModelPair *p,McdIntersectResult *result)

{
  int iVar1;
  McdModelID_conflict modplane;
  McdModelID_conflict mod;
  
  mod = p->model1;
  if ((char)((mod->mInstance).mGeometry)->mRefCtAndID == '\x03') {
                    /* Unresolved local var: int result2@[???] */
    modplane = mod;
    mod = p->model2;
  }
  else {
    modplane = p->model2;
    if ((char)((modplane->mInstance).mGeometry)->mRefCtAndID != '\x03') {
      result->contactCount = 0;
      result->touch = 0;
      return 0;
    }
  }
  iVar1 = McdPlaneIntersectTest(modplane,mod,result);
  return iVar1;
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


