/* ==== McdGjkRandomVector ==== */

void McdGjkRandomVector(MeReal *v)

{
  int iVar1;
  
  iVar1 = rand();
  *v = ((float)iVar1 + (float)iVar1) * 4.656613e-10 + -1.0;
  iVar1 = rand();
  v[1] = ((float)iVar1 + (float)iVar1) * 4.656613e-10 + -1.0;
  iVar1 = rand();
  v[2] = ((float)iVar1 + (float)iVar1) * 4.656613e-10 + -1.0;
  MeVector3Normalize(v);
  return;
}


/* ==== McdCacheInit ==== */

void McdCacheInit(McdCache *c,McdModelPair *p)

{
  McdGeometryInstance *pMVar1;
  float fVar2;
  
  c->padding = p->model2->mPadding + p->model1->mPadding;
  pMVar1 = &p->model1->mInstance;
  c->ins1 = pMVar1;
  c->ins2 = &p->model2->mInstance;
  fVar2 = McdGjkFatness(pMVar1);
  c->fat1 = fVar2;
  fVar2 = McdGjkFatness(c->ins2);
  c->separation = 1.0;
  c->fat2 = fVar2;
  c->normal[0] = 0.0;
  c->normal[1] = 1.0;
  c->normal[2] = 0.0;
  c->location[0] = 0.0;
  c->location[1] = 0.0;
  c->location[2] = 0.0;
  return;
}


/* ==== McdCacheHello ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeBool McdCacheHello(McdModelPair *p)

{
  MePool *pMVar1;
  McdFramework *pMVar2;
  undefined4 *puVar3;
  McdGeometryInstance *pMVar4;
  float fVar5;
  
                    /* Unresolved local var: MePool * pool@[DW_OP_reg3(EBX)] */
  pMVar2 = p->model1->frame;
  pMVar1 = &pMVar2->cachePool;
  if ((pMVar2->cachePool).t == MePoolNULL) {
    (*_MePoolFixedAPI)(pMVar1,100,0x3c,0x10);
  }
  puVar3 = (*_McdGeometryInstanceGetAABB)(pMVar1);
  p->m_cachedData = puVar3;
  if (puVar3 != (undefined4 *)0x0) {
                    /* Unresolved local var: McdCache * c@[???] */
    puVar3[0xb] = p->model2->mPadding + p->model1->mPadding;
    pMVar4 = &p->model1->mInstance;
    puVar3[0xd] = pMVar4;
    puVar3[0xe] = &p->model2->mInstance;
    fVar5 = McdGjkFatness(pMVar4);
    puVar3[9] = fVar5;
    fVar5 = McdGjkFatness((void *)puVar3[0xe]);
    *puVar3 = 0;
    puVar3[0xc] = 0x3f800000;
    puVar3[10] = fVar5;
    puVar3[1] = 0x3f800000;
    puVar3[2] = 0;
    puVar3[3] = 0;
    puVar3[4] = 0;
    puVar3[5] = 0;
  }
  return 1;
}


/* ==== McdCacheGoodbye ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void McdCacheGoodbye(McdModelPair *p)

{
                    /* Unresolved local var: MePool * pool@[DW_OP_reg2(EDX)] */
  if (p->m_cachedData != (void *)0x0) {
    (*_McdGjkPenetrationDepth)(&p->model1->frame->cachePool,p->m_cachedData);
  }
  p->m_cachedData = (void *)0x0;
  return;
}


/* ==== McdGjkNextIsDuplicate ==== */

int McdGjkNextIsDuplicate(McdGjkSimplex *s)

{
  float fVar1;
  int iVar2;
  McdGjkSimplex *pMVar3;
  uint uVar4;
  uint uVar5;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int b@[DW_OP_reg3(EBX)] */
  if (s->inflate == 0) {
    uVar5 = s->bits;
    if (s->next_bit < (int)uVar5) {
      fVar1 = s->lastw[0][0];
      if (1e-06 <= ABS(fVar1)) {
        iVar2 = s->next_i;
        fVar1 = (s->point[iVar2].w[0] - fVar1) / fVar1;
      }
      else {
        iVar2 = s->next_i;
        fVar1 = s->point[iVar2].w[0];
      }
      if (ABS(fVar1) < 1e-06) {
        fVar1 = s->lastw[0][1];
        if (1e-06 <= ABS(fVar1)) {
          fVar1 = (s->point[iVar2].w[1] - fVar1) / fVar1;
        }
        else {
          fVar1 = s->point[iVar2].w[1];
        }
        if (ABS(fVar1) < 1e-06) {
          fVar1 = s->lastw[0][2];
          if (1e-06 <= ABS(fVar1)) {
            fVar1 = (s->point[iVar2].w[2] - fVar1) / fVar1;
          }
          else {
            fVar1 = s->point[iVar2].w[2];
          }
          if (ABS(fVar1) < 1e-06) {
            return 1;
          }
        }
      }
      fVar1 = s->lastw[1][0];
      if (1e-06 <= ABS(fVar1)) {
        fVar1 = (s->point[iVar2].w[0] - fVar1) / fVar1;
      }
      else {
        fVar1 = s->point[iVar2].w[0];
      }
      if (ABS(fVar1) < 1e-06) {
        fVar1 = s->lastw[1][1];
        if (1e-06 <= ABS(fVar1)) {
          fVar1 = (s->point[iVar2].w[1] - fVar1) / fVar1;
        }
        else {
          fVar1 = s->point[iVar2].w[1];
        }
        if (ABS(fVar1) < 1e-06) {
          fVar1 = s->lastw[1][2];
          if (1e-06 <= ABS(fVar1)) {
            fVar1 = (s->point[iVar2].w[2] - fVar1) / fVar1;
          }
          else {
            fVar1 = s->point[iVar2].w[2];
          }
          if (ABS(fVar1) < 1e-06) {
            return 1;
          }
        }
      }
    }
  }
  else {
    uVar5 = s->bits;
  }
  uVar4 = 1;
  pMVar3 = s;
  if (0 < (int)uVar5) {
    do {
      if ((uVar5 & uVar4) != 0) {
        if (1e-06 <= ABS(pMVar3->point[0].w[0])) {
          iVar2 = s->next_i;
          fVar1 = pMVar3->point[0].w[0];
          fVar1 = (s->point[iVar2].w[0] - fVar1) / fVar1;
        }
        else {
          iVar2 = s->next_i;
          fVar1 = s->point[iVar2].w[0];
        }
        if (ABS(fVar1) < 1e-06) {
          if (1e-06 <= ABS(pMVar3->point[0].w[1])) {
            fVar1 = pMVar3->point[0].w[1];
            fVar1 = (s->point[iVar2].w[1] - fVar1) / fVar1;
          }
          else {
            fVar1 = s->point[iVar2].w[1];
          }
          if (ABS(fVar1) < 1e-06) {
            if (1e-06 <= ABS(pMVar3->point[0].w[2])) {
              fVar1 = pMVar3->point[0].w[2];
              fVar1 = (s->point[iVar2].w[2] - fVar1) / fVar1;
            }
            else {
              fVar1 = s->point[iVar2].w[2];
            }
            if (ABS(fVar1) < 1e-06) {
              return 1;
            }
          }
        }
      }
      uVar4 = uVar4 * 2;
      pMVar3 = (McdGjkSimplex *)(pMVar3->point + 1);
    } while ((int)uVar4 <= (int)uVar5);
  }
  return 0;
}


