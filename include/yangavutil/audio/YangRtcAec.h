/*
 * YangAceSpeex.h
 *
 *  Created on: 2020年9月8日
 *      Author: yang
 */

#ifndef YANGAUDIOPROC_YangRtcAec_H_
#define YANGAUDIOPROC_YangRtcAec_H_
#include <stdint.h>
typedef struct{
	void *context;
	void (*init)(void* context, int32_t sampleRate, int32_t pchannel,int32_t pframeSize,int32_t echopath);
	void (*closeAec)(void* context);

	void (*echoCapture)(void* context,short *rec, short *out);
	void (*preprocessRun)(void* context,short *pcm);
	void (*echoStateReset)(void* context);
	void (*echoPlayback)(void* context,short *play);
	void (*echoCancellation)(void* context,const short *rec, const short *play,
			short *out);
}YangRtcAec;
#ifdef __cplusplus
extern "C"{
#endif
void yang_init_rtcaec(YangRtcAec* aec);
void yang_destroy_rtcaec(YangRtcAec* aec);
#ifdef __cplusplus
}
#endif
#endif /* YANGAUDIOPROC_YANGAECSPEEX_H_ */
