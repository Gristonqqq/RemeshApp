#pragma once

#include <QVector3D>
#include <vector>

struct Mesh
{
    std::string filePath;
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> normals;
    std::vector<unsigned int> indices;

    void clear() {
        vertices.clear();
        normals.clear();
        indices.clear();
    }

    bool isEmpty() const {
        return vertices.empty() || indices.empty();
    }
    void recalculateNormals();
};