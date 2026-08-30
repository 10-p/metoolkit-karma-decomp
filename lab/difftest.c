/* Differential oracle: original Karma binary vs Ghidra-decompiled+recompiled C.
   Identical random inputs -> compare outputs bit-for-bit. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
typedef float  MeReal;  typedef short MeI16;  typedef unsigned char MeU8;
typedef unsigned int uint; typedef unsigned short ushort;
typedef unsigned char byte; typedef signed char sbyte;
typedef struct { float v[3]; }                    lsVec3;          /* 12 bytes */
typedef struct { lsVec3 v; float pad; }           lsTransformRow;  /* 16 bytes */
typedef struct { lsTransformRow row[4]; }         lsTransform;     /* 64 bytes */

/* the ORIGINAL, globalized out of the shipped archive */
extern int orig_OverlapOBBs(MeReal*,lsVec3*,MeReal*,MeI16*,MeReal,lsVec3*,lsVec3*,lsTransform*)
    __asm__("_Z21McdVanillaOverlapOBBsRfR6lsVec3S_RsfRKS0_S4_RK11lsTransform");

#include "obb_clean.c"   /* defines static int mine_OverlapOBBs(...) */

static uint32_t rs=0x12345678u;
static float frnd(float lo,float hi){ rs^=rs<<13; rs^=rs>>17; rs^=rs<<5;
    return lo + (hi-lo)*((rs>>8)*(1.0f/16777216.0f)); }

int main(void){
    int N=200000, mism=0, hits=0, tested=0, nexact=0, nfpnoise=0, nbig=0, nret=0, ndims=0; double maxrel=0;
    for(int t=0;t<N;t++){
        lsVec3 r1,r2; lsTransform T;
        for(int i=0;i<3;i++){ r1.v[i]=frnd(0.2f,2.0f); r2.v[i]=frnd(0.2f,2.0f); }
        /* random rotation-ish + translation, biased to produce both hits and misses */
        for(int i=0;i<4;i++){ for(int j=0;j<3;j++) T.row[i].v.v[j]=frnd(-1.f,1.f); T.row[i].pad=0; }
        for(int j=0;j<3;j++) T.row[3].v.v[j]=frnd(-4.f,4.f);
        MeReal sepA=0,pnA=0, sepB=0,pnB=0; lsVec3 nA={{0,0,0}},nB={{0,0,0}};
        MeI16 dA=0,dB=0;
        int a = orig_OverlapOBBs(&sepA,&nA,&pnA,&dA,0.001f,&r1,&r2,&T);
        int b = mine_OverlapOBBs(&sepB,&nB,&pnB,&dB,0.001f,&r1,&r2,&T);
        tested++; if(a) hits++;
        int exact = (a==b) && !memcmp(&sepA,&sepB,4) && !memcmp(&pnA,&pnB,4)
                    && !memcmp(&nA,&nB,sizeof nA) && dA==dB;
        if(exact){ nexact++; continue; }
        if(a!=b){ nret++; mism++; continue; }
        if(dA!=dB){ ndims++; mism++; continue; }
        double e=0;
        e=fmax(e,fabs((double)sepA-sepB)/(fabs((double)sepA)+1e-9));
        e=fmax(e,fabs((double)pnA -pnB )/(fabs((double)pnA )+1e-9));
        for(int i=0;i<3;i++) e=fmax(e,fabs((double)nA.v[i]-nB.v[i])/(fabs((double)nA.v[i])+1e-9));
        if(e>maxrel) maxrel=e;
        if(e<1e-5) nfpnoise++; else { nbig++; mism++;
            if(nbig<3) printf("  REAL MISMATCH #%d rel=%.3e ret %d/%d sep %.9g/%.9g pn %.9g/%.9g\n",
                              t,e,a,b,(double)sepA,(double)sepB,(double)pnA,(double)pnB); }
    }
    printf("\n%d cases, %d overlapping (%.1f%%)\n", tested,hits,100.0*hits/tested);
    printf("  bit-identical to shipped binary : %7d (%.3f%%)\n", nexact, 100.0*nexact/tested);
    printf("  FP rounding noise (rel < 1e-5)  : %7d (%.3f%%)\n", nfpnoise, 100.0*nfpnoise/tested);
    printf("  --------------------------------------------------\n");
    printf("  DIFFERENT RETURN VALUE          : %7d\n", nret);
    printf("  DIFFERENT dims/feature code     : %7d\n", ndims);
    printf("  numeric diff >= 1e-5 relative   : %7d\n", nbig);
    printf("  max relative error seen         : %.3e\n", maxrel);
    printf("\nRESULT: %s\n", (nret+ndims+nbig)==0
        ? "LOGICALLY IDENTICAL (differences are FP rounding only)"
        : "genuine behavioural divergence present");
    return mism!=0;
}
