/* ==== KaFileCreate_1_0 ==== */

MeFAsset * KaFileCreate_1_0(MeAssetDB *parent,MeIDPool *IDPool,PElement *e)

{
  PElement *pPVar1;
  void *pvVar2;
  MeFAsset *parent_00;
  int iVar3;
  void *pvVar4;
  MeFJoint *pMVar5;
  MeFAssetPart *pMVar6;
  MeFModel *pMVar7;
  MeFGeometry *pMVar8;
  undefined4 extraout_ECX;
  undefined4 extraout_EDX;
  PElementNode *pPVar9;
  longdouble lVar10;
  MeFAsset *asset;
  
                    /* Unresolved local var: char * name@[DW_OP_reg3(EBX)]
                       Unresolved local var: char * graphic@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * scale@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * refPart@[DW_OP_reg0(EAX)]
                       Unresolved local var: int id@[DW_OP_reg0(EAX)]
                       Unresolved local var: PElementNode * node@[DW_OP_reg7(EDI)]
                       Unresolved local var: PElement * childElem@[DW_OP_reg6(ESI)] */
  pPVar9 = e->childHead;
  pvVar2 = PElementGetAttributeValue(e,&DAT_000116a4);
  parent_00 = (MeFAsset *)0x0;
  if (pvVar2 != (void *)0x0) {
    if (IDPool == (MeIDPool *)0x0) {
      iVar3 = 0;
    }
    else {
      iVar3 = MeIDPoolRequestID(IDPool);
    }
    parent_00 = MeFAssetCreate(pvVar2,iVar3);
    for (; pPVar9 != (PElementNode *)0x0; pPVar9 = pPVar9->next) {
                    /* Unresolved local var: MeFGeometry * geom@[DW_OP_reg0(EAX)] */
      if ((pPVar9->current->type == 1) &&
         (pMVar8 = MeFGeometryCreateFromFile_1_0(parent_00,pPVar9->current),
         pMVar8 != (MeFGeometry *)0x0)) {
        MeFAssetInsertGeometry(parent_00,pMVar8);
      }
    }
    pPVar9 = e->childHead;
    if (pPVar9 != (PElementNode *)0x0) {
      do {
                    /* Unresolved local var: MeFModel * model@[DW_OP_reg0(EAX)] */
        if ((pPVar9->current->type == 3) &&
           (pMVar7 = MeFModelCreateFromFile_1_0(parent_00,pPVar9->current),
           pMVar7 != (MeFModel *)0x0)) {
          MeFAssetInsertModel(parent_00,pMVar7);
        }
        pPVar9 = pPVar9->next;
      } while (pPVar9 != (PElementNode *)0x0);
      pPVar9 = e->childHead;
      if (pPVar9 != (PElementNode *)0x0) {
        do {
                    /* Unresolved local var: MeFAssetPart * part@[DW_OP_reg0(EAX)] */
          if ((pPVar9->current->type == 7) &&
             (pMVar6 = MeFAssetPartCreateFromFile_1_0(parent_00,pPVar9->current),
             pMVar6 != (MeFAssetPart *)0x0)) {
            MeFAssetInsertPart(parent_00,pMVar6);
          }
          pPVar9 = pPVar9->next;
        } while (pPVar9 != (PElementNode *)0x0);
        pPVar9 = e->childHead;
        if (pPVar9 != (PElementNode *)0x0) {
          do {
                    /* Unresolved local var: MeFJoint * joint@[DW_OP_reg0(EAX)] */
            if ((pPVar9->current->type == 5) &&
               (pMVar5 = MeFJointCreateFromFile_1_0(parent_00,pPVar9->current),
               pMVar5 != (MeFJoint *)0x0)) {
              MeFAssetInsertJoint(parent_00,pMVar5);
            }
            pPVar9 = pPVar9->next;
          } while (pPVar9 != (PElementNode *)0x0);
          for (pPVar9 = e->childHead; pPVar9 != (PElementNode *)0x0; pPVar9 = pPVar9->next) {
            pPVar1 = pPVar9->current;
            if (pPVar1->type == 8) {
                    /* Unresolved local var: char * p1@[DW_OP_reg3(EBX)]
                       Unresolved local var: char * p2@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeFAssetPart * part1@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeFAssetPart * part2@[DW_OP_reg0(EAX)] */
              pvVar2 = PElementGetAttributeValue(pPVar1,"part1");
              pvVar4 = PElementGetAttributeValue(pPVar1,"part2");
              pvVar2 = MeFAssetLookupPart(parent_00,pvVar2);
              pvVar4 = MeFAssetLookupPart(parent_00,pvVar4);
              if ((pvVar2 != (void *)0x0) && (pvVar4 != (void *)0x0)) {
                MeFAssetPartEnableCollision(pvVar2,pvVar4,0);
              }
            }
          }
        }
      }
    }
    pvVar2 = PElementGetAttributeValue(e,"graphic");
    if (pvVar2 != (void *)0x0) {
      MeFAssetSetGraphicHint(parent_00,pvVar2);
    }
    pvVar2 = PElementGetAttributeValue(e,"scale");
    if (pvVar2 != (void *)0x0) {
                    /* Unresolved local var: MeReal s@[DW_OP_reg0(EAX)] */
      lVar10 = (longdouble)__strtod_internal(pvVar2,0,0,extraout_EDX);
      MeFAssetSetGraphicScale(parent_00,(float)lVar10);
    }
    pvVar2 = PElementGetAttributeValue(e,"ref_part");
    if (pvVar2 != (void *)0x0) {
      MeFAssetSetReferencePart(parent_00,pvVar2);
    }
                    /* Unresolved local var: char * massScale@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * lengthScale@[DW_OP_reg0(EAX)] */
    pvVar2 = PElementGetAttributeValue(e,"mass_scale");
    if (pvVar2 != (void *)0x0) {
                    /* Unresolved local var: MeReal s@[DW_OP_reg0(EAX)] */
      lVar10 = (longdouble)__strtod_internal(pvVar2,0,0,extraout_ECX);
      MeFAssetSetMassScale(parent_00,(float)lVar10);
    }
    pvVar2 = PElementGetAttributeValue(e,"length_scale");
    if (pvVar2 != (void *)0x0) {
                    /* Unresolved local var: MeReal s@[DW_OP_reg0(EAX)] */
      lVar10 = (longdouble)__strtod_internal(pvVar2,0,0,e);
      MeFAssetSetLengthScale(parent_00,(float)lVar10);
    }
  }
  return parent_00;
}


