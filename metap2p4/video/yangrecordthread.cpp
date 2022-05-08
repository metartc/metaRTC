//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "yangrecordthread.h"
#include <QDebug>
#include <QMapIterator>

YangRecordThread::YangRecordThread()
{
    m_isLoop=0;
    m_local_video=nullptr;
    m_remote_video=nullptr;
    m_videoBuffer=nullptr;
    m_playVideoBuffer=nullptr;
    m_bgColor={0,0,0};
    m_textColor={0,0,255};

    m_videoPlayNum=5;
    m_sid=1;
    showType=1;
    m_isStart=0;
    m_playWidth=640;
    m_playHeight=480;
    m_playBuffer=new uint8_t[m_playWidth*m_playHeight*3/2];
    m_renderPlay=false;
    m_message=nullptr;
}

YangRecordThread::~YangRecordThread(){
   m_local_video=nullptr;
   m_videoBuffer=nullptr;
   stopAll();
   yang_deleteA(m_playBuffer);
}
void YangRecordThread::stopAll(){
    if(m_isLoop){
        m_isLoop=0;
        while (m_isStart) {
            QThread::msleep(1);
        }
    }
     closeAll();
}
void YangRecordThread::initPara(YangContext *pini){

    m_para=pini;
    m_videoPlayNum=pini->avinfo.video.videoPlayCacheNum;
}
void YangRecordThread::closeAll(){
    //clearRender();
}


void YangRecordThread::render(){
    uint8_t* t_vb=NULL;
    if(m_videoBuffer&&m_videoBuffer->size()>0){
         t_vb=m_videoBuffer->getVideoRef(&m_frame);
        if(t_vb&&m_local_video&&m_videoBuffer->m_width>0){
            m_local_video->playVideo(t_vb,m_videoBuffer->m_width,m_videoBuffer->m_height);
        }
        t_vb=NULL;
    }
    if(!m_renderPlay) {
        m_playVideoBuffer=nullptr;
        return;
    }
    if(m_playVideoBuffer){

       if(m_playVideoBuffer->size()==0) return;
        YangVideoBuffer* play=m_playVideoBuffer->at(0);
           t_vb=play->getVideoRef(&m_frame);
            if(t_vb&&m_remote_video){
                m_remote_video->playVideo(t_vb,play->m_width,play->m_height);
            }
            t_vb=NULL;
        }else{
            if(m_message) m_playVideoBuffer=m_p2pfactory.getPlayVideoBuffer(m_message);
           // if(m_remote_video) m_remote_video->playVideo(m_playBuffer,m_playWidth,m_playHeight);
        }
}

void YangRecordThread::run(){
    m_isLoop=1;
    m_isStart=1;

    while(m_isLoop){

        QThread::msleep(20); 
        render();
    }
    m_isStart=0;

}
