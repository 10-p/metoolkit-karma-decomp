/* ==== MeCommandLineOptionsGetPos ==== */

int MeCommandLineOptionsGetPos(MeCommandLineOptions *options,char *arg)

{
  int iVar1;
  char **ppcVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
  MeBool found;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  bVar3 = false;
  if (options == (MeCommandLineOptions *)0x0) {
    MeFatalError(0,"MeCommandLineOptionsGetPos: You must pass in a valid MeCommandLineOptions*");
  }
  iVar5 = 0;
  iVar1 = options->m_argc;
  if (iVar1 == 0) {
LAB_00010056:
    if (!bVar3) {
      return -1;
    }
  }
  else {
    ppcVar2 = options->p_argv;
    do {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
      iVar4 = strcmp(arg,ppcVar2[iVar5]);
      if (iVar4 == 0) {
        bVar3 = true;
      }
      iVar5 = iVar5 + 1;
      if (iVar5 == iVar1) goto LAB_00010056;
    } while (!bVar3);
  }
  return iVar5 + -1;
}


/* ==== MeCommandLineOptionsGetNumeric ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int MeCommandLineOptionsGetNumeric(MeCommandLineOptions *options,char *arg,MeBool eat)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int newargc;
  int newargc_2;
  int newargc_1;
  int checker;
  int eats [2];
  
                    /* Unresolved local var: int parameter@[???] */
  if (options == (MeCommandLineOptions *)0x0) {
    MeFatalError(0,"MeCommandLineOptionsGetNumeric: You must pass in a valid MeCommandLineOptions*")
    ;
  }
  eats[0] = MeCommandLineOptionsGetPos(options,arg);
  if (eats[0] == -1) {
    return 0;
  }
  if (eats[0] < options->m_argc + -1) {
    iVar3 = __strtol_internal(options->p_argv[eats[0] + 1],0,10,0);
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???]
                       Unresolved local var: uchar * __s1@[DW_OP_reg0(EAX)]
                       Unresolved local var: int __result@[???] */
    if ((iVar3 != 0) ||
       ((*options->p_argv[eats[0] + 1] == '0' && (options->p_argv[eats[0] + 1][1] == '\0')))) {
      if (eat == 0) {
        return iVar3;
      }
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int b_eat@[DW_OP_reg2(EDX)] */
      iVar2 = 0;
      eats[1] = eats[0] + 1;
      newargc_1 = 0;
      iVar5 = options->m_argc;
      if (iVar5 != 0) {
        do {
                    /* Unresolved local var: int j@[DW_OP_reg0(EAX)] */
          bVar1 = false;
          iVar4 = 0;
          do {
            if (eats[iVar4] == iVar2) {
              bVar1 = true;
            }
            iVar4 = iVar4 + 1;
            if (iVar4 == 2) {
              if (!bVar1) {
                options->p_argv[newargc_1] = options->p_argv[iVar2];
                newargc_1 = newargc_1 + 1;
                iVar5 = options->m_argc;
              }
              break;
            }
          } while (!bVar1);
          iVar2 = iVar2 + 1;
        } while (iVar2 != iVar5);
      }
      options->m_argc = iVar5 + -2;
      return iVar3;
    }
    if (eat == 0) goto LAB_000100ed;
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int b_eat@[DW_OP_reg2(EDX)] */
    newargc_2 = 0;
    iVar3 = options->m_argc;
    iVar5 = 0;
    if (iVar3 != 0) {
      do {
                    /* Unresolved local var: int j@[DW_OP_reg0(EAX)] */
        bVar1 = false;
        iVar2 = 0;
        do {
          if (eats[iVar2 + -1] == iVar5) {
            bVar1 = true;
          }
          iVar2 = iVar2 + 1;
          if (iVar2 == 1) {
            if (!bVar1) {
              options->p_argv[newargc_2] = options->p_argv[iVar5];
              newargc_2 = newargc_2 + 1;
              iVar3 = options->m_argc;
            }
            break;
          }
        } while (!bVar1);
        iVar5 = iVar5 + 1;
      } while (iVar5 != iVar3);
    }
  }
  else {
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int b_eat@[DW_OP_reg2(EDX)] */
    newargc = 0;
    iVar3 = options->m_argc;
    if (options == (MeCommandLineOptions *)0x0) {
      MeFatalError(0,"MeCommandLineOptionsEat: You must pass in a valid MeCommandLineOptions*");
      iVar3 = _DAT_00000000;
    }
    iVar5 = 0;
    if (iVar3 != 0) {
      do {
                    /* Unresolved local var: int j@[DW_OP_reg0(EAX)] */
        bVar1 = false;
        iVar2 = 0;
        do {
          if (eats[iVar2 + -1] == iVar5) {
            bVar1 = true;
          }
          iVar2 = iVar2 + 1;
          if (iVar2 == 1) {
            if (!bVar1) {
              options->p_argv[newargc] = options->p_argv[iVar5];
              newargc = newargc + 1;
              iVar3 = options->m_argc;
            }
            break;
          }
        } while (!bVar1);
        iVar5 = iVar5 + 1;
      } while (iVar5 != iVar3);
    }
  }
  options->m_argc = iVar3 + -1;
