/********************************************************************************
** Form generated from reading UI file 'recordmainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RECORDMAINWINDOW_H
#define UI_RECORDMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RecordMainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *m_b_rec;
    QFrame *vdMain;
    QLabel *m_l_url;
    QLineEdit *m_url;
    QLabel *m_l_err;
    QLabel *m_l_url_3;
    QCheckBox *m_c_whip;
    QCheckBox *m_c_janus;
    QMenuBar *menubar;

    void setupUi(QMainWindow *RecordMainWindow)
    {
        if (RecordMainWindow->objectName().isEmpty())
            RecordMainWindow->setObjectName("RecordMainWindow");
        RecordMainWindow->resize(1150, 833);
        RecordMainWindow->setStyleSheet(QString::fromUtf8("QWidget, QLabel{\n"
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
        centralwidget = new QWidget(RecordMainWindow);
        centralwidget->setObjectName("centralwidget");
        m_b_rec = new QPushButton(centralwidget);
        m_b_rec->setObjectName("m_b_rec");
        m_b_rec->setGeometry(QRect(800, 660, 71, 31));
        vdMain = new QFrame(centralwidget);
        vdMain->setObjectName("vdMain");
        vdMain->setGeometry(QRect(20, 20, 1081, 601));
        vdMain->setFrameShape(QFrame::StyledPanel);
        vdMain->setFrameShadow(QFrame::Raised);
        m_l_url = new QLabel(centralwidget);
        m_l_url->setObjectName("m_l_url");
        m_l_url->setGeometry(QRect(70, 660, 80, 31));
        m_url = new QLineEdit(centralwidget);
        m_url->setObjectName("m_url");
        m_url->setGeometry(QRect(180, 660, 601, 29));
        m_l_err = new QLabel(centralwidget);
        m_l_err->setObjectName("m_l_err");
        m_l_err->setGeometry(QRect(180, 710, 861, 34));
        m_l_url_3 = new QLabel(centralwidget);
        m_l_url_3->setObjectName("m_l_url_3");
        m_l_url_3->setGeometry(QRect(60, 710, 91, 31));
        m_c_whip = new QCheckBox(centralwidget);
        m_c_whip->setObjectName("m_c_whip");
        m_c_whip->setGeometry(QRect(890, 660, 111, 27));
        m_c_whip->setChecked(true);
        m_c_janus = new QCheckBox(centralwidget);
        m_c_janus->setObjectName("m_c_janus");
        m_c_janus->setGeometry(QRect(1010, 660, 111, 27));
        m_c_janus->setChecked(false);
        RecordMainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(RecordMainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1150, 50));
        RecordMainWindow->setMenuBar(menubar);

        retranslateUi(RecordMainWindow);

        QMetaObject::connectSlotsByName(RecordMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *RecordMainWindow)
    {
        RecordMainWindow->setWindowTitle(QCoreApplication::translate("RecordMainWindow", "Webrtc\346\216\250\346\265\201demo", nullptr));
        m_b_rec->setText(QCoreApplication::translate("RecordMainWindow", "start", nullptr));
        m_l_url->setText(QCoreApplication::translate("RecordMainWindow", "URL:", nullptr));
        m_l_err->setText(QString());
        m_l_url_3->setText(QCoreApplication::translate("RecordMainWindow", "Error:", nullptr));
        m_c_whip->setText(QCoreApplication::translate("RecordMainWindow", "whip", nullptr));
        m_c_janus->setText(QCoreApplication::translate("RecordMainWindow", "janus", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RecordMainWindow: public Ui_RecordMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RECORDMAINWINDOW_H
