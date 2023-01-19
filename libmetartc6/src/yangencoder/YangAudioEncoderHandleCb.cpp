//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangencoder/YangAudioEncoderHandleCb.h>
#include <yangencoder/YangEncoderFactory.h>
#include <yangavutil/audio/YangMakeWave.h>
#include <yangutil/yangavinfotype.h>


YangAudioEncoderHandleCb::YangAudioEncoderHandleCb(YangAudioInfo *pcontext) {
	m_isInit = 0;
	m_isStart = 0;
	m_isConvert = 1;
	m_in_audioBuffer = NULL;
	m_enc = NULL;

	m_context = pcontext;
	m_cb=NULL;
	m_uid=0;
}

YangAudioEncoderHandleCb::~YangAudioEncoderHandleCb(void) {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	yang_delete(m_enc);
	m_context = NULL;
	m_in_audioBuffer = NULL;
	m_cb=NULL;

}
void YangAudioEncoderHandleCb::stop() {
	stopLoop();
}

void YangAudioEncoderHandleCb::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

void YangAudioEncoderHandleCb::init() {
	if (m_isInit)
		return;
	YangEncoderFactory ydf;
	if (!m_enc)
		m_enc = ydf.createAudioEncoder(m_context);
	m_enc->init(m_context);
	m_isInit = 1;

}
void YangAudioEncoderHandleCb::setCallback(YangEncoderCallback* pcb){
	m_cb=pcb;
}
void YangAudioEncoderHandleCb::setInAudioBuffer(YangAudioBuffer *pbuf) {
	m_in_audioBuffer = pbuf;
}



void YangAudioEncoderHandleCb::startLoop() {
	m_isConvert = 1;


	uint8_t t_buf1[4096];
	yang_reindex(m_in_audioBuffer);
	YangFrame audioFrame;
	memset(&audioFrame,0,sizeof(YangFrame));
	audioFrame.uid=m_uid;
	while (m_isConvert == 1) {
		if (m_in_audioBuffer->size() == 0) {
			yang_usleep(200);
			continue;
		}
		audioFrame.payload=t_buf1;
		if (m_in_audioBuffer->getAudio(&audioFrame)) {
			yang_usleep(2000);
			continue;
		} else {

			m_enc->encoder(&audioFrame, m_cb);

		}
	}			//end

}

void YangAudioEncoderHandleCb::stopLoop() {
	m_isConvert = 0;

}

