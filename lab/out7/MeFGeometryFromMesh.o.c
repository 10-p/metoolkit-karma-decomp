/* ==== MeFGeometryCreateFromASE ==== */

/* WARNING: Removing unreachable block (ram,0x000100d1) */
/* WARNING: Removing unreachable block (ram,0x000100d7) */
/* WARNING: Removing unreachable block (ram,0x000100e0) */
/* WARNING: Removing unreachable block (ram,0x000100f4) */
/* WARNING: Unknown calling convention */

MeFGeometry *
MeFGeometryCreateFromASE(char *name,char *file,MeReal xScale,MeReal yScale,MeReal zScale)

{
  MeASEObject **ppMVar1;
  int iVar2;
  MeASEObject *pMVar3;
  Mesh2GeometryType MVar4;
  MeFGeometry *pMVar5;
  MeFPrimitive *pMVar6;
  int iVar7;
  undefined4 extraout_ECX;
  undefined4 extraout_EDX;
  undefined4 uVar8;
  undefined4 extraout_EDX_00;
  int iVar9;
  int iVar10;
  char *pcVar11;
  int iVar12;
  int iVar13;
  int aiStack_198 [7];
  MeFPrimitive *partGeometry;
  MeASEObject *headObject;
  MeASEObject *aseObject;
  MeFGeometry *ag;
  char typeName [256];
  MeMatrix4 relTM;
  MyMesh mesh;
  
                    /* Unresolved local var: MeASEObject * tmpAse@[DW_OP_reg0(EAX)]
                       Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int maxFaces@[DW_OP_reg1(ECX)]
                       Unresolved local var: int maxGeom@[???]
                       Unresolved local var: MeBool bFlipX@[???] */
  mesh.faceVertex = (int (*) [3])0x0;
  ag = MeFGeometryCreate(name);
  headObject = MeASEObjectLoadParts(file,xScale,yScale,zScale,1);
  aseObject = headObject;
  pMVar5 = (MeFGeometry *)0x0;
  if (headObject != (MeASEObject *)0x0) {
    iVar7 = 0;
    pMVar3 = headObject;
    do {
      if (iVar7 < pMVar3->numFaces) {
        iVar7 = pMVar3->numFaces;
      }
      pMVar3 = pMVar3->nextObject;
    } while (pMVar3 != (MeASEObject *)0x0);
    iVar7 = -(iVar7 * 0xc + 0xfU & 0xfffffff0);
    mesh.faceVertex = (int (*) [3])((int)aiStack_198 + iVar7 + 0xc);
    pMVar5 = ag;
    if (headObject != (MeASEObject *)0x0) {
      do {
                    /* Unresolved local var: Mesh2GeometryType type@[DW_OP_reg3(EBX)] */
        partGeometry = (MeFPrimitive *)0x0;
        mesh.numFaces = aseObject->numFaces;
        mesh.numVerts = aseObject->numVerts;
        mesh.verts = aseObject->verts;
        iVar10 = 0;
        iVar13 = mesh.numFaces;
        if (0 < mesh.numFaces) {
          iVar9 = 0;
          iVar12 = 0;
          do {
            iVar10 = iVar10 + 1;
            iVar13 = iVar12 + 3;
            (*mesh.faceVertex)[iVar12] = *(int *)((int)aseObject->faces->vertexId + iVar9);
            (*mesh.faceVertex)[iVar12 + 1] = *(int *)((int)aseObject->faces->vertexId + iVar9 + 4);
            iVar2 = iVar9 + 8;
            iVar9 = iVar9 + 0x5c;
            (*mesh.faceVertex)[iVar12 + 2] = *(int *)((int)aseObject->faces->vertexId + iVar2);
            iVar12 = iVar13;
          } while (iVar10 < mesh.numFaces);
        }
        pcVar11 = aseObject->name;
        *(char **)(&stack0xfffffe64 + iVar7) = pcVar11;
        *(undefined4 *)((int)aiStack_198 + iVar7 + -8) = 0x1016d;
        MVar4 = name2Type(*(char **)(&stack0xfffffe64 + iVar7));
        if (MVar4 != kMesh2GeometryUnknown) {
          pMVar6 = (MeFPrimitive *)typeName;
          if (MVar4 == kMesh2GeometrySphere) {
            *(int *)((int)aiStack_198 + iVar7 + 8) = iVar13;
            *(undefined4 *)((int)aiStack_198 + iVar7 + 4) = 0x100;
            *(char **)((int)aiStack_198 + iVar7) = "McdSphere";
LAB_0001029f:
            *(MeFPrimitive **)(&stack0xfffffe64 + iVar7) = pMVar6;
            *(undefined4 *)((int)aiStack_198 + iVar7 + -8) = 0x102a5;
            pMVar6 = (MeFPrimitive *)
                     strncpy(*(char **)(&stack0xfffffe64 + iVar7),
                             *(char **)((int)aiStack_198 + iVar7),
                             *(size_t *)((int)aiStack_198 + iVar7 + 4));
            uVar8 = extraout_EDX_00;
          }
          else {
            if (MVar4 == kMesh2GeometryBox) {
              *(undefined4 *)((int)aiStack_198 + iVar7 + 8) = extraout_ECX;
              *(undefined4 *)((int)aiStack_198 + iVar7 + 4) = 0x100;
              *(char **)((int)aiStack_198 + iVar7) = "McdBox";
              goto LAB_0001029f;
            }
            if (MVar4 == kMesh2GeometryCylinder) {
              *(undefined4 *)((int)aiStack_198 + iVar7 + 8) = extraout_EDX;
              *(undefined4 *)((int)aiStack_198 + iVar7 + 4) = 0x100;
              *(char **)((int)aiStack_198 + iVar7) = "McdCylinder";
              goto LAB_0001029f;
            }
            if (MVar4 == kMesh2GeometryConvex) {
              *(int *)((int)aiStack_198 + iVar7 + 8) = iVar13;
              *(undefined4 *)((int)aiStack_198 + iVar7 + 4) = 0x100;
              *(char **)((int)aiStack_198 + iVar7) = "McdConvexHull";
              goto LAB_0001029f;
            }
            uVar8 = extraout_EDX;
            if (MVar4 == kMesh2GeometryUnknown) {
              *(undefined4 *)((int)aiStack_198 + iVar7 + 8) = extraout_ECX;
              *(undefined4 *)((int)aiStack_198 + iVar7 + 4) = 0x100;
              *(char **)((int)aiStack_198 + iVar7) = "Unknown";
              goto LAB_0001029f;
            }
          }
          if (MVar4 == kMesh2GeometrySphere) {
            *(undefined4 *)((int)aiStack_198 + iVar7 + 8) = uVar8;
            *(MeReal (**) [4])((int)aiStack_198 + iVar7 + 4) = relTM;
            *(MyMesh **)((int)aiStack_198 + iVar7) = &mesh;
            *(char **)(&stack0xfffffe64 + iVar7) = pcVar11;
            *(undefined4 *)((int)aiStack_198 + iVar7 + -8) = 0x10292;
            pMVar6 = MeFSphereCreateFromMesh
                               (*(char **)(&stack0xfffffe64 + iVar7),
                                *(MyMesh **)((int)aiStack_198 + iVar7),
                                *(MeMatrix4Ptr *)((int)aiStack_198 + iVar7 + 4));
LAB_00010253:
            partGeometry = pMVar6;
          }
          else {
            if (MVar4 == kMesh2GeometryBox) {
              *(MeFPrimitive **)((int)aiStack_198 + iVar7 + 8) = pMVar6;
              *(MeReal (**) [4])((int)aiStack_198 + iVar7 + 4) = relTM;
              *(MyMesh **)((int)aiStack_198 + iVar7) = &mesh;
              *(char **)(&stack0xfffffe64 + iVar7) = pcVar11;
              *(undefined4 *)((int)aiStack_198 + iVar7 + -8) = 0x10281;
              pMVar6 = MeFBoxCreateFromMesh
                                 (*(char **)(&stack0xfffffe64 + iVar7),
                                  *(MyMesh **)((int)aiStack_198 + iVar7),
                                  *(MeMatrix4Ptr *)((int)aiStack_198 + iVar7 + 4));
              goto LAB_00010253;
            }
            if (MVar4 == kMesh2GeometryCylinder) {
              *(MeFPrimitive **)((int)aiStack_198 + iVar7 + 8) = pMVar6;
              *(MeReal (**) [4])((int)aiStack_198 + iVar7 + 4) = relTM;
              *(MyMesh **)((int)aiStack_198 + iVar7) = &mesh;
              *(char **)(&stack0xfffffe64 + iVar7) = pcVar11;
              *(undefined4 *)((int)aiStack_198 + iVar7 + -8) = 0x10270;
              pMVar6 = MeFCylinderCreateFromMesh
                                 (*(char **)(&stack0xfffffe64 + iVar7),
                                  *(MyMesh **)((int)aiStack_198 + iVar7),
                                  *(MeMatrix4Ptr *)((int)aiStack_198 + iVar7 + 4));
              goto LAB_00010253;
            }
            if (MVar4 == kMesh2GeometryConvex) {
              *(MeFPrimitive **)((int)aiStack_198 + iVar7 + 8) = pMVar6;
              *(MeReal (**) [4])((int)aiStack_198 + iVar7 + 4) = relTM;
              *(MyMesh **)((int)aiStack_198 + iVar7) = &mesh;
              *(char **)(&stack0xfffffe64 + iVar7) = pcVar11;
              *(undefined4 *)((int)aiStack_198 + iVar7 + -8) = 0x10253;
              pMVar6 = MeFConvexCreateFromMesh
                                 (*(char **)(&stack0xfffffe64 + iVar7),
                                  *(MyMesh **)((int)aiStack_198 + iVar7),
                                  *(MeMatrix4Ptr *)((int)aiStack_198 + iVar7 + 4));
              goto LAB_00010253;
            }
          }
          if (partGeometry != (MeFPrimitive *)0x0) {
            *(MeFPrimitive **)((int)aiStack_198 + iVar7 + 8) = pMVar6;
            *(MeFPrimitive **)((int)aiStack_198 + iVar7 + 4) = pMVar6;
            *(MeReal (**) [4])((int)aiStack_198 + iVar7) = relTM;
            *(MeFPrimitive **)(&stack0xfffffe64 + iVar7) = partGeometry;
            *(undefined4 *)((int)aiStack_198 + iVar7 + -8) = 0x101e8;
            MeFPrimitiveSetTransform
                      (*(void **)(&stack0xfffffe64 + iVar7),*(void **)((int)aiStack_198 + iVar7));
            pMVar5 = ag;
            *(MeFPrimitive **)((int)aiStack_198 + iVar7) = partGeometry;
            *(MeFGeometry **)(&stack0xfffffe64 + iVar7) = pMVar5;
            *(undefined4 *)((int)aiStack_198 + iVar7 + -8) = 0x101fd;
            MeFGeometryInsertPrimitive
                      (*(void **)(&stack0xfffffe64 + iVar7),*(void **)((int)aiStack_198 + iVar7));
          }
        }
        ppMVar1 = &aseObject->nextObject;
        aseObject = *ppMVar1;
      } while (*ppMVar1 != (MeASEObject *)0x0);
      pMVar5 = ag;
      if (headObject != (MeASEObject *)0x0) {
        *(MeASEObject **)(&stack0xfffffe64 + iVar7) = headObject;
        *(undefined4 *)((int)aiStack_198 + iVar7 + -8) = 0x10233;
        MeASEObjectDestroy(*(void **)(&stack0xfffffe64 + iVar7));
        pMVar5 = ag;
      }
    }
  }
  return pMVar5;
}


