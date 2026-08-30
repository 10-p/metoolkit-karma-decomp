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

void MdtContactParamsSetType(MdtContactParamsID p,MdtContactType t)

{
  if (t == MdtContactTypeFriction1D) {
    p->type = MdtContactTypeFriction1D;
  }
  else {
    if (t < MdtContactTypeFriction2D) {
      if (t == MdtContactTypeFrictionZero) {
        p->type = MdtContactTypeFrictionZero;
        return;
      }
    }
    else if (t == MdtContactTypeFriction2D) {
      p->type = MdtContactTypeFriction2D;
      return;
    }
    MeWarning(0xc,"Unknown Contact Type, defaulting to zero friction.");
    p->type = MdtContactTypeFrictionZero;
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

void MdtContactParamsSetSecondaryFrictionModel(MdtContactParamsID p,MdtFrictionModel m)

{
  if (m == MdtFrictionModelBox) {
    p->model2 = MdtFrictionModelBox;
  }
  else if (m == MdtFrictionModelNormalForce) {
    p->model2 = MdtFrictionModelNormalForce;
  }
  else {
    MeWarning(0xc,"Unknown Friction Model, defaulting to box friction.");
    p->model2 = MdtFrictionModelBox;
  }
  return;
}


/* ==== MdtContactParamsSetFrictionModel ==== */

void MdtContactParamsSetFrictionModel(MdtContactParamsID p,MdtFrictionModel m)

{
  if (m != MdtFrictionModelBox) {
    if (m == MdtFrictionModelNormalForce) {
      p->model1 = MdtFrictionModelNormalForce;
      goto LAB_0001029c;
    }
    MeWarning(0xc,"Unknown Friction Model, defaulting to box friction.");
  }
  p->model1 = MdtFrictionModelBox;
LAB_0001029c:
  if (m != MdtFrictionModelBox) {
    if (m == MdtFrictionModelNormalForce) {
      p->model2 = MdtFrictionModelNormalForce;
      return;
    }
    MeWarning(0xc,"Unknown Friction Model, defaulting to box friction.");
  }
  p->model2 = MdtFrictionModelBox;
  return;
}


/* ==== MdtContactParamsSetRestitution ==== */

void MdtContactParamsSetRestitution(MdtContactParamsID p,MeReal r)

{
  if (r < 0.0) {
    MeWarning(0xc,
              "MdtContactParamsSetRestitution: Negative restitution not valid - setting to zero.");
    r = 0.0;
  }
  p->restitution = r;
  if (ABS(r) < 1e-06) {
    p->options = p->options & 0xfffffffd;
  }
  else {
    p->options = p->options | 2;
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

void MdtContactParamsSetSoftness(MdtContactParamsID p,MeReal s)

{
  if (s < 0.0) {
    MeWarning(0xc,"MdtContactParamsSetSoftness: Negative softness not valid - setting to zero.");
    s = 0.0;
  }
  p->softness = s;
  if (ABS(s) < 1e-06) {
    p->options = p->options & 0xfffffffb;
  }
  else {
    p->options = p->options | 4;
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

void MdtContactParamsSetPrimarySlip(MdtContactParamsID p,MeReal s)

{
  if (s < 0.0) {
    MeWarning(0xc,"MdtContactParamsSetPrimarySlip: Negative slip not valid - setting to zero.");
    s = 0.0;
  }
  p->slip1 = s;
  if (ABS(s) < 1e-06) {
    p->options = p->options & 0xffffffef;
  }
  else {
    p->options = p->options | 0x10;
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

void MdtContactParamsSetSecondarySlip(MdtContactParamsID p,MeReal s)

{
  if (s < 0.0) {
    MeWarning(0xc,"MdtContactParamsSetSecondarySlip: Negative slip not valid - setting to zero.");
    s = 0.0;
  }
  p->slip2 = s;
  if (ABS(s) < 1e-06) {
    p->options = p->options & 0xffffffdf;
  }
  else {
    p->options = p->options | 0x20;
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

void MdtContactParamsSetSlip(MdtContactParamsID p,MeReal s)

{
  MeReal MVar1;
  
  MVar1 = s;
  if (s < 0.0) {
    MeWarning(0xc,"MdtContactParamsSetPrimarySlip: Negative slip not valid - setting to zero.");
    MVar1 = 0.0;
  }
  p->slip1 = MVar1;
  if (ABS(MVar1) < 1e-06) {
    p->options = p->options & 0xffffffef;
  }
  else {
    p->options = p->options | 0x10;
  }
  if (s < 0.0) {
    MeWarning(0xc,"MdtContactParamsSetSecondarySlip: Negative slip not valid - setting to zero.");
    s = 0.0;
  }
  p->slip2 = s;
  if (ABS(s) < 1e-06) {
    p->options = p->options & 0xffffffdf;
  }
  else {
    p->options = p->options | 0x20;
  }
  return;
}


/* ==== MdtContactParamsSetFriction ==== */

void MdtContactParamsSetFriction(MdtContactParamsID p,MeReal f)

{
  if (f < 0.0) {
    MeWarning(0xc,"MdtContactParamsSetFriction: Negative friction not valid - setting to zero.");
    f = 0.0;
  }
  p->friction1 = f;
  p->friction2 = f;
  return;
}


/* ==== MdtContactParamsSetFrictionCoeffecient ==== */

void MdtContactParamsSetFrictionCoeffecient(MdtContactParamsID p,MeReal f)

{
  if (f < 0.0) {
    MeWarning(0xc,"MdtContactParamsSetPrimarySlip: Negative friction not valid - setting to zero.");
    f = 0.0;
  }
  p->frictioncoeff1 = f;
  p->frictioncoeff2 = f;
  return;
}


