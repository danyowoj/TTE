/********************************************************************************
** Form generated from reading UI file 'graphiceditor.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHICEDITOR_H
#define UI_GRAPHICEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_graphiceditor
{
public:
    QAction *SetPen;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QMenuBar *menubar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *graphiceditor)
    {
        if (graphiceditor->objectName().isEmpty())
            graphiceditor->setObjectName(QString::fromUtf8("graphiceditor"));
        graphiceditor->resize(735, 542);
        SetPen = new QAction(graphiceditor);
        SetPen->setObjectName(QString::fromUtf8("SetPen"));
        centralwidget = new QWidget(graphiceditor);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        graphiceditor->setCentralWidget(centralwidget);
        menubar = new QMenuBar(graphiceditor);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 735, 20));
        graphiceditor->setMenuBar(menubar);
        toolBar = new QToolBar(graphiceditor);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        graphiceditor->addToolBar(Qt::TopToolBarArea, toolBar);

        toolBar->addAction(SetPen);

        retranslateUi(graphiceditor);

        QMetaObject::connectSlotsByName(graphiceditor);
    } // setupUi

    void retranslateUi(QMainWindow *graphiceditor)
    {
        graphiceditor->setWindowTitle(QApplication::translate("graphiceditor", "MainWindow", nullptr));
        SetPen->setText(QApplication::translate("graphiceditor", "\320\232\320\270\321\201\321\202\321\214", nullptr));
        toolBar->setWindowTitle(QApplication::translate("graphiceditor", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class graphiceditor: public Ui_graphiceditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHICEDITOR_H
