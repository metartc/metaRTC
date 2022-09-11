//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "recordmainwindow.h"
#include "ui_recordmainwindow.h"
#include <yangp2p/YangP2pCommon.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangFile.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangIni.h>
#include <yangutil/sys/YangString.h>
#include <yangp2p/YangP2pFactory.h>
#include <QDebug>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QMessageBox>

#include <QSettings>
void g_qt_p2p_receiveData(void* context,YangFrame* msgFrame){
    RecordMainWindow* win=(RecordMainWindow*)context;

    win->setRecvText((char*)msgFrame->payload,msgFrame->nb);
}
RecordMainWindow::RecordMainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::RecordMainWindow)
{

    ui->setupUi(this);

    m_context=new YangContext();
    m_context->init((char*)"yang_config.ini");
    m_context->avinfo.video.videoEncoderFormat=YangI420;
#if Yang_Enable_Openh264
    m_context->avinfo.enc.createMeta=0;
#else
    m_context->avinfo.enc.createMeta=1;
#endif
#if Yang_Enable_GPU_Encoding
    //using gpu encode
    m_context->avinfo.video.videoEncHwType=YangV_Hw_Nvdia;//YangV_Hw_Intel,  YangV_Hw_Nvdia,
    m_context->avinfo.video.videoEncoderFormat=YangArgb;//YangI420;//YangArgb;
    m_context->avinfo.enc.createMeta=0;
#endif
#if Yang_Enable_Vr
    //using vr bg file name
    memset(m_context->avinfo.bgFilename,0,sizeof(m_ini->bgFilename));
    QSettings settingsread((char*)"yang_config.ini",QSettings::IniFormat);
    strcpy(m_context->avinfo.bgFilename, settingsread.value("sys/bgFilename",QVariant("d:\\bg.jpeg")).toString().toStdString().c_str());
#endif

    init();
    yang_setLogLevel(m_context->avinfo.sys.logLevel);
    yang_setLogFile(m_context->avinfo.sys.enableLogFile);

    m_context->avinfo.sys.httpPort=1988;
    m_context->avinfo.sys.mediaServer=Yang_Server_P2p;//Yang_Server_Srs/Yang_Server_Zlm/Yang_Server_P2p
    m_context->avinfo.sys.rtcLocalPort=10000+yang_random()%15000;
    memset(m_context->avinfo.sys.localIp,0,sizeof(m_context->avinfo.sys.localIp));
    yang_getLocalInfo(m_context->avinfo.sys.localIp);
    m_hb0=new QHBoxLayout();
    ui->vdMain->setLayout(m_hb0);
    m_win0=new YangPlayWidget(this);
    m_win1=new YangPlayWidget(this);
    m_hb0->addWidget(m_win0);
    m_hb0->addWidget(m_win1);
    m_hb0->setMargin(0);
    m_hb0->setSpacing(0);

    char s[128]={0};

    sprintf(s,"webrtc://10.42.0.1:%d/live/livestream",m_context->avinfo.sys.httpPort);
    ui->m_url->setText(s);

    m_hasAudio=true;
    m_isStartpushplay=false;
    //using h264 h265
    m_context->avinfo.video.videoEncoderType=Yang_VED_264;//Yang_VED_265;
    read_ip_address();

    m_context->streams.m_streamState=this;
    m_context->avinfo.audio.enableAec=yangtrue;

    m_context->channeldataRecv.context=this;
    m_context->channeldataRecv.receiveData=g_qt_p2p_receiveData;

    m_context->avinfo.rtc.enableDatachannel=yangtrue;

    //strcpy(m_context->avinfo.rtc.iceServerIP,"182.92.163.143");
    //m_context->avinfo.rtc.iceStunPort=3478;

    m_context->avinfo.rtc.enableAudioBuffer=yangtrue; //use audio buffer
    m_context->avinfo.audio.enableAudioFec=yangtrue; //use audio fec

}

RecordMainWindow::~RecordMainWindow()
{
    closeAll();
}

