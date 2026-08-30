/* ==== MeXMLParseUIntArray ==== */

/* WARNING: Unknown calling convention */

MeXMLError MeXMLParseUIntArray(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  MeXMLError MVar4;
  undefined4 uVar5;
  ushort **ppuVar6;
  char *pcVar7;
  uint uVar8;
  uint uVar9;
  bool bVar10;
  int posn;
  int line;
  char *c;
  char *d;
  char x [1024];
  
                    /* Unresolved local var: uint * dest@[DW_OP_reg7(EDI)]
                       Unresolved local var: uint i@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg0(EAX)] */
  c = x;
  iVar1 = file->line;
  iVar2 = file->posn;
  uVar8 = 0;
  uVar3 = action->offset;
  MVar4 = readToNextTag(file,c,0x400);
  if (MVar4 == MeXMLErrorNone) {
    if (x[0] == '\0') {
LAB_00010107:
      if (uVar8 < action->max) {
        sprintf(file->error,"line %d, char %d: expected %d unsigned ints, found %d\n",iVar1,iVar2,
                action->max,uVar8,action,action);
        return MeXMLErrorInvalidValue;
      }
    }
    else {
      uVar9 = uVar8;
      if (action->max != 0) {
        do {
          uVar8 = uVar9 + 1;
          uVar5 = __strtol_internal(c,&d,0,0);
          pcVar7 = d;
          *(undefined4 *)((int)data + uVar9 * 4 + uVar3) = uVar5;
          if (c == d) {
LAB_0001013e:
            sprintf(file->error,"line %d, char %d: unsigned int array data invalid\n",iVar1,iVar2);
            return MeXMLErrorInvalidValue;
          }
          ppuVar6 = __ctype_b_loc();
          while (((*ppuVar6)[*pcVar7] & 0x2000) != 0) {
            pcVar7 = pcVar7 + 1;
          }
          bVar10 = false;
          c = pcVar7;
          if (*pcVar7 != '\0') {
            if (*pcVar7 == ',') {
              ppuVar6 = __ctype_b_loc();
              do {
                pcVar7 = pcVar7 + 1;
              } while (((*ppuVar6)[*pcVar7] & 0x2000) != 0);
              bVar10 = *pcVar7 == '\0';
              c = pcVar7;
            }
            else {
              bVar10 = true;
            }
          }
          if (bVar10) goto LAB_0001013e;
          if (*c == '\0') goto LAB_00010107;
          uVar9 = uVar8;
        } while (uVar8 < action->max);
      }
    }
    MVar4 = MeXMLErrorNone;
  }
  return MVar4;
}


/* ==== MeXMLParseIntArray ==== */

/* WARNING: Unknown calling convention */

MeXMLError MeXMLParseIntArray(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  int iVar1;
  int iVar2;
  MeXMLError MVar3;
  undefined4 uVar4;
  ushort **ppuVar5;
  char *pcVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  bool bVar10;
  int posn;
  int line;
  char *c;
  char *d;
  char x [1024];
  
                    /* Unresolved local var: int * dest@[DW_OP_reg7(EDI)]
                       Unresolved local var: uint i@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg0(EAX)] */
  c = x;
  iVar1 = file->line;
  iVar2 = file->posn;
  uVar7 = 0;
  iVar9 = (int)data + action->offset;
  MVar3 = readToNextTag(file,c,0x400);
  if (MVar3 == MeXMLErrorNone) {
    if (x[0] == '\0') {
LAB_000102a7:
      if (uVar7 < action->max) {
        sprintf(file->error,"line %d, char %d: expected %d ints, found %d\n",iVar1,iVar2,action->max
                ,uVar7,iVar9,iVar9);
        return MeXMLErrorInvalidValue;
      }
    }
    else {
      uVar8 = uVar7;
      if (action->max != 0) {
        do {
          uVar7 = uVar8 + 1;
          uVar4 = __strtol_internal(c,&d,0,0);
          pcVar6 = d;
          *(undefined4 *)(iVar9 + uVar8 * 4) = uVar4;
          if (c == d) {
LAB_000102de:
            sprintf(file->error,"line %d, char %d: int array data invalid\n",iVar1,iVar2);
            return MeXMLErrorInvalidValue;
          }
          ppuVar5 = __ctype_b_loc();
          while (((*ppuVar5)[*pcVar6] & 0x2000) != 0) {
            pcVar6 = pcVar6 + 1;
          }
          bVar10 = false;
          c = pcVar6;
          if (*pcVar6 != '\0') {
            if (*pcVar6 == ',') {
              ppuVar5 = __ctype_b_loc();
              do {
                pcVar6 = pcVar6 + 1;
              } while (((*ppuVar5)[*pcVar6] & 0x2000) != 0);
              bVar10 = *pcVar6 == '\0';
              c = pcVar6;
            }
            else {
              bVar10 = true;
            }
          }
          if (bVar10) goto LAB_000102de;
          if (*c == '\0') goto LAB_000102a7;
          uVar8 = uVar7;
        } while (uVar7 < action->max);
      }
    }
    MVar3 = MeXMLErrorNone;
  }
  return MVar3;
}


