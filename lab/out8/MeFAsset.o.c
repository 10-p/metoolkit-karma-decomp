/* ==== MeFAssetCreateCopy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFAsset * MeFAssetCreateCopy(MeFAsset *asset,MeBool recurse)

{
  char *pcVar1;
  int iVar2;
  MeReal MVar3;
  MeFAssetPart **ppMVar4;
  MeFAsset *pMVar5;
  MeFGeometryNode *pMVar6;
  MeFModelNode *pMVar7;
  MeFAssetPartNode *pMVar8;
  MeFJointNode *pMVar9;
  MeHash *pMVar10;
  MeU32 *pMVar11;
  MeIDPool *pMVar12;
  MeFJoint *pMVar13;
  int iVar14;
  uint uVar15;
  MeFAssetPart *pMVar16;
  MeFAssetPart **ppMVar17;
  int iVar18;
  MeFModel *pMVar19;
  void *pvVar20;
  MeFGeometry *pMVar21;
  int extraout_EAX;
  int extraout_EAX_00;
  int extraout_EAX_01;
  int extraout_EAX_02;
  int extraout_EAX_03;
  int iVar22;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar23;
  undefined4 extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  uint uVar24;
  MeFAsset *pMVar25;
  MeFAsset *pMStackY_50;
  MeFAsset *pMStack_44;
  int iStack_40;
  MeFAssetPart *apMStack_3c [3];
  MeFAssetPart **local_30;
  MeFAssetPart **copyArray;
  int count;
  int i;
  MeFAsset *asset_1;
  MeFGeometryIt it_2;
  MeFModelIt it_1;
  MeFJointIt it;
  
                    /* Unresolved local var: MeFAsset * copy@[???] */
  pcVar1 = asset->name;
  iVar2 = asset->id;
  pMStackY_50 = (MeFAsset *)0x10019;
  asset_1 = (MeFAsset *)(*_MeHashCreate)();
  pMStackY_50 = (MeFAsset *)0x10029;
  pMVar6 = (MeFGeometryNode *)(*_MeMemoryAPI)();
  asset_1->nilGeometry = pMVar6;
  pMStackY_50 = (MeFAsset *)0x1003c;
  pMVar7 = (MeFModelNode *)(*_MeMemoryAPI)();
  asset_1->nilModel = pMVar7;
  pMStackY_50 = (MeFAsset *)0x1004f;
  pMVar8 = (MeFAssetPartNode *)(*_MeMemoryAPI)();
  asset_1->nilPart = pMVar8;
  pMStackY_50 = (MeFAsset *)0x10062;
  pMVar9 = (MeFJointNode *)(*_MeMemoryAPI)();
  asset_1->nilJoint = pMVar9;
  pMStackY_50 = (MeFAsset *)0x10074;
  pMVar10 = MeHashCreate(0x11);
  asset_1->nameToGeometry = pMVar10;
  pMStackY_50 = (MeFAsset *)0x10086;
  pMVar10 = MeHashCreate(0x11);
  asset_1->nameToModel = pMVar10;
  pMStackY_50 = (MeFAsset *)0x10098;
  pMVar10 = MeHashCreate(0x11);
  asset_1->nameToPart = pMVar10;
  pMStackY_50 = (MeFAsset *)0x100aa;
  pMVar10 = MeHashCreate(0x11);
  asset_1->nameToJoint = pMVar10;
  asset_1->maxParts = 0x100;
  pMStackY_50 = (MeFAsset *)0x100c4;
  pMVar11 = (MeU32 *)(*_MeHashCreate)();
  asset_1->disabledColArray = pMVar11;
  iStack_40 = 0x100d2;
  pMVar12 = MeIDPoolCreate();
  asset_1->disabledColIndexPool = pMVar12;
  pMStackY_50 = (MeFAsset *)0x100e1;
  _MeFAssetInit(asset_1);
  pMStackY_50 = (MeFAsset *)0x100ed;
  _FSetStringProperty(asset_1,pcVar1);
  asset_1->id = iVar2;
  pMStackY_50 = (MeFAsset *)0x10108;
  _FSetStringProperty(&asset_1->refPart,asset->refPart);
  pMStackY_50 = (MeFAsset *)0x1011d;
  _FSetStringProperty(&asset_1->graphicHint,asset->graphicHint);
  asset_1->graphicScale = asset->graphicScale;
  pMVar25 = (MeFAsset *)asset->massScale;
  MVar3 = asset->lengthScale;
  asset_1->massScale = (MeReal)pMVar25;
  asset_1->lengthScale = MVar3;
  if (recurse == 0) {
    return asset_1;
  }
                    /* Unresolved local var: MeFJoint * joint@[DW_OP_reg0(EAX)] */
  iStack_40 = (int)asset;
  pMStack_44 = asset;
  pMStackY_50 = (MeFAsset *)0x10165;
  MeFAssetInitJointIterator(asset,&it);
  while( true ) {
    pMStackY_50 = (MeFAsset *)0x1016d;
    pMVar13 = MeFAssetGetJoint(&it);
    if (pMVar13 == (MeFJoint *)0x0) break;
                    /* Unresolved local var: MeFJoint * copyJoint@[DW_OP_reg0(EAX)] */
    pMStackY_50 = (MeFAsset *)0x10180;
    pMStack_44 = pMVar25;
    iStack_40 = (int)pMVar25;
    pMVar13 = MeFJointCreateCopy(pMVar13,recurse);
    pMVar25 = asset_1;
    pMStackY_50 = (MeFAsset *)0x1018c;
    MeFAssetInsertJoint(asset_1,pMVar13);
  }
                    /* Unresolved local var: MeFAssetPart * * origArray@[DW_OP_reg7(EDI)] */
  pMStackY_50 = (MeFAsset *)0x1019a;
  iVar14 = MeFAssetGetPartCount(asset);
  count = iVar14;
  uVar24 = iVar14 * 4 + 0xfU & 0xfffffff0;
  iVar2 = -uVar24;
  copyArray = (MeFAssetPart **)((int)apMStack_3c + uVar24 * -2);
  *(int *)((int)apMStack_3c + uVar24 * -2 + -4) = iVar14;
  *(int *)((int)&stack0xffffffbc + uVar24 * -2) = iVar14;
  *(int *)(&stack0xffffffb8 + uVar24 * -2) = (int)apMStack_3c + iVar2;
  *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = asset;
  *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x101bf;
  MeFAssetGetPartsSortedByName
            (*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
             *(MeFAssetPart ***)(&stack0xffffffb8 + uVar24 * -2));
  i = 0;
  ppMVar17 = (MeFAssetPart **)count;
  if (count < 1) goto LAB_000103da;
  uVar15 = count & 3;
  pMVar25 = asset;
  if (count < 2) {
LAB_000101e2:
    *(MeFAsset **)((int)apMStack_3c + uVar24 * -2 + -4) = pMVar25;
    *(MeFAsset **)((int)&stack0xffffffbc + uVar24 * -2) = pMVar25;
    iVar14 = i;
    *(MeBool *)(&stack0xffffffb8 + uVar24 * -2) = recurse;
    *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
         *(undefined4 *)((int)apMStack_3c + iVar14 * 4 + iVar2);
    *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x101f4;
    pMVar16 = MeFAssetPartCreateCopy
                        (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                         *(int *)(&stack0xffffffb8 + uVar24 * -2));
    pMVar25 = asset_1;
    *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = pMVar16;
    *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = pMVar25;
    *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10202;
    MeFAssetInsertPart(*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
                       *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2));
    copyArray[i] = pMVar16;
    pMVar25 = (MeFAsset *)(i + 1);
    i = (int)pMVar25;
    ppMVar17 = copyArray;
    if ((int)pMVar25 < count) goto LAB_00010220;
  }
  else {
    if (uVar15 != 0) {
      if (1 < uVar15) {
        pMVar16 = (MeFAssetPart *)asset;
        if (2 < uVar15) {
                    /* Unresolved local var: MeFAssetPart * copyPart@[???] */
          *(MeFAsset **)((int)apMStack_3c + uVar24 * -2 + -4) = asset;
          *(MeFAsset **)((int)&stack0xffffffbc + uVar24 * -2) = asset;
          *(MeBool *)(&stack0xffffffb8 + uVar24 * -2) = recurse;
          *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
               *(undefined4 *)((int)apMStack_3c + iVar2);
          *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x105cf;
          pMVar16 = MeFAssetPartCreateCopy
                              (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                               *(int *)(&stack0xffffffb8 + uVar24 * -2));
          pMVar25 = asset_1;
          *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = pMVar16;
          *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = pMVar25;
          *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x105dd;
          MeFAssetInsertPart(*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
                             *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2));
          *copyArray = pMVar16;
          i = 1;
        }
        i = (int)(2 < uVar15);
        *(MeFAssetPart **)((int)apMStack_3c + uVar24 * -2 + -4) = pMVar16;
        *(MeFAssetPart **)((int)&stack0xffffffbc + uVar24 * -2) = pMVar16;
        iVar14 = i;
        *(MeBool *)(&stack0xffffffb8 + uVar24 * -2) = recurse;
        *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
             *(undefined4 *)((int)apMStack_3c + iVar14 * 4 + iVar2);
        *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x1059e;
        pMVar16 = MeFAssetPartCreateCopy
                            (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                             *(int *)(&stack0xffffffb8 + uVar24 * -2));
        pMVar25 = asset_1;
        *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = pMVar16;
        *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = pMVar25;
        *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x105ac;
        MeFAssetInsertPart(*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
                           *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2));
        copyArray[i] = pMVar16;
        pMVar25 = (MeFAsset *)(i + 1);
        i = (int)pMVar25;
      }
      goto LAB_000101e2;
    }
