/* ==== MdtSingleLimitReset ==== */

void MdtSingleLimitReset(MdtSingleLimitID limit)

{
  limit->stop = 0.0;
  limit->restitution = 1.0;
  limit->stiffness = 3.4028235e+38;
  limit->damping = 0.0;
  return;
}


/* ==== MdtSingleLimitGetStop ==== */

MeReal MdtSingleLimitGetStop(MdtSingleLimitID sl)

{
  return sl->stop;
}


/* ==== MdtSingleLimitGetStiffness ==== */

MeReal MdtSingleLimitGetStiffness(MdtSingleLimitID sl)

{
  return sl->stiffness;
}


/* ==== MdtSingleLimitGetDamping ==== */

MeReal MdtSingleLimitGetDamping(MdtSingleLimitID sl)

{
  return sl->damping;
}


/* ==== MdtSingleLimitGetRestitution ==== */

MeReal MdtSingleLimitGetRestitution(MdtSingleLimitID sl)

{
  return sl->restitution;
}


/* ==== MdtSingleLimitSetStop ==== */

void MdtSingleLimitSetStop(MdtSingleLimitID sl,MeReal NewStop)

{
  sl->stop = NewStop;
  return;
}


/* ==== MdtSingleLimitSetStiffness ==== */

void MdtSingleLimitSetStiffness(MdtSingleLimitID sl,MeReal NewStiffness)

{
  if (NewStiffness < 0.0) {
    sl->stiffness = -NewStiffness;
  }
  else {
    sl->stiffness = NewStiffness;
  }
  return;
}


/* ==== MdtSingleLimitSetDamping ==== */

void MdtSingleLimitSetDamping(MdtSingleLimitID sl,MeReal NewDamping)

{
  if (NewDamping < 0.0) {
    sl->damping = -NewDamping;
  }
  else {
    sl->damping = NewDamping;
  }
  return;
}


/* ==== MdtSingleLimitSetRestitution ==== */

void MdtSingleLimitSetRestitution(MdtSingleLimitID sl,MeReal NewRestitution)

{
  if (0.0 <= NewRestitution) {
    if (NewRestitution <= 1.0) {
      sl->restitution = NewRestitution;
      return;
    }
    if (0.0 <= NewRestitution) {
      sl->restitution = 1.0;
      return;
    }
  }
  sl->restitution = 0.0;
  return;
}


/* ==== MdtLimitReset ==== */

void MdtLimitReset(MdtLimitID limit)

{
  limit->bLimited = 0;
  limit->overshoot = 0.0;
  limit->desired_vel = 0.0;
  limit->fmax = 0.0;
  limit->offset = 0.0;
  limit->position = 0.0;
  limit->previous_position = 0.0;
  limit->velocity = 0.0;
  limit->damping_thresh = 3.4028235e+38;
  limit->bPowered = 0;
  limit->is_locked = 0;
  limit->bCalculatePosition = 0;
  limit->bPositionInitialised = 0;
  limit->bRelaxingToLimit = 0;
  limit->limit[0].restitution = 1.0;
  limit->limit[0].stiffness = 3.4028235e+38;
  limit->limit[0].damping = 0.0;
  limit->limit[0].stop = 0.0;
  limit->limit[1].stop = 0.0;
  limit->limit[1].restitution = 1.0;
  limit->limit[1].stiffness = 3.4028235e+38;
  limit->limit[1].damping = 0.0;
  return;
}


/* ==== MdtLimitIsActive ==== */

MeBool MdtLimitIsActive(MdtLimitID l)

{
  return l->bLimited;
}


/* ==== MdtLimitPositionIsCalculated ==== */

MeBool MdtLimitPositionIsCalculated(MdtLimitID l)

{
  return l->bCalculatePosition;
}


/* ==== MdtLimitGetLowerLimit ==== */

/* WARNING: Unknown calling convention */

MdtSingleLimitID MdtLimitGetLowerLimit(MdtLimitID l)

{
  return l->limit;
}


/* ==== MdtLimitGetUpperLimit ==== */

