/* ab_matrix.c — a direct A/B for MeMath's pure matrix functions.
 *
 * HANDOVER.md §5c refuses MeMath, and until now the recorded reason was that
 * "no gate here could verify a reconstruction". That was wrong in the same way
 * several other blockers this session were wrong: the gate did not exist, which
 * is not the same as being impossible. `difftest_pair.sh` and `ab_integrate.sh`
 * are both exactly this instrument for other objects, and these functions are
 * PURE — inputs in, a transform out — so they are easier to A/B than either.
 *
 * The recovered object is staged with its symbols prefixed `rec_`, the same
 * trick difftest_pair.sh uses, so both versions are callable from one binary.
 *
 * KD_SELFTEST=1 calls the SHIPPED function on both sides. It must read zero
 * differences; a harness that cannot tell identical from identical is measuring
 * itself, and every other number here would be meaningless.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef float MeReal;
typedef MeReal MeVector4[4];

void MeMatrix4TMUpdateFromVelocitiesAndAcceler(
    MeVector4 *r, MeReal eps, MeReal dt, MeReal *v, MeReal *a,
    MeReal *w, MeReal *alpha, MeReal (*t)[4]);
void rec_MeMatrix4TMUpdateFromVelocitiesAndAcceler(
    MeVector4 *r, MeReal eps, MeReal dt, MeReal *v, MeReal *a,
    MeReal *w, MeReal *alpha, MeReal (*t)[4]);

static unsigned s_rand = 12345;
static float rnd(float lo, float hi)
{
    s_rand = s_rand * 1103515245u + 12345u;
    return lo + (hi - lo) * ((s_rand >> 8) & 0xffffff) / (float)0xffffff;
}

int main(int argc, char **argv)
{
    long n = argc > 1 ? atol(argv[1]) : 200000;
    int selftest = getenv("KD_SELFTEST") && atoi(getenv("KD_SELFTEST"));
    long diff = 0, nonfinite = 0;
    float worst = 0.0f;
    long first = -1;

    for (long i = 0; i < n; i++) {
        MeReal v[3], a[3], w[3], alpha[3], t[4][4];
        MeVector4 r1[4], r2[4];
        MeReal dt = rnd(1e-4f, 1e-1f), eps = 1e-8f;
        for (int k = 0; k < 3; k++) {
            v[k] = rnd(-50.f, 50.f);
            a[k] = rnd(-50.f, 50.f);
            /* the regime that matters: a spin big enough that the rotation
             * branch is taken, not the |w| < eps shortcut */
            w[k] = rnd(-8.f, 8.f);
            alpha[k] = rnd(-8.f, 8.f);
        }
        for (int p = 0; p < 4; p++)
            for (int q = 0; q < 4; q++)
                t[p][q] = (p == q) ? 1.0f : rnd(-0.3f, 0.3f);
        memset(r1, 0, sizeof r1);
        memset(r2, 0, sizeof r2);

        MeMatrix4TMUpdateFromVelocitiesAndAcceler(r1, eps, dt, v, a, w, alpha, t);
        if (selftest)
            MeMatrix4TMUpdateFromVelocitiesAndAcceler(r2, eps, dt, v, a, w, alpha, t);
        else
            rec_MeMatrix4TMUpdateFromVelocitiesAndAcceler(r2, eps, dt, v, a, w,
                                                          alpha, t);

        int bad = 0;
        for (int p = 0; p < 4; p++) {
            for (int q = 0; q < 4; q++) {
                float x = r1[p][q], y = r2[p][q];
                if (!isfinite(y)) nonfinite++;
                if (memcmp(&x, &y, sizeof x) != 0) {
                    bad = 1;
                    float d = fabsf(x - y);
                    if (isfinite(d) && d > worst) worst = d;
                }
            }
        }
        if (bad) {
            if (first < 0) first = i;
            diff++;
        }
    }
    printf("%s: %ld cases, %ld differ (%.3f%%), worst |delta| %.6g, "
           "non-finite %ld, first@%ld\n",
           selftest ? "SELFTEST (shipped vs shipped)" : "recovered vs shipped",
           n, diff, 100.0 * diff / n, worst, nonfinite, first);
    return diff ? 1 : 0;
}
