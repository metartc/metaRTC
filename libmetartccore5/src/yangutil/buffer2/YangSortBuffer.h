//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGUTIL_BUFFER2_YANGSORTBUFFER_H_
#define SRC_YANGUTIL_BUFFER2_YANGSORTBUFFER_H_
#include <pthread.h>
#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>
#define yang_reindex2(p)  if(p!=NULL) p->resetIndex(&p->mediaBuffer);

typedef struct{
	int32_t mediaType;
	int32_t uid;
	uint32_t putIndex;
	uint32_t getIndex;
	uint32_t cache_num;
	uint32_t size;
	uint32_t nextIndex;
	int32_t ret;
	pthread_mutex_t mlock;
	YangFrame** frames;
	uint8_t* bufferManager;
}YangSortBufferSession;

typedef struct{
	YangSortBufferSession session;
	void (*resetIndex)(YangSortBufferSession* session);
	void (*initFrames)(YangSortBufferSession* session,int pnum,int unitsize);

	void (*putFrame)(YangSortBufferSession* session,YangFrame* pframe);
	void (*getFrame)(YangSortBufferSession* session,YangFrame* pframe);
	YangFrame* (*getCurFrameRef)(YangSortBufferSession* session);
	int64_t (*getNextFrameTimestamp)(YangSortBufferSession* session);
	uint8_t* (*getFrameRef)(YangSortBufferSession* session,YangFrame* pframe);


}YangSortBuffer;

void yang_create_sortBuffer(YangSortBuffer* buf);
void yang_destroy_sortBuffer(YangSortBuffer* buf);


#endif /* SRC_YANGUTIL_BUFFER2_YANGSORTBUFFER_H_ */
