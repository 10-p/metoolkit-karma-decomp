/* lineseg_lp64.c — every line-segment intersection Karma registers, over one seeded case set.
 *
 * Built three times by lineseg_lp64.sh and compared there:
 *
 *   ref    MathEngine's SHIPPED i386 archives       — what the game behaved like in 2004
 *   c64    the recovered product at LP64             — what native x86-64, Win64 and arm64 ship
 *   c32    the recovered product at i386, SSE math   — the pointer-width control for c64
 *
 * WHY THIS EXISTS. `USkeletalMesh::LineCheck` answers every zero-extent trace against a ragdoll by
 * asking each bone's geometry for its line-segment function and calling it (engine-ut2004
 * UnSkeletalMeshCollision.cpp). Nothing in this repository called those functions at LP64: the
 * scenes collide bodies, `difftest_pair.sh` drives pair interactions at i386, and `ktrace` follows
 * bodies that are never traced. `IxSphylLineSegment` read the capsule's radius and half-height at the
 * i386 offsets 0x10/0x14 — inside `_McdGeometry`'s pointers at LP64 — and every 64-bit build shipped
 * with capsule traces that missed or hit at garbage. This harness calls them the way the engine does.
 *
 * One line per case, floats as 8-hex-digit bit patterns:
 *
 *   <type> <category> <index> <input digest> <segment length> <true depth> <ret> <defined mask>
 *          <dist> <px> <py> <pz> <nx> <ny> <nz>
 *
 * `true depth` is how far the segment goes into the shape, computed here in double precision from the
 * geometry this file created — not from Karma (see depth()).
 *
 * ⚠ A FIELD THE FUNCTION DID NOT WRITE IS NOT A RESULT. Shipped Karma's sphyl start-inside path copies
 * an uninitialised stack local into `distance` and `normal`; the box's leaves `normal` alone. Each case
 * therefore runs twice, with the result struct AND the stack beneath the call filled with two different
 * patterns, and a field that differs between the two runs is marked undefined in the mask (bit 0 ret,
 * bit 1 distance, bits 2-4 position, bits 5-7 normal). The comparator compares defined fields only and
 * fails a candidate that leaves undefined what the reference defines.
 *
 * ⚠ THE INPUTS MUST BE BIT-IDENTICAL IN EVERY BUILD OR NOTHING BELOW MEANS ANYTHING. This file's own
 * arithmetic is compiled with SSE scalar math at both widths (`-msse2 -mfpmath=sse` on i386,
 * `-ffp-contract=off` everywhere), uses no libm but sqrtf (correctly rounded), draws every random
 * number as a 24-bit integer times 2^-24, and prints a digest of every input. The comparator refuses
 * a line whose digest differs.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MeMath.h"
#include "McdFrame.h"
#include "McdModel.h"
#include "McdGeometry.h"
#include "McdPrimitives.h"
#include "McdSphere.h"
#include "McdBox.h"
#include "McdPlane.h"
#include "McdCylinder.h"
#include "McdSphyl.h"
#include "McdConvexMesh.h"
#include "McdAggregate.h"
#include "McdNull.h"
#include "McdInteractions.h"

float sqrtf(float);

/* ---- deterministic numbers ------------------------------------------------------------------ */

static unsigned int rs = 2463534242u;
static unsigned int rnext(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static float u01(void) { return (float)(rnext() >> 8) * (1.0f / 16777216.0f); }
static float uab(float a, float b) { return a + (b - a) * u01(); }

typedef float V3[3];

static void vset(float *v, float x, float y, float z) { v[0] = x; v[1] = y; v[2] = z; }

/* ⚠ A DRAW NEVER APPEARS INSIDE AN ARGUMENT LIST. C leaves the order in which a call's arguments are
   evaluated unspecified, and the compilers really differ: x86 GCC evaluates `vset(v, uab(), uab(),
   uab())` right to left at both widths and aarch64 GCC left to right, so the same seed put the same
   numbers in different components and the comparator refused every case (its input digests
   differed). Each draw is a statement of its own, in order. */
static void rvec(float *v, float a0, float b0, float a1, float b1, float a2, float b2)
{
    float x = uab(a0, b0);
    float y = uab(a1, b1);
    float z = uab(a2, b2);
    vset(v, x, y, z);
}
static void rvec2(float *v, float a, float b)              /* x and y drawn, z = 0 */
{
    float x = uab(a, b);
    float y = uab(a, b);
    vset(v, x, y, 0);
}
static void vscale(float *v, float x, float y, float z) { v[0] *= x; v[1] *= y; v[2] *= z; }
static float vdot(const float *a, const float *b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }
static void vmad(float *o, const float *a, float s, const float *b)   /* o = a + s*b */
{
    o[0] = a[0] + s * b[0]; o[1] = a[1] + s * b[1]; o[2] = a[2] + s * b[2];
}
static void vnorm(float *v)
{
    float n = sqrtf(vdot(v, v));
    v[0] /= n; v[1] /= n; v[2] /= n;
}
static void unit(float *v)                     /* a uniform direction, by rejection: no trig */
{
    for (;;) {
        rvec(v, -1, 1, -1, 1, -1, 1);
        float q = vdot(v, v);
        if (q > 0.01f && q <= 1.0f) { vnorm(v); return; }
    }
}
static void in_ball(float *v, float r)
{
    for (;;) {
        rvec(v, -1, 1, -1, 1, -1, 1);
        if (vdot(v, v) <= 1.0f) { v[0] *= r; v[1] *= r; v[2] *= r; return; }
    }
}
static void perp(float *t, const float *n)    /* a random unit vector orthogonal to n */
{
    float r[3];
    for (;;) {
        unit(r);
        float k = vdot(r, n);
        vmad(t, r, -k, n);
        if (vdot(t, t) > 0.05f) { vnorm(t); return; }
    }
}

/* A rigid transform in Karma's convention: rows 0-2 are the local axes in world space, row 3 the
   origin, so world = l.x*R0 + l.y*R1 + l.z*R2 + T. Rotation from a unit quaternion. */
static void rotation(MeMatrix4 tm)
{
    float q[4], n;
    do {
        q[0] = uab(-1, 1); q[1] = uab(-1, 1); q[2] = uab(-1, 1); q[3] = uab(-1, 1);
        n = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
    } while (n < 0.05f || n > 1.0f);
    n = sqrtf(n);
    float w = q[0] / n, x = q[1] / n, y = q[2] / n, z = q[3] / n;
    tm[0][0] = 1 - 2 * (y * y + z * z); tm[0][1] = 2 * (x * y + w * z);     tm[0][2] = 2 * (x * z - w * y);
    tm[1][0] = 2 * (x * y - w * z);     tm[1][1] = 1 - 2 * (x * x + z * z); tm[1][2] = 2 * (y * z + w * x);
    tm[2][0] = 2 * (x * z + w * y);     tm[2][1] = 2 * (y * z - w * x);     tm[2][2] = 1 - 2 * (x * x + y * y);
    tm[0][3] = tm[1][3] = tm[2][3] = 0; tm[3][3] = 1;
}
/* One of the 24 proper rotations whose entries are 0 and +-1: an axis-parallel world segment is then
   EXACTLY axis-parallel in the local frame, with zero direction components and no rounding. */
static void axis_rotation(MeMatrix4 tm)
{
    static const int perms[6][3] = { {0,1,2}, {1,2,0}, {2,0,1}, {0,2,1}, {2,1,0}, {1,0,2} };
    for (;;) {
        int p = (int)(rnext() % 6), s = (int)(rnext() % 8), i, j;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) tm[i][j] = 0;
            tm[i][perms[p][i]] = (s >> i) & 1 ? -1.0f : 1.0f;
        }
        float det = tm[0][0] * (tm[1][1] * tm[2][2] - tm[1][2] * tm[2][1])
                  - tm[0][1] * (tm[1][0] * tm[2][2] - tm[1][2] * tm[2][0])
                  + tm[0][2] * (tm[1][0] * tm[2][1] - tm[1][1] * tm[2][0]);
        if (det > 0) break;
    }
    tm[0][3] = tm[1][3] = tm[2][3] = 0; tm[3][3] = 1;
}
static void to_world(float *w, const MeMatrix4 tm, const float *l)
{
    int k;
    for (k = 0; k < 3; k++)
        w[k] = l[0] * tm[0][k] + l[1] * tm[1][k] + l[2] * tm[2][k] + tm[3][k];
}
static void dir_to_world(float *w, const MeMatrix4 tm, const float *l)
{
    int k;
    for (k = 0; k < 3; k++)
        w[k] = l[0] * tm[0][k] + l[1] * tm[1][k] + l[2] * tm[2][k];
}

/* ---- the shapes, in their local frames ------------------------------------------------------ */

enum { K_SPHERE, K_BOX, K_PLANE, K_CYLINDER, K_SPHYL, K_CONVEX, K_AGGREGATE, K_COUNT };
static const char *kname[K_COUNT] = { "sphere", "box", "plane", "cylinder", "sphyl", "convex", "aggregate" };

#define NSHAPE 6          /* geometries per kind, cycled over the cases */
#define NVERT 14          /* convex hull input points */
#define NCHILD 3          /* aggregate children: a sphyl, a box, a sphere */

#define NFACET 400        /* convex: at most C(14,3) supporting planes */

typedef struct {
    int kind;
    float a, b, c;        /* sphere r | box half-extents | cylinder/sphyl r, half-height */
    float bound;          /* bounding radius about the local origin */
    float vert[NVERT][3]; /* convex: the hull's input points */
    int nfacet;
    double facet[NFACET][4];   /* convex: n.p - w <= 0 inside, the true hull of `vert` */
    int child_kind[NCHILD];
    float child[NCHILD][3];
    MeMatrix4 rel[NCHILD];
    McdGeometryID g;
    McdModelID m;
    MeMatrix4 tm;         /* the model's transform: Karma keeps the POINTER */
    McdLineSegIntersectFnPtr fn;
} Shape;

static Shape shapes[K_COUNT][NSHAPE];

/* a point inside a primitive at `frac` of its extent */
static void inside_prim(float *p, int kind, float a, float b, float c, float frac)
{
    float q[3];
    switch (kind) {
    case K_SPHERE: in_ball(p, frac * a); break;
    case K_BOX: rvec(p, -frac, frac, -frac, frac, -frac, frac); vscale(p, a, b, c); break;
    case K_CYLINDER:
        do { rvec2(q, -1, 1); } while (vdot(q, q) > 1.0f);
        vset(p, q[0] * frac * a, q[1] * frac * a, uab(-frac, frac) * b);
        break;
    case K_SPHYL:
        in_ball(p, frac * a);
        p[2] += uab(-frac, frac) * b;
        break;
    }
}

static void inside(float *p, const Shape *s, float frac)
{
    switch (s->kind) {
    case K_PLANE:                                  /* the solid half-space is z < 0 */
        rvec(p, -2, 2, -2, 2, -2.0f, -0.05f); p[2] *= frac;
        break;
    case K_CONVEX: {                               /* a convex combination, shrunk toward the mean */
        float w[NVERT], sum = 0, mean[3] = {0, 0, 0}, q[3] = {0, 0, 0};
        int i;
        for (i = 0; i < NVERT; i++) {
            float w0 = u01(), w1 = u01(), w2 = u01();   /* one per statement: see rvec */
            w[i] = w0 * w1 * w2; sum += w[i];
        }
        for (i = 0; i < NVERT; i++) {
            vmad(q, q, w[i] / sum, s->vert[i]);
            vmad(mean, mean, 1.0f / NVERT, s->vert[i]);
        }
        for (i = 0; i < 3; i++) p[i] = mean[i] + (q[i] - mean[i]) * frac;
        break;
    }
    case K_AGGREGATE: {
        int i = (int)(rnext() % NCHILD);
        float l[3];
        inside_prim(l, s->child_kind[i], s->child[i][0], s->child[i][1], s->child[i][2], frac);
        to_world(p, s->rel[i], l);
        break;
    }
    default:
        inside_prim(p, s->kind, s->a, s->b, s->c, frac);
    }
}

/* A surface point and its outward normal, for the four convex primitives with a closed form. The
   tangent plane there SUPPORTS the shape, so a line in it offset outward by d > 0 cannot touch the
   shape and one offset inward is inside near the point: a grazing case whose answer is not a coin. */
