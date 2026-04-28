#include "RemeshApp.h"

#include "OpenGLWidget.h"

#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QSet>
#include <qstyle.h>
#include <QStyleFactory>
#include <QTimer>
#include <QResizeEvent>

#include <CGAL/IO/polygon_mesh_io.h>

#include "DecimateStrategy.h"
#include "SubdivideStrategy.h"
#include "IsotropicRemeshStrategy.h"
#include "IRemeshStrategy.h"
#include "RemeshParams.h"
#include <QActionGroup>


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
static void setSelectedProp(QWidget* w, bool selected)
{
    w->setProperty("selected", selected);
    w->style()->unpolish(w);
    w->style()->polish(w);
    w->update();
}

// Update algorithm buttons UI based on selected algorithm
void RemeshApp::updateAlgorithmButtonsUI()
{
    setSelectedProp(ui->btnDecimate, m_selectedAlgo == RemeshAlgo::Decimate);
    setSelectedProp(ui->btnSubdivide, m_selectedAlgo == RemeshAlgo::Subdivide);
    setSelectedProp(ui->btnIsotropic, m_selectedAlgo == RemeshAlgo::Isotropic);
}

// Update wireframe button UI based on current state
void RemeshApp::clearInlineStyleSheets()
{
    const auto widgets = this->findChildren<QWidget*>();
    for (QWidget* w : widgets) {
        if (!w) continue;

        if (!w->styleSheet().isEmpty())
            w->setStyleSheet(QString());
    }
}

// Helper to load QSS content from a file
QString RemeshApp::loadQss(const QString& path) const
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open QSS:" << path << f.errorString();
        return {};
    }
    return QString::fromUtf8(f.readAll());
}

// Build combined QSS for the selected theme
QString RemeshApp::buildThemeQss(Theme t) const
{
    if (t == Theme::Dark) {
        return loadQss(":/themes/dark/widgets.qss") + "\n" +
            loadQss(":/themes/dark/checkbox.qss");
    }
    else {
        return loadQss(":/themes/light/widgets.qss") + "\n" +
            loadQss(":/themes/light/checkbox.qss");
    }
}

// Apply dark theme by loading and combining QSS files
void RemeshApp::setTheme(Theme t)
{
    m_theme = t;

    clearInlineStyleSheets();

    const QString qss = buildThemeQss(t);

    qApp->setStyleSheet(QString());
    qApp->setStyleSheet(qss);

    if (oglWidget) {
        oglWidget->setTheme(m_theme == Theme::Dark
            ? OpenGLWidget::Theme::Dark
            : OpenGLWidget::Theme::Light);
    }

    ui->actionDarkTheme->setChecked(t == Theme::Dark);
    ui->actionLightTheme->setChecked(t == Theme::Light);
}

// Toggle between dark and light themes
void RemeshApp::toggleTheme()
{
    setTheme(m_theme == Theme::Dark ? Theme::Light : Theme::Dark);
}

// Constructor
RemeshApp::RemeshApp(QWidget* parent)
    : QMainWindow(parent)
{
    ui = new Ui::RemeshAppClass();
    ui->setupUi(this);


    QTimer::singleShot(0, this, [this]() {
        welcomeOverlay = new WelcomeOverlay(ui->centralWidget);
        welcomeOverlay->setFixedSize(500, 500);

        const int x = (ui->centralWidget->width() - welcomeOverlay->width()) / 2;
        const int y = (ui->centralWidget->height() - welcomeOverlay->height()) / 2;

        welcomeOverlay->move(x, y);
        welcomeOverlay->raise();
        welcomeOverlay->show();
        });

    //qApp->setStyle(QStyleFactory::create("Fusion"));

    ui->actionDarkTheme->setCheckable(true);
    ui->actionLightTheme->setCheckable(true);

    auto* themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);
    themeGroup->addAction(ui->actionDarkTheme);
    themeGroup->addAction(ui->actionLightTheme);

    connect(ui->actionDarkTheme, &QAction::triggered, this, [this]() {
        setTheme(Theme::Dark);
        });
    connect(ui->actionLightTheme, &QAction::triggered, this, [this]() {
        setTheme(Theme::Light);
        });

    setTheme(Theme::Dark);

    m_baseBtnStyle[ui->btnDecimate] = ui->btnDecimate->styleSheet();
    m_baseBtnStyle[ui->btnSubdivide] = ui->btnSubdivide->styleSheet();
    m_baseBtnStyle[ui->btnIsotropic] = ui->btnIsotropic->styleSheet();

    oglWidget = ui->openGLWidget;

    updateWireframeButtonUI();

    ui->pushButtonRemesh->setEnabled(false);


    connect(oglWidget, &OpenGLWidget::glInitialized, this, [this]() {
        if (!pendingModelPath.isEmpty()) {
            oglWidget->makeCurrent();
            oglWidget->loadModel(pendingModelPath);
            oglWidget->doneCurrent();
            pendingModelPath.clear();
        }
        });

    connect(ui->showWireframeButton, &QPushButton::clicked, this, [this]() {
        if (!oglWidget) return;

        oglWidget->showWireframe = !oglWidget->showWireframe;
        oglWidget->update();

        updateWireframeButtonUI();  // <-- тільки оновлення property + polish
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

// Update wireframe button UI based on current state
void RemeshApp::updateWireframeButtonUI()
{
    const bool on = (oglWidget && oglWidget->showWireframe);

    ui->showWireframeButton->setProperty("toggled", on);

    ui->showWireframeButton->style()->unpolish(ui->showWireframeButton);
    ui->showWireframeButton->style()->polish(ui->showWireframeButton);
    ui->showWireframeButton->update();
}

// Override resize event to keep welcome overlay centered
void RemeshApp::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    if (welcomeOverlay && welcomeOverlay->isVisible()) {
        const int x = (ui->centralWidget->width() - welcomeOverlay->width()) / 2;
        const int y = (ui->centralWidget->height() - welcomeOverlay->height()) / 2;
        welcomeOverlay->move(x, y);
    }
}