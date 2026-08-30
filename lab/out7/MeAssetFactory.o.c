/* ==== MeAssetInstanceCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeAssetInstance *
MeAssetInstanceCreate
          (MeAssetFactory *af,MeFAsset *asset,MeMatrix4Ptr tm,MeBool owner,MdtWorldID world,
          McdSpaceID space)

{
  JointCreateFunc p_Var1;
  JointPostCreateCB p_Var2;
  ModelCreateFunc p_Var3;
  ModelPostCreateCB p_Var4;
  int iVar5;
  MdtConstraintID pMVar6;
  int iVar7;
  MeAssetInstance *pMVar8;
  MeHash *pMVar9;
  MeFGeometry *pMVar10;
  McdGeometryID pMVar11;
  char *__s;
  char *__dest;
  void *extraout_EAX;
  void *pvVar12;
  void *pvVar13;
  McdModelID_conflict pMVar14;
  McdModelID_conflict pMVar15;
  size_t sVar16;
  undefined4 uVar17;
  int iVar18;
  void *pvVar19;
  MeFGeometry *extraout_ECX;
  uint uVar20;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 extraout_ECX_04;
  undefined4 extraout_ECX_05;
  undefined4 uVar21;
  undefined4 extraout_ECX_06;
  MeAssetFactory *pMVar22;
  undefined4 auStackY_70 [5];
  MeFGeometry *pMStack_50;
  int aiStack_4c [3];
  McdModelID_conflict model2;
  McdModelID_conflict model1;
  MdtConstraintID constraint;
  McdModelID_conflict model1_1;
  McdGeometryID geom;
  McdModelID_conflict model;
  int count;
  int i;
  char *assetName;
  MeAssetInstance *ins;
  MeFJointIt it_1;
  MeFGeometryIt it;
  
                    /* Unresolved local var: char * key@[DW_OP_reg3(EBX)] */
  auStackY_70[4] = 0x10011;
  ins = (MeAssetInstance *)(*_MeMemoryAPI)();
  auStackY_70[4] = 0x1001e;
  assetName = MeFAssetGetName(asset);
  ins->af = af;
  ins->asset = asset;
  ins->owner = owner;
  ins->world = world;
  ins->space = space;
  auStackY_70[4] = 0x10050;
  pMVar9 = MeHashCreate(0x11);
  ins->nameToGeometry = pMVar9;
  auStackY_70[4] = 0x10062;
  pMVar9 = MeHashCreate(0x11);
  ins->nameToModel = pMVar9;
  auStackY_70[4] = 0x10074;
  pMVar9 = MeHashCreate(0x11);
  ins->nameToJoint = pMVar9;
  auStackY_70[4] = 0x1008c;
  MeHashSetKeyFreeFunc(ins->nameToGeometry,_MeHashSetKeyFreeFunc);
  auStackY_70[4] = 0x100a1;
  MeHashSetKeyFreeFunc(ins->nameToModel,_MeHashSetKeyFreeFunc);
  auStackY_70[4] = 0x100b6;
  MeHashSetKeyFreeFunc(ins->nameToJoint,_MeHashSetKeyFreeFunc);
                    /* Unresolved local var: MeFGeometry * geom@[DW_OP_reg3(EBX)] */
  auStackY_70[4] = 0x100c5;
  MeFAssetInitGeometryIterator(asset,&it);
  while( true ) {
    auStackY_70[4] = 0x100d0;
    pMVar10 = MeFAssetGetGeometry(&it);
    if (pMVar10 == (MeFGeometry *)0x0) break;
                    /* Unresolved local var: McdGeometryID mcdGeom@[DW_OP_reg7(EDI)] */
    auStackY_70[4] = 0x100ec;
    pMStack_50 = pMVar10;
    pMVar11 = McdGMCreateGeometry(ins->af->gm,pMVar10,assetName);
    auStackY_70[4] = 0x100f6;
    McdGeometryIncrementReferenceCount(pMVar11);
    if (af->geometryPostCreateCB != (GeometryPostCreateCB)0x0) {
      auStackY_70[4] = 0x10147;
      pMStack_50 = extraout_ECX;
      (*af->geometryPostCreateCB)(pMVar11,pMVar10,af->geometryPostCreateCBUserdata);
    }
                    /* Unresolved local var: char * key@[DW_OP_reg6(ESI)] */
    auStackY_70[4] = 0x1010c;
    __s = MeFGeometryGetName(pMVar10);
    auStackY_70[4] = 0x10116;
    strlen(__s);
    auStackY_70[4] = 0x10120;
    __dest = (char *)(*_MeMemoryAPI)();
    auStackY_70[4] = 0x1012b;
    strcpy(__dest,__s);
    auStackY_70[4] = 0x1013c;
    MeHashInsert(__dest,pMVar11,ins->nameToGeometry);
  }
                    /* Unresolved local var: MeFAssetPart * * part@[DW_OP_reg7(EDI)] */
  auStackY_70[4] = 0x10158;
  count = MeFAssetGetPartCount(asset);
  uVar20 = count * 4 + 0xfU & 0xfffffff0;
  iVar5 = -uVar20;
  *(uint *)((int)aiStack_4c + iVar5 + -4) = uVar20;
  *(uint *)(&stack0xffffffac + iVar5) = uVar20;
  *(int *)(&stack0xffffffa8 + iVar5) = (int)aiStack_4c + iVar5;
  *(MeFAsset **)(&stack0xffffffa4 + iVar5) = asset;
  *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10178;
  MeFAssetGetPartsSortedByName
            (*(void **)(&stack0xffffffa4 + iVar5),*(void **)(&stack0xffffffa8 + iVar5));
  i = 0;
  pvVar12 = extraout_EAX;
  if (0 < count) {
    do {
                    /* Unresolved local var: MeFModel * fmodel@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeFGeometry * fgeom@[DW_OP_reg6(ESI)]
                       Unresolved local var: char * key@[DW_OP_reg6(ESI)] */
      model = (McdModelID_conflict)0x0;
      *(undefined4 *)(&stack0xffffffa4 + iVar5) = *(undefined4 *)((int)aiStack_4c + i * 4 + iVar5);
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x1031c;
      pvVar12 = MeFAssetPartGetModel(*(void **)(&stack0xffffffa4 + iVar5));
      *(void **)(&stack0xffffffa4 + iVar5) = pvVar12;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10326;
      pvVar13 = MeFModelGetGeometry(*(void **)(&stack0xffffffa4 + iVar5));
      *(void **)(&stack0xffffffa4 + iVar5) = pvVar12;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10330;
      iVar18 = MeFModelGetType(*(void **)(&stack0xffffffa4 + iVar5));
      pMVar8 = ins;
      iVar18 = iVar18 + -1;
      if ((iVar18 == 0) || (pvVar13 == (void *)0x0)) {
        *(McdGeomMan **)(&stack0xffffffa4 + iVar5) = af->gm;
        *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x104fd;
        geom = McdGMGetNullGeometry(*(void **)(&stack0xffffffa4 + iVar5));
      }
      else {
        *(int *)((int)aiStack_4c + iVar5 + -4) = iVar18;
        *(int *)(&stack0xffffffac + iVar5) = iVar18;
        *(MeHash **)(&stack0xffffffa8 + iVar5) = pMVar8->nameToGeometry;
        *(void **)(&stack0xffffffa4 + iVar5) = pvVar13;
        *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10351;
        pvVar12 = MeFGeometryGetName(*(void **)(&stack0xffffffa4 + iVar5));
        *(void **)(&stack0xffffffa4 + iVar5) = pvVar12;
        *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10359;
        geom = MeHashLookup(*(void **)(&stack0xffffffa4 + iVar5),
                            *(void **)(&stack0xffffffa8 + iVar5));
      }
      *(undefined4 *)(&stack0xffffffa4 + iVar5) = *(undefined4 *)((int)aiStack_4c + i * 4 + iVar5);
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10369;
      pvVar12 = MeFAssetPartGetName(*(void **)(&stack0xffffffa4 + iVar5));
      *(void **)(&stack0xffffffa4 + iVar5) = pvVar12;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10373;
      sVar16 = strlen(*(char **)(&stack0xffffffa4 + iVar5));
      *(size_t *)(&stack0xffffffa4 + iVar5) = sVar16 + 1;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x1037d;
      uVar17 = (*_MeMemoryAPI)();
      *(void **)(&stack0xffffffa8 + iVar5) = pvVar12;
      *(undefined4 *)(&stack0xffffffa4 + iVar5) = uVar17;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x1038a;
      strcpy(*(char **)(&stack0xffffffa4 + iVar5),*(char **)(&stack0xffffffa8 + iVar5));
      p_Var3 = af->modelCreateFunc;
      pMVar15 = (McdModelID_conflict)0x0;
      pMVar22 = af;
      if (p_Var3 != (ModelCreateFunc)0x0) {
        *(MeMatrix4Ptr *)((int)aiStack_4c + iVar5 + -4) = tm;
        pMVar11 = geom;
        *(MdtWorldID *)(&stack0xffffffac + iVar5) = world;
        iVar18 = i;
        *(McdGeometryID *)(&stack0xffffffa8 + iVar5) = pMVar11;
        pMVar22 = *(MeAssetFactory **)((int)aiStack_4c + iVar18 * 4 + iVar5);
        *(MeAssetFactory **)(&stack0xffffffa4 + iVar5) = pMVar22;
        *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x104e4;
        pMVar15 = (*p_Var3)(*(MeFAssetPart **)(&stack0xffffffa4 + iVar5),
                            *(McdGeometryID *)(&stack0xffffffa8 + iVar5),
                            *(MdtWorldID *)(&stack0xffffffac + iVar5),
                            *(MeMatrix4Ptr *)((int)aiStack_4c + iVar5 + -4));
        model = pMVar15;
      }
      *(McdModelID_conflict *)((int)aiStack_4c + iVar5 + -4) = pMVar15;
      *(McdModelID_conflict *)(&stack0xffffffac + iVar5) = pMVar15;
      *(McdModelID_conflict *)(&stack0xffffffa8 + iVar5) = model;
      *(McdSpaceID *)(&stack0xffffffa4 + iVar5) = space;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x103aa;
      McdSpaceInsertModel(*(void **)(&stack0xffffffa4 + iVar5),*(void **)(&stack0xffffffa8 + iVar5))
      ;
      p_Var4 = af->modelPostCreateCB;
      uVar21 = extraout_ECX_05;
      if (p_Var4 != (ModelPostCreateCB)0x0) {
        *(MeAssetFactory **)((int)aiStack_4c + iVar5 + -4) = pMVar22;
        iVar18 = i;
        *(void **)(&stack0xffffffac + iVar5) = af->modelPostCreateCBUserdata;
        pMVar15 = model;
        *(undefined4 *)(&stack0xffffffa8 + iVar5) =
             *(undefined4 *)((int)aiStack_4c + iVar18 * 4 + iVar5);
        *(McdModelID_conflict *)(&stack0xffffffa4 + iVar5) = pMVar15;
        *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x104c7;
        (*p_Var4)(*(McdModelID_conflict *)(&stack0xffffffa4 + iVar5),
                  *(MeFAssetPart **)(&stack0xffffffa8 + iVar5),*(void **)(&stack0xffffffac + iVar5))
        ;
        uVar21 = extraout_ECX_06;
      }
      pMVar8 = ins;
      *(undefined4 *)((int)aiStack_4c + iVar5 + -4) = uVar21;
      pMVar15 = model;
      *(MeHash **)(&stack0xffffffac + iVar5) = pMVar8->nameToModel;
      *(McdModelID_conflict *)(&stack0xffffffa8 + iVar5) = pMVar15;
      *(undefined4 *)(&stack0xffffffa4 + iVar5) = uVar17;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x103cd;
      pvVar12 = MeHashInsert(*(void **)(&stack0xffffffa4 + iVar5),
                             *(void **)(&stack0xffffffa8 + iVar5),
                             *(void **)(&stack0xffffffac + iVar5));
      i = i + 1;
    } while (i < count);
    i = 0;
    if (0 < count) {
      do {
                    /* Unresolved local var: int j@[DW_OP_reg3(EBX)] */
        iVar18 = i + 1;
        aiStack_4c[2] = iVar18;
        if (iVar18 < count) {
          do {
            *(void **)((int)aiStack_4c + iVar5 + -4) = pvVar12;
            *(void **)(&stack0xffffffac + iVar5) = pvVar12;
            iVar7 = i;
            *(undefined4 *)(&stack0xffffffa8 + iVar5) =
                 *(undefined4 *)((int)aiStack_4c + iVar18 * 4 + iVar5);
            *(undefined4 *)(&stack0xffffffa4 + iVar5) =
                 *(undefined4 *)((int)aiStack_4c + iVar7 * 4 + iVar5);
            *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10425;
            pvVar12 = (void *)MeFAssetPartIsCollisionEnabled
                                        (*(void **)(&stack0xffffffa4 + iVar5),
                                         *(void **)(&stack0xffffffa8 + iVar5));
            pMVar8 = ins;
            if (pvVar12 == (void *)0x0) {
                    /* Unresolved local var: McdModelID.conflict model2@[DW_OP_reg6(ESI)] */
              *(undefined4 *)((int)aiStack_4c + iVar5 + -4) = 0;
              iVar7 = i;
              *(undefined4 *)(&stack0xffffffac + iVar5) = 0;
              *(MeHash **)(&stack0xffffffa8 + iVar5) = pMVar8->nameToModel;
              *(undefined4 *)(&stack0xffffffa4 + iVar5) =
                   *(undefined4 *)((int)aiStack_4c + iVar7 * 4 + iVar5);
              *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10449;
              pvVar12 = MeFAssetPartGetName(*(void **)(&stack0xffffffa4 + iVar5));
              *(void **)(&stack0xffffffa4 + iVar5) = pvVar12;
              *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10451;
              model1_1 = MeHashLookup(*(void **)(&stack0xffffffa4 + iVar5),
                                      *(void **)(&stack0xffffffa8 + iVar5));
              *(MeHash **)(&stack0xffffffa8 + iVar5) = ins->nameToModel;
              *(undefined4 *)(&stack0xffffffa4 + iVar5) =
                   *(undefined4 *)((int)aiStack_4c + iVar18 * 4 + iVar5);
              *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10466;
              pvVar12 = MeFAssetPartGetName(*(void **)(&stack0xffffffa4 + iVar5));
              *(void **)(&stack0xffffffa4 + iVar5) = pvVar12;
              *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x1046e;
              pvVar13 = MeHashLookup(*(void **)(&stack0xffffffa4 + iVar5),
                                     *(void **)(&stack0xffffffa8 + iVar5));
              pvVar12 = pvVar13;
              if ((model1_1 != (McdModelID_conflict)0x0) && (pvVar13 != (void *)0x0)) {
                *(McdModelID_conflict *)(&stack0xffffffa4 + iVar5) = model1_1;
                *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x1048a;
                pvVar19 = McdModelGetSpace(*(void **)(&stack0xffffffa4 + iVar5));
                pvVar12 = (void *)0x0;
                if (pvVar19 != (void *)0x0) {
                  *(void **)(&stack0xffffffa4 + iVar5) = pvVar13;
                  *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x1049a;
                  pvVar19 = McdModelGetSpace(*(void **)(&stack0xffffffa4 + iVar5));
                  pvVar12 = (void *)0x0;
                  if (pvVar19 != (void *)0x0) {
                    *(void **)((int)aiStack_4c + iVar5 + -4) = pvVar19;
                    *(void **)(&stack0xffffffac + iVar5) = pvVar19;
                    pMVar15 = model1_1;
                    *(void **)(&stack0xffffffa8 + iVar5) = pvVar13;
                    *(McdModelID_conflict *)(&stack0xffffffa4 + iVar5) = pMVar15;
                    *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x104ad;
                    pvVar12 = (void *)McdSpaceDisablePair(*(void **)(&stack0xffffffa4 + iVar5),
                                                          *(void **)(&stack0xffffffa8 + iVar5));
                  }
                }
              }
            }
            iVar18 = iVar18 + 1;
          } while (iVar18 < count);
        }
        i = aiStack_4c[2];
      } while (aiStack_4c[2] < count);
    }
  }
                    /* Unresolved local var: MeFJoint * joint@[DW_OP_reg7(EDI)] */
  *(void **)((int)aiStack_4c + iVar5 + -4) = pvVar12;
  *(void **)(&stack0xffffffac + iVar5) = pvVar12;
  *(MeFJointIt **)(&stack0xffffffa8 + iVar5) = &it_1;
  *(MeFAsset **)(&stack0xffffffa4 + iVar5) = asset;
  *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x1019d;
  MeFAssetInitJointIterator
            (*(void **)(&stack0xffffffa4 + iVar5),*(void **)(&stack0xffffffa8 + iVar5));
