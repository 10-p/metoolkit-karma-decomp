/* ==== MeDictLookup ==== */

MeDictNode * MeDictLookup(MeDict *dict,void *key)

{
  MeDict *pMVar1;
  MeDict *pMVar2;
  int iVar3;
  MeDictNode *saved;
  
                    /* Unresolved local var: MeDictNode * root@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: int result@[DW_OP_reg0(EAX)] */
  pMVar2 = (MeDict *)(dict->nilnode).left;
  while( true ) {
    while( true ) {
      if (pMVar2 == dict) {
        return (MeDictNode *)0x0;
      }
      _iVar3 = (*dict->compare)(key,(pMVar2->nilnode).key);
      if (-1 < (int)_iVar3) break;
      pMVar2 = (MeDict *)(pMVar2->nilnode).left;
    }
    if ((int)_iVar3 < 1) break;
    pMVar2 = (MeDict *)(pMVar2->nilnode).right;
  }
  if (dict->dupes == 0) {
    return &pMVar2->nilnode;
  }
  do {
    pMVar1 = pMVar2;
    pMVar2 = (MeDict *)(pMVar1->nilnode).left;
    while( true ) {
      if (pMVar2 == dict) {
        return &pMVar1->nilnode;
      }
      _iVar3 = (*dict->compare)(key,(pMVar2->nilnode).key);
      if ((int)_iVar3 == 0) break;
      pMVar2 = (MeDict *)(pMVar2->nilnode).right;
    }
  } while( true );
}


/* ==== MeDictLowerBound ==== */

MeDictNode * MeDictLowerBound(MeDict *dict,void *key)

{
  MeDict *pMVar1;
  MeDict *pMVar2;
  int iVar3;
  
                    /* Unresolved local var: MeDictNode * root@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * tentative@[DW_OP_reg7(EDI)] */
  pMVar1 = (MeDict *)0x0;
  pMVar2 = (MeDict *)(dict->nilnode).left;
  while( true ) {
    while( true ) {
      if (pMVar2 == dict) {
        return &pMVar1->nilnode;
      }
                    /* Unresolved local var: int result@[DW_OP_reg0(EAX)] */
      iVar3 = (*dict->compare)(key,(pMVar2->nilnode).key);
      if (iVar3 < 1) break;
      pMVar2 = (MeDict *)(pMVar2->nilnode).right;
    }
    if ((-1 < iVar3) && (dict->dupes == 0)) break;
    pMVar1 = pMVar2;
    pMVar2 = (MeDict *)(pMVar2->nilnode).left;
  }
  return &pMVar2->nilnode;
}


/* ==== MeDictUpperBound ==== */

MeDictNode * MeDictUpperBound(MeDict *dict,void *key)

{
  MeDict *pMVar1;
  MeDict *pMVar2;
  int iVar3;
  
                    /* Unresolved local var: MeDictNode * root@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * tentative@[DW_OP_reg7(EDI)] */
  pMVar2 = (MeDict *)0x0;
  pMVar1 = (MeDict *)(dict->nilnode).left;
  while( true ) {
    while( true ) {
      if (pMVar1 == dict) {
        return &pMVar2->nilnode;
      }
                    /* Unresolved local var: int result@[DW_OP_reg0(EAX)] */
      iVar3 = (*dict->compare)(key,(pMVar1->nilnode).key);
      if (-1 < iVar3) break;
      pMVar1 = (MeDict *)(pMVar1->nilnode).left;
    }
    if ((iVar3 < 1) && (dict->dupes == 0)) break;
    pMVar2 = pMVar1;
    pMVar1 = (MeDict *)(pMVar1->nilnode).right;
  }
  return &pMVar1->nilnode;
}


/* ==== MeDictInsert ==== */

void MeDictInsert(MeDict *dict,MeDictNode *node,void *key)

