/* ab_contact.c — per-function A/B for McdContactSimplify.
 *
 * The MeMath pattern (test/ab_matrix.sh) applied to the one symbol the engine
 * imports from `McdContact`. The function is PURE in the sense that matters: it
 * reads a contact array and writes another, so both implementations can be
 * driven with identical inputs in one process and compared BITWISE.
 *
 * WHY THIS AND NOT A SCENE. McdContactSimplify's frame is an alloca sized
 * `(count+1)*16` whose block Ghidra anchors 0x10 above its own base — the
 * defect recorded in proven.txt is that a repair COMPILED, passed all nine
 * gates, and indexed an eight-byte scratch slot as (count+1) sixteen-byte
 * structs. `check_frame_bounds` cannot see it because the index is a variable.
 * Nothing offline could falsify that repair. This can: the block is indexed
 * `inMaxContactPointCount` times per call, so a wrong base is a wrong answer.
 *
 * THE CONTROL IS THE POINT. KD_SELFTEST=1 drives the SHIPPED function on both
 * sides. It must read 0 differences; if it does not, the harness is measuring
 * itself and every other number here is noise.
 *
 * The inputs are shaped like a real contact set, not uniform noise: contacts
 * from one collision lie near a common plane with a shared normal, and the
 * function's whole job is to reduce that set to a hull. A fraction are left
 * fully random anyway, because the degenerate cases are where a hull algorithm
 * decides things.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <MePrecision.h>
#include <McdCTypes.h>

extern int MEAPI ab_orig_Simplify(const MeReal *, McdContact *, int,
                                  McdContact *, int, int, MeReal)
    __asm__("McdContactSimplify");
extern int MEAPI ab_rec_Simplify(const MeReal *, McdContact *, int,
                                 McdContact *, int, int, MeReal)
    __asm__("rec_McdContactSimplify");

static unsigned rs = 0xC0FFEEu;
static unsigned nextr(void) { rs = rs * 1664525u + 1013904223u; return rs; }
static float frand(void) { return (float)((double)(nextr() >> 8) / 16777216.0); }
static float srand1(void) { return 2.0f * frand() - 1.0f; }

#define MAXSRC 48
#define MAXDST 24

int main(int argc, char **argv)
{
    int N = argc > 1 ? atoi(argv[1]) : 200000;
    const char *e = getenv("KD_SELFTEST");
    int selftest = (e && *e == '1');
    long diffs = 0, retdiff = 0, bodydiff = 0, ran = 0;
    int shown = 0;

    for (int it = 0; it < N; it++) {
        McdContact src[MAXSRC], dstA[MAXDST + 1], dstB[MAXDST + 1];
        MeReal normal[3];
        int nsrc = 1 + (int)(nextr() % MAXSRC);
        int ndst = 1 + (int)(nextr() % MAXDST);
        int fnf  = (int)(nextr() & 1);
        MeReal scale = 0.25f + 4.0f * frand();

        /* a shared normal, and a basis for the plane it defines */
        MeReal n[3] = { srand1(), srand1(), srand1() };
        MeReal len = (MeReal)sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
        if (len < 1e-6f) { n[0] = 0; n[1] = 1; n[2] = 0; len = 1; }
        n[0] /= len; n[1] /= len; n[2] /= len;
        MeReal u[3] = { -n[1], n[0], 0 };
        MeReal ul = (MeReal)sqrt(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]);
        if (ul < 1e-6f) { u[0] = 1; u[1] = 0; u[2] = 0; ul = 1; }
        u[0] /= ul; u[1] /= ul; u[2] /= ul;
        MeReal w[3] = { n[1]*u[2] - n[2]*u[1],
                        n[2]*u[0] - n[0]*u[2],
                        n[0]*u[1] - n[1]*u[0] };
        normal[0] = n[0]; normal[1] = n[1]; normal[2] = n[2];

        int wild = (nextr() % 5) == 0;          /* one set in five is noise */
        for (int i = 0; i < nsrc; i++) {
            MeReal a = 2.0f * srand1(), b = 2.0f * srand1(), h = 0.02f * srand1();
            if (wild) {
                src[i].position[0] = 2.0f * srand1();
                src[i].position[1] = 2.0f * srand1();
                src[i].position[2] = 2.0f * srand1();
            } else {
                for (int k = 0; k < 3; k++)
                    src[i].position[k] = a * u[k] + b * w[k] + h * n[k];
            }
            /* normals near the shared one, occasionally not */
            for (int k = 0; k < 3; k++)
                src[i].normal[k] = n[k] + 0.05f * srand1();
            if ((nextr() % 8) == 0)
                for (int k = 0; k < 3; k++) src[i].normal[k] = srand1();
            src[i].separation = -0.2f * frand();
            /* the low byte and the high byte are both read (`dims >> 8`), and
               the two bytes at offset 0x1e are copied wholesale — so fill the
               whole struct, padding included, or the comparison is vacuous. */
            src[i].dims = (short)(nextr() & 0xffff);
            src[i].element1.tag = (int)nextr();
            src[i].element2.tag = (int)nextr();
        }

        /* Poison both destinations identically. Anything neither writes then
           compares equal, and anything one writes and the other does not is a
           difference rather than a coin toss. */
        memset(dstA, 0x5a, sizeof dstA);
        memset(dstB, 0x5a, sizeof dstB);

        McdContact srcA[MAXSRC], srcB[MAXSRC];
        memcpy(srcA, src, sizeof src);
        memcpy(srcB, src, sizeof src);

        int ra = ab_orig_Simplify(normal, srcA, nsrc, dstA, ndst, fnf, scale);
        int rb = selftest
               ? ab_orig_Simplify(normal, srcB, nsrc, dstB, ndst, fnf, scale)
               : ab_rec_Simplify(normal, srcB, nsrc, dstB, ndst, fnf, scale);
        ran++;

        int dr = (ra != rb);
        /* Compare the WHOLE destination buffer, not `ra` entries: a write past
           the count is exactly the smash this harness exists to catch. */
        int db = (memcmp(dstA, dstB, sizeof dstA) != 0)
              || (memcmp(srcA, srcB, sizeof srcA) != 0);
        retdiff += dr; bodydiff += db;
        if (dr || db) {
            diffs++;
            if (shown < 4) {
                shown++;
                printf("  !! it %d: nsrc %d ndst %d fnf %d scale %.9g  ret %d/%d%s\n",
                       it, nsrc, ndst, fnf, (double)scale, ra, rb,
                       db ? "  BODY DIFFERS" : "");
                for (int i = 0; i < MAXDST && i < (ra > rb ? ra : rb) + 1; i++)
                    if (memcmp(&dstA[i], &dstB[i], sizeof dstA[i]))
                        printf("     [%d] pos %g %g %g / %g %g %g  sep %g/%g  dims %d/%d\n",
                               i, (double)dstA[i].position[0], (double)dstA[i].position[1],
                               (double)dstA[i].position[2], (double)dstB[i].position[0],
                               (double)dstB[i].position[1], (double)dstB[i].position[2],
                               (double)dstA[i].separation, (double)dstB[i].separation,
                               dstA[i].dims, dstB[i].dims);
                if (memcmp(srcA, srcB, sizeof srcA))
                    printf("     THE INPUT ARRAY WAS MODIFIED DIFFERENTLY\n");
            }
        }
    }
    printf("McdContactSimplify%s: %ld calls\n",
           selftest ? " [KD_SELFTEST — shipped vs shipped]" : "", ran);
    printf("  bit-identical      : %ld (%.3f%%)\n", ran - diffs,
           100.0 * (double)(ran - diffs) / (double)ran);
    printf("  DIFFERENT return   : %ld\n", retdiff);
    printf("  DIFFERENT contacts : %ld\n", bodydiff);
    printf("  -> %s\n", diffs ? "FAIL -- divergence" : "PASS -- bit-identical");
    return diffs ? 1 : 0;
}