static int surface(float *s, float *n, float *scale, const Shape *sh)
{
    float q[3];
    switch (sh->kind) {
    case K_SPHERE:
        unit(n); vset(s, n[0] * sh->a, n[1] * sh->a, n[2] * sh->a); *scale = sh->a;
        return 1;
    case K_BOX: {
        int ax = (int)(rnext() % 3);
        float sg = (rnext() & 1) ? 1.0f : -1.0f, r[3] = { sh->a, sh->b, sh->c };
        rvec(s, -0.8f, 0.8f, -0.8f, 0.8f, -0.8f, 0.8f); vscale(s, r[0], r[1], r[2]);
        s[ax] = sg * r[ax];
        vset(n, 0, 0, 0); n[ax] = sg; *scale = r[ax];
        return 1;
    }
    case K_CYLINDER:
    case K_SPHYL:
        if (sh->kind == K_SPHYL && (rnext() & 1)) {          /* a cap: a sphere about (0,0,+-hh) */
            unit(n);
            if (n[2] < 0) n[2] = -n[2];
            float sg = (rnext() & 1) ? 1.0f : -1.0f;
            n[2] *= sg;
            vset(s, n[0] * sh->a, n[1] * sh->a, sg * sh->b + n[2] * sh->a);
            *scale = sh->a;
            return 1;
        }
        if (sh->kind == K_CYLINDER && (rnext() & 1)) {       /* a flat end */
            float sg = (rnext() & 1) ? 1.0f : -1.0f;
            do { rvec2(q, -1, 1); } while (vdot(q, q) > 1.0f);
            vset(s, q[0] * 0.8f * sh->a, q[1] * 0.8f * sh->a, sg * sh->b);
            vset(n, 0, 0, sg); *scale = sh->b;
            return 1;
        }
        do { rvec2(n, -1, 1); } while (vdot(n, n) > 1.0f || vdot(n, n) < 0.01f);
        vnorm(n);
        vset(s, n[0] * sh->a, n[1] * sh->a, uab(-0.95f, 0.95f) * sh->b);
        *scale = sh->a;
        return 1;
    case K_PLANE:
        rvec2(s, -2, 2); vset(n, 0, 0, 1); *scale = 1.0f;
        return 1;
    }
    return 0;
}

/* ---- the ground truth: how deep does the segment go? ---------------------------------------- */
/* The deepest penetration of the segment into the TRUE shape, in double precision and independent of
   Karma: > 0 it enters, < 0 it misses by that much. Every shape here is convex (the aggregate is a
   union of convex children), so the signed distance is convex along the segment and a ternary search
   finds the deepest point. The comparator uses it to tell a knife edge — a segment that barely
   touches, where float rounding legitimately decides hit or miss — from a disagreement about the
   geometry. Computed with IEEE double and sqrt only, so it is bit-identical in every build. */

double sqrt(double);

static double sd_prim(int kind, double a, double b, double c, const double *p)
{
    double q[3], m, o;
    switch (kind) {
    case K_SPHERE:
        return sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]) - a;
    case K_BOX:
        q[0] = (p[0] < 0 ? -p[0] : p[0]) - a; q[1] = (p[1] < 0 ? -p[1] : p[1]) - b;
        q[2] = (p[2] < 0 ? -p[2] : p[2]) - c;
        m = q[0] > q[1] ? q[0] : q[1]; m = m > q[2] ? m : q[2];
        o = 0;
        { int k; for (k = 0; k < 3; k++) if (q[k] > 0) o += q[k] * q[k]; }
        return m < 0 ? m : sqrt(o);
    case K_CYLINDER: {
        double dr = sqrt(p[0] * p[0] + p[1] * p[1]) - a, dz = (p[2] < 0 ? -p[2] : p[2]) - b;
        m = dr > dz ? dr : dz;
        if (m < 0) return m;
        o = (dr > 0 ? dr * dr : 0) + (dz > 0 ? dz * dz : 0);
        return sqrt(o);
    }
    case K_SPHYL: {
        double z = p[2] < -b ? -b : p[2] > b ? b : p[2];
        return sqrt(p[0] * p[0] + p[1] * p[1] + (p[2] - z) * (p[2] - z)) - a;
    }
    case K_PLANE:
        return p[2];
    }
    return 0;
}

