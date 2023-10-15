//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "yangrecordthread.h"

#include <yangutil/sys/YangLog.h>
#include "YangLvglDef.h"
YangRecordThread::YangRecordThread()
{
    m_isLoop=yangfalse;
    m_syn=NULL;
    m_sid=1;
    showType=1;

    m_isStart=0;
    m_isRender=false;
    m_playWidth=640;
    m_playHeight=480;
    m_playBuffer=new uint8_t[m_playWidth*m_playHeight*4];
    //memset(m_playBuffer,60,m_playWidth*m_playHeight*4);
    uint8_t* p=NULL;

    for(int j=0;j<m_playHeight;j++){
		for(int i=0;i<m_playWidth;i++){
			p=m_playBuffer+j*m_playWidth*4+i*4;
			*p=0;
			*(p+1)=255;
			*(p+2)=0;
			*(p+3)=255;
		}
    }

}

YangRecordThread::~YangRecordThread(){
    m_syn=NULL;
    stopAll();
    yang_deleteA(m_playBuffer);
}
void YangRecordThread::stopAll(){
    if(m_isLoop){
        m_isLoop=0;
        while (m_isStart) {
           yang_usleep(1000);
        }
    }
    closeAll();
}
void YangRecordThread::stop(){
    if(m_isLoop){
        m_isLoop=yangfalse;
        while (m_isStart) {
           yang_usleep(1000);
        }
    }
    closeAll();
}

void YangRecordThread::initPara(){

}
void YangRecordThread::closeAll(){
    //clearRender();
}


void YangRecordThread::setBlackBk(){

}


void YangRecordThread::render(){
    if(m_isRender&&m_syn){

            uint8_t* t_vb=m_syn->getVideoRef(m_syn->session,&m_frame);

        if(t_vb){
        	yang_lvgl_refreshImg(t_vb,0,m_syn->width(m_syn->session),m_syn->height(m_syn->session));
        }

        t_vb=NULL;
    }else{
    	yang_lvgl_refreshImg_background(m_playBuffer,0,m_playWidth,m_playHeight);

    }
}

void YangRecordThread::run(){

    m_isLoop=yangtrue;
    m_isStart=yangtrue;


    while(m_isLoop){
    	yang_usleep(10*1000);
        render();
    }
    m_isStart=yangfalse;
    // closeAll();
}
