//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPeerConnection.h>
#include <yangrtc/YangRtcSession.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>

#include <yangavutil/video/YangMeta.h>

#include <yangzlm/YangZlmConnection.h>
#include <yangsrs/YangSrsConnection.h>
#include <yangp2p/YangP2pConnection.h>

#include <yangsdp/YangAnswerSdp.h>

#include <stdio.h>
#include <string.h>
#include <yangrtc/YangStreamUrl.h>

void g_yang_pc_init(YangPeer* peer){
	if(peer==NULL) return;
	if (peer->conn == NULL)
		peer->conn = calloc(1, sizeof(YangRtcConnection));
	YangRtcConnection *conn = (YangRtcConnection*)peer->conn;
	yang_create_rtcConnection(conn, peer->avinfo);
	if (conn->session->context.streamConfig == NULL)	conn->session->context.streamConfig = peer->streamconfig;

}


int32_t g_yang_pc_initParam(char* url,YangStreamConfig* stream,YangAVInfo* avinfo,YangStreamOptType opt){
	return yang_stream_parseUrl(url, stream, avinfo, opt);
}

int32_t g_yang_pc_connectServer(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*)peer->conn;
	if(peer->mediaServer==Yang_Server_Zlm) return yang_zlm_connectRtcServer(conn);
	if(peer->mediaServer==Yang_Server_P2p) return yang_p2p_connectRtcServer(conn);
	return yang_srs_connectRtcServer(conn);
}

int32_t g_yang_pc_disconnectServer(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	yang_trace("\nwebrtc disconnect\n");
	if (conn->session->context.state)		conn->disconnect(conn->session);
	conn->session->context.state = 0;
	yang_destroy_rtcConnection(conn);
	yang_free(peer->conn);
	return Yang_Ok;
}

int32_t g_yang_pc_startRtc(YangPeer* peer,char* sdp){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return yang_p2p_startRtc(conn,sdp);
}

int32_t g_yang_pc_stopRtc(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	yang_trace("\nwebrtc stop\n");
	if (conn->session->context.state)		conn->disconnect(conn->session);
	conn->session->context.state = 0;
	yang_destroy_rtcConnection(conn);
	yang_free(peer->conn);
	return Yang_Ok;
}

int32_t g_yang_pc_publishVideo(YangPeer* peer,YangFrame* videoFrame){
	if(peer==NULL||peer->conn==NULL||videoFrame==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *session = (YangRtcConnection*) peer->conn;
	return  session->publishVideo(session->session,videoFrame);
}

int32_t g_yang_pc_publishAudio(YangPeer* peer,YangFrame *audioFrame){
	if(peer==NULL||peer->conn==NULL||audioFrame==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return  conn->publishAudio(conn->session,audioFrame);
}

int32_t g_yang_pc_publishMsg(YangPeer* peer,YangFrame *msgFrame){
	if(peer==NULL||peer->conn==NULL||msgFrame==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return  conn->publishMsg(conn->session,msgFrame);
}

int32_t g_yang_pc_recvvideo_notify(YangPeer* peer, YangRtcMessageType mess){
	if(peer==NULL||peer->conn==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return conn->notify(conn->session,mess);
}

int32_t g_yang_pc_getState(YangPeer* peer){
	if(peer==NULL||peer->conn==NULL) return Yang_Ok;
	YangRtcConnection *conn = (YangRtcConnection*) peer->conn;
	return conn->session->context.state;
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
	return yang_sdp_getAnswerSdp(conn->session,answer);

}

void yang_create_peerConnection(YangPeerConnection* peerconn){
	if(peerconn==NULL) return;
	peerconn->peer.conn=NULL;
	peerconn->init=g_yang_pc_init;
	peerconn->initParam=g_yang_pc_initParam;
	peerconn->getAnswerSdp=g_yang_pc_getAnswerSdp;
	peerconn->parseHeader=g_yang_pc_parseHeader;
	peerconn->startRtc=g_yang_pc_startRtc;
	peerconn->stopRtc=g_yang_pc_stopRtc;
	peerconn->connectServer=g_yang_pc_connectServer;
	peerconn->disconnectServer=g_yang_pc_disconnectServer;
	peerconn->publishAudio=g_yang_pc_publishAudio;
	peerconn->publishVideo=g_yang_pc_publishVideo;
	peerconn->publishMsg=g_yang_pc_publishMsg;
	peerconn->getState=g_yang_pc_getState;
	peerconn->isAlive=g_yang_pc_isAlive;
	peerconn->recvvideo_notify=g_yang_pc_recvvideo_notify;
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
