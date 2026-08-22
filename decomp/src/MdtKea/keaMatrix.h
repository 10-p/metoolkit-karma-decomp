/*=============================================================================
    keaMatrix.h — the MdtKea matrix class hierarchy, recovered as C.

    Milestone 2 deliverable: proof that Karma's C++/vtable code converts to C
    mechanically and ABI-identically. Nothing here is guessed; every field
    offset and vtable slot was read out of the shipped objects.

    HOW THE C++ MAPS ONTO C
    -----------------------
    * `this` is passed on the STACK as the first argument. GCC's i386 C++ ABI is
      plain cdecl — unlike MSVC, which passes `this` in ecx. Verified in the
      disassembly of keaMatrix_tester::factorize:
          767: mov 0x8(%ebp),%ebx     <- `this` from ebp+8, i.e. arg 0
      So a C function taking an explicit `this` pointer is ABI-identical to the
      original C++ method. No thunks, no wrappers.

    * Virtual calls appear in Ghidra output as
          (**(code **)(*(int *)obj + N))(obj, ...)
      The vptr points at vtable slot 2 (Itanium ABI puts offset-to-top at slot 0
      and the typeinfo pointer at slot 1), so byte offset N from the vptr is
      simply the Nth byte of keaMatrix_vtbl below. Cross-checked:
          +0x0c -> factorize()   as called by keaMatrix_tester::factorize
          +0x10 -> solve()       as called by keaMatrix_tester::solve

    * We own these vtables entirely: only libMdtKea references _ZTV*kea* /
      _ZTI*kea*, so no other archive constrains the layout.

    PROVENANCE
    ----------
      layouts   tools/dwarf_structs.py on keaMatrix.o / keaMatrix_PcSparse.o
                (the layout is only in the CU that defines each class — union
                 DWARF across ALL objects to find it)
      vtable    objdump -r --section=.gnu.linkonce.d._ZTV26keaMatrix_pcSparse_vanilla
      offsets   independently corroborated by the keaPoolAlloc() label strings
                in keaMatrix_pcSparse_vanilla::allocate: "A" -> matrix (+0xc),
                "Achol" -> matrixChol (+0x10), "rsD", "NAZ", "NCZ", "NR", "NC".
=============================================================================*/
#ifndef KD_KEAMATRIX_H
#define KD_KEAMATRIX_H

#include "kd_compat.h"

typedef struct keaMatrix        keaMatrix;
typedef struct keaMatrix_vtbl   keaMatrix_vtbl;

/* Slot order is the vtable's own order, starting at the vptr (= C++ slot 2). */
struct keaMatrix_vtbl {
    /* +0x00 */ void (*allocate)(keaMatrix *self, int size);
    /* +0x04 */ void (*makeFromJMJT)(keaMatrix *self, const MeReal *J, const MeReal *M,
                                     const int *a, const int *b, const MeReal *c,
                                     MeReal d, MeReal e);
    /* +0x08 */ void (*makeFromColMajorPSM)(keaMatrix *self, MeReal *out, const MeReal *a,
                                            const MeReal *b, const MeReal *c,
                                            const int *d, const int *e,
                                            int f, int g, int h);
    /* +0x0c */ void (*factorize)(keaMatrix *self);
    /* +0x10 */ void (*solve)(keaMatrix *self, MeReal *x, const MeReal *rhs);
    /* +0x14 */ void (*multiply)(keaMatrix *self, MeReal *out, const MeReal *in);
    /* +0x18 */ void (*solveUnits)(keaMatrix *self, MeReal *x, int *a,
                                   const int *b, int c, int d);
    /* +0x1c */ void (*writebackMatrixChol)(keaMatrix *self);
    /* +0x20 */ void (*prefetchMatrixChol)(keaMatrix *self);
};

/* keaMatrix — 20 bytes. Abstract base. */
struct keaMatrix {
    const keaMatrix_vtbl *vptr;     /* +0x00 */
    int                   m_numRows;/* +0x04 */
    int                   m_padded; /* +0x08 */
    MeReal               *matrix;   /* +0x0c */
    MeReal               *matrixChol;/*+0x10 */
};

/* keaMatrix_pcSparse — 52 bytes. Derives from keaMatrix; the base occupies
   [0x00, 0x14), so it is embedded as the first member to keep the layout
   byte-identical to the original. */
typedef struct keaMatrix_pcSparse {
    keaMatrix  base;                /* +0x00 .. +0x13 */
    int        m_blocks;            /* +0x14 */
    MeReal    *rsD;                 /* +0x18 */
    MeReal   **NAZ;                 /* +0x1c */
    MeReal   **NCZ;                 /* +0x20 */
    MeReal    *mLP;                 /* +0x24 */
    MeReal    *mcLP;                /* +0x28 */
    int       *NR;                  /* +0x2c */
    int       *NC;                  /* +0x30 */
} keaMatrix_pcSparse;               /* sizeof == 0x34 == 52 */

/* keaMatrix_pcSparse_vanilla adds no data members — it only overrides methods,
   so it shares keaMatrix_pcSparse's layout and differs solely in its vtable. */
typedef keaMatrix_pcSparse keaMatrix_pcSparse_vanilla;

/* Compile-time proof the recovered layout matches the shipped one. */
_Static_assert(sizeof(keaMatrix) == 20,           "keaMatrix must be 20 bytes");
_Static_assert(sizeof(keaMatrix_pcSparse) == 52,  "keaMatrix_pcSparse must be 52 bytes");
_Static_assert(offsetof(keaMatrix_pcSparse, m_blocks) == 0x14, "m_blocks at +0x14");
_Static_assert(offsetof(keaMatrix_pcSparse, rsD)      == 0x18, "rsD at +0x18");
_Static_assert(offsetof(keaMatrix_pcSparse, NC)       == 0x30, "NC at +0x30");
_Static_assert(offsetof(keaMatrix, matrix)            == 0x0c, "matrix at +0x0c");
_Static_assert(offsetof(keaMatrix, matrixChol)        == 0x10, "matrixChol at +0x10");

#endif /* KD_KEAMATRIX_H */
