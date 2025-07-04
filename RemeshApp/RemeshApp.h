#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RemeshApp.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class RemeshApp : public QMainWindow
{
    Q_OBJECT

public:
    RemeshApp(QWidget *parent = nullptr);
    ~RemeshApp();
    QString pendingModelPath;
private:
    Ui::RemeshAppClass* ui;
    OpenGLWidget* oglWidget;
    bool oglWidgetReady = false;
private slots:
    void on_actionImport_obj_triggered();
    //void on_actionImport_fbx_triggered();
    //void on_actionImport_stl_triggered();
};

