/* ==== MeInfo ==== */

/* WARNING: Unknown calling convention */

void MeInfo(int level,char *format,...)

{
                    /* Unresolved local var: va_list ap@[???] */
  (*MeInfoHandler)(level,format,&stack0x0000000c);
  return;
}


/* ==== MeLog ==== */

/* WARNING: Unknown calling convention */

void MeLog(int level,char *format,...)

{
                    /* Unresolved local var: va_list ap@[???] */
  (*MeLogHandler)(level,format,&stack0x0000000c);
  return;
}


/* ==== MeWarning ==== */

/* WARNING: Unknown calling convention */

void MeWarning(int level,char *format,...)

{
                    /* Unresolved local var: va_list ap@[???] */
  (*MeWarningHandler)(level,format,&stack0x0000000c);
  return;
}


/* ==== MeFatalError ==== */

/* WARNING: Unknown calling convention */

void MeFatalError(int level,char *format,...)

{
                    /* Unresolved local var: va_list ap@[???] */
  (*MeFatalErrorHandler)(level,format,&stack0x0000000c);
  return;
}


/* ==== MeDebug ==== */

/* WARNING: Unknown calling convention */

void MeDebug(int level,char *format,...)

{
                    /* Unresolved local var: va_list ap@[???] */
  (*MeDebugHandler)(level,format,&stack0x0000000c);
  return;
}


/* ==== MeDebugF ==== */

/* WARNING: Unknown calling convention */

void MeDebugF(FILE *f,char *format,...)

{
                    /* Unresolved local var: va_list ap@[???] */
  (*MeDebugHandler)(1,format,&stack0x0000000c);
  return;
}


/* ==== MeShowStdout ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void MeShowStdout(int level,char *string)

{
  fputs(string,_stdout);
  fputc(10,_stderr);
  return;
}


/* ==== MeShowStderr ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void MeShowStderr(int level,char *string)

{
  fputs(string,_stderr);
  fputc(10,_stderr);
  return;
}


/* ==== MeHandlerInfo ==== */

void MeHandlerInfo(int level,char *format,va_list ap)

{
  char message [5000];
  char format2 [5000];
  
  if (level <= MeInfoLevel) {
    snprintf(format2,5000,"MeInfo{%d}: %s",level,format);
    vsnprintf(message,5000,format2,ap);
    (*MeInfoShow)(level,message);
  }
  return;
}


/* ==== MeHandlerLog ==== */

void MeHandlerLog(int level,char *format,va_list ap)

{
  char message [5000];
  char format2 [5000];
  
  if (level <= MeInfoLevel) {
    snprintf(format2,5000,"MeLog{%d}: %s",level,format);
    vsnprintf(message,5000,format2,ap);
    (*MeLogShow)(level,message);
  }
  return;
}


/* ==== MeHandlerWarning ==== */

void MeHandlerWarning(int level,char *format,va_list ap)

{
  char message [5000];
  char format2 [5000];
  
  if (level <= MeWarningLevel) {
    snprintf(format2,5000,"MeWarning{%d}: %s",level,format);
    vsnprintf(message,5000,format2,ap);
    (*MeWarningShow)(level,message);
  }
  return;
}


/* ==== MeHandlerFatalError ==== */

void MeHandlerFatalError(int level,char *format,va_list ap)

{
  char message [5000];
  char format2 [5000];
  
  snprintf(format2,5000,"MeFatalError{%d}: %s",level,format);
  vsnprintf(message,5000,format2,ap);
  (*MeFatalErrorShow)(level,message);
                    /* WARNING: Subroutine does not return */
  exit(1);
}


/* ==== MeHandlerDebug ==== */

void MeHandlerDebug(int level,char *format,va_list ap)

{
  char message [5000];
  char format2 [5000];
  
  if (level <= MeDebugLevel) {
    snprintf(format2,5000,"MeDebug{%d}: %s",level,format);
    vsnprintf(message,5000,format,ap);
    (*MeDebugShow)(level,message);
  }
  return;
}


