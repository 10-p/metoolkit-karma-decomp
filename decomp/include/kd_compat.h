/*=============================================================================
    kd_compat.h — compatibility shims for Ghidra-decompiled Karma sources.

    Ghidra emits a small vocabulary of its own types and helper macros. Rather
    than rewrite every recovered function by hand, we define that vocabulary
    once, here, in terms of real C.

    The load-bearing decision in this file is `longdouble` (see below).
=============================================================================*/
#ifndef KD_COMPAT_H
#define KD_COMPAT_H

#include <math.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
/* Ghidra emits glibc's __ctype_b_loc() rather than the isalpha()-family macros
   that expand to it, because that is what the object calls. Only <ctype.h>
   declares it. MeXMLTree failed on nothing else. */
#include <ctype.h>

/* glibc's name for qsort's comparator, which Ghidra emits because that is what
   the object's DWARF calls it. It does not exist under Emscripten, so
   McdPolygonIntersection compiled for i386 and not for wasm32. The guard macro
   is glibc's own, so this is a no-op where the header already provided it. */
#ifndef __COMPAR_FN_T
# define __COMPAR_FN_T
typedef int (*__compar_fn_t)(const void *, const void *);
#endif

/* ---- what gcc 3.2 turned the standard library into ----------------------
    A call to strtol comes back out of the decompiler as __strtol_internal,
    because that is the symbol gcc 3.2 emitted against glibc 2.x. Declaring the
    glibc spelling would work on i386 and nowhere else, and this has to build
    for wasm32 and arm64 (see HANDOVER.md 12), so map each one back to the
    portable function it stands for.

    Variadic, because Ghidra over-counts the arguments at these call sites —
    MeCommandLine's __strtod_internal comes back with four. The extra ones are
    invisible to the callee under caller-cleanup cdecl, and the named arguments
    are the ones that matter.
------------------------------------------------------------------------- */
#define __strtol_internal(s, e, b, ...)  strtol((s), (e), (b))
#define __strtoul_internal(s, e, b, ...) strtoul((s), (e), (b))
#define __strtod_internal(s, e, ...)     strtod((s), (e))
#define _IO_putc(c, f)                   putc((c), (f))
#define _IO_getc(f)                      getc(f)
#define builtin_strncpy(d, s, n)         strncpy((d), (s), (n))
#define builtin_strcpy(d, s)             strcpy((d), (s))
#define builtin_memcpy(d, s, n)          memcpy((d), (s), (n))

/* ---- Ghidra integer vocabulary ---------------------------------------- */
typedef unsigned int        uint;
typedef unsigned short      ushort;
typedef unsigned char       byte;
typedef signed char         sbyte;
typedef unsigned char       undefined;
typedef unsigned char       undefined1;
typedef unsigned short      undefined2;
typedef unsigned int        undefined4;
typedef unsigned long long  undefined8;
typedef unsigned char       undefined3[3];
typedef unsigned long long  ulonglong;
typedef long long           longlong;
typedef unsigned int        uint3;
typedef unsigned char       uchar;
typedef unsigned short      ushort2;

/* ---- x87 extended precision -------------------------------------------
    Ghidra emits `longdouble` where a value provably lives in an x87 register
    across operations, i.e. where the original gcc 3.2 i386 build really did
    compute at 80-bit.

    We map it to `double`, NOT to C `long double`, deliberately:

      * On wasm, `long double` is a software-emulated 128-bit quad — correct
        but catastrophically slow, and Karma has no need of it.
      * MathEngine themselves shipped a pure-f32 build (lib.rel/linux_hx_single,
        x86-64/SSE), so f32 intermediates are vendor-blessed; f64 is strictly
        more precise than that.
      * Measured divergence between the two shipped builds is 0.25 mm over 15 s
        of articulated simulation without collisions, and unbounded with them —
        so 80-bit fidelity buys nothing observable. See docs/KARMA-ON-WASM.md §II.

    `double` is native and fast on wasm, arm64, armv7 and x86-64 alike.
------------------------------------------------------------------------- */
typedef double              longdouble;

