/* ==== MeASEObjectDestroy ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void MeASEObjectDestroy(MeASEObject *object)

{
  MeASESubMaterial *pMVar1;
  MeASEObject *pMVar2;
  MeASEMaterialStore *pMVar3;
  int iVar4;
  
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)] */
  pMVar3 = object->matStore;
  if (pMVar3->materials != (MeASEMaterial *)0x0) {
    if (0 < pMVar3->numMaterials) {
      iVar4 = 0;
      do {
        pMVar1 = pMVar3->materials[iVar4].subMaterials;
        if (pMVar1 != (MeASESubMaterial *)0x0) {
          (*_strlen)(pMVar1);
          pMVar3 = object->matStore;
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 < pMVar3->numMaterials);
    }
    (*_strlen)(pMVar3->materials);
    pMVar3 = object->matStore;
  }
  if (pMVar3 != (MeASEMaterialStore *)0x0) {
    (*_strlen)(pMVar3);
  }
  while (object != (MeASEObject *)0x0) {
                    /* Unresolved local var: MeASEObject * tmp@[DW_OP_reg6(ESI)] */
    pMVar2 = object->nextObject;
    if (object->verts != (MeVector3 *)0x0) {
      (*_strlen)(object->verts);
    }
    if (object->faces != (MeASEFace *)0x0) {
      (*_strlen)(object->faces);
    }
    if (object->uvs != (MeASEUV *)0x0) {
      (*_strlen)(object->uvs);
    }
    (*_strlen)(object);
    object = pMVar2;
  }
  return;
}


/* ==== MeASEObjectLoadPartsFromStream ==== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

MeASEObject *
MeASEObjectLoadPartsFromStream(MeStream fp,MeReal xScale,MeReal yScale,MeReal zScale,MeBool asParts)

{
  MeReal MVar1;
  bool bVar2;
  bool bVar3;
  bool bVar4;
  bool bVar5;
  float *pfVar6;
  MeASEMaterialStore *materials;
  void *pvVar7;
  char *pcVar8;
  size_t sVar9;
  MeBool MVar10;
  char *pcVar11;
  int iVar12;
  MeASESubMaterial *pMVar13;
  MeASEMaterial *pMVar14;
  MeASEUV *pMVar15;
  MeASEFace *pMVar16;
  MeVector3 *paMVar17;
  MeASEObject *object_00;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 extraout_ECX_04;
  undefined4 uVar18;
  int iVar19;
  MeReal *pMVar20;
  MeReal *pMVar21;
  char cVar22;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 extraout_EDX_01;
  undefined4 extraout_EDX_02;
  uint uVar23;
  uint uVar24;
  char *pcVar25;
  char *pcVar26;
  char *pcVar27;
  void *pvVar28;
  MeBool MVar29;
  undefined4 uVar30;
  int local_320;
  int local_31c;
  MeASEMaterial *local_318;
  int local_314;
  MeBool inmaterial;
  MeBool diffusemap_found;
  int submat;
  MeASEMaterialStore *materialList;
  MeASEObject *object;
  MeBool isError;
  int uvOffset;
  int faceOffset;
  int vertOffset;
  MeBool gotName;
  MeBool mappingchannel;
  int temp;
  int materialIndex;
  int temp_1;
  int i1;
  int i2;
  int i3;
  float x;
  float y;
  float z;
  int i1_1;
  int i2_1;
  int i3_1;
  int temp_2;
  float n1;
  float n2;
  float n3;
  int index;
  float x_1;
  float y_1;
  float z_1;
  float x_2;
  float y_2;
  float z_2;
  float x_3;
  float y_3;
  float z_3;
  float x_4;
  float y_4;
  float z_4;
  float x_5;
  float y_5;
  float z_5;
  MeVector4 norm;
  MeVector4 transNorm;
  MeMatrix4 vertexTM;
  char line [256];
  
                    /* Unresolved local var: char * args@[DW_OP_reg6(ESI)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)] */
  bVar3 = false;
  gotName = 0;
  vertOffset = 0;
  faceOffset = 0;
  uvOffset = 0;
  bVar2 = false;
  object = (MeASEObject *)0x0;
  materials = (MeASEMaterialStore *)(*_MeMemoryAPI)(8);
  materials->numMaterials = 0;
  materials->materials = (MeASEMaterial *)0x0;
  vertexTM[0][0] = 1.0;
  vertexTM[0][1] = 0.0;
  vertexTM[0][2] = 0.0;
  vertexTM[1][0] = 0.0;
  vertexTM[1][1] = 1.0;
  vertexTM[1][2] = 0.0;
  vertexTM[2][0] = 0.0;
  vertexTM[2][1] = 0.0;
  vertexTM[2][2] = 1.0;
