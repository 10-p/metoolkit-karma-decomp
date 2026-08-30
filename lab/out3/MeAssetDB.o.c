/* ==== MeAssetDBLookupAssetByName ==== */

MeFAsset * MeAssetDBLookupAssetByName(MeAssetDB *db,char *name)

{
  MeFAssetNode *pMVar1;
  MeFAsset *pMVar2;
  int iVar3;
  
                    /* Unresolved local var: MeFAssetIt it@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeFAsset * asset@[DW_OP_reg3(EBX)] */
  pMVar1 = db->nilAsset->prev;
  pMVar2 = pMVar1->current;
  pMVar1 = pMVar1->prev;
  while( true ) {
    if (pMVar2 == (MeFAsset *)0x0) {
      return (MeFAsset *)0x0;
    }
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    iVar3 = strcmp(pMVar2->name,name);
    if (iVar3 == 0) break;
                    /* Unresolved local var: MeFAsset * asset@[DW_OP_reg0(EAX)] */
    pMVar2 = pMVar1->current;
    pMVar1 = pMVar1->prev;
  }
  return pMVar2;
}


/* ==== MeAssetDBInsertAsset ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void MeAssetDBInsertAsset(MeAssetDB *db,MeFAsset *asset)

{
  MeAssetDB *pMVar1;
  MeFAssetNode *pMVar2;
  MeFAssetNode *pMVar3;
  int iVar4;
  MeFAssetNode *pMVar5;
  
                    /* Unresolved local var: MeFAssetNode * node@[DW_OP_reg7(EDI)] */
  pMVar1 = asset->db;
  if (pMVar1 != (MeAssetDB *)0x0) {
    if (pMVar1 == db) {
      return;
    }
    if (pMVar1 != (MeAssetDB *)0x0) {
      MeAssetDBRemoveAsset(asset);
    }
  }
  asset->db = db;
  pMVar3 = (MeFAssetNode *)(*_MeMemoryAPI)(0xc);
  pMVar3->current = asset;
                    /* Unresolved local var: MeFAssetNode * temp@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeFAsset * tempAsset@[DW_OP_reg0(EAX)] */
  pMVar2 = db->nilAsset;
  do {
    pMVar5 = pMVar2;
    pMVar2 = pMVar5->next;
    if (pMVar2->current == (MeFAsset *)0x0) break;
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    iVar4 = strcmp(pMVar2->current->name,asset->name);
  } while (0 < iVar4);
  pMVar3->next = pMVar2;
  pMVar2 = pMVar5->next;
  pMVar5->next = pMVar3;
  pMVar2->prev = pMVar3;
  pMVar3->prev = pMVar5;
  db->assetCount = db->assetCount + 1;
  return;
}


/* ==== MeAssetDBRemoveAsset ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeAssetDBRemoveAsset(MeFAsset *asset)

{
  MeFAssetNode *pMVar1;
  MeFAsset *pMVar2;
  MeFAssetNode *pMVar3;
  
                    /* Unresolved local var: MeFAssetNode * node@[DW_OP_reg1(ECX)] */
  if (asset->db != (MeAssetDB *)0x0) {
    pMVar1 = asset->db->nilAsset;
    pMVar3 = pMVar1->prev;
    if (pMVar3 != pMVar1) {
      pMVar2 = pMVar3->current;
      while ((pMVar2 != asset && (pMVar3 = pMVar3->prev, pMVar3 != pMVar1))) {
        pMVar2 = pMVar3->current;
      }
    }
    pMVar1 = pMVar3->prev;
    pMVar1->next = pMVar3->next;
    pMVar3->next->prev = pMVar1;
    (*_strlen)(pMVar3);
    asset->db->assetCount = asset->db->assetCount - 1;
    asset->db = (MeAssetDB *)0x0;
  }
  return;
}


/* ==== MeAssetDBCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

MeAssetDB * MeAssetDBCreate(void)

{
  MeAssetDB *pMVar1;
  MeFAssetNode *pMVar2;
  
                    /* Unresolved local var: MeAssetDB * db@[DW_OP_reg3(EBX)] */
  pMVar1 = (MeAssetDB *)(*_MeFAssetDestroy)(8);
  pMVar2 = (MeFAssetNode *)(*_MeMemoryAPI)(0xc);
  pMVar1->nilAsset = pMVar2;
  pMVar2->prev = pMVar2;
  pMVar2->next = pMVar2;
  pMVar1->nilAsset->current = (MeFAsset *)0x0;
  pMVar1->assetCount = 0;
  return pMVar1;
}


/* ==== MeAssetDBDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeAssetDBDestroy(MeAssetDB *db)

{
  MeFAsset *asset;
  
                    /* Unresolved local var: MeFAsset * asset@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeFAssetIt assetIt@[DW_OP_reg0(EAX)] */
  asset = db->nilAsset->prev->current;
  while (asset != (MeFAsset *)0x0) {
    MeAssetDBRemoveAsset(asset);
    MeFAssetDestroy(asset);
    asset = db->nilAsset->prev->current;
  }
  (*_strlen)(db->nilAsset);
  (*_strlen)(db);
  return;
}


/* ==== MeAssetDBDeleteContents ==== */

void MeAssetDBDeleteContents(MeAssetDB *db)

{
  MeFAsset *asset;
  
                    /* Unresolved local var: MeFAsset * asset@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeFAssetIt assetIt@[DW_OP_reg0(EAX)] */
  asset = db->nilAsset->prev->current;
  while (asset != (MeFAsset *)0x0) {
    MeAssetDBRemoveAsset(asset);
    MeFAssetDestroy(asset);
    asset = db->nilAsset->prev->current;
  }
  return;
}


/* ==== MeAssetDBCreateCopy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeAssetDB * MeAssetDBCreateCopy(MeAssetDB *db)

{
  MeFAsset **ppMVar1;
  MeFAssetNode *pMVar2;
  MeAssetDB *db_00;
  MeFAssetNode *pMVar3;
  MeFAsset *pMVar4;
  
                    /* Unresolved local var: MeFAssetIt assetIt@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeFAsset * asset@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeAssetDB * copyDB@[???] */
                    /* Unresolved local var: MeAssetDB * db@[DW_OP_reg6(ESI)] */
  db_00 = (MeAssetDB *)(*_MeFAssetDestroy)(8);
  pMVar3 = (MeFAssetNode *)(*_MeMemoryAPI)(0xc);
  db_00->nilAsset = pMVar3;
  pMVar3->prev = pMVar3;
  pMVar3->next = pMVar3;
  db_00->nilAsset->current = (MeFAsset *)0x0;
  pMVar3 = db->nilAsset;
  db_00->assetCount = 0;
  pMVar3 = pMVar3->prev;
  pMVar2 = pMVar3->prev;
  pMVar4 = pMVar3->current;
  while (pMVar4 != (MeFAsset *)0x0) {
                    /* Unresolved local var: MeFAsset * copy@[DW_OP_reg0(EAX)] */
    pMVar4 = MeFAssetCreateCopy(pMVar4,1);
    MeAssetDBInsertAsset(db_00,pMVar4);
    ppMVar1 = &pMVar2->current;
    pMVar2 = pMVar2->prev;
                    /* Unresolved local var: MeFAsset * asset@[DW_OP_reg0(EAX)] */
    pMVar4 = *ppMVar1;
  }
  return db_00;
}


/* ==== MeAssetDBInsertCopy ==== */

void MeAssetDBInsertCopy(MeAssetDB *to,MeAssetDB *from)

{
  MeFAsset **ppMVar1;
  MeFAssetNode *pMVar2;
  MeFAssetNode *pMVar3;
  MeFAsset *pMVar4;
  
                    /* Unresolved local var: MeFAssetIt assetIt@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeFAsset * asset@[DW_OP_reg0(EAX)] */
  pMVar2 = from->nilAsset->prev;
  pMVar3 = pMVar2->prev;
  pMVar4 = pMVar2->current;
  while (pMVar4 != (MeFAsset *)0x0) {
                    /* Unresolved local var: MeFAsset * copy@[DW_OP_reg0(EAX)] */
    pMVar4 = MeFAssetCreateCopy(pMVar4,1);
    MeAssetDBInsertAsset(to,pMVar4);
    ppMVar1 = &pMVar3->current;
    pMVar3 = pMVar3->prev;
                    /* Unresolved local var: MeFAsset * asset@[DW_OP_reg0(EAX)] */
    pMVar4 = *ppMVar1;
  }
  return;
}


/* ==== MeAssetDBIsEmpty ==== */

MeBool MeAssetDBIsEmpty(MeAssetDB *db)

{
  return (uint)(db->assetCount == 0);
}


/* ==== MeAssetDBGetAssetCount ==== */

int MeAssetDBGetAssetCount(MeAssetDB *db)

{
  return db->assetCount;
}


/* ==== MeAssetDBInitAssetIterator ==== */

void MeAssetDBInitAssetIterator(MeAssetDB *db,MeFAssetIt *it)

{
  it->node = db->nilAsset->prev;
  return;
}


/* ==== MeAssetDBGetAsset ==== */

MeFAsset * MeAssetDBGetAsset(MeFAssetIt *it)

{
  MeFAsset *pMVar1;
  
                    /* Unresolved local var: MeFAsset * asset@[DW_OP_reg0(EAX)] */
  pMVar1 = it->node->current;
  it->node = it->node->prev;
  return pMVar1;
}


/* ==== MeAssetDBLookupAsset ==== */

MeFAsset * MeAssetDBLookupAsset(MeAssetDB *db,int id)

{
  MeFAssetNode *pMVar1;
  MeFAsset *pMVar2;
  
                    /* Unresolved local var: MeFAssetIt it@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeFAsset * asset@[DW_OP_reg0(EAX)] */
  pMVar1 = db->nilAsset->prev;
  pMVar2 = pMVar1->current;
  pMVar1 = pMVar1->prev;
  while( true ) {
    if (pMVar2 == (MeFAsset *)0x0) {
      return (MeFAsset *)0x0;
    }
    if (pMVar2->id == id) break;
                    /* Unresolved local var: MeFAsset * asset@[DW_OP_reg0(EAX)] */
    pMVar2 = pMVar1->current;
    pMVar1 = pMVar1->prev;
  }
  return pMVar2;
}


/* ==== _FSetStringProperty ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void _FSetStringProperty(char **data,char *newVal)

{
  size_t sVar1;
  char *__dest;
  
  if (*data != (char *)0x0) {
    (*_strlen)(*data);
    *data = (char *)0x0;
  }
  if (newVal != (char *)0x0) {
    sVar1 = strlen(newVal);
    __dest = (char *)(*_MeMemoryAPI)(sVar1 + 1);
    *data = __dest;
    strcpy(__dest,newVal);
    return;
  }
  return;
}


