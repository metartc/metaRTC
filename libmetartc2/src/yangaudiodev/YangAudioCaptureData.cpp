#include <yangaudiodev/YangAudioCaptureData.h>
#include <yangutil/sys/YangLog.h>
#include <yangavutil/audio/YangAudioUtil.h>

YangAudioCaptureData::YangAudioCaptureData() {
	m_cacheLen = 1024 * 4 * 4;
	m_cache = new uint8_t[m_cacheLen];
	m_size = 0;
	m_pos = 0;
	m_cb = NULL;
	m_res.init(48000, 2, 48000, 2, 20);
	memset(&m_audioFrame,0,sizeof(YangFrame));
}

YangAudioCaptureData::~YangAudioCaptureData() {
	yang_deleteA(m_cache);
	m_cb = NULL;
}

void YangAudioCaptureData::initIn(int psample, int pchannel) {
	m_res.initIn(psample, pchannel);

}

void YangAudioCaptureData::initOut(int psample, int pchannel) {
	m_res.initOut(psample, pchannel);
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

	while(m_size >= m_res.getInBytes()) {
		if (m_pos + m_res.getInBytes() >= m_cacheLen) {
			memmove(m_cache, m_cache + m_pos, m_size);
			m_pos = 0;
		}
		captureData();

		m_pos += m_res.getInBytes();
		m_size -= m_res.getInBytes();
	}

}

int YangAudioCaptureData::getOutLength(){
	return m_res.getOutBytes();
}

void YangAudioCaptureData::captureData() {
	m_audioFrame.payload=m_cache + m_pos;
	m_audioFrame.nb=m_res.getInBytes();
	m_res.resample(&m_audioFrame);

	if (m_cb)
		m_cb->caputureAudioData(&m_audioFrame);

}