{
  MeDict *pMVar1;
  MeDictColor MVar2;
  MeDictNode *pMVar3;
  int iVar4;
  MeDict *pMVar5;
  MeDict *pMVar6;
  MeDict *pMVar7;
  
                    /* Unresolved local var: MeDictNode * where@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * parent@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDictNode * uncle@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * grandpa@[DW_OP_reg2(EDX)]
                       Unresolved local var: int result@[DW_OP_reg0(EAX)] */
  iVar4 = -1;
  pMVar1 = (MeDict *)(dict->nilnode).left;
  node->key = key;
  pMVar7 = dict;
  while (pMVar1 != dict) {
    iVar4 = (*dict->compare)(key,(pMVar1->nilnode).key);
    pMVar7 = pMVar1;
    if (iVar4 < 0) {
      pMVar1 = (MeDict *)(pMVar1->nilnode).left;
    }
    else {
      pMVar1 = (MeDict *)(pMVar1->nilnode).right;
    }
  }
  if (iVar4 < 0) {
    (pMVar7->nilnode).left = node;
  }
  else {
    (pMVar7->nilnode).right = node;
  }
  node->parent = &pMVar7->nilnode;
  node->left = &dict->nilnode;
  node->right = &dict->nilnode;
  dict->nodecount = dict->nodecount + 1;
  node->color = kMeDictColorRed;
  MVar2 = (pMVar7->nilnode).color;
  do {
    if (MVar2 != kMeDictColorRed) {
LAB_000101e7:
      ((dict->nilnode).left)->color = kMeDictColorBlack;
      return;
    }
    pMVar1 = (MeDict *)(pMVar7->nilnode).parent;
    pMVar6 = (MeDict *)(pMVar1->nilnode).left;
    if (pMVar7 == pMVar6) {
      pMVar5 = (MeDict *)(pMVar1->nilnode).right;
      if ((pMVar5->nilnode).color != kMeDictColorRed) {
        if ((MeDict *)node == (MeDict *)(pMVar7->nilnode).right) {
                    /* Unresolved local var: MeDictNode * lower@[???]
                       Unresolved local var: MeDictNode * lowleft@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * upparent@[DW_OP_reg0(EAX)] */
          pMVar3 = (((MeDict *)node)->nilnode).left;
          (pMVar7->nilnode).right = pMVar3;
          pMVar3->parent = &pMVar7->nilnode;
          pMVar3 = (pMVar7->nilnode).parent;
          (((MeDict *)node)->nilnode).parent = pMVar3;
          if (pMVar7 == (MeDict *)pMVar3->left) {
            pMVar3->left = node;
          }
          else {
            pMVar3->right = node;
          }
          (((MeDict *)node)->nilnode).left = &pMVar7->nilnode;
          (pMVar7->nilnode).parent = node;
          pMVar6 = (MeDict *)(pMVar1->nilnode).left;
          pMVar7 = (MeDict *)node;
        }
        (pMVar7->nilnode).color = kMeDictColorBlack;
                    /* Unresolved local var: MeDictNode * lower@[???]
                       Unresolved local var: MeDictNode * lowright@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * upparent@[DW_OP_reg0(EAX)] */
        pMVar3 = (pMVar6->nilnode).right;
        (pMVar1->nilnode).color = kMeDictColorRed;
        pMVar3->parent = (MeDictNode *)pMVar1;
        (pMVar1->nilnode).left = pMVar3;
        pMVar3 = (pMVar1->nilnode).parent;
        (pMVar6->nilnode).parent = pMVar3;
        if (pMVar1 == (MeDict *)pMVar3->right) {
          pMVar3->right = &pMVar6->nilnode;
        }
        else {
          pMVar3->left = &pMVar6->nilnode;
        }
        (pMVar6->nilnode).right = (MeDictNode *)pMVar1;
        goto LAB_00010224;
      }
    }
    else {
      pMVar5 = pMVar6;
      if ((pMVar6->nilnode).color != kMeDictColorRed) {
        if ((MeDict *)node == (MeDict *)(pMVar7->nilnode).left) {
                    /* Unresolved local var: MeDictNode * lower@[???]
                       Unresolved local var: MeDictNode * lowright@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * upparent@[DW_OP_reg0(EAX)] */
          pMVar3 = (((MeDict *)node)->nilnode).right;
          (pMVar7->nilnode).left = pMVar3;
          pMVar3->parent = &pMVar7->nilnode;
          pMVar3 = (pMVar7->nilnode).parent;
          (((MeDict *)node)->nilnode).parent = pMVar3;
          if (pMVar7 == (MeDict *)pMVar3->right) {
            pMVar3->right = node;
          }
          else {
            pMVar3->left = node;
          }
          (((MeDict *)node)->nilnode).right = &pMVar7->nilnode;
          (pMVar7->nilnode).parent = node;
          pMVar7 = (MeDict *)node;
        }
                    /* Unresolved local var: MeDictNode * lower@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeDictNode * lowleft@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * upparent@[DW_OP_reg0(EAX)] */
        pMVar6 = (MeDict *)(pMVar1->nilnode).right;
        (pMVar7->nilnode).color = kMeDictColorBlack;
        pMVar3 = (pMVar6->nilnode).left;
        (pMVar1->nilnode).color = kMeDictColorRed;
        pMVar3->parent = (MeDictNode *)pMVar1;
        (pMVar1->nilnode).right = pMVar3;
        pMVar3 = (pMVar1->nilnode).parent;
        (pMVar6->nilnode).parent = pMVar3;
        if (pMVar1 == (MeDict *)pMVar3->left) {
          pMVar3->left = (MeDictNode *)pMVar6;
        }
        else {
          pMVar3->right = (MeDictNode *)pMVar6;
        }
        (pMVar6->nilnode).left = (MeDictNode *)pMVar1;
LAB_00010224:
        (pMVar1->nilnode).parent = &pMVar6->nilnode;
        goto LAB_000101e7;
      }
    }
    (pMVar7->nilnode).color = kMeDictColorBlack;
    pMVar7 = (MeDict *)(pMVar1->nilnode).parent;
    (pMVar5->nilnode).color = kMeDictColorBlack;
    (pMVar1->nilnode).color = kMeDictColorRed;
    MVar2 = (pMVar7->nilnode).color;
    node = (MeDictNode *)pMVar1;
  } while( true );
}


/* ==== MeDictDelete ==== */

MeDictNode * MeDictDelete(MeDict *dict,MeDictNode *delete)

