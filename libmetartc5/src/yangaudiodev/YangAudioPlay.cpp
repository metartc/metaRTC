//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/yang_unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <yangaudiodev/YangAudioPlay.h>

#include <yangavutil/audio/YangMakeWave.h>
#include <yangstream/YangStreamManager.h>
YangAudioPlay::YangAudioPlay(YangContext* pcontext) {        //m_rc=0;

	m_context = pcontext;
	m_ace = NULL;
	aIndex = 0;
	m_frames = 0;
	m_channel = pcontext->avinfo.audio.channel;
	m_isStart = 0;

	m_sample = pcontext->avinfo.audio.sample;
	m_isStart=0;
	m_audioData.setInAudioBuffer(pcontext->streams.m_playBuffer);
	m_audioData.setInAudioBuffers(pcontext->streams.m_playBuffers);
	m_audioData.initPlay(pcontext->avinfo.audio.sample,pcontext->avinfo.audio.channel);
}

YangAudioPlay::~YangAudioPlay() {
	m_context = NULL;
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

