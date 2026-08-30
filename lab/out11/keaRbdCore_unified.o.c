/* ==== MdtKeaAddConstraintForces ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MdtKeaAddConstraintForces
               (MdtKeaConstraints pconstraints,MdtKeaBody **blist,MdtKeaTransformation *tlist,
               int num_bodies,MdtKeaParameters parameters)

{
  kd_agg92 kVar1;
  kd_agg92 kVar2;
  kd_agg76 kVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  MdtKeaParameters *pMVar11;
  undefined4 *puVar12;
  int *piVar13;
  MeReal *pMVar14;
  undefined4 *puVar15;
  MdtKeaConstraints *pMVar16;
  undefined1 in_stack_fffffdd8 [72];
  undefined4 in_stack_fffffe20;
  undefined4 in_stack_fffffe24;
  undefined4 in_stack_fffffe28;
  undefined8 in_stack_fffffe2c;
  undefined1 in_stack_fffffe34 [28];
  undefined4 in_stack_fffffe50;
  undefined4 in_stack_fffffe54;
  undefined4 in_stack_fffffe58;
  undefined4 in_stack_fffffe5c;
  undefined4 in_stack_fffffe60;
  undefined4 in_stack_fffffe64;
  undefined4 in_stack_fffffe68;
  undefined4 in_stack_fffffe6c;
  undefined4 in_stack_fffffe70;
  undefined4 in_stack_fffffe74;
  undefined4 in_stack_fffffe78;
  undefined4 in_stack_fffffe7c;
  MdtKeaBody **ppMVar17;
  undefined4 uVar18;
  keaFunctions_Vanilla *pkVar19;
  int ceil3_num_strips;
  int num_strips;
  MeReal *rhs;
  MdtKeaJBlockPair *jm;
  int (*bl2body_12padded) [8];
  int *jlen_12padded;
  keaMatrix_pcSparse_vanilla vanillaAMatrix;
  keaLCPSolver kSolver;
  keaFunctions_Vanilla vanillaFunctions;
  MdtKeaConstraints constraints;
  keaTempMemory mem;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: keaFunctions * keaFuncs@[???] */
  _vanillaFunctions = _ZN12keaFunctions8initPoolEPvi;
  if (num_bodies != 0) {
    gDebug = &parameters.debug;
    pMVar11 = &parameters;
    pMVar14 = (MeReal *)&stack0xfffffe34;
    ppMVar17 = blist;
    iVar9 = num_bodies;
    for (iVar8 = 0x13; iVar8 != 0; iVar8 = iVar8 + -1) {
      *pMVar14 = pMVar11->stepsize;
      pMVar11 = (MdtKeaParameters *)&pMVar11->epsilon;
      pMVar14 = pMVar14 + 1;
    }
    puVar15 = (undefined4 *)&stack0xfffffdd8;
    puVar12 = (undefined4 *)register0x00000010;
    for (iVar8 = 0x17; puVar12 = puVar12 + 1, iVar8 != 0; iVar8 = iVar8 + -1) {
      *puVar15 = *puVar12;
      puVar15 = puVar15 + 1;
    }
    kVar1._kd[0x48] = (char)in_stack_fffffe20;
    kVar1._kd[0x49] = (char)((uint)in_stack_fffffe20 >> 8);
    kVar1._kd[0x4a] = (char)((uint)in_stack_fffffe20 >> 0x10);
    kVar1._kd[0x4b] = (char)((uint)in_stack_fffffe20 >> 0x18);
    kVar1._kd[0] = in_stack_fffffdd8[0];
    kVar1._kd[1] = in_stack_fffffdd8[1];
    kVar1._kd[2] = in_stack_fffffdd8[2];
    kVar1._kd[3] = in_stack_fffffdd8[3];
    kVar1._kd[4] = in_stack_fffffdd8[4];
    kVar1._kd[5] = in_stack_fffffdd8[5];
    kVar1._kd[6] = in_stack_fffffdd8[6];
    kVar1._kd[7] = in_stack_fffffdd8[7];
    kVar1._kd[8] = in_stack_fffffdd8[8];
    kVar1._kd[9] = in_stack_fffffdd8[9];
    kVar1._kd[10] = in_stack_fffffdd8[10];
    kVar1._kd[0xb] = in_stack_fffffdd8[0xb];
    kVar1._kd[0xc] = in_stack_fffffdd8[0xc];
    kVar1._kd[0xd] = in_stack_fffffdd8[0xd];
    kVar1._kd[0xe] = in_stack_fffffdd8[0xe];
    kVar1._kd[0xf] = in_stack_fffffdd8[0xf];
    kVar1._kd[0x10] = in_stack_fffffdd8[0x10];
    kVar1._kd[0x11] = in_stack_fffffdd8[0x11];
    kVar1._kd[0x12] = in_stack_fffffdd8[0x12];
    kVar1._kd[0x13] = in_stack_fffffdd8[0x13];
    kVar1._kd[0x14] = in_stack_fffffdd8[0x14];
    kVar1._kd[0x15] = in_stack_fffffdd8[0x15];
    kVar1._kd[0x16] = in_stack_fffffdd8[0x16];
    kVar1._kd[0x17] = in_stack_fffffdd8[0x17];
    kVar1._kd[0x18] = in_stack_fffffdd8[0x18];
    kVar1._kd[0x19] = in_stack_fffffdd8[0x19];
    kVar1._kd[0x1a] = in_stack_fffffdd8[0x1a];
    kVar1._kd[0x1b] = in_stack_fffffdd8[0x1b];
    kVar1._kd[0x1c] = in_stack_fffffdd8[0x1c];
    kVar1._kd[0x1d] = in_stack_fffffdd8[0x1d];
    kVar1._kd[0x1e] = in_stack_fffffdd8[0x1e];
    kVar1._kd[0x1f] = in_stack_fffffdd8[0x1f];
    kVar1._kd[0x20] = in_stack_fffffdd8[0x20];
    kVar1._kd[0x21] = in_stack_fffffdd8[0x21];
    kVar1._kd[0x22] = in_stack_fffffdd8[0x22];
    kVar1._kd[0x23] = in_stack_fffffdd8[0x23];
    kVar1._kd[0x24] = in_stack_fffffdd8[0x24];
    kVar1._kd[0x25] = in_stack_fffffdd8[0x25];
    kVar1._kd[0x26] = in_stack_fffffdd8[0x26];
    kVar1._kd[0x27] = in_stack_fffffdd8[0x27];
    kVar1._kd[0x28] = in_stack_fffffdd8[0x28];
    kVar1._kd[0x29] = in_stack_fffffdd8[0x29];
    kVar1._kd[0x2a] = in_stack_fffffdd8[0x2a];
    kVar1._kd[0x2b] = in_stack_fffffdd8[0x2b];
    kVar1._kd[0x2c] = in_stack_fffffdd8[0x2c];
    kVar1._kd[0x2d] = in_stack_fffffdd8[0x2d];
    kVar1._kd[0x2e] = in_stack_fffffdd8[0x2e];
    kVar1._kd[0x2f] = in_stack_fffffdd8[0x2f];
    kVar1._kd[0x30] = in_stack_fffffdd8[0x30];
    kVar1._kd[0x31] = in_stack_fffffdd8[0x31];
    kVar1._kd[0x32] = in_stack_fffffdd8[0x32];
    kVar1._kd[0x33] = in_stack_fffffdd8[0x33];
    kVar1._kd[0x34] = in_stack_fffffdd8[0x34];
    kVar1._kd[0x35] = in_stack_fffffdd8[0x35];
    kVar1._kd[0x36] = in_stack_fffffdd8[0x36];
    kVar1._kd[0x37] = in_stack_fffffdd8[0x37];
    kVar1._kd[0x38] = in_stack_fffffdd8[0x38];
    kVar1._kd[0x39] = in_stack_fffffdd8[0x39];
    kVar1._kd[0x3a] = in_stack_fffffdd8[0x3a];
    kVar1._kd[0x3b] = in_stack_fffffdd8[0x3b];
    kVar1._kd[0x3c] = in_stack_fffffdd8[0x3c];
    kVar1._kd[0x3d] = in_stack_fffffdd8[0x3d];
    kVar1._kd[0x3e] = in_stack_fffffdd8[0x3e];
    kVar1._kd[0x3f] = in_stack_fffffdd8[0x3f];
    kVar1._kd[0x40] = in_stack_fffffdd8[0x40];
    kVar1._kd[0x41] = in_stack_fffffdd8[0x41];
    kVar1._kd[0x42] = in_stack_fffffdd8[0x42];
    kVar1._kd[0x43] = in_stack_fffffdd8[0x43];
    kVar1._kd[0x44] = in_stack_fffffdd8[0x44];
    kVar1._kd[0x45] = in_stack_fffffdd8[0x45];
    kVar1._kd[0x46] = in_stack_fffffdd8[0x46];
    kVar1._kd[0x47] = in_stack_fffffdd8[0x47];
    kVar1._kd[0x4c] = (char)in_stack_fffffe24;
    kVar1._kd[0x4d] = (char)((uint)in_stack_fffffe24 >> 8);
    kVar1._kd[0x4e] = (char)((uint)in_stack_fffffe24 >> 0x10);
    kVar1._kd[0x4f] = (char)((uint)in_stack_fffffe24 >> 0x18);
    kVar1._kd[0x50] = (char)in_stack_fffffe28;
    kVar1._kd[0x51] = (char)((uint)in_stack_fffffe28 >> 8);
    kVar1._kd[0x52] = (char)((uint)in_stack_fffffe28 >> 0x10);
    kVar1._kd[0x53] = (char)((uint)in_stack_fffffe28 >> 0x18);
    kVar1._kd[0x54] = (char)in_stack_fffffe2c;
    kVar1._kd[0x55] = (char)((ulonglong)in_stack_fffffe2c >> 8);
    kVar1._kd[0x56] = (char)((ulonglong)in_stack_fffffe2c >> 0x10);
    kVar1._kd[0x57] = (char)((ulonglong)in_stack_fffffe2c >> 0x18);
    kVar1._kd[0x58] = (char)((ulonglong)in_stack_fffffe2c >> 0x20);
    kVar1._kd[0x59] = (char)((ulonglong)in_stack_fffffe2c >> 0x28);
    kVar1._kd[0x5a] = (char)((ulonglong)in_stack_fffffe2c >> 0x30);
    kVar1._kd[0x5b] = (char)((ulonglong)in_stack_fffffe2c >> 0x38);
    kVar3._kd[0x1c] = (char)in_stack_fffffe50;
    kVar3._kd[0x1d] = (char)((uint)in_stack_fffffe50 >> 8);
    kVar3._kd[0x1e] = (char)((uint)in_stack_fffffe50 >> 0x10);
    kVar3._kd[0x1f] = (char)((uint)in_stack_fffffe50 >> 0x18);
    kVar3._kd[0] = in_stack_fffffe34[0];
    kVar3._kd[1] = in_stack_fffffe34[1];
    kVar3._kd[2] = in_stack_fffffe34[2];
    kVar3._kd[3] = in_stack_fffffe34[3];
    kVar3._kd[4] = in_stack_fffffe34[4];
    kVar3._kd[5] = in_stack_fffffe34[5];
    kVar3._kd[6] = in_stack_fffffe34[6];
    kVar3._kd[7] = in_stack_fffffe34[7];
    kVar3._kd[8] = in_stack_fffffe34[8];
    kVar3._kd[9] = in_stack_fffffe34[9];
    kVar3._kd[10] = in_stack_fffffe34[10];
    kVar3._kd[0xb] = in_stack_fffffe34[0xb];
    kVar3._kd[0xc] = in_stack_fffffe34[0xc];
    kVar3._kd[0xd] = in_stack_fffffe34[0xd];
    kVar3._kd[0xe] = in_stack_fffffe34[0xe];
    kVar3._kd[0xf] = in_stack_fffffe34[0xf];
    kVar3._kd[0x10] = in_stack_fffffe34[0x10];
    kVar3._kd[0x11] = in_stack_fffffe34[0x11];
    kVar3._kd[0x12] = in_stack_fffffe34[0x12];
    kVar3._kd[0x13] = in_stack_fffffe34[0x13];
    kVar3._kd[0x14] = in_stack_fffffe34[0x14];
    kVar3._kd[0x15] = in_stack_fffffe34[0x15];
    kVar3._kd[0x16] = in_stack_fffffe34[0x16];
    kVar3._kd[0x17] = in_stack_fffffe34[0x17];
    kVar3._kd[0x18] = in_stack_fffffe34[0x18];
    kVar3._kd[0x19] = in_stack_fffffe34[0x19];
    kVar3._kd[0x1a] = in_stack_fffffe34[0x1a];
    kVar3._kd[0x1b] = in_stack_fffffe34[0x1b];
    kVar3._kd[0x20] = (char)in_stack_fffffe54;
    kVar3._kd[0x21] = (char)((uint)in_stack_fffffe54 >> 8);
    kVar3._kd[0x22] = (char)((uint)in_stack_fffffe54 >> 0x10);
    kVar3._kd[0x23] = (char)((uint)in_stack_fffffe54 >> 0x18);
    kVar3._kd[0x24] = (char)in_stack_fffffe58;
    kVar3._kd[0x25] = (char)((uint)in_stack_fffffe58 >> 8);
    kVar3._kd[0x26] = (char)((uint)in_stack_fffffe58 >> 0x10);
    kVar3._kd[0x27] = (char)((uint)in_stack_fffffe58 >> 0x18);
    kVar3._kd[0x28] = (char)in_stack_fffffe5c;
    kVar3._kd[0x29] = (char)((uint)in_stack_fffffe5c >> 8);
    kVar3._kd[0x2a] = (char)((uint)in_stack_fffffe5c >> 0x10);
    kVar3._kd[0x2b] = (char)((uint)in_stack_fffffe5c >> 0x18);
    kVar3._kd[0x2c] = (char)in_stack_fffffe60;
    kVar3._kd[0x2d] = (char)((uint)in_stack_fffffe60 >> 8);
    kVar3._kd[0x2e] = (char)((uint)in_stack_fffffe60 >> 0x10);
    kVar3._kd[0x2f] = (char)((uint)in_stack_fffffe60 >> 0x18);
    kVar3._kd[0x30] = (char)in_stack_fffffe64;
    kVar3._kd[0x31] = (char)((uint)in_stack_fffffe64 >> 8);
    kVar3._kd[0x32] = (char)((uint)in_stack_fffffe64 >> 0x10);
    kVar3._kd[0x33] = (char)((uint)in_stack_fffffe64 >> 0x18);
    kVar3._kd[0x34] = (char)in_stack_fffffe68;
    kVar3._kd[0x35] = (char)((uint)in_stack_fffffe68 >> 8);
    kVar3._kd[0x36] = (char)((uint)in_stack_fffffe68 >> 0x10);
    kVar3._kd[0x37] = (char)((uint)in_stack_fffffe68 >> 0x18);
    kVar3._kd[0x38] = (char)in_stack_fffffe6c;
    kVar3._kd[0x39] = (char)((uint)in_stack_fffffe6c >> 8);
    kVar3._kd[0x3a] = (char)((uint)in_stack_fffffe6c >> 0x10);
    kVar3._kd[0x3b] = (char)((uint)in_stack_fffffe6c >> 0x18);
    kVar3._kd[0x3c] = (char)in_stack_fffffe70;
    kVar3._kd[0x3d] = (char)((uint)in_stack_fffffe70 >> 8);
    kVar3._kd[0x3e] = (char)((uint)in_stack_fffffe70 >> 0x10);
    kVar3._kd[0x3f] = (char)((uint)in_stack_fffffe70 >> 0x18);
    kVar3._kd[0x40] = (char)in_stack_fffffe74;
    kVar3._kd[0x41] = (char)((uint)in_stack_fffffe74 >> 8);
    kVar3._kd[0x42] = (char)((uint)in_stack_fffffe74 >> 0x10);
    kVar3._kd[0x43] = (char)((uint)in_stack_fffffe74 >> 0x18);
    kVar3._kd[0x44] = (char)in_stack_fffffe78;
    kVar3._kd[0x45] = (char)((uint)in_stack_fffffe78 >> 8);
    kVar3._kd[0x46] = (char)((uint)in_stack_fffffe78 >> 0x10);
    kVar3._kd[0x47] = (char)((uint)in_stack_fffffe78 >> 0x18);
    kVar3._kd[0x48] = (char)in_stack_fffffe7c;
    kVar3._kd[0x49] = (char)((uint)in_stack_fffffe7c >> 8);
    kVar3._kd[0x4a] = (char)((uint)in_stack_fffffe7c >> 0x10);
    kVar3._kd[0x4b] = (char)((uint)in_stack_fffffe7c >> 0x18);
    gDebugDataFile =
         _ZN12keaFunctions20checkPrintDebugInputE17MdtKeaConstraints16MdtKeaParametersPKPK10MdtKeaBodyi
                   (&vanillaFunctions,kVar1,kVar3,ppMVar17,iVar9);
    pkVar19 = &vanillaFunctions;
    (**(code **)(_vanillaFunctions + 0x10))(&vanillaFunctions);
    uVar18 = 0x100b3;
    _ZN12keaFunctions8initPoolEPvi(pkVar19,parameters.memory_pool,parameters.memory_pool_size);
    puVar15 = (undefined4 *)&stack0xfffffe2c;
    puVar12 = (undefined4 *)register0x00000010;
    iVar9 = num_bodies;
    for (iVar8 = 0x17; puVar12 = puVar12 + 1, iVar8 != 0; iVar8 = iVar8 + -1) {
      *puVar15 = *puVar12;
      puVar15 = puVar15 + 1;
    }
    kVar2._kd[8] = in_stack_fffffe34[0];
    kVar2._kd[9] = in_stack_fffffe34[1];
    kVar2._kd[10] = in_stack_fffffe34[2];
    kVar2._kd[0xb] = in_stack_fffffe34[3];
    kVar2._kd[0xc] = in_stack_fffffe34[4];
    kVar2._kd[0xd] = in_stack_fffffe34[5];
    kVar2._kd[0xe] = in_stack_fffffe34[6];
    kVar2._kd[0xf] = in_stack_fffffe34[7];
    kVar2._kd[0x10] = in_stack_fffffe34[8];
    kVar2._kd[0x11] = in_stack_fffffe34[9];
    kVar2._kd[0x12] = in_stack_fffffe34[10];
    kVar2._kd[0x13] = in_stack_fffffe34[0xb];
    kVar2._kd[0x14] = in_stack_fffffe34[0xc];
    kVar2._kd[0x15] = in_stack_fffffe34[0xd];
    kVar2._kd[0x16] = in_stack_fffffe34[0xe];
    kVar2._kd[0x17] = in_stack_fffffe34[0xf];
    kVar2._kd[0x18] = in_stack_fffffe34[0x10];
    kVar2._kd[0x19] = in_stack_fffffe34[0x11];
    kVar2._kd[0x1a] = in_stack_fffffe34[0x12];
    kVar2._kd[0x1b] = in_stack_fffffe34[0x13];
    kVar2._kd[0x1c] = in_stack_fffffe34[0x14];
    kVar2._kd[0x1d] = in_stack_fffffe34[0x15];
    kVar2._kd[0x1e] = in_stack_fffffe34[0x16];
    kVar2._kd[0x1f] = in_stack_fffffe34[0x17];
    kVar2._kd[0x20] = in_stack_fffffe34[0x18];
    kVar2._kd[0x21] = in_stack_fffffe34[0x19];
    kVar2._kd[0x22] = in_stack_fffffe34[0x1a];
    kVar2._kd[0x23] = in_stack_fffffe34[0x1b];
    kVar2._kd[0] = (char)in_stack_fffffe2c;
    kVar2._kd[1] = (char)((ulonglong)in_stack_fffffe2c >> 8);
    kVar2._kd[2] = (char)((ulonglong)in_stack_fffffe2c >> 0x10);
    kVar2._kd[3] = (char)((ulonglong)in_stack_fffffe2c >> 0x18);
    kVar2._kd[4] = (char)((ulonglong)in_stack_fffffe2c >> 0x20);
    kVar2._kd[5] = (char)((ulonglong)in_stack_fffffe2c >> 0x28);
    kVar2._kd[6] = (char)((ulonglong)in_stack_fffffe2c >> 0x30);
    kVar2._kd[7] = (char)((ulonglong)in_stack_fffffe2c >> 0x38);
    kVar2._kd[0x24] = (char)in_stack_fffffe50;
    kVar2._kd[0x25] = (char)((uint)in_stack_fffffe50 >> 8);
    kVar2._kd[0x26] = (char)((uint)in_stack_fffffe50 >> 0x10);
    kVar2._kd[0x27] = (char)((uint)in_stack_fffffe50 >> 0x18);
    kVar2._kd[0x28] = (char)in_stack_fffffe54;
    kVar2._kd[0x29] = (char)((uint)in_stack_fffffe54 >> 8);
    kVar2._kd[0x2a] = (char)((uint)in_stack_fffffe54 >> 0x10);
    kVar2._kd[0x2b] = (char)((uint)in_stack_fffffe54 >> 0x18);
    kVar2._kd[0x2c] = (char)in_stack_fffffe58;
    kVar2._kd[0x2d] = (char)((uint)in_stack_fffffe58 >> 8);
    kVar2._kd[0x2e] = (char)((uint)in_stack_fffffe58 >> 0x10);
    kVar2._kd[0x2f] = (char)((uint)in_stack_fffffe58 >> 0x18);
    kVar2._kd[0x30] = (char)in_stack_fffffe5c;
    kVar2._kd[0x31] = (char)((uint)in_stack_fffffe5c >> 8);
    kVar2._kd[0x32] = (char)((uint)in_stack_fffffe5c >> 0x10);
    kVar2._kd[0x33] = (char)((uint)in_stack_fffffe5c >> 0x18);
    kVar2._kd[0x34] = (char)in_stack_fffffe60;
    kVar2._kd[0x35] = (char)((uint)in_stack_fffffe60 >> 8);
    kVar2._kd[0x36] = (char)((uint)in_stack_fffffe60 >> 0x10);
    kVar2._kd[0x37] = (char)((uint)in_stack_fffffe60 >> 0x18);
    kVar2._kd[0x38] = (char)in_stack_fffffe64;
    kVar2._kd[0x39] = (char)((uint)in_stack_fffffe64 >> 8);
    kVar2._kd[0x3a] = (char)((uint)in_stack_fffffe64 >> 0x10);
    kVar2._kd[0x3b] = (char)((uint)in_stack_fffffe64 >> 0x18);
    kVar2._kd[0x3c] = (char)in_stack_fffffe68;
    kVar2._kd[0x3d] = (char)((uint)in_stack_fffffe68 >> 8);
    kVar2._kd[0x3e] = (char)((uint)in_stack_fffffe68 >> 0x10);
    kVar2._kd[0x3f] = (char)((uint)in_stack_fffffe68 >> 0x18);
    kVar2._kd[0x40] = (char)in_stack_fffffe6c;
    kVar2._kd[0x41] = (char)((uint)in_stack_fffffe6c >> 8);
    kVar2._kd[0x42] = (char)((uint)in_stack_fffffe6c >> 0x10);
    kVar2._kd[0x43] = (char)((uint)in_stack_fffffe6c >> 0x18);
    kVar2._kd[0x44] = (char)in_stack_fffffe70;
    kVar2._kd[0x45] = (char)((uint)in_stack_fffffe70 >> 8);
    kVar2._kd[0x46] = (char)((uint)in_stack_fffffe70 >> 0x10);
    kVar2._kd[0x47] = (char)((uint)in_stack_fffffe70 >> 0x18);
    kVar2._kd[0x48] = (char)in_stack_fffffe74;
    kVar2._kd[0x49] = (char)((uint)in_stack_fffffe74 >> 8);
    kVar2._kd[0x4a] = (char)((uint)in_stack_fffffe74 >> 0x10);
    kVar2._kd[0x4b] = (char)((uint)in_stack_fffffe74 >> 0x18);
    kVar2._kd[0x4c] = (char)in_stack_fffffe78;
    kVar2._kd[0x4d] = (char)((uint)in_stack_fffffe78 >> 8);
    kVar2._kd[0x4e] = (char)((uint)in_stack_fffffe78 >> 0x10);
    kVar2._kd[0x4f] = (char)((uint)in_stack_fffffe78 >> 0x18);
    kVar2._kd[0x50] = (char)in_stack_fffffe7c;
    kVar2._kd[0x51] = (char)((uint)in_stack_fffffe7c >> 8);
    kVar2._kd[0x52] = (char)((uint)in_stack_fffffe7c >> 0x10);
    kVar2._kd[0x53] = (char)((uint)in_stack_fffffe7c >> 0x18);
    kVar2._kd[0x54] = (char)uVar18;
    kVar2._kd[0x55] = (char)((uint)uVar18 >> 8);
    kVar2._kd[0x56] = (char)((uint)uVar18 >> 0x10);
    kVar2._kd[0x57] = (char)((uint)uVar18 >> 0x18);
    kVar2._kd._88_4_ = pkVar19;
    (**(code **)(_vanillaFunctions + 0x14))(&vanillaFunctions,&mem,kVar2,iVar9);
    (**(code **)(_vanillaFunctions + 4))
              (&vanillaFunctions,mem.invIworld,mem.vhmf,blist,tlist,num_bodies,parameters.stepsize);
    _ZN12keaFunctions18makejlenandbl2bodyEPiS0_PA8_iS2_S2_PA2_KiPS3_S6_S6_S6_ii
              (&vanillaFunctions,mem.jlen_12padded,mem.jlen,mem.bl2body_12padded,mem.bl2body,
               mem.bl2cbody,pconstraints.Jbody,pconstraints.Jsize,
               pconstraints.num_rows_inc_padding_partition,
               pconstraints.num_rows_exc_padding_partition,pconstraints.num_constraints_partition,
               pconstraints.num_constraints,pconstraints.num_partitions);
    iVar9 = pconstraints.num_rows_exc_padding;
    if ((pconstraints.num_rows_exc_padding & 3U) != 0) {
      uVar10 = pconstraints.num_rows_exc_padding;
      if (pconstraints.num_rows_exc_padding < 0) {
        uVar10 = pconstraints.num_rows_exc_padding + 3;
      }
      iVar9 = (uVar10 & 0xfffffffc) + 4;
    }
    (**(code **)_vanillaFunctions)
              (&vanillaFunctions,mem.rhs,mem.jm,pconstraints.Jstore,pconstraints.xgamma,
               pconstraints.c,pconstraints.xi,mem.invIworld,mem.bl2body,mem.jlen,mem.vhmf,num_bodies
               ,iVar9,pconstraints.num_rows_inc_padding,parameters.stepsize,parameters.gamma);
    pMVar16 = &constraints;
    piVar13 = (int *)register0x00000010;
    for (iVar9 = 0x17; piVar13 = piVar13 + 1, iVar9 != 0; iVar9 = iVar9 + -1) {
      pMVar16->num_partitions = *piVar13;
      pMVar16 = (MdtKeaConstraints *)&pMVar16->max_partitions;
    }
    jlen_12padded = mem.jlen_12padded;
    bl2body_12padded = mem.bl2body_12padded;
    jm = mem.jm;
    rhs = mem.rhs;
    gPartition = 0;
    iVar9 = constraints.num_partitions;
    if (constraints.num_partitions != 0) {
      do {
        if (0 < constraints.num_constraints_partition[gPartition]) {
                    /* Unresolved local var: int num_constraints@[DW_OP_reg7(EDI)]
                       Unresolved local var: int num_rows@[DW_OP_reg3(EBX)]
                       Unresolved local var: int ceil4_num_rows@[DW_OP_reg6(ESI)]
                       Unresolved local var: int ceil12_num_rows@[DW_OP_reg2(EDX)] */
          if ((parameters.debug.writeKeaInterData != 0) && (gDebug->frame == gDebug->badFrame)) {
            writeIntToFile(gDebugDataFile,"partition",gPartition);
          }
          keaPushPoolFrame();
          uVar10 = constraints.num_rows_exc_padding_partition[gPartition];
          iVar9 = constraints.num_constraints_partition[gPartition];
          uVar4 = uVar10;
          if ((uVar10 & 3) != 0) {
            if ((int)uVar10 < 0) {
              uVar4 = uVar10 + 3;
            }
            uVar4 = (uVar4 & 0xfffffffc) + 4;
          }
          uVar7 = uVar10;
          if ((int)uVar10 % 0xc != 0) {
            uVar7 = (uVar10 - (int)uVar10 % 0xc) + 0xc;
          }
          uVar5 = uVar4;
          if ((int)uVar4 < 0) {
            uVar5 = uVar4 + 3;
          }
          if ((int)uVar7 < 0) {
            uVar7 = uVar7 + 3;
          }
                    /* Unresolved local var: keaMatrix * A@[???] */
          _vanillaAMatrix = _ZN12keaLCPSolver8solveLCPEP9keaMatrixPfS2_S2_iP15_MeCPUResourcesf;
          (*__ZN12keaLCPSolver8solveLCPEP9keaMatrixPfS2_S2_iP15_MeCPUResourcesf)();
          _ZN12keaLCPSolver8allocateEi(&kSolver,uVar10);
          (**(code **)(_vanillaAMatrix + 4))
                    (&vanillaAMatrix,jm,constraints.Jstore,jlen_12padded,bl2body_12padded,
                     constraints.slipfactor,parameters.epsilon,1.0 / parameters.stepsize);
          (**(code **)(_vanillaAMatrix + 0xc))(&vanillaAMatrix);
          (**(code **)(_vanillaAMatrix + 0x1c))(&vanillaAMatrix);
          uVar10 = 0;
          _ZN12keaLCPSolver8solveLCPEP9keaMatrixPfS2_S2_iP15_MeCPUResourcesf
                    (&kSolver,&vanillaAMatrix,rhs,constraints.lo,constraints.hi,
                     parameters.max_iterations,parameters.cpu_resources,parameters.velocityZeroTol);
          if (uVar4 != 0) {
            uVar6 = uVar4 & 3;
            if (uVar6 != 0) {
              if (1 < uVar6) {
                if (2 < uVar6) {
                  *constraints.lambda = *kSolver.x;
                }
                uVar10 = (uint)(2 < uVar6);
                constraints.lambda[uVar10] = kSolver.x[uVar10];
                uVar10 = uVar10 + 1;
              }
              constraints.lambda[uVar10] = kSolver.x[uVar10];
              uVar10 = uVar10 + 1;
              if (uVar10 == uVar4) goto LAB_0001040f;
            }
            do {
              constraints.lambda[uVar10] = kSolver.x[uVar10];
              constraints.lambda[uVar10 + 1] = kSolver.x[uVar10 + 1];
              constraints.lambda[uVar10 + 2] = kSolver.x[uVar10 + 2];
              iVar8 = uVar10 + 3;
              uVar10 = uVar10 + 4;
              constraints.lambda[iVar8] = kSolver.x[iVar8];
            } while (uVar10 != uVar4);
          }
LAB_0001040f:
          iVar8 = constraints.num_rows_inc_padding_partition[gPartition];
          if (iVar8 < 0) {
            iVar8 = iVar8 + 3;
          }
          rhs = rhs + ((int)uVar5 >> 2) * 4;
          jm = jm + (iVar8 >> 2);
          jlen_12padded = jlen_12padded + ((int)uVar7 >> 2);
          bl2body_12padded = bl2body_12padded + ((int)uVar7 >> 2);
          constraints.Jstore = constraints.Jstore + (iVar8 >> 2);
          constraints.Jbody = constraints.Jbody + iVar9;
          constraints.xi = constraints.xi + uVar4;
          constraints.lo = constraints.lo + uVar4;
          constraints.c = constraints.c + uVar4;
          constraints.slipfactor = constraints.slipfactor + uVar4;
          constraints.hi = constraints.hi + uVar4;
          constraints.lambda = constraints.lambda + uVar4;
          constraints.xgamma = constraints.xgamma + uVar4;
          constraints.Jsize = constraints.Jsize + iVar9;
          constraints.Jofs = constraints.Jofs + iVar9;
          constraints.force = constraints.force + iVar9;
          keaPopPoolFrame();
          iVar9 = constraints.num_partitions;
        }
        gPartition = gPartition + 1;
      } while (gPartition != iVar9);
    }
    (**(code **)(_vanillaFunctions + 8))
              (&vanillaFunctions,blist,pconstraints.force,pconstraints.Jstore,pconstraints.Jbody,
               pconstraints.lambda,mem.bl2body,mem.bl2cbody,mem.jlen,
               pconstraints.num_rows_exc_padding,pconstraints.num_rows_inc_padding,
               pconstraints.num_constraints,num_bodies);
    (**(code **)(_vanillaFunctions + 0xc))(&vanillaFunctions,blist,mem.invIworld,num_bodies);
    _ZN12keaFunctions21keaCloseDebugDataFileEi(&vanillaFunctions,gDebugDataFile);
  }
  return;
}


/* ==== MdtKeaQueryCPUResources ==== */

MeCPUResources MdtKeaQueryCPUResources(void)

{
  return (MeCPUResources)0x0;
}


