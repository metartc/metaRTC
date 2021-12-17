#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangLog.h>
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

    //if using remote control
   // m_videoWin=new YangMouseListenWidget(this);
#if Yang_Listen_MouseAndKeyboard
    m_videoWin=new YangMouseListenWidget(this);
#else
    m_videoWin=new YangPlayWidget(this);
#endif


    m_hb1=new QHBoxLayout();
    m_hb2=new QHBoxLayout();
    m_hb1->addWidget(ui->m_l_url);
    m_hb1->addWidget(ui->m_url);
    m_hb1->addWidget(ui->m_b_play);

    m_hb2->addWidget(m_videoWin);
    m_vb->addLayout(m_hb1);
    m_vb->addLayout(m_hb2);
    m_vb->setStretchFactor(m_hb1,1);
    m_vb->setStretchFactor(m_hb2,10);
    m_ini=new YangContext();
    m_ini->init();

    m_ini->streams.m_playBuffer=new YangSynBuffer();
    m_ini->rtc.mixAvqueue=0;
    m_player= YangPlayerHandle::createPlayerHandle(m_ini,this);


    char s[128]={0};
    sprintf(s,"webrtc://%s:1985/live/livestream",yang_getLocalInfo().c_str());
    ui->m_url->setText(s);
    localPort=16005;

    yang_setLogLevle(5);
    yang_setLogFile(1);
    m_isStartplay=false;

    connect(this,SIGNAL(RtcConnectFailure(int)),SLOT(connectFailure(int)));



}

MainWindow::~MainWindow()
{
    delete ui;
    m_videoThread->m_syn=NULL;
    yang_delete(m_videoWin);
    yang_delete(m_player);
    yang_closeLogFile();
    yang_delete(m_ini);

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
    m_videoThread->m_syn= m_ini->streams.m_playBuffer;

}

void MainWindow::on_m_b_play_clicked()
{
    if(!m_isStartplay){
         m_videoThread->m_syn=m_ini->streams.m_playBuffer;
         m_videoThread->m_syn->resetVideoClock();
           if(m_player&&m_player->play(ui->m_url->text().toStdString(),localPort)==Yang_Ok){
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


