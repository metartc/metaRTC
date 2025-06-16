//
// Copyright (c) 2019-2025 yanggaofeng
//
#include <yangrtc/YangPeerConnection7.h>


void yang_peerconn7_onConnectionStateChange(void* context, int32_t uid,YangRtcConnectionState connectionState){
	YangCallbackIce* callback=(YangCallbackIce*)context;
	if (context == NULL)	return;

	callback->onConnectionStateChange(uid, connectionState);
}

 void yang_peerconn7_onIceStateChange(void* context,int32_t uid,YangIceCandidateState iceState){
	 YangCallbackIce* callback=(YangCallbackIce*)context;
	 if (context == NULL)	return;

	callback->onIceStateChange(uid, iceState);
}

 void yang_peerconn7_onIceCandidate(void* context, int32_t uid,char* sdp){
	 YangCallbackIce* callback=(YangCallbackIce*)context;
	 if (context == NULL)	return;

	callback->onIceCandidate(uid, sdp);
}

void yang_peerconn7_onIceGatheringState(void* context, int32_t uid,YangIceGatheringState gatherState){
    YangCallbackIce* callback=(YangCallbackIce*)context;
    if (context == NULL)	return;

	callback->onIceGatheringState(uid, gatherState);
 }

 void yang_peerconn7_rtcrecv_sslAlert(void *context, int32_t uid, char *type, char *desc) {
	 YangCallbackSslAlert* callback = (YangCallbackSslAlert*)context;

	if (context == NULL || type == NULL || desc == NULL)
		return;
	if (yang_strcmp(type, "warning") == 0 && yang_strcmp(desc, "CN") == 0) {

		callback->sslCloseAlert(uid);

	}
}

void yang_peerconn7_receiveAudio(void *user, YangFrame *audioFrame) {
	YangCallbackReceive* callback=(YangCallbackReceive*)user;
	if (user == NULL || audioFrame == NULL)
		return;

	callback->receiveAudio(audioFrame);
}

void yang_peerconn7_receiveVideo(void *user, YangFrame *videoFrame) {
	YangCallbackReceive* callback=(YangCallbackReceive*)user;
	if (user == NULL || videoFrame == NULL)
		return;

	callback->receiveVideo(videoFrame);
}

void yang_peerconn7_receiveMsg(void *user, YangFrame *msgFrame) {
	YangCallbackReceive* callback=(YangCallbackReceive*)user;
	if (user == NULL)	return;

	callback->receiveMsg(msgFrame);
}

void yang_peerconn7_setMediaConfig(void *user,int32_t puid,YangAudioParam* audio,YangVideoParam* video){
	if (user == NULL)	return;
	YangCallbackRtc* callback=(YangCallbackRtc*)user;
	callback->setMediaConfig(puid, audio, video);
}
void yang_peerconn7_sendRequest(void *user,int32_t puid,uint32_t ssrc,YangRequestType req){
	YangCallbackRtc* callback = (YangCallbackRtc*)user;
	if (user == NULL)	return;

	callback->sendRequest(puid, ssrc, req);
}

 int32_t yang_peerconn7_notify(void *user, int puid) {
	if (user == NULL)
		return 1;

	return Yang_Ok;
}

YangPeerConnection7::YangPeerConnection7(YangPeerInfo* peerInfo,YangCallbackReceive* receive,YangCallbackIce* ice,YangCallbackRtc* rtc, YangCallbackSslAlert* sslAlert){

	YangPeerCallback* peerCallback=&m_peer.peerCallback;
	memset(&m_conn,0,sizeof(YangMetaConnection));

	yang_memcpy(&m_peer.peerInfo,peerInfo,sizeof(YangPeerInfo));



	peerCallback->recvCallback.context=receive;
	peerCallback->recvCallback.receiveAudio=yang_peerconn7_receiveAudio;
	peerCallback->recvCallback.receiveVideo=yang_peerconn7_receiveVideo;
	peerCallback->recvCallback.receiveMsg=yang_peerconn7_receiveMsg;

	peerCallback->iceCallback.context=ice;
	peerCallback->iceCallback.onConnectionStateChange=yang_peerconn7_onConnectionStateChange;
	peerCallback->iceCallback.onIceCandidate=yang_peerconn7_onIceCandidate;
	peerCallback->iceCallback.onIceGatheringState=yang_peerconn7_onIceGatheringState;
	peerCallback->iceCallback.onIceStateChange=yang_peerconn7_onIceStateChange;

	peerCallback->rtcCallback.context=rtc;
	peerCallback->rtcCallback.setMediaConfig=yang_peerconn7_setMediaConfig;
	peerCallback->rtcCallback.sendRequest=yang_peerconn7_sendRequest;

	peerCallback->sslCallback.context = sslAlert;
	peerCallback->sslCallback.sslAlert=yang_peerconn7_rtcrecv_sslAlert;

	m_peer.conn=NULL;
	yang_create_peer(&m_peer);
	yang_create_metaConnection(&m_conn);
}

YangPeerConnection7::~YangPeerConnection7(){
	//yang_free(m_conn);
	m_conn.close(&m_peer);
}

int32_t YangPeerConnection7::addAudioTrack(YangAudioCodec codec){
	return m_conn.addAudioTrack(&m_peer,codec);
}

int32_t YangPeerConnection7::addVideoTrack(YangVideoCodec codec){
	return m_conn.addVideoTrack(&m_peer,codec);
}

int32_t YangPeerConnection7::addTransceiver(YangMediaTrack media,YangRtcDirection direction){
	return m_conn.addTransceiver(&m_peer,media,direction);
}
int32_t YangPeerConnection7::createDataChannel(){
	return m_conn.createDataChannel(&m_peer);
}

int32_t YangPeerConnection7::generateCertificate(){
	return m_conn.generateCertificate(&m_peer);
}
int32_t YangPeerConnection7::createOffer( char **psdp){
	return m_conn.createOffer(&m_peer,psdp);
}
int32_t YangPeerConnection7::createAnswer(char* answer){
	return m_conn.createAnswer(&m_peer,answer);
}

int32_t YangPeerConnection7::setRemoteDescription(char* sdp){
	return m_conn.setRemoteDescription(&m_peer,sdp);
}
int32_t YangPeerConnection7::setLocalDescription(char* sdp){
	return m_conn.setLocalDescription(&m_peer,sdp);
}


int32_t YangPeerConnection7::close(){
	return m_conn.close(&m_peer);
}

yangbool YangPeerConnection7::isAlive(){
	return m_conn.isAlive(&m_peer);
}

yangbool YangPeerConnection7::isConnected(){
	 return m_conn.isConnected(&m_peer);
}

int32_t YangPeerConnection7::on_audio(YangFrame* audioFrame){
	return m_conn.on_audio(&m_peer,audioFrame);
}

int32_t YangPeerConnection7::on_video(YangFrame* videoFrame){
	return m_conn.on_video(&m_peer,videoFrame);
}

int32_t YangPeerConnection7::on_message(YangFrame* msgFrame){
	return m_conn.on_message(&m_peer,msgFrame);
}

int32_t YangPeerConnection7::addIceCandidate(char* candidateStr){
	return m_conn.addIceCandidate(&m_peer,candidateStr);
}

int32_t YangPeerConnection7::sendRequestPli(){
	 return m_conn.sendRequestPli(&m_peer);
}


