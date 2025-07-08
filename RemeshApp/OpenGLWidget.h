#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QMouseEvent>
#include "Mesh.h"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget();
    void loadModel(const QString& path);
    void loadMeshToGPU(const Mesh& mesh);

    bool showWireframe = false;
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    void wheelEvent(QWheelEvent* event) override;
private:
    Mesh currentMesh;

    QOpenGLShaderProgram shader;
    GLuint VAO = 0, VBO = 0, EBO = 0;
    int indexCount = 0;

    float cameraDistance = 3.0f;
    float rotationX = 0.0f, rotationY = 0.0f;
    QMatrix4x4 projection, model;
    QPoint lastMousePos;
signals:
    void glInitialized();
};
