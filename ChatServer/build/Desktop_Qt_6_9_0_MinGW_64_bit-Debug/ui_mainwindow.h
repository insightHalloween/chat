/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTextEdit *logTextEdit;
    QLabel *statusLabel;
    QPushButton *stopServerButton;
    QPushButton *manageClientsButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        logTextEdit = new QTextEdit(centralwidget);
        logTextEdit->setObjectName("logTextEdit");
        logTextEdit->setGeometry(QRect(10, 10, 780, 500));
        logTextEdit->setReadOnly(true);
        logTextEdit->setStyleSheet(QString::fromUtf8("QTextEdit {\n"
"      background-color: white;\n"
"      border: 1px solid #cccccc;\n"
"      border-radius: 4px;\n"
"      padding: 8px;\n"
"      font-family: Microsoft YaHei;\n"
"      font-size: 12px;\n"
"     }"));
        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setGeometry(QRect(10, 520, 200, 30));
        statusLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"      color: #666666;\n"
"      font-family: Microsoft YaHei;\n"
"      font-size: 12px;\n"
"     }"));
        stopServerButton = new QPushButton(centralwidget);
        stopServerButton->setObjectName("stopServerButton");
        stopServerButton->setGeometry(QRect(680, 520, 100, 30));
        stopServerButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"      background-color: #f2f2f2;\n"
"      border: 1px solid #cccccc;\n"
"      border-radius: 4px;\n"
"      color: #333333;\n"
"      font-family: Microsoft YaHei;\n"
"      font-size: 12px;\n"
"     }\n"
"     QPushButton:hover {\n"
"      background-color: #e6e6e6;\n"
"     }\n"
"     QPushButton:pressed {\n"
"      background-color: #d9d9d9;\n"
"     }"));
        manageClientsButton = new QPushButton(centralwidget);
        manageClientsButton->setObjectName("manageClientsButton");
        manageClientsButton->setGeometry(QRect(570, 520, 100, 30));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        statusLabel->setText(QCoreApplication::translate("MainWindow", "\350\201\212\345\244\251\345\256\244\345\234\250\347\272\277\344\272\272\346\225\260\357\274\2323", nullptr));
        stopServerButton->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\346\234\215\345\212\241\345\231\250", nullptr));
        manageClientsButton->setText(QCoreApplication::translate("MainWindow", "\347\256\241\347\220\206\345\256\242\346\210\267\347\253\257", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
