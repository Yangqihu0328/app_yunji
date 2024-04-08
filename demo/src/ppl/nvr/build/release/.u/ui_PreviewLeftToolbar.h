/********************************************************************************
** Form generated from reading UI file 'PreviewLeftToolbar.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREVIEWLEFTTOOLBAR_H
#define UI_PREVIEWLEFTTOOLBAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PreviewLeftToolbar
{
public:
    QVBoxLayout *verticalLayout;
    QListView *listView;

    void setupUi(QWidget *PreviewLeftToolbar)
    {
        if (PreviewLeftToolbar->objectName().isEmpty())
            PreviewLeftToolbar->setObjectName(QString::fromUtf8("PreviewLeftToolbar"));
        PreviewLeftToolbar->resize(182, 631);
        verticalLayout = new QVBoxLayout(PreviewLeftToolbar);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        listView = new QListView(PreviewLeftToolbar);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setStyleSheet(QString::fromUtf8("QListView::item {height: 30px;}\n"
"font: 12pt;"));

        verticalLayout->addWidget(listView);


        retranslateUi(PreviewLeftToolbar);

        QMetaObject::connectSlotsByName(PreviewLeftToolbar);
    } // setupUi

    void retranslateUi(QWidget *PreviewLeftToolbar)
    {
        PreviewLeftToolbar->setWindowTitle(QCoreApplication::translate("PreviewLeftToolbar", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PreviewLeftToolbar: public Ui_PreviewLeftToolbar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREVIEWLEFTTOOLBAR_H
