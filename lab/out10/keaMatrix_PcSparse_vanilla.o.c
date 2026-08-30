/* ==== allocate ==== */

/* DWARF original prototype: void allocate(keaMatrix_pcSparse_vanilla * this, int size) */

void __thiscall keaMatrix_pcSparse_vanilla::allocate(keaMatrix_pcSparse_vanilla *this,int size)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  
  *(int *)&this->field_0x4 = size;
  iVar3 = size + 3 >> 2;
  *(int *)&this->field_0x14 = iVar3;
  *(int *)&this->field_0x8 = iVar3 << 2;
  iVar3 = keaPoolAlloc(*(int *)&this->field_0x8 * 4 + 0x10,"rsD");
  *(uint *)&this->field_0x18 = iVar3 + 0xfU & 0xfffffff0;
  iVar3 = keaPoolAlloc(*(int *)&this->field_0x8 * *(int *)&this->field_0x8 * 4 + 0x10,"A");
  *(uint *)&this->field_0xc = iVar3 + 0xfU & 0xfffffff0;
  iVar3 = keaPoolAlloc(*(int *)&this->field_0x8 * *(int *)&this->field_0x8 * 4 + 0x10,"Achol");
  *(undefined4 *)&this->field_0x24 = *(undefined4 *)&this->field_0xc;
  uVar1 = iVar3 + 0xfU & 0xfffffff0;
  *(uint *)&this->field_0x10 = uVar1;
  *(uint *)&this->field_0x28 = uVar1;
  uVar2 = keaPoolAlloc((*(int *)&this->field_0x14 * *(int *)&this->field_0x14 + 0xfU & 0xfffffff0)
                       << 2,"NAZ");
  *(undefined4 *)&this->field_0x1c = uVar2;
  uVar2 = keaPoolAlloc((*(int *)&this->field_0x14 * *(int *)&this->field_0x14 + 0xfU & 0xfffffff0)
                       << 2,"NCZ");
  *(undefined4 *)&this->field_0x20 = uVar2;
  uVar2 = keaPoolAlloc((*(int *)&this->field_0x14 + 0xfU & 0xfffffff0) << 2,"NR");
  *(undefined4 *)&this->field_0x2c = uVar2;
  uVar2 = keaPoolAlloc((*(int *)&this->field_0x14 + 0xfU & 0xfffffff0) << 2,"NC");
  *(undefined4 *)&this->field_0x30 = uVar2;
  return;
}


/* ==== makeFromJMJT ==== */

/* DWARF original prototype: void makeFromJMJT(keaMatrix_pcSparse_vanilla * this, MeReal * JM,
   MeReal * Js, int * num_in_strip, int * block2body, MeReal * slipfactor, MeReal epsilon, MeReal
   hinv) */

void __thiscall
keaMatrix_pcSparse_vanilla::makeFromJMJT
          (keaMatrix_pcSparse_vanilla *this,MeReal *JM,MeReal *Js,int *num_in_strip,int *block2body,
          MeReal *slipfactor,MeReal epsilon,MeReal hinv)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  MeReal *pMVar6;
  uint uVar7;
  int iVar8;
  int c;
  int i;
  int num_in_Jstrip;
  MeReal *jmstrip;
  int *JMblock2bodystrip;
  int block;
  MeReal *AcholMatrix;
  MeReal *AMatrix;
  MeReal *J;
  MeReal *jmptr;
  int *JMb;
  int *Jb;
  int step;
  int k;
  int z;
  int jm;
  int strip;
  
                    /* Unresolved local var: int j@[DW_OP_reg1(ECX)]
                       Unresolved local var: int p@[???]
                       Unresolved local var: int stridem@[???] */
  iVar5 = *(int *)&this->field_0x14;
  Jb = block2body;
  J = Js;
  AMatrix = *(MeReal **)&this->field_0x24;
  AcholMatrix = *(MeReal **)&this->field_0x28;
  strip = 0;
  z = 0;
  step = iVar5;
  if (0 < iVar5) {
    do {
      step = step + -1;
      jm = 0;
      jmptr = JM;
      JMb = block2body;
      k = 0xfffffff;
      if (-1 < strip) {
        do {
                    /* Unresolved local var: int body@[DW_OP_reg6(ESI)]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int j@[???]
                       Unresolved local var: int notzero@[DW_OP_reg2(EDX)]
                       Unresolved local var: int * Jbodys@[???]
                       Unresolved local var: int num_in_JMstrip@[DW_OP_reg0(EAX)] */
          jmstrip = jmptr;
          JMblock2bodystrip = JMb;
          iVar5 = 0;
          block = num_in_strip[jm];
          uVar7 = num_in_strip[strip];
          if (0 < block) {
            do {
              iVar2 = *JMblock2bodystrip;
              if ((iVar2 != -1) && (iVar8 = 0, 0 < (int)uVar7)) {
                uVar3 = uVar7 & 3;
                pMVar6 = J;
                if ((int)uVar7 < 2) {
LAB_000101d8:
                  if (iVar2 == Jb[iVar8]) {
                    Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar6,iVar5);
                    iVar5 = 1;
                  }
                  iVar8 = iVar8 + 1;
                  pMVar6 = pMVar6 + 0x18;
                  if ((int)uVar7 <= iVar8) goto LAB_00010240;
                }
                else if (uVar3 != 0) {
                  if (1 < uVar3) {
                    pMVar6 = J;
                    if (2 < uVar3) {
                      if (iVar2 == *Jb) {
                        Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,J,iVar5);
                        iVar5 = 1;
                      }
                      pMVar6 = J + 0x18;
                      iVar8 = 1;
                    }
                    if (iVar2 == Jb[iVar8]) {
                      Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar6,iVar5);
                      iVar5 = 1;
                    }
                    pMVar6 = pMVar6 + 0x18;
                    iVar8 = iVar8 + 1;
                  }
                  goto LAB_000101d8;
                }
                do {
                  if (iVar2 == Jb[iVar8]) {
                    Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar6,iVar5);
                    iVar5 = 1;
                  }
                  if (iVar2 == Jb[iVar8 + 1]) {
                    Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar6 + 0x18,iVar5);
                    iVar5 = 1;
                  }
                  if (iVar2 == Jb[iVar8 + 2]) {
                    Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar6 + 0x30,iVar5);
                    iVar5 = 1;
                  }
                  if (iVar2 == Jb[iVar8 + 3]) {
                    Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar6 + 0x48,iVar5);
                    iVar5 = 1;
                  }
                  iVar8 = iVar8 + 4;
                  pMVar6 = pMVar6 + 0x60;
                } while (iVar8 < (int)uVar7);
              }
