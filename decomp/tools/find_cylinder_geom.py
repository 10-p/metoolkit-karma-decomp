#!/usr/bin/env python3
"""Which shipped assets define a Karma CYLINDER collision element?

HANDOVER.md §3 has one open map question: `Cylinder`. `KUtils.cpp:796` really
does call `McdCylinderCreate` from a cylinder collision element, so the code
path exists, and the aggregate dispatcher in `KFarfield.cpp` recurses into
`KIntersect` per element — so a cylinder inside an aggregate DOES reach Karma's
interaction table, unlike the aggregate pair itself (§3a). The census has just
never seen a shipped asset define one. Answering it retires five registered
pairs permanently.

GREP CANNOT ANSWER IT, and that is the first thing to know. `FKAggregateGeom`
is serialised by a hand-written `operator<<` (Engine/Inc/KTypes.h:195) that
writes four TArrays in a fixed order and no field names, so "CylinderElems"
never appears in a package. Searching for it returns zero on every package in
the tree, and so does "BoxElems" — an absence that means nothing.

What IS in the file is the class name `KMeshProps`, because the export table
names an object's class. So the geometry has to be parsed.

Format, all read from the engine's own serialisers rather than guessed:
  FPackageFileSummary   Core/Src/UnLinker.cpp:116
  FObjectExport         Core/Src/UnLinker.cpp:48
  FObjectImport         Core/Src/UnLinker.cpp:86
  FNameEntry            Core/Src/UnName.cpp:236
  FCompactIndex         Core/Src/UnObj.cpp:5676
  UKMeshProps::Serialize / FKAggregateGeom / FK*Elem   Engine/Inc/KTypes.h

The one thing not written down anywhere is how many bytes `UObject::Serialize`
puts in front of the payload. Rather than assume, every plausible start offset
is tried and one is accepted ONLY if the parse consumes the export's
SerialSize EXACTLY — a whole-blob check, so a wrong offset does not survive.
An export whose parse is ambiguous or fails is reported, not silently dropped.
"""
import os, struct, sys, glob

PACKAGE_FILE_TAG = 0x9E2A83C1


class Reader:
    def __init__(self, buf, pos=0):
        self.b, self.p = buf, pos

    def u8(self):
        v = self.b[self.p]; self.p += 1; return v

    def i32(self):
        v = struct.unpack_from('<i', self.b, self.p)[0]; self.p += 4; return v

    def f32(self):
        v = struct.unpack_from('<f', self.b, self.p)[0]; self.p += 4; return v

    def index(self):
        """FCompactIndex — UnObj.cpp:5676."""
        b0 = self.u8()
        value = b0 & 0x3F
        if b0 & 0x40:
            shift = 6
            while True:
                bn = self.u8()
                value |= (bn & 0x7F) << shift
                shift += 7
                if not (bn & 0x80) or shift > 28:
                    break
        return -value if (b0 & 0x80) else value

    def fstring(self):
        n = self.index()
        if n >= 0:
            s = self.b[self.p:self.p + n]; self.p += n
            return s.split(b'\0')[0].decode('latin-1')
        s = self.b[self.p:self.p - 2 * n]; self.p += -2 * n
        return s.decode('utf-16-le', 'replace').split('\0')[0]


def read_package(path):
    buf = open(path, 'rb').read()
    r = Reader(buf)
    if r.i32() != struct.unpack('<i', struct.pack('<I', PACKAGE_FILE_TAG))[0]:
        return None
    ver_lic = r.i32()
    version = ver_lic & 0xFFFF
    r.i32()                                     # PackageFlags
    name_count, name_off = r.i32(), r.i32()
    export_count, export_off = r.i32(), r.i32()
    import_count, import_off = r.i32(), r.i32()

    names = []
    r.p = name_off
    for _ in range(name_count):
        if version < 64:
            e = buf.index(b'\0', r.p)
            names.append(buf[r.p:e].decode('latin-1')); r.p = e + 1
        else:
            names.append(r.fstring())
        r.i32()                                 # Flags
    imports = []
    r.p = import_off
    for _ in range(import_count):
        r.index(); r.index()                    # ClassPackage, ClassName
        r.i32()                                 # PackageIndex
        imports.append(r.index())               # ObjectName
    exports = []
    r.p = export_off
    for _ in range(export_count):
        cls = r.index(); r.index(); r.i32()
        nm = r.index(); r.i32()
        size = r.index()
        off = r.index() if size else 0
        exports.append((cls, nm, size, off))
    return buf, names, imports, exports


