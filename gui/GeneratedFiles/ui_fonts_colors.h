/********************************************************************************
** Form generated from reading UI file 'fonts_colors.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FONTS_COLORS_H
#define UI_FONTS_COLORS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_fonts_colors
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QListView *listView;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QCheckBox *checkBox_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QCheckBox *checkBox;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_2;
    QPushButton *pushButton;

    void setupUi(QWidget *fonts_colors)
    {
        if (fonts_colors->objectName().isEmpty())
            fonts_colors->setObjectName(QStringLiteral("fonts_colors"));
        fonts_colors->resize(832, 578);
        fonts_colors->setAutoFillBackground(true);
        verticalLayout = new QVBoxLayout(fonts_colors);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(9, 9, 9, 9);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        listView = new QListView(fonts_colors);
        listView->setObjectName(QStringLiteral("listView"));

        horizontalLayout->addWidget(listView);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_3 = new QLabel(fonts_colors);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_4->addWidget(label_3);

        checkBox_2 = new QCheckBox(fonts_colors);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));

        horizontalLayout_4->addWidget(checkBox_2);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(fonts_colors);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        lineEdit = new QLineEdit(fonts_colors);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout_2->addWidget(lineEdit);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(fonts_colors);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);

        checkBox = new QCheckBox(fonts_colors);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        horizontalLayout_3->addWidget(checkBox);


        verticalLayout_2->addLayout(horizontalLayout_3);


        horizontalLayout->addLayout(verticalLayout_2);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        pushButton_2 = new QPushButton(fonts_colors);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        horizontalLayout_5->addWidget(pushButton_2);

        pushButton = new QPushButton(fonts_colors);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_5->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout_5);


        retranslateUi(fonts_colors);

        QMetaObject::connectSlotsByName(fonts_colors);
    } // setupUi

    void retranslateUi(QWidget *fonts_colors)
    {
        fonts_colors->setWindowTitle(QApplication::translate("fonts_colors", "fonts_colors", nullptr));
        label_3->setText(QApplication::translate("fonts_colors", "TextLabel", nullptr));
        checkBox_2->setText(QApplication::translate("fonts_colors", "CheckBox", nullptr));
        label->setText(QApplication::translate("fonts_colors", "TextLabel", nullptr));
        label_2->setText(QApplication::translate("fonts_colors", "TextLabel", nullptr));
        checkBox->setText(QApplication::translate("fonts_colors", "CheckBox", nullptr));
        pushButton_2->setText(QApplication::translate("fonts_colors", "PushButton", nullptr));
        pushButton->setText(QApplication::translate("fonts_colors", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class fonts_colors: public Ui_fonts_colors {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FONTS_COLORS_H