/* ==== McdGjkComputeVector ==== */

int McdGjkComputeVector(MeReal *v,int bits,int sup,McdGjkSimplex *s)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float *pfVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  
                    /* Unresolved local var: MeReal d@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal det@[DW_OP_reg14(ST3)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int b@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal hi@[DW_OP_reg13(ST2)]
                       Unresolved local var: MeReal lo@[DW_OP_reg15(ST4)] */
  uVar6 = 1;
  iVar7 = 0;
  fVar2 = 0.0;
  *v = 0.0;
  v[1] = 0.0;
  v[2] = 0.0;
  fVar3 = fVar2;
  fVar4 = fVar2;
  if (0 < bits) {
    iVar8 = 0;
    do {
      if ((bits & uVar6) != 0) {
        fVar1 = s->delta[bits][iVar7];
        if (fVar1 < fVar4) {
          fVar4 = fVar1;
        }
        if (fVar2 < fVar1) {
          fVar2 = fVar1;
        }
        pfVar5 = (float *)((int)s->point[0].w + iVar8);
        fVar3 = fVar3 + fVar1;
        if (sup != 0) {
          pfVar5 = pfVar5 + 3;
        }
        *v = fVar1 * *pfVar5 + *v;
        v[1] = fVar1 * pfVar5[1] + v[1];
        v[2] = fVar1 * pfVar5[2] + v[2];
      }
      uVar6 = uVar6 * 2;
      iVar8 = iVar8 + 0x18;
      iVar7 = iVar7 + 1;
    } while ((int)uVar6 <= bits);
  }
  if (fVar3 != 0.0) {
    fVar3 = 1.0 / fVar3;
    *v = *v * fVar3;
    v[1] = v[1] * fVar3;
    v[2] = fVar3 * v[2];
  }
  return (uint)(-fVar2 * s->eps <= fVar4);
}


/* ==== McdGjkComputeNextVector ==== */

int McdGjkComputeNextVector(MeReal *v,int bits,int sup,McdGjkSimplex *s)

{
  McdGjkPoint *pMVar1;
  MeReal MVar2;
  int iVar3;
  
  if (bits == 0) {
    if (sup == 0) {
      pMVar1 = s->point + s->next_i;
      *v = pMVar1->w[0];
      v[1] = pMVar1->w[1];
      MVar2 = pMVar1->w[2];
    }
    else {
      iVar3 = s->next_i;
      *v = s->point[iVar3].s2[0];
      v[1] = s->point[iVar3].s2[1];
      MVar2 = s->point[iVar3].s2[2];
    }
    v[2] = MVar2;
    return 1;
  }
  iVar3 = McdGjkComputeVector(v,bits | s->next_bit,sup,s);
  return iVar3;
}


/* ==== McdGjkNextAvailablePoint ==== */

McdGjkPoint * McdGjkNextAvailablePoint(McdGjkSimplex *s)

{
  uint uVar1;
  int iVar2;
  
  s->next_i = 0;
  s->next_bit = 1;
  if ((s->bits & 1U) != 0) {
    uVar1 = 1;
    iVar2 = 0;
    do {
      uVar1 = uVar1 * 2;
      iVar2 = iVar2 + 1;
    } while ((s->bits & uVar1) != 0);
    s->next_i = iVar2;
    s->next_bit = uVar1;
  }
  return s->point + s->next_i;
}


/* ==== McdGjkCrossProd ==== */

MeReal McdGjkCrossProd(MeReal *v,McdGjkSimplex *s)

{
  McdGjkPoint *pMVar1;
  McdGjkPoint *pMVar2;
  McdGjkPoint *pMVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  int iVar7;
  float fVar8;
  float fVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  McdGjkPoint *p3;
  McdGjkPoint *p1;
  MeReal normalized [3];
  MeReal v2 [3];
  MeReal v1 [3];
  
                    /* Unresolved local var: McdGjkPoint * p0@[DW_OP_reg7(EDI)]
                       Unresolved local var: McdGjkPoint * p2@[DW_OP_reg1(ECX)]
                       Unresolved local var: int i@[DW_OP_reg2(EDX)]
                       Unresolved local var: MeReal len@[???]
                       Unresolved local var: float __result@[DW_OP_reg17(ST6)] */
  iVar7 = s->bits;
  if (iVar7 == 0xe) {
    p3 = s->point;
  }
  uVar10 = (uint)(iVar7 == 0xe);
  iVar11 = uVar10 + 1;
  pMVar1 = s->point + uVar10;
  if (iVar7 == 0xd) {
    iVar11 = 2;
    p3 = s->point + 1;
  }
  iVar12 = iVar11 + 1;
  pMVar2 = s->point + iVar11;
  if (iVar7 == 0xb) {
    iVar12 = iVar11 + 2;
    p3 = s->point + iVar11 + 1;
  }
  pMVar3 = s->point + iVar12;
  if (iVar7 == 7) {
    p3 = s->point + iVar12 + 1;
  }
  normalized[0] = pMVar1->w[0] - pMVar2->w[0];
  normalized[1] = pMVar1->w[1] - pMVar2->w[1];
  normalized[2] = pMVar1->w[2] - pMVar2->w[2];
  fVar8 = pMVar3->w[0] - pMVar2->w[0];
  fVar5 = pMVar3->w[1] - pMVar2->w[1];
  fVar6 = pMVar3->w[2] - pMVar2->w[2];
  fVar4 = normalized[1] * fVar6 - normalized[2] * fVar5;
                    /* Unresolved local var: MeReal m@[???] */
  *v = fVar4;
  fVar6 = normalized[2] * fVar8 - fVar6 * normalized[0];
  v[1] = fVar6;
  fVar8 = normalized[0] * fVar5 - fVar8 * normalized[1];
  v[2] = fVar8;
  fVar8 = SQRT(fVar8 * fVar8 + fVar6 * fVar6 + fVar4 * fVar4);
  if (fVar8 == 0.0) {
    MeVector3Normalize(normalized);
    if ((0.5 <= normalized[2]) || (normalized[2] <= -0.5)) {
      *v = -normalized[2];
      v[1] = 0.0;
      v[2] = normalized[0];
    }
    else {
      *v = -normalized[1];
      v[1] = normalized[0];
      v[2] = 0.0;
    }
    MeVector3Normalize(v);
  }
  else {
    fVar4 = *v;
    fVar5 = v[1];
    fVar6 = v[2];
    if ((p3->w[0] - pMVar2->w[0]) * fVar4 + (p3->w[1] - pMVar2->w[1]) * fVar5 +
        (p3->w[2] - pMVar2->w[2]) * fVar6 <= 0.0) {
      fVar9 = 1.0 / fVar8;
      *v = fVar4 * fVar9;
      v[1] = fVar5 * fVar9;
      v[2] = fVar9 * fVar6;
    }
    else {
      fVar9 = -1.0 / fVar8;
      *v = fVar4 * fVar9;
      v[1] = fVar5 * fVar9;
      v[2] = fVar9 * fVar6;
    }
  }
  return fVar8;
}


/* ==== McdGjkAdjustPerpendicular ==== */

void McdGjkAdjustPerpendicular(MeReal *v,MeReal v_len,McdGjkSimplex *s)

{
  McdGjkPoint *pMVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  uint uVar6;
  McdGjkSimplex *pMVar7;
  uint uVar8;
  int iVar9;
  MeReal MVar10;
  int k;
  MeReal normalized [3];
  MeReal v2 [3];
  
                    /* Unresolved local var: MeReal len@[DW_OP_reg13(ST2)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int b@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal t@[???]
                       Unresolved local var: MeReal d@[DW_OP_reg13(ST2)] */
  if ((v_len <= s->eps) && (uVar6 = s->bits, uVar6 != 0)) {
    if ((uVar6 == 7) || (((uVar6 == 0xb || (uVar6 == 0xd)) || (uVar6 == 0xe)))) {
      MVar10 = McdGjkCrossProd(v2,s);
      if (1e-06 < MVar10) {
        if (0.0 <= v[2] * v2[2] + v[1] * v2[1] + *v * v2[0]) {
          fVar5 = 1.0 / MVar10;
        }
        else {
          fVar5 = -1.0 / MVar10;
        }
        *v = v2[0] * fVar5;
        v[1] = v2[1] * fVar5;
        v[2] = v2[2] * fVar5;
        return;
      }
      uVar6 = s->bits;
    }
    uVar8 = 1;
    k = 0;
    fVar5 = 0.0;
    if (0 < (int)uVar6) {
      iVar9 = 0;
      pMVar7 = s;
      do {
                    /* Unresolved local var: MeReal m@[DW_OP_reg12(ST1)] */
        if (((uVar6 & uVar8) != 0) &&
           (fVar2 = pMVar7->point[0].w[0], fVar3 = pMVar7->point[0].w[1],
           fVar4 = pMVar7->point[0].w[2], fVar2 = fVar4 * fVar4 + fVar3 * fVar3 + fVar2 * fVar2,
           fVar5 < fVar2)) {
          fVar5 = fVar2;
          k = iVar9;
        }
        uVar8 = uVar8 * 2;
        pMVar7 = (McdGjkSimplex *)(pMVar7->point + 1);
        iVar9 = iVar9 + 1;
      } while ((int)uVar8 <= (int)uVar6);
    }
    pMVar1 = s->point + k;
    normalized[0] = pMVar1->w[0];
    normalized[1] = pMVar1->w[1];
    normalized[2] = pMVar1->w[2];
    MeVector3Normalize(normalized);
    if ((0.5 <= normalized[2]) || (normalized[2] <= -0.5)) {
      *v = -normalized[2];
      v[1] = 0.0;
      v[2] = normalized[0];
    }
    else {
      *v = -normalized[1];
      v[1] = normalized[0];
      v[2] = 0.0;
    }
    MeVector3Normalize(v);
  }
  return;
}


