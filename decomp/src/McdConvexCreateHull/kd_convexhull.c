/*
 * kd_convexhull.c — McdComputeHull and friends, without qhull.
 *
 * Replaces libMcdConvexCreateHull.a (qhull 2.6, 1998, 186 KB, global-state C)
 * for HANDOVER.md §12 item 4. Written against MEASUREMENT rather than against
 * McdQHullTypes.h's prose: test/hull_probe.sh runs the shipped implementation
 * and checks every claim that header makes, and §8a records what it found. The
 * same probe is this file's acceptance test, because equality is NOT available
 * — the hull reindexes, so a differently indexed but geometrically identical
 * answer is legitimate.
 *
 * WHY NOT VENDOR MODERN QHULL. The adapter is the bulk of the work either way:
 * qhull produces facets, and what Karma needs is a half-edge structure with
 * coplanar facets merged, edges stored twice, grouped by leftFace, wound ACW
 * seen from outside, plus an edgeIndex giving each vertex its outgoing edges,
 * plus sentinels. Building that from qhull's output is most of this file. A
 * self-contained implementation additionally drops a third-party build
 * integration and ~500 KB of source, which matters for §12 item 6 (wasm32 and
 * arm64 as ordinary C).
 *
 * WHAT THIS HAS TO GET RIGHT, and why each one is load-bearing:
 *
 *   - COPLANAR MERGING. A cube is 6 four-sided faces, not 12 triangles. A
 *     triangle-only hull passes Euler and every ring check and is still wrong.
 *   - WINDING, ACW seen from outside. Ring closure and outward normals both
 *     hold for EITHER handedness, so nothing but an explicit test catches it.
 *   - GROUPING BY leftFace. The header also says an edge names "the face on its
 *     right side"; measurement settled that face f's range holds the edges with
 *     leftFace == f.
 *   - THE DEGENERACY CUTOFF. `McdConvexMeshCreateHull` only checks the return
 *     value and yields NULL, so accepting a box the shipped code rejects
 *     invents collision geometry the game never had, and rejecting more removes
 *     geometry it did have. Neither presents as a crash. The shipped cutoff
 *     sits between 1e-6 and 1e-5 relative; see hull_thin_reject below.
 *
 * The adjacency this builds is not a load-time curiosity: McdConvexMeshCreateHull
 * stores the whole structure in the geometry object and
 * McdConvexMeshMaximumPointLocal — GJK's support function — hill-climbs it on
 * every query, which per the census is ~685,000 calls per match.
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <MeMath.h>
#include <MeMemory.h>
#include <McdQHullTypes.h>

/* ------------------------------------------------------------------ vectors */

typedef struct { double x, y, z; } V3;

