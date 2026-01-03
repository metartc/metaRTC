//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_SYS_YANGTIMESTAMP_H_
#define INCLUDE_YANGUTIL_SYS_YANGTIMESTAMP_H_
#include <yangutil/yangtype.h>
typedef struct{
	void* session;
	void (*initAudioTime)(void* session);
	void (*setAudioSample)(void* session,uint32_t sample);
	void (*initVideoTime)(void* session);
	void (*setAudioTime)(void* session);
	void (*setVideoTime)(void* session,uint64_t ts);
	uint64_t (*getVideoTimestamp)(void* session);
	uint64_t (*getAudioTimeStamp)(void* session);
}YangTimestamp;
#ifdef __cplusplus
extern "C"{
#endif

int32_t yang_create_timestamp(YangTimestamp* yts);
void yang_destroy_timestamp(YangTimestamp* yts);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_YANGUTIL_SYS_YANGTIMESTAMP_H_ */