/* ==== McdGjkComputeSupport ==== */

MeReal McdGjkComputeSupport(McdGjkPoint *p,MeReal *v,int sign,McdCache *c)

{
  float fVar1;
  float fVar2;
  float fVar3;
  MeReal *pMVar4;
  MeReal s1 [3];
  MeReal neg_v [3];
  
  s1[0] = -*v;
  s1[1] = -v[1];
  s1[2] = -v[2];
  neg_v[0] = s1[0];
  neg_v[1] = s1[1];
  neg_v[2] = s1[2];
  if (sign < 1) {
    McdGjkMaximumPoint(c->ins1,neg_v,s1);
    pMVar4 = v;
  }
  else {
    McdGjkMaximumPoint(c->ins1,v,s1);
    pMVar4 = neg_v;
  }
  McdGjkMaximumPoint(c->ins2,pMVar4,p->s2);
  fVar1 = s1[0] - p->s2[0];
  p->w[0] = fVar1;
  fVar3 = s1[1] - p->s2[1];
  p->w[1] = fVar3;
  fVar2 = s1[2] - p->s2[2];
  p->w[2] = fVar2;
  fVar1 = fVar1 + c->offset[0];
  p->w[0] = fVar1;
  fVar3 = fVar3 + c->offset[1];
  p->w[1] = fVar3;
  fVar2 = fVar2 + c->offset[2];
  p->w[2] = fVar2;
  return fVar1 * *v + fVar3 * v[1] + fVar2 * v[2];
}


/* ==== McdGjkFindNextSupportPoint ==== */

MeReal McdGjkFindNextSupportPoint(MeReal *v,MeReal v_len,McdCache *c,McdGjkSimplex *s,int i)

{
  McdGjkPoint *p;
  uint uVar1;
  int iVar2;
  MeReal MVar3;
  McdCache *c_local;
  
                    /* Unresolved local var: McdGjkPoint * p@[???]
                       Unresolved local var: MeReal d@[???] */
  s->next_i = 0;
  s->next_bit = 1;
  if ((s->bits & 1U) != 0) {
    uVar1 = 1;
    iVar2 = 0;
    do {
      uVar1 = uVar1 * 2;
      iVar2 = iVar2 + 1;
    } while ((s->bits & uVar1) != 0);
    s->next_i = iVar2;
    s->next_bit = uVar1;
  }
  uVar1 = i & 1;
  p = s->point + s->next_i;
  s->lastw[uVar1][0] = p->w[0];
  s->lastw[uVar1][1] = p->w[1];
  s->lastw[uVar1][2] = p->w[2];
  McdGjkAdjustPerpendicular(v,v_len,s);
  MVar3 = McdGjkComputeSupport(p,v,-1,c);
  return MVar3;
}


/* ==== McdGjkUpdateDotCache ==== */

void McdGjkUpdateDotCache(McdGjkSimplex *s)

{
  McdGjkPoint *pMVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  float fVar5;
  McdGjkSimplex *pMVar6;
  uint uVar7;
  int i;
  
                    /* Unresolved local var: int b@[DW_OP_reg6(ESI)] */
  uVar7 = 1;
  i = 0;
  uVar2 = s->bits;
  uVar3 = s->next_bit;
  pMVar6 = s;
  if (0 < (int)(uVar2 | uVar3)) {
    do {
      if (((uVar2 | uVar3) & uVar7) != 0) {
        iVar4 = s->next_i;
        pMVar1 = s->point + iVar4;
        fVar5 = pMVar1->w[2] * pMVar6->point[0].w[2] +
                pMVar1->w[1] * pMVar6->point[0].w[1] + pMVar1->w[0] * pMVar6->point[0].w[0];
        s->dot[i][iVar4] = fVar5;
        s->dot[iVar4][i] = fVar5;
      }
      i = i + 1;
      uVar7 = uVar7 * 2;
      pMVar6 = (McdGjkSimplex *)(pMVar6->point + 1);
    } while ((int)uVar7 <= (int)(uVar2 | uVar3));
  }
  return;
}


/* ==== McdGjkUpdateDeltaCache ==== */

