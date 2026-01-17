#include "Remesher.h"

#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>

#include <QDebug>

namespace PMP = CGAL::Polygon_mesh_processing;

// Mark border edges as constrained in the given edge constraint map
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

// Split constrained edges that are longer than maxLen
static void splitTooLongConstraintEdges(CoreMesh::SMesh& sm,
    CoreMesh::SMesh::Property_map<CoreMesh::SMesh::Edge_index, bool>& ecm,
    double maxLen)
{
    std::vector<CoreMesh::SMesh::Edge_index> constrained;
    constrained.reserve(sm.number_of_edges());

    for (auto e : sm.edges())
        if (ecm[e]) constrained.push_back(e);

    if (constrained.empty()) return;

    PMP::split_long_edges(constrained, maxLen, sm, PMP::parameters::edge_is_constrained_map(ecm));

    markBorderAsConstraints(sm, ecm);
}

// Remesh the mesh with given target edge length and iterations
bool Remesher::remeshMesh(CoreMesh& core, double target_edge_length, int iterations)
{
    if (core.empty() || target_edge_length <= 0.0)
        return false;

    core.sanitizeAndTriangulate();

    try { PMP::duplicate_non_manifold_vertices(core.sm); }
    catch (...) {}

    auto ecm_pair = core.sm.add_property_map<CoreMesh::SMesh::Edge_index, bool>(
        "e:is_constrained", false
    );
    auto ecm = ecm_pair.first;

    markBorderAsConstraints(core.sm, ecm);

    const double maxConstraintLen = (4.0 / 3.0) * target_edge_length;

    for (int pass = 0; pass < 3; ++pass)
        splitTooLongConstraintEdges(core.sm, ecm, maxConstraintLen);

    std::vector<CoreMesh::SMesh::Face_index> faces;
    faces.reserve(static_cast<size_t>(core.sm.number_of_faces()));
    for (auto f : core.sm.faces())
        faces.push_back(f);

    if (faces.empty())
        return false;

    try {
        PMP::isotropic_remeshing(
            faces,
            target_edge_length,
            core.sm,
            PMP::parameters::number_of_iterations(iterations)
            .protect_constraints(true)
            .edge_is_constrained_map(ecm)
        );
    }
    catch (const CGAL::Precondition_exception& e) {
        qWarning() << "Remesh precondition exception:" << e.what();
        return false;
    }
    catch (const std::exception& e) {
        qWarning() << "Remesh exception:" << e.what();
        return false;
    }
    catch (...) {
        qWarning() << "Remesh unknown exception";
        return false;
    }

    core.sanitizeAndTriangulate();
    return true;
}
