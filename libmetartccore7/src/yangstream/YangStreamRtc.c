//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangstream/YangStreamRtc.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/yangavinfotype.h>
#include <yangrtc/YangPeerConnection.h>


int32_t yang_stream_rtc_isconnected(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 0;
	YangPeerConnection* rtc=(YangPeerConnection*)context->context;
	return rtc->isConnected(&rtc->peer);
}
int32_t yang_stream_rtc_isAlive(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 0;
	YangPeerConnection* rtc=(YangPeerConnection*)context->context;
	return rtc->isAlive(&rtc->peer);
}
int32_t yang_stream_rtc_getConnectState(YangStreamContext* context){

	if(context==NULL||context->context==NULL) return ERROR_SOCKET;
	YangPeerConnection* rtc=(YangPeerConnection*)context->context;
	return rtc->isConnected(&rtc->peer)?Yang_Ok:ERROR_SOCKET;
}
int32_t yang_stream_rtc_disConnectServer(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 1;
	context->netState=1;
	YangPeerConnection* rtc=(YangPeerConnection*)context->context;
	return rtc->close(&rtc->peer);
}
int32_t yang_stream_rtc_reconnect(YangStreamContext* context) {
	if(context==NULL||context->context==NULL) return 1;
	YangPeerConnection* rtc=(YangPeerConnection*)context->context;
	return rtc->connectSfuServer(&rtc->peer);
}
int32_t yang_stream_rtc_connectServer(YangStreamContext* context) {
	if(context==NULL||context->context==NULL) return 1;

	if(yang_stream_rtc_isconnected(context)) return Yang_Ok;
	YangPeerConnection* rtc=(YangPeerConnection*)context->context;

	rtc->init(&rtc->peer);
	context->netState=yang_stream_rtc_reconnect(context);
	return context->netState;

}



int32_t yang_stream_rtc_publishAudioData(YangStreamContext* context,YangStreamCapture *audioFrame) {
	if(context==NULL||context->context==NULL) return 1;
	YangPeerConnection* rtc=(YangPeerConnection*)context->context;
	if(rtc->on_audio) return rtc->on_audio(&rtc->peer,audioFrame->getAudioFrame(audioFrame->context));
	return Yang_Ok;
}

int32_t yang_stream_rtc_publishVideoData(YangStreamContext* context,YangStreamCapture* videoFrame) {
	if(context==NULL||context->context==NULL) return 1;
	YangPeerConnection* rtc=(YangPeerConnection*)context->context;
	if(rtc->on_video) return rtc->on_video(&rtc->peer,videoFrame->getVideoFrame(videoFrame->context));
	return Yang_Ok;
}

int32_t yang_stream_rtc_publishMsg(YangStreamContext* context,YangFrame* msgFrame) {
	if(context==NULL||context->context==NULL) return 1;
	YangPeerConnection* rtc=(YangPeerConnection*)context->context;
	if(rtc->on_message) return rtc->on_message(&rtc->peer,msgFrame);
	return Yang_Ok;
}
int32_t yang_stream_rtc_receiveData(YangStreamContext* context,int32_t *plen) {

	return Yang_Ok;
}
int32_t yang_stream_rtc_sendNotifyMsg(YangStreamContext* context,YangRtcMessageType mess){
	if(context==NULL||context->context==NULL) return 1;
	YangPeerConnection* rtc=(YangPeerConnection*)context->context;
	if(rtc->sendRtcMessage) rtc->sendRtcMessage(&rtc->peer,mess);
	return Yang_Ok;
}

void yang_create_stream_rtc(YangStreamHandle* handle){
	if(handle==NULL||handle->context==NULL) return;
	handle->context->context=yang_calloc(sizeof(YangPeerConnection),1);
	YangPeerConnection* rtc=(YangPeerConnection*)handle->context->context;
	rtc->peer.avinfo=handle->context->avinfo;
	yang_memcpy(&rtc->peer.streamconfig,&handle->context->streamconfig,sizeof(YangStreamConfig));
	yang_create_peerConnection(rtc);



	handle->connectServer=yang_stream_rtc_connectServer;
	handle->disConnectServer=yang_stream_rtc_disConnectServer;
	handle->getConnectState=yang_stream_rtc_getConnectState;
	handle->isAlive=yang_stream_rtc_isAlive;
	handle->isconnected=yang_stream_rtc_isconnected;
	handle->publishAudioData=yang_stream_rtc_publishAudioData;
	handle->publishVideoData=yang_stream_rtc_publishVideoData;
	handle->publishMsg=yang_stream_rtc_publishMsg;
	handle->receiveData=yang_stream_rtc_receiveData;
	handle->reconnect=yang_stream_rtc_reconnect;
	handle->sendRtcMessage=yang_stream_rtc_sendNotifyMsg;
}
void yang_destroy_stream_rtc(YangStreamHandle* handle){
	if(handle==NULL||handle->context==NULL) return;
	YangPeerConnection* rtc=(YangPeerConnection*)handle->context->context;
	yang_destroy_peerConnection(rtc);
	yang_free(rtc);
}
