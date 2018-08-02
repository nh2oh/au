/********************************************************************************
** Form generated from reading UI file 'gui.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_H
#define UI_GUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_guiClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *guiClass)
    {
        if (guiClass->objectName().isEmpty())
            guiClass->setObjectName(QStringLiteral("guiClass"));
        guiClass->resize(600, 400);
        menuBar = new QMenuBar(guiClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        guiClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(guiClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        guiClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(guiClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        guiClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(guiClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        guiClass->setStatusBar(statusBar);

        retranslateUi(guiClass);

        QMetaObject::connectSlotsByName(guiClass);
    } // setupUi

    void retranslateUi(QMainWindow *guiClass)
    {
        guiClass->setWindowTitle(QApplication::translate("guiClass", "gui", nullptr));
    } // retranslateUi

};

namespace Ui {
    class guiClass: public Ui_guiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_H
