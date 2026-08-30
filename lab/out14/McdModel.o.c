/* ==== McdModelCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdModelID_conflict McdModelCreate(McdGeometryID g)

{
  McdModelID pMVar1;
  McdFramework *pMVar2;
  McdModelID_conflict pMVar3;
  
                    /* Unresolved local var: McdFramework * frame@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdModel.conflict * obj@[DW_OP_reg6(ESI)] */
  pMVar2 = McdGeometryGetFramework(g);
  pMVar3 = (*_MeMemoryAPI)(&pMVar2->modelPool);
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

void McdModelDestroy(McdModelID_conflict cm)

{
  McdFramework *pMVar1;
  McdModelID pMVar2;
  void *pvVar3;
  McdFramework *pMVar4;
  
                    /* Unresolved local var: McdFramework * frame@[DW_OP_reg6(ESI)] */
  if ((cm->flags & 1U) != 0) {
    (*_MeMatrix4TMCompound)(cm->mRelTM);
    pvVar3 = McdGeometryInstanceGetTransformPtr(&cm->mInstance);
    (*_MeMatrix4TMCompound)(pvVar3);
    cm->flags = cm->flags & 0xfffffffe;
  }
  McdGeometryInstanceSetGeometry(&cm->mInstance,(void *)0x0);
  pMVar1 = cm->frame;
  pMVar4 = pMVar1;
  if (pMVar1->firstModel == cm) {
    pMVar1->firstModel = (McdModelID_conflict)((uint)cm->next & (cm->next == cm) - 1);
    pMVar4 = cm->frame;
  }
  pMVar2 = cm->next;
  pMVar2->prev = cm->prev;
  cm->prev->next = pMVar2;
  (*_McdGeometryInstanceGetTransformPtr)(&pMVar4->modelPool,cm);
  pMVar1->modelCount = pMVar1->modelCount + -1;
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
  McdGeometryInstanceReset(&obj->mInstance);
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
  McdGeometryInstanceGetBSphere(&m->mInstance,center,radius);
  return;
}


/* ==== McdModelUpdate ==== */

void McdModelUpdate(McdModelID_conflict cm)

{
  float fVar1;
  float fVar2;
  
  if (cm->mTransformUpdateFn != (McdModelUpdateFnPtr)0x0) {
    (*cm->mTransformUpdateFn)(cm);
  }
  if (cm->mRelTM != (MeMatrix4Ptr)0x0) {
    MeMatrix4TMCompound((cm->mInstance).mTM,cm->mRelTM,cm->mRefTM);
  }
  McdGeometryInstanceUpdateAABB(&cm->mInstance,(void *)0x0,0);
  fVar1 = cm->mPadding;
  (cm->mInstance).min[0] = (cm->mInstance).min[0] - fVar1;
  (cm->mInstance).min[1] = (cm->mInstance).min[1] - fVar1;
  (cm->mInstance).min[2] = (cm->mInstance).min[2] - fVar1;
  (cm->mInstance).max[0] = (cm->mInstance).max[0] + fVar1;
  fVar2 = (cm->mInstance).max[2];
  (cm->mInstance).max[1] = (cm->mInstance).max[1] + fVar1;
  (cm->mInstance).max[2] = fVar1 + fVar2;
  return;
}


/* ==== McdModelUpdatePath ==== */

void McdModelUpdatePath(McdModelID_conflict cm,MeReal motionDuration)

{
  MeReal (*paMVar1) [4];
  MeReal tmEnd [4] [4];
  
                    /* Unresolved local var: MeMatrix4Ptr tmEndPtr@[DW_OP_reg1(ECX)] */
  if (cm->mTransformUpdateFn != (McdModelUpdateFnPtr)0x0) {
    (*cm->mTransformUpdateFn)(cm);
  }
  if (cm->mRelTM != (MeMatrix4Ptr)0x0) {
    MeMatrix4TMCompound((cm->mInstance).mTM,cm->mRelTM,cm->mRefTM);
  }
  paMVar1 = (MeReal (*) [4])0x0;
  if ((cm->linearVelocity != (MeReal *)0x0) && (cm->angularVelocity != (MeReal *)0x0)) {
                    /* Unresolved local var: MeReal EPS@[???] */
    paMVar1 = tmEnd;
    MeMatrix4TMUpdateFromVelocities
              (paMVar1,0.0001,motionDuration,cm->linearVelocity,cm->angularVelocity,
               (cm->mInstance).mTM);
  }
  McdGeometryInstanceUpdateAABB(&cm->mInstance,paMVar1,0);
  return;
}


/* ==== McdModelUpdatePathCompatible ==== */

void McdModelUpdatePathCompatible(McdModelID_conflict cm,MeReal param_2)

{
  McdModelUpdate(cm);
  return;
}


/* ==== McdModelGetAABB ==== */

void McdModelGetAABB(McdModelID_conflict cm,MeReal *minCorner,MeReal *maxCorner)

{
  McdGeometryInstanceGetAABB(&cm->mInstance,minCorner,maxCorner);
  return;
}


/* ==== McdModelGetGeometryInstance ==== */

McdGeometryInstanceID McdModelGetGeometryInstance(McdModelID_conflict cm)

{
  return &cm->mInstance;
}


/* ==== McdModelGetTransformPtr ==== */

MeMatrix4Ptr McdModelGetTransformPtr(McdModelID_conflict cm)

{
  MeMatrix4Ptr paMVar1;
  
  paMVar1 = McdGeometryInstanceGetTransformPtr(&cm->mInstance);
  return paMVar1;
}


/* ==== McdModelSetTransformPtr ==== */

void McdModelSetTransformPtr(McdModelID_conflict cm,MeMatrix4Ptr geometryTM)

{
  McdGeometryInstanceSetTransformPtr(&cm->mInstance,geometryTM);
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

void McdModelSetGeometry(McdModelID_conflict cm,McdGeometryID g)

{
  if (((char)g->mRefCtAndID == '\0') && (cm->mSpace != (McdSpaceID)0x0)) {
    MeFatalError(_MeFatalErrorLevel,
                 "Please Remove your model from the space before changinge geometry to Null");
  }
  McdGeometryInstanceSetGeometry(&cm->mInstance,g);
  return;
}


/* ==== McdModelGetGeometry ==== */

McdGeometryID McdModelGetGeometry(McdModelID_conflict cm)

{
  McdGeometryID pMVar1;
  
  pMVar1 = McdGeometryInstanceGetGeometry(&cm->mInstance);
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

void McdModelSetRelativeTransformPtrs
               (McdModelID_conflict cm,MeVector4 *relativeTM,MeVector4 *referenceTM,
               MeVector4 *compoundTM,MeBool own)

{
  void *pvVar1;
  MeBool own_local;
  MeVector4 *compoundTM_local;
  MeVector4 *referenceTM_local;
  
  if ((cm->flags & 1U) != 0) {
    (*_MeMatrix4TMCompound)(cm->mRelTM);
    pvVar1 = McdGeometryInstanceGetTransformPtr(&cm->mInstance);
    (*_MeMatrix4TMCompound)(pvVar1);
    cm->flags = cm->flags & 0xfffffffe;
  }
  cm->mRelTM = relativeTM;
  cm->mRefTM = referenceTM;
  if (relativeTM == (MeVector4 *)0x0) {
    McdGeometryInstanceSetTransformPtr(&cm->mInstance,referenceTM);
    return;
  }
  McdGeometryInstanceSetTransformPtr(&cm->mInstance,compoundTM);
  if (own == 0) {
    cm->flags = cm->flags & 0xfffffffe;
  }
  else {
    cm->flags = cm->flags | 1;
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

void McdModelSetRelativeTransform(McdModelID_conflict cm,MeMatrix4Ptr relTM,MeMatrix4Ptr refTM)

{
  MeVector4 *compoundTM;
  MeVector4 *relativeTM;
  
  if (relTM == (MeMatrix4Ptr)0x0) {
    compoundTM = (MeVector4 *)0x0;
    relativeTM = (MeVector4 *)0x0;
  }
  else {
                    /* Unresolved local var: MeMatrix4Ptr compound@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeMatrix4Ptr relative@[DW_OP_reg0(EAX)] */
    compoundTM = (*_McdGeometryInstanceGetBSphere)(0x40,0x10);
    relativeTM = (*_McdGeometryInstanceGetBSphere)(0x40,0x10);
    (*relativeTM)[0] = (*relTM)[0];
    (*relativeTM)[1] = (*relTM)[1];
    (*relativeTM)[2] = (*relTM)[2];
    (*relativeTM)[3] = (*relTM)[3];
    relativeTM[1][0] = relTM[1][0];
    relativeTM[1][1] = relTM[1][1];
    relativeTM[1][2] = relTM[1][2];
    relativeTM[1][3] = relTM[1][3];
    relativeTM[2][0] = relTM[2][0];
    relativeTM[2][1] = relTM[2][1];
    relativeTM[2][2] = relTM[2][2];
    relativeTM[2][3] = relTM[2][3];
    relativeTM[3][0] = relTM[3][0];
    relativeTM[3][1] = relTM[3][1];
    relativeTM[3][2] = relTM[3][2];
    relativeTM[3][3] = relTM[3][3];
  }
  McdModelSetRelativeTransformPtrs
            (cm,relativeTM,refTM,compoundTM,(uint)(relTM != (MeMatrix4Ptr)0x0));
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

void McdModelSetMaterial(McdModelID_conflict cm,int material)

{
  McdGeometryInstanceSetMaterial(&cm->mInstance,material);
  return;
}


/* ==== McdModelGetMaterial ==== */

uint McdModelGetMaterial(McdModelID_conflict cm)

{
  uint uVar1;
  
  uVar1 = McdGeometryInstanceGetMaterial(&cm->mInstance);
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


