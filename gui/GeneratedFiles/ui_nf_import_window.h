/********************************************************************************
** Form generated from reading UI file 'nf_import_window.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NF_IMPORT_WINDOW_H
#define UI_NF_IMPORT_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_nf_import_window
{
public:
    QWidget *centralwidget;
    QTableWidget *nf_table;
    QPushButton *cancel;
    QPushButton *import;
    QLineEdit *ts;
    QLineEdit *bpm;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *err;
    QPlainTextEdit *nv_t_counts;
    QComboBox *comboBox;
    QLabel *label_4;
    QLineEdit *lineEdit;
    QLabel *label_6;
    QLineEdit *curr_fname;
    QLabel *label_5;
    QMenuBar *menubar;
    QMenu *menuOpen;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *nf_import_window)
    {
        if (nf_import_window->objectName().isEmpty())
            nf_import_window->setObjectName(QStringLiteral("nf_import_window"));
        nf_import_window->resize(764, 543);
        centralwidget = new QWidget(nf_import_window);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        nf_table = new QTableWidget(centralwidget);
        if (nf_table->columnCount() < 4)
            nf_table->setColumnCount(4);
        if (nf_table->rowCount() < 100)
            nf_table->setRowCount(100);
        nf_table->setObjectName(QStringLiteral("nf_table"));
        nf_table->setGeometry(QRect(10, 50, 461, 451));
        nf_table->setRowCount(100);
        nf_table->setColumnCount(4);
        cancel = new QPushButton(centralwidget);
        cancel->setObjectName(QStringLiteral("cancel"));
        cancel->setGeometry(QRect(660, 480, 75, 23));
        import = new QPushButton(centralwidget);
        import->setObjectName(QStringLiteral("import"));
        import->setGeometry(QRect(560, 480, 75, 23));
        ts = new QLineEdit(centralwidget);
        ts->setObjectName(QStringLiteral("ts"));
        ts->setGeometry(QRect(530, 10, 61, 20));
        bpm = new QLineEdit(centralwidget);
        bpm->setObjectName(QStringLiteral("bpm"));
        bpm->setGeometry(QRect(530, 40, 61, 20));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(490, 10, 31, 16));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(490, 40, 31, 16));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(490, 70, 31, 16));
        err = new QLineEdit(centralwidget);
        err->setObjectName(QStringLiteral("err"));
        err->setGeometry(QRect(530, 70, 61, 20));
        nv_t_counts = new QPlainTextEdit(centralwidget);
        nv_t_counts->setObjectName(QStringLiteral("nv_t_counts"));
        nv_t_counts->setGeometry(QRect(610, 10, 131, 81));
        comboBox = new QComboBox(centralwidget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(500, 380, 241, 22));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(500, 360, 47, 13));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(500, 430, 231, 20));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(500, 410, 47, 13));
        curr_fname = new QLineEdit(centralwidget);
        curr_fname->setObjectName(QStringLiteral("curr_fname"));
        curr_fname->setGeometry(QRect(80, 10, 381, 21));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 10, 61, 16));
        nf_import_window->setCentralWidget(centralwidget);
        menubar = new QMenuBar(nf_import_window);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 764, 22));
        menuOpen = new QMenu(menubar);
        menuOpen->setObjectName(QStringLiteral("menuOpen"));
        nf_import_window->setMenuBar(menubar);
        statusbar = new QStatusBar(nf_import_window);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        nf_import_window->setStatusBar(statusbar);

        menubar->addAction(menuOpen->menuAction());

        retranslateUi(nf_import_window);

        QMetaObject::connectSlotsByName(nf_import_window);
    } // setupUi

    void retranslateUi(QMainWindow *nf_import_window)
    {
        nf_import_window->setWindowTitle(QApplication::translate("nf_import_window", "MainWindow", nullptr));
        cancel->setText(QApplication::translate("nf_import_window", "Cancel", nullptr));
        import->setText(QApplication::translate("nf_import_window", "Import", nullptr));
        label->setText(QApplication::translate("nf_import_window", "TS", nullptr));
        label_2->setText(QApplication::translate("nf_import_window", "BPM", nullptr));
        label_3->setText(QApplication::translate("nf_import_window", "Err", nullptr));
        comboBox->setItemText(0, QApplication::translate("nf_import_window", "Raw notefile", nullptr));
        comboBox->setItemText(1, QApplication::translate("nf_import_window", "Separate rp, scd", nullptr));
        comboBox->setItemText(2, QApplication::translate("nf_import_window", "Line", nullptr));

        label_4->setText(QApplication::translate("nf_import_window", "Import as:", nullptr));
        label_6->setText(QApplication::translate("nf_import_window", "Name", nullptr));
        label_5->setText(QApplication::translate("nf_import_window", "Current file:", nullptr));
        menuOpen->setTitle(QApplication::translate("nf_import_window", "Open...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class nf_import_window: public Ui_nf_import_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NF_IMPORT_WINDOW_H
