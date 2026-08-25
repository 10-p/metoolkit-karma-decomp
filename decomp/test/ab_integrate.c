/* ab_integrate.c — per-function differential driver for keaIntegrate_pc.
 *
 * The three substitute scenes answer "does the trajectory diverge, and from
 * which step". That is the right instrument for finding THAT an object is
 * wrong and the wrong one for finding WHERE: a last-bit error at step 13 of a
 * chaotic chain tells you nothing about which expression produced it.
 *
 * This calls MdtKeaIntegrateSystem and the recovered rec_MdtKeaIntegrateSystem
 * on identical randomised body state and reports the first FIELD that differs,
 * bitwise, per body. It localised the 1.5e-08 of HANDOVER.md §11 item 2 to
 * `qrot` in one run and to the dropped dq[1..3] spill in three more.
 *
 * Build it with test/ab_integrate.sh, which renames the recovered symbols so
 * both implementations can live in one link.
 *
 * TWO THINGS ABOUT IT ARE LOAD-BEARING, both learned by getting them wrong:
 *
 *  - the body count sweeps 1..NB. The velocity update is unrolled by two with
 *    an odd-body PEEL in front, and a driver that only ever passes 8 bodies
 *    never enters the peel. A control planted there read zero and looked like
 *    a clean result.
 *  - run the controls. Perturbing one term by 1.0000001f must move the column
 *    it belongs to and no other; see proven.txt for the numbers each one gives.
 *
 * The quaternions are normalised before the call because that is the only
 * state the integrator ever sees — MdtWorldStep renormalises every step — and
 * an unnormalised one exercises arithmetic the game never reaches.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <MePrecision.h>
#include <MdtKea.h>

void MEAPI MdtKeaIntegrateSystem(MdtKeaBody *const*blist, MdtKeaTransformation *tlist,
                                 int n, MdtKeaParameters p);
void MEAPI rec_MdtKeaIntegrateSystem(MdtKeaBody *const*blist, MdtKeaTransformation *tlist,
                                     int n, MdtKeaParameters p);

#define NB 8
static unsigned st = 12345;
static float rf(float lo, float hi){ st = st*1103515245u + 12345u;
    return lo + (hi-lo)*(float)((st>>8)&0xffffff)/(float)0x1000000; }

static int quiet=0;
static int nfirst=0;
static int cmpblk(const char *what, const void *a, const void *b, int nfloats, int iter, int body){
    const unsigned *x=a,*y=b; int d=0;
    for(int i=0;i<nfloats;i++) if(x[i]!=y[i]){
        if(!quiet && nfirst<8){ nfirst++;
        printf("iter %d body %d  %s[%d]  %08x vs %08x  (%.9g vs %.9g)\n",
               iter, body, what, i, x[i], y[i], (double)((const float*)a)[i], (double)((const float*)b)[i]); }
        d=1;
    }
    return d;
}

int main(int argc, char**argv){
    int iters = argc>1?atoi(argv[1]):200000;
    int fast  = argc>2?atoi(argv[2]):0;
    int nd=0, nq=0, nv=0, nw=0, nt=0, nb=0;
    if(argc>3) quiet=1;
    for(int it=0; it<iters; it++){
        int n = 1 + (it % NB);   /* cover the odd-body peel and every unroll residue */
        MdtKeaBody ba[NB], bb[NB];
        MdtKeaBody *pa[NB], *pb[NB];
        MdtKeaTransformation ta[NB], tb[NB];
        MdtKeaParameters par;
        memset(ba,0,sizeof ba); memset(ta,0,sizeof ta); memset(&par,0,sizeof par);
        for(int i=0;i<n;i++){
            ba[i].flags = fast ? 1 : 0;
            for(int k=0;k<3;k++){ ba[i].vel[k]=rf(-5,5); ba[i].velrot[k]=rf(-9,9);
                                  ba[i].accel[k]=rf(-30,30); ba[i].accelrot[k]=rf(-30,30); }
            /* a normalised quaternion, as the integrator always sees */
            float q[4]; float n=0;
            for(int k=0;k<4;k++){ q[k]=rf(-1,1); n+=q[k]*q[k]; }
            n = 1.0f/(float)sqrt(n);
            for(int k=0;k<4;k++) ba[i].qrot[k]=q[k]*n;
            /* unit fast-spin axis */
            float f[3]; n=0;
            for(int k=0;k<3;k++){ f[k]=rf(-1,1); n+=f[k]*f[k]; }
            n=1.0f/(float)sqrt(n);
            for(int k=0;k<3;k++) ba[i].fastSpinAxis[k]=f[k]*n;
            for(int k=0;k<3;k++){ ta[i].pos[k]=rf(-50,50); }
            ta[i].pos[3]=1.0f;
        }
        par.stepsize = 1.0f/60.0f;
        memcpy(bb,ba,sizeof ba); memcpy(tb,ta,sizeof ta);
        for(int i=0;i<n;i++){ pa[i]=&ba[i]; pb[i]=&bb[i]; }
        MdtKeaIntegrateSystem(pa, ta, n, par);
        rec_MdtKeaIntegrateSystem(pb, tb, n, par);
        for(int i=0;i<n;i++){
            if(cmpblk("qrot",   ba[i].qrot,   bb[i].qrot,   4, it, i)) nq++;
            if(cmpblk("vel",    ba[i].vel,    bb[i].vel,    3, it, i)) nv++;
            if(cmpblk("velrot", ba[i].velrot, bb[i].velrot, 3, it, i)) nw++;
            if(cmpblk("T",      &ta[i],       &tb[i],      16, it, i)) nt++;
            nb++;
        }
    }
    printf("bodies=%d  qrot=%d  vel=%d  velrot=%d  T=%d   [fastspin=%d]\n", nb, nq, nv, nw, nt, fast);
    nd = nq+nv+nw+nt;
    return nd!=0;
}