LAB_00010220:
    do {
      *(MeFAsset **)((int)apMStack_3c + uVar24 * -2 + -4) = pMVar25;
      *(MeFAsset **)((int)&stack0xffffffbc + uVar24 * -2) = pMVar25;
      iVar14 = i;
      *(MeBool *)(&stack0xffffffb8 + uVar24 * -2) = recurse;
      *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
           *(undefined4 *)((int)apMStack_3c + iVar14 * 4 + iVar2);
      *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10232;
      pMVar16 = MeFAssetPartCreateCopy
                          (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                           *(int *)(&stack0xffffffb8 + uVar24 * -2));
      pMVar25 = asset_1;
      *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = pMVar16;
      *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = pMVar25;
      *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10240;
      MeFAssetInsertPart(*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
                         *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2));
      copyArray[i] = pMVar16;
      iVar14 = i + 1;
      *(MeBool *)(&stack0xffffffb8 + uVar24 * -2) = recurse;
      *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
           *(undefined4 *)((int)apMStack_3c + iVar14 * 4 + iVar2);
      *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x1025b;
      pMVar16 = MeFAssetPartCreateCopy
                          (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                           *(int *)(&stack0xffffffb8 + uVar24 * -2));
      pMVar25 = asset_1;
      *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = pMVar16;
      *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = pMVar25;
      *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10269;
      MeFAssetInsertPart(*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
                         *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2));
      copyArray[iVar14] = pMVar16;
      iVar14 = i + 2;
      *(MeBool *)(&stack0xffffffb8 + uVar24 * -2) = recurse;
      *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
           *(undefined4 *)((int)apMStack_3c + iVar14 * 4 + iVar2);
      *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10284;
      pMVar16 = MeFAssetPartCreateCopy
                          (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                           *(int *)(&stack0xffffffb8 + uVar24 * -2));
      pMVar25 = asset_1;
      *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = pMVar16;
      *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = pMVar25;
      *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10292;
      MeFAssetInsertPart(*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
                         *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2));
      copyArray[iVar14] = pMVar16;
      pMVar25 = (MeFAsset *)(i + 3);
      *(MeBool *)(&stack0xffffffb8 + uVar24 * -2) = recurse;
      *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
           *(undefined4 *)((int)apMStack_3c + (int)pMVar25 * 4 + iVar2);
      *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x102ad;
      pMVar16 = MeFAssetPartCreateCopy
                          (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                           *(int *)(&stack0xffffffb8 + uVar24 * -2));
      pMVar5 = asset_1;
      *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = pMVar16;
      *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = pMVar5;
      *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x102bb;
      MeFAssetInsertPart(*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
                         *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2));
      copyArray[(int)pMVar25] = pMVar16;
      ppMVar17 = (MeFAssetPart **)(i + 4);
      i = (int)ppMVar17;
    } while ((int)ppMVar17 < count);
  }
  i = 0;
  if (0 < count) {
    do {
                    /* Unresolved local var: int j@[DW_OP_reg3(EBX)] */
      iVar14 = i;
      ppMVar17 = (MeFAssetPart **)(i + 1);
      local_30 = ppMVar17;
      if ((int)ppMVar17 < count) {
        iVar22 = i + 2;
        uVar15 = count - (int)ppMVar17 & 3;
        if (iVar22 < count) {
          iVar18 = count;
          if (uVar15 != 0) {
            if (1 < uVar15) {
              if (2 < uVar15) {
                *(int *)((int)apMStack_3c + uVar24 * -2 + -4) = iVar22;
                *(int *)((int)&stack0xffffffbc + uVar24 * -2) = iVar22;
                iVar22 = i;
                *(undefined4 *)(&stack0xffffffb8 + uVar24 * -2) =
                     *(undefined4 *)((int)apMStack_3c + (int)ppMVar17 * 4 + iVar2);
                *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
                     *(undefined4 *)((int)apMStack_3c + iVar22 * 4 + iVar2);
                *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x1054e;
                iVar18 = MeFAssetPartIsCollisionEnabled
                                   (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                                    *(void **)(&stack0xffffffb8 + uVar24 * -2));
                ppMVar4 = copyArray;
                ppMVar17 = local_30;
                if (iVar18 == 0) {
                  *(undefined4 *)((int)apMStack_3c + uVar24 * -2 + -4) = 0;
                  *(undefined4 *)((int)&stack0xffffffbc + uVar24 * -2) = 0;
                  iVar22 = i;
                  *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = ppMVar4[(int)ppMVar17];
                  *(MeFAssetPart **)(&stack0xffffffb4 + uVar24 * -2) = ppMVar4[iVar22];
                  *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10571;
                  MeFAssetPartEnableCollision
                            (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                             *(void **)(&stack0xffffffb8 + uVar24 * -2),
                             *(int *)((int)&stack0xffffffbc + uVar24 * -2));
                  iVar18 = extraout_EAX_03;
                }
                ppMVar17 = (MeFAssetPart **)(iVar14 + 2);
              }
              *(int *)((int)apMStack_3c + uVar24 * -2 + -4) = iVar18;
              *(int *)((int)&stack0xffffffbc + uVar24 * -2) = iVar18;
              iVar14 = i;
              *(undefined4 *)(&stack0xffffffb8 + uVar24 * -2) =
                   *(undefined4 *)((int)apMStack_3c + (int)ppMVar17 * 4 + iVar2);
              *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
                   *(undefined4 *)((int)apMStack_3c + iVar14 * 4 + iVar2);
              *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10514;
              iVar14 = MeFAssetPartIsCollisionEnabled
                                 (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                                  *(void **)(&stack0xffffffb8 + uVar24 * -2));
              ppMVar4 = copyArray;
              iVar22 = extraout_ECX_02;
              if (iVar14 == 0) {
                *(int *)((int)apMStack_3c + uVar24 * -2 + -4) = extraout_ECX_02;
                iVar14 = i;
                *(undefined4 *)((int)&stack0xffffffbc + uVar24 * -2) = 0;
                *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = ppMVar4[(int)ppMVar17];
                *(MeFAssetPart **)(&stack0xffffffb4 + uVar24 * -2) = ppMVar4[iVar14];
                *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10537;
                MeFAssetPartEnableCollision
                          (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                           *(void **)(&stack0xffffffb8 + uVar24 * -2),
                           *(int *)((int)&stack0xffffffbc + uVar24 * -2));
                iVar22 = extraout_ECX_03;
              }
              ppMVar17 = (MeFAssetPart **)((int)ppMVar17 + 1);
            }
            goto LAB_00010318;
          }
        }
        else {
LAB_00010318:
          *(int *)((int)apMStack_3c + uVar24 * -2 + -4) = iVar22;
          *(int *)((int)&stack0xffffffbc + uVar24 * -2) = iVar22;
          iVar14 = i;
          *(undefined4 *)(&stack0xffffffb8 + uVar24 * -2) =
               *(undefined4 *)((int)apMStack_3c + (int)ppMVar17 * 4 + iVar2);
          *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
               *(undefined4 *)((int)apMStack_3c + iVar14 * 4 + iVar2);
          *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x1032a;
          iVar18 = MeFAssetPartIsCollisionEnabled
                             (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                              *(void **)(&stack0xffffffb8 + uVar24 * -2));
          ppMVar4 = copyArray;
          if (iVar18 == 0) {
            *(undefined4 *)((int)apMStack_3c + uVar24 * -2 + -4) = 0;
            iVar14 = i;
            *(undefined4 *)((int)&stack0xffffffbc + uVar24 * -2) = 0;
            *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = ppMVar4[(int)ppMVar17];
            *(MeFAssetPart **)(&stack0xffffffb4 + uVar24 * -2) = ppMVar4[iVar14];
            *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x104e4;
            MeFAssetPartEnableCollision
                      (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                       *(void **)(&stack0xffffffb8 + uVar24 * -2),
                       *(int *)((int)&stack0xffffffbc + uVar24 * -2));
            iVar18 = extraout_EAX_02;
          }
          ppMVar17 = (MeFAssetPart **)((int)ppMVar17 + 1);
          if (count <= (int)ppMVar17) goto LAB_000103c9;
        }
        do {
          *(int *)((int)apMStack_3c + uVar24 * -2 + -4) = iVar18;
          *(int *)((int)&stack0xffffffbc + uVar24 * -2) = iVar18;
          iVar14 = i;
          *(undefined4 *)(&stack0xffffffb8 + uVar24 * -2) =
               *(undefined4 *)((int)apMStack_3c + (int)ppMVar17 * 4 + iVar2);
          *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
               *(undefined4 *)((int)apMStack_3c + iVar14 * 4 + iVar2);
          *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10352;
          iVar14 = MeFAssetPartIsCollisionEnabled
                             (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                              *(void **)(&stack0xffffffb8 + uVar24 * -2));
          ppMVar4 = copyArray;
          if (iVar14 == 0) {
            *(undefined4 *)((int)apMStack_3c + uVar24 * -2 + -4) = extraout_ECX;
            iVar14 = i;
            *(undefined4 *)((int)&stack0xffffffbc + uVar24 * -2) = 0;
            *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = ppMVar4[(int)ppMVar17];
            *(MeFAssetPart **)(&stack0xffffffb4 + uVar24 * -2) = ppMVar4[iVar14];
            *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x104c6;
            MeFAssetPartEnableCollision
                      (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                       *(void **)(&stack0xffffffb8 + uVar24 * -2),
                       *(int *)((int)&stack0xffffffbc + uVar24 * -2));
            iVar14 = extraout_EAX_01;
          }
          *(int *)((int)apMStack_3c + uVar24 * -2 + -4) = iVar14;
          iVar22 = i;
          *(int *)((int)&stack0xffffffbc + uVar24 * -2) = iVar14;
          *(undefined4 *)(&stack0xffffffb8 + uVar24 * -2) =
               *(undefined4 *)((int)apMStack_3c + ((int)ppMVar17 + 1) * 4 + iVar2);
          *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
               *(undefined4 *)((int)apMStack_3c + iVar22 * 4 + iVar2);
          *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10372;
          iVar14 = MeFAssetPartIsCollisionEnabled
                             (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                              *(void **)(&stack0xffffffb8 + uVar24 * -2));
          ppMVar4 = copyArray;
          if (iVar14 == 0) {
            *(undefined4 *)((int)apMStack_3c + uVar24 * -2 + -4) = 0;
            *(undefined4 *)((int)&stack0xffffffbc + uVar24 * -2) = 0;
            *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = ppMVar4[(int)ppMVar17 + 1];
            *(MeFAssetPart **)(&stack0xffffffb4 + uVar24 * -2) = ppMVar4[i];
            *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x104a7;
            MeFAssetPartEnableCollision
                      (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                       *(void **)(&stack0xffffffb8 + uVar24 * -2),
                       *(int *)((int)&stack0xffffffbc + uVar24 * -2));
            iVar14 = extraout_EAX_00;
          }
          *(int *)((int)apMStack_3c + uVar24 * -2 + -4) = iVar14;
          *(int *)((int)&stack0xffffffbc + uVar24 * -2) = iVar14;
          iVar14 = i;
          *(undefined4 *)(&stack0xffffffb8 + uVar24 * -2) =
               *(undefined4 *)((int)apMStack_3c + ((int)ppMVar17 + 2) * 4 + iVar2);
          *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
               *(undefined4 *)((int)apMStack_3c + iVar14 * 4 + iVar2);
          *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10392;
          iVar14 = MeFAssetPartIsCollisionEnabled
                             (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                              *(void **)(&stack0xffffffb8 + uVar24 * -2));
          ppMVar4 = copyArray;
          uVar23 = extraout_ECX_00;
          if (iVar14 == 0) {
            *(undefined4 *)((int)apMStack_3c + uVar24 * -2 + -4) = extraout_ECX_00;
            *(undefined4 *)((int)&stack0xffffffbc + uVar24 * -2) = 0;
            iVar14 = i;
            *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = ppMVar4[(int)ppMVar17 + 2];
            *(MeFAssetPart **)(&stack0xffffffb4 + uVar24 * -2) = ppMVar4[iVar14];
            *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10489;
            MeFAssetPartEnableCollision
                      (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                       *(void **)(&stack0xffffffb8 + uVar24 * -2),
                       *(int *)((int)&stack0xffffffbc + uVar24 * -2));
            uVar23 = extraout_ECX_01;
          }
          *(undefined4 *)((int)apMStack_3c + uVar24 * -2 + -4) = uVar23;
          *(undefined4 *)((int)&stack0xffffffbc + uVar24 * -2) = uVar23;
          iVar14 = i;
          *(undefined4 *)(&stack0xffffffb8 + uVar24 * -2) =
               *(undefined4 *)((int)apMStack_3c + ((int)ppMVar17 + 3) * 4 + iVar2);
          *(undefined4 *)(&stack0xffffffb4 + uVar24 * -2) =
               *(undefined4 *)((int)apMStack_3c + iVar14 * 4 + iVar2);
          *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x103b2;
          iVar18 = MeFAssetPartIsCollisionEnabled
                             (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                              *(void **)(&stack0xffffffb8 + uVar24 * -2));
          ppMVar4 = copyArray;
          if (iVar18 == 0) {
            *(undefined4 *)((int)apMStack_3c + uVar24 * -2 + -4) = 0;
            *(undefined4 *)((int)&stack0xffffffbc + uVar24 * -2) = 0;
            *(MeFAssetPart **)(&stack0xffffffb8 + uVar24 * -2) = ppMVar4[(int)ppMVar17 + 3];
            *(MeFAssetPart **)(&stack0xffffffb4 + uVar24 * -2) = ppMVar4[i];
            *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x1046b;
            MeFAssetPartEnableCollision
                      (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                       *(void **)(&stack0xffffffb8 + uVar24 * -2),
                       *(int *)((int)&stack0xffffffbc + uVar24 * -2));
            iVar18 = extraout_EAX;
          }
          ppMVar17 = ppMVar17 + 1;
        } while ((int)ppMVar17 < count);
      }
LAB_000103c9:
      i = (int)local_30;
      ppMVar17 = local_30;
    } while ((int)local_30 < count);
  }
