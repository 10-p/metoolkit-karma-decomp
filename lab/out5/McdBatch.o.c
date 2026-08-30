/* ==== McdBatchFlattenAggregate ==== */

/* WARNING: Unknown calling convention */

MeBool McdBatchFlattenAggregate
                 (McdBatchContext *context,int flags,McdGeometryInstanceID ins1,
                 McdGeometryInstanceID ins2,MeReal eps1,MeReal eps2)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float fVar4;
  byte bVar5;
  MeMatrix4Ptr paMVar6;
  MeU16 MVar7;
  void *pvVar8;
  MeMatrix4Ptr paMVar9;
  void *pvVar10;
  MeBool MVar11;
  float *pfVar12;
  McdBatchEntry *pMVar13;
  int iVar14;
  McdGeometryInstance *pMVar15;
  int iVar16;
  uint flags_00;
  McdGeometryInstanceID ins1_00;
  McdGeometryInstance *ins2_00;
  MeReal eps1_00;
  MeReal eps2_00;
  int local_70;
  McdBatchEntry *entry;
  int i;
  int type2;
  int type1;
  MeReal max2 [3];
  MeReal min2 [3];
  MeReal max1 [3];
  MeReal min1 [3];
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdAggregate * a@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdGeometryInstanceID elementIns@[DW_OP_reg3(EBX)] */
  pvVar8 = McdGeometryInstanceGetGeometry(ins1);
  pMVar15 = ins1->child;
  iVar16 = 0;
  McdGeometryInstanceGetAABB(ins2,min2,max2);
  MVar7 = McdGeometryInstanceGetGeometryType(ins2);
  iVar14 = *(int *)((int)pvVar8 + 0x18);
  if (0 < iVar14) {
    local_70 = 0;
    do {
      if (*(int *)(*(int *)((int)pvVar8 + 0x10) + 0x40 + local_70) != 0) {
        McdGeometryInstanceGetAABB(pMVar15,min1,max1);
        if ((((min1[0] <= max2[0]) && (min1[1] <= max2[1])) && (min1[2] <= max2[2])) &&
           (((min2[0] <= max1[0] && (min2[1] <= max1[1])) && (min2[2] <= max1[2])))) {
          if (pMVar15->mTM == (MeMatrix4Ptr)0x0) {
            pMVar15->mTM = context->tmArray[context->nextTM];
            context->tmTrack[context->nextTM] = &pMVar15->mTM;
            context->nextTM = context->nextTM + 1;
                    /* Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int j@[???] */
            paMVar9 = pMVar15->mTM;
            paMVar6 = ins1->mTM;
            pfVar12 = (float *)(local_70 + *(int *)((int)pvVar8 + 0x10));
            i = 3;
            do {
              (*paMVar9)[0] =
                   paMVar6[3][0] * pfVar12[3] +
                   paMVar6[2][0] * pfVar12[2] +
                   paMVar6[1][0] * pfVar12[1] + (*paMVar6)[0] * *pfVar12;
              (*paMVar9)[1] =
                   paMVar6[3][1] * pfVar12[3] +
                   paMVar6[2][1] * pfVar12[2] +
                   paMVar6[1][1] * pfVar12[1] + (*paMVar6)[1] * *pfVar12;
              (*paMVar9)[2] =
                   paMVar6[3][2] * pfVar12[3] +
                   paMVar6[2][2] * pfVar12[2] +
                   paMVar6[1][2] * pfVar12[1] + (*paMVar6)[2] * *pfVar12;
              fVar4 = *pfVar12;
              pfVar1 = pfVar12 + 1;
              pfVar2 = pfVar12 + 2;
              pfVar3 = pfVar12 + 3;
              pfVar12 = pfVar12 + 4;
              (*paMVar9)[3] =
                   paMVar6[3][3] * *pfVar3 +
                   paMVar6[2][3] * *pfVar2 + paMVar6[1][3] * *pfVar1 + (*paMVar6)[3] * fVar4;
              paMVar9 = paMVar9 + 1;
              i = i + -1;
            } while (-1 < i);
          }
          bVar5 = **(byte **)(*(int *)((int)pvVar8 + 0x10) + 0x40 + local_70);
          flags_00 = flags;
          ins1_00 = pMVar15;
          ins2_00 = ins2;
          eps1_00 = eps1;
          eps2_00 = eps2;
          if (bVar5 != 8) {
            if ((short)MVar7 != 8) {
                    /* Unresolved local var: int index@[DW_OP_reg2(EDX)] */
              if (context->maxEntryCount <= context->nextEntry) {
                return 0;
              }
              pMVar13 = context->entryArray + context->nextEntry;
              pvVar10 = McdFrameworkGetInteractions(context->frame,(uint)bVar5,(int)(short)MVar7);
              if (*(int *)((int)pvVar10 + 0x10) == 0) {
                (pMVar13->geometryData1).type = (ushort)bVar5;
                (pMVar13->geometryData1).min = pMVar15->min;
                (pMVar13->geometryData1).max = pMVar15->max;
                (pMVar13->geometryData1).eps = eps1;
                (pMVar13->geometryData1).geometry = pMVar15->mGeometry;
                (pMVar13->geometryData1).tm = pMVar15->mTM;
                (pMVar13->geometryData2).type = MVar7;
                (pMVar13->geometryData2).eps = eps2;
                (pMVar13->geometryData2).min = ins2->min;
                (pMVar13->geometryData2).max = ins2->max;
                (pMVar13->geometryData2).geometry = ins2->mGeometry;
                (pMVar13->geometryData2).tm = ins2->mTM;
                pMVar13->ins1 = pMVar15;
                pMVar13->ins2 = ins2;
                pMVar13->flags = flags | 2;
              }
              else {
                (pMVar13->geometryData1).type = MVar7;
                (pMVar13->geometryData1).eps = eps2;
                (pMVar13->geometryData1).min = ins2->min;
                (pMVar13->geometryData1).max = ins2->max;
                (pMVar13->geometryData1).geometry = ins2->mGeometry;
                (pMVar13->geometryData1).tm = ins2->mTM;
                (pMVar13->geometryData2).type = (ushort)bVar5;
                (pMVar13->geometryData2).eps = eps1;
                (pMVar13->geometryData2).min = pMVar15->min;
                (pMVar13->geometryData2).max = pMVar15->max;
                (pMVar13->geometryData2).geometry = pMVar15->mGeometry;
                (pMVar13->geometryData2).tm = pMVar15->mTM;
                pMVar13->ins2 = pMVar15;
                pMVar13->ins1 = ins2;
                pMVar13->flags = (flags >> 1 & 2U | flags | 4) ^ 1;
              }
              iVar14 = (uint)(pMVar13->geometryData1).type * context->typeCount +
                       (uint)(pMVar13->geometryData2).type;
              pMVar13->next = context->table[iVar14];
              context->table[iVar14] = pMVar13;
              context->nextEntry = context->nextEntry + 1;
              goto LAB_00010300;
            }
            flags_00 = flags | 5;
            ins1_00 = ins2;
            ins2_00 = pMVar15;
            eps1_00 = eps2;
            eps2_00 = eps1;
          }
          MVar11 = McdBatchFlattenAggregate(context,flags_00,ins1_00,ins2_00,eps1_00,eps2_00);
          if (MVar11 == 0) {
            return 0;
          }
        }
LAB_00010300:
        iVar14 = *(int *)((int)pvVar8 + 0x18);
      }
      iVar16 = iVar16 + 1;
      local_70 = local_70 + 0x44;
      pMVar15 = pMVar15->next;
    } while (iVar16 < iVar14);
  }
  return 1;
}