/* ==== MeFSphereCreateFromMesh ==== */

/* WARNING: Unknown calling convention */

MeFPrimitive * MeFSphereCreateFromMesh(char *name,MyMesh *mesh,MeMatrix4Ptr relTM)

{
  float fVar1;
  float fVar2;
  float fVar3;
  int iVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  MeFPrimitive *pMVar11;
  float *pfVar12;
  uint uVar13;
  MeVector3 *paMVar14;
  int iVar15;
  int iVar16;
  char *pcVar17;
  MeFPrimitive *fgeom;
  MeReal radius;
  MeVector3 ctov;
  MyAABB bbox;
  
                    /* Unresolved local var: MeReal longest@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal shortest@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal maxr@[DW_OP_reg14(ST3)]
                       Unresolved local var: MeReal minr@[DW_OP_reg18(ST7)]
                       Unresolved local var: int i@[DW_OP_reg1(ECX)] */
  iVar4 = mesh->numVerts;
  if (iVar4 == 0) {
    return (MeFPrimitive *)0x0;
  }
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)]
                       Unresolved local var: int j@[DW_OP_reg2(EDX)] */
  paMVar14 = mesh->verts;
  bbox.min[0] = (*paMVar14)[0];
  bbox.min[1] = (*paMVar14)[1];
  bbox.min[2] = (*paMVar14)[2];
  paMVar14 = mesh->verts;
  bbox.max[0] = (*paMVar14)[0];
  bbox.max[1] = (*paMVar14)[1];
  bbox.max[2] = (*paMVar14)[2];
  if (1 < iVar4) {
    iVar16 = 0xc;
    iVar15 = 1;
    do {
      pfVar12 = (float *)((int)*paMVar14 + iVar16);
      fVar1 = *pfVar12;
      if (fVar1 < bbox.max[0]) {
        fVar1 = bbox.max[0];
      }
      if (*pfVar12 < bbox.min[0]) {
        bbox.min[0] = *pfVar12;
      }
      fVar2 = pfVar12[1];
      if (fVar2 < bbox.max[1]) {
        fVar2 = bbox.max[1];
      }
      if (pfVar12[1] < bbox.min[1]) {
        bbox.min[1] = pfVar12[1];
      }
      fVar3 = pfVar12[2];
      if (fVar3 < bbox.max[2]) {
        fVar3 = bbox.max[2];
      }
      if (pfVar12[2] < bbox.min[2]) {
        bbox.min[2] = pfVar12[2];
      }
      iVar15 = iVar15 + 1;
      iVar16 = iVar16 + 0xc;
      bbox.max[0] = fVar1;
      bbox.max[1] = fVar2;
      bbox.max[2] = fVar3;
    } while (iVar15 < iVar4);
  }
  fVar1 = bbox.max[0] - bbox.min[0];
  fVar5 = bbox.max[1] - bbox.min[1];
  fVar2 = bbox.min[0] + fVar1 * 0.5;
  fVar3 = bbox.max[2] - bbox.min[2];
  fVar8 = fVar5 * 0.5 + bbox.min[1];
  fVar7 = fVar3 * 0.5 + bbox.min[2];
  fVar6 = fVar1;
  if (fVar1 < fVar5) {
    fVar6 = fVar5;
  }
  if (fVar6 < fVar3) {
    fVar6 = fVar3;
  }
  if (fVar1 < fVar5) {
    fVar5 = fVar1;
  }
  if (fVar5 < fVar3) {
    fVar3 = fVar5;
  }
  if (0.05 < (fVar6 - fVar3) / fVar6) {
    pcVar17 = "ERROR: Sphere bounding box not square.";
    goto LAB_00010716;
  }
  relTM[3][0] = 0.0;
  (*relTM)[0] = 1.0;
  (*relTM)[1] = 0.0;
  (*relTM)[2] = 0.0;
  (*relTM)[3] = 0.0;
  relTM[1][0] = 0.0;
  relTM[1][1] = 1.0;
  relTM[1][2] = 0.0;
  relTM[1][3] = 0.0;
  relTM[2][0] = 0.0;
  relTM[2][1] = 0.0;
  relTM[2][2] = 1.0;
  relTM[2][3] = 0.0;
  relTM[3][1] = 0.0;
  relTM[3][2] = 0.0;
  relTM[3][3] = 1.0;
  fVar1 = 3.4028235e+38;
  relTM[3][0] = fVar2;
  relTM[3][1] = fVar8;
  relTM[3][2] = fVar7;
  uVar13 = mesh->numVerts;
  fVar3 = 0.0;
  if ((int)uVar13 < 1) {
    fVar3 = 0.0;
    fVar1 = 3.4028235e+38;
  }
  else {
    paMVar14 = mesh->verts;
    if ((uVar13 & 1) != 0) {
                    /* Unresolved local var: MeReal rsqr@[???] */
      fVar5 = (*paMVar14)[0] - fVar2;
                    /* Unresolved local var: MeReal m@[DW_OP_reg13(ST2)] */
      fVar10 = (*paMVar14)[1] - fVar8;
      fVar9 = (*paMVar14)[2] - fVar7;
      fVar5 = fVar9 * fVar9 + fVar5 * fVar5 + fVar10 * fVar10;
      fVar10 = fVar5;
      if (fVar5 < 0.0) {
        fVar10 = fVar3;
      }
      fVar3 = fVar10;
      if ((0.001 < fVar5) && (fVar1 = 3.4028235e+38, fVar5 < 3.4028235e+38)) {
        fVar1 = fVar5;
      }
      paMVar14 = paMVar14 + 1;
      uVar13 = uVar13 - 1;
      if (uVar13 == 0) goto LAB_000106f2;
    }
    do {
      fVar10 = (*paMVar14)[0] - fVar2;
      fVar5 = (*paMVar14)[1] - fVar8;
      fVar9 = (*paMVar14)[2] - fVar7;
      fVar10 = fVar9 * fVar9 + fVar10 * fVar10 + fVar5 * fVar5;
      fVar5 = fVar10;
      if (fVar10 < fVar3) {
        fVar5 = fVar3;
      }
      if ((0.001 < fVar10) && (fVar10 < fVar1)) {
        fVar1 = fVar10;
      }
      fVar10 = paMVar14[1][0] - fVar2;
      fVar3 = paMVar14[1][1] - fVar8;
      fVar9 = paMVar14[1][2] - fVar7;
      fVar10 = fVar9 * fVar9 + fVar10 * fVar10 + fVar3 * fVar3;
      fVar3 = fVar10;
      if (fVar10 < fVar5) {
        fVar3 = fVar5;
      }
      if ((0.001 < fVar10) && (fVar10 < fVar1)) {
        fVar1 = fVar10;
      }
      paMVar14 = paMVar14 + 2;
      uVar13 = uVar13 - 2;
    } while (uVar13 != 0);
  }
LAB_000106f2:
  if ((1.0 / (fVar6 * 0.5)) * (SQRT(fVar3) - SQRT(fVar1)) <= 0.05) {
    pMVar11 = MeFPrimitiveCreate(name,1);
    if (pMVar11 == (MeFPrimitive *)0x0) {
      return (MeFPrimitive *)0x0;
    }
    MeFPrimitiveSetRadius(pMVar11,fVar6 * 0.5);
    return pMVar11;
  }
  pcVar17 = "ERROR: Vertices not at constant radius.";
