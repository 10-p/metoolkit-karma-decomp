/* ==== McdModelCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

McdModelID_conflict McdModelCreate(McdGeometryID g)

{
  McdModelID pMVar1;
  McdFramework *pMVar2;
  McdModelID_conflict pMVar3;
  
                    /* Unresolved local var: McdFramework * frame@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdModel.conflict * obj@[DW_OP_reg6(ESI)] */
  pMVar2 = (McdFramework *)McdGeometryGetFramework(g);
  pMVar3 = (McdModelID_conflict)(*_MeMemoryAPI)(&pMVar2->modelPool);
  if (pMVar3 != (McdModelID_conflict)0x0) {
    pMVar2->modelCount = pMVar2->modelCount + 1;
    pMVar3->mRelTM = (MeMatrix4Ptr)0x0;
    pMVar3->mRefTM = (MeMatrix4Ptr)0x0;
    pMVar3->mBody = (void *)0x0;
    pMVar3->mRequestID = 0;
    pMVar3->mSpace = (McdSpaceID)0x0;
    pMVar3->mSpaceID = -1;
    pMVar3->mData = (void *)0x0;
    pMVar3->linearVelocity = (MeReal *)0x0;
    pMVar3->angularVelocity = (MeReal *)0x0;
    pMVar3->mTransformUpdateFn = (McdModelUpdateFnPtr)0x0;
    pMVar3->sortKey = 0;
    pMVar3->mIntersectFn = (McdModelIntersectFnPtr)0x0;
    pMVar3->flags = 0;
    McdGeometryInstanceReset(&pMVar3->mInstance);
    pMVar3->mPadding = pMVar2->mDefaultPadding;
    pMVar3->frame = pMVar2;
    McdGeometryInstanceSetGeometry(&pMVar3->mInstance,g);
    if (pMVar2->firstModel == (McdModelID_conflict)0x0) {
      pMVar3->prev = pMVar3;
      pMVar3->next = pMVar3;
    }
    else {
      pMVar1 = pMVar2->firstModel->prev;
      pMVar3->prev = pMVar1;
      pMVar3->next = pMVar2->firstModel;
      pMVar1->next = pMVar3;
      pMVar3->next->prev = pMVar3;
    }
    pMVar2->firstModel = pMVar3;
  }
  return pMVar3;
}


/* ==== McdModelDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm1 McdModelDestroy(McdModelID_conflict cm)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int *in_stack_00000004;
  
                    /* Unresolved local var: McdFramework * frame@[DW_OP_reg6(ESI)] */
  if ((in_stack_00000004[0xf] & 1U) != 0) {
    (*_MeMatrix4TMCompound)(in_stack_00000004[0xc]);
    uVar3 = McdGeometryInstanceGetTransformPtr(in_stack_00000004 + 0x10);
    cm = (McdModelID_conflict)(*_MeMatrix4TMCompound)(uVar3);
    in_stack_00000004[0xf] = in_stack_00000004[0xf] & 0xfffffffe;
  }
  McdGeometryInstanceSetGeometry(in_stack_00000004 + 0x10,0,cm,cm);
  iVar1 = *in_stack_00000004;
  iVar4 = iVar1;
  if (*(int **)(iVar1 + 8) == in_stack_00000004) {
    *(uint *)(iVar1 + 8) =
         (uint)in_stack_00000004[2] & ((int *)in_stack_00000004[2] == in_stack_00000004) - 1;
    iVar4 = *in_stack_00000004;
  }
  iVar2 = in_stack_00000004[2];
  *(int *)(iVar2 + 4) = in_stack_00000004[1];
  *(int *)(in_stack_00000004[1] + 8) = iVar2;
  (*_McdGeometryInstanceGetTransformPtr)(iVar4 + 0x44);
  *(int *)(iVar1 + 0x10) = *(int *)(iVar1 + 0x10) + -1;
  return;
}


/* ==== McdModelReset ==== */

