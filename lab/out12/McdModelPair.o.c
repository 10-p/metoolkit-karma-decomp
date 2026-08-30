/* ==== McdModelPairCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdModelPairID McdModelPairCreate(McdModelID_conflict m1,McdModelID_conflict m2)

{
  McdModelPairID pMVar1;
  McdRequest *pMVar2;
  
                    /* Unresolved local var: McdModelPair * p@[DW_OP_reg3(EBX)] */
  pMVar1 = (*_MeMemoryAPI)(0x1c);
  pMVar1->model1 = m1;
  pMVar1->model2 = m2;
  pMVar2 = McdFrameworkGetDefaultRequestPtr(m1->frame);
  pMVar1->userData = (void *)0x0;
  pMVar1->request = pMVar2;
  pMVar1->responseData = (void *)0x0;
  return pMVar1;
}


/* ==== McdModelPairReset ==== */

void McdModelPairReset(McdModelPair *p,McdModelID_conflict model1,McdModelID_conflict model2)

{
  McdRequest *pMVar1;
  
  p->model1 = model1;
  p->model2 = model2;
  pMVar1 = McdFrameworkGetDefaultRequestPtr(model1->frame);
  p->userData = (void *)0x0;
  p->request = pMVar1;
  p->responseData = (void *)0x0;
  return;
}


/* ==== McdModelPairDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdModelPairDestroy(McdModelPairID p)

{
  (*_DAT_0001100c)(p);
  return;
}


/* ==== McdModelPairGetModels ==== */

void McdModelPairGetModels(McdModelPairID p,McdModelID_conflict *m1,McdModelID_conflict *m2)

{
  *m1 = p->model1;
  *m2 = p->model2;
  return;
}


/* ==== McdModelPairSetRequestPtr ==== */

void McdModelPairSetRequestPtr(McdModelPairID p,McdRequest *r)

{
  p->request = r;
  return;
}


/* ==== McdModelPairGetRequestPtr ==== */

McdRequest * McdModelPairGetRequestPtr(McdModelPairID p)

{
  return p->request;
}


/* ==== McdModelPairGetUserData ==== */

void * McdModelPairGetUserData(McdModelPairID p)

{
  return p->userData;
}


/* ==== McdModelPairSetUserData ==== */

void McdModelPairSetUserData(McdModelPairID p,void *d)

{
  p->userData = d;
  return;
}


