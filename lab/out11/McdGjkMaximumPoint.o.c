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
  void *pvVar4;
  float *pfVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  MeReal c [3];
  MeReal n [3];
  
                    /* Unresolved local var: McdBox * b@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg2(EDX)] */
  pvVar4 = McdGeometryInstanceGetGeometry(ins);
  pfVar5 = McdGeometryInstanceGetTransformPtr(ins);
  fVar1 = *inDir;
  fVar2 = inDir[1];
  fVar3 = inDir[2];
  fVar6 = *(float *)((int)pvVar4 + 0x10);
  if (fVar3 * pfVar5[2] + fVar1 * *pfVar5 + fVar2 * pfVar5[1] < 0.0) {
    fVar6 = -fVar6;
  }
  fVar7 = *(float *)((int)pvVar4 + 0x14);
  if (fVar3 * pfVar5[6] + fVar1 * pfVar5[4] + fVar2 * pfVar5[5] < 0.0) {
    fVar7 = -fVar7;
  }
  fVar8 = *(float *)((int)pvVar4 + 0x18);
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
  int iVar1;
  McdCnvVertex *pMVar2;
  float fVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  McdCnvVertex *pMVar8;
  int iVar9;
  McdConvexHull *pMVar10;
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
  iVar1 = (conv->mHull).numVertex;
  pMVar2 = (conv->mHull).vertex;
  if ((hint < 0) || (next = hint, iVar1 <= hint)) {
    next = 0;
  }
  pMVar8 = pMVar2 + next;
  t = -2;
  i = -1;
  d1 = pMVar8->position[2] * inDir[2] +
       pMVar8->position[1] * inDir[1] + pMVar8->position[0] * *inDir;
  if ((-2 < iVar1) && (next != -1)) {
    while (iVar5 = i, iVar4 = next, d1 <= minDist) {
      pMVar10 = &conv->mHull;
      i = next;
      uVar6 = McdCnvVertexGetCount(pMVar10,next);
      iVar9 = 0;
      if (0 < (int)uVar6) {
        if (((int)uVar6 < 2) || ((uVar6 & 1) != 0)) {
          iVar9 = McdCnvVertexGetNeighbor(pMVar10,next,0);
          if (iVar9 != iVar5) {
            pMVar8 = pMVar2 + iVar9;
            fVar3 = pMVar8->position[2] * inDir[2] +
                    pMVar8->position[1] * inDir[1] + pMVar8->position[0] * *inDir;
            if (d1 < fVar3) {
              d1 = fVar3;
              next = iVar9;
            }
          }
          iVar9 = 1;
          if ((int)uVar6 < 2) goto LAB_00010343;
        }
        do {
          iVar7 = McdCnvVertexGetNeighbor(pMVar10,iVar4,iVar9);
          if (iVar7 != iVar5) {
            pMVar8 = pMVar2 + iVar7;
            fVar3 = pMVar8->position[2] * inDir[2] +
                    pMVar8->position[1] * inDir[1] + pMVar8->position[0] * *inDir;
            if (d1 < fVar3) {
              d1 = fVar3;
              next = iVar7;
            }
          }
          iVar7 = McdCnvVertexGetNeighbor(pMVar10,iVar4,iVar9 + 1);
          if (iVar7 != iVar5) {
            pMVar8 = pMVar2 + iVar7;
            fVar3 = pMVar8->position[2] * inDir[2] +
                    pMVar8->position[1] * inDir[1] + pMVar8->position[0] * *inDir;
            if (d1 < fVar3) {
              d1 = fVar3;
              next = iVar7;
            }
          }
          iVar9 = iVar9 + 2;
        } while (iVar9 < (int)uVar6);
      }
LAB_00010343:
      t = t + 1;
      if ((iVar1 <= t) || (iVar4 == next)) break;
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
  conv = McdGeometryInstanceGetGeometry(ins);
  pfVar4 = McdGeometryInstanceGetTransformPtr(ins);
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
  McdConvexMesh *conv;
  float *pfVar7;
  McdFrameworkID pMVar8;
  void *pvVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  McdCnvVertex *pMVar13;
  int i;
  MeReal temp [3];
  MeReal c [3];
  MeReal n [3];
  
  bVar4 = (byte)ins->mGeometry->mRefCtAndID;
  if (bVar4 == 2) {
                    /* Unresolved local var: McdBox * b@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg2(EDX)] */
    pvVar9 = McdGeometryInstanceGetGeometry(ins);
    pfVar7 = McdGeometryInstanceGetTransformPtr(ins);
    fVar1 = *v;
    fVar2 = v[1];
    fVar3 = v[2];
    fVar10 = *(float *)((int)pvVar9 + 0x10);
    if (fVar3 * pfVar7[2] + fVar1 * *pfVar7 + fVar2 * pfVar7[1] < 0.0) {
      fVar10 = -fVar10;
    }
    fVar11 = *(float *)((int)pvVar9 + 0x14);
    if (fVar3 * pfVar7[6] + fVar1 * pfVar7[4] + fVar2 * pfVar7[5] < 0.0) {
      fVar11 = -fVar11;
    }
    fVar12 = *(float *)((int)pvVar9 + 0x18);
    if (fVar1 * pfVar7[8] + fVar2 * pfVar7[9] + fVar3 * pfVar7[10] < 0.0) {
      fVar12 = -fVar12;
    }
    *out = fVar12 * pfVar7[8] + fVar10 * *pfVar7 + fVar11 * pfVar7[4] + pfVar7[0xc];
    out[1] = fVar12 * pfVar7[9] + fVar11 * pfVar7[5] + fVar10 * pfVar7[1] + pfVar7[0xd];
    out[2] = fVar12 * pfVar7[10] + fVar10 * pfVar7[2] + fVar11 * pfVar7[6] + pfVar7[0xe];
    return;
  }
  if (bVar4 < 3) {
    if (bVar4 == 1) {
      paMVar5 = ins->mTM;
      *out = paMVar5[3][0];
      out[1] = paMVar5[3][1];
      out[2] = paMVar5[3][2];
      return;
    }
LAB_000104c9:
    McdGeometryInstanceMaximumPoint(ins,v,out);
    return;
  }
  if (bVar4 != 5) {
    if (bVar4 == 7) {
                    /* Unresolved local var: McdConvexMesh * conv@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal d@[???] */
      conv = McdGeometryInstanceGetGeometry(ins);
      pfVar7 = McdGeometryInstanceGetTransformPtr(ins);
      fVar1 = *v;
      fVar2 = v[1];
      fVar3 = v[2];
      temp[0] = fVar3 * pfVar7[2] + fVar2 * pfVar7[1] + fVar1 * *pfVar7;
      temp[1] = fVar3 * pfVar7[6] + fVar1 * pfVar7[4] + fVar2 * pfVar7[5];
      temp[2] = fVar3 * pfVar7[10] + fVar1 * pfVar7[8] + fVar2 * pfVar7[9];
      McdConvexMeshMaximumPointLocal(conv,temp,0,3.4028235e+38,&i);
      pMVar13 = (conv->mHull).vertex + i;
      *out = pfVar7[8] * pMVar13->position[2] +
             pfVar7[4] * pMVar13->position[1] + *pfVar7 * pMVar13->position[0] + pfVar7[0xc];
      out[1] = pfVar7[9] * pMVar13->position[2] +
               pfVar7[5] * pMVar13->position[1] + pfVar7[1] * pMVar13->position[0] + pfVar7[0xd];
      out[2] = pfVar7[10] * pMVar13->position[2] +
               pfVar7[6] * pMVar13->position[1] + pfVar7[2] * pMVar13->position[0] + pfVar7[0xe];
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
    pMVar8 = (McdFrameworkID)pMVar6[1].mRefCtAndID;
  }
  else {
    if (bVar4 < 6) {
      if (bVar4 == 1) goto LAB_00010655;
    }
    else if (bVar4 == 7) {
                    /* Unresolved local var: McdConvexMesh * cnv@[???] */
      pMVar8 = pMVar6[2].frame;
      goto LAB_00010629;
    }
    pMVar8 = (McdFrameworkID)0x0;
  }
LAB_00010629:
  fVar1 = -(float)pMVar8;
  *out = fVar1 * *v + *out;
  out[1] = fVar1 * v[1] + out[1];
  out[2] = fVar1 * v[2] + out[2];
  return;
}


