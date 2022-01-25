/*
 * YangResample.h
 *
 *  Created on: 2020年9月10日
 *      Author: yang
 */

#ifndef YANGAUDIOPROC_YANGRESAMPLE_H_
#define YANGAUDIOPROC_YANGRESAMPLE_H_

#include <stdint.h>
typedef struct{
	void* context;
	void (*init)(void* pcontext,int32_t pchannel,int32_t pinsample,int32_t poutsample);
	void (*resample)(void* context,const short *pin,uint32_t  pinLen,short* pout,uint32_t  *poutLen);
}YangResample;
#ifdef __cplusplus
extern "C"{
#endif
void yang_init_resample(YangResample* res);
void yang_destroy_resample(YangResample* res);
#ifdef __cplusplus
}
#endif

#endif /* YANGAUDIOPROC_YANGRESAMPLE_H_ */
