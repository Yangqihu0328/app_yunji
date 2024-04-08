/********************************************************************************
** Form generated from reading UI file 'PlaybackLeftToolbar.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYBACKLEFTTOOLBAR_H
#define UI_PLAYBACKLEFTTOOLBAR_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlaybackLeftToolbar
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;
    QTabWidget *tabWidget;
    QWidget *tab_1x1;
    QGridLayout *gridLayout_2;
    QTableWidget *tableWidget_1x1;
    QWidget *tab_2x2;
    QGridLayout *gridLayout_3;
    QTableWidget *tableWidget_2x2;
    QWidget *tab;
    QGridLayout *gridLayout_4;
    QTableWidget *tableWidget_1_7;
    QWidget *tab_4x4;
    QGridLayout *gridLayout_5;
    QTableWidget *tableWidget_4x4;
    QCalendarWidget *calendarWidget;

    void setupUi(QWidget *PlaybackLeftToolbar)
    {
        if (PlaybackLeftToolbar->objectName().isEmpty())
            PlaybackLeftToolbar->setObjectName(QString::fromUtf8("PlaybackLeftToolbar"));
        PlaybackLeftToolbar->resize(242, 631);
        gridLayout = new QGridLayout(PlaybackLeftToolbar);
        gridLayout->setSpacing(2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, -1, 12, -1);
        pushButton = new QPushButton(PlaybackLeftToolbar);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon);
        pushButton->setIconSize(QSize(20, 20));

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        tabWidget = new QTabWidget(PlaybackLeftToolbar);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab_1x1 = new QWidget();
        tab_1x1->setObjectName(QString::fromUtf8("tab_1x1"));
        gridLayout_2 = new QGridLayout(tab_1x1);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(9, 2, 9, 2);
        tableWidget_1x1 = new QTableWidget(tab_1x1);
        if (tableWidget_1x1->columnCount() < 1)
            tableWidget_1x1->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget_1x1->setHorizontalHeaderItem(0, __qtablewidgetitem);
        tableWidget_1x1->setObjectName(QString::fromUtf8("tableWidget_1x1"));
        tableWidget_1x1->setFrameShadow(QFrame::Plain);
        tableWidget_1x1->setRowCount(0);
        tableWidget_1x1->horizontalHeader()->setVisible(false);
        tableWidget_1x1->horizontalHeader()->setStretchLastSection(true);
        tableWidget_1x1->verticalHeader()->setVisible(false);

        gridLayout_2->addWidget(tableWidget_1x1, 0, 0, 1, 1);

        tabWidget->addTab(tab_1x1, QString());
        tab_2x2 = new QWidget();
        tab_2x2->setObjectName(QString::fromUtf8("tab_2x2"));
        gridLayout_3 = new QGridLayout(tab_2x2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(9, 2, 9, 2);
        tableWidget_2x2 = new QTableWidget(tab_2x2);
        tableWidget_2x2->setObjectName(QString::fromUtf8("tableWidget_2x2"));
        tableWidget_2x2->setFrameShadow(QFrame::Plain);
        tableWidget_2x2->horizontalHeader()->setVisible(false);
        tableWidget_2x2->horizontalHeader()->setStretchLastSection(true);
        tableWidget_2x2->verticalHeader()->setVisible(false);

        gridLayout_3->addWidget(tableWidget_2x2, 0, 0, 1, 1);

        tabWidget->addTab(tab_2x2, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_4 = new QGridLayout(tab);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(9, 2, 9, 2);
        tableWidget_1_7 = new QTableWidget(tab);
        tableWidget_1_7->setObjectName(QString::fromUtf8("tableWidget_1_7"));
        tableWidget_1_7->setFrameShadow(QFrame::Plain);
        tableWidget_1_7->horizontalHeader()->setVisible(false);
        tableWidget_1_7->horizontalHeader()->setStretchLastSection(true);
        tableWidget_1_7->verticalHeader()->setVisible(false);

        gridLayout_4->addWidget(tableWidget_1_7, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_4x4 = new QWidget();
        tab_4x4->setObjectName(QString::fromUtf8("tab_4x4"));
        gridLayout_5 = new QGridLayout(tab_4x4);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setContentsMargins(-1, 2, -1, 2);
        tableWidget_4x4 = new QTableWidget(tab_4x4);
        tableWidget_4x4->setObjectName(QString::fromUtf8("tableWidget_4x4"));
        tableWidget_4x4->setFrameShadow(QFrame::Plain);
        tableWidget_4x4->horizontalHeader()->setVisible(false);
        tableWidget_4x4->horizontalHeader()->setStretchLastSection(true);
        tableWidget_4x4->verticalHeader()->setVisible(false);

        gridLayout_5->addWidget(tableWidget_4x4, 0, 0, 1, 1);

        tabWidget->addTab(tab_4x4, QString());

        gridLayout->addWidget(tabWidget, 1, 0, 1, 1);

        calendarWidget = new QCalendarWidget(PlaybackLeftToolbar);
        calendarWidget->setObjectName(QString::fromUtf8("calendarWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(calendarWidget->sizePolicy().hasHeightForWidth());
        calendarWidget->setSizePolicy(sizePolicy);
        calendarWidget->setMaximumSize(QSize(16777215, 16777215));
        calendarWidget->setGridVisible(true);
        calendarWidget->setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
        calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

        gridLayout->addWidget(calendarWidget, 2, 0, 1, 1);

        gridLayout->setRowStretch(1, 1);

        retranslateUi(PlaybackLeftToolbar);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(PlaybackLeftToolbar);
    } // setupUi

    void retranslateUi(QWidget *PlaybackLeftToolbar)
    {
        PlaybackLeftToolbar->setWindowTitle(QCoreApplication::translate("PlaybackLeftToolbar", "Form", nullptr));
        pushButton->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = tableWidget_1x1->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("PlaybackLeftToolbar", "New Column", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_1x1), QString());
        tabWidget->setTabText(tabWidget->indexOf(tab_2x2), QString());
        tabWidget->setTabText(tabWidget->indexOf(tab), QString());
        tabWidget->setTabText(tabWidget->indexOf(tab_4x4), QString());
    } // retranslateUi

};

namespace Ui {
    class PlaybackLeftToolbar: public Ui_PlaybackLeftToolbar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYBACKLEFTTOOLBAR_H
