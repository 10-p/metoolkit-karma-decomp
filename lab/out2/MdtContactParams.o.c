/* ==== MdtContactParamsReset ==== */

void MdtContactParamsReset(MdtContactParamsID p)

{
  p->type = MdtContactTypeFrictionZero;
  p->model1 = MdtFrictionModelBox;
  p->model2 = MdtFrictionModelBox;
  p->options = 0;
  p->restitution = 0.0;
  p->velThreshold = 0.001;
  p->softness = 0.0;
  p->max_adhesive_force = 0.0;
  p->friction1 = 3.4028235e+38;
  p->frictioncoeff1 = 0.0;
  p->slip1 = 0.0;
  p->friction2 = 3.4028235e+38;
  p->frictioncoeff2 = 0.0;
  p->slip2 = 0.0;
  p->slide1 = 0.0;
  p->slide2 = 0.0;
  return;
}


/* ==== MdtContactParamsGetType ==== */

MdtContactType MdtContactParamsGetType(MdtContactParamsID p)

{
  return p->type;
}


/* ==== MdtContactParamsGetPrimaryFrictionModel ==== */

MdtFrictionModel MdtContactParamsGetPrimaryFrictionModel(MdtContactParamsID p)

{
  return p->model1;
}


/* ==== MdtContactParamsGetSecondaryFrictionModel ==== */

MdtFrictionModel MdtContactParamsGetSecondaryFrictionModel(MdtContactParamsID p)

{
  return p->model2;
}


/* ==== MdtContactParamsGetRestitution ==== */

MeReal MdtContactParamsGetRestitution(MdtContactParamsID p)

{
  return p->restitution;
}


/* ==== MdtContactParamsGetRestitutionThreshold ==== */

MeReal MdtContactParamsGetRestitutionThreshold(MdtContactParamsID p)

{
  return p->velThreshold;
}


/* ==== MdtContactParamsGetSoftness ==== */

MeReal MdtContactParamsGetSoftness(MdtContactParamsID p)

{
  return p->softness;
}


/* ==== MdtContactParamsGetMaxAdhesiveForce ==== */

MeReal MdtContactParamsGetMaxAdhesiveForce(MdtContactParamsID p)

{
  return p->max_adhesive_force;
}


/* ==== MdtContactParamsGetPrimaryFriction ==== */

MeReal MdtContactParamsGetPrimaryFriction(MdtContactParamsID p)

{
  return p->friction1;
}


/* ==== MdtContactParamsGetPrimaryFrictionCoeffecient ==== */

MeReal MdtContactParamsGetPrimaryFrictionCoeffecient(MdtContactParamsID p)

{
  return p->frictioncoeff1;
}


/* ==== MdtContactParamsGetPrimarySlip ==== */

MeReal MdtContactParamsGetPrimarySlip(MdtContactParamsID p)

{
  return p->slip1;
}


/* ==== MdtContactParamsGetPrimarySlide ==== */

MeReal MdtContactParamsGetPrimarySlide(MdtContactParamsID p)

{
  return p->slide1;
}


/* ==== MdtContactParamsGetSecondaryFriction ==== */

MeReal MdtContactParamsGetSecondaryFriction(MdtContactParamsID p)

{
  return p->friction2;
}


/* ==== MdtContactParamsGetSecondaryFrictionCoeffecient ==== */

MeReal MdtContactParamsGetSecondaryFrictionCoeffecient(MdtContactParamsID p)

{
  return p->frictioncoeff2;
}


/* ==== MdtContactParamsGetSecondarySlip ==== */

MeReal MdtContactParamsGetSecondarySlip(MdtContactParamsID p)

{
  return p->slip2;
}


/* ==== MdtContactParamsGetSecondarySlide ==== */

MeReal MdtContactParamsGetSecondarySlide(MdtContactParamsID p)

{
  return p->slide2;
}


/* ==== MdtContactParamsSetType ==== */

void __regparm2 MdtContactParamsSetType(MdtContactParamsID p,MdtContactType t)

{
  undefined4 *in_stack_00000004;
  int in_stack_00000008;
  
  if (in_stack_00000008 == 1) {
    *in_stack_00000004 = 1;
  }
  else {
    if (in_stack_00000008 < 2) {
      if (in_stack_00000008 == 0) {
        *in_stack_00000004 = 0;
        return;
      }
    }
    else if (in_stack_00000008 == 2) {
      *in_stack_00000004 = 2;
      return;
    }
    MeWarning(0xc,"Unknown Contact Type, defaulting to zero friction.");
    *in_stack_00000004 = 0;
  }
  return;
}


/* ==== MdtContactParamsSetPrimaryFrictionModel ==== */

void MdtContactParamsSetPrimaryFrictionModel(MdtContactParamsID p,MdtFrictionModel m)

{
  if (m == MdtFrictionModelBox) {
    p->model1 = MdtFrictionModelBox;
  }
  else if (m == MdtFrictionModelNormalForce) {
    p->model1 = MdtFrictionModelNormalForce;
  }
  else {
    MeWarning(0xc,"Unknown Friction Model, defaulting to box friction.");
    p->model1 = MdtFrictionModelBox;
  }
  return;
}


/* ==== MdtContactParamsSetSecondaryFrictionModel ==== */

void __regparm1 MdtContactParamsSetSecondaryFrictionModel(MdtContactParamsID p,MdtFrictionModel m)

{
  int iVar1;
  int in_stack_00000008;
  
  if (in_stack_00000008 == 0) {
    *(undefined4 *)(m + 8) = 0;
  }
  else {
    iVar1 = in_stack_00000008 + -1;
    if (iVar1 == 0) {
      *(undefined4 *)(m + 8) = 1;
    }
    else {
      MeWarning(0xc,"Unknown Friction Model, defaulting to box friction.",iVar1,iVar1,p);
      *(undefined4 *)(m + 8) = 0;
    }
  }
  return;
}


/* ==== MdtContactParamsSetFrictionModel ==== */

void __regparm1 MdtContactParamsSetFrictionModel(MdtContactParamsID p,MdtFrictionModel m)

{
  int in_stack_00000008;
  
  if (in_stack_00000008 != 0) {
    if (in_stack_00000008 == 1) {
      *(undefined4 *)(m + 4) = 1;
      goto LAB_0001029c;
    }
    p = (MdtContactParamsID)MeWarning(0xc,"Unknown Friction Model, defaulting to box friction.",p,p)
    ;
  }
  *(undefined4 *)(m + 4) = 0;
LAB_0001029c:
  if (in_stack_00000008 != 0) {
    if (in_stack_00000008 == 1) {
      *(undefined4 *)(m + 8) = 1;
      return;
    }
    MeWarning(0xc,"Unknown Friction Model, defaulting to box friction.",p,p);
  }
  *(undefined4 *)(m + 8) = 0;
  return;
}


/* ==== MdtContactParamsSetRestitution ==== */

void __regparm3 MdtContactParamsSetRestitution(MdtContactParamsID p,MeReal r)

{
  int in_stack_00000004;
  float in_stack_00000008;
  
  if (in_stack_00000008 < 0.0) {
    MeWarning(0xc,
              "MdtContactParamsSetRestitution: Negative restitution not valid - setting to zero.",r,
              r);
    in_stack_00000008 = 0.0;
  }
  *(float *)(in_stack_00000004 + 0x10) = in_stack_00000008;
  if (ABS(in_stack_00000008) < 1e-06) {
    *(uint *)(in_stack_00000004 + 0xc) = *(uint *)(in_stack_00000004 + 0xc) & 0xfffffffd;
  }
  else {
    *(uint *)(in_stack_00000004 + 0xc) = *(uint *)(in_stack_00000004 + 0xc) | 2;
  }
  return;
}


/* ==== MdtContactParamsSetRestitutionThreshold ==== */

void MdtContactParamsSetRestitutionThreshold(MdtContactParamsID p,MeReal v)

{
  p->velThreshold = v;
  return;
}


/* ==== MdtContactParamsSetSoftness ==== */

void __regparm1 MdtContactParamsSetSoftness(MdtContactParamsID p,MeReal s)

{
  ushort uVar1;
  ushort uVar2;
  float in_stack_00000008;
  
  uVar1 = (ushort)(in_stack_00000008 < 0.0) << 8 | (ushort)NAN(in_stack_00000008) << 10;
  uVar2 = uVar1 | (ushort)(in_stack_00000008 == 0.0) << 0xe;
  if (uVar1 != 0) {
    MeWarning(0xc,"MdtContactParamsSetSoftness: Negative softness not valid - setting to zero.",
              uVar2,uVar2,p);
    in_stack_00000008 = 0.0;
  }
  *(float *)((int)s + 0x18) = in_stack_00000008;
  if (ABS(in_stack_00000008) < 1e-06) {
    *(uint *)((int)s + 0xc) = *(uint *)((int)s + 0xc) & 0xfffffffb;
  }
  else {
    *(uint *)((int)s + 0xc) = *(uint *)((int)s + 0xc) | 4;
  }
  return;
}


/* ==== MdtContactParamsSetMaxAdhesiveForce ==== */

void MdtContactParamsSetMaxAdhesiveForce(MdtContactParamsID p,MeReal s)

{
  p->max_adhesive_force = s;
  if (ABS(s) < 1e-06) {
    p->options = p->options & 0xfffffff7;
  }
  else {
    p->options = p->options | 8;
  }
  return;
}


/* ==== MdtContactParamsSetPrimaryFriction ==== */

void MdtContactParamsSetPrimaryFriction(MdtContactParamsID p,MeReal f)

{
  p->friction1 = f;
  return;
}


/* ==== MdtContactParamsSetPrimaryFrictionCoeffecient ==== */

void MdtContactParamsSetPrimaryFrictionCoeffecient(MdtContactParamsID p,MeReal f)

{
  p->frictioncoeff1 = f;
  return;
}


/* ==== MdtContactParamsSetPrimarySlip ==== */

void __regparm1 MdtContactParamsSetPrimarySlip(MdtContactParamsID p,MeReal s)

{
  ushort uVar1;
  ushort uVar2;
  float in_stack_00000008;
  
  uVar1 = (ushort)(in_stack_00000008 < 0.0) << 8 | (ushort)NAN(in_stack_00000008) << 10;
  uVar2 = uVar1 | (ushort)(in_stack_00000008 == 0.0) << 0xe;
  if (uVar1 != 0) {
    MeWarning(0xc,"MdtContactParamsSetPrimarySlip: Negative slip not valid - setting to zero.",uVar2
              ,uVar2,p);
    in_stack_00000008 = 0.0;
  }
  *(float *)((int)s + 0x28) = in_stack_00000008;
  if (ABS(in_stack_00000008) < 1e-06) {
    *(uint *)((int)s + 0xc) = *(uint *)((int)s + 0xc) & 0xffffffef;
  }
  else {
    *(uint *)((int)s + 0xc) = *(uint *)((int)s + 0xc) | 0x10;
  }
  return;
}


/* ==== MdtContactParamsSetPrimarySlide ==== */

void MdtContactParamsSetPrimarySlide(MdtContactParamsID p,MeReal s)

{
  p->slide1 = s;
  if (ABS(s) < 1e-06) {
    p->options = p->options & 0xffffffbf;
  }
  else {
    p->options = p->options | 0x40;
  }
  return;
}


/* ==== MdtContactParamsSetSecondaryFriction ==== */

void MdtContactParamsSetSecondaryFriction(MdtContactParamsID p,MeReal f)

{
  p->friction2 = f;
  return;
}


/* ==== MdtContactParamsSetSecondaryFrictionCoeffecient ==== */

void MdtContactParamsSetSecondaryFrictionCoeffecient(MdtContactParamsID p,MeReal f)

{
  p->frictioncoeff2 = f;
  return;
}


/* ==== MdtContactParamsSetSecondarySlip ==== */

void __regparm3 MdtContactParamsSetSecondarySlip(MdtContactParamsID p,MeReal s)

{
  int in_stack_00000004;
  float in_stack_00000008;
  
  if (in_stack_00000008 < 0.0) {
    MeWarning(0xc,"MdtContactParamsSetSecondarySlip: Negative slip not valid - setting to zero.",s,s
             );
    in_stack_00000008 = 0.0;
  }
  *(float *)(in_stack_00000004 + 0x34) = in_stack_00000008;
  if (ABS(in_stack_00000008) < 1e-06) {
    *(uint *)(in_stack_00000004 + 0xc) = *(uint *)(in_stack_00000004 + 0xc) & 0xffffffdf;
  }
  else {
    *(uint *)(in_stack_00000004 + 0xc) = *(uint *)(in_stack_00000004 + 0xc) | 0x20;
  }
  return;
}


/* ==== MdtContactParamsSetSecondarySlide ==== */

