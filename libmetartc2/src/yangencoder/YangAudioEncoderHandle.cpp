#include "yangencoder/YangAudioEncoderHandle.h"

#include "malloc.h"
#include <yangutil/yang_unistd.h>
#include <yangavutil/audio/YangMakeWave.h>
#include <yangutil/yangavinfotype.h>
#include "stdlib.h"
#include "yangencoder/YangEncoderFactory.h"

YangAudioEncoderHandle::YangAudioEncoderHandle(YangAudioInfo *pcontext) {
	m_isInit = 0;
	m_isStart = 0;
	m_isConvert = 1;
	m_in_audioBuffer = NULL;
	m_enc = NULL;
	m_out_audioBuffer = NULL;
	m_context = pcontext;
	m_uid=0;

}

YangAudioEncoderHandle::~YangAudioEncoderHandle(void) {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	yang_delete(m_enc);
	m_context = NULL;
	m_in_audioBuffer = NULL;
	m_out_audioBuffer = NULL;
}
void YangAudioEncoderHandle::stop() {
	stopLoop();
}

void YangAudioEncoderHandle::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

void YangAudioEncoderHandle::init() {
	if (m_isInit)
		return;
	YangEncoderFactory ydf;
	if (!m_enc)
		m_enc = ydf.createAudioEncoder(m_context);
	m_enc->init(m_context);
	m_isInit = 1;

}

void YangAudioEncoderHandle::setInAudioBuffer(YangAudioBuffer *pbuf) {
	m_in_audioBuffer = pbuf;
}
void YangAudioEncoderHandle::setOutAudioBuffer(YangAudioEncoderBuffer *pbuf) {
	m_out_audioBuffer = pbuf;
}

void YangAudioEncoderHandle::onVideoData(YangFrame* pframe) {

}
void YangAudioEncoderHandle::onAudioData(YangFrame* pframe) {

	m_out_audioBuffer->putAudio(pframe);
}
void YangAudioEncoderHandle::startLoop() {
    if(m_in_audioBuffer==NULL) return;
	m_isConvert = 1;
	uint8_t t_buf1[4096];

	yang_reindex(m_in_audioBuffer);
	yang_reindex(m_out_audioBuffer);
	YangFrame audioFrame;
	memset(&audioFrame,0,sizeof(YangFrame));
	while (m_isConvert == 1) {
		if (m_in_audioBuffer->size() == 0) {
			yang_usleep(200);
			continue;
		}
		audioFrame.payload=t_buf1;
		audioFrame.uid=m_uid;
		audioFrame.nb=0;
		if (m_in_audioBuffer->getAudio(&audioFrame)) {
			yang_usleep(1000);
			continue;
		} else {
			m_enc->encoder(&audioFrame, this);

		}
	}			//end

}

void YangAudioEncoderHandle::stopLoop() {
	m_isConvert = 0;

}