LAB_000101a0:
  *(MeFJointIt **)(&stack0xffffffa4 + iVar5) = &it_1;
  *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x101ac;
  pvVar12 = MeFAssetGetJoint(*(void **)(&stack0xffffffa4 + iVar5));
  if (pvVar12 == (void *)0x0) {
    return ins;
  }
  *(void **)((int)aiStack_4c + iVar5 + -4) = pvVar12;
  *(void **)(&stack0xffffffac + iVar5) = pvVar12;
  *(undefined4 *)(&stack0xffffffa8 + iVar5) = 0;
  *(void **)(&stack0xffffffa4 + iVar5) = pvVar12;
  model1 = (McdModelID_conflict)0x0;
  model2 = (McdModelID_conflict)0x0;
  *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x101d1;
  pvVar13 = MeFJointGetPart(*(void **)(&stack0xffffffa4 + iVar5),*(int *)(&stack0xffffffa8 + iVar5))
  ;
  *(undefined4 *)(&stack0xffffffa8 + iVar5) = 1;
  *(void **)(&stack0xffffffa4 + iVar5) = pvVar12;
  *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x101dd;
  pMVar14 = MeFJointGetPart(*(void **)(&stack0xffffffa4 + iVar5),*(int *)(&stack0xffffffa8 + iVar5))
  ;
  pMVar8 = ins;
  pMVar15 = pMVar14;
  if (pvVar13 != (void *)0x0) {
    *(undefined4 *)((int)aiStack_4c + iVar5 + -4) = extraout_ECX_00;
    *(undefined4 *)(&stack0xffffffac + iVar5) = extraout_ECX_00;
    *(MeHash **)(&stack0xffffffa8 + iVar5) = pMVar8->nameToModel;
    *(void **)(&stack0xffffffa4 + iVar5) = pvVar13;
    *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x101f5;
    pvVar13 = MeFAssetPartGetName(*(void **)(&stack0xffffffa4 + iVar5));
    *(void **)(&stack0xffffffa4 + iVar5) = pvVar13;
    *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x101fd;
    pMVar15 = MeHashLookup(*(void **)(&stack0xffffffa4 + iVar5),*(void **)(&stack0xffffffa8 + iVar5)
                          );
    model1 = pMVar15;
  }
  if (pMVar14 != (McdModelID_conflict)0x0) {
    *(McdModelID_conflict *)((int)aiStack_4c + iVar5 + -4) = pMVar15;
    *(McdModelID_conflict *)(&stack0xffffffac + iVar5) = pMVar15;
    *(MeHash **)(&stack0xffffffa8 + iVar5) = ins->nameToModel;
    *(McdModelID_conflict *)(&stack0xffffffa4 + iVar5) = pMVar14;
    *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10216;
    pvVar13 = MeFAssetPartGetName(*(void **)(&stack0xffffffa4 + iVar5));
    *(void **)(&stack0xffffffa4 + iVar5) = pvVar13;
    *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x1021e;
    model2 = MeHashLookup(*(void **)(&stack0xffffffa4 + iVar5),*(void **)(&stack0xffffffa8 + iVar5))
    ;
  }
  if (model1 == (McdModelID_conflict)0x0) goto LAB_0001023e;
  *(McdModelID_conflict *)(&stack0xffffffa4 + iVar5) = model1;
  *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10237;
  pvVar13 = McdModelGetBody(*(void **)(&stack0xffffffa4 + iVar5));
  uVar17 = extraout_ECX_01;
  if (pvVar13 == (void *)0x0) goto LAB_0001023e;
  goto LAB_00010260;
LAB_0001023e:
  if (model2 != (McdModelID_conflict)0x0) {
    *(McdModelID_conflict *)(&stack0xffffffa4 + iVar5) = model2;
    *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10255;
    pvVar13 = McdModelGetBody(*(void **)(&stack0xffffffa4 + iVar5));
    uVar17 = extraout_ECX_02;
    if (pvVar13 != (void *)0x0) {
LAB_00010260:
      pMVar14 = model1;
      pMVar15 = model2;
      p_Var1 = af->jointCreateFunc;
      if (p_Var1 != (JointCreateFunc)0x0) {
        *(MeMatrix4Ptr *)(&stack0xffffffa4 + iVar5) = tm;
        *(McdModelID_conflict *)((int)auStackY_70 + iVar5 + 0x10) = pMVar15;
        *(McdModelID_conflict *)((int)auStackY_70 + iVar5 + 0xc) = pMVar14;
        *(MdtWorldID *)((int)auStackY_70 + iVar5 + 8) = world;
        *(void **)((int)auStackY_70 + iVar5 + 4) = pvVar12;
        *(undefined4 *)((int)auStackY_70 + iVar5) = 0x102f0;
        constraint = (*p_Var1)(*(MeFJoint **)((int)auStackY_70 + iVar5 + 4),
                               *(MdtWorldID *)((int)auStackY_70 + iVar5 + 8),
                               *(McdModelID_conflict *)((int)auStackY_70 + iVar5 + 0xc),
                               *(McdModelID_conflict *)((int)auStackY_70 + iVar5 + 0x10),
                               *(MeMatrix4Ptr *)(&stack0xffffffa4 + iVar5));
        uVar17 = extraout_ECX_04;
      }
      if (constraint != (MdtConstraintID)0x0) {
        *(MdtConstraintID *)(&stack0xffffffa4 + iVar5) = constraint;
        *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x1027d;
        MdtConstraintEnable(*(void **)(&stack0xffffffa4 + iVar5));
        uVar17 = extraout_ECX_03;
      }
      pvVar13 = af->jointPostCreateCBUserdata;
      if (pvVar13 != (void *)0x0) {
        *(undefined4 *)((int)aiStack_4c + iVar5 + -4) = uVar17;
        *(void **)(&stack0xffffffac + iVar5) = pvVar13;
        pMVar6 = constraint;
        *(void **)(&stack0xffffffa8 + iVar5) = pvVar12;
        *(MdtConstraintID *)(&stack0xffffffa4 + iVar5) = pMVar6;
        p_Var2 = af->jointPostCreateCB;
        *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x102d5;
        (*p_Var2)(*(MdtConstraintID *)(&stack0xffffffa4 + iVar5),
                  *(MeFJoint **)(&stack0xffffffa8 + iVar5),*(void **)(&stack0xffffffac + iVar5));
      }
                    /* Unresolved local var: char * key@[DW_OP_reg6(ESI)] */
      *(void **)(&stack0xffffffa4 + iVar5) = pvVar12;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x10293;
      pvVar12 = MeFJointGetName(*(void **)(&stack0xffffffa4 + iVar5));
      *(void **)(&stack0xffffffa4 + iVar5) = pvVar12;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x1029d;
      sVar16 = strlen(*(char **)(&stack0xffffffa4 + iVar5));
      *(size_t *)(&stack0xffffffa4 + iVar5) = sVar16 + 1;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x102a7;
      uVar17 = (*_MeMemoryAPI)();
      *(void **)(&stack0xffffffa8 + iVar5) = pvVar12;
      *(undefined4 *)(&stack0xffffffa4 + iVar5) = uVar17;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x102b2;
      strcpy(*(char **)(&stack0xffffffa4 + iVar5),*(char **)(&stack0xffffffa8 + iVar5));
      pMVar6 = constraint;
      *(MeHash **)(&stack0xffffffac + iVar5) = ins->nameToJoint;
      *(MdtConstraintID *)(&stack0xffffffa8 + iVar5) = pMVar6;
      *(undefined4 *)(&stack0xffffffa4 + iVar5) = uVar17;
      *(undefined4 *)((int)auStackY_70 + iVar5 + 0x10) = 0x102c6;
      MeHashInsert(*(void **)(&stack0xffffffa4 + iVar5),*(void **)(&stack0xffffffa8 + iVar5),
                   *(void **)(&stack0xffffffac + iVar5));
    }
  }
  goto LAB_000101a0;
}


/* ==== MeAssetInstanceDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeAssetInstanceDestroy(MeAssetInstance *ins)

{
  void *pvVar1;
  void *pvVar2;
  MeMatrix4Ptr tm;
  MeHashIterator it;
  
  if (ins->owner != 0) {
                    /* Unresolved local var: MdtConstraintID joint@[DW_OP_reg3(EBX)] */
    MeHashInitIterator(&it,ins->nameToJoint);
    while (pvVar1 = MeHashGetDatum(&it), pvVar1 != (void *)0x0) {
      MdtConstraintDisable(pvVar1);
      MdtConstraintDestroy(pvVar1);
    }
    if (ins->owner != 0) {
                    /* Unresolved local var: McdModelID.conflict model@[DW_OP_reg6(ESI)] */
      MeHashInitIterator(&it,ins->nameToModel);
      while (pvVar1 = MeHashGetDatum(&it), pvVar1 != (void *)0x0) {
        tm = (MeMatrix4Ptr)0x0;
        pvVar2 = McdModelGetSpace(pvVar1);
        if (pvVar2 != (void *)0x0) {
          McdSpaceRemoveModel(pvVar1);
          MstBridgeUpdateTransitions(ins->bridge,ins->space,ins->world);
        }
        pvVar2 = McdModelGetBody(pvVar1);
        if (pvVar2 == (void *)0x0) {
          tm = McdModelGetTransformPtr(pvVar1);
        }
        else {
          MdtBodyDisable(pvVar2);
          MdtBodyDestroy(pvVar2);
        }
        McdModelDestroy(pvVar1);
        if (tm != (MeMatrix4Ptr)0x0) {
          (*_MeFAssetInitGeometryIterator)(tm);
        }
      }
      if (ins->owner != 0) {
                    /* Unresolved local var: McdGeometryID geom@[DW_OP_reg3(EBX)] */
        MeHashInitIterator(&it,ins->nameToGeometry);
        while (pvVar1 = MeHashGetDatum(&it), pvVar1 != (void *)0x0) {
          McdGeometryDecrementReferenceCount(pvVar1);
          McdGMDestroyGeometry(ins->af->gm,pvVar1);
        }
      }
    }
  }
  MeHashDestroy(ins->nameToGeometry);
  MeHashDestroy(ins->nameToModel);
  MeHashDestroy(ins->nameToJoint);
  (*_MeHashSetKeyFreeFunc)(ins);
  return;
}


/* ==== MeAssetFactoryCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

MeAssetFactory * MeAssetFactoryCreate(McdFrameworkID fwk)

{
  MeAssetFactory *pMVar1;
  McdGeomMan *pMVar2;
  
                    /* Unresolved local var: MeAssetFactory * af@[DW_OP_reg3(EBX)] */
  pMVar1 = (MeAssetFactory *)(*_MeMemoryAPI)(0x24);
  pMVar2 = McdGMCreate(fwk);
  pMVar1->geometryPostCreateCB = (GeometryPostCreateCB)0x0;
  pMVar1->gm = pMVar2;
  pMVar1->geometryPostCreateCBUserdata = (void *)0x0;
  pMVar1->modelCreateFunc = McdModelCreateFromMeFAssetPart;
  pMVar1->modelPostCreateCB = (ModelPostCreateCB)0x0;
  pMVar1->modelPostCreateCBUserdata = (void *)0x0;
  pMVar1->jointCreateFunc = MdtConstraintCreateFromMeFJoint;
  pMVar1->jointPostCreateCB = (JointPostCreateCB)0x0;
  pMVar1->jointPostCreateCBUserdata = (void *)0x0;
  return pMVar1;
}


/* ==== MeAssetFactoryDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeAssetFactoryDestroy(MeAssetFactory *af)

{
  McdGMDestroy(af->gm);
  (*_MeHashSetKeyFreeFunc)(af);
  return;
}


/* ==== MeAssetFactorySetGeometryPostCreateCB ==== */

void MeAssetFactorySetGeometryPostCreateCB
               (MeAssetFactory *af,GeometryPostCreateCB cb,void *userdata)

{
  af->geometryPostCreateCB = cb;
  af->geometryPostCreateCBUserdata = userdata;
  return;
}


/* ==== MeAssetFactorySetModelCreateFunction ==== */

void MeAssetFactorySetModelCreateFunction(MeAssetFactory *af,ModelCreateFunc func)

{
  af->modelCreateFunc = func;
  return;
}


/* ==== MeAssetFactorySetModelPostCreateCB ==== */

void MeAssetFactorySetModelPostCreateCB(MeAssetFactory *af,ModelPostCreateCB cb,void *userdata)

{
  af->modelPostCreateCB = cb;
  af->modelPostCreateCBUserdata = userdata;
  return;
}


/* ==== MeAssetFactorySetJointCreateFunction ==== */

void MeAssetFactorySetJointCreateFunction(MeAssetFactory *af,JointCreateFunc func)

{
  af->jointCreateFunc = func;
  return;
}


/* ==== MeAssetFactorySetJointPostCreateCB ==== */

void MeAssetFactorySetJointPostCreateCB(MeAssetFactory *af,JointPostCreateCB cb,void *userdata)

{
  af->jointPostCreateCB = cb;
  af->jointPostCreateCBUserdata = userdata;
  return;
}


/* ==== MeAssetInstanceGetGeometry ==== */

McdGeometryID MeAssetInstanceGetGeometry(MeAssetInstance *ins,char *name)

{
  McdGeometryID pMVar1;
  
  pMVar1 = MeHashLookup(name,ins->nameToGeometry);
  return pMVar1;
}


/* ==== MeAssetInstanceGetModel ==== */

McdModelID_conflict MeAssetInstanceGetModel(MeAssetInstance *ins,char *name)

{
  McdModelID_conflict pMVar1;
  
  pMVar1 = MeHashLookup(name,ins->nameToModel);
  return pMVar1;
}


/* ==== MeAssetInstanceGetJoint ==== */

MdtConstraintID MeAssetInstanceGetJoint(MeAssetInstance *ins,char *name)

{
  MdtConstraintID pMVar1;
  
  pMVar1 = MeHashLookup(name,ins->nameToJoint);
  return pMVar1;
}


/* ==== MeAssetInstanceInitGeometryIterator ==== */

void MeAssetInstanceInitGeometryIterator(MeAssetInstance *ins,MeAIGeomIt *it)

{
  MeHashInitIterator(it,ins->nameToGeometry);
  return;
}


/* ==== MeAssetInstanceGetNextGeometry ==== */

/* WARNING: Unknown calling convention */

