#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QMouseEvent>

#include "CoreMesh.h"
#include "CpuMeshBuffer.h"
#include "MeshGL.h"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget() override;

    void loadModel(const QString& path);

    CoreMesh& coreMesh() { return m_core; }
    const CoreMesh& coreMesh() const { return m_core; }

    const CpuMeshBuffer& cpuMesh() const { return m_cpuCache; }

    void setCoreMesh(CoreMesh core);
    void refreshGPUFromCore(bool recomputeNormals = true);

    bool showWireframe = false;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

signals:
    void glInitialized();

private:
    void uploadToGPU();

    CoreMesh      m_core;
    CpuMeshBuffer m_cpuCache;
    MeshGL        m_meshGL;

    QOpenGLShaderProgram shader;

    float cameraDistance = 3.0f;
    float rotationX = 0.0f, rotationY = 0.0f;
    QMatrix4x4 projection;
    QPoint lastMousePos;
};