LAB_00010240:
              jmstrip = jmstrip + 0x18;
              JMblock2bodystrip = JMblock2bodystrip + 1;
              block = block + -1;
            } while (block != 0);
          }
          if (iVar5 == 0) {
            *(undefined4 *)(*(int *)&this->field_0x1c + z * 4) = 0;
            *(undefined4 *)(*(int *)&this->field_0x20 + z * 4) = 0;
          }
          else {
                    /* Unresolved local var: int d@[DW_OP_reg0(EAX)] */
            *(MeReal **)(*(int *)&this->field_0x1c + z * 4) = AMatrix;
            *(MeReal **)(*(int *)&this->field_0x20 + z * 4) = AcholMatrix;
            iVar5 = 0;
            do {
              AMatrix[iVar5] = AcholMatrix[iVar5];
              AMatrix[iVar5 + 1] = AcholMatrix[iVar5 + 1];
              AMatrix[iVar5 + 2] = AcholMatrix[iVar5 + 2];
              AMatrix[iVar5 + 3] = AcholMatrix[iVar5 + 3];
              AMatrix[iVar5 + 4] = AcholMatrix[iVar5 + 4];
              AMatrix[iVar5 + 5] = AcholMatrix[iVar5 + 5];
              AMatrix[iVar5 + 6] = AcholMatrix[iVar5 + 6];
              iVar2 = iVar5 + 7;
              iVar5 = iVar5 + 8;
              AMatrix[iVar2] = AcholMatrix[iVar2];
            } while (iVar5 < 0x10);
            AMatrix = AMatrix + 0x10;
            uVar7 = k - jm >> 0x1f;
            AcholMatrix = AcholMatrix + 0x10;
            k = ((uVar7 ^ 0xffffffff) & jm) + (k & uVar7);
          }
          JMb = JMb + 8;
          piVar1 = num_in_strip + jm;
          z = z + 1;
          jm = jm + 1;
          jmptr = jmptr + *piVar1 * 0x18;
        } while (jm <= strip);
      }
      *(int *)(*(int *)&this->field_0x30 + strip * 4) = k;
      piVar1 = num_in_strip + strip;
      Jb = Jb + 8;
      z = z + step;
      strip = strip + 1;
      J = J + *piVar1 * 0x18;
      iVar5 = *(int *)&this->field_0x14;
    } while (strip < iVar5);
  }
  *(MeReal **)&this->field_0x24 = AMatrix;
  *(MeReal **)&this->field_0x28 = AcholMatrix;
  i = 0;
  k = 0;
  if (0 < iVar5) {
    do {
      iVar5 = iVar5 * i + i;
      AMatrix = *(MeReal **)(*(int *)&this->field_0x1c + iVar5 * 4);
      AcholMatrix = *(MeReal **)(*(int *)&this->field_0x20 + iVar5 * 4);
      *AMatrix = hinv * slipfactor[k] + epsilon + *AMatrix;
      *AcholMatrix = hinv * slipfactor[k] + epsilon + *AcholMatrix;
      iVar5 = k + 2;
      AMatrix[5] = hinv * slipfactor[k + 1] + epsilon + AMatrix[5];
      AcholMatrix[5] = hinv * slipfactor[k + 1] + epsilon + AcholMatrix[5];
      iVar2 = k + 3;
      k = k + 4;
      AMatrix[10] = hinv * slipfactor[iVar5] + epsilon + AMatrix[10];
      AcholMatrix[10] = hinv * slipfactor[iVar5] + epsilon + AcholMatrix[10];
      AMatrix[0xf] = hinv * slipfactor[iVar2] + epsilon + AMatrix[0xf];
      AcholMatrix[0xf] = hinv * slipfactor[iVar2] + epsilon + AcholMatrix[0xf];
      i = i + 1;
      iVar5 = *(int *)&this->field_0x14;
    } while (i < iVar5);
  }
  uVar7 = *(int *)&this->field_0x4 - 1U & 3;
  i = uVar7 + 1;
  k = i * 5;
  if (3 < (uint)i) goto LAB_0001058a;
  uVar3 = -i & 3;
  if (uVar7 + 2 < 4) {
    if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
          AMatrix[i * 5] = 1.0;
          AcholMatrix[i * 5] = 1.0;
          k = k + 5;
          i = uVar7 + 2;
        }
        AMatrix[k] = 1.0;
        AcholMatrix[k] = 1.0;
        k = k + 5;
        i = i + 1;
      }
      goto LAB_00010505;
    }
LAB_00010527:
    do {
      AMatrix[k] = 1.0;
      AcholMatrix[k] = 1.0;
      AMatrix[k + 5] = 1.0;
      AcholMatrix[k + 5] = 1.0;
      AMatrix[k + 10] = 1.0;
      AcholMatrix[k + 10] = 1.0;
      AMatrix[k + 0xf] = 1.0;
      AcholMatrix[k + 0xf] = 1.0;
      i = i + 4;
      k = k + 0x14;
    } while (i < 4);
  }
  else {
LAB_00010505:
    AMatrix[k] = 1.0;
    AcholMatrix[k] = 1.0;
    k = k + 5;
    i = i + 1;
    if ((uint)i < 4) goto LAB_00010527;
  }
  iVar5 = *(int *)&this->field_0x14;
LAB_0001058a:
  i = 0;
  c = 0;
  if (0 < iVar5) {
    do {
      if (i < iVar5) {
        iVar2 = *(int *)&this->field_0x1c;
        uVar7 = iVar5 - i & 3;
        iVar8 = i;
        if (i + 1 < iVar5) {
          iVar4 = i;
          if (uVar7 != 0) {
            if (1 < uVar7) {
              if (2 < uVar7) {
                if ((*(int *)(iVar2 + (i * iVar5 + i) * 4) != 0) && (c < i)) {
                  c = i;
                }
                iVar8 = i + 1;
              }
              if ((*(int *)(iVar2 + (iVar8 * iVar5 + i) * 4) != 0) && (c < iVar8)) {
                c = iVar8;
              }
              iVar8 = iVar8 + 1;
            }
            goto LAB_000105cb;
          }
        }
        else {
LAB_000105cb:
          if ((*(int *)(iVar2 + (iVar8 * iVar5 + i) * 4) != 0) && (c < iVar8)) {
            c = iVar8;
          }
          iVar4 = iVar8 + 1;
          if (iVar5 <= iVar8 + 1) goto LAB_00010661;
        }
        do {
          if ((*(int *)(iVar2 + (iVar4 * iVar5 + i) * 4) != 0) && (c < iVar4)) {
            c = iVar4;
          }
          iVar8 = iVar4 + 1;
          if ((*(int *)(iVar2 + (iVar8 * iVar5 + i) * 4) != 0) && (c < iVar8)) {
            c = iVar8;
          }
          iVar8 = iVar4 + 2;
          if ((*(int *)(iVar2 + (iVar8 * iVar5 + i) * 4) != 0) && (c < iVar8)) {
            c = iVar8;
          }
          iVar8 = iVar4 + 3;
          if ((*(int *)(iVar2 + (iVar8 * iVar5 + i) * 4) != 0) && (c < iVar8)) {
            c = iVar8;
          }
          iVar4 = iVar4 + 4;
        } while (iVar4 < iVar5);
      }
LAB_00010661:
      *(int *)(*(int *)&this->field_0x2c + i * 4) = c;
      i = i + 1;
      iVar5 = *(int *)&this->field_0x14;
    } while (i < iVar5);
  }
  return;
}


/* ==== multiply ==== */

/* DWARF original prototype: void multiply(keaMatrix_pcSparse_vanilla * this, MeReal * B, MeReal *
   X) */