/* ==== McdBatchFlatten ==== */

MeBool McdBatchFlatten(McdBatchContext *context,McdModelPairContainer *pairs)

{
  McdBatchPairData *pMVar1;
  MeMatrix4Ptr **pppaMVar2;
  byte bVar3;
  byte bVar4;
  McdModelPairID pMVar5;
  McdModelID_conflict pMVar6;
  McdModelID_conflict pMVar7;
  McdGeometryInstanceID ins1_00;
  McdGeometryInstanceID ins2_00;
  MeBool MVar8;
  uint uVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  McdBatchEntry *pMVar13;
  int iVar14;
  float fVar15;
  float eps2;
  int local_40;
  int local_3c;
  int saveNextTM;
  int saveNextFlattenPair;
  int saveNextEntry;
  int index;
  McdGeometryInstanceID ins2;
  McdGeometryInstanceID ins1;
  int type2;
  McdModelID_conflict model2;
  McdModelID_conflict model1;
  McdBatchPairData *pd;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  iVar10 = 0;
  context->nextEntry = 0;
  context->nextTM = 0;
  context->nextPool = 1;
  context->nextPairData = 0;
  if (0 < context->typeCount * context->typeCount) {
    do {
      context->table[iVar10] = (McdBatchEntry *)0x0;
      iVar10 = iVar10 + 1;
    } while (iVar10 < context->typeCount * context->typeCount);
  }
  context->pools->contactCount = 0;
  iVar10 = 0xc;
  iVar11 = 0xf;
  do {
    *(undefined4 *)((int)&context->pools->contactCount + iVar10) = 0;
    *(undefined4 *)((int)&context->pools[1].contactCount + iVar10) = 0;
    *(undefined4 *)((int)&context->pools[2].contactCount + iVar10) = 0;
    *(undefined4 *)((int)&context->pools[3].contactCount + iVar10) = 0;
    iVar10 = iVar10 + 0x30;
    iVar11 = iVar11 + -4;
  } while (-1 < iVar11);
  if (context->nextFlattenPair < pairs->stayingEnd) {
    do {
                    /* Unresolved local var: int type1@[DW_OP_reg6(ESI)] */
      if (context->pairDataMaxCount <= context->nextPairData) {
        return 0;
      }
      pMVar1 = context->pairData + context->nextPairData;
      pMVar5 = pairs->array[context->nextFlattenPair];
      pMVar1->pair = pMVar5;
      pMVar6 = pMVar5->model1;
      pMVar7 = pMVar5->model2;
      iVar10 = McdSpaceModelIsFrozen(pMVar6);
      if ((iVar10 == 0) || (iVar10 = McdSpaceModelIsFrozen(pMVar7), iVar10 == 0)) {
        ins1_00 = McdModelGetGeometryInstance(pMVar6);
        ins2_00 = McdModelGetGeometryInstance(pMVar7);
        bVar3 = (byte)ins1_00->mGeometry->mRefCtAndID;
        bVar4 = (byte)ins2_00->mGeometry->mRefCtAndID;
        if (bVar3 == 8) {
          fVar15 = McdModelGetContactTolerance(pMVar6);
          eps2 = McdModelGetContactTolerance(pMVar7);
          iVar10 = context->nextEntry;
          iVar11 = context->nextFlattenPair;
          iVar12 = context->nextTM;
          pMVar1->start = context->entryArray + iVar10;
          MVar8 = McdBatchFlattenAggregate(context,0,ins1_00,ins2_00,fVar15,eps2);
          if (MVar8 == 0) {
            local_3c = context->nextEntry;
LAB_00010703:
            iVar14 = local_3c + -1;
            if (iVar10 <= iVar14) {
              local_40 = iVar14 * 0x60;
              uVar9 = (iVar10 + -1) - iVar14 & 3;
              if (iVar10 + -1 < local_3c + -2) {
                if (uVar9 == 0) goto LAB_00010772;
                if (uVar9 < 3) {
                  if (uVar9 < 2) {
                    /* Unresolved local var: McdBatchEntry * e@[???] */
                    pMVar13 = context->entryArray;
                    context->table
                    [(uint)pMVar13[iVar14].geometryData1.type * context->typeCount +
                     (uint)pMVar13[iVar14].geometryData2.type] = pMVar13[iVar14].next;
                    local_40 = local_40 + -0x60;
                    iVar14 = local_3c + -2;
                  }
                  pMVar13 = context->entryArray;
                  iVar14 = iVar14 + -1;
                  context->table
                  [(uint)*(ushort *)((int)pMVar13->normal + local_40 + -0x1c) * context->typeCount +
                   (uint)*(ushort *)((int)pMVar13->normal + local_40 + -4)] =
                       *(McdBatchEntry **)((int)pMVar13->normal + local_40 + 0x28);
                  local_40 = local_40 + -0x60;
                }
              }
              pMVar13 = context->entryArray;
              context->table
              [(uint)*(ushort *)((int)pMVar13->normal + local_40 + -0x1c) * context->typeCount +
               (uint)*(ushort *)((int)pMVar13->normal + local_40 + -4)] =
                   *(McdBatchEntry **)((int)pMVar13->normal + local_40 + 0x28);
              local_40 = local_40 + -0x60;
              for (iVar14 = iVar14 + -1; iVar10 <= iVar14; iVar14 = iVar14 + -4) {
LAB_00010772:
                pMVar13 = context->entryArray;
                context->table
                [(uint)*(ushort *)((int)pMVar13->normal + local_40 + -0x1c) * context->typeCount +
                 (uint)*(ushort *)((int)pMVar13->normal + local_40 + -4)] =
                     *(McdBatchEntry **)((int)pMVar13->normal + local_40 + 0x28);
                pMVar13 = context->entryArray;
                context->table
                [(uint)*(ushort *)((int)pMVar13[-1].normal + local_40 + -0x1c) * context->typeCount
                 + (uint)*(ushort *)((int)pMVar13[-1].normal + local_40 + -4)] =
                     *(McdBatchEntry **)((int)pMVar13[-1].normal + local_40 + 0x28);
                pMVar13 = context->entryArray;
                context->table
                [(uint)*(ushort *)((int)pMVar13[-2].normal + local_40 + -0x1c) * context->typeCount
                 + (uint)*(ushort *)((int)pMVar13[-2].normal + local_40 + -4)] =
                     *(McdBatchEntry **)((int)pMVar13[-2].normal + local_40 + 0x28);
                pMVar13 = context->entryArray;
                context->table
                [(uint)*(ushort *)((int)pMVar13[-3].normal + local_40 + -0x1c) * context->typeCount
                 + (uint)*(ushort *)((int)pMVar13[-3].normal + local_40 + -4)] =
                     *(McdBatchEntry **)((int)pMVar13[-3].normal + local_40 + 0x28);
                local_40 = local_40 + -0x180;
              }
            }
            context->nextEntry = iVar10;
            iVar10 = iVar12;
            if (iVar12 < context->nextTM) {
              do {
                pppaMVar2 = context->tmTrack + iVar10;
                iVar10 = iVar10 + 1;
                **pppaMVar2 = (MeMatrix4Ptr)0x0;
              } while (iVar10 < context->nextTM);
            }
            context->nextTM = iVar12;
            context->nextFlattenPair = iVar11;
            return 0;
          }
          local_3c = context->nextEntry;
          if ((iVar10 + 1 < local_3c) && (context->poolMaxCount <= context->nextPool))
          goto LAB_00010703;
          local_3c = local_3c - iVar10;
          pMVar1->entries = local_3c;
          pMVar1->done = 0;
          pMVar1->status = (local_3c == 0) + 0x11;
          if (local_3c < 2) {
            iVar10 = 0;
          }
          else {
            iVar10 = context->nextPool;
            context->nextPool = iVar10 + 1;
          }
          iVar11 = 0;
          if (0 < pMVar1->entries) {
            iVar12 = 0;
            do {
              iVar11 = iVar11 + 1;
              *(McdBatchPairData **)((int)pMVar1->start->normal + iVar12 + 0x2c) = pMVar1;
              *(int *)((int)pMVar1->start->normal + iVar12 + 0x20) = iVar10;
              iVar12 = iVar12 + 0x60;
            } while (iVar11 < pMVar1->entries);
          }
        }
        else {
                    /* Unresolved local var: McdBatchEntry * entry@[DW_OP_reg3(EBX)] */
          iVar11 = (uint)bVar3 * context->typeCount + (uint)bVar4;
          iVar10 = context->nextEntry;
          if (context->maxEntryCount <= iVar10) {
            return 0;
          }
          context->nextEntry = iVar10 + 1;
          pMVar13 = context->entryArray + iVar10;
          fVar15 = McdModelGetContactTolerance(pMVar6);
          (pMVar13->geometryData1).type = (ushort)bVar3;
          (pMVar13->geometryData1).eps = fVar15;
          (pMVar13->geometryData1).min = ins1_00->min;
          (pMVar13->geometryData1).max = ins1_00->max;
          (pMVar13->geometryData1).geometry = ins1_00->mGeometry;
          (pMVar13->geometryData1).tm = ins1_00->mTM;
          fVar15 = McdModelGetContactTolerance(pMVar7);
          (pMVar13->geometryData2).type = (ushort)bVar4;
          (pMVar13->geometryData2).eps = fVar15;
          (pMVar13->geometryData2).min = ins2_00->min;
          (pMVar13->geometryData2).max = ins2_00->max;
          (pMVar13->geometryData2).geometry = ins2_00->mGeometry;
          (pMVar13->geometryData2).tm = ins2_00->mTM;
          pMVar13->ins2 = ins2_00;
          pMVar13->ins1 = ins1_00;
          pMVar13->pool = 0;
          pMVar13->pairData = pMVar1;
          pMVar13->flags = 0;
          pMVar13->next = context->table[iVar11];
          context->table[iVar11] = pMVar13;
          pMVar1->start = pMVar13;
          pMVar1->entries = 1;
          pMVar1->done = 0;
          pMVar1->status = 1;
        }
      }
      else {
        pMVar1->status = 8;
      }
      iVar10 = context->nextFlattenPair + 1;
      context->nextFlattenPair = iVar10;
      context->nextPairData = context->nextPairData + 1;
    } while (iVar10 < pairs->stayingEnd);
  }
  return 1;
}


