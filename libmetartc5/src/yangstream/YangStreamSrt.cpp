//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangStreamSrt.h"
#include "yangsrt/YangSrtBase.h"
#include "yangsrt/YangTsMuxer.h"
#include "yangsrt/YangTsdemux.h"
#include <yangsrt/YangTsdemux.h>
#ifndef __ANDROID__
#include "yangutil/sys/YangLog.h"
#define YangSrtUnitSize 1316

typedef struct{
	int32_t bufLen;
	int32_t bufReceiveLen;
	int32_t bufRemainLen;
	char *buffer;
	YangSrtBase *srt;
	ts_media_data_callback_I callback;
	YangTsdemux demux;
	YangTsMuxer ts;

	YangFrame audioFrame;
	YangFrame videoFrame;
}YangStreamSrt;



int32_t yang_stream_srt_isconnected(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 1;
	YangStreamSrt* srt=(YangStreamSrt*)context->context;
	if(srt->srt)
		return srt->srt->getSrtSocketStatus()==SRTS_CONNECTED;
	else
		return 0;
}


int32_t yang_stream_srt_getConnectState(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 1;
	YangStreamSrt* srt=(YangStreamSrt*)context->context;
	int32_t ret=srt->srt->getSrtSocketStatus();
	if(ret==SRTS_CONNECTED)
		return Yang_Ok;
	else
		return Yang_SRTS_SocketBase+ret;
	//return ERROR_SOCKET;
}
int32_t yang_stream_srt_reconnect(YangStreamContext* context) {
	if(context==NULL||context->context==NULL) return 1;
	YangStreamSrt* srt=(YangStreamSrt*)context->context;
	srt->bufLen=0;
	char url[1024]={0};
	sprintf(url,"%s.sls.com/%s/%s",context->streamconfig.streamOptType?"uplive":"live",context->streamconfig.app,context->streamconfig.stream);
	int32_t ret=srt->srt->initConnect(url);
			if(ret) return ret;
	return srt->srt->connectServer();

}



int32_t yang_stream_srt_connectServer(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 1;

	if(yang_stream_srt_isconnected(context)) return Yang_Ok;
	YangStreamSrt* srt=(YangStreamSrt*)context->context;
	if(!srt->buffer) srt->buffer=new char[Yang_Srt_CacheSize];
	if(srt->srt==NULL) srt->srt=new YangSrtBase();
	if(!srt->srt->m_contextt){
		srt->bufLen=0;

		sprintf(context->streamconfig.url,"%s.sls.com/%s/%s",context->streamconfig.streamOptType==Yang_Stream_Publish?"uplive":"live",context->streamconfig.app,context->streamconfig.stream);

		srt->srt->init((char*)context->streamconfig.remoteIp,context->streamconfig.remotePort);
	}
	int32_t ret=srt->srt->initConnect((char*)context->streamconfig.url);
	if(ret) return ret;
	context->netState=srt->srt->connectServer();
	return context->netState;


}

int32_t yang_stream_srt_disConnectServer(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 1;
	YangStreamSrt* srt=(YangStreamSrt*)context->context;
	srt->srt->closeSrt();
	context->netState=1;
	return Yang_Ok;
}


int32_t yang_stream_srt_publishAudioData(YangStreamContext* context,YangStreamCapture* audioFrame) {
	if(context==NULL||context->context==NULL) return 1;
	YangStreamSrt* srt=(YangStreamSrt*)context->context;
	vector<YangTsBuffer> sb ;
	srt->ts.encode(audioFrame->getAudioData(audioFrame->context), audioFrame->getAudioLen(audioFrame->context),
			1, audioFrame->getAudioTimestamp(audioFrame->context), audioFrame->getAudioType(audioFrame->context) == 0 ? TS_AAC : TS_OPUS, &sb);
	int32_t ret=Yang_Ok;
	for (size_t i = 0; i < sb.size(); i++) {
		memcpy(srt->buffer+srt->bufLen,sb.at(i).data(),188);
		srt->bufLen+=188;
		if(srt->bufLen==YangSrtUnitSize){
			ret=srt->srt->publish((char*) srt->buffer, YangSrtUnitSize);
			srt->bufLen=0;
		}

		if (ret){
			yang_error("publish audio error..%d",ret);
			context->netState=ret;
			return ret;
		}

	}
	return Yang_Ok;
}


int32_t yang_stream_srt_publishVideoData(YangStreamContext* context,YangStreamCapture* videoFrame) {
	if(context==NULL||context->context==NULL) return 1;
	YangStreamSrt* srt=(YangStreamSrt*)context->context;
	int32_t ret=Yang_Ok;
	vector<YangTsBuffer> sb;
	srt->ts.encode(videoFrame->getVideoData(videoFrame->context), videoFrame->getVideoLen(videoFrame->context),
			videoFrame->getVideoFrametype(videoFrame->context), videoFrame->getVideoTimestamp(videoFrame->context), TS_H264, &sb);
	for (size_t i = 0; i < sb.size(); i++) {
		memcpy(srt->buffer+srt->bufLen,sb.at(i).data(),188);
		srt->bufLen+=188;
		if(srt->bufLen==YangSrtUnitSize){
			ret=srt->srt->publish((char*) srt->buffer, YangSrtUnitSize);
			srt->bufLen=0;
		}
		if (ret)	{
			return ret;
			context->netState=ret;
		}
	}
	return Yang_Ok;
}


