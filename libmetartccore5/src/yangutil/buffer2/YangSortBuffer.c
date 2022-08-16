//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangutil/buffer2/YangSortBuffer.h>

#include <yangutil/yangavinfotype.h>
#include <string.h>
#include <malloc.h>

void yang_sortBuffer_resetIndex(YangSortBufferSession* session) {
	session->putIndex = 0;
	session->getIndex = 0;
	session->nextIndex=0;

	session->ret=0;
	session->size = 0;
}


void yang_sortBuffer_initFrames(YangSortBufferSession* session,int32_t pnum, int unitsize) {
	if (session->bufferManager == NULL)		session->bufferManager = (uint8_t*)calloc(unitsize ,pnum);//new YangBufferManager(pnum, unitsize);
	if (session->frames == NULL) {
		session->frames =(YangFrame**) calloc(sizeof(YangFrame*),pnum);//new YangFrame*[pnum];
		for (int32_t i = 0; i < pnum; i++) {
			session->frames[i] = (YangFrame*)calloc(sizeof(YangFrame),1);//new YangFrame();
			memset(session->frames[i],0,sizeof(YangFrame));
			session->frames[i]->payload = session->bufferManager+i*sizeof(YangFrame);
		}
	}
}
void yang_sortBuffer_putFrame(YangSortBufferSession* session,YangFrame *pframe) {

	if (!pframe||session->size>=session->cache_num)		return;
	pthread_mutex_lock(&session->mlock);

	yang_frame_copy_buffer(pframe, session->frames[session->putIndex++]);
	if (session->putIndex >= session->cache_num)		session->putIndex = 0;
	session->size++;
	pthread_mutex_unlock(&session->mlock);

}
void yang_sortBuffer_getFrame(YangSortBufferSession* session,YangFrame *pframe) {
	if (!pframe||!session->size)	return;

	pthread_mutex_lock(&session->mlock);
	yang_frame_copy_buffer(session->frames[session->getIndex++], pframe);
	if (session->getIndex >= session->cache_num)		session->getIndex = 0;
	session->size--;
	pthread_mutex_unlock(&session->mlock);
}
uint8_t* yang_sortBuffer_getFrameRef(YangSortBufferSession* session,YangFrame *pframe) {
	if (!session->size||!pframe)				return NULL;
	pthread_mutex_lock(&session->mlock);
	yang_frame_copy_nobuffer(session->frames[session->getIndex], pframe);
	uint8_t *p = session->frames[session->getIndex]->payload;
	session->getIndex++;
	if (session->getIndex >= session->cache_num)		session->getIndex = 0;
	session->size--;
	pthread_mutex_unlock(&session->mlock);

	return p;

}
YangFrame* yang_sortBuffer_getCurFrameRef(YangSortBufferSession* session) {
	if(!session->size) return NULL;
    session->nextIndex=session->getIndex;
    if(session->nextIndex>=session->cache_num) session->nextIndex=0;
    return session->frames[session->nextIndex];
}

int64_t yang_sortBuffer_getNextFrameTimestamp(YangSortBufferSession* session){
	if(!session->size) return 0;
    session->nextIndex=session->getIndex;
    if(session->nextIndex>=session->cache_num) session->nextIndex=0;
    return session->frames[session->nextIndex]->pts;

}

void yang_create_sortBuffer(YangSortBuffer* buf){
	YangSortBufferSession* session=&buf->session;
	yang_sortBuffer_resetIndex(session);
	session->cache_num = 5;
	session->mediaType = 1;
	session->uid = -1;
	session->frames = NULL;
	session->bufferManager = NULL;

	pthread_mutex_init(&session->mlock,NULL);

	buf->initFrames=yang_sortBuffer_initFrames;
	buf->getCurFrameRef=yang_sortBuffer_getCurFrameRef;
	buf->getFrameRef=yang_sortBuffer_getFrameRef;
	buf->getNextFrameTimestamp=yang_sortBuffer_getNextFrameTimestamp;
	buf->putFrame=yang_sortBuffer_putFrame;
	buf->getFrame=yang_sortBuffer_getFrame;
	buf->resetIndex=yang_sortBuffer_resetIndex;
}
void yang_destroy_sortBuffer(YangSortBuffer* buf){
	YangSortBufferSession* session=&buf->session;
	if (session->frames) {
        for (uint32_t i = 0; i < session->cache_num; i++) {
			yang_free(session->frames[i]);
		}
		yang_free(session->frames);
	}
	yang_free(session->bufferManager);
	pthread_mutex_destroy(&session->mlock);
}
