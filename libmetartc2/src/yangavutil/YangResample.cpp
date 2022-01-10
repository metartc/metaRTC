/*
 * YangResample.cpp
 *
 *  Created on: 2020年9月10日
 *      Author: yang
 */

#include <yangavutil/audio/YangResample.h>


void YangResample::loadLib(){

	yang_speex_resampler_init=(SpeexResamplerState *(*)(spx_uint32_t nb_channels,spx_uint32_t in_rate,
			spx_uint32_t out_rate,int32_t quality,int32_t *err))m_lib.loadFunction("speex_resampler_init");
	yang_speex_resampler_process_int=(int32_t (*)(SpeexResamplerState *st,spx_uint32_t channel_index,const spx_int16_t *in,
	                                 spx_uint32_t *in_len,spx_int16_t *out,spx_uint32_t *out_len))m_lib.loadFunction("speex_resampler_process_int");
	yang_speex_resampler_process_interleaved_int=(int32_t (*)(SpeexResamplerState *st, const spx_int16_t *in,spx_uint32_t *in_len,
	                                             spx_int16_t *out,spx_uint32_t *out_len))m_lib.loadFunction("speex_resampler_process_interleaved_int");
	yang_speex_resampler_destroy=(void (*)(SpeexResamplerState *st))m_lib.loadFunction("speex_resampler_destroy");
}

void YangResample::unloadLib(){
	yang_speex_resampler_init=0;
	yang_speex_resampler_process_int=0;
	yang_speex_resampler_process_interleaved_int=0;
	yang_speex_resampler_destroy=0;
}
YangResample::YangResample() {
	// TODO Auto-generated constructor stub
	m_contextt=0;
	m_state=0;
	m_channel=2;
    m_in_sample=44100;
    m_out_sample=44100;
    m_in_len=0;
    m_out_len=0;
	unloadLib();
}

YangResample::~YangResample() {
	if(m_state) yang_speex_resampler_destroy(m_state);
	   m_state=0;
	   unloadLib();
	   	m_lib.unloadObject();
}

void YangResample::init(int32_t pchannel,int32_t pinsample,int32_t poutsample){
	if(m_contextt) return;
	m_lib.loadObject("libspeexdsp");
	loadLib();
	m_channel=pchannel;
	m_in_sample=pinsample;
	m_out_sample=poutsample;
	if(!m_state)    m_state = yang_speex_resampler_init(m_channel, m_in_sample, m_out_sample, 10, 0);
	//   speex_resampler_set_rate(m_state, 96000, 44100);
	   //speex_resampler_skip_zeros(m_state);


	m_contextt=1;
}

void YangResample::resample(const  short *pin,uint32_t  pinLen, short* pout,uint32_t*  poutLen){
    m_in_len=pinLen;
    //m_out_len=poutLen;
   // speex_resampler_process_interleaved_int
	if(m_channel==1){
        if(m_state) yang_speex_resampler_process_int(m_state, 0, pin, &m_in_len, pout, poutLen);
	}else{
        if(m_state) yang_speex_resampler_process_interleaved_int(m_state, pin, &m_in_len, pout, poutLen);
	}
}