LAB_000103da:
                    /* Unresolved local var: MeFModel * model@[DW_OP_reg0(EAX)] */
  *(MeFAssetPart ***)((int)apMStack_3c + uVar24 * -2 + -4) = ppMVar17;
  *(MeFAssetPart ***)((int)&stack0xffffffbc + uVar24 * -2) = ppMVar17;
  *(MeFModelIt **)(&stack0xffffffb8 + uVar24 * -2) = &it_1;
  *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = asset;
  *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x103e9;
  MeFAssetInitModelIterator
            (*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
             *(MeFModelIt **)(&stack0xffffffb8 + uVar24 * -2));
  while( true ) {
    *(MeFModelIt **)(&stack0xffffffb4 + uVar24 * -2) = &it_1;
    *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x103f8;
    pMVar19 = MeFAssetGetModel(*(MeFModelIt **)(&stack0xffffffb4 + uVar24 * -2));
    if (pMVar19 == (MeFModel *)0x0) break;
                    /* Unresolved local var: MeFModel * copyModel@[DW_OP_reg0(EAX)] */
    *(MeFAsset **)((int)apMStack_3c + uVar24 * -2 + -4) = asset;
    *(MeFAsset **)((int)&stack0xffffffbc + uVar24 * -2) = asset;
    *(MeBool *)(&stack0xffffffb8 + uVar24 * -2) = recurse;
    *(MeFModel **)(&stack0xffffffb4 + uVar24 * -2) = pMVar19;
    *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x1040b;
    pvVar20 = MeFModelCreateCopy(*(void **)(&stack0xffffffb4 + uVar24 * -2),
                                 *(int *)(&stack0xffffffb8 + uVar24 * -2));
    pMVar25 = asset_1;
    *(void **)(&stack0xffffffb8 + uVar24 * -2) = pvVar20;
    *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = pMVar25;
    *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10417;
    MeFAssetInsertModel(*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
                        *(MeFModel **)(&stack0xffffffb8 + uVar24 * -2));
  }
                    /* Unresolved local var: MeFGeometry * geometry@[DW_OP_reg0(EAX)] */
  *(undefined4 *)((int)apMStack_3c + uVar24 * -2 + -4) = 0;
  *(undefined4 *)((int)&stack0xffffffbc + uVar24 * -2) = 0;
  *(MeFGeometryIt **)(&stack0xffffffb8 + uVar24 * -2) = &it_2;
  *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = asset;
  *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10428;
  MeFAssetInitGeometryIterator
            (*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
             *(MeFGeometryIt **)(&stack0xffffffb8 + uVar24 * -2));
  while( true ) {
    *(MeFGeometryIt **)(&stack0xffffffb4 + uVar24 * -2) = &it_2;
    *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10430;
    pMVar21 = MeFAssetGetGeometry(*(MeFGeometryIt **)(&stack0xffffffb4 + uVar24 * -2));
    if (pMVar21 == (MeFGeometry *)0x0) break;
                    /* Unresolved local var: MeFGeometry * copyGeom@[DW_OP_reg0(EAX)] */
    *(MeFAsset **)((int)apMStack_3c + uVar24 * -2 + -4) = asset;
    *(MeFAsset **)((int)&stack0xffffffbc + uVar24 * -2) = asset;
    *(MeBool *)(&stack0xffffffb8 + uVar24 * -2) = recurse;
    *(MeFGeometry **)(&stack0xffffffb4 + uVar24 * -2) = pMVar21;
    *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10447;
    pvVar20 = MeFGeometryCreateCopy
                        (*(void **)(&stack0xffffffb4 + uVar24 * -2),
                         *(int *)(&stack0xffffffb8 + uVar24 * -2));
    asset = asset_1;
    *(void **)(&stack0xffffffb8 + uVar24 * -2) = pvVar20;
    *(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2) = asset;
    *(undefined4 *)((int)&pMStackY_50 + uVar24 * -2) = 0x10453;
    MeFAssetInsertGeometry
              (*(MeFAsset **)(&stack0xffffffb4 + uVar24 * -2),
               *(MeFGeometry **)(&stack0xffffffb8 + uVar24 * -2));
  }
  return asset_1;
}