void __thiscall
keaMatrix_pcSparse_vanilla::multiply(keaMatrix_pcSparse_vanilla *this,MeReal *B,MeReal *X)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float *pfVar5;
  float *pfVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int h;
  int g;
  int k;
  MeReal tmp [4];
  
                    /* Unresolved local var: MeReal * G@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int j@[DW_OP_reg3(EBX)] */
  iVar9 = 0;
  g = 0;
  iVar8 = *(int *)&this->field_0x14;
  if (0 < iVar8) {
    do {
      h = 0;
      tmp[3] = 0.0;
      tmp[2] = 0.0;
      tmp[1] = 0.0;
      tmp[0] = 0.0;
      k = 0;
      if (0 < iVar9) {
        do {
          pfVar6 = *(float **)(*(int *)&this->field_0x1c + (iVar9 * iVar8 + k) * 4);
          if (pfVar6 != (float *)0x0) {
            pfVar5 = X + h;
            iVar7 = 3;
            do {
              fVar1 = *pfVar5;
              fVar2 = pfVar5[1];
              pfVar5 = pfVar5 + 2;
              iVar7 = iVar7 + -2;
              tmp[0] = pfVar6[4] * fVar2 + *pfVar6 * fVar1 + tmp[0];
              tmp[1] = pfVar6[5] * fVar2 + pfVar6[1] * fVar1 + tmp[1];
              tmp[2] = pfVar6[6] * fVar2 + pfVar6[2] * fVar1 + tmp[2];
              tmp[3] = fVar2 * pfVar6[7] + pfVar6[3] * fVar1 + tmp[3];
              pfVar6 = pfVar6 + 8;
            } while (-1 < iVar7);
          }
          k = k + 1;
          h = h + 4;
        } while (k < iVar9);
      }
      if (iVar9 < iVar8) {
        pfVar6 = X + h;
        iVar7 = iVar9;
        do {
          pfVar5 = *(float **)(*(int *)&this->field_0x1c + (iVar8 * iVar7 + iVar9) * 4);
          if (pfVar5 != (float *)0x0) {
            fVar1 = *pfVar6;
            fVar2 = pfVar6[1];
            fVar3 = pfVar6[2];
            fVar4 = pfVar6[3];
            tmp[0] = pfVar5[3] * fVar4 + pfVar5[2] * fVar3 + *pfVar5 * fVar1 + pfVar5[1] * fVar2 +
                     tmp[0];
            tmp[1] = pfVar5[7] * fVar4 + pfVar5[6] * fVar3 + pfVar5[4] * fVar1 + pfVar5[5] * fVar2 +
                     tmp[1];
            tmp[2] = pfVar5[0xb] * fVar4 +
                     pfVar5[10] * fVar3 + pfVar5[8] * fVar1 + pfVar5[9] * fVar2 + tmp[2];
            tmp[3] = fVar1 * pfVar5[0xc] + fVar2 * pfVar5[0xd] + fVar3 * pfVar5[0xe] +
                     fVar4 * pfVar5[0xf] + tmp[3];
          }
          iVar7 = iVar7 + 1;
          pfVar6 = pfVar6 + 4;
        } while (iVar7 < iVar8);
      }
      iVar9 = iVar9 + 1;
      B[g] = tmp[0];
      B[g + 1] = tmp[1];
      B[g + 2] = tmp[2];
      B[g + 3] = tmp[3];
      g = g + 4;
      iVar8 = *(int *)&this->field_0x14;
    } while (iVar9 < iVar8);
  }
  iVar8 = *(int *)&this->field_0x4;
  iVar9 = iVar8;
  while( true ) {
    if (iVar8 % 0xc != 0) {
      iVar8 = (iVar8 - iVar8 % 0xc) + 0xc;
    }
    if (iVar9 == iVar8) break;
    B[iVar9] = 0.0;
    iVar9 = iVar9 + 1;
    iVar8 = *(int *)&this->field_0x4;
  }
  return;
}


/* ==== factorize ==== */

/* DWARF original prototype: void factorize(keaMatrix_pcSparse_vanilla * this) */

void __thiscall keaMatrix_pcSparse_vanilla::factorize(keaMatrix_pcSparse_vanilla *this)

{
  int iVar1;
  int iVar2;
  int w;
  int d;
  MeReal aMStack_70 [19];
  int local_24;
  int local_20;
  MeReal *ablock;
  int stride;
  int z;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int j@[DW_OP_reg3(EBX)]
                       Unresolved local var: int w@[DW_OP_reg6(ESI)] */
  ablock = aMStack_70;
  stride = *(int *)&this->field_0x14;
  z = 0;
  d = 0;
  if (0 < stride) {
    do {
      iVar2 = d + 1;
      vc_dstrip_Cholesky(ablock,*(MeReal **)&this->field_0x18,z,d,
                         *(int *)(*(int *)&this->field_0x30 + d * 4),*(MeReal ***)&this->field_0x20)
      ;
      w = z + stride;
      iVar1 = *(int *)&this->field_0x2c;
      local_24 = iVar2;
      local_20 = w;
      if (iVar2 <= *(int *)(iVar1 + d * 4)) {
        do {
          if (*(int *)(*(int *)&this->field_0x30 + iVar2 * 4) <= d) {
            vc_strip_Cholesky(ablock,*(MeReal **)&this->field_0x18,z,w,d,
                              *(int *)(*(int *)&this->field_0x30 + iVar2 * 4),
                              *(MeReal ***)&this->field_0x20,(MeReal **)&this->field_0x28);
            iVar1 = *(int *)&this->field_0x2c;
          }
          iVar2 = iVar2 + 1;
          w = w + stride;
        } while (iVar2 <= *(int *)(iVar1 + d * 4));
      }
      z = local_20;
      d = local_24;
    } while (local_24 < *(int *)&this->field_0x14);
  }
  return;
}


/* ==== solve ==== */

/* DWARF original prototype: void solve(keaMatrix_pcSparse_vanilla * this, MeReal * x, MeReal * b)
    */

