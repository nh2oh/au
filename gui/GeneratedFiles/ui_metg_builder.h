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
    QTableView *pg_display;
    QPushButton *import_btn;
    QPushButton *cancel_btn;
    QLineEdit *ts;
    QLabel *label;
    QGroupBox *groupBox;
    QLabel *label_2;
    QLabel *label_3;
    QTableView *nvph_display;
    QLabel *label_4;
    QPushButton *normalize_btn;
    QPushButton *set_levels_btn;
    QPushButton *set_ts_btn;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *metg_builder)
    {
        if (metg_builder->objectName().isEmpty())
            metg_builder->setObjectName(QStringLiteral("metg_builder"));
        metg_builder->resize(877, 448);
        centralwidget = new QWidget(metg_builder);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        pg_display = new QTableView(centralwidget);
        pg_display->setObjectName(QStringLiteral("pg_display"));
        pg_display->setGeometry(QRect(150, 50, 701, 241));
        import_btn = new QPushButton(centralwidget);
        import_btn->setObjectName(QStringLiteral("import_btn"));
        import_btn->setGeometry(QRect(780, 370, 75, 23));
        cancel_btn = new QPushButton(centralwidget);
        cancel_btn->setObjectName(QStringLiteral("cancel_btn"));
        cancel_btn->setGeometry(QRect(670, 370, 75, 23));
        ts = new QLineEdit(centralwidget);
        ts->setObjectName(QStringLiteral("ts"));
        ts->setGeometry(QRect(50, 20, 81, 20));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 21, 16));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(20, 310, 120, 61));
        groupBox->setCheckable(false);
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 20, 47, 14));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 40, 47, 14));
        nvph_display = new QTableView(centralwidget);
        nvph_display->setObjectName(QStringLiteral("nvph_display"));
        nvph_display->setGeometry(QRect(20, 50, 111, 191));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(190, 300, 91, 16));
        normalize_btn = new QPushButton(centralwidget);
        normalize_btn->setObjectName(QStringLiteral("normalize_btn"));
        normalize_btn->setGeometry(QRect(750, 300, 75, 23));
        set_levels_btn = new QPushButton(centralwidget);
        set_levels_btn->setObjectName(QStringLiteral("set_levels_btn"));
        set_levels_btn->setGeometry(QRect(20, 250, 75, 23));
        set_ts_btn = new QPushButton(centralwidget);
        set_ts_btn->setObjectName(QStringLiteral("set_ts_btn"));
        set_ts_btn->setGeometry(QRect(140, 20, 75, 23));
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
        set_levels_btn->setText(QApplication::translate("metg_builder", "Set levels", nullptr));
        set_ts_btn->setText(QApplication::translate("metg_builder", "Set ts", nullptr));
    } // retranslateUi

};

namespace Ui {
    class metg_builder: public Ui_metg_builder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_METG_BUILDER_H
