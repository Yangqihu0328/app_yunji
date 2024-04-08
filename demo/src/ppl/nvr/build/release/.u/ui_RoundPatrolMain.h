/********************************************************************************
** Form generated from reading UI file 'RoundPatrolMain.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROUNDPATROLMAIN_H
#define UI_ROUNDPATROLMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RoundPatrolMain
{
public:
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout;

    void setupUi(QWidget *RoundPatrolMain)
    {
        if (RoundPatrolMain->objectName().isEmpty())
            RoundPatrolMain->setObjectName(QString::fromUtf8("RoundPatrolMain"));
        RoundPatrolMain->resize(706, 513);
        verticalLayout_3 = new QVBoxLayout(RoundPatrolMain);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(4);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

        verticalLayout_3->addLayout(gridLayout);


        retranslateUi(RoundPatrolMain);

        QMetaObject::connectSlotsByName(RoundPatrolMain);
    } // setupUi

    void retranslateUi(QWidget *RoundPatrolMain)
    {
        RoundPatrolMain->setWindowTitle(QCoreApplication::translate("RoundPatrolMain", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RoundPatrolMain: public Ui_RoundPatrolMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROUNDPATROLMAIN_H
