/********************************************************************************
** Form generated from reading UI file 'SettingPageDevList.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGPAGEDEVLIST_H
#define UI_SETTINGPAGEDEVLIST_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingPageDevList
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButtonAdd;
    QPushButton *pushButtonEdit;
    QPushButton *pushButtonDel;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButtonImport;
    QPushButton *pushButtonExport;
    QTableWidget *tableDevList;

    void setupUi(QWidget *SettingPageDevList)
    {
        if (SettingPageDevList->objectName().isEmpty())
            SettingPageDevList->setObjectName(QString::fromUtf8("SettingPageDevList"));
        SettingPageDevList->resize(899, 630);
        SettingPageDevList->setStyleSheet(QString::fromUtf8(""));
        gridLayout = new QGridLayout(SettingPageDevList);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButtonAdd = new QPushButton(SettingPageDevList);
        pushButtonAdd->setObjectName(QString::fromUtf8("pushButtonAdd"));

        horizontalLayout->addWidget(pushButtonAdd);

        pushButtonEdit = new QPushButton(SettingPageDevList);
        pushButtonEdit->setObjectName(QString::fromUtf8("pushButtonEdit"));

        horizontalLayout->addWidget(pushButtonEdit);

        pushButtonDel = new QPushButton(SettingPageDevList);
        pushButtonDel->setObjectName(QString::fromUtf8("pushButtonDel"));

        horizontalLayout->addWidget(pushButtonDel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButtonImport = new QPushButton(SettingPageDevList);
        pushButtonImport->setObjectName(QString::fromUtf8("pushButtonImport"));

        horizontalLayout->addWidget(pushButtonImport);

        pushButtonExport = new QPushButton(SettingPageDevList);
        pushButtonExport->setObjectName(QString::fromUtf8("pushButtonExport"));

        horizontalLayout->addWidget(pushButtonExport);


        verticalLayout->addLayout(horizontalLayout);

        tableDevList = new QTableWidget(SettingPageDevList);
        if (tableDevList->columnCount() < 9)
            tableDevList->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableDevList->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableDevList->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableDevList->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableDevList->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableDevList->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableDevList->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableDevList->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableDevList->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableDevList->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        tableDevList->setObjectName(QString::fromUtf8("tableDevList"));
        tableDevList->setFrameShadow(QFrame::Plain);
        tableDevList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableDevList->setSelectionMode(QAbstractItemView::SingleSelection);
        tableDevList->horizontalHeader()->setStretchLastSection(true);
        tableDevList->verticalHeader()->setVisible(false);

        verticalLayout->addWidget(tableDevList);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(SettingPageDevList);

        QMetaObject::connectSlotsByName(SettingPageDevList);
    } // setupUi

    void retranslateUi(QWidget *SettingPageDevList)
    {
        SettingPageDevList->setWindowTitle(QCoreApplication::translate("SettingPageDevList", "device list", nullptr));
        pushButtonAdd->setText(QCoreApplication::translate("SettingPageDevList", "Add", nullptr));
        pushButtonEdit->setText(QCoreApplication::translate("SettingPageDevList", "Edit", nullptr));
        pushButtonDel->setText(QCoreApplication::translate("SettingPageDevList", "Delete", nullptr));
        pushButtonImport->setText(QCoreApplication::translate("SettingPageDevList", "Import", nullptr));
        pushButtonExport->setText(QCoreApplication::translate("SettingPageDevList", "Export", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableDevList->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("SettingPageDevList", "Chn", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableDevList->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("SettingPageDevList", "Alias", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableDevList->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("SettingPageDevList", "Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableDevList->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("SettingPageDevList", "Preview", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableDevList->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("SettingPageDevList", "Patrol", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableDevList->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("SettingPageDevList", "Main Rtsp", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableDevList->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("SettingPageDevList", "Record", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tableDevList->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("SettingPageDevList", "Sub1 Rtsp", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tableDevList->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("SettingPageDevList", "Record", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingPageDevList: public Ui_SettingPageDevList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGPAGEDEVLIST_H
