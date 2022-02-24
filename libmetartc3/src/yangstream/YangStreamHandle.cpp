//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangstream/YangStreamHandle.h>
#include <yangutil/sys/YangLog.h>
#include <yangstream/YangStreamSrt.h>

#include <stdio.h>
#include <string.h>


void yang_create_streamHandle(int32_t transType,YangStreamHandle* streamHandle,int32_t puid,YangAVInfo* avinfo,YangContextStream* stream,YangReceiveCallback* callback) {
	if(streamHandle==NULL) return;
	yang_create_stream(transType,streamHandle,puid,avinfo,stream,callback);
	if(transType==Yang_Srt){
		yang_create_stream_srt(streamHandle);
	}
}

void yang_destroy_streamHandle(YangStreamHandle* streamHandle) {
	if(streamHandle==NULL) return;
	if(streamHandle->context->transtype==Yang_Srt){
		yang_destroy_stream_srt(streamHandle);
	}
	yang_destroy_stream(streamHandle);
}




