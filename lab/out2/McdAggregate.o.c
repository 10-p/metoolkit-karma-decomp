/* ==== McdAggregateGetTypeId ==== */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

MeI16 McdAggregateGetTypeId(void)

{
  return 8;
}


/* ==== McdAggregateRegisterType ==== */

/* WARNING: Unknown calling convention */

void McdAggregateRegisterType(McdFramework *frame)

{
  McdFrameworkRegisterGeometryType
            (frame,8,"McdAggregate",McdAggregateDestroy,McdAggregateUpdateAABB,
             McdAggregateGetBSphere,McdAggregateMaximumPoint,McdAggregateGetMassProperties,
             McdAggregateDebugDraw);
  return;
}


/* ==== McdAggregateCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdAggregateID McdAggregateCreate(McdFramework *frame,int maxChildren)

{
  McdAggregateID pMVar1;
  MeU32 MVar2;
  uint uVar3;
  McdAggregateID pMVar4;
  undefined4 extraout_ECX;
  int iVar5;
  
                    /* Unresolved local var: McdAggregate * a@[DW_OP_reg3(EBX)]
                       Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  pMVar1 = (McdAggregateID)(*_McdGeometryIncrementReferenceCount)(0x1c,0x10);
  pMVar4 = (McdAggregateID)0x0;
  if (pMVar1 != (McdAggregateID)0x0) {
    McdGeometryInit(pMVar1,frame,8,extraout_ECX);
    MVar2 = (*_MeMemoryAPI)(maxChildren * 0x44);
    pMVar1[1].mRefCtAndID = MVar2;
    pMVar1[1].next = (McdGeometryID)maxChildren;
    pMVar1[1].prev = (McdGeometryID)0x0;
    pMVar4 = pMVar1;
    if (0 < maxChildren) {
      iVar5 = 0;
      uVar3 = -maxChildren & 3;
      if (uVar3 != 0) {
        if (uVar3 < 3) {
          if (uVar3 < 2) {
            *(undefined4 *)(MVar2 + 0x40) = 0;
            iVar5 = 0x44;
            maxChildren = maxChildren + -1;
          }
          maxChildren = maxChildren + -1;
          *(undefined4 *)(pMVar1[1].mRefCtAndID + 0x40 + iVar5) = 0;
          iVar5 = iVar5 + 0x44;
        }
        *(undefined4 *)(pMVar1[1].mRefCtAndID + 0x40 + iVar5) = 0;
        iVar5 = iVar5 + 0x44;
        maxChildren = maxChildren + -1;
        if (maxChildren == 0) {
          return pMVar1;
        }
      }
      do {
        *(undefined4 *)(pMVar1[1].mRefCtAndID + 0x40 + iVar5) = 0;
        *(undefined4 *)(pMVar1[1].mRefCtAndID + 0x84 + iVar5) = 0;
        *(undefined4 *)(pMVar1[1].mRefCtAndID + 200 + iVar5) = 0;
        *(undefined4 *)(pMVar1[1].mRefCtAndID + 0x10c + iVar5) = 0;
        iVar5 = iVar5 + 0x110;
        maxChildren = maxChildren + -4;
      } while (maxChildren != 0);
    }
  }
  return pMVar4;
}


/* ==== McdAggregateAddElement ==== */

int McdAggregateAddElement(McdAggregateID g,McdGeometryID element,MeMatrix4Ptr relTM)

{
  int *piVar1;
  MeReal *pMVar2;
  McdGeometryID pMVar3;
  int iVar4;
  
                    /* Unresolved local var: McdAggregate * a@[???]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar4 = 0;
  pMVar3 = g[1].next;
  if ((int)pMVar3 < 1) {
    pMVar3 = g[1].next;
  }
  else if (*(int *)(g[1].mRefCtAndID + 0x40) != 0) {
    piVar1 = (int *)(g[1].mRefCtAndID + 0x40);
    do {
      iVar4 = iVar4 + 1;
      piVar1 = piVar1 + 0x11;
      if ((int)pMVar3 <= iVar4) goto LAB_00010182;
    } while (*piVar1 != 0);
  }
  if (iVar4 < (int)pMVar3) {
    pMVar2 = (MeReal *)(g[1].mRefCtAndID + iVar4 * 0x44);
    *pMVar2 = (*relTM)[0];
    pMVar2[1] = (*relTM)[1];
    pMVar2[2] = (*relTM)[2];
    pMVar2[3] = (*relTM)[3];
    pMVar2[4] = relTM[1][0];
    pMVar2[5] = relTM[1][1];
    pMVar2[6] = relTM[1][2];
    pMVar2[7] = relTM[1][3];
    pMVar2[8] = relTM[2][0];
    pMVar2[9] = relTM[2][1];
    pMVar2[10] = relTM[2][2];
    pMVar2[0xb] = relTM[2][3];
    pMVar2[0xc] = relTM[3][0];
    pMVar2[0xd] = relTM[3][1];
    pMVar2[0xe] = relTM[3][2];
    pMVar2[0xf] = relTM[3][3];
    *(McdGeometryID *)(g[1].mRefCtAndID + 0x40 + iVar4 * 0x44) = element;
    g[1].prev = (McdGeometryID)((int)&(g[1].prev)->mRefCtAndID + 1);
    McdGeometryIncrementReferenceCount(element);
  }
  else {
LAB_00010182:
    iVar4 = -1;
  }
  return iVar4;
}


/* ==== McdAggregateRemoveElement ==== */

void McdAggregateRemoveElement(McdAggregateID g,int element)

{
                    /* Unresolved local var: McdAggregate * a@[???] */
  McdGeometryDecrementReferenceCount(*(undefined4 *)(g[1].mRefCtAndID + 0x40 + element * 0x44));
  *(undefined4 *)(g[1].mRefCtAndID + 0x40 + element * 0x44) = 0;
  g[1].prev = (McdGeometryID)((int)&g[1].prev[-1].frame + 3);
  return;
}


/* ==== McdAggregateGetElementCount ==== */

int McdAggregateGetElementCount(McdAggregateID g)

{
                    /* Unresolved local var: McdAggregate * a@[???] */
  return (int)g[1].prev;
}


/* ==== McdAggregateGetElementCountMax ==== */

int McdAggregateGetElementCountMax(McdAggregateID g)

{
                    /* Unresolved local var: McdAggregate * a@[???] */
  return (int)g[1].next;
}


/* ==== McdAggregateGetElementGeometry ==== */

McdGeometryID McdAggregateGetElementGeometry(McdAggregateID g,int i)

{
  McdGeometryID pMVar1;
  
                    /* Unresolved local var: McdAggregate * a@[???] */
  if (i < (int)g[1].next) {
    pMVar1 = *(McdGeometryID *)(g[1].mRefCtAndID + 0x40 + i * 0x44);
  }
  else {
    pMVar1 = (McdGeometryID)0x0;
  }
  return pMVar1;
}


/* ==== McdAggregateGetElementTransformPtr ==== */

MeMatrix4Ptr McdAggregateGetElementTransformPtr(McdAggregateID g,int i)

{
  MeMatrix4Ptr paMVar1;
  
                    /* Unresolved local var: McdAggregate * a@[???] */
  if (i < (int)g[1].next) {
    paMVar1 = (MeMatrix4Ptr)(g[1].mRefCtAndID + i * 0x44);
  }
  else {
    paMVar1 = (MeMatrix4Ptr)0x0;
  }
  return paMVar1;
}


/* ==== McdAggregateDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdAggregateDestroy(McdGeometry *g)

{
  int iVar1;
  MeU32 MVar2;
  McdGeometryID pMVar3;
  int iVar4;
  int iVar5;
  int i;
  
  if (g != (McdGeometry *)0x0) {
                    /* Unresolved local var: McdAggregate * a@[???] */
    i = 0;
    pMVar3 = g[1].next;
    if ((0 < (int)pMVar3) && (MVar2 = g[1].mRefCtAndID, *(int *)(MVar2 + 0x40) != 0)) {
      iVar5 = 0;
      iVar4 = 0;
      do {
        iVar1 = *(int *)(MVar2 + 0x40 + iVar4);
        if (iVar1 != 0) {
          McdGeometryDecrementReferenceCount(iVar1);
          pMVar3 = g[1].next;
        }
        iVar5 = iVar5 + 0x44;
        i = i + 1;
        iVar4 = iVar4 + 0x44;
      } while ((i < (int)pMVar3) && (MVar2 = g[1].mRefCtAndID, *(int *)(MVar2 + 0x40 + iVar5) != 0))
      ;
    }
    (*_McdGeometryDecrementReferenceCount)(g[1].mRefCtAndID);
    McdGeometryDeinit();
    return;
  }
  return;
}