/* ==== MeXMLParseFloatArray ==== */

/* WARNING: Unknown calling convention */

MeXMLError MeXMLParseFloatArray(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  MeXMLError MVar4;
  ushort **ppuVar5;
  ushort *extraout_EDX;
  ushort *puVar6;
  char *pcVar7;
  uint uVar8;
  uint uVar9;
  bool bVar10;
  longdouble lVar11;
  int posn;
  int line;
  char *c;
  char *d;
  char x [1024];
  
                    /* Unresolved local var: float * dest@[DW_OP_reg7(EDI)]
                       Unresolved local var: uint i@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg0(EAX)] */
  c = x;
  iVar1 = file->line;
  iVar2 = file->posn;
  uVar8 = 0;
  uVar3 = action->offset;
  MVar4 = readToNextTag(file,c,0x400);
  if (MVar4 == MeXMLErrorNone) {
    if (x[0] == '\0') {
LAB_0001043b:
      if (uVar8 < action->max) {
        sprintf(file->error,"line %d, char %d: expected %d floats, found %d\n",iVar1,iVar2,
                action->max,uVar8,action,action);
        return MeXMLErrorInvalidValue;
      }
    }
    else {
      puVar6 = extraout_EDX;
      uVar9 = uVar8;
      if (action->max != 0) {
        do {
          uVar8 = uVar9 + 1;
          lVar11 = (longdouble)__strtod_internal(c,&d,0,puVar6);
          pcVar7 = d;
          *(float *)((int)data + uVar9 * 4 + uVar3) = (float)lVar11;
          if (c == d) {
LAB_00010472:
            sprintf(file->error,"line %d, char %d: float array data invalid\n",iVar1,iVar2);
            return MeXMLErrorInvalidValue;
          }
          ppuVar5 = __ctype_b_loc();
          while (((*ppuVar5)[*pcVar7] & 0x2000) != 0) {
            pcVar7 = pcVar7 + 1;
          }
          bVar10 = false;
          puVar6 = (ushort *)(int)*pcVar7;
          c = pcVar7;
          if (puVar6 != (ushort *)0x0) {
            if (puVar6 == (ushort *)0x2c) {
              ppuVar5 = __ctype_b_loc();
              puVar6 = *ppuVar5;
              do {
                pcVar7 = pcVar7 + 1;
              } while ((puVar6[*pcVar7] & 0x2000) != 0);
              bVar10 = *pcVar7 == '\0';
              c = pcVar7;
            }
            else {
              bVar10 = true;
            }
          }
          if (bVar10) goto LAB_00010472;
          if (*c == '\0') goto LAB_0001043b;
          uVar9 = uVar8;
        } while (uVar8 < action->max);
      }
    }
    MVar4 = MeXMLErrorNone;
  }
  return MVar4;
}


/* ==== MeXMLParseMeRealArray ==== */

/* WARNING: Unknown calling convention */

MeXMLError MeXMLParseMeRealArray(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  MeXMLError MVar4;
  ushort **ppuVar5;
  ushort *extraout_EDX;
  ushort *puVar6;
  char *pcVar7;
  uint uVar8;
  uint uVar9;
  bool bVar10;
  longdouble lVar11;
  int posn;
  int line;
  char *c;
  char *d;
  char x [1024];
  
                    /* Unresolved local var: MeReal * dest@[DW_OP_reg7(EDI)]
                       Unresolved local var: uint i@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg0(EAX)] */
  c = x;
  iVar1 = file->line;
  iVar2 = file->posn;
  uVar8 = 0;
  uVar3 = action->offset;
  MVar4 = readToNextTag(file,c,0x400);
  if (MVar4 == MeXMLErrorNone) {
    puVar6 = extraout_EDX;
    if (x[0] == '\0') {
LAB_000105cb:
      if (uVar8 < action->max) {
        sprintf(file->error,"line %d, char %d: expected %d MeReals, found %d\n",iVar1,iVar2,
                action->max,uVar8,puVar6,puVar6);
        return MeXMLErrorInvalidValue;
      }
    }
    else {
      uVar9 = uVar8;
      if (action->max != 0) {
        do {
          uVar8 = uVar9 + 1;
          lVar11 = (longdouble)__strtod_internal(c,&d,0,puVar6);
          pcVar7 = d;
          *(float *)((int)data + uVar9 * 4 + uVar3) = (float)lVar11;
          if (c == d) {
LAB_00010602:
            sprintf(file->error,"line %d, char %d: MeReal array data invalid\n",iVar1,iVar2);
            return MeXMLErrorInvalidValue;
          }
          ppuVar5 = __ctype_b_loc();
          while (((*ppuVar5)[*pcVar7] & 0x2000) != 0) {
            pcVar7 = pcVar7 + 1;
          }
          bVar10 = false;
          puVar6 = (ushort *)(int)*pcVar7;
          c = pcVar7;
          if (puVar6 != (ushort *)0x0) {
            if (puVar6 == (ushort *)0x2c) {
              ppuVar5 = __ctype_b_loc();
              puVar6 = *ppuVar5;
              do {
                pcVar7 = pcVar7 + 1;
              } while ((puVar6[*pcVar7] & 0x2000) != 0);
              bVar10 = *pcVar7 == '\0';
              c = pcVar7;
            }
            else {
              bVar10 = true;
            }
          }
          if (bVar10) goto LAB_00010602;
          if (*c == '\0') goto LAB_000105cb;
          uVar9 = uVar8;
        } while (uVar8 < action->max);
      }
    }
    MVar4 = MeXMLErrorNone;
  }
  return MVar4;
}


/* ==== MeXMLParseDoubleArray ==== */

/* WARNING: Unknown calling convention */

MeXMLError MeXMLParseDoubleArray(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  MeXMLError MVar4;
  ushort **ppuVar5;
  ushort *extraout_EDX;
  ushort *puVar6;
  char *pcVar7;
  uint uVar8;
  uint uVar9;
  bool bVar10;
  longdouble lVar11;
  int posn;
  int line;
  char *c;
  char *d;
  char x [1024];
  
                    /* Unresolved local var: double * dest@[DW_OP_reg7(EDI)]
                       Unresolved local var: uint i@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg0(EAX)] */
  c = x;
  iVar1 = file->line;
  iVar2 = file->posn;
  uVar8 = 0;
  uVar3 = action->offset;
  MVar4 = readToNextTag(file,c,0x400);
  if (MVar4 == MeXMLErrorNone) {
    puVar6 = extraout_EDX;
    if (x[0] == '\0') {
LAB_0001075b:
      if (uVar8 < action->max) {
        sprintf(file->error,"line %d, char %d: expected %d doubles, found %d\n",iVar1,iVar2,
                action->max,uVar8,puVar6,puVar6);
        return MeXMLErrorInvalidValue;
      }
    }
    else {
      uVar9 = uVar8;
      if (action->max != 0) {
        do {
          uVar8 = uVar9 + 1;
          lVar11 = (longdouble)__strtod_internal(c,&d,0,puVar6);
          pcVar7 = d;
          *(double *)((int)data + uVar9 * 8 + uVar3) = (double)lVar11;
          if (c == d) {
LAB_00010792:
            sprintf(file->error,"line %d, char %d: double array data invalid\n",iVar1,iVar2);
            return MeXMLErrorInvalidValue;
          }
          ppuVar5 = __ctype_b_loc();
          while (((*ppuVar5)[*pcVar7] & 0x2000) != 0) {
            pcVar7 = pcVar7 + 1;
          }
          bVar10 = false;
          puVar6 = (ushort *)(int)*pcVar7;
          c = pcVar7;
          if (puVar6 != (ushort *)0x0) {
            if (puVar6 == (ushort *)0x2c) {
              ppuVar5 = __ctype_b_loc();
              puVar6 = *ppuVar5;
              do {
                pcVar7 = pcVar7 + 1;
              } while ((puVar6[*pcVar7] & 0x2000) != 0);
              bVar10 = *pcVar7 == '\0';
              c = pcVar7;
            }
            else {
              bVar10 = true;
            }
          }
          if (bVar10) goto LAB_00010792;
          if (*c == '\0') goto LAB_0001075b;
          uVar9 = uVar8;
        } while (uVar8 < action->max);
      }
    }
    MVar4 = MeXMLErrorNone;
  }
  return MVar4;
}


/* ==== MeXMLParseStringArray ==== */

MeXMLError MeXMLParseStringArray(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  int iVar1;
  int iVar2;
  MeXMLError MVar3;
  char *pcVar4;
  size_t sVar5;
  char *extraout_ECX;
  char *pcVar6;
  char *__dest;
  char *pcVar7;
  uint uVar8;
  undefined *puVar9;
  uint i;
  int posn;
  int line;
  char x [1024];
  
                    /* Unresolved local var: char * c@[DW_OP_reg3(EBX)]
                       Unresolved local var: char * dest@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg0(EAX)] */
  pcVar7 = x;
  iVar1 = file->line;
  iVar2 = file->posn;
  __dest = (char *)(action->offset + (int)data);
  i = 0;
  MVar3 = readToNextTag(file,pcVar7,0x400);
  if (MVar3 == MeXMLErrorNone) {
    pcVar6 = extraout_ECX;
    if (x[0] == '\0') {
LAB_000108d4:
      uVar8 = action->max;
      if (i < uVar8) {
        pcVar7 = "line %d, char %d: expected %d strings, found %d\n";
        puVar9 = (undefined *)i;
        pcVar4 = pcVar6;
LAB_000108fc:
        sprintf(file->error,pcVar7,iVar1,iVar2,uVar8,puVar9,pcVar4,pcVar6);
        return MeXMLErrorInvalidValue;
      }
    }
    else {
      pcVar4 = (char *)0x0;
      if (action->max != 0) {
        do {
          puVar9 = &DAT_00011fa7;
          pcVar6 = pcVar4;
          strtok(pcVar7,",");
          sVar5 = strlen(pcVar7);
          uVar8 = action->maxstr;
          if (uVar8 < sVar5) {
            pcVar7 = "line %d, char %d: string data greater than max of %d\n";
            goto LAB_000108fc;
          }
          strcpy(__dest,pcVar7);
          sVar5 = strlen(pcVar7);
          pcVar6 = (char *)(i + 1);
          pcVar4 = pcVar7 + sVar5;
          __dest = __dest + action->maxstr;
          pcVar7 = pcVar4 + 1;
          i = (uint)pcVar6;
          if (pcVar4[1] == '\0') goto LAB_000108d4;
        } while (pcVar6 < (char *)action->max);
      }
    }
    MVar3 = MeXMLErrorNone;
  }
  return MVar3;
}


