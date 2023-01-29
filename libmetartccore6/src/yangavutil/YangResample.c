//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangavutil/audio/YangResample.h>
#include <speex/speex_resampler.h>

typedef struct{
	int32_t in_sample;
	int32_t out_sample;
	int32_t context;
	int32_t channel;
    spx_uint32_t in_len;
    spx_uint32_t out_len;
    SpeexResamplerState *state;
}YangResampleContext;

void yang_resample_init(void* pcontext,int32_t pchannel,int32_t pinsample,int32_t poutsample){
	if(pcontext==NULL) return;
	YangResampleContext* context=(YangResampleContext*)pcontext;
	context->channel=pchannel;
	context->in_sample=pinsample;
	context->out_sample=poutsample;
	if(!context->state)    context->state = speex_resampler_init(context->channel, context->in_sample, context->out_sample, 10, 0);
	context->context=1;
}

void yang_resample_resample(void* pcontext,const  short *pin,uint32_t  pinLen, short* pout,uint32_t*  poutLen){
	if(pcontext==NULL) return;
	YangResampleContext* context=(YangResampleContext*)pcontext;
	context->in_len=pinLen;
	if(context->channel==1){
        if(context->state) speex_resampler_process_int(context->state, 0, pin, &context->in_len, pout, poutLen);
	}else{
        if(context->state) speex_resampler_process_interleaved_int(context->state, pin, &context->in_len, pout, poutLen);
	}
}

void yang_create_resample(YangResample* res){
	if(res==NULL) return;
	res->context=(YangResampleContext*)yang_calloc(sizeof(YangResampleContext),1);
	res->init=yang_resample_init;
	res->resample=yang_resample_resample;
}
void yang_destroy_resample(YangResample* res){
	if(res==NULL) return;
	YangResampleContext* context=(YangResampleContext*)res->context;
	if(context&&context->state) speex_resampler_destroy(context->state);
	yang_free(res->context);
}



