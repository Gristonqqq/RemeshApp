#include "CpuMeshBuffer.h"

// Clear all mesh data
void CpuMeshBuffer::clear()
{
    positions.clear();
    normals.clear();
    indices.clear();
}

// Check if the mesh buffer is empty
bool CpuMeshBuffer::isEmpty() const
{
    return positions.empty() || indices.empty();
}

// Recalculate normals based on current positions and indices
void CpuMeshBuffer::recalculateNormals()
{
    normals.assign(positions.size(), QVector3D(0, 0, 0));
    if (positions.empty() || indices.size() < 3) return;

    const size_t triCount = indices.size() / 3;
    for (size_t t = 0; t < triCount; ++t)
    {
        const uint32_t i0 = indices[t * 3 + 0];
        const uint32_t i1 = indices[t * 3 + 1];
        const uint32_t i2 = indices[t * 3 + 2];

        if (i0 >= positions.size() || i1 >= positions.size() || i2 >= positions.size())
            continue;

        const QVector3D& p0 = positions[i0];
        const QVector3D& p1 = positions[i1];
        const QVector3D& p2 = positions[i2];

        const QVector3D e1 = p1 - p0;
        const QVector3D e2 = p2 - p0;
        const QVector3D n = QVector3D::crossProduct(e1, e2);

        normals[i0] += n;
        normals[i1] += n;
        normals[i2] += n;
    }

    for (auto& n : normals)
    {
        const float len = n.length();
        if (len > 1e-12f) n /= len;
        else n = QVector3D(0, 0, 1);
    }
}
