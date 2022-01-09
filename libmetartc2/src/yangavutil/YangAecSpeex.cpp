/*
 * YangAceSpeex.cpp
 *
 *  Created on: 2020年9月8日
 *      Author: yang
 */

#include "YangAecSpeex.h"

#include "stdio.h"
#ifndef NULL
#define NULL	((void *)0)
#endif


void YangAecSpeex::loadLib(){
	//yang_opus_encoder_create=(OpusEncoder *(*)(opus_int32_t Fs, int32_t channels, int32_t application,  int32_t *error))m_lib.loadFunction("");
	yang_speex_preprocess_state_init=(SpeexPreprocessState *(*)(int32_t frame_size, int32_t sampling_rate))m_lib.loadFunction("speex_preprocess_state_init");;
	yang_speex_preprocess_ctl=(int32_t (*)(SpeexPreprocessState *st, int32_t request, void *ptr))m_lib.loadFunction("speex_preprocess_ctl");;
	yang_speex_preprocess_state_destroy=(void (*)(SpeexPreprocessState *st))m_lib.loadFunction("speex_preprocess_state_destroy");;
	yang_speex_preprocess_run=(int32_t (*)(SpeexPreprocessState *st, spx_int16_t *x))m_lib.loadFunction("speex_preprocess_run");

	yang_speex_echo_state_init_mc=(SpeexEchoState *(*)(int32_t frame_size, int32_t filter_length, int32_t nb_mic, int32_t nb_speakers))m_lib.loadFunction("speex_echo_state_init_mc");
	yang_speex_echo_ctl=(int32_t (*)(SpeexEchoState *st, int32_t request, void *ptr))m_lib.loadFunction("speex_echo_ctl");
	yang_speex_echo_state_reset=(void (*)(SpeexEchoState *st))m_lib.loadFunction("speex_echo_state_reset");
	yang_speex_echo_playback=(void (*)(SpeexEchoState *st, const spx_int16_t *play))m_lib.loadFunction("speex_echo_playback");
	yang_speex_echo_capture=(void (*)(SpeexEchoState *st, const spx_int16_t *rec, spx_int16_t *out))m_lib.loadFunction("speex_echo_capture");
	yang_speex_echo_cancellation=(void (*)(SpeexEchoState *st, const spx_int16_t *rec, const spx_int16_t *play, spx_int16_t *out))m_lib.loadFunction("speex_echo_cancellation");
	yang_speex_echo_state_destroy=(void (*)(SpeexEchoState *st))m_lib.loadFunction("speex_echo_state_destroy");
}

void YangAecSpeex::unloadLib(){
	yang_speex_preprocess_state_init=NULL;
		yang_speex_preprocess_ctl=NULL;
		yang_speex_preprocess_state_destroy=NULL;
		yang_speex_preprocess_run=NULL;

		yang_speex_echo_state_init_mc=NULL;
		yang_speex_echo_ctl=NULL;
		yang_speex_echo_state_reset=NULL;
		yang_speex_echo_playback=NULL;
		yang_speex_echo_capture=NULL;
		yang_speex_echo_cancellation=NULL;
		yang_speex_echo_state_destroy=NULL;
}
YangAecSpeex::YangAecSpeex() {
	memset(&m_audioFrame,0,sizeof(YangFrame));
	m_bHasInit = false;
	m_state = NULL;
	m_preprocState = NULL;
	m_audioBuffer = NULL;
	unloadLib();
	//m_nFrameSize = 0;
	//m_nFilterLen = 0;
	//m_nSampleRate = 0;
}

YangAecSpeex::~YangAecSpeex() {
	closeAec();
	m_audioBuffer = NULL;
	unloadLib();
	m_lib.unloadObject();
}
void YangAecSpeex::setPlayBuffer(YangAudioBuffer *pab) {
	m_audioBuffer = pab;
}
void YangAecSpeex::init(int32_t pFrameSize, int32_t sampleRate, int32_t pchannel) {
	if (m_bHasInit)
		return;
	m_lib.loadObject("libspeexdsp");
	loadLib();
	resetAec();
	m_frameSize = pFrameSize;
	m_sampleRate = sampleRate;
	m_channel = pchannel;
	int32_t frame_size = m_frameSize;//(m_frameSizeInMS * m_sampleRate * 1.0) / 1000;
	int32_t filter_length = m_frameSize * m_echoPath;//20;//30;//m_echoPath;//(m_frameSizeInMS * m_sampleRate * 1.0) / 1000;
	//m_nFrameSize = frame_size;
	//m_nFilterLen = filter_length;
	//m_nSampleRate = sampleRate;
	//通常是m_nFrameSize 80 160 320
	//m_pState = speex_echo_state_init(m_nFrameSize, 10*m_nFrameSize);
	if(!m_state) m_state = yang_speex_echo_state_init_mc(frame_size, filter_length, m_channel,m_channel);
	//m_pState = aec->speex_echo_state_init_mc(m_nFrameSize, m_nFilterLen, 2, 2);
	//frame_size *= 2; // length read each time

	if(!m_preprocState) m_preprocState = yang_speex_preprocess_state_init(frame_size, m_sampleRate);

	yang_speex_echo_ctl(m_state, SPEEX_ECHO_SET_SAMPLING_RATE, &m_sampleRate);
	yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_ECHO_STATE,
			m_state);
	int32_t i = 1;
	//speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_VAD, &i);
	int32_t noiseSuppress = -25;
	 i = 1;
	yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_DENOISE,	&i);
	yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS,	&noiseSuppress);
	 i=0;
	 yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_AGC, &i);
	 i=sampleRate;
	 yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);
	 i=0;
	 yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_DEREVERB, &i);
	 float f=.0;
	 yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
	 f=.0;
	 yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);


	//printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>> init Aec frameSize=%d,filterLen=%d,sample=%d channel==%d",frame_size,filter_length,m_sampleRate,m_channel);
	//m_pfNoise = new float[m_nFrameSize + 1];
	m_bHasInit = true;
}

void YangAecSpeex::closeAec() {
	if(m_state) yang_speex_echo_state_destroy(m_state);
	if(m_preprocState) yang_speex_preprocess_state_destroy(m_preprocState);
	m_state = 0;
	m_preprocState = 0;

}
void YangAecSpeex::resetAec() {
	closeAec();

	m_bHasInit = false;
}
void YangAecSpeex::echo_state_reset() {
	if(m_state) yang_speex_echo_state_reset(m_state);
}
void YangAecSpeex::echo_playback(short *play) {
	//printf(",");
	if(m_state) yang_speex_echo_playback(m_state, play);
}
void YangAecSpeex::echo_cancellation(const short *rec, const short *play,
		short *out) {
	if(m_state) yang_speex_echo_cancellation(m_state, rec, play, out);
}

void YangAecSpeex::put_echo_playback(uint8_t *play,int32_t plen) {
	//printf(",");
	if (m_audioBuffer){
		m_audioFrame.payload=play;
		m_audioFrame.nb=plen;
		m_audioBuffer->putAudio(&m_audioFrame);
	}

	//speex_echo_playback(m_state,play);
}
void YangAecSpeex::echo_capture(short *rec, short *out) {
	if(m_state) yang_speex_echo_capture(m_state, rec, out);
}
void YangAecSpeex::preprocess_run(short *pcm) {
	//speex_echo_cancellation(echo_state, ref_buf, echo_buf, e_buf);
	if(m_preprocState) yang_speex_preprocess_run(m_preprocState, pcm);
}
