#include "OpenGLWidget.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {}

OpenGLWidget::~OpenGLWidget() {
	if (isValid()) {
		makeCurrent();
		if (VBO) glDeleteBuffers(1, &VBO);
		if (EBO) glDeleteBuffers(1, &EBO);
		if (VAO) glDeleteVertexArrays(1, &VAO);
		doneCurrent();
	}
}

// Initialize OpenGL context and shaders
void OpenGLWidget::initializeGL() {
	initializeOpenGLFunctions();
	glEnable(GL_DEPTH_TEST);

	// Шейдери
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
				Normal = mat3(transpose(inverse(model))) * aNormal;  // нормалі трансформуються

				gl_Position = projection * view * vec4(FragPos, 1.0);
			})");

	shader.addShaderFromSourceCode(QOpenGLShader::Fragment,
		R"(#version 330 core
			in vec3 FragPos;
			in vec3 Normal;

			out vec4 FragColor;

			uniform vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.3));
			uniform vec3 lightColor = vec3(1.0);
			uniform vec3 objectColor = vec3(0.7, 0.7, 0.7);

			void main() {
				vec3 norm = normalize(Normal);
				float diff = max(dot(norm, -lightDir), 0.0);  // напрямок світла

				vec3 diffuse = diff * lightColor;
				vec3 result = diffuse * objectColor;

				FragColor = vec4(result, 1.0);
			})");

	if (!shader.link()) {
		qWarning() << "Shader link error:" << shader.log();
	}

	emit glInitialized();
}


// Resize OpenGL viewport and update projection matrix
void OpenGLWidget::resizeGL(int w, int h)
{
	projection.setToIdentity();
	projection.perspective(45.0f, float(w) / float(h), 0.1f, 100.0f);
}

// Paint the OpenGL scene
void OpenGLWidget::paintGL() {
	glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (indexCount == 0 || !shader.isLinked())
		return;

	QMatrix4x4 view, model;
	view.translate(0, 0, -cameraDistance);
	model.setToIdentity();
	model.rotate(rotationX, 1.0f, 0.0f, 0.0f);
	model.rotate(rotationY, 0.0f, 1.0f, 0.0f);


	shader.bind();
	shader.setUniformValue("model", model);
	shader.setUniformValue("view", view);
	shader.setUniformValue("projection", projection);


	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}


// Handle mouse press events to capture initial position
void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
	lastMousePos = event->pos();
}

// Handle mouse movement to update rotation angles
void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	QPointF pos = event->position();
	int dx = int(pos.x() - lastMousePos.x());
	int dy = int(pos.y() - lastMousePos.y());

	rotationX += dy * 0.5f;
	rotationY += dx * 0.5f;

	lastMousePos = event->pos();
	update();
}

// Handle mouse wheel events for zooming in and out
void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
	if (event->angleDelta().y() > 0)
		cameraDistance -= 0.2f;
	else
		cameraDistance += 0.2f;

	cameraDistance = std::clamp(cameraDistance, 1.0f, 100.0f);

	update();
}

// Load model from file using Assimp and update OpenGL buffers
void OpenGLWidget::loadModel(const QString& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.toStdString(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_PreTransformVertices);

	if (!scene || !scene->HasMeshes()) {
		qWarning() << "Assimp load error:" << importer.GetErrorString();
		return;
	}

	aiMesh* mesh = scene->mMeshes[0];
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		aiVector3D pos = mesh->mVertices[i];
		vertices.push_back(pos.x);
		vertices.push_back(pos.y);
		vertices.push_back(pos.z);

		aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[i] : aiVector3D(0, 0, 1);
		vertices.push_back(normal.x);
		vertices.push_back(normal.y);
		vertices.push_back(normal.z);

	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	if (VAO) glDeleteVertexArrays(1, &VAO);
	if (VBO) glDeleteBuffers(1, &VBO);
	if (EBO) glDeleteBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	indexCount = indices.size();
	update();
}