void __thiscall
keaMatrix_pcSparse_vanilla::solve(keaMatrix_pcSparse_vanilla *this,MeReal *x,MeReal *b)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float *pfVar7;
  float *pfVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  int iVar14;
  int local_48;
  int t;
  int h;
  int g;
  int j;
  int i;
  MeReal tmp [4];
  float afStack_1c [3];
  
                    /* Unresolved local var: int k@[DW_OP_reg3(EBX)]
                       Unresolved local var: int r@[DW_OP_reg6(ESI)]
                       Unresolved local var: int s@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * G@[DW_OP_reg7(EDI)] */
  if ((x != b) && (i = 0, *(int *)&this->field_0x8 != 0)) {
    do {
      x[i] = b[i];
      i = i + 1;
    } while (i != *(int *)&this->field_0x8);
  }
  i = 0;
  iVar11 = *(int *)&this->field_0x14;
  if (0 < iVar11) {
    do {
      j = 0;
      iVar11 = i * iVar11;
      tmp[3] = 0.0;
      tmp[2] = 0.0;
      tmp[1] = 0.0;
      tmp[0] = 0.0;
      h = 0;
      if (i < 1) {
        local_48 = *(int *)&this->field_0x20;
      }
      else {
        local_48 = *(int *)&this->field_0x20;
        do {
          pfVar7 = *(float **)(local_48 + iVar11 * 4);
          if (pfVar7 != (float *)0x0) {
            iVar14 = 3;
            pfVar8 = x + h;
            do {
              fVar3 = *pfVar8;
              fVar4 = pfVar8[1];
              pfVar8 = pfVar8 + 2;
              iVar14 = iVar14 + -2;
              tmp[0] = pfVar7[4] * fVar4 + *pfVar7 * fVar3 + tmp[0];
              tmp[1] = pfVar7[5] * fVar4 + pfVar7[1] * fVar3 + tmp[1];
              tmp[2] = pfVar7[6] * fVar4 + pfVar7[2] * fVar3 + tmp[2];
              tmp[3] = fVar4 * pfVar7[7] + pfVar7[3] * fVar3 + tmp[3];
              pfVar7 = pfVar7 + 8;
            } while (-1 < iVar14);
          }
          j = j + 1;
          h = h + 4;
          iVar11 = iVar11 + 1;
        } while (j < i);
      }
      iVar11 = *(int *)(local_48 + iVar11 * 4);
      iVar14 = 0;
      do {
        iVar12 = h + iVar14;
        fVar3 = (x[iVar12] - tmp[iVar14]) * *(float *)(*(int *)&this->field_0x18 + iVar12 * 4);
        x[iVar12] = fVar3;
        iVar12 = iVar14 + 1;
        if (iVar12 < 4) {
          pfVar7 = (float *)(iVar11 + (iVar12 + iVar14 * 4) * 4);
          uVar13 = -iVar12 & 3;
          iVar10 = iVar12;
          if (iVar14 + 2 < 4) {
            iVar9 = iVar12;
            if (uVar13 != 0) {
              if (1 < uVar13) {
                if (2 < uVar13) {
                  fVar4 = *pfVar7;
                  pfVar7 = pfVar7 + 1;
                  tmp[iVar12] = fVar4 * fVar3 + tmp[iVar12];
                  iVar10 = iVar14 + 2;
                }
                fVar4 = *pfVar7;
                pfVar7 = pfVar7 + 1;
                tmp[iVar10] = fVar4 * fVar3 + tmp[iVar10];
                iVar10 = iVar10 + 1;
              }
              goto LAB_00010d08;
            }
          }
          else {
LAB_00010d08:
            fVar4 = *pfVar7;
            pfVar7 = pfVar7 + 1;
            tmp[iVar10] = fVar4 * fVar3 + tmp[iVar10];
            iVar9 = iVar10 + 1;
            if (3 < iVar10 + 1) goto LAB_00010d6a;
          }
          do {
            tmp[iVar9] = *pfVar7 * fVar3 + tmp[iVar9];
            tmp[iVar9 + 1] = pfVar7[1] * fVar3 + tmp[iVar9 + 1];
            iVar14 = iVar9 + 4;
            tmp[iVar9 + 2] = pfVar7[2] * fVar3 + tmp[iVar9 + 2];
            pfVar8 = pfVar7 + 3;
            pfVar7 = pfVar7 + 4;
            tmp[iVar9 + 3] = *pfVar8 * fVar3 + tmp[iVar9 + 3];
            iVar9 = iVar14;
          } while (iVar14 < 4);
        }
LAB_00010d6a:
        iVar14 = iVar12;
      } while (iVar12 < 4);
      i = i + 1;
      iVar11 = *(int *)&this->field_0x14;
    } while (i < iVar11);
  }
  i = iVar11 + -1;
  iVar14 = i * iVar11;
  if (i < 0) {
LAB_00010f7b:
    i = *(int *)&this->field_0x4;
    if (i != *(int *)&this->field_0x8) {
      do {
        x[i] = 0.0;
        i = i + 1;
      } while (i != *(int *)&this->field_0x8);
    }
    return;
  }
  do {
    j = iVar11 + -1;
    iVar12 = iVar14 + i;
    tmp[3] = 0.0;
    tmp[2] = 0.0;
    tmp[1] = 0.0;
    tmp[0] = 0.0;
    g = h;
    if (i < j) {
      local_48 = *(int *)&this->field_0x20;
      pfVar7 = x + h;
      do {
        pfVar8 = *(float **)(local_48 + iVar12 * 4);
        if (pfVar8 != (float *)0x0) {
          fVar3 = *pfVar7;
          fVar4 = pfVar7[1];
          fVar5 = pfVar7[2];
          fVar6 = pfVar7[3];
          tmp[0] = pfVar8[3] * fVar6 + pfVar8[2] * fVar5 + *pfVar8 * fVar3 + pfVar8[1] * fVar4 +
                   tmp[0];
          tmp[1] = pfVar8[7] * fVar6 + pfVar8[6] * fVar5 + pfVar8[4] * fVar3 + pfVar8[5] * fVar4 +
                   tmp[1];
          tmp[2] = pfVar8[0xb] * fVar6 + pfVar8[10] * fVar5 + pfVar8[8] * fVar3 + pfVar8[9] * fVar4
                   + tmp[2];
          tmp[3] = fVar3 * pfVar8[0xc] + fVar4 * pfVar8[0xd] + fVar5 * pfVar8[0xe] +
                   fVar6 * pfVar8[0xf] + tmp[3];
        }
        j = j + -1;
        g = g + -4;
        iVar12 = iVar12 - iVar11;
        pfVar7 = pfVar7 + -4;
      } while (i < j);
    }
    else {
      local_48 = *(int *)&this->field_0x20;
    }
    iVar11 = *(int *)(local_48 + iVar12 * 4);
    iVar12 = 3;
    do {
      iVar10 = iVar12 + 1;
      if (iVar10 < 4) {
        pfVar7 = x + g + iVar10;
        pfVar8 = (float *)(iVar11 + (iVar10 + iVar12 * 4) * 4);
        uVar13 = -iVar10 & 3;
        if (iVar12 + 2 < 4) {
          if (uVar13 != 0) {
            if (1 < uVar13) {
              if (2 < uVar13) {
                fVar3 = *pfVar7;
                pfVar7 = pfVar7 + 1;
                fVar4 = *pfVar8;
                pfVar8 = pfVar8 + 1;
                tmp[iVar12] = fVar3 * fVar4 + tmp[iVar12];
                iVar10 = iVar12 + 2;
              }
              fVar3 = *pfVar7;
              iVar10 = iVar10 + 1;
              pfVar7 = pfVar7 + 1;
              fVar4 = *pfVar8;
              pfVar8 = pfVar8 + 1;
              tmp[iVar12] = fVar3 * fVar4 + tmp[iVar12];
            }
            goto LAB_00010eee;
          }
        }
        else {
LAB_00010eee:
          fVar3 = *pfVar7;
          iVar10 = iVar10 + 1;
          pfVar7 = pfVar7 + 1;
          fVar4 = *pfVar8;
          pfVar8 = pfVar8 + 1;
          tmp[iVar12] = fVar3 * fVar4 + tmp[iVar12];
          if (3 < iVar10) goto LAB_00010f44;
        }
        do {
          iVar10 = iVar10 + 4;
          fVar3 = *pfVar7 * *pfVar8 + tmp[iVar12];
          tmp[iVar12] = fVar3;
          fVar3 = pfVar7[1] * pfVar8[1] + fVar3;
          tmp[iVar12] = fVar3;
          fVar3 = pfVar7[2] * pfVar8[2] + fVar3;
          tmp[iVar12] = fVar3;
          pfVar1 = pfVar7 + 3;
          pfVar7 = pfVar7 + 4;
          pfVar2 = pfVar8 + 3;
          pfVar8 = pfVar8 + 4;
          tmp[iVar12] = *pfVar1 * *pfVar2 + fVar3;
        } while (iVar10 < 4);
      }
LAB_00010f44:
      iVar10 = g + iVar12;
      pfVar7 = tmp + iVar12;
      iVar12 = iVar12 + -1;
      x[iVar10] = (x[iVar10] - *pfVar7) * *(float *)(*(int *)&this->field_0x18 + iVar10 * 4);
    } while (-1 < iVar12);
    i = i + -1;
    if (i < 0) goto LAB_00010f7b;
    iVar11 = *(int *)&this->field_0x14;
  } while( true );
}


/* ==== solveUnits ==== */

/* DWARF original prototype: void solveUnits(keaMatrix_pcSparse_vanilla * this, MeReal * Ainv, int *
   cached, int * clamped, int numClamped, int AinvStride) */

void __thiscall
keaMatrix_pcSparse_vanilla::solveUnits
          (keaMatrix_pcSparse_vanilla *this,MeReal *Ainv,int *cached,int *clamped,int numClamped,
          int AinvStride)

{
  float fVar1;
  float fVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  float *pfVar10;
  float *pfVar11;
  int iVar12;
  int iVar13;
  float *pfVar14;
  MeReal *pMVar15;
  int iVar16;
  int iVar17;
  int aiStack_4c [4];
  int *rp;
  MeReal *G;
  int v;
  int h;
  int g;
  int k;
  int j;
  int i;
  int num_rhs;
  int t;
  int c;
  
                    /* Unresolved local var: int w@[???]
                       Unresolved local var: int step@[???]
                       Unresolved local var: int r@[DW_OP_reg7(EDI)]
                       Unresolved local var: int s@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * res@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal * wrk@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * B@[???] */
  t = 0;
  iVar3 = -(numClamped * 4 + 0xfU & 0xfffffff0);
  c = 0;
  if (0 < numClamped) {
    if ((numClamped < 2) || ((numClamped & 1U) != 0)) {
      iVar7 = *clamped;
      if (cached[iVar7] == 0) {
        pMVar15 = Ainv + AinvStride * iVar7;
        for (uVar9 = AinvStride & 0x3fffffff; uVar9 != 0; uVar9 = uVar9 - 1) {
          *pMVar15 = 0.0;
          pMVar15 = pMVar15 + 1;
        }
        *(MeReal *)((AinvStride * 4 + 4) * iVar7 + (int)Ainv) = 1.0;
        cached[iVar7] = 1;
        *(int *)((int)aiStack_4c + iVar3) = iVar7;
        t = 1;
      }
      c = 1;
      if (numClamped < 2) goto LAB_000111d1;
    }
    do {
      iVar7 = clamped[c];
      if (cached[iVar7] == 0) {
        pMVar15 = Ainv + AinvStride * iVar7;
        for (uVar9 = AinvStride & 0x3fffffff; uVar9 != 0; uVar9 = uVar9 - 1) {
          *pMVar15 = 0.0;
          pMVar15 = pMVar15 + 1;
        }
        *(MeReal *)((AinvStride * 4 + 4) * iVar7 + (int)Ainv) = 1.0;
        cached[iVar7] = 1;
        *(int *)((int)aiStack_4c + t * 4 + iVar3) = iVar7;
        t = t + 1;
      }
      iVar7 = clamped[c + 1];
      if (cached[iVar7] == 0) {
        pMVar15 = Ainv + AinvStride * iVar7;
        for (uVar9 = AinvStride & 0x3fffffff; uVar9 != 0; uVar9 = uVar9 - 1) {
          *pMVar15 = 0.0;
          pMVar15 = pMVar15 + 1;
        }
        *(MeReal *)((AinvStride * 4 + 4) * iVar7 + (int)Ainv) = 1.0;
        cached[iVar7] = 1;
        *(int *)((int)aiStack_4c + t * 4 + iVar3) = iVar7;
        t = t + 1;
      }
      c = c + 2;
    } while (c < numClamped);
  }
LAB_000111d1:
  iVar6 = t;
  iVar7 = *(int *)&this->field_0x14;
  i = 0;
  if (0 < iVar7) {
    do {
      k = iVar7 * i;
      j = 0;
      h = 0;
      if (i < 1) {
        iVar7 = *(int *)&this->field_0x20;
      }
      else {
        iVar7 = *(int *)&this->field_0x20;
        do {
          pfVar10 = *(float **)(iVar7 + k * 4);
          if ((pfVar10 != (float *)0x0) && (t = 0, 0 < iVar6)) {
            do {
              iVar7 = *(int *)((int)aiStack_4c + t * 4 + iVar3);
              if (iVar7 >> 2 <= j) {
                iVar7 = AinvStride * iVar7;
                iVar8 = iVar7 + h;
                iVar16 = 0;
                pfVar11 = Ainv + iVar7 + i * 4;
                pfVar14 = pfVar10;
                do {
                  iVar7 = iVar16 + 1;
                  fVar1 = *pfVar11 - Ainv[iVar8 + iVar16] * *pfVar14;
                  *pfVar11 = fVar1;
                  fVar2 = pfVar11[1] - Ainv[iVar8 + iVar16] * pfVar14[1];
                  pfVar11[1] = fVar2;
                  fVar4 = pfVar11[2] - Ainv[iVar8 + iVar16] * pfVar14[2];
                  pfVar11[2] = fVar4;
                  iVar17 = iVar8 + iVar16;
                  iVar16 = iVar16 + 2;
                  fVar5 = pfVar11[3] - Ainv[iVar17] * pfVar14[3];
                  pfVar11[3] = fVar5;
                  *pfVar11 = fVar1 - Ainv[iVar8 + iVar7] * pfVar14[4];
                  pfVar11[1] = fVar2 - Ainv[iVar8 + iVar7] * pfVar14[5];
                  pfVar11[2] = fVar4 - Ainv[iVar8 + iVar7] * pfVar14[6];
                  pfVar11[3] = fVar5 - Ainv[iVar8 + iVar7] * pfVar14[7];
                  pfVar14 = pfVar14 + 8;
                } while (iVar16 < 4);
              }
              t = t + 1;
            } while (t < iVar6);
            iVar7 = *(int *)&this->field_0x20;
          }
          j = j + 1;
          k = k + 1;
          h = h + 4;
        } while (j < i);
      }
      iVar7 = *(int *)(iVar7 + k * 4);
      t = 0;
      if (0 < iVar6) {
        do {
          iVar8 = *(int *)((int)aiStack_4c + t * 4 + iVar3);
          if (iVar8 >> 2 <= i) {
            iVar8 = AinvStride * iVar8 + i * 4;
            iVar16 = 0;
            do {
              iVar17 = iVar16 + 1;
              fVar1 = *(float *)(*(int *)&this->field_0x18 + (h + iVar16) * 4) *
                      Ainv[iVar8 + iVar16];
              Ainv[iVar8 + iVar16] = fVar1;
              if (iVar17 < 4) {
                pfVar10 = (float *)(iVar7 + (iVar17 + iVar16 * 4) * 4);
                uVar9 = -iVar17 & 3;
                iVar12 = iVar17;
                if (iVar16 + 2 < 4) {
                  if (uVar9 != 0) {
                    if (1 < uVar9) {
                      iVar12 = iVar17;
                      if (2 < uVar9) {
                        fVar2 = *pfVar10;
                        pfVar10 = pfVar10 + 1;
                        Ainv[iVar8 + iVar17] = Ainv[iVar8 + iVar17] - fVar1 * fVar2;
                        iVar12 = iVar16 + 2;
                      }
                      fVar1 = *pfVar10;
                      pfVar10 = pfVar10 + 1;
                      Ainv[iVar8 + iVar12] = Ainv[iVar8 + iVar12] - Ainv[iVar8 + iVar16] * fVar1;
                      iVar12 = iVar12 + 1;
                    }
                    goto LAB_000113a7;
                  }
                }
                else {
LAB_000113a7:
                  fVar1 = *pfVar10;
                  pfVar10 = pfVar10 + 1;
                  Ainv[iVar8 + iVar12] = Ainv[iVar8 + iVar12] - Ainv[iVar8 + iVar16] * fVar1;
                  iVar12 = iVar12 + 1;
                  if (3 < iVar12) goto LAB_00011403;
                }
                do {
                  Ainv[iVar8 + iVar12] = Ainv[iVar8 + iVar12] - Ainv[iVar8 + iVar16] * *pfVar10;
                  Ainv[iVar8 + iVar12 + 1] =
                       Ainv[iVar8 + iVar12 + 1] - Ainv[iVar8 + iVar16] * pfVar10[1];
                  Ainv[iVar8 + iVar12 + 2] =
                       Ainv[iVar8 + iVar12 + 2] - Ainv[iVar8 + iVar16] * pfVar10[2];
                  iVar13 = iVar12 + 4;
                  pfVar11 = pfVar10 + 3;
                  pfVar10 = pfVar10 + 4;
                  Ainv[iVar8 + iVar12 + 3] =
                       Ainv[iVar8 + iVar12 + 3] - Ainv[iVar8 + iVar16] * *pfVar11;
                  iVar12 = iVar13;
                } while (iVar13 < 4);
              }
LAB_00011403:
              iVar16 = iVar17;
            } while (iVar17 < 4);
          }
          t = t + 1;
        } while (t < iVar6);
      }
      i = i + 1;
      iVar7 = *(int *)&this->field_0x14;
    } while (i < iVar7);
  }
  i = iVar7 + -1;
  iVar8 = i * iVar7;
  if (i < 0) {
    return;
  }
  do {
    j = iVar7 + -1;
    k = iVar8 + i;
    g = h;
    if (i < j) {
      iVar16 = *(int *)&this->field_0x20;
      do {
        pfVar10 = *(float **)(iVar16 + k * 4);
        if ((pfVar10 != (float *)0x0) && (t = 0, 0 < iVar6)) {
          do {
            iVar7 = AinvStride * *(int *)((int)aiStack_4c + t * 4 + iVar3);
            pfVar11 = Ainv + iVar7 + g;
            pfVar14 = Ainv + iVar7 + i * 4;
            *pfVar14 = *pfVar14 -
                       (pfVar11[3] * pfVar10[3] +
                       pfVar11[2] * pfVar10[2] + pfVar11[1] * pfVar10[1] + *pfVar11 * *pfVar10);
            pfVar14[1] = pfVar14[1] -
                         (pfVar11[3] * pfVar10[7] +
                         pfVar11[2] * pfVar10[6] + pfVar11[1] * pfVar10[5] + *pfVar11 * pfVar10[4]);
            pfVar14[2] = pfVar14[2] -
                         (pfVar11[3] * pfVar10[0xb] +
                         pfVar11[2] * pfVar10[10] + pfVar11[1] * pfVar10[9] + *pfVar11 * pfVar10[8])
            ;
            pfVar14[3] = pfVar14[3] -
                         (pfVar11[3] * pfVar10[0xf] +
                         pfVar11[2] * pfVar10[0xe] +
                         pfVar11[1] * pfVar10[0xd] + *pfVar11 * pfVar10[0xc]);
            t = t + 1;
          } while (t < iVar6);
          iVar7 = *(int *)&this->field_0x14;
          iVar16 = *(int *)&this->field_0x20;
        }
        j = j + -1;
        g = g + -4;
        k = k - iVar7;
      } while (i < j);
    }
    else {
      iVar16 = *(int *)&this->field_0x20;
    }
    iVar7 = *(int *)(iVar16 + k * 4);
    t = 0;
    if (0 < iVar6) {
      do {
        iVar17 = 3;
        iVar16 = AinvStride * *(int *)((int)aiStack_4c + t * 4 + iVar3) + i * 4;
        pfVar10 = Ainv + iVar16 + 3;
        do {
          iVar12 = iVar17 + 1;
          if (iVar12 < 4) {
            pfVar11 = (float *)(iVar7 + (iVar12 + iVar17 * 4) * 4);
            uVar9 = -iVar12 & 3;
            if (iVar17 + 2 < 4) {
              if (uVar9 != 0) {
                if (1 < uVar9) {
                  iVar13 = iVar12;
                  if (2 < uVar9) {
                    fVar1 = *pfVar11;
                    pfVar11 = pfVar11 + 1;
                    *pfVar10 = *pfVar10 - Ainv[iVar16 + iVar12] * fVar1;
                    iVar13 = iVar17 + 2;
                  }
                  iVar12 = iVar13 + 1;
                  fVar1 = *pfVar11;
                  pfVar11 = pfVar11 + 1;
                  *pfVar10 = *pfVar10 - Ainv[iVar16 + iVar13] * fVar1;
                }
                goto LAB_00011616;
              }
            }
            else {
LAB_00011616:
              iVar13 = iVar16 + iVar12;
              iVar12 = iVar12 + 1;
              fVar1 = *pfVar11;
              pfVar11 = pfVar11 + 1;
              *pfVar10 = *pfVar10 - Ainv[iVar13] * fVar1;
              if (3 < iVar12) goto LAB_0001165d;
            }
            do {
              fVar1 = *pfVar10 - Ainv[iVar16 + iVar12] * *pfVar11;
              *pfVar10 = fVar1;
              fVar1 = fVar1 - Ainv[iVar16 + iVar12 + 1] * pfVar11[1];
              *pfVar10 = fVar1;
              fVar1 = fVar1 - Ainv[iVar16 + iVar12 + 2] * pfVar11[2];
              *pfVar10 = fVar1;
              iVar13 = iVar12 + 3;
              iVar12 = iVar12 + 4;
              pfVar14 = pfVar11 + 3;
              pfVar11 = pfVar11 + 4;
              *pfVar10 = fVar1 - Ainv[iVar16 + iVar13] * *pfVar14;
            } while (iVar12 < 4);
          }
LAB_0001165d:
          *pfVar10 = *(float *)(*(int *)&this->field_0x18 + (g + iVar17) * 4) * *pfVar10;
          pfVar10 = pfVar10 + -1;
          iVar17 = iVar17 + -1;
        } while (-1 < iVar17);
        t = t + 1;
      } while (t < iVar6);
    }
    i = i + -1;
    if (i < 0) {
      return;
    }
    iVar7 = *(int *)&this->field_0x14;
  } while( true );
}


