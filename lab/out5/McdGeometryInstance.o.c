/* ==== McdGeometryInstanceReset ==== */

void McdGeometryInstanceReset(McdGeometryInstanceID ins)

{
  ins->mGeometry = (McdGeometry *)0x0;
  ins->mMaterial = 0;
  ins->mTM = (MeMatrix4Ptr)0x0;
  ins->min[0] = 3.4028235e+38;
  ins->min[1] = 3.4028235e+38;
  ins->min[2] = 3.4028235e+38;
  ins->max[0] = -3.4028235e+38;
  ins->max[1] = -3.4028235e+38;
  ins->max[2] = -3.4028235e+38;
  ins->next = (_McdGeometryInstance *)0x0;
  ins->prev = (_McdGeometryInstance *)0x0;
  ins->parent = (_McdGeometryInstance *)0x0;
  ins->child = (_McdGeometryInstance *)0x0;
  return;
}


/* ==== McdGeometryInstanceGetBSphere ==== */

void McdGeometryInstanceGetBSphere(McdGeometryInstanceID ins,MeReal *center,MeReal *radius)

{
  McdGeometry *pMVar1;
  MeMatrix4Ptr paMVar2;
  MeReal c [3];
  
                    /* Unresolved local var: McdGeometryID g@[DW_OP_reg1(ECX)]
                       Unresolved local var: McdGeometryGetBSphereFnPtr fn@[???] */
  pMVar1 = ins->mGeometry;
  (*pMVar1->frame->geometryVTableTable[(byte)pMVar1->mRefCtAndID].getBSphere)(pMVar1,c,radius);
  paMVar2 = ins->mTM;
  *center = c[2] * paMVar2[2][0] + c[1] * paMVar2[1][0] + c[0] * (*paMVar2)[0] + paMVar2[3][0];
  center[1] = c[2] * paMVar2[2][1] + c[1] * paMVar2[1][1] + c[0] * (*paMVar2)[1] + paMVar2[3][1];
  center[2] = c[2] * paMVar2[2][2] + c[0] * (*paMVar2)[2] + c[1] * paMVar2[1][2] + paMVar2[3][2];
  return;
}