static double sd_shape(const Shape *s, int child, const double *p)
{
    int i;
    double m = -1e300;
    if (s->kind == K_CONVEX) {
        for (i = 0; i < s->nfacet; i++) {
            double v = s->facet[i][0] * p[0] + s->facet[i][1] * p[1] + s->facet[i][2] * p[2]
                       - s->facet[i][3];
            if (v > m) m = v;
        }
        return m;
    }
    if (s->kind == K_AGGREGATE)
        return sd_prim(s->child_kind[child], s->child[child][0], s->child[child][1],
                       s->child[child][2], p);
    return sd_prim(s->kind, s->a, s->b, s->c, p);
}

static void local_d(double *l, MeMatrix4 tm, const double *w)  /* inverse of a rigid transform */
{
    int k;
    for (k = 0; k < 3; k++)
        l[k] = (w[0] - tm[3][0]) * tm[k][0] + (w[1] - tm[3][1]) * tm[k][1] + (w[2] - tm[3][2]) * tm[k][2];
}

static double depth_at(const Shape *s, int child, const double *a, const double *b, double t)
{
    double p[3];
    p[0] = a[0] + t * (b[0] - a[0]); p[1] = a[1] + t * (b[1] - a[1]); p[2] = a[2] + t * (b[2] - a[2]);
    return -sd_shape(s, child, p);
}

static double depth_along(const Shape *s, int child, const double *a, const double *b)
{
    double lo = 0, hi = 1, best, v;
    int i;
    for (i = 0; i < 100; i++) {                 /* the depth is concave along the segment */
        double m1 = lo + (hi - lo) / 3, m2 = hi - (hi - lo) / 3;
        if (depth_at(s, child, a, b, m1) < depth_at(s, child, a, b, m2)) lo = m1; else hi = m2;
    }
    best = depth_at(s, child, a, b, (lo + hi) / 2);
    v = depth_at(s, child, a, b, 0.0); if (v > best) best = v;
    v = depth_at(s, child, a, b, 1.0); if (v > best) best = v;
    return best;
}

static double depth(Shape *s, const float *o, const float *d)
{
    double wo[3] = { o[0], o[1], o[2] }, wd[3] = { d[0], d[1], d[2] }, lo[3], ld[3], co[3], cd[3];
    double best = -1e300, v;
    int i;
    local_d(lo, s->tm, wo); local_d(ld, s->tm, wd);
    if (s->kind != K_AGGREGATE) return depth_along(s, 0, lo, ld);
    for (i = 0; i < NCHILD; i++) {
        local_d(co, s->rel[i], lo); local_d(cd, s->rel[i], ld);
        v = depth_along(s, i, co, cd);
        if (v > best) best = v;
    }
    return best;
}

/* the true hull of the input points, by brute force: a triple's plane is a facet when no point is
   outside it */
static void hull_facets(Shape *s)
{
    int i, j, k, l;
    s->nfacet = 0;
    for (i = 0; i < NVERT; i++) for (j = i + 1; j < NVERT; j++) for (k = j + 1; k < NVERT; k++) {
        double u[3], v[3], n[3], w, len;
        int q, pos = 0, neg = 0;
        for (q = 0; q < 3; q++) { u[q] = (double)s->vert[j][q] - s->vert[i][q];
                                  v[q] = (double)s->vert[k][q] - s->vert[i][q]; }
        n[0] = u[1] * v[2] - u[2] * v[1]; n[1] = u[2] * v[0] - u[0] * v[2]; n[2] = u[0] * v[1] - u[1] * v[0];
        len = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
        if (len < 1e-12) continue;
        n[0] /= len; n[1] /= len; n[2] /= len;
        w = n[0] * s->vert[i][0] + n[1] * s->vert[i][1] + n[2] * s->vert[i][2];
        for (l = 0; l < NVERT; l++) {
            double e = n[0] * s->vert[l][0] + n[1] * s->vert[l][1] + n[2] * s->vert[l][2] - w;
            if (e > 1e-12) pos++;
            if (e < -1e-12) neg++;
        }
        if (pos && neg) continue;
        if (pos) { n[0] = -n[0]; n[1] = -n[1]; n[2] = -n[2]; w = -w; }
        if (s->nfacet < NFACET) {
            s->facet[s->nfacet][0] = n[0]; s->facet[s->nfacet][1] = n[1];
            s->facet[s->nfacet][2] = n[2]; s->facet[s->nfacet][3] = w; s->nfacet++;
        }
    }
}

