#include "DecimateStrategy.h"

#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Face_count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_length_cost.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h>
#include <CGAL/Polygon_mesh_processing/stitch_borders.h>

#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>

#include <QDebug>
#include <algorithm>
#include <cmath>

namespace SMS = CGAL::Surface_mesh_simplification;
namespace PMP = CGAL::Polygon_mesh_processing;

// Helper to set error message
static void setError(QString* out, const QString& msg)
{
    if (out) *out = msg;
}

// Count border edges in the mesh
static std::size_t countBorderEdges(const CoreMesh::SMesh& sm)
{
    std::size_t c = 0;
    for (auto e : sm.edges())
        if (sm.is_border(e)) ++c;
    return c;
}

// Compute target number of faces based on parameters
static std::size_t computeTargetFaces(std::size_t curF, const RemeshParams& p)
{
    std::size_t targetF = 0;

    if (p.decimateRatio > 0.0) {
        const double r = std::clamp(p.decimateRatio, 0.0, 1.0);
        targetF = static_cast<std::size_t>(std::floor(double(curF) * r));
    }
    else {
        targetF = (p.decimateTargetFaces > 0) ? p.decimateTargetFaces : curF;
    }

    targetF = std::max<std::size_t>(4, std::min<std::size_t>(targetF, curF));
    return targetF;
}

// Run the decimation strategy
bool DecimateStrategy::run(CoreMesh& core, const RemeshParams& p, QString* outError)
{
    if (core.empty()) {
        setError(outError, "Core mesh is empty.");
        return false;
    }

    core.sanitizeAndTriangulate();
    if (core.sm.is_empty() || core.sm.number_of_faces() == 0) {
        setError(outError, "Mesh has no faces after sanitize.");
        return false;
    }

    try { PMP::stitch_borders(core.sm); }
    catch (...) {}

    core.sanitizeAndTriangulate();

    const std::size_t curF = static_cast<std::size_t>(core.sm.number_of_faces());
    const std::size_t targetF = computeTargetFaces(curF, p);

    const std::size_t borderE = countBorderEdges(core.sm);

    qInfo() << "[Decimate] BEFORE"
        << "V" << core.sm.number_of_vertices()
        << "F" << core.sm.number_of_faces()
        << "E" << core.sm.number_of_edges()
        << "borderE" << borderE
        << "targetFaces=" << targetF
        << "ratio=" << p.decimateRatio;

    if (targetF >= curF) {
        qInfo() << "[Decimate] No decimation needed (target >= current).";
        return true;
    }

    auto ecm_pair = core.sm.add_property_map<CoreMesh::SMesh::Edge_index, bool>(
        "e:is_constrained", false
    );
    auto ecm = ecm_pair.first;

    for (auto e : core.sm.edges())
        ecm[e] = core.sm.is_border(e);

    SMS::Edge_length_cost<CoreMesh::SMesh> cost;
    SMS::Midpoint_placement<CoreMesh::SMesh> placement;

    int removed = 0;

    try
    {
        SMS::Face_count_stop_predicate<CoreMesh::SMesh> stop(targetF);

        removed = SMS::edge_collapse(
            core.sm,
            stop,
            CGAL::parameters::
            get_cost(cost).
            get_placement(placement).
            edge_is_constrained_map(ecm)
        );

        if (core.sm.has_garbage())
            core.sm.collect_garbage();
    }
    catch (const std::exception& e)
    {
        setError(outError, QString("Decimate exception: ") + e.what());
        return false;
    }
    catch (...)
    {
        setError(outError, "Decimate unknown exception.");
        return false;
    }

    core.sanitizeAndTriangulate();

    const std::size_t afterF = static_cast<std::size_t>(core.sm.number_of_faces());
    qInfo() << "[Decimate] AFTER"
        << "removed=" << removed
        << "V" << core.sm.number_of_vertices()
        << "F" << core.sm.number_of_faces()
        << "E" << core.sm.number_of_edges()
        << "borderE" << countBorderEdges(core.sm);

    if (afterF > targetF) {
        qWarning() << "[Decimate] Could not reach targetFaces without collapsing border edges."
            << "Requested:" << targetF << "Got:" << afterF
            << "(This is expected for meshes with many boundaries / non-manifold issues).";
    }

    return !core.sm.is_empty() && core.sm.number_of_faces() > 0;
}
