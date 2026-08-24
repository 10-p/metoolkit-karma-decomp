#!/usr/bin/env python3
"""
gen_typedb.py — build one C header of all Karma-internal types, by unioning the
DWARF across every object in the SDK.

Why a union is necessary: gcc 3.2 emits a class's full layout only in the
compilation unit that defines it. Every other CU that merely *uses* the class
carries a declaration-only DIE — same name, no members, and often a bogus
byte_size. `keaMatrix` appears as 4 bytes in keaMatrix_tester.o and as its real
20 bytes in keaMatrix.o. Reading any single object therefore gives you a partly
wrong picture; reading all of them and keeping the richest definition gives you
the right one.

Types already declared in metoolkit's public headers are skipped, since the
recovered sources include those headers directly and redefining them is an error.

  ./gen_typedb.py <objdir>... --public-headers ../Thirdparty/metoolkit/include \
                  -o include/kd_types.h
"""
import argparse
import glob
import os
import re
import sys

OFF_RE_TD = re.compile(r'DW_OP_plus_uconst:\s*(\d+)')

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dwarf_structs import (parse, emit, emit_enum, declarator,  # noqa: E402
                           rtti_bases, REF_RE, SYSTEM_TYPES)


def public_type_names(include_dir):
    """Struct/union/class names that metoolkit's own headers already define."""
    names, tdnames = set(), set()
    if not include_dir:
        return names, tdnames
    # `enum` included: metoolkit declares plenty of them, and omitting it here
    # means we redefine every public enum and nothing compiles at all.
    pat = re.compile(r'\b(?:struct|union|class|enum)\s+([A-Za-z_]\w*)\s*\{')
    # Deliberately does NOT require the trailing `;`: metoolkit writes
    #     } MdtKeaTransformation
    #     #ifdef PS2
    #     __attribute__((aligned(16)))
    #     #endif
    #     ;
    # so anything anchored on `;` misses it, and we then redefine the typedef
    # and collide. Over-detecting is safe here — the cost is only that we skip
    # emitting a type the public headers already provide.
    tpat = re.compile(r'\}\s*\**\s*([A-Za-z_]\w*)\b')
    # Every typedef in a public header, struct-based or not: `typedef MeReal
    # MeMatrix4[4][4];` must be recognised too, or we redefine it and clash.
    anytd = re.compile(r'\btypedef\b[^;{}]*?([A-Za-z_]\w*)\s*(?:\[[^\]]*\]\s*)*;')
    # The TAG in `typedef struct _MeSet { ... } MeSet;` — without this we only
    # learn about `MeSet` and go on to redefine `_MeSet`, which the header
    # already declares.
    tagdef = re.compile(r'\btypedef\s+(?:struct|union|enum)\s+([A-Za-z_]\w*)\s*\{')
    for root, _, files in os.walk(include_dir):
        for f in files:
            if not f.endswith('.h'):
                continue
            try:
                txt = open(os.path.join(root, f), errors='ignore').read()
            except OSError:
                continue
            names.update(pat.findall(txt))
            names.update(tpat.findall(txt))
            names.update(anytd.findall(txt))
            names.update(tagdef.findall(txt))
            tdnames.update(tpat.findall(txt))
            tdnames.update(anytd.findall(txt))
    return names, tdnames


def value_deps(dies, die):
    """Names of struct/union types this one embeds BY VALUE (or as an array of).

    Pointer members need only a forward declaration, so they are not deps —
    which is what keeps the graph acyclic even though Karma's types point at
    each other freely.

    A BASE CLASS counts. dwarf_structs embeds it as a `super_<Base>` member, so
    it is every bit as by-value as a named field, and leaving it out of the
    graph emitted CxSmallSortMarker above Link and took the whole build to zero
    — the failure mode §4 warns about, reproduced exactly."""
    deps = set()
    for c in die['children']:
        if c['tag'] not in ('DW_TAG_member', 'DW_TAG_inheritance'):
            continue
        t = c['attrs'].get('DW_AT_type')
        if not t:
            continue
        m = REF_RE.search(t)
        if not m:
            continue
        ref, hops = int(m.group(1), 16), 0
        while ref is not None and hops < 12:
            hops += 1
            d = dies.get(ref)
            if d is None:
                break
            if d['tag'] == 'DW_TAG_pointer_type':
                break                       # forward decl suffices
            if d['tag'] in ('DW_TAG_structure_type', 'DW_TAG_class_type',
                            'DW_TAG_union_type'):
                n = d['attrs'].get('DW_AT_name')
                if n:
                    deps.add(n)
                break
            sub = d['attrs'].get('DW_AT_type')   # typedef/const/array/volatile
            sm = REF_RE.search(sub) if sub else None
            ref = int(sm.group(1), 16) if sm else None
    return deps


