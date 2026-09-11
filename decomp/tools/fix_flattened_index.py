#!/usr/bin/env python3
"""fix_flattened_index.py — a 2-D LOCAL array's ROW 0 subscripted PAST ITS INNER BOUND, which is
undefined and which GCC at -O2 is entitled to reason from.

    fix_flattened_index.py <kd_lp64/allobj> <kd_build> [metoolkit-root]

THE DEFECT. Ghidra renders the original's linear walk over a matrix as a subscript on the FIRST
row with an index that runs past that row:

    MeReal m1 [3] [3];
    ...
    m1[0][iVar9]     = ...;      /* iVar9 = 0, 3, 6 */
    m1[0][iVar9 + 1] = ...;
    m1[0][iVar9 + 2] = ...;

At the machine level that is exactly what the shipped object does — nine contiguous floats.
In C it is undefined: `m1[0]` is an array of THREE, and an index of 3..8 is outside it. GCC's
value-range propagation, on at -O2 and off at -O1/-Og, takes the bound as a fact, concludes the
second and third iterations of the enclosing loop cannot legally happen, and cuts the loop to one
iteration. GCC says so, once `-w` is lifted:

    warning: iteration 1 invokes undefined behavior [-Waggressive-loop-optimizations]

★ THIS IS THE WHOLE -O2 DIVERGENCE OF THE RECOVERED KARMA (ufront 2.58, 2026-09-11). A complement
bisect named `keaCalcIworldandNonInertialForceandVhmf_vanilla.c` alone: rows 2-3 of the world
inertia tensor were never written, the first constrained body solved wrong at frame 1, and the
reference trace (600 body rows / K=1396 / c31ed77b7323 at -O0) became 543 / 1074 / 179ace20fb15
at -O2. The SDK's own amd64 build compares its loop counters to 3 twelve times — three iterations
were always the intent. See proven.txt O2-VRP-FLATTENED-INDEX.

THE REPAIR keeps the ADDRESS and removes the BOUND the optimiser reasons from — the array decayed
to a pointer:

    NAME[0][j]        ->   ((T *)NAME)[j]        (T NAME[N][M]; j is the linear walk index)
    (*P)[j]           ->   ((T *)*P)[j]          (P a row pointer `T (*P)[M]`)

⚠⚠ THIS PASS IS UNIQUE: IT IS *NOT* AN i386 NO-OP, AND IT IS NOT MEANT TO BE. Fixing a -O2
miscompile IS a -O2 codegen change; GCC even lays out the raw `[0][...]` form differently at -O0
(a multiply-by-zero the decayed form drops), so the NATIVE object changes at every -O level, though
the physics at -O0 is unchanged. Every other pass in this repository is an i386 no-op and the
acceptance test enforces that; this one carves itself out (it writes a manifest the acceptance
reads). What it MUST preserve is the WEB: the wasm32/clang object stays byte-identical (measured per
file below, the mandatory gate), so the shipped wasm does not move. The native change is validated
BEHAVIOURALLY instead — ufront's ktrace gate must still read c31ed77b7323 after this lands. That
re-validation is the price the owner accepted for lifting the UB (ufront 2.58 addendum).

⚠ SCOPE: BARE-NAME LOCALS ONLY. The base of shape A/B must be a plain identifier, i.e. a local
array declared in the file (`m1`, `m2`, `GEt`, `qmul`). A struct-member base (`p->tm[0][j]`) is
NOT handled — its extent lives in a header this pass does not parse reliably, and the measured
defect is entirely local arrays. Shape C `(*P)[j]` covers the row-pointer spelling Ghidra also
emits.

WHAT COUNTS AS A SITE, deliberately narrow:
  * shape A/B `NAME[0][j]`: NAME a bare identifier declared `T NAME[N][M]` in the file; the OUTER
    index a literal 0; the inner index NON-CONSTANT or a constant >= M (a constant inside the row
    is a real 2-D access, not the walk, and is left alone);
  * shape C `(*P)[j]`: P declared `T (*P)[M]` in the file, all declarations agreeing on T; j
    non-constant. M is irrelevant — the decay is the same address for any M;
  * a non-zero outer index is a genuine 2-D row (folding it flat needs an `i*M+` term and is not
    the row-0 walk); it is LEFT ALONE and reported.

⚠ RUN IT ON A COPY: it edits in place.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kd_paths                                             # noqa: E402

HERE = kd_paths.MD
WORK = '/tmp/kd_flatidx'
MANIFEST = '.flattened_index_repaired'      # written beside the allobj dir; read by lp64_pipeline

EMCC = os.path.join(os.path.expanduser('~'), 'emsdk', 'upstream', 'emscripten', 'emcc')

# shape A/B:  NAME[i][j]  — NAME a BARE identifier; j bracket-free
SITE = re.compile(
    r'(?<![\w\]\)\.>])(?P<base>\w+)'
    r'\[(?P<i>\d+|0x[0-9a-fA-F]+)\]'
    r'\[(?P<j>[^\[\]\n;]+?)\](?!\s*\[)')
# shape C:     (*P)[j]
ROWPTR = re.compile(r'\(\*(?P<p>\w+)\)\[(?P<j>[^\[\]\n;]+?)\](?!\s*\[)')
CONST = re.compile(r'^\s*(?:0x[0-9a-fA-F]+|\d+)\s*$')
_KW = {'if', 'else', 'for', 'while', 'do', 'switch', 'case', 'return', 'goto', 'sizeof',
       'break', 'continue', 'default', 'typedef', 'struct', 'union', 'enum'}


def decl_2d(name, text):
    """(T, N, M) for every LOCAL declaration `T NAME[N][M]` in the file; T never a keyword."""
    rx = re.compile(r'\b(?P<t>(?:const\s+)?(?:unsigned\s+|signed\s+)?\w+)\s+' + re.escape(name)
                    + r'\s*\[\s*(?P<n>\d+)\s*\]\s*\[\s*(?P<m>\d+)\s*\]\s*[;,=)]')
    out = []
    for m in rx.finditer(text):
        t = m.group('t')
        if t.split()[-1] in _KW:
            continue
        out.append((t, int(m.group('n')), int(m.group('m'))))
    return out


def rowptr_types(name, text):
    """element type T for every local row pointer `T (*NAME)[M];` (M is irrelevant to the decay)."""
    rx = re.compile(r'\b(?P<t>(?:const\s+)?\w+)\s*\(\s*\*\s*' + re.escape(name)
                    + r'\s*\)\s*\[\s*\d+\s*\]\s*[;,=)]')
    return {m.group('t') for m in rx.finditer(text) if m.group('t').split()[-1] not in _KW}


def includes(inc):
    out = ['-I' + inc]
    for d in ('McdCommon', 'McdPrimitives', 'McdFrame', 'MeGlobals',
              'MdtBcl', 'MdtKea', 'Mst', 'MeApp'):
        out.append('-I' + os.path.join(inc, d))
    return out


def _obj(fn, text, inc, cc, opt):
    """Compile `text` (named fn) and return its bytes, or None on failure."""
    d = os.path.join(WORK, cc + opt.replace('-', ''))
    os.makedirs(d, exist_ok=True)
    src = os.path.join(d, fn)
    open(src, 'w').write(text)
    obj = os.path.join(d, fn[:-2] + '.o')
    common = ['-c', opt, '-std=gnu99', '-fno-strict-aliasing', '-w',
              '-Wno-int-conversion', '-Wno-incompatible-pointer-types', '-DLINUX',
              '-I' + os.path.join(HERE, 'include')] + includes(inc) + ['-o', obj, src]
    if cc == 'gcc':
        cmd = ['gcc', '-m32', '-fno-pic'] + common
    else:
        if not os.path.exists(EMCC):
            return None
        cmd = [EMCC, '-g0'] + common
    if subprocess.run(cmd, capture_output=True).returncode:
        return None
    return open(obj, 'rb').read()


def rewrite(text, notes, fn):
    """Return (new_text, n_fixed, n_declined, n_skipped, n_in_bounds)."""
    out, n_fix, n_dec, n_skip, n_ok = [], 0, 0, 0, 0
    pos = 0
    # ---- shapes A and B: the flattened ROW-0 walk on a bare-name local
    for m in SITE.finditer(text):
        base, i_txt, j = m.group('base'), m.group('i'), m.group('j').strip()
        decls = decl_2d(base, text)
        if not decls:
            continue                              # not a declared 2-D local: not this shape
        types = {(t, mm) for t, _, mm in decls}
        if len(types) != 1:
            n_dec += 1
            notes.append('%-30s %-16s [%s][%s] DECLINED: %d disagreeing declarations %s'
                         % (fn, base, i_txt, j, len(decls), sorted(types)))
            continue
        (t, mcol), = types
        if int(i_txt, 0) != 0:
            n_skip += 1
            notes.append('%-30s %-16s [%s][%s] left alone: non-zero outer index (genuine 2-D row)'
                         % (fn, base, i_txt, j))
            continue
        if CONST.match(j) and int(j, 0) < mcol:
            n_ok += 1
            continue
        repl = '((%s *)%s)[%s]' % (t, base, j)
        out.append(text[pos:m.start()]); out.append(repl); pos = m.end()
        n_fix += 1
        notes.append('%-30s %-16s [0][%s] -> %s' % (fn, base, j, repl))
    out.append(text[pos:])
    text2 = ''.join(out)
    # ---- shape C, on the result
    out, pos = [], 0
    for m in ROWPTR.finditer(text2):
        p, j = m.group('p'), m.group('j').strip()
        types = rowptr_types(p, text2)
        if not types:
            continue
        if len(types) != 1:
            n_dec += 1
            notes.append('%-30s (*%s)[%s] DECLINED: disagreeing element types %s'
                         % (fn, p, j, sorted(types)))
            continue
        (t,) = tuple(types)
        if CONST.match(j):
            n_ok += 1
            continue
        repl = '((%s *)*%s)[%s]' % (t, p, j)
        out.append(text2[pos:m.start()]); out.append(repl); pos = m.end()
        n_fix += 1
        notes.append('%-30s (*%s)[%s] -> %s' % (fn, p, j, repl))
    out.append(text2[pos:])
    return ''.join(out), n_fix, n_dec, n_skip, n_ok


def self_check():
    synth = ('MeReal m1 [3] [3];\n  MeReal (*prow) [4];\n'
             '  m1[0][iVar9 + 1] = 2.0;\n  m1[1][iVar7] = 1.0;\n  m1[0][2] = 0;\n'
             '  } else {\n  x = (*prow)[local_60 + k];\n  y = (*prow)[1];\n')
    got, n_fix, n_dec, n_skip, n_ok = rewrite(synth, [], 'self-check.c')
    for w in ('((MeReal *)m1)[iVar9 + 1]', '((MeReal *)*prow)[local_60 + k]'):
        if w not in got:
            sys.exit('fix_flattened_index: SELF-CHECK FAILED — expected %r in\n%s' % (w, got))
    for w in ('m1[1][iVar7]', 'm1[0][2]', '(*prow)[1]', 'else'):
        if w not in got:
            sys.exit('fix_flattened_index: SELF-CHECK FAILED — %r should be untouched\n%s' % (w, got))
    if (n_fix, n_dec, n_skip, n_ok) != (2, 0, 1, 2):
        sys.exit('fix_flattened_index: SELF-CHECK FAILED — counts %r, want (2, 0, 1, 2)'
                 % ((n_fix, n_dec, n_skip, n_ok),))


def main():
    srcdir, build = sys.argv[1], sys.argv[2]
    root = sys.argv[3] if len(sys.argv) > 3 else kd_paths.METOOLKIT_DIR
    inc = os.path.join(root, 'include')
    self_check()
    if not os.path.exists(EMCC):
        sys.exit('fix_flattened_index: emcc not found at %s — the wasm gate (the web is what this '
                 'pass must not change) cannot run.' % EMCC)

    fixed = declined = skipped = in_bounds = 0
    repaired_files = []
    notes = []
    for fn in sorted(os.listdir(srcdir)):
        if not fn.endswith('.c') or not os.path.exists(os.path.join(build, fn[:-2] + '.o')):
            continue
        path = os.path.join(srcdir, fn)
        text = open(path, errors='ignore').read()
        cand, n_fix, n_dec, n_skip, n_ok = rewrite(text, notes, fn)
        declined += n_dec
        skipped += n_skip
        in_bounds += n_ok
        if n_fix == 0:
            continue
        # ---- MANDATORY: the wasm32 object must not move (the web ships it, byte for byte).
        raw_w, rep_w = _obj(fn, text, inc, 'em', '-O3'), _obj(fn, cand, inc, 'em', '-O3')
        if raw_w is None or rep_w is None or raw_w != rep_w:
            declined += n_fix
            notes.append('%-30s DECLINED: %d rewrite(s) would change the wasm32 object — the web '
                         'ships it' % (fn, n_fix))
            continue
        # ---- the native object is EXPECTED to change; it only has to compile at both -O levels.
        if _obj(fn, cand, inc, 'gcc', '-O0') is None or _obj(fn, cand, inc, 'gcc', '-O2') is None:
            declined += n_fix
            notes.append('%-30s DECLINED: %d rewrite(s) do not compile at -m32 -O0/-O2'
                         % (fn, n_fix))
            continue
        open(path, 'w').write(cand)
        fixed += n_fix
        repaired_files.append(fn)
        notes.append('%-30s APPLIED: %d row-0 site(s) — wasm byte-identical, native object changes '
                     '(re-gated by ktrace)' % (fn, n_fix))

    # the manifest the i386 acceptance reads to know which files legitimately differ
    mpath = os.path.join(os.path.dirname(srcdir.rstrip('/')), MANIFEST)
    open(mpath, 'w').write('\n'.join(repaired_files) + ('\n' if repaired_files else ''))

    for t in notes:
        print('  ' + t)
    print('fix_flattened_index: %d row-0 site(s) respelled in %d file(s) (wasm byte-identical; '
          'native object changes, ktrace re-validates), %d declined, %d non-zero-outer left alone, '
          '%d in-bounds constants left alone'
          % (fixed, len(repaired_files), declined, skipped, in_bounds))
    print('  manifest -> %s (%s)' % (mpath, ', '.join(repaired_files) or 'none'))


if __name__ == '__main__':
    main()
