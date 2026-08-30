/* ==== calcIworldandNonInertialForceandVhmf ==== */

/* DWARF original prototype: void calcIworldandNonInertialForceandVhmf(keaFunctions_Vanilla * this,
   MdtKeaInverseMassMatrix * invIworld, MdtKeaVelocity * vhmf, MdtKeaBody * * blist,
   MdtKeaTransformation * tlist, int num_bodies, MeReal stepsize) */

void __thiscall
keaFunctions_Vanilla::calcIworldandNonInertialForceandVhmf
          (keaFunctions_Vanilla *this,MdtKeaInverseMassMatrix *invIworld,MdtKeaVelocity *vhmf,
          MdtKeaBody **blist,MdtKeaTransformation *tlist,int num_bodies,MeReal stepsize)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  MdtKeaBody *pMVar5;
  MdtKeaTransformation *pMVar6;
  int iVar7;
  MdtKeaInverseMassMatrix *pMVar8;
  int iVar9;
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
  pb = blist;
  pMVar8 = invIworld;
  if (num_bodies != 0) {
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
        pMVar8->invI2[1] = 0.0;
        pMVar8->invI2[0] = 0.0;
        pMVar8->invI1[2] = 0.0;
        pMVar8->invI1[0] = 0.0;
        pMVar8->invI0[2] = 0.0;
        pMVar8->invI0[1] = 0.0;
        pMVar8->invI2[2] = fVar1;
        pMVar8->invI1[1] = fVar1;
        pMVar8->invI0[0] = fVar1;
      }
      else {
                    /* Unresolved local var: int i@[DW_OP_reg2(EDX)]
                       Unresolved local var: int j@[???] */
        iVar7 = 0;
        iVar9 = 0;
        pMVar6 = tlist;
        do {
          fVar1 = pMVar6->R0[0];
          fVar2 = pMVar6->R1[0];
          iVar7 = iVar7 + 1;
          fVar3 = pMVar6->R2[0];
          pMVar6 = (MdtKeaTransformation *)(pMVar6->R0 + 1);
          m1[0][iVar9] = fVar3 * pMVar5->invI2[0] +
                         fVar2 * pMVar5->invI1[0] + fVar1 * pMVar5->invI0[0];
          m1[0][iVar9 + 1] =
               fVar3 * pMVar5->invI2[1] + fVar1 * pMVar5->invI0[1] + fVar2 * pMVar5->invI1[1];
          m1[0][iVar9 + 2] =
               fVar3 * pMVar5->invI2[2] + fVar1 * pMVar5->invI0[2] + fVar2 * pMVar5->invI1[2];
          iVar9 = iVar9 + 3;
        } while (iVar7 < 3);
        iVar9 = 0;
        iVar7 = 0;
        do {
          fVar1 = m1[0][iVar7];
          fVar2 = m1[0][iVar7 + 1];
          fVar3 = m1[0][iVar7 + 2];
          iVar9 = iVar9 + 1;
          m2[0][iVar7] = fVar3 * tlist->R2[0] + fVar2 * tlist->R1[0] + fVar1 * tlist->R0[0];
          m2[0][iVar7 + 1] = fVar3 * tlist->R2[1] + fVar1 * tlist->R0[1] + fVar2 * tlist->R1[1];
          m2[0][iVar7 + 2] = fVar3 * tlist->R2[2] + fVar1 * tlist->R0[2] + fVar2 * tlist->R1[2];
          iVar7 = iVar7 + 3;
        } while (iVar9 < 3);
        iVar7 = 0;
        do {
          pMVar8->invI0[iVar7] = m2[0][iVar7];
          pMVar8->invI1[iVar7] = m2[1][iVar7];
          pMVar8->invI2[iVar7] = m2[2][iVar7];
          iVar7 = iVar7 + 1;
        } while (iVar7 < 3);
      }
      pMVar8->invmass = (*pb)->invmass;
      pMVar8->pad2 = 0.0;
      pMVar8->pad1 = 0.0;
      pMVar5 = *pb;
      if ((pMVar5->flags & 8) != 0) {
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: MeReal * omega@[DW_OP_reg0(EAX)] */
        m2[0][0] = tlist->R0[2] * pMVar5->velrot[2] +
                   tlist->R0[1] * pMVar5->velrot[1] + tlist->R0[0] * pMVar5->velrot[0];
        m2[0][1] = tlist->R1[2] * pMVar5->velrot[2] +
                   tlist->R1[0] * pMVar5->velrot[0] + tlist->R1[1] * pMVar5->velrot[1];
        m2[0][2] = tlist->R2[2] * pMVar5->velrot[2] +
                   pMVar5->velrot[1] * tlist->R2[1] + pMVar5->velrot[0] * tlist->R2[0];
        m2[1][1] = m2[0][2] * pMVar5->I2[0] + m2[0][0] * pMVar5->I0[0] + m2[0][1] * pMVar5->I1[0];
        m2[1][2] = m2[0][2] * pMVar5->I2[1] + m2[0][0] * pMVar5->I0[1] + m2[0][1] * pMVar5->I1[1];
        m2[2][0] = m2[0][0] * pMVar5->I0[2] + m2[0][1] * pMVar5->I1[2] + m2[0][2] * pMVar5->I2[2];
        m2[2][2] = tlist->R0[0] * m2[1][1] + tlist->R1[0] * m2[1][2] + tlist->R2[0] * m2[2][0];
        fVar2 = tlist->R0[1] * m2[1][1] + tlist->R1[1] * m2[1][2] + tlist->R2[1] * m2[2][0];
        fVar1 = m2[1][1] * tlist->R0[2] + m2[1][2] * tlist->R1[2] + m2[2][0] * tlist->R2[2];
        m1[0][0] = fVar2 * pMVar5->velrot[2] - fVar1 * pMVar5->velrot[1];
        m1[0][1] = fVar1 * pMVar5->velrot[0] - m2[2][2] * pMVar5->velrot[2];
        m1[0][2] = m2[2][2] * pMVar5->velrot[1] - fVar2 * pMVar5->velrot[0];
        pMVar5->torque[0] = m1[0][0] + pMVar5->torque[0];
        (*pb)->torque[1] = m1[0][1] + (*pb)->torque[1];
        (*pb)->torque[2] = m1[0][2] + (*pb)->torque[2];
      }
      body = body + 1;
      pb = pb + 1;
      tlist = tlist + 1;
      pMVar8 = pMVar8 + 1;
    } while (body != num_bodies);
  }
  body = 0;
  fVar1 = 1.0 / stepsize;
  if (num_bodies != 0) {
    do {
      vhmf->velocity[0] = fVar1 * (*blist)->vel[0] + invIworld->invmass * (*blist)->force[0];
      vhmf->velocity[1] = invIworld->invmass * (*blist)->force[1] + fVar1 * (*blist)->vel[1];
      fVar2 = invIworld->invmass;
      fVar3 = (*blist)->vel[2];
      fVar4 = (*blist)->force[2];
      vhmf->pad0 = 0;
      vhmf->velocity[2] = fVar2 * fVar4 + fVar1 * fVar3;
      fVar2 = fVar1 * (*blist)->velrot[0];
      vhmf->angVelocity[0] = fVar2;
      fVar3 = fVar1 * (*blist)->velrot[1];
      vhmf->angVelocity[1] = fVar3;
      fVar4 = fVar1 * (*blist)->velrot[2];
      vhmf->angVelocity[2] = fVar4;
      fVar2 = invIworld->invI0[0] * (*blist)->torque[0] + fVar2;
      vhmf->angVelocity[0] = fVar2;
      fVar3 = invIworld->invI0[1] * (*blist)->torque[0] + fVar3;
      vhmf->angVelocity[1] = fVar3;
      fVar4 = invIworld->invI0[2] * (*blist)->torque[0] + fVar4;
      vhmf->angVelocity[2] = fVar4;
      fVar2 = invIworld->invI1[0] * (*blist)->torque[1] + fVar2;
      vhmf->angVelocity[0] = fVar2;
      fVar3 = invIworld->invI1[1] * (*blist)->torque[1] + fVar3;
      vhmf->angVelocity[1] = fVar3;
      fVar4 = invIworld->invI1[2] * (*blist)->torque[1] + fVar4;
      vhmf->angVelocity[2] = fVar4;
      vhmf->angVelocity[0] = invIworld->invI2[0] * (*blist)->torque[2] + fVar2;
      vhmf->angVelocity[1] = invIworld->invI2[1] * (*blist)->torque[2] + fVar3;
      pMVar5 = *blist;
      blist = blist + 1;
      fVar2 = invIworld->invI2[2];
      invIworld = invIworld + 1;
      fVar3 = pMVar5->torque[2];
      vhmf->pad1 = 0;
      vhmf->angVelocity[2] = fVar2 * fVar3 + fVar4;
      body = body + 1;
      vhmf = vhmf + 1;
    } while (body != num_bodies);
  }
  return;
}


