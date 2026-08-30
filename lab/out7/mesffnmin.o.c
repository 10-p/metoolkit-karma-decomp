/* ==== f1d ==== */

MeReal f1d(MeReal x)

{
  float *pfVar1;
  MeReal *pMVar2;
  float fVar3;
  MeReal *pMVar4;
  MeReal *pMVar5;
  MeReal *pMVar6;
  uint uVar7;
  longdouble lVar8;
  MeReal temp [4];
  
                    /* Unresolved local var: MeReal * b@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg3(EBX)] */
  pMVar5 = xicom;
  pMVar2 = temp + Ncom;
  if (temp < pMVar2) {
    uVar7 = (int)pMVar2 - (int)temp & 0xf;
    pMVar4 = temp;
    if (temp + 1 < pMVar2) {
      pMVar6 = temp;
      if (uVar7 != 0) {
        if (4 < uVar7) {
          if (8 < uVar7) {
            if (0xc < uVar7) goto LAB_00010050;
            pMVar5 = xicom + 1;
            temp[0] = x * xicom[0];
            pMVar4 = temp + 1;
          }
          fVar3 = *pMVar5;
          pMVar5 = pMVar5 + 1;
          *pMVar4 = x * fVar3;
          pMVar4 = pMVar4 + 1;
        }
        goto LAB_0001003c;
      }
    }
    else {
LAB_0001003c:
      fVar3 = *pMVar5;
      pMVar5 = pMVar5 + 1;
      *pMVar4 = x * fVar3;
      pMVar6 = pMVar4 + 1;
      if (pMVar2 <= pMVar4 + 1) goto LAB_0001007a;
    }
LAB_00010050:
    do {
      *pMVar6 = x * *pMVar5;
      pMVar6[1] = x * pMVar5[1];
      pMVar6[2] = x * pMVar5[2];
      pfVar1 = pMVar5 + 3;
      pMVar5 = pMVar5 + 4;
      pMVar6[3] = x * *pfVar1;
      pMVar6 = pMVar6 + 4;
    } while (pMVar6 < pMVar2);
  }
LAB_0001007a:
                    /* Unresolved local var: MeReal * b@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg1(ECX)] */
  pMVar5 = pcom;
  pMVar2 = temp + Ncom;
  if (pMVar2 <= temp) goto LAB_000100db;
  uVar7 = (int)pMVar2 - (int)temp & 0xf;
  pMVar4 = temp;
  if (temp + 1 < pMVar2) {
    if (uVar7 != 0) {
      if (4 < uVar7) {
        if (8 < uVar7) {
          if (0xc < uVar7) goto LAB_000100b0;
          pMVar5 = pcom + 1;
          temp[0] = pcom[0] + temp[0];
          pMVar4 = temp + 1;
        }
        fVar3 = *pMVar5;
        pMVar5 = pMVar5 + 1;
        *pMVar4 = fVar3 + *pMVar4;
        pMVar4 = pMVar4 + 1;
      }
      goto LAB_00010099;
    }
  }
  else {
LAB_00010099:
    fVar3 = *pMVar5;
    pMVar5 = pMVar5 + 1;
    *pMVar4 = fVar3 + *pMVar4;
    pMVar4 = pMVar4 + 1;
    if (pMVar2 <= pMVar4) goto LAB_000100db;
  }
LAB_000100b0:
  do {
    *pMVar4 = *pMVar5 + *pMVar4;
    pMVar4[1] = pMVar5[1] + pMVar4[1];
    pMVar4[2] = pMVar5[2] + pMVar4[2];
    pfVar1 = pMVar5 + 3;
    pMVar5 = pMVar5 + 4;
    pMVar4[3] = *pfVar1 + pMVar4[3];
    pMVar4 = pMVar4 + 4;
  } while (pMVar4 < pMVar2);
LAB_000100db:
  lVar8 = (longdouble)(*nrfunc)(temp);
  return (MeReal)lVar8;
}


/* ==== df1d ==== */

MeReal df1d(MeReal x)

