/********************************************************************************
** Form generated from reading UI file 'SettingsLeftToolbar.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSLEFTTOOLBAR_H
#define UI_SETTINGSLEFTTOOLBAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsLeftToolbar
{
public:
    QGridLayout *gridLayout;
    QPushButton *pushButtonSystem;
    QPushButton *pushButtonDevice;
    QPushButton *pushButtonRecord;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *SettingsLeftToolbar)
    {
        if (SettingsLeftToolbar->objectName().isEmpty())
            SettingsLeftToolbar->setObjectName(QString::fromUtf8("SettingsLeftToolbar"));
        SettingsLeftToolbar->resize(182, 631);
        SettingsLeftToolbar->setStyleSheet(QString::fromUtf8(""));
        gridLayout = new QGridLayout(SettingsLeftToolbar);
        gridLayout->setSpacing(2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        pushButtonSystem = new QPushButton(SettingsLeftToolbar);
        pushButtonSystem->setObjectName(QString::fromUtf8("pushButtonSystem"));

        gridLayout->addWidget(pushButtonSystem, 0, 0, 1, 1);

        pushButtonDevice = new QPushButton(SettingsLeftToolbar);
        pushButtonDevice->setObjectName(QString::fromUtf8("pushButtonDevice"));

        gridLayout->addWidget(pushButtonDevice, 1, 0, 1, 1);

        pushButtonRecord = new QPushButton(SettingsLeftToolbar);
        pushButtonRecord->setObjectName(QString::fromUtf8("pushButtonRecord"));

        gridLayout->addWidget(pushButtonRecord, 2, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 3, 0, 1, 1);


        retranslateUi(SettingsLeftToolbar);

        QMetaObject::connectSlotsByName(SettingsLeftToolbar);
    } // setupUi

    void retranslateUi(QWidget *SettingsLeftToolbar)
    {
        SettingsLeftToolbar->setWindowTitle(QCoreApplication::translate("SettingsLeftToolbar", "Form", nullptr));
        pushButtonSystem->setText(QCoreApplication::translate("SettingsLeftToolbar", "System Config", nullptr));
        pushButtonDevice->setText(QCoreApplication::translate("SettingsLeftToolbar", "Device Config", nullptr));
        pushButtonRecord->setText(QCoreApplication::translate("SettingsLeftToolbar", "Record Config", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsLeftToolbar: public Ui_SettingsLeftToolbar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSLEFTTOOLBAR_H