/* WARNING: Unknown calling convention */

MdtSingleLimitID MdtLimitGetUpperLimit(MdtLimitID l)

{
  return l->limit + 1;
}


/* ==== MdtLimitGetPosition ==== */

MeReal MdtLimitGetPosition(MdtLimitID l)

{
  return l->offset + l->position;
}


/* ==== MdtLimitGetOvershoot ==== */

MeReal MdtLimitGetOvershoot(MdtLimitID l)

{
  return l->overshoot;
}


/* ==== MdtLimitGetVelocity ==== */

MeReal MdtLimitGetVelocity(MdtLimitID l)

{
  return l->velocity;
}


/* ==== MdtLimitGetStiffnessThreshold ==== */

MeReal MdtLimitGetStiffnessThreshold(MdtLimitID l)

{
  return l->damping_thresh;
}


/* ==== MdtLimitIsMotorized ==== */

MeBool MdtLimitIsMotorized(MdtLimitID l)

{
  return l->bPowered;
}


/* ==== MdtLimitGetMotorDesiredVelocity ==== */

MeReal MdtLimitGetMotorDesiredVelocity(MdtLimitID l)

{
  return l->desired_vel;
}


/* ==== MdtLimitGetMotorMaxForce ==== */

MeReal MdtLimitGetMotorMaxForce(MdtLimitID l)

{
  return l->fmax;
}


/* ==== MdtLimitSetLowerLimit ==== */

void MdtLimitSetLowerLimit(MdtLimitID l,MdtSingleLimitID sl)

{
  float fVar1;
  float fVar2;
  
                    /* Unresolved local var: MdtSingleLimitID low_limit@[???]
                       Unresolved local var: MdtSingleLimitID hi_limit@[???] */
                    /* Unresolved local var: MeReal NewStop@[DW_OP_reg13(ST2)] */
  fVar1 = sl->stop;
  fVar2 = sl->stiffness;
  l->limit[0].stop = fVar1;
  if (fVar2 < 0.0) {
    fVar2 = -fVar2;
  }
  l->limit[0].stiffness = fVar2;
  fVar2 = sl->damping;
  if (fVar2 < 0.0) {
    fVar2 = -fVar2;
  }
  l->limit[0].damping = fVar2;
  fVar2 = sl->restitution;
  if (0.0 <= fVar2) {
    if (fVar2 <= 1.0) {
      l->limit[0].restitution = fVar2;
      goto LAB_0001030d;
    }
    if (0.0 <= fVar2) {
      l->limit[0].restitution = 1.0;
      goto LAB_0001030d;
    }
  }
  l->limit[0].restitution = 0.0;
LAB_0001030d:
  if (l->limit[1].stop < fVar1) {
    l->limit[1].stop = fVar1;
  }
  return;
}


/* ==== MdtLimitSetUpperLimit ==== */

void MdtLimitSetUpperLimit(MdtLimitID l,MdtSingleLimitID sl)

{
  float fVar1;
  float fVar2;
  
                    /* Unresolved local var: MdtSingleLimitID low_limit@[???]
                       Unresolved local var: MdtSingleLimitID hi_limit@[???] */
                    /* Unresolved local var: MeReal NewStop@[DW_OP_reg13(ST2)] */
  fVar1 = sl->stop;
  fVar2 = sl->stiffness;
  l->limit[1].stop = fVar1;
  if (fVar2 < 0.0) {
    fVar2 = -fVar2;
  }
  l->limit[1].stiffness = fVar2;
  fVar2 = sl->damping;
  if (fVar2 < 0.0) {
    fVar2 = -fVar2;
  }
  l->limit[1].damping = fVar2;
  fVar2 = sl->restitution;
  if (0.0 <= fVar2) {
    if (fVar2 <= 1.0) {
      l->limit[1].restitution = fVar2;
      goto LAB_000103ad;
    }
    if (0.0 <= fVar2) {
      l->limit[1].restitution = 1.0;
      goto LAB_000103ad;
    }
  }
  l->limit[1].restitution = 0.0;
LAB_000103ad:
  if (fVar1 < l->limit[0].stop) {
    l->limit[0].stop = fVar1;
  }
  return;
}


/* ==== MdtLimitSetPosition ==== */

void MdtLimitSetPosition(MdtLimitID l,MeReal NewPosition)

{
  l->offset = NewPosition - l->position;
  return;
}


/* ==== MdtLimitActivateLimits ==== */

void MdtLimitActivateLimits(MdtLimitID l,MeBool NewActivationState)

{
  MeBool MVar1;
  
  MVar1 = 0;
  l->bLimited = NewActivationState;
  if (((NewActivationState != 0) || (l->bPowered != 0)) || (l->is_locked != 0)) {
    MVar1 = 1;
  }
  l->bCalculatePosition = MVar1;
  return;
}


/* ==== MdtLimitCalculatePosition ==== */

void MdtLimitCalculatePosition(MdtLimitID l,MeBool NewState)

{
  MeBool MVar1;
  
  MVar1 = 0;
  if ((((NewState != 0) || (l->bLimited != 0)) || (l->bPowered != 0)) || (l->is_locked != 0)) {
    MVar1 = 1;
  }
  l->bCalculatePosition = MVar1;
  return;
}


/* ==== MdtLimitSetStiffnessThreshold ==== */

void MdtLimitSetStiffnessThreshold(MdtLimitID l,MeReal NewStiffnessThreshold)

{
  if (NewStiffnessThreshold < 0.0) {
    l->damping_thresh = -NewStiffnessThreshold;
  }
  else {
    l->damping_thresh = NewStiffnessThreshold;
  }
  return;
}


/* ==== MdtLimitActivateMotor ==== */

void MdtLimitActivateMotor(MdtLimitID l,MeBool NewActivationState)

{
  MeBool MVar1;
  
  l->bPowered = NewActivationState;
  if (NewActivationState != 0) {
    l->is_locked = 0;
  }
  MVar1 = 0;
  if (((NewActivationState != 0) || (l->bLimited != 0)) || (l->is_locked != 0)) {
    MVar1 = 1;
  }
  l->bCalculatePosition = MVar1;
  return;
}


/* ==== MdtLimitSetLimitedForceMotor ==== */

void MdtLimitSetLimitedForceMotor(MdtLimitID l,MeReal desiredVelocity,MeReal forceLimit)

{
  l->desired_vel = desiredVelocity;
  l->bPowered = (uint)(forceLimit != 0.0);
  l->is_locked = 0;
  if (0.0 <= forceLimit) {
    l->fmax = forceLimit;
  }
  else {
    l->fmax = -forceLimit;
  }
  return;
}


/* ==== MdtLimitSetLock ==== */

void MdtLimitSetLock(MdtLimitID l,MeReal position,MeReal forceLimit)

{
  l->desired_vel = 0.0;
  l->is_locked = (uint)(forceLimit != 0.0);
  l->bPowered = 0;
  l->position_lock = position;
  if (0.0 <= forceLimit) {
    l->fmax = forceLimit;
  }
  else {
    l->fmax = -forceLimit;
  }
  return;
}


/* ==== MdtLimitIsLocked ==== */

MeBool MdtLimitIsLocked(MdtLimitID l)

{
  return l->is_locked;
}


/* ==== MdtLimitActivateLock ==== */

void MdtLimitActivateLock(MdtLimitID l,MeBool NewActivationState)

{
  MeBool MVar1;
  
  l->is_locked = NewActivationState;
  if (NewActivationState != 0) {
    l->bPowered = 0;
  }
  MVar1 = 0;
  if (((NewActivationState != 0) || (l->bLimited != 0)) || (l->is_locked != 0)) {
    MVar1 = 1;
  }
  l->bCalculatePosition = MVar1;
  return;
}


/* ==== MdtLimitResetState ==== */

void MdtLimitResetState(MdtLimitID l)

{
  l->bPositionInitialised = 0;
  l->bRelaxingToLimit = 0;
  l->overshoot = 0.0;
  l->previous_position = 0.0;
  l->position = 0.0;
  return;
}


