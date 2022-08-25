//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "recordmainwindow.h"
#include "ui_recordmainwindow.h"
#include <yangpush/YangPushCommon.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangFile.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangIni.h>
#include <yangutil/sys/YangString.h>
#include <yangpush/YangPushFactory.h>
#include <QDebug>
//#include <QMessageBox>
#include <QDesktopWidget>
#include <QSettings>
void g_qt_push_receiveData(void* context,YangFrame* msgFrame){
    RecordMainWindow* win=(RecordMainWindow*)context;
    win->setRecvText((char*)msgFrame->payload,msgFrame->nb);
}
RecordMainWindow::RecordMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RecordMainWindow)
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
    m_context->avinfo.video.videoEncoderFormat=YangI420;//YangI420;//YangArgb;
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

    m_context->avinfo.sys.mediaServer=Yang_Server_Srs;//Yang_Server_Srs/Yang_Server_Zlm
    m_context->avinfo.sys.rtcLocalPort=10000+yang_random()%15000;
    //m_showev->event=1;
#if    Yang_SendVideo_
    m_videoType=Yang_VideoSrc_OutInterface;
    m_outInfo.width=1920;
    m_outInfo.height=1080;
   // YangPushFactory pf;
   // YangSendVideoI* send=pf.getSendVideo(this->m_message);
   // send->putVideoRgba( data,len);
   // send->putVideoI420( data,len);

#else
    m_videoType=Yang_VideoSrc_Camera;//Yang_VideoSrc_Camera/Yang_VideoSrc_Screen;
#endif
    m_isStartpush=0;

    m_win0=NULL;
    m_isVr=0;


    m_hb0=new QHBoxLayout();
    ui->vdMain->setLayout(m_hb0);
    m_win0=new YangPlayWidget(this);
    m_hb0->addWidget(m_win0);
    m_hb0->setMargin(0);
    m_hb0->setSpacing(0);

    char s[128]={0};
    memset(m_context->avinfo.sys.localIp,0,sizeof(m_context->avinfo.sys.localIp));
    yang_getLocalInfo(m_context->avinfo.sys.localIp);
    sprintf(s,"webrtc://%s/live/livestream",m_context->avinfo.sys.localIp);
    ui->m_url->setText(s);

    m_isDrawmouse=true; //screen draw mouse
    m_screenInternal=33;
    m_hasAudio=m_videoType==Yang_VideoSrc_Screen?false:true;

    m_isStartRecord=false;
    m_filename=NULL;
    m_initRecord=false;


    //using datachannel
    if(m_context->avinfo.sys.mediaServer!=Yang_Server_Srs){
        m_context->avinfo.rtc.enableDatachannel=yangtrue;
        m_context->channeldataRecv.context=this;
        m_context->channeldataRecv.receiveData=g_qt_push_receiveData;

    }else{
             m_context->avinfo.rtc.enableDatachannel=yangfalse;
      }
    //using h264 h265
    m_context->avinfo.video.videoEncoderType=Yang_VED_264;//Yang_VED_265;
    if(m_context->avinfo.video.videoEncoderType==Yang_VED_265){
           m_context->avinfo.enc.createMeta=1;
           m_context->avinfo.rtc.enableDatachannel=yangfalse;
    }
    m_context->avinfo.rtc.iceCandidateType=YangIceHost;

   // m_hasAudio=false;

    //srs do not use audio fec
    m_context->avinfo.audio.enableAudioFec=yangfalse;

}

RecordMainWindow::~RecordMainWindow()
{
    yang_deleteA(m_filename);
    closeAll();

}

void RecordMainWindow::closeEvent( QCloseEvent * event ){

    closeAll();
    exit(0);
}

void RecordMainWindow::screenEvent(char* str)
{


}
void RecordMainWindow::setRecvText(char* data,int32_t nb){
    ui->m_l_recv->setText(data);
}
void RecordMainWindow::receiveSysMessage(YangSysMessage *mss, int32_t err){
    switch (mss->messageId) {
    case YangM_Push_Connect:
        {
            if(err){
                ui->m_b_rec->setText("开始");
                m_isStartpush=!m_isStartpush;
                ui->m_l_err->setText("push error("+QString::number(err)+")!");
               // QMessageBox::about(NULL,  "error","push error("+QString::number(err)+")!");
            }
        }
        break;
   case YangM_Push_Disconnect:
        break;
    case YangM_Push_StartScreenCapture:
        if(m_context->avinfo.video.videoEncoderFormat==YangArgb)
            m_rt->m_videoBuffer=NULL;
        else
             m_rt->m_videoBuffer=m_pushfactory.getPreVideoBuffer(m_message);
          qDebug()<<"message==="<<m_message<<"..prevideobuffer==="<<m_rt->m_videoBuffer<<"....ret===="<<err;
         break;
    case YangM_Push_StartVideoCapture:
    {
        m_rt->m_videoBuffer=m_pushfactory.getPreVideoBuffer(m_message);
        qDebug()<<"message==="<<m_message<<"..prevideobuffer==="<<m_rt->m_videoBuffer<<"....ret===="<<err;
         break;
     }
    case YangM_Push_StartOutCapture:
            {
                m_rt->m_videoBuffer=m_pushfactory.getPreVideoBuffer(m_message);
                qDebug()<<"message==="<<m_message<<"..prevideobuffer==="<<m_rt->m_videoBuffer<<"....ret===="<<err;
                 break;
             }
    case YangM_Push_SwitchToCamera:
         m_rt->m_videoBuffer=m_pushfactory.getPreVideoBuffer(m_message);
         break;
    case YangM_Push_SwitchToScreen:
        m_rt->m_videoBuffer=m_pushfactory.getPreVideoBuffer(m_message);
         break;
    }


}
void RecordMainWindow::initVideoThread(YangRecordThread *prt){
    m_rt=prt;
    m_rt->m_video=m_win0;
    m_rt->initPara(m_context);

}
void RecordMainWindow::closeAll(){

    if(m_context){
        m_rt->stopAll();
        m_rt=NULL;
        yang_delete(m_message);
        yang_delete(m_context);
        delete ui;
    }
}

