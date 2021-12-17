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
	m_channel = pcontext->audio.channel;
	m_isStart = 0;

	m_sample = pcontext->audio.sample;
	m_isStart=0;
	m_audioData.setInAudioBuffer(pcontext->streams.m_playBuffer);
	m_audioData.setInAudioBuffers(pcontext->streams.m_playBuffers);
	m_audioData.initPlay(pcontext->audio.sample,pcontext->audio.channel);
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
   // if(m_audioData.m_in_audioBuffers) m_audioData.m_in_audioBuffers->setInAudioBuffers(pal);
	//m_in_audioBuffer = pal;
}
void YangAudioPlay::setAudioBuffer(YangAudioPlayBuffer* pal) {
	//m_buf=pal;
    //if(m_audioData.m_syn) m_audioData.m_syn->setInAudioBuffer(pal);
	//m_in_audioBuffer = pal;
}
void YangAudioPlay::setAecBase(YangAecBase *pace) {
	if (pace != NULL)
		m_aecInit = 1;
	m_ace = pace;
}