{
  float *pfVar1;
  float *pfVar2;
  MeReal *pMVar3;
  float *pfVar4;
  MeReal *pMVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  MeReal *pMVar9;
  MeReal *pMVar10;
  MeReal *pMVar11;
  MeReal *pMVar12;
  uint uVar13;
  MeReal temp2 [4];
  MeReal temp [4];
  
                    /* Unresolved local var: MeReal * b@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg3(EBX)] */
  pMVar9 = xicom;
  pMVar11 = temp + Ncom;
  if (temp < pMVar11) {
    uVar13 = (int)pMVar11 - (int)temp & 0xf;
    pMVar10 = temp;
    if (temp + 1 < pMVar11) {
      pMVar12 = temp;
      if (uVar13 != 0) {
        if (4 < uVar13) {
          if (8 < uVar13) {
            if (0xc < uVar13) goto LAB_000101d0;
            pMVar9 = xicom + 1;
            temp[0] = x * xicom[0];
            pMVar10 = temp + 1;
          }
          fVar6 = *pMVar9;
          pMVar9 = pMVar9 + 1;
          *pMVar10 = x * fVar6;
          pMVar10 = pMVar10 + 1;
        }
        goto LAB_000101bc;
      }
    }
    else {
LAB_000101bc:
      fVar6 = *pMVar9;
      pMVar9 = pMVar9 + 1;
      *pMVar10 = x * fVar6;
      pMVar12 = pMVar10 + 1;
      if (pMVar11 <= pMVar10 + 1) goto LAB_000101fa;
    }
LAB_000101d0:
    do {
      *pMVar12 = x * *pMVar9;
      pMVar12[1] = x * pMVar9[1];
      pMVar12[2] = x * pMVar9[2];
      pfVar1 = pMVar9 + 3;
      pMVar9 = pMVar9 + 4;
      pMVar12[3] = x * *pfVar1;
      pMVar12 = pMVar12 + 4;
    } while (pMVar12 < pMVar11);
  }
LAB_000101fa:
                    /* Unresolved local var: MeReal * b@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg1(ECX)] */
  pMVar9 = pcom;
  pMVar11 = temp + Ncom;
  if (temp < pMVar11) {
    uVar13 = (int)pMVar11 - (int)temp & 0xf;
    pMVar10 = temp;
    if (temp + 1 < pMVar11) {
      if (uVar13 != 0) {
        if (4 < uVar13) {
          if (8 < uVar13) {
            if (0xc < uVar13) goto LAB_00010230;
            pMVar9 = pcom + 1;
            temp[0] = pcom[0] + temp[0];
            pMVar10 = temp + 1;
          }
          fVar6 = *pMVar9;
          pMVar9 = pMVar9 + 1;
          *pMVar10 = fVar6 + *pMVar10;
          pMVar10 = pMVar10 + 1;
        }
        goto LAB_0001021d;
      }
    }
    else {
LAB_0001021d:
      fVar6 = *pMVar9;
      pMVar9 = pMVar9 + 1;
      *pMVar10 = fVar6 + *pMVar10;
      pMVar10 = pMVar10 + 1;
      if (pMVar11 <= pMVar10) goto LAB_0001025b;
    }
LAB_00010230:
    do {
      *pMVar10 = *pMVar9 + *pMVar10;
      pMVar10[1] = pMVar9[1] + pMVar10[1];
      pMVar10[2] = pMVar9[2] + pMVar10[2];
      pfVar1 = pMVar9 + 3;
      pMVar9 = pMVar9 + 4;
      pMVar10[3] = *pfVar1 + pMVar10[3];
      pMVar10 = pMVar10 + 4;
    } while (pMVar10 < pMVar11);
  }
LAB_0001025b:
  pMVar11 = temp2;
  (*nrdfun)(pMVar11,temp);
                    /* Unresolved local var: MeReal * a@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * b@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal dot@[DW_OP_reg12(ST1)] */
  pMVar10 = xicom;
  pMVar9 = pMVar11 + Ncom;
  fVar6 = 0.0;
  if (pMVar9 <= pMVar11) {
    return 0.0;
  }
  uVar13 = (int)pMVar9 - (int)pMVar11 & 0xf;
  if (temp2 + 1 < pMVar9) {
    if (uVar13 == 0) goto LAB_000102a0;
    if (4 < uVar13) {
      pMVar12 = pMVar11;
      if (8 < uVar13) {
        if (0xc < uVar13) goto LAB_000102a0;
        pMVar10 = xicom + 1;
        fVar6 = xicom[0] * temp2[0];
        pMVar12 = temp2 + 1;
      }
      fVar7 = *pMVar10;
      pMVar10 = pMVar10 + 1;
      pMVar11 = pMVar12 + 1;
      fVar6 = fVar7 * *pMVar12 + fVar6;
    }
  }
  fVar7 = *pMVar10;
  pMVar10 = pMVar10 + 1;
  fVar8 = *pMVar11;
  pMVar11 = pMVar11 + 1;
  fVar6 = fVar7 * fVar8 + fVar6;
  if (pMVar9 <= pMVar11) {
    return fVar6;
  }
LAB_000102a0:
  do {
    fVar7 = *pMVar10;
    fVar8 = *pMVar11;
    pfVar1 = pMVar10 + 1;
    pMVar12 = pMVar11 + 1;
    pfVar2 = pMVar10 + 2;
    pMVar3 = pMVar11 + 2;
    pfVar4 = pMVar10 + 3;
    pMVar10 = pMVar10 + 4;
    pMVar5 = pMVar11 + 3;
    pMVar11 = pMVar11 + 4;
    fVar6 = *pfVar4 * *pMVar5 + *pfVar2 * *pMVar3 + *pfVar1 * *pMVar12 + fVar7 * fVar8 + fVar6;
  } while (pMVar11 < pMVar9);
  return fVar6;
}


/* ==== mnbrak ==== */

void mnbrak(MeReal *ax,MeReal *bx,MeReal *cx,MeReal *fa,MeReal *fb,MeReal *fc,_func_MeReal_float *f)

