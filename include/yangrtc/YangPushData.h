//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef INCLUDE_YANGPUSH_YANGPUSHDATA_H_
#define INCLUDE_YANGPUSH_YANGPUSHDATA_H_

#include <yangutil/yangavinfo.h>

typedef struct{
	void* session;
	int32_t (*onAudioData)(void* session,YangPushData *audioData);
	int32_t (*onVideoData)(void* session,YangPushData *videoData);
}YangPushCallback;

typedef struct{
	void* session;
	void (*reset)(void* session);
	int32_t (*pushAudioData)(void* session,YangFrame* audioFrame,YangPushCallback* callback);
}YangPushAudioData;

typedef struct{
	void* session;
	void (*reset)(void* session);
	int32_t (*pushVideoData)(void* session,YangFrame* videoFrame,YangPushCallback* callback);
}YangPushVideoData;



#ifdef __cplusplus
extern "C"{
#endif

int32_t yang_create_pushAudioData(YangPushAudioData* data,YangAudioCodec codec,uint32_t sample,uint32_t channel);
void yang_destroy_pushAudioData(YangPushAudioData* data);

int32_t yang_create_pushVideoData(YangPushVideoData* data,YangVideoCodec codec,uint32_t pktCount);
void yang_destroy_pushVideoData(YangPushVideoData* data);

void yang_pushData_clear(YangPushData* pushData);
void yang_pushData_reset(YangPushData* pushData);
void yang_pushData_copyToPush(YangPushData* pushData);
void yang_pushData_setLength(YangPushData* pushData,uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_YANGPUSH_YANGPUSHDATA_H_ */
