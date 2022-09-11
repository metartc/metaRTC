//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPeerConnection.h>
#include <yangrtc/YangRtcConnection.h>
#include <yangrtc/YangStreamUrl.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>

#include <yangavutil/video/YangMeta.h>

#include <yangzlm/YangZlmConnection.h>
#include <yangsrs/YangSrsConnection.h>
#include <yangp2p/YangP2pConnection.h>

#include <stdio.h>
#include <string.h>


void g_yang_pc_init(YangPeer* peer){
	if(peer==NULL || peer->conn!=NULL) return;
	if (peer->conn == NULL)		peer->conn = calloc(1, sizeof(YangRtcConnection));
	YangRtcConnection *conn = (YangRtcConnection*)peer->conn;
	yang_create_rtcConnection(conn, &peer->streamconfig,peer->avinfo);
}
int32_t g_yang_pc_isConnected(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return Yang_Ok;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return conn->isConnected(conn->session);
}

int32_t g_yang_pc_initParam(char* url,YangStreamConfig* stream,YangAVInfo* avinfo,YangStreamOptType opt){
	return yang_stream_parseUrl(url, stream, avinfo, opt);
}

int32_t yang_p2p_getHttpSdp(char* httpIp,int32_t httpPort,char* localSdp,char* remoteSdp){
	return yang_p2p_connectRtcServer(httpIp,httpPort,localSdp,remoteSdp);
}

int32_t g_yang_pc_connectServer(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	int32_t mediaServer=peer->avinfo->sys.mediaServer;
	YangRtcConnection *conn = (YangRtcConnection*)peer->conn;
	if(conn->isConnected(conn->session)) return Yang_Ok;

	if(mediaServer==Yang_Server_Zlm) return yang_zlm_connectRtcServer(conn);
	return yang_srs_connectRtcServer(conn);
}

int32_t g_yang_pc_disconnectServer(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	yang_trace("\nwebrtc disconnect\n");
	if (conn->isConnected(conn->session))		conn->close(conn->session);
	yang_destroy_rtcConnection(conn);
	yang_free(peer->conn);
	return Yang_Ok;
}

int32_t g_yang_pc_stopRtc(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	yang_trace("\nwebrtc stop\n");
	if (conn->session->context.state)		conn->close(conn->session);
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
	peerconn->sendRtcMessage=g_yang_pc_sendRtcMessage;

	g_yang_pc_init(&peerconn->peer);
}

void yang_destroy_peerConnection(YangPeerConnection* peerconn){
	if(peerconn==NULL||peerconn->peer.conn==NULL) return;
	YangRtcConnection *conn = (YangRtcConnection*) peerconn->peer.conn;
	if(conn->session->context.state) g_yang_pc_disconnectServer(&peerconn->peer);
	if(peerconn->peer.conn){
		yang_destroy_rtcConnection(conn);
		yang_free(peerconn->peer.conn);
	}
}

