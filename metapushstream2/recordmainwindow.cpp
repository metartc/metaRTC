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
#include <QMessageBox>
#include <QDesktopWidget>
#include <QSettings>

RecordMainWindow::RecordMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RecordMainWindow)
{

    ui->setupUi(this);

    m_context=new YangContext();
    m_context->init((char*)"yang_config.ini");
    m_context->video.videoEncoderFormat=YangI420;

#if Yang_GPU_Encoding
    //using gpu encode
    m_context->video.videoEncHwType=YangV_Hw_Nvdia;//YangV_Hw_Intel,  YangV_Hw_Nvdia,
    m_context->video.videoEncoderFormat=YangArgb;//YangI420;//YangArgb;
    m_context->enc.createMeta=0;
#endif
#if Yang_HaveVr
    //using vr bg file name
    memset(m_context->bgFilename,0,sizeof(m_ini->bgFilename));
    QSettings settingsread((char*)"yang_config.ini",QSettings::IniFormat);
    strcpy(m_context->bgFilename, settingsread.value("sys/bgFilename",QVariant("d:\\bg.jpeg")).toString().toStdString().c_str());
#endif

     init();
    yang_setLogLevle(m_context->sys.logLevel);
    yang_setLogFile(m_context->sys.hasLogFile);


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
    m_videoType=Yang_VideoSrc_Camera;//Yang_VideoSrc_Camera;//;//Yang_VideoSrc_Screen;
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
    sprintf(s,"webrtc://%s:1985/live/livestream",yang_getLocalInfo().c_str());
    ui->m_url->setText(s);

    m_isDrawmouse=true; //screen draw mouse
    m_screenInternal=33;
    m_hasAudio=m_videoType==Yang_VideoSrc_Screen?false:true;

    m_isStartRecord=false;
    m_filename=NULL;
    m_initRecord=false;

    //using h264 h265
    m_context->video.videoEncoderType=Yang_VED_264;//Yang_VED_265;

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
    vector<string> commands=yang_split(str,',');
    if(commands.size() != 4)
        return ;

    string type = commands[0];
    string directionStr = commands[1];
    YangScreenKeyEvent ev;
    ev.direction=directionStr;
    ev.event=type;
    if(directionStr=="key"){
        ev.key=atoi(commands[3].c_str());
        ev.x=0;
        ev.y=0;

    }else{
        int xxx = m_screenWidth * atof(commands[2].c_str());
        int yyy = m_screenHeight * atof(commands[3].c_str());
       ev.x=xxx;
       ev.y=yyy;

      }
       if(type=="wheel") ev.wheel=atoi(directionStr.c_str());
       remoteScreenWinEvent(&ev);

}
void RecordMainWindow::receiveSysMessage(YangSysMessage *mss, int32_t err){
    switch (mss->messageId) {
    case YangM_Push_Publish_Start:
        {
            if(err){
                ui->m_b_rec->setText("开始");
                m_isStartpush=!m_isStartpush;
                QMessageBox::about(NULL,  "error","push error("+QString::number(err)+")!");
            }
        }
        break;
   case YangM_Push_Publish_Stop:
        break;
    case YangM_Push_StartScreenCapture:
        if(m_context->video.videoEncoderFormat==YangArgb)
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
    m_context->audio.usingMono=0;
    m_context->audio.sample=48000;
    m_context->audio.channel=2;
    m_context->audio.hasAec=0;
    m_context->audio.audioCacheNum=8;
    m_context->audio.audioCacheSize=8;
    m_context->audio.audioPlayCacheNum=8;

    m_context->video.videoCacheNum=10;
    m_context->video.evideoCacheNum=10;
    m_context->video.videoPlayCacheNum=10;

    m_context->audio.audioEncoderType=Yang_AED_OPUS;
    m_context->sys.rtcLocalPort=17000;
    m_context->enc.enc_threads=4;

    memcpy(&m_screenInfo,&m_context->video,sizeof(YangVideoInfo));
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
    //on_m_b_play_clicked();
    QMessageBox::aboutQt(NULL,  "push error("+QString::number(errcode)+")!");

}

void RecordMainWindow::on_m_b_rec_clicked()
{
    ui->m_b_record->setEnabled(false);
    if(!m_isStartpush){

        ui->m_b_rec->setText("停止");
        m_isStartpush=!m_isStartpush;
        qDebug()<<"url========="<<ui->m_url->text().toLatin1().data();
        m_url=ui->m_url->text().toLatin1().data();
        yang_post_message(YangM_Push_Publish_Start,0,NULL,(void*)m_url.c_str());
    }else{
        ui->m_b_rec->setText("开始");
       yang_post_message(YangM_Push_Publish_Stop,0,NULL);
              m_isStartpush=!m_isStartpush;
    }

}



void RecordMainWindow::switchToCamera(){
      yang_post_message(YangM_Push_SwitchToCamera,0,NULL);
}
void RecordMainWindow::switchToScreen(){
     if(m_context->video.videoEncHwType==YangV_Hw_Nvdia) m_context->video.videoEncoderFormat=YangArgb;
     yang_post_message(YangM_Push_SwitchToScreen,0,NULL);
    ui->m_c_screen->setCheckState(Qt::Checked);
    ui->m_c_screen->setEnabled(false);

}



//remote screen control
void RecordMainWindow::remoteScreenWinEvent(YangScreenKeyEvent* ev)
{
#ifdef _WIN32
    if(ev->event == "mousePress" && ev->direction == "left")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.leftBDown();
    }
    else if(ev->event == "mousePress" && ev->direction == "right")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.rightBDown();
    }
    if(ev->event == "mouseRelease" && ev->direction == "left")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.leftBUp();
    }
    else if(ev->event == "mouseRelease" && ev->direction == "right")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.rightBUp();
    }
    if(ev->event == "mouseDouble" && ev->direction == "left")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.leftbDClick();
    }
    else if(ev->event == "mouseDouble" && ev->direction == "right")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.rightBDbClick();
    }
    else if(ev->event == "mouseMove")
    {
        m_winMouse.moveTo(ev->x,ev->y);
    }else if(ev->event == "keyPressEvent")
    {
        keybd_event(ev->key, 0, 0, 0);

    }
    else if( ev->event == "keyReleaseEvent")
    {
        keybd_event(ev->key, 0, KEYEVENTF_KEYUP, 0);

    }else if(ev->event=="wheel"){
        m_winMouse.middleBRoll(ev->x,ev->y,ev->wheel);
    }
