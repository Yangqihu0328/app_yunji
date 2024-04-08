/********************************************************************************
** Form generated from reading UI file 'PlaybackCtrl.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYBACKCTRL_H
#define UI_PLAYBACKCTRL_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlaybackCtrl
{
public:
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButtonPlay;
    QPushButton *pushButtonPause;
    QPushButton *pushButtonStop;
    QPushButton *pushButtonReverse;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButtonPreviousFrame;
    QPushButton *pushButtonNextFrame;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButtonSlow;
    QPushButton *pushButtonFast;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButtonPrevious;
    QPushButton *pushButtonNext;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *PlaybackCtrl)
    {
        if (PlaybackCtrl->objectName().isEmpty())
            PlaybackCtrl->setObjectName(QString::fromUtf8("PlaybackCtrl"));
        PlaybackCtrl->resize(869, 38);
        horizontalLayout_5 = new QHBoxLayout(PlaybackCtrl);
        horizontalLayout_5->setSpacing(12);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(24, 4, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButtonPlay = new QPushButton(PlaybackCtrl);
        pushButtonPlay->setObjectName(QString::fromUtf8("pushButtonPlay"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/playback_start.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonPlay->setIcon(icon);
        pushButtonPlay->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(pushButtonPlay);

        pushButtonPause = new QPushButton(PlaybackCtrl);
        pushButtonPause->setObjectName(QString::fromUtf8("pushButtonPause"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/img/playback_pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonPause->setIcon(icon1);
        pushButtonPause->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(pushButtonPause);

        pushButtonStop = new QPushButton(PlaybackCtrl);
        pushButtonStop->setObjectName(QString::fromUtf8("pushButtonStop"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/img/playback_stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonStop->setIcon(icon2);
        pushButtonStop->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(pushButtonStop);

        pushButtonReverse = new QPushButton(PlaybackCtrl);
        pushButtonReverse->setObjectName(QString::fromUtf8("pushButtonReverse"));
        pushButtonReverse->setMinimumSize(QSize(0, 0));
        pushButtonReverse->setMaximumSize(QSize(16777215, 16777215));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/img/playback_rewind.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonReverse->setIcon(icon3);
        pushButtonReverse->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(pushButtonReverse);


        horizontalLayout_5->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButtonPreviousFrame = new QPushButton(PlaybackCtrl);
        pushButtonPreviousFrame->setObjectName(QString::fromUtf8("pushButtonPreviousFrame"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/img/playback_step_prev.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonPreviousFrame->setIcon(icon4);
        pushButtonPreviousFrame->setIconSize(QSize(24, 24));

        horizontalLayout_2->addWidget(pushButtonPreviousFrame);

        pushButtonNextFrame = new QPushButton(PlaybackCtrl);
        pushButtonNextFrame->setObjectName(QString::fromUtf8("pushButtonNextFrame"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/img/playback_step_next.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonNextFrame->setIcon(icon5);
        pushButtonNextFrame->setIconSize(QSize(24, 24));

        horizontalLayout_2->addWidget(pushButtonNextFrame);


        horizontalLayout_5->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(2);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pushButtonSlow = new QPushButton(PlaybackCtrl);
        pushButtonSlow->setObjectName(QString::fromUtf8("pushButtonSlow"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/img/playback_slow.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonSlow->setIcon(icon6);
        pushButtonSlow->setIconSize(QSize(24, 24));

        horizontalLayout_3->addWidget(pushButtonSlow);

        pushButtonFast = new QPushButton(PlaybackCtrl);
        pushButtonFast->setObjectName(QString::fromUtf8("pushButtonFast"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/img/playback_fast.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonFast->setIcon(icon7);
        pushButtonFast->setIconSize(QSize(24, 24));

        horizontalLayout_3->addWidget(pushButtonFast);


        horizontalLayout_5->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(2);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        pushButtonPrevious = new QPushButton(PlaybackCtrl);
        pushButtonPrevious->setObjectName(QString::fromUtf8("pushButtonPrevious"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/img/playback_prev.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonPrevious->setIcon(icon8);
        pushButtonPrevious->setIconSize(QSize(24, 24));

        horizontalLayout_4->addWidget(pushButtonPrevious);

        pushButtonNext = new QPushButton(PlaybackCtrl);
        pushButtonNext->setObjectName(QString::fromUtf8("pushButtonNext"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/img/playback_next.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonNext->setIcon(icon9);
        pushButtonNext->setIconSize(QSize(24, 24));

        horizontalLayout_4->addWidget(pushButtonNext);


        horizontalLayout_5->addLayout(horizontalLayout_4);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);


        retranslateUi(PlaybackCtrl);

        QMetaObject::connectSlotsByName(PlaybackCtrl);
    } // setupUi

    void retranslateUi(QWidget *PlaybackCtrl)
    {
        PlaybackCtrl->setWindowTitle(QCoreApplication::translate("PlaybackCtrl", "Form", nullptr));
        pushButtonPlay->setText(QString());
        pushButtonPause->setText(QString());
        pushButtonStop->setText(QString());
        pushButtonReverse->setText(QString());
        pushButtonPreviousFrame->setText(QString());
        pushButtonNextFrame->setText(QString());
        pushButtonSlow->setText(QString());
        pushButtonFast->setText(QString());
        pushButtonPrevious->setText(QString());
        pushButtonNext->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class PlaybackCtrl: public Ui_PlaybackCtrl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYBACKCTRL_H