LAB_00010210:
  do {
    pvVar7 = MeStreamReadLine(line,0x100,fp);
    if (pvVar7 == (void *)0x0) {
      if (bVar2) {
LAB_000115d0:
        if (object != (MeASEObject *)0x0) {
          MeASEObjectDestroy(object);
        }
        object = (MeASEObject *)0x0;
      }
      else if (object != (MeASEObject *)0x0) {
        object->isLoaded = 1;
      }
      return object;
    }
    if (bVar2) goto LAB_000115d0;
    pcVar25 = "*GEOMOBJECT";
    pvVar28 = pvVar7;
    pcVar8 = strstr(line,"*GEOMOBJECT");
    if (pcVar8 != (char *)0x0) {
      bVar3 = false;
      if (object != (MeASEObject *)0x0) break;
      object = (MeASEObject *)(*_MeMemoryAPI)(0x124,pcVar25,pvVar7,pvVar28);
      ASEObjectReset(object,materials);
      goto LAB_00011599;
    }
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
    pcVar25 = strstr(line,"*NODE_NAME");
    pcVar8 = (char *)0x0;
    uVar18 = extraout_ECX;
    if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
      sVar9 = strlen("*NODE_NAME");
      pcVar8 = pcVar25 + sVar9 + 1;
      uVar18 = extraout_ECX_00;
    }
    MVar10 = 0;
    if ((pcVar8 == (char *)0x0) || (MVar10 = gotName, gotName != 0)) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
      pcVar26 = "*TM_ROW0";
      MVar29 = MVar10;
      pcVar25 = strstr(line,"*TM_ROW0");
      pcVar8 = (char *)0x0;
      if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
        sVar9 = strlen("*TM_ROW0");
        pcVar8 = pcVar25 + sVar9 + 1;
      }
      if (pcVar8 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
        pcVar27 = "*TM_ROW1";
        pcVar26 = pcVar8;
        pcVar11 = strstr(line,"*TM_ROW1");
        pcVar25 = (char *)0x0;
        if (pcVar11 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
          sVar9 = strlen("*TM_ROW1");
          pcVar25 = pcVar11 + sVar9 + 1;
        }
        if (pcVar25 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
          pcVar27 = "*TM_ROW2";
          pcVar26 = pcVar25;
          pcVar11 = strstr(line,"*TM_ROW2");
          pcVar8 = (char *)0x0;
          if (pcVar11 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
            sVar9 = strlen("*TM_ROW2");
            pcVar8 = pcVar11 + sVar9 + 1;
          }
          if (pcVar8 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
            pcVar27 = "*TM_ROW3";
            pcVar26 = pcVar8;
            pcVar11 = strstr(line,"*TM_ROW3");
            pcVar25 = (char *)0x0;
            if (pcVar11 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
              sVar9 = strlen("*TM_ROW3");
              pcVar25 = pcVar11 + sVar9 + 1;
            }
            if (pcVar25 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
              pcVar25 = strstr(line,"*MESH_NUMVERTEX");
              pcVar8 = (char *)0x0;
              if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                sVar9 = strlen("*MESH_NUMVERTEX");
                pcVar8 = pcVar25 + sVar9 + 1;
              }
              if (pcVar8 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                pcVar25 = strstr(line,"*MESH_NUMFACES");
                pcVar8 = (char *)0x0;
                if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                  sVar9 = strlen("*MESH_NUMFACES");
                  pcVar8 = pcVar25 + sVar9 + 1;
                }
                if (pcVar8 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                  pcVar25 = strstr(line,"*MESH_VERTEX");
                  pcVar8 = (char *)0x0;
                  if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                    sVar9 = strlen("*MESH_VERTEX");
                    pcVar8 = pcVar25 + sVar9 + 1;
                  }
                  if (pcVar8 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                    pcVar25 = strstr(line,"*MESH_FACENORMAL");
                    pcVar8 = (char *)0x0;
                    uVar18 = extraout_EDX;
                    if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                      sVar9 = strlen("*MESH_FACENORMAL");
                      pcVar8 = pcVar25 + sVar9 + 1;
                      uVar18 = extraout_EDX_00;
                    }
                    if (pcVar8 == (char *)0x0) {
                      pcVar8 = strstr(line,"*MESH_MAPPINGCHANNEL");
                      if (pcVar8 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                        pcVar25 = strstr(line,"*MESH_FACE");
                        pcVar8 = (char *)0x0;
                        uVar18 = extraout_EDX_01;
                        if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                          sVar9 = strlen("*MESH_FACE");
                          pcVar8 = pcVar25 + sVar9 + 1;
                          uVar18 = extraout_EDX_02;
                        }
                        if (pcVar8 == (char *)0x0) {
                          if (!bVar3) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                            pcVar25 = strstr(line,"*MESH_NUMTVERTEX");
                            pcVar8 = (char *)0x0;
                            if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                              sVar9 = strlen("*MESH_NUMTVERTEX");
                              pcVar8 = pcVar25 + sVar9 + 1;
                            }
                            if (pcVar8 != (char *)0x0) {
                              uvOffset = object->numUvs;
                              sscanf(pcVar8,"%d",&object->numUvs,pcVar8);
                              iVar19 = uvOffset + object->numUvs;
                              object->numUvs = iVar19;
                              pMVar15 = (MeASEUV *)(*_strchr)(object->uvs,iVar19 * 8);
                              object->uvs = pMVar15;
                              goto LAB_00010210;
                            }
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                            pcVar25 = strstr(line,"*MESH_TVERT");
                            pcVar8 = (char *)0x0;
                            if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                              sVar9 = strlen("*MESH_TVERT");
                              pcVar8 = pcVar25 + sVar9 + 1;
                            }
                            if (pcVar8 != (char *)0x0) {
                              iVar19 = sscanf(pcVar8,"%d %f %f %f",&index,&x,&y,&z,pcVar8,pcVar8);
                              if (iVar19 == 4) {
                                object->uvs[uvOffset + index].u = x;
                                object->uvs[uvOffset + index].v = y;
                              }
                              goto LAB_00010210;
                            }
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                            pcVar25 = strstr(line,"*MESH_TFACE");
                            pcVar8 = (char *)0x0;
                            uVar18 = extraout_ECX_01;
                            if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                              sVar9 = strlen("*MESH_TFACE");
                              pcVar8 = pcVar25 + sVar9 + 1;
                              uVar18 = extraout_ECX_02;
                            }
                            if (pcVar8 != (char *)0x0) {
                              iVar19 = sscanf(pcVar8,"%d %d %d %d",&index,&i1,&i2,&i3,uVar18,uVar18)
                              ;
                              if (iVar19 == 4) {
                                pMVar16 = object->faces;
                                MVar1 = object->uvs[uvOffset + i1].u;
                                pMVar16[faceOffset + index].map[0].v = object->uvs[uvOffset + i1].v;
                                pMVar16[faceOffset + index].map[0].u = MVar1;
                                pMVar16 = object->faces;
                                MVar1 = object->uvs[uvOffset + i2].u;
                                pMVar16[faceOffset + index].map[1].v = object->uvs[uvOffset + i2].v;
                                pMVar16[faceOffset + index].map[1].u = MVar1;
                                pMVar16 = object->faces;
                                MVar1 = object->uvs[uvOffset + i3].u;
                                pMVar16[faceOffset + index].map[2].v = object->uvs[uvOffset + i3].v;
                                pMVar16[faceOffset + index].map[2].u = MVar1;
                              }
                              goto LAB_00010210;
                            }
                          }
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                          pcVar25 = strstr(line,"*MATERIAL_REF");
                          pcVar8 = (char *)0x0;
                          if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                            sVar9 = strlen("*MATERIAL_REF");
                            pcVar8 = pcVar25 + sVar9 + 1;
                          }
                          if (pcVar8 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                            pcVar25 = strstr(line,"*MATERIAL_COUNT");
                            pcVar8 = (char *)0x0;
                            if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                              sVar9 = strlen("*MATERIAL_COUNT");
                              pcVar8 = pcVar25 + sVar9 + 1;
                            }
                            if (pcVar8 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                              pcVar25 = strstr(line,"*MATERIAL");
                              pcVar8 = (char *)0x0;
                              if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                                sVar9 = strlen("*MATERIAL");
                                pcVar8 = pcVar25 + sVar9 + 1;
                              }
                              if (pcVar8 != (char *)0x0) {
                                submat = 0;
                                iVar12 = sscanf(pcVar8,"%d",&materialIndex,pcVar8);
                                iVar19 = materialIndex;
                                if (iVar12 == 1) {
                                  bVar4 = false;
                                  bVar5 = false;
                                  materials->materials[materialIndex].numSubs = 1;
                                  pMVar14 = materials->materials;
                                  pMVar13 = (MeASESubMaterial *)(*_MeStreamReadLine)(0x134);
                                  pMVar14[iVar19].subMaterials = pMVar13;
                                  (materials->materials[materialIndex].subMaterials)->type =
                                       MeASEMaterialFlagNone;
                                  local_31c = 0;
LAB_00010673:
                                  pvVar7 = MeStreamReadLine(line,0x100,fp);
                                  iVar19 = materialIndex;
                                  if (pvVar7 != (void *)0x0) {
                                    local_314 = materialIndex;
                                    local_318 = materials->materials;
                                    if (local_318[materialIndex].numSubs <= submat)
                                    goto LAB_00010210;
                                    pcVar8 = strstr(line,"*MATERIAL_NAME");
                                    if (pcVar8 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                                      pcVar25 = strstr(line,"*NUMSUBMTLS");
                                      pcVar8 = (char *)0x0;
                                      uVar18 = extraout_ECX_03;
                                      if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                                        sVar9 = strlen("*NUMSUBMTLS");
                                        pcVar8 = pcVar25 + sVar9 + 1;
                                        uVar18 = extraout_ECX_04;
                                      }
                                      if (pcVar8 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                                        pcVar26 = "*MATERIAL_AMBIENT";
                                        uVar30 = uVar18;
                                        pcVar25 = strstr(line,"*MATERIAL_AMBIENT");
                                        pcVar8 = (char *)0x0;
                                        if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                                          sVar9 = strlen("*MATERIAL_AMBIENT");
                                          pcVar8 = pcVar25 + sVar9 + 1;
                                        }
                                        if (pcVar8 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                                          pcVar27 = "*MATERIAL_DIFFUSE";
                                          pcVar26 = pcVar8;
                                          pcVar11 = strstr(line,"*MATERIAL_DIFFUSE");
                                          pcVar25 = (char *)0x0;
                                          if (pcVar11 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                                            sVar9 = strlen("*MATERIAL_DIFFUSE");
                                            pcVar25 = pcVar11 + sVar9 + 1;
                                          }
                                          if (pcVar25 == (char *)0x0) {
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                                            pcVar27 = "*MATERIAL_SPECULAR";
                                            pcVar26 = pcVar11;
                                            pcVar8 = strstr(line,"*MATERIAL_SPECULAR");
                                            pcVar25 = (char *)0x0;
                                            if (pcVar8 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                                              sVar9 = strlen("*MATERIAL_SPECULAR");
                                              pcVar25 = pcVar8 + sVar9 + 1;
                                            }
                                            if (pcVar25 == (char *)0x0) {
                                              pcVar8 = strstr(line,"*MAP_DIFFUSE");
                                              if (pcVar8 != (char *)0x0) {
                                                bVar4 = true;
                                                goto LAB_00010673;
                                              }
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                                              pcVar25 = strstr(line,"*BITMAP");
                                              pcVar8 = (char *)0x0;
                                              if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                                                sVar9 = strlen("*BITMAP");
                                                pcVar8 = pcVar25 + sVar9 + 1;
                                              }
                                              if ((pcVar8 == (char *)0x0) || (!bVar4)) {
                                                pcVar8 = strchr(line,0x7d);
                                                if (pcVar8 == (char *)0x0) goto LAB_00010673;
                                                if (bVar5) goto LAB_00010837;
                                              }
                                              else {
                    /* Unresolved local var: int j@[DW_OP_reg1(ECX)] */
                                                iVar19 = 0;
                                                cVar22 = pcVar8[1];
                                                if (cVar22 != '\"') {
                                                  while( true ) {
                                                    iVar12 = local_31c + iVar19;
                                                    iVar19 = iVar19 + 1;
                                                    (local_318[local_314].subMaterials)->texFilename
                                                    [iVar12] = cVar22;
                                                    cVar22 = (pcVar8 + 1)[iVar19];
                                                    if (cVar22 == '\"') break;
                                                    local_314 = materialIndex;
                                                    local_318 = materials->materials;
                                                  }
                                                  local_314 = materialIndex;
                                                  local_318 = materials->materials;
                                                }
                                                (local_318[local_314].subMaterials)->texFilename
                                                [local_31c + iVar19] = '\0';
                                                *(undefined4 *)
                                                 ((int)(materials->materials[materialIndex].
                                                       subMaterials)->ambient + local_31c + -4) = 1;
                                                bVar4 = false;
                                                if (!bVar5) goto LAB_00010673;
LAB_00010837:
                                                local_31c = local_31c + 0x134;
                                                submat = submat + 1;
                                              }
                                              bVar5 = false;
                                              goto LAB_00010673;
                                            }
                    /* Unresolved local var: float * specular@[DW_OP_reg3(EBX)] */
                                            pMVar13 = local_318[iVar19].subMaterials;
                                            iVar19 = (int)pMVar13->specular + local_31c + 8;
                                            pfVar6 = pMVar13->specular;
                                            iVar12 = (int)pMVar13->specular + local_31c + 4;
                                            pcVar8 = pcVar11;
                                          }
                                          else {
                    /* Unresolved local var: float * diffuse@[DW_OP_reg3(EBX)] */
                                            pMVar13 = local_318[iVar19].subMaterials;
                                            iVar19 = (int)pMVar13->diffuse + local_31c + 8;
                                            pfVar6 = pMVar13->diffuse;
                                            iVar12 = (int)pMVar13->diffuse + local_31c + 4;
                                          }
                                          sscanf(pcVar25,"%f %f %f",(int)pfVar6 + local_31c,iVar12,
                                                 iVar19,pcVar27,pcVar8,pcVar26);
                                          *(undefined4 *)((int)pfVar6 + local_31c + 0xc) =
                                               0x3f800000;
                                          goto LAB_00010673;
                                        }
                    /* Unresolved local var: float * ambient@[DW_OP_reg3(EBX)] */
                                        pMVar13 = local_318[iVar19].subMaterials;
                                        sscanf(pcVar8,"%f %f %f",(int)pMVar13->ambient + local_31c,
                                               (int)pMVar13->ambient + local_31c + 4,
                                               (int)pMVar13->ambient + local_31c + 8,pcVar26,uVar18,
                                               uVar30);
                                        *(undefined4 *)((int)pMVar13->ambient + local_31c + 0xc) = 0
                                        ;
                                      }
                                      else {
                                        sscanf(pcVar8,"%d",&temp,uVar18);
                                        iVar19 = materialIndex;
                                        materials->materials[materialIndex].numSubs = temp;
                                        pMVar14 = materials->materials;
                                        pMVar13 = (MeASESubMaterial *)
                                                  (*_strchr)(materials->materials[materialIndex].
                                                             subMaterials,
                                                             pMVar14[materialIndex].numSubs * 0x134)
                                        ;
                                        pMVar14[iVar19].subMaterials = pMVar13;
                                      }
                                    }
                                    else {
                                      bVar4 = false;
                                      bVar5 = true;
                                    }
                                    goto LAB_00010673;
                                  }
                                }
                              }
                            }
                            else {
                              sscanf(pcVar8,"%d",materials,pcVar25);
                              pMVar14 = (MeASEMaterial *)
                                        (*_strchr)(materials->materials,materials->numMaterials << 3
                                                  );
                              materials->materials = pMVar14;
                            }
                          }
                          else {
                            sscanf(pcVar8,"%d",&temp_1,pcVar8);
                            if (faceOffset < object->numFaces) {
                              iVar19 = faceOffset * 0x5c;
                              iVar12 = faceOffset;
                              do {
                                iVar12 = iVar12 + 1;
                                *(int *)((int)(object->faces->map + 3) + iVar19) = temp_1;
                                iVar19 = iVar19 + 0x5c;
                              } while (iVar12 < object->numFaces);
                            }
                          }
                          goto LAB_00010210;
                        }
                        iVar19 = sscanf(pcVar8,"%d: A: %d B: %d C: %d",&index,&i1_1,&i2_1,&i3_1,
                                        uVar18,uVar18);
                        if (iVar19 == 4) {
                          object->faces[faceOffset + index].vertexId[0] = vertOffset + i1_1;
                          object->faces[faceOffset + index].vertexId[1] = vertOffset + i2_1;
                          object->faces[faceOffset + index].vertexId[2] = vertOffset + i3_1;
                        }
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                        pcVar25 = strstr(pcVar8,"*MESH_MTLID");
                        pcVar8 = (char *)0x0;
                        if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                          sVar9 = strlen("*MESH_MTLID");
                          pcVar8 = pcVar25 + sVar9 + 1;
                        }
                        if ((pcVar8 != (char *)0x0) &&
                           (iVar19 = sscanf(pcVar8,"%d",&i1_1,pcVar8), iVar19 == 1)) {
                          object->faces[faceOffset + index].subMaterialId = i1_1;
                        }
                      }
                      else {
                        bVar3 = true;
                      }
                    }
                    else {
                      iVar19 = sscanf(pcVar8,"%d %f %f %f",&index,&n1,&n2,&n3,uVar18,uVar18);
                      if (iVar19 == 4) {
                        norm[3] = 0.0;
                        object->faces[faceOffset + index].normal[0] =
                             vertexTM[0][0] * n1 + vertexTM[1][0] * n2 + vertexTM[2][0] * n3;
                        object->faces[faceOffset + index].normal[1] =
                             vertexTM[2][1] * n3 + vertexTM[0][1] * n1 + vertexTM[1][1] * n2;
                        object->faces[faceOffset + index].normal[2] =
                             vertexTM[0][2] * n1 + vertexTM[1][2] * n2 + vertexTM[2][2] * n3;
                      }
                      iVar19 = 0;
                      local_320 = 0;
                      do {
                        pvVar7 = MeStreamReadLine(line,0x100,fp);
                        if (pvVar7 == (void *)0x0) {
                          bVar2 = true;
                        }
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
                        pcVar25 = strstr(line,"*MESH_VERTEXNORMAL");
                        pcVar8 = (char *)0x0;
                        if (pcVar25 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
                          sVar9 = strlen("*MESH_VERTEXNORMAL");
                          pcVar8 = pcVar25 + sVar9 + 1;
                        }
                        iVar12 = sscanf(pcVar8,"%d %f %f %f",&temp_2,&n1,&n2,&n3,pcVar25,pcVar25);
                        if (iVar12 == 4) {
                          norm[3] = 0.0;
                          *(float *)((int)object->faces[faceOffset + index].vNormal[0] + local_320)
                               = vertexTM[0][0] * n1 + vertexTM[1][0] * n2 + vertexTM[2][0] * n3;
                          *(float *)((int)object->faces[faceOffset + index].vNormal[0] +
                                    local_320 + 4) =
                               vertexTM[2][1] * n3 + vertexTM[0][1] * n1 + vertexTM[1][1] * n2;
                          *(float *)((int)object->faces[faceOffset + index].vNormal[0] +
                                    local_320 + 8) =
                               vertexTM[0][2] * n1 + vertexTM[1][2] * n2 + vertexTM[2][2] * n3;
                        }
                        iVar19 = iVar19 + 1;
                        local_320 = local_320 + 0xc;
                      } while (iVar19 < 3);
                    }
                  }
                  else {
                    iVar19 = sscanf(pcVar8,"%d %f %f %f",&index,&x_1,&y_1,&z_1,pcVar8,pcVar8);
                    if (iVar19 == 4) {
                      object->verts[vertOffset + index][0] = xScale * x_1;
                      object->verts[vertOffset + index][1] = yScale * y_1;
                      object->verts[vertOffset + index][2] = zScale * z_1;
                    }
                  }
                }
                else {
                  faceOffset = object->numFaces;
                  sscanf(pcVar8,"%d",&object->numFaces,pcVar8);
                  iVar19 = faceOffset + object->numFaces;
                  object->numFaces = iVar19;
                  pMVar16 = (MeASEFace *)(*_strchr)(object->faces,iVar19 * 0x5c);
                  object->faces = pMVar16;
                }
              }
              else {
                vertOffset = object->numVerts;
                sscanf(pcVar8,"%d",&object->numVerts,vertOffset);
                iVar19 = vertOffset + object->numVerts;
                object->numVerts = iVar19;
                paMVar17 = (MeVector3 *)(*_strchr)(object->verts,iVar19 * 0xc);
                object->verts = paMVar17;
              }
            }
            else {
              sscanf(pcVar25,"%f %f %f",&x_2,&y_2,&z_2,pcVar27,pcVar8,pcVar26);
            }
          }
          else {
            iVar19 = sscanf(pcVar8,"%f %f %f",&x_3,&y_3,&z_3,pcVar27,pcVar25,pcVar26);
            if (iVar19 == 3) {
              vertexTM[2][0] = x_3;
              vertexTM[2][1] = y_3;
              vertexTM[2][2] = z_3;
            }
          }
        }
        else {
          iVar19 = sscanf(pcVar25,"%f %f %f",&x_4,&y_4,&z_4,pcVar27,pcVar8,pcVar26);
          if (iVar19 == 3) {
            vertexTM[1][0] = x_4;
            vertexTM[1][1] = y_4;
            vertexTM[1][2] = z_4;
          }
        }
      }
      else {
        iVar19 = sscanf(pcVar8,"%f %f %f",&x_5,&y_5,&z_5,pcVar26,MVar10,MVar29);
        if (iVar19 == 3) {
          vertexTM[0][0] = x_5;
          vertexTM[0][1] = y_5;
          vertexTM[0][2] = z_5;
        }
      }
      goto LAB_00010210;
    }
    iVar19 = sscanf(pcVar8,"\"%s\"",norm,uVar18);
    pMVar21 = norm;
    if (iVar19 == 1) {
      do {
        pMVar20 = pMVar21;
        pMVar21 = pMVar20 + 1;
        uVar23 = (int)*pMVar20 + 0xfefefeffU & ((uint)*pMVar20 ^ 0xffffffff);
        uVar24 = uVar23 & 0x80808080;
      } while (uVar24 == 0);
      if ((uVar23 & 0x8080) == 0) {
        uVar24 = uVar24 >> 0x10;
        pMVar21 = (MeReal *)((int)pMVar20 + 6);
      }
      *(char *)((int)pMVar21 +
               (int)norm + (-(int)norm - (uint)CARRY1((byte)uVar24,(byte)uVar24)) + -4) = '\0';
      strncpy(object->name,(char *)norm,0x100);
      gotName = 1;
    }
  } while( true );
  if (asParts != 0) {
                    /* Unresolved local var: MeASEObject * oldObject@[DW_OP_reg3(EBX)] */
    object_00 = (MeASEObject *)(*_MeMemoryAPI)(0x124);
    ASEObjectReset(object_00,materials);
    object_00->nextObject = object;
    object = object_00;
LAB_00011599:
    gotName = 0;
  }
  goto LAB_00010210;
}


