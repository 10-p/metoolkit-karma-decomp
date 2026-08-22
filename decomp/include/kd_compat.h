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
#define NAN_f(x)   isnan(x)

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
#define CONCAT13(hi, lo) CONCAT44(hi, lo)
#define SUB84(x, n)      ((unsigned int)((unsigned long long)(x) >> ((n) * 8)))
#define SUB41(x, n)      ((unsigned char)((unsigned int)(x) >> ((n) * 8)))
#define SUB42(x, n)      ((unsigned short)((unsigned int)(x) >> ((n) * 8)))
#define ZEXT14(x)        ((unsigned int)(unsigned char)(x))
#define ZEXT24(x)        ((unsigned int)(unsigned short)(x))
#define ZEXT48(x)        ((unsigned long long)(unsigned int)(x))
#define SEXT14(x)        ((int)(signed char)(x))
#define SEXT24(x)        ((int)(short)(x))
#define SEXT48(x)        ((long long)(int)(x))

/* Ghidra's generic "some function" type, used for indirect calls through a
   vtable: `(**(code **)(*(int *)obj + N))(obj, ...)`. Spelling it as an
   unprototyped function type makes `code *` a callable function pointer that
   accepts any argument list, which is exactly what those call sites need. */
typedef void code();

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

/* Call a C++-mangled Karma symbol from C without a C++ compiler. */
#define KD_MANGLED(sym) __asm__(sym)

#endif /* KD_COMPAT_H */
