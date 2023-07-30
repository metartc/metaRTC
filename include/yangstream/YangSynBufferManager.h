//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef INCLUDE_YANGSTREAM_YANGSYNBUFFERMANAGER_H_
#define INCLUDE_YANGSTREAM_YANGSYNBUFFERMANAGER_H_
#include <yangstream/YangSynBuffer.h>
#include <yangutil/sys/YangVector.h>

yang_vector_declare2(YangSynBuffer)



typedef struct{
	yang_thread_mutex_t playBufferMutex;
	YangSynBuffer* playBuffer;
	YangSynBufferVector2* playBuffers;
}YangSynBufferSession;

typedef struct{
	YangSynBufferSession* session;
	YangSynBuffer* (*getSynBuffer)(YangSynBufferSession* session,int puid);
	int (*getPlayBufferIndex)(YangSynBufferSession* session,int puid);
	int32_t (*getAudioDatas)(YangSynBufferSession* session, uint8_t* mixBuf,YangFrame* pframe);
	int32_t (*addAudioBuffer)(YangSynBufferSession* session,void* yangAudioPlayBuffer,int32_t puid);
	void (*initAudioBuffer)(YangSynBufferSession* session);
	void (*removeAllAudioBuffer)(YangSynBufferSession* session);
	void (*removeAudioBuffer)(YangSynBufferSession* session,int32_t puid);
}YangSynBufferManager;

void yang_create_synBufferManager(YangSynBufferManager* man);
void yang_destroy_synBufferManager(YangSynBufferManager* man);
#endif /* INCLUDE_YANGSTREAM_YANGSYNBUFFERMANAGER_H_ */
