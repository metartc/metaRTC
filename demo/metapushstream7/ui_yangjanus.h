/********************************************************************************
** Form generated from reading UI file 'yangjanus.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_YANGJANUS_H
#define UI_YANGJANUS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_YangJanus
{
public:
    QPushButton *m_b_rec;
    QLabel *m_l_url;
    QLineEdit *m_url;
    QLabel *m_l_url_2;
    QLineEdit *m_data;
    QPushButton *m_b_rec_2;
    QLineEdit *m_retInfo;

    void setupUi(QDialog *YangJanus)
    {
        if (YangJanus->objectName().isEmpty())
            YangJanus->setObjectName("YangJanus");
        YangJanus->resize(791, 281);
        YangJanus->setStyleSheet(QString::fromUtf8("QWidget, QLabel{\n"
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
        m_b_rec = new QPushButton(YangJanus);
        m_b_rec->setObjectName("m_b_rec");
        m_b_rec->setGeometry(QRect(110, 210, 181, 31));
        m_l_url = new QLabel(YangJanus);
        m_l_url->setObjectName("m_l_url");
        m_l_url->setGeometry(QRect(40, 40, 80, 31));
        m_url = new QLineEdit(YangJanus);
        m_url->setObjectName("m_url");
        m_url->setGeometry(QRect(150, 40, 601, 29));
        m_l_url_2 = new QLabel(YangJanus);
        m_l_url_2->setObjectName("m_l_url_2");
        m_l_url_2->setGeometry(QRect(30, 100, 80, 31));
        m_data = new QLineEdit(YangJanus);
        m_data->setObjectName("m_data");
        m_data->setGeometry(QRect(150, 100, 601, 29));
        m_b_rec_2 = new QPushButton(YangJanus);
        m_b_rec_2->setObjectName("m_b_rec_2");
        m_b_rec_2->setGeometry(QRect(360, 210, 161, 31));
        m_retInfo = new QLineEdit(YangJanus);
        m_retInfo->setObjectName("m_retInfo");
        m_retInfo->setGeometry(QRect(150, 160, 601, 29));

        retranslateUi(YangJanus);

        QMetaObject::connectSlotsByName(YangJanus);
    } // setupUi

    void retranslateUi(QDialog *YangJanus)
    {
        YangJanus->setWindowTitle(QCoreApplication::translate("YangJanus", "Dialog", nullptr));
        m_b_rec->setText(QCoreApplication::translate("YangJanus", "createUser", nullptr));
        m_l_url->setText(QCoreApplication::translate("YangJanus", "URL:", nullptr));
        m_url->setText(QCoreApplication::translate("YangJanus", "http://10.42.0.1:7080/whip/create", nullptr));
        m_l_url_2->setText(QCoreApplication::translate("YangJanus", "Data:", nullptr));
        m_data->setText(QCoreApplication::translate("YangJanus", "{\"id\": \"metaRTC\", \"room\": 1234}", nullptr));
        m_b_rec_2->setText(QCoreApplication::translate("YangJanus", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class YangJanus: public Ui_YangJanus {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_YANGJANUS_H