/* ==== readToNextTag ==== */

/* WARNING: Removing unreachable block (ram,0x00010a5a) */
/* WARNING: Removing unreachable block (ram,0x00010a96) */

MeXMLError readToNextTag(MeXMLInput *fi,char *dest,int size)

{
  char cVar1;
  MeXMLError MVar2;
  char *pcVar3;
  int iVar4;
  int iVar5;
  
                    /* Unresolved local var: char c@[DW_OP_reg2(EDX)]
                       Unresolved local var: char * d@[DW_OP_reg6(ESI)] */
  pcVar3 = dest;
  if (dest == (char *)0x0) {
    iVar5 = fi->posn;
    iVar4 = fi->line;
    pcVar3 = "line %d, character %d: Nowhere to put parsed data!\n";
LAB_00010af8:
    sprintf(fi->error,pcVar3,iVar4,iVar5);
    MVar2 = MeXMLErrorParseFail;
  }
  else {
    do {
      cVar1 = XMLChomp(fi);
      iVar5 = fi->eof;
      while ((iVar5 == 0 && (cVar1 != '<'))) {
        *pcVar3 = cVar1;
        pcVar3 = pcVar3 + 1;
        if (cVar1 == '>') {
          iVar5 = fi->posn;
          iVar4 = fi->line;
          pcVar3 = "line %d, character %d: unexpected \'>\'\n";
          goto LAB_00010a24;
        }
        if (pcVar3 == dest + size + -1) {
          iVar5 = fi->posn;
          iVar4 = fi->line;
          pcVar3 = "line %d, character %d: internal error: buffer overflow (data too long)\n";
          goto LAB_00010af8;
        }
        cVar1 = XMLChomp(fi);
        iVar5 = fi->eof;
      }
      cVar1 = XMLChomp(fi);
      if (fi->eof != 0) {
        iVar5 = fi->posn;
        iVar4 = fi->line;
        pcVar3 = "line %d, character %d: found EOF inside tag\n";
        goto LAB_00010a24;
      }
      if (cVar1 != '!') {
        iVar5 = fi->top;
        if (iVar5 < 4) {
          fi->stack[iVar5] = cVar1;
          fi->top = iVar5 + 1;
        }
        if (cVar1 == '\n') {
          fi->line = fi->line + -1;
          fi->posn = 0x7fff0000;
        }
        else {
          fi->posn = fi->posn + -1;
        }
        iVar5 = fi->top;
        if (iVar5 < 4) {
          fi->stack[iVar5] = '<';
          fi->top = iVar5 + 1;
        }
        fi->posn = fi->posn + -1;
        *pcVar3 = '\0';
        return MeXMLErrorNone;
      }
      XMLChomp(fi);
LAB_000109d0:
      do {
        cVar1 = XMLChomp(fi);
        if (fi->eof == 0) {
          if (cVar1 != '-') goto LAB_000109d0;
        }
        cVar1 = XMLChomp(fi);
      } while (cVar1 != '-');
      cVar1 = XMLChomp(fi);
    } while (cVar1 == '>');
    iVar5 = fi->posn;
    iVar4 = fi->line;
    pcVar3 = "line %d, character %d: found illegal \'--\' in comment\n";
LAB_00010a24:
    sprintf(fi->error,pcVar3,iVar4,iVar5);
    MVar2 = MeXMLErrorMalformed;
  }
  return MVar2;
}


/* ==== MeXMLElementProcess ==== */

/* WARNING: Type propagation algorithm not settling */
/* WARNING: Unknown calling convention */

MeXMLError
MeXMLElementProcess(MeXMLElement *terminator,MeXMLHandler *actions,void *data,void *userdata)

