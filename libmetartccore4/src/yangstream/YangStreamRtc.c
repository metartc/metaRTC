//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangstream/YangStreamRtc.h>

#include <yangutil/yangavinfotype.h>
#include <yangutil/yang_unistd.h>
#include <string.h>


int32_t yang_stream_rtc_isconnected(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 0;
	YangRtcHandle* rtc=(YangRtcHandle*)context->context;
	return rtc->getState(rtc->context);
}
int32_t yang_stream_rtc_isAlive(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 0;
	YangRtcHandle* rtc=(YangRtcHandle*)context->context;
	return rtc->isAlive(rtc->context);
}
int32_t yang_stream_rtc_getConnectState(YangStreamContext* context){

	if(context==NULL||context->context==NULL) return ERROR_SOCKET;
	YangRtcHandle* rtc=(YangRtcHandle*)context->context;
	return rtc->getState(rtc->context)?Yang_Ok:ERROR_SOCKET;
}
int32_t yang_stream_rtc_disConnectServer(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 1;
	context->netState=1;
	YangRtcHandle* rtc=(YangRtcHandle*)context->context;
	return rtc->disconnectServer(rtc->context);
}
int32_t yang_stream_rtc_reconnect(YangStreamContext* context) {
	if(context==NULL||context->context==NULL) return 1;
	YangRtcHandle* rtc=(YangRtcHandle*)context->context;
	return rtc->connectRtcServer(rtc->context);
}
int32_t yang_stream_rtc_connectServer(YangStreamContext* context) {
	if(context==NULL||context->context==NULL) return 1;

	if(yang_stream_rtc_isconnected(context)) return Yang_Ok;
	YangRtcHandle* rtc=(YangRtcHandle*)context->context;

	rtc->init(rtc,rtc->context);
	context->netState=yang_stream_rtc_reconnect(context);
	return context->netState;

}



int32_t yang_stream_rtc_publishAudioData(YangStreamContext* context,YangStreamCapture *audioFrame) {
	if(context==NULL||context->context==NULL) return 1;
	YangRtcHandle* rtc=(YangRtcHandle*)context->context;
	if(rtc->publishAudio) return rtc->publishAudio(rtc->context,audioFrame);
	return Yang_Ok;
}

int32_t yang_stream_rtc_publishVideoData(YangStreamContext* context,YangStreamCapture* videoFrame) {
	if(context==NULL||context->context==NULL) return 1;
	YangRtcHandle* rtc=(YangRtcHandle*)context->context;
	if(rtc->publishVideo) return rtc->publishVideo(rtc->context,videoFrame);
	return Yang_Ok;
}

int32_t yang_stream_rtc_publishMsg(YangStreamContext* context,YangFrame* msgFrame) {
	if(context==NULL||context->context==NULL) return 1;
	YangRtcHandle* rtc=(YangRtcHandle*)context->context;
	if(rtc->publishMsg) return rtc->publishMsg(rtc->context,msgFrame);
	return Yang_Ok;
}
int32_t yang_stream_rtc_receiveData(YangStreamContext* context,int32_t *plen) {

	return Yang_Ok;
}
int32_t yang_stream_rtc_sendNotifyMsg(YangStreamContext* context,YangRtcMessageType mess){
	if(context==NULL||context->context==NULL) return 1;
	YangRtcHandle* rtc=(YangRtcHandle*)context->context;
	if(rtc->sendRtcMessage) rtc->sendRtcMessage(rtc->context,mess);
	return Yang_Ok;
}

void yang_create_stream_rtc(YangStreamHandle* handle){
	if(handle==NULL||handle->context==NULL) return;
	handle->context->context=calloc(sizeof(YangRtcHandle),1);
	YangRtcHandle* rtc=(YangRtcHandle*)handle->context->context;
	yang_create_rtcstream_handle(rtc,handle->context->avinfo);

	memcpy(&rtc->streamconfig,&handle->context->streamconfig,sizeof(YangStreamConfig));


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
	YangRtcHandle* rtc=(YangRtcHandle*)handle->context->context;
	yang_destroy_rtcstream_handle(rtc);
	yang_free(rtc);
}