/* ==== FindArgs ==== */

char * FindArgs(char *line,char *token)

{
  char *pcVar1;
  char *pcVar2;
  size_t sVar3;
  
                    /* Unresolved local var: char * args@[DW_OP_reg3(EBX)] */
  pcVar1 = strstr(line,token);
  pcVar2 = (char *)0x0;
  if (pcVar1 != (char *)0x0) {
                    /* Unresolved local var: int tokenLen@[???] */
    sVar3 = strlen(token);
    pcVar2 = pcVar1 + sVar3 + 1;
  }
  return pcVar2;
}


/* ==== MeASEObjectLoad ==== */

MeASEObject * MeASEObjectLoad(char *filename,MeReal xScale,MeReal yScale,MeReal zScale)

{
  MeStream fp;
  MeASEObject *pMVar1;
  
                    /* Unresolved local var: MeASEObject * aseObj@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeStream fp@[DW_OP_reg6(ESI)] */
  fp = MeStreamOpenWithSearch(filename,0);
  pMVar1 = (MeASEObject *)0x0;
  if (fp != (MeStream)0x0) {
    pMVar1 = MeASEObjectLoadPartsFromStream(fp,xScale,yScale,zScale,0);
    MeStreamClose(fp);
  }
  return pMVar1;
}


/* ==== MeASEObjectLoadParts ==== */

