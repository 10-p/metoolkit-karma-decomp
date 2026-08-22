/*  Hand-written prelude for IxBoxBox.
 *
 *  This is the per-object manual step: declare everything the original object
 *  imported, plus its file-scope statics. Everything else is generated.
 *
 *  Contract taken straight from `nm IxBoxBox.o`:
 *    exports  McdBoxBoxIntersect, McdBoxBoxSafeTime, McdBoxBoxRegisterInteraction
 *    imports  the 9 Mcd/Me symbols below (+ __gxx_personality_v0, not needed in C)
 *    statics  Vec4NegOne / Vec4One / Vec4Zero, McdBoxBoxSafeTime::maxAngle
 */
#include <McdCTypes.h>
#include <McdModel.h>
#include <McdBox.h>
#include <McdFrame.h>
#include <McdContact.h>
#include <MeMath.h>

/* --- file-scope statics ---------------------------------------------------
   In the original these were C++ globals filled by a static constructor
   (__static_initialization_and_destruction_0, which we drop). Constant data,
   so a plain C initializer is equivalent and avoids the ctor entirely. */
static const lsVec4 Vec4NegOne = {{-1.0f, -1.0f, -1.0f, -1.0f}};
static const lsVec4 Vec4One    = {{ 1.0f,  1.0f,  1.0f,  1.0f}};
static const lsVec4 Vec4Zero   = {{ 0.0f,  0.0f,  0.0f,  0.0f}};

/* Function-local static in McdBoxBoxSafeTime(); Ghidra renders it
   `McdBoxBoxSafeTime::maxAngle`, which ghidra_clean.py flattens to `__`.
   Original symbol _ZZ17McdBoxBoxSafeTimeE8maxAngle; value read out of the
   object's .data (cd cc 4c 3e = 0.2f). NEVER guess these — read them. */
static const float McdBoxBoxSafeTime__maxAngle = 0.2f;

/* --- imports --------------------------------------------------------------
   These resolve against the still-original metoolkit archives during the
   incremental substitution, and against other recovered objects later. */
extern MeMatrix4Ptr McdModelGetTransformPtr(McdModelID m);
extern MeReal       McdModelGetContactTolerance(McdModelID m);
extern McdGeometryID McdModelGetGeometry(McdModelID m);
extern MeReal      *McdModelGetLinearVelocityPtr(McdModelID m);
extern MeReal      *McdModelGetAngularVelocityPtr(McdModelID m);
extern MeReal      *McdBoxGetRadii(McdBoxID b);
extern void         McdFrameworkSetInteractions(McdFramework *f, int t1, int t2,
                                                McdInteractions *i);
extern void         MeQuaternionFromTM(MeVector4 q, const MeMatrix4 tm);

/* C++-mangled import: MovingBoxBoxIntersect(float const*, lsTransform const*,
   lsVec3 const&, float const*, lsTransform const*, lsVec3 const&, float,
   float&, lsVec3&). Declared with an asm label so C can call it directly. */
extern int MovingBoxBoxIntersect(const float *rA, const lsTransform *tA,
                                 const lsVec3 *vA,
                                 const float *rB, const lsTransform *tB,
                                 const lsVec3 *vB,
                                 float maxTime, float *outT, lsVec3 *outN)
    KD_MANGLED("_Z21MovingBoxBoxIntersectPKfPK11lsTransformRK6lsVec3S0_S3_S6_fRfRS4_");