void McdGjkUpdateDeltaCache(McdGjkSimplex *s)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  MeReal (*paMVar4) [4];
  uint uVar5;
  uint uVar6;
  MeReal (*local_64) [4];
  int local_60;
  int local_5c;
  int local_58;
  MeReal (*delta) [4];
  MeReal (*dot) [4];
  int bj;
  int bi;
  int k;
  int j;
  int i;
  
                    /* Unresolved local var: int bk@[DW_OP_reg7(EDI)] */
  paMVar4 = s->dot;
  iVar1 = s->next_i;
  uVar2 = s->next_bit;
  s->delta[uVar2][iVar1] = 1.0;
  bj = 1;
  j = 0;
  uVar5 = s->bits;
  if (0 < (int)uVar5) {
    local_5c = 0;
    local_60 = 0;
    local_64 = paMVar4;
    do {
      if ((bj & uVar5) != 0) {
        uVar3 = uVar2 | bj;
        s->delta[uVar3][iVar1] = (*local_64)[0] - (*paMVar4)[local_60 + iVar1];
        s->delta[uVar3][j] = *(float *)((int)paMVar4 + iVar1 * 0x14) - paMVar4[iVar1][j];
        uVar6 = 1;
        k = 0;
        uVar5 = s->bits;
        if (0 < (int)uVar5) {
          local_58 = 0;
          do {
            if ((uVar5 & uVar6) != 0) {
              if (bj <= (int)uVar6) break;
              uVar5 = uVar3 | uVar6;
              s->delta[uVar5][iVar1] =
                   (*(float *)((int)*paMVar4 + local_58) - paMVar4[k][iVar1]) *
                   s->delta[bj | uVar6][k] +
                   ((*paMVar4)[local_60 + k] - (*paMVar4)[local_60 + iVar1]) *
                   s->delta[bj | uVar6][j];
              s->delta[uVar5][j] =
                   (paMVar4[iVar1][k] - paMVar4[iVar1][j]) * s->delta[uVar2 | uVar6][iVar1] +
                   (*(float *)((int)*paMVar4 + local_58) - paMVar4[k][j]) *
                   s->delta[uVar2 | uVar6][k];
              s->delta[uVar5][k] =
                   (*(float *)((int)*paMVar4 + local_5c) - (*paMVar4)[local_60 + k]) *
                   s->delta[uVar3][j] +
                   (paMVar4[iVar1][j] - paMVar4[iVar1][k]) * s->delta[uVar3][iVar1];
              uVar5 = s->bits;
            }
            local_58 = local_58 + 0x14;
            k = k + 1;
            uVar6 = uVar6 * 2;
          } while ((int)uVar6 <= (int)uVar5);
        }
      }
      bj = bj * 2;
      local_60 = local_60 + 4;
      local_5c = local_5c + 0x14;
      local_64 = (MeReal (*) [4])(local_64[1] + 1);
      j = j + 1;
    } while (bj <= (int)uVar5);
  }
  if (((uVar2 | uVar5) == 0xf) && (s->inflate == 0)) {
    s->delta[0xf][0] =
         (s->dot[3][1] - s->dot[3][0]) * s->delta[0xe][3] +
         (s->dot[2][1] - s->dot[2][0]) * s->delta[0xe][2] +
         (s->dot[1][1] - s->dot[1][0]) * s->delta[0xe][1];
    s->delta[0xf][1] =
         (s->dot[3][0] - s->dot[3][1]) * s->delta[0xd][3] +
         (s->dot[2][0] - s->dot[2][1]) * s->delta[0xd][2] +
         ((*paMVar4)[0] - s->dot[0][1]) * s->delta[0xd][0];
    s->delta[0xf][2] =
         (s->dot[3][0] - s->dot[3][2]) * s->delta[0xb][3] +
         (s->dot[1][0] - s->dot[1][2]) * s->delta[0xb][1] +
         ((*paMVar4)[0] - s->dot[0][2]) * s->delta[0xb][0];
    s->delta[0xf][3] =
         (s->dot[2][0] - s->dot[2][3]) * s->delta[7][2] +
         (s->dot[1][0] - s->dot[1][3]) * s->delta[7][1] +
         ((*paMVar4)[0] - s->dot[0][3]) * s->delta[7][0];
  }
  return;
}


/* ==== McdGjkMain ==== */

int McdGjkMain(McdCache *c,McdGjkSimplex *s)

