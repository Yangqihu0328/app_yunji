/********************************************************************************
** Form generated from reading UI file 'PreviewPip.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREVIEWPIP_H
#define UI_PREVIEWPIP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PreviewPip
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QStackedWidget *stackedWidget;
    QWidget *page;

    void setupUi(QDialog *PreviewPip)
    {
        if (PreviewPip->objectName().isEmpty())
            PreviewPip->setObjectName(QString::fromUtf8("PreviewPip"));
        PreviewPip->setWindowModality(Qt::WindowModal);
        PreviewPip->resize(788, 490);
        PreviewPip->setStyleSheet(QString::fromUtf8(""));
        PreviewPip->setModal(true);
        gridLayout_2 = new QGridLayout(PreviewPip);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(9, 10, 9, 8);
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        stackedWidget = new QStackedWidget(PreviewPip);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setMinimumSize(QSize(768, 470));
        stackedWidget->setMaximumSize(QSize(768, 470));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        stackedWidget->addWidget(page);

        gridLayout->addWidget(stackedWidget, 0, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(PreviewPip);

        QMetaObject::connectSlotsByName(PreviewPip);
    } // setupUi

    void retranslateUi(QDialog *PreviewPip)
    {
        PreviewPip->setWindowTitle(QCoreApplication::translate("PreviewPip", "PIP", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PreviewPip: public Ui_PreviewPip {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREVIEWPIP_H
