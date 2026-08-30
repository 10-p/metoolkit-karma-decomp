/* ==== MeProfileStopTiming ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeProfileStopTiming(void)

{
  MeProfileTimer *pMVar1;
  MeProfileFrameData *pMVar2;
  MeProfileFrameSectionInfo *pMVar3;
  MeProfileTimer *pMVar4;
  MeProfileFrameData *pMVar5;
  MeProfileFrameSectionInfo *pMVar6;
  
                    /* Unresolved local var: MeProfileTimer * itemt@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeProfileTimer * nextt@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeProfileFrameData * itemfd@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeProfileFrameData * nextfd@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeProfileFrameSectionInfo * itemfsi@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeProfileFrameSectionInfo * nextfsi@[DW_OP_reg3(EBX)]
                        */
  (*_MeProfileStopHardwareTimer)(HWTMode);
  pMVar4 = firstt;
  while (pMVar5 = firstfd, pMVar4 != (MeProfileTimer *)0x0) {
    pMVar1 = pMVar4->next;
    (*_MeProfileStopHardwareTimer)(pMVar4);
    pMVar4 = pMVar1;
  }
  while (pMVar5 != (MeProfileFrameData *)0x0) {
    pMVar2 = pMVar5->next;
    pMVar6 = pMVar5->firstfsi;
    while (pMVar6 != (MeProfileFrameSectionInfo *)0x0) {
      pMVar3 = pMVar6->next;
      (*_MeProfileStopHardwareTimer)(pMVar6);
      pMVar6 = pMVar3;
    }
    (*_MeProfileStopHardwareTimer)(pMVar5);
    pMVar5 = pMVar2;
  }
  MeProfileDestroyHardwareTimer();
  timingRunning = '\0';
  return;
}


/* ==== MeProfileEndFrame ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeProfileEndFrame(void)

{
  MeI64 *pMVar1;
  int *piVar2;
  MeProfileTimerResult *pMVar3;
  MeU64 *pMVar4;
  MeProfileTimer *pMVar5;
  MeProfileFrameSectionInfo *pMVar6;
  undefined4 uVar7;
  MeProfileFrameData *pMVar8;
  MeI64 MVar9;
  MeU64 MVar10;
  int iVar11;
  MeProfileFrameSectionInfo *pMVar12;
  uint uVar13;
  int iVar14;
  int iVar15;
  MeProfileFrameSectionInfo *unaff_EDI;
  bool bVar16;
  bool bVar17;
  MeBool foundIt;
  MeProfileFrameData *thisfd;
  MeProfileTimerResult thist;
  
                    /* Unresolved local var: MeProfileTimer * thistt@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeProfileTimer * itemt@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeProfileFrameSectionInfo * thisfsi@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeProfileFrameSectionInfo * itemfsi@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeProfileFrameSectionInfo * nextfsi@[DW_OP_reg3(EBX)]
                        */
  MeProfileGetTimerValue(&thist);
  pMVar8 = firstfd;
  for (pMVar5 = firstt; firstfd = pMVar8, pMVar5 != (MeProfileTimer *)0x0; pMVar5 = pMVar5->next) {
    if (pMVar5->isRunning != '\0') {
      MeProfileEndSectionFn(pMVar5->codeSection);
      MeWarning(3,&DAT_00013585);
    }
    pMVar8 = firstfd;
  }
  if (logging == kMeProfileLogAll) {
    thisfd = (MeProfileFrameData *)(*_MeMemoryAPI)(0x24);
    if (firstfd == (MeProfileFrameData *)0x0) {
      MeProfileEndFrame::frame = 0;
      firstfd = thisfd;
    }
    else {
      MeProfileEndFrame::tailfd->next = thisfd;
    }
  }
  else if (logging == kMeProfileDontLog) {
    thisfd = pMVar8;
    if (pMVar8 == (MeProfileFrameData *)0x0) {
      thisfd = (MeProfileFrameData *)(*_MeMemoryAPI)(0x24);
      thisfd->next = (MeProfileFrameData *)0x0;
      thisfd->firstfsi = (MeProfileFrameSectionInfo *)0x0;
    }
    bVar16 = (uint)thist.cpuCycles < (uint)frameTime.cpuCycles;
    iVar11 = thist.cpuCycles._4_4_ - frameTime.cpuCycles._4_4_;
    bVar17 = (uint)thist.count0 < (uint)frameTime.count0;
    iVar14 = (uint)thist.count0 - (uint)frameTime.count0;
    firstfd = thisfd;
    *(uint *)&(thisfd->timings).cpuCycles = (uint)thist.cpuCycles - (uint)frameTime.cpuCycles;
    *(uint *)((int)&(thisfd->timings).cpuCycles + 4) = iVar11 - (uint)bVar16;
    iVar11 = thist.count0._4_4_ - frameTime.count0._4_4_;
    bVar16 = (uint)thist.count1 < (uint)frameTime.count1;
    iVar15 = (uint)thist.count1 - (uint)frameTime.count1;
    *(int *)&(thisfd->timings).count0 = iVar14;
    iVar14 = thist.count1._4_4_ - frameTime.count1._4_4_;
    *(uint *)((int)&(thisfd->timings).count0 + 4) = iVar11 - (uint)bVar17;
    *(int *)&(thisfd->timings).count1 = iVar15;
    *(uint *)((int)&(thisfd->timings).count1 + 4) = iVar14 - (uint)bVar16;
  }
  else if (logging == kMeProfileLogTotals) {
    if (pMVar8 == (MeProfileFrameData *)0x0) {
      thisfd = (MeProfileFrameData *)(*_MeMemoryAPI)(0x24);
      thisfd->next = (MeProfileFrameData *)0x0;
      thisfd->firstfsi = (MeProfileFrameSectionInfo *)0x0;
      *(undefined4 *)&(thisfd->timings).cpuCycles = 0;
      *(undefined4 *)((int)&(thisfd->timings).cpuCycles + 4) = 0;
      *(undefined4 *)&(thisfd->timings).count0 = 0;
      *(undefined4 *)((int)&(thisfd->timings).count0 + 4) = 0;
      *(undefined4 *)&(thisfd->timings).count1 = 0;
      *(undefined4 *)((int)&(thisfd->timings).count1 + 4) = 0;
      firstfd = thisfd;
    }
    else {
      bVar16 = (uint)thist.cpuCycles < (uint)frameTime.cpuCycles;
      uVar13 = (uint)thist.cpuCycles - (uint)frameTime.cpuCycles;
      iVar11 = thist.cpuCycles._4_4_ - frameTime.cpuCycles._4_4_;
      pMVar3 = &pMVar8->timings;
      MVar10 = pMVar3->cpuCycles;
      *(uint *)&pMVar3->cpuCycles = (int)pMVar3->cpuCycles + uVar13;
      piVar2 = (int *)((int)&(pMVar8->timings).cpuCycles + 4);
      *piVar2 = *piVar2 + (iVar11 - (uint)bVar16) + (uint)CARRY4((uint)MVar10,uVar13);
      bVar16 = (uint)thist.count0 < (uint)frameTime.count0;
      uVar13 = (uint)thist.count0 - (uint)frameTime.count0;
      iVar11 = thist.count0._4_4_ - frameTime.count0._4_4_;
      pMVar4 = &(pMVar8->timings).count0;
      MVar10 = *pMVar4;
      *(uint *)pMVar4 = (int)*pMVar4 + uVar13;
      piVar2 = (int *)((int)&(pMVar8->timings).count0 + 4);
      *piVar2 = *piVar2 + (iVar11 - (uint)bVar16) + (uint)CARRY4((uint)MVar10,uVar13);
      bVar16 = (uint)thist.count1 < (uint)frameTime.count1;
      uVar13 = (uint)thist.count1 - (uint)frameTime.count1;
      iVar11 = thist.count1._4_4_ - frameTime.count1._4_4_;
      pMVar4 = &(pMVar8->timings).count1;
      MVar10 = *pMVar4;
      *(uint *)pMVar4 = (int)*pMVar4 + uVar13;
      piVar2 = (int *)((int)&(pMVar8->timings).count1 + 4);
      *piVar2 = *piVar2 + (iVar11 - (uint)bVar16) + (uint)CARRY4((uint)MVar10,uVar13);
      thisfd = pMVar8;
    }
  }
  thisfd->frameNumber = MeProfileEndFrame::frame;
  MeProfileEndFrame::frame = MeProfileEndFrame::frame + 1;
  pMVar5 = firstt;
  if (logging == kMeProfileLogAll) {
    for (; pMVar5 != (MeProfileTimer *)0x0; pMVar5 = pMVar5->next) {
      pMVar12 = (MeProfileFrameSectionInfo *)(*_MeMemoryAPI)(0x24);
      pMVar12->next = (MeProfileFrameSectionInfo *)0x0;
      pMVar12->codeSection = pMVar5->codeSection;
      pMVar12->timesCalled = pMVar5->timesCalled;
      MVar9 = pMVar5->cpuCycles;
      *(undefined4 *)((int)&pMVar12->cpuCycles + 4) = *(undefined4 *)((int)&pMVar5->cpuCycles + 4);
      *(int *)&pMVar12->cpuCycles = (int)MVar9;
      uVar7 = *(undefined4 *)((int)&pMVar5->count0 + 4);
      *(int *)&pMVar12->count0 = (int)pMVar5->count0;
      *(undefined4 *)((int)&pMVar12->count0 + 4) = uVar7;
      uVar7 = *(undefined4 *)((int)&pMVar5->count1 + 4);
      *(int *)&pMVar12->count1 = (int)pMVar5->count1;
      *(undefined4 *)((int)&pMVar12->count1 + 4) = uVar7;
      if (thisfd->firstfsi == (MeProfileFrameSectionInfo *)0x0) {
        thisfd->firstfsi = pMVar12;
      }
      else {
        MeProfileEndFrame::tailfsi->next = pMVar12;
      }
      MeProfileEndFrame::tailfsi = pMVar12;
    }
  }
  else if (logging == kMeProfileDontLog) {
    pMVar12 = thisfd->firstfsi;
    while (pMVar12 != (MeProfileFrameSectionInfo *)0x0) {
      pMVar6 = pMVar12->next;
      (*_MeProfileStopHardwareTimer)(pMVar12);
      pMVar12 = pMVar6;
    }
    thisfd->firstfsi = (MeProfileFrameSectionInfo *)0x0;
    for (pMVar5 = firstt; pMVar5 != (MeProfileTimer *)0x0; pMVar5 = pMVar5->next) {
      pMVar12 = (MeProfileFrameSectionInfo *)(*_MeMemoryAPI)(0x24);
      pMVar12->next = (MeProfileFrameSectionInfo *)0x0;
      pMVar12->codeSection = pMVar5->codeSection;
      pMVar12->timesCalled = pMVar5->timesCalled;
      MVar9 = pMVar5->cpuCycles;
      *(undefined4 *)((int)&pMVar12->cpuCycles + 4) = *(undefined4 *)((int)&pMVar5->cpuCycles + 4);
      *(int *)&pMVar12->cpuCycles = (int)MVar9;
      MVar9 = pMVar5->count0;
      *(undefined4 *)((int)&pMVar12->count0 + 4) = *(undefined4 *)((int)&pMVar5->count0 + 4);
      *(int *)&pMVar12->count0 = (int)MVar9;
      uVar7 = *(undefined4 *)((int)&pMVar5->count1 + 4);
      *(int *)&pMVar12->count1 = (int)pMVar5->count1;
      *(undefined4 *)((int)&pMVar12->count1 + 4) = uVar7;
      if (thisfd->firstfsi == (MeProfileFrameSectionInfo *)0x0) {
        thisfd->firstfsi = pMVar12;
      }
      else {
        MeProfileEndFrame::tailfsi->next = pMVar12;
      }
      MeProfileEndFrame::tailfsi = pMVar12;
    }
  }
  else if (logging == kMeProfileLogTotals) {
    for (; pMVar5 != (MeProfileTimer *)0x0; pMVar5 = pMVar5->next) {
      bVar16 = false;
      pMVar12 = thisfd->firstfsi;
      if (pMVar12 == (MeProfileFrameSectionInfo *)0x0) {
LAB_000101b7:
        if (bVar16) goto LAB_0001017d;
        unaff_EDI = (MeProfileFrameSectionInfo *)(*_MeMemoryAPI)(0x24);
        unaff_EDI->next = (MeProfileFrameSectionInfo *)0x0;
        unaff_EDI->codeSection = pMVar5->codeSection;
        unaff_EDI->timesCalled = pMVar5->timesCalled;
        MVar9 = pMVar5->cpuCycles;
        *(undefined4 *)((int)&unaff_EDI->cpuCycles + 4) =
             *(undefined4 *)((int)&pMVar5->cpuCycles + 4);
        *(int *)&unaff_EDI->cpuCycles = (int)MVar9;
        MVar9 = pMVar5->count0;
        *(undefined4 *)((int)&unaff_EDI->count0 + 4) = *(undefined4 *)((int)&pMVar5->count0 + 4);
        *(int *)&unaff_EDI->count0 = (int)MVar9;
        MVar9 = pMVar5->count1;
        *(undefined4 *)((int)&unaff_EDI->count1 + 4) = *(undefined4 *)((int)&pMVar5->count1 + 4);
        *(int *)&unaff_EDI->count1 = (int)MVar9;
        if (thisfd->firstfsi == (MeProfileFrameSectionInfo *)0x0) {
          thisfd->firstfsi = unaff_EDI;
          MeProfileEndFrame::tailfsi = unaff_EDI;
        }
        else {
          MeProfileEndFrame::tailfsi->next = unaff_EDI;
          MeProfileEndFrame::tailfsi = unaff_EDI;
        }
      }
      else {
        do {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
          iVar11 = strcmp(pMVar5->codeSection,pMVar12->codeSection);
          if (iVar11 == 0) {
            bVar16 = true;
            unaff_EDI = pMVar12;
          }
          pMVar12 = pMVar12->next;
          if (pMVar12 == (MeProfileFrameSectionInfo *)0x0) goto LAB_000101b7;
        } while (!bVar16);
LAB_0001017d:
        unaff_EDI->timesCalled = unaff_EDI->timesCalled + pMVar5->timesCalled;
        uVar13 = (uint)pMVar5->cpuCycles;
        iVar11 = *(int *)((int)&pMVar5->cpuCycles + 4);
        pMVar1 = &unaff_EDI->cpuCycles;
        MVar9 = *pMVar1;
        *(uint *)pMVar1 = (int)*pMVar1 + uVar13;
        piVar2 = (int *)((int)&unaff_EDI->cpuCycles + 4);
        *piVar2 = *piVar2 + iVar11 + (uint)CARRY4((uint)MVar9,uVar13);
        uVar13 = (uint)pMVar5->count0;
        iVar11 = *(int *)((int)&pMVar5->count0 + 4);
        pMVar1 = &unaff_EDI->count0;
        MVar9 = *pMVar1;
        *(uint *)pMVar1 = (int)*pMVar1 + uVar13;
        piVar2 = (int *)((int)&unaff_EDI->count0 + 4);
        *piVar2 = *piVar2 + iVar11 + (uint)CARRY4((uint)MVar9,uVar13);
        uVar13 = (uint)pMVar5->count1;
        iVar11 = *(int *)((int)&pMVar5->count1 + 4);
        pMVar1 = &unaff_EDI->count1;
        MVar9 = *pMVar1;
        *(uint *)pMVar1 = (int)*pMVar1 + uVar13;
        piVar2 = (int *)((int)&unaff_EDI->count1 + 4);
        *piVar2 = *piVar2 + iVar11 + (uint)CARRY4((uint)MVar9,uVar13);
      }
    }
  }
  MeProfileEndFrame::tailfd = thisfd;
  MeProfileStopHardwareTimer();
  insideFrame = '\0';
  return;
}


/* ==== MeProfileStartSectionFn ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeProfileStartSectionFn(char *codeSection,uchar autoStop)

{
  bool bVar1;
  MeProfileTimer *pMVar2;
  int iVar3;
  MeProfileTimer *unaff_ESI;
  MeProfileTimer *unaff_EDI;
  MeBool foundIt;
  uchar autoStop_local;
  MeProfileTimerResult result;
  
                    /* Unresolved local var: MeProfileTimer * thistt@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeProfileTimer * itemt@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeProfileTimer * prevt@[DW_OP_reg7(EDI)] */
  bVar1 = false;
  pMVar2 = firstt;
  if (timingRunning != '\0') {
    for (; pMVar2 != (MeProfileTimer *)0x0; pMVar2 = pMVar2->next) {
      if ((pMVar2->autoStop != '\0') && (pMVar2->isRunning != '\0')) {
        MeProfileEndSectionFn(pMVar2->codeSection);
      }
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
      iVar3 = strcmp(pMVar2->codeSection,codeSection);
      if (iVar3 == 0) {
        bVar1 = true;
        unaff_ESI = pMVar2;
      }
      unaff_EDI = pMVar2;
    }
    if (!bVar1) {
      unaff_ESI = (MeProfileTimer *)(*_MeMemoryAPI)(0x40);
      unaff_ESI->next = (MeProfileTimer *)0x0;
      unaff_ESI->codeSection = (char *)0x0;
      unaff_ESI->timesCalled = 0;
      *(undefined4 *)&unaff_ESI->cpuCycles = 0;
      *(undefined4 *)((int)&unaff_ESI->cpuCycles + 4) = 0;
      *(undefined4 *)&unaff_ESI->count0 = 0;
      *(undefined4 *)((int)&unaff_ESI->count0 + 4) = 0;
      *(undefined4 *)&unaff_ESI->count1 = 0;
      *(undefined4 *)((int)&unaff_ESI->count1 + 4) = 0;
      unaff_ESI->autoStop = '\0';
      unaff_ESI->isRunning = '\0';
      *(undefined4 *)&unaff_ESI->cpuCycleStartValue = 0;
      *(undefined4 *)((int)&unaff_ESI->cpuCycleStartValue + 4) = 0;
      *(undefined4 *)&unaff_ESI->count0StartValue = 0;
      *(undefined4 *)((int)&unaff_ESI->count0StartValue + 4) = 0;
      *(undefined4 *)&unaff_ESI->count1StartValue = 0;
      *(undefined4 *)((int)&unaff_ESI->count1StartValue + 4) = 0;
      pMVar2 = unaff_ESI;
      if (firstt != (MeProfileTimer *)0x0) {
        unaff_EDI->next = unaff_ESI;
        pMVar2 = firstt;
      }
      firstt = pMVar2;
      unaff_ESI->codeSection = codeSection;
    }
    unaff_ESI->autoStop = autoStop;
    unaff_ESI->timesCalled = unaff_ESI->timesCalled + 1;
    unaff_ESI->isRunning = '\x01';
    MeProfileGetTimerValue(&result);
    *(undefined4 *)&unaff_ESI->cpuCycleStartValue = (undefined4)result.cpuCycles;
    *(undefined4 *)((int)&unaff_ESI->cpuCycleStartValue + 4) = result.cpuCycles._4_4_;
    *(undefined4 *)((int)&unaff_ESI->count0StartValue + 4) = result.count0._4_4_;
    *(undefined4 *)&unaff_ESI->count0StartValue = (undefined4)result.count0;
    *(undefined4 *)((int)&unaff_ESI->count1StartValue + 4) = result.count1._4_4_;
    *(undefined4 *)&unaff_ESI->count1StartValue = (undefined4)result.count1;
  }
  return;
}


/* ==== MeProfileEndSectionFn ==== */

void MeProfileEndSectionFn(char *cs)