{
  McdGjkPoint *pMVar1;
  uint uVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  int iVar6;
  McdGjkSimplex *pMVar7;
  uint uVar8;
  uint uVar9;
  MeReal MVar10;
  int v_bits;
  MeReal v_len;
  MeReal fatness;
  MeReal maxd;
  MeReal d;
  int counter;
  MeReal u [3];
  MeReal v [3];
  
                    /* Unresolved local var: int ok@[???]
                       Unresolved local var: int is_dup@[DW_OP_reg1(ECX)]
                       Unresolved local var: int dup_counter@[???] */
  counter = 1;
  maxd = 1e-06;
  fVar3 = c->fat2 + c->fat1;
  v[0] = 0.5;
  v[1] = 0.6;
  v[2] = 0.6245;
  v_len = 1.0;
  s->bits = 0;
  s->inflate = 0;
  fVar4 = c->ins1->mGeometry->frame->mScale * 0.0001;
  s->eps = fVar4;
  if (fVar4 < 1.0) {
    do {
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int bits@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeReal u_len@[DW_OP_reg11(ST0)] */
      MVar10 = McdGjkFindNextSupportPoint(v,v_len,c,s,counter);
      c->separation = MVar10 - fVar3;
      if (maxd < MVar10) {
        maxd = MVar10;
      }
      counter = counter + 1;
      if (0x32 < counter) {
        c->separation = 1.0;
        return 2;
      }
      iVar5 = McdGjkNextIsDuplicate(s);
      if ((c->padding < c->separation) || (v_len < maxd + s->eps)) {
LAB_000113eb:
        c->normal[0] = v[0];
        c->normal[1] = v[1];
        c->normal[2] = v[2];
        MeVector3Normalize(c->normal);
        McdGjkComputeVector(c->location,s->bits,1,s);
        fVar3 = c->separation * 0.5 + c->fat2;
        c->location[0] = fVar3 * c->normal[0] + c->location[0];
        c->location[1] = fVar3 * c->normal[1] + c->location[1];
        c->location[2] = fVar3 * c->normal[2] + c->location[2];
        return (uint)(MVar10 <= 0.0);
      }
      if (iVar5 != 0) {
        if (v_len < s->eps * 100.0 + fVar3) goto LAB_000113eb;
        if (iVar5 != 0) {
          return 2;
        }
      }
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int b@[DW_OP_reg6(ESI)] */
      iVar5 = 0;
      uVar8 = s->bits;
      uVar2 = s->next_bit;
      if (0 < (int)(uVar8 | uVar2)) {
        uVar9 = 1;
        pMVar7 = s;
        do {
          if (((uVar8 | uVar2) & uVar9) != 0) {
            iVar6 = s->next_i;
            pMVar1 = s->point + iVar6;
            fVar4 = pMVar1->w[2] * pMVar7->point[0].w[2] +
                    pMVar1->w[1] * pMVar7->point[0].w[1] + pMVar1->w[0] * pMVar7->point[0].w[0];
            s->dot[iVar5][iVar6] = fVar4;
            s->dot[iVar6][iVar5] = fVar4;
          }
          uVar9 = uVar9 * 2;
          pMVar7 = (McdGjkSimplex *)(pMVar7->point + 1);
          iVar5 = iVar5 + 1;
        } while ((int)uVar9 <= (int)(uVar8 | uVar2));
      }
      iVar5 = 0;
      McdGjkUpdateDeltaCache(s);
      v_bits = 0;
      do {
        uVar8 = McdGjkBinarySubset[s->bits][iVar5];
        if (((iVar5 != 0) || (maxd <= 1e-06)) || ((uVar8 | s->next_bit) != 0xf)) {
          iVar6 = McdGjkComputeNextVector(u,uVar8,0,s);
          if ((iVar6 != 0) && ((MVar10 = MeVector3Normalize(u), v_bits == 0 || (MVar10 <= v_len))))
          {
            v[0] = u[0];
            v[1] = u[1];
            v[2] = u[2];
            v_bits = uVar8;
            v_len = MVar10;
          }
        }
        iVar5 = iVar5 + 1;
      } while (uVar8 != 0);
      uVar8 = v_bits | s->next_bit;
      s->bits = uVar8;
    } while ((uVar8 != 0xf) && (s->eps < v_len));
  }
  return 1;
}


/* ==== McdGjkTest ==== */

int McdGjkTest(McdCache *c,McdGjkSimplex *s)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  bool bVar6;
  int iVar7;
  int iVar8;
  MeReal originalPadding;
  MeReal r2;
  MeReal r1;
  MeReal axisLen;
  MeReal diff [3];
  MeReal maxv [3];
  MeReal minv [3];
  MeReal c2 [3];
  MeReal c1 [3];
  MeReal axis [3];
  
                    /* Unresolved local var: MeReal offsetLen@[DW_OP_reg15(ST4)]
                       Unresolved local var: int rc@[DW_OP_reg2(EDX)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int i@[???] */
  iVar8 = 0;
  fVar1 = c->padding;
  fVar3 = NAN;
  c->offset[0] = 0.0;
  c->offset[1] = 0.0;
  c->offset[2] = 0.0;
  do {
    iVar7 = McdGjkMain(c,s);
    if (iVar7 == 1) {
      bVar6 = false;
      if ((((s->bits == 0xf) && (1e-06 < s->delta[0xf][0])) && (1e-06 < s->delta[0xf][1])) &&
         ((1e-06 < s->delta[0xf][2] && (1e-06 < s->delta[0xf][3])))) {
        bVar6 = true;
      }
      if (bVar6) {
        iVar7 = McdGjkPenetrationDepth(c,s);
        iVar7 = 1 - iVar7;
      }
    }
    if (iVar7 == 0) {
      if (iVar8 != 0) {
                    /* Unresolved local var: MeReal dot@[???] */
        fVar2 = c->normal[1];
        fVar5 = axis[2] * c->normal[2] + fVar2 * axis[1] + c->normal[0] * axis[0];
        fVar4 = -fVar5 * fVar3 * 0.5;
        c->location[0] = c->normal[0] * fVar4 + c->location[0];
        c->location[1] = fVar2 * fVar4 + c->location[1];
        c->location[2] = fVar4 * c->normal[2] + c->location[2];
        c->separation = c->separation - fVar5 * fVar3;
        c->padding = fVar1;
      }
      return 1;
    }
    if (iVar8 == 0) {
      McdGeometryInstanceUpdateAABB(c->ins1,(void *)0x0,0);
      McdGeometryInstanceGetAABB(c->ins1,minv,maxv);
                    /* Unresolved local var: float __result@[???] */
      fVar3 = minv[1] + maxv[1];
      fVar2 = minv[2] + maxv[2];
      fVar4 = minv[0] + maxv[0];
      r1 = SQRT((minv[2] - maxv[2]) * (minv[2] - maxv[2]) +
                (minv[1] - maxv[1]) * (minv[1] - maxv[1]) +
                (minv[0] - maxv[0]) * (minv[0] - maxv[0])) * 0.5;
      McdGeometryInstanceUpdateAABB(c->ins2,(void *)0x0,0);
      McdGeometryInstanceGetAABB(c->ins2,minv,maxv);
                    /* Unresolved local var: float __result@[???] */
      c2[2] = (minv[2] + maxv[2]) * 0.5;
      c2[0] = (minv[0] + maxv[0]) * 0.5;
      c2[1] = (minv[1] + maxv[1]) * 0.5;
      r2 = SQRT((minv[2] - maxv[2]) * (minv[2] - maxv[2]) +
                (minv[1] - maxv[1]) * (minv[1] - maxv[1]) +
                (minv[0] - maxv[0]) * (minv[0] - maxv[0])) * 0.5;
      axis[0] = fVar4 * 0.5 - c2[0];
      axis[1] = fVar3 * 0.5 - c2[1];
      axis[2] = fVar2 * 0.5 - c2[2];
      axisLen = MeVector3Normalize(axis);
      if (axisLen != 0.0) goto LAB_00011521;
      fVar2 = r1 + r2;
      axisLen = fVar2;
    }
    else {
LAB_00011521:
      fVar2 = r1 + r2;
    }
    fVar3 = fVar2;
    if (iVar8 != 3) {
      fVar3 = (fVar2 - axisLen) * ((float)iVar8 + 1.0) * 0.25;
    }
    c->offset[0] = axis[0] * fVar3;
    c->offset[1] = axis[1] * fVar3;
    c->offset[2] = axis[2] * fVar3;
    c->padding = fVar1;
    if (0.0 < fVar3) {
      c->padding = fVar1 + fVar3;
    }
    iVar8 = iVar8 + 1;
    if (4 < iVar8) {
      c->separation = 0.0;
      c->normal[0] = axis[0];
      c->normal[1] = axis[1];
      c->padding = fVar1;
      c->normal[2] = axis[2];
      fVar2 = r2 / fVar2;
      c->location[0] = axis[0] * fVar2 + c2[0];
      c->location[1] = axis[1] * fVar2 + c2[1];
      c->location[2] = fVar2 * axis[2] + c2[2];
      return 0;
    }
  } while( true );
}


