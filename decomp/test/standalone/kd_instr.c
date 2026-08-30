/* kd_instr.c — exact per-function call counter for the substitute scenes.
 *
 * WHY THIS EXISTS. substitute_test.sh reports "trajectory bit-identical" for
 * every one of the 93 recovered objects on scene_chain.c, including the nine
 * libMdtKea solver objects. For a Ghidra decompile of a numerical routine that
 * is not a plausible result, and there are only two explanations: the recovery
 * is perfect, or the object never ran. The gate could not tell them apart,
 * because "substituted and the answer did not change" and "substituted and the
 * code was never reached" produce the identical line of output.
 *
 * This is the same idea as KD_CENSUS in kd_shadow.c — count, do not perturb —
 * applied to the offline scenes instead of to a live match. It uses GCC's
 * -finstrument-functions, so it counts EVERY function in the instrumented
 * translation units, including the file-local ones, with no sampling and no
 * mutation of the code under test.
 *
 * Only the recovered .c files are compiled with -finstrument-functions, so the
 * shipped library, the scene and libc cost nothing.
 *
 * Output: $KD_INSTR_OUT (default /tmp/kd_instr.txt), one "<hex addr> <count>"
 * line per distinct function, written at exit. scene_census.sh symbolises it
 * against the linked executable.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KD_INSTR_SLOTS 8192          /* power of two; open addressing */

/* Everything here must be exempt, or instrumenting the counter recurses. The
 * file is a separate TU compiled without -finstrument-functions, so this is
 * belt and braces — but the attribute also survives someone adding the flag to
 * the whole build later. */
#define NOINSTR __attribute__((no_instrument_function))

static void *kd_key[KD_INSTR_SLOTS];
static unsigned long kd_cnt[KD_INSTR_SLOTS];
static int kd_used;
static int kd_registered;
static int kd_overflow;

static NOINSTR unsigned kd_hash(void *p)
{
    unsigned long v = (unsigned long)p;
    v ^= v >> 13; v *= 0x9E3779B1u; v ^= v >> 15;
    return (unsigned)(v & (KD_INSTR_SLOTS - 1));
}

static NOINSTR void kd_instr_dump(void)
{
    const char *path = getenv("KD_INSTR_OUT");
    FILE *f = fopen(path && *path ? path : "/tmp/kd_instr.txt", "w");
    int i;
    if (!f) return;
    /* The header is a comment so the file stays greppable, and it carries the
     * overflow flag: a silently truncated census reads exactly like a function
     * that was never called, which is the failure this tool exists to stop. */
    fprintf(f, "# kd_instr: %d distinct functions, overflow=%d\n",
            kd_used, kd_overflow);
    for (i = 0; i < KD_INSTR_SLOTS; i++)
        if (kd_key[i]) fprintf(f, "%p %lu\n", kd_key[i], kd_cnt[i]);
    fclose(f);
}

NOINSTR void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
    unsigned h = kd_hash(this_fn);
    unsigned start = h;
    (void)call_site;
    for (;;) {
        if (kd_key[h] == this_fn) { kd_cnt[h]++; break; }
        if (kd_key[h] == NULL) {
            kd_key[h] = this_fn; kd_cnt[h] = 1; kd_used++;
            break;
        }
        h = (h + 1) & (KD_INSTR_SLOTS - 1);
        if (h == start) { kd_overflow = 1; break; }
    }
    if (!kd_registered) { kd_registered = 1; atexit(kd_instr_dump); }
}

NOINSTR void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
    (void)this_fn; (void)call_site;
}