void MdtContactParamsSetSecondarySlide(MdtContactParamsID p,MeReal s)

{
  p->slide2 = s;
  if (ABS(s) < 1e-06) {
    p->options = p->options & 0xffffff7f;
  }
  else {
    p->options = p->options | 0x80;
  }
  return;
}


/* ==== MdtContactParamsSetSlip ==== */

void __regparm3 MdtContactParamsSetSlip(MdtContactParamsID p,MeReal s)

{
  ushort uVar1;
  undefined2 uVar4;
  uint uVar2;
  undefined4 uVar3;
  undefined2 extraout_var;
  float fVar5;
  int in_stack_00000004;
  float in_stack_00000008;
  
  uVar4 = (undefined2)((uint)p >> 0x10);
  fVar5 = in_stack_00000008;
  if (in_stack_00000008 < 0.0) {
    MeWarning(0xc,"MdtContactParamsSetPrimarySlip: Negative slip not valid - setting to zero.");
    fVar5 = 0.0;
    uVar4 = extraout_var;
  }
  *(float *)(in_stack_00000004 + 0x28) = fVar5;
  uVar2 = (uint)CONCAT21(uVar4,ABS(fVar5) < 1e-06) << 8;
  if (ABS(fVar5) < 1e-06) {
    *(uint *)(in_stack_00000004 + 0xc) = *(uint *)(in_stack_00000004 + 0xc) & 0xffffffef;
  }
  else {
    uVar2 = *(uint *)(in_stack_00000004 + 0xc) | 0x10;
    *(uint *)(in_stack_00000004 + 0xc) = uVar2;
  }
  uVar1 = (ushort)(in_stack_00000008 < 0.0) << 8 | (ushort)NAN(in_stack_00000008) << 10;
  uVar3 = CONCAT22((short)(uVar2 >> 0x10),uVar1 | (ushort)(in_stack_00000008 == 0.0) << 0xe);
  if (uVar1 != 0) {
    MeWarning(0xc,"MdtContactParamsSetSecondarySlip: Negative slip not valid - setting to zero.",
              uVar3,uVar3);
    in_stack_00000008 = 0.0;
  }
  *(float *)(in_stack_00000004 + 0x34) = in_stack_00000008;
  if (ABS(in_stack_00000008) < 1e-06) {
    *(uint *)(in_stack_00000004 + 0xc) = *(uint *)(in_stack_00000004 + 0xc) & 0xffffffdf;
  }
  else {
    *(uint *)(in_stack_00000004 + 0xc) = *(uint *)(in_stack_00000004 + 0xc) | 0x20;
  }
  return;
}


/* ==== MdtContactParamsSetFriction ==== */

void __regparm1 MdtContactParamsSetFriction(MdtContactParamsID p,MeReal f)

{
  ushort uVar1;
  ushort uVar2;
  float in_stack_00000008;
  
  uVar1 = (ushort)(in_stack_00000008 < 0.0) << 8 | (ushort)NAN(in_stack_00000008) << 10;
  uVar2 = uVar1 | (ushort)(in_stack_00000008 == 0.0) << 0xe;
  if (uVar1 != 0) {
    MeWarning(0xc,"MdtContactParamsSetFriction: Negative friction not valid - setting to zero.",
              uVar2,uVar2,p);
    in_stack_00000008 = 0.0;
  }
  *(float *)((int)f + 0x20) = in_stack_00000008;
  *(float *)((int)f + 0x2c) = in_stack_00000008;
  return;
}


/* ==== MdtContactParamsSetFrictionCoeffecient ==== */

void __regparm1 MdtContactParamsSetFrictionCoeffecient(MdtContactParamsID p,MeReal f)

{
  ushort uVar1;
  ushort uVar2;
  float in_stack_00000008;
  
  uVar1 = (ushort)(in_stack_00000008 < 0.0) << 8 | (ushort)NAN(in_stack_00000008) << 10;
  uVar2 = uVar1 | (ushort)(in_stack_00000008 == 0.0) << 0xe;
  if (uVar1 != 0) {
    MeWarning(0xc,"MdtContactParamsSetPrimarySlip: Negative friction not valid - setting to zero.",
              uVar2,uVar2,p);
    in_stack_00000008 = 0.0;
  }
  *(float *)((int)f + 0x24) = in_stack_00000008;
  *(float *)((int)f + 0x30) = in_stack_00000008;
  return;
}


