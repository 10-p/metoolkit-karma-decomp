/* ==== McdError ==== */

void McdError(McdErrorDescription *ErrorList,int errorCode,char *message,char *fn,char *file,
             int line)

{
  short sVar1;
  void *pvVar2;
  char *pcVar3;
  char msg [512];
  
                    /* Unresolved local var: McdErrorDescription * desc@[???]
                       Unresolved local var: char * desc_msg@[DW_OP_reg2(EDX)] */
                    /* Unresolved local var: McdErrorDescription * desc@[DW_OP_reg3(EBX)] */
  sVar1 = ErrorList->m_errNum;
  while ((sVar1 != 0x26 && (sVar1 != errorCode))) {
    ErrorList = ErrorList + 1;
    sVar1 = ErrorList->m_errNum;
  }
  msg[0] = '\0';
  pvVar2 = memset(msg + 1,0,0x1ff);
  pcVar3 = "Undefined Internal Error";
  if (errorCode - 1U < 0x25) {
    pcVar3 = ErrorList->m_description;
  }
  if (0 < ErrorList->m_errorCount) {
    sprintf(msg,"Mcd function %s (in file %s at line %d): %s%s",fn,file,line,pcVar3,message,pvVar2);
    if (ErrorList->m_errorCount == 1) {
      strcat(msg,"[This message will not be repeated again!]\n");
    }
    sVar1 = ErrorList->m_errorLevel;
    if (sVar1 == 1) {
      MeWarning(0,msg);
    }
    else if (sVar1 < 2) {
      if (sVar1 == 0) {
        MeFatalError(0,msg);
                    /* WARNING: Subroutine does not return */
        abort();
      }
    }
    else if (sVar1 == 2) {
      MeInfo(0,msg);
    }
  }
  return;
}


