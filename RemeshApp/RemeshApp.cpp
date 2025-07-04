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


