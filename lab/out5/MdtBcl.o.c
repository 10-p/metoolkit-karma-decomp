/* ==== MdtBclInitConstraintRowList ==== */

void MdtBclInitConstraintRowList(MdtKeaConstraints *clist)

{
  clist->num_rows_inc_padding = 0;
  clist->num_rows_exc_padding = 0;
  clist->num_constraints = 0;
  clist->num_partitions = 0;
  return;
}


/* ==== MdtBclEndPartition ==== */

void MdtBclEndPartition(MdtKeaConstraints *clist)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  
  uVar5 = clist->num_rows_exc_padding;
  while ((uVar5 & 3) != 0) {
    clist->xi[uVar5] = 0.0;
    clist->slipfactor[clist->num_rows_exc_padding] = 0.0;
    clist->c[clist->num_rows_exc_padding] = 0.0;
    clist->xgamma[clist->num_rows_exc_padding] = 0.0;
    uVar5 = clist->num_rows_exc_padding + 1;
    clist->num_rows_exc_padding = uVar5;
  }
  iVar3 = clist->num_partitions;
  piVar1 = clist->num_rows_inc_padding_partition;
  if ((piVar1[iVar3] != 0) && ((piVar1[iVar3] & 3U) != 0)) {
    iVar2 = clist->num_rows_inc_padding;
    uVar5 = iVar2 - 4;
    if ((uVar5 & 3) != 0) {
      uVar4 = uVar5;
      if ((int)uVar5 < 0) {
        uVar4 = iVar2 - 1;
      }
      uVar5 = iVar2 - (uVar5 - (uVar4 & 0xfffffffc));
    }
    clist->num_rows_inc_padding = uVar5;
    iVar2 = piVar1[iVar3];
    uVar5 = iVar2 - 4;
    if ((uVar5 & 3) != 0) {
      uVar4 = uVar5;
      if ((int)uVar5 < 0) {
        uVar4 = iVar2 - 1;
      }
      uVar5 = iVar2 - (uVar5 - (uVar4 & 0xfffffffc));
    }
    piVar1[iVar3] = uVar5;
    iVar3 = clist->num_partitions;
  }
  clist->num_partitions = iVar3 + 1;
  return;
}


/* ==== MdtBclStartPartition ==== */

void MdtBclStartPartition(MdtKeaConstraints *clist)

{
  clist->num_rows_exc_padding_partition[clist->num_partitions] = 0;
  clist->num_rows_inc_padding_partition[clist->num_partitions] = 0;
  clist->num_constraints_partition[clist->num_partitions] = 0;
  return;
}


/* ==== MdtBclDotJ ==== */

MeReal MdtBclDotJ(MeReal *v,MdtKeaConstraints *clist,int row,int body)

{
  MdtKeaJBlockPair *paMVar1;
  int iVar2;
  int iVar3;
  
                    /* Unresolved local var: MeReal dot@[DW_OP_reg11(ST0)] */
  iVar3 = clist->num_rows_inc_padding + row;
  iVar2 = iVar3;
  if (iVar3 < 0) {
    iVar2 = iVar3 + 3;
  }
  paMVar1 = clist->Jstore;
  iVar2 = body * 0x60 + (iVar2 >> 2) * 0xb0 + iVar3 * 4;
  return *(float *)((int)*paMVar1 + iVar2 + 0x50) * v[5] +
         *(float *)((int)*paMVar1 + iVar2 + 0x40) * v[4] +
         *(float *)((int)*paMVar1 + iVar2 + 0x30) * v[3] +
         *(float *)((int)*paMVar1 + iVar2 + 0x20) * v[2] +
         *(float *)((int)*paMVar1 + iVar2 + 0x10) * v[1] + *(float *)((int)*paMVar1 + iVar2) * *v;
}


/* ==== MdtBclEndConstraint ==== */

/* WARNING: Variable defined which should be unmapped: i */

void MdtBclEndConstraint(MdtKeaConstraints *clist,uint rows_added)

{
  uint uVar1;
  int i;
  
                    /* Unresolved local var: int rows_added_plus_padding@[DW_OP_reg1(ECX)] */
  if (rows_added != 0) {
    i = 0;
    do {
      uVar1 = clist->num_rows_inc_padding + rows_added;
      clist->Jstore[uVar1 >> 2][0].col[i][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added;
      clist->Jstore[uVar1 >> 2][1].col[i][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added;
      clist->Jstore[uVar1 >> 2][0].col[i + 1][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added;
      clist->Jstore[uVar1 >> 2][1].col[i + 1][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added;
      clist->Jstore[uVar1 >> 2][0].col[i + 2][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added;
      clist->Jstore[uVar1 >> 2][1].col[i + 2][uVar1 & 3] = 0.0;
      i = i + 3;
    } while (i != 6);
    i = 0;
    do {
      uVar1 = clist->num_rows_inc_padding + rows_added + 1;
      clist->Jstore[uVar1 >> 2][0].col[i][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 1;
      clist->Jstore[uVar1 >> 2][1].col[i][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 1;
      clist->Jstore[uVar1 >> 2][0].col[i + 1][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 1;
      clist->Jstore[uVar1 >> 2][1].col[i + 1][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 1;
      clist->Jstore[uVar1 >> 2][0].col[i + 2][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 1;
      clist->Jstore[uVar1 >> 2][1].col[i + 2][uVar1 & 3] = 0.0;
      i = i + 3;
    } while (i != 6);
    i = 0;
    do {
      uVar1 = clist->num_rows_inc_padding + rows_added + 2;
      clist->Jstore[uVar1 >> 2][0].col[i][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 2;
      clist->Jstore[uVar1 >> 2][1].col[i][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 2;
      clist->Jstore[uVar1 >> 2][0].col[i + 1][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 2;
      clist->Jstore[uVar1 >> 2][1].col[i + 1][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 2;
      clist->Jstore[uVar1 >> 2][0].col[i + 2][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 2;
      clist->Jstore[uVar1 >> 2][1].col[i + 2][uVar1 & 3] = 0.0;
      i = i + 3;
    } while (i != 6);
    i = 0;
    do {
      uVar1 = clist->num_rows_inc_padding + rows_added + 3;
      clist->Jstore[uVar1 >> 2][0].col[i][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 3;
      clist->Jstore[uVar1 >> 2][1].col[i][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 3;
      clist->Jstore[uVar1 >> 2][0].col[i + 1][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 3;
      clist->Jstore[uVar1 >> 2][1].col[i + 1][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 3;
      clist->Jstore[uVar1 >> 2][0].col[i + 2][uVar1 & 3] = 0.0;
      uVar1 = clist->num_rows_inc_padding + rows_added + 3;
      clist->Jstore[uVar1 >> 2][1].col[i + 2][uVar1 & 3] = 0.0;
      i = i + 3;
    } while (i != 6);
    clist->force[clist->num_constraints].primary_body.force[0] = 0.0;
    clist->force[clist->num_constraints].primary_body.force[1] = 0.0;
    clist->force[clist->num_constraints].primary_body.force[2] = 0.0;
    clist->force[clist->num_constraints].primary_body.torque[0] = 0.0;
    clist->force[clist->num_constraints].primary_body.torque[1] = 0.0;
    clist->force[clist->num_constraints].primary_body.torque[2] = 0.0;
    clist->force[clist->num_constraints].secondary_body.force[0] = 0.0;
    clist->force[clist->num_constraints].secondary_body.force[1] = 0.0;
    clist->force[clist->num_constraints].secondary_body.force[2] = 0.0;
    clist->force[clist->num_constraints].secondary_body.torque[0] = 0.0;
    clist->force[clist->num_constraints].secondary_body.torque[1] = 0.0;
    clist->force[clist->num_constraints].secondary_body.torque[2] = 0.0;
    uVar1 = rows_added;
    if ((rows_added + clist->num_rows_inc_padding & 3) != 0) {
      uVar1 = rows_added + 4;
    }
    clist->num_rows_inc_padding = uVar1 + clist->num_rows_inc_padding;
    clist->num_rows_inc_padding_partition[clist->num_partitions] =
         clist->num_rows_inc_padding_partition[clist->num_partitions] + uVar1;
    clist->num_rows_exc_padding = clist->num_rows_exc_padding + rows_added;
    clist->Jsize[clist->num_constraints] = rows_added;
    clist->Jofs[clist->num_constraints] =
         clist->num_rows_exc_padding_partition[clist->num_partitions];
    clist->num_rows_exc_padding_partition[clist->num_partitions] =
         clist->num_rows_exc_padding_partition[clist->num_partitions] + rows_added;
    clist->num_constraints_partition[clist->num_partitions] =
         clist->num_constraints_partition[clist->num_partitions] + 1;
    clist->num_constraints = clist->num_constraints + 1;
  }
  return;
}


/* ==== MdtBclAddSphericalRowsWorld ==== */

void MdtBclAddSphericalRowsWorld
               (MdtKeaConstraints *clist,MeReal *body1pos,MeReal *body2pos,MeReal *cpos1world,
               MeReal *cpos2world,int body2Present,MeReal *worldLinVel,MeReal *worldAngVel)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  int iVar7;
  MdtKeaJBlockPair *paMVar8;
  float *pfVar9;
  int iVar10;
  int iVar11;
  MeReal *pMVar12;
  int iVar13;
  MeReal *pMVar14;
  MeReal *pMVar15;
  MeReal *pMVar16;
  MeReal *pMVar17;
  int z_2;
  int z_1;
  int z;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal cpos2rel [3];
  MeReal cpos1rel [3];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: int i@[???] */
  iVar10 = clist->num_rows_exc_padding;
  pMVar16 = clist->c + iVar10;
  pfVar9 = clist->xi + iVar10;
  pMVar14 = clist->lo + iVar10;
  pMVar12 = clist->hi + iVar10;
  pMVar17 = clist->slipfactor + iVar10;
  pMVar15 = clist->xgamma + iVar10;
  z = 0;
  do {
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7;
    if (iVar7 < 0) {
      iVar10 = iVar7 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar10 >> 2) * 0xb0 + z * 0x10) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7;
    if (iVar7 < 0) {
      iVar10 = iVar7 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + z * 0x10 + (iVar10 >> 2) * 0xb0 + 0x60) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7;
    if (iVar7 < 0) {
      iVar10 = iVar7 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar10 >> 2) * 0xb0 + z * 0x10 + 0x10) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7;
    if (iVar7 < 0) {
      iVar10 = iVar7 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + z * 0x10 + (iVar10 >> 2) * 0xb0 + 0x70) = 0;
    z = z + 2;
  } while (z != 6);
  *pfVar9 = 0.0;
  *pMVar17 = 0.0;
  *pMVar16 = 0.0;
  *pMVar15 = 0.0;
  z_1 = 0;
  do {
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7 + 1;
    if (iVar10 < 0) {
      iVar10 = iVar7 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + 4 + (iVar10 >> 2) * 0xb0 + z_1 * 0x10) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7 + 1;
    if (iVar10 < 0) {
      iVar10 = iVar7 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + z_1 * 0x10 + (iVar10 >> 2) * 0xb0 + 100) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7 + 1;
    if (iVar10 < 0) {
      iVar10 = iVar7 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar10 >> 2) * 0xb0 + z_1 * 0x10 + 0x14) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7 + 1;
    if (iVar10 < 0) {
      iVar10 = iVar7 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + z_1 * 0x10 + (iVar10 >> 2) * 0xb0 + 0x74) = 0;
    z_1 = z_1 + 2;
  } while (z_1 != 6);
  pfVar9[1] = 0.0;
  pMVar17[1] = 0.0;
  pMVar16[1] = 0.0;
  pMVar15[1] = 0.0;
  z_2 = 0;
  do {
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7 + 2;
    if (iVar10 < 0) {
      iVar10 = iVar7 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + 8 + (iVar10 >> 2) * 0xb0 + z_2 * 0x10) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7 + 2;
    if (iVar10 < 0) {
      iVar10 = iVar7 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + z_2 * 0x10 + (iVar10 >> 2) * 0xb0 + 0x68) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7 + 2;
    if (iVar10 < 0) {
      iVar10 = iVar7 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar10 >> 2) * 0xb0 + z_2 * 0x10 + 0x18) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7 + 2;
    if (iVar10 < 0) {
      iVar10 = iVar7 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + z_2 * 0x10 + (iVar10 >> 2) * 0xb0 + 0x78) = 0;
    z_2 = z_2 + 2;
  } while (z_2 != 6);
  pfVar9[2] = 0.0;
  pMVar17[2] = 0.0;
  pMVar16[2] = 0.0;
  pMVar15[2] = 0.0;
  iVar7 = clist->num_rows_inc_padding;
  iVar10 = iVar7;
  if (iVar7 < 0) {
    iVar10 = iVar7 + 3;
  }
  *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar10 >> 2) * 0xb0) = 0x3f800000;
  iVar7 = clist->num_rows_inc_padding;
  iVar10 = iVar7 + 1;
  if (iVar10 < 0) {
    iVar10 = iVar7 + 4;
  }
  *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar10 >> 2) * 0xb0 + 0x14) = 0x3f800000;
  iVar7 = clist->num_rows_inc_padding;
  iVar10 = iVar7 + 2;
  if (iVar10 < 0) {
    iVar10 = iVar7 + 5;
  }
  *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar10 >> 2) * 0xb0 + 0x28) = 0x3f800000;
  fVar1 = *body1pos;
  fVar2 = *cpos1world;
  fVar3 = body1pos[1];
  fVar4 = cpos1world[1];
  fVar5 = body1pos[2];
  fVar6 = cpos1world[2];
  iVar7 = clist->num_rows_inc_padding;
  iVar10 = iVar7;
  if (iVar7 < 0) {
    iVar10 = iVar7 + 3;
  }
  paMVar8 = clist->Jstore;
  iVar13 = iVar7 + (iVar10 >> 2) * 0x2c;
  iVar10 = iVar7 + 1;
  if (iVar10 < 0) {
    iVar10 = iVar7 + 4;
  }
  iVar10 = iVar10 >> 2;
  iVar11 = iVar7 + 2;
  if (iVar11 < 0) {
    iVar11 = iVar7 + 5;
  }
  iVar11 = iVar11 >> 2;
  *(undefined4 *)((int)*paMVar8 + iVar13 * 4 + 0x30) = 0;
  *(float *)((int)*paMVar8 + iVar13 * 4 + 0x40) = fVar6 - fVar5;
  *(float *)((int)*paMVar8 + iVar13 * 4 + 0x50) = -(fVar4 - fVar3);
  *(float *)((int)*paMVar8 + iVar7 * 4 + iVar10 * 0xb0 + 0x34) = -(fVar6 - fVar5);
  *(undefined4 *)((int)*paMVar8 + iVar7 * 4 + iVar10 * 0xb0 + 0x44) = 0;
  *(float *)((int)*paMVar8 + iVar7 * 4 + iVar10 * 0xb0 + 0x54) = fVar2 - fVar1;
  *(float *)((int)*paMVar8 + iVar7 * 4 + iVar11 * 0xb0 + 0x38) = fVar4 - fVar3;
  *(float *)((int)*paMVar8 + iVar7 * 4 + iVar11 * 0xb0 + 0x48) = -(fVar2 - fVar1);
  *(undefined4 *)((int)*paMVar8 + iVar7 * 4 + iVar11 * 0xb0 + 0x58) = 0;
  *pfVar9 = *cpos1world - *cpos2world;
  pfVar9[1] = cpos1world[1] - cpos2world[1];
  pfVar9[2] = cpos1world[2] - cpos2world[2];
  if (body2Present != 0) {
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7;
    if (iVar7 < 0) {
      iVar10 = iVar7 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar10 >> 2) * 0xb0 + 0x60) = 0xbf800000;
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7 + 1;
    if (iVar10 < 0) {
      iVar10 = iVar7 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar10 >> 2) * 0xb0 + 0x74) = 0xbf800000;
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7 + 2;
    if (iVar10 < 0) {
      iVar10 = iVar7 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar10 >> 2) * 0xb0 + 0x88) = 0xbf800000;
    fVar1 = *body2pos;
    fVar2 = *cpos2world;
    fVar3 = body2pos[1];
    fVar4 = cpos2world[1];
    fVar5 = body2pos[2];
    fVar6 = cpos2world[2];
    iVar7 = clist->num_rows_inc_padding;
    iVar10 = iVar7;
    if (iVar7 < 0) {
      iVar10 = iVar7 + 3;
    }
    paMVar8 = clist->Jstore;
    iVar13 = iVar7 + (iVar10 >> 2) * 0x2c;
    iVar10 = iVar7 + 1;
    if (iVar10 < 0) {
      iVar10 = iVar7 + 4;
    }
    iVar10 = iVar10 >> 2;
    iVar11 = iVar7 + 2;
    if (iVar11 < 0) {
      iVar11 = iVar7 + 5;
    }
    iVar11 = iVar11 >> 2;
    *(undefined4 *)((int)*paMVar8 + iVar13 * 4 + 0x90) = 0;
    *(float *)((int)*paMVar8 + iVar13 * 4 + 0xa0) = (fVar6 - fVar5) * -1.0;
    *(float *)((int)*paMVar8 + iVar13 * 4 + 0xb0) = -(fVar4 - fVar3) * -1.0;
    *(float *)((int)*paMVar8 + iVar7 * 4 + iVar10 * 0xb0 + 0x94) = -(fVar6 - fVar5) * -1.0;
    *(undefined4 *)((int)*paMVar8 + iVar7 * 4 + iVar10 * 0xb0 + 0xa4) = 0;
    *(float *)((int)*paMVar8 + iVar7 * 4 + iVar10 * 0xb0 + 0xb4) = (fVar2 - fVar1) * -1.0;
    *(float *)((int)*paMVar8 + iVar7 * 4 + iVar11 * 0xb0 + 0x98) = (fVar4 - fVar3) * -1.0;
    *(undefined4 *)((int)*paMVar8 + iVar7 * 4 + iVar11 * 0xb0 + 0xb8) = 0;
    *(float *)((int)*paMVar8 + iVar7 * 4 + iVar11 * 0xb0 + 0xa8) = -(fVar2 - fVar1) * -1.0;
  }
  *pMVar14 = -3.4028235e+38;
  *pMVar12 = 3.4028235e+38;
  pMVar14[1] = -3.4028235e+38;
  pMVar12[1] = 3.4028235e+38;
  pMVar14[2] = -3.4028235e+38;
  pMVar12[2] = 3.4028235e+38;
  return;
}


/* ==== MdtBclAddBSJoint ==== */

/* WARNING: Unknown calling convention */

void MdtBclAddBSJoint(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                     MdtKeaBody **blist,void *params)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  MeReal (*paMVar5) [4];
  float *pfVar6;
  MdtKeaTransformation *pMVar7;
  int iVar8;
  MeReal ref2world [4] [4];
  MeReal ref1world [4] [4];
  MeReal *c;
  
                    /* Unresolved local var: MeReal * xi@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * lo@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal * hi@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal * slipfactor@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeReal * xgamma@[DW_OP_reg0(EAX)]
                       Unresolved local var: MdtBSJoint * joint@[???] */
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
                    /* Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[???] */
  paMVar5 = ref1world;
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  pMVar7 = tlist + *(int *)((int)constraint + 0xb4);
  pfVar6 = (float *)((int)constraint + 0xbc);
  iVar8 = 3;
  do {
    fVar1 = *pfVar6;
    fVar2 = pfVar6[1];
    fVar3 = pfVar6[2];
    fVar4 = pfVar6[3];
    pfVar6 = pfVar6 + 4;
    (*paMVar5)[0] =
         fVar4 * pMVar7->pos[0] +
         fVar3 * pMVar7->R2[0] + fVar1 * pMVar7->R0[0] + fVar2 * pMVar7->R1[0];
    (*paMVar5)[1] =
         fVar4 * pMVar7->pos[1] +
         fVar3 * pMVar7->R2[1] + fVar1 * pMVar7->R0[1] + fVar2 * pMVar7->R1[1];
    (*paMVar5)[2] =
         fVar4 * pMVar7->pos[2] +
         fVar3 * pMVar7->R2[2] + fVar1 * pMVar7->R0[2] + fVar2 * pMVar7->R1[2];
    (*paMVar5)[3] =
         fVar1 * pMVar7->R0[3] + fVar2 * pMVar7->R1[3] + fVar3 * pMVar7->R2[3] +
         fVar4 * pMVar7->pos[3];
    paMVar5 = paMVar5 + 1;
    iVar8 = iVar8 + -1;
  } while (-1 < iVar8);
  if (*(int *)((int)constraint + 0xb8) == -1) {
    ref2world[0][0] = *(MeReal *)((int)constraint + 0xfc);
    ref2world[0][1] = *(MeReal *)((int)constraint + 0x100);
    ref2world[0][2] = *(MeReal *)((int)constraint + 0x104);
    ref2world[0][3] = *(MeReal *)((int)constraint + 0x108);
    ref2world[1][0] = *(MeReal *)((int)constraint + 0x10c);
    ref2world[1][1] = *(MeReal *)((int)constraint + 0x110);
    ref2world[1][2] = *(MeReal *)((int)constraint + 0x114);
    ref2world[1][3] = *(MeReal *)((int)constraint + 0x118);
    ref2world[2][0] = *(MeReal *)((int)constraint + 0x11c);
    ref2world[2][1] = *(MeReal *)((int)constraint + 0x120);
    ref2world[2][2] = *(MeReal *)((int)constraint + 0x124);
    ref2world[2][3] = *(MeReal *)((int)constraint + 0x128);
    ref2world[3][0] = *(MeReal *)((int)constraint + 300);
    ref2world[3][1] = *(MeReal *)((int)constraint + 0x130);
    ref2world[3][2] = *(MeReal *)((int)constraint + 0x134);
    ref2world[3][3] = *(MeReal *)((int)constraint + 0x138);
    iVar8 = -1;
  }
  else {
    MeMatrix4MultiplyMatrix
              (ref2world,(MeReal (*) [4])((int)constraint + 0xfc),
               tlist[*(int *)((int)constraint + 0xb8)].R0);
    iVar8 = *(int *)((int)constraint + 0xb8);
  }
  MdtBclAddSphericalRowsWorld
            (clist,tlist[*(int *)((int)constraint + 0xb4)].pos,tlist[iVar8].pos,ref1world[3],
             ref2world[3],(uint)(iVar8 != -1),(MeReal *)((int)constraint + 0x13c),
             (MeReal *)((int)constraint + 0x148));
  MdtBclEndConstraint(clist,3);
  return;
}


/* ==== MdtBclAddContact ==== */

/* WARNING: Unknown calling convention */

void MdtBclAddContact(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                     MdtKeaBody **blist,void *params)

{
  float fVar1;
  float fVar2;
  float fVar3;
  uint uVar4;
  void *pvVar5;
  void *pvVar6;
  int iVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  bool bVar11;
  MeReal *pMVar12;
  MeReal MVar13;
  int iVar14;
  MeReal *pMVar15;
  MeReal *pMVar16;
  float *pfVar17;
  float *pfVar18;
  MeReal *pMVar19;
  int iVar20;
  int iVar21;
  MeReal boxScale;
  MdtBodyID b1;
  MeBool sliding_world;
  int friction_rows;
  int options;
  MdtBclContactParams *p;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal perp [3];
  MeReal par [3];
  MeReal dir2 [3];
  MeReal dir1 [3];
  MeReal rvel [3];
  MeReal vel1 [3];
  MeReal vel0 [3];
  MeReal pos1 [3];
  MeReal pos0 [3];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MdtContact * contact@[???]
                       Unresolved local var: MeReal one_over_root_2@[???]
                       Unresolved local var: MdtBodyID b0@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal rvel_normal@[DW_OP_reg14(ST3)] */
  iVar21 = clist->num_rows_exc_padding;
  pfVar17 = clist->c + iVar21;
  pMVar15 = clist->xi + iVar21;
  pMVar19 = clist->lo + iVar21;
  pMVar12 = clist->hi + iVar21;
  pfVar18 = clist->slipfactor + iVar21;
  pMVar16 = clist->xgamma + iVar21;
  uVar4 = *(uint *)((int)constraint + 0x1a0);
  if (*(int *)((int)constraint + 0x194) == 1) {
    friction_rows = 1;
  }
  else {
    friction_rows = (uint)(*(int *)((int)constraint + 0x194) == 2) * 2;
  }
  bVar11 = false;
  if ((*(int *)((int)constraint + 0xb8) == -1) && ((uVar4 & 0x100) != 0)) {
    bVar11 = true;
  }
  pvVar5 = *(void **)((int)constraint + 0x54);
  pvVar6 = *(void **)((int)constraint + 0x58);
  boxScale = 1.0;
  MdtBodyGetCenterOfMassPosition(pvVar5,pos0);
  pos0[0] = *(float *)((int)constraint + 0x160) - pos0[0];
  pos0[1] = *(float *)((int)constraint + 0x164) - pos0[1];
  pos0[2] = *(float *)((int)constraint + 0x168) - pos0[2];
  MdtBodyGetVelocityAtPoint(pvVar5,(void *)((int)constraint + 0x160),vel0);
  if (pvVar6 == (void *)0x0) {
    if ((uVar4 & 0x100) == 0) {
      vel1[2] = 0.0;
      vel1[1] = 0.0;
      vel1[0] = 0.0;
    }
    else {
      vel1[0] = *(MeReal *)((int)constraint + 0x188);
      vel1[1] = *(MeReal *)((int)constraint + 0x18c);
      vel1[2] = *(MeReal *)((int)constraint + 400);
    }
  }
  else {
    MdtBodyGetCenterOfMassPosition(pvVar6,pos1);
    pos1[0] = *(float *)((int)constraint + 0x160) - pos1[0];
    pos1[1] = *(float *)((int)constraint + 0x164) - pos1[1];
    pos1[2] = *(float *)((int)constraint + 0x168) - pos1[2];
    MdtBodyGetVelocityAtPoint(pvVar6,(void *)((int)constraint + 0x160),vel1);
  }
  fVar8 = vel0[0] - vel1[0];
  fVar10 = vel0[1] - vel1[1];
  fVar9 = vel0[2] - vel1[2];
  fVar1 = *(float *)((int)constraint + 0x16c);
  fVar2 = *(float *)((int)constraint + 0x170);
  fVar3 = *(float *)((int)constraint + 0x174);
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  fVar1 = fVar9 * fVar3 + fVar8 * fVar1 + fVar10 * fVar2;
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar21 = 0;
  do {
    iVar7 = clist->num_rows_inc_padding;
    iVar14 = iVar7;
    if (iVar7 < 0) {
      iVar14 = iVar7 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar14 >> 2) * 0xb0 + iVar21 * 0x10) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar14 = iVar7;
    if (iVar7 < 0) {
      iVar14 = iVar7 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + iVar21 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x60) =
         0;
    iVar7 = clist->num_rows_inc_padding;
    iVar14 = iVar7;
    if (iVar7 < 0) {
      iVar14 = iVar7 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar14 >> 2) * 0xb0 + iVar21 * 0x10 + 0x10) =
         0;
    iVar7 = clist->num_rows_inc_padding;
    iVar14 = iVar7;
    if (iVar7 < 0) {
      iVar14 = iVar7 + 3;
    }
    iVar20 = iVar21 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + iVar21 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x70) =
         0;
    iVar21 = iVar20;
  } while (iVar20 != 6);
  *pMVar15 = 0.0;
  *pfVar18 = 0.0;
  *pfVar17 = 0.0;
  *pMVar16 = 0.0;
  MdtContactWriteRow(clist,0,(MeReal *)((int)constraint + 0x16c),pos0,pos1,
                     (uint)(pvVar6 != (void *)0x0));
  *pMVar15 = -*(MeReal *)((int)constraint + 0x178);
  if ((uVar4 & 8) == 0) {
    MVar13 = 0.0;
  }
  else {
    MVar13 = -*(MeReal *)((int)constraint + 0x1b0);
  }
  *pMVar19 = MVar13;
  *pMVar12 = 3.4028235e+38;
  if (((uVar4 & 4) != 0) &&
     (fVar2 = *(float *)((int)constraint + 0x1ac), *pfVar18 = fVar2, fVar2 < 0.0)) {
    *pfVar18 = 0.0;
  }
  if (((uVar4 & 2) != 0) && (fVar1 < -*(float *)((int)constraint + 0x1a8))) {
    *pfVar17 = -*(float *)((int)constraint + 0x1a4) * fVar1;
  }
  if ((uVar4 & 0x100) != 0) {
    *pfVar17 = *(float *)((int)constraint + 0x174) * *(float *)((int)constraint + 400) +
               *(float *)((int)constraint + 0x170) * *(float *)((int)constraint + 0x18c) +
               *(float *)((int)constraint + 0x16c) * *(float *)((int)constraint + 0x188) + *pfVar17;
  }
  options._0_1_ = (byte)uVar4;
  if (friction_rows != 0) {
                    /* Unresolved local var: MeReal mag@[DW_OP_reg11(ST0)] */
    if ((uVar4 & 1) == 0) {
      dir1[0] = fVar8 - fVar1 * *(float *)((int)constraint + 0x16c);
      dir1[1] = fVar10 - fVar1 * *(float *)((int)constraint + 0x170);
      dir1[2] = fVar9 - fVar1 * *(float *)((int)constraint + 0x174);
    }
    else {
      fVar1 = *(float *)((int)constraint + 0x184) * *(float *)((int)constraint + 0x174) +
              *(float *)((int)constraint + 0x17c) * *(float *)((int)constraint + 0x16c) +
              *(float *)((int)constraint + 0x180) * *(float *)((int)constraint + 0x170);
      dir1[0] = *(float *)((int)constraint + 0x17c) - *(float *)((int)constraint + 0x16c) * fVar1;
      dir1[1] = *(float *)((int)constraint + 0x180) - *(float *)((int)constraint + 0x170) * fVar1;
      dir1[2] = *(float *)((int)constraint + 0x184) - *(float *)((int)constraint + 0x174) * fVar1;
    }
    MVar13 = MeVector3Normalize(dir1);
    if (friction_rows == 1) {
      if (MVar13 < 0.001) {
        friction_rows = 0;
      }
    }
    else if (0.001 <= MVar13) {
      dir2[0] = dir1[1] * *(float *)((int)constraint + 0x174) -
                dir1[2] * *(float *)((int)constraint + 0x170);
      dir2[1] = dir1[2] * *(float *)((int)constraint + 0x16c) -
                *(float *)((int)constraint + 0x174) * dir1[0];
      dir2[2] = dir1[0] * *(float *)((int)constraint + 0x170) -
                *(float *)((int)constraint + 0x16c) * dir1[1];
      if ((((byte)options ^ 1) & 1) != 0) {
        fVar2 = dir1[0] * 0.70710677;
        fVar1 = dir1[1] * 0.70710677;
        dir1[0] = fVar2 + dir2[0] * 0.70710677;
        fVar3 = dir1[2] * 0.70710677;
        dir1[1] = fVar1 + dir2[1] * 0.70710677;
        dir2[0] = fVar2 - dir2[0] * 0.70710677;
        dir2[1] = fVar1 - dir2[1] * 0.70710677;
        dir1[2] = fVar3 + dir2[2] * 0.70710677;
        dir2[2] = fVar3 - dir2[2] * 0.70710677;
        boxScale = 0.70710677;
      }
    }
    else {
      fVar1 = *(float *)((int)constraint + 0x16c);
      fVar2 = *(float *)((int)constraint + 0x170);
      if (ABS(fVar1) <= ABS(fVar2)) {
                    /* Unresolved local var: MeReal scale@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[???] */
        fVar1 = *(float *)((int)constraint + 0x174);
        fVar3 = 1.0 / SQRT(fVar1 * fVar1 + fVar2 * fVar2);
        dir1[0] = 0.0;
        dir1[2] = fVar2 * fVar3;
        dir1[1] = -fVar1 * fVar3;
        dir2[0] = fVar2 * dir1[2] - fVar1 * dir1[1];
        fVar1 = *(MeReal *)((int)constraint + 0x16c);
        dir2[2] = fVar1 * dir1[1];
        dir2[1] = -fVar1 * dir1[2];
      }
      else {
                    /* Unresolved local var: MeReal scale@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[???] */
        fVar3 = *(float *)((int)constraint + 0x174);
        fVar8 = 1.0 / SQRT(fVar3 * fVar3 + fVar1 * fVar1);
        dir1[1] = 0.0;
        dir1[0] = fVar3 * fVar8;
        dir1[2] = -fVar1 * fVar8;
        dir2[0] = fVar2 * dir1[2];
        dir2[1] = fVar3 * dir1[0] - fVar1 * dir1[2];
        dir2[2] = -fVar2 * dir1[0];
      }
    }
    if (friction_rows != 0) {
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
      iVar21 = 0;
      do {
        iVar7 = clist->num_rows_inc_padding;
        iVar14 = iVar7 + 1;
        if (iVar14 < 0) {
          iVar14 = iVar7 + 4;
        }
        *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + 4 + (iVar14 >> 2) * 0xb0 + iVar21 * 0x10)
             = 0;
        iVar7 = clist->num_rows_inc_padding;
        iVar14 = iVar7 + 1;
        if (iVar14 < 0) {
          iVar14 = iVar7 + 4;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar7 * 4 + iVar21 * 0x10 + (iVar14 >> 2) * 0xb0 + 100) = 0;
        iVar7 = clist->num_rows_inc_padding;
        iVar14 = iVar7 + 1;
        if (iVar14 < 0) {
          iVar14 = iVar7 + 4;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar7 * 4 + (iVar14 >> 2) * 0xb0 + iVar21 * 0x10 + 0x14) = 0;
        iVar7 = clist->num_rows_inc_padding;
        iVar14 = iVar7 + 1;
        if (iVar14 < 0) {
          iVar14 = iVar7 + 4;
        }
        iVar20 = iVar21 + 2;
        *(undefined4 *)
         ((int)*clist->Jstore + iVar7 * 4 + iVar21 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x74) = 0;
        iVar21 = iVar20;
      } while (iVar20 != 6);
      pMVar15[1] = 0.0;
      pfVar18[1] = 0.0;
      pfVar17[1] = 0.0;
      pMVar16[1] = 0.0;
      MdtContactWriteRow(clist,1,dir1,pos0,pos1,(uint)(pvVar6 != (void *)0x0));
      if (((uVar4 & 0x10) != 0) &&
         (fVar1 = *(float *)((int)constraint + 0x1bc), pfVar18[1] = fVar1, fVar1 < 0.0)) {
        pfVar18[1] = 0.0;
      }
      if ((uVar4 & 0x40) != 0) {
        pfVar17[1] = *(float *)((int)constraint + 0x1cc);
      }
      if (bVar11) {
        fVar1 = *(float *)((int)constraint + 400) * dir1[2] +
                *(float *)((int)constraint + 0x18c) * dir1[1] +
                *(float *)((int)constraint + 0x188) * dir1[0] + pfVar17[1];
        pfVar17[1] = fVar1;
        *(float *)((int)constraint + 0x1cc) = fVar1;
      }
      if (*(uint *)((int)constraint + 0x198) < 2) {
        fVar1 = boxScale * *(float *)((int)constraint + 0x1b4);
      }
      else {
        fVar1 = 3.4028235e+38;
      }
      pMVar12[1] = fVar1;
      pMVar19[1] = -fVar1;
    }
  }
  if (friction_rows == 2) {
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
    iVar21 = 0;
    do {
      iVar7 = clist->num_rows_inc_padding;
      iVar14 = iVar7 + 2;
      if (iVar14 < 0) {
        iVar14 = iVar7 + 5;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + 8 + (iVar14 >> 2) * 0xb0 + iVar21 * 0x10) =
           0;
      iVar7 = clist->num_rows_inc_padding;
      iVar14 = iVar7 + 2;
      if (iVar14 < 0) {
        iVar14 = iVar7 + 5;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + iVar21 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x68)
           = 0;
      iVar7 = clist->num_rows_inc_padding;
      iVar14 = iVar7 + 2;
      if (iVar14 < 0) {
        iVar14 = iVar7 + 5;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar14 >> 2) * 0xb0 + iVar21 * 0x10 + 0x18)
           = 0;
      iVar7 = clist->num_rows_inc_padding;
      iVar14 = iVar7 + 2;
      if (iVar14 < 0) {
        iVar14 = iVar7 + 5;
      }
      iVar20 = iVar21 + 2;
      *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + iVar21 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x78)
           = 0;
      iVar21 = iVar20;
    } while (iVar20 != 6);
    pMVar15[2] = 0.0;
    pfVar18[2] = 0.0;
    pfVar17[2] = 0.0;
    pMVar16[2] = 0.0;
    MdtContactWriteRow(clist,2,dir2,pos0,pos1,(uint)(pvVar6 != (void *)0x0));
    if (((uVar4 & 0x20) != 0) &&
       (fVar1 = *(float *)((int)constraint + 0x1c8), pfVar18[2] = fVar1, fVar1 < 0.0)) {
      pfVar18[2] = 0.0;
    }
    if ((char)(byte)options < '\0') {
      pfVar17[2] = *(float *)((int)constraint + 0x1d0);
    }
    if (bVar11) {
      fVar1 = *(float *)((int)constraint + 400) * dir2[2] +
              *(float *)((int)constraint + 0x18c) * dir2[1] +
              *(float *)((int)constraint + 0x188) * dir2[0] + pfVar17[2];
      pfVar17[2] = fVar1;
      *(float *)((int)constraint + 0x1d0) = fVar1;
    }
    if (*(uint *)((int)constraint + 0x19c) < 2) {
      fVar1 = boxScale * *(float *)((int)constraint + 0x1c0);
    }
    else {
      fVar1 = 3.4028235e+38;
    }
    pMVar12[2] = fVar1;
    pMVar19[2] = -fVar1;
  }
  MdtBclEndConstraint(clist,friction_rows + 1);
  return;
}


/* ==== MdtBclAddContactGroup ==== */

void MdtBclAddContactGroup
               (MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
               MdtKeaBody **blist,void *params)

{
  void *constraint_00;
  undefined4 uVar1;
  float fVar2;
  MeReal averageForce;
  
                    /* Unresolved local var: MdtContactID.conflict c@[DW_OP_reg3(EBX)]
                       Unresolved local var: MdtContactGroupID g@[???] */
  if (*(int *)((int)constraint + 0x160) == 0) {
    fVar2 = 0.0;
  }
  else {
    fVar2 = *(float *)((int)constraint + 0x170) / (float)*(int *)((int)constraint + 0x160);
  }
  for (constraint_00 = *(void **)((int)constraint + 0x164); constraint_00 != (void *)0x0;
      constraint_00 = *(void **)((int)constraint_00 + 0x1dc)) {
    uVar1 = *(undefined4 *)((int)constraint + 4);
    *(undefined4 *)((int)constraint_00 + 0xb4) = *(undefined4 *)((int)constraint + 0xb4);
    *(undefined4 *)((int)constraint_00 + 0x54) = *(undefined4 *)((int)constraint + 0x54);
    *(undefined4 *)((int)constraint_00 + 4) = uVar1;
    uVar1 = *(undefined4 *)((int)constraint + 0x58);
    *(undefined4 *)((int)constraint_00 + 0xb8) = *(undefined4 *)((int)constraint + 0xb8);
    *(undefined4 *)((int)constraint_00 + 0x58) = uVar1;
    if (*(int *)((int)constraint_00 + 0x198) == 1) {
      *(float *)((int)constraint_00 + 0x1b4) = fVar2 * *(float *)((int)constraint_00 + 0x1b8);
    }
    if (*(int *)((int)constraint_00 + 0x19c) == 1) {
      *(float *)((int)constraint_00 + 0x1c0) = fVar2 * *(float *)((int)constraint_00 + 0x1c4);
    }
    MdtBclAddContact(clist,constraint_00,tlist,blist,params);
  }
  return;
}


/* ==== MdtBclAddHinge ==== */

void MdtBclAddHinge(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                   MdtKeaBody **blist,void *params)

{
  MeReal *pMVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  int iVar6;
  MdtKeaBody *pMVar7;
  MdtKeaBody *pMVar8;
  int iVar9;
  bool bVar10;
  bool bVar11;
  MeReal (*paMVar12) [4];
  float *pfVar13;
  MeReal *pMVar14;
  MdtKeaTransformation *pMVar15;
  MeReal *pMVar16;
  int iVar17;
  MeReal *pMVar18;
  uint uVar19;
  longdouble lVar20;
  MdtBclLimit *local_1a4;
  uint i;
  int z_5;
  int z_4;
  int z_3;
  int z_2;
  int z_1;
  int z;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal LinearAxis [3];
  MeReal JwQ3 [3];
  MeReal JwQ2 [3];
  MeReal qmul [4] [4];
  MeReal qrel [4];
  MeReal qbcw [4];
  MeReal qacw [4];
  MeReal ref2world [4] [4];
  MeReal ref1world [4] [4];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MdtHinge * joint@[???]
                       Unresolved local var: uint j@[???]
                       Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar17 = clist->num_rows_exc_padding;
  factors.c = clist->c + iVar17;
  pMVar14 = clist->xi + iVar17;
  pMVar18 = clist->lo + iVar17;
  pMVar16 = clist->hi + iVar17;
  factors.slipfactor = clist->slipfactor + iVar17;
  factors.xgamma = clist->xgamma + iVar17;
  z = 0;
  do {
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6;
    if (iVar6 < 0) {
      iVar17 = iVar6 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + (iVar17 >> 2) * 0xb0 + z * 0x10) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6;
    if (iVar6 < 0) {
      iVar17 = iVar6 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z * 0x10 + (iVar17 >> 2) * 0xb0 + 0x60) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6;
    if (iVar6 < 0) {
      iVar17 = iVar6 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + (iVar17 >> 2) * 0xb0 + z * 0x10 + 0x10) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6;
    if (iVar6 < 0) {
      iVar17 = iVar6 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z * 0x10 + (iVar17 >> 2) * 0xb0 + 0x70) = 0;
    z = z + 2;
  } while (z != 6);
  *pMVar14 = 0.0;
  *factors.slipfactor = 0.0;
  *factors.c = 0.0;
  *factors.xgamma = 0.0;
  z_1 = 0;
  do {
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 1;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + 4 + (iVar17 >> 2) * 0xb0 + z_1 * 0x10) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 1;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z_1 * 0x10 + (iVar17 >> 2) * 0xb0 + 100) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 1;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + (iVar17 >> 2) * 0xb0 + z_1 * 0x10 + 0x14) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 1;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z_1 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x74) = 0;
    z_1 = z_1 + 2;
  } while (z_1 != 6);
  pMVar14[1] = 0.0;
  factors.slipfactor[1] = 0.0;
  factors.c[1] = 0.0;
  factors.xgamma[1] = 0.0;
  z_2 = 0;
  do {
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 2;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + 8 + (iVar17 >> 2) * 0xb0 + z_2 * 0x10) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 2;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z_2 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x68) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 2;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + (iVar17 >> 2) * 0xb0 + z_2 * 0x10 + 0x18) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 2;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z_2 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x78) = 0;
    z_2 = z_2 + 2;
  } while (z_2 != 6);
  pMVar14[2] = 0.0;
  factors.slipfactor[2] = 0.0;
  factors.c[2] = 0.0;
  factors.xgamma[2] = 0.0;
  z_3 = 0;
  do {
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 3;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + 0xc + (iVar17 >> 2) * 0xb0 + z_3 * 0x10) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 3;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z_3 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x6c) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 3;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + (iVar17 >> 2) * 0xb0 + z_3 * 0x10 + 0x1c) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 3;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z_3 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x7c) = 0;
    z_3 = z_3 + 2;
  } while (z_3 != 6);
  pMVar14[3] = 0.0;
  factors.slipfactor[3] = 0.0;
  factors.c[3] = 0.0;
  factors.xgamma[3] = 0.0;
  z_4 = 0;
  do {
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 4;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + 0x10 + (iVar17 >> 2) * 0xb0 + z_4 * 0x10) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 4;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z_4 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x70) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 4;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + (iVar17 >> 2) * 0xb0 + z_4 * 0x10 + 0x20) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 4;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z_4 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x80) = 0;
    z_4 = z_4 + 2;
  } while (z_4 != 6);
  pMVar14[4] = 0.0;
  factors.slipfactor[4] = 0.0;
  factors.c[4] = 0.0;
  factors.xgamma[4] = 0.0;
  z_5 = 0;
  do {
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + 0x14 + (iVar17 >> 2) * 0xb0 + z_5 * 0x10) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z_5 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x74) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + (iVar17 >> 2) * 0xb0 + z_5 * 0x10 + 0x24) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + z_5 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x84) = 0;
    z_5 = z_5 + 2;
  } while (z_5 != 6);
  pMVar14[5] = 0.0;
  factors.slipfactor[5] = 0.0;
  factors.c[5] = 0.0;
  factors.xgamma[5] = 0.0;
  pMVar15 = tlist + *(int *)((int)constraint + 0xb4);
  pfVar13 = (float *)((int)constraint + 0xbc);
  iVar17 = 3;
  paMVar12 = ref1world;
  factors.xi = pMVar14;
  factors.lo = pMVar18;
  factors.hi = pMVar16;
  do {
    fVar2 = *pfVar13;
    fVar3 = pfVar13[1];
    fVar4 = pfVar13[2];
    fVar5 = pfVar13[3];
    pfVar13 = pfVar13 + 4;
    (*paMVar12)[0] =
         fVar5 * pMVar15->pos[0] +
         fVar4 * pMVar15->R2[0] + fVar2 * pMVar15->R0[0] + fVar3 * pMVar15->R1[0];
    (*paMVar12)[1] =
         fVar5 * pMVar15->pos[1] +
         fVar4 * pMVar15->R2[1] + fVar2 * pMVar15->R0[1] + fVar3 * pMVar15->R1[1];
    (*paMVar12)[2] =
         fVar5 * pMVar15->pos[2] +
         fVar4 * pMVar15->R2[2] + fVar2 * pMVar15->R0[2] + fVar3 * pMVar15->R1[2];
    (*paMVar12)[3] =
         fVar2 * pMVar15->R0[3] + fVar3 * pMVar15->R1[3] + fVar4 * pMVar15->R2[3] +
         fVar5 * pMVar15->pos[3];
    paMVar12 = paMVar12 + 1;
    iVar17 = iVar17 + -1;
  } while (-1 < iVar17);
  if (*(int *)((int)constraint + 0xb8) == -1) {
    ref2world[0][0] = *(MeReal *)((int)constraint + 0xfc);
    ref2world[0][1] = *(MeReal *)((int)constraint + 0x100);
    ref2world[0][2] = *(MeReal *)((int)constraint + 0x104);
    ref2world[0][3] = *(MeReal *)((int)constraint + 0x108);
    ref2world[1][0] = *(MeReal *)((int)constraint + 0x10c);
    ref2world[1][1] = *(MeReal *)((int)constraint + 0x110);
    ref2world[1][2] = *(MeReal *)((int)constraint + 0x114);
    ref2world[1][3] = *(MeReal *)((int)constraint + 0x118);
    ref2world[2][0] = *(MeReal *)((int)constraint + 0x11c);
    ref2world[2][1] = *(MeReal *)((int)constraint + 0x120);
    ref2world[2][2] = *(MeReal *)((int)constraint + 0x124);
    ref2world[2][3] = *(MeReal *)((int)constraint + 0x128);
    ref2world[3][0] = *(MeReal *)((int)constraint + 300);
    ref2world[3][1] = *(MeReal *)((int)constraint + 0x130);
    ref2world[3][2] = *(MeReal *)((int)constraint + 0x134);
    ref2world[3][3] = *(MeReal *)((int)constraint + 0x138);
  }
  else {
    MeMatrix4MultiplyMatrix
              (ref2world,(MeReal (*) [4])((int)constraint + 0xfc),
               tlist[*(int *)((int)constraint + 0xb8)].R0);
  }
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  if (*(int *)((int)constraint + 0x164) == 0) {
    *(undefined4 *)((int)constraint + 0x170) = 0;
  }
  else {
    if (*(int *)((int)constraint + 0xb8) == -1) {
      pMVar7 = blist[*(int *)((int)constraint + 0xb4)];
      qacw[0] = pMVar7->velrot[0];
      qacw[1] = pMVar7->velrot[1];
      qacw[2] = pMVar7->velrot[2];
    }
    else {
      pMVar7 = blist[*(int *)((int)constraint + 0xb4)];
      pMVar8 = blist[*(int *)((int)constraint + 0xb8)];
      qacw[0] = pMVar7->velrot[0] - pMVar8->velrot[0];
      qacw[1] = pMVar7->velrot[1] - pMVar8->velrot[1];
      qacw[2] = pMVar7->velrot[2] - pMVar8->velrot[2];
    }
                    /* Unresolved local var: longdouble __value@[DW_OP_reg13(ST2)] */
    lVar20 = (longdouble)
             fpatan((longdouble)ref2world[1][2] * (longdouble)ref1world[2][2] +
                    (longdouble)ref2world[1][0] * (longdouble)ref1world[2][0] +
                    (longdouble)ref2world[1][1] * (longdouble)ref1world[2][1],
                    (longdouble)ref2world[1][0] * (longdouble)ref1world[1][0] +
                    (longdouble)ref2world[1][1] * (longdouble)ref1world[1][1] +
                    (longdouble)ref2world[1][2] * (longdouble)ref1world[1][2]);
    *(float *)((int)constraint + 0x174) = -(float)lVar20;
    *(float *)((int)constraint + 0x188) =
         ref1world[0][2] * qacw[2] + ref1world[0][1] * qacw[1] + ref1world[0][0] * qacw[0];
    CalculateLimitPositionAndOvershoot((MdtBclLimit *)((int)constraint + 0x160),*(MeReal *)params,1)
    ;
  }
  local_1a4 = (MdtBclLimit *)((int)constraint + 0x160);
  MdtBclAddSphericalRowsWorld
            (clist,tlist[*(int *)((int)constraint + 0xb4)].pos,
             tlist[*(int *)((int)constraint + 0xb8)].pos,ref1world[3],ref2world[3],
             (uint)(*(int *)((int)constraint + 0xb8) != -1),(MeReal *)((int)constraint + 0x13c),
             (MeReal *)((int)constraint + 0x148));
  MeQuaternionFromTM(qacw,ref1world);
  MeQuaternionFromTM(qbcw,ref2world);
  i = 0;
  do {
    fVar2 = qacw[i];
    qmul[i][0] = fVar2 * qbcw[0];
    qmul[i][1] = fVar2 * qbcw[1];
    qmul[i][3] = fVar2 * qbcw[3];
    qmul[i][2] = fVar2 * qbcw[2];
    iVar17 = i + 1;
    fVar2 = qacw[iVar17];
    qmul[iVar17][0] = qbcw[0] * fVar2;
    qmul[iVar17][1] = qbcw[1] * fVar2;
    qmul[iVar17][2] = qbcw[2] * fVar2;
    qmul[iVar17][3] = qbcw[3] * fVar2;
    i = i + 2;
  } while (i < 4);
  i = 0;
  JwQ2[0] = (((qmul[2][1] - qmul[3][0]) - qmul[0][3]) + qmul[1][2]) * 0.5;
  JwQ3[0] = (qmul[3][1] + qmul[2][0] + qmul[1][3] + qmul[0][2]) * 0.5;
  JwQ2[1] = (((qmul[2][2] - qmul[3][3]) + qmul[0][0]) - qmul[1][1]) * 0.5;
  JwQ3[2] = (((qmul[3][3] - qmul[2][2]) - qmul[1][1]) + qmul[0][0]) * 0.5;
  JwQ3[1] = (((qmul[3][2] + qmul[2][3]) - qmul[1][0]) - qmul[0][1]) * 0.5;
  JwQ2[2] = (qmul[1][0] + qmul[3][2] + qmul[2][3] + qmul[0][1]) * 0.5;
  do {
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 3;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + 0xc + (iVar17 >> 2) * 0xb0 + i * 0x10) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 3;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 6;
    }
    *(MeReal *)((int)*clist->Jstore + iVar6 * 4 + (iVar17 >> 2) * 0xb0 + i * 0x10 + 0x3c) = JwQ2[i];
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 4;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + 0x10 + (iVar17 >> 2) * 0xb0 + i * 0x10) = 0;
    iVar6 = clist->num_rows_inc_padding;
    iVar17 = iVar6 + 4;
    if (iVar17 < 0) {
      iVar17 = iVar6 + 7;
    }
    iVar9 = i * 0x10;
    pMVar1 = JwQ3 + i;
    i = i + 1;
    *(MeReal *)((int)*clist->Jstore + iVar6 * 4 + (iVar17 >> 2) * 0xb0 + iVar9 + 0x40) = *pMVar1;
  } while (i < 3);
  if (*(int *)((int)constraint + 0xb8) != -1) {
    i = 0;
    do {
      iVar6 = clist->num_rows_inc_padding;
      iVar17 = iVar6 + 3;
      if (iVar17 < 0) {
        iVar17 = iVar6 + 6;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + i * 0x10 + (iVar17 >> 2) * 0xb0 + 0x6c) = 0;
      iVar6 = clist->num_rows_inc_padding;
      iVar17 = iVar6 + 3;
      if (iVar17 < 0) {
        iVar17 = iVar6 + 6;
      }
      *(float *)((int)*clist->Jstore + iVar6 * 4 + i * 0x10 + (iVar17 >> 2) * 0xb0 + 0x9c) =
           -JwQ2[i];
      iVar6 = clist->num_rows_inc_padding;
      iVar17 = iVar6 + 4;
      if (iVar17 < 0) {
        iVar17 = iVar6 + 7;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar6 * 4 + i * 0x10 + (iVar17 >> 2) * 0xb0 + 0x70) = 0;
      iVar6 = clist->num_rows_inc_padding;
      iVar17 = iVar6 + 4;
      if (iVar17 < 0) {
        iVar17 = iVar6 + 7;
      }
      uVar19 = i + 1;
      *(float *)((int)*clist->Jstore + iVar6 * 4 + i * 0x10 + (iVar17 >> 2) * 0xb0 + 0xa0) =
           -JwQ3[i];
      i = uVar19;
    } while (uVar19 < 3);
  }
  pMVar14[3] = -((-qmul[2][0] - qmul[3][1]) + qmul[0][2] + qmul[1][3]);
  pMVar14[4] = -(((qmul[2][1] - qmul[3][0]) - qmul[1][2]) + qmul[0][3]);
  pMVar18[3] = -3.4028235e+38;
  pMVar16[3] = 3.4028235e+38;
  pMVar18[4] = -3.4028235e+38;
  pMVar16[4] = 3.4028235e+38;
                    /* Unresolved local var: int i@[???] */
  LinearAxis[0] = 0.0;
  LinearAxis[1] = 0.0;
  LinearAxis[2] = 0.0;
  bVar10 = LimitSingleAxis(local_1a4,&factors,5,params);
  if (bVar10) {
    bVar11 = ActuateJointAxes(local_1a4,(int *)((int)constraint + 0xb4),LinearAxis,ref2world[0],
                              ref2world[0],blist,&factors,5,clist);
    bVar10 = true;
    if (bVar11) goto LAB_00012dd5;
  }
  bVar10 = false;
LAB_00012dd5:
  if (bVar10) {
    uVar19 = 6;
  }
  else {
    uVar19 = 5;
  }
  MdtBclEndConstraint(clist,uVar19);
  return;
}


/* ==== MdtBclAddPrismatic ==== */

void MdtBclAddPrismatic(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                       MdtKeaBody **blist,void *params)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  MdtKeaBody *pMVar5;
  MdtKeaJBlockPair *paMVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  bool bVar10;
  bool bVar11;
  float *pfVar12;
  MeReal (*paMVar13) [4];
  MeReal *pMVar14;
  float *pfVar15;
  int iVar16;
  MeReal *pMVar17;
  MdtKeaTransformation *pMVar18;
  int iVar19;
  uint rows_added;
  MdtBclLimit *local_15c;
  int z_5;
  int z_4;
  int z_3;
  int z_2;
  int z_1;
  int z;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal RotationalAxis2 [3];
  MeReal RotationalAxis1 [3];
  MeReal tmp [3];
  MeReal pos_jJw [3];
  MeReal pos_iJw [3];
  MeReal pos_IJw [3];
  MeReal ref_Jw [4] [4];
  MeReal ref_Iw [4] [4];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MdtPrismatic * joint@[???]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal vel@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[???] */
  iVar19 = clist->num_rows_exc_padding;
  factors.c = clist->c + iVar19;
  pfVar12 = clist->xi + iVar19;
  pMVar17 = clist->lo + iVar19;
  pMVar14 = clist->hi + iVar19;
  factors.slipfactor = clist->slipfactor + iVar19;
  factors.xgamma = clist->xgamma + iVar19;
  z = 0;
  do {
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16;
    if (iVar16 < 0) {
      iVar19 = iVar16 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + z * 0x10) = 0;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16;
    if (iVar16 < 0) {
      iVar19 = iVar16 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z * 0x10 + (iVar19 >> 2) * 0xb0 + 0x60) = 0;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16;
    if (iVar16 < 0) {
      iVar19 = iVar16 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + z * 0x10 + 0x10) = 0;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16;
    if (iVar16 < 0) {
      iVar19 = iVar16 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z * 0x10 + (iVar19 >> 2) * 0xb0 + 0x70) = 0;
    z = z + 2;
  } while (z != 6);
  *pfVar12 = 0.0;
  *factors.slipfactor = 0.0;
  *factors.c = 0.0;
  *factors.xgamma = 0.0;
  z_1 = 0;
  do {
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 1;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + 4 + (iVar19 >> 2) * 0xb0 + z_1 * 0x10) = 0;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 1;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z_1 * 0x10 + (iVar19 >> 2) * 0xb0 + 100) = 0;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 1;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + z_1 * 0x10 + 0x14) = 0
    ;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 1;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z_1 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x74) = 0
    ;
    z_1 = z_1 + 2;
  } while (z_1 != 6);
  pfVar12[1] = 0.0;
  factors.slipfactor[1] = 0.0;
  factors.c[1] = 0.0;
  factors.xgamma[1] = 0.0;
  z_2 = 0;
  do {
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 2;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + 8 + (iVar19 >> 2) * 0xb0 + z_2 * 0x10) = 0;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 2;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z_2 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x68) = 0
    ;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 2;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + z_2 * 0x10 + 0x18) = 0
    ;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 2;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z_2 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x78) = 0
    ;
    z_2 = z_2 + 2;
  } while (z_2 != 6);
  pfVar12[2] = 0.0;
  factors.slipfactor[2] = 0.0;
  factors.c[2] = 0.0;
  factors.xgamma[2] = 0.0;
  z_3 = 0;
  do {
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 3;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + 0xc + (iVar19 >> 2) * 0xb0 + z_3 * 0x10) = 0;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 3;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z_3 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x6c) = 0
    ;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 3;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + z_3 * 0x10 + 0x1c) = 0
    ;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 3;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z_3 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x7c) = 0
    ;
    z_3 = z_3 + 2;
  } while (z_3 != 6);
  pfVar12[3] = 0.0;
  factors.slipfactor[3] = 0.0;
  factors.c[3] = 0.0;
  factors.xgamma[3] = 0.0;
  z_4 = 0;
  do {
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 4;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + 0x10 + (iVar19 >> 2) * 0xb0 + z_4 * 0x10) = 0
    ;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 4;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z_4 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x70) = 0
    ;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 4;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + z_4 * 0x10 + 0x20) = 0
    ;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 4;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z_4 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x80) = 0
    ;
    z_4 = z_4 + 2;
  } while (z_4 != 6);
  pfVar12[4] = 0.0;
  factors.slipfactor[4] = 0.0;
  factors.c[4] = 0.0;
  factors.xgamma[4] = 0.0;
  z_5 = 0;
  do {
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 5;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + 0x14 + (iVar19 >> 2) * 0xb0 + z_5 * 0x10) = 0
    ;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 5;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z_5 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x74) = 0
    ;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 5;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + z_5 * 0x10 + 0x24) = 0
    ;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 5;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar16 * 4 + z_5 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x84) = 0
    ;
    z_5 = z_5 + 2;
  } while (z_5 != 6);
  pfVar12[5] = 0.0;
  factors.slipfactor[5] = 0.0;
  paMVar13 = ref_Iw;
  factors.c[5] = 0.0;
  factors.xgamma[5] = 0.0;
  pfVar15 = (float *)((int)constraint + 0xbc);
  pMVar18 = tlist + *(int *)((int)constraint + 0xb4);
  iVar19 = 3;
  factors.xi = pfVar12;
  factors.lo = pMVar17;
  factors.hi = pMVar14;
  do {
    fVar1 = *pfVar15;
    fVar2 = pfVar15[1];
    fVar3 = pfVar15[2];
    fVar4 = pfVar15[3];
    pfVar15 = pfVar15 + 4;
    (*paMVar13)[0] =
         fVar4 * pMVar18->pos[0] +
         fVar3 * pMVar18->R2[0] + fVar1 * pMVar18->R0[0] + fVar2 * pMVar18->R1[0];
    (*paMVar13)[1] =
         fVar4 * pMVar18->pos[1] +
         fVar3 * pMVar18->R2[1] + fVar1 * pMVar18->R0[1] + fVar2 * pMVar18->R1[1];
    (*paMVar13)[2] =
         fVar4 * pMVar18->pos[2] +
         fVar3 * pMVar18->R2[2] + fVar1 * pMVar18->R0[2] + fVar2 * pMVar18->R1[2];
    (*paMVar13)[3] =
         fVar1 * pMVar18->R0[3] + fVar2 * pMVar18->R1[3] + fVar3 * pMVar18->R2[3] +
         fVar4 * pMVar18->pos[3];
    paMVar13 = paMVar13 + 1;
    iVar19 = iVar19 + -1;
  } while (-1 < iVar19);
  if (*(int *)((int)constraint + 0xb8) == -1) {
    ref_Jw[0][0] = *(MeReal *)((int)constraint + 0xfc);
    ref_Jw[0][1] = *(MeReal *)((int)constraint + 0x100);
    ref_Jw[0][2] = *(MeReal *)((int)constraint + 0x104);
    ref_Jw[0][3] = *(MeReal *)((int)constraint + 0x108);
    ref_Jw[1][0] = *(MeReal *)((int)constraint + 0x10c);
    ref_Jw[1][1] = *(MeReal *)((int)constraint + 0x110);
    ref_Jw[1][2] = *(MeReal *)((int)constraint + 0x114);
    ref_Jw[1][3] = *(MeReal *)((int)constraint + 0x118);
    ref_Jw[2][0] = *(MeReal *)((int)constraint + 0x11c);
    ref_Jw[2][1] = *(MeReal *)((int)constraint + 0x120);
    ref_Jw[2][2] = *(MeReal *)((int)constraint + 0x124);
    ref_Jw[2][3] = *(MeReal *)((int)constraint + 0x128);
    ref_Jw[3][0] = *(MeReal *)((int)constraint + 300);
    ref_Jw[3][1] = *(MeReal *)((int)constraint + 0x130);
    ref_Jw[3][2] = *(MeReal *)((int)constraint + 0x134);
    ref_Jw[3][3] = *(MeReal *)((int)constraint + 0x138);
  }
  else {
    MeMatrix4MultiplyMatrix
              (ref_Jw,(MeReal (*) [4])((int)constraint + 0xfc),
               tlist[*(int *)((int)constraint + 0xb8)].R0);
  }
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  fVar1 = ref_Jw[3][0] - ref_Iw[3][0];
  fVar8 = ref_Jw[3][1] - ref_Iw[3][1];
  fVar9 = ref_Jw[3][2] - ref_Iw[3][2];
  iVar19 = *(int *)((int)constraint + 0xb4);
  fVar4 = ref_Jw[3][0] - tlist[iVar19].pos[0];
  fVar3 = ref_Jw[3][1] - tlist[iVar19].pos[1];
  fVar2 = ref_Jw[3][2] - tlist[iVar19].pos[2];
  RotationalAxis1[0] = fVar3 * ref_Iw[0][2] - fVar2 * ref_Iw[0][1];
  RotationalAxis1[1] = fVar2 * ref_Iw[0][0] - fVar4 * ref_Iw[0][2];
  RotationalAxis1[2] = fVar4 * ref_Iw[0][1] - fVar3 * ref_Iw[0][0];
  iVar16 = *(int *)((int)constraint + 0xb8);
  if (iVar16 == -1) {
    pos_jJw[0] = ref_Jw[3][0];
    pos_jJw[1] = ref_Jw[3][1];
    pos_jJw[2] = ref_Jw[3][2];
  }
  else {
    pos_jJw[0] = ref_Jw[3][0] - tlist[iVar16].pos[0];
    pos_jJw[1] = ref_Jw[3][1] - tlist[iVar16].pos[1];
    pos_jJw[2] = ref_Jw[3][2] - tlist[iVar16].pos[2];
    RotationalAxis2[0] = pos_jJw[1] * ref_Iw[0][2] - pos_jJw[2] * ref_Iw[0][1];
    RotationalAxis2[2] = pos_jJw[0] * ref_Iw[0][1] - pos_jJw[1] * ref_Iw[0][0];
    RotationalAxis2[1] = pos_jJw[2] * ref_Iw[0][0] - pos_jJw[0] * ref_Iw[0][2];
  }
  pMVar5 = blist[iVar19];
  fVar7 = pMVar5->velrot[2] * RotationalAxis1[2] +
          pMVar5->velrot[1] * RotationalAxis1[1] + pMVar5->velrot[0] * RotationalAxis1[0] +
          ref_Iw[0][2] * pMVar5->vel[2] +
          ref_Iw[0][0] * pMVar5->vel[0] + ref_Iw[0][1] * pMVar5->vel[1];
  if (iVar16 != -1) {
    pMVar5 = blist[iVar16];
    fVar7 = (fVar7 - (ref_Iw[0][2] * pMVar5->vel[2] +
                     ref_Iw[0][1] * pMVar5->vel[1] + ref_Iw[0][0] * pMVar5->vel[0])) -
            (pMVar5->velrot[2] * RotationalAxis2[2] +
            pMVar5->velrot[0] * RotationalAxis2[0] + pMVar5->velrot[1] * RotationalAxis2[1]);
  }
  if (*(int *)((int)constraint + 0x164) == 0) {
    *(undefined4 *)((int)constraint + 0x170) = 0;
  }
  else {
    *(float *)((int)constraint + 0x188) = fVar7;
    *(float *)((int)constraint + 0x174) =
         -(ref_Iw[0][0] * fVar1 + ref_Iw[0][1] * fVar8 + ref_Iw[0][2] * fVar9);
    CalculateLimitPositionAndOvershoot((MdtBclLimit *)((int)constraint + 0x160),*(MeReal *)params,0)
    ;
  }
  local_15c = (MdtBclLimit *)((int)constraint + 0x160);
  *pfVar12 = ref_Iw[1][2] * fVar9 + ref_Iw[1][1] * fVar8 + ref_Iw[1][0] * fVar1;
  pfVar12[1] = ref_Iw[2][2] * fVar9 + ref_Iw[2][1] * fVar8 + ref_Iw[2][0] * fVar1;
  pfVar12[2] = ref_Jw[2][2] * ref_Iw[1][2] +
               ref_Jw[2][1] * ref_Iw[1][1] + ref_Jw[2][0] * ref_Iw[1][0];
  pfVar12[3] = ref_Jw[0][2] * ref_Iw[2][2] +
               ref_Jw[0][1] * ref_Iw[2][1] + ref_Jw[0][0] * ref_Iw[2][0];
  pfVar12[4] = ref_Jw[1][2] * ref_Iw[0][2] +
               ref_Jw[1][1] * ref_Iw[0][1] + ref_Jw[1][0] * ref_Iw[0][0];
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16;
  if (iVar16 < 0) {
    iVar19 = iVar16 + 3;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0) = -ref_Iw[1][0];
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16;
  if (iVar16 < 0) {
    iVar19 = iVar16 + 3;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x10) = -ref_Iw[1][1];
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16;
  if (iVar16 < 0) {
    iVar19 = iVar16 + 3;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x20) = -ref_Iw[1][2];
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 1;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 4;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + 4 + (iVar19 >> 2) * 0xb0) = -ref_Iw[2][0];
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 1;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 4;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x14) = -ref_Iw[2][1];
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 1;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 4;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x24) = -ref_Iw[2][2];
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16;
  if (iVar16 < 0) {
    iVar19 = iVar16 + 3;
  }
  paMVar6 = clist->Jstore;
  iVar16 = iVar16 + (iVar19 >> 2) * 0x2c;
  *(float *)((int)*paMVar6 + iVar16 * 4 + 0x30) = fVar2 * ref_Iw[1][1] - fVar3 * ref_Iw[1][2];
  *(float *)((int)*paMVar6 + iVar16 * 4 + 0x40) = fVar4 * ref_Iw[1][2] - fVar2 * ref_Iw[1][0];
  *(float *)((int)*paMVar6 + iVar16 * 4 + 0x50) = fVar3 * ref_Iw[1][0] - fVar4 * ref_Iw[1][1];
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 1;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 4;
  }
  iVar19 = iVar19 >> 2;
  paMVar6 = clist->Jstore;
  *(float *)((int)*paMVar6 + iVar16 * 4 + iVar19 * 0xb0 + 0x34) =
       fVar2 * ref_Iw[2][1] - fVar3 * ref_Iw[2][2];
  *(float *)((int)*paMVar6 + iVar16 * 4 + iVar19 * 0xb0 + 0x44) =
       fVar4 * ref_Iw[2][2] - fVar2 * ref_Iw[2][0];
  *(float *)((int)*paMVar6 + iVar16 * 4 + iVar19 * 0xb0 + 0x54) =
       fVar3 * ref_Iw[2][0] - fVar4 * ref_Iw[2][1];
  if (*(int *)((int)constraint + 0xb8) != -1) {
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16;
    if (iVar16 < 0) {
      iVar19 = iVar16 + 3;
    }
    *(MeReal *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x60) = ref_Iw[1][0];
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16;
    if (iVar16 < 0) {
      iVar19 = iVar16 + 3;
    }
    *(MeReal *)((int)*clist->Jstore + iVar16 * 4 + 0x70 + (iVar19 >> 2) * 0xb0) = ref_Iw[1][1];
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16;
    if (iVar16 < 0) {
      iVar19 = iVar16 + 3;
    }
    *(MeReal *)((int)*clist->Jstore + iVar16 * 4 + 0x80 + (iVar19 >> 2) * 0xb0) = ref_Iw[1][2];
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 1;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 4;
    }
    *(MeReal *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 100) = ref_Iw[2][0];
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 1;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 4;
    }
    *(MeReal *)((int)*clist->Jstore + iVar16 * 4 + 0x74 + (iVar19 >> 2) * 0xb0) = ref_Iw[2][1];
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 1;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 4;
    }
    *(MeReal *)((int)*clist->Jstore + iVar16 * 4 + 0x84 + (iVar19 >> 2) * 0xb0) = ref_Iw[2][2];
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16;
    if (iVar16 < 0) {
      iVar19 = iVar16 + 3;
    }
    paMVar6 = clist->Jstore;
    iVar16 = iVar16 + (iVar19 >> 2) * 0x2c;
    *(float *)((int)*paMVar6 + iVar16 * 4 + 0x90) =
         ref_Iw[1][2] * pos_jJw[1] - ref_Iw[1][1] * pos_jJw[2];
    *(float *)((int)*paMVar6 + iVar16 * 4 + 0xa0) =
         ref_Iw[1][0] * pos_jJw[2] - ref_Iw[1][2] * pos_jJw[0];
    *(float *)((int)*paMVar6 + iVar16 * 4 + 0xb0) =
         ref_Iw[1][1] * pos_jJw[0] - ref_Iw[1][0] * pos_jJw[1];
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 1;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 4;
    }
    iVar19 = iVar19 >> 2;
    paMVar6 = clist->Jstore;
    *(float *)((int)*paMVar6 + iVar16 * 4 + iVar19 * 0xb0 + 0x94) =
         ref_Iw[2][2] * pos_jJw[1] - ref_Iw[2][1] * pos_jJw[2];
    *(float *)((int)*paMVar6 + iVar16 * 4 + iVar19 * 0xb0 + 0xa4) =
         ref_Iw[2][0] * pos_jJw[2] - ref_Iw[2][2] * pos_jJw[0];
    *(float *)((int)*paMVar6 + iVar16 * 4 + iVar19 * 0xb0 + 0xb4) =
         ref_Iw[2][1] * pos_jJw[0] - ref_Iw[2][0] * pos_jJw[1];
  }
  fVar2 = ref_Iw[1][1] * ref_Jw[2][2] - ref_Iw[1][2] * ref_Jw[2][1];
  fVar3 = ref_Iw[1][2] * ref_Jw[2][0] - ref_Jw[2][2] * ref_Iw[1][0];
  fVar1 = ref_Iw[1][0] * ref_Jw[2][1] - ref_Iw[1][1] * ref_Jw[2][0];
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 2;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 5;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x38) = fVar2;
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 2;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 5;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x48) = fVar3;
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 2;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 5;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x58) = fVar1;
  if (*(int *)((int)constraint + 0xb8) != -1) {
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 2;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 5;
    }
    *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x98) = -fVar2;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 2;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 5;
    }
    *(float *)((int)*clist->Jstore + iVar16 * 4 + 0xa8 + (iVar19 >> 2) * 0xb0) = -fVar3;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 2;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 5;
    }
    *(float *)((int)*clist->Jstore + iVar16 * 4 + 0xb8 + (iVar19 >> 2) * 0xb0) = -fVar1;
  }
  fVar2 = ref_Iw[2][1] * ref_Jw[0][2] - ref_Iw[2][2] * ref_Jw[0][1];
  fVar3 = ref_Iw[2][2] * ref_Jw[0][0] - ref_Jw[0][2] * ref_Iw[2][0];
  fVar1 = ref_Iw[2][0] * ref_Jw[0][1] - ref_Iw[2][1] * ref_Jw[0][0];
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 3;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x3c) = fVar2;
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 3;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x4c) = fVar3;
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 3;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x5c) = fVar1;
  if (*(int *)((int)constraint + 0xb8) != -1) {
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 3;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 6;
    }
    *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x9c) = -fVar2;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 3;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 6;
    }
    *(float *)((int)*clist->Jstore + iVar16 * 4 + 0xac + (iVar19 >> 2) * 0xb0) = -fVar3;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 3;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 6;
    }
    *(float *)((int)*clist->Jstore + iVar16 * 4 + 0xbc + (iVar19 >> 2) * 0xb0) = -fVar1;
  }
  fVar2 = ref_Iw[0][1] * ref_Jw[1][2] - ref_Iw[0][2] * ref_Jw[1][1];
  fVar3 = ref_Iw[0][2] * ref_Jw[1][0] - ref_Jw[1][2] * ref_Iw[0][0];
  fVar1 = ref_Iw[0][0] * ref_Jw[1][1] - ref_Iw[0][1] * ref_Jw[1][0];
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 4;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 7;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x40) = fVar2;
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 4;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 7;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x50) = fVar3;
  iVar16 = clist->num_rows_inc_padding;
  iVar19 = iVar16 + 4;
  if (iVar19 < 0) {
    iVar19 = iVar16 + 7;
  }
  *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0x60) = fVar1;
  if (*(int *)((int)constraint + 0xb8) != -1) {
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 4;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 7;
    }
    *(float *)((int)*clist->Jstore + iVar16 * 4 + (iVar19 >> 2) * 0xb0 + 0xa0) = -fVar2;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 4;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 7;
    }
    *(float *)((int)*clist->Jstore + iVar16 * 4 + 0xb0 + (iVar19 >> 2) * 0xb0) = -fVar3;
    iVar16 = clist->num_rows_inc_padding;
    iVar19 = iVar16 + 4;
    if (iVar19 < 0) {
      iVar19 = iVar16 + 7;
    }
    *(float *)((int)clist->Jstore[1] + iVar16 * 4 + (iVar19 >> 2) * 0xb0) = -fVar1;
  }
  *pMVar17 = -3.4028235e+38;
  *pMVar14 = 3.4028235e+38;
  pMVar17[1] = -3.4028235e+38;
  pMVar14[1] = 3.4028235e+38;
  pMVar17[2] = -3.4028235e+38;
  pMVar14[2] = 3.4028235e+38;
  pMVar17[3] = -3.4028235e+38;
  pMVar14[3] = 3.4028235e+38;
  pMVar17[4] = -3.4028235e+38;
  pMVar14[4] = 3.4028235e+38;
  bVar10 = LimitSingleAxis(local_15c,&factors,5,params);
  if (bVar10) {
    bVar11 = ActuateJointAxes(local_15c,(int *)((int)constraint + 0xb4),ref_Iw[0],RotationalAxis1,
                              RotationalAxis2,blist,&factors,5,clist);
    bVar10 = true;
    if (bVar11) goto LAB_000145ba;
  }
  bVar10 = false;
LAB_000145ba:
  if (bVar10) {
    rows_added = 6;
  }
  else {
    rows_added = 5;
  }
  MdtBclEndConstraint(clist,rows_added);
  return;
}


/* ==== MdtBclAddCarWheel ==== */

void MdtBclAddCarWheel(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                      MdtKeaBody **blist,void *params)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  float fVar6;
  float fVar7;
  float *pfVar8;
  MeReal (*paMVar9) [4];
  MeReal *pMVar10;
  float *pfVar11;
  MeReal *pMVar12;
  float *pfVar13;
  MdtKeaTransformation *pMVar14;
  MeReal *pMVar15;
  float *pfVar16;
  int iVar17;
  uint rows_added;
  int z_5;
  int z_4;
  int z_3;
  int z_2;
  int z_1;
  int z;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal torque [3];
  MeReal at1 [3];
  MeReal tmp [3];
  MeReal b [3];
  MeReal a [3];
  MeReal ref2world [4] [4];
  MeReal ref1world [4] [4];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MdtCarWheel * joint@[???]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int limit@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[???] */
  iVar17 = clist->num_rows_exc_padding;
  pMVar15 = clist->c + iVar17;
  pfVar8 = clist->xi + iVar17;
  pMVar12 = clist->lo + iVar17;
  pMVar10 = clist->hi + iVar17;
  pfVar16 = clist->slipfactor + iVar17;
  pfVar13 = clist->xgamma + iVar17;
  z = 0;
  do {
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5;
    if (iVar5 < 0) {
      iVar17 = iVar5 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + z * 0x10) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5;
    if (iVar5 < 0) {
      iVar17 = iVar5 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z * 0x10 + (iVar17 >> 2) * 0xb0 + 0x60) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5;
    if (iVar5 < 0) {
      iVar17 = iVar5 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + z * 0x10 + 0x10) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5;
    if (iVar5 < 0) {
      iVar17 = iVar5 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z * 0x10 + (iVar17 >> 2) * 0xb0 + 0x70) = 0;
    z = z + 2;
  } while (z != 6);
  *pfVar8 = 0.0;
  *pfVar16 = 0.0;
  *pMVar15 = 0.0;
  *pfVar13 = 0.0;
  z_1 = 0;
  do {
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 1;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + 4 + (iVar17 >> 2) * 0xb0 + z_1 * 0x10) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 1;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_1 * 0x10 + (iVar17 >> 2) * 0xb0 + 100) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 1;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + z_1 * 0x10 + 0x14) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 1;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_1 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x74) = 0;
    z_1 = z_1 + 2;
  } while (z_1 != 6);
  pfVar8[1] = 0.0;
  pfVar16[1] = 0.0;
  pMVar15[1] = 0.0;
  pfVar13[1] = 0.0;
  z_2 = 0;
  do {
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 2;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + 8 + (iVar17 >> 2) * 0xb0 + z_2 * 0x10) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 2;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_2 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x68) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 2;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + z_2 * 0x10 + 0x18) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 2;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_2 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x78) = 0;
    z_2 = z_2 + 2;
  } while (z_2 != 6);
  pfVar8[2] = 0.0;
  pfVar16[2] = 0.0;
  pMVar15[2] = 0.0;
  pfVar13[2] = 0.0;
  z_3 = 0;
  do {
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 3;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + 0xc + (iVar17 >> 2) * 0xb0 + z_3 * 0x10) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 3;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_3 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x6c) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 3;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + z_3 * 0x10 + 0x1c) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 3;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_3 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x7c) = 0;
    z_3 = z_3 + 2;
  } while (z_3 != 6);
  pfVar8[3] = 0.0;
  pfVar16[3] = 0.0;
  pMVar15[3] = 0.0;
  pfVar13[3] = 0.0;
  z_4 = 0;
  do {
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 4;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + 0x10 + (iVar17 >> 2) * 0xb0 + z_4 * 0x10) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 4;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_4 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x70) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 4;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + z_4 * 0x10 + 0x20) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 4;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_4 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x80) = 0;
    z_4 = z_4 + 2;
  } while (z_4 != 6);
  pfVar8[4] = 0.0;
  pfVar16[4] = 0.0;
  pMVar15[4] = 0.0;
  pfVar13[4] = 0.0;
  z_5 = 0;
  do {
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + 0x14 + (iVar17 >> 2) * 0xb0 + z_5 * 0x10) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_5 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x74) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + z_5 * 0x10 + 0x24) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_5 * 0x10 + (iVar17 >> 2) * 0xb0 + 0x84) = 0;
    z_5 = z_5 + 2;
  } while (z_5 != 6);
  pfVar8[5] = 0.0;
  pfVar16[5] = 0.0;
  paMVar9 = ref1world;
  pMVar15[5] = 0.0;
  pfVar13[5] = 0.0;
  pfVar11 = (float *)((int)constraint + 0xbc);
  pMVar14 = tlist + *(int *)((int)constraint + 0xb4);
  iVar17 = 3;
  do {
    fVar1 = *pfVar11;
    fVar2 = pfVar11[1];
    fVar3 = pfVar11[2];
    fVar4 = pfVar11[3];
    pfVar11 = pfVar11 + 4;
    (*paMVar9)[0] =
         fVar4 * pMVar14->pos[0] +
         fVar3 * pMVar14->R2[0] + fVar1 * pMVar14->R0[0] + fVar2 * pMVar14->R1[0];
    (*paMVar9)[1] =
         fVar4 * pMVar14->pos[1] +
         fVar3 * pMVar14->R2[1] + fVar1 * pMVar14->R0[1] + fVar2 * pMVar14->R1[1];
    (*paMVar9)[2] =
         fVar4 * pMVar14->pos[2] +
         fVar3 * pMVar14->R2[2] + fVar1 * pMVar14->R0[2] + fVar2 * pMVar14->R1[2];
    (*paMVar9)[3] =
         fVar1 * pMVar14->R0[3] + fVar2 * pMVar14->R1[3] + fVar3 * pMVar14->R2[3] +
         fVar4 * pMVar14->pos[3];
    paMVar9 = paMVar9 + 1;
    iVar17 = iVar17 + -1;
  } while (-1 < iVar17);
  if (*(int *)((int)constraint + 0xb8) == -1) {
    ref2world[0][0] = *(MeReal *)((int)constraint + 0xfc);
    ref2world[0][1] = *(MeReal *)((int)constraint + 0x100);
    ref2world[0][2] = *(MeReal *)((int)constraint + 0x104);
    ref2world[0][3] = *(MeReal *)((int)constraint + 0x108);
    ref2world[1][0] = *(MeReal *)((int)constraint + 0x10c);
    ref2world[1][1] = *(MeReal *)((int)constraint + 0x110);
    ref2world[1][2] = *(MeReal *)((int)constraint + 0x114);
    ref2world[1][3] = *(MeReal *)((int)constraint + 0x118);
    ref2world[2][0] = *(MeReal *)((int)constraint + 0x11c);
    ref2world[2][1] = *(MeReal *)((int)constraint + 0x120);
    ref2world[2][2] = *(MeReal *)((int)constraint + 0x124);
    ref2world[2][3] = *(MeReal *)((int)constraint + 0x128);
    ref2world[3][0] = *(MeReal *)((int)constraint + 300);
    ref2world[3][1] = *(MeReal *)((int)constraint + 0x130);
    ref2world[3][2] = *(MeReal *)((int)constraint + 0x134);
    ref2world[3][3] = *(MeReal *)((int)constraint + 0x138);
  }
  else {
    MeMatrix4MultiplyMatrix
              (ref2world,(MeReal (*) [4])((int)constraint + 0xfc),
               tlist[*(int *)((int)constraint + 0xb8)].R0);
  }
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  iVar17 = *(int *)((int)constraint + 0xb4);
  fVar1 = ref1world[3][0] - tlist[iVar17].pos[0];
  fVar2 = ref1world[3][1] - tlist[iVar17].pos[1];
  fVar3 = ref1world[3][2] - tlist[iVar17].pos[2];
  if (ABS(ref1world[0][0]) <= ABS(ref1world[0][1])) {
                    /* Unresolved local var: MeReal scale@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[???] */
    fVar4 = 1.0 / SQRT(ref1world[0][1] * ref1world[0][1] + ref1world[0][2] * ref1world[0][2]);
    a[0] = 0.0;
    a[2] = ref1world[0][1] * fVar4;
    a[1] = -ref1world[0][2] * fVar4;
    b[0] = ref1world[0][1] * a[2] - ref1world[0][2] * a[1];
    b[2] = ref1world[0][0] * a[1];
    b[1] = -ref1world[0][0] * a[2];
  }
  else {
                    /* Unresolved local var: MeReal scale@[DW_OP_reg12(ST1)]
                       Unresolved local var: float __result@[???] */
    fVar4 = 1.0 / SQRT(ref1world[0][2] * ref1world[0][2] + ref1world[0][0] * ref1world[0][0]);
    a[1] = 0.0;
    a[0] = ref1world[0][2] * fVar4;
    a[2] = -ref1world[0][0] * fVar4;
    b[0] = ref1world[0][1] * a[2];
    b[1] = ref1world[0][2] * a[0] - a[2] * ref1world[0][0];
    b[2] = a[0] * -ref1world[0][1];
  }
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5;
  if (iVar5 < 0) {
    iVar17 = iVar5 + 3;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0) = ref1world[0][0];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5;
  if (iVar5 < 0) {
    iVar17 = iVar5 + 3;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x10) = ref1world[0][1];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5;
  if (iVar5 < 0) {
    iVar17 = iVar5 + 3;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x20) = ref1world[0][2];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5;
  if (iVar5 < 0) {
    iVar17 = iVar5 + 3;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x60) = -ref1world[0][0];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5;
  if (iVar5 < 0) {
    iVar17 = iVar5 + 3;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + 0x70 + (iVar17 >> 2) * 0xb0) = -ref1world[0][1];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5;
  if (iVar5 < 0) {
    iVar17 = iVar5 + 3;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + 0x80 + (iVar17 >> 2) * 0xb0) = -ref1world[0][2];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5;
  if (iVar5 < 0) {
    iVar17 = iVar5 + 3;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x30) =
       fVar2 * ref1world[0][2] - fVar3 * ref1world[0][1];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5;
  if (iVar5 < 0) {
    iVar17 = iVar5 + 3;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x40) =
       fVar3 * ref1world[0][0] - ref1world[0][2] * fVar1;
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5;
  if (iVar5 < 0) {
    iVar17 = iVar5 + 3;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x50) =
       fVar1 * ref1world[0][1] - fVar2 * ref1world[0][0];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 1;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 4;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + 4 + (iVar17 >> 2) * 0xb0) = a[0];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 1;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 4;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x14) = a[1];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 1;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 4;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x24) = a[2];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 1;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 4;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 100) = -a[0];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 1;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 4;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + 0x74 + (iVar17 >> 2) * 0xb0) = -a[1];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 1;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 4;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + 0x84 + (iVar17 >> 2) * 0xb0) = -a[2];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 1;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 4;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x34) =
       fVar2 * a[2] - fVar3 * a[1];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 1;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 4;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x44) =
       fVar3 * a[0] - a[2] * fVar1;
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 1;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 4;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x54) =
       fVar1 * a[1] - fVar2 * a[0];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 2;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 5;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + 8 + (iVar17 >> 2) * 0xb0) = b[0];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 2;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 5;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x18) = b[1];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 2;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 5;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x28) = b[2];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 2;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 5;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x68) = -b[0];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 2;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 5;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + 0x78 + (iVar17 >> 2) * 0xb0) = -b[1];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 2;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 5;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + 0x88 + (iVar17 >> 2) * 0xb0) = -b[2];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 2;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 5;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x38) =
       fVar2 * b[2] - fVar3 * b[1];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 2;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 5;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x48) =
       fVar3 * b[0] - b[2] * fVar1;
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 2;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 5;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x58) =
       fVar1 * b[1] - fVar2 * b[0];
  fVar4 = ref1world[3][0] - ref2world[3][0];
  fVar7 = ref1world[3][1] - ref2world[3][1];
  fVar6 = ref1world[3][2] - ref2world[3][2];
  *pfVar8 = fVar4 * ref1world[0][0] + fVar7 * ref1world[0][1] + fVar6 * ref1world[0][2];
  pfVar8[1] = a[2] * fVar6 + a[1] * fVar7 + a[0] * fVar4;
  pfVar8[2] = b[2] * fVar6 + b[1] * fVar7 + b[0] * fVar4;
  *pMVar12 = -3.4028235e+38;
  *pMVar10 = 3.4028235e+38;
  pMVar12[1] = -3.4028235e+38;
  pMVar10[1] = 3.4028235e+38;
  pMVar12[2] = -3.4028235e+38;
  pMVar10[2] = 3.4028235e+38;
  fVar4 = *pfVar8;
  iVar17 = 0;
  if (fVar4 <= -*(float *)((int)constraint + 0x180)) {
    if (-*(float *)((int)constraint + 0x17c) <= fVar4) {
                    /* Unresolved local var: MeReal hepsilon@[DW_OP_reg11(ST0)] */
      *pfVar8 = fVar4 + *(float *)((int)constraint + 0x184);
      fVar4 = 1.0 / (*(float *)((int)constraint + 0x160) * *(float *)params +
                    *(float *)((int)constraint + 0x164));
      *pfVar16 = fVar4;
      if (fVar4 < 0.0) {
        *pfVar16 = 0.0;
      }
      *pfVar13 = fVar4 * *(float *)params * *(float *)((int)constraint + 0x160);
    }
    else {
      *pfVar8 = *(float *)((int)constraint + 0x17c) + fVar4;
      iVar17 = 1;
      *pMVar12 = 0.0;
    }
  }
  else {
    iVar17 = -1;
    *pfVar8 = fVar4 + *(float *)((int)constraint + 0x180);
    *pMVar10 = 0.0;
  }
  if (iVar17 != 0) {
                    /* Unresolved local var: MeReal f@[DW_OP_reg13(ST2)] */
    fVar4 = *(float *)((int)constraint + 0x188);
    *pfVar16 = fVar4;
    if (fVar4 < 0.0) {
      *pfVar16 = 0.0;
    }
    if (iVar17 == 1) {
      fVar4 = (*(float *)((int)constraint + 0x17c) - *(float *)((int)constraint + 0x184)) *
              *(float *)((int)constraint + 0x160);
    }
    else {
      fVar4 = (*(float *)((int)constraint + 0x180) - *(float *)((int)constraint + 0x184)) *
              *(float *)((int)constraint + 0x160);
    }
    fVar6 = ref1world[0][0] * fVar4;
    fVar7 = ref1world[0][1] * fVar4;
    fVar4 = fVar4 * ref1world[0][2];
    blist[*(int *)((int)constraint + 0xb4)]->force[0] =
         fVar6 + blist[*(int *)((int)constraint + 0xb4)]->force[0];
    blist[*(int *)((int)constraint + 0xb4)]->force[1] =
         fVar7 + blist[*(int *)((int)constraint + 0xb4)]->force[1];
    blist[*(int *)((int)constraint + 0xb4)]->force[2] =
         fVar4 + blist[*(int *)((int)constraint + 0xb4)]->force[2];
    if (*(int *)((int)constraint + 0xb8) != -1) {
      blist[*(int *)((int)constraint + 0xb8)]->force[0] =
           blist[*(int *)((int)constraint + 0xb8)]->force[0] - fVar6;
      blist[*(int *)((int)constraint + 0xb8)]->force[1] =
           blist[*(int *)((int)constraint + 0xb8)]->force[1] - fVar7;
      blist[*(int *)((int)constraint + 0xb8)]->force[2] =
           blist[*(int *)((int)constraint + 0xb8)]->force[2] - fVar4;
    }
    blist[*(int *)((int)constraint + 0xb4)]->torque[0] =
         (fVar2 * fVar4 - fVar3 * fVar7) + blist[*(int *)((int)constraint + 0xb4)]->torque[0];
    blist[*(int *)((int)constraint + 0xb4)]->torque[1] =
         (fVar3 * fVar6 - fVar4 * fVar1) + blist[*(int *)((int)constraint + 0xb4)]->torque[1];
    blist[*(int *)((int)constraint + 0xb4)]->torque[2] =
         (fVar1 * fVar7 - fVar2 * fVar6) + blist[*(int *)((int)constraint + 0xb4)]->torque[2];
  }
  fVar2 = ref2world[1][1] * ref1world[0][2] - ref2world[1][2] * ref1world[0][1];
  fVar3 = ref2world[1][2] * ref1world[0][0] - ref1world[0][2] * ref2world[1][0];
  fVar1 = ref2world[1][0] * ref1world[0][1] - ref2world[1][1] * ref1world[0][0];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 3;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x3c) = -fVar2;
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 3;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x4c) = -fVar3;
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 3;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x5c) = -fVar1;
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 3;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x9c) = fVar2;
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 3;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + 0xac + (iVar17 >> 2) * 0xb0) = fVar3;
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 3;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + 0xbc + (iVar17 >> 2) * 0xb0) = fVar1;
  pfVar8[3] = ref1world[0][2] * ref2world[1][2] +
              ref1world[0][1] * ref2world[1][1] + ref1world[0][0] * ref2world[1][0];
  pMVar12[3] = -3.4028235e+38;
  pMVar10[3] = 3.4028235e+38;
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 4;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 7;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x40) = ref1world[0][0];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 4;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 7;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x50) = ref1world[0][1];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 4;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 7;
  }
  *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x60) = ref1world[0][2];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 4;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 7;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0xa0) = -ref1world[0][0];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 4;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 7;
  }
  *(float *)((int)*clist->Jstore + iVar5 * 4 + 0xb0 + (iVar17 >> 2) * 0xb0) = -ref1world[0][1];
  iVar5 = clist->num_rows_inc_padding;
  iVar17 = iVar5 + 4;
  if (iVar17 < 0) {
    iVar17 = iVar5 + 7;
  }
  *(float *)((int)clist->Jstore[1] + iVar5 * 4 + (iVar17 >> 2) * 0xb0) = -ref1world[0][2];
  pMVar15[4] = -*(MeReal *)((int)constraint + 0x168);
  pMVar12[4] = -*(MeReal *)((int)constraint + 0x16c);
  pMVar10[4] = *(MeReal *)((int)constraint + 0x16c);
  if (*(int *)((int)constraint + 0x178) != 0) {
    pfVar8[4] = (ref2world[1][0] * ref1world[1][1] - ref2world[1][1] * ref1world[1][0]) *
                ref1world[0][2] +
                (ref2world[1][1] * ref1world[1][2] - ref2world[1][2] * ref1world[1][1]) *
                ref1world[0][0] +
                (ref2world[1][2] * ref1world[1][0] - ref1world[1][2] * ref2world[1][0]) *
                ref1world[0][1];
  }
  if (*(float *)((int)constraint + 0x174) <= 0.0001) {
    rows_added = 5;
  }
  else {
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 8;
    }
    *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x44) = ref2world[1][0];
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 8;
    }
    *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0x54) = ref2world[1][1];
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 8;
    }
    *(MeReal *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 100) = ref2world[1][2];
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 8;
    }
    *(float *)((int)*clist->Jstore + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 0xa4) = -ref2world[1][0];
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 8;
    }
    *(float *)((int)*clist->Jstore + iVar5 * 4 + 0xb4 + (iVar17 >> 2) * 0xb0) = -ref2world[1][1];
    iVar5 = clist->num_rows_inc_padding;
    iVar17 = iVar5 + 5;
    if (iVar17 < 0) {
      iVar17 = iVar5 + 8;
    }
    *(float *)((int)clist->Jstore[1] + iVar5 * 4 + (iVar17 >> 2) * 0xb0 + 4) = -ref2world[1][2];
    pMVar15[5] = -*(MeReal *)((int)constraint + 0x170);
    pMVar12[5] = -*(MeReal *)((int)constraint + 0x174);
    pMVar10[5] = *(MeReal *)((int)constraint + 0x174);
    rows_added = 6;
  }
  MdtBclEndConstraint(clist,rows_added);
  return;
}


/* ==== MdtBclAddFixedPath ==== */

void MdtBclAddFixedPath(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                       MdtKeaBody **blist,void *params)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  MeReal *pMVar6;
  MeReal (*paMVar7) [4];
  uint uVar8;
  MeReal *pMVar9;
  float *pfVar10;
  MeReal *pMVar11;
  MdtKeaTransformation *pMVar12;
  MeReal *pMVar13;
  int iVar14;
  int z_2;
  int z_1;
  int z;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *xi;
  MeReal *c;
  MeReal v2 [4];
  MeReal negc [4];
  MeReal temp [4];
  MeReal ref2world [4] [4];
  MeReal ref1world [4] [4];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MeReal * lo@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * hi@[DW_OP_reg1(ECX)]
                       Unresolved local var: MdtFixedPath * joint@[???]
                       Unresolved local var: uint i@[DW_OP_reg0(EAX)] */
  iVar14 = clist->num_rows_exc_padding;
  pMVar9 = clist->c + iVar14;
  pMVar11 = clist->xi + iVar14;
  pMVar6 = clist->slipfactor + iVar14;
  pMVar13 = clist->xgamma + iVar14;
  z = 0;
  do {
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5;
    if (iVar5 < 0) {
      iVar14 = iVar5 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + (iVar14 >> 2) * 0xb0 + z * 0x10) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5;
    if (iVar5 < 0) {
      iVar14 = iVar5 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z * 0x10 + (iVar14 >> 2) * 0xb0 + 0x60) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5;
    if (iVar5 < 0) {
      iVar14 = iVar5 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + (iVar14 >> 2) * 0xb0 + z * 0x10 + 0x10) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5;
    if (iVar5 < 0) {
      iVar14 = iVar5 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z * 0x10 + (iVar14 >> 2) * 0xb0 + 0x70) = 0;
    z = z + 2;
  } while (z != 6);
  *pMVar11 = 0.0;
  *pMVar6 = 0.0;
  *pMVar9 = 0.0;
  *pMVar13 = 0.0;
  z_1 = 0;
  do {
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5 + 1;
    if (iVar14 < 0) {
      iVar14 = iVar5 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + 4 + (iVar14 >> 2) * 0xb0 + z_1 * 0x10) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5 + 1;
    if (iVar14 < 0) {
      iVar14 = iVar5 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_1 * 0x10 + (iVar14 >> 2) * 0xb0 + 100) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5 + 1;
    if (iVar14 < 0) {
      iVar14 = iVar5 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + (iVar14 >> 2) * 0xb0 + z_1 * 0x10 + 0x14) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5 + 1;
    if (iVar14 < 0) {
      iVar14 = iVar5 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_1 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x74) = 0;
    z_1 = z_1 + 2;
  } while (z_1 != 6);
  pMVar11[1] = 0.0;
  pMVar6[1] = 0.0;
  pMVar9[1] = 0.0;
  pMVar13[1] = 0.0;
  z_2 = 0;
  do {
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5 + 2;
    if (iVar14 < 0) {
      iVar14 = iVar5 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + 8 + (iVar14 >> 2) * 0xb0 + z_2 * 0x10) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5 + 2;
    if (iVar14 < 0) {
      iVar14 = iVar5 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_2 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x68) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5 + 2;
    if (iVar14 < 0) {
      iVar14 = iVar5 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + (iVar14 >> 2) * 0xb0 + z_2 * 0x10 + 0x18) = 0;
    iVar5 = clist->num_rows_inc_padding;
    iVar14 = iVar5 + 2;
    if (iVar14 < 0) {
      iVar14 = iVar5 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar5 * 4 + z_2 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x78) = 0;
    z_2 = z_2 + 2;
  } while (z_2 != 6);
  pMVar11[2] = 0.0;
  pMVar6[2] = 0.0;
  pMVar9[2] = 0.0;
  pMVar13[2] = 0.0;
                    /* Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[???] */
  paMVar7 = ref1world;
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  pMVar12 = tlist + *(int *)((int)constraint + 0xb4);
  pfVar10 = (float *)((int)constraint + 0xbc);
  iVar14 = 3;
  do {
    fVar1 = *pfVar10;
    fVar2 = pfVar10[1];
    fVar3 = pfVar10[2];
    fVar4 = pfVar10[3];
    pfVar10 = pfVar10 + 4;
    (*paMVar7)[0] =
         fVar4 * pMVar12->pos[0] +
         fVar3 * pMVar12->R2[0] + fVar1 * pMVar12->R0[0] + fVar2 * pMVar12->R1[0];
    (*paMVar7)[1] =
         fVar4 * pMVar12->pos[1] +
         fVar3 * pMVar12->R2[1] + fVar1 * pMVar12->R0[1] + fVar2 * pMVar12->R1[1];
    (*paMVar7)[2] =
         fVar4 * pMVar12->pos[2] +
         fVar3 * pMVar12->R2[2] + fVar1 * pMVar12->R0[2] + fVar2 * pMVar12->R1[2];
    (*paMVar7)[3] =
         fVar1 * pMVar12->R0[3] + fVar2 * pMVar12->R1[3] + fVar3 * pMVar12->R2[3] +
         fVar4 * pMVar12->pos[3];
    paMVar7 = paMVar7 + 1;
    iVar14 = iVar14 + -1;
  } while (-1 < iVar14);
  if (*(int *)((int)constraint + 0xb8) == -1) {
    ref2world[0][0] = *(MeReal *)((int)constraint + 0xfc);
    ref2world[0][1] = *(MeReal *)((int)constraint + 0x100);
    ref2world[0][2] = *(MeReal *)((int)constraint + 0x104);
    ref2world[0][3] = *(MeReal *)((int)constraint + 0x108);
    ref2world[1][0] = *(MeReal *)((int)constraint + 0x10c);
    ref2world[1][1] = *(MeReal *)((int)constraint + 0x110);
    ref2world[1][2] = *(MeReal *)((int)constraint + 0x114);
    ref2world[1][3] = *(MeReal *)((int)constraint + 0x118);
    ref2world[2][0] = *(MeReal *)((int)constraint + 0x11c);
    ref2world[2][1] = *(MeReal *)((int)constraint + 0x120);
    ref2world[2][2] = *(MeReal *)((int)constraint + 0x124);
    ref2world[2][3] = *(MeReal *)((int)constraint + 0x128);
    ref2world[3][0] = *(MeReal *)((int)constraint + 300);
    ref2world[3][1] = *(MeReal *)((int)constraint + 0x130);
    ref2world[3][2] = *(MeReal *)((int)constraint + 0x134);
    ref2world[3][3] = *(MeReal *)((int)constraint + 0x138);
    iVar14 = -1;
  }
  else {
    MeMatrix4MultiplyMatrix
              (ref2world,(MeReal (*) [4])((int)constraint + 0xfc),
               tlist[*(int *)((int)constraint + 0xb8)].R0);
    iVar14 = *(int *)((int)constraint + 0xb8);
  }
  MdtBclAddSphericalRowsWorld
            (clist,tlist[*(int *)((int)constraint + 0xb4)].pos,tlist[iVar14].pos,ref1world[3],
             ref2world[3],(uint)(iVar14 != -1),(MeReal *)((int)constraint + 0x13c),
             (MeReal *)((int)constraint + 0x148));
  fVar1 = *(float *)((int)constraint + 0x178);
  fVar2 = *(float *)((int)constraint + 0x17c);
  fVar3 = *(float *)((int)constraint + 0x180);
  negc[0] = ref1world[0][0] * fVar1 + ref1world[1][0] * fVar2 + ref1world[2][0] * fVar3;
  negc[1] = ref1world[2][1] * fVar3 + ref1world[0][1] * fVar1 + ref1world[1][1] * fVar2;
  negc[2] = ref1world[0][2] * fVar1 + ref1world[1][2] * fVar2 + ref1world[2][2] * fVar3;
  negc[3] = fVar1 * ref1world[0][3] + fVar2 * ref1world[1][3] + fVar3 * ref1world[2][3];
  if (*(int *)((int)constraint + 0xb8) == -1) {
    uVar8 = 0;
    do {
      negc[uVar8] = negc[uVar8] - *(float *)((int)constraint + uVar8 * 4 + 0x184);
      uVar8 = uVar8 + 1;
    } while (uVar8 < 3);
  }
  else {
    fVar1 = *(float *)((int)constraint + 0x184);
    fVar2 = *(float *)((int)constraint + 0x188);
    fVar3 = *(float *)((int)constraint + 0x18c);
    negc[0] = negc[0] - (ref2world[0][0] * fVar1 + ref2world[1][0] * fVar2 + ref2world[2][0] * fVar3
                        );
    negc[1] = negc[1] - (ref2world[2][1] * fVar3 + ref2world[0][1] * fVar1 + ref2world[1][1] * fVar2
                        );
    negc[2] = negc[2] - (ref2world[0][2] * fVar1 + ref2world[1][2] * fVar2 + ref2world[2][2] * fVar3
                        );
  }
  *pMVar9 = -negc[0];
  pMVar9[1] = -negc[1];
  pMVar9[2] = -negc[2];
  MdtBclEndConstraint(clist,3);
  return;
}


/* ==== MdtBclAddUniversal ==== */

void MdtBclAddUniversal(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                       MdtKeaBody **blist,void *params)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  MeReal *pMVar6;
  MeReal *pMVar7;
  int iVar8;
  MeReal *pMVar9;
  MeReal (*paMVar10) [4];
  float *pfVar11;
  MeReal *pMVar12;
  MdtKeaTransformation *pMVar13;
  MeReal *pMVar14;
  MeReal *pMVar15;
  int iVar16;
  int z_3;
  int z_2;
  int z_1;
  int z;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal universalPerpendicular [3];
  MeReal ref2world [4] [4];
  MeReal ref1world [4] [4];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MdtUniversal * joint@[???]
                       Unresolved local var: uint i@[DW_OP_reg3(EBX)] */
  iVar5 = clist->num_rows_exc_padding;
  pMVar14 = clist->c + iVar5;
  pMVar12 = clist->xi + iVar5;
  pMVar6 = clist->lo;
  pMVar7 = clist->hi;
  pMVar15 = clist->slipfactor + iVar5;
  pMVar9 = clist->xgamma + iVar5;
  z = 0;
  do {
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8;
    if (iVar8 < 0) {
      iVar16 = iVar8 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + (iVar16 >> 2) * 0xb0 + z * 0x10) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8;
    if (iVar8 < 0) {
      iVar16 = iVar8 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + z * 0x10 + (iVar16 >> 2) * 0xb0 + 0x60) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8;
    if (iVar8 < 0) {
      iVar16 = iVar8 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + (iVar16 >> 2) * 0xb0 + z * 0x10 + 0x10) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8;
    if (iVar8 < 0) {
      iVar16 = iVar8 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + z * 0x10 + (iVar16 >> 2) * 0xb0 + 0x70) = 0;
    z = z + 2;
  } while (z != 6);
  *pMVar12 = 0.0;
  *pMVar15 = 0.0;
  *pMVar14 = 0.0;
  *pMVar9 = 0.0;
  z_1 = 0;
  do {
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 1;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + 4 + (iVar16 >> 2) * 0xb0 + z_1 * 0x10) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 1;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + z_1 * 0x10 + (iVar16 >> 2) * 0xb0 + 100) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 1;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + (iVar16 >> 2) * 0xb0 + z_1 * 0x10 + 0x14) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 1;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + z_1 * 0x10 + (iVar16 >> 2) * 0xb0 + 0x74) = 0;
    z_1 = z_1 + 2;
  } while (z_1 != 6);
  pMVar12[1] = 0.0;
  pMVar15[1] = 0.0;
  pMVar14[1] = 0.0;
  pMVar9[1] = 0.0;
  z_2 = 0;
  do {
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 2;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + 8 + (iVar16 >> 2) * 0xb0 + z_2 * 0x10) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 2;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + z_2 * 0x10 + (iVar16 >> 2) * 0xb0 + 0x68) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 2;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + (iVar16 >> 2) * 0xb0 + z_2 * 0x10 + 0x18) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 2;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + z_2 * 0x10 + (iVar16 >> 2) * 0xb0 + 0x78) = 0;
    z_2 = z_2 + 2;
  } while (z_2 != 6);
  pMVar12[2] = 0.0;
  pMVar15[2] = 0.0;
  pMVar14[2] = 0.0;
  pMVar9[2] = 0.0;
  z_3 = 0;
  do {
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 3;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + 0xc + (iVar16 >> 2) * 0xb0 + z_3 * 0x10) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 3;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + z_3 * 0x10 + (iVar16 >> 2) * 0xb0 + 0x6c) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 3;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + (iVar16 >> 2) * 0xb0 + z_3 * 0x10 + 0x1c) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 3;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + z_3 * 0x10 + (iVar16 >> 2) * 0xb0 + 0x7c) = 0;
    z_3 = z_3 + 2;
  } while (z_3 != 6);
  pMVar12[3] = 0.0;
  pMVar15[3] = 0.0;
  pMVar14[3] = 0.0;
                    /* Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[???] */
  paMVar10 = ref1world;
  pMVar9[3] = 0.0;
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  pMVar13 = tlist + *(int *)((int)constraint + 0xb4);
  pfVar11 = (float *)((int)constraint + 0xbc);
  iVar16 = 3;
  do {
    fVar1 = *pfVar11;
    fVar2 = pfVar11[1];
    fVar3 = pfVar11[2];
    fVar4 = pfVar11[3];
    pfVar11 = pfVar11 + 4;
    (*paMVar10)[0] =
         fVar4 * pMVar13->pos[0] +
         fVar3 * pMVar13->R2[0] + fVar1 * pMVar13->R0[0] + fVar2 * pMVar13->R1[0];
    (*paMVar10)[1] =
         fVar4 * pMVar13->pos[1] +
         fVar3 * pMVar13->R2[1] + fVar1 * pMVar13->R0[1] + fVar2 * pMVar13->R1[1];
    (*paMVar10)[2] =
         fVar4 * pMVar13->pos[2] +
         fVar3 * pMVar13->R2[2] + fVar1 * pMVar13->R0[2] + fVar2 * pMVar13->R1[2];
    (*paMVar10)[3] =
         fVar1 * pMVar13->R0[3] + fVar2 * pMVar13->R1[3] + fVar3 * pMVar13->R2[3] +
         fVar4 * pMVar13->pos[3];
    paMVar10 = paMVar10 + 1;
    iVar16 = iVar16 + -1;
  } while (-1 < iVar16);
  if (*(int *)((int)constraint + 0xb8) == -1) {
    ref2world[0][0] = *(MeReal *)((int)constraint + 0xfc);
    ref2world[0][1] = *(MeReal *)((int)constraint + 0x100);
    ref2world[0][2] = *(MeReal *)((int)constraint + 0x104);
    ref2world[0][3] = *(MeReal *)((int)constraint + 0x108);
    ref2world[1][0] = *(MeReal *)((int)constraint + 0x10c);
    ref2world[1][1] = *(MeReal *)((int)constraint + 0x110);
    ref2world[1][2] = *(MeReal *)((int)constraint + 0x114);
    ref2world[1][3] = *(MeReal *)((int)constraint + 0x118);
    ref2world[2][0] = *(MeReal *)((int)constraint + 0x11c);
    ref2world[2][1] = *(MeReal *)((int)constraint + 0x120);
    ref2world[2][2] = *(MeReal *)((int)constraint + 0x124);
    ref2world[2][3] = *(MeReal *)((int)constraint + 0x128);
    ref2world[3][0] = *(MeReal *)((int)constraint + 300);
    ref2world[3][1] = *(MeReal *)((int)constraint + 0x130);
    ref2world[3][2] = *(MeReal *)((int)constraint + 0x134);
    ref2world[3][3] = *(MeReal *)((int)constraint + 0x138);
    iVar16 = -1;
  }
  else {
    MeMatrix4MultiplyMatrix
              (ref2world,(MeReal (*) [4])((int)constraint + 0xfc),
               tlist[*(int *)((int)constraint + 0xb8)].R0);
    iVar16 = *(int *)((int)constraint + 0xb8);
  }
  MdtBclAddSphericalRowsWorld
            (clist,tlist[*(int *)((int)constraint + 0xb4)].pos,tlist[iVar16].pos,ref1world[3],
             ref2world[3],(uint)(iVar16 != -1),(MeReal *)((int)constraint + 0x13c),
             (MeReal *)((int)constraint + 0x148));
  fVar2 = ref1world[0][1] * ref2world[1][2] - ref1world[0][2] * ref2world[1][1];
  fVar3 = ref1world[0][2] * ref2world[1][0] - ref2world[1][2] * ref1world[0][0];
  fVar1 = ref1world[0][0] * ref2world[1][1] - ref1world[0][1] * ref2world[1][0];
  iVar8 = clist->num_rows_inc_padding;
  iVar16 = iVar8 + 3;
  if (iVar16 < 0) {
    iVar16 = iVar8 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar8 * 4 + (iVar16 >> 2) * 0xb0 + 0x3c) = fVar2;
  iVar8 = clist->num_rows_inc_padding;
  iVar16 = iVar8 + 3;
  if (iVar16 < 0) {
    iVar16 = iVar8 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar8 * 4 + (iVar16 >> 2) * 0xb0 + 0x4c) = fVar3;
  iVar8 = clist->num_rows_inc_padding;
  iVar16 = iVar8 + 3;
  if (iVar16 < 0) {
    iVar16 = iVar8 + 6;
  }
  *(float *)((int)*clist->Jstore + iVar8 * 4 + (iVar16 >> 2) * 0xb0 + 0x5c) = fVar1;
  if (*(int *)((int)constraint + 0xb8) != -1) {
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 3;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 6;
    }
    *(float *)((int)*clist->Jstore + iVar8 * 4 + (iVar16 >> 2) * 0xb0 + 0x9c) = -fVar2;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 3;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 6;
    }
    *(float *)((int)*clist->Jstore + iVar8 * 4 + 0xac + (iVar16 >> 2) * 0xb0) = -fVar3;
    iVar8 = clist->num_rows_inc_padding;
    iVar16 = iVar8 + 3;
    if (iVar16 < 0) {
      iVar16 = iVar8 + 6;
    }
    *(float *)((int)*clist->Jstore + iVar8 * 4 + 0xbc + (iVar16 >> 2) * 0xb0) = -fVar1;
  }
  pMVar6[iVar5 + 3] = -3.4028235e+38;
  pMVar7[iVar5 + 3] = 3.4028235e+38;
  pMVar12[3] = ref2world[1][2] * ref1world[0][2] +
               ref2world[1][1] * ref1world[0][1] + ref2world[1][0] * ref1world[0][0];
  MdtBclEndConstraint(clist,4);
  return;
}


/* ==== MdtBclAddSkeletal ==== */

/* WARNING: Unknown calling convention */

void MdtBclAddSkeletal(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                      MdtKeaBody **blist,void *params)

{
  float fVar1;
  float fVar2;
  float fVar3;
  int iVar4;
  MeReal *pMVar5;
  MeReal *pMVar6;
  MdtKeaJBlockPair *paMVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  MeReal *pMVar14;
  uint uVar15;
  MeReal *pMVar16;
  int iVar17;
  int iVar18;
  int iVar19;
  MeReal *pMVar20;
  MeReal *pMVar21;
  int iVar22;
  MdtKeaTransformation *pMVar23;
  uint uVar24;
  MeReal PositionError;
  MeReal r20_div_sq_tan_qcla2;
  MeReal r10_div_sq_tan_qcla1;
  MeReal q0q2_2;
  MeReal q3sq_2;
  MeReal q1sq_2;
  MeReal q0q2_1;
  MeReal q3sq_1;
  MeReal q2sq_1;
  MeReal q1sq_1;
  MeReal q0q2;
  MeReal q3sq;
  MeReal q2sq;
  MeReal q1sq;
  MeI32 nRows;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal Jw [3];
  MeReal JwQ0 [3];
  MeReal JwQ1 [3];
  MeReal Rrel [3] [3];
  MeReal qmul [4] [4];
  MeReal qrel [4];
  MeReal qbcw [4];
  MeReal qacw [4];
  MeReal qbcb [4];
  MeReal qaca [4];
  MeReal Rbcw [4] [4];
  MeReal Racw [4] [4];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MdtSkeletal * joint@[???]
                       Unresolved local var: uint i@[DW_OP_reg2(EDX)]
                       Unresolved local var: uint j@[???] */
  iVar4 = clist->num_rows_exc_padding;
  pMVar16 = clist->c + iVar4;
  pMVar20 = clist->xi + iVar4;
  pMVar5 = clist->lo;
  pMVar6 = clist->hi;
  pMVar14 = clist->slipfactor + iVar4;
  pMVar21 = clist->xgamma + iVar4;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar19 = 0;
  do {
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17;
    if (iVar17 < 0) {
      iVar18 = iVar17 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10) = 0;
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17;
    if (iVar17 < 0) {
      iVar18 = iVar17 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 0x60)
         = 0;
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17;
    if (iVar17 < 0) {
      iVar18 = iVar17 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10 + 0x10)
         = 0;
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17;
    if (iVar17 < 0) {
      iVar18 = iVar17 + 3;
    }
    iVar22 = iVar19 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 0x70)
         = 0;
    iVar19 = iVar22;
  } while (iVar22 != 6);
  *pMVar20 = 0.0;
  *pMVar14 = 0.0;
  *pMVar16 = 0.0;
  *pMVar21 = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar19 = 0;
  do {
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17 + 1;
    if (iVar18 < 0) {
      iVar18 = iVar17 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + 4 + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10) = 0
    ;
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17 + 1;
    if (iVar18 < 0) {
      iVar18 = iVar17 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 100) =
         0;
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17 + 1;
    if (iVar18 < 0) {
      iVar18 = iVar17 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10 + 0x14)
         = 0;
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17 + 1;
    if (iVar18 < 0) {
      iVar18 = iVar17 + 4;
    }
    iVar22 = iVar19 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 0x74)
         = 0;
    iVar19 = iVar22;
  } while (iVar22 != 6);
  pMVar20[1] = 0.0;
  pMVar14[1] = 0.0;
  pMVar16[1] = 0.0;
  pMVar21[1] = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar19 = 0;
  do {
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17 + 2;
    if (iVar18 < 0) {
      iVar18 = iVar17 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + 8 + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10) = 0
    ;
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17 + 2;
    if (iVar18 < 0) {
      iVar18 = iVar17 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 0x68)
         = 0;
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17 + 2;
    if (iVar18 < 0) {
      iVar18 = iVar17 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10 + 0x18)
         = 0;
    iVar17 = clist->num_rows_inc_padding;
    iVar18 = iVar17 + 2;
    if (iVar18 < 0) {
      iVar18 = iVar17 + 5;
    }
    iVar22 = iVar19 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 0x78)
         = 0;
    iVar19 = iVar22;
  } while (iVar22 != 6);
  pMVar20[2] = 0.0;
  pMVar14[2] = 0.0;
  pMVar16[2] = 0.0;
  pMVar21[2] = 0.0;
  nRows = 3;
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  MeQuaternionFromTM(qaca,(void *)((int)constraint + 0xbc));
  iVar19 = *(int *)((int)constraint + 0x54);
  qacw[0] = ((qaca[0] * *(float *)(iVar19 + 0xb0) - qaca[1] * *(float *)(iVar19 + 0xb4)) -
            qaca[2] * *(float *)(iVar19 + 0xb8)) - qaca[3] * *(float *)(iVar19 + 0xbc);
  qacw[1] = (*(float *)(iVar19 + 0xb8) * qaca[3] +
            qaca[1] * *(float *)(iVar19 + 0xb0) + qaca[0] * *(float *)(iVar19 + 0xb4)) -
            *(float *)(iVar19 + 0xbc) * qaca[2];
  qacw[2] = (*(float *)(iVar19 + 0xbc) * qaca[1] +
            qaca[2] * *(float *)(iVar19 + 0xb0) + qaca[0] * *(float *)(iVar19 + 0xb8)) -
            *(float *)(iVar19 + 0xb4) * qaca[3];
                    /* Unresolved local var: MeReal q0sq@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal q0t2@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q1t2@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q0q1@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal q0q3@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q1q2@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q1q3@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q2q3@[DW_OP_reg12(ST1)] */
  fVar10 = qacw[2] * qacw[2];
  qacw[3] = (qaca[2] * *(float *)(iVar19 + 0xb4) +
            qaca[0] * *(float *)(iVar19 + 0xbc) + qaca[3] * *(float *)(iVar19 + 0xb0)) -
            qaca[1] * *(float *)(iVar19 + 0xb8);
  fVar9 = qacw[0] + qacw[0];
  fVar1 = qacw[3] * qacw[3];
  Racw[0][0] = ((qacw[1] * qacw[1] + qacw[0] * qacw[0]) - fVar10) - fVar1;
  fVar8 = (qacw[1] + qacw[1]) * qacw[2];
  fVar2 = (qacw[1] + qacw[1]) * qacw[3];
  Racw[1][0] = fVar8 - fVar9 * qacw[3];
  fVar3 = (qacw[2] + qacw[2]) * qacw[3];
  Racw[0][1] = fVar8 + fVar9 * qacw[3];
  fVar8 = qacw[0] * qacw[0] - qacw[1] * qacw[1];
  Racw[2][0] = fVar9 * qacw[2] + fVar2;
  Racw[0][2] = fVar2 - fVar9 * qacw[2];
  Racw[1][1] = (fVar10 + fVar8) - fVar1;
  Racw[2][2] = (fVar8 - fVar10) + fVar1;
  Racw[2][1] = fVar3 - fVar9 * qacw[1];
  Racw[1][2] = fVar9 * qacw[1] + fVar3;
  fVar1 = *(float *)((int)constraint + 0xec);
  fVar2 = *(float *)((int)constraint + 0xf0);
  pMVar23 = tlist + *(int *)((int)constraint + 0xb4);
  fVar3 = *(float *)((int)constraint + 0xf4);
  Racw[3][0] = fVar3 * pMVar23->R2[0] + fVar2 * pMVar23->R1[0] + fVar1 * pMVar23->R0[0] +
               pMVar23->pos[0];
  Racw[3][1] = fVar3 * pMVar23->R2[1] + fVar2 * pMVar23->R1[1] + fVar1 * pMVar23->R0[1] +
               pMVar23->pos[1];
  Racw[3][2] = fVar3 * pMVar23->R2[2] + fVar1 * pMVar23->R0[2] + fVar2 * pMVar23->R1[2] +
               pMVar23->pos[2];
  if (*(int *)((int)constraint + 0xb8) == -1) {
    Rbcw[0][0] = *(MeReal *)((int)constraint + 0xfc);
    Rbcw[0][1] = *(MeReal *)((int)constraint + 0x100);
    Rbcw[0][2] = *(MeReal *)((int)constraint + 0x104);
    Rbcw[0][3] = *(MeReal *)((int)constraint + 0x108);
    Rbcw[1][0] = *(MeReal *)((int)constraint + 0x10c);
    Rbcw[1][1] = *(MeReal *)((int)constraint + 0x110);
    Rbcw[1][2] = *(MeReal *)((int)constraint + 0x114);
    Rbcw[1][3] = *(MeReal *)((int)constraint + 0x118);
    Rbcw[2][0] = *(MeReal *)((int)constraint + 0x11c);
    Rbcw[2][1] = *(MeReal *)((int)constraint + 0x120);
    Rbcw[2][2] = *(MeReal *)((int)constraint + 0x124);
    Rbcw[2][3] = *(MeReal *)((int)constraint + 0x128);
    Rbcw[3][0] = *(MeReal *)((int)constraint + 300);
    Rbcw[3][1] = *(MeReal *)((int)constraint + 0x130);
    Rbcw[3][2] = *(MeReal *)((int)constraint + 0x134);
    Rbcw[3][3] = *(MeReal *)((int)constraint + 0x138);
    MeQuaternionFromTM(qbcw,Rbcw);
    iVar19 = *(int *)((int)constraint + 0xb8);
  }
  else {
    MeQuaternionFromTM(qbcb,(void *)((int)constraint + 0xfc));
    iVar19 = *(int *)((int)constraint + 0x58);
    qbcw[0] = ((qbcb[0] * *(float *)(iVar19 + 0xb0) - qbcb[1] * *(float *)(iVar19 + 0xb4)) -
              qbcb[2] * *(float *)(iVar19 + 0xb8)) - qbcb[3] * *(float *)(iVar19 + 0xbc);
    qbcw[1] = (*(float *)(iVar19 + 0xb8) * qbcb[3] +
              qbcb[1] * *(float *)(iVar19 + 0xb0) + qbcb[0] * *(float *)(iVar19 + 0xb4)) -
              *(float *)(iVar19 + 0xbc) * qbcb[2];
    qbcw[2] = (*(float *)(iVar19 + 0xbc) * qbcb[1] +
              qbcb[2] * *(float *)(iVar19 + 0xb0) + qbcb[0] * *(float *)(iVar19 + 0xb8)) -
              *(float *)(iVar19 + 0xb4) * qbcb[3];
                    /* Unresolved local var: MeReal q0sq@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal q0t2@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q1t2@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q0q1@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal q0q3@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q1q2@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q1q3@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q2q3@[DW_OP_reg12(ST1)] */
    fVar11 = qbcw[2] * qbcw[2];
    qbcw[3] = (qbcb[2] * *(float *)(iVar19 + 0xb4) +
              qbcb[0] * *(float *)(iVar19 + 0xbc) + qbcb[3] * *(float *)(iVar19 + 0xb0)) -
              qbcb[1] * *(float *)(iVar19 + 0xb8);
    fVar9 = qbcw[0] + qbcw[0];
    fVar1 = qbcw[3] * qbcw[3];
    fVar8 = (qbcw[1] + qbcw[1]) * qbcw[2];
    Rbcw[0][0] = ((qbcw[1] * qbcw[1] + qbcw[0] * qbcw[0]) - fVar11) - fVar1;
    fVar2 = (qbcw[1] + qbcw[1]) * qbcw[3];
    fVar3 = (qbcw[2] + qbcw[2]) * qbcw[3];
    Rbcw[1][0] = fVar8 - fVar9 * qbcw[3];
    fVar10 = qbcw[0] * qbcw[0] - qbcw[1] * qbcw[1];
    Rbcw[0][1] = fVar8 + fVar9 * qbcw[3];
    Rbcw[2][0] = fVar9 * qbcw[2] + fVar2;
    Rbcw[0][2] = fVar2 - fVar9 * qbcw[2];
    Rbcw[1][1] = (fVar11 + fVar10) - fVar1;
    Rbcw[2][2] = (fVar10 - fVar11) + fVar1;
    Rbcw[2][1] = fVar3 - fVar9 * qbcw[1];
    Rbcw[1][2] = fVar9 * qbcw[1] + fVar3;
    iVar19 = *(int *)((int)constraint + 0xb8);
    fVar1 = *(float *)((int)constraint + 300);
    fVar2 = *(float *)((int)constraint + 0x130);
    pMVar23 = tlist + iVar19;
    fVar3 = *(float *)((int)constraint + 0x134);
    Rbcw[3][0] = fVar3 * pMVar23->R2[0] + fVar2 * pMVar23->R1[0] + fVar1 * pMVar23->R0[0] +
                 pMVar23->pos[0];
    Rbcw[3][1] = fVar3 * pMVar23->R2[1] + fVar2 * pMVar23->R1[1] + fVar1 * pMVar23->R0[1] +
                 pMVar23->pos[1];
    Rbcw[3][2] = fVar3 * pMVar23->R2[2] + fVar1 * pMVar23->R0[2] + fVar2 * pMVar23->R1[2] +
                 pMVar23->pos[2];
  }
  uVar24 = 0;
  do {
    fVar1 = qacw[uVar24];
    iVar18 = uVar24 + 1;
    qmul[uVar24][0] = fVar1 * qbcw[0];
    uVar15 = uVar24 + 2;
    qmul[uVar24][1] = fVar1 * qbcw[1];
    qmul[uVar24][3] = fVar1 * qbcw[3];
    qmul[uVar24][2] = fVar1 * qbcw[2];
    fVar1 = qacw[iVar18];
    qmul[iVar18][0] = qbcw[0] * fVar1;
    qmul[iVar18][1] = qbcw[1] * fVar1;
    qmul[iVar18][2] = qbcw[2] * fVar1;
    qmul[iVar18][3] = fVar1 * qbcw[3];
    uVar24 = uVar15;
  } while (uVar15 < 4);
  fVar1 = ((qmul[0][1] - qmul[1][0]) + qmul[3][2]) - qmul[2][3];
  fVar2 = ((qmul[2][1] - qmul[3][0]) - qmul[1][2]) + qmul[0][3];
                    /* Unresolved local var: MeReal q0sq@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q2sq@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal q0t2@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q1t2@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q0q1@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal q0q3@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q1q2@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q1q3@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q2q3@[DW_OP_reg13(ST2)] */
  fVar8 = qmul[1][1] + qmul[0][0] + qmul[2][2] + qmul[3][3];
  fVar10 = (-qmul[2][0] - qmul[3][1]) + qmul[0][2] + qmul[1][3];
  fVar9 = ((fVar1 * fVar1 + fVar8 * fVar8) - fVar10 * fVar10) - fVar2 * fVar2;
  fVar3 = (fVar1 + fVar1) * fVar10 - (fVar8 + fVar8) * fVar2;
  fVar2 = (fVar8 + fVar8) * fVar10 + (fVar1 + fVar1) * fVar2;
  MdtBclAddSphericalRowsWorld
            (clist,tlist[*(int *)((int)constraint + 0xb4)].pos,tlist[iVar19].pos,Racw[3],Rbcw[3],
             (uint)(iVar19 != -1),(MeReal *)((int)constraint + 0x13c),
             (MeReal *)((int)constraint + 0x148));
  if (*(int *)((int)constraint + 0x168) != 0) {
                    /* Unresolved local var: uint i@[DW_OP_reg7(EDI)] */
    JwQ1[0] = (((qmul[0][0] + qmul[1][1]) - qmul[3][3]) - qmul[2][2]) * 0.5;
    JwQ1[1] = (qmul[0][3] + qmul[1][2] + qmul[3][0] + qmul[2][1]) * 0.5;
    JwQ1[2] = (((qmul[1][3] - qmul[0][2]) + qmul[3][1]) - qmul[2][0]) * 0.5;
    if (*(int *)((int)constraint + 0x168) == 2) {
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
      iVar19 = 0;
      do {
        iVar17 = clist->num_rows_inc_padding;
        iVar18 = iVar17 + 3;
        if (iVar18 < 0) {
          iVar18 = iVar17 + 6;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar17 * 4 + 0xc + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10) = 0;
        iVar17 = clist->num_rows_inc_padding;
        iVar18 = iVar17 + 3;
        if (iVar18 < 0) {
          iVar18 = iVar17 + 6;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar17 * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 0x6c) = 0;
        iVar17 = clist->num_rows_inc_padding;
        iVar18 = iVar17 + 3;
        if (iVar18 < 0) {
          iVar18 = iVar17 + 6;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar17 * 4 + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10 + 0x1c) = 0;
        iVar17 = clist->num_rows_inc_padding;
        iVar18 = iVar17 + 3;
        if (iVar18 < 0) {
          iVar18 = iVar17 + 6;
        }
        iVar22 = iVar19 + 2;
        *(undefined4 *)
         ((int)*clist->Jstore + iVar17 * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 0x7c) = 0;
        iVar19 = iVar22;
      } while (iVar22 != 6);
      pMVar20[3] = 0.0;
      pMVar14[3] = 0.0;
      pMVar16[3] = 0.0;
      pMVar21[3] = 0.0;
      iVar18 = clist->num_rows_inc_padding;
      iVar19 = iVar18 + 3;
      if (iVar19 < 0) {
        iVar19 = iVar18 + 6;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar18 * 4 + 0xc + (iVar19 >> 2) * 0xb0) = 0;
      iVar18 = clist->num_rows_inc_padding;
      iVar19 = iVar18 + 3;
      if (iVar19 < 0) {
        iVar19 = iVar18 + 6;
      }
      *(MeReal *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0x3c) = JwQ1[0];
      uVar24 = 1;
      do {
        iVar18 = clist->num_rows_inc_padding;
        iVar19 = iVar18 + 3;
        if (iVar19 < 0) {
          iVar19 = iVar18 + 6;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar18 * 4 + 0xc + (iVar19 >> 2) * 0xb0 + uVar24 * 0x10) = 0;
        iVar18 = clist->num_rows_inc_padding;
        iVar19 = iVar18 + 3;
        if (iVar19 < 0) {
          iVar19 = iVar18 + 6;
        }
        *(MeReal *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + uVar24 * 0x10 + 0x3c)
             = JwQ1[uVar24];
        iVar18 = clist->num_rows_inc_padding;
        iVar19 = iVar18 + 3;
        if (iVar19 < 0) {
          iVar19 = iVar18 + 6;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + uVar24 * 0x10 + 0x1c) = 0;
        iVar18 = clist->num_rows_inc_padding;
        iVar19 = iVar18 + 3;
        if (iVar19 < 0) {
          iVar19 = iVar18 + 6;
        }
        uVar15 = uVar24 + 2;
        *(MeReal *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + uVar24 * 0x10 + 0x4c)
             = JwQ1[uVar24 + 1];
        uVar24 = uVar15;
      } while (uVar15 < 3);
      if (*(int *)((int)constraint + 0xb8) != -1) {
        iVar18 = clist->num_rows_inc_padding;
        iVar19 = iVar18 + 3;
        if (iVar19 < 0) {
          iVar19 = iVar18 + 6;
        }
        *(undefined4 *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0x6c) = 0;
        iVar18 = clist->num_rows_inc_padding;
        iVar19 = iVar18 + 3;
        if (iVar19 < 0) {
          iVar19 = iVar18 + 6;
        }
        *(float *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0x9c) = -JwQ1[0];
        uVar24 = 1;
        do {
          iVar18 = clist->num_rows_inc_padding;
          iVar19 = iVar18 + 3;
          if (iVar19 < 0) {
            iVar19 = iVar18 + 6;
          }
          *(undefined4 *)
           ((int)*clist->Jstore + iVar18 * 4 + uVar24 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x6c) = 0;
          iVar18 = clist->num_rows_inc_padding;
          iVar19 = iVar18 + 3;
          if (iVar19 < 0) {
            iVar19 = iVar18 + 6;
          }
          *(float *)((int)*clist->Jstore + uVar24 * 0x10 + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0x9c)
               = -JwQ1[uVar24];
          iVar18 = clist->num_rows_inc_padding;
          iVar19 = iVar18 + 3;
          if (iVar19 < 0) {
            iVar19 = iVar18 + 6;
          }
          *(undefined4 *)
           ((int)*clist->Jstore + iVar18 * 4 + uVar24 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x7c) = 0;
          iVar18 = clist->num_rows_inc_padding;
          iVar19 = iVar18 + 3;
          if (iVar19 < 0) {
            iVar19 = iVar18 + 6;
          }
          uVar15 = uVar24 + 2;
          *(float *)((int)*clist->Jstore + uVar24 * 0x10 + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0xac)
               = -JwQ1[uVar24 + 1];
          uVar24 = uVar15;
        } while (uVar15 < 3);
      }
      pMVar5[iVar4 + 3] = -3.4028235e+38;
      pMVar6[iVar4 + 3] = 3.4028235e+38;
      pMVar20[3] = -fVar1;
    }
    else {
                    /* Unresolved local var: MeReal CSquared@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal SSquared@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal PositionError@[DW_OP_reg14(ST3)] */
      fVar10 = *(float *)((int)constraint + 0x16c) * *(float *)((int)constraint + 0x16c);
      fVar11 = 1.0 - fVar10;
      fVar13 = -fVar11 * fVar8 * fVar8 + fVar10 * fVar1 * fVar1;
      if (fVar13 <= 0.0) goto LAB_0001875e;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
      iVar19 = 0;
      do {
        iVar17 = clist->num_rows_inc_padding;
        iVar18 = iVar17 + 3;
        if (iVar18 < 0) {
          iVar18 = iVar17 + 6;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar17 * 4 + 0xc + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10) = 0;
        iVar17 = clist->num_rows_inc_padding;
        iVar18 = iVar17 + 3;
        if (iVar18 < 0) {
          iVar18 = iVar17 + 6;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar17 * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 0x6c) = 0;
        iVar17 = clist->num_rows_inc_padding;
        iVar18 = iVar17 + 3;
        if (iVar18 < 0) {
          iVar18 = iVar17 + 6;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar17 * 4 + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10 + 0x1c) = 0;
        iVar17 = clist->num_rows_inc_padding;
        iVar18 = iVar17 + 3;
        if (iVar18 < 0) {
          iVar18 = iVar17 + 6;
        }
        iVar22 = iVar19 + 2;
        *(undefined4 *)
         ((int)*clist->Jstore + iVar17 * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 0x7c) = 0;
        iVar19 = iVar22;
      } while (iVar22 != 6);
      pMVar20[3] = 0.0;
      pMVar14[3] = 0.0;
      pMVar16[3] = 0.0;
      pMVar21[3] = 0.0;
      uVar24 = 0;
      JwQ0[0] = (((qmul[1][0] - qmul[0][1]) - qmul[2][3]) + qmul[3][2]) * 0.5;
      JwQ0[1] = (((qmul[1][3] - qmul[0][2]) + qmul[2][0]) - qmul[3][1]) * 0.5;
      JwQ0[2] = ((-qmul[0][3] - qmul[1][2]) + qmul[2][1] + qmul[3][0]) * 0.5;
      do {
        iVar18 = clist->num_rows_inc_padding;
        iVar19 = iVar18 + 3;
        if (iVar19 < 0) {
          iVar19 = iVar18 + 6;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar18 * 4 + 0xc + (iVar19 >> 2) * 0xb0 + uVar24 * 0x10) = 0;
        iVar18 = clist->num_rows_inc_padding;
        iVar19 = iVar18 + 3;
        if (iVar19 < 0) {
          iVar19 = iVar18 + 6;
        }
        iVar17 = uVar24 * 0x10;
        fVar12 = (fVar11 + fVar11) * fVar8 * JwQ0[uVar24] - (fVar10 + fVar10) * fVar1 * JwQ1[uVar24]
        ;
        paMVar7 = clist->Jstore;
        Jw[uVar24] = fVar12;
        uVar24 = uVar24 + 1;
        *(float *)((int)*paMVar7 + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + iVar17 + 0x3c) = fVar12;
      } while (uVar24 < 3);
      if (*(int *)((int)constraint + 0xb8) != -1) {
        iVar18 = clist->num_rows_inc_padding;
        iVar19 = iVar18 + 3;
        if (iVar19 < 0) {
          iVar19 = iVar18 + 6;
        }
        *(undefined4 *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0x6c) = 0;
        iVar18 = clist->num_rows_inc_padding;
        iVar19 = iVar18 + 3;
        if (iVar19 < 0) {
          iVar19 = iVar18 + 6;
        }
        *(float *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0x9c) = -Jw[0];
        uVar24 = 1;
        do {
          iVar18 = clist->num_rows_inc_padding;
          iVar19 = iVar18 + 3;
          if (iVar19 < 0) {
            iVar19 = iVar18 + 6;
          }
          *(undefined4 *)
           ((int)*clist->Jstore + iVar18 * 4 + uVar24 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x6c) = 0;
          iVar18 = clist->num_rows_inc_padding;
          iVar19 = iVar18 + 3;
          if (iVar19 < 0) {
            iVar19 = iVar18 + 6;
          }
          *(float *)((int)*clist->Jstore + uVar24 * 0x10 + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0x9c)
               = -Jw[uVar24];
          iVar18 = clist->num_rows_inc_padding;
          iVar19 = iVar18 + 3;
          if (iVar19 < 0) {
            iVar19 = iVar18 + 6;
          }
          *(undefined4 *)
           ((int)*clist->Jstore + iVar18 * 4 + uVar24 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x7c) = 0;
          iVar18 = clist->num_rows_inc_padding;
          iVar19 = iVar18 + 3;
          if (iVar19 < 0) {
            iVar19 = iVar18 + 6;
          }
          uVar15 = uVar24 + 2;
          *(float *)((int)*clist->Jstore + uVar24 * 0x10 + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0xac)
               = -Jw[uVar24 + 1];
          uVar24 = uVar15;
        } while (uVar15 < 3);
      }
      pMVar5[iVar4 + 3] = -3.4028235e+38;
      pMVar6[iVar4 + 3] = 0.0;
      pMVar20[3] = fVar13;
    }
    nRows = 4;
  }
LAB_0001875e:
                    /* Unresolved local var: uint i@[DW_OP_reg7(EDI)] */
  if ((1 < *(uint *)((int)constraint + 0x164)) && (*(uint *)((int)constraint + 0x164) == 2)) {
                    /* Unresolved local var: MeReal sinhalfswing@[???]
                       Unresolved local var: MeReal coshalfswing@[???]
                       Unresolved local var: MeReal recip_one_plus_coshalfswing@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal sinswing@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal recip_elliptic@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal sqrt_recip_elliptic@[???]
                       Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
    fVar1 = (*(float *)((int)constraint + 0x170) + 1.0) * fVar3 *
            (1.0 / (1.0 - *(float *)((int)constraint + 0x170)));
    fVar11 = (1.0 / (1.0 - *(float *)((int)constraint + 0x174))) *
             (*(float *)((int)constraint + 0x174) + 1.0) * fVar2;
    fVar10 = SQRT((1.0 - fVar9) * 0.5);
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
    fVar8 = SQRT((fVar9 + 1.0) * 0.5);
    fVar13 = 1.0 / (fVar8 + 1.0);
    fVar2 = 1.0 / (fVar2 * fVar11 + fVar3 * fVar1);
    fVar8 = (fVar10 + fVar10) * fVar8;
                    /* Unresolved local var: float __result@[DW_OP_reg14(ST3)] */
    fVar12 = SQRT(fVar2);
    fVar3 = fVar10 * fVar13 - fVar8 * fVar12;
    if (0.0 < fVar3) {
                    /* Unresolved local var: MeReal ssellip3_2@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal hepsilon@[DW_OP_reg12(ST1)] */
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
      fVar2 = fVar8 * fVar2 * fVar12;
      fVar8 = (fVar9 * fVar12 - fVar13 * 0.5) * (1.0 / fVar8);
      fVar1 = fVar1 * fVar2;
      fVar2 = fVar2 * fVar11;
      Jw[0] = fVar2 * Rbcw[2][0] + fVar1 * Rbcw[1][0] + Rbcw[0][0] * fVar8;
      Jw[2] = Rbcw[0][2] * fVar8 + fVar1 * Rbcw[1][2] + fVar2 * Rbcw[2][2];
      Jw[1] = Rbcw[0][1] * fVar8 + fVar1 * Rbcw[1][1] + fVar2 * Rbcw[2][1];
      JwQ0[0] = Jw[1] * Racw[0][2] - Jw[2] * Racw[0][1];
      JwQ0[1] = Jw[2] * Racw[0][0] - Racw[0][2] * Jw[0];
      JwQ0[2] = Jw[0] * Racw[0][1] - Racw[0][0] * Jw[1];
      iVar19 = 0;
      do {
        iVar17 = nRows + clist->num_rows_inc_padding;
        iVar18 = iVar17;
        if (iVar17 < 0) {
          iVar18 = iVar17 + 3;
        }
        *(undefined4 *)((int)*clist->Jstore + iVar17 * 4 + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10) = 0
        ;
        iVar17 = nRows + clist->num_rows_inc_padding;
        iVar18 = iVar17;
        if (iVar17 < 0) {
          iVar18 = iVar17 + 3;
        }
        *(undefined4 *)
         ((int)*clist->Jstore + iVar17 * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 0x60) = 0;
        iVar18 = nRows + clist->num_rows_inc_padding;
        if (iVar18 < 0) {
          iVar18 = iVar18 + 3;
        }
        *(undefined4 *)
         ((int)*clist->Jstore +
         nRows * 4 + clist->num_rows_inc_padding * 4 + (iVar18 >> 2) * 0xb0 + iVar19 * 0x10 + 0x10)
             = 0;
        iVar18 = nRows + clist->num_rows_inc_padding;
        if (iVar18 < 0) {
          iVar18 = iVar18 + 3;
        }
        iVar17 = iVar19 + 2;
        *(undefined4 *)
         ((int)*clist->Jstore +
         nRows * 4 + clist->num_rows_inc_padding * 4 + iVar19 * 0x10 + (iVar18 >> 2) * 0xb0 + 0x70)
             = 0;
        iVar19 = iVar17;
      } while (iVar17 != 6);
      pMVar20[nRows] = 0.0;
      pMVar14[nRows] = 0.0;
      pMVar16[nRows] = 0.0;
      pMVar21[nRows] = 0.0;
      iVar18 = nRows + clist->num_rows_inc_padding;
      iVar19 = iVar18;
      if (iVar18 < 0) {
        iVar19 = iVar18 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0) = 0;
      iVar18 = nRows + clist->num_rows_inc_padding;
      iVar19 = iVar18;
      if (iVar18 < 0) {
        iVar19 = iVar18 + 3;
      }
      *(MeReal *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0x30) = JwQ0[0];
      uVar24 = 1;
      do {
        iVar18 = nRows + clist->num_rows_inc_padding;
        iVar19 = iVar18;
        if (iVar18 < 0) {
          iVar19 = iVar18 + 3;
        }
        *(undefined4 *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + uVar24 * 0x10) = 0
        ;
        iVar18 = nRows + clist->num_rows_inc_padding;
        iVar19 = iVar18;
        if (iVar18 < 0) {
          iVar19 = iVar18 + 3;
        }
        *(MeReal *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + uVar24 * 0x10 + 0x30)
             = JwQ0[uVar24];
        iVar19 = nRows + clist->num_rows_inc_padding;
        if (iVar19 < 0) {
          iVar19 = iVar19 + 3;
        }
        *(undefined4 *)
         ((int)*clist->Jstore +
         nRows * 4 + clist->num_rows_inc_padding * 4 + (iVar19 >> 2) * 0xb0 + uVar24 * 0x10 + 0x10)
             = 0;
        iVar19 = nRows + clist->num_rows_inc_padding;
        if (iVar19 < 0) {
          iVar19 = iVar19 + 3;
        }
        uVar15 = uVar24 + 2;
        *(MeReal *)
         ((int)*clist->Jstore +
         nRows * 4 + clist->num_rows_inc_padding * 4 + (iVar19 >> 2) * 0xb0 + uVar24 * 0x10 + 0x40)
             = JwQ0[uVar24 + 1];
        uVar24 = uVar15;
      } while (uVar15 < 3);
      if (*(int *)((int)constraint + 0xb8) != -1) {
        iVar18 = nRows + clist->num_rows_inc_padding;
        iVar19 = iVar18;
        if (iVar18 < 0) {
          iVar19 = iVar18 + 3;
        }
        *(undefined4 *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0x60) = 0;
        iVar18 = nRows + clist->num_rows_inc_padding;
        iVar19 = iVar18;
        if (iVar18 < 0) {
          iVar19 = iVar18 + 3;
        }
        *(float *)((int)*clist->Jstore + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0x90) = -JwQ0[0];
        uVar24 = 1;
        do {
          iVar18 = nRows + clist->num_rows_inc_padding;
          iVar19 = iVar18;
          if (iVar18 < 0) {
            iVar19 = iVar18 + 3;
          }
          *(undefined4 *)
           ((int)*clist->Jstore + iVar18 * 4 + uVar24 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x60) = 0;
          iVar18 = nRows + clist->num_rows_inc_padding;
          iVar19 = iVar18;
          if (iVar18 < 0) {
            iVar19 = iVar18 + 3;
          }
          *(float *)((int)*clist->Jstore + uVar24 * 0x10 + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0x90)
               = -JwQ0[uVar24];
          iVar19 = nRows + clist->num_rows_inc_padding;
          if (iVar19 < 0) {
            iVar19 = iVar19 + 3;
          }
          *(undefined4 *)
           ((int)*clist->Jstore +
           nRows * 4 + clist->num_rows_inc_padding * 4 + uVar24 * 0x10 + (iVar19 >> 2) * 0xb0 + 0x70
           ) = 0;
          iVar18 = nRows + clist->num_rows_inc_padding;
          iVar19 = iVar18;
          if (iVar18 < 0) {
            iVar19 = iVar18 + 3;
          }
          uVar15 = uVar24 + 2;
          *(float *)((int)*clist->Jstore + uVar24 * 0x10 + iVar18 * 4 + (iVar19 >> 2) * 0xb0 + 0xa0)
               = -JwQ0[uVar24 + 1];
          uVar24 = uVar15;
        } while (uVar15 < 3);
      }
      PositionError = -fVar3;
      pMVar20[nRows] = PositionError;
      pMVar5[iVar4 + nRows] = 0.0;
      pMVar6[iVar4 + nRows] = 3.4028235e+38;
      fVar1 = 1.0 / (*(float *)((int)constraint + 0x180) * *(float *)params +
                    *(float *)((int)constraint + 0x184));
      pMVar14[nRows] = fVar1;
      if (fVar1 < 0.0) {
        pMVar14[nRows] = 0.0;
      }
      pMVar21[nRows] = fVar1 * *(float *)params * *(float *)((int)constraint + 0x180);
      nRows = nRows + 1;
    }
  }
  MdtBclEndConstraint(clist,nRows);
  return;
}


/* ==== MdtBclAddLinear1 ==== */

/* WARNING: Unknown calling convention */

void MdtBclAddLinear1(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                     MdtKeaBody **blist,void *params)

{
  float fVar1;
  float fVar2;
  int iVar3;
  MeReal *pMVar4;
  MeReal *pMVar5;
  MeReal *pMVar6;
  MeReal *pMVar7;
  int iVar8;
  int iVar9;
  MeReal *pMVar10;
  int iVar11;
  MeReal *pMVar12;
  int iVar13;
  uint uVar14;
  uint uVar15;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal CrossProduct [3];
  MeReal displacement [3];
  MeReal normal [4];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MdtLinear1 * joint@[???]
                       Unresolved local var: uint i@[DW_OP_reg7(EDI)] */
  iVar3 = clist->num_rows_exc_padding;
  pMVar12 = clist->c;
  pMVar4 = clist->xi;
  pMVar5 = clist->lo;
  pMVar6 = clist->hi;
  pMVar10 = clist->slipfactor;
  pMVar7 = clist->xgamma;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar11 = 0;
  do {
    iVar8 = clist->num_rows_inc_padding;
    iVar9 = iVar8;
    if (iVar8 < 0) {
      iVar9 = iVar8 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + (iVar9 >> 2) * 0xb0 + iVar11 * 0x10) = 0;
    iVar8 = clist->num_rows_inc_padding;
    iVar9 = iVar8;
    if (iVar8 < 0) {
      iVar9 = iVar8 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + iVar11 * 0x10 + (iVar9 >> 2) * 0xb0 + 0x60) =
         0;
    iVar8 = clist->num_rows_inc_padding;
    iVar9 = iVar8;
    if (iVar8 < 0) {
      iVar9 = iVar8 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + (iVar9 >> 2) * 0xb0 + iVar11 * 0x10 + 0x10) =
         0;
    iVar8 = clist->num_rows_inc_padding;
    iVar9 = iVar8;
    if (iVar8 < 0) {
      iVar9 = iVar8 + 3;
    }
    iVar13 = iVar11 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar8 * 4 + iVar11 * 0x10 + (iVar9 >> 2) * 0xb0 + 0x70) =
         0;
    iVar11 = iVar13;
  } while (iVar13 != 6);
  pMVar4[iVar3] = 0.0;
  pMVar10[iVar3] = 0.0;
  pMVar12[iVar3] = 0.0;
  pMVar7[iVar3] = 0.0;
  uVar14 = 0;
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  if (*(int *)((int)constraint + 0xb8) == -1) {
    iVar11 = *(int *)((int)constraint + 0xb4);
    normal[0] = *(MeReal *)((int)constraint + 0x16c);
    displacement[0] = tlist[iVar11].pos[0];
    normal[1] = *(MeReal *)((int)constraint + 0x170);
    displacement[1] = tlist[iVar11].pos[1];
    normal[2] = *(MeReal *)((int)constraint + 0x174);
    displacement[2] = tlist[iVar11].pos[2];
  }
  else {
    MeMatrixMultiply(normal,4,3,1,tlist[*(int *)((int)constraint + 0xb8)].R0,
                     (MeReal *)((int)constraint + 0x16c));
    pMVar12 = tlist[*(int *)((int)constraint + 0xb8)].pos;
    pMVar10 = tlist[*(int *)((int)constraint + 0xb4)].pos;
    do {
      fVar1 = *pMVar12;
      pMVar12 = pMVar12 + 1;
      fVar2 = *pMVar10;
      pMVar10 = pMVar10 + 1;
      displacement[uVar14] = fVar2 - fVar1;
      uVar14 = uVar14 + 1;
    } while (uVar14 < 3);
  }
  iVar9 = clist->num_rows_inc_padding;
  iVar11 = iVar9;
  if (iVar9 < 0) {
    iVar11 = iVar9 + 3;
  }
  *(MeReal *)((int)*clist->Jstore + iVar9 * 4 + (iVar11 >> 2) * 0xb0) = normal[0];
  iVar9 = clist->num_rows_inc_padding;
  iVar11 = iVar9;
  if (iVar9 < 0) {
    iVar11 = iVar9 + 3;
  }
  *(MeReal *)((int)*clist->Jstore + iVar9 * 4 + (iVar11 >> 2) * 0xb0 + 0x10) = normal[1];
  iVar9 = clist->num_rows_inc_padding;
  iVar11 = iVar9;
  if (iVar9 < 0) {
    iVar11 = iVar9 + 3;
  }
  *(MeReal *)((int)*clist->Jstore + iVar9 * 4 + (iVar11 >> 2) * 0xb0 + 0x20) = normal[2];
  if (*(int *)((int)constraint + 0xb8) != -1) {
    CrossProduct[0] = normal[1] * displacement[2] - normal[2] * displacement[1];
    CrossProduct[1] = normal[2] * displacement[0] - displacement[2] * normal[0];
    CrossProduct[2] = normal[0] * displacement[1] - normal[1] * displacement[0];
    iVar9 = clist->num_rows_inc_padding;
    iVar11 = iVar9;
    if (iVar9 < 0) {
      iVar11 = iVar9 + 3;
    }
    *(float *)((int)*clist->Jstore + iVar9 * 4 + (iVar11 >> 2) * 0xb0 + 0x60) = -normal[0];
    iVar9 = clist->num_rows_inc_padding;
    iVar11 = iVar9;
    if (iVar9 < 0) {
      iVar11 = iVar9 + 3;
    }
    *(MeReal *)((int)*clist->Jstore + iVar9 * 4 + (iVar11 >> 2) * 0xb0 + 0x90) = CrossProduct[0];
    uVar14 = 1;
    do {
      iVar9 = clist->num_rows_inc_padding;
      iVar11 = iVar9;
      if (iVar9 < 0) {
        iVar11 = iVar9 + 3;
      }
      *(float *)((int)*clist->Jstore + iVar9 * 4 + uVar14 * 0x10 + (iVar11 >> 2) * 0xb0 + 0x60) =
           -normal[uVar14];
      iVar9 = clist->num_rows_inc_padding;
      iVar11 = iVar9;
      if (iVar9 < 0) {
        iVar11 = iVar9 + 3;
      }
      *(MeReal *)((int)*clist->Jstore + iVar9 * 4 + uVar14 * 0x10 + (iVar11 >> 2) * 0xb0 + 0x90) =
           CrossProduct[uVar14];
      iVar9 = clist->num_rows_inc_padding;
      iVar11 = iVar9;
      if (iVar9 < 0) {
        iVar11 = iVar9 + 3;
      }
      *(float *)((int)*clist->Jstore + iVar9 * 4 + uVar14 * 0x10 + (iVar11 >> 2) * 0xb0 + 0x70) =
           -normal[uVar14 + 1];
      iVar9 = clist->num_rows_inc_padding;
      iVar11 = iVar9;
      if (iVar9 < 0) {
        iVar11 = iVar9 + 3;
      }
      uVar15 = uVar14 + 2;
      *(MeReal *)((int)*clist->Jstore + iVar9 * 4 + uVar14 * 0x10 + (iVar11 >> 2) * 0xb0 + 0xa0) =
           CrossProduct[uVar14 + 1];
      uVar14 = uVar15;
    } while (uVar15 < 3);
  }
  pMVar5[iVar3] = -3.4028235e+38;
  pMVar6[iVar3] = 3.4028235e+38;
  pMVar4[iVar3] =
       (displacement[2] * normal[2] + displacement[1] * normal[1] + displacement[0] * normal[0]) -
       *(float *)((int)constraint + 0x178);
  MdtBclEndConstraint(clist,1);
  return;
}


/* ==== MdtBclAddLinear2 ==== */

/* WARNING: Unknown calling convention */

void MdtBclAddLinear2(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                     MdtKeaBody **blist,void *params)

{
  int iVar1;
  MeReal *pMVar2;
  MeReal *pMVar3;
  MeReal *pMVar4;
  MeReal *pMVar5;
  MeReal *pMVar6;
  int iVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float *pfVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  uint uVar18;
  int iVar19;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal Perpendicular2 [3];
  MeReal Perpendicular1 [3];
  MeReal displacement [3];
  MeReal abs_pos [3];
  MeReal initial_pos [4];
  MeReal rel_pos [4];
  MeReal vector2 [4];
  MeReal vector1 [4];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MdtLinear2 * joint@[???]
                       Unresolved local var: uint i@[DW_OP_reg6(ESI)] */
  iVar1 = clist->num_rows_exc_padding;
  pMVar2 = clist->c;
  pfVar14 = clist->xi + iVar1;
  pMVar3 = clist->lo;
  pMVar4 = clist->hi;
  pMVar5 = clist->slipfactor;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  pMVar6 = clist->xgamma;
  iVar16 = 0;
  do {
    iVar7 = clist->num_rows_inc_padding;
    iVar15 = iVar7;
    if (iVar7 < 0) {
      iVar15 = iVar7 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar15 >> 2) * 0xb0 + iVar16 * 0x10) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar15 = iVar7;
    if (iVar7 < 0) {
      iVar15 = iVar7 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + iVar16 * 0x10 + (iVar15 >> 2) * 0xb0 + 0x60) =
         0;
    iVar7 = clist->num_rows_inc_padding;
    iVar15 = iVar7;
    if (iVar7 < 0) {
      iVar15 = iVar7 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar15 >> 2) * 0xb0 + iVar16 * 0x10 + 0x10) =
         0;
    iVar7 = clist->num_rows_inc_padding;
    iVar15 = iVar7;
    if (iVar7 < 0) {
      iVar15 = iVar7 + 3;
    }
    iVar19 = iVar16 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + iVar16 * 0x10 + (iVar15 >> 2) * 0xb0 + 0x70) =
         0;
    iVar16 = iVar19;
  } while (iVar19 != 6);
  *pfVar14 = 0.0;
  pMVar5[iVar1] = 0.0;
  pMVar2[iVar1] = 0.0;
  pMVar6[iVar1] = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar16 = 0;
  do {
    iVar7 = clist->num_rows_inc_padding;
    iVar15 = iVar7 + 1;
    if (iVar15 < 0) {
      iVar15 = iVar7 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + 4 + (iVar15 >> 2) * 0xb0 + iVar16 * 0x10) = 0;
    iVar7 = clist->num_rows_inc_padding;
    iVar15 = iVar7 + 1;
    if (iVar15 < 0) {
      iVar15 = iVar7 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + iVar16 * 0x10 + (iVar15 >> 2) * 0xb0 + 100) =
         0;
    iVar7 = clist->num_rows_inc_padding;
    iVar15 = iVar7 + 1;
    if (iVar15 < 0) {
      iVar15 = iVar7 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + (iVar15 >> 2) * 0xb0 + iVar16 * 0x10 + 0x14) =
         0;
    iVar7 = clist->num_rows_inc_padding;
    iVar15 = iVar7 + 1;
    if (iVar15 < 0) {
      iVar15 = iVar7 + 4;
    }
    iVar19 = iVar16 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar7 * 4 + iVar16 * 0x10 + (iVar15 >> 2) * 0xb0 + 0x74) =
         0;
    iVar16 = iVar19;
  } while (iVar19 != 6);
  pfVar14[1] = 0.0;
  (pMVar5 + iVar1)[1] = 0.0;
  (pMVar2 + iVar1)[1] = 0.0;
  (pMVar6 + iVar1)[1] = 0.0;
  uVar17 = 0;
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  if (*(int *)((int)constraint + 0xb8) == -1) {
    do {
      vector1[uVar17] = *(MeReal *)((int)constraint + uVar17 * 4 + 0x184);
      vector2[uVar17] = *(MeReal *)((int)constraint + uVar17 * 4 + 400);
      initial_pos[uVar17] = *(MeReal *)((int)constraint + uVar17 * 4 + 0x16c);
      uVar17 = uVar17 + 1;
    } while (uVar17 < 3);
  }
  else {
    MeMatrixMultiply(vector1,4,3,1,tlist[*(int *)((int)constraint + 0xb8)].R0,
                     (MeReal *)((int)constraint + 0x184));
    MeMatrixMultiply(vector2,4,3,1,tlist[*(int *)((int)constraint + 0xb8)].R0,
                     (MeReal *)((int)constraint + 400));
    MeMatrixMultiply(initial_pos,4,3,1,tlist[*(int *)((int)constraint + 0xb8)].R0,
                     (MeReal *)((int)constraint + 0x16c));
    iVar16 = *(int *)((int)constraint + 0xb8);
    initial_pos[0] = tlist[iVar16].pos[0] + initial_pos[0];
    initial_pos[1] = tlist[iVar16].pos[1] + initial_pos[1];
    initial_pos[2] = tlist[iVar16].pos[2] + initial_pos[2];
  }
  MeMatrixMultiply(rel_pos,4,3,1,tlist[*(int *)((int)constraint + 0xb4)].R0,
                   (MeReal *)((int)constraint + 0x160));
  iVar16 = *(int *)((int)constraint + 0xb4);
  fVar12 = rel_pos[0] + tlist[iVar16].pos[0];
  fVar13 = fVar12 - initial_pos[0];
  uVar17 = 0;
  fVar8 = rel_pos[1] + tlist[iVar16].pos[1];
  fVar9 = fVar8 - initial_pos[1];
  fVar10 = rel_pos[2] + tlist[iVar16].pos[2];
  fVar11 = fVar10 - initial_pos[2];
  Perpendicular1[0] = rel_pos[1] * vector1[2] - rel_pos[2] * vector1[1];
  Perpendicular1[1] = rel_pos[2] * vector1[0] - vector1[2] * rel_pos[0];
  Perpendicular1[2] = vector1[1] * rel_pos[0] - vector1[0] * rel_pos[1];
  Perpendicular2[0] = rel_pos[1] * vector2[2] - rel_pos[2] * vector2[1];
  Perpendicular2[1] = rel_pos[2] * vector2[0] - vector2[2] * rel_pos[0];
  Perpendicular2[2] = rel_pos[0] * vector2[1] - rel_pos[1] * vector2[0];
  do {
    iVar15 = clist->num_rows_inc_padding;
    iVar16 = iVar15;
    if (iVar15 < 0) {
      iVar16 = iVar15 + 3;
    }
    *(MeReal *)((int)*clist->Jstore + iVar15 * 4 + (iVar16 >> 2) * 0xb0 + uVar17 * 0x10) =
         vector1[uVar17];
    iVar15 = clist->num_rows_inc_padding;
    iVar16 = iVar15 + 1;
    if (iVar16 < 0) {
      iVar16 = iVar15 + 4;
    }
    *(MeReal *)((int)*clist->Jstore + iVar15 * 4 + 4 + (iVar16 >> 2) * 0xb0 + uVar17 * 0x10) =
         vector2[uVar17];
    iVar15 = clist->num_rows_inc_padding;
    iVar16 = iVar15;
    if (iVar15 < 0) {
      iVar16 = iVar15 + 3;
    }
    *(MeReal *)((int)*clist->Jstore + iVar15 * 4 + (iVar16 >> 2) * 0xb0 + uVar17 * 0x10 + 0x30) =
         Perpendicular1[uVar17];
    iVar15 = clist->num_rows_inc_padding;
    iVar16 = iVar15 + 1;
    if (iVar16 < 0) {
      iVar16 = iVar15 + 4;
    }
    iVar7 = uVar17 * 0x10;
    pMVar2 = Perpendicular2 + uVar17;
    uVar17 = uVar17 + 1;
    *(MeReal *)((int)*clist->Jstore + iVar15 * 4 + (iVar16 >> 2) * 0xb0 + iVar7 + 0x34) = *pMVar2;
  } while (uVar17 < 3);
  iVar16 = *(int *)((int)constraint + 0xb8);
  if (iVar16 != -1) {
    fVar12 = fVar12 - tlist[iVar16].pos[0];
    fVar8 = fVar8 - tlist[iVar16].pos[1];
    fVar10 = fVar10 - tlist[iVar16].pos[2];
    Perpendicular1[0] = vector1[1] * fVar10 - vector1[2] * fVar8;
    Perpendicular1[2] = vector1[0] * fVar8 - vector1[1] * fVar12;
    Perpendicular1[1] = vector1[2] * fVar12 - vector1[0] * fVar10;
    Perpendicular2[0] = vector2[1] * fVar10 - vector2[2] * fVar8;
    Perpendicular2[1] = vector2[2] * fVar12 - fVar10 * vector2[0];
    Perpendicular2[2] = vector2[0] * fVar8 - vector2[1] * fVar12;
    uVar17 = 0;
    do {
      iVar15 = clist->num_rows_inc_padding;
      iVar16 = iVar15;
      if (iVar15 < 0) {
        iVar16 = iVar15 + 3;
      }
      *(float *)((int)*clist->Jstore + iVar15 * 4 + uVar17 * 0x10 + (iVar16 >> 2) * 0xb0 + 0x60) =
           -vector1[uVar17];
      iVar15 = clist->num_rows_inc_padding;
      iVar16 = iVar15 + 1;
      if (iVar16 < 0) {
        iVar16 = iVar15 + 4;
      }
      *(float *)((int)*clist->Jstore + iVar15 * 4 + uVar17 * 0x10 + (iVar16 >> 2) * 0xb0 + 100) =
           -vector2[uVar17];
      iVar15 = clist->num_rows_inc_padding;
      iVar16 = iVar15;
      if (iVar15 < 0) {
        iVar16 = iVar15 + 3;
      }
      *(MeReal *)((int)*clist->Jstore + iVar15 * 4 + uVar17 * 0x10 + (iVar16 >> 2) * 0xb0 + 0x90) =
           Perpendicular1[uVar17];
      iVar15 = clist->num_rows_inc_padding;
      iVar16 = iVar15 + 1;
      if (iVar16 < 0) {
        iVar16 = iVar15 + 4;
      }
      uVar18 = uVar17 + 1;
      *(MeReal *)((int)*clist->Jstore + iVar15 * 4 + uVar17 * 0x10 + (iVar16 >> 2) * 0xb0 + 0x94) =
           Perpendicular2[uVar17];
      uVar17 = uVar18;
    } while (uVar18 < 3);
  }
  pMVar3[iVar1] = -3.4028235e+38;
  pMVar4[iVar1] = 3.4028235e+38;
  (pMVar3 + iVar1)[1] = -3.4028235e+38;
  (pMVar4 + iVar1)[1] = 3.4028235e+38;
  *pfVar14 = vector1[2] * fVar11 + vector1[1] * fVar9 + vector1[0] * fVar13;
  pfVar14[1] = vector2[2] * fVar11 + vector2[1] * fVar9 + vector2[0] * fVar13;
  MdtBclEndConstraint(clist,2);
  return;
}


/* ==== MdtBclAddAngular3 ==== */

/* WARNING: Unknown calling convention */

void MdtBclAddAngular3(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                      MdtKeaBody **blist,void *params)

{
  float fVar1;
  int iVar2;
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
  MeReal *pMVar13;
  int iVar14;
  int iVar15;
  float *pfVar16;
  MeReal *pMVar17;
  MeReal *pMVar18;
  float *pfVar19;
  float *pfVar20;
  int iVar21;
  uint uVar22;
  uint uVar23;
  MeReal multfac;
  MeReal Recip_roop1pSqrt2_r00p1;
  MeReal q0q1_1;
  MeReal q3sq_2;
  MeReal q2sq_2;
  MeReal q1sq_2;
  MeReal q1q2;
  MeReal q0q2_1;
  MeReal q3sq_1;
  MeReal q2sq_1;
  MeReal q1sq_1;
  MeReal q0q2;
  MeReal q0q1;
  MeReal q3sq;
  MeReal q2sq;
  MeReal q1sq;
  MeReal q0sq;
  MeReal hepsilon;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal t2 [3];
  MeReal t1 [3];
  MeReal t0 [3];
  MeReal Rrel [3] [3];
  MeReal qmul [4] [4];
  MeReal qrel [4];
  MeReal qbcw [4];
  MeReal qacw [4];
  MeReal qbcb [4];
  MeReal qaca [4];
  MeReal Rbcw [3] [3];
  MeReal Racw [3] [3];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MdtAngular3 * joint@[???]
                       Unresolved local var: uint i@[DW_OP_reg7(EDI)]
                       Unresolved local var: uint j@[???]
                       Unresolved local var: MeReal two_r00p1@[DW_OP_reg12(ST1)] */
  iVar15 = clist->num_rows_exc_padding;
  pMVar17 = clist->c + iVar15;
  pfVar19 = clist->xi + iVar15;
  pMVar13 = clist->lo + iVar15;
  pMVar18 = clist->hi + iVar15;
  pfVar16 = clist->slipfactor + iVar15;
  pfVar20 = clist->xgamma + iVar15;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar15 = 0;
  do {
    iVar2 = clist->num_rows_inc_padding;
    iVar14 = iVar2;
    if (iVar2 < 0) {
      iVar14 = iVar2 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + (iVar14 >> 2) * 0xb0 + iVar15 * 0x10) = 0;
    iVar2 = clist->num_rows_inc_padding;
    iVar14 = iVar2;
    if (iVar2 < 0) {
      iVar14 = iVar2 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + iVar15 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x60) =
         0;
    iVar2 = clist->num_rows_inc_padding;
    iVar14 = iVar2;
    if (iVar2 < 0) {
      iVar14 = iVar2 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + (iVar14 >> 2) * 0xb0 + iVar15 * 0x10 + 0x10) =
         0;
    iVar2 = clist->num_rows_inc_padding;
    iVar14 = iVar2;
    if (iVar2 < 0) {
      iVar14 = iVar2 + 3;
    }
    iVar21 = iVar15 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + iVar15 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x70) =
         0;
    iVar15 = iVar21;
  } while (iVar21 != 6);
  *pfVar19 = 0.0;
  *pfVar16 = 0.0;
  *pMVar17 = 0.0;
  *pfVar20 = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar15 = 0;
  do {
    iVar2 = clist->num_rows_inc_padding;
    iVar14 = iVar2 + 1;
    if (iVar14 < 0) {
      iVar14 = iVar2 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + 4 + (iVar14 >> 2) * 0xb0 + iVar15 * 0x10) = 0;
    iVar2 = clist->num_rows_inc_padding;
    iVar14 = iVar2 + 1;
    if (iVar14 < 0) {
      iVar14 = iVar2 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + iVar15 * 0x10 + (iVar14 >> 2) * 0xb0 + 100) =
         0;
    iVar2 = clist->num_rows_inc_padding;
    iVar14 = iVar2 + 1;
    if (iVar14 < 0) {
      iVar14 = iVar2 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + (iVar14 >> 2) * 0xb0 + iVar15 * 0x10 + 0x14) =
         0;
    iVar2 = clist->num_rows_inc_padding;
    iVar14 = iVar2 + 1;
    if (iVar14 < 0) {
      iVar14 = iVar2 + 4;
    }
    iVar21 = iVar15 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + iVar15 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x74) =
         0;
    iVar15 = iVar21;
  } while (iVar21 != 6);
  pfVar19[1] = 0.0;
  pfVar16[1] = 0.0;
  pMVar17[1] = 0.0;
  pfVar20[1] = 0.0;
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  MeQuaternionFromTM(qaca,(void *)((int)constraint + 0xbc));
  iVar15 = *(int *)((int)constraint + 0x54);
  qacw[0] = ((qaca[0] * *(float *)(iVar15 + 0xb0) - qaca[1] * *(float *)(iVar15 + 0xb4)) -
            qaca[2] * *(float *)(iVar15 + 0xb8)) - qaca[3] * *(float *)(iVar15 + 0xbc);
  qacw[1] = (*(float *)(iVar15 + 0xb8) * qaca[3] +
            qaca[1] * *(float *)(iVar15 + 0xb0) + qaca[0] * *(float *)(iVar15 + 0xb4)) -
            *(float *)(iVar15 + 0xbc) * qaca[2];
  qacw[2] = (*(float *)(iVar15 + 0xbc) * qaca[1] +
            qaca[0] * *(float *)(iVar15 + 0xb8) + qaca[2] * *(float *)(iVar15 + 0xb0)) -
            *(float *)(iVar15 + 0xb4) * qaca[3];
  qacw[3] = (qaca[0] * *(float *)(iVar15 + 0xbc) + qaca[3] * *(float *)(iVar15 + 0xb0) +
            qaca[2] * *(float *)(iVar15 + 0xb4)) - qaca[1] * *(float *)(iVar15 + 0xb8);
  if (*(int *)((int)constraint + 0xb8) == -1) {
    MeQuaternionFromTM(qbcw,(void *)((int)constraint + 0xfc));
    Rbcw[0][0] = *(MeReal *)((int)constraint + 0xfc);
    Rbcw[0][1] = *(MeReal *)((int)constraint + 0x100);
    Rbcw[0][2] = *(MeReal *)((int)constraint + 0x104);
    Rbcw[1][0] = *(MeReal *)((int)constraint + 0x10c);
    Rbcw[1][1] = *(MeReal *)((int)constraint + 0x110);
    Rbcw[1][2] = *(MeReal *)((int)constraint + 0x114);
    Rbcw[2][0] = *(MeReal *)((int)constraint + 0x11c);
    Rbcw[2][1] = *(MeReal *)((int)constraint + 0x120);
    Rbcw[2][2] = *(MeReal *)((int)constraint + 0x124);
  }
  else {
    MeQuaternionFromTM(qbcb,(void *)((int)constraint + 0xfc));
    iVar15 = *(int *)((int)constraint + 0x58);
    qbcw[0] = ((qbcb[0] * *(float *)(iVar15 + 0xb0) - qbcb[1] * *(float *)(iVar15 + 0xb4)) -
              qbcb[2] * *(float *)(iVar15 + 0xb8)) - qbcb[3] * *(float *)(iVar15 + 0xbc);
    qbcw[1] = (*(float *)(iVar15 + 0xb8) * qbcb[3] +
              qbcb[0] * *(float *)(iVar15 + 0xb4) + qbcb[1] * *(float *)(iVar15 + 0xb0)) -
              qbcb[2] * *(float *)(iVar15 + 0xbc);
    qbcw[2] = (qbcb[1] * *(float *)(iVar15 + 0xbc) +
              qbcb[0] * *(float *)(iVar15 + 0xb8) + qbcb[2] * *(float *)(iVar15 + 0xb0)) -
              *(float *)(iVar15 + 0xb4) * qbcb[3];
                    /* Unresolved local var: MeReal q0t2@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q1t2@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q0q3@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q1q2@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q1q3@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q2q3@[DW_OP_reg12(ST1)] */
    qbcw[3] = (qbcb[0] * *(float *)(iVar15 + 0xbc) + qbcb[3] * *(float *)(iVar15 + 0xb0) +
              qbcb[2] * *(float *)(iVar15 + 0xb4)) - qbcb[1] * *(float *)(iVar15 + 0xb8);
    fVar7 = qbcw[0] + qbcw[0];
    fVar3 = qbcw[3] * qbcw[3];
    fVar6 = qbcw[2] * qbcw[2];
    Rbcw[0][0] = ((qbcw[0] * qbcw[0] + qbcw[1] * qbcw[1]) - fVar6) - fVar3;
    fVar1 = (qbcw[1] + qbcw[1]) * qbcw[2];
    fVar4 = (qbcw[1] + qbcw[1]) * qbcw[3];
    Rbcw[1][0] = fVar1 - fVar7 * qbcw[3];
    fVar8 = qbcw[0] * qbcw[0] - qbcw[1] * qbcw[1];
    fVar5 = (qbcw[2] + qbcw[2]) * qbcw[3];
    Rbcw[2][0] = fVar7 * qbcw[2] + fVar4;
    Rbcw[0][1] = fVar1 + fVar7 * qbcw[3];
    Rbcw[0][2] = fVar4 - fVar7 * qbcw[2];
    Rbcw[1][1] = (fVar8 + fVar6) - fVar3;
    Rbcw[2][2] = (fVar8 - fVar6) + fVar3;
    Rbcw[2][1] = fVar5 - qbcw[1] * fVar7;
    Rbcw[1][2] = fVar5 + qbcw[1] * fVar7;
  }
                    /* Unresolved local var: MeReal q0sq@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q0t2@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q1t2@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q0q1@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal q0q3@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q1q3@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q2q3@[DW_OP_reg14(ST3)] */
  fVar5 = ((qacw[1] * qacw[1] + qacw[0] * qacw[0]) - qacw[2] * qacw[2]) - qacw[3] * qacw[3];
  fVar4 = (qacw[0] + qacw[0]) * qacw[3] + (qacw[1] + qacw[1]) * qacw[2];
  fVar3 = (qacw[1] + qacw[1]) * qacw[3] - (qacw[0] + qacw[0]) * qacw[2];
  uVar23 = 0;
  do {
    fVar1 = qacw[uVar23];
    iVar15 = uVar23 + 1;
    qmul[uVar23][0] = fVar1 * qbcw[0];
    uVar22 = uVar23 + 2;
    qmul[uVar23][1] = fVar1 * qbcw[1];
    qmul[uVar23][3] = fVar1 * qbcw[3];
    qmul[uVar23][2] = fVar1 * qbcw[2];
    fVar1 = qacw[iVar15];
    qmul[iVar15][0] = qbcw[0] * fVar1;
    qmul[iVar15][1] = qbcw[1] * fVar1;
    qmul[iVar15][2] = qbcw[2] * fVar1;
    qmul[iVar15][3] = fVar1 * qbcw[3];
    uVar23 = uVar22;
  } while (uVar22 < 4);
  fVar1 = ((qmul[0][1] - qmul[1][0]) + qmul[3][2]) - qmul[2][3];
                    /* Unresolved local var: MeReal q0sq@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q0t2@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q1t2@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q0q2@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal q0q3@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q1q2@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal q1q3@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q2q3@[DW_OP_reg13(ST2)] */
  fVar8 = ((qmul[2][1] - qmul[3][0]) - qmul[1][2]) + qmul[0][3];
  fVar11 = qmul[1][1] + qmul[0][0] + qmul[2][2] + qmul[3][3];
  fVar7 = (-qmul[2][0] - qmul[3][1]) + qmul[0][2] + qmul[1][3];
  fVar6 = (fVar1 + fVar1) * fVar7 - (fVar11 + fVar11) * fVar8;
  fVar9 = ((fVar1 * fVar1 + fVar11 * fVar11) - fVar7 * fVar7) - fVar8 * fVar8;
  fVar7 = (fVar11 + fVar11) * fVar7 + (fVar1 + fVar1) * fVar8;
  fVar8 = fVar9 + 1.0;
  fVar11 = fVar8 + fVar8;
  if (1e-06 < fVar11) {
                    /* Unresolved local var: MeReal Sqrt2_r00p1@[???]
                       Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
    fVar12 = SQRT(fVar11);
    fVar10 = 1.0 / (fVar8 + fVar12);
    fVar8 = (1.0 / (fVar8 * (fVar11 + fVar11 + (fVar9 + 3.0) * fVar12))) * (fVar12 + 1.0);
    *pfVar19 = fVar6 * fVar10;
    pfVar19[1] = fVar10 * fVar7;
    fVar12 = fVar4 * Rbcw[0][2] - fVar3 * Rbcw[0][1];
    fVar11 = Rbcw[0][1] * fVar5 - Rbcw[0][0] * fVar4;
    fVar9 = fVar3 * Rbcw[0][0] - Rbcw[0][2] * fVar5;
    fVar6 = fVar8 * fVar6;
    t1[0] = (fVar4 * Rbcw[1][2] - fVar3 * Rbcw[1][1]) * fVar10 - fVar12 * fVar6;
    fVar8 = fVar8 * fVar7;
    t1[1] = (fVar3 * Rbcw[1][0] - Rbcw[1][2] * fVar5) * fVar10 - fVar9 * fVar6;
    t1[2] = (Rbcw[1][1] * fVar5 - Rbcw[1][0] * fVar4) * fVar10 - fVar6 * fVar11;
    t2[0] = (fVar4 * Rbcw[2][2] - fVar3 * Rbcw[2][1]) * fVar10 - fVar12 * fVar8;
    t2[1] = (fVar3 * Rbcw[2][0] - Rbcw[2][2] * fVar5) * fVar10 - fVar9 * fVar8;
    t2[2] = (fVar5 * Rbcw[2][1] - Rbcw[2][0] * fVar4) * fVar10 - fVar8 * fVar11;
    iVar14 = clist->num_rows_inc_padding;
    iVar15 = iVar14;
    if (iVar14 < 0) {
      iVar15 = iVar14 + 3;
    }
    *(MeReal *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + 0x30) = t1[0];
    iVar14 = clist->num_rows_inc_padding;
    iVar15 = iVar14 + 1;
    if (iVar15 < 0) {
      iVar15 = iVar14 + 4;
    }
    *(MeReal *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + 0x34) = t2[0];
    uVar23 = 1;
    do {
      iVar14 = clist->num_rows_inc_padding;
      iVar15 = iVar14;
      if (iVar14 < 0) {
        iVar15 = iVar14 + 3;
      }
      *(MeReal *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + uVar23 * 0x10 + 0x30) =
           t1[uVar23];
      iVar14 = clist->num_rows_inc_padding;
      iVar15 = iVar14 + 1;
      if (iVar15 < 0) {
        iVar15 = iVar14 + 4;
      }
      *(MeReal *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + uVar23 * 0x10 + 0x34) =
           t2[uVar23];
      iVar14 = clist->num_rows_inc_padding;
      iVar15 = iVar14;
      if (iVar14 < 0) {
        iVar15 = iVar14 + 3;
      }
      *(MeReal *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + uVar23 * 0x10 + 0x40) =
           t1[uVar23 + 1];
      iVar14 = clist->num_rows_inc_padding;
      iVar15 = iVar14 + 1;
      if (iVar15 < 0) {
        iVar15 = iVar14 + 4;
      }
      uVar22 = uVar23 + 2;
      *(MeReal *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + uVar23 * 0x10 + 0x44) =
           t2[uVar23 + 1];
      uVar23 = uVar22;
    } while (uVar22 < 3);
    if (*(int *)((int)constraint + 0xb8) != -1) {
      iVar14 = clist->num_rows_inc_padding;
      iVar15 = iVar14;
      if (iVar14 < 0) {
        iVar15 = iVar14 + 3;
      }
      *(float *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + 0x90) = -t1[0];
      iVar14 = clist->num_rows_inc_padding;
      iVar15 = iVar14 + 1;
      if (iVar15 < 0) {
        iVar15 = iVar14 + 4;
      }
      *(float *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + 0x94) = -t2[0];
      uVar23 = 1;
      do {
        iVar14 = clist->num_rows_inc_padding;
        iVar15 = iVar14;
        if (iVar14 < 0) {
          iVar15 = iVar14 + 3;
        }
        *(float *)((int)*clist->Jstore + iVar14 * 4 + uVar23 * 0x10 + (iVar15 >> 2) * 0xb0 + 0x90) =
             -t1[uVar23];
        iVar14 = clist->num_rows_inc_padding;
        iVar15 = iVar14 + 1;
        if (iVar15 < 0) {
          iVar15 = iVar14 + 4;
        }
        *(float *)((int)*clist->Jstore + iVar14 * 4 + uVar23 * 0x10 + (iVar15 >> 2) * 0xb0 + 0x94) =
             -t2[uVar23];
        iVar14 = clist->num_rows_inc_padding;
        iVar15 = iVar14;
        if (iVar14 < 0) {
          iVar15 = iVar14 + 3;
        }
        *(float *)((int)*clist->Jstore + iVar14 * 4 + uVar23 * 0x10 + (iVar15 >> 2) * 0xb0 + 0xa0) =
             -t1[uVar23 + 1];
        iVar14 = clist->num_rows_inc_padding;
        iVar15 = iVar14 + 1;
        if (iVar15 < 0) {
          iVar15 = iVar14 + 4;
        }
        uVar22 = uVar23 + 2;
        *(float *)((int)*clist->Jstore + iVar14 * 4 + uVar23 * 0x10 + (iVar15 >> 2) * 0xb0 + 0xa4) =
             -t2[uVar23 + 1];
        uVar23 = uVar22;
      } while (uVar22 < 3);
    }
    *pMVar13 = -3.4028235e+38;
    *pMVar18 = 3.4028235e+38;
    pMVar13[1] = -3.4028235e+38;
    pMVar18[1] = 3.4028235e+38;
    hepsilon = 1.0 / (*(float *)((int)constraint + 0x164) * *(float *)params +
                     *(float *)((int)constraint + 0x168));
    *pfVar16 = hepsilon;
    if (hepsilon < 0.0) {
      *pfVar16 = 0.0;
    }
    pfVar16[1] = hepsilon;
    if (hepsilon < 0.0) {
      pfVar16[1] = 0.0;
    }
    *pfVar20 = hepsilon * *(float *)params * *(float *)((int)constraint + 0x164);
    pfVar20[1] = hepsilon * *(float *)params * *(float *)((int)constraint + 0x164);
  }
  if (*(int *)((int)constraint + 0x160) == 0) {
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
    iVar15 = 0;
    do {
      iVar2 = clist->num_rows_inc_padding;
      iVar14 = iVar2 + 2;
      if (iVar14 < 0) {
        iVar14 = iVar2 + 5;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + 8 + (iVar14 >> 2) * 0xb0 + iVar15 * 0x10) =
           0;
      iVar2 = clist->num_rows_inc_padding;
      iVar14 = iVar2 + 2;
      if (iVar14 < 0) {
        iVar14 = iVar2 + 5;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + iVar15 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x68)
           = 0;
      iVar2 = clist->num_rows_inc_padding;
      iVar14 = iVar2 + 2;
      if (iVar14 < 0) {
        iVar14 = iVar2 + 5;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + (iVar14 >> 2) * 0xb0 + iVar15 * 0x10 + 0x18)
           = 0;
      iVar2 = clist->num_rows_inc_padding;
      iVar14 = iVar2 + 2;
      if (iVar14 < 0) {
        iVar14 = iVar2 + 5;
      }
      iVar21 = iVar15 + 2;
      *(undefined4 *)((int)*clist->Jstore + iVar2 * 4 + iVar15 * 0x10 + (iVar14 >> 2) * 0xb0 + 0x78)
           = 0;
      iVar15 = iVar21;
    } while (iVar21 != 6);
    pfVar19[2] = 0.0;
    pfVar16[2] = 0.0;
    pMVar17[2] = 0.0;
    pfVar20[2] = 0.0;
    t2[0] = (((qmul[0][0] + qmul[1][1]) - qmul[3][3]) - qmul[2][2]) * 0.5;
    t2[1] = (qmul[0][3] + qmul[1][2] + qmul[3][0] + qmul[2][1]) * 0.5;
    t2[2] = (((qmul[1][3] - qmul[0][2]) + qmul[3][1]) - qmul[2][0]) * 0.5;
    iVar14 = clist->num_rows_inc_padding;
    iVar15 = iVar14 + 2;
    if (iVar15 < 0) {
      iVar15 = iVar14 + 5;
    }
    *(MeReal *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + 0x38) = t2[0];
    iVar14 = clist->num_rows_inc_padding;
    iVar15 = iVar14 + 2;
    if (iVar15 < 0) {
      iVar15 = iVar14 + 5;
    }
    *(MeReal *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + 0x48) = t2[1];
    iVar14 = clist->num_rows_inc_padding;
    iVar15 = iVar14 + 2;
    if (iVar15 < 0) {
      iVar15 = iVar14 + 5;
    }
    *(MeReal *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + 0x58) = t2[2];
    if (*(int *)((int)constraint + 0xb8) != -1) {
      iVar14 = clist->num_rows_inc_padding;
      iVar15 = iVar14 + 2;
      if (iVar15 < 0) {
        iVar15 = iVar14 + 5;
      }
      *(float *)((int)*clist->Jstore + iVar14 * 4 + (iVar15 >> 2) * 0xb0 + 0x98) = -t2[0];
      iVar14 = clist->num_rows_inc_padding;
      iVar15 = iVar14 + 2;
      if (iVar15 < 0) {
        iVar15 = iVar14 + 5;
      }
      *(float *)((int)*clist->Jstore + iVar14 * 4 + 0xa8 + (iVar15 >> 2) * 0xb0) = -t2[1];
      iVar14 = clist->num_rows_inc_padding;
      iVar15 = iVar14 + 2;
      if (iVar15 < 0) {
        iVar15 = iVar14 + 5;
      }
      *(float *)((int)*clist->Jstore + iVar14 * 4 + 0xb8 + (iVar15 >> 2) * 0xb0) = -t2[2];
    }
    pMVar13[2] = -3.4028235e+38;
    pMVar18[2] = 3.4028235e+38;
    pfVar19[2] = -fVar1;
    pfVar16[2] = hepsilon;
    if (hepsilon < 0.0) {
      pfVar16[2] = 0.0;
    }
    pfVar20[2] = hepsilon * *(float *)params * *(float *)((int)constraint + 0x164);
    uVar23 = 3;
  }
  else {
    uVar23 = 2;
  }
  MdtBclEndConstraint(clist,uVar23);
  return;
}


/* ==== MdtBclAddSpring6 ==== */

/* WARNING: Unknown calling convention */

void MdtBclAddSpring6(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                     MdtKeaBody **blist,void *params)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  MdtKeaJBlockPair *paMVar5;
  float *pfVar6;
  int iVar7;
  MeReal (*paMVar8) [4];
  int iVar9;
  MeReal *pMVar10;
  float *pfVar11;
  MdtKeaTransformation *pMVar12;
  uint uVar13;
  float *pfVar14;
  float *pfVar15;
  int iVar16;
  MeReal *pMVar17;
  float *pfVar18;
  int iVar19;
  uint uVar20;
  undefined1 *local_124;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal angHEpsilon [3];
  MeReal linHEpsilon [3];
  MdtKeaInputRowFactors factors;
  MeReal xiWorld [3];
  MeReal cpos1rel [3];
  MeReal ref2world [4] [4];
  MeReal ref1world [4] [4];
  
                    /* Unresolved local var: MdtSpring6 * spring@[???]
                       Unresolved local var: uint i@[DW_OP_reg7(EDI)]
                       Unresolved local var: MdtBclSolverParameters * bclParams@[???] */
  iVar16 = clist->num_rows_exc_padding;
  pfVar14 = clist->c + iVar16;
  pfVar6 = clist->xi + iVar16;
  pMVar17 = clist->lo + iVar16;
  pMVar10 = clist->hi + iVar16;
  pfVar15 = clist->slipfactor + iVar16;
  pfVar18 = clist->xgamma + iVar16;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar16 = 0;
  do {
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9;
    if (iVar9 < 0) {
      iVar7 = iVar9 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10) = 0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9;
    if (iVar9 < 0) {
      iVar7 = iVar9 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 0x60) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9;
    if (iVar9 < 0) {
      iVar7 = iVar9 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10 + 0x10) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9;
    if (iVar9 < 0) {
      iVar7 = iVar9 + 3;
    }
    iVar19 = iVar16 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 0x70) =
         0;
    iVar16 = iVar19;
  } while (iVar19 != 6);
  *pfVar6 = 0.0;
  *pfVar15 = 0.0;
  *pfVar14 = 0.0;
  *pfVar18 = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar16 = 0;
  do {
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 1;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + 4 + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10) = 0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 1;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 100) = 0
    ;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 1;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10 + 0x14) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 1;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 4;
    }
    iVar19 = iVar16 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 0x74) =
         0;
    iVar16 = iVar19;
  } while (iVar19 != 6);
  pfVar6[1] = 0.0;
  pfVar15[1] = 0.0;
  pfVar14[1] = 0.0;
  pfVar18[1] = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar16 = 0;
  do {
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 2;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + 8 + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10) = 0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 2;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 0x68) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 2;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10 + 0x18) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 2;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 5;
    }
    iVar19 = iVar16 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 0x78) =
         0;
    iVar16 = iVar19;
  } while (iVar19 != 6);
  pfVar6[2] = 0.0;
  pfVar15[2] = 0.0;
  pfVar14[2] = 0.0;
  pfVar18[2] = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar16 = 0;
  do {
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 3;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + 0xc + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10) = 0
    ;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 3;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 0x6c) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 3;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10 + 0x1c) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 3;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 6;
    }
    iVar19 = iVar16 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 0x7c) =
         0;
    iVar16 = iVar19;
  } while (iVar19 != 6);
  pfVar6[3] = 0.0;
  pfVar15[3] = 0.0;
  pfVar14[3] = 0.0;
  pfVar18[3] = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar16 = 0;
  do {
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 4;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + 0x10 + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 4;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 0x70) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 4;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10 + 0x20) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 4;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 7;
    }
    iVar19 = iVar16 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 0x80) =
         0;
    iVar16 = iVar19;
  } while (iVar19 != 6);
  pfVar6[4] = 0.0;
  pfVar15[4] = 0.0;
  pfVar14[4] = 0.0;
  pfVar18[4] = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar16 = 0;
  do {
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 5;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + 0x14 + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 5;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 0x74) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 5;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + (iVar7 >> 2) * 0xb0 + iVar16 * 0x10 + 0x24) =
         0;
    iVar9 = clist->num_rows_inc_padding;
    iVar7 = iVar9 + 5;
    if (iVar7 < 0) {
      iVar7 = iVar9 + 8;
    }
    iVar19 = iVar16 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar9 * 4 + iVar16 * 0x10 + (iVar7 >> 2) * 0xb0 + 0x84) =
         0;
    iVar16 = iVar19;
  } while (iVar19 != 6);
  pfVar6[5] = 0.0;
  pfVar15[5] = 0.0;
  pfVar14[5] = 0.0;
  pfVar18[5] = 0.0;
                    /* Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[???] */
  paMVar8 = ref1world;
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  iVar16 = 3;
  pMVar12 = tlist + *(int *)((int)constraint + 0xb4);
  pfVar11 = (float *)((int)constraint + 0xbc);
  do {
    fVar1 = *pfVar11;
    fVar2 = pfVar11[1];
    fVar3 = pfVar11[2];
    fVar4 = pfVar11[3];
    pfVar11 = pfVar11 + 4;
    (*paMVar8)[0] =
         fVar4 * pMVar12->pos[0] +
         fVar3 * pMVar12->R2[0] + fVar1 * pMVar12->R0[0] + fVar2 * pMVar12->R1[0];
    (*paMVar8)[1] =
         fVar4 * pMVar12->pos[1] +
         fVar3 * pMVar12->R2[1] + fVar1 * pMVar12->R0[1] + fVar2 * pMVar12->R1[1];
    (*paMVar8)[2] =
         fVar4 * pMVar12->pos[2] +
         fVar3 * pMVar12->R2[2] + fVar1 * pMVar12->R0[2] + fVar2 * pMVar12->R1[2];
    (*paMVar8)[3] =
         fVar1 * pMVar12->R0[3] + fVar2 * pMVar12->R1[3] + fVar3 * pMVar12->R2[3] +
         fVar4 * pMVar12->pos[3];
    paMVar8 = paMVar8 + 1;
    iVar16 = iVar16 + -1;
  } while (-1 < iVar16);
  if (*(int *)((int)constraint + 0xb8) == -1) {
    ref2world[0][0] = *(MeReal *)((int)constraint + 0xfc);
    ref2world[0][1] = *(MeReal *)((int)constraint + 0x100);
    ref2world[0][2] = *(MeReal *)((int)constraint + 0x104);
    ref2world[0][3] = *(MeReal *)((int)constraint + 0x108);
    ref2world[1][0] = *(MeReal *)((int)constraint + 0x10c);
    ref2world[1][1] = *(MeReal *)((int)constraint + 0x110);
    ref2world[1][2] = *(MeReal *)((int)constraint + 0x114);
    ref2world[1][3] = *(MeReal *)((int)constraint + 0x118);
    ref2world[2][0] = *(MeReal *)((int)constraint + 0x11c);
    ref2world[2][1] = *(MeReal *)((int)constraint + 0x120);
    ref2world[2][2] = *(MeReal *)((int)constraint + 0x124);
    ref2world[2][3] = *(MeReal *)((int)constraint + 0x128);
    ref2world[3][0] = *(MeReal *)((int)constraint + 300);
    ref2world[3][1] = *(MeReal *)((int)constraint + 0x130);
    ref2world[3][2] = *(MeReal *)((int)constraint + 0x134);
    ref2world[3][3] = *(MeReal *)((int)constraint + 0x138);
  }
  else {
    MeMatrix4MultiplyMatrix
              (ref2world,(MeReal (*) [4])((int)constraint + 0xfc),
               tlist[*(int *)((int)constraint + 0xb8)].R0);
  }
  clist->Jstore[(uint)clist->num_rows_inc_padding >> 2][0].col[0][clist->num_rows_inc_padding & 3] =
       ref2world[0][0];
  clist->Jstore[(uint)clist->num_rows_inc_padding >> 2][0].col[1][clist->num_rows_inc_padding & 3] =
       ref2world[0][1];
  local_124 = &stack0xfffffff4;
  clist->Jstore[(uint)clist->num_rows_inc_padding >> 2][0].col[2][clist->num_rows_inc_padding & 3] =
       ref2world[0][2];
  uVar20 = 1;
  do {
    uVar13 = clist->num_rows_inc_padding + uVar20;
    clist->Jstore[uVar13 >> 2][0].col[0][uVar13 & 3] = *(MeReal *)(local_124 + -0x80);
    uVar13 = clist->num_rows_inc_padding + uVar20;
    clist->Jstore[uVar13 >> 2][0].col[1][uVar13 & 3] = *(MeReal *)(local_124 + -0x7c);
    uVar13 = clist->num_rows_inc_padding + uVar20;
    clist->Jstore[uVar13 >> 2][0].col[2][uVar13 & 3] = *(MeReal *)(local_124 + -0x78);
    iVar16 = uVar20 + 1;
    uVar20 = uVar20 + 2;
    uVar13 = clist->num_rows_inc_padding + iVar16;
    clist->Jstore[uVar13 >> 2][0].col[0][uVar13 & 3] = *(MeReal *)(local_124 + -0x70);
    uVar13 = clist->num_rows_inc_padding + iVar16;
    clist->Jstore[uVar13 >> 2][0].col[1][uVar13 & 3] = *(MeReal *)(local_124 + -0x6c);
    uVar13 = iVar16 + clist->num_rows_inc_padding;
    clist->Jstore[uVar13 >> 2][0].col[2][uVar13 & 3] = *(MeReal *)(local_124 + -0x68);
    local_124 = local_124 + 0x20;
  } while (uVar20 < 3);
  iVar7 = clist->num_rows_inc_padding;
  iVar16 = iVar7;
  if (iVar7 < 0) {
    iVar16 = iVar7 + 3;
  }
  iVar9 = iVar7 + 1;
  iVar16 = iVar7 + (iVar16 >> 2) * 0x2c;
  paMVar5 = clist->Jstore;
  if (iVar9 < 0) {
    iVar9 = iVar7 + 4;
  }
  iVar9 = iVar9 >> 2;
  iVar19 = iVar7 + 2;
  if (iVar19 < 0) {
    iVar19 = iVar7 + 5;
  }
  iVar19 = iVar19 >> 2;
  *(undefined4 *)((int)*paMVar5 + iVar16 * 4 + 0x30) = 0;
  *(undefined4 *)((int)*paMVar5 + iVar16 * 4 + 0x40) = 0;
  *(undefined4 *)((int)*paMVar5 + iVar16 * 4 + 0x50) = 0x80000000;
  *(undefined4 *)((int)*paMVar5 + iVar7 * 4 + iVar9 * 0xb0 + 0x34) = 0x80000000;
  *(undefined4 *)((int)*paMVar5 + iVar7 * 4 + iVar9 * 0xb0 + 0x44) = 0;
  *(undefined4 *)((int)*paMVar5 + iVar7 * 4 + iVar9 * 0xb0 + 0x54) = 0;
  *(undefined4 *)((int)*paMVar5 + iVar7 * 4 + iVar19 * 0xb0 + 0x38) = 0;
  *(undefined4 *)((int)*paMVar5 + iVar7 * 4 + iVar19 * 0xb0 + 0x48) = 0x80000000;
  *(undefined4 *)((int)*paMVar5 + iVar7 * 4 + iVar19 * 0xb0 + 0x58) = 0;
  fVar1 = ref1world[3][0] - ref2world[3][0];
  fVar3 = ref1world[3][1] - ref2world[3][1];
  fVar2 = ref1world[3][2] - ref2world[3][2];
  *pfVar6 = fVar1 * ref2world[0][0] + fVar3 * ref2world[0][1] + fVar2 * ref2world[0][2];
  pfVar6[1] = ref2world[1][2] * fVar2 + ref2world[1][1] * fVar3 + ref2world[1][0] * fVar1;
  pfVar6[2] = ref2world[2][2] * fVar2 + ref2world[2][1] * fVar3 + ref2world[2][0] * fVar1;
  *pfVar14 = ref2world[0][2] * *(float *)((int)constraint + 0x198) +
             ref2world[0][1] * *(float *)((int)constraint + 0x194) +
             ref2world[0][0] * *(float *)((int)constraint + 400);
  pfVar14[1] = ref2world[1][2] * *(float *)((int)constraint + 0x198) +
               ref2world[1][1] * *(float *)((int)constraint + 0x194) +
               ref2world[1][0] * *(float *)((int)constraint + 400);
  pfVar14[2] = ref2world[2][2] * *(float *)((int)constraint + 0x198) +
               ref2world[2][1] * *(float *)((int)constraint + 0x194) +
               ref2world[2][0] * *(float *)((int)constraint + 400);
  *pMVar17 = -3.4028235e+38;
  *pMVar10 = 3.4028235e+38;
  pMVar17[1] = -3.4028235e+38;
  pMVar10[1] = 3.4028235e+38;
  pMVar17[2] = -3.4028235e+38;
  pMVar10[2] = 3.4028235e+38;
  fVar1 = *(float *)params;
  fVar2 = 1.0 / (fVar1 * *(float *)((int)constraint + 0x160) + *(float *)((int)constraint + 0x16c));
  fVar3 = 1.0 / (fVar1 * *(float *)((int)constraint + 0x164) + *(float *)((int)constraint + 0x170));
  fVar1 = 1.0 / (fVar1 * *(float *)((int)constraint + 0x168) + *(float *)((int)constraint + 0x174));
  *pfVar15 = fVar2;
  pfVar15[1] = fVar3;
  pfVar15[2] = fVar1;
  if (fVar2 < 0.0) {
    *pfVar15 = 0.0;
  }
  if (pfVar15[1] < 0.0) {
    pfVar15[1] = 0.0;
  }
  if (pfVar15[2] < 0.0) {
    pfVar15[2] = 0.0;
  }
  *pfVar18 = *(float *)params * fVar2 * *(float *)((int)constraint + 0x160);
  pfVar18[1] = *(float *)params * fVar3 * *(float *)((int)constraint + 0x164);
  pfVar18[2] = *(float *)params * fVar1 * *(float *)((int)constraint + 0x168);
  iVar7 = clist->num_rows_inc_padding;
  iVar16 = iVar7 + 3;
  if (iVar16 < 0) {
    iVar16 = iVar7 + 6;
  }
  iVar16 = iVar16 >> 2;
  paMVar5 = clist->Jstore;
  *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0x3c) =
       ref2world[2][2] * ref1world[0][1] - ref2world[2][1] * ref1world[0][2];
  *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0x4c) =
       ref2world[2][0] * ref1world[0][2] - ref2world[2][2] * ref1world[0][0];
  *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0x5c) =
       ref2world[2][1] * ref1world[0][0] - ref2world[2][0] * ref1world[0][1];
  iVar7 = clist->num_rows_inc_padding;
  iVar16 = iVar7 + 4;
  if (iVar16 < 0) {
    iVar16 = iVar7 + 7;
  }
  iVar16 = iVar16 >> 2;
  paMVar5 = clist->Jstore;
  *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0x40) =
       ref2world[1][2] * ref1world[0][1] - ref2world[1][1] * ref1world[0][2];
  *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0x50) =
       ref2world[1][0] * ref1world[0][2] - ref2world[1][2] * ref1world[0][0];
  *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0x60) =
       ref2world[1][1] * ref1world[0][0] - ref2world[1][0] * ref1world[0][1];
  iVar7 = clist->num_rows_inc_padding;
  iVar16 = iVar7 + 5;
  if (iVar16 < 0) {
    iVar16 = iVar7 + 8;
  }
  iVar16 = iVar16 >> 2;
  paMVar5 = clist->Jstore;
  *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0x44) =
       ref2world[2][2] * ref1world[1][1] - ref2world[2][1] * ref1world[1][2];
  *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0x54) =
       ref2world[2][0] * ref1world[1][2] - ref2world[2][2] * ref1world[1][0];
  *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 100) =
       ref2world[2][1] * ref1world[1][0] - ref2world[2][0] * ref1world[1][1];
  if (*(int *)((int)constraint + 0xb8) != -1) {
    iVar7 = clist->num_rows_inc_padding;
    iVar16 = iVar7 + 3;
    if (iVar16 < 0) {
      iVar16 = iVar7 + 6;
    }
    iVar16 = iVar16 >> 2;
    paMVar5 = clist->Jstore;
    *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0x9c) =
         ref1world[0][2] * ref2world[2][1] - ref1world[0][1] * ref2world[2][2];
    *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0xac) =
         ref1world[0][0] * ref2world[2][2] - ref1world[0][2] * ref2world[2][0];
    *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0xbc) =
         ref1world[0][1] * ref2world[2][0] - ref1world[0][0] * ref2world[2][1];
    iVar7 = clist->num_rows_inc_padding;
    iVar16 = iVar7 + 4;
    if (iVar16 < 0) {
      iVar16 = iVar7 + 7;
    }
    iVar16 = iVar16 >> 2;
    paMVar5 = clist->Jstore;
    *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0xa0) =
         ref1world[0][2] * ref2world[1][1] - ref1world[0][1] * ref2world[1][2];
    *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0xb0) =
         ref1world[0][0] * ref2world[1][2] - ref1world[0][2] * ref2world[1][0];
    *(float *)((int)paMVar5[1] + iVar7 * 4 + iVar16 * 0xb0) =
         ref1world[0][1] * ref2world[1][0] - ref1world[0][0] * ref2world[1][1];
    iVar7 = clist->num_rows_inc_padding;
    iVar16 = iVar7 + 5;
    if (iVar16 < 0) {
      iVar16 = iVar7 + 8;
    }
    iVar16 = iVar16 >> 2;
    paMVar5 = clist->Jstore;
    *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0xa4) =
         ref1world[1][2] * ref2world[2][1] - ref1world[1][1] * ref2world[2][2];
    *(float *)((int)*paMVar5 + iVar7 * 4 + iVar16 * 0xb0 + 0xb4) =
         ref1world[1][0] * ref2world[2][2] - ref1world[1][2] * ref2world[2][0];
    *(float *)((int)paMVar5[1] + iVar7 * 4 + iVar16 * 0xb0 + 4) =
         ref1world[1][1] * ref2world[2][0] - ref1world[1][0] * ref2world[2][1];
  }
  pMVar17[3] = -3.4028235e+38;
  pMVar10[3] = 3.4028235e+38;
  pMVar17[4] = -3.4028235e+38;
  pMVar10[4] = 3.4028235e+38;
  pMVar17[5] = -3.4028235e+38;
  pMVar10[5] = 3.4028235e+38;
  pfVar6[3] = ref2world[2][2] * ref1world[0][2] +
              ref2world[2][1] * ref1world[0][1] + ref2world[2][0] * ref1world[0][0];
  pfVar6[4] = ref2world[1][2] * ref1world[0][2] +
              ref2world[1][1] * ref1world[0][1] + ref2world[1][0] * ref1world[0][0];
  pfVar6[5] = ref2world[2][2] * ref1world[1][2] +
              ref2world[2][1] * ref1world[1][1] + ref2world[2][0] * ref1world[1][0];
  pfVar14[3] = ref2world[0][2] * *(float *)((int)constraint + 0x1a4) +
               ref2world[0][1] * *(float *)((int)constraint + 0x1a0) +
               ref2world[0][0] * *(float *)((int)constraint + 0x19c);
  pfVar14[4] = ref2world[1][2] * *(float *)((int)constraint + 0x1a4) +
               ref2world[1][0] * *(float *)((int)constraint + 0x19c) +
               ref2world[1][1] * *(float *)((int)constraint + 0x1a0);
  pfVar14[5] = ref2world[2][2] * *(float *)((int)constraint + 0x1a4) +
               ref2world[2][0] * *(float *)((int)constraint + 0x19c) +
               ref2world[2][1] * *(float *)((int)constraint + 0x1a0);
  fVar1 = *(float *)params;
  fVar2 = 1.0 / (fVar1 * *(float *)((int)constraint + 0x178) + *(float *)((int)constraint + 0x184));
  fVar3 = 1.0 / (fVar1 * *(float *)((int)constraint + 0x17c) + *(float *)((int)constraint + 0x188));
  fVar1 = 1.0 / (fVar1 * *(float *)((int)constraint + 0x180) + *(float *)((int)constraint + 0x18c));
  pfVar15[3] = fVar2;
  pfVar15[4] = fVar3;
  pfVar15[5] = fVar1;
  if (fVar2 < 0.0) {
    pfVar15[3] = 0.0;
  }
  if (pfVar15[4] < 0.0) {
    pfVar15[4] = 0.0;
  }
  if (pfVar15[5] < 0.0) {
    pfVar15[5] = 0.0;
  }
  pfVar18[3] = *(float *)params * fVar2 * *(float *)((int)constraint + 0x178);
  pfVar18[4] = *(float *)params * fVar3 * *(float *)((int)constraint + 0x17c);
  pfVar18[5] = *(float *)params * fVar1 * *(float *)((int)constraint + 0x180);
  MdtBclEndConstraint(clist,6);
  return;
}


/* ==== MdtBclAddSpring ==== */

/* WARNING: Unknown calling convention */

void MdtBclAddSpring(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                    MdtKeaBody **blist,void *params)

{
  int iVar1;
  MeReal *pMVar2;
  MeReal *pMVar3;
  int iVar4;
  MdtKeaBody *pMVar5;
  MdtKeaBody *pMVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  bool bVar10;
  bool bVar11;
  int iVar12;
  int iVar13;
  MdtBclLimit *limit;
  MeReal *pMVar14;
  int iVar15;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal abs_pos2 [3];
  MeReal abs_offset2 [4];
  MeReal RotationalAxis2 [3];
  MeReal RotationalAxis1 [3];
  MeReal velocity [3];
  MeReal SpringAxis [3];
  MeReal abs_pos1 [3];
  MeReal abs_offset1 [4];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MdtSpring * joint@[???] */
  iVar1 = clist->num_rows_exc_padding;
  factors.c = clist->c + iVar1;
  pMVar14 = clist->xi + iVar1;
  pMVar2 = clist->lo;
  pMVar3 = clist->hi;
  factors.slipfactor = clist->slipfactor + iVar1;
  factors.xgamma = clist->xgamma + iVar1;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar13 = 0;
  do {
    iVar4 = clist->num_rows_inc_padding;
    iVar12 = iVar4;
    if (iVar4 < 0) {
      iVar12 = iVar4 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar4 * 4 + (iVar12 >> 2) * 0xb0 + iVar13 * 0x10) = 0;
    iVar4 = clist->num_rows_inc_padding;
    iVar12 = iVar4;
    if (iVar4 < 0) {
      iVar12 = iVar4 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar4 * 4 + iVar13 * 0x10 + (iVar12 >> 2) * 0xb0 + 0x60) =
         0;
    iVar4 = clist->num_rows_inc_padding;
    iVar12 = iVar4;
    if (iVar4 < 0) {
      iVar12 = iVar4 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar4 * 4 + (iVar12 >> 2) * 0xb0 + iVar13 * 0x10 + 0x10) =
         0;
    iVar4 = clist->num_rows_inc_padding;
    iVar12 = iVar4;
    if (iVar4 < 0) {
      iVar12 = iVar4 + 3;
    }
    iVar15 = iVar13 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar4 * 4 + iVar13 * 0x10 + (iVar12 >> 2) * 0xb0 + 0x70) =
         0;
    iVar13 = iVar15;
  } while (iVar15 != 6);
  *pMVar14 = 0.0;
  *factors.slipfactor = 0.0;
  *factors.c = 0.0;
  *factors.xgamma = 0.0;
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  if (*(int *)((int)constraint + 0x17c) == 0) {
    return;
  }
                    /* Unresolved local var: float __result@[???] */
  factors.xi = pMVar14;
  factors.lo = pMVar2 + iVar1;
  factors.hi = pMVar3 + iVar1;
  MeMatrixMultiply(abs_offset1,4,3,1,tlist[*(int *)((int)constraint + 0xb4)].R0,
                   (MeReal *)((int)constraint + 0x160));
  iVar13 = *(int *)((int)constraint + 0xb4);
  fVar9 = abs_offset1[0] + tlist[iVar13].pos[0];
  fVar8 = abs_offset1[1] + tlist[iVar13].pos[1];
  fVar7 = abs_offset1[2] + tlist[iVar13].pos[2];
  if (*(int *)((int)constraint + 0xb8) == -1) {
    SpringAxis[0] = fVar9 - *(float *)((int)constraint + 0x16c);
    SpringAxis[1] = fVar8 - *(float *)((int)constraint + 0x170);
    SpringAxis[2] = fVar7 - *(float *)((int)constraint + 0x174);
    pMVar5 = blist[iVar13];
    velocity[0] = pMVar5->vel[0];
    velocity[1] = pMVar5->vel[1];
    velocity[2] = pMVar5->vel[2];
  }
  else {
    MeMatrixMultiply(abs_offset2,4,3,1,tlist[*(int *)((int)constraint + 0xb8)].R0,
                     (MeReal *)((int)constraint + 0x16c));
    iVar13 = *(int *)((int)constraint + 0xb8);
    SpringAxis[0] = fVar9 - (abs_offset2[0] + tlist[iVar13].pos[0]);
    SpringAxis[1] = fVar8 - (abs_offset2[1] + tlist[iVar13].pos[1]);
    SpringAxis[2] = fVar7 - (abs_offset2[2] + tlist[iVar13].pos[2]);
    RotationalAxis2[0] = abs_offset2[1] * SpringAxis[2] - abs_offset2[2] * SpringAxis[1];
    RotationalAxis2[1] = abs_offset2[2] * SpringAxis[0] - SpringAxis[2] * abs_offset2[0];
    RotationalAxis2[2] = abs_offset2[0] * SpringAxis[1] - abs_offset2[1] * SpringAxis[0];
    pMVar5 = blist[iVar13];
    pMVar6 = blist[*(int *)((int)constraint + 0xb4)];
    velocity[0] = pMVar6->vel[0] - pMVar5->vel[0];
    velocity[1] = pMVar6->vel[1] - pMVar5->vel[1];
    velocity[2] = pMVar6->vel[2] - pMVar5->vel[2];
  }
  RotationalAxis1[0] = abs_offset1[1] * SpringAxis[2] - abs_offset1[2] * SpringAxis[1];
  RotationalAxis1[1] = abs_offset1[2] * SpringAxis[0] - abs_offset1[0] * SpringAxis[2];
                    /* Unresolved local var: MeReal m@[DW_OP_reg11(ST0)] */
  RotationalAxis1[2] = abs_offset1[0] * SpringAxis[1] - abs_offset1[1] * SpringAxis[0];
  *(float *)((int)constraint + 0x18c) =
       SQRT(SpringAxis[2] * SpringAxis[2] +
            SpringAxis[0] * SpringAxis[0] + SpringAxis[1] * SpringAxis[1]);
  MeVector3Normalize(SpringAxis);
  limit = (MdtBclLimit *)((int)constraint + 0x178);
  *(float *)((int)constraint + 0x1a0) =
       velocity[2] * SpringAxis[2] + velocity[1] * SpringAxis[1] + velocity[0] * SpringAxis[0];
  CalculateLimitPositionAndOvershoot(limit,*(MeReal *)params,0);
  bVar10 = LimitSingleAxis(limit,&factors,0,params);
  if (bVar10) {
    bVar11 = ActuateJointAxes(limit,(int *)((int)constraint + 0xb4),SpringAxis,RotationalAxis1,
                              RotationalAxis2,blist,&factors,0,clist);
    bVar10 = true;
    if (bVar11) goto LAB_0001cff9;
  }
  bVar10 = false;
LAB_0001cff9:
  if (!bVar10) {
    clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
    clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
    iVar12 = clist->num_rows_inc_padding;
    iVar13 = iVar12;
    if (iVar12 < 0) {
      iVar13 = iVar12 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0) = 0;
    iVar12 = clist->num_rows_inc_padding;
    iVar13 = iVar12;
    if (iVar12 < 0) {
      iVar13 = iVar12 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0 + 0x10) = 0;
    iVar12 = clist->num_rows_inc_padding;
    iVar13 = iVar12;
    if (iVar12 < 0) {
      iVar13 = iVar12 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0 + 0x20) = 0;
    iVar12 = clist->num_rows_inc_padding;
    iVar13 = iVar12;
    if (iVar12 < 0) {
      iVar13 = iVar12 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0 + 0x30) = 0;
    iVar12 = clist->num_rows_inc_padding;
    iVar13 = iVar12;
    if (iVar12 < 0) {
      iVar13 = iVar12 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0 + 0x40) = 0;
    iVar12 = clist->num_rows_inc_padding;
    iVar13 = iVar12;
    if (iVar12 < 0) {
      iVar13 = iVar12 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0 + 0x50) = 0;
    if (*(int *)((int)constraint + 0xb8) != -1) {
      iVar12 = clist->num_rows_inc_padding;
      iVar13 = iVar12;
      if (iVar12 < 0) {
        iVar13 = iVar12 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0 + 0x60) = 0;
      iVar12 = clist->num_rows_inc_padding;
      iVar13 = iVar12;
      if (iVar12 < 0) {
        iVar13 = iVar12 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0 + 0x70) = 0;
      iVar12 = clist->num_rows_inc_padding;
      iVar13 = iVar12;
      if (iVar12 < 0) {
        iVar13 = iVar12 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0 + 0x80) = 0;
      iVar12 = clist->num_rows_inc_padding;
      iVar13 = iVar12;
      if (iVar12 < 0) {
        iVar13 = iVar12 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0 + 0x90) = 0;
      iVar12 = clist->num_rows_inc_padding;
      iVar13 = iVar12;
      if (iVar12 < 0) {
        iVar13 = iVar12 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0 + 0xa0) = 0;
      iVar12 = clist->num_rows_inc_padding;
      iVar13 = iVar12;
      if (iVar12 < 0) {
        iVar13 = iVar12 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar12 * 4 + (iVar13 >> 2) * 0xb0 + 0xb0) = 0;
    }
    *pMVar14 = 0.0;
    pMVar2[iVar1] = -3.4028235e+38;
    pMVar3[iVar1] = 3.4028235e+38;
  }
  MdtBclEndConstraint(clist,1);
  return;
}


/* ==== MdtBclAddConeLimit ==== */

void MdtBclAddConeLimit(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                       MdtKeaBody **blist,void *params)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  MeReal *pMVar6;
  MeReal *pMVar7;
  MeReal *pMVar8;
  MeReal *pMVar9;
  int iVar10;
  float *pfVar11;
  MeReal (*paMVar12) [4];
  float *pfVar13;
  MdtKeaTransformation *pMVar14;
  float *pfVar15;
  int iVar16;
  longdouble lVar17;
  int z;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MdtKeaInputRowFactors factors;
  MeReal cross [3];
  MeReal ref2world [4] [4];
  MeReal ref1world [4] [4];
  
                    /* Unresolved local var: MdtConeLimit * joint@[???]
                       Unresolved local var: MeReal coneDot@[DW_OP_reg14(ST3)] */
  iVar5 = clist->num_rows_exc_padding;
  pMVar6 = clist->c;
  pfVar11 = clist->xi + iVar5;
  pMVar7 = clist->lo;
  pMVar8 = clist->hi;
  pfVar15 = clist->slipfactor + iVar5;
  pMVar9 = clist->xgamma;
  z = 0;
  do {
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + z * 0x10) = 0;
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + z * 0x10 + (iVar16 >> 2) * 0xb0 + 0x60) = 0;
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + z * 0x10 + 0x10) = 0;
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + z * 0x10 + (iVar16 >> 2) * 0xb0 + 0x70) = 0;
    z = z + 2;
  } while (z != 6);
  *pfVar11 = 0.0;
  *pfVar15 = 0.0;
                    /* Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[???] */
  paMVar12 = ref1world;
  pMVar6[iVar5] = 0.0;
  pMVar9[iVar5] = 0.0;
  pfVar13 = (float *)((int)constraint + 0xbc);
  pMVar14 = tlist + *(int *)((int)constraint + 0xb4);
  iVar16 = 3;
  do {
    fVar1 = *pfVar13;
    fVar2 = pfVar13[1];
    fVar3 = pfVar13[2];
    fVar4 = pfVar13[3];
    pfVar13 = pfVar13 + 4;
    (*paMVar12)[0] =
         fVar4 * pMVar14->pos[0] +
         fVar3 * pMVar14->R2[0] + fVar1 * pMVar14->R0[0] + fVar2 * pMVar14->R1[0];
    (*paMVar12)[1] =
         fVar4 * pMVar14->pos[1] +
         fVar3 * pMVar14->R2[1] + fVar1 * pMVar14->R0[1] + fVar2 * pMVar14->R1[1];
    (*paMVar12)[2] =
         fVar4 * pMVar14->pos[2] +
         fVar3 * pMVar14->R2[2] + fVar1 * pMVar14->R0[2] + fVar2 * pMVar14->R1[2];
    (*paMVar12)[3] =
         fVar1 * pMVar14->R0[3] + fVar2 * pMVar14->R1[3] + fVar3 * pMVar14->R2[3] +
         fVar4 * pMVar14->pos[3];
    paMVar12 = paMVar12 + 1;
    iVar16 = iVar16 + -1;
  } while (-1 < iVar16);
  if (*(int *)((int)constraint + 0xb8) == -1) {
    ref2world[0][0] = *(MeReal *)((int)constraint + 0xfc);
    ref2world[0][1] = *(MeReal *)((int)constraint + 0x100);
    ref2world[0][2] = *(MeReal *)((int)constraint + 0x104);
    ref2world[0][3] = *(MeReal *)((int)constraint + 0x108);
    ref2world[1][0] = *(MeReal *)((int)constraint + 0x10c);
    ref2world[1][1] = *(MeReal *)((int)constraint + 0x110);
    ref2world[1][2] = *(MeReal *)((int)constraint + 0x114);
    ref2world[1][3] = *(MeReal *)((int)constraint + 0x118);
    ref2world[2][0] = *(MeReal *)((int)constraint + 0x11c);
    ref2world[2][1] = *(MeReal *)((int)constraint + 0x120);
    ref2world[2][2] = *(MeReal *)((int)constraint + 0x124);
    ref2world[2][3] = *(MeReal *)((int)constraint + 0x128);
    ref2world[3][0] = *(MeReal *)((int)constraint + 300);
    ref2world[3][1] = *(MeReal *)((int)constraint + 0x130);
    ref2world[3][2] = *(MeReal *)((int)constraint + 0x134);
    ref2world[3][3] = *(MeReal *)((int)constraint + 0x138);
  }
  else {
    MeMatrix4MultiplyMatrix
              (ref2world,(MeReal (*) [4])((int)constraint + 0xfc),
               tlist[*(int *)((int)constraint + 0xb8)].R0);
  }
  lVar17 = (longdouble)ref2world[0][2] * (longdouble)ref1world[0][2] +
           (longdouble)ref2world[0][1] * (longdouble)ref1world[0][1] +
           (longdouble)ref2world[0][0] * (longdouble)ref1world[0][0];
  if ((longdouble)*(float *)((int)constraint + 0x164) <= lVar17) {
    clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
    clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0) = 0;
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x10) = 0;
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x20) = 0;
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x30) = 0;
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x40) = 0;
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x50) = 0;
    if (*(int *)((int)constraint + 0xb8) != -1) {
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x60) = 0;
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x70) = 0;
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x80) = 0;
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x90) = 0;
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0xa0) = 0;
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0xb0) = 0;
    }
    *pfVar11 = 0.0;
    pMVar7[iVar5] = -3.4028235e+38;
    pMVar8[iVar5] = 3.4028235e+38;
  }
  else {
                    /* Unresolved local var: MeReal angle@[???]
                       Unresolved local var: MeReal hepsilon@[DW_OP_reg11(ST0)] */
    clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
    clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
    if ((longdouble)1 < lVar17) {
      lVar17 = (longdouble)1;
    }
    if (lVar17 < (longdouble)-1.0) {
      lVar17 = (longdouble)-1.0;
    }
                    /* Unresolved local var: longdouble __result@[???]
                       Unresolved local var: longdouble __value@[???] */
    lVar17 = (longdouble)fpatan(SQRT((longdouble)1 - lVar17 * lVar17),lVar17);
    cross[0] = ref1world[0][1] * ref2world[0][2] - ref1world[0][2] * ref2world[0][1];
    cross[1] = ref1world[0][2] * ref2world[0][0] - ref2world[0][2] * ref1world[0][0];
    cross[2] = ref1world[0][0] * ref2world[0][1] - ref2world[0][0] * ref1world[0][1];
    MeVector3Normalize(cross);
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0) = 0;
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x10) = 0;
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x20) = 0;
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(MeReal *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x30) = cross[0];
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(MeReal *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x40) = cross[1];
    iVar10 = clist->num_rows_inc_padding;
    iVar16 = iVar10;
    if (iVar10 < 0) {
      iVar16 = iVar10 + 3;
    }
    *(MeReal *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x50) = cross[2];
    if (*(int *)((int)constraint + 0xb8) != -1) {
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x60) = 0;
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x70) = 0;
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x80) = 0;
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(float *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0x90) = -cross[0];
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(float *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0xa0) = -cross[1];
      iVar10 = clist->num_rows_inc_padding;
      iVar16 = iVar10;
      if (iVar10 < 0) {
        iVar16 = iVar10 + 3;
      }
      *(float *)((int)*clist->Jstore + iVar10 * 4 + (iVar16 >> 2) * 0xb0 + 0xb0) = -cross[2];
    }
    *pfVar11 = *(float *)((int)constraint + 0x160) - (float)lVar17;
    pMVar7[iVar5] = 0.0;
    pMVar8[iVar5] = 3.4028235e+38;
    fVar1 = 1.0 / (*(float *)((int)constraint + 0x168) * *(float *)params +
                  *(float *)((int)constraint + 0x16c));
    *pfVar15 = fVar1;
    if (fVar1 < 0.0) {
      *pfVar15 = 0.0;
    }
    pMVar9[iVar5] = fVar1 * *(float *)params * *(float *)((int)constraint + 0x168);
  }
  MdtBclEndConstraint(clist,1);
  return;
}


/* ==== MdtBclAddSphericalRows ==== */

void MdtBclAddSphericalRows
               (MdtKeaConstraints *clist,MeReal *body0trans,MeReal *body1trans,MeReal *pos0,
               MeReal *pos1,int body1Present)

{
  int iVar1;
  MdtKeaJBlockPair *paMVar2;
  float *pfVar3;
  int iVar4;
  int iVar5;
  MeReal *pMVar6;
  int iVar7;
  MeReal *pMVar8;
  MeReal *pMVar9;
  MeReal *pMVar10;
  MeReal *pMVar11;
  int z_2;
  int z_1;
  int z;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal at2 [4];
  MeReal at1 [4];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: int i@[DW_OP_reg0(EAX)] */
  iVar4 = clist->num_rows_exc_padding;
  pMVar10 = clist->c + iVar4;
  pfVar3 = clist->xi + iVar4;
  pMVar8 = clist->lo + iVar4;
  pMVar6 = clist->hi + iVar4;
  pMVar11 = clist->slipfactor + iVar4;
  pMVar9 = clist->xgamma + iVar4;
  z = 0;
  do {
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1;
    if (iVar1 < 0) {
      iVar4 = iVar1 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + (iVar4 >> 2) * 0xb0 + z * 0x10) = 0;
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1;
    if (iVar1 < 0) {
      iVar4 = iVar1 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + z * 0x10 + (iVar4 >> 2) * 0xb0 + 0x60) = 0;
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1;
    if (iVar1 < 0) {
      iVar4 = iVar1 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + (iVar4 >> 2) * 0xb0 + z * 0x10 + 0x10) = 0;
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1;
    if (iVar1 < 0) {
      iVar4 = iVar1 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + z * 0x10 + (iVar4 >> 2) * 0xb0 + 0x70) = 0;
    z = z + 2;
  } while (z != 6);
  *pfVar3 = 0.0;
  *pMVar11 = 0.0;
  *pMVar10 = 0.0;
  *pMVar9 = 0.0;
  z_1 = 0;
  do {
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1 + 1;
    if (iVar4 < 0) {
      iVar4 = iVar1 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + 4 + (iVar4 >> 2) * 0xb0 + z_1 * 0x10) = 0;
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1 + 1;
    if (iVar4 < 0) {
      iVar4 = iVar1 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + z_1 * 0x10 + (iVar4 >> 2) * 0xb0 + 100) = 0;
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1 + 1;
    if (iVar4 < 0) {
      iVar4 = iVar1 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + (iVar4 >> 2) * 0xb0 + z_1 * 0x10 + 0x14) = 0;
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1 + 1;
    if (iVar4 < 0) {
      iVar4 = iVar1 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + z_1 * 0x10 + (iVar4 >> 2) * 0xb0 + 0x74) = 0;
    z_1 = z_1 + 2;
  } while (z_1 != 6);
  pfVar3[1] = 0.0;
  pMVar11[1] = 0.0;
  pMVar10[1] = 0.0;
  pMVar9[1] = 0.0;
  z_2 = 0;
  do {
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1 + 2;
    if (iVar4 < 0) {
      iVar4 = iVar1 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + 8 + (iVar4 >> 2) * 0xb0 + z_2 * 0x10) = 0;
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1 + 2;
    if (iVar4 < 0) {
      iVar4 = iVar1 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + z_2 * 0x10 + (iVar4 >> 2) * 0xb0 + 0x68) = 0;
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1 + 2;
    if (iVar4 < 0) {
      iVar4 = iVar1 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + (iVar4 >> 2) * 0xb0 + z_2 * 0x10 + 0x18) = 0;
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1 + 2;
    if (iVar4 < 0) {
      iVar4 = iVar1 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + z_2 * 0x10 + (iVar4 >> 2) * 0xb0 + 0x78) = 0;
    z_2 = z_2 + 2;
  } while (z_2 != 6);
  pfVar3[2] = 0.0;
  pMVar11[2] = 0.0;
  pMVar10[2] = 0.0;
  pMVar9[2] = 0.0;
  iVar1 = clist->num_rows_inc_padding;
  iVar4 = iVar1;
  if (iVar1 < 0) {
    iVar4 = iVar1 + 3;
  }
  *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + (iVar4 >> 2) * 0xb0) = 0x3f800000;
  iVar1 = clist->num_rows_inc_padding;
  iVar4 = iVar1 + 1;
  if (iVar4 < 0) {
    iVar4 = iVar1 + 4;
  }
  *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + (iVar4 >> 2) * 0xb0 + 0x14) = 0x3f800000;
  iVar1 = clist->num_rows_inc_padding;
  iVar4 = iVar1 + 2;
  if (iVar4 < 0) {
    iVar4 = iVar1 + 5;
  }
  *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + (iVar4 >> 2) * 0xb0 + 0x28) = 0x3f800000;
  MeMatrixMultiply(at1,4,3,1,body0trans,pos0);
  iVar1 = clist->num_rows_inc_padding;
  iVar4 = iVar1;
  if (iVar1 < 0) {
    iVar4 = iVar1 + 3;
  }
  paMVar2 = clist->Jstore;
  iVar7 = iVar1 + (iVar4 >> 2) * 0x2c;
  iVar4 = iVar1 + 1;
  if (iVar4 < 0) {
    iVar4 = iVar1 + 4;
  }
  iVar4 = iVar4 >> 2;
  iVar5 = iVar1 + 2;
  if (iVar5 < 0) {
    iVar5 = iVar1 + 5;
  }
  iVar5 = iVar5 >> 2;
  *(undefined4 *)((int)*paMVar2 + iVar7 * 4 + 0x30) = 0;
  *(MeReal *)((int)*paMVar2 + iVar7 * 4 + 0x40) = at1[2];
  *(float *)((int)*paMVar2 + iVar7 * 4 + 0x50) = -at1[1];
  *(float *)((int)*paMVar2 + iVar1 * 4 + iVar4 * 0xb0 + 0x34) = -at1[2];
  *(undefined4 *)((int)*paMVar2 + iVar1 * 4 + iVar4 * 0xb0 + 0x44) = 0;
  *(MeReal *)((int)*paMVar2 + iVar1 * 4 + iVar4 * 0xb0 + 0x54) = at1[0];
  *(MeReal *)((int)*paMVar2 + iVar1 * 4 + iVar5 * 0xb0 + 0x38) = at1[1];
  *(float *)((int)*paMVar2 + iVar1 * 4 + iVar5 * 0xb0 + 0x48) = -at1[0];
  *(undefined4 *)((int)*paMVar2 + iVar1 * 4 + iVar5 * 0xb0 + 0x58) = 0;
  if (body1Present == 0) {
    iVar4 = 0;
    do {
      pfVar3[iVar4] = (body0trans[iVar4 + 0xc] + at1[iVar4]) - pos1[iVar4];
      iVar4 = iVar4 + 1;
    } while (iVar4 < 3);
  }
  else {
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1;
    if (iVar1 < 0) {
      iVar4 = iVar1 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + (iVar4 >> 2) * 0xb0 + 0x60) = 0xbf800000;
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1 + 1;
    if (iVar4 < 0) {
      iVar4 = iVar1 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + (iVar4 >> 2) * 0xb0 + 0x74) = 0xbf800000;
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1 + 2;
    if (iVar4 < 0) {
      iVar4 = iVar1 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar1 * 4 + (iVar4 >> 2) * 0xb0 + 0x88) = 0xbf800000;
    MeMatrixMultiply(at2,4,3,1,body1trans,pos1);
    iVar1 = clist->num_rows_inc_padding;
    iVar4 = iVar1;
    if (iVar1 < 0) {
      iVar4 = iVar1 + 3;
    }
    paMVar2 = clist->Jstore;
    iVar7 = iVar1 + (iVar4 >> 2) * 0x2c;
    iVar4 = iVar1 + 1;
    if (iVar4 < 0) {
      iVar4 = iVar1 + 4;
    }
    iVar4 = iVar4 >> 2;
    iVar5 = iVar1 + 2;
    if (iVar5 < 0) {
      iVar5 = iVar1 + 5;
    }
    iVar5 = iVar5 >> 2;
    *(undefined4 *)((int)*paMVar2 + iVar7 * 4 + 0x90) = 0;
    *(float *)((int)*paMVar2 + iVar7 * 4 + 0xa0) = at2[2] * -1.0;
    *(float *)((int)*paMVar2 + iVar7 * 4 + 0xb0) = -at2[1] * -1.0;
    *(float *)((int)*paMVar2 + iVar1 * 4 + iVar4 * 0xb0 + 0x94) = -at2[2] * -1.0;
    *(undefined4 *)((int)*paMVar2 + iVar1 * 4 + iVar4 * 0xb0 + 0xa4) = 0;
    *(float *)((int)*paMVar2 + iVar1 * 4 + iVar4 * 0xb0 + 0xb4) = at2[0] * -1.0;
    *(float *)((int)*paMVar2 + iVar1 * 4 + iVar5 * 0xb0 + 0x98) = at2[1] * -1.0;
    *(undefined4 *)((int)*paMVar2 + iVar1 * 4 + iVar5 * 0xb0 + 0xb8) = 0;
    *(float *)((int)*paMVar2 + iVar1 * 4 + iVar5 * 0xb0 + 0xa8) = -at2[0] * -1.0;
    *pfVar3 = (body0trans[0xc] + at1[0]) - (body1trans[0xc] + at2[0]);
    pfVar3[1] = (body0trans[0xd] + at1[1]) - (body1trans[0xd] + at2[1]);
    pfVar3[2] = (body0trans[0xe] + at1[2]) - (body1trans[0xe] + at2[2]);
  }
  *pMVar8 = -3.4028235e+38;
  *pMVar6 = 3.4028235e+38;
  pMVar8[1] = -3.4028235e+38;
  pMVar6[1] = 3.4028235e+38;
  pMVar8[2] = -3.4028235e+38;
  pMVar6[2] = 3.4028235e+38;
  return;
}


/* ==== MdtBclAddRPROJoint ==== */

/* WARNING: Unknown calling convention */

void MdtBclAddRPROJoint(MdtKeaConstraints *clist,void *constraint,MdtKeaTransformation *tlist,
                       MdtKeaBody **blist,void *params)

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
  int iVar10;
  MeReal *pMVar11;
  MeReal *pMVar12;
  MdtKeaBody *pMVar13;
  MdtKeaBody *pMVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  MeReal *pMVar18;
  int iVar19;
  MeReal *pMVar20;
  MeReal *pMVar21;
  MeReal *pMVar22;
  int iVar23;
  int iVar24;
  int iVar25;
  int local_244;
  int local_240;
  int local_238;
  int j_1;
  int j;
  float local_204;
  float local_200;
  float local_1fc;
  float local_1f8;
  float local_1f4;
  float local_1f0;
  MeReal *xgamma;
  MeReal *slipfactor;
  MeReal *hi;
  MeReal *lo;
  MeReal *xi;
  MeReal *c;
  MeReal err [3];
  MeReal tmp1 [4];
  MeReal tmp [4];
  MeReal q20Adj [4];
  MeReal q2Adj [4];
  MeReal ejej0 [4];
  MeReal qr [4];
  MeReal GrEqt [3] [3];
  MeReal Eq [3] [4];
  MeReal Gr [3] [4];
  MeReal GEt [3] [3];
  MeReal GMatrixB2 [3] [4];
  MeReal EMatrixB1 [3] [4];
  MdtKeaInputRowFactors factors;
  
                    /* Unresolved local var: MdtRPROJoint * joint@[???]
                       Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int j@[???] */
  iVar10 = clist->num_rows_exc_padding;
  pMVar20 = clist->c + iVar10;
  pMVar18 = clist->xi + iVar10;
  pMVar11 = clist->lo;
  pMVar12 = clist->hi;
  pMVar21 = clist->slipfactor + iVar10;
  pMVar22 = clist->xgamma + iVar10;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar25 = 0;
  do {
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19;
    if (iVar19 < 0) {
      iVar24 = iVar19 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10) = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19;
    if (iVar19 < 0) {
      iVar24 = iVar19 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x60)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19;
    if (iVar19 < 0) {
      iVar24 = iVar19 + 3;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10 + 0x10)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19;
    if (iVar19 < 0) {
      iVar24 = iVar19 + 3;
    }
    iVar23 = iVar25 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x70)
         = 0;
    iVar25 = iVar23;
  } while (iVar23 != 6);
  *pMVar18 = 0.0;
  *pMVar21 = 0.0;
  *pMVar20 = 0.0;
  *pMVar22 = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar25 = 0;
  do {
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 1;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + 4 + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10) = 0
    ;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 1;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 100) =
         0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 1;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 4;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10 + 0x14)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 1;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 4;
    }
    iVar23 = iVar25 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x74)
         = 0;
    iVar25 = iVar23;
  } while (iVar23 != 6);
  pMVar18[1] = 0.0;
  pMVar21[1] = 0.0;
  pMVar20[1] = 0.0;
  pMVar22[1] = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar25 = 0;
  do {
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 2;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + 8 + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10) = 0
    ;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 2;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x68)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 2;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 5;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10 + 0x18)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 2;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 5;
    }
    iVar23 = iVar25 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x78)
         = 0;
    iVar25 = iVar23;
  } while (iVar23 != 6);
  pMVar18[2] = 0.0;
  pMVar21[2] = 0.0;
  pMVar20[2] = 0.0;
  pMVar22[2] = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar25 = 0;
  do {
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 3;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + 0xc + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10) =
         0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 3;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x6c)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 3;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 6;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10 + 0x1c)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 3;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 6;
    }
    iVar23 = iVar25 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x7c)
         = 0;
    iVar25 = iVar23;
  } while (iVar23 != 6);
  pMVar18[3] = 0.0;
  pMVar21[3] = 0.0;
  pMVar20[3] = 0.0;
  pMVar22[3] = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar25 = 0;
  do {
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 4;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + 0x10 + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 4;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x70)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 4;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 7;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10 + 0x20)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 4;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 7;
    }
    iVar23 = iVar25 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x80)
         = 0;
    iVar25 = iVar23;
  } while (iVar23 != 6);
  pMVar18[4] = 0.0;
  pMVar21[4] = 0.0;
  pMVar20[4] = 0.0;
  pMVar22[4] = 0.0;
                    /* Unresolved local var: int z@[DW_OP_reg7(EDI)] */
  iVar25 = 0;
  do {
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 5;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + 0x14 + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 5;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x74)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 5;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 8;
    }
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + iVar25 * 0x10 + 0x24)
         = 0;
    iVar19 = clist->num_rows_inc_padding;
    iVar24 = iVar19 + 5;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 8;
    }
    iVar23 = iVar25 + 2;
    *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + iVar25 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x84)
         = 0;
    iVar25 = iVar23;
  } while (iVar23 != 6);
  pMVar18[5] = 0.0;
  pMVar21[5] = 0.0;
  pMVar20[5] = 0.0;
  pMVar22[5] = 0.0;
  clist->Jbody[clist->num_constraints][0] = *(int *)((int)constraint + 0xb4);
  clist->Jbody[clist->num_constraints][1] = *(int *)((int)constraint + 0xb8);
  MdtBclAddSphericalRows
            (clist,tlist[*(int *)((int)constraint + 0xb4)].R0,
             tlist[*(int *)((int)constraint + 0xb8)].R0,(MeReal *)((int)constraint + 0xec),
             (MeReal *)((int)constraint + 300),(uint)(*(int *)((int)constraint + 0xb8) != -1));
  if (*(int *)((int)constraint + 0x170) == 0) {
    local_238 = *(int *)((int)constraint + 0xb4);
    pMVar13 = blist[local_238];
    ejej0[0] = pMVar13->qrot[0];
    ejej0[1] = pMVar13->qrot[1];
    ejej0[2] = pMVar13->qrot[2];
    ejej0[3] = pMVar13->qrot[3];
  }
  else {
    local_238 = *(int *)((int)constraint + 0xb4);
    pMVar13 = blist[local_238];
    fVar1 = *(float *)((int)constraint + 0x160);
    fVar2 = *(float *)((int)constraint + 0x164);
    fVar3 = *(float *)((int)constraint + 0x168);
    fVar4 = *(float *)((int)constraint + 0x16c);
    ejej0[0] = ((fVar1 * pMVar13->qrot[0] - fVar2 * pMVar13->qrot[1]) - fVar3 * pMVar13->qrot[2]) -
               fVar4 * pMVar13->qrot[3];
    ejej0[1] = (pMVar13->qrot[2] * fVar4 + fVar2 * pMVar13->qrot[0] + fVar1 * pMVar13->qrot[1]) -
               pMVar13->qrot[3] * fVar3;
    ejej0[2] = (pMVar13->qrot[3] * fVar2 + fVar1 * pMVar13->qrot[2] + fVar3 * pMVar13->qrot[0]) -
               pMVar13->qrot[1] * fVar4;
    ejej0[3] = (fVar1 * pMVar13->qrot[3] + fVar4 * pMVar13->qrot[0] + fVar3 * pMVar13->qrot[1]) -
               fVar2 * pMVar13->qrot[2];
  }
  EMatrixB1[0][0] = -ejej0[1];
  EMatrixB1[1][0] = -ejej0[2];
  EMatrixB1[0][2] = -ejej0[3];
  EMatrixB1[0][3] = ejej0[2];
  EMatrixB1[2][3] = ejej0[0];
  EMatrixB1[1][2] = ejej0[0];
  EMatrixB1[0][1] = ejej0[0];
  EMatrixB1[2][2] = ejej0[1];
  EMatrixB1[1][1] = ejej0[3];
  iVar25 = *(int *)((int)constraint + 0xb8);
  if (iVar25 == -1) {
    if (*(int *)((int)constraint + 0x170) == 0) {
      pMVar13 = blist[local_238];
      qr[0] = pMVar13->qrot[0];
      qr[1] = pMVar13->qrot[1];
      qr[2] = pMVar13->qrot[2];
      qr[3] = pMVar13->qrot[3];
    }
    else {
      pMVar13 = blist[local_238];
      fVar1 = *(float *)((int)constraint + 0x160);
      fVar2 = *(float *)((int)constraint + 0x164);
      fVar3 = *(float *)((int)constraint + 0x168);
      fVar4 = *(float *)((int)constraint + 0x16c);
      qr[0] = ((fVar1 * pMVar13->qrot[0] - fVar2 * pMVar13->qrot[1]) - fVar3 * pMVar13->qrot[2]) -
              fVar4 * pMVar13->qrot[3];
      qr[1] = (pMVar13->qrot[2] * fVar4 + fVar2 * pMVar13->qrot[0] + fVar1 * pMVar13->qrot[1]) -
              pMVar13->qrot[3] * fVar3;
      qr[2] = (pMVar13->qrot[3] * fVar2 + fVar1 * pMVar13->qrot[2] + fVar3 * pMVar13->qrot[0]) -
              pMVar13->qrot[1] * fVar4;
      qr[3] = (fVar1 * pMVar13->qrot[3] + fVar4 * pMVar13->qrot[0] + fVar3 * pMVar13->qrot[1]) -
              fVar2 * pMVar13->qrot[2];
    }
    GMatrixB2[1][3] = *(MeReal *)((int)constraint + 0x1a8);
    GMatrixB2[2][2] = -GMatrixB2[1][3];
    GMatrixB2[2][1] = *(MeReal *)((int)constraint + 0x1ac);
    GMatrixB2[1][0] = -GMatrixB2[2][1];
    GMatrixB2[0][2] = *(float *)((int)constraint + 0x1b0);
    GMatrixB2[2][0] = -GMatrixB2[0][2];
    GMatrixB2[2][3] = *(MeReal *)((int)constraint + 0x1a4);
  }
  else if (*(int *)((int)constraint + 0x184) == 0) {
    pMVar13 = blist[iVar25];
    err[0] = pMVar13->qrot[0];
    err[1] = -pMVar13->qrot[1];
    err[2] = -pMVar13->qrot[2];
    fVar1 = -pMVar13->qrot[3];
    pMVar14 = blist[local_238];
    qr[0] = ((pMVar14->qrot[0] * err[0] - pMVar14->qrot[1] * err[1]) - pMVar14->qrot[2] * err[2]) -
            pMVar14->qrot[3] * fVar1;
    qr[1] = (err[2] * pMVar14->qrot[3] + pMVar14->qrot[1] * err[0] + pMVar14->qrot[0] * err[1]) -
            fVar1 * pMVar14->qrot[2];
    qr[2] = (fVar1 * pMVar14->qrot[1] + pMVar14->qrot[2] * err[0] + pMVar14->qrot[0] * err[2]) -
            err[1] * pMVar14->qrot[3];
    qr[3] = (fVar1 * pMVar14->qrot[0] + err[0] * pMVar14->qrot[3] + err[1] * pMVar14->qrot[2]) -
            err[2] * pMVar14->qrot[1];
    fVar1 = *(float *)((int)constraint + 0x1a8);
    fVar2 = *(float *)((int)constraint + 0x1a4);
    fVar3 = *(float *)((int)constraint + 0x1ac);
    fVar4 = *(float *)((int)constraint + 0x1b0);
    GMatrixB2[2][3] =
         ((fVar2 * pMVar13->qrot[0] - fVar1 * pMVar13->qrot[1]) - fVar3 * pMVar13->qrot[2]) -
         fVar4 * pMVar13->qrot[3];
    GMatrixB2[1][3] =
         (pMVar13->qrot[2] * fVar4 + fVar2 * pMVar13->qrot[1] + fVar1 * pMVar13->qrot[0]) -
         pMVar13->qrot[3] * fVar3;
    GMatrixB2[2][1] =
         (fVar1 * pMVar13->qrot[3] + fVar2 * pMVar13->qrot[2] + fVar3 * pMVar13->qrot[0]) -
         pMVar13->qrot[1] * fVar4;
    GMatrixB2[0][2] =
         (fVar2 * pMVar13->qrot[3] + fVar4 * pMVar13->qrot[0] + fVar3 * pMVar13->qrot[1]) -
         fVar1 * pMVar13->qrot[2];
    GMatrixB2[2][2] = -GMatrixB2[1][3];
    GMatrixB2[1][0] = -GMatrixB2[2][1];
    GMatrixB2[2][0] = -GMatrixB2[0][2];
  }
  else {
    pMVar13 = blist[iVar25];
    fVar1 = pMVar13->qrot[0];
    local_1f0 = -pMVar13->qrot[1];
    local_1f4 = -pMVar13->qrot[2];
    local_1f8 = -pMVar13->qrot[3];
    fVar2 = *(float *)((int)constraint + 0x174);
    local_1fc = -*(float *)((int)constraint + 0x178);
    local_200 = -*(float *)((int)constraint + 0x17c);
    local_204 = -*(float *)((int)constraint + 0x180);
    fVar3 = *(float *)((int)constraint + 0x1a4);
    fVar4 = *(float *)((int)constraint + 0x1a8);
    fVar5 = *(float *)((int)constraint + 0x178);
    fVar6 = *(float *)((int)constraint + 0x1ac);
    fVar7 = *(float *)((int)constraint + 0x17c);
    fVar8 = *(float *)((int)constraint + 0x1b0);
    fVar9 = *(float *)((int)constraint + 0x180);
    fVar17 = ((fVar2 * fVar3 - fVar4 * fVar5) - fVar6 * fVar7) - fVar8 * fVar9;
    fVar16 = (fVar7 * fVar8 + fVar4 * fVar2 + fVar5 * fVar3) - fVar9 * fVar6;
    fVar15 = (fVar3 * fVar7 + fVar2 * fVar6 + fVar4 * fVar9) - fVar5 * fVar8;
    fVar3 = (fVar3 * fVar9 + fVar8 * fVar2 + fVar6 * fVar5) - fVar7 * fVar4;
    err[0] = ((fVar17 * pMVar13->qrot[0] - fVar16 * pMVar13->qrot[1]) - fVar15 * pMVar13->qrot[2]) -
             fVar3 * pMVar13->qrot[3];
    err[1] = (pMVar13->qrot[2] * fVar3 + fVar16 * pMVar13->qrot[0] + fVar17 * pMVar13->qrot[1]) -
             pMVar13->qrot[3] * fVar15;
    err[2] = (pMVar13->qrot[3] * fVar16 + fVar15 * pMVar13->qrot[0] + fVar17 * pMVar13->qrot[2]) -
             pMVar13->qrot[1] * fVar3;
    GMatrixB2[0][2] =
         (fVar15 * pMVar13->qrot[1] + fVar17 * pMVar13->qrot[3] + fVar3 * pMVar13->qrot[0]) -
         fVar16 * pMVar13->qrot[2];
    fVar3 = *(float *)((int)constraint + 0x160);
    fVar4 = *(float *)((int)constraint + 0x164);
    pMVar13 = blist[local_238];
    fVar5 = *(float *)((int)constraint + 0x168);
    fVar6 = *(float *)((int)constraint + 0x16c);
    fVar7 = ((fVar3 * pMVar13->qrot[0] - fVar4 * pMVar13->qrot[1]) - fVar5 * pMVar13->qrot[2]) -
            fVar6 * pMVar13->qrot[3];
    fVar9 = (pMVar13->qrot[2] * fVar6 + fVar4 * pMVar13->qrot[0] + fVar3 * pMVar13->qrot[1]) -
            pMVar13->qrot[3] * fVar5;
    fVar8 = (fVar4 * pMVar13->qrot[3] + fVar3 * pMVar13->qrot[2] + fVar5 * pMVar13->qrot[0]) -
            pMVar13->qrot[1] * fVar6;
    fVar3 = (fVar3 * pMVar13->qrot[3] + fVar6 * pMVar13->qrot[0] + fVar5 * pMVar13->qrot[1]) -
            fVar4 * pMVar13->qrot[2];
    fVar4 = ((fVar1 * fVar7 - local_1f0 * fVar9) - local_1f4 * fVar8) - local_1f8 * fVar3;
    fVar6 = (local_1f4 * fVar3 + fVar1 * fVar9 + local_1f0 * fVar7) - local_1f8 * fVar8;
    fVar5 = (local_1f4 * fVar7 + fVar1 * fVar8 + local_1f8 * fVar9) - local_1f0 * fVar3;
    fVar1 = (fVar7 * local_1f8 + fVar3 * fVar1 + fVar8 * local_1f0) - fVar9 * local_1f4;
    qr[0] = ((fVar2 * fVar4 - fVar6 * local_1fc) - local_200 * fVar5) - local_204 * fVar1;
    qr[1] = (local_200 * fVar1 + local_1fc * fVar4 + fVar6 * fVar2) - local_204 * fVar5;
    qr[2] = (local_204 * fVar6 + local_200 * fVar4 + fVar2 * fVar5) - local_1fc * fVar1;
    GMatrixB2[2][2] = -err[1];
    qr[3] = (fVar1 * fVar2 + fVar4 * local_204 + fVar5 * local_1fc) - local_200 * fVar6;
    GMatrixB2[1][0] = -err[2];
    GMatrixB2[2][0] = -GMatrixB2[0][2];
    GMatrixB2[1][3] = err[1];
    GMatrixB2[2][1] = err[2];
    GMatrixB2[2][3] = err[0];
  }
  iVar24 = 1;
  GMatrixB2[0][0] = GMatrixB2[2][2];
  GMatrixB2[0][1] = GMatrixB2[2][3];
  GMatrixB2[0][3] = GMatrixB2[1][0];
  GMatrixB2[1][1] = GMatrixB2[2][0];
  GMatrixB2[1][2] = GMatrixB2[2][3];
  EMatrixB1[1][3] = EMatrixB1[0][0];
  EMatrixB1[2][0] = EMatrixB1[0][2];
  EMatrixB1[2][1] = EMatrixB1[1][0];
  MyMultiplyT2(3,4,3,GMatrixB2[0],EMatrixB1[0],GEt[0]);
  GEt[0][0] = GEt[0][0] * 0.5;
  GEt[0][1] = GEt[0][1] * 0.5;
  GEt[0][2] = GEt[0][2] * 0.5;
  iVar25 = 3;
  do {
    iVar24 = iVar24 + 2;
    GEt[0][iVar25] = GEt[0][iVar25] * 0.5;
    GEt[0][iVar25 + 1] = GEt[0][iVar25 + 1] * 0.5;
    GEt[0][iVar25 + 2] = GEt[0][iVar25 + 2] * 0.5;
    GEt[1][iVar25] = GEt[1][iVar25] * 0.5;
    GEt[1][iVar25 + 1] = GEt[1][iVar25 + 1] * 0.5;
    GEt[1][iVar25 + 2] = GEt[1][iVar25 + 2] * 0.5;
    iVar25 = iVar25 + 6;
  } while (iVar24 < 3);
  iVar25 = 0;
  local_244 = 0;
  do {
                    /* Unresolved local var: uint j@[DW_OP_reg3(EBX)] */
    iVar19 = clist->num_rows_inc_padding + iVar25;
    iVar24 = iVar19 + 3;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 6;
    }
    *(MeReal *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + 0x3c) = GEt[0][local_244];
    iVar19 = clist->num_rows_inc_padding + iVar25;
    iVar24 = iVar19 + 3;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 6;
    }
    *(MeReal *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + 0x4c) =
         GEt[0][local_244 + 1];
    iVar19 = clist->num_rows_inc_padding + iVar25;
    iVar24 = iVar19 + 3;
    if (iVar24 < 0) {
      iVar24 = iVar19 + 6;
    }
    iVar25 = iVar25 + 1;
    *(MeReal *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + 0x5c) =
         GEt[0][local_244 + 2];
    local_244 = local_244 + 3;
  } while (iVar25 < 3);
  if (*(int *)((int)constraint + 0xb8) != -1) {
    iVar25 = 0;
    local_240 = 0;
    do {
                    /* Unresolved local var: uint j@[DW_OP_reg3(EBX)] */
      iVar19 = clist->num_rows_inc_padding + iVar25;
      iVar24 = iVar19 + 3;
      if (iVar24 < 0) {
        iVar24 = iVar19 + 6;
      }
      *(float *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + 0x9c) =
           -GEt[0][local_240];
      iVar19 = clist->num_rows_inc_padding + iVar25;
      iVar24 = iVar19 + 3;
      if (iVar24 < 0) {
        iVar24 = iVar19 + 6;
      }
      *(float *)((int)*clist->Jstore + iVar19 * 4 + 0xac + (iVar24 >> 2) * 0xb0) =
           -GEt[0][local_240 + 1];
      iVar19 = clist->num_rows_inc_padding + iVar25;
      iVar24 = iVar19 + 3;
      if (iVar24 < 0) {
        iVar24 = iVar19 + 6;
      }
      iVar25 = iVar25 + 1;
      *(float *)((int)*clist->Jstore + iVar19 * 4 + 0xbc + (iVar24 >> 2) * 0xb0) =
           -GEt[0][local_240 + 2];
      local_240 = local_240 + 3;
    } while (iVar25 < 3);
  }
  Gr[1][3] = *(MeReal *)((int)constraint + 0x1a8);
  Gr[0][0] = -Gr[1][3];
  Gr[2][1] = *(MeReal *)((int)constraint + 0x1ac);
  Gr[0][3] = -Gr[2][1];
  Gr[0][2] = *(float *)((int)constraint + 0x1b0);
  Gr[1][1] = -Gr[0][2];
  Gr[0][1] = *(MeReal *)((int)constraint + 0x1a4);
  Gr[1][0] = Gr[0][3];
  Gr[1][2] = Gr[0][1];
  Gr[2][0] = Gr[1][1];
  Gr[2][2] = Gr[0][0];
  Gr[2][3] = Gr[0][1];
  MyMultiplyT2(3,4,1,Gr[0],qr,err);
  pMVar18[3] = err[0];
  pMVar18[4] = err[1];
  pMVar18[5] = err[2];
  Eq[0][3] = qr[2];
  Eq[1][0] = -qr[2];
  Eq[2][3] = qr[0];
  Eq[1][2] = qr[0];
  Eq[0][1] = qr[0];
  Eq[0][0] = -qr[1];
  Eq[0][2] = -qr[3];
  Eq[1][1] = qr[3];
  Eq[2][2] = qr[1];
  Eq[1][3] = Eq[0][0];
  Eq[2][0] = Eq[0][2];
  Eq[2][1] = Eq[1][0];
  MyMultiplyT2(3,4,3,Gr[0],Eq[0],GrEqt[0]);
  MyMultiplyT2(3,3,1,GrEqt[0],(MeReal *)((int)constraint + 0x1b4),pMVar20 + 3);
  pMVar20[3] = pMVar20[3] * 0.5;
  pMVar20[4] = pMVar20[4] * 0.5;
  pMVar20[5] = pMVar20[5] * 0.5;
  iVar25 = 0;
  do {
    pMVar11[iVar10 + iVar25] = -*(MeReal *)((int)constraint + iVar25 * 4 + 0x1cc);
    fVar1 = *(float *)((int)constraint + iVar25 * 4 + 0x1cc);
    pMVar12[iVar10 + iVar25] = fVar1;
    if (fVar1 == 0.0) {
      pMVar18[iVar25] = 0.0;
      iVar19 = clist->num_rows_inc_padding + iVar25;
      iVar24 = iVar19;
      if (iVar19 < 0) {
        iVar24 = iVar19 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0) = 0;
      if (*(int *)((int)constraint + 0xb8) != -1) {
        iVar19 = clist->num_rows_inc_padding + iVar25;
        iVar24 = iVar19;
        if (iVar19 < 0) {
          iVar24 = iVar19 + 3;
        }
        *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + 0x60) = 0;
      }
      j = 1;
      do {
        iVar19 = clist->num_rows_inc_padding + iVar25;
        iVar24 = iVar19;
        if (iVar19 < 0) {
          iVar24 = iVar19 + 3;
        }
        *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + j * 0x10) = 0;
        if (*(int *)((int)constraint + 0xb8) != -1) {
          iVar19 = clist->num_rows_inc_padding + iVar25;
          iVar24 = iVar19;
          if (iVar19 < 0) {
            iVar24 = iVar19 + 3;
          }
          *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + j * 0x10 + (iVar24 >> 2) * 0xb0 + 0x60)
               = 0;
        }
        iVar24 = clist->num_rows_inc_padding + iVar25;
        if (iVar24 < 0) {
          iVar24 = iVar24 + 3;
        }
        *(undefined4 *)
         ((int)*clist->Jstore +
         clist->num_rows_inc_padding * 4 + iVar25 * 4 + (iVar24 >> 2) * 0xb0 + j * 0x10 + 0x10) = 0;
        if (*(int *)((int)constraint + 0xb8) != -1) {
          iVar24 = clist->num_rows_inc_padding + iVar25;
          if (iVar24 < 0) {
            iVar24 = iVar24 + 3;
          }
          *(undefined4 *)
           ((int)*clist->Jstore +
           clist->num_rows_inc_padding * 4 + iVar25 * 4 + j * 0x10 + (iVar24 >> 2) * 0xb0 + 0x70) =
               0;
        }
        j = j + 2;
      } while (j < 3);
    }
    iVar25 = iVar25 + 1;
  } while (iVar25 < 3);
  iVar25 = 3;
  do {
    pMVar11[iVar10 + iVar25] = -*(MeReal *)((int)constraint + iVar25 * 4 + 0x1b4);
    fVar1 = *(float *)((int)constraint + iVar25 * 4 + 0x1b4);
    pMVar12[iVar10 + iVar25] = fVar1;
    if (fVar1 == 0.0) {
      pMVar18[iVar25] = 0.0;
      iVar19 = clist->num_rows_inc_padding + iVar25;
      iVar24 = iVar19;
      if (iVar19 < 0) {
        iVar24 = iVar19 + 3;
      }
      *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + 0x30) = 0;
      if (*(int *)((int)constraint + 0xb8) != -1) {
        iVar19 = clist->num_rows_inc_padding + iVar25;
        iVar24 = iVar19;
        if (iVar19 < 0) {
          iVar24 = iVar19 + 3;
        }
        *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + 0x90 + (iVar24 >> 2) * 0xb0) = 0;
      }
      j_1 = 4;
      do {
        iVar19 = clist->num_rows_inc_padding + iVar25;
        iVar24 = iVar19;
        if (iVar19 < 0) {
          iVar24 = iVar19 + 3;
        }
        *(undefined4 *)((int)*clist->Jstore + iVar19 * 4 + (iVar24 >> 2) * 0xb0 + j_1 * 0x10) = 0;
        if (*(int *)((int)constraint + 0xb8) != -1) {
          iVar19 = clist->num_rows_inc_padding + iVar25;
          iVar24 = iVar19;
          if (iVar19 < 0) {
            iVar24 = iVar19 + 3;
          }
          *(undefined4 *)
           ((int)*clist->Jstore + iVar19 * 4 + j_1 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x60) = 0;
        }
        iVar24 = clist->num_rows_inc_padding + iVar25;
        if (iVar24 < 0) {
          iVar24 = iVar24 + 3;
        }
        *(undefined4 *)
         ((int)*clist->Jstore +
         clist->num_rows_inc_padding * 4 + iVar25 * 4 + (iVar24 >> 2) * 0xb0 + j_1 * 0x10 + 0x10) =
             0;
        if (*(int *)((int)constraint + 0xb8) != -1) {
          iVar24 = clist->num_rows_inc_padding + iVar25;
          if (iVar24 < 0) {
            iVar24 = iVar24 + 3;
          }
          *(undefined4 *)
           ((int)*clist->Jstore +
           clist->num_rows_inc_padding * 4 + iVar25 * 4 + j_1 * 0x10 + (iVar24 >> 2) * 0xb0 + 0x70)
               = 0;
        }
        j_1 = j_1 + 2;
      } while (j_1 < 6);
    }
    iVar25 = iVar25 + 1;
  } while (iVar25 < 6);
  MdtBclEndConstraint(clist,6);
  return;
}


/* ==== FUN_00020000 ==== */

void FUN_00020000(void)

{
  undefined4 uVar1;
  float fVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  int unaff_EBP;
  int iVar10;
  uint uVar11;
  int iVar12;
  int unaff_EDI;
  longdouble in_ST0;
  longdouble in_ST1;
  longdouble in_ST2;
  longdouble lVar13;
  longdouble in_ST3;
  longdouble in_ST4;
  longdouble in_ST5;
  longdouble in_ST6;
  
  fVar2 = *(float *)(unaff_EDI + 8);
  *(float *)(unaff_EBP + -0x25c) = (float)in_ST4;
  uVar11 = *(uint *)(unaff_EBP + -0x25c);
  *(float *)(unaff_EBP + -0x25c) = (float)in_ST6;
  lVar13 = (in_ST0 + in_ST5 + in_ST2) - in_ST1 * (longdouble)fVar2;
  uVar11 = uVar11 ^ 0x80000000;
  uVar5 = *(uint *)(unaff_EBP + -0x25c) ^ 0x80000000;
  *(uint *)(unaff_EBP + -0x98) = uVar11;
  *(float *)(unaff_EBP + -0x1ac) = (float)lVar13;
  *(uint *)(unaff_EBP + -0x88) = uVar5;
  *(uint *)(unaff_EBP + -0x70) = uVar11;
  *(float *)(unaff_EBP + -0x25c) = (float)lVar13;
  *(uint *)(unaff_EBP + -0x8c) = uVar5;
  uVar11 = *(uint *)(unaff_EBP + -0x25c) ^ 0x80000000;
  *(uint *)(unaff_EBP + -0x78) = uVar11;
  *(uint *)(unaff_EBP + -0x84) = uVar11;
  *(float *)(unaff_EBP + -0x6c) = (float)in_ST3;
  *(float *)(unaff_EBP + -0x80) = (float)in_ST3;
  *(float *)(unaff_EBP + -0x94) = (float)in_ST3;
  *(float *)(unaff_EBP + -0x7c) = (float)in_ST4;
  *(float *)(unaff_EBP + -0x74) = (float)in_ST6;
  *(float *)(unaff_EBP + -0x90) = (float)lVar13;
  iVar12 = 1;
  MyMultiplyT2(3,4,3,(MeReal *)(unaff_EBP + -0x98),(MeReal *)(unaff_EBP + -0x68),
               (MeReal *)(unaff_EBP + -200));
  iVar6 = 3;
  *(float *)(unaff_EBP + -200) = *(float *)(unaff_EBP + -200) * 0.5;
  *(float *)(unaff_EBP + -0xc4) = *(float *)(unaff_EBP + -0xc4) * 0.5;
  *(float *)(unaff_EBP + -0xc0) = *(float *)(unaff_EBP + -0xc0) * 0.5;
  do {
    iVar9 = iVar6 + 3;
    iVar12 = iVar12 + 2;
    *(float *)(unaff_EBP + -200 + iVar6 * 4) = *(float *)(unaff_EBP + -200 + iVar6 * 4) * 0.5;
    *(float *)(unaff_EBP + -200 + (iVar6 + 1) * 4) =
         *(float *)(unaff_EBP + -200 + (iVar6 + 1) * 4) * 0.5;
    iVar10 = iVar6 + 4;
    *(float *)(unaff_EBP + -200 + (iVar6 + 2) * 4) =
         *(float *)(unaff_EBP + -200 + (iVar6 + 2) * 4) * 0.5;
    iVar7 = iVar6 + 5;
    iVar6 = iVar6 + 6;
    *(float *)(unaff_EBP + -200 + iVar9 * 4) = *(float *)(unaff_EBP + -200 + iVar9 * 4) * 0.5;
    *(float *)(unaff_EBP + -200 + iVar10 * 4) = *(float *)(unaff_EBP + -200 + iVar10 * 4) * 0.5;
    *(float *)(unaff_EBP + -200 + iVar7 * 4) = *(float *)(unaff_EBP + -200 + iVar7 * 4) * 0.5;
  } while (iVar12 < 3);
  iVar6 = 0;
  *(undefined4 *)(unaff_EBP + -0x240) = 0;
  do {
    iVar10 = *(int *)(unaff_EBP + -0x240);
    iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
    iVar12 = iVar7 + 3;
    iVar9 = iVar12;
    if (iVar12 < 0) {
      iVar9 = iVar7 + 6;
    }
    *(undefined4 *)
     (iVar12 * 4 + (iVar9 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x30) =
         *(undefined4 *)(unaff_EBP + -200 + iVar10 * 4);
    iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
    iVar12 = iVar7 + 3;
    iVar9 = iVar12;
    if (iVar12 < 0) {
      iVar9 = iVar7 + 6;
    }
    *(undefined4 *)
     (iVar12 * 4 + (iVar9 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x40) =
         *(undefined4 *)(unaff_EBP + -200 + (iVar10 + 1) * 4);
    iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
    iVar12 = iVar7 + 3;
    iVar9 = iVar12;
    if (iVar12 < 0) {
      iVar9 = iVar7 + 6;
    }
    iVar6 = iVar6 + 1;
    *(undefined4 *)
     (iVar12 * 4 + (iVar9 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x50) =
         *(undefined4 *)(unaff_EBP + -200 + (iVar10 + 2) * 4);
    *(int *)(unaff_EBP + -0x240) = *(int *)(unaff_EBP + -0x240) + 3;
  } while (iVar6 < 3);
  if (*(int *)(*(int *)(unaff_EBP + 0xc) + 0xb8) != -1) {
    iVar6 = 0;
    *(undefined4 *)(unaff_EBP + -0x23c) = 0;
    do {
      iVar10 = *(int *)(unaff_EBP + -0x23c);
      iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
      iVar12 = iVar7 + 3;
      iVar9 = iVar12;
      if (iVar12 < 0) {
        iVar9 = iVar7 + 6;
      }
      *(int *)(unaff_EBP + -0x260) = iVar9 >> 2;
      iVar7 = *(int *)(unaff_EBP + 8);
      *(uint *)((iVar9 >> 2) * 0xc0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x90 +
               (iVar12 + *(int *)(unaff_EBP + -0x260) * -4) * 4) =
           *(uint *)(unaff_EBP + -200 + iVar10 * 4) ^ 0x80000000;
      iVar7 = *(int *)(iVar7 + 0x14) + iVar6;
      iVar12 = iVar7 + 3;
      iVar9 = iVar12;
      if (iVar12 < 0) {
        iVar9 = iVar7 + 6;
      }
      *(int *)(unaff_EBP + -0x260) = iVar9 >> 2;
      iVar7 = *(int *)(unaff_EBP + 8);
      *(uint *)((iVar9 >> 2) * 0xc0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x90 +
               (iVar12 + *(int *)(unaff_EBP + -0x260) * -4 + 4) * 4) =
           *(uint *)(unaff_EBP + -200 + (iVar10 + 1) * 4) ^ 0x80000000;
      iVar7 = *(int *)(iVar7 + 0x14) + iVar6;
      iVar12 = iVar7 + 3;
      iVar9 = iVar12;
      if (iVar12 < 0) {
        iVar9 = iVar7 + 6;
      }
      iVar6 = iVar6 + 1;
      *(int *)(unaff_EBP + -0x260) = iVar9 >> 2;
      *(uint *)((iVar9 >> 2) * 0xc0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x90 +
               (iVar12 + *(int *)(unaff_EBP + -0x260) * -4 + 8) * 4) =
           *(uint *)(unaff_EBP + -200 + (iVar10 + 2) * 4) ^ 0x80000000;
      *(int *)(unaff_EBP + -0x23c) = *(int *)(unaff_EBP + -0x23c) + 3;
    } while (iVar6 < 3);
  }
  iVar6 = *(int *)(unaff_EBP + -0x238);
  uVar1 = *(undefined4 *)(iVar6 + 4);
  *(undefined4 *)(unaff_EBP + -0x25c) = uVar1;
  uVar8 = *(uint *)(unaff_EBP + -0x25c) ^ 0x80000000;
  *(uint *)(unaff_EBP + -0xf8) = uVar8;
  uVar11 = *(uint *)(iVar6 + 8);
  uVar5 = uVar11 ^ 0x80000000;
  *(uint *)(unaff_EBP + -0xe8) = uVar5;
  uVar3 = *(undefined4 *)(iVar6 + 0xc);
  *(undefined4 *)(unaff_EBP + -0x25c) = uVar3;
  *(float *)(unaff_EBP + -0xd8) = -*(float *)(unaff_EBP + -0x25c);
  uVar4 = *(undefined4 *)(*(int *)(unaff_EBP + 0xc) + 0x1a4);
  *(uint *)(unaff_EBP + -0xd4) = uVar11;
  *(undefined4 *)(unaff_EBP + -0xcc) = uVar4;
  *(undefined4 *)(unaff_EBP + -0xe0) = uVar4;
  *(undefined4 *)(unaff_EBP + -0xf4) = uVar4;
  *(undefined4 *)(unaff_EBP + -0xf0) = uVar3;
  *(uint *)(unaff_EBP + -0xd0) = uVar8;
  *(uint *)(unaff_EBP + -0xec) = uVar5;
  *(float *)(unaff_EBP + -0xe4) = -*(float *)(unaff_EBP + -0x25c);
  *(undefined4 *)(unaff_EBP + -0xdc) = uVar1;
  MyMultiplyT2(3,4,1,(MeReal *)(unaff_EBP + -0xf8),(MeReal *)(unaff_EBP + -0x168),
               (MeReal *)(unaff_EBP + -0x1c8));
  iVar6 = *(int *)(unaff_EBP + -0x1d4);
  *(undefined4 *)(iVar6 + 0xc) = *(undefined4 *)(unaff_EBP + -0x1c8);
  *(undefined4 *)(iVar6 + 0x10) = *(undefined4 *)(unaff_EBP + -0x1c4);
  *(undefined4 *)(iVar6 + 0x14) = *(undefined4 *)(unaff_EBP + -0x1c0);
  *(uint *)(unaff_EBP + -0x11c) = *(uint *)(unaff_EBP + -0x160);
  uVar11 = *(uint *)(unaff_EBP + -0x160) ^ 0x80000000;
  uVar1 = *(undefined4 *)(unaff_EBP + -0x168);
  *(undefined4 *)(unaff_EBP + -0x25c) = *(undefined4 *)(unaff_EBP + -0x164);
  *(uint *)(unaff_EBP + -0x118) = uVar11;
  *(uint *)(unaff_EBP + -0x104) = uVar11;
  *(undefined4 *)(unaff_EBP + -0xfc) = uVar1;
  *(undefined4 *)(unaff_EBP + -0x110) = uVar1;
  *(undefined4 *)(unaff_EBP + -0x124) = uVar1;
  uVar11 = *(uint *)(unaff_EBP + -0x15c) ^ 0x80000000;
  *(uint *)(unaff_EBP + -0x114) = *(uint *)(unaff_EBP + -0x15c);
  *(uint *)(unaff_EBP + -0x108) = uVar11;
  *(float *)(unaff_EBP + -0x128) = -*(float *)(unaff_EBP + -0x25c);
  *(float *)(unaff_EBP + -0x10c) = -*(float *)(unaff_EBP + -0x25c);
  *(undefined4 *)(unaff_EBP + -0x100) = *(undefined4 *)(unaff_EBP + -0x164);
  *(uint *)(unaff_EBP + -0x120) = uVar11;
  MyMultiplyT2(3,4,3,(MeReal *)(unaff_EBP + -0xf8),(MeReal *)(unaff_EBP + -0x128),
               (MeReal *)(unaff_EBP + -0x158));
  iVar6 = *(int *)(unaff_EBP + -0x1d0);
  MyMultiplyT2(3,3,1,(MeReal *)(unaff_EBP + -0x158),(MeReal *)(*(int *)(unaff_EBP + 0xc) + 0x1b4),
               (MeReal *)(iVar6 + 0xc));
  *(float *)(*(int *)(unaff_EBP + -0x1d0) + 0xc) =
       *(float *)(*(int *)(unaff_EBP + -0x1d0) + 0xc) * 0.5;
  *(float *)(iVar6 + 0x10) = *(float *)(iVar6 + 0x10) * 0.5;
  *(float *)(iVar6 + 0x14) = *(float *)(iVar6 + 0x14) * 0.5;
  iVar6 = 0;
  do {
    iVar12 = *(int *)(unaff_EBP + 0xc);
    *(uint *)(*(int *)(unaff_EBP + -0x1d8) + iVar6 * 4) =
         *(uint *)(iVar12 + 0x1cc + iVar6 * 4) ^ 0x80000000;
    fVar2 = *(float *)(iVar12 + 0x1cc + iVar6 * 4);
    *(float *)(*(int *)(unaff_EBP + -0x1dc) + iVar6 * 4) = fVar2;
    if (fVar2 == 0.0) {
      iVar12 = *(int *)(unaff_EBP + 8);
      *(undefined4 *)(*(int *)(unaff_EBP + -0x1d4) + iVar6 * 4) = 0;
      *(undefined4 *)(unaff_EBP + -0x22c) = 0;
      iVar9 = *(int *)(iVar12 + 0x14) + iVar6;
      iVar12 = iVar9;
      if (iVar9 < 0) {
        iVar12 = iVar9 + 3;
      }
      iVar10 = *(int *)(unaff_EBP + -0x22c);
      iVar7 = *(int *)(unaff_EBP + 0xc);
      *(undefined4 *)
       (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar10 * 0x10)
           = 0;
      if (*(int *)(iVar7 + 0xb8) != -1) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        *(undefined4 *)
         ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
         iVar9 * 4 + iVar10 * 0x10) = 0;
      }
      *(int *)(unaff_EBP + -0x22c) = *(int *)(unaff_EBP + -0x22c) + 1;
      iVar12 = *(int *)(unaff_EBP + -0x22c);
      while (iVar12 < 3) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar10 = *(int *)(unaff_EBP + -0x22c);
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar10 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar10 * 0x10) = 0;
        }
        iVar10 = *(int *)(unaff_EBP + -0x22c) + 1;
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar10 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar10 * 0x10) = 0;
        }
        *(int *)(unaff_EBP + -0x22c) = *(int *)(unaff_EBP + -0x22c) + 2;
        iVar12 = *(int *)(unaff_EBP + -0x22c);
      }
    }
    iVar6 = iVar6 + 1;
  } while (iVar6 < 3);
  iVar6 = 3;
  do {
    iVar12 = *(int *)(unaff_EBP + 0xc);
    *(uint *)(*(int *)(unaff_EBP + -0x1d8) + iVar6 * 4) =
         *(uint *)(iVar12 + 0x1b4 + iVar6 * 4) ^ 0x80000000;
    fVar2 = *(float *)(iVar12 + 0x1b4 + iVar6 * 4);
    *(float *)(*(int *)(unaff_EBP + -0x1dc) + iVar6 * 4) = fVar2;
    if (fVar2 == 0.0) {
      iVar12 = *(int *)(unaff_EBP + 8);
      *(undefined4 *)(*(int *)(unaff_EBP + -0x1d4) + iVar6 * 4) = 0;
      *(undefined4 *)(unaff_EBP + -0x230) = 3;
      iVar9 = *(int *)(iVar12 + 0x14) + iVar6;
      iVar12 = iVar9;
      if (iVar9 < 0) {
        iVar12 = iVar9 + 3;
      }
      iVar10 = *(int *)(unaff_EBP + -0x230);
      *(undefined4 *)
       (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar10 * 0x10)
           = 0;
      if (*(int *)(*(int *)(unaff_EBP + 0xc) + 0xb8) != -1) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        *(undefined4 *)
         ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
         iVar9 * 4 + iVar10 * 0x10) = 0;
      }
      *(int *)(unaff_EBP + -0x230) = *(int *)(unaff_EBP + -0x230) + 1;
      iVar12 = *(int *)(unaff_EBP + -0x230);
      while (iVar12 < 6) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar10 = *(int *)(unaff_EBP + -0x230);
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar10 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar10 * 0x10) = 0;
        }
        iVar10 = *(int *)(unaff_EBP + -0x230) + 1;
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar10 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar10 * 0x10) = 0;
        }
        *(int *)(unaff_EBP + -0x230) = *(int *)(unaff_EBP + -0x230) + 2;
        iVar12 = *(int *)(unaff_EBP + -0x230);
      }
    }
    iVar6 = iVar6 + 1;
  } while (iVar6 < 6);
  MdtBclEndConstraint(*(MdtKeaConstraints **)(unaff_EBP + 8),6);
  return;
}


/* ==== FUN_00020030 ==== */

void FUN_00020030(void)

{
  undefined4 uVar1;
  float fVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  int in_ECX;
  int iVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  int unaff_EBP;
  int iVar11;
  undefined4 unaff_ESI;
  int iVar12;
  longdouble in_ST0;
  longdouble in_ST1;
  longdouble in_ST2;
  longdouble in_ST3;
  
  *(char *)(in_ECX + 0xf1) = *(char *)(in_ECX + 0xf1) + -0x80;
  *(undefined4 *)(unaff_EBP + -0x98) = unaff_ESI;
  *(float *)(unaff_EBP + -0x1ac) = (float)in_ST0;
  *(int *)(unaff_EBP + -0x88) = in_ECX;
  *(undefined4 *)(unaff_EBP + -0x70) = unaff_ESI;
  *(float *)(unaff_EBP + -0x25c) = (float)in_ST0;
  *(int *)(unaff_EBP + -0x8c) = in_ECX;
  uVar10 = *(uint *)(unaff_EBP + -0x25c) ^ 0x80000000;
  *(uint *)(unaff_EBP + -0x78) = uVar10;
  *(uint *)(unaff_EBP + -0x84) = uVar10;
  *(float *)(unaff_EBP + -0x6c) = (float)in_ST1;
  *(float *)(unaff_EBP + -0x80) = (float)in_ST1;
  *(float *)(unaff_EBP + -0x94) = (float)in_ST1;
  *(float *)(unaff_EBP + -0x7c) = (float)in_ST2;
  *(float *)(unaff_EBP + -0x74) = (float)in_ST3;
  *(float *)(unaff_EBP + -0x90) = (float)in_ST0;
  iVar12 = 1;
  MyMultiplyT2(3,4,3,(MeReal *)(unaff_EBP + -0x98),(MeReal *)(unaff_EBP + -0x68),
               (MeReal *)(unaff_EBP + -200));
  iVar6 = 3;
  *(float *)(unaff_EBP + -200) = *(float *)(unaff_EBP + -200) * 0.5;
  *(float *)(unaff_EBP + -0xc4) = *(float *)(unaff_EBP + -0xc4) * 0.5;
  *(float *)(unaff_EBP + -0xc0) = *(float *)(unaff_EBP + -0xc0) * 0.5;
  do {
    iVar9 = iVar6 + 3;
    iVar12 = iVar12 + 2;
    *(float *)(unaff_EBP + -200 + iVar6 * 4) = *(float *)(unaff_EBP + -200 + iVar6 * 4) * 0.5;
    *(float *)(unaff_EBP + -200 + (iVar6 + 1) * 4) =
         *(float *)(unaff_EBP + -200 + (iVar6 + 1) * 4) * 0.5;
    iVar11 = iVar6 + 4;
    *(float *)(unaff_EBP + -200 + (iVar6 + 2) * 4) =
         *(float *)(unaff_EBP + -200 + (iVar6 + 2) * 4) * 0.5;
    iVar7 = iVar6 + 5;
    iVar6 = iVar6 + 6;
    *(float *)(unaff_EBP + -200 + iVar9 * 4) = *(float *)(unaff_EBP + -200 + iVar9 * 4) * 0.5;
    *(float *)(unaff_EBP + -200 + iVar11 * 4) = *(float *)(unaff_EBP + -200 + iVar11 * 4) * 0.5;
    *(float *)(unaff_EBP + -200 + iVar7 * 4) = *(float *)(unaff_EBP + -200 + iVar7 * 4) * 0.5;
  } while (iVar12 < 3);
  iVar6 = 0;
  *(undefined4 *)(unaff_EBP + -0x240) = 0;
  do {
    iVar11 = *(int *)(unaff_EBP + -0x240);
    iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
    iVar12 = iVar7 + 3;
    iVar9 = iVar12;
    if (iVar12 < 0) {
      iVar9 = iVar7 + 6;
    }
    *(undefined4 *)
     (iVar12 * 4 + (iVar9 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x30) =
         *(undefined4 *)(unaff_EBP + -200 + iVar11 * 4);
    iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
    iVar12 = iVar7 + 3;
    iVar9 = iVar12;
    if (iVar12 < 0) {
      iVar9 = iVar7 + 6;
    }
    *(undefined4 *)
     (iVar12 * 4 + (iVar9 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x40) =
         *(undefined4 *)(unaff_EBP + -200 + (iVar11 + 1) * 4);
    iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
    iVar12 = iVar7 + 3;
    iVar9 = iVar12;
    if (iVar12 < 0) {
      iVar9 = iVar7 + 6;
    }
    iVar6 = iVar6 + 1;
    *(undefined4 *)
     (iVar12 * 4 + (iVar9 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x50) =
         *(undefined4 *)(unaff_EBP + -200 + (iVar11 + 2) * 4);
    *(int *)(unaff_EBP + -0x240) = *(int *)(unaff_EBP + -0x240) + 3;
  } while (iVar6 < 3);
  if (*(int *)(*(int *)(unaff_EBP + 0xc) + 0xb8) != -1) {
    iVar6 = 0;
    *(undefined4 *)(unaff_EBP + -0x23c) = 0;
    do {
      iVar11 = *(int *)(unaff_EBP + -0x23c);
      iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
      iVar12 = iVar7 + 3;
      iVar9 = iVar12;
      if (iVar12 < 0) {
        iVar9 = iVar7 + 6;
      }
      *(int *)(unaff_EBP + -0x260) = iVar9 >> 2;
      iVar7 = *(int *)(unaff_EBP + 8);
      *(uint *)((iVar9 >> 2) * 0xc0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x90 +
               (iVar12 + *(int *)(unaff_EBP + -0x260) * -4) * 4) =
           *(uint *)(unaff_EBP + -200 + iVar11 * 4) ^ 0x80000000;
      iVar7 = *(int *)(iVar7 + 0x14) + iVar6;
      iVar12 = iVar7 + 3;
      iVar9 = iVar12;
      if (iVar12 < 0) {
        iVar9 = iVar7 + 6;
      }
      *(int *)(unaff_EBP + -0x260) = iVar9 >> 2;
      iVar7 = *(int *)(unaff_EBP + 8);
      *(uint *)((iVar9 >> 2) * 0xc0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x90 +
               (iVar12 + *(int *)(unaff_EBP + -0x260) * -4 + 4) * 4) =
           *(uint *)(unaff_EBP + -200 + (iVar11 + 1) * 4) ^ 0x80000000;
      iVar7 = *(int *)(iVar7 + 0x14) + iVar6;
      iVar12 = iVar7 + 3;
      iVar9 = iVar12;
      if (iVar12 < 0) {
        iVar9 = iVar7 + 6;
      }
      iVar6 = iVar6 + 1;
      *(int *)(unaff_EBP + -0x260) = iVar9 >> 2;
      *(uint *)((iVar9 >> 2) * 0xc0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x90 +
               (iVar12 + *(int *)(unaff_EBP + -0x260) * -4 + 8) * 4) =
           *(uint *)(unaff_EBP + -200 + (iVar11 + 2) * 4) ^ 0x80000000;
      *(int *)(unaff_EBP + -0x23c) = *(int *)(unaff_EBP + -0x23c) + 3;
    } while (iVar6 < 3);
  }
  iVar6 = *(int *)(unaff_EBP + -0x238);
  uVar1 = *(undefined4 *)(iVar6 + 4);
  *(undefined4 *)(unaff_EBP + -0x25c) = uVar1;
  uVar8 = *(uint *)(unaff_EBP + -0x25c) ^ 0x80000000;
  *(uint *)(unaff_EBP + -0xf8) = uVar8;
  uVar10 = *(uint *)(iVar6 + 8);
  uVar5 = uVar10 ^ 0x80000000;
  *(uint *)(unaff_EBP + -0xe8) = uVar5;
  uVar3 = *(undefined4 *)(iVar6 + 0xc);
  *(undefined4 *)(unaff_EBP + -0x25c) = uVar3;
  *(float *)(unaff_EBP + -0xd8) = -*(float *)(unaff_EBP + -0x25c);
  uVar4 = *(undefined4 *)(*(int *)(unaff_EBP + 0xc) + 0x1a4);
  *(uint *)(unaff_EBP + -0xd4) = uVar10;
  *(undefined4 *)(unaff_EBP + -0xcc) = uVar4;
  *(undefined4 *)(unaff_EBP + -0xe0) = uVar4;
  *(undefined4 *)(unaff_EBP + -0xf4) = uVar4;
  *(undefined4 *)(unaff_EBP + -0xf0) = uVar3;
  *(uint *)(unaff_EBP + -0xd0) = uVar8;
  *(uint *)(unaff_EBP + -0xec) = uVar5;
  *(float *)(unaff_EBP + -0xe4) = -*(float *)(unaff_EBP + -0x25c);
  *(undefined4 *)(unaff_EBP + -0xdc) = uVar1;
  MyMultiplyT2(3,4,1,(MeReal *)(unaff_EBP + -0xf8),(MeReal *)(unaff_EBP + -0x168),
               (MeReal *)(unaff_EBP + -0x1c8));
  iVar6 = *(int *)(unaff_EBP + -0x1d4);
  *(undefined4 *)(iVar6 + 0xc) = *(undefined4 *)(unaff_EBP + -0x1c8);
  *(undefined4 *)(iVar6 + 0x10) = *(undefined4 *)(unaff_EBP + -0x1c4);
  *(undefined4 *)(iVar6 + 0x14) = *(undefined4 *)(unaff_EBP + -0x1c0);
  *(uint *)(unaff_EBP + -0x11c) = *(uint *)(unaff_EBP + -0x160);
  uVar10 = *(uint *)(unaff_EBP + -0x160) ^ 0x80000000;
  uVar1 = *(undefined4 *)(unaff_EBP + -0x168);
  *(undefined4 *)(unaff_EBP + -0x25c) = *(undefined4 *)(unaff_EBP + -0x164);
  *(uint *)(unaff_EBP + -0x118) = uVar10;
  *(uint *)(unaff_EBP + -0x104) = uVar10;
  *(undefined4 *)(unaff_EBP + -0xfc) = uVar1;
  *(undefined4 *)(unaff_EBP + -0x110) = uVar1;
  *(undefined4 *)(unaff_EBP + -0x124) = uVar1;
  uVar10 = *(uint *)(unaff_EBP + -0x15c) ^ 0x80000000;
  *(uint *)(unaff_EBP + -0x114) = *(uint *)(unaff_EBP + -0x15c);
  *(uint *)(unaff_EBP + -0x108) = uVar10;
  *(float *)(unaff_EBP + -0x128) = -*(float *)(unaff_EBP + -0x25c);
  *(float *)(unaff_EBP + -0x10c) = -*(float *)(unaff_EBP + -0x25c);
  *(undefined4 *)(unaff_EBP + -0x100) = *(undefined4 *)(unaff_EBP + -0x164);
  *(uint *)(unaff_EBP + -0x120) = uVar10;
  MyMultiplyT2(3,4,3,(MeReal *)(unaff_EBP + -0xf8),(MeReal *)(unaff_EBP + -0x128),
               (MeReal *)(unaff_EBP + -0x158));
  iVar6 = *(int *)(unaff_EBP + -0x1d0);
  MyMultiplyT2(3,3,1,(MeReal *)(unaff_EBP + -0x158),(MeReal *)(*(int *)(unaff_EBP + 0xc) + 0x1b4),
               (MeReal *)(iVar6 + 0xc));
  *(float *)(*(int *)(unaff_EBP + -0x1d0) + 0xc) =
       *(float *)(*(int *)(unaff_EBP + -0x1d0) + 0xc) * 0.5;
  *(float *)(iVar6 + 0x10) = *(float *)(iVar6 + 0x10) * 0.5;
  *(float *)(iVar6 + 0x14) = *(float *)(iVar6 + 0x14) * 0.5;
  iVar6 = 0;
  do {
    iVar12 = *(int *)(unaff_EBP + 0xc);
    *(uint *)(*(int *)(unaff_EBP + -0x1d8) + iVar6 * 4) =
         *(uint *)(iVar12 + 0x1cc + iVar6 * 4) ^ 0x80000000;
    fVar2 = *(float *)(iVar12 + 0x1cc + iVar6 * 4);
    *(float *)(*(int *)(unaff_EBP + -0x1dc) + iVar6 * 4) = fVar2;
    if (fVar2 == 0.0) {
      iVar12 = *(int *)(unaff_EBP + 8);
      *(undefined4 *)(*(int *)(unaff_EBP + -0x1d4) + iVar6 * 4) = 0;
      *(undefined4 *)(unaff_EBP + -0x22c) = 0;
      iVar9 = *(int *)(iVar12 + 0x14) + iVar6;
      iVar12 = iVar9;
      if (iVar9 < 0) {
        iVar12 = iVar9 + 3;
      }
      iVar11 = *(int *)(unaff_EBP + -0x22c);
      iVar7 = *(int *)(unaff_EBP + 0xc);
      *(undefined4 *)
       (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10)
           = 0;
      if (*(int *)(iVar7 + 0xb8) != -1) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        *(undefined4 *)
         ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
         iVar9 * 4 + iVar11 * 0x10) = 0;
      }
      *(int *)(unaff_EBP + -0x22c) = *(int *)(unaff_EBP + -0x22c) + 1;
      iVar12 = *(int *)(unaff_EBP + -0x22c);
      while (iVar12 < 3) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar11 = *(int *)(unaff_EBP + -0x22c);
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar11 * 0x10) = 0;
        }
        iVar11 = *(int *)(unaff_EBP + -0x22c) + 1;
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar11 * 0x10) = 0;
        }
        *(int *)(unaff_EBP + -0x22c) = *(int *)(unaff_EBP + -0x22c) + 2;
        iVar12 = *(int *)(unaff_EBP + -0x22c);
      }
    }
    iVar6 = iVar6 + 1;
  } while (iVar6 < 3);
  iVar6 = 3;
  do {
    iVar12 = *(int *)(unaff_EBP + 0xc);
    *(uint *)(*(int *)(unaff_EBP + -0x1d8) + iVar6 * 4) =
         *(uint *)(iVar12 + 0x1b4 + iVar6 * 4) ^ 0x80000000;
    fVar2 = *(float *)(iVar12 + 0x1b4 + iVar6 * 4);
    *(float *)(*(int *)(unaff_EBP + -0x1dc) + iVar6 * 4) = fVar2;
    if (fVar2 == 0.0) {
      iVar12 = *(int *)(unaff_EBP + 8);
      *(undefined4 *)(*(int *)(unaff_EBP + -0x1d4) + iVar6 * 4) = 0;
      *(undefined4 *)(unaff_EBP + -0x230) = 3;
      iVar9 = *(int *)(iVar12 + 0x14) + iVar6;
      iVar12 = iVar9;
      if (iVar9 < 0) {
        iVar12 = iVar9 + 3;
      }
      iVar11 = *(int *)(unaff_EBP + -0x230);
      *(undefined4 *)
       (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10)
           = 0;
      if (*(int *)(*(int *)(unaff_EBP + 0xc) + 0xb8) != -1) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        *(undefined4 *)
         ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
         iVar9 * 4 + iVar11 * 0x10) = 0;
      }
      *(int *)(unaff_EBP + -0x230) = *(int *)(unaff_EBP + -0x230) + 1;
      iVar12 = *(int *)(unaff_EBP + -0x230);
      while (iVar12 < 6) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar11 = *(int *)(unaff_EBP + -0x230);
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar11 * 0x10) = 0;
        }
        iVar11 = *(int *)(unaff_EBP + -0x230) + 1;
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar11 * 0x10) = 0;
        }
        *(int *)(unaff_EBP + -0x230) = *(int *)(unaff_EBP + -0x230) + 2;
        iVar12 = *(int *)(unaff_EBP + -0x230);
      }
    }
    iVar6 = iVar6 + 1;
  } while (iVar6 < 6);
  MdtBclEndConstraint(*(MdtKeaConstraints **)(unaff_EBP + 8),6);
  return;
}


/* ==== FUN_000200f0 ==== */

/* WARNING: Control flow encountered bad instruction data */

void FUN_000200f0(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== FUN_00020120 ==== */

/* WARNING: Control flow encountered bad instruction data */

void FUN_00020120(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== FUN_000201a0 ==== */

void FUN_000201a0(void)

{
  float fVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int in_EDX;
  uint uVar8;
  int iVar9;
  undefined4 unaff_EBX;
  int unaff_EBP;
  uint uVar10;
  int iVar11;
  int iVar12;
  longdouble in_ST0;
  
  uVar10 = *(uint *)(in_EDX + 8);
  uVar5 = uVar10 ^ 0x80000000;
  *(uint *)(unaff_EBP + -0x88) = uVar5;
  uVar3 = *(undefined4 *)(in_EDX + 0xc);
  *(undefined4 *)(unaff_EBP + -0x25c) = uVar3;
  *(float *)(unaff_EBP + -0x78) = -*(float *)(unaff_EBP + -0x25c);
  uVar4 = *(undefined4 *)(*(int *)(unaff_EBP + 0xc) + 0x1a4);
  *(undefined4 *)(unaff_EBP + -0x70) = unaff_EBX;
  *(float *)(unaff_EBP + -0x7c) = (float)in_ST0;
  *(float *)(unaff_EBP + -0x84) = -*(float *)(unaff_EBP + -0x25c);
  *(undefined4 *)(unaff_EBP + -0x6c) = uVar4;
  *(undefined4 *)(unaff_EBP + -0x80) = uVar4;
  *(undefined4 *)(unaff_EBP + -0x94) = uVar4;
  *(uint *)(unaff_EBP + -0x8c) = uVar5;
  *(uint *)(unaff_EBP + -0x74) = uVar10;
  *(undefined4 *)(unaff_EBP + -0x90) = uVar3;
  iVar12 = 1;
  MyMultiplyT2(3,4,3,(MeReal *)(unaff_EBP + -0x98),(MeReal *)(unaff_EBP + -0x68),
               (MeReal *)(unaff_EBP + -200));
  iVar6 = 3;
  *(float *)(unaff_EBP + -200) = *(float *)(unaff_EBP + -200) * 0.5;
  *(float *)(unaff_EBP + -0xc4) = *(float *)(unaff_EBP + -0xc4) * 0.5;
  *(float *)(unaff_EBP + -0xc0) = *(float *)(unaff_EBP + -0xc0) * 0.5;
  do {
    iVar9 = iVar6 + 3;
    iVar12 = iVar12 + 2;
    *(float *)(unaff_EBP + -200 + iVar6 * 4) = *(float *)(unaff_EBP + -200 + iVar6 * 4) * 0.5;
    *(float *)(unaff_EBP + -200 + (iVar6 + 1) * 4) =
         *(float *)(unaff_EBP + -200 + (iVar6 + 1) * 4) * 0.5;
    iVar11 = iVar6 + 4;
    *(float *)(unaff_EBP + -200 + (iVar6 + 2) * 4) =
         *(float *)(unaff_EBP + -200 + (iVar6 + 2) * 4) * 0.5;
    iVar7 = iVar6 + 5;
    iVar6 = iVar6 + 6;
    *(float *)(unaff_EBP + -200 + iVar9 * 4) = *(float *)(unaff_EBP + -200 + iVar9 * 4) * 0.5;
    *(float *)(unaff_EBP + -200 + iVar11 * 4) = *(float *)(unaff_EBP + -200 + iVar11 * 4) * 0.5;
    *(float *)(unaff_EBP + -200 + iVar7 * 4) = *(float *)(unaff_EBP + -200 + iVar7 * 4) * 0.5;
  } while (iVar12 < 3);
  iVar6 = 0;
  *(undefined4 *)(unaff_EBP + -0x240) = 0;
  do {
    iVar11 = *(int *)(unaff_EBP + -0x240);
    iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
    iVar12 = iVar7 + 3;
    iVar9 = iVar12;
    if (iVar12 < 0) {
      iVar9 = iVar7 + 6;
    }
    *(undefined4 *)
     (iVar12 * 4 + (iVar9 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x30) =
         *(undefined4 *)(unaff_EBP + -200 + iVar11 * 4);
    iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
    iVar12 = iVar7 + 3;
    iVar9 = iVar12;
    if (iVar12 < 0) {
      iVar9 = iVar7 + 6;
    }
    *(undefined4 *)
     (iVar12 * 4 + (iVar9 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x40) =
         *(undefined4 *)(unaff_EBP + -200 + (iVar11 + 1) * 4);
    iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
    iVar12 = iVar7 + 3;
    iVar9 = iVar12;
    if (iVar12 < 0) {
      iVar9 = iVar7 + 6;
    }
    iVar6 = iVar6 + 1;
    *(undefined4 *)
     (iVar12 * 4 + (iVar9 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x50) =
         *(undefined4 *)(unaff_EBP + -200 + (iVar11 + 2) * 4);
    *(int *)(unaff_EBP + -0x240) = *(int *)(unaff_EBP + -0x240) + 3;
  } while (iVar6 < 3);
  if (*(int *)(*(int *)(unaff_EBP + 0xc) + 0xb8) != -1) {
    iVar6 = 0;
    *(undefined4 *)(unaff_EBP + -0x23c) = 0;
    do {
      iVar11 = *(int *)(unaff_EBP + -0x23c);
      iVar7 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
      iVar12 = iVar7 + 3;
      iVar9 = iVar12;
      if (iVar12 < 0) {
        iVar9 = iVar7 + 6;
      }
      *(int *)(unaff_EBP + -0x260) = iVar9 >> 2;
      iVar7 = *(int *)(unaff_EBP + 8);
      *(uint *)((iVar9 >> 2) * 0xc0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x90 +
               (iVar12 + *(int *)(unaff_EBP + -0x260) * -4) * 4) =
           *(uint *)(unaff_EBP + -200 + iVar11 * 4) ^ 0x80000000;
      iVar7 = *(int *)(iVar7 + 0x14) + iVar6;
      iVar12 = iVar7 + 3;
      iVar9 = iVar12;
      if (iVar12 < 0) {
        iVar9 = iVar7 + 6;
      }
      *(int *)(unaff_EBP + -0x260) = iVar9 >> 2;
      iVar7 = *(int *)(unaff_EBP + 8);
      *(uint *)((iVar9 >> 2) * 0xc0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x90 +
               (iVar12 + *(int *)(unaff_EBP + -0x260) * -4 + 4) * 4) =
           *(uint *)(unaff_EBP + -200 + (iVar11 + 1) * 4) ^ 0x80000000;
      iVar7 = *(int *)(iVar7 + 0x14) + iVar6;
      iVar12 = iVar7 + 3;
      iVar9 = iVar12;
      if (iVar12 < 0) {
        iVar9 = iVar7 + 6;
      }
      iVar6 = iVar6 + 1;
      *(int *)(unaff_EBP + -0x260) = iVar9 >> 2;
      *(uint *)((iVar9 >> 2) * 0xc0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x90 +
               (iVar12 + *(int *)(unaff_EBP + -0x260) * -4 + 8) * 4) =
           *(uint *)(unaff_EBP + -200 + (iVar11 + 2) * 4) ^ 0x80000000;
      *(int *)(unaff_EBP + -0x23c) = *(int *)(unaff_EBP + -0x23c) + 3;
    } while (iVar6 < 3);
  }
  iVar6 = *(int *)(unaff_EBP + -0x238);
  uVar3 = *(undefined4 *)(iVar6 + 4);
  *(undefined4 *)(unaff_EBP + -0x25c) = uVar3;
  uVar8 = *(uint *)(unaff_EBP + -0x25c) ^ 0x80000000;
  *(uint *)(unaff_EBP + -0xf8) = uVar8;
  uVar10 = *(uint *)(iVar6 + 8);
  uVar5 = uVar10 ^ 0x80000000;
  *(uint *)(unaff_EBP + -0xe8) = uVar5;
  uVar4 = *(undefined4 *)(iVar6 + 0xc);
  *(undefined4 *)(unaff_EBP + -0x25c) = uVar4;
  *(float *)(unaff_EBP + -0xd8) = -*(float *)(unaff_EBP + -0x25c);
  uVar2 = *(undefined4 *)(*(int *)(unaff_EBP + 0xc) + 0x1a4);
  *(uint *)(unaff_EBP + -0xd4) = uVar10;
  *(undefined4 *)(unaff_EBP + -0xcc) = uVar2;
  *(undefined4 *)(unaff_EBP + -0xe0) = uVar2;
  *(undefined4 *)(unaff_EBP + -0xf4) = uVar2;
  *(undefined4 *)(unaff_EBP + -0xf0) = uVar4;
  *(uint *)(unaff_EBP + -0xd0) = uVar8;
  *(uint *)(unaff_EBP + -0xec) = uVar5;
  *(float *)(unaff_EBP + -0xe4) = -*(float *)(unaff_EBP + -0x25c);
  *(undefined4 *)(unaff_EBP + -0xdc) = uVar3;
  MyMultiplyT2(3,4,1,(MeReal *)(unaff_EBP + -0xf8),(MeReal *)(unaff_EBP + -0x168),
               (MeReal *)(unaff_EBP + -0x1c8));
  iVar6 = *(int *)(unaff_EBP + -0x1d4);
  *(undefined4 *)(iVar6 + 0xc) = *(undefined4 *)(unaff_EBP + -0x1c8);
  *(undefined4 *)(iVar6 + 0x10) = *(undefined4 *)(unaff_EBP + -0x1c4);
  *(undefined4 *)(iVar6 + 0x14) = *(undefined4 *)(unaff_EBP + -0x1c0);
  *(uint *)(unaff_EBP + -0x11c) = *(uint *)(unaff_EBP + -0x160);
  uVar10 = *(uint *)(unaff_EBP + -0x160) ^ 0x80000000;
  uVar3 = *(undefined4 *)(unaff_EBP + -0x168);
  *(undefined4 *)(unaff_EBP + -0x25c) = *(undefined4 *)(unaff_EBP + -0x164);
  *(uint *)(unaff_EBP + -0x118) = uVar10;
  *(uint *)(unaff_EBP + -0x104) = uVar10;
  *(undefined4 *)(unaff_EBP + -0xfc) = uVar3;
  *(undefined4 *)(unaff_EBP + -0x110) = uVar3;
  *(undefined4 *)(unaff_EBP + -0x124) = uVar3;
  uVar10 = *(uint *)(unaff_EBP + -0x15c) ^ 0x80000000;
  *(uint *)(unaff_EBP + -0x114) = *(uint *)(unaff_EBP + -0x15c);
  *(uint *)(unaff_EBP + -0x108) = uVar10;
  *(float *)(unaff_EBP + -0x128) = -*(float *)(unaff_EBP + -0x25c);
  *(float *)(unaff_EBP + -0x10c) = -*(float *)(unaff_EBP + -0x25c);
  *(undefined4 *)(unaff_EBP + -0x100) = *(undefined4 *)(unaff_EBP + -0x164);
  *(uint *)(unaff_EBP + -0x120) = uVar10;
  MyMultiplyT2(3,4,3,(MeReal *)(unaff_EBP + -0xf8),(MeReal *)(unaff_EBP + -0x128),
               (MeReal *)(unaff_EBP + -0x158));
  iVar6 = *(int *)(unaff_EBP + -0x1d0);
  MyMultiplyT2(3,3,1,(MeReal *)(unaff_EBP + -0x158),(MeReal *)(*(int *)(unaff_EBP + 0xc) + 0x1b4),
               (MeReal *)(iVar6 + 0xc));
  *(float *)(*(int *)(unaff_EBP + -0x1d0) + 0xc) =
       *(float *)(*(int *)(unaff_EBP + -0x1d0) + 0xc) * 0.5;
  *(float *)(iVar6 + 0x10) = *(float *)(iVar6 + 0x10) * 0.5;
  *(float *)(iVar6 + 0x14) = *(float *)(iVar6 + 0x14) * 0.5;
  iVar6 = 0;
  do {
    iVar12 = *(int *)(unaff_EBP + 0xc);
    *(uint *)(*(int *)(unaff_EBP + -0x1d8) + iVar6 * 4) =
         *(uint *)(iVar12 + 0x1cc + iVar6 * 4) ^ 0x80000000;
    fVar1 = *(float *)(iVar12 + 0x1cc + iVar6 * 4);
    *(float *)(*(int *)(unaff_EBP + -0x1dc) + iVar6 * 4) = fVar1;
    if (fVar1 == 0.0) {
      iVar12 = *(int *)(unaff_EBP + 8);
      *(undefined4 *)(*(int *)(unaff_EBP + -0x1d4) + iVar6 * 4) = 0;
      *(undefined4 *)(unaff_EBP + -0x22c) = 0;
      iVar9 = *(int *)(iVar12 + 0x14) + iVar6;
      iVar12 = iVar9;
      if (iVar9 < 0) {
        iVar12 = iVar9 + 3;
      }
      iVar11 = *(int *)(unaff_EBP + -0x22c);
      iVar7 = *(int *)(unaff_EBP + 0xc);
      *(undefined4 *)
       (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10)
           = 0;
      if (*(int *)(iVar7 + 0xb8) != -1) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        *(undefined4 *)
         ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
         iVar9 * 4 + iVar11 * 0x10) = 0;
      }
      *(int *)(unaff_EBP + -0x22c) = *(int *)(unaff_EBP + -0x22c) + 1;
      iVar12 = *(int *)(unaff_EBP + -0x22c);
      while (iVar12 < 3) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar11 = *(int *)(unaff_EBP + -0x22c);
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar11 * 0x10) = 0;
        }
        iVar11 = *(int *)(unaff_EBP + -0x22c) + 1;
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar11 * 0x10) = 0;
        }
        *(int *)(unaff_EBP + -0x22c) = *(int *)(unaff_EBP + -0x22c) + 2;
        iVar12 = *(int *)(unaff_EBP + -0x22c);
      }
    }
    iVar6 = iVar6 + 1;
  } while (iVar6 < 3);
  iVar6 = 3;
  do {
    iVar12 = *(int *)(unaff_EBP + 0xc);
    *(uint *)(*(int *)(unaff_EBP + -0x1d8) + iVar6 * 4) =
         *(uint *)(iVar12 + 0x1b4 + iVar6 * 4) ^ 0x80000000;
    fVar1 = *(float *)(iVar12 + 0x1b4 + iVar6 * 4);
    *(float *)(*(int *)(unaff_EBP + -0x1dc) + iVar6 * 4) = fVar1;
    if (fVar1 == 0.0) {
      iVar12 = *(int *)(unaff_EBP + 8);
      *(undefined4 *)(*(int *)(unaff_EBP + -0x1d4) + iVar6 * 4) = 0;
      *(undefined4 *)(unaff_EBP + -0x230) = 3;
      iVar9 = *(int *)(iVar12 + 0x14) + iVar6;
      iVar12 = iVar9;
      if (iVar9 < 0) {
        iVar12 = iVar9 + 3;
      }
      iVar11 = *(int *)(unaff_EBP + -0x230);
      *(undefined4 *)
       (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10)
           = 0;
      if (*(int *)(*(int *)(unaff_EBP + 0xc) + 0xb8) != -1) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        *(undefined4 *)
         ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
         iVar9 * 4 + iVar11 * 0x10) = 0;
      }
      *(int *)(unaff_EBP + -0x230) = *(int *)(unaff_EBP + -0x230) + 1;
      iVar12 = *(int *)(unaff_EBP + -0x230);
      while (iVar12 < 6) {
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar11 = *(int *)(unaff_EBP + -0x230);
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar11 * 0x10) = 0;
        }
        iVar11 = *(int *)(unaff_EBP + -0x230) + 1;
        iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
        iVar12 = iVar9;
        if (iVar9 < 0) {
          iVar12 = iVar9 + 3;
        }
        iVar7 = *(int *)(unaff_EBP + 0xc);
        *(undefined4 *)
         (*(int *)(*(int *)(unaff_EBP + 8) + 0x2c) +
         iVar9 * 4 + (iVar12 >> 2) * 0xb0 + iVar11 * 0x10) = 0;
        if (*(int *)(iVar7 + 0xb8) != -1) {
          iVar9 = *(int *)(*(int *)(unaff_EBP + 8) + 0x14) + iVar6;
          iVar12 = iVar9;
          if (iVar9 < 0) {
            iVar12 = iVar9 + 3;
          }
          *(undefined4 *)
           ((iVar12 >> 2) * 0xb0 + *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + 0x60 +
           iVar9 * 4 + iVar11 * 0x10) = 0;
        }
        *(int *)(unaff_EBP + -0x230) = *(int *)(unaff_EBP + -0x230) + 2;
        iVar12 = *(int *)(unaff_EBP + -0x230);
      }
    }
    iVar6 = iVar6 + 1;
  } while (iVar6 < 6);
  MdtBclEndConstraint(*(MdtKeaConstraints **)(unaff_EBP + 8),6);
  return;
}


/* ==== MeVector3Normalize ==== */

MeReal MeVector3Normalize(MeReal *v)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float local_8;
  
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
  fVar1 = *v;
  fVar2 = v[1];
  fVar3 = v[2];
  local_8 = fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1;
  if (local_8 <= 0.0) {
    *v = 1.0;
    v[1] = 0.0;
    v[2] = 0.0;
    local_8 = 0.0;
  }
  else {
    local_8 = SQRT(local_8);
    fVar4 = 1.0 / local_8;
    *v = fVar1 * fVar4;
    v[1] = fVar2 * fVar4;
    v[2] = fVar4 * fVar3;
  }
  return local_8;
}


/* ==== MeMatrixMultiply ==== */

/* WARNING: Unknown calling convention */

void MeMatrixMultiply(MeReal *A,int p,int q,int r,MeReal *B,MeReal *C)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float *pfVar8;
  float *pfVar9;
  float *pfVar10;
  float *pfVar11;
  int iVar12;
  int iVar13;
  uint uVar14;
  int local_20;
  int j;
  int i;
  
                    /* Unresolved local var: int k@[DW_OP_reg3(EBX)] */
  j = 0;
  if (0 < r) {
    fVar7 = 0.0;
    local_20 = 0;
    do {
      i = 0;
      if (0 < p) {
        do {
                    /* Unresolved local var: MeReal * c@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal * b@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeReal sum@[DW_OP_reg12(ST1)] */
          pfVar10 = B + i;
          pfVar11 = (float *)((int)C + local_20);
          fVar6 = fVar7;
          if (0 < q) {
            uVar14 = -q & 3;
            iVar13 = q;
            if (q + -1 < 1) {
LAB_00020452:
              fVar4 = *pfVar11;
              pfVar11 = pfVar11 + 1;
              fVar5 = *pfVar10;
              pfVar10 = pfVar10 + p;
              fVar6 = fVar4 * fVar5 + fVar6;
              iVar12 = iVar13 + -1;
              if (iVar13 + -1 < 1) goto LAB_0002048f;
            }
            else {
              iVar12 = q;
              if (uVar14 != 0) {
                if (uVar14 < 3) {
                  fVar6 = fVar7;
                  if (uVar14 < 2) {
                    fVar6 = *pfVar11;
                    pfVar11 = pfVar11 + 1;
                    fVar6 = fVar6 * *pfVar10;
                    pfVar10 = pfVar10 + p;
                    iVar13 = q + -1;
                  }
                  fVar4 = *pfVar11;
                  iVar13 = iVar13 + -1;
                  pfVar11 = pfVar11 + 1;
                  fVar5 = *pfVar10;
                  pfVar10 = pfVar10 + p;
                  fVar6 = fVar4 * fVar5 + fVar6;
                }
                goto LAB_00020452;
              }
            }
            do {
              fVar4 = *pfVar11;
              iVar12 = iVar12 + -4;
              fVar5 = *pfVar10;
              pfVar8 = pfVar10 + p;
              pfVar1 = pfVar11 + 1;
              pfVar9 = pfVar8 + p;
              pfVar2 = pfVar11 + 2;
              pfVar3 = pfVar11 + 3;
              pfVar11 = pfVar11 + 4;
              pfVar10 = pfVar9 + p + p;
              fVar6 = *pfVar3 * pfVar9[p] +
                      *pfVar2 * *pfVar9 + *pfVar1 * *pfVar8 + fVar4 * fVar5 + fVar6;
            } while (0 < iVar12);
          }
LAB_0002048f:
          *A = fVar6;
          A = A + 1;
          i = i + 1;
        } while (i < p);
      }
      j = j + 1;
      local_20 = local_20 + q * 4;
    } while (j < r);
  }
  return;
}


/* ==== MeMatrix4MultiplyMatrix ==== */

void MeMatrix4MultiplyMatrix(MeVector4 *A,MeReal (*B) [4],MeReal (*C) [4])

{
  MeReal *pMVar1;
  MeReal *pMVar2;
  MeReal *pMVar3;
  MeReal *pMVar4;
  int iVar5;
  
                    /* Unresolved local var: MeReal * a@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int j@[???] */
  iVar5 = 3;
  do {
    (*A)[0] = C[3][0] * (*B)[3] + C[2][0] * (*B)[2] + C[1][0] * (*B)[1] + (*C)[0] * (*B)[0];
    (*A)[1] = C[3][1] * (*B)[3] + C[2][1] * (*B)[2] + C[1][1] * (*B)[1] + (*C)[1] * (*B)[0];
    (*A)[2] = C[3][2] * (*B)[3] + C[2][2] * (*B)[2] + C[1][2] * (*B)[1] + (*C)[2] * (*B)[0];
    pMVar1 = *B;
    pMVar2 = *B;
    pMVar3 = *B;
    pMVar4 = *B;
    B = B + 1;
    (*A)[3] = C[3][3] * pMVar4[3] + C[2][3] * pMVar3[2] + C[1][3] * pMVar2[1] + (*C)[3] * *pMVar1;
    A = A + 1;
    iVar5 = iVar5 + -1;
  } while (-1 < iVar5);
  return;
}


/* ==== MyMultiplyT2 ==== */

/* WARNING: Unknown calling convention */

void MyMultiplyT2(int p,int q,int r,MeReal *A,MeReal *B,MeReal *C)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float *pfVar4;
  float *pfVar5;
  float *pfVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  uint uVar11;
  float *pfVar12;
  float *pfVar13;
  int iVar14;
  float *pfVar15;
  int iVar16;
  int local_24;
  int local_20;
  int local_1c;
  int i;
  
                    /* Unresolved local var: int j@[DW_OP_reg7(EDI)]
                       Unresolved local var: int k@[DW_OP_reg3(EBX)] */
  i = 0;
  if (0 < p) {
    fVar10 = 0.0;
    local_20 = 0;
    local_24 = 0;
    do {
      iVar16 = 0;
      if (0 < r) {
        pfVar15 = C + local_24;
        local_1c = 0;
        do {
                    /* Unresolved local var: MeReal e@[DW_OP_reg12(ST1)] */
          fVar9 = fVar10;
          if (0 < q) {
            pfVar12 = B + local_1c;
            pfVar13 = A + local_20;
            uVar11 = -q & 3;
            iVar14 = q;
            if (uVar11 != 0) {
              if (uVar11 < 3) {
                if (uVar11 < 2) {
                  fVar9 = *pfVar12;
                  iVar14 = q + -1;
                  pfVar12 = pfVar12 + 1;
                  fVar9 = fVar9 * *pfVar13;
                  pfVar13 = pfVar13 + 1;
                }
                fVar7 = *pfVar12;
                iVar14 = iVar14 + -1;
                pfVar12 = pfVar12 + 1;
                fVar8 = *pfVar13;
                pfVar13 = pfVar13 + 1;
                fVar9 = fVar7 * fVar8 + fVar9;
              }
              fVar7 = *pfVar12;
              pfVar12 = pfVar12 + 1;
              fVar8 = *pfVar13;
              pfVar13 = pfVar13 + 1;
              iVar14 = iVar14 + -1;
              fVar9 = fVar7 * fVar8 + fVar9;
              if (iVar14 == 0) goto LAB_0002068b;
            }
            do {
              fVar7 = *pfVar12;
              fVar8 = *pfVar13;
              pfVar1 = pfVar12 + 1;
              pfVar2 = pfVar13 + 1;
              pfVar3 = pfVar12 + 2;
              pfVar4 = pfVar13 + 2;
              pfVar5 = pfVar12 + 3;
              pfVar12 = pfVar12 + 4;
              pfVar6 = pfVar13 + 3;
              pfVar13 = pfVar13 + 4;
              iVar14 = iVar14 + -4;
              fVar9 = *pfVar5 * *pfVar6 +
                      *pfVar3 * *pfVar4 + *pfVar1 * *pfVar2 + fVar7 * fVar8 + fVar9;
            } while (iVar14 != 0);
          }
LAB_0002068b:
          *pfVar15 = fVar9;
          local_1c = local_1c + q;
          iVar16 = iVar16 + 1;
          pfVar15 = pfVar15 + 1;
        } while (iVar16 < r);
      }
      i = i + 1;
      local_20 = local_20 + q;
      local_24 = local_24 + r;
    } while (i < p);
  }
  return;
}


/* ==== LimitSingleAxis ==== */

bool LimitSingleAxis(MdtBclLimit *limit,MdtKeaInputRowFactors *factors,uint RowIndex,
                    MdtBclSolverParameters *params)

{
  bool bVar1;
  
  if (0.0 <= limit->overshoot) {
    if ((0.0 < limit->overshoot) && (0.0 < limit->limit[1].stiffness)) {
      bVar1 = SetLimitFactors(factors,RowIndex,limit,limit->limit + 1,params);
      if (!bVar1) {
        return false;
      }
      factors->lo[RowIndex] = -3.4028235e+38;
      factors->hi[RowIndex] = 0.0;
    }
  }
  else {
                    /* Unresolved local var: MdtBclSingleLimit * lolimit@[DW_OP_reg7(EDI)] */
    if (0.0 < limit->limit[0].stiffness) {
      bVar1 = SetLimitFactors(factors,RowIndex,limit,limit->limit,params);
      if (!bVar1) {
        return false;
      }
      factors->lo[RowIndex] = 0.0;
      factors->hi[RowIndex] = 3.4028235e+38;
    }
  }
  return true;
}


/* ==== FUN_00020720 ==== */

longdouble FUN_00020720(void)

{
  int in_EDX;
  longdouble in_ST0;
  
  return (longdouble)*(float *)(in_EDX + 1) / in_ST0;
}


/* ==== ActuateJointAxes ==== */

bool ActuateJointAxes(MdtBclLimit *limit,int *BodyIndex,MeReal *LinearAxis,MeReal *RotationalAxis1,
                     MeReal *RotationalAxis2,MdtKeaBody **blist,MdtKeaInputRowFactors *factors,
                     uint RowIndex,MdtKeaConstraints *clist)

{
  float fVar1;
  bool bVar2;
  uint uVar3;
  
  if (((limit->overshoot == 0.0) && (limit->bPowered == 0)) && (limit->is_locked == 0)) {
    bVar2 = false;
  }
  else {
                    /* Unresolved local var: uint i@[???] */
    uVar3 = clist->num_rows_inc_padding + RowIndex;
    clist->Jstore[uVar3 >> 2][0].col[0][uVar3 & 3] = *LinearAxis;
    uVar3 = clist->num_rows_inc_padding + RowIndex;
    clist->Jstore[uVar3 >> 2][0].col[3][uVar3 & 3] = *RotationalAxis1;
    uVar3 = clist->num_rows_inc_padding + RowIndex;
    clist->Jstore[uVar3 >> 2][0].col[1][uVar3 & 3] = LinearAxis[1];
    uVar3 = clist->num_rows_inc_padding + RowIndex;
    clist->Jstore[uVar3 >> 2][0].col[4][uVar3 & 3] = RotationalAxis1[1];
    uVar3 = clist->num_rows_inc_padding + RowIndex;
    clist->Jstore[uVar3 >> 2][0].col[2][uVar3 & 3] = LinearAxis[2];
    uVar3 = clist->num_rows_inc_padding + RowIndex;
    clist->Jstore[uVar3 >> 2][0].col[5][uVar3 & 3] = RotationalAxis1[2];
    if (BodyIndex[1] != -1) {
      uVar3 = clist->num_rows_inc_padding + RowIndex;
      clist->Jstore[uVar3 >> 2][1].col[0][uVar3 & 3] = -*LinearAxis;
      uVar3 = clist->num_rows_inc_padding + RowIndex;
      clist->Jstore[uVar3 >> 2][1].col[3][uVar3 & 3] = -*RotationalAxis2;
      uVar3 = clist->num_rows_inc_padding + RowIndex;
      clist->Jstore[uVar3 >> 2][1].col[1][uVar3 & 3] = -LinearAxis[1];
      uVar3 = clist->num_rows_inc_padding + RowIndex;
      clist->Jstore[uVar3 >> 2][1].col[4][uVar3 & 3] = -RotationalAxis2[1];
      uVar3 = clist->num_rows_inc_padding + RowIndex;
      clist->Jstore[uVar3 >> 2][1].col[2][uVar3 & 3] = -LinearAxis[2];
      uVar3 = clist->num_rows_inc_padding + RowIndex;
      clist->Jstore[uVar3 >> 2][1].col[5][uVar3 & 3] = -RotationalAxis2[2];
    }
    if ((limit->bPowered != 0) || (limit->is_locked != 0)) {
      if (limit->overshoot == 0.0) {
        factors->lo[RowIndex] = -limit->fmax;
        factors->hi[RowIndex] = limit->fmax;
        if (limit->bPowered == 0) {
          factors->xi[RowIndex] = limit->position - limit->position_lock;
        }
        else {
          factors->c[RowIndex] = limit->desired_vel;
        }
      }
      else {
                    /* Unresolved local var: MeReal scale@[DW_OP_reg12(ST1)] */
        fVar1 = limit->fmax;
        if (limit->desired_vel <= limit->velocity) {
          fVar1 = -fVar1;
        }
        blist[*BodyIndex]->force[0] = fVar1 * *LinearAxis + blist[*BodyIndex]->force[0];
        blist[*BodyIndex]->force[1] = fVar1 * LinearAxis[1] + blist[*BodyIndex]->force[1];
        blist[*BodyIndex]->force[2] = fVar1 * LinearAxis[2] + blist[*BodyIndex]->force[2];
        blist[*BodyIndex]->torque[0] = fVar1 * *RotationalAxis1 + blist[*BodyIndex]->torque[0];
        blist[*BodyIndex]->torque[1] = fVar1 * RotationalAxis1[1] + blist[*BodyIndex]->torque[1];
        blist[*BodyIndex]->torque[2] = fVar1 * RotationalAxis1[2] + blist[*BodyIndex]->torque[2];
        if (BodyIndex[1] != -1) {
          fVar1 = -fVar1;
          blist[BodyIndex[1]]->force[0] = fVar1 * *LinearAxis + blist[BodyIndex[1]]->force[0];
          blist[BodyIndex[1]]->force[1] = fVar1 * LinearAxis[1] + blist[BodyIndex[1]]->force[1];
          blist[BodyIndex[1]]->force[2] = fVar1 * LinearAxis[2] + blist[BodyIndex[1]]->force[2];
          blist[BodyIndex[1]]->torque[0] = fVar1 * *RotationalAxis2 + blist[BodyIndex[1]]->torque[0]
          ;
          blist[BodyIndex[1]]->torque[1] =
               fVar1 * RotationalAxis2[1] + blist[BodyIndex[1]]->torque[1];
          blist[BodyIndex[1]]->torque[2] =
               fVar1 * RotationalAxis2[2] + blist[BodyIndex[1]]->torque[2];
        }
      }
    }
    bVar2 = true;
  }
  return bVar2;
}


/* ==== CalculateLimitPositionAndOvershoot ==== */

void CalculateLimitPositionAndOvershoot(MdtBclLimit *limit,MeReal stepsize,MeBool bRotary)

{
  float fVar1;
  float fVar2;
  
  if (limit->bPositionInitialised == 0) {
    limit->bPositionInitialised = 1;
    limit->previous_position = limit->position;
    limit->offset = limit->offset - limit->position;
  }
  if (1e-06 <= ABS(limit->velocity)) {
    if (bRotary == 0) {
      fVar2 = limit->position;
    }
    else {
      fVar2 = limit->position;
      fVar1 = limit->previous_position + 3.1415927;
      if (fVar1 < fVar2) {
        do {
          fVar2 = fVar2 - 6.2831855;
        } while (fVar1 < fVar2);
        limit->position = fVar2;
      }
      while (fVar2 + 3.1415927 < limit->previous_position) {
        fVar2 = fVar2 + 6.2831855;
        limit->position = fVar2;
      }
    }
  }
  else {
    fVar2 = limit->previous_position;
    limit->position = fVar2;
  }
  limit->previous_position = fVar2;
  limit->overshoot = 0.0;
  if (limit->bLimited != 0) {
                    /* Unresolved local var: MeReal UserCoordinate@[DW_OP_reg11(ST0)] */
    fVar1 = limit->limit[0].stop;
    fVar2 = fVar2 + limit->offset;
    if ((fVar2 < fVar1) || (fVar1 = limit->limit[1].stop, fVar1 < fVar2)) {
      limit->overshoot = fVar2 - fVar1;
    }
    else {
      limit->bRelaxingToLimit = 0;
    }
  }
  return;
}


/* ==== MdtContactWriteRow ==== */

void MdtContactWriteRow(MdtKeaConstraints *clist,int row,MeReal *vector,MeReal *pos0,MeReal *pos1,
                       MeBool writeBody1)

{
  MdtKeaJBlockPair *paMVar1;
  int iVar2;
  int iVar3;
  
                    /* Unresolved local var: int j@[DW_OP_reg6(ESI)] */
  iVar3 = row + clist->num_rows_inc_padding;
  iVar2 = iVar3;
  if (iVar3 < 0) {
    iVar2 = iVar3 + 3;
  }
  *(MeReal *)((int)*clist->Jstore + iVar3 * 4 + (iVar2 >> 2) * 0xb0) = *vector;
  iVar3 = row + clist->num_rows_inc_padding;
  iVar2 = iVar3;
  if (iVar3 < 0) {
    iVar2 = iVar3 + 3;
  }
  *(MeReal *)((int)*clist->Jstore + iVar3 * 4 + (iVar2 >> 2) * 0xb0 + 0x10) = vector[1];
  iVar3 = row + clist->num_rows_inc_padding;
  iVar2 = iVar3;
  if (iVar3 < 0) {
    iVar2 = iVar3 + 3;
  }
  *(MeReal *)((int)*clist->Jstore + iVar3 * 4 + (iVar2 >> 2) * 0xb0 + 0x20) = vector[2];
  iVar3 = row + clist->num_rows_inc_padding;
  iVar2 = iVar3;
  if (iVar3 < 0) {
    iVar2 = iVar3 + 3;
  }
  iVar3 = iVar3 + (iVar2 >> 2) * 0x2c;
  paMVar1 = clist->Jstore;
  *(float *)((int)*paMVar1 + iVar3 * 4 + 0x30) = vector[2] * pos0[1] - vector[1] * pos0[2];
  *(float *)((int)*paMVar1 + iVar3 * 4 + 0x40) = *vector * pos0[2] - vector[2] * *pos0;
  *(float *)((int)*paMVar1 + iVar3 * 4 + 0x50) = vector[1] * *pos0 - *vector * pos0[1];
  if (writeBody1 != 0) {
    iVar3 = row + clist->num_rows_inc_padding;
    iVar2 = iVar3;
    if (iVar3 < 0) {
      iVar2 = iVar3 + 3;
    }
    *(float *)((int)*clist->Jstore + iVar3 * 4 + (iVar2 >> 2) * 0xb0 + 0x60) = -*vector;
    iVar3 = row + clist->num_rows_inc_padding;
    iVar2 = iVar3;
    if (iVar3 < 0) {
      iVar2 = iVar3 + 3;
    }
    *(float *)((int)*clist->Jstore + iVar3 * 4 + 0x70 + (iVar2 >> 2) * 0xb0) = -vector[1];
    iVar3 = row + clist->num_rows_inc_padding;
    iVar2 = iVar3;
    if (iVar3 < 0) {
      iVar2 = iVar3 + 3;
    }
    *(float *)((int)*clist->Jstore + iVar3 * 4 + 0x80 + (iVar2 >> 2) * 0xb0) = -vector[2];
    iVar3 = row + clist->num_rows_inc_padding;
    iVar2 = iVar3;
    if (iVar3 < 0) {
      iVar2 = iVar3 + 3;
    }
    iVar3 = iVar3 + (iVar2 >> 2) * 0x2c;
    paMVar1 = clist->Jstore;
    *(float *)((int)*paMVar1 + iVar3 * 4 + 0x90) = pos1[2] * vector[1] - pos1[1] * vector[2];
    *(float *)((int)*paMVar1 + iVar3 * 4 + 0xa0) = *pos1 * vector[2] - pos1[2] * *vector;
    *(float *)((int)*paMVar1 + iVar3 * 4 + 0xb0) = pos1[1] * *vector - *pos1 * vector[1];
  }
  return;
}


/* ==== FUN_00020ee0 ==== */

void FUN_00020ee0(void)

{
  float *pfVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int in_ECX;
  int unaff_EBP;
  float *unaff_EDI;
  
  iVar3 = *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + (in_ECX + (in_ECX + 3 >> 2) * 0x2c) * 4;
  pfVar1 = *(float **)(unaff_EBP + 0x14);
  *(float *)(iVar3 + 0x30) = unaff_EDI[2] * pfVar1[1] - unaff_EDI[1] * pfVar1[2];
  *(float *)(iVar3 + 0x40) = *unaff_EDI * pfVar1[2] - unaff_EDI[2] * *pfVar1;
  *(float *)(iVar3 + 0x50) = unaff_EDI[1] * *pfVar1 - *unaff_EDI * pfVar1[1];
  if (*(int *)(unaff_EBP + 0x1c) != 0) {
    iVar4 = *(int *)(unaff_EBP + 0xc) + *(int *)(*(int *)(unaff_EBP + 8) + 0x14);
    iVar3 = iVar4;
    if (iVar4 < 0) {
      iVar3 = iVar4 + 3;
    }
    iVar2 = *(int *)(unaff_EBP + 8);
    *(float *)((iVar3 >> 2) * 0xb0 + *(int *)(iVar2 + 0x2c) + 0x60 + iVar4 * 4) = -*unaff_EDI;
    iVar4 = *(int *)(unaff_EBP + 0xc) + *(int *)(iVar2 + 0x14);
    iVar3 = iVar4;
    if (iVar4 < 0) {
      iVar3 = iVar4 + 3;
    }
    iVar2 = *(int *)(unaff_EBP + 8);
    *(float *)((iVar3 >> 2) * 0xb0 + *(int *)(iVar2 + 0x2c) + iVar4 * 4 + 0x70) = -unaff_EDI[1];
    iVar4 = *(int *)(unaff_EBP + 0xc) + *(int *)(iVar2 + 0x14);
    iVar3 = iVar4;
    if (iVar4 < 0) {
      iVar3 = iVar4 + 3;
    }
    iVar2 = *(int *)(unaff_EBP + 8);
    *(float *)((iVar3 >> 2) * 0xb0 + *(int *)(iVar2 + 0x2c) + iVar4 * 4 + 0x80) = -unaff_EDI[2];
    iVar4 = *(int *)(unaff_EBP + 0xc) + *(int *)(iVar2 + 0x14);
    iVar3 = iVar4;
    if (iVar4 < 0) {
      iVar3 = iVar4 + 3;
    }
    pfVar1 = *(float **)(unaff_EBP + 0x18);
    iVar3 = *(int *)(*(int *)(unaff_EBP + 8) + 0x2c) + (iVar4 + (iVar3 >> 2) * 0x2c) * 4;
    *(float *)(iVar3 + 0x90) = pfVar1[2] * unaff_EDI[1] - pfVar1[1] * unaff_EDI[2];
    *(float *)(iVar3 + 0xa0) = *pfVar1 * unaff_EDI[2] - pfVar1[2] * *unaff_EDI;
    *(float *)(iVar3 + 0xb0) = pfVar1[1] * *unaff_EDI - *pfVar1 * unaff_EDI[1];
  }
  return;
}


/* ==== SetLimitFactors ==== */

bool SetLimitFactors(MdtKeaInputRowFactors *factors,uint RowIndex,MdtBclLimit *limit,
                    MdtBclSingleLimit *sl,MdtBclSolverParameters *params)

{
  float fVar1;
  
  if (limit->damping_thresh <= sl->stiffness) {
    fVar1 = -sl->restitution * limit->velocity;
    limit->bRelaxingToLimit =
         (uint)(ABS((fVar1 * params->stepsize) / (params->gamma * limit->overshoot)) < 1.0);
    if (limit->velocity * limit->overshoot < 0.0) {
      if (limit->bRelaxingToLimit == 0) {
        return false;
      }
    }
    else {
      factors->c[RowIndex] = fVar1;
      if (limit->bRelaxingToLimit == 0) {
        return true;
      }
    }
    factors->c[RowIndex] = 0.0;
  }
  else {
                    /* Unresolved local var: MeReal hepsilon@[DW_OP_reg13(ST2)] */
    fVar1 = 1.0 / (sl->stiffness * params->stepsize + sl->damping);
    factors->slipfactor[RowIndex] = fVar1 - params->epsilon;
    if (factors->slipfactor[RowIndex] < 0.0) {
      factors->slipfactor[RowIndex] = 0.0;
    }
    if (sl->damping == 0.0) {
      factors->xgamma[RowIndex] = 1.0 - params->gamma;
    }
    else {
      factors->xgamma[RowIndex] = fVar1 * params->stepsize * sl->stiffness - params->gamma;
    }
  }
                    /* Unresolved local var: MeReal ReboundVelocity@[DW_OP_reg15(ST4)] */
  factors->xi[RowIndex] = limit->overshoot;
  return true;
}


/* ==== FUN_00021130 ==== */

/* WARNING: Instruction at (ram,0x00021309) overlaps instruction at (ram,0x00021308)
    */

void FUN_00021130(void)

{
  undefined1 uVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  code *pcVar4;
  undefined6 uVar5;
  uint uVar6;
  char cVar7;
  byte bVar8;
  byte bVar9;
  byte bVar10;
  byte bVar18;
  uint in_EAX;
  uint uVar11;
  int iVar12;
  byte *pbVar13;
  undefined3 uVar19;
  int *piVar14;
  uint *puVar15;
  uint3 uVar20;
  char *pcVar16;
  byte *pbVar17;
  int in_ECX;
  int iVar21;
  int extraout_ECX;
  int in_EDX;
  char *pcVar22;
  byte *unaff_EBX;
  int *piVar23;
  int unaff_EBP;
  byte *unaff_ESI;
  byte *pbVar24;
  undefined1 *unaff_EDI;
  undefined2 in_CS;
  undefined2 uVar25;
  bool bVar26;
  byte in_AF;
  byte in_TF;
  byte in_IF;
  byte bVar27;
  byte in_NT;
  byte in_AC;
  byte in_VIF;
  byte in_VIP;
  byte in_ID;
  undefined8 uVar28;
  undefined1 auStack_8 [4];
  undefined1 auStack_4 [4];
  
  bVar27 = 0;
  pcVar22 = (char *)(in_EDX + 1);
  uVar11 = in_EAX | 0x5834905;
  LOCK();
  uVar1 = unaff_EDI[uVar11 * 4];
  unaff_EDI[uVar11 * 4] = (char)uVar11;
  UNLOCK();
  iVar12 = CONCAT31((int3)(uVar11 >> 8),uVar1) + *(int *)unaff_EBX;
  cVar7 = (char)iVar12 + -1;
  bVar9 = (byte)((uint)in_ECX >> 8);
  *unaff_ESI = *unaff_ESI | bVar9;
  *unaff_EBX = *unaff_EBX + cVar7;
  *(int *)unaff_ESI = *(int *)unaff_ESI + unaff_EBP;
  pbVar13 = (byte *)(iVar12 + 0x2d);
  *pbVar13 = *pbVar13 & (byte)pcVar22;
  *unaff_EBX = *unaff_EBX + cVar7;
  uVar5 = *(undefined6 *)(unaff_ESI + unaff_EBP);
  pbVar13 = (byte *)uVar5;
  *pbVar13 = *pbVar13 ^ (byte)uVar5;
  *(char *)((int)pbVar13 * 2) = *(char *)((int)pbVar13 * 2) + bVar9;
  *pbVar13 = *pbVar13 + (byte)uVar5;
  uVar19 = (undefined3)((uint6)uVar5 >> 8);
  cVar7 = in(0);
  pbVar13 = (byte *)CONCAT31(uVar19,cVar7);
  *pbVar13 = *pbVar13 + cVar7;
  *pbVar13 = *pbVar13 ^ (byte)((uint)pcVar22 >> 8);
  *(byte **)pbVar13 = pbVar13 + *(int *)pbVar13;
  bVar10 = bRam00000018;
  piVar14 = (int *)CONCAT31(uVar19,bRam00000018);
  piVar23 = (int *)auStack_4;
  *(byte *)(unaff_EBP + 0x50d4202) = *(byte *)(unaff_EBP + 0x50d4202) | bRam00000018;
  uRam03870486 = uRam03870486 + 3;
  unaff_ESI[1] = unaff_ESI[1] | bVar9;
  bVar9 = *unaff_EBX;
  *unaff_EBX = *unaff_EBX + bVar10;
  if (SCARRY1(bVar9,bVar10) != (char)*unaff_EBX < '\0') {
    *piVar14 = (int)(*piVar14 + (int)piVar14);
    iVar12 = CONCAT31(uVar19,bVar10 + (byte)pcVar22);
    puVar15 = (uint *)(iVar12 + -1);
    uVar11 = *puVar15;
    *puVar15 = *puVar15 + (int)puVar15;
    iVar12 = iVar12 + -2;
    cVar7 = (char)iVar12 - CARRY4(uVar11,(uint)puVar15);
    piVar14 = (int *)CONCAT31((int3)((uint)iVar12 >> 8),cVar7);
    *(char *)piVar14 = (char)*piVar14 + cVar7;
    piVar23 = (int *)auStack_8;
    in_ECX = in_ECX + 1;
  }
  *unaff_EBX = *unaff_EBX ^ (byte)piVar14;
  *(byte *)piVar14 = (char)*piVar14 + (byte)piVar14;
  uVar19 = (undefined3)((uint)piVar14 >> 8);
  *(undefined1 *)((uint)piVar14 & 0xffffff00) = *(undefined1 *)((uint)piVar14 & 0xffffff00);
  pcVar16 = (char *)CONCAT31(uVar19,1);
  *pcVar16 = *pcVar16 + '\x01';
  pbVar13 = (byte *)(pcVar16 + 1);
  *pbVar13 = *pbVar13 >> 1 | *pbVar13 << 7;
  pcVar16[0x1c] = pcVar16[0x1c] + (char)in_ECX;
  *pcVar16 = *pcVar16 + '\x01';
  *(char *)(in_ECX + 0xe) = *(char *)(in_ECX + 0xe) + '\x01';
  *(byte *)(unaff_EBP + 0x50d4202) = *(byte *)(unaff_EBP + 0x50d4202) | 1;
  uRam03870486 = uRam03870486 + 3;
  pbVar24 = unaff_ESI + 2;
  bVar9 = (byte)((uint)(in_ECX + -1) >> 8);
  *pbVar24 = *pbVar24 | bVar9;
  *pcVar16 = *pcVar16 + '\x01';
  *pcVar16 = *pcVar16 + '\x01';
  pcVar16 = (char *)CONCAT31(uVar19,1);
  *pcVar16 = *pcVar16 + '\x01';
  *pcVar16 = *pcVar16 + '\x01';
  bVar18 = (byte)((uint)piVar14 >> 8);
  *(byte *)(unaff_EBP + 1) = *(byte *)(unaff_EBP + 1) & bVar18;
  pcVar16[8] = pcVar16[8] + bVar9;
  *pcVar16 = *pcVar16 + '\x01';
  *(char *)(in_ECX + 0xd) = *(char *)(in_ECX + 0xd) + '\x01';
  *(byte *)(unaff_EBP + 0x50d4202) = *(byte *)(unaff_EBP + 0x50d4202) | 1;
  bVar26 = 0xfffffffc < uRam03870486;
  uRam03870486 = uRam03870486 + 3;
  *pcVar22 = *pcVar22 + '\x01' + bVar26;
  pbVar13 = (byte *)*piVar23;
  *unaff_EBX = *unaff_EBX & 1;
  bVar9 = *pbVar13;
  bVar10 = bVar9 + 1;
  puVar15 = (uint *)CONCAT31(uVar19,bVar10);
  *(byte *)puVar15 = (char)*puVar15 + bVar10 + (0xfe < *pbVar13);
  *(char *)((int)puVar15 * 2) = *(char *)((int)puVar15 * 2) + (char)((uint)(in_ECX + -2) >> 8);
  *(byte *)puVar15 = (char)*puVar15 + bVar10;
  puVar2 = (undefined4 *)(unaff_EDI + 1);
  uVar1 = in((short)pbVar13);
  *unaff_EDI = uVar1;
  *puVar15 = (uint)(*puVar15 + (int)puVar15);
  *(char *)((int)puVar15 + -0x37fffe93) = *(char *)((int)puVar15 + -0x37fffe93) + (char)pbVar13;
  *puVar15 = *puVar15 | (uint)puVar15;
  *(byte *)puVar15 = (char)*puVar15 + bVar10;
  *(undefined2 *)piVar23 = in_CS;
  *(byte *)(unaff_EBP + 0x50d4202) = *(byte *)(unaff_EBP + 0x50d4202) | bVar10;
  bVar26 = 0xfffffffc < uRam03870486;
  uRam03870486 = uRam03870486 + 3;
  bVar9 = bVar9 - 5;
  bVar8 = bVar9 - bVar26;
  puVar15 = (uint *)CONCAT31(uVar19,bVar8);
  *pbVar13 = *pbVar13 + bVar8 + (bVar10 < 6 || bVar9 < bVar26);
  iVar12 = *piVar23;
  *unaff_EBX = *unaff_EBX & bVar8;
  bVar9 = *pbVar13;
  *pbVar13 = *pbVar13 + bVar8;
  *(byte *)puVar15 = (byte)*puVar15 + bVar8 + CARRY1(bVar9,bVar8);
  *(char *)((int)puVar15 * 2) = *(char *)((int)puVar15 * 2) + (char)((uint)(in_ECX + -2) >> 8);
  uVar11 = *puVar15;
  uVar6 = *puVar15;
  *(byte *)puVar15 = (byte)*puVar15 + bVar8;
  *(uint *)(iVar12 + -4) =
       (uint)(in_NT & 1) * 0x4000 | (uint)SCARRY1((byte)uVar6,bVar8) * 0x800 |
       (uint)(in_IF & 1) * 0x200 | (uint)(in_TF & 1) * 0x100 |
       (uint)((char)(byte)*puVar15 < '\0') * 0x80 | (uint)((byte)*puVar15 == 0) * 0x40 |
       (uint)(in_AF & 1) * 0x10 | (uint)((POPCOUNT((byte)*puVar15) & 1U) == 0) * 4 |
       (uint)CARRY1((byte)uVar11,bVar8) | (uint)(in_ID & 1) * 0x200000 |
       (uint)(in_VIP & 1) * 0x100000 | (uint)(in_VIF & 1) * 0x80000 | (uint)(in_AC & 1) * 0x40000;
  *puVar15 = (uint)(*puVar15 + (int)puVar15);
  *(byte *)((int)puVar15 + 0x77) = *(byte *)((int)puVar15 + 0x77) + bVar18;
  uVar11 = *puVar15;
  *puVar15 = (uint)(*puVar15 + (int)puVar15);
  *pbVar24 = *pbVar24 + (char)unaff_EBX + CARRY4(uVar11,(uint)puVar15);
  *(byte *)puVar15 = (byte)*puVar15 + bVar8;
  *(char *)(in_ECX + 0xc) = *(char *)(in_ECX + 0xc) + bVar8;
  *(byte *)(unaff_EBP + 0x50d4202) = *(byte *)(unaff_EBP + 0x50d4202) | bVar8;
  iVar21 = in_ECX + -3;
  uRam03870486 = uRam03870486 + 3;
  *(undefined2 *)(iVar12 + -8) = in_CS;
  bVar9 = bVar8 + 0x2e;
  pcVar22 = (char *)CONCAT31(uVar19,bVar9);
  *unaff_EBX = *unaff_EBX + bVar9 + (0xd1 < bVar8);
  *(undefined2 *)(iVar12 + -0xc) = in_CS;
  *unaff_EBX = *unaff_EBX & bVar9;
  *pcVar22 = *pcVar22 + (char)((uint)iVar21 >> 8);
  *pcVar22 = *pcVar22 + bVar9;
  piVar14 = (int *)CONCAT22((short)((uint)piVar14 >> 0x10),CONCAT11(bVar18 + (char)iVar21,bVar9));
  *piVar14 = *piVar14 + (int)piVar14;
  *(char *)((int)piVar14 + -0x6b) = *(char *)((int)piVar14 + -0x6b) + (char)((uint)pbVar13 >> 8);
  *piVar14 = *piVar14 + (int)piVar14;
  *(int **)(iVar12 + -0xc) = piVar14;
  *(int *)(iVar12 + -0x10) = iVar21;
  *(byte **)(iVar12 + -0x14) = pbVar13;
  *(byte **)(iVar12 + -0x18) = unaff_EBX;
  *(int *)(iVar12 + -0x1c) = iVar12 + -8;
  *(int *)(iVar12 + -0x20) = unaff_EBP;
  *(byte **)(iVar12 + -0x24) = pbVar24;
  *(undefined4 **)(iVar12 + -0x28) = puVar2;
  piVar14 = piVar14 + 0x10400000;
  *(undefined2 *)(iVar12 + -0x2c) = in_CS;
  bVar9 = (byte)piVar14;
  *(byte *)(unaff_EBP + 0x50d4202) = *(byte *)(unaff_EBP + 0x50d4202) | bVar9;
  uRam03870486 = uRam03870486 + 3;
  uVar19 = (undefined3)((uint)unaff_EBX >> 8);
  puVar15 = (uint *)CONCAT31(uVar19,1);
  *(byte *)puVar15 = (byte)*puVar15 & bVar9;
  *(byte *)piVar14 = (char)*piVar14 + bVar9;
  *(char *)piVar14 = (char)*piVar14 + (char)((uint)(in_ECX + -4) >> 8);
  *(byte *)piVar14 = (char)*piVar14 + bVar9;
  piVar14 = (int *)CONCAT31((int3)((uint)piVar14 >> 8),bVar9 + (char)((uint)unaff_EBX >> 8));
  *piVar14 = *piVar14 + (int)piVar14;
  *(undefined2 *)(iVar12 + -0x30) = in_CS;
  uVar25 = 0;
  *(uleb128 **)(iVar12 + -0x34) = &uleb128_00021274;
  uVar28 = func_0x08880001();
  pcVar22 = (char *)uVar28;
  bVar9 = (byte)uVar28;
  *(char *)(extraout_ECX + 0xe) = *(char *)(extraout_ECX + 0xe) + bVar9;
  *(byte *)(unaff_EBP + 0x50d4202) = *(byte *)(unaff_EBP + 0x50d4202) | bVar9;
  uRam03870486 = uRam03870486 + 3;
  *puVar15 = *puVar15 | (uint)pcVar22;
  *(byte *)puVar15 = (byte)*puVar15 & bVar9;
  pbVar13 = (byte *)CONCAT31(uVar19,4);
  *pcVar22 = *pcVar22 + bVar9;
  *(char *)((int)pcVar22 * 2) = *(char *)((int)pcVar22 * 2) + (char)((ulonglong)uVar28 >> 8);
  *pcVar22 = *pcVar22 + bVar9;
  pcVar22 = (char *)((uint)pcVar22 & 0xffffff02);
  bVar9 = (byte)pcVar22;
  *pcVar22 = *pcVar22 + bVar9;
  *(char **)(iVar12 + -0x34) = pcVar22;
  *(int *)(iVar12 + -0x38) = extraout_ECX + -1;
  *(int *)(iVar12 + -0x3c) = (int)((ulonglong)uVar28 >> 0x20);
  *(byte **)(iVar12 + -0x40) = pbVar13;
  *(int *)(iVar12 + -0x44) = iVar12 + -0x30;
  *(int *)(iVar12 + -0x48) = unaff_EBP;
  *(byte **)(iVar12 + -0x4c) = pbVar24;
  *(undefined4 **)(iVar12 + -0x50) = puVar2;
  pcRam14d80001 = pcVar22;
  *pcVar22 = *pcVar22 + bVar9;
  *(char *)(extraout_ECX + 0xd) = *(char *)(extraout_ECX + 0xd) + bVar9;
  *(byte *)(unaff_EBP + 0x50d4202) = *(byte *)(unaff_EBP + 0x50d4202) | bVar9;
  bVar26 = 0xfffffffc < uRam03870486;
  uRam03870486 = uRam03870486 + 3;
  uVar20 = (uint3)((uint)pcVar22 >> 8);
  cVar7 = in(2);
  pcVar22 = (char *)CONCAT31(uVar20,cVar7);
  *pcVar22 = *pcVar22 + cVar7 + bVar26;
  *pcVar22 = *pcVar22 + cVar7;
  pcVar22 = (char *)((uint)uVar20 << 8);
  *pcVar22 = *pcVar22;
  puVar15 = (uint *)CONCAT31(uVar20,*pcVar22);
  *(char *)(puVar15 + -0x12) = (char)puVar15[-0x12] + *pcVar22;
  uVar11 = *puVar15;
  *puVar15 = *puVar15 + (int)puVar15;
  uVar3 = in((short)((ulonglong)uVar28 >> 0x20));
  *puVar2 = uVar3;
  pcVar22 = (char *)((int)puVar15 + (uint)CARRY4(uVar11,(uint)puVar15) + *puVar15);
  bVar10 = (byte)pcVar22;
  *pcVar22 = *pcVar22 + bVar10;
  *(undefined2 *)(iVar12 + -0x55) = uVar25;
  *(byte *)(unaff_EBP + 0x50d4202) = *(byte *)(unaff_EBP + 0x50d4202) | bVar10;
  bVar26 = 0xfffffffc < uRam03870486;
  uRam03870486 = uRam03870486 + 3;
  bVar9 = 9 < (bVar10 & 0xf) | in_AF;
  uVar20 = (uint3)((uint)pcVar22 >> 8);
  cVar7 = bVar10 + bVar9 * -6 + (0x99 < bVar10 || bVar26) * -0x60;
  pcVar16 = (char *)CONCAT31(uVar20,cVar7);
  *pbVar24 = *pbVar24 | (byte)((uint)(extraout_ECX + -2) >> 8);
  *pcVar16 = *pcVar16 + cVar7;
  *pcVar16 = *pcVar16 + cVar7;
  pcVar16 = (char *)CONCAT31(uVar20,cVar7);
  *pcVar16 = *pcVar16 + cVar7;
  if (*pcVar16 != '\0') {
    *pcVar16 = *pcVar16 + cVar7;
  }
  pbVar17 = (byte *)CONCAT31(uVar20,0xcb);
  *(byte **)pbVar17 = pbVar17 + *(int *)pbVar17;
  *pbVar17 = *pbVar17 - 0x35;
  *(char *)(extraout_ECX + 0xc) = *(char *)(extraout_ECX + 0xc) + -0x35;
  *(byte *)(unaff_EBP + 0x50d4202) = *(byte *)(unaff_EBP + 0x50d4202) | 0xcb;
  uRam03870486 = uRam03870486 + 3;
  LOCK();
  *(int *)pbVar24 = *(int *)pbVar24 + unaff_EBP;
  UNLOCK();
  bVar26 = false;
  *pbVar13 = *pbVar13 & 0xcb;
  if (extraout_ECX + -1 != 3 && *pbVar13 == 0) {
    bVar26 = 0x34 < *pbVar13;
    *pbVar13 = *pbVar13 - 0x35;
  }
  *pbVar13 = (*pbVar13 - 0x35) + bVar26;
  if ((POPCOUNT(*pbVar13) & 1U) != 0) {
    *pbVar17 = *pbVar17 ^ 0xcb;
  }
  pbVar13 = (byte *)((uint)uVar20 << 8);
  *pbVar13 = *pbVar13;
  unaff_EDI[(uint)bVar27 * -8 + 5] = *pbVar24;
  bVar10 = *pbVar13;
  pbVar24 = (byte *)CONCAT31(uVar20,bVar10);
  pbVar13 = pbVar24 + 0x80001d3;
  *pbVar13 = *pbVar13 + (char)((ulonglong)uVar28 >> 0x20);
  bVar10 = bVar10 | *pbVar24;
  *(char *)CONCAT31(uVar20,bVar10) = *(char *)CONCAT31(uVar20,bVar10) + bVar10;
  *(undefined2 *)(iVar12 + -0x59) = uVar25;
  *(byte *)(unaff_EBP + 0x50d4202) = *(byte *)(unaff_EBP + 0x50d4202) | bVar10;
  bVar26 = 0xfffffffc < uRam03870486;
  uRam03870486 = uRam03870486 + 3;
  pcVar22 = (char *)CONCAT22((short)((uint)pcVar22 >> 0x10),
                             CONCAT11(((int)uRam03870486 < 0) << 7 | (uRam03870486 == 0) << 6 |
                                      bVar9 << 4 | ((POPCOUNT(uRam03870486 & 0xff) & 1U) == 0) << 2
                                      | 2 | bVar26,bVar10));
  *pcVar22 = *pcVar22 + bVar10 +
             CARRY1((byte)((uint)(extraout_ECX + -4) >> 8),unaff_ESI[(uint)bVar27 * -2 + 3]);
  *pcVar22 = *pcVar22 + bVar10;
  *(undefined1 *)((uint)pcVar22 & 0xffffff00) = *(undefined1 *)((uint)pcVar22 & 0xffffff00);
  pcVar4 = (code *)swi(3);
  (*pcVar4)();
  return;
}


