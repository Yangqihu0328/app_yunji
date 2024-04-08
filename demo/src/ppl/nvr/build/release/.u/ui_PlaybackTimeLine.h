/********************************************************************************
** Form generated from reading UI file 'PlaybackTimeLine.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYBACKTIMELINE_H
#define UI_PLAYBACKTIMELINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlaybackTimeLine
{
public:

    void setupUi(QWidget *PlaybackTimeLine)
    {
        if (PlaybackTimeLine->objectName().isEmpty())
            PlaybackTimeLine->setObjectName(QString::fromUtf8("PlaybackTimeLine"));
        PlaybackTimeLine->resize(873, 68);
        PlaybackTimeLine->setStyleSheet(QString::fromUtf8(""));

        retranslateUi(PlaybackTimeLine);

        QMetaObject::connectSlotsByName(PlaybackTimeLine);
    } // setupUi

    void retranslateUi(QWidget *PlaybackTimeLine)
    {
        PlaybackTimeLine->setWindowTitle(QCoreApplication::translate("PlaybackTimeLine", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlaybackTimeLine: public Ui_PlaybackTimeLine {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYBACKTIMELINE_H
