//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangaudio/YangResample.h>
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

static void yang_init(void* pcontext,int32_t pchannel,int32_t pinsample,int32_t poutsample){
	YangResampleContext* context=(YangResampleContext*)pcontext;

	if(pcontext==NULL)
		return;

	context->channel=pchannel;
	context->in_sample=pinsample;
	context->out_sample=poutsample;

	if(!context->state)
		context->state = speex_resampler_init(context->channel, context->in_sample, context->out_sample, 10, 0);

	context->context=1;
}

static void yang_resample(void* pcontext,const  short *pin,uint32_t  pinLen, short* pout,uint32_t*  poutLen){
	YangResampleContext* context=(YangResampleContext*)pcontext;

	if(pcontext==NULL)
		return;

	context->in_len=pinLen;

	if(context->channel==1){
        if(context->state)
        	speex_resampler_process_int(context->state, 0, pin, &context->in_len, pout, poutLen);
	}else{
        if(context->state)
        	speex_resampler_process_interleaved_int(context->state, pin, &context->in_len, pout, poutLen);
	}
}

void yang_create_resample(YangResample* res){
	if(res==NULL)
		return;

	res->context=(YangResampleContext*)yang_calloc(sizeof(YangResampleContext),1);
	res->init=yang_init;
	res->resample=yang_resample;
}

void yang_destroy_resample(YangResample* res){
	YangResampleContext* context;

	if(res==NULL)
		return;

	context=(YangResampleContext*)res->context;

	if(context&&context->state)
		speex_resampler_destroy(context->state);

	yang_free(res->context);
}