LAB_00010716:
  MeWarning(0,pcVar17);
  return (MeFPrimitive *)0x0;
}


/* ==== MeFBoxCreateFromMesh ==== */

/* WARNING: Unknown calling convention */

MeFPrimitive * MeFBoxCreateFromMesh(char *name,MyMesh *mesh,MeMatrix4Ptr relTM)

{
  MeVector3 *paMVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int (*paiVar8) [3];
  ushort uVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  float fVar13;
  MeReal *pMVar14;
  MeReal (*paMVar15) [2];
  MeReal MVar16;
  MeReal MVar17;
  MeReal MVar18;
  byte bVar23;
  int in_EAX;
  int *piVar19;
  undefined4 uVar20;
  float *pfVar21;
  MeFPrimitive *pMVar22;
  int iVar25;
  MeVector3 *paMVar26;
  MeVector3 *paMVar27;
  bool bVar28;
  ushort uVar29;
  int aiStack_130 [2];
  float afStack_128 [3];
  MeReal aMStack_11c [3];
  int local_110;
  float *local_104;
  int (*local_100) [3];
  int local_e8;
  MeReal *local_e4;
  int local_e0;
  int (*local_dc) [3];
  int local_d8;
  float local_d4;
  float local_d0;
  float local_c4;
  float local_c0;
  MeVector3 *local_b4;
  float local_b0;
  MeBool foundPlane;
  MeFPrimitive *boxgeom;
  int i;
  int numPlanes;
  int *planeDistCount;
  MeReal (*planeDist) [2];
  int local_94;
  MeVector3 *planeNormal;
  MeVector3 normal;
  MeVector3 e2;
  MeVector3 e1;
  MeVector3 size;
  MyAABB bbox;
  undefined2 uVar24;
  
                    /* Unresolved local var: int j@[DW_OP_reg1(ECX)] */
  local_94 = mesh->numFaces;
  boxgeom = (MeFPrimitive *)0x0;
  iVar10 = -(local_94 * 0xc + 0xfU & 0xfffffff0);
  planeNormal = (MeVector3 *)((int)aMStack_11c + iVar10);
  iVar11 = -(local_94 * 8 + 0xfU & 0xfffffff0);
  planeDist = (MeReal (*) [2])((int)aMStack_11c + iVar11 + iVar10);
  iVar12 = -(local_94 * 4 + 0xfU & 0xfffffff0);
  planeDistCount = (int *)((int)aMStack_11c + iVar12 + iVar11 + iVar10);
  numPlanes = 0;
  i = 0;
  if (0 < local_94) {
    local_dc = mesh->faceVertex;
    paMVar27 = mesh->verts;
    local_e4 = (MeReal *)((int)aMStack_11c + iVar10);
    local_e0 = 0;
    do {
      pMVar14 = local_e4;
      piVar19 = (int *)((int)*local_dc + local_e0);
      foundPlane = 0;
      paMVar26 = paMVar27 + piVar19[1];
      paMVar27 = paMVar27 + *piVar19;
      fVar2 = (*paMVar26)[0] - (*paMVar27)[0];
      fVar3 = (*paMVar26)[1] - (*paMVar27)[1];
      fVar6 = (*paMVar26)[2] - (*paMVar27)[2];
      paiVar8 = mesh->faceVertex;
      local_100 = paiVar8;
      local_dc = paiVar8;
      piVar19 = (int *)((int)*paiVar8 + local_e0);
      paMVar27 = mesh->verts;
      local_b4 = paMVar27;
      paMVar26 = paMVar27 + piVar19[2];
      paMVar1 = paMVar27 + *piVar19;
      fVar7 = (*paMVar26)[0] - (*paMVar1)[0];
      fVar5 = (*paMVar26)[1] - (*paMVar1)[1];
      fVar13 = (*paMVar26)[2] - (*paMVar1)[2];
      fVar4 = fVar3 * fVar13 - fVar6 * fVar5;
      fVar6 = fVar6 * fVar7 - fVar13 * fVar2;
      local_b0 = fVar5 * fVar2 - fVar3 * fVar7;
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg12(ST1)]
                       Unresolved local var: MeReal k@[DW_OP_reg11(ST0)] */
      fVar2 = local_b0 * local_b0 + fVar6 * fVar6 + fVar4 * fVar4;
      if (fVar2 <= 0.0) {
        normal[0] = 1.0;
        normal[1] = 0.0;
        normal[2] = 0.0;
      }
      else {
        fVar2 = 1.0 / SQRT(fVar2);
        normal[0] = fVar4 * fVar2;
        normal[1] = fVar6 * fVar2;
        normal[2] = fVar2 * local_b0;
      }
      MVar16 = normal[0];
      iVar25 = 0;
      paMVar26 = planeNormal;
      if (numPlanes < 1) {
LAB_00010f58:
        if (foundPlane == 0) {
          *local_e4 = normal[0];
          MVar17 = normal[1];
          pMVar14[1] = normal[1];
          MVar18 = normal[2];
          pMVar14[2] = normal[2];
          paMVar26 = local_b4 + *(int *)((int)*local_100 + local_e0);
          planeDist[numPlanes][0] =
               MVar18 * (*paMVar26)[2] + MVar16 * (*paMVar26)[0] + MVar17 * (*paMVar26)[1];
          planeDistCount[numPlanes] = 1;
          numPlanes = numPlanes + 1;
          local_e4 = local_e4 + 3;
        }
      }
      else {
        do {
                    /* Unresolved local var: MeReal dot@[DW_OP_reg12(ST1)] */
          fVar2 = normal[2] * (*paMVar26)[2] +
                  normal[1] * (*paMVar26)[1] + normal[0] * (*paMVar26)[0];
          local_c0 = ABS(fVar2 - 1.0);
          if (local_c0 < 0.001) {
LAB_00010a72:
            local_104 = (float *)0x1;
          }
          else {
            local_104 = (float *)0x0;
            local_c4 = ABS(-1.0 - fVar2);
            if (local_c4 < 0.001) goto LAB_00010a72;
          }
          if (local_104 != (float *)0x0) {
                    /* Unresolved local var: MeReal dist@[DW_OP_reg13(ST2)] */
            local_110 = *(int *)((int)*paiVar8 + local_e0);
            paMVar1 = paMVar27 + local_110;
            foundPlane = 1;
            local_104 = (float *)planeDistCount[iVar25];
            fVar2 = (*paMVar26)[2] * (*paMVar1)[2] +
                    (*paMVar26)[0] * (*paMVar1)[0] + (*paMVar26)[1] * (*paMVar1)[1];
            if (local_104 == (float *)0x1) {
              fVar3 = planeDist[iVar25][0];
              if (0.001 <= ABS(fVar3)) {
                bVar23 = ABS((1.0 / fVar3) * (fVar2 - fVar3)) < 0.001;
              }
              else {
                bVar23 = ABS(fVar2) < 0.001 | (byte)((ushort)((ushort)NAN(ABS(fVar2)) << 10) >> 8);
              }
              if (bVar23 == 0) {
                planeDist[iVar25][1] = fVar2;
                planeDistCount[iVar25] = planeDistCount[iVar25] + 1;
                goto LAB_00010af2;
              }
            }
            if (local_104 == (float *)0x2) {
              fVar3 = planeDist[iVar25][1];
              if (0.001 <= ABS(fVar3)) {
                fVar2 = ABS((1.0 / fVar3) * (fVar2 - fVar3));
                uVar29 = (ushort)(fVar2 < 0.001) << 8 | (ushort)NAN(fVar2) << 10 |
                         (ushort)(fVar2 == 0.001) << 0xe;
                bVar28 = fVar2 >= 0.001;
              }
              else {
                fVar2 = ABS(fVar2);
                uVar9 = (ushort)(fVar2 < 0.001) << 8 | (ushort)NAN(fVar2) << 10;
                uVar29 = uVar9 | (ushort)(fVar2 == 0.001) << 0xe;
                bVar28 = uVar9 == 0;
              }
              uVar20 = CONCAT22((short)((uint)planeDist >> 0x10),uVar29);
              if (bVar28) {
                *(undefined4 *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 8) = uVar20;
                *(undefined4 *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 4) = uVar20;
                *(char **)((int)afStack_128 + iVar12 + iVar11 + iVar10) =
                     "ERROR: Found more than 2 planes with different distances.";
                goto LAB_00010b56;
              }
            }
          }
LAB_00010af2:
          iVar25 = iVar25 + 1;
          if (numPlanes <= iVar25) goto LAB_00010f58;
          paMVar26 = paMVar26 + 1;
        } while (foundPlane == 0);
      }
      i = i + 1;
      local_e0 = local_e0 + 0xc;
      in_EAX = local_94;
    } while (i < local_94);
  }
  paMVar27 = planeNormal;
  if (numPlanes != 3) {
    *(int *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 8) = in_EAX;
    *(int *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 4) = in_EAX;
    *(char **)((int)afStack_128 + iVar12 + iVar11 + iVar10) =
         "ERROR: Not very box-like (need 3 sets of planes).";
    goto LAB_00010b56;
  }
  if (((*planeDistCount != 2) || (planeDistCount[1] != 2)) || (planeDistCount[2] != 2)) {
    *(int *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 8) = in_EAX;
    *(int *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 4) = in_EAX;
    *(char **)((int)afStack_128 + iVar12 + iVar11 + iVar10) =
         "ERROR: Incomplete set of planes (need 2 per axis).";
    goto LAB_00010b56;
  }
  (*relTM)[0] = 1.0;
  (*relTM)[1] = 0.0;
  (*relTM)[2] = 0.0;
  relTM[1][0] = 0.0;
  (*relTM)[3] = 0.0;
  relTM[1][1] = 1.0;
  relTM[1][2] = 0.0;
  relTM[1][3] = 0.0;
  relTM[2][0] = 0.0;
  relTM[2][1] = 0.0;
  relTM[2][2] = 1.0;
  relTM[2][3] = 0.0;
  relTM[3][0] = 0.0;
  relTM[3][1] = 0.0;
  relTM[3][2] = 0.0;
  relTM[3][3] = 1.0;
  fVar2 = (*planeNormal)[0];
  (*relTM)[0] = fVar2;
  local_d0 = (*planeNormal)[1];
  (*relTM)[1] = local_d0;
  fVar3 = (*planeNormal)[2];
  (*relTM)[2] = fVar3;
  fVar4 = planeNormal[1][0];
  relTM[1][0] = fVar4;
  fVar5 = planeNormal[1][1];
  relTM[1][1] = fVar5;
  fVar6 = planeNormal[1][2];
  relTM[1][2] = fVar6;
  local_d4 = local_d0 * fVar6;
  relTM[2][0] = local_d0 * fVar6 - fVar3 * fVar5;
  relTM[2][1] = fVar3 * fVar4 - fVar6 * fVar2;
  relTM[2][2] = fVar2 * fVar5 - local_d0 * fVar4;
  *(MeVector3 **)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 8) = planeNormal;
  *(MeVector3 **)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 4) = paMVar27;
  *(undefined4 *)((int)afStack_128 + iVar12 + iVar11 + iVar10) = 0x3a83126f;
  *(MeMatrix4Ptr *)((int)aiStack_130 + iVar12 + iVar11 + iVar10 + 4) = relTM;
  *(undefined4 *)((int)aiStack_130 + iVar12 + iVar11 + iVar10) = 0x10c95;
  iVar25 = MeMatrix4IsTM(*(void **)((int)aiStack_130 + iVar12 + iVar11 + iVar10 + 4),
                         *(float *)((int)afStack_128 + iVar12 + iVar11 + iVar10));
  paMVar15 = planeDist;
  uVar20 = 0;
  if (iVar25 != 0) {
                    /* Unresolved local var: MeReal dot@[DW_OP_reg15(ST4)] */
    fVar2 = planeNormal[2][2] * relTM[2][2] +
            planeNormal[2][1] * relTM[2][1] + planeNormal[2][0] * relTM[2][0];
    fVar3 = ABS(fVar2 - 1.0);
    uVar24 = (undefined2)((uint)planeNormal >> 0x10);
    uVar20 = CONCAT22(uVar24,(ushort)(fVar3 < 0.001) << 8 | (ushort)NAN(fVar3) << 10 |
                             (ushort)(fVar3 == 0.001) << 0xe);
    if (fVar3 < 0.001) {
LAB_00010cf7:
      bVar28 = true;
    }
    else {
      bVar28 = false;
      fVar2 = ABS(-1.0 - fVar2);
      uVar29 = (ushort)(fVar2 < 0.001) << 8 | (ushort)NAN(fVar2) << 10;
      uVar20 = CONCAT22(uVar24,uVar29 | (ushort)(fVar2 == 0.001) << 0xe);
      if (uVar29 != 0) goto LAB_00010cf7;
    }
    if (bVar28) {
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)]
                       Unresolved local var: int j@[DW_OP_reg2(EDX)] */
      paMVar27 = mesh->verts;
      bbox.min[0] = (*paMVar27)[0];
      bbox.min[1] = (*paMVar27)[1];
      bbox.min[2] = (*paMVar27)[2];
      paMVar27 = mesh->verts;
      iVar25 = 1;
      local_d8 = mesh->numVerts;
      bbox.max[0] = (*paMVar27)[0];
      bbox.max[1] = (*paMVar27)[1];
      bbox.max[2] = (*paMVar27)[2];
      if (1 < local_d8) {
        local_e8 = 0xc;
        do {
          pfVar21 = (float *)((int)*paMVar27 + local_e8);
          fVar2 = *pfVar21;
          if (fVar2 < bbox.max[0]) {
            fVar2 = bbox.max[0];
          }
          if (*pfVar21 < bbox.min[0]) {
            bbox.min[0] = *pfVar21;
          }
          fVar3 = pfVar21[1];
          if (fVar3 < bbox.max[1]) {
            fVar3 = bbox.max[1];
          }
          if (pfVar21[1] < bbox.min[1]) {
            bbox.min[1] = pfVar21[1];
          }
          local_104 = pfVar21 + 2;
          fVar4 = pfVar21[2];
          if (fVar4 < bbox.max[2]) {
            fVar4 = bbox.max[2];
          }
          fVar5 = pfVar21[2];
          if (fVar5 < bbox.min[2]) {
            bbox.min[2] = fVar5;
          }
          iVar25 = iVar25 + 1;
          local_e8 = local_e8 + 0xc;
          bbox.max[0] = fVar2;
          bbox.max[1] = fVar3;
          bbox.max[2] = fVar4;
        } while (iVar25 < local_d8);
      }
      relTM[3][0] = bbox.min[0] + (bbox.max[0] - bbox.min[0]) * 0.5;
      relTM[3][1] = bbox.min[1] + (bbox.max[1] - bbox.min[1]) * 0.5;
      relTM[3][2] = bbox.min[2] + (bbox.max[2] - bbox.min[2]) * 0.5;
      fVar2 = (*planeDist)[1];
      fVar3 = (*planeDist)[0];
      fVar4 = planeDist[1][1];
      fVar5 = planeDist[1][0];
      fVar6 = planeDist[2][1];
      fVar7 = planeDist[2][0];
      *(MeReal (**) [2])((int)afStack_128 + iVar12 + iVar11 + iVar10 + 8) = planeDist;
      *(MeReal (**) [2])((int)afStack_128 + iVar12 + iVar11 + iVar10 + 4) = paMVar15;
      *(undefined4 *)((int)afStack_128 + iVar12 + iVar11 + iVar10) = 2;
      *(char **)((int)aiStack_130 + iVar12 + iVar11 + iVar10 + 4) = name;
      *(undefined4 *)((int)aiStack_130 + iVar12 + iVar11 + iVar10) = 0x10ef5;
      pMVar22 = MeFPrimitiveCreate(*(void **)((int)aiStack_130 + iVar12 + iVar11 + iVar10 + 4),
                                   *(int *)((int)afStack_128 + iVar12 + iVar11 + iVar10));
      boxgeom = pMVar22;
      if (pMVar22 == (MeFPrimitive *)0x0) {
        return (MeFPrimitive *)0x0;
      }
      *(float *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 8) = ABS(fVar7 - fVar6);
      *(float *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 4) = ABS(fVar5 - fVar4);
      *(float *)((int)afStack_128 + iVar12 + iVar11 + iVar10) = ABS(fVar3 - fVar2);
      *(MeFPrimitive **)((int)aiStack_130 + iVar12 + iVar11 + iVar10 + 4) = pMVar22;
      *(undefined4 *)((int)aiStack_130 + iVar12 + iVar11 + iVar10) = 0x10f18;
      MeFPrimitiveSetDimensions
                (*(void **)((int)aiStack_130 + iVar12 + iVar11 + iVar10 + 4),
                 *(float *)((int)afStack_128 + iVar12 + iVar11 + iVar10),
                 *(float *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 4),
                 *(float *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 8));
      return boxgeom;
    }
  }
  *(undefined4 *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 8) = uVar20;
  *(undefined4 *)((int)afStack_128 + iVar12 + iVar11 + iVar10 + 4) = uVar20;
  *(char **)((int)afStack_128 + iVar12 + iVar11 + iVar10) = "ERROR: Box axes are not perpendicular."
  ;
