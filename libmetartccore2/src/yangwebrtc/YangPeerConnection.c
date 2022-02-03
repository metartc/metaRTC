/*
 * YangPeerConnection.c
 *
 *  Created on: 2022年1月6日
 *      Author: yang
 */

#include <stdio.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <yangavutil/video/YangMeta.h>
#include <yangcsrs/YangSrsConnection.h>
#include <yangwebrtc/YangPeerConnection.h>
#include <yangwebrtc/YangRtcSession.h>
#include <yangwebrtc/YangRtcSession.h>
#include <yangwebrtc/YangStreamHandle.h>
#include <string.h>

void g_yang_pc_init(YangPeer* peer){
	if(peer==NULL) return;
	if (peer->session == NULL)
		peer->session = calloc(1, sizeof(YangRtcSession));
	YangRtcSession *session = (YangRtcSession*)peer->session;
	yang_init_rtcsession_context(session, peer->avcontext, peer->user);
	if (session->context.streamConf == NULL)	session->context.streamConf = peer->streamconfig;

	memset(session->context.streamConf->localIp, 0,	sizeof(session->context.streamConf->localIp));
	yang_getLocalInfo(session->context.streamConf->localIp);
}
int32_t g_yang_pc_connectServer(YangPeer* peer){
	if(peer==NULL||peer->session==NULL) return 1;
	return yang_srs_connectRtcServer((YangRtcSession*)peer->session);
}

int32_t g_yang_pc_initParam(char* url,YangStreamConfig* stream,YangAVContext* context,YangStreamOptType opt){
	return yang_stream_parseUrl(url, stream, context, opt);
}

int32_t g_yang_pc_disconnectServer(YangPeer* peer){
	if(peer==NULL||peer->session==NULL) return 1;
	YangRtcSession *session = (YangRtcSession*) peer->session;
	yang_trace("\nwebrtc disconnect\n");
	if (session->context.state)		yang_rtcsession_disconnect(session);
	session->context.state = 0;
	yang_destroy_rtcsession(session);
	yang_free(peer->session);
	return Yang_Ok;
}

int32_t g_yang_pc_publishVideo(YangPeer* peer,YangFrame* videoFrame){
	if(peer==NULL||peer->session==NULL||videoFrame==NULL) return 1;
	return  yang_rtcsession_publishVideo(((YangRtcSession*)peer->session),videoFrame);
}
int32_t g_yang_pc_publishAudio(YangPeer* peer,YangFrame *audioFrame){
	if(peer==NULL||peer->session==NULL||audioFrame==NULL) return 1;
	return  yang_rtcsession_publishAudio(((YangRtcSession*)peer->session),audioFrame);
}
int32_t g_yang_pc_recvvideo_notify(YangPeer* peer, YangRtcMessageType mess){
	if(peer==NULL||peer->session==NULL) return 1;
	return yang_recvvideo_notify(((YangRtcSession*)peer->session),mess);
}

int32_t g_yang_pc_getState(YangPeer* peer){
	if(peer==NULL||peer->session==NULL) return 0;
	return ((YangRtcSession*)peer->session)->context.state;
}
void g_yang_pc_parseHeader(YangVideoCodec codec,uint8_t *buf, uint8_t *src, int32_t *hLen){
	if(codec==Yang_VED_264) yang_getH264RtmpHeader(buf,src,hLen);
	if(codec==Yang_VED_265) yang_getH265RtmpHeader(buf,src,hLen);
}



void yang_init_peerConnection(YangPeerConnection* peerconn){
	if(peerconn==NULL) return;
	peerconn->peer.session=NULL;
	peerconn->init=g_yang_pc_init;
	peerconn->initParam=g_yang_pc_initParam;
	peerconn->parseHeader=g_yang_pc_parseHeader;
	peerconn->connectServer=g_yang_pc_connectServer;
	peerconn->disconnectServer=g_yang_pc_disconnectServer;
	peerconn->publishAudio=g_yang_pc_publishAudio;
	peerconn->publishVideo=g_yang_pc_publishVideo;
	peerconn->getState=g_yang_pc_getState;
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
