//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangIni.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsl.h>
#include <string>
#include <string.h>

class YangInitContext {
public:
	YangInitContext() {
	}

	~YangInitContext() {
	}


};

YangContext::YangContext() {
	init();
	//m_certificate=NULL;
}

YangContext::~YangContext() {
	yang_closeLogFile();
	//yang_delete(m_certificate);
}
void YangContext::init(char *filename) {
	YangIni ini;
	ini.init(filename);
	ini.initAudio(&avinfo.audio);
	ini.initVideo(&avinfo.video);
	ini.initSys(&avinfo.sys);
	ini.initEnc(&avinfo.enc);
	ini.initRtc(&avinfo.rtc);
	initExt(&ini);
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
