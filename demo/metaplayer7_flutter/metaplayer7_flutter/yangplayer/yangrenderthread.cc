//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "yangrenderthread.h"


#include "YangPlayerDef.h"
#include <unistd.h>
#ifdef _WIN32
   // #define yang_usleep usleep
#else
    #define yang_usleep usleep
#endif
YangRenderThread::YangRenderThread()
{
    m_isLoop=0;
   
    m_sid=1;
 

    m_isStart=0;
    m_isRender=false;
    m_playWidth=640;
    m_playHeight=480;
    m_player=nullptr;

}

YangRenderThread::~YangRenderThread(){
   
    stopAll();
   
}
void YangRenderThread::stopAll(){
    if(m_isLoop){
        m_isLoop=0;
        while (m_isStart) {
           yang_usleep(1000);
        }
    }
    closeAll();
}
void YangRenderThread::stop(){
    if(m_isLoop){
        m_isLoop=0;
        while (m_isStart) {
           yang_usleep(1000);
        }
    }
    closeAll();
}

void YangRenderThread::initPara(){

}
void YangRenderThread::closeAll(){
    //clearRender();
}





void YangRenderThread::render(){

    if(m_isRender&&m_player){

            uint8_t* t_vb=m_player->getPlayBuffer(m_playWidth,m_playHeight);

        if(t_vb){
        	yang_player_refreshImg(t_vb,m_playWidth,m_playHeight);
        }

        t_vb=NULL;
    }else{
    	yang_player_refreshImg_background();

    }
}

void YangRenderThread::run(){

    m_isLoop=1;
    m_isStart=1;


    while(m_isLoop){
    	yang_usleep(10*1000);
        render();
    }
    m_isStart=0;
    // closeAll();
}
