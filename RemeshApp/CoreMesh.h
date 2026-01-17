#pragma once

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>

class CpuMeshBuffer;

class CoreMesh
{
public:
    using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Point = Kernel::Point_3;
    using SMesh = CGAL::Surface_mesh<Point>;

    SMesh sm;

    void clear() { sm.clear(); }
    bool empty() const { return sm.is_empty(); }

    bool sanitizeAndTriangulate();

    bool fromCpuMeshBuffer(const CpuMeshBuffer& cpu);
    CpuMeshBuffer toCpuMeshBuffer(bool computeNormals = true) const;
};
