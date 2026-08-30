/* ==== calculateAcceleration ==== */

/* DWARF original prototype: void calculateAcceleration(keaFunctions_Vanilla * this, MdtKeaBody * *
   blist, MdtKeaInverseMassMatrix * invIworld, int num_bodies) */

void __thiscall
keaFunctions_Vanilla::calculateAcceleration
          (keaFunctions_Vanilla *this,MdtKeaBody **blist,MdtKeaInverseMassMatrix *invIworld,
          int num_bodies)

{
  MdtKeaBody *pMVar1;
  int iVar2;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  if (num_bodies != 0) {
    iVar2 = 0;
    do {
                    /* Unresolved local var: MeReal * invIw@[???] */
      pMVar1 = *blist;
      iVar2 = iVar2 + 1;
      pMVar1->accel[0] = pMVar1->invmass * pMVar1->force[0];
      pMVar1 = *blist;
      pMVar1->accel[1] = pMVar1->invmass * pMVar1->force[1];
      pMVar1 = *blist;
      pMVar1->accel[2] = pMVar1->invmass * pMVar1->force[2];
      pMVar1 = *blist;
      pMVar1->accelrot[0] =
           invIworld->invI0[2] * pMVar1->torque[2] +
           invIworld->invI0[1] * pMVar1->torque[1] + invIworld->invI0[0] * pMVar1->torque[0];
      pMVar1 = *blist;
      pMVar1->accelrot[1] =
           invIworld->invI1[2] * pMVar1->torque[2] +
           invIworld->invI1[1] * pMVar1->torque[1] + invIworld->invI1[0] * pMVar1->torque[0];
      pMVar1 = *blist;
      blist = blist + 1;
      pMVar1->accelrot[2] =
           invIworld->invI2[2] * pMVar1->torque[2] +
           invIworld->invI2[1] * pMVar1->torque[1] + invIworld->invI2[0] * pMVar1->torque[0];
      invIworld = invIworld + 1;
    } while (iVar2 != num_bodies);
  }
  return;
}


