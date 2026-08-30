/* ==== KaFileWriteXML_1_0 ==== */

MeXMLElementID KaFileWriteXML_1_0(MeXMLOutput *op,MeFAsset *fa,MeXMLElementID parent)

{
  bool bVar1;
  MeXMLElementID MVar2;
  int iVar3;
  void *pvVar4;
  uint uVar5;
  uint uVar6;
  MeXMLElementID MVar7;
  int iVar8;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar9;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 extraout_EDX_01;
  undefined4 extraout_EDX_02;
  undefined4 extraout_EDX_03;
  undefined4 extraout_EDX_04;
  undefined4 extraout_EDX_05;
  undefined4 extraout_EDX_06;
  int iVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  char *__s;
  int iVar14;
  MeFAsset *pMVar15;
  float fVar16;
  undefined4 local_440;
  void *pvStack_430;
  int local_42c;
  int count;
  int i;
  MeXMLElementID asset;
  char buffer [1024];
  
                    /* Unresolved local var: char * pBuf@[DW_OP_reg6(ESI)]
                       Unresolved local var: char * p@[DW_OP_reg3(EBX)] */
  local_440 = 0x1001b;
  MeFAssetGetName(fa);
  local_440 = 0x10030;
  iVar3 = sprintf(buffer,"ASSET id=\"%s\"");
  __s = buffer + iVar3;
  local_440 = 0x1003c;
  pvVar4 = MeFAssetGetReferencePart(fa);
  if (pvVar4 != (void *)0x0) {
    local_440 = 0x10052;
    pvStack_430 = pvVar4;
    iVar3 = sprintf(__s," ref_part=\"%s\"");
    __s = __s + iVar3;
  }
  local_440 = 0x10063;
  pvVar4 = MeFAssetGetGraphicHint(fa);
  if (pvVar4 != (void *)0x0) {
    local_440 = 0x10078;
    fVar16 = MeFAssetGetGraphicScale(fa);
    local_440 = SUB84((double)fVar16,0);
    iVar3 = sprintf(__s," graphic=\"%s\" scale=\"%.7g\"",pvVar4);
    __s = __s + iVar3;
  }
  local_440 = 0x10099;
  MeFAssetGetLengthScale(fa);
  fVar16 = MeFAssetGetMassScale(fa);
  local_440 = (undefined4)((ulonglong)(double)fVar16 >> 0x20);
  sprintf(__s," mass_scale=\"%.7g\" length_scale=\"%.7g\"",SUB84((double)fVar16,0));
  local_440 = 0x100d0;
  asset = MeXMLWriteElement(op,parent,buffer);
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeFGeometry * * geomArray@[DW_OP_reg6(ESI)] */
  local_440 = 0x100e0;
  uVar5 = MeFAssetGetGeometryCount(fa);
  iVar3 = -(uVar5 * 4 + 0xf & 0xfffffff0);
  *(uint *)((int)&pvStack_430 + iVar3) = uVar5;
  *(uint *)(buffer + iVar3 + -0x18) = uVar5;
  *(int *)(buffer + iVar3 + -0x1c) = (int)&local_42c + iVar3;
  *(MeFAsset **)(buffer + iVar3 + -0x20) = fa;
  iVar10 = 0;
  *(undefined4 *)((int)&local_440 + iVar3) = 0x10101;
  MeFAssetGetGeometrySortedByName
            (*(void **)(buffer + iVar3 + -0x20),*(void **)(buffer + iVar3 + -0x1c));
  MVar2 = asset;
  if (0 < (int)uVar5) {
    uVar6 = uVar5 & 3;
    if ((int)uVar5 < 2) {
LAB_0001011a:
      MVar2 = asset;
      *(uint *)((int)&pvStack_430 + iVar3) = uVar6;
      *(MeXMLElementID *)(buffer + iVar3 + -0x18) = MVar2;
      iVar11 = iVar10 * 4;
      iVar10 = iVar10 + 1;
      *(undefined4 *)(buffer + iVar3 + -0x1c) = *(undefined4 *)((int)&local_42c + iVar11 + iVar3);
      *(MeXMLOutput **)(buffer + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar3) = 0x10130;
      MeFGeometryWriteXML_1_0
                (*(MeXMLOutput **)(buffer + iVar3 + -0x20),*(MeFGeometry **)(buffer + iVar3 + -0x1c)
                 ,*(MeXMLElementID *)(buffer + iVar3 + -0x18));
      uVar9 = extraout_EDX_00;
      if ((int)uVar5 <= iVar10) goto LAB_0001019e;
    }
    else {
      uVar9 = extraout_EDX;
      if (uVar6 != 0) {
        if (1 < uVar6) {
          if (2 < uVar6) {
            *(uint *)((int)&pvStack_430 + iVar3) = uVar6;
            *(MeXMLElementID *)(buffer + iVar3 + -0x18) = MVar2;
            *(undefined4 *)(buffer + iVar3 + -0x1c) = *(undefined4 *)((int)&local_42c + iVar3);
            *(MeXMLOutput **)(buffer + iVar3 + -0x20) = op;
            *(undefined4 *)((int)&local_440 + iVar3) = 0x107db;
            MeFGeometryWriteXML_1_0
                      (*(MeXMLOutput **)(buffer + iVar3 + -0x20),
                       *(MeFGeometry **)(buffer + iVar3 + -0x1c),
                       *(MeXMLElementID *)(buffer + iVar3 + -0x18));
            uVar9 = extraout_EDX_06;
          }
          uVar6 = (uint)(2 < uVar6);
          *(undefined4 *)((int)&pvStack_430 + iVar3) = uVar9;
          *(MeXMLElementID *)(buffer + iVar3 + -0x18) = asset;
          iVar10 = uVar6 + 1;
          *(undefined4 *)(buffer + iVar3 + -0x1c) =
               *(undefined4 *)((int)&local_42c + uVar6 * 4 + iVar3);
          *(MeXMLOutput **)(buffer + iVar3 + -0x20) = op;
          *(undefined4 *)((int)&local_440 + iVar3) = 0x107ba;
          uVar6 = MeFGeometryWriteXML_1_0
                            (*(MeXMLOutput **)(buffer + iVar3 + -0x20),
                             *(MeFGeometry **)(buffer + iVar3 + -0x1c),
                             *(MeXMLElementID *)(buffer + iVar3 + -0x18));
        }
        goto LAB_0001011a;
      }
    }
    do {
      *(undefined4 *)((int)&pvStack_430 + iVar3) = uVar9;
      *(MeXMLElementID *)(buffer + iVar3 + -0x18) = asset;
      *(undefined4 *)(buffer + iVar3 + -0x1c) =
           *(undefined4 *)((int)&local_42c + iVar10 * 4 + iVar3);
      *(MeXMLOutput **)(buffer + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar3) = 0x1014c;
      MeFGeometryWriteXML_1_0
                (*(MeXMLOutput **)(buffer + iVar3 + -0x20),*(MeFGeometry **)(buffer + iVar3 + -0x1c)
                 ,*(MeXMLElementID *)(buffer + iVar3 + -0x18));
      *(MeXMLElementID *)(buffer + iVar3 + -0x18) = asset;
      *(undefined4 *)(buffer + iVar3 + -0x1c) = *(undefined4 *)((int)&count + iVar10 * 4 + iVar3);
      *(MeXMLOutput **)(buffer + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar3) = 0x10164;
      MeFGeometryWriteXML_1_0
                (*(MeXMLOutput **)(buffer + iVar3 + -0x20),*(MeFGeometry **)(buffer + iVar3 + -0x1c)
                 ,*(MeXMLElementID *)(buffer + iVar3 + -0x18));
      *(MeXMLElementID *)(buffer + iVar3 + -0x18) = asset;
      *(undefined4 *)(buffer + iVar3 + -0x1c) = *(undefined4 *)((int)&i + iVar10 * 4 + iVar3);
      *(MeXMLOutput **)(buffer + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar3) = 0x1017c;
      MeFGeometryWriteXML_1_0
                (*(MeXMLOutput **)(buffer + iVar3 + -0x20),*(MeFGeometry **)(buffer + iVar3 + -0x1c)
                 ,*(MeXMLElementID *)(buffer + iVar3 + -0x18));
      *(MeXMLElementID *)(buffer + iVar3 + -0x18) = asset;
      iVar11 = iVar10 * 4;
      iVar10 = iVar10 + 4;
      *(undefined4 *)(buffer + iVar3 + -0x1c) = *(undefined4 *)(buffer + iVar11 + iVar3 + -4);
      *(MeXMLOutput **)(buffer + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar3) = 0x10197;
      MeFGeometryWriteXML_1_0
                (*(MeXMLOutput **)(buffer + iVar3 + -0x20),*(MeFGeometry **)(buffer + iVar3 + -0x1c)
                 ,*(MeXMLElementID *)(buffer + iVar3 + -0x18));
      uVar9 = extraout_EDX_01;
    } while (iVar10 < (int)uVar5);
  }
LAB_0001019e:
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeFModel * * modelArray@[DW_OP_reg6(ESI)] */
  iVar11 = 0;
  *(MeFAsset **)(buffer + iVar3 + -0x20) = fa;
  *(undefined4 *)((int)&local_440 + iVar3) = 0x101ac;
  uVar5 = MeFAssetGetModelCount(*(void **)(buffer + iVar3 + -0x20));
  iVar10 = -(uVar5 * 4 + 0xf & 0xfffffff0);
  *(uint *)((int)&pvStack_430 + iVar10 + iVar3) = uVar5;
  *(uint *)(buffer + iVar10 + iVar3 + -0x18) = uVar5;
  *(int *)(buffer + iVar10 + iVar3 + -0x1c) = (int)&local_42c + iVar10 + iVar3;
  *(MeFAsset **)(buffer + iVar10 + iVar3 + -0x20) = fa;
  *(undefined4 *)((int)&local_440 + iVar10 + iVar3) = 0x101cb;
  MeFAssetGetModelsSortedByName
            (*(void **)(buffer + iVar10 + iVar3 + -0x20),*(void **)(buffer + iVar10 + iVar3 + -0x1c)
            );
  MVar2 = asset;
  if (0 < (int)uVar5) {
    uVar6 = uVar5 & 3;
    uVar9 = extraout_ECX;
    if ((int)uVar5 < 2) {
LAB_000101e4:
      MVar2 = asset;
      *(undefined4 *)((int)&pvStack_430 + iVar10 + iVar3) = uVar9;
      *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18) = MVar2;
      iVar8 = iVar11 * 4;
      iVar11 = iVar11 + 1;
      *(undefined4 *)(buffer + iVar10 + iVar3 + -0x1c) =
           *(undefined4 *)((int)&local_42c + iVar8 + iVar10 + iVar3);
      *(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar10 + iVar3) = 0x101fa;
      MVar7 = MeFModelWriteXML_1_0
                        (*(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20),
                         *(MeFModel **)(buffer + iVar10 + iVar3 + -0x1c),
                         *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18));
      if ((int)uVar5 <= iVar11) goto LAB_00010268;
    }
    else {
      MVar7 = 0;
      if (uVar6 != 0) {
        if (1 < uVar6) {
          bVar1 = 2 < uVar6;
          if (bVar1) {
            *(undefined4 *)((int)&pvStack_430 + iVar10 + iVar3) = extraout_ECX;
            *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18) = MVar2;
            *(undefined4 *)(buffer + iVar10 + iVar3 + -0x1c) =
                 *(undefined4 *)((int)&local_42c + iVar10 + iVar3);
            *(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20) = op;
            *(undefined4 *)((int)&local_440 + iVar10 + iVar3) = 0x10789;
            uVar6 = MeFModelWriteXML_1_0
                              (*(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20),
                               *(MeFModel **)(buffer + iVar10 + iVar3 + -0x1c),
                               *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18));
          }
          MVar2 = asset;
          uVar13 = (uint)bVar1;
          *(uint *)((int)&pvStack_430 + iVar10 + iVar3) = uVar6;
          *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18) = MVar2;
          iVar11 = uVar13 + 1;
          *(undefined4 *)(buffer + iVar10 + iVar3 + -0x1c) =
               *(undefined4 *)((int)&local_42c + uVar13 * 4 + iVar10 + iVar3);
          *(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20) = op;
          *(undefined4 *)((int)&local_440 + iVar10 + iVar3) = 0x10768;
          MeFModelWriteXML_1_0
                    (*(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20),
                     *(MeFModel **)(buffer + iVar10 + iVar3 + -0x1c),
                     *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18));
          uVar9 = extraout_ECX_03;
        }
        goto LAB_000101e4;
      }
    }
    do {
      MVar2 = asset;
      *(MeXMLElementID *)((int)&pvStack_430 + iVar10 + iVar3) = MVar7;
      *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18) = MVar2;
      *(undefined4 *)(buffer + iVar10 + iVar3 + -0x1c) =
           *(undefined4 *)((int)&local_42c + iVar11 * 4 + iVar10 + iVar3);
      *(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar10 + iVar3) = 0x10216;
      MeFModelWriteXML_1_0
                (*(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20),
                 *(MeFModel **)(buffer + iVar10 + iVar3 + -0x1c),
                 *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18));
      *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18) = asset;
      *(undefined4 *)(buffer + iVar10 + iVar3 + -0x1c) =
           *(undefined4 *)((int)&count + iVar11 * 4 + iVar10 + iVar3);
      *(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar10 + iVar3) = 0x1022e;
      MeFModelWriteXML_1_0
                (*(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20),
                 *(MeFModel **)(buffer + iVar10 + iVar3 + -0x1c),
                 *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18));
      *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18) = asset;
      *(undefined4 *)(buffer + iVar10 + iVar3 + -0x1c) =
           *(undefined4 *)((int)&i + iVar11 * 4 + iVar10 + iVar3);
      *(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar10 + iVar3) = 0x10246;
      MeFModelWriteXML_1_0
                (*(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20),
                 *(MeFModel **)(buffer + iVar10 + iVar3 + -0x1c),
                 *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18));
      *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18) = asset;
      iVar8 = iVar11 * 4;
      iVar11 = iVar11 + 4;
      *(undefined4 *)(buffer + iVar10 + iVar3 + -0x1c) =
           *(undefined4 *)(buffer + iVar8 + iVar10 + iVar3 + -4);
      *(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar10 + iVar3) = 0x10261;
      MVar7 = MeFModelWriteXML_1_0
                        (*(MeXMLOutput **)(buffer + iVar10 + iVar3 + -0x20),
                         *(MeFModel **)(buffer + iVar10 + iVar3 + -0x1c),
                         *(MeXMLElementID *)(buffer + iVar10 + iVar3 + -0x18));
    } while (iVar11 < (int)uVar5);
  }
LAB_00010268:
                    /* Unresolved local var: MeFAssetPart * * partArray@[DW_OP_reg7(EDI)] */
  *(MeFAsset **)(buffer + iVar10 + iVar3 + -0x20) = fa;
  *(undefined4 *)((int)&local_440 + iVar10 + iVar3) = 0x10274;
  iVar8 = MeFAssetGetPartCount(*(void **)(buffer + iVar10 + iVar3 + -0x20));
  count = iVar8;
  iVar11 = -(iVar8 * 4 + 0xfU & 0xfffffff0);
  *(int *)((int)&pvStack_430 + iVar11 + iVar10 + iVar3) = iVar8;
  *(int *)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = iVar8;
  *(int *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) = (int)&local_42c + iVar11 + iVar10 + iVar3;
  *(MeFAsset **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = fa;
  *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10297;
  MeFAssetGetPartsSortedByName
            (*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
             *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c));
  MVar2 = asset;
  i = 0;
  if (0 < count) {
    uVar5 = count & 3;
    if (count < 2) {
LAB_000102c6:
      MVar2 = asset;
      iVar8 = i;
      *(uint *)((int)&pvStack_430 + iVar11 + iVar10 + iVar3) = uVar5;
      *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = MVar2;
      *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
           *(undefined4 *)((int)&local_42c + iVar8 * 4 + iVar11 + iVar10 + iVar3);
      *(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x102e1;
      MeFAssetPartWriteXML_1_0
                (*(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                 *(MeFAssetPart **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c),
                 *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18));
      i = i + 1;
      pMVar15 = (MeFAsset *)i;
      if (i < count) goto LAB_00010303;
    }
    else {
      pMVar15 = fa;
      if (uVar5 != 0) {
        if (1 < uVar5) {
          if (2 < uVar5) {
            *(uint *)((int)&pvStack_430 + iVar11 + iVar10 + iVar3) = uVar5;
            *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = MVar2;
            *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
                 *(undefined4 *)((int)&local_42c + iVar11 + iVar10 + iVar3);
            *(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = op;
            *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x1072d;
            MeFAssetPartWriteXML_1_0
                      (*(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                       *(MeFAssetPart **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c),
                       *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18));
            i = 1;
            pMVar15 = (MeFAsset *)MVar2;
          }
          MVar2 = asset;
          i = (int)(2 < uVar5);
          *(MeFAsset **)((int)&pvStack_430 + iVar11 + iVar10 + iVar3) = pMVar15;
          iVar8 = i;
          *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = MVar2;
          *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
               *(undefined4 *)((int)&local_42c + iVar8 * 4 + iVar11 + iVar10 + iVar3);
          *(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = op;
          *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10704;
          uVar5 = MeFAssetPartWriteXML_1_0
                            (*(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                             *(MeFAssetPart **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c),
                             *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18));
          i = i + 1;
        }
        goto LAB_000102c6;
      }
LAB_00010303:
      do {
        MVar2 = asset;
        *(MeFAsset **)((int)&pvStack_430 + iVar11 + iVar10 + iVar3) = pMVar15;
        iVar8 = i;
        *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = MVar2;
        *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
             *(undefined4 *)((int)&local_42c + iVar8 * 4 + iVar11 + iVar10 + iVar3);
        *(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = op;
        *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x1031e;
        MeFAssetPartWriteXML_1_0
                  (*(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                   *(MeFAssetPart **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c),
                   *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18));
        iVar8 = i;
        *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = asset;
        *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
             *(undefined4 *)((int)&count + iVar8 * 4 + iVar11 + iVar10 + iVar3);
        *(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = op;
        *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x1033c;
        MeFAssetPartWriteXML_1_0
                  (*(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                   *(MeFAssetPart **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c),
                   *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18));
        iVar8 = i;
        *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = asset;
        *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
             *(undefined4 *)((int)&i + iVar8 * 4 + iVar11 + iVar10 + iVar3);
        *(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = op;
        *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x1035a;
        MeFAssetPartWriteXML_1_0
                  (*(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                   *(MeFAssetPart **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c),
                   *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18));
        iVar8 = i;
        *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = asset;
        pMVar15 = *(MeFAsset **)(buffer + iVar8 * 4 + iVar11 + iVar10 + iVar3 + -4);
        *(MeFAsset **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) = pMVar15;
        *(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = op;
        *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10378;
        MeFAssetPartWriteXML_1_0
                  (*(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                   *(MeFAssetPart **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c),
                   *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x18));
        i = i + 4;
      } while (i < count);
    }
    i = 0;
    if (0 < count) {
      do {
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)] */
        iVar12 = i;
        iVar8 = count;
        iVar14 = i + 1;
        local_42c = iVar14;
        if (iVar14 < count) {
          if ((count <= i + 2) || (uVar9 = 0, (count - iVar14 & 1U) != 0)) {
            *(int *)((int)&pvStack_430 + iVar11 + iVar10 + iVar3) = count;
            *(int *)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = iVar8;
            iVar8 = i;
            *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
                 *(undefined4 *)((int)&local_42c + local_42c * 4 + iVar11 + iVar10 + iVar3);
            *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x20) =
                 *(undefined4 *)((int)&local_42c + iVar8 * 4 + iVar11 + iVar10 + iVar3);
            *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x1040a;
            iVar8 = MeFAssetPartIsCollisionEnabled
                              (*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                               *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c));
            uVar9 = extraout_EDX_02;
            if (iVar8 == 0) {
              *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x20) =
                   *(undefined4 *)((int)&local_42c + local_42c * 4 + iVar11 + iVar10 + iVar3);
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10680;
              pvVar4 = MeFAssetPartGetName(*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20));
              iVar8 = i;
              *(void **)((int)&pvStack_430 + iVar11 + iVar10 + iVar3) = pvVar4;
              *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = pvVar4;
              *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) = pvVar4;
              *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x20) =
                   *(undefined4 *)((int)&local_42c + iVar8 * 4 + iVar11 + iVar10 + iVar3);
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10695;
              pvVar4 = MeFAssetPartGetName(*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20));
              *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = pvVar4;
              *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
                   "NO_COLLISION part1=\"%s\" part2=\"%s\"";
              *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = buffer;
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x106aa;
              sprintf(*(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                      *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c));
              MVar2 = asset;
              *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = buffer;
              *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) = MVar2;
              *(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = op;
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x106be;
              MeXMLWriteElement(*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                                *(int *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c),
                                *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x18));
              uVar9 = extraout_EDX_05;
            }
            iVar14 = iVar12 + 2;
            if (count <= iVar14) goto LAB_0001046e;
          }
          do {
            *(undefined4 *)((int)&pvStack_430 + iVar11 + iVar10 + iVar3) = uVar9;
            *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = uVar9;
            iVar8 = i;
            *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
                 *(undefined4 *)((int)&local_42c + iVar14 * 4 + iVar11 + iVar10 + iVar3);
            *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x20) =
                 *(undefined4 *)((int)&local_42c + iVar8 * 4 + iVar11 + iVar10 + iVar3);
            *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10435;
            iVar8 = MeFAssetPartIsCollisionEnabled
                              (*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                               *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c));
            uVar9 = extraout_ECX_00;
            if (iVar8 == 0) {
              *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x20) =
                   *(undefined4 *)((int)&local_42c + iVar14 * 4 + iVar11 + iVar10 + iVar3);
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10622;
              pvVar4 = MeFAssetPartGetName(*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20));
              iVar8 = i;
              *(void **)((int)&pvStack_430 + iVar11 + iVar10 + iVar3) = pvVar4;
              *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = pvVar4;
              *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) = pvVar4;
              *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x20) =
                   *(undefined4 *)((int)&local_42c + iVar8 * 4 + iVar11 + iVar10 + iVar3);
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10637;
              pvVar4 = MeFAssetPartGetName(*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20));
              *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = pvVar4;
              *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
                   "NO_COLLISION part1=\"%s\" part2=\"%s\"";
              *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = buffer;
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x1064c;
              sprintf(*(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                      *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c));
              MVar2 = asset;
              *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = buffer;
              *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) = MVar2;
              *(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = op;
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10660;
              MeXMLWriteElement(*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                                *(int *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c),
                                *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x18));
              uVar9 = extraout_ECX_02;
            }
            *(undefined4 *)((int)&pvStack_430 + iVar11 + iVar10 + iVar3) = uVar9;
            iVar8 = i;
            *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = uVar9;
            *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
                 *(undefined4 *)((int)&local_42c + (iVar14 + 1) * 4 + iVar11 + iVar10 + iVar3);
            *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x20) =
                 *(undefined4 *)((int)&local_42c + iVar8 * 4 + iVar11 + iVar10 + iVar3);
            *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10458;
            iVar8 = MeFAssetPartIsCollisionEnabled
                              (*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                               *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c));
            uVar9 = extraout_EDX_03;
            if (iVar8 == 0) {
              *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x20) =
                   *(undefined4 *)((int)&local_42c + (iVar14 + 1) * 4 + iVar11 + iVar10 + iVar3);
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x105bf;
              pvVar4 = MeFAssetPartGetName(*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20));
              iVar8 = i;
              *(void **)((int)&pvStack_430 + iVar11 + iVar10 + iVar3) = pvVar4;
              *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = pvVar4;
              *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) = pvVar4;
              *(undefined4 *)(buffer + iVar11 + iVar10 + iVar3 + -0x20) =
                   *(undefined4 *)((int)&local_42c + iVar8 * 4 + iVar11 + iVar10 + iVar3);
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x105d4;
              pvVar4 = MeFAssetPartGetName(*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20));
              *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = pvVar4;
              *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) =
                   "NO_COLLISION part1=\"%s\" part2=\"%s\"";
              *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = buffer;
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x105e9;
              sprintf(*(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                      *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x1c));
              MVar2 = asset;
              *(char **)(buffer + iVar11 + iVar10 + iVar3 + -0x18) = buffer;
              *(MeXMLElementID *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c) = MVar2;
              *(MeXMLOutput **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = op;
              *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10603;
              MeXMLWriteElement(*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20),
                                *(int *)(buffer + iVar11 + iVar10 + iVar3 + -0x1c),
                                *(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x18));
              uVar9 = extraout_EDX_04;
            }
            iVar14 = iVar14 + 2;
          } while (iVar14 < count);
        }
