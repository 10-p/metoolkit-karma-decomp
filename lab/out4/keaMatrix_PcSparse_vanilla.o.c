/* ==== allocate ==== */

/* DWARF original prototype: void allocate(keaMatrix_pcSparse_vanilla * this, int size) */

void keaMatrix_pcSparse_vanilla::allocate(int size)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  int in_stack_00000008;
  
  *(int *)(size + 4) = in_stack_00000008;
  iVar3 = in_stack_00000008 + 3 >> 2;
  *(int *)(size + 0x14) = iVar3;
  *(int *)(size + 8) = iVar3 << 2;
  iVar3 = keaPoolAlloc(*(int *)(size + 8) * 4 + 0x10,"rsD");
  *(uint *)(size + 0x18) = iVar3 + 0xfU & 0xfffffff0;
  iVar3 = keaPoolAlloc(*(int *)(size + 8) * *(int *)(size + 8) * 4 + 0x10,"A");
  *(uint *)(size + 0xc) = iVar3 + 0xfU & 0xfffffff0;
  iVar3 = keaPoolAlloc(*(int *)(size + 8) * *(int *)(size + 8) * 4 + 0x10,"Achol");
  *(undefined4 *)(size + 0x24) = *(undefined4 *)(size + 0xc);
  uVar1 = iVar3 + 0xfU & 0xfffffff0;
  *(uint *)(size + 0x10) = uVar1;
  *(uint *)(size + 0x28) = uVar1;
  uVar2 = keaPoolAlloc((*(int *)(size + 0x14) * *(int *)(size + 0x14) + 0xfU & 0xfffffff0) << 2,
                       "NAZ");
  *(undefined4 *)(size + 0x1c) = uVar2;
  uVar2 = keaPoolAlloc((*(int *)(size + 0x14) * *(int *)(size + 0x14) + 0xfU & 0xfffffff0) << 2,
                       "NCZ");
  *(undefined4 *)(size + 0x20) = uVar2;
  uVar2 = keaPoolAlloc((*(int *)(size + 0x14) + 0xfU & 0xfffffff0) << 2,"NR");
  *(undefined4 *)(size + 0x2c) = uVar2;
  uVar2 = keaPoolAlloc((*(int *)(size + 0x14) + 0xfU & 0xfffffff0) << 2,"NC");
  *(undefined4 *)(size + 0x30) = uVar2;
  return;
}


/* ==== makeFromJMJT ==== */

/* DWARF original prototype: void makeFromJMJT(keaMatrix_pcSparse_vanilla * this, MeReal * JM,
   MeReal * Js, int * num_in_strip, int * block2body, MeReal * slipfactor, MeReal epsilon, MeReal
   hinv) */

void keaMatrix_pcSparse_vanilla::makeFromJMJT
               (MeReal *JM,MeReal *Js,int *num_in_strip,int *block2body,MeReal *slipfactor,
               MeReal epsilon,MeReal hinv)

{
  int *piVar1;
  MeReal MVar2;
  uint uVar3;
  MeReal MVar4;
  int iVar5;
  int iVar6;
  MeReal *pMVar7;
  uint uVar8;
  int iVar9;
  float in_stack_00000020;
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
  MVar4 = JM[5];
  Jb = (int *)slipfactor;
  J = (MeReal *)num_in_strip;
  AMatrix = (MeReal *)JM[9];
  AcholMatrix = (MeReal *)JM[10];
  strip = 0;
  z = 0;
  step = (int)MVar4;
  if (0 < (int)MVar4) {
    do {
      step = step + -1;
      jm = 0;
      jmptr = Js;
      JMb = (int *)slipfactor;
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
          iVar6 = 0;
          block = block2body[jm];
          uVar8 = block2body[strip];
          if (0 < block) {
            do {
              iVar5 = *JMblock2bodystrip;
              if ((iVar5 != -1) && (iVar9 = 0, 0 < (int)uVar8)) {
                uVar3 = uVar8 & 3;
                pMVar7 = J;
                if ((int)uVar8 < 2) {
LAB_000101d8:
                  if (iVar5 == Jb[iVar9]) {
                    Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar7,iVar6);
                    iVar6 = 1;
                  }
                  iVar9 = iVar9 + 1;
                  pMVar7 = pMVar7 + 0x18;
                  if ((int)uVar8 <= iVar9) goto LAB_00010240;
                }
                else if (uVar3 != 0) {
                  if (1 < uVar3) {
                    pMVar7 = J;
                    if (2 < uVar3) {
                      if (iVar5 == *Jb) {
                        Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,J,iVar6);
                        iVar6 = 1;
                      }
                      pMVar7 = J + 0x18;
                      iVar9 = 1;
                    }
                    if (iVar5 == Jb[iVar9]) {
                      Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar7,iVar6);
                      iVar6 = 1;
                    }
                    pMVar7 = pMVar7 + 0x18;
                    iVar9 = iVar9 + 1;
                  }
                  goto LAB_000101d8;
                }
                do {
                  if (iVar5 == Jb[iVar9]) {
                    Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar7,iVar6);
                    iVar6 = 1;
                  }
                  if (iVar5 == Jb[iVar9 + 1]) {
                    Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar7 + 0x18,iVar6);
                    iVar6 = 1;
                  }
                  if (iVar5 == Jb[iVar9 + 2]) {
                    Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar7 + 0x30,iVar6);
                    iVar6 = 1;
                  }
                  if (iVar5 == Jb[iVar9 + 3]) {
                    Multiply46BlockBy46BlockTranspose(AcholMatrix,jmstrip,pMVar7 + 0x48,iVar6);
                    iVar6 = 1;
                  }
                  iVar9 = iVar9 + 4;
                  pMVar7 = pMVar7 + 0x60;
                } while (iVar9 < (int)uVar8);
              }