LAB_00010b56:
  *(undefined4 *)((int)aiStack_130 + iVar12 + iVar11 + iVar10 + 4) = 0;
  *(undefined4 *)((int)aiStack_130 + iVar12 + iVar11 + iVar10) = 0x10b5d;
  MeWarning(*(int *)((int)aiStack_130 + iVar12 + iVar11 + iVar10 + 4),
            *(void **)((int)afStack_128 + iVar12 + iVar11 + iVar10));
  return boxgeom;
}


/* ==== MeFCylinderCreateFromMesh ==== */

/* WARNING: Unknown calling convention */

MeFPrimitive * MeFCylinderCreateFromMesh(char *name,MyMesh *mesh,MeMatrix4Ptr relTM)

{
  float *pfVar1;
  float fVar2;
  float fVar3;
  ushort uVar4;
  int iVar5;
  int iVar6;
  float fVar7;
  bool bVar8;
  uint uVar9;
  undefined2 uVar12;
  undefined4 uVar10;
  MeFPrimitive *pMVar11;
  MeVector3 *paMVar13;
  MeVector3 *paMVar14;
  int iVar15;
  int iVar16;
  float fVar17;
  float fVar18;
  MeReal (*paMVar19) [3];
  bool bVar20;
  ushort uVar21;
  int aiStack_120 [2];
  float fStack_118;
  uint auStack_114 [2];
  MeReal MStack_10c;
  MeVector3 *local_108;
  MeVector3 *local_104;
  float local_100;
  int local_f8;
  int (*local_f4) [3];
  MeReal *local_f0;
  MeVector3 *local_ec;
  float local_e8;
  float __result;
  MeVector3 *local_e0;
  int i_1;
  MeReal minlen;
  MeReal maxlen;
  MeReal minr;
  int axisIx;
  int j;
  int i;
  MeFPrimitive *cylgeom;
  int numDirs;
  int *dirCount;
  int local_b4;
  MeVector3 *dir;
  MeVector3 toClosest;
  MeVector3 relOrigin;
  MeReal edge [3] [3];
  MeVector3 cylAxis;
  MyAABB bbox;
  
                    /* Unresolved local var: int k@[DW_OP_reg3(EBX)]
                       Unresolved local var: int mostCommonCount@[DW_OP_reg7(EDI)]
                       Unresolved local var: int nextMostCommonCount@[DW_OP_reg1(ECX)]
                       Unresolved local var: MeReal maxr@[DW_OP_reg18(ST7)] */
  local_b4 = mesh->numFaces;
  cylgeom = (MeFPrimitive *)0x0;
  iVar5 = -(local_b4 * 0x24 + 0xfU & 0xfffffff0);
  dir = (MeVector3 *)((int)&MStack_10c + iVar5);
  numDirs = 0;
  paMVar14 = (MeVector3 *)(local_b4 * 0xc + 0xfU & 0xfffffff0);
  iVar6 = -(int)paMVar14;
  dirCount = (int *)((int)&MStack_10c + iVar6 + iVar5);
  i = 0;
  if (0 < local_b4) {
    local_ec = mesh->verts;
    local_f4 = mesh->faceVertex;
    local_f8 = 0;
    do {
      paMVar14 = local_ec + (*local_f4)[1];
      paMVar13 = local_ec + (*local_f4)[0];
      fVar18 = (*paMVar14)[0] - (*paMVar13)[0];
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
      fVar2 = (*paMVar14)[1] - (*paMVar13)[1];
      fVar7 = (*paMVar14)[2] - (*paMVar13)[2];
      fVar3 = fVar7 * fVar7 + fVar2 * fVar2 + fVar18 * fVar18;
      if (fVar3 <= 0.0) {
        edge[0][0] = 1.0;
        edge[0][1] = 0.0;
        edge[0][2] = 0.0;
      }
      else {
        fVar3 = 1.0 / SQRT(fVar3);
        edge[0][0] = fVar18 * fVar3;
        edge[0][1] = fVar2 * fVar3;
        edge[0][2] = fVar3 * fVar7;
      }
      paMVar14 = local_ec + (*local_f4)[2];
      paMVar13 = local_ec + (*local_f4)[0];
      fVar2 = (*paMVar14)[0] - (*paMVar13)[0];
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
      fVar3 = (*paMVar14)[1] - (*paMVar13)[1];
      fVar7 = (*paMVar14)[2] - (*paMVar13)[2];
      fVar18 = fVar7 * fVar7 + fVar3 * fVar3 + fVar2 * fVar2;
      if (fVar18 <= 0.0) {
        edge[1][0] = 1.0;
        edge[1][1] = 0.0;
        edge[1][2] = 0.0;
      }
      else {
        fVar18 = 1.0 / SQRT(fVar18);
        edge[1][0] = fVar2 * fVar18;
        edge[1][1] = fVar3 * fVar18;
        edge[1][2] = fVar18 * fVar7;
      }
      paMVar14 = local_ec + (*local_f4)[2];
      paMVar13 = local_ec + (*local_f4)[1];
      fVar2 = (*paMVar14)[0] - (*paMVar13)[0];
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
      fVar3 = (*paMVar14)[1] - (*paMVar13)[1];
      fVar7 = (*paMVar14)[2] - (*paMVar13)[2];
      fVar18 = fVar7 * fVar7 + fVar3 * fVar3 + fVar2 * fVar2;
      if (fVar18 <= 0.0) {
        edge[2][0] = 1.0;
        edge[2][1] = 0.0;
        edge[2][2] = 0.0;
      }
      else {
        fVar18 = 1.0 / SQRT(fVar18);
        edge[2][0] = fVar2 * fVar18;
        edge[2][1] = fVar3 * fVar18;
        edge[2][2] = fVar18 * fVar7;
      }
      paMVar19 = edge;
      j = 0;
      local_f0 = (MeReal *)((int)*dir + local_f8);
      paMVar14 = dir;
      do {
                    /* Unresolved local var: MeBool dirFound@[DW_OP_reg7(EDI)] */
        iVar15 = 0;
        bVar20 = false;
        if (numDirs < 1) {
LAB_00011d06:
          if (!bVar20) {
            *local_f0 = (*paMVar19)[0];
            local_f0[1] = (*paMVar19)[1];
            local_f0[2] = (*paMVar19)[2];
            dirCount[numDirs] = 1;
            local_f0 = local_f0 + 3;
            numDirs = numDirs + 1;
            local_f8 = local_f8 + 0xc;
            paMVar14 = (MeVector3 *)dirCount;
          }
        }
        else {
          fVar18 = (*paMVar19)[2];
          local_104 = dir;
          do {
                    /* Unresolved local var: MeReal dot@[DW_OP_reg12(ST1)] */
            fVar2 = fVar18 * (*local_104)[2] +
                    (*local_104)[1] * (*paMVar19)[1] + (*local_104)[0] * (*paMVar19)[0];
            if ((ABS(fVar2 - 1.0) < 0.001) || (bVar8 = false, ABS((fVar2 - -1.0) * -1.0) < 0.001)) {
              bVar8 = true;
            }
            if (bVar8) {
              dirCount[iVar15] = dirCount[iVar15] + 1;
              bVar20 = true;
            }
            iVar15 = iVar15 + 1;
            paMVar14 = local_104 + 1;
            local_104 = paMVar14;
            if (numDirs <= iVar15) goto LAB_00011d06;
          } while (!bVar20);
        }
        paMVar19 = paMVar19 + 1;
        j = j + 1;
      } while (j < 3);
      i = i + 1;
      local_f4 = local_f4 + 1;
    } while (i < local_b4);
  }
  iVar15 = 0;
  iVar16 = 0;
  axisIx = -1;
  if (0 < numDirs) {
    uVar9 = numDirs & 3;
    paMVar14 = dir;
    if (numDirs < 2) {
LAB_00011479:
      if (iVar15 < dirCount[iVar16]) {
        axisIx = iVar16;
        cylAxis[0] = (*paMVar14)[0];
        cylAxis[1] = (*paMVar14)[1];
        cylAxis[2] = (*paMVar14)[2];
        iVar15 = dirCount[iVar16];
      }
      iVar16 = iVar16 + 1;
      paMVar14 = paMVar14 + 1;
      if (numDirs <= iVar16) goto LAB_00011592;
    }
    else if (uVar9 != 0) {
      if (1 < uVar9) {
        paMVar14 = dir;
        if (2 < uVar9) {
          if (0 < *dirCount) {
            axisIx = 0;
            cylAxis[0] = (*dir)[0];
            cylAxis[1] = (*dir)[1];
            cylAxis[2] = (*dir)[2];
            iVar15 = *dirCount;
          }
          paMVar14 = dir + 1;
          iVar16 = 1;
        }
        if (iVar15 < dirCount[iVar16]) {
          axisIx = iVar16;
          cylAxis[0] = (*paMVar14)[0];
          cylAxis[1] = (*paMVar14)[1];
          cylAxis[2] = (*paMVar14)[2];
          iVar15 = dirCount[iVar16];
        }
        paMVar14 = paMVar14 + 1;
        iVar16 = iVar16 + 1;
      }
      goto LAB_00011479;
    }
    do {
      if (iVar15 < dirCount[iVar16]) {
        axisIx = iVar16;
        cylAxis[0] = (*paMVar14)[0];
        cylAxis[1] = (*paMVar14)[1];
        cylAxis[2] = (*paMVar14)[2];
        iVar15 = dirCount[iVar16];
      }
      if (iVar15 < dirCount[iVar16 + 1]) {
        axisIx = iVar16 + 1;
        cylAxis[0] = paMVar14[1][0];
        cylAxis[1] = paMVar14[1][1];
        cylAxis[2] = paMVar14[1][2];
        iVar15 = dirCount[iVar16 + 1];
      }
      if (iVar15 < dirCount[iVar16 + 2]) {
        axisIx = iVar16 + 2;
        cylAxis[0] = paMVar14[2][0];
        cylAxis[1] = paMVar14[2][1];
        cylAxis[2] = paMVar14[2][2];
        iVar15 = dirCount[iVar16 + 2];
      }
      local_108 = paMVar14 + 3;
      if (iVar15 < dirCount[iVar16 + 3]) {
        axisIx = iVar16 + 3;
        cylAxis[0] = paMVar14[3][0];
        cylAxis[1] = paMVar14[3][1];
        cylAxis[2] = paMVar14[3][2];
        iVar15 = dirCount[iVar16 + 3];
      }
      iVar16 = iVar16 + 4;
      paMVar14 = paMVar14 + 4;
    } while (iVar16 < numDirs);
  }
LAB_00011592:
  paMVar13 = (MeVector3 *)0x0;
  iVar16 = 0;
  if (numDirs < 1) goto LAB_00011648;
  uVar9 = numDirs & 3;
  if (numDirs < 2) {
LAB_000115b8:
    if (((int)paMVar13 < dirCount[iVar16]) && (iVar16 != axisIx)) {
      paMVar13 = (MeVector3 *)dirCount[iVar16];
    }
    iVar16 = iVar16 + 1;
    if (numDirs <= iVar16) goto LAB_00011648;
  }
  else if (uVar9 != 0) {
    if (1 < uVar9) {
      if (2 < uVar9) {
        if ((0 < *dirCount) && (axisIx != 0)) {
          paMVar13 = (MeVector3 *)*dirCount;
        }
        iVar16 = 1;
      }
      if (((int)paMVar13 < dirCount[iVar16]) && (iVar16 != axisIx)) {
        paMVar13 = (MeVector3 *)dirCount[iVar16];
      }
      iVar16 = iVar16 + 1;
      paMVar14 = (MeVector3 *)dirCount;
    }
    goto LAB_000115b8;
  }
  do {
    if (((int)paMVar13 < dirCount[iVar16]) && (iVar16 != axisIx)) {
      paMVar13 = (MeVector3 *)dirCount[iVar16];
    }
    if (((int)paMVar13 < dirCount[iVar16 + 1]) && (iVar16 + 1 != axisIx)) {
      paMVar13 = (MeVector3 *)dirCount[iVar16 + 1];
    }
    if (((int)paMVar13 < dirCount[iVar16 + 2]) && (iVar16 + 2 != axisIx)) {
      paMVar13 = (MeVector3 *)dirCount[iVar16 + 2];
    }
    paMVar14 = (MeVector3 *)dirCount[iVar16 + 3];
    if (((int)paMVar13 < (int)paMVar14) && (iVar16 + 3 != axisIx)) {
      paMVar13 = paMVar14;
    }
    iVar16 = iVar16 + 4;
  } while (iVar16 < numDirs);
LAB_00011648:
  if ((int)paMVar13 < iVar15) {
                    /* Unresolved local var: int j@[DW_OP_reg1(ECX)] */
    paMVar14 = mesh->verts;
    bbox.min[0] = (*paMVar14)[0];
    bbox.min[1] = (*paMVar14)[1];
    bbox.min[2] = (*paMVar14)[2];
    local_e0 = mesh->verts;
    i_1 = 1;
    local_104 = (MeVector3 *)mesh->numVerts;
    bbox.max[0] = (*local_e0)[0];
    bbox.max[1] = (*local_e0)[1];
    bbox.max[2] = (*local_e0)[2];
    if (1 < (int)local_104) {
      iVar15 = 0xc;
      do {
        pfVar1 = (float *)((int)*local_e0 + iVar15);
        fVar18 = *pfVar1;
        if (fVar18 < bbox.max[0]) {
          fVar18 = bbox.max[0];
        }
        if (*pfVar1 < bbox.min[0]) {
          bbox.min[0] = *pfVar1;
        }
        fVar2 = pfVar1[1];
        if (fVar2 < bbox.max[1]) {
          fVar2 = bbox.max[1];
        }
        if (pfVar1[1] < bbox.min[1]) {
          bbox.min[1] = pfVar1[1];
        }
        fVar3 = pfVar1[2];
        if (fVar3 < bbox.max[2]) {
          fVar3 = bbox.max[2];
        }
        if (pfVar1[2] < bbox.min[2]) {
          bbox.min[2] = pfVar1[2];
        }
        i_1 = i_1 + 1;
        iVar15 = iVar15 + 0xc;
        bbox.max[0] = fVar18;
        bbox.max[1] = fVar2;
        bbox.max[2] = fVar3;
      } while (i_1 < (int)local_104);
    }
    (*relTM)[0] = 1.0;
    (*relTM)[1] = 0.0;
    (*relTM)[2] = 0.0;
    (*relTM)[3] = 0.0;
    relTM[1][0] = 0.0;
    relTM[1][1] = 1.0;
    relTM[1][2] = 0.0;
    relTM[1][3] = 0.0;
    relTM[2][0] = 0.0;
    relTM[2][1] = 0.0;
    relTM[2][2] = 1.0;
    relTM[2][3] = 0.0;
    relTM[3][0] = 0.0;
    relTM[3][1] = 0.0;
    relTM[3][2] = 0.0;
    relTM[3][3] = 1.0;
    relTM[3][0] = bbox.min[0] + (bbox.max[0] - bbox.min[0]) * 0.5;
    relTM[3][1] = bbox.min[1] + (bbox.max[1] - bbox.min[1]) * 0.5;
    relTM[3][2] = bbox.min[2] + (bbox.max[2] - bbox.min[2]) * 0.5;
    relTM[2][0] = cylAxis[0];
    relTM[2][1] = cylAxis[1];
    relTM[2][2] = cylAxis[2];
    if (ABS(cylAxis[0]) <= ABS(cylAxis[1])) {
                    /* Unresolved local var: MeReal scale@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[???] */
      fVar18 = relTM[2][2];
      fVar2 = relTM[2][1];
      fVar3 = 1.0 / SQRT(fVar2 * fVar2 + fVar18 * fVar18);
      (*relTM)[0] = 0.0;
      fVar7 = fVar2 * fVar3;
      fVar3 = -fVar18 * fVar3;
      (*relTM)[2] = fVar7;
      (*relTM)[1] = fVar3;
      relTM[1][0] = fVar2 * fVar7 - fVar18 * fVar3;
      fVar18 = relTM[2][0];
      relTM[1][2] = fVar18 * fVar3;
      relTM[1][1] = -fVar18 * fVar7;
    }
    else {
                    /* Unresolved local var: MeReal scale@[DW_OP_reg11(ST0)]
                       Unresolved local var: float __result@[???] */
      fVar18 = 1.0 / SQRT(cylAxis[0] * cylAxis[0] + cylAxis[2] * cylAxis[2]);
      (*relTM)[1] = 0.0;
      fVar2 = cylAxis[2] * fVar18;
      fVar18 = -cylAxis[0] * fVar18;
      (*relTM)[0] = fVar2;
      (*relTM)[2] = fVar18;
      relTM[1][0] = cylAxis[1] * fVar18;
      relTM[1][1] = cylAxis[2] * fVar2 - cylAxis[0] * fVar18;
      relTM[1][2] = -cylAxis[1] * fVar2;
    }
    fVar18 = 0.0;
    maxlen = 0.0;
    minr = 3.4028235e+38;
    minlen = 0.0;
    iVar15 = mesh->numVerts;
    if (0 < iVar15) {
      local_e8 = relTM[3][0];
      paMVar14 = mesh->verts;
      local_100 = fVar18;
      i = iVar15;
      do {
                    /* Unresolved local var: MeReal rsqr@[???]
                       Unresolved local var: MeReal alongAxis@[DW_OP_reg13(ST2)] */
        fVar2 = (*paMVar14)[0] - relTM[3][0];
        fVar3 = (*paMVar14)[1] - relTM[3][1];
        fVar7 = (*paMVar14)[2] - relTM[3][2];
        fVar18 = fVar7 * cylAxis[2] + fVar2 * cylAxis[0] + fVar3 * cylAxis[1];
        uVar12 = (undefined2)((uint)iVar15 >> 0x10);
        if (fVar18 < minlen) {
          minlen = fVar18;
        }
        fVar17 = fVar18;
        if (fVar18 < maxlen) {
          fVar17 = maxlen;
        }
        maxlen = fVar17;
        toClosest[0] = fVar2 - cylAxis[0] * fVar18;
        toClosest[1] = fVar3 - cylAxis[1] * fVar18;
        toClosest[2] = fVar7 - fVar18 * cylAxis[2];
                    /* Unresolved local var: MeReal m@[DW_OP_reg11(ST0)] */
        fVar2 = toClosest[0] * toClosest[0] + toClosest[1] * toClosest[1] +
                toClosest[2] * toClosest[2];
        fVar18 = fVar2;
        if (fVar2 < local_100) {
          fVar18 = local_100;
        }
        uVar21 = (ushort)(fVar2 < 0.001) << 8 | (ushort)NAN(fVar2) << 10 |
                 (ushort)(fVar2 == 0.001) << 0xe;
        iVar15 = CONCAT22(uVar12,uVar21);
        local_100 = fVar18;
        if (((char)(uVar21 >> 8) == '\0') &&
           (uVar21 = (ushort)(minr < fVar2) << 8 | (ushort)(NAN(minr) || NAN(fVar2)) << 10 |
                     (ushort)(minr == fVar2) << 0xe, iVar15 = CONCAT22(uVar12,uVar21),
           (char)(uVar21 >> 8) == '\0')) {
          minr = fVar2;
        }
        paMVar14 = paMVar14 + 1;
        i = i + -1;
      } while (i != 0);
    }
    __result = SQRT(fVar18);
                    /* Unresolved local var: float __result@[DW_OP_reg13(ST2)] */
    uVar12 = (undefined2)((uint)iVar15 >> 0x10);
    if (0.001 <= ABS(minlen)) {
      fVar18 = ABS((1.0 / -minlen) * (maxlen + minlen));
      uVar21 = (ushort)(fVar18 < 0.001) << 8 | (ushort)NAN(fVar18) << 10 |
               (ushort)(fVar18 == 0.001) << 0xe;
      bVar20 = fVar18 >= 0.001;
    }
    else {
      fVar18 = ABS(maxlen);
      uVar4 = (ushort)(fVar18 < 0.001) << 8 | (ushort)NAN(fVar18) << 10;
      uVar21 = uVar4 | (ushort)(fVar18 == 0.001) << 0xe;
      bVar20 = uVar4 == 0;
    }
    uVar10 = CONCAT22(uVar12,uVar21);
    if (bVar20) {
      *(undefined4 *)((int)auStack_114 + iVar6 + iVar5 + 4) = uVar10;
      *(undefined4 *)((int)auStack_114 + iVar6 + iVar5) = uVar10;
      *(char **)((int)&fStack_118 + iVar6 + iVar5) = "ERROR: Cylinder ends are not symmetric.";
    }
    else {
      fVar18 = (1.0 / __result) * (__result - SQRT(minr));
      uVar21 = (ushort)(fVar18 < 0.05) << 8 | (ushort)NAN(fVar18) << 10 |
               (ushort)(fVar18 == 0.05) << 0xe;
      uVar10 = CONCAT22(uVar12,uVar21);
      if ((char)(uVar21 >> 8) != '\0') {
        *(undefined4 *)((int)auStack_114 + iVar6 + iVar5 + 4) = uVar10;
        *(undefined4 *)((int)auStack_114 + iVar6 + iVar5) = uVar10;
        *(undefined4 *)((int)&fStack_118 + iVar6 + iVar5) = 3;
        *(char **)((int)aiStack_120 + iVar6 + iVar5 + 4) = name;
        *(undefined4 *)((int)aiStack_120 + iVar6 + iVar5) = 0x11b15;
        pMVar11 = MeFPrimitiveCreate(*(void **)((int)aiStack_120 + iVar6 + iVar5 + 4),
                                     *(int *)((int)&fStack_118 + iVar6 + iVar5));
        cylgeom = pMVar11;
        if (pMVar11 == (MeFPrimitive *)0x0) {
          return (MeFPrimitive *)0x0;
        }
        fVar18 = maxlen - minlen;
        *(float *)((int)&fStack_118 + iVar6 + iVar5) = fVar18;
        *(MeFPrimitive **)((int)aiStack_120 + iVar6 + iVar5 + 4) = pMVar11;
        maxlen = fVar18;
        *(undefined4 *)((int)aiStack_120 + iVar6 + iVar5) = 0x11b40;
        MeFPrimitiveSetHeight
                  (*(void **)((int)aiStack_120 + iVar6 + iVar5 + 4),
                   *(float *)((int)&fStack_118 + iVar6 + iVar5));
        pMVar11 = cylgeom;
        *(float *)((int)&fStack_118 + iVar6 + iVar5) = __result;
        *(MeFPrimitive **)((int)aiStack_120 + iVar6 + iVar5 + 4) = pMVar11;
        *(undefined4 *)((int)aiStack_120 + iVar6 + iVar5) = 0x11b55;
        MeFPrimitiveSetRadius
                  (*(void **)((int)aiStack_120 + iVar6 + iVar5 + 4),
                   *(float *)((int)&fStack_118 + iVar6 + iVar5));
        return cylgeom;
      }
      *(undefined4 *)((int)auStack_114 + iVar6 + iVar5 + 4) = uVar10;
      *(undefined4 *)((int)auStack_114 + iVar6 + iVar5) = uVar10;
      *(char **)((int)&fStack_118 + iVar6 + iVar5) = "ERROR: Vertices not at constant radius.";
    }
  }
  else {
    *(MeVector3 **)((int)auStack_114 + iVar6 + iVar5 + 4) = paMVar14;
    *(MeVector3 **)((int)auStack_114 + iVar6 + iVar5) = paMVar14;
    *(char **)((int)&fStack_118 + iVar6 + iVar5) = "ERROR: Can\'t find definitive cylinder axis.";
  }
  *(undefined4 *)((int)aiStack_120 + iVar6 + iVar5 + 4) = 0;
  *(undefined4 *)((int)aiStack_120 + iVar6 + iVar5) = 0x11acf;
  MeWarning(*(int *)((int)aiStack_120 + iVar6 + iVar5 + 4),
            *(void **)((int)&fStack_118 + iVar6 + iVar5));
  return cylgeom;
}


