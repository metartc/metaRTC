//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGUTIL_BUFFER2_YANGSORTBUFFER_H_
#define SRC_YANGUTIL_BUFFER2_YANGSORTBUFFER_H_
#include <pthread.h>
#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>


typedef struct{
	int64_t pts;
	YangFrame frame;
}YangSortFrame;

typedef struct{
	int32_t mediaType;
	int32_t uid;
	uint32_t capacity;
	int32_t cacheNum;
	uint32_t size;
	int32_t ret;
	//pthread_mutex_t mlock;
	YangSortFrame** frames;
	uint8_t* bufferManager;
}YangSortBufferSession;

typedef struct{
	YangSortBufferSession session;
	void (*resetIndex)(YangSortBufferSession* session);
	void (*initFrames)(YangSortBufferSession* session,int32_t pnum,int32_t unitsize);

	void (*putFrame)(YangSortBufferSession* session,int64_t pts,YangFrame* pframe);
	void (*removeFirst)(YangSortBufferSession* session);
	void (*getFrame)(YangSortBufferSession* session);
	YangFrame* (*getCurFrameRef)(YangSortBufferSession* session);
	int64_t (*getNextFrameTimestamp)(YangSortBufferSession* session);
	uint8_t* (*getFrameRef)(YangSortBufferSession* session,YangFrame* pframe);


}YangSortBuffer;

void yang_create_sortBuffer(YangSortBuffer* buf,int cacheNum);
void yang_destroy_sortBuffer(YangSortBuffer* buf);


#endif /* SRC_YANGUTIL_BUFFER2_YANGSORTBUFFER_H_ */