/* ==== McdGeometryInstanceSetGeometry ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

void McdGeometryInstanceSetGeometry(McdGeometryInstanceID ins,McdGeometryID geom)

{
  McdGeometryInstanceID p_Var1;
  _McdGeometryInstance *p_Var2;
  uint uVar3;
  void *pvVar4;
  _McdGeometryInstance *p_Var5;
  uint uVar6;
  _McdGeometryInstance *p_Var7;
  McdGeometryID pMVar8;
  McdFrameworkID frame;
  undefined4 uVar9;
  undefined4 extraout_EAX;
  int iVar10;
  int iVar11;
  int max;
  McdGeometryInstanceID nextChild;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdFramework * frame@[DW_OP_reg0(EAX)]
                       Unresolved local var: McdGeometryInstanceID child@[DW_OP_reg3(EBX)]
                       Unresolved local var: McdGeometryInstanceID next@[???] */
  if (ins->child != (_McdGeometryInstance *)0x0) {
                    /* Unresolved local var: McdFrameworkID frame@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdGeometryInstanceID child@[DW_OP_reg6(ESI)] */
    frame = McdGeometryGetFramework(ins->mGeometry);
    if (ins->child != (_McdGeometryInstance *)0x0) {
      p_Var5 = ins->child;
      do {
        p_Var7 = p_Var5->next;
        p_Var1 = p_Var5->child;
        while (p_Var1 != (McdGeometryInstanceID)0x0) {
                    /* Unresolved local var: McdGeometryInstanceID child@[DW_OP_reg0(EAX)]
                       Unresolved local var: McdGeometryInstanceID nextChild@[DW_OP_reg3(EBX)] */
          p_Var2 = p_Var1->next;
          McdGeometryInstanceDestroy(frame,p_Var1);
          p_Var1 = p_Var2;
        }
        uVar9 = 0;
        if (p_Var5->mGeometry != (McdGeometry *)0x0) {
          McdGeometryDecrementReferenceCount(p_Var5->mGeometry);
          uVar9 = extraout_EAX;
        }
        (*_DAT_0001101c)(&frame->instancePool,p_Var5,uVar9,uVar9);
        p_Var5 = p_Var7;
      } while (p_Var7 != (_McdGeometryInstance *)0x0);
    }
    ins->child = (_McdGeometryInstance *)0x0;
  }
  if (ins->mGeometry != (McdGeometry *)0x0) {
    McdGeometryDecrementReferenceCount(ins->mGeometry);
  }
  ins->mGeometry = geom;
  if (geom == (McdGeometryID)0x0) {
    return;
  }
  McdGeometryIncrementReferenceCount(geom);
  if ((char)geom->mRefCtAndID != '\b') {
    return;
  }
  uVar3 = McdAggregateGetElementCountMax(geom);
  if (uVar3 == 0) {
    return;
  }
  pvVar4 = McdGeometryGetFramework(geom);
                    /* Unresolved local var: McdGeometryInstance * ins@[DW_OP_reg0(EAX)] */
  iVar10 = (int)pvVar4 + 0x60;
  p_Var5 = (_McdGeometryInstance *)(*___gxx_personality_v0)(iVar10);
  p_Var5->min[0] = 3.4028235e+38;
  p_Var5->mMaterial = 0;
  p_Var5->mTM = (MeMatrix4Ptr)0x0;
  p_Var5->mGeometry = (McdGeometry *)0x0;
  p_Var5->min[1] = 3.4028235e+38;
  p_Var5->min[2] = 3.4028235e+38;
  p_Var5->max[0] = -3.4028235e+38;
  p_Var5->max[1] = -3.4028235e+38;
  p_Var5->max[2] = -3.4028235e+38;
  p_Var5->parent = (_McdGeometryInstance *)0x0;
  p_Var5->child = (_McdGeometryInstance *)0x0;
  p_Var5->next = (_McdGeometryInstance *)0x0;
  p_Var5->prev = (_McdGeometryInstance *)0x0;
  p_Var5->parent = ins;
  ins->child = p_Var5;
  uVar6 = uVar3 - 1;
  if (0 < (int)uVar6) {
    p_Var7 = p_Var5;
    if ((uVar6 & 1) != 0) {
                    /* Unresolved local var: McdGeometryInstance * ins@[???] */
      p_Var7 = (_McdGeometryInstance *)(*___gxx_personality_v0)(iVar10);
      p_Var7->min[0] = 3.4028235e+38;
      p_Var7->mMaterial = 0;
      p_Var7->mTM = (MeMatrix4Ptr)0x0;
      p_Var7->mGeometry = (McdGeometry *)0x0;
      p_Var7->min[1] = 3.4028235e+38;
      p_Var7->min[2] = 3.4028235e+38;
      p_Var7->max[0] = -3.4028235e+38;
      p_Var7->max[1] = -3.4028235e+38;
      p_Var7->max[2] = -3.4028235e+38;
      p_Var7->next = (_McdGeometryInstance *)0x0;
      p_Var7->child = (_McdGeometryInstance *)0x0;
      p_Var7->prev = p_Var5;
      p_Var5->next = p_Var7;
      p_Var7->parent = ins;
      uVar6 = uVar3 - 2;
      if (uVar6 == 0) goto LAB_00010362;
    }
    do {
      p_Var5 = (_McdGeometryInstance *)(*___gxx_personality_v0)(iVar10);
      p_Var5->mGeometry = (McdGeometry *)0x0;
      p_Var5->min[0] = 3.4028235e+38;
      p_Var5->mMaterial = 0;
      p_Var5->mTM = (MeMatrix4Ptr)0x0;
      p_Var5->min[1] = 3.4028235e+38;
      p_Var5->min[2] = 3.4028235e+38;
      p_Var5->max[0] = -3.4028235e+38;
      p_Var5->max[1] = -3.4028235e+38;
      p_Var5->max[2] = -3.4028235e+38;
      p_Var5->next = (_McdGeometryInstance *)0x0;
      p_Var5->child = (_McdGeometryInstance *)0x0;
      p_Var5->prev = p_Var7;
      p_Var5->parent = ins;
      p_Var7->next = p_Var5;
      p_Var7 = (_McdGeometryInstance *)(*___gxx_personality_v0)(iVar10);
      uVar6 = uVar6 - 2;
      p_Var7->mGeometry = (McdGeometry *)0x0;
      p_Var7->min[0] = 3.4028235e+38;
      p_Var7->mMaterial = 0;
      p_Var7->mTM = (MeMatrix4Ptr)0x0;
      p_Var7->min[1] = 3.4028235e+38;
      p_Var7->min[2] = 3.4028235e+38;
      p_Var7->max[0] = -3.4028235e+38;
      p_Var7->max[1] = -3.4028235e+38;
      p_Var7->max[2] = -3.4028235e+38;
      p_Var7->next = (_McdGeometryInstance *)0x0;
      p_Var7->child = (_McdGeometryInstance *)0x0;
      p_Var7->prev = p_Var5;
      p_Var5->next = p_Var7;
      p_Var7->parent = ins;
    } while (uVar6 != 0);
  }
