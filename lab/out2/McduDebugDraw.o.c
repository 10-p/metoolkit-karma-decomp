/* ==== McduDebugDrawAABB ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McduDebugDrawAABB(MeReal *min,MeReal *max,MeReal *colour)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  int iVar7;
  int iVar8;
  MeReal wv2 [3];
  MeReal wv1 [3];
  MeReal lv [3];
  MeReal radius [3];
  MeReal centre [3];
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  iVar8 = 0xb;
  fVar1 = (*max + *min) * 0.5;
  fVar2 = (min[1] + max[1]) * 0.5;
  fVar3 = (min[2] + max[2]) * 0.5;
  fVar4 = (*min - *max) * 0.5;
  fVar5 = (min[1] - max[1]) * 0.5;
  fVar6 = (min[2] - max[2]) * 0.5;
  iVar7 = 0;
  do {
    wv1[0] = *(float *)((int)boxDraw[0][0] + iVar7) * fVar4 + fVar1;
    wv1[1] = *(float *)((int)boxDraw[0][0] + iVar7 + 4) * fVar5 + fVar2;
    wv1[2] = fVar6 * *(float *)((int)boxDraw[0][0] + iVar7 + 8) + fVar3;
    wv2[0] = fVar1 + *(float *)((int)boxDraw[0][1] + iVar7) * fVar4;
    wv2[1] = fVar2 + *(float *)((int)boxDraw[0][1] + iVar7 + 4) * fVar5;
    wv2[2] = fVar3 + fVar6 * *(float *)((int)boxDraw[0][1] + iVar7 + 8);
    (*_MeDebugDrawAPI)(wv1,wv2,*colour,colour[1],colour[2]);
    wv1[0] = *(float *)((int)boxDraw[1][0] + iVar7) * fVar4 + fVar1;
    wv1[1] = *(float *)((int)boxDraw[1][0] + iVar7 + 4) * fVar5 + fVar2;
    wv1[2] = fVar6 * *(float *)((int)boxDraw[1][0] + iVar7 + 8) + fVar3;
    wv2[0] = fVar1 + *(float *)((int)boxDraw[1][1] + iVar7) * fVar4;
    wv2[2] = fVar3 + fVar6 * *(float *)((int)boxDraw[1][1] + iVar7 + 8);
    wv2[1] = fVar2 + *(float *)((int)boxDraw[1][1] + iVar7 + 4) * fVar5;
    (*_MeDebugDrawAPI)(wv1,wv2,*colour,colour[1],colour[2]);
    iVar8 = iVar8 + -2;
    iVar7 = iVar7 + 0x30;
  } while (-1 < iVar8);
  return;
}


/* ==== McduDebugDrawModel ==== */

void McduDebugDrawModel(McdModelID_conflict model,McduDebugDrawFlags flags,MeReal *colour)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 extraout_EAX;
  undefined4 extraout_ECX;
  McdGeometryID geom;
  MeReal max [3];
  MeReal min [3];
  
                    /* Unresolved local var: MeMatrix4Ptr tm@[DW_OP_reg6(ESI)] */
  uVar1 = McdModelGetGeometry(model);
  uVar2 = McdModelGetTransformPtr(model);
  uVar3 = uVar2;
  if ((flags & kMcduDebugDrawAABB) != 0) {
    McdModelGetAABB(model,min,max,extraout_ECX);
    McduDebugDrawAABB(min,max,colour);
    uVar3 = extraout_EAX;
  }
  if ((flags & kMcduDebugDrawDetail) != 0) {
    McdGeometryDebugDraw(uVar1,uVar2,colour,uVar3);
  }
  return;
}


/* ==== McduDebugDrawSpace ==== */

void McduDebugDrawSpace(McdSpaceID space,McduDebugDrawFlags flags,MeReal *colour)

{
  McdModelID_conflict pMVar1;
  int iVar2;
  undefined4 uVar3;
  int extraout_EAX;
  undefined8 uVar4;
  MeMatrix4Ptr tm;
  McdGeometryID geom;
  McdModelID_conflict m;
  McdSpaceModelIterator it;
  MeReal max [3];
  MeReal min [3];
  
  iVar2 = McdSpaceModelIteratorBegin(space,&it);
LAB_00010290:
  iVar2 = McdSpaceGetModel(space,&it,&m,iVar2);
  if (iVar2 == 0) {
    return;
  }
  if ((flags & kMcduDebugDrawEnabledOnly) != 0) goto code_r0x000102b2;
  goto LAB_000102c5;
code_r0x000102b2:
  iVar2 = McdSpaceModelIsFrozen(m);
  if (iVar2 == 0) {
LAB_000102c5:
    pMVar1 = m;
    uVar3 = McdModelGetGeometry(m);
    uVar4 = McdModelGetTransformPtr(pMVar1);
    iVar2 = (int)uVar4;
    if ((flags & kMcduDebugDrawAABB) != 0) {
      McdModelGetAABB(pMVar1,min,max,(int)((ulonglong)uVar4 >> 0x20));
      McduDebugDrawAABB(min,max,colour);
      iVar2 = extraout_EAX;
    }
    if ((flags & kMcduDebugDrawDetail) != 0) {
      iVar2 = McdGeometryDebugDraw(uVar3,(int)uVar4,colour,iVar2);
    }
  }
  goto LAB_00010290;
}


