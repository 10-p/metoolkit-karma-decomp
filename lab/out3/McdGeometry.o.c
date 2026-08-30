/* ==== McdGeometryIsValid ==== */

MeBool McdGeometryIsValid(McdGeometryID g)

{
  MeBool MVar1;
  
  MVar1 = 0;
  if (g != (McdGeometryID)0x0) {
    MVar1 = McdFrameworkTypeIsValid(g->frame,(uint)(byte)g->mRefCtAndID);
  }
  return MVar1;
}


/* ==== McdGeometryInit ==== */

void McdGeometryInit(McdGeometry *g,McdFramework *frame,MeI16 typeId)

{
  McdGeometryID pMVar1;
  
  g->mRefCtAndID = (int)typeId;
  g->frame = frame;
  if (frame->firstGeometry == (McdGeometryID)0x0) {
    frame->firstGeometry = g;
    g->prev = g;
    g->next = g;
  }
  else {
    pMVar1 = frame->firstGeometry->prev;
    g->prev = pMVar1;
    g->next = frame->firstGeometry;
    pMVar1->next = g;
    g->next->prev = g;
  }
  frame->geometryCount = frame->geometryCount + 1;
  return;
}


/* ==== McdGeometryDeinit ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdGeometryDeinit(McdGeometry *g)

{
  McdFrameworkID pMVar1;
  McdGeometryID pMVar2;
  
                    /* Unresolved local var: McdFramework * frame@[DW_OP_reg3(EBX)] */
  pMVar1 = g->frame;
  if (pMVar1->firstGeometry == g) {
    if (g->prev == g) {
      pMVar2 = (McdGeometryID)0x0;
    }
    else {
      pMVar2 = g->next;
    }
    pMVar1->firstGeometry = pMVar2;
  }
  pMVar2 = g->next;
  pMVar2->prev = g->prev;
  g->prev->next = pMVar2;
  (*___gxx_personality_v0)(g);
  pMVar1->geometryCount = pMVar1->geometryCount + -1;
  return;
}


/* ==== McdGeometryGetTypeName ==== */

char * McdGeometryGetTypeName(McdGeometryID g)

{
  char *pcVar1;
  
  pcVar1 = McdFrameworkGetTypeName(g->frame,(uint)(byte)g->mRefCtAndID);
  return pcVar1;
}


/* ==== McdGeometryGetReferenceCount ==== */

int McdGeometryGetReferenceCount(McdGeometryID g)

{
  return g->mRefCtAndID >> 8;
}


/* ==== McdGeometryIncrementReferenceCount ==== */

void McdGeometryIncrementReferenceCount(McdGeometryID g)

{
  g->mRefCtAndID = g->mRefCtAndID + 0x100;
  return;
}


/* ==== McdGeometryDecrementReferenceCount ==== */

void McdGeometryDecrementReferenceCount(McdGeometryID g)

{
  g->mRefCtAndID = g->mRefCtAndID - 0x100;
  return;
}


/* ==== McdGeometrySetReferenceCount ==== */

void McdGeometrySetReferenceCount(McdGeometryID g,int refCount)

{
  g->mRefCtAndID = 0;
  return;
}


/* ==== McdGeometryDestroy ==== */

void McdGeometryDestroy(McdGeometryID g)

{
  uint uVar1;
  
                    /* Unresolved local var: McdGeometryDestroyFnPtr fn@[???] */
  uVar1 = g->mRefCtAndID;
  if (uVar1 >> 8 != 0) {
    MeWarning(0,"Destroying geometry with non-zero reference count!");
    uVar1 = g->mRefCtAndID;
  }
  (*g->frame->geometryVTableTable[uVar1 & 0xff].destroy)(g);
  return;
}


/* ==== McdGeometryGetFramework ==== */

McdFrameworkID McdGeometryGetFramework(McdGeometryID g)

{
  return g->frame;
}


/* ==== McdGeometryGetLineSegIntersectFnPtr ==== */

McdLineSegIntersectFnPtr McdGeometryGetLineSegIntersectFnPtr(McdGeometryID g)

{
  return g->frame->geometryVTableTable[(byte)g->mRefCtAndID].lineSegIntersect;
}


/* ==== McdGeometryGetAABB ==== */

void McdGeometryGetAABB(McdGeometryID g,MeVector4 *tm,MeReal *minCorner,MeReal *maxCorner)

{
  McdGeometryInstance ins;
  
  ins.mTM = tm;
  ins.mGeometry = g;
  McdGeometryInstanceGetAABB(&ins,minCorner,maxCorner);
  return;
}


/* ==== McdGeometryGetBSphere ==== */

void McdGeometryGetBSphere(McdGeometryID g,MeReal *center,MeReal *radius)

{
                    /* Unresolved local var: McdGeometryGetBSphereFnPtr fn@[???] */
  (*g->frame->geometryVTableTable[(byte)g->mRefCtAndID].getBSphere)(g,center,radius);
  return;
}


/* ==== McdGeometryMaximumPoint ==== */

void McdGeometryMaximumPoint(McdGeometryID g,MeVector4 *tm,MeReal *inDir,MeReal *outPoint)

{
  McdGeometryInstance ins;
  
                    /* Unresolved local var: McdGeometryMaximumPointFnPtr fn@[DW_OP_reg1(ECX)] */
  ins.mTM = tm;
  ins.mGeometry = g;
  (*g->frame->geometryVTableTable[(byte)g->mRefCtAndID].maximumPoint)(&ins,inDir,outPoint);
  return;
}


/* ==== McdGeometryGetMassProperties ==== */

MeI16 __regparm1
McdGeometryGetMassProperties(McdGeometryID g,MeVector4 *relTM,MeVector3 *m,MeReal *volume)

{
  MeI16 MVar1;
  
                    /* Unresolved local var: McdGeometryGetMassPropertiesFnPtr fn@[???] */
  MVar1 = (**(code **)(*(int *)((int)(*relTM)[3] + 0x18) + 0x10 + (uint)*(byte *)*relTM * 0x28))
                    (relTM,m,volume);
  return MVar1;
}


/* ==== McdGeometryDebugDraw ==== */

void McdGeometryDebugDraw(McdGeometryID g,MeMatrix4Ptr tm,MeReal *colour)

{
                    /* Unresolved local var: McdGeometryDebugDrawFnPtr fn@[???] */
  (*g->frame->geometryVTableTable[(byte)g->mRefCtAndID].debugDraw)(g,tm,colour);
  return;
}