/* ==== MeFGeometryCreateFromFile_1_0 ==== */

MeFGeometry * MeFGeometryCreateFromFile_1_0(MeFAsset *parent,PElement *e)

{
  PElementNode *pPVar1;
  void *pvVar2;
  MeFGeometry *parent_00;
  MeFPrimitive *pMVar3;
  undefined4 extraout_ECX;
  char *extraout_EDX;
  char *extraout_EDX_00;
  char *pcVar4;
  longdouble lVar5;
  int i;
  char *c;
  char *d;
  MeVector3 off;
  
                    /* Unresolved local var: MeFGeometry * fg@[DW_OP_reg7(EDI)]
                       Unresolved local var: char * name@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * graphic@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * scale@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * offset@[DW_OP_reg0(EAX)]
                       Unresolved local var: PElementNode * node@[DW_OP_reg3(EBX)]
                       Unresolved local var: PElement * childElem@[DW_OP_reg0(EAX)] */
  pPVar1 = e->childHead;
  pvVar2 = PElementGetAttributeValue(e,&DAT_000116a4);
  parent_00 = MeFGeometryCreate(pvVar2);
  for (; pPVar1 != (PElementNode *)0x0; pPVar1 = pPVar1->next) {
                    /* Unresolved local var: MeFPrimitive * prim@[DW_OP_reg0(EAX)] */
    if ((pPVar1->current->type == 2) &&
       (pMVar3 = MeFPrimitiveCreateFromFile_1_0(parent_00,pPVar1->current),
       pMVar3 != (MeFPrimitive *)0x0)) {
      MeFGeometryInsertPrimitive(parent_00,pMVar3);
    }
  }
  pvVar2 = PElementGetAttributeValue(e,"graphic");
  if (pvVar2 != (void *)0x0) {
    MeFGeometrySetGraphicHint(parent_00,pvVar2);
  }
  pvVar2 = PElementGetAttributeValue(e,"scale");
  if (pvVar2 != (void *)0x0) {
                    /* Unresolved local var: MeReal s@[DW_OP_reg0(EAX)] */
    lVar5 = (longdouble)__strtod_internal(pvVar2,0,0,extraout_ECX);
    MeFGeometrySetGraphicScale(parent_00,(float)lVar5);
  }
  c = PElementGetAttributeValue(e,"graphic_offset");
  if (c != (char *)0x0) {
    if (*c != '\0') {
      lVar5 = (longdouble)__strtod_internal(c,&d,0,0);
      off[0] = (MeReal)lVar5;
      pcVar4 = d;
      if (c != d) {
        MeXMLParseComma(d,&c);
        pcVar4 = extraout_EDX;
      }
      if (*c != '\0') {
        lVar5 = (longdouble)__strtod_internal(c,&d,0,pcVar4);
        off[1] = (MeReal)lVar5;
        pcVar4 = d;
        if (c != d) {
          MeXMLParseComma(d,&c);
          pcVar4 = extraout_EDX_00;
        }
        if (*c != '\0') {
          lVar5 = (longdouble)__strtod_internal(c,&d,0,pcVar4);
          off[2] = (MeReal)lVar5;
          if (c != d) {
            MeXMLParseComma(d,&c);
          }
        }
      }
    }
    MeFGeometrySetGraphicOffset(parent_00,off[0],off[1],off[2]);
  }
  return parent_00;
}


/* ==== MeFPrimitiveCreateFromFile_1_0 ==== */

/* WARNING: Variable defined which should be unmapped: p */

MeFPrimitive * MeFPrimitiveCreateFromFile_1_0(MeFGeometry *parent,PElement *e)

{
  float *pfVar1;
  void *pvVar2;
  char *pcVar3;
  int iVar4;
  int iVar5;
  char *pcVar6;
  char *pcVar7;
  bool bVar8;
  char *name;
  PPrimitive *source;
  MeFPrimitive *p;
  
                    /* Unresolved local var: char * type@[DW_OP_reg3(EBX)] */
  p = (MeFPrimitive *)0x0;
  pfVar1 = e->cdata;
  pvVar2 = PElementGetAttributeValue(e,&DAT_000116a4);
  pcVar3 = PElementGetAttributeValue(e,&DAT_000116f1);
  if (pcVar3 == (char *)0x0) {
    return (MeFPrimitive *)0x0;
  }
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar4 = 7;
  bVar8 = false;
  pcVar6 = pcVar3;
  pcVar7 = "sphere";
  do {
    if (iVar4 == 0) break;
    iVar4 = iVar4 + -1;
    bVar8 = *pcVar6 == *pcVar7;
    pcVar6 = pcVar6 + 1;
    pcVar7 = pcVar7 + 1;
  } while (bVar8);
  if (bVar8) {
    p = MeFPrimitiveCreate(pvVar2,1);
    MeFPrimitiveSetRadius(p,*pfVar1);
  }
  else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???]
                       Unresolved local var: uchar * __s1@[???]
                       Unresolved local var: int __result@[???] */
    bVar8 = *pcVar3 == 'b';
    if ((((bVar8) && (bVar8 = pcVar3[1] == 'o', bVar8)) && (bVar8 = pcVar3[2] == 'x', bVar8)) &&
       (bVar8 = pcVar3[3] == '\0', bVar8)) {
      p = MeFPrimitiveCreate(pvVar2,2);
      MeFPrimitiveSetDimensions(p,*pfVar1,pfVar1[1],pfVar1[2]);
    }
    else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
      iVar4 = 9;
      pcVar6 = pcVar3;
      pcVar7 = "cylinder";
      do {
        if (iVar4 == 0) break;
        iVar4 = iVar4 + -1;
        bVar8 = *pcVar6 == *pcVar7;
        pcVar6 = pcVar6 + 1;
        pcVar7 = pcVar7 + 1;
      } while (bVar8);
      if (bVar8) {
        iVar4 = 3;
      }
      else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
        iVar4 = 6;
        bVar8 = false;
        pcVar6 = pcVar3;
        pcVar7 = "sphyl";
        do {
          if (iVar4 == 0) break;
          iVar4 = iVar4 + -1;
          bVar8 = *pcVar6 == *pcVar7;
          pcVar6 = pcVar6 + 1;
          pcVar7 = pcVar7 + 1;
        } while (bVar8);
        if (!bVar8) {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
          iVar4 = 6;
          pcVar6 = pcVar3;
          pcVar7 = "plane";
          do {
            if (iVar4 == 0) break;
            iVar4 = iVar4 + -1;
            bVar8 = *pcVar6 == *pcVar7;
            pcVar6 = pcVar6 + 1;
            pcVar7 = pcVar7 + 1;
          } while (bVar8);
          if (bVar8) {
            p = MeFPrimitiveCreate(pvVar2,5);
          }
          else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
            iVar4 = 7;
            bVar8 = false;
            pcVar6 = "convex";
            do {
              if (iVar4 == 0) break;
              iVar4 = iVar4 + -1;
              bVar8 = *pcVar3 == *pcVar6;
              pcVar3 = pcVar3 + 1;
              pcVar6 = pcVar6 + 1;
            } while (bVar8);
            if (bVar8) {
                    /* Unresolved local var: MeReal * v@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)] */
              iVar4 = 0;
              p = MeFPrimitiveCreate(pvVar2,6);
              if (0 < (int)pfVar1[3]) {
                iVar5 = 0;
                do {
                  iVar4 = iVar4 + 1;
                  pvVar2 = (void *)((int)pfVar1[4] + iVar5);
                  iVar5 = iVar5 + 0xc;
                  MeFPrimitiveAddVertex(p,pvVar2);
                } while (iVar4 < (int)pfVar1[3]);
              }
            }
          }
          goto LAB_00010590;
        }
        iVar4 = 4;
      }
      p = MeFPrimitiveCreate(pvVar2,iVar4);
      MeFPrimitiveSetRadius(p,*pfVar1);
      MeFPrimitiveSetHeight(p,pfVar1[1]);
    }
  }
LAB_00010590:
  if (p != (MeFPrimitive *)0x0) {
    MeFPrimitiveSetTransform(p,pfVar1 + 9);
  }
  return p;
}


/* ==== MeFModelCreateFromFile_1_0 ==== */

MeFModel * MeFModelCreateFromFile_1_0(MeFAsset *parent,PElement *e)

{
  PElementNode *pPVar1;
  void *pvVar2;
  void *pvVar3;
  char *pcVar4;
  MeFModel *pMVar5;
  int iVar6;
  char *pcVar7;
  char *pcVar8;
  bool bVar9;
  MeFGeometry *fg;
  PElementNode *dynamics;
  MeFModelType modelType;
  char *name;
  MeMatrix3 I;
  
                    /* Unresolved local var: char * geometryId@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * type@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeFModel * fm@[DW_OP_reg6(ESI)] */
  modelType = kMeFModelTypeDynamicsOnly;
  fg = (MeFGeometry *)0x0;
  pvVar2 = PElementGetAttributeValue(e,&DAT_000116a4);
  pvVar3 = PElementGetAttributeValue(e,"geometry");
  if (pvVar3 != (void *)0x0) {
    fg = MeFAssetLookupGeometry(parent,pvVar3);
  }
  pPVar1 = e->childHead;
  pcVar4 = PElementGetAttributeValue(e,&DAT_000116f1);
  if (pcVar4 == (char *)0x0) {
    modelType = (MeFModelType)(fg == (MeFGeometry *)0x0);
    if (pPVar1 == (PElementNode *)0x0) {
      if (fg == (MeFGeometry *)0x0) {
        return (MeFModel *)0x0;
      }
      modelType = kMeFModelTypeGeometryOnly;
    }
  }
  else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    iVar6 = 0xe;
    bVar9 = false;
    pcVar7 = pcVar4;
    pcVar8 = "dynamics_only";
    do {
      if (iVar6 == 0) break;
      iVar6 = iVar6 + -1;
      bVar9 = *pcVar7 == *pcVar8;
      pcVar7 = pcVar7 + 1;
      pcVar8 = pcVar8 + 1;
    } while (bVar9);
    if (!bVar9) {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
      iVar6 = 0xe;
      pcVar7 = pcVar4;
      pcVar8 = "geometry_only";
      do {
        if (iVar6 == 0) break;
        iVar6 = iVar6 + -1;
        bVar9 = *pcVar7 == *pcVar8;
        pcVar7 = pcVar7 + 1;
        pcVar8 = pcVar8 + 1;
      } while (bVar9);
      if (bVar9) {
        modelType = kMeFModelTypeGeometryOnly;
      }
      else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
        iVar6 = 0x16;
        pcVar7 = "dynamics_and_geometry";
        do {
          if (iVar6 == 0) break;
          iVar6 = iVar6 + -1;
          bVar9 = *pcVar4 == *pcVar7;
          pcVar4 = pcVar4 + 1;
          pcVar7 = pcVar7 + 1;
        } while (bVar9);
        if (bVar9) {
          modelType = kMeFModelTypeDynamicsAndGeometry;
        }
      }
    }
  }
  pMVar5 = MeFModelCreate(pvVar2,modelType);
  if (pPVar1 != (PElementNode *)0x0) {
                    /* Unresolved local var: PDynamics * dyn@[DW_OP_reg3(EBX)] */
    pvVar2 = pPVar1->current->cdata;
    MeFModelSetMassOffset(pMVar5,pvVar2);
    MeFModelSetMass(pMVar5,*(float *)((int)pvVar2 + 0xc));
    MeFModelSetDensity(pMVar5,*(float *)((int)pvVar2 + 0x10));
    I[0][0] = *(MeReal *)((int)pvVar2 + 0x14);
    I[1][1] = *(MeReal *)((int)pvVar2 + 0x20);
    I[2][2] = *(MeReal *)((int)pvVar2 + 0x28);
    I[0][1] = *(MeReal *)((int)pvVar2 + 0x18);
    I[0][2] = *(MeReal *)((int)pvVar2 + 0x1c);
    I[1][2] = *(MeReal *)((int)pvVar2 + 0x24);
    I[1][0] = I[0][1];
    I[2][0] = I[0][2];
    I[2][1] = I[1][2];
    MeFModelSetInertiaTensor(pMVar5,I);
    MeFModelSetLinearVelocityDamping(pMVar5,*(float *)((int)pvVar2 + 0x2c));
    MeFModelSetAngularVelocityDamping(pMVar5,*(float *)((int)pvVar2 + 0x30));
    MeFModelSetFastSpinAxis
              (pMVar5,*(float *)((int)pvVar2 + 0x34),*(float *)((int)pvVar2 + 0x38),
               *(float *)((int)pvVar2 + 0x3c));
    MeFModelEnableFastSpinAxis(pMVar5,*(int *)((int)pvVar2 + 0x40));
  }
  if (fg != (MeFGeometry *)0x0) {
    MeFModelSetGeometry(pMVar5,fg);
  }
  return pMVar5;
}


