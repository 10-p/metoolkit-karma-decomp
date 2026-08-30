/* ==== MdtBodyReset  @ 00010000 ==== */

void MdtBodyReset(MdtBodyID b)

{
  float fVar1;
  MdtWorldID pMVar2;
  MeReal MVar3;
  MdtBodyID b_00;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: MdtWorldParams * p@[DW_OP_reg6(ESI)] */
  pMVar2 = b->world;
  b_00 = (MdtBodyID)MdtBodyIsEnabled(b);
  if (b_00 != (MdtBodyID)0x0) {
    MdtBodyDisable(b_00);
  }
  fVar1 = (pMVar2->params).massScale;
  (b->keaBody).force[0] = 0.0;
  (b->keaBody).torque[0] = 0.0;
  (b->keaBody).invI2[0] = 0.0;
  (b->keaBody).invI1[0] = 0.0;
  (b->keaBody).invI0[0] = 0.0;
  (b->keaBody).I2[0] = 0.0;
  (b->keaBody).I1[0] = 0.0;
  (b->keaBody).I0[0] = 0.0;
  (b->keaBody).vel[0] = 0.0;
  (b->keaBody).velrot[0] = 0.0;
  (b->keaBody).qrot[0] = 0.0;
  (b->keaBody).accel[0] = 0.0;
  (b->keaBody).accelrot[0] = 0.0;
  (b->keaBody).fastSpinAxis[0] = 0.0;
  b->impulseLinear[0] = 0.0;
  (b->keaBody).flags = 0;
  b->impulseAngular[0] = 0.0;
  (b->keaBody).force[1] = 0.0;
  (b->keaBody).torque[1] = 0.0;
  (b->keaBody).invI2[1] = 0.0;
  (b->keaBody).invI1[1] = 0.0;
  (b->keaBody).invI0[1] = 0.0;
  (b->keaBody).I2[1] = 0.0;
  (b->keaBody).I1[1] = 0.0;
  (b->keaBody).I0[1] = 0.0;
  (b->keaBody).vel[1] = 0.0;
  (b->keaBody).velrot[1] = 0.0;
  (b->keaBody).qrot[1] = 0.0;
  (b->keaBody).accel[1] = 0.0;
  (b->keaBody).accelrot[1] = 0.0;
  (b->keaBody).fastSpinAxis[1] = 0.0;
  (b->keaBody).invmass = 1.0 / fVar1;
  b->impulseLinear[1] = 0.0;
  b->impulseAngular[1] = 0.0;
  b->impulseAngular[2] = 0.0;
  (b->keaBody).force[2] = 0.0;
  (b->keaBody).torque[2] = 0.0;
  (b->keaBody).invI2[2] = 0.0;
  (b->keaBody).invI1[2] = 0.0;
  (b->keaBody).invI0[2] = 0.0;
  (b->keaBody).I2[2] = 0.0;
  (b->keaBody).I1[2] = 0.0;
  (b->keaBody).I0[2] = 0.0;
  (b->keaBody).vel[2] = 0.0;
  (b->keaBody).velrot[2] = 0.0;
  (b->keaBody).qrot[2] = 0.0;
  (b->keaBody).accel[2] = 0.0;
  (b->keaBody).accelrot[2] = 0.0;
  (b->keaBody).fastSpinAxis[2] = 0.0;
  b->impulseLinear[2] = 0.0;
  b->impulseAngular[3] = 0.0;
  (b->keaBody).force[3] = 0.0;
  (b->keaBody).torque[3] = 0.0;
  (b->keaBody).invI2[3] = 0.0;
  (b->keaBody).invI1[3] = 0.0;
  (b->keaBody).invI0[3] = 0.0;
  (b->keaBody).I2[3] = 0.0;
  (b->keaBody).I1[3] = 0.0;
  (b->keaBody).I0[3] = 0.0;
  (b->keaBody).vel[3] = 0.0;
  (b->keaBody).velrot[3] = 0.0;
  (b->keaBody).qrot[3] = 0.0;
  (b->keaBody).accel[3] = 0.0;
  (b->keaBody).accelrot[3] = 0.0;
  (b->keaBody).fastSpinAxis[3] = 0.0;
  b->impulseLinear[3] = 0.0;
  b->impulseAdded = 0;
  (b->keaBody).qrot[0] = 1.0;
  fVar1 = (pMVar2->params).massScale * 0.4;
  (b->keaBody).I2[2] = fVar1;
  (b->keaBody).I1[1] = fVar1;
  (b->keaBody).I0[0] = fVar1;
  fVar1 = (pMVar2->params).massScale;
  (b->keaBody).fastSpinAxis[0] = 0.0;
  (b->keaBody).fastSpinAxis[2] = 0.0;
  (b->keaBody).fastSpinAxis[1] = 1.0;
  fVar1 = 1.0 / (fVar1 * 0.4);
  b->comTM[0][0] = 1.0;
  (b->keaBody).invI2[2] = fVar1;
  (b->keaBody).invI1[1] = fVar1;
  (b->keaBody).invI0[0] = fVar1;
  b->comTM[0][1] = 0.0;
  b->comTM[0][2] = 0.0;
  b->comTM[0][3] = 0.0;
  b->comTM[1][0] = 0.0;
  b->comTM[1][1] = 1.0;
  b->comTM[1][2] = 0.0;
  b->comTM[1][3] = 0.0;
  b->comTM[2][0] = 0.0;
  b->comTM[2][1] = 0.0;
  b->comTM[2][2] = 1.0;
  b->comTM[2][3] = 0.0;
  b->comTM[3][0] = 0.0;
  b->comTM[3][1] = 0.0;
  b->comTM[3][2] = 0.0;
  b->comTM[3][3] = 1.0;
  b->bodyTM[0][0] = 1.0;
  b->bodyTM[0][1] = 0.0;
  b->bodyTM[0][2] = 0.0;
  b->bodyTM[0][3] = 0.0;
  b->bodyTM[1][0] = 0.0;
  b->bodyTM[1][1] = 1.0;
  b->bodyTM[2][2] = 1.0;
  b->bodyTM[3][3] = 1.0;
  b->bodyTM[1][2] = 0.0;
  b->bodyTM[1][3] = 0.0;
  b->bodyTM[2][0] = 0.0;
  b->bodyTM[2][1] = 0.0;
  b->bodyTM[2][3] = 0.0;
  b->bodyTM[3][0] = 0.0;
  b->bodyTM[3][1] = 0.0;
  b->bodyTM[3][2] = 0.0;
  b->com[0] = 0.0;
  b->com[1] = 0.0;
  b->com[2] = 0.0;
  b->useCom = 0;
  b->arrayIdWorld = -1;
  b->arrayIdPartition = -1;
  b->partitionIndex = -1;
  MVar3 = (pMVar2->params).massScale;
  b->damping = 0.0;
  b->mass = MVar3;
  b->angularDamping = 0.0;
  b->flags = 0;
  b->enabledTime = 0.0;
  b->userData = (void *)0x0;
  b->safeTime = 3.4028235e+38;
  return;
}


/* ==== MdtBodyDisable  @ 00010320 ==== */

void __regparm1 MdtBodyDisable(MdtBodyID b)

{
  code *pcVar1;
  int in_stack_00000004;
  
  if ((*(byte *)(in_stack_00000004 + 0x1ec) & 1) != 0) {
    MeDictDelete(*(int *)(in_stack_00000004 + 0x1a4) + 0x68,in_stack_00000004 + 0x1c4,b,b,b);
    *(uint *)(in_stack_00000004 + 0x1ec) = *(uint *)(in_stack_00000004 + 0x1ec) & 0xfffffffa;
    *(undefined4 *)(in_stack_00000004 + 0x230) = 0;
    *(undefined4 *)(in_stack_00000004 + 0x1e0) = 0xffffffff;
    *(int *)(*(int *)(in_stack_00000004 + 0x1a4) + 0xbc) =
         *(int *)(*(int *)(in_stack_00000004 + 0x1a4) + 0xbc) + -1;
    pcVar1 = *(code **)(*(int *)(in_stack_00000004 + 0x1a4) + 0x1d0);
    if (pcVar1 != (code *)0x0) {
      (*pcVar1)();
    }
  }
  return;
}


/* ==== MdtBodyIsEnabled  @ 000103b0 ==== */

MeBool MdtBodyIsEnabled(MdtBodyID b)

{
  return b->flags & 1;
}


/* ==== MdtBodyCreate  @ 000103d0 ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MdtBodyID MdtBodyCreate(MdtWorldID w)

{
  MeI32 MVar1;
  MdtBodyID b;
  MdtBodyID pMVar2;
  
                    /* Unresolved local var: MdtBody * b@[DW_OP_reg6(ESI)] */
  b = (MdtBodyID)(*_MdtDictCompare)(&w->bodyPool);
  pMVar2 = (MdtBodyID)0x0;
  if (b != (MdtBodyID)0x0) {
    b->world = w;
    (b->keaBody).tag = 100;
    (b->keaBody).len = 0xf0;
    b->sortKey = 0;
    b->model = (void *)0x0;
    b->flags = 0;
    MdtBodyReset(b);
    MeDictNodeInit(&b->worldNode,b->sortKey);
    (b->worldNode).data = b;
    MeDictNodeInit(&b->worldEnabledNode,b->sortKey);
    MVar1 = b->sortKey;
    (b->worldEnabledNode).data = b;
    MeDictInsert(w,&b->worldNode,MVar1);
    MeDictInit(&b->constraintDict,1000000,&MdtDictCompare);
    MeDictAllowDupes(&b->constraintDict);
    w->nBodies = w->nBodies + 1;
    pMVar2 = b;
  }
  return pMVar2;
}


/* ==== MdtBodyDestroy  @ 000104c0 ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void MdtBodyDestroy(MdtBodyID b)

{
  undefined4 uVar1;
  MdtWorldID pMVar2;
  undefined4 uVar3;
  int iVar4;
  MdtBodyID b_00;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  MeDict *pMVar5;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDictNode * next@[DW_OP_reg3(EBX)] */
  MeDictDelete(b->world,&b->worldNode);
  pMVar5 = &b->constraintDict;
  iVar4 = MeDictFirst(pMVar5);
  uVar3 = extraout_ECX;
  while (iVar4 != 0) {
                    /* Unresolved local var: MdtBaseConstraint * cp@[DW_OP_reg6(ESI)] */
    uVar1 = *(undefined4 *)(iVar4 + 0x14);
    MeDictNext(pMVar5,iVar4,uVar3,uVar3);
    iVar4 = MeDictNext(pMVar5,iVar4);
    MdtConstraintDisable(uVar1);
    MdtConstraintDestroy(uVar1);
    uVar3 = extraout_ECX_00;
  }
  b_00 = (MdtBodyID)MdtBodyIsEnabled(b);
  if (b_00 != (MdtBodyID)0x0) {
    MdtBodyDisable(b_00);
  }
  pMVar2 = b->world;
  (b->keaBody).tag = -1;
  iVar4 = pMVar2->nBodies + -1;
  pMVar2->nBodies = iVar4;
  (*_MeDictInit)(&b->world->bodyPool,b,iVar4,iVar4);
  return;
}


/* ==== MdtBodyCopy  @ 00010590 ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MdtBodyID MdtBodyCopy(MdtBodyID b,MdtWorldID w)

{
  MeI32 MVar1;
  MdtBodyID b_00;
  MdtBodyID pMVar2;
  
                    /* Unresolved local var: MdtBodyID newb@[???] */
                    /* Unresolved local var: MdtBody * b@[DW_OP_reg6(ESI)] */
  b_00 = (MdtBodyID)(*_MdtDictCompare)(&w->bodyPool);
  pMVar2 = (MdtBodyID)0x0;
  if (b_00 != (MdtBodyID)0x0) {
    b_00->world = w;
    (b_00->keaBody).tag = 100;
    (b_00->keaBody).len = 0xf0;
    b_00->sortKey = 0;
    b_00->model = (void *)0x0;
    b_00->flags = 0;
    MdtBodyReset(b_00);
    MeDictNodeInit(&b_00->worldNode,b_00->sortKey);
    MVar1 = b_00->sortKey;
    (b_00->worldNode).data = b_00;
    MeDictNodeInit(&b_00->worldEnabledNode,MVar1);
    MVar1 = b_00->sortKey;
    (b_00->worldEnabledNode).data = b_00;
    MeDictInsert(w,&b_00->worldNode,MVar1);
    MeDictInit(&b_00->constraintDict,1000000,&MdtDictCompare);
    MeDictAllowDupes(&b_00->constraintDict);
    w->nBodies = w->nBodies + 1;
    pMVar2 = b_00;
  }
  MeFatalError(0,"MdtBodyCopy: NOT YET IMPLEMENTED.",b_00,b_00);
  return pMVar2;
}


/* ==== MdtBodyGetWorld  @ 00010690 ==== */

MdtWorldID MdtBodyGetWorld(MdtBodyID b)

{
  return b->world;
}


/* ==== MdtBodyGetPartition  @ 000106a0 ==== */

int MdtBodyGetPartition(MdtBodyID b)

{
  return b->partitionIndex;
}


/* ==== MdtBodyGetTransformPtr  @ 000106b0 ==== */

/* WARNING: Unknown calling convention */

MeMatrix4Ptr MdtBodyGetTransformPtr(MdtBodyID b)

{
  return b->bodyTM;
}


/* ==== MdtBodyGetTransform  @ 000106c0 ==== */

void MdtBodyGetTransform(MdtBodyID b,MeVector4 *m)

{
  (*m)[0] = b->bodyTM[0][0];
  (*m)[1] = b->bodyTM[0][1];
  (*m)[2] = b->bodyTM[0][2];
  (*m)[3] = b->bodyTM[0][3];
  m[1][0] = b->bodyTM[1][0];
  m[1][1] = b->bodyTM[1][1];
  m[1][2] = b->bodyTM[1][2];
  m[1][3] = b->bodyTM[1][3];
  m[2][0] = b->bodyTM[2][0];
  m[2][1] = b->bodyTM[2][1];
  m[2][2] = b->bodyTM[2][2];
  m[2][3] = b->bodyTM[2][3];
  m[3][0] = b->bodyTM[3][0];
  m[3][1] = b->bodyTM[3][1];
  m[3][2] = b->bodyTM[3][2];
  m[3][3] = b->bodyTM[3][3];
  return;
}


/* ==== MdtBodyGetUserData  @ 00010740 ==== */

void * MdtBodyGetUserData(MdtBodyID b)

{
  return b->userData;
}


/* ==== MdtBodyGetMass  @ 00010750 ==== */

MeReal MdtBodyGetMass(MdtBodyID b)

{
  return b->mass;
}


/* ==== MdtBodyGetInertiaTensor  @ 00010760 ==== */

void MdtBodyGetInertiaTensor(MdtBodyID b,MeVector3 *i)

{
                    /* Unresolved local var: int j@[???] */
  (*i)[0] = (b->keaBody).I0[0];
  i[1][0] = (b->keaBody).I1[0];
  i[2][0] = (b->keaBody).I2[0];
  (*i)[1] = (b->keaBody).I0[1];
  i[1][1] = (b->keaBody).I1[1];
  i[2][1] = (b->keaBody).I2[1];
  (*i)[2] = (b->keaBody).I0[2];
  i[1][2] = (b->keaBody).I1[2];
  i[2][2] = (b->keaBody).I2[2];
  return;
}


/* ==== MdtBodyGetPosition  @ 000107b0 ==== */

void MdtBodyGetPosition(MdtBodyID b,MeReal *p)

{
  *p = b->bodyTM[3][0];
  p[1] = b->bodyTM[3][1];
  p[2] = b->bodyTM[3][2];
  return;
}


/* ==== MdtBodyGetQuaternion  @ 000107e0 ==== */

void MdtBodyGetQuaternion(MdtBodyID b,MeReal *q)

{
  *q = (b->keaBody).qrot[0];
  q[1] = (b->keaBody).qrot[1];
  q[2] = (b->keaBody).qrot[2];
  q[3] = (b->keaBody).qrot[3];
  return;
}


/* ==== MdtBodyGetOrientation  @ 00010810 ==== */

void MdtBodyGetOrientation(MdtBodyID b,MeVector3 *R)

{
  (*R)[0] = b->bodyTM[0][0];
  (*R)[1] = b->bodyTM[0][1];
  (*R)[2] = b->bodyTM[0][2];
  R[1][0] = b->bodyTM[1][0];
  R[1][1] = b->bodyTM[1][1];
  R[1][2] = b->bodyTM[1][2];
  R[2][0] = b->bodyTM[2][0];
  R[2][1] = b->bodyTM[2][1];
  R[2][2] = b->bodyTM[2][2];
  return;
}


/* ==== MdtBodyGetLinearVelocity  @ 00010860 ==== */

void MdtBodyGetLinearVelocity(MdtBodyID b,MeReal *v)

{
  *v = (b->keaBody).vel[0];
  v[1] = (b->keaBody).vel[1];
  v[2] = (b->keaBody).vel[2];
  return;
}


/* ==== MdtBodyGetLinearVelocityPtr  @ 00010890 ==== */

/* WARNING: Unknown calling convention */

MeReal * MdtBodyGetLinearVelocityPtr(MdtBodyID b)

{
  return (b->keaBody).vel;
}


/* ==== MdtBodyGetAngularVelocity  @ 000108a0 ==== */

void MdtBodyGetAngularVelocity(MdtBodyID b,MeReal *v)

{
  *v = (b->keaBody).velrot[0];
  v[1] = (b->keaBody).velrot[1];
  v[2] = (b->keaBody).velrot[2];
  return;
}


/* ==== MdtBodyGetAngularVelocityPtr  @ 000108d0 ==== */

/* WARNING: Unknown calling convention */

MeReal * MdtBodyGetAngularVelocityPtr(MdtBodyID b)

{
  return (b->keaBody).velrot;
}


/* ==== MdtBodyGetVelocityAtPoint  @ 000108e0 ==== */

void MdtBodyGetVelocityAtPoint(MdtBodyID b,MeReal *p,MeReal *v)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  MeVector3 toP;
  MeVector3 tmp;
  
  fVar7 = p[1] - b->comTM[3][1];
  fVar6 = p[2] - b->comTM[3][2];
  fVar5 = *p - b->comTM[3][0];
  fVar1 = (b->keaBody).velrot[0];
  fVar2 = (b->keaBody).velrot[2];
  fVar3 = (b->keaBody).velrot[0];
  fVar4 = (b->keaBody).velrot[1];
  *v = ((b->keaBody).velrot[1] * fVar6 - (b->keaBody).velrot[2] * fVar7) + (b->keaBody).vel[0];
  v[1] = (fVar2 * fVar5 - fVar6 * fVar1) + (b->keaBody).vel[1];
  v[2] = (fVar7 * fVar3 - fVar5 * fVar4) + (b->keaBody).vel[2];
  return;
}


/* ==== MdtBodyGetLinearAcceleration  @ 00010970 ==== */

void MdtBodyGetLinearAcceleration(MdtBodyID b,MeReal *v)

{
  *v = (b->keaBody).accel[0];
  v[1] = (b->keaBody).accel[1];
  v[2] = (b->keaBody).accel[2];
  return;
}


/* ==== MdtBodyGetAngularAcceleration  @ 000109a0 ==== */

void MdtBodyGetAngularAcceleration(MdtBodyID b,MeReal *v)

{
  *v = (b->keaBody).accelrot[0];
  v[1] = (b->keaBody).accelrot[1];
  v[2] = (b->keaBody).accelrot[2];
  return;
}


/* ==== MdtBodyGetFastSpinAxis  @ 000109d0 ==== */

void MdtBodyGetFastSpinAxis(MdtBodyID b,MeReal *v)

{
  *v = (b->keaBody).fastSpinAxis[0];
  v[1] = (b->keaBody).fastSpinAxis[1];
  v[2] = (b->keaBody).fastSpinAxis[2];
  return;
}


/* ==== MdtBodyGetLinearVelocityDamping  @ 00010a00 ==== */

MeReal MdtBodyGetLinearVelocityDamping(MdtBodyID b)

{
  return b->damping;
}


/* ==== MdtBodyGetAngularVelocityDamping  @ 00010a10 ==== */

MeReal MdtBodyGetAngularVelocityDamping(MdtBodyID b)

{
  return b->angularDamping;
}


/* ==== MdtBodyGetForce  @ 00010a20 ==== */

void MdtBodyGetForce(MdtBodyID b,MeReal *v)

{
  *v = (b->keaBody).force[0];
  v[1] = (b->keaBody).force[1];
  v[2] = (b->keaBody).force[2];
  return;
}


/* ==== MdtBodyGetTorque  @ 00010a40 ==== */

void MdtBodyGetTorque(MdtBodyID b,MeReal *v)

{
  *v = (b->keaBody).torque[0];
  v[1] = (b->keaBody).torque[1];
  v[2] = (b->keaBody).torque[2];
  return;
}


/* ==== MdtBodyGetKineticEnergy  @ 00010a60 ==== */

MeReal MdtBodyGetKineticEnergy(MdtBodyID b)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  
                    /* Unresolved local var: MeReal ke@[DW_OP_reg11(ST0)] */
  fVar1 = b->mass;
  fVar2 = (b->keaBody).velrot[0];
  fVar3 = (b->keaBody).velrot[1];
  fVar4 = (b->keaBody).velrot[2];
  return ((b->keaBody).I1[2] * fVar4 * fVar3 +
          (b->keaBody).I2[1] * fVar3 * fVar4 +
          (b->keaBody).I1[1] * fVar3 * fVar3 +
          (b->keaBody).I0[1] * fVar3 * fVar2 +
          (b->keaBody).I2[0] * fVar2 * fVar4 +
          (b->keaBody).I1[0] * fVar2 * fVar3 +
          (b->keaBody).I0[0] * fVar2 * fVar2 +
          fVar1 * (b->keaBody).vel[0] * (b->keaBody).vel[0] +
          fVar1 * (b->keaBody).vel[1] * (b->keaBody).vel[1] +
          fVar1 * (b->keaBody).vel[2] * (b->keaBody).vel[2] + (b->keaBody).I0[2] * fVar4 * fVar2 +
         (b->keaBody).I2[2] * fVar4 * fVar4) * 0.5;
}


/* ==== MdtBodyGetFirst  @ 00010b20 ==== */

/* WARNING: Unknown calling convention */

MdtBodyID MdtBodyGetFirst(MdtWorldID w)

{
  int iVar1;
  MdtBodyID pMVar2;
  
                    /* Unresolved local var: MeDictNode * first@[DW_OP_reg0(EAX)] */
  iVar1 = MeDictFirst(w);
  pMVar2 = (MdtBodyID)0x0;
  if (iVar1 != 0) {
    pMVar2 = *(MdtBodyID *)(iVar1 + 0x14);
  }
  return pMVar2;
}


/* ==== MdtBodyGetNext  @ 00010b50 ==== */

MdtBodyID MdtBodyGetNext(MdtBodyID b)

{
  int iVar1;
  MdtBodyID pMVar2;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg0(EAX)] */
  iVar1 = MeDictNext(b->world,&b->worldNode);
  pMVar2 = (MdtBodyID)0x0;
  if (iVar1 != 0) {
    pMVar2 = *(MdtBodyID *)(iVar1 + 0x14);
  }
  return pMVar2;
}


/* ==== MdtBodyGetEnabledTime  @ 00010b80 ==== */

MeReal MdtBodyGetEnabledTime(MdtBodyID b)

{
  return b->enabledTime;
}


/* ==== MdtBodyGetCenterOfMassRelativeTransform  @ 00010b90 ==== */

void MdtBodyGetCenterOfMassRelativeTransform(MdtBodyID b,MeVector4 *t)

{
  (*t)[0] = 1.0;
  (*t)[1] = 0.0;
  (*t)[2] = 0.0;
  (*t)[3] = 0.0;
  t[1][0] = 0.0;
  t[1][1] = 1.0;
  t[1][2] = 0.0;
  t[1][3] = 0.0;
  t[2][0] = 0.0;
  t[2][1] = 0.0;
  t[2][2] = 1.0;
  t[2][3] = 0.0;
  t[3][0] = 0.0;
  t[3][1] = 0.0;
  t[3][2] = 0.0;
  t[3][3] = 1.0;
  if (b->useCom != 0) {
    t[3][0] = b->com[0] * -1.0;
    t[3][1] = b->com[1] * -1.0;
    t[3][2] = b->com[2] * -1.0;
  }
  return;
}


/* ==== MdtBodyGetCenterOfMassRelativePosition  @ 00010c40 ==== */

void MdtBodyGetCenterOfMassRelativePosition(MdtBodyID b,MeReal *pos)

{
  *pos = b->com[0];
  pos[1] = b->com[1];
  pos[2] = b->com[2];
  return;
}


/* ==== MdtBodyGetCenterOfMassPosition  @ 00010c70 ==== */

void MdtBodyGetCenterOfMassPosition(MdtBodyID b,MeReal *pos)

{
  *pos = b->bodyTM[2][0] * b->com[2] + b->bodyTM[1][0] * b->com[1] + b->bodyTM[0][0] * b->com[0] +
         b->bodyTM[3][0];
  pos[1] = b->bodyTM[2][1] * b->com[2] + b->bodyTM[1][1] * b->com[1] + b->bodyTM[0][1] * b->com[0] +
           b->bodyTM[3][1];
  pos[2] = b->bodyTM[2][2] * b->com[2] + b->bodyTM[1][2] * b->com[1] + b->bodyTM[0][2] * b->com[0] +
           b->bodyTM[3][2];
  return;
}


/* ==== MdtBodyGetCenterOfMassTransformPtr  @ 00010d00 ==== */

/* WARNING: Unknown calling convention */

MeMatrix4Ptr MdtBodyGetCenterOfMassTransformPtr(MdtBodyID b)

{
  return b->comTM;
}


/* ==== MdtBodyGetSafeTime  @ 00010d10 ==== */

MeReal MdtBodyGetSafeTime(MdtBodyID b)

{
  return b->safeTime;
}


/* ==== MdtBodyGetSortKey  @ 00010d20 ==== */

MeI32 MdtBodyGetSortKey(MdtBodyID b)

{
  return b->sortKey;
}


/* ==== MdtBodySetUserData  @ 00010d30 ==== */

void MdtBodySetUserData(MdtBodyID b,void *d)

{
  b->userData = d;
  return;
}


/* ==== MdtBodySetTransform  @ 00010d50 ==== */

void MdtBodySetTransform(MdtBodyID b,MeReal (*tm) [4])

{
  int iVar1;
  MeReal (*paMVar2) [4];
  
  paMVar2 = b->bodyTM;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    (*paMVar2)[0] = (*tm)[0];
    tm = (MeReal (*) [4])((int)tm + 4);
    paMVar2 = (MeReal (*) [4])(*paMVar2 + 1);
  }
  UpdateCOMTransform();
  return;
}


/* ==== MdtBodySetPosition  @ 00010d80 ==== */

void MdtBodySetPosition(MdtBodyID b,MeReal x,MeReal y,MeReal z)

{
  b->bodyTM[3][0] = x;
  b->bodyTM[3][1] = y;
  b->bodyTM[3][2] = z;
  UpdateCOMTransform();
  return;
}


/* ==== MdtBodySetOrientation  @ 00010db0 ==== */

void MdtBodySetOrientation(MdtBodyID b,MeReal (*R) [3])

{
  b->bodyTM[0][0] = (*R)[0];
  b->bodyTM[0][1] = (*R)[1];
  b->bodyTM[0][2] = (*R)[2];
  b->bodyTM[1][0] = R[1][0];
  b->bodyTM[1][1] = R[1][1];
  b->bodyTM[1][2] = R[1][2];
  b->bodyTM[2][0] = R[2][0];
  b->bodyTM[2][1] = R[2][1];
  b->bodyTM[2][2] = R[2][2];
  UpdateCOMTransform();
  return;
}


/* ==== MdtBodySetQuaternion  @ 00010e10 ==== */

void MdtBodySetQuaternion(MdtBodyID b,MeReal qw,MeReal qx,MeReal qy,MeReal qz)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  MeReal q0q2;
  MeReal q3sq;
  MeReal q2sq;
  MeReal q1sq;
  MeVector4 bodyq;
  
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
                    /* Unresolved local var: MeReal m@[???] */
  bodyq[0] = qw;
  bodyq[1] = qx;
  bodyq[2] = qy;
  bodyq[3] = qz;
  fVar1 = qw * qw + qx * qx + qy * qy + qz * qz;
  if ((0.001 <= ABS(fVar1)) && (0.001 <= ABS(fVar1 - 1.0))) {
                    /* Unresolved local var: float __result@[DW_OP_reg11(ST0)] */
    fVar1 = 1.0 / SQRT(fVar1);
    bodyq[0] = qw * fVar1;
    bodyq[1] = qx * fVar1;
    bodyq[2] = qy * fVar1;
    bodyq[3] = fVar1 * qz;
  }
                    /* Unresolved local var: MeReal q0sq@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal q0t2@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q1t2@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q0q1@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal q0q3@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal q1q2@[DW_OP_reg17(ST6)]
                       Unresolved local var: MeReal q1q3@[DW_OP_reg15(ST4)]
                       Unresolved local var: MeReal q2q3@[DW_OP_reg14(ST3)] */
  fVar6 = bodyq[2] * bodyq[2];
  fVar5 = bodyq[0] + bodyq[0];
  fVar4 = bodyq[3] * bodyq[3];
  fVar3 = (bodyq[1] + bodyq[1]) * bodyq[2];
  fVar1 = (bodyq[1] + bodyq[1]) * bodyq[3];
  fVar2 = (bodyq[2] + bodyq[2]) * bodyq[3];
  b->bodyTM[0][0] = ((bodyq[1] * bodyq[1] + bodyq[0] * bodyq[0]) - fVar6) - fVar4;
  b->bodyTM[1][0] = fVar3 - fVar5 * bodyq[3];
  b->bodyTM[0][1] = fVar3 + fVar5 * bodyq[3];
  b->bodyTM[2][0] = fVar5 * bodyq[2] + fVar1;
  fVar3 = bodyq[0] * bodyq[0] - bodyq[1] * bodyq[1];
  b->bodyTM[1][1] = (fVar6 + fVar3) - fVar4;
  b->bodyTM[2][1] = fVar2 - fVar5 * bodyq[1];
  b->bodyTM[1][2] = fVar5 * bodyq[1] + fVar2;
  b->bodyTM[0][2] = fVar1 - fVar5 * bodyq[2];
  b->bodyTM[2][2] = (fVar3 - fVar6) + fVar4;
  UpdateCOMTransform(b);
  return;
}


/* ==== MdtBodySetLinearVelocity  @ 00010fb0 ==== */

void MdtBodySetLinearVelocity(MdtBodyID b,MeReal dx,MeReal dy,MeReal dz)

{
  (b->keaBody).vel[0] = dx;
  (b->keaBody).vel[1] = dy;
  (b->keaBody).vel[2] = dz;
  return;
}


/* ==== MdtBodySetAngularVelocity  @ 00010fe0 ==== */

void MdtBodySetAngularVelocity(MdtBodyID b,MeReal wx,MeReal wy,MeReal wz)

{
  (b->keaBody).velrot[0] = wx;
  (b->keaBody).velrot[1] = wy;
  (b->keaBody).velrot[2] = wz;
  return;
}


/* ==== MdtBodySetLinearVelocityDamping  @ 00011010 ==== */

void MdtBodySetLinearVelocityDamping(MdtBodyID b,MeReal d)

{
  b->damping = d;
  return;
}


/* ==== MdtBodySetAngularVelocityDamping  @ 00011030 ==== */

void MdtBodySetAngularVelocityDamping(MdtBodyID b,MeReal d)

{
  b->angularDamping = d;
  return;
}


/* ==== MdtBodySetFastSpinAxis  @ 00011050 ==== */

void MdtBodySetFastSpinAxis(MdtBodyID b,MeReal x,MeReal y,MeReal z)

{
  (b->keaBody).flags = (b->keaBody).flags | 1;
  (b->keaBody).fastSpinAxis[0] = x;
  (b->keaBody).fastSpinAxis[1] = y;
  (b->keaBody).fastSpinAxis[2] = z;
  MeVector3Normalize((b->keaBody).fastSpinAxis);
  return;
}


/* ==== MdtBodySetNoFastSpinAxis  @ 00011090 ==== */

void MdtBodySetNoFastSpinAxis(MdtBodyID b)

{
  (b->keaBody).flags = (b->keaBody).flags & 0xfffffffe;
  return;
}


/* ==== MdtBodySetMass  @ 000110b0 ==== */

void MdtBodySetMass(MdtBodyID b,MeReal mass)

{
  b->mass = mass;
  (b->keaBody).invmass = 1.0 / mass;
  return;
}


/* ==== MdtBodySetInertiaTensor  @ 000110d0 ==== */

void MdtBodySetInertiaTensor(MdtBodyID b,MeReal (*i) [3])

{
  float fVar1;
  MeReal MVar2;
  MeReal MVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  MeMatrix3 clamped;
  
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal iMag@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal r@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal a00@[???]
                       Unresolved local var: MeReal a10@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal a20@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal a11@[???]
                       Unresolved local var: MeReal a21@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal a22@[???] */
  fVar1 = (b->world->params).lengthScale;
  MVar2 = (*i)[1];
  MVar3 = (*i)[2];
  fVar4 = i[1][0];
  fVar1 = (b->world->params).massScale * fVar1 * fVar1;
  fVar5 = i[1][2];
  fVar6 = i[2][0];
  fVar8 = fVar1 * 100.0;
  fVar7 = i[2][1];
  if ((*i)[0] < fVar8) {
    fVar8 = (*i)[0];
  }
  if (fVar8 < fVar1 * 0.01) {
    fVar8 = fVar1 * 0.01;
  }
  fVar9 = fVar1 * 100.0;
  if (i[1][1] < fVar9) {
    fVar9 = i[1][1];
  }
  if (fVar9 < fVar1 * 0.01) {
    fVar9 = fVar1 * 0.01;
  }
  fVar10 = fVar1 * 100.0;
  if (i[2][2] < fVar10) {
    fVar10 = i[2][2];
  }
  if (fVar10 < fVar1 * 0.01) {
    fVar10 = fVar1 * 0.01;
  }
  (b->keaBody).I0[0] = fVar8;
  (b->keaBody).I1[0] = fVar4;
  (b->keaBody).I2[0] = fVar6;
  (b->keaBody).I0[1] = MVar2;
  (b->keaBody).I1[1] = fVar9;
  (b->keaBody).I2[1] = fVar7;
  (b->keaBody).I0[2] = MVar3;
  (b->keaBody).I1[2] = fVar5;
  (b->keaBody).I2[2] = fVar10;
  fVar1 = fVar7 * fVar6 - fVar10 * fVar4;
  fVar11 = fVar4 * fVar7 - fVar9 * fVar6;
  fVar13 = 1.0 / (fVar6 * fVar11 + fVar4 * fVar1 + (fVar9 * fVar10 - fVar7 * fVar7) * fVar8);
  fVar1 = fVar1 * fVar13;
  fVar11 = fVar11 * fVar13;
  fVar12 = (fVar6 * fVar4 - fVar7 * fVar8) * fVar13;
  (b->keaBody).invI0[0] = (fVar9 * fVar10 - fVar7 * fVar5) * fVar13;
  (b->keaBody).invI0[1] = fVar1;
  (b->keaBody).invI0[2] = fVar11;
  (b->keaBody).invI1[2] = fVar12;
  (b->keaBody).invI1[1] = (fVar10 * fVar8 - fVar6 * fVar6) * fVar13;
  (b->keaBody).invI2[2] = (fVar8 * fVar9 - fVar4 * fVar4) * fVar13;
  (b->keaBody).invI1[0] = fVar1;
  (b->keaBody).invI2[0] = fVar11;
  (b->keaBody).invI2[1] = fVar12;
  return;
}


/* ==== MdtBodySetSphericalInertiaTensor  @ 00011330 ==== */

void MdtBodySetSphericalInertiaTensor(MdtBodyID b,MeReal i)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  MeMatrix3 moi;
  
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal clamped@[???]
                       Unresolved local var: MeReal r@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal a00@[???]
                       Unresolved local var: MeReal a10@[DW_OP_reg18(ST7)]
                       Unresolved local var: MeReal a20@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal a11@[???]
                       Unresolved local var: MeReal a21@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal a22@[???] */
  (b->keaBody).I0[0] = i;
  (b->keaBody).I1[0] = 0.0;
  (b->keaBody).I2[0] = 0.0;
  (b->keaBody).I0[1] = 0.0;
  (b->keaBody).I1[1] = i;
  (b->keaBody).I2[1] = 0.0;
  (b->keaBody).I0[2] = 0.0;
  (b->keaBody).I1[2] = 0.0;
  (b->keaBody).I2[2] = i;
  fVar2 = 0.0 - i * 0.0;
  fVar3 = 0.0 - i * 0.0;
  fVar1 = 1.0 / (fVar3 * 0.0 + fVar2 * 0.0 + (i * i - 0.0) * i);
  fVar2 = fVar2 * fVar1;
  fVar3 = fVar3 * fVar1;
  fVar4 = (0.0 - i * 0.0) * fVar1;
  (b->keaBody).invI0[0] = (i * i - 0.0) * fVar1;
  (b->keaBody).invI0[1] = fVar2;
  (b->keaBody).invI0[2] = fVar3;
  (b->keaBody).invI1[2] = fVar4;
  (b->keaBody).invI1[1] = (i * i - 0.0) * fVar1;
  (b->keaBody).invI2[2] = (i * i - 0.0) * fVar1;
  (b->keaBody).invI1[0] = fVar2;
  (b->keaBody).invI2[0] = fVar3;
  (b->keaBody).invI2[1] = fVar4;
  return;
}


/* ==== MdtBodySetCenterOfMassRelativePosition  @ 00011490 ==== */

void MdtBodySetCenterOfMassRelativePosition(MdtBodyID b,MeReal *pos)

{
  MeBool MVar1;
  
  if ((b->constraintDict).nodecount == 0) {
    b->com[0] = *pos;
    b->com[1] = pos[1];
    b->com[2] = pos[2];
    MVar1 = 0;
    if (((*pos != 0.0) || (pos[1] != 0.0)) || (pos[2] != 0.0)) {
      MVar1 = 1;
    }
    b->useCom = MVar1;
    UpdateCOMTransform();
    return;
  }
  return;
}


/* ==== MdtBodySetCenterOfMassPosition  @ 00011510 ==== */

void MdtBodySetCenterOfMassPosition(MdtBodyID b,MeReal *pos)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  MeBool MVar6;
  MeVector3 tmp_1;
  MeVector3 tmp;
  
  fVar1 = *pos - b->bodyTM[3][0];
  fVar4 = pos[1] - b->bodyTM[3][1];
  fVar5 = pos[2] - b->bodyTM[3][2];
  fVar2 = fVar5 * b->bodyTM[0][2] + fVar1 * b->bodyTM[0][0] + fVar4 * b->bodyTM[0][1];
  fVar3 = fVar5 * b->bodyTM[1][2] + fVar1 * b->bodyTM[1][0] + fVar4 * b->bodyTM[1][1];
  fVar1 = fVar5 * b->bodyTM[2][2] + fVar1 * b->bodyTM[2][0] + fVar4 * b->bodyTM[2][1];
  if ((b->constraintDict).nodecount == 0) {
    b->com[0] = fVar2;
    b->com[1] = fVar3;
    b->com[2] = fVar1;
    MVar6 = 0;
    if (((fVar2 != 0.0) || (fVar3 != 0.0)) || (fVar1 != 0.0)) {
      MVar6 = 1;
    }
    b->useCom = MVar6;
    UpdateCOMTransform(b);
  }
  return;
}


/* ==== MdtBodySetCenterOfMassRelativeTransform  @ 00011620 ==== */

void MdtBodySetCenterOfMassRelativeTransform(MdtBodyID b,MeReal (*t) [4])

{
  MeBool MVar1;
  
  if ((b->constraintDict).nodecount == 0) {
    b->com[0] = t[3][0];
    b->com[1] = t[3][1];
    b->com[2] = t[3][2];
    MVar1 = 0;
    if (((t[3][0] != 0.0) || (t[3][1] != 0.0)) || (t[3][2] != 0.0)) {
      MVar1 = 1;
    }
    b->useCom = MVar1;
    UpdateCOMTransform();
    return;
  }
  return;
}


/* ==== MdtBodySetSafeTime  @ 000116b0 ==== */

void MdtBodySetSafeTime(MdtBodyID b,MeReal t)

{
  b->safeTime = t;
  return;
}


/* ==== MdtBodySetSortKey  @ 000116d0 ==== */

void MdtBodySetSortKey(MdtBodyID b,MeI32 key)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  
  uVar3 = MeDictDelete(b->world,&b->worldNode);
  uVar2 = (undefined4)((ulonglong)uVar3 >> 0x20);
  uVar1 = (undefined4)uVar3;
  if ((b->flags & 1) != 0) {
    uVar1 = MeDictDelete(&b->world->enabledBodyDict,&b->worldEnabledNode,uVar2,uVar2);
  }
  b->sortKey = key;
  uVar1 = MeDictInsert(b->world,&b->worldNode,key,uVar1);
  if ((b->flags & 1) != 0) {
    MeDictInsert(&b->world->enabledBodyDict,&b->worldEnabledNode,key,uVar1);
  }
  return;
}


/* ==== MdtBodyEnable  @ 00011770 ==== */

void __regparm1 MdtBodyEnable(MdtBodyID b)

{
  code *pcVar1;
  uint uVar2;
  int in_stack_00000004;
  
  uVar2 = *(uint *)(in_stack_00000004 + 0x1ec);
  if ((uVar2 & 1) == 0) {
    MeDictInsert(*(int *)(in_stack_00000004 + 0x1a4) + 0x68,in_stack_00000004 + 0x1c4,
                 *(undefined4 *)(in_stack_00000004 + 0x1a8),uVar2,b);
    *(uint *)(in_stack_00000004 + 0x1ec) = *(uint *)(in_stack_00000004 + 0x1ec) | 1;
    *(int *)(*(int *)(in_stack_00000004 + 0x1a4) + 0xbc) =
         *(int *)(*(int *)(in_stack_00000004 + 0x1a4) + 0xbc) + 1;
    pcVar1 = *(code **)(*(int *)(in_stack_00000004 + 0x1a4) + 0x1cc);
    if (pcVar1 != (code *)0x0) {
      (*pcVar1)();
    }
    uVar2 = *(uint *)(in_stack_00000004 + 0x1ec);
  }
  *(undefined4 *)(in_stack_00000004 + 0x230) = 0;
  *(uint *)(in_stack_00000004 + 0x1ec) = uVar2 & 0xfffffffb;
  return;
}


/* ==== MdtBodyResetForces  @ 00011810 ==== */

void MdtBodyResetForces(MdtBodyID b)

{
  (b->keaBody).force[0] = 0.0;
  (b->keaBody).force[1] = 0.0;
  (b->keaBody).force[2] = 0.0;
  (b->keaBody).force[3] = 0.0;
  (b->keaBody).torque[0] = 0.0;
  (b->keaBody).torque[1] = 0.0;
  (b->keaBody).torque[2] = 0.0;
  (b->keaBody).torque[3] = 0.0;
  return;
}


/* ==== MdtBodyResetImpulses  @ 00011850 ==== */

void MdtBodyResetImpulses(MdtBodyID b)

{
  b->impulseLinear[0] = 0.0;
  b->impulseLinear[1] = 0.0;
  b->impulseLinear[2] = 0.0;
  b->impulseLinear[3] = 0.0;
  b->impulseAngular[0] = 0.0;
  b->impulseAngular[1] = 0.0;
  b->impulseAngular[2] = 0.0;
  b->impulseAngular[3] = 0.0;
  return;
}


/* ==== MdtBodyForAllConstraints  @ 000118b0 ==== */

/* WARNING: Unknown calling convention */

void MdtBodyForAllConstraints(MdtBodyID b,MdtConstraintIteratorCBPtr cb,void *ccbdata)

{
  MdtConstraintID pMVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * nextNode@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDict * dict@[DW_OP_reg7(EDI)] */
  iVar3 = MeDictFirst(&b->constraintDict);
  uVar2 = extraout_ECX;
  while (iVar3 != 0) {
                    /* Unresolved local var: MdtBaseConstraint * cp@[DW_OP_reg6(ESI)] */
    pMVar1 = *(MdtConstraintID *)(iVar3 + 0x14);
    iVar3 = MeDictNext(&b->constraintDict,iVar3,uVar2,uVar2);
    (*cb)(pMVar1,ccbdata);
    uVar2 = extraout_ECX_00;
  }
  return;
}


/* ==== MdtBodyDisableConstraints  @ 00011900 ==== */

/* WARNING: Unknown calling convention */

void MdtBodyDisableConstraints(MdtBodyID b)

{
  int iVar1;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDict * dict@[DW_OP_reg6(ESI)] */
  for (iVar1 = MeDictFirst(&b->constraintDict); iVar1 != 0;
      iVar1 = MeDictNext(&b->constraintDict,iVar1)) {
    MdtConstraintDisable(*(undefined4 *)(iVar1 + 0x14));
  }
  return;
}


/* ==== MdtBodyDestroyConstraints  @ 00011950 ==== */

/* WARNING: Unknown calling convention */

void MdtBodyDestroyConstraints(MdtBodyID b)

{
  undefined4 uVar1;
  int iVar2;
  int unaff_ESI;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * nextNode@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeDict * dict@[DW_OP_reg7(EDI)] */
  iVar2 = MeDictFirst(&b->constraintDict);
  while (iVar2 != 0) {
                    /* Unresolved local var: MdtBaseConstraint * cp@[DW_OP_reg3(EBX)] */
    uVar1 = *(undefined4 *)(iVar2 + 0x14);
    iVar2 = MeDictNext(&b->constraintDict,iVar2,unaff_ESI,unaff_ESI);
    MdtConstraintDisable(uVar1);
    MdtConstraintDestroy(uVar1);
    unaff_ESI = iVar2;
  }
  return;
}


/* ==== MdtBodyDisableContacts  @ 000119a0 ==== */

/* WARNING: Unknown calling convention */

void MdtBodyDisableContacts(MdtBodyID b)

{
  int iVar1;
  int iVar2;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeDict * dict@[DW_OP_reg6(ESI)] */
  for (iVar1 = MeDictFirst(&b->constraintDict); iVar1 != 0;
      iVar1 = MeDictNext(&b->constraintDict,iVar1,iVar2,iVar2)) {
                    /* Unresolved local var: MdtBaseConstraint * cp@[DW_OP_reg2(EDX)] */
    iVar2 = iVar1;
    if (*(int *)(*(int *)(iVar1 + 0x14) + 0xb0) - 4U < 2) {
      iVar2 = MdtConstraintDisable(*(int *)(iVar1 + 0x14));
    }
  }
  return;
}


/* ==== MdtBodyDestroyContacts  @ 00011a00 ==== */

/* WARNING: Unknown calling convention */

void MdtBodyDestroyContacts(MdtBodyID b)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  undefined4 extraout_EDX;
  undefined8 uVar4;
  
                    /* Unresolved local var: MeDictNode * node@[DW_OP_reg0(EAX)]
                       Unresolved local var: MeDictNode * nextNode@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeDict * dict@[DW_OP_reg7(EDI)] */
  uVar4 = MeDictFirst(&b->constraintDict);
  while( true ) {
    if ((int)uVar4 == 0) break;
                    /* Unresolved local var: MdtBaseConstraint * cp@[DW_OP_reg3(EBX)] */
    iVar1 = *(int *)((int)uVar4 + 0x14);
    uVar2 = MeDictNext(&b->constraintDict,uVar4,(int)((ulonglong)uVar4 >> 0x20));
    uVar3 = *(int *)(iVar1 + 0xb0) - 4;
    uVar4 = CONCAT44(uVar3,uVar2);
    if (uVar3 < 2) {
      MdtConstraintDisable(iVar1);
      MdtConstraintDestroy(iVar1);
      uVar4 = CONCAT44(extraout_EDX,uVar2);
    }
  }
  return;
}


/* ==== MdtBodyAddForce  @ 00011a70 ==== */

void MdtBodyAddForce(MdtBodyID b,MeReal fx,MeReal fy,MeReal fz)

{
  float fVar1;
  float fVar2;
  
  fVar1 = (b->keaBody).force[0];
  fVar2 = (b->keaBody).force[2];
  (b->keaBody).force[1] = fy + (b->keaBody).force[1];
  (b->keaBody).force[0] = fVar1 + fx;
  (b->keaBody).force[2] = fz + fVar2;
  return;
}


/* ==== MdtBodyAddForceAtPosition  @ 00011aa0 ==== */

void MdtBodyAddForceAtPosition
               (MdtBodyID b,MeReal fx,MeReal fy,MeReal fz,MeReal px,MeReal py,MeReal pz)

{
  float fVar1;
  float fVar2;
  float fVar3;
  MeVector3 t;
  MeVector3 f;
  MeVector3 d;
  
  fVar1 = px - (b->bodyTM[0][0] * b->com[0] + b->bodyTM[1][0] * b->com[1] +
                b->bodyTM[2][0] * b->com[2] + b->bodyTM[3][0]);
  fVar2 = py - (b->bodyTM[2][1] * b->com[2] +
                b->bodyTM[0][1] * b->com[0] + b->bodyTM[1][1] * b->com[1] + b->bodyTM[3][1]);
  fVar3 = pz - (b->bodyTM[2][2] * b->com[2] +
                b->bodyTM[0][2] * b->com[0] + b->bodyTM[1][2] * b->com[1] + b->bodyTM[3][2]);
  (b->keaBody).force[0] = (b->keaBody).force[0] + fx;
  (b->keaBody).force[1] = fy + (b->keaBody).force[1];
  (b->keaBody).force[2] = fz + (b->keaBody).force[2];
  (b->keaBody).torque[0] = (fVar2 * fz - fVar3 * fy) + (b->keaBody).torque[0];
  (b->keaBody).torque[1] = (fVar3 * fx - fz * fVar1) + (b->keaBody).torque[1];
  (b->keaBody).torque[2] = (fVar1 * fy - fVar2 * fx) + (b->keaBody).torque[2];
  return;
}


/* ==== MdtBodyAddTorque  @ 00011bd0 ==== */

void MdtBodyAddTorque(MdtBodyID b,MeReal tx,MeReal ty,MeReal tz)

{
  float fVar1;
  float fVar2;
  
  fVar1 = (b->keaBody).torque[0];
  fVar2 = (b->keaBody).torque[2];
  (b->keaBody).torque[1] = ty + (b->keaBody).torque[1];
  (b->keaBody).torque[0] = fVar1 + tx;
  (b->keaBody).torque[2] = tz + fVar2;
  return;
}


/* ==== MdtBodyAddImpulse  @ 00011c00 ==== */

void MdtBodyAddImpulse(MdtBodyID b,MeReal ix,MeReal iy,MeReal iz)

{
  float fVar1;
  float fVar2;
  float fVar3;
  
  fVar1 = b->impulseLinear[0];
  fVar2 = b->impulseLinear[1];
  fVar3 = b->impulseLinear[2];
  b->impulseAdded = 1;
  b->impulseLinear[1] = iy + fVar2;
  b->impulseLinear[0] = fVar1 + ix;
  b->impulseLinear[2] = iz + fVar3;
  return;
}


/* ==== MdtBodyAddImpulseAtPosition  @ 00011c50 ==== */

void MdtBodyAddImpulseAtPosition
               (MdtBodyID b,MeReal ix,MeReal iy,MeReal iz,MeReal px,MeReal py,MeReal pz)

{
  float fVar1;
  float fVar2;
  float fVar3;
  MeVector3 t;
  MeVector3 i;
  MeVector3 d;
  
  fVar1 = px - (b->bodyTM[0][0] * b->com[0] + b->bodyTM[1][0] * b->com[1] +
                b->bodyTM[2][0] * b->com[2] + b->bodyTM[3][0]);
  fVar2 = py - (b->bodyTM[2][1] * b->com[2] +
                b->bodyTM[0][1] * b->com[0] + b->bodyTM[1][1] * b->com[1] + b->bodyTM[3][1]);
  fVar3 = pz - (b->bodyTM[2][2] * b->com[2] +
                b->bodyTM[0][2] * b->com[0] + b->bodyTM[1][2] * b->com[1] + b->bodyTM[3][2]);
  b->impulseLinear[0] = b->impulseLinear[0] + ix;
  b->impulseLinear[1] = iy + b->impulseLinear[1];
  b->impulseLinear[2] = iz + b->impulseLinear[2];
  b->impulseAngular[0] = (fVar2 * iz - fVar3 * iy) + b->impulseAngular[0];
  b->impulseAngular[1] = (fVar3 * ix - iz * fVar1) + b->impulseAngular[1];
  b->impulseAdded = 1;
  b->impulseAngular[2] = (fVar1 * iy - fVar2 * ix) + b->impulseAngular[2];
  return;
}


/* ==== MdtBodyIsMovingTest  @ 00011db0 ==== */

MeBool MdtBodyIsMovingTest(MdtBodyID b,MdtPartitionParams *params)

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
  float fVar10;
  float fVar11;
  float fVar12;
  MeBool MVar13;
  
                    /* Unresolved local var: MeReal ss_vel@[???]
                       Unresolved local var: MeReal ss_velrot@[???]
                       Unresolved local var: MeReal ss_acc@[???]
                       Unresolved local var: MeReal ss_accrot@[???] */
                    /* Unresolved local var: MeReal m@[DW_OP_reg12(ST1)] */
  fVar1 = (b->keaBody).vel[0];
  fVar2 = (b->keaBody).vel[1];
                    /* Unresolved local var: MeReal m@[DW_OP_reg15(ST4)] */
  fVar3 = (b->keaBody).accel[0];
                    /* Unresolved local var: MeReal m@[DW_OP_reg14(ST3)] */
  fVar4 = (b->keaBody).velrot[0];
  fVar5 = (b->keaBody).vel[2];
                    /* Unresolved local var: MeReal m@[DW_OP_reg13(ST2)] */
  fVar6 = (b->keaBody).accelrot[0];
  fVar7 = (b->keaBody).accel[1];
  fVar8 = (b->keaBody).accel[2];
  fVar9 = (b->keaBody).velrot[1];
  fVar10 = (b->keaBody).velrot[2];
  fVar11 = (b->keaBody).accelrot[1];
  fVar12 = (b->keaBody).accelrot[2];
  if ((((params->vel_thresh < fVar1 * fVar1 + fVar2 * fVar2 + fVar5 * fVar5) ||
       (params->velrot_thresh < fVar10 * fVar10 + fVar9 * fVar9 + fVar4 * fVar4)) ||
      (params->acc_thresh < fVar8 * fVar8 + fVar7 * fVar7 + fVar3 * fVar3)) ||
     ((params->accrot_thresh < fVar12 * fVar12 + fVar11 * fVar11 + fVar6 * fVar6 ||
      ((b->enabledTime < params->alive_time_thresh && ((b->flags & 4) == 0)))))) {
    MVar13 = 1;
  }
  else {
    MVar13 = 0;
  }
  return MVar13;
}


/* ==== MdtBodyEnableNonSphericalInertia  @ 00011eb0 ==== */

void MdtBodyEnableNonSphericalInertia(MdtBodyID b)

{
  (b->keaBody).flags = (b->keaBody).flags | 4;
  return;
}


/* ==== MdtBodyDisableNonSphericalInertia  @ 00011ed0 ==== */

void MdtBodyDisableNonSphericalInertia(MdtBodyID b)

{
  (b->keaBody).flags = (b->keaBody).flags & 0xfffffffb;
  return;
}


/* ==== MdtBodyNonSphericalInertiaIsEnabled  @ 00011ef0 ==== */

MeBool MdtBodyNonSphericalInertiaIsEnabled(MdtBodyID b)

{
  return (uint)(b->keaBody).flags >> 2 & 1;
}


/* ==== MdtBodyEnableCoriolisForce  @ 00011f10 ==== */

void MdtBodyEnableCoriolisForce(MdtBodyID b)

{
  (b->keaBody).flags = (b->keaBody).flags | 8;
  return;
}


/* ==== MdtBodyDisableCoriolisForce  @ 00011f30 ==== */

void MdtBodyDisableCoriolisForce(MdtBodyID b)

{
  (b->keaBody).flags = (b->keaBody).flags & 0xfffffff7;
  return;
}


/* ==== MdtBodyCoriolisForceIsEnabled  @ 00011f50 ==== */

MeBool MdtBodyCoriolisForceIsEnabled(MdtBodyID b)

{
  return (uint)(b->keaBody).flags >> 3 & 1;
}


/* ==== MeVector3Normalize  @ 00011f70 ==== */

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


/* ==== MeDictDelete  @ 00013000 ==== */

/* WARNING: Control flow encountered bad instruction data */

void MeDictDelete(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== MeDictNodeInit  @ 00013008 ==== */

/* WARNING: Control flow encountered bad instruction data */

void MeDictNodeInit(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== MeDictInsert  @ 0001300c ==== */

/* WARNING: Control flow encountered bad instruction data */

void MeDictInsert(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== MeDictInit  @ 00013014 ==== */

/* WARNING: Control flow encountered bad instruction data */

void MeDictInit(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== MeDictAllowDupes  @ 00013018 ==== */

/* WARNING: Control flow encountered bad instruction data */

void MeDictAllowDupes(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== MeDictFirst  @ 0001301c ==== */

/* WARNING: Control flow encountered bad instruction data */

void MeDictFirst(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== MeDictNext  @ 00013020 ==== */

/* WARNING: Control flow encountered bad instruction data */

void MeDictNext(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== MdtConstraintDisable  @ 00013024 ==== */

/* WARNING: Control flow encountered bad instruction data */

void MdtConstraintDisable(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== MdtConstraintDestroy  @ 00013028 ==== */

/* WARNING: Control flow encountered bad instruction data */

void MdtConstraintDestroy(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== MeFatalError  @ 0001302c ==== */

/* WARNING: Control flow encountered bad instruction data */

void MeFatalError(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


/* ==== UpdateCOMTransform  @ 00013030 ==== */

/* WARNING: Control flow encountered bad instruction data */

void UpdateCOMTransform(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