{
  MeXMLInput *fi;
  MeXMLError MVar1;
  int iVar2;
  MeXMLActionType MVar3;
  MeXMLError MVar4;
  ushort **ppuVar5;
  int iVar6;
  MeXMLHandler *pMVar7;
  MeXMLError MVar8;
  char *__format;
  MeXMLElement *e;
  MeXMLError MVar9;
  int posn;
  int line;
  MeXMLElement end;
  MeXMLElement start;
  MeXMLHandler skipAction [1];
  
                    /* Unresolved local var: MeXMLInput * file@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeXMLHandler * t@[DW_OP_reg3(EBX)] */
  skipAction[0].name = (char *)0x0;
  skipAction[0].fn = (void *)0x0;
  skipAction[0].offset = 0;
  skipAction[0].max = 0;
  skipAction[0].maxstr = 0;
  skipAction[0].called = 0;
  skipAction[0].cb = (_func_MeXMLError_MeXMLElement_ptr_void_ptr_void_ptr *)0x0;
  skipAction[0].type = MeXMLActionEnd;
  fi = terminator->fi;
  MVar1 = fi->line;
  iVar2 = fi->posn;
  MVar4 = MVar1;
  do {
    do {
      MVar9 = 0x100;
      e = &start;
      MVar8 = MVar4;
      MVar4 = readNextTag(fi,e,0x100);
      if ((MVar4 == MeXMLErrorEOF) && (terminator->level == 0)) {
        return MeXMLErrorNone;
      }
      if (MVar4 != MeXMLErrorNone) {
        return MVar4;
      }
      MVar4 = (MeXMLError)(byte)start.name[0];
      if (start.name[0] == '/') {
        if (terminator->level == 0) {
          terminator = &start;
          __format = "line %d, char %d: unexpected closing tag, found %s\n";
        }
        else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
          MVar9 = strcmp(start.name + 1,terminator->name);
          if (MVar9 == MeXMLErrorNone) {
            return MeXMLErrorNone;
          }
          e = &start;
          __format = "line %d, char %d: closing tag %s doesn\'t match opening tag %s\n";
          MVar8 = MVar9;
        }
        sprintf(fi->error,__format,MVar1,iVar2,terminator,e,MVar9,MVar8);
        return MeXMLErrorMalformed;
      }
    } while (start.name[0] == '?');
    start.level = terminator->level + 1;
    start.fi = fi;
    start.attr = strchr(start.name,0x20);
    iVar6 = 0;
    pMVar7 = actions;
    if (start.attr != (char *)0x0) {
      *start.attr = '\0';
      start.attr = start.attr + 1;
      ppuVar5 = __ctype_b_loc();
      while (iVar6 = (int)*start.attr, ((*ppuVar5)[iVar6] & 0x2000) != 0) {
        start.attr = start.attr + 1;
      }
    }
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    while ((MVar3 = pMVar7->type, MVar3 != MeXMLActionEnd &&
           (iVar6 = strcmp(start.name,pMVar7->name), iVar6 != 0))) {
      pMVar7 = pMVar7 + 1;
    }
    if (MVar3 == MeXMLActionCallback) {
      pMVar7->called = 1;
      MVar4 = (*pMVar7->fn)(&start,userdata,iVar6,iVar6);
      MVar8 = MVar4;
    }
    else if (MVar3 == MeXMLActionEnd) {
      MVar4 = MeXMLElementProcess(&start,skipAction,(void *)0x0,userdata);
      MVar8 = MVar4;
    }
    else {
      pMVar7->called = 1;
      MVar4 = (*pMVar7->fn)(fi,pMVar7,data,MVar3);
      if (MVar4 != MeXMLErrorNone) {
        return MVar4;
      }
      if ((pMVar7->cb != (_func_MeXMLError_MeXMLElement_ptr_void_ptr_void_ptr *)0x0) &&
         (MVar4 = (*pMVar7->cb)(&start,data,userdata), MVar4 != MeXMLErrorNone)) {
        return MVar4;
      }
      MVar4 = readNextTag(fi,&end,0x100);
      if (MVar4 != MeXMLErrorNone) {
        return MVar4;
      }
      iVar6 = 0;
      if (end.name[0] == '/') {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
        iVar6 = strcmp(end.name + 1,start.name);
        MVar4 = MeXMLErrorNone;
        MVar8 = MeXMLErrorNone;
        if (iVar6 == 0) goto LAB_00010d0e;
      }
      MVar4 = sprintf(fi->error,"line %d, char %d: closing tag %s does not match opening tag %s",
                      MVar1,iVar2,&end,&start,iVar6,iVar6);
      MVar8 = MeXMLErrorMalformed;
    }
LAB_00010d0e:
    if (MVar8 != MeXMLErrorNone) {
      return MVar8;
    }
  } while( true );
}


/* ==== MeXMLInputSetErrorString ==== */

/* WARNING: Unknown calling convention */

void MeXMLInputSetErrorString(MeXMLInput *input,char *error,...)

{
                    /* Unresolved local var: va_list args@[???] */
  vsprintf(input->error,error,&stack0x0000000c);
  return;
}


/* ==== MeXMLHandlerWasCalled ==== */

MeBool MeXMLHandlerWasCalled(MeXMLHandler *handlers,char *name)

{
  char *pcVar1;
  int iVar2;
  
  pcVar1 = handlers->name;
  while( true ) {
    if (pcVar1 == (char *)0x0) {
      return 0;
    }
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    iVar2 = strcmp(name,handlers->name);
    if (iVar2 == 0) break;
    handlers = handlers + 1;
    pcVar1 = handlers->name;
  }
  return handlers->called;
}


/* ==== MeXMLParseComma ==== */

int MeXMLParseComma(char *in,char **out)

