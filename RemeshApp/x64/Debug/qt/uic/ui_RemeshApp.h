/********************************************************************************
** Form generated from reading UI file 'RemeshApp.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REMESHAPP_H
#define UI_REMESHAPP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RemeshAppClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RemeshAppClass)
    {
        if (RemeshAppClass->objectName().isEmpty())
            RemeshAppClass->setObjectName("RemeshAppClass");
        RemeshAppClass->resize(600, 400);
        menuBar = new QMenuBar(RemeshAppClass);
        menuBar->setObjectName("menuBar");
        RemeshAppClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(RemeshAppClass);
        mainToolBar->setObjectName("mainToolBar");
        RemeshAppClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(RemeshAppClass);
        centralWidget->setObjectName("centralWidget");
        RemeshAppClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(RemeshAppClass);
        statusBar->setObjectName("statusBar");
        RemeshAppClass->setStatusBar(statusBar);

        retranslateUi(RemeshAppClass);

        QMetaObject::connectSlotsByName(RemeshAppClass);
    } // setupUi

    void retranslateUi(QMainWindow *RemeshAppClass)
    {
        RemeshAppClass->setWindowTitle(QCoreApplication::translate("RemeshAppClass", "RemeshApp", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RemeshAppClass: public Ui_RemeshAppClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REMESHAPP_H
