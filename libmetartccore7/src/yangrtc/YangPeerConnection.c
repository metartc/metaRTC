//
// Copyright (c) 2019-2025 yanggaofeng
//

#include <yangrtc/YangRtcConnection.h>

#include <yangrtc/YangPeerConnection.h>
#include <yangrtc/YangMetaConnection.h>
#include <yangutil/sys/YangLog.h>

#include <yangavutil/video/YangMeta.h>



static yangbool yang_pc_isConnected(YangPeer* peer){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return yangfalse;

	conn = (YangRtcConnection*) peer->conn;
	return conn->isConnected(conn->session);
}


static int32_t yang_pc_close(YangPeer* peer){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return  ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	if(conn->session->context.state==Yang_Conn_State_Disconnected || conn->session->context.state==Yang_Conn_State_Closed)
		return Yang_Ok;
	conn->session->context.state = Yang_Conn_State_Disconnected;
	if(conn->onConnectionStateChange) conn->onConnectionStateChange(conn->session,Yang_Conn_State_Disconnected);
	yang_trace("\nwebrtc disconnected\n");
	conn->close(conn->session);
	yang_destroy_rtcConnection(conn);
	yang_free(peer->conn);

	return Yang_Ok;
}

static int32_t yang_pc_setLocalDescription(YangPeer* peer,char* sdp){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return  ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return conn->setLocalDescription(conn->session,sdp);
}

static int32_t yang_pc_setRemoteDescription(YangPeer* peer,char* sdp){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return  ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return conn->setRemoteDescription(conn->session,sdp);
}

static int32_t yang_addIceCandidate(YangPeer* peer,char* candidateStr){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL||candidateStr==NULL)
		return ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return conn->addIceCandidate(conn->session,candidateStr);
}


static int32_t yang_pc_onVideo(YangPeer* peer,YangFrame* videoFrame){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL||videoFrame==NULL)
		return  ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return  conn->on_video(conn->session,videoFrame);
}

static int32_t yang_pc_onAudio(YangPeer* peer,YangFrame *audioFrame){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL||audioFrame==NULL)
		return  ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return  conn->on_audio(conn->session,audioFrame);
}

static int32_t yang_pc_onMessage(YangPeer* peer,YangFrame *msgFrame){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL||msgFrame==NULL)
		return  ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return  conn->on_message(conn->session,msgFrame);
}

static int32_t yang_pc_sendRtcMessage(YangPeer* peer, YangRtcMessageType mess){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return  ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return conn->notify(conn->session,mess);
}


static yangbool yang_pc_isAlive(YangPeer* peer){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return yangfalse;

	conn = (YangRtcConnection*) peer->conn;
	return conn->isAlive(conn->session);
}

static YangRtcConnectionState yang_pc_getConnectionState(YangPeer* peer){
	YangRtcConnection *conn;

	if(peer==NULL || peer->conn==NULL)
		return Yang_Conn_State_New;

	conn = (YangRtcConnection*) peer->conn;
	return conn->session->context.state;
}

//static void yang_pc_parseHeader(YangVideoCodec codec,uint8_t *buf, uint8_t *src, int32_t *hLen){
//	if(codec==Yang_VED_H264) yang_getH264RtmpHeader(buf,src,hLen);
//	if(codec==Yang_VED_H265) yang_getH265RtmpHeader(buf,src,hLen);
//}

static int32_t yang_pc_getAnswerSdp(YangPeer* peer,char* answer){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;

	return conn->createAnswer(conn->session,answer);
}


static int32_t yang_pc_addAudioTrack(YangPeer* peer,YangAudioCodec codec){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return conn->addAudioTrack(conn->session,codec);

}

static int32_t yang_pc_addVideoTrack(YangPeer* peer,YangVideoCodec codec){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return conn->addVideoTrack(conn->session,codec);
}

static int32_t yang_pc_addTransceiver(YangPeer* peer,YangMediaTrack media,YangRtcDirection direction){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return conn->addTransceiver(conn->session,media,direction);
}


static int32_t yang_pc_createOffer(YangPeer* peer,char **psdp){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return conn->createOffer(conn->session,psdp);

}


static YangIceCandidateType yang_pc_getIceCandidateType(YangPeer* peer){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return YangIceHost;

	conn = (YangRtcConnection*) peer->conn;
	return conn->session->ice.session.candidateType;
}


static int32_t yang_createDataChannel(YangPeer* peer){
	YangRtcConnection *conn;

	if(peer==NULL||peer->conn==NULL)
		return ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return conn->createDataChannel(conn->session);
}



static int32_t yang_sendRequestPli(YangPeer* peer){
	YangRtcConnection *conn = NULL;

	if(peer==NULL||peer->conn==NULL)
		return  ERROR_RTC_PEERCONNECTION;

	conn = (YangRtcConnection*) peer->conn;
	return conn->sendRequestPli(conn->session);
}

void yang_create_metaConnection(YangMetaConnection* peerconn){

	if(peerconn==NULL)
		return;

	peerconn->addAudioTrack=yang_pc_addAudioTrack;
	peerconn->addVideoTrack=yang_pc_addVideoTrack;
	peerconn->addTransceiver=yang_pc_addTransceiver;

	peerconn->getIceCandidateType=yang_pc_getIceCandidateType;

	peerconn->createOffer=yang_pc_createOffer;
	peerconn->createAnswer=yang_pc_getAnswerSdp;

	peerconn->setLocalDescription=yang_pc_setLocalDescription;
	peerconn->setRemoteDescription=yang_pc_setRemoteDescription;

	peerconn->close=yang_pc_close;

	peerconn->on_audio=yang_pc_onAudio;
	peerconn->on_video=yang_pc_onVideo;
	peerconn->on_message=yang_pc_onMessage;
	peerconn->isConnected=yang_pc_isConnected;
	peerconn->isAlive=yang_pc_isAlive;
	peerconn->getConnectionState=yang_pc_getConnectionState;
	peerconn->addIceCandidate=yang_addIceCandidate;
	peerconn->sendRtcMessage=yang_pc_sendRtcMessage;
	peerconn->createDataChannel=yang_createDataChannel;



	peerconn->sendRequestPli=yang_sendRequestPli;

}

void yang_create_peer(YangPeer* peer){
	YangRtcConnection *conn ;

	if(peer==NULL)
		return;

	if (peer->conn == NULL){
		peer->conn = yang_calloc(1, sizeof(YangRtcConnection));
		conn = (YangRtcConnection*)peer->conn;
		yang_create_rtcConnection(conn,&peer->peerInfo, &peer->peerCallback);
	}

}

void yang_create_peerConnection(YangPeerConnection* peerconn){

	YangMetaConnection conn;

	if(peerconn==NULL)
		return;

	peerconn->peer.conn=NULL;
	yang_create_peer(&peerconn->peer);
	yang_create_metaConnection(&conn);

	peerconn->addAudioTrack=conn.addAudioTrack;
	peerconn->addVideoTrack=conn.addVideoTrack;
	peerconn->addTransceiver=conn.addTransceiver;

	peerconn->getIceCandidateType=conn.getIceCandidateType;

	peerconn->createOffer=conn.createOffer;
	peerconn->createAnswer=conn.createAnswer;

	peerconn->setLocalDescription=conn.setLocalDescription;
	peerconn->setRemoteDescription=conn.setRemoteDescription;

	peerconn->close=conn.close;

	peerconn->on_audio=conn.on_audio;
	peerconn->on_video=conn.on_video;
	peerconn->on_message=conn.on_message;
	peerconn->isConnected=conn.isConnected;
	peerconn->isAlive=conn.isAlive;
	peerconn->getConnectionState=conn.getConnectionState;
	peerconn->sendRtcMessage=conn.sendRtcMessage;
	peerconn->addIceCandidate=conn.addIceCandidate;
	peerconn->createDataChannel=conn.createDataChannel;

	peerconn->sendRequestPli=conn.sendRequestPli;

}

void yang_destroy_peerConnection(YangPeerConnection* peerconn){

	if(peerconn==NULL||peerconn->peer.conn==NULL)
		return;

	yang_pc_close(&peerconn->peer);
}