/* ==== McdBatchIntersectBucket ==== */

MeBool McdBatchIntersectBucket(McdBatchContext *context,int type1,int type2)

{
  McdBatchContactPool *pMVar1;
  code *pcVar2;
  McdBatchEntry *pMVar3;
  McdBatchPairData *pMVar4;
  void *pvVar5;
  int iVar6;
  int iVar7;
  McdIntersectFn fn;
  int index;
  McdModel_conflict m2;
  McdModel_conflict m1;
  McdIntersectResult r;
  McdModelPair p;
  
                    /* Unresolved local var: McdBatchEntry * entry@[DW_OP_reg3(EBX)]
                       Unresolved local var: int ct@[???] */
  iVar6 = type1 * context->typeCount + type2;
  pvVar5 = McdFrameworkGetInteractions(context->frame,type1,type2);
  pcVar2 = *(code **)((int)pvVar5 + 8);
  if ((pcVar2 == (code *)0x0) &&
     (iVar7 = McdFrameworkGetInteractionsWarned(context->frame,type1,type2), iVar7 == 0)) {
    McdFrameworkGetTypeName(context->frame,type2);
    McdFrameworkGetTypeName(context->frame,type1);
    MeWarning(1,"No test registered between %s and %s");
    McdFrameworkSetInteractionsWarned(context->frame,type1,type2,1);
  }
  pMVar3 = context->table[iVar6];
  while( true ) {
    if (pMVar3 == (McdBatchEntry *)0x0) {
      return 1;
    }
                    /* Unresolved local var: McdBatchContactPool * pool@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdBatchPairData * pd@[DW_OP_reg6(ESI)] */
    pMVar4 = pMVar3->pairData;
    pMVar1 = context->pools + pMVar3->pool;
    if ((pMVar3->pool == 0) && (pMVar1->contactMaxCount < pMVar1->contactCount + 0x80)) break;
    m1.mInstance.mTM = (pMVar3->geometryData1).tm;
    m1.mInstance.mGeometry = (pMVar3->geometryData1).geometry;
    m1.mPadding = (pMVar3->geometryData1).eps;
    m1.mData = pMVar4->pair->model1->mData;
    m2.mInstance.mTM = (pMVar3->geometryData2).tm;
    m2.mInstance.mGeometry = (pMVar3->geometryData2).geometry;
    m2.mPadding = (pMVar3->geometryData2).eps;
    m2.mData = pMVar4->pair->model2->mData;
    p.model1 = &m1;
    m2.frame = context->frame;
    p.model2 = &m2;
    p.request = pMVar4->pair->request;
    p.userData = pMVar4->pair->userData;
    if ((pMVar4->status & 0x10) == 0) {
      p.m_cachedData = pMVar4->pair->m_cachedData;
    }
    else {
      p.m_cachedData = (void *)0x0;
    }
    r.contacts = pMVar1->contacts + pMVar1->contactCount;
    r.pair = &p;
    iVar7 = pMVar1->contactCount;
    r.contactMaxCount = pMVar1->contactMaxCount - iVar7;
    if (pcVar2 == (code *)0x0) {
      r.touch = 0;
      r.contactCount = 0;
    }
    else {
      m1.frame = m2.frame;
      (*pcVar2)(r.pair,&r,iVar7,iVar7);
    }
    pMVar4->pair->m_cachedData = p.m_cachedData;
    pMVar3->contacts = pMVar1->contacts + pMVar1->contactCount;
    pMVar3->touch = r.touch;
    pMVar3->contactCount = 0;
    iVar7 = pMVar4->done + 1;
    pMVar4->done = iVar7;
    if (iVar7 == pMVar4->entries) {
      pMVar4->status = pMVar4->status & 0x10U | 2;
    }
    if (pMVar3->touch != 0) {
      pMVar3->contactCount = r.contactCount;
      pMVar1->contactCount = pMVar1->contactCount + r.contactCount;
      pMVar3->normal[0] = r.normal[0];
      pMVar3->normal[1] = r.normal[1];
      pMVar3->normal[2] = r.normal[2];
    }
    context->table[iVar6] = pMVar3->next;
    pMVar3 = context->table[iVar6];
  }
  return 0;
}