/* ==== MeFAssetPartCreateFromFile_1_0 ==== */

MeFAssetPart * MeFAssetPartCreateFromFile_1_0(MeFAsset *asset,PElement *e)

{
  void *pvVar1;
  void *pvVar2;
  void *pvVar3;
  MeFAssetPart *pMVar4;
  undefined4 extraout_EDX;
  char *extraout_EDX_00;
  char *extraout_EDX_01;
  char *pcVar5;
  longdouble lVar6;
  int i;
  char *c;
  char *d;
  MeVector3 off;
  
                    /* Unresolved local var: char * name@[DW_OP_reg6(ESI)]
                       Unresolved local var: char * modelId@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * graphic@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * scale@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * parent@[DW_OP_reg0(EAX)]
                       Unresolved local var: char * offset@[DW_OP_reg0(EAX)]
                       Unresolved local var: PAssetPart * data@[DW_OP_reg7(EDI)]
                       Unresolved local var: MeFAssetPart * part@[DW_OP_reg6(ESI)]
                       Unresolved local var: MeFModel * model@[DW_OP_reg0(EAX)] */
  pvVar3 = e->cdata;
  pvVar1 = PElementGetAttributeValue(e,&DAT_000116a4);
  pvVar2 = PElementGetAttributeValue(e,"model");
  pvVar2 = MeFAssetLookupModel(asset,pvVar2);
  pMVar4 = (MeFAssetPart *)0x0;
  if (pvVar2 != (void *)0x0) {
    pMVar4 = MeFAssetPartCreate(pvVar1,pvVar2,pvVar3);
    pvVar3 = PElementGetAttributeValue(e,"graphic");
    if (pvVar3 != (void *)0x0) {
      MeFAssetPartSetGraphicHint(pMVar4,pvVar3);
    }
    pvVar3 = PElementGetAttributeValue(e,"scale");
    if (pvVar3 != (void *)0x0) {
                    /* Unresolved local var: MeReal s@[DW_OP_reg0(EAX)] */
      lVar6 = (longdouble)__strtod_internal(pvVar3,0,0,extraout_EDX);
      MeFAssetPartSetGraphicScale(pMVar4,(float)lVar6);
    }
    pvVar3 = PElementGetAttributeValue(e,"parent");
    if (pvVar3 != (void *)0x0) {
      MeFAssetPartSetParentPartByName(pMVar4,pvVar3);
    }
    c = PElementGetAttributeValue(e,"graphic_offset");
    if (c != (char *)0x0) {
      if (*c != '\0') {
        lVar6 = (longdouble)__strtod_internal(c,&d,0,e);
        off[0] = (MeReal)lVar6;
        pcVar5 = d;
        if (c != d) {
          MeXMLParseComma(d,&c);
          pcVar5 = extraout_EDX_00;
        }
        if (*c != '\0') {
          lVar6 = (longdouble)__strtod_internal(c,&d,0,pcVar5);
          off[1] = (MeReal)lVar6;
          pcVar5 = d;
          if (c != d) {
            MeXMLParseComma(d,&c);
            pcVar5 = extraout_EDX_01;
          }
          if (*c != '\0') {
            lVar6 = (longdouble)__strtod_internal(c,&d,0,pcVar5);
            off[2] = (MeReal)lVar6;
            if (c != d) {
              MeXMLParseComma(d,&c);
            }
          }
        }
      }
      MeFAssetPartSetGraphicOffset(pMVar4,off[0],off[1],off[2]);
    }
  }
  return pMVar4;
}


/* ==== MeFJointCreateFromFile_1_0 ==== */

/* WARNING: Variable defined which should be unmapped: fj */

MeFJoint * MeFJointCreateFromFile_1_0(MeFAsset *asset,PElement *e)

