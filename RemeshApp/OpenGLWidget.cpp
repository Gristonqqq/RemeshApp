#include "OpenGLWidget.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

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
				Normal = mat3(transpose(inverse(model))) * aNormal;

				gl_Position = projection * view * vec4(FragPos, 1.0);
			})");

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
					FragColor = vec4(0.0, 0.0, 0.0, 1.0);  // сітка чорна
				} else {
					vec3 norm = normalize(Normal);
					float diff = max(dot(norm, -lightDir), 0.0);
					vec3 diffuse = diff * lightColor;
					vec3 result = diffuse * objectColor;

					FragColor = vec4(result, 1.0);
				}
			}
			)");

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
	qDebug() << "paintGL called, indexCount =" << indexCount;
	glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.bind();
	shader.setUniformValue("lightDir", QVector3D(-0.5f, -1.0f, -0.3f).normalized());
	shader.setUniformValue("lightColor", QVector3D(1.0f, 1.0f, 1.0f));
	shader.setUniformValue("objectColor", QVector3D(0.7f, 0.7f, 0.7f));

	if (indexCount == 0 || !shader.isLinked())
		return;

	QMatrix4x4 view, model;
	view.setToIdentity();
	view.translate(0, 0, -cameraDistance);
	model.setToIdentity();
	model.rotate(rotationX, 1.0f, 0.0f, 0.0f);
	model.rotate(rotationY, 0.0f, 1.0f, 0.0f);

	shader.setUniformValue("projection", projection);
	shader.setUniformValue("view", view);
	shader.setUniformValue("model", model);

	// Draw the model
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	shader.setUniformValue("isWireframe", false);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	glDisable(GL_POLYGON_OFFSET_FILL);

	// Draw wireframe if enabled
	if (showWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(1.0f);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		shader.setUniformValue("isWireframe", true);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

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

	cameraDistance = std::clamp(cameraDistance, 0.1f, 100.0f);

	update();
}

// Load model from file using Assimp and update OpenGL buffers
void OpenGLWidget::loadModel(const QString& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.toStdString(),
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals);

	if (!scene || !scene->HasMeshes()) {
		qWarning() << "Assimp load error:" << importer.GetErrorString();
		return;
	}

	const aiMesh* mesh = scene->mMeshes[0];
	currentMesh.clear();

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		aiVector3D pos = mesh->mVertices[i];
		aiVector3D normal = mesh->mNormals ? mesh->mNormals[i] : aiVector3D(0, 0, 1);
		currentMesh.vertices.push_back(QVector3D(pos.x, pos.y, pos.z));
		currentMesh.normals.push_back(QVector3D(normal.x, normal.y, normal.z));
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		const aiFace& face = mesh->mFaces[i];
		if (face.mNumIndices == 3) {
			currentMesh.indices.push_back(face.mIndices[0]);
			currentMesh.indices.push_back(face.mIndices[1]);
			currentMesh.indices.push_back(face.mIndices[2]);
		}
	}

	loadMeshToGPU(currentMesh);
}

//load mesh data to GPU buffers
void OpenGLWidget::loadMeshToGPU(const Mesh& mesh) {
	qDebug() << "Load to gpu started!";
	if (mesh.isEmpty()) return;

	std::vector<float> vertexData;
	for (int i = 0; i < mesh.vertices.size(); ++i) {
		const QVector3D& pos = mesh.vertices[i];
		const QVector3D& norm = mesh.normals[i];
		vertexData.push_back(pos.x());
		vertexData.push_back(pos.y());
		vertexData.push_back(pos.z());

		vertexData.push_back(norm.x());
		vertexData.push_back(norm.y());
		vertexData.push_back(norm.z());
	}

	if (VAO) glDeleteVertexArrays(1, &VAO);
	if (VBO) glDeleteBuffers(1, &VBO);
	if (EBO) glDeleteBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // позиції
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // нормалі
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	indexCount = static_cast<int>(mesh.indices.size());
	update();
}