LAB_000100ed:
  MeInfo(0,"Numerical parameter that should follow %s is missing! Defaulting to 0.");
  return 0;
}


/* ==== MeCommandLineOptionsGetFloat ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double MeCommandLineOptionsGetFloat(MeCommandLineOptions *options,char *arg,MeBool eat)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  longdouble lVar5;
  int newargc;
  int newargc_2;
  int newargc_1;
  int checker;
  int eats [2];
  
                    /* Unresolved local var: double parameter@[???] */
  if (options == (MeCommandLineOptions *)0x0) {
    MeFatalError(0,"MeCommandLineOptionsGetFloat: You must pass in a valid MeCommandLineOptions*");
  }
  eats[0] = MeCommandLineOptionsGetPos(options,arg);
  if (eats[0] != -1) {
    iVar4 = options->m_argc + -1;
    if (eats[0] < iVar4) {
      lVar5 = (longdouble)__strtod_internal(options->p_argv[eats[0] + 1],0,0,iVar4);
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???]
                       Unresolved local var: uchar * __s1@[DW_OP_reg0(EAX)]
                       Unresolved local var: int __result@[???] */
      if ((lVar5 != (longdouble)0.0) ||
         ((*options->p_argv[eats[0] + 1] == '0' && (options->p_argv[eats[0] + 1][1] == '\0')))) {
        if (eat != 0) {
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int b_eat@[DW_OP_reg2(EDX)] */
          iVar3 = 0;
          eats[1] = eats[0] + 1;
          newargc_1 = 0;
          iVar4 = options->m_argc;
          if (iVar4 != 0) {
            do {
                    /* Unresolved local var: int j@[DW_OP_reg0(EAX)] */
              bVar1 = false;
              iVar2 = 0;
              do {
                if (eats[iVar2] == iVar3) {
                  bVar1 = true;
                }
                iVar2 = iVar2 + 1;
                if (iVar2 == 2) {
                  if (!bVar1) {
                    options->p_argv[newargc_1] = options->p_argv[iVar3];
                    newargc_1 = newargc_1 + 1;
                    iVar4 = options->m_argc;
                  }
                  break;
                }
              } while (!bVar1);
              iVar3 = iVar3 + 1;
            } while (iVar3 != iVar4);
          }
          options->m_argc = iVar4 + -2;
        }
        goto LAB_000102d2;
      }
      if (eat != 0) {
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int b_eat@[DW_OP_reg2(EDX)] */
        newargc_2 = 0;
        iVar3 = 0;
        iVar4 = options->m_argc;
        if (iVar4 != 0) {
          do {
                    /* Unresolved local var: int j@[DW_OP_reg0(EAX)] */
            bVar1 = false;
            iVar2 = 0;
            do {
              if (eats[iVar2 + -1] == iVar3) {
                bVar1 = true;
              }
              iVar2 = iVar2 + 1;
              if (iVar2 == 1) {
                if (!bVar1) {
                  options->p_argv[newargc_2] = options->p_argv[iVar3];
                  newargc_2 = newargc_2 + 1;
                  iVar4 = options->m_argc;
                }
                break;
              }
            } while (!bVar1);
            iVar3 = iVar3 + 1;
          } while (iVar3 != iVar4);
        }
        goto LAB_000102ba;
      }
    }
    else {
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int b_eat@[DW_OP_reg2(EDX)] */
      newargc = 0;
      iVar4 = options->m_argc;
      if (options == (MeCommandLineOptions *)0x0) {
        MeFatalError(0,"MeCommandLineOptionsEat: You must pass in a valid MeCommandLineOptions*");
        iVar4 = _DAT_00000000;
      }
      iVar3 = 0;
      if (iVar4 != 0) {
        do {
                    /* Unresolved local var: int j@[DW_OP_reg0(EAX)] */
          bVar1 = false;
          iVar2 = 0;
          do {
            if (eats[iVar2 + -1] == iVar3) {
              bVar1 = true;
            }
            iVar2 = iVar2 + 1;
            if (iVar2 == 1) {
              if (!bVar1) {
                options->p_argv[newargc] = options->p_argv[iVar3];
                newargc = newargc + 1;
                iVar4 = options->m_argc;
              }
              break;
            }
          } while (!bVar1);
          iVar3 = iVar3 + 1;
        } while (iVar3 != iVar4);
      }
LAB_000102ba:
      options->m_argc = iVar4 + -1;
    }
    MeInfo(0,"Floating point parameter that should follow %s is missing! Defaulting to 0.");
  }
  lVar5 = (longdouble)0;