{
  MeI64 *pMVar1;
  int *piVar2;
  MeProfileTimer *pMVar3;
  bool bVar4;
  MeI64 MVar5;
  int iVar6;
  MeProfileTimer *unaff_ESI;
  uint uVar7;
  MeBool foundIt;
  MeProfileTimerResult result;
  
                    /* Unresolved local var: MeProfileTimer * thistt@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeProfileTimer * itemt@[DW_OP_reg3(EBX)] */
  if (timingRunning != '\0') {
    MeProfileGetTimerValue(&result);
    bVar4 = false;
    for (pMVar3 = firstt; pMVar3 != (MeProfileTimer *)0x0; pMVar3 = pMVar3->next) {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
      if ((pMVar3->isRunning != '\0') && (iVar6 = strcmp(pMVar3->codeSection,cs), iVar6 == 0)) {
        bVar4 = true;
        unaff_ESI = pMVar3;
        break;
      }
    }
    if (bVar4) {
      unaff_ESI->isRunning = '\0';
      uVar7 = (uint)result.cpuCycles - (int)unaff_ESI->cpuCycleStartValue;
      iVar6 = *(int *)((int)&unaff_ESI->cpuCycleStartValue + 4);
      pMVar1 = &unaff_ESI->cpuCycles;
      MVar5 = *pMVar1;
      *(uint *)pMVar1 = (int)*pMVar1 + uVar7;
      piVar2 = (int *)((int)&unaff_ESI->cpuCycles + 4);
      *piVar2 = *piVar2 + ((result.cpuCycles._4_4_ - iVar6) -
                          (uint)((uint)result.cpuCycles < (uint)unaff_ESI->cpuCycleStartValue)) +
                (uint)CARRY4((uint)MVar5,uVar7);
      uVar7 = (uint)result.count0 - (int)unaff_ESI->count0StartValue;
      iVar6 = *(int *)((int)&unaff_ESI->count0StartValue + 4);
      pMVar1 = &unaff_ESI->count0;
      MVar5 = *pMVar1;
      *(uint *)pMVar1 = (int)*pMVar1 + uVar7;
      piVar2 = (int *)((int)&unaff_ESI->count0 + 4);
      *piVar2 = *piVar2 + ((result.count0._4_4_ - iVar6) -
                          (uint)((uint)result.count0 < (uint)unaff_ESI->count0StartValue)) +
                (uint)CARRY4((uint)MVar5,uVar7);
      uVar7 = (uint)result.count1 - (int)unaff_ESI->count1StartValue;
      iVar6 = *(int *)((int)&unaff_ESI->count1StartValue + 4);
      pMVar1 = &unaff_ESI->count1;
      MVar5 = *pMVar1;
      *(uint *)pMVar1 = (int)*pMVar1 + uVar7;
      piVar2 = (int *)((int)&unaff_ESI->count1 + 4);
      *piVar2 = *piVar2 + ((result.count1._4_4_ - iVar6) -
                          (uint)((uint)result.count1 < (uint)unaff_ESI->count1StartValue)) +
                (uint)CARRY4((uint)MVar5,uVar7);
    }
    else {
      MeWarning(3,
                "MeProfileEndSectionFn: You tried to stop \'%s\'\nwhich you hadn\'t started, which is a bit silly."
               );
    }
  }
  return;
}


/* ==== MeProfileGetSectionTime ==== */

MeReal MeProfileGetSectionTime(char *codeSection)

{
  MeProfileFrameData *pMVar1;
  MeProfileFrameData *pMVar2;
  int iVar3;
  MeProfileFrameData *unaff_EBX;
  MeProfileFrameSectionInfo *pMVar4;
  longdouble lVar5;
  
                    /* Unresolved local var: MeProfileFrameData * thisfd@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeProfileFrameSectionInfo * thisfsi@[DW_OP_reg3(EBX)]
                        */
  if (firstfd != (MeProfileFrameData *)0x0) {
    if (insideFrame != '\0') {
      MeWarning(3,
                "MeProfileGetSectionTime: You called\nMeProfileGetSectionTime(\"%s\") while a frame was under way.\nThis time is therefore for the frame before last!"
               );
    }
    if (logging == kMeProfileLogAll) {
      pMVar2 = firstfd->next;
      unaff_EBX = firstfd;
      while (pMVar1 = pMVar2, pMVar1 != (MeProfileFrameData *)0x0) {
        unaff_EBX = pMVar1;
        pMVar2 = pMVar1->next;
      }
    }
    else if ((logging == kMeProfileDontLog) || (logging == kMeProfileLogTotals)) {
      unaff_EBX = firstfd;
    }
    for (pMVar4 = unaff_EBX->firstfsi; pMVar4 != (MeProfileFrameSectionInfo *)0x0;
        pMVar4 = pMVar4->next) {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
      iVar3 = strcmp(pMVar4->codeSection,codeSection);
      if (iVar3 == 0) {
        lVar5 = ((longdouble)pMVar4->cpuCycles / (longdouble)clockSpeed) * (longdouble)1000.0;
        goto LAB_0001078d;
      }
    }
  }
  lVar5 = (longdouble)0;
LAB_0001078d:
  return (MeReal)lVar5;
}


/* ==== MeProfileOutputResults ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeProfileOutputResults(void)

{
  uint *puVar1;
  MeProfileFrameData *pMVar2;
  MeProfileFrameSectionInfo *pMVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  uint uVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  uint extraout_ECX;
  uint uVar11;
  MeProfileTimer *pMVar12;
  MeProfileFrameData *pMVar13;
  int iVar14;
  bool bVar15;
  longdouble lVar16;
  longdouble lVar17;
  longdouble lVar18;
  MeI64 cyclesSectAvgEvent;
  MeI64 *count1PerSection;
  MeI64 *count0PerSection;
  MeI64 *cyclesPerSection;
  MeI64 *eventsPerSection;
  float secondsAvgPerFrame;
  float secondsAllFrames;
  MeI64 count1AllFrames;
  MeI64 count0AllFrames;
  MeI64 cyclesAllFrames;
  MeI64 eventsAllFrames;
  float avSectionsTimeInSecs;
  float avTimeInSecs;
  MeU64 sectionsTotal;
  MeU64 total;
  MeU64 *sectionTotal;
  uint nSections;
  uint nFrames;
  
                    /* Unresolved local var: MeProfileFrameData * thisfd@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeProfileFrameSectionInfo * thisfsi@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeProfileTimer * timer@[DW_OP_reg3(EBX)]
                       Unresolved local var: uint nFrameDatas@[???] */
  nSections = 0;
  if (logging == kMeProfileDontLog) {
    MeWarning(1,&DAT_00013585);
  }
  GetOutputInfo();
  uVar11 = extraout_ECX;
  pMVar2 = firstfd;
  if (firstfd != (MeProfileFrameData *)0x0) {
    do {
      pMVar13 = pMVar2;
      pMVar2 = pMVar13->next;
    } while (pMVar2 != (MeProfileFrameData *)0x0);
    uVar11 = pMVar13->frameNumber + 1;
    nFrames = uVar11;
  }
  pMVar12 = firstt;
  if (logging == kMeProfileDontLog) {
    nFrames = 1;
  }
  for (; pMVar12 != (MeProfileTimer *)0x0; pMVar12 = pMVar12->next) {
    nSections = nSections + 1;
  }
  if (output.style == kMeProfileOutputEvent) {
    iVar14 = nSections << 3;
    eventsAllFrames._0_4_ = 0;
    eventsAllFrames._4_4_ = 0;
    cyclesAllFrames._0_4_ = 0;
    cyclesAllFrames._4_4_ = 0;
    count0AllFrames._0_4_ = 0;
    count0AllFrames._4_4_ = 0;
    count1AllFrames._0_4_ = 0;
    count1AllFrames._4_4_ = 0;
    puVar8 = (undefined4 *)(*_MeProfileGetTimerValue)(iVar14,8,uVar11);
    puVar9 = (undefined4 *)(*_MeProfileGetTimerValue)(iVar14);
    uVar11 = 0;
    puVar10 = (undefined4 *)(*_MeProfileGetTimerValue)(iVar14);
    puVar6 = (undefined4 *)(*_MeProfileGetTimerValue)(iVar14);
    pMVar2 = firstfd;
    if (nSections != 0) {
      uVar7 = nSections & 3;
      if (nSections < 2) {
LAB_00010d50:
        puVar8[uVar11 * 2] = 0;
        puVar8[uVar11 * 2 + 1] = 0;
        puVar9[uVar11 * 2] = 0;
        puVar9[uVar11 * 2 + 1] = 0;
        puVar10[uVar11 * 2] = 0;
        puVar10[uVar11 * 2 + 1] = 0;
        puVar6[uVar11 * 2] = 0;
        puVar6[uVar11 * 2 + 1] = 0;
        uVar11 = uVar11 + 1;
        pMVar2 = firstfd;
        if (nSections <= uVar11) goto joined_r0x00010efd;
      }
      else if (uVar7 != 0) {
        if (1 < uVar7) {
          if (2 < uVar7) {
            *puVar8 = 0;
            puVar8[1] = 0;
            *puVar9 = 0;
            puVar9[1] = 0;
            *puVar10 = 0;
            puVar10[1] = 0;
            *puVar6 = 0;
            puVar6[1] = 0;
          }
          uVar11 = (uint)(2 < uVar7);
          puVar8[uVar11 * 2] = 0;
          puVar8[uVar11 * 2 + 1] = 0;
          puVar9[uVar11 * 2] = 0;
          puVar9[uVar11 * 2 + 1] = 0;
          puVar10[uVar11 * 2] = 0;
          puVar10[uVar11 * 2 + 1] = 0;
          puVar6[uVar11 * 2] = 0;
          puVar6[uVar11 * 2 + 1] = 0;
          uVar11 = uVar11 + 1;
        }
        goto LAB_00010d50;
      }
      do {
        puVar8[uVar11 * 2] = 0;
        puVar8[uVar11 * 2 + 1] = 0;
        puVar9[uVar11 * 2] = 0;
        puVar9[uVar11 * 2 + 1] = 0;
        puVar10[uVar11 * 2] = 0;
        puVar10[uVar11 * 2 + 1] = 0;
        iVar14 = uVar11 + 1;
        puVar6[uVar11 * 2] = 0;
        puVar6[uVar11 * 2 + 1] = 0;
        puVar8[iVar14 * 2] = 0;
        puVar8[iVar14 * 2 + 1] = 0;
        puVar9[iVar14 * 2] = 0;
        puVar9[iVar14 * 2 + 1] = 0;
        puVar10[iVar14 * 2] = 0;
        puVar10[iVar14 * 2 + 1] = 0;
        puVar6[iVar14 * 2] = 0;
        puVar6[iVar14 * 2 + 1] = 0;
        iVar14 = uVar11 + 2;
        puVar8[iVar14 * 2] = 0;
        puVar8[iVar14 * 2 + 1] = 0;
        puVar9[iVar14 * 2] = 0;
        puVar9[iVar14 * 2 + 1] = 0;
        puVar10[iVar14 * 2] = 0;
        puVar10[iVar14 * 2 + 1] = 0;
        puVar6[iVar14 * 2] = 0;
        puVar6[iVar14 * 2 + 1] = 0;
        iVar14 = uVar11 + 3;
        uVar11 = uVar11 + 4;
        puVar8[iVar14 * 2] = 0;
        puVar8[iVar14 * 2 + 1] = 0;
        puVar9[iVar14 * 2] = 0;
        puVar9[iVar14 * 2 + 1] = 0;
        puVar10[iVar14 * 2] = 0;
        puVar10[iVar14 * 2 + 1] = 0;
        puVar6[iVar14 * 2] = 0;
        puVar6[iVar14 * 2 + 1] = 0;
        pMVar2 = firstfd;
      } while (uVar11 < nSections);
    }
joined_r0x00010efd:
    for (; pMVar2 != (MeProfileFrameData *)0x0; pMVar2 = pMVar2->next) {
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
      uVar11 = (uint)(pMVar2->timings).cpuCycles;
      bVar15 = CARRY4((uint)cyclesAllFrames,uVar11);
      cyclesAllFrames._0_4_ = (uint)cyclesAllFrames + uVar11;
      cyclesAllFrames._4_4_ =
           cyclesAllFrames._4_4_ + *(int *)((int)&(pMVar2->timings).cpuCycles + 4) + (uint)bVar15;
      uVar11 = (uint)(pMVar2->timings).count0;
      bVar15 = CARRY4((uint)count0AllFrames,uVar11);
      count0AllFrames._0_4_ = (uint)count0AllFrames + uVar11;
      count0AllFrames._4_4_ =
           count0AllFrames._4_4_ + *(int *)((int)&(pMVar2->timings).count0 + 4) + (uint)bVar15;
      uVar11 = (uint)(pMVar2->timings).count1;
      bVar15 = CARRY4((uint)count1AllFrames,uVar11);
      count1AllFrames._0_4_ = (uint)count1AllFrames + uVar11;
      count1AllFrames._4_4_ =
           count1AllFrames._4_4_ + *(int *)((int)&(pMVar2->timings).count1 + 4) + (uint)bVar15;
      iVar14 = 0;
      for (pMVar3 = pMVar2->firstfsi; pMVar3 != (MeProfileFrameSectionInfo *)0x0;
          pMVar3 = pMVar3->next) {
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
        uVar7 = pMVar3->timesCalled;
        bVar15 = CARRY4((uint)eventsAllFrames,uVar7);
        eventsAllFrames._0_4_ = (uint)eventsAllFrames + uVar7;
        eventsAllFrames._4_4_ = eventsAllFrames._4_4_ + (uint)bVar15;
        puVar1 = puVar8 + iVar14 * 2;
        uVar11 = *puVar1;
        *puVar1 = *puVar1 + uVar7;
        puVar8[iVar14 * 2 + 1] = puVar8[iVar14 * 2 + 1] + (uint)CARRY4(uVar11,uVar7);
        uVar7 = (uint)pMVar3->cpuCycles;
        iVar4 = *(int *)((int)&pMVar3->cpuCycles + 4);
        puVar1 = puVar9 + iVar14 * 2;
        uVar11 = *puVar1;
        *puVar1 = *puVar1 + uVar7;
        puVar9[iVar14 * 2 + 1] = puVar9[iVar14 * 2 + 1] + iVar4 + (uint)CARRY4(uVar11,uVar7);
        uVar7 = (uint)pMVar3->count0;
        iVar4 = *(int *)((int)&pMVar3->count0 + 4);
        puVar1 = puVar10 + iVar14 * 2;
        uVar11 = *puVar1;
        *puVar1 = *puVar1 + uVar7;
        puVar10[iVar14 * 2 + 1] = puVar10[iVar14 * 2 + 1] + iVar4 + (uint)CARRY4(uVar11,uVar7);
        uVar7 = (uint)pMVar3->count1;
        iVar4 = *(int *)((int)&pMVar3->count1 + 4);
        puVar1 = puVar6 + iVar14 * 2;
        uVar11 = *puVar1;
        *puVar1 = *puVar1 + uVar7;
        puVar6[iVar14 * 2 + 1] = puVar6[iVar14 * 2 + 1] + iVar4 + (uint)CARRY4(uVar11,uVar7);
        iVar14 = iVar14 + 1;
      }
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
    }
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
    __divdi3((uint)clockSpeed + 100000,clockSpeed._4_4_ + (uint)(0xfffe795f < (uint)clockSpeed),
             1000000,0);
    MeInfo(0,"CLOCK SPEED (approx): %lldMHz");
    MeInfo(0,&DAT_000135a2);
    MeInfo(0,"OVERALL TOTALS:");
    MeInfo(0,"  %6.1f milliseconds, %u frames");
    MeInfo(0,"  cycles: %11lld; calls:  %11lld");
    MeInfo(0,"  count0: %11lld; count1: %11lld");
    MeInfo(0,&DAT_000135a2);
                    /* Unresolved local var: MeI64 eventsAvgPerFrame@[???]
                       Unresolved local var: MeI64 cyclesAvgPerFrame@[???]
                       Unresolved local var: MeI64 count0AvgPerFrame@[???]
                       Unresolved local var: MeI64 count1AvgPerFrame@[???] */
    __divdi3((uint)eventsAllFrames,eventsAllFrames._4_4_,nFrames,0);
    __divdi3((uint)cyclesAllFrames,cyclesAllFrames._4_4_,nFrames,0);
    __divdi3((uint)count0AllFrames,count0AllFrames._4_4_,nFrames,0);
    __divdi3((uint)count1AllFrames,count1AllFrames._4_4_,nFrames,0);
    MeInfo(0,"OVERALL PER FRAME AVERAGES:");
    MeInfo(0,"  %6.1f milliseconds, %-4.1f%% of 60Hz");
    MeInfo(0,"  cycles: %11lld; calls:  %11lld");
    MeInfo(0,"  count0: %11lld; count1: %11lld");
    MeInfo(0,&DAT_000135a2);
    MeInfo(0,"    %s");
    MeInfo(0,&DAT_000135a2);
    MeInfo(0,"SECTION TOTALS                  CYCLES   CALLS %10.10s %10.10s");
    for (pMVar12 = firstt; pMVar12 != (MeProfileTimer *)0x0; pMVar12 = pMVar12->next) {
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
      MeInfo(0,"%-26.26s:%11lld %7lld %10lld %10lld");
    }
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
    MeInfo(0,&DAT_000135a2);
    iVar14 = 0;
    MeInfo(0,"SECTION AVG. PER CALL           CYCLES     MS. %10.10s %10.10s");
    for (pMVar12 = firstt; pMVar12 != (MeProfileTimer *)0x0; pMVar12 = pMVar12->next) {
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
                    /* Unresolved local var: MeU32 nEvents@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeU32 count0SectAvgEvent@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeU32 count1SectAvgEvent@[DW_OP_reg0(EAX)]
                       Unresolved local var: float secondsSectAvgEvent@[???] */
      if (puVar8[iVar14 * 2] != 0) {
        __divdi3(puVar9[iVar14 * 2],puVar9[iVar14 * 2 + 1],puVar8[iVar14 * 2],0);
      }
      iVar14 = iVar14 + 1;
      MeInfo(0,"%-26.26s:%11lld %7.3f %10lu %10lu");
    }
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
    (*_strcmp)(puVar8);
    (*_strcmp)(puVar9);
    (*_strcmp)(puVar10);
    goto LAB_00010c28;
  }
  if (output.style != kMeProfileOutputAverage) {
    pMVar2 = firstfd;
    if (output.style == kMeProfileOutputNormal) {
      for (; pMVar2 != (MeProfileFrameData *)0x0; pMVar2 = pMVar2->next) {
        MeInfo(0,"Frame %d.");
        for (pMVar3 = pMVar2->firstfsi; pMVar3 != (MeProfileFrameSectionInfo *)0x0;
            pMVar3 = pMVar3->next) {
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
          MeInfo(0,"%s: %lld cycles");
          MeInfo(0,"%s: %lld cycles");
        }
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
      }
      return;
    }
    if (output.style != kMeProfileOutputGnuplot) {
      MeWarning(0xc,"MeProfileOutputResults: MeProfile output style not recognized");
      return;
    }
    doGnuplot();
    return;
  }
                    /* Unresolved local var: MeU64 avCpu@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeU64 avSections@[???]
                       Unresolved local var: uint i@[DW_OP_reg7(EDI)] */
  uVar11 = 0;
  puVar6 = (undefined4 *)(*_MeProfileGetTimerValue)(nSections << 3,8,logging,logging);
  total._0_4_ = 0;
  total._4_4_ = 0;
  sectionsTotal._0_4_ = 0;
  sectionsTotal._4_4_ = 0;
  pMVar2 = firstfd;
  if (nSections != 0) {
    uVar7 = nSections & 3;
    if (nSections < 2) {
LAB_00010931:
      puVar6[uVar11 * 2] = 0;
      puVar6[uVar11 * 2 + 1] = 0;
      uVar11 = uVar11 + 1;
      pMVar2 = firstfd;
      if (nSections <= uVar11) goto joined_r0x0001099b;
    }
    else if (uVar7 != 0) {
      if (1 < uVar7) {
        if (2 < uVar7) {
          *puVar6 = 0;
          puVar6[1] = 0;
        }
        uVar11 = (uint)(2 < uVar7);
        puVar6[uVar11 * 2] = 0;
        puVar6[uVar11 * 2 + 1] = 0;
        uVar11 = uVar11 + 1;
      }
      goto LAB_00010931;
    }
    do {
      puVar6[uVar11 * 2] = 0;
      puVar6[uVar11 * 2 + 1] = 0;
      puVar6[uVar11 * 2 + 2] = 0;
      puVar6[uVar11 * 2 + 3] = 0;
      puVar6[uVar11 * 2 + 4] = 0;
      puVar6[uVar11 * 2 + 5] = 0;
      puVar6[uVar11 * 2 + 6] = 0;
      puVar6[uVar11 * 2 + 7] = 0;
      uVar11 = uVar11 + 4;
      pMVar2 = firstfd;
    } while (uVar11 < nSections);
  }
joined_r0x0001099b:
  for (; pMVar2 != (MeProfileFrameData *)0x0; pMVar2 = pMVar2->next) {
    uVar11 = (uint)(pMVar2->timings).cpuCycles;
    iVar14 = 0;
    bVar15 = CARRY4((uint)total,uVar11);
    total._0_4_ = (uint)total + uVar11;
    total._4_4_ = total._4_4_ + *(int *)((int)&(pMVar2->timings).cpuCycles + 4) + (uint)bVar15;
    for (pMVar3 = pMVar2->firstfsi; pMVar3 != (MeProfileFrameSectionInfo *)0x0;
        pMVar3 = pMVar3->next) {
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
      uVar7 = (uint)pMVar3->cpuCycles;
      iVar4 = *(int *)((int)&pMVar3->cpuCycles + 4);
      puVar1 = puVar6 + iVar14 * 2;
      uVar11 = *puVar1;
      *puVar1 = *puVar1 + uVar7;
      puVar6[iVar14 * 2 + 1] = puVar6[iVar14 * 2 + 1] + iVar4 + (uint)CARRY4(uVar11,uVar7);
      iVar14 = iVar14 + 1;
      uVar11 = (uint)pMVar3->cpuCycles;
      bVar15 = CARRY4((uint)sectionsTotal,uVar11);
      sectionsTotal._0_4_ = (uint)sectionsTotal + uVar11;
      sectionsTotal._4_4_ =
           sectionsTotal._4_4_ + *(int *)((int)&pMVar3->cpuCycles + 4) + (uint)bVar15;
    }
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
  }
  iVar14 = 0;
  MeInfo(0,"Averages over %u frames. NB: Sections can be concurrent.");
  MeInfo(0,&DAT_000135a2);
  MeInfo(0,"SECTION PROFILED                 AV CYCLES   AV %%  AV MS");
  if (firstt != (MeProfileTimer *)0x0) {
    pMVar12 = firstt;
    do {
      lVar16 = (longdouble)CONCAT44(puVar6[iVar14 * 2 + 1],puVar6[iVar14 * 2]);
      if ((int)puVar6[iVar14 * 2 + 1] < 0) {
        lVar16 = _DAT_000136c0 + lVar16;
      }
      lVar17 = (longdouble)CONCAT44(puVar6[iVar14 * 2 + 1],puVar6[iVar14 * 2]);
      if ((int)puVar6[iVar14 * 2 + 1] < 0) {
        lVar17 = _DAT_000136c0 + lVar17;
      }
      lVar18 = (longdouble)CONCAT44(total._4_4_,(uint)total);
      if (total._4_4_ < 0) {
        lVar18 = _DAT_000136c0 + (longdouble)CONCAT44(total._4_4_,(uint)total);
      }
      iVar4 = iVar14 * 2;
      iVar5 = iVar14 * 2;
      iVar14 = iVar14 + 1;
      __udivdi3(puVar6[iVar5],puVar6[iVar4 + 1],nFrames,0,
                (double)((float)lVar17 * 100.0 * (1.0 / (float)lVar18)),
                ((double)(float)lVar16 * 1000.0) /
                ((double)nFrames * (double)CONCAT44(clockSpeed._4_4_,(uint)clockSpeed)));
      MeInfo(0,"%-30s: %10u %6.3f %6.3f");
      pMVar12 = pMVar12->next;
    } while (pMVar12 != (MeProfileTimer *)0x0);
  }
  __udivdi3((uint)sectionsTotal,sectionsTotal._4_4_,nFrames,0);
  __udivdi3((uint)total,total._4_4_,nFrames,0);
  MeInfo(0,"SECTIONS TOTAL                  %10lld");
  MeInfo(0,"TOTAL                           %10lld");
  __divdi3((uint)clockSpeed + 100000,clockSpeed._4_4_ + (uint)(0xfffe795f < (uint)clockSpeed),
           1000000,0);
  MeInfo(0,"Clock speed (approx): %lldMHz");
  MeInfo(0,"Average time %6.2f milliseconds, %5.2f%% of a 60hz frame (sections)");
  MeInfo(0,"Average time %6.2f milliseconds, %5.2f%% of a 60hz frame (total)");
LAB_00010c28:
                    /* Unresolved local var: MeI64 secondsTotal@[???]
                       Unresolved local var: uint i@[DW_OP_reg7(EDI)] */
  (*_strcmp)(puVar6);
                    /* WARNING: Read-only address (ram,0x000136c0) is written */
  return;
}


