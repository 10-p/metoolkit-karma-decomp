/* ==== calculateConstraintAndResultantForces ==== */

/* DWARF original prototype: void calculateConstraintAndResultantForces(keaFunctions_Vanilla * this,
   MdtKeaBody * * blist, MdtKeaForcePair * cforcePairs, MdtKeaJBlock[2] * Jstore, int[2] * Jbody,
   MeReal * lambda, int[8] * bl2body, int[8] * bl2cbody, int * jlen, int num_rows_exc_padding, int
   num_rows_inc_padding, int num_constraints, int num_bodies) */

void __thiscall
keaFunctions_Vanilla::calculateConstraintAndResultantForces
          (keaFunctions_Vanilla *this,MdtKeaBody **blist,MdtKeaForcePair *cforcePairs,
          MdtKeaJBlock (*Jstore) [2],int (*Jbody) [2],MeReal *lambda,int (*bl2body) [8],
          int (*bl2cbody) [8],int *jlen,int num_rows_exc_padding,int num_rows_inc_padding,
          int num_constraints,int num_bodies)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  MdtKeaJBlock *jstrip;
  MdtKeaJBlock *J;
  MeReal *Lambda;
  int block;
  int strip;
  
                    /* Unresolved local var: int body@[DW_OP_reg2(EDX)]
                       Unresolved local var: MdtKeaForce * cforces@[???] */
  Lambda = lambda;
  J = *Jstore;
  strip = 0;
  while( true ) {
    iVar1 = num_rows_exc_padding;
    if (num_rows_exc_padding < 0) {
      iVar1 = num_rows_exc_padding + 3;
    }
    if (strip == iVar1 >> 2) break;
                    /* Unresolved local var: MdtKeaJBlock * jstrip@[DW_OP_reg7(EDI)]
                       Unresolved local var: int * JMb@[DW_OP_reg3(EBX)]
                       Unresolved local var: int * JMc@[DW_OP_reg6(ESI)] */
    block = 0;
    iVar1 = *jlen;
    piVar2 = *bl2body;
    piVar3 = *bl2cbody;
    jstrip = J;
    if (iVar1 != 0) {
      do {
        if (*piVar2 != -1) {
          Multiply4VectorWith46Matrix
                    (blist[*piVar2]->force,&cforcePairs->primary_body + *piVar3,Lambda,jstrip);
          iVar1 = *jlen;
        }
        piVar2 = piVar2 + 1;
        block = block + 1;
        piVar3 = piVar3 + 1;
        jstrip = jstrip + 1;
      } while (block != iVar1);
    }
    Lambda = Lambda + 4;
    J = J + iVar1;
    bl2body = bl2body + 1;
    bl2cbody = bl2cbody + 1;
    jlen = jlen + 1;
    strip = strip + 1;
  }
  return;
}


/* ==== Multiply4VectorWith46Matrix ==== */

void Multiply4VectorWith46Matrix
               (MeReal *bodyforce,MdtKeaForce *conyforce,MeReal *lambda,MdtKeaJBlock *jstrip)

{
  float fVar1;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: MeReal f@[DW_OP_reg12(ST1)] */
  fVar1 = jstrip->col[0][3] * lambda[3] +
          jstrip->col[0][2] * lambda[2] +
          jstrip->col[0][1] * lambda[1] + jstrip->col[0][0] * *lambda;
  conyforce->force[0] = conyforce->force[0] + fVar1;
  *bodyforce = fVar1 + *bodyforce;
  fVar1 = jstrip->col[1][3] * lambda[3] +
          jstrip->col[1][2] * lambda[2] +
          jstrip->col[1][0] * *lambda + jstrip->col[1][1] * lambda[1];
  conyforce->force[1] = conyforce->force[1] + fVar1;
  bodyforce[1] = fVar1 + bodyforce[1];
  fVar1 = jstrip->col[2][3] * lambda[3] +
          jstrip->col[2][2] * lambda[2] +
          jstrip->col[2][0] * *lambda + jstrip->col[2][1] * lambda[1];
  conyforce->force[2] = conyforce->force[2] + fVar1;
  bodyforce[2] = fVar1 + bodyforce[2];
  fVar1 = jstrip->col[3][3] * lambda[3] +
          jstrip->col[3][2] * lambda[2] +
          jstrip->col[3][1] * lambda[1] + jstrip->col[3][0] * *lambda;
  conyforce->torque[0] = conyforce->torque[0] + fVar1;
  bodyforce[4] = fVar1 + bodyforce[4];
  fVar1 = jstrip->col[4][3] * lambda[3] +
          jstrip->col[4][2] * lambda[2] +
          jstrip->col[4][1] * lambda[1] + jstrip->col[4][0] * *lambda;
  conyforce->torque[1] = conyforce->torque[1] + fVar1;
  bodyforce[5] = fVar1 + bodyforce[5];
  fVar1 = jstrip->col[5][3] * lambda[3] +
          jstrip->col[5][2] * lambda[2] +
          jstrip->col[5][1] * lambda[1] + jstrip->col[5][0] * *lambda;
  conyforce->torque[2] = conyforce->torque[2] + fVar1;
  bodyforce[6] = fVar1 + bodyforce[6];
  return;
}