def referenced_typedefs(dies, die, public, acc):
    """Collect typedef DIEs a struct's members depend on.

    Members are spelled with whatever name DWARF gives them, so a typedef that
    metoolkit's public headers do NOT declare (an internal one like
    McdUpdateAABBFnPtr) has to be emitted by us or the header won't compile.
    Struct-aliasing typedefs are already resolved to `struct Tag` by
    dwarf_structs.type_name, so they never reach here."""
    for c in die['children']:
        if c['tag'] != 'DW_TAG_member':
            continue
        t = c['attrs'].get('DW_AT_type')
        m = REF_RE.search(t) if t else None
        ref, hops = (int(m.group(1), 16) if m else None), 0
        while ref is not None and hops < 12:
            hops += 1
            d = dies.get(ref)
            if d is None:
                break
            if d['tag'] == 'DW_TAG_typedef':
                n = d['attrs'].get('DW_AT_name')
                if n and n not in public and n not in SYSTEM_TYPES and n not in acc:
                    acc[n] = (d, dies)
                break
            nxt = d['attrs'].get('DW_AT_type')
            nm2 = REF_RE.search(nxt) if nxt else None
            ref = int(nm2.group(1), 16) if nm2 else None


def toposort(names, depmap):
    """Emit order: a type appears after everything it embeds by value."""
    out, state = [], {}
    def visit(n):
        st = state.get(n)
        if st == 2:
            return
        if st == 1:                          # cycle: only possible via a bug
            return
        state[n] = 1
        for d in sorted(depmap.get(n, ())):
            if d in depmap:
                visit(d)
        state[n] = 2
        out.append(n)
    for n in sorted(names):
        visit(n)
    return out


def richness(die):
    """How complete is this DIE? More members wins; ties broken by byte_size."""
    n = sum(1 for c in die['children'] if c['tag'] == 'DW_TAG_member')
    try:
        size = int(die['attrs'].get('DW_AT_byte_size', 0))
    except ValueError:
        size = 0
    return (n, size)


