/*
 * YangResample.h
 *
 *  Created on: 2020年9月10日
 *      Author: yang
 */

#ifndef YANGAUDIOPROC_YANGRESAMPLE_H_
#define YANGAUDIOPROC_YANGRESAMPLE_H_
#include "speex/speex_resampler.h"
#include "yangutil/sys/YangLoadLib.h"

class YangResample {
public:
	YangResample();
	virtual ~YangResample();
        void resample(const short *pin,uint32_t  pinLen,short* pout,uint32_t  *poutLen);
	void init(int32_t pchannel,int32_t pinsample,int32_t poutsample);
	int32_t m_in_sample,m_out_sample;
	int32_t m_contextt;
	int32_t m_channel;
	SpeexResamplerState *m_state;

private:
        spx_uint32_t m_in_len;
        spx_uint32_t m_out_len;
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	SpeexResamplerState *(*yang_speex_resampler_init)(spx_uint32_t nb_channels,spx_uint32_t in_rate, spx_uint32_t out_rate,int32_t quality,int32_t *err);

	int32_t (*yang_speex_resampler_process_int)(SpeexResamplerState *st,spx_uint32_t channel_index,const spx_int16_t *in,
                                         spx_uint32_t *in_len,spx_int16_t *out,spx_uint32_t *out_len);
        int32_t (*yang_speex_resampler_process_interleaved_int)(SpeexResamplerState *st, const spx_int16_t *in,spx_uint32_t *in_len,
                                                     spx_int16_t *out,spx_uint32_t *out_len);
	void (*yang_speex_resampler_destroy)(SpeexResamplerState *st);
};

#endif /* YANGAUDIOPROC_YANGRESAMPLE_H_ */