/* ---- running one case --------------------------------------------------------------------- */

static void __attribute__((noinline)) poison_stack(unsigned char v)
{
    volatile unsigned char buf[16384];
    int i;
    for (i = 0; i < (int)sizeof buf; i++) buf[i] = v;
}

static int __attribute__((noinline)) call_once(McdLineSegIntersectFnPtr fn, McdModelID m,
                                                const float *o, const float *d, unsigned char pat,
                                                McdLineSegIntersectResult *r)
{
    MeReal oo[3], dd[3];
    memcpy(oo, o, sizeof oo); memcpy(dd, d, sizeof dd);
    memset(r, pat, sizeof *r);
    poison_stack(pat);
    return fn(m, oo, dd, r);
}

static unsigned int bits(float f) { unsigned int u; memcpy(&u, &f, 4); return u; }

static unsigned int digest;
static void dg(const void *p, int n)
{
    const unsigned char *c = (const unsigned char *)p;
    int i;
    for (i = 0; i < n; i++) { digest ^= c[i]; digest *= 16777619u; }
}

static void run_case(Shape *s, int cat, int idx, const float *o, const float *d)
{
    McdLineSegIntersectResult r1, r2;
    int ret1, ret2, mask = 0, k;
    float v1[7], v2[7];

    McdModelUpdate(s->m);
    ret1 = call_once(s->fn, s->m, o, d, 0x3f, &r1);
    ret2 = call_once(s->fn, s->m, o, d, 0x41, &r2);
    v1[0] = r1.distance; v2[0] = r2.distance;
    for (k = 0; k < 3; k++) {
        v1[1 + k] = r1.position[k]; v2[1 + k] = r2.position[k];
        v1[4 + k] = r1.normal[k];   v2[4 + k] = r2.normal[k];
    }
    if (ret1 == ret2) mask |= 1;
    for (k = 0; k < 7; k++)
        if (bits(v1[k]) == bits(v2[k])) mask |= 2 << k;

    digest = 2166136261u;
    dg(&s->kind, sizeof s->kind); dg(&cat, sizeof cat); dg(&idx, sizeof idx);
    dg(s->tm, sizeof(MeMatrix4)); dg(o, 12); dg(d, 12);
    float seg[3];
    vmad(seg, d, -1.0f, o);
    printf("%s %d %d %08x %08x %08x %d %02x", kname[s->kind], cat, idx, digest,
           bits(sqrtf(vdot(seg, seg))), bits((float)depth(s, o, d)), ret1, mask);
    for (k = 0; k < 7; k++) printf(" %08x", bits(v1[k]));
    printf("\n");
}

/* ---- the categories ----------------------------------------------------------------------- */

enum { C_RANDOM, C_START_INSIDE, C_END_INSIDE, C_THROUGH, C_GRAZING, C_AXIS, C_DEGENERATE, C_COUNT };

static void place(Shape *s, int axis)
{
    if (axis) axis_rotation(s->tm); else rotation(s->tm);
    /* UT2004 positions in Karma units are U * 0.02: a map spans a few hundred */
    s->tm[3][0] = uab(-40, 40); s->tm[3][1] = uab(-40, 40); s->tm[3][2] = uab(-40, 40);
}

