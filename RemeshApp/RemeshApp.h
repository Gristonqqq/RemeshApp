#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RemeshApp.h"
#include <QHash>
#include <QPushButton>
#include "WelcomeOverlay.h"


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
    enum class Theme { Dark, Light };

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
    void updateWireframeButtonUI();

    void clearInlineStyleSheets();

    QString loadQss(const QString& path) const;
    QString buildThemeQss(Theme t) const;
    Theme m_theme = Theme::Dark;

    WelcomeOverlay* welcomeOverlay = nullptr;
private slots:
    void on_actionImport_obj_triggered();
    void onRemeshButtonClicked();
    void on_actionExport_obj_triggered();

    void setTheme(Theme t);
    void toggleTheme();
protected:
    void resizeEvent(QResizeEvent* event) override;
};
