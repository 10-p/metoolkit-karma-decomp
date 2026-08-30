/* ==== McdGjkFatness ==== */

MeReal McdGjkFatness(McdGeometryInstanceID ins)

{
  MeReal MVar1;
  byte bVar2;
  McdGeometry *pMVar3;
  
  pMVar3 = ins->mGeometry;
  bVar2 = (byte)pMVar3->mRefCtAndID;
  if (bVar2 == 5) {
LAB_00010020:
                    /* Unresolved local var: McdSphyl * s@[???] */
    MVar1 = (MeReal)pMVar3[1].mRefCtAndID;
  }
  else {
    if (bVar2 < 6) {
      if (bVar2 == 1) goto LAB_00010020;
    }
    else if (bVar2 == 7) {
                    /* Unresolved local var: McdConvexMesh * cnv@[???] */
      return (MeReal)pMVar3[2].frame;
    }
    MVar1 = 0.0;
  }
  return MVar1;
}


/* ==== McdBoxMaximumPointNew ==== */

void McdBoxMaximumPointNew(McdGeometryInstanceID ins,MeReal *inDir,MeReal *outPoint)

{
  float fVar1;
  float fVar2;
  float fVar3;
  int iVar4;
  float *pfVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  MeReal c [3];
  MeReal n [3];
  
                    /* Unresolved local var: McdBox * b@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg2(EDX)] */
  iVar4 = McdGeometryInstanceGetGeometry(ins);
  pfVar5 = (float *)McdGeometryInstanceGetTransformPtr(ins);
  fVar1 = *inDir;
  fVar2 = inDir[1];
  fVar3 = inDir[2];
  fVar6 = *(float *)(iVar4 + 0x10);
  if (fVar3 * pfVar5[2] + fVar1 * *pfVar5 + fVar2 * pfVar5[1] < 0.0) {
    fVar6 = -fVar6;
  }
  fVar7 = *(float *)(iVar4 + 0x14);
  if (fVar3 * pfVar5[6] + fVar1 * pfVar5[4] + fVar2 * pfVar5[5] < 0.0) {
    fVar7 = -fVar7;
  }
  fVar8 = *(float *)(iVar4 + 0x18);
  if (fVar1 * pfVar5[8] + fVar2 * pfVar5[9] + fVar3 * pfVar5[10] < 0.0) {
    fVar8 = -fVar8;
  }
  *outPoint = fVar8 * pfVar5[8] + fVar6 * *pfVar5 + fVar7 * pfVar5[4] + pfVar5[0xc];
  outPoint[1] = fVar8 * pfVar5[9] + fVar7 * pfVar5[5] + fVar6 * pfVar5[1] + pfVar5[0xd];
  outPoint[2] = fVar8 * pfVar5[10] + fVar6 * pfVar5[2] + fVar7 * pfVar5[6] + pfVar5[0xe];
  return;
}


/* ==== McdConvexMeshMaximumPointLocal ==== */

MeReal McdConvexMeshMaximumPointLocal
                 (McdConvexMesh *conv,MeReal *inDir,int hint,MeReal minDist,int *outIndex)

{
  McdCnvVertex *pMVar1;
  McdCnvVertex *pMVar2;
  float fVar3;
  int iVar4;
  int iVar5;
  McdCnvVertex *pMVar6;
  undefined4 uVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  McdConvexHull *pMVar12;
  ushort uVar13;
  MeReal d1;
  McdCnvVertex *vert;
  int n;
  int prev;
  int next;
  int m;
  int t;
  int i;
  
                    /* Unresolved local var: int j@[DW_OP_reg3(EBX)]
                       Unresolved local var: int k@[???]
                       Unresolved local var: MeReal d2@[???] */
  pMVar1 = (McdCnvVertex *)(conv->mHull).numVertex;
  pMVar2 = (conv->mHull).vertex;
  if ((hint < 0) || (next = hint, (int)pMVar1 <= hint)) {
    next = 0;
  }
  pMVar6 = pMVar2 + next;
  t = -2;
  i = -1;
  d1 = pMVar6->position[2] * inDir[2] +
       pMVar6->position[1] * inDir[1] + pMVar6->position[0] * *inDir;
  if ((-2 < (int)pMVar1) && (pMVar6 = pMVar2, next != -1)) {
    while (iVar5 = i, iVar4 = next,
          uVar13 = (ushort)(d1 < minDist) << 8 | (ushort)(NAN(d1) || NAN(minDist)) << 10 |
                   (ushort)(d1 == minDist) << 0xe,
          uVar7 = CONCAT22((short)((uint)pMVar6 >> 0x10),uVar13), (char)(uVar13 >> 8) != '\0') {
      pMVar12 = &conv->mHull;
      i = next;
      uVar8 = McdCnvVertexGetCount(pMVar12,next,uVar7,uVar7);
      iVar11 = 0;
      if (0 < (int)uVar8) {
        if (((int)uVar8 < 2) || (iVar9 = 0, (uVar8 & 1) != 0)) {
          iVar11 = McdCnvVertexGetNeighbor(pMVar12,next,0,uVar8 & 1);
          iVar9 = iVar11;
          if (iVar11 != iVar5) {
            pMVar6 = pMVar2 + iVar11;
            fVar3 = pMVar6->position[2] * inDir[2] +
                    pMVar6->position[1] * inDir[1] + pMVar6->position[0] * *inDir;
            uVar13 = (ushort)(fVar3 < d1) << 8 | (ushort)(NAN(fVar3) || NAN(d1)) << 10 |
                     (ushort)(fVar3 == d1) << 0xe;
            iVar9 = CONCAT22((short)((uint)pMVar6 >> 0x10),uVar13);
            if ((char)(uVar13 >> 8) == '\0') {
              d1 = fVar3;
              next = iVar11;
            }
          }
          iVar11 = 1;
          if ((int)uVar8 < 2) goto LAB_00010343;
        }
        do {
          iVar9 = McdCnvVertexGetNeighbor(pMVar12,iVar4,iVar11,iVar9);
          if (iVar9 != iVar5) {
            pMVar6 = pMVar2 + iVar9;
            fVar3 = pMVar6->position[2] * inDir[2] +
                    pMVar6->position[1] * inDir[1] + pMVar6->position[0] * *inDir;
            if (d1 < fVar3) {
              d1 = fVar3;
              next = iVar9;
            }
          }
          iVar10 = McdCnvVertexGetNeighbor(pMVar12,iVar4,iVar11 + 1,iVar9);
          iVar9 = iVar10;
          if (iVar10 != iVar5) {
            pMVar6 = pMVar2 + iVar10;
            fVar3 = pMVar6->position[2] * inDir[2] +
                    pMVar6->position[1] * inDir[1] + pMVar6->position[0] * *inDir;
            uVar13 = (ushort)(fVar3 < d1) << 8 | (ushort)(NAN(fVar3) || NAN(d1)) << 10 |
                     (ushort)(fVar3 == d1) << 0xe;
            iVar9 = CONCAT22((short)((uint)pMVar6 >> 0x10),uVar13);
            if ((char)(uVar13 >> 8) == '\0') {
              d1 = fVar3;
              next = iVar10;
            }
          }
          iVar11 = iVar11 + 2;
        } while (iVar11 < (int)uVar8);
      }
LAB_00010343:
      t = t + 1;
      if (((int)pMVar1 <= t) || (pMVar6 = pMVar1, iVar4 == next)) break;
    }
  }
  *outIndex = next;
  return d1;
}


/* ==== McdConvexMeshMaximumPointNew ==== */

MeReal McdConvexMeshMaximumPointNew(McdGeometryInstanceID ins,MeReal *inDir,MeReal *outPoint)

{
  float fVar1;
  float fVar2;
  float fVar3;
  McdConvexMesh *conv;
  float *pfVar4;
  McdCnvVertex *pMVar5;
  MeReal MVar6;
  int i;
  MeReal temp [3];
  
                    /* Unresolved local var: McdConvexMesh * conv@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal d@[DW_OP_reg11(ST0)] */
  conv = (McdConvexMesh *)McdGeometryInstanceGetGeometry(ins);
  pfVar4 = (float *)McdGeometryInstanceGetTransformPtr(ins);
  fVar1 = *inDir;
  fVar2 = inDir[1];
  fVar3 = inDir[2];
  temp[0] = fVar3 * pfVar4[2] + fVar2 * pfVar4[1] + fVar1 * *pfVar4;
  temp[1] = fVar3 * pfVar4[6] + fVar1 * pfVar4[4] + fVar2 * pfVar4[5];
  temp[2] = fVar3 * pfVar4[10] + fVar1 * pfVar4[8] + fVar2 * pfVar4[9];
  MVar6 = McdConvexMeshMaximumPointLocal(conv,temp,0,3.4028235e+38,&i);
  pMVar5 = (conv->mHull).vertex + i;
  *outPoint = pfVar4[8] * pMVar5->position[2] +
              pfVar4[4] * pMVar5->position[1] + *pfVar4 * pMVar5->position[0] + pfVar4[0xc];
  outPoint[1] = pfVar4[9] * pMVar5->position[2] +
                pfVar4[5] * pMVar5->position[1] + pfVar4[1] * pMVar5->position[0] + pfVar4[0xd];
  outPoint[2] = pfVar4[10] * pMVar5->position[2] +
                pfVar4[6] * pMVar5->position[1] + pfVar4[2] * pMVar5->position[0] + pfVar4[0xe];
  return MVar6;
}


/* ==== McdGjkMaximumPoint ==== */

void McdGjkMaximumPoint(McdGeometryInstanceID ins,MeReal *v,MeReal *out)

{
  float fVar1;
  float fVar2;
  float fVar3;
  byte bVar4;
  MeMatrix4Ptr paMVar5;
  McdGeometry *pMVar6;
  uint uVar7;
  McdConvexMesh *conv;
  float *pfVar8;
  McdFrameworkID pMVar9;
  int iVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  McdCnvVertex *pMVar14;
  int i;
  MeReal temp [3];
  MeReal c [3];
  MeReal n [3];
  
  uVar7 = (uint)(byte)ins->mGeometry->mRefCtAndID;
  if (uVar7 == 2) {
                    /* Unresolved local var: McdBox * b@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg2(EDX)] */
    iVar10 = McdGeometryInstanceGetGeometry(ins);
    pfVar8 = (float *)McdGeometryInstanceGetTransformPtr(ins);
    fVar1 = *v;
    fVar2 = v[1];
    fVar3 = v[2];
    fVar11 = *(float *)(iVar10 + 0x10);
    if (fVar3 * pfVar8[2] + fVar1 * *pfVar8 + fVar2 * pfVar8[1] < 0.0) {
      fVar11 = -fVar11;
    }
    fVar12 = *(float *)(iVar10 + 0x14);
    if (fVar3 * pfVar8[6] + fVar1 * pfVar8[4] + fVar2 * pfVar8[5] < 0.0) {
      fVar12 = -fVar12;
    }
    fVar13 = *(float *)(iVar10 + 0x18);
    if (fVar1 * pfVar8[8] + fVar2 * pfVar8[9] + fVar3 * pfVar8[10] < 0.0) {
      fVar13 = -fVar13;
    }
    *out = fVar13 * pfVar8[8] + fVar11 * *pfVar8 + fVar12 * pfVar8[4] + pfVar8[0xc];
    out[1] = fVar13 * pfVar8[9] + fVar12 * pfVar8[5] + fVar11 * pfVar8[1] + pfVar8[0xd];
    out[2] = fVar13 * pfVar8[10] + fVar11 * pfVar8[2] + fVar12 * pfVar8[6] + pfVar8[0xe];
    return;
  }
  if (uVar7 < 3) {
    uVar7 = uVar7 - 1;
    if (uVar7 == 0) {
      paMVar5 = ins->mTM;
      *out = paMVar5[3][0];
      out[1] = paMVar5[3][1];
      out[2] = paMVar5[3][2];
      return;
    }
LAB_000104c9:
    McdGeometryInstanceMaximumPoint(ins,v,out,uVar7);
    return;
  }
  if (uVar7 != 5) {
    if (uVar7 == 7) {
                    /* Unresolved local var: McdConvexMesh * conv@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal d@[???] */
      conv = (McdConvexMesh *)McdGeometryInstanceGetGeometry(ins);
      pfVar8 = (float *)McdGeometryInstanceGetTransformPtr(ins);
      fVar1 = *v;
      fVar2 = v[1];
      fVar3 = v[2];
      temp[0] = fVar3 * pfVar8[2] + fVar2 * pfVar8[1] + fVar1 * *pfVar8;
      temp[1] = fVar3 * pfVar8[6] + fVar1 * pfVar8[4] + fVar2 * pfVar8[5];
      temp[2] = fVar3 * pfVar8[10] + fVar1 * pfVar8[8] + fVar2 * pfVar8[9];
      McdConvexMeshMaximumPointLocal(conv,temp,0,3.4028235e+38,&i);
      pMVar14 = (conv->mHull).vertex + i;
      *out = pfVar8[8] * pMVar14->position[2] +
             pfVar8[4] * pMVar14->position[1] + *pfVar8 * pMVar14->position[0] + pfVar8[0xc];
      out[1] = pfVar8[9] * pMVar14->position[2] +
               pfVar8[5] * pMVar14->position[1] + pfVar8[1] * pMVar14->position[0] + pfVar8[0xd];
      out[2] = pfVar8[10] * pMVar14->position[2] +
               pfVar8[6] * pMVar14->position[1] + pfVar8[2] * pMVar14->position[0] + pfVar8[0xe];
      return;
    }
    goto LAB_000104c9;
  }
  McdGeometryInstanceMaximumPoint(ins,v,out);
  pMVar6 = ins->mGeometry;
  bVar4 = (byte)pMVar6->mRefCtAndID;
  if (bVar4 == 5) {
LAB_00010655:
                    /* Unresolved local var: McdSphyl * s@[???] */
    pMVar9 = (McdFrameworkID)pMVar6[1].mRefCtAndID;
  }
  else {
    if (bVar4 < 6) {
      if (bVar4 == 1) goto LAB_00010655;
    }
    else if (bVar4 == 7) {
                    /* Unresolved local var: McdConvexMesh * cnv@[???] */
      pMVar9 = pMVar6[2].frame;
      goto LAB_00010629;
    }
    pMVar9 = (McdFrameworkID)0x0;
  }
LAB_00010629:
  fVar1 = -(float)pMVar9;
  *out = fVar1 * *v + *out;
  out[1] = fVar1 * v[1] + out[1];
  out[2] = fVar1 * v[2] + out[2];
  return;
}


