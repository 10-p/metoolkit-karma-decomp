/* ==== singPartmakejlenandbl2body ==== */

void singPartmakejlenandbl2body
               (int *next_constraint,int *jlen,int (*bl2body) [8],int (*bl2cony) [8],
               int (*jbody) [2],int *jsize,int num_strips_inc_padding,int num_strips,
               int num_constraints)

{
  int iVar1;
  int iVar2;
  int *pbl2conybase;
  int *pbl2bodybase;
  int body1;
  int body0;
  int offs;
  int i;
  
                    /* Unresolved local var: int * pbl2body@[DW_OP_reg6(ESI)]
                       Unresolved local var: int * pbl2cony@[DW_OP_reg3(EBX)]
                       Unresolved local var: int * pjlen@[DW_OP_reg2(EDX)] */
  offs = 0;
  *jlen = 0;
  body0 = jbody[*next_constraint][0];
  body1 = jbody[*next_constraint][1];
  pbl2bodybase = *bl2body;
  i = 0;
  pbl2conybase = *bl2cony;
  if (num_strips_inc_padding != 0) {
    do {
      *jlen = *jlen + 2;
      (*bl2body)[0] = body0;
      *(int *)((int)bl2body + 4) = body1;
      bl2body = (int (*) [8])((int)bl2body + 8);
      (*bl2cony)[0] = *next_constraint * 2;
      *(int *)((int)bl2cony + 4) = *next_constraint * 2 + 1;
      bl2cony = (int (*) [8])((int)bl2cony + 8);
      iVar1 = *next_constraint;
      iVar2 = offs + jsize[iVar1];
      if (iVar2 < 5) {
        if (iVar2 == 4) {
          jlen = jlen + 1;
          offs = 0;
          *jlen = 0;
          bl2cony = (int (*) [8])(pbl2conybase + 8);
          bl2body = (int (*) [8])(pbl2bodybase + 8);
          pbl2conybase = (int *)bl2cony;
          pbl2bodybase = (int *)bl2body;
          if (*next_constraint < num_constraints + -1) {
            iVar1 = *next_constraint + 1;
            *next_constraint = iVar1;
            body0 = jbody[iVar1][0];
            body1 = jbody[iVar1][1];
          }
        }
        else {
          offs = iVar2;
          if (iVar1 < num_constraints + -1) {
            iVar1 = iVar1 + 1;
            *next_constraint = iVar1;
            body0 = jbody[iVar1][0];
            body1 = jbody[iVar1][1];
          }
        }
      }
      else {
        jlen = jlen + 1;
        *jlen = 0;
        bl2body = (int (*) [8])(pbl2bodybase + 8);
        bl2cony = (int (*) [8])(pbl2conybase + 8);
        pbl2conybase = (int *)bl2cony;
        pbl2bodybase = (int *)bl2body;
        offs = offs + -4;
      }
      i = i + 1;
    } while (i != num_strips_inc_padding);
  }
  return;
}


/* ==== makejlenandbl2body ==== */

/* DWARF original prototype: void makejlenandbl2body(keaFunctions * this, int * jlen_12padded, int *
   jlen, int[8] * bl2body_12padded, int[8] * bl2body, int[8] * bl2cbody, int[2] * Jbody, int *
   Jsize, int * num_rows_inc_padding_partition, int * num_rows_exc_padding_partition, int *
   num_constraints_partition, int num_constraints, int num_partitions) */

void __thiscall
keaFunctions::makejlenandbl2body
          (keaFunctions *this,int *jlen_12padded,int *jlen,int (*bl2body_12padded) [8],
          int (*bl2body) [8],int (*bl2cbody) [8],int (*Jbody) [2],int *Jsize,
          int *num_rows_inc_padding_partition,int *num_rows_exc_padding_partition,
          int *num_constraints_partition,int num_constraints,int num_partitions)

