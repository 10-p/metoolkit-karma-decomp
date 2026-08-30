/* ==== calcJinvMandRHS ==== */

/* DWARF original prototype: void calcJinvMandRHS(keaFunctions_Vanilla * this, MeReal * rhs,
   MdtKeaJBlockPair * jmstore, MdtKeaJBlock[2] * jstore, MeReal * xgamma, MeReal * c, MeReal * xi,
   MdtKeaInverseMassMatrix * invIworld, int[8] * bl2body, int * jlen, MdtKeaVelocity * vhmf, int
   num_bodies, int num_rows_exc_padding, int num_rows_inc_padding, MeReal stepsize, MeReal gamma) */

void __thiscall
keaFunctions_Vanilla::calcJinvMandRHS
          (keaFunctions_Vanilla *this,MeReal *rhs,MdtKeaJBlockPair *jmstore,
          MdtKeaJBlock (*jstore) [2],MeReal *xgamma,MeReal *c,MeReal *xi,
          MdtKeaInverseMassMatrix *invIworld,int (*bl2body) [8],int *jlen,MdtKeaVelocity *vhmf,
          int num_bodies,int num_rows_exc_padding,int num_rows_inc_padding,MeReal stepsize,
          MeReal gamma)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  int iVar4;
  float fVar5;
  int iVar6;
  MdtKeaJBlock *jblock;
  MdtKeaJBlock *jmblock;
  MeReal *xiptr;
  MeReal *cptr;
  MeReal *xgammaptr;
  MdtKeaJBlock *JM;
  MdtKeaJBlock *J;
  MeReal *ptrhs;
  int block;
  int strip;
  
                    /* Unresolved local var: int body@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal hinv@[DW_OP_reg12(ST1)] */
  xgammaptr = xgamma;
  ptrhs = rhs;
  J = *jstore;
  JM = *jmstore;
  cptr = c;
  xiptr = xi;
  fVar5 = 1.0 / stepsize;
  strip = 0;
  while( true ) {
    iVar6 = num_rows_exc_padding;
    if (num_rows_exc_padding < 0) {
      iVar6 = num_rows_exc_padding + 3;
    }
    if (strip == iVar6 >> 2) break;
                    /* Unresolved local var: MdtKeaJBlock * jmstrip@[DW_OP_reg7(EDI)]
                       Unresolved local var: MdtKeaJBlock * jstrip@[DW_OP_reg6(ESI)] */
    ptrhs[3] = 0.0;
    ptrhs[2] = 0.0;
    ptrhs[1] = 0.0;
    *ptrhs = 0.0;
    block = 0;
    iVar6 = jlen[strip];
    jblock = J;
    jmblock = JM;
    if (iVar6 != 0) {
      do {
        iVar4 = bl2body[strip][block];
        if (iVar4 != -1) {
          Multiply46BlockByMBlock(jmblock,jblock,invIworld + iVar4);
          Multiply46BlockBy61Vector(ptrhs,jblock,vhmf + iVar4);
          iVar6 = jlen[strip];
        }
        jmblock = jmblock + 1;
        block = block + 1;
        jblock = jblock + 1;
      } while (block != iVar6);
    }
    J = J + iVar6;
    JM = JM + iVar6;
    *ptrhs = (*cptr - (gamma + *xgammaptr) * *xiptr * fVar5) * fVar5 - *ptrhs;
    ptrhs[1] = (cptr[1] - (gamma + xgammaptr[1]) * xiptr[1] * fVar5) * fVar5 - ptrhs[1];
    ptrhs[2] = (cptr[2] - (gamma + xgammaptr[2]) * xiptr[2] * fVar5) * fVar5 - ptrhs[2];
    pfVar1 = xgammaptr + 3;
    xgammaptr = xgammaptr + 4;
    pfVar2 = xiptr + 3;
    xiptr = xiptr + 4;
    pfVar3 = cptr + 3;
    cptr = cptr + 4;
    ptrhs[3] = (*pfVar3 - (gamma + *pfVar1) * *pfVar2 * fVar5) * fVar5 - ptrhs[3];
    ptrhs = ptrhs + 4;
    strip = strip + 1;
  }
  return;
}


/* ==== Multiply46BlockByMBlock ==== */

