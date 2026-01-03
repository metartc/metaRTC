//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangrtc/YangPeerConnection8.h>


void yang_peerconn8_onConnectionStateChange(void* context, int32_t uid,YangRtcConnectionState connectionState){
	YangCallbackIce* callback=(YangCallbackIce*)context;
	if (context == NULL)	return;

	callback->onConnectionStateChange(uid, connectionState);
}

 void yang_peerconn8_onIceStateChange(void* context,int32_t uid,YangIceCandidateState iceState){
	 YangCallbackIce* callback=(YangCallbackIce*)context;
	 if (context == NULL)	return;

	callback->onIceStateChange(uid, iceState);
}

 void yang_peerconn8_onIceCandidate(void* context, int32_t uid,char* sdp){
	 YangCallbackIce* callback=(YangCallbackIce*)context;
	 if (context == NULL)	return;

	callback->onIceCandidate(uid, sdp);
}

void yang_peerconn8_onIceGatheringState(void* context, int32_t uid,YangIceGatheringState gatherState){
    YangCallbackIce* callback=(YangCallbackIce*)context;
    if (context == NULL)	return;

	callback->onIceGatheringState(uid, gatherState);
 }

 void yang_peerconn8_rtcrecv_sslAlert(void *context, int32_t uid, char *type, char *desc) {
	 YangCallbackSslAlert* callback = (YangCallbackSslAlert*)context;

	if (context == NULL || type == NULL || desc == NULL)
		return;
	if (yang_strcmp(type, "warning") == 0 && yang_strcmp(desc, "CN") == 0) {

		callback->sslCloseAlert(uid);

	}
}

void yang_peerconn8_receiveAudio(void *user, YangFrame *audioFrame) {
	YangCallbackReceive* callback=(YangCallbackReceive*)user;
	if (user == NULL || audioFrame == NULL)
		return;

	callback->receiveAudio(audioFrame);
}

void yang_peerconn8_receiveVideo(void *user, YangFrame *videoFrame) {
	YangCallbackReceive* callback=(YangCallbackReceive*)user;
	if (user == NULL || videoFrame == NULL)
		return;

	callback->receiveVideo(videoFrame);
}

void yang_peerconn8_receiveMsg(void *user, YangFrame *msgFrame) {
	YangCallbackReceive* callback=(YangCallbackReceive*)user;
	if (user == NULL)	return;

	callback->receiveMsg(msgFrame);
}

void yang_peerconn8_setMediaConfig(void *user,int32_t puid,YangAudioParam* audio,YangVideoParam* video){
	if (user == NULL)	return;
	YangCallbackRtc* callback=(YangCallbackRtc*)user;
	callback->setMediaConfig(puid, audio, video);
}
void yang_peerconn8_sendRequest(void *user,int32_t puid,uint32_t ssrc,YangRequestType req){
	YangCallbackRtc* callback = (YangCallbackRtc*)user;
	if (user == NULL)	return;

	callback->sendRequest(puid, ssrc, req);
}

 int32_t yang_peerconn8_notify(void *user, int puid) {
	if (user == NULL)
		return 1;

	return Yang_Ok;
}

YangPeerConnection8::YangPeerConnection8(YangPeerInfo* peerInfo,YangCallbackReceive* receive,YangCallbackIce* ice,YangCallbackRtc* rtc, YangCallbackSslAlert* sslAlert){

	YangPeerCallback* peerCallback=&m_peer.peerCallback;
	memset(&m_conn,0,sizeof(YangMetaConnection));

	yang_memcpy(&m_peer.peerInfo,peerInfo,sizeof(YangPeerInfo));



	peerCallback->recvCallback.context=receive;
	peerCallback->recvCallback.receiveAudio=yang_peerconn8_receiveAudio;
	peerCallback->recvCallback.receiveVideo=yang_peerconn8_receiveVideo;
	peerCallback->recvCallback.receiveMsg=yang_peerconn8_receiveMsg;

	peerCallback->iceCallback.context=ice;
	peerCallback->iceCallback.onConnectionStateChange=yang_peerconn8_onConnectionStateChange;
	peerCallback->iceCallback.onIceCandidate=yang_peerconn8_onIceCandidate;
	peerCallback->iceCallback.onIceGatheringState=yang_peerconn8_onIceGatheringState;
	peerCallback->iceCallback.onIceStateChange=yang_peerconn8_onIceStateChange;

	peerCallback->rtcCallback.context=rtc;
	peerCallback->rtcCallback.setMediaConfig=yang_peerconn8_setMediaConfig;
	peerCallback->rtcCallback.sendRequest=yang_peerconn8_sendRequest;

	peerCallback->sslCallback.context = sslAlert;
	peerCallback->sslCallback.sslAlert=yang_peerconn8_rtcrecv_sslAlert;

	m_peer.conn=NULL;
	yang_create_peer(&m_peer);
	yang_create_metaConnection(&m_conn);
}

YangPeerConnection8::~YangPeerConnection8(){
	//yang_free(m_conn);
	m_conn.close(&m_peer);
}

int32_t YangPeerConnection8::addAudioTrack(YangAudioCodec codec){
	return m_conn.addAudioTrack(&m_peer,codec);
}

int32_t YangPeerConnection8::addVideoTrack(YangVideoCodec codec){
	return m_conn.addVideoTrack(&m_peer,codec);
}

int32_t YangPeerConnection8::addTransceiver(YangMediaTrack media,YangRtcDirection direction){
	return m_conn.addTransceiver(&m_peer,media,direction);
}
int32_t YangPeerConnection8::createDataChannel(){
	return m_conn.createDataChannel(&m_peer);
}

int32_t YangPeerConnection8::generateCertificate(){
	return m_conn.generateCertificate(&m_peer);
}
int32_t YangPeerConnection8::createOffer( char **psdp){
	return m_conn.createOffer(&m_peer,psdp);
}
int32_t YangPeerConnection8::createAnswer(char* answer){
	return m_conn.createAnswer(&m_peer,answer);
}

int32_t YangPeerConnection8::setRemoteDescription(char* sdp){
	return m_conn.setRemoteDescription(&m_peer,sdp);
}
int32_t YangPeerConnection8::setLocalDescription(char* sdp){
	return m_conn.setLocalDescription(&m_peer,sdp);
}


int32_t YangPeerConnection8::close(){
	return m_conn.close(&m_peer);
}

yangbool YangPeerConnection8::isAlive(){
	return m_conn.isAlive(&m_peer);
}

yangbool YangPeerConnection8::isConnected(){
	 return m_conn.isConnected(&m_peer);
}

int32_t YangPeerConnection8::on_audio(YangPushData* pushData){
	return m_conn.on_audio(&m_peer,pushData);
}

int32_t YangPeerConnection8::on_video(YangPushData* pushData){
	return m_conn.on_video(&m_peer,pushData);
}

int32_t YangPeerConnection8::on_message(YangFrame* msgFrame){
	return m_conn.on_message(&m_peer,msgFrame);
}

int32_t YangPeerConnection8::addIceCandidate(char* candidateStr){
	return m_conn.addIceCandidate(&m_peer,candidateStr);
}

int32_t YangPeerConnection8::sendRequestPli(){
	 return m_conn.sendRequestPli(&m_peer);
}


YangRtcPacer::YangRtcPacer(){
	yang_memset(&m_pacer,0,sizeof(YangPacer));
	yang_create_pacer(&m_pacer);
}

YangRtcPacer::~YangRtcPacer(){
	yang_destroy_pacer(&m_pacer);
}

int32_t YangRtcPacer::reset(){
	return m_pacer.reset(m_pacer.session);
}

int32_t YangRtcPacer::initAudio(YangAudioCodec acodec,int32_t sample,int32_t channel){
	return m_pacer.initAudio(m_pacer.session,acodec,sample,channel);
}

int32_t YangRtcPacer::initVideo(YangVideoCodec vcodec,int32_t pktCount){
	return m_pacer.initVideo(m_pacer.session,vcodec,pktCount);
}

int32_t YangRtcPacer::resetAudioTime(){
	return m_pacer.resetAudioTime(m_pacer.session);
}

int32_t YangRtcPacer::resetVideoTime(){
	return m_pacer.resetVideoTime(m_pacer.session);
}

YangPushData* YangRtcPacer::getAudioData(YangFrame* audioFrame){
	return m_pacer.getAudioData(m_pacer.session,audioFrame);
}

YangPushData* YangRtcPacer::getVideoData(YangFrame* videoFrame){
	return m_pacer.getVideoData(m_pacer.session,videoFrame);
}

