#!/usr/bin/env python3
"""mk_trace_obj.py — turn one exported function of a recovered object into a
SHADOW: it calls the recovered body and the SHIPPED one on identical inputs,
reports the first divergences, and returns the shipped answer.

    mk_trace_obj.py <recovered.c> <kd_fn> <shipped-asm-label>

NO RENAMING, NO MIXED LINK. An earlier attempt renamed the shipped definition
with `objcopy --redefine-sym` so a wrapper could take its name — but objcopy
renames the CALL SITES in the same object too, so the shipped caller went
straight to the renamed original and the wrapper was never reached. It printed
nothing and looked like a harness that had run.

It does not need renaming: `_Z13OverlapCylCyl...` is a GLOBAL in the shipped
object, and `difftest_pair.sh` stages the recovered object with its own symbols
prefixed `rec_`. So the shipped name is free, and the shadow just declares it.

The wrapper RETURNS THE SHIPPED ANSWER, so a divergence does not steer the rest
of the run: every call is measured, not just the first bad one. Both scratch
outputs are SEEDED from the caller's, so a path that writes nothing compares
equal rather than reading as a difference.
"""
import re
import sys

FN = 'kd_OverlapCylCyl'
P = ('MeReal *outSep,lsVec3 *outN,MeReal *outPN,lsVec3 **outPos,MeI16 *outDims,'
     'MeReal inEps,MeReal inR1,MeReal inHH1,MeReal inR2,MeReal inHH2,'
     'lsTransform *inT12,MeReal scale')
A = 'outSep,outN,outPN,outPos,outDims,inEps,inR1,inHH1,inR2,inHH2,inT12,scale'
RA = '&rSep,&rN,&rPN,&rP,&rDims,inEps,inR1,inHH1,inR2,inHH2,inT12,scale'


def main():
    src = open(sys.argv[1]).read()
    label = sys.argv[2]

    ban = re.search(r'(?m)^/\* ---- \S+ \(exported as ' + re.escape(FN) + r',.*$', src)
    if not ban:
        sys.exit('mk_trace_obj: no banner for ' + FN)
    d = re.search(r'(?m)^bool\s+' + re.escape(FN) + r'\s*\(', src[ban.end():])
    if not d:
        sys.exit('mk_trace_obj: no definition after the banner')
    at = ban.end() + d.start()
    src = src[:at] + src[at:].replace(FN + '(', 'kd_traced_' + FN + '(', 1)

    # STRIP THE MANGLED ASM LABEL from the forward declaration. Without this the
    # shadow's own definition IS `_Z13OverlapCylCyl...` — the same symbol the
    # `extern` below names — so the compiler resolves the shadow's call to
    # ITSELF and the run dies of unbounded recursion. Removing it makes the
    # recovered symbol plain `kd_OverlapCylCyl`, which difftest_pair's staging
    # then renames to `rec_kd_OverlapCylCyl`, leaving the shipped name free and
    # UNRENAMED in this object (objcopy only rewrites names it DEFINES).
    n = 1
    src, n = re.subn(r'(bool ' + re.escape(FN) + r'\([^;]*?\))\s*KD_MANGLED\("[^"]+"\)',
                     r'\1', src, count=1)
    if n != 1:
        sys.exit('mk_trace_obj: could not strip KD_MANGLED from %s' % FN)

    wrap = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern bool kd_shipped_OverlapCylCyl(%(P)s) __asm__("%(L)s");
bool kd_traced_%(F)s(%(P)s);
long kd_calls, kd_bad, kd_dret, kd_dsep, kd_dpn, kd_ddims, kd_dn, kd_dnp;
static int kd_shown;
static int kd_discrete = -1;
bool %(F)s(%(P)s)
{
  static lsVec3 sbuf[1024];
  if (kd_discrete < 0) kd_discrete = getenv("KD_DISCRETE") != NULL;
  MeReal rSep = *outSep, rPN = *outPN; lsVec3 rN = *outN; MeI16 rDims = *outDims;
  lsVec3 *rP = sbuf, *p0 = *outPos;
  bool r = kd_traced_%(F)s(%(RA)s);
  bool o = kd_shipped_OverlapCylCyl(%(A)s);
  int nr = (int)(rP - sbuf), no = (int)(*outPos - p0);
  /* BITWISE: a degenerate configuration yields NaN on both sides, and NaN != NaN */
  #define NE(a,b) (memcmp(&(a), &(b), sizeof (a)) != 0)
  int dr = (r != o), dnp = (nr != no);
  int ds = NE(rSep, *outSep), dp = NE(rPN, *outPN), dd = NE(rDims, *outDims);
  int dn = NE(rN.v[0], outN->v[0]) || NE(rN.v[1], outN->v[1]) || NE(rN.v[2], outN->v[2]);
  kd_calls++; kd_dret += dr; kd_dsep += ds; kd_dpn += dp;
  kd_ddims += dd; kd_dn += dn; kd_dnp += dnp;
  if (dr || ds || dp || dd || dn || dnp) {
    kd_bad++;
    /* KD_DISCRETE=1 prints ONLY the divergences that change a decision. sep/PN
       differ on ~half of all calls at one ULP and would bury the 20 dims and
       the 1 npts, which are the whole remaining charge against this pair. */
    if (kd_shown < 8 && (!kd_discrete || dr || dd || dnp)) {
      kd_shown++;
      fprintf(stderr,
        "CC#%%d call %%ld  ret %%d/%%d  npts %%d/%%d  dims %%d/%%d\n"
        "    sep %%.9g/%%.9g   PN %%.9g/%%.9g\n"
        "    N   (%%.9g %%.9g %%.9g) / (%%.9g %%.9g %%.9g)\n"
        "    eps %%.9g R1 %%.9g HH1 %%.9g R2 %%.9g HH2 %%.9g scale %%.9g\n",
        kd_shown, kd_calls, (int)r, (int)o, nr, no, (int)rDims, (int)*outDims,
        (double)rSep, (double)*outSep, (double)rPN, (double)*outPN,
        (double)rN.v[0], (double)rN.v[1], (double)rN.v[2],
        (double)outN->v[0], (double)outN->v[1], (double)outN->v[2],
        (double)inEps, (double)inR1, (double)inHH1, (double)inR2, (double)inHH2,
        (double)scale);
    }
  }
  return o;
}
__attribute__((destructor)) static void kd_report(void)
{
  fprintf(stderr, "OVERLAPCYLCYL: %%ld calls, %%ld diverged"
          "  (ret %%ld, sep %%ld, PN %%ld, dims %%ld, N %%ld, npts %%ld)\n",
          kd_calls, kd_bad, kd_dret, kd_dsep, kd_dpn, kd_ddims, kd_dn, kd_dnp);
}
''' % {'P': P, 'A': A, 'RA': RA, 'F': FN, 'L': label}
    i = src.index('/* ---- C++ ABI data')
    sys.stdout.write(src[:i] + wrap + src[i:])


if __name__ == '__main__':
    main()