LAB_0001046e:
        i = local_42c;
      } while (local_42c < count);
    }
  }
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int count@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeFJoint * * jointArray@[DW_OP_reg6(ESI)] */
  *(MeFAsset **)(buffer + iVar11 + iVar10 + iVar3 + -0x20) = fa;
  *(undefined4 *)((int)&local_440 + iVar11 + iVar10 + iVar3) = 0x10494;
  uVar5 = MeFAssetGetJointCount(*(void **)(buffer + iVar11 + iVar10 + iVar3 + -0x20));
  iVar8 = -(uVar5 * 4 + 0xf & 0xfffffff0);
  *(undefined4 *)((int)&pvStack_430 + iVar8 + iVar11 + iVar10 + iVar3) = extraout_ECX_01;
  *(undefined4 *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18) = extraout_ECX_01;
  *(int *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c) =
       (int)&local_42c + iVar8 + iVar11 + iVar10 + iVar3;
  *(MeFAsset **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20) = fa;
  iVar12 = 0;
  *(undefined4 *)((int)&local_440 + iVar8 + iVar11 + iVar10 + iVar3) = 0x104b5;
  MeFAssetGetJointsSortedByName
            (*(void **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20),
             *(void **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c));
  MVar2 = asset;
  if ((int)uVar5 < 1) {
    return asset;
  }
  uVar6 = uVar5 & 3;
  if (1 < (int)uVar5) {
    MVar7 = 0;
    if (uVar6 == 0) goto LAB_000104f0;
    if (1 < uVar6) {
      bVar1 = 2 < uVar6;
      if (bVar1) {
        *(uint *)((int)&pvStack_430 + iVar8 + iVar11 + iVar10 + iVar3) = uVar6;
        *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18) = MVar2;
        *(undefined4 *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c) =
             *(undefined4 *)((int)&local_42c + iVar8 + iVar11 + iVar10 + iVar3);
        *(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20) = op;
        *(undefined4 *)((int)&local_440 + iVar8 + iVar11 + iVar10 + iVar3) = 0x105ae;
        uVar6 = MeFJointWriteXML_1_0
                          (*(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20),
                           *(MeFJoint **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c),
                           *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18));
      }
      MVar2 = asset;
      uVar13 = (uint)bVar1;
      *(uint *)((int)&pvStack_430 + iVar8 + iVar11 + iVar10 + iVar3) = uVar6;
      *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18) = MVar2;
      iVar12 = uVar13 + 1;
      *(undefined4 *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c) =
           *(undefined4 *)((int)&local_42c + uVar13 * 4 + iVar8 + iVar11 + iVar10 + iVar3);
      *(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20) = op;
      *(undefined4 *)((int)&local_440 + iVar8 + iVar11 + iVar10 + iVar3) = 0x1058d;
      uVar6 = MeFJointWriteXML_1_0
                        (*(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20),
                         *(MeFJoint **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c),
                         *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18));
    }
  }
  MVar2 = asset;
  *(uint *)((int)&pvStack_430 + iVar8 + iVar11 + iVar10 + iVar3) = uVar6;
  *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18) = MVar2;
  iVar14 = iVar12 * 4;
  iVar12 = iVar12 + 1;
  *(undefined4 *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c) =
       *(undefined4 *)((int)&local_42c + iVar14 + iVar8 + iVar11 + iVar10 + iVar3);
  *(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20) = op;
  *(undefined4 *)((int)&local_440 + iVar8 + iVar11 + iVar10 + iVar3) = 0x104e4;
  MVar7 = MeFJointWriteXML_1_0
                    (*(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20),
                     *(MeFJoint **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c),
                     *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18));
  if ((int)uVar5 <= iVar12) {
    return asset;
  }
