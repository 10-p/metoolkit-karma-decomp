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

/* Degenerate inputs. These are NOT exotic for UT2004 — its collision volumes are
 * boxes and low-poly prisms, so duplicate and coplanar vertices are the normal
 * case. What McdComputeHull DOES here is a contract detail a replacement has to
 * match, and it is not written down anywhere: a hull library that aborts, or
 * that returns a degenerate structure the caller then walks, is the difference
 * between a working map and a crash at load. */
static Shape make_literal(const char *name, int n, const MeReal (*v)[3])
{
    Shape s; s.name = name; s.n = n; s.p = alloc_pts(n);
    memcpy(s.p, v, n * sizeof(MeVector3));
    return s;
}

static const MeReal PTS_TRI[3][3]      = {{0,0,0},{1,0,0},{0,1,0}};
static const MeReal PTS_SQUARE[4][3]   = {{0,0,0},{1,0,0},{1,1,0},{0,1,0}};
static const MeReal PTS_COLLINEAR[4][3]= {{0,0,0},{1,0,0},{2,0,0},{3,0,0}};
static const MeReal PTS_DUP[8][3]      = {{0,0,0},{0,0,0},{1,0,0},{1,0,0},
                                          {0,1,0},{0,1,0},{0,0,1},{0,0,1}};
static const MeReal PTS_SLIVER[8][3]   = {{0,0,0},{1,0,0},{1,1,0},{0,1,0},
                                          {0,0,1e-6f},{1,0,1e-6f},
                                          {1,1,1e-6f},{0,1,1e-6f}};
