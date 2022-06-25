//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangIni.h>
#include <yangutil/sys/YangLog.h>
//#include <yangutil/sys/YangSsl.h>
#include <string>
#include <string.h>
void yang_context_setMediaConfig(void* context,int32_t puid,YangAudioParam* audio,YangVideoParam* video){
	if(context==NULL) return;
	YangStreamManager* streams=(YangStreamManager*)context;
	streams->setMediaConfig(puid, audio, video);

}

void yang_context_sendRequest(void* context,int32_t puid,uint32_t ssrc,YangRequestType req){
	if(context==NULL) return;
	YangStreamManager* streams=(YangStreamManager*)context;
	streams->sendRequest(puid, ssrc, req);
}

class YangInitContext {
public:
	YangInitContext() {
	}

	~YangInitContext() {
	}


};

YangContext::YangContext() {
	memset(&rtcCallback,0,sizeof(YangRtcCallback));
	memset(&sendRtcMessage,0,sizeof(YangSendRtcMessage));

	rtcCallback.context=&streams;
	rtcCallback.setMediaConfig=yang_context_setMediaConfig;
	rtcCallback.sendRequest=yang_context_sendRequest;

	memset(&channeldataRecv,0,sizeof(YangChannelDataRecvI));
	memset(&channeldataSend,0,sizeof(YangChannelDataSendI));
	init();
#ifdef __ANDROID__
	nativeWindow=NULL;
#endif

}

YangContext::~YangContext() {
	yang_closeLogFile();

}
void YangContext::init(char *filename) {
#ifndef __ANDROID__
	YangIni ini;
	ini.init(filename);
	ini.initAudio(&avinfo.audio);
	ini.initVideo(&avinfo.video);
	ini.initSys(&avinfo.sys);
	ini.initEnc(&avinfo.enc);
	ini.initRtc(&avinfo.rtc);
	initExt(&ini);
#else
	yang_init_avinfo(&avinfo);
#endif

}

void YangContext::init() {
	yang_init_avinfo(&avinfo);
	initExt();
}


void YangContext::initExt() {

}
void YangContext::initExt(void *filename) {

}

YangBufferManager::YangBufferManager() {
	m_curindex = 0;
	m_size = 0;
	m_unitsize = 0;
	m_cache = NULL;
}
YangBufferManager::YangBufferManager(int32_t num, int32_t unitsize) {
	m_curindex = 0;
	m_size = 0;
	m_unitsize = 0;
	m_cache = NULL;
	init(num, unitsize);
}
YangBufferManager::~YangBufferManager() {
	yang_deleteA(m_cache);
}

void YangBufferManager::init(int32_t num, int32_t unitsize) {
	m_size = num;
	m_unitsize = unitsize;
	if (m_cache == NULL)
		m_cache = new uint8_t[m_unitsize * m_size];
}
uint8_t* YangBufferManager::getBuffer() {
	if (!m_cache)
		return NULL;
	if (m_curindex >= m_size)
		m_curindex = 0;
	return m_cache + m_unitsize * m_curindex++;
}