static void one(Shape *s, int cat, int idx)
{
    float lo[3], ld[3], o[3], d[3], p[3], n[3], t[3], sc;
    float B = s->bound;

    /* every case draws from its own stream, so (geometry, category, index) names the same segment
       whatever the case count — which is what lets baselines.sh name a case */
    rs = 2166136261u;
    rs = (rs ^ (unsigned)s->kind) * 16777619u; rs = (rs ^ (unsigned)cat) * 16777619u;
    rs = (rs ^ (unsigned)idx) * 16777619u; rs ^= rs >> 15; rs |= 1;
    place(s, cat == C_AXIS);
    switch (cat) {
    case C_RANDOM:
        rvec(lo, -3, 3, -3, 3, -3, 3); vscale(lo, B, B, B);
        rvec(ld, -3, 3, -3, 3, -3, 3); vscale(ld, B, B, B);
        break;
    case C_START_INSIDE:
        inside(lo, s, 0.9f); unit(t); vmad(ld, lo, uab(1.5f, 20.0f) * B, t);
        break;
    case C_END_INSIDE:
        inside(ld, s, 0.9f); unit(t); vmad(lo, ld, uab(1.5f, 20.0f) * B, t);
        break;
    case C_THROUGH:                       /* through a point near the middle, both ends outside */
        inside(p, s, 0.5f); unit(t);
        vmad(lo, p, -uab(2.5f, 10.0f) * B, t); vmad(ld, p, uab(2.5f, 10.0f) * B, t);
        break;
    case C_GRAZING: {                     /* +-1/64 and +-1/1024 of the local scale off a support plane */
        static const float f[4] = { 1.0f / 64, -1.0f / 64, 1.0f / 1024, -1.0f / 1024 };
        if (!surface(p, n, &sc, s)) { inside(p, s, 0.5f); unit(n); sc = B; }
        perp(t, n);
        vmad(p, p, f[idx & 3] * sc, n);
        vmad(lo, p, -uab(1.5f, 6.0f) * B, t); vmad(ld, p, uab(1.5f, 6.0f) * B, t);
        break;
    }
    case C_AXIS: {                        /* parallel to a WORLD axis, which `place` made a local one */
        float w0[3], wd[3] = { 0, 0, 0 }, L = uab(1.5f, 8.0f) * B;
        int ax = (int)(rnext() % 3);
        if (idx & 1) inside(p, s, 0.95f);
        else { rvec(p, -2, 2, -2, 2, -2, 2); vscale(p, B, B, B); }
        to_world(w0, s->tm, p);
        wd[ax] = (idx & 2) ? L : -L;
        vmad(o, w0, -1.0f, wd); vmad(d, w0, 1.0f, wd);
        run_case(s, cat, idx, o, d);
        return;
    }
    case C_DEGENERATE:
        switch (idx & 3) {
        case 0: rvec(lo, -3, 3, -3, 3, -3, 3); vscale(lo, B, B, B);         /* zero length, anywhere */
                memcpy(ld, lo, sizeof lo); break;
        case 1: inside(lo, s, 0.9f); memcpy(ld, lo, sizeof lo); break;      /* zero length, inside */
        case 2: inside(lo, s, 0.9f); unit(t); vmad(ld, lo, 1e-3f * B, t); break;   /* tiny, inside */
        default: unit(t); inside(p, s, 0.5f);       /* long and through: 100x the bound, a shot */
                 vmad(lo, p, -50.0f * B, t); vmad(ld, p, 50.0f * B, t); break;   /* from across a room */
        }
        break;
    }
    to_world(o, s->tm, lo);
    to_world(d, s->tm, ld);
    run_case(s, cat, idx, o, d);
}

/* ---- building the shapes ------------------------------------------------------------------ */

