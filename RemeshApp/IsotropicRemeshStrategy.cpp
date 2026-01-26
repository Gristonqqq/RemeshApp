#include "IsotropicRemeshStrategy.h"

#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Polygon_mesh_processing/repair.h>

#include <QDebug>
#include <cmath>
#include <vector>
#include <algorithm>

namespace PMP = CGAL::Polygon_mesh_processing;

// Helper to set error message
static void setError(QString* out, const QString& msg)
{
    if (out) *out = msg;
}

// Compute length of an edge
static double edgeLength(const CoreMesh::SMesh& sm, CoreMesh::SMesh::Edge_index e)
{
    auto h = sm.halfedge(e);
    auto v0 = sm.source(h);
    auto v1 = sm.target(h);

    const auto& p0 = sm.point(v0);
    const auto& p1 = sm.point(v1);

    const double dx = p0.x() - p1.x();
    const double dy = p0.y() - p1.y();
    const double dz = p0.z() - p1.z();
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

// Edge length statistics
struct EdgeStats {
    double mn = 0, mx = 0, avg = 0;
    std::size_t count = 0;
};

// Compute edge length statistics
static EdgeStats computeEdgeStats(const CoreMesh::SMesh& sm)
{
    EdgeStats s;
    if (sm.number_of_edges() == 0) return s;

    double sum = 0.0;
    bool first = true;

    for (auto e : sm.edges())
    {
        const double len = edgeLength(sm, e);
        if (first) { s.mn = s.mx = len; first = false; }
        s.mn = std::min(s.mn, len);
        s.mx = std::max(s.mx, len);
        sum += len;
        s.count++;
    }

    s.avg = (s.count > 0) ? (sum / double(s.count)) : 0.0;
    return s;
}

// Clear all constraints
static void clearConstraints(CoreMesh::SMesh& sm,
    CoreMesh::SMesh::Property_map<CoreMesh::SMesh::Edge_index, bool>& ecm)
{
    for (auto e : sm.edges())
        ecm[e] = false;
}

// Mark border edges as constraints
static void markBorderAsConstraints(CoreMesh::SMesh& sm,
    CoreMesh::SMesh::Property_map<CoreMesh::SMesh::Edge_index, bool>& ecm)
{
    for (auto e : sm.edges())
    {
        auto h = sm.halfedge(e);
        if (sm.is_border(h) || sm.is_border(sm.opposite(h)))
            ecm[e] = true;
    }
}

// Get maximum length of constrained edges
static double maxConstrainedEdgeLength(const CoreMesh::SMesh& sm,
    const CoreMesh::SMesh::Property_map<CoreMesh::SMesh::Edge_index, bool>& ecm)
{
    double mx = 0.0;
    for (auto e : sm.edges())
        if (ecm[e])
            mx = std::max(mx, edgeLength(sm, e));
    return mx;
}

// Split too long constrained edges
static void splitTooLongConstraintEdges(CoreMesh::SMesh& sm,
    CoreMesh::SMesh::Property_map<CoreMesh::SMesh::Edge_index, bool>& ecm,
    double maxLen)
{
    std::vector<CoreMesh::SMesh::Edge_index> constrained;
    constrained.reserve(sm.number_of_edges());

    for (auto e : sm.edges())
        if (ecm[e]) constrained.push_back(e);

    if (constrained.empty()) return;

    PMP::split_long_edges(constrained, maxLen, sm,
        PMP::parameters::edge_is_constrained_map(ecm));

    clearConstraints(sm, ecm);
    markBorderAsConstraints(sm, ecm);
}

// Main remeshing function
bool IsotropicRemeshStrategy::run(CoreMesh& core, const RemeshParams& p, QString* outError)
{
    if (core.empty()) {
        setError(outError, "Core mesh is empty.");
        return false;
    }

    double target = p.targetEdgeLength;
    if (target <= 0.0) {
        setError(outError, "targetEdgeLength must be > 0.");
        return false;
    }

    const int iters = (p.iterations > 0) ? p.iterations : 1;
    const int itersUse = std::max(iters, 10);

    core.sanitizeAndTriangulate();

    try { PMP::duplicate_non_manifold_vertices(core.sm); }
    catch (...) {}

    {
        const auto stBefore = computeEdgeStats(core.sm);
        qInfo() << "[IsoRemesh] BEFORE"
            << "V" << core.sm.number_of_vertices()
            << "F" << core.sm.number_of_faces()
            << "E" << core.sm.number_of_edges()
            << "edgeLen(min/avg/max)=" << stBefore.mn << stBefore.avg << stBefore.mx
            << "target=" << target
            << "iters=" << itersUse;
    }

    auto ecm_pair = core.sm.add_property_map<CoreMesh::SMesh::Edge_index, bool>(
        "e:is_constrained", false
    );
    auto ecm = ecm_pair.first;

    clearConstraints(core.sm, ecm);
    markBorderAsConstraints(core.sm, ecm);

    const double maxConstraintLen = (4.0 / 3.0) * target;

    for (int pass = 0; pass < 3; ++pass) {
        const double mx = maxConstrainedEdgeLength(core.sm, ecm);
        qInfo() << "[IsoRemesh] split pass" << pass
            << "maxBorderEdge=" << mx
            << "limit=" << maxConstraintLen;

        if (mx <= maxConstraintLen) break;

        try {
            splitTooLongConstraintEdges(core.sm, ecm, maxConstraintLen);
        }
        catch (const std::exception& e) {
            setError(outError, QString("split_long_edges exception: ") + e.what());
            return false;
        }
        catch (...) {
            setError(outError, "split_long_edges unknown exception.");
            return false;
        }
    }

    std::vector<CoreMesh::SMesh::Face_index> faces;
    faces.reserve((size_t)core.sm.number_of_faces());
    for (auto f : core.sm.faces())
        faces.push_back(f);

    if (faces.empty()) {
        setError(outError, "No faces to remesh.");
        return false;
    }

    try {
        PMP::isotropic_remeshing(
            faces,
            target,
            core.sm,
            PMP::parameters::number_of_iterations(itersUse)
            .number_of_relaxation_steps(5)
            .protect_constraints(true)
            .edge_is_constrained_map(ecm)
        );
    }
    catch (const std::exception& e) {
        setError(outError, QString("Isotropic remesh exception: ") + e.what());
        return false;
    }
    catch (...) {
        setError(outError, "Isotropic remesh unknown exception.");
        return false;
    }

    core.sanitizeAndTriangulate();

    {
        const auto stAfter = computeEdgeStats(core.sm);
        qInfo() << "[IsoRemesh] AFTER"
            << "V" << core.sm.number_of_vertices()
            << "F" << core.sm.number_of_faces()
            << "E" << core.sm.number_of_edges()
            << "edgeLen(min/avg/max)=" << stAfter.mn << stAfter.avg << stAfter.mx;
    }

    return true;
}