void RecordMainWindow::initPreview(){
    if(m_videoType==Yang_VideoSrc_Screen) {
        ui->m_c_screen->setCheckState(Qt::Checked);
        ui->m_c_screen->setEnabled(false);
        yang_post_message(YangM_Push_StartScreenCapture,0,NULL);
    }else if(m_videoType==Yang_VideoSrc_Camera){
        yang_post_message(YangM_Push_StartVideoCapture,0,NULL);
    }else if(m_videoType==Yang_VideoSrc_OutInterface){
        yang_post_message(YangM_Push_StartOutCapture,0,NULL);
    }


}


void RecordMainWindow::init() {
    m_context->avinfo.audio.enableMono=yangfalse;
    m_context->avinfo.audio.sample=48000;
    m_context->avinfo.audio.channel=2;
    m_context->avinfo.audio.enableAec=yangfalse;
    m_context->avinfo.audio.audioCacheNum=8;
    m_context->avinfo.audio.audioCacheSize=8;
    m_context->avinfo.audio.audioPlayCacheNum=8;

    m_context->avinfo.video.videoCacheNum=10;
    m_context->avinfo.video.evideoCacheNum=10;
    m_context->avinfo.video.videoPlayCacheNum=10;

    m_context->avinfo.audio.audioEncoderType=Yang_AED_OPUS;
    m_context->avinfo.sys.rtcLocalPort=17000;
    m_context->avinfo.enc.enc_threads=4;

    memcpy(&m_screenInfo,&m_context->avinfo.video,sizeof(YangVideoInfo));
    QDesktopWidget* desk=QApplication::desktop();
    m_screenWidth=desk->screenGeometry().width();
    m_screenHeight=desk->screenGeometry().height();
    m_screenInfo.width=m_screenWidth;
    m_screenInfo.height=m_screenHeight;
    m_screenInfo.outWidth=m_screenWidth;
    m_screenInfo.outHeight=m_screenHeight;

}

void RecordMainWindow::success(){

}
void RecordMainWindow::failure(int32_t errcode){
      ui->m_l_err->setText("push error("+QString::number(errcode)+")!");
}

void RecordMainWindow::on_m_b_rec_clicked()
{
   // ui->m_b_record->setEnabled(false);
    if(!m_isStartpush){
        ui->m_l_err->setText("");
        ui->m_b_rec->setText("停止");
        m_isStartpush=!m_isStartpush;
        qDebug()<<"url========="<<ui->m_url->text().toLatin1().data();
        m_url=ui->m_url->text().toLatin1().data();
        yang_post_message(YangM_Push_Connect,0,NULL,(void*)m_url.c_str());
    }else{
        ui->m_b_rec->setText("开始");
       yang_post_message(YangM_Push_Disconnect,0,NULL);
              m_isStartpush=!m_isStartpush;
    }

}



void RecordMainWindow::switchToCamera(){
      yang_post_message(YangM_Push_SwitchToCamera,0,NULL);
}
void RecordMainWindow::switchToScreen(){
     if(m_context->avinfo.video.videoEncHwType==YangV_Hw_Nvdia) m_context->avinfo.video.videoCaptureFormat=YangArgb;
     yang_post_message(YangM_Push_SwitchToScreen,0,NULL);
    ui->m_c_screen->setCheckState(Qt::Checked);
    ui->m_c_screen->setEnabled(false);

}

void RecordMainWindow::on_m_c_screen_clicked()
{
        switchToScreen();

}

void RecordMainWindow::initRecord(){
    if(!m_initRecord){
        m_context->avinfo.audio.audioEncoderType=Yang_AED_AAC;
        m_context->avinfo.audio.sample=44100;
        m_context->avinfo.enc.createMeta=1;

        m_context->avinfo.video.videoCacheNum=200;
        m_context->avinfo.video.evideoCacheNum=200;
        m_context->avinfo.audio.audioCacheNum=500;
        m_context->avinfo.audio.audioCacheSize=500;
        m_initRecord=true;
    }
}



void RecordMainWindow::on_m_b_senddc_clicked()
{
    YangFrame frame;
    memset(&frame,0,sizeof(YangFrame));
    frame.mediaType=YANG_DATA_CHANNEL_STRING;
    frame.payload=(uint8_t*)ui->m_t_data->text().toLatin1().data();
    frame.nb=strlen(ui->m_t_data->text().toLatin1().data());
    if(m_context->channeldataSend.sendData)
        m_context->channeldataSend.sendData(m_context->channeldataSend.context,&frame);
}
