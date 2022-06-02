//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangRtcHandle.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/sys/YangLog.h>

#ifdef _WIN32
#include <winsock2.h>
#define be32toh ntohl
#endif

#include <string.h>


#include <yangrtc/YangPeerConnection.h>

typedef struct{
	int32_t isInit;
	YangAVInfo* avinfo;
	YangStreamConfig *streamconfig;

	YangFrame audioFrame;
	YangFrame videoFrame;
	YangPeerConnection peerconn;
}YangRtcHandleContext;
void g_yang_setPlayMediaConfig(	YangAudioParam *remote_audio,YangVideoParam *remote_video,void* user){
	if(user==NULL) return;
	YangRtcHandle* rtcHandle=(YangRtcHandle*)user;

	if(rtcHandle&&rtcHandle->streamconfig.rtcCallback.setMediaConfig){
		rtcHandle->streamconfig.rtcCallback.setMediaConfig(rtcHandle->streamconfig.rtcCallback.context,rtcHandle->streamconfig.uid,	remote_audio,remote_video);
	}
}

void g_yang_sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req,void* user){
	if(user==NULL) return;
	YangRtcHandle* rtcHandle=(YangRtcHandle*)user;
	if(rtcHandle&&rtcHandle->streamconfig.rtcCallback.sendRequest){
		rtcHandle->streamconfig.rtcCallback.sendRequest(rtcHandle->streamconfig.rtcCallback.context,puid, ssrc,req);
	}
}

int32_t yang_rtcstream_sendRtcMessage(void* pcontext,YangRtcMessageType mess){
	if(pcontext==NULL) return 0;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	if(context->peerconn.recvvideo_notify) context->peerconn.recvvideo_notify(&context->peerconn.peer,mess);
	return Yang_Ok;
}
int32_t yang_rtcstream_handle_getState(void* pcontext){
	if(pcontext==NULL) return 0;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	if(context->peerconn.getState)	return context->peerconn.getState(&context->peerconn.peer);
	return 0;
}
int32_t yang_rtcstream_handle_isAlive(void* pcontext){
	if(pcontext==NULL) return 0;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	if(context->peerconn.isAlive)	return context->peerconn.isAlive(&context->peerconn.peer);
	return 0;
}
int32_t yang_rtcstream_handle_publishVideo(void* pcontext,YangStreamCapture* p){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	context->videoFrame.payload=p->getVideoData(p->context);
	context->videoFrame.nb=p->getVideoLen(p->context);
	context->videoFrame.pts=p->getVideoTimestamp(p->context);
	context->videoFrame.frametype=p->getVideoFrametype(p->context);

	if(context->peerconn.publishVideo) return  context->peerconn.publishVideo(&context->peerconn.peer,&context->videoFrame);
	return Yang_Ok;
}

int32_t yang_rtcstream_handle_publishMsg(void* pcontext,YangFrame* msgFrame){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	if(context->peerconn.publishMsg) return  context->peerconn.publishMsg(&context->peerconn.peer,msgFrame);
	return Yang_Ok;
}

int32_t yang_rtcstream_handle_publishAudio(void* pcontext,YangStreamCapture* p){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	context->audioFrame.payload=p->getAudioData(p->context);
	context->audioFrame.nb=p->getAudioLen(p->context);
	context->audioFrame.pts=p->getAudioTimestamp(p->context);
	if(context->peerconn.publishAudio) return  context->peerconn.publishAudio(&context->peerconn.peer,&context->audioFrame);
	return Yang_Ok;
}


void yang_rtcstream_handle_init(void* phandle,void* pcontext) {
	if(pcontext==NULL) return;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;


	if(context->isInit==0){
		YangAVContext avcontext;
		memset(&avcontext,0,sizeof(YangAVContext));
		avcontext.avinfo=context->avinfo;
		avcontext.sendRequest=g_yang_sendRequest;
		avcontext.setPlayMediaConfig=g_yang_setPlayMediaConfig;

		context->peerconn.peer.avcontext=&avcontext;
		context->peerconn.peer.streamconfig=context->streamconfig;
		context->peerconn.peer.user=phandle;
		context->peerconn.init(&context->peerconn.peer);


		context->isInit=1;
	}

}

int32_t yang_rtcstream_handle_connectRtcServer(void* pcontext){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	return context->peerconn.connectServer(&context->peerconn.peer);
}
int32_t yang_rtcstream_handle_disconnectServer(void* pcontext){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	if(context->peerconn.disconnectServer) context->peerconn.disconnectServer(&context->peerconn.peer);
	return Yang_Ok;
}
int32_t yang_rtcstream_handle_startRtc(void* pcontext,char* sdp){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	return context->peerconn.startRtc(&context->peerconn.peer,sdp);
}
int32_t yang_rtcstream_handle_stopRtc(void* pcontext){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	if(context->peerconn.disconnectServer) context->peerconn.stopRtc(&context->peerconn.peer);
	return Yang_Ok;
}
int32_t yang_rtcstream_handle_getAnswerSdp(void* pcontext,char* answer){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	if(context->peerconn.getAnswerSdp) return context->peerconn.getAnswerSdp(&context->peerconn.peer,answer);
	return 1;
}
void yang_create_rtcstream_handle(YangRtcHandle* handle,YangAVInfo* avinfo) {
	if(handle==NULL) return;
	handle->context=calloc(sizeof(YangRtcHandleContext),1);
	YangRtcHandleContext* context=(YangRtcHandleContext*)handle->context;

	memset(&context->peerconn,0,sizeof(YangPeerConnection));
	yang_create_peerConnection(&context->peerconn);
	context->peerconn.peer.mediaServer=avinfo->sys.mediaServer;
	context->avinfo=avinfo;

	context->streamconfig=&handle->streamconfig;
	context->isInit=0;

	memset(&context->audioFrame,0,sizeof(YangFrame));
	memset(&context->videoFrame,0,sizeof(YangFrame));

	handle->init=yang_rtcstream_handle_init;
	handle->sendRtcMessage=yang_rtcstream_sendRtcMessage;
	handle->getAnswerSdp=yang_rtcstream_handle_getAnswerSdp;
	handle->startRtc=yang_rtcstream_handle_startRtc;
	handle->stopRtc=yang_rtcstream_handle_stopRtc;
	handle->connectRtcServer=yang_rtcstream_handle_connectRtcServer;
	handle->disconnectServer=yang_rtcstream_handle_disconnectServer;
	handle->getState=yang_rtcstream_handle_getState;
	handle->isAlive=yang_rtcstream_handle_isAlive;
	handle->publishAudio=yang_rtcstream_handle_publishAudio;
	handle->publishVideo=yang_rtcstream_handle_publishVideo;
	handle->publishMsg=yang_rtcstream_handle_publishMsg;

}

void yang_destroy_rtcstream_handle(YangRtcHandle* handle) {
	if(handle==NULL) return;
	YangRtcHandleContext* context=(YangRtcHandleContext*)handle->context;
	yang_destroy_peerConnection(&context->peerconn);
	yang_free(handle->context);
}
