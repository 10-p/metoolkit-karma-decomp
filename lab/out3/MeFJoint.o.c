/* ==== MeFJointCreateCopy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFJoint * MeFJointCreateCopy(MeFJoint *fj,MeBool recurse)

{
  MeBool MVar1;
  MeFJoint *pMVar2;
  
                    /* Unresolved local var: MeFJoint * copy@[DW_OP_reg3(EBX)]
                       Unresolved local var: int i@[???] */
  pMVar2 = (MeFJoint *)(*__FSetStringProperty)(0xd0);
  pMVar2->type = fj->type;
  _FSetStringProperty(pMVar2,fj->id);
  _FSetStringProperty(pMVar2->part,fj->part[0]);
  _FSetStringProperty(pMVar2->part + 1,fj->part[1]);
  pMVar2->pos[0][0] = fj->pos[0][0];
  pMVar2->pos[0][1] = fj->pos[0][1];
  pMVar2->pos[0][2] = fj->pos[0][2];
  pMVar2->pax[0][0] = fj->pax[0][0];
  pMVar2->pax[0][1] = fj->pax[0][1];
  pMVar2->pax[0][2] = fj->pax[0][2];
  pMVar2->oax[0][0] = fj->oax[0][0];
  pMVar2->oax[0][1] = fj->oax[0][1];
  pMVar2->oax[0][2] = fj->oax[0][2];
  pMVar2->pos[1][0] = fj->pos[1][0];
  pMVar2->pos[1][1] = fj->pos[1][1];
  pMVar2->pos[1][2] = fj->pos[1][2];
  pMVar2->pax[1][0] = fj->pax[1][0];
  pMVar2->pax[1][1] = fj->pax[1][1];
  pMVar2->pax[1][2] = fj->pax[1][2];
  pMVar2->oax[1][0] = fj->oax[1][0];
  pMVar2->oax[1][1] = fj->oax[1][1];
  pMVar2->oax[1][2] = fj->oax[1][2];
  pMVar2->stop[0] = fj->stop[0];
  pMVar2->stop[1] = fj->stop[1];
  pMVar2->stiff[0] = fj->stiff[0];
  pMVar2->stop[2] = fj->stop[2];
  pMVar2->stiff[1] = fj->stiff[1];
  pMVar2->stiff[2] = fj->stiff[2];
  pMVar2->stiff[3] = fj->stiff[3];
  pMVar2->stiff[4] = fj->stiff[4];
  pMVar2->damp[0] = fj->damp[0];
  pMVar2->stiff[5] = fj->stiff[5];
  pMVar2->damp[1] = fj->damp[1];
  pMVar2->damp[2] = fj->damp[2];
  pMVar2->damp[3] = fj->damp[3];
  pMVar2->damp[4] = fj->damp[4];
  MVar1 = fj->bLimited[0];
  pMVar2->damp[5] = fj->damp[5];
  pMVar2->bLimited[0] = MVar1;
  MVar1 = fj->bMotorized[0];
  pMVar2->strength[0] = fj->strength[0];
  pMVar2->bMotorized[0] = MVar1;
  pMVar2->strength[1] = fj->strength[1];
  pMVar2->strength[2] = fj->strength[2];
  pMVar2->strength[3] = fj->strength[3];
  pMVar2->strength[4] = fj->strength[4];
  pMVar2->desVel[0] = fj->desVel[0];
  pMVar2->strength[5] = fj->strength[5];
  pMVar2->desVel[1] = fj->desVel[1];
  pMVar2->special_i[0] = fj->special_i[0];
  pMVar2->special_f[0] = fj->special_f[0];
  pMVar2->special_i[1] = fj->special_i[1];
  pMVar2->special_b[0] = fj->special_b[0];
  return pMVar2;
}


/* ==== MeFJointCopy ==== */

void MeFJointCopy(MeFJoint *dst,MeFJoint *src,MeBool copyAxes)

