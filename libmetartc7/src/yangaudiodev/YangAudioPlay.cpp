//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangaudiodev/YangAudioPlay.h>
#include <yangstream/YangStreamManager.h>

YangAudioPlay::YangAudioPlay(YangAVInfo* avinfo,YangSynBufferManager* streams) {        //m_rc=0;

	m_avinfo = avinfo;
	m_ace = NULL;
	aIndex = 0;
	m_frames = 0;
	m_channel = m_avinfo->audio.channel;
	m_isStart = 0;

	m_sample = m_avinfo->audio.sample;
	m_isStart=0;
    m_audioData.setContext(streams);
	m_audioData.setInAudioBuffer(streams->session->playBuffer);

	m_audioData.initPlay(m_avinfo->audio.sample,m_avinfo->audio.channel);
}

YangAudioPlay::~YangAudioPlay() {
	m_avinfo = NULL;
	m_ace = NULL;
}
void YangAudioPlay::run() {
	m_isStart=1;
	startLoop();
	m_isStart=0;
}
void YangAudioPlay::stop() {
	stopLoop();
}
void YangAudioPlay::setAudioBuffers(vector<YangAudioPlayBuffer*> *pal) {

}
void YangAudioPlay::setAudioBuffer(YangAudioPlayBuffer* pal) {

}
void YangAudioPlay::setAecBase(YangRtcAec *pace) {
	if (pace != NULL)
		m_aecInit = 1;
	m_ace = pace;
}

