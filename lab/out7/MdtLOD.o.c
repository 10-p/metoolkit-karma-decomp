/* ==== MdtLODLastPartition ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MdtLODLastPartition(MdtPartitionOutput *po,MdtPartitionParams *params)

{
  MdtBody **ppMVar1;
  int iVar2;
  MdtBaseConstraint *pMVar3;
  int iVar4;
  MdtPartitionInfo *pMVar5;
  int iVar6;
  MdtContactGroupID_conflict pMVar7;
  MdtLODPartitionData *pMVar8;
  uint uVar9;
  void *pvVar10;
  MeBool MVar11;
  MdtBody **ppMVar12;
  uint uVar13;
  MdtBody *pMVar14;
  int iVar15;
  int iVar16;
  undefined4 extraout_EDX;
  MdtLODPartitionData *pMVar17;
  MdtBaseConstraint **ppMVar18;
  MeReal MVar19;
  int aiStack_90 [6];
  int aiStack_78 [3];
  MdtLODPartitionData local_6c;
  int b0PIx;
  MdtBody *b1;
  MdtBody *b0;
  MdtContactGroupID_conflict group;
  MdtBaseConstraint *currentCon;
  MdtLODPartitionData *pdataArray;
  int partitionCount;
  MdtBaseConstraint **clist;
  int nCon;
  int p;
  int i;
  MeHeap q;
  
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)]
                       Unresolved local var: MdtPartitionInfo * info@[???]
                       Unresolved local var: int nBod@[DW_OP_reg6(ESI)]
                       Unresolved local var: MdtBody * * blist@[DW_OP_reg3(EBX)]
                       Unresolved local var: void * * qMem@[DW_OP_reg2(EDX)] */
  p = po->nPartitions + -1;
  uVar13 = po->bodiesSize[p];
  ppMVar12 = po->bodies + po->bodiesStart[p];
  iVar2 = po->constraintsSize[p];
  nCon = iVar2;
  partitionCount = 0;
  clist = po->constraints + po->constraintsStart[p];
  iVar6 = -(iVar2 * 4 + 0x13U & 0xfffffff0);
  *(code **)((int)aiStack_78 + iVar6 + 8) = CompareImportance;
  *(int *)((int)aiStack_78 + iVar6 + 4) = iVar2 + 1;
  *(int *)((int)aiStack_78 + iVar6) = (int)&local_6c + iVar6;
  *(MeHeap **)((int)aiStack_90 + iVar6 + 0x14) = &q;
  *(undefined4 *)((int)aiStack_90 + iVar6 + 0x10) = 0x1006d;
  MeHeapInit(*(void **)((int)aiStack_90 + iVar6 + 0x14),*(void **)((int)aiStack_78 + iVar6),
             *(int *)((int)aiStack_78 + iVar6 + 4),*(void **)((int)aiStack_78 + iVar6 + 8));
  ppMVar18 = clist;
  iVar2 = -(uVar13 * 0x14 + 0xf & 0xfffffff0);
  i = 0;
  pdataArray = (MdtLODPartitionData *)((int)&local_6c + iVar2 + iVar6);
  if (0 < (int)uVar13) {
    uVar9 = uVar13 & 3;
    if ((int)uVar13 < 2) {
LAB_0001009c:
      iVar16 = i + 1;
      ppMVar12[i]->LODpartIndex = -1;
      i = iVar16;
      if ((int)uVar13 <= iVar16) goto LAB_000100f8;
    }
    else if (uVar9 != 0) {
      if (1 < uVar9) {
        if (2 < uVar9) {
          (*ppMVar12)->LODpartIndex = -1;
        }
        i = (int)(2 < uVar9);
        ppMVar1 = ppMVar12 + i;
        i = i + 1;
        (*ppMVar1)->LODpartIndex = -1;
      }
      goto LAB_0001009c;
    }
    do {
      ppMVar12[i]->LODpartIndex = -1;
      ppMVar12[i + 1]->LODpartIndex = -1;
      ppMVar12[i + 2]->LODpartIndex = -1;
      iVar16 = i + 4;
      ppMVar12[i + 3]->LODpartIndex = -1;
      i = iVar16;
    } while (iVar16 < (int)uVar13);
  }