/* ==== McdAggregateUpdateAABB ==== */

void McdAggregateUpdateAABB(McdGeometryInstanceID ins,MeMatrix4Ptr finalTM,MeBool tight)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float fVar4;
  MeMatrix4Ptr paMVar5;
  int iVar6;
  MeReal (*paMVar7) [4];
  int iVar8;
  float *pfVar9;
  int iVar10;
  _McdGeometryInstance *p_Var11;
  int local_b4;
  int i;
  MeMatrix4Ptr elementFinalTMPtr;
  McdAggregate *a;
  MeReal elementFinalTM [4] [4];
  MeReal elementTM [4] [4];
  
                    /* Unresolved local var: McdGeometryInstanceID elementIns@[DW_OP_reg7(EDI)] */
  elementFinalTMPtr = (MeMatrix4Ptr)0x0;
  iVar6 = McdGeometryInstanceGetGeometry(ins);
  ins->min[0] = 3.4028235e+38;
  ins->min[1] = 3.4028235e+38;
  ins->min[2] = 3.4028235e+38;
  ins->max[0] = -3.4028235e+38;
  ins->max[1] = -3.4028235e+38;
  ins->max[2] = -3.4028235e+38;
  p_Var11 = ins->child;
  i = 0;
  iVar10 = *(int *)(iVar6 + 0x18);
  if (0 < iVar10) {
    local_b4 = 0;
    do {
      iVar8 = *(int *)(iVar6 + 0x10);
      if (*(int *)(iVar8 + 0x40 + local_b4) != 0) {
        paMVar7 = p_Var11->mTM;
        if (paMVar7 == (MeReal (*) [4])0x0) {
          paMVar7 = elementTM;
          p_Var11->mTM = paMVar7;
          iVar8 = *(int *)(iVar6 + 0x10);
        }
                    /* Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
        paMVar5 = ins->mTM;
        pfVar9 = (float *)(iVar8 + local_b4);
        iVar10 = 3;
        do {
          (*paMVar7)[0] =
               paMVar5[3][0] * pfVar9[3] +
               paMVar5[2][0] * pfVar9[2] + paMVar5[1][0] * pfVar9[1] + (*paMVar5)[0] * *pfVar9;
          (*paMVar7)[1] =
               paMVar5[3][1] * pfVar9[3] +
               paMVar5[2][1] * pfVar9[2] + paMVar5[1][1] * pfVar9[1] + (*paMVar5)[1] * *pfVar9;
          (*paMVar7)[2] =
               paMVar5[3][2] * pfVar9[3] +
               paMVar5[2][2] * pfVar9[2] + paMVar5[1][2] * pfVar9[1] + (*paMVar5)[2] * *pfVar9;
          fVar4 = *pfVar9;
          pfVar1 = pfVar9 + 1;
          pfVar2 = pfVar9 + 2;
          pfVar3 = pfVar9 + 3;
          pfVar9 = pfVar9 + 4;
          (*paMVar7)[3] =
               paMVar5[3][3] * *pfVar3 +
               paMVar5[2][3] * *pfVar2 + paMVar5[1][3] * *pfVar1 + (*paMVar5)[3] * fVar4;
          paMVar7 = paMVar7 + 1;
          iVar10 = iVar10 + -1;
        } while (-1 < iVar10);
        if (finalTM != (MeMatrix4Ptr)0x0) {
                    /* Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int j@[???] */
          elementFinalTMPtr = elementFinalTM;
          iVar10 = 3;
          pfVar9 = (float *)(local_b4 + *(int *)(iVar6 + 0x10));
          paMVar7 = elementFinalTMPtr;
          do {
            (*paMVar7)[0] =
                 finalTM[3][0] * pfVar9[3] +
                 finalTM[2][0] * pfVar9[2] + finalTM[1][0] * pfVar9[1] + (*finalTM)[0] * *pfVar9;
            (*paMVar7)[1] =
                 finalTM[3][1] * pfVar9[3] +
                 finalTM[2][1] * pfVar9[2] + finalTM[1][1] * pfVar9[1] + (*finalTM)[1] * *pfVar9;
            (*paMVar7)[2] =
                 finalTM[3][2] * pfVar9[3] +
                 finalTM[2][2] * pfVar9[2] + finalTM[1][2] * pfVar9[1] + (*finalTM)[2] * *pfVar9;
            fVar4 = *pfVar9;
            pfVar1 = pfVar9 + 1;
            pfVar2 = pfVar9 + 2;
            pfVar3 = pfVar9 + 3;
            pfVar9 = pfVar9 + 4;
            (*paMVar7)[3] =
                 finalTM[3][3] * *pfVar3 +
                 finalTM[2][3] * *pfVar2 + finalTM[1][3] * *pfVar1 + (*finalTM)[3] * fVar4;
            paMVar7 = paMVar7 + 1;
            iVar10 = iVar10 + -1;
          } while (-1 < iVar10);
        }
        McdGeometryInstanceUpdateAABB(p_Var11,elementFinalTMPtr,tight,paMVar7);
        fVar4 = p_Var11->min[0];
        if (ins->min[0] < fVar4) {
          fVar4 = ins->min[0];
        }
        ins->min[0] = fVar4;
        fVar4 = p_Var11->min[1];
        if (ins->min[1] < fVar4) {
          fVar4 = ins->min[1];
        }
        ins->min[1] = fVar4;
        fVar4 = p_Var11->min[2];
        if (ins->min[2] < fVar4) {
          fVar4 = ins->min[2];
        }
        ins->min[2] = fVar4;
        fVar4 = p_Var11->max[0];
        if (fVar4 < ins->max[0]) {
          fVar4 = ins->max[0];
        }
        ins->max[0] = fVar4;
        fVar4 = p_Var11->max[1];
        if (fVar4 < ins->max[1]) {
          fVar4 = ins->max[1];
        }
        ins->max[1] = fVar4;
        fVar4 = p_Var11->max[2];
        if (fVar4 < ins->max[2]) {
          fVar4 = ins->max[2];
        }
        ins->max[2] = fVar4;
        if (p_Var11->mTM == elementTM) {
          p_Var11->mTM = (MeMatrix4Ptr)0x0;
          iVar10 = *(int *)(iVar6 + 0x18);
        }
        else {
          iVar10 = *(int *)(iVar6 + 0x18);
        }
      }
      i = i + 1;
      p_Var11 = p_Var11->next;
      local_b4 = local_b4 + 0x44;
    } while (i < iVar10);
  }
  return;
}


