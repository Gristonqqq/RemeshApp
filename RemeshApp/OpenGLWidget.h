#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QMouseEvent>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    void wheelEvent(QWheelEvent* event) override;
private:
    GLuint VAO, VBO, EBO;
    QOpenGLShaderProgram* shaderProgram;


    QMatrix4x4 projection;
    QMatrix4x4 model;

    QPoint lastMousePos;
    float rotationX = 0.0f;
    float rotationY = 0.0f;

    float cameraDistance = 3.0f;
};