{
  ushort **ppuVar1;
  uint uVar2;
  
  ppuVar1 = __ctype_b_loc();
  while (((*ppuVar1)[*in] & 0x2000) != 0) {
    in = in + 1;
  }
  *out = in;
  uVar2 = 0;
  if (*in != '\0') {
    if (*in == ',') {
      ppuVar1 = __ctype_b_loc();
      do {
        in = in + 1;
      } while (((*ppuVar1)[*in] & 0x2000) != 0);
      *out = in;
      uVar2 = (uint)(*in == '\0');
    }
    else {
      uVar2 = 1;
    }
  }
  return uVar2;
}


/* ==== MeXMLParseUInt ==== */

MeXMLError MeXMLParseUInt(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  MeXMLError MVar4;
  undefined4 uVar5;
  ushort **ppuVar6;
  char *pcVar7;
  int posn;
  int line;
  char *c;
  char x [1024];
  
                    /* Unresolved local var: uint * dest@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg0(EAX)] */
  iVar1 = file->line;
  uVar2 = action->offset;
  iVar3 = file->posn;
  MVar4 = readToNextTag(file,x,0x400);
  if (MVar4 == MeXMLErrorNone) {
    uVar5 = __strtol_internal(x,&c,0,0);
    pcVar7 = c;
    *(undefined4 *)(uVar2 + (int)data) = uVar5;
    if (c == x) {
      sprintf(file->error,"line %d, char %d: Expected unsigned int, found %s\n",iVar1,iVar3,c);
    }
    else {
      ppuVar6 = __ctype_b_loc();
      while (((*ppuVar6)[*pcVar7] & 0x2000) != 0) {
        pcVar7 = pcVar7 + 1;
        c = pcVar7;
      }
      if (*pcVar7 == '\0') {
        return MeXMLErrorNone;
      }
      sprintf(file->error,"line %d, char %d: found garbage after data\n",iVar1,iVar3);
    }
    MVar4 = MeXMLErrorInvalidValue;
  }
  return MVar4;
}


/* ==== MeXMLParseInt ==== */

MeXMLError MeXMLParseInt(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  MeXMLError MVar4;
  undefined4 uVar5;
  ushort **ppuVar6;
  char *pcVar7;
  int posn;
  int line;
  char *c;
  char x [1024];
  
                    /* Unresolved local var: int * dest@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg0(EAX)] */
  iVar1 = file->line;
  uVar2 = action->offset;
  iVar3 = file->posn;
  MVar4 = readToNextTag(file,x,0x400);
  if (MVar4 == MeXMLErrorNone) {
    uVar5 = __strtol_internal(x,&c,0,0);
    pcVar7 = c;
    *(undefined4 *)(uVar2 + (int)data) = uVar5;
    if (c == x) {
      sprintf(file->error,"line %d, char %d: Expected int, found %s\n",iVar1,iVar3,c);
    }
    else {
      ppuVar6 = __ctype_b_loc();
      while (((*ppuVar6)[*pcVar7] & 0x2000) != 0) {
        pcVar7 = pcVar7 + 1;
        c = pcVar7;
      }
      if (*pcVar7 == '\0') {
        return MeXMLErrorNone;
      }
      sprintf(file->error,"line %d, char %d: found garbage after data\n",iVar1,iVar3);
    }
    MVar4 = MeXMLErrorInvalidValue;
  }
  return MVar4;
}


/* ==== MeXMLParseFloat ==== */

MeXMLError MeXMLParseFloat(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  MeXMLError MVar4;
  ushort **ppuVar5;
  char *pcVar6;
  longdouble lVar7;
  int posn;
  int line;
  char *c;
  char x [1024];
  
                    /* Unresolved local var: float * dest@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg0(EAX)] */
  iVar1 = file->line;
  uVar2 = action->offset;
  iVar3 = file->posn;
  MVar4 = readToNextTag(file,x,0x400);
  if (MVar4 == MeXMLErrorNone) {
    lVar7 = (longdouble)__strtod_internal(x,&c,0,0);
    pcVar6 = c;
    *(float *)(uVar2 + (int)data) = (float)lVar7;
    if (c == x) {
      sprintf(file->error,"line %d, char %d: Expected float, found %s\n",iVar1,iVar3,c);
    }
    else {
      ppuVar5 = __ctype_b_loc();
      while (((*ppuVar5)[*pcVar6] & 0x2000) != 0) {
        pcVar6 = pcVar6 + 1;
        c = pcVar6;
      }
      if (*pcVar6 == '\0') {
        return MeXMLErrorNone;
      }
      sprintf(file->error,"line %d, char %d: found garbage after data\n",iVar1,iVar3);
    }
    MVar4 = MeXMLErrorInvalidValue;
  }
  return MVar4;
}


/* ==== MeXMLParseMeReal ==== */

