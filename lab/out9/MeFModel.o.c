/* ==== MeFModelRename ==== */

void MeFModelRename(MeFModel *fm,char *newName)

{
  int iVar1;
  
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar1 = strcmp(fm->id,newName);
  if (iVar1 != 0) {
    if (fm->asset != (MeFAsset *)0x0) {
      MeFAssetResolveModelReferences(fm->asset,fm->id,newName);
      MeHashDelete(fm->id,fm->asset->nameToModel);
    }
    _FSetStringProperty(fm,newName);
    if (fm->asset != (MeFAsset *)0x0) {
      MeHashInsert(fm->id,fm,fm->asset->nameToModel);
    }
  }
  return;
}


/* ==== MeFModelCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFModel * MeFModelCreate(char *name,MeFModelType type)

{
  MeFModel *pMVar1;
  
                    /* Unresolved local var: MeFModel * fm@[DW_OP_reg3(EBX)] */
  pMVar1 = (*_MeFAssetLookupGeometry)(0x54);
  pMVar1->id = (char *)0x0;
  pMVar1->mass_offset[2] = 0.0;
  pMVar1->asset = (MeFAsset *)0x0;
  pMVar1->geometry = (char *)0x0;
  pMVar1->mass_offset[1] = 0.0;
  pMVar1->mass_offset[0] = 0.0;
  pMVar1->angularDamp = 0.0;
  pMVar1->linearDamp = 0.0;
  pMVar1->fastSpin[0] = 0.0;
  pMVar1->fastSpin[2] = 0.0;
  pMVar1->fastSpin[1] = 1.0;
  pMVar1->useFastSpin = 0;
  pMVar1->inertia[0] = 0.4;
  pMVar1->inertia[1] = 0.0;
  pMVar1->inertia[2] = 0.0;
  pMVar1->inertia[3] = 0.4;
  pMVar1->inertia[4] = 0.0;
  pMVar1->inertia[5] = 0.4;
  pMVar1->mass = 1.0;
  pMVar1->density = 1.0;
  _FSetStringProperty(pMVar1,name);
  pMVar1->type = type;
  return pMVar1;
}


/* ==== MeFModelCreateCopy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeFModel * MeFModelCreateCopy(MeFModel *model,MeBool recurse)

{
  MeReal MVar1;
  MeBool MVar2;
  MeReal MVar3;
  MeFModel *pMVar4;
  
                    /* Unresolved local var: MeFModel * copy@[DW_OP_reg3(EBX)] */
  pMVar4 = (*_MeFAssetLookupGeometry)(0x54);
  _FSetStringProperty(pMVar4,model->id);
  pMVar4->type = model->type;
  _FSetStringProperty(&pMVar4->geometry,model->geometry);
  pMVar4->mass_offset[0] = model->mass_offset[0];
  pMVar4->mass_offset[1] = model->mass_offset[1];
  pMVar4->mass_offset[2] = model->mass_offset[2];
  MVar1 = model->linearDamp;
  pMVar4->angularDamp = model->angularDamp;
  pMVar4->linearDamp = MVar1;
  pMVar4->fastSpin[0] = model->fastSpin[0];
  pMVar4->fastSpin[1] = model->fastSpin[1];
  pMVar4->fastSpin[2] = model->fastSpin[2];
  MVar2 = model->useFastSpin;
  pMVar4->inertia[0] = model->inertia[0];
  pMVar4->useFastSpin = MVar2;
  pMVar4->inertia[1] = model->inertia[1];
  pMVar4->inertia[2] = model->inertia[2];
  pMVar4->inertia[3] = model->inertia[3];
  pMVar4->inertia[4] = model->inertia[4];
  MVar1 = model->inertia[5];
  pMVar4->mass = model->mass;
  MVar3 = model->density;
  pMVar4->inertia[5] = MVar1;
  pMVar4->density = MVar3;
  return pMVar4;
}


/* ==== MeFModelCopy ==== */

void MeFModelCopy(MeFModel *dst,MeFModel *src,MeBool setGeometry)

{
  MeReal MVar1;
  MeBool MVar2;
  
  dst->type = src->type;
  if (setGeometry != 0) {
    _FSetStringProperty(&dst->geometry,src->geometry);
  }
  dst->mass_offset[0] = src->mass_offset[0];
  dst->mass_offset[1] = src->mass_offset[1];
  dst->mass_offset[2] = src->mass_offset[2];
  MVar1 = src->linearDamp;
  dst->angularDamp = src->angularDamp;
  dst->linearDamp = MVar1;
  dst->fastSpin[0] = src->fastSpin[0];
  dst->fastSpin[1] = src->fastSpin[1];
  dst->fastSpin[2] = src->fastSpin[2];
  MVar2 = src->useFastSpin;
  dst->inertia[0] = src->inertia[0];
  dst->useFastSpin = MVar2;
  dst->inertia[1] = src->inertia[1];
  dst->inertia[2] = src->inertia[2];
  dst->inertia[3] = src->inertia[3];
  dst->inertia[4] = src->inertia[4];
  MVar1 = src->inertia[5];
  dst->mass = src->mass;
  dst->inertia[5] = MVar1;
  dst->density = src->density;
  return;
}


/* ==== MeFModelDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeFModelDestroy(MeFModel *fm)

{
  (*_DAT_00011020)(fm->id);
  if (fm->geometry != (char *)0x0) {
    (*_DAT_00011020)(fm->geometry);
  }
  (*_DAT_00011020)(fm);
  return;
}


/* ==== MeFModelGetName ==== */

char * MeFModelGetName(MeFModel *fm)

{
  return fm->id;
}


/* ==== MeFModelGetType ==== */

MeFModelType MeFModelGetType(MeFModel *fm)

{
  return fm->type;
}


/* ==== MeFModelGetGeometryName ==== */

char * MeFModelGetGeometryName(MeFModel *fm)

{
  return fm->geometry;
}


/* ==== MeFModelGetGeometry ==== */

MeFGeometry * MeFModelGetGeometry(MeFModel *fm)

{
  MeFGeometry *pMVar1;
  
                    /* Unresolved local var: MeFGeometry * g@[DW_OP_reg0(EAX)] */
  pMVar1 = (MeFGeometry *)0x0;
  if ((fm->asset != (MeFAsset *)0x0) && (fm->geometry != (char *)0x0)) {
    pMVar1 = MeFAssetLookupGeometry(fm->asset,fm->geometry);
  }
  return pMVar1;
}


/* ==== MeFModelGetMassOffset ==== */

void MeFModelGetMassOffset(MeFModel *fm,MeReal *o)

{
  *o = fm->mass_offset[0];
  o[1] = fm->mass_offset[1];
  o[2] = fm->mass_offset[2];
  return;
}


/* ==== MeFModelGetMass ==== */

MeReal MeFModelGetMass(MeFModel *fm)

{
  return fm->mass;
}


/* ==== MeFModelGetDensity ==== */

MeReal MeFModelGetDensity(MeFModel *fm)

{
  return fm->density;
}


/* ==== MeFModelGetInertiaTensor ==== */

void MeFModelGetInertiaTensor(MeFModel *fm,MeMatrix3Ptr I)

{
  MeReal MVar1;
  
  (*I)[0] = 1.0;
  (*I)[1] = 0.0;
  (*I)[2] = 0.0;
  I[1][0] = 0.0;
  I[1][1] = 1.0;
  I[1][2] = 0.0;
  I[2][0] = 0.0;
  I[2][1] = 0.0;
  I[2][2] = 1.0;
  (*I)[0] = fm->inertia[0];
  I[1][1] = fm->inertia[3];
  I[2][2] = fm->inertia[5];
  MVar1 = fm->inertia[1];
  (*I)[1] = MVar1;
  I[1][0] = MVar1;
  MVar1 = fm->inertia[2];
  (*I)[2] = MVar1;
  I[2][0] = MVar1;
  MVar1 = fm->inertia[4];
  I[1][2] = MVar1;
  I[2][1] = MVar1;
  return;
}


/* ==== MeFModelGetLinearVelocityDamping ==== */

MeReal MeFModelGetLinearVelocityDamping(MeFModel *fm)

{
  return fm->linearDamp;
}


/* ==== MeFModelGetAngularVelocityDamping ==== */

MeReal MeFModelGetAngularVelocityDamping(MeFModel *fm)

{
  return fm->angularDamp;
}


/* ==== MeFModelGetFastSpinAxis ==== */

void MeFModelGetFastSpinAxis(MeFModel *fm,MeReal *axis)

{
  *axis = fm->fastSpin[0];
  axis[1] = fm->fastSpin[1];
  axis[2] = fm->fastSpin[2];
  return;
}


/* ==== MeFModelIsFastSpinAxisEnabled ==== */

MeBool MeFModelIsFastSpinAxisEnabled(MeFModel *model)

{
  return model->useFastSpin;
}


/* ==== MeFModelSetType ==== */

void MeFModelSetType(MeFModel *fm,MeFModelType type)

{
  fm->type = type;
  return;
}


/* ==== MeFModelSetGeometry ==== */

void MeFModelSetGeometry(MeFModel *fm,MeFGeometry *fg)

{
  if (fg != (MeFGeometry *)0x0) {
    _FSetStringProperty(&fm->geometry,fg->id);
    return;
  }
  return;
}


/* ==== MeFModelSetGeometryByName ==== */

void MeFModelSetGeometryByName(MeFModel *model,char *name)

{
  _FSetStringProperty(&model->geometry,name);
  return;
}


/* ==== MeFModelSetMassOffset ==== */

void MeFModelSetMassOffset(MeFModel *fm,MeReal *v)

{
  fm->mass_offset[0] = *v;
  fm->mass_offset[1] = v[1];
  fm->mass_offset[2] = v[2];
  return;
}


/* ==== MeFModelSetMass ==== */

void MeFModelSetMass(MeFModel *fm,MeReal mass)

{
  fm->mass = mass;
  return;
}


/* ==== MeFModelSetDensity ==== */

void MeFModelSetDensity(MeFModel *fm,MeReal density)

{
  fm->density = density;
  return;
}


/* ==== MeFModelSetInertiaTensor ==== */

void MeFModelSetInertiaTensor(MeFModel *fm,MeMatrix3Ptr I)

{
  fm->inertia[0] = (*I)[0];
  fm->inertia[3] = I[1][1];
  fm->inertia[5] = I[2][2];
  fm->inertia[1] = I[1][0];
  fm->inertia[2] = I[2][0];
  fm->inertia[4] = I[2][1];
  return;
}


/* ==== MeFModelSetLinearVelocityDamping ==== */

void MeFModelSetLinearVelocityDamping(MeFModel *fm,MeReal d)

{
  fm->linearDamp = d;
  return;
}


/* ==== MeFModelSetAngularVelocityDamping ==== */

void MeFModelSetAngularVelocityDamping(MeFModel *fm,MeReal d)

{
  fm->angularDamp = d;
  return;
}


/* ==== MeFModelSetFastSpinAxis ==== */

void MeFModelSetFastSpinAxis(MeFModel *fm,MeReal x,MeReal y,MeReal z)

{
  fm->fastSpin[0] = x;
  fm->fastSpin[1] = y;
  fm->fastSpin[2] = z;
  return;
}


/* ==== MeFModelEnableFastSpinAxis ==== */

void MeFModelEnableFastSpinAxis(MeFModel *model,MeBool b)

{
  model->useFastSpin = b;
  return;
}


/* ==== MeFModelScale ==== */

void MeFModelScale(MeFModel *model,MeReal scale)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  MeMatrix3 I;
  
                    /* Unresolved local var: int i@[???] */
  fVar1 = model->inertia[1];
  fVar2 = model->inertia[3];
  fVar3 = model->inertia[2];
  fVar4 = model->inertia[4];
  model->mass = model->mass * scale * scale * scale;
  fVar5 = model->inertia[5];
  model->inertia[0] = model->inertia[0] * scale * scale * scale * scale * scale;
  model->inertia[3] = fVar2 * scale * scale * scale * scale * scale;
  model->inertia[5] = fVar5 * scale * scale * scale * scale * scale;
  fVar2 = model->mass_offset[0];
  model->inertia[1] = fVar1 * scale * scale * scale * scale * scale;
  model->inertia[2] = fVar3 * scale * scale * scale * scale * scale;
  model->inertia[4] = fVar4 * scale * scale * scale * scale * scale;
  model->mass_offset[0] = fVar2 * scale;
  model->mass_offset[1] = model->mass_offset[1] * scale;
  model->mass_offset[2] = scale * model->mass_offset[2];
  return;
}


