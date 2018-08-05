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
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
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
    QTableWidget *nf_data;
    QPushButton *cancel;
    QPushButton *import;
    QLineEdit *ts;
    QLineEdit *bpm;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *err;
    QPlainTextEdit *note_value_counts;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *nf_import_window)
    {
        if (nf_import_window->objectName().isEmpty())
            nf_import_window->setObjectName(QStringLiteral("nf_import_window"));
        nf_import_window->resize(796, 543);
        centralwidget = new QWidget(nf_import_window);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        nf_data = new QTableWidget(centralwidget);
        if (nf_data->columnCount() < 10)
            nf_data->setColumnCount(10);
        if (nf_data->rowCount() < 100)
            nf_data->setRowCount(100);
        nf_data->setObjectName(QStringLiteral("nf_data"));
        nf_data->setGeometry(QRect(20, 20, 571, 481));
        nf_data->setRowCount(100);
        nf_data->setColumnCount(10);
        cancel = new QPushButton(centralwidget);
        cancel->setObjectName(QStringLiteral("cancel"));
        cancel->setGeometry(QRect(700, 480, 75, 23));
        import = new QPushButton(centralwidget);
        import->setObjectName(QStringLiteral("import"));
        import->setGeometry(QRect(610, 480, 75, 23));
        ts = new QLineEdit(centralwidget);
        ts->setObjectName(QStringLiteral("ts"));
        ts->setGeometry(QRect(670, 40, 113, 20));
        bpm = new QLineEdit(centralwidget);
        bpm->setObjectName(QStringLiteral("bpm"));
        bpm->setGeometry(QRect(670, 70, 113, 20));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(610, 40, 47, 13));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(610, 70, 47, 13));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(610, 100, 47, 13));
        err = new QLineEdit(centralwidget);
        err->setObjectName(QStringLiteral("err"));
        err->setGeometry(QRect(670, 100, 113, 20));
        note_value_counts = new QPlainTextEdit(centralwidget);
        note_value_counts->setObjectName(QStringLiteral("note_value_counts"));
        note_value_counts->setGeometry(QRect(610, 150, 171, 231));
        nf_import_window->setCentralWidget(centralwidget);
        menubar = new QMenuBar(nf_import_window);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 796, 18));
        nf_import_window->setMenuBar(menubar);
        statusbar = new QStatusBar(nf_import_window);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        nf_import_window->setStatusBar(statusbar);

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
    } // retranslateUi

};

namespace Ui {
    class nf_import_window: public Ui_nf_import_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NF_IMPORT_WINDOW_H