MeXMLError MeXMLParseMeReal(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  MeXMLError MVar4;
  ushort **ppuVar5;
  char *pcVar6;
  longdouble lVar7;
  int posn;
  int line;
  char *c;
  char x [1024];
  
                    /* Unresolved local var: MeReal * dest@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg0(EAX)] */
  iVar1 = file->line;
  uVar2 = action->offset;
  iVar3 = file->posn;
  MVar4 = readToNextTag(file,x,0x400);
  if (MVar4 == MeXMLErrorNone) {
    lVar7 = (longdouble)__strtod_internal(x,&c,0,0);
    pcVar6 = c;
    *(float *)(uVar2 + (int)data) = (float)lVar7;
    if (c == x) {
      sprintf(file->error,"line %d, char %d: Expected MeReal, found %s\n",iVar1,iVar3,c);
    }
    else {
      ppuVar5 = __ctype_b_loc();
      while (((*ppuVar5)[*pcVar6] & 0x2000) != 0) {
        pcVar6 = pcVar6 + 1;
        c = pcVar6;
      }
      if (*pcVar6 == '\0') {
        return MeXMLErrorNone;
      }
      sprintf(file->error,"line %d, char %d: found garbage after data\n",iVar1,iVar3);
    }
    MVar4 = MeXMLErrorInvalidValue;
  }
  return MVar4;
}


/* ==== MeXMLParseDouble ==== */

MeXMLError MeXMLParseDouble(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  MeXMLError MVar4;
  ushort **ppuVar5;
  undefined4 extraout_EDX;
  char *pcVar6;
  longdouble lVar7;
  int posn;
  int line;
  char *c;
  char x [1024];
  
                    /* Unresolved local var: double * dest@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeXMLError err@[DW_OP_reg0(EAX)] */
  iVar1 = file->line;
  uVar2 = action->offset;
  iVar3 = file->posn;
  MVar4 = readToNextTag(file,x,0x400);
  if (MVar4 == MeXMLErrorNone) {
    lVar7 = (longdouble)__strtod_internal(x,&c,0,extraout_EDX);
    pcVar6 = c;
    *(double *)(uVar2 + (int)data) = (double)lVar7;
    if (c == x) {
      sprintf(file->error,"line %d, char %d: Expected double, found %s\n",iVar1,iVar3,c);
    }
    else {
      ppuVar5 = __ctype_b_loc();
      while (((*ppuVar5)[*pcVar6] & 0x2000) != 0) {
        pcVar6 = pcVar6 + 1;
        c = pcVar6;
      }
      if (*pcVar6 == '\0') {
        return MeXMLErrorNone;
      }
      sprintf(file->error,"line %d, char %d: found garbage after data\n",iVar1,iVar3);
    }
    MVar4 = MeXMLErrorInvalidValue;
  }
  return MVar4;
}


/* ==== MeXMLParseString ==== */

MeXMLError MeXMLParseString(MeXMLInput *file,MeXMLHandler *action,void *data)

{
  MeXMLError MVar1;
  
                    /* Unresolved local var: MeXMLError err@[DW_OP_reg2(EDX)] */
  MVar1 = readToNextTag(file,(char *)(action->offset + (int)data),action->max);
  return (MVar1 == MeXMLErrorNone) - MeXMLErrorMalformed & MVar1;
}


/* ==== MeXMLInputCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeXMLInput * MeXMLInputCreate(MeStream stream)

{
  MeXMLInput *pMVar1;
  
                    /* Unresolved local var: MeXMLInput * fi@[DW_OP_reg0(EAX)] */
  pMVar1 = (MeXMLInput *)(*_MeMemoryAPI)(0x1528);
  pMVar1->bufptr = 0x1000;
  pMVar1->bufmax = 0x1000;
  pMVar1->top = 0;
  pMVar1->stream = stream;
  pMVar1->line = 1;
  pMVar1->posn = 1;
  pMVar1->eof = 0;
  builtin_strncpy(pMVar1->error,"No error found\n",0x10);
  pMVar1->userdata = (void *)0x0;
  return pMVar1;
}


/* ==== MeXMLInputDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void MeXMLInputDestroy(MeXMLInput *input)

{
  (*_DAT_00013034)(input);
  return;
}


/* ==== MeXMLInputSetUserData ==== */

void MeXMLInputSetUserData(MeXMLInput *input,void *userData)

{
  input->userdata = userData;
  return;
}


/* ==== MeXMLInputGetUserData ==== */

void * MeXMLInputGetUserData(MeXMLInput *input)

{
  return input->userdata;
}


/* ==== MeXMLInputGetErrorString ==== */

/* WARNING: Unknown calling convention */

char * MeXMLInputGetErrorString(MeXMLInput *input)

{
  return input->error;
}


/* ==== MeXMLInputProcess ==== */

MeXMLError MeXMLInputProcess(MeXMLInput *input,MeXMLHandler *handlers,void *userdata)

{
  MeXMLError MVar1;
  int iVar2;
  undefined4 *puVar3;
  MeXMLElement *pMVar4;
  MeXMLElement e;
  
  puVar3 = &DAT_00011fac;
  pMVar4 = &e;
  for (iVar2 = 0x103; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined4 *)pMVar4->name = *puVar3;
    puVar3 = puVar3 + 1;
    pMVar4 = (MeXMLElement *)(pMVar4->name + 4);
  }
  e.fi = input;
  MVar1 = MeXMLElementProcess(&e,handlers,(void *)0x0,userdata);
  return MVar1;
}


