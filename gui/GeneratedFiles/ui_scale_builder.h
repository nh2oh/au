/********************************************************************************
** Form generated from reading UI file 'scale_builder.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCALE_BUILDER_H
#define UI_SCALE_BUILDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
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
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_scale_builder
{
public:
    QWidget *centralWidget;
    QLabel *label;
    QLineEdit *name;
    QLabel *label_2;
    QPlainTextEdit *description;
    QTableWidget *npc_table;
    QGroupBox *groupBox;
    QComboBox *comboBox;
    QLabel *label_3;
    QComboBox *comboBox_2;
    QPushButton *cancel;
    QPushButton *save;
    QPushButton *save_2;
    QMenuBar *menuBar;
    QMenu *menuFiler;
    QMenu *menuHelp;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *scale_builder)
    {
        if (scale_builder->objectName().isEmpty())
            scale_builder->setObjectName(QStringLiteral("scale_builder"));
        scale_builder->resize(708, 590);
        centralWidget = new QWidget(scale_builder);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 40, 47, 13));
        name = new QLineEdit(centralWidget);
        name->setObjectName(QStringLiteral("name"));
        name->setGeometry(QRect(70, 40, 351, 20));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 70, 71, 16));
        description = new QPlainTextEdit(centralWidget);
        description->setObjectName(QStringLiteral("description"));
        description->setGeometry(QRect(20, 90, 401, 101));
        npc_table = new QTableWidget(centralWidget);
        if (npc_table->columnCount() < 3)
            npc_table->setColumnCount(3);
        if (npc_table->rowCount() < 10)
            npc_table->setRowCount(10);
        npc_table->setObjectName(QStringLiteral("npc_table"));
        npc_table->setGeometry(QRect(20, 210, 351, 291));
        npc_table->setMinimumSize(QSize(0, 0));
        npc_table->setRowCount(10);
        npc_table->setColumnCount(3);
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(390, 210, 291, 291));
        comboBox = new QComboBox(groupBox);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(20, 30, 251, 22));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 10, 91, 16));
        comboBox_2 = new QComboBox(centralWidget);
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));
        comboBox_2->setGeometry(QRect(110, 10, 311, 22));
        cancel = new QPushButton(centralWidget);
        cancel->setObjectName(QStringLiteral("cancel"));
        cancel->setGeometry(QRect(590, 510, 75, 23));
        save = new QPushButton(centralWidget);
        save->setObjectName(QStringLiteral("save"));
        save->setGeometry(QRect(490, 510, 75, 23));
        save_2 = new QPushButton(centralWidget);
        save_2->setObjectName(QStringLiteral("save_2"));
        save_2->setGeometry(QRect(290, 510, 75, 23));
        scale_builder->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(scale_builder);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 708, 22));
        menuFiler = new QMenu(menuBar);
        menuFiler->setObjectName(QStringLiteral("menuFiler"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        scale_builder->setMenuBar(menuBar);
        mainToolBar = new QToolBar(scale_builder);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        scale_builder->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(scale_builder);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        scale_builder->setStatusBar(statusBar);

        menuBar->addAction(menuFiler->menuAction());
        menuBar->addAction(menuHelp->menuAction());

        retranslateUi(scale_builder);

        QMetaObject::connectSlotsByName(scale_builder);
    } // setupUi

    void retranslateUi(QMainWindow *scale_builder)
    {
        scale_builder->setWindowTitle(QApplication::translate("scale_builder", "Scale Builder", nullptr));
        label->setText(QApplication::translate("scale_builder", "Name", nullptr));
        label_2->setText(QApplication::translate("scale_builder", "Description", nullptr));
        groupBox->setTitle(QApplication::translate("scale_builder", "Frq generator", nullptr));
        label_3->setText(QApplication::translate("scale_builder", "Load pre-existing", nullptr));
        cancel->setText(QApplication::translate("scale_builder", "Cancel", nullptr));
        save->setText(QApplication::translate("scale_builder", "Save", nullptr));
        save_2->setText(QApplication::translate("scale_builder", "Reset", nullptr));
        menuFiler->setTitle(QApplication::translate("scale_builder", "File", nullptr));
        menuHelp->setTitle(QApplication::translate("scale_builder", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class scale_builder: public Ui_scale_builder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCALE_BUILDER_H
