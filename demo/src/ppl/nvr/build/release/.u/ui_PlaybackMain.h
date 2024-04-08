/********************************************************************************
** Form generated from reading UI file 'PlaybackMain.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYBACKMAIN_H
#define UI_PLAYBACKMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlaybackMain
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_playback;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QWidget *widget_playbackctrl;
    QVBoxLayout *verticalLayout_3;

    void setupUi(QWidget *PlaybackMain)
    {
        if (PlaybackMain->objectName().isEmpty())
            PlaybackMain->setObjectName(QString::fromUtf8("PlaybackMain"));
        PlaybackMain->resize(661, 564);
        verticalLayout = new QVBoxLayout(PlaybackMain);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget_playback = new QWidget(PlaybackMain);
        widget_playback->setObjectName(QString::fromUtf8("widget_playback"));
        gridLayout_2 = new QGridLayout(widget_playback);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        verticalLayout->addWidget(widget_playback);

        widget_playbackctrl = new QWidget(PlaybackMain);
        widget_playbackctrl->setObjectName(QString::fromUtf8("widget_playbackctrl"));
        widget_playbackctrl->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_3 = new QVBoxLayout(widget_playbackctrl);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);

        verticalLayout->addWidget(widget_playbackctrl);

        verticalLayout->setStretch(0, 5);
        verticalLayout->setStretch(1, 1);

        retranslateUi(PlaybackMain);

        QMetaObject::connectSlotsByName(PlaybackMain);
    } // setupUi

    void retranslateUi(QWidget *PlaybackMain)
    {
        PlaybackMain->setWindowTitle(QCoreApplication::translate("PlaybackMain", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlaybackMain: public Ui_PlaybackMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYBACKMAIN_H
