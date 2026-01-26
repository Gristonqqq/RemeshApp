#include "SubdivideStrategy.h"

#include <unordered_map>
#include <vector>
#include <cmath>

// Helper to set error message
static void setError(QString* out, const QString& msg)
{
    if (out) *out = msg;
}

// Edge key for midpoint map
struct EdgeKey
{
    std::size_t a, b;
    bool operator==(const EdgeKey& o) const noexcept { return a == o.a && b == o.b; }
};

// Hash for EdgeKey
struct EdgeKeyHash
{
    std::size_t operator()(const EdgeKey& k) const noexcept
    {
        return (k.a * 1315423911u) ^ (k.b + 0x9e3779b97f4a7c15ull + (k.a << 6) + (k.a >> 2));
    }
};

// Compute midpoint between two points
static CoreMesh::Point midpoint(const CoreMesh::Point& p0, const CoreMesh::Point& p1)
{
    return CoreMesh::Point(
        (p0.x() + p1.x()) * 0.5,
        (p0.y() + p1.y()) * 0.5,
        (p0.z() + p1.z()) * 0.5
    );
}

// Subdivide mesh once using midpoint subdivision
bool SubdivideStrategy::subdivideOnce(CoreMesh& core, QString* outError)
{
    if (core.empty()) {
        setError(outError, "Core mesh is empty.");
        return false;
    }

    core.sanitizeAndTriangulate();

    std::vector<CoreMesh::Point> pts;
    pts.reserve((size_t)core.sm.number_of_vertices());

    std::unordered_map<CoreMesh::SMesh::Vertex_index, std::size_t> vid;
    vid.reserve((size_t)core.sm.number_of_vertices());

    std::size_t id = 0;
    for (auto v : core.sm.vertices())
    {
        vid[v] = id++;
        pts.push_back(core.sm.point(v));
    }

    std::unordered_map<EdgeKey, std::size_t, EdgeKeyHash> midId;
    midId.reserve((size_t)core.sm.number_of_edges());

    auto getMid = [&](std::size_t i0, std::size_t i1) -> std::size_t
        {
            EdgeKey k{ std::min(i0, i1), std::max(i0, i1) };
            auto it = midId.find(k);
            if (it != midId.end())
                return it->second;

            const CoreMesh::Point m = midpoint(pts[k.a], pts[k.b]);
            const std::size_t newId = pts.size();
            pts.push_back(m);
            midId.emplace(k, newId);
            return newId;
        };

    struct Tri { std::size_t a, b, c; };
    std::vector<Tri> tris;
    tris.reserve((size_t)core.sm.number_of_faces() * 4);

    for (auto f : core.sm.faces())
    {
        auto h = core.sm.halfedge(f);
        if (h == CoreMesh::SMesh::null_halfedge()) continue;

        auto v0 = core.sm.target(h); h = core.sm.next(h);
        auto v1 = core.sm.target(h); h = core.sm.next(h);
        auto v2 = core.sm.target(h);

        const std::size_t i0 = vid[v0];
        const std::size_t i1 = vid[v1];
        const std::size_t i2 = vid[v2];

        const std::size_t m01 = getMid(i0, i1);
        const std::size_t m12 = getMid(i1, i2);
        const std::size_t m20 = getMid(i2, i0);

        tris.push_back({ i0,  m01, m20 });
        tris.push_back({ i1,  m12, m01 });
        tris.push_back({ i2,  m20, m12 });
        tris.push_back({ m01, m12, m20 });
    }

    if (tris.empty()) {
        setError(outError, "No faces to subdivide.");
        return false;
    }

    CoreMesh::SMesh newSm;
    std::vector<CoreMesh::SMesh::Vertex_index> vmap;
    vmap.reserve(pts.size());

    for (const auto& p : pts)
        vmap.push_back(newSm.add_vertex(p));

    for (const auto& t : tris)
        newSm.add_face(vmap[t.a], vmap[t.b], vmap[t.c]);

    core.sm = std::move(newSm);
    core.sanitizeAndTriangulate();
    return true;
}

// Subdivide mesh multiple times
bool SubdivideStrategy::run(CoreMesh& core, const RemeshParams& p, QString* outError)
{
    const int levels = std::max(1, p.subdivLevels);

    for (int i = 0; i < levels; ++i)
    {
        if (!subdivideOnce(core, outError))
            return false;
    }
    return true;
}
