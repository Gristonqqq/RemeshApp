#include "CoreMesh.h"
#include "CpuMeshBuffer.h"

#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <unordered_map>

namespace PMP = CGAL::Polygon_mesh_processing;

namespace
{
    template <typename IndexT>
    struct CgalIndexHash
    {
        std::size_t operator()(const IndexT& i) const noexcept
        {
            return static_cast<std::size_t>(i);
        }
    };
}

// sanitize mesh: remove isolated vertices, degenerate faces, triangulate faces
bool CoreMesh::sanitizeAndTriangulate()
{
    if (sm.is_empty()) return false;

    PMP::remove_isolated_vertices(sm);

    PMP::remove_degenerate_faces(sm);

    PMP::triangulate_faces(sm);

    PMP::remove_isolated_vertices(sm);

    return !sm.is_empty() && sm.number_of_faces() > 0;
}

// load from CpuMeshBuffer
bool CoreMesh::fromCpuMeshBuffer(const CpuMeshBuffer& cpu)
{
    sm.clear();

    if (cpu.positions.empty() || cpu.indices.empty()) return false;
    if (cpu.indices.size() % 3 != 0) return false;

    std::vector<SMesh::Vertex_index> vmap;
    vmap.reserve(cpu.positions.size());

    for (const auto& p : cpu.positions)
        vmap.push_back(sm.add_vertex(Point(p.x(), p.y(), p.z())));

    for (size_t i = 0; i < cpu.indices.size(); i += 3)
    {
        const uint32_t ia = cpu.indices[i + 0];
        const uint32_t ib = cpu.indices[i + 1];
        const uint32_t ic = cpu.indices[i + 2];

        if (ia >= vmap.size() || ib >= vmap.size() || ic >= vmap.size())
            continue;

        const auto f = sm.add_face(vmap[ia], vmap[ib], vmap[ic]);
        (void)f;
    }

    return sanitizeAndTriangulate();
}

// export to CpuMeshBuffer
CpuMeshBuffer CoreMesh::toCpuMeshBuffer(bool computeNormals) const
{
    CpuMeshBuffer out;
    if (sm.is_empty()) return out;

    std::unordered_map<SMesh::Vertex_index, uint32_t, CgalIndexHash<SMesh::Vertex_index>> vid;
    vid.reserve(static_cast<size_t>(sm.number_of_vertices()));

    out.positions.reserve(static_cast<size_t>(sm.number_of_vertices()));

    uint32_t idx = 0;
    for (auto v : sm.vertices())
    {
        const auto& p = sm.point(v);
        out.positions.push_back(QVector3D((float)p.x(), (float)p.y(), (float)p.z()));
        vid[v] = idx++;
    }

    out.indices.reserve(static_cast<size_t>(sm.number_of_faces()) * 3);

    for (auto f : sm.faces())
    {
        auto h = sm.halfedge(f);
        if (h == SMesh::null_halfedge()) continue;

        auto v0 = sm.target(h); h = sm.next(h);
        auto v1 = sm.target(h); h = sm.next(h);
        auto v2 = sm.target(h);

        out.indices.push_back(vid[v0]);
        out.indices.push_back(vid[v1]);
        out.indices.push_back(vid[v2]);
    }

    if (computeNormals)
        out.recalculateNormals();

    return out;
}
