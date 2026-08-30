/* ==== MdtUpdatePartitions ==== */

/* WARNING: Unknown calling convention */

void MdtUpdatePartitions(MeDict *enabledBodyDict,MdtPartitionOutput *po,MdtPartitionEndCB pcb,
                        void *pcbdata)

{
  MdtBaseConstraint **ppMVar1;
  MdtBody **ppMVar2;
  MdtBody *pMVar3;
  MdtBaseConstraint *pMVar4;
  MdtPartitionInfo *pMVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  MeDictNode *pMVar9;
  int iVar10;
  MeDictNode *pMVar11;
  int iVar12;
  uint uVar13;
  MeDict *pMVar14;
  int iVar15;
  int aiStack_50 [5];
  MdtBody *local_3c;
  MdtBody *currentBody;
  MeDictNode *node;
  MeDict *dict;
  int addNext;
  int exploreNext;
  int numAddedConstraints;
  MdtBaseConstraint **addedConstraints;
  int numAddedBodies;
  MdtBody **addedBodies;
  MeDictNode *rootNode;
  
                    /* Unresolved local var: MdtBody * rootBody@[DW_OP_reg3(EBX)]
                       Unresolved local var: int i@[DW_OP_reg2(EDX)]
                       Unresolved local var: MdtBaseConstraint * cp@[DW_OP_reg3(EBX)]
                       Unresolved local var: MdtBody * bp@[???] */
  numAddedBodies = 0;
  iVar15 = -(po->maxBodies * 4 + 0xfU & 0xfffffff0);
  addedBodies = (MdtBody **)((int)&local_3c + iVar15);
  numAddedConstraints = 0;
  iVar8 = -(po->maxConstraints * 4 + 0xfU & 0xfffffff0);
  addedConstraints = (MdtBaseConstraint **)((int)&local_3c + iVar8 + iVar15);
  po->nPartitions = 0;
  po->totalBodies = 0;
  po->totalConstraints = 0;
  (po->overallInfo).contactCount = 0;
  (po->overallInfo).jointCount = 0;
  (po->overallInfo).rowCount = 0;
  *(MeDict **)((int)aiStack_50 + iVar8 + iVar15 + 4) = enabledBodyDict;
  *(undefined4 *)((int)aiStack_50 + iVar8 + iVar15) = 0x10074;
  pMVar9 = (MeDictNode *)MeDictFirst();
  rootNode = pMVar9;
  while (pMVar9 != (MeDictNode *)0x0) {
    pMVar3 = rootNode->data;
    po->bodiesSize[po->nPartitions] = 0;
    po->constraintsSize[po->nPartitions] = 0;
    pMVar5 = po->info + po->nPartitions;
    pMVar5->contactCount = 0;
    pMVar5->jointCount = 0;
    pMVar5->rowCount = 0;
    po->bodiesStart[po->nPartitions] = po->totalBodies;
    po->constraintsStart[po->nPartitions] = po->totalConstraints;
    iVar10 = po->bodiesStart[po->nPartitions];
    exploreNext = iVar10;
    po->bodies[iVar10] = pMVar3;
    addNext = iVar10 + 1;
    local_3c = (MdtBody *)(iVar10 + 1);
    addedBodies[numAddedBodies] = pMVar3;
    numAddedBodies = numAddedBodies + 1;
    pMVar3->arrayIdPartition = po->bodiesSize[po->nPartitions];
    pMVar3->arrayIdWorld = po->bodiesSize[po->nPartitions] + po->bodiesStart[po->nPartitions];
    iVar12 = po->nPartitions;
    pMVar3->flags = pMVar3->flags | 2;
    pMVar3->partitionIndex = iVar12;
    po->totalBodies = po->totalBodies + 1;
    po->bodiesSize[po->nPartitions] = po->bodiesSize[po->nPartitions] + 1;
    iVar12 = po->nPartitions;
    if (iVar10 < po->bodiesSize[iVar12] + po->bodiesStart[iVar12]) {
      while( true ) {
        currentBody = po->bodies[exploreNext];
        exploreNext = (int)local_3c;
        pMVar14 = &currentBody->constraintDict;
        *(MeDict **)((int)aiStack_50 + iVar8 + iVar15 + 4) = pMVar14;
        dict = pMVar14;
        *(undefined4 *)((int)aiStack_50 + iVar8 + iVar15) = 0x1019f;
        pMVar9 = (MeDictNode *)MeDictFirst();
        while (node = pMVar9, pMVar9 != (MeDictNode *)0x0) {
          pMVar4 = pMVar9->data;
          if (((pMVar4->head).flags & 2) == 0) {
                    /* Unresolved local var: int rows@[???]
                       Unresolved local var: int padding@[???]
                       Unresolved local var: MdtContactGroupID.conflict group@[DW_OP_reg7(EDI)] */
            po->constraints[po->totalConstraints] = pMVar4;
            addedConstraints[numAddedConstraints] = pMVar4;
            numAddedConstraints = numAddedConstraints + 1;
            po->totalConstraints = po->totalConstraints + 1;
            po->constraintsSize[po->nPartitions] = po->constraintsSize[po->nPartitions] + 1;
            iVar10 = po->nPartitions;
            pMVar5 = po->info;
            *(MdtBaseConstraint **)((int)aiStack_50 + iVar8 + iVar15 + 4) = pMVar4;
            pMVar5 = pMVar5 + iVar10;
            *(undefined4 *)((int)aiStack_50 + iVar8 + iVar15) = 0x104c9;
            iVar10 = MdtConstraintDCastContactGroup();
            *(MdtBaseConstraint **)((int)aiStack_50 + iVar8 + iVar15 + 4) = pMVar4;
            *(undefined4 *)((int)aiStack_50 + iVar8 + iVar15) = 0x104d3;
            pMVar9 = (MeDictNode *)MdtConstraintGetRowCount();
            pMVar5->rowCount = (int)&pMVar9->left + pMVar5->rowCount;
            if (iVar10 == 0) {
              pMVar9 = (MeDictNode *)(pMVar5->jointCount + 1);
              pMVar5->jointCount = (int)pMVar9;
            }
            else {
              pMVar5->contactCount = pMVar5->contactCount + *(int *)(iVar10 + 0x160);
            }
            pMVar3 = (pMVar4->head).mdtbody[0];
            (pMVar4->head).flags = (pMVar4->head).flags | 2;
            if (((pMVar3 != (MdtBody *)0x0) && (pMVar3 != currentBody)) &&
               ((pMVar3->flags & 2) == 0)) {
              po->bodies[addNext] = pMVar3;
              addedBodies[numAddedBodies] = pMVar3;
              numAddedBodies = numAddedBodies + 1;
              pMVar3->arrayIdPartition = po->bodiesSize[po->nPartitions];
              pMVar3->arrayIdWorld =
                   po->bodiesSize[po->nPartitions] + po->bodiesStart[po->nPartitions];
              iVar10 = po->nPartitions;
              pMVar3->flags = pMVar3->flags | 2;
              pMVar3->partitionIndex = iVar10;
              pMVar9 = (MeDictNode *)po->nPartitions;
              po->totalBodies = po->totalBodies + 1;
              po->bodiesSize[(int)pMVar9] = po->bodiesSize[(int)pMVar9] + 1;
              addNext = addNext + 1;
            }
            pMVar3 = (pMVar4->head).mdtbody[1];
            if (((pMVar3 != (MdtBody *)0x0) && (pMVar3 != currentBody)) &&
               ((pMVar3->flags & 2) == 0)) {
              po->bodies[addNext] = pMVar3;
              addedBodies[numAddedBodies] = pMVar3;
              numAddedBodies = numAddedBodies + 1;
              pMVar3->arrayIdPartition = po->bodiesSize[po->nPartitions];
              pMVar9 = (MeDictNode *)
                       (po->bodiesSize[po->nPartitions] + po->bodiesStart[po->nPartitions]);
              pMVar3->arrayIdWorld = (int)pMVar9;
              iVar10 = po->nPartitions;
              pMVar3->flags = pMVar3->flags | 2;
              pMVar3->partitionIndex = iVar10;
              po->totalBodies = po->totalBodies + 1;
              po->bodiesSize[po->nPartitions] = po->bodiesSize[po->nPartitions] + 1;
              addNext = addNext + 1;
            }
          }
          *(MeDictNode **)((int)aiStack_50 + iVar8 + iVar15 + 0x10) = pMVar9;
          *(MeDictNode **)((int)aiStack_50 + iVar8 + iVar15 + 0xc) = pMVar9;
          *(MeDictNode **)((int)aiStack_50 + iVar8 + iVar15 + 8) = node;
          *(MeDict **)((int)aiStack_50 + iVar8 + iVar15 + 4) = dict;
          *(undefined4 *)((int)aiStack_50 + iVar8 + iVar15) = 65999;
          pMVar9 = (MeDictNode *)MeDictNext();
        }
        *(MdtBody **)((int)aiStack_50 + iVar8 + iVar15 + 4) = currentBody;
        *(undefined4 *)((int)aiStack_50 + iVar8 + iVar15) = 0x101e5;
        iVar10 = MdtBodyIsEnabled();
        if (iVar10 == 0) {
          *(MdtBody **)((int)aiStack_50 + iVar8 + iVar15 + 4) = currentBody;
          *(undefined4 *)((int)aiStack_50 + iVar8 + iVar15) = 0x1046e;
          MdtBodyEnable();
          currentBody->flags = currentBody->flags | 4;
        }
        else {
          currentBody->flags = currentBody->flags & 0xfffffffb;
        }
        iVar12 = po->nPartitions;
        if (po->bodiesSize[iVar12] + po->bodiesStart[iVar12] <= (int)local_3c) break;
        local_3c = (MdtBody *)((int)&(local_3c->keaBody).tag + 1);
      }
    }
    uVar13 = po->info[iVar12].rowCount;
    if ((uVar13 & 3) != 0) {
      if ((int)uVar13 < 0) {
        uVar13 = uVar13 + 3;
      }
      uVar13 = (uVar13 & 0xfffffffc) + 4;
    }
    po->info[iVar12].rowCount = uVar13;
    iVar10 = po->nPartitions;
    pMVar5 = po->info;
    pMVar11 = (MeDictNode *)(iVar10 * 0xc);
    iVar12 = pMVar5[iVar10].jointCount;
    (po->overallInfo).rowCount = (po->overallInfo).rowCount + pMVar5[iVar10].rowCount;
    iVar6 = pMVar5[iVar10].contactCount;
    iVar7 = (po->overallInfo).contactCount;
    (po->overallInfo).jointCount = (po->overallInfo).jointCount + iVar12;
    po->nPartitions = iVar10 + 1;
    (po->overallInfo).contactCount = iVar7 + iVar6;
    pMVar9 = rootNode;
    while (pMVar9 != (MeDictNode *)0x0) {
                    /* Unresolved local var: MdtBody * b@[DW_OP_reg0(EAX)] */
      if ((*(byte *)((int)pMVar9->data + 0x1ec) & 2) == 0) goto LAB_000102b7;
      *(MeDictNode **)((int)aiStack_50 + iVar8 + iVar15 + 0x10) = pMVar11;
      *(MeDictNode **)((int)aiStack_50 + iVar8 + iVar15 + 0xc) = pMVar11;
      *(MeDictNode **)((int)aiStack_50 + iVar8 + iVar15 + 8) = pMVar9;
      *(MeDict **)((int)aiStack_50 + iVar8 + iVar15 + 4) = enabledBodyDict;
      *(undefined4 *)((int)aiStack_50 + iVar8 + iVar15) = 0x102ac;
      pMVar11 = (MeDictNode *)MeDictNext();
      pMVar9 = pMVar11;
    }
    pMVar9 = (MeDictNode *)0x0;
LAB_000102b7:
    rootNode = pMVar9;
    if (pcb != (MdtPartitionEndCB)0x0) {
      *(MeDictNode **)((int)aiStack_50 + iVar8 + iVar15 + 0x10) = pMVar11;
      *(MeDictNode **)((int)aiStack_50 + iVar8 + iVar15 + 0xc) = pMVar11;
      *(void **)((int)aiStack_50 + iVar8 + iVar15 + 8) = pcbdata;
      *(MdtPartitionOutput **)((int)aiStack_50 + iVar8 + iVar15 + 4) = po;
      *(undefined4 *)((int)aiStack_50 + iVar8 + iVar15) = 0x102ce;
      (*pcb)(*(MdtPartitionOutput **)((int)aiStack_50 + iVar8 + iVar15 + 4),
             *(void **)((int)aiStack_50 + iVar8 + iVar15 + 8));
    }
  }
  iVar15 = 0;
  if (0 < numAddedConstraints) {
    uVar13 = numAddedConstraints & 3;
    if (numAddedConstraints < 2) {
LAB_000102f0:
      ppMVar1 = addedConstraints + iVar15;
      iVar15 = iVar15 + 1;
      ((*ppMVar1)->head).flags = ((*ppMVar1)->head).flags & 0xfffffffd;
      if (numAddedConstraints <= iVar15) goto LAB_00010343;
    }
    else if (uVar13 != 0) {
      if (1 < uVar13) {
        if (2 < uVar13) {
          ((*addedConstraints)->head).flags = ((*addedConstraints)->head).flags & 0xfffffffd;
        }
        iVar15 = (2 < uVar13) + 1;
        (addedConstraints[2 < uVar13]->head).flags =
             (addedConstraints[2 < uVar13]->head).flags & 0xfffffffd;
      }
      goto LAB_000102f0;
    }
    do {
      (addedConstraints[iVar15]->head).flags = (addedConstraints[iVar15]->head).flags & 0xfffffffd;
      (addedConstraints[iVar15 + 1]->head).flags =
           (addedConstraints[iVar15 + 1]->head).flags & 0xfffffffd;
      (addedConstraints[iVar15 + 2]->head).flags =
           (addedConstraints[iVar15 + 2]->head).flags & 0xfffffffd;
      iVar8 = iVar15 + 3;
      iVar15 = iVar15 + 4;
      (addedConstraints[iVar8]->head).flags = (addedConstraints[iVar8]->head).flags & 0xfffffffd;
    } while (iVar15 < numAddedConstraints);
  }
LAB_00010343:
  iVar15 = 0;
  if (numAddedBodies < 1) {
    return;
  }
  uVar13 = numAddedBodies & 3;
  if (1 < numAddedBodies) {
    if (uVar13 == 0) goto LAB_00010371;
    if (1 < uVar13) {
      if (2 < uVar13) {
        (*addedBodies)->flags = (*addedBodies)->flags & 0xfffffffd;
      }
      iVar15 = (2 < uVar13) + 1;
      addedBodies[2 < uVar13]->flags = addedBodies[2 < uVar13]->flags & 0xfffffffd;
    }
  }
  ppMVar2 = addedBodies + iVar15;
  iVar15 = iVar15 + 1;
  (*ppMVar2)->flags = (*ppMVar2)->flags & 0xfffffffd;
  if (numAddedBodies <= iVar15) {
    return;
  }
LAB_00010371:
  do {
    addedBodies[iVar15]->flags = addedBodies[iVar15]->flags & 0xfffffffd;
    addedBodies[iVar15 + 1]->flags = addedBodies[iVar15 + 1]->flags & 0xfffffffd;
    addedBodies[iVar15 + 2]->flags = addedBodies[iVar15 + 2]->flags & 0xfffffffd;
    iVar8 = iVar15 + 3;
    iVar15 = iVar15 + 4;
    addedBodies[iVar8]->flags = addedBodies[iVar8]->flags & 0xfffffffd;
  } while (iVar15 < numAddedBodies);
  return;
}


/* ==== FindNextUnadded ==== */

MeDictNode * __regparm1 FindNextUnadded(MeDict *dict,MeDictNode *node)

{
  MeDict *in_stack_00000008;
  MeDict *pMVar1;
  
  pMVar1 = dict;
  while( true ) {
    if (in_stack_00000008 == (MeDict *)0x0) {
      return (MeDictNode *)0x0;
    }
                    /* Unresolved local var: MdtBody * b@[DW_OP_reg0(EAX)] */
    if ((*(byte *)((int)(in_stack_00000008->nilnode).data + 0x1ec) & 2) == 0) break;
    dict = (MeDict *)MeDictNext(node,in_stack_00000008,dict,dict,pMVar1);
    in_stack_00000008 = dict;
  }
  return &in_stack_00000008->nilnode;
}


/* ==== MdtPartOutCreateFromChunk ==== */

MdtPartitionOutput * MdtPartOutCreateFromChunk(MeChunk *chunk,int maxBodies,int maxConstraints)

{
  MdtPartitionOutput *pMVar1;
  MdtBaseConstraint ***pppMVar2;
  
                    /* Unresolved local var: MdtPartitionOutput * po@[DW_OP_reg0(EAX)]
                       Unresolved local var: int maxPartitions@[???]
                       Unresolved local var: int structSize@[???]
                       Unresolved local var: int bStartSize@[???]
                       Unresolved local var: int bSizeSize@[???]
                       Unresolved local var: int bSize@[???]
                       Unresolved local var: int cStartSize@[???]
                       Unresolved local var: int cSizeSize@[???]
                       Unresolved local var: int cSize@[???]
                       Unresolved local var: int pInfoSize@[???]
                       Unresolved local var: int totalSize@[DW_OP_reg0(EAX)] */
  pMVar1 = (MdtPartitionOutput *)MeChunkGetMem(chunk,maxBodies * 0x20 + 0x40 + maxConstraints * 4);
  pMVar1->maxPartitions = maxBodies;
  pMVar1->bodiesStart = (int *)(pMVar1 + 1);
  pppMVar2 = &pMVar1[1].constraints + maxBodies;
  pMVar1->bodiesSize = (int *)pppMVar2;
  pppMVar2 = pppMVar2 + maxBodies;
  pMVar1->bodies = (MdtBody **)pppMVar2;
  pppMVar2 = pppMVar2 + maxBodies;
  pMVar1->constraintsStart = (int *)pppMVar2;
  pppMVar2 = pppMVar2 + maxBodies;
  pMVar1->constraintsSize = (int *)pppMVar2;
  pMVar1->info = (MdtPartitionInfo *)(pppMVar2 + maxBodies);
  pMVar1->constraints =
       (MdtBaseConstraint **)((MdtPartitionInfo *)(pppMVar2 + maxBodies) + maxBodies);
  pMVar1->maxBodies = maxBodies;
  pMVar1->maxConstraints = maxConstraints;
  pMVar1->nPartitions = 0;
  pMVar1->totalBodies = 0;
  pMVar1->totalConstraints = 0;
  return pMVar1;
}


/* ==== MdtPartInfoReset ==== */

void MdtPartInfoReset(MdtPartitionInfo *info)

{
  info->contactCount = 0;
  info->jointCount = 0;
  info->rowCount = 0;
  return;
}


/* ==== MdtPartInfoAddConstraint ==== */

void MdtPartInfoAddConstraint(MdtPartitionInfo *info,MdtBaseConstraint *c)

{
  int iVar1;
  int iVar2;
  
                    /* Unresolved local var: int rows@[???]
                       Unresolved local var: int padding@[???]
                       Unresolved local var: MdtContactGroupID.conflict group@[DW_OP_reg7(EDI)] */
  iVar1 = MdtConstraintDCastContactGroup(c);
  iVar2 = MdtConstraintGetRowCount(c);
  info->rowCount = info->rowCount + iVar2;
  if (iVar1 == 0) {
    info->jointCount = info->jointCount + 1;
  }
  else {
    info->contactCount = info->contactCount + *(int *)(iVar1 + 0x160);
  }
  return;
}