/* ==== Multiply46BlockBy46BlockTranspose ==== */

void Multiply46BlockBy46BlockTranspose
               (MeReal *acholblock,MeReal *jmblock,MeReal *jblock,int notzero)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float *pfVar9;
  MeReal *pMVar10;
  int iVar11;
  int iVar12;
  MeReal tmp [4];
  
  if (notzero == 0) {
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
    fVar8 = 0.0;
    iVar12 = 0;
    do {
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)] */
      pfVar9 = jmblock + iVar12;
      iVar11 = 5;
      pMVar10 = jblock;
      fVar4 = fVar8;
      fVar5 = fVar8;
      fVar6 = fVar8;
      fVar7 = fVar8;
      do {
        fVar1 = *pfVar9;
        fVar2 = pfVar9[4];
        fVar3 = pfVar9[8];
        pfVar9 = pfVar9 + 0xc;
        iVar11 = iVar11 + -3;
        fVar7 = fVar3 * pMVar10[8] + fVar2 * pMVar10[4] + fVar1 * *pMVar10 + fVar7;
        fVar6 = fVar3 * pMVar10[9] + fVar2 * pMVar10[5] + fVar1 * pMVar10[1] + fVar6;
        fVar4 = fVar4 + fVar1 * pMVar10[3] + fVar2 * pMVar10[7] + fVar3 * pMVar10[0xb];
        fVar5 = fVar5 + fVar1 * pMVar10[2] + fVar2 * pMVar10[6] + fVar3 * pMVar10[10];
        pMVar10 = pMVar10 + 0xc;
      } while (-1 < iVar11);
      iVar12 = iVar12 + 1;
      acholblock[1] = fVar6;
      *acholblock = fVar7;
      acholblock[2] = fVar5;
      acholblock[3] = fVar4;
      acholblock = acholblock + 4;
    } while (iVar12 < 4);
  }
  else {
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
    fVar8 = 0.0;
    iVar12 = 0;
    do {
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)] */
      pfVar9 = jmblock + iVar12;
      iVar11 = 5;
      pMVar10 = jblock;
      fVar4 = fVar8;
      fVar5 = fVar8;
      fVar6 = fVar8;
      fVar7 = fVar8;
      do {
        fVar1 = *pfVar9;
        fVar2 = pfVar9[4];
        fVar3 = pfVar9[8];
        pfVar9 = pfVar9 + 0xc;
        iVar11 = iVar11 + -3;
        fVar7 = fVar3 * pMVar10[8] + fVar2 * pMVar10[4] + fVar1 * *pMVar10 + fVar7;
        fVar6 = fVar3 * pMVar10[9] + fVar2 * pMVar10[5] + fVar1 * pMVar10[1] + fVar6;
        fVar4 = fVar4 + fVar1 * pMVar10[3] + fVar2 * pMVar10[7] + fVar3 * pMVar10[0xb];
        fVar5 = fVar5 + fVar1 * pMVar10[2] + fVar2 * pMVar10[6] + fVar3 * pMVar10[10];
        pMVar10 = pMVar10 + 0xc;
      } while (-1 < iVar11);
      iVar12 = iVar12 + 1;
      *acholblock = fVar7 + *acholblock;
      acholblock[1] = fVar6 + acholblock[1];
      acholblock[2] = fVar5 + acholblock[2];
      acholblock[3] = fVar4 + acholblock[3];
      acholblock = acholblock + 4;
    } while (iVar12 < 4);
  }
  return;
}


