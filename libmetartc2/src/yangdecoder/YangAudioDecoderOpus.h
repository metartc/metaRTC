/*
 * YangAudioDecoderOpus.h
 *
 *  Created on: 2019年9月29日
 *      Author: yang
 */

#ifndef YANGDECODER_SRC_YANGAUDIODECODEROPUS_H_
#define YANGDECODER_SRC_YANGAUDIODECODEROPUS_H_
#include <yangutil/sys/YangLoadLib.h>
#include "opus/opus.h"
#include "yangdecoder/YangAudioDecoder.h"

class YangAudioDecoderOpus:public YangAudioDecoder {
public:
	YangAudioDecoderOpus(YangAudioParam *pcontext);
	virtual ~YangAudioDecoderOpus();
	void init();
	int32_t decode(YangFrame* pframe,YangDecoderCallback* pcallback);
	private:

		void closedec();
		int32_t isConvert;
		OpusDecoder *m_decoder;


		int32_t m_isMono;
		int32_t ret;
		int32_t m_sample;
		int32_t m_channel;
		short *m_out;
		uint8_t *m_output;

		YangLoadLib m_lib;
		void loadLib();
		void unloadLib();
		 OpusDecoder *(*yang_opus_decoder_create)( opus_int32 Fs,  int32_t channels,  int32_t *error);
		 int32_t (*yang_opus_decoder_init)(OpusDecoder *st,opus_int32 Fs, int32_t channels ) ;
		 int32_t (*yang_opus_decode)(OpusDecoder *st,const uint8_t *data, opus_int32 len,opus_int16 *pcm,
		     int32_t frame_size, int32_t decode_fec);
		 int32_t (*yang_opus_decoder_ctl)(OpusDecoder *st, int32_t request, ...);
		 void (*yang_opus_decoder_destroy)(OpusDecoder *st);
		 const char *(*yang_opus_strerror)(int32_t error);
};

#endif /* YANGDECODER_SRC_YANGAUDIODECODEROPUS_H_ */