{
  float *pfVar1;
  void *pvVar2;
  char *pcVar3;
  void *pvVar4;
  int iVar5;
  char *pcVar6;
  char *pcVar7;
  bool bVar8;
  float fVar9;
  char *part1;
  char *name;
  PJoint *source;
  MeFJoint *fj;
  
                    /* Unresolved local var: char * type@[DW_OP_reg3(EBX)]
                       Unresolved local var: char * part2@[DW_OP_reg3(EBX)]
                       Unresolved local var: int i@[???] */
  fj = (MeFJoint *)0x0;
  pfVar1 = e->cdata;
  pvVar2 = PElementGetAttributeValue(e,&DAT_000116a4);
  pcVar3 = PElementGetAttributeValue(e,&DAT_000116f1);
  if (pcVar3 == (char *)0x0) {
    return (MeFJoint *)0x0;
  }
  pvVar4 = PElementGetAttributeValue(e,"part1");
  if (pvVar4 == (void *)0x0) {
    return (MeFJoint *)0x0;
  }
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
  iVar5 = 0xe;
  bVar8 = false;
  pcVar6 = pcVar3;
  pcVar7 = "ballandsocket";
  do {
    if (iVar5 == 0) break;
    iVar5 = iVar5 + -1;
    bVar8 = *pcVar6 == *pcVar7;
    pcVar6 = pcVar6 + 1;
    pcVar7 = pcVar7 + 1;
  } while (bVar8);
  if (bVar8) {
    iVar5 = 3;
LAB_00011021:
    fj = MeFJointCreate(pvVar2,iVar5);
  }
  else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
    iVar5 = 6;
    bVar8 = false;
    pcVar6 = pcVar3;
    pcVar7 = "hinge";
    do {
      if (iVar5 == 0) break;
      iVar5 = iVar5 + -1;
      bVar8 = *pcVar6 == *pcVar7;
      pcVar6 = pcVar6 + 1;
      pcVar7 = pcVar7 + 1;
    } while (bVar8);
    if (bVar8) {
      iVar5 = 2;
LAB_00010ec5:
      fj = MeFJointCreate(pvVar2,iVar5);
      MeFJointSetProperty1f(fj,0,pfVar1[0x12]);
      MeFJointSetProperty1f(fj,1,pfVar1[0x13]);
      MeFJointSetProperty1f(fj,3,pfVar1[0x15]);
      MeFJointSetProperty1f(fj,4,pfVar1[0x16]);
      MeFJointSetProperty1b(fj,0xf,(int)pfVar1[0x21]);
      MeFJointSetProperty1b(fj,0x10,(int)pfVar1[0x22]);
      MeFJointSetProperty1f(fj,0x17,pfVar1[0x29]);
      fVar9 = pfVar1[0x23];
      iVar5 = 0x11;
    }
    else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
      iVar5 = 9;
      bVar8 = false;
      pcVar6 = pcVar3;
      pcVar7 = "carwheel";
      do {
        if (iVar5 == 0) break;
        iVar5 = iVar5 + -1;
        bVar8 = *pcVar6 == *pcVar7;
        pcVar6 = pcVar6 + 1;
        pcVar7 = pcVar7 + 1;
      } while (bVar8);
      if (bVar8) {
        fj = MeFJointCreate(pvVar2,1);
        MeFJointSetProperty1f(fj,0x17,pfVar1[0x29]);
        MeFJointSetProperty1f(fj,0x11,pfVar1[0x23]);
        MeFJointSetProperty1f(fj,0x18,pfVar1[0x2a]);
        MeFJointSetProperty1f(fj,0x12,pfVar1[0x24]);
        MeFJointSetProperty1b(fj,0x1c,(int)pfVar1[0x2e]);
        MeFJointSetProperty1f(fj,9,pfVar1[0x1b]);
        MeFJointSetProperty1f(fj,0,pfVar1[0x12]);
        MeFJointSetProperty1f(fj,1,pfVar1[0x13]);
        MeFJointSetProperty1f(fj,0x19,pfVar1[0x2b]);
        MeFJointSetProperty1f(fj,3,pfVar1[0x15]);
        fVar9 = pfVar1[0x16];
        iVar5 = 4;
      }
      else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
        iVar5 = 10;
        bVar8 = false;
        pcVar6 = pcVar3;
        pcVar7 = "conelimit";
        do {
          if (iVar5 == 0) break;
          iVar5 = iVar5 + -1;
          bVar8 = *pcVar6 == *pcVar7;
          pcVar6 = pcVar6 + 1;
          pcVar7 = pcVar7 + 1;
        } while (bVar8);
        if (bVar8) {
          fj = MeFJointCreate(pvVar2,4);
          MeFJointSetProperty1f(fj,0,pfVar1[0x12]);
          fVar9 = pfVar1[0x15];
          iVar5 = 3;
        }
        else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
          iVar5 = 10;
          bVar8 = false;
          pcVar6 = pcVar3;
          pcVar7 = "universal";
          do {
            if (iVar5 == 0) break;
            iVar5 = iVar5 + -1;
            bVar8 = *pcVar6 == *pcVar7;
            pcVar6 = pcVar6 + 1;
            pcVar7 = pcVar7 + 1;
          } while (bVar8);
          if (bVar8) {
            iVar5 = 5;
            goto LAB_00011021;
          }
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
          iVar5 = 5;
          bVar8 = false;
          pcVar6 = pcVar3;
          pcVar7 = "rpro";
          do {
            if (iVar5 == 0) break;
            iVar5 = iVar5 + -1;
            bVar8 = *pcVar6 == *pcVar7;
            pcVar6 = pcVar6 + 1;
            pcVar7 = pcVar7 + 1;
          } while (bVar8);
          if (bVar8) {
            fj = MeFJointCreate(pvVar2,6);
            MeFJointSetProperty1f(fj,0x11,pfVar1[0x23]);
            MeFJointSetProperty1f(fj,0x12,pfVar1[0x24]);
            MeFJointSetProperty1f(fj,0x13,pfVar1[0x25]);
            MeFJointSetProperty1f(fj,0x14,pfVar1[0x26]);
            MeFJointSetProperty1f(fj,0x15,pfVar1[0x27]);
            fVar9 = pfVar1[0x28];
            iVar5 = 0x16;
          }
          else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
            iVar5 = 10;
            bVar8 = false;
            pcVar6 = pcVar3;
            pcVar7 = "prismatic";
            do {
              if (iVar5 == 0) break;
              iVar5 = iVar5 + -1;
              bVar8 = *pcVar6 == *pcVar7;
              pcVar6 = pcVar6 + 1;
              pcVar7 = pcVar7 + 1;
            } while (bVar8);
            if (bVar8) {
              iVar5 = 7;
              goto LAB_00010ec5;
            }
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
            iVar5 = 9;
            bVar8 = false;
            pcVar6 = pcVar3;
            pcVar7 = "skeletal";
            do {
              if (iVar5 == 0) break;
              iVar5 = iVar5 + -1;
              bVar8 = *pcVar6 == *pcVar7;
              pcVar6 = pcVar6 + 1;
              pcVar7 = pcVar7 + 1;
            } while (bVar8);
            if (bVar8) {
              fj = MeFJointCreate(pvVar2,8);
              MeFJointSetProperty1i(fj,0x1a,(int)pfVar1[0x2c]);
              MeFJointSetProperty1f(fj,0,pfVar1[0x12]);
              MeFJointSetProperty1f(fj,1,pfVar1[0x13]);
              MeFJointSetProperty1f(fj,3,pfVar1[0x15]);
              MeFJointSetProperty1f(fj,9,pfVar1[0x1b]);
              MeFJointSetProperty1i(fj,0x1b,(int)pfVar1[0x2d]);
              MeFJointSetProperty1f(fj,2,pfVar1[0x14]);
              MeFJointSetProperty1f(fj,4,pfVar1[0x16]);
              MeFJointSetProperty1f(fj,10,pfVar1[0x1c]);
              MeFJointSetProperty1b(fj,0x1c,(int)pfVar1[0x2e]);
              goto LAB_00010b70;
            }
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
            iVar5 = 9;
            bVar8 = false;
            pcVar6 = pcVar3;
            pcVar7 = "angular3";
            do {
              if (iVar5 == 0) break;
              iVar5 = iVar5 + -1;
              bVar8 = *pcVar6 == *pcVar7;
              pcVar6 = pcVar6 + 1;
              pcVar7 = pcVar7 + 1;
            } while (bVar8);
            if (bVar8) {
              fj = MeFJointCreate(pvVar2,9);
              MeFJointSetProperty1b(fj,0x1c,(int)pfVar1[0x2e]);
              MeFJointSetProperty1f(fj,3,pfVar1[0x15]);
              fVar9 = pfVar1[0x1b];
              iVar5 = 9;
            }
            else {
                    /* Unresolved local var: size_t __s1_len@[???]
                       Unresolved local var: size_t __s2_len@[???] */
              iVar5 = 8;
              bVar8 = false;
              pcVar6 = "spring6";
              do {
                if (iVar5 == 0) break;
                iVar5 = iVar5 + -1;
                bVar8 = *pcVar3 == *pcVar6;
                pcVar3 = pcVar3 + 1;
                pcVar6 = pcVar6 + 1;
              } while (bVar8);
              if (!bVar8) goto LAB_00010b70;
              fj = MeFJointCreate(pvVar2,10);
              MeFJointSetProperty1f(fj,3,pfVar1[0x15]);
              MeFJointSetProperty1f(fj,4,pfVar1[0x16]);
              MeFJointSetProperty1f(fj,5,pfVar1[0x17]);
              MeFJointSetProperty1f(fj,6,pfVar1[0x18]);
              MeFJointSetProperty1f(fj,7,pfVar1[0x19]);
              MeFJointSetProperty1f(fj,8,pfVar1[0x1a]);
              MeFJointSetProperty1f(fj,9,pfVar1[0x1b]);
              MeFJointSetProperty1f(fj,10,pfVar1[0x1c]);
              MeFJointSetProperty1f(fj,0xb,pfVar1[0x1d]);
              MeFJointSetProperty1f(fj,0xc,pfVar1[0x1e]);
              MeFJointSetProperty1f(fj,0xd,pfVar1[0x1f]);
              fVar9 = pfVar1[0x20];
              iVar5 = 0xe;
            }
          }
        }
      }
    }
    MeFJointSetProperty1f(fj,iVar5,fVar9);
  }