void McdModelReset(McdModelID_conflict obj)

{
  obj->mRelTM = (MeMatrix4Ptr)0x0;
  obj->mRefTM = (MeMatrix4Ptr)0x0;
  obj->mBody = (void *)0x0;
  obj->mRequestID = 0;
  obj->mSpace = (McdSpaceID)0x0;
  obj->mSpaceID = -1;
  obj->mData = (void *)0x0;
  obj->linearVelocity = (MeReal *)0x0;
  obj->angularVelocity = (MeReal *)0x0;
  obj->mTransformUpdateFn = (McdModelUpdateFnPtr)0x0;
  obj->sortKey = 0;
  obj->mIntersectFn = (McdModelIntersectFnPtr)0x0;
  obj->flags = 0;
  McdGeometryInstanceReset();
  return;
}


/* ==== McdModelGetGeometryType ==== */

McdGeometryType McdModelGetGeometryType(McdModelID_conflict cm)

{
  return (McdGeometryType)(byte)((cm->mInstance).mGeometry)->mRefCtAndID;
}


/* ==== McdModelGetSpace ==== */

McdSpaceID McdModelGetSpace(McdModelID_conflict cm)

{
  return cm->mSpace;
}


/* ==== McdModelSetContactTolerance ==== */

void McdModelSetContactTolerance(McdModelID_conflict m,MeReal tol)

{
  m->mPadding = tol;
  return;
}


/* ==== McdModelGetContactTolerance ==== */

MeReal McdModelGetContactTolerance(McdModelID_conflict m)

{
  return m->mPadding;
}


/* ==== McdModelGetBSphere ==== */

void McdModelGetBSphere(McdModelID_conflict m,MeReal *center,MeReal *radius)

{
  McdGeometryInstanceGetBSphere();
  return;
}


/* ==== McdModelUpdate ==== */

void __regparm3 McdModelUpdate(McdModelID_conflict cm)

{
  float fVar1;
  undefined4 uVar2;
  undefined4 in_EDX;
  undefined4 extraout_EDX;
  int in_stack_00000004;
  
  if (*(code **)(in_stack_00000004 + 0x2c) != (code *)0x0) {
    (**(code **)(in_stack_00000004 + 0x2c))();
    in_EDX = extraout_EDX;
  }
  uVar2 = 0;
  if (*(int *)(in_stack_00000004 + 0x30) != 0) {
    uVar2 = MeMatrix4TMCompound(*(undefined4 *)(in_stack_00000004 + 0x44),
                                *(int *)(in_stack_00000004 + 0x30),
                                *(undefined4 *)(in_stack_00000004 + 0x34),in_EDX);
  }
  McdGeometryInstanceUpdateAABB(in_stack_00000004 + 0x40,0,0,uVar2);
  fVar1 = *(float *)(in_stack_00000004 + 0x18);
  *(float *)(in_stack_00000004 + 0x48) = *(float *)(in_stack_00000004 + 0x48) - fVar1;
  *(float *)(in_stack_00000004 + 0x4c) = *(float *)(in_stack_00000004 + 0x4c) - fVar1;
  *(float *)(in_stack_00000004 + 0x50) = *(float *)(in_stack_00000004 + 0x50) - fVar1;
  *(float *)(in_stack_00000004 + 0x54) = *(float *)(in_stack_00000004 + 0x54) + fVar1;
  *(float *)(in_stack_00000004 + 0x58) = *(float *)(in_stack_00000004 + 0x58) + fVar1;
  *(float *)(in_stack_00000004 + 0x5c) = fVar1 + *(float *)(in_stack_00000004 + 0x5c);
  return;
}


/* ==== McdModelUpdatePath ==== */

void McdModelUpdatePath(McdModelID_conflict cm,MeReal motionDuration)