{
  MeDictColor MVar1;
  MeDictNode *pMVar2;
  MeDictNode *pMVar3;
  MeDictNode *pMVar4;
  MeDictNode *pMVar5;
  MeDictNode *child;
  
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * delparent@[DW_OP_reg3(EBX)] */
  pMVar4 = delete->parent;
  child = delete->left;
  if ((MeDict *)child == dict) {
LAB_000105a7:
    child = delete->right;
  }
  else {
    if ((MeDict *)delete->right != dict) {
                    /* Unresolved local var: MeDictNode * next@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeDictNode * nextparent@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictColor nextcolor@[DW_OP_reg7(EDI)] */
      pMVar3 = MeDictNext(dict,delete);
      pMVar5 = pMVar3->parent;
      child = pMVar3->right;
      MVar1 = pMVar3->color;
      pMVar2 = pMVar5->left;
      child->parent = pMVar5;
      if (pMVar2 == pMVar3) {
        pMVar5->left = child;
      }
      else {
        pMVar5->right = child;
      }
      pMVar5 = delete->left;
      pMVar2 = delete->right;
      pMVar3->parent = pMVar4;
      pMVar3->left = pMVar5;
      pMVar5->parent = pMVar3;
      pMVar3->color = delete->color;
      pMVar3->right = pMVar2;
      pMVar2->parent = pMVar3;
      delete->color = MVar1;
      if (pMVar4->left == delete) {
        pMVar4->left = pMVar3;
      }
      else {
        pMVar4->right = pMVar3;
      }
      goto LAB_0001034a;
    }
    if ((MeDict *)child == dict) goto LAB_000105a7;
  }
  pMVar5 = pMVar4->left;
  child->parent = pMVar4;
  if (delete == pMVar5) {
    pMVar4->left = child;
  }
  else {
    pMVar4->right = child;
  }
LAB_0001034a:
  delete->parent = (MeDictNode *)0x0;
  delete->right = (MeDictNode *)0x0;
  delete->left = (MeDictNode *)0x0;
  dict->nodecount = dict->nodecount - 1;
  if (delete->color == kMeDictColorBlack) {
                    /* Unresolved local var: MeDictNode * parent@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDictNode * sister@[DW_OP_reg1(ECX)] */
    pMVar4 = (dict->nilnode).left;
    pMVar4->color = kMeDictColorRed;
    if (child->color == kMeDictColorBlack) {
      do {
        pMVar4 = child->parent;
        pMVar5 = pMVar4->left;
        if (child == pMVar5) {
          pMVar5 = pMVar4->right;
          if (pMVar5->color == kMeDictColorRed) {
                    /* Unresolved local var: MeDictNode * lower@[???]
                       Unresolved local var: MeDictNode * lowleft@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * upparent@[DW_OP_reg0(EAX)] */
            pMVar2 = pMVar5->left;
            pMVar5->color = kMeDictColorBlack;
            pMVar2->parent = pMVar4;
            pMVar4->right = pMVar2;
            pMVar2 = pMVar4->parent;
            pMVar4->color = kMeDictColorRed;
            pMVar5->parent = pMVar2;
            if (pMVar4 == pMVar2->left) {
              pMVar2->left = pMVar5;
            }
            else {
              pMVar2->right = pMVar5;
            }
            pMVar5->left = pMVar4;
            pMVar4->parent = pMVar5;
            pMVar5 = pMVar4->right;
          }
          pMVar2 = pMVar5->left;
          if (pMVar2->color == kMeDictColorBlack) {
            pMVar3 = pMVar5->right;
            if (pMVar3->color == kMeDictColorBlack) goto LAB_000103fd;
          }
          else {
            pMVar3 = pMVar5->right;
          }
          if (pMVar3->color == kMeDictColorBlack) {
                    /* Unresolved local var: MeDictNode * lower@[???]
                       Unresolved local var: MeDictNode * lowright@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * upparent@[DW_OP_reg0(EAX)] */
            pMVar3 = pMVar2->right;
            pMVar2->color = kMeDictColorBlack;
            pMVar3->parent = pMVar5;
            pMVar5->left = pMVar3;
            pMVar3 = pMVar5->parent;
            pMVar5->color = kMeDictColorRed;
            pMVar2->parent = pMVar3;
            if (pMVar5 == pMVar3->right) {
              pMVar3->right = pMVar2;
            }
            else {
              pMVar3->left = pMVar2;
            }
            pMVar2->right = pMVar5;
            pMVar5->parent = pMVar2;
            pMVar5 = pMVar4->right;
            pMVar3 = pMVar5->right;
          }
          pMVar5->color = pMVar4->color;
                    /* Unresolved local var: MeDictNode * lower@[???]
                       Unresolved local var: MeDictNode * lowleft@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * upparent@[DW_OP_reg0(EAX)] */
          pMVar2 = pMVar5->left;
          pMVar3->color = kMeDictColorBlack;
          pMVar2->parent = pMVar4;
          pMVar4->right = pMVar2;
          pMVar2 = pMVar4->parent;
          pMVar4->color = kMeDictColorBlack;
          pMVar5->parent = pMVar2;
          if (pMVar4 == pMVar2->left) {
            pMVar2->left = pMVar5;
          }
          else {
            pMVar2->right = pMVar5;
          }
          pMVar5->left = pMVar4;
          pMVar4->parent = pMVar5;
          break;
        }
        if (pMVar5->color == kMeDictColorRed) {
                    /* Unresolved local var: MeDictNode * lower@[???]
                       Unresolved local var: MeDictNode * lowright@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * upparent@[DW_OP_reg0(EAX)] */
          pMVar2 = pMVar5->right;
          pMVar5->color = kMeDictColorBlack;
          pMVar2->parent = pMVar4;
          pMVar4->left = pMVar2;
          pMVar2 = pMVar4->parent;
          pMVar4->color = kMeDictColorRed;
          pMVar5->parent = pMVar2;
          if (pMVar4 == pMVar2->right) {
            pMVar2->right = pMVar5;
          }
          else {
            pMVar2->left = pMVar5;
          }
          pMVar5->right = pMVar4;
          pMVar4->parent = pMVar5;
          pMVar5 = pMVar4->left;
        }
        pMVar2 = pMVar5->right;
        if (pMVar2->color != kMeDictColorBlack) {
          pMVar3 = pMVar5->left;
LAB_00010417:
          if (pMVar3->color == kMeDictColorBlack) {
                    /* Unresolved local var: MeDictNode * lower@[???]
                       Unresolved local var: MeDictNode * lowleft@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * upparent@[DW_OP_reg0(EAX)] */
            pMVar3 = pMVar2->left;
            pMVar2->color = kMeDictColorBlack;
            pMVar3->parent = pMVar5;
            pMVar5->right = pMVar3;
            pMVar3 = pMVar5->parent;
            pMVar5->color = kMeDictColorRed;
            pMVar2->parent = pMVar3;
            if (pMVar5 == pMVar3->left) {
              pMVar3->left = pMVar2;
            }
            else {
              pMVar3->right = pMVar2;
            }
            pMVar2->left = pMVar5;
            pMVar5->parent = pMVar2;
            pMVar5 = pMVar4->left;
            pMVar3 = pMVar5->left;
          }
          pMVar5->color = pMVar4->color;
          pMVar3->color = kMeDictColorBlack;
                    /* Unresolved local var: MeDictNode * lower@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeDictNode * lowright@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * upparent@[DW_OP_reg0(EAX)] */
          pMVar5 = pMVar4->left;
          pMVar4->color = kMeDictColorBlack;
          pMVar2 = pMVar5->right;
          pMVar4->left = pMVar2;
          pMVar2->parent = pMVar4;
          pMVar2 = pMVar4->parent;
          pMVar5->parent = pMVar2;
          if (pMVar4 == pMVar2->right) {
            pMVar2->right = pMVar5;
          }
          else {
            pMVar2->left = pMVar5;
          }
          pMVar5->right = pMVar4;
          pMVar4->parent = pMVar5;
          pMVar4 = (dict->nilnode).left;
          goto LAB_0001038a;
        }
        pMVar3 = pMVar5->left;
        if (pMVar3->color != kMeDictColorBlack) goto LAB_00010417;
LAB_000103fd:
        pMVar5->color = kMeDictColorRed;
        child = pMVar4;
      } while (pMVar4->color == kMeDictColorBlack);
      pMVar4 = (dict->nilnode).left;
    }
LAB_0001038a:
    child->color = kMeDictColorBlack;
    pMVar4->color = kMeDictColorBlack;
  }
  return delete;
}


/* ==== MeDictNext ==== */

MeDictNode * MeDictNext(MeDict *dict,MeDictNode *curr)

{
  MeDict *pMVar1;
  MeDict *pMVar2;
  bool bVar3;
  
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * parent@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeDictNode * left@[DW_OP_reg2(EDX)] */
  pMVar1 = (MeDict *)curr->right;
  if (pMVar1 == dict) {
    pMVar1 = (MeDict *)curr->parent;
    if (pMVar1 != dict) {
      pMVar2 = pMVar1;
      if (curr == (pMVar1->nilnode).right) {
        do {
          pMVar1 = (MeDict *)(pMVar2->nilnode).parent;
          if (pMVar1 == dict) goto LAB_000105e4;
          bVar3 = pMVar2 == (MeDict *)(pMVar1->nilnode).right;
          pMVar2 = pMVar1;
        } while (bVar3);
      }
      if (pMVar1 != dict) {
        return &pMVar1->nilnode;
      }
    }
LAB_000105e4:
    pMVar1 = (MeDict *)0x0;
  }
  else {
    for (pMVar2 = (MeDict *)(pMVar1->nilnode).left; pMVar2 != dict;
        pMVar2 = (MeDict *)(pMVar2->nilnode).left) {
      pMVar1 = pMVar2;
    }
  }
  return &pMVar1->nilnode;
}