LAB_00010362:
  iVar10 = 0;
  p_Var5 = ins->child;
  if ((int)uVar3 < 1) {
    return;
  }
  uVar6 = uVar3 & 3;
  if (1 < (int)uVar3) {
    if (uVar6 == 0) goto LAB_000103a8;
    if (1 < uVar6) {
      if (2 < uVar6) {
                    /* Unresolved local var: McdGeometryID g@[???] */
        pMVar8 = McdAggregateGetElementGeometry(geom,0);
        McdGeometryInstanceSetGeometry(p_Var5,pMVar8);
        p_Var5 = p_Var5->next;
      }
      uVar6 = (uint)(2 < uVar6);
      iVar10 = uVar6 + 1;
      pMVar8 = McdAggregateGetElementGeometry(geom,uVar6);
      McdGeometryInstanceSetGeometry(p_Var5,pMVar8);
      p_Var5 = p_Var5->next;
    }
  }
  iVar11 = iVar10 + 1;
  pMVar8 = McdAggregateGetElementGeometry(geom,iVar10);
  McdGeometryInstanceSetGeometry(p_Var5,pMVar8);
  p_Var5 = p_Var5->next;
  iVar10 = iVar11;
  if ((int)uVar3 <= iVar11) {
    return;
  }
LAB_000103a8:
  do {
    pMVar8 = McdAggregateGetElementGeometry(geom,iVar10);
    McdGeometryInstanceSetGeometry(p_Var5,pMVar8);
    p_Var1 = p_Var5->next;
    pMVar8 = McdAggregateGetElementGeometry(geom,iVar10 + 1);
    McdGeometryInstanceSetGeometry(p_Var1,pMVar8);
    p_Var1 = p_Var1->next;
    pMVar8 = McdAggregateGetElementGeometry(geom,iVar10 + 2);
    iVar11 = iVar10 + 3;
    iVar10 = iVar10 + 4;
    McdGeometryInstanceSetGeometry(p_Var1,pMVar8);
    p_Var1 = p_Var1->next;
    pMVar8 = McdAggregateGetElementGeometry(geom,iVar11);
    McdGeometryInstanceSetGeometry(p_Var1,pMVar8);
    p_Var5 = p_Var1->next;
  } while (iVar10 < (int)uVar3);
  return;
}


/* ==== McdGeometryInstanceGetGeometry ==== */

McdGeometryID McdGeometryInstanceGetGeometry(McdGeometryInstanceID ins)

{
  return ins->mGeometry;
}


/* ==== McdGeometryInstanceUpdateAABB ==== */

void McdGeometryInstanceUpdateAABB(McdGeometryInstanceID ins,MeMatrix4Ptr finalTM,MeBool tight)

{
                    /* Unresolved local var: McdGeometryID g@[DW_OP_reg0(EAX)]
                       Unresolved local var: McdGeometryGetAABBFnPtr fn@[???]
                       Unresolved local var: int type@[DW_OP_reg2(EDX)] */
  (*ins->mGeometry->frame->geometryVTableTable[(byte)ins->mGeometry->mRefCtAndID].getAABB)
            (ins,finalTM,tight);
  return;
}


/* ==== McdGeometryInstanceMaximumPoint ==== */

void McdGeometryInstanceMaximumPoint(McdGeometryInstanceID ins,MeReal *inDir,MeReal *outPoint)

{
                    /* Unresolved local var: McdGeometryID g@[DW_OP_reg0(EAX)]
                       Unresolved local var: McdGeometryMaximumPointFnPtr fn@[???] */
  (*ins->mGeometry->frame->geometryVTableTable[(byte)ins->mGeometry->mRefCtAndID].maximumPoint)
            (ins,inDir,outPoint);
  return;
}


/* ==== McdGeometryInstanceGetAABB ==== */

void McdGeometryInstanceGetAABB(McdGeometryInstanceID ins,MeReal *minCorner,MeReal *maxCorner)

{
  *minCorner = ins->min[0];
  minCorner[1] = ins->min[1];
  minCorner[2] = ins->min[2];
  *maxCorner = ins->max[0];
  maxCorner[1] = ins->max[1];
  maxCorner[2] = ins->max[2];
  return;
}


/* ==== McdGeometryInstanceGetTransformPtr ==== */

MeMatrix4Ptr McdGeometryInstanceGetTransformPtr(McdGeometryInstanceID ins)

{
  return ins->mTM;
}


/* ==== McdGeometryInstanceSetTransformPtr ==== */

void McdGeometryInstanceSetTransformPtr(McdGeometryInstanceID ins,MeMatrix4Ptr tm)

{
  ins->mTM = tm;
  return;
}