static void make(McdFrameworkID f, Shape *s, int kind, int i)
{
    int k;
    memset(s, 0, sizeof *s);
    s->kind = kind;
    switch (kind) {
    case K_SPHERE:
        s->a = uab(0.05f, 0.6f); s->bound = s->a;
        s->g = (McdGeometryID)McdSphereCreate(f, s->a);
        break;
    case K_BOX:
        s->a = uab(0.05f, 0.6f); s->b = uab(0.05f, 0.6f); s->c = (i == 0) ? 0.002f : uab(0.05f, 0.6f);
        s->bound = sqrtf(s->a * s->a + s->b * s->b + s->c * s->c);
        s->g = (McdGeometryID)McdBoxCreate(f, 2 * s->a, 2 * s->b, 2 * s->c);
        break;
    case K_PLANE:
        s->bound = 1.0f;
        s->g = (McdGeometryID)McdPlaneCreate(f);
        break;
    case K_CYLINDER:
        s->a = uab(0.05f, 0.5f); s->b = (i == 0) ? 0.003f : uab(0.05f, 0.6f);
        s->bound = sqrtf(s->a * s->a + s->b * s->b);
        s->g = (McdGeometryID)McdCylinderCreate(f, s->a, 2 * s->b);
        break;
    case K_SPHYL:                        /* shape 0 has no cylinder part: a sphyl that is a sphere */
        s->a = uab(0.05f, 0.4f); s->b = (i == 0) ? 0.0f : uab(0.02f, 0.6f);
        s->bound = s->a + s->b;
        s->g = (McdGeometryID)McdSphylCreate(f, s->a, 2 * s->b);
        break;
    case K_CONVEX: {
        MeVector3 v[NVERT];
        s->bound = 0;
        for (k = 0; k < NVERT; k++) {
            unit(s->vert[k]);
            float r = uab(0.15f, 0.45f);
            s->vert[k][0] *= r; s->vert[k][1] *= r * (i == 0 ? 0.3f : 1.0f); s->vert[k][2] *= r;
            v[k][0] = s->vert[k][0]; v[k][1] = s->vert[k][1]; v[k][2] = s->vert[k][2];
            float q = sqrtf(vdot(s->vert[k], s->vert[k]));
            if (q > s->bound) s->bound = q;
        }
        s->g = (McdGeometryID)McdConvexMeshCreateHull(f, (const MeVector3 *)v, NVERT, 0);
        hull_facets(s);
        break;
    }
    case K_AGGREGATE: {
        s->g = (McdGeometryID)McdAggregateCreate(f, NCHILD);
        s->bound = 0;
        for (k = 0; k < NCHILD; k++) {
            McdGeometryID cg;
            float off[3];
            s->child_kind[k] = (k == 0) ? K_SPHYL : (k == 1) ? K_BOX : K_SPHERE;
            s->child[k][0] = uab(0.05f, 0.25f); s->child[k][1] = uab(0.05f, 0.3f);
            s->child[k][2] = uab(0.05f, 0.25f);
            if (k == 0) cg = (McdGeometryID)McdSphylCreate(f, s->child[k][0], 2 * s->child[k][1]);
            else if (k == 1) cg = (McdGeometryID)McdBoxCreate(f, 2 * s->child[k][0],
                                                               2 * s->child[k][1], 2 * s->child[k][2]);
            else { s->child[k][1] = s->child[k][2] = 0;
                   cg = (McdGeometryID)McdSphereCreate(f, s->child[k][0]); }
            rotation(s->rel[k]);
            in_ball(off, 0.3f);
            s->rel[k][3][0] = off[0]; s->rel[k][3][1] = off[1]; s->rel[k][3][2] = off[2];
            McdAggregateAddElement((McdAggregateID)s->g, cg, s->rel[k]);
            float q = sqrtf(vdot(off, off)) + s->child[k][0] + s->child[k][1] + s->child[k][2];
            if (q > s->bound) s->bound = q;
        }
        break;
    }
    }
    s->m = McdModelCreate(s->g);
    MeMatrix4TMMakeIdentity(s->tm);
    McdModelSetTransformPtr(s->m, s->tm);
    s->fn = McdGeometryGetLineSegIntersectFnPtr(s->g);
}

int main(int argc, char **argv)
{
    int n = argc > 1 ? atoi(argv[1]) : 2500, kind, cat, i;
    McdFrameworkID f = McdInit(16, 256, 256, 1);

    /* the game's own registration, in its order (engine-ut2004 KarmaSupport.cpp) */
    McdPrimitivesRegisterTypes(f);
    McdConvexMeshRegisterType(f);
    McdAggregateRegisterType(f);
    McdNullRegisterType(f);
    McdPrimitivesRegisterInteractions(f);
    McdConvexMeshPrimitivesRegisterInteractions(f);
    McdAggregateRegisterInteractions(f);

    printf("# lineseg_lp64 pointer=%d cases-per-category=%d\n", (int)sizeof(void *), n);
    for (kind = 0; kind < K_COUNT; kind++) {
        for (i = 0; i < NSHAPE; i++) make(f, &shapes[kind][i], kind, i);
        printf("# %s typeid=%d linesegfn=%s\n", kname[kind],
               (int)McdGeometryGetTypeId(shapes[kind][0].g), shapes[kind][0].fn ? "yes" : "no");
    }
    for (kind = 0; kind < K_COUNT; kind++) {
        if (!shapes[kind][0].fn) continue;
        for (cat = 0; cat < C_COUNT; cat++)
            for (i = 0; i < n; i++)
                one(&shapes[kind][i % NSHAPE], cat, i);
    }
    return 0;
}
