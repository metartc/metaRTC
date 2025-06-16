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
#if Yang_OS_WIN
    m_audioPlay = new YangWinAudioApiRender(&paudio->avinfo,&paudio->synMgr);
#else
#if Yang_OS_ANDROID
    m_audioPlay = new YangAudioPlayAndroid(&paudio->avinfo,&paudio->synMgr);
#else
        #if Yang_OS_APPLE
            m_audioPlay = new YangAudioPlayMac(&paudio->avinfo,&paudio->synMgr);
        #else
            m_audioPlay = new YangAudioPlayLinux(&paudio->avinfo,&paudio->synMgr);
        #endif
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