/* ==== MeDictLoadEnd ==== */

void MeDictLoadEnd(MeDictLoad *load)

{
  MeDict *pMVar1;
  MeDict *pMVar2;
  MeDictNode *pMVar3;
  MeDict *pMVar4;
  MeDict *pMVar5;
  uint uVar6;
  int iVar7;
  MeDictColor MVar8;
  MeDictNode **ppMVar9;
  MeDict *pMVar10;
  uint baselevel;
  ulong botrowcount;
  MeDict *dict;
  MeDictNode *tree [64];
  
  pMVar1 = load->dictptr;
  ppMVar9 = tree;
                    /* Unresolved local var: MeDictNode * curr@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeDictNode * dictnil@[???]
                       Unresolved local var: MeDictNode * loadnil@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDictNode * next@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeDictNode * complete@[DW_OP_reg7(EDI)]
                       Unresolved local var: ulong fullcount@[DW_OP_reg0(EAX)]
                       Unresolved local var: ulong nodecount@[DW_OP_reg1(ECX)]
                       Unresolved local var: uint level@[DW_OP_reg2(EDX)]
                       Unresolved local var: uint i@[DW_OP_reg1(ECX)] */
  for (iVar7 = 0x40; iVar7 != 0; iVar7 = iVar7 + -1) {
    *ppMVar9 = (MeDictNode *)0x0;
    ppMVar9 = ppMVar9 + 1;
  }
  pMVar10 = (MeDict *)0x0;
  uVar6 = 0xffffffff;
  baselevel = 0;
  do {
    uVar6 = uVar6 >> 1;
    if (uVar6 < pMVar1->nodecount) break;
  } while (uVar6 != 0);
  botrowcount = pMVar1->nodecount - uVar6;
  MVar8 = kMeDictColorRed;
  pMVar4 = (MeDict *)(load->nilnode).left;
  pMVar5 = (MeDict *)tree[0];
  do {
    while( true ) {
      if (pMVar4 == (MeDict *)&load->nilnode) {
        if (pMVar10 == (MeDict *)0x0) {
          pMVar10 = pMVar1;
        }
        uVar6 = 0;
        do {
          if (tree[uVar6] != (MeDictNode *)0x0) {
            tree[uVar6]->right = &pMVar10->nilnode;
            (pMVar10->nilnode).parent = tree[uVar6];
            pMVar10 = (MeDict *)tree[uVar6];
          }
          iVar7 = uVar6 + 1;
          if (tree[iVar7] != (MeDictNode *)0x0) {
            tree[iVar7]->right = &pMVar10->nilnode;
            (pMVar10->nilnode).parent = tree[iVar7];
            pMVar10 = (MeDict *)tree[iVar7];
          }
          iVar7 = uVar6 + 2;
          if (tree[iVar7] != (MeDictNode *)0x0) {
            tree[iVar7]->right = &pMVar10->nilnode;
            (pMVar10->nilnode).parent = tree[iVar7];
            pMVar10 = (MeDict *)tree[iVar7];
          }
          iVar7 = uVar6 + 3;
          if (tree[iVar7] != (MeDictNode *)0x0) {
            tree[iVar7]->right = &pMVar10->nilnode;
            (pMVar10->nilnode).parent = tree[iVar7];
            pMVar10 = (MeDict *)tree[iVar7];
          }
          iVar7 = uVar6 + 4;
          if (tree[iVar7] != (MeDictNode *)0x0) {
            tree[iVar7]->right = &pMVar10->nilnode;
            (pMVar10->nilnode).parent = tree[iVar7];
            pMVar10 = (MeDict *)tree[iVar7];
          }
          iVar7 = uVar6 + 5;
          if (tree[iVar7] != (MeDictNode *)0x0) {
            tree[iVar7]->right = &pMVar10->nilnode;
            (pMVar10->nilnode).parent = tree[iVar7];
            pMVar10 = (MeDict *)tree[iVar7];
          }
          iVar7 = uVar6 + 6;
          if (tree[iVar7] != (MeDictNode *)0x0) {
            tree[iVar7]->right = &pMVar10->nilnode;
            (pMVar10->nilnode).parent = tree[iVar7];
            pMVar10 = (MeDict *)tree[iVar7];
          }
          iVar7 = uVar6 + 7;
          if (tree[iVar7] != (MeDictNode *)0x0) {
            tree[iVar7]->right = &pMVar10->nilnode;
            (pMVar10->nilnode).parent = tree[iVar7];
            pMVar10 = (MeDict *)tree[iVar7];
          }
          uVar6 = uVar6 + 8;
        } while (uVar6 < 0x40);
        (pMVar10->nilnode).parent = &pMVar1->nilnode;
        (pMVar1->nilnode).color = kMeDictColorBlack;
        (pMVar1->nilnode).right = &pMVar1->nilnode;
        (pMVar1->nilnode).left = &pMVar10->nilnode;
        (pMVar10->nilnode).color = kMeDictColorBlack;
        return;
      }
      pMVar2 = (MeDict *)(pMVar4->nilnode).left;
      tree[0] = &pMVar5->nilnode;
      if (pMVar10 == (MeDict *)0x0) break;
LAB_0001066a:
      (pMVar4->nilnode).left = &pMVar10->nilnode;
      (pMVar10->nilnode).parent = &pMVar4->nilnode;
      (pMVar4->nilnode).color = MVar8 + kMeDictColorBlack & kMeDictColorBlack;
      tree[MVar8] = &pMVar4->nilnode;
      pMVar10 = (MeDict *)0x0;
      MVar8 = baselevel;
      pMVar4 = pMVar2;
      pMVar5 = (MeDict *)tree[0];
    }
    botrowcount = botrowcount - 1;
    if (botrowcount == 0xffffffff) {
      MVar8 = kMeDictColorBlack;
      baselevel = 1;
      if (pMVar5 != (MeDict *)0x0) {
        tree[0] = (MeDictNode *)0x0;
        (pMVar5->nilnode).right = &pMVar1->nilnode;
        pMVar10 = pMVar5;
        pMVar3 = tree[1];
        while (pMVar3 != (MeDictNode *)0x0) {
          pMVar3->right = &pMVar10->nilnode;
          (pMVar10->nilnode).parent = tree[MVar8];
          pMVar10 = (MeDict *)tree[MVar8];
          tree[MVar8] = (MeDictNode *)0x0;
          MVar8 = MVar8 + kMeDictColorBlack;
          pMVar3 = tree[MVar8];
        }
        goto LAB_000107f8;
      }
    }
    else {
LAB_000107f8:
      if (pMVar10 != (MeDict *)0x0) goto LAB_0001066a;
    }
    (pMVar4->nilnode).left = &pMVar1->nilnode;
    (pMVar4->nilnode).right = &pMVar1->nilnode;
    (pMVar4->nilnode).color = MVar8 & kMeDictColorBlack;
    pMVar3 = tree[MVar8];
    pMVar10 = pMVar4;
    while (pMVar4 = pMVar2, pMVar5 = (MeDict *)tree[0], pMVar3 != (MeDictNode *)0x0) {
      pMVar3->right = &pMVar10->nilnode;
      (pMVar10->nilnode).parent = tree[MVar8];
      pMVar10 = (MeDict *)tree[MVar8];
      tree[MVar8] = (MeDictNode *)0x0;
      MVar8 = MVar8 + kMeDictColorBlack;
      pMVar3 = tree[MVar8];
    }
  } while( true );
}


/* ==== MeDictMerge ==== */

/* WARNING: Unknown calling convention */

void MeDictMerge(MeDict *dest,MeDict *source)

{
  MeDictNode *pMVar1;
  MeDictNode **ppMVar2;
  int iVar3;
  MeDict *pMVar4;
  MeDict *pMVar5;
  MeDict *curr;
  MeDictLoad load;
  
                    /* Unresolved local var: MeDictNode * leftnode@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeDictNode * rightnode@[DW_OP_reg3(EBX)] */
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * root@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeDictNode * left@[DW_OP_reg0(EAX)] */
  curr = (MeDict *)(dest->nilnode).left;
  if (curr == dest) {
LAB_000109de:
    curr = (MeDict *)0x0;
  }
  else {
    for (pMVar5 = (MeDict *)(curr->nilnode).left; pMVar5 != dest;
        pMVar5 = (MeDict *)(pMVar5->nilnode).left) {
      curr = pMVar5;
    }
    if (curr == dest) goto LAB_000109de;
  }
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * root@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeDictNode * left@[DW_OP_reg0(EAX)] */
  pMVar5 = (MeDict *)(source->nilnode).left;
  if (pMVar5 != source) {
    for (pMVar4 = (MeDict *)(pMVar5->nilnode).left; pMVar4 != source;
        pMVar4 = (MeDict *)(pMVar4->nilnode).left) {
      pMVar5 = pMVar4;
    }
    if (pMVar5 != source) goto LAB_0001091b;
  }
  pMVar5 = (MeDict *)0x0;
LAB_0001091b:
  if (source != dest) {
    pMVar1 = &load.nilnode;
    dest->nodecount = 0;
    load.dictptr = dest;
    load.nilnode.left = pMVar1;
    load.nilnode.right = pMVar1;
    while( true ) {
      while (curr != (MeDict *)0x0) {
        if ((pMVar5 != (MeDict *)0x0) &&
           (iVar3 = (*dest->compare)((curr->nilnode).key,(pMVar5->nilnode).key), -1 < iVar3))
        goto LAB_0001095f;
                    /* Unresolved local var: MeDictNode * next@[DW_OP_reg0(EAX)] */
        pMVar4 = (MeDict *)MeDictNext(dest,&curr->nilnode);
        ppMVar2 = &(load.nilnode.right)->left;
        load.nilnode.right = &curr->nilnode;
                    /* Unresolved local var: MeDict * dict@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeDictNode * nil@[???] */
        *ppMVar2 = &curr->nilnode;
        (curr->nilnode).left = pMVar1;
        (load.dictptr)->nodecount = (load.dictptr)->nodecount + 1;
        curr = pMVar4;
      }
      if (pMVar5 == (MeDict *)0x0) break;
LAB_0001095f:
                    /* Unresolved local var: MeDictNode * next@[DW_OP_reg0(EAX)] */
      pMVar4 = (MeDict *)MeDictNext(source,&pMVar5->nilnode);
      ppMVar2 = &(load.nilnode.right)->left;
      load.nilnode.right = &pMVar5->nilnode;
                    /* Unresolved local var: MeDict * dict@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeDictNode * nil@[???] */
      *ppMVar2 = &pMVar5->nilnode;
      (pMVar5->nilnode).left = pMVar1;
      (load.dictptr)->nodecount = (load.dictptr)->nodecount + 1;
      pMVar5 = pMVar4;
    }
    source->nodecount = 0;
    (source->nilnode).left = &source->nilnode;
    (source->nilnode).right = &source->nilnode;
    (source->nilnode).parent = &source->nilnode;
    MeDictLoadEnd(&load);
  }
  return;
}


/* ==== MeDictCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeDict * MeDictCreate(ulong maxcount,MeDictCompareFn comp)

{
  MeDict *pMVar1;
  
                    /* Unresolved local var: MeDict * new@[DW_OP_reg2(EDX)] */
  pMVar1 = (MeDict *)(*_MeMemoryAPI)(0x34);
  if (pMVar1 != (MeDict *)0x0) {
    pMVar1->compare = comp;
    pMVar1->allocnode = dnode_alloc;
    pMVar1->freenode = dnode_free;
    pMVar1->context = (void *)0x0;
    pMVar1->nodecount = 0;
    (pMVar1->nilnode).left = (MeDictNode *)pMVar1;
    pMVar1->maxcount = maxcount;
    (pMVar1->nilnode).right = (MeDictNode *)pMVar1;
    (pMVar1->nilnode).parent = (MeDictNode *)pMVar1;
    (pMVar1->nilnode).color = kMeDictColorBlack;
    pMVar1->dupes = 0;
  }
  return pMVar1;
}


/* ==== MeDictSetAllocator ==== */

void MeDictSetAllocator(MeDict *dict,MeDictAllocFn al,MeDictFreeFn fr,void *context)

{
  if (al == (MeDictAllocFn)0x0) {
    al = dnode_alloc;
  }
  dict->allocnode = al;
  if (fr == (MeDictFreeFn)0x0) {
    fr = dnode_free;
  }
  dict->freenode = fr;
  dict->context = context;
  return;
}