/* ==== MeFConvexCreateFromMesh ==== */

MeFPrimitive * MeFConvexCreateFromMesh(char *name,MyMesh *mesh,MeMatrix4Ptr relTM)

{
  int iVar1;
  int iVar2;
  MeFPrimitive *pMVar3;
  MeFPrimitive *extraout_EAX;
  MeFPrimitive *pMVar4;
  int iVar5;
  int iVar6;
  MeReal MStackY_60;
  float afStack_4c [3];
  MeFPrimitive *convexGeom;
  MeVector3 newOrigin;
  MyMesh localMesh;
  
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)]
                       Unresolved local var: int isok@[DW_OP_reg0(EAX)] */
  convexGeom = (MeFPrimitive *)0x0;
  MStackY_60 = 1.02597e-40;
  iVar2 = PointInsideConvex(mesh,newOrigin);
  pMVar4 = (MeFPrimitive *)0x0;
  if (iVar2 != 0) {
    iVar2 = mesh->numVerts;
    iVar5 = 0;
    iVar1 = -(iVar2 * 0xc + 0xfU & 0xfffffff0);
    if ((int)convexGeom < iVar2) {
      iVar6 = 0;
      do {
        iVar5 = iVar5 + 1;
        *(float *)((int)afStack_4c + iVar6 + iVar1) =
             *(float *)((int)*mesh->verts + iVar6) - newOrigin[0];
        *(float *)((int)afStack_4c + iVar6 + iVar1 + 4) =
             *(float *)((int)*mesh->verts + iVar6 + 4) - newOrigin[1];
        *(float *)((int)afStack_4c + iVar6 + iVar1 + 8) =
             *(float *)((int)*mesh->verts + iVar6 + 8) - newOrigin[2];
        iVar6 = iVar6 + 0xc;
      } while (iVar5 < iVar2);
    }
    relTM[3][0] = 0.0;
    relTM[3][1] = 0.0;
    relTM[3][2] = 0.0;
    (*relTM)[0] = 1.0;
    (*relTM)[1] = 0.0;
    (*relTM)[2] = 0.0;
    (*relTM)[3] = 0.0;
    relTM[1][0] = 0.0;
    relTM[1][1] = 1.0;
    relTM[1][2] = 0.0;
    relTM[1][3] = 0.0;
    relTM[2][0] = 0.0;
    relTM[2][1] = 0.0;
    relTM[2][2] = 1.0;
    relTM[2][3] = 0.0;
    relTM[3][3] = 1.0;
    relTM[3][0] = newOrigin[0];
    relTM[3][1] = newOrigin[1];
    relTM[3][2] = newOrigin[2];
    *(MeReal *)(&stack0xffffffb0 + iVar1) = newOrigin[0];
    *(MeReal *)(&stack0xffffffac + iVar1) = newOrigin[0];
    *(undefined4 *)((int)newOrigin + iVar1 + -0x1c) = 6;
    *(char **)((int)newOrigin + iVar1 + -0x20) = name;
    *(undefined4 *)((int)&MStackY_60 + iVar1) = 0x11f10;
    pMVar3 = MeFPrimitiveCreate(*(void **)((int)newOrigin + iVar1 + -0x20),
                                *(int *)((int)newOrigin + iVar1 + -0x1c));
    convexGeom = pMVar3;
    pMVar4 = convexGeom;
    if ((pMVar3 != (MeFPrimitive *)0x0) && (iVar5 = 0, 0 < iVar2)) {
      iVar6 = 0;
      do {
        *(MeFPrimitive **)(&stack0xffffffb0 + iVar1) = pMVar3;
        *(MeFPrimitive **)(&stack0xffffffac + iVar1) = pMVar3;
        pMVar4 = convexGeom;
        *(int *)((int)newOrigin + iVar1 + -0x1c) = (int)afStack_4c + iVar6 + iVar1;
        iVar5 = iVar5 + 1;
        *(MeFPrimitive **)((int)newOrigin + iVar1 + -0x20) = pMVar4;
        iVar6 = iVar6 + 0xc;
        *(undefined4 *)((int)&MStackY_60 + iVar1) = 0x11f38;
        MeFPrimitiveAddVertex
                  (*(void **)((int)newOrigin + iVar1 + -0x20),
                   *(void **)((int)newOrigin + iVar1 + -0x1c));
        pMVar3 = extraout_EAX;
        pMVar4 = convexGeom;
      } while (iVar5 < iVar2);
    }
  }
  return pMVar4;
}


