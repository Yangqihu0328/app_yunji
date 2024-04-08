/********************************************************************************
** Form generated from reading UI file 'SettingPageSystem.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGPAGESYSTEM_H
#define UI_SETTINGPAGESYSTEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingPageSystem
{
public:
    QGridLayout *gridLayout_2;
    QTabWidget *tabWidget;
    QWidget *tabRoundPatral;
    QGridLayout *gridLayout_5;
    QGridLayout *gridLayout_4;
    QLabel *label_2;
    QComboBox *comboBoxRPType;
    QLabel *label_3;
    QLabel *label_4;
    QSpinBox *spinBox;
    QCheckBox *checkBoxRPEnable;
    QLabel *label_5;
    QComboBox *comboBoxSplit;
    QSpacerItem *verticalSpacer_2;
    QWidget *widget_2;
    QWidget *tabDetect;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout;
    QCheckBox *checkBoxDetectEnable;
    QLabel *label;
    QSpacerItem *verticalSpacer;
    QWidget *widget;
    QWidget *tabSystem;

    void setupUi(QWidget *SettingPageSystem)
    {
        if (SettingPageSystem->objectName().isEmpty())
            SettingPageSystem->setObjectName(QString::fromUtf8("SettingPageSystem"));
        SettingPageSystem->resize(675, 360);
        SettingPageSystem->setStyleSheet(QString::fromUtf8(""));
        gridLayout_2 = new QGridLayout(SettingPageSystem);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        tabWidget = new QTabWidget(SettingPageSystem);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setStyleSheet(QString::fromUtf8(""));
        tabRoundPatral = new QWidget();
        tabRoundPatral->setObjectName(QString::fromUtf8("tabRoundPatral"));
        gridLayout_5 = new QGridLayout(tabRoundPatral);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        label_2 = new QLabel(tabRoundPatral);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_4->addWidget(label_2, 0, 0, 1, 1);

        comboBoxRPType = new QComboBox(tabRoundPatral);
        comboBoxRPType->addItem(QString());
        comboBoxRPType->addItem(QString());
        comboBoxRPType->addItem(QString());
        comboBoxRPType->setObjectName(QString::fromUtf8("comboBoxRPType"));

        gridLayout_4->addWidget(comboBoxRPType, 1, 1, 1, 1);

        label_3 = new QLabel(tabRoundPatral);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_4->addWidget(label_3, 1, 0, 1, 1);

        label_4 = new QLabel(tabRoundPatral);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_4->addWidget(label_4, 3, 0, 1, 1);

        spinBox = new QSpinBox(tabRoundPatral);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setMinimum(5);
        spinBox->setMaximum(120);

        gridLayout_4->addWidget(spinBox, 3, 1, 1, 1);

        checkBoxRPEnable = new QCheckBox(tabRoundPatral);
        checkBoxRPEnable->setObjectName(QString::fromUtf8("checkBoxRPEnable"));

        gridLayout_4->addWidget(checkBoxRPEnable, 0, 1, 1, 1);

        label_5 = new QLabel(tabRoundPatral);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_4->addWidget(label_5, 2, 0, 1, 1);

        comboBoxSplit = new QComboBox(tabRoundPatral);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/1x1.png"), QSize(), QIcon::Normal, QIcon::Off);
        comboBoxSplit->addItem(icon, QString());
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/img/2x2.png"), QSize(), QIcon::Normal, QIcon::Off);
        comboBoxSplit->addItem(icon1, QString());
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/img/1+7.png"), QSize(), QIcon::Normal, QIcon::Off);
        comboBoxSplit->addItem(icon2, QString());
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/img/4x4.png"), QSize(), QIcon::Normal, QIcon::Off);
        comboBoxSplit->addItem(icon3, QString());
        comboBoxSplit->setObjectName(QString::fromUtf8("comboBoxSplit"));

        gridLayout_4->addWidget(comboBoxSplit, 2, 1, 1, 1);

        gridLayout_4->setColumnStretch(0, 1);
        gridLayout_4->setColumnStretch(1, 4);

        gridLayout_5->addLayout(gridLayout_4, 0, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 241, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_2, 1, 0, 1, 1);

        widget_2 = new QWidget(tabRoundPatral);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));

        gridLayout_5->addWidget(widget_2, 1, 1, 1, 1);

        gridLayout_5->setColumnStretch(0, 2);
        gridLayout_5->setColumnStretch(1, 3);
        tabWidget->addTab(tabRoundPatral, QString());
        tabDetect = new QWidget();
        tabDetect->setObjectName(QString::fromUtf8("tabDetect"));
        gridLayout_3 = new QGridLayout(tabDetect);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        checkBoxDetectEnable = new QCheckBox(tabDetect);
        checkBoxDetectEnable->setObjectName(QString::fromUtf8("checkBoxDetectEnable"));

        gridLayout->addWidget(checkBoxDetectEnable, 0, 1, 1, 1);

        label = new QLabel(tabDetect);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        gridLayout->setColumnStretch(0, 1);
        gridLayout->setColumnStretch(1, 4);

        gridLayout_3->addLayout(gridLayout, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 293, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer, 1, 0, 1, 1);

        widget = new QWidget(tabDetect);
        widget->setObjectName(QString::fromUtf8("widget"));

        gridLayout_3->addWidget(widget, 1, 1, 1, 1);

        gridLayout_3->setColumnStretch(0, 2);
        gridLayout_3->setColumnStretch(1, 3);
        tabWidget->addTab(tabDetect, QString());
        tabSystem = new QWidget();
        tabSystem->setObjectName(QString::fromUtf8("tabSystem"));
        tabWidget->addTab(tabSystem, QString());

        gridLayout_2->addWidget(tabWidget, 1, 0, 1, 1);


        retranslateUi(SettingPageSystem);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SettingPageSystem);
    } // setupUi

    void retranslateUi(QWidget *SettingPageSystem)
    {
        SettingPageSystem->setWindowTitle(QCoreApplication::translate("SettingPageSystem", "Form", nullptr));
        label_2->setText(QCoreApplication::translate("SettingPageSystem", "Enable", nullptr));
        comboBoxRPType->setItemText(0, QCoreApplication::translate("SettingPageSystem", "SPLIT", nullptr));
        comboBoxRPType->setItemText(1, QCoreApplication::translate("SettingPageSystem", "CHANNELS", nullptr));
        comboBoxRPType->setItemText(2, QCoreApplication::translate("SettingPageSystem", "BOTH", nullptr));

        label_3->setText(QCoreApplication::translate("SettingPageSystem", "Type", nullptr));
        label_4->setText(QCoreApplication::translate("SettingPageSystem", "Interval", nullptr));
        checkBoxRPEnable->setText(QString());
        label_5->setText(QCoreApplication::translate("SettingPageSystem", "Grid", nullptr));
        comboBoxSplit->setItemText(0, QCoreApplication::translate("SettingPageSystem", "1x1", nullptr));
        comboBoxSplit->setItemText(1, QCoreApplication::translate("SettingPageSystem", "2x2", nullptr));
        comboBoxSplit->setItemText(2, QCoreApplication::translate("SettingPageSystem", "1+7", nullptr));
        comboBoxSplit->setItemText(3, QCoreApplication::translate("SettingPageSystem", "4x4", nullptr));

        tabWidget->setTabText(tabWidget->indexOf(tabRoundPatral), QCoreApplication::translate("SettingPageSystem", "RoundPatrol", nullptr));
        checkBoxDetectEnable->setText(QString());
        label->setText(QCoreApplication::translate("SettingPageSystem", "Enable", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabDetect), QCoreApplication::translate("SettingPageSystem", "Detect", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabSystem), QCoreApplication::translate("SettingPageSystem", "System", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingPageSystem: public Ui_SettingPageSystem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGPAGESYSTEM_H
