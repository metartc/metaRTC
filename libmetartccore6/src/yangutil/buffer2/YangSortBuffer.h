//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGUTIL_BUFFER2_YANGSORTBUFFER_H_
#define SRC_YANGUTIL_BUFFER2_YANGSORTBUFFER_H_

#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>


typedef struct{
	int64_t pts;
	YangFrame frame;
}YangSortFrame;

typedef struct{

	uint32_t capacity;
	int32_t cacheNum;
	uint32_t vsize;
	int64_t lastPts;
	YangSortFrame** frames;
	uint8_t* bufferManager;
}YangSortBufferSession;

typedef struct{
	YangSortBufferSession session;
	void (*initFrames)(YangSortBufferSession* session,int32_t pnum,int32_t unitsize);
	void (*putFrame)(YangSortBufferSession* session,int64_t pts,YangFrame* pframe);
	void (*removeFirst)(YangSortBufferSession* session);
	YangFrame* (*getCurFrameRef)(YangSortBufferSession* session);
}YangSortBuffer;

void yang_create_sortBuffer(YangSortBuffer* buf,int cacheNum);
void yang_destroy_sortBuffer(YangSortBuffer* buf);


#endif /* SRC_YANGUTIL_BUFFER2_YANGSORTBUFFER_H_ */
