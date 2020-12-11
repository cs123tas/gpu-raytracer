/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>
#include "view.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    View *view;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QGroupBox *shapeParameters;
    QGridLayout *gridLayout_4;
    QLabel *shapeParameterLabel2;
    QLabel *shapeParameterLabel1;
    QLabel *shapeParameterLabel3;
    QSlider *shapeParameterSlider1;
    QSlider *shapeParameterSlider2;
    QSlider *shapeParameterSlider3;
    QLineEdit *shapeParameterTextbox3;
    QLineEdit *shapeParameterTextbox2;
    QLineEdit *shapeParameterTextbox1;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        view = new View(centralWidget);
        view->setObjectName(QString::fromUtf8("view"));

        horizontalLayout->addWidget(view);

        MainWindow->setCentralWidget(centralWidget);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setMaximumSize(QSize(200, 300));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        shapeParameters = new QGroupBox(dockWidgetContents);
        shapeParameters->setObjectName(QString::fromUtf8("shapeParameters"));
        shapeParameters->setGeometry(QRect(10, 10, 191, 121));
        gridLayout_4 = new QGridLayout(shapeParameters);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setVerticalSpacing(5);
        gridLayout_4->setContentsMargins(-1, 5, -1, 5);
        shapeParameterLabel2 = new QLabel(shapeParameters);
        shapeParameterLabel2->setObjectName(QString::fromUtf8("shapeParameterLabel2"));

        gridLayout_4->addWidget(shapeParameterLabel2, 2, 0, 1, 1);

        shapeParameterLabel1 = new QLabel(shapeParameters);
        shapeParameterLabel1->setObjectName(QString::fromUtf8("shapeParameterLabel1"));

        gridLayout_4->addWidget(shapeParameterLabel1, 0, 0, 1, 1);

        shapeParameterLabel3 = new QLabel(shapeParameters);
        shapeParameterLabel3->setObjectName(QString::fromUtf8("shapeParameterLabel3"));

        gridLayout_4->addWidget(shapeParameterLabel3, 4, 0, 1, 1);

        shapeParameterSlider1 = new QSlider(shapeParameters);
        shapeParameterSlider1->setObjectName(QString::fromUtf8("shapeParameterSlider1"));
        shapeParameterSlider1->setMinimumSize(QSize(100, 0));
        shapeParameterSlider1->setMinimum(-100);
        shapeParameterSlider1->setMaximum(100);
        shapeParameterSlider1->setSingleStep(0);
        shapeParameterSlider1->setPageStep(30);
        shapeParameterSlider1->setOrientation(Qt::Horizontal);

        gridLayout_4->addWidget(shapeParameterSlider1, 0, 1, 1, 1);

        shapeParameterSlider2 = new QSlider(shapeParameters);
        shapeParameterSlider2->setObjectName(QString::fromUtf8("shapeParameterSlider2"));
        shapeParameterSlider2->setMinimumSize(QSize(100, 0));
        shapeParameterSlider2->setMinimum(-100);
        shapeParameterSlider2->setMaximum(100);
        shapeParameterSlider2->setSingleStep(1);
        shapeParameterSlider2->setOrientation(Qt::Horizontal);

        gridLayout_4->addWidget(shapeParameterSlider2, 2, 1, 1, 1);

        shapeParameterSlider3 = new QSlider(shapeParameters);
        shapeParameterSlider3->setObjectName(QString::fromUtf8("shapeParameterSlider3"));
        shapeParameterSlider3->setMinimumSize(QSize(100, 0));
        shapeParameterSlider3->setMinimum(-100);
        shapeParameterSlider3->setMaximum(100);
        shapeParameterSlider3->setOrientation(Qt::Horizontal);

        gridLayout_4->addWidget(shapeParameterSlider3, 4, 1, 1, 1);

        shapeParameterTextbox3 = new QLineEdit(shapeParameters);
        shapeParameterTextbox3->setObjectName(QString::fromUtf8("shapeParameterTextbox3"));
        shapeParameterTextbox3->setMinimumSize(QSize(40, 0));
        shapeParameterTextbox3->setMaximumSize(QSize(40, 16777215));

        gridLayout_4->addWidget(shapeParameterTextbox3, 4, 2, 1, 1);

        shapeParameterTextbox2 = new QLineEdit(shapeParameters);
        shapeParameterTextbox2->setObjectName(QString::fromUtf8("shapeParameterTextbox2"));
        shapeParameterTextbox2->setMinimumSize(QSize(40, 0));
        shapeParameterTextbox2->setMaximumSize(QSize(40, 16777215));

        gridLayout_4->addWidget(shapeParameterTextbox2, 2, 2, 1, 1);

        shapeParameterTextbox1 = new QLineEdit(shapeParameters);
        shapeParameterTextbox1->setObjectName(QString::fromUtf8("shapeParameterTextbox1"));
        shapeParameterTextbox1->setMinimumSize(QSize(40, 0));
        shapeParameterTextbox1->setMaximumSize(QSize(40, 16777215));

        gridLayout_4->addWidget(shapeParameterTextbox1, 0, 2, 1, 1);

        dockWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "A CS123 Final", nullptr));
        shapeParameters->setTitle(QCoreApplication::translate("MainWindow", "Physics Parameters", nullptr));
        shapeParameterLabel2->setText(QCoreApplication::translate("MainWindow", "2", nullptr));
        shapeParameterLabel1->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        shapeParameterLabel3->setText(QCoreApplication::translate("MainWindow", "3", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