LAB_00010240:
              jmstrip = jmstrip + 0x18;
              JMblock2bodystrip = JMblock2bodystrip + 1;
              block = block + -1;
            } while (block != 0);
          }
          if (iVar6 == 0) {
            *(undefined4 *)((int)JM[7] + z * 4) = 0;
            *(undefined4 *)((int)JM[8] + z * 4) = 0;
          }
          else {
                    /* Unresolved local var: int d@[DW_OP_reg0(EAX)] */
            *(MeReal **)((int)JM[7] + z * 4) = AMatrix;
            *(MeReal **)((int)JM[8] + z * 4) = AcholMatrix;
            iVar6 = 0;
            do {
              AMatrix[iVar6] = AcholMatrix[iVar6];
              AMatrix[iVar6 + 1] = AcholMatrix[iVar6 + 1];
              AMatrix[iVar6 + 2] = AcholMatrix[iVar6 + 2];
              AMatrix[iVar6 + 3] = AcholMatrix[iVar6 + 3];
              AMatrix[iVar6 + 4] = AcholMatrix[iVar6 + 4];
              AMatrix[iVar6 + 5] = AcholMatrix[iVar6 + 5];
              AMatrix[iVar6 + 6] = AcholMatrix[iVar6 + 6];
              iVar5 = iVar6 + 7;
              iVar6 = iVar6 + 8;
              AMatrix[iVar5] = AcholMatrix[iVar5];
            } while (iVar6 < 0x10);
            AMatrix = AMatrix + 0x10;
            uVar8 = k - jm >> 0x1f;
            AcholMatrix = AcholMatrix + 0x10;
            k = ((uVar8 ^ 0xffffffff) & jm) + (k & uVar8);
          }
          JMb = JMb + 8;
          piVar1 = block2body + jm;
          z = z + 1;
          jm = jm + 1;
          jmptr = jmptr + *piVar1 * 0x18;
        } while (jm <= strip);
      }
      *(int *)((int)JM[0xc] + strip * 4) = k;
      piVar1 = block2body + strip;
      Jb = Jb + 8;
      z = z + step;
      strip = strip + 1;
      J = J + *piVar1 * 0x18;
      MVar4 = JM[5];
    } while (strip < (int)MVar4);
  }
  JM[9] = (MeReal)AMatrix;
  JM[10] = (MeReal)AcholMatrix;
  i = 0;
  k = 0;
  if (0 < (int)MVar4) {
    do {
      iVar6 = (int)MVar4 * i + i;
      AMatrix = *(MeReal **)((int)JM[7] + iVar6 * 4);
      AcholMatrix = *(MeReal **)((int)JM[8] + iVar6 * 4);
      *AMatrix = in_stack_00000020 * *(float *)((int)epsilon + k * 4) + hinv + *AMatrix;
      *AcholMatrix = in_stack_00000020 * *(float *)((int)epsilon + k * 4) + hinv + *AcholMatrix;
      iVar6 = k + 2;
      AMatrix[5] = in_stack_00000020 * *(float *)((int)epsilon + (k + 1) * 4) + hinv + AMatrix[5];
      AcholMatrix[5] =
           in_stack_00000020 * *(float *)((int)epsilon + (k + 1) * 4) + hinv + AcholMatrix[5];
      iVar5 = k + 3;
      k = k + 4;
      AMatrix[10] = in_stack_00000020 * *(float *)((int)epsilon + iVar6 * 4) + hinv + AMatrix[10];
      AcholMatrix[10] =
           in_stack_00000020 * *(float *)((int)epsilon + iVar6 * 4) + hinv + AcholMatrix[10];
      AMatrix[0xf] = in_stack_00000020 * *(float *)((int)epsilon + iVar5 * 4) + hinv + AMatrix[0xf];
      AcholMatrix[0xf] =
           in_stack_00000020 * *(float *)((int)epsilon + iVar5 * 4) + hinv + AcholMatrix[0xf];
      i = i + 1;
      MVar4 = JM[5];
    } while (i < (int)MVar4);
  }
  uVar8 = (int)JM[1] - 1U & 3;
  i = uVar8 + 1;
  k = i * 5;
  if (3 < (uint)i) goto LAB_0001058a;
  uVar3 = -i & 3;
  if (uVar8 + 2 < 4) {
    if (uVar3 != 0) {
      if (1 < uVar3) {
        if (2 < uVar3) {
          AMatrix[i * 5] = 1.0;
          AcholMatrix[i * 5] = 1.0;
          k = k + 5;
          i = uVar8 + 2;
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
  MVar4 = JM[5];
LAB_0001058a:
  i = 0;
  c = 0;
  if (0 < (int)MVar4) {
    do {
      if (i < (int)MVar4) {
        MVar2 = JM[7];
        uVar8 = (int)MVar4 - i & 3;
        iVar6 = i;
        if (i + 1 < (int)MVar4) {
          iVar5 = i;
          if (uVar8 != 0) {
            if (1 < uVar8) {
              if (2 < uVar8) {
                if ((*(int *)((int)MVar2 + (i * (int)MVar4 + i) * 4) != 0) && (c < i)) {
                  c = i;
                }
                iVar6 = i + 1;
              }
              if ((*(int *)((int)MVar2 + (iVar6 * (int)MVar4 + i) * 4) != 0) && (c < iVar6)) {
                c = iVar6;
              }
              iVar6 = iVar6 + 1;
            }
            goto LAB_000105cb;
          }
        }
        else {
LAB_000105cb:
          if ((*(int *)((int)MVar2 + (iVar6 * (int)MVar4 + i) * 4) != 0) && (c < iVar6)) {
            c = iVar6;
          }
          iVar5 = iVar6 + 1;
          if ((int)MVar4 <= iVar6 + 1) goto LAB_00010661;
        }
        do {
          if ((*(int *)((int)MVar2 + (iVar5 * (int)MVar4 + i) * 4) != 0) && (c < iVar5)) {
            c = iVar5;
          }
          iVar6 = iVar5 + 1;
          if ((*(int *)((int)MVar2 + (iVar6 * (int)MVar4 + i) * 4) != 0) && (c < iVar6)) {
            c = iVar6;
          }
          iVar6 = iVar5 + 2;
          if ((*(int *)((int)MVar2 + (iVar6 * (int)MVar4 + i) * 4) != 0) && (c < iVar6)) {
            c = iVar6;
          }
          iVar6 = iVar5 + 3;
          if ((*(int *)((int)MVar2 + (iVar6 * (int)MVar4 + i) * 4) != 0) && (c < iVar6)) {
            c = iVar6;
          }
          iVar5 = iVar5 + 4;
        } while (iVar5 < (int)MVar4);
      }
LAB_00010661:
      *(int *)((int)JM[0xb] + i * 4) = c;
      i = i + 1;
      MVar4 = JM[5];
    } while (i < (int)MVar4);
  }
  return;
}


/* ==== multiply ==== */

/* DWARF original prototype: void multiply(keaMatrix_pcSparse_vanilla * this, MeReal * B, MeReal *
   X) */

void keaMatrix_pcSparse_vanilla::multiply(MeReal *B,MeReal *X)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float *pfVar5;
  float *pfVar6;
  int iVar7;
  MeReal MVar8;
  int iVar9;
  MeReal MVar10;
  int in_stack_0000000c;
  int h;
  int g;
  int k;
  MeReal tmp [4];
  
                    /* Unresolved local var: MeReal * G@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int j@[DW_OP_reg3(EBX)] */
  iVar9 = 0;
  g = 0;
  MVar8 = B[5];
  if (0 < (int)MVar8) {
    do {
      h = 0;
      tmp[3] = 0.0;
      tmp[2] = 0.0;
      tmp[1] = 0.0;
      tmp[0] = 0.0;
      k = 0;
      if (0 < iVar9) {
        do {
          pfVar6 = *(float **)((int)B[7] + (iVar9 * (int)MVar8 + k) * 4);
          if (pfVar6 != (float *)0x0) {
            pfVar5 = (float *)(in_stack_0000000c + h * 4);
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
      if (iVar9 < (int)MVar8) {
        pfVar6 = (float *)(in_stack_0000000c + h * 4);
        iVar7 = iVar9;
        do {
          pfVar5 = *(float **)((int)B[7] + ((int)MVar8 * iVar7 + iVar9) * 4);
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
        } while (iVar7 < (int)MVar8);
      }
      iVar9 = iVar9 + 1;
      X[g] = tmp[0];
      X[g + 1] = tmp[1];
      X[g + 2] = tmp[2];
      X[g + 3] = tmp[3];
      g = g + 4;
      MVar8 = B[5];
    } while (iVar9 < (int)MVar8);
  }
  MVar8 = B[1];
  MVar10 = MVar8;
  while( true ) {
    if ((int)MVar8 % 0xc != 0) {
      MVar8 = (MeReal)(((int)MVar8 - (int)MVar8 % 0xc) + 0xc);
    }
    if (MVar10 == MVar8) break;
    X[(int)MVar10] = 0.0;
    MVar10 = (MeReal)((int)MVar10 + 1);
    MVar8 = B[1];
  }
  return;
}


/* ==== factorize ==== */

/* DWARF original prototype: void factorize(keaMatrix_pcSparse_vanilla * this) */

void keaMatrix_pcSparse_vanilla::factorize(void)

{
  int iVar1;
  int iVar2;
  int w;
  int d;
  int in_stack_00000004;
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
  stride = *(int *)(in_stack_00000004 + 0x14);
  z = 0;
  d = 0;
  if (0 < stride) {
    do {
      iVar2 = d + 1;
      vc_dstrip_Cholesky(ablock,*(MeReal **)(in_stack_00000004 + 0x18),z,d,
                         *(int *)(*(int *)(in_stack_00000004 + 0x30) + d * 4),
                         *(MeReal ***)(in_stack_00000004 + 0x20));
      w = z + stride;
      iVar1 = *(int *)(in_stack_00000004 + 0x2c);
      local_24 = iVar2;
      local_20 = w;
      if (iVar2 <= *(int *)(iVar1 + d * 4)) {
        do {
          if (*(int *)(*(int *)(in_stack_00000004 + 0x30) + iVar2 * 4) <= d) {
            vc_strip_Cholesky(ablock,*(MeReal **)(in_stack_00000004 + 0x18),z,w,d,
                              *(int *)(*(int *)(in_stack_00000004 + 0x30) + iVar2 * 4),
                              *(MeReal ***)(in_stack_00000004 + 0x20),
                              (MeReal **)(in_stack_00000004 + 0x28));
            iVar1 = *(int *)(in_stack_00000004 + 0x2c);
          }
          iVar2 = iVar2 + 1;
          w = w + stride;
        } while (iVar2 <= *(int *)(iVar1 + d * 4));
      }
      z = local_20;
      d = local_24;
    } while (local_24 < *(int *)(in_stack_00000004 + 0x14));
  }
  return;
}


/* ==== solve ==== */

/* DWARF original prototype: void solve(keaMatrix_pcSparse_vanilla * this, MeReal * x, MeReal * b)
    */

void keaMatrix_pcSparse_vanilla::solve(MeReal *x,MeReal *b)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float *pfVar7;
  MeReal MVar8;
  float *pfVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  MeReal *in_stack_0000000c;
  MeReal local_48;
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
  if ((b != in_stack_0000000c) && (i = 0, x[2] != 0.0)) {
    do {
      b[i] = in_stack_0000000c[i];
      i = i + 1;
    } while ((MeReal)i != x[2]);
  }
  i = 0;
  MVar8 = x[5];
  if (0 < (int)MVar8) {
    do {
      j = 0;
      iVar12 = i * (int)MVar8;
      tmp[3] = 0.0;
      tmp[2] = 0.0;
      tmp[1] = 0.0;
      tmp[0] = 0.0;
      h = 0;
      if (i < 1) {
        local_48 = x[8];
      }
      else {
        local_48 = x[8];
        do {
          pfVar7 = *(float **)((int)local_48 + iVar12 * 4);
          if (pfVar7 != (float *)0x0) {
            iVar15 = 3;
            pfVar9 = b + h;
            do {
              fVar3 = *pfVar9;
              fVar4 = pfVar9[1];
              pfVar9 = pfVar9 + 2;
              iVar15 = iVar15 + -2;
              tmp[0] = pfVar7[4] * fVar4 + *pfVar7 * fVar3 + tmp[0];
              tmp[1] = pfVar7[5] * fVar4 + pfVar7[1] * fVar3 + tmp[1];
              tmp[2] = pfVar7[6] * fVar4 + pfVar7[2] * fVar3 + tmp[2];
              tmp[3] = fVar4 * pfVar7[7] + pfVar7[3] * fVar3 + tmp[3];
              pfVar7 = pfVar7 + 8;
            } while (-1 < iVar15);
          }
          j = j + 1;
          h = h + 4;
          iVar12 = iVar12 + 1;
        } while (j < i);
      }
      iVar12 = *(int *)((int)local_48 + iVar12 * 4);
      iVar15 = 0;
      do {
        iVar13 = h + iVar15;
        fVar3 = (b[iVar13] - tmp[iVar15]) * *(float *)((int)x[6] + iVar13 * 4);
        b[iVar13] = fVar3;
        iVar13 = iVar15 + 1;
        if (iVar13 < 4) {
          pfVar7 = (float *)(iVar12 + (iVar13 + iVar15 * 4) * 4);
          uVar14 = -iVar13 & 3;
          iVar11 = iVar13;
          if (iVar15 + 2 < 4) {
            iVar10 = iVar13;
            if (uVar14 != 0) {
              if (1 < uVar14) {
                if (2 < uVar14) {
                  fVar4 = *pfVar7;
                  pfVar7 = pfVar7 + 1;
                  tmp[iVar13] = fVar4 * fVar3 + tmp[iVar13];
                  iVar11 = iVar15 + 2;
                }
                fVar4 = *pfVar7;
                pfVar7 = pfVar7 + 1;
                tmp[iVar11] = fVar4 * fVar3 + tmp[iVar11];
                iVar11 = iVar11 + 1;
              }
              goto LAB_00010d08;
            }
          }
          else {
LAB_00010d08:
            fVar4 = *pfVar7;
            pfVar7 = pfVar7 + 1;
            tmp[iVar11] = fVar4 * fVar3 + tmp[iVar11];
            iVar10 = iVar11 + 1;
            if (3 < iVar11 + 1) goto LAB_00010d6a;
          }
          do {
            tmp[iVar10] = *pfVar7 * fVar3 + tmp[iVar10];
            tmp[iVar10 + 1] = pfVar7[1] * fVar3 + tmp[iVar10 + 1];
            iVar15 = iVar10 + 4;
            tmp[iVar10 + 2] = pfVar7[2] * fVar3 + tmp[iVar10 + 2];
            pfVar9 = pfVar7 + 3;
            pfVar7 = pfVar7 + 4;
            tmp[iVar10 + 3] = *pfVar9 * fVar3 + tmp[iVar10 + 3];
            iVar10 = iVar15;
          } while (iVar15 < 4);
        }
LAB_00010d6a:
        iVar15 = iVar13;
      } while (iVar13 < 4);
      i = i + 1;
      MVar8 = x[5];
    } while (i < (int)MVar8);
  }
  i = (int)MVar8 + -1;
  iVar12 = i * (int)MVar8;
  if (i < 0) {
LAB_00010f7b:
    i = (int)x[1];
    if ((MeReal)i != x[2]) {
      do {
        b[i] = 0.0;
        i = i + 1;
      } while ((MeReal)i != x[2]);
    }
    return;
  }
  do {
    j = (int)MVar8 + -1;
    iVar15 = iVar12 + i;
    tmp[3] = 0.0;
    tmp[2] = 0.0;
    tmp[1] = 0.0;
    tmp[0] = 0.0;
    g = h;
    if (i < j) {
      local_48 = x[8];
      pfVar7 = b + h;
      do {
        pfVar9 = *(float **)((int)local_48 + iVar15 * 4);
        if (pfVar9 != (float *)0x0) {
          fVar3 = *pfVar7;
          fVar4 = pfVar7[1];
          fVar5 = pfVar7[2];
          fVar6 = pfVar7[3];
          tmp[0] = pfVar9[3] * fVar6 + pfVar9[2] * fVar5 + *pfVar9 * fVar3 + pfVar9[1] * fVar4 +
                   tmp[0];
          tmp[1] = pfVar9[7] * fVar6 + pfVar9[6] * fVar5 + pfVar9[4] * fVar3 + pfVar9[5] * fVar4 +
                   tmp[1];
          tmp[2] = pfVar9[0xb] * fVar6 + pfVar9[10] * fVar5 + pfVar9[8] * fVar3 + pfVar9[9] * fVar4
                   + tmp[2];
          tmp[3] = fVar3 * pfVar9[0xc] + fVar4 * pfVar9[0xd] + fVar5 * pfVar9[0xe] +
                   fVar6 * pfVar9[0xf] + tmp[3];
        }
        j = j + -1;
        g = g + -4;
        iVar15 = iVar15 - (int)MVar8;
        pfVar7 = pfVar7 + -4;
      } while (i < j);
    }
    else {
      local_48 = x[8];
    }
    iVar15 = *(int *)((int)local_48 + iVar15 * 4);
    iVar13 = 3;
    do {
      iVar11 = iVar13 + 1;
      if (iVar11 < 4) {
        pfVar7 = b + g + iVar11;
        pfVar9 = (float *)(iVar15 + (iVar11 + iVar13 * 4) * 4);
        uVar14 = -iVar11 & 3;
        if (iVar13 + 2 < 4) {
          if (uVar14 != 0) {
            if (1 < uVar14) {
              if (2 < uVar14) {
                fVar3 = *pfVar7;
                pfVar7 = pfVar7 + 1;
                fVar4 = *pfVar9;
                pfVar9 = pfVar9 + 1;
                tmp[iVar13] = fVar3 * fVar4 + tmp[iVar13];
                iVar11 = iVar13 + 2;
              }
              fVar3 = *pfVar7;
              iVar11 = iVar11 + 1;
              pfVar7 = pfVar7 + 1;
              fVar4 = *pfVar9;
              pfVar9 = pfVar9 + 1;
              tmp[iVar13] = fVar3 * fVar4 + tmp[iVar13];
            }
            goto LAB_00010eee;
          }
        }
        else {
LAB_00010eee:
          fVar3 = *pfVar7;
          iVar11 = iVar11 + 1;
          pfVar7 = pfVar7 + 1;
          fVar4 = *pfVar9;
          pfVar9 = pfVar9 + 1;
          tmp[iVar13] = fVar3 * fVar4 + tmp[iVar13];
          if (3 < iVar11) goto LAB_00010f44;
        }
        do {
          iVar11 = iVar11 + 4;
          fVar3 = *pfVar7 * *pfVar9 + tmp[iVar13];
          tmp[iVar13] = fVar3;
          fVar3 = pfVar7[1] * pfVar9[1] + fVar3;
          tmp[iVar13] = fVar3;
          fVar3 = pfVar7[2] * pfVar9[2] + fVar3;
          tmp[iVar13] = fVar3;
          pfVar1 = pfVar7 + 3;
          pfVar7 = pfVar7 + 4;
          pfVar2 = pfVar9 + 3;
          pfVar9 = pfVar9 + 4;
          tmp[iVar13] = *pfVar1 * *pfVar2 + fVar3;
        } while (iVar11 < 4);
      }
LAB_00010f44:
      iVar11 = g + iVar13;
      pfVar7 = tmp + iVar13;
      iVar13 = iVar13 + -1;
      b[iVar11] = (b[iVar11] - *pfVar7) * *(float *)((int)x[6] + iVar11 * 4);
    } while (-1 < iVar13);
    i = i + -1;
    if (i < 0) goto LAB_00010f7b;
    MVar8 = x[5];
  } while( true );
}


/* ==== solveUnits ==== */

/* DWARF original prototype: void solveUnits(keaMatrix_pcSparse_vanilla * this, MeReal * Ainv, int *
   cached, int * clamped, int numClamped, int AinvStride) */

void keaMatrix_pcSparse_vanilla::solveUnits
               (MeReal *Ainv,int *cached,int *clamped,int numClamped,int AinvStride)

{
  float fVar1;
  float fVar2;
  int iVar3;
  int iVar4;
  float fVar5;
  float fVar6;
  int iVar7;
  int iVar8;
  MeReal MVar9;
  MeReal MVar10;
  uint uVar11;
  float *pfVar12;
  float *pfVar13;
  int iVar14;
  int iVar15;
  float *pfVar16;
  int *piVar17;
  int iVar18;
  int iVar19;
  uint in_stack_00000018;
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
  iVar4 = -(AinvStride * 4 + 0xfU & 0xfffffff0);
  c = 0;
  if (0 < AinvStride) {
    if ((AinvStride < 2) || ((AinvStride & 1U) != 0)) {
      iVar3 = *(int *)numClamped;
      if (clamped[iVar3] == 0) {
        piVar17 = cached + in_stack_00000018 * iVar3;
        for (uVar11 = in_stack_00000018 & 0x3fffffff; uVar11 != 0; uVar11 = uVar11 - 1) {
          *piVar17 = 0;
          piVar17 = piVar17 + 1;
        }
        *(int *)((in_stack_00000018 * 4 + 4) * iVar3 + (int)cached) = 0x3f800000;
        clamped[iVar3] = 1;
        *(int *)((int)aiStack_4c + iVar4) = iVar3;
        t = 1;
      }
      c = 1;
      if (AinvStride < 2) goto LAB_000111d1;
    }
    do {
      iVar3 = *(int *)(numClamped + c * 4);
      if (clamped[iVar3] == 0) {
        piVar17 = cached + in_stack_00000018 * iVar3;
        for (uVar11 = in_stack_00000018 & 0x3fffffff; uVar11 != 0; uVar11 = uVar11 - 1) {
          *piVar17 = 0;
          piVar17 = piVar17 + 1;
        }
        *(int *)((in_stack_00000018 * 4 + 4) * iVar3 + (int)cached) = 0x3f800000;
        clamped[iVar3] = 1;
        *(int *)((int)aiStack_4c + t * 4 + iVar4) = iVar3;
        t = t + 1;
      }
      iVar3 = *(int *)(numClamped + 4 + c * 4);
      if (clamped[iVar3] == 0) {
        piVar17 = cached + in_stack_00000018 * iVar3;
        for (uVar11 = in_stack_00000018 & 0x3fffffff; uVar11 != 0; uVar11 = uVar11 - 1) {
          *piVar17 = 0;
          piVar17 = piVar17 + 1;
        }
        *(int *)((in_stack_00000018 * 4 + 4) * iVar3 + (int)cached) = 0x3f800000;
        clamped[iVar3] = 1;
        *(int *)((int)aiStack_4c + t * 4 + iVar4) = iVar3;
        t = t + 1;
      }
      c = c + 2;
    } while (c < AinvStride);
  }
LAB_000111d1:
  iVar3 = t;
  MVar9 = Ainv[5];
  i = 0;
  if (0 < (int)MVar9) {
    do {
      k = (int)MVar9 * i;
      j = 0;
      h = 0;
      if (i < 1) {
        MVar9 = Ainv[8];
      }
      else {
        MVar9 = Ainv[8];
        do {
          pfVar12 = *(float **)((int)MVar9 + k * 4);
          if ((pfVar12 != (float *)0x0) && (t = 0, 0 < iVar3)) {
            do {
              iVar7 = *(int *)((int)aiStack_4c + t * 4 + iVar4);
              if (iVar7 >> 2 <= j) {
                iVar7 = in_stack_00000018 * iVar7;
                iVar8 = iVar7 + h;
                iVar18 = 0;
                pfVar13 = (float *)(cached + iVar7 + i * 4);
                pfVar16 = pfVar12;
                do {
                  iVar7 = iVar18 + 1;
                  fVar1 = *pfVar13 - (float)cached[iVar8 + iVar18] * *pfVar16;
                  *pfVar13 = fVar1;
                  fVar2 = pfVar13[1] - (float)cached[iVar8 + iVar18] * pfVar16[1];
                  pfVar13[1] = fVar2;
                  fVar5 = pfVar13[2] - (float)cached[iVar8 + iVar18] * pfVar16[2];
                  pfVar13[2] = fVar5;
                  iVar19 = iVar8 + iVar18;
                  iVar18 = iVar18 + 2;
                  fVar6 = pfVar13[3] - (float)cached[iVar19] * pfVar16[3];
                  pfVar13[3] = fVar6;
                  *pfVar13 = fVar1 - (float)cached[iVar8 + iVar7] * pfVar16[4];
                  pfVar13[1] = fVar2 - (float)cached[iVar8 + iVar7] * pfVar16[5];
                  pfVar13[2] = fVar5 - (float)cached[iVar8 + iVar7] * pfVar16[6];
                  pfVar13[3] = fVar6 - (float)cached[iVar8 + iVar7] * pfVar16[7];
                  pfVar16 = pfVar16 + 8;
                } while (iVar18 < 4);
              }
              t = t + 1;
            } while (t < iVar3);
            MVar9 = Ainv[8];
          }
          j = j + 1;
          k = k + 1;
          h = h + 4;
        } while (j < i);
      }
      iVar7 = *(int *)((int)MVar9 + k * 4);
      t = 0;
      if (0 < iVar3) {
        do {
          iVar8 = *(int *)((int)aiStack_4c + t * 4 + iVar4);
          if (iVar8 >> 2 <= i) {
            iVar8 = in_stack_00000018 * iVar8 + i * 4;
            iVar18 = 0;
            do {
              iVar19 = iVar18 + 1;
              fVar1 = *(float *)((int)Ainv[6] + (h + iVar18) * 4) * (float)cached[iVar8 + iVar18];
              cached[iVar8 + iVar18] = (int)fVar1;
              if (iVar19 < 4) {
                pfVar12 = (float *)(iVar7 + (iVar19 + iVar18 * 4) * 4);
                uVar11 = -iVar19 & 3;
                iVar14 = iVar19;
                if (iVar18 + 2 < 4) {
                  if (uVar11 != 0) {
                    if (1 < uVar11) {
                      iVar14 = iVar19;
                      if (2 < uVar11) {
                        fVar2 = *pfVar12;
                        pfVar12 = pfVar12 + 1;
                        cached[iVar8 + iVar19] =
                             (int)((float)cached[iVar8 + iVar19] - fVar1 * fVar2);
                        iVar14 = iVar18 + 2;
                      }
                      fVar1 = *pfVar12;
                      pfVar12 = pfVar12 + 1;
                      cached[iVar8 + iVar14] =
                           (int)((float)cached[iVar8 + iVar14] -
                                (float)cached[iVar8 + iVar18] * fVar1);
                      iVar14 = iVar14 + 1;
                    }
                    goto LAB_000113a7;
                  }
                }
                else {
LAB_000113a7:
                  fVar1 = *pfVar12;
                  pfVar12 = pfVar12 + 1;
                  cached[iVar8 + iVar14] =
                       (int)((float)cached[iVar8 + iVar14] - (float)cached[iVar8 + iVar18] * fVar1);
                  iVar14 = iVar14 + 1;
                  if (3 < iVar14) goto LAB_00011403;
                }
                do {
                  cached[iVar8 + iVar14] =
                       (int)((float)cached[iVar8 + iVar14] -
                            (float)cached[iVar8 + iVar18] * *pfVar12);
                  cached[iVar8 + iVar14 + 1] =
                       (int)((float)cached[iVar8 + iVar14 + 1] -
                            (float)cached[iVar8 + iVar18] * pfVar12[1]);
                  cached[iVar8 + iVar14 + 2] =
                       (int)((float)cached[iVar8 + iVar14 + 2] -
                            (float)cached[iVar8 + iVar18] * pfVar12[2]);
                  iVar15 = iVar14 + 4;
                  pfVar13 = pfVar12 + 3;
                  pfVar12 = pfVar12 + 4;
                  cached[iVar8 + iVar14 + 3] =
                       (int)((float)cached[iVar8 + iVar14 + 3] -
                            (float)cached[iVar8 + iVar18] * *pfVar13);
                  iVar14 = iVar15;
                } while (iVar15 < 4);
              }
LAB_00011403:
              iVar18 = iVar19;
            } while (iVar19 < 4);
          }
          t = t + 1;
        } while (t < iVar3);
      }
      i = i + 1;
      MVar9 = Ainv[5];
    } while (i < (int)MVar9);
  }
  i = (int)MVar9 + -1;
  iVar7 = i * (int)MVar9;
  if (i < 0) {
    return;
  }
  do {
    j = (int)MVar9 + -1;
    k = iVar7 + i;
    g = h;
    if (i < j) {
      MVar10 = Ainv[8];
      do {
        pfVar12 = *(float **)((int)MVar10 + k * 4);
        if ((pfVar12 != (float *)0x0) && (t = 0, 0 < iVar3)) {
          do {
            iVar8 = in_stack_00000018 * *(int *)((int)aiStack_4c + t * 4 + iVar4);
            pfVar13 = (float *)(cached + iVar8 + g);
            pfVar16 = (float *)(cached + iVar8 + i * 4);
            *pfVar16 = *pfVar16 -
                       (pfVar13[3] * pfVar12[3] +
                       pfVar13[2] * pfVar12[2] + pfVar13[1] * pfVar12[1] + *pfVar13 * *pfVar12);
            pfVar16[1] = pfVar16[1] -
                         (pfVar13[3] * pfVar12[7] +
                         pfVar13[2] * pfVar12[6] + pfVar13[1] * pfVar12[5] + *pfVar13 * pfVar12[4]);
            pfVar16[2] = pfVar16[2] -
                         (pfVar13[3] * pfVar12[0xb] +
                         pfVar13[2] * pfVar12[10] + pfVar13[1] * pfVar12[9] + *pfVar13 * pfVar12[8])
            ;
            pfVar16[3] = pfVar16[3] -
                         (pfVar13[3] * pfVar12[0xf] +
                         pfVar13[2] * pfVar12[0xe] +
                         pfVar13[1] * pfVar12[0xd] + *pfVar13 * pfVar12[0xc]);
            t = t + 1;
          } while (t < iVar3);
          MVar9 = Ainv[5];
          MVar10 = Ainv[8];
        }
        j = j + -1;
        g = g + -4;
        k = k - (int)MVar9;
      } while (i < j);
    }
    else {
      MVar10 = Ainv[8];
    }
    iVar8 = *(int *)((int)MVar10 + k * 4);
    t = 0;
    if (0 < iVar3) {
      do {
        iVar19 = 3;
        iVar18 = in_stack_00000018 * *(int *)((int)aiStack_4c + t * 4 + iVar4) + i * 4;
        pfVar12 = (float *)(cached + iVar18 + 3);
        do {
          iVar14 = iVar19 + 1;
          if (iVar14 < 4) {
            pfVar13 = (float *)(iVar8 + (iVar14 + iVar19 * 4) * 4);
            uVar11 = -iVar14 & 3;
            if (iVar19 + 2 < 4) {
              if (uVar11 != 0) {
                if (1 < uVar11) {
                  iVar15 = iVar14;
                  if (2 < uVar11) {
                    fVar1 = *pfVar13;
                    pfVar13 = pfVar13 + 1;
                    *pfVar12 = *pfVar12 - (float)cached[iVar18 + iVar14] * fVar1;
                    iVar15 = iVar19 + 2;
                  }
                  iVar14 = iVar15 + 1;
                  fVar1 = *pfVar13;
                  pfVar13 = pfVar13 + 1;
                  *pfVar12 = *pfVar12 - (float)cached[iVar18 + iVar15] * fVar1;
                }
                goto LAB_00011616;
              }
            }
            else {
LAB_00011616:
              iVar15 = iVar18 + iVar14;
              iVar14 = iVar14 + 1;
              fVar1 = *pfVar13;
              pfVar13 = pfVar13 + 1;
              *pfVar12 = *pfVar12 - (float)cached[iVar15] * fVar1;
              if (3 < iVar14) goto LAB_0001165d;
            }
            do {
              fVar1 = *pfVar12 - (float)cached[iVar18 + iVar14] * *pfVar13;
              *pfVar12 = fVar1;
              fVar1 = fVar1 - (float)cached[iVar18 + iVar14 + 1] * pfVar13[1];
              *pfVar12 = fVar1;
              fVar1 = fVar1 - (float)cached[iVar18 + iVar14 + 2] * pfVar13[2];
              *pfVar12 = fVar1;
              iVar15 = iVar14 + 3;
              iVar14 = iVar14 + 4;
              pfVar16 = pfVar13 + 3;
              pfVar13 = pfVar13 + 4;
              *pfVar12 = fVar1 - (float)cached[iVar18 + iVar15] * *pfVar16;
            } while (iVar14 < 4);
          }
LAB_0001165d:
          *pfVar12 = *(float *)((int)Ainv[6] + (g + iVar19) * 4) * *pfVar12;
          pfVar12 = pfVar12 + -1;
          iVar19 = iVar19 + -1;
        } while (-1 < iVar19);
        t = t + 1;
      } while (t < iVar3);
    }
    i = i + -1;
    if (i < 0) {
      return;
    }
    MVar9 = Ainv[5];
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


