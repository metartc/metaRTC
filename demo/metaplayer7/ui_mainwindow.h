/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *m_l_url;
    QLineEdit *m_url;
    QPushButton *m_b_play;
    QCheckBox *m_c_whep;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1101, 689);
        MainWindow->setAutoFillBackground(false);
        MainWindow->setStyleSheet(QString::fromUtf8("QWidget, QLabel{\n"
"                        color:white;\n"
"                        border: 1px solid white;\n"
"                        padding:0;\n"
"                        border-radius:5px;\n"
"							background-color:rgb(20,50,70);\n"
"                      /**  background-color:rgba(250,170,0,150);**/\n"
"  }\n"
"\n"
"\n"
"QTabWidget{\n"
"\n"
"border: 1px solid white;\n"
"background-color:rgb(20,50,70);\n"
"}\n"
"QTabWidget::tab-bar{\n"
"alignment:left;\n"
"left:1px;\n"
"}\n"
"QTabWidget::pane { \n"
"/*background-color:rgb(20,50,70);*/\n"
"border:1px solid white;\n"
"} \n"
"QTabBar{\n"
"\n"
"background-color:white;\n"
"}\n"
"QTabBar::tab{\n"
"min-height:28px;\n"
"padding:0 10px;\n"
"border:0;\n"
"margin:1px 1px 0 0;\n"
"background-color:rgb(20,50,70);\n"
"}\n"
"QTabBar::tab:first{\n"
"margin-left:1px;\n"
"}\n"
"QTabBar::tab:hover{\n"
"color:cyan;\n"
"}\n"
"QTabBar::tab:selected{\n"
"background-color: rgb(20, 100, 150);\n"
"}\n"
"QTabBar::tab:selected:hover{\n"
"}\n"
"\n"
"QTabBar::tear{\n"
"}\n"
"QTab"
                        "Bar::scroller{\n"
"}\n"
"QMenu {\n"
"     background-color: rgb(20,50,70); \n"
"     border: 1px solid white;\n"
" }\n"
"  \n"
" QMenu::item {\n"
"\n"
"     background-color: transparent;\n"
"     padding:8px 32px;\n"
"     margin:0px 8px;\n"
"     border-bottom:1px solid #DBDBDB;\n"
" }\n"
"  \n"
" QMenu::item:selected { \n"
"     background-color: #2dabf9;\n"
" }"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        m_l_url = new QLabel(centralwidget);
        m_l_url->setObjectName("m_l_url");
        m_l_url->setGeometry(QRect(20, 20, 80, 21));
        m_url = new QLineEdit(centralwidget);
        m_url->setObjectName("m_url");
        m_url->setGeometry(QRect(130, 10, 651, 29));
        m_b_play = new QPushButton(centralwidget);
        m_b_play->setObjectName("m_b_play");
        m_b_play->setGeometry(QRect(810, 10, 106, 41));
        m_c_whep = new QCheckBox(centralwidget);
        m_c_whep->setObjectName("m_c_whep");
        m_c_whep->setGeometry(QRect(940, 20, 121, 27));
        m_c_whep->setChecked(true);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1101, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "webrtc\346\213\211\346\265\201\346\222\255\346\224\276demo", nullptr));
        m_l_url->setText(QCoreApplication::translate("MainWindow", "URL:", nullptr));
        m_b_play->setText(QCoreApplication::translate("MainWindow", "Play", nullptr));
        m_c_whep->setText(QCoreApplication::translate("MainWindow", "whep", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