/* ==== MeDictDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void MeDictDestroy(MeDict *dict)

{
  (*_DAT_0001200c)(dict);
  return;
}


/* ==== MeDictFreeNodes ==== */

void MeDictFreeNodes(MeDict *dict)

{
  MeDict *pMVar1;
  
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * root@[DW_OP_reg6(ESI)] */
  pMVar1 = (MeDict *)(dict->nilnode).left;
  if (pMVar1 != dict) {
    free_nodes(dict,(pMVar1->nilnode).left,&dict->nilnode);
    free_nodes(dict,(pMVar1->nilnode).right,&dict->nilnode);
    (*dict->freenode)((MeDictNode *)pMVar1,dict->context);
  }
  dict->nodecount = 0;
  (dict->nilnode).left = &dict->nilnode;
  (dict->nilnode).right = &dict->nilnode;
  return;
}


/* ==== MeDictFree ==== */

void MeDictFree(MeDict *dict)

{
  MeDict *pMVar1;
  MeDict *pMVar2;
  
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * root@[DW_OP_reg7(EDI)] */
  pMVar1 = (MeDict *)(dict->nilnode).left;
  if (pMVar1 != dict) {
    pMVar2 = (MeDict *)(pMVar1->nilnode).left;
    if (pMVar2 != dict) {
      free_nodes(dict,(pMVar2->nilnode).left,&dict->nilnode);
      free_nodes(dict,(pMVar2->nilnode).right,&dict->nilnode);
      (*dict->freenode)((MeDictNode *)pMVar2,dict->context);
    }
    pMVar2 = (MeDict *)(pMVar1->nilnode).right;
    if (pMVar2 != dict) {
      free_nodes(dict,(pMVar2->nilnode).left,&dict->nilnode);
      free_nodes(dict,(pMVar2->nilnode).right,&dict->nilnode);
      (*dict->freenode)((MeDictNode *)pMVar2,dict->context);
    }
    (*dict->freenode)((MeDictNode *)pMVar1,dict->context);
  }
  dict->nodecount = 0;
  (dict->nilnode).left = &dict->nilnode;
  (dict->nilnode).right = &dict->nilnode;
  return;
}


/* ==== MeDictInit ==== */

MeDict * MeDictInit(MeDict *dict,ulong maxcount,MeDictCompareFn comp)

{
  dict->compare = comp;
  dict->allocnode = dnode_alloc;
  dict->freenode = dnode_free;
  dict->context = (void *)0x0;
  dict->nodecount = 0;
  (dict->nilnode).left = &dict->nilnode;
  dict->maxcount = maxcount;
  (dict->nilnode).right = &dict->nilnode;
  (dict->nilnode).parent = &dict->nilnode;
  (dict->nilnode).color = kMeDictColorBlack;
  dict->dupes = 0;
  return dict;
}


/* ==== MeDictInitLike ==== */

void MeDictInitLike(MeDict *dict,MeDict *template)

{
  MeDictCompareFn p_Var1;
  int iVar2;
  
  p_Var1 = template->compare;
  (dict->nilnode).left = &dict->nilnode;
  dict->compare = p_Var1;
  dict->allocnode = template->allocnode;
  dict->freenode = template->freenode;
  dict->context = template->context;
  dict->maxcount = template->maxcount;
  iVar2 = template->dupes;
  dict->nodecount = 0;
  (dict->nilnode).right = &dict->nilnode;
  (dict->nilnode).parent = &dict->nilnode;
  (dict->nilnode).color = kMeDictColorBlack;
  dict->dupes = iVar2;
  return;
}


/* ==== MeDictVerify ==== */

int MeDictVerify(MeDict *dict)

{
  MeDict *pMVar1;
  MeDictColor MVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  ulong uVar6;
  ulong uVar7;
  uint uVar8;
  
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * root@[DW_OP_reg6(ESI)] */
  pMVar1 = (MeDict *)(dict->nilnode).left;
  if (((((pMVar1->nilnode).color != kMeDictColorBlack) ||
       ((dict->nilnode).color != kMeDictColorBlack)) || ((MeDict *)(dict->nilnode).right != dict))
     || ((MeDict *)(pMVar1->nilnode).parent != dict)) {
    return 0;
  }
  iVar3 = verify_bintree(dict);
  if (iVar3 == 0) {
    return 0;
  }
                    /* Unresolved local var: uint height_left@[DW_OP_reg3(EBX)]
                       Unresolved local var: uint height_right@[DW_OP_reg0(EAX)] */
  if (pMVar1 == dict) {
    uVar8 = 1;
    goto LAB_00010ca4;
  }
  uVar4 = verify_redblack(&dict->nilnode,(pMVar1->nilnode).left);
  uVar5 = verify_redblack(&dict->nilnode,(pMVar1->nilnode).right);
  if ((uVar4 != 0) && (uVar5 != 0)) {
    uVar8 = 0;
    if (uVar4 != uVar5) goto LAB_00010ca4;
    MVar2 = (pMVar1->nilnode).color;
    if (MVar2 != kMeDictColorRed) {
      uVar8 = uVar4 + 1 & (MVar2 != kMeDictColorBlack) - 1;
      goto LAB_00010ca4;
    }
    if (((pMVar1->nilnode).left)->color == kMeDictColorBlack) {
      uVar8 = (((pMVar1->nilnode).right)->color != kMeDictColorBlack) - 1 & uVar4;
      goto LAB_00010ca4;
    }
  }
  uVar8 = 0;
LAB_00010ca4:
  uVar4 = 0;
  if (uVar8 != 0) {
    uVar6 = 0;
    if (pMVar1 != dict) {
      uVar6 = verify_node_count(&dict->nilnode,(pMVar1->nilnode).left);
      uVar7 = verify_node_count(&dict->nilnode,(pMVar1->nilnode).right);
      uVar6 = uVar7 + 1 + uVar6;
    }
    uVar4 = (uint)(uVar6 == dict->nodecount);
  }
  return uVar4;
}


/* ==== MeDictSimilar ==== */

int MeDictSimilar(MeDict *left,MeDict *right)

{
  uint uVar1;
  
  if ((((left->compare == right->compare) && (left->allocnode == right->allocnode)) &&
      (left->freenode == right->freenode)) && (left->context == right->context)) {
    uVar1 = (uint)(left->dupes == right->dupes);
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}


/* ==== MeDictAllocInsert ==== */

int MeDictAllocInsert(MeDict *dict,void *key,void *data)

{
  MeDictNode *node;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg2(EDX)] */
  node = (*dict->allocnode)(dict->context);
  if (node != (MeDictNode *)0x0) {
    node->left = (MeDictNode *)0x0;
    node->data = data;
    node->parent = (MeDictNode *)0x0;
    node->right = (MeDictNode *)0x0;
    MeDictInsert(dict,node,key);
  }
  return (uint)(node != (MeDictNode *)0x0);
}


/* ==== MeDictDeleteFree ==== */

void MeDictDeleteFree(MeDict *dict,MeDictNode *node)

{
  MeDictDelete(dict,node);
  (*dict->freenode)(node,dict->context);
  return;
}


/* ==== MeDictFirst ==== */

MeDictNode * MeDictFirst(MeDict *dict)

{
  MeDict *pMVar1;
  MeDict *pMVar2;
  
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * root@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * left@[DW_OP_reg2(EDX)] */
  pMVar1 = (MeDict *)(dict->nilnode).left;
  if (pMVar1 != dict) {
    for (pMVar2 = (MeDict *)(pMVar1->nilnode).left; pMVar2 != dict;
        pMVar2 = (MeDict *)(pMVar2->nilnode).left) {
      pMVar1 = pMVar2;
    }
    if (pMVar1 != dict) {
      return &pMVar1->nilnode;
    }
  }
  return (MeDictNode *)0x0;
}


/* ==== MeDictLast ==== */

MeDictNode * MeDictLast(MeDict *dict)

{
  MeDict *pMVar1;
  MeDict *pMVar2;
  
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * root@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * right@[DW_OP_reg2(EDX)] */
  pMVar1 = (MeDict *)(dict->nilnode).left;
  if (pMVar1 != dict) {
    for (pMVar2 = (MeDict *)(pMVar1->nilnode).right; pMVar2 != dict;
        pMVar2 = (MeDict *)(pMVar2->nilnode).right) {
      pMVar1 = pMVar2;
    }
    if (pMVar1 != dict) {
      return &pMVar1->nilnode;
    }
  }
  return (MeDictNode *)0x0;
}


/* ==== MeDictPrev ==== */

MeDictNode * MeDictPrev(MeDict *dict,MeDictNode *curr)

{
  MeDict *pMVar1;
  MeDict *pMVar2;
  bool bVar3;
  
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * parent@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeDictNode * right@[DW_OP_reg2(EDX)] */
  pMVar1 = (MeDict *)curr->left;
  if (pMVar1 == dict) {
    pMVar1 = (MeDict *)curr->parent;
    if (pMVar1 != dict) {
      pMVar2 = pMVar1;
      if (curr == (pMVar1->nilnode).left) {
        do {
          pMVar1 = (MeDict *)(pMVar2->nilnode).parent;
          if (pMVar1 == dict) goto LAB_00010e94;
          bVar3 = pMVar2 == (MeDict *)(pMVar1->nilnode).left;
          pMVar2 = pMVar1;
        } while (bVar3);
      }
      if (pMVar1 != dict) {
        return &pMVar1->nilnode;
      }
    }
LAB_00010e94:
    pMVar1 = (MeDict *)0x0;
  }
  else {
    for (pMVar2 = (MeDict *)(pMVar1->nilnode).right; pMVar2 != dict;
        pMVar2 = (MeDict *)(pMVar2->nilnode).right) {
      pMVar1 = pMVar2;
    }
  }
  return &pMVar1->nilnode;
}


/* ==== MeDictAllowDupes ==== */

void MeDictAllowDupes(MeDict *dict)

{
  dict->dupes = 1;
  return;
}


/* ==== MeDictCount ==== */

ulong MeDictCount(MeDict *dict)

{
  return dict->nodecount;
}


/* ==== MeDictIsEmpty ==== */

int MeDictIsEmpty(MeDict *dict)

{
  return (uint)(dict->nodecount == 0);
}


/* ==== MeDictIsFull ==== */

int MeDictIsFull(MeDict *dict)

{
  return (uint)(dict->nodecount == dict->maxcount);
}


/* ==== MeDictContains ==== */

int MeDictContains(MeDict *dict,MeDictNode *node)

{
  MeDict *pMVar1;
  int iVar2;
  int local_14;
  
  pMVar1 = (MeDict *)(dict->nilnode).left;
  if (pMVar1 == dict) {
    local_14 = 0;
  }
  else {
    local_14 = 0;
    if (((pMVar1 == (MeDict *)node) ||
        (iVar2 = verify_dict_has_node(&dict->nilnode,(pMVar1->nilnode).left,node), iVar2 != 0)) ||
       (iVar2 = verify_dict_has_node(&dict->nilnode,(pMVar1->nilnode).right,node), iVar2 != 0)) {
      local_14 = 1;
    }
  }
  return local_14;
}


/* ==== MeDictNodeCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeDictNode * MeDictNodeCreate(void *data)

{
  MeDictNode *pMVar1;
  
                    /* Unresolved local var: MeDictNode * new@[DW_OP_reg2(EDX)] */
  pMVar1 = (MeDictNode *)(*_MeMemoryAPI)(0x18);
  if (pMVar1 != (MeDictNode *)0x0) {
    pMVar1->left = (MeDictNode *)0x0;
    pMVar1->data = data;
    pMVar1->parent = (MeDictNode *)0x0;
    pMVar1->right = (MeDictNode *)0x0;
  }
  return pMVar1;
}


/* ==== MeDictNodeInit ==== */

MeDictNode * MeDictNodeInit(MeDictNode *dnode,void *data)

{
  dnode->data = data;
  dnode->left = (MeDictNode *)0x0;
  dnode->parent = (MeDictNode *)0x0;
  dnode->right = (MeDictNode *)0x0;
  return dnode;
}


/* ==== MeDictNodeDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void MeDictNodeDestroy(MeDictNode *dnode)

{
  (*_DAT_0001200c)(dnode);
  return;
}


/* ==== MeDictNodeGet ==== */

void * MeDictNodeGet(MeDictNode *dnode)

{
  return dnode->data;
}


/* ==== MeDictNodeGetkey ==== */

void * MeDictNodeGetkey(MeDictNode *dnode)

{
  return dnode->key;
}


/* ==== MeDictNodePut ==== */

void MeDictNodePut(MeDictNode *dnode,void *data)

{
  dnode->data = data;
  return;
}


/* ==== MeDictNodeIsInADict ==== */

int MeDictNodeIsInADict(MeDictNode *dnode)