LAB_00010b70:
  if (fj != (MeFJoint *)0x0) {
    MeFJointSetPosition(fj,0,*pfVar1,pfVar1[1],pfVar1[2]);
    MeFJointSetPrimaryAxis(fj,0,pfVar1[6],pfVar1[7],pfVar1[8]);
    MeFJointSetOrthogonalAxis(fj,0,pfVar1[0xc],pfVar1[0xd],pfVar1[0xe]);
    MeFJointSetPosition(fj,1,pfVar1[3],pfVar1[4],pfVar1[5]);
    MeFJointSetPrimaryAxis(fj,1,pfVar1[9],pfVar1[10],pfVar1[0xb]);
    MeFJointSetOrthogonalAxis(fj,1,pfVar1[0xf],pfVar1[0x10],pfVar1[0x11]);
    pvVar2 = PElementGetAttributeValue(e,"part2");
    MeFJointSetPartByName(fj,pvVar4,0);
    if (pvVar2 != (void *)0x0) {
      MeFJointSetPartByName(fj,pvVar2,1);
    }
  }
  return fj;
}


/* ==== Handle_Asset_1_0 ==== */

MeXMLError Handle_Asset_1_0(MeXMLElement *elem,PElement *parent)

{
  void *pvVar1;
  MeXMLError MVar2;
  int iVar3;
  MeXMLHandler *pMVar4;
  MeXMLHandler handlers [6];
  
  pMVar4 = handlers;
                    /* Unresolved local var: MeAssetDBXMLInput * input@[???]
                       Unresolved local var: PElement * e@[DW_OP_reg3(EBX)] */
  for (iVar3 = 0x30; iVar3 != 0; iVar3 = iVar3 + -1) {
    pMVar4->name = (char *)0x0;
    pMVar4 = (MeXMLHandler *)&pMVar4->type;
  }
  handlers[0].name = "GEOMETRY";
  handlers[0].fn = Handle_Geometry_1_0;
  handlers[1].name = "MODEL";
  handlers[1].fn = Handle_Model_1_0;
  handlers[2].name = "JOINT";
  handlers[2].fn = Handle_Joint_1_0;
  handlers[3].name = "PART";
  handlers[3].fn = Handle_AssetPart_1_0;
  handlers[4].name = "NO_COLLISION";
  handlers[4].fn = Handle_NoCollision_1_0;
  handlers[5].type = MeXMLActionEnd;
  pvVar1 = MeXMLElementGetInput(elem);
  MeXMLInputGetUserData(pvVar1);
  pvVar1 = PElementCreate(6,elem,(void *)0x0,(void *)0x0,elem->attr);
  PElementInsert(pvVar1,parent);
  MVar2 = MeXMLElementProcess(elem,handlers,(void *)0x0,pvVar1);
  return MVar2;
}


