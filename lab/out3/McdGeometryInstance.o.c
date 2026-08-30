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
  McdFrameworkID pMVar1;
  McdFrameworkID frame;
  uint uVar2;
  void *pvVar3;
  _McdGeometryInstance *p_Var4;
  uint uVar5;
  _McdGeometryInstance *p_Var6;
  McdGeometryID pMVar7;
  undefined4 uVar8;
  undefined4 extraout_EAX;
  McdGeometryInstanceID extraout_EDX;
  McdGeometryInstanceID p_Var9;
  McdGeometryInstanceID extraout_EDX_00;
  McdGeometryInstanceID extraout_EDX_01;
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
    pvVar3 = McdGeometryGetFramework(ins->mGeometry);
    if (ins->child != (_McdGeometryInstance *)0x0) {
      p_Var9 = extraout_EDX;
      p_Var4 = ins->child;
      do {
        p_Var6 = p_Var4->next;
        frame = (McdFrameworkID)p_Var4->child;
        while (frame != (McdFrameworkID)0x0) {
                    /* Unresolved local var: McdGeometryInstanceID child@[DW_OP_reg0(EAX)]
                       Unresolved local var: McdGeometryInstanceID nextChild@[DW_OP_reg3(EBX)] */
          pMVar1 = (frame->cachePool).u.fixed.structArray;
          McdGeometryInstanceDestroy(frame,p_Var9);
          frame = pMVar1;
          p_Var9 = extraout_EDX_00;
        }
        uVar8 = 0;
        if (p_Var4->mGeometry != (McdGeometry *)0x0) {
          McdGeometryDecrementReferenceCount(p_Var4->mGeometry);
          uVar8 = extraout_EAX;
        }
        (*_DAT_0001101c)((int)pvVar3 + 0x60,p_Var4,uVar8,uVar8);
        p_Var9 = extraout_EDX_01;
        p_Var4 = p_Var6;
      } while (p_Var6 != (_McdGeometryInstance *)0x0);
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
  uVar2 = McdAggregateGetElementCountMax(geom);
  if (uVar2 == 0) {
    return;
  }
  pvVar3 = McdGeometryGetFramework(geom);
                    /* Unresolved local var: McdGeometryInstance * ins@[DW_OP_reg0(EAX)] */
  iVar10 = (int)pvVar3 + 0x60;
  p_Var4 = (_McdGeometryInstance *)(*___gxx_personality_v0)(iVar10);
  p_Var4->min[0] = 3.4028235e+38;
  p_Var4->mMaterial = 0;
  p_Var4->mTM = (MeMatrix4Ptr)0x0;
  p_Var4->mGeometry = (McdGeometry *)0x0;
  p_Var4->min[1] = 3.4028235e+38;
  p_Var4->min[2] = 3.4028235e+38;
  p_Var4->max[0] = -3.4028235e+38;
  p_Var4->max[1] = -3.4028235e+38;
  p_Var4->max[2] = -3.4028235e+38;
  p_Var4->parent = (_McdGeometryInstance *)0x0;
  p_Var4->child = (_McdGeometryInstance *)0x0;
  p_Var4->next = (_McdGeometryInstance *)0x0;
  p_Var4->prev = (_McdGeometryInstance *)0x0;
  p_Var4->parent = ins;
  ins->child = p_Var4;
  uVar5 = uVar2 - 1;
  if (0 < (int)uVar5) {
    p_Var6 = p_Var4;
    if ((uVar5 & 1) != 0) {
                    /* Unresolved local var: McdGeometryInstance * ins@[???] */
      p_Var6 = (_McdGeometryInstance *)(*___gxx_personality_v0)(iVar10);
      p_Var6->min[0] = 3.4028235e+38;
      p_Var6->mMaterial = 0;
      p_Var6->mTM = (MeMatrix4Ptr)0x0;
      p_Var6->mGeometry = (McdGeometry *)0x0;
      p_Var6->min[1] = 3.4028235e+38;
      p_Var6->min[2] = 3.4028235e+38;
      p_Var6->max[0] = -3.4028235e+38;
      p_Var6->max[1] = -3.4028235e+38;
      p_Var6->max[2] = -3.4028235e+38;
      p_Var6->next = (_McdGeometryInstance *)0x0;
      p_Var6->child = (_McdGeometryInstance *)0x0;
      p_Var6->prev = p_Var4;
      p_Var4->next = p_Var6;
      p_Var6->parent = ins;
      uVar5 = uVar2 - 2;
      if (uVar5 == 0) goto LAB_00010362;
    }
    do {
      p_Var4 = (_McdGeometryInstance *)(*___gxx_personality_v0)(iVar10);
      p_Var4->mGeometry = (McdGeometry *)0x0;
      p_Var4->min[0] = 3.4028235e+38;
      p_Var4->mMaterial = 0;
      p_Var4->mTM = (MeMatrix4Ptr)0x0;
      p_Var4->min[1] = 3.4028235e+38;
      p_Var4->min[2] = 3.4028235e+38;
      p_Var4->max[0] = -3.4028235e+38;
      p_Var4->max[1] = -3.4028235e+38;
      p_Var4->max[2] = -3.4028235e+38;
      p_Var4->next = (_McdGeometryInstance *)0x0;
      p_Var4->child = (_McdGeometryInstance *)0x0;
      p_Var4->prev = p_Var6;
      p_Var4->parent = ins;
      p_Var6->next = p_Var4;
      p_Var6 = (_McdGeometryInstance *)(*___gxx_personality_v0)(iVar10);
      uVar5 = uVar5 - 2;
      p_Var6->mGeometry = (McdGeometry *)0x0;
      p_Var6->min[0] = 3.4028235e+38;
      p_Var6->mMaterial = 0;
      p_Var6->mTM = (MeMatrix4Ptr)0x0;
      p_Var6->min[1] = 3.4028235e+38;
      p_Var6->min[2] = 3.4028235e+38;
      p_Var6->max[0] = -3.4028235e+38;
      p_Var6->max[1] = -3.4028235e+38;
      p_Var6->max[2] = -3.4028235e+38;
      p_Var6->next = (_McdGeometryInstance *)0x0;
      p_Var6->child = (_McdGeometryInstance *)0x0;
      p_Var6->prev = p_Var4;
      p_Var4->next = p_Var6;
      p_Var6->parent = ins;
    } while (uVar5 != 0);
  }
LAB_00010362:
  iVar10 = 0;
  p_Var4 = ins->child;
  if ((int)uVar2 < 1) {
    return;
  }
  uVar5 = uVar2 & 3;
  if (1 < (int)uVar2) {
    if (uVar5 == 0) goto LAB_000103a8;
    if (1 < uVar5) {
      if (2 < uVar5) {
                    /* Unresolved local var: McdGeometryID g@[???] */
        pMVar7 = McdAggregateGetElementGeometry(geom,0);
        McdGeometryInstanceSetGeometry(p_Var4,pMVar7);
        p_Var4 = p_Var4->next;
      }
      uVar5 = (uint)(2 < uVar5);
      iVar10 = uVar5 + 1;
      pMVar7 = McdAggregateGetElementGeometry(geom,uVar5);
      McdGeometryInstanceSetGeometry(p_Var4,pMVar7);
      p_Var4 = p_Var4->next;
    }
  }
  iVar11 = iVar10 + 1;
  pMVar7 = McdAggregateGetElementGeometry(geom,iVar10);
  McdGeometryInstanceSetGeometry(p_Var4,pMVar7);
  p_Var4 = p_Var4->next;
  iVar10 = iVar11;
  if ((int)uVar2 <= iVar11) {
    return;
  }
LAB_000103a8:
  do {
    pMVar7 = McdAggregateGetElementGeometry(geom,iVar10);
    McdGeometryInstanceSetGeometry(p_Var4,pMVar7);
    p_Var9 = p_Var4->next;
    pMVar7 = McdAggregateGetElementGeometry(geom,iVar10 + 1);
    McdGeometryInstanceSetGeometry(p_Var9,pMVar7);
    p_Var9 = p_Var9->next;
    pMVar7 = McdAggregateGetElementGeometry(geom,iVar10 + 2);
    iVar11 = iVar10 + 3;
    iVar10 = iVar10 + 4;
    McdGeometryInstanceSetGeometry(p_Var9,pMVar7);
    p_Var9 = p_Var9->next;
    pMVar7 = McdAggregateGetElementGeometry(geom,iVar11);
    McdGeometryInstanceSetGeometry(p_Var9,pMVar7);
    p_Var4 = p_Var9->next;
  } while (iVar10 < (int)uVar2);
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

void __regparm2 McdGeometryInstanceDestroy(McdFrameworkID frame,McdGeometryInstanceID ins)

{
  McdFrameworkID pMVar1;
  McdFrameworkID frame_00;
  McdGeometryInstanceID extraout_EDX;
  int in_stack_00000004;
  undefined4 *in_stack_00000008;
  
  frame_00 = (McdFrameworkID)in_stack_00000008[0xc];
  while (frame_00 != (McdFrameworkID)0x0) {
                    /* Unresolved local var: McdGeometryInstanceID child@[DW_OP_reg0(EAX)]
                       Unresolved local var: McdGeometryInstanceID nextChild@[DW_OP_reg3(EBX)] */
    pMVar1 = (frame_00->cachePool).u.fixed.structArray;
    McdGeometryInstanceDestroy(frame_00,ins);
    frame_00 = pMVar1;
    ins = extraout_EDX;
  }
  if ((void *)*in_stack_00000008 != (void *)0x0) {
    McdGeometryDecrementReferenceCount((void *)*in_stack_00000008);
  }
  (*_DAT_0001101c)(in_stack_00000004 + 0x60);
  return;
}


