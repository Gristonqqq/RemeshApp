#include "RemeshApp.h"
#include "OpenGLWidget.h"
#include <QFileDialog>
#include <QDebug>
#include <QTimer>


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
        }
        else {
            qDebug() << "Waiting for OpenGL to initialize...";
        }
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
