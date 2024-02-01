//
// Copyright (c) 2019-2023 yanggaofeng
//
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangMath.h>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_centerWdiget=new QWidget(this);
    m_vb=new QVBoxLayout();

    setCentralWidget(m_centerWdiget);
    m_centerWdiget->setLayout(m_vb);
    m_vb->setMargin(0);
    m_vb->setSpacing(0);
#if Yang_OS_APPLE
     m_videoWin=new YangYuvPlayWidget(this);
#else
     m_videoWin=new YangPlayWidget(this);
#endif



    m_hb1=new QHBoxLayout();
    m_hb2=new QHBoxLayout();
    m_hb1->addWidget(ui->m_l_url);
    m_hb1->addWidget(ui->m_url);
    m_hb1->addWidget(ui->m_b_play);
    m_hb1->addWidget(ui->m_c_whep);

    m_hb2->addWidget(m_videoWin);

    m_vb->addLayout(m_hb1);
    m_vb->addLayout(m_hb2);
    m_vb->setStretchFactor(m_hb1,1);
    m_vb->setStretchFactor(m_hb2,10);
    m_context=new YangContext();
    m_context->init();

    m_context->synMgr.session->playBuffer=(YangSynBuffer*)yang_calloc(sizeof(YangSynBuffer),1);//new YangSynBuffer();
    yang_create_synBuffer(m_context->synMgr.session->playBuffer);

    m_context->avinfo.sys.mediaServer=Yang_Server_Srs;//Yang_Server_Srs/Yang_Server_Zlm
    m_context->avinfo.rtc.rtcSocketProtocol=Yang_Socket_Protocol_Udp;//

    m_context->avinfo.rtc.rtcLocalPort=10000+yang_random()%15000;
    memset(m_context->avinfo.rtc.localIp,0,sizeof(m_context->avinfo.rtc.localIp));
    yang_getLocalInfo(m_context->avinfo.sys.familyType,m_context->avinfo.rtc.localIp);
    m_player= YangPlayerHandle::createPlayerHandle(m_context,this);
    yang_setLogLevel(5);
    yang_setLogFile(1);

    char s[128]={0};

    sprintf(s,"http://%s:1985/rtc/v1/whip-play/?app=live&stream=livestream",m_context->avinfo.rtc.localIp);
    yang_trace("\nurl===%s",s);
    ui->m_url->setText(s);
    m_isStartplay=false;

    connect(this,SIGNAL(RtcConnectFailure(int)),SLOT(connectFailure(int)));

    m_context->avinfo.rtc.iceCandidateType=YangIceHost;
     m_context->avinfo.rtc.turnSocketProtocol=Yang_Socket_Protocol_Udp;

    m_context->avinfo.rtc.enableAudioBuffer=yangtrue; //use audio buffer
    m_context->avinfo.audio.enableAudioFec=yangfalse; //srs not use audio fec
}

MainWindow::~MainWindow()
{
    delete ui;
    m_videoThread->m_syn=NULL;
    yang_delete(m_videoWin);
    yang_delete(m_player);
    yang_closeLogFile();
    yang_delete(m_context);

}


void MainWindow::success(){

}
void MainWindow::failure(int32_t errcode){
    emit RtcConnectFailure(errcode);


}
void MainWindow::connectFailure(int errcode){
    QMessageBox::about(NULL, "Error", "play error("+QString::number(errcode)+")!");
   on_m_b_play_clicked();
}
void MainWindow::initVideoThread(YangRecordThread *prt){
    m_videoThread=prt;
    m_videoThread->m_video=m_videoWin;
    m_videoThread->initPara();
     m_videoThread->m_syn= m_context->synMgr.session->playBuffer;


}

void MainWindow::on_m_b_play_clicked()
{
    if(!m_isStartplay){
        m_videoThread->m_syn=m_context->synMgr.session->playBuffer;
        m_videoThread->m_syn->resetVideoClock(m_videoThread->m_syn->session);
        int32_t err=Yang_Ok;
        if(ui->m_c_whep->checkState()==Qt::CheckState::Checked){
             err=m_player->playRtc(0,(char*)ui->m_url->text().toStdString().c_str());
        }else{
            err=m_player->play((char*)ui->m_url->text().toStdString().c_str());
        }
           if(err==Yang_Ok){
               ui->m_b_play->setText("stop");

               m_isStartplay=!m_isStartplay;
               m_videoThread->m_isRender=true;
           }else{
                QMessageBox::about(NULL, "Error", "play url error!");
                 m_videoThread->m_isRender=false;
           }

    }else{
        m_videoThread->m_isRender=false;
        ui->m_b_play->setText("play");

       QThread::msleep(50);
       if(m_player) m_player->stopPlay();

        m_isStartplay=!m_isStartplay;

    }
}



void MainWindow::on_m_c_whep_clicked()
{
    char s[128]={0};

    if(ui->m_c_whep->checkState()==Qt::CheckState::Checked)
        sprintf(s,"http://%s:1985/rtc/v1/whip-play/?app=live&stream=livestream",m_context->avinfo.rtc.localIp);
    else
        sprintf(s,"webrtc://%s:1985/live/livestream",m_context->avinfo.rtc.localIp);

    ui->m_url->setText(s);
}
