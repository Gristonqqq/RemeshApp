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
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RemeshAppClass
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_2;
    QFrame *ViewPort;
    QFrame *MenuTab;

    void setupUi(QMainWindow *RemeshAppClass)
    {
        if (RemeshAppClass->objectName().isEmpty())
            RemeshAppClass->setObjectName("RemeshAppClass");
        RemeshAppClass->resize(846, 531);
        centralWidget = new QWidget(RemeshAppClass);
        centralWidget->setObjectName("centralWidget");
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(1, 1, 1, 1);
        ViewPort = new QFrame(centralWidget);
        ViewPort->setObjectName("ViewPort");
        ViewPort->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ViewPort->sizePolicy().hasHeightForWidth());
        ViewPort->setSizePolicy(sizePolicy);
        ViewPort->setStyleSheet(QString::fromUtf8("background-color: rgb(211, 211, 211);"));
        ViewPort->setFrameShape(QFrame::Shape::StyledPanel);
        ViewPort->setFrameShadow(QFrame::Shadow::Raised);

        horizontalLayout_2->addWidget(ViewPort);

        MenuTab = new QFrame(centralWidget);
        MenuTab->setObjectName("MenuTab");
        MenuTab->setStyleSheet(QString::fromUtf8("background-color: rgb(131, 131, 131);"));
        MenuTab->setFrameShape(QFrame::Shape::StyledPanel);
        MenuTab->setFrameShadow(QFrame::Shadow::Raised);

        horizontalLayout_2->addWidget(MenuTab);

        horizontalLayout_2->setStretch(0, 8);
        horizontalLayout_2->setStretch(1, 2);
        RemeshAppClass->setCentralWidget(centralWidget);

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