LAB_000104f0:
  do {
    MVar2 = asset;
    *(MeXMLElementID *)((int)&pvStack_430 + iVar8 + iVar11 + iVar10 + iVar3) = MVar7;
    *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18) = MVar2;
    *(undefined4 *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c) =
         *(undefined4 *)((int)&local_42c + iVar12 * 4 + iVar8 + iVar11 + iVar10 + iVar3);
    *(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20) = op;
    *(undefined4 *)((int)&local_440 + iVar8 + iVar11 + iVar10 + iVar3) = 0x10505;
    MeFJointWriteXML_1_0
              (*(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20),
               *(MeFJoint **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c),
               *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18));
    *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18) = asset;
    *(undefined4 *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c) =
         *(undefined4 *)((int)&count + iVar12 * 4 + iVar8 + iVar11 + iVar10 + iVar3);
    *(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20) = op;
    *(undefined4 *)((int)&local_440 + iVar8 + iVar11 + iVar10 + iVar3) = 0x1051d;
    MeFJointWriteXML_1_0
              (*(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20),
               *(MeFJoint **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c),
               *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18));
    *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18) = asset;
    *(undefined4 *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c) =
         *(undefined4 *)((int)&i + iVar12 * 4 + iVar8 + iVar11 + iVar10 + iVar3);
    *(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20) = op;
    *(undefined4 *)((int)&local_440 + iVar8 + iVar11 + iVar10 + iVar3) = 0x10535;
    MeFJointWriteXML_1_0
              (*(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20),
               *(MeFJoint **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c),
               *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18));
    *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18) = asset;
    iVar14 = iVar12 * 4;
    iVar12 = iVar12 + 4;
    *(undefined4 *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c) =
         *(undefined4 *)(buffer + iVar14 + iVar8 + iVar11 + iVar10 + iVar3 + -4);
    *(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20) = op;
    *(undefined4 *)((int)&local_440 + iVar8 + iVar11 + iVar10 + iVar3) = 0x10550;
    MVar7 = MeFJointWriteXML_1_0
                      (*(MeXMLOutput **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x20),
                       *(MeFJoint **)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x1c),
                       *(MeXMLElementID *)(buffer + iVar8 + iVar11 + iVar10 + iVar3 + -0x18));
  } while (iVar12 < (int)uVar5);
  return asset;
}


/* ==== MeFGeometryWriteXML_1_0 ==== */

MeXMLElementID MeFGeometryWriteXML_1_0(MeXMLOutput *op,MeFGeometry *fg,MeXMLElementID parent)

{
  int iVar1;
  void *pvVar2;
  MeFPrimitive *prim;
  float fVar3;
  double dVar4;
  double dVar5;
  double dVar6;
  void *pvVar7;
  MeFPrimitiveIt it;
  MeVector3 v;
  char buffer [1024];
  
                    /* Unresolved local var: char * pBuf@[DW_OP_reg6(ESI)]
                       Unresolved local var: char * p@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeXMLElementID elem@[DW_OP_reg3(EBX)] */
  MeFGeometryGetName(fg);
  iVar1 = sprintf(buffer,"GEOMETRY id=\"%s\"");
  pvVar2 = MeFGeometryGetGraphicHint(fg);
  if (pvVar2 != (void *)0x0) {
    pvVar7 = pvVar2;
    MeFGeometryGetGraphicOffset(fg,v);
    dVar6 = (double)v[2];
    dVar5 = (double)v[1];
    dVar4 = (double)v[0];
    fVar3 = MeFGeometryGetGraphicScale(fg);
    sprintf(buffer + iVar1," graphic=\"%s\" scale=\"%.7g\" graphic_offset=\"%.7g,%.7g,%.7g\"",pvVar2
            ,(double)fVar3,dVar4,dVar5,dVar6,pvVar7);
  }
  iVar1 = MeXMLWriteElement(op,parent,buffer);
                    /* Unresolved local var: MeFPrimitive * prim@[DW_OP_reg0(EAX)] */
  MeFGeometryInitPrimitiveIterator(fg,&it);
  while( true ) {
    prim = MeFGeometryGetPrimitive(&it);
    if (prim == (MeFPrimitive *)0x0) break;
    MeFPrimitiveWriteXML_1_0(op,prim,iVar1);
  }
  return iVar1;
}


/* ==== MeFPrimitiveWriteXML_1_0 ==== */

MeXMLElementID MeFPrimitiveWriteXML_1_0(MeXMLOutput *op,MeFPrimitive *prim,MeXMLElementID parent)

{
  char *__s;
  MeFPrimitiveType MVar1;
  void *pvVar2;
  int iVar3;
  int unaff_ESI;
  char *__format;
  MeFVertexIt it;
  MeVector3 v;
  char buffer [1024];
  
                    /* Unresolved local var: MeXMLElementID part@[DW_OP_reg6(ESI)]
                       Unresolved local var: char * pBuf@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg0(EAX)] */
  pvVar2 = MeFPrimitiveGetName(prim);
  iVar3 = sprintf(buffer,"PRIMITIVE id=\"%s\"",pvVar2);
  __s = buffer + iVar3;
  MVar1 = prim->type;
  switch(MVar1) {
  case kMeFPrimitiveTypeSphere:
    sprintf(__s," type=\"sphere\"",MVar1);
    unaff_ESI = MeXMLWriteElement(op,parent,buffer);
    MeXMLWriteElement(op,unaff_ESI,"RADIUS");
    MeFPrimitiveGetRadius(prim);
    goto LAB_0001093b;
  case kMeFPrimitiveTypeBox:
    sprintf(__s," type=\"box\"");
    unaff_ESI = MeXMLWriteElement(op,parent,buffer);
    MeFPrimitiveGetDimensions(prim,v);
    MeXMLWriteElement(op,unaff_ESI,&DAT_00012611);
    MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g");
    break;
  case kMeFPrimitiveTypeCylinder:
    __format = " type=\"cylinder\"";
    goto LAB_00010a62;
  case kMeFPrimitiveTypeSphyl:
    __format = " type=\"sphyl\"";
LAB_00010a62:
    sprintf(__s,__format,CONCAT44(MVar1,MVar1));
    unaff_ESI = MeXMLWriteElement(op,parent,buffer);
    MeXMLWriteElement(op,unaff_ESI,"RADIUS");
    MeFPrimitiveGetRadius(prim);
    MeXMLWritePCDATA(op,&DAT_000125fd);
    MeXMLWriteElement(op,unaff_ESI,"HEIGHT");
    MeFPrimitiveGetHeight(prim);
LAB_0001093b:
    MeXMLWritePCDATA(op,&DAT_000125fd);
    break;
  case kMeFPrimitiveTypePlane:
    sprintf(__s," type=\"plane\"",MVar1);
    unaff_ESI = MeXMLWriteElement(op,parent,buffer);
    break;
  case kMeFPrimitiveTypeConvex:
    sprintf(__s," type=\"convex\"",MVar1);
    unaff_ESI = MeXMLWriteElement(op,parent,buffer);
                    /* Unresolved local var: MeVector3Ptr v@[DW_OP_reg3(EBX)] */
    MeFPrimitiveGetVertexCount(prim);
    MeFPrimitiveInitVertexIterator(prim,&it);
    while (pvVar2 = MeFPrimitiveGetVertex(&it), pvVar2 != (void *)0x0) {
      MeXMLWriteElement(op,unaff_ESI,"VERTEX");
      MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g");
    }
  }
  MeXMLWriteElement(op,unaff_ESI,&DAT_00012602);
  MeFPrimitiveGetTransformPtr(prim);
  MeXMLWritePCDATA(op,
                   "%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g"
                  );
  return unaff_ESI;
}


/* ==== MeFModelWriteXML_1_0 ==== */

MeXMLElementID MeFModelWriteXML_1_0(MeXMLOutput *op,MeFModel *fm,MeXMLElementID parent)

{
  int iVar1;
  void *pvVar2;
  int iVar3;
  undefined4 extraout_EDX;
  char *__format;
  MeVector3 fast;
  MeMatrix3 I;
  MeVector3 offset;
  char buffer [1024];
  
                    /* Unresolved local var: MeXMLElementID model@[DW_OP_reg6(ESI)]
                       Unresolved local var: char * pBuf@[DW_OP_reg3(EBX)]
                       Unresolved local var: char * p@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeFModelType type@[DW_OP_reg6(ESI)] */
  iVar1 = MeFModelGetType(fm);
  pvVar2 = MeFModelGetName(fm);
  iVar3 = sprintf(buffer,"MODEL id=\"%s\"",pvVar2);
  if (iVar1 == 1) {
    __format = " type=\"dynamics_only\"";
  }
  else if (iVar1 == 2) {
    __format = " type=\"geometry_only\"";
  }
  else {
    __format = " type=\"dynamics_and_geometry\"";
  }
  iVar1 = sprintf(buffer + iVar3,__format,iVar3,iVar3);
  pvVar2 = MeFModelGetGeometryName(fm);
  if (pvVar2 != (void *)0x0) {
    sprintf(buffer + iVar3 + iVar1," geometry=\"%s\"",pvVar2,extraout_EDX);
  }
  iVar1 = MeXMLWriteElement(op,parent,buffer);
  if (fm->type < kMeFModelTypeGeometryOnly) {
                    /* Unresolved local var: MeXMLElementID dynamics@[DW_OP_reg3(EBX)] */
    iVar3 = MeXMLWriteElement(op,iVar1,"DYNAMICS");
    MeXMLWriteElement(op,iVar3,&DAT_000126b3);
    MeFModelGetMass(fm);
    MeXMLWritePCDATA(op,&DAT_000125fd);
    MeXMLWriteElement(op,iVar3,"DENSITY");
    MeFModelGetDensity(fm);
    MeXMLWritePCDATA(op,&DAT_000125fd);
    MeXMLWriteElement(op,iVar3,"MASS_OFFSET");
    MeFModelGetMassOffset(fm,offset);
    MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g");
    MeFModelGetInertiaTensor(fm,I);
    MeXMLWriteElement(op,iVar3,"INERTIA");
    MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g,%.7g,%.7g,%.7g");
    MeXMLWriteElement(op,iVar3,"LIN_DAMP");
    MeFModelGetLinearVelocityDamping(fm);
    MeXMLWritePCDATA(op,&DAT_000125fd);
    MeXMLWriteElement(op,iVar3,"ANG_DAMP");
    MeFModelGetAngularVelocityDamping(fm);
    MeXMLWritePCDATA(op,&DAT_000125fd);
    MeFModelGetFastSpinAxis(fm,fast);
    MeXMLWriteElement(op,iVar3,"FAST_SPIN");
    MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g");
    MeXMLWriteElement(op,iVar3,"USE_FAST_SPIN");
    MeFModelIsFastSpinAxisEnabled(fm);
    MeXMLWritePCDATA(op,&DAT_0001271c);
  }
  return iVar1;
}


/* ==== MeFAssetPartWriteXML_1_0 ==== */

MeXMLElementID MeFAssetPartWriteXML_1_0(MeXMLOutput *op,MeFAssetPart *part,MeXMLElementID parent)

{
  void *pvVar1;
  void *pvVar2;
  int iVar3;
  char *__s;
  float fVar4;
  double dVar5;
  double dVar6;
  double dVar7;
  MeVector3 v;
  char buffer [1024];
  
                    /* Unresolved local var: char * pBuf@[DW_OP_reg6(ESI)]
                       Unresolved local var: char * p@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeXMLElementID elem@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg0(EAX)] */
  pvVar1 = MeFAssetPartGetModelName(part);
  pvVar2 = MeFAssetPartGetName(part);
  iVar3 = sprintf(buffer,"PART id=\"%s\" model=\"%s\"",pvVar2,pvVar1);
  __s = buffer + iVar3;
  pvVar1 = MeFAssetPartGetGraphicHint(part);
  if (pvVar1 != (void *)0x0) {
    pvVar2 = pvVar1;
    MeFAssetPartGetGraphicOffset(part,v);
    dVar7 = (double)v[2];
    dVar6 = (double)v[1];
    dVar5 = (double)v[0];
    fVar4 = MeFAssetPartGetGraphicScale(part);
    iVar3 = sprintf(__s," graphic=\"%s\" scale=\"%.7g\" graphic_offset=\"%.7g,%.7g,%.7g\"",pvVar1,
                    SUB84((double)fVar4,0),(int)((ulonglong)(double)fVar4 >> 0x20),SUB84(dVar5,0),
                    (int)((ulonglong)dVar5 >> 0x20),SUB84(dVar6,0),(int)((ulonglong)dVar6 >> 0x20),
                    SUB84(dVar7,0),(int)((ulonglong)dVar7 >> 0x20),pvVar2);
    __s = __s + iVar3;
  }
  pvVar1 = MeFAssetPartGetParentPartName(part);
  if (pvVar1 != (void *)0x0) {
    sprintf(__s," parent=\"%s\"",pvVar1,pvVar1);
  }
  iVar3 = MeXMLWriteElement(op,parent,buffer);
  MeXMLWriteElement(op,iVar3,&DAT_00012602);
  MeFAssetPartGetTransformPtr(part);
  MeXMLWritePCDATA(op,
                   "%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g"
                  );
  return iVar3;
}


/* ==== MeFJointWriteXML_1_0 ==== */

MeXMLElementID MeFJointWriteXML_1_0(MeXMLOutput *op,MeFJoint *fj,MeXMLElementID parent)

{
  int iVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  undefined4 extraout_ECX;
  undefined4 uVar5;
  undefined4 extraout_ECX_00;
  char *__s;
  int unaff_EDI;
  char *__format;
  undefined8 uVar6;
  MeBool b;
  MeReal p;
  MeReal p_1;
  MeVector3 v;
  char buffer [1024];
  
                    /* Unresolved local var: char * pBuf@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeXMLElementID elem@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeFJointType type@[DW_OP_reg6(ESI)] */
  iVar1 = MeFJointGetType(fj);
  pvVar2 = MeFJointGetName(fj);
  iVar3 = sprintf(buffer,"JOINT id=\"%s\"",pvVar2);
  pvVar2 = MeFJointGetPartName(fj,0);
  iVar4 = sprintf(buffer + iVar3," part1=\"%s\"",pvVar2);
  __s = buffer + iVar3 + iVar4;
  pvVar2 = MeFJointGetPart(fj,1);
  iVar3 = 0;
  uVar5 = extraout_ECX;
  if (pvVar2 != (void *)0x0) {
    pvVar2 = MeFJointGetPartName(fj,1);
    iVar3 = sprintf(__s," part2=\"%s\"",pvVar2);
    __s = __s + iVar3;
    uVar5 = extraout_ECX_00;
  }
  if (iVar1 == 3) {
    uVar6 = CONCAT44(iVar3,iVar3);
    __format = " type=\"ballandsocket\"";
LAB_0001148f:
    sprintf(__s,__format,uVar6);
    unaff_EDI = MeXMLWriteElement(op,parent,buffer);
  }
  else {
    if (iVar1 == 2) {
      sprintf(__s," type=\"hinge\"",iVar3);
      unaff_EDI = MeXMLWriteElement(op,parent,buffer);
      HingeWriteXML_1_0(op,fj,unaff_EDI);
      goto LAB_0001108e;
    }
    if (iVar1 == 1) {
      sprintf(__s," type=\"carwheel\"",1);
      unaff_EDI = MeXMLWriteElement(op,parent,buffer);
      CarWheelWriteXML_1_0(op,fj,unaff_EDI);
      goto LAB_0001108e;
    }
    if (iVar1 == 4) {
      sprintf(__s," type=\"conelimit\"",iVar3);
      unaff_EDI = MeXMLWriteElement(op,parent,buffer);
      p_1 = 0.0;
      iVar3 = MeFJointGetProperty1f(fj,0,&p_1);
      if (iVar3 != 0) {
        MeXMLWriteElement(op,unaff_EDI,"HALF_ANGLE");
        MeXMLWritePCDATA(op,&DAT_000125fd);
      }
      iVar3 = MeFJointGetProperty1f(fj,3,&p_1);
      if (iVar3 == 0) goto LAB_0001108e;
      MeXMLWriteElement(op,unaff_EDI,"STIFFNESS");
    }
    else {
      if (iVar1 == 5) {
        uVar6 = CONCAT44(uVar5,uVar5);
        __format = " type=\"universal\"";
        goto LAB_0001148f;
      }
      if (iVar1 == 6) {
        sprintf(__s," type=\"rpro\"",iVar3);
        unaff_EDI = MeXMLWriteElement(op,parent,buffer);
        RproWriteXML_1_0(op,fj,unaff_EDI);
        goto LAB_0001108e;
      }
      if (iVar1 == 7) {
        sprintf(__s," type=\"prismatic\"",7);
        unaff_EDI = MeXMLWriteElement(op,parent,buffer);
        PrismaticWriteXML_1_0(op,fj,unaff_EDI);
        goto LAB_0001108e;
      }
      if (iVar1 == 8) {
        sprintf(__s," type=\"skeletal\"",iVar3);
        unaff_EDI = MeXMLWriteElement(op,parent,buffer);
        SkeletalWriteXML_1_0(op,fj,unaff_EDI);
        goto LAB_0001108e;
      }
      if (iVar1 != 9) {
        if (iVar1 == 10) {
          sprintf(__s," type=\"spring6\"");
          unaff_EDI = MeXMLWriteElement(op,parent,buffer);
          Spring6WriteXML_1_0(op,fj,unaff_EDI);
        }
        goto LAB_0001108e;
      }
      sprintf(__s," type=\"angular3\"");
      unaff_EDI = MeXMLWriteElement(op,parent,buffer);
                    /* Unresolved local var: int i@[???] */
      p = 0.0;
      b = 0;
      iVar3 = MeFJointGetProperty1b(fj,0x1c,&b);
      if (iVar3 != 0) {
        MeXMLWriteElement(op,unaff_EDI,"ROTATION_ENABLED");
        MeXMLWritePCDATA(op,&DAT_0001271c);
      }
      iVar3 = MeFJointGetProperty1f(fj,3,&p);
      if (iVar3 != 0) {
        MeXMLWriteElement(op,unaff_EDI,"STIFFNESS");
        MeXMLWritePCDATA(op,&DAT_000125fd);
      }
      iVar3 = MeFJointGetProperty1f(fj,9,&p);
      if (iVar3 == 0) goto LAB_0001108e;
      MeXMLWriteElement(op,unaff_EDI,"DAMPING");
    }
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
LAB_0001108e:
  MeFJointGetPosition(fj,0,v);
  MeXMLWriteElement(op,unaff_EDI,&DAT_0001278a);
  MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g");
  MeFJointGetPosition(fj,1,v);
  MeXMLWriteElement(op,unaff_EDI,&DAT_0001278f);
  MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g");
  MeFJointGetPrimaryAxis(fj,0,v);
  MeXMLWriteElement(op,unaff_EDI,"PRIMARY_AXIS1");
  MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g");
  MeFJointGetPrimaryAxis(fj,1,v);
  MeXMLWriteElement(op,unaff_EDI,"PRIMARY_AXIS2");
  MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g");
  MeFJointGetOrthogonalAxis(fj,0,v);
  MeXMLWriteElement(op,unaff_EDI,"ORTHOGONAL_AXIS1");
  MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g");
  MeFJointGetOrthogonalAxis(fj,1,v);
  MeXMLWriteElement(op,unaff_EDI,"ORTHOGONAL_AXIS2");
  MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g");
  return unaff_EDI;
}


/* ==== RproWriteXML_1_0 ==== */

void RproWriteXML_1_0(MeXMLOutput *op,MeFJoint *j,MeXMLElementID parent)

{
  int iVar1;
  MeReal p;
  
  p = 0.0;
  iVar1 = MeFJointGetProperty1f(j,0x11,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,&DAT_000128c5);
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0x12,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,&DAT_000128c2);
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0x13,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,&DAT_000128bf);
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0x14,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,&DAT_000128bc);
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0x15,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,&DAT_000128b9);
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0x16,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,&DAT_000128b6);
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  return;
}


