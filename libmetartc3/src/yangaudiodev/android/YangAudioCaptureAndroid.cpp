
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifdef __ANDROID__
#include <yangaudiodev/android/YangAudioCaptureAndroid.h>
#include <yangavutil/audio/YangMakeWave.h>
#include <yangavutil/audio/YangAudioUtil.h>
#include "stdlib.h"
#include <stdio.h>
#include "memory.h"
#include "malloc.h"

YangAudioCaptureAndroid::YangAudioCaptureAndroid(YangContext *pcontext) //:YangAudioCapture(pcontext)
		{

	m_context = pcontext;
    m_ahandle = new YangAudioCaptureHandle(pcontext);
	aIndex = 0;
	m_size = 0;
	m_loops = 0;
	m_channel = pcontext->avinfo.audio.channel;
	m_sample = pcontext->avinfo.audio.sample;

	m_audioAndroid=(YangAudioAndroid*)calloc(sizeof(YangAudioAndroid),1);

}

YangAudioCaptureAndroid::~YangAudioCaptureAndroid() {
	/*if (m_loops) {
		stop();
		while (m_isStart) {
			yang_usleep(500);
		}
	}*/
	closeAudio();
}
void YangAudioCaptureAndroid::closeAudio() {

	yang_destroy_audioAndroid(m_audioAndroid);
	yang_free(m_audioAndroid);

}
void YangAudioCaptureAndroid::setCatureStart() {
	m_ahandle->isBuf = 1;
}
void YangAudioCaptureAndroid::setCatureStop() {
	m_ahandle->isBuf = 0;
}
void YangAudioCaptureAndroid::setOutAudioBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->setOutAudioBuffer(pbuffer);
}
void YangAudioCaptureAndroid::setPlayAudoBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->m_aecPlayBuffer = pbuffer;
}
void YangAudioCaptureAndroid::setAec(YangRtcAec *paec) {
	m_ahandle->m_aec = paec;
}
void YangAudioCaptureAndroid::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal) {

}
void YangAudioCaptureAndroid::setPreProcess(YangPreProcess *pp) {

}


int32_t YangAudioCaptureAndroid::init() {
	yang_create_audioAndroid_record(m_audioAndroid,m_ahandle, m_sample,m_channel);
	return Yang_Ok;
}

void YangAudioCaptureAndroid::startLoop() {
	// loops = 5000000 / val;
	m_loops = 1;

	m_loops = 0;

}

void YangAudioCaptureAndroid::stopLoop() {
	m_loops = 0;
}
#endif