/* ---- Ghidra helper macros --------------------------------------------- */
#ifndef ABS
#  define ABS(x)   fabsf(x)
#endif
#ifndef SQRT
#  define SQRT(x)  sqrtf(x)
#endif
#define ABSF(x)    fabsf(x)
#define ABSD(x)    fabs(x)
#define SQRTF(x)   sqrtf(x)

/* ROUND is emitted for x87 `fistp`, which converts using the CURRENT rounding
   mode — by default round-to-nearest, ties-to-even. It is NOT truncation, so
   `(int)x` would be wrong. rintf() has exactly fistp's semantics. */
#define ROUND(x)   rintf(x)

/* Bit-level reinterpretation helpers Ghidra emits for register subfields.
   CONCATab glues an a-byte and a b-byte value into an (a+b)-byte one;
   SUBab extracts the low b bytes of an a-byte value; ZEXT/SEXT widen. */
#define CONCAT44(hi, lo) \
    ((unsigned long long)(unsigned int)(hi) << 32 | (unsigned int)(lo))
#define CONCAT22(hi, lo) \
    ((unsigned int)(unsigned short)(hi) << 16 | (unsigned short)(lo))
#define CONCAT31(hi, lo) \
    ((unsigned int)((hi) & 0xffffffu) << 8 | (unsigned char)(lo))
#define CONCAT13(hi, lo) \
    ((unsigned int)(unsigned char)(hi) << 24 | ((lo) & 0xffffffu))
#define CONCAT11(hi, lo) \
    ((unsigned short)((unsigned char)(hi) << 8 | (unsigned char)(lo)))
#define SUB84(x, n)      ((unsigned int)((unsigned long long)(x) >> ((n) * 8)))
#define SUB41(x, n)      ((unsigned char)((unsigned int)(x) >> ((n) * 8)))
#define SUB42(x, n)      ((unsigned short)((unsigned int)(x) >> ((n) * 8)))
#define ZEXT14(x)        ((unsigned int)(unsigned char)(x))
#define ZEXT24(x)        ((unsigned int)(unsigned short)(x))
#define ZEXT48(x)        ((unsigned long long)(unsigned int)(x))
#define SEXT14(x)        ((int)(signed char)(x))
#define SEXT24(x)        ((int)(short)(x))
#define SEXT48(x)        ((long long)(int)(x))

/* ---- x86 flag computations ---------------------------------------------
    Ghidra keeps the arithmetic flags as explicit expressions when the original
    used one as a value — gcc 3.2 does this constantly for `a < b` on unsigned
    types, and for the `sbb`/`adc` branchless-compare idiom. CARRYn is the carry
    OUT of an n-byte unsigned add; SBORROWn is signed overflow of an n-byte
    subtract, which is what `<` on signed ints compiles to together with the
    sign flag.
------------------------------------------------------------------------- */
#define CARRY1(a, b)  ((unsigned int)((unsigned char)(a) + (unsigned char)(b)) > 0xffu)
#define CARRY2(a, b)  ((unsigned int)((unsigned short)(a) + (unsigned short)(b)) > 0xffffu)
#define CARRY4(a, b)  ((unsigned int)((unsigned int)(a) + (unsigned int)(b)) < (unsigned int)(a))
#define SBORROW1(a, b) \
    ((signed char)(((signed char)(a) ^ (signed char)(b)) \
                   & ((signed char)(a) ^ (signed char)((a) - (b)))) < 0)
#define SBORROW2(a, b) \
    ((short)(((short)(a) ^ (short)(b)) & ((short)(a) ^ (short)((a) - (b)))) < 0)
#define SBORROW4(a, b) \
    ((int)(((int)(a) ^ (int)(b)) & ((int)(a) ^ (int)((a) - (b)))) < 0)

/* The x86 LOCK prefix, which Ghidra brackets around the instruction it guards.
   Karma is single-threaded within a world step and these appear only in the
   inherited `MeAtomic` helpers, so the guarded statement stands alone. */
#define LOCK()    ((void)0)
#define UNLOCK()  ((void)0)

/* ---- x87 transcendental instructions -----------------------------------
    gcc 3.2 -O2 on i386 expands sin/cos/atan2 to the FPU instructions rather
    than calling libm, so what comes back out of the decompiler is the
    INSTRUCTION, not the function. Ghidra models each as a pcodeop of the same
    name, and its arity/order is fixed by the sleigh definition — for FPATAN,

        ia.sinc:5953   ST1 = fpatan(ST1, ST0);

    and the instruction computes atan(ST1/ST0) over the full circle, which is
    exactly atan2(ST1, ST0). So `fpatan(a, b)` is `atan2(a, b)`, in that order.
    Confirmed at a call site: MeQuaternionSlerp computes
    `fpatan(SQRT(1 - c*c), c)`, i.e. acos(c), which is what a slerp needs.

    These take `longdouble` (= double) operands, so the double-precision libm
    entry points are the right ones: they match x87's internal precision more
    closely than the float ones would.
------------------------------------------------------------------------- */
#define fsin(x)        sin(x)
#define fcos(x)        cos(x)
#define fptan(x)       tan(x)
#define fsqrt(x)       sqrt(x)
#define fpatan(y, x)   atan2((y), (x))
#define f2xm1(x)       (exp2(x) - 1.0)
#define fyl2x(y, x)    ((y) * log2(x))
#define frndint(x)     rint(x)

/* Ghidra's NaN TEST is `NAN(x)`, which collides with C99's NAN constant —
   `(0.0f/0.0f)(x)` fails with "called object is not a function". It also emits
   the bare constant, in the same corpus, so neither spelling can simply win:
   ghidra_clean rewrites the call form to isnan() and leaves the constant to
   <math.h>. This alias is kept because the dumps use it too. */
#define NAN_f(x)   isnan(x)

/* Ghidra's generic "some function" type, used for indirect calls through a
   vtable or a stored callback: `(**(code **)(*(int *)obj + N))(obj, ...)`.
   An unprototyped function type makes `code *` a callable pointer accepting any
   argument list, which is what those call sites need.

   It returns int, not void: some call sites USE the result
   (`count = (*pcVar11)();` in IxSphereTriList), and a void return makes those
   "invalid use of void expression". Discarding an int return is legal C, so int
   is correct for both kinds of site while void is correct for only one. */
typedef int code();

/* Variable-length stack allocations are restored as real alloca() calls; see
   materialise_alloca_frame() in tools/ghidra_clean.py for why a fixed-size
   buffer was the wrong answer. */
#include <alloca.h>

/* ---- calling-convention annotations ------------------------------------
    Ghidra tags recovered C++ methods with MSVC's `__thiscall`. On GCC's i386
    C++ ABI `this` is passed on the STACK as the first argument — plain cdecl,
    not MSVC's ecx convention — so the annotation is a no-op for us. Verified in
    the disassembly of keaMatrix_tester::factorize:

        767:  mov 0x8(%ebp),%ebx      <- `this` from ebp+8, i.e. argument 0

    That is what makes a C function with an explicit `this` parameter
    ABI-identical to the original method: no thunks, no wrappers. We define the
    keyword away rather than strip it, so the recovered sources keep the
    annotation as documentation of which functions were methods.
------------------------------------------------------------------------- */
#ifndef __thiscall
#  define __thiscall
#endif
#ifndef __cdecl
#  define __cdecl
#endif
#ifndef __fastcall
#  define __fastcall
#endif

/* Ghidra also emits __regparmN, claiming the first N arguments arrive in
   registers. As a CALLING CONVENTION it is a misdetection, and defining it away
   is right: gcc 3.2's i386 ABI passes everything on the stack here.

   But do not read that as "harmless". Ghidra does not just annotate — it lays
   the parameter list out to match, so in a __regparmN function every parameter
   name in the BODY is shifted by N and the last incoming argument falls off the
   end entirely:

       MeI16 __regparm1 McdGeometryGetMassProperties(g, relTM, m, volume)
       { return (**vtable)(relTM, m, volume); }

   The original pushes four arguments starting with `g`; this passes three,
   starting with the second. It compiled, it linked, it was bit-identical on the
   collision-free scene, and it segfaulted on the first collision scene. Every
   other instance is shifted the same way — McdConvexMeshCreate dereferences
   `fatness` as a pointer, McdGeometryInstanceDestroy never touches either
   declared parameter and reads `in_stack_*` instead.

   So recover.py holds any object containing __regparmN out of the validated
   set. The macro exists only so the file still parses; it is not a statement
   that the function is correct. */
