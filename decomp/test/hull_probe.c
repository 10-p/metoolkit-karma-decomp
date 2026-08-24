/*
 * hull_probe.c — what does the SHIPPED McdComputeHull actually produce?
 *
 * Ground truth for HANDOVER.md §8a. Replacing libMcdConvexCreateHull means
 * reproducing the structure in McdQHullTypes.h exactly enough that GJK's support
 * function can hill-climb it, and that structure is documented in prose rather
 * than in code. This runs the shipped implementation on known point sets and
 * CHECKS every invariant the prose claims, so that:
 *
 *   - the contract is verified rather than assumed, before anything is written
 *     against it (two readings of that header were already wrong: the hull is
 *     not thrown away after construction, and it is not load-time only);
 *   - the same checker can be pointed at a replacement, where diffing against
 *     the shipped hull is NOT available — a differently indexed but
 *     geometrically identical hull is a legitimate answer, so the acceptance
 *     test has to be invariants plus geometry, not equality.
 *
 * Build and run with test/hull_probe.sh.
 *
 * Every check below is a claim McdQHullTypes.h makes in its header comment. A
 * failure here means either the header is wrong or this file misread it; both
 * are worth knowing before writing a replacement.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <MeMath.h>
#include <McdQHullTypes.h>

static int g_fail;
static int g_checks;

#define CHECK(cond, ...)                                                      \
    do {                                                                      \
        g_checks++;                                                           \
        if (!(cond)) {                                                        \
            g_fail++;                                                         \
            printf("    FAIL: ");                                             \
            printf(__VA_ARGS__);                                              \
            printf("\n");                                                     \
        }                                                                     \
    } while (0)

/* ---------------------------------------------------------------- point sets */

typedef struct {
    const char *name;
    int n;
    MeVector3 *p;
} Shape;

static MeVector3 *alloc_pts(int n) { return (MeVector3 *)malloc(n * sizeof(MeVector3)); }

static unsigned rng_state = 12345u;
static double rnd(void)          /* deterministic; no time() anywhere */
{
    rng_state = rng_state * 1103515245u + 12345u;
    return (double)((rng_state >> 16) & 0x7fff) / 32767.0;
}

static Shape make_tetra(void)
{
    Shape s; s.name = "tetrahedron"; s.n = 4; s.p = alloc_pts(4);
    MeReal v[4][3] = {{0,0,0},{1,0,0},{0,1,0},{0,0,1}};
    memcpy(s.p, v, sizeof v);
    return s;
}

static Shape make_cube(void)
{
    Shape s; s.name = "cube"; s.n = 8; s.p = alloc_pts(8);
    int i = 0, x, y, z;
    for (x = 0; x < 2; x++) for (y = 0; y < 2; y++) for (z = 0; z < 2; z++) {
        s.p[i][0] = (MeReal)x; s.p[i][1] = (MeReal)y; s.p[i][2] = (MeReal)z; i++;
    }
    return s;
}

/* A cube with extra points on the faces and inside. The interior points must be
 * discarded and the coplanar ones must MERGE, which is what makes Karma's faces
 * polygons rather than triangles — the property a triangle-only hull library
 * would silently get wrong. */
static Shape make_cube_redundant(void)
{
    Shape s; s.name = "cube + face/interior points"; s.n = 8 + 6 + 5;
    s.p = alloc_pts(s.n);
    int i = 0, x, y, z;
    for (x = 0; x < 2; x++) for (y = 0; y < 2; y++) for (z = 0; z < 2; z++) {
        s.p[i][0] = (MeReal)x; s.p[i][1] = (MeReal)y; s.p[i][2] = (MeReal)z; i++;
    }
    MeReal face[6][3] = {{0.5f,0.5f,0},{0.5f,0.5f,1},{0.5f,0,0.5f},
                         {0.5f,1,0.5f},{0,0.5f,0.5f},{1,0.5f,0.5f}};
    memcpy(s.p + i, face, sizeof face); i += 6;
    for (; i < s.n; i++) {
        s.p[i][0] = (MeReal)(0.3 + 0.4 * rnd());
        s.p[i][1] = (MeReal)(0.3 + 0.4 * rnd());
        s.p[i][2] = (MeReal)(0.3 + 0.4 * rnd());
    }
    return s;
}

static Shape make_cloud(const char *name, int n)
{
    Shape s; s.name = name; s.n = n; s.p = alloc_pts(n);
    int i;
    for (i = 0; i < n; i++) {          /* on a sphere: no interior points, all extreme */
        double u = rnd() * 2.0 - 1.0, t = rnd() * 6.283185307;
        double r = sqrt(1.0 - u * u);
        s.p[i][0] = (MeReal)(r * cos(t));
        s.p[i][1] = (MeReal)(r * sin(t));
        s.p[i][2] = (MeReal)u;
    }
    return s;
}

/* ------------------------------------------------------------- the invariants */

static void check_hull(const McdConvexHull *h, const Shape *s)
{
    int f, e, v, i;

    printf("    V=%d  F=%d  E=%d (each edge twice -> %d undirected)\n",
           h->numVertex, h->numFace, h->numEdge, h->numEdge / 2);

    /* 1. Sentinels. The header says the last face/vertex in each list is a
     *    sentinel with firstEdge == numEdges. Everything that iterates a face or
     *    a vertex reads element [a+1], so without this every such loop runs off
     *    the end — it is the single most load-bearing claim in the header. */
    CHECK(h->face[h->numFace].firstEdge == h->numEdge,
          "face sentinel: face[%d].firstEdge=%d, want %d",
          h->numFace, h->face[h->numFace].firstEdge, h->numEdge);
    CHECK(h->vertex[h->numVertex].firstEdgeIndex == h->numEdge,
          "vertex sentinel: vertex[%d].firstEdgeIndex=%d, want %d",
          h->numVertex, h->vertex[h->numVertex].firstEdgeIndex, h->numEdge);

    /* 2. Euler's formula for a closed convex polyhedron. numEdge counts each
     *    edge twice, hence /2. This catches a hull that is topologically open. */
    CHECK(h->numVertex - h->numEdge / 2 + h->numFace == 2,
          "Euler: V - E/2 + F = %d, want 2",
          h->numVertex - h->numEdge / 2 + h->numFace);

    /* 3. Face edge ranges partition the edge array, in order. */
    CHECK(h->face[0].firstEdge == 0, "face[0].firstEdge=%d, want 0", h->face[0].firstEdge);
    for (f = 0; f < h->numFace; f++)
        CHECK(h->face[f].firstEdge <= h->face[f + 1].firstEdge,
              "face %d range not ascending", f);

    /* 4. Each face's edges form a CLOSED RING: consecutive edges share a vertex,
     *    and the last closes back to the first. This is what "sorted ACW around
     *    the face" means operationally, and it is what McdCnvFaceGetVertexPosition
     *    walks. */
    for (f = 0; f < h->numFace; f++) {
        int b = h->face[f].firstEdge, n = h->face[f + 1].firstEdge - b;
        CHECK(n >= 3, "face %d has %d edges, want >= 3", f, n);
        if (n < 3) continue;
        for (i = 0; i < n; i++) {
            const McdCnvEdge *a = &h->edge[b + i];
            const McdCnvEdge *c = &h->edge[b + (i + 1) % n];
            CHECK(a->toVert == c->fromVert,
                  "face %d edge %d: toVert=%d but next fromVert=%d",
                  f, i, a->toVert, c->fromVert);
        }
    }

    /* 5. Every directed edge has its reverse present, and the two agree about
     *    which face is on which side. */
    for (e = 0; e < h->numEdge; e++) {
        int found = 0;
        for (i = 0; i < h->numEdge; i++)
            if (h->edge[i].fromVert == h->edge[e].toVert &&
                h->edge[i].toVert == h->edge[e].fromVert) { found = 1; break; }
        CHECK(found, "edge %d (%d->%d) has no reverse",
              e, h->edge[e].fromVert, h->edge[e].toVert);
    }

    /* 6. invLength really is 1/|edge|. */
    for (e = 0; e < h->numEdge; e++) {
        const MeReal *a = h->vertex[h->edge[e].fromVert].position;
        const MeReal *b = h->vertex[h->edge[e].toVert].position;
        double dx = b[0]-a[0], dy = b[1]-a[1], dz = b[2]-a[2];
        double len = sqrt(dx*dx + dy*dy + dz*dz);
        if (len > 1e-9) {
            double want = 1.0 / len;
            CHECK(fabs(h->edge[e].invLength - want) <= 1e-3 * want,
                  "edge %d invLength=%g, want %g", e, (double)h->edge[e].invLength, want);
        }
    }

    /* 7. edgeIndex: every edge listed against a vertex must START at that
     *    vertex. This is exactly what McdCnvVertexGetNeighbor relies on, and it
     *    is the structure GJK hill-climbs. */
    for (v = 0; v < h->numVertex; v++) {
        int b = h->vertex[v].firstEdgeIndex, n = h->vertex[v + 1].firstEdgeIndex - b;
        CHECK(n >= 3, "vertex %d has %d incident edges, want >= 3", v, n);
        for (i = 0; i < n; i++) {
            int id = h->edgeIndex[b + i];
            CHECK(id >= 0 && id < h->numEdge, "vertex %d edgeIndex out of range", v);
            if (id >= 0 && id < h->numEdge)
                CHECK(h->edge[id].fromVert == v,
                      "vertex %d edgeIndex[%d] -> edge %d starts at %d",
                      v, i, id, h->edge[id].fromVert);
        }
    }

    /* 8. Face normals point OUTWARD: no vertex lies in front of any face. */
    for (f = 0; f < h->numFace; f++) {
        const MeReal *n = h->face[f].normal;
        const MeReal *p = h->vertex[h->edge[h->face[f].firstEdge].fromVert].position;
        double nl = sqrt((double)n[0]*n[0] + (double)n[1]*n[1] + (double)n[2]*n[2]);
        CHECK(fabs(nl - 1.0) < 1e-3, "face %d normal length %g", f, nl);
        for (v = 0; v < h->numVertex; v++) {
            const MeReal *q = h->vertex[v].position;
            double d = (q[0]-p[0])*n[0] + (q[1]-p[1])*n[1] + (q[2]-p[2])*n[2];
            CHECK(d < 1e-3, "face %d: vertex %d is %g in FRONT of it", f, v, d);
            if (d >= 1e-3) break;              /* one report per face is enough */
        }
    }

    /* 9. Every INPUT point is inside or on the hull. A hull that drops a real
     *    extreme point is the failure mode that matters for collision. */
    for (i = 0; i < s->n; i++) {
        int outside = 0;
        for (f = 0; f < h->numFace && !outside; f++) {
            const MeReal *n = h->face[f].normal;
            const MeReal *p = h->vertex[h->edge[h->face[f].firstEdge].fromVert].position;
            double d = ((double)s->p[i][0]-p[0])*n[0] + ((double)s->p[i][1]-p[1])*n[1]
                     + ((double)s->p[i][2]-p[2])*n[2];
            if (d > 1e-3) outside = 1;
        }
        CHECK(!outside, "input point %d is OUTSIDE the hull", i);
    }

    /* 10. WINDING. The header says the edges are anti-clockwise around the face;
     *     it does not say from which side, and a replacement that guessed the
     *     other handedness would still close every ring and still have outward
     *     normals — checks 4 and 8 cannot see it — while reversing every polygon
     *     the collision code reads. Newell's method gives the polygon's own
     *     normal from its vertex positions in order; it must agree in SIGN with
     *     the face normal, which pins the convention to ACW seen from outside. */
    for (f = 0; f < h->numFace; f++) {
        int b = h->face[f].firstEdge, n = h->face[f + 1].firstEdge - b;
        double nx = 0, ny = 0, nz = 0;
        if (n < 3) continue;
        for (i = 0; i < n; i++) {
            const MeReal *a = h->vertex[h->edge[b + i].fromVert].position;
            const MeReal *c = h->vertex[h->edge[b + (i + 1) % n].fromVert].position;
            nx += ((double)a[1] - c[1]) * ((double)a[2] + c[2]);
            ny += ((double)a[2] - c[2]) * ((double)a[0] + c[0]);
            nz += ((double)a[0] - c[0]) * ((double)a[1] + c[1]);
        }
        {
            const MeReal *fn = h->face[f].normal;
            double dot = nx * fn[0] + ny * fn[1] + nz * fn[2];
            double len = sqrt(nx*nx + ny*ny + nz*nz);
            CHECK(len > 1e-9 && dot > 0,
                  "face %d winding: Newell normal . face normal = %g (want > 0)", f, dot);
        }
    }
}