/* ==== McdBatchUnflattenAggregate ==== */

/* WARNING: Unknown calling convention */

void McdBatchUnflattenAggregate
               (McdBatchContext *context,McdIntersectResult *ir,McdBatchPairData *pd)

{
  McdContact *pMVar1;
  int iVar2;
  McdBatchEntry *pMVar3;
  uint uVar4;
  int iVar5;
  McdBatchEntry *pMVar6;
  int iVar7;
  McdContact *contacts;
  int contactCount;
  
                    /* Unresolved local var: McdBatchEntry * entry@[DW_OP_reg1(ECX)]
                       Unresolved local var: int j@[DW_OP_reg6(ESI)] */
  ir->touch = 0;
  ir->contactCount = 0;
  ir->normal[0] = 0.0;
  ir->normal[1] = 0.0;
  ir->normal[2] = 0.0;
  iVar7 = pd->entries;
  if (iVar7 == 1) {
    pMVar3 = pd->start;
    contacts = pMVar3->contacts;
    iVar2 = pMVar3->contactCount;
  }
  else {
    pMVar3 = pd->start;
    contacts = context->pools[pMVar3->pool].contacts;
    iVar2 = context->pools[pMVar3->pool].contactCount;
  }
  pMVar6 = pMVar3;
  if (pMVar3 < pMVar3 + iVar7) {
    do {
      if (pMVar3->touch != 0) {
        iVar7 = 0;
        ir->touch = 1;
        if (0 < pMVar3->contactCount) {
          iVar5 = 0;
          do {
            uVar4 = pMVar3->flags;
            if ((uVar4 & 1) == 0) {
              if ((uVar4 & 2) != 0) {
                *(McdGeometryInstance **)((int)pMVar3->contacts->normal + iVar5 + 0x14) =
                     pMVar3->ins1;
                uVar4 = pMVar3->flags;
              }
              if ((uVar4 & 4) != 0) {
                *(McdGeometryInstance **)((int)pMVar3->contacts->normal + iVar5 + 0x18) =
                     pMVar3->ins2;
              }
            }
            else {
              pMVar1 = pMVar3->contacts;
              *(float *)((int)pMVar1->normal + iVar5) =
                   *(float *)((int)pMVar1->normal + iVar5) * -1.0;
              *(float *)((int)pMVar1->normal + iVar5 + 4) =
                   *(float *)((int)pMVar1->normal + iVar5 + 4) * -1.0;
              *(float *)((int)pMVar1->normal + iVar5 + 8) =
                   *(float *)((int)pMVar1->normal + iVar5 + 8) * -1.0;
              uVar4 = pMVar3->flags;
              if ((uVar4 & 2) != 0) {
                *(McdGeometryInstance **)((int)pMVar3->contacts->normal + iVar5 + 0x18) =
                     pMVar3->ins1;
                uVar4 = pMVar3->flags;
              }
              if ((uVar4 & 4) != 0) {
                *(McdGeometryInstance **)((int)pMVar3->contacts->normal + iVar5 + 0x14) =
                     pMVar3->ins2;
              }
            }
            iVar7 = iVar7 + 1;
            iVar5 = iVar5 + 0x28;
          } while (iVar7 < pMVar3->contactCount);
        }
        ir->normal[0] = pMVar3->normal[0] + ir->normal[0];
        ir->normal[1] = pMVar3->normal[1] + ir->normal[1];
        ir->normal[2] = pMVar3->normal[2] + ir->normal[2];
        iVar7 = pd->entries;
        pMVar6 = pd->start;
      }
      pMVar3 = pMVar3 + 1;
    } while (pMVar3 < pMVar6 + iVar7);
  }
  if (0 < iVar2) {
    MeVector3Normalize(ir->normal);
    iVar7 = McdContactSimplify(ir->normal,contacts,iVar2,ir->contacts,
                               ir->pair->request->contactMaxCount,
                               ir->pair->request->faceNormalsFirst,context->frame->mScale);
    ir->contactCount = iVar7;
  }
  return;
}


