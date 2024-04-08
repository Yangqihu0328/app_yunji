/********************************************************************************
** Form generated from reading UI file 'PreviewMain.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREVIEWMAIN_H
#define UI_PREVIEWMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PreviewMain
{
public:
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout;

    void setupUi(QWidget *PreviewMain)
    {
        if (PreviewMain->objectName().isEmpty())
            PreviewMain->setObjectName(QString::fromUtf8("PreviewMain"));
        PreviewMain->resize(706, 513);
        verticalLayout_3 = new QVBoxLayout(PreviewMain);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(4);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

        verticalLayout_3->addLayout(gridLayout);


        retranslateUi(PreviewMain);

        QMetaObject::connectSlotsByName(PreviewMain);
    } // setupUi

    void retranslateUi(QWidget *PreviewMain)
    {
        PreviewMain->setWindowTitle(QCoreApplication::translate("PreviewMain", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PreviewMain: public Ui_PreviewMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREVIEWMAIN_H
