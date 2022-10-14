//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGAUDIOPROC_YangRtcAec_H_
#define YANGAUDIOPROC_YangRtcAec_H_
#include <stdint.h>
typedef struct{
	void *session;
	void (*init)(void* session, int32_t sampleRate, int32_t pchannel,int32_t pframeSize,int32_t echopath);
	void (*closeAec)(void* session);

	void (*echoCapture)(void* session,short *rec, short *out);
	void (*preprocessRun)(void* session,short *pcm);
	void (*echoStateReset)(void* session);
	void (*echoPlayback)(void* session,short *play);
	void (*echoCancellation)(void* session,const short *rec, const short *play,
			short *out);
}YangRtcAec;
#ifdef __cplusplus
extern "C"{
#endif
void yang_create_rtcaec(YangRtcAec* aec);
void yang_destroy_rtcaec(YangRtcAec* aec);
#ifdef __cplusplus
}
#endif
#endif /* YANGAUDIOPROC_YANGAECSPEEX_H_ */