/* ==== McdAggregateGetMassProperties ==== */

MeI16 McdAggregateGetMassProperties(McdGeometry *g,MeVector4 *relTM,MeVector3 *m,MeReal *volume)

{
  undefined4 *puVar1;
  float fVar2;
  MeReal MVar3;
  MeReal MVar4;
  MeReal *pMVar5;
  MeReal *pMVar6;
  MeReal *pMVar7;
  MeReal *pMVar8;
  McdGeometryID pMVar9;
  MeReal (*paMVar10) [4];
  float *pfVar11;
  MeReal (*paMVar12) [4];
  int iVar13;
  int local_164;
  MeReal density;
  int i;
  MeReal subVol_1;
  MeReal subVol;
  MeReal massSoFar;
  MeReal newTM [4] [4];
  MeReal totalSubTM [4] [4];
  MeReal subTM_1 [4] [4];
  MeReal subTM [4] [4];
  MeReal subI [3] [3];
  
                    /* Unresolved local var: McdAggregate * a@[???] */
  massSoFar = 0.0;
  *volume = 0.0;
  (*relTM)[0] = 1.0;
  (*relTM)[1] = 0.0;
  (*relTM)[2] = 0.0;
  (*relTM)[3] = 0.0;
  relTM[1][0] = 0.0;
  relTM[1][1] = 1.0;
  relTM[1][2] = 0.0;
  relTM[1][3] = 0.0;
  relTM[2][0] = 0.0;
  relTM[2][1] = 0.0;
  relTM[2][2] = 1.0;
  relTM[2][3] = 0.0;
  relTM[3][0] = 0.0;
  relTM[3][1] = 0.0;
  relTM[3][2] = 0.0;
  relTM[3][3] = 1.0;
  (*m)[1] = 0.0;
  (*m)[2] = 0.0;
  m[1][0] = 0.0;
  m[1][2] = 0.0;
  m[2][0] = 0.0;
  m[2][1] = 0.0;
  (*m)[0] = 0.0;
  m[1][1] = 0.0;
  m[2][2] = 0.0;
  i = 0;
  if ((int)g[1].prev < 1) {
    pMVar9 = g[1].prev;
  }
  else {
    iVar13 = 0;
    do {
                    /* Unresolved local var: McdGeometryID subgeom@[???] */
      puVar1 = (undefined4 *)(g[1].mRefCtAndID + 0x40 + iVar13);
      iVar13 = iVar13 + 0x44;
      McdGeometryGetMassProperties(*puVar1,subTM,subI,&subVol);
      *volume = subVol + *volume;
      i = i + 1;
      pMVar9 = g[1].prev;
    } while (i < (int)pMVar9);
  }
  fVar2 = *volume;
  i = 0;
  massSoFar = 0.0;
  if (0 < (int)pMVar9) {
    local_164 = 0;
    do {
                    /* Unresolved local var: McdGeometryID subgeom@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[???] */
      subTM_1[0][0] = 1.0;
      subTM_1[0][1] = 0.0;
      subTM_1[0][2] = 0.0;
      subTM_1[0][3] = 0.0;
      subTM_1[1][0] = 0.0;
      subTM_1[1][1] = 1.0;
      subTM_1[1][2] = 0.0;
      subTM_1[1][3] = 0.0;
      subTM_1[2][0] = 0.0;
      subTM_1[2][1] = 0.0;
      subTM_1[2][2] = 1.0;
      subTM_1[2][3] = 0.0;
      subTM_1[3][0] = 0.0;
      subTM_1[3][1] = 0.0;
      subTM_1[3][2] = 0.0;
      subTM_1[3][3] = 1.0;
      McdGeometryGetMassProperties
                (*(undefined4 *)(g[1].mRefCtAndID + 0x40 + local_164),subTM_1,subTM,&subVol_1);
      paMVar10 = totalSubTM;
      pfVar11 = (float *)(local_164 + g[1].mRefCtAndID);
      paMVar12 = subTM_1;
      iVar13 = 3;
      do {
        (*paMVar10)[0] =
             pfVar11[0xc] * (*paMVar12)[3] +
             pfVar11[8] * (*paMVar12)[2] + pfVar11[4] * (*paMVar12)[1] + *pfVar11 * (*paMVar12)[0];
        (*paMVar10)[1] =
             pfVar11[0xd] * (*paMVar12)[3] +
             pfVar11[9] * (*paMVar12)[2] + pfVar11[5] * (*paMVar12)[1] + pfVar11[1] * (*paMVar12)[0]
        ;
        (*paMVar10)[2] =
             pfVar11[0xe] * (*paMVar12)[3] +
             pfVar11[10] * (*paMVar12)[2] +
             pfVar11[6] * (*paMVar12)[1] + pfVar11[2] * (*paMVar12)[0];
        pMVar5 = *paMVar12;
        pMVar6 = *paMVar12;
        pMVar7 = *paMVar12;
        pMVar8 = *paMVar12;
        paMVar12 = paMVar12 + 1;
        (*paMVar10)[3] =
             pfVar11[0xf] * pMVar8[3] +
             pfVar11[0xb] * pMVar7[2] + pfVar11[7] * pMVar6[1] + pfVar11[3] * *pMVar5;
        paMVar10 = paMVar10 + 1;
        iVar13 = iVar13 + -1;
      } while (-1 < iVar13);
      CombineInertiaTensors
                (relTM,m,massSoFar,totalSubTM,(MeVector3 *)subTM,(1.0 / fVar2) * subVol_1,newTM,
                 (MeVector3 *)(subTM + 3),&massSoFar);
      (*relTM)[0] = newTM[0][0];
      (*relTM)[1] = newTM[0][1];
      (*relTM)[2] = newTM[0][2];
      (*relTM)[3] = newTM[0][3];
      relTM[1][0] = newTM[1][0];
      relTM[1][1] = newTM[1][1];
      relTM[1][2] = newTM[1][2];
      relTM[1][3] = newTM[1][3];
      relTM[2][0] = newTM[2][0];
      relTM[2][1] = newTM[2][1];
      relTM[2][2] = newTM[2][2];
      relTM[2][3] = newTM[2][3];
      relTM[3][0] = newTM[3][0];
      relTM[3][1] = newTM[3][1];
      relTM[3][2] = newTM[3][2];
      relTM[3][3] = newTM[3][3];
      (*m)[0] = subTM[3][0];
      (*m)[1] = subTM[3][1];
      (*m)[2] = subTM[3][2];
      m[1][0] = subTM[3][3];
      m[1][1] = subI[0][0];
      m[1][2] = subI[0][1];
      m[2][0] = subI[0][2];
      m[2][1] = subI[1][0];
      m[2][2] = subI[1][1];
      i = i + 1;
      local_164 = local_164 + 0x44;
    } while (i < (int)g[1].prev);
  }
  MVar3 = m[1][2];
  MVar4 = m[2][0];
  (*m)[1] = m[1][0];
  (*m)[2] = MVar4;
  m[2][1] = MVar3;
  return 0;
}


