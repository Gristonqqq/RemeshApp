#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include "Remesher.h"
#include <QVector3D>
#include <vector>
#include "Mesh.h"
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <qdebug.h>

namespace PMP = CGAL::Polygon_mesh_processing;

// CGAL typedefs
typedef CGAL::Exact_predicates_inexact_constructions_kernel   K;
typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Surface_mesh<Point> SurfaceMesh;

void Remesher::remeshMesh(Mesh& mesh, double target_edge_length, int iterations)
{
    if (mesh.isEmpty()) return;

    SurfaceMesh sm;

	//Working with my own mesh structure
    /*
    std::vector<SurfaceMesh::Vertex_index> sm_vertices;
    for (const QVector3D& v : mesh.vertices) {
        sm_vertices.push_back(sm.add_vertex(Point(v.x(), v.y(), v.z())));
    }

    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        SurfaceMesh::Vertex_index v0 = sm_vertices[mesh.indices[i]];
        SurfaceMesh::Vertex_index v1 = sm_vertices[mesh.indices[i + 1]];
        SurfaceMesh::Vertex_index v2 = sm_vertices[mesh.indices[i + 2]];

        sm.add_face(v0, v1, v2);
    }*/

	// library mesh loading
    if (!PMP::IO::read_polygon_mesh(mesh.filePath, sm) || !CGAL::is_triangle_mesh(sm))
    {
        qDebug() << "Invalid input.";
        return ;
    }

	// Remesh the surface mesh
    PMP::isotropic_remeshing(
        faces(sm),
        target_edge_length,
        sm,
        PMP::parameters::number_of_iterations(iterations)
        .protect_constraints(true)
    );

	// library mesh saving
    CGAL::IO::write_polygon_mesh("out.off", sm, CGAL::parameters::stream_precision(17));

	// load remeshed mesh into my own structure    
    mesh.clear();

    std::map<SurfaceMesh::Vertex_index, int> index_map;
    for (SurfaceMesh::Vertex_index v : sm.vertices()) {
        const Point& p = sm.point(v);
        mesh.vertices.push_back(QVector3D(p.x(), p.y(), p.z()));
        index_map[v] = mesh.vertices.size() - 1;
    }

    for (SurfaceMesh::Face_index f : sm.faces()) {
        std::vector<unsigned int> face_indices;
        for (auto v : CGAL::vertices_around_face(sm.halfedge(f), sm)) {
            face_indices.push_back(index_map[v]);
        }
        if (face_indices.size() == 3) {
            mesh.indices.insert(mesh.indices.end(), face_indices.begin(), face_indices.end());
        }
    }

	// ecalculate normals for the remeshed mesh
    mesh.recalculateNormals();
}
