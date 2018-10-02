/********************************************************************************
** Form generated from reading UI file 'metg_builder.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_METG_BUILDER_H
#define UI_METG_BUILDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_metg_builder
{
public:
    QWidget *centralwidget;
    QTableView *tableView;
    QPushButton *import_btn;
    QPushButton *cancel_btn;
    QLineEdit *lineEdit;
    QLabel *label;
    QGroupBox *groupBox;
    QLabel *label_2;
    QLabel *label_3;
    QTableView *tableView_2;
    QLabel *label_4;
    QPushButton *normalize_btn;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *metg_builder)
    {
        if (metg_builder->objectName().isEmpty())
            metg_builder->setObjectName(QStringLiteral("metg_builder"));
        metg_builder->resize(877, 364);
        centralwidget = new QWidget(metg_builder);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        tableView = new QTableView(centralwidget);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setGeometry(QRect(180, 50, 671, 161));
        import_btn = new QPushButton(centralwidget);
        import_btn->setObjectName(QStringLiteral("import_btn"));
        import_btn->setGeometry(QRect(780, 290, 75, 23));
        cancel_btn = new QPushButton(centralwidget);
        cancel_btn->setObjectName(QStringLiteral("cancel_btn"));
        cancel_btn->setGeometry(QRect(670, 290, 75, 23));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(50, 20, 81, 20));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 21, 16));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(20, 230, 120, 61));
        groupBox->setCheckable(false);
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 20, 47, 14));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 40, 47, 14));
        tableView_2 = new QTableView(centralwidget);
        tableView_2->setObjectName(QStringLiteral("tableView_2"));
        tableView_2->setGeometry(QRect(50, 50, 111, 161));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(190, 220, 91, 16));
        normalize_btn = new QPushButton(centralwidget);
        normalize_btn->setObjectName(QStringLiteral("normalize_btn"));
        normalize_btn->setGeometry(QRect(750, 220, 75, 23));
        metg_builder->setCentralWidget(centralwidget);
        menubar = new QMenuBar(metg_builder);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 877, 22));
        metg_builder->setMenuBar(menubar);
        statusbar = new QStatusBar(metg_builder);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        metg_builder->setStatusBar(statusbar);

        retranslateUi(metg_builder);

        QMetaObject::connectSlotsByName(metg_builder);
    } // setupUi

    void retranslateUi(QMainWindow *metg_builder)
    {
        metg_builder->setWindowTitle(QApplication::translate("metg_builder", "MainWindow", nullptr));
        import_btn->setText(QApplication::translate("metg_builder", "Import", nullptr));
        cancel_btn->setText(QApplication::translate("metg_builder", "Cancel", nullptr));
        label->setText(QApplication::translate("metg_builder", "ts", nullptr));
        groupBox->setTitle(QApplication::translate("metg_builder", "chr grid", nullptr));
        label_2->setText(QApplication::translate("metg_builder", "gres", nullptr));
        label_3->setText(QApplication::translate("metg_builder", "Period", nullptr));
        label_4->setText(QApplication::translate("metg_builder", "Extends_or_not", nullptr));
        normalize_btn->setText(QApplication::translate("metg_builder", "Normalize", nullptr));
    } // retranslateUi

};

namespace Ui {
    class metg_builder: public Ui_metg_builder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_METG_BUILDER_H
