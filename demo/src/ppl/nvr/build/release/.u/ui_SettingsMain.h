/********************************************************************************
** Form generated from reading UI file 'SettingsMain.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSMAIN_H
#define UI_SETTINGSMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsMain
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButtonSave;
    QSpacerItem *horizontalSpacer;
    QStackedWidget *stackedWidget;
    QWidget *page_system;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout_system;
    QWidget *page_dev_list;
    QGridLayout *gridLayout_4;
    QVBoxLayout *verticalLayout_dev_list;
    QWidget *page_storage;
    QGridLayout *gridLayout_5;
    QVBoxLayout *verticalLayout_storage;

    void setupUi(QWidget *SettingsMain)
    {
        if (SettingsMain->objectName().isEmpty())
            SettingsMain->setObjectName(QString::fromUtf8("SettingsMain"));
        SettingsMain->resize(703, 480);
        gridLayout = new QGridLayout(SettingsMain);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pushButtonSave = new QPushButton(SettingsMain);
        pushButtonSave->setObjectName(QString::fromUtf8("pushButtonSave"));
        pushButtonSave->setMinimumSize(QSize(150, 30));

        horizontalLayout_3->addWidget(pushButtonSave);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout_3, 1, 0, 1, 1);

        stackedWidget = new QStackedWidget(SettingsMain);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        page_system = new QWidget();
        page_system->setObjectName(QString::fromUtf8("page_system"));
        gridLayout_2 = new QGridLayout(page_system);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_system = new QVBoxLayout();
        verticalLayout_system->setObjectName(QString::fromUtf8("verticalLayout_system"));

        gridLayout_2->addLayout(verticalLayout_system, 0, 0, 1, 1);

        stackedWidget->addWidget(page_system);
        page_dev_list = new QWidget();
        page_dev_list->setObjectName(QString::fromUtf8("page_dev_list"));
        gridLayout_4 = new QGridLayout(page_dev_list);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_dev_list = new QVBoxLayout();
        verticalLayout_dev_list->setSpacing(4);
        verticalLayout_dev_list->setObjectName(QString::fromUtf8("verticalLayout_dev_list"));

        gridLayout_4->addLayout(verticalLayout_dev_list, 0, 0, 1, 1);

        stackedWidget->addWidget(page_dev_list);
        page_storage = new QWidget();
        page_storage->setObjectName(QString::fromUtf8("page_storage"));
        gridLayout_5 = new QGridLayout(page_storage);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        verticalLayout_storage = new QVBoxLayout();
        verticalLayout_storage->setObjectName(QString::fromUtf8("verticalLayout_storage"));

        gridLayout_5->addLayout(verticalLayout_storage, 0, 0, 1, 1);

        stackedWidget->addWidget(page_storage);

        gridLayout->addWidget(stackedWidget, 0, 0, 1, 1);

        gridLayout->setRowStretch(0, 10);

        retranslateUi(SettingsMain);

        QMetaObject::connectSlotsByName(SettingsMain);
    } // setupUi

    void retranslateUi(QWidget *SettingsMain)
    {
        SettingsMain->setWindowTitle(QCoreApplication::translate("SettingsMain", "Form", nullptr));
        pushButtonSave->setText(QCoreApplication::translate("SettingsMain", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsMain: public Ui_SettingsMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSMAIN_H