def class_name(cls, names, imports, exports):
    if cls < 0:
        i = -cls - 1
        return names[imports[i]] if i < len(imports) else '?'
    if cls > 0:
        i = cls - 1
        return names[exports[i][1]] if i < len(exports) else '?'
    return 'Class'


def parse_meshprops(blob, start):
    """UKMeshProps::Serialize's payload. Returns counts, or None if it does not
    consume the blob exactly — which is the whole point of trying offsets."""
    r = Reader(blob, start)
    try:
        for _ in range(6):
            r.f32()                             # InertiaTensor[6]
        for _ in range(3):
            r.f32()                             # COMOffset
        counts = {}
        for key, elem in (('sphere', 68), ('box', 76), ('cylinder', 72)):
            n = r.index()
            if n < 0 or n > 4096:
                return None
            counts[key] = n
            r.p += n * elem
            if r.p > len(blob):
                return None
        n = r.index()                           # ConvexElems, variable size
        if n < 0 or n > 4096:
            return None
        counts['convex'] = n
        for _ in range(n):
            r.p += 64                           # FMatrix TM
            v = r.index()                       # TArray<FVector> VertexData
            if v < 0 or v > 1 << 20:
                return None
            r.p += v * 12
            if r.p > len(blob):
                return None
    except (IndexError, struct.error):
        return None
    return counts if r.p == len(blob) else None


def scan(path):
    try:
        pk = read_package(path)
    except Exception as e:
        return [('ERROR', str(e), None)]
    if pk is None:
        return []
    buf, names, imports, exports = pk
    out = []
    for cls, nm, size, off in exports:
        if class_name(cls, names, imports, exports) != 'KMeshProps' or not size:
            continue
        blob = buf[off:off + size]
        hits = [(s, c) for s in range(0, min(64, size))
                for c in [parse_meshprops(blob, s)] if c]
        obj = names[nm] if nm < len(names) else '?'
        if len(hits) == 1:
            out.append(('OK', obj, hits[0][1]))
        elif not hits:
            out.append(('UNPARSED', obj, None))
        else:
            # More than one offset consumed the blob exactly. Only trust it if
            # every reading agrees about the cylinders, which is the question.
            cyl = {c['cylinder'] for _s, c in hits}
            out.append(('OK' if len(cyl) == 1 else 'AMBIGUOUS', obj,
                        hits[0][1] if len(cyl) == 1 else None))
    return out


def main():
    roots = sys.argv[1:] or ['/home/ion/ut2004-assets']
    files = []
    for root in roots:
        if os.path.isfile(root):
            files.append(root)
        else:
            for ext in ('usx', 'ukx', 'ut2', 'u', 'uax', 'utx'):
                files += glob.glob(os.path.join(root, '**', '*.' + ext),
                                   recursive=True)
    tot = {'sphere': 0, 'box': 0, 'cylinder': 0, 'convex': 0}
    n_ok = n_bad = n_amb = 0
    cyl_files = []
    for f in sorted(files):
        for status, obj, counts in scan(f):
            if status == 'OK':
                n_ok += 1
                for k in tot:
                    tot[k] += counts[k]
                if counts['cylinder']:
                    cyl_files.append((f, obj, counts))
            elif status == 'AMBIGUOUS':
                n_amb += 1
            elif status == 'UNPARSED':
                n_bad += 1
    print(f'packages scanned          : {len(files)}')
    print(f'KMeshProps objects parsed : {n_ok}')
    print(f'  unparsed                : {n_bad}')
    print(f'  ambiguous on cylinders  : {n_amb}')
    print('\nelements found across every shipped asset:')
    for k in ('sphere', 'box', 'cylinder', 'convex'):
        print(f'  {k:9}: {tot[k]}')
    if cyl_files:
        print('\nCYLINDER collision geometry:')
        for f, obj, c in cyl_files:
            print(f'  {f}  {obj}  {c}')
    else:
        print('\nNo cylinder collision element in any parsed asset.')


if __name__ == '__main__':
    main()
