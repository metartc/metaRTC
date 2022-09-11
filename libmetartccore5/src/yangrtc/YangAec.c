//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangavutil/audio/YangRtcAec.h>
#include <yangavutil/audio/YangAudioUtil.h>
#include <yangutil/yangtype.h>
#include <string.h>
#include <stdio.h>
#include <yangrtc/YangMetaConnection.h>
typedef struct{
	int32_t inSample;
	int32_t outSample;
	int32_t inChannel;
	int32_t outChannel;
	YangRtcAec aec;
	YangAudioResample inRes;
	YangAudioResample outRes;
	YangFrame audioFrame;
	uint8_t* buf;
}YangAecContext;

void yang_ma_init(void* pcontext,int32_t sample,int32_t channel,int32_t echopath){
	if(pcontext==NULL) return;
	YangAecContext* aec=(YangAecContext*)pcontext;
	aec->inSample=sample;
	aec->inChannel=channel;
	aec->outSample=16000;
	aec->outChannel=1;
	aec->inRes.init(aec->inRes.context,aec->inSample,aec->inChannel,aec->outSample,aec->outChannel,20);
	aec->outRes.init(aec->outRes.context,aec->outSample,aec->outChannel,aec->inSample,aec->inChannel,20);
    aec->aec.init(aec->aec.session,aec->outSample,aec->outChannel,aec->outSample/50,echopath);
	if(aec->buf==NULL) aec->buf=(uint8_t*)malloc(aec->outRes.context->inBytes);
}
void yang_ma_close(void* pcontext){
	if(pcontext==NULL) return;
	YangAecContext* aec=(YangAecContext*)pcontext;

	aec->aec.closeAec(aec->aec.session);
}

void yang_ma_echo_capture(void* pcontext,short *rec, short *out){
	if(pcontext==NULL) return;
	YangAecContext* aec=(YangAecContext*)pcontext;
	aec->audioFrame.payload=(uint8_t*)rec;
	aec->audioFrame.nb=aec->inRes.context->inBytes;
	aec->inRes.resample(aec->inRes.context,&aec->audioFrame);
	aec->aec.echoCapture(aec->aec.session,(short*)aec->audioFrame.payload,(short*)aec->buf);
	aec->audioFrame.payload=aec->buf;
	aec->audioFrame.nb=aec->outRes.context->inBytes;
	aec->outRes.resample(aec->outRes.context,&aec->audioFrame);
	memcpy((char*)out,aec->audioFrame.payload,aec->outRes.context->outBytes);

}
void yang_ma_preprocess_run(void* pcontext,short *pcm){
	if(pcontext==NULL) return;
	YangAecContext* aec=(YangAecContext*)pcontext;
	aec->aec.preprocessRun(aec->aec.session,pcm);
}
void yang_ma_echo_state_reset(void* pcontext){
	if(pcontext==NULL) return;
	YangAecContext* aec=(YangAecContext*)pcontext;
	aec->aec.echoStateReset(aec->aec.session);
}
void yang_ma_echo_playback(void* pcontext,short *play){
	if(pcontext==NULL) return;
	YangAecContext* aec=(YangAecContext*)pcontext;
	aec->audioFrame.payload=(uint8_t*)play;
	aec->audioFrame.nb=aec->inRes.context->inBytes;
	aec->inRes.resample(aec->inRes.context,&aec->audioFrame);
	aec->aec.echoPlayback(aec->aec.session,(short*)aec->audioFrame.payload);
}
void yang_ma_echo_cancellation(void* pcontext,const short *rec, const short *play,
		short *out){
	if(pcontext==NULL) return;
	YangAecContext* aec=(YangAecContext*)pcontext;
	aec->aec.echoCancellation(aec->aec.session,rec,play,out);
}

void yang_create_aec(YangAec* aec){
	if(aec==NULL) return;
	if(aec->session==NULL) {
		aec->session=(YangAecContext*)calloc(sizeof(YangAecContext),1);
		YangAecContext* context=(YangAecContext*)aec->session;
		yang_create_rtcaec(&context->aec);
		yang_create_audioresample(&context->inRes);
		yang_create_audioresample(&context->outRes);
	}
	aec->init=yang_ma_init;
	aec->closeAec=yang_ma_close;
	aec->echoCancellation=yang_ma_echo_cancellation;
	aec->echoCapture=yang_ma_echo_capture;
	aec->echoPlayback=yang_ma_echo_playback;
	aec->echoStateReset=yang_ma_echo_state_reset;
	aec->preprocessRun=yang_ma_preprocess_run;

}
void yang_destroy_aec(YangAec* aec){
	if(aec==NULL) return;
	yang_destroy_rtcaec(&((YangAecContext*)aec->session)->aec);
	yang_destroy_audioresample(&((YangAecContext*)aec->session)->inRes);
	yang_destroy_audioresample(&((YangAecContext*)aec->session)->outRes);
	yang_free(aec->session);

}
