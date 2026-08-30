/* ==== MdtDictCompare ==== */

int MdtDictCompare(void *v1,void *v2)

{
  uint uVar1;
  
                    /* Unresolved local var: int i1@[???]
                       Unresolved local var: int i2@[???] */
  uVar1 = 0xffffffff;
  if ((int)v2 <= (int)v1) {
    uVar1 = (uint)((int)v2 < (int)v1);
  }
  return uVar1;
}


/* ==== MdtConvertVector ==== */

void MdtConvertVector(MdtBodyID from_body,MeReal *f,MdtBodyID to_body,MeReal *t)

{
  float fVar1;
  float fVar2;
  float fVar3;
  MeVector3 tmp;
  
  if (from_body == (MdtBodyID)0x0) {
    tmp[0] = *f;
    tmp[1] = f[1];
    tmp[2] = f[2];
  }
  else {
    fVar1 = *f;
    fVar2 = f[1];
    fVar3 = f[2];
    tmp[0] = fVar3 * from_body->bodyTM[2][0] +
             fVar2 * from_body->bodyTM[1][0] + fVar1 * from_body->bodyTM[0][0];
    tmp[1] = fVar3 * from_body->bodyTM[2][1] +
             fVar1 * from_body->bodyTM[0][1] + fVar2 * from_body->bodyTM[1][1];
    tmp[2] = fVar3 * from_body->bodyTM[2][2] +
             fVar1 * from_body->bodyTM[0][2] + fVar2 * from_body->bodyTM[1][2];
  }
  if (to_body == (MdtBodyID)0x0) {
    *t = tmp[0];
    t[1] = tmp[1];
    t[2] = tmp[2];
  }
  else {
    *t = tmp[2] * to_body->bodyTM[0][2] +
         tmp[1] * to_body->bodyTM[0][1] + tmp[0] * to_body->bodyTM[0][0];
    t[1] = tmp[2] * to_body->bodyTM[1][2] +
           tmp[0] * to_body->bodyTM[1][0] + tmp[1] * to_body->bodyTM[1][1];
    t[2] = tmp[2] * to_body->bodyTM[2][2] +
           tmp[0] * to_body->bodyTM[2][0] + tmp[1] * to_body->bodyTM[2][1];
  }
  return;
}


/* ==== MdtConvertPositionVector ==== */

void MdtConvertPositionVector(MdtBodyID from_body,MeReal *f,MdtBodyID to_body,MeReal *t)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  MeVector3 tmp_1;
  MeVector3 tmp;
  
  if (from_body == (MdtBodyID)0x0) {
    if (to_body == (MdtBodyID)0x0) {
      *t = *f;
      t[1] = f[1];
      t[2] = f[2];
      return;
    }
    fVar1 = *f - to_body->bodyTM[3][0];
    fVar3 = f[1] - to_body->bodyTM[3][1];
    fVar2 = f[2] - to_body->bodyTM[3][2];
    *t = fVar2 * to_body->bodyTM[0][2] +
         fVar1 * to_body->bodyTM[0][0] + fVar3 * to_body->bodyTM[0][1];
    t[1] = fVar2 * to_body->bodyTM[1][2] +
           fVar1 * to_body->bodyTM[1][0] + fVar3 * to_body->bodyTM[1][1];
    fVar1 = fVar1 * to_body->bodyTM[2][0] + fVar3 * to_body->bodyTM[2][1] +
            fVar2 * to_body->bodyTM[2][2];
  }
  else {
    if (to_body == (MdtBodyID)0x0) {
      *t = from_body->bodyTM[2][0] * f[2] +
           from_body->bodyTM[1][0] * f[1] + from_body->bodyTM[0][0] * *f + from_body->bodyTM[3][0];
      t[1] = from_body->bodyTM[2][1] * f[2] +
             from_body->bodyTM[1][1] * f[1] + from_body->bodyTM[0][1] * *f + from_body->bodyTM[3][1]
      ;
      t[2] = from_body->bodyTM[2][2] * f[2] +
             from_body->bodyTM[1][2] * f[1] + from_body->bodyTM[0][2] * *f + from_body->bodyTM[3][2]
      ;
      return;
    }
    fVar1 = *f;
    fVar2 = f[1];
    fVar3 = f[2];
    fVar4 = (fVar3 * from_body->bodyTM[2][0] +
             fVar1 * from_body->bodyTM[0][0] + fVar2 * from_body->bodyTM[1][0] +
            from_body->bodyTM[3][0]) - to_body->bodyTM[3][0];
    fVar5 = (fVar3 * from_body->bodyTM[2][1] +
             fVar1 * from_body->bodyTM[0][1] + fVar2 * from_body->bodyTM[1][1] +
            from_body->bodyTM[3][1]) - to_body->bodyTM[3][1];
    fVar1 = (fVar1 * from_body->bodyTM[0][2] + fVar2 * from_body->bodyTM[1][2] +
             fVar3 * from_body->bodyTM[2][2] + from_body->bodyTM[3][2]) - to_body->bodyTM[3][2];
    *t = fVar1 * to_body->bodyTM[0][2] +
         fVar4 * to_body->bodyTM[0][0] + fVar5 * to_body->bodyTM[0][1];
    t[1] = fVar1 * to_body->bodyTM[1][2] +
           fVar4 * to_body->bodyTM[1][0] + fVar5 * to_body->bodyTM[1][1];
    fVar1 = fVar1 * to_body->bodyTM[2][2] +
            fVar4 * to_body->bodyTM[2][0] + fVar5 * to_body->bodyTM[2][1];
  }
  t[2] = fVar1;
  return;
}


/* ==== ConvertCOMVector ==== */

void ConvertCOMVector(MdtBodyID from_body,MeReal *f,MdtBodyID to_body,MeReal *t)

{
  MdtConvertVector(from_body,f,to_body,t);
  return;
}


/* ==== ConvertCOMPositionVector ==== */

void ConvertCOMPositionVector(MdtBodyID from_body,MeReal *f,MdtBodyID to_body,MeReal *t)

{
  MeVector3 tmp;
  
  if (from_body != (MdtBodyID)0x0) {
    tmp[0] = from_body->com[0] + *f;
    tmp[1] = from_body->com[1] + f[1];
    tmp[2] = from_body->com[2] + f[2];
    f = tmp;
  }
  MdtConvertPositionVector(from_body,f,to_body,t);
  if (to_body != (MdtBodyID)0x0) {
    *t = *t - to_body->com[0];
    t[1] = t[1] - to_body->com[1];
    t[2] = t[2] - to_body->com[2];
  }
  return;
}


/* ==== MdtMakeInertiaTensorSphere ==== */

void MdtMakeInertiaTensorSphere(MeReal mass,MeReal radius,MeVector3 *i)

{
  float fVar1;
  
                    /* Unresolved local var: int m@[???]
                       Unresolved local var: int n@[???]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
  (*i)[1] = 0.0;
  (*i)[2] = 0.0;
  i[1][0] = 0.0;
  i[1][2] = 0.0;
  fVar1 = mass * radius * radius * 0.4;
  i[2][0] = 0.0;
  i[2][1] = 0.0;
  (*i)[0] = fVar1;
  i[1][1] = fVar1;
  i[2][2] = fVar1;
  return;
}


/* ==== MdtMakeInertiaTensorBox ==== */

void MdtMakeInertiaTensorBox(MeReal mass,MeReal lx,MeReal ly,MeReal lz,MeVector3 *i)

{
                    /* Unresolved local var: int m@[???]
                       Unresolved local var: int n@[???] */
  (*i)[1] = 0.0;
  (*i)[2] = 0.0;
  i[1][0] = 0.0;
  i[1][2] = 0.0;
  i[2][0] = 0.0;
  (*i)[0] = (ly * ly + lz * lz) * mass * 0.083333336;
  i[1][1] = (lz * lz + lx * lx) * mass * 0.083333336;
  i[2][2] = (lx * lx + ly * ly) * mass * 0.083333336;
  i[2][1] = 0.0;
  return;
}


/* ==== MdtLimitController ==== */

void MdtLimitController(MdtLimitID limit,MeReal desiredPosition,MeReal gap,MeReal maxSpeed,
                       MeReal maxForce)

{
  int iVar1;
  float fVar2;
  MeReal error;
  MeReal maxSpeed_local;
  MeReal gap_local;
  MeReal desiredPosition_local;
  
                    /* Unresolved local var: MeReal current@[???] */
  fVar2 = MdtLimitGetPosition(limit);
  fVar2 = fVar2 - desiredPosition;
  iVar1 = MdtLimitPositionIsCalculated(limit);
  if (iVar1 == 0) {
    MdtLimitCalculatePosition(limit,1);
  }
  desiredPosition = maxSpeed;
  if (-gap <= fVar2) {
    if (gap < fVar2) {
      maxSpeed_local = -maxSpeed;
      desiredPosition = maxSpeed_local;
    }
    else {
      error = -fVar2;
      desiredPosition = (maxSpeed / gap) * error;
    }
  }
                    /* Unresolved local var: MeReal vel@[???] */
  MdtLimitSetLimitedForceMotor(limit,desiredPosition,maxForce);
  return;
}


/* ==== UpdateBodyTransform ==== */

void UpdateBodyTransform(MdtBodyID b,MeReal (*keaTM) [4])

{
  float fVar1;
  float fVar2;
  float fVar3;
  int iVar4;
  MeReal *pMVar5;
  MeReal (*paMVar6) [4];
  MeVector3 tmp;
  
  iVar4 = 0x10;
  pMVar5 = *keaTM;
  paMVar6 = b->comTM;
  for (; iVar4 != 0; iVar4 = iVar4 + -1) {
    (*paMVar6)[0] = *pMVar5;
    pMVar5 = pMVar5 + 1;
    paMVar6 = (MeReal (*) [4])(*paMVar6 + 1);
  }
  iVar4 = 0x10;
  pMVar5 = *keaTM;
  paMVar6 = b->bodyTM;
  for (; iVar4 != 0; iVar4 = iVar4 + -1) {
    (*paMVar6)[0] = *pMVar5;
    pMVar5 = pMVar5 + 1;
    paMVar6 = (MeReal (*) [4])(*paMVar6 + 1);
  }
  if (b->useCom != 0) {
    fVar1 = b->com[0] * -1.0;
    fVar2 = b->com[1] * -1.0;
    fVar3 = b->com[2] * -1.0;
    b->bodyTM[3][0] = fVar3 * keaTM[2][0] + fVar1 * (*keaTM)[0] + fVar2 * keaTM[1][0] + keaTM[3][0];
    b->bodyTM[3][1] = fVar3 * keaTM[2][1] + fVar1 * (*keaTM)[1] + fVar2 * keaTM[1][1] + keaTM[3][1];
    b->bodyTM[3][2] = fVar3 * keaTM[2][2] + fVar1 * (*keaTM)[2] + fVar2 * keaTM[1][2] + keaTM[3][2];
  }
  return;
}


/* ==== UpdateCOMTransform ==== */

void UpdateCOMTransform(MdtBodyID b)

{
  float fVar1;
  int iVar2;
  MeReal (*paMVar3) [4];
  MeReal (*paMVar4) [4];
  
  paMVar3 = b->bodyTM;
  paMVar4 = b->comTM;
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    (*paMVar4)[0] = (*paMVar3)[0];
    paMVar3 = (MeReal (*) [4])(*paMVar3 + 1);
    paMVar4 = (MeReal (*) [4])(*paMVar4 + 1);
  }
  if (b->useCom != 0) {
    fVar1 = b->com[0];
    b->comTM[3][0] =
         b->bodyTM[2][0] * b->com[2] + fVar1 * b->bodyTM[0][0] + b->bodyTM[1][0] * b->com[1] +
         b->bodyTM[3][0];
    b->comTM[3][1] =
         b->bodyTM[2][1] * b->com[2] + b->bodyTM[1][1] * b->com[1] + fVar1 * b->bodyTM[0][1] +
         b->bodyTM[3][1];
    b->comTM[3][2] =
         b->bodyTM[2][2] * b->com[2] + b->bodyTM[1][2] * b->com[1] + b->bodyTM[0][2] * b->com[0] +
         b->bodyTM[3][2];
  }
  MeQuaternionFromTM((b->keaBody).qrot,b->bodyTM);
  return;
}


/* ==== GetCOMTransform ==== */

void GetCOMTransform(MdtBodyID b,MeVector4 *keaTM)

{
  int iVar1;
  MeReal (*paMVar2) [4];
  
  paMVar2 = b->comTM;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    (*keaTM)[0] = (*paMVar2)[0];
    paMVar2 = (MeReal (*) [4])(*paMVar2 + 1);
    keaTM = (MeVector4 *)((int)keaTM + 4);
  }
  return;
}


