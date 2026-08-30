/* ==== calcIworldandNonInertialForceandVhmf ==== */

/* DWARF original prototype: void calcIworldandNonInertialForceandVhmf(keaFunctions_Vanilla * this,
   MdtKeaInverseMassMatrix * invIworld, MdtKeaVelocity * vhmf, MdtKeaBody * * blist,
   MdtKeaTransformation * tlist, int num_bodies, MeReal stepsize) */

void keaFunctions_Vanilla::calcIworldandNonInertialForceandVhmf
               (MdtKeaInverseMassMatrix *invIworld,MdtKeaVelocity *vhmf,MdtKeaBody **blist,
               MdtKeaTransformation *tlist,int num_bodies,MeReal stepsize)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  MdtKeaBody *pMVar5;
  MdtKeaBody *pMVar6;
  MdtKeaBody *pMVar7;
  MeReal *pMVar8;
  float *pfVar9;
  int iVar10;
  MdtKeaVelocity *pMVar11;
  int iVar12;
  float in_stack_0000001c;
  MdtKeaBody **pb;
  int body;
  MeReal m2 [3] [3];
  MeReal m1 [3] [3];
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: MdtKeaTransformation * tb@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeReal * pinvIworld@[DW_OP_reg3(EBX)]
                       Unresolved local var: MdtKeaInverseMassMatrix * invMworld@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal hinv@[DW_OP_reg17(ST6)]
                       Unresolved local var: MdtKeaVelocity * pvhmf@[DW_OP_reg2(EDX)] */
  body = 0;
  pb = (MdtKeaBody **)tlist;
  pMVar11 = vhmf;
  if (stepsize != 0.0) {
    do {
      pMVar5 = *pb;
      if ((pMVar5->flags & 4) == 0) {
                    /* Unresolved local var: MeReal s@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal t@[DW_OP_reg11(ST0)] */
        fVar1 = pMVar5->I0[0];
        if (fVar1 < pMVar5->I1[1]) {
          fVar1 = pMVar5->I1[1];
        }
        if (fVar1 < pMVar5->I2[2]) {
          fVar1 = pMVar5->I2[2];
        }
        fVar1 = 1.0 / fVar1;
        pMVar11[1].velocity[1] = 0.0;
        pMVar11[1].velocity[0] = 0.0;
        pMVar11->angVelocity[2] = 0.0;
        pMVar11->angVelocity[0] = 0.0;
        pMVar11->velocity[2] = 0.0;
        pMVar11->velocity[1] = 0.0;
        pMVar11[1].velocity[2] = fVar1;
        pMVar11->angVelocity[1] = fVar1;
        pMVar11->velocity[0] = fVar1;
      }
      else {
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)]
                       Unresolved local var: int j@[???] */
        iVar10 = 0;
        iVar12 = 0;
        pfVar9 = (float *)num_bodies;
        do {
          fVar1 = *pfVar9;
          fVar2 = pfVar9[4];
          iVar10 = iVar10 + 1;
          fVar3 = pfVar9[8];
          pfVar9 = pfVar9 + 1;
          m1[0][iVar12] =
               fVar3 * pMVar5->invI2[0] + fVar2 * pMVar5->invI1[0] + fVar1 * pMVar5->invI0[0];
          m1[0][iVar12 + 1] =
               fVar3 * pMVar5->invI2[1] + fVar1 * pMVar5->invI0[1] + fVar2 * pMVar5->invI1[1];
          m1[0][iVar12 + 2] =
               fVar3 * pMVar5->invI2[2] + fVar1 * pMVar5->invI0[2] + fVar2 * pMVar5->invI1[2];
          iVar12 = iVar12 + 3;
        } while (iVar10 < 3);
        iVar12 = 0;
        iVar10 = 0;
        do {
          fVar1 = m1[0][iVar10];
          fVar2 = m1[0][iVar10 + 1];
          fVar3 = m1[0][iVar10 + 2];
          iVar12 = iVar12 + 1;
          m2[0][iVar10] =
               fVar3 * *(float *)(num_bodies + 0x20) +
               fVar2 * *(float *)(num_bodies + 0x10) + fVar1 * *(float *)num_bodies;
          m2[0][iVar10 + 1] =
               fVar3 * *(float *)(num_bodies + 0x24) +
               fVar1 * *(float *)(num_bodies + 4) + fVar2 * *(float *)(num_bodies + 0x14);
          m2[0][iVar10 + 2] =
               fVar3 * *(float *)(num_bodies + 0x28) +
               fVar1 * *(float *)(num_bodies + 8) + fVar2 * *(float *)(num_bodies + 0x18);
          iVar10 = iVar10 + 3;
        } while (iVar12 < 3);
        iVar10 = 0;
        do {
          pMVar11->velocity[iVar10] = m2[0][iVar10];
          pMVar11->angVelocity[iVar10] = m2[1][iVar10];
          pMVar11[1].velocity[iVar10] = m2[2][iVar10];
          iVar10 = iVar10 + 1;
        } while (iVar10 < 3);
      }
      pMVar11->pad0 = (int)(*pb)->invmass;
      pMVar11[1].pad0 = 0;
      pMVar11->pad1 = 0;
      pMVar5 = *pb;
      if ((pMVar5->flags & 8) != 0) {
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: MeReal * omega@[DW_OP_reg0(EAX)] */
        m2[0][0] = *(float *)(num_bodies + 8) * pMVar5->velrot[2] +
                   *(float *)(num_bodies + 4) * pMVar5->velrot[1] +
                   *(float *)num_bodies * pMVar5->velrot[0];
        m2[0][1] = *(float *)(num_bodies + 0x18) * pMVar5->velrot[2] +
                   *(float *)(num_bodies + 0x10) * pMVar5->velrot[0] +
                   *(float *)(num_bodies + 0x14) * pMVar5->velrot[1];
        m2[0][2] = *(float *)(num_bodies + 0x28) * pMVar5->velrot[2] +
                   pMVar5->velrot[1] * *(float *)(num_bodies + 0x24) +
                   pMVar5->velrot[0] * *(float *)(num_bodies + 0x20);
        m2[1][1] = m2[0][2] * pMVar5->I2[0] + m2[0][0] * pMVar5->I0[0] + m2[0][1] * pMVar5->I1[0];
        m2[1][2] = m2[0][2] * pMVar5->I2[1] + m2[0][0] * pMVar5->I0[1] + m2[0][1] * pMVar5->I1[1];
        m2[2][0] = m2[0][0] * pMVar5->I0[2] + m2[0][1] * pMVar5->I1[2] + m2[0][2] * pMVar5->I2[2];
        m2[2][2] = *(float *)num_bodies * m2[1][1] + *(float *)(num_bodies + 0x10) * m2[1][2] +
                   *(float *)(num_bodies + 0x20) * m2[2][0];
        fVar2 = *(float *)(num_bodies + 4) * m2[1][1] + *(float *)(num_bodies + 0x14) * m2[1][2] +
                *(float *)(num_bodies + 0x24) * m2[2][0];
        fVar1 = m2[1][1] * *(float *)(num_bodies + 8) + m2[1][2] * *(float *)(num_bodies + 0x18) +
                m2[2][0] * *(float *)(num_bodies + 0x28);
        m1[0][0] = fVar2 * pMVar5->velrot[2] - fVar1 * pMVar5->velrot[1];
        m1[0][1] = fVar1 * pMVar5->velrot[0] - m2[2][2] * pMVar5->velrot[2];
        m1[0][2] = m2[2][2] * pMVar5->velrot[1] - fVar2 * pMVar5->velrot[0];
        pMVar5->torque[0] = m1[0][0] + pMVar5->torque[0];
        (*pb)->torque[1] = m1[0][1] + (*pb)->torque[1];
        (*pb)->torque[2] = m1[0][2] + (*pb)->torque[2];
      }
      body = body + 1;
      pb = pb + 1;
      num_bodies = num_bodies + 0x40;
      pMVar11 = (MdtKeaVelocity *)pMVar11[1].angVelocity;
    } while ((MeReal)body != stepsize);
  }
  body = 0;
  fVar1 = 1.0 / in_stack_0000001c;
  if (stepsize != 0.0) {
    do {
      *blist = (MdtKeaBody *)
               (fVar1 * *(float *)((int)tlist->R0[0] + 0x90) +
               (float)vhmf->pad0 * *(float *)((int)tlist->R0[0] + 0x10));
      blist[1] = (MdtKeaBody *)
                 ((float)vhmf->pad0 * *(float *)((int)tlist->R0[0] + 0x14) +
                 fVar1 * *(float *)((int)tlist->R0[0] + 0x94));
      fVar2 = (float)vhmf->pad0;
      fVar3 = *(float *)((int)tlist->R0[0] + 0x98);
      fVar4 = *(float *)((int)tlist->R0[0] + 0x18);
      blist[3] = (MdtKeaBody *)0x0;
      blist[2] = (MdtKeaBody *)(fVar2 * fVar4 + fVar1 * fVar3);
      pMVar5 = (MdtKeaBody *)(fVar1 * *(float *)((int)tlist->R0[0] + 0xa0));
      blist[4] = pMVar5;
      pMVar7 = (MdtKeaBody *)(fVar1 * *(float *)((int)tlist->R0[0] + 0xa4));
      blist[5] = pMVar7;
      pMVar6 = (MdtKeaBody *)(fVar1 * *(float *)((int)tlist->R0[0] + 0xa8));
      blist[6] = pMVar6;
      pMVar5 = (MdtKeaBody *)
               (vhmf->velocity[0] * *(float *)((int)tlist->R0[0] + 0x20) + (float)pMVar5);
      blist[4] = pMVar5;
      pMVar7 = (MdtKeaBody *)
               (vhmf->velocity[1] * *(float *)((int)tlist->R0[0] + 0x20) + (float)pMVar7);
      blist[5] = pMVar7;
      pMVar6 = (MdtKeaBody *)
               (vhmf->velocity[2] * *(float *)((int)tlist->R0[0] + 0x20) + (float)pMVar6);
      blist[6] = pMVar6;
      pMVar5 = (MdtKeaBody *)
               (vhmf->angVelocity[0] * *(float *)((int)tlist->R0[0] + 0x24) + (float)pMVar5);
      blist[4] = pMVar5;
      pMVar7 = (MdtKeaBody *)
               (vhmf->angVelocity[1] * *(float *)((int)tlist->R0[0] + 0x24) + (float)pMVar7);
      blist[5] = pMVar7;
      pMVar6 = (MdtKeaBody *)
               (vhmf->angVelocity[2] * *(float *)((int)tlist->R0[0] + 0x24) + (float)pMVar6);
      blist[6] = pMVar6;
      blist[4] = (MdtKeaBody *)
                 (vhmf[1].velocity[0] * *(float *)((int)tlist->R0[0] + 0x28) + (float)pMVar5);
      blist[5] = (MdtKeaBody *)
                 (vhmf[1].velocity[1] * *(float *)((int)tlist->R0[0] + 0x28) + (float)pMVar7);
      pMVar8 = tlist->R0;
      tlist = (MdtKeaTransformation *)(tlist->R0 + 1);
      fVar2 = vhmf[1].velocity[2];
      vhmf = (MdtKeaVelocity *)vhmf[1].angVelocity;
      fVar3 = *(float *)((int)*pMVar8 + 0x28);
      blist[7] = (MdtKeaBody *)0x0;
      blist[6] = (MdtKeaBody *)(fVar2 * fVar3 + (float)pMVar6);
      body = body + 1;
      blist = blist + 8;
    } while ((MeReal)body != stepsize);
  }
  return;
}