/* ==== MeSetInfoShow ==== */

MeShow MeSetInfoShow(MeShow n)

{
  MeShow p_Var1;
  
                    /* Unresolved local var: MeShow current@[DW_OP_reg0(EAX)] */
  p_Var1 = MeInfoShow;
  MeInfoShow = n;
  return p_Var1;
}


/* ==== MeSetLogShow ==== */

MeShow MeSetLogShow(MeShow n)

{
  MeShow p_Var1;
  
                    /* Unresolved local var: MeShow current@[DW_OP_reg0(EAX)] */
  p_Var1 = MeLogShow;
  MeLogShow = n;
  return p_Var1;
}


/* ==== MeSetWarningShow ==== */

MeShow MeSetWarningShow(MeShow n)

{
  MeShow p_Var1;
  
                    /* Unresolved local var: MeShow current@[DW_OP_reg0(EAX)] */
  p_Var1 = MeWarningShow;
  MeWarningShow = n;
  return p_Var1;
}


/* ==== MeSetFatalErrorShow ==== */

MeShow MeSetFatalErrorShow(MeShow n)

{
  MeShow p_Var1;
  
                    /* Unresolved local var: MeShow current@[DW_OP_reg0(EAX)] */
  p_Var1 = MeFatalErrorShow;
  MeFatalErrorShow = n;
  return p_Var1;
}


/* ==== MeSetDebugShow ==== */

MeShow MeSetDebugShow(MeShow n)

{
  MeShow p_Var1;
  
                    /* Unresolved local var: MeShow current@[DW_OP_reg0(EAX)] */
  p_Var1 = MeDebugShow;
  MeDebugShow = n;
  return p_Var1;
}


/* ==== MeSetInfoHandler ==== */

MeHandler MeSetInfoHandler(MeHandler n)

{
  MeHandler p_Var1;
  
                    /* Unresolved local var: MeHandler current@[DW_OP_reg0(EAX)] */
  p_Var1 = MeInfoHandler;
  MeInfoHandler = n;
  return p_Var1;
}


/* ==== MeSetLogHandler ==== */

MeHandler MeSetLogHandler(MeHandler n)

{
  MeHandler p_Var1;
  
                    /* Unresolved local var: MeHandler current@[DW_OP_reg0(EAX)] */
  p_Var1 = MeLogHandler;
  MeLogHandler = n;
  return p_Var1;
}


/* ==== MeSetWarningHandler ==== */

MeHandler MeSetWarningHandler(MeHandler n)

{
  MeHandler p_Var1;
  
                    /* Unresolved local var: MeHandler current@[DW_OP_reg0(EAX)] */
  p_Var1 = MeWarningHandler;
  MeWarningHandler = n;
  return p_Var1;
}


/* ==== MeSetFatalErrorHandler ==== */

MeHandler MeSetFatalErrorHandler(MeHandler n)

{
  MeHandler p_Var1;
  
                    /* Unresolved local var: MeHandler current@[DW_OP_reg0(EAX)] */
  p_Var1 = MeFatalErrorHandler;
  MeFatalErrorHandler = n;
  return p_Var1;
}


/* ==== MeSetDebugHandler ==== */

MeHandler MeSetDebugHandler(MeHandler n)

{
  MeHandler p_Var1;
  
                    /* Unresolved local var: MeHandler current@[DW_OP_reg0(EAX)] */
  p_Var1 = MeDebugHandler;
  MeDebugHandler = n;
  return p_Var1;
}


/* ==== MeShortenPath ==== */

char * MeShortenPath(char *path)

{
  char *pcVar1;
  
                    /* Unresolved local var: char * ptr@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * ptr2@[DW_OP_reg3(EBX)]
                       Unresolved local var: int delimiter@[???] */
  pcVar1 = strchr(path,0x2f);
  while (pcVar1 != (char *)0x0) {
    path = pcVar1 + 1;
    pcVar1 = strchr(path,0x2f);
  }
  return path;
}


