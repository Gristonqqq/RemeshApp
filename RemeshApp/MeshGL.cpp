#include "MeshGL.h"

// Constructor
MeshGL::MeshGL()
    : m_vbo(QOpenGLBuffer::VertexBuffer),
    m_ebo(QOpenGLBuffer::IndexBuffer)
{
}

// Destructor
MeshGL::~MeshGL()
{
    destroy();
}

// Initialize OpenGL resources
void MeshGL::initialize()
{
    if (m_initialized) return;

    m_vao.create();
    m_vbo.create();
    m_ebo.create();

    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_initialized = true;
}

// Destroy OpenGL resources
void MeshGL::destroy()
{
    if (!m_initialized) return;

    m_vao.destroy();
    m_vbo.destroy();
    m_ebo.destroy();

    m_indexCount = 0;
    m_initialized = false;
}

// Upload mesh data to GPU
void MeshGL::upload(QOpenGLFunctions_3_3_Core* f, const CpuMeshBuffer& cpu)
{
    if (!f) return;
    if (!m_initialized) initialize();

    if (cpu.positions.empty() || cpu.indices.empty())
    {
        m_indexCount = 0;
        return;
    }

    const bool hasNormals = (cpu.normals.size() == cpu.positions.size());

    std::vector<Vertex> verts;
    verts.resize(cpu.positions.size());

    for (size_t i = 0; i < cpu.positions.size(); ++i)
    {
        const auto& p = cpu.positions[i];
        const QVector3D n = hasNormals ? cpu.normals[i] : QVector3D(0, 0, 1);

        verts[i] = Vertex{
            p.x(), p.y(), p.z(),
            n.x(), n.y(), n.z()
        };
    }

    m_vao.bind();

    m_vbo.bind();
    m_vbo.allocate(verts.data(), int(verts.size() * sizeof(Vertex)));

    m_ebo.bind();
    m_ebo.allocate(cpu.indices.data(), int(cpu.indices.size() * sizeof(uint32_t)));

    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, px));

    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));

    m_vao.release();

    m_ebo.release();
    m_vbo.release();

    m_indexCount = int(cpu.indices.size());
}

// Draw the mesh
void MeshGL::draw(QOpenGLFunctions_3_3_Core* f)
{
    if (!f || !m_initialized || m_indexCount <= 0) return;

    m_vao.bind();

    // optional safety:
    // m_ebo.bind();

    f->glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);

    // m_ebo.release();
    m_vao.release();
}

