#include "OpenGLWidget.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <QDebug>
#include <algorithm>

// Constructor
OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

// Destructor
OpenGLWidget::~OpenGLWidget()
{
    if (isValid()) {
        makeCurrent();
        m_meshGL.destroy();
        doneCurrent();
    }
}

// Initialize OpenGL
void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    m_meshGL.initialize();

    shader.addShaderFromSourceCode(QOpenGLShader::Vertex,
        R"(#version 330 core
           layout (location = 0) in vec3 aPos;
           layout (location = 1) in vec3 aNormal;

           uniform mat4 model;
           uniform mat4 view;
           uniform mat4 projection;

           out vec3 FragPos;
           out vec3 Normal;

           void main() {
               FragPos = vec3(model * vec4(aPos, 1.0));
               Normal  = mat3(transpose(inverse(model))) * aNormal;
               gl_Position = projection * view * vec4(FragPos, 1.0);
           })"
    );

    shader.addShaderFromSourceCode(QOpenGLShader::Fragment,
        R"(#version 330 core
           in vec3 FragPos;
           in vec3 Normal;

           out vec4 FragColor;

           uniform vec3 lightDir;
           uniform vec3 lightColor;
           uniform vec3 objectColor;
           uniform bool isWireframe;

           void main() {
               if (isWireframe) {
                   FragColor = vec4(0.0, 0.0, 0.0, 1.0);
               } else {
                   vec3 norm = normalize(Normal);
                   float diff = max(dot(norm, -lightDir), 0.0);
                   vec3 diffuse = diff * lightColor;
                   vec3 result = diffuse * objectColor;
                   FragColor = vec4(result, 1.0);
               }
           })"
    );

    if (!shader.link()) {
        qWarning() << "Shader link error:" << shader.log();
    }

    emit glInitialized();
}

// Resize OpenGL viewport
void OpenGLWidget::resizeGL(int w, int h)
{
    projection.setToIdentity();
    projection.perspective(45.0f, float(w) / float(std::max(h, 1)), 0.1f, 100.0f);
}

// Paint OpenGL scene
void OpenGLWidget::paintGL()
{
    glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!shader.isLinked() || m_meshGL.isEmpty())
        return;

    shader.bind();
    shader.setUniformValue("lightDir", QVector3D(-0.5f, -1.0f, -0.3f).normalized());
    shader.setUniformValue("lightColor", QVector3D(1.0f, 1.0f, 1.0f));
    shader.setUniformValue("objectColor", QVector3D(0.7f, 0.7f, 0.7f));

    QMatrix4x4 view, model;
    view.setToIdentity();
    view.translate(0, 0, -cameraDistance);

    model.setToIdentity();
    model.rotate(rotationX, 1.0f, 0.0f, 0.0f);
    model.rotate(rotationY, 0.0f, 1.0f, 0.0f);

    shader.setUniformValue("projection", projection);
    shader.setUniformValue("view", view);
    shader.setUniformValue("model", model);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    shader.setUniformValue("isWireframe", false);
    m_meshGL.draw(this);

    glDisable(GL_POLYGON_OFFSET_FILL);

    if (showWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.0f);

        shader.setUniformValue("isWireframe", true);
        m_meshGL.draw(this);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    shader.release();
}

// Mouse press event
void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    lastMousePos = event->pos();
}

// Mouse move event
void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    const QPointF pos = event->position();
    const int dx = int(pos.x() - lastMousePos.x());
    const int dy = int(pos.y() - lastMousePos.y());

    rotationX += dy * 0.5f;
    rotationY += dx * 0.5f;

    lastMousePos = event->pos();
    update();
}

// Mouse wheel event
void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() > 0)
        cameraDistance -= 0.2f;
    else
        cameraDistance += 0.2f;

    cameraDistance = std::clamp(cameraDistance, 0.1f, 100.0f);
    update();
}

// Set the core mesh
void OpenGLWidget::setCoreMesh(CoreMesh core)
{
    m_core = std::move(core);

    if (isValid()) {
        makeCurrent();
        refreshGPUFromCore(true);
        doneCurrent();
    }

    update();
}

// Refresh GPU data from core mesh
void OpenGLWidget::refreshGPUFromCore(bool recomputeNormals)
{
    if (!isValid())
        return;

    if (m_core.empty()) {
        m_cpuCache.clear();
        update();
        return;
    }

    m_core.sanitizeAndTriangulate();
    m_cpuCache = m_core.toCpuMeshBuffer(recomputeNormals);

    uploadToGPU();
    update();
}

// Upload CPU mesh data to GPU
void OpenGLWidget::uploadToGPU()
{
    if (m_cpuCache.isEmpty())
        return;

    m_meshGL.upload(this, m_cpuCache);
}

// Load model from file using Assimp
void OpenGLWidget::loadModel(const QString& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.toStdString(),
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_RemoveRedundantMaterials |
        aiProcess_SortByPType
    );

    if (!scene || !scene->HasMeshes()) {
        qWarning() << "Assimp load error:" << importer.GetErrorString();
        return;
    }

    const aiMesh* mesh = scene->mMeshes[0];

    CpuMeshBuffer cpu;
    cpu.positions.reserve(mesh->mNumVertices);
    cpu.normals.reserve(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        const aiVector3D pos = mesh->mVertices[i];
        cpu.positions.emplace_back(pos.x, pos.y, pos.z);

        if (mesh->HasNormals()) {
            const aiVector3D n = mesh->mNormals[i];
            cpu.normals.emplace_back(n.x, n.y, n.z);
        }
    }

    cpu.indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        if (face.mNumIndices == 3) {
            cpu.indices.push_back(static_cast<std::uint32_t>(face.mIndices[0]));
            cpu.indices.push_back(static_cast<std::uint32_t>(face.mIndices[1]));
            cpu.indices.push_back(static_cast<std::uint32_t>(face.mIndices[2]));
        }
    }

    if (cpu.normals.size() != cpu.positions.size())
        cpu.recalculateNormals();

    CoreMesh core;
    if (!core.fromCpuMeshBuffer(cpu)) {
        qWarning() << "CoreMesh build failed (invalid geometry?) for:" << path;
        return;
    }

    setCoreMesh(std::move(core));
}
