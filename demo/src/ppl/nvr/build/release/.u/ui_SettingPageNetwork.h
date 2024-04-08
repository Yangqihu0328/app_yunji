/********************************************************************************
** Form generated from reading UI file 'SettingPageNetwork.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGPAGENETWORK_H
#define UI_SETTINGPAGENETWORK_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingPageNetwork
{
public:

    void setupUi(QWidget *SettingPageNetwork)
    {
        if (SettingPageNetwork->objectName().isEmpty())
            SettingPageNetwork->setObjectName(QString::fromUtf8("SettingPageNetwork"));
        SettingPageNetwork->resize(1255, 815);

        retranslateUi(SettingPageNetwork);

        QMetaObject::connectSlotsByName(SettingPageNetwork);
    } // setupUi

    void retranslateUi(QWidget *SettingPageNetwork)
    {
        SettingPageNetwork->setWindowTitle(QCoreApplication::translate("SettingPageNetwork", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingPageNetwork: public Ui_SettingPageNetwork {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGPAGENETWORK_H
