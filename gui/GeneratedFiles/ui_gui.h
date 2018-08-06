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
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
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
    QAction *actionFonts_and_colors;
    QAction *actionScale_Builder;
    QAction *actionOpen;
    QAction *actionImport;
    QWidget *centralWidget;
    QPushButton *btn_make;
    QPlainTextEdit *output;
    QLineEdit *command;
    QPushButton *btn_clear;
    QPushButton *btn_list;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuTools;
    QMenu *menuHelp;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *guiClass)
    {
        if (guiClass->objectName().isEmpty())
            guiClass->setObjectName(QStringLiteral("guiClass"));
        guiClass->resize(1054, 718);
        actionFonts_and_colors = new QAction(guiClass);
        actionFonts_and_colors->setObjectName(QStringLiteral("actionFonts_and_colors"));
        actionScale_Builder = new QAction(guiClass);
        actionScale_Builder->setObjectName(QStringLiteral("actionScale_Builder"));
        actionOpen = new QAction(guiClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionImport = new QAction(guiClass);
        actionImport->setObjectName(QStringLiteral("actionImport"));
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
        menuBar->setGeometry(QRect(0, 0, 1054, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QStringLiteral("menuTools"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        guiClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(guiClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        guiClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(guiClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        guiClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionImport);
        menuView->addAction(actionFonts_and_colors);
        menuTools->addAction(actionScale_Builder);

        retranslateUi(guiClass);
        QObject::connect(btn_clear, SIGNAL(clicked()), output, SLOT(clear()));

        QMetaObject::connectSlotsByName(guiClass);
    } // setupUi

    void retranslateUi(QMainWindow *guiClass)
    {
        guiClass->setWindowTitle(QApplication::translate("guiClass", "gui", nullptr));
        actionFonts_and_colors->setText(QApplication::translate("guiClass", "Fonts and colors", nullptr));
        actionScale_Builder->setText(QApplication::translate("guiClass", "Scale Builder", nullptr));
        actionOpen->setText(QApplication::translate("guiClass", "Open", nullptr));
        actionImport->setText(QApplication::translate("guiClass", "Import", nullptr));
        btn_make->setText(QApplication::translate("guiClass", "make", nullptr));
        btn_clear->setText(QApplication::translate("guiClass", "clear", nullptr));
        btn_list->setText(QApplication::translate("guiClass", "list existing", nullptr));
        menuFile->setTitle(QApplication::translate("guiClass", "File", nullptr));
        menuView->setTitle(QApplication::translate("guiClass", "View", nullptr));
        menuTools->setTitle(QApplication::translate("guiClass", "Tools", nullptr));
        menuHelp->setTitle(QApplication::translate("guiClass", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class guiClass: public Ui_guiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_H