/* ==== McdAggregateGenericIntersect ==== */

/* WARNING: Unknown calling convention */

int McdAggregateGenericIntersect(McdModelPairID p,McdIntersectResult *result)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float fVar4;
  float *pfVar5;
  bool bVar6;
  short sVar7;
  int iVar8;
  int iVar9;
  undefined4 uVar10;
  MeReal (*paMVar11) [4];
  int iVar12;
  int iVar13;
  float *pfVar14;
  undefined4 *puVar15;
  McdModelID_conflict pMVar16;
  undefined4 *puVar17;
  McdModel_conflict *pMVar18;
  McdGeometryInstance *pMVar19;
  int local_3fd4;
  int contactCount;
  McdAggregate *g;
  McdGeometryInstanceID ins;
  McdGeometryInstanceID instance2;
  int i;
  McdContact contactBuffer [400];
  MeReal avgNormal [3];
  MeReal elementTM [4] [4];
  McdIntersectResult dummyResult;
  McdModel_conflict dummyModel;
  McdModelPair dummyPair;
  
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdGeometryInstanceID element@[DW_OP_reg3(EBX)] */
  iVar8 = McdModelGetGeometryInstance(p->model1);
  iVar9 = McdGeometryInstanceGetGeometry(iVar8);
  contactCount = 0;
  result->touch = 0;
  result->contactCount = 0;
  dummyPair.model1 = &dummyModel;
  dummyResult.pair = &dummyPair;
  pMVar16 = p->model1;
  pMVar18 = &dummyModel;
  for (iVar12 = 0x1e; iVar12 != 0; iVar12 = iVar12 + -1) {
    pMVar18->frame = pMVar16->frame;
    pMVar16 = (McdModelID_conflict)&pMVar16->prev;
    pMVar18 = (McdModel_conflict *)&pMVar18->prev;
  }
  dummyPair.model2 = p->model2;
  dummyPair.request = p->request;
  uVar10 = McdModelGetGeometryInstance(dummyPair.model2);
  i = 0;
  avgNormal[0] = 0.0;
  avgNormal[1] = 0.0;
  avgNormal[2] = 0.0;
  puVar15 = *(undefined4 **)(iVar8 + 0x30);
  iVar12 = *(int *)(iVar9 + 0x18);
  if (0 < iVar12) {
    local_3fd4 = 0;
    do {
      if (*(int *)(*(int *)(iVar9 + 0x10) + 0x40 + local_3fd4) != 0) {
        iVar12 = McdGeometryInstanceOverlap(uVar10,puVar15,local_3fd4,local_3fd4);
        if (iVar12 == 0) {
          iVar12 = *(int *)(iVar9 + 0x18);
        }
        else {
          puVar17 = puVar15;
          pMVar19 = &dummyModel.mInstance;
          for (iVar12 = 0xd; iVar12 != 0; iVar12 = iVar12 + -1) {
            pMVar19->mGeometry = (McdGeometry *)*puVar17;
            puVar17 = puVar17 + 1;
            pMVar19 = (McdGeometryInstance *)&pMVar19->mTM;
          }
          if (dummyModel.mInstance.mTM == (MeMatrix4Ptr)0x0) {
                    /* Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
            paMVar11 = elementTM;
            pfVar5 = *(float **)(iVar8 + 4);
            pfVar14 = (float *)(local_3fd4 + *(int *)(iVar9 + 0x10));
            iVar12 = 3;
            dummyModel.mInstance.mTM = paMVar11;
            do {
              (*paMVar11)[0] =
                   pfVar5[0xc] * pfVar14[3] +
                   pfVar5[8] * pfVar14[2] + pfVar5[4] * pfVar14[1] + *pfVar5 * *pfVar14;
              (*paMVar11)[1] =
                   pfVar5[0xd] * pfVar14[3] +
                   pfVar5[9] * pfVar14[2] + pfVar5[5] * pfVar14[1] + pfVar5[1] * *pfVar14;
              (*paMVar11)[2] =
                   pfVar5[0xe] * pfVar14[3] +
                   pfVar5[10] * pfVar14[2] + pfVar5[6] * pfVar14[1] + pfVar5[2] * *pfVar14;
              fVar4 = *pfVar14;
              pfVar1 = pfVar14 + 1;
              pfVar2 = pfVar14 + 2;
              pfVar3 = pfVar14 + 3;
              pfVar14 = pfVar14 + 4;
              (*paMVar11)[3] =
                   pfVar5[0xf] * *pfVar3 +
                   pfVar5[0xb] * *pfVar2 + pfVar5[7] * *pfVar1 + pfVar5[3] * fVar4;
              paMVar11 = paMVar11 + 1;
              iVar12 = iVar12 + -1;
            } while (-1 < iVar12);
          }
          dummyResult.contactMaxCount = 400 - contactCount;
          dummyResult.contacts = contactBuffer + contactCount;
          dummyResult.touch = 0;
          McdHello(&dummyPair);
          McdIntersect(&dummyPair,&dummyResult);
          McdGoodbye(&dummyPair);
          if (dummyResult.touch == 0) {
            iVar12 = *(int *)(iVar9 + 0x18);
          }
          else {
                    /* Unresolved local var: McdGeometryType gtype@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeBool fillin@[DW_OP_reg7(EDI)] */
            bVar6 = false;
            result->touch = 1;
            sVar7 = McdModelGetGeometryType(&dummyModel);
            if ((sVar7 != 6) && (sVar7 != 8)) {
              bVar6 = true;
            }
            if (dummyPair.model2 == p->model2) {
              if ((bVar6) && (iVar12 = 0, 0 < dummyResult.contactCount)) {
                iVar13 = 0;
                do {
                  iVar12 = iVar12 + 1;
                  *(undefined4 **)((int)(dummyResult.contacts)->normal + iVar13 + 0x14) = puVar15;
                  iVar13 = iVar13 + 0x28;
                } while (iVar12 < dummyResult.contactCount);
              }
            }
            else {
              iVar12 = 0;
              dummyResult.normal[0] = dummyResult.normal[0] * -1.0;
              dummyResult.normal[1] = dummyResult.normal[1] * -1.0;
              dummyResult.normal[2] = dummyResult.normal[2] * -1.0;
              if (0 < dummyResult.contactCount) {
                iVar13 = 0;
                do {
                  *(float *)((int)(dummyResult.contacts)->normal + iVar13) =
                       *(float *)((int)(dummyResult.contacts)->normal + iVar13) * -1.0;
                  *(float *)((int)(dummyResult.contacts)->normal + iVar13 + 4) =
                       *(float *)((int)(dummyResult.contacts)->normal + iVar13 + 4) * -1.0;
                  *(float *)((int)(dummyResult.contacts)->normal + iVar13 + 8) =
                       *(float *)((int)(dummyResult.contacts)->normal + iVar13 + 8) * -1.0;
                  *(undefined4 *)((int)(dummyResult.contacts)->normal + iVar13 + 0x18) =
                       *(undefined4 *)((int)(dummyResult.contacts)->normal + iVar13 + 0x14);
                  if (bVar6) {
                    *(undefined4 **)((int)(dummyResult.contacts)->normal + iVar13 + 0x14) = puVar15;
                  }
                  iVar12 = iVar12 + 1;
                  iVar13 = iVar13 + 0x28;
                } while (iVar12 < dummyResult.contactCount);
              }
            }
            fVar4 = (float)dummyResult.contactCount;
            contactCount = contactCount + dummyResult.contactCount;
            avgNormal[0] = fVar4 * dummyResult.normal[0] + avgNormal[0];
            avgNormal[2] = fVar4 * dummyResult.normal[2] + avgNormal[2];
            avgNormal[1] = fVar4 * dummyResult.normal[1] + avgNormal[1];
            iVar12 = *(int *)(iVar9 + 0x18);
          }
        }
      }
      local_3fd4 = local_3fd4 + 0x44;
      i = i + 1;
      puVar15 = (undefined4 *)puVar15[10];
    } while (i < iVar12);
  }
  MeVector3Normalize(avgNormal);
  return result->touch;
}


