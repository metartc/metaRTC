//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangstream/YangStreamManager.h>
#include <yangutil/sys/YangLog.h>
YangStreamManager::YangStreamManager() {
	m_playBuffers=NULL;
	m_playBuffer=NULL;
	m_sendRequestCb=NULL;
	m_mediaConfig_dec=NULL;
	m_mediaConfig_render=NULL;


	m_audioClock=48000;
	m_videoClock=90000;
	m_streamState=NULL;
}

YangStreamManager::~YangStreamManager() {
	yang_delete(m_playBuffer);
	if(m_playBuffers){
		for(size_t i=0;i<m_playBuffers->size();i++){
			yang_delete(m_playBuffers->at(i));
		}
		m_playBuffers->clear();
	}

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
	size_t i=0;
	if(audio){
		if(m_playBuffer) m_playBuffer->setAudioClock(audio->audioClock);
		if(m_playBuffers){
			for(i=0;i<m_playBuffers->size();i++){
				m_playBuffers->at(i)->setAudioClock(audio->audioClock);
			}
		}
		m_audioClock=audio->audioClock;
	}
	if(video){
		if(m_playBuffer) m_playBuffer->setVideoClock(video->videoClock);
		if(m_playBuffers){
			for(i=0;i<m_playBuffers->size();i++){
				m_playBuffers->at(i)->setVideoClock(video->videoClock);
			}
		}
		m_videoClock=video->videoClock;
	}
}

void YangStreamManager::sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req){
	if(m_sendRequestCb) m_sendRequestCb->sendRequest(puid,ssrc,req);
}

int YangStreamManager::getIndex(int puid){
	if(m_playBuffers){
		for(size_t i=0;i<m_playBuffers->size();i++){
			if(m_playBuffers->at(i)->m_uid==puid) return i;
		}
	}
	return -1;
}

YangSynBuffer* YangStreamManager::getSynBuffer(int puid){
	int ind=getIndex(puid);
	if(ind==-1) return NULL;
	return m_playBuffers->at(ind);
}


int32_t YangStreamManager::getPushOnlineCount(){
	return m_onlinePushUsers.size();
}
int32_t YangStreamManager::getPlayOnlineCount(){
	return m_onlinePlayUsers.size();
}
void YangStreamManager::connectNotify(int32_t puid,YangStreamOptType opt,bool isConnect){

	size_t i=0;

	if(opt==Yang_Stream_Play||opt==Yang_Stream_Both){
		for(i=0;i<m_onlinePlayUsers.size();i++){
			if(puid==m_onlinePlayUsers.at(i)) 	m_onlinePlayUsers.erase(m_onlinePlayUsers.begin()+i);
		}
		if(isConnect) m_onlinePlayUsers.push_back(puid);
	}
	if(opt==Yang_Stream_Publish||opt==Yang_Stream_Both){
		for(i=0;i<m_onlinePushUsers.size();i++){
			if(puid==m_onlinePushUsers.at(i)) 	m_onlinePushUsers.erase(m_onlinePushUsers.begin()+i);
		}
		if(isConnect) m_onlinePushUsers.push_back(puid);
	}
	if(m_streamState) m_streamState->streamStateNotify(puid, opt,isConnect);
}