{
  undefined4 in_ECX;
  MeReal (*paMVar1) [4];
  undefined4 extraout_ECX;
  MeReal (*unaff_EBX) [4];
  MeReal tmEnd [4] [4];
  
                    /* Unresolved local var: MeMatrix4Ptr tmEndPtr@[DW_OP_reg1(ECX)] */
  if (cm->mTransformUpdateFn != (McdModelUpdateFnPtr)0x0) {
    (*cm->mTransformUpdateFn)(cm);
    in_ECX = extraout_ECX;
  }
  if (cm->mRelTM != (MeMatrix4Ptr)0x0) {
    MeMatrix4TMCompound((cm->mInstance).mTM,cm->mRelTM,cm->mRefTM,in_ECX);
  }
  paMVar1 = (MeReal (*) [4])0x0;
  if ((cm->linearVelocity != (MeReal *)0x0) && (cm->angularVelocity != (MeReal *)0x0)) {
                    /* Unresolved local var: MeReal EPS@[???] */
    paMVar1 = tmEnd;
    MeMatrix4TMUpdateFromVelocities
              (paMVar1,0x38d1b717,motionDuration,cm->linearVelocity,cm->angularVelocity,
               (cm->mInstance).mTM);
    unaff_EBX = paMVar1;
  }
  McdGeometryInstanceUpdateAABB(&cm->mInstance,paMVar1,0,unaff_EBX);
  return;
}


/* ==== McdModelUpdatePathCompatible ==== */

/* WARNING: Unknown calling convention */

void McdModelUpdatePathCompatible(McdModelID_conflict cm,MeReal param_2)

{
  McdModelID_conflict in_EAX;
  
  McdModelUpdate(in_EAX);
  return;
}


/* ==== McdModelGetAABB ==== */

void McdModelGetAABB(McdModelID_conflict cm,MeReal *minCorner,MeReal *maxCorner)

{
  McdGeometryInstanceGetAABB();
  return;
}


/* ==== McdModelGetGeometryInstance ==== */

/* WARNING: Unknown calling convention */

McdGeometryInstanceID McdModelGetGeometryInstance(McdModelID_conflict cm)

{
  return &cm->mInstance;
}


/* ==== McdModelGetTransformPtr ==== */

/* WARNING: Unknown calling convention */

MeMatrix4Ptr McdModelGetTransformPtr(McdModelID_conflict cm)

{
  MeMatrix4Ptr paMVar1;
  
  paMVar1 = (MeMatrix4Ptr)McdGeometryInstanceGetTransformPtr();
  return paMVar1;
}


/* ==== McdModelSetTransformPtr ==== */

/* WARNING: Unknown calling convention */

void McdModelSetTransformPtr(McdModelID_conflict cm,MeMatrix4Ptr geometryTM)

{
  McdGeometryInstanceSetTransformPtr();
  return;
}


/* ==== McdModelSetLinearVelocityPtr ==== */

void McdModelSetLinearVelocityPtr(McdModelID_conflict cm,MeReal *linVelocity)

{
  cm->linearVelocity = linVelocity;
  return;
}


/* ==== McdModelGetLinearVelocityPtr ==== */

MeReal * McdModelGetLinearVelocityPtr(McdModelID_conflict cm)

{
  return cm->linearVelocity;
}


/* ==== McdModelSetAngularVelocityPtr ==== */

void McdModelSetAngularVelocityPtr(McdModelID_conflict cm,MeReal *angVelocity)

{
  cm->angularVelocity = angVelocity;
  return;
}


/* ==== McdModelGetAngularVelocityPtr ==== */

MeReal * McdModelGetAngularVelocityPtr(McdModelID_conflict cm)

{
  return cm->angularVelocity;
}


/* ==== McdModelSetUserData ==== */

void McdModelSetUserData(McdModelID_conflict cm,void *data)

{
  cm->mData = data;
  return;
}


/* ==== McdModelGetUserData ==== */

void * McdModelGetUserData(McdModelID_conflict cm)

{
  return cm->mData;
}


/* ==== McdModelSetGeometry ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm1 McdModelSetGeometry(McdModelID_conflict cm,McdGeometryID g)

{
  char *in_stack_00000008;
  
  if ((*in_stack_00000008 == '\0') && (g->frame != (McdFrameworkID)0x0)) {
    MeFatalError(_MeFatalErrorLevel,
                 "Please Remove your model from the space before changinge geometry to Null",cm,cm);
  }
  McdGeometryInstanceSetGeometry();
  return;
}


/* ==== McdModelGetGeometry ==== */

/* WARNING: Unknown calling convention */

McdGeometryID McdModelGetGeometry(McdModelID_conflict cm)

{
  McdGeometryID pMVar1;
  
  pMVar1 = (McdGeometryID)McdGeometryInstanceGetGeometry();
  return pMVar1;
}


/* ==== McdModelGetBodyData ==== */

void * McdModelGetBodyData(McdModelID_conflict cm)

{
  return cm->mBody;
}


/* ==== McdModelSetBodyData ==== */

void McdModelSetBodyData(McdModelID_conflict cm,void *body)

{
  cm->mBody = body;
  return;
}


/* ==== McdModelSetRelativeTransformPtrs ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm1
McdModelSetRelativeTransformPtrs
          (McdModelID_conflict cm,MeVector4 *relativeTM,MeVector4 *referenceTM,MeVector4 *compoundTM
          ,MeBool own)

{
  undefined4 uVar1;
  int in_stack_00000014;
  MeBool own_local;
  MeVector4 *compoundTM_local;
  MeVector4 *referenceTM_local;
  
  if (((uint)relativeTM[3][3] & 1) != 0) {
    (*_MeMatrix4TMCompound)(relativeTM[3][0]);
    uVar1 = McdGeometryInstanceGetTransformPtr(relativeTM + 4);
    cm = (McdModelID_conflict)(*_MeMatrix4TMCompound)(uVar1);
    relativeTM[3][3] = (MeReal)((uint)relativeTM[3][3] & 0xfffffffe);
  }
  relativeTM[3][0] = (MeReal)referenceTM;
  relativeTM[3][1] = (MeReal)compoundTM;
  if (referenceTM == (MeVector4 *)0x0) {
    McdGeometryInstanceSetTransformPtr();
    return;
  }
  McdGeometryInstanceSetTransformPtr(relativeTM + 4,own,cm,cm);
  if (in_stack_00000014 == 0) {
    relativeTM[3][3] = (MeReal)((uint)relativeTM[3][3] & 0xfffffffe);
  }
  else {
    relativeTM[3][3] = (MeReal)((uint)relativeTM[3][3] | 1);
  }
  return;
}


/* ==== McdModelGetRelativeTransformPtrs ==== */

void McdModelGetRelativeTransformPtrs
               (McdModelID_conflict cm,MeMatrix4Ptr *relativeTM,MeMatrix4Ptr *referenceTM)

{
  *relativeTM = cm->mRelTM;
  *referenceTM = cm->mRefTM;
  return;
}


