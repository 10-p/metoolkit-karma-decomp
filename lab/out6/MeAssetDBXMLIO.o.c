/* ==== MeAssetDBXMLInputRead ==== */

MeBool MeAssetDBXMLInputRead(MeAssetDBXMLInput *i,MeStream stream)

{
  undefined4 *puVar1;
  void *pvVar2;
  MeBool MVar3;
  void *pvVar4;
  MeFAsset *pMVar5;
  int iVar6;
  undefined **ppuVar7;
  MeXMLHandler *pMVar8;
  MeXMLHandler handlers [2];
  
  ppuVar7 = &PTR_s_KARMA_000104d4;
  pMVar8 = handlers;
                    /* Unresolved local var: MeXMLInput * input@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeXMLError error@[DW_OP_reg0(EAX)]
                       Unresolved local var: int version@[???]
                       Unresolved local var: PElement * xmlRoot@[DW_OP_reg7(EDI)] */
  for (iVar6 = 0x10; iVar6 != 0; iVar6 = iVar6 + -1) {
    pMVar8->name = *ppuVar7;
    ppuVar7 = ppuVar7 + 1;
    pMVar8 = (MeXMLHandler *)&pMVar8->type;
  }
  pvVar2 = PElementCreate(0,&DAT_0001047e,(void *)0x0,(void *)0x0,(void *)0x0);
  MVar3 = 0;
  if ((i != (MeAssetDBXMLInput *)0x0) && (MVar3 = 0, stream != (MeStream)0x0)) {
    pvVar4 = MeXMLInputCreate(stream);
    MeXMLInputSetUserData(pvVar4,i);
    iVar6 = MeXMLInputProcess(pvVar4,handlers,pvVar2);
    if (iVar6 == 0) {
      MeXMLInputDestroy(pvVar4);
                    /* Unresolved local var: PElementNode * node@[DW_OP_reg6(ESI)] */
      for (puVar1 = *(undefined4 **)((int)pvVar2 + 4); puVar1 != (undefined4 *)0x0;
          puVar1 = (undefined4 *)puVar1[1]) {
                    /* Unresolved local var: PElement * e@[???]
                       Unresolved local var: MeFAsset * asset@[DW_OP_reg0(EAX)] */
        pMVar5 = (*MeFAssetCreateFunc[0])(i->db,i->IDPool,(PElement *)*puVar1);
        MeAssetDBInsertAsset(i->db,pMVar5);
      }
      PElementTraverseAll(pvVar2,PElementDestroyChildren,0,(void *)0x0);
      PElementDestroyChildren(pvVar2,(void *)0x0,(void *)0x0);
      PElementDestroy(pvVar2);
      MVar3 = 1;
    }
    else {
      MeXMLInputGetErrorString(pvVar4);
      MeWarning(3,"Parse Error in file %s.\n%s");
      MeXMLInputDestroy(pvVar4);
      MVar3 = 0;
    }
  }
  return MVar3;
}


/* ==== MeAssetDBXMLInputReadFirst ==== */

MeFAsset * MeAssetDBXMLInputReadFirst(MeAssetDBXMLInput *i,MeStream stream)

{
  void *pvVar1;
  void *pvVar2;
  MeFAsset *pMVar3;
  int iVar4;
  undefined **ppuVar5;
  MeXMLHandler *pMVar6;
  MeFAsset *asset;
  MeXMLHandler handlers [2];
  
  asset = (MeFAsset *)0x0;
  ppuVar5 = &PTR_s_KARMA_00010514;
  pMVar6 = handlers;
                    /* Unresolved local var: MeXMLInput * input@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeXMLError error@[DW_OP_reg0(EAX)]
                       Unresolved local var: int version@[???]
                       Unresolved local var: PElement * xmlRoot@[DW_OP_reg7(EDI)] */
  for (iVar4 = 0x10; iVar4 != 0; iVar4 = iVar4 + -1) {
    pMVar6->name = *ppuVar5;
    ppuVar5 = ppuVar5 + 1;
    pMVar6 = (MeXMLHandler *)&pMVar6->type;
  }
  pvVar1 = PElementCreate(0,&DAT_0001047e,(void *)0x0,(void *)0x0,(void *)0x0);
  pMVar3 = (MeFAsset *)0x0;
  if ((i != (MeAssetDBXMLInput *)0x0) && (pMVar3 = (MeFAsset *)0x0, stream != (MeStream)0x0)) {
    pvVar2 = MeXMLInputCreate(stream);
    MeXMLInputSetUserData(pvVar2,i);
    iVar4 = MeXMLInputProcess(pvVar2,handlers,pvVar1);
    if (iVar4 == 0) {
      MeXMLInputDestroy(pvVar2);
                    /* Unresolved local var: PElementNode * node@[DW_OP_reg0(EAX)] */
      if (*(undefined4 **)((int)pvVar1 + 4) != (undefined4 *)0x0) {
                    /* Unresolved local var: PElement * e@[???] */
        asset = (*MeFAssetCreateFunc[0])
                          (i->db,i->IDPool,(PElement *)**(undefined4 **)((int)pvVar1 + 4));
        MeAssetDBInsertAsset(i->db,asset);
      }
      PElementTraverseAll(pvVar1,PElementDestroyChildren,0,(void *)0x0);
      PElementDestroyChildren(pvVar1,(void *)0x0,(void *)0x0);
      PElementDestroy(pvVar1);
      pMVar3 = asset;
    }
    else {
      MeXMLInputGetErrorString(pvVar2);
      MeWarning(3,"Parse Error in file %s.\n%s");
      MeXMLInputDestroy(pvVar2);
      pMVar3 = (MeFAsset *)0x0;
    }
  }
  return pMVar3;
}


/* ==== MeAssetDBXMLOutputSetFileHeader ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeAssetDBXMLOutputSetFileHeader(MeAssetDBXMLOutput *output,char *header,...)

{
  char *__dest;
  uint *puVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  char buf [4096];
  
                    /* Unresolved local var: va_list args@[???] */
  vsnprintf(buf,0xfff,header,&stack0x0000000c);
  puVar2 = (uint *)buf;
  if (output->fileHeader != (char *)0x0) {
    (*_sprintf)(output->fileHeader);
  }
  do {
    puVar1 = puVar2;
    puVar2 = puVar1 + 1;
    uVar3 = *puVar1 + 0xfefefeff & (*puVar1 ^ 0xffffffff);
    uVar4 = uVar3 & 0x80808080;
  } while (uVar4 == 0);
  if ((uVar3 & 0x8080) == 0) {
    uVar4 = uVar4 >> 0x10;
    puVar2 = (uint *)((int)puVar1 + 6);
  }
  __dest = (char *)(*_MeMemoryAPI)((char *)((int)puVar2 +
                                           (-(int)buf - (uint)CARRY1((byte)uVar4,(byte)uVar4)) + -2)
                                  );
  output->fileHeader = __dest;
  strcpy(__dest,buf);
  return;
}


/* ==== GetLatestKaFileVersionString ==== */

char * GetLatestKaFileVersionString(void)

{
  return Ka_File_Version_String[0];
}


/* ==== MeAssetDBXMLInputCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeAssetDBXMLInput * MeAssetDBXMLInputCreate(MeAssetDB *db,MeIDPool *IDPool)

{
  MeAssetDBXMLInput *pMVar1;
  
                    /* Unresolved local var: MeAssetDBXMLInput * input@[DW_OP_reg0(EAX)] */
  pMVar1 = (MeAssetDBXMLInput *)(*_MeMemoryAPI)(8);
  pMVar1->db = db;
  pMVar1->IDPool = IDPool;
  return pMVar1;
}


/* ==== MeAssetDBXMLInputDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeAssetDBXMLInputDestroy(MeAssetDBXMLInput *input)

{
  (*_sprintf)(input);
  return;
}


/* ==== MeAssetDBXMLOutputCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeAssetDBXMLOutput * MeAssetDBXMLOutputCreate(MeAssetDB *db)

{
  MeAssetDBXMLOutput *pMVar1;
  
                    /* Unresolved local var: MeAssetDBXMLOutput * output@[DW_OP_reg0(EAX)] */
  pMVar1 = (MeAssetDBXMLOutput *)(*_MeMemoryAPI)(8);
  pMVar1->db = db;
  pMVar1->fileHeader = (char *)0x0;
  return pMVar1;
}


/* ==== MeAssetDBXMLOutputDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeAssetDBXMLOutputDestroy(MeAssetDBXMLOutput *output)

{
  if (output->fileHeader != (char *)0x0) {
    (*_sprintf)(output->fileHeader);
  }
  (*_sprintf)(output);
  return;
}


/* ==== MeAssetDBXMLOutputWrite ==== */

void MeAssetDBXMLOutputWrite(MeAssetDBXMLOutput *output,MeStream stream)

{
  MeXMLOutput *pMVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  MeFAsset *pMVar5;
  undefined4 extraout_EAX;
  uint *puVar6;
  uint *puVar7;
  uint uVar8;
  uint uVar9;
  MeFAssetIt asset_it;
  char buffer [1024];
  
                    /* Unresolved local var: MeFAsset * fa@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * pBuf@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeXMLOutput * op@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeXMLElementID karma@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeXMLElementID asset@[???] */
  pMVar1 = MeXMLOutputCreate(stream);
  sprintf(buffer,"<?xml version=\"1.0\"?>\n\n");
  puVar7 = (uint *)buffer;
  do {
    puVar6 = puVar7;
    puVar7 = puVar6 + 1;
    uVar8 = *puVar6 + 0xfefefeff & (*puVar6 ^ 0xffffffff);
    uVar9 = uVar8 & 0x80808080;
  } while (uVar9 == 0);
  if ((uVar8 & 0x8080) == 0) {
    uVar9 = uVar9 >> 0x10;
    puVar7 = (uint *)((int)puVar6 + 6);
  }
  MeStreamWrite(buffer,(uint)((int)puVar7 +
                             ((-3 - (uint)CARRY1((byte)uVar9,(byte)uVar9)) - (int)buffer)),1,stream)
  ;
  uVar2 = 0;
  if (output->fileHeader != (char *)0x0) {
    MeXMLWriteComment(pMVar1,output->fileHeader);
    uVar2 = extraout_EAX;
  }
  sprintf(buffer,"KARMA ka_file_version=\"%s\"",Ka_File_Version_String[0],uVar2);
  iVar3 = MeXMLWriteElement(pMVar1,0,buffer);
  iVar4 = MeAssetDBGetAssetCount(output->db);
  if (0 < iVar4) {
    MeAssetDBInitAssetIterator(output->db,&asset_it);
    while( true ) {
      pMVar5 = MeAssetDBGetAsset(&asset_it);
      if (pMVar5 == (MeFAsset *)0x0) break;
      (*MeFAssetWriteXMLFunc)(pMVar1,pMVar5,iVar3);
    }
  }
  MeXMLOutputDestroy(pMVar1);
  return;
}


