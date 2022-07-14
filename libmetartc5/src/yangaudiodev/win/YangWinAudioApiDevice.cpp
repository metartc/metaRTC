//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/win/YangWinAudioApiDevice.h>
#ifdef _WIN32
#include <yangutil/yang_unistd.h>
#include <yangutil/yangtype.h>
#include <yangavutil/audio/YangAudioUtil.h>
#include <yangaudiodev/win/YangWinAudioApiAec.h>

YangWinAudioApiDevice::YangWinAudioApiDevice(YangContext *pcontext,bool isRecord,
		bool usingBuiltinAec) {
	m_usingBuiltinAec = usingBuiltinAec;
	m_isStart = 0;
	m_isInit = 0;
	m_loops = 0;
	m_player=NULL;
    m_context=pcontext;
#ifdef _MSC_VER
	if (m_usingBuiltinAec&&!isRecord)
        m_capture = new YangWinAudioApiAec();
	else{
#endif
		m_capture = new YangWinAudioApiCapture(pcontext);
#ifdef _MSC_VER
	}
#else
	m_usingBuiltinAec=false;
#endif
	m_outLen=640;
	m_ahandle = new YangAudioCaptureHandle(pcontext);
    if(!isRecord) m_player = new YangWinAudioApiRender(pcontext);

}

YangWinAudioApiDevice::~YangWinAudioApiDevice() {


	yang_stop(m_player);
	yang_stop(m_capture);
	yang_stop_thread(m_player);
	yang_stop_thread(m_capture);
	yang_delete(m_ahandle);
	yang_delete(m_player);
	yang_delete(m_capture);


}

void YangWinAudioApiDevice::setCatureStart() {
	if(m_ahandle) m_ahandle->isBuf = 1;
}
void YangWinAudioApiDevice::setCatureStop() {
	if(m_ahandle) m_ahandle->isBuf = 0;
}
void YangWinAudioApiDevice::setOutAudioBuffer(YangAudioBuffer *pbuffer) {
	if(m_ahandle) m_ahandle->setOutAudioBuffer(pbuffer);
}
void YangWinAudioApiDevice::setPlayAudoBuffer(YangAudioBuffer *pbuffer) {

	if(m_ahandle) m_ahandle->m_aecPlayBuffer = pbuffer;
}
void YangWinAudioApiDevice::setAec(YangRtcAec *paec) {
	if(!m_usingBuiltinAec){
		if(m_ahandle&&m_player){
			m_ahandle->m_aec = paec;
			m_player->setAec(paec);
		}

	}

}
void YangWinAudioApiDevice::setPreProcess(YangPreProcess *pp) {
if(!m_usingBuiltinAec&&m_player)	m_player->m_audioData.m_preProcess = pp;

}

void YangWinAudioApiDevice::setInAudioBuffer(
		vector<YangAudioPlayBuffer*> *pal) {

}
void YangWinAudioApiDevice::caputureAudioData(YangFrame *audioFrame) {

#ifdef _MSC_VER
	if (m_usingBuiltinAec){
        if(m_ahandle) m_ahandle->putBuffer2(audioFrame->payload, audioFrame->nb);
	}else {
#endif
        if (m_player&&m_player->m_hasRenderEcho){
			if(m_ahandle) m_ahandle->putEchoBuffer(audioFrame->payload, m_outLen); //aec filter render data
		}else{
            if(m_ahandle) m_ahandle->putBuffer2(audioFrame->payload, m_outLen);
		}
#ifdef _MSC_VER
	}
#endif
}

void YangWinAudioApiDevice::stopLoop() {
	m_loops = 0;
	if(m_capture) m_capture->stop();
}



int YangWinAudioApiDevice::init() {
	if (m_isInit) return Yang_Ok;

	if(m_capture)		{
		m_capture->initCapture();
		m_capture->setCaptureCallback(this);
		m_outLen=m_capture->getAudioOutLength();
	}
	if(m_player) m_player->init();
	m_isInit = 1;
	return Yang_Ok;
}
void YangWinAudioApiDevice::startLoop() {



	m_loops = 1;
    if (m_player)		m_player->start();
	if(m_capture){
		m_capture->captureThread();
	}
	if (m_player)
		m_player->stop();
	if (m_capture){
		m_capture->stop();
	}


}

#endif
