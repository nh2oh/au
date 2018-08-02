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
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_guiClass
{
public:
    QWidget *centralWidget;
    QPushButton *btn_make;
    QPlainTextEdit *output;
    QLineEdit *command;
    QPushButton *btn_clear;
    QPushButton *btn_list;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *guiClass)
    {
        if (guiClass->objectName().isEmpty())
            guiClass->setObjectName(QStringLiteral("guiClass"));
        guiClass->resize(1054, 718);
        centralWidget = new QWidget(guiClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        btn_make = new QPushButton(centralWidget);
        btn_make->setObjectName(QStringLiteral("btn_make"));
        btn_make->setGeometry(QRect(600, 120, 81, 31));
        output = new QPlainTextEdit(centralWidget);
        output->setObjectName(QStringLiteral("output"));
        output->setGeometry(QRect(40, 170, 521, 411));
        command = new QLineEdit(centralWidget);
        command->setObjectName(QStringLiteral("command"));
        command->setGeometry(QRect(40, 120, 521, 31));
        btn_clear = new QPushButton(centralWidget);
        btn_clear->setObjectName(QStringLiteral("btn_clear"));
        btn_clear->setGeometry(QRect(50, 590, 75, 23));
        btn_list = new QPushButton(centralWidget);
        btn_list->setObjectName(QStringLiteral("btn_list"));
        btn_list->setGeometry(QRect(740, 120, 91, 31));
        guiClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(guiClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1054, 18));
        guiClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(guiClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        guiClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(guiClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        guiClass->setStatusBar(statusBar);

        retranslateUi(guiClass);
        QObject::connect(btn_clear, SIGNAL(clicked()), output, SLOT(clear()));

        QMetaObject::connectSlotsByName(guiClass);
    } // setupUi

    void retranslateUi(QMainWindow *guiClass)
    {
        guiClass->setWindowTitle(QApplication::translate("guiClass", "gui", nullptr));
        btn_make->setText(QApplication::translate("guiClass", "make", nullptr));
        btn_clear->setText(QApplication::translate("guiClass", "clear", nullptr));
        btn_list->setText(QApplication::translate("guiClass", "list existing", nullptr));
    } // retranslateUi

};

namespace Ui {
    class guiClass: public Ui_guiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_H