{
                    /* Unresolved local var: int i@[???] */
  dst->type = src->type;
  if (copyAxes != 0) {
    dst->pos[0][0] = src->pos[0][0];
    dst->pos[0][1] = src->pos[0][1];
    dst->pos[0][2] = src->pos[0][2];
    dst->pax[0][0] = src->pax[0][0];
    dst->pax[0][1] = src->pax[0][1];
    dst->pax[0][2] = src->pax[0][2];
    dst->oax[0][0] = src->oax[0][0];
    dst->oax[0][1] = src->oax[0][1];
    dst->oax[0][2] = src->oax[0][2];
    dst->pos[1][0] = src->pos[1][0];
    dst->pos[1][1] = src->pos[1][1];
    dst->pos[1][2] = src->pos[1][2];
    dst->pax[1][0] = src->pax[1][0];
    dst->pax[1][1] = src->pax[1][1];
    dst->pax[1][2] = src->pax[1][2];
    dst->oax[1][0] = src->oax[1][0];
    dst->oax[1][1] = src->oax[1][1];
    dst->oax[1][2] = src->oax[1][2];
  }
  dst->stop[0] = src->stop[0];
  dst->stop[1] = src->stop[1];
  dst->stop[2] = src->stop[2];
  dst->stiff[0] = src->stiff[0];
  dst->stiff[1] = src->stiff[1];
  dst->stiff[2] = src->stiff[2];
  dst->stiff[3] = src->stiff[3];
  dst->stiff[4] = src->stiff[4];
  dst->stiff[5] = src->stiff[5];
  dst->damp[0] = src->damp[0];
  dst->damp[1] = src->damp[1];
  dst->damp[2] = src->damp[2];
  dst->damp[3] = src->damp[3];
  dst->damp[4] = src->damp[4];
  dst->damp[5] = src->damp[5];
  dst->bLimited[0] = src->bLimited[0];
  dst->bMotorized[0] = src->bMotorized[0];
  dst->strength[0] = src->strength[0];
  dst->strength[1] = src->strength[1];
  dst->strength[2] = src->strength[2];
  dst->strength[3] = src->strength[3];
  dst->strength[4] = src->strength[4];
  dst->strength[5] = src->strength[5];
  dst->desVel[0] = src->desVel[0];
  dst->desVel[1] = src->desVel[1];
  dst->special_f[0] = src->special_f[0];
  dst->special_i[0] = src->special_i[0];
  dst->special_i[1] = src->special_i[1];
  dst->special_b[0] = src->special_b[0];
  return;
}


/* ==== MeFJointGetProperty1f ==== */

MeBool MeFJointGetProperty1f(MeFJoint *joint,MeFJointProperty p,MeReal *x)

{
  MeReal MVar1;
  
  switch(p) {
  case kMeFJointPropertyStop1:
    MVar1 = joint->stop[0];
    break;
  case kMeFJointPropertyStop2:
    MVar1 = joint->stop[1];
    break;
  case kMeFJointPropertyStop3:
    MVar1 = joint->stop[2];
    break;
  case kMeFJointPropertyStiffness1:
    MVar1 = joint->stiff[0];
    break;
  case kMeFJointPropertyStiffness2:
    MVar1 = joint->stiff[1];
    break;
  case kMeFJointPropertyStiffness3:
    MVar1 = joint->stiff[2];
    break;
  case kMeFJointPropertyStiffness4:
    MVar1 = joint->stiff[3];
    break;
  case kMeFJointPropertyStiffness5:
    MVar1 = joint->stiff[4];
    break;
  case kMeFJointPropertyStiffness6:
    MVar1 = joint->stiff[5];
    break;
  case kMeFJointPropertyDamping1:
    MVar1 = joint->damp[0];
    break;
  case kMeFJointPropertyDamping2:
    MVar1 = joint->damp[1];
    break;
  case kMeFJointPropertyDamping3:
    MVar1 = joint->damp[2];
    break;
  case kMeFJointPropertyDamping4:
    MVar1 = joint->damp[3];
    break;
  case kMeFJointPropertyDamping5:
    MVar1 = joint->damp[4];
    break;
  case kMeFJointPropertyDamping6:
    MVar1 = joint->damp[5];
    break;
  default:
    return 0;
  case kMeFJointPropertyStrength1:
    MVar1 = joint->strength[0];
    break;
  case kMeFJointPropertyStrength2:
    MVar1 = joint->strength[1];
    break;
  case kMeFJointPropertyStrength3:
    MVar1 = joint->strength[2];
    break;
  case kMeFJointPropertyStrength4:
    MVar1 = joint->strength[3];
    break;
  case kMeFJointPropertyStrength5:
    MVar1 = joint->strength[4];
    break;
  case kMeFJointPropertyStrength6:
    MVar1 = joint->strength[5];
    break;
  case kMeFJointPropertyDesiredVelocity1:
    MVar1 = joint->desVel[0];
    break;
  case kMeFJointPropertyDesiredVelocity2:
    MVar1 = joint->desVel[1];
    break;
  case kMeFJointPropertySpecialFloat1:
    MVar1 = joint->special_f[0];
  }
  *x = MVar1;
  return 1;
}