/* ==== Handle_KaFile_0_1 ==== */

MeXMLError Handle_KaFile_0_1(MeXMLElement *elem,PElement *parent)

{
  MeXMLError MVar1;
  int iVar2;
  undefined **ppuVar3;
  MeXMLHandler *pMVar4;
  MeXMLHandler handlers [2];
  
  ppuVar3 = &PTR_s_ASSET_000124fc;
  pMVar4 = handlers;
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    pMVar4->name = *ppuVar3;
    ppuVar3 = ppuVar3 + 1;
    pMVar4 = (MeXMLHandler *)&pMVar4->type;
  }
  MVar1 = MeXMLElementProcess(elem,handlers,(void *)0x0,parent);
  return MVar1;
}


/* ==== AddVertex_1_0 ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeXMLError AddVertex_1_0(MeXMLElement *elem,void *prim,void *parent)

{
  undefined4 *puVar1;
  int iVar2;
  void *pvVar3;
  int iVar4;
  
                    /* Unresolved local var: PPrimitive * cx@[???] */
  puVar1 = (undefined4 *)(*(int *)((int)prim + 0x10) + *(int *)((int)prim + 0xc) * 0xc);
  *puVar1 = *(undefined4 *)((int)prim + 0x18);
  puVar1[1] = *(undefined4 *)((int)prim + 0x1c);
  puVar1[2] = *(undefined4 *)((int)prim + 0x20);
  iVar2 = *(int *)((int)prim + 0x14);
  iVar4 = *(int *)((int)prim + 0xc) + 1;
  *(int *)((int)prim + 0xc) = iVar4;
  if (iVar2 <= iVar4) {
    *(int *)((int)prim + 0x14) = iVar2 * 2;
    pvVar3 = (*_DAT_00013104)(*(void **)((int)prim + 0x10),iVar2 * 0x18);
    *(void **)((int)prim + 0x10) = pvVar3;
  }
  return MeXMLErrorNone;
}


/* ==== Handle_GeometryPrimitive_1_0 ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeXMLError Handle_GeometryPrimitive_1_0(MeXMLElement *elem,PElement *parent)

{
  void *pvVar1;
  void *pvVar2;
  MeXMLError MVar3;
  int iVar4;
  undefined **ppuVar5;
  MeXMLHandler *pMVar6;
  MeXMLHandler handlers [6];
  
  ppuVar5 = &PTR_s_RADIUS_00011b1c;
  pMVar6 = handlers;
                    /* Unresolved local var: PPrimitive * data@[DW_OP_reg3(EBX)]
                       Unresolved local var: PElement * e@[DW_OP_reg6(ESI)] */
  for (iVar4 = 0x30; iVar4 != 0; iVar4 = iVar4 + -1) {
    pMVar6->name = *ppuVar5;
    ppuVar5 = ppuVar5 + 1;
    pMVar6 = (MeXMLHandler *)&pMVar6->type;
  }
  pvVar1 = (*_DAT_000130f4)(100);
  *(undefined4 *)((int)pvVar1 + 0xc) = 0;
  *(undefined4 *)((int)pvVar1 + 0x14) = 1;
  pvVar2 = (*_MeMemoryAPI)(0xc);
  *(void **)((int)pvVar1 + 0x10) = pvVar2;
  pvVar2 = PElementCreate(2,elem,pvVar1,FreePrimitive,elem->attr);
  PElementInsert(pvVar2,parent);
  MVar3 = MeXMLElementProcess(elem,handlers,pvVar1,pvVar2);
  return MVar3;
}


/* ==== Handle_Geometry_1_0 ==== */

MeXMLError Handle_Geometry_1_0(MeXMLElement *elem,PElement *parent)

{
  void *pvVar1;
  MeXMLError MVar2;
  int iVar3;
  undefined **ppuVar4;
  MeXMLHandler *pMVar5;
  MeXMLHandler handlers [2];
  
  ppuVar4 = &PTR_s_PRIMITIVE_00011bdc;
  pMVar5 = handlers;
                    /* Unresolved local var: PElement * e@[DW_OP_reg3(EBX)] */
  for (iVar3 = 0x10; iVar3 != 0; iVar3 = iVar3 + -1) {
    pMVar5->name = *ppuVar4;
    ppuVar4 = ppuVar4 + 1;
    pMVar5 = (MeXMLHandler *)&pMVar5->type;
  }
  pvVar1 = PElementCreate(1,elem,(void *)0x0,(void *)0x0,elem->attr);
  PElementInsert(pvVar1,parent);
  MVar2 = MeXMLElementProcess(elem,handlers,(void *)0x0,pvVar1);
  return MVar2;
}


