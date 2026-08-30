/* ==== McdPrimitivesRegisterTypes ==== */

void McdPrimitivesRegisterTypes(McdFramework *frame)

{
  McdSphereBoxPlaneRegisterTypes(frame);
  McdCylinderRegisterType(frame);
  McdTriangleListRegisterType(frame);
  McdSphylRegisterType(frame);
  return;
}