/* ==== MeXMLElementGetInput ==== */

MeXMLInput * MeXMLElementGetInput(MeXMLElement *elem)

{
  return elem->fi;
}


/* ==== MeXMLElementHandlerCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeXMLElementHandlerCreate(MeXMLHandler *handler,char *name,void *fn)

{
  size_t sVar1;
  char *__dest;
  
  sVar1 = strlen(name);
  __dest = (char *)(*_MeMemoryAPI)(sVar1 + 1);
  handler->name = __dest;
  strcpy(__dest,name);
  handler->fn = fn;
  handler->type = MeXMLActionCallback;
  handler->called = 0;
  handler->max = 0;
  handler->maxstr = 0;
  handler->offset = 0;
  return;
}


/* ==== MeXMLElementHandlerDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeXMLElementHandlerDestroy(MeXMLHandler *handler)

{
  if (handler->name != (char *)0x0) {
    (*_DAT_00013034)(handler->name);
  }
  return;
}


/* ==== XMLChomp ==== */

char XMLChomp(MeXMLInput *fi)

{
  int iVar1;
  char cVar2;
  uint uVar3;
  
                    /* Unresolved local var: char c@[DW_OP_reg2(EDX)] */
  if (fi->eof == 0) {
    iVar1 = fi->top;
    if (iVar1 == 0) {
      iVar1 = fi->bufptr;
      if (iVar1 < fi->bufmax) {
        cVar2 = fi->buffer[iVar1];
        fi->bufptr = iVar1 + 1;
      }
      else {
        fi->bufptr = 1;
        uVar3 = MeStreamRead(fi,1,0x1000,fi->stream);
        fi->bufmax = uVar3;
        if (uVar3 == 0) {
          cVar2 = '\0';
          fi->eof = 1;
        }
        else {
          cVar2 = fi->buffer[0];
        }
      }
    }
    else {
      fi->top = iVar1 + -1;
      cVar2 = fi->stack[iVar1 + -1];
    }
    if (cVar2 == '\n') {
      fi->line = fi->line + 1;
      fi->posn = 1;
    }
    else {
      fi->posn = fi->posn + 1;
    }
  }
  else {
    cVar2 = '\0';
  }
  return cVar2;
}


/* ==== readNextTag ==== */

MeXMLError readNextTag(MeXMLInput *fi,MeXMLElement *e,int size)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  int iVar4;
  
                    /* Unresolved local var: char c@[DW_OP_reg2(EDX)]
                       Unresolved local var: char * dest@[DW_OP_reg6(ESI)] */
  if (e == (MeXMLElement *)0x0) {
    iVar4 = fi->posn;
    iVar3 = fi->line;
    pcVar2 = "line %d, character %d: Nowhere to put parsed data!\n";
LAB_0001183c:
    sprintf(fi->error,pcVar2,iVar3,iVar4);
    return MeXMLErrorParseFail;
  }
LAB_00011700:
  do {
    do {
      cVar1 = XMLChomp(fi);
      if (fi->eof != 0) {
        return MeXMLErrorEOF;
      }
    } while (cVar1 != '<');
    cVar1 = XMLChomp(fi);
    if (cVar1 != '!') {
      pcVar2 = e->name;
      goto LAB_000117e9;
    }
    cVar1 = XMLChomp(fi);
    if (cVar1 != '-') {
      do {
        cVar1 = XMLChomp(fi);
        if (fi->eof != 0) break;
      } while (cVar1 != '>');
      goto LAB_00011700;
    }
    do {
      cVar1 = XMLChomp(fi);
      if (fi->eof != 0) {
        iVar4 = fi->posn;
        iVar3 = fi->line;
        pcVar2 = "line %d, character %d: found EOF in comment\n";
        goto LAB_000117b4;
      }
    } while ((cVar1 != '-') || (cVar1 = XMLChomp(fi), cVar1 != '-'));
    cVar1 = XMLChomp(fi);
  } while (cVar1 == '>');
  iVar4 = fi->posn;
  iVar3 = fi->line;
  pcVar2 = "line %d, character %d: found illegal \'--\' in comment\n";
LAB_000117b4:
  sprintf(fi->error,pcVar2,iVar3,iVar4);
  return MeXMLErrorMalformed;
LAB_000117e9:
  e->name[0] = cVar1;
  e = (MeXMLElement *)(e->name + 1);
  if (e == (MeXMLElement *)(pcVar2 + size + -1)) {
    iVar4 = fi->posn;
    iVar3 = fi->line;
    pcVar2 = "line %d, character %d: internal error: buffer overflow (tag too long)\n";
    goto LAB_0001183c;
  }
  cVar1 = XMLChomp(fi);
  if (fi->eof != 0) {
    iVar4 = fi->posn;
    iVar3 = fi->line;
    pcVar2 = "line %d, character %d: found EOF inside tag\n";
    goto LAB_000117b4;
  }
  if (cVar1 == '>') {
    e->name[0] = '\0';
    return MeXMLErrorNone;
  }
  goto LAB_000117e9;
}


