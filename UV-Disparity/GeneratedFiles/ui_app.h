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
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AppClass
{
public:

    void setupUi(QWidget *AppClass)
    {
        if (AppClass->objectName().isEmpty())
            AppClass->setObjectName(QStringLiteral("AppClass"));
        AppClass->resize(600, 400);

        retranslateUi(AppClass);

        QMetaObject::connectSlotsByName(AppClass);
    } // setupUi

    void retranslateUi(QWidget *AppClass)
    {
        AppClass->setWindowTitle(QApplication::translate("AppClass", "App", 0));
    } // retranslateUi

};

namespace Ui {
    class AppClass: public Ui_AppClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APP_H