/* ==== vc_strip_Cholesky ==== */

void vc_strip_Cholesky(MeReal *tmp,MeReal *rsD,int z,int w,int d,int s,MeReal **NZ,MeReal **mcLPptr)

{
  uint uVar1;
  int iVar2;
  MeReal *pMVar3;
  float *pfVar4;
  int iVar5;
  uint uVar6;
  MeReal *pMVar7;
  float *pfVar8;
  MeReal *b;
  int p;
  int t;
  int r;
  int i;
  
  pMVar7 = tmp;
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)]
                       Unresolved local var: int n@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeReal * a@[DW_OP_reg3(EBX)] */
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *pMVar7 = 0.0;
    pMVar7 = pMVar7 + 1;
  }
  t = z + s;
  i = d - s;
  p = s + w;
  if (0 < i) {
    do {
      pMVar7 = NZ[t];
      pfVar8 = NZ[p];
      if ((pMVar7 != (MeReal *)0x0) && (pfVar8 != (float *)0x0)) {
        r = 0;
        do {
          iVar2 = 0;
          pMVar3 = tmp;
          do {
            iVar5 = r * 4 + iVar2;
            *pMVar3 = *pfVar8 * pMVar7[iVar5] + *pMVar3;
            pMVar3[1] = pfVar8[1] * pMVar7[iVar5] + pMVar3[1];
            pMVar3[2] = pfVar8[2] * pMVar7[iVar5] + pMVar3[2];
            pMVar3[3] = pfVar8[3] * pMVar7[iVar5] + pMVar3[3];
            iVar5 = r * 4 + 1 + iVar2;
            iVar2 = iVar2 + 2;
            pMVar3[4] = *pfVar8 * pMVar7[iVar5] + pMVar3[4];
            pMVar3[5] = pfVar8[1] * pMVar7[iVar5] + pMVar3[5];
            pMVar3[6] = pfVar8[2] * pMVar7[iVar5] + pMVar3[6];
            pMVar3[7] = pfVar8[3] * pMVar7[iVar5] + pMVar3[7];
            pMVar3 = pMVar3 + 8;
          } while (iVar2 < 4);
          pfVar8 = pfVar8 + 4;
          r = r + 1;
        } while (r < 4);
      }
      t = t + 1;
      p = p + 1;
      i = i + -1;
    } while (i != 0);
  }
  pMVar7 = NZ[p];
  pMVar3 = NZ[t];
  if (pMVar7 == (MeReal *)0x0) {
    pMVar7 = *mcLPptr;
    *mcLPptr = pMVar7 + 0x10;
    uVar6 = 0;
    do {
      iVar2 = uVar6 + d * 4;
      pMVar7[uVar6 * 4] = -tmp[uVar6 * 4] * rsD[iVar2];
      pfVar8 = pMVar7 + uVar6 * 4;
      pfVar8[1] = -tmp[uVar6 * 4 + 1] * rsD[iVar2];
      pfVar8[2] = -tmp[uVar6 * 4 + 2] * rsD[iVar2];
      uVar1 = uVar6 + 1;
      pfVar8[3] = -tmp[uVar6 * 4 + 3] * rsD[iVar2];
      if ((int)uVar1 < 4) {
        pfVar4 = tmp + uVar1 * 4;
        iVar2 = uVar6 * 4;
        if ((3 < (int)(uVar6 + 2)) || (r = uVar1, (uVar1 & 1) != 0)) {
          iVar5 = iVar2 + uVar1;
          *pfVar4 = *pfVar8 * pMVar3[iVar5] + *pfVar4;
          pfVar4[1] = pfVar8[1] * pMVar3[iVar5] + pfVar4[1];
          pfVar4[2] = pfVar8[2] * pMVar3[iVar5] + pfVar4[2];
          pfVar4[3] = pfVar8[3] * pMVar3[iVar5] + pfVar4[3];
          pfVar4 = pfVar4 + 4;
          r = uVar6 + 2;
          if (3 < (int)(uVar6 + 2)) goto LAB_00011d93;
        }
        do {
          iVar5 = iVar2 + r;
          *pfVar4 = pMVar7[uVar6 * 4] * pMVar3[iVar5] + *pfVar4;
          pfVar4[1] = pfVar8[1] * pMVar3[iVar5] + pfVar4[1];
          pfVar4[2] = pfVar8[2] * pMVar3[iVar5] + pfVar4[2];
          pfVar4[3] = pfVar8[3] * pMVar3[iVar5] + pfVar4[3];
          iVar5 = iVar2 + 1 + r;
          pfVar4[4] = pMVar7[uVar6 * 4] * pMVar3[iVar5] + pfVar4[4];
          pfVar4[5] = pfVar8[1] * pMVar3[iVar5] + pfVar4[5];
          pfVar4[6] = pfVar8[2] * pMVar3[iVar5] + pfVar4[6];
          pfVar4[7] = pfVar8[3] * pMVar3[iVar5] + pfVar4[7];
          r = r + 2;
          pfVar4 = pfVar4 + 8;
        } while (r < 4);
      }
LAB_00011d93:
      uVar6 = uVar1;
    } while ((int)uVar1 < 4);
    NZ[p] = pMVar7;
  }
  else {
    uVar6 = 0;
    do {
      iVar2 = uVar6 + d * 4;
      pfVar8 = pMVar7 + uVar6 * 4;
      pMVar7[uVar6 * 4] = (pMVar7[uVar6 * 4] - tmp[uVar6 * 4]) * rsD[iVar2];
      pfVar8[1] = (pfVar8[1] - tmp[uVar6 * 4 + 1]) * rsD[iVar2];
      pfVar8[2] = (pfVar8[2] - tmp[uVar6 * 4 + 2]) * rsD[iVar2];
      uVar1 = uVar6 + 1;
      pfVar8[3] = (pfVar8[3] - tmp[uVar6 * 4 + 3]) * rsD[iVar2];
      if ((int)uVar1 < 4) {
        pfVar4 = tmp + uVar1 * 4;
        iVar2 = uVar6 * 4;
        if ((3 < (int)(uVar6 + 2)) || (r = uVar1, (uVar1 & 1) != 0)) {
          iVar5 = iVar2 + uVar1;
          *pfVar4 = *pfVar8 * pMVar3[iVar5] + *pfVar4;
          pfVar4[1] = pfVar8[1] * pMVar3[iVar5] + pfVar4[1];
          pfVar4[2] = pfVar8[2] * pMVar3[iVar5] + pfVar4[2];
          pfVar4[3] = pfVar8[3] * pMVar3[iVar5] + pfVar4[3];
          pfVar4 = pfVar4 + 4;
          r = uVar6 + 2;
          if (3 < (int)(uVar6 + 2)) goto LAB_00011be1;
        }
        do {
          iVar5 = iVar2 + r;
          *pfVar4 = pMVar7[uVar6 * 4] * pMVar3[iVar5] + *pfVar4;
          pfVar4[1] = pfVar8[1] * pMVar3[iVar5] + pfVar4[1];
          pfVar4[2] = pfVar8[2] * pMVar3[iVar5] + pfVar4[2];
          pfVar4[3] = pfVar8[3] * pMVar3[iVar5] + pfVar4[3];
          iVar5 = iVar2 + 1 + r;
          pfVar4[4] = pMVar7[uVar6 * 4] * pMVar3[iVar5] + pfVar4[4];
          pfVar4[5] = pfVar8[1] * pMVar3[iVar5] + pfVar4[5];
          pfVar4[6] = pfVar8[2] * pMVar3[iVar5] + pfVar4[6];
          pfVar4[7] = pfVar8[3] * pMVar3[iVar5] + pfVar4[7];
          r = r + 2;
          pfVar4 = pfVar4 + 8;
        } while (r < 4);
      }
LAB_00011be1:
      uVar6 = uVar1;
    } while ((int)uVar1 < 4);
  }
  return;
}