#ifndef __regparm0
#  define __regparm0
#endif
#ifndef __regparm1
#  define __regparm1
#endif
#ifndef __regparm2
#  define __regparm2
#endif
#ifndef __regparm3
#  define __regparm3
#endif

/* ---- Karma-internal types ---------------------------------------------
    These live in kd_types.h, generated by tools/gen_typedb.py from the DWARF
    of all 192 objects. They are NOT hand-written: a class layout is complete
    only in the CU that defines it, so the generator unions across every object
    and keeps the richest definition.

    (Recovering them by hand is exactly where mistakes creep in — the
    hand-written lsTransformRow here used to read `lsVec3 v; float pad;`, when
    the real member is an lsVec4. Same 16 bytes, but `v.v[3]` is a genuine
    component, not padding.)
------------------------------------------------------------------------- */
/* see kd_types.h */

/* Reinterpret a float's four bytes as an integer, for the case where Ghidra
   typed a memory slot as a pointer and the original stored a float in it:

       pMVar4[1].prev = (McdGeometryID)(dy * 0.5);

   C rejects converting a float to a pointer, and there is nothing to convert —
   the bytes are already what the original wrote. Going through the integer
   preserves them exactly, and the pointer cast that follows is then an ordinary
   integer-to-pointer conversion, which is well defined on every 32-bit-pointer
   target this project builds for. */
static __inline unsigned int KD_FBITS(double f)
{
    float v = (float)f;
    unsigned int u;
    __builtin_memcpy(&u, &v, sizeof u);
    return u;
}

/* ---- tags metoolkit's own headers use C++-style -------------------------
    A few public headers define `struct X { ... };` and then go on to use bare
    `X` as a type name, which is C++ and not C:

        struct McdErrorDescription { MeI16 m_errNum; ... };
        void McdError(McdErrorDescription* ErrorList, ...);   <- needs `struct`

    The header is what it is; these aliases make it parse. They must come BEFORE
    kd_karma.h, which is why they live here rather than in the generated
    kd_types.h — the typedef forward-declares the tag and the header completes
    it later.

    This is deliberately a LIST and not a rule. HANDOVER.md §9 dead end 1:
    aliasing every public tag takes the build to zero, because `MePoolAPI` is a
    struct tag AND an ordinary identifier and `typedef struct MePoolAPI
    MePoolAPI;` is then "redeclared as a different kind of symbol". Every name
    below was found by scanning the headers for a tag that is defined, never
    typedef'd, and used bare — and each one checked to be a tag only.
------------------------------------------------------------------------- */
typedef struct McdErrorDescription McdErrorDescription;
typedef struct MePoolFixed         MePoolFixed;
typedef struct MePoolMalloc        MePoolMalloc;

/* Call a C++-mangled Karma symbol from C without a C++ compiler. */
#define KD_MANGLED(sym) __asm__(sym)

/* A symbol the shipped object exports WEAKLY has to stay weak, and this is not
   cosmetic. gcc emits `putchar` weakly into keaDebug.o, keaMatrix_tester.o and
   keaPrintBasicTypes.o — three separate members of libMdtKea.a — precisely so
   that libc's strong definition wins and none of them is ever used. Recover the
   same function as a GLOBAL definition and it stops being a decompiled
   curiosity and becomes the putchar the whole engine calls; recover two of the
   three and the link fails outright with a duplicate symbol.

   The same applies to the C++ functions gcc emits weakly because they are
   defined in a header (IxCylinderCylinder's CylPerpAndPara, Polynomial's
   BracketedRootN): weak is how the ODR is enforced at link time, and a strong
   definition silently wins over the copy the linker would otherwise have
   chosen. */
#define KD_WEAK __attribute__((weak))

#endif /* KD_COMPAT_H */
