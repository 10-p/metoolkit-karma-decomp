/* ==== McdSphereBoxPlaneRegisterInteractions ==== */

void McdSphereBoxPlaneRegisterInteractions(McdFramework *frame)

{
  McdSphereSphereRegisterInteraction(frame);
  McdBoxBoxRegisterInteraction(frame);
  McdBoxPlaneRegisterInteraction(frame);
  McdBoxSphereRegisterInteraction(frame);
  McdSpherePlaneRegisterInteraction();
  return;
}