/* ==== McdWitnessPlaneTest ==== */

int McdWitnessPlaneTest(McdModelPair *p,McdIntersectResult *result)

{
  MeReal *c;
  float fVar1;
  bool bVar2;
  MeReal MVar3;
  McdGjkPoint point;
  
                    /* Unresolved local var: McdCache * c@[DW_OP_reg3(EBX)] */
  c = p->m_cachedData;
  MeVector3Normalize(c);
                    /* Unresolved local var: int i@[???] */
  c[6] = 0.0;
  c[7] = 0.0;
  c[8] = 0.0;
  MVar3 = McdGjkComputeSupport(&point,c,-1,(McdCache *)c);
  c[0xc] = MVar3;
  bVar2 = MVar3 <= c[9] + c[0xb] + c[10];
  if (!bVar2) {
    c[3] = point.s2[0];
    c[4] = point.s2[1];
    c[5] = point.s2[2];
    fVar1 = c[0xc] * 0.5 + c[10];
    c[3] = fVar1 * *c + c[3];
    c[4] = fVar1 * c[1] + c[4];
    c[5] = fVar1 * c[2] + c[5];
    result->touch = 0;
  }
  return (uint)bVar2;
}


/* ==== McdContactGen ==== */

void McdContactGen(McdCache *c,McdIntersectResult *result)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  McdContact *pMVar7;
  float fVar8;
  float fVar9;
  MeReal *pMVar10;
  MeReal *pMVar11;
  MeReal (*paMVar12) [3];
  MeReal (*unaff_ESI) [3];
  int iVar13;
  int iVar14;
  int j0;
  int i;
  MeReal d;
  MeReal dist;
  int numIntersect;
  int size2;
  int size1;
  MeReal vspace [100] [3];
  
                    /* Unresolved local var: MeReal eps@[???]
                       Unresolved local var: int j@[DW_OP_reg7(EDI)]
                       Unresolved local var: int k@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeVector3 * intersect@[DW_OP_reg6(ESI)] */
  fVar1 = c->location[2] * c->normal[2] +
          c->location[1] * c->normal[1] + c->location[0] * c->normal[0];
  d = ABS(c->separation * 0.5);
  fVar8 = c->ins1->mGeometry->frame->mScale * 0.001 + d;
  numIntersect = 0;
  j0 = 1;
  i = 1;
  do {
    McdGeometryInstanceGetSlice(c->ins1,c,(float)i * fVar8 + fVar1 + c->fat1,0x21,&size1,vspace);
    iVar13 = j0;
    if (size1 != 0) {
      while ((-2 < iVar13 && (numIntersect == 0))) {
        McdGeometryInstanceGetSlice
                  (c->ins2,c,(fVar1 - c->fat2) - (float)iVar13 * fVar8,0x32 - size1,&size2,
                   vspace + size1);
        if (size2 == 0) {
          if (j0 == iVar13) {
            j0 = j0 + -1;
          }
        }
        else {
          iVar14 = 0;
          if (0 < size2 + size1) {
            paMVar12 = vspace;
            fVar2 = c->normal[0];
            fVar3 = c->normal[1];
            fVar4 = c->normal[2];
            do {
              fVar5 = (*paMVar12)[1];
              fVar6 = (*paMVar12)[2];
              iVar14 = iVar14 + 1;
              fVar9 = fVar1 - (fVar6 * fVar4 + fVar5 * fVar3 + (*paMVar12)[0] * fVar2);
              (*paMVar12)[0] = fVar9 * fVar2 + (*paMVar12)[0];
              (*paMVar12)[2] = fVar6 + fVar9 * fVar4;
              (*paMVar12)[1] = fVar9 * fVar3 + fVar5;
              paMVar12 = paMVar12 + 1;
            } while (iVar14 < size2 + size1);
          }
          unaff_ESI = vspace;
          if (size1 != 1) {
            if (size2 != 1) {
              unaff_ESI = vspace + size2 + size1;
              McdPolygonIntersection
                        (c,fVar1,size1,vspace,size2,vspace + size1,&numIntersect,unaff_ESI);
              goto LAB_00011af4;
            }
            unaff_ESI = vspace + size1;
          }
          numIntersect = 1;
        }
LAB_00011af4:
        iVar13 = iVar13 + -1;
      }
    }
    i = i + -1;
    if (i < -1) {
      fVar1 = c->separation;
      goto LAB_00011b45;
    }
    if (j0 < -1) {
      fVar1 = c->separation;
      goto LAB_00011b45;
    }
  } while (numIntersect == 0);
  fVar1 = c->separation;
