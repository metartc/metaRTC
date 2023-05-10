//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangavutil/audio/YangAudioUtil.h>
#include <yangutil/sys/YangLog.h>
#include <math.h>


void yang_audiores_initParam(YangAudioResampleContext* context) {
	if(context==NULL) return;

	context->inFrames = context->inSample * context->intervalTime / 1000;
	context->outFrames = context->outSample * context->intervalTime / 1000;
	context->inBytes = context->inFrames * 2 * context->inChannel;
	context->outBytes = context->outFrames * 2 * context->outChannel;
    context->sampleTrans = (context->inSample == context->outSample ? 0 : 1);
    context->channelTrans = (context->inChannel == context->outChannel ? 0 : 1);
    context->isTrans = (context->sampleTrans || context->channelTrans) ? 1 : 0;
}
void yang_audiores_initIn(YangAudioResampleContext* context,int32_t psample, int32_t pchannel) {
	if(context==NULL) return;

	context->inSample = psample;
	context->inChannel = pchannel;
    context->sampleTrans = (context->inSample == context->outSample ? 0 : 1);
    context->channelTrans = (context->inChannel == context->outChannel ? 0 : 1);
	context->inFrames = context->inSample * context->intervalTime / 1000;
	context->inBytes = context->inFrames * 2 * context->inChannel;
    context->isTrans = (context->sampleTrans || context->channelTrans) ? 1 : 0;
}
void yang_audiores_initOut(YangAudioResampleContext* context,int32_t psample, int32_t pchannel) {
	if(context==NULL) return;

	context->outSample = psample;
	context->outChannel = pchannel;
    context->sampleTrans = (context->inSample == context->outSample ? 0 : 1);
    context->channelTrans = (context->inChannel == context->outChannel ? 0 : 1);

	context->outFrames = context->outSample * context->intervalTime / 1000;
	context->outBytes = context->outFrames * 2 * context->outChannel;
    context->isTrans = (context->sampleTrans || context->channelTrans) ? 1 : 0;
}
void yang_audiores_init(YangAudioResampleContext* context,int32_t insample, int32_t inchannel, int32_t outsample,
		int32_t outchannel, int32_t ms) {
	if(context==NULL) return;

	context->inSample = insample;
	context->inChannel = inchannel;
	context->outSample = outsample;
	context->outChannel = outchannel;
	context->intervalTime = ms;
	yang_audiores_initParam(context);
}
void yang_audiores_initTrans(YangAudioResampleContext* context) {
	if(context==NULL) return;


	if(!context->isTrans) return;
		if(context->channelTrans){
			if (context->inBuf == NULL) 	context->inBuf =(uint8_t*) yang_calloc( context->inFrames<<2,1);
		}
		if(context->sampleTrans){
				if (context->outBuf == NULL)	context->outBuf = (uint8_t*) yang_calloc( context->outFrames<<2,1);

                context->res.init(context->res.context,context->outChannel, context->inSample, context->outSample);

		}

}




int32_t yang_audiores_resample(YangAudioResampleContext* context,YangFrame *audioFrame) {
    if(context==NULL) return 1;

    if (!context->isTrans||!audioFrame||!audioFrame->payload)		return Yang_Ok;

	if (!context->isInitTrans){
		yang_audiores_initTrans(context);
        context->isInitTrans = 1;
	}
	uint8_t *buf = audioFrame->payload;
	uint8_t *buf_in=buf;
	if (context->channelTrans) {
		if (context->inChannel == 1)
			MonoToStereo((short*) buf, (short*) context->inBuf, context->inFrames);
		else
			StereoToMono((short*) buf, (short*) context->inBuf, context->inFrames);


        buf_in=context->inBuf;
	}
	if (context->sampleTrans) {
		uint32_t outlen = 6400;
        context->res.resample(context->res.context,(short*) (buf_in), context->inFrames,(short*) context->outBuf, &outlen);
  		audioFrame->payload = context->outBuf;
	} else {
		audioFrame->payload = context->inBuf;
	}

	audioFrame->nb = context->outBytes;
    buf_in=NULL;
    buf=NULL;
	return Yang_Ok;
}



void yang_create_audioresample(YangAudioResample* res){
	if(res==NULL) return;
	res->context=(YangAudioResampleContext*)yang_calloc(sizeof(YangAudioResampleContext),1);
	res->context->intervalTime = 20;
	res->context->inSample = 48000;
	res->context->outSample = 48000;
	res->context->inChannel = 2;
	res->context->outChannel = 2;
	res->context->inBuf = NULL;
	res->context->outBuf = NULL;
    res->context->sampleTrans = 0;
    res->context->channelTrans = 0;
    res->context->isInitTrans = 0;
    res->context->isTrans = 1;
	yang_audiores_initParam(res->context);
	res->init=yang_audiores_init;
	res->initIn=yang_audiores_initIn;
	res->initOut=yang_audiores_initOut;
	res->resample=yang_audiores_resample;
	yang_create_resample(&res->context->res);
}
void yang_destroy_audioresample(YangAudioResample* res){
	if(res==NULL) return;
	yang_destroy_resample(&res->context->res);
	yang_free(res->context->inBuf);
	yang_free(res->context->outBuf);
	yang_free(res->context);
}
int32_t MonoToStereo(int16_t *pData, int16_t *dData,
		int32_t samples_per_channel) {


	for (int32_t i = 0; i < samples_per_channel; i++) {
		dData[2 * i] = pData[i];
		dData[2 * i + 1] = pData[i];
	}
	return samples_per_channel * 2;
}

int32_t StereoToMono(const int16_t *src_audio, int16_t *dst_audio,
		int32_t samples_per_channel) {
	for (int i = 0; i < samples_per_channel; i++) {
		dst_audio[i] = ((int32_t)(src_audio[2 * i])
				+ src_audio[2 * i + 1]) >> 1;
	}
	return Yang_Ok;
}

