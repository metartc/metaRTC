//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangwebrtc/YangRtcHandle.h>

#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#ifdef _WIN32
#include <winsock2.h>
#define be32toh ntohl
#endif

#include <string.h>


#include <yangwebrtc/YangPeerConnection.h>


//using namespace std;
typedef struct{
	int32_t isInit;
	YangAVInfo* avinfo;
	YangStreamConfig *streamconfig;
	YangContextStream* stream;
	YangFrame audioFrame;
	YangFrame videoFrame;
	YangPeerConnection peerconn;
}YangRtcHandleContext;
void g_yang_setPlayMediaConfig(	YangAudioParam *remote_audio,YangVideoParam *remote_video,void* user){
	if(user==NULL) return;
		YangRtcHandle* rtcHandle=(YangRtcHandle*)user;

	if(rtcHandle->streamconfig&&rtcHandle->stream){
		rtcHandle->stream->setMediaConfig(rtcHandle->stream->context,rtcHandle->streamconfig->uid,	remote_audio,remote_video);
	}
}

void g_yang_sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req,void* user){
	if(user==NULL) return;
	YangRtcHandle* rtcHandle=(YangRtcHandle*)user;
	rtcHandle->stream->sendRequest(rtcHandle->stream->context,puid, ssrc,req);
}
void g_rtc_receiveAudio(YangFrame *audioFrame,void* user){
	if(user==NULL) return;
	YangRtcHandle* rtcHandle=(YangRtcHandle*)user;
	if(rtcHandle&&rtcHandle->recvcb) rtcHandle->recvcb->receiveAudio(rtcHandle->recvcb->context,audioFrame);
}
void g_rtc_receiveVideo(YangFrame *videoFrame,void* user){
	if(user==NULL) return;
	YangRtcHandle* rtcHandle=(YangRtcHandle*)user;
	if(rtcHandle&&rtcHandle->recvcb) rtcHandle->recvcb->receiveVideo(rtcHandle->recvcb->context,videoFrame);
}






int32_t yang_rtcstream_handle_disconnectServer(void* pcontext){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	if(context->peerconn.disconnectServer) context->peerconn.disconnectServer(&context->peerconn.peer);
	return Yang_Ok;
}

int32_t yang_rtcstream_handle_getState(void* pcontext){
	if(pcontext==NULL) return 1;
		YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
    if(context->peerconn.getState)	return context->peerconn.getState(&context->peerconn.peer);
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
int32_t yang_rtcstream_handle_publishAudio(void* pcontext,YangStreamCapture* p){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	context->audioFrame.payload=p->getAudioData(p->context);
	context->audioFrame.nb=p->getAudioLen(p->context);
	context->audioFrame.pts=p->getAudioTimestamp(p->context);
	if(context->peerconn.publishAudio) return  context->peerconn.publishAudio(&context->peerconn.peer,&context->audioFrame);
	return Yang_Ok;
}
int yang_rtcstream_handle_notify(void* pcontext,int puid,YangRtcMessageType mess){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	if(context->peerconn.recvvideo_notify) context->peerconn.recvvideo_notify(&context->peerconn.peer,mess);
	return Yang_Ok;

}

void yang_rtcstream_handle_init(void* handle,void* pcontext,YangStreamConfig* pconf) {
	if(pcontext==NULL||pconf==NULL) return;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
	context->streamconfig=pconf;
	if(context->isInit==0){
		YangAVContext avcontext;
		memset(&avcontext,0,sizeof(YangAVContext));
		avcontext.avinfo=context->avinfo;
		avcontext.sendRequest=g_yang_sendRequest;
		avcontext.setPlayMediaConfig=g_yang_setPlayMediaConfig;

		avcontext.recvcb.receiveAudio=g_rtc_receiveAudio;
		avcontext.recvcb.receiveVideo=g_rtc_receiveVideo;

		context->peerconn.peer.avcontext=&avcontext;
		context->peerconn.peer.streamconfig=pconf;
		context->peerconn.peer.user=handle;
		context->peerconn.init(&context->peerconn.peer);
        if(pconf->streamOptType==Yang_Stream_Play&&context->stream){
        	YangRtcMessageNotify* notify=(YangRtcMessageNotify*)calloc(sizeof(YangRtcMessageNotify),1);
        	notify->context=pcontext;
        	notify->notify=yang_rtcstream_handle_notify;
        	context->stream->setRtcMessageNotify(context->stream->context,pconf->uid,notify);
        }

		context->isInit=1;
	}

}
int32_t yang_rtcstream_handle_connectRtcServer(void* pcontext){
	if(pcontext==NULL) return 1;
	YangRtcHandleContext* context=(YangRtcHandleContext*)pcontext;
    return context->peerconn.connectServer(&context->peerconn.peer);
}

void yang_create_rtcstream_handle(YangRtcHandle* handle,YangAVInfo* avinfo,YangContextStream* stream) {
	if(handle==NULL) return;
	handle->context=calloc(sizeof(YangRtcHandleContext),1);
	YangRtcHandleContext* context=(YangRtcHandleContext*)handle->context;

	memset(&context->peerconn,0,sizeof(YangPeerConnection));
    yang_create_peerConnection(&context->peerconn);
    context->peerconn.peer.mediaServer=avinfo->sys.mediaServer;
	context->avinfo=avinfo;
	context->stream=stream;
	context->streamconfig=NULL;
	context->isInit=0;

	memset(&context->audioFrame,0,sizeof(YangFrame));
	memset(&context->videoFrame,0,sizeof(YangFrame));

	handle->init=yang_rtcstream_handle_init;
	handle->connectRtcServer=yang_rtcstream_handle_connectRtcServer;
	handle->disconnectServer=yang_rtcstream_handle_disconnectServer;
	handle->getState=yang_rtcstream_handle_getState;
	handle->publishAudio=yang_rtcstream_handle_publishAudio;
	handle->publishVideo=yang_rtcstream_handle_publishVideo;
	handle->recvcb=NULL;
	handle->streamconfig=NULL;
	handle->stream=stream;

}

void yang_destroy_rtcstream_handle(YangRtcHandle* handle) {
	if(handle==NULL) return;
	YangRtcHandleContext* context=(YangRtcHandleContext*)handle->context;
	yang_destroy_peerConnection(&context->peerconn);
	yang_free(handle->context);
}