/* ==== MeMesh2AABB ==== */

void MeMesh2AABB(MyMesh *mesh,MyAABB *bbox)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  MeVector3 *paMVar5;
  float fVar6;
  int iVar7;
  int local_1c;
  int i;
  
                    /* Unresolved local var: int j@[DW_OP_reg1(ECX)] */
  paMVar5 = mesh->verts;
  bbox->min[0] = (*paMVar5)[0];
  bbox->min[1] = (*paMVar5)[1];
  bbox->min[2] = (*paMVar5)[2];
  paMVar5 = mesh->verts;
  bbox->max[0] = (*paMVar5)[0];
  bbox->max[1] = (*paMVar5)[1];
  bbox->max[2] = (*paMVar5)[2];
  i = 1;
  if (1 < mesh->numVerts) {
    local_1c = 3;
    iVar7 = 3;
    do {
      fVar1 = (*mesh->verts)[iVar7];
      if (fVar1 < bbox->max[0]) {
        fVar1 = bbox->max[0];
      }
      bbox->max[0] = fVar1;
      fVar1 = bbox->min[0];
      fVar2 = (*mesh->verts)[local_1c];
      if (fVar2 < fVar1) {
        fVar1 = fVar2;
      }
      bbox->min[0] = fVar1;
      fVar1 = (*mesh->verts)[iVar7 + 1];
      if (fVar1 < bbox->max[1]) {
        fVar1 = bbox->max[1];
      }
      bbox->max[1] = fVar1;
      fVar1 = bbox->min[1];
      fVar2 = (*mesh->verts)[local_1c + 1];
      if (fVar2 < fVar1) {
        fVar1 = fVar2;
      }
      bbox->min[1] = fVar1;
      fVar1 = (*mesh->verts)[iVar7 + 2];
      if (fVar1 < bbox->max[2]) {
        fVar1 = bbox->max[2];
      }
      bbox->max[2] = fVar1;
      fVar1 = bbox->min[2];
      fVar2 = (*mesh->verts)[local_1c + 2];
      if (fVar2 < fVar1) {
        fVar1 = fVar2;
      }
      bbox->min[2] = fVar1;
      i = i + 1;
      local_1c = local_1c + 3;
      iVar7 = iVar7 + 3;
    } while (i < mesh->numVerts);
  }
  fVar1 = bbox->min[0];
  fVar2 = bbox->min[1];
  fVar6 = bbox->max[0] - fVar1;
  bbox->size[0] = fVar6;
  bbox->size[1] = bbox->max[1] - fVar2;
  fVar3 = bbox->min[2];
  bbox->size[2] = bbox->max[2] - fVar3;
  fVar4 = bbox->size[1];
  bbox->centre[0] = fVar1 + fVar6 * 0.5;
  fVar1 = bbox->size[2];
  bbox->centre[1] = fVar2 + fVar4 * 0.5;
  bbox->centre[2] = fVar3 + fVar1 * 0.5;
  return;
}


