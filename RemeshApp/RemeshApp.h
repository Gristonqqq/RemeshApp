#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RemeshApp.h"

class OpenGLWidget;

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

private slots:
    void on_actionImport_obj_triggered();
    void onRemeshButtonClicked();
    void on_actionExport_obj_triggered();
};