LAB_000102d2:
  return (double)lVar5;
}


/* ==== MeCommandLineOptionsGetString ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

char * MeCommandLineOptionsGetString(MeCommandLineOptions *options,char *arg,MeBool eat)

{
  char *pcVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int newargc_2;
  int newargc_1;
  int newargc;
  char *parameter;
  int checker;
  int eats [2];
  
  if (options == (MeCommandLineOptions *)0x0) {
    MeFatalError(0,"MeCommandLineOptionsGetNumeric: You must pass in a valid MeCommandLineOptions*")
    ;
  }
  eats[0] = MeCommandLineOptionsGetPos(options,arg);
  if (eats[0] == -1) {
    return (char *)0x0;
  }
  iVar5 = options->m_argc;
  if (eats[0] < iVar5 + -1) {
    pcVar1 = options->p_argv[eats[0] + 1];
    if (*pcVar1 != '-') {
      if (eat == 0) {
        return pcVar1;
      }
      eats[1] = eats[0] + 1;
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int b_eat@[DW_OP_reg2(EDX)] */
      iVar4 = 0;
      newargc = 0;
      if (iVar5 != 0) {
        do {
                    /* Unresolved local var: int j@[DW_OP_reg0(EAX)] */
          bVar2 = false;
          iVar3 = 0;
          do {
            if (eats[iVar3] == iVar4) {
              bVar2 = true;
            }
            iVar3 = iVar3 + 1;
            if (iVar3 == 2) {
              if (!bVar2) {
                options->p_argv[newargc] = options->p_argv[iVar4];
                newargc = newargc + 1;
                iVar5 = options->m_argc;
              }
              break;
            }
          } while (!bVar2);
          iVar4 = iVar4 + 1;
        } while (iVar4 != iVar5);
      }
      options->m_argc = iVar5 + -2;
      return pcVar1;
    }
    if (eat == 0) goto LAB_00010519;
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int b_eat@[DW_OP_reg2(EDX)] */
    iVar4 = 0;
    newargc_1 = 0;
    if (iVar5 != 0) {
      do {
                    /* Unresolved local var: int j@[DW_OP_reg0(EAX)] */
        bVar2 = false;
        iVar3 = 0;
        do {
          if (eats[iVar3 + -1] == iVar4) {
            bVar2 = true;
          }
          iVar3 = iVar3 + 1;
          if (iVar3 == 1) {
            if (!bVar2) {
              options->p_argv[newargc_1] = options->p_argv[iVar4];
              newargc_1 = newargc_1 + 1;
              iVar5 = options->m_argc;
            }
            break;
          }
        } while (!bVar2);
        iVar4 = iVar4 + 1;
      } while (iVar4 != iVar5);
    }
  }
  else {
    newargc_2 = 0;
    if (options == (MeCommandLineOptions *)0x0) {
      MeFatalError(0,"MeCommandLineOptionsEat: You must pass in a valid MeCommandLineOptions*");
      iVar5 = _DAT_00000000;
    }
    iVar4 = 0;
    if (iVar5 != 0) {
      do {
                    /* Unresolved local var: int j@[DW_OP_reg0(EAX)] */
        bVar2 = false;
        iVar3 = 0;
        do {
          if (eats[iVar3 + -1] == iVar4) {
            bVar2 = true;
          }
          iVar3 = iVar3 + 1;
          if (iVar3 == 1) {
            if (!bVar2) {
              options->p_argv[newargc_2] = options->p_argv[iVar4];
              newargc_2 = newargc_2 + 1;
              iVar5 = options->m_argc;
            }
            break;
          }
        } while (!bVar2);
        iVar4 = iVar4 + 1;
      } while (iVar4 != iVar5);
    }
  }
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int b_eat@[DW_OP_reg2(EDX)] */
  options->m_argc = iVar5 + -1;