/* ==== MeFJointSetPerTypeDefaults ==== */

void MeFJointSetPerTypeDefaults(MeFJoint *fj)

{
  MeFJointProperty p;
  MeReal MVar1;
  
  fj->stop[0] = 0.0;
  fj->stop[2] = 0.0;
  fj->stop[1] = 0.0;
  fj->stiff[2] = 0.0;
  fj->stiff[1] = 0.0;
  fj->stiff[0] = 0.0;
  fj->stiff[5] = 0.0;
  fj->stiff[4] = 0.0;
  fj->stiff[3] = 0.0;
  fj->damp[2] = 0.0;
  fj->damp[1] = 0.0;
  fj->damp[0] = 0.0;
  fj->damp[5] = 0.0;
  fj->damp[4] = 0.0;
  fj->damp[3] = 0.0;
  fj->bLimited[0] = 0;
  fj->bMotorized[0] = 0;
  fj->strength[2] = 0.0;
  fj->strength[1] = 0.0;
  fj->strength[0] = 0.0;
  fj->strength[5] = 0.0;
  fj->strength[4] = 0.0;
  fj->strength[3] = 0.0;
  fj->desVel[1] = 0.0;
  fj->desVel[0] = 0.0;
  fj->special_f[0] = 0.0;
  fj->special_i[0] = 0;
  fj->special_i[1] = 0;
  fj->special_b[0] = 0;
  switch(fj->type) {
  default:
    goto switchD_00010544_caseD_0;
  case kMeFJointTypeCarwheel:
    MeFJointSetProperty1f(fj,kMeFJointPropertyDesiredVelocity1,1.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStrength1,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyDesiredVelocity2,1.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStrength2,1000.0);
    MeFJointSetProperty1b(fj,kMeFJointPropertySpecialBool1,0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyDamping1,1.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStop1,0.5);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStop2,-0.5);
    MeFJointSetProperty1f(fj,kMeFJointPropertySpecialFloat1,0.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness1,1000.0);
    MVar1 = 1000.0;
    p = kMeFJointPropertyStiffness2;
    break;
  case kMeFJointTypeHinge:
    MeFJointSetProperty1f(fj,kMeFJointPropertyStop1,1.5707964);
    MVar1 = -1.5707964;
    goto LAB_00010561;
  case kMeFJointTypeConeLimit:
    MeFJointSetProperty1f(fj,kMeFJointPropertyStop1,1.5707964);
    MVar1 = 1000.0;
    p = kMeFJointPropertyStiffness1;
    break;
  case kMeFJointTypeRpro:
    MeFJointSetProperty1f(fj,kMeFJointPropertyStrength1,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStrength2,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStrength3,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStrength4,0.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStrength5,0.0);
    MVar1 = 0.0;
    p = kMeFJointPropertyStrength6;
    break;
  case kMeFJointTypePrismatic:
    MeFJointSetProperty1f(fj,kMeFJointPropertyStop1,0.5);
    MVar1 = -0.5;
LAB_00010561:
    MeFJointSetProperty1f(fj,kMeFJointPropertyStop2,MVar1);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness1,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness2,1000.0);
    MeFJointSetProperty1b(fj,kMeFJointPropertyLimited1,0);
    MeFJointSetProperty1b(fj,kMeFJointPropertyMotorized1,0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyDesiredVelocity1,1.0);
    MVar1 = 1000.0;
    p = kMeFJointPropertyStrength1;
    break;
  case kMeFJointTypeSkeletal:
    MeFJointSetProperty1i(fj,kMeFJointPropertySpecialInt1,2);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStop1,0.7853982);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStop2,0.7853982);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness1,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyDamping1,1.0);
    MeFJointSetProperty1i(fj,kMeFJointPropertySpecialInt2,2);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStop3,1.5707964);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness2,1000.0);
    MVar1 = 1.0;
    p = kMeFJointPropertyDamping2;
    break;
  case kMeFJointTypeAngular3:
    MeFJointSetProperty1b(fj,kMeFJointPropertySpecialBool1,0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness1,1000.0);
    MVar1 = 1.0;
    p = kMeFJointPropertyDamping1;
    break;
  case kMeFJointTypeSpring6:
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness1,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness2,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness3,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness4,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness5,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyStiffness6,1000.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyDamping1,1.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyDamping2,1.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyDamping3,1.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyDamping4,1.0);
    MeFJointSetProperty1f(fj,kMeFJointPropertyDamping5,1.0);
    MVar1 = 1.0;
    p = kMeFJointPropertyDamping6;
  }
  MeFJointSetProperty1f(fj,p,MVar1);
switchD_00010544_caseD_0:
  return;
}


/* ==== MeFJointRename ==== */

void MeFJointRename(MeFJoint *fj,char *newName)

{
  int iVar1;
  
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar1 = strcmp(fj->id,newName);
  if (iVar1 != 0) {
    if (fj->asset != (MeFAsset *)0x0) {
      MeHashDelete(fj->id,fj->asset->nameToJoint);
    }
    _FSetStringProperty(fj,newName);
    if (fj->asset != (MeFAsset *)0x0) {
      MeHashInsert(fj->id,fj,fj->asset->nameToJoint);
    }
  }
  return;
}


/* ==== MeFJointSetProperty1i ==== */

void MeFJointSetProperty1i(MeFJoint *joint,MeFJointProperty p,int i)

{
  if (p == kMeFJointPropertySpecialInt1) {
    joint->special_i[0] = i;
  }
  else if (p == kMeFJointPropertySpecialInt2) {
    joint->special_i[1] = i;
  }
  return;
}


/* ==== MeFJointSetProperty1b ==== */

void MeFJointSetProperty1b(MeFJoint *joint,MeFJointProperty p,MeBool b)

{
  if (p == kMeFJointPropertyMotorized1) {
    joint->bMotorized[0] = b;
  }
  else if (p < kMeFJointPropertyStrength1) {
    if (p == kMeFJointPropertyLimited1) {
      joint->bLimited[0] = b;
    }
  }
  else if (p == kMeFJointPropertySpecialBool1) {
    joint->special_b[0] = b;
  }
  return;
}


/* ==== MeFJointSetProperty1f ==== */

void MeFJointSetProperty1f(MeFJoint *joint,MeFJointProperty p,MeReal x)

{
  switch(p) {
  case kMeFJointPropertyStop1:
    joint->stop[0] = x;
    break;
  case kMeFJointPropertyStop2:
    joint->stop[1] = x;
    break;
  case kMeFJointPropertyStop3:
    joint->stop[2] = x;
    break;
  case kMeFJointPropertyStiffness1:
    joint->stiff[0] = x;
    break;
  case kMeFJointPropertyStiffness2:
    joint->stiff[1] = x;
    break;
  case kMeFJointPropertyStiffness3:
    joint->stiff[2] = x;
    break;
  case kMeFJointPropertyStiffness4:
    joint->stiff[3] = x;
    break;
  case kMeFJointPropertyStiffness5:
    joint->stiff[4] = x;
    break;
  case kMeFJointPropertyStiffness6:
    joint->stiff[5] = x;
    break;
  case kMeFJointPropertyDamping1:
    joint->damp[0] = x;
    break;
  case kMeFJointPropertyDamping2:
    joint->damp[1] = x;
    break;
  case kMeFJointPropertyDamping3:
    joint->damp[2] = x;
    break;
  case kMeFJointPropertyDamping4:
    joint->damp[3] = x;
    break;
  case kMeFJointPropertyDamping5:
    joint->damp[4] = x;
    break;
  case kMeFJointPropertyDamping6:
    joint->damp[5] = x;
    break;
  case kMeFJointPropertyStrength1:
    joint->strength[0] = x;
    break;
  case kMeFJointPropertyStrength2:
    joint->strength[1] = x;
    break;
  case kMeFJointPropertyStrength3:
    joint->strength[2] = x;
    break;
  case kMeFJointPropertyStrength4:
    joint->strength[3] = x;
    break;
  case kMeFJointPropertyStrength5:
    joint->strength[4] = x;
    break;
  case kMeFJointPropertyStrength6:
    joint->strength[5] = x;
    break;
  case kMeFJointPropertyDesiredVelocity1:
    joint->desVel[0] = x;
    break;
  case kMeFJointPropertyDesiredVelocity2:
    joint->desVel[1] = x;
    break;
  case kMeFJointPropertySpecialFloat1:
    joint->special_f[0] = x;
  }
  return;
}


/* ==== MeFJointCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFJoint * MeFJointCreate(char *name,MeFJointType type)

{
  MeFJoint *fj;
  
                    /* Unresolved local var: MeFJoint * fj@[DW_OP_reg3(EBX)] */
  fj = (MeFJoint *)(*__FSetStringProperty)(0xd0);
  _FSetStringProperty(fj,name);
  fj->type = type;
                    /* Unresolved local var: int i@[???] */
  fj->part[1] = (char *)0x0;
  fj->part[0] = (char *)0x0;
  fj->asset = (MeFAsset *)0x0;
  fj->pos[0][0] = 0.0;
  fj->pos[0][1] = 0.0;
  fj->pos[0][2] = 0.0;
  fj->pax[0][0] = 1.0;
  fj->pax[0][1] = 0.0;
  fj->pax[0][2] = 0.0;
  fj->oax[0][0] = 0.0;
  fj->oax[0][1] = 0.0;
  fj->oax[0][2] = 1.0;
  fj->pos[1][0] = 0.0;
  fj->pos[1][1] = 0.0;
  fj->pos[1][2] = 0.0;
  fj->pax[1][0] = 1.0;
  fj->pax[1][1] = 0.0;
  fj->pax[1][2] = 0.0;
  fj->oax[1][0] = 0.0;
  fj->oax[1][1] = 0.0;
  fj->oax[1][2] = 1.0;
  MeFJointSetPerTypeDefaults(fj);
  return fj;
}


/* ==== MeFJointDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFJointDestroy(MeFJoint *fj)

{
  (*_MeHashInsert)(fj->part[0]);
  if (fj->part[1] != (char *)0x0) {
    (*_MeHashInsert)(fj->part[1]);
  }
  (*_MeHashInsert)(fj->id);
  (*_MeHashInsert)(fj);
  return;
}


/* ==== MeFJointGetName ==== */

char * MeFJointGetName(MeFJoint *joint)

{
  return joint->id;
}


/* ==== MeFJointGetType ==== */

MeFJointType MeFJointGetType(MeFJoint *joint)

{
  return joint->type;
}


/* ==== MeFJointGetPartName ==== */

char * MeFJointGetPartName(MeFJoint *joint,int index)

{
  return joint->part[index];
}


/* ==== MeFJointGetPart ==== */

MeFAssetPart * MeFJointGetPart(MeFJoint *joint,int index)

{
  MeFAssetPart *pMVar1;
  
                    /* Unresolved local var: MeFAssetPart * p@[DW_OP_reg1(ECX)] */
  pMVar1 = (MeFAssetPart *)0x0;
  if (joint->asset == (MeFAsset *)0x0) {
    pMVar1 = (MeFAssetPart *)0x0;
  }
  else if (joint->part[index] != (char *)0x0) {
    pMVar1 = MeFAssetLookupPart(joint->asset,joint->part[index]);
  }
  return pMVar1;
}


/* ==== MeFJointGetPosition ==== */

void MeFJointGetPosition(MeFJoint *j,int index,MeReal *pos)

{
  *pos = j->pos[index][0];
  pos[1] = j->pos[index][1];
  pos[2] = j->pos[index][2];
  return;
}


/* ==== MeFJointGetPrimaryAxis ==== */

void MeFJointGetPrimaryAxis(MeFJoint *j,int index,MeReal *axis)

{
  *axis = j->pax[index][0];
  axis[1] = j->pax[index][1];
  axis[2] = j->pax[index][2];
  return;
}


/* ==== MeFJointGetOrthogonalAxis ==== */

void MeFJointGetOrthogonalAxis(MeFJoint *j,int index,MeReal *axis)

{
  *axis = j->oax[index][0];
  axis[1] = j->oax[index][1];
  axis[2] = j->oax[index][2];
  return;
}


/* ==== MeFJointGetProperty1i ==== */

MeBool MeFJointGetProperty1i(MeFJoint *joint,MeFJointProperty p,int *i)

{
  int iVar1;
  
  if (p == kMeFJointPropertySpecialInt1) {
    iVar1 = joint->special_i[0];
  }
  else {
    if (p != kMeFJointPropertySpecialInt2) {
      return 0;
    }
    iVar1 = joint->special_i[1];
  }
  *i = iVar1;
  return 1;
}


/* ==== MeFJointGetProperty1ui ==== */

/* WARNING: Unknown calling convention */

MeBool MeFJointGetProperty1ui(MeFJoint *joint,MeFJointProperty p,uint *u)

{
  return 0;
}


/* ==== MeFJointGetProperty1b ==== */

MeBool MeFJointGetProperty1b(MeFJoint *joint,MeFJointProperty p,MeBool *b)

