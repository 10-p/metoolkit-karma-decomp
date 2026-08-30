/* ==== McdNullGetTypeId ==== */

MeI16 McdNullGetTypeId(void)

{
  return 0;
}


/* ==== McdNullRegisterType ==== */

void McdNullRegisterType(McdFramework *frame)

{
  McdFrameworkRegisterGeometryType
            (frame,0,&DAT_00010158,McdNullDestroy,McdNullUpdateAABB,McdNullGetBSphere,
             McdNullMaximumPoint,McdNullGetMassProperties,McdNullDebugDraw);
  return;
}


/* ==== McdNullCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

McdNullID McdNullCreate(McdFramework *frame)

{
  McdNullID pMVar1;
  McdNullID pMVar2;
  
                    /* Unresolved local var: McdNull * n@[DW_OP_reg3(EBX)] */
  pMVar1 = (*_McdGeometryDeinit)(0x20,0x10);
  pMVar2 = (McdNullID)0x0;
  if (pMVar1 != (McdNullID)0x0) {
    McdGeometryInit(pMVar1,frame,0);
    pMVar2 = pMVar1;
  }
  return pMVar2;
}


/* ==== McdNullDestroy ==== */

void McdNullDestroy(McdGeometry *g)

{
  McdGeometryDeinit(g);
  return;
}


/* ==== McdNullUpdateAABB ==== */

void McdNullUpdateAABB(McdGeometryInstanceID ins,MeMatrix4Ptr finalTM,MeBool tight)

{
  MeFatalError(0,
               "Attempt to calculate the bounding box of a null geometry (perhaps you inserted it into an McdSpace?)"
              );
  return;
}


/* ==== McdNullGetBSphere ==== */

void McdNullGetBSphere(McdGeometryID g,MeReal *center,MeReal *radius)

{
  MeFatalError(0,"Attempt to calculate the bounding sphere of a null geometry");
  return;
}


/* ==== McdNullMaximumPoint ==== */

void McdNullMaximumPoint(McdGeometryInstanceID ins,MeReal *inDir,MeReal *outPoint)

{
  MeFatalError(0,"Attempt to calculate a maximum point on a null geometry");
  return;
}


/* ==== McdNullGetMassProperties ==== */

MeI16 McdNullGetMassProperties(McdGeometry *g,MeVector4 *relTM,MeVector3 *m,MeReal *volume)

{
  MeFatalError(0,"Attempt to calculate mass properties of a null geometry");
  return 0;
}


/* ==== McdNullDebugDraw ==== */

void McdNullDebugDraw(McdGeometryID geom,MeReal (*tm) [4],MeReal *colour)

{
  if (McdNullDebugDraw::warn != 0) {
    MeWarning(0,"McdNullDebugDraw: Don\'t know how to draw a Null!");
  }
  McdNullDebugDraw::warn = 0;
  return;
}


