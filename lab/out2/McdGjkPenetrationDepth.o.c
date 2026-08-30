/* ==== McdGjkFaceAddPoint ==== */

void McdGjkFaceAddPoint(McdGjkFaceQueue *q,int i)

{
  int iVar1;
  McdGjkPoint *pMVar2;
  McdGjkSimplex *pMVar3;
  
  iVar1 = q->nump;
  pMVar2 = q->point;
  pMVar3 = q->s;
  pMVar2[iVar1].w[0] = pMVar3->point[i].w[0];
  pMVar2[iVar1].w[1] = pMVar3->point[i].w[1];
  pMVar2[iVar1].w[2] = pMVar3->point[i].w[2];
  pMVar2[iVar1].s2[0] = pMVar3->point[i].s2[0];
  pMVar2[iVar1].s2[1] = pMVar3->point[i].s2[1];
  pMVar2[iVar1].s2[2] = pMVar3->point[i].s2[2];
  iVar1 = q->nump;
  q->si[i] = iVar1;
  q->nump = iVar1 + 1;
  return;
}


/* ==== McdGjkFaceCompare ==== */

int McdGjkFaceCompare(void *f1,void *f2)

{
  return (uint)(*(float *)((int)f1 + 0xc) < *(float *)((int)f2 + 0xc));
}


/* ==== McdGjkFaceAdd ==== */

void McdGjkFaceAdd(McdGjkFaceQueue *q,MeReal *v,int bits,int slant,int depth)

{
  MeReal *v_00;
  MeReal MVar1;
  int depth_local;
  int slant_local;
  int bits_local;
  
                    /* Unresolved local var: McdGjkFace * f@[DW_OP_reg3(EBX)] */
  v_00 = (MeReal *)MePoolxGet(&q->fpool);
  if (v_00 != (MeReal *)0x0) {
    v_00[5] = (MeReal)bits;
    v_00[4] = (MeReal)depth;
    v_00[10] = (MeReal)slant;
    *v_00 = *v;
    v_00[1] = v[1];
    v_00[2] = v[2];
    MVar1 = MeVector3Normalize(v_00);
    v_00[3] = MVar1;
    v_00[6] = (MeReal)q->si[0];
    v_00[7] = (MeReal)q->si[1];
    v_00[8] = (MeReal)q->si[2];
    v_00[9] = (MeReal)q->si[3];
    MeHeapPush();
    return;
  }
  return;
}


/* ==== McdGjkFacePop ==== */

McdGjkFace * McdGjkFacePop(McdGjkFaceQueue *q)

{
  McdGjkFace *pMVar1;
  
                    /* Unresolved local var: McdGjkFace * f@[DW_OP_reg0(EAX)] */
  pMVar1 = (McdGjkFace *)MeHeapPop(&q->face);
  q->lastpop = pMVar1;
  return pMVar1;
}


/* ==== McdGjkFaceLoad ==== */

/* WARNING: Unknown calling convention */

void McdGjkFaceLoad(McdGjkFaceQueue *q,McdGjkFace *f)

{
  McdGjkSimplex *pMVar1;
  int iVar2;
  McdGjkPoint *pMVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int local_18;
  int stale_bits;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int b@[DW_OP_reg6(ESI)] */
  uVar4 = 1;
  iVar6 = 0;
  stale_bits = 0;
  uVar5 = f->bits;
  if (0 < (int)uVar5) {
    do {
      if (((uVar5 & uVar4) != 0) && (f->fi[iVar6] != q->si[iVar6])) {
        stale_bits = stale_bits | uVar4;
      }
      uVar4 = uVar4 * 2;
      iVar6 = iVar6 + 1;
    } while ((int)uVar4 <= (int)uVar5);
  }
  iVar6 = 0;
  q->s->bits = uVar5 & (stale_bits ^ 0xffffffffU);
  if (0 < stale_bits) {
    local_18 = 0;
    uVar5 = 1;
    do {
      if ((stale_bits & uVar5) != 0) {
        q->s->next_i = iVar6;
        q->s->next_bit = uVar5;
        pMVar1 = q->s;
        iVar2 = f->fi[iVar6];
        pMVar3 = q->point;
        *(MeReal *)((int)pMVar1->point[0].w + local_18) = pMVar3[iVar2].w[0];
        *(MeReal *)((int)pMVar1->point[0].w + local_18 + 4) = pMVar3[iVar2].w[1];
        *(MeReal *)((int)pMVar1->point[0].w + local_18 + 8) = pMVar3[iVar2].w[2];
        *(MeReal *)((int)pMVar1->point[0].s2 + local_18) = pMVar3[iVar2].s2[0];
        *(MeReal *)((int)pMVar1->point[0].s2 + local_18 + 4) = pMVar3[iVar2].s2[1];
        *(MeReal *)((int)pMVar1->point[0].s2 + local_18 + 8) = pMVar3[iVar2].s2[2];
        q->si[iVar6] = f->fi[iVar6];
        McdGjkUpdateDotCache(q->s);
        McdGjkUpdateDeltaCache(q->s);
        q->s->bits = q->s->bits | uVar5;
      }
      uVar5 = uVar5 * 2;
      local_18 = local_18 + 0x18;
      iVar6 = iVar6 + 1;
    } while ((int)uVar5 <= stale_bits);
  }
  return;
}


/* ==== McdGjkFindNextInflatePoint ==== */

MeReal McdGjkFindNextInflatePoint(MeReal *v,MeReal v_len,McdCache *c,McdGjkSimplex *s)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  ushort uVar7;
  float fVar8;
  undefined4 *puVar9;
  ushort uVar10;
  longdouble lVar11;
  MeReal d1;
  MeReal neg [3];
  McdGjkPoint p1;
  
                    /* Unresolved local var: MeReal d2@[DW_OP_reg12(ST1)]
                       Unresolved local var: McdGjkPoint * p@[DW_OP_reg3(EBX)] */
  puVar9 = (undefined4 *)McdGjkNextAvailablePoint(s);
  fVar8 = s->eps * 10.0;
  uVar7 = (ushort)(v_len < fVar8) << 8 | (ushort)(NAN(v_len) || NAN(fVar8)) << 10;
  uVar10 = uVar7 | (ushort)(v_len == fVar8) << 0xe;
  if (uVar7 != 0) {
    McdGjkCrossProd(v,s,uVar10,uVar10);
  }
  lVar11 = (longdouble)McdGjkComputeSupport(puVar9,v,1,c);
  d1 = (MeReal)lVar11;
  if (v_len < s->eps * 10.0) {
    neg[0] = -*v;
    neg[1] = -v[1];
    neg[2] = -v[2];
    uVar1 = *puVar9;
    uVar2 = puVar9[1];
    uVar3 = puVar9[2];
    uVar4 = puVar9[3];
    uVar5 = puVar9[4];
    uVar6 = puVar9[5];
    lVar11 = (longdouble)McdGjkComputeSupport(puVar9,neg,1,c);
    if ((longdouble)d1 - (longdouble)10.0 * (longdouble)s->eps <= lVar11) {
      *puVar9 = uVar1;
      puVar9[1] = uVar2;
      puVar9[2] = uVar3;
      puVar9[3] = uVar4;
      puVar9[4] = uVar5;
      puVar9[5] = uVar6;
    }
    else {
      d1 = (MeReal)lVar11;
      *v = neg[0];
      v[1] = neg[1];
      v[2] = neg[2];
    }
  }
  return d1;
}


