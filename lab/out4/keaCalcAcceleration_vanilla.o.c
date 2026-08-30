/* ==== calculateAcceleration ==== */

/* DWARF original prototype: void calculateAcceleration(keaFunctions_Vanilla * this, MdtKeaBody * *
   blist, MdtKeaInverseMassMatrix * invIworld, int num_bodies) */

void keaFunctions_Vanilla::calculateAcceleration
               (MdtKeaBody **blist,MdtKeaInverseMassMatrix *invIworld,int num_bodies)

{
  MeReal MVar1;
  int iVar2;
  int in_stack_00000010;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[???] */
  if (in_stack_00000010 != 0) {
    iVar2 = 0;
    do {
                    /* Unresolved local var: MeReal * invIw@[???] */
      MVar1 = invIworld->invI0[0];
      iVar2 = iVar2 + 1;
      *(float *)((int)MVar1 + 0xc0) = *(float *)((int)MVar1 + 8) * *(float *)((int)MVar1 + 0x10);
      MVar1 = invIworld->invI0[0];
      *(float *)((int)MVar1 + 0xc4) = *(float *)((int)MVar1 + 8) * *(float *)((int)MVar1 + 0x14);
      MVar1 = invIworld->invI0[0];
      *(float *)((int)MVar1 + 200) = *(float *)((int)MVar1 + 8) * *(float *)((int)MVar1 + 0x18);
      MVar1 = invIworld->invI0[0];
      *(float *)((int)MVar1 + 0xd0) =
           *(float *)(num_bodies + 8) * *(float *)((int)MVar1 + 0x28) +
           *(float *)(num_bodies + 4) * *(float *)((int)MVar1 + 0x24) +
           *(float *)num_bodies * *(float *)((int)MVar1 + 0x20);
      MVar1 = invIworld->invI0[0];
      *(float *)((int)MVar1 + 0xd4) =
           *(float *)(num_bodies + 0x18) * *(float *)((int)MVar1 + 0x28) +
           *(float *)(num_bodies + 0x14) * *(float *)((int)MVar1 + 0x24) +
           *(float *)(num_bodies + 0x10) * *(float *)((int)MVar1 + 0x20);
      MVar1 = invIworld->invI0[0];
      invIworld = (MdtKeaInverseMassMatrix *)(invIworld->invI0 + 1);
      *(float *)((int)MVar1 + 0xd8) =
           *(float *)(num_bodies + 0x28) * *(float *)((int)MVar1 + 0x28) +
           *(float *)(num_bodies + 0x24) * *(float *)((int)MVar1 + 0x24) +
           *(float *)(num_bodies + 0x20) * *(float *)((int)MVar1 + 0x20);
      num_bodies = (int)(num_bodies + 0x30);
    } while (iVar2 != in_stack_00000010);
  }
  return;
}


