#include "RemeshApp.h"
#include <QtWidgets/QApplication>
#include "OpenGLWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    RemeshApp window;
    window.show();
    return app.exec();
}
