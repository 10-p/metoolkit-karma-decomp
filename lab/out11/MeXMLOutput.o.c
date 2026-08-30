/* ==== MeXMLWriteElement ==== */

int MeXMLWriteElement(MeXMLOutput *op,int parent,char *tag,...)

{
  int iVar1;
  uint *puVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  char buffer [1024];
  
                    /* Unresolved local var: va_list args@[???] */
  iVar1 = op->depth;
  while (parent < iVar1) {
    closeTag(op);
    iVar1 = op->depth;
  }
  if (op->tagHead != (tagNode *)0x0) {
    op->tagHead->contents = kElement;
  }
  pushTag(op,tag);
  vsprintf(buffer,tag,&stack0x00000010);
  if (parent != 0) {
    newLine(op);
  }
  MeStreamWrite(&DAT_00010405,1,1,op->stream);
  puVar3 = (uint *)buffer;
  do {
    puVar2 = puVar3;
    puVar3 = puVar2 + 1;
    uVar4 = *puVar2 + 0xfefefeff & (*puVar2 ^ 0xffffffff);
    uVar5 = uVar4 & 0x80808080;
  } while (uVar5 == 0);
  if ((uVar4 & 0x8080) == 0) {
    uVar5 = uVar5 >> 0x10;
    puVar3 = (uint *)((int)puVar2 + 6);
  }
  MeStreamWrite(buffer,(uint)((int)puVar3 +
                             ((-3 - (uint)CARRY1((byte)uVar5,(byte)uVar5)) - (int)buffer)),1,
                op->stream);
  MeStreamWrite(&DAT_00010407,1,1,op->stream);
  return op->depth;
}


/* ==== MeXMLWritePCDATA ==== */

void MeXMLWritePCDATA(MeXMLOutput *op,char *format,...)

{
  uint *puVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  char buffer [1024];
  
                    /* Unresolved local var: va_list args@[???] */
  vsprintf(buffer,format,&stack0x0000000c);
  puVar2 = (uint *)buffer;
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
  MeStreamWrite(buffer,(uint)((int)puVar2 +
                             ((-3 - (uint)CARRY1((byte)uVar4,(byte)uVar4)) - (int)buffer)),1,
                op->stream);
  op->tagHead->contents = kPCDATA;
  closeTag(op);
  return;
}


/* ==== MeXMLWriteComment ==== */

void MeXMLWriteComment(MeXMLOutput *op,char *c,...)

{
  uint *puVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  char buffer [4096];
  
                    /* Unresolved local var: va_list args@[???] */
  vsprintf(buffer,c,&stack0x0000000c);
  MeStreamWrite("<!--\n",5,1,op->stream);
  puVar2 = (uint *)buffer;
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
  MeStreamWrite(buffer,(uint)((int)puVar2 +
                             ((-3 - (uint)CARRY1((byte)uVar4,(byte)uVar4)) - (int)buffer)),1,
                op->stream);
  MeStreamWrite("\n-->\n",5,1,op->stream);
  return;
}


/* ==== pushTag ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void pushTag(MeXMLOutput *op,char *tag)

{
  tagNode *ptVar1;
  char *__dest;
  uint *puVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  char buffer [1024];
  
                    /* Unresolved local var: tagNode * node@[DW_OP_reg3(EBX)] */
  strncpy(buffer,tag,0x400);
  strtok(buffer," ");
  ptVar1 = (*_MeMemoryAPI)(0xc);
  puVar3 = (uint *)buffer;
  do {
    puVar2 = puVar3;
    puVar3 = puVar2 + 1;
    uVar4 = *puVar2 + 0xfefefeff & (*puVar2 ^ 0xffffffff);
    uVar5 = uVar4 & 0x80808080;
  } while (uVar5 == 0);
  if ((uVar4 & 0x8080) == 0) {
    uVar5 = uVar5 >> 0x10;
    puVar3 = (uint *)((int)puVar2 + 6);
  }
  __dest = (*_MeMemoryAPI)((uint)((int)puVar3 +
                                 (-(int)buffer - (uint)CARRY1((byte)uVar5,(byte)uVar5)) + -2));
  ptVar1->tag = __dest;
  strcpy(__dest,buffer);
  ptVar1->contents = kNothing;
  if (op->tagHead == (tagNode *)0x0) {
    op->tagHead = ptVar1;
    ptVar1->next = (tagNode *)0x0;
  }
  else {
    ptVar1->next = op->tagHead;
    op->tagHead = ptVar1;
  }
  op->depth = op->depth + 1;
  return;
}


/* ==== closeTag ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void closeTag(MeXMLOutput *op)

{
  tagNode *ptVar1;
  size_t sVar2;
  uint uVar3;
  MeStream p_Var4;
  
  if (op->tagHead->contents == kElement) {
    newLine(op);
  }
  MeStreamWrite(&DAT_00010417,2,1,op->stream);
  p_Var4 = op->stream;
  uVar3 = 1;
  ptVar1 = op->tagHead;
  sVar2 = strlen(ptVar1->tag);
  MeStreamWrite(ptVar1->tag,sVar2,uVar3,p_Var4);
  MeStreamWrite(&DAT_00010407,1,1,op->stream);
                    /* Unresolved local var: tagNode * temp@[DW_OP_reg3(EBX)] */
  ptVar1 = op->tagHead->next;
  (*_DAT_0001101c)(op->tagHead->tag);
  (*_DAT_0001101c)(op->tagHead);
  op->tagHead = ptVar1;
  op->depth = op->depth + -1;
  return;
}


/* ==== newLine ==== */

void newLine(MeXMLOutput *op)

{
  int iVar1;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar1 = 0;
  MeStreamWrite(&DAT_0001041a,1,1,op->stream);
  if (0 < op->depth + -1) {
    do {
      iVar1 = iVar1 + 1;
      MeStreamWrite(&DAT_0001041c,1,1,op->stream);
    } while (iVar1 < op->depth + -1);
  }
  return;
}


/* ==== MeXMLOutputCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeXMLOutput * MeXMLOutputCreate(MeStream stream)

{
  MeXMLOutput *pMVar1;
  
                    /* Unresolved local var: MeXMLOutput * op@[DW_OP_reg0(EAX)] */
  pMVar1 = (*_MeMemoryAPI)(0xc);
  pMVar1->stream = stream;
  pMVar1->depth = 0;
  pMVar1->tagHead = (tagNode *)0x0;
  return pMVar1;
}


/* ==== MeXMLOutputDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeXMLOutputDestroy(MeXMLOutput *op)

{
  tagNode *ptVar1;
  
  ptVar1 = op->tagHead;
  while (ptVar1 != (tagNode *)0x0) {
    closeTag(op);
    ptVar1 = op->tagHead;
  }
  (*_DAT_0001101c)(op);
  return;
}


