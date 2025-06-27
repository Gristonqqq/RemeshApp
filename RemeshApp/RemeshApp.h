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

private:
    Ui::RemeshAppClass ui;
};

