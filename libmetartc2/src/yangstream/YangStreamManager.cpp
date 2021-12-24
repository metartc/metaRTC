/*
 * YangStreamManager.cpp
 *
 *  Created on: 2021年10月2日
 *      Author: yang
 */

#include <yangstream/YangStreamManager.h>
#include <yangutil/sys/YangLog.h>
YangStreamManager::YangStreamManager() {
	m_playBuffers=NULL;
	m_playBuffer=NULL;
	m_sendPli=NULL;
	m_mediaConfig_dec=NULL;
	m_mediaConfig_render=NULL;
	m_rtcMsg=NULL;
	m_rtcMsgMap=NULL;

	m_audioClock=0;
	m_videoClock=0;
}

YangStreamManager::~YangStreamManager() {
	yang_delete(m_playBuffer);
	if(m_playBuffers){
		for(size_t i=0;i<m_playBuffers->size();i++){
			yang_delete(m_playBuffers->at(i));
		}
		m_playBuffers->clear();
	}
	m_sendPli=NULL;
	m_mediaConfig_dec=NULL;
	m_mediaConfig_render=NULL;
	if(m_rtcMsgMap){
		m_rtcMsgMap->clear();
		yang_delete(m_rtcMsgMap);
	}
}
int32_t YangStreamManager::getAudioClock(){
	return m_audioClock;
}

void YangStreamManager::sendRequest(int32_t puid, YangRtcMessageType msg) {
	if(puid>0&&m_rtcMsgMap){
		map<int32_t,YangRtcMessageNotify*>::iterator iter=m_rtcMsgMap->find(puid);
		if(iter!=m_rtcMsgMap->end()){
			yang_trace("\nuser(%d)..send pli....",puid);
			iter->second->notify(puid, msg);
		}
	}else if(m_rtcMsg) {
        yang_trace("\n..send pli....");
		m_rtcMsg->notify(puid, msg);
	}
}

	int32_t YangStreamManager::getVideoClock(){
		return m_videoClock;
	}
void YangStreamManager::setSendRequestCallback(YangSendRequestCallback* pli){
	m_sendPli=pli;
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
		if(m_sendPli) m_sendPli->sendRequest(puid,ssrc,req);
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

void YangStreamManager::setRtcMessageNotify(int puid,YangRtcMessageNotify *rtcmsg) {

	if(puid>0){
		if(m_rtcMsgMap==NULL) m_rtcMsgMap=new map<int32_t,YangRtcMessageNotify*>();
		map<int32_t,YangRtcMessageNotify*>::iterator iter=m_rtcMsgMap->find(puid);
			if(iter==m_rtcMsgMap->end()){
				(*m_rtcMsgMap)[puid]=rtcmsg;
			}
	}else{
		m_rtcMsg=rtcmsg;

	}
}
