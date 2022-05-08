//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/YangAudioCaptureData.h>
#include <yangutil/sys/YangLog.h>
#include <yangavutil/audio/YangAudioUtil.h>

YangAudioCaptureData::YangAudioCaptureData() {
	m_cacheLen = 1024 * 4 * 4;
	m_cache = new uint8_t[m_cacheLen];
	m_size = 0;
	m_pos = 0;
	m_cb = NULL;
	memset(&m_res,0,sizeof(YangAudioResample));
	yang_create_audioresample(&m_res);
	m_res.init(m_res.context,48000, 2, 48000, 2, 20);
	memset(&m_audioFrame,0,sizeof(YangFrame));
}

YangAudioCaptureData::~YangAudioCaptureData() {
	yang_deleteA(m_cache);
	m_cb = NULL;
	 yang_destroy_audioresample(&m_res);
}

void YangAudioCaptureData::initIn(int psample, int pchannel) {
	m_res.initIn(m_res.context,psample, pchannel);

}

void YangAudioCaptureData::initOut(int psample, int pchannel) {
	m_res.initOut(m_res.context,psample, pchannel);
}

void YangAudioCaptureData::caputure(YangFrame *audioFrame) {
    if(m_size + audioFrame->nb > m_cacheLen) return;
	if ((m_pos + m_size + audioFrame->nb) >= m_cacheLen) {
		memmove(m_cache, m_cache + m_pos, m_size);
		m_pos = 0;
	}

	if (audioFrame->payload && audioFrame->nb > 0) {

		memcpy(m_cache + m_pos + m_size, audioFrame->payload, audioFrame->nb);
		m_size += audioFrame->nb;
	}

	while(m_size >= m_res.context->inBytes) {
		if (m_pos + m_res.context->inBytes >= m_cacheLen) {
			memmove(m_cache, m_cache + m_pos, m_size);
			m_pos = 0;
		}
		captureData();

		m_pos += m_res.context->inBytes;
		m_size -= m_res.context->inBytes;
        if(m_size==0) m_pos=0;
	}

}

int YangAudioCaptureData::getOutLength(){
	return m_res.context->outBytes;
}

void YangAudioCaptureData::captureData() {
	m_audioFrame.payload=m_cache + m_pos;
	m_audioFrame.nb=m_res.context->inBytes;

	m_res.resample(m_res.context,&m_audioFrame);

	if (m_cb)
		m_cb->caputureAudioData(&m_audioFrame);

}

