//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangstream/YangStreamRtmp.h>
#include <yangutil/yang_unistd.h>
#include <yangrtmp/YangRtmpHandle.h>
#include <string.h>
typedef struct{
	YangRtmpHandle *rtmpHandle;
	int32_t rtmpRet;
	char *buf;
	int32_t bufLen;
	RTMPPacket pac;
	RTMPPacket pac_a;

	YangFrame audioFrame;
	YangFrame videoFrame;
}YangStreamRtmp;
int32_t yang_stream_rtmp_isconnected(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 1;
	YangStreamRtmp* rtmp=(YangStreamRtmp*)context->context;
	if(rtmp->rtmpHandle&&rtmp->rtmpHandle->context)
		return rtmp->rtmpHandle->isConnect(rtmp->rtmpHandle->context);
	else
		return 0;
}


int32_t yang_stream_rtmp_getConnectState(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 1;
	YangStreamRtmp* rtmp=(YangStreamRtmp*)context->context;
	if(rtmp->rtmpHandle&&rtmp->rtmpHandle->isConnect(rtmp->rtmpHandle->context)) return Yang_Ok;
	return ERROR_SOCKET;
}
int32_t yang_stream_rtmp_disConnectServer(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 1;
	context->netState=1;
	YangStreamRtmp* rtmp=(YangStreamRtmp*)context->context;
	rtmp->rtmpHandle->RTMP_Close(rtmp->rtmpHandle->context);
	return Yang_Ok;
}
int32_t yang_stream_rtmp_reconnect(YangStreamContext* context) {
	if(context==NULL||context->context==NULL) return 1;
	YangStreamRtmp* rtmp=(YangStreamRtmp*)context->context;
	int32_t ret = rtmp->rtmpHandle->connectRtmpServer( rtmp->rtmpHandle->context,context->streamconfig.streamOptType, context->streamconfig.remoteIp, context->streamconfig.app, context->streamconfig.remotePort);
	if (ret)	{
		rtmp->rtmpHandle->RTMP_Close(rtmp->rtmpHandle->context);
		return ret;
	}
	return rtmp->rtmpHandle->HandleStream(rtmp->rtmpHandle->context,(char*)context->streamconfig.stream,  context->streamconfig.streamOptType, context->uid);
}

int32_t yang_stream_rtmp_connectServer(YangStreamContext* context) {
	if(context==NULL||context->context==NULL) return 1;
	if(yang_stream_rtmp_isconnected(context)) return Yang_Ok;

	YangStreamRtmp* rtmp=(YangStreamRtmp*)context->context;
	if (rtmp->rtmpHandle == NULL){
		rtmp->rtmpHandle = (YangRtmpHandle*)calloc(sizeof(YangRtmpHandle),1);//new YangRtmpBase();
		yang_create_rtmp(rtmp->rtmpHandle);
	}
	if(context->streamconfig.streamOptType){
		rtmp->pac.m_packetType = RTMP_PACKET_TYPE_VIDEO;
		rtmp->pac_a.m_packetType = RTMP_PACKET_TYPE_AUDIO;
	}
	rtmp->rtmpHandle->initRTMP(rtmp->rtmpHandle->context,context->streamconfig.remoteIp, context->streamconfig.app, context->streamconfig.remotePort);
	context->netState=yang_stream_rtmp_reconnect(context);
	return context->netState;
}
int32_t yang_stream_rtmp_publishAacheader(YangStreamContext* context,YangFrame *audioFrame){
	if(context==NULL||context->context==NULL) return 1;
	YangStreamRtmp* rtmp=(YangStreamRtmp*)context->context;
	rtmp->pac_a.m_nBodySize = audioFrame->nb;
	rtmp->pac_a.m_headerType = RTMP_PACKET_SIZE_LARGE;
	rtmp->pac_a.m_nTimeStamp = 0;
	rtmp->pac_a.m_nChannel = 0x04;
	rtmp->pac_a.m_nInfoField2 = rtmp->rtmpHandle->getStreamId(rtmp->rtmpHandle->context);
	rtmp->pac_a.m_body=(char*)audioFrame->payload;
	rtmp->rtmpRet = rtmp->rtmpHandle->sendPacket(rtmp->rtmpHandle->context,&rtmp->pac_a, 0);
	rtmp->rtmpRet = rtmp->rtmpHandle->sendPacket(rtmp->rtmpHandle->context,&rtmp->pac, 0);
	if(!rtmp->rtmpRet) context->netState=ERROR_SOCKET;
	return context->netState;
}
int32_t yang_stream_rtmp_publishAudioData(YangStreamContext* context,YangStreamCapture* audioFrame) {
	if(context==NULL||context->context==NULL) return 1;
	YangStreamRtmp* rtmp=(YangStreamRtmp*)context->context;
	rtmp->pac_a.m_nBodySize = audioFrame->getAudioLen(audioFrame->context);
	rtmp->pac_a.m_headerType = RTMP_PACKET_SIZE_LARGE;
	rtmp->pac_a.m_nTimeStamp = audioFrame->getAudioTimestamp(audioFrame->context);
	rtmp->pac_a.m_nChannel = 0x04;
	rtmp->pac_a.m_nInfoField2 = rtmp->rtmpHandle->getStreamId(rtmp->rtmpHandle->context);
	rtmp->pac_a.m_body=(char*)audioFrame->getAudioData(audioFrame->context);
	rtmp->rtmpRet = rtmp->rtmpHandle->sendPacket(rtmp->rtmpHandle->context,&rtmp->pac_a, 0);
	if(!rtmp->rtmpRet) context->netState=ERROR_SOCKET;
	return context->netState;
}

int32_t yang_stream_rtmp_publishVideoData(YangStreamContext* context,YangStreamCapture* videoFrame) {
	if(context==NULL||context->context==NULL) return 1;
	YangStreamRtmp* rtmp=(YangStreamRtmp*)context->context;
	//int32_t ret = Yang_Ok;
	rtmp->pac.m_nBodySize = videoFrame->getVideoLen(videoFrame->context);
	rtmp->pac.m_headerType = RTMP_PACKET_SIZE_LARGE;
	rtmp->pac.m_body=(char*)videoFrame->getVideoData(videoFrame->context);
	rtmp->pac.m_nChannel = 0x04;
	rtmp->pac.m_nTimeStamp = videoFrame->getVideoTimestamp(videoFrame->context);	//(unsigned int)videoTimestamp;
	rtmp->pac.m_nInfoField2 = rtmp->rtmpHandle->getStreamId(rtmp->rtmpHandle->context);
	rtmp->rtmpRet = rtmp->rtmpHandle->sendPacket(rtmp->rtmpHandle->context,&rtmp->pac, 0);
	if(!rtmp->rtmpRet) context->netState=ERROR_SOCKET;
	return context->netState;
}




int32_t yang_stream_rtmp_receiveData(YangStreamContext* context,int32_t *plen) {
	if(context==NULL||context->context==NULL) return 1;
	YangStreamRtmp* rtmp=(YangStreamRtmp*)context->context;
	if(!rtmp->rtmpHandle||!rtmp->rtmpHandle->isConnect(rtmp->rtmpHandle->context)) return 1;
	if (!rtmp->buf)
		rtmp->buf =(char*)malloc(1024*1024);//[1024 * 64];
	char *buf = rtmp->buf + 30;
	if (!rtmp->rtmpHandle->yangReadPacket(rtmp->rtmpHandle->context,&rtmp->pac, buf)) {
		return 1;
	}
	//while (!RTMPPacket_IsReady(&pac)&&rtmp->yangReadPacket(&pac, buf)) {
	//	yang_usleep(1);
	//}
	if (RTMPPacket_IsReady(&rtmp->pac)) {
		uint8_t *temp = NULL;
		if ((rtmp->pac.m_packetType == RTMP_PACKET_TYPE_AUDIO
				|| rtmp->pac.m_packetType == RTMP_PACKET_TYPE_VIDEO)) {
			temp = (uint8_t*) rtmp->pac.m_body;
			if (temp) {

				if (rtmp->pac.m_packetType == RTMP_PACKET_TYPE_AUDIO) {
					rtmp->audioFrame.payload=temp;
					rtmp->audioFrame.uid=context->uid;
					rtmp->audioFrame.nb=rtmp->pac.m_nBodySize;
					if(context->streamconfig.recvCallback.receiveAudio)
						context->streamconfig.recvCallback.receiveAudio(context->streamconfig.recvCallback.context,&rtmp->audioFrame);
					*plen = rtmp->pac.m_nBodySize;
				} else {
					rtmp->videoFrame.pts = rtmp->pac.m_nTimeStamp;
					rtmp->videoFrame.payload=temp;
					rtmp->videoFrame.uid=context->uid;
					rtmp->videoFrame.nb=rtmp->pac.m_nBodySize;
					if(context->streamconfig.recvCallback.receiveVideo)
						context->streamconfig.recvCallback.receiveVideo(context->streamconfig.recvCallback.context,&rtmp->videoFrame);
					*plen = rtmp->pac.m_nBodySize;
				}
			}

		} else {
			rtmp->rtmpHandle->clientPacket(rtmp->rtmpHandle->context,&rtmp->pac);
		}

		memset(rtmp->buf, 0,
				rtmp->pac.m_nBodySize < RTMP_MAX_BODY_SIZE ?
						rtmp->pac.m_nBodySize + 36 : RTMP_MAX_BODY_SIZE);
		memset(&rtmp->pac, 0, sizeof(RTMPPacket));
		rtmp->pac.m_body = NULL;

	}
	return Yang_Ok;
}

void yang_create_stream_rtmp(YangStreamHandle* handle) {
	if(handle==NULL||handle->context==NULL) return;
	handle->context->context=(YangStreamRtmp*)calloc(sizeof(YangStreamRtmp),1);
	YangStreamRtmp* context=(YangStreamRtmp*)handle->context->context;
	context->rtmpHandle = NULL;
	context->rtmpRet = 0;
	memset(&context->pac_a, 0, sizeof(context->pac_a));
	memset(&context->pac, 0, sizeof(context->pac));
	context->buf = NULL;
	context->bufLen = 0;
	memset(&context->audioFrame,0,sizeof(YangFrame));
	memset(&context->videoFrame,0,sizeof(YangFrame));

	handle->connectServer = yang_stream_rtmp_connectServer;
	handle->disConnectServer = yang_stream_rtmp_disConnectServer;
	handle->getConnectState = yang_stream_rtmp_getConnectState;

	handle->isconnected = yang_stream_rtmp_isconnected;
	handle->publishAacheader=yang_stream_rtmp_publishAacheader;
	handle->publishAudioData = yang_stream_rtmp_publishAudioData;
	handle->publishVideoData = yang_stream_rtmp_publishVideoData;
	handle->receiveData = yang_stream_rtmp_receiveData;
	handle->reconnect = yang_stream_rtmp_reconnect;

}
void yang_destroy_stream_rtmp(YangStreamHandle* handle){
	if(handle==NULL||handle->context==NULL) return;
	YangStreamRtmp* context=(YangStreamRtmp*)handle->context->context;
	yang_destroy_rtmp(context->rtmpHandle);
	context->pac.m_body=NULL;
	context->pac_a.m_body=NULL;
	yang_free(context->rtmpHandle);
    yang_free(context->buf);
}