/* ==== IxAggregateLineSegment ==== */

/* WARNING: Unknown calling convention */

int IxAggregateLineSegment
              (McdModelID_conflict model,MeReal *inOrig,MeReal *inDest,
              McdLineSegIntersectResult *overlap)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float fVar4;
  float *pfVar5;
  int iVar6;
  int iVar7;
  MeReal (*paMVar8) [4];
  int iVar9;
  float *pfVar10;
  undefined4 *puVar11;
  McdModelID_conflict pMVar12;
  undefined4 *puVar13;
  McdModel_conflict *pMVar14;
  McdGeometryInstance *pMVar15;
  int local_10c;
  int i;
  McdGeometryInstanceID ins;
  McdAggregate *a;
  McdModel_conflict dummyModel;
  McdLineSegIntersectResult dummyOverlap;
  MeReal elementTM [4] [4];
  
                    /* Unresolved local var: McdGeometryID g@[???]
                       Unresolved local var: McdGeometryInstanceID element@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeMatrix4Ptr tm@[???]
                       Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  iVar6 = McdModelGetGeometry(model);
  iVar7 = McdModelGetGeometryInstance(model);
  McdModelGetTransformPtr(model);
  overlap->distance = 3.4028235e+38;
  pMVar12 = model;
  pMVar14 = &dummyModel;
  for (iVar9 = 0x1e; iVar9 != 0; iVar9 = iVar9 + -1) {
    pMVar14->frame = pMVar12->frame;
    pMVar12 = (McdModelID_conflict)&pMVar12->prev;
    pMVar14 = (McdModel_conflict *)&pMVar14->prev;
  }
  i = 0;
  puVar11 = *(undefined4 **)(iVar7 + 0x30);
  iVar9 = *(int *)(iVar6 + 0x18);
  if (0 < iVar9) {
    local_10c = 0;
    do {
      if (*(int *)(*(int *)(iVar6 + 0x10) + 0x40 + local_10c) != 0) {
        puVar13 = puVar11;
        pMVar15 = &dummyModel.mInstance;
        for (iVar9 = 0xd; iVar9 != 0; iVar9 = iVar9 + -1) {
          pMVar15->mGeometry = (McdGeometry *)*puVar13;
          puVar13 = puVar13 + 1;
          pMVar15 = (McdGeometryInstance *)&pMVar15->mTM;
        }
        if (dummyModel.mInstance.mTM == (MeMatrix4Ptr)0x0) {
          dummyModel.mInstance.mTM = elementTM;
          pfVar5 = *(float **)(iVar7 + 4);
          iVar9 = 3;
          pfVar10 = (float *)(local_10c + *(int *)(iVar6 + 0x10));
          paMVar8 = dummyModel.mInstance.mTM;
          do {
            (*paMVar8)[0] =
                 pfVar5[0xc] * pfVar10[3] +
                 pfVar5[8] * pfVar10[2] + pfVar5[4] * pfVar10[1] + *pfVar5 * *pfVar10;
            (*paMVar8)[1] =
                 pfVar5[0xd] * pfVar10[3] +
                 pfVar5[9] * pfVar10[2] + pfVar5[5] * pfVar10[1] + pfVar5[1] * *pfVar10;
            (*paMVar8)[2] =
                 pfVar5[0xe] * pfVar10[3] +
                 pfVar5[10] * pfVar10[2] + pfVar5[6] * pfVar10[1] + pfVar5[2] * *pfVar10;
            fVar4 = *pfVar10;
            pfVar1 = pfVar10 + 1;
            pfVar2 = pfVar10 + 2;
            pfVar3 = pfVar10 + 3;
            pfVar10 = pfVar10 + 4;
            (*paMVar8)[3] =
                 pfVar5[0xf] * *pfVar3 +
                 pfVar5[0xb] * *pfVar2 + pfVar5[7] * *pfVar1 + pfVar5[3] * fVar4;
            paMVar8 = paMVar8 + 1;
            iVar9 = iVar9 + -1;
          } while (-1 < iVar9);
        }
        dummyOverlap.model = (McdModelID_conflict)0x0;
        dummyOverlap.position[0] = 0.0;
        dummyOverlap.position[1] = 0.0;
        dummyOverlap.position[2] = 0.0;
        dummyOverlap.normal[0] = 0.0;
        dummyOverlap.normal[1] = 0.0;
        dummyOverlap.normal[2] = 0.0;
        dummyOverlap.distance = 0.0;
        iVar9 = McdLineSegIntersect(&dummyModel,inOrig,inDest,&dummyOverlap);
        if (iVar9 == 0) {
          iVar9 = *(int *)(iVar6 + 0x18);
        }
        else if (overlap->distance <= dummyOverlap.distance) {
          iVar9 = *(int *)(iVar6 + 0x18);
        }
        else {
          overlap->normal[0] = dummyOverlap.normal[0];
          overlap->normal[1] = dummyOverlap.normal[1];
          overlap->normal[2] = dummyOverlap.normal[2];
          overlap->position[0] = dummyOverlap.position[0];
          overlap->position[1] = dummyOverlap.position[1];
          overlap->position[2] = dummyOverlap.position[2];
          overlap->model = model;
          overlap->distance = dummyOverlap.distance;
          iVar9 = *(int *)(iVar6 + 0x18);
        }
      }
      local_10c = local_10c + 0x44;
      i = i + 1;
      puVar11 = (undefined4 *)puVar11[10];
    } while (i < iVar9);
  }
  return (uint)(overlap->distance != 3.4028235e+38);
}


/* ==== McdAggregateGenericRegisterInteractions ==== */

MeBool McdAggregateGenericRegisterInteractions(McdFrameworkID frame)

{
  int iVar1;
  int iVar2;
  McdInteractions interactions;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  interactions.helloFn = (McdHelloFn)0x0;
  interactions.goodbyeFn = (McdGoodbyeFn)0x0;
  interactions.intersectFn = McdAggregateGenericIntersect;
  interactions.safetimeFn = (McdSafeTimeFn)0x0;
  interactions.cull = 1;
  if (0 < frame->geometryRegisteredCountMax) {
    iVar1 = 0;
    do {
      iVar2 = iVar1 + 1;
      McdFrameworkSetInteractions(frame,8,iVar1,&interactions);
      iVar1 = iVar2;
    } while (iVar2 < frame->geometryRegisteredCountMax);
  }
  return 1;
}


/* ==== McdAggregateDebugDraw ==== */

void McdAggregateDebugDraw(McdGeometryID geom,MeReal (*tm) [4],MeReal *colour)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float fVar4;
  MeReal (*paMVar5) [4];
  float *pfVar6;
  int iVar7;
  int iVar8;
  int local_68;
  int local_64;
  undefined4 local_60;
  MeReal totaltm [4] [4];
  
                    /* Unresolved local var: McdAggregateID agg@[???]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)] */
  if (((char)geom->mRefCtAndID == '\b') && (iVar8 = 0, 0 < (int)geom[1].prev)) {
    local_64 = 0;
    local_68 = 0;
    do {
                    /* Unresolved local var: McdGeometryID instgeom@[???]
                       Unresolved local var: MeMatrix4Ptr insttm@[???]
                       Unresolved local var: McdAggregate * a@[???] */
      local_60 = 0;
      if (iVar8 < (int)geom[1].next) {
        local_60 = *(undefined4 *)(geom[1].mRefCtAndID + 0x40 + local_68);
      }
                    /* Unresolved local var: McdAggregate * a@[???] */
      pfVar6 = (float *)0x0;
      if (iVar8 < (int)geom[1].next) {
        pfVar6 = (float *)(local_64 + geom[1].mRefCtAndID);
      }
                    /* Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int j@[???] */
      paMVar5 = totaltm;
      iVar7 = 3;
      do {
        (*paMVar5)[0] =
             tm[3][0] * pfVar6[3] + tm[2][0] * pfVar6[2] + tm[1][0] * pfVar6[1] + (*tm)[0] * *pfVar6
        ;
        (*paMVar5)[1] =
             tm[3][1] * pfVar6[3] + tm[2][1] * pfVar6[2] + tm[1][1] * pfVar6[1] + (*tm)[1] * *pfVar6
        ;
        (*paMVar5)[2] =
             tm[3][2] * pfVar6[3] + tm[2][2] * pfVar6[2] + tm[1][2] * pfVar6[1] + (*tm)[2] * *pfVar6
        ;
        fVar4 = *pfVar6;
        pfVar1 = pfVar6 + 1;
        pfVar2 = pfVar6 + 2;
        pfVar3 = pfVar6 + 3;
        pfVar6 = pfVar6 + 4;
        (*paMVar5)[3] =
             tm[3][3] * *pfVar3 + tm[2][3] * *pfVar2 + tm[1][3] * *pfVar1 + (*tm)[3] * fVar4;
        paMVar5 = paMVar5 + 1;
        iVar7 = iVar7 + -1;
      } while (-1 < iVar7);
      iVar8 = iVar8 + 1;
      McdGeometryDebugDraw(local_60,totaltm,colour,paMVar5);
      local_64 = local_64 + 0x44;
      local_68 = local_68 + 0x44;
    } while (iVar8 < (int)geom[1].prev);
  }
  return;
}