LAB_00010519:
  MeInfo(0,"String parameter that should follow %s is missing! Returning null pointer.");
  MeInfo(0,"String parameters aren\'t allowed to start with `-\'.");
  return (char *)0x0;
}


/* ==== MeCommandLineOptionsCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeCommandLineOptions * MeCommandLineOptionsCreate(int argc,char **argv)

{
  int iVar1;
  MeCommandLineOptions *pMVar2;
  char **ppcVar3;
  uint uVar4;
  int iVar5;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeCommandLineOptions * options@[DW_OP_reg6(ESI)] */
  pMVar2 = (*_MeMemoryAPI)(8);
  if (pMVar2 == (MeCommandLineOptions *)0x0) {
    MeFatalError(0,"Unable to allocate memory for command line options");
  }
  pMVar2->m_argc = argc;
  ppcVar3 = (*_MeMemoryAPI)(argc * 4 + 4);
  pMVar2->p_argv = ppcVar3;
  if (ppcVar3 == (char **)0x0) {
    MeFatalError(0,"Unable to allocate memory for command line options");
  }
  iVar5 = 0;
  if (argc != 0) {
    uVar4 = argc & 3;
    if (uVar4 != 0) {
      if (1 < uVar4) {
        if (2 < uVar4) {
          *pMVar2->p_argv = *argv;
        }
        uVar4 = (uint)(2 < uVar4);
        pMVar2->p_argv[uVar4] = argv[uVar4];
        iVar5 = uVar4 + 1;
      }
      pMVar2->p_argv[iVar5] = argv[iVar5];
      iVar5 = iVar5 + 1;
      if (iVar5 == argc) {
        return pMVar2;
      }
    }
    do {
      pMVar2->p_argv[iVar5] = argv[iVar5];
      pMVar2->p_argv[iVar5 + 1] = argv[iVar5 + 1];
      pMVar2->p_argv[iVar5 + 2] = argv[iVar5 + 2];
      iVar1 = iVar5 + 3;
      iVar5 = iVar5 + 4;
      pMVar2->p_argv[iVar1] = argv[iVar1];
    } while (iVar5 != argc);
  }
  return pMVar2;
}


/* ==== MeCommandLineOptionsDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeCommandLineOptionsDestroy(MeCommandLineOptions *options)

{
  if (options == (MeCommandLineOptions *)0x0) {
    MeFatalError(0,"MeCommandLineOptionsDestroy: You must pass in a valid MeCommandLineOptions*");
  }
  (*_DAT_00011020)(options->p_argv);
  (*_DAT_00011020)(options);
  return;
}


/* ==== MeCommandLineOptionsEat ==== */

