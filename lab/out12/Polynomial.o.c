/* ==== PolynomialRoots ==== */

MeU32 PolynomialRoots(MeReal *roots,MeReal xmin,MeReal xmax,MeU32 order,MeReal *coef)

{
  MeReal *coef_00;
  float fVar1;
  float xmax_00;
  float fVar2;
  float f_xmin;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  float *pfVar7;
  uint uVar8;
  MeReal MVar9;
  MeU32 newRootsFound;
  MeReal *d_1;
  MeU32 rootsFound;
  MeU32 presentOrder;
  MeU32 d;
  
                    /* Unresolved local var: MeReal * c@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal root@[DW_OP_reg11(ST0)] */
  if (order != 0) {
    fVar1 = coef[order];
    while (fVar1 == 0.0) {
      order = order - 1;
      if (order == 0) {
        return 0;
      }
      fVar1 = coef[order];
    }
    if (order != 0) {
      d = 1;
      pfVar7 = coef;
      if (order != 1) {
        do {
                    /* Unresolved local var: MeReal * lastC@[DW_OP_reg7(EDI)] */
                    /* Unresolved local var: MeU32 i@[DW_OP_reg6(ESI)] */
          uVar8 = 0;
          uVar3 = (order - d) + 1;
          coef = pfVar7 + order + 1;
          uVar5 = uVar3 & 3;
          if (1 < uVar3) {
            if (uVar5 == 0) goto LAB_000100c6;
            if (1 < uVar5) {
              if (2 < uVar5) {
                pfVar7[order + 1] = pfVar7[1];
              }
              uVar3 = (uint)(2 < uVar5);
              uVar8 = uVar3 + 1;
              coef[uVar3] = (float)uVar8 * pfVar7[uVar3 + 1];
            }
          }
          coef[uVar8] = (float)(uVar8 + 1) * pfVar7[uVar8 + 1];
          uVar8 = uVar8 + 1;
          while (uVar8 <= order - d) {
LAB_000100c6:
            coef[uVar8] = (float)(uVar8 + 1) * pfVar7[uVar8 + 1];
            coef[uVar8 + 1] = (float)(uVar8 + 2) * pfVar7[uVar8 + 2];
            coef[uVar8 + 2] = (float)(uVar8 + 3) * pfVar7[uVar8 + 3];
            coef[uVar8 + 3] = (float)(uVar8 + 4) * pfVar7[uVar8 + 4];
            uVar8 = uVar8 + 4;
          }
          d = d + 1;
          pfVar7 = coef;
        } while (d <= order - 1);
      }
      rootsFound = 0;
      fVar1 = -*coef / coef[1];
      if ((xmin <= fVar1) && (fVar1 <= xmax)) {
        *roots = fVar1;
        rootsFound = 1;
      }
      presentOrder = 2;
      if (order < 2) {
        return rootsFound;
      }
      do {
                    /* Unresolved local var: MeReal x0@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal f0@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal f_xmax@[???] */
                    /* Unresolved local var: MeReal value@[DW_OP_reg11(ST0)] */
        newRootsFound = 0;
        coef_00 = coef + (-1 - order);
        uVar8 = presentOrder - 1;
        fVar1 = coef_00[presentOrder];
        fVar2 = fVar1;
        if (uVar8 != 0xffffffff) {
          uVar5 = (uVar8 ^ 0xffffffff) & 3;
          uVar3 = uVar8;
          if (uVar5 != 0) {
            if (uVar5 < 3) {
              if (uVar5 < 2) {
                fVar2 = fVar1 * xmin + coef_00[uVar8];
                uVar3 = presentOrder - 2;
              }
              fVar2 = fVar2 * xmin + coef_00[uVar3];
              uVar3 = uVar3 - 1;
            }
            fVar2 = fVar2 * xmin + coef_00[uVar3];
            uVar3 = uVar3 - 1;
            if (uVar3 == 0xffffffff) goto LAB_000101f7;
          }
          do {
            fVar2 = (((fVar2 * xmin + coef_00[uVar3]) * xmin + coef[(uVar3 - 2) - order]) * xmin +
                    coef[(uVar3 - 3) - order]) * xmin + coef[(uVar3 - 4) - order];
            uVar3 = uVar3 - 4;
          } while (uVar3 != 0xffffffff);
        }
LAB_000101f7:
                    /* Unresolved local var: MeU32 r@[DW_OP_reg7(EDI)] */
        uVar3 = 0;
        xmax_00 = xmin;
        MVar9 = xmin;
        f_xmin = fVar2;
        if (rootsFound != 0) {
          do {
                    /* Unresolved local var: MeReal x1@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal f1@[???] */
                    /* Unresolved local var: MeReal value@[DW_OP_reg12(ST1)] */
            xmax_00 = roots[uVar3];
            fVar2 = fVar1;
            if (uVar8 != 0xffffffff) {
              uVar4 = (uVar8 ^ 0xffffffff) & 3;
              uVar5 = uVar8;
              if (uVar4 != 0) {
                uVar6 = uVar8;
                if (uVar4 < 3) {
                  if (uVar4 < 2) {
                    uVar5 = presentOrder - 2;
                    fVar2 = fVar1 * xmax_00 + coef_00[uVar8];
                  }
                  uVar6 = uVar5 - 1;
                  fVar2 = fVar2 * xmax_00 + coef_00[uVar5];
                }
                fVar2 = fVar2 * xmax_00 + coef_00[uVar6];
                uVar5 = uVar6 - 1;
                if (uVar6 - 1 == 0xffffffff) goto LAB_00010267;
              }
              do {
                fVar2 = (((fVar2 * xmax_00 + coef_00[uVar5]) * xmax_00 + coef[(uVar5 - 2) - order])
                         * xmax_00 + coef[(uVar5 - 3) - order]) * xmax_00 +
                        coef[(uVar5 - 4) - order];
                uVar5 = uVar5 - 4;
              } while (uVar5 != 0xffffffff);
            }
LAB_00010267:
            if (fVar2 == 0.0) {
              roots[newRootsFound] = xmax_00;
              newRootsFound = newRootsFound + 1;
              fVar1 = coef_00[presentOrder];
            }
            else if (f_xmin * fVar2 < 0.0) {
              MVar9 = BracketedRootN(MVar9,xmax_00,f_xmin,fVar2,presentOrder,coef_00,coef);
              roots[newRootsFound] = MVar9;
              newRootsFound = newRootsFound + 1;
              fVar1 = coef_00[presentOrder];
            }
            uVar3 = uVar3 + 1;
            MVar9 = xmax_00;
            f_xmin = fVar2;
          } while (uVar3 < rootsFound);
        }
                    /* Unresolved local var: MeReal value@[DW_OP_reg12(ST1)] */
        if (uVar8 != 0xffffffff) {
          uVar3 = (uVar8 ^ 0xffffffff) & 3;
          if (uVar3 != 0) {
            if (uVar3 < 3) {
              uVar5 = uVar8;
              if (uVar3 < 2) {
                uVar5 = presentOrder - 2;
                fVar1 = fVar1 * xmax + coef_00[uVar8];
              }
              fVar1 = fVar1 * xmax + coef_00[uVar5];
              uVar8 = uVar5 - 1;
            }
            fVar1 = fVar1 * xmax + coef_00[uVar8];
            uVar8 = uVar8 - 1;
            if (uVar8 == 0xffffffff) goto LAB_000102ef;
          }
          do {
            fVar1 = (((fVar1 * xmax + coef_00[uVar8]) * xmax + coef[(uVar8 - 2) - order]) * xmax +
                    coef[(uVar8 - 3) - order]) * xmax + coef[(uVar8 - 4) - order];
            uVar8 = uVar8 - 4;
          } while (uVar8 != 0xffffffff);
        }
LAB_000102ef:
        if (fVar2 * fVar1 < 0.0) {
          MVar9 = BracketedRootN(xmax_00,xmax,fVar2,fVar1,presentOrder,coef_00,coef);
          roots[newRootsFound] = MVar9;
          newRootsFound = newRootsFound + 1;
        }
        rootsFound = newRootsFound;
        presentOrder = presentOrder + 1;
        coef = coef_00;
        if (order < presentOrder) {
          return newRootsFound;
        }
      } while( true );
    }
  }
  return 0;
}


