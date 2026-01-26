#include "RemeshApp.h"
#include <QtWidgets/QApplication>
#include "OpenGLWidget.h"
#include "LogManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    LogManager::instance().init();
    qInfo() << "App started. Log file:" << LogManager::instance().logFilePath();
    
    RemeshApp window;
    window.setWindowState(Qt::WindowMaximized);
    window.show();
    return app.exec();
}