/* ==== MeFAssetDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFAssetDestroy(MeFAsset *asset)

{
  MeFJoint *joint;
  MeFAssetPart *part;
  MeFModel *model;
  MeFGeometry *geometry;
  MeFGeometryIt *pMVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  MeFGeometryIt it_3;
  MeFModelIt it_2;
  MeFAssetPartIt it_1;
  MeFJointIt it;
  
                    /* Unresolved local var: MeFJoint * joint@[DW_OP_reg3(EBX)] */
  while( true ) {
    MeFAssetInitJointIterator(asset,&it);
    joint = MeFAssetGetJoint(&it);
    if (joint == (MeFJoint *)0x0) break;
    MeFAssetRemoveJoint(joint);
    MeFJointDestroy(joint);
  }
                    /* Unresolved local var: MeFAssetPart * part@[DW_OP_reg3(EBX)] */
  while( true ) {
    MeFAssetInitPartIterator(asset,&it_1);
    part = MeFAssetGetPart(&it_1);
    if (part == (MeFAssetPart *)0x0) break;
    MeFAssetRemovePart(part);
    MeFAssetPartDestroy(part);
  }
                    /* Unresolved local var: MeFModel * model@[DW_OP_reg3(EBX)] */
  while( true ) {
    MeFAssetInitModelIterator(asset,&it_2);
    model = MeFAssetGetModel(&it_2);
    if (model == (MeFModel *)0x0) break;
    MeFAssetRemoveModel(model);
    MeFModelDestroy(model);
  }
                    /* Unresolved local var: MeFGeometry * geometry@[DW_OP_reg3(EBX)] */
  uVar2 = 0;
  uVar3 = 0;
  while( true ) {
    pMVar1 = &it_3;
    MeFAssetInitGeometryIterator(asset,&it_3);
    geometry = MeFAssetGetGeometry(&it_3);
    if (geometry == (MeFGeometry *)0x0) break;
    MeFAssetRemoveGeometry(geometry);
    MeFGeometryDestroy(geometry);
  }
  if (asset->name != (char *)0x0) {
    (*__FSetStringProperty)(asset->name,pMVar1,uVar2,uVar3);
  }
  if (asset->graphicHint != (char *)0x0) {
    (*__FSetStringProperty)(asset->graphicHint);
  }
  if (asset->refPart != (char *)0x0) {
    (*__FSetStringProperty)(asset->refPart);
  }
  (*__FSetStringProperty)(asset->nilGeometry);
  (*__FSetStringProperty)(asset->nilModel);
  (*__FSetStringProperty)(asset->nilPart);
  (*__FSetStringProperty)(asset->nilJoint);
  MeHashDestroy(asset->nameToGeometry);
  MeHashDestroy(asset->nameToModel);
  MeHashDestroy(asset->nameToPart);
  MeHashDestroy(asset->nameToJoint);
  MeIDPoolDestroy(asset->disabledColIndexPool);
  (*__FSetStringProperty)(asset->disabledColArray);
  (*__FSetStringProperty)(asset);
  return;
}


/* ==== MeFAssetCombine ==== */

void MeFAssetCombine(MeFAsset *dest,MeFAsset *source)

{
  void *pvVar1;
  MeFGeometry *pMVar2;
  char *pcVar3;
  MeFModel *pMVar4;
  MeFAssetPart *pMVar5;
  MeFJoint *pMVar6;
  MeFAssetPart *pMVar7;
  int iVar8;
  int iVar9;
  MeFAssetPart *pMVar10;
  MeFAssetPartIt it2;
  MeFAssetPartIt it1;
  MeFJointIt it_7;
  MeFAssetPartIt it_6;
  MeFModelIt it_5;
  MeFGeometryIt it_4;
  MeFJointIt it_3;
  MeFAssetPartIt it_2;
  MeFModelIt it_1;
  MeFGeometryIt it;
  char buffer [256];
  
                    /* Unresolved local var: char * name@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeHash * hash@[DW_OP_reg0(EAX)] */
                    /* Unresolved local var: MeFGeometry * geom@[DW_OP_reg3(EBX)] */
  pvVar1 = MeHashCreate(0x61);
  MeHashSetKeyCompareFunc(pvVar1,MeHashIntCompare);
  MeFAssetInitGeometryIterator(source,&it);
  while (pMVar2 = MeFAssetGetGeometry(&it), pMVar2 != (MeFGeometry *)0x0) {
    pcVar3 = MeFAssetMakeGeometryNameUnique(dest,source,pMVar2->id,buffer,0x100);
    MeFGeometryRename(pMVar2,pcVar3);
  }
                    /* Unresolved local var: MeFModel * model@[DW_OP_reg3(EBX)] */
  MeFAssetInitModelIterator(source,&it_1);
  while (pMVar4 = MeFAssetGetModel(&it_1), pMVar4 != (MeFModel *)0x0) {
    pcVar3 = MeFAssetMakeModelNameUnique(dest,source,pMVar4->id,buffer,0x100);
    MeFModelRename(pMVar4,pcVar3);
  }
                    /* Unresolved local var: MeFAssetPart * part@[DW_OP_reg3(EBX)] */
  MeFAssetInitPartIterator(source,&it_2);
  while (pMVar5 = MeFAssetGetPart(&it_2), pMVar5 != (MeFAssetPart *)0x0) {
    pcVar3 = MeFAssetMakePartNameUnique(dest,source,pMVar5->id,buffer,0x100);
    MeFAssetPartRename(pMVar5,pcVar3);
  }
                    /* Unresolved local var: MeFJoint * joint@[DW_OP_reg3(EBX)] */
  MeFAssetInitJointIterator(source,&it_3);
  while (pMVar6 = MeFAssetGetJoint(&it_3), pMVar6 != (MeFJoint *)0x0) {
    pcVar3 = MeFAssetMakeJointNameUnique(dest,source,pMVar6->id,buffer,0x100);
    MeFJointRename(pMVar6,pcVar3);
  }
                    /* Unresolved local var: MeFGeometry * geom@[DW_OP_reg0(EAX)] */
  MeFAssetInitGeometryIterator(source,&it_4);
  while (pMVar2 = MeFAssetGetGeometry(&it_4), pMVar2 != (MeFGeometry *)0x0) {
                    /* Unresolved local var: MeFGeometry * copy@[DW_OP_reg0(EAX)] */
    pMVar2 = MeFGeometryCreateCopy(pMVar2,1);
    MeFAssetInsertGeometry(dest,pMVar2);
  }
                    /* Unresolved local var: MeFModel * model@[DW_OP_reg0(EAX)] */
  MeFAssetInitModelIterator(source,&it_5);
  while (pMVar4 = MeFAssetGetModel(&it_5), pMVar4 != (MeFModel *)0x0) {
                    /* Unresolved local var: MeFModel * copy@[DW_OP_reg0(EAX)] */
    pMVar4 = MeFModelCreateCopy(pMVar4,1);
    MeFAssetInsertModel(dest,pMVar4);
  }
                    /* Unresolved local var: MeFAssetPart * part@[DW_OP_reg0(EAX)] */
  MeFAssetInitPartIterator(source,&it_6);
  while (pMVar5 = MeFAssetGetPart(&it_6), pMVar5 != (MeFAssetPart *)0x0) {
                    /* Unresolved local var: MeFAssetPart * copy@[DW_OP_reg0(EAX)] */
    pMVar5 = MeFAssetPartCreateCopy(pMVar5,1);
    MeFAssetInsertPart(dest,pMVar5);
  }
                    /* Unresolved local var: MeFJoint * joint@[DW_OP_reg3(EBX)] */
  MeFAssetInitJointIterator(source,&it_7);
  while (pMVar6 = MeFAssetGetJoint(&it_7), pMVar6 != (MeFJoint *)0x0) {
                    /* Unresolved local var: MeFJoint * copy@[???] */
    MeFJointCreateCopy(pMVar6,1);
    MeFAssetInsertJoint(dest,pMVar6);
  }
                    /* Unresolved local var: MeFAssetPart * part1@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeFAssetPart * part2@[DW_OP_reg6(ESI)] */
  MeFAssetInitPartIterator(source,&it1);
  while (pMVar5 = MeFAssetGetPart(&it1), pMVar5 != (MeFAssetPart *)0x0) {
    MeFAssetInitPartIterator(source,&it2);
    while (pMVar7 = MeFAssetGetPart(&it2), pMVar7 != (MeFAssetPart *)0x0) {
                    /* Unresolved local var: MeFAssetPart * p1@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeFAssetPart * p2@[DW_OP_reg0(EAX)] */
      iVar8 = MeFAssetPartGetDisabledCollisionIndex(pMVar5);
      iVar9 = MeFAssetPartGetDisabledCollisionIndex(pMVar7);
      if ((iVar8 < iVar9) && (iVar8 = MeFAssetPartIsCollisionEnabled(pMVar5,pMVar7), iVar8 == 0)) {
        pMVar10 = MeFAssetLookupPart(dest,pMVar5->id);
        pMVar7 = MeFAssetLookupPart(dest,pMVar7->id);
        MeFAssetPartEnableCollision(pMVar10,pMVar7,0);
      }
    }
  }
  return;
}


/* ==== MeFAssetGetPartCount ==== */

int MeFAssetGetPartCount(MeFAsset *asset)

{
  return asset->partCount;
}


/* ==== MeFAssetInitGeometryIterator ==== */

void MeFAssetInitGeometryIterator(MeFAsset *asset,MeFGeometryIt *it)

{
  it->node = asset->nilGeometry->prev;
  return;
}


/* ==== MeFAssetGetGeometry ==== */

MeFGeometry * MeFAssetGetGeometry(MeFGeometryIt *it)

{
  MeFGeometry *pMVar1;
  
                    /* Unresolved local var: MeFGeometry * g@[DW_OP_reg0(EAX)] */
  pMVar1 = it->node->current;
  it->node = it->node->prev;
  return pMVar1;
}


/* ==== MeFAssetInitModelIterator ==== */

void MeFAssetInitModelIterator(MeFAsset *asset,MeFModelIt *it)

{
  it->node = asset->nilModel->prev;
  return;
}


/* ==== MeFAssetGetModel ==== */

MeFModel * MeFAssetGetModel(MeFModelIt *it)

{
  MeFModel *pMVar1;
  
                    /* Unresolved local var: MeFModel * model@[DW_OP_reg0(EAX)] */
  pMVar1 = it->node->current;
  it->node = it->node->prev;
  return pMVar1;
}


/* ==== MeFAssetInitJointIterator ==== */

void MeFAssetInitJointIterator(MeFAsset *asset,MeFJointIt *it)

{
  it->node = asset->nilJoint->prev;
  return;
}


