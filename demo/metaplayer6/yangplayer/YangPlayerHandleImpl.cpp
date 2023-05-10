//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangPlayerHandleImpl.h"
#include <yangutil/sys/YangLog.h>

YangPlayerHandle* YangPlayerHandle::createPlayerHandle(YangContext* pcontext,YangSysMessageI* pmessage){
	return new YangPlayerHandleImpl(pcontext,pmessage);
}
YangPlayerHandleImpl::YangPlayerHandleImpl(YangContext* pcontext,YangSysMessageI* pmessage) {
	m_context=pcontext;
	m_message=pmessage;
    //m_recv = NULL;
	m_play = NULL;
	m_rtcRecv=NULL;
	m_outVideoBuffer = NULL;
	m_outAudioBuffer = NULL;
	m_url.netType=0;
	m_url.port=1935;
}

YangPlayerHandleImpl::~YangPlayerHandleImpl() {
	if(m_rtcRecv) m_rtcRecv->disConnect();

	yang_delete(m_play);
	yang_delete(m_rtcRecv);
	yang_delete(m_outVideoBuffer);
	yang_delete(m_outAudioBuffer);

}

void YangPlayerHandleImpl::stopPlay(){
	if(m_rtcRecv) {
		m_rtcRecv->disConnect();
	}

	if(m_play) m_play->stopAll();
	if(m_rtcRecv){
		yang_stop(m_rtcRecv);
		yang_stop_thread(m_rtcRecv);
		yang_delete(m_rtcRecv);
	}



    yang_delete(m_play);

}
int YangPlayerHandleImpl::play(char* url) {
	memset(m_url.server,0,sizeof(m_url.server));
	m_url.port=0;
    if(yang_url_parse(m_context->avinfo.sys.familyType,url,&m_url)) return 1;

	stopPlay();
	yang_trace("\nnetType==%d,server=%s,port=%d,app=%s,stream=%s\n",m_url.netType,m_url.server,m_url.port,m_url.app,m_url.stream);
	m_context->avinfo.sys.transType=m_url.netType;
    return playRtc(0,m_url.server,m_url.server,m_url.port,m_url.app,m_url.stream);

}

int32_t YangPlayerHandleImpl::playRtc(int32_t puid,char* localIp,char* server, int32_t pport,char* app,char* stream){

	stopPlay();
	if (!m_play)	{
		m_play = new YangPlayerBase();

		m_context->avinfo.audio.sample=48000;
		m_context->avinfo.audio.channel=2;
		m_context->avinfo.audio.audioDecoderType=Yang_AED_OPUS;//3;
		m_context->avinfo.audio.enableMono=yangfalse;
        m_context->avinfo.audio.aIndex=-1;
		m_play->init(m_context);
	}
	initList();
	m_play->startAudioDecoder(m_outAudioBuffer);
	m_play->startVideoDecoder(m_outVideoBuffer);

	m_play->startAudioPlay(m_context);


	if(m_rtcRecv==NULL) {
		m_rtcRecv=new YangRtcReceive(m_context,m_message);
		m_rtcRecv->setBuffer(m_outAudioBuffer, m_outVideoBuffer);
		m_rtcRecv->init(puid,localIp,server,pport,app,stream);
	}

	 m_rtcRecv->start();

	 return Yang_Ok;
}



int32_t YangPlayerHandleImpl::playRtc(int32_t puid,char* url){

    stopPlay();
    if (!m_play)	{
        m_play = new YangPlayerBase();

        m_context->avinfo.audio.sample=48000;
        m_context->avinfo.audio.channel=2;
        m_context->avinfo.audio.audioDecoderType=Yang_AED_OPUS;//3;
        m_context->avinfo.audio.enableMono=yangfalse;
        m_context->avinfo.audio.aIndex=-1;
        m_play->init(m_context);
    }
    initList();
    m_play->startAudioDecoder(m_outAudioBuffer);
    m_play->startVideoDecoder(m_outVideoBuffer);

    m_play->startAudioPlay(m_context);


    if(m_rtcRecv==NULL) {
        m_rtcRecv=new YangRtcReceive(m_context,m_message);
        m_rtcRecv->setBuffer(m_outAudioBuffer, m_outVideoBuffer);
        m_rtcRecv->init(puid,url);
    }

     m_rtcRecv->start();

     return Yang_Ok;
}

YangVideoBuffer* YangPlayerHandleImpl::getVideoBuffer(){
	if(m_play) return m_play->m_ydb->getOutVideoBuffer();
	return NULL;
}

void YangPlayerHandleImpl::initList() {
	if (m_outAudioBuffer == NULL) {
		m_outAudioBuffer = new YangAudioEncoderBuffer(10);
	}
	if (m_outVideoBuffer == NULL)
		m_outVideoBuffer = new YangVideoDecoderBuffer(8);

}
