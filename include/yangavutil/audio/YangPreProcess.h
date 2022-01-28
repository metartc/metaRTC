/*
 * YangPreProcessSpeex.h
 *
 *  Created on: 2020年9月29日
 *      Author: yang
 */

#ifndef YANGAUDIOPROC_YANGPREPROCESSSPEEX_H_
#define YANGAUDIOPROC_YANGPREPROCESSSPEEX_H_
#include <stdint.h>

typedef struct{
	void* context;
	void (*init)(void* context,int32_t pFrameSize, int32_t sampleRate, int32_t pchannel);
	void (*state_reset)(void* context);
	void (*preprocess_run)(void* context,short *pcm);
	void (*closePreprocess)(void* context);
}YangPreProcess;
#ifdef __cplusplus
extern "C"{
#endif

void yang_init_preProcess(YangPreProcess* pre);
void yang_destroy_preProcess(YangPreProcess* pre);

#ifdef __cplusplus
}
#endif

#endif /* YANGAUDIOPROC_YANGPREPROCESSSPEEX_H_ */