LAB_000100f8:
  if (0 < nCon) {
    i = nCon;
    if ((nCon & 1U) != 0) {
      ((*clist)->head).LODpartIndex = -1;
      ((*clist)->head).rowBudget = 0;
      *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = *clist;
      *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10138;
      group = MdtConstraintDCastContactGroup(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
      iVar16 = 0;
      if (group != (MdtContactGroupID_conflict)0x0) {
        pMVar3 = *ppMVar18;
        *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = pMVar3;
        *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x1014d;
        iVar16 = MdtConstraintGetRowCount(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
        (pMVar3->head).maxRows = iVar16;
      }
      pMVar3 = *ppMVar18;
      *(int *)((int)aiStack_78 + iVar2 + iVar6 + 8) = iVar16;
      *(int *)((int)aiStack_78 + iVar2 + iVar6 + 4) = iVar16;
      *(MdtPartitionParams **)((int)aiStack_78 + iVar2 + iVar6) = params;
      *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = *ppMVar18;
      *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10163;
      MVar19 = ConstraintCalcImportance
                         (*(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14),
                          *(MdtPartitionParams **)((int)aiStack_78 + iVar2 + iVar6));
      pMVar7 = group;
      (pMVar3->head).importance = MVar19;
      if ((group == (MdtContactGroupID_conflict)0x0) || (0 < (group->head).maxRows)) {
        *(MdtContactGroupID_conflict *)((int)aiStack_78 + iVar2 + iVar6 + 8) = group;
        *(MdtContactGroupID_conflict *)((int)aiStack_78 + iVar2 + iVar6 + 4) = pMVar7;
        *(MdtBaseConstraint **)((int)aiStack_78 + iVar2 + iVar6) = *ppMVar18;
        *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
        *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x1018b;
        MeHeapPush(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14),
                   *(void **)((int)aiStack_78 + iVar2 + iVar6));
      }
      ppMVar18 = ppMVar18 + 1;
      i = i + -1;
      if (i == 0) goto LAB_000102af;
    }
    do {
      ((*ppMVar18)->head).LODpartIndex = -1;
      ((*ppMVar18)->head).rowBudget = 0;
      *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = *ppMVar18;
      *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x101c3;
      group = MdtConstraintDCastContactGroup(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
      iVar16 = 0;
      if (group != (MdtContactGroupID_conflict)0x0) {
        pMVar3 = *ppMVar18;
        *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = pMVar3;
        *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x101d8;
        iVar16 = MdtConstraintGetRowCount(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
        (pMVar3->head).maxRows = iVar16;
      }
      pMVar3 = *ppMVar18;
      *(int *)((int)aiStack_78 + iVar2 + iVar6 + 8) = iVar16;
      *(int *)((int)aiStack_78 + iVar2 + iVar6 + 4) = iVar16;
      *(MdtPartitionParams **)((int)aiStack_78 + iVar2 + iVar6) = params;
      *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = *ppMVar18;
      *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x101ee;
      MVar19 = ConstraintCalcImportance
                         (*(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14),
                          *(MdtPartitionParams **)((int)aiStack_78 + iVar2 + iVar6));
      pMVar7 = group;
      (pMVar3->head).importance = MVar19;
      if ((group == (MdtContactGroupID_conflict)0x0) || (0 < (group->head).maxRows)) {
        *(MdtContactGroupID_conflict *)((int)aiStack_78 + iVar2 + iVar6 + 8) = group;
        *(MdtContactGroupID_conflict *)((int)aiStack_78 + iVar2 + iVar6 + 4) = pMVar7;
        *(MdtBaseConstraint **)((int)aiStack_78 + iVar2 + iVar6) = *ppMVar18;
        *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
        *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10216;
        MeHeapPush(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14),
                   *(void **)((int)aiStack_78 + iVar2 + iVar6));
      }
      local_6c.rowCount = (int)(ppMVar18 + 1);
      (ppMVar18[1]->head).LODpartIndex = -1;
      (ppMVar18[1]->head).rowBudget = 0;
      *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = ppMVar18[1];
      *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10245;
      group = MdtConstraintDCastContactGroup(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
      iVar16 = 0;
      if (group != (MdtContactGroupID_conflict)0x0) {
        pMVar3 = ppMVar18[1];
        *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = pMVar3;
        *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x1025b;
        iVar16 = MdtConstraintGetRowCount(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
        (pMVar3->head).maxRows = iVar16;
      }
      iVar15 = local_6c.rowCount;
      pMVar3 = *(MdtBaseConstraint **)local_6c.rowCount;
      *(int *)((int)aiStack_78 + iVar2 + iVar6 + 8) = iVar16;
      *(int *)((int)aiStack_78 + iVar2 + iVar6 + 4) = iVar16;
      *(MdtPartitionParams **)((int)aiStack_78 + iVar2 + iVar6) = params;
      *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) =
           *(MdtBaseConstraint **)iVar15;
      *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10274;
      MVar19 = ConstraintCalcImportance
                         (*(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14),
                          *(MdtPartitionParams **)((int)aiStack_78 + iVar2 + iVar6));
      iVar16 = local_6c.rowCount;
      (pMVar3->head).importance = MVar19;
      if ((group == (MdtContactGroupID_conflict)0x0) || (0 < (group->head).maxRows)) {
        *(undefined4 *)((int)aiStack_78 + iVar2 + iVar6 + 8) = extraout_EDX;
        *(undefined4 *)((int)aiStack_78 + iVar2 + iVar6 + 4) = extraout_EDX;
        *(MdtBaseConstraint **)((int)aiStack_78 + iVar2 + iVar6) = *(MdtBaseConstraint **)iVar16;
        *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
        *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x1029f;
        MeHeapPush(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14),
                   *(void **)((int)aiStack_78 + iVar2 + iVar6));
      }
      ppMVar18 = ppMVar18 + 2;
      i = i + -2;
    } while (i != 0);
  }
LAB_000102af:
  *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
  *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x102bb;
  currentCon = MeHeapPop(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
  if (currentCon != (MdtBaseConstraint *)0x0) {
    local_6c.bodyArray = (MdtBody **)(pdataArray + partitionCount);
    do {
      pMVar8 = pdataArray;
      b0 = (currentCon->head).mdtbody[0];
      b1 = (currentCon->head).mdtbody[1];
      iVar16 = b0->LODpartIndex;
      b0PIx = iVar16;
      if (b1 == (MdtBody *)0x0) {
        if (iVar16 == -1) {
          *(int *)local_6c.bodyArray = 0;
          *(int *)((int)local_6c.bodyArray + 8) = 0;
          *(MdtBody ***)((int)local_6c.bodyArray + 4) = (MdtBody **)0x0;
          *(int *)((int)local_6c.bodyArray + 0x10) = 0;
          *(MdtBaseConstraint ***)((int)local_6c.bodyArray + 0xc) = (MdtBaseConstraint **)0x0;
          iVar16 = partitionCount;
          *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
          local_6c.bodyArray = (MdtBody **)((int)local_6c.bodyArray + 0x14);
          *(MdtPartitionParams **)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = params;
          pMVar8 = pdataArray;
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0xc) = partitionCount;
          partitionCount = partitionCount + 1;
          *(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 8) = pMVar8;
          *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 4) = currentCon;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6) = 0x109f5;
          MVar11 = IncrementConstraintRowBudget
                             (*(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 4),
                              *(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 8),
                              *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0xc),
                              *(MdtPartitionParams **)((int)aiStack_90 + iVar2 + iVar6 + 0x10),
                              *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
          pMVar8 = pdataArray;
          if (MVar11 != 0) {
            pMVar17 = pdataArray + iVar16;
            *(MdtLODPartitionData **)((int)aiStack_78 + iVar2 + iVar6 + 8) = pdataArray;
            *(MdtLODPartitionData **)((int)aiStack_78 + iVar2 + iVar6 + 4) = pMVar8;
            *(int *)((int)aiStack_78 + iVar2 + iVar6) = pMVar17->bodyCount * 4 + 4;
            *(MdtBody ***)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = pMVar17->bodyArray;
            *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10a20;
            ppMVar12 = (MdtBody **)(*_DAT_00012030)();
            pMVar17->bodyArray = ppMVar12;
            b0->LODpartIndex = iVar16;
            iVar16 = pMVar17->bodyCount;
            ppMVar12 = pMVar17->bodyArray;
            pMVar14 = b0;
LAB_0001086e:
                    /* Unresolved local var: int newPIx@[???]
                       Unresolved local var: MdtLODPartitionData * pData@[DW_OP_reg3(EBX)] */
            ppMVar12[iVar16] = pMVar14;
            pMVar17->bodyCount = pMVar17->bodyCount + 1;
          }
        }
        else {
LAB_00010800:
          *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
LAB_000107de:
          iVar16 = b0PIx;
          *(MdtPartitionParams **)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = params;
          pMVar8 = pdataArray;
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0xc) = iVar16;
          pMVar3 = currentCon;
          *(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 8) = pMVar8;
          *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 4) = pMVar3;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6) = 0x107f3;
          IncrementConstraintRowBudget
                    (*(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 4),
                     *(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 8),
                     *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0xc),
                     *(MdtPartitionParams **)((int)aiStack_90 + iVar2 + iVar6 + 0x10),
                     *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
        }
      }
      else {
                    /* Unresolved local var: int b1PIx@[DW_OP_reg6(ESI)] */
        iVar15 = b1->LODpartIndex;
        if (iVar16 == -1) {
          if (iVar15 == -1) {
                    /* Unresolved local var: int newPIx@[???] */
            *(int *)local_6c.bodyArray = 0;
            *(int *)((int)local_6c.bodyArray + 8) = 0;
            *(MdtBody ***)((int)local_6c.bodyArray + 4) = (MdtBody **)0x0;
            *(int *)((int)local_6c.bodyArray + 0x10) = 0;
            *(MdtBaseConstraint ***)((int)local_6c.bodyArray + 0xc) = (MdtBaseConstraint **)0x0;
            iVar16 = partitionCount;
            *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
            local_6c.bodyArray = (MdtBody **)((int)local_6c.bodyArray + 0x14);
            *(MdtPartitionParams **)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = params;
            pMVar8 = pdataArray;
            *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0xc) = partitionCount;
            partitionCount = partitionCount + 1;
            *(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 8) = pMVar8;
            *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 4) = currentCon;
            *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6) = 0x1092b;
            MVar11 = IncrementConstraintRowBudget
                               (*(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 4),
                                *(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 8),
                                *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0xc),
                                *(MdtPartitionParams **)((int)aiStack_90 + iVar2 + iVar6 + 0x10),
                                *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
            pMVar17 = pdataArray;
            if (MVar11 != 0) {
                    /* Unresolved local var: MdtLODPartitionData * pData@[DW_OP_reg3(EBX)] */
              *(MeBool *)((int)aiStack_78 + iVar2 + iVar6 + 8) = MVar11;
              pMVar17 = pMVar17 + iVar16;
              *(MeBool *)((int)aiStack_78 + iVar2 + iVar6 + 4) = MVar11;
              *(int *)((int)aiStack_78 + iVar2 + iVar6) = pMVar17->bodyCount * 4 + 4;
              *(MdtBody ***)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = pMVar17->bodyArray;
              *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10956;
              ppMVar12 = (MdtBody **)(*_DAT_00012030)();
              pMVar17->bodyArray = ppMVar12;
              b0->LODpartIndex = iVar16;
              pMVar17->bodyArray[pMVar17->bodyCount] = b0;
              iVar15 = pMVar17->bodyCount + 1;
              pMVar17->bodyCount = iVar15;
                    /* Unresolved local var: MdtLODPartitionData * pData@[???] */
              *(int *)((int)aiStack_78 + iVar2 + iVar6) = iVar15 * 4 + 4;
              *(MdtBody ***)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = pMVar17->bodyArray;
              *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10989;
              ppMVar12 = (MdtBody **)(*_DAT_00012030)();
              pMVar17->bodyArray = ppMVar12;
              b1->LODpartIndex = iVar16;
              goto LAB_00010865;
            }
          }
          else {
            *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
            pMVar3 = currentCon;
            *(MdtPartitionParams **)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = params;
            *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0xc) = iVar15;
            *(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 8) = pMVar8;
            *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 4) = pMVar3;
            *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6) = 0x1089b;
            MVar11 = IncrementConstraintRowBudget
                               (*(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 4),
                                *(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 8),
                                *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0xc),
                                *(MdtPartitionParams **)((int)aiStack_90 + iVar2 + iVar6 + 0x10),
                                *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
            if (MVar11 != 0) {
                    /* Unresolved local var: MdtLODPartitionData * pData@[DW_OP_reg3(EBX)] */
              pMVar17 = pdataArray + iVar15;
              *(int *)((int)aiStack_78 + iVar2 + iVar6 + 8) = iVar15 * 5;
              *(int *)((int)aiStack_78 + iVar2 + iVar6 + 4) = iVar15 * 5;
              *(int *)((int)aiStack_78 + iVar2 + iVar6) = pMVar17->bodyCount * 4 + 4;
              *(MdtBody ***)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = pMVar17->bodyArray;
              *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x108c6;
              ppMVar12 = (MdtBody **)(*_DAT_00012030)();
              pMVar17->bodyArray = ppMVar12;
              b0->LODpartIndex = iVar15;
              iVar16 = pMVar17->bodyCount;
              ppMVar12 = pMVar17->bodyArray;
              pMVar14 = b0;
              goto LAB_0001086e;
            }
          }
        }
        else if (iVar15 == -1) {
          *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
          pMVar8 = pdataArray;
          *(MdtPartitionParams **)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = params;
          pMVar3 = currentCon;
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0xc) = iVar16;
          *(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 8) = pMVar8;
          *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 4) = pMVar3;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6) = 0x10825;
          MVar11 = IncrementConstraintRowBudget
                             (*(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 4),
                              *(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 8),
                              *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0xc),
                              *(MdtPartitionParams **)((int)aiStack_90 + iVar2 + iVar6 + 0x10),
                              *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
          pMVar17 = pdataArray;
          iVar16 = b0PIx;
          if (MVar11 != 0) {
                    /* Unresolved local var: MdtLODPartitionData * pData@[DW_OP_reg3(EBX)] */
            *(MdtBaseConstraint **)((int)aiStack_78 + iVar2 + iVar6 + 8) = pMVar3;
            *(MdtBaseConstraint **)((int)aiStack_78 + iVar2 + iVar6 + 4) = pMVar3;
            pMVar17 = pMVar17 + iVar16;
            *(int *)((int)aiStack_78 + iVar2 + iVar6) = pMVar17->bodyCount * 4 + 4;
            *(MdtBody ***)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = pMVar17->bodyArray;
            *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10853;
            ppMVar12 = (MdtBody **)(*_DAT_00012030)();
            pMVar17->bodyArray = ppMVar12;
            b1->LODpartIndex = b0PIx;
LAB_00010865:
            iVar16 = pMVar17->bodyCount;
            ppMVar12 = pMVar17->bodyArray;
            pMVar14 = b1;
            goto LAB_0001086e;
          }
        }
        else {
          if (iVar16 == iVar15) goto LAB_00010800;
                    /* Unresolved local var: int newRowCount@[DW_OP_reg3(EBX)]
                       Unresolved local var: MdtContactGroupID.conflict group@[DW_OP_reg0(EAX)] */
          iVar4 = pdataArray[iVar15].rowCount;
          iVar16 = pdataArray[iVar16].rowCount;
          *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = currentCon;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x1033b;
          pvVar10 = MdtConstraintDCastContactGroup
                              (*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
          if ((iVar4 + iVar16 + 1 < params->maxMatrixSize) || (pvVar10 == (void *)0x0)) {
            *(void **)((int)aiStack_78 + iVar2 + iVar6 + 8) = pvVar10;
            *(int *)((int)aiStack_78 + iVar2 + iVar6 + 4) = iVar15;
            *(int *)((int)aiStack_78 + iVar2 + iVar6) = b0PIx;
            *(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = pdataArray;
            *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x107db;
            MergePartitions(*(MdtLODPartitionData **)((int)aiStack_90 + iVar2 + iVar6 + 0x14),
                            *(int *)((int)aiStack_78 + iVar2 + iVar6),
                            *(int *)((int)aiStack_78 + iVar2 + iVar6 + 4));
            *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
            goto LAB_000107de;
          }
        }
      }
      *(MeHeap **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = &q;
      *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x1035e;
      currentCon = MeHeapPop(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
    } while (currentCon != (MdtBaseConstraint *)0x0);
  }
  po->nPartitions = po->nPartitions + -1;
  po->totalBodies = po->totalBodies - po->bodiesSize[p];
  pMVar5 = po->info;
  iVar16 = (po->overallInfo).rowCount;
  po->totalConstraints = po->totalConstraints - po->constraintsSize[p];
  iVar15 = pMVar5[p].jointCount;
  (po->overallInfo).rowCount = iVar16 - pMVar5[p].rowCount;
  iVar16 = pMVar5[p].contactCount;
  (po->overallInfo).jointCount = (po->overallInfo).jointCount - iVar15;
  (po->overallInfo).contactCount = (po->overallInfo).contactCount - iVar16;
  i = 0;
  if (0 < partitionCount) {
    local_6c.bodyCount = 0;
    do {
      if (0 < *(int *)((int)&pdataArray->bodyCount + local_6c.bodyCount)) {
        po->bodiesSize[po->nPartitions] = 0;
        po->constraintsSize[po->nPartitions] = 0;
        *(MdtPartitionInfo **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = po->info + po->nPartitions;
        *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10421;
        MdtPartInfoReset(*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14));
        po->bodiesStart[po->nPartitions] = po->totalBodies;
        po->constraintsStart[po->nPartitions] = po->totalConstraints;
        if (0 < *(int *)((int)&pdataArray->bodyCount + local_6c.bodyCount)) {
          local_6c.conCount = local_6c.bodyCount;
          iVar16 = 0;
          do {
                    /* Unresolved local var: MdtBody * bp@[DW_OP_reg1(ECX)] */
            pMVar14 = *(MdtBody **)
                       (*(int *)((int)&pdataArray->bodyArray + local_6c.bodyCount) + iVar16 * 4);
            iVar16 = iVar16 + 1;
            pMVar14->arrayIdPartition = po->bodiesSize[po->nPartitions];
            pMVar14->arrayIdWorld =
                 po->bodiesSize[po->nPartitions] + po->bodiesStart[po->nPartitions];
            pMVar14->partitionIndex = po->nPartitions;
            po->bodies[po->totalBodies] = pMVar14;
            po->totalBodies = po->totalBodies + 1;
            po->bodiesSize[po->nPartitions] = po->bodiesSize[po->nPartitions] + 1;
          } while (iVar16 < *(int *)((int)&pdataArray->bodyCount + local_6c.bodyCount));
        }
        iVar16 = 0;
        if (0 < *(int *)((int)&pdataArray->conCount + local_6c.bodyCount)) {
          local_6c.conArray = (MdtBaseConstraint **)local_6c.bodyCount;
          do {
                    /* Unresolved local var: MdtBaseConstraint * cp@[DW_OP_reg3(EBX)] */
            iVar15 = *(int *)((int)&pdataArray->conArray + (int)local_6c.conArray);
            pMVar3 = *(MdtBaseConstraint **)(iVar15 + iVar16 * 4);
            *(int *)((int)aiStack_78 + iVar2 + iVar6 + 8) = iVar15;
            iVar16 = iVar16 + 1;
            *(int *)((int)aiStack_78 + iVar2 + iVar6 + 4) = iVar15;
            *(MdtPartitionParams **)((int)aiStack_78 + iVar2 + iVar6) = params;
            *(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = pMVar3;
            *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x104df;
            ResizeConstraint(*(MdtBaseConstraint **)((int)aiStack_90 + iVar2 + iVar6 + 0x14),
                             *(MdtPartitionParams **)((int)aiStack_78 + iVar2 + iVar6));
            po->constraints[po->totalConstraints] = pMVar3;
            po->totalConstraints = po->totalConstraints + 1;
            po->constraintsSize[po->nPartitions] = po->constraintsSize[po->nPartitions] + 1;
            *(MdtBaseConstraint **)((int)aiStack_78 + iVar2 + iVar6) = pMVar3;
            *(MdtPartitionInfo **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) =
                 po->info + po->nPartitions;
            *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10510;
            MdtPartInfoAddConstraint
                      (*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14),
                       *(void **)((int)aiStack_78 + iVar2 + iVar6));
          } while (iVar16 < *(int *)((int)&pdataArray->conCount + (int)local_6c.conArray));
        }
        uVar13 = po->info[po->nPartitions].rowCount;
        if ((uVar13 & 3) != 0) {
          if ((int)uVar13 < 0) {
            uVar13 = uVar13 + 3;
          }
          uVar13 = (uVar13 & 0xfffffffc) + 4;
        }
        po->info[po->nPartitions].rowCount = uVar13;
        iVar16 = po->nPartitions;
        pMVar5 = po->info;
        iVar15 = pMVar5[iVar16].jointCount;
        (po->overallInfo).rowCount = (po->overallInfo).rowCount + pMVar5[iVar16].rowCount;
        iVar4 = pMVar5[iVar16].contactCount;
        (po->overallInfo).jointCount = (po->overallInfo).jointCount + iVar15;
        iVar15 = (po->overallInfo).contactCount;
        po->nPartitions = iVar16 + 1;
        (po->overallInfo).contactCount = iVar15 + iVar4;
        iVar15 = params->maxMatrixSize;
        if (pMVar5[iVar16].rowCount <= iVar15) {
          *(int *)((int)aiStack_78 + iVar2 + iVar6 + 8) = iVar15;
          *(int *)((int)aiStack_78 + iVar2 + iVar6 + 4) = iVar15;
          *(MdtPartitionParams **)((int)aiStack_78 + iVar2 + iVar6) = params;
          *(MdtPartitionOutput **)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = po;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x107b9;
          MdtAutoDisableLastPartition
                    (*(void **)((int)aiStack_90 + iVar2 + iVar6 + 0x14),
                     *(void **)((int)aiStack_78 + iVar2 + iVar6));
        }
      }
      i = i + 1;
      local_6c.bodyCount = (int)(local_6c.bodyCount + 0x14);
    } while (i < partitionCount);
    if (0 < partitionCount) {
      i = partitionCount;
      iVar16 = 0;
      uVar13 = -partitionCount & 3;
      if (uVar13 != 0) {
        if (uVar13 < 3) {
          if (uVar13 < 2) {
            if (pdataArray->bodyArray != (MdtBody **)0x0) {
              *(MdtBody ***)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = pdataArray->bodyArray;
              *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x107a7;
              (*_qsort)();
            }
            if (pdataArray->conArray != (MdtBaseConstraint **)0x0) {
              *(MdtBaseConstraint ***)((int)aiStack_90 + iVar2 + iVar6 + 0x14) =
                   pdataArray->conArray;
              *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10798;
              (*_qsort)();
            }
            iVar16 = 0x14;
            i = i + -1;
          }
          iVar15 = *(int *)((int)&pdataArray->bodyArray + iVar16);
          if (iVar15 != 0) {
            *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
            *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10762;
            (*_qsort)();
          }
          iVar15 = *(int *)((int)&pdataArray->conArray + iVar16);
          if (iVar15 != 0) {
            *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
            *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10750;
            (*_qsort)();
          }
          iVar16 = iVar16 + 0x14;
          i = i + -1;
        }
        iVar15 = *(int *)((int)&pdataArray->bodyArray + iVar16);
        if (iVar15 != 0) {
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x1073e;
          (*_qsort)();
        }
        iVar15 = *(int *)((int)&pdataArray->conArray + iVar16);
        if (iVar15 != 0) {
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x1072c;
          (*_qsort)();
        }
        iVar16 = iVar16 + 0x14;
        i = i + -1;
        if (i == 0) {
          return;
        }
      }
      do {
        iVar15 = *(int *)((int)&pdataArray->bodyArray + iVar16);
        if (iVar15 != 0) {
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x1071a;
          (*_qsort)();
        }
        iVar15 = *(int *)((int)&pdataArray->conArray + iVar16);
        if (iVar15 != 0) {
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x10708;
          (*_qsort)();
        }
        iVar15 = *(int *)((int)&pdataArray[1].bodyArray + iVar16);
        if (iVar15 != 0) {
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x106f6;
          (*_qsort)();
        }
        iVar15 = *(int *)((int)&pdataArray[1].conArray + iVar16);
        if (iVar15 != 0) {
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x106e4;
          (*_qsort)();
        }
        iVar15 = *(int *)((int)&pdataArray[2].bodyArray + iVar16);
        if (iVar15 != 0) {
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x106d5;
          (*_qsort)();
        }
        iVar15 = *(int *)((int)&pdataArray[2].conArray + iVar16);
        if (iVar15 != 0) {
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x106c6;
          (*_qsort)();
        }
        iVar15 = *(int *)((int)&pdataArray[3].bodyArray + iVar16);
        if (iVar15 != 0) {
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x106b7;
          (*_qsort)();
        }
        iVar15 = *(int *)((int)&pdataArray[3].conArray + iVar16);
        if (iVar15 != 0) {
          *(int *)((int)aiStack_90 + iVar2 + iVar6 + 0x14) = iVar15;
          *(undefined4 *)((int)aiStack_90 + iVar2 + iVar6 + 0x10) = 0x106a8;
          (*_qsort)();
        }
        iVar16 = iVar16 + 0x50;
        i = i + -4;
      } while (i != 0);
    }
  }
  return;
}


