#include "RemeshApp.h"

#include "OpenGLWidget.h"

#include <QFileDialog>
#include <QDebug>
#include <QDir>

#include <CGAL/IO/polygon_mesh_io.h>

#include "DecimateStrategy.h"
#include "SubdivideStrategy.h"
#include "IsotropicRemeshStrategy.h"
#include "IRemeshStrategy.h"
#include "RemeshParams.h"

// Helper to apply background color to existing style
static QString applyBgColorToStyle(const QString& baseStyle, const QString& bgCss)
{
    return baseStyle + "\n" + bgCss;
}

// Set selected algorithm and update UI
void RemeshApp::setSelectedAlgorithm(RemeshAlgo algo)
{
    m_selectedAlgo = algo;
    updateAlgorithmButtonsUI();
}

// Set button selected state
void RemeshApp::setButtonSelected(QPushButton* btn, bool selected, const QString& selectedColorCss)
{
    const QString base = m_baseBtnStyle.value(btn);

    if (!selected) {
        btn->setStyleSheet(base);
        return;
    }
    btn->setStyleSheet(applyBgColorToStyle(base, selectedColorCss + " color: white;"));
}

// Update algorithm buttons UI
void RemeshApp::updateAlgorithmButtonsUI()
{
    setButtonSelected(ui->btnDecimate,  m_selectedAlgo == RemeshAlgo::Decimate,  "background-color: rgb(52,152,219);");
    setButtonSelected(ui->btnSubdivide, m_selectedAlgo == RemeshAlgo::Subdivide, "background-color: rgb(52,152,219);");
    setButtonSelected(ui->btnIsotropic, m_selectedAlgo == RemeshAlgo::Isotropic, "background-color: rgb(52,152,219);");
}

// Constructor
RemeshApp::RemeshApp(QWidget* parent)
    : QMainWindow(parent)
{
    ui = new Ui::RemeshAppClass();
    ui->setupUi(this);

    m_baseBtnStyle[ui->btnDecimate] = ui->btnDecimate->styleSheet();
    m_baseBtnStyle[ui->btnSubdivide] = ui->btnSubdivide->styleSheet();
    m_baseBtnStyle[ui->btnIsotropic] = ui->btnIsotropic->styleSheet();

    oglWidget = ui->openGLWidget;

    ui->pushButtonRemesh->setEnabled(false);


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
            ui->showWireframeButton->setStyleSheet("background-color: rgb(52,152,219); color: white;");
        else
            ui->showWireframeButton->setStyleSheet("background-color: rgb(214, 214, 214); color: rgb(83, 83, 83); ");
        });

    connect(ui->pushButtonRemesh, &QPushButton::clicked, this, &RemeshApp::onRemeshButtonClicked);
    
    connect(ui->btnDecimate, &QPushButton::clicked, this, [this]() {
        setSelectedAlgorithm(RemeshAlgo::Decimate);
        ui->stackedWidgeParameters->setCurrentIndex(2);
        });
    connect(ui->btnSubdivide, &QPushButton::clicked, this, [this]() {
        setSelectedAlgorithm(RemeshAlgo::Subdivide);
        ui->stackedWidgeParameters->setCurrentIndex(1);
        });
    connect(ui->btnIsotropic, &QPushButton::clicked, this, [this]() {
        setSelectedAlgorithm(RemeshAlgo::Isotropic);
        ui->stackedWidgeParameters->setCurrentIndex(0);
        });
    setSelectedAlgorithm(RemeshAlgo::Isotropic);
    ui->stackedWidgeParameters->setCurrentIndex(0);
    updateAlgorithmButtonsUI();
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

    ui->pushButtonRemesh->setEnabled(true);
}

// Slot for remeshing the loaded mesh
void RemeshApp::onRemeshButtonClicked()
{
    CoreMesh& core = oglWidget->coreMesh();
    if (core.empty()) {
        qWarning() << "No mesh loaded (core is empty).";
        return;
    }

    RemeshParams params;
    params.targetEdgeLength = ui->SpinBoxRes->value();
    params.subdivLevels = ui->SpinBox_Subd_level->value();
    params.decimateRatio = ui->SpinBox_decim_ratio->value();

    qDebug() << "Remesh button clicked. algo =" << int(m_selectedAlgo)
        << " targetEdgeLength =" << params.targetEdgeLength;

    core.sanitizeAndTriangulate();

    qDebug() << "Before remesh verts:" << core.sm.number_of_vertices()
        << " faces:" << core.sm.number_of_faces();

    bool ok = false;
    QString err;

    switch (m_selectedAlgo)
    {
    case RemeshAlgo::Decimate: {
        DecimateStrategy s;
        ok = s.run(core, params, &err);
        break;
    }
    case RemeshAlgo::Subdivide: {
        SubdivideStrategy s;
        ok = s.run(core, params, &err);
        break;
    }
    case RemeshAlgo::Isotropic:
    default: {
        IsotropicRemeshStrategy s;
        ok = s.run(core, params, &err);
        break;
    }
    }

    qDebug() << "After remesh verts:" << core.sm.number_of_vertices()
        << " faces:" << core.sm.number_of_faces();

    if (!ok) {
        qWarning() << "Remesh failed:" << err;
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