static V3   v3(double x, double y, double z) { V3 r; r.x=x; r.y=y; r.z=z; return r; }
static V3   v3sub(V3 a, V3 b) { return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
static double v3dot(V3 a, V3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static V3   v3cross(V3 a, V3 b)
{
    return v3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}
static double v3len(V3 a) { return sqrt(v3dot(a, a)); }

/* ------------------------------------------------------------ working state */

typedef struct {
    int  v[3];          /* vertex indices, wound ACW seen from outside */
    V3   n;             /* unit outward normal */
    double d;           /* plane offset: dot(n, x) == d on the face */
    int  alive;
    int  face;          /* merged-face id, assigned in phase 4 */
} Tri;

typedef struct {
    V3  *pt;            /* deduplicated input points */
    int  npt;
    Tri *tri;
    int  ntri, captri;
    double scale;       /* bounding-box extent; every tolerance is relative to it */
} Build;

static int tri_push(Build *b, int a, int c, int d)
{
    Tri t;
    V3 e1, e2, n;
    double l;
    if (b->ntri == b->captri) {
        int cap = b->captri ? b->captri * 2 : 64;
        Tri *p = (Tri *)realloc(b->tri, cap * sizeof(Tri));
        if (!p) return 0;
        b->tri = p; b->captri = cap;
    }
    t.v[0] = a; t.v[1] = c; t.v[2] = d;
    e1 = v3sub(b->pt[c], b->pt[a]);
    e2 = v3sub(b->pt[d], b->pt[a]);
    n  = v3cross(e1, e2);
    l  = v3len(n);
    if (l <= 0.0) return 0;
    t.n = v3(n.x/l, n.y/l, n.z/l);
    t.d = v3dot(t.n, b->pt[a]);
    t.alive = 1;
    t.face  = -1;
    b->tri[b->ntri++] = t;
    return 1;
}

/* ------------------------------------------------------------------ phase 1 */
/* Deduplicate. The shipped code collapses repeated vertices (measured: a
 * tetrahedron with every vertex doubled comes back V=4), and leaving them in
 * would produce zero-length edges and degenerate triangles downstream. */

static int dedup(Build *b, const MeVector3 *src, int n)
{
    int i, j;
    double tol;
    V3 lo, hi;

    b->pt = (V3 *)malloc((size_t)n * sizeof(V3));
    if (!b->pt) return 0;
    b->npt = 0;

    lo = v3(1e300, 1e300, 1e300);
    hi = v3(-1e300, -1e300, -1e300);
    for (i = 0; i < n; i++) {
        double x = src[i][0], y = src[i][1], z = src[i][2];
        if (x < lo.x) lo.x = x;
        if (x > hi.x) hi.x = x;
        if (y < lo.y) lo.y = y;
        if (y > hi.y) hi.y = y;
        if (z < lo.z) lo.z = z;
        if (z > hi.z) hi.z = z;
    }
    b->scale = hi.x - lo.x;
    if (hi.y - lo.y > b->scale) b->scale = hi.y - lo.y;
    if (hi.z - lo.z > b->scale) b->scale = hi.z - lo.z;
    if (b->scale <= 0.0) return 0;                 /* all points identical */
    tol = b->scale * 1e-7;

    for (i = 0; i < n; i++) {
        V3 p = v3(src[i][0], src[i][1], src[i][2]);
        for (j = 0; j < b->npt; j++) {
            if (fabs(p.x - b->pt[j].x) <= tol &&
                fabs(p.y - b->pt[j].y) <= tol &&
                fabs(p.z - b->pt[j].z) <= tol) break;
        }
        if (j == b->npt) b->pt[b->npt++] = p;
    }
    return 1;
}

/* ------------------------------------------------------------------ phase 2 */
/* An initial tetrahedron. Failure here is the degeneracy answer: fewer than
 * four points, or all of them coplanar/collinear, means there is no solid and
 * McdComputeHull returns 0 — which is what the shipped code does. */

static int initial_tetra(Build *b, double eps)
{
    int i, ia = 0, ib = 0, ic = -1, id = -1;
    double best = -1.0;
    V3 e1, nrm;
    double nl;

    if (b->npt < 4) return 0;

    /* Two points furthest apart along the widest axis, then genuinely furthest. */
    for (i = 1; i < b->npt; i++) {
        if (b->pt[i].x < b->pt[ia].x) ia = i;
        if (b->pt[i].x > b->pt[ib].x) ib = i;
    }
    if (ia == ib) return 0;
    for (i = 0; i < b->npt; i++) {
        double dd = v3len(v3sub(b->pt[i], b->pt[ia]));
        if (dd > best) { best = dd; ib = i; }
    }
    if (best <= eps) return 0;

    /* Third: furthest from the line ab. */
    e1 = v3sub(b->pt[ib], b->pt[ia]);
    best = -1.0;
    for (i = 0; i < b->npt; i++) {
        V3 c = v3cross(e1, v3sub(b->pt[i], b->pt[ia]));
        double dd = v3len(c) / v3len(e1);
        if (dd > best) { best = dd; ic = i; }
    }
    if (ic < 0 || best <= eps) return 0;           /* collinear */

    /* Fourth: furthest off the plane abc. */
    nrm = v3cross(e1, v3sub(b->pt[ic], b->pt[ia]));
    nl = v3len(nrm);
    if (nl <= 0.0) return 0;
    nrm = v3(nrm.x/nl, nrm.y/nl, nrm.z/nl);
    best = -1.0;
    for (i = 0; i < b->npt; i++) {
        double dd = fabs(v3dot(nrm, v3sub(b->pt[i], b->pt[ia])));
        if (dd > best) { best = dd; id = i; }
    }
    /* THE DEGENERACY CUTOFF. Everything coplanar to within eps is "not a solid".
     * eps is relative to the bounding box, which is what makes the shipped
     * behaviour reproducible at any scale. */
    if (id < 0 || best <= eps) return 0;           /* coplanar / too thin */

    /* Wind both seed triangles outward, then close the tetrahedron. */
    if (v3dot(nrm, v3sub(b->pt[id], b->pt[ia])) > 0.0) {
        if (!tri_push(b, ia, ic, ib)) return 0;
    } else {
        if (!tri_push(b, ia, ib, ic)) return 0;
    }
    {
        Tri base = b->tri[0];
        if (!tri_push(b, base.v[0], base.v[2], id)) return 0;
        if (!tri_push(b, base.v[2], base.v[1], id)) return 0;
        if (!tri_push(b, base.v[1], base.v[0], id)) return 0;
    }
    return 1;
}

/* ------------------------------------------------------------------ phase 3 */
/* Incremental hull: for each point outside the current shell, delete every face
 * it can see and rebuild a cone from the horizon. */

static int add_points(Build *b, double eps)
{
    int p, i, k;
    int *horiz = NULL, ncap = 0;

    for (p = 0; p < b->npt; p++) {
        int nvis = 0, nh = 0;
        for (i = 0; i < b->ntri; i++) {
            if (!b->tri[i].alive) continue;
            if (v3dot(b->tri[i].n, b->pt[p]) - b->tri[i].d > eps) {
                b->tri[i].alive = 0;
                nvis++;
            }
        }
        if (!nvis) continue;

        /* Horizon = directed edges of dead faces whose reverse is not also dead.
         * Collect them, then fan to p. Keeping the dead face's winding makes the
         * new triangles come out wound outward automatically. */
        if (ncap < b->ntri * 3) {
            int cap = b->ntri * 3 + 64;
            int *q = (int *)realloc(horiz, (size_t)cap * 2 * sizeof(int));
            if (!q) { free(horiz); return 0; }
            horiz = q; ncap = cap;
        }
        for (i = 0; i < b->ntri; i++) {
            if (b->tri[i].alive) continue;
            for (k = 0; k < 3; k++) {
                int a = b->tri[i].v[k], c = b->tri[i].v[(k + 1) % 3];
                int j, shared = 0;
                for (j = 0; j < b->ntri && !shared; j++) {
                    int m;
                    if (b->tri[j].alive || j == i) continue;
                    for (m = 0; m < 3; m++)
                        if (b->tri[j].v[m] == c && b->tri[j].v[(m + 1) % 3] == a) {
                            shared = 1; break;
                        }
                }
                if (!shared) { horiz[nh * 2] = a; horiz[nh * 2 + 1] = c; nh++; }
            }
        }

        /* Reclaim the dead faces BEFORE fanning. Without this the array only
         * ever grows, every later horizon scan is quadratic in the accumulated
         * corpses rather than in the live hull, and a 200-point cloud does not
         * finish. Found by the probe hanging, not by reading the code. */
        {
            int w = 0;
            for (i = 0; i < b->ntri; i++)
                if (b->tri[i].alive) b->tri[w++] = b->tri[i];
            b->ntri = w;
        }
        for (i = 0; i < nh; i++)
            if (!tri_push(b, horiz[i * 2], horiz[i * 2 + 1], p)) { free(horiz); return 0; }
    }
    free(horiz);
    return b->ntri >= 4;
}

/* ------------------------------------------------------------------ phase 4 */
/* Merge coplanar triangles into polygonal faces. This is what makes a cube six
 * quads rather than twelve triangles, and it is not cosmetic: the collision code
 * reads these as polygons. */

static int merge_faces(Build *b, double eps)
{
    int i, j, nface = 0;
    double cos_tol = 1e-6;

    for (i = 0; i < b->ntri; i++) b->tri[i].face = -1;
    for (i = 0; i < b->ntri; i++) {
        if (b->tri[i].face >= 0) continue;
        b->tri[i].face = nface;
        for (j = i + 1; j < b->ntri; j++) {
            if (b->tri[j].face >= 0) continue;
            /* Same plane: normals parallel AND offsets equal. Comparing only the
             * normal would merge the two sides of a flat sliver. */
            if (v3dot(b->tri[i].n, b->tri[j].n) > 1.0 - cos_tol &&
                fabs(b->tri[i].d - b->tri[j].d) <= eps)
                b->tri[j].face = nface;
        }
        nface++;
    }
    return nface;
}

/* ------------------------------------------------------------------ phase 5 */
/* Emit the structure McdQHullTypes.h describes. */

typedef struct { int from, to, face; } DEdge;

static int build_hull(Build *b, int nface, McdConvexHull *out)
{
    int i, j, k, f, ne = 0, nv = b->npt;
    DEdge *ed = NULL;
    int *vremap = NULL, *loop = NULL;
    int nedge_total = b->ntri * 3;
    int rc = 0;

    ed = (DEdge *)malloc((size_t)nedge_total * sizeof(DEdge));
    loop = (int *)malloc((size_t)nedge_total * 2 * sizeof(int));
    vremap = (int *)malloc((size_t)nv * sizeof(int));
    if (!ed || !loop || !vremap) goto done;

    /* Boundary of each merged face: directed edges of its triangles whose
     * reverse is NOT also in the same face. Chained into one closed ring, which
     * inherits the triangles' outward winding — i.e. ACW seen from outside. */
    for (f = 0; f < nface; f++) {
        int nb = 0, start;
        for (i = 0; i < b->ntri; i++) {
            if (b->tri[i].face != f) continue;
            for (k = 0; k < 3; k++) {
                int a = b->tri[i].v[k], c = b->tri[i].v[(k + 1) % 3];
                int interior = 0;
                for (j = 0; j < b->ntri && !interior; j++) {
                    int m;
                    if (b->tri[j].face != f || j == i) continue;
                    for (m = 0; m < 3; m++)
                        if (b->tri[j].v[m] == c && b->tri[j].v[(m + 1) % 3] == a) {
                            interior = 1; break;
                        }
                }
                if (!interior) { loop[nb * 2] = a; loop[nb * 2 + 1] = c; nb++; }
            }
        }
        if (nb < 3) goto done;

        /* Chain: each edge's `to` is the next edge's `from`. */
        start = ne;
        {
            int cur = loop[0], first = loop[0], used = 0;
            while (used < nb) {
                int found = -1;
                for (i = 0; i < nb; i++)
                    if (loop[i * 2] == cur && loop[i * 2 + 1] >= 0) { found = i; break; }
                if (found < 0) goto done;              /* boundary is not a ring */
                ed[ne].from = cur;
                ed[ne].to   = loop[found * 2 + 1];
                ed[ne].face = f;
                cur = loop[found * 2 + 1];
                loop[found * 2 + 1] = -1 - loop[found * 2 + 1];  /* mark consumed */
                ne++; used++;
            }
            if (cur != first) goto done;               /* did not close */
            (void)start;
        }
    }

    /* Drop vertices no face uses (interior points), and renumber. */
    for (i = 0; i < nv; i++) vremap[i] = -1;
    {
        int w = 0;
        for (i = 0; i < ne; i++) {
            if (vremap[ed[i].from] < 0) vremap[ed[i].from] = w++;
        }
        nv = w;
    }
    for (i = 0; i < ne; i++) {
        ed[i].from = vremap[ed[i].from];
        ed[i].to   = vremap[ed[i].to];
        if (ed[i].from < 0 || ed[i].to < 0) goto done;
    }

    /* Allocate: one extra face and vertex for the sentinels. */
    out->numVertex = nv;
    out->numFace   = nface;
    out->numEdge   = ne;
    out->vertex    = (McdCnvVertex *)MeMemoryAPI.create((nv + 1) * sizeof(McdCnvVertex));
    out->face      = (McdCnvFace *)  MeMemoryAPI.create((nface + 1) * sizeof(McdCnvFace));
    out->edge      = (McdCnvEdge *)  MeMemoryAPI.create(ne * sizeof(McdCnvEdge));
    out->edgeIndex = (int *)         MeMemoryAPI.create(ne * sizeof(int));
    if (!out->vertex || !out->face || !out->edge || !out->edgeIndex) goto done;

    for (i = 0; i < b->npt; i++) {
        if (vremap[i] < 0) continue;
        out->vertex[vremap[i]].position[0] = (MeReal)b->pt[i].x;
        out->vertex[vremap[i]].position[1] = (MeReal)b->pt[i].y;
        out->vertex[vremap[i]].position[2] = (MeReal)b->pt[i].z;
    }

    /* Faces: normal from the owning triangle, firstEdge from the edge order
     * above (edges were emitted face by face, so ranges are already contiguous
     * and ascending). */
    for (f = 0; f < nface; f++) {
        int owner = -1;
        for (i = 0; i < b->ntri; i++) if (b->tri[i].face == f) { owner = i; break; }
        if (owner < 0) goto done;
        out->face[f].normal[0] = (MeReal)b->tri[owner].n.x;
        out->face[f].normal[1] = (MeReal)b->tri[owner].n.y;
        out->face[f].normal[2] = (MeReal)b->tri[owner].n.z;
        out->face[f].firstEdge = -1;
    }
    for (i = 0; i < ne; i++)
        if (out->face[ed[i].face].firstEdge < 0) out->face[ed[i].face].firstEdge = i;
    out->face[nface].firstEdge = ne;               /* sentinel */
    out->face[nface].normal[0] = out->face[nface].normal[1] =
        out->face[nface].normal[2] = (MeReal)0;

    /* Edges. leftFace is the face whose ring the edge belongs to — measured, see
     * §8a — and rightFace is the face across it, found via the reverse edge. */
    for (i = 0; i < ne; i++) {
        V3 a, c;
        double len;
        out->edge[i].fromVert  = ed[i].from;
        out->edge[i].toVert    = ed[i].to;
        out->edge[i].leftFace  = ed[i].face;
        out->edge[i].rightFace = -1;
        for (j = 0; j < ne; j++)
            if (ed[j].from == ed[i].to && ed[j].to == ed[i].from) {
                out->edge[i].rightFace = ed[j].face; break;
            }
        if (out->edge[i].rightFace < 0) goto done;  /* not a closed surface */
        a = v3(out->vertex[ed[i].from].position[0],
               out->vertex[ed[i].from].position[1],
               out->vertex[ed[i].from].position[2]);
        c = v3(out->vertex[ed[i].to].position[0],
               out->vertex[ed[i].to].position[1],
               out->vertex[ed[i].to].position[2]);
        len = v3len(v3sub(c, a));
        out->edge[i].invLength = (MeReal)(len > 0.0 ? 1.0 / len : 0.0);
    }

    /* edgeIndex: each vertex's OUTGOING edges, grouped contiguously, with the
     * sentinel. McdCnvVertexGetNeighbor reads this and GJK hill-climbs it, so
     * what matters is that every outgoing edge appears exactly once under its
     * own vertex. */
    {
        int *cnt = (int *)calloc((size_t)nv + 1, sizeof(int));
        int *fill;
        if (!cnt) goto done;
        for (i = 0; i < ne; i++) cnt[out->edge[i].fromVert]++;
        out->vertex[0].firstEdgeIndex = 0;
        for (i = 1; i <= nv; i++)
            out->vertex[i].firstEdgeIndex = out->vertex[i - 1].firstEdgeIndex + cnt[i - 1];
        fill = (int *)calloc((size_t)nv, sizeof(int));
        if (!fill) { free(cnt); goto done; }
        for (i = 0; i < ne; i++) {
            int v = out->edge[i].fromVert;
            out->edgeIndex[out->vertex[v].firstEdgeIndex + fill[v]] = i;
            fill[v]++;
        }
        free(cnt); free(fill);
        out->vertex[nv].firstEdgeIndex = ne;       /* sentinel */
    }
    rc = 1;

done:
    free(ed); free(loop); free(vremap);
    if (!rc) {
        if (out->vertex)    MeMemoryAPI.destroy(out->vertex);
        if (out->face)      MeMemoryAPI.destroy(out->face);
        if (out->edge)      MeMemoryAPI.destroy(out->edge);
        if (out->edgeIndex) MeMemoryAPI.destroy(out->edgeIndex);
        memset(out, 0, sizeof *out);
    }
    return rc;
}

/* --------------------------------------------------------------- public API */

/* Relative to the bounding box, so the answer does not depend on world scale.
 * The shipped cutoff sits between 1e-6 and 1e-5 of the extent (§8a); 2e-6 puts
 * this on the same side of both measured cases. */
static const double hull_thin_reject = 2e-6;

int McdComputeHull(McdConvexHull *cnv, int numpoints, const MeVector3 *points)
{
    Build b;
    int nface, rc = 0;

    if (!cnv || !points || numpoints < 4) {
        if (cnv) memset(cnv, 0, sizeof *cnv);
        return 0;
    }
    memset(cnv, 0, sizeof *cnv);
    memset(&b, 0, sizeof b);

    if (dedup(&b, points, numpoints)) {
        double eps = b.scale * hull_thin_reject;
        if (initial_tetra(&b, eps) && add_points(&b, eps)) {
            nface = merge_faces(&b, eps);
            if (nface >= 4) rc = build_hull(&b, nface, cnv);
        }
    }
    free(b.pt); free(b.tri);
    return rc;
}

void McdDeallocateHull(McdConvexHull *cnv)
{
    if (!cnv) return;
    if (cnv->vertex)    MeMemoryAPI.destroy(cnv->vertex);
    if (cnv->face)      MeMemoryAPI.destroy(cnv->face);
    if (cnv->edge)      MeMemoryAPI.destroy(cnv->edge);
    if (cnv->edgeIndex) MeMemoryAPI.destroy(cnv->edgeIndex);
    memset(cnv, 0, sizeof *cnv);
}

/* The shipped library splits construction into measure / allocate / populate so
 * a caller can supply its own storage. Nothing in metoolkit uses the split —
 * McdConvexMesh.o calls McdComputeHull and McdDeallocateHull and nothing else —
 * so these are kept API-compatible by doing the whole job in the sizing step and
 * handing the result over, rather than by reproducing qhull's global state. */
static McdConvexHull g_pending;
static int           g_pending_valid;

int McdComputeHullSizes(McdConvexHull *cnv, int numpoints, const MeVector3 *points)
{
    if (g_pending_valid) { McdDeallocateHull(&g_pending); g_pending_valid = 0; }
    if (!McdComputeHull(&g_pending, numpoints, points)) return 0;
    g_pending_valid = 1;
    cnv->numVertex = g_pending.numVertex;
    cnv->numFace   = g_pending.numFace;
    cnv->numEdge   = g_pending.numEdge;
    return 1;
}

void McdAllocateHull(McdConvexHull *cnv)
{
    /* Storage already belongs to the pending hull; nothing to do. */
    (void)cnv;
}

int McdGetHullData(McdConvexHull *cnv)
{
    if (!g_pending_valid) return 0;
    *cnv = g_pending;
    g_pending_valid = 0;                 /* ownership transfers to the caller */
    memset(&g_pending, 0, sizeof g_pending);
    return 1;
}

void McdGetTriangleHull(McdConvexHull *cnv, const MeVector3 p1,
                        const MeVector3 p2, const MeVector3 p3)
{
    /* A triangle is not a solid, so it has no hull in this structure's sense —
     * McdComputeHull rejects three coplanar points outright (measured). Nothing
     * in metoolkit calls this; zeroing is the honest answer, and it is the one
     * a caller checking numFace will handle. */
    (void)p1; (void)p2; (void)p3;
    if (cnv) memset(cnv, 0, sizeof *cnv);
}

/* ----------------------------------------------------------- the accessors */

int McdCnvFaceGetCount(const McdConvexHull *cnv, int face)
{
    return cnv->face[face + 1].firstEdge - cnv->face[face].firstEdge;
}

const McdCnvEdge *McdCnvFaceGetEdge(const McdConvexHull *cnv, int face, int i)
{
    return &cnv->edge[cnv->face[face].firstEdge + i];
}

int McdCnvFaceGetVertexId(const McdConvexHull *cnv, int face, int i)
{
    return cnv->edge[cnv->face[face].firstEdge + i].fromVert;
}

const McdCnvVertex *McdCnvFaceGetVertex(const McdConvexHull *cnv, int face, int i)
{
    return &cnv->vertex[McdCnvFaceGetVertexId(cnv, face, i)];
}

const MeReal *McdCnvFaceGetVertexPosition(const McdConvexHull *cnv, int face, int i)
{
    return cnv->vertex[McdCnvFaceGetVertexId(cnv, face, i)].position;
}

int McdCnvVertexGetCount(const McdConvexHull *cnv, int vertex)
{
    return cnv->vertex[vertex + 1].firstEdgeIndex - cnv->vertex[vertex].firstEdgeIndex;
}

int McdCnvVertexGetEdgeId(const McdConvexHull *cnv, int vertex, int i)
{
    return cnv->edgeIndex[cnv->vertex[vertex].firstEdgeIndex + i];
}

const McdCnvEdge *McdCnvVertexGetEdge(const McdConvexHull *cnv, int vertex, int i)
{
    return &cnv->edge[McdCnvVertexGetEdgeId(cnv, vertex, i)];
}

int McdCnvVertexGetNeighbor(const McdConvexHull *cnv, int vertex, int i)
{
    return cnv->edge[McdCnvVertexGetEdgeId(cnv, vertex, i)].toVert;
}