/* ==== MeFAssetGetJoint ==== */

MeFJoint * MeFAssetGetJoint(MeFJointIt *it)

{
  MeFJoint *pMVar1;
  
                    /* Unresolved local var: MeFJoint * fj@[DW_OP_reg0(EAX)] */
  pMVar1 = it->node->current;
  it->node = it->node->prev;
  return pMVar1;
}


/* ==== MeFAssetInitPartIterator ==== */

void MeFAssetInitPartIterator(MeFAsset *asset,MeFAssetPartIt *it)

{
  it->node = asset->nilPart->prev;
  return;
}


/* ==== MeFAssetGetPart ==== */

MeFAssetPart * MeFAssetGetPart(MeFAssetPartIt *it)

{
  MeFAssetPart *pMVar1;
  
                    /* Unresolved local var: MeFAssetPart * p@[DW_OP_reg0(EAX)] */
  pMVar1 = it->node->current;
  it->node = it->node->prev;
  return pMVar1;
}


/* ==== MeFAssetLookupPart ==== */

MeFAssetPart * MeFAssetLookupPart(MeFAsset *asset,char *name)

{
  MeFAssetPart *pMVar1;
  
  pMVar1 = MeHashLookup(name,asset->nameToPart);
  return pMVar1;
}


/* ==== MeFAssetGetPartsSortedByName ==== */

void MeFAssetGetPartsSortedByName(MeFAsset *asset,MeFAssetPart **partArray)

{
  MeFAssetPart *pMVar1;
  int iVar2;
  MeFAssetPartIt it;
  
                    /* Unresolved local var: MeFAssetPart * part@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar2 = 0;
  MeFAssetInitPartIterator(asset,&it);
  while( true ) {
    pMVar1 = MeFAssetGetPart(&it);
    if (pMVar1 == (MeFAssetPart *)0x0) break;
    partArray[iVar2] = pMVar1;
    iVar2 = iVar2 + 1;
  }
  qsort(partArray,asset->partCount,4,_MeFAssetPartSortFunc);
  return;
}


/* ==== MeFAssetMakeGeometryNameUnique ==== */

char * MeFAssetMakeGeometryNameUnique
                 (MeFAsset *asset,MeFAsset *asset2,char *name,char *buffer,int bufLength)

{
  bool bVar1;
  void *pvVar2;
  size_t sVar3;
  ushort **ppuVar4;
  int iVar5;
  size_t sVar6;
  int count;
  char number [32];
  
  strncpy(buffer,name,bufLength);
  do {
                    /* Unresolved local var: MeFGeometry * geom@[DW_OP_reg0(EAX)] */
    pvVar2 = MeHashLookup(buffer,asset->nameToGeometry);
    if (pvVar2 == (void *)0x0) {
      pvVar2 = (void *)0x0;
      if (asset2 != (MeFAsset *)0x0) {
        pvVar2 = MeHashLookup(buffer,asset2->nameToGeometry);
      }
      if (pvVar2 == (void *)0x0) {
        return buffer;
      }
    }
                    /* Unresolved local var: MeBool done@[DW_OP_reg7(EDI)]
                       Unresolved local var: int length@[???]
                       Unresolved local var: int last@[DW_OP_reg3(EBX)] */
    count = 0;
    bVar1 = false;
    sVar3 = strlen(buffer);
    ppuVar4 = __ctype_b_loc();
    sVar6 = sVar3;
    do {
      sVar6 = sVar6 - 1;
      if (((*ppuVar4)[buffer[sVar6]] & 0x800) == 0) {
        bVar1 = true;
      }
      else {
        count = count + 1;
      }
    } while (!bVar1);
    if (count < 1) {
      strcat(buffer,"1");
    }
    else {
                    /* Unresolved local var: int n@[DW_OP_reg0(EAX)] */
      strcpy(number,buffer + (sVar3 - count));
      iVar5 = __strtol_internal(number,0,10,0);
      sprintf(number,"%d",iVar5 + 1);
      strcpy(buffer + (sVar3 - count),number);
    }
  } while( true );
}


/* ==== MeFAssetMakeModelNameUnique ==== */

char * MeFAssetMakeModelNameUnique
                 (MeFAsset *asset,MeFAsset *asset2,char *name,char *buffer,int bufLength)

{
  bool bVar1;
  void *pvVar2;
  size_t sVar3;
  ushort **ppuVar4;
  int iVar5;
  size_t sVar6;
  int count;
  char number [32];
  
  strncpy(buffer,name,bufLength);
  do {
                    /* Unresolved local var: MeFModel * model@[DW_OP_reg0(EAX)] */
    pvVar2 = MeHashLookup(buffer,asset->nameToModel);
    if (pvVar2 == (void *)0x0) {
      pvVar2 = (void *)0x0;
      if (asset2 != (MeFAsset *)0x0) {
        pvVar2 = MeHashLookup(buffer,asset2->nameToModel);
      }
      if (pvVar2 == (void *)0x0) {
        return buffer;
      }
    }
                    /* Unresolved local var: MeBool done@[DW_OP_reg7(EDI)]
                       Unresolved local var: int length@[???]
                       Unresolved local var: int last@[DW_OP_reg3(EBX)] */
    count = 0;
    bVar1 = false;
    sVar3 = strlen(buffer);
    ppuVar4 = __ctype_b_loc();
    sVar6 = sVar3;
    do {
      sVar6 = sVar6 - 1;
      if (((*ppuVar4)[buffer[sVar6]] & 0x800) == 0) {
        bVar1 = true;
      }
      else {
        count = count + 1;
      }
    } while (!bVar1);
    if (count < 1) {
      strcat(buffer,"1");
    }
    else {
                    /* Unresolved local var: int n@[DW_OP_reg0(EAX)] */
      strcpy(number,buffer + (sVar3 - count));
      iVar5 = __strtol_internal(number,0,10,0);
      sprintf(number,"%d",iVar5 + 1);
      strcpy(buffer + (sVar3 - count),number);
    }
  } while( true );
}


/* ==== MeFAssetMakePartNameUnique ==== */

char * MeFAssetMakePartNameUnique
                 (MeFAsset *asset,MeFAsset *asset2,char *name,char *buffer,int bufLength)

{
  bool bVar1;
  MeFAssetPart *pMVar2;
  size_t sVar3;
  ushort **ppuVar4;
  int iVar5;
  size_t sVar6;
  int count;
  char number [32];
  
  strncpy(buffer,name,bufLength);
  do {
                    /* Unresolved local var: MeFAssetPart * part@[DW_OP_reg0(EAX)] */
    pMVar2 = MeFAssetLookupPart(asset,buffer);
    if (pMVar2 == (MeFAssetPart *)0x0) {
      pMVar2 = (MeFAssetPart *)0x0;
      if (asset2 != (MeFAsset *)0x0) {
        pMVar2 = MeFAssetLookupPart(asset2,buffer);
      }
      if (pMVar2 == (MeFAssetPart *)0x0) {
        return buffer;
      }
    }
                    /* Unresolved local var: MeBool done@[DW_OP_reg7(EDI)]
                       Unresolved local var: int length@[???]
                       Unresolved local var: int last@[DW_OP_reg3(EBX)] */
    count = 0;
    bVar1 = false;
    sVar3 = strlen(buffer);
    ppuVar4 = __ctype_b_loc();
    sVar6 = sVar3;
    do {
      sVar6 = sVar6 - 1;
      if (((*ppuVar4)[buffer[sVar6]] & 0x800) == 0) {
        bVar1 = true;
      }
      else {
        count = count + 1;
      }
    } while (!bVar1);
    if (count < 1) {
      strcat(buffer,"1");
    }
    else {
                    /* Unresolved local var: int n@[DW_OP_reg0(EAX)] */
      strcpy(number,buffer + (sVar3 - count));
      iVar5 = __strtol_internal(number,0,10,0);
      sprintf(number,"%d",iVar5 + 1);
      strcpy(buffer + (sVar3 - count),number);
    }
  } while( true );
}


/* ==== MeFAssetMakeJointNameUnique ==== */

char * MeFAssetMakeJointNameUnique
                 (MeFAsset *asset,MeFAsset *asset2,char *name,char *buffer,int bufLength)

{
  bool bVar1;
  void *pvVar2;
  size_t sVar3;
  ushort **ppuVar4;
  int iVar5;
  size_t sVar6;
  int count;
  char number [32];
  
  strncpy(buffer,name,bufLength);
  do {
                    /* Unresolved local var: MeFJoint * joint@[DW_OP_reg0(EAX)] */
    pvVar2 = MeHashLookup(buffer,asset->nameToJoint);
    if (pvVar2 == (void *)0x0) {
      pvVar2 = (void *)0x0;
      if (asset2 != (MeFAsset *)0x0) {
        pvVar2 = MeHashLookup(buffer,asset2->nameToJoint);
      }
      if (pvVar2 == (void *)0x0) {
        return buffer;
      }
    }
                    /* Unresolved local var: MeBool done@[DW_OP_reg7(EDI)]
                       Unresolved local var: int length@[???]
                       Unresolved local var: int last@[DW_OP_reg3(EBX)] */
    count = 0;
    bVar1 = false;
    sVar3 = strlen(buffer);
    ppuVar4 = __ctype_b_loc();
    sVar6 = sVar3;
    do {
      sVar6 = sVar6 - 1;
      if (((*ppuVar4)[buffer[sVar6]] & 0x800) == 0) {
        bVar1 = true;
      }
      else {
        count = count + 1;
      }
    } while (!bVar1);
    if (count < 1) {
      strcat(buffer,"1");
    }
    else {
                    /* Unresolved local var: int n@[DW_OP_reg0(EAX)] */
      strcpy(number,buffer + (sVar3 - count));
      iVar5 = __strtol_internal(number,0,10,0);
      sprintf(number,"%d",iVar5 + 1);
      strcpy(buffer + (sVar3 - count),number);
    }
  } while( true );
}


/* ==== MeFAssetInsertGeometry ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFAssetInsertGeometry(MeFAsset *asset,MeFGeometry *geometry)

{
  MeFAsset *pMVar1;
  MeFGeometryNode *pMVar2;
  
                    /* Unresolved local var: MeFGeometryNode * node@[DW_OP_reg0(EAX)] */
  pMVar1 = geometry->asset;
  if (pMVar1 != (MeFAsset *)0x0) {
    if (pMVar1 == asset) {
      return;
    }
    if (pMVar1 != (MeFAsset *)0x0) {
      MeFAssetRemoveGeometry(geometry);
    }
  }
  pMVar2 = (MeFGeometryNode *)(*_MeMemoryAPI)(0xc);
  pMVar2->current = geometry;
  pMVar2->next = asset->nilGeometry->next;
  asset->nilGeometry->next->prev = pMVar2;
  asset->nilGeometry->next = pMVar2;
  pMVar2->prev = asset->nilGeometry;
  MeHashInsert(geometry->id,geometry,asset->nameToGeometry);
  geometry->asset = asset;
  asset->geomCount = asset->geomCount + 1;
  return;
}