void yang_stream_srt_on_data_callback(void* pcontext,SRT_DATA_MSG_PTR data_ptr,
		uint32_t  media_type, uint64_t dts, uint64_t pts) {
	if(pcontext==NULL) return;
	YangStreamContext* context=(YangStreamContext*)pcontext;
	YangStreamSrt* srt=(YangStreamSrt*)context->context;
	uint8_t *temp = data_ptr->get_data();
	int32_t len = data_ptr->data_len();

	if ((media_type == Yang_H264_PID || media_type == Yang_H265_PID)
			&& context->videoStream) {
		//int64_t t_timestamp = dts;
		srt->videoFrame.pts=dts;
		srt->videoFrame.uid=context->uid;
		srt->videoFrame.payload=temp;
		srt->videoFrame.nb=len;
		if (context->streamconfig.recvCallback.receiveVideo)
			context->streamconfig.recvCallback.receiveVideo(context->streamconfig.recvCallback.context,&srt->videoFrame);
	} else if (media_type == Yang_AAC_PID && context->audioStream) {
		srt->audioFrame.uid=context->uid;
		srt->audioFrame.nb=len;
		srt->audioFrame.payload=temp;
		if (context->streamconfig.recvCallback.receiveAudio)
			context->streamconfig.recvCallback.receiveAudio(context->streamconfig.recvCallback.context,&srt->audioFrame);
	} else if (media_type == Yang_OPUS_PID && context->audioStream) {
		srt->audioFrame.uid=context->uid;
		srt->audioFrame.nb=len;
		srt->audioFrame.payload=temp;
		if (context->streamconfig.recvCallback.receiveAudio)
			context->streamconfig.recvCallback.receiveAudio(context->streamconfig.recvCallback.context,&srt->audioFrame);
	}
	temp = NULL;
}

int32_t yang_stream_srt_receiveData(YangStreamContext* context,int32_t *plen) {
	if(context==NULL||context->context==NULL) return 1;
	YangStreamSrt* srt=(YangStreamSrt*)context->context;

	if (!srt->srt)
		return Yang_Ok;
	srt->bufReceiveLen = 0;
	if (srt->srt->receive(srt->buffer + srt->bufRemainLen, &srt->bufReceiveLen))
		return ERROR_SRT_PushFailure;
	srt->bufLen = srt->bufReceiveLen + srt->bufRemainLen;
	if (srt->bufLen < YangSrtUnitLen) {
		srt->bufRemainLen = srt->bufLen;
		return Yang_Ok;
	}
	srt->bufRemainLen = srt->bufLen % YangSrtUnitLen;
	*plen = srt->bufLen;
	auto input_ptr = std::make_shared<SRT_DATA_MSG>((uint8_t*) srt->buffer,
			srt->bufLen - srt->bufRemainLen);
	srt->demux.decode(input_ptr, &srt->callback);
	if (srt->bufRemainLen > 0)
		memcpy(srt->buffer, srt->buffer + (srt->bufLen - srt->bufRemainLen),
				srt->bufRemainLen);
	return Yang_Ok;
}


int32_t yang_stream_srt_sendPmt(YangStreamContext* context){
	if(context==NULL||context->context==NULL) return 1;
	YangStreamSrt* srt=(YangStreamSrt*)context->context;
	vector<YangTsBuffer> sb;
	srt->ts.encodePmtWithoutData(&sb);
	int32_t ret=Yang_Ok;
	for (size_t i = 0; i < sb.size(); i++) {
		ret=srt->srt->publish((char*) sb.at(i).data(), 188);
		if (ret){
			yang_error("publish audio error..%d",i);
			return ret;
		}

	}
	sb.clear();
	return Yang_Ok;
}

void yang_create_stream_srt(YangStreamHandle* handle) {
	if(handle==NULL||handle->context==NULL) return;
	handle->context->context=calloc(sizeof(YangStreamSrt),1);
	YangStreamSrt* srt=(YangStreamSrt*)handle->context->context;
	srt->callback.context=handle->context;
	srt->callback.on_data_callback=yang_stream_srt_on_data_callback;
	srt->bufLen = 0;
	srt->buffer = NULL;
	srt->bufReceiveLen = 0, srt->bufRemainLen = 0;
	srt->srt = NULL;
	memset(&srt->audioFrame,0,sizeof(YangFrame));
	memset(&srt->videoFrame,0,sizeof(YangFrame));

	handle->connectServer = yang_stream_srt_connectServer;
		handle->disConnectServer = yang_stream_srt_disConnectServer;
		handle->getConnectState = yang_stream_srt_getConnectState;

		handle->isconnected = yang_stream_srt_isconnected;
		handle->publishAudioData = yang_stream_srt_publishAudioData;
		handle->publishVideoData = yang_stream_srt_publishVideoData;
		handle->receiveData = yang_stream_srt_receiveData;
		handle->reconnect = yang_stream_srt_reconnect;
}

void yang_destroy_stream_srt(YangStreamHandle* handle) {
	if(handle==NULL||handle->context==NULL) return;
	YangStreamSrt* srt=(YangStreamSrt*)handle->context->context;
	if (srt->buffer)
		delete[] srt->buffer;
	srt->buffer = NULL;
}
#endif