{
  uint uVar1;
  int num_strips_00;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int (*paiVar6) [8];
  int iVar7;
  int iVar8;
  int num_rows;
  int ceil3_num_strips;
  int num_strips_inc_padding;
  int num_strips;
  int partition;
  int constraint;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  partition = 0;
  constraint = 0;
  if (num_partitions != 0) {
    do {
      if (0 < num_constraints_partition[partition]) {
        uVar5 = num_rows_exc_padding_partition[partition];
        if ((uVar5 & 3) == 0) {
          uVar1 = uVar5;
          if ((int)uVar5 < 0) {
            uVar1 = uVar5 + 3;
          }
        }
        else {
          uVar4 = uVar5;
          if ((int)uVar5 < 0) {
            uVar4 = uVar5 + 3;
          }
          uVar1 = (uVar4 & 0xfffffffc) + 4;
          if ((int)uVar1 < 0) {
            uVar1 = (uVar4 & 0xfffffffc) + 7;
          }
        }
        num_strips_00 = (int)uVar1 >> 2;
        iVar3 = num_rows_inc_padding_partition[partition];
        if (iVar3 < 0) {
          iVar3 = iVar3 + 3;
        }
        if ((int)uVar5 % 0xc == 0) {
          if ((int)uVar5 < 0) {
            uVar5 = uVar5 + 3;
          }
        }
        else {
          iVar2 = uVar5 - (int)uVar5 % 0xc;
          uVar5 = iVar2 + 0xc;
          if ((int)uVar5 < 0) {
            uVar5 = iVar2 + 0xf;
          }
        }
        iVar2 = (int)uVar5 >> 2;
        iVar7 = 0;
        if (iVar2 != 0) {
          do {
            paiVar6 = bl2body_12padded + iVar7;
            jlen_12padded[iVar7] = 0;
            iVar7 = iVar7 + 1;
            (*paiVar6)[0] = -3;
            (*paiVar6)[1] = -3;
            (*paiVar6)[2] = -3;
            (*paiVar6)[3] = -3;
            (*paiVar6)[4] = -3;
            (*paiVar6)[5] = -3;
            (*paiVar6)[6] = -3;
            (*paiVar6)[7] = -3;
          } while (iVar7 != iVar2);
        }
        if (num_strips_00 != 0) {
          iVar7 = 0;
          do {
            jlen[iVar7] = 0;
            iVar8 = iVar7 + 1;
            bl2body[iVar7][0] = -3;
            bl2cbody[iVar7][0] = -1;
            bl2body[iVar7][1] = -3;
            bl2cbody[iVar7][1] = -1;
            bl2body[iVar7][2] = -3;
            bl2cbody[iVar7][2] = -1;
            bl2body[iVar7][3] = -3;
            bl2cbody[iVar7][3] = -1;
            bl2body[iVar7][4] = -3;
            bl2cbody[iVar7][4] = -1;
            bl2body[iVar7][5] = -3;
            bl2cbody[iVar7][5] = -1;
            bl2body[iVar7][6] = -3;
            bl2cbody[iVar7][6] = -1;
            bl2body[iVar7][7] = -3;
            bl2cbody[iVar7][7] = -1;
            iVar7 = iVar8;
          } while (iVar8 != num_strips_00);
        }
        singPartmakejlenandbl2body
                  (&constraint,jlen,bl2body,bl2cbody,Jbody,Jsize,iVar3 >> 2,num_strips_00,
                   num_constraints);
        iVar3 = 0;
        if (num_strips_00 != 0) {
          do {
            jlen_12padded[iVar3] = jlen[iVar3];
            iVar7 = iVar3 + 1;
            bl2body_12padded[iVar3][0] = bl2body[iVar3][0];
            bl2body_12padded[iVar3][1] = bl2body[iVar3][1];
            bl2body_12padded[iVar3][2] = bl2body[iVar3][2];
            bl2body_12padded[iVar3][3] = bl2body[iVar3][3];
            bl2body_12padded[iVar3][4] = bl2body[iVar3][4];
            bl2body_12padded[iVar3][5] = bl2body[iVar3][5];
            bl2body_12padded[iVar3][6] = bl2body[iVar3][6];
            bl2body_12padded[iVar3][7] = bl2body[iVar3][7];
            iVar3 = iVar7;
          } while (iVar7 != num_strips_00);
        }
        jlen_12padded = jlen_12padded + iVar2;
        bl2body_12padded = bl2body_12padded + iVar2;
        jlen = jlen + num_strips_00;
        bl2cbody = bl2cbody + num_strips_00;
        bl2body = bl2body + num_strips_00;
      }
      partition = partition + 1;
    } while (partition != num_partitions);
  }
  return;
}


