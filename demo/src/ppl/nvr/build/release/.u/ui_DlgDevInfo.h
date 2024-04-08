/********************************************************************************
** Form generated from reading UI file 'DlgDevInfo.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGDEVINFO_H
#define UI_DLGDEVINFO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_DlgDevInfo
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLineEdit *lineEditRtspUrlMain;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label_4;
    QCheckBox *checkBoxRecordMain;
    QComboBox *comboBoxPreviewIndex;
    QLineEdit *lineEditRtspUrlSub1;
    QLineEdit *lineEditAlias;
    QLabel *label;
    QComboBox *comboBoxChannels;
    QComboBox *comboBoxType;
    QLabel *label_5;
    QSpacerItem *verticalSpacer;
    QLabel *label_6;
    QCheckBox *checkBoxRecordSub1;
    QLabel *label_7;
    QComboBox *comboBoxPatrolIndex;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DlgDevInfo)
    {
        if (DlgDevInfo->objectName().isEmpty())
            DlgDevInfo->setObjectName(QString::fromUtf8("DlgDevInfo"));
        DlgDevInfo->setWindowModality(Qt::WindowModal);
        DlgDevInfo->resize(400, 407);
        DlgDevInfo->setStyleSheet(QString::fromUtf8(""));
        DlgDevInfo->setModal(true);
        gridLayout = new QGridLayout(DlgDevInfo);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(DlgDevInfo);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        lineEditRtspUrlMain = new QLineEdit(groupBox);
        lineEditRtspUrlMain->setObjectName(QString::fromUtf8("lineEditRtspUrlMain"));
        lineEditRtspUrlMain->setMaxLength(128);

        gridLayout_2->addWidget(lineEditRtspUrlMain, 5, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 2, 0, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 5, 0, 1, 1);

        checkBoxRecordMain = new QCheckBox(groupBox);
        checkBoxRecordMain->setObjectName(QString::fromUtf8("checkBoxRecordMain"));

        gridLayout_2->addWidget(checkBoxRecordMain, 5, 2, 1, 1);

        comboBoxPreviewIndex = new QComboBox(groupBox);
        comboBoxPreviewIndex->addItem(QString());
        comboBoxPreviewIndex->addItem(QString());
        comboBoxPreviewIndex->setObjectName(QString::fromUtf8("comboBoxPreviewIndex"));

        gridLayout_2->addWidget(comboBoxPreviewIndex, 3, 1, 1, 2);

        lineEditRtspUrlSub1 = new QLineEdit(groupBox);
        lineEditRtspUrlSub1->setObjectName(QString::fromUtf8("lineEditRtspUrlSub1"));
        lineEditRtspUrlSub1->setMaxLength(128);

        gridLayout_2->addWidget(lineEditRtspUrlSub1, 6, 1, 1, 1);

        lineEditAlias = new QLineEdit(groupBox);
        lineEditAlias->setObjectName(QString::fromUtf8("lineEditAlias"));
        lineEditAlias->setMaxLength(64);

        gridLayout_2->addWidget(lineEditAlias, 1, 1, 1, 2);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        comboBoxChannels = new QComboBox(groupBox);
        comboBoxChannels->setObjectName(QString::fromUtf8("comboBoxChannels"));

        gridLayout_2->addWidget(comboBoxChannels, 0, 1, 1, 2);

        comboBoxType = new QComboBox(groupBox);
        comboBoxType->addItem(QString());
        comboBoxType->addItem(QString());
        comboBoxType->setObjectName(QString::fromUtf8("comboBoxType"));

        gridLayout_2->addWidget(comboBoxType, 2, 1, 1, 2);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_2->addWidget(label_5, 6, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 7, 0, 1, 2);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_2->addWidget(label_6, 3, 0, 1, 1);

        checkBoxRecordSub1 = new QCheckBox(groupBox);
        checkBoxRecordSub1->setObjectName(QString::fromUtf8("checkBoxRecordSub1"));

        gridLayout_2->addWidget(checkBoxRecordSub1, 6, 2, 1, 1);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_2->addWidget(label_7, 4, 0, 1, 1);

        comboBoxPatrolIndex = new QComboBox(groupBox);
        comboBoxPatrolIndex->addItem(QString());
        comboBoxPatrolIndex->addItem(QString());
        comboBoxPatrolIndex->setObjectName(QString::fromUtf8("comboBoxPatrolIndex"));

        gridLayout_2->addWidget(comboBoxPatrolIndex, 4, 1, 1, 2);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DlgDevInfo);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(DlgDevInfo);
        QObject::connect(buttonBox, SIGNAL(accepted()), DlgDevInfo, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DlgDevInfo, SLOT(reject()));

        QMetaObject::connectSlotsByName(DlgDevInfo);
    } // setupUi

    void retranslateUi(QDialog *DlgDevInfo)
    {
        DlgDevInfo->setWindowTitle(QCoreApplication::translate("DlgDevInfo", "device info", nullptr));
        groupBox->setTitle(QString());
        label_3->setText(QCoreApplication::translate("DlgDevInfo", "Type", nullptr));
        label_2->setText(QCoreApplication::translate("DlgDevInfo", "Alias", nullptr));
        label_4->setText(QCoreApplication::translate("DlgDevInfo", "Main", nullptr));
        checkBoxRecordMain->setText(QCoreApplication::translate("DlgDevInfo", "Record", nullptr));
        comboBoxPreviewIndex->setItemText(0, QCoreApplication::translate("DlgDevInfo", "MAIN", nullptr));
        comboBoxPreviewIndex->setItemText(1, QCoreApplication::translate("DlgDevInfo", "SUB1", nullptr));

        label->setText(QCoreApplication::translate("DlgDevInfo", "Channel", nullptr));
        comboBoxType->setItemText(0, QCoreApplication::translate("DlgDevInfo", "COMMON", nullptr));
        comboBoxType->setItemText(1, QCoreApplication::translate("DlgDevInfo", "FISHEYE", nullptr));

        label_5->setText(QCoreApplication::translate("DlgDevInfo", "Sub1", nullptr));
        label_6->setText(QCoreApplication::translate("DlgDevInfo", "Preview", nullptr));
        checkBoxRecordSub1->setText(QCoreApplication::translate("DlgDevInfo", "Record", nullptr));
        label_7->setText(QCoreApplication::translate("DlgDevInfo", "Patrol", nullptr));
        comboBoxPatrolIndex->setItemText(0, QCoreApplication::translate("DlgDevInfo", "MAIN", nullptr));
        comboBoxPatrolIndex->setItemText(1, QCoreApplication::translate("DlgDevInfo", "SUB1", nullptr));

    } // retranslateUi

};

namespace Ui {
    class DlgDevInfo: public Ui_DlgDevInfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGDEVINFO_H