/* ==== McdAggregateLineSegmentRegisterInteraction ==== */

/* WARNING: Unknown calling convention */

void McdAggregateLineSegmentRegisterInteraction(McdFramework *frame)

{
  McdFrameworkSetLineSegInteraction(frame,8,IxAggregateLineSegment);
  return;
}


/* ==== McdAggregateRegisterInteractions ==== */

void __regparm2 McdAggregateRegisterInteractions(McdFrameworkID frame)

{
  int iVar1;
  int *in_stack_00000004;
  McdInteractions interactions;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar1 = 0;
  if (0 < *in_stack_00000004) {
    do {
      iVar1 = iVar1 + 1;
      McdFrameworkSetInteractions();
    } while (iVar1 < *in_stack_00000004);
  }
  McdFrameworkSetLineSegInteraction();
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


/* ==== CombineInertiaTensors ==== */

void CombineInertiaTensors
               (MeVector4 *TM1,MeVector3 *I1,MeReal mass1,MeVector4 *TM2,MeVector3 *I2,MeReal mass2,
               MeVector4 *outTM,MeVector3 *outI,MeReal *outMass)

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
  float fVar38;
  float fVar39;
  float fVar40;
  float fVar41;
  float fVar42;
  float fVar43;
  float fVar44;
  float fVar45;
  float fVar46;
  float fVar47;
  float fVar48;
  MeReal tmp_1 [3] [3];
  MeReal tmpI [3] [3];
  MeReal tmp [3] [3];
  MeReal comRel2 [3];
  MeReal comRel1 [3];
  MeReal massSkew2 [3] [3];
  MeReal massSkew1 [3] [3];
  MeReal rotI2 [3] [3];
  MeReal rotI1 [3] [3];
  MeReal R2 [3] [3];
  MeReal R1 [3] [3];
  MeReal skew2 [3] [3];
  MeReal skew1 [3] [3];
  
  (*outTM)[0] = 1.0;
  (*outTM)[1] = 0.0;
  (*outTM)[2] = 0.0;
  (*outTM)[3] = 0.0;
  outTM[1][0] = 0.0;
  outTM[1][1] = 1.0;
  outTM[1][2] = 0.0;
  outTM[1][3] = 0.0;
  outTM[2][0] = 0.0;
  outTM[2][1] = 0.0;
  outTM[2][2] = 1.0;
  outTM[2][3] = 0.0;
  outTM[3][0] = 0.0;
  outTM[3][1] = 0.0;
  outTM[3][2] = 0.0;
  outTM[3][3] = 1.0;
  *outMass = mass1 + mass2;
  fVar22 = (mass2 * TM2[3][0] + mass1 * TM1[3][0]) / (mass1 + mass2);
  outTM[3][0] = fVar22;
  outTM[3][1] = (1.0 / *outMass) * (mass2 * TM2[3][1] + mass1 * TM1[3][1]);
  outTM[3][2] = (mass2 * TM2[3][2] + mass1 * TM1[3][2]) * (1.0 / *outMass);
  fVar1 = (*TM1)[0];
  fVar10 = (*TM1)[1];
  fVar11 = (*TM1)[2];
  fVar12 = TM1[1][0];
  fVar2 = TM1[1][1];
  fVar13 = TM1[1][2];
  fVar14 = TM1[2][0];
  fVar15 = TM1[2][1];
  fVar3 = TM1[2][2];
  fVar4 = (*I1)[0];
  fVar5 = (*I1)[1];
  fVar6 = (*I1)[2];
  fVar23 = fVar14 * fVar6 + fVar5 * fVar12 + fVar1 * fVar4;
  fVar31 = fVar15 * fVar6 + fVar2 * fVar5 + fVar10 * fVar4;
  fVar24 = fVar6 * fVar3 + fVar5 * fVar13 + fVar4 * fVar11;
  fVar4 = I1[1][1];
  fVar5 = I1[1][0];
  fVar6 = I1[1][2];
  fVar30 = fVar14 * fVar6 + fVar4 * fVar12 + fVar1 * fVar5;
  fVar29 = fVar15 * fVar6 + fVar2 * fVar4 + fVar10 * fVar5;
  fVar28 = fVar6 * fVar3 + fVar4 * fVar13 + fVar5 * fVar11;
  fVar4 = I1[2][0];
  fVar5 = I1[2][1];
  fVar6 = I1[2][2];
  fVar25 = fVar1 * fVar4 + fVar12 * fVar5 + fVar14 * fVar6;
  fVar27 = fVar10 * fVar4 + fVar2 * fVar5 + fVar15 * fVar6;
  fVar26 = fVar4 * fVar11 + fVar5 * fVar13 + fVar6 * fVar3;
  fVar4 = (*TM2)[0];
  fVar16 = (*TM2)[1];
  fVar17 = (*TM2)[2];
  fVar18 = TM2[1][0];
  fVar5 = TM2[1][1];
  fVar19 = TM2[1][2];
  fVar20 = TM2[2][0];
  fVar21 = TM2[2][1];
  fVar6 = TM2[2][2];
  fVar7 = (*I2)[0];
  fVar8 = (*I2)[1];
  fVar9 = (*I2)[2];
  fVar32 = fVar20 * fVar9 + fVar8 * fVar18 + fVar4 * fVar7;
  fVar39 = fVar21 * fVar9 + fVar5 * fVar8 + fVar16 * fVar7;
  fVar33 = fVar9 * fVar6 + fVar8 * fVar19 + fVar7 * fVar17;
  fVar7 = I2[1][1];
  fVar8 = I2[1][0];
  fVar9 = I2[1][2];
  fVar38 = fVar20 * fVar9 + fVar7 * fVar18 + fVar4 * fVar8;
  fVar37 = fVar21 * fVar9 + fVar5 * fVar7 + fVar16 * fVar8;
  fVar36 = fVar9 * fVar6 + fVar7 * fVar19 + fVar8 * fVar17;
  fVar7 = I2[2][0];
  fVar8 = I2[2][1];
  fVar9 = I2[2][2];
  fVar34 = fVar4 * fVar7 + fVar18 * fVar8 + fVar20 * fVar9;
  fVar35 = fVar16 * fVar7 + fVar5 * fVar8 + fVar21 * fVar9;
  fVar7 = fVar7 * fVar17 + fVar8 * fVar19 + fVar9 * fVar6;
  fVar9 = outTM[3][1] - TM1[3][1];
  fVar44 = outTM[3][2] - TM1[3][2];
  fVar8 = fVar22 - TM1[3][0];
  fVar48 = -fVar9;
  fVar46 = -fVar44;
  fVar47 = -fVar8;
  fVar43 = outTM[3][2] - TM2[3][2];
  fVar40 = outTM[3][1] - TM2[3][1];
  fVar22 = fVar22 - TM2[3][0];
  fVar45 = -fVar43;
  fVar42 = -fVar40;
  fVar41 = -fVar22;
  (*outI)[0] = ((fVar14 * fVar25 + fVar12 * fVar30 + fVar1 * fVar23 +
                fVar20 * fVar34 + fVar18 * fVar38 + fVar4 * fVar32) -
               (fVar44 * fVar46 + fVar48 * fVar9) * mass1) -
               (fVar43 * fVar45 + fVar42 * fVar40) * mass2;
  (*outI)[1] = ((fVar14 * fVar27 + fVar12 * fVar29 + fVar1 * fVar31 +
                fVar20 * fVar35 + fVar18 * fVar37 + fVar4 * fVar39) - fVar48 * fVar47 * mass1) -
               fVar42 * fVar41 * mass2;
  (*outI)[2] = ((fVar12 * fVar28 + fVar1 * fVar24 + fVar14 * fVar26 +
                fVar18 * fVar36 + fVar4 * fVar33 + fVar20 * fVar7) - fVar44 * fVar8 * mass1) -
               fVar43 * fVar22 * mass2;
  outI[1][0] = ((fVar15 * fVar25 + fVar10 * fVar23 + fVar2 * fVar30 +
                fVar21 * fVar34 + fVar16 * fVar32 + fVar5 * fVar38) - fVar8 * fVar9 * mass1) -
               fVar22 * fVar40 * mass2;
  outI[1][1] = ((fVar15 * fVar27 + fVar2 * fVar29 + fVar10 * fVar31 +
                fVar21 * fVar35 + fVar5 * fVar37 + fVar16 * fVar39) -
               (fVar44 * fVar46 + fVar8 * fVar47) * mass1) -
               (fVar43 * fVar45 + fVar22 * fVar41) * mass2;
  outI[1][2] = ((fVar10 * fVar24 + fVar2 * fVar28 + fVar15 * fVar26 +
                fVar16 * fVar33 + fVar5 * fVar36 + fVar21 * fVar7) - fVar48 * fVar46 * mass1) -
               fVar42 * fVar45 * mass2;
  outI[2][0] = ((fVar25 * fVar3 + fVar11 * fVar23 + fVar13 * fVar30 +
                fVar34 * fVar6 + fVar17 * fVar32 + fVar19 * fVar38) - fVar46 * fVar47 * mass1) -
               fVar41 * fVar45 * mass2;
  outI[2][1] = ((fVar27 * fVar3 + fVar13 * fVar29 + fVar11 * fVar31 +
                fVar35 * fVar6 + fVar19 * fVar37 + fVar17 * fVar39) - fVar9 * fVar44 * mass1) -
               fVar40 * fVar43 * mass2;
  outI[2][2] = ((fVar11 * fVar24 + fVar13 * fVar28 + fVar26 * fVar3 +
                fVar17 * fVar33 + fVar19 * fVar36 + fVar7 * fVar6) -
               (fVar8 * fVar47 + fVar48 * fVar9) * mass1) -
               (fVar22 * fVar41 + fVar42 * fVar40) * mass2;
  return;
}