/* ------------------------------------------------------------------------ main */

static void run(Shape s, int dump)
{
    McdConvexHull h;
    int ok;

    printf("  %s (%d input points)\n", s.name, s.n);
    memset(&h, 0, sizeof h);
    ok = McdComputeHull(&h, s.n, s.p);
    if (!ok) {
        printf("    McdComputeHull returned 0 (no hull)\n");
        g_checks++; g_fail++;
        free(s.p);
        return;
    }
    check_hull(&h, &s);

    if (dump) {
        int f, i;
        for (f = 0; f < h.numFace; f++) {
            int b = h.face[f].firstEdge, n = h.face[f + 1].firstEdge - b;
            printf("      face %d  n=(%.2f %.2f %.2f)  verts:", f,
                   h.face[f].normal[0], h.face[f].normal[1], h.face[f].normal[2]);
            for (i = 0; i < n; i++) printf(" %d", h.edge[b + i].fromVert);
            printf("\n");
        }
    }
    McdDeallocateHull(&h);
    free(s.p);
}

int main(void)
{
    printf("Ground truth from the SHIPPED McdComputeHull (libMcdConvexCreateHull.a)\n\n");
    run(make_tetra(), 1);
    run(make_cube(), 1);
    run(make_cube_redundant(), 1);
    run(make_cloud("sphere cloud, 24 pts", 24), 0);
    run(make_cloud("sphere cloud, 60 pts", 60), 0);
    run(make_cloud("sphere cloud, 200 pts", 200), 0);

    printf("\n%d checks, %d failures\n", g_checks, g_fail);
    if (g_fail)
        printf("A failure here means the header's contract is not what this file "
               "assumed. Fix the assumption before writing a replacement.\n");
    return g_fail ? 1 : 0;
}