{
  float fVar1;
  MeReal MVar2;
  longdouble lVar3;
  longdouble lVar4;
  longdouble lVar5;
  longdouble lVar6;
  longdouble lVar7;
  longdouble lVar8;
  longdouble lVar9;
  
  lVar3 = (longdouble)(*f)(*ax);
  *fa = (float)lVar3;
  lVar3 = (longdouble)(*f)(*bx);
  *fb = (float)lVar3;
  if ((longdouble)*fa < lVar3) {
                    /* Unresolved local var: MeReal dum@[DW_OP_reg2(EDX)] */
    MVar2 = *ax;
    *ax = *bx;
    *bx = MVar2;
    MVar2 = *fb;
    *fb = *fa;
    *fa = MVar2;
  }
  fVar1 = (*bx - *ax) * DAT_00011528 + *bx;
  *cx = fVar1;
  lVar3 = (longdouble)(*f)(fVar1);
  *fc = (float)lVar3;
  lVar4 = (longdouble)*fb;
  if (lVar3 < lVar4) {
    do {
                    /* Unresolved local var: MeReal r@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal u@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal ulim@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal fu@[DW_OP_reg12(ST1)] */
      lVar5 = (longdouble)*bx;
      lVar6 = (longdouble)*cx;
      lVar7 = (lVar4 - (longdouble)*fc) * (lVar5 - (longdouble)*ax);
      lVar4 = (lVar4 - (longdouble)*fa) * (lVar5 - lVar6);
      lVar9 = lVar4 - lVar7;
      lVar3 = (longdouble)DAT_0001152c;
      lVar8 = ABS(lVar9);
      if (lVar3 < lVar8) {
        lVar3 = lVar8;
      }
      if (lVar9 < (longdouble)0.0) {
        lVar3 = -ABS(lVar3);
      }
      else {
        lVar3 = ABS(lVar3);
      }
      lVar8 = (longdouble)0;
      lVar4 = lVar5 - ((lVar5 - lVar6) * lVar4 - (lVar5 - (longdouble)*ax) * lVar7) /
                      (lVar3 + lVar3);
      lVar3 = (lVar6 - lVar5) * (longdouble)DAT_00011530 + lVar5;
      if (lVar8 < (lVar5 - lVar4) * (lVar4 - lVar6)) {
        lVar3 = (longdouble)(*f)((float)lVar4);
        if (lVar3 < (longdouble)*fc) {
          *ax = *bx;
          *bx = (float)lVar4;
          *fa = *fb;
          *fb = (float)lVar3;
          return;
        }
        if ((longdouble)*fb < lVar3) {
          *cx = (float)lVar4;
          *fc = (float)lVar3;
          return;
        }
        lVar3 = ((longdouble)*cx - (longdouble)*bx) * (longdouble)DAT_00011528 + (longdouble)*cx;
LAB_00010505:
        lVar5 = (longdouble)(*f)((float)lVar3);
        lVar3 = (longdouble)(float)lVar3;
      }
      else {
        if ((lVar6 - lVar4) * (lVar4 - lVar3) <= lVar8) {
          if ((lVar3 - lVar6) * (lVar4 - lVar3) < lVar8) {
            lVar3 = (lVar6 - lVar5) * (longdouble)DAT_00011528 + lVar6;
          }
          goto LAB_00010505;
        }
        lVar5 = (longdouble)(*f)((float)lVar4);
        lVar3 = (longdouble)(float)lVar4;
        if (lVar5 < (longdouble)*fc) {
          *bx = *cx;
          *cx = (float)lVar4;
          fVar1 = *bx;
          *fb = *fc;
          lVar4 = (longdouble)DAT_00011528;
          *fc = (float)lVar5;
          lVar3 = lVar3 + (lVar3 - (longdouble)fVar1) * lVar4;
          goto LAB_00010505;
        }
      }
      *ax = *bx;
      *bx = *cx;
      *cx = (float)lVar3;
      *fa = *fb;
      *fb = *fc;
      *fc = (float)lVar5;
      lVar4 = (longdouble)*fb;
    } while (lVar5 < lVar4);
  }
  return;
}


/* ==== dbrent ==== */

MeReal dbrent(MeReal *xmin,MeReal ax,MeReal bx,MeReal cx,_func_MeReal_float *f,
             _func_MeReal_float *df,MeReal tol)

