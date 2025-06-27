#include "OpenGLWidget.h"


static const float vertices[] = {
    // позиції           // кольори
    -0.5f, -0.5f, -0.5f,  1.f, 0.f, 0.f,
     0.5f, -0.5f, -0.5f,  0.f, 1.f, 0.f,
     0.5f,  0.5f, -0.5f,  0.f, 0.f, 1.f,
    -0.5f,  0.5f, -0.5f,  1.f, 1.f, 0.f,
    -0.5f, -0.5f,  0.5f,  0.f, 1.f, 1.f,
     0.5f, -0.5f,  0.5f,  1.f, 0.f, 1.f,
     0.5f,  0.5f,  0.5f,  1.f, 1.f, 1.f,
    -0.5f,  0.5f,  0.5f,  0.f, 0.f, 0.f
};

static const GLuint indices[] = {
    0, 1, 2, 2, 3, 0,   // back face
    4, 5, 6, 6, 7, 4,   // front face
    0, 1, 5, 5, 4, 0,   // bottom
    2, 3, 7, 7, 6, 2,   // top
    1, 2, 6, 6, 5, 1,   // right
    0, 3, 7, 7, 4, 0    // left
};

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {}

OpenGLWidget::~OpenGLWidget() {
    makeCurrent();
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    doneCurrent();
}

void OpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    // Шейдери
    shaderProgram = new QOpenGLShaderProgram(this);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
        R"(#version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aColor;
        uniform mat4 proj;
        out vec3 ourColor;
        void main() {
            gl_Position = proj * vec4(aPos, 1.0);
            ourColor = aColor;
        })");

    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
        R"(#version 330 core
        in vec3 ourColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(ourColor, 1.0);
        })");

    shaderProgram->link();

    // VAO + VBO
    GLuint EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Позиції
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Кольори
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGLWidget::resizeGL(int w, int h)
{
    projection.setToIdentity();
    projection.perspective(45.0f, float(w) / float(h), 0.1f, 100.0f);
}


void OpenGLWidget::paintGL()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 view;
    view.setToIdentity();
    view.translate(0, 0, -cameraDistance);  // <-- тепер враховується zoom завжди

    model.setToIdentity();
    model.rotate(rotationX, 1.0f, 0.0f, 0.0f);
    model.rotate(rotationY, 0.0f, 1.0f, 0.0f);

    shaderProgram->bind();
    shaderProgram->setUniformValue("proj", projection * view * model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}


void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    lastMousePos = event->pos();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPointF pos = event->position();
    int dx = int(pos.x() - lastMousePos.x());
    int dy = int(pos.y() - lastMousePos.y());

    rotationX += dy * 0.5f;
    rotationY += dx * 0.5f;

    lastMousePos = event->pos();
    update();  // перерисувати сцену
}

void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() > 0)
        cameraDistance -= 0.2f;  // приближення
    else
        cameraDistance += 0.2f;  // віддалення

    cameraDistance = std::clamp(cameraDistance, 1.0f, 10.0f); // обмеження

    update();  // перерисувати
}