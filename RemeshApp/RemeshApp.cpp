#include "RemeshApp.h"
#include "OpenGLWidget.h"
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include "Remesher.h"

RemeshApp::RemeshApp(QWidget *parent)
    : QMainWindow(parent)
{
    ui = new Ui::RemeshAppClass();
    ui->setupUi(this);
    oglWidget = ui->openGLWidget;
    connect(oglWidget, &OpenGLWidget::glInitialized, this, [this]() {
        if (!pendingModelPath.isEmpty()) {
            oglWidget->makeCurrent();
            oglWidget->loadModel(pendingModelPath);
            oglWidget->doneCurrent();
            pendingModelPath.clear();
        }
        });
    connect(ui->showWireframeButton, &QPushButton::clicked, this, [=]() {
        oglWidget->showWireframe = !oglWidget->showWireframe;
        oglWidget->update();

        static bool toggled = false;
        toggled = !toggled;

        if (toggled)
            ui->showWireframeButton->setStyleSheet("background-color: blue; color: white;");
        else
            ui->showWireframeButton->setStyleSheet("background-color: rgb(214, 214, 214); color: rgb(83, 83, 83); ");
        });

    connect(ui->pushButtonRemesh, &QPushButton::clicked, this, &RemeshApp::onRemeshButtonClicked);
}

RemeshApp::~RemeshApp()
{}

// This function is triggered when the user selects "Import OBJ" from the menu
void RemeshApp::on_actionImport_obj_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Import OBJ", "", "OBJ Files (*.obj)");
    if (!filePath.isEmpty()) {
        pendingModelPath = filePath;

        if (oglWidget->isValid()) {
            oglWidget->makeCurrent();
            oglWidget->loadModel(filePath);
            oglWidget->doneCurrent();
            pendingModelPath.clear();
			Mesh& mesh = ui->openGLWidget->getMesh();
			mesh.filePath = filePath.toStdString();
        }
        else {
            qDebug() << "Waiting for OpenGL to initialize...";
        }
    }
}

void RemeshApp::onRemeshButtonClicked() {
	Mesh& mesh = ui->openGLWidget->getMesh();
    qDebug() << "Remesh button clicked!";
    double targetEdgeLength = ui->doubleSpinBox->value();
    qDebug() << "Before remesh:" << ui->openGLWidget->getMesh().vertices.size();
    Remesher::remeshMesh(mesh, targetEdgeLength);
    qDebug() << "After remesh:" << ui->openGLWidget->getMesh().vertices.size();

    ui->openGLWidget->makeCurrent();
    ui->openGLWidget->loadMeshToGPU(mesh);
    ui->openGLWidget->doneCurrent();
    ui->openGLWidget->update();

}

bool exportToOBJ(const Mesh& mesh, const QString& filePath)
{
    if (mesh.isEmpty()) return false;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream out(&file);

    for (const auto& v : mesh.vertices) {
        out << "v " << v.x() << " " << v.y() << " " << v.z() << "\n";
    }

    if (mesh.normals.size() == mesh.vertices.size()) {
        for (const auto& n : mesh.normals) {
            out << "vn " << n.x() << " " << n.y() << " " << n.z() << "\n";
        }
    }

    for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
        unsigned int i1 = mesh.indices[i] + 1;
        unsigned int i2 = mesh.indices[i + 1] + 1;
        unsigned int i3 = mesh.indices[i + 2] + 1;

        if (mesh.normals.size() == mesh.vertices.size()) {
            out << "f "
                << i1 << "//" << i1 << " "
                << i2 << "//" << i2 << " "
                << i3 << "//" << i3 << "\n";
        }
        else {
            out << "f " << i1 << " " << i2 << " " << i3 << "\n";
        }
    }

    file.close();
    return true;
}

void RemeshApp::on_actionExport_obj_triggered() {
	Mesh& mesh = ui->openGLWidget->getMesh();
    if (mesh.isEmpty()) return;

    QString dir = QFileDialog::getExistingDirectory(this, "Оберіть папку для експорту");

    if (dir.isEmpty()) return;

    QString filePath = QDir(dir).filePath("export.obj");

    if (exportToOBJ(mesh, filePath)) {
        qDebug() << "Меш експортовано до:" << filePath;
    }
    else {
        qWarning() << "Помилка експорту.";
    }
}


// Uncomment these lines if you want to support FBX and STL formats(Import works with bugs)
// MenuBar should have corresponding actions

/*
void RemeshApp::on_actionImport_fbx_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Import FBX", "", "FBX Files (*.fbx)");
    if (!filePath.isEmpty()) {
        pendingModelPath = filePath;

        if (oglWidget->isValid()) {
            oglWidget->makeCurrent();
            oglWidget->loadModel(filePath);
            oglWidget->doneCurrent();
            pendingModelPath.clear();
        }
        else {
            qDebug() << "Waiting for OpenGL to initialize...";
        }
    }
}

void RemeshApp::on_actionImport_stl_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Import STL", "", "STL Files (*.stl)");
    if (!filePath.isEmpty()) {
        pendingModelPath = filePath;

        if (oglWidget->isValid()) {
            oglWidget->makeCurrent();
            oglWidget->loadModel(filePath);
            oglWidget->doneCurrent();
            pendingModelPath.clear();
        }
        else {
            qDebug() << "Waiting for OpenGL to initialize...";
        }
    }
}
*/