/* ==== vc_dstrip_Cholesky ==== */

void vc_dstrip_Cholesky(MeReal *tmp,MeReal *rsD,int z,int d,int s,MeReal **NZ)

{
  float *pfVar1;
  float fVar2;
  float fVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  float *pfVar9;
  int iVar10;
  MeReal *pMVar11;
  MeReal *MB;
  int t;
  int r;
  int j;
  int i;
  
  pMVar11 = tmp;
                    /* Unresolved local var: int c@[DW_OP_reg1(ECX)]
                       Unresolved local var: int n@[DW_OP_reg0(EAX)]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
  for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
    *pMVar11 = 0.0;
    pMVar11 = pMVar11 + 1;
  }
  i = d - s;
  t = s + z;
  if (0 < i) {
    do {
      pMVar11 = NZ[t];
      if (pMVar11 != (MeReal *)0x0) {
        r = 0;
        do {
          j = 0;
          do {
            if (j < 4) {
              iVar10 = j * 4;
              iVar5 = j + r * 4;
              pfVar9 = pMVar11 + r * 4 + j;
              uVar4 = -j & 3;
              iVar8 = j;
              if (j + 1 < 4) {
                iVar6 = j;
                if (uVar4 != 0) {
                  if (1 < uVar4) {
                    if (2 < uVar4) {
                      fVar2 = *pfVar9;
                      pfVar9 = pfVar9 + 1;
                      tmp[j * 5] = fVar2 * pMVar11[iVar5] + tmp[j * 5];
                      iVar6 = j + 1;
                    }
                    iVar8 = iVar6 + 1;
                    fVar2 = *pfVar9;
                    pfVar9 = pfVar9 + 1;
                    tmp[iVar6 + iVar10] = fVar2 * pMVar11[iVar5] + tmp[iVar6 + iVar10];
                  }
                  goto LAB_00011e59;
                }
              }
              else {
LAB_00011e59:
                iVar6 = iVar8 + 1;
                fVar2 = *pfVar9;
                pfVar9 = pfVar9 + 1;
                tmp[iVar8 + iVar10] = fVar2 * pMVar11[iVar5] + tmp[iVar8 + iVar10];
                if (3 < iVar6) goto LAB_00011eed;
              }
              do {
                tmp[iVar6 + iVar10] = *pfVar9 * pMVar11[iVar5] + tmp[iVar6 + iVar10];
                iVar8 = iVar10 + 1 + iVar6;
                tmp[iVar8] = pfVar9[1] * pMVar11[iVar5] + tmp[iVar8];
                iVar8 = iVar10 + 2 + iVar6;
                tmp[iVar8] = pfVar9[2] * pMVar11[iVar5] + tmp[iVar8];
                iVar8 = iVar10 + 3 + iVar6;
                iVar6 = iVar6 + 4;
                pfVar1 = pfVar9 + 3;
                pfVar9 = pfVar9 + 4;
                tmp[iVar8] = *pfVar1 * pMVar11[iVar5] + tmp[iVar8];
              } while (iVar6 < 4);
            }
LAB_00011eed:
            j = j + 1;
          } while (j < 4);
          r = r + 1;
        } while (r < 4);
      }
      t = t + 1;
      i = i + -1;
    } while (i != 0);
  }
  pMVar11 = NZ[t];
  j = 0;
  do {
    iVar8 = j * 4;
    iVar5 = j + d * 4;
    fVar2 = tmp[j * 5];
    fVar3 = pMVar11[j * 5];
    pMVar11[j * 5] = SQRT(fVar3 - fVar2);
    fVar2 = 1.0 / SQRT(fVar3 - fVar2);
    rsD[iVar5] = fVar2;
    iVar10 = j + 1;
    if (iVar10 < 4) {
      uVar4 = -iVar10 & 3;
      r = iVar10;
      if (j + 2 < 4) {
        if (uVar4 == 0) goto LAB_00011fb3;
        if (1 < uVar4) {
          if (2 < uVar4) {
            iVar6 = iVar10 + iVar8;
            pMVar11[iVar6] = (pMVar11[iVar6] - tmp[iVar6]) * fVar2;
            r = j + 2;
          }
          iVar6 = r + iVar8;
          pMVar11[iVar6] = (pMVar11[iVar6] - tmp[iVar6]) * rsD[iVar5];
          r = r + 1;
        }
      }
      iVar6 = r + iVar8;
      pMVar11[iVar6] = (pMVar11[iVar6] - tmp[iVar6]) * rsD[iVar5];
      for (r = r + 1; r < 4; r = r + 4) {
LAB_00011fb3:
        iVar6 = r + iVar8;
        pMVar11[iVar6] = (pMVar11[iVar6] - tmp[iVar6]) * rsD[iVar5];
        iVar6 = iVar8 + 1 + r;
        pMVar11[iVar6] = (pMVar11[iVar6] - tmp[iVar6]) * rsD[iVar5];
        iVar6 = iVar8 + 2 + r;
        pMVar11[iVar6] = (pMVar11[iVar6] - tmp[iVar6]) * rsD[iVar5];
        iVar6 = iVar8 + 3 + r;
        pMVar11[iVar6] = (pMVar11[iVar6] - tmp[iVar6]) * rsD[iVar5];
      }
      if (iVar10 < 4) {
        r = iVar10;
        do {
          if (r < 4) {
            iVar6 = r * 4;
            iVar5 = r + j * 4;
            pfVar9 = pMVar11 + j * 4 + r;
            uVar4 = -r & 3;
            iVar8 = r;
            if (r + 1 < 4) {
              iVar7 = r;
              if (uVar4 != 0) {
                if (1 < uVar4) {
                  if (2 < uVar4) {
                    fVar2 = *pfVar9;
                    pfVar9 = pfVar9 + 1;
                    tmp[r * 5] = fVar2 * pMVar11[iVar5] + tmp[r * 5];
                    iVar7 = r + 1;
                  }
                  iVar8 = iVar7 + 1;
                  fVar2 = *pfVar9;
                  pfVar9 = pfVar9 + 1;
                  tmp[iVar7 + iVar6] = fVar2 * pMVar11[iVar5] + tmp[iVar7 + iVar6];
                }
                goto LAB_00012084;
              }
            }
            else {
LAB_00012084:
              fVar2 = *pfVar9;
              pfVar9 = pfVar9 + 1;
              tmp[iVar8 + iVar6] = fVar2 * pMVar11[iVar5] + tmp[iVar8 + iVar6];
              iVar7 = iVar8 + 1;
              if (3 < iVar8 + 1) goto LAB_00012111;
            }
            do {
              tmp[iVar7 + iVar6] = *pfVar9 * pMVar11[iVar5] + tmp[iVar7 + iVar6];
              iVar8 = iVar6 + 1 + iVar7;
              tmp[iVar8] = pfVar9[1] * pMVar11[iVar5] + tmp[iVar8];
              iVar8 = iVar6 + 2 + iVar7;
              tmp[iVar8] = pfVar9[2] * pMVar11[iVar5] + tmp[iVar8];
              iVar8 = iVar6 + 3 + iVar7;
              iVar7 = iVar7 + 4;
              pfVar1 = pfVar9 + 3;
              pfVar9 = pfVar9 + 4;
              tmp[iVar8] = *pfVar1 * pMVar11[iVar5] + tmp[iVar8];
            } while (iVar7 < 4);
          }
LAB_00012111:
          r = r + 1;
        } while (r < 4);
      }
    }
    j = iVar10;
    if (3 < iVar10) {
      return;
    }
  } while( true );
}


