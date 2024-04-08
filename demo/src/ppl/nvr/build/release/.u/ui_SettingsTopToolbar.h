/********************************************************************************
** Form generated from reading UI file 'SettingsTopToolbar.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSTOPTOOLBAR_H
#define UI_SETTINGSTOPTOOLBAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsTopToolbar
{
public:
    QHBoxLayout *horizontalLayout;

    void setupUi(QWidget *SettingsTopToolbar)
    {
        if (SettingsTopToolbar->objectName().isEmpty())
            SettingsTopToolbar->setObjectName(QString::fromUtf8("SettingsTopToolbar"));
        SettingsTopToolbar->resize(706, 69);
        horizontalLayout = new QHBoxLayout(SettingsTopToolbar);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

        retranslateUi(SettingsTopToolbar);

        QMetaObject::connectSlotsByName(SettingsTopToolbar);
    } // setupUi

    void retranslateUi(QWidget *SettingsTopToolbar)
    {
        SettingsTopToolbar->setWindowTitle(QCoreApplication::translate("SettingsTopToolbar", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsTopToolbar: public Ui_SettingsTopToolbar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSTOPTOOLBAR_H
