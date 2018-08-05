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
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
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
    QListView *listView;
    QHBoxLayout *horizontalLayout;
    QPushButton *update;
    QPushButton *pushButton_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *gui2)
    {
        if (gui2->objectName().isEmpty())
            gui2->setObjectName(QStringLiteral("gui2"));
        gui2->resize(767, 556);
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
        listView = new QListView(subwindow_2);
        listView->setObjectName(QStringLiteral("listView"));

        verticalLayout_2->addWidget(listView);

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
        menubar->setGeometry(QRect(0, 0, 767, 18));
        gui2->setMenuBar(menubar);
        statusbar = new QStatusBar(gui2);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        gui2->setStatusBar(statusbar);

        retranslateUi(gui2);

        QMetaObject::connectSlotsByName(gui2);
    } // setupUi

    void retranslateUi(QMainWindow *gui2)
    {
        gui2->setWindowTitle(QApplication::translate("gui2", "MainWindow", nullptr));
        subwindow->setWindowTitle(QApplication::translate("gui2", "Subwindow", nullptr));
        pushButton->setText(QApplication::translate("gui2", "PushButton", nullptr));
        dump->setText(QApplication::translate("gui2", "dump", nullptr));
        subwindow_2->setWindowTitle(QApplication::translate("gui2", "Subwindow", nullptr));
        update->setText(QApplication::translate("gui2", "update", nullptr));
        pushButton_2->setText(QApplication::translate("gui2", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class gui2: public Ui_gui2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI2_H