{
  float fVar1;
  bool bVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  bool bVar8;
  MeReal MVar9;
  MeReal MVar10;
  int iVar11;
  float fVar12;
  longdouble lVar13;
  MeReal tol2;
  MeReal e;
  MeReal d;
  MeReal dw;
  MeReal dv;
  MeReal dx;
  MeReal fw;
  MeReal fx;
  MeReal w;
  MeReal b;
  MeReal a;
  
  a = cx;
                    /* Unresolved local var: MeReal v@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal x@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal fv@[DW_OP_reg7(EDI)] */
  if (ax < cx) {
    a = ax;
  }
  b = cx;
  if (cx < ax) {
    b = ax;
  }
  w = bx;
                    /* Unresolved local var: int iter@[DW_OP_reg6(ESI)] */
  lVar13 = (longdouble)(*f)(bx);
  fVar12 = (float)lVar13;
  lVar13 = (longdouble)(*df)(bx);
  dv = (MeReal)lVar13;
  dx = (MeReal)lVar13;
  dw = (MeReal)lVar13;
  d = 0.0;
  e = 0.0;
  iVar11 = 99;
  fw = fVar12;
  fx = fVar12;
  fVar4 = bx;
  do {
                    /* Unresolved local var: MeReal u@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal fu@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal xm@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal tol1@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal du@[DW_OP_reg11(ST0)] */
    MVar9 = fw;
    MVar10 = e;
    fVar6 = b - a;
    fVar3 = (ABS(fVar4) + 1e-10) * tol;
    fVar5 = (a + b) * DAT_00011538;
    fVar1 = fVar3 + fVar3;
    if (ABS(fVar4 - fVar5) <= fVar1 - fVar6 * DAT_00011538) {
LAB_00010a2c:
      *xmin = fVar4;
      lVar13 = (longdouble)fx;
      goto LAB_00010966;
    }
    if (ABS(e) <= fVar3) {
      fVar1 = b;
      if (DAT_00011534 < dx) {
        fVar1 = a;
      }
      e = fVar1 - fVar4;
      d = DAT_00011538 * e;
    }
    else {
                    /* Unresolved local var: MeReal d1@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal d2@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal u1@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal u2@[DW_OP_reg13(ST2)]
                       Unresolved local var: int ok1@[DW_OP_reg2(EDX)]
                       Unresolved local var: int ok2@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal olde@[DW_OP_reg13(ST2)] */
      fVar6 = fVar6 + fVar6;
      fVar7 = fVar6;
      if (dw != dx) {
        fVar7 = ((w - fVar4) * dx) / (dx - dw);
      }
      if (dv != dx) {
        fVar6 = ((bx - fVar4) * dx) / (dx - dv);
      }
      bVar2 = false;
      if ((0.0 < (a - (fVar4 + fVar7)) * ((fVar4 + fVar7) - b)) && (dx * fVar7 < 0.0)) {
        bVar2 = true;
      }
      bVar8 = false;
      if ((0.0 < ((fVar4 + fVar6) - b) * (a - (fVar4 + fVar6))) && (dx * fVar6 < 0.0)) {
        bVar8 = true;
      }
      e = d;
      if (bVar2) {
        d = fVar7;
        if ((bVar8) && (ABS(fVar6) <= ABS(fVar7))) {
          d = fVar6;
        }
LAB_00010814:
        if (ABS(d) <= ABS(MVar10 * DAT_00011538)) {
          if (((d + fVar4) - a < fVar1) || (b - (d + fVar4) < fVar1)) {
            if (fVar5 - fVar4 < 0.0) {
              d = -ABS(fVar3);
            }
            else {
              d = ABS(fVar3);
            }
          }
          goto LAB_00010890;
        }
      }
      else {
        d = fVar6;
        if (bVar8) goto LAB_00010814;
      }
      fVar1 = b;
      if (0.0 < dx) {
        fVar1 = a;
      }
      e = fVar1 - fVar4;
      d = DAT_00011538 * e;
    }
LAB_00010890:
    if (fVar3 <= ABS(d)) {
      fVar3 = d + fVar4;
      lVar13 = (longdouble)(*f)(fVar3);
    }
    else {
      if (d < 0.0) {
        fVar3 = -ABS(fVar3);
      }
      else {
        fVar3 = ABS(fVar3);
      }
      fVar3 = fVar4 + fVar3;
      lVar13 = (longdouble)(*f)(fVar3);
      if ((longdouble)fx < lVar13) goto LAB_00010a2c;
    }
    fVar1 = (float)lVar13;
    lVar13 = (longdouble)(*df)(fVar3);
    if (fx <= fVar1) {
      MVar10 = fVar3;
      if (fVar3 < fVar4) {
        MVar10 = b;
        a = fVar3;
      }
      b = MVar10;
      if ((fVar1 < fw) || (w == fVar4)) {
        dv = dw;
        dw = (MeReal)lVar13;
        bx = w;
        fVar12 = MVar9;
        fw = fVar1;
        w = fVar3;
      }
      else if (((fVar1 < fVar12) || (bx == fVar4)) || (bx == w)) {
        dv = (MeReal)lVar13;
        bx = fVar3;
        fVar12 = fVar1;
      }
    }
    else {
      MVar10 = fVar4;
      if (fVar4 <= fVar3) {
        MVar10 = b;
        a = fVar4;
      }
      b = MVar10;
      dv = dw;
      fw = fx;
      dw = dx;
      bx = w;
      fVar12 = MVar9;
      dx = (float)lVar13;
      fx = fVar1;
      w = fVar4;
      fVar4 = fVar3;
    }
    bVar2 = 0 < iVar11;
    iVar11 = iVar11 + -1;
  } while (bVar2);
  lVar13 = (longdouble)(*f)(*xmin);
LAB_00010966:
  return (MeReal)lVar13;
}


/* ==== LineFnMinNd ==== */

MeReal LineFnMinNd(int N,MeReal *p,MeReal *xi,MeReal tol,_func_MeReal_MeReal_ptr *f,
                  _func_void_MeReal_ptr_MeReal_ptr *grad)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  uint uVar4;
  MeReal *pMVar5;
  float *pfVar6;
  MeReal *pMVar7;
  MeReal *pMVar8;
  MeReal *pMVar9;
  MeReal MVar10;
  MeReal xmin;
  MeReal bx;
  MeReal fa;
  MeReal fx;
  MeReal fb;
  MeReal xx;
  MeReal ax;
  
                    /* Unresolved local var: MeReal fRet@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal * b@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg3(EBX)] */
  pMVar5 = pcom;
  nrfunc = f;
  pMVar7 = pcom + N;
  nrdfun = grad;
  Ncom = N;
  if (pcom < pMVar7) {
    uVar4 = N * 4 & 0xf;
    pMVar8 = p;
    if (pcom + 1 < pMVar7) {
      pMVar9 = p;
      if (uVar4 != 0) {
        if (4 < uVar4) {
          if (8 < uVar4) {
            if (0xc < uVar4) goto LAB_00010be0;
            pcom[0] = *p;
            pMVar9 = p + 1;
            pMVar5 = pcom + 1;
          }
          pMVar8 = pMVar9 + 1;
          *pMVar5 = *pMVar9;
          pMVar5 = pMVar5 + 1;
        }
        goto LAB_00010bcc;
      }
    }
    else {
LAB_00010bcc:
      pMVar9 = pMVar8 + 1;
      *pMVar5 = *pMVar8;
      pMVar5 = pMVar5 + 1;
      if (pMVar7 <= pMVar5) goto LAB_00010c00;
    }
LAB_00010be0:
    do {
      *pMVar5 = *pMVar9;
      pMVar5[1] = pMVar9[1];
      pMVar5[2] = pMVar9[2];
      pMVar5[3] = pMVar9[3];
      pMVar5 = pMVar5 + 4;
      pMVar9 = pMVar9 + 4;
    } while (pMVar5 < pMVar7);
  }
