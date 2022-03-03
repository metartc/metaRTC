//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_
#define YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_

#include <yangstream/YangStream.h>

void yang_create_streamHandle(int32_t transType,YangStreamHandle* streamHandle,int32_t puid,YangStreamConfig* streamconfig,YangAVInfo* pcontext,YangContextStream* stream,YangReceiveCallback* callback);
void yang_destroy_streamHandle(YangStreamHandle* stream);

#endif /* YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_ */