McdGeometryID MeAssetInstanceGetNextGeometry(MeAIGeomIt *it)

{
  McdGeometryID pMVar1;
  
  pMVar1 = MeHashGetDatum(it);
  return pMVar1;
}


/* ==== MeAssetInstanceInitModelIterator ==== */

void MeAssetInstanceInitModelIterator(MeAssetInstance *ins,MeAIModelIt *it)

{
  MeHashInitIterator(it,ins->nameToModel);
  return;
}


/* ==== MeAssetInstanceGetNextModel ==== */

/* WARNING: Unknown calling convention */

McdModelID_conflict MeAssetInstanceGetNextModel(MeAIModelIt *it)

{
  McdModelID_conflict pMVar1;
  
  pMVar1 = MeHashGetDatum(it);
  return pMVar1;
}


/* ==== MeAssetInstanceInitJointIterator ==== */

void MeAssetInstanceInitJointIterator(MeAssetInstance *ins,MeAIJointIt *it)

{
  MeHashInitIterator(it,ins->nameToJoint);
  return;
}


/* ==== MeAssetInstanceGetNextJoint ==== */

/* WARNING: Unknown calling convention */

MdtConstraintID MeAssetInstanceGetNextJoint(MeAIJointIt *it)

{
  MdtConstraintID pMVar1;
  
  pMVar1 = MeHashGetDatum(it);
  return pMVar1;
}


/* ==== MeAssetInstanceEnableDynamics ==== */

/* WARNING: Unknown calling convention */

void MeAssetInstanceEnableDynamics(MeAssetInstance *ins)

{
  return;
}


/* ==== MeAssetInstanceDisableDynamics ==== */

/* WARNING: Unknown calling convention */

void MeAssetInstanceDisableDynamics(MeAssetInstance *ins)

{
  return;
}


/* ==== MeAssetInstanceGetUserData ==== */

void * MeAssetInstanceGetUserData(MeAssetInstance *ins)

{
  return ins->userdata;
}


/* ==== MeAssetInstanceSetUserData ==== */

void MeAssetInstanceSetUserData(MeAssetInstance *ins,void *userdata)

{
  ins->userdata = userdata;
  return;
}


