//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_
#define YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_

#include <yangstream/YangStream.h>

void yang_create_streamHandle(YangStreamHandle* streamHandle,YangStreamConfig* streamconfig,YangAVInfo* pcontext);
void yang_destroy_streamHandle(YangStreamHandle* stream);

#endif /* YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_ */
