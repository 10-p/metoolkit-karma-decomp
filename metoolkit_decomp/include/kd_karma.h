/* kd_karma.h — umbrella over metoolkit's public headers.
 *
 * Generated sources reference Karma types (McdBoxID, McdUserTriangle, ...)
 * far beyond the handful of headers a per-object prelude happens to pull in.
 * Including everything once is simpler and cheaper than tracking which
 * header declares which type. Headers that do not compile standalone
 * (the MeViewer/D3D/GL ones, which we never build) are excluded.
 */
#ifndef KD_KARMA_H
#define KD_KARMA_H

#include <Mcd.h>
#include <McdAggregate.h>
#include <McdBatch.h>
#include <McdBox.h>
#include <McdCTypes.h>
#include <McdCheck.h>
#include <McdContact.h>
#include <McdConvexMesh.h>
#include <McdCullingTable.h>
#include <McdCylinder.h>
#include <McdFrame.h>
#include <McdGeometry.h>
#include <McdGeometryInstance.h>
#include <McdGeometryTypes.h>
#include <McdGjk.h>
#include <McdGjkMaximumPoint.h>
#include <McdInteractionTable.h>
#include <McdInteractions.h>
#include <McdModel.h>
#include <McdModelPair.h>
#include <McdModelPairContainer.h>
#include <McdModelPairManager.h>
#include <McdNull.h>
#include <McdPlane.h>
#include <McdPlaneIntersect.h>
#include <McdPrimitives.h>
#include <McdProfile.h>
#include <McdQHullTypes.h>
#include <McdSpace.h>
#include <McdSphere.h>
#include <McdSphyl.h>
#include <McdTriangleList.h>
#include <McduDebugDraw.h>
#include <McduRequestTable.h>
#include <Mdt.h>
#include <MdtAlignment.h>
#include <MdtAngular3.h>
#include <MdtBSJoint.h>
#include <MdtBcl.h>
#include <MdtBody.h>
#include <MdtCarWheel.h>
#include <MdtCheckMacros.h>
#include <MdtConeLimit.h>
#include <MdtConstraint.h>
#include <MdtContact.h>
#include <MdtContactGroup.h>
#include <MdtContactParams.h>
#include <MdtDefaults.h>
#include <MdtFixedPath.h>
#include <MdtHinge.h>
#include <MdtKea.h>
#include <MdtLimit.h>
#include <MdtLinear1.h>
#include <MdtLinear2.h>
#include <MdtMainLoop.h>
#include <MdtPartition.h>
#include <MdtPrismatic.h>
#include <MdtRPROJoint.h>
#include <MdtSkeletal.h>
#include <MdtSpring.h>
#include <MdtSpring6.h>
#include <MdtTypes.h>
#include <MdtUniversal.h>
#include <MdtUserConstraint.h>
#include <MdtUtilities.h>
#include <MdtWorld.h>
#include <MeApp.h>
#include <MeAppTypes.h>
#include <MeAssert.h>
#include <MeAssetDB.h>
#include <MeAssetDBTypes.h>
#include <MeAssetDBXMLIO.h>
#include <MeAssetFactory.h>
#include <MeAssetFactoryTypes.h>
#include <MeBounding.h>
#include <MeCall.h>
#include <MeChunk.h>
#include <MeCommandLine.h>
#include <MeDebugDraw.h>
#include <MeDict.h>
#include <MeHash.h>
#include <MeHeap.h>
#include <MeIDPool.h>
#include <MeInline.h>
#include <MeMath.h>
#include <MeMemory.h>
#include <MeMessage.h>
#include <MePool.h>
#include <MePoolx.h>
#include <MePrecision.h>
#include <MeProfile.h>
#include <MeSet.h>
#include <MeSimpleFile.h>
#include <MeStream.h>
#include <MeString.h>
#include <MeVersion.h>
#include <MeXMLOutput.h>
#include <MeXMLParser.h>
#include <MeXMLTree.h>
#include <Mst.h>
#include <MstBridge.h>
#include <MstTypes.h>
#include <MstUniverse.h>
#include <MstUtils.h>

/* Do not compile standalone (they need a sibling first) but are fine here. */
#include <McdPolygonIntersection.h>
#include <MeAssetDBXMLIOTypes.h>
#include <MeMisc.h>
#include <MstModelDynamics.h>

/* McdMessage.h was in the excluded list below and does not belong there: it
 * compiles cleanly. Without it `McdErrorDescription` is only the forward
 * typedef kd_compat.h supplies for the bare-tag problem, so it stays
 * INCOMPLETE and McdMessage.o fails seven times on `invalid use of incomplete
 * typedef`. The header carries the layout — three MeI16 and a const char* —
 * and completing the tag is the whole fix. It includes McdCoreErrorList.h,
 * but as an X-macro list inside its own `#define ERR(...)`, which is why that
 * file is unusable standalone and fine here. */
#include <McdMessage.h>

/* Excluded: these do not compile at all.
 *   McdCone.h           references an McdConeID the SDK never defines — cone
 *                       geometry appears to have been dropped before release.
 *   McdCoreErrorList.h  an X-macro list of bare ERR(...) entries, meant to be
 *                       included inside a definition of ERR. Not a header.
 * Karma does not use them, and neither do we.
 */

#endif /* KD_KARMA_H */
