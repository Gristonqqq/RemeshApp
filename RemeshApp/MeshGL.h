#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>

#include "CpuMeshBuffer.h"

class MeshGL
{
public:
    MeshGL();
    ~MeshGL();

    MeshGL(const MeshGL&) = delete;
    MeshGL& operator=(const MeshGL&) = delete;

    void initialize();

    void upload(QOpenGLFunctions_3_3_Core* f, const CpuMeshBuffer& cpu);

    void draw(QOpenGLFunctions_3_3_Core* f);

    void destroy();
    bool isInitialized() const { return m_initialized; }
    bool isEmpty() const { return m_indexCount == 0; }

private:
    struct Vertex
    {
        float px, py, pz;
        float nx, ny, nz;
    };

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_ebo;
    int m_indexCount = 0;
    bool m_initialized = false;
};