/* ==== McdGjkFaceQueueInit ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdGjkFaceQueueInit(McdGjkFaceQueue *q,McdGjkSimplex *s)

{
  McdGjkPoint *pMVar1;
  McdGjkSimplex *pMVar2;
  MeReal *pMVar3;
  int iVar4;
  MeReal MVar5;
  MeReal MVar6;
  int i;
  MeReal v [3];
  
                    /* Unresolved local var: int b@[DW_OP_reg6(ESI)] */
  iVar4 = q->nump;
  q->s = s;
  q->lastpop = (McdGjkFace *)0x0;
  pMVar1 = q->point;
  pMVar1[iVar4].w[0] = s->point[0].w[0];
  pMVar1[iVar4].w[1] = s->point[0].w[1];
  pMVar1[iVar4].w[2] = s->point[0].w[2];
  pMVar1[iVar4].s2[0] = s->point[0].s2[0];
  pMVar1[iVar4].s2[1] = s->point[0].s2[1];
  pMVar1[iVar4].s2[2] = s->point[0].s2[2];
  pMVar2 = q->s;
  q->si[0] = q->nump;
  iVar4 = q->nump + 1;
  pMVar1 = q->point;
  q->nump = iVar4;
  pMVar1[iVar4].w[0] = pMVar2->point[1].w[0];
  pMVar1[iVar4].w[1] = pMVar2->point[1].w[1];
  pMVar1[iVar4].w[2] = pMVar2->point[1].w[2];
  pMVar1[iVar4].s2[0] = pMVar2->point[1].s2[0];
  pMVar1[iVar4].s2[1] = pMVar2->point[1].s2[1];
  pMVar1[iVar4].s2[2] = pMVar2->point[1].s2[2];
  iVar4 = q->nump;
  pMVar1 = q->point;
  q->si[1] = iVar4;
  iVar4 = iVar4 + 1;
  q->nump = iVar4;
  pMVar2 = q->s;
  pMVar1[iVar4].w[0] = pMVar2->point[2].w[0];
  pMVar1[iVar4].w[1] = pMVar2->point[2].w[1];
  pMVar1[iVar4].w[2] = pMVar2->point[2].w[2];
  pMVar1[iVar4].s2[0] = pMVar2->point[2].s2[0];
  pMVar1[iVar4].s2[1] = pMVar2->point[2].s2[1];
  pMVar1[iVar4].s2[2] = pMVar2->point[2].s2[2];
  iVar4 = q->nump;
  pMVar2 = q->s;
  q->si[2] = iVar4;
  iVar4 = iVar4 + 1;
  q->nump = iVar4;
  pMVar1 = q->point;
  pMVar1[iVar4].w[0] = pMVar2->point[3].w[0];
  pMVar1[iVar4].w[1] = pMVar2->point[3].w[1];
  pMVar1[iVar4].w[2] = pMVar2->point[3].w[2];
  pMVar1[iVar4].s2[0] = pMVar2->point[3].s2[0];
  pMVar1[iVar4].s2[1] = pMVar2->point[3].s2[1];
  pMVar1[iVar4].s2[2] = pMVar2->point[3].s2[2];
  iVar4 = q->nump;
  q->si[3] = iVar4;
  q->nump = iVar4 + 1;
  i = 0;
  do {
    MVar5 = *(MeReal *)(_DAT_0001205c + 4 + i * 4);
    iVar4 = McdGjkComputeVector(v,MVar5,0,s);
    if (iVar4 == 0) {
      if (((uint)MVar5 & 1) != 0) {
        iVar4 = McdGjkComputeVector(v,(int)MVar5 - 1,0,s);
        if (iVar4 != 0) {
                    /* Unresolved local var: McdGjkFace * f@[DW_OP_reg3(EBX)] */
          pMVar3 = (MeReal *)MePoolxGet(&q->fpool);
          if (pMVar3 != (MeReal *)0x0) {
            pMVar3[5] = MVar5;
            pMVar3[4] = 0.0;
            pMVar3[10] = 1.4013e-45;
            *pMVar3 = v[0];
            pMVar3[1] = v[1];
            pMVar3[2] = v[2];
            MVar6 = MeVector3Normalize(pMVar3);
            pMVar3[3] = MVar6;
            pMVar3[6] = (MeReal)q->si[0];
            pMVar3[7] = (MeReal)q->si[1];
            pMVar3[8] = (MeReal)q->si[2];
            pMVar3[9] = (MeReal)q->si[3];
            MeHeapPush(&q->face,pMVar3);
          }
        }
      }
      if (((uint)MVar5 & 2) != 0) {
        iVar4 = McdGjkComputeVector(v,(int)MVar5 - 2,0,s);
        if (iVar4 != 0) {
                    /* Unresolved local var: McdGjkFace * f@[DW_OP_reg3(EBX)] */
          pMVar3 = (MeReal *)MePoolxGet(&q->fpool);
          if (pMVar3 != (MeReal *)0x0) {
            pMVar3[5] = MVar5;
            pMVar3[4] = 0.0;
            pMVar3[10] = 2.8026e-45;
            *pMVar3 = v[0];
            pMVar3[1] = v[1];
            pMVar3[2] = v[2];
            MVar6 = MeVector3Normalize(pMVar3);
            pMVar3[3] = MVar6;
            pMVar3[6] = (MeReal)q->si[0];
            pMVar3[7] = (MeReal)q->si[1];
            pMVar3[8] = (MeReal)q->si[2];
            pMVar3[9] = (MeReal)q->si[3];
            MeHeapPush(&q->face,pMVar3);
          }
        }
      }
      if (((uint)MVar5 & 4) != 0) {
        iVar4 = McdGjkComputeVector(v,(int)MVar5 - 4,0,s);
        if (iVar4 != 0) {
                    /* Unresolved local var: McdGjkFace * f@[DW_OP_reg3(EBX)] */
          pMVar3 = (MeReal *)MePoolxGet(&q->fpool);
          if (pMVar3 != (MeReal *)0x0) {
            pMVar3[5] = MVar5;
            pMVar3[4] = 0.0;
            pMVar3[10] = 5.60519e-45;
            *pMVar3 = v[0];
            pMVar3[1] = v[1];
            pMVar3[2] = v[2];
            MVar6 = MeVector3Normalize(pMVar3);
            pMVar3[3] = MVar6;
            pMVar3[6] = (MeReal)q->si[0];
            pMVar3[7] = (MeReal)q->si[1];
            pMVar3[8] = (MeReal)q->si[2];
            pMVar3[9] = (MeReal)q->si[3];
            MeHeapPush(&q->face,pMVar3);
          }
        }
      }
      if (((uint)MVar5 & 8) != 0) {
        iVar4 = McdGjkComputeVector(v,(int)MVar5 - 8,0,s);
        if (iVar4 != 0) {
                    /* Unresolved local var: McdGjkFace * f@[DW_OP_reg3(EBX)] */
          pMVar3 = (MeReal *)MePoolxGet(&q->fpool);
          if (pMVar3 != (MeReal *)0x0) {
            pMVar3[5] = MVar5;
            pMVar3[4] = 0.0;
            pMVar3[10] = 1.12104e-44;
            goto LAB_000105d4;
          }
        }
      }
    }
    else {
                    /* Unresolved local var: McdGjkFace * f@[DW_OP_reg3(EBX)] */
      pMVar3 = (MeReal *)MePoolxGet(&q->fpool);
      if (pMVar3 != (MeReal *)0x0) {
        pMVar3[5] = MVar5;
        pMVar3[4] = 0.0;
        pMVar3[10] = 0.0;
LAB_000105d4:
        *pMVar3 = v[0];
        pMVar3[1] = v[1];
        pMVar3[2] = v[2];
        MVar5 = MeVector3Normalize(pMVar3);
        pMVar3[3] = MVar5;
        pMVar3[6] = (MeReal)q->si[0];
        pMVar3[7] = (MeReal)q->si[1];
        pMVar3[8] = (MeReal)q->si[2];
        pMVar3[9] = (MeReal)q->si[3];
        MeHeapPush(&q->face,pMVar3);
      }
    }
    i = i + 1;
    if (3 < i) {
      return;
    }
  } while( true );
}


/* ==== McdGjkResultIsReasonable ==== */

int McdGjkResultIsReasonable(McdCache *c,McdGjkSimplex *s)

{
  MeMatrix4Ptr paMVar1;
  MeMatrix4Ptr paMVar2;
  float fVar3;
  float fVar4;
  undefined2 extraout_var;
  uint uVar5;
  MeReal d1;
  MeReal p [3];
  MeReal v [3];
  MeReal neg [3];
  MeReal s2 [3];
  MeReal s1 [3];
  
                    /* Unresolved local var: MeReal d2@[???]
                       Unresolved local var: MeReal dp@[???] */
  neg[0] = c->normal[0] * -1.0;
  neg[1] = c->normal[1] * -1.0;
  neg[2] = c->normal[2] * -1.0;
  McdGjkMaximumPoint(c->ins1,neg,s1);
  McdGjkMaximumPoint(c->ins2,c,s2);
  fVar3 = (s1[2] - s2[2]) * c->normal[2] +
          (s1[1] - s2[1]) * c->normal[1] + (s1[0] - s2[0]) * c->normal[0];
  uVar5 = 0;
  if (ABS((fVar3 + c->normal[2] * c->offset[2] +
                   c->normal[0] * c->offset[0] + c->normal[1] * c->offset[1]) - c->separation) <=
      0.1) {
    paMVar1 = c->ins2->mTM;
    paMVar2 = c->ins1->mTM;
    p[0] = paMVar2[3][0] - paMVar1[3][0];
    p[1] = paMVar2[3][1] - paMVar1[3][1];
    p[2] = paMVar2[3][2] - paMVar1[3][2];
    MeVector3Normalize(p);
    uVar5 = 0;
    fVar4 = c->normal[2] * p[2] + c->normal[1] * p[1] + c->normal[0] * p[0];
    if (fVar4 >= 0.1) {
      neg[2] = p[2] * -1.0;
      neg[0] = p[0] * -1.0;
      neg[1] = p[1] * -1.0;
      McdGjkMaximumPoint(c->ins1,neg,s1,
                         CONCAT22(extraout_var,
                                  (ushort)(fVar4 < 0.1) << 8 | (ushort)NAN(fVar4) << 10 |
                                  (ushort)(fVar4 == 0.1) << 0xe));
      McdGjkMaximumPoint(c->ins2,p,s2);
      uVar5 = (uint)(((s1[0] - s2[0]) * p[0] + (s1[1] - s2[1]) * p[1] + (s1[2] - s2[2]) * p[2]) -
                     0.01 <= fVar3);
    }
  }
  return uVar5;
}