{
  int iVar1;
  
  iVar1 = 0;
  if (((dnode->parent != (MeDictNode *)0x0) && (dnode->left != (MeDictNode *)0x0)) &&
     (dnode->right != (MeDictNode *)0x0)) {
    iVar1 = 1;
  }
  return iVar1;
}


/* ==== MeDictProcess ==== */

void MeDictProcess(MeDict *dict,void *context,MeDictProcessFn function)

{
  MeDict *pMVar1;
  MeDict *curr;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeDictNode * next@[DW_OP_reg3(EBX)] */
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * root@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeDictNode * left@[DW_OP_reg0(EAX)] */
  curr = (MeDict *)(dict->nilnode).left;
  if (curr != dict) {
    for (pMVar1 = (MeDict *)(curr->nilnode).left; pMVar1 != dict;
        pMVar1 = (MeDict *)(pMVar1->nilnode).left) {
      curr = pMVar1;
    }
    if (curr != dict) goto joined_r0x0001109a;
  }
  curr = (MeDict *)0x0;
joined_r0x0001109a:
  while (curr != (MeDict *)0x0) {
    pMVar1 = (MeDict *)MeDictNext(dict,&curr->nilnode);
    (*function)(dict,&curr->nilnode,context);
    curr = pMVar1;
  }
  return;
}


/* ==== MeDictLoadBegin ==== */

void MeDictLoadBegin(MeDictLoad *load,MeDict *dict)

{
  load->dictptr = dict;
  (load->nilnode).left = &load->nilnode;
  (load->nilnode).right = &load->nilnode;
  return;
}


/* ==== MeDictLoadNext ==== */

void MeDictLoadNext(MeDictLoad *load,MeDictNode *newnode,void *key)

{
  MeDict *pMVar1;
  MeDictNode *pMVar2;
  
                    /* Unresolved local var: MeDict * dict@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDictNode * nil@[DW_OP_reg2(EDX)] */
  pMVar1 = load->dictptr;
  newnode->key = key;
  pMVar2 = (load->nilnode).right;
  (load->nilnode).right = newnode;
  pMVar2->left = newnode;
  newnode->left = &load->nilnode;
  pMVar1->nodecount = pMVar1->nodecount + 1;
  return;
}


/* ==== dnode_alloc ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

MeDictNode * dnode_alloc(void *context)

{
  MeDictNode *pMVar1;
  
  pMVar1 = (MeDictNode *)(*_MeMemoryAPI)(0x18);
  return pMVar1;
}


/* ==== dnode_free ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void dnode_free(MeDictNode *node,void *context)

{
  (*_DAT_0001200c)(node);
  return;
}


/* ==== free_nodes ==== */

void free_nodes(MeDict *dict,MeDictNode *node,MeDictNode *nil)

{
  if (node != nil) {
    free_nodes(dict,node->left,nil);
    free_nodes(dict,node->right,nil);
    (*dict->freenode)(node,dict->context);
  }
  return;
}


/* ==== verify_bintree ==== */

int verify_bintree(MeDict *dict)

{
  MeDict *pMVar1;
  int iVar2;
  MeDict *curr;
  
                    /* Unresolved local var: MeDictNode * first@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeDictNode * next@[DW_OP_reg3(EBX)] */
                    /* Unresolved local var: MeDictNode * nil@[???]
                       Unresolved local var: MeDictNode * root@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeDictNode * left@[DW_OP_reg0(EAX)] */
  curr = (MeDict *)(dict->nilnode).left;
  if (curr != dict) {
    for (pMVar1 = (MeDict *)(curr->nilnode).left; pMVar1 != dict;
        pMVar1 = (MeDict *)(pMVar1->nilnode).left) {
      curr = pMVar1;
    }
    if (curr != dict) goto LAB_000111e2;
  }
  curr = (MeDict *)0x0;
LAB_000111e2:
  if (dict->dupes == 0) {
    do {
      if (curr == (MeDict *)0x0) {
        return 1;
      }
      pMVar1 = (MeDict *)MeDictNext(dict,&curr->nilnode);
      if (pMVar1 == (MeDict *)0x0) {
        return 1;
      }
      iVar2 = (*dict->compare)((curr->nilnode).key,(pMVar1->nilnode).key);
      curr = pMVar1;
    } while (iVar2 < 0);
  }
  else {
    do {
      if ((curr == (MeDict *)0x0) ||
         (pMVar1 = (MeDict *)MeDictNext(dict,&curr->nilnode), pMVar1 == (MeDict *)0x0)) {
        return 1;
      }
      iVar2 = (*dict->compare)((curr->nilnode).key,(pMVar1->nilnode).key);
      curr = pMVar1;
    } while (iVar2 < 1);
  }
  return 0;
}


/* ==== verify_redblack ==== */

uint verify_redblack(MeDictNode *nil,MeDictNode *root)

{
  uint uVar1;
  uint uVar2;
  
                    /* Unresolved local var: uint height_left@[DW_OP_reg7(EDI)]
                       Unresolved local var: uint height_right@[DW_OP_reg0(EAX)] */
  if (root == nil) {
    return 1;
  }
  uVar1 = verify_redblack(nil,root->left);
  uVar2 = verify_redblack(nil,root->right);
  if ((uVar1 != 0) && (uVar2 != 0)) {
    if (uVar1 != uVar2) {
      return 0;
    }
    if (root->color != kMeDictColorRed) {
      return uVar1 + 1 & (root->color != kMeDictColorBlack) - 1;
    }
    if (root->left->color == kMeDictColorBlack) {
      return (root->right->color != kMeDictColorBlack) - 1 & uVar1;
    }
  }
  return 0;
}


/* ==== verify_node_count ==== */

ulong verify_node_count(MeDictNode *nil,MeDictNode *root)

{
  ulong uVar1;
  ulong uVar2;
  
  uVar1 = 0;
  if (root != nil) {
    uVar1 = verify_node_count(nil,root->left);
    uVar2 = verify_node_count(nil,root->right);
    uVar1 = uVar2 + 1 + uVar1;
  }
  return uVar1;
}


/* ==== verify_dict_has_node ==== */

int verify_dict_has_node(MeDictNode *nil,MeDictNode *root,MeDictNode *node)

{
  int iVar1;
  int local_14;
  
  if (root == nil) {
    local_14 = 0;
  }
  else {
    local_14 = 0;
    if (((root == node) || (iVar1 = verify_dict_has_node(nil,root->left,node), iVar1 != 0)) ||
       (iVar1 = verify_dict_has_node(nil,root->right,node), iVar1 != 0)) {
      local_14 = 1;
    }
  }
  return local_14;
}


