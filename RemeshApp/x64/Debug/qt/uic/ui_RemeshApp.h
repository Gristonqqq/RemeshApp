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
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "OpenGLWidget.h"

QT_BEGIN_NAMESPACE

class Ui_RemeshAppClass
{
public:
    QAction *action_obj;
    QAction *action_fbx;
    QAction *action_stl;
    QAction *action_obj_2;
    QAction *action_fbx_2;
    QAction *action_stl_2;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_2;
    QFrame *ViewPort;
    QHBoxLayout *horizontalLayout_3;
    OpenGLWidget *openGLWidget;
    QFrame *MenuTab;
    QVBoxLayout *verticalLayout;
    QPushButton *showWire;
    QLabel *label;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QPushButton *remeshTypeBlock;
    QPushButton *remeshTypeSharp;
    QPushButton *remeshTypeVoxel;
    QPushButton *pushButtonRemesh;
    QMenuBar *menuBar;
    QMenu *menuImport;
    QMenu *menuExport;

    void setupUi(QMainWindow *RemeshAppClass)
    {
        if (RemeshAppClass->objectName().isEmpty())
            RemeshAppClass->setObjectName("RemeshAppClass");
        RemeshAppClass->setEnabled(true);
        RemeshAppClass->resize(900, 600);
        RemeshAppClass->setMinimumSize(QSize(900, 600));
        RemeshAppClass->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        RemeshAppClass->setStyleSheet(QString::fromUtf8(""));
        action_obj = new QAction(RemeshAppClass);
        action_obj->setObjectName("action_obj");
        action_fbx = new QAction(RemeshAppClass);
        action_fbx->setObjectName("action_fbx");
        action_stl = new QAction(RemeshAppClass);
        action_stl->setObjectName("action_stl");
        action_obj_2 = new QAction(RemeshAppClass);
        action_obj_2->setObjectName("action_obj_2");
        action_fbx_2 = new QAction(RemeshAppClass);
        action_fbx_2->setObjectName("action_fbx_2");
        action_stl_2 = new QAction(RemeshAppClass);
        action_stl_2->setObjectName("action_stl_2");
        centralWidget = new QWidget(RemeshAppClass);
        centralWidget->setObjectName("centralWidget");
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(0);
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
        horizontalLayout_3 = new QHBoxLayout(ViewPort);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        openGLWidget = new OpenGLWidget(ViewPort);
        openGLWidget->setObjectName("openGLWidget");
        openGLWidget->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_3->addWidget(openGLWidget);


        horizontalLayout_2->addWidget(ViewPort);

        MenuTab = new QFrame(centralWidget);
        MenuTab->setObjectName("MenuTab");
        MenuTab->setEnabled(true);
        MenuTab->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        MenuTab->setAutoFillBackground(false);
        MenuTab->setStyleSheet(QString::fromUtf8("background-color: rgb(42, 42, 42);"));
        MenuTab->setFrameShape(QFrame::Shape::StyledPanel);
        MenuTab->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout = new QVBoxLayout(MenuTab);
        verticalLayout->setSpacing(5);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        showWire = new QPushButton(MenuTab);
        showWire->setObjectName("showWire");
        showWire->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(showWire->sizePolicy().hasHeightForWidth());
        showWire->setSizePolicy(sizePolicy1);
        showWire->setMinimumSize(QSize(0, 30));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush);
        QBrush brush1(QColor(214, 214, 214, 255));
        brush1.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush1);
        QBrush brush2(QColor(255, 255, 255, 255));
        brush2.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush2);
        QBrush brush3(QColor(233, 233, 233, 255));
        brush3.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush3);
        QBrush brush4(QColor(106, 106, 106, 255));
        brush4.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush4);
        QBrush brush5(QColor(141, 141, 141, 255));
        brush5.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush2);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush3);
        QBrush brush6(QColor(255, 255, 220, 255));
        brush6.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush6);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush);
        QBrush brush7(QColor(0, 0, 0, 127));
        brush7.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush7);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush2);
#endif
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush4);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush6);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush7);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush2);
#endif
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush);
        QBrush brush8(QColor(212, 212, 212, 255));
        brush8.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush8);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush6);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush);
        QBrush brush9(QColor(106, 106, 106, 127));
        brush9.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush9);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush2);
#endif
        showWire->setPalette(palette);
        showWire->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        showWire->setStyleSheet(QString::fromUtf8("background-color: rgb(214, 214, 214);\n"
"color: rgb(83, 83, 83);"));
        showWire->setCheckable(false);
        showWire->setChecked(false);

        verticalLayout->addWidget(showWire);

        label = new QLabel(MenuTab);
        label->setObjectName("label");
        label->setMaximumSize(QSize(16777215, 20));
        label->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);

        verticalLayout->addWidget(label);

        frame = new QFrame(MenuTab);
        frame->setObjectName("frame");
        frame->setMaximumSize(QSize(16777215, 30));
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        remeshTypeBlock = new QPushButton(frame);
        remeshTypeBlock->setObjectName("remeshTypeBlock");
        remeshTypeBlock->setMaximumSize(QSize(300, 16777215));
        remeshTypeBlock->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        remeshTypeBlock->setStyleSheet(QString::fromUtf8("border: 1px solid #8f8f91;\n"
"border-top-left-radius: 6px;\n"
"border-top-right-radius: 0px;\n"
"border-bottom-left-radius: 6px;\n"
"border-bottom-right-radius: 0px;"));

        horizontalLayout->addWidget(remeshTypeBlock);

        remeshTypeSharp = new QPushButton(frame);
        remeshTypeSharp->setObjectName("remeshTypeSharp");
        remeshTypeSharp->setMaximumSize(QSize(300, 16777215));
        remeshTypeSharp->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        remeshTypeSharp->setStyleSheet(QString::fromUtf8("border: 1px solid #8f8f91;\n"
"border-radius: 0px;"));

        horizontalLayout->addWidget(remeshTypeSharp);

        remeshTypeVoxel = new QPushButton(frame);
        remeshTypeVoxel->setObjectName("remeshTypeVoxel");
        remeshTypeVoxel->setMaximumSize(QSize(300, 16777215));
        remeshTypeVoxel->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        remeshTypeVoxel->setStyleSheet(QString::fromUtf8("border: 1px solid #8f8f91;\n"
"border-top-left-radius: 0px;\n"
"border-top-right-radius: 6px;\n"
"border-bottom-left-radius: 0px;\n"
"border-bottom-right-radius: 6px;"));

        horizontalLayout->addWidget(remeshTypeVoxel);


        verticalLayout->addWidget(frame);

        pushButtonRemesh = new QPushButton(MenuTab);
        pushButtonRemesh->setObjectName("pushButtonRemesh");
        pushButtonRemesh->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout->addWidget(pushButtonRemesh);


        horizontalLayout_2->addWidget(MenuTab, 0, Qt::AlignmentFlag::AlignTop);

        horizontalLayout_2->setStretch(0, 8);
        horizontalLayout_2->setStretch(1, 2);
        RemeshAppClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(RemeshAppClass);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 900, 21));
        menuImport = new QMenu(menuBar);
        menuImport->setObjectName("menuImport");
        menuExport = new QMenu(menuBar);
        menuExport->setObjectName("menuExport");
        RemeshAppClass->setMenuBar(menuBar);

        menuBar->addAction(menuImport->menuAction());
        menuBar->addAction(menuExport->menuAction());
        menuImport->addAction(action_obj);
        menuImport->addAction(action_fbx);
        menuImport->addAction(action_stl);
        menuExport->addAction(action_obj_2);
        menuExport->addAction(action_fbx_2);
        menuExport->addAction(action_stl_2);

        retranslateUi(RemeshAppClass);

        QMetaObject::connectSlotsByName(RemeshAppClass);
    } // setupUi

    void retranslateUi(QMainWindow *RemeshAppClass)
    {
        RemeshAppClass->setWindowTitle(QCoreApplication::translate("RemeshAppClass", "RemeshApp", nullptr));
        action_obj->setText(QCoreApplication::translate("RemeshAppClass", ".obj", nullptr));
        action_fbx->setText(QCoreApplication::translate("RemeshAppClass", ".fbx", nullptr));
        action_stl->setText(QCoreApplication::translate("RemeshAppClass", ".stl", nullptr));
        action_obj_2->setText(QCoreApplication::translate("RemeshAppClass", ".obj", nullptr));
        action_fbx_2->setText(QCoreApplication::translate("RemeshAppClass", ".fbx", nullptr));
        action_stl_2->setText(QCoreApplication::translate("RemeshAppClass", ".stl", nullptr));
        showWire->setText(QCoreApplication::translate("RemeshAppClass", "Show wireframe", nullptr));
        label->setText(QCoreApplication::translate("RemeshAppClass", "Remesh algorithm", nullptr));
        remeshTypeBlock->setText(QCoreApplication::translate("RemeshAppClass", "Block", nullptr));
        remeshTypeSharp->setText(QCoreApplication::translate("RemeshAppClass", "Sharp", nullptr));
        remeshTypeVoxel->setText(QCoreApplication::translate("RemeshAppClass", "Voxel", nullptr));
        pushButtonRemesh->setText(QCoreApplication::translate("RemeshAppClass", "Remesh", nullptr));
        menuImport->setTitle(QCoreApplication::translate("RemeshAppClass", "Import", nullptr));
        menuExport->setTitle(QCoreApplication::translate("RemeshAppClass", "Export", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RemeshAppClass: public Ui_RemeshAppClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REMESHAPP_H
