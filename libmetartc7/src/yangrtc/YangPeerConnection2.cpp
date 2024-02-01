//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPeerConnection2.h>

YangPeerConnection2::YangPeerConnection2(YangAVInfo* avinfo,YangStreamConfig* pstreamConfig) {
	memset(&m_conn,0,sizeof(YangPeerConnection));
	m_conn.peer.avinfo=avinfo;
	memcpy(&m_conn.peer.streamconfig,pstreamConfig,sizeof(YangStreamConfig));
	streamConfig=&m_conn.peer.streamconfig;
	yang_create_peerConnection(&m_conn);
}

YangPeerConnection2::~YangPeerConnection2() {
	yang_destroy_peerConnection(&m_conn);
}

void YangPeerConnection2::init(){
	m_conn.init(&m_conn.peer);
}

int32_t YangPeerConnection2::addAudioTrack(YangAudioCodec codec){
	return m_conn.addAudioTrack(&m_conn.peer,codec);
}
int32_t YangPeerConnection2::addVideoTrack(YangVideoCodec codec){
	return m_conn.addVideoTrack(&m_conn.peer,codec);
}

int32_t YangPeerConnection2::addTransceiver(YangRtcDirection direction){
	return m_conn.addTransceiver(&m_conn.peer,direction);
}
int32_t YangPeerConnection2::createDataChannel(){
	return m_conn.createDataChannel(&m_conn.peer);
}
int32_t YangPeerConnection2::createOffer( char **psdp){
	return m_conn.createOffer(&m_conn.peer,psdp);
}
int32_t YangPeerConnection2::createAnswer(char* answer){
	return m_conn.createAnswer(&m_conn.peer,answer);
}
int32_t YangPeerConnection2::createHttpAnswer(char* answer){
	return m_conn.createHttpAnswer(&m_conn.peer,answer);
}

int32_t YangPeerConnection2::setRemoteDescription(char* sdp){
	return m_conn.setRemoteDescription(&m_conn.peer,sdp);
}
int32_t YangPeerConnection2::connectSfuServer(){
	return m_conn.connectSfuServer(&m_conn.peer);
}

int32_t YangPeerConnection2::connectWhipServer(char* url){
	return m_conn.connectWhipWhepServer(&m_conn.peer,url);
}

int32_t YangPeerConnection2::close(){
	return m_conn.close(&m_conn.peer);
}

int32_t YangPeerConnection2::isAlive(){
	return m_conn.isAlive(&m_conn.peer);
}
int32_t YangPeerConnection2::isConnected(){
	 return m_conn.isConnected(&m_conn.peer);
}

int32_t YangPeerConnection2::on_audio(YangFrame* audioFrame){
	return m_conn.on_audio(&m_conn.peer,audioFrame);
}
int32_t YangPeerConnection2::on_video(YangFrame* videoFrame){
	return m_conn.on_video(&m_conn.peer,videoFrame);
}
int32_t YangPeerConnection2::on_message(YangFrame* msgFrame){
	return m_conn.on_message(&m_conn.peer,msgFrame);
}

int32_t YangPeerConnection2::sendRtcMessage( YangRtcMessageType mess){
	 return m_conn.sendRtcMessage(&m_conn.peer,mess);
}
