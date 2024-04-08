/********************************************************************************
** Form generated from reading UI file 'PlaybackTopToolbar.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYBACKTOPTOOLBAR_H
#define UI_PLAYBACKTOPTOOLBAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlaybackTopToolbar
{
public:
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_main_sub1;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *PlaybackTopToolbar)
    {
        if (PlaybackTopToolbar->objectName().isEmpty())
            PlaybackTopToolbar->setObjectName(QString::fromUtf8("PlaybackTopToolbar"));
        PlaybackTopToolbar->resize(706, 78);
        horizontalLayout = new QHBoxLayout(PlaybackTopToolbar);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton_main_sub1 = new QPushButton(PlaybackTopToolbar);
        pushButton_main_sub1->setObjectName(QString::fromUtf8("pushButton_main_sub1"));
        pushButton_main_sub1->setMinimumSize(QSize(48, 48));
        pushButton_main_sub1->setMaximumSize(QSize(48, 48));
        pushButton_main_sub1->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(pushButton_main_sub1);

        horizontalSpacer = new QSpacerItem(631, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        retranslateUi(PlaybackTopToolbar);

        QMetaObject::connectSlotsByName(PlaybackTopToolbar);
    } // setupUi

    void retranslateUi(QWidget *PlaybackTopToolbar)
    {
        PlaybackTopToolbar->setWindowTitle(QCoreApplication::translate("PlaybackTopToolbar", "Form", nullptr));
        pushButton_main_sub1->setText(QCoreApplication::translate("PlaybackTopToolbar", "M/S", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlaybackTopToolbar: public Ui_PlaybackTopToolbar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYBACKTOPTOOLBAR_H
