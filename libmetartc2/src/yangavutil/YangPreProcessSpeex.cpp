/*
 * YangPreProcessSpeex.cpp
 *
 *  Created on: 2020年9月29日
 *      Author: yang
 */

#include "YangPreProcessSpeex.h"

void YangPreProcessSpeex::loadLib(){

	yang_speex_preprocess_state_init=(SpeexPreprocessState *(*)(int32_t frame_size, int32_t sampling_rate))m_lib.loadFunction("speex_preprocess_state_init");;
	yang_speex_preprocess_ctl=(int32_t (*)(SpeexPreprocessState *st, int32_t request, void *ptr))m_lib.loadFunction("speex_preprocess_ctl");;
	yang_speex_preprocess_state_destroy=(void (*)(SpeexPreprocessState *st))m_lib.loadFunction("speex_preprocess_state_destroy");;
	yang_speex_preprocess_run=(int32_t (*)(SpeexPreprocessState *st, spx_int16_t *x))m_lib.loadFunction("speex_preprocess_run");;
}

void YangPreProcessSpeex::unloadLib(){
	yang_speex_preprocess_state_init=0;
	yang_speex_preprocess_ctl=0;
	yang_speex_preprocess_state_destroy=0;
	yang_speex_preprocess_run=0;
}
YangPreProcessSpeex::YangPreProcessSpeex() {
	m_bHasInit = false;
	m_preprocState = 0;
	unloadLib();
}

YangPreProcessSpeex::~YangPreProcessSpeex() {
	closePre();
	unloadLib();
	m_lib.unloadObject();
}

void YangPreProcessSpeex::init(int32_t pFrameSize, int32_t sampleRate, int32_t pchannel) {
	if (m_bHasInit)
		return;
	m_lib.loadObject("libspeexdsp");
	loadLib();
	//resetAec();
	m_frameSize = pFrameSize;
	m_sampleRate = sampleRate;
	m_channel = pchannel;
	int32_t frame_size = m_frameSize; //(m_frameSizeInMS * m_sampleRate * 1.0) / 1000;
	m_preprocState = yang_speex_preprocess_state_init(frame_size, m_sampleRate);

	//speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_ECHO_STATE,m_state);
//	int32_t i = 1;
	//speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_VAD, &i);
	int32_t noiseSuppress = -25;
	int32_t i = 1;
	yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_DENOISE, &i);
	yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS,
			&noiseSuppress);
	i = 0;
	yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_AGC, &i);
	i = sampleRate;
	yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);
	i = 0;
	yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_DEREVERB, &i);
	float f = .0;
	yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_DEREVERB_DECAY,
			&f);
	f = .0;
	yang_speex_preprocess_ctl(m_preprocState, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL,
			&f);

	m_bHasInit = true;
}

void YangPreProcessSpeex::closePre() {
//	speex_echo_state_destroy(m_state);
	if(m_preprocState) yang_speex_preprocess_state_destroy(m_preprocState);
	//m_state = 0;
	m_preprocState = 0;

}

void YangPreProcessSpeex::preprocess_run(short *pcm) {
	//speex_echo_cancellation(echo_state, ref_buf, echo_buf, e_buf);
	if(m_preprocState) yang_speex_preprocess_run(m_preprocState, pcm);
}

void YangPreProcessSpeex::state_reset() {
	//speex_preprocess_state_reset(m_preprocState);
}