LAB_00010c00:
                    /* Unresolved local var: MeReal * b@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg3(EBX)] */
  pMVar5 = xicom;
  pMVar7 = xicom + N;
  if (xicom < pMVar7) {
    uVar4 = N * 4 & 0xf;
    pMVar8 = xi;
    if (xicom + 1 < pMVar7) {
      if (uVar4 != 0) {
        if (4 < uVar4) {
          pMVar9 = xi;
          if (8 < uVar4) {
            if (0xc < uVar4) goto LAB_00010c34;
            xicom[0] = *xi;
            pMVar9 = xi + 1;
            pMVar5 = xicom + 1;
          }
          pMVar8 = pMVar9 + 1;
          *pMVar5 = *pMVar9;
          pMVar5 = pMVar5 + 1;
        }
        goto LAB_00010c26;
      }
    }
    else {
LAB_00010c26:
      *pMVar5 = *pMVar8;
      pMVar5 = pMVar5 + 1;
      pMVar8 = pMVar8 + 1;
      if (pMVar7 <= pMVar5) goto LAB_00010c54;
    }
LAB_00010c34:
    do {
      *pMVar5 = *pMVar8;
      pMVar5[1] = pMVar8[1];
      pMVar5[2] = pMVar8[2];
      pMVar5[3] = pMVar8[3];
      pMVar5 = pMVar5 + 4;
      pMVar8 = pMVar8 + 4;
    } while (pMVar5 < pMVar7);
  }
LAB_00010c54:
  ax = 0.0;
  xx = 1.0;
  mnbrak(&ax,&xx,&bx,&fa,&fx,&fb,f1d);
                    /* Unresolved local var: MeReal * b@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * c@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg3(EBX)] */
  pfVar2 = p + N;
  MVar10 = dbrent(&xmin,ax,xx,bx,f1d,df1d,tol);
  if (pfVar2 <= p) {
    return MVar10;
  }
  uVar4 = (int)pfVar2 - (int)p & 0xf;
  if (p + 1 < pfVar2) {
    pfVar6 = p;
    if (uVar4 == 0) goto LAB_00010ce6;
    if (4 < uVar4) {
      pMVar7 = p;
      if (8 < uVar4) {
        if (0xc < uVar4) goto LAB_00010ce6;
        fVar3 = *xi;
        xi = xi + 1;
        *p = xmin * fVar3 + *p;
        pMVar7 = p + 1;
      }
      fVar3 = *xi;
      xi = xi + 1;
      p = pMVar7 + 1;
      *pMVar7 = xmin * fVar3 + *pMVar7;
    }
  }
  fVar3 = *xi;
  xi = xi + 1;
  pfVar6 = p + 1;
  *p = xmin * fVar3 + *p;
  p = p + 1;
  if (pfVar2 <= p) {
    return MVar10;
  }
LAB_00010ce6:
  do {
    *p = xmin * *xi + *pfVar6;
    p[1] = xmin * xi[1] + pfVar6[1];
    p[2] = xmin * xi[2] + pfVar6[2];
    pfVar1 = xi + 3;
    xi = xi + 4;
    p[3] = xmin * *pfVar1 + pfVar6[3];
    p = p + 4;
    pfVar6 = pfVar6 + 4;
  } while (p < pfVar2);
  return MVar10;
}


/* ==== FnMin1d ==== */

MeReal FnMin1d(MeReal *p,MeReal fTol,_func_MeReal_float *f,_func_MeReal_float *df)

{
  MeReal MVar1;
  MeReal bx;
  MeReal fa;
  MeReal fx;
  MeReal fb;
  MeReal xx;
  MeReal ax;
  
  ax = 0.0;
  xx = 1.0;
  mnbrak(&ax,&xx,&bx,&fa,&fx,&fb,f);
  MVar1 = dbrent(p,ax,xx,bx,f,df,fTol);
  return MVar1;
}


/* ==== FnMinNd ==== */

/* WARNING: Unknown calling convention */

MeReal FnMinNd(int N,MeReal *p,MeReal tol,MeReal fTol,_func_MeReal_MeReal_ptr *f,
              _func_void_MeReal_ptr_MeReal_ptr *grad)

{
  MeReal *pMVar1;
  float *pfVar2;
  float *pfVar3;
  float *pfVar4;
  float *pfVar5;
  MeReal *pMVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  MeReal MVar11;
  bool bVar12;
  float *pfVar13;
  MeReal *pMVar14;
  float *pfVar15;
  MeReal *pMVar16;
  float *pfVar17;
  float *pfVar18;
  float *pfVar19;
  uint uVar20;
  longdouble lVar21;
  MeReal MVar22;
  int its;
  MeReal fMin;
  MeReal fp;
  MeReal temp [4];
  MeReal h [4];
  MeReal g [4];
  MeReal xi [4];
  
  lVar21 = (longdouble)(*f)(p);
  (*grad)(xi,p);
                    /* Unresolved local var: MeReal * e@[DW_OP_reg1(ECX)] */
  pMVar16 = xi;
  pMVar14 = xi + N;
  if (pMVar16 < pMVar14) {
    uVar20 = (int)pMVar14 - (int)pMVar16 & 0xf;
    if (xi + 1 < pMVar14) {
      if (uVar20 != 0) {
        if (4 < uVar20) {
          if (8 < uVar20) {
            if (0xc < uVar20) goto LAB_00010e9e;
                    /* Unresolved local var: MeReal t@[???] */
            xi[0] = -xi[0];
            pMVar16 = xi + 1;
          }
          *(byte *)((int)pMVar16 + 3U) = *(byte *)((int)pMVar16 + 3U) ^ 0x80;
          pMVar16 = pMVar16 + 1;
        }
        goto LAB_00010e93;
      }
    }
    else {
LAB_00010e93:
      *(byte *)((int)pMVar16 + 3U) = *(byte *)((int)pMVar16 + 3U) ^ 0x80;
      pMVar16 = pMVar16 + 1;
      if (pMVar14 <= pMVar16) goto LAB_00010eb5;
    }
LAB_00010e9e:
    do {
      *(byte *)((int)pMVar16 + 3U) = *(byte *)((int)pMVar16 + 3U) ^ 0x80;
      *(byte *)((int)pMVar16 + 7U) = *(byte *)((int)pMVar16 + 7U) ^ 0x80;
      *(byte *)((int)pMVar16 + 0xbU) = *(byte *)((int)pMVar16 + 0xbU) ^ 0x80;
      *(byte *)((int)pMVar16 + 0xfU) = *(byte *)((int)pMVar16 + 0xfU) ^ 0x80;
      pMVar16 = pMVar16 + 4;
    } while (pMVar16 < pMVar14);
  }
LAB_00010eb5:
                    /* Unresolved local var: MeReal * b@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg3(EBX)] */
  pMVar16 = g;
  pMVar14 = xi;
  pMVar6 = g + N;
  if (pMVar16 < pMVar6) {
    uVar20 = (int)pMVar6 - (int)pMVar16 & 0xf;
    if (g + 1 < pMVar6) {
      if (uVar20 != 0) {
        if (4 < uVar20) {
          if (8 < uVar20) {
            if (0xc < uVar20) goto LAB_00010ee6;
            pMVar14 = xi + 1;
            g[0] = xi[0];
            pMVar16 = g + 1;
          }
          MVar22 = *pMVar14;
          pMVar14 = pMVar14 + 1;
          *pMVar16 = MVar22;
          pMVar16 = pMVar16 + 1;
        }
        goto LAB_00010ed8;
      }
    }
    else {
LAB_00010ed8:
      MVar22 = *pMVar14;
      pMVar14 = pMVar14 + 1;
      *pMVar16 = MVar22;
      pMVar16 = pMVar16 + 1;
      if (pMVar6 <= pMVar16) goto LAB_00010f06;
    }
LAB_00010ee6:
    do {
      *pMVar16 = *pMVar14;
      pMVar16[1] = pMVar14[1];
      pMVar16[2] = pMVar14[2];
      pMVar1 = pMVar14 + 3;
      pMVar14 = pMVar14 + 4;
      pMVar16[3] = *pMVar1;
      pMVar16 = pMVar16 + 4;
    } while (pMVar16 < pMVar6);
  }
LAB_00010f06:
                    /* Unresolved local var: MeReal * b@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg3(EBX)] */
  pMVar16 = h;
  pMVar14 = xi;
  pMVar6 = h + N;
  if (pMVar6 <= pMVar16) goto LAB_00010f57;
  uVar20 = (int)pMVar6 - (int)pMVar16 & 0xf;
  if (h + 1 < pMVar6) {
    if (uVar20 != 0) {
      if (4 < uVar20) {
        if (8 < uVar20) {
          if (0xc < uVar20) goto LAB_00010f37;
          pMVar16 = h + 1;
          h[0] = xi[0];
          pMVar14 = xi + 1;
        }
        MVar22 = *pMVar14;
        pMVar14 = pMVar14 + 1;
        *pMVar16 = MVar22;
        pMVar16 = pMVar16 + 1;
      }
      goto LAB_00010f29;
    }
  }
  else {
LAB_00010f29:
    MVar22 = *pMVar14;
    pMVar14 = pMVar14 + 1;
    *pMVar16 = MVar22;
    pMVar16 = pMVar16 + 1;
    if (pMVar6 <= pMVar16) goto LAB_00010f57;
  }
LAB_00010f37:
  do {
    *pMVar16 = *pMVar14;
    pMVar16[1] = pMVar14[1];
    pMVar16[2] = pMVar14[2];
    pMVar1 = pMVar14 + 3;
    pMVar14 = pMVar14 + 4;
    pMVar16[3] = *pMVar1;
    pMVar16 = pMVar16 + 4;
  } while (pMVar16 < pMVar6);
LAB_00010f57:
  its = 99;
  do {
                    /* Unresolved local var: MeReal gg@[???]
                       Unresolved local var: MeReal dgg@[???] */
    fp = (MeReal)lVar21;
    MVar22 = LineFnMinNd(N,p,xi,tol,f,grad);
    fVar7 = MVar22 - fp;
    fp = ABS(fp);
    if (ABS(fVar7) + ABS(fVar7) <= (ABS(MVar22) + fp + 1e-10) * fTol) {
      return MVar22;
    }
                    /* Unresolved local var: MeReal * a@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal norm2@[DW_OP_reg13(ST2)] */
    pfVar13 = g;
    fVar7 = 0.0;
    pfVar15 = g + N;
    fVar8 = 0.0;
    if (pfVar13 < pfVar15) {
      uVar20 = (int)pfVar15 - (int)pfVar13 & 0xf;
      fVar9 = fVar7;
      if (g + 1 < pfVar15) {
        if (uVar20 != 0) {
          fVar9 = fVar8;
          if (4 < uVar20) {
            pfVar19 = pfVar13;
            if (8 < uVar20) {
              if (0xc < uVar20) goto LAB_00010ff0;
                    /* Unresolved local var: MeReal t@[???] */
              fVar7 = g[0] * g[0];
              pfVar19 = g + 1;
            }
            pfVar13 = pfVar19 + 1;
            fVar9 = *pfVar19 * *pfVar19 + fVar7;
          }
          goto LAB_00010fdf;
        }
      }
      else {
LAB_00010fdf:
        fVar7 = *pfVar13;
        pfVar13 = pfVar13 + 1;
        fVar7 = fVar7 * fVar7 + fVar9;
        fVar8 = fVar7;
        if (pfVar15 <= pfVar13) goto LAB_00011012;
      }
LAB_00010ff0:
      do {
        fVar7 = *pfVar13;
        pfVar19 = pfVar13 + 1;
        pfVar17 = pfVar13 + 2;
        pfVar18 = pfVar13 + 3;
        pfVar13 = pfVar13 + 4;
        fVar7 = *pfVar18 * *pfVar18 +
                *pfVar17 * *pfVar17 + *pfVar19 * *pfVar19 + fVar7 * fVar7 + fVar8;
        fVar8 = fVar7;
      } while (pfVar13 < pfVar15);
    }
LAB_00011012:
    if (fVar7 == 0.0) {
      return MVar22;
    }
    lVar21 = (longdouble)(*f)(p);
    pMVar16 = xi;
    (*grad)(pMVar16,p);
                    /* Unresolved local var: MeReal * b@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * c@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg6(ESI)] */
    pfVar13 = temp;
    pfVar19 = temp + N;
    pfVar15 = g;
    if (pfVar13 < pfVar19) {
      uVar20 = (int)pfVar19 - (int)pfVar13 & 0xf;
      if (temp + 1 < pfVar19) {
        if (uVar20 != 0) {
          if (4 < uVar20) {
            if (8 < uVar20) {
              if (0xc < uVar20) goto LAB_00011080;
              pfVar15 = g + 1;
              pMVar16 = xi + 1;
              temp[0] = g[0] + xi[0];
              pfVar13 = temp + 1;
            }
            fVar8 = *pfVar15;
            pfVar15 = pfVar15 + 1;
            fVar9 = *pMVar16;
            pMVar16 = pMVar16 + 1;
            *pfVar13 = fVar8 + fVar9;
            pfVar13 = pfVar13 + 1;
          }
          goto LAB_0001106b;
        }
      }
      else {
LAB_0001106b:
        fVar8 = *pfVar15;
        pfVar15 = pfVar15 + 1;
        fVar9 = *pMVar16;
        pMVar16 = pMVar16 + 1;
        *pfVar13 = fVar8 + fVar9;
        pfVar13 = pfVar13 + 1;
        if (pfVar19 <= pfVar13) goto LAB_000110ae;
      }
LAB_00011080:
      do {
        *pfVar13 = *pfVar15 + *pMVar16;
        pfVar13[1] = pfVar15[1] + pMVar16[1];
        pfVar13[2] = pfVar15[2] + pMVar16[2];
        pfVar17 = pfVar15 + 3;
        pfVar15 = pfVar15 + 4;
        pMVar14 = pMVar16 + 3;
        pMVar16 = pMVar16 + 4;
        pfVar13[3] = *pfVar17 + *pMVar14;
        pfVar13 = pfVar13 + 4;
      } while (pfVar13 < pfVar19);
    }
LAB_000110ae:
                    /* Unresolved local var: MeReal * a@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * b@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal dot@[DW_OP_reg12(ST1)] */
    pfVar13 = temp;
    pfVar15 = xi;
    pfVar19 = pfVar13 + N;
    fVar8 = 0.0;
    fVar9 = 0.0;
    if (pfVar13 < pfVar19) {
      uVar20 = (int)pfVar19 - (int)pfVar13 & 0xf;
      fVar10 = fVar8;
      if (temp + 1 < pfVar19) {
        if (uVar20 != 0) {
          fVar10 = fVar9;
          if (4 < uVar20) {
            pfVar17 = pfVar13;
            if (8 < uVar20) {
              if (0xc < uVar20) goto LAB_000110e2;
              pfVar15 = xi + 1;
              fVar8 = xi[0] * temp[0];
              pfVar17 = temp + 1;
            }
            fVar9 = *pfVar15;
            pfVar15 = pfVar15 + 1;
            pfVar13 = pfVar17 + 1;
            fVar10 = fVar9 * *pfVar17 + fVar8;
          }
          goto LAB_000110d2;
        }
      }
      else {
LAB_000110d2:
        fVar8 = *pfVar15;
        pfVar15 = pfVar15 + 1;
        fVar9 = *pfVar13;
        pfVar13 = pfVar13 + 1;
        fVar8 = fVar8 * fVar9 + fVar10;
        fVar9 = fVar8;
        if (pfVar19 <= pfVar13) goto LAB_0001110a;
      }
LAB_000110e2:
      do {
        fVar8 = *pfVar15;
        fVar10 = *pfVar13;
        pfVar17 = pfVar15 + 1;
        pfVar18 = pfVar13 + 1;
        pfVar2 = pfVar15 + 2;
        pfVar3 = pfVar13 + 2;
        pfVar4 = pfVar15 + 3;
        pfVar15 = pfVar15 + 4;
        pfVar5 = pfVar13 + 3;
        pfVar13 = pfVar13 + 4;
        fVar8 = *pfVar4 * *pfVar5 + *pfVar2 * *pfVar3 + *pfVar17 * *pfVar18 + fVar8 * fVar10 + fVar9
        ;
        fVar9 = fVar8;
      } while (pfVar13 < pfVar19);
    }
LAB_0001110a:
                    /* Unresolved local var: MeReal * b@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg3(EBX)] */
    pMVar16 = g;
    pMVar14 = xi;
    pMVar6 = g + N;
    if (pMVar16 < pMVar6) {
      uVar20 = (int)pMVar6 - (int)pMVar16 & 0xf;
      if (g + 1 < pMVar6) {
        if (uVar20 != 0) {
          if (4 < uVar20) {
            if (8 < uVar20) {
              if (0xc < uVar20) goto LAB_00011143;
              g[0] = -xi[0];
              pMVar14 = xi + 1;
              pMVar16 = g + 1;
            }
            MVar11 = *pMVar14;
            pMVar14 = pMVar14 + 1;
            *pMVar16 = -MVar11;
            pMVar16 = pMVar16 + 1;
          }
          goto LAB_0001112f;
        }
      }
      else {
LAB_0001112f:
        MVar11 = *pMVar14;
        pMVar14 = pMVar14 + 1;
        *pMVar16 = -MVar11;
        pMVar16 = pMVar16 + 1;
        if (pMVar6 <= pMVar16) goto LAB_0001117b;
      }
LAB_00011143:
      do {
        *pMVar16 = -*pMVar14;
        pMVar16[1] = -pMVar14[1];
        pMVar16[2] = -pMVar14[2];
        pMVar1 = pMVar14 + 3;
        pMVar14 = pMVar14 + 4;
        pMVar16[3] = -*pMVar1;
        pMVar16 = pMVar16 + 4;
      } while (pMVar16 < pMVar6);
    }
LAB_0001117b:
                    /* Unresolved local var: MeReal * b@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * c@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg6(ESI)] */
    pfVar13 = h;
    fVar8 = fVar8 / fVar7;
    pfVar19 = h + N;
    pfVar15 = g;
    if (pfVar13 < pfVar19) {
      uVar20 = (int)pfVar19 - (int)pfVar13 & 0xf;
      pfVar17 = pfVar13;
      if (h + 1 < pfVar19) {
        pfVar18 = pfVar13;
        if (uVar20 != 0) {
          if (4 < uVar20) {
            if (8 < uVar20) {
              if (0xc < uVar20) goto LAB_000111c0;
              pfVar15 = g + 1;
              h[0] = fVar8 * h[0] + g[0];
              pfVar13 = h + 1;
            }
            fVar7 = *pfVar15;
            pfVar15 = pfVar15 + 1;
            *pfVar13 = fVar8 * *pfVar13 + fVar7;
            pfVar17 = pfVar13 + 1;
          }
          goto LAB_000111a6;
        }
      }
      else {
LAB_000111a6:
        pfVar13 = pfVar17 + 1;
        fVar7 = *pfVar15;
        pfVar15 = pfVar15 + 1;
        *pfVar17 = fVar8 * *pfVar17 + fVar7;
        pfVar18 = pfVar17 + 1;
        if (pfVar19 <= pfVar17 + 1) goto LAB_000111f8;
      }
LAB_000111c0:
      do {
        *pfVar18 = fVar8 * *pfVar13 + *pfVar15;
        pfVar18[1] = fVar8 * pfVar13[1] + pfVar15[1];
        pfVar18[2] = fVar8 * pfVar13[2] + pfVar15[2];
        pfVar17 = pfVar13 + 3;
        pfVar13 = pfVar13 + 4;
        pfVar2 = pfVar15 + 3;
        pfVar15 = pfVar15 + 4;
        pfVar18[3] = fVar8 * *pfVar17 + *pfVar2;
        pfVar18 = pfVar18 + 4;
      } while (pfVar18 < pfVar19);
    }
LAB_000111f8:
                    /* Unresolved local var: MeReal * b@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * e@[DW_OP_reg3(EBX)] */
    pMVar16 = xi;
    pMVar14 = h;
    pMVar6 = pMVar16 + N;
    if (pMVar16 < pMVar6) {
      uVar20 = (int)pMVar6 - (int)pMVar16 & 0xf;
      if (xi + 1 < pMVar6) {
        if (uVar20 != 0) {
          if (4 < uVar20) {
            if (8 < uVar20) {
              if (0xc < uVar20) goto LAB_00011224;
              xi[0] = h[0];
              pMVar14 = h + 1;
              pMVar16 = xi + 1;
            }
            MVar11 = *pMVar14;
            pMVar14 = pMVar14 + 1;
            *pMVar16 = MVar11;
            pMVar16 = pMVar16 + 1;
          }
          goto LAB_00011216;
        }
      }
      else {
LAB_00011216:
        MVar11 = *pMVar14;
        pMVar14 = pMVar14 + 1;
        *pMVar16 = MVar11;
        pMVar16 = pMVar16 + 1;
        if (pMVar6 <= pMVar16) goto LAB_00011244;
      }
LAB_00011224:
      do {
        *pMVar16 = *pMVar14;
        pMVar16[1] = pMVar14[1];
        pMVar16[2] = pMVar14[2];
        pMVar1 = pMVar14 + 3;
        pMVar14 = pMVar14 + 4;
        pMVar16[3] = *pMVar1;
        pMVar16 = pMVar16 + 4;
      } while (pMVar16 < pMVar6);
    }
LAB_00011244:
    bVar12 = its < 1;
    its = its + -1;
    if (bVar12) {
      return MVar22;
    }
  } while( true );
}