{
  MeBool MVar1;
  
  if (p == kMeFJointPropertyMotorized1) {
    MVar1 = joint->bMotorized[0];
LAB_00010cc6:
    *b = MVar1;
    MVar1 = 1;
  }
  else {
    if (p < kMeFJointPropertyStrength1) {
      if (p == kMeFJointPropertyLimited1) {
        MVar1 = joint->bLimited[0];
        goto LAB_00010cc6;
      }
    }
    else if (p == kMeFJointPropertySpecialBool1) {
      MVar1 = joint->special_b[0];
      goto LAB_00010cc6;
    }
    MVar1 = 0;
  }
  return MVar1;
}


/* ==== MeFJointSetGenericDefaults ==== */

void MeFJointSetGenericDefaults(MeFJoint *fj)

{
                    /* Unresolved local var: int i@[???] */
  fj->pos[0][0] = 0.0;
  fj->pos[0][1] = 0.0;
  fj->pos[0][2] = 0.0;
  fj->pax[0][0] = 1.0;
  fj->pax[0][1] = 0.0;
  fj->pax[0][2] = 0.0;
  fj->oax[0][0] = 0.0;
  fj->oax[0][1] = 0.0;
  fj->oax[0][2] = 1.0;
  fj->part[1] = (char *)0x0;
  fj->part[0] = (char *)0x0;
  fj->asset = (MeFAsset *)0x0;
  fj->pos[1][0] = 0.0;
  fj->pos[1][1] = 0.0;
  fj->pos[1][2] = 0.0;
  fj->pax[1][0] = 1.0;
  fj->pax[1][1] = 0.0;
  fj->pax[1][2] = 0.0;
  fj->oax[1][0] = 0.0;
  fj->oax[1][1] = 0.0;
  fj->oax[1][2] = 1.0;
  return;
}


/* ==== MeFJointSetType ==== */

void MeFJointSetType(MeFJoint *fj,MeFJointType type)

{
  fj->type = type;
  MeFJointSetPerTypeDefaults(fj);
  return;
}


/* ==== MeFJointSetPart ==== */

void MeFJointSetPart(MeFJoint *j,MeFAssetPart *part,int index)

{
  char *pcVar1;
  
                    /* Unresolved local var: char * name@[DW_OP_reg2(EDX)] */
  pcVar1 = (char *)0x0;
  if (part != (MeFAssetPart *)0x0) {
    pcVar1 = part->id;
  }
  if ((index == 0) && (pcVar1 == (char *)0x0)) {
    return;
  }
  _FSetStringProperty(j->part + index,pcVar1);
  return;
}


/* ==== MeFJointSetPartByName ==== */

void MeFJointSetPartByName(MeFJoint *j,char *part,int index)

{
  if ((index == 0) && (part == (char *)0x0)) {
    return;
  }
  _FSetStringProperty(j->part + index,part);
  return;
}


/* ==== MeFJointSetPosition ==== */

void MeFJointSetPosition(MeFJoint *j,int index,MeReal x,MeReal y,MeReal z)

{
  j->pos[index][0] = x;
  j->pos[index][1] = y;
  j->pos[index][2] = z;
  return;
}


/* ==== MeFJointSetPrimaryAxis ==== */

void MeFJointSetPrimaryAxis(MeFJoint *j,int index,MeReal x,MeReal y,MeReal z)

{
  j->pax[index][0] = x;
  j->pax[index][1] = y;
  j->pax[index][2] = z;
  return;
}


/* ==== MeFJointSetOrthogonalAxis ==== */

void MeFJointSetOrthogonalAxis(MeFJoint *j,int index,MeReal x,MeReal y,MeReal z)

{
  j->oax[index][0] = x;
  j->oax[index][1] = y;
  j->oax[index][2] = z;
  return;
}


/* ==== MeFJointScale ==== */

void MeFJointScale(MeFJoint *j,MeReal scale)

{
  float fVar1;
  
  j->pos[0][0] = j->pos[0][0] * scale;
  j->pos[0][1] = j->pos[0][1] * scale;
  j->pos[0][2] = j->pos[0][2] * scale;
  j->pos[1][0] = j->pos[1][0] * scale;
  j->pos[1][1] = j->pos[1][1] * scale;
  j->pos[1][2] = j->pos[1][2] * scale;
  if (j->type == kMeFJointTypePrismatic) {
    fVar1 = j->stop[1];
    j->stop[0] = j->stop[0] * scale;
    j->stop[1] = scale * fVar1;
  }
  return;
}


/* ==== MeFJointSetProperty1ui ==== */

/* WARNING: Unknown calling convention */

void MeFJointSetProperty1ui(MeFJoint *joint,MeFJointProperty p,uint i)

{
  return;
}


