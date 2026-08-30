/* ==== McdNullRegisterType ==== */

/* WARNING: Unknown calling convention */

void McdNullRegisterType(McdFramework *frame)

{
  McdFrameworkRegisterGeometryType
            (frame,0,&DAT_00010158,McdNullDestroy,McdNullUpdateAABB,McdNullGetBSphere,
             McdNullMaximumPoint,McdNullGetMassProperties,McdNullDebugDraw);
  return;
}


/* ==== McdNullCreate ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

McdNullID McdNullCreate(McdFramework *frame)

{
  McdNullID pMVar1;
  McdNullID pMVar2;
  undefined4 extraout_ECX;
  
                    /* Unresolved local var: McdNull * n@[DW_OP_reg3(EBX)] */
  pMVar1 = (McdNullID)(*_McdGeometryDeinit)(0x20,0x10);
  pMVar2 = (McdNullID)0x0;
  if (pMVar1 != (McdNullID)0x0) {
    McdGeometryInit(pMVar1,frame,0,extraout_ECX);
    pMVar2 = pMVar1;
  }
  return pMVar2;
}


/* ==== McdNullDestroy ==== */

/* WARNING: Unknown calling convention */

void McdNullDestroy(McdGeometry *g)

{
  McdGeometryDeinit();
  return;
}


/* ==== McdNullUpdateAABB ==== */

/* WARNING: Unknown calling convention */

void McdNullUpdateAABB(McdGeometryInstanceID ins,MeMatrix4Ptr finalTM,MeBool tight)

{
  MeFatalError();
  return;
}


/* ==== McdNullGetBSphere ==== */

/* WARNING: Unknown calling convention */

void McdNullGetBSphere(McdGeometryID g,MeReal *center,MeReal *radius)

{
  MeFatalError();
  return;
}


/* ==== McdNullMaximumPoint ==== */

/* WARNING: Unknown calling convention */

void McdNullMaximumPoint(McdGeometryInstanceID ins,MeReal *inDir,MeReal *outPoint)

{
  MeFatalError();
  return;
}


/* ==== McdNullGetMassProperties ==== */

/* WARNING: Unknown calling convention */

MeI16 McdNullGetMassProperties(McdGeometry *g,MeVector4 *relTM,MeVector3 *m,MeReal *volume)

{
  MeFatalError(0,"Attempt to calculate mass properties of a null geometry");
  return 0;
}


/* ==== McdNullDebugDraw ==== */

/* WARNING: Unknown calling convention */

void McdNullDebugDraw(McdGeometryID geom,MeReal (*tm) [4],MeReal *colour)

{
  if (McdNullDebugDraw::warn != 0) {
    MeWarning(0,"McdNullDebugDraw: Don\'t know how to draw a Null!");
  }
  McdNullDebugDraw::warn = 0;
  return;
}