void Multiply46BlockByMBlock
               (MdtKeaJBlock *jmblock,MdtKeaJBlock *jblock,MdtKeaInverseMassMatrix *invMblock)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  int iVar13;
  MeReal tmp [4];
  
                    /* Unresolved local var: int i@[???] */
  jmblock->col[0][0] = invMblock->invmass * jblock->col[0][0];
  jmblock->col[0][1] = invMblock->invmass * jblock->col[0][1];
  jmblock->col[0][2] = invMblock->invmass * jblock->col[0][2];
  jmblock->col[0][3] = invMblock->invmass * jblock->col[0][3];
  jmblock->col[1][0] = invMblock->invmass * jblock->col[1][0];
  jmblock->col[1][1] = invMblock->invmass * jblock->col[1][1];
  jmblock->col[1][2] = invMblock->invmass * jblock->col[1][2];
  jmblock->col[1][3] = invMblock->invmass * jblock->col[1][3];
  jmblock->col[2][0] = invMblock->invmass * jblock->col[2][0];
  jmblock->col[2][1] = invMblock->invmass * jblock->col[2][1];
  jmblock->col[2][2] = invMblock->invmass * jblock->col[2][2];
                    /* Unresolved local var: int j@[DW_OP_reg1(ECX)] */
  jmblock->col[2][3] = invMblock->invmass * jblock->col[2][3];
  iVar13 = 2;
  do {
    fVar1 = invMblock->invI0[0];
    fVar2 = jblock->col[3][1];
    fVar3 = jblock->col[3][2];
    fVar4 = jblock->col[3][3];
    fVar5 = invMblock->invI1[0];
    fVar6 = jblock->col[4][1];
    fVar7 = jblock->col[4][2];
    fVar8 = jblock->col[4][3];
    fVar9 = invMblock->invI2[0];
    invMblock = (MdtKeaInverseMassMatrix *)(invMblock->invI0 + 1);
    fVar10 = jblock->col[5][1];
    fVar11 = jblock->col[5][2];
    fVar12 = jblock->col[5][3];
    (*(MeReal (*) [4])((int)jmblock + 0x30))[0] =
         jblock->col[4][0] * fVar5 + jblock->col[3][0] * fVar1 + jblock->col[5][0] * fVar9;
    (*(MeReal (*) [4])((int)jmblock + 0x30))[1] = fVar10 * fVar9 + fVar6 * fVar5 + fVar2 * fVar1;
    (*(MeReal (*) [4])((int)jmblock + 0x30))[2] = fVar3 * fVar1 + fVar7 * fVar5 + fVar11 * fVar9;
    (*(MeReal (*) [4])((int)jmblock + 0x30))[3] = fVar4 * fVar1 + fVar5 * fVar8 + fVar9 * fVar12;
    jmblock = (MdtKeaJBlock *)((int)jmblock + 0x10);
    iVar13 = iVar13 + -1;
  } while (-1 < iVar13);
  return;
}


/* ==== Multiply46BlockBy61Vector ==== */

void Multiply46BlockBy61Vector(MeReal *arhs,MdtKeaJBlock *jblock,MdtKeaVelocity *vmMif)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float fVar20;
  float fVar21;
  float fVar22;
  float fVar23;
  float fVar24;
  MeReal tmp [4];
  
                    /* Unresolved local var: int i@[???] */
  fVar1 = vmMif->velocity[0];
  fVar2 = jblock->col[0][1];
  fVar3 = jblock->col[0][2];
  fVar4 = jblock->col[0][3];
  fVar5 = vmMif->velocity[1];
  fVar6 = jblock->col[1][1];
  fVar7 = jblock->col[1][2];
  fVar8 = jblock->col[1][3];
  fVar9 = vmMif->velocity[2];
  fVar10 = jblock->col[2][1];
  fVar11 = jblock->col[2][2];
  fVar12 = jblock->col[2][3];
  fVar13 = vmMif->angVelocity[0];
  fVar14 = jblock->col[3][1];
  fVar15 = jblock->col[3][2];
  fVar16 = jblock->col[3][3];
  fVar17 = vmMif->angVelocity[1];
  fVar18 = jblock->col[4][1];
  fVar19 = jblock->col[4][2];
  fVar20 = jblock->col[4][3];
  fVar21 = vmMif->angVelocity[2];
  fVar22 = jblock->col[5][1];
  fVar23 = jblock->col[5][2];
  fVar24 = jblock->col[5][3];
  *arhs = jblock->col[5][0] * fVar21 +
          jblock->col[4][0] * fVar17 +
          jblock->col[1][0] * fVar5 + jblock->col[0][0] * fVar1 + jblock->col[2][0] * fVar9 +
          jblock->col[3][0] * fVar13 + *arhs;
  arhs[1] = fVar22 * fVar21 +
            fVar18 * fVar17 + fVar14 * fVar13 + fVar10 * fVar9 + fVar6 * fVar5 + fVar2 * fVar1 +
            arhs[1];
  arhs[2] = fVar23 * fVar21 +
            fVar19 * fVar17 + fVar15 * fVar13 + fVar3 * fVar1 + fVar7 * fVar5 + fVar11 * fVar9 +
            arhs[2];
  arhs[3] = fVar20 * fVar17 + fVar13 * fVar16 + fVar4 * fVar1 + fVar5 * fVar8 + fVar9 * fVar12 +
            fVar21 * fVar24 + arhs[3];
  return;
}


