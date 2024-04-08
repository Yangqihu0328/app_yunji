/********************************************************************************
** Form generated from reading UI file 'nvrmainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NVRMAINWINDOW_H
#define UI_NVRMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NVRMainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QWidget *widgetTop;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QStackedWidget *stackedWidgetTopToolbar;
    QWidget *page_tt_preview;
    QGridLayout *gridLayout_6;
    QHBoxLayout *horizontalLayout_tt_preview;
    QWidget *page_tt_playback;
    QGridLayout *gridLayout_7;
    QHBoxLayout *horizontalLayout_tt_playback;
    QWidget *page_tt_settings;
    QGridLayout *gridLayout_9;
    QHBoxLayout *horizontalLayout_tt_settings;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButtonPolling;
    QPushButton *pushButtonPreview;
    QPushButton *pushButtonPlayback;
    QPushButton *pushButtonSettings;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButtonPower;
    QStackedWidget *stackedWidgetLeftToolbar;
    QWidget *page_lt_preview;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout_lt_preview;
    QWidget *page_lt_playback;
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout_lt_playback;
    QWidget *page_lt_settings;
    QGridLayout *gridLayout_5;
    QVBoxLayout *verticalLayout_lt_settings;
    QStackedWidget *stackedWidget;
    QWidget *page_preview;
    QGridLayout *gridLayout_4;
    QVBoxLayout *verticalLayout_preview;
    QWidget *page_playback;
    QGridLayout *gridLayout_11;
    QVBoxLayout *verticalLayout_playback;
    QWidget *page_settings;
    QGridLayout *gridLayout_13;
    QVBoxLayout *verticalLayout_settings;
    QWidget *widgetLogo;
    QGridLayout *gridLayout_8;
    QLabel *label;
    QLabel *label_version;

    void setupUi(QMainWindow *NVRMainWindow)
    {
        if (NVRMainWindow->objectName().isEmpty())
            NVRMainWindow->setObjectName(QString::fromUtf8("NVRMainWindow"));
        NVRMainWindow->resize(928, 594);
        NVRMainWindow->setStyleSheet(QString::fromUtf8(""));
        centralwidget = new QWidget(NVRMainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setStyleSheet(QString::fromUtf8(""));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        widgetTop = new QWidget(centralwidget);
        widgetTop->setObjectName(QString::fromUtf8("widgetTop"));
        widgetTop->setStyleSheet(QString::fromUtf8(""));
        horizontalLayout_4 = new QHBoxLayout(widgetTop);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(4, 4, 4, 4);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(1);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        stackedWidgetTopToolbar = new QStackedWidget(widgetTop);
        stackedWidgetTopToolbar->setObjectName(QString::fromUtf8("stackedWidgetTopToolbar"));
        stackedWidgetTopToolbar->setStyleSheet(QString::fromUtf8(""));
        page_tt_preview = new QWidget();
        page_tt_preview->setObjectName(QString::fromUtf8("page_tt_preview"));
        gridLayout_6 = new QGridLayout(page_tt_preview);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_6->setContentsMargins(2, 2, 2, 2);
        horizontalLayout_tt_preview = new QHBoxLayout();
        horizontalLayout_tt_preview->setObjectName(QString::fromUtf8("horizontalLayout_tt_preview"));

        gridLayout_6->addLayout(horizontalLayout_tt_preview, 0, 0, 1, 1);

        stackedWidgetTopToolbar->addWidget(page_tt_preview);
        page_tt_playback = new QWidget();
        page_tt_playback->setObjectName(QString::fromUtf8("page_tt_playback"));
        gridLayout_7 = new QGridLayout(page_tt_playback);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        horizontalLayout_tt_playback = new QHBoxLayout();
        horizontalLayout_tt_playback->setObjectName(QString::fromUtf8("horizontalLayout_tt_playback"));

        gridLayout_7->addLayout(horizontalLayout_tt_playback, 0, 0, 1, 1);

        stackedWidgetTopToolbar->addWidget(page_tt_playback);
        page_tt_settings = new QWidget();
        page_tt_settings->setObjectName(QString::fromUtf8("page_tt_settings"));
        gridLayout_9 = new QGridLayout(page_tt_settings);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        horizontalLayout_tt_settings = new QHBoxLayout();
        horizontalLayout_tt_settings->setObjectName(QString::fromUtf8("horizontalLayout_tt_settings"));

        gridLayout_9->addLayout(horizontalLayout_tt_settings, 0, 0, 1, 1);

        stackedWidgetTopToolbar->addWidget(page_tt_settings);

        horizontalLayout_3->addWidget(stackedWidgetTopToolbar);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(1);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButtonPolling = new QPushButton(widgetTop);
        pushButtonPolling->setObjectName(QString::fromUtf8("pushButtonPolling"));
        pushButtonPolling->setMinimumSize(QSize(48, 48));
        pushButtonPolling->setMaximumSize(QSize(48, 48));
        pushButtonPolling->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/poll.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonPolling->setIcon(icon);
        pushButtonPolling->setIconSize(QSize(32, 32));
        pushButtonPolling->setCheckable(true);

        horizontalLayout->addWidget(pushButtonPolling);

        pushButtonPreview = new QPushButton(widgetTop);
        pushButtonPreview->setObjectName(QString::fromUtf8("pushButtonPreview"));
        pushButtonPreview->setMinimumSize(QSize(48, 48));
        pushButtonPreview->setMaximumSize(QSize(48, 48));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/img/live.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonPreview->setIcon(icon1);
        pushButtonPreview->setIconSize(QSize(36, 36));

        horizontalLayout->addWidget(pushButtonPreview);

        pushButtonPlayback = new QPushButton(widgetTop);
        pushButtonPlayback->setObjectName(QString::fromUtf8("pushButtonPlayback"));
        pushButtonPlayback->setMinimumSize(QSize(48, 48));
        pushButtonPlayback->setMaximumSize(QSize(48, 48));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/img/playback.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonPlayback->setIcon(icon2);
        pushButtonPlayback->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(pushButtonPlayback);

        pushButtonSettings = new QPushButton(widgetTop);
        pushButtonSettings->setObjectName(QString::fromUtf8("pushButtonSettings"));
        pushButtonSettings->setMinimumSize(QSize(48, 48));
        pushButtonSettings->setMaximumSize(QSize(48, 48));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/img/setting.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonSettings->setIcon(icon3);
        pushButtonSettings->setIconSize(QSize(34, 34));

        horizontalLayout->addWidget(pushButtonSettings);


        horizontalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(1);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(60, -1, -1, -1);
        pushButtonPower = new QPushButton(widgetTop);
        pushButtonPower->setObjectName(QString::fromUtf8("pushButtonPower"));
        pushButtonPower->setMinimumSize(QSize(48, 48));
        pushButtonPower->setMaximumSize(QSize(48, 48));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/img/shutdown.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonPower->setIcon(icon4);
        pushButtonPower->setIconSize(QSize(32, 32));

        horizontalLayout_2->addWidget(pushButtonPower);


        horizontalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout_3->setStretch(0, 5);
        horizontalLayout_3->setStretch(1, 4);
        horizontalLayout_3->setStretch(2, 1);

        horizontalLayout_4->addLayout(horizontalLayout_3);


        gridLayout->addWidget(widgetTop, 0, 1, 1, 1);

        stackedWidgetLeftToolbar = new QStackedWidget(centralwidget);
        stackedWidgetLeftToolbar->setObjectName(QString::fromUtf8("stackedWidgetLeftToolbar"));
        stackedWidgetLeftToolbar->setStyleSheet(QString::fromUtf8(""));
        page_lt_preview = new QWidget();
        page_lt_preview->setObjectName(QString::fromUtf8("page_lt_preview"));
        gridLayout_2 = new QGridLayout(page_lt_preview);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(4, 4, 4, 4);
        verticalLayout_lt_preview = new QVBoxLayout();
        verticalLayout_lt_preview->setObjectName(QString::fromUtf8("verticalLayout_lt_preview"));

        gridLayout_2->addLayout(verticalLayout_lt_preview, 0, 0, 1, 1);

        stackedWidgetLeftToolbar->addWidget(page_lt_preview);
        page_lt_playback = new QWidget();
        page_lt_playback->setObjectName(QString::fromUtf8("page_lt_playback"));
        gridLayout_3 = new QGridLayout(page_lt_playback);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(4, 4, 4, 4);
        verticalLayout_lt_playback = new QVBoxLayout();
        verticalLayout_lt_playback->setObjectName(QString::fromUtf8("verticalLayout_lt_playback"));

        gridLayout_3->addLayout(verticalLayout_lt_playback, 0, 0, 1, 1);

        stackedWidgetLeftToolbar->addWidget(page_lt_playback);
        page_lt_settings = new QWidget();
        page_lt_settings->setObjectName(QString::fromUtf8("page_lt_settings"));
        gridLayout_5 = new QGridLayout(page_lt_settings);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setContentsMargins(4, 4, 4, 4);
        verticalLayout_lt_settings = new QVBoxLayout();
        verticalLayout_lt_settings->setObjectName(QString::fromUtf8("verticalLayout_lt_settings"));

        gridLayout_5->addLayout(verticalLayout_lt_settings, 0, 0, 1, 1);

        stackedWidgetLeftToolbar->addWidget(page_lt_settings);

        gridLayout->addWidget(stackedWidgetLeftToolbar, 1, 0, 1, 1);

        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        page_preview = new QWidget();
        page_preview->setObjectName(QString::fromUtf8("page_preview"));
        gridLayout_4 = new QGridLayout(page_preview);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(4, 4, 4, 4);
        verticalLayout_preview = new QVBoxLayout();
        verticalLayout_preview->setObjectName(QString::fromUtf8("verticalLayout_preview"));
        verticalLayout_preview->setContentsMargins(-1, 0, -1, -1);

        gridLayout_4->addLayout(verticalLayout_preview, 0, 0, 1, 1);

        stackedWidget->addWidget(page_preview);
        page_playback = new QWidget();
        page_playback->setObjectName(QString::fromUtf8("page_playback"));
        gridLayout_11 = new QGridLayout(page_playback);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        gridLayout_11->setContentsMargins(4, 4, 4, 4);
        verticalLayout_playback = new QVBoxLayout();
        verticalLayout_playback->setObjectName(QString::fromUtf8("verticalLayout_playback"));

        gridLayout_11->addLayout(verticalLayout_playback, 0, 0, 1, 1);

        stackedWidget->addWidget(page_playback);
        page_settings = new QWidget();
        page_settings->setObjectName(QString::fromUtf8("page_settings"));
        gridLayout_13 = new QGridLayout(page_settings);
        gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));
        gridLayout_13->setContentsMargins(4, 4, 4, 4);
        verticalLayout_settings = new QVBoxLayout();
        verticalLayout_settings->setObjectName(QString::fromUtf8("verticalLayout_settings"));

        gridLayout_13->addLayout(verticalLayout_settings, 0, 0, 1, 1);

        stackedWidget->addWidget(page_settings);

        gridLayout->addWidget(stackedWidget, 1, 1, 1, 1);

        widgetLogo = new QWidget(centralwidget);
        widgetLogo->setObjectName(QString::fromUtf8("widgetLogo"));
        gridLayout_8 = new QGridLayout(widgetLogo);
        gridLayout_8->setSpacing(4);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        gridLayout_8->setContentsMargins(4, 4, 4, 4);
        label = new QLabel(widgetLogo);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(18);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setStyleSheet(QString::fromUtf8(""));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_8->addWidget(label, 0, 0, 1, 1);

        label_version = new QLabel(widgetLogo);
        label_version->setObjectName(QString::fromUtf8("label_version"));
        label_version->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_8->addWidget(label_version, 0, 1, 1, 1);


        gridLayout->addWidget(widgetLogo, 0, 0, 1, 1);

        gridLayout->setRowStretch(0, 1);
        gridLayout->setRowStretch(1, 20);
        gridLayout->setColumnStretch(0, 1);
        gridLayout->setColumnStretch(1, 9);
        NVRMainWindow->setCentralWidget(centralwidget);

        retranslateUi(NVRMainWindow);

        stackedWidgetTopToolbar->setCurrentIndex(0);
        stackedWidgetLeftToolbar->setCurrentIndex(2);
        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(NVRMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *NVRMainWindow)
    {
        NVRMainWindow->setWindowTitle(QCoreApplication::translate("NVRMainWindow", "NVRMainWindow", nullptr));
        pushButtonPolling->setText(QString());
#if QT_CONFIG(tooltip)
        pushButtonPreview->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        pushButtonPreview->setText(QString());
#if QT_CONFIG(tooltip)
        pushButtonPlayback->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        pushButtonPlayback->setText(QString());
#if QT_CONFIG(tooltip)
        pushButtonSettings->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        pushButtonSettings->setText(QString());
#if QT_CONFIG(tooltip)
        pushButtonPower->setToolTip(QCoreApplication::translate("NVRMainWindow", "Power", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButtonPower->setText(QString());
        label->setText(QCoreApplication::translate("NVRMainWindow", "NVR Demo", nullptr));
        label_version->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class NVRMainWindow: public Ui_NVRMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NVRMAINWINDOW_H
