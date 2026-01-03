//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef INCLUDE_YANGCODEC_YANGCVIDEOENCODER_H_
#define INCLUDE_YANGCODEC_YANGCVIDEOENCODER_H_
#include "YangCodec.h"
#include <yangutil/yangavinfo.h>
//typedef struct{
//	YangRequestType request;
//	int32_t  requestValue;
//}YangRtcEncoderMessage;

typedef struct{
	void* session;
    int32_t (*init)(void* session,YangVideoInfo* video,YangVideoEncInfo* enc,YangCodecCallback* pcallback);
	int32_t (*encode)(void* session,YangFrame* frame);
	void (*sendMsgToEncoder)(void* session,YangRequestType request);
}YangCVideoEncoder;

#endif /* INCLUDE_YANGCODEC_YANGCVIDEOENCODER_H_ */
