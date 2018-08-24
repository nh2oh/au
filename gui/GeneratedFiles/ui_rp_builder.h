/********************************************************************************
** Form generated from reading UI file 'rp_builder.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RP_BUILDER_H
#define UI_RP_BUILDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_rp_builder
{
public:
    QWidget *centralwidget;
    QLineEdit *ts;
    QLabel *label;
    QLineEdit *nv;
    QLabel *label_2;
    QPushButton *add_nv;
    QPushButton *remove_nv;
    QPushButton *import_2;
    QPushButton *cancel;
    QLineEdit *nnts;
    QLabel *label_3;
    QLineEdit *nbars;
    QLabel *label_4;
    QPlainTextEdit *rp_result;
    QPushButton *generate;
    QPushButton *halt;
    QListView *nvpool;
    QListView *nvpool_addnl;
    QListView *nvprobs;
    QLabel *rand_rp_status_msg;
    QMenuBar *menubar;
    QMenu *menuLoad_template;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *rp_builder)
    {
        if (rp_builder->objectName().isEmpty())
            rp_builder->setObjectName(QStringLiteral("rp_builder"));
        rp_builder->resize(800, 385);
        centralwidget = new QWidget(rp_builder);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        ts = new QLineEdit(centralwidget);
        ts->setObjectName(QStringLiteral("ts"));
        ts->setGeometry(QRect(50, 20, 51, 20));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 21, 16));
        nv = new QLineEdit(centralwidget);
        nv->setObjectName(QStringLiteral("nv"));
        nv->setGeometry(QRect(20, 80, 61, 20));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 60, 47, 14));
        add_nv = new QPushButton(centralwidget);
        add_nv->setObjectName(QStringLiteral("add_nv"));
        add_nv->setGeometry(QRect(90, 130, 31, 23));
        remove_nv = new QPushButton(centralwidget);
        remove_nv->setObjectName(QStringLiteral("remove_nv"));
        remove_nv->setGeometry(QRect(90, 170, 31, 23));
        import_2 = new QPushButton(centralwidget);
        import_2->setObjectName(QStringLiteral("import_2"));
        import_2->setGeometry(QRect(700, 280, 75, 23));
        cancel = new QPushButton(centralwidget);
        cancel->setObjectName(QStringLiteral("cancel"));
        cancel->setGeometry(QRect(610, 280, 75, 23));
        nnts = new QLineEdit(centralwidget);
        nnts->setObjectName(QStringLiteral("nnts"));
        nnts->setGeometry(QRect(160, 20, 61, 20));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(130, 20, 21, 16));
        nbars = new QLineEdit(centralwidget);
        nbars->setObjectName(QStringLiteral("nbars"));
        nbars->setGeometry(QRect(280, 20, 61, 20));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(250, 20, 21, 16));
        rp_result = new QPlainTextEdit(centralwidget);
        rp_result->setObjectName(QStringLiteral("rp_result"));
        rp_result->setGeometry(QRect(380, 80, 401, 171));
        generate = new QPushButton(centralwidget);
        generate->setObjectName(QStringLiteral("generate"));
        generate->setGeometry(QRect(290, 130, 75, 23));
        halt = new QPushButton(centralwidget);
        halt->setObjectName(QStringLiteral("halt"));
        halt->setGeometry(QRect(290, 170, 75, 23));
        nvpool = new QListView(centralwidget);
        nvpool->setObjectName(QStringLiteral("nvpool"));
        nvpool->setGeometry(QRect(130, 80, 61, 171));
        nvpool_addnl = new QListView(centralwidget);
        nvpool_addnl->setObjectName(QStringLiteral("nvpool_addnl"));
        nvpool_addnl->setGeometry(QRect(20, 110, 61, 141));
        nvpool_addnl->setSelectionMode(QAbstractItemView::MultiSelection);
        nvprobs = new QListView(centralwidget);
        nvprobs->setObjectName(QStringLiteral("nvprobs"));
        nvprobs->setGeometry(QRect(200, 80, 71, 171));
        rand_rp_status_msg = new QLabel(centralwidget);
        rand_rp_status_msg->setObjectName(QStringLiteral("rand_rp_status_msg"));
        rand_rp_status_msg->setGeometry(QRect(390, 20, 381, 41));
        rp_builder->setCentralWidget(centralwidget);
        menubar = new QMenuBar(rp_builder);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 18));
        menuLoad_template = new QMenu(menubar);
        menuLoad_template->setObjectName(QStringLiteral("menuLoad_template"));
        rp_builder->setMenuBar(menubar);
        statusbar = new QStatusBar(rp_builder);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        rp_builder->setStatusBar(statusbar);

        menubar->addAction(menuLoad_template->menuAction());

        retranslateUi(rp_builder);

        QMetaObject::connectSlotsByName(rp_builder);
    } // setupUi

    void retranslateUi(QMainWindow *rp_builder)
    {
        rp_builder->setWindowTitle(QApplication::translate("rp_builder", "MainWindow", nullptr));
        label->setText(QApplication::translate("rp_builder", "TS", nullptr));
        label_2->setText(QApplication::translate("rp_builder", "NV Pool", nullptr));
        add_nv->setText(QApplication::translate("rp_builder", ">", nullptr));
        remove_nv->setText(QApplication::translate("rp_builder", "<", nullptr));
        import_2->setText(QApplication::translate("rp_builder", "Import", nullptr));
        cancel->setText(QApplication::translate("rp_builder", "Cancel", nullptr));
        label_3->setText(QApplication::translate("rp_builder", "nnts", nullptr));
        nbars->setText(QString());
        label_4->setText(QApplication::translate("rp_builder", "nbrs", nullptr));
        generate->setText(QApplication::translate("rp_builder", "Generate", nullptr));
        halt->setText(QApplication::translate("rp_builder", "Halt", nullptr));
        rand_rp_status_msg->setText(QString());
        menuLoad_template->setTitle(QApplication::translate("rp_builder", "Load template...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class rp_builder: public Ui_rp_builder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RP_BUILDER_H