void MeCommandLineOptionsEat(MeCommandLineOptions *options,int *eat,int num_eat)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int newargc;
  
                    /* Unresolved local var: int i@[DW_OP_reg1(ECX)]
                       Unresolved local var: int b_eat@[DW_OP_reg2(EDX)] */
  newargc = 0;
  if (options == (MeCommandLineOptions *)0x0) {
    MeFatalError(0,"MeCommandLineOptionsEat: You must pass in a valid MeCommandLineOptions*");
  }
  if (num_eat != 0) {
    iVar3 = 0;
    iVar4 = options->m_argc;
    if (iVar4 != 0) {
      do {
                    /* Unresolved local var: int j@[DW_OP_reg0(EAX)] */
        bVar1 = false;
        iVar2 = 0;
        if (num_eat == 0) {
LAB_0001077a:
          if (!bVar1) {
            options->p_argv[newargc] = options->p_argv[iVar3];
            newargc = newargc + 1;
            iVar4 = options->m_argc;
          }
        }
        else {
          do {
            if (eat[iVar2] == iVar3) {
              bVar1 = true;
            }
            iVar2 = iVar2 + 1;
            if (iVar2 == num_eat) goto LAB_0001077a;
          } while (!bVar1);
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 != iVar4);
    }
    options->m_argc = iVar4 - num_eat;
  }
  return;
}


/* ==== MeCommandLineOptionsCheckFor ==== */

MeBool MeCommandLineOptionsCheckFor(MeCommandLineOptions *options,char *arg,MeBool eat)

{
  bool bVar1;
  int iVar2;
  MeBool MVar3;
  int iVar4;
  int iVar5;
  int newargc;
  int i;
  
  if (options == (MeCommandLineOptions *)0x0) {
    MeFatalError(0,"MeCommandLineOptionsGetPos: You must pass in a valid MeCommandLineOptions*");
  }
  if (arg == (char *)0x0) {
    MeFatalError(0,"MeCommandLineOptionsGetPos: You must pass in a valid char*");
  }
  i = MeCommandLineOptionsGetPos(options,arg);
  MVar3 = 0;
  if (i != -1) {
    if (eat != 0) {
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int b_eat@[DW_OP_reg1(ECX)] */
      newargc = 0;
      if (options == (MeCommandLineOptions *)0x0) {
        MeFatalError(0,"MeCommandLineOptionsEat: You must pass in a valid MeCommandLineOptions*");
      }
      iVar2 = options->m_argc;
      iVar5 = 0;
      if (iVar2 != 0) {
        do {
                    /* Unresolved local var: int j@[DW_OP_reg2(EDX)] */
          bVar1 = false;
          iVar4 = 0;
          do {
            if ((&i)[iVar4] == iVar5) {
              bVar1 = true;
            }
            iVar4 = iVar4 + 1;
            if (iVar4 == 1) {
              if (!bVar1) {
                options->p_argv[newargc] = options->p_argv[iVar5];
                newargc = newargc + 1;
                iVar2 = options->m_argc;
              }
              break;
            }
          } while (!bVar1);
          iVar5 = iVar5 + 1;
        } while (iVar5 != iVar2);
      }
      options->m_argc = iVar2 + -1;
    }
    MVar3 = 1;
  }
  return MVar3;
}


/* ==== MeCommandLineOptionsCheckForList ==== */

MeBool MeCommandLineOptionsCheckForList(MeCommandLineOptions *options,char **arglist,MeBool eat)

{
  char *pcVar1;
  MeBool MVar2;
  int iVar3;
  MeBool found;
  
                    /* Unresolved local var: int j@[DW_OP_reg3(EBX)] */
  found = 0;
  if (options == (MeCommandLineOptions *)0x0) {
    MeFatalError(0,
                 "MeCommandLineOptionsCheckForList: You must pass in a valid MeCommandLineOptions*")
    ;
  }
  if (arglist == (char **)0x0) {
    MeFatalError(0,"MeCommandLineOptionsCheckForList: You must pass in a valid char*[]");
  }
  iVar3 = 0;
  pcVar1 = *arglist;
  while (pcVar1 != (char *)0x0) {
    MVar2 = MeCommandLineOptionsCheckFor(options,arglist[iVar3],eat);
    if (MVar2 != 0) {
      found = 1;
    }
    iVar3 = iVar3 + 1;
    pcVar1 = arglist[iVar3];
  }
  return found;
}