/* ==== ComparePenetration ==== */

int ComparePenetration(void *contact1,void *contact2)

{
  uint uVar1;
  
                    /* Unresolved local var: MdtContactID.conflict c1@[DW_OP_reg2(EDX)]
                       Unresolved local var: MdtContactID.conflict c2@[DW_OP_reg0(EAX)] */
  uVar1 = 0xffffffff;
  if (*(float *)(*(int *)contact2 + 0x178) <= *(float *)(*(int *)contact1 + 0x178)) {
    uVar1 = (uint)(*(float *)(*(int *)contact2 + 0x178) < *(float *)(*(int *)contact1 + 0x178));
  }
  return uVar1;
}


/* ==== ConstraintCalcImportance ==== */

MeReal ConstraintCalcImportance(MdtBaseConstraint *con,MdtPartitionParams *params)

{
  float fVar1;
  MdtBody *pMVar2;
  MdtBody *pMVar3;
  void *pvVar4;
  void *pvVar5;
  int iVar6;
  MeReal normVel;
  MeReal pen;
  MeVector3 vel;
  
                    /* Unresolved local var: MeReal imp@[DW_OP_reg13(ST2)]
                       Unresolved local var: MdtContactID.conflict contact@[DW_OP_reg3(EBX)]
                       Unresolved local var: MdtContactGroupID.conflict g@[DW_OP_reg6(ESI)]
                       Unresolved local var: MdtBodyID b0@[DW_OP_reg1(ECX)]
                       Unresolved local var: MdtBodyID b1@[DW_OP_reg2(EDX)] */
  pen = 0.0;
  normVel = 0.0;
  pvVar4 = MdtConstraintDCastContactGroup(con);
  pMVar2 = (con->head).mdtbody[0];
  pMVar3 = (con->head).mdtbody[1];
  fVar1 = 0.0;
  if (pvVar4 == (void *)0x0) {
    return 3.4028235e+38;
  }
  iVar6 = *(int *)((int)pvVar4 + 0x160);
  if (iVar6 != 0) {
    if (*(int *)((int)pvVar4 + 0x58) == 0) {
      fVar1 = (params->lodParams).toWorldBonus;
    }
    fVar1 = (float)*(int *)((int)pvVar4 + 100) * (params->lodParams).rowCountBias +
            fVar1 + (params->lodParams).zeroRowBonus;
    if (((0.0 < pMVar2->enabledTime) || ((pMVar2->flags & 4) == 0)) ||
       ((pMVar3 != (MdtBody *)0x0 && ((0.0 < pMVar3->enabledTime || ((pMVar3->flags & 4) == 0))))))
    {
      fVar1 = fVar1 + (params->lodParams).nonAutoBonus;
    }
    pvVar5 = *(void **)((int)pvVar4 + 0x164);
    if (pvVar5 != (void *)0x0) {
      do {
        pen = pen + *(float *)((int)pvVar5 + 0x178);
        MdtContactGetRelativeVelocity(pvVar5,vel);
        normVel = *(float *)((int)pvVar5 + 0x174) * vel[2] +
                  *(float *)((int)pvVar5 + 0x16c) * vel[0] +
                  *(float *)((int)pvVar5 + 0x170) * vel[1] + normVel;
        pvVar5 = *(void **)((int)pvVar5 + 0x1dc);
      } while (pvVar5 != (void *)0x0);
      iVar6 = *(int *)((int)pvVar4 + 0x160);
    }
    return normVel * (params->lodParams).normVelBias * (1.0 / (float)iVar6) +
           pen * (params->lodParams).penetrationBias * (1.0 / (float)iVar6) + fVar1;
  }
  return -3.4028235e+38;
}


