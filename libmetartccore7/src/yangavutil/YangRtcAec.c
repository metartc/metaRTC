//
// Copyright (c) 2019-2025 yanggaofeng
//

#include <yangavutil/audio/YangRtcAec.h>

#include <yangutil/sys/YangLog.h>

#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>

typedef struct YangRtcAec{
	int32_t channel;
	int32_t frameSize;
	int32_t sampleRate;
	int32_t echoPath;
	SpeexEchoState *state;
	SpeexPreprocessState *preprocState;
}YangRtcAecContext;

static void yang_init(void* context, int32_t sampleRate, int32_t pchannel,int32_t pframeSize,int32_t echopath){
	float f;
	int32_t frame_size, filter_length,noiseSuppress,i;
	YangRtcAecContext* aec=(YangRtcAecContext*)context;

	if(context==NULL)
		return;

	aec->frameSize = pframeSize;
	aec->sampleRate = sampleRate;
	aec->channel = pchannel;
	aec->echoPath=echopath;

	frame_size = aec->frameSize;//(aec->frameSizeInMS * aec->sampleRate * 1.0) / 1000;
	filter_length = aec->frameSize * aec->echoPath;//20;//30;//aec->echoPath;//(aec->frameSizeInMS * aec->sampleRate * 1.0) / 1000;

	//通常是aec->nFrameSize 80 160 320
	//aec->pState = speex_echo_state_init(aec->nFrameSize, 10*aec->nFrameSize);
	if(!aec->state)
		aec->state = speex_echo_state_init_mc(frame_size, filter_length, aec->channel,aec->channel);
	//aec->pState = aec->speex_echo_state_init_mc(aec->nFrameSize, aec->nFilterLen, 2, 2);
	//frame_size *= 2; // length read each time

	if(!aec->preprocState)
		aec->preprocState = speex_preprocess_state_init(frame_size, aec->sampleRate);

	speex_echo_ctl(aec->state, SPEEX_ECHO_SET_SAMPLING_RATE, &aec->sampleRate);
	speex_preprocess_ctl(aec->preprocState, SPEEX_PREPROCESS_SET_ECHO_STATE,aec->state);
	i = 1;
	//speex_preprocess_ctl(aec->preprocState, SPEEX_PREPROCESS_SET_VAD, &i);
	noiseSuppress = -25;
	i = 1;
	speex_preprocess_ctl(aec->preprocState, SPEEX_PREPROCESS_SET_DENOISE,	&i);
	speex_preprocess_ctl(aec->preprocState, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS,	&noiseSuppress);
	i=0;
	speex_preprocess_ctl(aec->preprocState, SPEEX_PREPROCESS_SET_AGC, &i);
	i=sampleRate;
	speex_preprocess_ctl(aec->preprocState, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);
	i=0;
	speex_preprocess_ctl(aec->preprocState, SPEEX_PREPROCESS_SET_DEREVERB, &i);
	f=.0;
	speex_preprocess_ctl(aec->preprocState, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
	f=.0;
	speex_preprocess_ctl(aec->preprocState, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);
}

static void yang_close(void* context){
	YangRtcAecContext* aec=(YangRtcAecContext*)context;

	if(context==NULL)
		return;

	if(aec->state)
		speex_echo_state_destroy(aec->state);

	if(aec->preprocState)
		speex_preprocess_state_destroy(aec->preprocState);

	aec->state = NULL;
	aec->preprocState = NULL;
}

static void yang_echo_capture(void* context,short *rec, short *out) {
	YangRtcAecContext* aec=(YangRtcAecContext*)context;

	if(context==NULL)
		return;

	if(aec->state)
		speex_echo_capture(aec->state, rec, out);
}

static void yang_preprocess_run(void* context,short *pcm) {
	YangRtcAecContext* aec=(YangRtcAecContext*)context;

	if(context==NULL)
		return;

	if(aec->preprocState)
		speex_preprocess_run(aec->preprocState, pcm);
}

static void yang_echo_state_reset(void* context) {
	YangRtcAecContext* aec=(YangRtcAecContext*)context;

	if(context==NULL)
		return;

	if(aec->state)
		speex_echo_state_reset(aec->state);
}

static void yang_echo_playback(void* context,short *play) {
	YangRtcAecContext* aec=(YangRtcAecContext*)context;

	if(context==NULL)
		return;

	if(aec->state)
		speex_echo_playback(aec->state, play);
}

static void yang_echo_cancellation(void* context,const short *rec, const short *play,
		short *out) {
	YangRtcAecContext* aec=(YangRtcAecContext*)context;

	if(context==NULL)
		return;

	if(aec->state)
		speex_echo_cancellation(aec->state, rec, play, out);
}

void yang_create_rtcaec(YangRtcAec* aec){
	if(aec==NULL)
		return;

	aec->session=(YangRtcAecContext*)yang_calloc(sizeof(YangRtcAecContext),1);
	aec->init=yang_init;
	aec->closeAec=yang_close;
	aec->echoCancellation=yang_echo_cancellation;
	aec->echoCapture=yang_echo_capture;
	aec->echoPlayback=yang_echo_playback;
	aec->echoStateReset=yang_echo_state_reset;
	aec->preprocessRun=yang_preprocess_run;
}

void yang_destroy_rtcaec(YangRtcAec* aec){
	if(aec==NULL)
		return;

	yang_close((YangRtcAecContext*)aec->session);
	yang_free(aec->session);
}