MeASEObject *
MeASEObjectLoadParts(char *filename,MeReal xScale,MeReal yScale,MeReal zScale,MeBool asParts)

{
  MeStream fp;
  MeASEObject *pMVar1;
  
                    /* Unresolved local var: MeASEObject * aseObj@[DW_OP_reg3(EBX)]
                       Unresolved local var: MeStream fp@[DW_OP_reg6(ESI)] */
  fp = MeStreamOpenWithSearch(filename,0);
  pMVar1 = (MeASEObject *)0x0;
  if (fp != (MeStream)0x0) {
    pMVar1 = MeASEObjectLoadPartsFromStream(fp,xScale,yScale,zScale,asParts);
    MeStreamClose(fp);
  }
  return pMVar1;
}


/* ==== ASEObjectReset ==== */

void ASEObjectReset(MeASEObject *object,MeASEMaterialStore *materials)

{
  object->isLoaded = 0;
                    /* Unresolved local var: void * __s@[DW_OP_reg0(EAX)] */
  object->numVerts = 0;
  object->numFaces = 0;
  object->numUvs = 0;
  object->verts = (MeVector3 *)0x0;
  object->faces = (MeASEFace *)0x0;
  object->uvs = (MeASEUV *)0x0;
  object->nextObject = (MeASEObject *)0x0;
  memset(object->name,0,0x100);
  object->matStore = materials;
  return;
}