/* ==== name2Type ==== */

/* WARNING: Unknown calling convention */

Mesh2GeometryType name2Type(char *name)

{
  int iVar1;
  byte *pbVar2;
  byte *pbVar3;
  undefined1 uVar4;
  undefined1 uVar5;
  bool bVar6;
  Mesh2GeometryType type;
  char tmpName [256];
  
  uVar4 = &stack0xfffffff0 < (undefined1 *)0x120;
  uVar5 = &stack0x00000000 == (undefined1 *)0x130;
  type = kMesh2GeometryUnknown;
  strncpy(tmpName,name,0x100);
  strupr(tmpName);
  iVar1 = 5;
  pbVar2 = (byte *)tmpName;
  pbVar3 = &DAT_00012466;
  do {
    if (iVar1 == 0) break;
    iVar1 = iVar1 + -1;
    uVar4 = *pbVar2 < *pbVar3;
    uVar5 = *pbVar2 == *pbVar3;
    pbVar2 = pbVar2 + 1;
    pbVar3 = pbVar3 + 1;
  } while ((bool)uVar5);
  bVar6 = (!(bool)uVar4 && !(bool)uVar5) == (bool)uVar4;
  if (bVar6) {
    type = kMesh2GeometrySphere;
  }
  else {
    iVar1 = 5;
    pbVar2 = (byte *)tmpName;
    pbVar3 = &DAT_0001246c;
    do {
      if (iVar1 == 0) break;
      iVar1 = iVar1 + -1;
      bVar6 = *pbVar2 == *pbVar3;
      pbVar2 = pbVar2 + 1;
      pbVar3 = pbVar3 + 1;
    } while (bVar6);
    if (bVar6) {
      type = kMesh2GeometryBox;
    }
    else {
      iVar1 = 5;
      pbVar2 = (byte *)tmpName;
      pbVar3 = &DAT_00012472;
      do {
        if (iVar1 == 0) break;
        iVar1 = iVar1 + -1;
        bVar6 = *pbVar2 == *pbVar3;
        pbVar2 = pbVar2 + 1;
        pbVar3 = pbVar3 + 1;
      } while (bVar6);
      if (bVar6) {
        type = kMesh2GeometryCylinder;
      }
      else {
        iVar1 = 5;
        pbVar2 = (byte *)tmpName;
        pbVar3 = &DAT_00012478;
        do {
          if (iVar1 == 0) break;
          iVar1 = iVar1 + -1;
          bVar6 = *pbVar2 == *pbVar3;
          pbVar2 = pbVar2 + 1;
          pbVar3 = pbVar3 + 1;
        } while (bVar6);
        if (bVar6) {
          type = kMesh2GeometryConvex;
        }
      }
    }
  }
  return type;
}