/* ==== MeFAssetRemoveGeometry ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFAssetRemoveGeometry(MeFGeometry *geometry)

{
  MeFGeometry *pMVar1;
  MeFGeometryNode *pMVar2;
  MeHash *pMVar3;
  MeFGeometryNode *pMVar4;
  MeFGeometryNode *pMVar5;
  
                    /* Unresolved local var: MeFGeometryNode * node@[DW_OP_reg3(EBX)] */
  if (geometry->asset != (MeFAsset *)0x0) {
    pMVar4 = geometry->asset->nilGeometry;
    pMVar2 = pMVar4->prev;
    if (pMVar2 != pMVar4) {
      pMVar1 = pMVar2->current;
      while ((pMVar1 != geometry && (pMVar2 = pMVar2->prev, pMVar2 != pMVar4))) {
        pMVar1 = pMVar2->current;
      }
    }
    pMVar5 = pMVar2->prev;
    pMVar5->next = pMVar2->next;
    pMVar2->next->prev = pMVar5;
    pMVar3 = geometry->asset->nameToGeometry;
    pMVar5 = pMVar4;
    MeHashDelete(geometry->id,pMVar3);
    (*__FSetStringProperty)(pMVar2,pMVar3,pMVar4,pMVar5);
    geometry->asset->geomCount = geometry->asset->geomCount + -1;
    geometry->asset = (MeFAsset *)0x0;
  }
  return;
}


/* ==== MeFAssetInsertModel ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFAssetInsertModel(MeFAsset *asset,MeFModel *model)

{
  MeFAsset *pMVar1;
  MeFModelNode *pMVar2;
  
                    /* Unresolved local var: MeFModelNode * node@[DW_OP_reg0(EAX)] */
  pMVar1 = model->asset;
  if (pMVar1 != (MeFAsset *)0x0) {
    if (pMVar1 == asset) {
      return;
    }
    if (pMVar1 != (MeFAsset *)0x0) {
      MeFAssetRemoveModel(model);
    }
  }
  pMVar2 = (MeFModelNode *)(*_MeMemoryAPI)(0xc);
  pMVar2->current = model;
  pMVar2->next = asset->nilModel->next;
  asset->nilModel->next->prev = pMVar2;
  asset->nilModel->next = pMVar2;
  pMVar2->prev = asset->nilModel;
  MeHashInsert(model->id,model,asset->nameToModel);
  model->asset = asset;
  asset->modelCount = asset->modelCount + 1;
  return;
}


/* ==== MeFAssetRemoveModel ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFAssetRemoveModel(MeFModel *model)

{
  MeFModel *pMVar1;
  MeFModelNode *pMVar2;
  MeHash *pMVar3;
  MeFModelNode *pMVar4;
  MeFModelNode *pMVar5;
  
                    /* Unresolved local var: MeFModelNode * node@[DW_OP_reg3(EBX)] */
  if (model->asset != (MeFAsset *)0x0) {
    pMVar4 = model->asset->nilModel;
    pMVar2 = pMVar4->prev;
    if (pMVar2 != pMVar4) {
      pMVar1 = pMVar2->current;
      while ((pMVar1 != model && (pMVar2 = pMVar2->prev, pMVar2 != pMVar4))) {
        pMVar1 = pMVar2->current;
      }
    }
    pMVar5 = pMVar2->prev;
    pMVar5->next = pMVar2->next;
    pMVar2->next->prev = pMVar5;
    pMVar3 = model->asset->nameToModel;
    pMVar5 = pMVar4;
    MeHashDelete(model->id,pMVar3);
    (*__FSetStringProperty)(pMVar2,pMVar3,pMVar4,pMVar5);
    model->asset->modelCount = model->asset->modelCount + -1;
    model->asset = (MeFAsset *)0x0;
  }
  return;
}


/* ==== MeFAssetInsertJoint ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFAssetInsertJoint(MeFAsset *asset,MeFJoint *joint)

{
  MeFAsset *pMVar1;
  MeFJointNode *pMVar2;
  
                    /* Unresolved local var: MeFJointNode * node@[DW_OP_reg0(EAX)] */
  pMVar1 = joint->asset;
  if (pMVar1 != (MeFAsset *)0x0) {
    if (pMVar1 == asset) {
      return;
    }
    if (pMVar1 != (MeFAsset *)0x0) {
      MeFAssetRemoveJoint(joint);
    }
  }
  pMVar2 = (MeFJointNode *)(*_MeMemoryAPI)(0xc);
  pMVar2->current = joint;
  pMVar2->next = asset->nilJoint->next;
  asset->nilJoint->next->prev = pMVar2;
  asset->nilJoint->next = pMVar2;
  pMVar2->prev = asset->nilJoint;
  MeHashInsert(joint->id,joint,asset->nameToJoint);
  joint->asset = asset;
  asset->jointCount = asset->jointCount + 1;
  return;
}


/* ==== MeFAssetRemoveJoint ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFAssetRemoveJoint(MeFJoint *joint)

{
  MeFJoint *pMVar1;
  MeFJointNode *pMVar2;
  MeHash *pMVar3;
  MeFJointNode *pMVar4;
  MeFJointNode *pMVar5;
  
                    /* Unresolved local var: MeFJointNode * node@[DW_OP_reg3(EBX)] */
  if (joint->asset != (MeFAsset *)0x0) {
    pMVar4 = joint->asset->nilJoint;
    pMVar2 = pMVar4->prev;
    if (pMVar2 != pMVar4) {
      pMVar1 = pMVar2->current;
      while ((pMVar1 != joint && (pMVar2 = pMVar2->prev, pMVar2 != pMVar4))) {
        pMVar1 = pMVar2->current;
      }
    }
    pMVar5 = pMVar2->prev;
    pMVar5->next = pMVar2->next;
    pMVar2->next->prev = pMVar5;
    pMVar3 = joint->asset->nameToJoint;
    pMVar5 = pMVar4;
    MeHashDelete(joint->id,pMVar3);
    (*__FSetStringProperty)(pMVar2,pMVar3,pMVar4,pMVar5);
    joint->asset->jointCount = joint->asset->jointCount + -1;
    joint->asset = (MeFAsset *)0x0;
  }
  return;
}


/* ==== MeFAssetInsertPart ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFAssetInsertPart(MeFAsset *asset,MeFAssetPart *part)

{
  MeFAsset *pMVar1;
  MeFAssetPartNode *pMVar2;
  int iVar3;
  
                    /* Unresolved local var: MeFAssetPartNode * node@[DW_OP_reg0(EAX)] */
  pMVar1 = part->asset;
  if (pMVar1 != (MeFAsset *)0x0) {
    if (pMVar1 == asset) {
      return;
    }
    if (pMVar1 != (MeFAsset *)0x0) {
      if (pMVar1->maxParts <= pMVar1->partCount) {
        return;
      }
      MeFAssetRemovePart(part);
    }
  }
  pMVar2 = (MeFAssetPartNode *)(*_MeMemoryAPI)(0xc);
  pMVar2->current = part;
  pMVar2->next = asset->nilPart->next;
  asset->nilPart->next->prev = pMVar2;
  asset->nilPart->next = pMVar2;
  pMVar2->prev = asset->nilPart;
  MeHashInsert(part->id,part,asset->nameToPart);
  part->asset = asset;
  iVar3 = MeIDPoolRequestID(asset->disabledColIndexPool);
  part->index = iVar3;
  asset->partCount = asset->partCount + 1;
  return;
}


/* ==== MeFAssetRemovePart ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFAssetRemovePart(MeFAssetPart *part)

{
  MeFAssetPart *pMVar1;
  MeFAssetPartNode *pMVar2;
  MeHash *pMVar3;
  MeFAssetPartNode *pMVar4;
  MeFAssetPartNode *pMVar5;
  
                    /* Unresolved local var: MeFAssetPartNode * node@[DW_OP_reg3(EBX)] */
  if (part->asset != (MeFAsset *)0x0) {
    pMVar4 = part->asset->nilPart;
    pMVar2 = pMVar4->prev;
    if (pMVar2 != pMVar4) {
      pMVar1 = pMVar2->current;
      while ((pMVar1 != part && (pMVar2 = pMVar2->prev, pMVar2 != pMVar4))) {
        pMVar1 = pMVar2->current;
      }
    }
    pMVar5 = pMVar2->prev;
    pMVar5->next = pMVar2->next;
    pMVar2->next->prev = pMVar5;
    pMVar3 = part->asset->nameToPart;
    pMVar5 = pMVar4;
    MeHashDelete(part->id,pMVar3);
    (*__FSetStringProperty)(pMVar2,pMVar3,pMVar4,pMVar5);
    MeFAssetPartEnableAllCollisions(part);
    MeIDPoolReturnID(part->asset->disabledColIndexPool,part->index);
    part->index = -1;
    part->asset->partCount = part->asset->partCount + -1;
    part->asset = (MeFAsset *)0x0;
  }
  return;
}