void RecordMainWindow::closeEvent( QCloseEvent * event ){

    closeAll();
    exit(0);
}
void RecordMainWindow::setRecvText(char* data,int32_t nb){
    ui->m_t_recv->setText(data);
}
void RecordMainWindow::read_ip_address()
{
    QString ip_address;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); ++i)
    {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&  ipAddressesList.at(i).toIPv4Address())
        {
            ip_address = ipAddressesList.at(i).toString();
            if(strlen(m_context->avinfo.sys.localIp)==0){
                memset(m_context->avinfo.sys.localIp,0,sizeof(m_context->avinfo.sys.localIp));
                strcpy(m_context->avinfo.sys.localIp,ip_address.toLatin1().data());
                yang_trace("\nlocal ip====%s",m_context->avinfo.sys.localIp);
                qDebug()<<ip_address;  //debug
            }
            //break;
        }
    }
    if (ip_address.isEmpty())
        ip_address = QHostAddress(QHostAddress::LocalHost).toString();
    //return ip_address;
}
void RecordMainWindow::receiveSysMessage(YangSysMessage *mss, int32_t err){
    switch (mss->messageId) {
    case YangM_P2p_Connect:
        {
            if(err){
                ui->m_b_pushplay->setText("推拉流");
                m_isStartpushplay=!m_isStartpushplay;
                QMessageBox::about(NULL,  "error","push error("+QString::number(err)+")!");
             }
        }
        break;
   case YangM_P2p_Disconnect:
        break;
    case YangM_P2p_Play_Start:
         break;
    case YangM_P2p_Play_Stop:
         break;
   case YangM_P2p_StartVideoCapture:
    {
        m_render->m_videoBuffer=m_p2pfactory.getPreVideoBuffer(m_message);
        qDebug()<<"message==="<<m_message<<"..prevideobuffer==="<<m_render->m_videoBuffer<<"....ret===="<<err;
         break;
     }

    }


}
void RecordMainWindow::initVideoThread(YangRecordThread *prt){
    m_render=prt;
    m_render->m_local_video=m_win0;
    m_render->m_remote_video=m_win1;
    m_render->initPara(m_context);
    m_render->m_message=m_message;
   // m_rt->m_syn= m_context->streams.m_playBuffer;

}
void RecordMainWindow::closeAll(){

    if(m_context){
        m_render->stopAll();
        m_render=NULL;
        yang_delete(m_message);
        yang_delete(m_context);
        delete ui;
    }
}

void RecordMainWindow::initPreview(){

        yang_post_message(YangM_Push_StartVideoCapture,0,NULL);
       // ui->m_b_pushplay->setEnabled(false);

}

void RecordMainWindow::streamStateNotify(int32_t puid,YangStreamOptType opt,bool isConnect){
    qDebug()<<"online user play count=="<<m_context->streams.getPlayOnlineCount()<<",push count=="<<m_context->streams.getPushOnlineCount();
    if(m_context){
        m_render->m_renderPlay=m_context->streams.getPlayOnlineCount()>0?true:false;
    }
}
void RecordMainWindow::init() {
    m_context->avinfo.audio.enableMono=0;
    m_context->avinfo.audio.sample=48000;
    m_context->avinfo.audio.channel=2;
    m_context->avinfo.audio.enableAec=yangtrue;
    m_context->avinfo.audio.audioCacheNum=8;
    m_context->avinfo.audio.audioCacheSize=8;
    m_context->avinfo.audio.audioPlayCacheNum=8;

    m_context->avinfo.video.videoCacheNum=10;
    m_context->avinfo.video.evideoCacheNum=10;
    m_context->avinfo.video.videoPlayCacheNum=10;

    m_context->avinfo.audio.audioEncoderType=Yang_AED_OPUS;
    m_context->avinfo.sys.rtcLocalPort=17000;
    m_context->avinfo.enc.enc_threads=4;
}

void RecordMainWindow::success(){

}

void RecordMainWindow::failure(int32_t errcode){
    //on_m_b_play_clicked();
    QMessageBox::aboutQt(NULL,  "push error("+QString::number(errcode)+")!");

}


void RecordMainWindow::on_m_b_pushplay_clicked()
{

    //ui->m_b_pushplay->setEnabled(false);
    if(!m_isStartpushplay){
        ui->m_b_pushplay->setText("停止");
        m_isStartpushplay=!m_isStartpushplay;
        qDebug()<<"url========="<<ui->m_url->text().toLatin1().data();
        m_url=ui->m_url->text().toLatin1().data();
        yang_post_message(YangM_Push_Connect,0,NULL,(void*)m_url.c_str());

    }else{

        ui->m_b_pushplay->setText("推拉流");
        yang_post_message(YangM_Push_Disconnect,0,NULL);
        m_isStartpushplay=!m_isStartpushplay;
    }
}

//void RecordMainWindow::on_m_b_play_clicked()
//{
//    if(!m_isStartplay){
//         m_rt->m_syn=m_context->streams.m_playBuffer;
//         m_rt->m_syn->resetVideoClock();
//           if(m_player&&m_player->play(ui->m_url->text().toStdString(),16000)==Yang_Ok){
//               ui->m_b_play->setText("stop");

//               m_isStartplay=!m_isStartplay;
//              // m_videoThread->m_isRender=true;
//           }else{
//                QMessageBox::about(NULL, "Error", "play url error!");
//                // m_videoThread->m_isRender=false;
//           }

//    }else{
//       // m_rt->m_isRender=false;
//        ui->m_b_play->setText("play");

//       QThread::msleep(50);
//       if(m_player) m_player->stopPlay();

//        m_isStartplay=!m_isStartplay;

//    }
//}

void RecordMainWindow::on_m_b_senddata_clicked()
{   
    if(m_context->channeldataSend.sendData){
        YangFrame frame;
        memset(&frame,0,sizeof(YangFrame));
        frame.mediaType=YANG_DATA_CHANNEL_STRING;
        frame.payload=(uint8_t*)ui->m_t_data->text().toLatin1().data();
        frame.nb=strlen(ui->m_t_data->text().toLatin1().data());
        m_context->channeldataSend.sendData(m_context->channeldataSend.context,&frame);
    }
}