/* ==== McdModelSetRelativeTransform ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3
McdModelSetRelativeTransform(McdModelID_conflict cm,MeMatrix4Ptr relTM,MeMatrix4Ptr refTM)

{
  MeBool own;
  MeVector4 *in_stack_00000004;
  undefined4 *in_stack_00000008;
  MeVector4 *in_stack_0000000c;
  McdModelID_conflict referenceTM;
  
  if (in_stack_00000008 == (undefined4 *)0x0) {
    own = 0;
    referenceTM = (McdModelID_conflict)0x0;
  }
  else {
                    /* Unresolved local var: MeMatrix4Ptr compound@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeMatrix4Ptr relative@[DW_OP_reg0(EAX)] */
    own = (*_McdGeometryInstanceGetBSphere)(0x40,0x10,refTM,refTM);
    cm = (McdModelID_conflict)(*_McdGeometryInstanceGetBSphere)(0x40,0x10);
    cm->frame = (McdFramework *)*in_stack_00000008;
    cm->prev = (McdModelID)in_stack_00000008[1];
    cm->next = (McdModelID)in_stack_00000008[2];
    cm->mSpace = (McdSpaceID)in_stack_00000008[3];
    cm->linearVelocity = (MeReal *)in_stack_00000008[4];
    cm->angularVelocity = (MeReal *)in_stack_00000008[5];
    cm->mPadding = (MeReal)in_stack_00000008[6];
    cm->mSpaceID = in_stack_00000008[7];
    *(undefined4 *)&cm->sortKey = in_stack_00000008[8];
    cm->mBody = (void *)in_stack_00000008[9];
    cm->mRequestID = in_stack_00000008[10];
    cm->mTransformUpdateFn = (McdModelUpdateFnPtr)in_stack_00000008[0xb];
    cm->mRelTM = (MeMatrix4Ptr)in_stack_00000008[0xc];
    cm->mRefTM = (MeMatrix4Ptr)in_stack_00000008[0xd];
    cm->mIntersectFn = (McdModelIntersectFnPtr)in_stack_00000008[0xe];
    cm->flags = in_stack_00000008[0xf];
    referenceTM = cm;
  }
  McdModelSetRelativeTransformPtrs
            (cm,in_stack_00000004,(MeVector4 *)referenceTM,in_stack_0000000c,own);
  return;
}


/* ==== McdModelGetRelativeTransform ==== */

MeMatrix4Ptr McdModelGetRelativeTransform(McdModelID_conflict cm)

{
  return cm->mRelTM;
}


/* ==== McdModelCompoundTransforms ==== */

void McdModelCompoundTransforms(McdModelID_conflict cm)

{
  MeMatrix4TMCompound((cm->mInstance).mTM,cm->mRelTM,cm->mRefTM);
  return;
}


/* ==== McdModelSetMaterial ==== */

/* WARNING: Unknown calling convention */

void McdModelSetMaterial(McdModelID_conflict cm,int material)

{
  McdGeometryInstanceSetMaterial();
  return;
}


/* ==== McdModelGetMaterial ==== */

/* WARNING: Unknown calling convention */

uint McdModelGetMaterial(McdModelID_conflict cm)

{
  uint uVar1;
  
  uVar1 = McdGeometryInstanceGetMaterial();
  return uVar1;
}


/* ==== McdModelSetRequestID ==== */

void McdModelSetRequestID(McdModelID_conflict cm,int requestId)

{
  cm->mRequestID = requestId;
  return;
}


/* ==== McdModelGetRequestID ==== */

int McdModelGetRequestID(McdModelID_conflict cm)

{
  return cm->mRequestID;
}


/* ==== McdModelSetUpdateCallback ==== */

void McdModelSetUpdateCallback(McdModelID_conflict cm,McdModelUpdateFnPtr f)

{
  cm->mTransformUpdateFn = f;
  return;
}


/* ==== McdModelGetUpdateCallback ==== */

McdModelUpdateFnPtr McdModelGetUpdateCallback(McdModelID_conflict cm)

{
  return cm->mTransformUpdateFn;
}


/* ==== McdModelSetIntersectCallback ==== */

void McdModelSetIntersectCallback(McdModelID_conflict cm,McdModelIntersectFnPtr fn)

{
  cm->mIntersectFn = fn;
  return;
}


/* ==== McdModelGetIntersectCallback ==== */

McdModelIntersectFnPtr McdModelGetIntersectCallback(McdModelID_conflict cm)

{
  return cm->mIntersectFn;
}


/* ==== McdModelGetSortKey ==== */

MeI16 McdModelGetSortKey(McdModelID_conflict cm)

{
  return cm->sortKey;
}


/* ==== McdModelSetSortKey ==== */

void McdModelSetSortKey(McdModelID_conflict cm,MeI16 key)

{
  cm->sortKey = key;
  return;
}


