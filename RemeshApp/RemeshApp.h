#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RemeshApp.h"
#include <QHash>
#include <QPushButton>


class OpenGLWidget;

enum class RemeshAlgo
{
    Decimate,
    Subdivide,
    Isotropic
};

class RemeshApp : public QMainWindow
{
    Q_OBJECT
public:
    RemeshApp(QWidget* parent = nullptr);
    ~RemeshApp() override;

    QString pendingModelPath;

private:
    Ui::RemeshAppClass* ui = nullptr;
    OpenGLWidget* oglWidget = nullptr;
    QHash<QPushButton*, QString> m_baseBtnStyle;

    RemeshAlgo m_selectedAlgo = RemeshAlgo::Isotropic;

    void setSelectedAlgorithm(RemeshAlgo algo);
    void updateAlgorithmButtonsUI();
    void setButtonSelected(QPushButton* btn, bool selected, const QString& selectedColorCss);
private slots:
    void on_actionImport_obj_triggered();
    void onRemeshButtonClicked();
    void on_actionExport_obj_triggered();
};