/* ==== MeFAssetResolveGeometryReferences ==== */

void MeFAssetResolveGeometryReferences(MeFAsset *asset,char *oldName,char *newName)

{
  MeFModel *pMVar1;
  int iVar2;
  MeFModelIt it;
  
                    /* Unresolved local var: MeFModel * model@[DW_OP_reg3(EBX)] */
  MeFAssetInitModelIterator(asset,&it);
  while (pMVar1 = MeFAssetGetModel(&it), pMVar1 != (MeFModel *)0x0) {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    if ((pMVar1->geometry != (char *)0x0) && (iVar2 = strcmp(pMVar1->geometry,oldName), iVar2 == 0))
    {
      _FSetStringProperty(&pMVar1->geometry,newName);
    }
  }
  return;
}


/* ==== MeFAssetResolveModelReferences ==== */

void MeFAssetResolveModelReferences(MeFAsset *asset,char *oldName,char *newName)

{
  MeFAssetPart *pMVar1;
  int iVar2;
  MeFAssetPartIt it;
  
                    /* Unresolved local var: MeFAssetPart * part@[DW_OP_reg3(EBX)] */
  MeFAssetInitPartIterator(asset,&it);
  while (pMVar1 = MeFAssetGetPart(&it), pMVar1 != (MeFAssetPart *)0x0) {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    if ((pMVar1->model != (char *)0x0) && (iVar2 = strcmp(pMVar1->model,oldName), iVar2 == 0)) {
      _FSetStringProperty(&pMVar1->model,newName);
    }
  }
  return;
}


/* ==== MeFAssetResolvePartReferences ==== */

void MeFAssetResolvePartReferences(MeFAsset *asset,char *oldName,char *newName)

{
  MeFJoint *pMVar1;
  int iVar2;
  MeFAssetPart *pMVar3;
  MeFAssetPartIt it_1;
  MeFJointIt it;
  
                    /* Unresolved local var: MeFJoint * joint@[DW_OP_reg3(EBX)] */
  MeFAssetInitJointIterator(asset,&it);
  while (pMVar1 = MeFAssetGetJoint(&it), pMVar1 != (MeFJoint *)0x0) {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    if ((pMVar1->part[0] != (char *)0x0) && (iVar2 = strcmp(pMVar1->part[0],oldName), iVar2 == 0)) {
      _FSetStringProperty(pMVar1->part,newName);
    }
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    if ((pMVar1->part[1] != (char *)0x0) && (iVar2 = strcmp(pMVar1->part[1],oldName), iVar2 == 0)) {
      _FSetStringProperty(pMVar1->part + 1,newName);
    }
  }
                    /* Unresolved local var: MeFAssetPart * part@[DW_OP_reg3(EBX)] */
  MeFAssetInitPartIterator(asset,&it_1);
  while (pMVar3 = MeFAssetGetPart(&it_1), pMVar3 != (MeFAssetPart *)0x0) {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    if ((pMVar3->parent != (char *)0x0) && (iVar2 = strcmp(pMVar3->parent,oldName), iVar2 == 0)) {
      _FSetStringProperty(&pMVar3->parent,newName);
    }
  }
  return;
}


/* ==== MeFAssetCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFAsset * MeFAssetCreate(char *name,int id)

{
  MeFAsset *asset;
  MeFGeometryNode *pMVar1;
  MeFModelNode *pMVar2;
  MeFAssetPartNode *pMVar3;
  MeFJointNode *pMVar4;
  MeHash *pMVar5;
  MeU32 *pMVar6;
  MeIDPool *pMVar7;
  
                    /* Unresolved local var: MeFAsset * asset@[DW_OP_reg3(EBX)] */
  asset = (MeFAsset *)(*_MeHashCreate)(0x5c);
  pMVar1 = (MeFGeometryNode *)(*_MeMemoryAPI)(0xc);
  asset->nilGeometry = pMVar1;
  pMVar2 = (MeFModelNode *)(*_MeMemoryAPI)(0xc);
  asset->nilModel = pMVar2;
  pMVar3 = (MeFAssetPartNode *)(*_MeMemoryAPI)(0xc);
  asset->nilPart = pMVar3;
  pMVar4 = (MeFJointNode *)(*_MeMemoryAPI)(0xc);
  asset->nilJoint = pMVar4;
  pMVar5 = MeHashCreate(0x11);
  asset->nameToGeometry = pMVar5;
  pMVar5 = MeHashCreate(0x11);
  asset->nameToModel = pMVar5;
  pMVar5 = MeHashCreate(0x11);
  asset->nameToPart = pMVar5;
  pMVar5 = MeHashCreate(0x11);
  asset->maxParts = 0x100;
  asset->nameToJoint = pMVar5;
  pMVar6 = (MeU32 *)(*_MeHashCreate)(0x2000);
  asset->disabledColArray = pMVar6;
  pMVar7 = MeIDPoolCreate();
  asset->disabledColIndexPool = pMVar7;
  _MeFAssetInit(asset);
  _FSetStringProperty(asset,name);
  asset->id = id;
  return asset;
}


/* ==== MeFAssetGetName ==== */

char * MeFAssetGetName(MeFAsset *asset)

{
  return asset->name;
}


/* ==== MeFAssetGetID ==== */

int MeFAssetGetID(MeFAsset *asset)

{
  return asset->id;
}


/* ==== MeFAssetGetReferencePart ==== */

char * MeFAssetGetReferencePart(MeFAsset *asset)

{
  return asset->refPart;
}


/* ==== MeFAssetGetGraphicHint ==== */

char * MeFAssetGetGraphicHint(MeFAsset *asset)

{
  return asset->graphicHint;
}


/* ==== MeFAssetGetGraphicScale ==== */

MeReal MeFAssetGetGraphicScale(MeFAsset *asset)

{
  return asset->graphicScale;
}


/* ==== MeFAssetGetGeometryCount ==== */

int MeFAssetGetGeometryCount(MeFAsset *asset)

{
  return asset->geomCount;
}


/* ==== MeFAssetGetModelCount ==== */

int MeFAssetGetModelCount(MeFAsset *asset)

{
  return asset->modelCount;
}


/* ==== MeFAssetGetJointCount ==== */

int MeFAssetGetJointCount(MeFAsset *asset)

{
  return asset->jointCount;
}


/* ==== MeFAssetLookupGeometry ==== */

MeFGeometry * MeFAssetLookupGeometry(MeFAsset *asset,char *name)

{
  MeFGeometry *pMVar1;
  
  pMVar1 = MeHashLookup(name,asset->nameToGeometry);
  return pMVar1;
}


/* ==== MeFAssetLookupModel ==== */

MeFModel * MeFAssetLookupModel(MeFAsset *asset,char *name)

{
  MeFModel *pMVar1;
  
  pMVar1 = MeHashLookup(name,asset->nameToModel);
  return pMVar1;
}


/* ==== MeFAssetLookupJoint ==== */

MeFJoint * MeFAssetLookupJoint(MeFAsset *asset,char *name)

{
  MeFJoint *pMVar1;
  
  pMVar1 = MeHashLookup(name,asset->nameToJoint);
  return pMVar1;
}


/* ==== MeFAssetGetGeometrySortedByName ==== */

void MeFAssetGetGeometrySortedByName(MeFAsset *asset,MeFGeometry **geomArray)