/* ==== BracketedRootN ==== */

MeReal BracketedRootN(MeReal xmin,MeReal xmax,MeReal f_xmin,MeReal f_xmax,MeU32 order,MeReal *coef,
                     MeReal *derivativeCoef)

{
  uint uVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  MeReal MVar5;
  uint uVar6;
  uint uVar7;
  bool bVar8;
  MeI32 maxIterations;
  
                    /* Unresolved local var: MeReal x@[DW_OP_reg14(ST3)] */
  uVar1 = order - 1;
  maxIterations = 0x17;
  fVar4 = (f_xmax * xmin - xmax * f_xmin) / (f_xmax - f_xmin);
  do {
                    /* Unresolved local var: MeReal f_x@[???]
                       Unresolved local var: MeReal df_dx@[???]
                       Unresolved local var: MeBool posSlope@[???]
                       Unresolved local var: MeReal xNew@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal delta@[???]
                       Unresolved local var: MeReal value@[DW_OP_reg12(ST1)] */
    fVar2 = coef[order];
    if (uVar1 != 0xffffffff) {
      uVar6 = (uVar1 ^ 0xffffffff) & 3;
      uVar7 = uVar1;
      if (uVar6 != 0) {
        if (uVar6 < 3) {
          if (uVar6 < 2) {
            uVar7 = order - 2;
            fVar2 = fVar2 * fVar4 + coef[uVar1];
          }
          fVar2 = fVar2 * fVar4 + coef[uVar7];
          uVar7 = uVar7 - 1;
        }
        fVar2 = fVar2 * fVar4 + coef[uVar7];
        uVar7 = uVar7 - 1;
        if (uVar7 == 0xffffffff) goto LAB_00010535;
      }
      do {
        fVar2 = (((fVar2 * fVar4 + coef[uVar7]) * fVar4 + coef[uVar7 - 1]) * fVar4 + coef[uVar7 - 2]
                ) * fVar4 + coef[uVar7 - 3];
        uVar7 = uVar7 - 4;
      } while (uVar7 != 0xffffffff);
    }
LAB_00010535:
    fVar3 = fVar2;
    MVar5 = fVar4;
    if ((0.0 <= fVar2 * f_xmin) &&
       (fVar3 = f_xmax, f_xmin = fVar2, xmin = fVar4, MVar5 = xmax, 0.0 <= fVar2 * f_xmax)) {
      return fVar4;
    }
                    /* Unresolved local var: MeReal value@[DW_OP_reg13(ST2)] */
    xmax = MVar5;
    f_xmax = fVar3;
    uVar7 = order - 2;
    fVar3 = derivativeCoef[uVar1];
    if (uVar7 != 0xffffffff) {
      uVar6 = (uVar7 ^ 0xffffffff) & 3;
      if (uVar6 != 0) {
        if (uVar6 < 3) {
          if (uVar6 < 2) {
            fVar3 = fVar3 * fVar4 + derivativeCoef[uVar7];
            uVar7 = order - 3;
          }
          fVar3 = fVar3 * fVar4 + derivativeCoef[uVar7];
          uVar7 = uVar7 - 1;
        }
        fVar3 = fVar3 * fVar4 + derivativeCoef[uVar7];
        uVar7 = uVar7 - 1;
        if (uVar7 == 0xffffffff) goto LAB_000105a3;
      }
      do {
        fVar3 = (((fVar3 * fVar4 + derivativeCoef[uVar7]) * fVar4 + derivativeCoef[uVar7 - 1]) *
                 fVar4 + derivativeCoef[uVar7 - 2]) * fVar4 + derivativeCoef[uVar7 - 3];
        uVar7 = uVar7 - 4;
      } while (uVar7 != 0xffffffff);
    }
LAB_000105a3:
    bVar8 = 0.0 < (fVar4 - xmin) * fVar3 - fVar2;
    if ((bVar8 == 0.0 < fVar3) && (0.0 < (xmax - fVar4) * fVar3 + fVar2 == bVar8)) {
      fVar2 = fVar4 - fVar2 * (1.0 / fVar3);
    }
    else {
      fVar2 = (1.0 / (f_xmax - f_xmin)) * (f_xmax * xmin - xmax * f_xmin);
    }
    if ((fVar2 - fVar4) * (fVar2 - fVar4) < fVar2 * fVar2 * 1.4399999e-14) {
      return fVar2;
    }
    maxIterations = maxIterations + -1;
    fVar4 = fVar2;
    if (maxIterations == 0) {
      return fVar2;
    }
  } while( true );
}


