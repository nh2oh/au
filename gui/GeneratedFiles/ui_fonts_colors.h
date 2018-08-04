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
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_5;
    QListView *listView;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_5;
    QCheckBox *checkBox_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QCheckBox *checkBox;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QCheckBox *checkBox_3;
    QHBoxLayout *horizontalLayout_8;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QWidget *fonts_colors)
    {
        if (fonts_colors->objectName().isEmpty())
            fonts_colors->setObjectName(QStringLiteral("fonts_colors"));
        fonts_colors->resize(764, 525);
        verticalLayoutWidget_2 = new QWidget(fonts_colors);
        verticalLayoutWidget_2->setObjectName(QStringLiteral("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(70, 0, 501, 351));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        listView = new QListView(verticalLayoutWidget_2);
        listView->setObjectName(QStringLiteral("listView"));

        horizontalLayout_5->addWidget(listView);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_2 = new QLabel(verticalLayoutWidget_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        lineEdit_2 = new QLineEdit(verticalLayoutWidget_2);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));

        horizontalLayout->addWidget(lineEdit_2);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_5 = new QLabel(verticalLayoutWidget_2);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_2->addWidget(label_5);

        checkBox_2 = new QCheckBox(verticalLayoutWidget_2);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));

        horizontalLayout_2->addWidget(checkBox_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(verticalLayoutWidget_2);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        checkBox = new QCheckBox(verticalLayoutWidget_2);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        horizontalLayout_3->addWidget(checkBox);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_4 = new QLabel(verticalLayoutWidget_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_4->addWidget(label_4);

        checkBox_3 = new QCheckBox(verticalLayoutWidget_2);
        checkBox_3->setObjectName(QStringLiteral("checkBox_3"));

        horizontalLayout_4->addWidget(checkBox_3);


        verticalLayout->addLayout(horizontalLayout_4);


        horizontalLayout_5->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer);

        okButton = new QPushButton(verticalLayoutWidget_2);
        okButton->setObjectName(QStringLiteral("okButton"));

        horizontalLayout_8->addWidget(okButton);

        cancelButton = new QPushButton(verticalLayoutWidget_2);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        horizontalLayout_8->addWidget(cancelButton);


        verticalLayout_2->addLayout(horizontalLayout_8);


        retranslateUi(fonts_colors);

        QMetaObject::connectSlotsByName(fonts_colors);
    } // setupUi

    void retranslateUi(QWidget *fonts_colors)
    {
        fonts_colors->setWindowTitle(QApplication::translate("fonts_colors", "fonts_colors", nullptr));
        label_2->setText(QApplication::translate("fonts_colors", "Size:", nullptr));
        label_5->setText(QApplication::translate("fonts_colors", "TextLabel", nullptr));
        checkBox_2->setText(QApplication::translate("fonts_colors", "CheckBox", nullptr));
        label_3->setText(QApplication::translate("fonts_colors", "TextLabel", nullptr));
        checkBox->setText(QApplication::translate("fonts_colors", "CheckBox", nullptr));
        label_4->setText(QApplication::translate("fonts_colors", "TextLabel", nullptr));
        checkBox_3->setText(QApplication::translate("fonts_colors", "CheckBox", nullptr));
        okButton->setText(QApplication::translate("fonts_colors", "OK", nullptr));
        cancelButton->setText(QApplication::translate("fonts_colors", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class fonts_colors: public Ui_fonts_colors {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FONTS_COLORS_H