{
  MeFGeometry *pMVar1;
  int iVar2;
  MeFGeometryIt it;
  
                    /* Unresolved local var: MeFGeometry * geom@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar2 = 0;
  MeFAssetInitGeometryIterator(asset,&it);
  while( true ) {
    pMVar1 = MeFAssetGetGeometry(&it);
    if (pMVar1 == (MeFGeometry *)0x0) break;
    geomArray[iVar2] = pMVar1;
    iVar2 = iVar2 + 1;
  }
  qsort(geomArray,asset->geomCount,4,_MeFGeometrySortFunc);
  return;
}


/* ==== MeFAssetGetModelsSortedByName ==== */

void MeFAssetGetModelsSortedByName(MeFAsset *asset,MeFModel **modelArray)

{
  MeFModel *pMVar1;
  int iVar2;
  MeFModelIt it;
  
                    /* Unresolved local var: MeFModel * model@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar2 = 0;
  MeFAssetInitModelIterator(asset,&it);
  while( true ) {
    pMVar1 = MeFAssetGetModel(&it);
    if (pMVar1 == (MeFModel *)0x0) break;
    modelArray[iVar2] = pMVar1;
    iVar2 = iVar2 + 1;
  }
  qsort(modelArray,asset->modelCount,4,_MeFModelSortFunc);
  return;
}


/* ==== MeFAssetGetJointsSortedByName ==== */

void MeFAssetGetJointsSortedByName(MeFAsset *asset,MeFJoint **jointArray)

{
  MeFJoint *pMVar1;
  int iVar2;
  MeFJointIt it;
  
                    /* Unresolved local var: MeFJoint * joint@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar2 = 0;
  MeFAssetInitJointIterator(asset,&it);
  while( true ) {
    pMVar1 = MeFAssetGetJoint(&it);
    if (pMVar1 == (MeFJoint *)0x0) break;
    jointArray[iVar2] = pMVar1;
    iVar2 = iVar2 + 1;
  }
  qsort(jointArray,asset->jointCount,4,_MeFJointSortFunc);
  return;
}


/* ==== MeFAssetIsEmpty ==== */

MeBool MeFAssetIsEmpty(MeFAsset *asset)

{
  return (uint)(asset->modelCount + asset->geomCount + asset->partCount + asset->jointCount == 0);
}


/* ==== MeFAssetGetMassScale ==== */

MeReal MeFAssetGetMassScale(MeFAsset *asset)

{
  return asset->massScale;
}


/* ==== MeFAssetGetLengthScale ==== */

MeReal MeFAssetGetLengthScale(MeFAsset *asset)

{
  return asset->lengthScale;
}


/* ==== MeFAssetRename ==== */

void MeFAssetRename(MeFAsset *asset,char *name)

{
  _FSetStringProperty(asset,name);
  return;
}


/* ==== MeFAssetSetID ==== */

void MeFAssetSetID(MeFAsset *asset,int id)

{
  asset->id = id;
  return;
}


/* ==== MeFAssetSetReferencePart ==== */

void MeFAssetSetReferencePart(MeFAsset *asset,char *refPart)

{
  _FSetStringProperty(&asset->refPart,refPart);
  return;
}


/* ==== MeFAssetSetGraphicHint ==== */

void MeFAssetSetGraphicHint(MeFAsset *asset,char *hint)

{
  _FSetStringProperty(&asset->graphicHint,hint);
  return;
}


/* ==== MeFAssetSetGraphicScale ==== */

void MeFAssetSetGraphicScale(MeFAsset *asset,MeReal scale)

{
  asset->graphicScale = scale;
  return;
}


/* ==== MeFAssetRemoveAllGeometry ==== */

void MeFAssetRemoveAllGeometry(MeFAsset *asset)

{
  MeFGeometry *geometry;
  MeFGeometryIt it;
  
                    /* Unresolved local var: MeFGeometry * geom@[DW_OP_reg0(EAX)] */
  while( true ) {
    MeFAssetInitGeometryIterator(asset,&it);
    geometry = MeFAssetGetGeometry(&it);
    if (geometry == (MeFGeometry *)0x0) break;
    MeFAssetRemoveGeometry(geometry);
  }
  return;
}


/* ==== MeFAssetRemoveAllModels ==== */

void MeFAssetRemoveAllModels(MeFAsset *asset)

{
  MeFModel *model;
  MeFModelIt it;
  
                    /* Unresolved local var: MeFModel * model@[DW_OP_reg0(EAX)] */
  while( true ) {
    MeFAssetInitModelIterator(asset,&it);
    model = MeFAssetGetModel(&it);
    if (model == (MeFModel *)0x0) break;
    MeFAssetRemoveModel(model);
  }
  return;
}


/* ==== MeFAssetRemoveAllParts ==== */

void MeFAssetRemoveAllParts(MeFAsset *asset)

{
  MeFAssetPart *part;
  MeFAssetPartIt it;
  
                    /* Unresolved local var: MeFAssetPart * part@[DW_OP_reg0(EAX)] */
  while( true ) {
    MeFAssetInitPartIterator(asset,&it);
    part = MeFAssetGetPart(&it);
    if (part == (MeFAssetPart *)0x0) break;
    MeFAssetRemovePart(part);
  }
  return;
}


/* ==== MeFAssetRemoveAllJoints ==== */

void MeFAssetRemoveAllJoints(MeFAsset *asset)

{
  MeFJoint *joint;
  MeFJointIt it;
  
                    /* Unresolved local var: MeFJoint * joint@[DW_OP_reg0(EAX)] */
  while( true ) {
    MeFAssetInitJointIterator(asset,&it);
    joint = MeFAssetGetJoint(&it);
    if (joint == (MeFJoint *)0x0) break;
    MeFAssetRemoveJoint(joint);
  }
  return;
}


/* ==== MeFAssetScale ==== */

void MeFAssetScale(MeFAsset *asset,MeReal scale)

{
  MeFGeometry *pMVar1;
  MeFModel *pMVar2;
  MeFAssetPart *pMVar3;
  MeFJoint *pMVar4;
  MeFJointIt jointIt;
  MeFAssetPartIt partIt;
  MeFModelIt modelIt;
  MeFGeometryIt geomIt;
  
                    /* Unresolved local var: MeFGeometry * geom@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeFModel * model@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeFAssetPart * part@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeFJoint * joint@[DW_OP_reg0(EAX)] */
  MeFAssetInitGeometryIterator(asset,&geomIt);
  while( true ) {
    pMVar1 = MeFAssetGetGeometry(&geomIt);
    if (pMVar1 == (MeFGeometry *)0x0) break;
    MeFGeometryScale(pMVar1,scale);
  }
  MeFAssetInitModelIterator(asset,&modelIt);
  while( true ) {
    pMVar2 = MeFAssetGetModel(&modelIt);
    if (pMVar2 == (MeFModel *)0x0) break;
    MeFModelScale(pMVar2,scale);
  }
  MeFAssetInitPartIterator(asset,&partIt);
  while( true ) {
    pMVar3 = MeFAssetGetPart(&partIt);
    if (pMVar3 == (MeFAssetPart *)0x0) break;
    MeFAssetPartScale(pMVar3,scale);
  }
  MeFAssetInitJointIterator(asset,&jointIt);
  while( true ) {
    pMVar4 = MeFAssetGetJoint(&jointIt);
    if (pMVar4 == (MeFJoint *)0x0) break;
    MeFJointScale(pMVar4,scale);
  }
  asset->graphicScale = scale * asset->graphicScale;
  return;
}


/* ==== MeFAssetSetMassScale ==== */

void MeFAssetSetMassScale(MeFAsset *asset,MeReal scale)

{
  asset->massScale = scale;
  return;
}


/* ==== MeFAssetSetLengthScale ==== */

void MeFAssetSetLengthScale(MeFAsset *asset,MeReal scale)

{
  asset->lengthScale = scale;
  return;
}


/* ==== _MeFAssetInit ==== */

void _MeFAssetInit(MeFAsset *asset)

{
  MeFGeometryNode *pMVar1;
  MeFModelNode *pMVar2;
  MeFAssetPartNode *pMVar3;
  MeFJointNode *pMVar4;
  int iVar5;
  
  pMVar1 = asset->nilGeometry;
  asset->name = (char *)0x0;
  asset->id = 0;
  asset->db = (MeAssetDB *)0x0;
  asset->graphicHint = (char *)0x0;
  asset->refPart = (char *)0x0;
  asset->graphicScale = 1.0;
  asset->geomCount = 0;
  asset->modelCount = 0;
  asset->partCount = 0;
  asset->jointCount = 0;
  asset->massScale = 1.0;
  asset->lengthScale = 1.0;
  pMVar1->prev = pMVar1;
  pMVar1->next = pMVar1;
  asset->nilGeometry->current = (MeFGeometry *)0x0;
  pMVar2 = asset->nilModel;
  pMVar2->prev = pMVar2;
  pMVar2->next = pMVar2;
  asset->nilModel->current = (MeFModel *)0x0;
  pMVar3 = asset->nilPart;
  pMVar3->prev = pMVar3;
  pMVar3->next = pMVar3;
  asset->nilPart->current = (MeFAssetPart *)0x0;
  pMVar4 = asset->nilJoint;
  pMVar4->prev = pMVar4;
  pMVar4->next = pMVar4;
  asset->nilJoint->current = (MeFJoint *)0x0;
  MeIDPoolReset(asset->disabledColIndexPool);
                    /* Unresolved local var: void * __s@[DW_OP_reg2(EDX)] */
  iVar5 = asset->maxParts * asset->maxParts;
  if (iVar5 < 0) {
    iVar5 = iVar5 + 7;
  }
  memset(asset->disabledColArray,0,iVar5 >> 3);
  return;
}


/* ==== _MeFGeometrySortFunc ==== */

int _MeFGeometrySortFunc(void *elem1,void *elem2)

{
  int iVar1;
  
                    /* Unresolved local var: MeFGeometry * geom1@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeFGeometry * geom2@[DW_OP_reg0(EAX)] */
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar1 = strcmp((char *)**(undefined4 **)elem1,(char *)**(undefined4 **)elem2);
  return iVar1;
}


/* ==== _MeFModelSortFunc ==== */

int _MeFModelSortFunc(void *elem1,void *elem2)

{
  int iVar1;
  
                    /* Unresolved local var: MeFModel * model1@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeFModel * model2@[DW_OP_reg0(EAX)] */
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar1 = strcmp((char *)**(undefined4 **)elem1,(char *)**(undefined4 **)elem2);
  return iVar1;
}


/* ==== _MeFAssetPartSortFunc ==== */

int _MeFAssetPartSortFunc(void *elem1,void *elem2)

{
  int iVar1;
  
                    /* Unresolved local var: MeFAssetPart * part1@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeFAssetPart * part2@[DW_OP_reg0(EAX)] */
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar1 = strcmp((char *)**(undefined4 **)elem1,(char *)**(undefined4 **)elem2);
  return iVar1;
}


/* ==== _MeFJointSortFunc ==== */

int _MeFJointSortFunc(void *elem1,void *elem2)

{
  int iVar1;
  
                    /* Unresolved local var: MeFJoint * joint1@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeFJoint * joint2@[DW_OP_reg0(EAX)] */
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar1 = strcmp((char *)**(undefined4 **)elem1,(char *)**(undefined4 **)elem2);
  return iVar1;
}


