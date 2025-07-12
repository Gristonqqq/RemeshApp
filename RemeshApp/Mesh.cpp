#include "Mesh.h"

void Mesh::recalculateNormals() {
    normals.resize(vertices.size(), QVector3D(0, 0, 0));
    for (size_t i = 0; i < indices.size(); i += 3) {
        auto& v0 = vertices[indices[i]];
        auto& v1 = vertices[indices[i + 1]];
        auto& v2 = vertices[indices[i + 2]];
        QVector3D n = QVector3D::normal(v1 - v0, v2 - v0);
        normals[indices[i]] += n;
        normals[indices[i + 1]] += n;
        normals[indices[i + 2]] += n;
    }
    for (auto& n : normals) {
        n.normalize();
    }
}