static const MeReal PTS_ONE[1][3]      = {{0,0,0}};

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

    /* 11. WHICH SIDE IS THE FACE ON? The header says the edge array is "sorted by
     *     leftFace and ACW", and separately that an edge records "the face on its
     *     right side". Those two sentences disagree about which field groups the
     *     array, and getting it backwards mirrors every adjacency a replacement
     *     builds. Settle it by measurement rather than by reading. */
    {
        int by_left = 0, by_right = 0;
        for (f = 0; f < h->numFace; f++) {
            int b = h->face[f].firstEdge, n = h->face[f + 1].firstEdge - b;
            for (i = 0; i < n; i++) {
                if (h->edge[b + i].leftFace  == f) by_left++;
                if (h->edge[b + i].rightFace == f) by_right++;
            }
        }
        CHECK(by_left == h->numEdge || by_right == h->numEdge,
              "face ranges group by neither leftFace (%d/%d) nor rightFace (%d/%d)",
              by_left, h->numEdge, by_right, h->numEdge);
        printf("      [face ranges group by %s]\n",
               by_left == h->numEdge ? "leftFace"
               : by_right == h->numEdge ? "rightFace" : "NEITHER");
    }

    /* 12. An edge and its reverse must name the same two faces, swapped. */
    for (e = 0; e < h->numEdge; e++) {
        for (i = 0; i < h->numEdge; i++) {
            if (h->edge[i].fromVert == h->edge[e].toVert &&
                h->edge[i].toVert == h->edge[e].fromVert) {
                CHECK(h->edge[i].leftFace == h->edge[e].rightFace &&
                      h->edge[i].rightFace == h->edge[e].leftFace,
                      "edge %d (L=%d R=%d) vs reverse %d (L=%d R=%d): not swapped",
                      e, h->edge[e].leftFace, h->edge[e].rightFace,
                      i, h->edge[i].leftFace, h->edge[i].rightFace);
                break;
            }
        }
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

/* Canonical geometric dump, for the tier-2 A/B in HANDOVER.md §8a.
 *
 * The hull REINDEXES, so two correct implementations can disagree about every
 * index and still describe the same solid. Comparing them therefore means
 * comparing GEOMETRY as unordered sets: the vertex positions, and the face
 * planes (unit normal + offset). Both are sorted and printed at fixed precision
 * so `diff` does the work.
 *
 * Volume is printed too, as a single scalar that no amount of reordering can
 * disguise — if the vertex and plane sets both match and the volume matches,
 * the two hulls are the same solid. */
static int cmp_line(const void *a, const void *b)
{
    return strcmp(*(const char *const *)a, *(const char *const *)b);
}

static void dump_canonical(const McdConvexHull *h, const char *name)
{
    char **lines;
    int i, f, n = 0;
    double vol = 0.0;

    printf("### %s\n", name);
    lines = (char **)malloc((size_t)(h->numVertex + h->numFace) * sizeof(char *));
    if (!lines) return;

    /* Adding 0.0 turns -0.0 into +0.0 under round-to-nearest. Without it the two
     * implementations print the same plane as "-0.0000" and "+0.0000", which
     * also reorders the sort and makes a signed zero look like a real
     * disagreement. */
    #define Z(x) ((double)(x) + 0.0)
    for (i = 0; i < h->numVertex; i++) {
        lines[n] = (char *)malloc(64);
        sprintf(lines[n], "V %+.4f %+.4f %+.4f", Z(h->vertex[i].position[0]),
                Z(h->vertex[i].position[1]), Z(h->vertex[i].position[2]));
        n++;
    }
    for (f = 0; f < h->numFace; f++) {
        const MeReal *nr = h->face[f].normal;
        const MeReal *p = h->vertex[h->edge[h->face[f].firstEdge].fromVert].position;
        double d = (double)nr[0]*p[0] + (double)nr[1]*p[1] + (double)nr[2]*p[2];
        lines[n] = (char *)malloc(64);
        sprintf(lines[n], "F %+.4f %+.4f %+.4f %+.4f", Z(nr[0]), Z(nr[1]), Z(nr[2]), Z(d));
        n++;
    }
    #undef Z
    qsort(lines, (size_t)n, sizeof(char *), cmp_line);
    for (i = 0; i < n; i++) { printf("%s\n", lines[i]); free(lines[i]); }
    free(lines);

    /* Signed volume by the divergence theorem, fanning each face. */
    for (f = 0; f < h->numFace; f++) {
        int b = h->face[f].firstEdge, cnt = h->face[f + 1].firstEdge - b;
        const MeReal *v0 = h->vertex[h->edge[b].fromVert].position;
        for (i = 1; i + 1 < cnt; i++) {
            const MeReal *v1 = h->vertex[h->edge[b + i].fromVert].position;
            const MeReal *v2 = h->vertex[h->edge[b + i + 1].fromVert].position;
            double ax=v1[0]-v0[0], ay=v1[1]-v0[1], az=v1[2]-v0[2];
            double bx=v2[0]-v0[0], by=v2[1]-v0[1], bz=v2[2]-v0[2];
            vol += ((double)v0[0]*(ay*bz-az*by) + (double)v0[1]*(az*bx-ax*bz)
                  + (double)v0[2]*(ax*by-ay*bx)) / 6.0;
        }
    }
    printf("VOL %.5f\n", fabs(vol));
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
    if (getenv("KD_HULL_DUMP")) { dump_canonical(&h, s.name); McdDeallocateHull(&h); free(s.p); return; }
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

/* Degenerate inputs are reported, not asserted on: the point is to LEARN what
 * the shipped code does so a replacement can match it. Returning 0 is a
 * perfectly good answer; silently producing a structure the caller then walks
 * is a different one, and the caller (McdConvexMeshCreateHull) only checks the
 * return value. */
static void run_degenerate(Shape s)
{
    McdConvexHull h;
    int ok;
    printf("  %-28s (%2d pts) -> ", s.name, s.n);
    fflush(stdout);
    memset(&h, 0, sizeof h);
    ok = McdComputeHull(&h, s.n, s.p);
    if (!ok) {
        printf("returns 0 (no hull)\n");
    } else {
        printf("returns 1: V=%d F=%d E=%d", h.numVertex, h.numFace, h.numEdge);
        if (h.numVertex - h.numEdge / 2 + h.numFace == 2) printf("  [Euler ok]");
        else printf("  [Euler = %d, NOT a closed polyhedron]",
                    h.numVertex - h.numEdge / 2 + h.numFace);
        printf("\n");
        McdDeallocateHull(&h);
    }
    free(s.p);
}

/* Where exactly is the "too thin to be a solid" cutoff? It matters in both
 * directions: a replacement that ACCEPTS a box the shipped code rejects creates
 * collision geometry the game never had, and one that rejects more removes
 * geometry it did have. Neither shows up as a crash — the caller just gets NULL
 * and silently makes no primitive. */
static void run_threshold_sweep(void)
{
    double t;
    printf("\nHow thin is too thin? (unit square extruded by t)\n\n");
    for (t = 1e-8; t < 2e-1; t *= 10.0) {
        McdConvexHull h;
        MeVector3 p[8];
        int i, ok;
        const MeReal base[4][2] = {{0,0},{1,0},{1,1},{0,1}};
        for (i = 0; i < 4; i++) {
            p[i][0] = base[i][0];   p[i][1] = base[i][1];   p[i][2] = 0;
            p[i+4][0] = base[i][0]; p[i+4][1] = base[i][1]; p[i+4][2] = (MeReal)t;
        }
        memset(&h, 0, sizeof h);
        ok = McdComputeHull(&h, 8, p);
        printf("    t = %-9.0e -> %s\n", t, ok ? "hull built" : "returns 0");
        if (ok) McdDeallocateHull(&h);
    }
}

int main(void)
{
    if (getenv("KD_HULL_DUMP")) {
        /* Tier 2: geometry only, canonical, for diffing one implementation
         * against another. No banner — the whole output is the comparand. */
        run(make_tetra(), 0);
        run(make_cube(), 0);
        run(make_cube_redundant(), 0);
        run(make_cloud("sphere cloud, 24 pts", 24), 0);
        run(make_cloud("sphere cloud, 60 pts", 60), 0);
        run(make_cloud("sphere cloud, 200 pts", 200), 0);
        return 0;
    }
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

    printf("\nDegenerate input — what does the shipped code DO?\n"
           "(UT2004 collision volumes are boxes and prisms, so duplicate and\n"
           " coplanar vertices are ordinary, not exotic.)\n\n");
    run_degenerate(make_literal("single point",        1, PTS_ONE));
    run_degenerate(make_literal("triangle (coplanar)", 3, PTS_TRI));
    run_degenerate(make_literal("square (coplanar)",   4, PTS_SQUARE));
    run_degenerate(make_literal("collinear",           4, PTS_COLLINEAR));
    run_degenerate(make_literal("tetra w/ duplicates", 8, PTS_DUP));
    run_degenerate(make_literal("1e-6 sliver box",     8, PTS_SLIVER));
    run_threshold_sweep();
    return g_fail ? 1 : 0;
}