#endif
}


void RecordMainWindow::on_m_c_vr_clicked()
{
    m_isVr=ui->m_c_vr->checkState()==Qt::Checked?1:0;
    if(m_isVr){

        yang_post_message(YangM_Sys_Setvr,0,NULL);
    }else{

         yang_post_message(YangM_Sys_UnSetvr,0,NULL);
    }
}

void RecordMainWindow::on_m_c_screen_clicked()
{
   // if(ui->m_c_screen->checkState()==Qt::Checked){
        switchToScreen();
    //}else{
      //  switchToCamera();
  //  }
}

void RecordMainWindow::initRecord(){
    if(!m_initRecord){
        m_context->audio.audioEncoderType=Yang_AED_AAC;
        m_context->audio.sample=44100;
        m_context->enc.createMeta=1;

        m_context->video.videoCacheNum=200;
        m_context->video.evideoCacheNum=200;
        m_context->audio.audioCacheNum=500;
        m_context->audio.audioCacheSize=500;
        m_initRecord=true;
    }
}

void RecordMainWindow::on_m_b_record_clicked()
{
    ui->m_b_rec->setEnabled(false);
    initRecord();
    if(m_isStartRecord){
        yang_post_message(YangM_Push_Record_Stop,0,NULL);
         ui->m_b_record->setText("录制");
    }else{
        if(m_filename==NULL) m_filename=new char[256];
        char path[200];
        memset(m_filename,0,256);
        memset(path,0,sizeof(path));
        srand(time(0));
        int st = rand();
        yang_getCurpath(path);
#ifdef _WIN32
        sprintf(m_filename, "%s\\rec0%d.mp4", path,st);
#else
        sprintf(m_filename, "%s/rec0%d.mp4", path,st);
#endif
        ui->m_b_record->setText("停止");
        yang_post_message(YangM_Push_Record_Start,0,NULL,(void*)m_filename);
    }
    m_isStartRecord=!m_isStartRecord;
}
