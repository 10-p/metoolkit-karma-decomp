/* ==== MeHSVtoRGB ==== */

void MeHSVtoRGB(float h,float s,float v,float *rgb)

{
  int iVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  ushort __cwtmp;
  ushort __cw;
  
                    /* Unresolved local var: int i@[DW_OP_reg0(EAX)]
                       Unresolved local var: float f@[DW_OP_reg12(ST1)]
                       Unresolved local var: float p@[DW_OP_reg15(ST4)]
                       Unresolved local var: float q@[DW_OP_reg11(ST0)]
                       Unresolved local var: float t@[DW_OP_reg13(ST2)] */
  if (s == 0.0) {
    rgb[2] = v;
    rgb[1] = v;
    *rgb = v;
    return;
  }
                    /* Unresolved local var: longdouble __value@[DW_OP_reg11(ST0)] */
  iVar1 = (int)ROUND(ROUND(h * 0.016666668));
  fVar2 = h * 0.016666668 - (float)iVar1;
  fVar4 = (1.0 - s) * v;
  fVar3 = (1.0 - s * fVar2) * v;
  fVar2 = (1.0 - s * (1.0 - fVar2)) * v;
  switch(iVar1) {
  case 0:
    *rgb = v;
    rgb[1] = fVar2;
    break;
  case 1:
    *rgb = fVar3;
    rgb[1] = v;
    break;
  case 2:
    *rgb = fVar4;
    rgb[1] = v;
    rgb[2] = fVar2;
    return;
  case 3:
    *rgb = fVar4;
    rgb[1] = fVar3;
    goto LAB_000100ee;
  case 4:
    *rgb = fVar2;
    rgb[1] = fVar4;
LAB_000100ee:
    rgb[2] = v;
    return;
  default:
    *rgb = v;
    rgb[1] = fVar4;
    rgb[2] = fVar3;
    return;
  }
  rgb[2] = fVar4;
  return;
}


