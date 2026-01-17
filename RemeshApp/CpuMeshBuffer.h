#pragma once

#include <vector>
#include <cstdint>

#include <QVector3D>

class CpuMeshBuffer
{
public:
    std::vector<QVector3D> positions;
    std::vector<QVector3D> normals;
    std::vector<uint32_t>  indices;

    void clear();
    bool isEmpty() const;

    void recalculateNormals();
};
