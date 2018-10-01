/********************************************************************************
** Form generated from reading UI file 'gui2.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI2_H
#define UI_GUI2_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_gui2
{
public:
    QAction *actionImport;
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionScale_builder;
    QAction *actionRandrp;
    QAction *actionmetg_builder;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QMdiArea *mdiArea;
    QWidget *subwindow;
    QVBoxLayout *verticalLayout_3;
    QPlainTextEdit *plainTextEdit;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton;
    QPushButton *dump;
    QWidget *subwindow_2;
    QVBoxLayout *verticalLayout_2;
    QListWidget *gdata_list;
    QHBoxLayout *horizontalLayout;
    QPushButton *update;
    QPushButton *pushButton_2;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuTools;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *gui2)
    {
        if (gui2->objectName().isEmpty())
            gui2->setObjectName(QStringLiteral("gui2"));
        gui2->resize(1137, 737);
        actionImport = new QAction(gui2);
        actionImport->setObjectName(QStringLiteral("actionImport"));
        actionOpen = new QAction(gui2);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionExit = new QAction(gui2);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionScale_builder = new QAction(gui2);
        actionScale_builder->setObjectName(QStringLiteral("actionScale_builder"));
        actionRandrp = new QAction(gui2);
        actionRandrp->setObjectName(QStringLiteral("actionRandrp"));
        actionmetg_builder = new QAction(gui2);
        actionmetg_builder->setObjectName(QStringLiteral("actionmetg_builder"));
        centralwidget = new QWidget(gui2);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        mdiArea = new QMdiArea(centralwidget);
        mdiArea->setObjectName(QStringLiteral("mdiArea"));
        subwindow = new QWidget();
        subwindow->setObjectName(QStringLiteral("subwindow"));
        verticalLayout_3 = new QVBoxLayout(subwindow);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        plainTextEdit = new QPlainTextEdit(subwindow);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));

        verticalLayout_3->addWidget(plainTextEdit);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        pushButton = new QPushButton(subwindow);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_2->addWidget(pushButton);

        dump = new QPushButton(subwindow);
        dump->setObjectName(QStringLiteral("dump"));

        horizontalLayout_2->addWidget(dump);


        verticalLayout_3->addLayout(horizontalLayout_2);

        mdiArea->addSubWindow(subwindow);
        subwindow_2 = new QWidget();
        subwindow_2->setObjectName(QStringLiteral("subwindow_2"));
        verticalLayout_2 = new QVBoxLayout(subwindow_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        gdata_list = new QListWidget(subwindow_2);
        gdata_list->setObjectName(QStringLiteral("gdata_list"));

        verticalLayout_2->addWidget(gdata_list);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        update = new QPushButton(subwindow_2);
        update->setObjectName(QStringLiteral("update"));

        horizontalLayout->addWidget(update);

        pushButton_2 = new QPushButton(subwindow_2);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        horizontalLayout->addWidget(pushButton_2);


        verticalLayout_2->addLayout(horizontalLayout);

        mdiArea->addSubWindow(subwindow_2);

        verticalLayout->addWidget(mdiArea);

        gui2->setCentralWidget(centralwidget);
        menubar = new QMenuBar(gui2);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1137, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuTools = new QMenu(menubar);
        menuTools->setObjectName(QStringLiteral("menuTools"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        gui2->setMenuBar(menubar);
        statusbar = new QStatusBar(gui2);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        gui2->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuTools->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionImport);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionExit);
        menuTools->addAction(actionScale_builder);
        menuTools->addAction(actionRandrp);
        menuTools->addAction(actionmetg_builder);

        retranslateUi(gui2);
        QObject::connect(update, SIGNAL(clicked()), gui2, SLOT(update()));

        QMetaObject::connectSlotsByName(gui2);
    } // setupUi

    void retranslateUi(QMainWindow *gui2)
    {
        gui2->setWindowTitle(QApplication::translate("gui2", "au", nullptr));
        actionImport->setText(QApplication::translate("gui2", "Import", nullptr));
        actionOpen->setText(QApplication::translate("gui2", "Open", nullptr));
        actionExit->setText(QApplication::translate("gui2", "Exit", nullptr));
        actionScale_builder->setText(QApplication::translate("gui2", "Scale builder", nullptr));
        actionRandrp->setText(QApplication::translate("gui2", "rand_rp()", nullptr));
        actionmetg_builder->setText(QApplication::translate("gui2", "metg builder", nullptr));
        subwindow->setWindowTitle(QApplication::translate("gui2", "Subwindow", nullptr));
        pushButton->setText(QApplication::translate("gui2", "PushButton", nullptr));
        dump->setText(QApplication::translate("gui2", "dump", nullptr));
        subwindow_2->setWindowTitle(QApplication::translate("gui2", "Subwindow", nullptr));
        update->setText(QApplication::translate("gui2", "update", nullptr));
        pushButton_2->setText(QApplication::translate("gui2", "PushButton", nullptr));
        menuFile->setTitle(QApplication::translate("gui2", "File", nullptr));
        menuView->setTitle(QApplication::translate("gui2", "View", nullptr));
        menuTools->setTitle(QApplication::translate("gui2", "Tools", nullptr));
        menuHelp->setTitle(QApplication::translate("gui2", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class gui2: public Ui_gui2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI2_H