/* ==== McdGjkPenetrationDepth ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */

int McdGjkPenetrationDepth(McdCache *c,McdGjkSimplex *s)

{
  float fVar1;
  MeReal MVar2;
  ushort uVar3;
  float fVar4;
  McdGjkFace *pMVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  MeReal *v_00;
  MeReal MVar10;
  MeReal MVar11;
  McdGjkFaceQueue *q;
  McdGjkFace *pMVar12;
  McdCache *pMVar13;
  McdCache *pMVar14;
  McdGjkFace *min_f;
  MeReal min_d;
  MeReal d;
  int ok;
  int depth;
  int i;
  MeReal v [3];
  McdGjkFaceQueue qmem;
  void *heapmem [50];
  McdGjkFace poolmem [50];
  McdGjkPoint ptmem [50];
  
                    /* Unresolved local var: int b@[DW_OP_reg3(EBX)]
                       Unresolved local var: int slant@[DW_OP_reg7(EDI)]
                       Unresolved local var: int is_dup@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdGjkFace * f@[DW_OP_reg6(ESI)]
                       Unresolved local var: McdGjkFaceQueue * q@[???] */
  s->inflate = 1;
  if (s->bits == 0xf) {
    min_d = 3.4028235e+38;
    min_f = (McdGjkFace *)0x0;
    MePoolxInit(&qmem.fpool,poolmem,0x2c,0x32);
    MeHeapInit(&qmem.face,heapmem,0x32,McdGjkFaceCompare);
    qmem.maxp = 0x32;
    qmem.nump = 1;
    qmem.point = ptmem;
    McdGjkFaceQueueInit(&qmem,s);
    while( true ) {
                    /* Unresolved local var: McdGjkFace * f@[DW_OP_reg3(EBX)] */
      pMVar5 = (McdGjkFace *)MeHeapPop(&qmem.face);
      ok = 0;
      qmem.lastpop = pMVar5;
      if ((pMVar5 == (McdGjkFace *)0x0) || (0x31 < qmem.nump)) break;
      ok = 1;
      fVar4 = min_d - s->eps;
      fVar1 = pMVar5->v_len;
      uVar3 = (ushort)(fVar4 < fVar1) << 8 | (ushort)(NAN(fVar4) || NAN(fVar1)) << 10;
      uVar6 = (uint)(ushort)(uVar3 | (ushort)(fVar4 == fVar1) << 0xe);
      if ((uVar3 != 0) && (pMVar5->slant == 0)) break;
      pMVar12 = pMVar5;
      uVar8 = uVar6;
      McdGjkFaceLoad(&qmem,pMVar5);
      MVar11 = McdGjkFindNextInflatePoint((MeReal *)pMVar5,pMVar5->v_len,c,s);
      iVar9 = s->next_i;
      qmem.point[qmem.nump].w[0] = (qmem.s)->point[iVar9].w[0];
      qmem.point[qmem.nump].w[1] = (qmem.s)->point[iVar9].w[1];
      qmem.point[qmem.nump].w[2] = (qmem.s)->point[iVar9].w[2];
      qmem.point[qmem.nump].s2[0] = (qmem.s)->point[iVar9].s2[0];
      qmem.point[qmem.nump].s2[1] = (qmem.s)->point[iVar9].s2[1];
      qmem.point[qmem.nump].s2[2] = (qmem.s)->point[iVar9].s2[2];
      qmem.si[iVar9] = qmem.nump;
      qmem.nump = qmem.nump + 1;
      uVar7 = McdGjkNextIsDuplicate(s,pMVar12,uVar6,uVar8);
      uVar6 = pMVar5->slant;
      MVar10 = (MeReal)(pMVar5->depth + 1);
      uVar8 = uVar7;
      if ((uVar6 == 0) &&
         (uVar3 = (ushort)(MVar11 < min_d) << 8 | (ushort)(NAN(MVar11) || NAN(min_d)) << 10,
         uVar8 = (uint)(ushort)(uVar3 | (ushort)(MVar11 == min_d) << 0xe), uVar3 != 0)) {
        if (min_f != (McdGjkFace *)0x0) {
          MePoolxPut(&qmem.fpool,min_f,uVar8,uVar8);
        }
        min_f = pMVar5;
        min_d = MVar11;
        if (MVar11 - s->eps < pMVar5->v_len) break;
      }
      else {
        MePoolxPut(&qmem.fpool,pMVar5,uVar8,uVar8);
      }
      McdGjkUpdateDotCache(s);
      McdGjkUpdateDeltaCache(s);
      s->bits = 0xf;
      if (((int)MVar10 < 10) && (uVar7 == 0)) {
        i = 0;
        do {
          MVar11 = *(MeReal *)(_DAT_0001205c + 4 + i * 4);
          if ((((uint)MVar11 & s->next_bit) != 0) && ((uVar6 == 0 || ((uVar6 & (uint)MVar11) != 0)))
             ) {
            iVar9 = McdGjkComputeVector(v,MVar11,0,s);
            if (iVar9 == 0) {
              if ((uVar6 == 0) &&
                 (iVar9 = McdGjkComputeVector(v,(int)MVar11 - s->next_bit,0,s), iVar9 != 0)) {
                    /* Unresolved local var: McdGjkFace * f@[DW_OP_reg6(ESI)] */
                MVar2 = (MeReal)s->next_bit;
                v_00 = (MeReal *)MePoolxGet(&qmem.fpool);
                if (v_00 != (MeReal *)0x0) {
                  v_00[5] = MVar11;
                  v_00[4] = MVar10;
                  v_00[10] = MVar2;
                  *v_00 = v[0];
                  v_00[1] = v[1];
                  v_00[2] = v[2];
                  MVar11 = MeVector3Normalize(v_00);
                  v_00[3] = MVar11;
                  v_00[6] = (MeReal)qmem.si[0];
                  v_00[7] = (MeReal)qmem.si[1];
                  v_00[8] = (MeReal)qmem.si[2];
                  v_00[9] = (MeReal)qmem.si[3];
                  goto LAB_00010daf;
                }
              }
            }
            else {
                    /* Unresolved local var: McdGjkFace * f@[DW_OP_reg6(ESI)] */
              v_00 = (MeReal *)MePoolxGet(&qmem.fpool);
              if (v_00 != (MeReal *)0x0) {
                v_00[5] = MVar11;
                v_00[4] = MVar10;
                v_00[10] = 0.0;
                *v_00 = v[0];
                v_00[1] = v[1];
                v_00[2] = v[2];
                MVar11 = MeVector3Normalize(v_00);
                v_00[3] = MVar11;
                v_00[6] = (MeReal)qmem.si[0];
                v_00[7] = (MeReal)qmem.si[1];
                v_00[8] = (MeReal)qmem.si[2];
                v_00[9] = (MeReal)qmem.si[3];
LAB_00010daf:
                MeHeapPush(&qmem.face,v_00);
              }
            }
          }
          i = i + 1;
        } while (i < 4);
      }
    }
    if ((min_f != (McdGjkFace *)0x0) && (ok != 0)) {
      q = &qmem;
      c->normal[0] = min_f->v[0] * -1.0;
      c->normal[1] = min_f->v[1] * -1.0;
      c->normal[2] = min_f->v[2] * -1.0;
      min_d = -min_d;
      c->separation = min_d - (c->fat2 + c->fat1);
      pMVar5 = min_f;
      pMVar13 = c;
      pMVar14 = c;
      McdGjkFaceLoad(q,min_f);
      iVar9 = McdGjkComputeVector(c->location,min_f->bits,1,s,q,pMVar5,pMVar13,pMVar14);
      fVar1 = c->separation * 0.5 + c->fat2;
      c->location[0] = fVar1 * c->normal[0] + c->location[0];
      c->location[1] = fVar1 * c->normal[1] + c->location[1];
      c->location[2] = fVar1 * c->normal[2] + c->location[2];
      ok = 0;
      if (iVar9 != 0) {
        ok = McdGjkResultIsReasonable(c,s);
      }
    }
  }
  else {
    ok = 0;
  }
  return ok;
}


/* ==== MeVector3Normalize ==== */

MeReal MeVector3Normalize(MeReal *v)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float local_8;
  
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg16(ST5)]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
  fVar1 = *v;
  fVar2 = v[1];
  fVar3 = v[2];
  local_8 = fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1;
  if (local_8 <= 0.0) {
    *v = 1.0;
    v[1] = 0.0;
    v[2] = 0.0;
    local_8 = 0.0;
  }
  else {
    local_8 = SQRT(local_8);
    fVar4 = 1.0 / local_8;
    *v = fVar1 * fVar4;
    v[1] = fVar2 * fVar4;
    v[2] = fVar4 * fVar3;
  }
  return local_8;
}


