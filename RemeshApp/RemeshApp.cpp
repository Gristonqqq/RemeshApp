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
