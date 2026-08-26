/* ab_rodrigues.c — a candidate reconstruction of MeMath's eR, A/B'd bit-exact.
 *
 * MeMatrix4TMUpdateFromVelocities builds a rotation matrix `eR[3][3]` from an
 * angular velocity and a timestep and then multiplies the incoming transform by
 * it. Ghidra discards the x87 fcos/fsin results, so the whole construction of
 * eR is missing from the recovery (HANDOVER.md §5c, and proven.txt).
 *
 * The structure is readable from the disassembly:
 *     bd0: fsqrt                    |w|
 *     c80: fld1 ; c88: fdiv %st(3)  1/|w|      -> the unit axis
 *     lVar9 = dt * |w|              the angle
 *     cd8: fcos ; ce0: fsin
 * i.e. Rodrigues' formula. This tries the textbook form and asks the SHIPPED
 * function whether it is bit-exact. It is a hypothesis with an oracle, not a
 * repair: if it does not read 0 it does not go anywhere near the pipeline.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef float MeReal;
typedef MeReal MeVector4[4];

void MeMatrix4TMUpdateFromVelocities(MeVector4 *r, MeReal eps, MeReal dt,
                                     MeReal *v, MeReal *w, MeReal (*t)[4]);

static void candidate(MeVector4 *r, MeReal eps, MeReal dt,
                      MeReal *v, MeReal *w, MeReal (*t)[4])
{
    long double mag = sqrtl((long double)w[2] * w[2] + (long double)w[1] * w[1]
                            + (long double)w[0] * w[0]);
    if ((long double)dt < eps || mag < eps) {
        memcpy(r, t, 16 * sizeof(MeReal));
        r[3][0] = (MeReal)((long double)v[0] * dt + t[3][0]);
        r[3][1] = (MeReal)((long double)v[1] * dt + t[3][1]);
        r[3][2] = (MeReal)((long double)dt * v[2] + t[3][2]);
        return;
    }
    r[3][0] = (MeReal)((long double)v[0] * dt + t[3][0]);
    r[3][1] = (MeReal)((long double)v[1] * dt + t[3][1]);
    r[3][3] = 1.0f; r[0][3] = 0.0f; r[1][3] = 0.0f; r[2][3] = 0.0f;
    r[3][2] = (MeReal)((long double)v[2] * dt + t[3][2]);

    long double theta = (long double)dt * mag;
    long double inv = 1.0L / mag;
    long double n0 = w[0] * inv, n1 = w[1] * inv, n2 = w[2] * inv;
    long double c = cosl(theta), s = sinl(theta), C = 1.0L - c;
    if (getenv("KD_FLIP")) s = -s;      /* the transpose convention */

    MeReal eR[3][3];
    eR[0][0] = (MeReal)(c + n0 * n0 * C);
    eR[0][1] = (MeReal)(n0 * n1 * C + n2 * s);
    eR[0][2] = (MeReal)(n0 * n2 * C - n1 * s);
    eR[1][0] = (MeReal)(n0 * n1 * C - n2 * s);
    eR[1][1] = (MeReal)(c + n1 * n1 * C);
    eR[1][2] = (MeReal)(n1 * n2 * C + n0 * s);
    eR[2][0] = (MeReal)(n0 * n2 * C + n1 * s);
    eR[2][1] = (MeReal)(n1 * n2 * C - n0 * s);
    eR[2][2] = (MeReal)(c + n2 * n2 * C);

    for (int i = 0; i < 3; i++) {
        MeReal a = eR[i][0], b = eR[i][1], d = eR[i][2];
        r[i][0] = d * t[2][0] + b * t[1][0] + a * t[0][0];
        r[i][1] = d * t[2][1] + a * t[0][1] + b * t[1][1];
        r[i][2] = d * t[2][2] + a * t[0][2] + b * t[1][2];
    }
}

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
    long diff = 0; float worst = 0.0f; long first = -1;
    long rotdiff = 0;                    /* differences in the 3x3 part only */

    for (long i = 0; i < n; i++) {
        MeReal v[3], w[3], t[4][4];
        MeVector4 r1[4], r2[4];
        MeReal dt = rnd(1e-4f, 1e-1f), eps = 1e-8f;
        for (int k = 0; k < 3; k++) { v[k] = rnd(-50.f, 50.f); w[k] = rnd(-8.f, 8.f); }
        for (int p = 0; p < 4; p++)
            for (int q = 0; q < 4; q++)
                t[p][q] = (p == q) ? 1.0f : rnd(-0.3f, 0.3f);
        memset(r1, 0, sizeof r1); memset(r2, 0, sizeof r2);

        MeMatrix4TMUpdateFromVelocities(r1, eps, dt, v, w, t);
        if (selftest) MeMatrix4TMUpdateFromVelocities(r2, eps, dt, v, w, t);
        else          candidate(r2, eps, dt, v, w, t);

        int bad = 0;
        for (int p = 0; p < 4; p++)
            for (int q = 0; q < 4; q++) {
                float x = r1[p][q], y = r2[p][q];
                if (memcmp(&x, &y, sizeof x)) {
                    bad = 1;
                    if (p < 3 && q < 3) rotdiff = 1;
                    float d = fabsf(x - y);
                    if (isfinite(d) && d > worst) worst = d;
                }
            }
        if (bad) { if (first < 0) first = i; diff++; }
    }
    printf("%s: %ld cases, %ld differ (%.3f%%), worst |delta| %.6g, first@%ld%s\n",
           selftest ? "SELFTEST (shipped vs shipped)" : "candidate vs shipped",
           n, diff, 100.0 * diff / n, worst, first,
           (!selftest && diff && !rotdiff) ? "  [3x3 rotation part MATCHES]" : "");
    return diff ? 1 : 0;
}
