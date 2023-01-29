//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGAUDIOPROC_YANGRESAMPLE_H_
#define YANGAUDIOPROC_YANGRESAMPLE_H_

#include <yangutil/yangtype.h>
typedef struct{
	void* context;
	void (*init)(void* pcontext,int32_t pchannel,int32_t pinsample,int32_t poutsample);
	void (*resample)(void* context,const short *pin,uint32_t  pinLen,short* pout,uint32_t  *poutLen);
}YangResample;
#ifdef __cplusplus
extern "C"{
#endif
void yang_create_resample(YangResample* res);
void yang_destroy_resample(YangResample* res);
#ifdef __cplusplus
}
#endif

#endif /* YANGAUDIOPROC_YANGRESAMPLE_H_ */
