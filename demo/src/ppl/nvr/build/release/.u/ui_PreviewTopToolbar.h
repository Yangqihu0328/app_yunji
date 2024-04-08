/********************************************************************************
** Form generated from reading UI file 'PreviewTopToolbar.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREVIEWTOPTOOLBAR_H
#define UI_PREVIEWTOPTOOLBAR_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PreviewTopToolbar
{
public:
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_1x1;
    QPushButton *pushButton_2x2;
    QPushButton *pushButton_1x8;
    QPushButton *pushButton_4x4;
    QPushButton *pushButton_6x6;
    QPushButton *pushButton_8x8;
    QPushButton *pushButton_previous;
    QPushButton *pushButton_next;
    QPushButton *pushButton_polling;
    QPushButton *pushButton_main_sub1;
    QPushButton *pushButton_pip;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *PreviewTopToolbar)
    {
        if (PreviewTopToolbar->objectName().isEmpty())
            PreviewTopToolbar->setObjectName(QString::fromUtf8("PreviewTopToolbar"));
        PreviewTopToolbar->resize(870, 66);
        horizontalLayout = new QHBoxLayout(PreviewTopToolbar);
        horizontalLayout->setSpacing(1);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton_1x1 = new QPushButton(PreviewTopToolbar);
        pushButton_1x1->setObjectName(QString::fromUtf8("pushButton_1x1"));
        pushButton_1x1->setMinimumSize(QSize(48, 48));
        pushButton_1x1->setMaximumSize(QSize(48, 48));
        pushButton_1x1->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/1x1.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_1x1->setIcon(icon);
        pushButton_1x1->setIconSize(QSize(28, 28));

        horizontalLayout->addWidget(pushButton_1x1);

        pushButton_2x2 = new QPushButton(PreviewTopToolbar);
        pushButton_2x2->setObjectName(QString::fromUtf8("pushButton_2x2"));
        pushButton_2x2->setMinimumSize(QSize(48, 48));
        pushButton_2x2->setMaximumSize(QSize(48, 48));
        pushButton_2x2->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/img/2x2.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2x2->setIcon(icon1);
        pushButton_2x2->setIconSize(QSize(28, 28));

        horizontalLayout->addWidget(pushButton_2x2);

        pushButton_1x8 = new QPushButton(PreviewTopToolbar);
        pushButton_1x8->setObjectName(QString::fromUtf8("pushButton_1x8"));
        pushButton_1x8->setMinimumSize(QSize(48, 48));
        pushButton_1x8->setMaximumSize(QSize(48, 48));
        pushButton_1x8->setFocusPolicy(Qt::NoFocus);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/img/1+7.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_1x8->setIcon(icon2);
        pushButton_1x8->setIconSize(QSize(28, 28));

        horizontalLayout->addWidget(pushButton_1x8);

        pushButton_4x4 = new QPushButton(PreviewTopToolbar);
        pushButton_4x4->setObjectName(QString::fromUtf8("pushButton_4x4"));
        pushButton_4x4->setMinimumSize(QSize(48, 48));
        pushButton_4x4->setMaximumSize(QSize(48, 48));
        pushButton_4x4->setFocusPolicy(Qt::NoFocus);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/img/4x4.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_4x4->setIcon(icon3);
        pushButton_4x4->setIconSize(QSize(28, 28));

        horizontalLayout->addWidget(pushButton_4x4);

        pushButton_6x6 = new QPushButton(PreviewTopToolbar);
        pushButton_6x6->setObjectName(QString::fromUtf8("pushButton_6x6"));
        pushButton_6x6->setMinimumSize(QSize(48, 48));
        pushButton_6x6->setMaximumSize(QSize(48, 48));
        pushButton_6x6->setFocusPolicy(Qt::NoFocus);
        pushButton_6x6->setIcon(icon3);
        pushButton_6x6->setIconSize(QSize(28, 28));

        horizontalLayout->addWidget(pushButton_6x6);

        pushButton_8x8 = new QPushButton(PreviewTopToolbar);
        pushButton_8x8->setObjectName(QString::fromUtf8("pushButton_8x8"));
        pushButton_8x8->setMinimumSize(QSize(48, 48));
        pushButton_8x8->setMaximumSize(QSize(48, 48));
        pushButton_8x8->setFocusPolicy(Qt::NoFocus);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/img/8x8.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_8x8->setIcon(icon4);
        pushButton_8x8->setIconSize(QSize(28, 28));

        horizontalLayout->addWidget(pushButton_8x8);

        pushButton_previous = new QPushButton(PreviewTopToolbar);
        pushButton_previous->setObjectName(QString::fromUtf8("pushButton_previous"));
        pushButton_previous->setMinimumSize(QSize(48, 48));
        pushButton_previous->setMaximumSize(QSize(48, 48));
        pushButton_previous->setFocusPolicy(Qt::NoFocus);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/img/previous.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_previous->setIcon(icon5);
        pushButton_previous->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(pushButton_previous);

        pushButton_next = new QPushButton(PreviewTopToolbar);
        pushButton_next->setObjectName(QString::fromUtf8("pushButton_next"));
        pushButton_next->setMinimumSize(QSize(48, 48));
        pushButton_next->setMaximumSize(QSize(48, 48));
        pushButton_next->setFocusPolicy(Qt::NoFocus);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/img/forward.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_next->setIcon(icon6);
        pushButton_next->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(pushButton_next);

        pushButton_polling = new QPushButton(PreviewTopToolbar);
        pushButton_polling->setObjectName(QString::fromUtf8("pushButton_polling"));
        pushButton_polling->setMinimumSize(QSize(48, 48));
        pushButton_polling->setMaximumSize(QSize(48, 48));
        pushButton_polling->setFocusPolicy(Qt::NoFocus);
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/img/poll.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_polling->setIcon(icon7);
        pushButton_polling->setIconSize(QSize(32, 32));
        pushButton_polling->setCheckable(true);

        horizontalLayout->addWidget(pushButton_polling);

        pushButton_main_sub1 = new QPushButton(PreviewTopToolbar);
        pushButton_main_sub1->setObjectName(QString::fromUtf8("pushButton_main_sub1"));
        pushButton_main_sub1->setMinimumSize(QSize(48, 48));
        pushButton_main_sub1->setMaximumSize(QSize(48, 48));
        pushButton_main_sub1->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(pushButton_main_sub1);

        pushButton_pip = new QPushButton(PreviewTopToolbar);
        pushButton_pip->setObjectName(QString::fromUtf8("pushButton_pip"));
        pushButton_pip->setMinimumSize(QSize(48, 48));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/img/pip.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_pip->setIcon(icon8);
        pushButton_pip->setIconSize(QSize(32, 32));
        pushButton_pip->setCheckable(true);

        horizontalLayout->addWidget(pushButton_pip);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        retranslateUi(PreviewTopToolbar);

        QMetaObject::connectSlotsByName(PreviewTopToolbar);
    } // setupUi

    void retranslateUi(QWidget *PreviewTopToolbar)
    {
        PreviewTopToolbar->setWindowTitle(QCoreApplication::translate("PreviewTopToolbar", "Form", nullptr));
        pushButton_1x1->setText(QString());
        pushButton_2x2->setText(QString());
        pushButton_1x8->setText(QString());
        pushButton_4x4->setText(QString());
        pushButton_6x6->setText(QString());
        pushButton_8x8->setText(QString());
        pushButton_previous->setText(QString());
        pushButton_next->setText(QString());
        pushButton_polling->setText(QString());
        pushButton_main_sub1->setText(QCoreApplication::translate("PreviewTopToolbar", "M/S", nullptr));
        pushButton_pip->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class PreviewTopToolbar: public Ui_PreviewTopToolbar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREVIEWTOPTOOLBAR_H
