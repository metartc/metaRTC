//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPeerConnection.h>
#include <yangrtc/YangRtcConnection.h>
#include <yangrtc/YangStreamUrl.h>

#include <yangutil/sys/YangLog.h>

#include <yangavutil/video/YangMeta.h>

#include <yangzlm/YangZlmConnection.h>
#include <yangsrs/YangSrsConnection.h>
#include <yangp2p/YangP2pConnection.h>
#include <yangwhip/YangWhip.h>

void g_yang_pc_init(YangPeer* peer){
	if(peer==NULL || peer->conn!=NULL) return;
	if (peer->conn == NULL)		peer->conn = yang_calloc(1, sizeof(YangRtcConnection));
	YangRtcConnection *conn = (YangRtcConnection*)peer->conn;
	yang_create_rtcConnection(conn, &peer->streamconfig,peer->avinfo);
}
int32_t g_yang_pc_isConnected(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return Yang_Ok;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return conn->isConnected(conn->session);
}

int32_t g_yang_pc_initParam(char* url,YangStreamConfig* stream,YangAVInfo* avinfo,YangStreamDirection opt){
	return yang_stream_parseUrl(url, stream, avinfo, opt);
}

int32_t yang_p2p_getHttpSdp(YangIpFamilyType family,char* httpIp,int32_t httpPort,char* localSdp,char* remoteSdp){
	return yang_p2p_connectRtcServer(family,httpIp,httpPort,localSdp,remoteSdp);
}

int32_t g_yang_pc_connectServer(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	int32_t mediaServer=peer->avinfo->sys.mediaServer;
	YangRtcConnection *conn = (YangRtcConnection*)peer->conn;
	if(conn->isConnected(conn->session)) return Yang_Ok;

	if(mediaServer==Yang_Server_Zlm) return yang_zlm_connectRtcServer(conn);

	return yang_srs_connectRtcServer(conn);
}


int32_t g_yang_pc_connectWhipWhepServer(YangPeer* peer,char* url){
	if(peer==NULL||url==NULL) return ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return yang_whip_connectPeer(conn,url);
}

int32_t g_yang_pc_stopRtc(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
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

int32_t g_yang_pc_startRtc(YangPeer* peer,char* sdp){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return conn->setRemoteDescription(conn->session,sdp);
}

int32_t g_yang_pc_onVideo(YangPeer* peer,YangFrame* videoFrame){
	if(peer==NULL||peer->conn==NULL||videoFrame==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *session = (YangRtcConnection*) peer->conn;
	return  session->on_video(session->session,videoFrame);
}

int32_t g_yang_pc_onAudio(YangPeer* peer,YangFrame *audioFrame){
	if(peer==NULL||peer->conn==NULL||audioFrame==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return  conn->on_audio(conn->session,audioFrame);
}

int32_t g_yang_pc_onMessage(YangPeer* peer,YangFrame *msgFrame){
	if(peer==NULL||peer->conn==NULL||msgFrame==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return  conn->on_message(conn->session,msgFrame);
}

int32_t g_yang_pc_sendRtcMessage(YangPeer* peer, YangRtcMessageType mess){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return conn->notify(conn->session,mess);
}


int32_t g_yang_pc_isAlive(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return Yang_Ok;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return conn->isAlive(conn->session);
}

YangRtcConnectionState g_yang_pc_getConnectionState(YangPeer* peer){
	if(peer==NULL || peer->conn==NULL) return Yang_Conn_State_New;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return conn->session->context.state;
}

void g_yang_pc_parseHeader(YangVideoCodec codec,uint8_t *buf, uint8_t *src, int32_t *hLen){
	if(codec==Yang_VED_264) yang_getH264RtmpHeader(buf,src,hLen);
	if(codec==Yang_VED_265) yang_getH265RtmpHeader(buf,src,hLen);
}

int32_t g_yang_pc_getAnswerSdp(YangPeer* peer,char* answer){
	if(peer==NULL||peer->conn==NULL) return ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;

	return conn->createAnswer(conn->session,answer);
}
int32_t g_yang_pc_gethttpanswersdp(YangPeer* peer,char* answer){
	if(peer==NULL||peer->conn==NULL) return ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;

	return conn->createHttpAnswer(conn->session,answer);
}
int32_t g_yang_pc_getAnswerSdp2(YangPeer* peer,char* answer){
	if(peer==NULL||peer->conn==NULL) return ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;

	return conn->createHttpAnswer(conn->session,answer);
}

int32_t g_yang_pc_createOffer(YangPeer* peer,char **psdp){
	if(peer==NULL||peer->conn==NULL) return ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return conn->createOffer(conn->session,psdp);

}



YangIceCandidateType g_yang_pc_getIceCandidateType(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return YangIceHost;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return conn->session->ice.session.candidateType;
}

void yang_create_peerConnection(YangPeerConnection* peerconn){
	if(peerconn==NULL) return;
	peerconn->peer.conn=NULL;
	peerconn->init=g_yang_pc_init;
	peerconn->getIceCandidateType=g_yang_pc_getIceCandidateType;

	peerconn->connectSfuServer=g_yang_pc_connectServer;
	peerconn->connectWhipWhepServer=g_yang_pc_connectWhipWhepServer;

	peerconn->createOffer=g_yang_pc_createOffer;
	peerconn->createAnswer=g_yang_pc_getAnswerSdp;
	peerconn->createHttpAnswer=g_yang_pc_gethttpanswersdp;

	peerconn->setRemoteDescription=g_yang_pc_startRtc;
	peerconn->close=g_yang_pc_stopRtc;

	peerconn->on_audio=g_yang_pc_onAudio;
	peerconn->on_video=g_yang_pc_onVideo;
	peerconn->on_message=g_yang_pc_onMessage;
	peerconn->isConnected=g_yang_pc_isConnected;
	peerconn->isAlive=g_yang_pc_isAlive;
	peerconn->getConnectionState=g_yang_pc_getConnectionState;
	peerconn->sendRtcMessage=g_yang_pc_sendRtcMessage;

	g_yang_pc_init(&peerconn->peer);
}

void yang_destroy_peerConnection(YangPeerConnection* peerconn){
	if(peerconn==NULL||peerconn->peer.conn==NULL) return;
	g_yang_pc_stopRtc(&peerconn->peer);

}

