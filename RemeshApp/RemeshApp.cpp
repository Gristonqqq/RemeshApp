#include "RemeshApp.h"

#include "OpenGLWidget.h"
#include "Remesher.h"

#include <QFileDialog>
#include <QDebug>
#include <QDir>

#include <CGAL/IO/polygon_mesh_io.h>

// Constructor
RemeshApp::RemeshApp(QWidget* parent)
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

// Destructor
RemeshApp::~RemeshApp()
{
    delete ui;
}

// Slot for importing OBJ files
void RemeshApp::on_actionImport_obj_triggered()
{
    const QString filePath = QFileDialog::getOpenFileName(this, "Import OBJ", "", "OBJ Files (*.obj)");
    if (filePath.isEmpty())
        return;

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

// Slot for remeshing the loaded mesh
void RemeshApp::onRemeshButtonClicked()
{
    CoreMesh& core = oglWidget->coreMesh();
    if (core.empty()) {
        qWarning() << "No mesh loaded (core is empty).";
        return;
    }

    const double targetEdgeLength = ui->doubleSpinBox->value();
    qDebug() << "Remesh button clicked. targetEdgeLength =" << targetEdgeLength;

    core.sanitizeAndTriangulate();

    qDebug() << "Before remesh verts:" << core.sm.number_of_vertices()
        << " faces:" << core.sm.number_of_faces();

    const bool ok = Remesher::remeshMesh(core, targetEdgeLength, 3);

    qDebug() << "After remesh verts:" << core.sm.number_of_vertices()
        << " faces:" << core.sm.number_of_faces();

    if (!ok) {
        qWarning() << "Remesh failed.";
        return;
    }

    oglWidget->makeCurrent();
    oglWidget->refreshGPUFromCore(true);
    oglWidget->doneCurrent();
}

// Slot for exporting the mesh to an OBJ file
static bool exportCoreToOBJ(const CoreMesh& core, const QString& filePath)
{
    if (core.empty()) return false;

    return CGAL::IO::write_polygon_mesh(
        filePath.toStdString(),
        core.sm,
        CGAL::parameters::stream_precision(17)
    );
}

// Slot for exporting the mesh to an OBJ file
void RemeshApp::on_actionExport_obj_triggered()
{
    const CoreMesh& core = oglWidget->coreMesh();
    if (core.empty()) return;

    const QString dir = QFileDialog::getExistingDirectory(this, "Оберіть папку для експорту");
    if (dir.isEmpty()) return;

    const QString filePath = QDir(dir).filePath("export.obj");

    if (exportCoreToOBJ(core, filePath)) {
        qDebug() << "Меш експортовано до:" << filePath;
    }
    else {
        qWarning() << "Помилка експорту.";
    }
}
