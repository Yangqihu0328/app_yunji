/********************************************************************************
** Form generated from reading UI file 'SettingPageStorage.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGPAGESTORAGE_H
#define UI_SETTINGPAGESTORAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingPageStorage
{
public:

    void setupUi(QWidget *SettingPageStorage)
    {
        if (SettingPageStorage->objectName().isEmpty())
            SettingPageStorage->setObjectName(QString::fromUtf8("SettingPageStorage"));
        SettingPageStorage->resize(1255, 815);

        retranslateUi(SettingPageStorage);

        QMetaObject::connectSlotsByName(SettingPageStorage);
    } // setupUi

    void retranslateUi(QWidget *SettingPageStorage)
    {
        SettingPageStorage->setWindowTitle(QCoreApplication::translate("SettingPageStorage", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingPageStorage: public Ui_SettingPageStorage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGPAGESTORAGE_H
