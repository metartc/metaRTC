/*
 * YangMetaConnection.c
 *
 *  Created on: 2022年1月16日
 *      Author: yang
 */

#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <yangavutil/video/YangCMeta.h>
#include <yangcsrs/YangSrsConnection.h>

#include <yangwebrtc/YangRtcSession.h>
#include <yangwebrtc/YangCRtcSession.h>
#include <yangwebrtc/YangStreamHandle.h>
#include <stdio.h>
#include <string.h>
#include <yangwebrtc/YangMetaConnection.h>
typedef struct{
	YangRtcSession* session;
	YangAVContext* avcontext;
	YangStreamConfig* streamconfig;

}YangMetaSession;


void g_yang_mt_init(void* pcontext,YangMetaRtcCallback* callback,void* user){
	if(pcontext==NULL) return;
	YangMetaSession* context=(YangMetaSession*)pcontext;
	if (context->session == NULL)
		context->session = (YangRtcSession*)calloc(1, sizeof(YangRtcSession));
	context->avcontext=(YangAVContext*)calloc(sizeof(YangAVContext),1);
	context->avcontext->avinfo=(YangAVInfo*)calloc(sizeof(YangAVInfo),1);
	context->streamconfig=(YangStreamConfig*)calloc(sizeof(YangStreamConfig),1);
	context->avcontext->sendRequest=callback->sendRequest;
	context->avcontext->setPlayMediaConfig=callback->setPlayMediaConfig;
	context->avcontext->recvcb.receiveAudio=callback->receiveAudio;
	context->avcontext->recvcb.receiveVideo=callback->receiveVideo;

	yang_init_rtcsession_context(context->session, context->avcontext,user);

	if (context->session->context.streamConf == NULL)	context->session->context.streamConf = context->streamconfig;

	memset(context->session->context.streamConf->localIp, 0,	sizeof(context->session->context.streamConf->localIp));
	yang_getLocalInfo(context->session->context.streamConf->localIp);
}
int32_t g_yang_mt_connectServer(void* peer){
	if(peer==NULL) return 1;
	YangRtcSession* session=((YangMetaSession*)peer)->session;
	if(session==NULL) return 1;
	return yang_srs_connectRtcServer(session);
}

int32_t g_yang_mt_initParam(void* pcontext,char* url,YangStreamOptType opt){
	if(pcontext==NULL) return 1;
	YangMetaSession* context=(YangMetaSession*)pcontext;
	return yang_stream_parseUrl(url, context->streamconfig, context->avcontext, opt);
}

int32_t g_yang_mt_disconnectServer(void* peer){
	if(peer==NULL) return 1;
	YangRtcSession *session = ((YangMetaSession*) peer)->session;
	if(session==NULL) return 1;
	yang_trace("\nwebrtc disconnect\n");
	if (session->context.state)		yang_rtcsession_disconnect(session);
	session->context.state = 0;
	yang_destroy_rtcsession(session);
	yang_free(((YangMetaSession*) peer)->session);
	return Yang_Ok;
}

int32_t g_yang_mt_publishVideo(void* peer,YangFrame* videoFrame){
	if(peer==NULL||videoFrame==NULL) return 1;
	return  yang_rtcsession_publishVideo(((YangMetaSession*) peer)->session,videoFrame);
}
int32_t g_yang_mt_publishAudio(void* peer,YangFrame *audioFrame){
	if(peer==NULL||audioFrame==NULL) return 1;
	return  yang_rtcsession_publishAudio(((YangMetaSession*) peer)->session,audioFrame);
}
int32_t g_yang_mt_recvvideo_notify(void* peer, YangRtcMessageType mess){
	if(peer==NULL) return 1;
	return yang_recvvideo_notify(((YangMetaSession*) peer)->session,mess);
}

int32_t g_yang_mt_getState(void* peer){
	if(peer==NULL) return 0;
	return ((YangMetaSession*) peer)->session->context.state;
}
void g_yang_mt_parseHeader(YangVideoCodec codec,uint8_t *buf, uint8_t *src, int32_t *hLen){
	if(codec==Yang_VED_264) yang_getH264RtmpHeader(buf,src,hLen);
	if(codec==Yang_VED_265) yang_getH265RtmpHeader(buf,src,hLen);
}



void yang_init_metaConnection(YangMetaConnection* metaconn){

	metaconn->context=calloc(sizeof(YangMetaSession),1);
	yang_setLogLevle(5);
	    yang_setLogFile(1);

	metaconn->init=g_yang_mt_init;
	metaconn->initParam=g_yang_mt_initParam;
	metaconn->parseHeader=g_yang_mt_parseHeader;
	metaconn->connectServer=g_yang_mt_connectServer;
	metaconn->disconnectServer=g_yang_mt_disconnectServer;
	metaconn->publishAudio=g_yang_mt_publishAudio;
	metaconn->publishVideo=g_yang_mt_publishVideo;
	metaconn->getState=g_yang_mt_getState;
	metaconn->recvvideo_notify=g_yang_mt_recvvideo_notify;
}
void yang_destroy_metaConnection(YangMetaConnection* metaconn){
	if(metaconn==NULL) return;
	 if(((YangRtcSession*)metaconn->context)->context.state) {
		 g_yang_mt_disconnectServer(metaconn->context);
	 }else{
		 if(((YangMetaSession*)metaconn->context)->session){
			yang_destroy_rtcsession( ((YangMetaSession*)metaconn->context)->session);
			yang_free(((YangMetaSession*)metaconn->context)->session);
		 }
	 }
	 yang_free(((YangMetaSession*)metaconn->context)->streamconfig);
	 yang_free(((YangMetaSession*)metaconn->context)->avcontext->avinfo);
	 yang_free(((YangMetaSession*)metaconn->context)->avcontext);
}