LAB_00011b45:
  if (0.0 < fVar1) {
    c->separation = 0.0;
  }
  i = 0;
  if ((0 < numIntersect) && (0 < result->contactMaxCount)) {
    iVar13 = 0;
    do {
      pMVar7 = result->contacts;
      *(MeReal *)((int)pMVar7->normal + iVar13) = c->normal[0];
      *(MeReal *)((int)pMVar7->normal + iVar13 + 4) = c->normal[1];
      *(MeReal *)((int)pMVar7->normal + iVar13 + 8) = c->normal[2];
      pMVar11 = (MeReal *)((int)result->contacts->position + iVar13);
      *pMVar11 = (*unaff_ESI)[0];
      pMVar11[1] = (*unaff_ESI)[1];
      pMVar10 = *unaff_ESI;
      unaff_ESI = unaff_ESI + 1;
      pMVar11[2] = pMVar10[2];
      *(MeReal *)((int)result->contacts->normal + iVar13 + 0xc) = c->separation;
      iVar13 = iVar13 + 0x28;
      i = i + 1;
      if (numIntersect <= i) break;
    } while (i < result->contactMaxCount);
  }
  result->contactCount = i;
  return;
}


/* ==== McdGjkCgIntersect2 ==== */

int McdGjkCgIntersect2(McdModelPair *p,McdIntersectResult *result)

{
  float fVar1;
  float fVar2;
  McdContact *pMVar3;
  int iVar4;
  McdCache *c;
  McdGjkSimplex *pMVar5;
  McdModelID_conflict pMVar6;
  McdModelID_conflict pMVar7;
  McdGjkSimplex s;
  McdCache cmem;
  
  c = p->m_cachedData;
  pMVar5 = &s;
                    /* Unresolved local var: McdCache * c@[DW_OP_reg6(ESI)]
                       Unresolved local var: int ok@[???] */
  for (iVar4 = 0x73; iVar4 != 0; iVar4 = iVar4 + -1) {
    pMVar5->point[0].w[0] = 0.0;
    pMVar5 = (McdGjkSimplex *)(pMVar5->point[0].w + 1);
  }
  pMVar7 = p->model1;
  if ((char)((pMVar7->mInstance).mGeometry)->mRefCtAndID == '\x03') {
    pMVar6 = pMVar7;
    pMVar7 = p->model2;
  }
  else {
    pMVar6 = p->model2;
    if ((char)((pMVar6->mInstance).mGeometry)->mRefCtAndID != '\x03') {
      if (c == (McdCache *)0x0) {
        cmem.ins2 = &pMVar6->mInstance;
        cmem.padding = pMVar6->mPadding + pMVar7->mPadding;
        cmem.ins1 = &pMVar7->mInstance;
        c = &cmem;
        cmem.fat1 = McdGjkFatness(cmem.ins1);
        cmem.fat2 = McdGjkFatness(cmem.ins2);
        cmem.separation = 1.0;
        cmem.normal[0] = 0.0;
        cmem.normal[1] = 1.0;
        cmem.normal[2] = 0.0;
        cmem.location[0] = 0.0;
        cmem.location[1] = 0.0;
        cmem.location[2] = 0.0;
      }
      else {
        iVar4 = McdWitnessPlaneTest(p,result);
        if (iVar4 == 0) {
          return 0;
        }
      }
      McdGjkTest(c,&s);
      fVar1 = c->separation;
      fVar2 = c->padding;
      result->contactCount = 0;
      result->touch = (uint)(fVar1 <= fVar2);
      result->normal[0] = c->normal[0];
      result->normal[1] = c->normal[1];
      result->normal[2] = c->normal[2];
      if (result->touch == 0) {
        return 0;
      }
      if (result->contactMaxCount < 1) {
        return result->touch;
      }
      McdContactGen(c,result);
      if (result->contactCount == 0) {
        result->contactCount = 1;
        pMVar3 = result->contacts;
        pMVar3->normal[0] = c->normal[0];
        pMVar3->normal[1] = c->normal[1];
        pMVar3->normal[2] = c->normal[2];
        pMVar3 = result->contacts;
        pMVar3->position[0] = c->location[0];
        pMVar3->position[1] = c->location[1];
        pMVar3->position[2] = c->location[2];
        result->contacts->separation = 0.0;
        result->contacts->dims = 0;
      }
      return result->touch;
    }
  }
  iVar4 = McdPlaneIntersectTest(pMVar6,pMVar7,result);
  return iVar4;
}


/* ==== McdGjkCgIntersect ==== */

int McdGjkCgIntersect(McdModelPair *p,McdIntersectResult *result)

{
  int iVar1;
  
                    /* Unresolved local var: int t@[???] */
  iVar1 = McdGjkCgIntersect2(p,result);
  return iVar1;
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


