/* ==== MeProfileGetTimerValue ==== */

void MeProfileGetTimerValue(MeProfileTimerResult *result)

{
  undefined8 uVar1;
  
                    /* Unresolved local var: MeU32 eax@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeU32 edx@[DW_OP_reg2(EDX)] */
  uVar1 = rdtsc();
  *(int *)&result->cpuCycles = (int)uVar1;
  *(int *)((int)&result->cpuCycles + 4) = (int)((ulonglong)uVar1 >> 0x20);
  *(undefined4 *)&result->count0 = 0;
  *(undefined4 *)((int)&result->count0 + 4) = 0;
  *(undefined4 *)&result->count1 = 0;
  *(undefined4 *)((int)&result->count1 + 4) = 0;
  return;
}


/* ==== MeProfileStartHardwareTimer ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int MeProfileStartHardwareTimer(MeProfileTimerMode *mode)

{
  undefined8 uVar1;
  
                    /* Unresolved local var: MeU32 eax@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeU32 edx@[DW_OP_reg2(EDX)] */
  uVar1 = rdtsc();
  _frameTime = (int)uVar1;
  _select = (int)((ulonglong)uVar1 >> 0x20);
  _clockSpeed = 0;
  _DAT_0001100c = 0;
  _DAT_00011010 = 0;
  _DAT_00011014 = 0;
  return 0;
}


/* ==== MeProfileStopHardwareTimer ==== */

int MeProfileStopHardwareTimer(void)

{
  return 0;
}


/* ==== MeProfileCreateHardwareTimer ==== */

int MeProfileCreateHardwareTimer(MeProfileTimerMode mode)

{
  *mode.count0Label = "N.A.";
  *mode.count1Label = "N.A.";
  return 0;
}


/* ==== MeProfileDestroyHardwareTimer ==== */

int MeProfileDestroyHardwareTimer(void)

{
  return 0;
}


/* ==== GetOutputInfo ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void GetOutputInfo(void)

{
  undefined8 uVar1;
  undefined8 uVar2;
  timeval t;
  MeProfileTimerResult finish;
  MeProfileTimerResult start;
  
  t.tv_sec = 1;
  t.tv_usec = 0;
                    /* Unresolved local var: MeU32 eax@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeU32 edx@[DW_OP_reg2(EDX)] */
  uVar1 = rdtsc();
  select(0,(fd_set *)0x0,(fd_set *)0x0,(fd_set *)0x0,(timeval *)&t);
                    /* Unresolved local var: MeU32 eax@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeU32 edx@[DW_OP_reg2(EDX)] */
  uVar2 = rdtsc();
  _clockSpeed = (uint)uVar2 - (uint)uVar1;
  _DAT_0001100c =
       ((int)((ulonglong)uVar2 >> 0x20) - (int)((ulonglong)uVar1 >> 0x20)) -
       (uint)((uint)uVar2 < (uint)uVar1);
  return;
}