/* ==== McdBatchUnflatten ==== */

/* WARNING: Unknown calling convention */

MeBool McdBatchUnflatten(McdBatchContext *context,McdIntersectResult *resultArray,int *resultCount,
                        int resultMaxCount,McdContact *contactArray,int *contactCount,
                        int contactMaxCount)

{
  McdBatchPairData *pMVar1;
  McdIntersectResult *pMVar2;
  McdBatchEntry *pMVar3;
  McdRequest *pMVar4;
  int iVar5;
  void *pvVar6;
  int iVar7;
  McdContact *pMVar8;
  McdContact *pMVar9;
  McdBatchPairData *pd;
  
  iVar5 = context->nextSingleUnflattenPair;
  iVar7 = context->nextPairData;
  if (iVar5 < iVar7) {
    do {
      pMVar1 = context->pairData + iVar5;
      if (pMVar1->status == 2) {
                    /* Unresolved local var: McdBatchEntry * entry@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdRequestID request@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdIntersectResult * ir@[DW_OP_reg3(EBX)] */
        pMVar3 = pMVar1->start;
        iVar5 = *resultCount;
        pMVar4 = pMVar1->pair->request;
        if (resultMaxCount <= iVar5) {
          return 0;
        }
        if (contactMaxCount <= *contactCount + pMVar4->contactMaxCount) {
          return 0;
        }
        pMVar2 = resultArray + iVar5;
        *resultCount = iVar5 + 1;
        pMVar2->pair = pMVar1->pair;
        pMVar2->touch = 0;
        pMVar2->contactCount = 0;
        pMVar2->contacts = contactArray + *contactCount;
        if (pMVar3->touch != 0) {
          pMVar2->touch = 1;
          pMVar2->normal[0] = pMVar3->normal[0];
          pMVar2->normal[1] = pMVar3->normal[1];
          pMVar2->normal[2] = pMVar3->normal[2];
          pvVar6 = McdFrameworkGetInteractions
                             (context->frame,(uint)(pMVar3->geometryData1).type,
                              (uint)(pMVar3->geometryData2).type);
          if (*(int *)((int)pvVar6 + 0x14) == 0) {
            iVar5 = pMVar3->contactCount;
            pMVar2->contactCount = iVar5;
            pMVar8 = pMVar3->contacts;
            pMVar9 = pMVar2->contacts;
            for (iVar5 = (iVar5 * 5 & 0x1fffffffU) << 1; iVar5 != 0; iVar5 = iVar5 + -1) {
              pMVar9->position[0] = pMVar8->position[0];
              pMVar8 = (McdContact *)(pMVar8->position + 1);
              pMVar9 = (McdContact *)(pMVar9->position + 1);
            }
          }
          else {
            iVar5 = McdContactSimplify(pMVar2->normal,pMVar3->contacts,pMVar3->contactCount,
                                       pMVar2->contacts,pMVar4->contactMaxCount,
                                       pMVar4->faceNormalsFirst,context->frame->mScale);
            pMVar2->contactCount = iVar5;
          }
          *contactCount = *contactCount + pMVar2->contactCount;
        }
        pMVar1->status = 3;
        iVar7 = context->nextPairData;
      }
      iVar5 = context->nextSingleUnflattenPair + 1;
      context->nextSingleUnflattenPair = iVar5;
    } while (iVar5 < iVar7);
  }
  iVar5 = context->nextAggregateUnflattenPair;
  if (iVar5 < iVar7) {
    do {
                    /* Unresolved local var: McdBatchPairData * pd@[DW_OP_reg6(ESI)] */
      pMVar1 = context->pairData + iVar5;
      if (pMVar1->status == 0x12) {
                    /* Unresolved local var: McdRequestID request@[DW_OP_reg1(ECX)]
                       Unresolved local var: McdIntersectResult * ir@[DW_OP_reg7(EDI)] */
        iVar5 = *resultCount;
        if ((iVar5 == resultMaxCount) ||
           (contactMaxCount <= *contactCount + pMVar1->pair->request->contactMaxCount)) {
          return 0;
        }
        pMVar2 = resultArray + iVar5;
        *resultCount = iVar5 + 1;
        pMVar2->pair = pMVar1->pair;
        pMVar2->touch = 0;
        pMVar2->contactCount = 0;
        pMVar2->contacts = contactArray + *contactCount;
        if (pMVar1->entries != 0) {
          McdBatchUnflattenAggregate(context,pMVar2,pMVar1);
          *contactCount = *contactCount + pMVar2->contactCount;
        }
        pMVar1->status = 0x13;
        iVar7 = context->nextPairData;
      }
      iVar5 = context->nextAggregateUnflattenPair + 1;
      context->nextAggregateUnflattenPair = iVar5;
    } while (iVar5 < iVar7);
  }
  return 1;
}


