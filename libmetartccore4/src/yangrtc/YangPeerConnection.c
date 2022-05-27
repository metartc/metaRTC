//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPeerConnection.h>
#include <stdio.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>

#include <yangavutil/video/YangMeta.h>
#include <yangzlm/YangZlmConnection.h>
#include <string.h>

#include <yangrtc/YangRtcSession.h>
#include <yangrtc/YangStreamHandle.h>
#include <yangsrs/YangSrsConnection.h>
#include <yangp2p/YangP2pConnection.h>

#include <yangsdp/YangAnswerSdp.h>
void g_yang_pc_init(YangPeer* peer){
	if(peer==NULL) return;
	if (peer->session == NULL)
		peer->session = calloc(1, sizeof(YangRtcSession));
	YangRtcSession *session = (YangRtcSession*)peer->session;
	yang_create_rtcsession(session, peer->avcontext, peer->user);
	if (session->context.streamConf == NULL)	session->context.streamConf = peer->streamconfig;

}


int32_t g_yang_pc_initParam(char* url,YangStreamConfig* stream,YangAVContext* context,YangStreamOptType opt){
	return yang_stream_parseUrl(url, stream, context, opt);
}

int32_t g_yang_pc_connectServer(YangPeer* peer){
	if(peer==NULL||peer->session==NULL) return  ERROR_RTC_PEERCONNECTION;
	if(peer->mediaServer==Yang_Server_Zlm) return yang_zlm_connectRtcServer((YangRtcSession*)peer->session);
	if(peer->mediaServer==Yang_Server_P2p) return yang_p2p_connectRtcServer((YangRtcSession*)peer->session);
	return yang_srs_connectRtcServer((YangRtcSession*)peer->session);
}

int32_t g_yang_pc_disconnectServer(YangPeer* peer){
	if(peer==NULL||peer->session==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcSession *session = (YangRtcSession*) peer->session;
	yang_trace("\nwebrtc disconnect\n");
	if (session->context.state)		yang_rtcsession_disconnect(session);
	session->context.state = 0;
	yang_destroy_rtcsession(session);
	yang_free(peer->session);
	return Yang_Ok;
}

int32_t g_yang_pc_startRtc(YangPeer* peer,char* sdp){
	if(peer==NULL||peer->session==NULL) return  ERROR_RTC_PEERCONNECTION;
	return yang_p2p_startRtc((YangRtcSession*)peer->session,sdp);
}

int32_t g_yang_pc_stopRtc(YangPeer* peer){
	if(peer==NULL||peer->session==NULL) return  ERROR_RTC_PEERCONNECTION;
	YangRtcSession *session = (YangRtcSession*) peer->session;
	yang_trace("\nwebrtc stop\n");
	if (session->context.state)		yang_rtcsession_disconnect(session);
	session->context.state = 0;
	yang_destroy_rtcsession(session);
	yang_free(peer->session);
	return Yang_Ok;
}

int32_t g_yang_pc_publishVideo(YangPeer* peer,YangFrame* videoFrame){
	if(peer==NULL||peer->session==NULL||videoFrame==NULL) return  ERROR_RTC_PEERCONNECTION;
	return  yang_rtcsession_publishVideo(((YangRtcSession*)peer->session),videoFrame);
}

int32_t g_yang_pc_publishAudio(YangPeer* peer,YangFrame *audioFrame){
	if(peer==NULL||peer->session==NULL||audioFrame==NULL) return  ERROR_RTC_PEERCONNECTION;
	return  yang_rtcsession_publishAudio(((YangRtcSession*)peer->session),audioFrame);
}

int32_t g_yang_pc_publishMsg(YangPeer* peer,YangFrame *msgFrame){
	if(peer==NULL||peer->session==NULL||msgFrame==NULL) return  ERROR_RTC_PEERCONNECTION;
	return  yang_rtcsession_publishMsg(((YangRtcSession*)peer->session),msgFrame);
}

int32_t g_yang_pc_recvvideo_notify(YangPeer* peer, YangRtcMessageType mess){
	if(peer==NULL||peer->session==NULL) return  ERROR_RTC_PEERCONNECTION;
	return yang_recvvideo_notify(((YangRtcSession*)peer->session),mess);
}

int32_t g_yang_pc_getState(YangPeer* peer){
	if(peer==NULL||peer->session==NULL) return Yang_Ok;
	return ((YangRtcSession*)peer->session)->context.state;
}

int32_t g_yang_pc_isAlive(YangPeer* peer){
	if(peer==NULL||peer->session==NULL) return Yang_Ok;
	return yang_rtcsession_isAlive((YangRtcSession*)peer->session);
}

void g_yang_pc_parseHeader(YangVideoCodec codec,uint8_t *buf, uint8_t *src, int32_t *hLen){
	if(codec==Yang_VED_264) yang_getH264RtmpHeader(buf,src,hLen);
	if(codec==Yang_VED_265) yang_getH265RtmpHeader(buf,src,hLen);
}

int32_t g_yang_pc_getAnswerSdp(YangPeer* peer,char* answer){
	if(peer==NULL||peer->session==NULL) return ERROR_RTC_PEERCONNECTION;
	return yang_sdp_getAnswerSdp(((YangRtcSession*)peer->session),answer);

}

void yang_create_peerConnection(YangPeerConnection* peerconn){
	if(peerconn==NULL) return;
	peerconn->peer.session=NULL;
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
	if(peerconn==NULL||peerconn->peer.session==NULL) return;

	 if(((YangRtcSession*)peerconn->peer.session)->context.state) g_yang_pc_disconnectServer(&peerconn->peer);
	 if(peerconn->peer.session){
		yang_destroy_rtcsession( (YangRtcSession*)peerconn->peer.session);
		yang_free(peerconn->peer.session);
	 }
}