/* ==== PointInsideConvex ==== */

int PointInsideConvex(MyMesh *mesh,MeReal *inside)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  MeVector3 *paMVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  bool bVar9;
  int iVar10;
  MeVector3 *local_70;
  MeBool haveTetra;
  MeVector3 vertRel;
  MeVector3 faceNormal;
  MeVector3 e2;
  MeVector3 e1;
  
                    /* Unresolved local var: int tetra4@[DW_OP_reg3(EBX)] */
  bVar9 = false;
  paMVar5 = mesh->verts;
  fVar1 = paMVar5[1][0] - (*paMVar5)[0];
  fVar7 = paMVar5[1][1] - (*paMVar5)[1];
  fVar4 = paMVar5[1][2] - (*paMVar5)[2];
  paMVar5 = mesh->verts;
  fVar6 = paMVar5[2][0] - (*paMVar5)[0];
  fVar2 = paMVar5[2][1] - (*paMVar5)[1];
  fVar8 = paMVar5[2][2] - (*paMVar5)[2];
  fVar3 = fVar7 * fVar8 - fVar4 * fVar2;
  fVar4 = fVar4 * fVar6 - fVar8 * fVar1;
                    /* Unresolved local var: int j@[???]
                       Unresolved local var: MeReal mag@[DW_OP_reg11(ST0)]
                       Unresolved local var: MeReal k@[DW_OP_reg12(ST1)] */
  fVar1 = fVar2 * fVar1 - fVar7 * fVar6;
  fVar2 = fVar3 * fVar3 + fVar4 * fVar4 + fVar1 * fVar1;
  if (fVar2 <= 0.0) {
    faceNormal[0] = 1.0;
    faceNormal[1] = 0.0;
    faceNormal[2] = 0.0;
  }
  else {
    fVar2 = 1.0 / SQRT(fVar2);
    faceNormal[0] = fVar3 * fVar2;
    faceNormal[1] = fVar4 * fVar2;
    faceNormal[2] = fVar2 * fVar1;
  }
  iVar10 = 3;
  if (mesh->numVerts < 4) {
LAB_000123f5:
    if (!bVar9) {
      MeWarning(0,"PointInsideConvex: Mesh seem to be flat.");
      return 0;
    }
  }
  else {
    local_70 = paMVar5 + 3;
    do {
                    /* Unresolved local var: MeReal fromFace@[???] */
      if (0.001 < ABS(((*local_70)[2] - (*paMVar5)[2]) * faceNormal[2] +
                      ((*local_70)[0] - (*paMVar5)[0]) * faceNormal[0] +
                      ((*local_70)[1] - (*paMVar5)[1]) * faceNormal[1])) {
        bVar9 = true;
      }
      iVar10 = iVar10 + 1;
      local_70 = local_70 + 1;
      if (mesh->numVerts <= iVar10) goto LAB_000123f5;
    } while (!bVar9);
  }
                    /* Unresolved local var: int i@[???] */
  *inside = 0.0;
  inside[1] = 0.0;
  inside[2] = 0.0;
  paMVar5 = mesh->verts;
  fVar1 = (*paMVar5)[0];
  *inside = fVar1;
  fVar2 = (*paMVar5)[1];
  inside[1] = fVar2;
  fVar3 = (*paMVar5)[2];
  inside[2] = fVar3;
  paMVar5 = mesh->verts;
  fVar1 = fVar1 + paMVar5[1][0];
  *inside = fVar1;
  fVar2 = fVar2 + paMVar5[1][1];
  inside[1] = fVar2;
  fVar3 = fVar3 + paMVar5[1][2];
  inside[2] = fVar3;
  paMVar5 = mesh->verts;
  fVar1 = fVar1 + paMVar5[2][0];
  *inside = fVar1;
  fVar2 = fVar2 + paMVar5[2][1];
  inside[1] = fVar2;
  fVar3 = fVar3 + paMVar5[2][2];
  inside[2] = fVar3;
  paMVar5 = mesh->verts + iVar10;
  fVar1 = fVar1 + (*paMVar5)[0];
  *inside = fVar1;
  fVar2 = fVar2 + (*paMVar5)[1];
  inside[1] = fVar2;
  fVar4 = (*paMVar5)[2];
  *inside = fVar1 * 0.25;
  inside[1] = fVar2 * 0.25;
  inside[2] = (fVar3 + fVar4) * 0.25;
  return 1;
}