/* ==== McdBatchContextCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdBatchContext * McdBatchContextCreate(McdFramework *frame)

{
  McdBatchContext *pMVar1;
  MeReal (*paaMVar2) [4] [4];
  MeMatrix4Ptr **pppaMVar3;
  McdBatchEntry **ppMVar4;
  McdBatchEntry *pMVar5;
  McdBatchPairData *pMVar6;
  McdBatchContactPool *pMVar7;
  McdContact *pMVar8;
  undefined4 uVar9;
  int local_18;
  int i;
  
                    /* Unresolved local var: McdBatchContext * context@[DW_OP_reg7(EDI)] */
  pMVar1 = (McdBatchContext *)(*_MeMemoryAPI)(0x50);
  pMVar1->frame = frame;
  pMVar1->typeCount = frame->geometryRegisteredCountMax;
  paaMVar2 = (MeReal (*) [4] [4])(*_DAT_00012038)(64000,0x10);
  pMVar1->tmArray = paaMVar2;
  pppaMVar3 = (MeMatrix4Ptr **)(*_MeMemoryAPI)(4000);
  pMVar1->tmTrack = pppaMVar3;
  pMVar1->tmMaxCount = 1000;
  ppMVar4 = (McdBatchEntry **)(*_MeMemoryAPI)(pMVar1->typeCount * pMVar1->typeCount * 4);
  pMVar1->table = ppMVar4;
  pMVar5 = (McdBatchEntry *)(*_MeMemoryAPI)(96000);
  pMVar1->maxEntryCount = 1000;
  pMVar1->entryArray = pMVar5;
  pMVar6 = (McdBatchPairData *)(*_MeMemoryAPI)(20000);
  pMVar1->pairDataMaxCount = 1000;
  pMVar1->pairData = pMVar6;
  pMVar7 = (McdBatchContactPool *)(*_MeMemoryAPI)(0xcc);
  pMVar1->poolMaxCount = 0x11;
  pMVar1->pools = pMVar7;
  pMVar8 = (McdContact *)(*_MeMemoryAPI)(0xa000);
  pMVar7->contacts = pMVar8;
  pMVar1->pools->contactMaxCount = 0x400;
  local_18 = 0xc;
  i = 0xf;
  do {
    pMVar7 = pMVar1->pools;
    uVar9 = (*_MeMemoryAPI)(0x1400);
    *(undefined4 *)((int)&pMVar7->contacts + local_18) = uVar9;
    *(undefined4 *)((int)&pMVar1->pools->contactMaxCount + local_18) = 0x80;
    pMVar7 = pMVar1->pools;
    uVar9 = (*_MeMemoryAPI)(0x1400);
    *(undefined4 *)((int)&pMVar7[1].contacts + local_18) = uVar9;
    *(undefined4 *)((int)&pMVar1->pools[1].contactMaxCount + local_18) = 0x80;
    pMVar7 = pMVar1->pools;
    uVar9 = (*_MeMemoryAPI)(0x1400);
    *(undefined4 *)((int)&pMVar7[2].contacts + local_18) = uVar9;
    *(undefined4 *)((int)&pMVar1->pools[2].contactMaxCount + local_18) = 0x80;
    pMVar7 = pMVar1->pools;
    uVar9 = (*_MeMemoryAPI)(0x1400);
    *(undefined4 *)((int)&pMVar7[3].contacts + local_18) = uVar9;
    *(undefined4 *)((int)&pMVar1->pools[3].contactMaxCount + local_18) = 0x80;
    i = i + -4;
    local_18 = local_18 + 0x30;
  } while (-1 < i);
  return pMVar1;
}


/* ==== McdBatchContextReset ==== */

void McdBatchContextReset(McdBatchContext *context)

{
  context->state = 0;
  return;
}


/* ==== McdBatchContextDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdBatchContextDestroy(McdBatchContext *context)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  (*_DAT_0001203c)(context->table);
  iVar3 = 0xf;
  (*_DAT_00012040)(context->tmArray);
  (*_DAT_0001203c)(context->tmTrack);
  (*_DAT_0001203c)(context->entryArray);
  (*_DAT_0001203c)(context->pairData);
  iVar2 = 0xc;
  (*_DAT_0001203c)(context->pools->contacts);
  do {
    (*_DAT_0001203c)(*(undefined4 *)((int)&context->pools->contacts + iVar2));
    (*_DAT_0001203c)(*(undefined4 *)((int)&context->pools[1].contacts + iVar2));
    (*_DAT_0001203c)(*(undefined4 *)((int)&context->pools[2].contacts + iVar2));
    puVar1 = (undefined4 *)((int)&context->pools[3].contacts + iVar2);
    iVar2 = iVar2 + 0x30;
    (*_DAT_0001203c)(*puVar1);
    iVar3 = iVar3 + -4;
  } while (-1 < iVar3);
  (*_DAT_0001203c)(context->pools);
  (*_DAT_0001203c)(context);
  return;
}


/* ==== McdBatchIntersectEach ==== */

MeBool McdBatchIntersectEach
                 (McdBatchContext *context,McdModelPairContainer *pairs,
                 McdIntersectResult *resultArray,int *resultCount,int resultMaxCount,
                 McdContact *contactArray,int *contactCount,int contactMaxCount)

{
  MeMatrix4Ptr **pppaMVar1;
  MeBool MVar2;
  uint uVar3;
  int iVar4;
  
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)] */
  *resultCount = 0;
  *contactCount = 0;
  uVar3 = context->state;
  if ((uVar3 & 4) == 0) {
    if ((uVar3 & 6) == 0) {
      if (uVar3 == 0) {
        context->nextFlattenPair = pairs->helloFirst;
      }
      MVar2 = McdBatchFlatten(context,pairs);
      if (MVar2 == 0) {
        context->state = context->state | 1;
      }
      else {
        context->state = context->state & 0xfffffffe;
      }
    }
    MVar2 = McdBatchTest(context);
    if (MVar2 == 0) {
      context->state = context->state | 2;
    }
    else {
      context->state = context->state & 0xfffffffd;
    }
    context->nextSingleUnflattenPair = 0;
    context->nextAggregateUnflattenPair = 0;
  }
  MVar2 = McdBatchUnflatten(context,resultArray,resultCount,resultMaxCount,contactArray,contactCount
                            ,contactMaxCount);
  if (MVar2 == 0) {
    context->state = context->state | 4;
  }
  else {
    context->state = context->state & 0xfffffffb;
  }
  uVar3 = context->state;
  if ((uVar3 & 6) == 0) {
    iVar4 = 0;
    if (0 < context->nextTM) {
      do {
        pppaMVar1 = context->tmTrack + iVar4;
        iVar4 = iVar4 + 1;
        **pppaMVar1 = (MeMatrix4Ptr)0x0;
      } while (iVar4 < context->nextTM);
      uVar3 = context->state;
    }
    context->nextTM = 0;
    if (uVar3 == 0) {
      iVar4 = context->nextPairData;
      if (0 < iVar4) {
        uVar3 = -iVar4 & 3;
        if (uVar3 != 0) {
          if (uVar3 < 3) {
            if (uVar3 < 2) {
              iVar4 = iVar4 + -1;
            }
            iVar4 = iVar4 + -1;
          }
          iVar4 = iVar4 + -1;
          if (iVar4 == 0) {
            return 1;
          }
        }
        do {
          iVar4 = iVar4 + -4;
        } while (iVar4 != 0);
      }
      return 1;
    }
  }
  return 0;
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


/* ==== McdBatchTest ==== */

MeBool McdBatchTest(McdBatchContext *context)

{
  MeBool MVar1;
  uint uVar2;
  uint type2;
  uint type1;
  
                    /* Unresolved local var: uint type1@[DW_OP_reg6(ESI)]
                       Unresolved local var: uint type2@[DW_OP_reg3(EBX)] */
  type1 = 0;
  context->pools->contactCount = 0;
  uVar2 = context->typeCount;
  if (uVar2 != 0) {
    do {
      type2 = 0;
      if (uVar2 != 0) {
        do {
                    /* Unresolved local var: uint index@[DW_OP_reg0(EAX)] */
          if (context->table[uVar2 * type1 + type2] != (McdBatchEntry *)0x0) {
            MVar1 = McdBatchIntersectBucket(context,type1,type2);
            if (MVar1 == 0) {
              return 0;
            }
            uVar2 = context->typeCount;
          }
          type2 = type2 + 1;
        } while (type2 < uVar2);
      }
      type1 = type1 + 1;
    } while (type1 < uVar2);
  }
  return 1;
}


