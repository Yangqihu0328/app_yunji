/********************************************************************************
** Form generated from reading UI file 'SettingPageRoundPatrol.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGPAGEROUNDPATROL_H
#define UI_SETTINGPAGEROUNDPATROL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingPageRoundPatrol
{
public:
    QGridLayout *gridLayout_2;

    void setupUi(QWidget *SettingPageRoundPatrol)
    {
        if (SettingPageRoundPatrol->objectName().isEmpty())
            SettingPageRoundPatrol->setObjectName(QString::fromUtf8("SettingPageRoundPatrol"));
        SettingPageRoundPatrol->resize(897, 531);
        gridLayout_2 = new QGridLayout(SettingPageRoundPatrol);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));

        retranslateUi(SettingPageRoundPatrol);

        QMetaObject::connectSlotsByName(SettingPageRoundPatrol);
    } // setupUi

    void retranslateUi(QWidget *SettingPageRoundPatrol)
    {
        SettingPageRoundPatrol->setWindowTitle(QCoreApplication::translate("SettingPageRoundPatrol", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingPageRoundPatrol: public Ui_SettingPageRoundPatrol {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGPAGEROUNDPATROL_H
