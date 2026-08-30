/* ==== McdPrimitivesRegisterInteractions ==== */

MeBool McdPrimitivesRegisterInteractions(McdFramework *frame)

{
  McdSphereBoxPlaneRegisterInteractions(frame);
  McdSphereLineSegmentRegisterInteraction(frame);
  McdBoxLineSegmentRegisterInteraction(frame);
  McdPlaneLineSegmentRegisterInteraction(frame);
  McdBoxCylinderRegisterInteraction(frame);
  McdCylinderPlaneRegisterInteraction(frame);
  McdCylinderSphereRegisterInteraction(frame);
  McdCylinderCylinderRegisterInteraction(frame);
  McdCylinderLineSegmentRegisterInteraction(frame);
  McdSphereTriangleListRegisterInteraction(frame);
  McdBoxTriangleListRegisterInteraction(frame);
  McdCylinderTriangleListRegisterInteraction(frame);
  McdSphylPlaneRegisterInteraction(frame);
  McdSphylSphereRegisterInteraction(frame);
  McdSphylSphylRegisterInteraction(frame);
  McdSphylBoxRegisterInteraction(frame);
  McdSphylCylinderRegisterInteraction(frame);
  McdSphylTriangleListRegisterInteraction(frame);
  McdSphylLineSegmentRegisterInteraction(frame);
  return 1;
}