def name_anonymous_typedef_targets(dies, public):
    """Give `typedef struct { ... } Foo;` its name back.

    gcc emits that as an UNNAMED DW_TAG_structure_type plus a DW_TAG_typedef
    that points at it, so the scan below — which keys on DW_AT_name of the
    aggregate — skipped the type entirely and the recovered source failed with
    `unknown type name`. BodyData in MstModelDynamics.o (libMst, which IS
    linked), weightingData in MeProfile.o and Mesh2GeometryType in
    MeFGeometryFromMesh.o are all this shape.

    Naming the target in place is enough: everything downstream — richness,
    dependency ordering, emit() — reads the name from the same attribute.

    First typedef wins. Two typedefs for one anonymous aggregate are aliases of
    each other, and picking either gives the right layout; picking arbitrarily
    is better than emitting the same struct twice under two names, which would
    make every pointer between them a different type.

    PUBLIC names are left anonymous, and skipping that guard takes the build to
    ZERO. metoolkit's headers use this same idiom — MeProfile.h has
    `typedef struct { ... } MeProfileTimerResult;` — so naming the target makes
    declarator() spell a FIELD of some other type as
    `struct MeProfileTimerResult`, and that tag does not exist because the
    header's struct is anonymous too. The failure is nowhere near the type that
    was named, which is the third time a kd_types.h change has failed globally
    for a reason local to one line."""
    for die in dies.values():
        if die['tag'] != 'DW_TAG_typedef':
            continue
        tname = die['attrs'].get('DW_AT_name')
        ref = die['attrs'].get('DW_AT_type')
        if not tname or not ref:
            continue
        m = REF_RE.search(ref)
        if not m:
            continue
        target = dies.get(int(m.group(1), 16))
        if target is None:
            continue
        if target['tag'] not in ('DW_TAG_structure_type', 'DW_TAG_class_type',
                                 'DW_TAG_union_type'):
            continue
        if target['attrs'].get('DW_AT_name'):
            continue                                   # already named
        if not any(c['tag'] == 'DW_TAG_member' for c in target['children']):
            continue                                   # no layout to recover
        if tname in public:
            continue                                   # see the docstring
        target['attrs']['DW_AT_name'] = tname


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('objdirs', nargs='+', help='directories of extracted .o files')
    ap.add_argument('--public-headers', help='metoolkit include/ root — types here are skipped')
    ap.add_argument('-o', '--output', required=True)
    ap.add_argument('--quiet', action='store_true')
    ap.add_argument('--exclude', action='append', default=[],
                    help='skip objects whose path contains this substring (repeatable). '
                         'Use for archives that are never linked, so their types '
                         '(and their unresolvable dependencies) stay out of the db.')
    ap.add_argument('--include', action='append', default=[],
                    help='public header the generated types depend on (repeatable)')
    args = ap.parse_args()

    public, typedef_names = public_type_names(args.public_headers)
    best = {}        # name -> (richness, die, dies, source object)  [emitted]
    pub_layout = {}  # same, for PUBLIC types: field map only, never emitted
    enums = {}       # name -> (die, source object)
    conflicts = {}   # name -> set of (nmembers, size) seen

    objs = []
    for d in args.objdirs:
        objs.extend(sorted(glob.glob(os.path.join(d, '**', '*.o'), recursive=True)))
    if args.exclude:
        objs = [o for o in objs if not any(x in o for x in args.exclude)]
    if not objs:
        print('no objects found', file=sys.stderr)
        sys.exit(1)

    for obj in objs:
        try:
            dies = parse(obj)
        except Exception as e:                                  # noqa: BLE001
            print(f'  ! {obj}: {e}', file=sys.stderr)
            continue
        name_anonymous_typedef_targets(dies, public)
        for die in dies.values():
            if die['tag'] == 'DW_TAG_enumeration_type':
                en = die['attrs'].get('DW_AT_name')
                if (en and en not in public and en not in SYSTEM_TYPES
                        and en not in enums
                        and any(c['tag'] == 'DW_TAG_enumerator'
                                for c in die['children'])):
                    enums[en] = (die, obj)
                continue
            if die['tag'] not in ('DW_TAG_structure_type', 'DW_TAG_class_type',
                                  'DW_TAG_union_type'):
                continue
            name = die['attrs'].get('DW_AT_name')
            if not name or name in SYSTEM_TYPES:
                continue
            if not any(c['tag'] == 'DW_TAG_member' for c in die['children']):
                continue                                        # declaration only
            r = richness(die)
            # PUBLIC types are recorded for the field map but never emitted: the
            # header already defines them. Ghidra still writes field_0xNN for
            # them, and mapping an offset back to a name needs the layout
            # regardless of who declares it.
            if name in public:
                if name not in pub_layout or r > pub_layout[name][0]:
                    pub_layout[name] = (r, die, dies, obj)
                continue
            conflicts.setdefault(name, set()).add(r)
            if name not in best or r > best[name][0]:
                best[name] = (r, die, dies, obj)

    # A name seen with two DIFFERENT non-zero member counts is a genuine problem
    # (two distinct types sharing a name), not just decl-vs-def.
    real_conflicts = {n: v for n, v in conflicts.items()
                      if len({m for m, _ in v if m}) > 1}

    out = ['/* kd_types.h — Karma-internal types recovered from DWARF.',
           ' *',
           ' * Generated by tools/gen_typedb.py by unioning .debug_info across every',
           ' * object in the SDK: a class layout is only complete in the CU that',
           ' * DEFINES it, so no single object gives the full picture.',
           ' *',
           ' * Types that metoolkit\'s public headers already define are omitted.',
           ' */',
           '#ifndef KD_TYPES_H',
           '#define KD_TYPES_H',
           '',
           '#include "kd_compat.h"',
           '']
    # The recovered types reference public Karma typedefs (MeReal, MeU8, MeI32,
    # McdFramework, ...). Those come from metoolkit's own headers, which we skip
    # emitting but must include.
    out.append('#include <stdbool.h>')
    for h in args.include:
        out.append(f'#include <{h}>')
    if args.include:
        out.append('')

    inherit_pre = {}
    for _o in objs:
        inherit_pre.update(rtti_bases(_o))
    depmap = {n: value_deps(best[n][2], best[n][1]) for n in best}
    names = toposort(sorted(best), depmap)
    # Ghidra writes a struct tag bare — `(_McdIntersectResult *)result` — where C
    # requires `struct _McdIntersectResult *`. Aliasing every public tag to
    # itself makes both spellings legal, with no edit to the recovered sources.
    tagalias = set()
    if args.public_headers:
        # ONLY _-prefixed tags. Broadening this to every public tag takes the
        # build to zero: `MePoolAPI` is a struct tag AND an ordinary identifier,
        # so `typedef struct MePoolAPI MePoolAPI;` is "redeclared as a different
        # kind of symbol". The _-prefixed names are safe precisely because
        # metoolkit does not reuse them outside the tag namespace.
        #
        # The cost is that headers written in C++ style — McdMessage.h says
        # `extern McdErrorDescription gMcdCoreErrorList[];` with no `struct` —
        # still cannot compile as C, which is why they stay out of the umbrella.
        tagpat = re.compile(r'\b(?:struct|union)\s+(_\w+)')
        for r_, _d, fs in os.walk(args.public_headers):
            for f in fs:
                if not f.endswith('.h'):
                    continue
                try:
                    tagalias.update(tagpat.findall(
                        open(os.path.join(r_, f), errors='ignore').read()))
                except OSError:
                    pass
    if tagalias:
        out.append('/* ---- bare-tag aliases (Ghidra omits the `struct` keyword) ---- */')
        # Skip any tag that is ALREADY a typedef name pointing somewhere else:
        # McdCTypes.h has `typedef struct _McdGeometry McdGeometry;`, so
        # `typedef struct McdGeometry McdGeometry;` would be a conflicting
        # redefinition. Emitting these unconditionally took the build to zero.
        for t in sorted(tagalias - typedef_names):
            out.append(f'typedef struct {t} {t};')
        out.append('')

    out.append('/* ---- forward declarations (so pointer members need no ordering) ---- */')
    for n in sorted(names):
        kw = 'union' if best[n][1]['tag'] == 'DW_TAG_union_type' else 'struct'
        # Both spellings: the tag (so pointer members need no ordering) and the
        # typedef alias, because the recovered sources use the bare name.
        out.append(f'{kw} {n};')
        out.append(f'typedef {kw} {n} {n};')
    out.append('')

    if real_conflicts:
        out.append('/* ---- WARNING: conflicting layouts seen for these names ---- */')
        for n, v in sorted(real_conflicts.items()):
            out.append(f'/*   {n}: {sorted(v)} — richest kept; verify by hand */')
        out.append('')

    # Anonymous aggregate members: Ghidra names their type
    # `anon_union_4_2_<hash>_for_<member>`, which exists nowhere. Emitting
    #     typedef __typeof__(((struct Owner *)0)->member) kd_anon_<member>;
    # gives it a name that is EXACTLY the right type, so declarations, casts and
    # assignments all work without any special-casing downstream.
    anon_owners = {}
    for obj in objs:
        try:
            dies = parse(obj)
        except Exception:                                       # noqa: BLE001
            continue
        for die in dies.values():
            if die['tag'] not in ('DW_TAG_structure_type', 'DW_TAG_class_type'):
                continue
            owner = die['attrs'].get('DW_AT_name')
            if not owner:
                continue
            for c in die['children']:
                if c['tag'] != 'DW_TAG_member':
                    continue
                mn = c['attrs'].get('DW_AT_name')
                mt = c['attrs'].get('DW_AT_type')
                m = REF_RE.search(mt) if mt else None
                if not (mn and m):
                    continue
                d = dies.get(int(m.group(1), 16))
                if (d and d['tag'] in ('DW_TAG_union_type', 'DW_TAG_structure_type')
                        and not d['attrs'].get('DW_AT_name')):
                    anon_owners.setdefault(mn, owner)
    if anon_owners:
        out.append('/* ---- anonymous aggregate members, named exactly ---- */')
        for mn, owner in sorted(anon_owners.items()):
            out.append(f'typedef __typeof__(((struct {owner} *)0)->{mn}) '
                       f'kd_anon_{mn};')
        out.append('')

    aux = {}
    for n in names:
        referenced_typedefs(best[n][2], best[n][1], public, aux)
    if aux:
        out.append('/* ---- internal typedefs the recovered structs depend on ---- */')
        for n, (d, dd) in sorted(aux.items()):
            if n in best:          # already aliased in the forward section
                continue
            sub = d['attrs'].get('DW_AT_type')
            m = REF_RE.search(sub) if sub else None
            ref = int(m.group(1), 16) if m else None
            out.append(f'typedef {declarator(dd, ref, n)};')
        out.append('')

    if enums:
        out.append('/* ---- enums ---- */')
        for n in sorted(enums):
            die, obj = enums[n]
            out.append(f'/* from {os.path.basename(obj)} */')
            out.append(emit_enum(die))
            out.append(f'typedef enum {n} {n};')
            out.append('')

    out.append('/* ---- definitions ---- */')
    for n in names:
        _, die, dies, obj = best[n]
        # A derived class lists only its own members; splice the base in so the
        # offsets line up and the base's fields exist. Inheritance from RTTI.
        bname = inherit_pre.get(n)
        bdie = bdies = None
        if bname and bname in best:
            _, bdie, bdies, _bo = best[bname]
        out.append(f'/* from {os.path.basename(obj)} */')
        out.append(emit(dies, die, bdie, bdies))
        out.append('')

    # ---- C++ classes with no DWARF layout ---------------------------------
    # A derived class that adds no data members gets no member list of its own,
    # and for Karma's "_vanilla" implementation classes the defining CU is not
    # in the SDK at all. The Itanium ABI's typeinfo still records the base, so
    # the inheritance graph is recoverable even when the layout is not.
    inherit = {}
    for obj in objs:
        inherit.update(rtti_bases(obj))
    aliased = [(d, b) for d, b in sorted(inherit.items())
               if d not in best and b in best]
    if aliased:
        out.append('/* ---- polymorphic classes with no DWARF layout ---- */')
        out.append('/*')
        out.append(' * Each of these has a vtable but no member list. Its base is read from')
        out.append(' * RTTI (the _ZTI relocation), and it is aliased to that base on the')
        out.append(' * assumption that it adds no data members — true for an override-only')
        out.append(' * subclass, which is what these are.')
        out.append(' *')
        out.append(' * TO VERIFY: the highest field offset the class\'s own methods touch must')
        out.append(' * be inside the base. If a recovered method reads past sizeof(base), the')
        out.append(' * assumption is wrong for that class and it needs a hand-written layout.')
        out.append(' */')
        for d, b in aliased:
            out.append(f'typedef {b} {d};   /* base from RTTI */')
        out.append('')

    out.append('#endif /* KD_TYPES_H */')
    open(args.output, 'w').write('\n'.join(out) + '\n')

    # Side map: {type: {offset: member}}. Ghidra writes `this->field_0x14` for a
    # class whose DWARF layout it did not apply; with the offsets we can put the
    # real name back. Emitted here because this is where the offsets are known.
    import json
    fieldmap = {}
    for n, src in list((k, best) for k in names) + \
                  list((k, pub_layout) for k in pub_layout):
        _, die, dies, _o = src[n]
        bname = inherit_pre.get(n)
        kids = list(die['children'])
        if bname and bname in best:
            kids = list(best[bname][1]['children']) + kids
        m = {}
        for c in kids:
            if c['tag'] != 'DW_TAG_member':
                continue
            mn = c['attrs'].get('DW_AT_name')
            mo = OFF_RE_TD.search(c['attrs'].get('DW_AT_data_member_location', ''))
            if mn and mo:
                m[int(mo.group(1))] = ('vptr' if mn.startswith('_vptr')
                                       else re.sub(r'[^A-Za-z0-9_]', '_', mn))
        if m:
            fieldmap[n] = m
    mp = os.path.splitext(args.output)[0] + '_fields.json'
    json.dump(fieldmap, open(mp, 'w'), indent=1, sort_keys=True)
    print(f'{mp}: field offsets for {len(fieldmap)} type(s)')

    if not args.quiet:
        print(f'{args.output}: {len(names)} types + {len(enums)} enums from '
              f'{len(objs)} objects ({len(public)} skipped as public)')
        if real_conflicts:
            print(f'  {len(real_conflicts)} name(s) with conflicting layouts — see header')


if __name__ == '__main__':
    main()