/* ==== McdGeometryInstanceSetMaterial ==== */

void McdGeometryInstanceSetMaterial(McdGeometryInstanceID ins,int material)

{
  ins->mMaterial = material;
  return;
}


/* ==== McdGeometryInstanceGetMaterial ==== */

uint McdGeometryInstanceGetMaterial(McdGeometryInstanceID ins)

{
  return ins->mMaterial;
}


/* ==== McdGeometryInstanceGetChild ==== */

McdGeometryInstanceID McdGeometryInstanceGetChild(McdGeometryInstanceID ins,int num)

{
  _McdGeometryInstance *p_Var1;
  uint uVar2;
  McdGeometryInstanceID p_Var3;
  
                    /* Unresolved local var: McdGeometryInstanceID child@[DW_OP_reg2(EDX)] */
  p_Var3 = ins->child;
  if (p_Var3 == (McdGeometryInstanceID)0x0) {
    return (McdGeometryInstanceID)0x0;
  }
  if (0 < num) {
    uVar2 = -num & 3;
    if (0 < num + -1) {
      if (uVar2 == 0) goto LAB_00010633;
      if (uVar2 < 3) {
        if ((uVar2 < 2) &&
           (p_Var3 = p_Var3->next, num = num + -1, p_Var3 == (McdGeometryInstanceID)0x0)) {
          return (McdGeometryInstanceID)0x0;
        }
        p_Var3 = p_Var3->next;
        num = num + -1;
        if (p_Var3 == (McdGeometryInstanceID)0x0) {
          return (McdGeometryInstanceID)0x0;
        }
      }
    }
    p_Var3 = p_Var3->next;
    num = num + -1;
    while( true ) {
      if ((p_Var3 == (McdGeometryInstanceID)0x0) || (num < 1)) {
        return p_Var3;
      }
LAB_00010633:
      if (p_Var3->next == (_McdGeometryInstance *)0x0) {
        return (McdGeometryInstanceID)0x0;
      }
      p_Var1 = p_Var3->next->next;
      if (p_Var1 == (_McdGeometryInstance *)0x0) break;
      p_Var1 = p_Var1->next;
      if (p_Var1 == (_McdGeometryInstance *)0x0) {
        return (McdGeometryInstanceID)0x0;
      }
      p_Var3 = p_Var1->next;
      num = num + -4;
    }
    return (McdGeometryInstanceID)0x0;
  }
  return p_Var3;
}


/* ==== McdGeometryInstanceOverlap ==== */

MeBool McdGeometryInstanceOverlap(McdGeometryInstanceID ins1,McdGeometryInstanceID ins2)

{
  bool bVar1;
  bool bVar2;
  MeBool MVar3;
  
                    /* Unresolved local var: int result@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[???] */
  bVar1 = false;
  if ((ins1->min[0] < ins2->max[0]) && (ins2->min[0] < ins1->max[0])) {
    bVar1 = true;
  }
  bVar2 = false;
  if (((bVar1) && (ins1->min[1] < ins2->max[1])) && (ins2->min[1] < ins1->max[1])) {
    bVar2 = true;
  }
  MVar3 = 0;
  if (((bVar2) && (ins1->min[2] < ins2->max[2])) && (ins2->min[2] < ins1->max[2])) {
    MVar3 = 1;
  }
  return MVar3;
}


/* ==== McdGeometryInstanceGetGeometryType ==== */

McdGeometryType McdGeometryInstanceGetGeometryType(McdGeometryInstanceID ins)

{
  return (McdGeometryType)(byte)ins->mGeometry->mRefCtAndID;
}


/* ==== McdGeometryInstanceDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdGeometryInstanceDestroy(McdFrameworkID frame,McdGeometryInstanceID ins)

{
  _McdGeometryInstance *p_Var1;
  McdGeometryInstanceID ins_00;
  undefined4 uVar2;
  undefined4 extraout_EAX;
  
  ins_00 = ins->child;
  while (ins_00 != (McdGeometryInstanceID)0x0) {
                    /* Unresolved local var: McdGeometryInstanceID child@[DW_OP_reg0(EAX)]
                       Unresolved local var: McdGeometryInstanceID nextChild@[DW_OP_reg3(EBX)] */
    p_Var1 = ins_00->next;
    McdGeometryInstanceDestroy(frame,ins_00);
    ins_00 = p_Var1;
  }
  uVar2 = 0;
  if (ins->mGeometry != (McdGeometry *)0x0) {
    McdGeometryDecrementReferenceCount(ins->mGeometry);
    uVar2 = extraout_EAX;
  }
  (*_DAT_0001101c)(&frame->instancePool,ins,uVar2,uVar2);
  return;
}


