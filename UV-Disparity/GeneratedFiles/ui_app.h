/********************************************************************************
** Form generated from reading UI file 'app.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APP_H
#define UI_APP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AppClass
{
public:
    QAction *actionV_Disparity;
    QAction *actionU_Disparity;
    QAction *actionHoughLineV_Disp;
    QAction *actionPHoughLineV_Disp;
    QAction *actionCudaHoughLineV_Disp;
    QAction *actionCudaPHoughLineV_Disp;
    QAction *action_Load_mask;
    QAction *action_Load_source;
    QAction *action_Remove_mask;
    QAction *action_Load_disparity;
    QAction *actionLoad_directory;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menuRun;

    void setupUi(QMainWindow *AppClass)
    {
        if (AppClass->objectName().isEmpty())
            AppClass->setObjectName(QStringLiteral("AppClass"));
        AppClass->resize(600, 400);
        actionV_Disparity = new QAction(AppClass);
        actionV_Disparity->setObjectName(QStringLiteral("actionV_Disparity"));
        actionU_Disparity = new QAction(AppClass);
        actionU_Disparity->setObjectName(QStringLiteral("actionU_Disparity"));
        actionHoughLineV_Disp = new QAction(AppClass);
        actionHoughLineV_Disp->setObjectName(QStringLiteral("actionHoughLineV_Disp"));
        actionPHoughLineV_Disp = new QAction(AppClass);
        actionPHoughLineV_Disp->setObjectName(QStringLiteral("actionPHoughLineV_Disp"));
        actionCudaHoughLineV_Disp = new QAction(AppClass);
        actionCudaHoughLineV_Disp->setObjectName(QStringLiteral("actionCudaHoughLineV_Disp"));
        actionCudaPHoughLineV_Disp = new QAction(AppClass);
        actionCudaPHoughLineV_Disp->setObjectName(QStringLiteral("actionCudaPHoughLineV_Disp"));
        action_Load_mask = new QAction(AppClass);
        action_Load_mask->setObjectName(QStringLiteral("action_Load_mask"));
        action_Load_source = new QAction(AppClass);
        action_Load_source->setObjectName(QStringLiteral("action_Load_source"));
        action_Remove_mask = new QAction(AppClass);
        action_Remove_mask->setObjectName(QStringLiteral("action_Remove_mask"));
        action_Load_disparity = new QAction(AppClass);
        action_Load_disparity->setObjectName(QStringLiteral("action_Load_disparity"));
        actionLoad_directory = new QAction(AppClass);
        actionLoad_directory->setObjectName(QStringLiteral("actionLoad_directory"));
        centralWidget = new QWidget(AppClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        AppClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(AppClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 21));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menuRun = new QMenu(menuBar);
        menuRun->setObjectName(QStringLiteral("menuRun"));
        AppClass->setMenuBar(menuBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menuRun->menuAction());
        menu_File->addAction(action_Load_disparity);
        menu_File->addAction(action_Load_mask);
        menu_File->addAction(action_Load_source);
        menu_File->addAction(action_Remove_mask);
        menu_File->addAction(actionLoad_directory);
        menuRun->addAction(actionV_Disparity);
        menuRun->addAction(actionU_Disparity);
        menuRun->addAction(actionHoughLineV_Disp);
        menuRun->addAction(actionPHoughLineV_Disp);
        menuRun->addAction(actionCudaHoughLineV_Disp);
        menuRun->addAction(actionCudaPHoughLineV_Disp);

        retranslateUi(AppClass);

        QMetaObject::connectSlotsByName(AppClass);
    } // setupUi

    void retranslateUi(QMainWindow *AppClass)
    {
        AppClass->setWindowTitle(QApplication::translate("AppClass", "UV-Disparity", 0));
        actionV_Disparity->setText(QApplication::translate("AppClass", "V-Disparity", 0));
        actionU_Disparity->setText(QApplication::translate("AppClass", "U-Disparity", 0));
        actionHoughLineV_Disp->setText(QApplication::translate("AppClass", "HoughLineV_Disp", 0));
        actionPHoughLineV_Disp->setText(QApplication::translate("AppClass", "PHoughLineV_Disp", 0));
        actionCudaHoughLineV_Disp->setText(QApplication::translate("AppClass", "CudaHoughLineV_Disp", 0));
        actionCudaPHoughLineV_Disp->setText(QApplication::translate("AppClass", "CudaPHoughLineV_Disp", 0));
        action_Load_mask->setText(QApplication::translate("AppClass", "&Load mask", 0));
        action_Load_source->setText(QApplication::translate("AppClass", "&Load source", 0));
        action_Remove_mask->setText(QApplication::translate("AppClass", "&Remove mask", 0));
        action_Load_disparity->setText(QApplication::translate("AppClass", "&Load disparity", 0));
        actionLoad_directory->setText(QApplication::translate("AppClass", "Load directory", 0));
        menu_File->setTitle(QApplication::translate("AppClass", "&File", 0));
        menuRun->setTitle(QApplication::translate("AppClass", "Run", 0));
    } // retranslateUi

};

namespace Ui {
    class AppClass: public Ui_AppClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APP_H