/* ==== HingeWriteXML_1_0 ==== */

void HingeWriteXML_1_0(MeXMLOutput *op,MeFJoint *j,MeXMLElementID parent)

{
  int iVar1;
  MeBool b;
  MeReal p;
  
  p = 0.0;
  b = 0;
  iVar1 = MeFJointGetProperty1f(j,0,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"HIGH_LIMIT");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,1,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LOW_LIMIT");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,3,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"HIGH_STIFFNESS");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,4,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LOW_STIFFNESS");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1b(j,0xf,&b);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LIMITED");
    MeXMLWritePCDATA(op,&DAT_0001271c);
  }
  iVar1 = MeFJointGetProperty1b(j,0x10,&b);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"MOTORIZED");
    MeXMLWritePCDATA(op,&DAT_0001271c);
  }
  iVar1 = MeFJointGetProperty1f(j,0x17,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"DES_VEL");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0x11,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"MAX_FORCE");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  return;
}


/* ==== CarWheelWriteXML_1_0 ==== */

void CarWheelWriteXML_1_0(MeXMLOutput *op,MeFJoint *j,MeXMLElementID parent)

{
  int iVar1;
  MeBool b;
  MeReal p;
  
  p = 0.0;
  b = 0;
  iVar1 = MeFJointGetProperty1f(j,0x11,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"HINGE_MAX_FORCE");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0x17,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"HINGE_DES_VEL");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0x12,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"STEERING_MAX_FORCE");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0x18,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"STEERING_DES_VEL");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1b(j,0x1c,&b);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"STEERING_LOCK");
    MeXMLWritePCDATA(op,&DAT_0001271c);
  }
  iVar1 = MeFJointGetProperty1f(j,0,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"SUSP_HIGH_LIMIT");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,1,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"SUSP_LOW_LIMIT");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,3,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"SUSP_STIFFNESS");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,4,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"SUSP_SOFTNESS");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,9,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"SUSP_DAMPING");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0x19,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"SUSP_REF");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  return;
}


/* ==== PrismaticWriteXML_1_0 ==== */

void PrismaticWriteXML_1_0(MeXMLOutput *op,MeFJoint *j,MeXMLElementID parent)

{
  int iVar1;
  MeBool b;
  MeReal p;
  
  p = 0.0;
  b = 0;
  iVar1 = MeFJointGetProperty1f(j,0,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"HIGH_LIMIT");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,1,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LOW_LIMIT");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,3,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"HIGH_STIFFNESS");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,4,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LOW_STIFFNESS");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1b(j,0xf,&b);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LIMITED");
    MeXMLWritePCDATA(op,&DAT_0001271c);
  }
  iVar1 = MeFJointGetProperty1b(j,0x10,&b);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"MOTORIZED");
    MeXMLWritePCDATA(op,&DAT_0001271c);
  }
  iVar1 = MeFJointGetProperty1f(j,0x17,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"DES_VEL");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0x11,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"MAX_FORCE");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  return;
}


/* ==== SkeletalWriteXML_1_0 ==== */

void SkeletalWriteXML_1_0(MeXMLOutput *op,MeFJoint *j,MeXMLElementID parent)

{
  int iVar1;
  int i;
  MeReal p;
  
                    /* Unresolved local var: MeBool b@[???] */
  p = 0.0;
  i = 0;
  iVar1 = MeFJointGetProperty1i(j,0x1a,&i);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"CONE_TYPE");
    MeXMLWritePCDATA(op,&DAT_0001271c);
  }
  iVar1 = MeFJointGetProperty1f(j,0,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"CONE_HALF_ANGLE_X");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,1,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"CONE_HALF_ANGLE_Y");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,3,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"CONE_STIFFNESS");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,9,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"CONE_DAMPING");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1i(j,0x1b,&i);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"TWIST_TYPE");
    MeXMLWritePCDATA(op,&DAT_0001271c);
  }
  iVar1 = MeFJointGetProperty1f(j,2,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"TWIST_HALF_ANGLE");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,4,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"TWIST_STIFFNESS");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,10,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"TWIST_DAMPING");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  return;
}


/* ==== Spring6WriteXML_1_0 ==== */

void Spring6WriteXML_1_0(MeXMLOutput *op,MeFJoint *j,MeXMLElementID parent)

{
  int iVar1;
  MeReal p;
  
  p = 0.0;
  iVar1 = MeFJointGetProperty1f(j,3,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LINEAR_STIFF_X");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,4,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LINEAR_STIFF_Y");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,5,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LINEAR_STIFF_Z");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,9,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LINEAR_DAMP_X");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,10,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LINEAR_DAMP_Y");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0xb,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"LINEAR_DAMP_Z");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,6,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"ANGULAR_STIFF_X");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,7,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"ANGULAR_STIFF_Y");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,8,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"ANGULAR_STIFF_Z");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0xc,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"ANGULAR_DAMP_X");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0xd,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"ANGULAR_DAMP_Y");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  iVar1 = MeFJointGetProperty1f(j,0xe,&p);
  if (iVar1 != 0) {
    MeXMLWriteElement(op,parent,"ANGULAR_DAMP_Z");
    MeXMLWritePCDATA(op,&DAT_000125fd);
  }
  return;
}


