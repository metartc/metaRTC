//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangstream/YangStreamManager.h>
#include <yangutil/sys/YangLog.h>

YangStreamManager::YangStreamManager(YangSynBufferManager* synMgr) {

	m_sendRequestCb=NULL;
	m_mediaConfig_dec=NULL;
	m_mediaConfig_render=NULL;


	m_audioClock=48000;
	m_videoClock=90000;
	m_streamState=NULL;
	m_synMgr=synMgr;
}

YangStreamManager::~YangStreamManager() {

	m_sendRequestCb=NULL;
	m_mediaConfig_dec=NULL;
	m_mediaConfig_render=NULL;



}

int32_t YangStreamManager::getAudioClock(){
	return m_audioClock;
}



int32_t YangStreamManager::getVideoClock(){
	return m_videoClock;
}

void YangStreamManager::setSendRequestCallback(YangSendRequestCallback* request){
	m_sendRequestCb=request;
}

void YangStreamManager::setDecoderMediaConfigCallback(YangMediaConfigCallback* dec){
	m_mediaConfig_dec=dec;
}

void YangStreamManager::setRenderMediaConfigCallback(YangMediaConfigCallback* render){
	m_mediaConfig_render=render;
}

void YangStreamManager::setMediaConfig(int32_t puid,YangAudioParam* audio,YangVideoParam* video){
	if(m_mediaConfig_dec) m_mediaConfig_dec->setMediaConfig(puid,audio,video);
	if(m_mediaConfig_render) m_mediaConfig_render->setMediaConfig(puid,audio,video);
	if(m_synMgr==NULL) return;
	int32_t i=0;
	if(audio){
		if(m_synMgr->session->playBuffer) m_synMgr->session->playBuffer->setAudioClock(m_synMgr->session->playBuffer->session,audio->audioClock);
		if(m_synMgr->session->playBuffers){
			for(i=0;i<m_synMgr->session->playBuffers->vec.vsize;i++){
				m_synMgr->session->playBuffers->vec.payload[i]->setAudioClock(m_synMgr->session->playBuffers->vec.payload[i]->session,audio->audioClock);
			}
		}
		m_audioClock=audio->audioClock;
	}
	if(video){
		if(m_synMgr->session->playBuffer) m_synMgr->session->playBuffer->setVideoClock(m_synMgr->session->playBuffer->session,video->videoClock);
		if(m_synMgr->session->playBuffers){
			for(i=0;i<m_synMgr->session->playBuffers->vec.vsize;i++){
				m_synMgr->session->playBuffers->vec.payload[i]->setVideoClock(m_synMgr->session->playBuffers->vec.payload[i]->session,video->videoClock);
			}
		}
		m_videoClock=video->videoClock;
	}

}

void YangStreamManager::sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req){
	if(m_sendRequestCb) m_sendRequestCb->sendRequest(puid,ssrc,req);
}




int32_t YangStreamManager::getPushOnlineCount(){
	return m_onlinePushUsers.size();
}
int32_t YangStreamManager::getPlayOnlineCount(){
	return m_onlinePlayUsers.size();
}
void YangStreamManager::connectNotify(int32_t puid,YangRtcDirection opt,bool isConnect){

	size_t i=0;

	if(opt==YangRecvonly||opt==YangSendrecv){
		for(i=0;i<m_onlinePlayUsers.size();i++){
			if(puid==m_onlinePlayUsers.at(i)) 	m_onlinePlayUsers.erase(m_onlinePlayUsers.begin()+i);
		}
		if(isConnect) m_onlinePlayUsers.push_back(puid);
	}
	if(opt==YangSendonly||opt==YangSendrecv){
		for(i=0;i<m_onlinePushUsers.size();i++){
			if(puid==m_onlinePushUsers.at(i)) 	m_onlinePushUsers.erase(m_onlinePushUsers.begin()+i);
		}
		if(isConnect) m_onlinePushUsers.push_back(puid);
	}
	if(m_streamState) m_streamState->streamStateNotify(puid, opt,isConnect);
}
