//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangplayer/YangPlayerPlay.h>
#include <yangplayer/YangPlayFactory.h>
#include <yangutil/sys/YangLog.h>
#include <yangaudiodev/android/YangAudioPlayAndroid.h>

YangPlayerPlay::YangPlayerPlay() {
	m_audioPlay=NULL;
	vm_audio_player_start=0;
}

YangPlayerPlay::~YangPlayerPlay() {
    yang_stop(m_audioPlay);
    yang_stop_thread(m_audioPlay);

}
void YangPlayerPlay::stopAll(){
	if(m_audioPlay)	m_audioPlay->stop();

}
void YangPlayerPlay::initAudioPlay(YangContext* paudio){

	if (m_audioPlay == NULL) {
#ifdef _WIN32
    m_audioPlay = new YangWinAudioApiRender(paudio);
#else
#ifdef __ANDROID__
    m_audioPlay = new YangAudioPlayAndroid(paudio);
#else
		m_audioPlay = new YangAudioPlayLinux(paudio);
#endif
#endif
		m_audioPlay->init();

	}

}

void YangPlayerPlay::startAudioPlay(){

	if(vm_audio_player_start) return;

	m_audioPlay->start();
	vm_audio_player_start=1;
}
void YangPlayerPlay::setInAudioList(YangAudioPlayBuffer *paudioList){
	if(m_audioPlay!=NULL) m_audioPlay->setAudioBuffer(paudioList);
}

