//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangstream/YangStream.h>

#include "YangStreamRtc.h"


void yang_create_stream(int32_t mediaServer,int32_t transType,YangStreamHandle* streamHandle,YangStreamConfig* streamconfig,YangAVInfo* avinfo) {
	if(streamHandle==NULL) return;
	if(streamHandle->context==NULL) streamHandle->context=(YangStreamContext*)yang_calloc(sizeof(YangStreamContext),1);
	YangStreamContext* context=streamHandle->context;

	yang_memcpy(&context->streamconfig,streamconfig,sizeof(YangStreamConfig));

	context->avinfo=avinfo;


	context->streamInit = 0;
	context->audioStream=0;
	context->videoStream=0;
	context->uid=streamconfig->uid;
	context->netState = 1;
	context->transtype=transType;
	context->mediaServer=mediaServer;
	yang_create_stream_rtc(streamHandle);

}

void yang_destroy_stream(YangStreamHandle* stream) {
	yang_destroy_stream_rtc(stream);
	stream->context->avinfo=NULL;
	yang_free(stream->context);
}
