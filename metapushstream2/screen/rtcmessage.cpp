#include "rtcmessage.h"
#include <QVBoxLayout>
RtcMessage::RtcMessage(QWidget *parent):QWidget(parent)
{
    m_lab=new QLabel(this);
    m_lab->setText("无法连接信令服务器！正在进行重连!");
    QVBoxLayout *m_vl=new QVBoxLayout(this);
    m_vl->addWidget(m_lab);
    setLayout(m_vl);

}
RtcMessage::~RtcMessage(){
    if(m_lab) delete m_lab;
    m_lab=NULL;
}