/* ==== doGnuplot ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void doGnuplot(void)

{
  MeProfileTimer *pMVar1;
  MeProfileFrameSectionInfo *pMVar2;
  _weightingData *p_Var3;
  MeProfileFrameData *pMVar4;
  longlong lVar5;
  longlong lVar6;
  weightingData *pwVar7;
  float fVar8;
  MeI64 MVar9;
  MeU64 MVar10;
  weightingData *pwVar11;
  _weightingData *p_Var12;
  int iVar13;
  undefined *puVar14;
  int iVar15;
  uint uVar16;
  uint uVar17;
  uint *puVar18;
  char *pcVar19;
  uint *puVar20;
  byte bVar21;
  uint uVar22;
  uint uVar23;
  int iVar24;
  bool bVar25;
  ulonglong uVar26;
  ulonglong uVar27;
  undefined8 uVar28;
  char *pcVar29;
  MeProfileFrameSectionInfo *thisfsi;
  MeProfileFrameData *thisfd;
  ushort frames;
  weightingData *firstWD;
  weightingData *tailWD;
  weightingData *thisWD;
  MeU64 count1Acc;
  MeU64 count0Acc;
  MeU64 cpuCycleAcc;
  int fh;
  char buffer [1000];
  
                    /* Unresolved local var: int counter@[DW_OP_reg6(ESI)]
                       Unresolved local var: uchar twiddler@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeProfileTimer * thist@[DW_OP_reg3(EBX)] */
  cpuCycleAcc._0_4_ = 0;
  cpuCycleAcc._4_4_ = 0;
  count0Acc._0_4_ = 0;
  count0Acc._4_4_ = 0;
  count1Acc._0_4_ = 0;
  count1Acc._4_4_ = 0;
  firstWD = (weightingData *)0x0;
  pwVar7 = firstWD;
  for (pMVar1 = firstt; pMVar1 != (MeProfileTimer *)0x0; pMVar1 = pMVar1->next) {
    pwVar11 = (weightingData *)(*_MeMemoryAPI)(0x24);
    pwVar11->next = (_weightingData *)0x0;
    *(undefined4 *)&pwVar11->timesCalled = 0;
    *(undefined4 *)((int)&pwVar11->timesCalled + 4) = 0;
    *(undefined4 *)&pwVar11->cpuCycles = 0;
    *(undefined4 *)((int)&pwVar11->cpuCycles + 4) = 0;
    *(undefined4 *)&pwVar11->count0 = 0;
    *(undefined4 *)((int)&pwVar11->count0 + 4) = 0;
    *(undefined4 *)&pwVar11->count1 = 0;
    *(undefined4 *)((int)&pwVar11->count1 + 4) = 0;
    firstWD = pwVar11;
    if (pwVar7 != (weightingData *)0x0) {
      tailWD->next = pwVar11;
      firstWD = pwVar7;
    }
    pwVar7 = firstWD;
    tailWD = pwVar11;
  }
  p_Var12 = (_weightingData *)(*_MeProfileDestroyHardwareTimer)(0x24);
  tailWD->next = p_Var12;
  thisfd = firstfd;
  MeInfo(0,"twiddler=%d output.settletime=%d thisfd=%08x");
  bVar21 = 1;
  if (output.settletime != 0) {
    do {
      uVar22 = (uint)bVar21;
      bVar21 = bVar21 + 1;
      thisfd = thisfd->next;
    } while (uVar22 != output.settletime);
  }
  p_Var3 = pwVar7;
  for (pMVar2 = thisfd->firstfsi; pMVar2 != (MeProfileFrameSectionInfo *)0x0; pMVar2 = pMVar2->next)
  {
    MVar9 = pMVar2->cpuCycles;
    *(undefined4 *)((int)&p_Var3->cpuCycles + 4) = *(undefined4 *)((int)&pMVar2->cpuCycles + 4);
    *(int *)&p_Var3->cpuCycles = (int)MVar9;
    *(uint *)&p_Var3->timesCalled = pMVar2->timesCalled;
    *(undefined4 *)((int)&p_Var3->timesCalled + 4) = 0;
    MVar9 = pMVar2->count0;
    *(undefined4 *)((int)&p_Var3->count0 + 4) = *(undefined4 *)((int)&pMVar2->count0 + 4);
    *(int *)&p_Var3->count0 = (int)MVar9;
    MVar9 = pMVar2->count1;
    *(undefined4 *)((int)&p_Var3->count1 + 4) = *(undefined4 *)((int)&pMVar2->count1 + 4);
    *(int *)&p_Var3->count1 = (int)MVar9;
    p_Var3 = p_Var3->next;
  }
  MVar10 = (thisfd->timings).cpuCycles;
  *(undefined4 *)((int)&p_Var12->cpuCycles + 4) =
       *(undefined4 *)((int)&(thisfd->timings).cpuCycles + 4);
  *(int *)&p_Var12->cpuCycles = (int)MVar10;
  MVar10 = (thisfd->timings).count0;
  *(undefined4 *)((int)&p_Var12->count0 + 4) = *(undefined4 *)((int)&(thisfd->timings).count0 + 4);
  *(int *)&p_Var12->count0 = (int)MVar10;
  MVar10 = (thisfd->timings).count1;
  *(undefined4 *)((int)&p_Var12->count1 + 4) = *(undefined4 *)((int)&(thisfd->timings).count1 + 4);
  *(int *)&p_Var12->count1 = (int)MVar10;
  if (output.style == kMeProfileOutputGnuplot) {
    iVar13 = MeOpen("cyclePlot.gpt",1);
    sprintf((char *)0x0,buffer,"plot ",thisfd);
    puVar18 = (uint *)buffer;
    do {
      puVar20 = puVar18;
      puVar18 = puVar20 + 1;
      uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
      uVar23 = uVar22 & 0x80808080;
    } while (uVar23 == 0);
    if ((uVar22 & 0x8080) == 0) {
      uVar23 = uVar23 >> 0x10;
      puVar18 = (uint *)((int)puVar20 + 6);
    }
    iVar24 = 2;
    MeWrite(iVar13,buffer,
            (int)((int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer)));
    for (pMVar1 = firstt; pMVar1 != (MeProfileTimer *)0x0; pMVar1 = pMVar1->next) {
      pcVar29 = pMVar1->codeSection;
      iVar15 = iVar24;
      sprintf(buffer,"\"MeProfile.dat\" using 1:%d title \"%s - Cycles\" with lines",iVar24,pcVar29)
      ;
      puVar18 = (uint *)buffer;
      do {
        puVar20 = puVar18;
        puVar18 = puVar20 + 1;
        uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
        uVar23 = uVar22 & 0x80808080;
      } while (uVar23 == 0);
      if ((uVar22 & 0x8080) == 0) {
        uVar23 = uVar23 >> 0x10;
        puVar18 = (uint *)((int)puVar20 + 6);
      }
      sprintf((char *)((int)puVar18 + (-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23))),", ",iVar15,
              pcVar29);
      puVar18 = (uint *)buffer;
      do {
        puVar20 = puVar18;
        puVar18 = puVar20 + 1;
        uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
        uVar23 = uVar22 & 0x80808080;
      } while (uVar23 == 0);
      if ((uVar22 & 0x8080) == 0) {
        uVar23 = uVar23 >> 0x10;
        puVar18 = (uint *)((int)puVar20 + 6);
      }
      iVar24 = iVar24 + 4;
      MeWrite(iVar13,buffer,
              (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
    }
    pcVar29 = "Whole Frame";
    sprintf(buffer,"\"MeProfile.dat\" using 1:%d title \"%s\" with lines",iVar24,"Whole Frame");
    puVar18 = (uint *)buffer;
    do {
      puVar20 = puVar18;
      puVar18 = puVar20 + 1;
      uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
      uVar23 = uVar22 & 0x80808080;
    } while (uVar23 == 0);
    if ((uVar22 & 0x8080) == 0) {
      uVar23 = uVar23 >> 0x10;
      puVar18 = (uint *)((int)puVar20 + 6);
    }
    sprintf((char *)((int)puVar18 + (-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23))),"",iVar24,
            pcVar29);
    puVar18 = (uint *)buffer;
    do {
      puVar20 = puVar18;
      puVar18 = puVar20 + 1;
      uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
      uVar23 = uVar22 & 0x80808080;
    } while (uVar23 == 0);
    if ((uVar22 & 0x8080) == 0) {
      uVar23 = uVar23 >> 0x10;
      puVar18 = (uint *)((int)puVar20 + 6);
    }
    MeWrite(iVar13,buffer,
            (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
    MeClose(iVar13);
    iVar13 = MeOpen("timesCalledPlot.gpt",1);
    sprintf(buffer,"plot ");
    puVar18 = (uint *)buffer;
    do {
      puVar20 = puVar18;
      puVar18 = puVar20 + 1;
      uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
      uVar23 = uVar22 & 0x80808080;
    } while (uVar23 == 0);
    if ((uVar22 & 0x8080) == 0) {
      uVar23 = uVar23 >> 0x10;
      puVar18 = (uint *)((int)puVar20 + 6);
    }
    iVar24 = 3;
    MeWrite(iVar13,buffer,
            (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
    for (pMVar1 = firstt; pMVar1 != (MeProfileTimer *)0x0; pMVar1 = pMVar1->next) {
      if (pMVar1->next == (MeProfileTimer *)0x0) {
        puVar14 = &DAT_000135a2;
      }
      else {
        puVar14 = &DAT_0001363e;
      }
      sprintf(buffer,"\"MeProfile.dat\" using 1:%d title \"%s - Times Called\" with lines%s",iVar24,
              pMVar1->codeSection,puVar14);
      puVar18 = (uint *)buffer;
      do {
        puVar20 = puVar18;
        puVar18 = puVar20 + 1;
        uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
        uVar23 = uVar22 & 0x80808080;
      } while (uVar23 == 0);
      if ((uVar22 & 0x8080) == 0) {
        uVar23 = uVar23 >> 0x10;
        puVar18 = (uint *)((int)puVar20 + 6);
      }
      iVar24 = iVar24 + 4;
      MeWrite(iVar13,buffer,
              (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
    }
    MeClose(iVar13);
    iVar13 = MeOpen("count0Plot.gpt",1);
    sprintf(buffer,"plot ");
    puVar18 = (uint *)buffer;
    do {
      puVar20 = puVar18;
      puVar18 = puVar20 + 1;
      uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
      uVar23 = uVar22 & 0x80808080;
    } while (uVar23 == 0);
    if ((uVar22 & 0x8080) == 0) {
      uVar23 = uVar23 >> 0x10;
      puVar18 = (uint *)((int)puVar20 + 6);
    }
    iVar24 = 4;
    MeWrite(iVar13,buffer,
            (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
    for (pMVar1 = firstt; pMVar1 != (MeProfileTimer *)0x0; pMVar1 = pMVar1->next) {
      if (HWTMode->counterMode == kMeProfileCounterModeFlops) {
        pcVar29 = pMVar1->codeSection;
        pcVar19 = "\"MeProfile.dat\" using 1:%d title \"%s - VeOps\" with lines, ";
LAB_000128f2:
        sprintf(buffer,pcVar19,iVar24,pcVar29);
      }
      else if (HWTMode->counterMode == kMeProfileCounterModeCache) {
        pcVar29 = pMVar1->codeSection;
        pcVar19 = "\"MeProfile.dat\" using 1:%d title \"%s - ICache Misses\" with lines, ";
        goto LAB_000128f2;
      }
      puVar18 = (uint *)buffer;
      do {
        puVar20 = puVar18;
        puVar18 = puVar20 + 1;
        uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
        uVar23 = uVar22 & 0x80808080;
      } while (uVar23 == 0);
      if ((uVar22 & 0x8080) == 0) {
        uVar23 = uVar23 >> 0x10;
        puVar18 = (uint *)((int)puVar20 + 6);
      }
      iVar24 = iVar24 + 4;
      MeWrite(iVar13,buffer,
              (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
    }
    pcVar29 = "Whole Frame";
    sprintf(buffer,"\"MeProfile.dat\" using 1:%d title \"%s\" with lines",iVar24 + -1,"Whole Frame")
    ;
    puVar18 = (uint *)buffer;
    do {
      puVar20 = puVar18;
      puVar18 = puVar20 + 1;
      uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
      uVar23 = uVar22 & 0x80808080;
    } while (uVar23 == 0);
    if ((uVar22 & 0x8080) == 0) {
      uVar23 = uVar23 >> 0x10;
      puVar18 = (uint *)((int)puVar20 + 6);
    }
    pcVar19 = (char *)((int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer)
                      );
    MeWrite(iVar13,buffer,(int)pcVar19);
    MeClose(iVar13);
    iVar13 = MeOpen("count1Plot.gpt",1);
    sprintf(buffer,"plot ",pcVar19,pcVar29);
    puVar18 = (uint *)buffer;
    do {
      puVar20 = puVar18;
      puVar18 = puVar20 + 1;
      uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
      uVar23 = uVar22 & 0x80808080;
    } while (uVar23 == 0);
    if ((uVar22 & 0x8080) == 0) {
      uVar23 = uVar23 >> 0x10;
      puVar18 = (uint *)((int)puVar20 + 6);
    }
    iVar24 = 5;
    MeWrite(iVar13,buffer,
            (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
    for (pMVar1 = firstt; pMVar1 != (MeProfileTimer *)0x0; pMVar1 = pMVar1->next) {
      if (HWTMode->counterMode == kMeProfileCounterModeFlops) {
        pcVar29 = pMVar1->codeSection;
        pcVar19 = "\"MeProfile.dat\" using 1:%d title \"%s - FlOps\" with lines";
LAB_000128c1:
        iVar15 = sprintf(buffer,pcVar19,iVar24,pcVar29);
      }
      else {
        iVar15 = HWTMode->counterMode - kMeProfileCounterModeCache;
        if (iVar15 == 0) {
          pcVar29 = pMVar1->codeSection;
          pcVar19 = "\"MeProfile.dat\" using 1:%d title \"%s - DCache Misses\" with lines";
          goto LAB_000128c1;
        }
      }
      puVar18 = (uint *)buffer;
      do {
        puVar20 = puVar18;
        puVar18 = puVar20 + 1;
        uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
        uVar23 = uVar22 & 0x80808080;
      } while (uVar23 == 0);
      if ((uVar22 & 0x8080) == 0) {
        uVar23 = uVar23 >> 0x10;
        puVar18 = (uint *)((int)puVar20 + 6);
      }
      sprintf((char *)((int)puVar18 + (-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23))),", ",iVar15,
              iVar15);
      puVar18 = (uint *)buffer;
      do {
        puVar20 = puVar18;
        puVar18 = puVar20 + 1;
        uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
        uVar23 = uVar22 & 0x80808080;
      } while (uVar23 == 0);
      if ((uVar22 & 0x8080) == 0) {
        uVar23 = uVar23 >> 0x10;
        puVar18 = (uint *)((int)puVar20 + 6);
      }
      iVar24 = iVar24 + 4;
      MeWrite(iVar13,buffer,
              (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
    }
    pcVar29 = "Whole Frame";
    iVar24 = iVar24 + -1;
    sprintf(buffer,"\"MeProfile.dat\" using 1:%d title \"%s\" with lines",iVar24,"Whole Frame");
    puVar18 = (uint *)buffer;
    do {
      puVar20 = puVar18;
      puVar18 = puVar20 + 1;
      uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
      uVar23 = uVar22 & 0x80808080;
    } while (uVar23 == 0);
    if ((uVar22 & 0x8080) == 0) {
      uVar23 = uVar23 >> 0x10;
      puVar18 = (uint *)((int)puVar20 + 6);
    }
    sprintf((char *)((int)puVar18 + (-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23))),"",iVar24,
            pcVar29);
    puVar18 = (uint *)buffer;
    do {
      puVar20 = puVar18;
      puVar18 = puVar20 + 1;
      uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
      uVar23 = uVar22 & 0x80808080;
    } while (uVar23 == 0);
    if ((uVar22 & 0x8080) == 0) {
      uVar23 = uVar23 >> 0x10;
      puVar18 = (uint *)((int)puVar20 + 6);
    }
    MeWrite(iVar13,buffer,
            (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
    MeClose(iVar13);
    iVar24 = MeOpen("MeProfile.dat",1);
    iVar13 = iVar24;
    for (pMVar4 = firstfd; pMVar4 != (MeProfileFrameData *)0x0; pMVar4 = pMVar4->next) {
      if (pMVar4->frameNumber <= output.settletime) {
        sprintf(buffer,"#",iVar13,iVar13);
        puVar18 = (uint *)buffer;
        do {
          puVar20 = puVar18;
          puVar18 = puVar20 + 1;
          uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
          uVar23 = uVar22 & 0x80808080;
        } while (uVar23 == 0);
        if ((uVar22 & 0x8080) == 0) {
          uVar23 = uVar23 >> 0x10;
          puVar18 = (uint *)((int)puVar20 + 6);
        }
        iVar13 = MeWrite(iVar24,buffer,
                         (int)((int)puVar18 +
                              ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer)));
      }
      sprintf(buffer,"%d",pMVar4->frameNumber,iVar13);
      puVar18 = (uint *)buffer;
      do {
        puVar20 = puVar18;
        puVar18 = puVar20 + 1;
        uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
        uVar23 = uVar22 & 0x80808080;
      } while (uVar23 == 0);
      if ((uVar22 & 0x8080) == 0) {
        uVar23 = uVar23 >> 0x10;
        puVar18 = (uint *)((int)puVar20 + 6);
      }
      MeWrite(iVar24,buffer,
              (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
      p_Var3 = pwVar7;
      fVar8 = output.jaggedness;
      for (pMVar2 = pMVar4->firstfsi; output.jaggedness = fVar8,
          pMVar2 != (MeProfileFrameSectionInfo *)0x0; pMVar2 = pMVar2->next) {
        uVar26 = __fixunssfdi(fVar8);
        uVar22 = (uint)pMVar2->cpuCycles;
        iVar13 = *(int *)((int)&pMVar2->cpuCycles + 4);
        lVar5 = (uVar26 & 0xffffffff) * (ulonglong)uVar22;
        uVar16 = (uint)lVar5;
        uVar27 = __fixunssfdi(1.0 - fVar8);
        uVar23 = (uint)p_Var3->cpuCycles;
        lVar6 = (ulonglong)uVar23 * (uVar27 & 0xffffffff);
        uVar17 = (uint)lVar6;
        iVar15 = uVar16 + uVar17;
        iVar13 = (int)(uVar26 >> 0x20) * uVar22 +
                 (int)((ulonglong)lVar5 >> 0x20) + (int)uVar26 * iVar13 +
                 uVar23 * (int)(uVar27 >> 0x20) +
                 (int)((ulonglong)lVar6 >> 0x20) +
                 *(int *)((int)&p_Var3->cpuCycles + 4) * (int)uVar27 + (uint)CARRY4(uVar16,uVar17);
        *(int *)&p_Var3->cpuCycles = iVar15;
        *(int *)((int)&p_Var3->cpuCycles + 4) = iVar13;
        sprintf(buffer," %llu",iVar15,iVar13);
        puVar18 = (uint *)buffer;
        do {
          puVar20 = puVar18;
          puVar18 = puVar20 + 1;
          uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
          uVar23 = uVar22 & 0x80808080;
        } while (uVar23 == 0);
        if ((uVar22 & 0x8080) == 0) {
          uVar23 = uVar23 >> 0x10;
          puVar18 = (uint *)((int)puVar20 + 6);
        }
        MeWrite(iVar24,buffer,
                (int)((int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer))
               );
        fVar8 = output.jaggedness;
        uVar26 = __fixunssfdi(output.jaggedness);
        uVar22 = pMVar2->timesCalled;
        lVar5 = (uVar26 & 0xffffffff) * (ulonglong)uVar22;
        uVar16 = (uint)lVar5;
        uVar27 = __fixunssfdi(1.0 - fVar8);
        uVar23 = (uint)p_Var3->timesCalled;
        lVar6 = (ulonglong)uVar23 * (uVar27 & 0xffffffff);
        uVar17 = (uint)lVar6;
        iVar15 = uVar16 + uVar17;
        iVar13 = (int)(uVar26 >> 0x20) * uVar22 + (int)((ulonglong)lVar5 >> 0x20) +
                 uVar23 * (int)(uVar27 >> 0x20) +
                 (int)((ulonglong)lVar6 >> 0x20) +
                 *(int *)((int)&p_Var3->timesCalled + 4) * (int)uVar27 + (uint)CARRY4(uVar16,uVar17)
        ;
        *(int *)&p_Var3->timesCalled = iVar15;
        *(int *)((int)&p_Var3->timesCalled + 4) = iVar13;
        sprintf(buffer," %llu",iVar15,iVar13);
        puVar18 = (uint *)buffer;
        do {
          puVar20 = puVar18;
          puVar18 = puVar20 + 1;
          uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
          uVar23 = uVar22 & 0x80808080;
        } while (uVar23 == 0);
        if ((uVar22 & 0x8080) == 0) {
          uVar23 = uVar23 >> 0x10;
          puVar18 = (uint *)((int)puVar20 + 6);
        }
        MeWrite(iVar24,buffer,
                (int)((int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer))
               );
        fVar8 = output.jaggedness;
        uVar26 = __fixunssfdi(output.jaggedness);
        uVar22 = (uint)pMVar2->count0;
        iVar13 = *(int *)((int)&pMVar2->count0 + 4);
        lVar5 = (uVar26 & 0xffffffff) * (ulonglong)uVar22;
        uVar16 = (uint)lVar5;
        uVar27 = __fixunssfdi(1.0 - fVar8);
        uVar23 = (uint)p_Var3->count0;
        lVar6 = (ulonglong)uVar23 * (uVar27 & 0xffffffff);
        uVar17 = (uint)lVar6;
        iVar15 = uVar16 + uVar17;
        iVar13 = (int)(uVar26 >> 0x20) * uVar22 +
                 (int)((ulonglong)lVar5 >> 0x20) + (int)uVar26 * iVar13 +
                 uVar23 * (int)(uVar27 >> 0x20) +
                 (int)((ulonglong)lVar6 >> 0x20) + *(int *)((int)&p_Var3->count0 + 4) * (int)uVar27
                 + (uint)CARRY4(uVar16,uVar17);
        *(int *)&p_Var3->count0 = iVar15;
        *(int *)((int)&p_Var3->count0 + 4) = iVar13;
        sprintf(buffer," %llu",iVar15,iVar13);
        puVar18 = (uint *)buffer;
        do {
          puVar20 = puVar18;
          puVar18 = puVar20 + 1;
          uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
          uVar23 = uVar22 & 0x80808080;
        } while (uVar23 == 0);
        if ((uVar22 & 0x8080) == 0) {
          uVar23 = uVar23 >> 0x10;
          puVar18 = (uint *)((int)puVar20 + 6);
        }
        MeWrite(iVar24,buffer,
                (int)((int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer))
               );
        fVar8 = output.jaggedness;
        uVar26 = __fixunssfdi(output.jaggedness);
        uVar22 = (uint)pMVar2->count1;
        iVar13 = *(int *)((int)&pMVar2->count1 + 4);
        lVar5 = (uVar26 & 0xffffffff) * (ulonglong)uVar22;
        uVar16 = (uint)lVar5;
        uVar27 = __fixunssfdi(1.0 - fVar8);
        uVar23 = (uint)p_Var3->count1;
        lVar6 = (ulonglong)uVar23 * (uVar27 & 0xffffffff);
        uVar17 = (uint)lVar6;
        iVar15 = uVar16 + uVar17;
        iVar13 = (int)(uVar26 >> 0x20) * uVar22 +
                 (int)((ulonglong)lVar5 >> 0x20) + (int)uVar26 * iVar13 +
                 uVar23 * (int)(uVar27 >> 0x20) +
                 (int)((ulonglong)lVar6 >> 0x20) + *(int *)((int)&p_Var3->count1 + 4) * (int)uVar27
                 + (uint)CARRY4(uVar16,uVar17);
        *(int *)&p_Var3->count1 = iVar15;
        *(int *)((int)&p_Var3->count1 + 4) = iVar13;
        sprintf(buffer," %llu",iVar15,iVar13);
        puVar18 = (uint *)buffer;
        do {
          puVar20 = puVar18;
          puVar18 = puVar20 + 1;
          uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
          uVar23 = uVar22 & 0x80808080;
        } while (uVar23 == 0);
        if ((uVar22 & 0x8080) == 0) {
          uVar23 = uVar23 >> 0x10;
          puVar18 = (uint *)((int)puVar20 + 6);
        }
        MeWrite(iVar24,buffer,
                (int)((int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer))
               );
        p_Var3 = p_Var3->next;
        fVar8 = output.jaggedness;
      }
      uVar26 = __fixunssfdi(fVar8);
      uVar22 = (uint)(pMVar4->timings).cpuCycles;
      iVar13 = *(int *)((int)&(pMVar4->timings).cpuCycles + 4);
      lVar5 = (uVar26 & 0xffffffff) * (ulonglong)uVar22;
      uVar16 = (uint)lVar5;
      uVar27 = __fixunssfdi(1.0 - fVar8);
      uVar23 = (uint)p_Var3->cpuCycles;
      lVar6 = (ulonglong)uVar23 * (uVar27 & 0xffffffff);
      uVar17 = (uint)lVar6;
      iVar15 = uVar16 + uVar17;
      iVar13 = (int)(uVar26 >> 0x20) * uVar22 +
               (int)((ulonglong)lVar5 >> 0x20) + (int)uVar26 * iVar13 +
               uVar23 * (int)(uVar27 >> 0x20) +
               (int)((ulonglong)lVar6 >> 0x20) + *(int *)((int)&p_Var3->cpuCycles + 4) * (int)uVar27
               + (uint)CARRY4(uVar16,uVar17);
      *(int *)&p_Var12->cpuCycles = iVar15;
      *(int *)((int)&p_Var12->cpuCycles + 4) = iVar13;
      sprintf(buffer," %llu",iVar15,iVar13);
      puVar18 = (uint *)buffer;
      do {
        puVar20 = puVar18;
        puVar18 = puVar20 + 1;
        uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
        uVar23 = uVar22 & 0x80808080;
      } while (uVar23 == 0);
      if ((uVar22 & 0x8080) == 0) {
        uVar23 = uVar23 >> 0x10;
        puVar18 = (uint *)((int)puVar20 + 6);
      }
      MeWrite(iVar24,buffer,
              (int)((int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer)));
      fVar8 = output.jaggedness;
      uVar26 = __fixunssfdi(output.jaggedness);
      uVar22 = (uint)(pMVar4->timings).count0;
      iVar13 = *(int *)((int)&(pMVar4->timings).count0 + 4);
      lVar5 = (uVar26 & 0xffffffff) * (ulonglong)uVar22;
      uVar16 = (uint)lVar5;
      uVar27 = __fixunssfdi(1.0 - fVar8);
      uVar23 = (uint)p_Var3->count0;
      lVar6 = (ulonglong)uVar23 * (uVar27 & 0xffffffff);
      uVar17 = (uint)lVar6;
      iVar15 = uVar16 + uVar17;
      iVar13 = (int)(uVar26 >> 0x20) * uVar22 +
               (int)((ulonglong)lVar5 >> 0x20) + (int)uVar26 * iVar13 +
               uVar23 * (int)(uVar27 >> 0x20) +
               (int)((ulonglong)lVar6 >> 0x20) + *(int *)((int)&p_Var3->count0 + 4) * (int)uVar27 +
               (uint)CARRY4(uVar16,uVar17);
      *(int *)&p_Var12->count0 = iVar15;
      *(int *)((int)&p_Var12->count0 + 4) = iVar13;
      sprintf(buffer," %llu",iVar15,iVar13);
      puVar18 = (uint *)buffer;
      do {
        puVar20 = puVar18;
        puVar18 = puVar20 + 1;
        uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
        uVar23 = uVar22 & 0x80808080;
      } while (uVar23 == 0);
      if ((uVar22 & 0x8080) == 0) {
        uVar23 = uVar23 >> 0x10;
        puVar18 = (uint *)((int)puVar20 + 6);
      }
      MeWrite(iVar24,buffer,
              (int)((int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer)));
      fVar8 = output.jaggedness;
      uVar26 = __fixunssfdi(output.jaggedness);
      uVar22 = (uint)(pMVar4->timings).count1;
      iVar13 = *(int *)((int)&(pMVar4->timings).count1 + 4);
      lVar5 = (uVar26 & 0xffffffff) * (ulonglong)uVar22;
      uVar16 = (uint)lVar5;
      uVar27 = __fixunssfdi(1.0 - fVar8);
      uVar23 = (uint)p_Var3->count1;
      lVar6 = (ulonglong)uVar23 * (uVar27 & 0xffffffff);
      uVar17 = (uint)lVar6;
      iVar15 = uVar16 + uVar17;
      iVar13 = (int)(uVar26 >> 0x20) * uVar22 +
               (int)((ulonglong)lVar5 >> 0x20) + (int)uVar26 * iVar13 +
               uVar23 * (int)(uVar27 >> 0x20) +
               (int)((ulonglong)lVar6 >> 0x20) + *(int *)((int)&p_Var3->count1 + 4) * (int)uVar27 +
               (uint)CARRY4(uVar16,uVar17);
      *(int *)&p_Var12->count1 = iVar15;
      *(int *)((int)&p_Var12->count1 + 4) = iVar13;
      sprintf(buffer," %llu",iVar15,iVar13);
      puVar18 = (uint *)buffer;
      do {
        puVar20 = puVar18;
        puVar18 = puVar20 + 1;
        uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
        uVar23 = uVar22 & 0x80808080;
      } while (uVar23 == 0);
      if ((uVar22 & 0x8080) == 0) {
        uVar23 = uVar23 >> 0x10;
        puVar18 = (uint *)((int)puVar20 + 6);
      }
      MeWrite(iVar24,buffer,
              (int)((int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer)));
      sprintf(buffer,"");
      puVar18 = (uint *)buffer;
      do {
        puVar20 = puVar18;
        puVar18 = puVar20 + 1;
        uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
        uVar23 = uVar22 & 0x80808080;
      } while (uVar23 == 0);
      if ((uVar22 & 0x8080) == 0) {
        uVar23 = uVar23 >> 0x10;
        puVar18 = (uint *)((int)puVar20 + 6);
      }
      MeWrite(iVar24,buffer,
              (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
      uVar22 = (uint)(pMVar4->timings).cpuCycles;
      bVar25 = CARRY4((uint)cpuCycleAcc,uVar22);
      cpuCycleAcc._0_4_ = (uint)cpuCycleAcc + uVar22;
      cpuCycleAcc._4_4_ =
           cpuCycleAcc._4_4_ + *(int *)((int)&(pMVar4->timings).cpuCycles + 4) + (uint)bVar25;
      uVar22 = (uint)(pMVar4->timings).count0;
      bVar25 = CARRY4((uint)count0Acc,uVar22);
      count0Acc._0_4_ = (uint)count0Acc + uVar22;
      count0Acc._4_4_ =
           count0Acc._4_4_ + *(int *)((int)&(pMVar4->timings).count0 + 4) + (uint)bVar25;
      uVar23 = (uint)(pMVar4->timings).count1;
      bVar25 = CARRY4((uint)count1Acc,uVar23);
      count1Acc._0_4_ = (uint)count1Acc + uVar23;
      count1Acc._4_4_ =
           count1Acc._4_4_ + *(int *)((int)&(pMVar4->timings).count1 + 4) + (uint)bVar25;
      frames = (ushort)pMVar4->frameNumber;
      iVar13 = CONCAT22((short)(uVar22 >> 0x10),frames);
    }
    uVar28 = __udivdi3((uint)count1Acc,count1Acc._4_4_,frames,0);
    uVar28 = __udivdi3((uint)count0Acc,count0Acc._4_4_,frames,0,(uint)count1Acc,(uint)count1Acc,
                       uVar28);
    uVar28 = __udivdi3((uint)cpuCycleAcc,cpuCycleAcc._4_4_,frames,0,uVar28);
    sprintf(buffer,"#Averages:\n#CPU Cycles: %llu\n#Count 0: %llu\n#Count 1: %llu\n",(int)uVar28,
            (int)((ulonglong)uVar28 >> 0x20));
    puVar18 = (uint *)buffer;
    do {
      puVar20 = puVar18;
      puVar18 = puVar20 + 1;
      uVar22 = *puVar20 + 0xfefefeff & (*puVar20 ^ 0xffffffff);
      uVar23 = uVar22 & 0x80808080;
    } while (uVar23 == 0);
    if ((uVar22 & 0x8080) == 0) {
      uVar23 = uVar23 >> 0x10;
      puVar18 = (uint *)((int)puVar20 + 6);
    }
    MeWrite(iVar24,buffer,
            (int)puVar18 + ((-3 - (uint)CARRY1((byte)uVar23,(byte)uVar23)) - (int)buffer));
    MeClose(iVar24);
  }
  else {
    MeWarning(0xc,"MeProfile: Unkown output style");
  }
  return;
}


/* ==== MeProfileStartTiming ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeProfileStartTiming(MeProfileTimerMode mode,MeProfileLogModes_enum log)

{
  MeProfileTimerMode *pMVar1;
  
  firstt = (MeProfileTimer *)0x0;
  logging = log;
  firstfd = (MeProfileFrameData *)0x0;
  output.jaggedness = 0.2;
  output.settletime = 10;
  output.style = kMeProfileOutputEvent;
  pMVar1 = (MeProfileTimerMode *)(*_MeMemoryAPI)(0x10);
  HWTMode = pMVar1;
  pMVar1->granularity = mode.granularity;
  pMVar1->counterMode = mode.counterMode;
  pMVar1->count0Label = mode.count0Label;
  pMVar1->count1Label = mode.count1Label;
  if (mode.count0Label == (char **)0x0) {
    HWTMode->count0Label = &count0Label;
  }
  if (mode.count1Label == (char **)0x0) {
    HWTMode->count1Label = &count1Label;
  }
  MeProfileCreateHardwareTimer((kd_agg16)*HWTMode);
  GetOutputInfo();
  timingRunning = '\x01';
  return;
}


/* ==== MeProfileStartFrame ==== */

void MeProfileStartFrame(void)

{
  MeProfileTimer *pMVar1;
  void *in_stack_00000004;
  
                    /* Unresolved local var: MeProfileTimer * itemt@[DW_OP_reg0(EAX)] */
  pMVar1 = firstt;
  if (firstfd != (MeProfileFrameData *)0x0) {
    for (; pMVar1 != (MeProfileTimer *)0x0; pMVar1 = pMVar1->next) {
      pMVar1->timesCalled = 0;
      *(undefined4 *)&pMVar1->cpuCycles = 0;
      *(undefined4 *)((int)&pMVar1->cpuCycles + 4) = 0;
      *(undefined4 *)&pMVar1->count0 = 0;
      *(undefined4 *)((int)&pMVar1->count0 + 4) = 0;
      *(undefined4 *)&pMVar1->count1 = 0;
      *(undefined4 *)((int)&pMVar1->count1 + 4) = 0;
    }
  }
  if (insideFrame != '\0') {
    MeWarning(3,&DAT_00013585);
  }
  insideFrame = '\x01';
  MeProfileStartHardwareTimer(in_stack_00000004);
  return;
}


/* ==== MeProfileGetClockSpeed ==== */

MeU64 MeProfileGetClockSpeed(void)

{
  return CONCAT44(clockSpeed._4_4_,(undefined4)clockSpeed);
}


/* ==== MeProfileGetAllSectionTime ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeReal MeProfileGetAllSectionTime(void)

{
  MeProfileFrameSectionInfo *pMVar1;
  float fVar2;
  MeProfileFrameData *pMVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  bool bVar7;
  longdouble lVar8;
  longlong lVar9;
  
                    /* Unresolved local var: MeProfileFrameData * thisfd@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeProfileFrameSectionInfo * thisfsi@[DW_OP_reg1(ECX)]
                       Unresolved local var: uint nFrameDatas@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeU64 sectionsTotal@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeU64 avSections@[???]
                       Unresolved local var: MeReal avSectionsTimeInSecs@[???] */
  iVar4 = 0;
  uVar5 = 0;
  fVar2 = 0.0;
  if (firstfd != (MeProfileFrameData *)0x0) {
    iVar6 = 0;
    pMVar3 = firstfd;
    do {
      pMVar3 = pMVar3->next;
      iVar4 = iVar4 + 1;
    } while (pMVar3 != (MeProfileFrameData *)0x0);
    if (firstfd != (MeProfileFrameData *)0x0) {
      iVar6 = 0;
      pMVar3 = firstfd;
      do {
        for (pMVar1 = pMVar3->firstfsi; pMVar1 != (MeProfileFrameSectionInfo *)0x0;
            pMVar1 = pMVar1->next) {
          bVar7 = CARRY4(uVar5,(uint)pMVar1->cpuCycles);
          uVar5 = uVar5 + (int)pMVar1->cpuCycles;
          iVar6 = iVar6 + *(int *)((int)&pMVar1->cpuCycles + 4) + (uint)bVar7;
        }
        pMVar3 = pMVar3->next;
      } while (pMVar3 != (MeProfileFrameData *)0x0);
    }
    lVar9 = __udivdi3(uVar5,iVar6,iVar4,0);
    lVar8 = (longdouble)lVar9;
    if (lVar9 < 0) {
      lVar8 = _DAT_000136d0 + lVar8;
    }
    fVar2 = ((float)lVar8 / (float)clockSpeed) * 1000.0;
  }
  return fVar2;
}


/* ==== MeProfileStopTimers ==== */

void MeProfileStopTimers(void)

{
  MeProfileTimer *pMVar1;
  
  for (pMVar1 = firstt; pMVar1 != (MeProfileTimer *)0x0; pMVar1 = pMVar1->next) {
    if ((pMVar1->autoStop != '\0') && (pMVar1->isRunning != '\0')) {
      MeProfileEndSectionFn(pMVar1->codeSection);
    }
  }
  return;
}


/* ==== MeProfileSetOutputParameters ==== */

void MeProfileSetOutputParameters(MeProfileOutput p)

{
  output.settletime = p.settletime;
  output.jaggedness = p.jaggedness;
  output.style = p.style;
  return;
}


