#include "DecimateStrategy.h"

#include <CGAL/Surface_mesh_simplification/edge_collapse.h>

#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Face_count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Face_count_ratio_stop_predicate.h>

#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_length_cost.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h>

namespace SMS = CGAL::Surface_mesh_simplification;

// Helper to set error message
static void setError(QString* out, const QString& msg)
{
    if (out) *out = msg;
}

// Decimate strategy implementation
bool DecimateStrategy::run(CoreMesh& core, const RemeshParams& p, QString* outError)
{
    if (core.empty()) {
        setError(outError, "Core mesh is empty.");
        return false;
    }

    core.sanitizeAndTriangulate();

    const std::size_t faces0 = static_cast<std::size_t>(core.sm.number_of_faces());
    if (faces0 == 0) {
        setError(outError, "No faces to decimate.");
        return false;
    }

    const bool useRatio = (p.decimateRatio > 0.0 && p.decimateRatio < 1.0);

    if (!useRatio) {
        const std::size_t targetFaces = p.decimateTargetFaces;

        if (targetFaces == 0 || targetFaces >= faces0) {
            return true;
        }

        SMS::Face_count_stop_predicate<CoreMesh::SMesh> stop(targetFaces);

        try {
            SMS::edge_collapse(
                core.sm,
                stop,
                CGAL::parameters::get_cost(SMS::Edge_length_cost<CoreMesh::SMesh>())
                .get_placement(SMS::Midpoint_placement<CoreMesh::SMesh>())
            );
        }
        catch (const std::exception& e) {
            setError(outError, QString("Decimate exception: ") + e.what());
            return false;
        }
        catch (...) {
            setError(outError, "Decimate unknown exception.");
            return false;
        }
    }
    else {
        const double r = p.decimateRatio;
        SMS::Face_count_ratio_stop_predicate<CoreMesh::SMesh> stop(r, core.sm);

        try {
            SMS::edge_collapse(
                core.sm,
                stop,
                CGAL::parameters::get_cost(SMS::Edge_length_cost<CoreMesh::SMesh>())
                .get_placement(SMS::Midpoint_placement<CoreMesh::SMesh>())
            );
        }
        catch (const std::exception& e) {
            setError(outError, QString("Decimate exception: ") + e.what());
            return false;
        }
        catch (...) {
            setError(outError, "Decimate unknown exception.");
            return false;
        }
    }

    core.sanitizeAndTriangulate();
    return true;
}
