/*
 * YangPlayBase.cpp
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */

#include <yangplayer/YangPlayerPlay.h>
#include <yangplayer/YangPlayFactory.h>



YangPlayerPlay::YangPlayerPlay() {
	m_audioPlay=NULL;
	vm_audio_player_start=0;
}

YangPlayerPlay::~YangPlayerPlay() {
	//m_context=NULL;
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
		m_audioPlay = new YangAudioPlayAlsa(paudio);
#endif
		m_audioPlay->init();

	}

}
/**
void YangPlayerPlay::initVideoPlay(YangVideoDecoderHandle *handle){
	if (vm_videoPlay == NULL)	{
		vm_videoPlay = new YangVideoPlay();
		vm_videoPlay->init();
		//handle->m_yvp=vm_videoPlay;
	}

}

void YangPlayerPlay::setInVideoList(vector<YangVideoBuffer*> *pvideoList){
	if(vm_videoPlay!=NULL) vm_videoPlay->setVideoList(pvideoList);//setAudioList(paudioList);
}
void YangPlayerPlay::startVideoPlay(){
	//if(vm_audio_player_start) return;
	vm_videoPlay->start();
	//vm_audio_player_start=1;
}
int32_t YangPlayerPlay::getIsAecInit(){
	if(vm_audioPlay!=NULL) return vm_audioPlay->m_aecInit;
 return 0;
}
//F_initSdlWin YangPlayApp::getInitSdlWin(){
//	return vm_contexttSdlWin;
//}

void YangPlayerPlay::setAec(YangAecBase *paec){
	if(vm_audioPlay!=NULL) vm_audioPlay->setAecBase(paec);
}
**/
void YangPlayerPlay::startAudioPlay(){
	if(vm_audio_player_start) return;
	m_audioPlay->start();
	vm_audio_player_start=1;
}
void YangPlayerPlay::setInAudioList(YangAudioPlayBuffer *paudioList){
	if(m_audioPlay!=NULL) m_audioPlay->setAudioBuffer(paudioList);
}