/* ==== CompareImportance ==== */

int CompareImportance(void *elem1,void *elem2)

{
                    /* Unresolved local var: MdtBaseConstraint * c1@[???]
                       Unresolved local var: MdtBaseConstraint * c2@[???] */
  return (uint)(*(float *)((int)elem2 + 0x15c) < *(float *)((int)elem1 + 0x15c));
}


/* ==== ResizeConstraint ==== */

/* WARNING: Unknown calling convention */

void ResizeConstraint(MdtBaseConstraint *con,MdtPartitionParams *params)

{
  float fVar1;
  MdtContactID_conflict pMVar2;
  int iVar3;
  float fVar4;
  MdtContactGroupID_conflict pMVar5;
  undefined4 uVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  size_t sStackY_40;
  int aiStack_2c [2];
  int nRows;
  int nFrictionContacts;
  int nContacts;
  int initialNContacts;
  MdtContactGroupID_conflict group;
  
  sStackY_40 = 0x10c72;
  group = MdtConstraintDCastContactGroup(con);
  if (group != (MdtContactGroupID_conflict)0x0) {
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: MdtContactID.conflict contact@[DW_OP_reg0(EAX)]
                       Unresolved local var: MdtContactID.conflict * byPenetration@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeBool * removed@[DW_OP_reg7(EDI)] */
    initialNContacts = group->count;
    iVar8 = 0;
    uVar7 = group->count * 4 + 0xfU & 0xfffffff0;
    iVar3 = -uVar7;
    nFrictionContacts = 0;
    for (pMVar2 = group->first; pMVar2 != (MdtContactID_conflict)0x0; pMVar2 = pMVar2->nextContact)
    {
      if ((pMVar2->params).type != MdtContactTypeFrictionZero) {
        nFrictionContacts = nFrictionContacts + 1;
      }
      *(MdtContactID_conflict *)((int)aiStack_2c + iVar8 * 4 + iVar3) = pMVar2;
      *(undefined4 *)((int)aiStack_2c + iVar8 * 4 + uVar7 * -2) = 0;
      iVar8 = iVar8 + 1;
    }
    *(code **)(&stack0xffffffd0 + uVar7 * -2) = ComparePenetration;
    iVar8 = initialNContacts;
    *(undefined4 *)(&stack0xffffffcc + uVar7 * -2) = 4;
    *(int *)(&stack0xffffffc8 + uVar7 * -2) = iVar8;
    *(int *)(&stack0xffffffc4 + uVar7 * -2) = (int)aiStack_2c + iVar3;
    *(undefined4 *)((int)&sStackY_40 + uVar7 * -2) = 0x10ce8;
    qsort(*(void **)(&stack0xffffffc4 + uVar7 * -2),*(size_t *)(&stack0xffffffc8 + uVar7 * -2),
          *(size_t *)(&stack0xffffffcc + uVar7 * -2),
          *(__compar_fn_t *)(&stack0xffffffd0 + uVar7 * -2));
    nContacts = initialNContacts;
    iVar8 = (group->head).maxRows;
    nRows = iVar8;
    if ((group->head).rowBudget < iVar8) {
      do {
        fVar4 = (float)nFrictionContacts / (float)nContacts;
        fVar1 = (params->lodParams).frictionRatio;
        uVar6 = CONCAT22((short)((uint)iVar8 >> 0x10),
                         (ushort)(fVar4 < fVar1) << 8 | (ushort)(NAN(fVar4) || NAN(fVar1)) << 10 |
                         (ushort)(fVar4 == fVar1) << 0xe);
        if ((fVar4 < fVar1) || (uVar6 = 0, nFrictionContacts == 0)) {
          iVar8 = 0;
          if (*(int *)(*(int *)((int)aiStack_2c + iVar3) + 0x194) != 0) goto LAB_00010d30;
          iVar9 = *(int *)((int)aiStack_2c + uVar7 * -2);
          while (iVar9 == 1) {
LAB_00010d30:
            do {
              iVar8 = iVar8 + 1;
            } while (*(int *)(*(int *)((int)aiStack_2c + iVar8 * 4 + iVar3) + 0x194) != 0);
            iVar9 = *(int *)((int)aiStack_2c + iVar8 * 4 + uVar7 * -2);
          }
          *(undefined4 *)(&stack0xffffffd0 + uVar7 * -2) = uVar6;
          *(undefined4 *)(&stack0xffffffcc + uVar7 * -2) = uVar6;
          pMVar5 = group;
          *(undefined4 *)(&stack0xffffffc8 + uVar7 * -2) =
               *(undefined4 *)((int)aiStack_2c + iVar8 * 4 + iVar3);
          *(MdtContactGroupID_conflict *)(&stack0xffffffc4 + uVar7 * -2) = pMVar5;
          *(undefined4 *)((int)&sStackY_40 + uVar7 * -2) = 0x10d53;
          MdtContactGroupDestroyContact
                    (*(void **)(&stack0xffffffc4 + uVar7 * -2),
                     *(void **)(&stack0xffffffc8 + uVar7 * -2));
          *(undefined4 *)((int)aiStack_2c + iVar8 * 4 + uVar7 * -2) = 1;
          iVar8 = nRows + -1;
          nContacts = nContacts + -1;
          nRows = iVar8;
        }
        else {
          iVar9 = 0;
          if ((*(int *)(*(int *)((int)aiStack_2c + iVar3) + 0x194) == 0) ||
             (*(int *)((int)aiStack_2c + uVar7 * -2) == 1)) {
            do {
              do {
                iVar9 = iVar9 + 1;
                iVar8 = *(int *)((int)aiStack_2c + iVar9 * 4 + iVar3);
              } while (*(int *)(iVar8 + 0x194) == 0);
            } while (*(int *)((int)aiStack_2c + iVar9 * 4 + uVar7 * -2) == 1);
          }
          else {
            iVar8 = *(int *)((int)aiStack_2c + iVar3);
          }
          if (*(int *)(iVar8 + 0x194) == 1) {
            nRows = nRows + -1;
          }
          else {
            nRows = nRows + -2;
          }
          *(undefined4 *)(iVar8 + 0x194) = 0;
          nFrictionContacts = nFrictionContacts + -1;
        }
      } while ((group->head).rowBudget < nRows);
    }
  }
  return;
}


/* ==== MergePartitions ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MergePartitions(MdtLODPartitionData *pdataArray,int p1Ix,int p2Ix)

{
  MdtLODPartitionData *pMVar1;
  MdtLODPartitionData *pMVar2;
  int iVar3;
  MdtBody **ppMVar4;
  MdtBaseConstraint **ppMVar5;
  int iVar6;
  int iVar7;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int newNBod@[???]
                       Unresolved local var: int newNCon@[DW_OP_reg3(EBX)]
                       Unresolved local var: MdtLODPartitionData * p1Data@[DW_OP_reg6(ESI)]
                       Unresolved local var: MdtLODPartitionData * p2Data@[DW_OP_reg7(EDI)] */
  pMVar1 = pdataArray + p1Ix;
  pMVar2 = pdataArray + p2Ix;
  iVar7 = pMVar1->conCount;
  iVar3 = pMVar2->conCount;
  ppMVar4 = (MdtBody **)
            (*_DAT_00012030)(pMVar1->bodyArray,(pMVar2->bodyCount + pMVar1->bodyCount) * 4);
  pMVar1->bodyArray = ppMVar4;
  iVar6 = 0;
  ppMVar5 = (MdtBaseConstraint **)(*_DAT_00012030)(pMVar1->conArray,(iVar3 + iVar7) * 4);
  iVar7 = pMVar2->bodyCount;
  pMVar1->conArray = ppMVar5;
  if (0 < iVar7) {
    do {
      ppMVar4 = pMVar2->bodyArray + iVar6;
      iVar6 = iVar6 + 1;
      pMVar1->bodyArray[pMVar1->bodyCount] = *ppMVar4;
      pMVar1->bodyArray[pMVar1->bodyCount]->LODpartIndex = p1Ix;
      pMVar1->bodyCount = pMVar1->bodyCount + 1;
    } while (iVar6 < pMVar2->bodyCount);
  }
  iVar7 = 0;
  if (0 < pMVar2->conCount) {
    do {
      ppMVar5 = pMVar2->conArray + iVar7;
      iVar7 = iVar7 + 1;
      pMVar1->conArray[pMVar1->conCount] = *ppMVar5;
      (pMVar1->conArray[pMVar1->conCount]->head).LODpartIndex = p1Ix;
      pMVar1->conCount = pMVar1->conCount + 1;
    } while (iVar7 < pMVar2->conCount);
  }
  ppMVar4 = pMVar2->bodyArray;
  pMVar1->rowCount = pMVar1->rowCount + pMVar2->rowCount;
  if (ppMVar4 != (MdtBody **)0x0) {
    (*_qsort)(ppMVar4);
    pMVar2->bodyArray = (MdtBody **)0x0;
  }
  pMVar2->bodyCount = 0;
  if (pMVar2->conArray != (MdtBaseConstraint **)0x0) {
    (*_qsort)(pMVar2->conArray);
    pMVar2->conArray = (MdtBaseConstraint **)0x0;
  }
  pMVar2->conCount = 0;
  pMVar2->rowCount = 0;
  return;
}


/* ==== IncrementConstraintRowBudget ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeBool IncrementConstraintRowBudget
                 (MdtBaseConstraint *con,MdtLODPartitionData *pdataArray,int LODpartIx,
                 MdtPartitionParams *params,MeHeap *q)

{
  MdtLODPartitionData *pMVar1;
  int iVar2;
  void *pvVar3;
  MdtBaseConstraint **ppMVar4;
  
                    /* Unresolved local var: MdtContactGroupID.conflict group@[DW_OP_reg0(EAX)]
                       Unresolved local var: MdtLODPartitionData * pData@[DW_OP_reg3(EBX)] */
  pMVar1 = pdataArray + LODpartIx;
  pvVar3 = MdtConstraintDCastContactGroup(con);
  if (pvVar3 == (void *)0x0) {
    iVar2 = (con->head).maxRows;
    (con->head).rowBudget = iVar2;
    pMVar1->rowCount = pMVar1->rowCount + iVar2;
  }
  else {
    if (params->maxMatrixSize <= pMVar1->rowCount) {
      return 0;
    }
    iVar2 = (con->head).rowBudget;
    if (iVar2 == 0) {
      (con->head).importance = (con->head).importance - (params->lodParams).zeroRowBonus;
    }
    (con->head).rowBudget = iVar2 + 1;
    (con->head).importance = (con->head).importance - (params->lodParams).rowCountBias;
    pMVar1->rowCount = pMVar1->rowCount + 1;
    if ((con->head).rowBudget < (con->head).maxRows) {
      MeHeapPush(q,con);
    }
  }
  if ((con->head).LODpartIndex == -1) {
                    /* Unresolved local var: MdtLODPartitionData * pData@[DW_OP_reg3(EBX)] */
    ppMVar4 = (MdtBaseConstraint **)
              (*_DAT_00012030)(pdataArray[LODpartIx].conArray,pdataArray[LODpartIx].conCount * 4 + 4
                               ,pdataArray,pdataArray);
    pdataArray[LODpartIx].conArray = ppMVar4;
    (con->head).LODpartIndex = LODpartIx;
    pdataArray[LODpartIx].conArray[pdataArray[LODpartIx].conCount] = con;
    pdataArray[LODpartIx].conCount = pdataArray[LODpartIx].conCount + 1;
  }
  return 1;
}