/* ==== Handle_Dynamics_1_0 ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeXMLError Handle_Dynamics_1_0(MeXMLElement *elem,PElement *parent)

{
  void *pvVar1;
  void *pvVar2;
  MeXMLError MVar3;
  int iVar4;
  undefined **ppuVar5;
  MeXMLHandler *pMVar6;
  MeXMLHandler handlers [9];
  
  ppuVar5 = &PTR_s_MASS_OFFSET_00011c1c;
  pMVar6 = handlers;
                    /* Unresolved local var: PDynamics * data@[DW_OP_reg3(EBX)]
                       Unresolved local var: PElement * e@[DW_OP_reg6(ESI)] */
  for (iVar4 = 0x48; iVar4 != 0; iVar4 = iVar4 + -1) {
    pMVar6->name = *ppuVar5;
    ppuVar5 = ppuVar5 + 1;
    pMVar6 = (MeXMLHandler *)&pMVar6->type;
  }
  pvVar1 = (*_DAT_000130f4)(0x44);
  pvVar2 = PElementCreate(4,elem,pvVar1,_DAT_000130fc,elem->attr);
  PElementInsert(pvVar2,parent);
  *(undefined4 *)((int)pvVar1 + 0x10) = 0x3f800000;
  *(undefined4 *)((int)pvVar1 + 0xc) = 0x3f800000;
  *(undefined4 *)((int)pvVar1 + 0x30) = 0;
  *(undefined4 *)((int)pvVar1 + 0x2c) = 0;
  *(undefined4 *)((int)pvVar1 + 0x34) = 0;
  *(undefined4 *)((int)pvVar1 + 0x38) = 0x3f800000;
  *(undefined4 *)((int)pvVar1 + 0x3c) = 0;
  *(undefined4 *)((int)pvVar1 + 0x40) = 0;
  *(undefined4 *)((int)pvVar1 + 0x14) = 0x3ecccccd;
  *(undefined4 *)((int)pvVar1 + 0x20) = 0x3ecccccd;
  *(undefined4 *)((int)pvVar1 + 0x28) = 0x3ecccccd;
  MVar3 = MeXMLElementProcess(elem,handlers,pvVar1,pvVar2);
  return MVar3;
}


/* ==== Handle_Model_1_0 ==== */

MeXMLError Handle_Model_1_0(MeXMLElement *elem,PElement *parent)

{
  void *pvVar1;
  MeXMLError MVar2;
  int iVar3;
  undefined **ppuVar4;
  MeXMLHandler *pMVar5;
  MeXMLHandler handlers [2];
  
  ppuVar4 = &PTR_s_DYNAMICS_00011d3c;
  pMVar5 = handlers;
                    /* Unresolved local var: PElement * e@[DW_OP_reg3(EBX)] */
  for (iVar3 = 0x10; iVar3 != 0; iVar3 = iVar3 + -1) {
    pMVar5->name = *ppuVar4;
    ppuVar4 = ppuVar4 + 1;
    pMVar5 = (MeXMLHandler *)&pMVar5->type;
  }
  pvVar1 = PElementCreate(3,elem,(void *)0x0,(void *)0x0,elem->attr);
  PElementInsert(pvVar1,parent);
  MVar2 = MeXMLElementProcess(elem,handlers,(void *)0x0,pvVar1);
  return MVar2;
}


/* ==== Handle_Joint_1_0 ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeXMLError Handle_Joint_1_0(MeXMLElement *elem,PElement *parent)

{
  void *pvVar1;
  void *pvVar2;
  MeXMLError MVar3;
  int iVar4;
  undefined **ppuVar5;
  MeXMLHandler *pMVar6;
  MeXMLHandler handlers [58];
  
  ppuVar5 = &PTR_DAT_00011d7c;
  pMVar6 = handlers;
                    /* Unresolved local var: PJoint * data@[DW_OP_reg6(ESI)]
                       Unresolved local var: PElement * e@[DW_OP_reg3(EBX)] */
  for (iVar4 = 0x1d0; iVar4 != 0; iVar4 = iVar4 + -1) {
    pMVar6->name = *ppuVar5;
    ppuVar5 = ppuVar5 + 1;
    pMVar6 = (MeXMLHandler *)&pMVar6->type;
  }
  pvVar1 = (*_DAT_000130f4)(0xbc);
  pvVar2 = PElementCreate(5,elem,pvVar1,_DAT_000130fc,elem->attr);
  PElementInsert(pvVar2,parent);
  MVar3 = MeXMLElementProcess(elem,handlers,pvVar1,pvVar2);
  return MVar3;
}


/* ==== Handle_AssetPart_1_0 ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeXMLError Handle_AssetPart_1_0(MeXMLElement *elem,PElement *parent)

{
  void *pvVar1;
  void *pvVar2;
  MeXMLError MVar3;
  int iVar4;
  undefined **ppuVar5;
  MeXMLHandler *pMVar6;
  MeXMLHandler handlers [2];
  
  ppuVar5 = &PTR_DAT_000124bc;
  pMVar6 = handlers;
                    /* Unresolved local var: PAssetPart * data@[DW_OP_reg6(ESI)]
                       Unresolved local var: PElement * e@[DW_OP_reg3(EBX)] */
  for (iVar4 = 0x10; iVar4 != 0; iVar4 = iVar4 + -1) {
    pMVar6->name = *ppuVar5;
    ppuVar5 = ppuVar5 + 1;
    pMVar6 = (MeXMLHandler *)&pMVar6->type;
  }
  pvVar1 = (*_DAT_000130f4)(0x40);
  pvVar2 = PElementCreate(7,elem,pvVar1,_DAT_000130fc,elem->attr);
  PElementInsert(pvVar2,parent);
  MVar3 = MeXMLElementProcess(elem,handlers,pvVar1,pvVar2);
  return MVar3;
}


/* ==== Handle_NoCollision_1_0 ==== */

MeXMLError Handle_NoCollision_1_0(MeXMLElement *elem,PElement *parent)

{
  void *pvVar1;
  MeXMLError MVar2;
  MeXMLHandler handlers [1];
  
                    /* Unresolved local var: PElement * e@[DW_OP_reg3(EBX)] */
  handlers[0].name = (char *)0x0;
  handlers[0].fn = (void *)0x0;
  handlers[0].offset = 0;
  handlers[0].max = 0;
  handlers[0].maxstr = 0;
  handlers[0].called = 0;
  handlers[0].cb = (_func_MeXMLError_MeXMLElement_ptr_void_ptr_void_ptr *)0x0;
  handlers[0].type = MeXMLActionEnd;
  pvVar1 = PElementCreate(8,elem,(void *)0x0,(void *)0x0,elem->attr);
  PElementInsert(pvVar1,parent);
  MVar2 = MeXMLElementProcess(elem,handlers,(void *)0x0,pvVar1);
  return MVar2;
}


/* ==== FreePrimitive ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FreePrimitive(void *m)

{
                    /* Unresolved local var: PPrimitive * p@[???] */
  (*_DAT_000130fc)(*(void **)((int)m + 0x10));
  (*_DAT_000130fc)(m);
  return;
}


